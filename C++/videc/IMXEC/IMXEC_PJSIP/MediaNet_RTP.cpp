#include "MediaNet_RTP.h"
#include "../IMXEC_Util/RTPHeader.h"
#include <winsock2.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include "../IMXEC_H323/EncryptionCapability.h"

#ifdef HAS_H323
#include "../IMXEC_H323/H323Channel.h"
#include "../IMXEC_H323/H323CallMgr.h"
#endif

//Frame Rcv CallBack
static void OnRecvMediaNetFrame(PFRAMEHDR pFrmHdr, u32 dwContext)
{

}

//RTP PACK CallBack
static void OnRecvMediaNetRtp(TRtpPack *pRtpPack, u32 dwContext)
{
    MediaNet_RTP *pMediaNet_RTP=NULL;
    pMediaNet_RTP = (MediaNet_RTP *)dwContext;
	if(pMediaNet_RTP) pMediaNet_RTP->CopyRTPToData(pRtpPack);
}

void MediaNet_RTP::SetRTPHeaderFromData(TRtpPack & tRtpPack,const void *data,int len)
{
	memset(&tRtpPack,0,sizeof(tRtpPack));
	PRTP_HEADER pRTPHeader = (PRTP_HEADER)data;
	tRtpPack.m_dwSSRC = ntohl(pRTPHeader->ssrc);
	tRtpPack.m_dwTimeStamp = ntohl(pRTPHeader->timestamp);
	tRtpPack.m_wSequence = ntohs(pRTPHeader->sequence);
	tRtpPack.m_byPayload = pRTPHeader->type;
	tRtpPack.m_pRealData = (u8 *)data + RTP_HEADER_LEN;
	tRtpPack.m_nRealSize = len - RTP_HEADER_LEN;
	tRtpPack.m_byMark = pRTPHeader->marker;
}

void MediaNet_RTP::CopyRTPToData(TRtpPack * pRtpPack)
{
    if (pRtpPack == NULL)
    {
        return;
    }

    if(FALSE == m_bNetRcvRunning)
    {
        return;
    }

	if(m_pDataBuf)
	{
        if ( (pRtpPack->m_dwSrcIP != 0) 
            && (pRtpPack->m_wSrcPort != 0)
            && (m_strRemoteIP != "") 
            && (m_nRemoteRTPPort != 0) )
        {
            unsigned char *pSrcIP = (unsigned char*)&(pRtpPack->m_dwSrcIP);
            char szSrcIP[MAX_ADDR_STRING_LEN];
            memset(szSrcIP, 0, sizeof(szSrcIP));

#ifdef _WINDOWS_
            _snprintf(szSrcIP, MAX_ADDR_STRING_LEN, "%u.%u.%u.%u", pSrcIP[0], pSrcIP[1], pSrcIP[2], pSrcIP[3]);
#else
            _snprintf(szSrcIP, MAX_ADDR_STRING_LEN, "%u.%u.%u.%u", pSrcIP[0], pSrcIP[1], pSrcIP[2], pSrcIP[3]);
#endif

            std::string strSrcIP = szSrcIP;
            unsigned short nSrcPort = pRtpPack->m_wSrcPort;

            //if ( (strSrcIP != m_strRemoteIP) || (nSrcPort != m_nRemoteRTPPort) )
			if (strSrcIP != m_strRemoteIP)
            {
                if (m_bAllowChangeRemoteAddr)
                {
                    m_strRemoteIP = strSrcIP;
                    m_nRemoteRTPPort = nSrcPort;
                    m_bNeedChangeRemoteAddr = TRUE;
                }
            }
        }



		PRTP_HEADER pRTPHeader=(PRTP_HEADER)m_pDataBuf;
		memset(pRTPHeader,0,RTP_HEADER_LEN);
		pRTPHeader->version=RTP_VERSION;
		pRTPHeader->ssrc=htonl(pRtpPack->m_dwSSRC);
		pRTPHeader->timestamp=htonl(pRtpPack->m_dwTimeStamp);
		pRTPHeader->sequence=htons(pRtpPack->m_wSequence);
		pRTPHeader->type=pRtpPack->m_byPayload;
		pRTPHeader->marker=pRtpPack->m_byMark;
		memcpy(m_pDataBuf+RTP_HEADER_LEN,pRtpPack->m_pRealData,pRtpPack->m_nRealSize);
		int nRet = pRtpPack->m_nRealSize + RTP_HEADER_LEN;
		if(m_bRecv) OnRcvdRTPPacket(m_pDataBuf,nRet,pRtpPack->m_byPayload,RTP_HEADER_LEN,pRtpPack->m_wSequence,pRtpPack->m_dwTimeStamp);
	}
}

MediaNet_RTP::MediaNet_RTP(MediaNet_RTPSessionCallback& rCallback,void * pMediaEndpoint,unsigned nClockRate,int nSamplePerFrame,int nUseICE)
					 : m_rRTPCallback(rCallback),
					 m_nClockRate(nClockRate),
					 m_nSamplesPerFrame(nSamplePerFrame)
{
	m_pMediaSnd =NULL;
	m_pMediaRcv =NULL;
	m_nMaxFrameSize =0;
	m_strLocalIP ="";
	m_strRemoteIP ="";
	m_nLocalRTPPort=0;
	m_nLocalRTCPPort=0;
	m_nRemoteRTPPort=0;
	m_nRemoteRTCPPort=0;
	m_pDataBuf =NULL;
	m_nMaxRtpBufLen = 10*1024;//notice
	m_pDataBuf=(unsigned char*)malloc(m_nMaxRtpBufLen);

    m_bAllowChangeRemoteAddr = TRUE;
    m_bNeedChangeRemoteAddr  = FALSE;

    m_bNetSndRunning = FALSE;
    m_bNetRcvRunning = FALSE;

    m_nFlagReliable = 0;
	m_bRecv =false;
	m_bSend =false;
	m_pH323ChanCallBack =false;
	m_nProtocolType =1;
	m_nRecvThreadID =0;
}

MediaNet_RTP::~MediaNet_RTP(void)
{
	Close();
	if(m_pDataBuf)
	{
		free(m_pDataBuf);
		m_pDataBuf=NULL;
	}
}

//nDir 1 recv , 2 send , 3 sendrecv
int MediaNet_RTP::Open(const char*cszLocalIP,unsigned short nLocalPort,unsigned char nPayloadType,const char*cszPayloadName,int nDir,char * szKeyBuf,int nAlgoId)
{
	int nRet=-1;
	m_strLocalIP =cszLocalIP;
	m_nLocalRTPPort=nLocalPort;
	m_nLocalRTCPPort=nLocalPort+1;

	if(nDir ==1 || nDir ==3)
	{
		m_nRecvPayloadType = nPayloadType;
		if(m_pMediaRcv)
		{
			m_bNetRcvRunning = FALSE;
			m_pMediaRcv->StopRcv();
			delete m_pMediaRcv;
			m_pMediaRcv =NULL;
		}
		if(m_pMediaRcv ==NULL)
		{
			m_pMediaRcv = new CHpMediaRcv();
			if(m_pMediaRcv)
			{
				u16 wRet = m_pMediaRcv->Create(m_nMaxFrameSize,(PRTPCALLBACK)OnRecvMediaNetRtp,(u32)this);
				if (wRet !=0)
				{
					delete m_pMediaRcv;
					m_pMediaRcv =NULL;
				}
				else
				{
					if(szKeyBuf !=NULL )
					{
						int nKeySize =0;int nMode=0;
						GetKeyParam(nDir,nAlgoId,nKeySize,nMode);
						m_pMediaRcv->SetDecryptKey(szKeyBuf,nKeySize,nMode);
					}
					nRet =0;
				}
			}

		}
	}

	if(nDir==2 || nDir ==3)
	{
		if(m_pMediaSnd)
		{
			m_bNetSndRunning = FALSE;
			delete m_pMediaSnd;
			m_pMediaSnd =NULL;
		}
		if(m_pMediaSnd ==NULL)
		{
			nRet=-1;
			m_pMediaSnd = new CHpMediaSnd();
			if(m_pMediaSnd)
			{
				u16 wRet = m_pMediaSnd->Create(m_nMaxFrameSize,m_nNetBand,m_nFrameRate,m_nSendMediaType);
				if (wRet !=0)
				{
					delete m_pMediaSnd;
					m_pMediaSnd =NULL;
				}
				else
				{
					if(szKeyBuf !=NULL )
					{
						int nKeySize =0;int nMode=0;
						GetKeyParam(nDir,nAlgoId,nKeySize,nMode);
						m_pMediaSnd->SetEncryptKey(szKeyBuf,nKeySize,nMode);
					}
					nRet =0;
				}
			}
		}
	}

	return nRet;
}

void MediaNet_RTP::AddParam(unsigned char nPayloadType,unsigned nClockRate,int nSamplePerFrame)
{
	m_nClockRate=nClockRate;
	m_nSamplesPerFrame=nSamplePerFrame;
    m_nRecvPayloadType = nPayloadType;

}

void MediaNet_RTP::InitRtpFromParam(unsigned char nPayloadType)
{

}

void MediaNet_RTP::Close(void)
{
	m_bNetSndRunning = FALSE;
	m_bNetRcvRunning = FALSE;

	if(m_pMediaRcv)
	{
		m_pMediaRcv->StopRcv();
		delete m_pMediaRcv;
		m_pMediaRcv =NULL;
	}
	if(m_pMediaSnd)
	{
		delete m_pMediaSnd;
		m_pMediaSnd =NULL;
	}
}

BOOL32 MediaNet_RTP::SetRecvParam()
{
    if(NULL == m_pMediaRcv)
    {
        return FALSE;
    }

    TLocalNetParam tLocalNetPara;
    tLocalNetPara.m_tLocalNet.m_dwRTPAddr  = (inet_addr(m_strLocalIP.c_str()));
    tLocalNetPara.m_tLocalNet.m_wRTPPort   = m_nLocalRTPPort;
    tLocalNetPara.m_tLocalNet.m_dwRTCPAddr = tLocalNetPara.m_tLocalNet.m_dwRTPAddr;
    tLocalNetPara.m_tLocalNet.m_wRTCPPort  = m_nLocalRTCPPort;
    tLocalNetPara.m_dwRtcpBackAddr         = (inet_addr(m_strRemoteIP.c_str()));
    tLocalNetPara.m_wRtcpBackPort          = m_nRemoteRTCPPort;


    m_pMediaRcv->RemoveNetRcvLocalParam();

    u16 wRet = m_pMediaRcv->SetNetRcvLocalParam(tLocalNetPara);
    if (0 != wRet)
    {
        return FALSE;
    }

    TRSParam tRsParam;
    tRsParam.m_wFirstTimeSpan  = 40;
    tRsParam.m_wSecondTimeSpan = 160;
    tRsParam.m_wThirdTimeSpan  = 480;
    tRsParam.m_wRejectTimeSpan = 2000;//960;
    wRet = m_pMediaRcv->ResetRSFlag(tRsParam, TRUE, FALSE);
    if (0 != wRet)
    {
        return FALSE;
    }

    if ( (96 <= m_nRecvPayloadType) && (m_nRecvPayloadType < 255) )
    {
        wRet = m_pMediaRcv->SetActivePT(m_nRecvPayloadType,m_nRecvMediaType);
        if (0 != wRet)
        {
            return FALSE;
        }
    }

    wRet = m_pMediaRcv->StartRcv();
    if (0 != wRet)
    {
        return FALSE;
    }
    if (m_nFlagReliable)
    {
        m_pMediaRcv->RsFecRcvStart();
    }

    return TRUE;

}

BOOL32 MediaNet_RTP::SetSendParam()
{
    if(NULL == m_pMediaSnd)
    {
        return FALSE;
    }

    TNetSndParam tNetSndPara;
    tNetSndPara.m_byNum=1;//notice
    int i=tNetSndPara.m_byNum -1;

    tNetSndPara.m_tLocalNet.m_dwRTPAddr    = (inet_addr(m_strLocalIP.c_str()));
    tNetSndPara.m_tLocalNet.m_wRTPPort     = m_nLocalRTPPort;
    tNetSndPara.m_tLocalNet.m_dwRTCPAddr   = tNetSndPara.m_tLocalNet.m_dwRTPAddr;
    tNetSndPara.m_tLocalNet.m_wRTCPPort    = m_nLocalRTCPPort;

    tNetSndPara.m_tRemoteNet[i].m_dwRTPAddr   = (inet_addr(m_strRemoteIP.c_str()));
    tNetSndPara.m_tRemoteNet[i].m_wRTPPort    = m_nRemoteRTPPort;
    tNetSndPara.m_tRemoteNet[i].m_dwRTCPAddr  = tNetSndPara.m_tRemoteNet[i].m_dwRTPAddr;
    tNetSndPara.m_tRemoteNet[i].m_wRTCPPort   = m_nRemoteRTCPPort;

	m_pMediaSnd->RemoveNetSndLocalParam();

    u16 wRet = m_pMediaSnd->SetNetSndParam(tNetSndPara);
    if (0 != wRet)
    {
        return FALSE;
    }

    wRet = m_pMediaSnd->ResetRSFlag(1500, TRUE);
    if (0 != wRet)
    {
        return FALSE;
    }

    if (m_nFlagReliable)
    {
        m_pMediaSnd->RsFecSndStart();
    }

    return TRUE;
}

void MediaNet_RTP::AddRemote(const char*cszRemoteIP,unsigned short nRemotePort,
						  void *tmp_pool,void *sdp_local,void *sdp_remote,int media_index,int nCmpCount, int nFlagReliable,unsigned short nRemoteRTCPPort,int nDir)
{
	m_strRemoteIP =cszRemoteIP;
	m_nRemoteRTPPort =nRemotePort;
	if(nRemoteRTCPPort ==0) m_nRemoteRTCPPort =nRemotePort+1;
	else m_nRemoteRTCPPort =nRemoteRTCPPort;

    m_nFlagReliable = nFlagReliable;

	StartRTP(nDir);
}

void MediaNet_RTP::SendRTPPacket(const void *data,size_t len,unsigned char pt,bool mark,unsigned int timestampinc,unsigned char*pPacketData,int nPacketLen,unsigned char nRepeatCount)
{
	if(!m_bSend) return;
    if (m_bNeedChangeRemoteAddr)
    {
        AddRemote(m_strRemoteIP.c_str(), m_nRemoteRTPPort, NULL, NULL, NULL, 0, 0, m_nFlagReliable);
        m_bNeedChangeRemoteAddr = FALSE;
    }

    if(FALSE == m_bNetSndRunning)
    {
        return;
    }

	if(m_pMediaSnd)
	{
		TRtpPack tRtpPack;
		SetRTPHeaderFromData(tRtpPack,pPacketData,nPacketLen);
		
        u16 wRet = m_pMediaSnd->Send(&tRtpPack, FALSE);
        if (0 != wRet)
        {
            return;
        }
	}

}

void MediaNet_RTP::OnRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
#ifdef HAS_H323

	if(m_nProtocolType ==2) ((H323Channel *)m_pH323ChanCallBack)->OnMediaNet_RTPSessionCallbackRcvdRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	else if(m_nProtocolType ==1)
	{
		m_rRTPCallback.OnMediaNet_RTPSessionCallbackRcvdRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}

#else

	if(m_nProtocolType ==1)
	{
		m_rRTPCallback.OnMediaNet_RTPSessionCallbackRcvdRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
#endif
}

void MediaNet_RTP::InitRtpParam2(int nMaxFrameSize, int nNetBand, int nFrameRate,int nMedia,int nCodecType,int nSSRC ,int nDir)
{
	//1 audio 2 video 3 H239 4 Data
	int nMediaType=0;
	if(nMedia ==1)
	{
		X_AUDIO_CODEC_ID audioType=(X_AUDIO_CODEC_ID)nCodecType;
		if(nCodecType >= 0)
		{
			switch(audioType)
			{
			case X_AUDIO_CODEC_G711U:
				nMediaType =MEDIA_TYPE_PCMU;
				break;
			case X_AUDIO_CODEC_G711A:
				nMediaType =MEDIA_TYPE_PCMA;
				break;
			case X_AUDIO_CODEC_G719_32:
			case X_AUDIO_CODEC_G719_48:
			case X_AUDIO_CODEC_G719_64:
				nMediaType =MEDIA_TYPE_G719;
				break;
			case X_AUDIO_CODEC_G722_48:
			case X_AUDIO_CODEC_G722_56:
			case X_AUDIO_CODEC_G722_64:
				nMediaType =MEDIA_TYPE_G722;
				break;
			case X_AUDIO_CODEC_G729:
				nMediaType =MEDIA_TYPE_G729;
				break;
			case X_AUDIO_CODEC_G7231_53:
			case X_AUDIO_CODEC_G7231_63:
				nMediaType =MEDIA_TYPE_G7231;
				break;
			default:
				nMediaType=0;
				break;
			}
		}
	}
	else if(nMedia ==2 || nMedia==3)
	{
		VIDEC_CODEC_TYPE videoType=(VIDEC_CODEC_TYPE)nCodecType;
		if(nCodecType >= 0)
		{
			switch(videoType)
			{
			case VIDEC_CODEC_H264:
			case VIDEC_CODEC_H264_SVC:
				nMediaType =MEDIA_TYPE_H264;
				break;
			case VIDEC_CODEC_H263:
				nMediaType =MEDIA_TYPE_H263;
				break;
			case VIDEC_CODEC_H263P:
			case VIDEC_CODEC_H263PP:
				nMediaType =MEDIA_TYPE_H263PLUS;
				break;
			default:
				nMediaType=0;
				break;
			}
		}
	}
	else if(nMedia ==4)
	{
		if(nCodecType >= 0) nMediaType = MEDIA_TYPE_H224;
	}
	else nMediaType = 0;

	m_nMaxFrameSize =nMaxFrameSize;
	m_nNetBand = nNetBand;
	m_nFrameRate =nFrameRate;

	if(nDir == 1 || nDir == 3) m_nRecvMediaType =nMediaType;
	if(nDir == 2 || nDir == 3) m_nSendMediaType =nMediaType;
}

void MediaNet_RTP::GetKeyParam(int nDir,int algorithmId,int & keySize,int & nMode)
{
	EncDecAlgorithmID algId=(EncDecAlgorithmID)algorithmId;
	switch(algId)
	{
	case ALGORITHM_AES_CBC:
		keySize =16;
		nMode = AES_ENCRYPT_MODE;
		break;
	//case ALGORITHM_AES192_CBC:
	//	keySize =24;
	//	nMode = AES_ENCRYPT_MODE;
	//	break;
	case ALGORITHM_AES256_CBC:
		keySize =32;
		nMode = AES_ENCRYPT_MODE;
		break;
	case ALGORITHM_DES_CBC:
		keySize =8;
		nMode = DES_ENCRYPT_MODE;
		break;
	case ALGORITHM_TRIPLE_DES_CBC:
		keySize =24;
		nMode = DES_ENCRYPT_MODE;
		break;
	case ALGORITHM_AES_ECB:
	case ALGORITHM_DES_ECB:
	case ALGORITHM_TRIPLE_DES_ECB:
		break;
	default:
		break;
	}
}

void MediaNet_RTP::SendRecvPause(int nDir) //nDir 1 recv , 2 send , 3 sendrecv
{
	if(nDir==3)
	{
		m_bRecv =false;
		m_bSend =false;
	}
	else if(nDir==1)
	{
		m_bRecv =false;
	}
	else if(nDir==2)
	{
		m_bSend =false;
	}

	if(m_bNetRcvRunning && (nDir ==1 || nDir==3) )
	{
		m_bNetRcvRunning =false;
		m_pMediaRcv->StopRcv();
	}
}

void MediaNet_RTP::SendRecvStart(int nDir,void * pCallBack) //nDir 1 recv , 2 send , 3 sendrecv
{
	if(pCallBack ) m_nProtocolType =2; //h323 use
	if(nDir ==1 || nDir==3) StartRTP(nDir);

	if(nDir==3)
	{
		if(m_bNetRcvRunning) { m_pH323ChanCallBack =pCallBack ; m_bRecv =true; }
		if(m_bNetSndRunning) m_bSend =true;
	}
	else if(nDir==1)
	{
		if(m_bNetRcvRunning) { m_pH323ChanCallBack =pCallBack ; m_bRecv =true; }
	}
	else if(nDir==2)
	{
		if(m_bNetSndRunning) m_bSend =true;
	}
}

void MediaNet_RTP::StartRTP(int nDir)
{
	char msg[256]={0};
	BOOL32 bRet =FALSE;

	if(nDir==1 || nDir==3)
	{
		bRet = SetRecvParam();
		if (FALSE == bRet)
		{
			sprintf(msg,"recv Fail to set param of .local rtp:%d,remote rtp:%d,remote rtcp:%d,localIP:%s,remoteIP:%s\n",m_nLocalRTPPort,m_nRemoteRTPPort,m_nRemoteRTCPPort,m_strLocalIP.c_str(),m_strRemoteIP.c_str());
			OutputDebugString(msg);
			m_bNetRcvRunning = FALSE;
		}
		else
		{
			sprintf(msg,"recv Success to set param of .local rtp:%d,remote rtp:%d,remote rtcp:%d,localIP:%s,remoteIP:%s\n",m_nLocalRTPPort,m_nRemoteRTPPort,m_nRemoteRTCPPort,m_strLocalIP.c_str(),m_strRemoteIP.c_str());
			//OutputDebugString(msg);
			m_bNetRcvRunning = TRUE;
		}
	}

	if(nDir==2 || nDir==3)
	{
		bRet = SetSendParam();  
		if (FALSE == bRet)
		{
			sprintf(msg,"send Fail to set param of .local rtp:%d,remote rtp:%d,remote rtcp:%d,localIP:%s,remoteIP:%s\n",m_nLocalRTPPort,m_nRemoteRTPPort,m_nRemoteRTCPPort,m_strLocalIP.c_str(),m_strRemoteIP.c_str());
			OutputDebugString(msg);
			m_bNetSndRunning = FALSE;
		}
		else
		{
			sprintf(msg,"send Success to set param of .local rtp:%d,remote rtp:%d,remote rtcp:%d,,localIP:%s,remoteIP:%s\n",m_nLocalRTPPort,m_nRemoteRTPPort,m_nRemoteRTCPPort,m_strLocalIP.c_str(),m_strRemoteIP.c_str());
			//OutputDebugString(msg);
			m_bNetSndRunning = TRUE;
		}
	}

}

void MediaNet_RTP::GetRecvStatistics(THpRcvStatistics &tHpRcvStatistics)
{
	if(m_bRecv && m_pMediaRcv)
	{
		m_pMediaRcv->GetStatistics(tHpRcvStatistics );
	}
}

void MediaNet_RTP::GetSendStatistics(THpSndStatistics &tHpSndStatistics)
{
	if(m_bSend && m_pMediaSnd)
	{
		m_pMediaSnd->GetStatistics(tHpSndStatistics );
	}
}
