#ifndef __XDECODERFRAMEAH400_H__
#define __XDECODERFRAMEAH400_H__

#include "XDecoderFrame.h"


class XDecoderFrameAH400 
	: public XDecoderFrame
{
public:
	XDecoderFrameAH400(MPVideo_DecoderCallback&rCallback,VIDEC_CODEC_TYPE CodecType);
	virtual ~XDecoderFrameAH400(void);
public:
	int Open(int nWidth,int nHeight);
	void Close(void);
	int DecodeFramePacket(void*pPacketData,int nPacketSize);
	int DecodeRTPPacket(void*pPacketData,int nPacketSize);

	void OnImageData(unsigned char*pData,int nLen,int nWidth,int nHeight,long nStamp);
protected:
	unsigned char*					m_pImageBuf;
	int								m_nImageBufLen;								

	bool							m_bUserError;

	void*							m_pHandle;
	VIDEC_CODEC_TYPE				m_CodecType;
};


XDecoderFrame*CreateXDecoderFrameAH400(MPVideo_DecoderCallback&rCallback,VIDEC_CODEC_TYPE CodecType);


#endif