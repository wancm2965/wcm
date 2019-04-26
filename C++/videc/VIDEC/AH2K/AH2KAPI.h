//AH2KAPI.h
#ifndef __AH2KAPI_H__
#define __AH2KAPI_H__

#ifdef AH2K_EXPORT
#define AH2K_API _declspec(dllexport)
#elif AH2K_DLL
#define AH2K_API _declspec(dllimport)
#else
#define AH2K_API
#endif

#define INPUT_FRAME_TYPE_MERGED		0x00000001UL
#define INPUT_FRAME_TYPE_422		0x00000002UL
#define INPUT_FRAME_TYPE_I			0x00000004UL

typedef void* (*AH2K_VIDEO_CB)(const void*pUserData,unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp,unsigned long nHeaderLen,unsigned long nInputFrameType);
typedef void (*AH2K_AUDIO_CB)(const void*pUserData,short*pSamples,int nSamples,int nSampleRate,unsigned long nTimestamp);
typedef void* (*AH2K_MALLOC)(size_t size);
typedef void (*AH2K_FREE)(void*ptr);


AH2K_API int AH2K_GetDevCount(void);
AH2K_API int AH2K_GetDevName(int nDevID,char*szName,int nCount);
AH2K_API int AH2K_HasAudio(int nDevID);
AH2K_API int AH2K_IsHD(int nDevID);

AH2K_API void*AH2K_Open(int nDevID,AH2K_VIDEO_CB video_cb,AH2K_AUDIO_CB audio_cb,const void*pUserData);
AH2K_API void AH2K_Close(void*pHandle);
AH2K_API int AH2K_SetParams(void*pHandle,int nScaleType,int nWidth,int nHeight,int nFrameRate);
AH2K_API void AH2K_SetMemory(AH2K_MALLOC Malloc,AH2K_FREE Free);
AH2K_API int AH2K_HasVideoSignal(void*pHandle);

#endif