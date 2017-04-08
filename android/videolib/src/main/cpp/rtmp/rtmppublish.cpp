/************************************************************************
#      Copyright (C) 2010-2020 VIAZIJING TECHNOLOGY CO., LTD.
#      All rights reserved.
#***********************************************************************/

#include "os.h"
#include "librtmppublish_defs.h"
#include "librtmppublishbase.h"
//#include "../include/make_ts.h"

int volatile	g_exit = 0;

static void sig_catch(int signo)
{
	if (SIGPIPE != signo)
	{
		printf("      exiting ...\n");
		g_exit = 1;
	}
}

#define CUR_VERSION "1.0.0"

void version()
{
	printf("rtmppublish - %s\n", CUR_VERSION);
}

void help()
{
	printf("\n");

	version();

	printf("\n");
}


bool g_bConnected = false;

void MyLibRtmpPublishCallback(CLibRtmpPublishBase*, int cmd, int result, int, int, int64, char*, int)
{
	printf("CALLBACK: cmd %d, result %d\n", cmd, result);

	if (RTMPP_CMD_CONNECTED == cmd && RTMPP_RET_OK == result)
	{
		printf("CALLBACK: RTMP/TS Publish connected\n\n\n");
		g_bConnected = true;
	}
	else
	{
		g_exit = 1;
	}
}


char* g_bufLoadVideo = NULL;
int   g_lenLoadVideo;

int   g_iNalPos[65536];
int   g_numNalPos = 0;
int   g_curNal = 0;

char* g_bufLoadAudio = NULL;
int   g_lenLoadAudio;
int   lenLoadAudio;
char* pLoadAudio = NULL;

struct timespec g_tStart;


uint32 GetTimeMs()
{
	struct timespec tNow;
	clock_gettime(CLOCK_MONOTONIC_RAW, &tNow);

	int iSec = (int)(tNow.tv_sec-g_tStart.tv_sec);
	int iMs  = (int)(tNow.tv_nsec/1000000)-(int)(g_tStart.tv_nsec/1000000);

	return (uint32)(1000*iSec+iMs);
}


int main(int argc, char* argv[])
{
	if (argc > 1 && 0 == strcmp("-v", argv[1]))
	{
		version();
		return 0;
	}

	char szUrl[512];
	//strcpy(szUrl, "ts://239.100.100.100:8643"); // "rtmp://172.16.0.92/live/liveTest");
	strcpy(szUrl, "rtmp://10.75.225.110/live/test"); // "rtmp://172.16.0.92/live/liveTest");

	if (argc > 1)
		strcpy(szUrl, argv[1]);

	char szAvcFile[512];
	sprintf(szAvcFile, "live_ud.264");

	char szAacFile[512];
	sprintf(szAacFile, "live_ud.aac");

	FILE* fpLoad;

	printf("Loading video data (%s) ...\n", szAvcFile);

	fpLoad = fopen(szAvcFile, "rb");
	if (NULL == fpLoad)
	{
		printf("\nopen avc file '%s' failed\n\n", szAvcFile);
		return 0;
	}

	int mallocSize = 256*1024*1024;

	do
	{
		mallocSize >>= 1;
		g_bufLoadVideo = (char*)malloc(mallocSize);
	} while (NULL == g_bufLoadVideo);

	memset(g_bufLoadVideo, 0, mallocSize);

	g_lenLoadVideo = fread(g_bufLoadVideo, 1, mallocSize, fpLoad);
	fclose(fpLoad);

	printf("Build video index ...\n\n");

	g_numNalPos = 0;
	int nalHeader = 0;
	for (int k=0; k<g_lenLoadVideo; k++)
	{
		if (0x00 == g_bufLoadVideo[k])
		{
			if (nalHeader <= 2)
				nalHeader++;
		}
		else if (0x01 == g_bufLoadVideo[k])
		{
			if (3 == nalHeader)
				nalHeader++;
			else
				nalHeader = 0;
		}
		else
		{
			nalHeader = 0;
		}

		if (4 == nalHeader)
		{
			g_iNalPos[g_numNalPos++] = k+1;
			nalHeader = 0;
		}
	}
	g_iNalPos[g_numNalPos] = g_lenLoadVideo+4;

	printf("Loading audio data (%s) ...\n\n", szAacFile);

	fpLoad = fopen(szAacFile, "rb");
	if (NULL == fpLoad)
	{
		printf("\nopen aac file '%s' failed\n\n", szAacFile);
		return 0;
	}

	g_bufLoadAudio = (char*)malloc(16*1024*1024);
	memset(g_bufLoadAudio, 0, 16*1024*1024);
	g_lenLoadAudio = fread(g_bufLoadAudio, 1, 16*1024*1024, fpLoad);
	fclose(fpLoad);

	if (g_lenLoadAudio > 0)
	{
		pLoadAudio = g_bufLoadAudio;
		lenLoadAudio = g_lenLoadAudio;
	}

	printf("Starting ...\n\n");

	signal(SIGINT, sig_catch);
	signal(SIGQUIT, sig_catch);
	signal(SIGTERM, sig_catch);
	signal(SIGPIPE, sig_catch);

//	TS_Init();
	
//	CLibRtmpPublishBase* pLibRtmpPublish = strstr(szUrl, "ts:") ? CreateLibTsPublish(1000) : CreateLibRtmpPublish();
	CLibRtmpPublishBase* pLibRtmpPublish = CreateLibRtmpPublish();
	pLibRtmpPublish->SetOnLibRtmpPublishCallback(MyLibRtmpPublishCallback);
	pLibRtmpPublish->Start();
	pLibRtmpPublish->Connect(szUrl);

	uint64 timecodeWorld = 0;
	uint32 timecodeVideo = 0;
	uint32 timecodeAudio = 0;
	uint64 durationAudio = 0;

	clock_gettime(CLOCK_MONOTONIC_RAW, &g_tStart);

	while (!g_exit)
	{
		if (!g_bConnected)
		{
			SleepUs(10000);
			clock_gettime(CLOCK_MONOTONIC_RAW, &g_tStart);
			continue;
		}

		if (timecodeWorld >= timecodeVideo)
		{
			int naluType = g_bufLoadVideo[g_iNalPos[g_curNal]] & 0x1F;

			if (7 == naluType || 8 == naluType || 5 == naluType || 1 == naluType)
			{
				pLibRtmpPublish->SendAVC(g_bufLoadVideo+g_iNalPos[g_curNal], g_iNalPos[g_curNal+1]-g_iNalPos[g_curNal]-4, timecodeVideo);
			}

			if (1 == naluType || 5 == naluType)
			{
				timecodeVideo += 40;
			}

			g_curNal++;
			if (g_curNal >= g_numNalPos)
				g_curNal = 0;
		}	//if (timecodeWorld >= timecodeVideo)

		if (timecodeWorld >= timecodeAudio)
		{
			if (0 == timecodeAudio)
			{
				char buf[2];
				buf[0] = 0x12;
				buf[1] = 0x10;
				pLibRtmpPublish->SendAAC(buf, 2, 0);
				pLibRtmpPublish->SetH264Fps(30);
			}

			unsigned char* pOut = (unsigned char*)pLoadAudio;

			if (0xFF != pOut[0])
			{
				printf("AAC data error\n");
				pLoadAudio = g_bufLoadAudio;
				lenLoadAudio = g_lenLoadAudio;
			}
			else
			{
				int lenFrameAAC = 0;

				lenFrameAAC = (int)(pOut[3] & 0x03);
				lenFrameAAC <<= 8;
				lenFrameAAC |= (int)pOut[4];
				lenFrameAAC <<= 3;
				lenFrameAAC |= (int)((pOut[5] & 0xE0) >> 5);
				
				lenLoadAudio -= lenFrameAAC;
				pLoadAudio = (char*)(pOut+lenFrameAAC);

				if(!pLibRtmpPublish->isTs()){
					pOut += 7;
					lenFrameAAC -= 7;
				}
				pLibRtmpPublish->SendAAC((const char*)pOut, lenFrameAAC, timecodeAudio);
				
				durationAudio += 1024;
				timecodeAudio = (uint32)(durationAudio*1000/44100);

				if (lenLoadAudio <= 0)
				{
					pLoadAudio = g_bufLoadAudio;
					lenLoadAudio = g_lenLoadAudio;
				}
			}
		}	//if (timecodeWorld >= timecodeAudio)

		SleepUs(5000);
		timecodeWorld = GetTimeMs();
	}	//while (!g_exit)

	pLibRtmpPublish->Stop();
	delete pLibRtmpPublish;

	if (g_bufLoadAudio)
		free(g_bufLoadAudio);

	if (g_bufLoadVideo)
		free(g_bufLoadVideo);
	
	return 0;
}
