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
	*��������¼���ļ�
	*���룺	cszFileName			-�ļ�����
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int Open(const char*cszFileName)=0;

	/******************************************************************************
	* Close
	*�������ر�¼���ļ�,����ָ�������ٲ���delete
	*���룺	��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void Close(void)=0;

	/******************************************************************************
	* WriteVideo
	*������д��Ƶ��¼���ļ�
	*���룺	data			-��Ƶ������
	len			        -��Ƶ�����ݳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int WriteVideo(unsigned char*data, unsigned int len, unsigned long nTimestamp, bool bKeyFrame, unsigned int nWidth = 640, unsigned int nHeight = 480) = 0;

	/******************************************************************************
	* WriteAudio
	*������д��Ƶ��¼���ļ�
	*���룺	data	����
	len		        ���ݳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int WriteAudio(unsigned char*data, unsigned int len, unsigned long nTimestamp) = 0;

	/******************************************************************************
	* Create
	*����������¼���ļ�����
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����IFFmpegrecord��ָ�룬ʧ�ܷ���NULL
	*****************************************************************************/
	static IFFmpegrecord*Create();
};

#endif

