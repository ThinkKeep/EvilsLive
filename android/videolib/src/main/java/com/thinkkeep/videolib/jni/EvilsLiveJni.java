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
     * 设置推流配置参数
     * @param url 推流服务器
     */
    public static native void setStreamConfig(byte[] url);

    /**
     * 启动推流
     * @param url 推流服务器
     * @return 返回启流句柄
     */
    public static native int startPushStream(byte[] url);
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
}
