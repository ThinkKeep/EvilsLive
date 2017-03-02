package com.thinkkeep.evils.di.modules;

/**
 * Created by jsson on 17/3/1.
 */

import android.app.Activity;

import dagger.Module;
import dagger.Provides;

/**
 * 提供baseactivity的module
 * Created by niuxiaowei on 16/3/20.
 */
@Module
public class ActivityModule {

    private final Activity activity;
    public ActivityModule(Activity activity){
        this.activity = activity;
    }

    @Provides
    public Activity provideActivity(){
        return activity;
    }



}
