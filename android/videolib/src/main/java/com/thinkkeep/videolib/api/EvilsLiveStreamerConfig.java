package com.thinkkeep.videolib.api;

/**
 * Created by jason on 17/3/16.
 */

public class EvilsLiveStreamerConfig {
    public enum EENCODE_METHOD {
        SOFTWARE_ENCODE,
        HARDWARE_ENCODE,
        ;
    }

    public enum EVIDEO_RESOLUTION {
        E360P(480, 360),
        E480P(858, 480),
        E540P(960, 540),
        E720P(1080, 720),
        ;
        private final int width;
        private final int height;

        public int getWidth() {
            return width;
        }

        public int getHeight() {
            return height;
        }

        EVIDEO_RESOLUTION(int width, int height) {
            this.width = width;
            this.height = height;
        }
    }

    private static final EvilsLiveStreamerConfig DEFAULT_INSTANCE;

    static {
        DEFAULT_INSTANCE = new EvilsLiveStreamerConfig();
    }

    private int frameRate;

    private String streamUrl;

    private EENCODE_METHOD encodeMethod;

    private EVIDEO_RESOLUTION videoresolution;

    private boolean frontCamera;


    private EvilsLiveStreamerConfig(Builder builder) {
        builder(builder);
    }

    private EvilsLiveStreamerConfig() {
        this.frameRate = 20;
        this.frontCamera = false;
        this.streamUrl = "";
        this.videoresolution = EVIDEO_RESOLUTION.E360P;
        this.encodeMethod = EENCODE_METHOD.SOFTWARE_ENCODE;
    }

    private void builder(Builder builder) {
        this.frameRate = builder.frameRate;
        this.streamUrl = builder.streamUrl;
        this.encodeMethod = builder.encodeMethod;
        this.videoresolution = builder.videoresolution;
        this.frontCamera = builder.frontCamera;
    }

    /**
     * 获取推流帧率
     * @return rate
     */
    public int getFrameRate() {
        return frameRate;
    }


    /**
     * 获取推流url
     * @return url
     */
    public String getStreamUrl() {
        return streamUrl;
    }

    /**
     * 获取视频编码方式
     * @return resolution
     */
    public EENCODE_METHOD getEncodeMethod() {
        return encodeMethod;
    }


    /**
     * 获取视频分辨率
     * @return resolution
     */
    public EVIDEO_RESOLUTION getVideoResolution() {
        return videoresolution;
    }


    /**
     * 获取是否是前置摄像头
     * @return true/false
     */
    public boolean getFrontCamera() {
        return frontCamera;
    }


    public static final class Builder {

        private int frameRate;

        private String streamUrl;

        private EENCODE_METHOD encodeMethod;

        private EVIDEO_RESOLUTION videoresolution;

        private boolean frontCamera;

        private Builder() {
        }

        /**
         * 设置前置摄像头
         * @param frontCamera
         */
        public void setFrontCamera(boolean frontCamera) {
            this.frontCamera = frontCamera;
        }


        /**
         * 设置视频编码方式
         * @param encodeMethod 软编码/硬编码
         */
        public void setEncodeMethod(EENCODE_METHOD encodeMethod) {
            this.encodeMethod = encodeMethod;
        }

        /**
         * 设置推流帧率
         * @param frameRate rate
         */
        public void setFrameRate(int frameRate) {
            this.frameRate = frameRate;
        }


        public static Builder newBuilder() {
            return DEFAULT_INSTANCE.toBuilder();
        }

        public EvilsLiveStreamerConfig build() {
            return new EvilsLiveStreamerConfig(this);
        }

        public Builder mergeFrom(EvilsLiveStreamerConfig config) {
            return null;
        }
    }

    private Builder toBuilder() {
//        return this == DEFAULT_INSTANCE ? new Builder() : new Builder().mergeFrom(this);
        Builder builder = new Builder();
        builder.streamUrl = DEFAULT_INSTANCE.streamUrl;
        builder.encodeMethod = DEFAULT_INSTANCE.encodeMethod;
        builder.videoresolution = DEFAULT_INSTANCE.videoresolution;
        return builder;
    }
}
