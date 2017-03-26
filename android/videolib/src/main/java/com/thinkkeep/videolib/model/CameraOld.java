package com.thinkkeep.videolib.model;

import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.thinkkeep.videolib.api.EvilsLiveStreamerConfig;

import java.util.List;

/**
 * API < 21 的调用类
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
    private SurfaceView surfaceView;

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
    public void setDisplayPreview(SurfaceView surfaceView) {
        this.surfaceView = surfaceView;
    }


    private void startPreview() {
        try {

            if (surfaceView != null) {
                SurfaceHolder surfaceHolder = surfaceView.getHolder();
//                surfaceHolder.setFixedSize(720, 480);
//                surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
                camera.setPreviewDisplay(surfaceHolder);

            }

            Camera.Parameters parameters = setParameters();

            camera.setParameters(parameters);
            camera.setPreviewCallback(callback);
            camera.startPreview();
        } catch (Exception e) {
            Log.e(TAG, "startPreview: error" + Log.getStackTraceString(e));
        }
    }

    @NonNull
    private Camera.Parameters setParameters() {
        Camera.Parameters parameters = camera.getParameters();
        if (config != null) {
            EvilsLiveStreamerConfig.EVIDEO_RESOLUTION videoresolution = config.getVideoResolution();
            // TODO: 17/3/26
            int width = videoresolution.getWidth();
            int height = videoresolution.getHeight();
//            parameters.setPreviewSize(width, height);
        }

        //设置预览方向
        camera.setDisplayOrientation(90);
        //设置拍照之后方向
        parameters.setRotation(90);

        // TODO: 17/3/26 再来先写死
        parameters.setPreviewSize(720, 480);
        parameters.setPictureSize(720, 480);
        parameters.setPreviewFormat(ImageFormat.NV21);

        List<Integer> list = parameters.getSupportedPreviewFormats();
        for (Integer val : list) {
            Log.d(TAG, "startPreview: val: " + val);
        }

        List<Camera.Size> sizeList = parameters.getSupportedPreviewSizes();
        if (sizeList.size() > 1) {
            for (Camera.Size cur : sizeList) {
                Log.d(TAG, "size==" + cur.width + " " + cur.height);
            }
        }
        return parameters;
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
    @Override
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
