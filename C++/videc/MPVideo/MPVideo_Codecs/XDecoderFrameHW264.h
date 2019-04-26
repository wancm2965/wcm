#ifndef __XDECODERFRAMEHW264_H__
#define __XDECODERFRAMEHW264_H__

#include "XDecoderFrame.h"
#include "HWVideoDecoder.h"

class XDecoderFrameHW264 
	: public XDecoderFrame
	, public HWVideoDecoderCallback
{
public:
	XDecoderFrameHW264(MPVideo_DecoderCallback&rCallback);
	virtual ~XDecoderFrameHW264(void);
public:
	int Open(int nWidth,int nHeight);
	void Close(void);
	int DecodeFramePacket(void*pPacketData,int nPacketSize);
	int DecodeRTPPacket(void*pPacketData,int nPacketSize);

	void OnHWVideoDecoderCallback(unsigned char*pData,int nLen,int nWidth,int nHeight,int nStride);
protected:
	HWVideoDecoder*					m_pVideoDecoder;
};


XDecoderFrame*CreateXDecoderFrameHW264(MPVideo_DecoderCallback&rCallback);


#endif