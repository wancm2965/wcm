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

	//Ƶ�������˳����鵼���ڴ������������
	// (FFMpeg����bug��ʹ����ȫ�ֵ����ü����������Ƿ��ͷ��ڴ���Դ
	//  ����XDecoderFrameH264����ĳ�Ա���������޷���Ч)
	static XCritSec g_GroupLock;
#ifdef REC_H264
	FILE*			m_pFile264;
#endif
};

#endif
