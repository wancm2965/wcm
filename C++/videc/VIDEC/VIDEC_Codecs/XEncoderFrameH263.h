//XEncoderFrameH263.h
#ifndef __XENCODERFRAMEH263_H__
#define __XENCODERFRAMEH263_H__

#include "XEncoderFrame.h"
#include "VBRControl.h"
#include "H263RTPFrame.h"
#include "XDataPool.h"


class XEncoderFrameH263 
	: public XEncoderFrame
	, public XDataPool
{
public:
	XEncoderFrameH263(VIDEC_EncoderCallback&rCallback,bool bRTPPacket);
	virtual ~XEncoderFrameH263(void);
public:
	void OnRTPCallback(void* pRTPContext,void *data, int size,int keyframe,int marker,int mb_count);
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

	VBRControl		m_VBRControl;

	int				m_nFormat;
	unsigned char*	m_pRTPPacketBuffer;
	unsigned char*	m_pPayloadBuffer;
	int				m_nMaxPacketsToSend;
	unsigned long	m_nLastSentPacketTimestamp;
	unsigned long	m_nRTPTimestamp;
	unsigned short	m_nRTPSeqNo;
	bool			m_bEncoding;
	int				m_nLastKeyFrameCount;

	int				m_nMaxBitrate;
};


#endif