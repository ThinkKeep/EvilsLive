package com.thinkkeep.videolib.model;

import android.Manifest;
import android.annotation.TargetApi;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.ImageReader;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.support.annotation.NonNull;
import android.support.annotation.RequiresApi;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.util.Range;
import android.util.Size;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.thinkkeep.videolib.api.EvilsLiveStreamerConfig;

import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

/**
 * API >= 21 调用类
 * Created by jason on 17/2/27.
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class CameraNew implements CameraSupport {
    private static final String TAG = CameraNew.class.getName();
    private final Context context;
    private CameraDevice mCamera;
    private CameraManager manager;
    private SurfaceView surfaceView;
    private EvilsLiveStreamerConfig config;

    private CaptureRequest.Builder mPreviewBuilder;

    private ImageReader mImageReader;

    /**
     *
     * A {@link Semaphore} to prevent the app from exiting before closing the mCamera.
     */
    private Semaphore cameraOpenCloseLock = new Semaphore(1);
    private boolean mIsInPreview;

    private Handler mHandler;

    private OnPreviewFrameListener listener;
    private ImageReader.OnImageAvailableListener mOnImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader imageReader) {
            Log.e(TAG, "onImageAvailable: xx");
//            Image image = imageReader.acquireNextImage();
//            ByteBuffer buffer = image.getPlanes()[0].getBuffer();
//            byte[] data = new byte[buffer.remaining()];
//            buffer.get(data);
//            image.close();
//
//            if (listener != null) {
//                listener.onPreviewFrameListener(data, 754, 360);
//            }
        }
    };
    private HandlerThread handlerThread;
    private Range<Integer>[] mFps;


    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public CameraNew(final Context context) {
        this.context = context;
        this.manager = (CameraManager) context.getSystemService(Context.CAMERA_SERVICE);
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @Override
    public CameraSupport open(final int cameraId) {
        try {
            startCamera2Thread();

            if (mIsInPreview) {
                reconfigureCamera();
                return this;
            }

            mIsInPreview = true;


            if (!cameraOpenCloseLock.tryAcquire(2500, TimeUnit.MICROSECONDS)) {
                throw new RuntimeException("Time out waiting to lock camera opening.");
            }

            printSupportFormats();

            String[] cameraIds = manager.getCameraIdList();

            CameraDevice.StateCallback deviceCallback = new CameraDevice.StateCallback() {
                @Override
                public void onOpened(@NonNull CameraDevice camera) {
                    cameraOpenCloseLock.release();
                    mCamera = camera;
                    createCameraPreviewSession();
                }

                @Override
                public void onDisconnected(@NonNull CameraDevice camera) {
                    Log.d(TAG, "onDisconnected:  open");
                    cameraOpenCloseLock.release();
                    camera.close();
                    mCamera = null;
                }

                @Override
                public void onError(@NonNull CameraDevice camera, int error) {
                    cameraOpenCloseLock.release();
                    camera.close();
                    mCamera = null;
                }
            };
            if (ActivityCompat.checkSelfPermission(context, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
                Log.d(TAG, "open: jjjj");
            }
            manager.openCamera(cameraIds[cameraId], deviceCallback, mHandler);

        } catch (Exception e) {
            Log.e(TAG, "open: " + Log.getStackTraceString(e));
        }
        return this;
    }

    @Override
    public int startPushStream() {
        return 0;
    }

    private void printSupportFormats() {
        try {
            for (String cameraId : manager.getCameraIdList()) {
                CameraCharacteristics cameraCharacteristics = manager.getCameraCharacteristics(cameraId);
                StreamConfigurationMap map = cameraCharacteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                if (map != null) {
                    int[] outputFormats = map.getOutputFormats();
                    Log.e(TAG, "printSupportFormats: imageFormat " + Arrays.toString(outputFormats));
                    Size[] outputSizes = map.getOutputSizes(ImageFormat.JPEG);
                    Log.e(TAG, "printSupportFormats: size: " + Arrays.toString(outputSizes));
                    mImageReader = ImageReader.newInstance(640, 480, ImageFormat.YUV_420_888, 2);
                    mImageReader.setOnImageAvailableListener(mOnImageAvailableListener, mHandler);
                }

                mFps = cameraCharacteristics.get(CameraCharacteristics.CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);
                Log.e(TAG, "printSupportFormats: fps " + Arrays.toString(mFps));
            }
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private void startCamera2Thread() {
        handlerThread = new HandlerThread("Camera2");
        handlerThread.start();
        mHandler = new Handler(handlerThread.getLooper());
    }

    private void stopCamera2Thread() {
        if (handlerThread != null) {
            handlerThread.quitSafely();
            try {
                handlerThread.join();
                handlerThread = null;
                mHandler = null;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private void reconfigureCamera() {
        if(mCamera != null) {
            try {
                mCaptureSession.stopRepeating();

                mPreviewBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER, CameraMetadata.CONTROL_AF_TRIGGER_CANCEL);
                mCaptureSession.capture(mPreviewBuilder.build(), mPreCaptureCallback, mHandler);

                doPreviewConfiguration();
            } catch (CameraAccessException e) {
                e.printStackTrace();
            }
        }
    }

    private void doPreviewConfiguration() {
        if (mCamera == null) {
            return;
        }

        try {
//            mPreviewBuilder = mCamera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
//            mPreviewBuilder.addTarget(surfaceView.getHolder().getSurface());
//            mPreviewBuilder.addTarget(mImageReader.getSurface());
            mPreviewBuilder.set(CaptureRequest.CONTROL_MODE, CameraMetadata.CONTROL_MODE_AUTO);
//            mPreviewBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_AUTO);
//            mPreviewBuilder.set(CaptureRequest.FLASH_MODE, CaptureRequest.FLASH_MODE_OFF);
//            mPreviewBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_OFF);

            mPreviewBuilder.set(CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, mFps[mFps.length -1 ]);
            mCaptureSession.setRepeatingRequest(mPreviewBuilder.build(), mPreCaptureCallback, mHandler);

        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private CameraCaptureSession.CaptureCallback mPreCaptureCallback
            = new CameraCaptureSession.CaptureCallback() {

        private void process(CaptureResult result) {

        }

        @Override
        public void onCaptureProgressed(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request,
                                        @NonNull CaptureResult partialResult) {
            process(partialResult);
        }

        @Override
        public void onCaptureCompleted(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request,
                                       @NonNull TotalCaptureResult result) {
            process(result);
        }

    };

    private void createCameraPreviewSession() {
        if (mCamera == null) {
            return;
        }


        SurfaceHolder holder = surfaceView.getHolder();
//        if (config != null) {
//            holder.setFixedSize(640, 480);
//        }
        Surface surface = holder.getSurface();


        List<Surface> surfaceList = Arrays.asList(surface, mImageReader.getSurface());

        try {
            mPreviewBuilder = mCamera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            mPreviewBuilder.addTarget(surface);

            mCamera.createCaptureSession(surfaceList, sessionCallback, mHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private CameraCaptureSession mCaptureSession;

    private CameraCaptureSession.StateCallback sessionCallback = new CameraCaptureSession.StateCallback() {
        @Override
        public void onConfigured(@NonNull CameraCaptureSession cameraCaptureSession) {
            mCaptureSession = cameraCaptureSession;
            doPreviewConfiguration();
        }

        @Override
        public void onConfigureFailed(@NonNull CameraCaptureSession cameraCaptureSession) {

        }
    };

    @Override
    public int getOrientation(final int cameraId) {
        try {
            String[] cameraIds = manager.getCameraIdList();
            CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraIds[cameraId]);
            return characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);
        } catch (CameraAccessException e) {
            // TODO handle
            return 0;
        }
    }

    @Override
    public void setDisplayPreview(SurfaceView surfaceView) {
        this.surfaceView = surfaceView;
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

    }

    @Override
    public void close() {
        mIsInPreview = false;
        stopCamera2Thread();
        closeCamera();
    }

    private void closeCamera() {
        try {
            cameraOpenCloseLock.acquire();
            if (mCaptureSession != null) {
                try {
                    mCaptureSession.stopRepeating();
                    mCaptureSession.abortCaptures();
                } catch (CameraAccessException e) {
                    e.printStackTrace();
                }
                mCaptureSession.close();
                mCaptureSession = null;
            }
            if (mCamera != null) {
                mCamera.close();
                mCamera = null;
            }
            if (mImageReader != null) {
                mImageReader.close();
                mImageReader = null;
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            cameraOpenCloseLock.release();
        }
    }
}