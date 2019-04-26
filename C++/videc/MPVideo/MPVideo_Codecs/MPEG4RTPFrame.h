//MPEG4RTPFrame.h
#ifndef __MPEG4RTPFRAME_H__
#define __MPEG4RTPFRAME_H__

#include "BaseRTPFrame.h"

class MPEG4RTPFrame
	: public BaseRTPFrame
{
public:
	MPEG4RTPFrame(BaseRTPFrameCallback&rCallback);
	virtual~MPEG4RTPFrame(void);
public:
	virtual void DoSendFrame(const unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nKeyFrame,unsigned long ulTimestamp);
protected:
	void nal_send(const unsigned char *buf, int size, int last);
	int handle_rtp_packet( unsigned char*pPayloadData,int nPayloadLen,unsigned char*pFrameBuf,int nBufLen);
	int get_codec_header_len(void);
};

#endif