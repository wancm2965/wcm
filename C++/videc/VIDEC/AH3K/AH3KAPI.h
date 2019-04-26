//AH3KAPI.h
#ifndef __AH3KAPI_H__
#define __AH3KAPI_H__

#ifdef AH3K_EXPORT
#define AH3K_API _declspec(dllexport)
#elif AH3K_DLL
#define AH3K_API _declspec(dllimport)
#else
#define AH3K_API
#endif


typedef void* (*AH3K_VIDEO_CB)(const void*pUserData,unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp,unsigned long nHeaderLen);
typedef void (*AH3K_AUDIO_CB)(const void*pUserData,short*pSamples,int nSamples,int nSampleRate,unsigned long nTimestamp);


AH3K_API int AH3K_GetDevCount(void);
AH3K_API int AH3K_GetDevName(int nDevID,char*szName,int nCount);
AH3K_API int AH3K_HasAudio(int nDevID);
AH3K_API int AH3K_IsHD(int nDevID);

AH3K_API void*AH3K_Open(int nDevID,AH3K_VIDEO_CB video_cb,AH3K_AUDIO_CB audio_cb,const void*pUserData);
AH3K_API void AH3K_Close(void*pHandle);
AH3K_API int AH3K_SetParams(void*pHandle,int nScaleType,int nWidth,int nHeight,int nFrameRate);

#endif