
#include "MediaNet_RTPSession.h"
#include "MediaNet_RTP.h"

MediaNet_RTPSession* MediaNet_RTPSession::Create(MediaNet_RTPSessionCallback&rCallback,void * pMediaEndpoint,unsigned nClockRate,int nSamplePerFrame,int nUseICE)
{
	MediaNet_RTPSession * pMediaNet_RTPSession=NULL;
	pMediaNet_RTPSession=new MediaNet_RTP(rCallback,pMediaEndpoint,nClockRate,nSamplePerFrame,nUseICE);
	return pMediaNet_RTPSession;
}