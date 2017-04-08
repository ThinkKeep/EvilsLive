
#include "os.h"
#include "evils_live_api.h"
#include "librtmppublish.h"
#include "libh264enc.h"

#define MAX_FRAME_SIZE      (2 * 1024 * 1024)
#define MAX_PUSH_STREAMS    (10)

typedef struct {
    CLibRtmpPublishBase *   rtmp_handle;
    H264VENC_Handle         x264_handle;
    unsigned char *         x264_frame;
    unsigned                index;
    bool                    is_exist;
}PushHandle;


static PushHandle g_PushHandle[MAX_PUSH_STREAMS];

static CMutex mutex;


static int find_idle_handle()
{
    int i;

    for (i = 0; i < MAX_PUSH_STREAMS; ++i) {
        if (!g_PushHandle[i].is_exist) {
            return i;
        }
    }

    return i;
}

int evils_live_init()
{
    memset(g_PushHandle, 0, sizeof(g_PushHandle));
    return 0;
}

void MyLibRtmpPublishCallback(CLibRtmpPublishBase* pLibRtmpPublish, int cmd, int result, int, int, int64, char*, int)
{

}

int evils_live_start_push_stream(int protocol, char *url)
{
    (void)protocol;

    unsigned char * h264_frame = malloc(MAX_FRAME_SIZE);
    if (NULL == h264_frame) {
        return -1;
    }

    CLibRtmpPublishBase* rtmp_handle = CreateLibRtmpPublish();
    if (NULL == rtmp_handle) {
        return -1;
    }

    rtmp_handle->SetOnLibRtmpPublishCallback(MyLibRtmpPublishCallback);

    if (!rtmp_handle->Start())
    {
        //m_pLogger->error("CLibRtmpPublish::Start() failed\n");
        rtmp_handle->Stop();
        delete rtmp_handle;
        rtmp_handle = NULL;
        return -1;
    }
    else
    {
        rtmp_handle->Connect(url);
        //m_pLogger->info("MEDIASVR: live streaming to '%s' success\n", pTask->m_listLivePath[k]);
    }

    /* to do */
    H264VENC_Params h264Params;
	h264Params.ProfileId        = 100;
	h264Params.FrameWidth       = 720;
	h264Params.FrameHeight      = 1280;
	h264Params.LevelId          = 40;
	h264Params.IdrFrameInterval = 20;
	h264Params.OutputFormat     = 1;
	h264Params.SarHeight        = 0;
	h264Params.SarWidth         = 0;
	h264Params.SlicePackMode    = 0;
	h264Params.Transform8x8Flag = 1;
	h264Params.InterPartition   = 1;
	h264Params.RateControlMode  = 1;
	h264Params.bitrate = 1000;
	h264Params.FrameRate = 20;
    H264VENC_Handle x264_handle = H264VENC_Create();

    PushHandle handle;
    handle.rtmp_handle = rtmp_handle;
    handle.x264_handle = x264_handle;
    handle.x264_frame = h264_frame;
    handle.is_exist = true;

    int index;

    mutex.Lock();

    index = find_idle_handle();
    if (index >= MAX_PUSH_STREAMS) {
        //
        delete rtmp_handle;
        rtmp_handle = NULL;

        H264VENC_Close(x264_handle);
        mutex.Unlock();
        return -1;
    }
    handle.index = index;

    g_PushHandle[index] = handle;

    mutex.Unlock();

    return index;
}

int evils_live_stop_push_stream(int index)
{
    bool flag = false;
    mutex.Lock();
    if (index >= 0 && index < MAX_PUSH_STREAMS) {
        if (g_PushHandle[index].is_exist) {
            if (g_PushHandle[index].rtmp_handle) {
                g_PushHandle[index].rtmp_handle->Stop();
            
            }
            
            if (g_PushHandle[index].x264_frame) {
                free(g_PushHandle[index].x264_frame);
                g_PushHandle[index].x264_frame = NULL;
            }
            
            if (g_PushHandle[index].x264_handle) {
                H264VENC_Close(g_PushHandle[index].x264_handle);
                g_PushHandle[index].x264_handle = NULL;
            }
            g_PushHandle[index].is_exist = false;
            flag = true;
        }
    }
    mutex.Unlock();
    return flag;
}

int evils_live_send_h264(int index, char *frame, int frame_len)
{
    bool flag = false;
    mutex.Lock();
    if (index >= 0 && index < MAX_PUSH_STREAMS) {
        if (g_PushHandle[index].rtmp_handle) {
            flag = g_PushHandle[index].rtmp_handle->SendAVC(frame, frame_len, 0);
        }
    }
    mutex.Unlock();
    return flag;
}

int evils_live_send_yuv420(int index, char * yuv420, int yuv_len, int width, int height)
{

    bool flag = false;

    mutex.Lock();
    if (index >= 0 && index < MAX_PUSH_STREAMS) {

        /* x264 encode yuv to h264 */
        H264VENC_Handle x264_handle = g_PushHandle[index].x264_handle;
        YUV_POINTERS capture_frame;

        capture_frame.pu8Y = yuv420;
    	capture_frame.pu8U = yuv420 + width * height;
    	capture_frame.pu8V = yuv420 + width * height * 5 / 4;
    	capture_frame.strideY = (unsigned int)width;
    	capture_frame.strideU = (unsigned int)width / 2;
    	capture_frame.strideV = (unsigned int)width;
        int frame_size = H264_EncodeFrame(x264_handle, &capture_frame, g_PushHandle[index].x264_frame);
        if (frame_size > MAX_FRAME_SIZE) {
            //m_pLogger->info("\n", pTask->m_listLivePath[k]);
            mutex.Unlock();
        }

        CLibRtmpPublishBase* pRtmp = g_PushHandle[index].rtmp_handle;
        if (pRtmp) {
            flag = pRtmp->SendAVC(g_PushHandle[index].x264_frame, frame_size, 0);
        }
    }
    mutex.Unlock();

    return flag;
}

int evils_live_send_aac(long handle, char *frame, int frame_len)
{
    //to do
    return 0;
}

int evils_live_send_pcm(long handle, char *pcm, int data_len)
{
    return 0;
}


int evils_live_start_pull_stream(int protocol, char *url)
{
    return 0;
}

int evils_live_stop_pull_stream(long handle, char *url)
{
    return 0;
}

int evils_live_destory()
{

    for (int i = 0; i < MAX_PUSH_STREAMS; ++i) {
        evils_live_stop_push_stream(i);
    }

    return 0;
}


