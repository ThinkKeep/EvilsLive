package com.thinkkeep.videolib.jni;

import android.util.Log;

import com.thinkkeep.videolib.api.EvilsLiveStreamerConfig;
import com.thinkkeep.videolib.model.CameraOld;

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
        EvilsLiveJni.init();
        start();
    }

    /**
     *
     */
    public void createPushStream(final int protocol) {
        callList.add(new Runnable() {
            @Override
            public void run() {
                index = EvilsLiveJni.createPushStream(protocol);
            }
        });
    }

    /**
     * 设置推流配置参数
     */
    public void setStreamConfig(final int index, final int width, final int height, final int framerate,
                                final int bitrate, final boolean forcedI) {
        callList.add(new Runnable() {
            @Override
            public void run() {
                EvilsLiveJni.setStreamConfig(index, width, height, framerate, bitrate, forcedI);
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
    public int startPushStream(final CameraOld.Size size, final byte[] url, final int frameRate, final int bitRate) {
        callList.add(new Runnable() {
            @Override
            public void run() {
                Log.e("hujd", "run:  startPush");
                index = EvilsLiveJni.createPushStream(0);

                EvilsLiveJni.setStreamConfig(index, size.width, size.height, frameRate, bitRate, false);
                EvilsLiveJni.startPushStream(index, url);
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
