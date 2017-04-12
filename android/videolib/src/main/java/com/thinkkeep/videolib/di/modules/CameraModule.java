package com.thinkkeep.videolib.di.modules;

import android.content.Context;
import android.os.Build;

import com.thinkkeep.videolib.jni.JniManager;
import com.thinkkeep.videolib.model.CameraNew;
import com.thinkkeep.videolib.model.CameraOld;
import com.thinkkeep.videolib.model.CameraSupport;

import dagger.Module;
import dagger.Provides;

/**
 * Created by jason on 17/2/27.
 */

@Module
public class CameraModule {
    private Context context;

    public CameraModule(Context context) {
        this.context = context;
    }

    @Provides
    CameraSupport provideCameraSupport(){
        // FIXME: 17/4/9 目前只暂时不支持camera2 接口
//        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
//            return new CameraNew(context);
//        } else {
        JniManager.getInstance().init();
            return new CameraOld();
//        }
    }
}