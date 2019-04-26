//XEncoderFrameH264.h
#ifndef __XENCODERFRAMEH264_H__
#define __XENCODERFRAMEH264_H__

#include "XEncoderFrame.h"
#include "H264RTPFrame.h"
#include "XBitrateExt.h"

//#define REC_H264_YUV	

class XEncoderFrameH264 
	: public XEncoderFrame
	, public BaseRTPFrameCallback
{
public:
	XEncoderFrameH264(VIDEC_EncoderCallback&rCallback,bool bRTPPacket,VIDEC_CODEC_TYPE nCodecType);
	virtual ~XEncoderFrameH264(void);
public:
	int Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType);
	void Close(void);
	bool DoRTPPacket(void);
protected:
	int DoEncodeFrame(unsigned char*pData[],int nStride[],unsigned char*pPacketData,int&nPacketLen,int&nKeyFrame,int&nBFrame);
	unsigned char GetCodecType(void);

	void OnBaseRTPFrameCallbackRTPPacket(void*pPacketData,int nPacketLen);
protected:
	void*			m_hEncoder;
	bool			m_bMobile;

	H264RTPFrame*	m_pH264RTPFrame;


	VIDEC_CODEC_TYPE m_nCodecType;


	int				m_nLastKeyFrameCount;

#ifdef REC_H264_YUV
	FILE*			m_pFileYUV;
	FILE*			m_pFile264;
#endif
};


#endif