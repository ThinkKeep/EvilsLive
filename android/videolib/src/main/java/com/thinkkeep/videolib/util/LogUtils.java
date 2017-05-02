package com.thinkkeep.videolib.util;

import android.util.Log;

/**
 * Created by jsson on 17/5/1.
 */

public class LogUtils {
    private static final int LEVEL = Log.DEBUG;

    private static boolean isDebug(int level) {
        return level >= LogUtils.LEVEL && AppUtils.isDebug();
    }

    private static void println(String tag, int level, String msg) {
        if (isDebug(level)) {
            Log.println(level, tag, msg);
        }
    }

    public static void d(String tag, String msg) {
        println(tag, Log.DEBUG, msg);
    }

    public static void i(String tag, String msg) {
        println(tag, Log.INFO, msg);
    }

    public static void w(String tag, String msg) {
        println(tag, Log.WARN, msg);
    }

    public static void e(String tag, String msg) {
        println(tag, Log.ERROR, msg);
    }

}
