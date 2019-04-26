//XDecoderFrameMPEG4.h
#ifndef __XDECODERFRAMEMPEG4_H__
#define __XDECODERFRAMEMPEG4_H__

#include "XDecoderFrame.h"
#include "libxpostproc.h"

#include <AVCONAVC/AVCONAVC.h>

class XDecoderFrameMPEG4
	: public XDecoderFrame
{
public:
	XDecoderFrameMPEG4(VIDEC_DecoderCallback&rCallback);
	virtual~XDecoderFrameMPEG4(void);
public:
	int Open(int nWidth,int nHeight);
	void Close(void);
	int DecodeFramePacket(void*pPacketData,int nPacketSize);
	int DecodeRTPPacket(void*pPacketData,int nPacketSize);
protected:
    void*			m_pHandle;
	int				m_nLastFrameIsKeyFrame;
	bool			m_bGotFirstKeyFrame;
	unsigned short	m_usSeqNo;
};


#endif
