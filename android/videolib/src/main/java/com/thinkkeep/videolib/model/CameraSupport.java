package com.thinkkeep.videolib.model;

import android.opengl.GLSurfaceView;

import com.thinkkeep.videolib.api.EvilsLiveStreamerConfig;

/**
 * 对Camera类封装
 * Created by jason on 17/2/27.
 */

public interface CameraSupport {

    /**
     * 打开Camera
     * @param cameraId 前置或后置
     * @return Camera实例
     */
    CameraSupport open(int cameraId);

    /**
     * 获取Camera 方向
     * @param cameraId 前置或后置
     */
    int getOrientation(int cameraId);

    /**
     * 设置预览View
     * @param surfaceView view
     */
    void setDisplayPreview(GLSurfaceView surfaceView);

    /**
     * 设置推流预览回调
     * @param listener listener
     */
    void setOnPreviewFrameListener(OnPreviewFrameListener listener);

    /**
     * 设置推流配置
     * @param config config
     */
    void setStreamConfig(EvilsLiveStreamerConfig config);
}
