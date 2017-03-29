//
// Created by jason on 17/3/29.
//

#ifndef ANDROID_LOG_H
#define ANDROID_LOG_H

#include <android/log.h>
#include <jni.h>

#define LOG_TAG "AVLIB"

#define log_debug(fmt, ...) \
    do { \
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "[%s]"fmt, __FUNCTION__, ##__VA_ARGS__); \
    } while(0)

#define log_info(fmt, ...) \
    do { \
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "[%s]"fmt, __FUNCTION__, ##__VA_ARGS__); \
    } while(0)

#define log_warning(fmt, ...) \
    do { \
        __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "[%s]"fmt, __FUNCTION__, ##__VA_ARGS__); \
    } while(0)

#define log_error(fmt, ...) \
    do { \
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "[%s]"fmt, __FUNCTION__, ##__VA_ARGS__); \
    } while(0)

#define log_fatal(fmt, ...) \
    do { \
        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, "[%s]"fmt, __FUNCTION__, ##__VA_ARGS__); \
    } while(0)

#endif //ANDROID_LOG_H
