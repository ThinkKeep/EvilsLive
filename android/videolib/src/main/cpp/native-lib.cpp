//
// Created by jason on 17/1/11.
//

#include "native-lib.h"

jstring Java_com_thinkkeep_evils_jni_EvilsJni_testJni(JNIEnv* env, jobject instance) {
    return env->NewStringUTF("test Jni");
}

JNIEXPORT jstring JNICALL
Java_com_thinkkeep_evils_jni_EvilsJni_startPushStream(JNIEnv *env, jclass type) {

    // TODO
    return env->NewStringUTF("");
}

