package com.thinkkeep.videolib.activity;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.thinkkeep.videolib.R;
import com.thinkkeep.videolib.di.component.CameraComponent;
import com.thinkkeep.videolib.model.CameraSupport;

import javax.inject.Inject;

public class StreamActivity extends AppCompatActivity {

    @Inject
    CameraSupport cameraSupport;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_stream);

        CameraComponent cameraComponent = CameraComponent.Initializer.init(this);
//
        cameraComponent.inject(this);
    }

    private void initCameara() {

    }
}
