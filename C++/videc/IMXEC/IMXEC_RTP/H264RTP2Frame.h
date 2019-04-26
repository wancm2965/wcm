//H264RTP2Frame.h
#ifndef __H264RTP2FRAME_H__
#define __H264RTP2FRAME_H__

#include "BaseRTP2Frame.h"

//RFC3984


class H264RTP2Frame
	: public BaseRTP2Frame
{
public:
	H264RTP2Frame(BaseRTPFrameCallback&rCallback);
	virtual~H264RTP2Frame(void);
protected:
	int handle_rtp_packet( unsigned char*pPayloadData,int nPayloadLen,unsigned char*pFrameBuf,int nBufLen);
};

#endif