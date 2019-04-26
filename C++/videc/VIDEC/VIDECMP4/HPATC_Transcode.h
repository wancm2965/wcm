//HPATCCodec.h
#ifndef __HPATC_TRANSCODE_H__
#define __HPATC_TRANSCODE_H__

#include "HPATC_Export.h"


class HPATC_API HPATC_Transcode;
class HPATC_TranscodeCallback
{
public:
	virtual void OnHPATC_TranscodeCallbackPacketData(HPATC_Transcode*pTranscode,void*pPacketData,int nPacketLen,unsigned long ulTimestamp/*原音频编码时间*/)=0;
};

//输出为AAC编码裸数据，输入为带AUDEC音频包头的音频数据
class HPATC_API HPATC_Transcode 
{
public:
	HPATC_Transcode(void){};
	virtual~HPATC_Transcode(void){};
public:
	virtual int Open(int nSampleRate=44100,int nSamplesPerFrame=1024)=0;
	virtual void Close(void)=0;

	/*输入：pPacketData，带AUDEC音频包头的音频数据；nPacketLen音频数据数据长度；
	 *输出：0：转码成功；非0：转码失败；
	 *回调：AAC编码裸数据 */
	virtual int TranscodePacketData(void*pPacketData,int nPacketLen)=0;

	/*输入：pPacketData，PCM音频数据；nPacketLen音频数据数据长度；
	 *输出：0：转码成功；非0：转码失败；
	 *回调：AAC编码裸数据 */
	virtual int TranscodePCMData(short*pSamples,int nSamples,int nSampleRate,unsigned long ulTimestamp)=0;

	static HPATC_Transcode*Create(HPATC_TranscodeCallback*pCallback);
};

/*导出函数*/
typedef void (*HPATC_CALLBACK_AAC_DATA)(void*pUserData,void*pHandle,void*pPacketData,int nPacketLen,unsigned long ulTimestamp);

HPATC_API void* HPATC_Open(void*pUserData,HPATC_CALLBACK_AAC_DATA cb_aac_data);
HPATC_API void  HPATC_Close(void*pHandle);
HPATC_API int   HPATC_TranscodePCMData(void*pHandle,short*pSamples,int nSamples,int nSampleRate,unsigned long ulTimestamp);
HPATC_API int   HPATC_TranscodePacketData(void*pHandle,void*pPacketData,int nPacketLen);

#endif