//XEncoderFrameI264.h
#ifndef __XENCODERFRAMEI264_H__
#define __XENCODERFRAMEI264_H__

#include "XEncoderFrame.h"
#include "H264RTPFrame.h"
#include "IntelCodecWrapper.h"


class XEncoderFrameI264 
	: public XEncoderFrame
	, public BaseRTPFrameCallback
{
public:
	XEncoderFrameI264(VIDEC_EncoderCallback&rCallback,bool bRTPPacket,VIDEC_CODEC_TYPE nCodecType);
	virtual ~XEncoderFrameI264(void);
public:
	int Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType);
	void Close(void);
	bool DoRTPPacket(void);
	void OnIntelH264EncoderCallback(unsigned char*pData,int nLen,int nKeyFrame);
protected:
	int DoEncodeFrame(unsigned char*pData[],int nStride[],unsigned char*pPacketData,int&nPacketLen,int&nKeyFrame,int&nBFrame);
	unsigned char GetCodecType(void);
	virtual bool AsyncEncoding(void){return true;};

	void OnBaseRTPFrameCallbackRTPPacket(void*pPacketData,int nPacketLen);
protected:
	H264RTPFrame*		m_pH264RTPFrame;

	VIDEC_CODEC_TYPE	m_nCodecType;

	void*				m_pEncoder;
};


#endif