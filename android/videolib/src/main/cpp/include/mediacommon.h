#ifndef MEDIACOMMON
#define MEDIACOMMON
#include<stddef.h>
#include<string.h>
typedef  long  PID;
typedef  long  CID;
#define MAX_NAME_LEN  256
enum MediaType
{
    MediaUnknown=-1,
    MediaVideo,//视频数据
    MediaAudio,//音频数据
    MediaSubtitle// 字幕
};
enum MediaDataSubType //表明媒体的具体类型 例如具体是h264 数据还是 aac数据还是ts流等
{//暂时只有h264和aac
   MediaDataUnknown = -1,
    MediaDataH264,//视频h164
    MediaDataADTS,//音频aac
};
typedef  struct VideoBaseInfo
{
    int  nWidth;
    int  nHeight;
    int   nFrameRate;
    VideoBaseInfo()
    {
        memset(this,0,sizeof(VideoBaseInfo));
    }
}*PVIDEOBASEINFO,VIDEOBASEINFO;
typedef  struct AudioBaseInfo
{
    int  nSample;
    AudioBaseInfo()
    {
        memset(this,0,sizeof(AudioBaseInfo));
    }
}*PAUDIOBASEINFO ,AUDIOBASEINFO;
#define PID_EXTRA_VIDEO_DATA                100 //额外的视频数据 例如h264的 sps pps
#define PID_EXTRA_AUDIO_DATA                102 //额外的音频数据
#define PID_VIDEO_FRAME_RATE                103 //视频帧率
#define PID_AUDIO_SAMPLE_RATE              104 // 音频采样率

#define PID_VIDEO_STREAM_ID                   201 //视频流ID
#define PID_AUDIO_STREAM_ID                  202 //音频流ID

#define PID_TS_TRANSPORT_STREAM_ID    301 //TS传送流ID
#define PID_TS_SERVICE_NAME                    302 //TS ServiceName
#define PID_TS_SERVICE_PROVIDER             303//TS Service Provider
#define PID_FILE_NAME                                 304 //转码保存的目标文件名 如果想要转码并保存文件 则设置文件名就可以了
#define PID_TRANSCODE_STYLE_NALE         305 //转码的目标文件格式名  例如 .ts后缀文件对应mpegts


#define PID_PRIVETA_USE_DATA                     10000
//#define PID_WORK_STATE                                10001  //工作状态 闲置 挂起 工作 禁用

#define  ES_INITIALIXED   0x1     ////对象已经初始化了
#define  ES_ENABLE          0x2     ////对象可以使用
#define  ES_PAULE            0x4     //对象被挂起了
#define  ES_LOCKED         0x8     // 对象被锁定 可以理解为正在使用或其它
//如果只转码 而不保存文件 则只需要设置格式名 不需要设置文件名

#define CID_START_RECORD_FILE    0x100
#define CID_STOP_RECORD_FILE      0x101
//声明一个回调函数
typedef void  (*PMEDIADATACALLBACK)(char*pBuf,int nBufSize,MediaType ,void * UserData,void* pReserved );
typedef void (*PERRORCALLBACK)(char *pErrorInfo,void * UserData,void* pReserved );
#define SAFE_DELETE_PTR(p) if(p) delete p; (p)= NULL;
#define SAFE_DELETE_ARRAY(p) if(p) delete[]p; (p)= NULL;
#define SAFE_CLOSE(fd)   if(fd) fclose(fd);(fd)=NULL;
#endif // MEDIACOMMON

