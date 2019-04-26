//PlayChan.cpp
#include "../IMXEC_win32/stdafx.h"

#include "PlayChan.h"
#include "PlayChanMgr.h"
#include <VIDEC/VIDEC_Header.h>
#include <AUDEC/AUDEC_Header.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <NETEC/NETEC_Setting.h>
#include "AudioCapChanMgr.h"
#include "CapChanDevMgr.h"
#include "XUtil.h"

//Add WebRTC by zys 20160929
#include "RTPHeader.h"

#pragma comment(lib,"Strmiids.lib") 
#pragma comment(lib,"Dmoguids.lib")  
#pragma comment(lib,"wmcodecdspuuid.lib") 
#pragma comment(lib,"Msdmo.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMXEC_PlayChan*IMXEC_PlayChan::Create(IMXEC_PlayChanCallback&rCallback)
{
	return new PlayChan(rCallback);
}

unsigned long IMXEC_PlayChan::GetTotalBytes(void)
{
	return PlayChanMgr::Instance().GetTotalKBytes();
}

int IMXEC_PlayChan::GetBitrate(void)
{
	return PlayChanMgr::Instance().GetBitrate();
}

void IMXEC_PlayChan::ClearTotalBytes(void)
{
	PlayChanMgr::Instance().ClearTotalBytes();
}

void IMXEC_PlayChan::SetForcibleReceiving(bool bForcible)
{
	PlayChanMgr::Instance().SetForcibleReceiving(bForcible);
}

bool IMXEC_PlayChan::GetForcibleReceiving(void)
{
	return PlayChanMgr::Instance().GetForcibleReceiving();
}

PlayChan::PlayChan(IMXEC_PlayChanCallback&rCallback)
:m_rCallback(rCallback)
,m_pMediaReceiver(NULL)
,m_pAudPlayChan(NULL)
,m_nAudioSynTime(0)
,m_nVideoSynTime(0)
,m_nSynFlag(1)
,m_nAudioChanID(0)
,m_pWebRTCAudio(NULL)
,m_nWebRTCPlayChan(-1)

,m_hwndParent(NULL)

,m_pAVIWriter(NULL)
#ifdef __VIDEC_PLAYER_H__
,m_pVideoPlayer(NULL)
#else
,m_pWnd(NULL)
,m_pVidPlayChan(NULL)
#endif

,m_bHardwareError(false)
,m_nVirtualWidth(0)
,m_nVirtualHeight(0)
{
	m_nAudioVolume=10;

	m_nVideoID=0;

	m_nAudioChanID=0;
	m_strPeerNodeID="";
	m_strPeerNATIP="";
	m_strPeerLocalIP="";
	m_nPeerLocalPort=0;
	m_nPeerNATPort=0;
	m_strPeerMCUID="";
	m_strPeerMCUIP="";
	m_nPeerMCUPort=0;
	m_nForceUserMCU=0;
	
	m_nAudioStarted=0;
	m_nAudioEnable=1;
	m_bMute = false;

	m_nTotalBytes=0;
	m_nEnableVideo=1;
	m_nVideoStarted=0;

	m_nFrameTypeControlMode=FRAME_TYPE_CONTROL_MODE_MAIN;
	m_nFrameTypeControlModeCurrent=FRAME_TYPE_CONTROL_MODE_MAIN;

	m_nScreenIndex=-1;
	m_bAudioCast=false;
	m_nMixingAudioCodecID=X_AUDIO_CODEC_AUDEC_24;

	m_nKeyID=(unsigned long)this;
	m_nKeyID|=0xFF000000;

	m_nLastRequestKeyFrameTS=0;

	//Add WebRTC by zys 20160929
	m_nRTPTimestamp = 0;
	m_nRTPSeqNo = 0;

	AudioPlayChanMgr::Instance();
#if USE_WEBRTC
	m_nAudioMixInChannel = AudioPlayChanMgr::Instance().m_pAudioMixer->CreateInputChannel();

	m_nAudioMixChanInChannel = AudioPlayChanMgr::Instance().m_pAudioMixerChan->CreateInputChannel();
#endif

	m_nVideoWnBkR = -1;
	m_nVideoWnBkG = -1;
	m_nVideoWnBkB = -1;
}

PlayChan::~PlayChan(void)
{
#if USE_WEBRTC
	AudioPlayChanMgr::Instance().m_pAudioMixer->RemoveInputChannel(m_nAudioMixInChannel);

	AudioPlayChanMgr::Instance().m_pAudioMixerChan->RemoveInputChannel(m_nAudioMixChanInChannel);
#endif
}

int PlayChan::Open(HWND hwndParent,unsigned long nAudChanID,unsigned long nVidChanID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU,unsigned long nEnableMulticast)
//int PlayChan::Open(HWND hwndParent,unsigned long nAudChanID,unsigned long nVidChanID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU)
{
	if (m_pMediaReceiver)
	{
		std::string		strPeerNodeID="";
		std::string		strPeerNATIP="";
		std::string		strPeerLocalIP="";
		std::string		strPeerMCUID="";
		std::string		strPeerMCUIP="";

		if (cszPeerNodeID!=NULL)
			strPeerNodeID=cszPeerNodeID;
		if (cszPeerNATIP!=NULL)
			strPeerNATIP=cszPeerNATIP;
		if (cszPeerLocalIP!=NULL)
			strPeerLocalIP=cszPeerLocalIP;
		if (cszPeerMCUID!=NULL)
			strPeerMCUID=cszPeerMCUID;
		if (cszPeerMCUIP!=NULL)
			strPeerMCUIP=cszPeerMCUIP;

		if (strPeerNodeID!=m_strPeerNodeID ||
			strPeerNATIP!=m_strPeerNATIP ||
			strPeerLocalIP!=m_strPeerLocalIP ||
			strPeerMCUID!=m_strPeerMCUID ||
			strPeerMCUIP!=m_strPeerMCUIP ||
			nPeerNATPort!=m_nPeerNATPort ||
			nPeerLocalPort!=m_nPeerLocalPort ||
			nPeerMCUPort!=m_nPeerMCUPort ||
			nForceUseMCU!=m_nForceUserMCU ||
			m_nVideoID!=nVidChanID)
		{
			m_pMediaReceiver->Close();
			delete m_pMediaReceiver;
			m_pMediaReceiver=NULL;
		}
	}

	if (nAudChanID!=m_nAudioChanID)
	{
		if (m_nAudioChanID!=0)
		{
			SetAudioID(nAudChanID);
		}
	}

#ifdef __VIDEC_PLAYER_H__
	if (m_hwndParent!=hwndParent)
	{
		m_hwndParent=hwndParent;
		if (m_pVideoPlayer!=NULL)
		{
			m_pVideoPlayer->Close();
			delete m_pVideoPlayer;
			m_pVideoPlayer=NULL;
		}
	}

	if (m_pVideoPlayer==NULL && m_hwndParent!=NULL)
	{
		m_pVideoPlayer=VIDEC_Player::Create(*this);
		if (m_pVideoPlayer==NULL)
		{
			return -1;
		}

		if (0!=m_pVideoPlayer->Open(m_hwndParent,&m_nVideoSynTime,&m_nAudioSynTime, (char*)(m_strVideoWndBkImage[0]?m_strVideoWndBkImage.c_str():NULL), m_nVideoWnBkR, m_nVideoWnBkG, m_nVideoWnBkB))
		{
			m_pVideoPlayer->Close();
			delete m_pVideoPlayer;
			m_pVideoPlayer=NULL;

			return -1;
		}
		m_pVideoPlayer->SetSynFlag(m_nSynFlag);
	}
#else
	if (m_hwndParent!=hwndParent)
	{
		m_hwndParent=hwndParent;
		if (m_pWnd!=NULL)
		{
			m_pWnd->Close();
			delete m_pWnd;
			m_pWnd=NULL;
		}
	}

	if (m_pWnd==NULL && m_hwndParent!=NULL)
	{
		m_pWnd=VIDEC_Wnd::Create();
		if (m_pWnd==NULL)
		{
			return -1;
		}

		if (0!=m_pWnd->Open(m_hwndParent,false,&m_nVideoSynTime,&m_nAudioSynTime))
		{
			m_pWnd->Close();
			delete m_pWnd;
			m_pWnd=NULL;

			return -1;
		}
		m_pWnd->SetSynFlag(m_nSynFlag);
	}

	if (m_pVidPlayChan==NULL)
	{
		m_pVidPlayChan=VIDEC_PlayChan::Create();
		if (m_pVidPlayChan==NULL)
		{
			return -1;
		}

		if (0!=m_pVidPlayChan->Open())
		{
			m_pVidPlayChan->Close();
			delete m_pVidPlayChan;
			m_pVidPlayChan=NULL;

			return -1;
		}

		if (m_pWnd!=NULL)
		{
			m_pVidPlayChan->AddImageSink(m_pWnd);
		}
	}
#endif


	if (m_pMediaReceiver==NULL)
	{
		m_pMediaReceiver=NETEC_MediaReceiver::Create(*this);
		if (m_pMediaReceiver==NULL)
		{
			return -1;
		}

		if (nForceUseMCU!=0)
		{
			if (0!=m_pMediaReceiver->Open(cszPeerNodeID,"",          0,         cszPeerLocalIP,nPeerLocalPort,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort))
			{
				m_pMediaReceiver->Close();
				delete m_pMediaReceiver;
				m_pMediaReceiver=NULL;

				return -1;
			}
		}
		else
		{
			if (0!=m_pMediaReceiver->Open(cszPeerNodeID,cszPeerNATIP,nPeerNATPort,cszPeerLocalIP,nPeerLocalPort,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort))
			{
				m_pMediaReceiver->Close();
				delete m_pMediaReceiver;
				m_pMediaReceiver=NULL;

				return -1;
			}
		}

		m_pMediaReceiver->SetVideoID(nVidChanID);
		m_nVideoID=nVidChanID;
	}
	//A7 add
	if (nEnableMulticast!=0)
	{
		NETEC_Setting::SetEnableMulticast(1);
		m_pMediaReceiver->SetVideoCast(true);
		m_pMediaReceiver->SetAudioCast(true);
	}

	m_nAudioChanID=nAudChanID;

	if (cszPeerNodeID!=NULL)
		m_strPeerNodeID=cszPeerNodeID;
	if (cszPeerNATIP!=NULL)
		m_strPeerNATIP=cszPeerNATIP;
	if (cszPeerLocalIP!=NULL)
		m_strPeerLocalIP=cszPeerLocalIP;
	m_nPeerLocalPort=nPeerLocalPort;
	m_nPeerNATPort=nPeerNATPort;
	if (cszPeerMCUID!=NULL)
		m_strPeerMCUID=cszPeerMCUID;
	if (cszPeerMCUIP!=NULL)
		m_strPeerMCUIP=cszPeerMCUIP;
	m_nPeerMCUPort=nPeerMCUPort;
	m_nForceUserMCU=nForceUseMCU;

	PlayChanMgr::Instance().AddPlayChan(this);

	return 0;
}

/******************************************************************************
* Open
*描述：打开本地播放
*输入：	hwndParent		-视频父窗口句柄
*输出： 无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int PlayChan::Open(HWND hwndParent)
{
#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer!=NULL)
	{
		m_pVideoPlayer->Close();
		delete m_pVideoPlayer;
		m_pVideoPlayer=NULL;
	}

	m_pVideoPlayer=VIDEC_Player::Create(*this);
	if (m_pVideoPlayer==NULL)
	{
		return -1;
	}

	if (0!=m_pVideoPlayer->Open(hwndParent,&m_nVideoSynTime,&m_nAudioSynTime, (char*)(m_strVideoWndBkImage[0]?m_strVideoWndBkImage.c_str():NULL), m_nVideoWnBkR, m_nVideoWnBkG, m_nVideoWnBkB))
	{
		m_pVideoPlayer->Close();
		delete m_pVideoPlayer;
		m_pVideoPlayer=NULL;

		return -1;
	}
	m_pVideoPlayer->SetSynFlag(m_nSynFlag);
#else
	m_pWnd=VIDEC_Wnd::Create();
	if (m_pWnd==NULL)
	{
		return -1;
	}

	if (0!=m_pWnd->Open(hwndParent,false,&m_nVideoSynTime,&m_nAudioSynTime))
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;

		return -1;
	}

	if (m_pVidPlayChan==NULL)
	{
		m_pVidPlayChan=VIDEC_PlayChan::Create();
		if (m_pVidPlayChan==NULL)
		{
			return -1;
		}

		if (0!=m_pVidPlayChan->Open())
		{
			m_pVidPlayChan->Close();
			delete m_pVidPlayChan;
			m_pVidPlayChan=NULL;

			return -1;
		}

		m_pVidPlayChan->AddImageSink(m_pWnd);
	}
#endif

	//Add WebRTC by zys 20160929
#if USE_WEBRTC
	//if (NULL == m_pWebRTCAudio)
	{
		m_pWebRTCAudio = MyVoiceEngineInterface::GetVoiceEngine();
		m_pWebRTCAudio->Init();
	}

	if ( -1 == m_nWebRTCPlayChan)
	{
		m_nWebRTCPlayChan = m_pWebRTCAudio->CreateDecodeChannel();
		m_pWebRTCAudio->StartPlayout(m_nWebRTCPlayChan);
		AudioPlayChanMgr::Instance().m_pAudioMixer->StartInputMixing(m_nAudioMixInChannel);

		AudioPlayChanMgr::Instance().m_pAudioMixerChan->StartInputMixing(m_nAudioMixChanInChannel);
	}

#else
	if (m_pAudPlayChan==NULL)
	{
		AUDEC_GetEngine().AUDEC_CreatePlayChan(&m_pAudPlayChan);
		if (m_pAudPlayChan!=NULL)
		{
			if (0!=m_pAudPlayChan->Connect(&m_nVideoSynTime,&m_nAudioSynTime))
			{
				m_pAudPlayChan->ReleaseConnections();
				delete m_pAudPlayChan;
				m_pAudPlayChan=NULL;
			}
			else
			{
				m_pAudPlayChan->SetSynFlag(m_nSynFlag);
				m_pAudPlayChan->SetRecordSamples(this);
			}
		}
	}
#endif
	return 0;
}

void PlayChan::Close(void)
{
	StopRecordToAVI();

	PlayChanMgr::Instance().RemovePlayChan(this);

	StopMixingAudio();

	StopAudio();
	StopVideo();

	if (m_pMediaReceiver!=NULL)
	{
		m_pMediaReceiver->Close();
		delete m_pMediaReceiver;
		m_pMediaReceiver=NULL;
	}

#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer!=NULL)
	{
		m_pVideoPlayer->Close();
		delete m_pVideoPlayer;
		m_pVideoPlayer=NULL;
	}

#else
	if (m_pVidPlayChan!=NULL)
	{
		m_pVidPlayChan->Close();
		delete m_pVidPlayChan;
		m_pVidPlayChan=NULL;
	}

	if (m_pWnd!=NULL)
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;
	}
#endif

	//Add WebRTC by zys 20160929
#if USE_WEBRTC
	if (NULL != m_pWebRTCAudio)
	{
		if (-1 != m_nWebRTCPlayChan)
		{
			m_pWebRTCAudio->StopPlayout(m_nWebRTCPlayChan);
			m_pWebRTCAudio->DestroyDecodeChannel(m_nWebRTCPlayChan);
			AudioPlayChanMgr::Instance().m_pAudioMixer->StopInputMixing(m_nAudioMixInChannel);

			AudioPlayChanMgr::Instance().m_pAudioMixerChan->StopInputMixing(m_nAudioMixChanInChannel);
			m_nWebRTCPlayChan = -1;
		}
		
		//MyVoiceEngineInterface::DestroyVoiceEngine(m_pWebRTCAudio);
		//m_pWebRTCAudio = NULL;
	}
#else
	if (NULL!=m_pAudPlayChan)
	{
		m_pAudPlayChan->ReleaseConnections();
		delete m_pAudPlayChan;
		m_pAudPlayChan=NULL;
	}
#endif
}

//取视频窗口
HWND PlayChan::GetVideoWnd(void)
{
#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer==NULL)
	{
		return NULL;
	}

	return m_pVideoPlayer->GetWnd();
#else
	if (m_pWnd==NULL)
	{
		return NULL;
	}

	return m_pWnd->GetWnd();
#endif
}

//取视频宽度
int PlayChan::GetVideoWidth(void)
{
#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer==NULL)
	{
		return 320;
	}

	return m_pVideoPlayer->GetVideoWidth();
#else
	if (m_pWnd==NULL)
	{
		return 320;
	}

	return m_pWnd->GetVideoWidth();
#endif
}

//取视频高度
int PlayChan::GetVideoHeight(void)
{
#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer==NULL)
	{
		return 240;
	}

	return m_pVideoPlayer->GetVideoHeight();
#else
	if (m_pWnd==NULL)
	{
		return 240;
	}

	return m_pWnd->GetVideoHeight();
#endif
}

//same as CWnd::IsWindowVisible
bool PlayChan::IsWindowVisible(void)
{
#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer==NULL)
	{
		return false;
	}

	return m_pVideoPlayer->IsWindowVisible();
#else
	if (m_pWnd==NULL)
	{
		return false;
	}

	return m_pWnd->IsWindowVisible();
#endif
}

//same as CWnd::SetWindowPos
bool PlayChan::SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags)
{
#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer!=NULL)
	{
		return m_pVideoPlayer->SetWindowPos(hWndInsertAfter,x,y,cx,cy,nFlags);
	}
#else
	if (m_pWnd!=NULL)
	{
		return m_pWnd->SetWindowPos(hWndInsertAfter,x,y,cx,cy,nFlags);
	}
#endif
	return false;
}

//保持当前图象到BMP文件
bool PlayChan::CaptureToBMPFile(const char*cszFile)
{
#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer!=NULL)
	{
		return m_pVideoPlayer->CaptureToBMPFile(cszFile);
	}
#else
	if (m_pWnd!=NULL)
	{
		return m_pWnd->CaptureToBMPFile(cszFile);
	}
#endif
	return false;
}

//设置视频属性
int PlayChan::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer)
	{
		return m_pVideoPlayer->SetVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}
#else
	if (m_pWnd)
	{
		m_pWnd->SetVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
		return 0;
	}
#endif
	return -1;
}

//int nGammaR[1-400 100]
//int nGammaG[1-400 100]
//int nGammaB[1-400 100]
int PlayChan::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer)
	{
		return m_pVideoPlayer->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
	}
#else
	if (m_pWnd)
	{
		m_pWnd->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
		return 0;
	}
#endif
	return -1;
}

/******************************************************************************
* SetScreenWnd
*描述：设置所在屏幕窗口位置
*输入：	nScreenIndex	-屏幕号
		nWndIndex		-窗口号
		nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
*输出：无
*返回值：无
*****************************************************************************/
void PlayChan::SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	m_nScreenIndex=nScreenIndex;
#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer!=NULL)
	{
		m_pVideoPlayer->SetScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
#else
	if (m_pWnd!=NULL)
	{
		m_pWnd->SetScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
#endif

	int nMixerScreenIndex=CapChanDevMgr::Instance().GetMixerScreenIndex();
	if (nMixerScreenIndex>=0 && m_nAudioChanID)
	{
		if (nMixerScreenIndex==nScreenIndex)
		{
			AudioCapChanMgr::Instance().StartMixingWithPlayChan(AudioPlayChanMgr::Instance().GetPlayChan(m_nAudioChanID,nMixerScreenIndex,true));
		}
		else
		{
			AudioCapChanMgr::Instance().StopMixingWithPlayChan(AudioPlayChanMgr::Instance().GetPlayChan(m_nAudioChanID,nMixerScreenIndex,false));
		}
	}
}

void PlayChan::ApplyMixerScreenIndexChanged(int nMixerScreenIndex)
{
	if (m_nAudioChanID)
	{
		if (nMixerScreenIndex==m_nScreenIndex)
		{
			AudioCapChanMgr::Instance().StartMixingWithPlayChan(AudioPlayChanMgr::Instance().GetPlayChan(m_nAudioChanID,nMixerScreenIndex,true));
		}
		else
		{
			AudioCapChanMgr::Instance().StopMixingWithPlayChan(AudioPlayChanMgr::Instance().GetPlayChan(m_nAudioChanID,nMixerScreenIndex,false));
		}
	}
}

void PlayChan::SetVideoWndBk(const char* szBkImage, int nVideoWnBkR, int nVideoWnBkG, int nVideoWnBkB)
{
	if (szBkImage)
	{
		m_strVideoWndBkImage = szBkImage;
	}
	m_nVideoWnBkR = nVideoWnBkR;
	m_nVideoWnBkG = nVideoWnBkG;
	m_nVideoWnBkB = nVideoWnBkB;
}

int PlayChan::SetVolume(int nVolume)
{
	m_nAudioVolume=nVolume;
	if (m_nAudioEnable==0)
	{
		return 0;
	}

#if USE_WEBRTC
	if (m_nWebRTCPlayChan != -1)
	{
		if (m_pWebRTCAudio)
		{
			return m_pWebRTCAudio->SetSpeakerVolume(nVolume*255/100);
		}
	}
#else
	if (m_pAudPlayChan!=NULL)
	{
		return m_pAudPlayChan->SetVolume(nVolume);
	}

#endif
	return AudioPlayChanMgr::Instance().SetVolume(m_nAudioChanID,nVolume);
}

int PlayChan::GetVolume()
{
	if (m_nAudioEnable==0)
	{
		return 0;
	}
#if USE_WEBRTC
	if (m_nWebRTCPlayChan != -1)
	{
		if (m_pWebRTCAudio)
		{
			int nValue = m_pWebRTCAudio->GetSpeakerVolume();
			int nTemp = nValue*100%255;
			nValue = nValue*100/255;
			if (nTemp != 0)
			{
				nValue++;
			}
			return nValue;
		}
	}
#else
	if (m_pAudPlayChan!=NULL)
	{
		return m_pAudPlayChan->GetVolume();
	}
#endif

	return AudioPlayChanMgr::Instance().GetVolume(m_nAudioChanID);
}

int PlayChan::GetLevel(int&nLevel)
{
	if (m_nAudioEnable==0)
	{
		nLevel=0;
		return 0;
	}

#if USE_WEBRTC
	if (m_nWebRTCPlayChan != -1)
	{
		if (NULL != m_pWebRTCAudio)
		{
			//接口获取db范围为-96db~0db，将其转化为0-10输出
			nLevel=(((int)m_pWebRTCAudio->GetSpeakerSpeechDBValue())*10/96)+10;

			if (nLevel<0)
			{
				nLevel=0;
				return -1;
			}
			return 0;
		}
	}
#else
	if (m_pAudPlayChan!=NULL)
	{
		nLevel=m_pAudPlayChan->GetLevel();
		if (nLevel<0)
		{
			nLevel=0;
			return -1;
		}
		return 0;
	}
#endif

	nLevel=0;
	return AudioPlayChanMgr::Instance().GetLevel(m_nAudioChanID,nLevel);
}

void PlayChan::SetMute(bool bMute)
{
	if (m_nAudioEnable==0)
	{
		return;
	}

	m_bMute = bMute;
#if USE_WEBRTC
	if (m_nWebRTCPlayChan != -1)
	{
		if (m_pWebRTCAudio)
		{
			m_pWebRTCAudio->SetSpeakerMute(m_bMute);
			return;
		}
	}
	
#else
	if (m_pAudPlayChan!=NULL)
	{
		m_pAudPlayChan->SetMute(m_bMute?1:0);
		return;
	}
#endif

	return AudioPlayChanMgr::Instance().SetMute(m_nAudioChanID,m_bMute);
}

bool PlayChan::IsMute()
{
	return m_bMute;
}

int PlayChan::StartAudio(void)
{
	//Add WebRTC by zys 20160929
#if USE_WEBRTC

	if (m_nAudioStarted==0 && m_nAudioChanID!=0)
 	{
 		m_nAudioStarted=1;
 
 		int nRet = AudioPlayChanMgr::Instance().OpenAudioPlayChan(this,m_nAudioChanID,m_strPeerNodeID.c_str(),m_strPeerNATIP.c_str(),m_nPeerNATPort,m_strPeerLocalIP.c_str(),m_nPeerLocalPort,m_strPeerMCUID.c_str(),m_strPeerMCUIP.c_str(),m_nPeerMCUPort,m_nForceUserMCU,m_nSynFlag,&m_nVideoSynTime,&m_nAudioSynTime);
 		AudioPlayChanMgr::Instance().EnableAudio(m_nAudioChanID,m_nAudioEnable);
 		AudioPlayChanMgr::Instance().SetSynFlag(m_nAudioChanID,m_nSynFlag,&m_nVideoSynTime,&m_nAudioSynTime);
 
 		AudioCapChanMgr::Instance().ExcludePlayChan(m_nKeyID,AudioPlayChanMgr::Instance().GetPlayChan(m_nAudioChanID,-2,false));
 
 		return nRet;
 	}
#else

	if (m_pAudPlayChan==NULL)
	{
		if (m_nAudioStarted==0 && m_nAudioChanID!=0)
		{
			m_nAudioStarted=1;
			int nRet = AudioPlayChanMgr::Instance().OpenAudioPlayChan(this,m_nAudioChanID,m_strPeerNodeID.c_str(),m_strPeerNATIP.c_str(),m_nPeerNATPort,m_strPeerLocalIP.c_str(),m_nPeerLocalPort,m_strPeerMCUID.c_str(),m_strPeerMCUIP.c_str(),m_nPeerMCUPort,m_nForceUserMCU,m_nSynFlag,&m_nVideoSynTime,&m_nAudioSynTime);
			AudioPlayChanMgr::Instance().EnableAudio(m_nAudioChanID,m_nAudioEnable);
			AudioPlayChanMgr::Instance().SetSynFlag(m_nAudioChanID,m_nSynFlag,&m_nVideoSynTime,&m_nAudioSynTime);

			AudioCapChanMgr::Instance().ExcludePlayChan(m_nKeyID,AudioPlayChanMgr::Instance().GetPlayChan(m_nAudioChanID,-2,false));

			return nRet;
		}
	}

#endif
	return 0;
}

int PlayChan::EnableAudio(int nEnable/*0:不接收，1:接收*/)
{
	m_nAudioEnable=nEnable;
	if (m_nAudioEnable!=0)
	{
#if !USE_WEBRTC
		SetVolume(m_nAudioVolume);
#endif
	}

	if (m_pAudPlayChan==NULL)
	{
		if (m_nAudioChanID!=0)
		{
			AudioPlayChanMgr::Instance().EnableAudio(m_nAudioChanID,m_nAudioEnable);
		}
	}
	else if (m_pMediaReceiver!=NULL)
	{
		m_pMediaReceiver->EnableAudio(m_nAudioEnable);
	}

	return 0;
}

int PlayChan::SetAudioID(unsigned long nAudioID)
{
	if (m_pAudPlayChan==NULL)
	{
		if (nAudioID!=m_nAudioChanID)
		{
			if (m_nAudioChanID!=0)
			{
				AudioPlayChanMgr::Instance().CloseAudioPlayChan(this,m_nAudioChanID);
			}

			m_nAudioChanID=nAudioID;
			
			if (m_nAudioChanID!=0 && m_nAudioStarted!=0)
			{
				AudioPlayChanMgr::Instance().OpenAudioPlayChan(this,m_nAudioChanID,m_strPeerNodeID.c_str(),m_strPeerNATIP.c_str(),m_nPeerNATPort,m_strPeerLocalIP.c_str(),m_nPeerLocalPort,m_strPeerMCUID.c_str(),m_strPeerMCUIP.c_str(),m_nPeerMCUPort,m_nForceUserMCU,m_nSynFlag,&m_nVideoSynTime,&m_nAudioSynTime);
				AudioPlayChanMgr::Instance().EnableAudio(m_nAudioChanID,m_nAudioEnable);
				AudioPlayChanMgr::Instance().SetSynFlag(m_nAudioChanID,m_nSynFlag,&m_nVideoSynTime,&m_nAudioSynTime);

				AudioCapChanMgr::Instance().ExcludePlayChan(m_nKeyID,AudioPlayChanMgr::Instance().GetPlayChan(m_nAudioChanID,-2,false));
			}
		}
	}

	return 0;
}

int PlayChan::StopAudio(void)
{
	//Add WebRTC by zys 20160929
 #if USE_WEBRTC

	m_nAudioStarted=0;

	AudioCapChanMgr::Instance().ExcludePlayChan(m_nKeyID,NULL);

	AudioPlayChanMgr::Instance().CloseAudioPlayChan(this,m_nAudioChanID);

 #else
	if (m_pAudPlayChan==NULL)
	{
		m_nAudioStarted=0;

		AudioCapChanMgr::Instance().ExcludePlayChan(m_nKeyID,NULL);

		AudioPlayChanMgr::Instance().CloseAudioPlayChan(this,m_nAudioChanID);
	}
#endif

	return 0;
}

int PlayChan::StartVideo(void)
{
	if (m_pMediaReceiver==NULL)
	{
		return -1;
	}

	m_nVideoStarted=1;

	return m_pMediaReceiver->StartVideo();
}

int PlayChan::EnableVideo(int nEnable/*0:不接收，1:接收*/)
{
	if (m_pMediaReceiver==NULL)
	{
		return -1;
	}

	m_nEnableVideo=nEnable;

	if (m_pVideoPlayer)
	{
		m_pVideoPlayer->SetEnableVideo(m_nEnableVideo);
	}
	
	return m_pMediaReceiver->EnableVideo(nEnable);
}

int PlayChan::StopVideo(void)
{
	m_nVideoStarted=0;

	if (m_pMediaReceiver==NULL)
	{
		return -1;
	}
	
	m_pMediaReceiver->StopVideo();
	return 0;
}

int PlayChan::SetAudioCast(bool bAudioCast)
{
	int nRet=-1;
	bool bChanged=(m_bAudioCast!=bAudioCast);

	m_bAudioCast=bAudioCast;
	if (m_pAudPlayChan==NULL)
	{
		if (m_nAudioChanID!=0)
		{
			nRet=AudioPlayChanMgr::Instance().SetAudioCast(m_nAudioChanID,bAudioCast);
		}
	}
	else if (m_pMediaReceiver!=NULL)
	{
		nRet=m_pMediaReceiver->SetAudioCast(bAudioCast);
	}

	if (bChanged)
	{
		int nMixerScreenIndex=CapChanDevMgr::Instance().GetMixerScreenIndex();
		if (nMixerScreenIndex>=0)
		{
			ApplyMixerScreenIndexChanged(nMixerScreenIndex);
		}
	}

	return nRet;
}

int PlayChan::SetVideoCast(bool bVideoCast)
{
	if (m_pMediaReceiver==NULL)
	{
		return -1;
	}

	return m_pMediaReceiver->SetVideoCast(bVideoCast);
}

/******************************************************************************
* RequestKeyFrame
*描述：请求关键帧
*输入：	无
*输出： 无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int PlayChan::RequestKeyFrame(void)
{
	if (m_pMediaReceiver==NULL)
	{
		return -1;
	}
	
	m_pMediaReceiver->RequestKeyFrame();
	return 0;
}

/******************************************************************************
* SetFrameRateControlMode
*描述：设置帧率控制模式
*输入：	nMode			-帧率控制模式
*输出： 无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int PlayChan::SetFrameRateControlMode(FRAME_RATE_CONTROL_MODE nMode)
{
	if (m_pMediaReceiver==NULL)
	{
		return -1;
	}
	
	m_pMediaReceiver->SetFrameRateControlMode(nMode);
	return 0;
}

/******************************************************************************
* SetFrameTypeControlMode
*描述：设置帧类型控制模式
*输入：	nMode			-帧类型控制模式
*输出： 无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int PlayChan::SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE nMode)
{
	m_nFrameTypeControlMode=nMode;
	m_nFrameTypeControlModeCurrent=nMode;
	if (m_pMediaReceiver==NULL)
	{
		return -1;
	}
	
	m_pMediaReceiver->SetFrameTypeControlMode(nMode);
	return 0;
}


void PlayChan::OnAudioPlayChanUIRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)
{
	m_rCallback.OnIMXEC_PlayChanCallbackAudioSamples(pSamples,nSamples,nSampleRate,nTimestamp);

	{
		XAutoLock l(m_csAVIWriter);
		if (m_pAVIWriter!=NULL)
		{
			m_pAVIWriter->WriteAudio((unsigned char*)pSamples,nSamples<<1,nSampleRate,nTimestamp);
		}
	}
}


void PlayChan::OnRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)
{
	OnAudioPlayChanUIRecordSamples(pSamples,nSamples,nSampleRate,nTimestamp);
}

void PlayChan::OnAudioPlayChanUIAudioPacket(unsigned char*pData,int nLen,unsigned long nRefCount,bool&bLoop)
{
	if (nRefCount==0)
		m_nTotalBytes+=nLen;

	m_rCallback.OnIMXEC_PlayChanCallbackAudChanData(pData,nLen,bLoop);
}

void PlayChan::OnAudioPlayChanUIGetSynFlag(int&nSynFlag,unsigned long**ppnVideoSynTime,unsigned long**ppnAudioSynTime)
{
	nSynFlag=m_nSynFlag;
	*ppnVideoSynTime=&m_nVideoSynTime;
	*ppnAudioSynTime=&m_nAudioSynTime;
}

void PlayChan::OnAudioPlayChanUIGetAudioEnable(int&nEnable)
{
	nEnable=m_nAudioEnable;
}

void PlayChan::OnAudioPlayChanUIGetScreenIndex(int&nScreenIndex)
{
	nScreenIndex=m_nScreenIndex;
}

void PlayChan::OnAudioPlayChanUIGetAudioCast(bool&bAudioCast)
{
	bAudioCast=m_bAudioCast;
}

/******************************************************************************
* OnNETEC_MediaReceiverCallbackVideoPacket
*描述：接收到视频数据包的回调
*输入：	pData			-数据包指针
		nLen			-数据包长度
*输出： 无
*返回值：无
*****************************************************************************/
void PlayChan::OnNETEC_MediaReceiverCallbackVideoPacket(unsigned char*pData,int nLen)
{
	m_XBitrateVideo.Calculate(nLen);

	m_nTotalBytes+=nLen;

	int nKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pData);

	bool bHareware=VIDEC_HEADER_EXT_GET_HARDWARE(pData)!=0;
	if (bHareware && m_pMediaReceiver && GetHarewareDecoder())
	{
		bool bChanged=false;
		if (VIDEC_HEADER_EXT_GET_KEYFRAME(pData))
		{
			int nWidth=VIDEC_HEADER_EXT_GET_VIRTUAL_WIDTH(pData);
			int nHeight=VIDEC_HEADER_EXT_GET_VIRTUAL_HEIGHT(pData);
			if (nWidth!=m_nVirtualWidth || nHeight!=m_nVirtualHeight)
			{
				if (m_bHardwareError==false)
				{
					bChanged=true;
				}

				m_nVirtualWidth=nWidth;
				m_nVirtualHeight=nHeight;
			}
		}

		//if ((m_nVirtualWidth==1024 && m_nVirtualHeight==768) ||
		//	(m_nVirtualWidth==1280 && m_nVirtualHeight==1024) ||
		//	(m_nVirtualWidth==1280 && m_nVirtualHeight==720) ||
		//	(m_nVirtualWidth==960 && m_nVirtualHeight==540) ||
		//	(m_nVirtualWidth==800 && m_nVirtualHeight==600))
		//{

		//}
		//else
		{
			if (m_nVirtualWidth>1280)
			{
				if (m_nFrameTypeControlModeCurrent==FRAME_TYPE_CONTROL_MODE_MAIN)
				{
					m_nFrameTypeControlModeCurrent=FRAME_TYPE_CONTROL_MODE_SUB;
					m_pMediaReceiver->SetFrameTypeControlMode(m_nFrameTypeControlModeCurrent);
				}

				if (m_bHardwareError==false)
				{
					m_bHardwareError=true;
					m_rCallback.OnIMXEC_PlayChanCallbackHardwareError(1);
				}
				return;
			}
		}

		if (bChanged && m_nFrameTypeControlModeCurrent!=m_nFrameTypeControlMode)
		{
			m_nFrameTypeControlModeCurrent=m_nFrameTypeControlMode;
			m_pMediaReceiver->SetFrameTypeControlMode(m_nFrameTypeControlModeCurrent);
		}
	}

	if (m_bHardwareError)
	{
		m_bHardwareError=false;
		m_rCallback.OnIMXEC_PlayChanCallbackHardwareError(0);
	}

	int nFlagDontDecode=0;
	m_rCallback.OnIMXEC_PlayChanCallbackVidChanData(pData,nLen,nKeyFrame,nFlagDontDecode);

#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer!=NULL && nFlagDontDecode==0)
	{
		if (m_pVideoPlayer->PlayData(pData,nLen)<0)
		{
			unsigned long nTimestamp=XGetTimestamp();
			if (nTimestamp-m_nLastRequestKeyFrameTS>=2)
			{
				m_nLastRequestKeyFrameTS=nTimestamp;
				RequestKeyFrame();
			}
		}
	}
#else
	if (m_pVidPlayChan!=NULL && nFlagDontDecode==0)
	{
		m_pVidPlayChan->OnReceivedStreamData(NULL,pData,nLen,nKeyFrame!=0);
	}
#endif
}

void PlayChan::OnVIDEC_PlayerCallbackPlayData(void*pPacketData,int nPacketSize)
{
	XAutoLock l(m_csAVIWriter);
	if (m_pAVIWriter!=NULL)
	{
		m_pAVIWriter->WriteVideo((unsigned char*)pPacketData,nPacketSize);
	}
}

void PlayChan::OnVIDEC_PlayerCallbackVQDResult(unsigned long nResult)
{
	m_rCallback.OnIMXEC_PlayChanCallbackVQDResult(nResult);
}

void PlayChan::OnVIDEC_PlayerCallbackDrawD3D9DC(void* hDC/*HDC*/)
{
	m_rCallback.OnIMXEC_PlayChanCallbackDrawD3D9DC(hDC);
}

/******************************************************************************
* StartRecordToAVI
*描述：开始录像
*输入：	cszPathName		-录像文件名称
		nStandardFormat	-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int PlayChan::StartRecordToAVI(const char*cszPathName,int nStandardFormat, int nCacheTimeMS)
{
	StopRecordToAVI();

	VIDEC_AVIWriter*pAVIWriter=VIDEC_AVIWriter::Create();
	if (pAVIWriter==NULL)
	{
		return -1;
	}

	if (0!=pAVIWriter->Open(cszPathName,nStandardFormat,2013265920,nCacheTimeMS))
	{
		pAVIWriter->Close();
		delete pAVIWriter;
		pAVIWriter=NULL;

		return -1;
	}
	
	{
		XAutoLock l(m_csAVIWriter);
		m_pAVIWriter=pAVIWriter;
	}

	RequestKeyFrame();

	return 0;
}

/******************************************************************************
* StopRecordToAVI
*描述：停止录像
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void PlayChan::StopRecordToAVI(void)
{
	VIDEC_AVIWriter*pAVIWriter=m_pAVIWriter;
	{
		XAutoLock l(m_csAVIWriter);
		m_pAVIWriter=NULL;
	}
	if (pAVIWriter!=NULL)
	{
		pAVIWriter->Close();
		delete pAVIWriter;
		pAVIWriter=NULL;
	}
}

void PlayChan::SetRecording(int nFlagRecording)
{
	if (m_pMediaReceiver)
	{
		m_pMediaReceiver->SetRecording(nFlagRecording);
	}
}

int IMXEC_PlayChan::GetSpeakerDevNum()
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	return pVoiceEngine->GetNumOfPlayDevices();
#endif
	return 0;
}

int IMXEC_PlayChan::GetSpeakerDevName(int nIndex, char* szName, int nMaxCount)
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	PlayDevice dev;
	int ret = pVoiceEngine->GetPlayDevices(nIndex, dev);

	USES_CONVERSION;
	wstring strW = A2W_CP(dev.deviceName, CP_UTF8);

	strcpy_s(szName, nMaxCount, W2A(strW.c_str()));
	return ret;
#endif
	return 0;
}

int IMXEC_PlayChan::SetSpeakerDev(int nIndex)
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	return pVoiceEngine->SetPlayDevice(nIndex);
#else
	return AUDEC_GetEngine().AUDEC_SetSpeakerDev(nIndex);
#endif
}

int IMXEC_PlayChan::SetSpeakerVolume(int nVolume)
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	return pVoiceEngine->SetSpeakerVolume(nVolume*255/100);
#endif
}

int IMXEC_PlayChan::GetSpeakerVolume()
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	int nValue = pVoiceEngine->GetSpeakerVolume();
	int nTemp = nValue*100%255;
	nValue = nValue*100/255;
	if (nTemp != 0)
	{
		nValue++;
	}
	return nValue;
#endif
	
}

int IMXEC_PlayChan::SetSpeakerMute(bool bMute)
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	return pVoiceEngine->SetSpeakerMute(bMute);
#endif
}

int IMXEC_PlayChan::StartPlayLocalWAVFile (const char* filename, bool loop)
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	USES_CONVERSION;
	std::wstring strW = A2W(filename);
	char* filename_Utf8 = W2A_UTF8(strW.c_str());

	return pVoiceEngine->StartPlayLocalWAVFile(filename_Utf8, loop);
#endif
	return 0;
}

int IMXEC_PlayChan::StopPlayLocalWAVFile ()
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	return pVoiceEngine->StopPlayLocalWAVFile();
#endif
	return 0;
}

/******************************************************************************
* GetAudioPacketStatistics
*描述：获取音频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int PlayChan::GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if (m_nAudioEnable==0)
	{
		nBitrate=0;
		ulTotalPackets=0;
		ulLostPackets=0;
		AvgPacketLossRate=0.00;
		CurPacketLossRate=0.00;
		return 0;
	}

	nBitrate=0;

 #if USE_WEBRTC
 	if ((NULL != m_pWebRTCAudio) && (-1 != m_nWebRTCPlayChan))
 	{
 		m_pWebRTCAudio->EnableStatistics(true);
 		nBitrate=m_pWebRTCAudio->GetStatistics(m_nWebRTCPlayChan);
 		nBitrate = 0;
 
 		ulTotalPackets=0;
 		ulLostPackets=0;
 		AvgPacketLossRate=0.00;
 		CurPacketLossRate=0.00;
 
 		return 0;
 	}
 #else
	if (m_pAudPlayChan!=NULL)
	{
		nBitrate=m_pAudPlayChan->GetBitrate();

		ulTotalPackets=0;
		ulLostPackets=0;
		AvgPacketLossRate=0.00;
		CurPacketLossRate=0.00;

		return 0;
	}
#endif
	
	return AudioPlayChanMgr::Instance().GetAudioPacketStatistics(m_nAudioChanID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);

}

/******************************************************************************
* GetVideoPacketStatistics
*描述：获取视频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int PlayChan::GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	nBitrate=m_XBitrateVideo.GetBitrate();
	if (m_pMediaReceiver!=NULL)
	{
		m_pMediaReceiver->GetVideoPacketStatistics((uint32_t &)ulTotalPackets,(uint32_t &)ulLostPackets,AvgPacketLossRate,CurPacketLossRate);
	}
	else
	{
		ulTotalPackets=0;
		ulLostPackets=0;
		AvgPacketLossRate=0.00;
		CurPacketLossRate=0.00;
	}
	
	return 0;
}


/******************************************************************************
* PlayAudioData
*描述：播放音频数据
*输入：	pData			-数据指针
		nLen			-数据长度
*输出： 无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int PlayChan::PlayAudioData(const void*pData,int nLen)
{
	if (m_pMediaReceiver!=NULL)
	{
		return -1;
	}


	//---------------------------------------
	//Add WebRTC by zys 20160929
#if USE_WEBRTC


	unsigned char *packet = (unsigned char *)pData;
	if ( (NULL != m_pWebRTCAudio) && (-1 != m_nWebRTCPlayChan) )
	{
		unsigned char pRTPPacket[1500];
		int nRTPPacketLen = AudecHeaderToRTPHeader(pRTPPacket,pData,nLen);

		m_pWebRTCAudio->Decode(m_nWebRTCPlayChan, pRTPPacket, nRTPPacketLen);
		AudioPlayChanMgr::Instance().m_pAudioMixer->AddRtpData(m_nAudioMixInChannel, (const char *)pRTPPacket, nRTPPacketLen);

		AudioPlayChanMgr::Instance().m_pAudioMixerChan->AddRtpData(m_nAudioMixChanInChannel, (const char *)pRTPPacket, nRTPPacketLen);
	}
	else
	{
		AudioPlayChanMgr::Instance().PlayAudioData(m_nAudioChanID,pData,nLen);
	}
#else
	if (m_pAudPlayChan!=NULL)
	{
		m_pAudPlayChan->Write(pData,nLen);
	}
	else
	{
		AudioPlayChanMgr::Instance().PlayAudioData(m_nAudioChanID,pData,nLen);
	}
#endif
	return 0;
}

/******************************************************************************
* PlayVideoData
*描述：播放视频数据
*输入：	pData			-数据指针
		nLen			-数据长度
*输出： 无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int PlayChan::PlayVideoData(const void*pData,int nLen)
{
	if (m_pMediaReceiver!=NULL)
	{
		return -1;
	}

	OnNETEC_MediaReceiverCallbackVideoPacket((unsigned char*)pData,nLen);

	return 0;
}


void PlayChan::SendCtrlData(void*pData,int nLen)
{
	if (m_pMediaReceiver!=NULL)
	{
		m_pMediaReceiver->SendCtrl(pData,nLen);
	}
}


/******************************************************************************
* SetSynFlag
*描述：设置视频音频同步标记
*输入：	nSynFlag		-视频音频同步标记【0：视频音频不同步，非0：视频音频同步】
*输出：无
*返回值：无
*****************************************************************************/
void PlayChan::SetSynFlag(int nSynFlag)
{
	m_nSynFlag=nSynFlag;
	if (m_pAudPlayChan!=NULL)
	{
		m_pAudPlayChan->SetSynFlag(m_nSynFlag);
	}

#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer!=NULL)
	{
		m_pVideoPlayer->SetSynFlag(m_nSynFlag);
	}
#else
	if (m_pWnd!=NULL)
	{
		m_pWnd->SetSynFlag(m_nSynFlag);
	}
#endif

	if (m_nAudioChanID!=0)
	{
		AudioPlayChanMgr::Instance().SetSynFlag(m_nAudioChanID,m_nSynFlag,&m_nVideoSynTime,&m_nAudioSynTime);
	}
}

void PlayChan::SetImage(VIDEC_Image*pImage)
{
#ifdef __VIDEC_PLAYER_H__
	if (m_pVideoPlayer!=NULL)
	{
		m_pVideoPlayer->ResetImage(pImage);
	}
#else
	if (m_pWnd!=NULL)
	{
		m_pWnd->Reset();
		m_pWnd->OnReceivedImage(pImage);
	}
#endif
}

unsigned long PlayChan::DoGetTotalBytes(void)
{
	unsigned long nTotalBytes=m_nTotalBytes;
	m_nTotalBytes=0;
	return nTotalBytes;
}

void PlayChan::OnTimer(void)
{
	if (m_nEnableVideo!=0 && m_nVideoStarted!=0 && m_pMediaReceiver!=NULL)
	{
		if (::IsWindowVisible(m_hwndParent) || 
			IMXEC_PlayChan::GetForcibleReceiving() || 
#ifdef __VIDEC_PLAYER_H__
			(m_pVideoPlayer!=NULL && m_pVideoPlayer->GetSupportPreview()!=0) ||
#else
			(m_pWnd!=NULL && m_pWnd->GetSupportPreview()!=0) ||
#endif
			m_pAVIWriter!=NULL)
		{
			m_pMediaReceiver->EnableVideo(m_nEnableVideo);
		}
		else
		{
			m_pMediaReceiver->EnableVideo(0);
		}
	}
}

int PlayChan::StartMixingAudio(X_AUDIO_CODEC_ID nCodecID)
{
	m_nMixingAudioCodecID=nCodecID;
	return AudioCapChanMgr::Instance().OpenMixingAudio(m_nKeyID,this,AudioPlayChanMgr::Instance().GetPlayChan(m_nAudioChanID,-2,false),m_nMixingAudioCodecID);
}

void PlayChan::StopMixingAudio(void)
{
	AudioCapChanMgr::Instance().CloseMixingAudio(m_nKeyID,this);
}

int PlayChan::GetMixingAudioID(unsigned long&nAudioID)
{
	return AudioCapChanMgr::Instance().GetMixingAudioID(m_nKeyID,nAudioID);
}

AUDEC_PlayChan*PlayChan::GetPlayChan(void)
{
	return AudioPlayChanMgr::Instance().GetPlayChan(m_nAudioChanID,-2,false);
}


int PlayChan::StartVQD(unsigned long nDetectFlags)
{
	if (m_pVideoPlayer)
	{
		return m_pVideoPlayer->StartVQD(nDetectFlags);
	}

	return -1;
}

void PlayChan::StopVQD(void)
{
	if (m_pVideoPlayer)
	{
		m_pVideoPlayer->StopVQD();
	}
}

int PlayChan::GetDecoderType(void)
{
	if (m_pVideoPlayer)
	{
		return m_pVideoPlayer->GetDecoderType();
	}
	return 0;
}

int PlayChan::GetVideoShowType(void)
{
	if (m_pVideoPlayer)
	{
		return m_pVideoPlayer->GetVideoShowType();
	}
	return 0;
}
int PlayChan::GetVideoShowFrameRate(void)
{
	if (m_pVideoPlayer)
	{
		return m_pVideoPlayer->GetVideoShowFrameRate();
	}
	return 0;
}

int PlayChan::GetVideoDecodeFrameRate(void)
{
	if (m_pVideoPlayer)
	{
		return m_pVideoPlayer->GetVideoDecodeFrameRate();
	}
	return 0;
}

int PlayChan::GetVideoRecvFrameRate(void)
{
	if (m_pVideoPlayer)
	{
		return m_pVideoPlayer->GetVideoRecvFrameRate();
	}
	return 0;
}

#ifdef _TIANZHU_CACHE_
int PlayChan::SetRemoteVideoCache(int nCacheFrameCount)
{
	if (m_pVideoPlayer)
	{
		m_pVideoPlayer->SetCacheFrameCount(nCacheFrameCount);
		return 0;
	}
	return -1;
}
#endif

void PlayChan::SetVideoDelay(int nMilliSecond)
{
	if (m_pVideoPlayer)
	{
		m_pVideoPlayer->SetVideoDelay(nMilliSecond);
	}
}

int PlayChan::SetVideoPolledMode(void)
{
	return SetVideoSkipFrame(1,21);
}

int PlayChan::SetVideoSkipFrame(int nInterval,int nSkip)
{
	if (m_pVideoPlayer)
	{
		return m_pVideoPlayer->SetVideoSkipFrame( nInterval, nSkip);
	}
	return -1;
}

//notice netec send ptz 
int PlayChan::SendCtrl(void*pData,int nLen)
{
	if (m_pMediaReceiver!=NULL)
	{
		return m_pMediaReceiver->SendCtrl(pData,nLen);
	}
	return 0;
}

int PlayChan::SendPTZ(void*pData,int nLen)
{
	if (m_pMediaReceiver!=NULL)
	{
		return m_pMediaReceiver->SendPTZ(pData,nLen);
	}
	return 0;
}

//Add WebRTC by zys 20160929
int PlayChan::AudecHeaderToRTPHeader(unsigned char* pRTPData,void const* pAudecData,size_t nAudecLen)
{
	X_AUDIO_CODEC_ID nCodecID = (X_AUDIO_CODEC_ID)AUDEC_HEADER_GET_CODEC_ID(pAudecData);

	//m_nAudioSynTime = AUDEC_HEADER_GET_TIMESTAMP(pAudecData); //唇音同步

	std::string strCodecName="gsm";
	int nSampleRate=8000;
	int nBitrate=13200;
	int nPayloadType=0;
	int nSamplesPerFrame=160;

	switch (nCodecID)
	{
	case X_AUDIO_CODEC_G711U:
		{
			strCodecName="PCMU";
			nSampleRate=8000;
			nBitrate=64000;
			nPayloadType=0;
			nSamplesPerFrame=160;
		}
		break;
	case X_AUDIO_CODEC_G711A:
		{
			strCodecName="PCMA";
			nSampleRate=8000;
			nBitrate=64000;
			nPayloadType=8;
			nSamplesPerFrame=160;
		}
		break;
	case X_AUDIO_CODEC_G722_64:
		{
			strCodecName="G722";
			nSampleRate=16000;
			nBitrate=64000;
			nPayloadType=9;
			nSamplesPerFrame=160;
		}
		break;

	case X_AUDIO_CODEC_SIREN_G7221_7_24://16khz
	case X_AUDIO_CODEC_G7221_7_24://16khz
		{
			strCodecName="g7221";
			nSampleRate=16000;
			nBitrate=24000;
			nPayloadType=121;
			nSamplesPerFrame=320;
		}
		break;

	case X_AUDIO_CODEC_SIREN_G7221_7_32://16khz
	case X_AUDIO_CODEC_G7221_7_32://16khz
		{
			strCodecName="g7221";
			nSampleRate=16000;
			nBitrate=32000;
			nPayloadType=122;
			nSamplesPerFrame=320;
		}
		break;


	case X_AUDIO_CODEC_SIREN_G7221_14_24:
	case X_AUDIO_CODEC_G7221_14_24://32khz
		{
			strCodecName="g7221";
			nSampleRate=32000;
			nBitrate=24000;
			nPayloadType=123;
			nSamplesPerFrame=640;
		}
		break;

	case X_AUDIO_CODEC_SIREN_G7221_14_32:
	case X_AUDIO_CODEC_G7221_14_32:
		{
			strCodecName="g7221";
			nSampleRate=32000;
			nBitrate=32000;
			nPayloadType=124;
			nSamplesPerFrame=640;
		}
		break;
	case X_AUDIO_CODEC_SIREN_G7221_14_48:
	case X_AUDIO_CODEC_G7221_14_48:
		{
			strCodecName = "g7221";
			nSampleRate = 32000;
			nBitrate = 48000;
			nPayloadType = 125;
			nSamplesPerFrame = 640;
		}
		break;
	case X_AUDIO_CODEC_OPUS_FWB_48:
		{
			strCodecName = "opus";
			nSampleRate = 48000;
			nBitrate = 64000;
			nPayloadType = 120;
			nSamplesPerFrame = 960;
		}
		break;


	default:
		return -1;
	};


	int nHeaderLen=AUDEC_HEADER_GET_LEN(pAudecData);
	int nPayloadLen=nAudecLen-nHeaderLen;
	if (nPayloadLen<=0 || (nPayloadLen+RTP_HEADER_LEN)>1500)
	{
		return -1;
	}

	unsigned short nSeqNo=AUDEC_HEADER_GET_SEQUENCE(pAudecData);

	PRTP_HEADER pRTPHeader=(PRTP_HEADER)pRTPData;
	memset(pRTPHeader,0,RTP_HEADER_LEN);
	pRTPHeader->version=RTP_VERSION;
	pRTPHeader->type=nPayloadType;
	pRTPHeader->marker=AUDEC_HEADER_GET_SYN_POINT(pAudecData);
	pRTPHeader->timestamp=htonl(m_nRTPTimestamp);
	pRTPHeader->ssrc=htonl((unsigned long)this);
	pRTPHeader->sequence=htons(m_nRTPSeqNo++);
	m_nRTPTimestamp+=nSamplesPerFrame;

	memcpy(pRTPData+RTP_HEADER_LEN,(unsigned char*)pAudecData+nHeaderLen,nPayloadLen);

	return nPayloadLen+RTP_HEADER_LEN;
}