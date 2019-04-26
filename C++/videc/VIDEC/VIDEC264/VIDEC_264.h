//VIDEC_264.h
#ifndef __VIDEC_264_H__
#define __VIDEC_264_H__

#ifdef VIDEC_264_EXPORT
#define VIDEC_264_API _declspec(dllexport)
#elif VIDEC_264_DLL
#define VIDEC_264_API _declspec(dllimport)
#else
#define VIDEC_264_API
#endif

typedef enum{
	VIDEC_H264_CODEC_H264=0,
	VIDEC_H264_CODEC_MPEG4,
	VIDEC_H264_CODEC_VP8,
	VIDEC_H264_CODEC_H263,
	VIDEC_H264_CODEC_H263P,
	VIDEC_H264_CODEC_H263PP,
	VIDEC_H264_CODEC_COUNT
}VIDEC_H264_CODEC_TYPE;

VIDEC_264_API void VIDEC264_Initialize(void);
VIDEC_264_API void VIDEC264_UnInitialize(void);

VIDEC_264_API void*VIDEC264_EncoderOpen(VIDEC_H264_CODEC_TYPE nType,int nWidth,int nHeight,int nFrameRate,int nBitrate);
VIDEC_264_API void VIDEC264_EncoderClose(void*pHandle);
VIDEC_264_API int VIDEC264_EncoderEncode(void*pHandle,unsigned char*pYV12[],unsigned char*pStreamBuf,int nBufLen,int* pbKeyFrame);

VIDEC_264_API void*VIDEC264_DecoderOpen(VIDEC_H264_CODEC_TYPE nType,int nWidth,int nHeight);
VIDEC_264_API void VIDEC264_DecoderClose(void*pHandle);
VIDEC_264_API int  VIDEC264_DecoderDecode(void*pHandle,unsigned char*pStreamData,int nStreamLen,unsigned char*pYV12[],int nStride[],int*pnWidth,int*pnHeight,int*pnUsedBytes,int*pnKeyFrame);

#endif