#include "../IMXEC_Win32/stdafx.h"
#include "MediaCall.h"
#include "RTPHeader.h"
#include "CallMgr.h"
#include "../IMXEC_Util/XUtil.h"
#include "../IMXEC_MediaChannel/PlayChan.h"
//added by jiangdingfeng 2014-05-19
#include "VIDEC/VIDEC_Screen.h"

//notice test
#ifdef HAS_H323
#include <RVINC/cm.h>
#endif

#ifndef HAS_H323
typedef  void *  HPVT ;
typedef  void *  HPST ;
typedef  void *  HCALL ;
#endif

//Add WebRTC by zys 20160929
#pragma comment(lib,"Strmiids.lib") 
#pragma comment(lib,"Dmoguids.lib")  
#pragma comment(lib,"wmcodecdspuuid.lib") 
#pragma comment(lib,"Msdmo.lib")

#define ObjectClose(type , a) \
{ \
	type temp = a ; \
	a = NULL; \
	if (temp != NULL) \
{ \
	temp->Close(); \
	delete temp; \
	temp = NULL; \
} \
}

//notice test
extern void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info);

MediaCall::MediaCall(void)
: m_pLocalAudioCapChan(NULL)
, m_pLocalAudioCapChanBuf(NULL)
, m_nLocalAudioCapChanBufLen(0)
, m_ulLocalAudioCapChanRTPTimestamp(0)
, m_nLocalAudioCapChanRTPPayloadType(0)
, m_pAudioMixer(NULL)
, m_pExcludePlayChan(NULL)
, m_pWebRTCAudio(NULL)
, m_nWebRTCPlayChan(-1)

, m_nLocalAudioCodecID(X_AUDIO_CODEC_G711U)
, m_nLocalAudioMute(0)
, m_nLocalAudioVolume(50)

, m_pRemoteAudioPlayChan(NULL)
, m_nRemoteAudioPlayChanCodecID(X_AUDIO_CODEC_G711U)
, m_pRemoteAudioPlayChanBuf(NULL)
, m_nRemoteAudioPlayChanBufLen(0)
, m_nRemoteAudioMute(0)
, m_nRemoteAudioVolume(50)
, m_nRemoteAudioPayloadType(0)

, m_LocalMainCapChanRTP(*this)
, m_LocalSecondCapChanRTP(*this)
, m_RemoteMainPlayChanRTP(*this)
, m_RemoteSecondPlayChanRTP(*this)
{
	m_hLocalMainVideoWnd=NULL;
	m_pLocalMainVideoWnd=NULL;
	m_pLocalMainVideoCapDev=NULL;
	m_pLocalMainVideoCapChan=NULL;
	m_nLocalMainVideoCodecType=VIDEC_CODEC_H264;

	m_nLocalMainVideoBrightness=VIDEC_GetDefaultBrightness();
	m_nLocalMainVideoContrast=VIDEC_GetDefaultContrast();
	m_nLocalMainVideoSaturation=VIDEC_GetDefaultSaturation();
	m_nLocalMainVideoHue=VIDEC_GetDefaultHue();
	m_nLocalMainVideoGamma=VIDEC_GetDefaultGamma();
	m_nLocalMainVideoSharpen=VIDEC_GetDefaultSharpen();
	m_bLocalMainVideoSharpenLocalOnly=true;

	m_nLocalMainVideoEnableOSD=0;
	m_strLocalMainVideoOSD="";
	m_nLocalMainVideoAlign=0;
	ZeroMemory(&m_fontLocalMainVideoOSD, sizeof(m_fontLocalMainVideoOSD));	
	m_fontLocalMainVideoOSD.lfWeight = FW_HEAVY;//FW_BOLD;
	strcpy(m_fontLocalMainVideoOSD.lfFaceName, "Arial");
	m_fontLocalMainVideoOSD.lfHeight = -16;
	m_fontLocalMainVideoOSD.lfQuality = NONANTIALIASED_QUALITY;
	m_crLocalMainVideoText=(RGB(0,0,255));
	m_crLocalMainVideoBk=(RGB(255,255,255));

	m_hRemoteMainVideoWnd=NULL;
	m_pRemoteMainVideoWnd=NULL;
	m_pRemoteMainVideoPlayChan=NULL;
	m_nRemoteMainVideoCodecType=VIDEC_CODEC_H264;

	m_nRemoteMainVideoBrightness=VIDEC_GetDefaultBrightness();
	m_nRemoteMainVideoContrast=VIDEC_GetDefaultContrast();
	m_nRemoteMainVideoSaturation=VIDEC_GetDefaultSaturation();
	m_nRemoteMainVideoHue=VIDEC_GetDefaultHue();
	m_nRemoteMainVideoGamma=VIDEC_GetDefaultGamma();
	m_nRemoteMainVideoSharpen=VIDEC_GetDefaultSharpen();

	m_hSecondVideoWnd=NULL;
	m_pSecondVideoCapDev=NULL;
	m_pSecondVideoCapChan=NULL;
	m_pSecondVideoWnd=NULL;
	m_pSecondVideoPlayChan=NULL;
	m_pSecondVideoTranscoder=NULL;
	m_pMainVideoTranscoder=NULL;
	m_nSecondVideoCodecType=VIDEC_CODEC_H264;

	m_nSecondVideoBrightness=VIDEC_GetDefaultBrightness();
	m_nSecondVideoContrast=VIDEC_GetDefaultContrast();
	m_nSecondVideoSaturation=VIDEC_GetDefaultSaturation();
	m_nSecondVideoHue=VIDEC_GetDefaultHue();
	m_nSecondVideoGamma=VIDEC_GetDefaultGamma();
	m_nSecondVideoSharpen=VIDEC_GetDefaultSharpen();
	m_bSecondVideoSharpenLocalOnly=true;

	m_nSecondVideoEnableOSD=0;
	m_strSecondVideoOSD="";
	m_nSecondVideoAlign=0;
	ZeroMemory(&m_fontSecondVideoOSD, sizeof(m_fontSecondVideoOSD));	
	m_fontSecondVideoOSD.lfWeight = FW_HEAVY;//FW_BOLD;
	strcpy(m_fontSecondVideoOSD.lfFaceName, "Tahoma");
	m_fontSecondVideoOSD.lfHeight = -16;
	m_fontSecondVideoOSD.lfQuality = NONANTIALIASED_QUALITY;
	m_crSecondVideoText=(RGB(0,0,255));
	m_crSecondVideoBk=(RGB(255,255,255));

	m_strSecondRecordPathName="";
	m_nSecondRecordIndex=0;

	m_nMainVideoScreenIndex=-1;
	m_nSecondVideoScreenIndex=-1;
	
	m_pMainVideoMPScreen=NULL;
	m_pSecondVideoMPScreen=NULL;

	m_pLocalRecordCallback=NULL;
	m_pRemoteRecordCallback=NULL;
	m_pSecondRecordCallback=NULL;


	m_pMediaSenderRemoteMain=NULL;
	m_pMediaSenderRemoteSecond=NULL;
	m_pRTP2FrameRemoteMain=NULL;
	m_pRTP2FrameRemoteSecond=NULL;
	m_nSeqNoRemoteMain=0;
	m_nSeqNoRemoteSecond=0;

	m_pMediaSenderLocalMain=NULL;
	m_pRTP2FrameLocalMain=NULL;
	m_nSeqNoLocalMain=0;

	m_pMediaSenderLocalSecond=NULL;
	m_pRTP2FrameLocalSecond=NULL;
	m_nSeqNoLocalSecond=0;

	m_pRTPJBufMainVideo=NULL;
	m_pRTPJBufSecondVideo=NULL;
	m_pRTPJBufAudio=NULL;

	m_bRecording=false;

	m_pRemoteMainPlayChan=NULL;
	m_pRemoteSecondPlayChan=NULL;

	IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
	CallMgr::Instance().GetCallType(nCallType);
	if (nCallType==IMXEC_CONFERENCE_CALL ||
		nCallType==IMXEC_MP_CALL ||
		nCallType==IMXEC_MCU_CALL)
	{
		m_pMediaSenderRemoteMain=NETEC_MediaSender::Create(*this);
		if (m_pMediaSenderRemoteMain!=NULL)
		{
			if (0!=m_pMediaSenderRemoteMain->Open())
			{
				m_pMediaSenderRemoteMain->Close();
				delete m_pMediaSenderRemoteMain;
				m_pMediaSenderRemoteMain=NULL;
			}
		}
		m_pMediaSenderRemoteSecond=NETEC_MediaSender::Create(*this);
		if (m_pMediaSenderRemoteSecond!=NULL)
		{
			if (0!=m_pMediaSenderRemoteSecond->Open())
			{
				m_pMediaSenderRemoteSecond->Close();
				delete m_pMediaSenderRemoteSecond;
				m_pMediaSenderRemoteSecond=NULL;
			}
		}
	}

	m_pMediaSenderLocalMain=NETEC_MediaSender::Create(*this);
	if (m_pMediaSenderLocalMain!=NULL)
	{
		if (0!=m_pMediaSenderLocalMain->Open())
		{
			m_pMediaSenderLocalMain->Close();
			delete m_pMediaSenderLocalMain;
			m_pMediaSenderLocalMain=NULL;
		}
	}

	m_pMediaSenderLocalSecond=NETEC_MediaSender::Create(*this);
	if (m_pMediaSenderLocalSecond!=NULL)
	{
		if (0!=m_pMediaSenderLocalSecond->Open())
		{
			m_pMediaSenderLocalSecond->Close();
			delete m_pMediaSenderLocalSecond;
			m_pMediaSenderLocalSecond=NULL;
		}
	}

	//FlowControlCommand 
	m_nMainVideoFlowCCBitRate =1;
	m_nDualVideoFlowCCBitRate =1;
	m_nAudioFlowCCBitRate =1;

	//WebRTC
	m_nRTPTimestamp = 0;
	m_nRTPSeqNo = 0;
	m_nAudioSeqNo = 0;
}

MediaCall::~MediaCall(void)
{

}

void MediaCall::Close(void)
{
	SetLocalRecordCallback(NULL);
	SetRemoteRecordCallback(NULL);
	SetSecondRecordCallback(NULL);

	ObjectClose(IMXEC_PlayChan* ,m_pRemoteMainPlayChan)
	//if (m_pRemoteMainPlayChan)
	//{
	//	m_pRemoteMainPlayChan->Close();
	//	delete m_pRemoteMainPlayChan;
	//	m_pRemoteMainPlayChan=NULL;
	//}

	ObjectClose(IMXEC_PlayChan* ,m_pRemoteSecondPlayChan)

	CloseLocalAudioCapDev();
	CloseRemoteAudioPlayChan();

	ObjectClose(VIDEC_Wnd* ,m_pLocalMainVideoWnd)
	//if (m_pLocalMainVideoWnd!=NULL)
	//{
	//	m_pLocalMainVideoWnd->Close();
	//	delete m_pLocalMainVideoWnd;
	//	m_pLocalMainVideoWnd=NULL;
	//}

	CloseLocalMainVideoCapDev();

	ObjectClose(VIDEC_Wnd* ,m_pRemoteMainVideoWnd)
	//if (m_pRemoteMainVideoWnd!=NULL)
	//{
	//	m_pRemoteMainVideoWnd->Close();
	//	delete m_pRemoteMainVideoWnd;
	//	m_pRemoteMainVideoWnd=NULL;
	//}

	CloseRemoteMainVideoPlayChan();

	ObjectClose(VIDEC_Wnd* ,m_pSecondVideoWnd)
	//if (m_pSecondVideoWnd!=NULL)
	//{
	//	m_pSecondVideoWnd->Close();
	//	delete m_pSecondVideoWnd;
	//	m_pSecondVideoWnd=NULL;
	//}

	CloseLocalSecondVideoCapDev();
	CloseRemoteSecondVideoPlayChan();

	ObjectClose(NETEC_MediaSender* ,m_pMediaSenderRemoteMain)
	//if (m_pMediaSenderRemoteMain!=NULL)
	//{
	//	m_pMediaSenderRemoteMain->Close();
	//	delete m_pMediaSenderRemoteMain;
	//	m_pMediaSenderRemoteMain=NULL;
	//}

	ObjectClose(NETEC_MediaSender* ,m_pMediaSenderRemoteSecond)
	//if (m_pMediaSenderRemoteSecond!=NULL)
	//{
	//	m_pMediaSenderRemoteSecond->Close();
	//	delete m_pMediaSenderRemoteSecond;
	//	m_pMediaSenderRemoteSecond=NULL;
	//}

	ObjectClose(NETEC_MediaSender* ,m_pMediaSenderLocalMain)
	//if (m_pMediaSenderLocalMain!=NULL)
	//{
	//	m_pMediaSenderLocalMain->Close();
	//	delete m_pMediaSenderLocalMain;
	//	m_pMediaSenderLocalMain=NULL;
	//}

	ObjectClose(NETEC_MediaSender* ,m_pMediaSenderLocalSecond)
}

/******************************************************************************
* SetLocalMainVideoWnd
*描述：设置本地主流视频显示的窗口
*输入：	hWnd			-本地主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::SetLocalMainVideoWnd(HWND hWnd)
{
	IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
	CallMgr::Instance().GetCallType(nCallType);
	if (nCallType!=IMXEC_PERSONAL_CALL)
		return -1;

	if (m_hLocalMainVideoWnd!=hWnd)
	{
		if (m_pLocalMainVideoWnd!=NULL)
		{
			m_pLocalMainVideoWnd->Close();
			delete m_pLocalMainVideoWnd;
			m_pLocalMainVideoWnd=NULL;
		}

		m_hLocalMainVideoWnd=hWnd;

		if (m_hLocalMainVideoWnd!=NULL)
		{
			if (m_hLocalMainVideoWnd!=NULL)
			{
				m_pLocalMainVideoWnd=VIDEC_Wnd::CreateLocalVideo(*this);
				if (m_pLocalMainVideoWnd!=NULL)
				{
					if (0!=m_pLocalMainVideoWnd->Open(m_hLocalMainVideoWnd,true,NULL,NULL))
					{
						m_pLocalMainVideoWnd->Close();
						delete m_pLocalMainVideoWnd;
						m_pLocalMainVideoWnd=NULL;
					}
					else
					{
						if (m_pLocalMainVideoCapDev!=NULL)
						{
							m_pLocalMainVideoCapDev->AddImageSink(m_pLocalMainVideoWnd);
						}

						CRect rc;
						::GetClientRect(m_hLocalMainVideoWnd,rc);
						m_pLocalMainVideoWnd->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);

						m_pLocalMainVideoWnd->SetVideoParam(VIDEC_GetDefaultBrightness(),VIDEC_GetDefaultContrast(),VIDEC_GetDefaultSaturation(),VIDEC_GetDefaultHue(),VIDEC_GetDefaultGamma(),(m_bLocalMainVideoSharpenLocalOnly?m_nLocalMainVideoSharpen:VIDEC_GetDefaultSharpen()));
					}
				}
			}
		}
	}

	return 0;
}

/******************************************************************************
* SetLocalMainVideoWndPos
*描述：设置本地主流视频显示的窗口位置
*输入：	x				-窗口的左边位置
		y				-窗口的上边位置
		cx				-窗口的宽度
		cy				-窗口的高度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::SetLocalMainVideoWndPos(int x,int y,int cx,int cy)
{
	if (m_pLocalMainVideoWnd!=NULL)
	{
		m_pLocalMainVideoWnd->SetWindowPos(NULL,x,y,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);
		return 0;
	}
	return -1;
}

/******************************************************************************
* SetLocalMainVideoParam
*描述：设置本地主流视频参数
*输入：	nBrightness		-亮度值0-255
		nContrast		-对比度0-127
		nSaturation		-饱和度0-127
		nHue			-色调0-255
		nGamma			-伽马1-400
		nSharpen		-锐化0-127
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetLocalMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	m_nLocalMainVideoBrightness=nBrightness;
	m_nLocalMainVideoContrast=nContrast;
	m_nLocalMainVideoSaturation=nSaturation;
	m_nLocalMainVideoHue=nHue;
	m_nLocalMainVideoGamma=nGamma;
	m_nLocalMainVideoSharpen=nSharpen;
	m_bLocalMainVideoSharpenLocalOnly=bSharpenLocalOnly;

	if (m_pLocalMainVideoWnd)
	{
		m_pLocalMainVideoWnd->SetVideoParam(VIDEC_GetDefaultBrightness(),VIDEC_GetDefaultContrast(),VIDEC_GetDefaultSaturation(),VIDEC_GetDefaultHue(),VIDEC_GetDefaultGamma(),(m_bLocalMainVideoSharpenLocalOnly?m_nLocalMainVideoSharpen:VIDEC_GetDefaultSharpen()));
	}

	if (m_bLocalMainVideoSharpenLocalOnly)
	{
		nSharpen=VIDEC_GetDefaultSharpen();
	}

	if (m_pLocalMainVideoCapDev)
	{
		VIDEC_AH400Dev*pAH400Dev=dynamic_cast<VIDEC_AH400Dev*>(m_pLocalMainVideoCapDev);
		if (pAH400Dev!=NULL)
		{
			pAH400Dev->SetVideoParam(m_nLocalMainVideoBrightness,m_nLocalMainVideoContrast,m_nLocalMainVideoSaturation,m_nLocalMainVideoHue,m_nLocalMainVideoGamma,nSharpen);
			return ;
		}
		VIDEC_AH2KDev*pAH2KDev=dynamic_cast<VIDEC_AH2KDev*>(m_pLocalMainVideoCapDev);
		if (pAH2KDev!=NULL)
		{
			pAH2KDev->SetVideoParam(m_nLocalMainVideoBrightness,m_nLocalMainVideoContrast,m_nLocalMainVideoSaturation,m_nLocalMainVideoHue,m_nLocalMainVideoGamma,nSharpen);
			return ;
		}
		VIDEC_CameraDev*pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pLocalMainVideoCapDev);
		if (pCameraDev!=NULL)
		{
			pCameraDev->SetVideoParam(m_nLocalMainVideoBrightness,m_nLocalMainVideoContrast,m_nLocalMainVideoSaturation,m_nLocalMainVideoHue,m_nLocalMainVideoGamma,nSharpen);
			return ;
		}
	}
}

/******************************************************************************
* SetLocalMainVideoEnableOSD
*描述：设置本地主流视频是否启用OSD
*输入：	cszCallID		-呼叫标识
		nEnable			-是否启用OSD
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetLocalMainVideoEnableOSD(int nEnable)
{
	m_nLocalMainVideoEnableOSD=nEnable;
	if (m_pLocalMainVideoCapDev!=NULL)
	{
		m_pLocalMainVideoCapDev->EnableOSD(m_nLocalMainVideoEnableOSD);
	}
}

/******************************************************************************
* SetLocalMainVideoOSDText
*描述：设置本地主流视频OSD文字
*输入：	cszCallID		-呼叫标识
		cszText			-OSD文字
		nAlign			-对齐方式
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetLocalMainVideoOSDText(const char*cszText,int nAlign)
{
	if (cszText!=NULL)
	{
		m_strLocalMainVideoOSD=cszText;
	}
	else
	{
		m_strLocalMainVideoOSD="";
	}

	m_nLocalMainVideoAlign=nAlign;
	if (m_pLocalMainVideoCapDev!=NULL)
	{
		m_pLocalMainVideoCapDev->SetOSDText(m_strLocalMainVideoOSD.c_str(),m_nLocalMainVideoAlign);
	}
}

/******************************************************************************
* SetLocalMainVideoOSDFont
*描述：设置本地主流视频OSD字体,字的颜色和背景色
*输入：	logfont			-OSD字体
		crText			-字的颜色
		crBk			-背景色
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetLocalMainVideoOSDFont(LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	m_fontLocalMainVideoOSD=logfont;
	m_crLocalMainVideoText=crText;
	m_crLocalMainVideoBk=crBk;

	if (m_pLocalMainVideoCapDev!=NULL)
	{
		m_pLocalMainVideoCapDev->SetOSDFont(m_fontLocalMainVideoOSD,m_crLocalMainVideoText,m_crLocalMainVideoBk);
	}
}


/******************************************************************************
* GetLocalMainVideoInfo
*描述：获取本地主流视频信息
*输入：	无
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetLocalMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd,int &nFrameRate)
{
	nVideoCodecType=m_nLocalMainVideoCodecType;
	if (m_pLocalMainVideoWnd!=NULL)
	{
		nWidth=m_pLocalMainVideoWnd->GetVideoWidth();
		if (nWidth<=0)
		{
			nWidth=0;
		}
		nHeight=m_pLocalMainVideoWnd->GetVideoHeight();
		if (nHeight<=0)
		{
			nHeight=0;
		}
		hVideoWnd=m_pLocalMainVideoWnd->GetWnd();
		nFrameRate =m_pLocalMainVideoWnd->GetVideoShowFrameRate();

		//char msg[128]={0};
		//sprintf(msg,"local video wnd framerate:%d\n",nFrameRate);
		//OutputDebugString(msg);

	}

	return 0;
}

/******************************************************************************
* GetLocalMainVideoPacketStatistics
*描述：获取本地主视频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetLocalMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	ulTotalPackets=0;
	ulLostPackets=0;
	AvgPacketLossRate=0;
	CurPacketLossRate=0;
	nBitrate=m_XBitrateLocalMainVideo.GetBitrate();

	return 0;
}

/******************************************************************************
* SetRemoteMainVideoWnd
*描述：设置远程主流视频显示的窗口
*输入：	无
		hWnd			-远程主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::SetRemoteMainVideoWnd(HWND hWnd)
{
	if (m_hRemoteMainVideoWnd!=hWnd)
	{
		if (m_pRemoteMainVideoWnd!=NULL)
		{
			m_pRemoteMainVideoWnd->Close();
			delete m_pRemoteMainVideoWnd;
			m_pRemoteMainVideoWnd=NULL;
		}

		m_hRemoteMainVideoWnd=hWnd;

		if (HasAVCONTransport())
		{
			return 0;
		}

		if (m_hRemoteMainVideoWnd!=NULL)
		{
			m_pRemoteMainVideoWnd=VIDEC_Wnd::Create(*this);
			if (m_pRemoteMainVideoWnd!=NULL)
			{
				if (0!=m_pRemoteMainVideoWnd->Open(m_hRemoteMainVideoWnd,true,NULL,NULL))
				{
					m_pRemoteMainVideoWnd->Close();
					delete m_pRemoteMainVideoWnd;
					m_pRemoteMainVideoWnd=NULL;
				}
				else 
				{
					CRect rc;
					::GetClientRect(m_hRemoteMainVideoWnd,rc);
					m_pRemoteMainVideoWnd->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);

					m_pRemoteMainVideoWnd->SetVideoParam(m_nRemoteMainVideoBrightness,m_nRemoteMainVideoContrast,m_nRemoteMainVideoSaturation,m_nRemoteMainVideoHue,m_nRemoteMainVideoGamma,m_nRemoteMainVideoSharpen);

					if (m_pRemoteMainVideoPlayChan!=NULL)
					{
						m_pRemoteMainVideoPlayChan->AddImageSink(m_pRemoteMainVideoWnd);
					}
				}
			}
		}
	}

	return 0;
}

/******************************************************************************
* SetRemoteMainVideoWndPos
*描述：设置远程主流视频显示的窗口位置
*输入：	x				-窗口的左边位置
		y				-窗口的上边位置
		cx				-窗口的宽度
		cy				-窗口的高度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::SetRemoteMainVideoWndPos(int x,int y,int cx,int cy)
{
	if (m_pRemoteMainPlayChan)
	{
		m_pRemoteMainPlayChan->SetWindowPos(NULL,x,y,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);
		return 0;
	}
	if (m_pRemoteMainVideoWnd!=NULL)
	{
		m_pRemoteMainVideoWnd->SetWindowPos(NULL,x,y,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);
		return 0;
	}
	return -1;
}

/******************************************************************************
* SetRemoteMainVideoParam
*描述：设置远程主流视频参数
*输入：	nBrightness		-亮度值0-255
		nContrast		-对比度0-127
		nSaturation		-饱和度0-127
		nHue			-色调0-255
		nGamma			-伽马1-400
		nSharpen		-锐化0-127
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetRemoteMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)
{
	m_nRemoteMainVideoBrightness=nBrightness;
	m_nRemoteMainVideoContrast=nContrast;
	m_nRemoteMainVideoSaturation=nSaturation;
	m_nRemoteMainVideoHue=nHue;
	m_nRemoteMainVideoGamma=nGamma;
	m_nRemoteMainVideoSharpen=nSharpen;

	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->SetVideoParam(m_nRemoteMainVideoBrightness,m_nRemoteMainVideoContrast,m_nRemoteMainVideoSaturation,m_nRemoteMainVideoHue,m_nRemoteMainVideoGamma,m_nRemoteMainVideoSharpen);
	}

	if (m_pRemoteMainVideoWnd!=NULL)
	{
		m_pRemoteMainVideoWnd->SetVideoParam(m_nRemoteMainVideoBrightness,m_nRemoteMainVideoContrast,m_nRemoteMainVideoSaturation,m_nRemoteMainVideoHue,m_nRemoteMainVideoGamma,m_nRemoteMainVideoSharpen);
	}
}


/******************************************************************************
* GetRemoteMainVideoInfo
*描述：获取远程主流视频信息
*输入：	无
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetRemoteMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd,int &nFrameRate)
{
	nVideoCodecType=m_nRemoteMainVideoCodecType;

	if (m_pRemoteMainPlayChan!=NULL)
	{
		nWidth=m_pRemoteMainPlayChan->GetVideoWidth();
		if (nWidth<=0)
		{
			nWidth=0;
		}
		nHeight=m_pRemoteMainPlayChan->GetVideoHeight();
		if (nHeight<=0)
		{
			nHeight=0;
		}
		hVideoWnd=m_pRemoteMainPlayChan->GetVideoWnd();
		return 0;
	}

	if (m_pRemoteMainVideoWnd!=NULL)
	{
		nWidth=m_pRemoteMainVideoWnd->GetVideoWidth();
		if (nWidth<=0)
		{
			nWidth=0;
		}
		nHeight=m_pRemoteMainVideoWnd->GetVideoHeight();
		if (nHeight<=0)
		{
			nHeight=0;
		}
		hVideoWnd=m_pRemoteMainVideoWnd->GetWnd();
		nFrameRate =m_pRemoteMainVideoWnd->GetVideoShowFrameRate();
	}
	return 0;
}

/******************************************************************************
* GetRemoteMainVideoPacketStatistics
*描述：获取远端主视频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetRemoteMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	ulTotalPackets=0;
	ulLostPackets=0;
	AvgPacketLossRate=0;
	CurPacketLossRate=0;

	if (m_pRemoteMainPlayChan)
	{
		m_pRemoteMainPlayChan->GetVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	else
		nBitrate=m_XBitrateRemoteMainVideo.GetBitrate();

	////信息栏显示有视频丢包、音频丢包，且丢包数上升很快 暂不启用
	//ulLostPackets=0;
	//CurPacketLossRate=0;
	return 0;
}


/******************************************************************************
* SetSecondVideoWnd
*描述：设置辅流视频显示的窗口
*输入：无
		hWnd			-远程辅流视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::SetSecondVideoWnd(HWND hWnd)
{
	if (m_hSecondVideoWnd!=hWnd)
	{
		if (m_pSecondVideoWnd!=NULL)
		{
			m_pSecondVideoWnd->Close();
			delete m_pSecondVideoWnd;
			m_pSecondVideoWnd=NULL;
		}

		m_hSecondVideoWnd=hWnd;

		if (HasAVCONTransport() /*&& m_pSecondVideoCapDev==NULL*/)
		{
			return 0;
		}

		if (m_hSecondVideoWnd!=NULL)
		{
			m_pSecondVideoWnd=VIDEC_Wnd::Create(*this);
			if (m_pSecondVideoWnd!=NULL)
			{
				if (0!=m_pSecondVideoWnd->Open(m_hSecondVideoWnd,true,NULL,NULL))
				{
					m_pSecondVideoWnd->Close();
					delete m_pSecondVideoWnd;
					m_pSecondVideoWnd=NULL;
				}
				else 
				{

					CRect rc;
					::GetClientRect(m_hSecondVideoWnd,rc);
					m_pSecondVideoWnd->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
					m_pSecondVideoWnd->SetVideoParam(VIDEC_GetDefaultBrightness(),VIDEC_GetDefaultContrast(),VIDEC_GetDefaultSaturation(),VIDEC_GetDefaultHue(),VIDEC_GetDefaultGamma(),(m_bSecondVideoSharpenLocalOnly?m_nSecondVideoSharpen:VIDEC_GetDefaultSharpen()));

					if (m_pSecondVideoCapDev!=NULL)
					{
						m_pSecondVideoCapDev->AddImageSink(m_pSecondVideoWnd);
					}
					else if (m_pSecondVideoPlayChan!=NULL)
					{
						m_pSecondVideoPlayChan->AddImageSink(m_pSecondVideoWnd);
					}
				}
			}
		}
	}

	return 0;
}

/******************************************************************************
* SetSecondVideoWndPos
*描述：设置辅流视频显示的窗口位置
*输入：	x				-窗口的左边位置
		y				-窗口的上边位置
		cx				-窗口的宽度
		cy				-窗口的高度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::SetSecondVideoWndPos(int x,int y,int cx,int cy)
{
	if (m_pRemoteSecondPlayChan)
	{
		m_pRemoteSecondPlayChan->SetWindowPos(NULL,x,y,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);
		return 0;
	}
	if (m_pSecondVideoWnd!=NULL)
	{
		m_pSecondVideoWnd->SetWindowPos(NULL,x,y,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);
		return 0;
	}
	return -1;
}

/******************************************************************************
* SetSecondVideoParam
*描述：设置辅流视频参数
*输入：	nBrightness		-亮度值0-255
		nContrast		-对比度0-127
		nSaturation		-饱和度0-127
		nHue			-色调0-255
		nGamma			-伽马1-400
		nSharpen		-锐化0-127
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetSecondVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	m_nSecondVideoBrightness=nBrightness;
	m_nSecondVideoContrast=nContrast;
	m_nSecondVideoSaturation=nSaturation;
	m_nSecondVideoHue=nHue;
	m_nSecondVideoGamma=nGamma;
	m_nSecondVideoSharpen=nSharpen;
	m_bSecondVideoSharpenLocalOnly=bSharpenLocalOnly;

	if (m_pSecondVideoWnd)
	{
		m_pSecondVideoWnd->SetVideoParam(VIDEC_GetDefaultBrightness(),VIDEC_GetDefaultContrast(),VIDEC_GetDefaultSaturation(),VIDEC_GetDefaultHue(),VIDEC_GetDefaultGamma(),(m_bSecondVideoSharpenLocalOnly?m_nSecondVideoSharpen:VIDEC_GetDefaultSharpen()));
	}

	if (m_bSecondVideoSharpenLocalOnly)
	{
		nSharpen=VIDEC_GetDefaultSharpen();
	}

	if (m_pSecondVideoCapDev)
	{
		VIDEC_AH400Dev*pAH400Dev=dynamic_cast<VIDEC_AH400Dev*>(m_pSecondVideoCapDev);
		if (pAH400Dev!=NULL)
		{
			pAH400Dev->SetVideoParam(m_nSecondVideoBrightness,m_nSecondVideoContrast,m_nSecondVideoSaturation,m_nSecondVideoHue,m_nSecondVideoGamma,nSharpen);
			return ;
		}
		VIDEC_AH2KDev*pAH2KDev=dynamic_cast<VIDEC_AH2KDev*>(m_pSecondVideoCapDev);
		if (pAH2KDev!=NULL)
		{
			pAH2KDev->SetVideoParam(m_nSecondVideoBrightness,m_nSecondVideoContrast,m_nSecondVideoSaturation,m_nSecondVideoHue,m_nSecondVideoGamma,nSharpen);
			return ;
		}
		VIDEC_CameraDev*pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pSecondVideoCapDev);
		if (pCameraDev!=NULL)
		{
			pCameraDev->SetVideoParam(m_nSecondVideoBrightness,m_nSecondVideoContrast,m_nSecondVideoSaturation,m_nSecondVideoHue,m_nSecondVideoGamma,nSharpen);
			return ;
		}
	}
}

/******************************************************************************
* SetSecondVideoEnableOSD
*描述：设置辅流视频是否启用OSD
*输入：	nEnable			-是否启用OSD
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetSecondVideoEnableOSD(int nEnable)
{
	m_nSecondVideoEnableOSD=nEnable;
	if (m_pSecondVideoCapDev!=NULL)
	{
		m_pSecondVideoCapDev->EnableOSD(m_nSecondVideoEnableOSD);
	}
}

/******************************************************************************
* SetSecondVideoOSDText
*描述：设置辅流视频OSD文字
*输入：	cszText			-OSD文字
		nAlign			-对齐方式
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetSecondVideoOSDText(const char*cszText,int nAlign)
{
	if (cszText!=NULL)
	{
		m_strSecondVideoOSD=cszText;
	}
	else
	{
		m_strSecondVideoOSD="";
	}

	m_nSecondVideoAlign=nAlign;
	if (m_pSecondVideoCapDev!=NULL)
	{
		m_pSecondVideoCapDev->SetOSDText(m_strSecondVideoOSD.c_str(),m_nSecondVideoAlign);
	}
}

/******************************************************************************
* SetSecondVideoOSDFont
*描述：设置辅流视频OSD字体,字的颜色和背景色
*输入：	logfont			-OSD字体
		crText			-字的颜色
		crBk			-背景色
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetSecondVideoOSDFont(LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	m_fontSecondVideoOSD=logfont;
	m_crSecondVideoText=crText;
	m_crSecondVideoBk=crBk;

	if (m_pSecondVideoCapDev!=NULL)
	{
		m_pSecondVideoCapDev->SetOSDFont(m_fontSecondVideoOSD,m_crSecondVideoText,m_crSecondVideoBk);
	}
}

/******************************************************************************
* GetSecondVideoInfo
*描述：获取辅流视频信息
*输入：	无
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetSecondVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)
{
	nVideoCodecType=m_nSecondVideoCodecType;

	if (m_pRemoteSecondPlayChan!=NULL)
	{
		nWidth=m_pRemoteSecondPlayChan->GetVideoWidth();
		if (nWidth<=0)
		{
			nWidth=0;
		}
		nHeight=m_pRemoteSecondPlayChan->GetVideoHeight();
		if (nHeight<=0)
		{
			nHeight=0;
		}
		hVideoWnd=m_pRemoteSecondPlayChan->GetVideoWnd();
		return 0;
	}

	if (m_pSecondVideoWnd!=NULL)
	{
		nWidth=m_pSecondVideoWnd->GetVideoWidth();
		if (nWidth<=0)
		{
			nWidth=0;
		}
		nHeight=m_pSecondVideoWnd->GetVideoHeight();
		if (nHeight<=0)
		{
			nHeight=0;
		}
		hVideoWnd=m_pSecondVideoWnd->GetWnd();
	}

	return 0;
}

/******************************************************************************
* GetSecondVideoPacketStatistics
*描述：获取辅流视频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetSecondVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	ulTotalPackets=0;
	ulLostPackets=0;
	AvgPacketLossRate=0;
	CurPacketLossRate=0;
	if (m_pRemoteSecondPlayChan)
	{
		m_pRemoteSecondPlayChan->GetVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	else
		nBitrate=m_XBitrateSecondVideo.GetBitrate();

	////信息栏显示有视频丢包、音频丢包，且丢包数上升很快 暂不启用
	//ulLostPackets=0;
	//CurPacketLossRate=0;
	return 0;
}

int MediaCall::OpenLocalMainVideoCapDev(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,int nNetworkType)
{
	m_nLocalMainVideoCodecType=codecType;

	if (m_pRTP2FrameLocalMain==NULL)
	{
		switch (codecType)
		{
		case VIDEC_CODEC_H263://H.263
			m_pRTP2FrameLocalMain=NETEC_RTP2Frame::Create(*this,"H263");
			break;
		case VIDEC_CODEC_H263P://H.263P
			m_pRTP2FrameLocalMain=NETEC_RTP2Frame::Create(*this,"H263-1998");
			break;
		case VIDEC_CODEC_H263PP://H.263PP
			m_pRTP2FrameLocalMain=NETEC_RTP2Frame::Create(*this,"H263-2000");
			break;
			//notice 20120116 update
		case VIDEC_CODEC_H264://H.264
		case VIDEC_CODEC_H264_SVC://H.264
			m_pRTP2FrameLocalMain=NETEC_RTP2Frame::Create(*this,"H264");
			break;
		case VIDEC_CODEC_MPEG4://MPEG4 ,use to call kedacom
			m_pRTP2FrameLocalMain=NETEC_RTP2Frame::Create(*this,"MPEG4");
			break;
		}

		if (m_pRTP2FrameLocalMain!=0)
		{
			if (0!=m_pRTP2FrameLocalMain->Open(VIDEC_HEADER_EXT_KEY_FRAME_LEN))
			{
				m_pRTP2FrameLocalMain->Close();
				delete m_pRTP2FrameLocalMain;
				m_pRTP2FrameLocalMain=NULL;
			}
			else
			{
				if (m_pMediaSenderLocalMain!=NULL)
				{
					m_pMediaSenderLocalMain->StartVideo(0);
				}
			}
		}
	}

	IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
	CallMgr::Instance().GetCallType(nCallType);
	if (nCallType==IMXEC_PERSONAL_CALL)
	{
		if (nDevID<0)
		{
			CloseLocalMainVideoCapDev();
			return -1;
		}

		VIDEC_CapDev*pOldCapDev=m_pLocalMainVideoCapDev;
		while (1)
		{
			int nTempDevCnt=VIDEC_AH2KDev::GetDevCount();
			if (nDevID < nTempDevCnt)
			{
				if (nDevID>=0)
				{
					VIDEC_AH2KDev*pAH2KDev=NULL;
					if (m_pLocalMainVideoCapDev!=NULL)
					{
						pAH2KDev=dynamic_cast<VIDEC_AH2KDev*>(m_pLocalMainVideoCapDev);
					}
					if (pAH2KDev==NULL)
					{
						if (m_pLocalMainVideoCapDev!=NULL)
						{
							m_pLocalMainVideoCapDev->Close();
							delete m_pLocalMainVideoCapDev;
							m_pLocalMainVideoCapDev=NULL;

							pOldCapDev=NULL;
						}

						pAH2KDev=VIDEC_AH2KDev::Create();
						if (pAH2KDev!=NULL)
						{
							if (0!=pAH2KDev->Open(nDevID,nWidth,nHeight,nFrameRate))
							{
								pAH2KDev->Close();
								delete pAH2KDev;
								pAH2KDev=NULL;
							}
							else
							{
								m_pLocalMainVideoCapDev=pAH2KDev;
							}
						}
					}
					else
					{
						pAH2KDev->Reset(nDevID,nWidth,nHeight,nFrameRate);
					}

					break ;
				}
			}

			nDevID-=nTempDevCnt;
			nTempDevCnt=VIDEC_CameraDev::GetDevCount();
			if (nDevID < nTempDevCnt)
			{
				if (nDevID>=0)
				{
					VIDEC_CameraDev*pCameraDev=NULL;
					if (m_pLocalMainVideoCapDev!=NULL)
					{
						pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pLocalMainVideoCapDev);
					}
					if (pCameraDev==NULL)
					{
						if (m_pLocalMainVideoCapDev!=NULL)
						{
							m_pLocalMainVideoCapDev->Close();
							delete m_pLocalMainVideoCapDev;
							m_pLocalMainVideoCapDev=NULL;

							pOldCapDev=NULL;
						}

						pCameraDev=VIDEC_CameraDev::Create();
						if (pCameraDev!=NULL)
						{
							if (0!=pCameraDev->Open(nDevID,nWidth,nHeight,nFrameRate))
							{
								pCameraDev->Close();
								delete pCameraDev;
								pCameraDev=NULL;
							}
							else
							{
								m_pLocalMainVideoCapDev=pCameraDev;
							}
						}
					}
					else
					{
						pCameraDev->Reset(nDevID,nWidth,nHeight,nFrameRate);
					}
					break ;
				}
			}

			nDevID-=nTempDevCnt;
			nTempDevCnt=VIDEC_AH400Dev::GetDevCount();
			if (nDevID < nTempDevCnt)
			{
				if (nDevID>=0)
				{
					VIDEC_AH400Dev*pAH400Dev=NULL;
					if (m_pLocalMainVideoCapDev!=NULL)
					{
						pAH400Dev=dynamic_cast<VIDEC_AH400Dev*>(m_pLocalMainVideoCapDev);
					}
					if (pAH400Dev==NULL)
					{
						if (m_pLocalMainVideoCapDev!=NULL)
						{
							m_pLocalMainVideoCapDev->Close();
							delete m_pLocalMainVideoCapDev;
							m_pLocalMainVideoCapDev=NULL;

							pOldCapDev=NULL;
						}

						pAH400Dev=VIDEC_AH400Dev::Create();
						if (pAH400Dev!=NULL)
						{
							if (0!=pAH400Dev->Open(nDevID,nWidth,nHeight,nFrameRate))
							{
								pAH400Dev->Close();
								delete pAH400Dev;
								pAH400Dev=NULL;
							}
							else
							{
								m_pLocalMainVideoCapDev=pAH400Dev;
							}
						}
					}
					else
					{
						pAH400Dev->Reset(nDevID,nWidth,nHeight,nFrameRate);
					}
					break ;
				}
			}

			break;
		}

		if (pOldCapDev!=m_pLocalMainVideoCapDev && 
			m_pLocalMainVideoCapDev!=NULL)
		{
			//change source set OSD by gao modify
			//m_pLocalMainVideoCapDev->EnableOSD(m_nLocalMainVideoEnableOSD);
			//m_pLocalMainVideoCapDev->SetOSDFont(m_fontLocalMainVideoOSD,m_crLocalMainVideoText,m_crLocalMainVideoBk);
			//m_pLocalMainVideoCapDev->SetOSDText(m_strLocalMainVideoOSD.c_str(),m_nLocalMainVideoAlign);

			int nSharpen=m_nLocalMainVideoSharpen;
			if (m_bLocalMainVideoSharpenLocalOnly)
			{
				nSharpen=VIDEC_GetDefaultSharpen();
			}

			if (m_pLocalMainVideoCapDev)
			{
				VIDEC_AH400Dev*pAH400Dev=dynamic_cast<VIDEC_AH400Dev*>(m_pLocalMainVideoCapDev);
				if (pAH400Dev!=NULL)
				{
					pAH400Dev->SetVideoParam(m_nLocalMainVideoBrightness,m_nLocalMainVideoContrast,m_nLocalMainVideoSaturation,m_nLocalMainVideoHue,m_nLocalMainVideoGamma,nSharpen);
				}
				else
				{
					VIDEC_AH2KDev*pAH2KDev=dynamic_cast<VIDEC_AH2KDev*>(m_pLocalMainVideoCapDev);
					if (pAH2KDev!=NULL)
					{
						pAH2KDev->SetVideoParam(m_nLocalMainVideoBrightness,m_nLocalMainVideoContrast,m_nLocalMainVideoSaturation,m_nLocalMainVideoHue,m_nLocalMainVideoGamma,nSharpen);
					}
					else
					{
						VIDEC_CameraDev*pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pLocalMainVideoCapDev);
						if (pCameraDev!=NULL)
						{
							pCameraDev->SetVideoParam(m_nLocalMainVideoBrightness,m_nLocalMainVideoContrast,m_nLocalMainVideoSaturation,m_nLocalMainVideoHue,m_nLocalMainVideoGamma,nSharpen);
						}
					}
				}
			}

			if (m_pLocalMainVideoWnd!=NULL)
			{
				m_pLocalMainVideoCapDev->AddImageSink(m_pLocalMainVideoWnd);
			}
			//add A7
			if (m_pLocalMainVideoCapChan!=NULL)
			{
				m_pLocalMainVideoCapDev->AddImageSink(m_pLocalMainVideoCapChan);
			}
		}

		if (m_pLocalMainVideoCapDev!=NULL)
		{
			if (m_pLocalMainVideoCapChan==NULL)
			{
				switch (codecType)
				{
				case VIDEC_CODEC_H261://H.261
					nPayloadType=31;
					break;
				case VIDEC_CODEC_H263://H.263
					nPayloadType=34;
					break;
				case VIDEC_CODEC_H263P://H.263
					if (nPayloadType<96)
					{
						nPayloadType=107;
					}
					break;
				case VIDEC_CODEC_H263PP://H.263
					if (nPayloadType<96)
					{
						nPayloadType=108;
					}
					break;
					//notice 20120116 update
				case VIDEC_CODEC_H264://H.264
				case VIDEC_CODEC_H264_SVC://H.264
				case VIDEC_CODEC_MPEG4://MPEG4 ,use to call kedacom
					if (nPayloadType<96)
					{
						nPayloadType=106;
					}
					break;
				}			


				VIDEC_CapChanRTP*pVideoCapChan=VIDEC_CapChanRTP::Create(m_LocalMainCapChanRTP);
				if (pVideoCapChan!=NULL)
				{
					if (0!=pVideoCapChan->Open(nPayloadType,nWidth,nHeight,nFrameRate,nBitrate,nNetworkType,codecType))
					{
						pVideoCapChan->Close();
						delete pVideoCapChan;
						pVideoCapChan=NULL;
					}
					else
					{
						m_pLocalMainVideoCapChan=pVideoCapChan;
						m_pLocalMainVideoCapChan->AddStreamSink(&m_LocalMainCapChanRTP);
						m_pLocalMainVideoCapDev->AddImageSink(m_pLocalMainVideoCapChan);
					}
				}
			}
		}
		//change source set OSD by gao add
		if (m_pLocalMainVideoCapDev!=NULL)
		{
			m_pLocalMainVideoCapDev->EnableOSD(m_nLocalMainVideoEnableOSD);
			m_pLocalMainVideoCapDev->SetOSDFont(m_fontLocalMainVideoOSD,m_crLocalMainVideoText,m_crLocalMainVideoBk);
			m_pLocalMainVideoCapDev->SetOSDText(m_strLocalMainVideoOSD.c_str(),m_nLocalMainVideoAlign);
		}
		return (m_pLocalMainVideoCapDev!=NULL?0:-1);
	}
	else if (nCallType==IMXEC_CONFERENCE_CALL || nCallType==IMXEC_MP_CALL)
	{
		if (m_pMainVideoMPScreen==NULL)
		{
			m_pMainVideoMPScreen=VIDEC_MPScreen::Create();
			if (m_pMainVideoMPScreen!=NULL)
			{
				if (0!=m_pMainVideoMPScreen->Open(nWidth,nHeight,nFrameRate))
				{
					m_pMainVideoMPScreen->Close();
					delete m_pMainVideoMPScreen;
					m_pMainVideoMPScreen=NULL;
				}
			}
		}

		if (m_pMainVideoMPScreen!=NULL)
		{
			if (m_nMainVideoScreenIndex>=0)
			{
				m_pMainVideoMPScreen->SetScreenIndex(m_nMainVideoScreenIndex);
			}

			if (m_pLocalMainVideoCapChan==NULL)
			{
				switch (codecType)
				{
				case VIDEC_CODEC_H261://H.261
					nPayloadType=31;
					break;
				case VIDEC_CODEC_H263://H.263
					nPayloadType=34;
					break;
				case VIDEC_CODEC_H263P://H.263
					if (nPayloadType<96)
					{
						nPayloadType=107;
					}
					break;
				case VIDEC_CODEC_H263PP://H.263
					if (nPayloadType<96)
					{
						nPayloadType=108;
					}
					break;
					//notice 20120116 update
				case VIDEC_CODEC_H264://H.264
				case VIDEC_CODEC_H264_SVC://H.264
				case VIDEC_CODEC_MPEG4://MPEG4 ,use to call kedacom
					if (nPayloadType<96)
					{
						nPayloadType=106;
					}
					break;
				}			


				VIDEC_CapChanRTP*pVideoCapChan=VIDEC_CapChanRTP::Create(m_LocalMainCapChanRTP);
				if (pVideoCapChan!=NULL)
				{
					if (0!=pVideoCapChan->Open(nPayloadType,nWidth,nHeight,nFrameRate,nBitrate,nNetworkType,codecType))
					{
						pVideoCapChan->Close();
						delete pVideoCapChan;
						pVideoCapChan=NULL;
					}
					else
					{
						m_pLocalMainVideoCapChan=pVideoCapChan;
						m_pLocalMainVideoCapChan->AddStreamSink(&m_LocalMainCapChanRTP);
						m_pMainVideoMPScreen->AddImageSink(m_pLocalMainVideoCapChan);
					}
				}
			}
		}

		return (m_pMainVideoMPScreen!=NULL?0:-1);
	}
	else if (nCallType==IMXEC_MCU_CALL)
	{
		return CallMgr::Instance().OpenLocalMainVideoCapDev(this,nWidth,nHeight,nFrameRate,codecType,nBitrate,nPayloadType);
	}


	return -1;
}

void MediaCall::OnRequestLocalMainVideoKeyFrame(void)
{
	if (m_pLocalMainVideoCapChan!=NULL)
	{
		m_pLocalMainVideoCapChan->RequestKeyFrame();
	}
	else
	{
		CallMgr::Instance().OnRequestLocalMainVideoKeyFrame(this);
	}
}

void MediaCall::CloseLocalMainVideoCapDev(void)
{
	CallMgr::Instance().CloseLocalMainVideoCapDev(this);

	ObjectClose(VIDEC_CapChanRTP* ,m_pLocalMainVideoCapChan)
	//if (m_pLocalMainVideoCapChan!=NULL)
	//{
	//	m_pLocalMainVideoCapChan->Close();
	//	delete m_pLocalMainVideoCapChan;
	//	m_pLocalMainVideoCapChan=NULL;
	//}

	ObjectClose(VIDEC_CapDev* ,m_pLocalMainVideoCapDev)
	//if (m_pLocalMainVideoCapDev!=NULL)
	//{
	//	m_pLocalMainVideoCapDev->Close();
	//	delete m_pLocalMainVideoCapDev;
	//	m_pLocalMainVideoCapDev=NULL;
	//}

	ObjectClose(VIDEC_MPScreen* ,m_pMainVideoMPScreen)
	//if (m_pMainVideoMPScreen!=NULL)
	//{
	//	m_pMainVideoMPScreen->Close();
	//	delete m_pMainVideoMPScreen;
	//	m_pMainVideoMPScreen=NULL;
	//}

	ObjectClose(NETEC_RTP2Frame* ,m_pRTP2FrameLocalMain)
	//if (m_pRTP2FrameLocalMain!=NULL)
	//{
	//	m_pRTP2FrameLocalMain->Close();
	//	delete m_pRTP2FrameLocalMain;
	//	m_pRTP2FrameLocalMain=NULL;
	//}

	ObjectClose(NETEC_RTP2Frame* ,m_pRTP2FrameLocalSecond)
}

/******************************************************************************
* GetLocalAudioLevel
*描述：获取本地音频Level【实际声音大小】
*输入：	无
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetLocalAudioLevel(int&nLevel)
{
	nLevel=0;
	if (m_pLocalAudioCapChan!=NULL)
	{
		nLevel=m_pLocalAudioCapChan->GetLevel();
		if (nLevel<0)
		{
			nLevel=0;
		}
	}
	return 0;
}

/******************************************************************************
* SetLocalAudioVolume
*描述：获取本地音频音量
*输入：	nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void MediaCall::SetLocalAudioVolume(int nVolume)
{
	m_nLocalAudioVolume=nVolume;
	if (m_pLocalAudioCapChan!=NULL)
	{
		if (m_nLocalAudioMute!=0)
		{
			m_pLocalAudioCapChan->SetVolume(0);
		}
		else
		{
			m_pLocalAudioCapChan->SetVolume(m_nLocalAudioVolume);
		}
	}
}

/******************************************************************************
* SetLocalAudioMute
*描述：获取本地音频是否静音
*输入：	nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void MediaCall::SetLocalAudioMute(int nMute)
{
	m_nLocalAudioMute=nMute;
	if (m_pLocalAudioCapChan!=NULL)
	{
		if (m_nLocalAudioMute!=0)
		{
			m_pLocalAudioCapChan->SetVolume(0);
		}
		else
		{
			m_pLocalAudioCapChan->SetVolume(m_nLocalAudioVolume);
		}
	}
}

/******************************************************************************
* GetLocalAudioCodecID
*描述：获取本地音频编码信息
*输入：	无
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetLocalAudioCodecID(X_AUDIO_CODEC_ID &nAudioCodecID)
{
	nAudioCodecID=m_nLocalAudioCodecID;
	return 0;
}

/******************************************************************************
* GetLocalAudioPacketStatistics
*描述：获取本机音频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetLocalAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	ulTotalPackets=0;
	ulLostPackets=0;
	AvgPacketLossRate=0;
	CurPacketLossRate=0;
	nBitrate=0;

	if (m_pLocalAudioCapChan!=NULL)
	{
		nBitrate=m_pLocalAudioCapChan->GetBitrate();
		if (nBitrate<0)
		{
			nBitrate=0;
		}
	}

	return 0;
}

int MediaCall::OpenLocalAudioCapDev(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType)
{
	m_nLocalAudioCapChanRTPPayloadType=nPayloadType;
	m_nLocalAudioCodecID=AudioCodecID;

	if (m_pMediaSenderLocalMain!=NULL)
	{
		m_pMediaSenderLocalMain->StartAudio(0);
	}

	//Add WebRTC by zys 20160929
#if USE_WEBRTC
	//if (NULL == m_pWebRTCAudio)
	{
		m_pWebRTCAudio = MyVoiceEngineInterface::GetVoiceEngine();
		m_pWebRTCAudio->Init();
	}

	//CodecInst inst;
	//m_pWebRTCAudio->GetCodec (CODEC_INST_INDEX, inst);
	m_pWebRTCAudio->SetSendCodec (g_emAudioCodecID);
	m_pWebRTCAudio->RegisterAudioCallback(this);
	m_pWebRTCAudio->StartRecording();

#else
	AUDEC_CodecInst codecInst;
	if (0!=AUDEC_Engine::AUDEC_GetCodecByID(AudioCodecID,&codecInst))
	{
		return -1;
	}

	if (m_pLocalAudioCapChan==NULL)
	{
		AUDEC_GetEngine().AUDEC_CreateCapChan(&m_pLocalAudioCapChan);
		if (m_pLocalAudioCapChan==NULL)
		{
			return -1;
		}
		if (0!=m_pLocalAudioCapChan->Connect(&codecInst))
		{
			m_pLocalAudioCapChan->ReleaseConnections();
			delete m_pLocalAudioCapChan;
			m_pLocalAudioCapChan=NULL;
			return -1;
		}

		AUDEC_GetEngine().AUDEC_SetVAD(0);

		if (m_nLocalAudioMute!=0)
		{
			m_pLocalAudioCapChan->SetVolume(0);
		}
		else
		{
			m_pLocalAudioCapChan->SetVolume(m_nLocalAudioVolume);
		}

		if (m_pAudioMixer==NULL)
		{
			m_pAudioMixer=AUDEC_Mixer::Create();
			if (m_pAudioMixer!=NULL)
			{
				if (0!=m_pAudioMixer->Open(m_pLocalAudioCapChan))
				{
					m_pAudioMixer->Close();
					delete m_pAudioMixer;
					m_pAudioMixer=NULL;
				}
				else
				{
					if (m_pRemoteMainPlayChan)
					{
						AUDEC_PlayChan*pPlayChan=((PlayChan*)m_pRemoteMainPlayChan)->GetPlayChan();
						if (pPlayChan)
						{
							m_pAudioMixer->ExcludePlayChan(pPlayChan);
						}
					}

					if (m_pRemoteAudioPlayChan!=NULL)
					{
						m_pAudioMixer->ExcludePlayChan(m_pRemoteAudioPlayChan);
					}
					if (m_pExcludePlayChan!=NULL)
					{
						m_pAudioMixer->ExcludePlayChan(m_pExcludePlayChan);
					}
				}
			}
		}

		m_pLocalAudioCapChan->AddStreamOut(&m_LocalMainCapChanRTP);
		m_pLocalAudioCapChan->SetRecordSamples(&m_LocalMainCapChanRTP);

		m_pLocalAudioCapChan->StartCapture();
	}
	else
	{
		if (0!=m_pLocalAudioCapChan->SetCodec(&codecInst))
		{
			return -1;
		}
	}	
#endif

	return 0;
}

void MediaCall::CloseLocalAudioCapDev(void)
{
	//Add WebRTC by zys 20160929
#if USE_WEBRTC
	if (NULL != m_pWebRTCAudio)
	{
		m_pWebRTCAudio->StopRecording ();
		m_pWebRTCAudio->DeRegisterAudioCallback();
		//MyVoiceEngineInterface::DestroyVoiceEngine(m_pWebRTCAudio);
		//m_pWebRTCAudio = NULL;
	}
	
#else
	ObjectClose(AUDEC_Mixer* ,m_pAudioMixer)

	//if (m_pAudioMixer!=NULL)
	//{
	//	m_pAudioMixer->Close();
	//	delete m_pAudioMixer;
	//	m_pAudioMixer=NULL;
	//}

	if (m_pLocalAudioCapChan!=NULL)
	{
		m_pLocalAudioCapChan->ReleaseConnections();
		delete m_pLocalAudioCapChan;
		m_pLocalAudioCapChan=NULL;
	}

	if (m_pLocalAudioCapChanBuf!=NULL)
	{
		free(m_pLocalAudioCapChanBuf);
		m_pLocalAudioCapChanBuf=NULL;
	}

	m_nLocalAudioCapChanBufLen=0;
#endif
}


//add A7
//int MediaCall::OpenRemoteMainPlayChan(unsigned long nAudioID,unsigned long nVideoID,const std::string&strPeerLocalIP,unsigned short nPeerLocalPort)
int MediaCall::OpenRemoteMainPlayChan(unsigned long nAudioID,unsigned long nVideoID,const std::string&strPeerLocalIP,unsigned short nPeerLocalPort,const std::string&strPeerMCUID, unsigned long nEnableMulticast)
{
	if (m_pRemoteMainPlayChan==NULL)
	{
		m_pRemoteMainPlayChan=IMXEC_PlayChan::Create(*this);
		if (m_pRemoteMainPlayChan!=NULL)
		{
			int nForceUseMCU =1;
			//if(strPeerMCUID.size() ==0 ) nForceUseMCU =0;

			const char * peerLIP=strPeerLocalIP.c_str();
			if(strPeerMCUID.size() > 0 && nEnableMulticast >0 )
			{
				peerLIP=NULL;
				nPeerLocalPort=0;
			}

			//if (0!=m_pRemoteMainPlayChan->Open(m_hRemoteMainVideoWnd,nAudioID,nVideoID,NULL,NULL,0,strPeerLocalIP.c_str(),nPeerLocalPort,NULL,                NULL,                  0,             0             ))
			//if (0!=m_pRemoteMainPlayChan->Open(m_hRemoteMainVideoWnd,nAudioID,nVideoID,NULL,NULL,0,NULL,   0,             strPeerMCUID.c_str(),strPeerLocalIP.c_str(),nPeerLocalPort,nForceUseMCU, nEnableMulticast))
			if (0!=m_pRemoteMainPlayChan->Open(  m_hRemoteMainVideoWnd,nAudioID,nVideoID,NULL,NULL,0,peerLIP,nPeerLocalPort,strPeerMCUID.c_str(),strPeerLocalIP.c_str(),nPeerLocalPort,nForceUseMCU, nEnableMulticast))
			{
				m_pRemoteMainPlayChan->Close();
				delete m_pRemoteMainPlayChan;
				m_pRemoteMainPlayChan=NULL;
			}
			else
			{
				CRect rc;
				::GetClientRect(m_hRemoteMainVideoWnd,rc);
				m_pRemoteMainPlayChan->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
				m_pRemoteMainPlayChan->SetVideoParam(m_nRemoteMainVideoBrightness,m_nRemoteMainVideoContrast,m_nRemoteMainVideoSaturation,m_nRemoteMainVideoHue,m_nRemoteMainVideoGamma,m_nRemoteMainVideoSharpen);

				m_pRemoteMainPlayChan->StartAudio();
				m_pRemoteMainPlayChan->StartVideo();
				m_pRemoteMainPlayChan->SetFrameRateControlMode(FRAME_RATE_CONTROL_MODE_FULL);
				m_pRemoteMainPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);

				if (m_nRemoteAudioMute!=0)
				{
					m_pRemoteMainPlayChan->SetVolume(0);
				}
				else
				{
					m_pRemoteMainPlayChan->SetVolume(m_nRemoteAudioVolume);
				}

				if (m_pAudioMixer)
				{
					AUDEC_PlayChan*pPlayChan=((PlayChan*)m_pRemoteMainPlayChan)->GetPlayChan();
					if (pPlayChan)
					{
						m_pAudioMixer->ExcludePlayChan(pPlayChan);
					}
				}
			}
		}
	}
	return (m_pRemoteMainPlayChan!=NULL?0:1);
}

void MediaCall::CloseRemoteSecondPlayChanMedia()
{
	//本端发辅流时删除远端的显示窗口
	if (m_pRemoteSecondPlayChan!=NULL)
	{
		m_pRemoteSecondPlayChan->Close();
		delete m_pRemoteSecondPlayChan;
		m_pRemoteSecondPlayChan=NULL;
	}

	//本端发辅流时再次创建显示本端
	if (m_hSecondVideoWnd != NULL && m_pSecondVideoWnd != NULL)
	{
		m_pSecondVideoWnd->Close();
		delete m_pSecondVideoWnd;
		m_pSecondVideoWnd=NULL;
	}
	if (m_hSecondVideoWnd!=NULL)
	{
		m_pSecondVideoWnd=VIDEC_Wnd::Create(*this);
		if (m_pSecondVideoWnd!=NULL)
		{
			if (0!=m_pSecondVideoWnd->Open(m_hSecondVideoWnd,true,NULL,NULL))
			{
				m_pSecondVideoWnd->Close();
				delete m_pSecondVideoWnd;
				m_pSecondVideoWnd=NULL;
			}
			else 
			{
				CRect rc;
				::GetClientRect(m_hSecondVideoWnd,rc);
				m_pSecondVideoWnd->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
				m_pSecondVideoWnd->SetVideoParam(VIDEC_GetDefaultBrightness(),VIDEC_GetDefaultContrast(),VIDEC_GetDefaultSaturation(),VIDEC_GetDefaultHue(),VIDEC_GetDefaultGamma(),(m_bSecondVideoSharpenLocalOnly?m_nSecondVideoSharpen:VIDEC_GetDefaultSharpen()));

				if (m_pSecondVideoCapDev!=NULL)
				{
					m_pSecondVideoCapDev->AddImageSink(m_pSecondVideoWnd);
				}
				else if (m_pSecondVideoPlayChan!=NULL)
				{
					m_pSecondVideoPlayChan->AddImageSink(m_pSecondVideoWnd);
				}
			}
		}
	}
}

int MediaCall::OpenRemoteSecondPlayChan(unsigned long nSecondVideoID,const std::string&strPeerLocalIP,unsigned short nPeerLocalPort,const std::string&strPeerMCUID, unsigned long nEnableMulticast)
{
	//if (m_pRemoteSecondPlayChan!=NULL)
	//{
	//	m_pRemoteSecondPlayChan->Close();
	//	delete m_pRemoteSecondPlayChan;
	//	m_pRemoteSecondPlayChan=NULL;
	//}

	if (m_pRemoteSecondPlayChan == NULL)
		m_pRemoteSecondPlayChan=IMXEC_PlayChan::Create(*this);

	if (m_pRemoteSecondPlayChan!=NULL)
	{
		int nForceUseMCU =1;
		//if(strPeerMCUID.size() ==0 ) nForceUseMCU =0;

		const char * peerLIP=strPeerLocalIP.c_str();
		if(strPeerMCUID.size() > 0 && nEnableMulticast >0 )
		{
			peerLIP=NULL;
			nPeerLocalPort=0;
		}

		if (0!=m_pRemoteSecondPlayChan->Open(m_hSecondVideoWnd,0,nSecondVideoID,NULL,NULL,0,peerLIP,nPeerLocalPort,strPeerMCUID.c_str(),strPeerLocalIP.c_str(),nPeerLocalPort,nForceUseMCU, nEnableMulticast))
		{
			m_pRemoteSecondPlayChan->Close();
			delete m_pRemoteSecondPlayChan;
			m_pRemoteSecondPlayChan=NULL;
		}
		else
		{
			CRect rc;
			::GetClientRect(m_hSecondVideoWnd,rc);
			m_pRemoteSecondPlayChan->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
			m_pRemoteSecondPlayChan->SetVideoParam(m_nSecondVideoBrightness,m_nSecondVideoContrast,m_nSecondVideoSaturation,m_nSecondVideoHue,m_nSecondVideoGamma,m_nSecondVideoSharpen);

			m_pRemoteSecondPlayChan->StartVideo();
			m_pRemoteSecondPlayChan->SetFrameRateControlMode(FRAME_RATE_CONTROL_MODE_FULL);
			m_pRemoteSecondPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);
		}
	}

	return (m_pRemoteSecondPlayChan!=NULL?0:1);
}

int MediaCall::OpenRemoteMainVideoPlayChan(VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType)
{
	if (nPayloadType==34)
		codecType=VIDEC_CODEC_H263;
	m_nRemoteMainVideoCodecType=codecType;

	if (HasAVCONTransport())
	{
		if (m_pMediaSenderRemoteMain!=NULL)
		{
			m_pMediaSenderRemoteMain->StartVideo(0);
		}
		return 0;
	}

	if (m_pRTPJBufMainVideo==NULL)
	{
		//if(CallMgr::Instance().m_nUseNETECSend > 0 )
		if( 0 )
			m_pRTPJBufMainVideo=NETEC_RTPJBuf::Create(*this);
		if (m_pRTPJBufMainVideo)
		{
			if (0!=m_pRTPJBufMainVideo->Open())
			{
				m_pRTPJBufMainVideo->Close();
				delete m_pRTPJBufMainVideo;
				m_pRTPJBufMainVideo=NULL;
			}
		}
	}

	IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
	CallMgr::Instance().GetCallType(nCallType);
	if (nCallType==IMXEC_MP_CALL ||
		nCallType==IMXEC_MCU_CALL)
	{
		if (IMXEC_Call::GetEnableVideoTranscoding()==0)
		{
			if (m_pRTP2FrameRemoteMain==NULL)
			{
				switch (codecType)
				{
				case VIDEC_CODEC_H263://H.263
					m_pRTP2FrameRemoteMain=NETEC_RTP2Frame::Create(*this,"H263");
					break;
				case VIDEC_CODEC_H263P://H.263P
					m_pRTP2FrameRemoteMain=NETEC_RTP2Frame::Create(*this,"H263-1998");
					break;
				case VIDEC_CODEC_H263PP://H.263PP
					m_pRTP2FrameRemoteMain=NETEC_RTP2Frame::Create(*this,"H263-2000");
					break;
					//notice 20120116 update
				case VIDEC_CODEC_H264://H.264
				case VIDEC_CODEC_H264_SVC://H.264
					m_pRTP2FrameRemoteMain=NETEC_RTP2Frame::Create(*this,"H264");
					break;
				case VIDEC_CODEC_MPEG4://MPEG4 ,use to call kedacom
					m_pRTP2FrameRemoteMain=NETEC_RTP2Frame::Create(*this,"MPEG4");
					break;
				}

				if (m_pRTP2FrameRemoteMain!=0)
				{
					if (0!=m_pRTP2FrameRemoteMain->Open(VIDEC_HEADER_EXT_KEY_FRAME_LEN))
					{
						m_pRTP2FrameRemoteMain->Close();
						delete m_pRTP2FrameRemoteMain;
						m_pRTP2FrameRemoteMain=NULL;
					}
					else
					{
						if (m_pMediaSenderRemoteMain!=NULL)
						{
							m_pMediaSenderRemoteMain->StartVideo(0);
						}
					}
				}
			}
			return 0;
		}
	}

	if (m_pRemoteMainVideoPlayChan==NULL)
	{
		switch (codecType)
		{
		case VIDEC_CODEC_H261://H.261
			nPayloadType=31;
			break;
		case VIDEC_CODEC_H263://H.263
			nPayloadType=34;
			break;
		case VIDEC_CODEC_H263P://H.263
			break;
		case VIDEC_CODEC_H263PP://H.263
			break;
		case VIDEC_CODEC_H264://H.264
		case VIDEC_CODEC_H264_SVC://H.264
		case VIDEC_CODEC_MPEG4://MPEG4 ,use to call kedacom
			break;
		}

		if (nPayloadType!=0)
		{
			VIDEC_PlayChanRTP*pVideoPlayChan=VIDEC_PlayChanRTP::Create();
			if (pVideoPlayChan!=NULL)
			{
				if (0!=pVideoPlayChan->Open(nPayloadType,codecType))
				{
					pVideoPlayChan->Close();
					delete pVideoPlayChan;
					pVideoPlayChan=NULL;
				}
				else
				{
					m_pRemoteMainVideoPlayChan=pVideoPlayChan;

					if (m_pMediaSenderRemoteMain!=NULL)
					{
						m_pMediaSenderRemoteMain->StartVideo(0);
					}
					
					if (IMXEC_Call::GetEnableVideoTranscoding()!=0)
					{
						if (m_pMainVideoTranscoder!=NULL)
						{
							m_pMainVideoTranscoder->Close();
							delete m_pMainVideoTranscoder;
							m_pMainVideoTranscoder=NULL;
						}
						m_pMainVideoTranscoder=VIDEC_CapChan::Create(*this);
						if (m_pMainVideoTranscoder!=NULL)
						{
							if (0!=m_pMainVideoTranscoder->Open(nBitrate,VIDEC_NETWORK_PRIVATE,VIDEC_CODEC_H264_SVC,0))
							{
								m_pMainVideoTranscoder->Close();
								delete m_pMainVideoTranscoder;
								m_pMainVideoTranscoder=NULL;
							}
							else
							{
								m_pRemoteMainVideoPlayChan->AddImageSink(m_pMainVideoTranscoder);
								m_pMainVideoTranscoder->AddStreamSink(&m_RemoteMainPlayChanRTP);
							}
						}
					}

					if (m_pMainVideoTranscoder==NULL)
					{
						m_pRemoteMainVideoPlayChan->AddStreamSink(&m_RemoteMainPlayChanRTP);
					}

					if (m_pRemoteMainVideoWnd!=NULL)
					{
						m_pRemoteMainVideoPlayChan->AddImageSink(m_pRemoteMainVideoWnd);
					}
				}
			}
		}
	}

	return (m_pRemoteMainVideoPlayChan!=NULL?0:-1);
}

void MediaCall::OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	if (m_pRTPJBufMainVideo)
	{
		m_pRTPJBufMainVideo->OnRecvdPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
	else
	{
		OnRecvdRemoteMainVideoRTPPacket(pRTPPacketData,nRTPPacketLen);
	}
}

void MediaCall::OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen)
{
	m_XBitrateRemoteMainVideo.Calculate(nRTPPacketLen);

	if (m_pRemoteMainVideoPlayChan!=NULL)
	{
		m_pRemoteMainVideoPlayChan->OnRecvdRTPPacket((unsigned char*)pRTPPacketData,nRTPPacketLen);
	}
	if (m_pRTP2FrameRemoteMain!=NULL)
	{
		m_pRTP2FrameRemoteMain->OnRecvdRTPPacket((unsigned char*)pRTPPacketData,nRTPPacketLen);
	}
}

void MediaCall::CloseRemoteMainVideoPlayChan(void)
{
	ObjectClose(NETEC_RTP2Frame* ,m_pRTP2FrameRemoteMain)

	//if (m_pRTP2FrameRemoteMain!=NULL)
	//{
	//	m_pRTP2FrameRemoteMain->Close();
	//	delete m_pRTP2FrameRemoteMain;
	//	m_pRTP2FrameRemoteMain=NULL;
	//}

	ObjectClose(VIDEC_PlayChanRTP* ,m_pRemoteMainVideoPlayChan)

	//if (m_pRemoteMainVideoPlayChan!=NULL)
	//{
	//	m_pRemoteMainVideoPlayChan->Close();
	//	delete m_pRemoteMainVideoPlayChan;
	//	m_pRemoteMainVideoPlayChan=NULL;
	//}
	ObjectClose(VIDEC_CapChan* ,m_pMainVideoTranscoder)

	//if (m_pMainVideoTranscoder!=NULL)
	//{
	//	m_pMainVideoTranscoder->Close();
	//	delete m_pMainVideoTranscoder;
	//	m_pMainVideoTranscoder=NULL;
	//}

	ObjectClose(NETEC_RTPJBuf* ,m_pRTPJBufMainVideo)

	//if (m_pRTPJBufMainVideo!=NULL)
	//{
	//	m_pRTPJBufMainVideo->Close();
	//	delete m_pRTPJBufMainVideo;
	//	m_pRTPJBufMainVideo=NULL;
	//}
}

/******************************************************************************
* GetRemoteAudioLevel
*描述：获取远程音频Level【实际声音大小】
*输入：	无
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetRemoteAudioLevel(int&nLevel)
{
	nLevel=0;
	if (m_pRemoteMainPlayChan)
	{
		m_pRemoteMainPlayChan->GetLevel(nLevel);
		return 0;
	}

	if (m_pRemoteAudioPlayChan!=NULL)
	{
		nLevel=m_pRemoteAudioPlayChan->GetLevel();
		if (nLevel<0)
		{
			nLevel=0;
		}
	}

	return 0;
}

/******************************************************************************
* SetRemoteAudioVolume
*描述：获取远程音频音量
*输入：	nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void MediaCall::SetRemoteAudioVolume(int nVolume)
{
	m_nRemoteAudioVolume=nVolume;

	if (m_pRemoteMainPlayChan)
	{
		if (m_nRemoteAudioMute!=0)
		{
			m_pRemoteMainPlayChan->SetVolume(0);
		}
		else
		{
			m_pRemoteMainPlayChan->SetVolume(m_nRemoteAudioVolume);
		}
		return ;
	}

	if (m_pRemoteAudioPlayChan!=NULL)
	{
		if (m_nRemoteAudioMute!=0)
		{
			m_pRemoteAudioPlayChan->SetVolume(0);
		}
		else
		{
			m_pRemoteAudioPlayChan->SetVolume(m_nRemoteAudioVolume);
		}
	}
}

/******************************************************************************
* SetRemoteAudioMute
*描述：获取远程音频是否静音
*输入：	nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void MediaCall::SetRemoteAudioMute(int nMute)
{
	m_nRemoteAudioMute=nMute;
	if (m_pRemoteMainPlayChan)
	{
		if (m_nRemoteAudioMute!=0)
		{
			m_pRemoteMainPlayChan->SetVolume(0);
		}
		else
		{
			m_pRemoteMainPlayChan->SetVolume(m_nRemoteAudioVolume);
		}
		return ;
	}
	if (m_pRemoteAudioPlayChan!=NULL)
	{
		if (m_nRemoteAudioMute!=0)
		{
			m_pRemoteAudioPlayChan->SetVolume(0);
		}
		else
		{
			m_pRemoteAudioPlayChan->SetVolume(m_nRemoteAudioVolume);
		}
	}
}

/******************************************************************************
* GetRemoteAudioCodecID
*描述：获取远端音频编码
*输入：	无
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetRemoteAudioCodecID (X_AUDIO_CODEC_ID &nAudioCodecID)
{
	nAudioCodecID=m_nRemoteAudioPlayChanCodecID;
	return 0;
}


/******************************************************************************
* GetRemoteAudioPacketStatistics
*描述：获取远端音频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetRemoteAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	ulTotalPackets=0;
	ulLostPackets=0;
	AvgPacketLossRate=0;
	CurPacketLossRate=0;
	nBitrate=0;

	if (m_pRemoteAudioPlayChan!=NULL)
	{
		nBitrate=m_pRemoteAudioPlayChan->GetBitrate();
		if (nBitrate<0)
		{
			nBitrate=0;
		}
	}
	if (m_pRemoteMainPlayChan)
	{
		m_pRemoteMainPlayChan->GetAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	////信息栏显示有视频丢包、音频丢包，且丢包数上升很快 暂不启用
	//ulLostPackets = 0;
	return 0;
}

int MediaCall::OpenRemoteAudioPlayChan(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType)
{
	m_nRemoteAudioPlayChanCodecID=AudioCodecID;
	m_nRemoteAudioPayloadType=nPayloadType;

	IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
	CallMgr::Instance().GetCallType(nCallType);
	if (nCallType==IMXEC_MP_CALL ||
		nCallType==IMXEC_MCU_CALL)
	{
		if (m_pMediaSenderRemoteMain!=NULL)
		{
			m_pMediaSenderRemoteMain->StartAudio(0);
		}

		return 0;
	}

	if (HasAVCONTransport())
	{
		return 0;
	}

	if (m_pRTPJBufAudio==NULL)
	{
		if(CallMgr::Instance().m_nUseNETECSend > 0 )
		if( 0 )
			m_pRTPJBufAudio=NETEC_RTPJBuf::Create(*this);
		if (m_pRTPJBufAudio)
		{
			if (0!=m_pRTPJBufAudio->Open())
			{
				m_pRTPJBufAudio->Close();
				delete m_pRTPJBufAudio;
				m_pRTPJBufAudio=NULL;
			}
			//OutputDebugString("m_pRTPJBufAudio Create.\n");
		}
	}

	//Add WebRTC by zys 20160929
#if USE_WEBRTC

	//if (NULL == m_pWebRTCAudio)
	{
		m_pWebRTCAudio = MyVoiceEngineInterface::GetVoiceEngine();
		m_pWebRTCAudio->Init();
	}

	if (-1 == m_nWebRTCPlayChan)
	{
		m_nWebRTCPlayChan = m_pWebRTCAudio->CreateDecodeChannel();
		m_pWebRTCAudio->StartPlayout(m_nWebRTCPlayChan);
	}

	if (m_pMediaSenderRemoteMain!=NULL)
	{
		m_pMediaSenderRemoteMain->StartAudio(0);
	}


#else
	if (m_pRemoteAudioPlayChan==NULL)
	{
		AUDEC_PlayChan*pAudioPlayChan=NULL;
		AUDEC_GetEngine().AUDEC_CreatePlayChan(&pAudioPlayChan);
		if (pAudioPlayChan!=NULL)
		{
			if (0!=pAudioPlayChan->Connect())
			{
				pAudioPlayChan->ReleaseConnections();
				delete pAudioPlayChan;
				pAudioPlayChan=NULL;

				return -1;
			}
			else
			{
				m_pRemoteAudioPlayChan=pAudioPlayChan;
				m_pRemoteAudioPlayChan->SetRecordSamples(&m_RemoteMainPlayChanRTP);

				if (m_nRemoteAudioMute!=0)
				{
					m_pRemoteAudioPlayChan->SetVolume(0);
				}
				else
				{
					m_pRemoteAudioPlayChan->SetVolume(m_nRemoteAudioVolume);
				}

				if (m_pAudioMixer!=NULL)
				{
					m_pAudioMixer->ExcludePlayChan(m_pRemoteAudioPlayChan);
				}

				if (m_pMediaSenderRemoteMain!=NULL)
				{
					m_pMediaSenderRemoteMain->StartAudio(0);
				}

				pAudioPlayChan->SetDTMFCallback(this);//add by Jan for DTMF [20140730]

			}
		}
	}
#endif
	return 0;
}

void MediaCall::ExcludePlayChan(AUDEC_PlayChan*pPlayChan)
{
	m_pExcludePlayChan=pPlayChan;
	if (m_pAudioMixer!=NULL)
	{
		m_pAudioMixer->ExcludePlayChan(pPlayChan);
	}
}

void MediaCall::OnRecvdRemoteAudioRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	if (nPayloadType==101 || nRTPPacketLen<=RTP_HEADER_LEN)
	{
		return;
	}

	if (m_pRTPJBufAudio)
	{
		m_pRTPJBufAudio->OnRecvdPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
	else
	{
		OnRecvdRemoteAudioRTPPacketJBuf(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
}

void MediaCall::OnRecvdRemoteAudioRTPPacketJBuf(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	if (m_pRemoteAudioPlayChanBuf==NULL ||
		m_nRemoteAudioPlayChanBufLen<(nRTPPacketLen-nRTPHeaderLen+AUDEC_HEADER_VER_01_LEN))
	{
		if (m_pRemoteAudioPlayChanBuf!=NULL)
		{
			free(m_pRemoteAudioPlayChanBuf);
			m_pRemoteAudioPlayChanBuf=NULL;
		}

		m_nRemoteAudioPlayChanBufLen=(nRTPPacketLen-nRTPHeaderLen+AUDEC_HEADER_VER_01_LEN)*2;
		m_pRemoteAudioPlayChanBuf=(unsigned char*)malloc(m_nRemoteAudioPlayChanBufLen);
	}

	if (m_pRemoteAudioPlayChanBuf!=NULL)
	{
		unsigned char*pAudioPacket=m_pRemoteAudioPlayChanBuf;
		unsigned char*pData=pRTPPacketData+nRTPHeaderLen;
		int nLen=nRTPPacketLen-nRTPHeaderLen;

		ulTimestamp=XGetTimestamp();

		AUDEC_HEADER_RESET(pAudioPacket);
		AUDEC_HEADER_SET_SEQUENCE(pAudioPacket,usSequence);
		AUDEC_HEADER_SET_TIMESTAMP(pAudioPacket,ulTimestamp);
		AUDEC_HEADER_SET_CODEC_ID(pAudioPacket,m_nRemoteAudioPlayChanCodecID);
		int nHeaderSize=AUDEC_HEADER_GET_LEN(pAudioPacket);

		memcpy(pAudioPacket+nHeaderSize,pData,nLen);

		//Add WebRTC by zys 20160929
#if USE_WEBRTC
		if ( (NULL != m_pWebRTCAudio) && (-1 !=m_nWebRTCPlayChan) )
		{
			unsigned char pRTPPacket[1500];
			int nRTPPacketLen = AudecHeaderToRTPHeader(pRTPPacket,pData,nLen);

			m_pWebRTCAudio->Decode(m_nWebRTCPlayChan, pRTPPacket, nRTPPacketLen);
		}
#else
		if (m_pRemoteAudioPlayChan!=NULL)
		{
			m_pRemoteAudioPlayChan->Write(pAudioPacket,nHeaderSize+nLen);
		}
#endif

		if (m_pMediaSenderRemoteMain)
		{
			m_pMediaSenderRemoteMain->SendAudio(pAudioPacket,nHeaderSize+nLen);
		}
		{
			XAutoLock l(m_csRemoteRecordCallback);
			if (m_pRemoteRecordCallback!=NULL)
			{
				m_pRemoteRecordCallback->OnIMXEC_RecordCallbackAudioData((unsigned char*)pAudioPacket,nHeaderSize+nLen);
			}
		}
	}
}

void MediaCall::CloseRemoteAudioPlayChan(void)
{
	//Add WebRTC by zys 20160929
#if USE_WEBRTC
	if (NULL != m_pWebRTCAudio)
	{
		if( -1 != m_nWebRTCPlayChan)
		{
			m_pWebRTCAudio->StopPlayout(m_nWebRTCPlayChan);
			m_pWebRTCAudio->DestroyDecodeChannel(m_nWebRTCPlayChan);
			m_nWebRTCPlayChan = -1;
		}
		
		//MyVoiceEngineInterface::DestroyVoiceEngine(m_pWebRTCAudio);
		//m_pWebRTCAudio = NULL;
	}

#else
	if (m_pRemoteAudioPlayChan!=NULL)
	{
		m_pRemoteAudioPlayChan->ReleaseConnections();
		delete m_pRemoteAudioPlayChan;
		m_pRemoteAudioPlayChan=NULL;
	}
#endif

	ObjectClose(NETEC_RTPJBuf* ,m_pRTPJBufAudio)

	//if (m_pRTPJBufAudio!=NULL)
	//{
	//	m_pRTPJBufAudio->Close();
	//	delete m_pRTPJBufAudio;
	//	m_pRTPJBufAudio=NULL;
	//}

	if (m_pRemoteAudioPlayChanBuf!=NULL)
	{
		free(m_pRemoteAudioPlayChanBuf);
		m_pRemoteAudioPlayChanBuf=NULL;
		m_nRemoteAudioPlayChanBufLen=0;
	}
}


int MediaCall::OpenLocalSecondVideoCapDev(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nNetworkType)
{
	char msg[256]={0};
	sprintf(msg,"nWidth%d,nHeight%d,nFrameRate%d,cType%d,nBitrate%d",nWidth,nHeight,nFrameRate,codecType,nBitrate);
	LogFile(NULL,NULL,"",msg,"OpenLocalSecondVideoCapDev");

	m_nSecondVideoCodecType=codecType;

	if (m_pRTP2FrameLocalSecond==NULL)
	{
		switch (codecType)
		{
		case VIDEC_CODEC_H263://H.263
			m_pRTP2FrameLocalSecond=NETEC_RTP2Frame::Create(*this,"H263");
			break;
		case VIDEC_CODEC_H263P://H.263P
			m_pRTP2FrameLocalSecond=NETEC_RTP2Frame::Create(*this,"H263-1998");
			break;
		case VIDEC_CODEC_H263PP://H.263PP
			m_pRTP2FrameLocalSecond=NETEC_RTP2Frame::Create(*this,"H263-2000");
			break;
			//notice 20120116 update
		case VIDEC_CODEC_H264://H.264
		case VIDEC_CODEC_H264_SVC://H.264
			m_pRTP2FrameLocalSecond=NETEC_RTP2Frame::Create(*this,"H264");
			break;
		case VIDEC_CODEC_MPEG4://MPEG4 ,use to call kedacom
			m_pRTP2FrameLocalSecond=NETEC_RTP2Frame::Create(*this,"MPEG4");
			break;
		}

		if (m_pRTP2FrameLocalSecond!=0)
		{
			if (0!=m_pRTP2FrameLocalSecond->Open(VIDEC_HEADER_EXT_KEY_FRAME_LEN))
			{
				m_pRTP2FrameLocalSecond->Close();
				delete m_pRTP2FrameLocalSecond;
				m_pRTP2FrameLocalSecond=NULL;
			}
			else
			{
				if (m_pMediaSenderLocalSecond!=NULL)
				{
					m_pMediaSenderLocalSecond->StartVideo(0);
				}
			}
		}
	}

	IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
	CallMgr::Instance().GetCallType(nCallType);
	if (nCallType==IMXEC_PERSONAL_CALL)
	{
		VIDEC_CapDev*pOldCapDev=m_pSecondVideoCapDev;
		if (nDevID<0)
		{
			//Added by jiangdingfeng 2014-05-19
			VIDEC_Screen* pScreen = NULL;

			RECT rt;
			GetWindowRect(GetDesktopWindow(),&rt);
			if (m_pSecondVideoCapDev != NULL)
			{
				pScreen = dynamic_cast<VIDEC_Screen*>(m_pSecondVideoCapDev);
			}

			if (pScreen ==  NULL)
			{
				if (m_pSecondVideoCapDev!=NULL)
				{
					m_pSecondVideoCapDev->Close();
					delete m_pSecondVideoCapDev;
					m_pSecondVideoCapDev=NULL;
					pOldCapDev = NULL;
				}

				pScreen = VIDEC_Screen::Create();

				if (pScreen != NULL)
				{
					if (0 != pScreen->Open(rt.left, rt.top, rt.right, rt.bottom, nFrameRate, 24))
					//if (0 != pScreen->Open(0,0,nWidth,nHeight, nFrameRate, 24))
					{
						pScreen->Close();
						delete pScreen;
						pScreen=NULL;
					}
					else
					{
						m_pSecondVideoCapDev=pScreen;
					}
				}
				else
				{
					pScreen->Reset(rt.left, rt.top, rt.right, rt.bottom, nFrameRate, 24);
					//pScreen->Reset(0,0,nWidth,nHeight, nFrameRate, 24);
				}
			}
			else
			{
				pScreen->Reset(rt.left, rt.top, rt.right, rt.bottom, nFrameRate, 24);
				//pScreen->Reset(0,0,nWidth,nHeight, nFrameRate, 24);
			}

			//CloseLocalSecondVideoCapDev();
			//return -1;
		}
		else
		{
			//VIDEC_CapDev*pOldCapDev=m_pSecondVideoCapDev;
			while (1)
			{
				int nTempDevCnt=VIDEC_AH2KDev::GetDevCount();
				if (nDevID < nTempDevCnt)
				{
					if (nDevID>=0)
					{
						VIDEC_AH2KDev*pAH2KDev=NULL;
						if (m_pSecondVideoCapDev!=NULL)
						{
							pAH2KDev=dynamic_cast<VIDEC_AH2KDev*>(m_pSecondVideoCapDev);
						}
						if (pAH2KDev==NULL)
						{
							if (m_pSecondVideoCapDev!=NULL)
							{
								m_pSecondVideoCapDev->Close();
								delete m_pSecondVideoCapDev;
								m_pSecondVideoCapDev=NULL;

								pOldCapDev=NULL;
							}

							pAH2KDev=VIDEC_AH2KDev::Create();
							if (pAH2KDev!=NULL)
							{
								if (0!=pAH2KDev->Open(nDevID,0,0,nFrameRate))
								{
									pAH2KDev->Close();
									delete pAH2KDev;
									pAH2KDev=NULL;
								}
								else
								{
									m_pSecondVideoCapDev=pAH2KDev;
								}
							}
						}
						else
						{
							pAH2KDev->Reset(nDevID,0,0,nFrameRate);
						}

						break ;
					}
				}

				nDevID-=nTempDevCnt;
				nTempDevCnt=VIDEC_CameraDev::GetDevCount();
				if (nDevID < nTempDevCnt)
				{
					if (nDevID>=0)
					{
						VIDEC_CameraDev*pCameraDev=NULL;
						if (m_pSecondVideoCapDev!=NULL)
						{
							pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pSecondVideoCapDev);
						}
						if (pCameraDev==NULL)
						{
							if (m_pSecondVideoCapDev!=NULL)
							{
								m_pSecondVideoCapDev->Close();
								delete m_pSecondVideoCapDev;
								m_pSecondVideoCapDev=NULL;

								pOldCapDev=NULL;
							}

							pCameraDev=VIDEC_CameraDev::Create();
							if (pCameraDev!=NULL)
							{
								if (0!=pCameraDev->Open(nDevID,0,0,nFrameRate))
								{
									pCameraDev->Close();
									delete pCameraDev;
									pCameraDev=NULL;
								}
								else
								{
									m_pSecondVideoCapDev=pCameraDev;
								}
							}
						}
						else
						{
							pCameraDev->Reset(nDevID,0,0,nFrameRate);
						}
						break ;
					}
				}

				nDevID-=nTempDevCnt;
				nTempDevCnt=VIDEC_AH400Dev::GetDevCount();
				if (nDevID < nTempDevCnt)
				{
					if (nDevID>=0)
					{
						VIDEC_AH400Dev*pAH400Dev=NULL;
						if (m_pSecondVideoCapDev!=NULL)
						{
							pAH400Dev=dynamic_cast<VIDEC_AH400Dev*>(m_pSecondVideoCapDev);
						}
						if (pAH400Dev==NULL)
						{
							if (m_pSecondVideoCapDev!=NULL)
							{
								m_pSecondVideoCapDev->Close();
								delete m_pSecondVideoCapDev;
								m_pSecondVideoCapDev=NULL;

								pOldCapDev=NULL;
							}

							pAH400Dev=VIDEC_AH400Dev::Create();
							if (pAH400Dev!=NULL)
							{
								if (0!=pAH400Dev->Open(nDevID,0,0,nFrameRate))
								{
									pAH400Dev->Close();
									delete pAH400Dev;
									pAH400Dev=NULL;
								}
								else
								{
									m_pSecondVideoCapDev=pAH400Dev;
								}
							}
						}
						else
						{
							pAH400Dev->Reset(nDevID,0,0,nFrameRate);
						}
						break ;
					}
				}

				break;
			}
		}

		if (pOldCapDev!=m_pSecondVideoCapDev && 
			m_pSecondVideoCapDev!=NULL)
		{
			m_pSecondVideoCapDev->EnableOSD(m_nSecondVideoEnableOSD);
			m_pSecondVideoCapDev->SetOSDFont(m_fontSecondVideoOSD,m_crSecondVideoText,m_crSecondVideoBk);
			m_pSecondVideoCapDev->SetOSDText(m_strSecondVideoOSD.c_str(),m_nSecondVideoAlign);

			int nSharpen=m_nSecondVideoSharpen;
			if (m_bSecondVideoSharpenLocalOnly)
			{
				nSharpen=VIDEC_GetDefaultSharpen();
			}

			if (m_pSecondVideoCapDev)
			{
				VIDEC_AH400Dev*pAH400Dev=dynamic_cast<VIDEC_AH400Dev*>(m_pSecondVideoCapDev);
				if (pAH400Dev!=NULL)
				{
					pAH400Dev->SetVideoParam(m_nSecondVideoBrightness,m_nSecondVideoContrast,m_nSecondVideoSaturation,m_nSecondVideoHue,m_nSecondVideoGamma,nSharpen);
				}
				else
				{
					VIDEC_AH2KDev*pAH2KDev=dynamic_cast<VIDEC_AH2KDev*>(m_pSecondVideoCapDev);
					if (pAH2KDev!=NULL)
					{
						pAH2KDev->SetVideoParam(m_nSecondVideoBrightness,m_nSecondVideoContrast,m_nSecondVideoSaturation,m_nSecondVideoHue,m_nSecondVideoGamma,nSharpen);
					}
					else
					{
						VIDEC_CameraDev*pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pSecondVideoCapDev);
						if (pCameraDev!=NULL)
						{
							pCameraDev->SetVideoParam(m_nSecondVideoBrightness,m_nSecondVideoContrast,m_nSecondVideoSaturation,m_nSecondVideoHue,m_nSecondVideoGamma,nSharpen);
						}
					}
				}
			}

			if (m_pSecondVideoWnd!=NULL)
			{
				m_pSecondVideoCapDev->AddImageSink(m_pSecondVideoWnd);
			}
			//added by jiangdingfeng 2013-11-28
			if (m_pSecondVideoCapDev!=NULL)
			{
				m_pSecondVideoCapDev->AddImageSink(m_pSecondVideoCapChan);
			}
		}

		if (m_pSecondVideoCapDev!=NULL)
		{
			if (m_pSecondVideoCapChan==NULL)
			{
				int nPayloadType=0;
				switch (codecType)
				{
				case VIDEC_CODEC_H261://H.261
					nPayloadType=31;
					break;
				case VIDEC_CODEC_H263://H.263
					nPayloadType=34;
					break;
					//notice 20120116 update
				case VIDEC_CODEC_H264://H.264
				case VIDEC_CODEC_H264_SVC://H.264
				case VIDEC_CODEC_MPEG4://MPEG4,use to call kedacom
					nPayloadType=106;
					break;
				}			

				VIDEC_CapChanRTP*pVideoCapChan=VIDEC_CapChanRTP::Create(m_LocalSecondCapChanRTP);
				if (pVideoCapChan!=NULL)
				{
					if (0!=pVideoCapChan->Open(nPayloadType,nWidth,nHeight,nFrameRate,nBitrate,nNetworkType,codecType))
					{
						pVideoCapChan->Close();
						delete pVideoCapChan;
						pVideoCapChan=NULL;
					}
					else
					{
						m_pSecondVideoCapChan=pVideoCapChan;

						m_LocalSecondCapChanRTP.StopRecordToAVI();
						m_RemoteSecondPlayChanRTP.StopRecordToAVI();
						if (m_strSecondRecordPathName.size())
						{
							std::string strPathName=m_strSecondRecordPathName;
							if (m_nSecondRecordIndex>0)
							{
								char szFileIndex[128]="";
								sprintf(szFileIndex,"(%u)",m_nSecondRecordIndex);
								std::string strExt="";
								int nPos=strPathName.find_last_of(".");
								if (nPos>0)
								{
									strExt=strPathName.substr(nPos);
									strPathName=strPathName.substr(0,nPos);
									strPathName+=szFileIndex;
									strPathName+=strExt;
								}
							}
							m_nSecondRecordIndex++;
							m_LocalSecondCapChanRTP.StartRecordToAVI(strPathName.c_str(),1);
						}

						m_pSecondVideoCapChan->AddStreamSink(&m_LocalSecondCapChanRTP);
						m_pSecondVideoCapDev->AddImageSink(m_pSecondVideoCapChan);
					}
				}
			}
		}

		return (m_pSecondVideoCapDev!=NULL?0:-1);
	}
	else if (nCallType==IMXEC_CONFERENCE_CALL || nCallType==IMXEC_MP_CALL)
	{
		if (m_pSecondVideoMPScreen==NULL)
		{
			m_pSecondVideoMPScreen=VIDEC_MPScreen::Create();
			if (m_pSecondVideoMPScreen!=NULL)
			{
				if (0!=m_pSecondVideoMPScreen->Open(nWidth,nHeight,nFrameRate))
				{
					m_pSecondVideoMPScreen->Close();
					delete m_pSecondVideoMPScreen;
					m_pSecondVideoMPScreen=NULL;
				}
			}
		}

		if (m_pSecondVideoMPScreen!=NULL)
		{
			if (m_nSecondVideoScreenIndex>=0)
			{
				m_pSecondVideoMPScreen->SetScreenIndex(m_nSecondVideoScreenIndex);
			}
		}

		if (m_pSecondVideoMPScreen!=NULL && m_pSecondVideoCapChan==NULL)
		{
			int nPayloadType=0;
			switch (codecType)
			{
			case VIDEC_CODEC_H261://H.261
				nPayloadType=31;
				break;
			case VIDEC_CODEC_H263://H.263
				nPayloadType=34;
				break;
			case VIDEC_CODEC_H263P://H.263
				nPayloadType=107;
				break;
			case VIDEC_CODEC_H263PP://H.263
				nPayloadType=108;
				break;
				//notice 20120116 update
			case VIDEC_CODEC_H264://H.264
			case VIDEC_CODEC_H264_SVC://H.264
			case VIDEC_CODEC_MPEG4://MPEG4,use to call kedacom
				nPayloadType=106;
				break;
			}			

			VIDEC_CapChanRTP*pVideoCapChan=VIDEC_CapChanRTP::Create(m_LocalSecondCapChanRTP);
			if (pVideoCapChan!=NULL)
			{
				if (0!=pVideoCapChan->Open(nPayloadType,nWidth,nHeight,nFrameRate,nBitrate,nNetworkType,codecType))
				{
					pVideoCapChan->Close();
					delete pVideoCapChan;
					pVideoCapChan=NULL;
				}
				else
				{
					m_pSecondVideoCapChan=pVideoCapChan;

					m_LocalSecondCapChanRTP.StopRecordToAVI();
					m_RemoteSecondPlayChanRTP.StopRecordToAVI();
					if (m_strSecondRecordPathName.size())
					{
						std::string strPathName=m_strSecondRecordPathName;
						if (m_nSecondRecordIndex>0)
						{
							char szFileIndex[128]="";
							sprintf(szFileIndex,"(%u)",m_nSecondRecordIndex);
							std::string strExt="";
							int nPos=strPathName.find_last_of(".");
							if (nPos>0)
							{
								strExt=strPathName.substr(nPos);
								strPathName=strPathName.substr(0,nPos);
								strPathName+=szFileIndex;
								strPathName+=strExt;
							}
						}
						m_nSecondRecordIndex++;

						m_LocalSecondCapChanRTP.StartRecordToAVI(strPathName.c_str(),1);
					}

					m_pSecondVideoCapChan->AddStreamSink(&m_LocalSecondCapChanRTP);
					m_pSecondVideoMPScreen->AddImageSink(m_pSecondVideoCapChan);
				}
			}
		}
		return (m_pSecondVideoMPScreen!=NULL?0:-1);
	}
	else if (nCallType==IMXEC_MCU_CALL)
	{
		return CallMgr::Instance().OpenLocalSecondVideoCapDev(this,nWidth,nHeight,nFrameRate,codecType,nBitrate);
	}

	return -1;
}

void MediaCall::OnRequestSecondVideoKeyFrame(void)
{
	if (m_pSecondVideoCapChan!=NULL)
	{
		m_pSecondVideoCapChan->RequestKeyFrame();
	}
	else
	{
		CallMgr::Instance().OnRequestSecondVideoKeyFrame(this);
	}
}

void MediaCall::CloseLocalSecondVideoCapDev(void)
{
	CallMgr::Instance().CloseLocalSecondVideoCapDev(this);

	ObjectClose(VIDEC_CapChanRTP* ,m_pSecondVideoCapChan)

	//if (m_pSecondVideoCapChan!=NULL)
	//{
	//	m_pSecondVideoCapChan->Close();
	//	delete m_pSecondVideoCapChan;
	//	m_pSecondVideoCapChan=NULL;
	//}

	ObjectClose(VIDEC_CapDev* ,m_pSecondVideoCapDev)
	//if (m_pSecondVideoCapDev!=NULL)
	//{
	//	m_pSecondVideoCapDev->Close();
	//	delete m_pSecondVideoCapDev;
	//	m_pSecondVideoCapDev=NULL;
	//}

	ObjectClose(VIDEC_MPScreen* ,m_pSecondVideoMPScreen)
	//if (m_pSecondVideoMPScreen!=NULL)
	//{
	//	m_pSecondVideoMPScreen->Close();
	//	delete m_pSecondVideoMPScreen;
	//	m_pSecondVideoMPScreen=NULL;
	//}
}


int MediaCall::OpenRemoteSecondVideoPlayChan(VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType)
{
	if (nPayloadType==34)
		codecType=VIDEC_CODEC_H263;

	m_nSecondVideoCodecType=codecType;

	if (HasAVCONTransport())
	{
		if (m_pMediaSenderRemoteSecond!=NULL)
		{
			m_pMediaSenderRemoteSecond->StartVideo(0);
		}
		return 0;
	}

	if (m_pRTPJBufSecondVideo==NULL)
	{
		//if(CallMgr::Instance().m_nUseNETECSend > 0 )
		if( 0 )
			m_pRTPJBufSecondVideo=NETEC_RTPJBuf::Create(*this);
		if (m_pRTPJBufSecondVideo)
		{
			if (0!=m_pRTPJBufSecondVideo->Open())
			{
				m_pRTPJBufSecondVideo->Close();
				delete m_pRTPJBufSecondVideo;
				m_pRTPJBufSecondVideo=NULL;
			}
		}
	}

	IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
	CallMgr::Instance().GetCallType(nCallType);
	if (nCallType==IMXEC_MP_CALL ||
		nCallType==IMXEC_MCU_CALL)
	{
		if (IMXEC_Call::GetEnableVideoTranscoding()==0)
		{
			if (m_pRTP2FrameRemoteSecond==NULL)
			{
				switch (codecType)
				{
				case VIDEC_CODEC_H263://H.263
					m_pRTP2FrameRemoteSecond=NETEC_RTP2Frame::Create(*this,"H263");
					break;
				case VIDEC_CODEC_H263P://H.263P
					m_pRTP2FrameRemoteSecond=NETEC_RTP2Frame::Create(*this,"H263-1998");
					break;
				case VIDEC_CODEC_H263PP://H.263PP
					m_pRTP2FrameRemoteSecond=NETEC_RTP2Frame::Create(*this,"H263-2000");
					break;
					//notice 20120116 update
				case VIDEC_CODEC_H264://H.264
				case VIDEC_CODEC_H264_SVC://H.264
					m_pRTP2FrameRemoteSecond=NETEC_RTP2Frame::Create(*this,"H264");
					break;
				case VIDEC_CODEC_MPEG4://MPEG4 ,use to call kedacom
					m_pRTP2FrameRemoteSecond=NETEC_RTP2Frame::Create(*this,"MPEG4");
					break;
				}

				if (m_pRTP2FrameRemoteSecond!=0)
				{
					if (0!=m_pRTP2FrameRemoteSecond->Open(VIDEC_HEADER_EXT_KEY_FRAME_LEN))
					{
						m_pRTP2FrameRemoteSecond->Close();
						delete m_pRTP2FrameRemoteSecond;
						m_pRTP2FrameRemoteSecond=NULL;
					}
					else
					{
						if (m_pMediaSenderRemoteSecond!=NULL)
						{
							m_pMediaSenderRemoteSecond->StartVideo(0);
						}
					}
				}
			}
			return 0;
		}
	}

	if (m_pSecondVideoPlayChan==NULL)
	{
		switch (codecType)
		{
		case VIDEC_CODEC_H261://H.261
			nPayloadType=31;
			break;
		case VIDEC_CODEC_H263://H.263
			nPayloadType=34;
			break;
		case VIDEC_CODEC_H263P://H.263
			break;
		case VIDEC_CODEC_H263PP://H.263
			break;
			//notice 20120116 update
		case VIDEC_CODEC_H264://H.264
		case VIDEC_CODEC_H264_SVC://H.264
		case VIDEC_CODEC_MPEG4://MPEG4,use to call kedacom
			break;
		}

		if (nPayloadType!=0)
		{
			VIDEC_PlayChanRTP*pVideoPlayChan=VIDEC_PlayChanRTP::Create();
			if (pVideoPlayChan!=NULL)
			{
				if (0!=pVideoPlayChan->Open(nPayloadType,codecType))
				{
					pVideoPlayChan->Close();
					delete pVideoPlayChan;
					pVideoPlayChan=NULL;
				}
				else
				{
					m_pSecondVideoPlayChan=pVideoPlayChan;

					if (m_pMediaSenderRemoteSecond!=NULL)
					{
						m_pMediaSenderRemoteSecond->StartVideo(0);
					}

					if (IMXEC_Call::GetEnableVideoTranscoding()!=0)
					{
						if (m_pSecondVideoTranscoder!=NULL)
						{
							m_pSecondVideoTranscoder->Close();
							delete m_pSecondVideoTranscoder;
							m_pSecondVideoTranscoder=NULL;
						}
						m_pSecondVideoTranscoder=VIDEC_CapChan::Create(*this);
						if (m_pSecondVideoTranscoder!=NULL)
						{
							if (0!=m_pSecondVideoTranscoder->Open(nBitrate,VIDEC_NETWORK_PRIVATE,VIDEC_CODEC_H264_SVC,0))
							{
								m_pSecondVideoTranscoder->Close();
								delete m_pSecondVideoTranscoder;
								m_pSecondVideoTranscoder=NULL;
							}
							else
							{
								m_pSecondVideoPlayChan->AddImageSink(m_pSecondVideoTranscoder);
								m_pSecondVideoTranscoder->AddStreamSink(&m_RemoteSecondPlayChanRTP);
							}
						}
					}


					m_LocalSecondCapChanRTP.StopRecordToAVI();
					m_RemoteSecondPlayChanRTP.StopRecordToAVI();
					if (m_strSecondRecordPathName.size())
					{
						std::string strPathName=m_strSecondRecordPathName;
						if (m_nSecondRecordIndex>0)
						{
							char szFileIndex[128]="";
							sprintf(szFileIndex,"(%u)",m_nSecondRecordIndex);
							std::string strExt="";
							int nPos=strPathName.find_last_of(".");
							if (nPos>0)
							{
								strExt=strPathName.substr(nPos);
								strPathName=strPathName.substr(0,nPos);
								strPathName+=szFileIndex;
								strPathName+=strExt;
							}
						}
						m_nSecondRecordIndex++;
						m_RemoteSecondPlayChanRTP.StartRecordToAVI(strPathName.c_str(),1);
					}

					if (m_pSecondVideoTranscoder==NULL)
					{
						m_pSecondVideoPlayChan->AddStreamSink(&m_RemoteSecondPlayChanRTP);
					}
					
					if (m_pSecondVideoWnd!=NULL)
					{
						m_pSecondVideoPlayChan->AddImageSink(m_pSecondVideoWnd);
					}
				}
			}
		}
	}

	return (m_pSecondVideoPlayChan!=NULL?0:-1);
}

void MediaCall::OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	if (m_pRTPJBufSecondVideo)
	{
		m_pRTPJBufSecondVideo->OnRecvdPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
	else
	{
		OnRecvdRemoteSecondVideoRTPPacket(pRTPPacketData,nRTPPacketLen);
	}
}

void MediaCall::OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen)
{
	m_XBitrateSecondVideo.Calculate(nRTPPacketLen);

	if (m_pSecondVideoPlayChan!=NULL)
	{
		m_pSecondVideoPlayChan->OnRecvdRTPPacket((unsigned char*)pRTPPacketData,nRTPPacketLen);
	}

	if (m_pRTP2FrameRemoteSecond!=NULL)
	{
		m_pRTP2FrameRemoteSecond->OnRecvdRTPPacket((unsigned char*)pRTPPacketData,nRTPPacketLen);
	}
}

void MediaCall::CloseRemoteSecondVideoPlayChan(void)
{
	ObjectClose(VIDEC_PlayChanRTP* ,m_pSecondVideoPlayChan)

	//if (m_pSecondVideoPlayChan!=NULL)
	//{
	//	m_pSecondVideoPlayChan->Close();
	//	delete m_pSecondVideoPlayChan;
	//	m_pSecondVideoPlayChan=NULL;
	//}
	ObjectClose(NETEC_RTP2Frame* ,m_pRTP2FrameRemoteSecond)

	//if (m_pRTP2FrameRemoteSecond!=NULL)
	//{
	//	m_pRTP2FrameRemoteSecond->Close();
	//	delete m_pRTP2FrameRemoteSecond;
	//	m_pRTP2FrameRemoteSecond=NULL;
	//}

	ObjectClose(VIDEC_CapChan* ,m_pSecondVideoTranscoder)

	//if (m_pSecondVideoTranscoder!=NULL)
	//{
	//	m_pSecondVideoTranscoder->Close();
	//	delete m_pSecondVideoTranscoder;
	//	m_pSecondVideoTranscoder=NULL;
	//}

	ObjectClose(NETEC_RTPJBuf* ,m_pRTPJBufSecondVideo)
	//if (m_pRTPJBufSecondVideo!=NULL)
	//{
	//	m_pRTPJBufSecondVideo->Close();
	//	delete m_pRTPJBufSecondVideo;
	//	m_pRTPJBufSecondVideo=NULL;
	//}
}

void MediaCall::OnNETEC_MediaSenderCallbackRecvdPTZ(void*pData,int nLen)
{
	//notice netec send ptz 
	OnNETEC_RecvdPTZ(pData,nLen);
}

void MediaCall::OnNETEC_MediaSenderCallbackRecvdCtrl(void*pData,int nLen)
{
	//notice netec send ptz 
	OnNETEC_RecvdCtrl(pData,nLen);
}

//add by Jan for DTMF [20140730]
void MediaCall::OnRecvDTMF(char dtmf)
{
	//这里是消息入口
	OnAudioRecvDTMF(dtmf);
}
//end [20140730]

//nSkipFlag,1:跳过这帧不压缩，0:压缩当前帧
void MediaCall::OnVIDEC_CapChanCallbackPreEncode(VIDEC_CapChan*pCapChan,int&nSkipFlag)
{
	nSkipFlag=1;

	if (m_pMainVideoTranscoder==pCapChan)
	{
		if (m_pMediaSenderRemoteMain!=NULL)
		{
			nSkipFlag=(m_pMediaSenderRemoteMain->IsVideoBlocking(0)?1:0);
			if (nSkipFlag==0)
			{
				if (m_pMediaSenderRemoteMain->IsVideoRequestKeyFrame(0))
				{
					m_pMainVideoTranscoder->RequestKeyFrame();
				}
			}

			if (nSkipFlag!=0)
				nSkipFlag=(m_pMediaSenderRemoteMain->IsVideoBlocking(1)?1:0);
			if (nSkipFlag==0)
			{
				if (m_pMediaSenderRemoteMain->IsVideoRequestKeyFrame(1))
				{
					m_pMainVideoTranscoder->RequestKeyFrame();
				}
			}

			if (nSkipFlag!=0)
				nSkipFlag=(m_pMediaSenderRemoteMain->IsVideoBlocking(2)?1:0);
			if (nSkipFlag==0)
			{
				if (m_pMediaSenderRemoteMain->IsVideoRequestKeyFrame(2))
				{
					m_pMainVideoTranscoder->RequestKeyFrame();
				}
			}
		}

	}
	else if (pCapChan==m_pSecondVideoTranscoder)
	{
		if (m_pMediaSenderRemoteSecond!=NULL)
		{
			nSkipFlag=(m_pMediaSenderRemoteSecond->IsVideoBlocking(0)?1:0);
			if (nSkipFlag==0)
			{
				if (m_pMediaSenderRemoteSecond->IsVideoRequestKeyFrame(0))
				{
					m_pSecondVideoTranscoder->RequestKeyFrame();
				}
			}

			if (nSkipFlag!=0)
				nSkipFlag=(m_pMediaSenderRemoteSecond->IsVideoBlocking(1)?1:0);
			if (nSkipFlag==0)
			{
				if (m_pMediaSenderRemoteSecond->IsVideoRequestKeyFrame(1))
				{
					m_pSecondVideoTranscoder->RequestKeyFrame();
				}
			}

			if (nSkipFlag!=0)
				nSkipFlag=(m_pMediaSenderRemoteSecond->IsVideoBlocking(2)?1:0);
			if (nSkipFlag==0)
			{
				if (m_pMediaSenderRemoteSecond->IsVideoRequestKeyFrame(2))
				{
					m_pSecondVideoTranscoder->RequestKeyFrame();
				}
			}
		}
	}
}


/******************************************************************************
* StartRecordLocalMainToAVI
*描述：开始本地主流录像
*输入：	cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::StartRecordLocalMainToAVI(const char*cszPathName,int nStandardFormat)
{
	m_bRecording=true;
	return m_LocalMainCapChanRTP.StartRecordToAVI(cszPathName,nStandardFormat);
}

/******************************************************************************
* StopRecordLocalMainToAVI
*描述：停止本地主流录像
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::StopRecordLocalMainToAVI(void)
{
	m_bRecording=false;
	m_LocalMainCapChanRTP.StopRecordToAVI();
}

/******************************************************************************
* StartRecordRemoteMainToAVI
*描述：开始远端主流录像
*输入：	cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::StartRecordRemoteMainToAVI(const char*cszPathName,int nStandardFormat)
{
	if (HasAVCONTransport())
	{
		if (m_pRemoteMainPlayChan)
		{
			return m_pRemoteMainPlayChan->StartRecordToAVI(cszPathName,nStandardFormat);
		}

		return -1;
	}

	return m_RemoteMainPlayChanRTP.StartRecordToAVI(cszPathName,nStandardFormat);
}

/******************************************************************************
* StopRecordRemoteMainToAVI
*描述：停止远端主流录像
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::StopRecordRemoteMainToAVI(void)
{
	if (HasAVCONTransport())
	{
		if (m_pRemoteMainPlayChan)
		{
			m_pRemoteMainPlayChan->StopRecordToAVI();
		}
		return ;
	}
			
	m_RemoteMainPlayChanRTP.StopRecordToAVI();
}

/******************************************************************************
* StartRecordSecondToAVI
*描述：开始辅流录像
*输入：	cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::StartRecordSecondToAVI(const char*cszPathName,int nStandardFormat)
{
	if (cszPathName==NULL)
	{
		return -1;
	}

	m_strSecondRecordPathName=cszPathName;
	m_nSecondRecordIndex=0;

	IMXEC_CALL_DIRECTION nDir=IMXEC_CALL_DIR_UNKOWN;
	GetSecondVideoDirection(nDir);

	if (nDir==IMXEC_CALL_DIR_IN)
	{
		if (HasAVCONTransport())
		{
			if (m_pRemoteSecondPlayChan)
			{
				m_pRemoteSecondPlayChan++;
				return m_pRemoteSecondPlayChan->StartRecordToAVI(cszPathName,nStandardFormat);
			}

			return -1;
		}

		if (m_pSecondVideoPlayChan!=NULL)
		{
			m_nSecondRecordIndex++;
			return m_RemoteSecondPlayChanRTP.StartRecordToAVI(cszPathName,nStandardFormat);
		}
	}
	else if (nDir==IMXEC_CALL_DIR_OUT)
	{
		if (m_pSecondVideoCapChan!=NULL)
		{
			m_nSecondRecordIndex++;
			return m_LocalSecondCapChanRTP.StartRecordToAVI(cszPathName,nStandardFormat);
		}
	}

	return 0;
}

/******************************************************************************
* StopRecordSecondToAVI
*描述：停止辅流录像
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::StopRecordSecondToAVI(void)
{
	m_strSecondRecordPathName="";

	if (HasAVCONTransport())
	{
		if (m_pRemoteSecondPlayChan)
		{
			m_pRemoteSecondPlayChan->StopRecordToAVI();
		}
		return ;
	}

	m_LocalSecondCapChanRTP.StopRecordToAVI();
	m_RemoteSecondPlayChanRTP.StopRecordToAVI();
}

/******************************************************************************
* GetMainAudioID
*描述：获取主流音频标识
*输入： 无
*输出： nMainAudioID	-主流音频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetMainAudioID(unsigned long&nMainAudioID)
{
	if (m_pMediaSenderRemoteMain!=NULL)
	{
		return m_pMediaSenderRemoteMain->GetAudioID((uint32_t &)nMainAudioID);
	}
	return -1;
}

int MediaCall::GetLocalMainAudioID(unsigned long&nMainAudioID)
{
	if (m_pMediaSenderLocalMain!=NULL)
	{
		return m_pMediaSenderLocalMain->GetAudioID((uint32_t &)nMainAudioID);
	}
	return -1;
}

/******************************************************************************
* GetMainVideoID
*描述：获取主流视频标识
*输入： 无
*输出： nMainVideoID	-主流视频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetMainVideoID(unsigned long&nMainVideoID)
{
	if (m_pMediaSenderRemoteMain!=NULL)
	{
		return m_pMediaSenderRemoteMain->GetVideoID((uint32_t &)nMainVideoID);
	}
	return -1;
}

int MediaCall::GetLocalMainVideoID(unsigned long&nMainVideoID)
{
	if (m_pMediaSenderLocalMain!=NULL)
	{
		return m_pMediaSenderLocalMain->GetVideoID((uint32_t &)nMainVideoID);
	}
	return -1;
}

/******************************************************************************
* GetSecondVideoID
*描述：获取辅流视频标识
*输入： 无
*输出： nSecondVideoID	-辅流视频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::GetSecondVideoID(unsigned long&nSecondVideoID)
{
	if (m_pMediaSenderRemoteSecond!=NULL)
	{
		return m_pMediaSenderRemoteSecond->GetVideoID((uint32_t &)nSecondVideoID);
	}
	return -1;
}

int MediaCall::GetLocalSecondVideoID(unsigned long&nSecondVideoID)
{
	if (m_pMediaSenderLocalSecond!=NULL)
	{
		return m_pMediaSenderLocalSecond->GetVideoID((uint32_t &)nSecondVideoID);
	}
	return -1;
}

void MediaCall::OnCapChanRTPCallbackSendVideo(CapChanRTP*pCapChanRTP,void*pPacketData,int nPacketLen)
{
	if (m_nMainVideoFlowCCBitRate >0 && pCapChanRTP == &m_LocalMainCapChanRTP)
	{
		if (m_pRTP2FrameLocalMain!=NULL)
		{
			m_pRTP2FrameLocalMain->OnRecvdRTPPacket((unsigned char*)pPacketData,nPacketLen);
		}

		SendMainVideo((char*)pPacketData,nPacketLen);

		return ;
	}
	else if ( m_nDualVideoFlowCCBitRate >0 && pCapChanRTP == &m_LocalSecondCapChanRTP)
	{
		if (m_pRTP2FrameLocalSecond!=NULL)
		{
			m_pRTP2FrameLocalSecond->OnRecvdRTPPacket((unsigned char*)pPacketData,nPacketLen);
		}

		SendSecondVideo((char*)pPacketData,nPacketLen);

		return ;
	}
}

void MediaCall::OnCapChanRTPCallbackSendAudio(CapChanRTP*pCapChanRTP,void*pPacketData,int nPacketLen)
{
	if (m_nAudioFlowCCBitRate >0 && pCapChanRTP==&m_LocalMainCapChanRTP)
	{
		if (HasAVCONTransport())
		{
			if (m_pMediaSenderLocalMain )
			{
				m_pMediaSenderLocalMain->SendAudio((unsigned char*)pPacketData,nPacketLen);
			}
		}
		else
		{
			int nRTPHeaderLen=12;
			char*pData=(char*)pPacketData;
			int nLen=nPacketLen;


			if (m_nLocalAudioCapChanBufLen<(nLen+nRTPHeaderLen) ||
				m_pLocalAudioCapChanBuf==NULL)
			{
				if (m_pLocalAudioCapChanBuf!=NULL)
				{
					free(m_pLocalAudioCapChanBuf);
					m_pLocalAudioCapChanBuf=NULL;
				}

				m_nLocalAudioCapChanBufLen=(nLen+nRTPHeaderLen)*2;
				if (m_nLocalAudioCapChanBufLen<1500)
				{
					m_nLocalAudioCapChanBufLen=1500;
				}

				m_pLocalAudioCapChanBuf=(unsigned char*)malloc(m_nLocalAudioCapChanBufLen);
				if (m_pLocalAudioCapChanBuf==NULL)
				{
					return ;
				}
			}

			unsigned short usSequence=AUDEC_HEADER_GET_SEQUENCE(pData);
			unsigned char ucAudioCodec=AUDEC_HEADER_GET_CODEC_ID(pData);
			int nPayloadSize=nLen-AUDEC_HEADER_VER_01_LEN;
			char*pPayloadData=(char*)pData+AUDEC_HEADER_VER_01_LEN;
			unsigned short nDuration=160;
			m_ulLocalAudioCapChanRTPTimestamp+=nDuration;

			PRTP_HEADER pRTPHeader=(PRTP_HEADER)m_pLocalAudioCapChanBuf;
			memset(pRTPHeader,0,RTP_HEADER_LEN);
			pRTPHeader->version=RTP_VERSION;
			pRTPHeader->ssrc=htonl((unsigned long)this);
			pRTPHeader->timestamp=htonl(m_ulLocalAudioCapChanRTPTimestamp);
			pRTPHeader->sequence=htons(usSequence);
			pRTPHeader->type=m_nLocalAudioCapChanRTPPayloadType;
			memcpy(m_pLocalAudioCapChanBuf+RTP_HEADER_LEN,pPayloadData,nPayloadSize);

			SendAudio((char*)m_pLocalAudioCapChanBuf,nPayloadSize+RTP_HEADER_LEN);
			//added by jiangdingfeng 环回测试
			/*m_nRemoteAudioPlayChanCodecID = m_nLocalAudioCodecID;
			MediaCall::OnRecvdRemoteAudioRTPPacket((unsigned char*)m_pLocalAudioCapChanBuf,nPayloadSize+RTP_HEADER_LEN
				                        , m_nLocalAudioCapChanRTPPayloadType, RTP_HEADER_LEN, pRTPHeader->sequence, pRTPHeader->timestamp);*/

		}

		{
			XAutoLock l(m_csLocalRecordCallback);
			if (m_pLocalRecordCallback!=NULL)
			{
				m_pLocalRecordCallback->OnIMXEC_RecordCallbackAudioData((unsigned char*)pPacketData,nPacketLen);
			}
		}
		return ;
	}
}

void MediaCall::OnPlayChanRTPCallbackVideoPacket(PlayChanRTP*pPlayChanRTP,void*pPacketData,int nPacketLen)
{
	if (pPlayChanRTP==&m_RemoteMainPlayChanRTP)
	{
		if (m_pMediaSenderRemoteMain!=NULL)
		{
			if (m_pMediaSenderRemoteMain->IsVideoRequestKeyFrame(0) || 
				m_pMediaSenderRemoteMain->IsVideoRequestKeyFrame(1) || 
				m_pMediaSenderRemoteMain->IsVideoRequestKeyFrame(2))
			{
				//RequestMainVideoKeyFrame();
				OnNETECRequestMainVideoKeyFrame();
			}

			//char szDebug[1024];
			//sprintf(szDebug,"seq=%u ts=%u now=%u\n",VIDEC_HEADER_EXT_GET_SEQUENCE(pPacketData),VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketData),XGetTimestamp());
			//OutputDebugString(szDebug);

			VIDEC_HEADER_EXT_SET_MAIN_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_SUB_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_QSUB_FRAME(pPacketData,1);
			if(m_pMediaSenderRemoteMain) m_pMediaSenderRemoteMain->SendVideo((unsigned char*)pPacketData,nPacketLen);
		}
		{
			XAutoLock l(m_csRemoteRecordCallback);
			if (m_pRemoteRecordCallback!=NULL)
			{
				int nKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData);
				m_pRemoteRecordCallback->OnIMXEC_RecordCallbackVideoData((unsigned char*)pPacketData,nPacketLen,nKeyFrame);
			}
		}
	}
	else if (pPlayChanRTP==&m_RemoteSecondPlayChanRTP)
	{
		if (m_pMediaSenderRemoteSecond!=NULL)
		{
			if (m_pMediaSenderRemoteSecond->IsVideoRequestKeyFrame(0) || 
				m_pMediaSenderRemoteSecond->IsVideoRequestKeyFrame(1) || 
				m_pMediaSenderRemoteSecond->IsVideoRequestKeyFrame(2))
			{
				//RequestSecondVideoKeyFrame();
				OnNETECRequestSecondVideoKeyFrame();
			}

			VIDEC_HEADER_EXT_SET_MAIN_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_SUB_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_QSUB_FRAME(pPacketData,1);
			if(m_pMediaSenderRemoteSecond) m_pMediaSenderRemoteSecond->SendVideo((unsigned char*)pPacketData,nPacketLen);
		}
		{
			XAutoLock l(m_csSecondRecordCallback);
			if (m_pSecondRecordCallback!=NULL)
			{
				int nKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData);
				m_pSecondRecordCallback->OnIMXEC_RecordCallbackVideoData((unsigned char*)pPacketData,nPacketLen,nKeyFrame);
			}
		}
	}
}

void MediaCall::OnNETEC_RTP2FrameCallbackFramePacket(NETEC_RTP2Frame*pRTP2Frame,unsigned char*pPacketData,int nPacketLen,int nKeyFrame)
{
	if (pRTP2Frame==m_pRTP2FrameRemoteMain)
	{
		int nHeaderLen=(nKeyFrame!=0)?VIDEC_HEADER_EXT_KEY_FRAME_LEN:VIDEC_HEADER_EXT_LEN;
		pPacketData-=nHeaderLen;
		nPacketLen+=nHeaderLen;

		VIDEC_HEADER_EXT_RESET(pPacketData);
		VIDEC_HEADER_EXT_SET_CODEC_ID(pPacketData,m_nRemoteMainVideoCodecType);
		VIDEC_HEADER_EXT_SET_EVEN_FIELD_FIRST(pPacketData,0);
		VIDEC_HEADER_EXT_SET_DOUBLE_FIELD(pPacketData,0);
		VIDEC_HEADER_EXT_SET_HARDWARE(pPacketData,0);
		VIDEC_HEADER_EXT_SET_KEYFRAME(pPacketData,(nKeyFrame!=0?1:0));
		VIDEC_HEADER_EXT_SET_BFRAME(pPacketData,0);

		VIDEC_HEADER_EXT_SET_SEQUENCE(pPacketData,m_nSeqNoRemoteMain++);
		VIDEC_HEADER_EXT_SET_TIMESTAMP(pPacketData,XGetTimestamp());
		if (nKeyFrame)
		{
			VIDEC_HEADER_EXT_SET_ACTUAL_WIDTH(pPacketData,0);
			VIDEC_HEADER_EXT_SET_ACTUAL_HEIGHT(pPacketData,0);
			VIDEC_HEADER_EXT_SET_VIRTUAL_WIDTH(pPacketData,0);
			VIDEC_HEADER_EXT_SET_VIRTUAL_HEIGHT(pPacketData,0);
		}

		if (m_pMediaSenderRemoteMain!=NULL)
		{
			if (m_pMediaSenderRemoteMain->IsVideoRequestKeyFrame(0) || 
				m_pMediaSenderRemoteMain->IsVideoRequestKeyFrame(1) || 
				m_pMediaSenderRemoteMain->IsVideoRequestKeyFrame(2))
			{
				//RequestMainVideoKeyFrame();
				OnNETECRequestMainVideoKeyFrame();
			}

			VIDEC_HEADER_EXT_SET_MAIN_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_SUB_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_QSUB_FRAME(pPacketData,1);
			if(m_pMediaSenderRemoteMain) m_pMediaSenderRemoteMain->SendVideo((unsigned char*)pPacketData,nPacketLen);
		}
		{
			XAutoLock l(m_csRemoteRecordCallback);
			if (m_pRemoteRecordCallback!=NULL)
			{
				int nKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData);
				m_pRemoteRecordCallback->OnIMXEC_RecordCallbackVideoData((unsigned char*)pPacketData,nPacketLen,nKeyFrame);
			}
		}
	}
	else if (pRTP2Frame==m_pRTP2FrameRemoteSecond)
	{
		int nHeaderLen=(nKeyFrame!=0)?VIDEC_HEADER_EXT_KEY_FRAME_LEN:VIDEC_HEADER_EXT_LEN;
		pPacketData-=nHeaderLen;
		nPacketLen+=nHeaderLen;

		VIDEC_HEADER_EXT_RESET(pPacketData);
		VIDEC_HEADER_EXT_SET_CODEC_ID(pPacketData,m_nSecondVideoCodecType);
		VIDEC_HEADER_EXT_SET_EVEN_FIELD_FIRST(pPacketData,0);
		VIDEC_HEADER_EXT_SET_DOUBLE_FIELD(pPacketData,0);
		VIDEC_HEADER_EXT_SET_HARDWARE(pPacketData,0);
		VIDEC_HEADER_EXT_SET_KEYFRAME(pPacketData,(nKeyFrame!=0?1:0));
		VIDEC_HEADER_EXT_SET_BFRAME(pPacketData,0);

		VIDEC_HEADER_EXT_SET_SEQUENCE(pPacketData,m_nSeqNoRemoteSecond++);
		VIDEC_HEADER_EXT_SET_TIMESTAMP(pPacketData,XGetTimestamp());
		if (nKeyFrame)
		{
			VIDEC_HEADER_EXT_SET_ACTUAL_WIDTH(pPacketData,0);
			VIDEC_HEADER_EXT_SET_ACTUAL_HEIGHT(pPacketData,0);
			VIDEC_HEADER_EXT_SET_VIRTUAL_WIDTH(pPacketData,0);
			VIDEC_HEADER_EXT_SET_VIRTUAL_HEIGHT(pPacketData,0);
		}

		if (m_pMediaSenderRemoteSecond!=NULL)
		{
			if (m_pMediaSenderRemoteSecond->IsVideoRequestKeyFrame(0) || 
				m_pMediaSenderRemoteSecond->IsVideoRequestKeyFrame(1) || 
				m_pMediaSenderRemoteSecond->IsVideoRequestKeyFrame(2))
			{
				//RequestSecondVideoKeyFrame();
				OnNETECRequestSecondVideoKeyFrame();
			}

			VIDEC_HEADER_EXT_SET_MAIN_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_SUB_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_QSUB_FRAME(pPacketData,1);
			if(m_pMediaSenderRemoteSecond) m_pMediaSenderRemoteSecond->SendVideo((unsigned char*)pPacketData,nPacketLen);
		}
		{
			XAutoLock l(m_csSecondRecordCallback);
			if (m_pSecondRecordCallback!=NULL)
			{
				int nKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData);
				m_pSecondRecordCallback->OnIMXEC_RecordCallbackVideoData((unsigned char*)pPacketData,nPacketLen,nKeyFrame);
			}
		}
	}
	else if (pRTP2Frame==m_pRTP2FrameLocalMain)
	{
		int nHeaderLen=(nKeyFrame!=0)?VIDEC_HEADER_EXT_KEY_FRAME_LEN:VIDEC_HEADER_EXT_LEN;
		pPacketData-=nHeaderLen;
		nPacketLen+=nHeaderLen;

		VIDEC_HEADER_EXT_RESET(pPacketData);
		VIDEC_HEADER_EXT_SET_CODEC_ID(pPacketData,m_nLocalMainVideoCodecType);
		VIDEC_HEADER_EXT_SET_EVEN_FIELD_FIRST(pPacketData,0);
		VIDEC_HEADER_EXT_SET_DOUBLE_FIELD(pPacketData,0);
		VIDEC_HEADER_EXT_SET_HARDWARE(pPacketData,0);
		VIDEC_HEADER_EXT_SET_KEYFRAME(pPacketData,(nKeyFrame!=0?1:0));
		VIDEC_HEADER_EXT_SET_BFRAME(pPacketData,0);

		VIDEC_HEADER_EXT_SET_SEQUENCE(pPacketData,m_nSeqNoLocalMain++);
		VIDEC_HEADER_EXT_SET_TIMESTAMP(pPacketData,XGetTimestamp());
		if (nKeyFrame)
		{
			VIDEC_HEADER_EXT_SET_ACTUAL_WIDTH(pPacketData,0);
			VIDEC_HEADER_EXT_SET_ACTUAL_HEIGHT(pPacketData,0);
			VIDEC_HEADER_EXT_SET_VIRTUAL_WIDTH(pPacketData,0);
			VIDEC_HEADER_EXT_SET_VIRTUAL_HEIGHT(pPacketData,0);
		}

		if (m_pMediaSenderLocalMain!=NULL)
		{
			if (m_pMediaSenderLocalMain->IsVideoRequestKeyFrame(0) || 
				m_pMediaSenderLocalMain->IsVideoRequestKeyFrame(1) || 
				m_pMediaSenderLocalMain->IsVideoRequestKeyFrame(2))
			{
				//RequestMainVideoKeyFrame();
				//OnRequestLocalMainVideoKeyFrame();
				OnNETECRequestMainVideoKeyFrame();
			}

			VIDEC_HEADER_EXT_SET_MAIN_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_SUB_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_QSUB_FRAME(pPacketData,1);
			if(m_pMediaSenderLocalMain) m_pMediaSenderLocalMain->SendVideo((unsigned char*)pPacketData,nPacketLen);
		}
	}
	else if (pRTP2Frame==m_pRTP2FrameLocalSecond)
	{
		int nHeaderLen=(nKeyFrame!=0)?VIDEC_HEADER_EXT_KEY_FRAME_LEN:VIDEC_HEADER_EXT_LEN;
		pPacketData-=nHeaderLen;
		nPacketLen+=nHeaderLen;

		VIDEC_HEADER_EXT_RESET(pPacketData);
		VIDEC_HEADER_EXT_SET_CODEC_ID(pPacketData,m_nSecondVideoCodecType);
		VIDEC_HEADER_EXT_SET_EVEN_FIELD_FIRST(pPacketData,0);
		VIDEC_HEADER_EXT_SET_DOUBLE_FIELD(pPacketData,0);
		VIDEC_HEADER_EXT_SET_HARDWARE(pPacketData,0);
		VIDEC_HEADER_EXT_SET_KEYFRAME(pPacketData,(nKeyFrame!=0?1:0));
		VIDEC_HEADER_EXT_SET_BFRAME(pPacketData,0);

		VIDEC_HEADER_EXT_SET_SEQUENCE(pPacketData,m_nSeqNoLocalSecond++);
		VIDEC_HEADER_EXT_SET_TIMESTAMP(pPacketData,XGetTimestamp());
		if (nKeyFrame)
		{
			VIDEC_HEADER_EXT_SET_ACTUAL_WIDTH(pPacketData,0);
			VIDEC_HEADER_EXT_SET_ACTUAL_HEIGHT(pPacketData,0);
			VIDEC_HEADER_EXT_SET_VIRTUAL_WIDTH(pPacketData,0);
			VIDEC_HEADER_EXT_SET_VIRTUAL_HEIGHT(pPacketData,0);
		}

		if (m_pMediaSenderLocalSecond!=NULL)
		{
			if (m_pMediaSenderLocalSecond->IsVideoRequestKeyFrame(0) || 
				m_pMediaSenderLocalSecond->IsVideoRequestKeyFrame(1) || 
				m_pMediaSenderLocalSecond->IsVideoRequestKeyFrame(2))
			{
				//RequestSecondVideoKeyFrame();
				//OnRequestSecondVideoKeyFrame();
				OnNETECRequestSecondVideoKeyFrame();
			}

			VIDEC_HEADER_EXT_SET_MAIN_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_SUB_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_QSUB_FRAME(pPacketData,1);
			if(m_pMediaSenderLocalSecond) m_pMediaSenderLocalSecond->SendVideo((unsigned char*)pPacketData,nPacketLen);
		}
	}
}


void MediaCall::OnNETEC_RTPJBufCallbackRTPPacket(NETEC_RTPJBuf*pRTPJBuf,unsigned char*pRTPPacketData,int32_t nRTPPacketLen,int32_t nPayloadType,int32_t nRTPHeaderLen,unsigned short nSeqNo,uint32_t nTimestamp)
{
	if (pRTPJBuf!=NULL)
	{
		if (pRTPJBuf==m_pRTPJBufMainVideo)
		{
			OnRecvdRemoteMainVideoRTPPacket(pRTPPacketData,nRTPPacketLen);
		}
		else if (pRTPJBuf==m_pRTPJBufSecondVideo)
		{
			OnRecvdRemoteSecondVideoRTPPacket(pRTPPacketData,nRTPPacketLen);
		}
		else if (pRTPJBuf==m_pRTPJBufAudio)
		{
			OnRecvdRemoteAudioRTPPacketJBuf(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,nSeqNo,nTimestamp);
		}
	}
}


/******************************************************************************
* SetMainVideoScreenIndex
*描述：设置主流视频图像桌面序号
*输入：nScreenIndex	-桌面序号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::SetMainVideoScreenIndex(int nScreenIndex)
{
	m_nMainVideoScreenIndex=nScreenIndex;
	if (m_pMainVideoMPScreen!=NULL)
	{
		return m_pMainVideoMPScreen->SetScreenIndex(m_nMainVideoScreenIndex);
	}

	return -1;
}

/******************************************************************************
* SetSecondVideoScreenIndex
*描述：设置辅流视频图像桌面序号
*输入： cszCallID		-呼叫标识
		nScreenIndex	-桌面序号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::SetSecondVideoScreenIndex(int nScreenIndex)
{
	m_nSecondVideoScreenIndex=nScreenIndex;
	if (m_pSecondVideoMPScreen!=NULL)
	{
		return m_pSecondVideoMPScreen->SetScreenIndex(m_nSecondVideoScreenIndex);
	}
	return -1;
}


/******************************************************************************
* SetSecondVideoScreenWnd
*描述：设置辅流视频所在屏幕窗口位置
*输入：	nScreenIndex	-屏幕号
		nWndIndex		-窗口号
		nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetSecondVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	if (m_pSecondVideoWnd!=NULL)
	{
		m_pSecondVideoWnd->SetScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
}

/******************************************************************************
* SetRemoteVideoScreenWnd
*描述：设置远端视频所在屏幕窗口位置
*输入：	nScreenIndex	-屏幕号
		nWndIndex		-窗口号
		nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetRemoteVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	if (HasAVCONTransport())
	{
		if (m_pRemoteMainPlayChan)
		{
			m_pRemoteMainPlayChan->SetScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
		}
		return ;
	}
	if (m_pRemoteMainVideoWnd!=NULL)
	{
		m_pRemoteMainVideoWnd->SetScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
}

/******************************************************************************
* SetLocalRecordCallback
*描述：设置本地录像回调
*输入：	pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetLocalRecordCallback(IMXEC_RecordCallback*pRecordCallback)
{
	XAutoLock l(m_csLocalRecordCallback);
	m_pLocalRecordCallback=pRecordCallback;
}

/******************************************************************************
* SetRemoteRecordCallback
*描述：设置远程录像回调
*输入：pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetRemoteRecordCallback(IMXEC_RecordCallback*pRecordCallback)
{
	XAutoLock l(m_csRemoteRecordCallback);
	m_pRemoteRecordCallback=pRecordCallback;
}

/******************************************************************************
* SetSecondRecordCallback
*描述：设置辅流录像回调
*输入：	pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void MediaCall::SetSecondRecordCallback(IMXEC_RecordCallback*pRecordCallback)
{
	XAutoLock l(m_csSecondRecordCallback);
	m_pSecondRecordCallback=pRecordCallback;
}

/******************************************************************************
* SaveLocalMainVideoToBMPFile
*描述：保存本地主流视频到BMP图片文件
*输入：	cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::SaveLocalMainVideoToBMPFile(const char*cszPathName)
{
	if (m_pLocalMainVideoWnd!=NULL)
	{
		if (m_pLocalMainVideoWnd->CaptureToBMPFile(cszPathName))
		{
			return 0;
		}
	}

	return -1;
}

/******************************************************************************
* SaveRemoteMainVideoToBMPFile
*描述：保存远程主流视频到BMP图片文件
*输入：	cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::SaveRemoteMainVideoToBMPFile(const char*cszPathName)
{
	if (HasAVCONTransport())
	{
		int nRet=-1;
		if (m_pRemoteMainPlayChan)
		{
			if (m_pRemoteMainPlayChan->CaptureToBMPFile(cszPathName))
			{
				nRet=0;
			}
		}
		return nRet;
	}
	if (m_pRemoteMainVideoWnd!=NULL)
	{
		if (m_pRemoteMainVideoWnd->CaptureToBMPFile(cszPathName))
		{
			return 0;
		}
	}

	return -1;
}

/******************************************************************************
* SaveSecondVideoToBMPFile
*描述：保存辅流视频到BMP图片文件
*输入：	cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::SaveSecondVideoToBMPFile(const char*cszPathName)
{
	if (HasAVCONTransport())
	{
		int nRet=-1;
		if (m_pRemoteSecondPlayChan)
		{
			if (m_pRemoteSecondPlayChan->CaptureToBMPFile(cszPathName))
			{
				nRet=0;
			}
		}
		return nRet;
	}
	if (m_pSecondVideoWnd!=NULL)
	{
		if (m_pSecondVideoWnd->CaptureToBMPFile(cszPathName))
		{
			return 0;
		}
	}

	return -1;
}

/******************************************************************************
* StartMixingWithWaveFile
*描述：开始对呼叫进行WAVE文件混音，对方会听到WAVE文件播放的声音
*输入：	cszPathName		-WAVE文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::StartMixingWithWaveFile(const char*cszPathName)
{
	if (m_pLocalAudioCapChan!=NULL)
	{
		return m_pLocalAudioCapChan->StartMixingWithWaveFile(cszPathName);
	}

	return -1;
}

/******************************************************************************
* StopMixingWithWaveFile
*描述：停止对呼叫进行WAVE文件混音
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int MediaCall::StopMixingWithWaveFile(void)
{
	if (m_pLocalAudioCapChan!=NULL)
	{
		m_pLocalAudioCapChan->StopMixingWithWaveFile();
		return 0;
	}

	return -1;
}

void MediaCall::OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop)
{
	if (m_pMediaSenderRemoteMain)
	{
		m_pMediaSenderRemoteMain->SendAudio(pData,nLen);
	}
}

void MediaCall::OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode)
{
	if (m_pMediaSenderRemoteMain)
	{
		m_pMediaSenderRemoteMain->SendVideo(pData,nLen);
	}
}


void GetVideoSizeByH264Level(int nH264Level,VIDEO_FORMAT_TYPE nVideoFormatType,int&nWidth,int&nHeight)
{
	VIDEO_FORMAT_TYPE nDstVideoFormatType=nVideoFormatType;
#if (0)
	if (nH264Level<=19)
	{
		nDstVideoFormatType=VIDEO_FORMAT_QCIF;
	}
	else if (nH264Level<=29)
	{
		if (nVideoFormatType==VIDEO_FORMAT_QCIF ||
			nVideoFormatType==VIDEO_FORMAT_CIF)
		{
			nDstVideoFormatType=nVideoFormatType;
		}
		else
		{
			nDstVideoFormatType=VIDEO_FORMAT_CIF;
		}
	}
	else if (nH264Level<=71)
	{
		if (nVideoFormatType==VIDEO_FORMAT_QCIF ||
			nVideoFormatType==VIDEO_FORMAT_CIF ||
			nVideoFormatType==VIDEO_FORMAT_4CIF ||
			nVideoFormatType==VIDEO_FORMAT_VGA ||
			nVideoFormatType==VIDEO_FORMAT_SVGA ||
			nVideoFormatType==VIDEO_FORMAT_XGA ||
			nVideoFormatType==VIDEO_FORMAT_720I ||
			nVideoFormatType==VIDEO_FORMAT_720P)
		{
			nDstVideoFormatType=nVideoFormatType;
		}
		else
		{
			nDstVideoFormatType=VIDEO_FORMAT_720P;
		}
	}
	else
	{
		if (nVideoFormatType==VIDEO_FORMAT_QCIF ||
			nVideoFormatType==VIDEO_FORMAT_CIF ||
			nVideoFormatType==VIDEO_FORMAT_4CIF ||
			nVideoFormatType==VIDEO_FORMAT_VGA ||
			nVideoFormatType==VIDEO_FORMAT_SVGA ||
			nVideoFormatType==VIDEO_FORMAT_XGA ||
			nVideoFormatType==VIDEO_FORMAT_720I ||
			nVideoFormatType==VIDEO_FORMAT_720P ||
			nVideoFormatType==VIDEO_FORMAT_1080I ||
			nVideoFormatType==VIDEO_FORMAT_SXGA ||
			nVideoFormatType==VIDEO_FORMAT_1080P ||
			nVideoFormatType==VIDEO_FORMAT_16CIF)
		{
			nDstVideoFormatType=nVideoFormatType;
		}
		else
		{
			nDstVideoFormatType=VIDEO_FORMAT_720P;
		}
	}
#endif

	switch (nDstVideoFormatType)
	{
	case VIDEO_FORMAT_QCIF:
		{
			nWidth=176;
			nHeight=144;
		}
		break;
	case VIDEO_FORMAT_CIF:
		{
			nWidth=352;
			nHeight=288;
		}
		break;
	case VIDEO_FORMAT_4CIF:
		{
			nWidth=704;
			nHeight=576;
		}
		break;
	case VIDEO_FORMAT_16CIF:
		{
			nWidth=1408;
			nHeight=1152;
		}
		break;
	case VIDEO_FORMAT_720I:
		{
			nWidth=1280;
			nHeight=360;
		}
		break;
	case VIDEO_FORMAT_720P:
		{
			nWidth=1280;
			nHeight=720;
		}
		break;
	case VIDEO_FORMAT_1080I:
		{
			nWidth=1920;
			nHeight=540;
		}
		break;
	case VIDEO_FORMAT_1080P:
		{
			nWidth=1920;
			nHeight=1080;
		}
		break;
	case VIDEO_FORMAT_VGA:
		{
			nWidth=640;
			nHeight=480;
		}
		break;
	case VIDEO_FORMAT_SVGA:
		{
			nWidth=800;
			nHeight=600;
		}
		break;
	case VIDEO_FORMAT_XGA:
		{
			nWidth=1024;
			nHeight=768;
		}
		break;
	case VIDEO_FORMAT_SXGA:
		{
			nWidth=1280;
			nHeight=1024;
		}
		break;
	default:
		{
			nWidth=176;
			nHeight=144;
		}
		break;
	}


}

//notice netec send ptz 
int MediaCall::MediaSendCtrl(void*pData,int nLen)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		return m_pRemoteMainPlayChan->SendCtrl(pData,nLen);
	}
	return 0;
}

int MediaCall::MediaSendPTZ(void*pData,int nLen)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		return m_pRemoteMainPlayChan->SendPTZ(pData,nLen);
	}
	return 0;
}

int MediaCall::ReSetMainAudioCapBitrate(int nBitrate)
{
	m_nAudioFlowCCBitRate = nBitrate;
	if(m_pLocalMainVideoCapChan)
	{
		//notice Audio not use
		//return m_pLocalAudioCapChan ->SetBitrate(nBitrate);
	}
	return 0;
}

int MediaCall::ReSetMainVideoCapBitrate(int nBitrate)
{
	m_nMainVideoFlowCCBitRate = nBitrate;
	if(m_pLocalMainVideoCapChan)
	{
		//notice the call need videc support 
		//return m_pLocalMainVideoCapChan ->SetBitrate(nBitrate);
	}
	return 0;
}

int MediaCall::ReSetSecondVideoCapBitrate(int nBitrate)
{
	m_nDualVideoFlowCCBitRate = nBitrate;
	if(m_pSecondVideoCapChan)
	{
		//notice the call need videc support 
		//return m_pSecondVideoCapChan ->SetBitrate(nBitrate);
	}
	return 0;
}

//Add WebRTC by zys 20160929
bool MediaCall::SendRtp(const uint8_t* packet, size_t length, const PacketOptions& options)
{

	unsigned char pAudecBuffer[1500];
	//将rtp头替换为audec头
	int nAudecPacketLen = SetAudecHeader(pAudecBuffer,packet,length);

	if (m_nAudioFlowCCBitRate >0)
	{
		if (HasAVCONTransport())
		{
			if (m_pMediaSenderLocalMain )
			{
				m_pMediaSenderLocalMain->SendAudio((unsigned char*)pAudecBuffer,nAudecPacketLen);
			}
		}
		else
		{
			int nRTPHeaderLen=12;
			char*pData=(char*)pAudecBuffer;
			int nLen=nAudecPacketLen;


			if (m_nLocalAudioCapChanBufLen<(nLen+nRTPHeaderLen) ||
				m_pLocalAudioCapChanBuf==NULL)
			{
				if (m_pLocalAudioCapChanBuf!=NULL)
				{
					free(m_pLocalAudioCapChanBuf);
					m_pLocalAudioCapChanBuf=NULL;
				}

				m_nLocalAudioCapChanBufLen=(nLen+nRTPHeaderLen)*2;
				if (m_nLocalAudioCapChanBufLen<1500)
				{
					m_nLocalAudioCapChanBufLen=1500;
				}

				m_pLocalAudioCapChanBuf=(unsigned char*)malloc(m_nLocalAudioCapChanBufLen);
				if (m_pLocalAudioCapChanBuf==NULL)
				{
					return true;
				}
			}

			unsigned short usSequence=AUDEC_HEADER_GET_SEQUENCE(pData);
			unsigned char ucAudioCodec=AUDEC_HEADER_GET_CODEC_ID(pData);
			int nPayloadSize=nLen-AUDEC_HEADER_VER_01_LEN;
			char*pPayloadData=(char*)pData+AUDEC_HEADER_VER_01_LEN;
			unsigned short nDuration=160;
			m_ulLocalAudioCapChanRTPTimestamp+=nDuration;

			PRTP_HEADER pRTPHeader=(PRTP_HEADER)m_pLocalAudioCapChanBuf;
			memset(pRTPHeader,0,RTP_HEADER_LEN);
			pRTPHeader->version=RTP_VERSION;
			pRTPHeader->ssrc=htonl((unsigned long)this);
			pRTPHeader->timestamp=htonl(m_ulLocalAudioCapChanRTPTimestamp);
			pRTPHeader->sequence=htons(usSequence);
			pRTPHeader->type=m_nLocalAudioCapChanRTPPayloadType;
			memcpy(m_pLocalAudioCapChanBuf+RTP_HEADER_LEN,pPayloadData,nPayloadSize);

			SendAudio((char*)m_pLocalAudioCapChanBuf,nPayloadSize+RTP_HEADER_LEN);

		}

		XAutoLock l(m_csLocalRecordCallback);
		if (m_pLocalRecordCallback!=NULL)
		{
			m_pLocalRecordCallback->OnIMXEC_RecordCallbackAudioData((unsigned char*)pAudecBuffer,nAudecPacketLen);
		}
		return true;
	}
	return true;
}

bool MediaCall::SendRtcp(const uint8_t* packet, size_t length)
{
	return true;
}

int MediaCall::SetAudecHeader(unsigned char* pData,void const* pSrcData,size_t nlen)
{
	int nDataLen = nlen-RTP_HEADER_LEN;
	int nPacketLen=nDataLen+AUDEC_HEADER_VER_01_LEN;
	if (nDataLen<=0 || nPacketLen>1500)
	{
		return -1;
	}

	AUDEC_HEADER_RESET(pData);
	AUDEC_HEADER_SET_SEQUENCE(pData, m_nAudioSeqNo++);
	AUDEC_HEADER_SET_TIMESTAMP(pData,  XGetTimestamp());
	unsigned short usAudioCodecID = 0;
	switch(g_emAudioCodecID)
	{
	case CODEC_PCMA:
		{
			usAudioCodecID = X_AUDIO_CODEC_G711A;
		}
	case CODEC_G7221C_48K:
		{
			usAudioCodecID = X_AUDIO_CODEC_G7221_14_48;
		}
	default:
		break;
	}
	AUDEC_HEADER_SET_CODEC_ID(pData, usAudioCodecID);  //默认使用G7221 16K
	AUDEC_HEADER_SET_FEC(pData, 1);
	AUDEC_HEADER_SET_SYN_POINT(pData, 0);
	AUDEC_HEADER_SET_DO_NOT_SYN(pData, 0);
	AUDEC_HEADER_SET_USED_FEC(pData,0);

	memcpy(pData+AUDEC_HEADER_VER_01_LEN, (unsigned char*)pSrcData+RTP_HEADER_LEN , nDataLen);
	return nPacketLen;
}

int MediaCall::AudecHeaderToRTPHeader(unsigned char* pRTPData,void const* pAudecData,size_t nAudecLen)
{
	X_AUDIO_CODEC_ID nCodecID = (X_AUDIO_CODEC_ID)AUDEC_HEADER_GET_CODEC_ID(pAudecData);

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