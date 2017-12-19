#pragma once

extern "C"
{
#ifdef __cplusplus

	//C99整数范围常量. [纯C程序可以不用, 而C++程序必须定义该宏.]
#define  __STDC_LIMIT_MACROS
	//C99整数常量宏. [纯C程序可以不用, 而C++程序必须定义该宏.]
#define  __STDC_CONSTANT_MACROS
	// for int64_t print using PRId64 format.
#define __STDC_FORMAT_MACROS

	//解决错误：错误处理error C4996: 'fopen': This function or variable may be unsafe
	//#define _CRT_SECURE_NO_DEPRECATE //这个定义需要放到FFmpeg的c文件中，或者最好定义在vs的(全局工程可以应用):
	//	项目->属性->配置属性->C / C++->预处理器->预处理器定义，增加： 		_CRT_SECURE_NO_DEPRECATE

	//直接复制到：预处理器定义
	//	；__STDC_LIMIT_MACROS；__STDC_CONSTANT_MACROS；__STDC_FORMAT_MACROS；_CRT_SECURE_NO_DEPRECATE；

#ifdef _STDINT_H
#undef _STDINT_H
#endif

#include <stdint.h>

#endif
}


#ifdef __cplusplus
extern "C"
{
#endif

#include <libavcodec/avcodec.h> 
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avio.h>
#include <libavutil/file.h>
#include <libavutil/mem.h>
#include <libavutil/time.h>

#ifdef __cplusplus
}
#endif


#include <string>
using namespace std;

#ifdef _WIN32

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS 

#include <atlbase.h>
#include <atlstr.h>

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"postproc.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"KBASE.lib")

//linux
#else

#endif



