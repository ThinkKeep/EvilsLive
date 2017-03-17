package com.thinkkeep.videolib.model;

import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.opengl.GLSurfaceView;
import android.util.Log;

import com.thinkkeep.videolib.api.EvilsLiveStreamerConfig;

import java.util.List;

/**
 * API < 23 的调用类
 * Created by jason on 17/2/27.
 */

@SuppressWarnings("deprecation")
public class CameraOld implements CameraSupport {

    private static final String TAG = CameraOld.class.getName();
    private Camera camera;

    private OnPreviewFrameListener listener;

    private Camera.PreviewCallback callback = new Camera.PreviewCallback() {
        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
            Log.d(TAG, "onPreviewFrame: " + data.length);
            if (listener != null) {
                int width = camera.getParameters().getPictureSize().width;
                int height = camera.getParameters().getPictureSize().height;
                listener.onPreviewFrameListener(data, width, height);
            }
        }
    };
    private EvilsLiveStreamerConfig config;

    @Override
    public CameraSupport open(final int cameraId) {
        this.camera = Camera.open(cameraId);
        startPreview();
        return this;
    }

    @Override
    public int getOrientation(final int cameraId) {
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(cameraId, info);
        return info.orientation;
    }

    @Override
    public void setDisplayPreview(GLSurfaceView surfaceView) {

    }


    private void startPreview() {
        Camera.Parameters parameters = camera.getParameters();
        if (config != null) {
            EvilsLiveStreamerConfig.EVIDEO_RESOLUTION videoresolution = config.getVideoresolution();
            int width = videoresolution.getWidth();
            int height = videoresolution.getHeigth();
            parameters.setPreviewSize(width, height);
            parameters.setPictureSize(width, height);
        }
        parameters.setPreviewFormat(ImageFormat.NV21);

        parameters.setRotation(90);

        List<Integer> list = parameters.getSupportedPictureFormats();
        for (Integer val : list) {
            Log.d(TAG, "startPreview: val: " + val);
        }

        camera.setParameters(parameters);
        camera.setPreviewCallback(callback);

    }

    @Override
    public void setOnPreviewFrameListener(OnPreviewFrameListener listener) {
        this.listener = listener;
    }

    @Override
    public void setStreamConfig(EvilsLiveStreamerConfig config) {
        this.config = config;
    }

    /**
     * 关闭Camera
     */
    public void close() {
        if (camera == null) {
            return ;
        }

        camera.setPreviewCallback(null);
        camera.stopPreview();
        camera.release();
        camera = null;
    }
}
