//VIDEC_convert.h
#ifndef __VIDEC_CONVERT_H__
#define __VIDEC_CONVERT_H__

#include <VIDEC\VIDEC_Export.h>

VIDEC_API int yuv_to_yuy2(unsigned char*src[],int src_stride[],unsigned char*dst,int dst_stride,int width,int height);


class VIDEC_Resize
{
public:
	VIDEC_Resize(void){};
	virtual~VIDEC_Resize(void){};
public:
	virtual int Open(void)=0;
	virtual void Close(void)=0;
	virtual int Resize(unsigned char*src[],int nSrcWidth,int nSrcHeight,unsigned char*dst[],int nDstWidth,int nDstHeight,int nSrcStride=0,int nDstStride=0)=0;

	static VIDEC_Resize*Create(void);
};

void Frame2Field(unsigned char*pSrc,unsigned char*pDst,int nWidth,int nHeight);
//704X576->352X576
void HalfWidthScaleFrame2Field(unsigned char*pSrc,unsigned char*pDst,int nWidth,int nHeight);


class VIDEC_Denoise
{
public:
	VIDEC_Denoise(void){};
	virtual~VIDEC_Denoise(void){};
public:
	virtual int Open(int nWidth,int nHeight)=0;
	virtual void Close(void)=0;
	virtual int Denoise(unsigned char*src[],unsigned char*dst[])=0;

	static VIDEC_Denoise*Create(void);
};

#endif