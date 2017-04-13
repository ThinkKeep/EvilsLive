package com.thinkkeep.videolib.api;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.SurfaceView;

import com.thinkkeep.videolib.di.component.CameraComponent;
import com.thinkkeep.videolib.model.CameraSupport;
import com.thinkkeep.videolib.model.OnPreviewFrameListener;

import javax.inject.Inject;

/**
 * 推流
 * Created by jason on 17/3/16.
 */
public class EvilsLiveStreamer {

    private final Context context;
    @Inject
    CameraSupport cameraSupport;
    private EvilsLiveStreamerConfig streamerConfig;


    public EvilsLiveStreamer(Context context) {
        this.context = context;
        CameraComponent cameraComponent = CameraComponent.Initializer.init(context);
//
        cameraComponent.inject(this);

        //init default config
        EvilsLiveStreamerConfig.Builder builder = EvilsLiveStreamerConfig.Builder.newBuilder();
        setStreamConfig(builder.build());

    }

    /**
     * 设置预览界面
     * @param surfaceView view
     */
    public void setDisplayPreview(SurfaceView surfaceView) {
        cameraSupport.setDisplayPreview(surfaceView);
    }

    /**
     * 设置推流配置信息
     * @param config config
     */
    public void setStreamConfig(EvilsLiveStreamerConfig config) {
//        EvilsLiveStreamerConfig.Builder builder = EvilsLiveStreamerConfig.Builder.newBuilder();
        cameraSupport.setStreamConfig(config);
    }


    /**
     * 设置每一帧回调
     * @param listener listener
     */
    public void setOnPreviewFrameListener(OnPreviewFrameListener listener) {
        cameraSupport.setOnPreviewFrameListener(listener);
    }

    /**
     * 开始预览
     */
    public void startPreview(int cameraId) {
        cameraSupport.open(cameraId);
    }

    /**
     * 停止预览
     */
    public void stopPreview() {
        cameraSupport.close();
    }

    /**
     * 开始推流
     * @return 开启结果
     */
    public int startStream() {
        return cameraSupport.startPushStream();
    }

    /**
     * 停止推流
     */
    public void stopStream() {
        cameraSupport.stopPushStream();
    }
}
