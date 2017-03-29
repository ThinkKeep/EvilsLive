//
// Created by jason on 17/1/11.
//

#include "native-lib.h"
#include <stdlib.h>

#include "log.h"

char *ja2c(JNIEnv *pEnv, jbyteArray pArray, int *pInt);

jstring Java_com_thinkkeep_videolib_jni_EvilsLiveJni_testJni(JNIEnv* env, jobject instance) {
    return env->NewStringUTF("test Jni");
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
Java_com_thinkkeep_videolib_jni_EvilsLiveJni_sendStream(JNIEnv *env, jclass type, jbyteArray j_data) {
    int size = 0;
    char *data = ja2c(env, j_data, &size);
    AutoFree afdata(data);
    //TODO
}


JNIEXPORT void JNICALL
Java_com_thinkkeep_videolib_jni_EvilsLiveJni_setStreamConfig(JNIEnv *env, jclass type,
                                                             jbyteArray url_) {
    jbyte *url = env->GetByteArrayElements(url_, NULL);

    // TODO

    env->ReleaseByteArrayElements(url_, url, 0);
}