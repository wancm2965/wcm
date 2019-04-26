//H264Decoder.h
//use ffmpeg 3.2 , only decode h264 h263 mpeg4 mjpeg
//add 201611  

#ifndef __AVH264DECODER_H__
#define __AVH264DECODER_H__

#include <AVCONAVC/AVCONAVC.h>

#ifdef AVH264DECODER_EXPORTS
#define AVH264DECODER_API _declspec(dllexport)
#elif AVH264DECODER_DLL
#define AVH264DECODER_API _declspec(dllimport)
#else
#define AVH264DECODER_API
#endif


AVH264DECODER_API void AVH264DECODER_Initialize(void);
AVH264DECODER_API void AVH264DECODER_UnInitialize(void);

AVH264DECODER_API void*AVH264DECODER_DecoderOpen(int nWidth,int nHeight,int AVCCodecID=AVC_CODEC_H264);
AVH264DECODER_API void AVH264DECODER_DecoderClose(void*pHandle);
AVH264DECODER_API int  AVH264DECODER_DecoderDecode(void*pHandle,unsigned char*pStreamData,int nStreamLen,unsigned char*pYV12[],int nStride[],int*pnWidth,int*pnHeight,int*pnUsedBytes,int*pnKeyFrame,char**ppqscale_table,int*pqstride);


#endif