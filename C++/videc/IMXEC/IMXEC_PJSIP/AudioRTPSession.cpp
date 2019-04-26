#include "AudioRTPSession.h"
#include "XSocket.h"
#include "SIPCallMgr.h"

AudioRTPSession::AudioRTPSession(AudioRTPSessionCallback&rCallback)
:m_rCallback(rCallback)
,m_pSession(NULL)
{
	m_bUsePJSIPRTP=true;
	m_pPJSIPRTPSession=NULL;
	m_bRecvPause=true;
	m_bUseMediaNetRTP=false;
	m_pMediaNetRTPSession=NULL;
}

AudioRTPSession::~AudioRTPSession(void)
{
}

void AudioRTPSession::InitRtpParam(int nMaxFrameSize, int nNetBand, int nFrameRate,int nMediaType,int nCodecType,int nSSRC,unsigned nClockRate ,int nSamplePerFrame,int nFlagReliable)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

	//add notice use medianetrtp ?
#else

	//add notice ?
	//if(nFlagReliable >0 ) m_bUseMediaNetRTP=true;

#endif

//notice mcu not use wait stable
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#else

	if(m_bUseMediaNetRTP)
	{
		//use medianetrtp
		m_pMediaNetRTPSession=MediaNet_RTPSession::Create(*this,NULL,nClockRate,nSamplePerFrame,0);
		if(m_pMediaNetRTPSession) m_pMediaNetRTPSession->InitRtpParam2(nMaxFrameSize,nNetBand, nFrameRate,nMediaType,nCodecType,nSSRC);
	}
#endif

}

int AudioRTPSession::Open(const char*cszLocalIP,unsigned short nLocalPort,int nFlagReliable,const std::string&strCodecName,int nRTPPayloadType 
						  ,pjmedia_endpt * pMediaEndpoint,unsigned nClockRate,int nSamplePerFrame)
{
	if(m_bUseMediaNetRTP)
	{
		if (m_pMediaNetRTPSession==NULL)
		{
			return -1;
		}
		if (m_pMediaNetRTPSession->Open(cszLocalIP,nLocalPort,nRTPPayloadType,strCodecName.c_str())!=0)
		{
			m_pMediaNetRTPSession->Close();
			delete m_pMediaNetRTPSession;
			m_pMediaNetRTPSession=NULL;
			return -1;
		}
	}
	else
	{
		if(pMediaEndpoint!=NULL)
		{
			m_bUsePJSIPRTP=true;
		}
		if(m_bUsePJSIPRTP)
		{
			m_pPJSIPRTPSession=PJSIP_RTPSession::Create(*this,pMediaEndpoint,nClockRate,nSamplePerFrame,0);
			if (m_pPJSIPRTPSession==NULL)
			{
				return -1;
			}
			if (m_pPJSIPRTPSession->Open(cszLocalIP,nLocalPort,nRTPPayloadType,strCodecName.c_str())!=0)
			{
				m_pPJSIPRTPSession->Close();
				delete m_pPJSIPRTPSession;
				m_pPJSIPRTPSession=NULL;
				return -1;
			}
		}
		else if (nFlagReliable)
		{
			m_pSession=NETEC_AudioRTPSession::Create(*this);
			if (m_pSession==NULL)
			{
				return -1;
			}
			if (m_pSession->Open(cszLocalIP,nLocalPort,nRTPPayloadType,strCodecName.c_str())!=0)
			{
				m_pSession->Close();
				delete m_pSession;
				m_pSession=NULL;
				return -1;
			}
		}
		else
		{
			RTPUDPv4TransmissionParams transparams;
			RTPSessionParams sessparams;
			sessparams.SetUsePollThread(true);
			sessparams.SetOwnTimestampUnit(1.0/90000.0);
			sessparams.SetMaximumPacketSize(64000);

			transparams.SetPortbase(nLocalPort);
			transparams.SetBindIP(XSocket::atoh(cszLocalIP));

			Create(sessparams,&transparams);
		}
	}
	return 0;
}

int AudioRTPSession::PJSIPICEOpen(pjmedia_endpt * pMediaEndpoint,int nUseICE,pjmedia_transport * pMediaTransport)
{
	if(m_bUseMediaNetRTP)
	{
		return 0;
	}
	else if(pMediaEndpoint!=NULL)
	{
		m_bUsePJSIPRTP=true;
	}
	if(m_bUsePJSIPRTP)
	{
		m_pPJSIPRTPSession=PJSIP_RTPSession::Create(*this,pMediaEndpoint,0,0,nUseICE);
		if (m_pPJSIPRTPSession==NULL)
		{
			return -1;
		}
		if (m_pPJSIPRTPSession->PJSIPICEOpen(pMediaTransport)!=0)
		{
			m_pPJSIPRTPSession->Close();
			delete m_pPJSIPRTPSession;
			m_pPJSIPRTPSession=NULL;
			return -1;
		}
	}
	return 0;
}

void AudioRTPSession::AddParam(unsigned char nPayloadType,unsigned nClockRate,int nSamplePerFrame)
{
	//add
	if(m_bUseMediaNetRTP)
	{
		if (m_pMediaNetRTPSession)
		{
			m_pMediaNetRTPSession->AddParam(nPayloadType,nClockRate,nSamplePerFrame);
		}
	}
	else if(m_bUsePJSIPRTP)
	{
		if (m_pPJSIPRTPSession)
		{
			m_pPJSIPRTPSession->AddParam(nPayloadType,nClockRate,nSamplePerFrame);
		}
	}
}

int AudioRTPSession::PJSIPEncodeInOfferSDP(pj_pool_t *sdp_pool,int nMediaIndex,pjmedia_sdp_session *sdp,int nCmpCount)
{
	//add
	int status=-1;
	if(m_bUseMediaNetRTP)
	{
		return 0;
	}
	else if(m_bUsePJSIPRTP)
	{
		if (m_pPJSIPRTPSession)
		{
			status=m_pPJSIPRTPSession->PJSIPEncodeInOfferSDP(sdp_pool,sdp,nMediaIndex,nCmpCount);
		}
	}
	return status;
}

int AudioRTPSession::PJSIPEncodeInAnswerSDP(pj_pool_t *sdp_pool,int nMediaIndex,pjmedia_sdp_session * localSdp,pjmedia_sdp_session * remSdp,int nCmpCount)
{
	//add
	int status=-1;
	if(m_bUseMediaNetRTP)
	{
		return 0;
	}
	else if(m_bUsePJSIPRTP)
	{
		if (m_pPJSIPRTPSession)
		{
			status=m_pPJSIPRTPSession->PJSIPEncodeInAnswerSDP(sdp_pool,localSdp,remSdp,nMediaIndex,nCmpCount);
		}
	}
	return status;
}

void AudioRTPSession::Close(void)
{
	//add
	if(m_bUseMediaNetRTP)
	{
		if (m_pMediaNetRTPSession)
		{
			m_pMediaNetRTPSession->Close();
			delete m_pMediaNetRTPSession;
			m_pMediaNetRTPSession=NULL;
		}
	}
	else
	{
		if(m_bUsePJSIPRTP)
		{
			if (m_pPJSIPRTPSession)
			{
				m_pPJSIPRTPSession->Close();
				delete m_pPJSIPRTPSession;
				m_pPJSIPRTPSession=NULL;
			}
		}
		else if (m_pSession)
		{
			m_pSession->Close();
			delete m_pSession;
			m_pSession=NULL;
		}
		else
		{
			BYEDestroy(RTPTime(0,1),NULL,0);
		}
	}
}

void AudioRTPSession::AddRemote(const char*cszRemoteIP,unsigned short nRemotePort,
								pj_pool_t *tmp_pool,pjmedia_sdp_session *sdp_local,pjmedia_sdp_session *sdp_remote,int media_index,int nCmpCount)
{
	//add
	if(m_bUseMediaNetRTP)
	{
		if (m_pMediaNetRTPSession)
		{
			m_pMediaNetRTPSession->AddRemote(cszRemoteIP,nRemotePort,tmp_pool,sdp_local,sdp_remote,media_index,nCmpCount);
		}
	}
	else
	{
		if(m_bUsePJSIPRTP)
		{
			if (m_pPJSIPRTPSession)
			{
				m_pPJSIPRTPSession->AddRemote(cszRemoteIP,nRemotePort,tmp_pool,sdp_local,sdp_remote,media_index,nCmpCount);
			}
		}
		else if (m_pSession)
		{
			m_pSession->AddRemote(cszRemoteIP,nRemotePort);
		}
		else
		{
			ClearDestinations();

			RTPIPv4Address dstAddr(XSocket::atoh(cszRemoteIP),nRemotePort);
			AddDestination(dstAddr);
		}
	}
}

void AudioRTPSession::SendRTPPacket(const void *data,size_t len,uint8_t pt,bool mark,uint32_t timestampinc,unsigned char*pPacketData,int nPacketLen,unsigned char nRepeatCount)
{
	//add
	if(m_bUseMediaNetRTP)
	{
		if (m_pMediaNetRTPSession)
		{
			m_pMediaNetRTPSession->SendRTPPacket(data,len,pt,mark,timestampinc,pPacketData,nPacketLen);
		}
	}
	else
	{
		if(m_bUsePJSIPRTP)
		{
			if (m_pPJSIPRTPSession)
			{
				m_pPJSIPRTPSession->SendRTPPacket(data,len,pt,mark,timestampinc,pPacketData,nPacketLen,nRepeatCount);
			}
		}
		else if (m_pSession)
		{
			for (unsigned char i=0;i<=nRepeatCount;++i)
			{
				m_pSession->SendRTPPacket(pPacketData,nPacketLen);
			}
		}
		else
		{
			SendPacket(data,len,pt,mark,timestampinc,nRepeatCount);
		}
	}
}

void AudioRTPSession::OnPollThreadStep()
{
	BeginDataAccess();
		
	if (GotoFirstSourceWithData())
	{
		do
		{
			RTPPacket *pack;
			RTPSourceData *srcdat;
			
			srcdat = GetCurrentSourceInfo();
			
			while ((pack = GetNextPacket()) != NULL)
			{
				ProcessRTPPacket(*srcdat,*pack);
				DeletePacket(pack);
			}
		} while (GotoNextSourceWithData());
	}
		
	EndDataAccess();
}

void AudioRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	if(!m_bRecvPause)
	m_rCallback.OnAudioRTPSessionCallbackRTPPacket(this,rtppack.GetPacketData(),rtppack.GetPacketLength(),rtppack.GetPayloadType(),rtppack.GetPacketLength()-rtppack.GetPayloadLength(),rtppack.GetSequenceNumber(),rtppack.GetTimestamp());
}

void AudioRTPSession::OnNETEC_AudioRTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int32_t nRTPPacketLen,int32_t nPayloadType,int32_t nRTPHeaderLen,unsigned short usSequence,uint32_t ulTimestamp)
{
	if(!m_bRecvPause)
	m_rCallback.OnAudioRTPSessionCallbackRTPPacket(this,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
}
//add
void AudioRTPSession::OnPJSIP_RTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	if(!m_bRecvPause)
	m_rCallback.OnAudioRTPSessionCallbackRTPPacket(this,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
}

//add
void AudioRTPSession::OnMediaNet_RTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	if(!m_bRecvPause)
	m_rCallback.OnAudioRTPSessionCallbackRTPPacket(this,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
}

void AudioRTPSession::PacketRecvPause()
{
	m_bRecvPause=true;
	if(m_bUseMediaNetRTP)
	{
		if (m_pMediaNetRTPSession)
		{
			m_pMediaNetRTPSession->SendRecvPause();
		}
	}
}

void AudioRTPSession::PacketRecvRestart()
{
	m_bRecvPause=false;
	if(m_bUseMediaNetRTP)
	{
		if (m_pMediaNetRTPSession)
		{
			m_pMediaNetRTPSession->SendRecvStart();
		}
	}
}
