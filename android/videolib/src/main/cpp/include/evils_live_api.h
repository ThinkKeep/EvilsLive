
#ifndef __EVILS_LIVE_API_H__
#define __EVILS_LIVE_API_H__
#ifdef __cplusplus
extern "C" {
#endif

int evils_live_init();


/* push stream to rtmp server */
int evils_live_start_push_stream(int protocol, char *url);

int evils_live_stop_push_stream(int index);

int evils_live_send_h264(int index, char *frame, int frame_len);

int evils_live_send_yuv420(int index, char *yuv420, int yuv_len, int width, int height);

int evils_live_send__aac(int index, char *frame, int frame_len);

int evils_live_send__pcm(int index, char *pcm, int data_len);


/* pull stream from rtmp server */
int evils_live_start_pull_stream(int protocol, char *url);

int evils_live_stop_pull_stream(long handle);


int evils_live_destory();

#ifdef __cplusplus
}
#endif
#endif
