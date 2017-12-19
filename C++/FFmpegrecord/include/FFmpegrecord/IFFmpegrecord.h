#ifndef  _IFFMPEGRECORD_H_H_
#define  _IFFMPEGRECORD_H_H_


#ifdef WIN32
//#define FFMPEGRECORD_EXPORTS
#ifdef FFMPEGRECORD_EXPORTS
#define FFMPEGRECORD_API _declspec(dllexport)
#else
#define FFMPEGRECORD_API _declspec(dllimport)
#endif
#else
#define FFMPEGRECORD_API
#define TRACE  printf
#endif

#if !defined(_MSC_VER)
#include <stdint.h>
#else
// Define C99 equivalent types, since MSVC doesn't provide stdint.h.
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef __int64             int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned __int64    uint64_t;
#endif


class FFMPEGRECORD_API IFFmpegrecord
{
public:
	IFFmpegrecord(){};
	virtual ~IFFmpegrecord(){};
	/******************************************************************************
	* Open
	*描述：打开录像文件
	*输入：	cszFileName			-文件名称
	*输出： 无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int Open(const char*cszFileName)=0;

	/******************************************************************************
	* Close
	*描述：关闭录像文件,对象指针自销毁不需delete
	*输入：	无
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void Close(void)=0;

	/******************************************************************************
	* WriteVideo
	*描述：写视频到录像文件
	*输入：	data			-视频包数据
	len			        -视频包数据长度
	*输出： 无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int WriteVideo(unsigned char*data, unsigned int len, unsigned long nTimestamp, bool bKeyFrame, unsigned int nWidth = 640, unsigned int nHeight = 480) = 0;

	/******************************************************************************
	* WriteAudio
	*描述：写音频到录像文件
	*输入：	data	数据
	len		        数据长度
	*输出： 无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int WriteAudio(unsigned char*data, unsigned int len, unsigned long nTimestamp) = 0;

	/******************************************************************************
	* Create
	*描述：创建录像文件对象
	*输入：	无
	*输出： 无
	*返回值：成功返回IFFmpegrecord的指针，失败返回NULL
	*****************************************************************************/
	static IFFmpegrecord*Create();
};

#endif

