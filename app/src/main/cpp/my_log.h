//
// Created by Administrator on 2017/3/3 0003.
//

#ifndef ANDROIDJNITSOCKET_MY_LOG_H
#define ANDROIDJNITSOCKET_MY_LOG_H
#include "android/log.h"

static const char *TAG = "native_socket";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)
#endif //ANDROIDJNITSOCKET_MY_LOG_H
