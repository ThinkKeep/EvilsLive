package com.thinkkeep.videolib.api;

import android.graphics.ImageFormat;

import com.thinkkeep.videolib.util.Defines;

import static com.thinkkeep.videolib.util.Defines.EVIDEO_RESOLUTION.E640P;

/**
 * Created by jason on 17/3/16.
 */

public class EvilsLiveStreamerConfig {

    private static final EvilsLiveStreamerConfig DEFAULT_INSTANCE;

    static {
        DEFAULT_INSTANCE = new EvilsLiveStreamerConfig();
    }

    private int frameRate;

    private String streamUrl;

    private Defines.EENCODE_METHOD encodeMethod;

    private Defines.EVIDEO_RESOLUTION videoresolution;

    private boolean frontCamera;

    private int imageFormat;


    private EvilsLiveStreamerConfig(Builder builder) {
        builder(builder);
    }

    private EvilsLiveStreamerConfig() {
        this.frameRate = 20;
        this.frontCamera = false;
        this.streamUrl = "";
        this.videoresolution = E640P;
        this.encodeMethod = Defines.EENCODE_METHOD.SOFTWARE_ENCODE;
        this.imageFormat = ImageFormat.NV21;
    }

    private void builder(Builder builder) {
        this.frameRate = builder.frameRate;
        this.streamUrl = builder.streamUrl;
        this.encodeMethod = builder.encodeMethod;
        this.videoresolution = builder.videoresolution;
        this.frontCamera = builder.frontCamera;
    }

    /**
     * 获取图像格式
     * @return
     */
    public int getImageFormat() {
        return imageFormat;
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
    public Defines.EENCODE_METHOD getEncodeMethod() {
        return encodeMethod;
    }


    /**
     * 获取视频分辨率
     * @return resolution
     */
    public Defines.EVIDEO_RESOLUTION getVideoResolution() {
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

        private Defines.EENCODE_METHOD encodeMethod;

        private Defines.EVIDEO_RESOLUTION videoresolution;

        private boolean frontCamera;

        private int imageFormat;

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
         * 设置推流url
         * @param streamUrl url
         */
        public void setStreamUrl(String streamUrl) {
            this.streamUrl = streamUrl;
        }

        /**
         * 设置视频编码方式
         * @param encodeMethod 软编码/硬编码
         */
        public void setEncodeMethod(Defines.EENCODE_METHOD encodeMethod) {
            this.encodeMethod = encodeMethod;
        }

        /**
         * 设置推流帧率
         * @param frameRate rate
         */
        public void setFrameRate(int frameRate) {
            this.frameRate = frameRate;
        }

        /**
         * 设置图像格式
         * @param imageFormat imageFormat
         */
        public void setImageFormat(int imageFormat) {
            this.imageFormat = imageFormat;
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
        builder.imageFormat = DEFAULT_INSTANCE.imageFormat;
        return builder;
    }
}
