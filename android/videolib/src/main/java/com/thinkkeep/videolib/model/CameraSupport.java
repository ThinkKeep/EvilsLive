package com.thinkkeep.videolib.model;

/**
 * Created by jsson on 17/2/27.
 */

public interface CameraSupport {

    CameraSupport open(int cameraId);

    int getOrientation(int cameraId);
}
