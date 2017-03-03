package com.thinkkeep.evils.di.component;

import android.content.Context;

import com.thinkkeep.evils.di.modules.AppModule;

import javax.inject.Singleton;

import dagger.Component;

/**
 * Created by jsson on 17/3/3.
 */

@Singleton
@Component(modules = {AppModule.class})
public interface AppComponent {

    Context getContext();
}
