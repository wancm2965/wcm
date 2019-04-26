//XDecoderFrameH264.h
#ifndef __XDECODERFRAMEH264_H__
#define __XDECODERFRAMEH264_H__

#include <stdio.h>
#include <stdlib.h>

#include "XDecoderFrame.h"
#include "libxpostproc.h"
#include "H264RTPFrame.h"


class XDecoderFrameH264
	: public XDecoderFrame
	, public BaseRTPFrameCallback
{
public:
	XDecoderFrameH264(MPVideo_DecoderCallback&rCallback);
	virtual~XDecoderFrameH264(void);
public:
	int Open(int nWidth,int nHeight);
	void Close(void);
	int DecodeFramePacket(void*pPacketData,int nPacketSize);
	int DecodeRTPPacket(void*pPacketData,int nPacketSize);

	void OnBaseRTPFrameCallbackFramePacket(BaseRTPFrame*pBaseRTPFrame,void*pPacketData,int nPacketLen,int nKeyFrame);
protected:
	int GetPostProcQuality(void);
protected:
    void*			m_pHandle;
	H264RTPFrame*	m_pH264RTPFrame;
	int				m_nLastFrameIsKeyFrame;
	bool			m_bGotFirstKeyFrame;
	unsigned short	m_usSeqNo;

	unsigned long		m_nLastTS;

};

#endif
