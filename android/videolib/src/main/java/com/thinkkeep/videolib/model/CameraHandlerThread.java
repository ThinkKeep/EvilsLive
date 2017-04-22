package com.thinkkeep.videolib.model;

import android.os.Handler;
import android.os.HandlerThread;

/**
 * Created by jsson on 17/4/21.
 */

public class CameraHandlerThread extends HandlerThread {
    private final Handler mHandler;

    public CameraHandlerThread(String name) {
        super(name);
        start();
        mHandler = new Handler(getLooper());
    }

    synchronized void notifyCameraOpened() {
        notify();
    }

    public synchronized void openCamera(final CameraSupport cameraSupport) {
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                cameraSupport.open();
                notifyCameraOpened();
            }
        });

        try {
            wait();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
