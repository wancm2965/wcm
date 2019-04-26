#include "stdafx.h"
#include "CallMgr.h"
#include "../IMXEC_IMX/IMXCallMgr.h"

#ifdef HAS_H323
#include "H323CallMgr.h"
#endif

#ifdef HAS_SIP
#ifdef USED_PJSIP
#include "../IMXEC_PJSIP/SIPCallMgr.h"
#else
#include "../IMXEC_SIP/SIPCallMgr.h"
#endif
#endif

#include "XSocketUDP.h"
#include "XSocketTCP.h"

char * g_DisConnectReason[ModeDisconnectedReasonCount]={0};

#ifndef HAS_H323
#include "../IMXEC_H323/capalitynewtype.h"
#endif

extern void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info);
extern void ClosLogFile();

void SetDisConnectReason()
{
	g_DisConnectReason[ModeDisconnectedBusy]="DisconnectedBusy";
	g_DisConnectReason[ModeDisconnectedNormal]="DisconnectedNormal";
	g_DisConnectReason[ModeDisconnectedReject]="DisconnectedReject";
	g_DisConnectReason[ModeDisconnectedUnreachable]="DisconnectedUnreachable";
	g_DisConnectReason[ModeDisconnectedUnknown]="DisconnectedUnknown";
	g_DisConnectReason[ModeDisconnectedLocal]="DisconnectedLocal";
	g_DisConnectReason[ModeDisconnectedNotFound]="DisconnectedNotFound";
	g_DisConnectReason[ModeDisconnectedServiceUnavailable]="DisconnectedServiceUnavailable";
	g_DisConnectReason[ModeDisconnectedRequestTimeout]="DisconnectedRequestTimeout";
	g_DisConnectReason[ModeDisconnectedRemote]="ModeDisconnectedRemote";
	g_DisConnectReason[ModeDisconnectedRemoteNotReg]="ModeDisconnectedRemoteNotReg";
	g_DisConnectReason[ModeDisconnectedNet]="ModeDisconnectedNet";
	g_DisConnectReason[ModeDisconnectedError]="ModeDisconnectedError";
}

IMXEC_CALL_END_REASON GetCallDisconnectReason(char * szReason)
{
	if(g_DisConnectReason[ModeDisconnectedBusy]==0) SetDisConnectReason();
	IMXEC_CALL_END_REASON nReason=ModeDisconnectedReasonCount;
	for(int i=0;i<ModeDisconnectedReasonCount;i++)
	{
		if(strcmp(g_DisConnectReason[i],szReason) ==0)
		{
			nReason=(IMXEC_CALL_END_REASON) i;
		}
	}
	return nReason;
}

char * GetCallDisconnectReason(IMXEC_CALL_END_REASON nReason)
{
	if(g_DisConnectReason[ModeDisconnectedBusy]==0) SetDisConnectReason();
	char * pReason="UnKnown";
	if(nReason >= ModeDisconnectedBusy && nReason <ModeDisconnectedReasonCount)
		pReason=g_DisConnectReason[nReason];
	return pReason;
}

unsigned short GetTCPPort(void)
{
	XSocketTCP tXSocketTCP;
	tXSocketTCP.Bind(0,0);
	unsigned short usPort=tXSocketTCP.GetLocalPort();
	tXSocketTCP.Close();
	return usPort;
}

//unsigned short GetRTPPort(unsigned short usRTPBasePort)
//{
//	if (usRTPBasePort<2000)
//	{
//		usRTPBasePort=2000;
//	}
//	else if (usRTPBasePort>64000)
//	{
//		usRTPBasePort=2000;
//	}
//	usRTPBasePort+=(usRTPBasePort%2);
//	while (true)
//	{
//		XSocketUDP tXSocketUDP;
//		if (0!=tXSocketUDP.Open(0,usRTPBasePort,false))
//		{
//			tXSocketUDP.Close();
//			usRTPBasePort+=2;
//			if (usRTPBasePort<2000)
//			{
//				usRTPBasePort=2000;
//			}
//			else if (usRTPBasePort>64000)
//			{
//				usRTPBasePort=2000;
//			}
//		}
//		else
//		{
//			tXSocketUDP.Close();
//			break;
//		}
//	}
//	return usRTPBasePort;
//}

bool CheckUDPPortEnable(unsigned short usRTPBasePort)
{
	bool bRet=false;
	XSocketUDP tXSocketUDP;
	if (0 == tXSocketUDP.Open(0,usRTPBasePort,false))
	{
		bRet=true;
	}
	tXSocketUDP.Close();
	return bRet;
}

unsigned short GetRTPPort(unsigned short usRTPBasePort)
{
	if (usRTPBasePort<2000)
	{
		usRTPBasePort=2000;
	}
	else if (usRTPBasePort>64000)
	{
		usRTPBasePort=2000;
	}
	usRTPBasePort+=(usRTPBasePort%2);
	while (true)
	{
		bool bRtp=false;
		bool bRtcp=false;
		bRtp = CheckUDPPortEnable(usRTPBasePort);
		if(bRtp) bRtcp = CheckUDPPortEnable(usRTPBasePort +1 );
		if(bRtp && bRtcp)
		{
			break;
		}
		else
		{
			usRTPBasePort+=2;
			if (usRTPBasePort<2000)
			{
				usRTPBasePort=2000;
			}
			else if (usRTPBasePort>64000)
			{
				usRTPBasePort=2000;
				break;
			}
		}

		//XSocketUDP tXSocketUDP;
		//if (0!=tXSocketUDP.Open(0,usRTPBasePort,false))
		//{
		//	tXSocketUDP.Close();
		//	usRTPBasePort+=2;
		//	if (usRTPBasePort<2000)
		//	{
		//		usRTPBasePort=2000;
		//	}
		//	else if (usRTPBasePort>64000)
		//	{
		//		usRTPBasePort=2000;
		//      break; //add
		//	}
		//}
		//else
		//{
		//	tXSocketUDP.Close();
		//	break;
		//}
	}
	return usRTPBasePort;
}

//注意 此排除了 0. 开头IP
int GetLocalFirstIP(std::string & strLocalIP)
{
	char host_name[256] ={0};
	if (gethostname(host_name, sizeof(host_name) -1 ) == SOCKET_ERROR) return -1;
	struct hostent *phe = gethostbyname(host_name);
	if (phe == 0) return -1;
	for(int i = 0; phe->h_addr_list[i] != 0; i++)
	{
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i],sizeof(struct in_addr) );
		char * p=inet_ntoa(addr);
		if(p)
		{
			strLocalIP =p;
			if( strlen(p) >0 && p[0] >='1' && strLocalIP !="127.0.0.1" ) break; //IP 0. , 127.0.0.1 , rule out
		}
	}
	return 0;
}

static std::string g_strLocalIPAddress="";
const std::string& GetLocalIPByRemoteIP(const std::string&strHost,unsigned short nPort)
{
	std::string strHostAddress="";
	XSocketUDP tXSocketUDP;
	tXSocketUDP.Connect(strHost.c_str(),nPort);
	strHostAddress=XSocket::htoa(tXSocketUDP.GetLocalIP());
	tXSocketUDP.Close();
	//add
	if (strHostAddress.size()==0) strHostAddress=NETEC_Node::GetLocalIP();
	if (strHostAddress.size()==0) GetLocalFirstIP(strHostAddress);

	g_strLocalIPAddress=strHostAddress;
	return g_strLocalIPAddress;
}


static CallMgr g_CallMgr;
CallMgr&CallMgr::Instance(void)
{
	return g_CallMgr;
}

CallMgr::CallMgr(void)
:m_strEmpty("")
,m_nCallType(IMXEC_PERSONAL_CALL)
,m_pIMXEC_CallExclusive(NULL)
,m_nAudioOnly(0)
{
	m_nRTPBasePort=12000;
	m_nEnableVideoTranscoding=0;
	m_nMainVideoScreenIndex=-1;
	m_nSecondVideoScreenIndex=-1;
	m_strExternalIP="";
	m_nExternalIPAddress=0;
	m_nCallNum=0;
	m_nUseNETECSend = 0 ;
	m_nUseCheckMedia = 1;
	m_nUseAjustMediaRate = 0;//notice
	m_nIPCPort =2120;
	m_nResStand =99;

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

	m_nUseNETECSend = 0 ;
	//m_nUseCheckMedia = 0;

#endif

	m_bUseBlackIPList = false;
	CString strPath = _T("");
	GetModuleFileName(NULL,strPath.GetBufferSetLength(512 + 1),512);
	int n = strPath.ReverseFind('\\');
	strPath = strPath.Left(n);
	strPath += _T("\\blacklist.ini");
	m_bUseBlackIPList = GetPrivateProfileInt(_T("Config"), _T("IsUseBlackList "), 0, strPath);
	if (m_bUseBlackIPList)
	{
		int nIpIndex = 0;
		while (1)
		{
			TCHAR strKey[128];
			CString strIP;
			_stprintf_s(strKey, _T("ip%d"), nIpIndex++);
			GetPrivateProfileString(_T("BlackList"), strKey, _T(""), strIP.GetBufferSetLength(128), 128, strPath);
			if (strIP != _T(""))
			{
				m_vecBlackIP.push_back(strIP.GetString());
			}
			else
			{
				break;
			}
		}
	}
}

bool CallMgr::IsBlackListIP(const char* strIP)
{
	if (m_bUseBlackIPList)
	{
		for (int i=0; i<m_vecBlackIP.size(); i++)
		{
			if(m_vecBlackIP[i].compare(strIP) == 0)
			{
				return true;
			}
		}
	}
	return false;
}

CallMgr::~CallMgr(void)
{
}

void CallMgr::AddIMXEC_Call(IMXEC_Call*pIMXEC_Call)
{
	XAutoLock l(m_csMapIMXEC_Call);
	if (pIMXEC_Call!=NULL)
	{
		m_MapIMXEC_Call[pIMXEC_Call]=pIMXEC_Call;

		if (pIMXEC_Call->GetExclusive()!=0)
		{
			m_pIMXEC_CallExclusive=pIMXEC_Call;
		}
	}
}

void CallMgr::RemoveIMXEC_Call(IMXEC_Call*pIMXEC_Call)
{
	XAutoLock l(m_csMapIMXEC_Call);
	if (pIMXEC_Call!=NULL)
	{
		m_MapIMXEC_Call.erase(pIMXEC_Call);

		if (m_pIMXEC_CallExclusive==pIMXEC_Call)
		{
			m_pIMXEC_CallExclusive=NULL;
		}
	}
}

/******************************************************************************
* MakeCall
*描述：发起呼叫
*输入：	cszUserID		-对方的用户ID或IP
		cszProtocol		-呼叫协议，"H323","SIP","IMXEC"
*输出：无
*返回值：呼叫标识
*****************************************************************************/
const char*CallMgr::MakeCall(const char*cszUserID,const char*cszProtocol)
{
	//限制同时只能有一个call在呼叫
#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )
#else

	if( IsCanCall() ==false) return m_strEmpty.c_str();
	SetCanCallNot();

#endif

	if (cszUserID==NULL || strlen(cszUserID)==0 || strlen(cszUserID)>128)
	{
		//限制同时只能有一个call在呼叫
		SetCallNumZero();
		return m_strEmpty.c_str();
	}

	if (cszProtocol!=NULL)
	{
		std::string strProtocol=cszProtocol;

#ifdef HAS_H323
		if (strProtocol=="H323")
		{
			std::string strCallID=H323CallMgr::Instance().MakeCall(cszUserID);
			if (strCallID.size())
			{
				m_MapCallID[strCallID]="H323";
				MapCallID::iterator iter=m_MapCallID.find(strCallID);
				if (iter!=m_MapCallID.end())
				{
					return iter->first.c_str();
				}
			}
			SetCallNumZero();
			return m_strEmpty.c_str();
		}
#endif

#ifdef HAS_SIP
		if (strProtocol=="SIP")
		{
			std::string strCallID=SIPCallMgr::Instance().MakeCall(cszUserID);
			if (strCallID.size())
			{
				m_MapCallID[strCallID]="SIP";
				MapCallID::iterator iter=m_MapCallID.find(strCallID);
				if (iter!=m_MapCallID.end())
				{
					return iter->first.c_str();
				}
			}
			SetCallNumZero();
			return m_strEmpty.c_str();
		}
#endif
		
		if (strProtocol=="IMX")
		{
			std::string strCallID=IMXCallMgr::Instance().MakeCall(cszUserID);
			if (strCallID.size())
			{
				m_MapCallID[strCallID]="IMX";
				MapCallID::iterator iter=m_MapCallID.find(strCallID);
				if (iter!=m_MapCallID.end())
				{
					return iter->first.c_str();
				}
			}
		}
	}
	SetCallNumZero();
	return m_strEmpty.c_str();
}


/******************************************************************************
* AcceptCall
*描述：接受呼叫请求
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::AcceptCall(const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}

	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().AcceptCall(cszCallID);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().AcceptCall(cszCallID);
		}
#endif
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().AcceptCall(cszCallID);
		}
	}
	return -1;
}

/******************************************************************************
* HangupCall
*描述：挂断呼叫
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::HangupCall(const char*cszCallID)
{
	LogFile(NULL,NULL,"",cszCallID,"CallMgr::HangupCall. 111");

	int nRet=-1;
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		LogFile(NULL,NULL,"",cszCallID,"CallMgr::HangupCall. 222");

		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			nRet = H323CallMgr::Instance().HangupCall(cszCallID);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			nRet = SIPCallMgr::Instance().HangupCall(cszCallID);
		}
#endif
		
		if (iter->second=="IMX")
		{
			nRet=IMXCallMgr::Instance().HangupCall(cszCallID);
		}
	}

	LogFile(NULL,NULL,"",cszCallID,"CallMgr::HangupCall. 222");

	return nRet;
}


/******************************************************************************
* SendPTZ
*描述：发送PTZ【发送云台控制命令】
*输入：	cszCallID		-呼叫标识
		nPTZAction		-云台动作
		nSpeed			-动作的速度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SendPTZ(cszCallID,nPTZAction,nSpeed);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SendPTZ(cszCallID,nPTZAction,nSpeed);
		}
#endif
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SendPTZ(cszCallID,nPTZAction,nSpeed);
		}
	}

	return -1;
}


/******************************************************************************
* StartSecondVideo
*描述：开始辅流
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::StartSecondVideo (const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().StartSecondVideo(cszCallID);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().StartSecondVideo(cszCallID);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().StartSecondVideo(cszCallID);
		}
	}

	return -1;
}

/******************************************************************************
* StopSecondVideo
*描述：停止辅流
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::StopSecondVideo (const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().StopSecondVideo(cszCallID);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().StopSecondVideo(cszCallID);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().StopSecondVideo(cszCallID);
		}
	}

	return -1;
}

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#else
/******************************************************************************
* SetLocalMainVideoWnd
*描述：设置本地主流视频显示的窗口
*输入：	cszCallID		-呼叫标识
		hWnd			-本地主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SetLocalMainVideoWnd(const char*cszCallID,HWND hWnd)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SetLocalMainVideoWnd(cszCallID,hWnd);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SetLocalMainVideoWnd(cszCallID,hWnd);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SetLocalMainVideoWnd(cszCallID,hWnd);
		}
	}

	return -1;
}

/******************************************************************************
* SetLocalMainVideoWndPos
*描述：设置本地主流视频显示的窗口位置
*输入：	cszCallID		-呼叫标识
		x				-窗口的左边位置
		y				-窗口的上边位置
		cx				-窗口的宽度
		cy				-窗口的高度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SetLocalMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SetLocalMainVideoWndPos(cszCallID,x,y,cx,cy);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SetLocalMainVideoWndPos(cszCallID,x,y,cx,cy);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SetLocalMainVideoWndPos(cszCallID,x,y,cx,cy);
		}
	}

	return -1;
}

/******************************************************************************
* SetLocalMainVideoParam
*描述：设置本地主流视频参数
*输入：	cszCallID		-呼叫标识
		nBrightness		-亮度值0-255
		nContrast		-对比度0-127
		nSaturation		-饱和度0-127
		nHue			-色调0-255
		nGamma			-伽马1-400
		nSharpen		-锐化0-127
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetLocalMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetLocalMainVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetLocalMainVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
		}
#endif
		
		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetLocalMainVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
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
void CallMgr::SetLocalMainVideoEnableOSD(const char*cszCallID,int nEnable)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetLocalMainVideoEnableOSD(cszCallID,nEnable);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetLocalMainVideoEnableOSD(cszCallID,nEnable);
		}
#endif
		
		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetLocalMainVideoEnableOSD(cszCallID,nEnable);
		}
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
void CallMgr::SetLocalMainVideoOSDText(const char*cszCallID,const char*cszText,int nAlign)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetLocalMainVideoOSDText(cszCallID,cszText,nAlign);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetLocalMainVideoOSDText(cszCallID,cszText,nAlign);
		}
#endif
		
		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetLocalMainVideoOSDText(cszCallID,cszText,nAlign);
		}
	}
}

/******************************************************************************
* SetLocalMainVideoOSDFont
*描述：设置本地主流视频OSD字体,字的颜色和背景色
*输入：	cszCallID		-呼叫标识
		logfont			-OSD字体
		crText			-字的颜色
		crBk			-背景色
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetLocalMainVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetLocalMainVideoOSDFont(cszCallID,logfont,crText,crBk);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetLocalMainVideoOSDFont(cszCallID,logfont,crText,crBk);
		}
#endif
		
		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetLocalMainVideoOSDFont(cszCallID,logfont,crText,crBk);
		}
	}
}

/******************************************************************************
* SetRemoteMainVideoWnd
*描述：设置远程主流视频显示的窗口
*输入：	cszCallID		-呼叫标识
		hWnd			-远程主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SetRemoteMainVideoWnd(const char*cszCallID,HWND hWnd)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SetRemoteMainVideoWnd(cszCallID,hWnd);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SetRemoteMainVideoWnd(cszCallID,hWnd);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SetRemoteMainVideoWnd(cszCallID,hWnd);
		}
	}

	return -1;
}

/******************************************************************************
* SetRemoteMainVideoWndPos
*描述：设置远程主流视频显示的窗口位置
*输入：	cszCallID		-呼叫标识
		x				-窗口的左边位置
		y				-窗口的上边位置
		cx				-窗口的宽度
		cy				-窗口的高度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SetRemoteMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SetRemoteMainVideoWndPos(cszCallID,x,y,cx,cy);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SetRemoteMainVideoWndPos(cszCallID,x,y,cx,cy);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SetRemoteMainVideoWndPos(cszCallID,x,y,cx,cy);
		}
	}

	return -1;
}

/******************************************************************************
* SetRemoteMainVideoParam
*描述：设置远程主流视频参数
*输入：	cszCallID		-呼叫标识
		nBrightness		-亮度值0-255
		nContrast		-对比度0-127
		nSaturation		-饱和度0-127
		nHue			-色调0-255
		nGamma			-伽马1-400
		nSharpen		-锐化0-127
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetRemoteMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetRemoteMainVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetRemoteMainVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
		}
#endif
		
		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetRemoteMainVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
		}
	}
}

/******************************************************************************
* SetSecondVideoWnd
*描述：设置辅流视频显示的窗口
*输入：	cszCallID		-呼叫标识
		hWnd			-远程辅流视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SetSecondVideoWnd(const char*cszCallID,HWND hWnd)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SetSecondVideoWnd(cszCallID,hWnd);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SetSecondVideoWnd(cszCallID,hWnd);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SetSecondVideoWnd(cszCallID,hWnd);
		}
	}

	return -1;
}

/******************************************************************************
* SetSecondVideoWndPos
*描述：设置辅流视频显示的窗口位置
*输入：	cszCallID		-呼叫标识
		x				-窗口的左边位置
		y				-窗口的上边位置
		cx				-窗口的宽度
		cy				-窗口的高度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SetSecondVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SetSecondVideoWndPos(cszCallID,x,y,cx,cy);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SetSecondVideoWndPos(cszCallID,x,y,cx,cy);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SetSecondVideoWndPos(cszCallID,x,y,cx,cy);
		}
	}

	return -1;
}

/******************************************************************************
* SetSecondVideoParam
*描述：设置辅流视频参数
*输入：	cszCallID		-呼叫标识
		nBrightness		-亮度值0-255
		nContrast		-对比度0-127
		nSaturation		-饱和度0-127
		nHue			-色调0-255
		nGamma			-伽马1-400
		nSharpen		-锐化0-127
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetSecondVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetSecondVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
			return;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetSecondVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
			return ;
		}
#endif
		
		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetSecondVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
		}
	}
}

/******************************************************************************
* SetSecondVideoEnableOSD
*描述：设置辅流视频是否启用OSD
*输入：	cszCallID		-呼叫标识
		nEnable			-是否启用OSD
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetSecondVideoEnableOSD(const char*cszCallID,int nEnable)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetSecondVideoEnableOSD(cszCallID,nEnable);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetSecondVideoEnableOSD(cszCallID,nEnable);
			return ;
		}
#endif
		
		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetSecondVideoEnableOSD(cszCallID,nEnable);
		}
	}
}

/******************************************************************************
* SetSecondVideoOSDText
*描述：设置辅流视频OSD文字
*输入：	cszCallID		-呼叫标识
		cszText			-OSD文字
		nAlign			-对齐方式
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetSecondVideoOSDText(const char*cszCallID,const char*cszText,int nAlign)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetSecondVideoOSDText(cszCallID,cszText,nAlign);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetSecondVideoOSDText(cszCallID,cszText,nAlign);
			return ;
		}
#endif
		
		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetSecondVideoOSDText(cszCallID,cszText,nAlign);
		}
	}
}

/******************************************************************************
* SetSecondVideoOSDFont
*描述：设置辅流视频OSD字体,字的颜色和背景色
*输入：	cszCallID		-呼叫标识
		logfont			-OSD字体
		crText			-字的颜色
		crBk			-背景色
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetSecondVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetSecondVideoOSDFont(cszCallID,logfont,crText,crBk);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetSecondVideoOSDFont(cszCallID,logfont,crText,crBk);
			return ;
		}
#endif
		
		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetSecondVideoOSDFont(cszCallID,logfont,crText,crBk);
		}
	}
}

#endif


/******************************************************************************
* GetLocalMainVideoInfo
*描述：获取本地主流视频信息
*输入：	cszCallID		-呼叫标识
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetLocalMainVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetLocalMainVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetLocalMainVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,hVideoWnd);
		}
	}

	return -1;
}

/******************************************************************************
* GetLocalMainVideoPacketStatistics
*描述：获取本地主视频的包的统计数据
*输入：	cszCallID			-呼叫标识
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	ulTotalPackets=0;
	ulLostPackets=0;
	AvgPacketLossRate=0.00;
	CurPacketLossRate=0.00;
	nBitrate=0;

	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetLocalMainVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetLocalMainVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetLocalMainVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
	}

	return -1;
}


/******************************************************************************
* GetRemoteMainVideoInfo
*描述：获取远程主流视频信息
*输入：	cszCallID		-呼叫标识
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetRemoteMainVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetRemoteMainVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetRemoteMainVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,hVideoWnd);
		}
	}

	return -1;
}


/******************************************************************************
* GetRemoteMainVideoPacketStatistics
*描述：获取远端主视频的包的统计数据
*输入：	cszCallID			-呼叫标识
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	ulTotalPackets=0;
	ulLostPackets=0;
	AvgPacketLossRate=0.00;
	CurPacketLossRate=0.00;
	nBitrate=0;

	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetRemoteMainVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetRemoteMainVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetRemoteMainVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
	}

	return -1;
}

/******************************************************************************
* GetSecondVideoInfo
*描述：获取辅流视频信息
*输入：	cszCallID		-呼叫标识
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetSecondVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetSecondVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetSecondVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,hVideoWnd);
		}
	}

	return -1;
}

/******************************************************************************
* GetSecondVideoPacketStatistics
*描述：获取辅流视频的包的统计数据
*输入：	cszCallID			-呼叫标识
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	ulTotalPackets=0;
	ulLostPackets=0;
	AvgPacketLossRate=0.00;
	CurPacketLossRate=0.00;
	nBitrate=0;

	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetSecondVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetSecondVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetSecondVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
	}

	return -1;
}

/******************************************************************************
* GetLocalAudioLevel
*描述：获取本地音频Level【实际声音大小】
*输入：	cszCallID		-呼叫标识
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetLocalAudioLevel(const char*cszCallID,int&nLevel)
{
	nLevel=0;

	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetLocalAudioLevel(cszCallID,nLevel);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetLocalAudioLevel(cszCallID,nLevel);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetLocalAudioLevel(cszCallID,nLevel);
		}
	}

	return -1;
}

/******************************************************************************
* SetLocalAudioVolume
*描述：获取本地音频音量
*输入：	cszCallID		-呼叫标识
		nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void CallMgr::SetLocalAudioVolume(const char*cszCallID,int nVolume)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetLocalAudioVolume(cszCallID,nVolume);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetLocalAudioVolume(cszCallID,nVolume);
			return ;
		}
#endif
		
		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetLocalAudioVolume(cszCallID,nVolume);
		}
	}
}

/******************************************************************************
* SetLocalAudioMute
*描述：获取本地音频是否静音
*输入：	cszCallID		-呼叫标识
		nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void CallMgr::SetLocalAudioMute(const char*cszCallID,int nMute)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetLocalAudioMute(cszCallID,nMute);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetLocalAudioMute(cszCallID,nMute);
			return ;
		}
#endif
		
		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetLocalAudioMute(cszCallID,nMute);
		}
	}
}

/******************************************************************************
* GetLocalAudioCodecID
*描述：获取本地音频编码信息
*输入：	cszCallID		-呼叫标识
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetLocalAudioCodecID(cszCallID,nAudioCodecID);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetLocalAudioCodecID(cszCallID,nAudioCodecID);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetLocalAudioCodecID(cszCallID,nAudioCodecID);
		}
	}

	return -1;
}

/******************************************************************************
* GetLocalAudioPacketStatistics
*描述：获取本机音频的包的统计数据
*输入：	cszCallID			-呼叫标识
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	ulTotalPackets=0;
	ulLostPackets=0;
	AvgPacketLossRate=0.00;
	CurPacketLossRate=0.00;
	nBitrate=0;

	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetLocalAudioPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetLocalAudioPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetLocalAudioPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
	}

	return -1;
}

/******************************************************************************
* GetRemoteAudioLevel
*描述：获取远程音频Level【实际声音大小】
*输入：	cszCallID		-呼叫标识
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetRemoteAudioLevel(const char*cszCallID,int&nLevel)
{
	nLevel=0;

	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetRemoteAudioLevel(cszCallID,nLevel);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetRemoteAudioLevel(cszCallID,nLevel);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetRemoteAudioLevel(cszCallID,nLevel);
		}
	}

	return -1;
}

/******************************************************************************
* SetRemoteAudioVolume
*描述：获取远程音频音量
*输入：	cszCallID		-呼叫标识
		nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void CallMgr::SetRemoteAudioVolume(const char*cszCallID,int nVolume)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetRemoteAudioVolume(cszCallID,nVolume);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetRemoteAudioVolume(cszCallID,nVolume);
			return ;
		}
#endif
		
		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetRemoteAudioVolume(cszCallID,nVolume);
		}
	}
}

/******************************************************************************
* SetRemoteAudioMute
*描述：获取远程音频是否静音
*输入：	cszCallID		-呼叫标识
		nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void CallMgr::SetRemoteAudioMute(const char*cszCallID,int nMute)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetRemoteAudioMute(cszCallID,nMute);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetRemoteAudioMute(cszCallID,nMute);
			return ;
		}
#endif

		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetRemoteAudioMute(cszCallID,nMute);
		}
	}
}

/******************************************************************************
* GetRemoteAudioCodecID
*描述：获取远端音频编码
*输入：	cszCallID		-呼叫标识
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetRemoteAudioCodecID(cszCallID,nAudioCodecID);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetRemoteAudioCodecID(cszCallID,nAudioCodecID);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetRemoteAudioCodecID(cszCallID,nAudioCodecID);
		}
	}

	return -1;
}

/******************************************************************************
* GetRemoteAudioPacketStatistics
*描述：获取远端音频的包的统计数据
*输入：	cszCallID			-呼叫标识
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	ulTotalPackets=0;
	ulLostPackets=0;
	AvgPacketLossRate=0.00;
	CurPacketLossRate=0.00;
	nBitrate=0;

	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetRemoteAudioPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetRemoteAudioPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetRemoteAudioPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
	}

	return -1;
}

/******************************************************************************
* GetCallSupportSecondVideo
*描述：获取呼叫是否支持辅流【H239】
*输入：	cszCallID		-呼叫标识
*输出：	无
*返回值：支持返回非0，不支持返回0
*****************************************************************************/
int CallMgr::GetCallSupportSecondVideo(const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return 0;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetCallSupportSecondVideo(cszCallID);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetCallSupportSecondVideo(cszCallID);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetCallSupportSecondVideo(cszCallID);
		}
	}

	return 0;
}


void CallMgr::OnCallRingIn(const std::string&strCallID,const std::string&strUserID,const std::string&strUserName,const std::string&strProto)
{
	m_MapCallID[strCallID]=strProto;

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnIMXEC_CallRingIn(strCallID.c_str(),strUserID.c_str(),strUserName.c_str());
		}

		++iter;
	}
}

void CallMgr::OnCallRingOut(const std::string&strCallID,const std::string&strUserID,const std::string&strUserName)
{
	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnIMXEC_CallRingOut(strCallID.c_str(),strUserID.c_str(),strUserName.c_str());
		}

		++iter;
	}
}

void CallMgr::OnCallEstablished(const std::string&strCallID,const std::string&strUserID,const std::string&strUserName)
{
	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnIMXEC_CallEstablished(strCallID.c_str(),strUserID.c_str(),strUserName.c_str());
		}
		++iter;
	}
}

void CallMgr::OnCallDestroyed(const std::string&strCallID,const std::string&strUserID,const std::string&strUserName,IMXEC_CALL_END_REASON nReason ,int callNum)
{
	LogFile(NULL,NULL,strUserID.c_str(),strCallID.c_str(),"CallMgr::OnCallDestroyed. 111");

	m_MapCallID.erase(strCallID);
	if (m_MapCallID.size()==0)
	{
#ifdef HAS_H323
		//SetRTPBasePort(H323CallMgr::Instance().GetRTPBasePort());
#endif
	}

	LogFile(NULL,NULL,strUserID.c_str(),strCallID.c_str(),"CallMgr::OnCallDestroyed. 111 00");

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			LogFile(NULL,NULL,strUserID.c_str(),strCallID.c_str(),"CallMgr::OnCallDestroyed. 111 01");
			iter->second->OnIMXEC_CallSecondVideoStop(strCallID.c_str());
			LogFile(NULL,NULL,strUserID.c_str(),strCallID.c_str(),"CallMgr::OnCallDestroyed. 111 02");
			iter->second->OnIMXEC_CallDestroyed(strCallID.c_str(),strUserID.c_str(),strUserName.c_str(),nReason);
			LogFile(NULL,NULL,strUserID.c_str(),strCallID.c_str(),"CallMgr::OnCallDestroyed. 111 03");

		}

		++iter;
	}

	LogFile(NULL,NULL,strUserID.c_str(),strCallID.c_str(),"CallMgr::OnCallDestroyed. 222");

	SetCallNumZero();
}

/******************************************************************************
* OnCallRecvdPTZ
*描述：接收到PTZ事件，当对方发送云台控制命令时，系统会调用此事件
*输入：	cszCallID		-呼叫标识
		nPTZAction		-云台动作
		nSpeed			-动作的速度
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnCallRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
	//LogFile(NULL,NULL,"",cszCallID,"CallMgr::OnCallRecvdPTZ. 111");

	{
		//XAutoLock l(m_csMapIMXEC_Call);

		MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
		while (iter!=m_MapIMXEC_Call.end())
		{
			if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
			{
				iter->second->OnIMXEC_CallRecvdPTZ(cszCallID,nPTZAction,nSpeed);
			}
			++iter;
		}
	}
	//LogFile(NULL,NULL,"",cszCallID,"CallMgr::OnCallRecvdPTZ. 222");

}

//add by Jan for DTMF [20140730]
/******************************************************************************
* OnCallRecvdDTMF
*描述：接收到DTMF事件，当对方发送DTMF命令时，系统会调用此事件
*输入：	cszCallID		-呼叫标识
		szDtmf			-DTMF信令
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnCallRecvdDTMF(const char*cszCallID,char dtmf)
{
// 	XAutoLock l(m_csMapIMXEC_Call);
// 
// 	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
// 	while (iter!=m_MapIMXEC_Call.end())
// 	{
// 		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
// 		{
// 			iter->second->OnIMXEC_CallRecvdDTMF(cszCallID,dtmf);
// 		}
// 		++iter;
// 	}
}
//end [20140730]

/******************************************************************************
* OnCallSecondVideoStart
*描述：辅流开始事件，当辅流开始时，系统会调用此事件
*输入：	strCallID		-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnCallSecondVideoStart(const std::string&strCallID)
{
	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnIMXEC_CallSecondVideoStart(strCallID.c_str());
		}
		++iter;
	}
}

/******************************************************************************
* OnCallSecondVideoStop
*描述：辅流停止事件，当辅流停止时，系统会调用此事件
*输入：	strCallID		-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnCallSecondVideoStop(const std::string&strCallID)
{
	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnIMXEC_CallSecondVideoStop(strCallID.c_str());
		}
		++iter;
	}
}

/******************************************************************************
* OnSIPVirtualLineRegisterOK
*描述：SIP注册成功事件
*输入：	nLineID		-线路标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnSIPVirtualLineRegistrationSuccess(int nLineID)
{
	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		iter->second->OnIMXEC_SIPVirtualLineRegistrationSuccess(nLineID);
		++iter;
	}
}

/******************************************************************************
* OnSIPVirtualLineRegisterFailed
*描述：SIP注册失败事件
*输入：	nLineID			-线路标识
		cszErrorCode	-错误码
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnSIPVirtualLineRegistrationError(int nLineID,const char*cszErrorCode)
{
#ifdef HAS_SIP
	pj_str_t strError=pj_str((char *)cszErrorCode);
	int nCode=pjsip_get_status_code(&strError);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		iter->second->OnIMXEC_SIPVirtualLineRegistrationError(nLineID,nCode,cszErrorCode);
		++iter;
	}
#endif
}

/******************************************************************************
* OnH323RegisterOK
*描述：H323注册成功事件
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnH323RegisterOK(const char*cszGKHost,int port)
{
	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		iter->second->OnIMXEC_H323RegistrationSuccess(cszGKHost,port);
		++iter;
	}
}

/******************************************************************************
* OnH323RegisterFailed
*描述：H323注册失败事件
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnH323RegisterFailed()
{
#ifdef HAS_H323
	int nRejectCode= H323CallMgr::Instance().m_nREGRejectReason;

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		iter->second->OnIMXEC_H323RegistrationError(nRejectCode);
		++iter;
	}
#endif

}
/******************************************************************************
* SetCallType
*描述：设置呼叫类型
*输入：	nCallType		-呼叫类型
*输出：	无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SetCallType(IMXEC_CALL_TYPE nCallType)
{
	if (nCallType==IMXEC_MP_CALL)
	{
		nCallType=IMXEC_MCU_CALL;
	}

	m_nCallType=nCallType;
	return 0;
}

/******************************************************************************
* GetCallType
*描述：获取呼叫类型
*输入：	无
*输出：	nCallType		-呼叫类型
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetCallType(IMXEC_CALL_TYPE&nCallType)
{
	nCallType=m_nCallType;
	return 0;
}

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#else

/******************************************************************************
* StartRecordLocalMainToAVI
*描述：开始本地主流录像
*输入：	cszCallID			-呼叫标识
		cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::StartRecordLocalMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().StartRecordLocalMainToAVI(cszCallID,cszPathName,nStandardFormat);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().StartRecordLocalMainToAVI(cszCallID,cszPathName,nStandardFormat);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().StartRecordLocalMainToAVI(cszCallID,cszPathName,nStandardFormat);
		}
	}

	return -1;
}

/******************************************************************************
* StopRecordLocalMainToAVI
*描述：停止本地主流录像
*输入：cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::StopRecordLocalMainToAVI(const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().StopRecordLocalMainToAVI(cszCallID);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().StopRecordLocalMainToAVI(cszCallID);
			return ;
		}
#endif

		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().StopRecordLocalMainToAVI(cszCallID);
		}
	}
}

/******************************************************************************
* StartRecordRemoteMainToAVI
*描述：开始远端主流录像
*输入：	cszCallID			-呼叫标识
		cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::StartRecordRemoteMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().StartRecordRemoteMainToAVI(cszCallID,cszPathName,nStandardFormat);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().StartRecordRemoteMainToAVI(cszCallID,cszPathName,nStandardFormat);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().StartRecordRemoteMainToAVI(cszCallID,cszPathName,nStandardFormat);
		}
	}

	return -1;
}


/******************************************************************************
* StopRecordRemoteMainToAVI
*描述：停止远端主流录像
*输入：cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::StopRecordRemoteMainToAVI(const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().StopRecordRemoteMainToAVI(cszCallID);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().StopRecordRemoteMainToAVI(cszCallID);
			return ;
		}
#endif

		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().StopRecordRemoteMainToAVI(cszCallID);
		}
	}
}

/******************************************************************************
* StartRecordSecondToAVI
*描述：开始辅流录像
*输入：	cszCallID			-呼叫标识
		cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::StartRecordSecondToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().StartRecordSecondToAVI(cszCallID,cszPathName,nStandardFormat);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().StartRecordSecondToAVI(cszCallID,cszPathName,nStandardFormat);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().StartRecordSecondToAVI(cszCallID,cszPathName,nStandardFormat);
		}
	}

	return -1;
}


/******************************************************************************
* StopRecordSecondToAVI
*描述：停止辅流录像
*输入：cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::StopRecordSecondToAVI(const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().StopRecordSecondToAVI(cszCallID);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().StopRecordSecondToAVI(cszCallID);
			return ;
		}
#endif

		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().StopRecordSecondToAVI(cszCallID);
		}
	}
}

#endif

/******************************************************************************
* GetMainAudioID
*描述：获取主流音频标识
*输入： cszCallID		-呼叫标识
*输出： nMainAudioID	-主流音频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetMainAudioID(cszCallID,nMainAudioID);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetMainAudioID(cszCallID,nMainAudioID);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetMainAudioID(cszCallID,nMainAudioID);
		}
	}
	return -1;
}

/******************************************************************************
* GetMainVideoID
*描述：获取主流视频标识
*输入： cszCallID		-呼叫标识
*输出： nMainVideoID	-主流视频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetMainVideoID(cszCallID,nMainVideoID);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetMainVideoID(cszCallID,nMainVideoID);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetMainVideoID(cszCallID,nMainVideoID);
		}
	}
	return -1;
}

/******************************************************************************
* GetSecondVideoID
*描述：获取辅流视频标识
*输入： cszCallID		-呼叫标识
*输出： nSecondVideoID	-辅流视频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetSecondVideoID(cszCallID,nSecondVideoID);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetSecondVideoID(cszCallID,nSecondVideoID);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().GetSecondVideoID(cszCallID,nSecondVideoID);
		}
	}
	return -1;
}

/******************************************************************************
* SetMainVideoScreenIndex
*描述：设置主流视频图像桌面序号
*输入： cszCallID		-呼叫标识
		nScreenIndex	-桌面序号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
		m_nMainVideoScreenIndex=nScreenIndex;
		{
			XAutoLock l(m_csMapScreenCapChanRTPMain);
			MapScreenCapChanRTP::iterator iter=m_MapScreenCapChanRTPMain.begin();
			while (iter!=m_MapScreenCapChanRTPMain.end())
			{
				ScreenCapChanRTP*pScreenCapChanRTP=iter->second;
				pScreenCapChanRTP->SetScreenIndex(m_nMainVideoScreenIndex);
				++iter;
			}
		}

#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SetMainVideoScreenIndex(cszCallID,nScreenIndex);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SetMainVideoScreenIndex(cszCallID,nScreenIndex);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SetMainVideoScreenIndex(cszCallID,nScreenIndex);
		}
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
int CallMgr::SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
		m_nSecondVideoScreenIndex=nScreenIndex;
		{
			XAutoLock l(m_csMapScreenCapChanRTPSecond);
			MapScreenCapChanRTP::iterator iter=m_MapScreenCapChanRTPSecond.begin();
			while (iter!=m_MapScreenCapChanRTPSecond.end())
			{
				ScreenCapChanRTP*pScreenCapChanRTP=iter->second;
				pScreenCapChanRTP->SetScreenIndex(m_nSecondVideoScreenIndex);
				++iter;
			}
		}

#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SetSecondVideoScreenIndex(cszCallID,nScreenIndex);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SetSecondVideoScreenIndex(cszCallID,nScreenIndex);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SetSecondVideoScreenIndex(cszCallID,nScreenIndex);
		}
	}
	return -1;
}

/******************************************************************************
* SetSecondVideoScreenWnd
*描述：设置辅流视频所在屏幕窗口位置
*输入：	cszCallID		-呼叫标识
		nScreenIndex	-屏幕号
		nWndIndex		-窗口号
		nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetSecondVideoScreenWnd(cszCallID,nScreenIndex,nWndIndex,nFullScreen);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetSecondVideoScreenWnd(cszCallID,nScreenIndex,nWndIndex,nFullScreen);
			return ;
		}
#endif

		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetSecondVideoScreenWnd(cszCallID,nScreenIndex,nWndIndex,nFullScreen);
		}
	}
}

/******************************************************************************
* SetRemoteVideoScreenWnd
*描述：设置远端视频所在屏幕窗口位置
*输入：	cszCallID		-呼叫标识
		nScreenIndex	-屏幕号
		nWndIndex		-窗口号
		nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetRemoteVideoScreenWnd(cszCallID,nScreenIndex,nWndIndex,nFullScreen);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetRemoteVideoScreenWnd(cszCallID,nScreenIndex,nWndIndex,nFullScreen);
			return ;
		}
#endif

		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetRemoteVideoScreenWnd(cszCallID,nScreenIndex,nWndIndex,nFullScreen);
		}
	}
}

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#else

/******************************************************************************
* SetLocalRecordCallback
*描述：设置本地录像回调
*输入：	cszCallID		-呼叫标识
		pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetLocalRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetLocalRecordCallback(cszCallID,pRecordCallback);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetLocalRecordCallback(cszCallID,pRecordCallback);
			return ;
		}
#endif

		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetLocalRecordCallback(cszCallID,pRecordCallback);
		}
	}
}

/******************************************************************************
* SetRemoteRecordCallback
*描述：设置远程录像回调
*输入：	cszCallID		-呼叫标识
		pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetRemoteRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetRemoteRecordCallback(cszCallID,pRecordCallback);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetRemoteRecordCallback(cszCallID,pRecordCallback);
			return ;
		}
#endif

		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetRemoteRecordCallback(cszCallID,pRecordCallback);
		}
	}
}

/******************************************************************************
* SetSecondRecordCallback
*描述：设置辅流录像回调
*输入：	cszCallID		-呼叫标识
		pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetSecondRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetSecondRecordCallback(cszCallID,pRecordCallback);
			return ;
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetSecondRecordCallback(cszCallID,pRecordCallback);
			return ;
		}
#endif

		if (iter->second=="IMX")
		{
			IMXCallMgr::Instance().SetSecondRecordCallback(cszCallID,pRecordCallback);
		}
	}
}

/******************************************************************************
* SaveLocalMainVideoToBMPFile
*描述：保存本地主流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
		cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SaveLocalMainVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SaveLocalMainVideoToBMPFile(cszCallID,cszPathName);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SaveLocalMainVideoToBMPFile(cszCallID,cszPathName);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SaveLocalMainVideoToBMPFile(cszCallID,cszPathName);
		}
	}

	return -1;
}

/******************************************************************************
* SaveRemoteMainVideoToBMPFile
*描述：保存远程主流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
		cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SaveRemoteMainVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SaveRemoteMainVideoToBMPFile(cszCallID,cszPathName);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SaveRemoteMainVideoToBMPFile(cszCallID,cszPathName);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SaveRemoteMainVideoToBMPFile(cszCallID,cszPathName);
		}
	}

	return -1;
}

/******************************************************************************
* SaveSecondVideoToBMPFile
*描述：保存辅流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
		cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SaveSecondVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SaveSecondVideoToBMPFile(cszCallID,cszPathName);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SaveSecondVideoToBMPFile(cszCallID,cszPathName);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SaveSecondVideoToBMPFile(cszCallID,cszPathName);
		}
	}

	return -1;
}

/******************************************************************************
* HotDisplayVideo
*描述：视频显示支持显示器热插拔接口
*输入：	cszCallID		    -呼叫标识
hWndLocal			-本地主视频显示的窗口句柄
hWndRemote			-远程主视频显示的窗口句柄
hWndSecond         	-远程辅流视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::HotDisplayVideo(const char*cszCallID,HWND hWndLocal,HWND hWndRemote,HWND hWndSecond)
{

	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().HotDisplayVideo(cszCallID,hWndLocal,hWndRemote,hWndSecond);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().HotDisplayVideo(cszCallID,hWndLocal,hWndRemote,hWndSecond);
		}
#endif
		
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().HotDisplayVideo(cszCallID,hWndLocal,hWndRemote,hWndSecond);
		}
	}

	return -1;

}

#endif


/******************************************************************************
* SendDTMF
*描述：发送DTMF
*输入：	cszCallID		-呼叫标识
		nDTMF			-DTMF
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SendDTMF(const char*cszCallID,const char nDTMF)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().SendDTMF(cszCallID,nDTMF);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().SendDTMF(cszCallID,nDTMF);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SendDTMF(cszCallID,nDTMF);
		}
	}

	return -1;
}

/******************************************************************************
* StartMixingWithWaveFile
*描述：开始对呼叫进行WAVE文件混音，对方会听到WAVE文件播放的声音
*输入：	cszCallID		-呼叫标识
		cszPathName		-WAVE文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::StartMixingWithWaveFile(const char*cszCallID,const char*cszPathName)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().StartMixingWithWaveFile(cszCallID,cszPathName);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().StartMixingWithWaveFile(cszCallID,cszPathName);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().StartMixingWithWaveFile(cszCallID,cszPathName);
		}
	}

	return -1;
}

/******************************************************************************
* StopMixingWithWaveFile
*描述：停止对呼叫进行WAVE文件混音
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::StopMixingWithWaveFile(const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().StopMixingWithWaveFile(cszCallID);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().StopMixingWithWaveFile(cszCallID);
		}
#endif

		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().StopMixingWithWaveFile(cszCallID);
		}
	}

	return -1;
}


/******************************************************************************
* SetAudioOnly
*描述：设置是否只支持音频
*输入：	nAudioOnly		-是否只支持音频【0：支持视频音频，非0：只支持音频】
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::SetAudioOnly(int nAudioOnly)
{
	m_nAudioOnly=nAudioOnly;
	return 0;
}

int CallMgr::GetAudioOnly(void)
{
	//m_nAudioOnly=1;
	return m_nAudioOnly;
}

/******************************************************************************
* GetCallProtocol
*描述：获取呼叫协议
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回呼叫协议，失败返回空串""
*****************************************************************************/
const char*CallMgr::GetCallProtocol(const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return m_strEmpty.c_str();
	}

	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
		return iter->second.c_str();
	}

	return m_strEmpty.c_str();
}

/******************************************************************************
* GetHasVideo
*描述：获取是否只有视频
*输入：cszCallID		-呼叫标识
*输出：无
*返回值：如果有视频，那么返回非0，否则返回0
*****************************************************************************/
int CallMgr::GetHasVideo(const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return 0;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return 1;//H323CallMgr::Instance().GetHasVideo(cszCallID);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetHasVideo(cszCallID);
		}
#endif
		if (iter->second=="IMX")
		{
			return 1;//IMXCallMgr::Instance().GetHasVideo(cszCallID);
		}
	}

	return 0;
}

unsigned short CallMgr::GetRTPBasePort(void)
{
	return m_nRTPBasePort;
}

void CallMgr::SetRTPBasePort(unsigned short nRTPBasePort)
{
	m_nRTPBasePort=nRTPBasePort;
}


/******************************************************************************
* SetEnableVideoTranscoding
*描述：设置是否启用视频转码
*输入：nEnable		-是否启用视频转码【0：不启用视频转码，非0：启用视频转码】
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetEnableVideoTranscoding(int nEnable)
{
	m_nEnableVideoTranscoding=nEnable;
}

/******************************************************************************
* GetEnableVideoTranscoding
*描述：获取是否启用视频转码
*输入：无
*输出：无
*返回值：如果启用视频转码，那么返回非0，否则返回0
*****************************************************************************/
int CallMgr::GetEnableVideoTranscoding(void)
{
	return m_nEnableVideoTranscoding;
}

/******************************************************************************
* GetSecondVideoDirection
*描述：获取辅流的方向
*输入：	cszCallID		-呼叫标识
*输出： nDir			-辅流的方向
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CallMgr::GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir)
{
	nDir=IMXEC_CALL_DIR_UNKOWN;

	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetSecondVideoDirection(cszCallID,nDir);
		}
#endif
#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().GetSecondVideoDirection(cszCallID,nDir);
		}
#endif

	}

	return -1;
}




//H235 Media use

/******************************************************************************
* GetH235AuthCheckState
*描述：获取H235验证检查状态 用于取呼叫验证及为网守时作验证检查
*输入：	cszCallID		  -呼叫标识
*输出： nState -验证检查状态
nLength -返回验证失败原因字符串长度
*返回值：成功返回验证失败原因，失败返回空
*****************************************************************************/

const char*  CallMgr::GetH235AuthCheckState(const char*cszCallID,IMXEC_CALL_H235AUTH_STATE &nState,int &nLength)
{

	nState=IMXEC_CALL_H235AUTH_UNKOWN;
	nLength=0;

	if (cszCallID==NULL || strlen(cszCallID)==0)
	{		
		return NULL;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetH235AuthCheckState(cszCallID,nState,nLength);
		}
#endif
	}
	
	return NULL;

}


/******************************************************************************
* OnChannelRecvMediaEncryption
*描述：接收媒体通道已加密状态通知，当打开加密的接收媒体通道时，系统会调用此事件
*输入：	cszCallID		-呼叫标识
		nMediaType		-媒体类型
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnChannelRecvMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnChannelRecvMediaEncryption(cszCallID,nMediaType);
		}
		++iter;
	}
}

/******************************************************************************
* OnChannelSendMediaEncryption
*描述：发送媒体通道已加密状态通知，当打开加密的发送媒体通道时，系统会调用此事件
*输入：	cszCallID		-呼叫标识
		nMediaType		-媒体类型
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnChannelSendMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnChannelSendMediaEncryption(cszCallID,nMediaType);
		}
		++iter;
	}
}





bool CallMgr::ExcludePlayChan(unsigned long nAudioID,AUDEC_PlayChan*pPlayChan)
{
	if (m_nCallType!=IMXEC_MP_CALL && m_nCallType!=IMXEC_MCU_CALL)
	{
		return false;
	}

#ifdef HAS_H323
	if (H323CallMgr::Instance().ExcludePlayChan(nAudioID,pPlayChan))
	{
		return true;
	}
#endif

#ifdef HAS_SIP
	if (SIPCallMgr::Instance().ExcludePlayChan(nAudioID,pPlayChan))
	{
		return true;
	}
#endif

	return false;
}


int CallMgr::OpenLocalMainVideoCapDev(MediaCall*pMediaCall,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType)
{
	IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
	CallMgr::Instance().GetCallType(nCallType);
	if (nCallType==IMXEC_MCU_CALL)
	{
		ScreenCapChanRTP*pScreenCapChanRTP=NULL;
		char szKey[1024]="";
		sprintf(szKey,"%dx%dx%dx%d",nWidth,nHeight,codecType,nBitrate);
		XAutoLock l(m_csMapScreenCapChanRTPMain);
		MapScreenCapChanRTP::iterator iter=m_MapScreenCapChanRTPMain.find(szKey);
		if (iter!=m_MapScreenCapChanRTPMain.end())
		{
			pScreenCapChanRTP=iter->second;
		}
		else
		{
			pScreenCapChanRTP=new ScreenCapChanRTPMain();
			if (0!=pScreenCapChanRTP->Open(nWidth,nHeight,nFrameRate,codecType,nBitrate))
			{
				pScreenCapChanRTP->Close();
				delete pScreenCapChanRTP;
				pScreenCapChanRTP=NULL;
				return -1;
			}
			pScreenCapChanRTP->SetScreenIndex(m_nMainVideoScreenIndex);
			m_MapScreenCapChanRTPMain[szKey]=pScreenCapChanRTP;
		}
		pScreenCapChanRTP->AddMediaCall(pMediaCall);
		pScreenCapChanRTP->RequestKeyFrame(NULL);
	}

	return 0;
}


void CallMgr::OnRequestLocalMainVideoKeyFrame(MediaCall*pMediaCall)
{
	ScreenCapChanRTP*pScreenCapChanRTP=NULL;
	XAutoLock l(m_csMapScreenCapChanRTPMain);
	MapScreenCapChanRTP::iterator iter=m_MapScreenCapChanRTPMain.begin();
	while (iter!=m_MapScreenCapChanRTPMain.end())
	{
		pScreenCapChanRTP=iter->second;
		if (pScreenCapChanRTP->RequestKeyFrame(pMediaCall))
		{
			break;
		}
		++iter;
	}
}

void CallMgr::CloseLocalMainVideoCapDev(MediaCall*pMediaCall)
{
	ScreenCapChanRTP*pScreenCapChanRTP=NULL;

	{
		XAutoLock l(m_csMapScreenCapChanRTPMain);
		MapScreenCapChanRTP::iterator iter=m_MapScreenCapChanRTPMain.begin();
		while (iter!=m_MapScreenCapChanRTPMain.end())
		{
			pScreenCapChanRTP=iter->second;
			if (pScreenCapChanRTP->RemoveMediaCall(pMediaCall))
			{
				if (pScreenCapChanRTP->GetMediaCallCount()>0)
				{
					pScreenCapChanRTP=NULL;
				}
				else
				{
					m_MapScreenCapChanRTPMain.erase(iter);
				}

				break;
			}
			else
			{
				pScreenCapChanRTP=NULL;
			}
			++iter;
		}
	}

	if (pScreenCapChanRTP)
	{
		pScreenCapChanRTP->Close();
		delete pScreenCapChanRTP;
		pScreenCapChanRTP=NULL;
	}
}

int CallMgr::OpenLocalSecondVideoCapDev(MediaCall*pMediaCall,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate)
{
	IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
	CallMgr::Instance().GetCallType(nCallType);
	if (nCallType==IMXEC_MCU_CALL)
	{
		ScreenCapChanRTP*pScreenCapChanRTP=NULL;
		char szKey[1024]="";
		sprintf(szKey,"%dx%dx%dx%d",nWidth,nHeight,codecType,nBitrate);
		XAutoLock l(m_csMapScreenCapChanRTPSecond);
		MapScreenCapChanRTP::iterator iter=m_MapScreenCapChanRTPSecond.find(szKey);
		if (iter!=m_MapScreenCapChanRTPSecond.end())
		{
			pScreenCapChanRTP=iter->second;
		}
		else
		{
			pScreenCapChanRTP=new ScreenCapChanRTPSecond();
			if (0!=pScreenCapChanRTP->Open(nWidth,nHeight,nFrameRate,codecType,nBitrate))
			{
				pScreenCapChanRTP->Close();
				delete pScreenCapChanRTP;
				pScreenCapChanRTP=NULL;
				return -1;
			}
			pScreenCapChanRTP->SetScreenIndex(m_nSecondVideoScreenIndex);
			m_MapScreenCapChanRTPSecond[szKey]=pScreenCapChanRTP;
		}
		pScreenCapChanRTP->AddMediaCall(pMediaCall);
		pScreenCapChanRTP->RequestKeyFrame(NULL);
	}

	return 0;
}


void CallMgr::OnRequestSecondVideoKeyFrame(MediaCall*pMediaCall)
{
	ScreenCapChanRTP*pScreenCapChanRTP=NULL;
	XAutoLock l(m_csMapScreenCapChanRTPSecond);
	MapScreenCapChanRTP::iterator iter=m_MapScreenCapChanRTPSecond.begin();
	while (iter!=m_MapScreenCapChanRTPSecond.end())
	{
		pScreenCapChanRTP=iter->second;
		if (pScreenCapChanRTP->RequestKeyFrame(pMediaCall))
		{
			break;
		}
		++iter;
	}
}

void CallMgr::CloseLocalSecondVideoCapDev(MediaCall*pMediaCall)
{
	ScreenCapChanRTP*pScreenCapChanRTP=NULL;

	{
		XAutoLock l(m_csMapScreenCapChanRTPSecond);
		MapScreenCapChanRTP::iterator iter=m_MapScreenCapChanRTPSecond.begin();
		while (iter!=m_MapScreenCapChanRTPSecond.end())
		{
			pScreenCapChanRTP=iter->second;
			if (pScreenCapChanRTP->RemoveMediaCall(pMediaCall))
			{
				if (pScreenCapChanRTP->GetMediaCallCount()>0)
				{
					pScreenCapChanRTP=NULL;
				}
				else
				{
					m_MapScreenCapChanRTPSecond.erase(iter);
				}

				break;
			}
			else
			{
				pScreenCapChanRTP=NULL;
			}
			++iter;
		}
	}

	if (pScreenCapChanRTP)
	{
		pScreenCapChanRTP->Close();
		delete pScreenCapChanRTP;
		pScreenCapChanRTP=NULL;
	}
}

/******************************************************************************
* SetExternalIP
*描述：设置外网映射IP
*输入：cszExternalIP		-外网映射IP
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetExternalIP(const char*cszExternalIP)
{
	if (cszExternalIP)
	{
		m_strExternalIP=cszExternalIP;
	}
	else
	{
		m_strExternalIP="";
	}
	if (m_strExternalIP.size()>0 && m_strExternalIP!="127.0.0.1" && m_strExternalIP!="0.0.0.0")
	{
		m_nExternalIPAddress = htonl(XSocket::atoh(m_strExternalIP.c_str()));
	}
	else
	{
		m_nExternalIPAddress=0;
	}
}

const char*CallMgr::GetExternalIP(void)
{
	return m_strExternalIP.c_str();
}

unsigned long CallMgr::GetExternalIPAddress(void)
{
	return m_nExternalIPAddress;
}

/******************************************************************************
* OnDAP_RecvCallH245ControlMessage
*描述：接收到H245控制消息回调 包含flowControlIndication H239令牌相关 会控消息
*输入：	cszCallID			-呼叫标识
nMsgType			-控制消息类型
cszPacket			-消息包
nPacketLen			-消息包长度
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_RecvCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_RecvCallH245ControlMessage(cszCallID,nMsgType,cszPacket,nPacketLen);
		}
		++iter;
	}
}


/******************************************************************************
* RequestMainVideoKeyFrame
*描述：请求主流I帧
*输入：cszCallID		-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::RequestMainVideoKeyFrame(const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().RequestMainVideoKeyFrame(cszCallID);
			return ;
		}
#endif
#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().RequestMainVideoKeyFrame(cszCallID);
			return ;
		}
#endif
	}
}

/******************************************************************************
* RequestSecondVideoKeyFrame
*描述：请求辅流I帧
*输入：cszCallID		-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::RequestSecondVideoKeyFrame(const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().RequestSecondVideoKeyFrame(cszCallID);
			return ;
		}
#endif
#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().RequestSecondVideoKeyFrame(cszCallID);
			return ;
		}
#endif
	}
}

/******************************************************************************
	* HasActiveCall
	*描述：是否有呼叫
	*输入：无
	*输出：无
	*返回值：BOOL
*****************************************************************************/
bool CallMgr::HasActiveCall()
{
	bool bHasH323Call = false, bHasSipCall = false;

#ifdef HAS_H323
	bHasH323Call = H323CallMgr::Instance().HasCall();
#endif

#ifdef HAS_SIP
	bHasSipCall = SIPCallMgr::Instance().HasCall();
#endif

	return (bHasH323Call || bHasSipCall);
}

/******************************************************************************
* SetLocalMainVideoSource
*描述：设置本地主视频源
*输入：cszCallID		  -呼叫标识
nMainVideoDevID    -主流设备ID
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetLocalMainVideoSource(const char*cszCallID, int nMainVideoDevID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetLocalMainVideoSource(cszCallID, nMainVideoDevID);
			return ;
		}
#endif
#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetLocalMainVideoSource(cszCallID, nMainVideoDevID);
			return ;
		}
#endif
	}
}
/******************************************************************************
* SetLocalSecondVideoSource
*描述：设置本地辅视频源
*输入：cszCallID		  -呼叫标识
nSecondVideoDevID  -辅流设备ID
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetLocalSecondVideoSource(const char*cszCallID, int nSecondVideoDevID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return ;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			H323CallMgr::Instance().SetLocalSecondVideoSource(cszCallID, nSecondVideoDevID);
			return ;
		}
#endif
#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			SIPCallMgr::Instance().SetLocalSecondVideoSource(cszCallID, nSecondVideoDevID);
			return ;
		}
#endif
	}
}

/******************************************************************************
*描述:	向对端发送自定义非标消息
*输入:	cszCallID		-呼叫标识
cszContent		-自定义非标准信息内容
nContentLen		-自定义非标准信息长度
cszMimeType		-自定义非标信息内容类型 如为XML 值设为application/xml,如为一般字符串则设为NULL
*****************************************************************************/
int CallMgr::SendCallCustomMsg(const char*cszCallID,const char * cszContent,int nContentLen,const char * cszMimeType)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().DAPSendCallH245NonstandardData(cszCallID,cszContent);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			return SIPCallMgr::Instance().DAPSendCallCustomMsg(cszCallID,cszContent,cszMimeType);
		}
#endif

	}

	return -1;
}

/******************************************************************************
* OnDAP_CallRecvNonstandardData
*描述：接收到有终端发来H245非标消息时系统会调用此事件
*输入：	cszCallID		-呼叫标识
		cszPacket		-非标准信息负载
		nPacketLen		-非标准信息长度
		cszMimeType		-目前仅SIP使用 信息内容类型 如application/xml
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_RecvNonstandardData(const char*cszCallID,const char * cszPacket,int nPacketLen,const char * cszMimeType)
{
	//LogFile(NULL,NULL,"",cszCallID,"CallMgr::OnDAP_RecvNonstandardData. 111");
	//XAutoLock l(m_csMapIMXEC_Call);
	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_RecvNonstandardData(cszCallID,cszPacket,nPacketLen,cszMimeType);
		}
		++iter;
	}
	//LogFile(NULL,NULL,"",cszCallID,"CallMgr::OnDAP_RecvNonstandardData. 222");
}

bool CallMgr::IsCanCall()
{
#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

	return true;

#elif ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

	return true;

#else 

	//XAutoLock l(m_csMapCall);
	if(m_nCallNum ==0) return true;
	else return false; 

#endif

}

void CallMgr::SetCallNumZero()
{
	m_nCallNum =0;
}

void CallMgr::SetCanCallNot()
{
	m_nCallNum =1;
}


int CallMgr::GetCallH323ID(const char*cszCallID ,char * szH323ID,int nMaxLen)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return 0;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetCallH323ID(cszCallID ,szH323ID,nMaxLen);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			//return SIPCallMgr::Instance().GetCallH323ID(cszCallID ,szH323ID,nMaxLen);
		}
#endif

		if (iter->second=="IMX")
		{
			//return IMXCallMgr::Instance().GetCallH323ID(cszCallID ,szH323ID,nMaxLen);
		}
	}

	return 0;
}

int CallMgr::GetCallE164ID(const char*cszCallID ,char * szE164ID,int nMaxLen)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return 0;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
#ifdef HAS_H323
		if (iter->second=="H323")
		{
			return H323CallMgr::Instance().GetCallE164ID(cszCallID ,szE164ID,nMaxLen);
		}
#endif

#ifdef HAS_SIP
		if (iter->second=="SIP")
		{
			//return SIPCallMgr::Instance().GetCallE164ID(cszCallID ,szE164ID,nMaxLen);
		}
#endif

		if (iter->second=="IMX")
		{
			//return IMXCallMgr::Instance().GetCallE164ID(cszCallID ,szE164ID,nMaxLen);
		}
	}

	return 0;
}

/******************************************************************************
* OnDAP_RecvRemoteAudioRTPPacket
*描述：接收到音频媒体流回调
*输入：	cszCallID			-呼叫标识
pRTPPacketData		-RTP包
nRTPPacketLen		-RTP包长度
nPayloadType		-PayloadType
nRTPHeaderLen		-RTP包头长度
usSequence			-RTP包次序
ulTimestamp			-RTP包时间戳
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_RecvRemoteAudioRTPPacket(const char*cszCallID,X_AUDIO_CODEC_ID codecID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_RecvRemoteAudioRTPPacket(cszCallID,codecID,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_RecvRemoteMainVideoRTPPacket
*描述：接收到主流视频媒体流回调
*输入：	cszCallID			-呼叫标识
pRTPPacketData		-RTP包
nRTPPacketLen		-RTP包长度
nPayloadType		-PayloadType
nRTPHeaderLen		-RTP包头长度
usSequence			-RTP包次序
ulTimestamp			-RTP包时间戳
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_RecvRemoteMainVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_RecvRemoteMainVideoRTPPacket(cszCallID,codecType,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_RecvRemoteSecondVideoRTPPacket
*描述：接收到辅流视频媒体流回调
*输入：	cszCallID			-呼叫标识
pRTPPacketData		-RTP包
nRTPPacketLen		-RTP包长度
nPayloadType		-PayloadType
nRTPHeaderLen		-RTP包头长度
usSequence			-RTP包次序
ulTimestamp			-RTP包时间戳
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_RecvRemoteSecondVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_RecvRemoteSecondVideoRTPPacket(cszCallID,codecType,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
		}
		++iter;
	}
}

/******************************************************************************
* OnPeerEntityInfo
*描述：返回对端实体信息
*输入：	pCallParam			-CallParamSet 信息体
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnPeerEntityInfo(const CallParamSet * pCallParam)
{
    MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
    while (iter!=m_MapIMXEC_Call.end())
    {
        if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
        {
            iter->second->OnPeerEntityInfo(pCallParam);
        }
        ++iter;
    }
}

/******************************************************************************
* SetNetecTransEnable
*描述：是否使用NETEC私有传流
*输入：	cszCallID			-呼叫标识
        nEnable             -是否使用,1为使用,0为不使用
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::SetNetecTransEnable(const char*cszCallID,int nEnable)
{
    m_nUseNETECSend = nEnable;
}


void CallMgr::SaveMediaReleasePort(MediaPortRelease & MediaPortRelease,unsigned short rtpPort)
{
	if(rtpPort > 0)
	{
		int nCount=MediaPortRelease.m_nCount;
		if(MAX_PORTRLEASE_NUM == nCount) return ;
		nCount ++;
		MediaPortRelease.m_Array[nCount -1].m_nRtpPort=rtpPort;
		MediaPortRelease.m_nCount = nCount;

		//char msg[128]={0};
		//sprintf(msg,"-----------------------------------Save Port %d count %d\n",rtpPort,nCount);
		//OutputDebugString(msg);
	}
}

unsigned short CallMgr::GetMediaReleasePort(MediaPortRelease & MediaPortRelease)
{
	unsigned short rtpPort=0;
	int nCount=MediaPortRelease.m_nCount;
	int num=nCount;
	if(nCount >0) nCount--;
	MediaPortRelease.m_nCount = nCount;

	//rtpPort=MediaPortRelease.m_Array[nCount].m_nRtpPort; //the not check port eanble
	while(num >0)
	{
		num --;
		rtpPort=MediaPortRelease.m_Array[num].m_nRtpPort;
		if(CheckUDPPortEnable(rtpPort) )
		{
			break;
		}
	}

	//char msg[128]={0};
	//sprintf(msg,"=======================================Get Port %d count %d\n",rtpPort,nCount);
	//OutputDebugString(msg);

	return rtpPort;
}


/******************************************************************************
* OnDAP_CallAudioRecvChannelConnected
*描述：音频接收通道连接成功事件
*输入：	cszCallID			-呼叫标识
AudioCodecID		-音频CodecID
nPayloadType		-PayloadType
ip					-本端接收rtp ip
port				-本端接收rtp port
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallAudioRecvChannelConnected(const char*cszCallID,X_AUDIO_CODEC_ID AudioCodecID
												  ,int nPayloadType,unsigned long ip, int port)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallAudioRecvChannelConnected(cszCallID,AudioCodecID,nPayloadType,ip, port);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallAudioRecvChannelConnected
*描述：音频接收通道关闭事件
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallAudioRecvChannelDestroyed(const char*cszCallID)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallAudioRecvChannelDestroyed(cszCallID);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallMainVideoRecvChannelConnected
*描述：主流视频接收通道连接成功事件
*输入：	cszCallID			-呼叫标识
codecType			-视频CodecID
nBitrate			-码率
nPayloadType		-PayloadType
width				-分辨率宽度像素数 可选
nHeight				-分辨率高度像素数 可选
ip					-本端接收rtp ip
port				-本端接收rtp port
nLevel				-分辨率实际H264level值 可选
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallMainVideoRecvChannelConnected(const char*cszCallID ,VIDEC_CODEC_TYPE codecType
													  ,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth,int nHeight,*/int nLevel)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallMainVideoRecvChannelConnected(cszCallID,codecType,nBitrate,nPayloadType,ip, port,VideoFormatType/*nWidth,nHeight*/,nLevel);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallMainVideoRecvChannelDestroyed
*描述：主流视频接收通道关闭事件
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallMainVideoRecvChannelDestroyed(const char*cszCallID)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallMainVideoRecvChannelDestroyed(cszCallID);
		}
		++iter;
	}
}
/******************************************************************************
* OnDAP_CallSecondVideoRecvChannelConnected
*描述：辅流视频接收通道连接成功事件
*输入：	cszCallID			-呼叫标识
codecType			-视频CodecID
nBitrate			-码率
nPayloadType		-PayloadType
width				-分辨率宽度像素数 可选
nHeight				-分辨率高度像素数 可选
ip					-本端接收rtp ip
port				-本端接收rtp port
nLevel				-分辨率实际H264level值 可选
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallSecondVideoRecvChannelConnected(const char*cszCallID ,VIDEC_CODEC_TYPE codecType
														,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth,int nHeight,*/int nLevel)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallSecondVideoRecvChannelConnected(cszCallID,codecType,nBitrate,nPayloadType,ip, port,VideoFormatType,/*nWidth,nHeight,*/nLevel);
		}
		++iter;
	}
}
/******************************************************************************
* OnDAP_CallSecondVideoRecvChannelDestroyed
*描述：辅流视频接收通道关闭事件
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallSecondVideoRecvChannelDestroyed(const char*cszCallID)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallSecondVideoRecvChannelDestroyed(cszCallID);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallFECCRecvChannelConnected
*描述：FECC控制接收通道连接成功事件
*输入：	cszCallID			-呼叫标识
nPayloadType		-PayloadType
ip					-本端接收rtp ip
port				-本端接收rtp port
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallFECCRecvChannelConnected(const char*cszCallID ,int nPayloadType,unsigned long ip
												 , int port)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallFECCRecvChannelConnected(cszCallID,nPayloadType,ip, port);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallFECCRecvChannelDestroyed
*描述：FECC控制接收通道关闭事件
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallFECCRecvChannelDestroyed(const char*cszCallID)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallFECCRecvChannelDestroyed(cszCallID);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallAudioSendChannelConnected
*描述：音频发送通道连接成功事件
*输入：	HDAPAPPCALL hdapCall 实体call句柄
cszCallID			-呼叫标识
AudioCodecID		-音频CodecID
nPayloadType		-PayloadType
ip					-对端rtp ip
port				-对端rtp port
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallAudioSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,X_AUDIO_CODEC_ID AudioCodecID
												  ,int nPayloadType,unsigned long ip, int port)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallAudioSendChannelConnected(hdapCall,cszCallID,AudioCodecID,nPayloadType,ip, port);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallAudioSendChannelDestroyed
*描述：音频发送通道关闭事件
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallAudioSendChannelDestroyed(const char*cszCallID)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallAudioSendChannelDestroyed(cszCallID);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallMainVideoSendChannelConnected
*描述：主流视频发送通道连接成功事件
*输入：	HDAPAPPCALL hdapCall 实体call句柄
cszCallID			-呼叫标识
codecType			-视频CodecID
nBitrate			-码率
nPayloadType		-PayloadType
width				-分辨率宽度像素数 可选
nHeight				-分辨率高度像素数 可选
nLevel				-分辨率实际H264level值 可选
ip					-对端rtp ip
port				-对端rtp port
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallMainVideoSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,VIDEC_CODEC_TYPE codecType
													  ,int nBitrate,int nPayloadType,unsigned long ip,int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth,int nHeight,*/int nLevel)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallMainVideoSendChannelConnected( hdapCall,cszCallID,codecType,nBitrate,nPayloadType,ip, port,VideoFormatType/*nWidth,nHeight*/,nLevel);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallMainVideoSendChannelDestroyed
*描述：主流视频发送通道连接成功事件
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallMainVideoSendChannelDestroyed(const char*cszCallID)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallMainVideoSendChannelDestroyed( cszCallID);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallSecondVideoSendChannelConnected
*描述：辅流视频发送通道连接成功事件
*输入：	HDAPAPPCALL hdapCall 实体call句柄
cszCallID			-呼叫标识
codecType			-视频CodecID
nBitrate			-码率
nPayloadType		-PayloadType
width				-分辨率宽度像素数 可选
nHeight				-分辨率高度像素数 可选
nLevel				-分辨率实际H264level值 可选
ip					-对端rtp ip
port				-对端rtp port
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallSecondVideoSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,VIDEC_CODEC_TYPE codecType
														,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth,int nHeight,*/int nLevel )
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallSecondVideoSendChannelConnected( hdapCall,cszCallID,codecType,nBitrate,nPayloadType,ip, port,VideoFormatType/*nWidth,nHeight*/,nLevel);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallSecondVideoSendChannelDestroyed
*描述：辅流视频发送通道连接成功事件
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallSecondVideoSendChannelDestroyed(const char*cszCallID)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallSecondVideoSendChannelDestroyed( cszCallID);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallFECCSendChannelConnected
*描述：FECC控制发送通道连接成功事件
*输入：	HDAPAPPCALL hdapCall 实体call句柄
cszCallID			-呼叫标识
ip					-对端rtp ip
port				-对端rtp port
nPayloadType		-PayloadType
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallFECCSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID,int nPayloadType
												 ,unsigned long ip, int port)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallFECCSendChannelConnected( hdapCall,cszCallID,nPayloadType,ip, port);
		}
		++iter;
	}
}

/******************************************************************************
* OnDAP_CallFECCSendChannelDestroyed
*描述：FECC控制发送通道关闭事件
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CallMgr::OnDAP_CallFECCSendChannelDestroyed(const char*cszCallID)
{
	//XAutoLock l(m_csMapIMXEC_Call);

	MapIMXEC_Call::iterator iter=m_MapIMXEC_Call.begin();
	while (iter!=m_MapIMXEC_Call.end())
	{
		if (m_pIMXEC_CallExclusive==NULL || m_pIMXEC_CallExclusive==iter->second)
		{
			iter->second->OnDAP_CallFECCSendChannelDestroyed( cszCallID);
		}
		++iter;
	}
}

void CallMgr::CheckMapCallEmpty(void)
{
	bool bRetH323=false;
	bool bRetSIP=false;
#ifdef HAS_H323
	bRetH323 = H323CallMgr::Instance().CheckMapCallEmpty();
#endif

#ifdef HAS_SIP
	bRetSIP  = SIPCallMgr::Instance().CheckMapCallEmpty();
#endif
	if(bRetH323 && bRetSIP ) SetCallNumZero();
}

bool CallMgr::GetOneSaveLocalIP(std::string & strLocalIP)
{
	bool bRet=false;
	if(m_AllLocalIPSave.m_nNum >0)
	{
		strLocalIP = m_AllLocalIPSave.m_AllIP[0].strIP ;
		bRet=true;
	}
	return bRet;
}

bool CallMgr::GetSameToDestIP(std::string & strLocalIP,std::string & strDestIP)
{
	bool bRet=false;
	bRet=GetOneSameDestIP3(m_AllLocalIPSave,strLocalIP,strDestIP);
	return bRet;
}

#ifdef _TIANZHU_CACHE_
int CallMgr::SetRemoteVideoCache(const char*cszCallID, int nCacheFrameCount)
{
	if (cszCallID==NULL || strlen(cszCallID)==0)
	{
		return -1;
	}
	MapCallID::iterator iter=m_MapCallID.find(cszCallID);
	if (iter!=m_MapCallID.end())
	{
		if (iter->second=="IMX")
		{
			return IMXCallMgr::Instance().SetRemoteVideoCache(cszCallID, nCacheFrameCount);
		}
	}

	return -1;
}
#endif

void GetVideoFormatType(VIDEO_FORMAT_TYPE nFormatType,int & w,int & h)
{
	switch(nFormatType)
	{
	case VIDEO_FORMAT_QCIF:
		w=176,h=144;
		break;
	case VIDEO_FORMAT_CIF:
		w=352,h=288;
		break;
	case VIDEO_FORMAT_4CIF:
		w=704,h=576;
		break;
	case VIDEO_FORMAT_16CIF:
		w=1408,h=1152;
		break;
	case VIDEO_FORMAT_720I:
		w=1280,h=360;
		break;
	case VIDEO_FORMAT_720P:
		w=1280,h=720;
		break;
	case VIDEO_FORMAT_1080I:
		w=1920,h=540;
		break;
	case VIDEO_FORMAT_1080P:
		w=1920,h=1080;
		break;
	case VIDEO_FORMAT_QVGA:
		w=320,h=240;
		break;
	case VIDEO_FORMAT_VGA:
		w=640,h=480;
		break;
	case VIDEO_FORMAT_SVGA:
		w=800,h=600;
		break;
	case VIDEO_FORMAT_XGA:
		w=1024,h=768;
		break;
	case VIDEO_FORMAT_SXGA:
		w=1280,h=1024;
		break;
	default:
		w=0,h=0;
		break;
	}
}

//A7 add
VIDEO_FORMAT_TYPE GetVideoFormatType(int nVideoWidth,int nVideoHeight)
{
	VIDEO_FORMAT_TYPE videoFormatType =VIDEO_FORMAT_4CIF;
	switch (nVideoWidth)
	{
	case 176:
		if(nVideoHeight==144) videoFormatType=VIDEO_FORMAT_QCIF;
		break;
	case 352:
		if(nVideoHeight==288) videoFormatType=VIDEO_FORMAT_CIF;
		break;
	case 704:
		if(nVideoHeight==576) videoFormatType=VIDEO_FORMAT_4CIF;
		break;
	case 1408:
		if(nVideoHeight==1152) videoFormatType=VIDEO_FORMAT_16CIF;
		break;
	case 1280:
		if(nVideoHeight==360) videoFormatType=VIDEO_FORMAT_720I;
		if(nVideoHeight==720) videoFormatType=VIDEO_FORMAT_720P;
		if(nVideoHeight==1024) videoFormatType=VIDEO_FORMAT_SXGA;
		break;
	case 1920:
		if(nVideoHeight==540) videoFormatType=VIDEO_FORMAT_1080I;
		if(nVideoHeight==1080) videoFormatType=VIDEO_FORMAT_1080P;
		break;
	case 640:
		if(nVideoHeight==480) videoFormatType=VIDEO_FORMAT_VGA;
		break;
	case 800:
		if(nVideoHeight==600) videoFormatType=VIDEO_FORMAT_SVGA;
		break;
	case 1024:
		if(nVideoHeight==768) videoFormatType=VIDEO_FORMAT_XGA;
		break;
	default:
		videoFormatType=VIDEO_FORMAT_4CIF;
		break;
	}
	return videoFormatType;
}

//查看是否为合法IP,排除掉全数字或全为首端带有0x及0X的十六进制的数字
bool IsCallIPString(const char * str)
{
	bool bRet=false;
	bool bLicit=false;
	int len=0;
	if(str!=NULL)
	{
		len=strlen(str);
		//查看是否为合法IP
		if(inet_addr(str) != INADDR_NONE)
			bLicit=true;
	}
	//查看是否有.或: 排除掉全数字或全为首端带有0x及0X的十六进制的数字
	if(bLicit)
	{
		int i=0;
		while(i<len)
		{
			if( str[i] == '.' ||  str[i] == ':')
			{
				bRet=true;
				break;
			}
			i++;
		}
	}
	return bRet;
}

//return -1 is not,return 0 is only IP,return >0 is port
int IsCallIPPortString(const char * str)
{
	int nRet=-1;
	if(IsCallIPString(str) )
	{
		nRet =0;
	}
	else
	{
		std::string strTemp=str;
		std::string strPort="";
		int nPos=strTemp.find(":");
		if(nPos>0)
		{
			strPort=strTemp.substr(nPos+1,strTemp.size()-nPos-1);
			nRet =atoi(strPort.c_str());
		}
	}
	return nRet;
}