package com.thinkkeep.evils;

import android.app.Application;

import com.thinkkeep.evils.di.component.AppComponent;
import com.thinkkeep.evils.di.modules.AppModule;
import com.thinkkeep.evils.di.component.DaggerAppComponent;

/**
 * Created by jason on 17/3/3.
 */

public class App extends Application {
    private AppComponent mAppComponent;

    @Override
    public void onCreate() {
        super.onCreate();

        mAppComponent = DaggerAppComponent.builder().appModule(new AppModule(this)).build();
    }

    public AppComponent getAppComponent() {
        return mAppComponent;
    }
}
