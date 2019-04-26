//VIDECINLAPI.h
#ifndef __VIDECINLAPI_H__
#define __VIDECINLAPI_H__

#ifdef VIDECINL_EXPORT
#define VIDECINL_API _declspec(dllexport)
#elif VIDECINL_DLL
#define VIDECINL_API _declspec(dllimport)
#else
#define VIDECINL_API
#endif


#define INL_BASELINE_PROFILE	0
#define INL_MAIN_PROFILE		1
#define INL_HIGH_PROFILE		2

typedef void (*VIDECINL_ENCODER_CB)(const void*pUserData,unsigned char*pData,int nLen,int nKeyFrame);

typedef void (*VIDECINL_DECODER_CB)(const void*pUserData,unsigned char*pData,int nLen,int nWidth,int nHeight);

typedef void* (*VIDECINL_ALIGNED_MALLOC)(size_t size,size_t alignment);
typedef void (*VIDECINL_ALIGNED_FREE)(void*ptr);


VIDECINL_API void VIDECINL_EncoderSetMaxCount(unsigned int nMaxCount);
VIDECINL_API int VIDECINL_EncoderTry(int nWidth,int nHeight);
VIDECINL_API void*VIDECINL_EncoderOpen(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nQMin,int nQMax,int nKeyIntMax,VIDECINL_ENCODER_CB encoder_cb,void*pUserData,int nKey);
VIDECINL_API void VIDECINL_EncoderClose(void*pHandle);
VIDECINL_API int VIDECINL_EncoderEncoding(void*pHandle,unsigned char*pData,int nLen,int nRequestKeyFrame);

//VIDECINL_API int VIDECINL_DecoderTry(VIDECINL_ALIGNED_MALLOC aligned_malloc,VIDECINL_ALIGNED_FREE aligned_free);
//VIDECINL_API void* VIDECINL_DecoderOpen(int nWidth,int nHeight,VIDECINL_DECODER_CB decoder_cb,void*pUserData,int nKey);
//VIDECINL_API void VIDECINL_DecoderClose(void*pHandle);
//VIDECINL_API int VIDECINL_DecoderDecode(void*pHandle,const unsigned char*pData,int nLen);


#endif