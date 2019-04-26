//XEncoderFrameH263P.h
#ifndef __XENCODERFRAMEH263P_H__
#define __XENCODERFRAMEH263P_H__

#include "XEncoderFrame.h"
#include "H263PRTPFrame.h"
#include "VBRControl.h"

class XEncoderFrameH263P 
	: public XEncoderFrame
	, public BaseRTPFrameCallback
{
public:
	XEncoderFrameH263P(VIDEC_EncoderCallback&rCallback,bool bRTPPacket);
	virtual ~XEncoderFrameH263P(void);
public:
	int Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType);
	void Close(void);
	bool DoRTPPacket(void);
	void OnBaseRTPFrameCallbackRTPPacket(void*pPacketData,int nPacketLen);
protected:
	int DoEncodeFrame(unsigned char*pData[],int nStride[],unsigned char*pPacketData,int&nPacketLen,int&nKeyFrame,int&nBFrame);
	unsigned char GetCodecType(void);
protected:
	void*			m_hEncoder;

	H263PRTPFrame*	m_pH263PRTPFrame;
	VBRControl		m_VBRControl;
};


#endif