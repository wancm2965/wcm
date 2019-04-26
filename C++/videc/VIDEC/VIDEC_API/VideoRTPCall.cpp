#include "VideoRTPCall.h"
#include "XSocket.h"
#include "../VIDEC_Codecs/BaseRTPFrame.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"


VIDEC_RTPCall*VIDEC_RTPCall::Create(void)
{
	//return new VideoRTPCall();
	NEWRECORDret3(  VIDEC_RTPCall* ,p , new VideoRTPCall() ) 
}

VideoRTPCall::VideoRTPCall(void)
{
	m_pLocalWnd=NULL;
	m_pDev=NULL;
	m_pCapChanRTP=NULL;

	m_pPeerWnd=NULL;
	m_pPlayChanRTP=NULL;

	m_pVideoRTPSession=NULL;
}

VideoRTPCall::~VideoRTPCall(void)
{

}


int VideoRTPCall::Open(HWND hLocalWnd,int nDevID,int nLocalWidth,int nLocalHeight,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nCodecType,int nPayloadType,const char*cszLocalIP,unsigned short nLocalPort,HWND hPeerWnd,const char*cszPeerIP,unsigned short nPeerPort)
{
	m_pPeerWnd=VIDEC_Wnd::Create(*this);
	if (m_pPeerWnd==NULL)
	{
		return -1;
	}
	if (0!=m_pPeerWnd->Open(hPeerWnd,true))
	{
		m_pPeerWnd->Close();
		delete m_pPeerWnd;
		m_pPeerWnd=NULL;

		return -1;
	}

	m_pPlayChanRTP=VIDEC_PlayChanRTP::Create();
	if (m_pPlayChanRTP==NULL)
	{
		return -1;
	}
	if (0!=m_pPlayChanRTP->Open(nPayloadType,nCodecType))
	{
		m_pPlayChanRTP->Close();
		delete m_pPlayChanRTP;
		m_pPlayChanRTP=NULL;

		return -1;
	}

	m_pPlayChanRTP->AddImageSink(m_pPeerWnd);

	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
	sessparams.SetUsePollThread(true);
	sessparams.SetOwnTimestampUnit(1.0/90000.0);
	sessparams.SetMaximumPacketSize(1600);
	sessparams.SetAcceptOwnPackets(true);

	unsigned long nIP=XSocket::atoh(cszLocalIP);

	transparams.SetPortbase(nLocalPort);
	transparams.SetBindIP(nIP);

	NEWRECORD( m_pVideoRTPSession , new VideoRTPSession(*this) );
	NULLReturnRet( m_pVideoRTPSession , -1 ) 
	m_pVideoRTPSession->Create(sessparams,&transparams);

	RTPIPv4Address dstAddr;
	nIP=XSocket::atoh(cszPeerIP);
	dstAddr.SetIP(nIP);
	dstAddr.SetPort(nPeerPort);
	m_pVideoRTPSession->AddDestination(dstAddr);


	m_pLocalWnd=VIDEC_Wnd::Create(*this);
	if (m_pLocalWnd==NULL)
	{
		return -1;
	}
	if (0!=m_pLocalWnd->Open(hLocalWnd,true))
	{
		m_pLocalWnd->Close();
		delete m_pLocalWnd;
		m_pLocalWnd=NULL;

		return -1;
	}

	m_pCapChanRTP=VIDEC_CapChanRTP::Create(*this);
	if (m_pCapChanRTP==NULL)
	{
		return -1;
	}
	if (0!=m_pCapChanRTP->Open(nPayloadType,nLocalWidth,nLocalHeight,nFrameRate,nBitrate,nCodecType))
	{
		m_pCapChanRTP->Close();
		delete m_pCapChanRTP;
		m_pCapChanRTP=NULL;

		return -1;
	}

	while (true)
	{
		VIDEC_CameraDev*pDev=VIDEC_CameraDev::Create();
		//VIDEC_AH400Dev*pDev=VIDEC_AH400Dev::Create();
		//VIDEC_AH2KDev*pDev=VIDEC_AH2KDev::Create();
		if (pDev==NULL)
		{
			break;
		}
		if (0!=pDev->Open(nDevID,nLocalWidth,nLocalHeight,nFrameRate))
		{
			pDev->Close();
			delete pDev;
			pDev=NULL;

			break;
		}
		m_pDev=pDev;
		m_pDev->AddImageSink(m_pLocalWnd);
		m_pDev->AddImageSink(m_pCapChanRTP);

		break;
	}

	return 0;
}

void VideoRTPCall::Close(void)
{
	if (m_pDev!=NULL)
	{
		m_pDev->Close();
		delete m_pDev;
		m_pDev=NULL;
	}
	if (m_pCapChanRTP!=NULL)
	{
		m_pCapChanRTP->Close();
		delete m_pCapChanRTP;
		m_pCapChanRTP=NULL;
	}
	if (m_pLocalWnd!=NULL)
	{
		m_pLocalWnd->Close();
		delete m_pLocalWnd;
		m_pLocalWnd=NULL;
	}

	if (m_pVideoRTPSession!=NULL)
	{
		m_pVideoRTPSession->BYEDestroy(RTPTime(0,1),NULL,0);
		delete m_pVideoRTPSession;
		m_pVideoRTPSession=NULL;
	}

	if (m_pPlayChanRTP!=NULL)
	{
		m_pPlayChanRTP->Close();
		delete m_pPlayChanRTP;
		m_pPlayChanRTP=NULL;
	}

	if (m_pPeerWnd!=NULL)
	{
		m_pPeerWnd->Close();
		delete m_pPeerWnd;
		m_pPeerWnd=NULL;
	}
}

void VideoRTPCall::SetLocalWndPos(int x,int y,int cx,int cy)
{
	if (m_pLocalWnd!=NULL)
	{
		m_pLocalWnd->SetWindowPos(NULL,x,y,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}

int VideoRTPCall::GetLocalWidth(void)
{
	if (m_pLocalWnd!=NULL)
	{
		return m_pLocalWnd->GetVideoWidth();
	}
	return 176;
}

int VideoRTPCall::GetLocalHeight(void)
{
	if (m_pLocalWnd!=NULL)
	{
		return m_pLocalWnd->GetVideoHeight();
	}
	return 144;
}

HWND VideoRTPCall::GetLocalWnd(void)
{
	if (m_pLocalWnd!=NULL)
	{
		return m_pLocalWnd->GetWnd();
	}
	return NULL;
}

void VideoRTPCall::SetPeerWndPos(int x,int y,int cx,int cy)
{
	if (m_pPeerWnd!=NULL)
	{
		m_pPeerWnd->SetWindowPos(NULL,x,y,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}

int VideoRTPCall::GetPeerWidth(void)
{
	if (m_pPeerWnd!=NULL)
	{
		return m_pPeerWnd->GetVideoWidth();
	}
	return 176;
}

int VideoRTPCall::GetPeerHeight(void)
{
	if (m_pPeerWnd!=NULL)
	{
		return m_pPeerWnd->GetVideoHeight();
	}
	return 144;
}

HWND VideoRTPCall::GetPeerWnd(void)
{
	if (m_pPeerWnd!=NULL)
	{
		return m_pPeerWnd->GetWnd();
	}
	return NULL;
}

void VideoRTPCall::OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)
{
	if (m_pVideoRTPSession!=NULL)
	{
		PRTP_HEADER pRTPHeader=(PRTP_HEADER)pPacketData;
		unsigned char*pPayloadData=(unsigned char*)pPacketData+RTP_HEADER_LEN;
		int nPayloadLen=nPacketLen-RTP_HEADER_LEN;
		unsigned char nPayloadType=pRTPHeader->type;
		bool bMarker=(pRTPHeader->marker!=0);

		int nStatus=m_pVideoRTPSession->SendPacket(pPayloadData,nPayloadLen,nPayloadType,bMarker,bMarker?3003:0);
	}
}

void VideoRTPCall::OnVideoRTPSessionCallbackRTPPacket(VideoRTPSession*pVideoRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	PRTP_HEADER pRTPHeader=(PRTP_HEADER)pRTPPacketData;
	unsigned short nSeqNo=ntohs(pRTPHeader->sequence);
	bool bMarker=(pRTPHeader->marker!=0);
	static unsigned short nLastSeqNo=0;
	static unsigned short nCount=0;
	if (nLastSeqNo!=(nSeqNo-1))
	{
		nCount=10;
	}
	if (nCount>0)
	{
		nCount--;
		char szDebug[1024];
		sprintf(szDebug,"nCount=%u nSeqNo=%u nLastSeqNo=%u bMarker=%d\n",nCount,nSeqNo,nLastSeqNo,bMarker);
		DebugOutput( logthread ,szDebug);
	}
	nLastSeqNo=nSeqNo;


	if (m_pPlayChanRTP!=NULL)
	{
		m_pPlayChanRTP->OnRecvdRTPPacket(pRTPPacketData,nRTPPacketLen);
	}
}
