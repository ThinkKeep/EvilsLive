package com.thinkkeep.videolib.jni;

/**
 * Created by jason on 17/1/12.
 */

public class EvilsLiveJni {
    static {
        System.loadLibrary("native-lib");
    }

    /**
     * for test
     * @return hello
     */
    public static native String testJni();

    /**
     * 初始化
     */
    public static native void init();

    /**
     * 创建推流上下文
     * @param protocol
     * @return
     */
    public static native int createPushStream(int protocol);

    /**
     * 设置推流配置参数
     * @param index index
     * @param width width
     * @param height height
     * @param framerate framerate
     * @param bitrate bitrate
     * @param forcedI forcedI
     */
    public static native void setStreamConfig(int index, int width, int height, int framerate,
                                              int bitrate, boolean forcedI);

    /**
     * 启动推流
     * @param index index
     * @param url 推流服务器
     * @return 返回启流句柄
     */
    public static native int startPushStream(int index, byte[] url);
    /**
     * 发送一帧数据
     * @param data 一帧数据
     */
    public static native void sendStream(int index, byte[] data, int width, int height);

    /**
     * 停止推流
     * @param hanlder 推流句柄
     */
    public static native void stopPushStream(int hanlder);

    /**
     * 销毁
     */
    public static native void destory();
}
