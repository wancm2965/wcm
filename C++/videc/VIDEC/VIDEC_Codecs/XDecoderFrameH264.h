//XDecoderFrameH264.h
#ifndef __XDECODERFRAMEH264_H__
#define __XDECODERFRAMEH264_H__

#include <stdio.h>
#include <stdlib.h>

#include "XDecoderFrame.h"
#include "libxpostproc.h"
#include "H264RTPFrame.h"


//#define REC_H264

class XDecoderFrameH264
	: public XDecoderFrame
	, public BaseRTPFrameCallback
{
public:
	XDecoderFrameH264(VIDEC_DecoderCallback&rCallback);
	virtual~XDecoderFrameH264(void);
public:
	int Open(int nWidth,int nHeight);
	void Close(void);
	int DecodeFramePacket(void*pPacketData,int nPacketSize);
	int DecodeRTPPacket(void*pPacketData,int nPacketSize);

	void OnBaseRTPFrameCallbackFramePacket(BaseRTPFrame*pBaseRTPFrame,void*pPacketData,int nPacketLen,int nKeyFrame,int nFrameRate);
protected:
	int GetPostProcQuality(void);
protected:
    void*			m_pHandle;
	H264RTPFrame*	m_pH264RTPFrame;
	int				m_nLastFrameIsKeyFrame;
	bool			m_bGotFirstKeyFrame;
	unsigned short	m_usSeqNo;

	int				m_nFrameRateActual;

	//频繁进入退出分组导致内存增长问题加锁
	// (FFMpeg存在bug，使用了全局的引用计数来控制是否释放内存资源
	//  所以XDecoderFrameH264对象的成员变量锁，无法起效)
	static XCritSec g_GroupLock;
#ifdef REC_H264
	FILE*			m_pFile264;
#endif
};

#endif
