//H263RTPFrame.h
#ifndef __H263RTPFRAME_H__
#define __H263RTPFRAME_H__

#include "BaseRTPFrame.h"


class H263RTPFrame
	: public BaseRTPFrame
{
public:
	H263RTPFrame(BaseRTPFrameCallback&rCallback,int nMaxBitrate=6144);
	virtual~H263RTPFrame(void);
public:
	virtual void DoSendFrame(const unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nKeyFrame,unsigned long ulTimestamp);
protected:
	int handle_rtp_packet(unsigned char*pPayloadData,int nPayloadLen,unsigned char*pFrameBuf,int nBufLen);
	int get_codec_header_len(void);
protected:
	unsigned char			m_cPacketSpan;
};

#endif