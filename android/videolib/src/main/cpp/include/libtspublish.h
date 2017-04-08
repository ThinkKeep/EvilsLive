/************************************************************************
#      Copyright (C) 2010-2020 VIAZIJING TECHNOLOGY CO., LTD.
#      All rights reserved.
#***********************************************************************/

#ifndef __LIBTS_PUBLISH_H__
#define	__LIBTS_PUBLISH_H__

#include "clogger.h"
#include "rtmp.h"
#include "log.h"
#include "librtmppublish_defs.h"
#include "librtmppublishbase.h"


#define RTMPP_BUF_SIZE			(32*1024)

typedef struct TsPBuf
{
    void*		pNext;
    void*		pPrev;
    int			cmd;
    uint32		timecode;
    int			len;
    int          nBufLen;//
    char		data[RTMPP_BUF_SIZE];
    // unsigned   long lTimeAdd;
    bool  Initialize()
    {
        pNext = 0;
        pPrev = 0;
        cmd = 0;
        timecode = 0;
        len = 0;
    }
~TsPBuf()
{
    int i =0;
}
} *PTSPBUF;

#include<list>
using namespace  std;
/*
 * class CLibTsPublish
 */
class CLibTsPublish : public CLibRtmpPublishBase
{
public:
    CLibTsPublish(int pmtid);
    ~CLibTsPublish();

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
    void						SetH264Fps(int fps);

private:
    OnLibRtmpPublishCallback	m_fnLibRtmpPublishCallback;

private:
    void						Lock();
    void						Unlock();

private:
    void						AddBuf(TsPBuf* pBuf);
    TsPBuf*						GetBuf();
    void						FreeBuf(TsPBuf* pBuf);

private:
    void						Connect();
    bool						SendAudioDataPacket(char* pData, int sizeOfData, uint32 timecode);
    bool						SendVideoDataPacket(char* pData, int sizeOfData, uint32 timecode);
    bool						Send(const char* pData, int sizeOfData, int type, uint32 timecode);
    TsPBuf                 *GetIdleTsBuf(int nNeedBufSize);
    void                       SetIdleTsBuf(TsPBuf*);
    void						MainThreadFn();
private:
    CLogger*					m_pLogger;

private:
  volatile  bool						m_bExit;
volatile    bool						m_bInit;
 volatile   bool						m_bQuit;
    thread_t					m_thread;
    CMutex						m_mutex;
    CMutex						m_mutexForTsBuf;
private:
    bool						m_bIsConnected;
    int							m_ErrCode;

private:
    TsPBuf*						m_pFirst;
    TsPBuf*						m_pLast;
volatile    int							m_iBufNum;
    int							m_iMallocSize;

private:
    uint64						m_iTotalASendBytes;
    uint64						m_iTotalVSendBytes;
    struct sockaddr_in 			peeraddr;
    FILE*						fAacInput;
    FILE*						fAvcInput;
    FILE*						fTsOutput;
    unsigned int				m_tsid;
    unsigned char*				m_tsbuf;
    int64_t 					m_atime64;
    int64_t 					m_vtime64;
    int							m_aSamplerate;
    int							m_vFps;
    int							m_pmtID;
    int							m_shouldSleep;

    struct timeval 				m_tvPre;
public:
    static CLibRtmpPublishBase* GetEntityByID(int nID);
    static list<CLibRtmpPublishBase*> s_EntityList;
    list<TsPBuf*>m_BufList;
    list<TsPBuf*>m_AllBufList;
    list<TsPBuf*>m_IdleBufList;

private:
    friend void					LibTsPublishThreadFn(void* pArg);
    int                                  m_nVNoACount;////只有视频没有音频时的个数 如果超过半秒还没有音频则开启只视频模式
    bool                              m_nOnlyVideoStyle;
};

#endif	//#ifndef __LIBTS_PUBLISH_H__
