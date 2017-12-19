// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#ifdef _WIN32
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlstr.h>

// TODO:  在此处引用程序需要的其他头文件

#else


#endif
/*
FIX: H.264 in some container format (FLV, MP4, MKV etc.) need
"h264_mp4toannexb" bitstream filter (BSF)
*Add SPS,PPS in front of IDR frame
*Add start code ("0,0,0,1") in front of NALU
H.264 in some container (MPEG2TS) don't need this BSF.
*/
//'1': Use H.264 Bitstream Filter 
#define USE_H264BSF 1

/*
FIX:AAC in some container format (FLV, MP4, MKV etc.) need
"aac_adtstoasc" bitstream filter (BSF)
*/
//'1': Use AAC Bitstream Filter 
#define USE_AACBSF 1


#include <string>

using namespace std;


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

#ifdef __cplusplus
}
#endif

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"postproc.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")
