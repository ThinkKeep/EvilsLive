package com.thinkkeep.videolib.model;

import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;

import com.thinkkeep.videolib.api.EvilsLiveStreamerConfig;
import com.thinkkeep.videolib.jni.EvilsLiveJni;
import com.thinkkeep.videolib.jni.JniManager;
import com.thinkkeep.videolib.util.Defines;

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
    private Size size;
    private int cameraId;
    private boolean isStartStream;


    public static final class Size {
        public final int width;
        public final int height;

        Size(int width, int height) {
            this.width = width;
            this.height = height;
        }
    }

//    static final int MAX_SIZE = 64;//must 2 mul
//    RingBuffer mRingBuffer;
//    void initRingBuffer(){
//        mRingBuffer = new RingBuffer(MAX_SIZE, mVideoFrame.capacity());
//    }

    private Camera.PreviewCallback callback = new Camera.PreviewCallback() {
        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
//            Log.e(TAG, "onPreviewFrame: " + data.length);
            int width = camera.getParameters().getPreviewSize().width;
            int height = camera.getParameters().getPreviewSize().height;
            int index = JniManager.getInstance().getIndex();
            Log.e(TAG, "hujd onPreviewFrame: "+ index);

            if (index >= 0) {
                //Log.e(TAG, "onPreviewFrame: "+ index);
                EvilsLiveJni.sendStream(index, data, width, height);
            }
            if (listener != null) {
                listener.onPreviewFrameListener(data, width, height);
            }
        }
    };
    private EvilsLiveStreamerConfig config;
    private SurfaceView surfaceView;



    @Override
    public CameraSupport open() {
        cameraId = getCameraId();
        this.camera = Camera.open(cameraId);

        startPreview();
        return this;
    }

    private int getCameraId() {
        int cameraId = Defines.EcameraFacing.CAMERA_FACING_FRONT.index;
        if (config != null) {
            Defines.EcameraFacing cameraFacing = config.getCameraFacing();
            if (cameraFacing != null) {
                cameraId = cameraFacing.index;
            }
        }
        return cameraId;
    }

    @Override
    public int startPushStream() {
        if (config != null && this.camera != null) {
            String streamUrl = config.getStreamUrl();
            if (streamUrlValid(streamUrl)) {
                JniManager.getInstance().startPushStream(size, streamUrl.getBytes(), 15, 512);
                camera.setPreviewCallback(callback);
                isStartStream = true;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        while (isStartStream) {
                            int index = JniManager.getInstance().getIndex();
                            try {
                                Thread.sleep(3000);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            JniManager.getInstance().setStreamConfig(index, size.width, size.height, 15, 512, true);
                        }
                    }
                }).start();
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
            Camera.Parameters parameters = setParameters();
            camera.setParameters(parameters);
            if (surfaceView != null) {
                Camera.Size previewSize = parameters.getPreviewSize();
                int previewWidth = previewSize.width;
                int previewHeight = previewSize.height;
                //fixed 预览画面中物体被拉伸变形, 因为旋转90度
                ViewGroup.LayoutParams layoutParams = surfaceView.getLayoutParams();
                layoutParams.width = previewHeight;
                layoutParams.height = previewWidth;
                SurfaceHolder surfaceHolder = surfaceView.getHolder();
                surfaceHolder.setSizeFromLayout();
                //设置分辨率
//                surfaceHolder.setFixedSize(previewWidth, previewHeight);
//                surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
                camera.setPreviewDisplay(surfaceHolder);

            }

            camera.startPreview();
        } catch (Exception e) {
            Log.e(TAG, "startPreview: error" + Log.getStackTraceString(e));
        }
    }

    @NonNull
    private Camera.Parameters setParameters() {
        Camera.Parameters parameters = camera.getParameters();
        size = choosePreviewSize(parameters);

        setPreviewFrameRate(parameters);

        //设置预览方向
        //if (cameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
            camera.setDisplayOrientation(90);
        //}
        //TODO 设置拍照之后方向
        parameters.setRotation(90);

        parameters.setPreviewSize(size.width, size.height);
        parameters.setPictureSize(size.width, size.height);

        int imageFormat = chooseImageFormat(parameters);

        parameters.setPreviewFormat(imageFormat);


        return parameters;
    }

    private void setPreviewFrameRate(Camera.Parameters parameters) {
        int frameRate = Defines.DEFAULT_FRAME_RATE;
        if (config != null ) {
            int previewFrameRate = config.getPreviewFrameRate();
            if (previewFrameRate >= 0) {
                frameRate = previewFrameRate;
            }
        }
//        List<int[]> supportedPreviewFpsRange = parameters.getSupportedPreviewFpsRange();
//        Log.d(TAG, "setPreviewFrameRate: hujd fps: " + supportedPreviewFpsRange);
//        for (int[] arg : supportedPreviewFpsRange) {
//            for (int k = 0; k < arg.length; k++) {
//                Log.d(TAG, "setPreviewFrameRate: hujd fps: " + arg[k]);
//            }
//        }
        parameters.setPreviewFpsRange( 25 * 1000, 25 * 1000);
//        parameters.setPreviewFrameRate(frameRate);
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
            return new Size(Defines.EvideoResolution.E640P.getWidth(), Defines.EvideoResolution.E640P.getHeight());
        }

        Defines.EvideoResolution videoResolution = config.getVideoResolution();
        if (videoResolution != null) {
            int width = videoResolution.getWidth();
            int height = videoResolution.getHeight();
            List<Camera.Size> sizeList = parameters.getSupportedPreviewSizes();
            Camera.Size optimalSize = getOptimalSize(sizeList,  Math.max(width, height), Math.min(width, height));
            size = new Size(optimalSize.width, optimalSize.height);
        }
        if (size == null) {
            size = new Size(Defines.EvideoResolution.E640P.getWidth(), Defines.EvideoResolution.E640P.getHeight());
        }

        return size;
    }

    private Camera.Size getOptimalSize(@NonNull List<Camera.Size> sizes, int width, int height) {
        final double ASPECT_TOLERANCE = 0.1;

        assert(width == 0);

        double targetRatio = (double) height / width;
        Camera.Size optimalSize = null;
        double minDiff = Double.MIN_VALUE;

        int targetHeight = height;

        for (Camera.Size size : sizes) {
            double ratio = (double) size.width /size.height;
            if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE ) {
                continue;
            }
            if (Math.abs(size.height - targetHeight) < minDiff) {
                optimalSize = size;
                minDiff = Math.abs(size.height = targetHeight);
            }
        }

        if (optimalSize == null) {
            minDiff = Double.MIN_VALUE;
            for (Camera.Size size : sizes) {
                if (Math.abs(size.height - targetHeight) < minDiff) {
                    optimalSize = size;
                    minDiff = Math.abs(size.height - targetHeight);
                }
            }
        }

        return optimalSize;
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
        isStartStream = false;

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
