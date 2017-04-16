package com.thinkkeep.videolib.model;

import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.thinkkeep.videolib.api.EvilsLiveStreamerConfig;
import com.thinkkeep.videolib.util.Defines;
import com.thinkkeep.videolib.jni.JniManager;

import java.util.List;

/**
 * API < 21 的调用类
 * Created by jason on 17/2/27.
 */

@SuppressWarnings("deprecation")
public class CameraOld implements CameraSupport {

    private static final String TAG = CameraOld.class.getName();
    private static final int DEFAULT_IMAGE_FORMAT = ImageFormat.NV21;
    private Camera camera;

    private OnPreviewFrameListener listener;


    static final class Size {
        final int withd;
        final int height;

        Size(int width, int height) {
            this.withd = width;
            this.height = height;
        }
    }

    private Camera.PreviewCallback callback = new Camera.PreviewCallback() {
        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
//            Log.e(TAG, "onPreviewFrame: " + data.length);
            int width = camera.getParameters().getPreviewSize().width;
            int height = camera.getParameters().getPreviewSize().height;
            int index = JniManager.getInstance().getIndex();
//            Log.e(TAG, "onPreviewFrame: "+ index);
            if (index >= 0) {
                Log.e(TAG, "onPreviewFrame: "+ index);

                //JniManager.getInstance().sendStream(index, data, width, height);
            }
            if (listener != null) {
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
    public int startPushStream() {
        if (config != null && this.camera != null) {
            String streamUrl = config.getStreamUrl();
            if (streamUrlValid(streamUrl)) {
                JniManager.getInstance().startPushStream(streamUrl.getBytes());
                camera.setPreviewCallback(callback);
            }
        }

        return 0;
    }

    private boolean streamUrlValid(String streamUrl) {
        return streamUrl != null && !("".equals(streamUrl));
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
            camera.startPreview();
        } catch (Exception e) {
            Log.e(TAG, "startPreview: error" + Log.getStackTraceString(e));
        }
    }

    @NonNull
    private Camera.Parameters setParameters() {
        Camera.Parameters parameters = camera.getParameters();
        Size size = choosePreviewSize(parameters);

        //TODO 设置预览方向
        camera.setDisplayOrientation(90);
        //TODO 设置拍照之后方向
        parameters.setRotation(90);

        parameters.setPreviewSize(size.withd, size.height);
        parameters.setPictureSize(size.withd, size.height);

        int imageFormat = chooseImageFormat(parameters);

        parameters.setPreviewFormat(imageFormat);


        return parameters;
    }

    private int chooseImageFormat(Camera.Parameters parameters) {
        if (config == null) {
            return DEFAULT_IMAGE_FORMAT;
        }

        List<Integer> list = parameters.getSupportedPreviewFormats();
        for (Integer val : list) {
            Log.e(TAG, "startPreview: val: " + val);
            if (val == config.getImageFormat()) {
                return val;
            }
        }
        return DEFAULT_IMAGE_FORMAT;
    }

    private Size choosePreviewSize(Camera.Parameters parameters) {
        Size size = null;
        if (config == null) {
            return new Size(Defines.EVIDEO_RESOLUTION.E640P.getWidth(), Defines.EVIDEO_RESOLUTION.E640P.getHeight());
        }

        Defines.EVIDEO_RESOLUTION videoResolution = config.getVideoResolution();
        if (videoResolution != null) {
                      int width = videoResolution.getWidth();
            int height = videoResolution.getHeight();
            List<Camera.Size> sizeList = parameters.getSupportedPreviewSizes();
            for (Camera.Size cur : sizeList) {
                Log.e(TAG, "size==" + cur.width + " " + cur.height);
                if (cur.width == width && cur.height == height) {
                    size = new Size(width, height);
                    break;
                }
            }
        }
        if (size == null) {
            size = new Size(Defines.EVIDEO_RESOLUTION.E640P.getWidth(), Defines.EVIDEO_RESOLUTION.E640P.getHeight());
        }

        return size;
    }

    @Override
    public void setOnPreviewFrameListener(OnPreviewFrameListener listener) {
        this.listener = listener;
    }

    @Override
    public void setStreamConfig(EvilsLiveStreamerConfig config) {
        this.config = config;
    }

    @Override
    public void stopPushStream() {
        if (camera == null) {
            return;
        }
        int index = JniManager.getInstance().getIndex();
        if (index >= 0 ) {
            JniManager.getInstance().stopPushStream(index);
        }
        camera.setPreviewCallback(null);
    }

    /**
     * 关闭Camera
     */
    @Override
    public void close() {
        if (camera == null) {
            return ;
        }

        camera.stopPreview();
        camera.release();
        camera = null;
    }
}
