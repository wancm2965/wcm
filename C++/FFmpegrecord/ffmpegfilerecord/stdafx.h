// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#ifdef _WIN32
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <atlbase.h>
#include <atlstr.h>

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�

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

	//C99������Χ����. [��C������Բ���, ��C++������붨��ú�.]
#define  __STDC_LIMIT_MACROS
	//C99����������. [��C������Բ���, ��C++������붨��ú�.]
#define  __STDC_CONSTANT_MACROS
	// for int64_t print using PRId64 format.
#define __STDC_FORMAT_MACROS

	//������󣺴�����error C4996: 'fopen': This function or variable may be unsafe
	//#define _CRT_SECURE_NO_DEPRECATE //���������Ҫ�ŵ�FFmpeg��c�ļ��У�������ö�����vs��(ȫ�ֹ��̿���Ӧ��):
	//	��Ŀ->����->��������->C / C++->Ԥ������->Ԥ���������壬���ӣ� 		_CRT_SECURE_NO_DEPRECATE

	//ֱ�Ӹ��Ƶ���Ԥ����������
	//	��__STDC_LIMIT_MACROS��__STDC_CONSTANT_MACROS��__STDC_FORMAT_MACROS��_CRT_SECURE_NO_DEPRECATE��

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
