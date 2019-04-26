#ifndef __XDECODERFRAMEUSER_H__
#define __XDECODERFRAMEUSER_H__

#include "XDecoderFrame.h"


class XDecoderFrameUser 
	: public XDecoderFrame
{
public:
	XDecoderFrameUser(VIDEC_DecoderCallback&rCallback);
	virtual ~XDecoderFrameUser(void);
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
};


XDecoderFrame*CreateXDecoderFrameUser(VIDEC_DecoderCallback&rCallback);


#endif