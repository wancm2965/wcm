//MPVideo_convert.h
#ifndef __MPVIDEO_CONVERT_H__
#define __MPVIDEO_CONVERT_H__


int yuv_to_yuy2(unsigned char*src[],int src_stride[],unsigned char*dst[],int dst_stride[],int width,int height);


class MPVideo_Resize
{
public:
	MPVideo_Resize(void){};
	virtual~MPVideo_Resize(void){};
public:
	virtual int Open(void)=0;
	virtual void Close(void)=0;
	virtual int Resize(unsigned char*src[],int nSrcWidth,int nSrcHeight,unsigned char*dst[],int nDstWidth,int nDstHeight)=0;

	static MPVideo_Resize*Create(void);
};

void Frame2Field(unsigned char*pSrc,unsigned char*pDst,int nWidth,int nHeight);
//704X576->352X576
void HalfWidthScaleFrame2Field(unsigned char*pSrc,unsigned char*pDst,int nWidth,int nHeight);


class MPVideo_Denoise
{
public:
	MPVideo_Denoise(void){};
	virtual~MPVideo_Denoise(void){};
public:
	virtual int Open(int nWidth,int nHeight)=0;
	virtual void Close(void)=0;
	virtual int Denoise(unsigned char*src[],unsigned char*dst[])=0;

	static MPVideo_Denoise*Create(void);
};

#endif