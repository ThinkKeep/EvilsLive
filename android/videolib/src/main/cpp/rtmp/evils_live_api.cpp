
#include "os.h"
#include "evils_live_api.h"
#include "librtmppublish.h"
#include "libh264enc.h"

#include "../log.h"

#define MAX_FRAME_SIZE      (2 * 1024 * 1024)
#define MAX_PUSH_STREAMS    (10)

typedef struct {
    int type;                   /**< 0:yuv420sp 1:nv21 2:other */
    int framerate;
    int bitrate;
    int width;
    int height;
    int forcedI;
}StreamConfig;

typedef struct {
    CLibRtmpPublishBase *   rtmp_handle;
    H264VENC_Handle         x264_handle;
    unsigned char *         x264_frame;
    unsigned char *         y_plane;
    unsigned char *         u_plane;
    unsigned char *         v_plane;
    StreamConfig            config;
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

int evils_live_create_push_stream(int protocol)
{
    (void)protocol;

    int index;
    mutex.Lock();
    index = find_idle_handle();
    if (index >= MAX_PUSH_STREAMS) {
        log_error("index = %d over MAX_PUSH_STREAMS!!", index);
        mutex.Unlock();
        return -1;
    }
    g_PushHandle[index].is_exist = true;
    mutex.Unlock();

    return index;
}

int evils_live_stream_config(int index, int width, int height, int framerate, int bitrate, bool forcedI)
{
    log_error("config:width(%d), height(%d) framerate(%d), birate(%d), forcedI(%d)", width, height, framerate, bitrate, forcedI);
    if (index < 0) {
        log_error("index(%d) < 0 !!!", index);
        return -1;
    }
    mutex.Lock();

    StreamConfig *config = &g_PushHandle[index].config;

    config->type = 1;/**< nv21 default */

    if (width != config->width || height != config->height) {
        config->width = width;
        config->height = height;
        if (g_PushHandle[index].x264_handle) {
            /* close x264 encoder */
            H264VENC_Close(g_PushHandle[index].x264_handle);
            g_PushHandle[index].x264_handle = NULL;
        } else {
            if (NULL == g_PushHandle[index].x264_frame) {
                g_PushHandle[index].x264_frame = (unsigned char *)malloc(MAX_FRAME_SIZE);
                if (NULL == g_PushHandle[index].x264_frame) {
                    mutex.Unlock();
                    log_error("malloc x264_frame failed!");
                    return -1;
                }
                g_PushHandle[index].u_plane = (unsigned char *)malloc(width * height / 4);
                if (NULL == g_PushHandle[index].u_plane) {
                    mutex.Unlock();
                    log_error("malloc u_plane failed!");
                    return -1;
                }
                g_PushHandle[index].v_plane = (unsigned char *)malloc(width * height / 4);
                if (NULL == g_PushHandle[index].v_plane) {
                    mutex.Unlock();
                    log_error("malloc v_plane failed!");
                    return -1;
                }
            }
        }

        /* create x264 encoder */
        H264VENC_Params h264Params;
        h264Params.ProfileId        = 100;
        h264Params.FrameWidth       = width;
        h264Params.FrameHeight      = height;
        h264Params.LevelId          = 40;
        h264Params.IdrFrameInterval = 60;
        h264Params.OutputFormat     = 1;
        h264Params.SarHeight        = 0;
        h264Params.SarWidth         = 0;
        h264Params.SlicePackMode    = 0;
        h264Params.Transform8x8Flag = 1;
        h264Params.InterPartition   = 1;
        h264Params.RateControlMode  = 1;
        h264Params.bitrate          = bitrate;
        h264Params.FrameRate        = framerate;
        h264Params.preset           = SuperFast;
        h264Params.csp              = CSP_I420;

        log_error("H264VENC_Create x264_frame %p", g_PushHandle[index].x264_frame);
        g_PushHandle[index].x264_handle = H264VENC_Create(&h264Params);
        if (NULL == g_PushHandle[index].x264_handle) {
            log_error("H264VENC_Create failed!");
            mutex.Unlock();
            return -1;
        }
    } else {
        if (framerate != config->framerate || bitrate != config->bitrate) {
            H264VENC_DynamicParams h264DynamicParams;
            h264DynamicParams.Deblock       = 1;
            h264DynamicParams.ForceIFrame   = 0;
            h264DynamicParams.SliceSize     = 0xFFFF;
            h264DynamicParams.RcQMax        = 45;
            h264DynamicParams.RcQMin        = 10;
            h264DynamicParams.FrameRate     = framerate;
            h264DynamicParams.TargetBitRate = bitrate;      // in kbps
            H264_SetDynamicParams(g_PushHandle[index].x264_handle, &h264DynamicParams);

            config->framerate = framerate;
            config->bitrate = bitrate;
        } else if (!forcedI) {
            log_error("parameter are same as last");
        }
    }
    config->forcedI = forcedI;
    config->bitrate = bitrate;
    config->framerate = framerate;
    config->height = height;
    config->width = width;

    mutex.Unlock();
    return index;
}

int evils_live_start_push_stream(int index, char *url)
{
    if (index < 0 || index > MAX_PUSH_STREAMS || NULL == url) {
        log_error("parameters error");
        return -1;
    }
    CLibRtmpPublishBase* rtmp_handle = CreateLibRtmpPublish();
    if (NULL == rtmp_handle) {
        log_error("CreateLibRtmpPublish failed!");
        return -1;
    }

    rtmp_handle->SetOnLibRtmpPublishCallback(MyLibRtmpPublishCallback);

    if (!rtmp_handle->Start())
    {
        //m_pLogger->error("CLibRtmpPublish::Start() failed\n");
        log_error("evils_live_start_push_stream Start fail");
        rtmp_handle->Stop();
        delete rtmp_handle;
        return -1;
    }
    else {
        log_error("evils_live_start_push_stream Connect");
        log_error("evils_live_start_push_stream Connect url %s", url);
        rtmp_handle->Connect(url);
        //m_pLogger->info("MEDIASVR: live streaming to '%s' success\n", pTask->m_listLivePath[k]);
    }

    g_PushHandle[index].rtmp_handle = rtmp_handle;

    return index;
}

int evils_live_stop_push_stream(int index)
{
    bool flag = false;
    log_error("stop push 1");
    mutex.Lock();
    log_error("stop push 2");
    if (index >= 0 && index < MAX_PUSH_STREAMS) {
        if (g_PushHandle[index].is_exist) {
            if (g_PushHandle[index].rtmp_handle) {
                log_error("stop push 1212 flag %d", flag);
                g_PushHandle[index].rtmp_handle->Stop();
                log_error("stop push 1212 Stop %d");
            }
            
            if (g_PushHandle[index].x264_handle) {
                H264VENC_Close(g_PushHandle[index].x264_handle);
                g_PushHandle[index].x264_handle = NULL;
            }

            if (g_PushHandle[index].x264_frame) {
                free(g_PushHandle[index].x264_frame);
                g_PushHandle[index].x264_frame = NULL;
            }
            if (g_PushHandle[index].u_plane) {
                free(g_PushHandle[index].u_plane);
                g_PushHandle[index].u_plane = NULL;
            }
            if (g_PushHandle[index].v_plane) {
                free(g_PushHandle[index].v_plane);
                g_PushHandle[index].v_plane = NULL;
            }
            
            g_PushHandle[index].is_exist = false;
            flag = true;
        }
    }
    mutex.Unlock();
    //log_error("stop push 3 flag %d", flag);
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

static int evils_live_send_yuv(int index, YUV_POINTERS capture_frame)
{

    bool flag = false;
    CLibRtmpPublishBase* pRtmp = NULL;
    mutex.Lock();
    //log_error("evils_live_send_yuv420 index %d (yvv420 %p yuv_len %d width %d, height %d)", index, yuv420, yuv_len, width, height);
    if (index >= 0 && index < MAX_PUSH_STREAMS) {
        //log_error("g_PushHandle[index].rtmp_handle %p ", g_PushHandle[index].rtmp_handle);
        pRtmp = g_PushHandle[index].rtmp_handle;
        if (NULL == pRtmp) {
            log_error("rtmp has NOT been created");
            mutex.Unlock();
            return -3;
        }
        if (!pRtmp->IsConnected()) {
            log_error("rtmp has NOT been connected to server ignore this yuv");
            mutex.Unlock();
            return -2;
        }
        /* x264 encode yuv to h264 */
        H264VENC_Handle x264_handle = g_PushHandle[index].x264_handle;
//check forcedI flag
        if (g_PushHandle[index].config.forcedI) {
            H264VENC_DynamicParams h264DynamicParams;
            h264DynamicParams.Deblock       = 1;
            h264DynamicParams.ForceIFrame   = 1;
            h264DynamicParams.SliceSize     = 0xFFFF;
            h264DynamicParams.RcQMax        = 45;
            h264DynamicParams.RcQMin        = 10;
            h264DynamicParams.FrameRate     = g_PushHandle[index].config.framerate;
            h264DynamicParams.TargetBitRate = g_PushHandle[index].config.bitrate;      // in kbps
            H264_SetDynamicParams(g_PushHandle[index].x264_handle, &h264DynamicParams);
        }
        //log_error("H264_EncodeFrame(%p, %d, %p) ", x264_handle, index, g_PushHandle[index].x264_frame);
        int frame_size = H264_EncodeFrame(x264_handle, &capture_frame, g_PushHandle[index].x264_frame);
        if (frame_size > MAX_FRAME_SIZE || frame_size <= 4) {
            log_error("evils_live_send_yuv420 H264_EncodeFrame failed frame_size = %d", frame_size);
        }

        //check forcedI flag
        if (g_PushHandle[index].config.forcedI) {
            H264VENC_DynamicParams h264DynamicParams;
            h264DynamicParams.Deblock       = 1;
            h264DynamicParams.ForceIFrame   = 0;
            h264DynamicParams.SliceSize     = 0xFFFF;
            h264DynamicParams.RcQMax        = 45;
            h264DynamicParams.RcQMin        = 10;
            h264DynamicParams.FrameRate     = g_PushHandle[index].config.framerate;
            h264DynamicParams.TargetBitRate = g_PushHandle[index].config.bitrate;      // in kbps
            H264_SetDynamicParams(g_PushHandle[index].x264_handle, &h264DynamicParams);
            g_PushHandle[index].config.forcedI = 0;
        }

        if (frame_size > 4) {
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
                if ((frame[nalPos[i] + 4] & 0x1F) == 5) {
                    log_error("I frame！");
                }
                flag = pRtmp->SendAVC((char *)frame + nalPos[i] + 4, (int)slice_len, ts);
            }
        }
    }
    mutex.Unlock();

    return flag;
}

static YUV_POINTERS nv21toyuv420sp(int index, unsigned char *yuv420, int w, int h)
{
    YUV_POINTERS capture_frame;
    capture_frame.pu8Y = yuv420;
    capture_frame.pu8U = yuv420 + w * h;
    capture_frame.pu8V = yuv420 + w * h * 5 / 4;
    capture_frame.strideY = (unsigned int)w;
    capture_frame.strideU = (unsigned int)w / 2;
    capture_frame.strideV = (unsigned int)w / 2;

    if (g_PushHandle[index].u_plane && g_PushHandle[index].v_plane) {
        int uv_len = w * h / 2;
        unsigned char *uv_data = capture_frame.pu8U;
        for (int i = 0; i < uv_len; i += 2) {
            g_PushHandle[index].v_plane[i / 2] = uv_data[i];
            g_PushHandle[index].u_plane[i / 2] = uv_data[i + 1];
        }
        capture_frame.pu8U = g_PushHandle[index].u_plane;
        capture_frame.pu8V = g_PushHandle[index].v_plane;
    }

    return capture_frame;

}

int evils_live_send_yuv420(int index, char * yuv420, int yuv_len, int width, int height)
{
    YUV_POINTERS capture_frame;
    if (1 == g_PushHandle[index].config.type) {
        capture_frame = nv21toyuv420sp(index, (unsigned char *)yuv420, width, height);
    } else {
        capture_frame.pu8Y = (unsigned char *)yuv420;
        capture_frame.pu8U = (unsigned char *)yuv420 + width * height;
        capture_frame.pu8V = (unsigned char *)yuv420 + width * height * 5 / 4;
        capture_frame.strideY = (unsigned int)width;
        capture_frame.strideU = (unsigned int)width / 2;
        capture_frame.strideV = (unsigned int)width / 2;
    }
    return evils_live_send_yuv(index, capture_frame);
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


