/************************************************************************
#      Copyright (C) 2010-2020 VIAZIJING TECHNOLOGY CO., LTD.
#      All rights reserved.
#***********************************************************************/

#include "os.h"
#include "misc.h"
#include "AmfByteStream.h"
#include "librtmppublish.h"


#define PUBLISH_THRESHOLD		6
#define AMF_HEADER_SIZE			16
#define OVERFLOW_THRESHOLD		100
 list<CLibRtmpPublishBase*>  CLibRtmpPublish::s_EntityList;

CLibRtmpPublishBase* CreateLibRtmpPublish()
{
	return (CLibRtmpPublishBase*)new CLibRtmpPublish;
}

CLibRtmpPublishBase* CLibRtmpPublish::GetEntityByID(int nID)
{
    list<CLibRtmpPublishBase*>::iterator it2 = s_EntityList.begin();
    for(;it2!=s_EntityList.end();it2++)
    {
        CLibRtmpPublishBase *pEntity = *it2;
        if(pEntity&&pEntity->GetStreamID()==nID)
        {
            return pEntity ;
        }

    }
    return NULL ;
}

void LibRtmpPublishThreadFn(void* pArg)
{
	const char* name = "LibRtmpPublish";
	prctl(PR_SET_NAME, (unsigned long)name);

	CLibRtmpPublish* pLibRtmpPublish = (CLibRtmpPublish*)pArg;
	pLibRtmpPublish->MainThreadFn();
}


/*
 * class CLibRtmpPublish
 */
CLibRtmpPublish::CLibRtmpPublish()
{
	m_pLogger = new CLogger("LIBRTMPP");

	m_pLogger->info("LIBRTMPPUBLISH: %08X, Create CLibRtmpPublish\n", this);

	m_fnLibRtmpPublishCallback = NULL;

	m_iRtmpSkt = -1;
	m_bIsConnected = false;
	m_bIsFailed = false;
	m_bIsStart = false;
	m_timecodeZero = 0xFFFFFFFF;

	m_lenAACDecoderSpecificInfo = 0;
	m_bSentAACDecoderSpecificInfo = false;
	m_lenAVCSps = 0;
	m_lenAVCPps = 0;
	m_bSentAVCDecoderSpecificInfo = false;

	m_iSampleFreqIndex = 4;		// 44100 HZ
	m_iChannel = 2;				// Stereo

	m_pRtmp = NULL;

	m_pFirst = NULL;
	m_pLast = NULL;
	m_iBufNum = 0;
	m_iMallocSize = 0;

	m_iTotalSendBytes = 0;
	m_iCurrentTimecode = 0;

	m_bInit = false;
	m_bExit = false;
	m_thread = (thread_t)-1; 
    s_EntityList.push_back(this);
}

CLibRtmpPublish::~CLibRtmpPublish()
{
    s_EntityList.remove(this);
	Stop();

	if (0 != m_iBufNum || 0 != m_iMallocSize)
		m_pLogger->warn("LIBRTMPPUBLISH: %08X, WARNING, memory leaks, bufNum %d, %d bytes\n", this, m_iBufNum, m_iMallocSize);

	m_pLogger->info("LIBRTMPPUBLISH: %08X, Release CLibRtmpPublish\n", this);
	delete m_pLogger;
	m_pLogger = NULL;

}

void CLibRtmpPublish::SetOnLibRtmpPublishCallback(OnLibRtmpPublishCallback callback)
{
	m_fnLibRtmpPublishCallback = callback;
}

bool CLibRtmpPublish::Start()
{
	if (m_bInit)
		return false;

	Lock();

	m_bExit = false;

	bool retStart = true;

	int ret = pthread_create(&m_thread, NULL, (void* (*)(void*))LibRtmpPublishThreadFn, (void*)this);
	if (0 != ret)
	{
		m_pLogger->error("LIBRTMPPUBLISH: %08X, create LibRtmpPublishThreadFn failed, errno %d\n", this, ret);
		m_thread = (thread_t)-1;
		retStart = false;
	}

	while ((thread_t)-1 != m_thread && !m_bInit)
	{
		SleepUs(10000);
	}

	Unlock();

	return retStart;
}

bool CLibRtmpPublish::Stop()
{
	m_fnLibRtmpPublishCallback = NULL;

	m_pLogger->debug("LIBRTMPPUBLISH: %08X, Stop() - Step 1\n", this);

	if (!m_bInit)
		return true;

	m_pLogger->debug("LIBRTMPPUBLISH: %08X, Stop() - Step 2\n", this);

	m_bQuit = true;

	RtmpPBuf* pBuf = (RtmpPBuf*)malloc(sizeof(RtmpPBuf));

	if (NULL == pBuf)
	{
		m_pLogger->debug("LIBRTMPPUBLISH: %08X, Stop() - malloc failed\n", this);
	}
	else
	{
		memset(pBuf, 0, sizeof(RtmpPBuf));
		pBuf->cmd = RTMPP_CMD_QUIT;
		pBuf->timecode = 0xFFFFFFFF;
		pBuf->len = 0;

		AddBuf(pBuf);
	}

	SleepUs(10000);

	if (m_iRtmpSkt > 0)
	{
		shutdown(m_iRtmpSkt, SHUT_RDWR);
	}

	m_bExit = true;

	SleepUs(10000);

	m_pLogger->debug("LIBRTMPPUBLISH: %08X, Stop() - Step 3\n", this);

	while (m_bInit)
	{
		SleepUs(10000);
	}

	m_pLogger->debug("LIBRTMPPUBLISH: %08X, Stop() - Step 4\n", this);

	if ((thread_t)-1 != m_thread)
	{
		pthread_join(m_thread, NULL);
	}

	m_thread = (thread_t)-1;

	m_pLogger->debug("LIBRTMPPUBLISH: %08X, Stop() - Step End\n", this);

	return true;
}

bool CLibRtmpPublish::IsConnected()
{
	return m_bIsConnected;
}

bool CLibRtmpPublish::IsFailed()
{
	return m_bIsFailed;
}

void CLibRtmpPublish::Connect(const char* szRtmpURL)	// for example : "rtmp://0.0.0.0/live/liveStream"
{
	m_bIsConnected = false;
	m_bIsFailed = false;
	m_bIsStart = false;
	m_timecodeZero = 0xFFFFFFFF;

	if (NULL == szRtmpURL)
		return;

	memset(m_szRtmpURL, 0, sizeof(m_szRtmpURL));
	strcpy(m_szRtmpURL, szRtmpURL);

	RtmpPBuf* pBuf = (RtmpPBuf*)malloc(sizeof(RtmpPBuf));

	if (NULL == pBuf)
	{
		m_pLogger->debug("LIBRTMPPUBLISH: %08X, Connect() - malloc failed\n", this);
	}
	else
	{
		memset(pBuf, 0, sizeof(RtmpPBuf));
		pBuf->cmd = RTMPP_CMD_CONNECT;
		pBuf->timecode = 0;
		pBuf->len = 0;

		AddBuf(pBuf);
	}
}

bool CLibRtmpPublish::SendAAC(const char* pData, int sizeOfData, uint32 timecode)
{
	//m_pLogger->debug("LIBRTMPPUBLISH: %08X, SendAAC(%d, %u)\n", this, sizeOfData, timecode);

	if (m_bIsFailed)
		return false;

	if (0 == m_lenAACDecoderSpecificInfo)
	{
		if (2 != sizeOfData)
		{
			if (0) m_pLogger->error(
				"LIBRTMPPUBLISH: %08X, WARNING !!! AAC Decoder Specific Info invalid, %d bytes, %02X, %02X, %02X, %02X\n",
				this,
				sizeOfData,
				(unsigned char)pData[0],
				(unsigned char)pData[1],
				(unsigned char)pData[2],
				(unsigned char)pData[3]
			);

			return false;
		}

		memcpy(m_bufAACDecoderSpecificInfo, pData, sizeOfData);
		m_lenAACDecoderSpecificInfo = sizeOfData;

		int iAACDecoderSpecificInfo = ((int)((unsigned char)m_bufAACDecoderSpecificInfo[0]) << 8) | ((int)((unsigned char)m_bufAACDecoderSpecificInfo[1]));

		m_iSampleFreqIndex = (iAACDecoderSpecificInfo & 0x780) >> 7;
		m_iChannel = (iAACDecoderSpecificInfo & 0x78) >> 3;

		m_pLogger->debug("LIBRTMPPUBLISH: %08X, AAC Decoder Specific Info ready, sampleFreqIndex %d, channel %d\n", this, m_iSampleFreqIndex, m_iChannel);

		if (0xFFFFFFFF == m_timecodeZero)
		{
			m_timecodeZero = timecode;

			m_iTotalSendBytes = 0;
			m_iCurrentTimecode = 0;
		}

		if (m_bIsConnected && m_bIsStart && !m_bSentAACDecoderSpecificInfo)
			SendAACSequenceHeaderPacket();

		return true;
	}

	if (!m_bIsConnected || !m_bIsStart)
		return true;

	if (0xFFFFFFFF == m_timecodeZero)
		return true;

	if (sizeOfData <= 3)
		return true;

	RtmpPBuf* pBuf = (RtmpPBuf*)malloc(sizeof(RtmpPBuf));

	if (NULL == pBuf)
	{
		m_pLogger->debug("LIBRTMPPUBLISH: %08X, SendAAC() - malloc failed\n", this);
		return false;
	}

	memset(pBuf, 0, sizeof(RtmpPBuf));
	pBuf->cmd = RTMPP_CMD_AAC;
	pBuf->timecode = timecode;
	pBuf->len = sizeOfData;

	if (pBuf->len+AMF_HEADER_SIZE > RTMPP_BUF_SIZE)		// we reserve 16 bytes for AMF header
	{
		pBuf->pLargeData = (char*)malloc(pBuf->len+AMF_HEADER_SIZE);

		if (NULL == pBuf->pLargeData)
		{
			FreeBuf(pBuf);
			m_pLogger->debug("LIBRTMPPUBLISH: %08X, SendAAC() - malloc large data failed\n", this);
			return false;
		}

		memcpy(pBuf->pLargeData+AMF_HEADER_SIZE, pData, pBuf->len);
	}
	else
	{
		memcpy(pBuf->data+AMF_HEADER_SIZE, pData, pBuf->len);
	}

	AddBuf(pBuf);

	m_iTotalSendBytes += pBuf->len;
	m_iCurrentTimecode = timecode-m_timecodeZero;

	return true;
}

bool CLibRtmpPublish::SendAVC(const char* pData, int sizeOfData, uint32 timecode)
{
	int naluType = (int)((unsigned char)pData[0] & 0x1F);

	//m_pLogger->debug("LIBRTMPPUBLISH: %08X, SendAVC(%d, %u), naluType %d\n", this, sizeOfData, timecode, naluType);

	if (m_bIsFailed)
		return false;

	if (NULL == m_pAvcBuf)
	{
		m_pAvcBuf = (RtmpPBuf*)malloc(sizeof(RtmpPBuf));

		if (NULL == m_pAvcBuf)
		{
			m_pLogger->debug("LIBRTMPPUBLISH: %08X, SendAVC() - malloc failed\n", this);
			return false;
		}

		memset(m_pAvcBuf, 0, sizeof(RtmpPBuf));
		m_pAvcBuf->cmd = RTMPP_CMD_AVC;
		m_pAvcBuf->timecode = timecode;
		m_pAvcBuf->len = 0;
	}

	if (7 == naluType || 1 == naluType)
	{
		if (m_pAvcBuf->pLargeData)
		{
			free(m_pAvcBuf->pLargeData);
			m_pAvcBuf->pLargeData = NULL;
		}

		m_pAvcBuf->len = 0;
	}

	char* newBuf = m_pAvcBuf->data+AMF_HEADER_SIZE+m_pAvcBuf->len;

	if (m_pAvcBuf->len+AMF_HEADER_SIZE+4+sizeOfData > RTMPP_BUF_SIZE)		// we reserve 16 bytes for AMF header
	{
		char* backBuf = m_pAvcBuf->pLargeData;
		m_pAvcBuf->pLargeData = (char*)malloc(m_pAvcBuf->len+AMF_HEADER_SIZE+4+sizeOfData);

		if (NULL == m_pAvcBuf->pLargeData)
		{
			m_pAvcBuf->pLargeData = backBuf;
			m_pLogger->debug("LIBRTMPPUBLISH: %08X, SendAAC() - malloc large data failed\n", this);
			return false;
		}

		if (backBuf)
		{
			memcpy(m_pAvcBuf->pLargeData+AMF_HEADER_SIZE, backBuf+AMF_HEADER_SIZE, m_pAvcBuf->len);
			free(backBuf);
		}
		else
		{
			memcpy(m_pAvcBuf->pLargeData+AMF_HEADER_SIZE, m_pAvcBuf->data+AMF_HEADER_SIZE, m_pAvcBuf->len);
		}

		newBuf = m_pAvcBuf->pLargeData+AMF_HEADER_SIZE+m_pAvcBuf->len;
	}

	*((int*)(newBuf)) = htonl(sizeOfData);
	m_pAvcBuf->len += 4;
	memcpy(newBuf+4, pData, sizeOfData);
	m_pAvcBuf->len += sizeOfData;

	if (0 == m_lenAVCSps && 7 == naluType)
	{
		if (sizeOfData <= (int)sizeof(m_bufAVCSps))
		{
			memcpy(m_bufAVCSps, pData, sizeOfData);
			m_lenAVCSps = sizeOfData;

			if (m_lenAVCSps > 0 && m_lenAVCPps > 0)
			{
				if (0xFFFFFFFF == m_timecodeZero)
				{
					m_timecodeZero = timecode;

					m_iTotalSendBytes = 0;
					m_iCurrentTimecode = 0;
				}
			}
		}

		return true;
	}

	if (0 == m_lenAVCPps && 8 == naluType)
	{
		if (sizeOfData <= (int)sizeof(m_bufAVCPps))
		{
			memcpy(m_bufAVCPps, pData, sizeOfData);
			m_lenAVCPps = sizeOfData;

			m_pLogger->debug("LIBRTMPPUBLISH: %08X, AVC Decoder Specific Info ready\n", this);

			if (m_lenAVCSps > 0 && m_lenAVCPps > 0)
			{
				if (0xFFFFFFFF == m_timecodeZero)
				{
					m_timecodeZero = timecode;

					m_iTotalSendBytes = 0;
					m_iCurrentTimecode = 0;
				}

				if (m_bIsConnected && m_bIsStart && !m_bSentAVCDecoderSpecificInfo)
					SendAVCSequenceHeaderPacket();
			}
		}

		return true;
	}

	if (!m_bIsConnected || !m_bIsStart)
		return true;

	if (0 == m_lenAVCSps || 0 == m_lenAVCPps)
		return true;

	if (0xFFFFFFFF == m_timecodeZero)
		return true;

	if (5 == naluType || 1 == naluType)
	{
		//m_pLogger->debug("LIBRTMPPUBLISH: %08X, AddBuf(), %d bytes\n", this, m_pAvcBuf->len);
		AddBuf(m_pAvcBuf);

		m_iTotalSendBytes += m_pAvcBuf->len;
		m_iCurrentTimecode = timecode-m_timecodeZero;

		m_pAvcBuf = NULL;
	}

	return true;
}

void CLibRtmpPublish::Lock()
{
	m_mutex.Lock();
}

void CLibRtmpPublish::Unlock()
{
	m_mutex.Unlock();
}

void CLibRtmpPublish::AddBuf(RtmpPBuf* pBuf)
{
	if (NULL == pBuf)
		return;

	if (m_iBufNum > OVERFLOW_THRESHOLD)
	{
		m_pLogger->error(
			"LIBRTMPPUBLISH: %08X, AddBuf() overflow, bufNum %d, buf(cmd %d, timecode %u, len %d)\n",
			this,
			m_iBufNum,
			pBuf->cmd,
			pBuf->timecode,
			pBuf->len
		);

		FreeBuf(pBuf);
		return;
	}

	Lock();

	pBuf->pPrev = NULL;
	pBuf->pNext = NULL;

	if (m_pLast)
	{
		RtmpPBuf* pHere = m_pLast;

		while (pHere)
		{
			if (pBuf->timecode >= pHere->timecode)
				break;

			pHere = (RtmpPBuf*)(pHere->pPrev);
		}

		if (pHere)
		{
			if (pHere->pNext)
			{
				((RtmpPBuf*)(pHere->pNext))->pPrev = pBuf;
				pBuf->pNext = pHere->pNext;
				pBuf->pPrev = pHere;
				pHere->pNext = pBuf;
			}
			else
			{
				pHere->pNext = pBuf;
				pBuf->pPrev = pHere;
				m_pLast = pBuf;
			}
		}
		else
		{
			pBuf->pNext = m_pFirst;
			m_pFirst->pPrev = pBuf;
			m_pFirst = pBuf;
		}
	}
	else
	{
		m_pFirst = pBuf;
		m_pLast = pBuf;
	}

	m_iBufNum++;

	if (pBuf->pLargeData)
		m_iMallocSize += pBuf->len;
	m_iMallocSize += sizeof(RtmpPBuf);

	Unlock();
}

RtmpPBuf* CLibRtmpPublish::GetBuf()
{
	RtmpPBuf* pBuf = NULL;

	Lock();

	if (m_bIsConnected && !m_bQuit && m_iBufNum < PUBLISH_THRESHOLD)
	{
		Unlock();
		return NULL;
	}

	if (m_pFirst)
	{
		pBuf = m_pFirst;
		m_pFirst = (RtmpPBuf*)m_pFirst->pNext;

		if (m_pFirst)
			m_pFirst->pPrev = NULL;
		else
			m_pLast = NULL;

		m_iBufNum--;

		if (pBuf->pLargeData)
			m_iMallocSize -= pBuf->len;
		m_iMallocSize -= sizeof(RtmpPBuf);
	}

	Unlock();

	return pBuf;
}

void CLibRtmpPublish::FreeBuf(RtmpPBuf* pBuf)
{
	if (NULL == pBuf)
		return;

	if (pBuf->pLargeData)
	{
		free(pBuf->pLargeData);
		pBuf->pLargeData = NULL;
	}

	free(pBuf);
}

void CLibRtmpPublish::Connect()
{
	if (NULL == m_pRtmp)
		return;

	m_pLogger->debug("LIBRTMPPUBLISH: %08X, Connect(%s)\n", this, m_szRtmpURL);

	int err = 0;

	err = RTMP_SetupURL(m_pRtmp, m_szRtmpURL);
	if (err <= 0)
	{
		m_pLogger->error("LIBRTMPPUBLISH: %08X, FATAL ERROR, RTMP_SetupURL failed, err %d\n", this, err);

		if (m_fnLibRtmpPublishCallback)
			m_fnLibRtmpPublishCallback(this, RTMPP_CMD_SETURL, RTMPP_RET_ERROR, err, 0, 0, NULL, 0);

		return;
	}

	RTMP_EnableWrite(m_pRtmp);

	err = RTMP_Connect(m_pRtmp, NULL);
	if (err <= 0)
	{
		m_pLogger->error("LIBRTMPPUBLISH: %08X, FATAL ERROR, RTMP_Connect failed, err %d\n", this, err);

		m_bIsFailed = true;

		if (m_fnLibRtmpPublishCallback)
			m_fnLibRtmpPublishCallback(this, RTMPP_CMD_CONNECT, RTMPP_RET_ERROR, err, 0, 0, NULL, 0);

		return;
	}

	err = RTMP_ConnectStream(m_pRtmp, 0);
	if (err <= 0)
	{
		m_pLogger->error("LIBRTMPPUBLISH: %08X, FATAL ERROR, RTMP_ConnectStream failed, err %d\n", this, err);

		if (m_fnLibRtmpPublishCallback)
			m_fnLibRtmpPublishCallback(this, RTMPP_CMD_CONNECTSTREAM, RTMPP_RET_ERROR, err, 0, 0, NULL, 0);

		return;
	}

	m_pRtmp->m_outChunkSize = 8*1024*1024;
	SendSetChunkSize(m_pRtmp->m_outChunkSize);

	m_iRtmpSkt = RTMP_Socket(m_pRtmp);
	m_bIsConnected = true;
	m_bIsFailed = false;
	m_bIsStart = true;

	m_iTotalSendBytes = 0;
	m_iCurrentTimecode = 0;

	m_pLogger->debug("LIBRTMPPUBLISH: %08X, Connect(%s) success\n", this, m_szRtmpURL);

	if (!m_bSentAACDecoderSpecificInfo && m_lenAACDecoderSpecificInfo > 0)
		SendAACSequenceHeaderPacket();

	if (!m_bSentAVCDecoderSpecificInfo && m_lenAVCSps > 0 && m_lenAVCPps > 0)
		SendAVCSequenceHeaderPacket();

	if (m_fnLibRtmpPublishCallback)
		m_fnLibRtmpPublishCallback(this, RTMPP_CMD_CONNECTED, RTMPP_RET_OK, 0, 0, 0, NULL, 0);
}

void CLibRtmpPublish::SendSetChunkSize(unsigned int chunkSize)
{
	RTMPPacket rtmp_pakt;
	RTMPPacket_Reset(&rtmp_pakt);
	RTMPPacket_Alloc(&rtmp_pakt, 4);

	rtmp_pakt.m_packetType = 0x01;
	rtmp_pakt.m_nChannel = 0x02;    // control channel
	rtmp_pakt.m_headerType = RTMP_PACKET_SIZE_LARGE;
	rtmp_pakt.m_nInfoField2 = 0;

	rtmp_pakt.m_nBodySize = 4;
	UI32ToBytes(rtmp_pakt.m_body, chunkSize);

	RTMP_SendPacket(m_pRtmp, &rtmp_pakt, 0);
	RTMPPacket_Free(&rtmp_pakt);
}

static uint8 SampleFreqIndex2SoundRateIndex(int iSampleFreqIndex)
{
	if (iSampleFreqIndex <= 4)
		return 3;
	else if (iSampleFreqIndex <= 7)
		return 2;
	else if (iSampleFreqIndex <= 10)
		return 1;
	else
		return 0;
}

bool CLibRtmpPublish::SendAACSequenceHeaderPacket()
{
	char aac_seq_buf[1024];

	char* pbuf = aac_seq_buf;

	uint8 flag = 0;
	// soundformat "10 == AAC"
	// soundrate   "3  == 44-kHZ"
	// soundsize   "1  == 16bit"
	// soundtype   "1  == Stereo"
	flag = (10 << 4) | (SampleFreqIndex2SoundRateIndex(m_iSampleFreqIndex) << 2) | (1 << 1) | (2 == m_iChannel ? 1 : 0);

	pbuf = UI08ToBytes(pbuf, flag);
	pbuf = UI08ToBytes(pbuf, 0);    // aac packet type (0, header)

	for (int k=0; k<m_lenAACDecoderSpecificInfo; k++)
	{
		pbuf = UI08ToBytes(pbuf, m_bufAACDecoderSpecificInfo[k]);
	}

	m_bSentAACDecoderSpecificInfo = true;

	return Send(aac_seq_buf, (int)(pbuf-aac_seq_buf), FLV_TAG_TYPE_AUDIO, 0);
}

bool CLibRtmpPublish::SendAVCSequenceHeaderPacket()
{
	char avc_seq_buf[1024];

	char* pbuf = avc_seq_buf;

	uint8 flag = 0;
	// frametype "1  == keyframe"
	// codecid   "7  == AVC"
	flag = (1 << 4) | 7;

	pbuf = UI08ToBytes(pbuf, flag);
	pbuf = UI08ToBytes(pbuf, 0);	// avc packet type (0, header)
	pbuf = UI24ToBytes(pbuf, 0);	// composition time

	// AVCDecoderConfigurationRecord
	pbuf = UI08ToBytes(pbuf, 1);				// configurationVersion
	pbuf = UI08ToBytes(pbuf, m_bufAVCSps[1]);	// AVCProfileIndication
	pbuf = UI08ToBytes(pbuf, m_bufAVCSps[2]);	// profile_compatibility
	pbuf = UI08ToBytes(pbuf, m_bufAVCSps[3]);	// AVCLevelIndication
	pbuf = UI08ToBytes(pbuf, 0xff);				// 6 bits reserved (111111) + 2 bits nal size length - 1
	pbuf = UI08ToBytes(pbuf, 0xe1);				// 3 bits reserved (111) + 5 bits number of sps (00001)
	pbuf = UI16ToBytes(pbuf, m_lenAVCSps);		// sps
	memcpy(pbuf, m_bufAVCSps, m_lenAVCSps);
	pbuf += m_lenAVCSps;
	pbuf = UI08ToBytes(pbuf, 1);				// number of pps
	pbuf = UI16ToBytes(pbuf, m_lenAVCPps);    // pps
	memcpy(pbuf, m_bufAVCPps, m_lenAVCPps);
	pbuf += m_lenAVCPps;

	m_bSentAVCDecoderSpecificInfo = true;

	return Send(avc_seq_buf, (int)(pbuf-avc_seq_buf), FLV_TAG_TYPE_VIDEO, 0);
}

bool CLibRtmpPublish::SendAudioDataPacket(char* pData, int sizeOfData, uint32 timecode)
{
	//m_pLogger->debug("LIBRTMPPUBLISH: %08X, SendAudioDataPacket(%d, %u)\n", this, sizeOfData, timecode);

	if (m_bQuit)
		return false;

	char tmpbuf[16];
	char* pbuf = tmpbuf;

	uint8 flag = 0;
	// soundformat "10 == AAC"
	// soundrate   "3  == 44-kHZ"
	// soundsize   "1  == 16bit"
	// soundtype   "1  == Stereo"
	flag = (10 << 4) | (SampleFreqIndex2SoundRateIndex(m_iSampleFreqIndex) << 2) | (1 << 1) | (2 == m_iChannel ? 1 : 0);

	pbuf = UI08ToBytes(pbuf, flag);
	pbuf = UI08ToBytes(pbuf, 1);    // aac packet type (1, raw)

	int lenAMFHeader = (int)(pbuf-tmpbuf);
	memcpy(pData-lenAMFHeader, tmpbuf, lenAMFHeader);

	return Send(pData-lenAMFHeader, sizeOfData+lenAMFHeader, FLV_TAG_TYPE_AUDIO, timecode-m_timecodeZero);
}

bool CLibRtmpPublish::SendVideoDataPacket(char* pData, int sizeOfData, uint32 timecode)
{
	//m_pLogger->debug("LIBRTMPPUBLISH: %08X, SendVideoDataPacket(%d, %u)\n", this, sizeOfData, timecode);

	if (m_bQuit)
		return false;

	int naluType = (int)((unsigned char)pData[4] & 0x1F);

	char tmpbuf[16];
	char* pbuf = tmpbuf;

	uint8 flag = 0;
	// frametype "1  == keyframe"
	// codecid   "7  == AVC"
	if (7 == naluType)
		flag = 0x17;
	else
		flag = 0x27;

	pbuf = UI08ToBytes(pbuf, flag);
	pbuf = UI08ToBytes(pbuf, 1);	// avc packet type (0, nalu)
	pbuf = UI24ToBytes(pbuf, 0);	// composition time

	int lenAMFHeader = (int)(pbuf-tmpbuf);
	memcpy(pData-lenAMFHeader, tmpbuf, lenAMFHeader);

	return Send(pData-lenAMFHeader, sizeOfData+lenAMFHeader, FLV_TAG_TYPE_VIDEO, timecode-m_timecodeZero);
}

bool CLibRtmpPublish::Send(const char* pData, int sizeOfData, int type, uint32 timecode)
{
	RTMPPacket rtmp_pakt;
	RTMPPacket_Reset(&rtmp_pakt);
	RTMPPacket_Alloc(&rtmp_pakt, sizeOfData);

	rtmp_pakt.m_packetType = type;
	rtmp_pakt.m_nBodySize = sizeOfData;
	rtmp_pakt.m_nTimeStamp = timecode;
	rtmp_pakt.m_nChannel = 4;
	rtmp_pakt.m_headerType = RTMP_PACKET_SIZE_LARGE;
	rtmp_pakt.m_nInfoField2 = m_pRtmp->m_stream_id;
	memcpy(rtmp_pakt.m_body, pData, sizeOfData);

	int retval = RTMP_SendPacket(m_pRtmp, &rtmp_pakt, 0);
	RTMPPacket_Free(&rtmp_pakt);

	if (!retval)
	{
		m_pLogger->debug("LIBRTMPPUBLISH: %08X, Send() failed, %d bytes\n", this, sizeOfData);
	}

	return retval;
}

void CLibRtmpPublish::MainThreadFn()
{
	m_pLogger->info("LIBRTMPPUBLISH: %08X, LibRtmpPublishThreadFn start ...\n", this);

	m_bInit = true;
	m_bQuit = false;

	m_bIsConnected = false;
	m_bIsStart = false;
	m_timecodeZero = 0xFFFFFFFF;

	m_lenAACDecoderSpecificInfo = 0;
	m_lenAVCSps = 0;
	m_lenAVCPps = 0;

	m_pAvcBuf = NULL;
	m_pRtmp = NULL;

	RTMP_LogSetLevel(RTMP_LOGERROR);
	RTMP_LogSetOutput(NULL);

	m_pRtmp = RTMP_Alloc();
	RTMP_Init(m_pRtmp);
	RTMP_SetBufferMS(m_pRtmp, 300);

	if (NULL == m_pRtmp)
	{
		m_pLogger->error("LIBRTMPPUBLISH: %08X, FATAL ERROR, create RTMP failed\n", this);

		if (m_fnLibRtmpPublishCallback)
			m_fnLibRtmpPublishCallback(this, RTMPP_CMD_CREATE, RTMPP_RET_ERROR, 0, 0, 0, NULL, 0);
	}

	uint32 uiDumpMemInfo = 0;

	while (!m_bExit)
	{
		RtmpPBuf* pBuf = GetBuf();

		if (m_bExit)
		{
			FreeBuf(pBuf);
			break;
		}

		if (NULL == pBuf || NULL == m_pRtmp)
		{
			FreeBuf(pBuf);
			SleepUs(10000);
			continue;
		}

		switch (pBuf->cmd)
		{
			case RTMPP_CMD_QUIT:
				{
					m_pLogger->info("LIBRTMPPUBLISH: %08X, QUIT\n", this);

					m_bIsConnected = false;

					if (m_pRtmp)
					{
						RTMP_DeleteStream(m_pRtmp);
						RTMP_Close(m_pRtmp);
						RTMP_Free(m_pRtmp);
						m_pRtmp = NULL;
					}
				}
				break;

			case RTMPP_CMD_CONNECT:
				{
					Connect();
				}
				break;

			case RTMPP_CMD_AAC:
				{
					bool bOK;

					if (pBuf->pLargeData)
						bOK = SendAudioDataPacket(pBuf->pLargeData+AMF_HEADER_SIZE, pBuf->len, pBuf->timecode);
					else
						bOK = SendAudioDataPacket(pBuf->data+AMF_HEADER_SIZE, pBuf->len, pBuf->timecode);

					if (!m_bIsFailed && !bOK)
					{
						if (m_fnLibRtmpPublishCallback)
							m_fnLibRtmpPublishCallback(this, RTMPP_CMD_CONNECT, RTMPP_RET_ERROR, 0, 0, 0, NULL, 0);
					}

					m_bIsFailed = !bOK;
				}
				break;

			case RTMPP_CMD_AVC:
				{
					bool bOK;

					if (pBuf->pLargeData)
						bOK = SendVideoDataPacket(pBuf->pLargeData+AMF_HEADER_SIZE, pBuf->len, pBuf->timecode);
					else
						bOK = SendVideoDataPacket(pBuf->data+AMF_HEADER_SIZE, pBuf->len, pBuf->timecode);

					if (bOK && m_iBufNum > OVERFLOW_THRESHOLD)
					{
						m_pLogger->error("LIBRTMPPUBLISH: %08X, BUFFER OVERFLOW !!! bufNum %d\n", this, m_iBufNum);
						bOK = false;
					}

					if (!m_bIsFailed && !bOK)
					{
						if (m_fnLibRtmpPublishCallback)
							m_fnLibRtmpPublishCallback(this, RTMPP_CMD_CONNECT, RTMPP_RET_ERROR, 0, 0, 0, NULL, 0);
					}

					m_bIsFailed = !bOK;
				}
				break;
		}

		FreeBuf(pBuf);

		if (0 != m_iCurrentTimecode && (int)(m_iCurrentTimecode-uiDumpMemInfo) >= 300000)
		{
			uiDumpMemInfo = m_iCurrentTimecode;
			m_pLogger->debug(
				"LIBRTMPPUBLISH: %08X, timecode %u, total send %lld bytes, bufNum %d, %d bytes\n",
				this,
				m_iCurrentTimecode,
				m_iTotalSendBytes,
				m_iBufNum,
				m_iMallocSize
			);
		}
	}	//while (!m_bExit)

	m_bInit = false;

	m_bIsConnected = false;
	m_bIsStart = false;
	m_timecodeZero = 0xFFFFFFFF;

	m_lenAACDecoderSpecificInfo = 0;
	m_lenAVCSps = 0;
	m_lenAVCPps = 0;

	if (m_pAvcBuf) {
		FreeBuf(m_pAvcBuf);
		m_pAvcBuf = NULL;
	}

	if (m_pRtmp) {
		m_pLogger->info("LIBRTMPPUBLISH: %08X, Disconnect()\n", this);

		RTMP_DeleteStream(m_pRtmp);
		RTMP_Close(m_pRtmp);
		RTMP_Free(m_pRtmp);
		m_pRtmp = NULL;
	}

	m_iRtmpSkt = -1;

	while (1) {
		RtmpPBuf* pBuf = GetBuf();
		if (NULL == pBuf)
			break;

		FreeBuf(pBuf);
	}

	m_pLogger->info("LIBRTMPPUBLISH: %08X, LibRtmpPublishThreadFn stopped\n", this);
}
