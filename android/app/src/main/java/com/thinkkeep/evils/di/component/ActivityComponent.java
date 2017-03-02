package com.thinkkeep.evils.di.component;

/**
 * Created by jsson on 17/3/1.
 */

import android.app.Activity;

import com.thinkkeep.evils.di.modules.ActivityModule;

import dagger.Component;

/**
 *
 * Created by niuxiaowei on 16/3/20.
 */
@Component(modules = {ActivityModule.class})
public interface ActivityComponent {

    Activity getActivity();
}
