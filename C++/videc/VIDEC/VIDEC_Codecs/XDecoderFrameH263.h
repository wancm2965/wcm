//XDecoderFrameH263.h
#ifndef __XDECODERFRAMEH263_H__
#define __XDECODERFRAMEH263_H__

#include "XDecoderFrame.h"
#include "libxpostproc.h"
#include "H263RTPFrame.h"
#include "H263PRTPFrame.h"

class XDecoderFrameH263
	: public XDecoderFrame
	, public BaseRTPFrameCallback
{
public:
	XDecoderFrameH263(VIDEC_DecoderCallback&rCallback,int nH263Plus);
	virtual~XDecoderFrameH263(void);
public:
	int Open(int nWidth,int nHeight);
	void Close(void);
	int DecodeFramePacket(void*pPacketData,int nPacketSize);
	int DecodeRTPPacket(void*pPacketData,int nPacketSize);
	void OnBaseRTPFrameCallbackFramePacket(BaseRTPFrame*pBaseRTPFrame,void*pPacketData,int nPacketLen,int nKeyFrame,int nFrameRate);
protected:
    void*			m_pHandle;
	int				m_nH263Plus;
	BaseRTPFrame*	m_pRTPFrame;
	int				m_nLastFrameIsKeyFrame;
	bool			m_bGotFirstKeyFrame;
	unsigned short	m_usSeqNo;
	int				m_nFrameRateActual;
};


#endif
