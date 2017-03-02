package com.thinkkeep.videolib.util;

import android.os.Looper;

/**
 * Created by jsson on 17/2/27.
 */
public class ThreadUtil {
    private static long androidUIThreadId = -1L;

    public static long getAndroidUIThreadId() {
        if (androidUIThreadId < 0L) {
            androidUIThreadId = Looper.getMainLooper().getThread().getId();
        }

        return androidUIThreadId;
    }

    public static boolean isInAndroidUIThread() {
        return Thread.currentThread().getId() == getAndroidUIThreadId();
    }
}
