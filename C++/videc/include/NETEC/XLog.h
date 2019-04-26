// XLog.h: interface for the XUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XLOG_H__
#define __XLOG_H__

//#define _LOG_DEBUG_ //是否开启日志调试

#ifdef _LOG_DEBUG_
#if defined(_ANDROID_)
#  include <android/log.h>
#  define LOGD(...)       __android_log_print(ANDROID_LOG_DEBUG, _LOG_TAG, __VA_ARGS__)
#  define LOGW(...)       __android_log_print(ANDROID_LOG_WARN,  _LOG_TAG, __VA_ARGS__)
#  define LOGE(...)       __android_log_print(ANDROID_LOG_ERROR, _LOG_TAG, __VA_ARGS__)
#else
#  define LOGD(...)       {printf("D/" _LOG_TAG ": ");printf( __VA_ARGS__);printf("\n");}
#  define LOGW(...)       {printf("W/" _LOG_TAG ": ");printf( __VA_ARGS__);printf("\n");}
#  define LOGE(...)       {printf("E/" _LOG_TAG ": ");printf( __VA_ARGS__);printf("\n");}
#endif
#else
#  define LOGD(...)
#  define LOGW(...)
#  define LOGE(...)
#endif
#endif
