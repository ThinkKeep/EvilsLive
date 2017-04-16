//
// Created by jason on 17/1/11.
//

#ifndef ANDROID_NATIVE_LIB_H
#define ANDROID_NATIVE_LIB_H

#include "jni.h"


extern "C"{
    jstring Java_com_thinkkeep_videolib_jni_EvilsLiveJni_testJni(JNIEnv* env, jobject instance);

    JNIEXPORT void JNICALL
        Java_com_thinkkeep_videolib_jni_EvilsLiveJni_init(JNIEnv* env, jclass jc);

    JNIEXPORT void JNICALL
        Java_com_thinkkeep_videolib_jni_EvilsLiveJni_sendStream(JNIEnv *env, jclass jc,
                                                                jint index, jbyteArray j_data,
                                                                jint width_, jint height_);

    JNIEXPORT void JNICALL
        Java_com_thinkkeep_videolib_jni_EvilsLiveJni_setStreamConfig(JNIEnv *env, jclass jc,
                                                             jbyteArray url_);

    jint
        Java_com_thinkkeep_videolib_jni_EvilsLiveJni_startPushStream(JNIEnv *env, jclass jc,
                                                                     jbyteArray url_);
    JNIEXPORT void JNICALL
        Java_com_thinkkeep_videolib_jni_EvilsLiveJni_stopPushStream(JNIEnv *env, jclass jc,
                                                                    jint hanlder);
}
#endif //ANDROID_NATIVE_LIB_H
