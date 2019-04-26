#ifndef __XDECODERFRAMEDH264_H__
#define __XDECODERFRAMEDH264_H__

#include "XDecoderFrame.h"


class XDecoderFrameDH264 
	: public XDecoderFrame
{
public:
	XDecoderFrameDH264(MPVideo_DecoderCallback&rCallback);
	virtual ~XDecoderFrameDH264(void);
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


XDecoderFrame*CreateXDecoderFrameDH264(MPVideo_DecoderCallback&rCallback);


#endif