//PJSIP_RTP.h
#ifndef __PJSIP_RTP_H__
#define __PJSIP_RTP_H__

#include "PJSIP_RTPSession.h"
#include <pjmedia.h>
#include <string>

class PJSIP_RTP
	: public PJSIP_RTPSession
{
public:
	PJSIP_RTP(PJSIP_RTPSessionCallback&rCallback,pjmedia_endpt * pMediaEndpoint
		 ,unsigned nClockRate,int nSamplePerFrame,int nUseICE);

	virtual~PJSIP_RTP(void){};
public:
	virtual int Open(const char*cszLocalIP,unsigned short nLocalPort,unsigned char nPayloadType,const char*cszPayloadName);
	//use ice pMediaTransport �ǵ�pjmedia_ice_create3�����Ĳ���
	virtual int PJSIPICEOpen(pjmedia_transport * pMediaTransport);
	virtual void Close(void);
	virtual void AddRemote(const char*cszRemoteIP,unsigned short nRemotePort,
		pj_pool_t *tmp_pool,pjmedia_sdp_session *sdp_local,pjmedia_sdp_session *sdp_remote,int media_index,int nCmpCount);
	virtual void SendRTPPacket(const void *data,size_t len,unsigned char pt,bool mark,unsigned int timestampinc,unsigned char*pPacketData,int nPacketLen,unsigned char nRepeatCount=0);
	virtual void OnRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	//after open is call the function
	virtual int PJSIPEncodeInOfferSDP(pj_pool_t *sdp_pool,pjmedia_sdp_session *sdp,int nMediaIndex,int nCmpCount);
	virtual int PJSIPEncodeInAnswerSDP(pj_pool_t *sdp_pool,pjmedia_sdp_session * localSdp,pjmedia_sdp_session * remSdp,int nMediaIndex,int nCmpCount);
	//��PJSIPICEOpen��������������������ô˺���
	virtual void AddParam(unsigned char nPayloadType,unsigned nClockRate,int nSamplePerFrame);
public:
	bool							m_bTransportAttach;
	pjmedia_rtp_session				m_RtpRecvSession;					//incoming RTP session	
	pjmedia_rtp_session				m_RtpSendSession;					//outgoing RTP session
	pjmedia_rtcp_session			m_RtcpSession;						//incoming RTCP session

private:
	void InitRtpFromParam(unsigned char nPayloadType);
	//����SDP���ؽ���RTP��ַ,���ڵ��Է�û��ICE����ʱ
	int ICELocalSDPAddrUpdate(pj_pool_t *sdp_pool,pjmedia_sdp_session *sdp_local,int nMediaIndex,int comp_cnt);
	//���Է���û��ICE���ԣ���û��������ICEԶ�˴����ַ,��ICE���Է���true,���ڵ��Է�û��ICE����ʱ
	bool ICECheckCandAddRemoteAddr(pj_pool_t *sdp_pool,pjmedia_sdp_session *sdp_local,pjmedia_sdp_session *sdp_rem,int nMediaIndex,int comp_cnt);
	//���Է���û��ICE���ԣ���û�з���false,�з���true
	bool ICECheckCand(pj_pool_t *sdp_pool,pjmedia_sdp_session *sdp_local,pjmedia_sdp_session *sdp_rem,int nMediaIndex,int& nFindRtp,int& nFindRtcp);

protected:
	PJSIP_RTPSessionCallback& m_rRTPCallback;
	unsigned						m_nClockRate;						//clock rate		
	unsigned						m_nSamplesPerFrame;					//samples per frame	
	pjmedia_endpt *					m_pMediaEndpoint;					
	pjmedia_transport *				m_pMediaTransport;					//To send/recv RTP/RTCP
	pj_timestamp					m_nNextRtp;
	pj_timestamp					m_nNextRtcp;
	std::string						m_strRemoteLastIP;
	unsigned short					m_nRemoteLastPort;
	int								m_nUseICE;							//0��ʹ�� 1ʹ��
	bool							m_bTransCreate;
	bool							m_bAnswerMediaCreate;
	bool							m_bOfferMediaCreate;
};

#endif
