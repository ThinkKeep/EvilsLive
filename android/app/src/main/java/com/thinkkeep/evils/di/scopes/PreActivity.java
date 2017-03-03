package com.thinkkeep.evils.di.scopes;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

import javax.inject.Scope;

/**
 * Created by jsson on 17/3/3.
 */

@Scope
@Retention(RetentionPolicy.RUNTIME)
public @interface PreActivity {
}
