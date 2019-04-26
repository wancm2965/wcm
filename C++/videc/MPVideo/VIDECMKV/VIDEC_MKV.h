//VIDEC_MKV.h
#ifndef __VIDEC_MKV_H__
#define __VIDEC_MKV_H__

#ifdef VIDEC_MKV_EXPORT
#define VIDEC_MKV_API _declspec(dllexport)
#elif VIDEC_MKV_DLL
#define VIDEC_MKV_API _declspec(dllimport)
#else
#define VIDEC_MKV_API
#endif

typedef enum
{
	AUDIO_MP3=0,
	AUDIO_MP2=1,
	AUDIO_AAC4=2,
}AUDIO_TYPE;

typedef enum
{
	VIDEO_H264=0,
	VIDEO_H263=1,
}VIDEO_TYPE;

VIDEC_MKV_API void* VIDEC_MKV_Open(const char*cszPathName);
VIDEC_MKV_API void VIDEC_MKV_Close(void*pHandle);

VIDEC_MKV_API int VIDEC_MKV_SetAudio(void*pHandle,int nBitrate,int nSamplesRate,int nChannels,int nBitPerSample,AUDIO_TYPE nType);
VIDEC_MKV_API int VIDEC_MKV_SetVideo(void*pHandle,int nFrameRate,int nWidth,int nHeight,VIDEO_TYPE nType);

VIDEC_MKV_API int VIDEC_MKV_WriteAudio(void*pHandle,const void *pData,int nLen,unsigned long nTimestamp);
VIDEC_MKV_API int VIDEC_MKV_WriteVideo(void*pHandle,const void *pData,int nLen,unsigned long nTimestamp,int nKeyFrame);

#endif