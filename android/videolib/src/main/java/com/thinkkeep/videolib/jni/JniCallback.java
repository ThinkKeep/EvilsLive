package com.thinkkeep.videolib.jni;

/**
 * 底层调用 java 接口类
 * usage:
 * javac JniCallback.java
 * javap -s -private JniCallback
 * >public static int testCall(int);
 * > descriptor: (I)I
 * Created by jason on 17/4/15.
 */

public class JniCallback {

    public static int testCall(int index, String str) {
        // TODO: 17/4/15  用于测试
        return 0;
    }
}
