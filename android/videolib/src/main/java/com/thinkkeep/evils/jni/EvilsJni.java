package com.thinkkeep.evils.jni;

/**
 * Created by jsson on 17/1/12.
 */

public class EvilsJni {
    static {
        System.loadLibrary("native-lib");
    }

    public static native String testJni();

    public static native String startPushStream();
}
