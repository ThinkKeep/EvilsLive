/************************************************************************
#      Copyright (C) 2010-2020 VIAZIJING TECHNOLOGY CO., LTD.
#      All rights reserved.
#***********************************************************************/

#ifndef __LIBRTMP_PUBLISH_H__
#define	__LIBRTMP_PUBLISH_H__

#include "clogger.h"
#include "rtmp.h"
#include "log.h"
#include "librtmppublish_defs.h"
#include "librtmppublishbase.h"
#include <list>
using namespace  std ;


#define RTMPP_BUF_SIZE			(32*1024)

typedef struct
{
	void*		pNext;
	void*		pPrev;
	int			cmd;
	uint32		timecode;
	int			len;
	char		data[RTMPP_BUF_SIZE];
	char*		pLargeData;
} RtmpPBuf;


/*
 * class CLibRtmpPublish
 */
class CLibRtmpPublish : public CLibRtmpPublishBase
{
public:
	CLibRtmpPublish();
	~CLibRtmpPublish();

public:
	void						SetOnLibRtmpPublishCallback(OnLibRtmpPublishCallback);

public:
	bool						Start();
	bool						Stop();

public:
	bool						IsConnected();
	bool						IsFailed();

public:
	void						Connect(const char* szRtmpURL);		// for example : "rtmp://172.16.0.160/live/filmStream"
	bool						SendAAC(const char* pData, int sizeOfData, uint32 timecode);
	bool						SendAVC(const char* pData, int sizeOfData, uint32 timecode);

private:
	OnLibRtmpPublishCallback	m_fnLibRtmpPublishCallback;

private:
	void						Lock();
	void						Unlock();

private:
	void						AddBuf(RtmpPBuf* pBuf);
	RtmpPBuf*					GetBuf();
	void						FreeBuf(RtmpPBuf* pBuf);

private:
	void						Connect();
	void						SendSetChunkSize(unsigned int chunkSize);
	bool						SendAACSequenceHeaderPacket();
	bool						SendAVCSequenceHeaderPacket();
	bool						SendAudioDataPacket(char* pData, int sizeOfData, uint32 timecode);
	bool						SendVideoDataPacket(char* pData, int sizeOfData, uint32 timecode);
	bool						Send(const char* pData, int sizeOfData, int type, uint32 timecode);
	void						MainThreadFn();

private:
	CLogger*					m_pLogger;

private:
	bool						m_bExit;
	bool						m_bInit;
	bool						m_bQuit;
	thread_t					m_thread;
	CMutex						m_mutex;

private:
	int							m_iRtmpSkt;
	bool						m_bIsConnected;
	bool						m_bIsFailed;
	bool						m_bIsStart;
	uint32						m_timecodeZero;

private:
	char						m_bufAACDecoderSpecificInfo[16];
	int							m_lenAACDecoderSpecificInfo;
	bool						m_bSentAACDecoderSpecificInfo;
	char						m_bufAVCSps[1024];
	int							m_lenAVCSps;
	char						m_bufAVCPps[128];
	int							m_lenAVCPps;
	bool						m_bSentAVCDecoderSpecificInfo;
	RtmpPBuf*					m_pAvcBuf;

private:
	int							m_iSampleFreqIndex;
	int							m_iChannel;

private:
	RTMP*						m_pRtmp;
	RtmpPBuf*					m_pFirst;
	RtmpPBuf*					m_pLast;
	int							m_iBufNum;
	int							m_iMallocSize;

private:
	uint64						m_iTotalSendBytes;
	uint32						m_iCurrentTimecode;

private:
	friend void					LibRtmpPublishThreadFn(void* pArg);
public:
    static CLibRtmpPublishBase* GetEntityByID(int nID);
    static list<CLibRtmpPublishBase*> s_EntityList;
};

#endif	//#ifndef __LIBRTMP_PUBLISH_H__
