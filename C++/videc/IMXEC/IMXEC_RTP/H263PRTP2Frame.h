//H263PRTP2Frame.h
#ifndef __H263PRTP2FRAME_H__
#define __H263PRTP2FRAME_H__

#include "BaseRTP2Frame.h"

class H263PRTP2Frame
	: public BaseRTP2Frame
{
public:
	H263PRTP2Frame(BaseRTPFrameCallback&rCallback);
	virtual~H263PRTP2Frame(void);
public:
	virtual int GetFrame(unsigned char*pFrameBuf,int nBufLen);
protected:
	int handle_rtp_packet(unsigned char*pPayloadData,int nPayloadLen,unsigned char*pFrameBuf,int nBufLen);
protected:
	unsigned long	m_nLastRequestKeyFrameTimestamp;
};

#endif