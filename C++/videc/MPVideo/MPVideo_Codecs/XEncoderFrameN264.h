//XEncoderFrameN264.h
#ifndef __XENCODERFRAMEN264_H__
#define __XENCODERFRAMEN264_H__

#include "XEncoderFrame.h"
#include "H264RTPFrame.h"
#include "./CUDA/CUDAVideoEncoder.h"


class XEncoderFrameN264 
	: public XEncoderFrame
	, public BaseRTPFrameCallback
	, public CUDAVideoEncoderCallback
{
public:
	XEncoderFrameN264(MPVideo_EncoderCallback&rCallback,bool bRTPPacket,VIDEC_CODEC_TYPE nCodecType);
	virtual ~XEncoderFrameN264(void);
public:
	int Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType);
	void Close(void);
	bool DoRTPPacket(void);
protected:
	int DoEncodeFrame(unsigned char*pData[],int nStride[],unsigned char*pPacketData,int&nPacketLen,int&nKeyFrame,int&nBFrame);
	unsigned char GetCodecType(void);
	virtual bool AsyncEncoding(void){return true;};

	void OnBaseRTPFrameCallbackRTPPacket(void*pPacketData,int nPacketLen);
	void OnCUDAVideoEncoderCallback(unsigned char*pData,int nLen,int nKeyFrame);
protected:
	H264RTPFrame*		m_pH264RTPFrame;

	VIDEC_CODEC_TYPE	m_nCodecType;

	CUDAVideoEncoder*	m_pCUDAVideoEncoder;
};

extern bool TryCUDAH264HardwareEncoding(void);

#endif