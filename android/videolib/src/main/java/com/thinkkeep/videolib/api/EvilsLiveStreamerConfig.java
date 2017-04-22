package com.thinkkeep.videolib.api;

import android.graphics.ImageFormat;

import com.thinkkeep.videolib.util.Defines;

import static com.thinkkeep.videolib.util.Defines.EvideoResolution.E640P;

/**
 * Created by jason on 17/3/16.
 */

public class EvilsLiveStreamerConfig {

    private static final EvilsLiveStreamerConfig DEFAULT_INSTANCE;

    static {
        DEFAULT_INSTANCE = new EvilsLiveStreamerConfig();
    }

    private String streamUrl;

    private Defines.EencodeMethod encodeMethod;

    private Defines.EvideoResolution videoresolution;

    private int imageFormat;
    private int previewFrameRate;
    private int encodeFrameRate;
    private Defines.EcameraFacing cameraFacing;


    private EvilsLiveStreamerConfig(Builder builder) {
        builder(builder);
    }

    private EvilsLiveStreamerConfig() {
        this.streamUrl = "";
        this.videoresolution = E640P;
        this.encodeMethod = Defines.EencodeMethod.SOFTWARE_ENCODE;
        this.imageFormat = ImageFormat.NV21;
        this.previewFrameRate = Defines.DEFAULT_FRAME_RATE;
        this.encodeFrameRate = Defines.DEFAULT_FRAME_RATE;
        this.cameraFacing = Defines.EcameraFacing.CAMERA_FACING_FRONT;
    }

    private void builder(Builder builder) {
        this.streamUrl = builder.streamUrl;
        this.encodeMethod = builder.encodeMethod;
        this.imageFormat = builder.imageFormat;
        this.videoresolution = builder.videoresolution;
        this.previewFrameRate = builder.previewFrameRate;
        this.encodeFrameRate = builder.encodeFrameRate;
        this.cameraFacing = builder.cameraFacing;
    }

    /**
     * 获取图像格式
     * @return
     */
    public int getImageFormat() {
        return imageFormat;
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
    public Defines.EencodeMethod getEncodeMethod() {
        return encodeMethod;
    }


    /**
     * 获取视频分辨率
     * @return resolution
     */
    public Defines.EvideoResolution getVideoResolution() {
        return videoresolution;
    }

    /**
     * 获取预览帧率
     * @return
     */
    public int getPreviewFrameRate() {
        return previewFrameRate;
    }

    /**
     * 获取视频编码帧率
     * @return framerate
     */
    public int getEncodeFrameRate() {
        return encodeFrameRate;
    }

    /**
     * 获取摄像头状态
     * @return 前置/后置
     */
    public Defines.EcameraFacing getCameraFacing() {
        return cameraFacing;
    }

    public static final class Builder {
        private String streamUrl;

        private Defines.EencodeMethod encodeMethod;

        private Defines.EvideoResolution videoresolution;

        private int imageFormat;

        private int previewFrameRate;
        private int encodeFrameRate;
        public Defines.EcameraFacing cameraFacing;

        private Builder() {
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
        public void setEncodeMethod(Defines.EencodeMethod encodeMethod) {
            this.encodeMethod = encodeMethod;
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

        /**
         * 设置视频预览帧率
         * @param previewFrameRate frameRate
         */
        public void setPreviewFrameRate(int previewFrameRate) {
            this.previewFrameRate = previewFrameRate;
        }

        /**
         * 设置视频编码帧率
         * @param encodeFrameRate frameRate
         */
        public void setEncodeFrameRate(int encodeFrameRate) {
            this.encodeFrameRate = encodeFrameRate;
        }

        /**
         * 设置预览摄像头
         * @param cameraFacing facing
         */
        public void setCameraFacing(Defines.EcameraFacing cameraFacing) {
            this.cameraFacing = cameraFacing;
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
        builder.previewFrameRate = DEFAULT_INSTANCE.previewFrameRate;
        builder.encodeFrameRate = DEFAULT_INSTANCE.encodeFrameRate;
        builder.cameraFacing = DEFAULT_INSTANCE.cameraFacing;
        return builder;
    }
}
