//H263PRTPFrame.h
#ifndef __H263PRTPFRAME_H__
#define __H263PRTPFRAME_H__

#include "BaseRTPFrame.h"

class H263PRTPFrame
	: public BaseRTPFrame
{
public:
	H263PRTPFrame(BaseRTPFrameCallback&rCallback,int nMaxBitrate=6144);
	virtual~H263PRTPFrame(void);
public:
	virtual void DoSendFrame(const unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nKeyFrame,unsigned long ulTimestamp);
protected:
	int handle_rtp_packet(unsigned char*pPayloadData,int nPayloadLen,unsigned char*pFrameBuf,int nBufLen);
	void rfc4629_generate_follow_on_packets(unsigned char *psc, int len, int last_packet);
	int get_codec_header_len(void);
};

#endif