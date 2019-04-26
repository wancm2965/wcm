
#include "PJSIP_RTP.h"

extern void sip_register_thread(void);

#define CandAddrBufLen 64

int media_rtp_rand(void)
{
	pj_time_val t;
	unsigned seed=0;
	pj_gettimeofday(&t);
	seed = pj_hash_calc(seed, &t, sizeof(t));
	srand(seed);
	return ((rand() & 0xFFFF) << 16) | (rand() & 0xFFFF);
}

static void on_rx_pjsip_rtp(void *user_data, void *pkt, pj_ssize_t size)
{
    PJSIP_RTP *pPJSIP_RTP=NULL;
    pj_status_t status;
    const pjmedia_rtp_hdr *hdr;
    const void *payload;
    unsigned payload_len;
    pPJSIP_RTP = (PJSIP_RTP *)user_data;
	if(pPJSIP_RTP==NULL) return ;
    if (pPJSIP_RTP->m_bTransportAttach==false) return;
    if (size < 0) return;

	sip_register_thread();

    //Decode RTP packet
    status = pjmedia_rtp_decode_rtp(&pPJSIP_RTP->m_RtpRecvSession,pkt, size, &hdr, &payload, &payload_len);
    if (status != PJ_SUCCESS) return;

	int hdrlen = size-payload_len;
	pPJSIP_RTP->OnRcvdRTPPacket((unsigned char *)pkt,size,hdr->pt,hdrlen,pj_ntohs(hdr->seq),pj_ntohl(hdr->ts));

    //Update the RTCP session.
    pjmedia_rtcp_rx_rtp(&pPJSIP_RTP->m_RtcpSession, pj_ntohs(hdr->seq),pj_ntohl(hdr->ts), payload_len);

    //Update RTP session
    pjmedia_rtp_session_update(&pPJSIP_RTP->m_RtpRecvSession, hdr, NULL);

}

static void on_rx_pjsip_rtcp(void *user_data, void *pkt, pj_ssize_t size)
{
    PJSIP_RTP *pPJSIP_RTP=NULL;
    pPJSIP_RTP =  (PJSIP_RTP *)user_data;
	if(pPJSIP_RTP==NULL) return ;
	if (pPJSIP_RTP->m_bTransportAttach==false) return;
    if (size < 0) return;

	sip_register_thread();
    //Update RTCP session
    pjmedia_rtcp_rx_rtcp(&pPJSIP_RTP->m_RtcpSession, pkt, size);
}

PJSIP_RTP::PJSIP_RTP(PJSIP_RTPSessionCallback& rCallback,pjmedia_endpt * pMediaEndpoint,unsigned nClockRate,int nSamplePerFrame,int nUseICE)
					 : m_rRTPCallback(rCallback),
					 m_nClockRate(nClockRate),
					 m_nSamplesPerFrame(nSamplePerFrame),
					 m_nUseICE(nUseICE)
{
	m_pMediaEndpoint=NULL;
	m_pMediaTransport=NULL;
	m_bTransportAttach=false;
	if(pMediaEndpoint!=NULL) m_pMediaEndpoint=pMediaEndpoint;
	m_bTransCreate=false;
	m_bAnswerMediaCreate=false;
	m_bOfferMediaCreate=false;
}

int PJSIP_RTP::Open(const char*cszLocalIP,unsigned short nLocalPort,unsigned char nPayloadType,const char*cszPayloadName)
{
    pj_uint16_t	rtp_port;
    pj_status_t	status;
	if(m_pMediaEndpoint==NULL) return -1;

	sip_register_thread();
	rtp_port = (pj_uint16_t)(nLocalPort);
	pj_str_t local_addr = pj_str((char *)cszLocalIP);
	status = pjmedia_transport_udp_create2(m_pMediaEndpoint,"RTP%p",&local_addr,rtp_port, 0, &m_pMediaTransport);
	if (status != PJ_SUCCESS) goto on_error;
	m_bTransCreate=true;
	InitRtpFromParam(nPayloadType);
    return PJ_SUCCESS;
on_error:
    Close();
    return status;
}

int PJSIP_RTP::PJSIPICEOpen(pjmedia_transport * pMediaTransport)
{
	if( (m_pMediaEndpoint==NULL) || (pMediaTransport==NULL) ) return -1;
	m_pMediaTransport=pMediaTransport;
	m_bTransCreate=true;
	return PJ_SUCCESS;
}

void PJSIP_RTP::AddParam(unsigned char nPayloadType,unsigned nClockRate,int nSamplePerFrame)
{
	if(m_nUseICE != 0)
	{
		m_nClockRate=nClockRate;
		m_nSamplesPerFrame=nSamplePerFrame;
		sip_register_thread();
		InitRtpFromParam(nPayloadType);
	}
}

void PJSIP_RTP::InitRtpFromParam(unsigned char nPayloadType)
{
	int clock_rate = m_nClockRate;
	int pt=nPayloadType;
	int samples_per_frame =m_nSamplesPerFrame;
	pjmedia_rtp_session_init(&m_RtpSendSession,pt,pj_rand());
	pjmedia_rtp_session_init(&m_RtpRecvSession,pt, 0);
	pjmedia_rtcp_init(&m_RtcpSession, "PJSIPRTCP%p",clock_rate,samples_per_frame, 0);
}

int PJSIP_RTP::PJSIPEncodeInOfferSDP(pj_pool_t *sdp_pool,pjmedia_sdp_session *sdp,int nMediaIndex,int nCmpCount)
{
	int status=-1;
	if(m_bTransCreate==false) return status;
	if(sdp!=NULL)
	{
		sip_register_thread();
		if(m_bOfferMediaCreate==false)
		{
			status=pjmedia_transport_media_create(m_pMediaTransport,sdp_pool,0,NULL,nMediaIndex);
			if(status==PJ_SUCCESS) m_bOfferMediaCreate=true;
		}
		if(status==PJ_SUCCESS)
		{
			status=ICELocalSDPAddrUpdate(sdp_pool,sdp,nMediaIndex,nCmpCount);
		}
		status=pjmedia_transport_encode_sdp(m_pMediaTransport,sdp_pool,sdp,NULL,nMediaIndex);
	}
	return status;
}

int PJSIP_RTP::PJSIPEncodeInAnswerSDP(pj_pool_t *sdp_pool,pjmedia_sdp_session * localSdp,pjmedia_sdp_session * remSdp,int nMediaIndex,int nCmpCount)
{
	int status=-1;
	if(m_bTransCreate==false) return status;
	if( (localSdp!=NULL) && (remSdp!=NULL) )
	{
		sip_register_thread();
		if(m_bAnswerMediaCreate==false)
		{
			status=pjmedia_transport_media_create(m_pMediaTransport,sdp_pool,0,remSdp,nMediaIndex);
			if(status==PJ_SUCCESS) m_bAnswerMediaCreate=true;
		}
		if(status==PJ_SUCCESS)
		{
			status=ICELocalSDPAddrUpdate(sdp_pool,localSdp,nMediaIndex,nCmpCount);
		}
		status=pjmedia_transport_encode_sdp(m_pMediaTransport,sdp_pool,localSdp,remSdp,nMediaIndex);
	}
	return status;
}

int PJSIP_RTP::ICELocalSDPAddrUpdate(pj_pool_t *sdp_pool,pjmedia_sdp_session *sdp_local,int nMediaIndex,int comp_cnt)
{
	pjmedia_sdp_media *m = sdp_local->media[nMediaIndex];
	pj_status_t status=-1;
	pjmedia_sdp_conn *conn=NULL;
	pjmedia_sdp_attr *a_rtcp=NULL;
	pj_sockaddr * pRtpAddr;
	pj_sockaddr * pRtcpAddr;
	pj_bool_t has_rtcp=PJ_FALSE;
	const pj_str_t STR_RTCP = { "rtcp", 4 };
	int comp=0;
	char ConnAddrBuf[CandAddrBufLen]={0};
	bool bRtpAddr=false;
	bool bRtcpAddr=false;
	pj_sockaddr	    rtp_addr_name;
	pj_sockaddr	    rtcp_addr_name;
	if(m_nUseICE==0) return status;
	//此函数取到的默认是Turn分配的地址,不是本机接收地址
	//status=pjmedia_transport_get_info(m_pMediaTransport,&info);
	//pRtpAddr=&info.sock_info.rtp_addr_name;
	//pRtcpAddr=&info.sock_info.rtcp_addr_name;
	conn = m->conn;
	if (conn == NULL) conn = sdp_local->conn;
	strncpy(ConnAddrBuf,conn->addr.ptr,conn->addr.slen);
	//查找候选地址为本端IP的CAND
	for (comp=0; comp < comp_cnt; ++comp)
	{
		unsigned cand_cnt;
		pj_ice_sess_cand cand[PJ_ICE_ST_MAX_CAND];
		unsigned i;
		cand_cnt = PJ_ARRAY_SIZE(cand);
		status = pj_get_ice_enum_cands(m_pMediaTransport, comp+1,&cand_cnt, cand);
		if (status != PJ_SUCCESS) return status;
		char AddrBuf[CandAddrBufLen]={0};
		for (i=0; i<cand_cnt; ++i)
		{
			if(cand[i].type==PJ_ICE_CAND_TYPE_HOST)
			{
				pj_sockaddr_print(&cand[i].addr,AddrBuf,CandAddrBufLen,0);
				if( (comp==0) && (strcmp(AddrBuf,ConnAddrBuf)==0) )
				{
					pj_sockaddr_cp(&rtp_addr_name, &cand[i].addr);
					bRtpAddr=true;
					break;
				}
				if( (comp==1) && (strcmp(AddrBuf,ConnAddrBuf)==0) )
				{
					pj_sockaddr_cp(&rtcp_addr_name, &cand[i].addr);
					bRtcpAddr=true;
					break;
				}
			}
		}
	}

	pRtpAddr=&rtp_addr_name;
	pRtcpAddr=&rtcp_addr_name;
	//覆盖本地接收地址 Override connection line address and media port number
	if(bRtpAddr)
	{
		conn->addr.ptr = (char*) pj_pool_alloc(sdp_pool,PJ_INET6_ADDRSTRLEN);
		pj_sockaddr_print(pRtpAddr, conn->addr.ptr,PJ_INET6_ADDRSTRLEN, 0);
		conn->addr.slen = pj_ansi_strlen(conn->addr.ptr);
		m->desc.port = pj_sockaddr_get_port(pRtpAddr);
	}
	//覆盖本地接收地址 Override address RTCP attribute if it's present
	if(bRtcpAddr)
	{
		a_rtcp = pjmedia_sdp_attr_find(m->attr_count, m->attr,&STR_RTCP, NULL);
		has_rtcp = (a_rtcp != NULL);
		if(comp_cnt==2 && has_rtcp)
		{
			pjmedia_sdp_attr_remove(&m->attr_count, m->attr, a_rtcp);
			a_rtcp = pjmedia_sdp_attr_create_rtcp(sdp_pool,pRtcpAddr);
			pjmedia_sdp_attr_add(&m->attr_count, m->attr, a_rtcp);
		}
	}
	return status;
}

//检查对方有没有ICE属性，如没有则设置ICE远端传输地址
bool PJSIP_RTP::ICECheckCandAddRemoteAddr(pj_pool_t *sdp_pool,pjmedia_sdp_session *sdp_local,pjmedia_sdp_session *sdp_rem,int nMediaIndex,int comp_cnt)
{
	pjmedia_sdp_media * rem_m = sdp_rem->media[nMediaIndex];
	pjmedia_sdp_media * local_m = sdp_local->media[nMediaIndex];
	bool bFindCand=false;
	int status=-1;
	pjmedia_sdp_conn *conn=NULL;
	pjmedia_sdp_attr *a_rtcp=NULL;
	pj_bool_t has_rtcp=PJ_FALSE;
	const pj_str_t STR_RTCP = { "rtcp", 4 };
	int comp_id=1;
	char ConnAddrBuf[CandAddrBufLen]={0};
	pj_sockaddr	    rtp_addr_name;
	pj_sockaddr	    rtcp_addr_name;
	pj_sockaddr	    local_addr;
	pj_str_t connAddr;
	int nFindRtp=0;
	int nFindRtcp=0;

	if(m_nUseICE==0) return bFindCand;
	bFindCand=ICECheckCand(sdp_pool,sdp_local,sdp_rem,nMediaIndex,nFindRtp,nFindRtcp);
	//取本端RTP地址
	conn = local_m->conn;
	if (conn == NULL) conn = sdp_local->conn;
	strncpy(ConnAddrBuf,conn->addr.ptr,conn->addr.slen);
	connAddr=pj_str(ConnAddrBuf);
	pj_sockaddr_init(pj_AF_INET(), &local_addr, &connAddr, local_m->desc.port);
	pj_sockaddr_set_port(&local_addr,local_m->desc.port);

	//对方没有CAND属性
	if(nFindRtp==0)
	{
		conn = rem_m->conn;
		if (conn == NULL) conn = sdp_rem->conn;
		strncpy(ConnAddrBuf,conn->addr.ptr,conn->addr.slen);
		connAddr=pj_str(ConnAddrBuf);
		pj_sockaddr_init(pj_AF_INET(), &rtp_addr_name,  &connAddr, rem_m->desc.port);
		pj_sockaddr_set_port(&rtp_addr_name,rem_m->desc.port);
		unsigned transport_id=1;//tp_type
		pj_ice_cand_type type=PJ_ICE_CAND_TYPE_HOST;//pj_ice_cand_type
		comp_id=1;
		//添加远端RTP地址到ICE Valid Check结构
		status=pj_add_ice_valid_check(m_pMediaTransport,comp_id,transport_id,type,&local_addr,&rtp_addr_name);
	}
	//对方没有CAND属性
	if(nFindRtcp==0)
	{
		a_rtcp = pjmedia_sdp_attr_find(rem_m->attr_count, rem_m->attr,&STR_RTCP, NULL);
		has_rtcp = (a_rtcp != NULL);
		if(comp_cnt==2 && has_rtcp)
		{
			pjmedia_sdp_rtcp_attr rtcp;
			status = pjmedia_sdp_attr_get_rtcp(a_rtcp, &rtcp);
			if(status==PJ_SUCCESS)
			{
				strncpy(ConnAddrBuf,conn->addr.ptr,conn->addr.slen);
				connAddr=pj_str(ConnAddrBuf);
				pj_sockaddr_init(pj_AF_INET(), &rtcp_addr_name,  &connAddr, rem_m->desc.port);
				pj_sockaddr_set_port(&rtcp_addr_name,rtcp.port);
				unsigned transport_id=1;//tp_type
				pj_ice_cand_type type=PJ_ICE_CAND_TYPE_HOST;//pj_ice_cand_type
				comp_id=2;
				//添加远端RTCP地址到ICE Valid Check结构
				status=pj_add_ice_valid_check(m_pMediaTransport,comp_id,transport_id,type,&local_addr,&rtcp_addr_name);
			}
		}
	}
	return bFindCand;
}

//检查对方有没有ICE属性，如没有返回false,有返回true
bool PJSIP_RTP::ICECheckCand(pj_pool_t *sdp_pool,pjmedia_sdp_session *sdp_local,pjmedia_sdp_session *sdp_rem,int nMediaIndex,int& nFindRtp,int& nFindRtcp)
{
	pjmedia_sdp_media * rem_m = sdp_rem->media[nMediaIndex];
	//pjmedia_sdp_media * local_m = sdp_local->media[nMediaIndex];
	bool bFindCand=false;
	int status=-1;
	nFindRtp=0;
	nFindRtcp=0;
	//Find the default addresses in a=candidate attributes
	const pj_str_t STR_CANDIDATE	= { "candidate", 9};
	for (int i=0; i<rem_m->attr_count; ++i)
	{
		pj_ice_sess_cand cand;
		if (pj_strcmp(&rem_m->attr[i]->name, &STR_CANDIDATE)!=0) continue;
		status = pj_parse_cand(m_pMediaTransport, sdp_pool,&rem_m->attr[i]->value, &cand);
		if (status == PJ_SUCCESS) 
			bFindCand=true;
		else 
			continue;
		if (!nFindRtp && cand.comp_id==1) 
		{
			nFindRtp=1;
		}
		else if (!nFindRtcp && cand.comp_id==2 ) 
		{
			nFindRtcp=1;
		}
	}
	return bFindCand;
}


void PJSIP_RTP::Close(void)
{
	sip_register_thread();
	if(m_bTransportAttach==true)
	{
		pjmedia_transport_media_stop(m_pMediaTransport);
		pjmedia_transport_detach(m_pMediaTransport, this);
		m_bTransportAttach=false;
	}
	if (m_pMediaTransport) 
	{
		pjmedia_transport_close(m_pMediaTransport);
		m_pMediaTransport = NULL;
	}
	m_bTransCreate=false;
}

void PJSIP_RTP::AddRemote(const char*cszRemoteIP,unsigned short nRemotePort,
						  pj_pool_t *tmp_pool,pjmedia_sdp_session *sdp_local,pjmedia_sdp_session *sdp_remote,int media_index,int nCmpCount)
{
	pj_sockaddr		rem_addr;   //Remote RTP address		
	pj_sockaddr		rem_rtcp;   //Optional remote RTCP address.
	pj_str_t		remote_addr = pj_str((char *)cszRemoteIP);
	pj_status_t	status;
	if(m_bTransCreate==false) return;

	sip_register_thread();
	//notice ipv4
	int rem_af = pj_AF_INET();
	status = pj_sockaddr_init(rem_af, &rem_addr, &remote_addr,nRemotePort);
	if (status != PJ_SUCCESS)
	{
		return ;
	}
	status = pj_sockaddr_init(rem_af, &rem_rtcp, &remote_addr,nRemotePort+1);
	if (status != PJ_SUCCESS)
	{
		return ;
	}
	//detach media to transport
	if(m_bTransportAttach==true)
	{
		if((m_strRemoteLastIP!=cszRemoteIP) || (m_nRemoteLastPort!=nRemotePort) )
		{
			pjmedia_transport_media_stop(m_pMediaTransport);
			pjmedia_transport_detach(m_pMediaTransport,this);
			m_bTransportAttach=false;
		}
	}
	//Attach media to transport
	if(m_bTransportAttach==false)
	{
		bool bFindCand=ICECheckCandAddRemoteAddr(tmp_pool,sdp_local,sdp_remote,media_index,nCmpCount);
		//对方没有ICE属性并且不启用ICE时或对方有ICE属性时调用
		if( (!bFindCand && m_nUseICE==0 ) || bFindCand )
		{
			status=pjmedia_transport_media_start(m_pMediaTransport,tmp_pool,sdp_local,sdp_remote,media_index);
		}
		status = pjmedia_transport_attach(m_pMediaTransport,this, 
			&rem_addr,&rem_rtcp, sizeof(pj_sockaddr_in),&on_rx_pjsip_rtp,&on_rx_pjsip_rtcp);
		if (status != PJ_SUCCESS) 
		{
			return;
		}
	}
	m_strRemoteLastIP=cszRemoteIP;
	m_nRemoteLastPort=nRemotePort;
	m_bTransportAttach=true;
}

void PJSIP_RTP::SendRTPPacket(const void *data,size_t len,unsigned char pt,bool mark,unsigned int timestampinc,unsigned char*pPacketData,int nPacketLen,unsigned char nRepeatCount)
{
	int clock_rate = m_nClockRate;
	int samples_per_frame =m_nSamplesPerFrame;
	if(m_bTransCreate==false) return;

	sip_register_thread();
	pj_timestamp freq;
	enum { RTCP_INTERVAL = 5000, RTCP_RAND = 2000 };
	unsigned msec_interval;
	msec_interval = samples_per_frame * 1000 / clock_rate;
	pj_get_timestamp_freq(&freq);
	pj_get_timestamp(&m_nNextRtp);

	m_nNextRtp.u64 += (freq.u64 * msec_interval / 1000);
	m_nNextRtcp = m_nNextRtp;
	m_nNextRtcp.u64 += (freq.u64 * (RTCP_INTERVAL+(media_rtp_rand()%RTCP_RAND)) / 1000);
	pj_bool_t send_rtp, send_rtcp;
	send_rtp = PJ_TRUE ;
	send_rtcp = PJ_FALSE ;
	if(send_rtp) 
	{
	    pj_status_t status;
	    const void *p_hdr;
	    int hdrlen;
		//notice: pjsip rtp timestamp diff to pPacketData timestamp we codec can not decode
	    status = pjmedia_rtp_encode_rtp( &m_RtpSendSession,pt,mark,
			len, timestampinc,&p_hdr, &hdrlen);
		if (status == PJ_SUCCESS) 
		{
		}
		int nRepeat=nRepeatCount;
		if(nRepeatCount==0) nRepeat=1;
		for(int cnt=0;cnt<nRepeat;cnt++)
			status = pjmedia_transport_send_rtp(m_pMediaTransport,pPacketData,nPacketLen);
	    //Update RTCP SR
	    pjmedia_rtcp_tx_rtp(&m_RtcpSession,len);
	    //Schedule next send
	    m_nNextRtp.u64 += (msec_interval * freq.u64 / 1000);
		//test
		//PJ_LOG(5,("SendRTPPacket", "SendRTPPacket size %d",nPacketLen));
	}

	if(send_rtcp) 
	{
	    void *rtcp_pkt;
	    int rtcp_len;
	    pj_ssize_t size;
	    pj_status_t status;
	    //Build RTCP packet
	    pjmedia_rtcp_build_rtcp(&m_RtcpSession, &rtcp_pkt, &rtcp_len);
	    //Send RTP packet 
	    size = rtcp_len;
	    status = pjmedia_transport_send_rtcp(m_pMediaTransport,rtcp_pkt, size);
	    //Schedule next send
    	m_nNextRtcp.u64 += (freq.u64 * (RTCP_INTERVAL+(pj_rand()%RTCP_RAND)) / 1000);
	}
}

void PJSIP_RTP::OnRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	m_rRTPCallback.OnPJSIP_RTPSessionCallbackRcvdRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
}
