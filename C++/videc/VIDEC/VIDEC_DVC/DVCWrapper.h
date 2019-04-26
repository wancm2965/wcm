#ifndef __DVCWRAPPER_H__
#define __DVCWRAPPER_H__

#include "../VIDEC_Codecs/XDllLoadLibrary.h"

#include "../DVC/DVCAPI.h"

void DVC_init(void);
void DVC_terminate(void);

int DVCSetIP(const char*cszIP,unsigned short nPort,const char*cszPassword);

int DVCGetDevCount(void);
int DVCGetDevName(int nDevID,char*szName,int nCount);

void*DVCOpen(int nDevID,DVC_VIDEO_CB video_cb,const void*pUserData);
void DVCClose(void*pHandle);
int DVCSetParams(void*pHandle,int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution);
int DVCGetStatus(void*pHandle);
int DVCRequestKeyFrame(void*pHandle);
int DVCSendData(void*pHandle,const char*pData,int nLen);

class DVCWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	DVCWrapper(HINSTANCE hInstance);
	~DVCWrapper(void);
public:
	static DVCWrapper&Instance(void);
	bool IsOK(void);

	int (*pDVC_SetIP)(const char*cszIP,unsigned short nPort,const char*cszPassword);
	int (*pDVC_GetDevCount)(void);
	int (*pDVC_GetDevName)(int nDevID,char*szName,int nCount);

	void*(*pDVC_Open)(int nDevID,DVC_VIDEO_CB video_cb,const void*pUserData);
	void (*pDVC_Close)(void*pHandle);
	int (*pDVC_SetParams)(void*pHandle,int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution);
	int (*pDVC_GetStatus)(void*pHandle);
	int (*pDVC_RequestKeyFrame)(void*pHandle);
	int (*pDVC_SendData)(void*pHandle,const char*pData,int nLen);
};

#endif
