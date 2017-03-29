package com.thinkkeep.videolib.jni;

import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * Created by jason on 17/3/29.
 */

public class JniManager extends Thread {
    private static final JniManager INSTANCE = new JniManager();
    private final ConcurrentLinkedQueue<Runnable> callList = new ConcurrentLinkedQueue<>();
    private boolean running;

    public void init() {
        if (this.running) {
            return;
        }

        this.running = true;
        callList.clear();
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
    public void sendStream(final byte[] data) {
        callList.add(new Runnable() {
            @Override
            public void run() {
               EvilsLiveJni.sendStream(data);
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
}
