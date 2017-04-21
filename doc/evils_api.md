//jni 调用相关接口
/** 初始化*/
evils_live_init();

/** 创建推流上下文*/
evils_live_start_push_stream(int protocl);

/** 开始推流*/
evils_live_start_push_stream(int index, char *url);

/** 停止推流*/
evils_live_stop_push_stream(int index);

/** 开始拉流*/
evils_live_start_pull_stream(int protocol, char *url);

/** 停止拉流*/
evils_live_stop_pull_stream(char *url);

/** 反初始化*/
evils_live_destory();
