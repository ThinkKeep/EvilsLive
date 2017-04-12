#pragma once
#include "stdint.h"
#include"mediacommon.h"
#include"clogger.h"
#define MAX_TRANS_CHANNEL  64
#ifdef __cplusplus
extern "C"
{
#endif
	//初始化MakeTS 内部结构参数的默认值，必须在调其它函数前，首先调用此函数
	//参数说明：
	//在调用下面函数前，必须要调用此初始化函数
	// 返回值说明:
	//0: 成功
    int TS_Init();

	//参数说明：	
	// logfilename打开或关闭LOG文件, 默认不写日志
	// TS_Init 会初始化为日志指针为空,注意调用顺序
	void TS_Log(const char* logfilename=0);//日志文件名, 调用时覆盖此前可能存在的文件, NULL表示关闭LOG

	//开始一个节目合成信息，有增加多个节目要调用相应的次数，最多16套节目
	//参数说明：
	// sid			[out]流号
	// pmtpid		[in]节目号
	// h264pid		[in]视频号
	// aacpid		[in]音频号
	// interval		[in]产生PAT PMT 的间隔 默认为400包后，插入PAT　PMT
	// samplerate	[in]音频采集率
	//				默认为0, 直接使用外部时间，
	//				如不为零，则用于内部计算时间，外部在使用 PackAAC 时，time要带入，并在外部要保存此值，下次使用时要将此保存的值再带入
	// framerate	[in]视频帧率
	//				默认为0, 直接使用外部时间，
	//				如不为零，则用于内部计算时间，外部在使用 PackH264时，time要带入，并在外部要保存此值，下次使用时要将此保存的值再带入
	
	//name		[in]节目名称
	//namelen [in]节目名称长度
	// 返回值说明:
	//0: 成功
    //-1此PATPMT号已经存在且正使用
	//1: 节目数已达到最大, 最多16套节目
    //必须传入视频帧率 因为里边只分析了sps pps但二者可能不含帧率
    int TS_Start(unsigned int *sid, unsigned int pmtpid, unsigned int h264pid, unsigned int aacpid, unsigned int interval=400, unsigned int samplerate=0, unsigned int framerate=0, const char* name=0, unsigned int namelen=0);

    //先分配一个
    int TS_Alloc(unsigned int *sid, unsigned int pmtpid, const char* name=0, unsigned int namelen=0);
    int TS_AddStream(unsigned int sid,MediaDataSubType StreamType,/*0 video  1 audio orther false*/ int nRate, int nStreamID);
	//将一个完整的ADTS音频数据，打包成188长的TS包，输入一个包，输出有可能是一个或多个，其中有可能包括PAT PMT的TS数据包
        //参数说明：
	// sid			[in]流号
	// data			[in]输入缓冲区
	// len			[in]输入缓冲区数据长度
	// time			[in]外部时间 采样率为44100时，此处时间间隔为 1024*90000/44100=2089
	//				[in][out] 前提是StartTS时要设置音频采集率， 用于内部计算时间，并在外部保存此值，下次使用时要将此保存的值再带入
	// outdata		输出缓冲区
	// outlen		[in]输出缓冲区最大长度
	//				[out]输出产生包的个数，外部使用要 此数*188=数据长度
	
	// 返回值说明:
	//0: 成功
	//1: 流号无效
	//2: 输入指针无效
	//3: 输出指针无效
	//100: 测试版限制 正式版没有
    int TS_PackAAC(unsigned int sid, unsigned char* data, unsigned int len, int64_t *time,
                   unsigned char* outdata, unsigned int* outlen);//现在是为同步处理

	
	//将一个完整的视频数据，打包成188长的TS包，输入一个包，输出有可能是一个或多个，其中有可能包括PAT PMT的TS数据包
	//完整的视频数据是指，以00 00 01 或 00 00 00 01 开头的，
	//完整的SPS 或 完整的PPS 或 完整的一帧一个单片包的I帧或P帧 或 完整的一帧多个包的I帧或P帧
	//注意:一帧多个包的情况要一个接一个打包,否则播放可能不正常
	//参数说明：
	// sid			[in]流号
	// data			[in]输入缓冲区
	// len			[in]输入缓冲区数据长度
	// time			[in]外部时间 1秒钟 25帧，即每帧为40ms = 1s/25=1000ms/25, 但此处的时间应为3600间隔时间（90000/25=3600),因此用40*90=3600
	//				[in][out] 前提是StartTS时要设置视频帧率， 用于内部计算时间，并在外部保存此值，下次使用时要将此保存的值再带入
	// outdata		输出缓冲区
	// outlen		[in]输出缓冲区最大长度
	//				[out]输出产生包的个数，外部使用要 此数*188=数据长度
	
	// 返回值说明:
	//0: 成功
	//1: 流号无效
	//2: 输入指针无效
	//3: 输出指针无效
	//100: 测试版限制 正式版没有
    int TS_PackH264(unsigned int sid, unsigned char* data, unsigned int len, int64_t *time,
                    unsigned char* outdata, unsigned int* outlen);//现在是为同步处理


	//停止一个节目合成信息
	//参数说明：
	// sid			[in]流号
	// 返回值说明:
	//0: 成功
	//1: 流号无效
    int TS_Stop(unsigned int sid); //停止一个对象，只停止工作 并没有从内存释放
    int TS_Release(unsigned int sid);////从内存释放一个对象 程序关闭时清理内存  //当调用此函数后 不应当再调用TS_Start函数
    // 如果设置了回调函数，  则回调
    //如果想直接获取到数据，

    int TS_SetOutPutCallBack(unsigned int sid,PMEDIADATACALLBACK,void*pUserData);
    int TS_SetErrorCallBack   (unsigned int sid,PERRORCALLBACK,void*pUserData);

    //实现打包ts流后发送到目的地址 UDP方式, pDest为目标地址 nPort为目标IP
    //bAdd为true是增加 数据可以发送到多个地址 为false时 去除某个IP地址
    int TS_AddDest(unsigned int sid,char*pDestID,int nPort,bool bAdd );
    //按url方式 增加目标地址如 ts://172.16.2.155:9000;172.16.0.62:8000  可以增加多个目标地址 为false时 去除某个目标地址
    int TS_AddDestByUrl(unsigned int sid,char*pUrl,bool bAdd );
    int TS_DoCommand(unsigned int sid,CID command,void*pParam,int nSize);////增加执行命令接口

  #ifdef __cplusplus
}
#endif
extern CLogger g_logger;
