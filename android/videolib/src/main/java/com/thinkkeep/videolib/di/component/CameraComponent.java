package com.thinkkeep.videolib.di.component;

import android.content.Context;

import com.thinkkeep.videolib.activity.StreamActivity;
import com.thinkkeep.videolib.di.modules.CameraModule;
import com.thinkkeep.videolib.di.component.DaggerCameraComponent;

import dagger.Component;

/**
 * Created by jason on 17/2/27.
 */
@Component(modules = CameraModule.class)
public interface CameraComponent {
    void inject(StreamActivity streamActivity);

    final class Initializer {
        private Initializer() {

        } // No instance

        public static CameraComponent init(Context context) {
           return DaggerCameraComponent.builder()
                    .cameraModule(new CameraModule(context)).build();
        }
    }

}
