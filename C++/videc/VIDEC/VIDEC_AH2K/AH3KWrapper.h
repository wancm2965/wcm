#ifndef __AH3KWRAPPER_H__
#define __AH3KWRAPPER_H__

#include "../VIDEC_Codecs/XDllLoadLibrary.h"

#include "../AH3K/AH3KAPI.h"

void AH3K_init(void);
void AH3K_terminate(void);

int AH3KGetDevCount(void);
int AH3KGetDevName(int nDevID,char*szName,int nCount);
int AH3KHasAudio(int nDevID);
int AH3KIsHD(int nDevID);

void*AH3KOpen(int nDevID,AH3K_VIDEO_CB video_cb,AH3K_AUDIO_CB audio_cb,const void*pUserData);
void AH3KClose(void*pHandle);
int AH3KSetParams(void*pHandle,int nScaleType,int nWidth,int nHeight,int nFrameRate);

class AH3KWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	AH3KWrapper(HINSTANCE hInstance);
	~AH3KWrapper(void);
public:
	static AH3KWrapper&Instance(void);
	bool IsOK(void);

	int (*pAH3K_GetDevCount)(void);
	int (*pAH3K_GetDevName)(int nDevID,char*szName,int nCount);
	int (*pAH3K_HasAudio)(int nDevID);
	int (*pAH3K_IsHD)(int nDevID);

	void*(*pAH3K_Open)(int nDevID,AH3K_VIDEO_CB video_cb,AH3K_AUDIO_CB audio_cb,const void*pUserData);
	void (*pAH3K_Close)(void*pHandle);
	int (*pAH3K_SetParams)(void*pHandle,int nScaleType,int nWidth,int nHeight,int nFrameRate);
};

#endif
