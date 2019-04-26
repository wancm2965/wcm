//XEncoderFrameMpeg4.h
#ifndef __XENCODERFRAMEMPEG4_H__
#define __XENCODERFRAMEMPEG4_H__

#include "XEncoderFrame.h"

class XEncoderFrameMPEG4 
	: public XEncoderFrame
{
public:
	XEncoderFrameMPEG4(VIDEC_EncoderCallback&rCallback,bool bRTPPacket);
	virtual ~XEncoderFrameMPEG4(void);
public:
	int Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType);
	void Close(void);
	bool DoRTPPacket(void);
protected:
	int DoEncodeFrame(unsigned char*pData[],int nStride[],unsigned char*pPacketData,int&nPacketLen,int&nKeyFrame,int&nBFrame);
	unsigned char GetCodecType(void);
protected:
	void*			m_hEncoder;
};


#endif