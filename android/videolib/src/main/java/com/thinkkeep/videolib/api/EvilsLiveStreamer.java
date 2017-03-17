package com.thinkkeep.videolib.api;

import android.content.Context;
import android.opengl.GLSurfaceView;

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
    }

    /**
     * 设置预览界面
     * @param surfaceView view
     */
    public void setDisplayPreview(GLSurfaceView surfaceView) {
        cameraSupport.setDisplayPreview(surfaceView);
    }

    /**
     * 设置推流配置信息
     * @param config config
     */
    public void setStreamConfig(EvilsLiveStreamerConfig config) {
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
     * 开始推流
     */
    public void startStream() {

    }

    /**
     * 停止推流
     */
    public void stopStream() {

    }
}
