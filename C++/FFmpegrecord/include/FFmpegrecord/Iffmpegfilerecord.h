#ifndef  _IFFMPEGFILERECORD_H_H_
#define  _IFFMPEGFILERECORD_H_H_


#ifdef WIN32
//#define FFMPEGRECORD_EXPORTS
#ifdef FFMPEGFILERECORD_EXPORTS
#define FFMPEGFILERECORD_API _declspec(dllexport)
#else
#define FFMPEGFILERECORD_API _declspec(dllimport)
#endif
#else
#define FFMPEGFILERECORD_API
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

#endif

class IFFmpegfilerecordNotify
{
public:
	virtual  void OnCompoundResut(int nResult) = 0;
};

class FFMPEGFILERECORD_API IFFmpegfilerecord
{
public:
	IFFmpegfilerecord(){};
	virtual ~IFFmpegfilerecord(){};
	/******************************************************************************
	* Open
	*�������ϳ�¼���ļ�
	*���룺	cszFileName			-�ϳ��ļ�����
	cszVideoFileName            -��Ƶ�ļ�����
	cszAudioFileName            -��Ƶ�ļ�����
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int CompoundFile(const char*cszFileName,const char*cszVideoFileName,const char*cszAudioFileName)=0;

	

	/******************************************************************************
	* Create
	*����������¼���ļ�����
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����IFFmpegrecord��ָ�룬ʧ�ܷ���NULL
	*****************************************************************************/
	static IFFmpegfilerecord*Create(IFFmpegfilerecordNotify& rNotify);
};



