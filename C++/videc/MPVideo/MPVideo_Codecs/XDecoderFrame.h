//XDecoderFrame.h
#ifndef __XDECODERFRAME_H__
#define __XDECODERFRAME_H__

#include <MPVideo/MPVideo_Codec.h>
#include "libxpostproc.h"

class XDecoderFrame
	: public MPVideo_Decoder
{
public:
	XDecoderFrame(MPVideo_DecoderCallback&rCallback);
	virtual~XDecoderFrame(void);
public:
	virtual int Open(int nWidth,int nHeight)=0;
	virtual void Close(void);
protected:
	virtual int OnResized(int nWidth,int nHeight);
	virtual int PostProcess(unsigned char*pDecodedFrameData[],int nDecodedStride[],char*qscale_table,int qstride);
	virtual int GetPostProcQuality(void);
protected:
	MPVideo_DecoderCallback&	m_rCallback;
	int							m_nWidth;
	int							m_nHeight;
	int							m_nFrameSize;
	unsigned char*				m_pFrameData;
	unsigned char*				m_pData[3];
	int							m_nStride[3];
	int							m_nYImageSize;

	IXPostProcess*				m_pIXPostProcess;
	//bool						m_bLevelFixer;
};

#endif
