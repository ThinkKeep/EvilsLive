package com.thinkkeep.videolib.model;

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
}
