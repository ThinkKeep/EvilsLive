/************************************************************************
#      Copyright (C) 2010-2020 VIAZIJING TECHNOLOGY CO., LTD.
#      All rights reserved.
#***********************************************************************/

#ifndef __LIBRTMP_PUBLISH_BASE_H__
#define	__LIBRTMP_PUBLISH_BASE_H__


class CLibRtmpPublishBase;

CLibRtmpPublishBase* CreateLibRtmpPublish();
CLibRtmpPublishBase* CreateLibTsPublish(int pmtid);
CLibRtmpPublishBase *GetEntityByID(char *pEntityType,int nID);//
/*
 * OnLibRtmpPublishCallback
 * cmd, RTMPP_CMD_xxxx
 * result, RTMPP_RET_xxxx
 */
typedef void (*OnLibRtmpPublishCallback)(CLibRtmpPublishBase*, int cmd, int result, int p1, int p2, int64 p3, char* pData, int sizeOfData);

#define TSPREFIX_FLAG   "ts:"
/*
 * class CLibRtmpPublishBase
 */
class CLibRtmpPublishBase
{
public:
    CLibRtmpPublishBase() {m_nStreamID = 0;memset(m_szRtmpURL,0,sizeof(m_szRtmpURL));};
	virtual ~CLibRtmpPublishBase() {};

public:
	virtual void		SetOnLibRtmpPublishCallback(OnLibRtmpPublishCallback) = 0;

public:
	virtual bool		Start() = 0;
	virtual bool		Stop() = 0;

public:
	virtual bool		IsConnected() = 0;
	virtual bool		IsFailed() = 0;
	virtual void		SetH264Fps(int /*fps*/){}
	
public:
	bool isTs()			{ return strstr(m_szRtmpURL, "ts:"); };
    int      GetStreamID(){return m_nStreamID;};
    void      SetStreamID(int nStreamID ){ m_nStreamID = nStreamID;};
     char*     GetUrl(){return m_szRtmpURL;};
public:
	virtual void		Connect(const char* szRtmpURL) = 0;		// for example : "rtmp://172.16.0.160/live/filmStream"
	virtual bool		SendAAC(const char* pData, int sizeOfData, uint32 timecode) = 0;
	virtual bool		SendAVC(const char* pData, int sizeOfData, uint32 timecode) = 0;
public:
protected:	
	char				m_szRtmpURL[1024];
    int                   m_nStreamID;
};

#endif	//#ifndef __LIBRTMP_PUBLISH_BASE_H__
