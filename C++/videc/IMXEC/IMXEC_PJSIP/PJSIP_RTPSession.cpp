
#include "PJSIP_RTPSession.h"
#include "PJSIP_RTP.h"

PJSIP_RTPSession* PJSIP_RTPSession::Create(PJSIP_RTPSessionCallback&rCallback,pjmedia_endpt * pMediaEndpoint,unsigned nClockRate,int nSamplePerFrame,int nUseICE)
{
	PJSIP_RTPSession * pPJSIP_RTPSession=NULL;
	pPJSIP_RTPSession=new PJSIP_RTP(rCallback,pMediaEndpoint,nClockRate,nSamplePerFrame,nUseICE);
	return pPJSIP_RTPSession;
}