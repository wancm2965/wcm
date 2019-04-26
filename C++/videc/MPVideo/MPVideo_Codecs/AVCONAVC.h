//AVCONAVC.h
#ifndef __AVCONAVC_H__
#define __AVCONAVC_H__

#include <AVCONAVC/AVCONAVC_Export.h>

#define AVC_BASELINE_PROFILE	0
#define AVC_MAIN_PROFILE		1
#define AVC_HIGH_PROFILE		2

#define AVC_CODEC_H263			0
#define AVC_CODEC_H264			1
#define AVC_CODEC_MPEG4			2

AVCONAVC_API void AVCONAVC_Initialize(void);
AVCONAVC_API void AVCONAVC_UnInitialize(void);

AVCONAVC_API void*AVCONAVC_EncoderOpen(int nWidth,int nHeight,int nFrameRate,int nProfile,int nMaxRTPPayloadSize,int nThreadCount,int nBitrate,int nQMin,int nQMax,int nKeyIntMax);
AVCONAVC_API void AVCONAVC_EncoderClose(void*pHandle);
AVCONAVC_API int  AVCONAVC_EncoderEncode(void*pHandle,unsigned char*pYV12[],unsigned char*pStreamBuf,int nBufLen,int* pnKeyFrame,int* pnBFrame,int*pCurQP);

AVCONAVC_API void*AVCONAVC_DecoderOpen(int nWidth,int nHeight,int AVCCodecID=AVC_CODEC_H264);
AVCONAVC_API void AVCONAVC_DecoderClose(void*pHandle);
AVCONAVC_API int  AVCONAVC_DecoderDecode(void*pHandle,unsigned char*pStreamData,int nStreamLen,unsigned char*pYV12[],int nStride[],int*pnWidth,int*pnHeight,int*pnUsedBytes,int*pnKeyFrame,char**ppqscale_table,int*pqstride);


class AVCONAVC_API AVCONAVC_LevelFixer
{
public:
	AVCONAVC_LevelFixer(void){};
	virtual~AVCONAVC_LevelFixer(void){};
public:
	virtual int Open(int nWidth,int nHeight,unsigned long nCPUFlags)=0;
	virtual void Close(void)=0;
	virtual void Process(unsigned char * src[3], int srcStride[3], unsigned char * dst[3],int dstStride[3],int horizontalSize, int verticalSize)=0;

	static AVCONAVC_LevelFixer*Create(void);
};

#endif