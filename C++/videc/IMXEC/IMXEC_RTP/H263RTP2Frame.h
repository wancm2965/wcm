//H263RTP2Frame.h
#ifndef __H263RTP2FRAME_H__
#define __H263RTP2FRAME_H__

#include "BaseRTP2Frame.h"

//RFC2190


class H263RTP2Frame
	: public BaseRTP2Frame
{
public:
	H263RTP2Frame(BaseRTPFrameCallback&rCallback);
	virtual~H263RTP2Frame(void);
protected:
	int handle_rtp_packet(unsigned char*pPayloadData,int nPayloadLen,unsigned char*pFrameBuf,int nBufLen);
protected:
	unsigned char			m_cPacketSpan;
};

#endif