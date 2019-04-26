//HPATCCodec.h
#ifndef __HPATC_TRANSCODE_H__
#define __HPATC_TRANSCODE_H__

#include "HPATC_Export.h"


class HPATC_API HPATC_Transcode;
class HPATC_TranscodeCallback
{
public:
	virtual void OnHPATC_TranscodeCallbackPacketData(HPATC_Transcode*pTranscode,void*pPacketData,int nPacketLen,unsigned long ulTimestamp/*ԭ��Ƶ����ʱ��*/)=0;
};

//���ΪAAC���������ݣ�����Ϊ��AUDEC��Ƶ��ͷ����Ƶ����
class HPATC_API HPATC_Transcode 
{
public:
	HPATC_Transcode(void){};
	virtual~HPATC_Transcode(void){};
public:
	virtual int Open(int nSampleRate=44100,int nSamplesPerFrame=1024)=0;
	virtual void Close(void)=0;

	/*���룺pPacketData����AUDEC��Ƶ��ͷ����Ƶ���ݣ�nPacketLen��Ƶ�������ݳ��ȣ�
	 *�����0��ת��ɹ�����0��ת��ʧ�ܣ�
	 *�ص���AAC���������� */
	virtual int TranscodePacketData(void*pPacketData,int nPacketLen)=0;

	/*���룺pPacketData��PCM��Ƶ���ݣ�nPacketLen��Ƶ�������ݳ��ȣ�
	 *�����0��ת��ɹ�����0��ת��ʧ�ܣ�
	 *�ص���AAC���������� */
	virtual int TranscodePCMData(short*pSamples,int nSamples,int nSampleRate,unsigned long ulTimestamp)=0;

	static HPATC_Transcode*Create(HPATC_TranscodeCallback*pCallback);
};

/*��������*/
typedef void (*HPATC_CALLBACK_AAC_DATA)(void*pUserData,void*pHandle,void*pPacketData,int nPacketLen,unsigned long ulTimestamp);

HPATC_API void* HPATC_Open(void*pUserData,HPATC_CALLBACK_AAC_DATA cb_aac_data);
HPATC_API void  HPATC_Close(void*pHandle);
HPATC_API int   HPATC_TranscodePCMData(void*pHandle,short*pSamples,int nSamples,int nSampleRate,unsigned long ulTimestamp);
HPATC_API int   HPATC_TranscodePacketData(void*pHandle,void*pPacketData,int nPacketLen);

#endif