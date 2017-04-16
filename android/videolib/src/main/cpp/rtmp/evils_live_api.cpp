
#include "os.h"
#include "evils_live_api.h"
#include "librtmppublish.h"
#include "libh264enc.h"

#include "../log.h"


#define MAX_FRAME_SIZE      (2 * 1024 * 1024)
#define MAX_PUSH_STREAMS    (10)

typedef struct {
    CLibRtmpPublishBase *   rtmp_handle;
    H264VENC_Handle         x264_handle;
    unsigned char *         x264_frame;
    unsigned                index;
    bool                    is_exist;
}PushHandle;

static timespec start_ms;

static uint32 getTimeMs()
{
    struct timespec tNow;

    clock_gettime(CLOCK_MONOTONIC, &tNow);

    int iSec = (int)(tNow.tv_sec - start_ms.tv_sec);
    int iMs  = (int)(tNow.tv_nsec / 1000000) - (int)(start_ms.tv_nsec / 1000000);

    return (uint32)(1000 * iSec + iMs);
}

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
    clock_gettime(CLOCK_MONOTONIC, &start_ms);
    memset(g_PushHandle, 0, sizeof(g_PushHandle));
    return 0;
}

void MyLibRtmpPublishCallback(CLibRtmpPublishBase* pLibRtmpPublish, int cmd, int result, int, int, int64, char*, int)
{
    log_error("CALLBACK: cmd %d, result %d\n", cmd, result);

    if (RTMPP_CMD_CONNECTED == cmd && RTMPP_RET_OK == result)
    {
        log_error("CALLBACK: RTMP Publish connected\n\n\n");
    }
}

int evils_live_start_push_stream(int protocol, char *url)
{
    (void)protocol;
    unsigned char * h264_frame = (unsigned char *)malloc(MAX_FRAME_SIZE);
    if (NULL == h264_frame) {
        return -1;
    }

    CLibRtmpPublishBase* rtmp_handle = CreateLibRtmpPublish();
    if (NULL == rtmp_handle) {
        free(h264_frame);
        return -1;
    }

    rtmp_handle->SetOnLibRtmpPublishCallback(MyLibRtmpPublishCallback);

    if (!rtmp_handle->Start())
    {
        //m_pLogger->error("CLibRtmpPublish::Start() failed\n");
        log_error("evils_live_start_push_stream Start fail");
        rtmp_handle->Stop();
        delete rtmp_handle;
        free(h264_frame);
        return -1;
    }
    else
    {
        log_error("evils_live_start_push_stream Connect");
        log_error("evils_live_start_push_stream Connect url %s", url);
        rtmp_handle->Connect(url);
        //m_pLogger->info("MEDIASVR: live streaming to '%s' success\n", pTask->m_listLivePath[k]);
    }

    /* to do */
    H264VENC_Params h264Params;

    h264Params.ProfileId        = 100;
    h264Params.FrameWidth       = 640;
    h264Params.FrameHeight      = 480;
    h264Params.LevelId          = 40;
    h264Params.IdrFrameInterval = 60;
    h264Params.OutputFormat     = 1;
    h264Params.SarHeight        = 0;
    h264Params.SarWidth         = 0;
    h264Params.SlicePackMode    = 0;
    h264Params.Transform8x8Flag = 1;
    h264Params.InterPartition   = 1;
    h264Params.RateControlMode  = 1;
    h264Params.bitrate          = 1024;
    h264Params.FrameRate        = 30;
    h264Params.preset           = SuperFast;
    h264Params.csp              = CSP_I420;

    H264VENC_Handle x264_handle = H264VENC_Create(&h264Params);
    if (NULL == x264_handle) {
        log_error("evils_live_start_push_stream H264VENC_Create failed!");
        rtmp_handle->Stop();
        delete rtmp_handle;
        free(h264_frame);
        return -1;
    }
    PushHandle handle;
    handle.rtmp_handle = rtmp_handle;
    handle.x264_handle = x264_handle;
    handle.x264_frame = h264_frame;
    handle.is_exist = true;

    int index;

    mutex.Lock();

    index = find_idle_handle();

    if (index >= MAX_PUSH_STREAMS) {
        log_error("index = %d over MAX_PUSH_STREAMS!!", index);
        delete rtmp_handle;
        free(h264_frame);
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
            
            if (g_PushHandle[index].x264_handle) {
                H264VENC_Close(g_PushHandle[index].x264_handle);
                g_PushHandle[index].x264_handle = NULL;
            }

            if (g_PushHandle[index].x264_frame) {
                free(g_PushHandle[index].x264_frame);
                g_PushHandle[index].x264_frame = NULL;
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
    CLibRtmpPublishBase* pRtmp = NULL;
    mutex.Lock();
    //log_error("evils_live_send_yuv420 index %d (yvv420 %p yuv_len %d width %d, height %d)", index, yuv420, yuv_len, width, height);
    if (index >= 0 && index < MAX_PUSH_STREAMS) {

        pRtmp = g_PushHandle[index].rtmp_handle;
        if (!pRtmp->IsConnected()) {
            log_error("rtmp has NOT been connected to server ignore this yuv");
            mutex.Unlock();
            return -2;
        }
        /* x264 encode yuv to h264 */
        H264VENC_Handle x264_handle = g_PushHandle[index].x264_handle;
        YUV_POINTERS capture_frame;

        capture_frame.pu8Y = (unsigned char *)yuv420;
    	capture_frame.pu8U = (unsigned char *)yuv420 + width * height;
    	capture_frame.pu8V = (unsigned char *)yuv420 + width * height * 5 / 4;
    	capture_frame.strideY = (unsigned int)width;
    	capture_frame.strideU = (unsigned int)width / 2;
    	capture_frame.strideV = (unsigned int)width / 2;
        //log_error("H264_EncodeFrame(%p, %d, %p) ", x264_handle, index, g_PushHandle[index].x264_frame);
        int frame_size = H264_EncodeFrame(x264_handle, &capture_frame, g_PushHandle[index].x264_frame);
        if (frame_size > MAX_FRAME_SIZE || frame_size <= 4) {
            log_error("evils_live_send_yuv420 H264_EncodeFrame failed frame_size = %d", frame_size);
        }

        if (pRtmp && frame_size > 4) {
            unsigned char * frame = g_PushHandle[index].x264_frame;
            int nalPos[256];
            int nalPos_index = 0;
            int frame_index = 0;
            uint32 slice_len;

            while (1) {
                int naluType = frame[frame_index + 4] & 0x1F;
                if (7 == naluType || 8 == naluType || 5 == naluType || 1 == naluType) {
                    nalPos[nalPos_index] = frame_index;
                    nalPos_index++;
                }

                slice_len = *(uint32 *)(frame + frame_index);
                slice_len = ntohl(slice_len);
                frame_index += (slice_len + 4);
                if (frame_index >= frame_size) {
                    break;
                }
            }
            for (int i = 0; i < nalPos_index; ++i) {
                slice_len = *(uint32 *)(frame + nalPos[i]);
                slice_len = ntohl(slice_len);
                uint32 ts = getTimeMs();
                //log_error("SendAVC nalu type = %d slice len = %d time stamp = %u", frame[nalPos[i] + 4] & 0x1F, slice_len, ts);
                flag = pRtmp->SendAVC((char *)frame + nalPos[i] + 4, (int)slice_len, ts);
            }
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


