//VIDEC_AVIWriter.h
#ifndef __VIDEC_AVIWRITER_H__
#define __VIDEC_AVIWRITER_H__

#include "VIDEC_Export.h"

#define WRITER_STANDARD_FORMAT_NONE		0
#define WRITER_STANDARD_FORMAT_AVI		1
#define WRITER_STANDARD_FORMAT_MKV		2
#define WRITER_STANDARD_FORMAT_MP3		3
#define WRITER_STANDARD_FORMAT_MP4		4

//AVI¼�����
class VIDEC_API VIDEC_AVIWriter
{
public:
	VIDEC_AVIWriter(void){};
	virtual~VIDEC_AVIWriter(void){};
public:
	/******************************************************************************
	* Open
	*��������¼���ļ�
	*���룺	cszFileName			-�ļ�����
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int Open(const char*cszFileName,int nStandardFormat,int nMaxFileSize=2013265920, int CacheTimeMS = 10000)=0;

	/******************************************************************************
	* Close
	*�������ر�¼���ļ�
	*���룺	��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void Close(void)=0;

	/******************************************************************************
	* WriteVideo
	*������д��Ƶ��¼���ļ�
	*���룺	pPacketData			-��Ƶ������
			nPacketLen			-��Ƶ�����ݳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int WriteVideo(unsigned char*pPacketData,int nPacketLen)=0;

	/******************************************************************************
	* WriteAudio
	*������д��Ƶ��¼���ļ�
	*���룺	pSampleData			-��������
			nSampleDataLen		-�������ݳ���
			nSampleRate			-������
			nSynTimestamp		-ͬ��ʱ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int WriteAudio(unsigned char*pSampleData,int nSampleDataLen,int nSampleRate,unsigned long nSynTimestamp)=0;

	/******************************************************************************
	* Create
	*����������¼���ļ�����
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����VIDEC_AVIWriter��ָ�룬ʧ�ܷ���NULL
	*****************************************************************************/
	static VIDEC_AVIWriter*Create(void);
};


#endif