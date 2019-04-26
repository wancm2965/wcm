#ifndef __AVMSWRAPPER_H__
#define __AVMSWRAPPER_H__

#include "../VIDEC_Codecs/XDllLoadLibrary.h"

typedef void (*AVMS_PLUGIN_CALLBACK_PCM_DATA)(const void*pUserData,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp);
typedef void (*AVMS_PLUGIN_CALLBACK_VIDEO_PACKET)(const void*pUserData,unsigned char*pData,int nLen);

void AVMSP_init(void);
void AVMSP_terminate(void);

void*AVMSP_Open(const char*cszFile,AVMS_PLUGIN_CALLBACK_PCM_DATA cb_pcm_data,AVMS_PLUGIN_CALLBACK_VIDEO_PACKET cb_video_packet,const void*pUserData,unsigned long*lpnVideoTimestamp=NULL,unsigned long*lpnAudioTimestamp=NULL);
void AVMSP_Close(void*pHandle);
int AVMSP_Seek(void*pHandle,unsigned long ulSeekTime);
int AVMSP_Play(void*pHandle);
int AVMSP_Pause(void*pHandle);
int AVMSP_SetVolume(void*pHandle,int nValue);
unsigned long AVMSP_GetDuration(void*pHandle);
unsigned long AVMSP_GetPosition(void*pHandle);

class AVMSWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	AVMSWrapper(HINSTANCE hInstance);
	~AVMSWrapper(void);
public:
	static AVMSWrapper&Instance(void);
	bool IsOK(void);

	void*(*pAVMSP_Open)(const char*cszFile,AVMS_PLUGIN_CALLBACK_PCM_DATA cb_pcm_data,AVMS_PLUGIN_CALLBACK_VIDEO_PACKET cb_video_packet,const void*pUserData,unsigned long*lpnVideoTimestamp,unsigned long*lpnAudioTimestamp);
	void (*pAVMSP_Close)(void*pHandle);
	int (*pAVMSP_Seek)(void*pHandle,unsigned long ulSeekTime);
	int (*pAVMSP_Play)(void*pHandle);
	int (*pAVMSP_Pause)(void*pHandle);
	int (*pAVMSP_SetVolume)(void*pHandle,int nValue);
	unsigned long (*pAVMSP_GetDuration)(void*pHandle);
	unsigned long (*pAVMSP_GetPosition)(void*pHandle);
};

#endif
