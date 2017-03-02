package com.thinkkeep.videolib.util;

import android.content.Context;
import android.widget.Toast;

/**
 * Created by jsson on 17/2/27.
 */

public class UIUtil {
    private static Context context;

    public static void init(Context context) {
        UIUtil.context = context.getApplicationContext();
    }

    public static void showToast(CharSequence message) {
        showToast(message, Toast.LENGTH_SHORT);
    }

    private static void showToast(CharSequence message, int duration) {
        if (ThreadUtil.isInAndroidUIThread()) {
            Toast.makeText(context, message, duration).show();
        } else {
            // TODO: 17/2/27

        }
    }
}
