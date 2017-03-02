package com.thinkkeep.videolib.di.modules;

import android.content.Context;
import android.os.Build;

import com.thinkkeep.videolib.model.CameraNew;
import com.thinkkeep.videolib.model.CameraOld;
import com.thinkkeep.videolib.model.CameraSupport;

import dagger.Module;
import dagger.Provides;

/**
 * Created by jsson on 17/2/27.
 */

@Module
public class CameraModule {
    private Context context;

    public CameraModule(Context context) {
        this.context = context;
    }

    @Provides
    CameraSupport provideCameraSupport(){
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            return new CameraNew(context);
        } else {
            return new CameraOld();
        }
    }
}