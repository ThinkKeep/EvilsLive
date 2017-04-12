package com.thinkkeep.videolib.jni;

import android.util.Log;

import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * Created by jason on 17/3/29.
 */

public class JniManager extends Thread {
    private static final JniManager INSTANCE = new JniManager();
    private final ConcurrentLinkedQueue<Runnable> callList = new ConcurrentLinkedQueue<>();
    private boolean running;
    private int index = -2;

    public void init() {
        if (this.running) {
            return;
        }

        this.running = true;
        callList.clear();
        start();
    }


    /**
     * 设置推流配置参数
     */
    public void setStreamConfig(final String url) {
        callList.add(new Runnable() {
            @Override
            public void run() {
                EvilsLiveJni.setStreamConfig(url.getBytes());
            }
        });
    }

    /**
     * 发送一帧数据
     */
    public void sendStream(final int index, final byte[] data, final  int width, final int height) {
        callList.add(new Runnable() {
            @Override
            public void run() {
               EvilsLiveJni.sendStream(index, data, width, height);
            }
        });
    }


    /**
     * 发送一帧数据
     */
    public int startPushStream(final byte[] url) {
        callList.add(new Runnable() {
            @Override
            public void run() {
                Log.e("hujd", "run:  startPush");
                index = EvilsLiveJni.startPushStream(url);
            }
        });
        return 0;
    }
    /**
     * 发送一帧数据
     */
    public void stopPushStream(final int index) {
        callList.add(new Runnable() {
            @Override
            public void run() {
                Log.e("hujd", "run:  stopPush");
                EvilsLiveJni.stopPushStream(index);
            }
        });
    }

    public static JniManager getInstance() {
        return INSTANCE;
    }
    @Override
    public void run() {
        while (this.running) {
            checkCallList();
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {

            }
        }
    }

    private void checkCallList() {
        Runnable r;
        while ((r = this.callList.poll()) != null) {
            r.run();
        }
    }

    public int getIndex() {
        return index;
    }
}
