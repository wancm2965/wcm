

#include "player.h"


typedef enum {
	VIDEC_CODEC_H261=0,		//unsupport	
	VIDEC_CODEC_H263,		
	VIDEC_CODEC_H263P,		
	VIDEC_CODEC_H263PP,		//same to H263P		
	VIDEC_CODEC_H264,
	VIDEC_CODEC_LSCC,		//unsupport
	VIDEC_CODEC_AH400,
	VIDEC_CODEC_MPEG4,		//unsupport
	VIDEC_CODEC_DH264,
	VIDEC_CODEC_HIKH,
	VIDEC_CODEC_H264_SVC,
	VIDEC_CODEC_HIKC,
	VIDEC_CODEC_MJPG,
	VIDEC_CODEC_IMKH,
	VIDEC_CODEC_DS81,
	VIDEC_CODEC_DS71,
	VIDEC_CODEC_X62MF,
	VIDEC_CODEC_MEGA,
	VIDEC_CODEC_DS80HD,
	VIDEC_CODEC_USER,
	VIDEC_CODEC_COUNT
}VIDEC_CODEC_TYPE;


unsigned long XGetTimestamp(void)
{
#ifdef _WIN32
	return ::timeGetTime();//毫秒
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec/1000; 
#endif
}

void XSleep(unsigned long ulMS)
{
#ifdef _WIN32
	Sleep(ulMS);
#else
	usleep(ulMS*1000);
#endif
}

bool		g_bStartNode = false;

//---------------------------------------------------------------------------------------
// 构造函数
player::player(void)
:m_pIAVReceiver(NULL)
, m_pHPATC_Transcode(NULL)
, m_nVideoWidth(0)
, m_nVideoHeigh(0)
, m_bWaitStop(false)
{
}

//---------------------------------------------------------------------------------------
// 析构函数
player::~player(void)
{
}

unsigned long STR2ULONG(const std::string& value)
{
	return strtoul(value.c_str(),NULL,10);
}


void player::ThreadProcMain(void)
{
	while(!m_bWaitStop)
	{
		if (m_pHPATC_Transcode == NULL)
		{
			m_pHPATC_Transcode = HPATC_Transcode::Create(this);
			if (m_pHPATC_Transcode)
			{
				m_pHPATC_Transcode->Open();
			}
		}

		if (m_pIAVReceiver==NULL)
		{
			m_pIAVReceiver = NETEC_MediaReceiver::Create(*this);
			if (m_pIAVReceiver && 0==m_pIAVReceiver->Open(m_strNoidID.c_str(), "",0, "",0, m_strMCUID.c_str(), m_strMCUIP.c_str(), 4222))
			{
				m_pIAVReceiver->SetAudioID(m_nAudChanID);
				m_pIAVReceiver->SetVideoID(m_nVidChanID);
				m_pIAVReceiver->StartAudio();
				m_pIAVReceiver->StartVideo();
				m_pIAVReceiver->RequestKeyFrame();
			}
		}

		XSleep(1);
	}

	printf("Demo => player::StopReceiver\n");

	NETEC_MediaReceiver* pIAVReceiver = m_pIAVReceiver;
	m_pIAVReceiver = NULL;


	printf("Demo => HPATC_Transcode::Stop\n");
	{
		XAutoLock l(m_csTranscode);
		if (m_pHPATC_Transcode)
		{
			m_pHPATC_Transcode->Close();
			delete m_pHPATC_Transcode;
			m_pHPATC_Transcode = NULL;
		}
	}
	printf("Demo => AVReceive::Stop\n");
	if (pIAVReceiver)
	{
		pIAVReceiver->EnableAudio(0);
		pIAVReceiver->EnableVideo(0);
		printf("Demo => AVReceive::StopAudio\n");
		pIAVReceiver->StopAudio();
		printf("Demo => AVReceive::StopVideo\n");
		pIAVReceiver->StopVideo();
		printf("Demo => AVReceive::Close\n");
#ifdef _WIN32
		pIAVReceiver->Close();
#endif // _WIN32
		printf("Demo => AVReceive::delete pIAVReceiver\n");
		delete pIAVReceiver;
		pIAVReceiver = NULL;
	}

	printf("Demo => player::Stop END\n");

}

//---------------------------------------------------------------------------------------
// 启动服务
bool player::Start(std::string strAV,int nIndex)
{
	m_nIndex = nIndex;
	int nPos = strAV.find(',',0);
	std::string strTmp = strAV.substr(0,nPos);
	std::string strNewAV = strAV.substr(nPos + 1,strAV.length() - nPos - 1);
	m_nAudChanID = STR2ULONG(strTmp);

	nPos = strNewAV.find(',',0);
	strTmp = strNewAV.substr(0,nPos);
	strNewAV = strNewAV.substr(nPos + 1,strAV.length() - nPos - 1);
	m_nVidChanID = STR2ULONG(strTmp);

	nPos = strNewAV.find(',',0);
	strTmp = strNewAV.substr(0,nPos);
	strNewAV = strNewAV.substr(nPos + 1,strAV.length() - nPos - 1);
	m_strNoidID = strTmp;

	nPos = strNewAV.find(',',0);
	strTmp = strNewAV.substr(0,nPos);
	strNewAV = strNewAV.substr(nPos + 1,strAV.length() - nPos - 1);
	m_strMCUID = strTmp;

	nPos = strNewAV.find(',',0);
	strTmp = strNewAV.substr(0,nPos);
	strNewAV = strNewAV.substr(nPos + 1,strAV.length() - nPos - 1);
	m_strMCUIP = strTmp;

	if (m_nAudChanID == 0 || m_nVidChanID == 0 || m_strNoidID.size() == 0 || m_strMCUID.size() == 0 ||  m_strMCUIP.size() == 0)
	{
		printf("Demo => AV parameters ERROR exit!\n");
		return 0;
	}

	if (g_bStartNode == false)
	{
		g_bStartNode = true;

		//远程服务器端口
		NETEC_Node::SetServerIP(m_strMCUIP.c_str());
		NETEC_Node::SetServerPort(4222);
		//启动NODE
		NETEC_Node::Start();

		//unsigned long ulTime = XGetTimestamp();
		//unsigned long ulTime2 = ulTime;
		//while(ulTime2 - ulTime < 2*1000)
		//{
		//	ulTime2 = XGetTimestamp();
		//	XSleep(1);
		//}
	}

	StartThread();
	
	return true;
}

//---------------------------------------------------------------------------------------
// 停止服务
void player::Stop(void)
{
	m_bWaitStop = true;
	WaitForStop();
}

//---------------------------------------------------------------------------------------
void player::OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs)
{
	if (cs == NETEC_Session::CS_CONNECTED || cs == NETEC_Session::CS_RECONNECTED)
	{
		printf("Demo => OnNETEC_NodeConnectStatusChanged::CS_CONNECTED\n");
	}
	else
	{
		printf("Demo => OnNETEC_NodeConnectStatusChanged::%d\n",cs);
	}
}

void player::OnNETEC_MediaReceiverCallbackAudioPacket(unsigned char*pData,int nLen)
{
	XAutoLock l(m_csTranscode);
	if (m_pHPATC_Transcode)
	{
		m_pHPATC_Transcode->TranscodePacketData(pData,nLen);
	}
}

void player::OnNETEC_MediaReceiverCallbackVideoPacket(unsigned char*pData,int nLen)
{
	if (g_mkvHandle)
	{
		int nVideoWidth = VIDEC_HEADER_EXT_GET_VIRTUAL_WIDTH((void *)pData);
		int nVideoHeigh = VIDEC_HEADER_EXT_GET_VIRTUAL_HEIGHT((void *)pData);
		int nKeyFrame = VIDEC_HEADER_EXT_GET_KEYFRAME((void *)pData);
		//if (nKeyFrame )
		//{
		//	if (m_nVideoWidth == 0)
		//	{
		//		//VIDEC_MP4_SetVideo(g_mkvHandle,30,nVideoWidth,nVideoHeigh,VIDEO_H264);
		//	}
		//	else if (g_mkvHandle && ( m_nVideoWidth != nVideoWidth || nVideoHeigh != m_nVideoHeigh))
		//	{
		//		StartRecord();
		//	}
		//	m_nVideoWidth = nVideoWidth;
		//	m_nVideoHeigh = nVideoHeigh;
		//}

			
		unsigned short usTyep = VIDEC_HEADER_EXT_GET_CODEC_ID((void *)pData);
		VIDEO_TYPE nType = VIDEO_H264;
		if (usTyep == VIDEC_CODEC_H263 || usTyep == VIDEC_CODEC_H263P || usTyep == VIDEC_CODEC_H263PP)
		{
			nType = VIDEO_H263;
		}
		int nHeaderLen = VIDEC_HEADER_EXT_GET_LEN((void *)pData);
		unsigned long nTimestamp = VIDEC_HEADER_EXT_GET_TIMESTAMP((void *)pData);
		{
			XAutoLock l(m_csWriteAVData);
			VIDEC_MP4_WriteVideo(g_mkvHandle,pData+nHeaderLen,nLen-nHeaderLen,nTimestamp,nKeyFrame,nVideoWidth,nVideoHeigh,nType,m_nIndex);
		}
	}
}

void player::OnHPATC_TranscodeCallbackPacketData(HPATC_Transcode*pTranscode,void*pPacketData,int nPacketLen,unsigned long ulTimestamp/*原音频编码时间*/)
{
	if (g_mkvHandle)
	{
		XAutoLock l(m_csWriteAVData);
		VIDEC_MP4_WriteAudio(g_mkvHandle,pPacketData,nPacketLen,ulTimestamp,m_nIndex);
	}
}
