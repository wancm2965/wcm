#ifndef __AH2KWRAPPER_H__
#define __AH2KWRAPPER_H__

#include "../VIDEC_Codecs/XDllLoadLibrary.h"

#include "../AH2K/AH2KAPI.h"

void AH2K_init(void);
void AH2K_terminate(void);

int AH2KGetDevCount(void);
int AH2KGetDevName(int nDevID,char*szName,int nCount);
int AH2KHasAudio(int nDevID);
int AH2KIsHD(int nDevID);

void*AH2KOpen(int nDevID,AH2K_VIDEO_CB video_cb,AH2K_AUDIO_CB audio_cb,const void*pUserData);
void AH2KClose(void*pHandle);
int AH2KSetParams(void*pHandle,int nScaleType,int nWidth,int nHeight,int nFrameRate);
void AH2KSetMemory(AH2K_MALLOC Malloc,AH2K_FREE Free);
int AH2KHasVideoSignal(void*pHandle);

class AH2KWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	AH2KWrapper(HINSTANCE hInstance);
	~AH2KWrapper(void);
public:
	static AH2KWrapper&Instance(void);
	bool IsOK(void);

	int (*pAH2K_GetDevCount)(void);
	int (*pAH2K_GetDevName)(int nDevID,char*szName,int nCount);
	int (*pAH2K_HasAudio)(int nDevID);
	int (*pAH2K_IsHD)(int nDevID);

	void*(*pAH2K_Open)(int nDevID,AH2K_VIDEO_CB video_cb,AH2K_AUDIO_CB audio_cb,const void*pUserData);
	void (*pAH2K_Close)(void*pHandle);
	int (*pAH2K_SetParams)(void*pHandle,int nScaleType,int nWidth,int nHeight,int nFrameRate);
	void (*pAH2K_SetMemory)(AH2K_MALLOC Malloc,AH2K_FREE Free);
	int (*pAH2K_HasVideoSignal)(void*pHandle);
};

#endif
