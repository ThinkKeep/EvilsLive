/*初始化*/
evils_init();

/* */
evils_start_push_stream(int protocol, char *url);

/** */
evils_stop_push_stream(char *url);

/** */
evils_start_pull_stream(int protocol, char *url);

/** */
evils_stop_pull_stream(char *url);

/*反初始化*/
evils_destory();
