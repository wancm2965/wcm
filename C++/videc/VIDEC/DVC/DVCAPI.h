//DVCAPI.h
#ifndef __DVCAPI_H__
#define __DVCAPI_H__

#ifdef DVC_EXPORT
#define DVC_API _declspec(dllexport)
#elif DVC_DLL
#define DVC_API _declspec(dllimport)
#else
#define DVC_API
#endif


typedef void (*DVC_VIDEO_CB)(const void*pUserData,unsigned char*pFrameData,int nFrameLen);

DVC_API int DVC_SetIP(const char*cszIP,unsigned short nPort,const char*cszPassword);

DVC_API int DVC_GetDevCount(void);
DVC_API int DVC_GetDevName(int nDevID,char*szName,int nCount);

DVC_API void*DVC_Open(int nDevID,DVC_VIDEO_CB video_cb,const void*pUserData);
DVC_API void DVC_Close(void*pHandle);
DVC_API int DVC_SetParams(void*pHandle,int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution);
DVC_API int DVC_GetStatus(void*pHandle);
DVC_API int DVC_RequestKeyFrame(void*pHandle);
DVC_API int DVC_SendData(void*pHandle,const char*pData,int nLen);

#endif