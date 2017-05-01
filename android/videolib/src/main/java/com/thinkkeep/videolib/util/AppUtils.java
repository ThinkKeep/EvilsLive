package com.thinkkeep.videolib.util;

import android.content.Context;
import android.content.pm.ApplicationInfo;

/**
 * Created by jsson on 17/5/1.
 */

public class AppUtils {
    private static Boolean isDebug = null;

    public static boolean isDebug() {
        return isDebug == null ? false : isDebug;
    }
    public static void debugMode(Context context) {
        if (isDebug == null) {
            isDebug = context.getApplicationInfo() != null &&
                    (context.getApplicationInfo().flags & ApplicationInfo.FLAG_DEBUGGABLE) != 0;
        }
    }
}
