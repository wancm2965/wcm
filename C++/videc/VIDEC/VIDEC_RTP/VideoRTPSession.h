//VideoRTPSession.h
#ifndef __VIDEORTPSESSION_H__
#define __VIDEORTPSESSION_H__

#include "../../IMXEC/jrtplib/src/rtpsession.h"
#include "../../IMXEC/jrtplib/src/rtppacket.h"
#include "../../IMXEC/jrtplib/src/rtpudpv4transmitter.h"
#include "../../IMXEC/jrtplib/src/rtpipv4address.h"
#include "../../IMXEC/jrtplib/src/rtpsessionparams.h"
#include "../../IMXEC/jrtplib/src/rtperrors.h"
#ifndef WIN32
	#include <netinet/in.h>
	#include <arpa/inet.h>
#else
	#include <winsock2.h>
#endif // WIN32
#include "../../IMXEC/jrtplib/src/rtpsourcedata.h"

class VideoRTPSession;
class VideoRTPSessionCallback
{
public:
	VideoRTPSessionCallback(void){};
	virtual~VideoRTPSessionCallback(void){};
public:
	virtual void OnVideoRTPSessionCallbackRTPPacket(VideoRTPSession*pVideoRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)=0;
};

class VideoRTPSession
	: public RTPSession
{
public:
	VideoRTPSession(VideoRTPSessionCallback&rCallback);
	virtual~VideoRTPSession(void);
protected:
	virtual void OnPollThreadStep();
	virtual void ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack);
protected:
	VideoRTPSessionCallback&	m_rCallback;
};

#endif