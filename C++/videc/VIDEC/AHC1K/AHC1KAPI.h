//AHC1KAPI.h
#ifndef __AHC1KAPI_H__
#define __AHC1KAPI_H__

#ifdef AHC1K_EXPORT
#define AHC1K_API _declspec(dllexport)
#elif AHC1K_DLL
#define AHC1K_API _declspec(dllimport)
#else
#define AHC1K_API
#endif



typedef void* (*AHC1K_VIDEO_CB)(const void*pUserData,unsigned char*pFrameData,int nFrameLen,
								int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp,unsigned long nHeaderLen);

typedef void* (*AHC1K_STREAM_CB)(const void*pUserData,unsigned char*pStreamData,int nStreamLen,
								 int nWidth,int nHeight,int nFrameRate,int nType);
typedef void* (*AHC1K_MALLOC)(size_t size);
typedef void (*AHC1K_FREE)(void*ptr);


AHC1K_API int AHC1K_GetDevCount(void);
AHC1K_API int AHC1K_GetDevName(int nDevID,char*szName,int nMaxCount);


AHC1K_API void*AHC1K_Open(int nDevID,AHC1K_VIDEO_CB video_cb,AHC1K_STREAM_CB stream_cb,const void*pUserData);
AHC1K_API void AHC1K_Close(void*pHandle);

AHC1K_API int AHC1K_SetParams(void*pHandle,int nScaleType,int nWidth,int nHeight,int nFrameRate,
							  int nBitrate,int nRateControlType);

AHC1K_API void AHC1K_SetMemory(AHC1K_MALLOC Malloc,AHC1K_FREE Free);

#endif