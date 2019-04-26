//CapChanDev.cpp
#include "../IMXEC_win32/stdafx.h"

#include "CapChanDev.h"
#include <VIDEC/VIDEC_Header.h>
#include <HPMCU/AVMTType.h>
#include "XUtil.h"
#include "CapChanDevMgr.h"
#include "PlayChanMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Add WebRTC by zys 20160929
#pragma comment(lib,"Strmiids.lib") 
#pragma comment(lib,"Dmoguids.lib")  
#pragma comment(lib,"wmcodecdspuuid.lib") 
#pragma comment(lib,"Msdmo.lib")

IMXEC_CapChanDev*IMXEC_CapChanDev::Create(IMXEC_CapChanCallback&rCallback)
{
	return new CapChanDev(rCallback);
}

class DVCDevMgr
{
protected:
	typedef std::map<std::string,CapChanDev*>	MapDVCDev;
	MapDVCDev	m_MapDVCDev;
public:
	DVCDevMgr(void)
	{

	}
	virtual~DVCDevMgr(void)
	{

	}
public:
	bool AddDVCDev(const std::string&strIP,CapChanDev*pCapChanDev)
	{
		MapDVCDev::iterator iter=m_MapDVCDev.find(strIP);
		if (iter!=m_MapDVCDev.end())
		{
			if (iter->second!=pCapChanDev)
			{
				return false;
			}
		}

		m_MapDVCDev[strIP]=pCapChanDev;
		return true;
	}
	void RemoveDVCDev(const std::string&strIP,CapChanDev*pCapChanDev)
	{
		MapDVCDev::iterator iter=m_MapDVCDev.find(strIP);
		if (iter!=m_MapDVCDev.end())
		{
			if (iter->second==pCapChanDev)
			{
				m_MapDVCDev.erase(iter);
			}
		}
	}
	bool CanOpenDVCDev(const std::string&strIP,CapChanDev*pCapChanDev)
	{
		MapDVCDev::iterator iter=m_MapDVCDev.find(strIP);
		if (iter==m_MapDVCDev.end())
		{
			return true;
		}

		if (iter->second==pCapChanDev)
		{
			return true;
		}

		return false;
	}
};

static DVCDevMgr g_DVCDevMgr;
bool CapChanDev::ms_bAudioRecordAll = false;

CapChanDev::CapChanDev(IMXEC_CapChanCallback&rCallback)
:CapChan(rCallback)
,m_pCapDev(NULL)
,m_pScreen(NULL)
,m_pMediaFile(NULL)
,m_nLastOpenCapDevTime(0)
,m_strMediaFilePathName("")

,m_pMainVidCapChan(NULL)
,m_pSubVidCapChan(NULL)
,m_pQSubVidCapChan(NULL)
,m_bGotSubFrame(false)
,m_bGotQSubFrame(false)

,m_nVideoCardType(VCT_UNKOWN)

,m_pAudCapChanExternal(NULL)
,m_nAudioCapDevID(-2)
,m_nAudioCodecID(X_AUDIO_CODEC_G7221_14_24)
,m_pWebRTCAudio(NULL)

,m_pAVIWriter(NULL)

,m_nForceEncodeVideo(0)
,m_nForceEncodeAudio(1)

,m_nScreenIndex(-1)
,m_nWndIndex(-1)
,m_bMute(false)
{
	m_bVideoDevIsOpen=false;

	m_nDevID=-1;
	m_nWidth=352;
	m_nHeight=288;
	m_nFrameRate=15;
	m_nBitrate=384;
	m_nBitrateControlType=0;
	m_nCodecType=VIDEC_CODEC_H264_SVC;
	m_nSub=0;
	m_nQSub=0;
	m_nSubWidth=0;
	m_nSubHeight=0;
	m_nSubBitrate=0;
	m_nQSubWidth=0;
	m_nQSubHeight=0;
	m_nQSubBitrate=0;

	m_nInvert=0;
	m_nDenoise=1;
	m_nLevelFixer=0;
	m_bEnableOSD=false;
	m_strOSDText="";
	m_nOSDAlign=0;
	m_nTop=-1;
	m_nLeft=-1;
	ZeroMemory(&m_logfontOSD, sizeof(m_logfontOSD));	
	m_logfontOSD.lfWeight = FW_HEAVY;//FW_BOLD;
	strcpy(m_logfontOSD.lfFaceName, _T("Tahoma"));
	m_logfontOSD.lfHeight = -16;
	m_logfontOSD.lfQuality = NONANTIALIASED_QUALITY;

	m_crOSDText=(RGB(0,0,255));
	m_crOSDBk=(RGB(255,255,255));

	m_nBrightness=VIDEC_GetDefaultBrightness();
	m_nContrast=VIDEC_GetDefaultContrast();
	m_nSaturation=VIDEC_GetDefaultSaturation();
	m_nHue=VIDEC_GetDefaultHue();
	m_nGamma=VIDEC_GetDefaultGamma();
	m_nSharpen=VIDEC_GetDefaultSharpen();
	m_bSharpenLocalOnly=true;
	m_nGammaR=VIDEC_GetDefaultGamma();
	m_nGammaG=VIDEC_GetDefaultGamma();
	m_nGammaB=VIDEC_GetDefaultGamma();

	m_pMixerScreen=NULL;
	m_nMixerScreenIndex=-1;
	m_nMixerScreenWidth=320;
	m_nMixerScreenHeight=240;
	m_nMixerScreenFrameRate=15;

	m_pDVC=NULL;
	m_strDVCIP="";
	m_strDVCPassword="";
	m_nDVCPort=0;

	m_bVideoSignal=true;
	m_nNewVideoSignal=1;

	m_pVQD=NULL;
	m_nLastGotKeyFrameTS=9;
	m_nFlagRecording=0;

	m_pAudioMixer = NULL;
	m_nAudioMixOut = 0;
	m_nAudioMixIn = 0;
	AudioPlayChanMgr::Instance();
}

CapChanDev::~CapChanDev(void)
{

}

//打开通道
int CapChanDev::Open(HWND hwndParent,int nOnlyPreview)
{
	return CapChan::Open(hwndParent,nOnlyPreview);
}

//关闭通道
void CapChanDev::Close(void)
{
	StopRecordToAVI();

	CloseVideoDev();
	CloseAudio();
	CapChan::Close();

	if (m_pVQD)
	{
		m_pVQD->Close();
		delete m_pVQD;
		m_pVQD=NULL;
	}
}

void CapChanDev::EnablePreview(int nPreview)
{
	CapChan::EnablePreview(nPreview);
}

//取视频窗口
HWND CapChanDev::GetVideoWnd(void)
{
	return CapChan::GetVideoWnd();
}
//取视频宽度
int CapChanDev::GetVideoWidth(void)
{
	return CapChan::GetVideoWidth();
}

//取视频高度
int CapChanDev::GetVideoHeight(void)
{
	return CapChan::GetVideoHeight();
}

//same as CWnd::IsWindowVisible
bool CapChanDev::IsWindowVisible(void)
{
	return CapChan::IsWindowVisible();
}
//same as CWnd::SetWindowPos
bool CapChanDev::SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags)
{
	return CapChan::SetWindowPos(hWndInsertAfter,x,y,cx,cy,nFlags);
}

//保持当前图象到BMP文件
bool CapChanDev::CaptureToBMPFile(const char*cszFile)
{
	return CapChan::CaptureToBMPFile(cszFile);
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
void CapChanDev::SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	m_nScreenIndex=nScreenIndex;
	m_nWndIndex=nWndIndex;

	if (m_pScreen!=NULL)
	{
		m_pScreen->SetScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
	else if (m_pWnd!=NULL)
	{
		m_pWnd->SetScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}

	int nMixerScreenIndex=CapChanDevMgr::Instance().GetMixerScreenIndex();
	if (m_pMediaFile && m_pAudCapChanExternal && m_nScreenIndex>=0 && nMixerScreenIndex>=0)
	{
		if (nMixerScreenIndex==m_nScreenIndex)
		{
			AudioCapChanMgr::Instance().StartMixingWithXCapChanExternal(m_pAudCapChanExternal);
		}
		else
		{
			AudioCapChanMgr::Instance().StopMixingWithXCapChanExternal(m_pAudCapChanExternal);
		}
	}
}

void CapChanDev::SetVideoWndBk(const char* szBkImage, int nVideoWnBkR, int nVideoWnBkG, int nVideoWnBkB)
{
	if (szBkImage)
	{
		m_strVideoWndBkImage = szBkImage;
	}
	m_nVideoWnBkR = nVideoWnBkR;
	m_nVideoWnBkG = nVideoWnBkG;
	m_nVideoWnBkB = nVideoWnBkB;
}

int CapChanDev::SetVolume(int nVolume)
{

	if (m_pMediaFile!=NULL)
	{
		return m_pMediaFile->SetVolume(nVolume);
	}

	if (m_pAudCapChanExternal!=NULL)
	{
		return m_pAudCapChanExternal->SetVolume(nVolume);
	}

	if (0==AudioCapChanMgr::Instance().SetVolume(nVolume))
	{
		return 0;
	}

	return CapChan::SetVolume(nVolume);
}

int CapChanDev::GetVolume()
{
	if (m_pAudCapChanExternal!=NULL)
	{
		return m_pAudCapChanExternal->GetVolume();
	}

	return AudioCapChanMgr::Instance().GetVolume();
}

int CapChanDev::GetLevel(int&nLevel)
{
	if (m_pAudCapChanExternal!=NULL)
	{
		nLevel=m_pAudCapChanExternal->GetLevel();
		if (nLevel<0)
		{
			nLevel=0;
			return -1;
		}
		return 0;
	}

	if (0==AudioCapChanMgr::Instance().GetLevel(nLevel))
	{
		return 0;
	}

	return CapChan::GetLevel(nLevel);
}

unsigned long CapChanDev::GetAudioID(void)
{
	if (m_pAudCapChanExternal==NULL && m_pMediaFile==NULL)
	{
		unsigned long nAudioID=0;
		if (m_pMixerScreen)
		{
			unsigned long nKeyID=(unsigned long)this;
			nKeyID|=0xFF000000;
			if (0==AudioCapChanMgr::Instance().GetMixingAudioID(nKeyID,nAudioID))
			{
				return nAudioID;
			}
		}

		if (0==AudioCapChanMgr::Instance().GetAudioID(nAudioID))
		{
			return nAudioID;
		}
	}
	return CapChan::GetAudioID();
}

unsigned long CapChanDev::GetVideoID(void)
{
	return CapChan::GetVideoID();
}

//打开视频设备
int CapChanDev::OpenVideoDev(int nDevID,int nWidth,int nHeight,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub,const char*cszDVCIP,unsigned short nDVCPort,const char*cszDVCPassword,int nSubWidth,int nSubHeight,int nSubBitrate,int nQSubWidth,int nQSubHeight,int nQSubBitrate)
{
	std::string strDVCIP="";
	if (cszDVCIP)
	{
		strDVCIP=cszDVCIP;
	}
	std::string strDVCPassword="";
	if (cszDVCPassword)
	{
		strDVCPassword=cszDVCPassword;
	}

	if (m_strDVCIP.size())
	{
		g_DVCDevMgr.RemoveDVCDev(m_strDVCIP,this);
	}

	if (m_strDVCIP!=strDVCIP || m_nDVCPort!=nDVCPort || m_strDVCPassword!=strDVCPassword)
	{
		if (m_strDVCIP.size())
		{
			if (m_strDVCIP!=strDVCIP)
			{
				VIDEC_DVCDev::SetIP(m_strDVCIP.c_str(),0,"");

				g_DVCDevMgr.RemoveDVCDev(m_strDVCIP,this);
			}
			else
			{
				VIDEC_DVCDev::SetIP(m_strDVCIP.c_str(),nDVCPort,strDVCPassword.c_str());
			}
		}

		if (m_pCapDev!=NULL)
		{
			VIDEC_DVCDev*pCapDev=dynamic_cast<VIDEC_DVCDev*>(m_pCapDev);
			if (pCapDev)
			{
				m_pCapDev->Close();
				delete m_pCapDev;
				m_pCapDev=NULL;
				m_nDevID=-1;
			}
		}

		m_strDVCPassword=strDVCPassword;
		m_strDVCIP=strDVCIP;
		m_nDVCPort=nDVCPort;

		NETEC_DVC*pDVC=m_pDVC;
		{
			XAutoLock l(m_csDVC);
			m_pDVC=NULL;
		}

		if (pDVC)
		{
			pDVC->Close();
			delete pDVC;
			pDVC=NULL;
		}
	}

	if (nDevID>=0)
	{
		if (m_pDVC==NULL && m_strDVCIP.size() && m_nDVCPort && m_strDVCPassword.size() && g_DVCDevMgr.CanOpenDVCDev(m_strDVCIP,this))
		{
			NETEC_DVC*pDVC=NETEC_DVC::Create(*this);
			if (pDVC)
			{
				if (0!=pDVC->Open(m_strDVCIP.c_str(),m_nDVCPort,m_strDVCPassword.c_str()))
				{
					pDVC->Close();
					delete pDVC;
					pDVC=NULL;
				}
				else
				{
					g_DVCDevMgr.AddDVCDev(m_strDVCIP.c_str(),this);

					XAutoLock l(m_csDVC);
					m_pDVC=pDVC;

					int nResolution=0;
					if (nWidth<=176)
						nResolution=0;
					else if (nWidth<=320)
						nResolution=1;
					else if (nWidth<=352)
						nResolution=2;
					else if (nWidth<=640)
						nResolution=3;
					else if (nWidth<=704)
						nResolution=4;
					else if (nWidth<=960)
						nResolution=5;
					else if (nWidth<=1280)
						nResolution=6;
					else if (nWidth<=1600)
						nResolution=7;
					else
						nResolution=8;

					m_pDVC->SetVideoParams(VIDEC_CODEC_H264,nBitrate,nBitrateControlType,nResolution,m_nFrameRate,0);
				}
			}
		}
		else if (m_pDVC)
		{
			int nResolution=0;
			if (nWidth<=176)
				nResolution=0;
			else if (nWidth<=320)
				nResolution=1;
			else if (nWidth<=352)
				nResolution=2;
			else if (nWidth<=640)
				nResolution=3;
			else if (nWidth<=704)
				nResolution=4;
			else if (nWidth<=960)
				nResolution=5;
			else if (nWidth<=1280)
				nResolution=6;
			else if (nWidth<=1600)
				nResolution=7;
			else
				nResolution=8;

			m_pDVC->SetVideoParams(VIDEC_CODEC_H264,nBitrate,nBitrateControlType,nResolution,m_nFrameRate,0);
		}
	}
	else
	{
		NETEC_DVC*pDVC=m_pDVC;
		{
			XAutoLock l(m_csDVC);
			m_pDVC=NULL;
		}

		if (pDVC)
		{
			pDVC->Close();
			delete pDVC;
			pDVC=NULL;
		}
	}

	return OpenVideoDevActual(nDevID,nWidth,nHeight,nFrameRate,nBitrate,nBitrateControlType,CodecType,nSub,nQSub,nSubWidth, nSubHeight, nSubBitrate, nQSubWidth, nQSubHeight, nQSubBitrate);
}

int CapChanDev::OpenVideoDevActual(int nDevID,int nWidth,int nHeight,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub,int nSubWidth,int nSubHeight,int nSubBitrate,int nQSubWidth,int nQSubHeight,int nQSubBitrate)
{
	if (nDevID<0)
	{
		if (m_strDVCIP.size() && m_nDVCPort && m_strDVCPassword.size() && g_DVCDevMgr.CanOpenDVCDev(m_strDVCIP,this))
		{
			nDevID=IMXEC_CapChanDev::GetDevCount();
		}
		else
		{
			m_nVideoCardType=VCT_UNKOWN;

			m_nDevID=nDevID;
			CloseVideoDev();
			return -1;
		}
	}

	int nResetWidth=nWidth;
	int nResetHeight=nHeight;

	int nEncoderWidth=nWidth;
	int nEncoderHeight=nHeight;

	int nFlagVideoAutoFormat=IMXEC_CapChanDev::GetVideoAutoFormat();
	if (nFlagVideoAutoFormat)
	{
		nWidth=0;
		nHeight=0;
	}
	//else
	//{
	//	nEncoderWidth=0;
	//	nEncoderHeight=0;
	//}

	if (m_nDevID!=nDevID ||
		m_nWidth!=nResetWidth ||
		m_nHeight!=nResetHeight ||
		m_nFrameRate!=nFrameRate ||
		m_nBitrate!=nBitrate ||
		m_nBitrateControlType!=nBitrateControlType ||
		m_nCodecType!=CodecType ||
		m_nSub!=nSub ||
		m_nQSub!=nQSub ||
		m_nSubWidth!=nSubWidth ||
		m_nSubHeight!=nSubHeight ||
		m_nSubBitrate!=nSubBitrate ||
		m_nQSubWidth!=nQSubWidth ||
		m_nQSubHeight!=nQSubHeight ||
		m_nQSubBitrate!=nQSubBitrate ||
		m_pCapDev==NULL)
	{
		m_nDevID=nDevID;
		m_nWidth=nResetWidth;
		m_nHeight=nResetHeight;
		m_nFrameRate=nFrameRate;
		m_nBitrate=nBitrate;
		m_nBitrateControlType=nBitrateControlType;
		m_nCodecType=CodecType;
		m_nSub=nSub;
		m_nQSub=nQSub;

		m_nSubWidth=nSubWidth;
		m_nSubHeight=nSubHeight;
		m_nSubBitrate=nSubBitrate;
		m_nQSubWidth=nQSubWidth;
		m_nQSubHeight=nQSubHeight;
		m_nQSubBitrate=nQSubBitrate;
	}
	else
	{
		return 0;
	}

	m_nLastOpenCapDevTime=XGetTimestamp();

	VIDEC_CapDev*pOldCapDev=m_pCapDev;

	int nAH2KDevID=-1;
	int nDVCDevID=-1;
	while (true)
	{
		int nDevCount=0;
		nDevCount=VIDEC_AH2KDev::GetDevCount();
		if (nDevID<nDevCount)
		{
			m_nVideoCardType=VCT_AH;

			nAH2KDevID=nDevID;

			VIDEC_AH2KDev*pCapDev=NULL;
			if (m_pCapDev!=NULL)
			{
				pCapDev=dynamic_cast<VIDEC_AH2KDev*>(m_pCapDev);
				if (pCapDev==NULL)
				{
					m_pCapDev->Close();
					delete m_pCapDev;
					m_pCapDev=NULL;

					pOldCapDev=NULL;
				}
			}

			if (m_pCapDev==NULL)
			{
				pCapDev=VIDEC_AH2KDev::Create();
				if (pCapDev==NULL)
				{
					CloseVideoDev();
					return -1;
				}
				if (0!=pCapDev->Open(nDevID,nWidth,nHeight,nFrameRate,false))
				{
					pCapDev->Close();
					delete pCapDev;
					pCapDev=NULL;

					CloseVideoDev();
					return -1;
				}
				m_pCapDev=pCapDev;
				break;
			}

			if (0!=pCapDev->Reset(nDevID,nWidth,nHeight,nFrameRate))
			{
				CloseVideoDev();
				return -1;
			}
			break;
		}
		nDevID-=nDevCount;

		nDevCount=VIDEC_CameraDev::GetDevCount();
		if (nDevID<nDevCount)
		{
			m_nVideoCardType=VCT_CAMERA;

			VIDEC_CameraDev*pCapDev=NULL;
			if (m_pCapDev!=NULL)
			{
				pCapDev=dynamic_cast<VIDEC_CameraDev*>(m_pCapDev);
				if (pCapDev==NULL)
				{
					m_pCapDev->Close();
					delete m_pCapDev;
					m_pCapDev=NULL;

					pOldCapDev=NULL;
				}
			}

			if (m_pCapDev==NULL)
			{
				pCapDev=VIDEC_CameraDev::Create();
				if (pCapDev==NULL)
				{
					CloseVideoDev();
					return -1;
				}
				if (0!=pCapDev->Open(nDevID,nWidth,nHeight,nFrameRate))
				{
					pCapDev->Close();
					delete pCapDev;
					pCapDev=NULL;

					CloseVideoDev();
					return -1;
				}
				m_pCapDev=pCapDev;
				break;
			}

			if (0!=pCapDev->Reset(nDevID,nWidth,nHeight,nFrameRate))
			{
				CloseVideoDev();
				return -1;
			}
			break;
		}
		nDevID-=nDevCount;

		nDevCount=VIDEC_AH400Dev::GetDevCount();
		if (nDevID<nDevCount)
		{
			m_nVideoCardType=VCT_HIK;

			CodecType=VIDEC_CODEC_AH400;

			VIDEC_AH400Dev*pCapDev=NULL;

			if (m_pCapDev!=NULL)
			{
				pCapDev=dynamic_cast<VIDEC_AH400Dev*>(m_pCapDev);
				if (pCapDev==NULL)
				{
					m_pCapDev->Close();
					delete m_pCapDev;
					m_pCapDev=NULL;

					pOldCapDev=NULL;
				}
			}

			if (m_pCapDev==NULL)
			{
				pCapDev=VIDEC_AH400Dev::Create();
				if (pCapDev==NULL)
				{
					CloseVideoDev();
					return -1;
				}
				if (0!=pCapDev->Open(nDevID,nWidth,nHeight,nFrameRate))
				{
					pCapDev->Close();
					delete pCapDev;
					pCapDev=NULL;

					CloseVideoDev();
					return -1;
				}

				CodecType=VIDEC_CODEC_AH400;

				m_pCapDev=pCapDev;
				break;
			}

			CodecType=VIDEC_CODEC_AH400;

			if (0!=pCapDev->Reset(nDevID,nWidth,nHeight,nFrameRate))
			{
				CloseVideoDev();
				return -1;
			}
			break;
		}
		nDevID-=nDevCount;

		if (m_strDVCIP.size())
			VIDEC_DVCDev::SetIP(m_strDVCIP.c_str(),m_nDVCPort,m_strDVCPassword.c_str());

		nDevCount=VIDEC_DVCDev::GetDevCount();
		if (nDevCount>1)
		{
			char szDevName[1025]="";
			for (int nDevIndex=0;nDevIndex<nDevCount;++nDevIndex)
			{
				VIDEC_DVCDev::GetDevName(nDevIndex,szDevName,1024);
				std::string strDevName=szDevName;
				if (strDevName.size()>m_strDVCIP.size())
				{
					int nOffset=strDevName.size()-m_strDVCIP.size();
					std::string strDevIP=strDevName.substr(nOffset);
					if (strDevIP==m_strDVCIP)
					{
						nDevID=nDevIndex;
						break;
					}
				}
			}
		}

		if (nDevID<nDevCount)
		{
			m_nVideoCardType=VCT_AH;

			nDVCDevID=nDevID;

			VIDEC_DVCDev*pCapDev=NULL;
			if (m_pCapDev!=NULL)
			{
				pCapDev=dynamic_cast<VIDEC_DVCDev*>(m_pCapDev);
				if (pCapDev==NULL)
				{
					m_pCapDev->Close();
					delete m_pCapDev;
					m_pCapDev=NULL;

					pOldCapDev=NULL;
				}
			}

			int nResolution=6;
			if (nFlagVideoAutoFormat)
			{
				if (nEncoderWidth<=176)
					nResolution=0;
				else if (nEncoderWidth<=320)
					nResolution=1;
				else if (nEncoderWidth<=352)
					nResolution=2;
				else if (nEncoderWidth<=640)
					nResolution=3;
				else if (nEncoderWidth<=704)
					nResolution=4;
				else if (nEncoderWidth<=960)
					nResolution=5;
				else if (nEncoderWidth<=1280)
					nResolution=6;
				else 
					nResolution=7;
			}
			else
			{
				if (nWidth<=176)
					nResolution=0;
				else if (nWidth<=320)
					nResolution=1;
				else if (nWidth<=352)
					nResolution=2;
				else if (nWidth<=640)
					nResolution=3;
				else if (nWidth<=704)
					nResolution=4;
				else if (nWidth<=960)
					nResolution=5;
				else if (nWidth<=1280)
					nResolution=6;
				else 
					nResolution=7;
			}

			if (m_pCapDev==NULL)
			{
				pCapDev=VIDEC_DVCDev::Create();
				if (pCapDev==NULL)
				{
					CloseVideoDev();

					return -1;
				}

				if (0!=pCapDev->Open(nDevID,nFrameRate,nBitrate,nBitrateControlType,nResolution))
				{
					pCapDev->Close();
					delete pCapDev;
					pCapDev=NULL;

					CloseVideoDev();

					return -1;
				}
				m_pCapDev=pCapDev;

				g_DVCDevMgr.AddDVCDev(m_strDVCIP.c_str(),this);

				break;
			}

			if (0!=pCapDev->Reset(nDevID,nFrameRate,nBitrate,nBitrateControlType,nResolution))
			{
				CloseVideoDev();

				return -1;
			}

			break;
		}


		nDevID-=nDevCount;

		CloseVideoDev();
		return -1;
	}

	if (m_pCapDev==NULL)
	{
		CloseVideoDev();
		return -1;
	}

	if (m_pCapDev!=pOldCapDev)
	{
		m_bGotSubFrame=false;
		m_bGotQSubFrame=false;

		m_pCapDev->AddImageSink(this);

		if (m_pVQD)
		{
			m_pCapDev->AddImageSink(m_pVQD);
		}

		if (m_pWnd!=NULL)
		{
			if (pOldCapDev!=NULL)
			{
				int nWidth=160;
				int nHeight=120;
				unsigned char*pY=(unsigned char*)malloc(nWidth*nHeight*2);
				unsigned char*pU=pY+nWidth*nHeight;
				unsigned char*pV=pU+nWidth*nHeight/4;
				memset(pY,0,nWidth*nHeight);
				memset(pU,128,nWidth*nHeight/4);
				memset(pV,128,nWidth*nHeight/4);

				VIDEC_Image*pImage=VIDEC_Image::Create(pY,nWidth*nHeight*3/2,nWidth,nHeight,30);
				if (pImage!=NULL)
				{
					m_pWnd->Reset();
					m_pWnd->OnReceivedImage(pImage);

					pImage->Release();
					pImage=NULL;
				}
			}

			m_pCapDev->AddImageSink(m_pWnd);
		}

		if (pOldCapDev!=NULL)
		{
			m_bVideoSignal=m_pCapDev->HasVideoSignal()!=0;
			m_rCallback.OnIMXEC_CapChanCallbackVideoSignalChanged();
		}
	}

	if (m_nOnlyPreview!=0)
		return 0;

	if (CodecType==VIDEC_CODEC_AH400 ||
		(nAH2KDevID>=0 && VIDEC_AH2KDev::SupportHardwareEncoding(nAH2KDevID)) ||
		(nDVCDevID>=0))
	{
		VIDEC_AH400Dev*pAH400Dev=NULL;
		VIDEC_AH2KDev*pAH2KDev=NULL;
		VIDEC_DVCDev*pDVCDev=NULL;
		if (m_pCapDev!=NULL)
		{
			pAH400Dev=dynamic_cast<VIDEC_AH400Dev*>(m_pCapDev);
			pAH2KDev=dynamic_cast<VIDEC_AH2KDev*>(m_pCapDev);
			pDVCDev=dynamic_cast<VIDEC_DVCDev*>(m_pCapDev);
		}
		if (pAH400Dev==NULL && pAH2KDev==NULL && pDVCDev==NULL)
		{
			CodecType=VIDEC_CODEC_H264;
		}
		else
		{
			if (m_pMainVidCapChan!=NULL)
			{
				m_pMainVidCapChan->Close();
				delete m_pMainVidCapChan;
				m_pMainVidCapChan=NULL;
			}

			if (m_pSubVidCapChan!=NULL)
			{
				m_pSubVidCapChan->Close();
				delete m_pSubVidCapChan;
				m_pSubVidCapChan=NULL;
			}

			if (m_pQSubVidCapChan!=NULL)
			{
				m_pQSubVidCapChan->Close();
				delete m_pQSubVidCapChan;
				m_pQSubVidCapChan=NULL;
			}

			if (pAH400Dev!=NULL)
			{
				pAH400Dev->AddStreamSink(this);

				int nQMax=31;
				int nQMin=1;
				//int nSubBitrateTemp=(nBitrate*0.35);
				int nSubBitrateTemp=(nBitrate*0.5);
				if (nSubBitrateTemp<16)
					nSubBitrateTemp=16;

				int nSubQMax=31;
				int nSubQMin=1;
				int nSubBitrateControlType=nBitrateControlType;

				GetCodecInfo(nBitrate,nBitrateControlType,nBitrate,nQMin,nQMax,CodecType,false);
				GetCodecInfo(nSubBitrateTemp,nSubBitrateControlType,nSubBitrateTemp,nSubQMin,nSubQMax,CodecType,false);

				pAH400Dev->StartVideoStream(nBitrate,nQMin,nQMin,nQMax,nFrameRate*8,nSub!=0,1,nSubBitrateTemp,nSubQMin,nSubQMin,nSubQMax);
			}
			else if (pAH2KDev!=NULL)
			{
				pAH2KDev->AddStreamSink(this);

				int nQMax=31;
				int nQMin=1;
				CodecType=VIDEC_CODEC_H264;
				GetCodecInfo(nBitrate,nBitrateControlType,nBitrate,nQMin,nQMax,CodecType,false);
				pAH2KDev->StartVideoStream(nBitrate,nBitrateControlType,nQMin,nQMax);

				CodecType=VIDEC_CODEC_AH400;
			}
			else if (pDVCDev!=NULL)
			{
				pDVCDev->AddStreamSink(this);

				CodecType=VIDEC_CODEC_AH400;
			}

			if (m_pMediaSender!=NULL)
			{
				m_pMediaSender->StartVideo(0);
			}
		}
	}
	
	if (CodecType!=VIDEC_CODEC_AH400)
	{
		if (m_pCapDev!=pOldCapDev)
		{
			if (m_pMainVidCapChan!=NULL)
			{
				m_pCapDev->AddImageSink(m_pMainVidCapChan);
			}
			if (m_pSubVidCapChan!=NULL)
			{
				m_pCapDev->AddImageSink(m_pSubVidCapChan);
			}
			if (m_pQSubVidCapChan!=NULL)
			{
				m_pCapDev->AddImageSink(m_pQSubVidCapChan);
			}
		}


		if (nQSub!=0)
		{
			int nQSubBitrateTemp=(nBitrate>>3);
			if (nFlagVideoAutoFormat)
			{
				nQSubBitrateTemp=(nBitrate/3);
			}
			if (nQSubBitrateTemp<16)
				nQSubBitrateTemp=16;

			if (nQSubBitrate > 0)
			{
				nQSubBitrateTemp=nQSubBitrate;
			}
			
			int nTempHeight=nEncoderHeight;
			if (nEncoderWidth==1920 && nEncoderHeight==540)
			{
				nTempHeight=1080;
			}
			int nQSubWidthTemp = nEncoderWidth>>2;
			if (nQSubWidth > 0)
			{
				nQSubWidthTemp = nQSubWidth;
			}
			int nQSubHeightTemp= nTempHeight>>2;
			if (nQSubHeight > 0)
			{
				nQSubHeightTemp = nQSubHeight;
			}

			if (!nFlagVideoAutoFormat)
			{
				nQSubHeightTemp=0;
				nQSubWidthTemp=0;
			}

			if (m_pQSubVidCapChan==NULL)
			{
				m_pQSubVidCapChan=VIDEC_CapChan::Create(*this);
				if (m_pQSubVidCapChan==NULL)
				{
					return -1;
				}


				if (0!=m_pQSubVidCapChan->Open(nQSubBitrateTemp,nBitrateControlType,CodecType,2,nQSubWidthTemp,nQSubHeightTemp))
				{
					m_pQSubVidCapChan->Close();
					delete m_pQSubVidCapChan;
					m_pQSubVidCapChan=NULL;

					return -1;
				}
				m_pQSubVidCapChan->AddStreamSink(this);
				m_pCapDev->AddImageSink(m_pQSubVidCapChan);

				if (m_pMediaSender!=NULL)
				{
					m_pMediaSender->StartVideo(0);
				}
			}
			else
			{
				if (nFlagVideoAutoFormat)
				{
					m_pCapDev->RemoveImageSink(m_pQSubVidCapChan);
					m_pQSubVidCapChan->Reset(nQSubBitrateTemp,nBitrateControlType,CodecType,2,nQSubWidthTemp,nQSubHeightTemp);
					m_pCapDev->AddImageSink(m_pQSubVidCapChan);
				}
				else
				{
					m_pQSubVidCapChan->Reset(nQSubBitrateTemp,nBitrateControlType,CodecType,2,nQSubWidthTemp,nQSubHeightTemp);
				}
			}
		}
		else
		{
			if (m_pQSubVidCapChan!=NULL)
			{
				m_pQSubVidCapChan->Close();
				delete m_pQSubVidCapChan;
				m_pQSubVidCapChan=NULL;
			}
			m_bGotQSubFrame=false;
		}

		if (nSub!=0)
		{
			int nSubBitrateTemp=(nBitrate*0.35);
			if (nFlagVideoAutoFormat)
			{
				nSubBitrateTemp=(nBitrate*2/3);
				if (nFrameRate > 30)
				{
					nSubBitrateTemp=(nBitrate/2);
				}			
			}
			
			if (nSubBitrateTemp<16)
				nSubBitrateTemp=16;

			if (nSubBitrate > 0)
			{
				nSubBitrateTemp=nSubBitrate;
			}

			int nTempHeight=nEncoderHeight;
			if (nEncoderWidth==1920 && nEncoderHeight==540)
			{
				nTempHeight=1080;
			}

			int nSubWidthTemp = nEncoderWidth>>1;
			if (nSubWidth > 0)
			{
				nSubWidthTemp = nSubWidth;
			}
			int nSubHeightTemp= nTempHeight>>1;
			if (nSubHeight > 0)
			{
				nSubHeightTemp = nSubHeight;
			}

			if (!nFlagVideoAutoFormat)
			{
				nSubHeightTemp=0;
				nSubWidthTemp=0;
			}

			if (m_pSubVidCapChan==NULL)
			{
				m_pSubVidCapChan=VIDEC_CapChan::Create(*this);
				if (m_pSubVidCapChan==NULL)
				{
					return -1;
				}
				if (0!=m_pSubVidCapChan->Open(nSubBitrateTemp,nBitrateControlType,CodecType,1,nSubWidthTemp,nSubHeightTemp/*(nEncoderWidth<<1)/3,(nTempHeight<<1)/3)*/))
				{
					m_pSubVidCapChan->Close();
					delete m_pSubVidCapChan;
					m_pSubVidCapChan=NULL;

					return -1;
				}
				m_pSubVidCapChan->AddStreamSink(this);
				m_pCapDev->AddImageSink(m_pSubVidCapChan);

				if (m_pMediaSender!=NULL)
				{
					m_pMediaSender->StartVideo(0);
				}
			}
			else
			{
				if (nFlagVideoAutoFormat)
				{
					m_pCapDev->RemoveImageSink(m_pSubVidCapChan);
					m_pSubVidCapChan->Reset(nSubBitrateTemp,nBitrateControlType,CodecType,1,nSubWidthTemp,nSubHeightTemp);
					m_pCapDev->AddImageSink(m_pSubVidCapChan);
				}
				else
				{
					m_pSubVidCapChan->Reset(nSubBitrateTemp,nBitrateControlType,CodecType,1,nSubWidthTemp,nSubHeightTemp);
				}
			}
		}
		else
		{
			if (m_pSubVidCapChan!=NULL)
			{
				m_pSubVidCapChan->Close();
				delete m_pSubVidCapChan;
				m_pSubVidCapChan=NULL;
			}
			m_bGotSubFrame=false;
		}

		if (m_pMainVidCapChan==NULL)
		{
			m_pMainVidCapChan=VIDEC_CapChan::Create(*this);
			if (m_pMainVidCapChan==NULL)
			{
				return -1;
			}
			if (0!=m_pMainVidCapChan->Open(nBitrate,nBitrateControlType,CodecType,0,nEncoderWidth,nEncoderHeight))
			{
				m_pMainVidCapChan->Close();
				delete m_pMainVidCapChan;
				m_pMainVidCapChan=NULL;

				return -1;
			}
			m_pMainVidCapChan->AddStreamSink(this);
			m_pCapDev->AddImageSink(m_pMainVidCapChan);

			if (m_pMediaSender!=NULL)
			{
				m_pMediaSender->StartVideo(0);
			}
		}
		else
		{
			if (nFlagVideoAutoFormat)
			{
				m_pCapDev->RemoveImageSink(m_pMainVidCapChan);
				m_pMainVidCapChan->Reset(nBitrate,nBitrateControlType,CodecType,0,nEncoderWidth,nEncoderHeight);
				m_pCapDev->AddImageSink(m_pMainVidCapChan);
			}
			else
			{
				m_pMainVidCapChan->Reset(nBitrate,nBitrateControlType,CodecType,0,nEncoderWidth,nEncoderHeight);
			}
		}
	}

	if (nBitrateControlType&VIDEC_NETWORK_PRIVATE || nBitrateControlType&VIDEC_NETWORK_SATELLITE || nBitrateControlType&VIDEC_NETWORK_3G)
	{
		int nQMin=0;
		int nQMax=0;
		GetCodecInfo(nBitrate,nBitrateControlType,nBitrate,nQMin,nQMax,CodecType,false);

		nBitrate+=(nBitrate<<1);
	}
	else
	{
		nBitrate=0;
	}
	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->SetMaxBitrate(nBitrate);
	}

	m_bVideoDevIsOpen=true;
	CapChanDevMgr::Instance().AddCapChanDev(this);

	return 0;
}

int CapChanDev::OpenVideoDev(int x,int y,int cx,int cy,int nBitCount,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub,HDC hDC,int nSubWidth,int nSubHeight,int nSubBitrate,int nQSubWidth,int nQSubHeight,int nQSubBitrate)
{
	if (m_pCapDev!=NULL)
	{
		m_pCapDev->Close();
		delete m_pCapDev;
		m_pCapDev=NULL;
	}

	if (m_pWnd!=NULL)
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;
	}

	if (m_pScreen==NULL)
	{
		m_pScreen=VIDEC_Screen::Create(hDC);
		if (m_pScreen==NULL)
		{
			return -1;
		}
		if (0!=m_pScreen->Open(x,y,cx,cy,nFrameRate,nBitCount))
		{
			m_pScreen->Close();
			delete m_pScreen;
			m_pScreen=NULL;
			return -1;
		}
	}
	else
	{
		if (0!=m_pScreen->Reset(x,y,cx,cy,nFrameRate,nBitCount))
		{
			m_pScreen->Close();
			delete m_pScreen;
			m_pScreen=NULL;
			return -1;
		}
	}

	m_pScreen->GetRect(x,y,cx,cy);
	int nEncoderWidth=cx;
	int nEncoderHeight=cy;

	int nFlagVideoAutoFormat=IMXEC_CapChanDev::GetVideoAutoFormat();
	//if (!nFlagVideoAutoFormat)
	//{
	//	nEncoderWidth=0;
	//	nEncoderHeight=0;
	//}
	if (nQSub!=0)
	{
		int nQSubBitrateTemp=(nBitrate>>3);
		if (nFlagVideoAutoFormat)
		{
			nQSubBitrateTemp=(nBitrate/3);
		}
		if (nQSubBitrateTemp<16)
			nQSubBitrateTemp=16;

		if (nQSubBitrate > 0)
		{
			nQSubBitrateTemp=nQSubBitrate;
		}
		int nQSubWidthTemp = nEncoderWidth>>2;
		if (nQSubWidth > 0)
		{
			nQSubWidthTemp = nQSubWidth;
		}
		int nQSubHeightTemp= nEncoderHeight>>2;
		if (nQSubHeight > 0)
		{
			nQSubHeightTemp = nQSubHeight;
		}

		if (m_pQSubVidCapChan==NULL)
		{
			m_pQSubVidCapChan=VIDEC_CapChan::Create(*this);
			if (m_pQSubVidCapChan==NULL)
			{
				return -1;
			}
			if (0!=m_pQSubVidCapChan->Open(nQSubBitrateTemp,nBitrateControlType,CodecType,2,nQSubWidthTemp,nQSubHeightTemp))
			{
				m_pQSubVidCapChan->Close();
				delete m_pQSubVidCapChan;
				m_pQSubVidCapChan=NULL;

				return -1;
			}
			m_pQSubVidCapChan->AddStreamSink(this);
			m_pScreen->AddImageSink(m_pQSubVidCapChan);

			if (m_pMediaSender!=NULL)
			{
				m_pMediaSender->StartVideo(0);
			}
		}
		else
		{
			m_pQSubVidCapChan->Reset(nQSubBitrateTemp,nBitrateControlType,CodecType,2,nQSubWidthTemp,nQSubHeightTemp);
		}
	}
	else
	{
		if (m_pQSubVidCapChan!=NULL)
		{
			m_pQSubVidCapChan->Close();
			delete m_pQSubVidCapChan;
			m_pQSubVidCapChan=NULL;
		}
		m_bGotQSubFrame=false;
	}

	if (nSub!=0)
	{
		int nSubBitrateTemp=(nBitrate*0.35);
		if (nFlagVideoAutoFormat)
		{
			nSubBitrateTemp=(nBitrate*2/3);
			if (nFrameRate > 30)
			{
				nSubBitrateTemp=(nBitrate/2);
			}			
		}
		if (nSubBitrateTemp<16)
			nSubBitrateTemp=16;

		if (nSubBitrate > 0)
		{
			nSubBitrateTemp=nSubBitrate;
		}

		int nSubWidthTemp = nEncoderWidth>>1;
		if (nSubWidth > 0)
		{
			nSubWidthTemp = nSubWidth;
		}
		int nSubHeightTemp= nEncoderHeight>>1;
		if (nSubHeight > 0)
		{
			nSubHeightTemp = nSubHeight;
		}

		if (m_pSubVidCapChan==NULL)
		{
			m_pSubVidCapChan=VIDEC_CapChan::Create(*this);
			if (m_pSubVidCapChan==NULL)
			{
				return -1;
			}
			if (0!=m_pSubVidCapChan->Open(nSubBitrateTemp,nBitrateControlType,CodecType,1,nSubWidthTemp,nSubHeightTemp))
			{
				m_pSubVidCapChan->Close();
				delete m_pSubVidCapChan;
				m_pSubVidCapChan=NULL;

				return -1;
			}
			m_pSubVidCapChan->AddStreamSink(this);
			m_pScreen->AddImageSink(m_pSubVidCapChan);

			if (m_pMediaSender!=NULL)
			{
				m_pMediaSender->StartVideo(0);
			}
		}
		else
		{
			m_pSubVidCapChan->Reset(nSubBitrateTemp,nBitrateControlType,CodecType,1,nSubWidthTemp,nSubHeightTemp);
		}
	}
	else
	{
		if (m_pSubVidCapChan!=NULL)
		{
			m_pSubVidCapChan->Close();
			delete m_pSubVidCapChan;
			m_pSubVidCapChan=NULL;
		}
		m_bGotSubFrame=false;
	}


	if (m_pMainVidCapChan==NULL)
	{
		m_pMainVidCapChan=VIDEC_CapChan::Create(*this);
		if (m_pMainVidCapChan==NULL)
		{
			return -1;
		}
		if (0!=m_pMainVidCapChan->Open(nBitrate,nBitrateControlType,CodecType,0))
		{
			m_pMainVidCapChan->Close();
			delete m_pMainVidCapChan;
			m_pMainVidCapChan=NULL;

			return -1;
		}
		m_pMainVidCapChan->AddStreamSink(this);
		m_pScreen->AddImageSink(m_pMainVidCapChan);

		if (m_pMediaSender!=NULL)
		{
			m_pMediaSender->StartVideo(0);
		}
	}
	else
	{
		m_pMainVidCapChan->Reset(nBitrate,nBitrateControlType,CodecType,0);
	}


	if (nBitrateControlType&VIDEC_NETWORK_PRIVATE || nBitrateControlType&VIDEC_NETWORK_SATELLITE || nBitrateControlType&VIDEC_NETWORK_3G)
	{
		int nQMin=0;
		int nQMax=0;
		GetCodecInfo(nBitrate,nBitrateControlType,nBitrate,nQMin,nQMax,CodecType,false);
		
		nBitrate+=(nBitrate<<1);
	}
	else
	{
		nBitrate=0;
	}
	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->SetMaxBitrate(nBitrate);
	}

	CapChanDevMgr::Instance().AddCapChanDev(this);
	return 0;
}

int CapChanDev::OpenVideoDev(const char*cszMediaFile,int nLoop,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub,int nSubWidth,int nSubHeight,int nSubBitrate,int nQSubWidth,int nQSubHeight,int nQSubBitrate)
{
	AudioCapChanMgr::Instance().CloseAudioCapChan(this);

	if (m_pCapDev!=NULL)
	{
		m_pCapDev->Close();
		delete m_pCapDev;
		m_pCapDev=NULL;
	}

	if (m_pScreen!=NULL)
	{
		m_pScreen->Close();
		delete m_pScreen;
		m_pScreen=NULL;
	}

	if (m_pMediaFile!=NULL)
	{
		if (m_strMediaFilePathName!=cszMediaFile)
		{
			m_pMediaFile->Close();
			delete m_pMediaFile;
			m_pMediaFile=NULL;
			m_strMediaFilePathName="";

			if (m_pWnd!=NULL)
			{
				m_pWnd->Close();
				delete m_pWnd;
				m_pWnd=NULL;
			}


		}
	}

	if (m_pWnd==NULL)
	{
		m_pWnd=VIDEC_Wnd::CreateLocalVideo(*this);
		if (m_pWnd==NULL)
		{
			return -1;
		}

		if (0!=m_pWnd->Open(m_hWndParent,true,NULL,NULL,(char*)(m_strVideoWndBkImage[0]?m_strVideoWndBkImage.c_str():NULL), m_nVideoWnBkR, m_nVideoWnBkG, m_nVideoWnBkB))
		{
			m_pWnd->Close();
			delete m_pWnd;
			m_pWnd=NULL;

			return -1;
		}
	}

	if (m_pMediaFile==NULL)
	{
		m_pMediaFile=VIDEC_MediaFile::Create(*this);
		if (m_pMediaFile==NULL)
		{
			return -1;
		}
		if (0!=m_pMediaFile->Open(m_hWndParent,cszMediaFile,nLoop!=0))
		{
			m_pMediaFile->Close();
			delete m_pMediaFile;
			m_pMediaFile=NULL;
			return -1;
		}

		m_strMediaFilePathName=cszMediaFile;

		m_pMediaFile->SetMute(CapChanDevMgr::Instance().GetSpeakerMute());

		if (m_pMediaFile->HasAudio())
		{
			//Add WebRTC by zys 20160802
			//Temporary cancel by zys 20170221
			//WEBRTC暂无外部数据编码接口，媒体文件音频编码暂用AUDEC
// #if USE_WEBRTC
// 
// 			if (NULL == m_pWebRTCAudio)
// 			{
// 				m_pWebRTCAudio = MyVoiceEngineInterface::GetVoiceEngine();
// 				m_pWebRTCAudio->Init();
// 			}
// 
// //			CodecInst inst;
// //			m_pWebRTCAudio->GetCodec (CODEC_INST_INDEX, inst);
// 			m_pWebRTCAudio->SetSendCodec (g_emAudioCodecID);
// 			m_pWebRTCAudio->RegisterAudioCallback(this);
// 			m_pWebRTCAudio->StartRecording();
// 
// 			if (m_pMediaSender!=NULL)
// 			{
// 				m_pMediaSender->StartAudio(0);
// 			}
// 
// #else
			unsigned long nOldAudioID=GetAudioID();
			if (m_pAudCapChanExternal==NULL)
			{
				AUDEC_CapChanExternal*pCapChanExternal=NULL;
				AUDEC_GetEngine().AUDEC_CreateCapChanExternal(&pCapChanExternal);
				if (pCapChanExternal!=NULL)
				{
					AUDEC_CodecInst codec_inst;
					if (0!=AUDEC_Engine::AUDEC_GetCodecByID(X_AUDIO_CODEC_G7221_14_24,&codec_inst) || 0!=pCapChanExternal->Connect(&codec_inst))
					{
						pCapChanExternal->ReleaseConnections();
						delete pCapChanExternal;
						pCapChanExternal=NULL;
					}
					else
					{
						pCapChanExternal->AddStreamOut(this);
						pCapChanExternal->StartCapture();

						if (m_pMediaSender!=NULL)
						{
							m_pMediaSender->StartAudio(0);
						}

						{
							XAutoLock l(m_csAudCapChanExternal);
							m_pAudCapChanExternal=pCapChanExternal;
						}

						unsigned long nNewAudioID=GetAudioID();
						if (nNewAudioID!=nOldAudioID)
						{
							m_rCallback.OnIMXEC_CapChanCallbackAudioIDChanged(nNewAudioID);
						}
					}
				}
			}
//#endif
		}
		else
		{
			//Add WebRTC by zys 20160802
			//Temporary cancel by zys 20170221
			//WEBRTC暂无外部数据编码接口，媒体文件音频编码暂用AUDEC
// #if USE_WEBRTC
// 
// 			if (NULL != m_pWebRTCAudio)
// 			{
// 				m_pWebRTCAudio->StopRecording();
// 				m_pWebRTCAudio->DeRegisterAudioCallback();
// 				//MyVoiceEngineInterface::DestroyVoiceEngine(m_pWebRTCAudio);
// 				//m_pWebRTCAudio = NULL;
// 			}
// 
// #else
			if (m_pAudCapChanExternal!=NULL)
			{
				m_pAudCapChanExternal->ReleaseConnections();
				delete m_pAudCapChanExternal;
				m_pAudCapChanExternal=NULL;
			}
//#endif

			if (m_pMediaSender!=NULL)
			{
				m_pMediaSender->StopAudio();
			}
		}

		if (m_pWnd!=NULL)
		{
			m_pMediaFile->AddImageSink(m_pWnd);

			m_pWnd->SetEnableVideo(m_pMediaFile->HasVideo()?1:0);
		}

		if (m_pVQD)
		{
			m_pMediaFile->AddImageSink(m_pVQD);
		}
	}

	int nEncoderWidth=m_pMediaFile->GetWidth();
	int nEncoderHeight=m_pMediaFile->GetHeight();

	int nFlagVideoAutoFormat=IMXEC_CapChanDev::GetVideoAutoFormat();
	//if (!nFlagVideoAutoFormat)
	//{
	//	nEncoderWidth=0;
	//	nEncoderHeight=0;
	//}
	if (nQSub!=0)
	{
		int nQSubBitrateTemp=(nBitrate>>3);
		if (nFlagVideoAutoFormat)
		{
			nQSubBitrateTemp=(nBitrate/3);
		}
		if (nQSubBitrateTemp<16)
			nQSubBitrateTemp=16;

		if (nQSubBitrate > 0)
		{
			nQSubBitrateTemp=nQSubBitrate;
		}
		int nQSubWidthTemp = nEncoderWidth>>2;
		if (nQSubWidth > 0)
		{
			nQSubWidthTemp = nQSubWidth;
		}
		int nQSubHeightTemp= nEncoderHeight>>2;
		if (nQSubHeight > 0)
		{
			nQSubHeightTemp = nQSubHeight;
		}

		if (m_pQSubVidCapChan==NULL)
		{
			m_pQSubVidCapChan=VIDEC_CapChan::Create(*this);
			if (m_pQSubVidCapChan==NULL)
			{
				return -1;
			}
			if (0!=m_pQSubVidCapChan->Open(nQSubBitrateTemp,nBitrateControlType,CodecType,2,nQSubWidthTemp,nQSubHeightTemp))
			{
				m_pQSubVidCapChan->Close();
				delete m_pQSubVidCapChan;
				m_pQSubVidCapChan=NULL;

				return -1;
			}
			m_pQSubVidCapChan->AddStreamSink(this);
			m_pMediaFile->AddImageSink(m_pQSubVidCapChan);

			if (m_pMediaSender!=NULL)
			{
				m_pMediaSender->StartVideo(0);
			}
		}
		else
		{
			m_pQSubVidCapChan->Reset(nQSubBitrateTemp,nBitrateControlType,CodecType,2,nQSubWidthTemp,nQSubHeightTemp);
			m_pMediaFile->AddImageSink(m_pQSubVidCapChan);
		}
	}
	else
	{
		if (m_pQSubVidCapChan!=NULL)
		{
			m_pQSubVidCapChan->Close();
			delete m_pQSubVidCapChan;
			m_pQSubVidCapChan=NULL;
		}
		m_bGotQSubFrame=false;
	}

	if (nSub!=0)
	{
		int nSubBitrateTemp=(nBitrate*0.35);
		if (nFlagVideoAutoFormat)
		{
			nSubBitrateTemp=(nBitrate*2/3);
		}
		if (nSubBitrateTemp<16)
			nSubBitrateTemp=16;

		if (nSubBitrate > 0)
		{
			nSubBitrateTemp=nSubBitrate;
		}

		int nSubWidthTemp = nEncoderWidth>>1;
		if (nSubWidth > 0)
		{
			nSubWidthTemp = nSubWidth;
		}
		int nSubHeightTemp= nEncoderHeight>>1;
		if (nSubHeight > 0)
		{
			nSubHeightTemp = nSubHeight;
		}

		if (m_pSubVidCapChan==NULL)
		{
			m_pSubVidCapChan=VIDEC_CapChan::Create(*this);
			if (m_pSubVidCapChan==NULL)
			{
				return -1;
			}
			if (0!=m_pSubVidCapChan->Open(nSubBitrateTemp,nBitrateControlType,CodecType,1,nSubWidthTemp,nSubHeightTemp))
			{
				m_pSubVidCapChan->Close();
				delete m_pSubVidCapChan;
				m_pSubVidCapChan=NULL;

				return -1;
			}
			m_pSubVidCapChan->AddStreamSink(this);
			m_pMediaFile->AddImageSink(m_pSubVidCapChan);

			if (m_pMediaSender!=NULL)
			{
				m_pMediaSender->StartVideo(0);
			}
		}
		else
		{
			m_pSubVidCapChan->Reset(nSubBitrateTemp,nBitrateControlType,CodecType,1,nSubWidthTemp,nSubHeightTemp);
			m_pMediaFile->AddImageSink(m_pSubVidCapChan);
		}
	}
	else
	{
		if (m_pSubVidCapChan!=NULL)
		{
			m_pSubVidCapChan->Close();
			delete m_pSubVidCapChan;
			m_pSubVidCapChan=NULL;
		}
		m_bGotSubFrame=false;
	}

	if (m_pMainVidCapChan==NULL)
	{
		m_pMainVidCapChan=VIDEC_CapChan::Create(*this);
		if (m_pMainVidCapChan==NULL)
		{
			return -1;
		}
		if (0!=m_pMainVidCapChan->Open(nBitrate,nBitrateControlType,CodecType,0))
		{
			m_pMainVidCapChan->Close();
			delete m_pMainVidCapChan;
			m_pMainVidCapChan=NULL;

			return -1;
		}
		m_pMainVidCapChan->AddStreamSink(this);
		m_pMediaFile->AddImageSink(m_pMainVidCapChan);

		if (m_pMediaSender!=NULL)
		{
			m_pMediaSender->StartVideo(0);
		}
	}
	else
	{
		m_pMainVidCapChan->Reset(nBitrate,nBitrateControlType,CodecType,0);
		m_pMediaFile->AddImageSink(m_pMainVidCapChan);
	}


	if (nBitrateControlType&VIDEC_NETWORK_PRIVATE || nBitrateControlType&VIDEC_NETWORK_SATELLITE || nBitrateControlType&VIDEC_NETWORK_3G)
	{
		int nQMin=0;
		int nQMax=0;
		GetCodecInfo(nBitrate,nBitrateControlType,nBitrate,nQMin,nQMax,CodecType,false);
		
		nBitrate+=(nBitrate<<1);
	}
	else
	{
		nBitrate=0;
	}
	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->SetMaxBitrate(nBitrate);
	}

	CapChanDevMgr::Instance().AddCapChanDev(this);

	int nMixerScreenIndex=CapChanDevMgr::Instance().GetMixerScreenIndex();
	if (m_pMediaFile && m_pAudCapChanExternal && m_nScreenIndex>=0 && nMixerScreenIndex>=0)
	{
		if (nMixerScreenIndex==m_nScreenIndex)
		{
			AudioCapChanMgr::Instance().StartMixingWithXCapChanExternal(m_pAudCapChanExternal);
		}
		else
		{
			AudioCapChanMgr::Instance().StopMixingWithXCapChanExternal(m_pAudCapChanExternal);
		}
	}

	return 0;
}

void CapChanDev::ApplyMixerScreenIndexChanged(int nMixerScreenIndex)
{
	if (m_pMediaFile && m_pAudCapChanExternal)
	{
		if (nMixerScreenIndex==m_nScreenIndex)
		{
			AudioCapChanMgr::Instance().StartMixingWithXCapChanExternal(m_pAudCapChanExternal);
		}
		else
		{
			AudioCapChanMgr::Instance().StopMixingWithXCapChanExternal(m_pAudCapChanExternal);
		}
	}
}

//nScreenType[0:本地视频混合,1:屏幕模版区域混合（本地+远程+白板+共享文档）]
int CapChanDev::OpenVideoDev(int nScreenIndex,int nScreenType,int nWidth,int nHeight,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub,int nSubWidth,int nSubHeight,int nSubBitrate,int nQSubWidth,int nQSubHeight,int nQSubBitrate)
{
	if (nScreenIndex<0)
	{
		CloseVideoDev();
		return -1;
	}
		
	unsigned long nOldAudioID=GetAudioID();

	m_nMixerScreenIndex=nScreenIndex;
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nFrameRate=nFrameRate;
	m_nBitrate=nBitrate;
	m_nBitrateControlType=nBitrateControlType;
	m_nCodecType=CodecType;
	m_nSub=nSub;
	m_nQSub=nQSub;
	m_nSubWidth=nSubWidth;
	m_nSubHeight=nSubHeight;
	m_nSubBitrate=nSubBitrate;
	m_nQSubWidth=nQSubWidth;
	m_nQSubHeight=nQSubHeight;
	m_nQSubBitrate=nQSubBitrate;


	m_bGotSubFrame=false;
	m_bGotQSubFrame=false;

	int nEncoderWidth=nWidth;
	int nEncoderHeight=nHeight;

	int nFlagVideoAutoFormat=IMXEC_CapChanDev::GetVideoAutoFormat();
	//if (!nFlagVideoAutoFormat)
	//{
	//	nEncoderWidth=0;
	//	nEncoderHeight=0;
	//}


	if (nWidth!=m_nMixerScreenWidth ||
		nHeight!=m_nMixerScreenHeight ||
		nFrameRate!=m_nMixerScreenFrameRate)
	{
		if (m_pMixerScreen!=NULL)
		{
			m_pMixerScreen->Close();
			delete m_pMixerScreen;
			m_pMixerScreen=NULL;
		}
	}

	if (m_pMixerScreen==NULL)
	{
		m_nMixerScreenWidth=nWidth;
		m_nMixerScreenHeight=nHeight;
		m_nMixerScreenFrameRate=nFrameRate;

		m_pMixerScreen=VIDEC_MPScreen::Create();
		if (m_pMixerScreen!=NULL)
		{
			if (0!=m_pMixerScreen->Open(nWidth,nHeight,nFrameRate))
			{
				m_pMixerScreen->Close();
				delete m_pMixerScreen;
				m_pMixerScreen=NULL;
			}
			else
			{
				m_pMixerScreen->SetScreenIndex(nScreenIndex);
			}
		}
	}
	else
	{
		m_pMixerScreen->SetScreenIndex(nScreenIndex);
	}

	if (m_pMixerScreen==NULL)
	{
		CloseVideoDev();
		return -1;
	}
	
	AUDEC_Mixer::SetDefaultMixingPlayChan(0);

	{
		m_pMixerScreen->AddImageSink(this);

		if (m_pWnd!=NULL)
		{
			m_pMixerScreen->AddImageSink(m_pWnd);
		}
	}

	{
		if (m_pMainVidCapChan!=NULL)
		{
			m_pMixerScreen->AddImageSink(m_pMainVidCapChan);
		}
		if (m_pSubVidCapChan!=NULL)
		{
			m_pMixerScreen->AddImageSink(m_pSubVidCapChan);
		}
		if (m_pQSubVidCapChan!=NULL)
		{
			m_pMixerScreen->AddImageSink(m_pQSubVidCapChan);
		}
	}

	if (nQSub!=0)
	{
		int nQSubBitrateTemp=(nBitrate>>3);
		if (nFlagVideoAutoFormat)
		{
			nQSubBitrateTemp=(nBitrate/3);
		}
		if (nQSubBitrateTemp<16)
			nQSubBitrateTemp=16;

		if (nQSubBitrate > 0)
		{
			nQSubBitrateTemp=nQSubBitrate;
		}
		int nQSubWidthTemp = nEncoderWidth>>2;
		if (nQSubWidth > 0)
		{
			nQSubWidthTemp = nQSubWidth;
		}
		int nQSubHeightTemp= nEncoderHeight>>2;
		if (nQSubHeight > 0)
		{
			nQSubHeightTemp = nQSubHeight;
		}

		if (m_pQSubVidCapChan==NULL)
		{
			m_pQSubVidCapChan=VIDEC_CapChan::Create(*this);
			if (m_pQSubVidCapChan==NULL)
			{
				return -1;
			}
			if (0!=m_pQSubVidCapChan->Open(nQSubBitrateTemp,nBitrateControlType,CodecType,2,nQSubWidthTemp,nQSubHeightTemp))
			{
				m_pQSubVidCapChan->Close();
				delete m_pQSubVidCapChan;
				m_pQSubVidCapChan=NULL;

				return -1;
			}
			m_pQSubVidCapChan->AddStreamSink(this);
			m_pMixerScreen->AddImageSink(m_pQSubVidCapChan);

			if (m_pMediaSender!=NULL)
			{
				m_pMediaSender->StartVideo(0);
			}
		}
		else
		{
			m_pQSubVidCapChan->Reset(nQSubBitrateTemp,nBitrateControlType,CodecType,2,nQSubWidthTemp,nQSubHeightTemp);
		}
	}
	else
	{
		if (m_pQSubVidCapChan!=NULL)
		{
			m_pQSubVidCapChan->Close();
			delete m_pQSubVidCapChan;
			m_pQSubVidCapChan=NULL;
		}
	}

	if (nSub!=0)
	{
		int nSubBitrateTemp=(nBitrate*0.35);
		if (nFlagVideoAutoFormat)
		{
			nSubBitrateTemp=(nBitrate*2/3);
			if (nFrameRate > 30)
			{
				nSubBitrateTemp=(nBitrate/2);
			}			
		}
		if (nSubBitrateTemp<16)
			nSubBitrateTemp=16;

		if (nSubBitrate > 0)
		{
			nSubBitrateTemp=nSubBitrate;
		}

		int nSubWidthTemp = nEncoderWidth>>1;
		if (nSubWidth > 0)
		{
			nSubWidthTemp = nSubWidth;
		}
		int nSubHeightTemp= nEncoderHeight>>1;
		if (nSubHeight > 0)
		{
			nSubHeightTemp = nSubHeight;
		}

		if (m_pSubVidCapChan==NULL)
		{
			m_pSubVidCapChan=VIDEC_CapChan::Create(*this);
			if (m_pSubVidCapChan==NULL)
			{
				return -1;
			}
			if (0!=m_pSubVidCapChan->Open(nSubBitrateTemp,nBitrateControlType,CodecType,1,nSubWidthTemp,nSubHeightTemp))
			{
				m_pSubVidCapChan->Close();
				delete m_pSubVidCapChan;
				m_pSubVidCapChan=NULL;

				return -1;
			}
			m_pSubVidCapChan->AddStreamSink(this);
			m_pMixerScreen->AddImageSink(m_pSubVidCapChan);

			if (m_pMediaSender!=NULL)
			{
				m_pMediaSender->StartVideo(0);
			}
		}
		else
		{
			m_pSubVidCapChan->Reset(nSubBitrateTemp,nBitrateControlType,CodecType,1,nSubWidthTemp,nSubHeightTemp);
		}
	}
	else
	{
		if (m_pSubVidCapChan!=NULL)
		{
			m_pSubVidCapChan->Close();
			delete m_pSubVidCapChan;
			m_pSubVidCapChan=NULL;
		}
	}

	if (m_pMainVidCapChan==NULL)
	{
		m_pMainVidCapChan=VIDEC_CapChan::Create(*this);
		if (m_pMainVidCapChan==NULL)
		{
			return -1;
		}
		if (0!=m_pMainVidCapChan->Open(nBitrate,nBitrateControlType,CodecType,0))
		{
			m_pMainVidCapChan->Close();
			delete m_pMainVidCapChan;
			m_pMainVidCapChan=NULL;

			return -1;
		}
		m_pMainVidCapChan->AddStreamSink(this);
		m_pMixerScreen->AddImageSink(m_pMainVidCapChan);

		if (m_pMediaSender!=NULL)
		{
			m_pMediaSender->StartVideo(0);
		}
	}
	else
	{
		m_pMainVidCapChan->Reset(nBitrate,nBitrateControlType,CodecType,0);
	}

	if (nBitrateControlType&VIDEC_NETWORK_PRIVATE || nBitrateControlType&VIDEC_NETWORK_SATELLITE || nBitrateControlType&VIDEC_NETWORK_3G)
	{
		int nQMin=0;
		int nQMax=0;
		GetCodecInfo(nBitrate,nBitrateControlType,nBitrate,nQMin,nQMax,CodecType,false);
		
		nBitrate+=(nBitrate<<1);
	}
	else
	{
		nBitrate=0;
	}
	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->SetMaxBitrate(nBitrate);
	}

	CapChanDevMgr::Instance().AddCapChanDev(this);

	unsigned long nKeyID=(unsigned long)this;
	nKeyID|=0xFF000000;
	AudioCapChanMgr::Instance().OpenMixingAudio(nKeyID,this,NULL,AudioCapChanMgr::Instance().GetCodecID());

	unsigned long nNewAudioID=GetAudioID();
	if (nNewAudioID!=nOldAudioID)
	{
		m_rCallback.OnIMXEC_CapChanCallbackAudioIDChanged(nNewAudioID);
	}

	PlayChanMgr::Instance().ApplyMixerScreenIndexChanged(nScreenIndex);
	CapChanDevMgr::Instance().ApplyMixerScreenIndexChanged(nScreenIndex);

	return 0;
}


/******************************************************************************
* IsVideoDevOpen
*描述：获取是否已经打开视频设备
*输入：无
*输出：	无
*返回值：为true时已经打开视频设备，否则没有打开
*****************************************************************************/
bool CapChanDev::IsVideoDevOpen(void)
{
	if (m_pCapDev!=NULL ||
		m_pScreen!=NULL ||
		m_pMediaFile!=NULL ||
		m_pMixerScreen!=NULL)
	{
		return true;
	}

	return m_bVideoDevIsOpen;
}

//关闭视频设备
void CapChanDev::CloseVideoDev(void)
{
	m_bVideoDevIsOpen=false;
	CapChanDevMgr::Instance().ReleaseCapChanDev(this);

	if (m_strDVCIP.size())
	{
		VIDEC_DVCDev::SetIP(m_strDVCIP.c_str(),0,"");

		g_DVCDevMgr.RemoveDVCDev(m_strDVCIP,this);
	}

	NETEC_DVC*pDVC=m_pDVC;
	{
		XAutoLock l(m_csDVC);
		m_pDVC=NULL;
	}

	if (pDVC)
	{
		pDVC->Close();
		delete pDVC;
		pDVC=NULL;
	}

	if (m_pMainVidCapChan!=NULL)
	{
		m_pMainVidCapChan->Close();
		delete m_pMainVidCapChan;
		m_pMainVidCapChan=NULL;
	}

	if (m_pSubVidCapChan!=NULL)
	{
		m_pSubVidCapChan->Close();
		delete m_pSubVidCapChan;
		m_pSubVidCapChan=NULL;
	}

	if (m_pQSubVidCapChan!=NULL)
	{
		m_pQSubVidCapChan->Close();
		delete m_pQSubVidCapChan;
		m_pQSubVidCapChan=NULL;
	}

	if (m_pCapDev!=NULL)
	{
		m_pCapDev->Close();
		delete m_pCapDev;
		m_pCapDev=NULL;
	}

	if (m_pScreen!=NULL)
	{
		m_pScreen->Close();
		delete m_pScreen;
		m_pScreen=NULL;
	}

	if (m_pMediaFile!=NULL)
	{
		m_pMediaFile->Close();
		delete m_pMediaFile;
		m_pMediaFile=NULL;

		if (m_pWnd!=NULL)
		{
			m_pWnd->Close();
			delete m_pWnd;
			m_pWnd=NULL;
		}
	}
	m_strMediaFilePathName="";

	if (m_pMixerScreen!=NULL)
	{
		m_pMixerScreen->Close();
		delete m_pMixerScreen;
		m_pMixerScreen=NULL;
	}

	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->StopVideo();
	}

	AUDEC_Mixer::SetDefaultMixingPlayChan(CapChanDevMgr::Instance().GetMixerScreenIndex()>0?0:1);
}
 
 	
/******************************************************************************
* SetForceEncodeVideo
*描述：设置是否强制压缩视频【录像时或者本地环回测试时，设置为非0，其他时候请设置为0，以节省资源】
*输入：nForceEncodeVideo	-是否强制压缩视频【0:有人接收的时候压缩视频,非0：任何时候都压缩视频】
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CapChanDev::SetForceEncodeVideo(int nForceEncodeVideo)
{
	m_nForceEncodeVideo=nForceEncodeVideo;

	//char szDebug[1024]="";
	//sprintf(szDebug,"m_nForceEncodeVideo=%d\n",m_nForceEncodeVideo);
	//OutputDebugString(szDebug);

	return 0;
}
 	
/******************************************************************************
* SetForceEncodeAudio
*描述：设置是否强制压缩音频【录像时或者本地环回测试时，设置为非0，其他时候请设置为0，以节省资源】
*输入：nForceEncodeAudio	-是否强制压缩音频【0:有人接收的时候压缩音频,非0：任何时候都压缩音频】
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CapChanDev::SetForceEncodeAudio(int nForceEncodeAudio)
{
	m_nForceEncodeAudio=nForceEncodeAudio;

	return 0;
}

/******************************************************************************
* RequestKeyFrame
*描述：请求关键帧
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void CapChanDev::RequestKeyFrame(void)
{
	if (m_pMainVidCapChan!=NULL)
	{
		m_pMainVidCapChan->RequestKeyFrame();
	}
}

void CapChanDev::SetInvert(int nInvert)
{
	m_nInvert=nInvert;
	if (m_pCapDev!=NULL)
	{
		m_pCapDev->SetInvert(nInvert);
	}
}

int CapChanDev::GetInvert(void)
{
	if (m_pCapDev!=NULL)
	{
		return m_pCapDev->GetInvert();
	}
	return m_nInvert;
}

void CapChanDev::SetDenoise(int nDenoise)
{
	m_nDenoise=nDenoise;
	if (m_pCapDev!=NULL)
	{
		m_pCapDev->SetDenoise(nDenoise);
	}
}

int CapChanDev::GetDenoise(void)
{
	if (m_pCapDev!=NULL)
	{
		return m_pCapDev->GetDenoise();
	}
	return m_nDenoise;
}

void CapChanDev::SetLevelFixer(int nLevelFixer)
{
	m_nLevelFixer=nLevelFixer;
	if (m_pCapDev!=NULL)
	{
		m_pCapDev->SetLevelFixer(nLevelFixer);
	}
}

int CapChanDev::GetLevelFixer(void)
{
	if (m_pCapDev!=NULL)
	{
		return m_pCapDev->GetLevelFixer();
	}
	return m_nLevelFixer;
}

//设置遮掩并启动遮掩
int CapChanDev::SetupMask(RECT *RectList, int iRectCount)
{
	if (m_pCapDev!=NULL)
	{
		return m_pCapDev->SetupMask(RectList,iRectCount);
	}
	return -1;
}

//停止遮掩
int CapChanDev::StopMask(void)
{
	if (m_pCapDev!=NULL)
	{
		return m_pCapDev->StopMask();
	}
	return -1;
}

int CapChanDev::StartMotionDetect(void)
{
	if (m_pCapDev!=NULL)
	{
		return m_pCapDev->StartMotionDetect();
	}
	return -1;
}

int CapChanDev::StopMotionDetect(void)
{
	if (m_pCapDev!=NULL)
	{
		return m_pCapDev->StopMotionDetect();
	}
	return -1;
}

int CapChanDev::SetupMotionDetect(int iGrade/*灵敏度等级0-6*/,RECT *RectList, int iRectCount)
{
	if (m_pCapDev!=NULL)
	{
		return m_pCapDev->SetupMotionDetect(iGrade,RectList,iRectCount);
	}
	return -1;
}

bool CapChanDev::GetMotionImageSize(int&nWidth,int&nHeight)
{
	nWidth=704;
	nHeight=576;
	return true;
}

//显示OSD
void CapChanDev::EnableOSD(bool bEnable)
{
	m_bEnableOSD=bEnable;
	if (m_pCapDev!=NULL)
	{
		m_pCapDev->EnableOSD(bEnable);
	}
}
//是否显示OSD 
bool CapChanDev::IsOSDEnable(void)
{
	if (m_pCapDev!=NULL)
	{
		return m_pCapDev->IsOSDEnable();
	}
	return m_bEnableOSD;
}

//设置OSD 
void CapChanDev::SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft)//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT
{
	if (cszText==NULL)
	{
		m_strOSDText="";
	}
	else
	{
		m_strOSDText=cszText;
	}
	m_nOSDAlign=nAlign;
	m_nTop =nTop;
	m_nLeft=nLeft;

	if (m_pCapDev!=NULL)
	{
		m_pCapDev->SetOSDText(cszText,nAlign, nTop, nLeft);
	}
}


//设置OSD字体,字的颜色和背景色
void CapChanDev::SetOSDFont(LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	m_logfontOSD = logfont;
	m_crOSDText=crText;
	m_crOSDBk=crBk;

	if (m_pCapDev!=NULL)
	{
		m_pCapDev->SetOSDFont(logfont,crText,crBk);
	}
}

//设置视频属性
int CapChanDev::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/,bool bSharpenLocalOnly)
{
	m_nBrightness=nBrightness;
	m_nContrast=nContrast;
	m_nSaturation=nSaturation;
	m_nHue=nHue;
	m_nGamma=nGamma;
	m_nSharpen=nSharpen;
	m_bSharpenLocalOnly=bSharpenLocalOnly;

	//if (m_pWnd)
	//{
	//	m_pWnd->SetVideoParam(VIDEC_GetDefaultBrightness(),VIDEC_GetDefaultContrast(),VIDEC_GetDefaultSaturation(),VIDEC_GetDefaultHue(),VIDEC_GetDefaultGamma(),(bSharpenLocalOnly?nSharpen:VIDEC_GetDefaultSharpen()));
	//}
	//
	//if (bSharpenLocalOnly)
	//{
	//	nSharpen=VIDEC_GetDefaultSharpen();
	//}

	if (m_pCapDev)
	{
		VIDEC_AH400Dev*pAH400Dev=dynamic_cast<VIDEC_AH400Dev*>(m_pCapDev);
		if (pAH400Dev!=NULL)
		{
			pAH400Dev->SetVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
			return 0;
		}
		VIDEC_AH2KDev*pAH2KDev=dynamic_cast<VIDEC_AH2KDev*>(m_pCapDev);
		if (pAH2KDev!=NULL)
		{
			pAH2KDev->SetVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
			return 0;
		}
		VIDEC_CameraDev*pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pCapDev);
		if (pCameraDev!=NULL)
		{
			pCameraDev->SetVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
			return 0;
		}
	}
	return -1;
}
	
//int nGammaR[1-400 100]
//int nGammaG[1-400 100]
//int nGammaB[1-400 100]
int CapChanDev::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	m_nGammaR=nGammaR;
	m_nGammaG=nGammaG;
	m_nGammaB=nGammaB;

	if (m_pCapDev)
	{
		VIDEC_AH400Dev*pAH400Dev=dynamic_cast<VIDEC_AH400Dev*>(m_pCapDev);
		if (pAH400Dev!=NULL)
		{
			pAH400Dev->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
			return 0;
		}
		VIDEC_AH2KDev*pAH2KDev=dynamic_cast<VIDEC_AH2KDev*>(m_pCapDev);
		if (pAH2KDev!=NULL)
		{
			pAH2KDev->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
			return 0;
		}
		VIDEC_CameraDev*pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pCapDev);
		if (pCameraDev!=NULL)
		{
			pCameraDev->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
			return 0;
		}
	}

	return -1;
}

bool CapChanDev::HasVideoInputWnd(void)
{
	if (m_pCapDev)
	{
		VIDEC_CameraDev*pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pCapDev);
		if (pCameraDev!=NULL)
		{
			return pCameraDev->HasVideoInputWnd();
		}
	}
	return false;
}

void CapChanDev::ShowVideoInputWnd(void)
{
	if (m_pCapDev)
	{
		VIDEC_CameraDev*pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pCapDev);
		if (pCameraDev!=NULL)
		{
			pCameraDev->ShowVideoInputWnd(m_hWndParent);
		}
	}
}

bool CapChanDev::HasVideoAdjustWnd(void)
{
	if (m_pCapDev)
	{
		VIDEC_CameraDev*pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pCapDev);
		return (pCameraDev!=NULL);
	}
	return true;
}

void CapChanDev::ShowAdjustVideoWnd(void)
{
	if (m_pCapDev)
	{
		VIDEC_CameraDev*pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pCapDev);
		if (pCameraDev!=NULL)
		{
			pCameraDev->ShowAdjustVideoWnd(m_hWndParent);
		}
	}
}
	
int CapChanDev::TVTunerPutChannel(long lChannel)
{
	if (m_pCapDev)
	{
		VIDEC_CameraDev*pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pCapDev);
		if (pCameraDev!=NULL)
		{
			return pCameraDev->TVTunerPutChannel(lChannel);
		}
	}

	return -1;
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
int CapChanDev::GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return CapChan::GetVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}

//打开音频通道
//nDevID:音频采集设备号，-2从采集卡中采集音频，>=-1从声卡中采集音频
//CodecType:编码类型
int CapChanDev::OpenAudio(int nDevID,X_AUDIO_CODEC_ID CodecType)
{
	m_nAudioCapDevID=nDevID;
	m_nAudioCodecID=CodecType;

	if (m_pMediaFile!=NULL)
	{
		return 0;
	}

	if (nDevID<-1)
	{
		if (nDevID==-2)
		{
			unsigned long nOldAudioID=GetAudioID();

			//Add WebRTC by zys 20160929
#if USE_WEBRTC

			//if (NULL == m_pWebRTCAudio)
			{
 				m_pWebRTCAudio = MyVoiceEngineInterface::GetVoiceEngine();
 			
				int ret = m_pWebRTCAudio->Init();
			}

//			CodecInst inst;
//			m_pWebRTCAudio->GetCodec (CODEC_INST_INDEX, inst);
			m_pWebRTCAudio->SetSendCodec (g_emAudioCodecID);
			m_pWebRTCAudio->RegisterAudioCallback(this);
			m_pWebRTCAudio->StartRecording();

			if (m_pMediaSender!=NULL)
			{
				m_pMediaSender->StartAudio(0);
			}

			unsigned long nNewAudioID=GetAudioID();
			if (nNewAudioID!=nOldAudioID)
			{
				m_rCallback.OnIMXEC_CapChanCallbackAudioIDChanged(nNewAudioID);
			}


#else

			AudioCapChanMgr::Instance().CloseAudioCapChan(this);

			if (m_pAudCapChanExternal==NULL)
			{
				AUDEC_CapChanExternal*pCapChanExternal=NULL;
				AUDEC_GetEngine().AUDEC_CreateCapChanExternal(&pCapChanExternal);
				if (pCapChanExternal!=NULL)
				{
					AUDEC_CodecInst codec_inst;
					if (0!=AUDEC_Engine::AUDEC_GetCodecByID(X_AUDIO_CODEC_G7221_7_24,&codec_inst) || 0!=pCapChanExternal->Connect(&codec_inst))
					{
						pCapChanExternal->ReleaseConnections();
						delete pCapChanExternal;
						pCapChanExternal=NULL;
					}
					else
					{
						pCapChanExternal->AddStreamOut(this);
						pCapChanExternal->StartCapture();

						{
							XAutoLock l(m_csAudCapChanExternal);
							m_pAudCapChanExternal=pCapChanExternal;
						}

						if (m_pMediaSender!=NULL)
						{
							m_pMediaSender->StartAudio(0);
						}

						unsigned long nNewAudioID=GetAudioID();
						if (nNewAudioID!=nOldAudioID)
						{
							m_rCallback.OnIMXEC_CapChanCallbackAudioIDChanged(nNewAudioID);
						}
					}
				}
			}
#endif
		}
		else
		{
			CloseAudio();
		}
		return 0;
	}
	else
	{
		unsigned long nOldAudioID=GetAudioID();

//Add WebRTC by zys 20160929
#if USE_WEBRTC

		if (NULL != m_pWebRTCAudio)
		{
			
			m_pWebRTCAudio->StopRecording();
			m_pWebRTCAudio->DeRegisterAudioCallback();
			
			//MyVoiceEngineInterface::DestroyVoiceEngine(m_pWebRTCAudio);
			//m_pWebRTCAudio = NULL;
		}

#else
		if (m_pAudCapChanExternal!=NULL)
		{
			m_pAudCapChanExternal->ReleaseConnections();
			delete m_pAudCapChanExternal;
			m_pAudCapChanExternal=NULL;
		}
#endif
		if (m_pMixerScreen)
		{
			unsigned long nKeyID=(unsigned long)this;
			nKeyID|=0xFF000000;
			AudioCapChanMgr::Instance().OpenMixingAudio(nKeyID,this,NULL,CodecType);
		}
		else
		{
			AudioCapChanMgr::Instance().OpenAudioCapChan(this,nDevID,CodecType);
		}

		if (m_pMediaSender!=NULL)
		{
			m_pMediaSender->StopAudio();
		}

		unsigned long nNewAudioID=GetAudioID();
		if (nNewAudioID!=nOldAudioID)
		{
			m_rCallback.OnIMXEC_CapChanCallbackAudioIDChanged(nNewAudioID);
		}
	}

	return 0;
}

/******************************************************************************
* IsAudioOpen
*描述：获取是否已经打开音频
*输入：无
*输出：	nDevID		-音频采集设备号，-2从采集卡中采集音频，>=-1从声卡中采集音频
		CodecType	-编码类型
*返回值：为true时已经打开音频，否则没有打开
*****************************************************************************/
bool CapChanDev::IsAudioOpen(int&nDevID,X_AUDIO_CODEC_ID&CodecType)
{
	bool bRet=false;
	if (m_nAudioCapDevID==-2)
	{
		//Add WebRTC by zys 20160929
#if USE_WEBRTC
		bRet = (NULL != m_pWebRTCAudio);
#else
		bRet=(m_pAudCapChanExternal!=NULL);
#endif
	}
	else if (m_nAudioCapDevID>=-1)
	{
		bRet=AudioCapChanMgr::Instance().IsAudioOpen();
	}

	nDevID=m_nAudioCapDevID;
	CodecType=m_nAudioCodecID;

	return bRet;
}

//关闭音频通道
void CapChanDev::CloseAudio(void)
{
	unsigned long nKeyID=(unsigned long)this;
	nKeyID|=0xFF000000;
	AudioCapChanMgr::Instance().CloseMixingAudio(nKeyID,this);

	AudioCapChanMgr::Instance().CloseAudioCapChan(this);

	//Add WebRTC by zys 20160929
#if USE_WEBRTC

	if (NULL != m_pWebRTCAudio)
	{
		
		m_pWebRTCAudio->StopRecording();
		m_pWebRTCAudio->DeRegisterAudioCallback();
		
		//MyVoiceEngineInterface::DestroyVoiceEngine(m_pWebRTCAudio);
		//m_pWebRTCAudio = NULL;
	}

#else
	if (m_pAudCapChanExternal!=NULL)
	{
		m_pAudCapChanExternal->ReleaseConnections();
		delete m_pAudCapChanExternal;
		m_pAudCapChanExternal=NULL;
	}
#endif

	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->StopAudio();
	}
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
int CapChanDev::GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if(m_nAudioCapDevID>=-1)
	{
		return AudioCapChanMgr::Instance().GetAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}

	return CapChan::GetAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}


unsigned long CapChanDev::GetDuration(void)
{
	if (m_pMediaFile!=NULL)
	{
		return m_pMediaFile->GetDuration();
	}
	return 0;
}

unsigned long CapChanDev::GetPosition(void)
{
	if (m_pMediaFile!=NULL)
	{
		return m_pMediaFile->GetPosition();
	}
	return 0;
}

void CapChanDev::Seek(unsigned long ulSeconds)
{
	if (m_pMediaFile!=NULL)
	{
		m_pMediaFile->Seek(ulSeconds);
	}
}

void CapChanDev::Pause(void)
{
	if (m_pMediaFile!=NULL)
	{
		m_pMediaFile->Pause();
	}
}

void CapChanDev::Play(void)
{
	if (m_pMediaFile!=NULL)
	{
		m_pMediaFile->Play();
	}
}

void CapChanDev::SetMute(bool bMute)
{
	m_bMute = bMute;
	if (m_pMediaFile!=NULL)
	{
		m_pMediaFile->SetMute(bMute?1:0);
	}

#if USE_WEBRTC
	if (m_pWebRTCAudio == NULL)
	{
		m_pWebRTCAudio = MyVoiceEngineInterface::GetVoiceEngine();
	}
	m_pWebRTCAudio->SetMicMute(m_bMute);
#endif
}

bool CapChanDev::IsMute()
{
	return m_bMute;
}

/******************************************************************************
* StartRecordToAVI
*描述：开始辅流录像
*输入：	cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CapChanDev::StartRecordToAVI(const char*cszPathName,int nStandardFormat, int nCacheTimeMS)
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

#if USE_WEBRTC
	{
		XAutoLock l(m_csAudioMixer);
		if (!ms_bAudioRecordAll)
		{
			m_pAudioMixer = AudioMixerAPI::CreateMixer();
			m_nAudioMixOut = m_pAudioMixer->CreateOutputChannel(CODEC_PCM_48K, this);
			m_pAudioMixer->StartOutputMixing(m_nAudioMixOut);

			m_nAudioMixIn = m_pAudioMixer->CreateInputChannel();
			m_pAudioMixer->StartInputMixing(m_nAudioMixIn);
		}
		else
		{
			m_nAudioMixIn = AudioPlayChanMgr::Instance().m_pAudioMixer->CreateInputChannel();
			AudioPlayChanMgr::Instance().m_pAudioMixer->StartInputMixing(m_nAudioMixIn);
			AudioPlayChanMgr::Instance().AddAudioMixerCB(this);
		}
	}
#endif
	

	if (m_pMainVidCapChan!=NULL)
	{
		m_pMainVidCapChan->RequestKeyFrame();
	}
	return 0;
}

/******************************************************************************
* StopRecordToAVI
*描述：停止辅流录像
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void CapChanDev::StopRecordToAVI(void)
{
	VIDEC_AVIWriter*pAVIWriter=m_pAVIWriter;
	{
		XAutoLock l(m_csAVIWriter);
		m_pAVIWriter=NULL;
	}

#if USE_WEBRTC
	{
		XAutoLock l(m_csAudioMixer);
		if (m_nAudioMixOut)
		{
			AudioMixerAPI::Destroy(m_pAudioMixer);
			m_pAudioMixer = NULL;
			m_nAudioMixOut = 0;
			m_nAudioMixIn = 0;
		}
		else
		{
			if (m_nAudioMixIn)
			{
				AudioPlayChanMgr::Instance().m_pAudioMixer->RemoveInputChannel(m_nAudioMixIn);
				AudioPlayChanMgr::Instance().RemoveAudioMixerCB(this);
				m_nAudioMixIn = 0;
			}
		}

	}
#endif
	
	if (pAVIWriter!=NULL)
	{
		pAVIWriter->Close();
		delete pAVIWriter;
		pAVIWriter=NULL;
	}
}

void CapChanDev::SetRecording(int nFlagRecording)
{
	m_nFlagRecording=nFlagRecording;
}

void CapChanDev::SetSpeakerMute(int nMute)
{
	if (m_pMediaFile)
	{
		m_pMediaFile->SetMute(nMute);
	}
}

void CapChanDev::OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource)
{

}

void CapChanDev::OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource)
{

}

void CapChanDev::OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
{
	//if (VIDEC_HEADER_EXT_GET_KEYFRAME(pData) || bKeyFrame)
	//{
	//	char szDebug[1024]="";
	//	sprintf(szDebug,"bKeyFrame=%d %u\n",bKeyFrame,VIDEC_HEADER_EXT_GET_KEYFRAME(pData));
	//	OutputDebugString(szDebug);
	//}

	if ((pStreamSource==m_pMainVidCapChan && m_pMainVidCapChan!=NULL) ||
		(m_pMainVidCapChan==NULL && m_pSubVidCapChan==NULL && m_pQSubVidCapChan==NULL))
	{
		if (CapChanDevMgr::Instance().IsEnableVideoSend() ||
			(pStreamSource==m_pMainVidCapChan && m_pMainVidCapChan!=NULL))
		{
			VIDEC_HEADER_EXT_SET_MAIN_FRAME(pData,1);
			
			if (!m_bGotSubFrame)
			{
				if (m_pSubVidCapChan==NULL)
				{
					VIDEC_HEADER_EXT_SET_SUB_FRAME(pData,1);
				}

				if (!m_bGotQSubFrame)
				{
					if (m_pQSubVidCapChan==NULL)
					{
						VIDEC_HEADER_EXT_SET_QSUB_FRAME(pData,1);
					}
				}
			}

			SendMainVideo(pData,nLen);
		}

		if (NETEC_MediaReceiver::GetConferenceRecording() || m_nFlagRecording)
		{
			unsigned long nCurrentTimestamp=XGetTimestamp();
			if (bKeyFrame)
			{
				m_nLastGotKeyFrameTS=nCurrentTimestamp;
			}
			else
			{
				if (nCurrentTimestamp-m_nLastGotKeyFrameTS>20000)
				{
					m_nLastGotKeyFrameTS=nCurrentTimestamp;
					RequestKeyFrame();
				}
			}
		}

		{
			XAutoLock l(m_csAVIWriter);
			if (m_pAVIWriter!=NULL)
			{
				m_pAVIWriter->WriteVideo(pData,nLen);
			}
		}
	}
	else if (pStreamSource==m_pSubVidCapChan && m_pSubVidCapChan!=NULL)
	{
		m_bGotSubFrame=true;
		VIDEC_HEADER_EXT_SET_SUB_FRAME(pData,1);

		if (!m_bGotQSubFrame)
		{
			if (m_pQSubVidCapChan==NULL)
			{
				VIDEC_HEADER_EXT_SET_QSUB_FRAME(pData,1);
			}
		}

		SendSubVideo(pData,nLen);
	}
	else if (pStreamSource==m_pQSubVidCapChan && m_pQSubVidCapChan!=NULL)
	{
		m_bGotQSubFrame=true;
		VIDEC_HEADER_EXT_SET_QSUB_FRAME(pData,1);

		SendQSubVideo(pData,nLen);
	}
}

void CapChanDev::OnReceivedSubStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
{
	if (!m_nSub || !m_nQSub)
	{
		return;
	}

	if (CapChanDevMgr::Instance().IsEnableVideoSend())
	{
		m_bGotSubFrame=true;
		VIDEC_HEADER_EXT_SET_SUB_FRAME(pData,1);

		if (!m_bGotQSubFrame)
			VIDEC_HEADER_EXT_SET_QSUB_FRAME(pData,1);

		SendSubVideo(pData,nLen);
	}
}
	

void CapChanDev::OnReceivedAudioStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen)
{
	if (m_pAudCapChanExternal!=NULL)
	{
		if (CapChanDevMgr::Instance().IsEnableAudioSend())
		{
			SendAudio(pData,nLen);
		}

		bool bLoop=true;
		m_rCallback.OnIMXEC_CapChanCallbackAudioData(pData,nLen,bLoop);
	}
}

void CapChanDev::OnVIDEC_CapChanCallbackPreEncode(VIDEC_CapChan*pCapChan,int&nSkipFlag)
{
	nSkipFlag=1;

	if (m_pDVC)
	{
		if (m_pMediaSender)
		{
			if (m_pMediaSender->IsVideoRequestKeyFrame(0) ||
			    m_pMediaSender->IsVideoRequestKeyFrame(1) ||
			    m_pMediaSender->IsVideoRequestKeyFrame(2) )
			{
				m_pDVC->RequestKeyFrame();
			}
		}
		return ;
	}

	if (m_pCapDev)
	{
		VIDEC_DVCDev*pCapDev=dynamic_cast<VIDEC_DVCDev*>(m_pCapDev);
		if (pCapDev)
		{
			if (m_pMediaSender)
			{
				if (m_pMediaSender->IsVideoRequestKeyFrame(0) ||
					m_pMediaSender->IsVideoRequestKeyFrame(1) ||
					m_pMediaSender->IsVideoRequestKeyFrame(2) )
				{
					pCapDev->RequestKeyFrame();
				}
			}
			return ;
		}
	}

	if (pCapChan==m_pMainVidCapChan && m_pMainVidCapChan!=NULL)
	{
		if (CapChanDevMgr::Instance().IsEnableVideoSend()==false)
		{
			nSkipFlag=1;
			return;
		}

		if (m_pMediaSender!=NULL)
		{
			unsigned long nTimestamp=XGetTimestamp();

			nSkipFlag=(m_pMediaSender->IsVideoBlocking(0)?1:0);
			if (nSkipFlag)
			{
				if (m_pSubVidCapChan==NULL)
				{
					nSkipFlag=(m_pMediaSender->IsVideoBlocking(1)?1:0);
					if (nSkipFlag)
					{
						if (m_pQSubVidCapChan==NULL)
						{
							nSkipFlag=(m_pMediaSender->IsVideoBlocking(2)?1:0);
						}
					}
				}
			}

			if (m_nForceEncodeVideo!=0)
			{
				nSkipFlag=0;
			}

			{
				XAutoLock l(m_csAVIWriter);
				if (m_pAVIWriter!=NULL)
				{
					nSkipFlag=0;
				}
			}

			if (nSkipFlag==0)
			{
				if (m_pMediaSender->IsVideoRequestKeyFrame(0) ||
					(m_pSubVidCapChan==NULL && m_pMediaSender->IsVideoRequestKeyFrame(1)) ||
					(m_pQSubVidCapChan==NULL && m_pMediaSender->IsVideoRequestKeyFrame(2)) )
				{
					m_pMainVidCapChan->RequestKeyFrame();
				}
			}
			
			int nSubSkipFlag=(m_pMediaSender->IsVideoBlocking(1)?1:0);
			int nQSubSkipFlag=(m_pMediaSender->IsVideoBlocking(2)?1:0);
			if (!nSubSkipFlag || !nQSubSkipFlag)
			{
				if (m_pMainVidCapChan->GetEncoderType()==1)
				{
					m_pMainVidCapChan->SetSkipFrame(true);
				}
				else
				{
					m_pMainVidCapChan->SetSkipFrame(false);
				}
			}
			else
			{
				m_pMainVidCapChan->SetSkipFrame(false);
			}

			//unsigned long nDelta=XGetTimestamp()-nTimestamp;
			//if (nDelta!=0)
			//{
			//	char szDebug[1024];
			//	sprintf(szDebug,"%u\n",nDelta);
			//	OutputDebugString(szDebug);
			//}
			return;
		}
	}
	else if (pCapChan==m_pSubVidCapChan && m_pSubVidCapChan!=NULL)
	{
		if (CapChanDevMgr::Instance().IsEnableVideoSend()==false)
		{
			nSkipFlag=1;
			return;
		}

		if (m_pMediaSender!=NULL)
		{
			nSkipFlag=(m_pMediaSender->IsVideoBlocking(1)?1:0);
			if (nSkipFlag==0)
			{
				if (m_pMediaSender->IsVideoRequestKeyFrame(1))
				{
					m_pSubVidCapChan->RequestKeyFrame();
				}
			}
			return;
		}
	}
	else if (pCapChan==m_pQSubVidCapChan && m_pQSubVidCapChan!=NULL)
	{
		if (CapChanDevMgr::Instance().IsEnableVideoSend()==false)
		{
			nSkipFlag=1;
			return;
		}

		if (m_pMediaSender!=NULL)
		{
			nSkipFlag=(m_pMediaSender->IsVideoBlocking(2)?1:0);
			if (nSkipFlag==0)
			{
				if (m_pMediaSender->IsVideoRequestKeyFrame(2))
				{
					m_pQSubVidCapChan->RequestKeyFrame();
				}
			}
			return;
		}
	}
}


void CapChanDev::OnNETEC_DVCCallbackVideoFrameData(unsigned char*pFrameData,int nFrameLen)
{
	VIDEC_HEADER_EXT_SET_TIMESTAMP(pFrameData,XGetTimestamp());
	VIDEC_HEADER_EXT_SET_MAIN_FRAME(pFrameData,1);
	VIDEC_HEADER_EXT_SET_SUB_FRAME(pFrameData,1);
	VIDEC_HEADER_EXT_SET_QSUB_FRAME(pFrameData,1);

	SendMainVideo(pFrameData,nFrameLen);
}

bool CapChanDev::Write(const void*data,int len)
{
	if (m_nVideoCardType==VCT_HIK)
	{
		return false;
	}

	if (m_pMediaFile!=NULL || CapChanDevMgr::Instance().IsEnableAudioSend())
	{
		SendAudio((unsigned char*)data,len);
	}

	bool bLoop=true;
	m_nTotalBytes+=len;
	m_rCallback.OnIMXEC_CapChanCallbackAudioData((unsigned char*)data,len,bLoop);
	return true;
}

void CapChanDev::OnRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)
{
	m_rCallback.OnIMXEC_CapChanCallbackAudioSamples(pSamples,nSamples,nSampleRate,nTimestamp);

	{
		XAutoLock l(m_csAVIWriter);
		if (m_pAVIWriter!=NULL)
		{
			m_pAVIWriter->WriteAudio((unsigned char*)pSamples,nSamples<<1,nSampleRate,nTimestamp);
		}
	}
}


void CapChanDev::OnAudioCapChanUIAudioData(unsigned char*pData,int nLen,unsigned long nRefCount,bool&bLoop)
{
	if (m_pAudCapChanExternal==NULL)
	{
		if (nRefCount==0)
			m_nTotalBytes+=nLen;

		m_rCallback.OnIMXEC_CapChanCallbackAudioData(pData,nLen,bLoop);
	}
}

void CapChanDev::OnAudioCapChanUIRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)
{
	OnRecordSamples(pSamples,nSamples,nSampleRate,nTimestamp);
}

void CapChanDev::OnAudioCapChanUIGetForceEncode(int&nEncode)
{
	nEncode=m_nForceEncodeAudio;
	if (m_pAVIWriter!=NULL)
	{
		nEncode=1;
	}
}	

void CapChanDev::OnReceivedMotionDetect(int*pResultList,int nResultCount)
{
	m_rCallback.OnIMXEC_CapChanCallbackMotionDetected(pResultList,nResultCount);
}

void CapChanDev::OnReceivedAudioSamples(short*pSamples,int nSamples,int nSampleRate)
{
	{
		XAutoLock l(m_csAudCapChanExternal);
		if (m_pAudCapChanExternal!=NULL)
		{
			m_pAudCapChanExternal->WriteSamples(pSamples,nSamples,nSampleRate,false);
		}
		else
		{
			return ;
		}
	}

	{
		XAutoLock l(m_csAVIWriter);
		if (m_pAVIWriter!=NULL)
		{
			m_pAVIWriter->WriteAudio((unsigned char*)pSamples,nSamples<<1,nSampleRate,XGetTimestamp());
		}
	}
}

void CapChanDev::OnVIDEC_MediaFileCallbackPositionChanged(VIDEC_MediaFile*pMediaFile,unsigned long ulSeconds)
{
	m_rCallback.OnIMXEC_CapChanCallbackFilePositionChanged(ulSeconds);
}

void CapChanDev::OnVIDEC_MediaFileCallbackEnd(VIDEC_MediaFile*pMediaFile)
{
	m_rCallback.OnIMXEC_CapChanCallbackFileEnd();
}

void CapChanDev::OnVIDEC_MediaFileCallbackPCMData(VIDEC_MediaFile*pMediaFile,unsigned char*pPCMData,int nLen,unsigned long ulTimestamp)
{
	{
		XAutoLock l(m_csAudCapChanExternal);
		if (m_pAudCapChanExternal!=NULL)
		{
			m_pAudCapChanExternal->WriteSamples((short*)pPCMData,nLen>>1,32000,false);
		}
	}

	OnRecordSamples((const short*)pPCMData,nLen>>1,32000,ulTimestamp);
}

int CapChanDev::GetVideoShowFrameRate(void)
{
	return CapChan::GetVideoShowFrameRate();
}

int CapChanDev::GetVideoCaptureFrameRate(void)
{
	return CapChan::GetVideoCaptureFrameRate();
}

int CapChanDev::GetVideoEncodeFrameRate(void)
{
	return CapChan::GetVideoEncodeFrameRate();
}

int CapChanDev::GetVideoSendFrameRate(void)
{
	return CapChan::GetVideoSendFrameRate();
}

int CapChanDev::GetVideoShowType(void)
{
	return CapChan::GetVideoShowType();
}
int CapChanDev::GetEncoderType(void)
{	
	int nEncoderType=0;
	if (m_pMainVidCapChan!=NULL)
	{
		nEncoderType = m_pMainVidCapChan->GetEncoderType();
	}
	if (nEncoderType==0 && m_pSubVidCapChan!=NULL)
	{
		nEncoderType = m_pSubVidCapChan->GetEncoderType();
	}
	if (nEncoderType==0 && m_pQSubVidCapChan!=NULL)
	{
		nEncoderType = m_pQSubVidCapChan->GetEncoderType();
	}
	return nEncoderType;
}

void CapChanDev::OnTimer(void)
{
	if (m_nOnlyPreview!=0 ||
		m_pMediaSender==NULL ||
		m_nDevID<0)
	{
		return ;
	}

	if (m_pMediaSender->IsVideoEnable() || 
		m_pAVIWriter!=NULL || 
		(m_nScreenIndex>=0 && m_nWndIndex>=0) ||
		m_nForceEncodeVideo!=0 ||
		(m_pWnd!=NULL && m_pWnd->GetSupportPreview()!=0))
	{
		//char szDebug[1024]="";
		//sprintf(szDebug,"%u m_nForceEncodeVideo=%d m_nOnlyPreview=%d m_pMediaSender=%u m_nScreenIndex=%d m_nWndIndex=%d m_nDevID=%d\n",(unsigned long)this,m_nForceEncodeVideo,m_nOnlyPreview,(unsigned long)m_pMediaSender,m_nScreenIndex,m_nWndIndex,m_nDevID);
		//OutputDebugString(szDebug);

		if (m_pCapDev==NULL)
		{
			OpenVideoDevActual(m_nDevID,m_nWidth,m_nHeight,m_nFrameRate,m_nBitrate,m_nBitrateControlType,m_nCodecType,m_nSub,m_nQSub,m_nSubWidth,m_nSubHeight,m_nSubBitrate,m_nQSubWidth,m_nQSubHeight,m_nQSubBitrate);

			SetInvert(m_nInvert);
			SetDenoise(m_nDenoise);
			SetLevelFixer(m_nLevelFixer);
			EnableOSD(m_bEnableOSD);
			SetOSDText(m_strOSDText.c_str(),m_nOSDAlign,m_nTop,m_nLeft);
			SetOSDFont(m_logfontOSD,m_crOSDText,m_crOSDBk);
			SetVideoParam(m_nBrightness,m_nContrast,m_nSaturation,m_nHue,m_nGamma,m_nSharpen,m_bSharpenLocalOnly);
			SetVideoGammaRGB(m_nGammaR,m_nGammaG,m_nGammaB);

			m_rCallback.OnIMXEC_CapChanCallbackVideoDevOpen();
		}
		else
		{
			VIDEC_AH400Dev*pAH400Dev=dynamic_cast<VIDEC_AH400Dev*>(m_pCapDev);
			if (pAH400Dev!=NULL)
			{
				int nSkipFlag=(m_pMediaSender->IsVideoBlocking(0)?1:0);
				if (nSkipFlag==0)
				{
					if (m_pMediaSender->IsVideoRequestKeyFrame(0))
					{
						pAH400Dev->RequestKeyFrame();
						//OutputDebugString("RequestKeyFrame()\n");
					}
				}
				
				nSkipFlag=(m_pMediaSender->IsVideoBlocking(1)?1:0);
				if (nSkipFlag==0)
				{
					if (m_pMediaSender->IsVideoRequestKeyFrame(1))
					{
						if (m_nSub)
						{
							pAH400Dev->RequestSubKeyFrame();
						}
						else
						{
							pAH400Dev->RequestKeyFrame();
						}
					}

				}

				nSkipFlag=(m_pMediaSender->IsVideoBlocking(2)?1:0);
				if (nSkipFlag==0)
				{
					if (m_pMediaSender->IsVideoRequestKeyFrame(2))
					{
						if (m_nSub)
						{
							pAH400Dev->RequestSubKeyFrame();
						}
						else
						{
							pAH400Dev->RequestKeyFrame();
						}
					}
				}
			}

		}

		if (m_pCapDev)
		{
			bool bVideoSignal=m_pCapDev->HasVideoSignal()!=0;
			if (bVideoSignal!=m_bVideoSignal)
			{
				m_bVideoSignal=bVideoSignal;
				m_rCallback.OnIMXEC_CapChanCallbackVideoSignalChanged();
			}
		}
	}
	//else if (XGetTimestamp()-m_nLastOpenCapDevTime>=4000 && m_nForceEncodeVideo==0)
	//{
	//	if (m_pMainVidCapChan!=NULL)
	//	{
	//		m_pMainVidCapChan->Close();
	//		delete m_pMainVidCapChan;
	//		m_pMainVidCapChan=NULL;
	//	}

	//	if (m_pSubVidCapChan!=NULL)
	//	{
	//		m_pSubVidCapChan->Close();
	//		delete m_pSubVidCapChan;
	//		m_pSubVidCapChan=NULL;
	//	}

	//	if (m_pQSubVidCapChan!=NULL)
	//	{
	//		m_pQSubVidCapChan->Close();
	//		delete m_pQSubVidCapChan;
	//		m_pQSubVidCapChan=NULL;
	//	}

	//	if (m_pCapDev!=NULL)
	//	{
	//		m_pCapDev->Close();
	//		delete m_pCapDev;
	//		m_pCapDev=NULL;
	//	}
	//}
}

/******************************************************************************
* SendPTZ
*描述：发送PTZ【发送云台控制命令】
*输入：	nPTZAction		-云台动作
		nSpeed			-动作的速度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CapChanDev::SendPTZ (PTZ_ACTION nPTZAction,int nSpeed)
{
	return 0;
}

VIDEC_Image*CapChanDev::GetImage(void)
{
	VIDEC_Image*pImage=NULL;
	if (m_pWnd!=NULL)
	{
		pImage=m_pWnd->GetImage();
	}
	return pImage;
}


int CapChanDev::GetDVCStatus(void)
{
	return 0;
}

int CapChanDev::DVCSendData(const char*pData,int nLen)
{
	if (m_pDVC)
	{
		return m_pDVC->SendData(pData,nLen);
	}

	if (m_pCapDev)
	{
		VIDEC_DVCDev*pCapDev=dynamic_cast<VIDEC_DVCDev*>(m_pCapDev);
		if (pCapDev)
		{
			return pCapDev->SendData(pData,nLen);
		}
	}
	return -1;
}

int CapChanDev::StartVQD(unsigned long nDetectFlags)
{
	if (m_pVQD)
	{
		m_pVQD->Close();
		delete m_pVQD;
		m_pVQD=NULL;
	}

	m_pVQD=VIDEC_VQD::Create(*this);
	if (m_pVQD==NULL)
	{
		return -1;
	}
	if (0!=m_pVQD->Open(nDetectFlags))
	{
		m_pVQD->Close();
		delete m_pVQD;
		m_pVQD=NULL;

		return -1;
	}

	if (m_pMediaFile)
	{
		m_pMediaFile->AddImageSink(m_pVQD);
	}
	else
	{
		if (m_pCapDev)
		{
			m_pCapDev->AddImageSink(m_pVQD);
		}
	}
	return 0;
}

void CapChanDev::StopVQD(void)
{
	if (m_pVQD)
	{
		m_pVQD->Close();
		delete m_pVQD;
		m_pVQD=NULL;
	}
}

void CapChanDev::OnVIDEC_VQDCallbackResult(unsigned long nDetectResult)
{
	m_rCallback.OnIMXEC_CapChanCallbackVQDResult(nDetectResult);

	StopVQD();
}


//取设备个数
int IMXEC_CapChanDev::GetDevCount(void)
{
	int nDevCount=0;
	nDevCount+=VIDEC_AH2KDev::GetDevCount();
	nDevCount+=VIDEC_CameraDev::GetDevCount();
	nDevCount+=VIDEC_AH400Dev::GetDevCount();
	return nDevCount;
}

//取设备名称
int IMXEC_CapChanDev::GetDevName(int nDevID,char*szName,int nMaxCount)
{
	if (nDevID<0)
		return -1;

	int nDevCount=0;
	nDevCount=VIDEC_AH2KDev::GetDevCount();
	if (nDevID<nDevCount)
	{
		return VIDEC_AH2KDev::GetDevName(nDevID,szName,nMaxCount);
	}
	nDevID-=nDevCount;

	nDevCount=VIDEC_CameraDev::GetDevCount();
	if (nDevID<nDevCount)
	{
		return VIDEC_CameraDev::GetDevName(nDevID,szName,nMaxCount);
	}
	nDevID-=nDevCount;

	nDevCount=VIDEC_AH400Dev::GetDevCount();
	if (nDevID<nDevCount)
	{
		return VIDEC_AH400Dev::GetDevName(nDevID,szName,nMaxCount);
	}
	nDevID-=nDevCount;

	return -1;
}

//取设备类型
int IMXEC_CapChanDev::GetDevType(int nDevID,VIDEC_DEV_TYPE&nDevType)
{
	if (nDevID<0)
		return -1;

	int nDevCount=0;
	nDevCount=VIDEC_AH2KDev::GetDevCount();
	if (nDevID<nDevCount)
	{
		return VIDEC_AH2KDev::GetDevType(nDevID,nDevType);
	}
	nDevID-=nDevCount;

	nDevCount=VIDEC_CameraDev::GetDevCount();
	if (nDevID<nDevCount)
	{
		nDevType=VIDEC_DEV_SD;
		return 0;
	}
	nDevID-=nDevCount;

	nDevCount=VIDEC_AH400Dev::GetDevCount();
	if (nDevID<nDevCount)
	{
		nDevType=VIDEC_DEV_SD;
		return 0;
	}
	nDevID-=nDevCount;

	//nDevCount=VIDEC_DVCDev::GetDevCount();
	//if (nDevID<nDevCount)
	//{
	//	return VIDEC_DVCDev::GetDevType(nDevID,nDevType);
	//}
	//nDevID-=nDevCount;

	return -1;
}

//列出格式的个数
int IMXEC_CapChanDev::GetFormatCount(VIDEC_DEV_TYPE nDevType)
{
	switch (nDevType)
	{
	case VIDEC_DEV_HD:
		{
			return VIDEC_AH2KDev::GetFormatCount();
		}
		break;
	case VIDEC_DEV_SD:
		{
			return VIDEC_CameraDev::GetFormatCount();
		}
		break;

	}
	return -1;
}

//取格式名称
int IMXEC_CapChanDev::GetFormatName(VIDEC_DEV_TYPE nDevType,int nIndex,char*szName,int nMaxCount)
{
	switch (nDevType)
	{
	case VIDEC_DEV_HD:
		{
			return VIDEC_AH2KDev::GetFormatName(nIndex,szName,nMaxCount);
		}
		break;
	case VIDEC_DEV_SD:
		{
			return VIDEC_CameraDev::GetFormatName(nIndex,szName,nMaxCount);
		}
		break;
	}
	return -1;

}

//取格式大小
int IMXEC_CapChanDev::GetFormatSize(VIDEC_DEV_TYPE nDevType,int nIndex,int&nWidth,int&nHeight)
{
	switch (nDevType)
	{
	case VIDEC_DEV_HD:
		{
			return VIDEC_AH2KDev::GetFormatSize(nIndex,nWidth,nHeight);
		}
		break;
	case VIDEC_DEV_SD:
		{
			return VIDEC_CameraDev::GetFormatSize(nIndex,nWidth,nHeight);
		}
		break;
	}
	return -1;
}

//判断视频设备是否有音频输入
bool IMXEC_CapChanDev::HasAudioInput(int nDevID)
{
	if (nDevID<0)
	{
		return false;
	}

	int nTempDevCnt=VIDEC_AH2KDev::GetDevCount();
	if (nDevID<nTempDevCnt)
	{
		return VIDEC_AH2KDev::HasAudioInput(nDevID);
	}
	nDevID-=nTempDevCnt;

	nTempDevCnt=VIDEC_CameraDev::GetDevCount();
	nDevID-=nTempDevCnt;
	if (nDevID<0)
		return false;
	nTempDevCnt=VIDEC_AH400Dev::GetDevCount();
	return (nDevID<nTempDevCnt);
}


void IMXEC_CapChanDev::EnableAudioSend(bool bEnable)
{
	CapChanDevMgr::Instance().EnableAudioSend(bEnable);
}

bool IMXEC_CapChanDev::IsEnableAudioSend(void)
{
	return CapChanDevMgr::Instance().IsEnableAudioSend();
}

void IMXEC_CapChanDev::EnableVideoSend(bool bEnable)
{
	CapChanDevMgr::Instance().EnableVideoSend(bEnable);
}

bool IMXEC_CapChanDev::IsEnableVideoSend(void)
{
	return CapChanDevMgr::Instance().IsEnableVideoSend();
}

unsigned long IMXEC_CapChanDev::GetTotalBytes(void)
{
	return CapChanDevMgr::Instance().GetTotalKBytes();
}

int IMXEC_CapChanDev::GetBitrate(void)
{
	return CapChanDevMgr::Instance().GetBitrate();
}

void IMXEC_CapChanDev::ClearTotalBytes(void)
{
	return CapChanDevMgr::Instance().ClearTotalBytes();
}

void IMXEC_CapChanDev::SetVideoAutoFormat(int nFlagAuto)
{
	CapChanDevMgr::Instance().SetVideoAutoFormat(nFlagAuto);
}

int IMXEC_CapChanDev::GetVideoAutoFormat(void)
{
	return CapChanDevMgr::Instance().GetVideoAutoFormat();
}

//获取音频输入设备
int IMXEC_CapChanDev::GetMicDevNum()
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	return pVoiceEngine->GetNumOfMicDevices();
#endif
	return 0;
}

int IMXEC_CapChanDev::GetMicDevName(int nIndex, char* szName, int nMaxCount)
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	RecordDevice dev;
	int ret = pVoiceEngine->GetMicDevices(nIndex, dev);

	USES_CONVERSION;
	wstring strW = A2W_CP(dev.deviceName, CP_UTF8);

	strcpy_s(szName, nMaxCount, W2A(strW.c_str()));
	return ret;
#endif
	return 0;
}

int IMXEC_CapChanDev::SetMicDev(int nIndex)
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	return pVoiceEngine->SetMicDevice(nIndex);
#else
	return AUDEC_GetEngine().AUDEC_SetMicDev(nIndex);
#endif
}

void IMXEC_CapChanDev::SetSpeakerMute(int nMute)
{
	CapChanDevMgr::Instance().SetSpeakerMute(nMute);
}

//设置录音是否混音
void IMXEC_CapChanDev::SetAudioRecordAll(bool bRecordAll)
{
#if USE_WEBRTC
	CapChanDev::ms_bAudioRecordAll = bRecordAll;
#else
	AUDEC_GetEngine().AUDEC_SetRecordAll(1);
#endif
}

//设置是否静音
void IMXEC_CapChanDev::SetMicphoneMute(bool bMute)
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	pVoiceEngine->SetMicMute(bMute);
#else
	AUDEC_GetEngine().AUDEC_SetMicphoneMute(bMute);
#endif
}

int IMXEC_CapChanDev::SetMicphoneVolume(int nVolume)
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	return pVoiceEngine->SetMicVolume(nVolume*255/100);
#endif
}

int IMXEC_CapChanDev::GetMicphoneVolume()
{
#if USE_WEBRTC
	MyVoiceEngineInterface* pVoiceEngine = MyVoiceEngineInterface::GetVoiceEngine();
	pVoiceEngine->Init();
	int nValue = pVoiceEngine->GetMicVolume();
	int nTemp = nValue*100%255;
	nValue = nValue*100/255;
	if (nTemp != 0)
	{
		nValue++;
	}
	return nValue;
#endif
}

void CapChanDev::OnAudioCapChanUIRtpData(unsigned char*pData,int nLen)
{
#if USE_WEBRTC
	XAutoLock l(m_csAVIWriter);
	XAutoLock m(m_csAudioMixer);
	if (m_pAVIWriter)
	{
		if (m_pAudioMixer)
		{
			m_pAudioMixer->AddRtpData(m_nAudioMixIn, (const char *)pData, nLen);
		}
		else
		{
			AudioPlayChanMgr::Instance().m_pAudioMixer->AddRtpData(m_nAudioMixIn, (const char *)pData, nLen);
		}
	}
#endif
}

//Add WebRTC by zys 20160929
bool CapChanDev::SendRtp(const uint8_t* packet, size_t length, const PacketOptions& options)
{
	if (CapChanDevMgr::Instance().IsEnableAudioSend())
	{
		SendAudio((unsigned char*)packet,length);

		XAutoLock l(m_csAVIWriter);
		XAutoLock m(m_csAudioMixer);
		if (m_pAVIWriter)
		{
			if (m_pAudioMixer)
			{
				m_pAudioMixer->AddRtpData(m_nAudioMixIn, (const char *)packet, length);
			}
			else
			{
				AudioPlayChanMgr::Instance().m_pAudioMixer->AddRtpData(m_nAudioMixIn, (const char *)packet, length);
			}
		}
	}

	bool bLoop=true;
	m_rCallback.OnIMXEC_CapChanCallbackAudioData((unsigned char*)packet,length,bLoop);
	return true;
}

bool CapChanDev::SendRtcp(const uint8_t* packet, size_t length)
{
	return true;
}

int CapChanDev::OnMixedData (int out_channel, const char* rtp_data, int len)
{
	XAutoLock l(m_csAVIWriter);
	if (m_pAVIWriter)
	{
		m_pAVIWriter->WriteAudio((unsigned char *)rtp_data, len, 48000, XGetTimestamp());

		//static FILE* m_fVideoRecordFile = NULL;
		//if (!m_fVideoRecordFile)
		//{
		//	m_fVideoRecordFile = fopen("e:\\Data\\tests.pcm", "wb");
		//}
		//if (m_fVideoRecordFile)
		//{
		//	fwrite(rtp_data, 1, len, m_fVideoRecordFile);
		//	fflush(stdout);
		//}
	}
	return 0;
}