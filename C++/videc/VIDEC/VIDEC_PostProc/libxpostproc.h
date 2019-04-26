//libxpostproc.h
#ifndef __LIBXPOSTPROC_H__
#define __LIBXPOSTPROC_H__

void VIDEC_PostProcInit(void);
void VIDEC_PostProcTerminate(void);

class IXDeinterlace
{
public:
	IXDeinterlace(void){};
	virtual~IXDeinterlace(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual bool Deinterlace(unsigned char * src[3], int srcStride[3], unsigned char * dst[3],int dstStride[3],int horizontalSize, int verticalSize)=0;
};

IXDeinterlace*CreateIXDeinterlace(int nWidth,int nHeight);


class IXDenoise
{
public:
	IXDenoise(void){};
	virtual~IXDenoise(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual bool Denoise(unsigned char * src[3], int srcStride[3], unsigned char * dst[3],int dstStride[3],int horizontalSize, int verticalSize)=0;
};

IXDenoise*CreateIXDenoise(int nWidth,int nHeight);

class IXPostProcess
{
public:
	IXPostProcess(void){};
	virtual~IXPostProcess(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual bool PostProcess(unsigned char * src[3], int srcStride[3], unsigned char * dst[3],int dstStride[3],int horizontalSize, int verticalSize,char*qscale_table,int qstride)=0;
	virtual void SetLevel(int nLevel)=0;
};

IXPostProcess*CreateIXPostProcess(int nWidth,int nHeight,int nLevel);


#endif