//
// Created by jason on 17/1/11.
//

#include "native-lib.h"
#include <stdlib.h>
#include <evils_live_api.h>

#include "log.h"

char *ja2c(JNIEnv *pEnv, jbyteArray pArray, int *pInt);

///------ java call jni
jstring Java_com_thinkkeep_videolib_jni_EvilsLiveJni_testJni(JNIEnv* env, jobject instance) {
    return env->NewStringUTF("test Jni");
}

JNIEnv *gJniEnv = NULL;

jclass gJniJc = NULL;

JNIEXPORT void JNICALL
Java_com_thinkkeep_videolib_jni_EvilsLiveJni_init(JNIEnv *env, jclass jc) {
    // TODO
    gJniEnv = env;
    gJniJc = jc;
}

/**
 * Trans jbyteArray to char
 */
JNIEXPORT char *ja2c(JNIEnv *pEnv, jbyteArray pArray, int *pSize) {
    *pSize = 0;
    if (pArray == NULL) {
        return NULL;
    }

    int len = pEnv->GetArrayLength(pArray);
    if (len < 1) {
        return NULL;
    }

    char *src = (char *)pEnv->GetByteArrayElements(pArray, JNI_FALSE);
    if (!src) {
        log_error("GetByteArrayElements error, byte_array len:%d", len);
        return NULL;
    }

    char *buf = new char[len + 1];

    memcpy(buf, src, (size_t) len);
    buf[len] = 0;
    *pSize = len;

    pEnv->ReleaseByteArrayElements(pArray, (jbyte *)src, 0);
    return buf;
}

class  AutoFree {
public:
    AutoFree(char *mem) {
        this->mem = mem;
    }

    ~AutoFree() {
        if (this->mem != NULL) {
            delete [] this->mem;
            this->mem = NULL;
        }
    }
private:
    char *mem;
};

void JNICALL
Java_com_thinkkeep_videolib_jni_EvilsLiveJni_sendStream(JNIEnv *env, jclass jc, jint index,
                                                        jbyteArray j_data, jint width_, jint height_) {
    int size = 0;
    char *data = (char *)env->GetByteArrayElements(j_data, JNI_FALSE);
    if (NULL == data) {
        log_error("GetByteArrayElements failed");
        return;
    }
    size = env->GetArrayLength(j_data);
    if (size <= 0) {
        log_error("GetArrayLength %d <= 0", size);
        return;
    }
    //TODO

    int res = evils_live_send_yuv420(index, data, size, width_, height_);

    log_error("evils_live_send_yuv420 size %d width %d height %d res %d", size, width_, height_, res);

    env->ReleaseByteArrayElements(j_data, (jbyte *)data, 0);

}


JNIEXPORT void JNICALL
Java_com_thinkkeep_videolib_jni_EvilsLiveJni_setStreamConfig(JNIEnv *env, jclass jc,
                                                             jbyteArray url_) {

    jbyte *url = env->GetByteArrayElements(url_, NULL);
    // TODO

    env->ReleaseByteArrayElements(url_, url, 0);
}

jint Java_com_thinkkeep_videolib_jni_EvilsLiveJni_startPushStream(JNIEnv *env, jclass jc,
                                                             jbyteArray url_) {
    int size = 0;
    char *url = ja2c(env, url_, &size);
    AutoFree afdata(url);
    // TODO
    log_error("url fff hujd");
    evils_live_init();
    int index = evils_live_start_push_stream(0, url);
    log_error("url fff hujd %s, %d", url, index);

    return index;
}

JNIEXPORT void JNICALL
Java_com_thinkkeep_videolib_jni_EvilsLiveJni_stopPushStream(JNIEnv *env, jclass jc,
                                                            jint hanlder) {
    log_error("url hujd");

    // TODO
//    evils_live_stop_push_stream(hanlder);
//    evils_live_destory();
}


//------------- c call jni ---------------------

void CheckJNIException(JNIEnv *env, const char *name) {
    if(env->ExceptionCheck()) {
        env->ExceptionClear();//清除异常
        log_error("java %s Exception", name);
    }
}

//用于测试
int testCallJava(int index) {
    if (!gJniEnv) {
        return -1;
    }
    const char *mname = "testCall";

    jmethodID method = gJniEnv->GetStaticMethodID(gJniJc, mname, "(ILjava/lang/String;)I");
    if (method == NULL) {
        log_error("method ID is NULL!");
        return -1;
    }

    int ret = gJniEnv->CallStaticIntMethod(gJniJc, method, index, "test");
    CheckJNIException(gJniEnv, mname);
    if(ret != 0) {
        log_warning("call java testCall, index:%d, return:%d", index, ret);
    }
    return ret;
}
