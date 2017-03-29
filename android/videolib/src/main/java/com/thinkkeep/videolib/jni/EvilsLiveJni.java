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
     * 设置推流配置参数
     * @param url 推流服务器
     */
    public static native void setStreamConfig(byte[] url);

    /**
     * 发送一帧数据
     * @param data 一帧数据
     */
    public static native void sendStream(byte[] data);
}
