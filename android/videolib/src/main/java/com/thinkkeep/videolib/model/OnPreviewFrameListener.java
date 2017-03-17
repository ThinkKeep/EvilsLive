package com.thinkkeep.videolib.model;

/**
 * Created by jsson on 17/3/16.
 */
public interface OnPreviewFrameListener {
    /**
     * 预览一帧视频回调
     * @param data
     * @param width
     * @param height
     */
    void onPreviewFrameListener(byte[] data, int width, int height);
}
