//
// Created by jason on 17/1/11.
//

#ifndef ANDROID_NATIVE_LIB_H
#define ANDROID_NATIVE_LIB_H

#include "jni.h"


extern "C"{
    jstring Java_com_thinkkeep_evils_jni_EvilsJni_testJni(JNIEnv* env, jobject instance);

    JNIEXPORT jstring JNICALL
        Java_com_thinkkeep_evils_jni_EvilsJni_startPushStream(JNIEnv *env, jclass type);
}
#endif //ANDROID_NATIVE_LIB_H
