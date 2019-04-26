
#ifndef __HPAVC_ENCODER_H__
#define __HPAVC_ENCODER_H__

#include "HPAVC_Export.h"
#include "mfx/mfxvideo.h"

#define HPAVC_BASELINE_PROFILE	0
#define HPAVC_MAIN_PROFILE		1
#define HPAVC_HIGH_PROFILE		2

typedef void (*HPAVC_ENCODER_CB_DATA)(const void*pUserData,unsigned char*pData,int nLen,int nKeyFrame);
typedef void (*HPAVC_ENCODER_CB_SF)(const void*pUserData,mfxFrameSurface1* pSurface,mfxFrameAllocator* pmfxAlloc);


HPAVC_API void HPAVC_EncoderSetMaxCount(unsigned int nMaxCount);
HPAVC_API int  HPAVC_EncoderTry(int nWidth,int nHeight);
HPAVC_API void*HPAVC_EncoderOpen(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nQMin,int nQMax,int nKeyIntMax,HPAVC_ENCODER_CB_DATA encoder_cb_data,HPAVC_ENCODER_CB_SF encoder_cb_sf,void*pUserData,int nKey);
HPAVC_API void HPAVC_EncoderClose(void*pHandle);
HPAVC_API int  HPAVC_EncoderEncoding(void*pHandle,unsigned char*pData[],int nRequestKeyFrame);
HPAVC_API int  HPAVC_EncoderSetVideoParam(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);


#endif
