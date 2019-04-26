#include "stdafx.h"
#include "SIPCallMgr.h"
#include "CallMgr.h"
#include <re/re.h>
#include <re/bfcp.h>
#include "PortMonitoring.h"
#include "../IMXEC_Util/XUtil.h"

//notice test
#ifdef HAS_H323

#include <RVINC/cm.h>
#endif

//notice test
extern bool IsCallIPString(const char * str);
extern int IsCallIPPortString(const char * str);
extern void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info);

extern unsigned short GetRTPPort(unsigned short usRTPBasePort);
static SIPCallMgr g_SIPCallMgr;

#define KA_INTERVAL 300

void DebugOutput(int nType,char *szFormat, ...)
{
#if defined(SOCKLOGOUTPUT)

	va_list args;
	va_start(args, szFormat);
	char buffer[2048]={0};

#ifdef _WIN32

	char szTime[128]={0};
	time_t now = time(NULL);
	_tcsftime(szTime, 128, _T("%m-%d %H:%M:%S"), localtime(&now));
	strcpy(buffer,szTime);
	int nLen =strlen(buffer);
	_vsnprintf(buffer +nLen,sizeof(buffer) -nLen,szFormat,args);
	OutputDebugString(buffer);

#else
	vsnprintf(buffer,sizeof(buffer),szFormat,args);
	printf(buffer);
#endif

	va_end(args);

#endif 

}

int __stdcall OnMessageReceived(char* from, char * text)
{
	char msg[200]={0};
	strcpy(msg,"<<");
	strcpy(msg+2,text);
	g_SIPCallMgr.StackDisplayMsg(msg);
	return 1;
}

int __stdcall OnDtmfDigit(int call_id, int digit)
{
	return 1;
}

VOID CALLBACK SIPCallMgrTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	g_SIPCallMgr.OnTimer(idEvent);
}

void SIPCallMgr::StackDisplayMsg(char * msg,int type)
{

}

static void OnPJSIPICENegComplete(pjmedia_transport *tp,pj_ice_strans_op op,pj_status_t status)
{
	if(tp)
	{
		sip_register_thread();
		PJ_LOG(3,("ICENegComplete","OnPJSIPICENegComplete,op:%d,status:%d",op,status));
		SIPCallMgr * pSIPCallMgr=(SIPCallMgr * )tp->user_data;
		if(pSIPCallMgr)
		{
			pSIPCallMgr->OnICENegotiationComplete(tp,op,status);
		}
	}
}

static void stun_nat_detect_cb(void *user_data,const pj_stun_nat_detect_result *res)
{
	sip_register_thread();
	SIPCallMgr * pSipCallMgr=(SIPCallMgr *)user_data;
	if(res)
	{
		char * pName=(char *)pj_stun_get_nat_name(res->nat_type);
		PJ_LOG(3,("Detect","stun nat detect,status:%d,status type name:%s,status text:%s,nat name:%s",res->status,(char *)res->nat_type_name,(char *)res->status_text,pName));
	}
}

SIPCallMgr&SIPCallMgr::Instance(void)
{
	return g_SIPCallMgr;
}

SIPCallMgr::SIPCallMgr(void)
:m_bRunning(false)
,m_strEmpty("")
,m_nCurrentLineID(0)
,m_nRTPBasePort(22000)
,m_nSIPListenPort(5060)
,m_nActualSIPListenPort(0)
,m_nTimerID(0)
,m_nMainVideoDevID(0)
,m_nMainVideoFrameRate(15)
,m_nMainVideoBitrate(320)
,m_nMainVideoCodecType(VIDEC_CODEC_H264)
,m_nMainVideoFormatType(VIDEO_FORMAT_CIF)
,m_nMainVideoNetworkType(VIDEC_NETWORK_PRIVATE)
,m_nAudioCodecID(X_AUDIO_CODEC_G711U)
{
	m_H264PT=106, m_H263PPT=107, m_H263PPPT=108, m_H224PT=100;
	m_H264SVCPT=104;
	m_DualH264SVCPT=105;
	m_DualH264PT=106;
	m_DualH263PPT=107;
	m_DualH263PPPT=108;
	m_G7221_24k_PT=110;
	m_G7221_32k_PT=111;
	m_G7221C_24k_PT=112;
	m_G7221C_32k_PT=113;
	m_G7221C_48k_PT=114;
	m_nSecondVideoMaxBitrate=384;
	m_nSecondVideoDevID=0;
	m_nSecondVideoDevFrameRate=15;
	m_nSecondVideoCodecType=VIDEC_CODEC_H264;
	m_nSecondVideoFormatType=VIDEO_FORMAT_CIF;
    m_nSecondVideoNetworkType=VIDEC_NETWORK_PRIVATE;
	m_nSupportH239=0;
	m_nStarted=0;
	m_nRequestConfId=1;
	m_nRequestUserId=2;
	m_nRequestFloorId=1;
	m_nRequestPriority=BFCP_PRIO_NORMAL;
	m_nRequestFloorControlType=e_Floor_Control_Client;
	m_nRequestSetupType=e_BFCP_Setup_Actpass;

	m_nDualVideoSDPMode =1; //notice test
	if(m_nDualVideoSDPMode ==2)
	{
		m_nRequestConfId=0;
		m_nRequestUserId=0;
		m_nRequestFloorId=0;
		m_nRequestFloorControlType=e_Floor_Control_All;
	}

	m_nRequestConfIdCalled=m_nRequestConfId +1;
	m_nRequestUserIdCalled=m_nRequestUserId +2;
	m_nRequestFloorIdCalled=m_nRequestFloorId +1;
	m_nRequestFloorControlTypeCalled=e_Floor_Control_Server;
	m_nRequestSetupTypeCalled=e_BFCP_Setup_Passive;

	m_nRequestTransportType=e_BFCP_Transport_UDP;
	//m_nRequestTransportType=e_BFCP_Transport_TCP;

	strcpy(m_strUserAgentName,"AVCON-SIP2.0");

//A7 add
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

	strcpy(m_strUserAgentName,"AVCONMCU-SIP2.0");
#endif

	m_nSIPTransType=IMXEC_SIP::TRANS_UDP;
	m_pIPCallSIPUser=NULL;
	m_nTurnTPType=PJ_TURN_TP_UDP;
	m_nICEInit=0;
	m_pICEPool=NULL;
	m_pICEThread=NULL;
	m_bICEThreadQuitFlag=PJ_FALSE;
	m_nICETransportCount=1;
	m_strStunSrv="";
	m_strTurnSrv="";
	m_strTurnUser="";
	m_strTurnPWD="";
	m_strDNSSrv="";
	m_strDNSSrv2="";
	m_pMediaEndpoint=NULL;
	m_nMaxICESocketCount=6*10;
	m_pDNSResolver=NULL;
	m_nOutboundProxyCount=0;
	m_nForcelrParam=0;
	m_nUseRegReqLine=0;
	m_nUseInvReqLine=0;
	m_strRegReqLineAddr="";
	m_strInvReqLineAddr="";
	m_strDateTime="";
	m_nUseDualVideoToken=1;
	m_nSDPOrignIPConverse=0;
	m_nSendSubscribre=0;
	m_nSendPicFastUpdate=1;
	for(int j=0;j<8;j++)
	{
		m_strOutboundProxy[j][0]='\0';
	}
	m_nSendMultipartSDP=0;

	m_nSIPRegFailTimerTimestamp=2*60*1000;
	m_nSIPREGFail=0;
	m_nUseDualVideoTokenType=0;
	m_strConfCentralNum[0]='\0';
	m_nCapPrint=0;
	m_nRegExpires=0;
	m_nSIPLogLevel=0;
	m_strSIPLogFile[0]='\0';
	m_nLastPacketCheckTimestamp=XGetTimestamp();
	m_nLastBFCPCheckTimestamp=XGetTimestamp();
	m_nSIPCheckLastTimestamp=XGetTimestamp();
	m_nProxySendRtp =1;
	m_nProxyUseFEC  =1;
	m_nRTPUseType   =1;
	m_nSIPServerType = 0;
	memset(&m_MediaPortRelease,0,sizeof(m_MediaPortRelease) );
	m_nLastCheckCallNumTS =XGetTimestamp();

}

SIPCallMgr::~SIPCallMgr(void)
{
}

/******************************************************************************
* AddVirtualLine
*描述：添加虚拟线路
*输入：	cszUserID		-用户帐号
cszUserName		-用户名称
cszPassword		-用户密码
cszHost			-SIP代理服务器
*输出：无
*返回值：成功返回>=0[line id]，失败返回-1
*****************************************************************************/
//注意 cszAuthUserID 不需要时要设为空
int SIPCallMgr::AddVirtualLine(const char*cszUserID,const char*cszUserName,const char*cszPassword,const char*cszSIPProxy,const char*cszAuthUserID,const char*cszDomain)
{
	if (m_nStarted ==0 ) return -1;
	m_nSIPREGFail=0;
	if (cszUserID==NULL || strlen(cszUserID)==0 ||
		cszSIPProxy==NULL || strlen(cszSIPProxy)==0)
	{
		return -1;
	}
	sip_register_thread();
	int nLineID=0;

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#else
	RemoveVirtualLine(nLineID);//此语句会限制为只注册一路账号
#endif

	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPUser);
#endif

		while (true)
		{
			MapSIPUser::iterator iter=m_MapSIPUser.find(nLineID);
			if (iter==m_MapSIPUser.end())
			{
				break;
			}
			else
			{
				nLineID++;
			}
		}
	}
	if(nLineID+1>PJSUA_MAX_ACC-1)
	{
		std::string strErrorCode="Maximum accounts error.";
		OnRegistrationError(nLineID,strErrorCode,cszUserID);
		return -1;
	}
	std::string strUserID=cszUserID;
	std::string strUserName="";
	std::string strPassword="";
	std::string strSIPProxy=cszSIPProxy;
	std::string strAuthUserID="";
	std::string strDomain="";
	if (cszPassword!=NULL)
	{
		strPassword=cszPassword;
	}
	if (cszUserName!=NULL)
	{
		strUserName=cszUserName;
	}
	if (cszAuthUserID!=NULL && strlen(cszAuthUserID)>0)
	{
		strAuthUserID=cszAuthUserID;
	}
	if (cszDomain!=NULL && strlen(cszDomain)>0)
	{
		strDomain=cszDomain;
	}
	int nPos=strDomain.find(":");
	if (nPos>0)
	{
		strDomain=strDomain.substr(0,nPos);
	}

	SIPUser*pSIPUser=new SIPUser(*this,nLineID,strUserID,strUserName,strPassword,strSIPProxy,strAuthUserID,strDomain);
	if(pSIPUser ==NULL) return -1;

	if (0!=pSIPUser->Open())
	{
		WaitSIPUserDelete(pSIPUser);
		pSIPUser=NULL;
		return -1;
	}
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPUser);
#endif

	m_MapSIPUser[nLineID]=pSIPUser;
	}
	{
		XAutoLock l(m_csMapThrSIPUser);
		m_MapThrSIPUser[nLineID]=pSIPUser;
	}
	pSIPUser->m_nRegSetting =2;
	return nLineID;
}

/******************************************************************************
* RemoveVirtualLine
*描述：删除虚拟线路
*输入：	nLineID			-线路标识
*输出：无
*返回值：无
*****************************************************************************/
void SIPCallMgr::RemoveVirtualLine(int nLineID)
{
	SIPUser*pSIPUser=NULL;
	bool bFind=false;
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPUser);
#endif
		MapSIPUser::iterator iter=m_MapSIPUser.find(nLineID);
		if (iter!=m_MapSIPUser.end())
		{
			pSIPUser=iter->second;
			m_MapSIPUser.erase(iter);
			bFind=true;
		}
		//}
		if(pSIPUser)
		{
			pSIPUser->m_nRegSetting =1;
			pSIPUser->Close();
			//WaitSIPUserUnRegComplete();
			WaitSIPUserDelete(pSIPUser);
			pSIPUser=NULL;
		}
	}
	if(bFind)
	{
		XAutoLock l(m_csMapThrSIPUser);
		MapSIPUser::iterator iter=m_MapThrSIPUser.find(nLineID);
		if (iter!=m_MapThrSIPUser.end())
		{
			m_MapThrSIPUser.erase(iter);
		}
	}
}

int SIPCallMgr::SetCurrentVirtualLine(int nLineID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPUser);
#endif
	MapSIPUser::iterator iter=m_MapSIPUser.find(nLineID);
	if (iter!=m_MapSIPUser.end())
	{
		m_nCurrentLineID=nLineID;
		return 0;
	}
	return -1;
}

int SIPCallMgr::GetCurrentVirtualLine(void)
{
	return m_nCurrentLineID;
}

bool SIPCallMgr::HasVirtualLineRegistrationSuccess(void)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapThrSIPUser);
#endif
	MapSIPUser::iterator iter=m_MapThrSIPUser.begin();
	if (iter!=m_MapThrSIPUser.end())
	{
		SIPUser*pSIPUser=iter->second;
		if(pSIPUser->IsRegistrationSuccess())
		{
			return true;
		}
		iter++;
	}
	return false;
}

int SIPCallMgr::IsVirtualLineRegistrationSuccess(int nLineID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPUser);
#endif
	MapSIPUser::iterator iter=m_MapSIPUser.find(nLineID);
	if (iter!=m_MapSIPUser.end())
	{
		SIPUser*pSIPUser=iter->second;
		return pSIPUser->IsRegistrationSuccess()?1:0;
	}
	return 0;
}

int SIPCallMgr::IsCurrentVirtualLineRegistrationSuccess(void)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPUser);
#endif
	MapSIPUser::iterator iter=m_MapSIPUser.find(m_nCurrentLineID);
	if (iter!=m_MapSIPUser.end())
	{
		SIPUser*pSIPUser=iter->second;
		return pSIPUser->IsRegistrationSuccess()?1:0;
	}
	return 0;
}

int SIPCallMgr::IsVirtualLineRegistrationError(int nLineID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPUser);
#endif
	MapSIPUser::iterator iter=m_MapSIPUser.find(nLineID);
	if (iter!=m_MapSIPUser.end())
	{
		SIPUser*pSIPUser=iter->second;
		return pSIPUser->IsRegistrationError()?1:0;
	}
	return 1;
}

int SIPCallMgr::IsCurrentVirtualLineRegistrationError(void)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPUser);
#endif
	MapSIPUser::iterator iter=m_MapSIPUser.find(m_nCurrentLineID);
	if (iter!=m_MapSIPUser.end())
	{
		SIPUser*pSIPUser=iter->second;
		return pSIPUser->IsRegistrationError()?1:0;
	}
	return 0;
}

const char*SIPCallMgr::GetVirtualLineRegistrationErrorCode(int nLineID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPUser);
#endif
	MapSIPUser::iterator iter=m_MapSIPUser.find(nLineID);
	if (iter!=m_MapSIPUser.end())
	{
		SIPUser*pSIPUser=iter->second;
		return pSIPUser->GetRegistrationErrorCode().c_str();
	}
	return "";
}

const char*SIPCallMgr::GetCurrentVirtualLineRegistrationErrorCode(void)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPUser);
#endif
	MapSIPUser::iterator iter=m_MapSIPUser.find(m_nCurrentLineID);
	if (iter!=m_MapSIPUser.end())
	{
		SIPUser*pSIPUser=iter->second;
		return pSIPUser->GetRegistrationErrorCode().c_str();
	}
	return "";
}

/******************************************************************************
* SetSIPListenPort
*描述：设置侦听端口
*输入：	nSIPListenPort			-侦听端口
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetSIPListenPort(unsigned short nSIPListenPort)
{
	m_nSIPListenPort=nSIPListenPort;
	if(m_nSIPListenPort <= 0) m_nSIPListenPort=5060;
	return 0;
}

unsigned short SIPCallMgr::GetSIPListenPort(void)
{
	return m_nSIPListenPort;
}

/******************************************************************************
* SetLocalIP
*描述：设置注册帐号
*输入：	cszSIPID		-注册帐号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetLocalIP(const char*cszLocalIP)
{
	m_strLocalIP=cszLocalIP;
	return 0;
}

std::string SIPCallMgr::GetLocalIP()
{
	return m_strLocalIP;

}

/******************************************************************************
* SetRTPBasePort
*描述：设置侦听端口
*输入：	nRTPBasePort			-RTP开始端口
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetRTPBasePort(unsigned short nRTPBasePort)
{
	m_nRTPBasePort=nRTPBasePort;
	if (m_nRTPBasePort<2000)
	{
		m_nRTPBasePort=2000;
	}
	return 0;
}

unsigned short SIPCallMgr::GetRTPBasePort(void)
{
	return m_nRTPBasePort;
}

/******************************************************************************
* SetAudioDevParam
*描述：设置主流视频设备参数
*输入： nAudioCodecID		-音频编码
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID)
{
	m_nAudioCodecID=nAudioCodecID;
	return 0;
}

/******************************************************************************
* SetMainVideoDevParam
*描述：设置主流视频设备参数
*输入： nDevID				-设备标识
nFrameRate			-帧率
nBitrate			-码流
nVideoCodecType		-视频编码
nVideoFormatType	-视频格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
	m_nMainVideoDevID=nDevID;
	m_nMainVideoFrameRate=nFrameRate;
	m_nMainVideoBitrate=nBitrate;
	m_nMainVideoCodecType=nVideoCodecType;
	m_nMainVideoFormatType=nVideoFormatType;
    m_nMainVideoNetworkType=nNetworkType;

	char msg[128]={0};
	sprintf(msg,"SetMainVideoDevParam codecTye=%d,formatType=%d",m_nMainVideoCodecType,m_nMainVideoFormatType);
	LogFile(NULL,NULL,"","",msg);

	return 0;
}

/******************************************************************************
* SetSecondVideoDevParam
*描述：设置辅流视频设备参数
*输入： nDevID				-设备标识
nFrameRate			-帧率
nBitrate			-码流
nVideoCodecType		-视频编码
nVideoFormatType	-视频格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/

int SIPCallMgr::SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
	m_nSecondVideoMaxBitrate=nBitrate;
	m_nSecondVideoDevID=nDevID;
	m_nSecondVideoDevFrameRate=nFrameRate;
	m_nSecondVideoCodecType=nVideoCodecType;
	m_nSecondVideoFormatType=nVideoFormatType;
    m_nSecondVideoNetworkType=nNetworkType;

	char msg[128]={0};
	sprintf(msg,"SetSecondVideoDevParam codecTye=%d,formatType=%d",m_nSecondVideoCodecType,m_nSecondVideoFormatType);
	LogFile(NULL,NULL,"","",msg);

	return 0;
}

/******************************************************************************
* Start
*描述：启动SIP服务
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::Start(void)
{
	//注 UDP也可用XSocketUDP.Open检测 TCP则检测不到
	CPortMonitoring portMonitor(false);
	bool bCheckPort=false;
	if(m_nSIPTransType == IMXEC_SIP::TRANS_TCP)
		bCheckPort=portMonitor.CheckTCPPort(m_nSIPListenPort);
	if(m_nSIPTransType == IMXEC_SIP::TRANS_UDP)
		bCheckPort=portMonitor.CheckUDPPort(m_nSIPListenPort);
	if(bCheckPort) return -1;

	if (m_nStarted !=0) return 0;

////notice mcu not use wait stable
//#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
//
//#else

    //use medianetrtp
    if (FALSE == IsIPCInitd())
    {
        if (FALSE == IPCInit(0,CallMgr::Instance().m_nIPCPort))
        {
        }
    }

	if (0 !=  HpSocketStartup(FALSE))
	{
	}

//#endif

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

	m_nSIPTransType = IMXEC_SIP::TRANS_TCP;
	IPCRegistModule("HPAdapter.dll");

#else

	IPCRegistModule("IMXEC.dll");

	//add by sgb for SDP bigger than MTU then use TCP
	//or use control PJSIP_UDP_SIZE_THRESHOLD and PJSIP_DONT_SWITCH_TO_TCP
	if( m_nSIPTransType == IMXEC_SIP::TRANS_UDP && GetSupportH239() > 0 )
	{
		m_nSIPTransType = IMXEC_SIP::TRANS_TCP;
	}
	else if( m_nSIPTransType == IMXEC_SIP::TRANS_UDP && GetSupportH239() <= 0 )
	{
		m_nSIPTransType = IMXEC_SIP::TRANS_UDP;
	}
	//add end

	//m_nSIPTransType = IMXEC_SIP::TRANS_TCP;

#endif

	pj_status_t status;
	//初始化SIP协议栈
	SipSetting();

	m_bRunning=true;
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	if (!StartThread())
	{
		OutPutStackLog(1,"SIPStack Start MSG Thread Failure!");
		return -1;
	}

#else
	m_nTimerID=::SetTimer(NULL,0,1,SIPCallMgrTimerProc);
#endif

	//m_nTimerID=::SetTimer(NULL,0,1,SIPCallMgrTimerProc);
	setSipConfig(&m_sipConfig);
	status = SipInit();
	sip_register_thread();

	if(status != PJ_SUCCESS)
	{
		PJ_LOG(4,("SipInit","pjsip init error=%d",status));
		OutPutStackLog(1,"SIPStack Initialize Failure!" );
		return -1;
	}

	SIP_BFCP::SIP_BFCPStart();
	//Add use to ip call
	m_pIPCallSIPUser =new SIPUser(*this);
	if(m_pIPCallSIPUser==NULL) return -1;
	CreateMediaEndpoint();
	ICEInstanceInit();
	SetNatDetectCallback();
	int nMaxCount=3*m_nICETransportCount;//初始创建最多ICE通道数
	CreateICEMediaTrans(nMaxCount);
	OutPutStackLog(0,"SIPStack Initialize Success!" );
	m_nStarted=1;

	return 0;
}

/******************************************************************************
* Stop
*描述：停止SIP服务
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::Stop(void)
{
	if (m_nStarted==0) return 0;
	sip_register_thread();
	m_nStarted=0;

	HungupAll();
	m_bRunning=false;

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

	WaitForStop();

#else
	if (m_nTimerID!=0)
	{
		KillTimer(NULL,m_nTimerID);
		m_nTimerID=0;
	}
#endif

	//close SIPUser notice prevent lock to lock add
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPUser);
#endif
		int nLineID=0;
		while(true)
		{
			SIPUser *pSIPUser=NULL;
			MapSIPUser::iterator iter=m_MapSIPUser.find(nLineID);
			if(iter!=m_MapSIPUser.end())
			{
				pSIPUser=iter->second;
				nLineID++;
			}
			else break;
			if(pSIPUser)
			{
				pSIPUser->Close();
			}
		}
	}

	if(m_pIPCallSIPUser)
	{
		delete m_pIPCallSIPUser;
		m_pIPCallSIPUser =NULL;
	}
	ICEInstanceExit();
	DestroyAllICEMediaTrans();
	CloseMediaEndpoint();
	SipDestroy();
	m_nActualSIPListenPort=0;
	SIP_BFCP::SIP_BFCPStop();

////notice mcu not use wait stable
//#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
//
//#else

	HpSocketCleanup(FALSE);//use medianetrtp add
	IPCQuit(FALSE);

//#endif

	OutPutStackLog(0,"SIPStack Stop Success." );
	return 0;
}

bool SIPCallMgr::GetCallSDP(const std::string&strCallID,std::string&strLocalIP,unsigned short&nLocalAudioPort,unsigned short&nLocalVideoPort)
{
	XAutoLock l(m_csMapThrSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapThrSIPMediaCall.find(strCallID);
	if (iter!=m_MapThrSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		strLocalIP=pSIPMediaCall->GetLocalIP();
		nLocalAudioPort=pSIPMediaCall->GetLocalAudioPort();
		nLocalVideoPort=pSIPMediaCall->GetLocalVideoPort();
		return true;
	}
	return false;
}

void SIPCallMgr::OnMediaUpdate(const std::string&strCallID,const std::string&strRemoteIP,unsigned short nRemoteAudioPort,int nAudioPayloadType,const std::string&strAudioPayloadName,int nAudioSampleRate,X_AUDIO_CODEC_ID AudioCodecID,unsigned short nRemoteVideoPort,int nVideoPayloadType,const std::string&strVideoPayloadName,int nVideoBitrate,int nVideoWidth,int nVideoHeight)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPMediaCall);
#endif
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->OnMediaUpdate(strRemoteIP,nRemoteAudioPort,nAudioPayloadType,strAudioPayloadName,nAudioSampleRate,AudioCodecID,nRemoteVideoPort,nVideoPayloadType,strVideoPayloadName,nVideoBitrate,nVideoWidth,nVideoHeight);
	}
}

SIPUser * SIPCallMgr::GetSIPUserFromAccountID(int accountID) 
{
	//find sipuser from accountid 
	SIPUser *pReturnSIPUser=NULL;
	{
		XAutoLock l(m_csMapThrSIPUser);
		MapSIPUser::iterator iter=m_MapThrSIPUser.begin();
		while (iter!=m_MapThrSIPUser.end())
		{
			SIPUser * pSIPUser=iter->second;
			++iter;
			if(pSIPUser->m_nAccountId==accountID)
			{
				pReturnSIPUser=pSIPUser;
				break;	
			}
		}
	}
	return pReturnSIPUser;
}

SIPMediaCall* SIPCallMgr::CreateSIPMediaCall(SIPUser * pSIPUser,std::string callID,std::string strHost) 
{
	if(strHost.size() ==0 ) strHost=m_strLocalIP;
	SIPMediaCall*pSIPMediaCall=NULL;
	if( pSIPUser == NULL ) pSIPUser=m_pIPCallSIPUser;
	if(pSIPUser!=NULL)
		pSIPMediaCall=pSIPUser->onNewSession(callID,strHost);
	return pSIPMediaCall;
}

SIPMediaCall*  SIPCallMgr::GetSIPMediaCallFromCallID(const char*cszCallID)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	if (cszCallID==NULL || strlen(cszCallID)==0)
		return NULL;
	{
		XAutoLock l(m_csMapThrSIPMediaCall);
		MapSIPMediaCall::iterator iter=m_MapThrSIPMediaCall.find(cszCallID);
		if (iter!=m_MapThrSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	}
	return pSIPMediaCall;
}

void SIPCallMgr::OnUserIdRegistration(int nAccountId,int status,const std::string&strErrorCode)
{
	//wait find form AccountId to lineid
	int nLineID=0;
	SIPUser *pSIPUser=NULL;
	pSIPUser=GetSIPUserFromAccountID(nAccountId);
	if(pSIPUser!=NULL)
	{
		nLineID=pSIPUser->GetLineID();
		std::string strUser=pSIPUser->m_strUserID;
		if(status == 200 && pSIPUser->m_nRegSetting ==2)
		{
			pSIPUser->m_nSIPCheckLastTimestamp =XGetTimestamp();
			SetCurrentVirtualLine(nLineID);
			OnRegistrationSuccess(nLineID,strUser);
			pSIPUser->onRegistrationSuccess();
			m_nSIPREGFail=2;
			char szUserIDBack[128]={0}; strcpy(szUserIDBack,pSIPUser->m_strUserID.c_str());
			OutPutStackLog(0,"SIPStack Register Success.",szUserIDBack);
		}
		else
		{
			OnRegistrationError(nLineID,strErrorCode,strUser);
			pSIPUser->onRegistrationError(strErrorCode);
			//RemoveVirtualLine(nLineID);
			m_nSIPREGFail=1;
			char szUserIDBack[128]={0}; strcpy(szUserIDBack,pSIPUser->m_strUserID.c_str());
			OutPutStackLog(1,"SIPStack Register Failure!",szUserIDBack);
		}
	}
}

/******************************************************************************
* MakeCall
*描述：发起呼叫
*输入：	cszUserID		-对方的用户ID或IP
*输出：无
*返回值：呼叫标识
*****************************************************************************/
const char*SIPCallMgr::MakeCall(const char*cszUserID,ConfInfo *pConfInfo)
{
	if (m_nStarted ==0 ) return m_strEmpty.c_str();
	//test
	char msg[256]={0};
	sprintf(msg,"nVideoFormatType%d,nFrameRate%d,cType%d,nBitrate%d",m_nSecondVideoFormatType,m_nSecondVideoDevFrameRate,m_nSecondVideoCodecType,m_nSecondVideoMaxBitrate);
	LogFile(NULL,NULL,"",msg,"SIPCallMgr::MakeCall");

	if (cszUserID==NULL || strlen(cszUserID)==0) return m_strEmpty.c_str();
	SIPUser*pSIPUser=NULL;
	int nLineID=0;
	SIPMediaCall*pSIPMediaCall=NULL;
	sip_register_thread();

	if(IsCallIPPortString(cszUserID) >=0 )
	//if(IsCallIPString(cszUserID)==true)
	{
		//use to ip call
		pSIPUser=m_pIPCallSIPUser;
		//call self control
		if(m_pIPCallSIPUser->CompareLocalIPEqual(cszUserID) ==true) return m_strEmpty.c_str();
		if(pSIPUser)
		{
			m_nCurrentLineID=nLineID;
			pSIPMediaCall=pSIPUser->MakeCall(cszUserID,pConfInfo);
		}
	}
	else
	{
		nLineID=m_nCurrentLineID;
		{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
			XAutoLock l(m_csMapSIPUser);
#endif
			while (nLineID>=0)
			{
				MapSIPUser::iterator it=m_MapSIPUser.find(nLineID);
				if (it!=m_MapSIPUser.end())
				{
					pSIPUser=it->second;
					const std::string strUserID=pSIPUser->GetUserID();
					if (strUserID==cszUserID)
					{
						return m_strEmpty.c_str();
					}
					break;
				}
				else
				{
					nLineID--;
				}
			}
			if(pSIPUser)
			{
				m_nCurrentLineID=nLineID;
				pSIPMediaCall=pSIPUser->MakeCall(cszUserID,pConfInfo);
			}
		}
	}
	if (pSIPMediaCall!=NULL)
	{
		//if(pSIPMediaCall) pSIPMediaCall->SetCallNum(1);
		pSIPMediaCall->SetLineID(m_nCurrentLineID);
		return pSIPMediaCall->GetCallID().c_str();
	}
	return m_strEmpty.c_str();
}

void SIPCallMgr::AddCall(SIPMediaCall*pSIPMediaCall)
{
	if (pSIPMediaCall!=NULL)
	{
		{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
			XAutoLock l(m_csMapSIPMediaCall);
#endif
			m_MapSIPMediaCall[pSIPMediaCall->GetCallID()]=pSIPMediaCall;
		}
		{
			XAutoLock l(m_csMapThrSIPMediaCall);
			m_MapThrSIPMediaCall[pSIPMediaCall->GetCallID()]=pSIPMediaCall;
		}
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
		OnDAP_CallRingOut(pSIPMediaCall->GetCallID(),pSIPMediaCall->GetUserID(),pSIPMediaCall->GetUserName());

#else
		OnCallRingOut(pSIPMediaCall->GetCallID());
#endif
	}
}

/******************************************************************************
* AcceptCall
*描述：接受呼叫请求
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::AcceptCall(const char*cszCallID,ConfInfo *pConfInfo)
{
	if (cszCallID==NULL || strlen(cszCallID)==0) return -1;
	//wait find from callid to call
	//put it to answercall
	SIPMediaCall*pSIPMediaCall=NULL;
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif
		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
		//}
		if(pSIPMediaCall!=NULL)
		{
			return pSIPMediaCall->AcceptCall(pConfInfo);
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
int SIPCallMgr::HangupCall(const char*cszCallID)
{
	if (cszCallID==NULL || strlen(cszCallID)==0) return -1;

	LogFile(NULL,NULL,"","","SIPCallMgr::HangupCall         111");

	std::string strCallID = cszCallID;
	SIPMediaCall*pSIPMediaCall=NULL;
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif
		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
		//}
		//先挂断协议call
		if(pSIPMediaCall)
		{
			pSIPMediaCall->m_nLocalHungUP=1;
			pSIPMediaCall->HangupSIPCall();
		}
	}
	LogFile(NULL,NULL,"","","SIPCallMgr::HangupCall         222");

	return 0;
}

/******************************************************************************
* OnRecvHangupCall
*描述：接收到对端挂断呼叫
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::OnRecvHangupCall(const char*cszCallID)
{
	IMXEC_CALL_END_REASON discReason =ModeDisconnectedUnknown;
	if (cszCallID==NULL || strlen(cszCallID)==0)
		return -1;
	OnCallDestroyed(cszCallID,discReason);
	return 0;
}

//added by jiangdingfeng 2013-11-18 判断是否有呼叫
bool SIPCallMgr::HasCall()
{
	XAutoLock l(m_csMapSIPMediaCall);
	if (m_MapSIPMediaCall.size() >= 1)
	{
		return true;
	}
	return false;
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
int SIPCallMgr::SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
//#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
//	XAutoLock l(m_csMapSIPMediaCall);
//#endif
	//MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	//if (iter!=m_MapSIPMediaCall.end())
	//{
	//	SIPMediaCall*pSIPMediaCall=iter->second;
	//	return pSIPMediaCall->SendPTZ(nPTZAction,nSpeed);
	//}
	return -1;
}

/******************************************************************************
* StartSecondVideo
*描述：开始辅流
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::StartSecondVideo (const char*cszCallID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPMediaCall);
#endif
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;

		if(pSIPMediaCall->GetSupportSecondVideo() )
		{
			//notice straight send dualvideo
			if( (m_nUseDualVideoToken>0) || ( m_nUseDualVideoToken==0 && m_nUseDualVideoTokenType==1 ) )
				return pSIPMediaCall->StartSecondVideo();
			else
			{
				pSIPMediaCall->OnLocalSecondVideoStarted();
				return 0;
			}
		}
		else
		{
			pSIPMediaCall->OnLocalSecondVideoStarted();
			return 0;
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
int SIPCallMgr::StopSecondVideo (const char*cszCallID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPMediaCall);
#endif
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		if(pSIPMediaCall->GetSupportSecondVideo() )
		{
			//notice straight send dualvideo stop
			if( (m_nUseDualVideoToken>0) || ( m_nUseDualVideoToken==0 && m_nUseDualVideoTokenType==1 ) )
			{
				pSIPMediaCall->SendGenericCommand();
				return pSIPMediaCall->StopSecondVideo();
			}
			else
			{
				SIPCallMgr::Instance().OnCallSecondVideoStop(cszCallID);
				pSIPMediaCall->OnLocalSecondVideoStop();
				return 0;
			}
		}
		else
		{
			SIPCallMgr::Instance().OnCallSecondVideoStop(cszCallID);
			pSIPMediaCall->OnLocalSecondVideoStop();
			return 0;
		}
	}
	return -1;
}


#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0


/******************************************************************************
* SetLocalMainVideoWnd
*描述：设置本地主流视频显示的窗口
*输入：	cszCallID		-呼叫标识
hWnd			-本地主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetLocalMainVideoWnd(const char*cszCallID,HWND hWnd)
{
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->SetLocalMainVideoWnd(hWnd);
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
int SIPCallMgr::SetLocalMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->SetLocalMainVideoWndPos(x,y,cx,cy);
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
void SIPCallMgr::SetLocalMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetLocalMainVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
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
void SIPCallMgr::SetLocalMainVideoEnableOSD(const char*cszCallID,int nEnable)
{
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetLocalMainVideoEnableOSD(nEnable);
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
void SIPCallMgr::SetLocalMainVideoOSDText(const char*cszCallID,const char*cszText,int nAlign)//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT
{
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetLocalMainVideoOSDText(cszText,nAlign);
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
void SIPCallMgr::SetLocalMainVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetLocalMainVideoOSDFont(logfont,crText,crBk);
	}
}

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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
int SIPCallMgr::GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	int nRet=-1;
	int nFrameRate =0;

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return 0;
#else//#endif

	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		nRet= pSIPMediaCall->GetLocalMainVideoInfo(nWidth,nHeight,nVideoCodecType,hVideoWnd,nFrameRate);
		nRet = pSIPMediaCall->GetLocalMainVideoResolution(nWidth,nHeight,chInfo);
		if(nFrameRate >0) chInfo.m_nFrame =nFrameRate;
	}
#endif

	return nRet;
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
int SIPCallMgr::GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetLocalMainVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return -1;
}

#if defined(HAS_SIP_AVCON_MP) && ( (HAS_SIP_AVCON_MP == 1) || (HAS_SIP_AVCON_MP == 2) )

#else

/******************************************************************************
* SetRemoteMainVideoWnd
*描述：设置远程主流视频显示的窗口
*输入：	cszCallID		-呼叫标识
hWnd			-远程主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetRemoteMainVideoWnd(const char*cszCallID,HWND hWnd)
{
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->SetRemoteMainVideoWnd(hWnd);
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
int SIPCallMgr::SetRemoteMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->SetRemoteMainVideoWndPos(x,y,cx,cy);
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
void SIPCallMgr::SetRemoteMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)
{
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetRemoteMainVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}
}

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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
int SIPCallMgr::GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	int nRet =0;
	int nFrameRate =0;

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return nRet;
#else//

	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		nRet = pSIPMediaCall->GetRemoteMainVideoInfo(nWidth,nHeight,nVideoCodecType,hVideoWnd,nFrameRate);
		nRet = pSIPMediaCall->GetRemoteMainVideoResolution(nWidth,nHeight,chInfo);
		if(nFrameRate >0) chInfo.m_nFrame =nFrameRate;
	}
#endif

	return nRet;
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
int SIPCallMgr::GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetRemoteMainVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return -1;
}

/******************************************************************************
* SetSupportH239
*描述：设置是否支持辅流
*输入：nSupportH239		-是否支持辅流
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetSupportH239(int nSupportH239)
{
	m_nSupportH239=nSupportH239;
	return 0;
}

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

/******************************************************************************
* SetSecondVideoWnd
*描述：设置辅流视频显示的窗口
*输入：	cszCallID		-呼叫标识
hWnd			-远程辅流视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetSecondVideoWnd(const char*cszCallID,HWND hWnd)
{
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->SetSecondVideoWnd(hWnd);
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
int SIPCallMgr::SetSecondVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->SetSecondVideoWndPos(x,y,cx,cy);
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
void SIPCallMgr::SetSecondVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetSecondVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
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
void SIPCallMgr::SetSecondVideoEnableOSD(const char*cszCallID,int nEnable)
{
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetSecondVideoEnableOSD(nEnable);
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
void SIPCallMgr::SetSecondVideoOSDText(const char*cszCallID,const char*cszText,int nAlign)//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT
{
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetSecondVideoOSDText(cszText,nAlign);
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
void SIPCallMgr::SetSecondVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetSecondVideoOSDFont(logfont,crText,crBk);
	}
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
int SIPCallMgr::HotDisplayVideo(const char*cszCallID,HWND hWndLocal,HWND hWndRemote,HWND hWndSecond)
{
	//notice the not definition
	//int nRet=0;
	//SIPMediaCall*pSIPMediaCall=NULL;
	//MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	//if (iter!=m_MapSIPMediaCall.end())
	//{
	//	pSIPMediaCall=iter->second;

	//}
	//if(pSIPMediaCall)
	//{
	//	if(hWndRemote) nRet=pSIPMediaCall->SetRemoteMainVideoWnd(hWndRemote);
	//	if(hWndLocal) nRet=pSIPMediaCall->SetLocalMainVideoWnd(hWndLocal);
	//	if(hWndSecond) nRet=pSIPMediaCall->SetSecondVideoWnd(hWndSecond);
	//	//pSIPMediaCall->OpenRemoteMainPlayChan();
	//	return nRet;
	//}
	return -1;
}

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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
int SIPCallMgr::GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	int nRet=-1;

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return 0;
#else//

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		nRet = pSIPMediaCall->GetSecondVideoInfo(nWidth,nHeight,nVideoCodecType,hVideoWnd);
		nRet = pSIPMediaCall->GetSecondVideoResolution(nWidth,nHeight,chInfo);
	}
#endif

	return nRet;
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
int SIPCallMgr::GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetSecondVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return 0;
}

/******************************************************************************
* GetSecondVideoDirection
*描述：获取辅流的方向
*输入：	cszCallID		-呼叫标识
*输出： nDir			-辅流的方向
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir)
{
	int nRet=-1;
	#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
	#endif
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		if (pSIPMediaCall->GetSecondVideoDirection(nDir)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
}


/******************************************************************************
* GetLocalAudioLevel
*描述：获取本地音频Level【实际声音大小】
*输入：	cszCallID		-呼叫标识
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::GetLocalAudioLevel(const char*cszCallID,int&nLevel)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return 0;
#else//

	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetLocalAudioLevel(nLevel);
	}
#endif
	return 0;
}

/******************************************************************************
* SetLocalAudioVolume
*描述：获取本地音频音量
*输入：	cszCallID		-呼叫标识
nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void SIPCallMgr::SetLocalAudioVolume(const char*cszCallID,int nVolume)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return ;
#else//

	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetLocalAudioVolume(nVolume);
	}
#endif
}

/******************************************************************************
* SetLocalAudioMute
*描述：获取本地音频是否静音
*输入：	cszCallID		-呼叫标识
nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void SIPCallMgr::SetLocalAudioMute(const char*cszCallID,int nMute)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return ;
#else//

	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetLocalAudioMute(nMute);
	}
#endif
}

/******************************************************************************
* GetLocalAudioCodecID
*描述：获取本地音频编码信息
*输入：	cszCallID		-呼叫标识
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return 0;
#else//

	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetLocalAudioCodecID(nAudioCodecID);
	}
#endif
	return 0;
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
int SIPCallMgr::GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetLocalAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return 0;
}

/******************************************************************************
* GetRemoteAudioLevel
*描述：获取远程音频Level【实际声音大小】
*输入：	cszCallID		-呼叫标识
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::GetRemoteAudioLevel(const char*cszCallID,int&nLevel)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return 0;
#else//
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetRemoteAudioLevel(nLevel);
	}
#endif
	return 0;
}

/******************************************************************************
* SetRemoteAudioVolume
*描述：获取远程音频音量
*输入：	cszCallID		-呼叫标识
nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void SIPCallMgr::SetRemoteAudioVolume(const char*cszCallID,int nVolume)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return ;
#else//
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetRemoteAudioVolume(nVolume);
	}
#endif
}

/******************************************************************************
* SetRemoteAudioMute
*描述：获取远程音频是否静音
*输入：	cszCallID		-呼叫标识
nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void SIPCallMgr::SetRemoteAudioMute(const char*cszCallID,int nMute)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return ;
#else//

	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetRemoteAudioMute(nMute);
	}
#endif
}

/******************************************************************************
* GetRemoteAudioCodecID
*描述：获取远端音频编码
*输入：	cszCallID		-呼叫标识
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return 0;
#else//

	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetRemoteAudioCodecID(nAudioCodecID);
	}
#endif
	return 0;
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
int SIPCallMgr::GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetRemoteAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return 0;
}

/******************************************************************************
* GetCallSupportSecondVideo
*描述：获取呼叫是否支持辅流【H239】
*输入：	cszCallID		-呼叫标识
*输出：	无
*返回值：支持返回非0，不支持返回0
*****************************************************************************/
int SIPCallMgr::GetCallSupportSecondVideo(const char*cszCallID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPMediaCall);
#endif
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetSupportSecondVideo();
	}
	return 0;
}

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

/******************************************************************************
* StartRecordLocalMainToAVI
*描述：开始本地主流录像
*输入：	cszCallID			-呼叫标识
cszPathName			-录像文件名称
nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::StartRecordLocalMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->StartRecordLocalMainToAVI(cszPathName,nStandardFormat);
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
void SIPCallMgr::StopRecordLocalMainToAVI(const char*cszCallID)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->StopRecordLocalMainToAVI();
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
int SIPCallMgr::StartRecordRemoteMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->StartRecordRemoteMainToAVI(cszPathName,nStandardFormat);
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
void SIPCallMgr::StopRecordRemoteMainToAVI(const char*cszCallID)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->StopRecordRemoteMainToAVI();
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
int SIPCallMgr::StartRecordSecondToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->StartRecordSecondToAVI(cszPathName,nStandardFormat);
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
void SIPCallMgr::StopRecordSecondToAVI(const char*cszCallID)
{
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->StopRecordSecondToAVI();
	}
}

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

/******************************************************************************
* GetMainAudioID
*描述：获取主流音频标识
*输入： cszCallID		-呼叫标识
*输出： nMainAudioID	-主流音频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetMainAudioID(nMainAudioID);
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
int SIPCallMgr::GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetMainVideoID(nMainVideoID);
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
int SIPCallMgr::GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		return pSIPMediaCall->GetSecondVideoID(nSecondVideoID);
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
int SIPCallMgr::SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return 0;
#else//

	//XAutoLock l(m_csMapSIPMediaCall);
	int nRet=-1;
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		if (pSIPMediaCall->SetMainVideoScreenIndex(nScreenIndex)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
#endif
}

/******************************************************************************
* SetSecondVideoScreenIndex
*描述：设置辅流视频图像桌面序号
*输入： cszCallID		-呼叫标识
nScreenIndex	-桌面序号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return 0;
#else//
	int nRet=-1;
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		if (pSIPMediaCall->SetSecondVideoScreenIndex(nScreenIndex)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
#endif
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
void SIPCallMgr::SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return ;
#else//
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetSecondVideoScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
#endif
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
void SIPCallMgr::SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return ;
#else//

	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetRemoteVideoScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
#endif
}

//added by jiangdingfeng 2013-09-25 增加I帧请求
/******************************************************************************
* RequestMainVideoKeyFrame
*描述：请求主流I帧
*输入：cszCallID		-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void SIPCallMgr::RequestMainVideoKeyFrame(const char*cszCallID)
{
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->RequestMainVideoKeyFrame();
	}
}

/******************************************************************************
* RequestSecondVideoKeyFrame
*描述：请求辅流I帧
*输入：cszCallID		-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void SIPCallMgr::RequestSecondVideoKeyFrame(const char*cszCallID)
{
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->RequestSecondVideoKeyFrame();
	}
}

/******************************************************************************
* SetLocalMainVideoSource
*描述：设置本地主视频源
*输入：cszCallID		  -呼叫标识
nMainVideoDevID    -主流设备ID
*输出：无
*返回值：无
*****************************************************************************/
void SIPCallMgr::SetLocalMainVideoSource(const char*cszCallID, int nMainVideoDevID)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","LOCAL_MAIN_VIDEO");
		tStrPacket.Set("STATE","ON");
		tStrPacket.Set("CID",cszCallID);

		std::string strText="";

		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
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
void SIPCallMgr::SetLocalSecondVideoSource(const char*cszCallID, int nSecondVideoDevID)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","LOCAL_SECOND_VIDEO");
		tStrPacket.Set("STATE","ON");
		tStrPacket.Set("CID",cszCallID);

		std::string strText="";

		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}


#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

/******************************************************************************
* SetLocalRecordCallback
*描述：设置本地录像回调
*输入：	cszCallID		-呼叫标识
pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void SIPCallMgr::SetLocalRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetLocalRecordCallback(pRecordCallback);
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
void SIPCallMgr::SetRemoteRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetRemoteRecordCallback(pRecordCallback);
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
void SIPCallMgr::SetSecondRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->SetSecondRecordCallback(pRecordCallback);
	}
}

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0


/******************************************************************************
* SaveLocalMainVideoToBMPFile
*描述：保存本地主流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SaveLocalMainVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	int nRet=-1;

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		if (pSIPMediaCall->SaveLocalMainVideoToBMPFile(cszPathName)>=0)
		{
			nRet=0;
		}
	}

#endif

	return nRet;
}

/******************************************************************************
* SaveRemoteMainVideoToBMPFile
*描述：保存远程主流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SaveRemoteMainVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	//XAutoLock l(m_csMapSIPMediaCall);

	int nRet=-1;

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		if (pSIPMediaCall->SaveRemoteMainVideoToBMPFile(cszPathName)>=0)
		{
			nRet=0;
		}
	}
#endif

	return nRet;
}

/******************************************************************************
* SaveSecondVideoToBMPFile
*描述：保存辅流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SaveSecondVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	int nRet=-1;

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		if (pSIPMediaCall->SaveSecondVideoToBMPFile(cszPathName)>=0)
		{
			nRet=0;
		}
	}
#endif

	return nRet;
}

/******************************************************************************
* SendDTMF
*描述：发送DTMF
*输入：	cszCallID		-呼叫标识
nDTMF			-DTMF
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SendDTMF(const char*cszCallID,const char nDTMF)
{
	int nRet=-1;
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		if (pSIPMediaCall->SendDTMF(nDTMF)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
}

/******************************************************************************
* StartMixingWithWaveFile
*描述：开始对呼叫进行WAVE文件混音，对方会听到WAVE文件播放的声音
*输入：	cszCallID		-呼叫标识
cszPathName		-WAVE文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::StartMixingWithWaveFile(const char*cszCallID,const char*cszPathName)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return 0;
#else//
	int nRet=-1;
	//XAutoLock l(m_csMapSIPMediaCall);
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		if (pSIPMediaCall->StartMixingWithWaveFile(cszPathName)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
#endif
}

/******************************************************************************
* StopMixingWithWaveFile
*描述：停止对呼叫进行WAVE文件混音
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::StopMixingWithWaveFile(const char*cszCallID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return 0;
#else//
	int nRet=-1;
	//XAutoLock l(m_csMapSIPMediaCall);

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		if (pSIPMediaCall->StopMixingWithWaveFile()>=0)
		{
			nRet=0;
		}
	}
	return nRet;
#endif
}

/******************************************************************************
* GetHasVideo
*描述：获取是否只有视频
*输入：cszCallID		-呼叫标识
*输出：无
*返回值：如果有视频，那么返回非0，否则返回0
*****************************************************************************/
int SIPCallMgr::GetHasVideo(const char*cszCallID)
{
	int nRet=0;
	//XAutoLock l(m_csMapSIPMediaCall);
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPMediaCall);
#endif
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		nRet=pSIPMediaCall->GetHasVideo();
	}
	return nRet;
}

bool SIPCallMgr::ExcludePlayChan(unsigned long nAudioID,AUDEC_PlayChan*pPlayChan)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return true;
#else//

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.begin();
	while (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		unsigned long nTempID=0;
		pSIPMediaCall->GetMainAudioID(nTempID);
		if (nTempID==nAudioID)
		{
			pSIPMediaCall->ExcludePlayChan(pPlayChan);
			return true;
		}
		else
		{
			++iter;
		}
	}
#endif
	return false;
}

void SIPCallMgr::OnRegistrationSuccess(int nLineID,std::string strUserID)
{
	unsigned int uLinedID=nLineID;
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","REG");
		tStrPacket.Set("TYPE","OK");
		tStrPacket.Set("LINE",uLinedID);
		tStrPacket.Set("PHONENUM",strUserID);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void SIPCallMgr::OnRegistrationError(int nLineID,const std::string&strErrorCode,std::string strUserID)
{
	unsigned int uLinedID=nLineID;
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","REG");
		tStrPacket.Set("TYPE","FAILED");
		tStrPacket.Set("LINE",uLinedID);
		tStrPacket.Set("PHONENUM",strUserID);
		tStrPacket.Set("ERROR",strErrorCode);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void SIPCallMgr::OnSIPMediaCallCreated (SIPMediaCall*pSIPMediaCall)
{
	XAutoLock l(m_csMapThrSIPMediaCall);
	m_MapThrSIPMediaCall[pSIPMediaCall->GetCallID()]=pSIPMediaCall;
}

void SIPCallMgr::OnCallRingIn (const std::string& strCallID)
{
	std::string strUserID;
	std::string strUserName;

	SIPMediaCall*pSIPMediaCall=NULL;
	{
		XAutoLock l(m_csMapThrSIPMediaCall);
		MapSIPMediaCall::iterator iter=m_MapThrSIPMediaCall.find(strCallID);
		if (iter!=m_MapThrSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
		if (pSIPMediaCall!=NULL)
		{
			unsigned char nUserIDIndex=0;
			bool bUserIDIndexChanged=false;
			while (true)
			{
				bUserIDIndexChanged=false;
				iter=m_MapThrSIPMediaCall.begin();
				while (iter!=m_MapThrSIPMediaCall.end())
				{
					SIPMediaCall*pTempCall=iter->second;
					if (pSIPMediaCall!=pTempCall && pTempCall->GetUserID()==pSIPMediaCall->GetUserID())
					{
						if (nUserIDIndex==pTempCall->GetUserIDIndex())
						{
							nUserIDIndex++;
							bUserIDIndexChanged=true;
							break;
						}
					}
					++iter;
				}
				if (bUserIDIndexChanged==false)
				{
					break;
				}
			}
			pSIPMediaCall->SetUserIDIndex(nUserIDIndex);
		}
	}
	if(pSIPMediaCall!=NULL) 
	{
		if (pSIPMediaCall->GetCallStatus()==SIPCall::CALL_STATUS_RING)
		{
			strUserID=pSIPMediaCall->GetUserID();
			strUserName=pSIPMediaCall->GetUserName();

			unsigned char nUserIDIndex=pSIPMediaCall->GetUserIDIndex();
			if (nUserIDIndex>0)
			{
				char szIndex[128]="";
				sprintf(szIndex,"(%u)",nUserIDIndex);
				strUserID+=szIndex;
			}

			XDataBuffer*pXDataBuffer=new XDataBuffer();
			if (pXDataBuffer!=NULL)
			{
				StrPacket tStrPacket;
				tStrPacket.Set("CMD","CALL");
				tStrPacket.Set("STATE","IN");
				tStrPacket.Set("CID",strCallID);
				tStrPacket.Set("UID",strUserID);
				tStrPacket.Set("UNAME",strUserName);

				std::string strText="";
				tStrPacket.GetString(strText);

				pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
				SetDataBuffer(pXDataBuffer);
			}
		}
	}
}

void SIPCallMgr::OnCallSDPNegotiate(SIPCall*pSIPMediaCall)
{
	std::string strCallID;
	std::string strUserID;
	std::string strUserName;
	if(!pSIPMediaCall) return;	
	//if(pSIPMediaCall->IsCallEstablished()) return; //限制接通后再次收到200 OK SDP不再重新打开音视频库
	strCallID=pSIPMediaCall->GetCallID();
	strUserID=pSIPMediaCall->GetUserID();
	strUserName=pSIPMediaCall->GetUserName();

	StrPacket tStrPacket;
	tStrPacket.Set("CMD","CALL");
	tStrPacket.Set("STATE","SDPNEG");
	tStrPacket.Set("CID",strCallID);
	tStrPacket.Set("UID",strUserID);
	tStrPacket.Set("UNAME",strUserName);
	std::string strText="";
	tStrPacket.GetString(strText);
	if(pSIPMediaCall->IsCallEstablished())
	{
		XDataBuffer*pXDataBuffer=new XDataBuffer();
		if (pXDataBuffer!=NULL)
		{
			pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
			SetDataBuffer(pXDataBuffer);
		}
	}
	else
	{
		pSIPMediaCall->SaveCallMediaUpdatePacket(strText);
	}

	//retrieveCall test
	return;

	tStrPacket.Set("CMD","CALL");
	tStrPacket.Set("STATE","RECALL");
	tStrPacket.Set("CID",strCallID);
	tStrPacket.Set("UID",strUserID);
	tStrPacket.Set("UNAME",strUserName);
	strText="";
	tStrPacket.GetString(strText);
	if(pSIPMediaCall->IsCallEstablished())
	{
		XDataBuffer* pXDataBuffer=new XDataBuffer();
		if (pXDataBuffer!=NULL)
		{
			pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
			SetDataBuffer(pXDataBuffer);
		}
	}
	else
	{
		pSIPMediaCall->SaveCallMediaUpdatePacket(strText);
	}

}

void SIPCallMgr::OnCallBFCPTransportChange(SIPCall*pSIPMediaCall)
{
	std::string strCallID;
	std::string strUserID;
	std::string strUserName;
	if(!pSIPMediaCall) return;
	strCallID=pSIPMediaCall->GetCallID();
	strUserID=pSIPMediaCall->GetUserID();
	strUserName=pSIPMediaCall->GetUserName();
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","BFCP");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void SIPCallMgr::OnCallRingOut (const std::string& strCallID)
{
	std::string strUserID;
	std::string strUserName;
	SIPMediaCall*pSIPMediaCall=NULL;
	{
		//XAutoLock l(m_csMapSIPMediaCall);

		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	}
	if(!pSIPMediaCall) return;
	strUserID=pSIPMediaCall->GetUserID();
	strUserName=pSIPMediaCall->GetUserName();
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","OUT");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void SIPCallMgr::OnCallSecondVideoStart(const std::string&strCallID,int type)
{
	std::string strType="NOTIFY";
	if(type==1) strType="MEDIA";
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","H239");
		tStrPacket.Set("STATE","ON");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("TYPE",strType);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}


void SIPCallMgr::OnCallSecondVideoStop(const std::string&strCallID,int type)
{
	std::string strType="NOTIFY";
	if(type==1) strType="MEDIA";

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","H239");
		tStrPacket.Set("STATE","OFF");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("TYPE",strType);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void SIPCallMgr::OutPutStackLog(int nLogType,const char * cszLog,char * szCallID)
{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

	int nLen1=0;
	if(cszLog) nLen1=strlen(cszLog);
	int nLen2=0;
	if(szCallID) nLen2 =strlen(szCallID);
	char logBuf[512]={0};
	int nLenPos=0;
	if(nLen1 >0) nLenPos += _snprintf(&logBuf[nLenPos],510 -nLenPos,"%s",cszLog);
	if(nLen2 >0) nLenPos += _snprintf(&logBuf[nLenPos],510 -nLenPos,"ID:%s",szCallID);
	nLenPos += _snprintf(&logBuf[nLenPos],510 -nLenPos,"\n");

	OnDAP_OutPutStackLog(nLogType,logBuf,nLenPos);
	//OutputDebugString(logBuf);

#else
#endif

}

void SIPCallMgr::OutPutCapContent(const char*cszCallID,int nCapType,const char * cszContent)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	OnDAP_OutPutCapContent(cszCallID,nCapType,cszContent);
#else
#endif
}

int SIPCallMgr::GetDAPCapPrintEnable(void)
{
	return m_nCapPrint;
}

void SIPCallMgr::OnDAP_OutPutStackLog(int nLogType,const char * cszLog,int nLen)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	nLen=strlen(cszLog);
	CallMgr::Instance().OnDAP_OutPutStackLog(nLogType,cszLog,nLen);
#endif
}

void SIPCallMgr::OnDAP_OutPutCapContent(const char*cszCallID,int nCapType,const char * cszContent)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	int nLen=strlen(cszContent);
	CallMgr::Instance().OnDAP_OutPutCapContent(cszCallID,nCapType,cszContent,nLen);
#endif
}

void SIPCallMgr::OnDAP_CallRingIn(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,const std::string& strE164ID,const std::string& strPacket,std::string & strTelNum)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","IN");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);
		tStrPacket.Set("UEID",strE164ID);
		tStrPacket.Set("PACKET",strPacket);
		tStrPacket.Set("TELNUM",strTelNum);

		std::string strText="";
		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
	OutPutStackLog(0,"SIPStack CallRingIn,",(char *)strCallID.c_str());
}

void SIPCallMgr::OnDAP_CallRingOut(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","OUT");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);

		std::string strText="";
		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
	OutPutStackLog(0,"SIPStack CallRingOut,",(char *)strCallID.c_str());

}


void SIPCallMgr::OnDAP_CallEstablished(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,const std::string& strPacket)
{
	//notice the test
//#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","OK");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);
		tStrPacket.Set("PACKET",strPacket);

		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

//#else
//
//#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
//	//notice the test
//	CallMgr::Instance().OnCallEstablished(strCallID,strUserID,strUserName,strPacket.c_str(),strPacket.length());
//#endif
//
//#endif

	OutPutStackLog(0,"SIPStack CallEstablished,",(char *)strCallID.c_str());

}

void SIPCallMgr::OnDAP_CallDestroyed(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,IMXEC_CALL_END_REASON nReason,const std::string& strReason )
{
	OutPutStackLog(0,"SIPStack CallDestroyed,",(char *)strCallID.c_str());

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","BYE");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);
		tStrPacket.Set("RSN",strReason);

		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

}


void SIPCallMgr::OnDAP_CallMediaChannelConnected(int type,int sendRecv,HDAPAPPCALL hdapCall,const std::string& strCallID,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,unsigned long ip,int port,int nWidth,int nHeight,int nLevel)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	if(type==2) //main video
	{
		if(sendRecv==1) //send
		{
			SetDataBufferChannelConnectedVD(strCallID,codecType,nBitrate,nPayloadType,ip, port,nWidth,nHeight,nLevel,"SCHCONN","VIDEO",hdapCall);
			OutPutStackLog(0,"SIPStack MainVideoSendChannelConnected,",(char *)strCallID.c_str());
		}
		if(sendRecv==2) //recv
		{
			SetDataBufferChannelConnectedVD(strCallID,codecType,nBitrate,nPayloadType,ip, port,nWidth,nHeight,nLevel,"RCHCONN","VIDEO");
			OutPutStackLog(0,"SIPStack MainVideoRecvChannelConnected,",(char *)strCallID.c_str());
		}
	}
	if(type==3)  //second video
	{
		if(sendRecv==1) //send
		{
			SetDataBufferChannelConnectedVD(strCallID,codecType,nBitrate,nPayloadType,ip, port,nWidth,nHeight,nLevel,"SCHCONN","DUALVIDEO",hdapCall);
			OutPutStackLog(0,"SIPStack SecondVideoSendChannelConnected,",(char *)strCallID.c_str());
		}
		if(sendRecv==2) //recv
		{
			SetDataBufferChannelConnectedVD(strCallID,codecType,nBitrate,nPayloadType,ip, port,nWidth,nHeight,nLevel,"RCHCONN","DUALVIDEO");
			OutPutStackLog(0,"SIPStack SecondVideoRecvChannelConnected,",(char *)strCallID.c_str());
		}
	}

#else
	if(type==2) //main video
	{
		//A7 add
		VIDEO_FORMAT_TYPE formatType=GetVideoFormatType(nWidth,nHeight);

		if(sendRecv==1) //send
		{
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
			CallMgr::Instance().OnDAP_CallMainVideoSendChannelConnected(hdapCall,strCallID.c_str(),codecType
				,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);

#else

			CallMgr::Instance().OnDAP_CallMainVideoSendChannelConnected(hdapCall,strCallID.c_str(),codecType
				,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
#endif

			OutPutStackLog(0,"SIPStack MainVideoSendChannelConnected,",(char *)strCallID.c_str());
		}
		if(sendRecv==2) //recv
		{
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
			CallMgr::Instance().OnDAP_CallMainVideoRecvChannelConnected(strCallID.c_str(),codecType,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
#else

			CallMgr::Instance().OnDAP_CallMainVideoRecvChannelConnected(strCallID.c_str(),codecType,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
#endif
			OutPutStackLog(0,"SIPStack MainVideoRecvChannelConnected,",(char *)strCallID.c_str());
		}
	}
	if(type==3)  //second video
	{
		//A7 add
		VIDEO_FORMAT_TYPE formatType=GetVideoFormatType(nWidth,nHeight);

		if(sendRecv==1) //send
		{
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
			CallMgr::Instance().OnDAP_CallSecondVideoSendChannelConnected(hdapCall,strCallID.c_str(),codecType
				,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);

#else

			CallMgr::Instance().OnDAP_CallSecondVideoSendChannelConnected(hdapCall,strCallID.c_str(),codecType
				,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
#endif

			OutPutStackLog(0,"SIPStack SecondVideoSendChannelConnected,",(char *)strCallID.c_str());
		}
		if(sendRecv==2) //recv
		{
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
			CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelConnected(strCallID.c_str(),codecType,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
#else

			CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelConnected(strCallID.c_str(),codecType,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
#endif
			OutPutStackLog(0,"SIPStack SecondVideoRecvChannelConnected,",(char *)strCallID.c_str());
		}
	}
#endif

#endif
}

void SIPCallMgr::OnDAP_CallMediaChannelDestroyed(int type,int sendRecv,const std::string& strCallID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	if(type==1) //audio
	{
		if(sendRecv==1) //send
		{
			SetDataBufferChannelDestroyed(strCallID,"SCHDISCO","AUDIO");
			OutPutStackLog(0,"SIPStack AudioSendChannelDestroyed,",(char *)strCallID.c_str());
		}
		if(sendRecv==2)  //recv
		{
			SetDataBufferChannelDestroyed(strCallID,"RCHDISCO","AUDIO");
			OutPutStackLog(0,"SIPStack AudioRecvChannelDestroyed,",(char *)strCallID.c_str());
		}
	}
	else if(type==2) //main video
	{
		if(sendRecv==1) //send
		{
			SetDataBufferChannelDestroyed(strCallID,"SCHDISCO","VIDEO");
			OutPutStackLog(0,"SIPStack MainVideoSendChannelDestroyed,",(char *)strCallID.c_str());
		}
		if(sendRecv==2) //recv
		{
			SetDataBufferChannelDestroyed(strCallID,"RCHDISCO","VIDEO");
			OutPutStackLog(0,"SIPStack MainVideoRecvChannelDestroyed,",(char *)strCallID.c_str());
		}
	}
	else if(type==3) //second video
	{
		if(sendRecv==1) //send
		{
			SetDataBufferChannelDestroyed(strCallID,"SCHDISCO","DUALVIDEO");
			OutPutStackLog(0,"SIPStack SecondVideoSendChannelDestroyed,",(char *)strCallID.c_str());
		}
		if(sendRecv==2) //recv
		{
			SetDataBufferChannelDestroyed(strCallID,"RCHDISCO","DUALVIDEO");
			OutPutStackLog(0,"SIPStack SecondVideoRecvChannelDestroyed,",(char *)strCallID.c_str());
		}
	}
	else if(type==4) //fecc
	{
		if(sendRecv==1) //send
		{
			SetDataBufferChannelDestroyed(strCallID,"SCHDISCO","FECC");
			OutPutStackLog(0,"SIPStack FECCVideoSendChannelDestroyed,",(char *)strCallID.c_str());
		}
		if(sendRecv==2) //recv
		{
			SetDataBufferChannelDestroyed(strCallID,"RCHDISCO","FECC");
			OutPutStackLog(0,"SIPStack FECCRecvChannelDestroyed,",(char *)strCallID.c_str());
		}
	}

#else
	if(type==1) //audio
	{
		if(sendRecv==1) //send
		{
			CallMgr::Instance().OnDAP_CallAudioSendChannelDestroyed(strCallID.c_str());
			OutPutStackLog(0,"SIPStack AudioSendChannelDestroyed,",(char *)strCallID.c_str());
		}
		if(sendRecv==2)  //recv
		{
			CallMgr::Instance().OnDAP_CallAudioRecvChannelDestroyed(strCallID.c_str());
			OutPutStackLog(0,"SIPStack AudioRecvChannelDestroyed,",(char *)strCallID.c_str());
		}
	}
	else if(type==2) //main video
	{
		if(sendRecv==1) //send
		{
			CallMgr::Instance().OnDAP_CallMainVideoSendChannelDestroyed(strCallID.c_str());
			OutPutStackLog(0,"SIPStack MainVideoSendChannelDestroyed,",(char *)strCallID.c_str());
		}
		if(sendRecv==2) //recv
		{
			CallMgr::Instance().OnDAP_CallMainVideoRecvChannelDestroyed(strCallID.c_str());
			OutPutStackLog(0,"SIPStack MainVideoRecvChannelDestroyed,",(char *)strCallID.c_str());
		}
	}
	else if(type==3) //second video
	{
		if(sendRecv==1) //send
		{
			CallMgr::Instance().OnDAP_CallSecondVideoSendChannelDestroyed(strCallID.c_str());
			OutPutStackLog(0,"SIPStack SecondVideoSendChannelDestroyed,",(char *)strCallID.c_str());
		}
		if(sendRecv==2) //recv
		{
			CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelDestroyed(strCallID.c_str());
			OutPutStackLog(0,"SIPStack SecondVideoRecvChannelDestroyed,",(char *)strCallID.c_str());
		}
	}
	else if(type==4) //fecc
	{
		if(sendRecv==1) //send
		{
			CallMgr::Instance().OnDAP_CallFECCSendChannelDestroyed(strCallID.c_str());
			OutPutStackLog(0,"SIPStack FECCVideoSendChannelDestroyed,",(char *)strCallID.c_str());
		}
		if(sendRecv==2) //recv
		{
			CallMgr::Instance().OnDAP_CallFECCRecvChannelDestroyed(strCallID.c_str());
			OutPutStackLog(0,"SIPStack FECCRecvChannelDestroyed,",(char *)strCallID.c_str());
		}
	}

#endif
#endif
}

void SIPCallMgr::OnDAP_CallAudioRecvChannelConnected(const std::string& strCallID,X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType,unsigned long ip, int port)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","RCHCONN");
		tStrPacket.Set("MEDIA","AUDIO");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("CODECID",(unsigned int)AudioCodecID);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		tStrPacket.Set("IP",ip);
		tStrPacket.Set("PORT",(unsigned int)port);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
#else

	CallMgr::Instance().OnDAP_CallAudioRecvChannelConnected(strCallID.c_str(),AudioCodecID,nPayloadType,ip,port);

#endif
#endif
	OutPutStackLog(0,"SIPStack AudioRecvChannelConnected,",(char *)strCallID.c_str());
}

void SIPCallMgr::OnDAP_CallAudioSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID,X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType,unsigned long ip, int port)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		unsigned int callPointer=(unsigned int)hdapCall;
		tStrPacket.Set("CMD","SCHCONN");
		tStrPacket.Set("MEDIA","AUDIO");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("CODECID",(unsigned int)AudioCodecID);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		tStrPacket.Set("IP",ip);
		tStrPacket.Set("PORT",(unsigned int)port);
		tStrPacket.Set("CPR",callPointer);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
#else

	CallMgr::Instance().OnDAP_CallAudioSendChannelConnected(hdapCall,strCallID.c_str(),AudioCodecID,nPayloadType,ip,port);

#endif
#endif
	OutPutStackLog(0,"SIPStack AudioSendChannelConnected,",(char *)strCallID.c_str());
}

void SIPCallMgr::OnDAP_CallFECCRecvChannelConnected(const std::string& strCallID ,int nPayloadType,unsigned long ip,int port)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","RCHCONN");
		tStrPacket.Set("MEDIA","FECC");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		tStrPacket.Set("IP",ip);
		tStrPacket.Set("PORT",(unsigned int)port);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
#else

	CallMgr::Instance().OnDAP_CallFECCRecvChannelConnected(strCallID.c_str(),nPayloadType,ip,port);

#endif
#endif
	OutPutStackLog(0,"SIPStack FECCRecvChannelConnected,",(char *)strCallID.c_str());
}

void SIPCallMgr::OnDAP_CallFECCSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID,int nPayloadType,unsigned long ip, int port)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		unsigned int callPointer=(unsigned int)hdapCall;
		tStrPacket.Set("CMD","SCHCONN");
		tStrPacket.Set("MEDIA","FECC");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		tStrPacket.Set("IP",ip);
		tStrPacket.Set("PORT",(unsigned int)port);
		tStrPacket.Set("CPR",callPointer);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
#else

CallMgr::Instance().OnDAP_CallFECCSendChannelConnected(hdapCall,strCallID.c_str(),nPayloadType,ip,port);

#endif
#endif
	OutPutStackLog(0,"SIPStack FECCVideoSendChannelConnected,",(char *)strCallID.c_str());
}

void SIPCallMgr::SetDataBufferChannelDestroyed(const std::string& strCallID,std::string strCMD,std::string strMedia)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD",strCMD);
		tStrPacket.Set("MEDIA",strMedia);
		tStrPacket.Set("CID",strCallID);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void SIPCallMgr::SetDataBufferChannelConnectedVD(const std::string& strCallID ,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth,int nHeight,int nLevel,std::string strCMD,std::string strMedia,HDAPAPPCALL hdapCall)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD",strCMD);
		tStrPacket.Set("MEDIA",strMedia);
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("CODECID",(unsigned int)codecType);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		tStrPacket.Set("IP",ip);
		tStrPacket.Set("PORT",(unsigned int)port);
		tStrPacket.Set("BIT",(unsigned int)nBitrate);
		tStrPacket.Set("WID",(unsigned int)nWidth);
		tStrPacket.Set("HEIG",(unsigned int)nHeight);
		tStrPacket.Set("LEV",(unsigned int)nLevel);
		if(hdapCall!=NULL)
		{
			unsigned int callPointer=(unsigned int)hdapCall;
			tStrPacket.Set("CPR",callPointer);
		}
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

int SIPCallMgr::DAPSendCallAudio(HDAPAPPCALL hdapCall,const char*cszCallID,char* RTPPacketData, int PacketDataLen)
{
	((SIPCall *)hdapCall)->SendAudio(RTPPacketData,PacketDataLen);
	return 0;
}

int SIPCallMgr::DAPSendCallMainVideo(HDAPAPPCALL hdapCall,const char*cszCallID,char* RTPPacketData, int PacketDataLen)
{
	((SIPCall *)hdapCall)->SendMainVideo(RTPPacketData,PacketDataLen);
	return 0;
}

int SIPCallMgr::DAPSendCallDualVideo(HDAPAPPCALL hdapCall,const char*cszCallID,char* RTPPacketData, int PacketDataLen)
{
	((/*SIPMediaCall*/SIPCall *)hdapCall)->SendSecondVideo(RTPPacketData,PacketDataLen);
	return 0;
}

int SIPCallMgr::SetAgentName(const char*cszAgent)
{
	int len=0;
	if(cszAgent)
	{
		len=strlen(cszAgent);
	}
	if(len>0 && len<256) strcpy(m_strUserAgentName,cszAgent);
	return 0;
}

int SIPCallMgr::SetDAPCapPrintEnable(int nPrint)
{
	m_nCapPrint=nPrint;
	return 0;
}

int SIPCallMgr::SetRegisterTimer(int regTimer,int regFailTimer)
{
	if(regTimer>0) m_nRegExpires=regTimer;
	if(regFailTimer>0) m_nSIPRegFailTimerTimestamp=regFailTimer*1000;
	return 0;
}

int SIPCallMgr::SetSIPLogFile(int logLevel,const char*cszLogFileName)
{
	if(logLevel>0) m_nSIPLogLevel=logLevel;
	if(cszLogFileName && strlen(cszLogFileName) >0 ) strcpy(m_strSIPLogFile,cszLogFileName);
	return 0;
}

int SIPCallMgr::SetDAPLocalMainVideoParam(const char*cszCallID,int nFrameRate,int nBitrate)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPMediaCall);
#endif
	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->m_nMainVideoBitrate=nBitrate;
		pSIPMediaCall->m_nMainVideoMaxFrame=nFrameRate;
	}
	return 0;
}

int SIPCallMgr::SetDAPLocalSecondVideoParam(const char*cszCallID,int nFrameRate,int nBitrate)
{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPMediaCall);
#endif

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->m_nSecondVideoMaxBitrate=nBitrate;
		pSIPMediaCall->m_nSecondVideoMaxFrame=nFrameRate;
	}
	return 0;
}

int SIPCallMgr::SetSIPCallSupportH239(const char*cszCallID,int nSupportH239)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPMediaCall);
#endif

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->m_nSupportH239=nSupportH239;
	}
	return 0;
}




int SIPCallMgr::SetLocalMainVideoH264Level(const char*cszCallID,int nLevel)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif
		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	if(pSIPMediaCall) pSIPMediaCall->SetLocalMainVideoH264Level(nLevel);
}
	return 0;
}

int SIPCallMgr::SetLocalSecondVideoH264Level(const char*cszCallID,int nLevel)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif
		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	if(pSIPMediaCall) pSIPMediaCall->SetLocalSecondVideoH264Level(nLevel);
}
	return 0;
}

int SIPCallMgr::SetLocalMainVideoH264HPLevel(const char*cszCallID,int nLevel)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif
		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	if(pSIPMediaCall) pSIPMediaCall->SetLocalMainVideoH264HPLevel(nLevel);
}
	return 0;
}

int SIPCallMgr::SetLocalSecondVideoH264HPLevel(const char*cszCallID,int nLevel)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif
		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	if(pSIPMediaCall) pSIPMediaCall->SetLocalSecondVideoH264HPLevel(nLevel);
}
	return 0;
}

int SIPCallMgr::SetCallRTPMediaMulticast(const char*cszCallID,const char*cszMulticastIP,int ttl)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif

		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	if(pSIPMediaCall) pSIPMediaCall->SetRTPMediaMulticast(cszMulticastIP,ttl);
	}
	return 0;
}


int SIPCallMgr::DAPSendCallDTMF (const char*cszCallID,const char dtmf)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif
		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	if(pSIPMediaCall) pSIPMediaCall->SendDTMF(dtmf,0);
	}
	return 0;
}

int SIPCallMgr::DAPSendCallCustomMsg (const char*cszCallID,const char * cszContent,const char * cszMimeType)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif
		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	if(pSIPMediaCall) pSIPMediaCall->SIPSendInfoRequest((char *)cszContent,(char *)cszMimeType);
	}
	return 0;
}

int SIPCallMgr::SendCallDualVideoControl(const char*cszCallID,int nType,int transactionId,int userId,int floorId,int status)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif
		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	
	if(pSIPMediaCall) pSIPMediaCall->SendDualVideoControlMsg(nType,transactionId,userId,floorId,status);
	}
	return 0;
}

void SIPCallMgr::ThreadProcMain(void)
{
while (m_bRunning)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && ( ( defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 0) ) || (! defined(NOT_WIN32_SIP_MSG) ) )
	m_bRunning=false;
#endif

	while (true)
	{
		XDataBuffer*pXDataBuffer=GetDataBuffer();
		if (pXDataBuffer!=NULL)
		{
			StrPacket tStrPacket(pXDataBuffer->GetData(),false);
			std::string strCMD="";
			tStrPacket.Get("CMD",strCMD);
			if (strCMD=="CALL")
			{
				std::string strState;
				std::string strCallID="";
				std::string strUserID="";
				std::string strUserName="";
				std::string strPacket="";
				std::string strTelNum="";
				std::string strUserE164ID="";

				tStrPacket.Get("STATE",strState);
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("UID",strUserID);
				tStrPacket.Get("UNAME",strUserName);
				tStrPacket.Get("UEID",strUserE164ID);

				if (strState=="IN")
				{
					tStrPacket.Get("PACKET",strPacket);
					tStrPacket.Get("TELNUM",strTelNum);
					SIPMediaCall*pSIPMediaCall=NULL;
					{
						XAutoLock l(m_csMapThrSIPMediaCall);
						MapSIPMediaCall::iterator iter=m_MapThrSIPMediaCall.find(strCallID);
						if (iter!=m_MapThrSIPMediaCall.end())
						{
							pSIPMediaCall=iter->second;
						}
					}
					if(pSIPMediaCall!=NULL)
					{
						pSIPMediaCall->OnCallRingIn();
					}
					if(pSIPMediaCall!=NULL)
					{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
						XAutoLock l(m_csMapSIPMediaCall);
#endif
						m_MapSIPMediaCall[strCallID]=pSIPMediaCall;
					}
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
					CallMgr::Instance().OnCallRingIn(strCallID,strUserID,strUserName,strUserE164ID,"SIP",strPacket.c_str(),strPacket.length(),(char *)(strTelNum.c_str()));
#endif

				}
				else if (strState=="OUT")
				{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
					CallMgr::Instance().OnCallRingOut(strCallID,strUserID,strUserName);
#endif
				}
				else if (strState=="OK")
				{
					tStrPacket.Get("PACKET",strPacket);

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
					CallMgr::Instance().OnCallEstablished(strCallID,strUserID,strUserName,strPacket.c_str(),strPacket.length());
#endif
				}
				else if (strState=="BYE")
				{
					std::string strReason="";
					tStrPacket.Get("RSN",strReason);

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
					CallMgr::Instance().OnCallDestroyed(strCallID,strUserID,strUserName,strReason.c_str(),strReason.length());
#endif

					SIPMediaCall*pSIPMediaCall=NULL;
					{
						{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
							XAutoLock l(m_csMapSIPMediaCall);
#endif

							MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
							if (iter!=m_MapSIPMediaCall.end())
							{
								pSIPMediaCall=iter->second;
								m_MapSIPMediaCall.erase(iter);
							}
							//}
							if(pSIPMediaCall) 
							{
								pSIPMediaCall->Close();
								pSIPMediaCall->CloseBFCPTransports();
								delete pSIPMediaCall;
								pSIPMediaCall=NULL;
							}
						}
						{
							XAutoLock l(m_csMapThrSIPMediaCall);
							MapSIPMediaCall::iterator iter=m_MapThrSIPMediaCall.find(strCallID);
							if (iter!=m_MapThrSIPMediaCall.end())
							{
								m_MapThrSIPMediaCall.erase(iter);
							}
						}
					}

				}
				else if (strState=="SDPNEG")
				{
					SIPMediaCall*pSIPMediaCall=NULL;
					{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
						XAutoLock l(m_csMapSIPMediaCall);
#endif
						MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
						if (iter!=m_MapSIPMediaCall.end())
						{
							pSIPMediaCall=iter->second;
						}
					//}
					if(pSIPMediaCall)
					{
						pSIPMediaCall->CloseMedia();
						pSIPMediaCall->OnMediaUpdate();
					}
					}
				}
				else if (strState=="BFCP")
				{
					SIPMediaCall*pSIPMediaCall=NULL;
					{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
						XAutoLock l(m_csMapSIPMediaCall);
#endif
						MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
						if (iter!=m_MapSIPMediaCall.end())
						{
							pSIPMediaCall=iter->second;
						}

						if(pSIPMediaCall) 
						{
							pSIPMediaCall->CreateBFCPTransports();
						}
					}
				}
				//retrieveCall test
				else if (strState=="RECALL")
				{
					SIPMediaCall*pSIPMediaCall=NULL;
					{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
						XAutoLock l(m_csMapSIPMediaCall);
#endif
						MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
						if (iter!=m_MapSIPMediaCall.end())
						{
							pSIPMediaCall=iter->second;
						}
					//}
					if(pSIPMediaCall) 
					{
						//retrieveCall(pSIPMediaCall->m_nCallId);
						//holdCall(pSIPMediaCall->m_nCallId);
						pSIPMediaCall->SetSipCallHold(pSIPMediaCall->m_nCallId);
					}
					}
				}
				else if (strState=="NONS")
				{
					std::string strPacket="";
					std::string strMime ="";
					tStrPacket.Get("INFO",strPacket);
					tStrPacket.Get("MIME",strMime);
					CallMgr::Instance().OnDAP_RecvNonstandardData(strCallID.c_str(),strPacket.c_str(),strPacket.length(),strMime.c_str());
				}
			}
			else if (strCMD=="H239")
			{
				std::string strCallID="";
				std::string strState="";
				std::string strType="";
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("STATE",strState);
				tStrPacket.Get("TYPE",strType);

				SIPMediaCall*pSIPMediaCall=NULL;
				{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
					XAutoLock l(m_csMapSIPMediaCall);
#endif
					MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
					if (iter!=m_MapSIPMediaCall.end())
					{
						pSIPMediaCall=iter->second;
					}
				}
				if (strState=="ON")
				{
					if( (strType=="MEDIA") && (pSIPMediaCall!=NULL) )
					{
						//pSIPMediaCall->OpenLocalSecondVideo();
					}
					CallMgr::Instance().OnCallSecondVideoStart(strCallID);
				}
				else if (strState=="OFF")
				{
					if( (strType=="MEDIA") && (pSIPMediaCall!=NULL) )
					{
						//pSIPMediaCall->OnSecondVideoSendChannelDestroyed();
					}

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

					//SetSecondVideoWnd(strCallID.c_str(),NULL);

#endif

					CallMgr::Instance().OnCallSecondVideoStop(strCallID);
				}
			}
			else if (strCMD=="REG")
			{
				std::string strUser="";
				std::string strType="";
				std::string strErrorCode="";
				unsigned int nLineID=0;
				tStrPacket.Get("TYPE",strType);
				tStrPacket.Get("LINE",nLineID);
				tStrPacket.Get("PHONENUM",strUser);
				if(strType=="OK") 
				{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
					CallMgr::Instance().OnDAP_SIPVirtualLineRegistrationSuccess(strUser.c_str(),nLineID);
#endif
				}
				if(strType=="FAILED")
				{
					tStrPacket.Get("ERROR",strErrorCode);
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
					CallMgr::Instance().OnDAP_SIPVirtualLineRegistrationError(strUser.c_str(),nLineID,(char *)strErrorCode.c_str());
#endif
				}
			}

			else if(strCMD=="RCHOPEN")
			{
				std::string strMedia="";
				std::string strCallID="";
				unsigned int codecType=0;
				unsigned int nBitrate=0;
				unsigned int nWidth=0;
				unsigned int nHeight=0;
				unsigned int nPayloadType=0;
				unsigned int nLevel=0;
				tStrPacket.Get("MEDIA",strMedia);
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("CODECID",codecType);
				tStrPacket.Get("PAYLOAD",nPayloadType);
				tStrPacket.Get("BIT",nBitrate);
				tStrPacket.Get("WID",nWidth);
				tStrPacket.Get("HEIG",nHeight);
				tStrPacket.Get("LEV",nLevel);
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

				if(strMedia=="FECC")
				{
					CallMgr::Instance().OnDAP_CallFECCRecvChannelOpen(strCallID.c_str());
				}
				else if(strMedia=="AUDIO")
				{ 
					CallMgr::Instance().OnDAP_CallAudioRecvChannelOpen(strCallID.c_str(),(X_AUDIO_CODEC_ID)codecType,nPayloadType);
				}
				else if(strMedia=="VIDEO")
				{
					CallMgr::Instance().OnDAP_CallMainVideoRecvChannelOpen(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType,nBitrate,nPayloadType,nWidth,nHeight,nLevel);
				}
				else if(strMedia=="DUALVIDEO")
				{
					CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelOpen(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType,nBitrate,nPayloadType,nWidth,nHeight,nLevel);
				}

#endif
			}
			else if( (strCMD=="SCHDISCO") || (strCMD=="RCHDISCO") )
			{
				std::string strMedia="";
				std::string strCallID="";
				tStrPacket.Get("MEDIA",strMedia);
				tStrPacket.Get("CID",strCallID);
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

				if(strMedia=="FECC")
				{
					if( strCMD=="SCHDISCO")
						CallMgr::Instance().OnDAP_CallFECCSendChannelDestroyed(strCallID.c_str());
					if( strCMD=="RCHDISCO")
						CallMgr::Instance().OnDAP_CallFECCRecvChannelDestroyed(strCallID.c_str());
				}
				else if(strMedia=="AUDIO")
				{
					if( strCMD=="SCHDISCO")
						CallMgr::Instance().OnDAP_CallAudioSendChannelDestroyed(strCallID.c_str());
					if( strCMD=="RCHDISCO")
						CallMgr::Instance().OnDAP_CallAudioRecvChannelDestroyed(strCallID.c_str());
				}
				else if(strMedia=="VIDEO")
				{
					if( strCMD=="SCHDISCO")
						CallMgr::Instance().OnDAP_CallMainVideoSendChannelDestroyed(strCallID.c_str());
					if( strCMD=="RCHDISCO")
						CallMgr::Instance().OnDAP_CallMainVideoRecvChannelDestroyed(strCallID.c_str());
				}
				else if(strMedia=="DUALVIDEO")
				{
					if( strCMD=="SCHDISCO")
						CallMgr::Instance().OnDAP_CallSecondVideoSendChannelDestroyed(strCallID.c_str());
					if( strCMD=="RCHDISCO")
						CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelDestroyed(strCallID.c_str());
				}

#endif

			}
			else if( (strCMD=="SCHCONN") || (strCMD=="RCHCONN") )
			{
				std::string strMedia="";
				std::string strCallID="";
				unsigned int codecType=0;
				unsigned int nBitrate=0;
				unsigned int nWidth=0;
				unsigned int nHeight=0;
				unsigned int nPayloadType=0;
				unsigned int nLevel=0;
				unsigned long ip=0;
				unsigned int  port=0;

				tStrPacket.Get("MEDIA",strMedia);
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("CODECID",codecType);
				tStrPacket.Get("PAYLOAD",nPayloadType);
				tStrPacket.Get("BIT",nBitrate);
				tStrPacket.Get("WID",nWidth);
				tStrPacket.Get("HEIG",nHeight);
				tStrPacket.Get("LEV",nLevel);
				tStrPacket.Get("IP",ip);
				tStrPacket.Get("PORT",(unsigned int)port);
				unsigned int callPointer=0;
				tStrPacket.Get("CPR",callPointer);
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

				if(strMedia=="FECC")
				{
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallFECCSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),nPayloadType,ip, port);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallFECCRecvChannelConnected(strCallID.c_str(),nPayloadType,ip, port);
				}
				else if(strMedia=="AUDIO")
				{
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallAudioSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(X_AUDIO_CODEC_ID)codecType,nPayloadType,ip, port);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallAudioRecvChannelConnected(strCallID.c_str(),(X_AUDIO_CODEC_ID)codecType,nPayloadType,ip, port);
				}
				else if(strMedia=="VIDEO")
				{
//A7 add
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
					VIDEO_FORMAT_TYPE formatType=GetVideoFormatType(nWidth,nHeight);
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallMainVideoSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallMainVideoRecvChannelConnected(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
#else
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallMainVideoSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallMainVideoRecvChannelConnected(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
#endif
				}
				else if(strMedia=="DUALVIDEO")
				{
//A7 add
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
					VIDEO_FORMAT_TYPE formatType=GetVideoFormatType(nWidth,nHeight);
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallSecondVideoSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelConnected(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
#else
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallSecondVideoSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelConnected(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
#endif

				}

#endif
			}

			delete pXDataBuffer;
			pXDataBuffer=NULL;
		}
		else
		{
			break;
		}
	}

	unsigned long nTimestamp=XGetTimestamp();
	//if (m_nLastBFCPCheckTimestamp>nTimestamp)
	//{
	//	m_nLastBFCPCheckTimestamp=nTimestamp;
	//}
	//else if (nTimestamp-m_nLastBFCPCheckTimestamp>=2000)
	//{
	//	m_nLastBFCPCheckTimestamp=nTimestamp;
	//	KeepConnectedSendBfcpInfo();
	//}

	if (m_nLastPacketCheckTimestamp>nTimestamp)
	{
		m_nLastPacketCheckTimestamp=nTimestamp;
	}
	else if (nTimestamp-m_nLastPacketCheckTimestamp>=10000)//10s check once
	{
		m_nLastPacketCheckTimestamp=nTimestamp;
		DoCheckMediaStop();
	}

	//检测注册结果
	if (m_nSIPCheckLastTimestamp > nTimestamp)
	{
		m_nSIPCheckLastTimestamp=nTimestamp;
	}
	else if (nTimestamp - m_nSIPCheckLastTimestamp >= 20 * 1000 ) //20s check
	{
		TimerRegAccountCheck(nTimestamp);
	}

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && ( ( defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1) ) && ( defined(NOT_WIN32_SIP_MSG) ) )
	XSleep(1000);
#endif

}

}

void SIPCallMgr::OnTimer(UINT32 nIDEvent)
{
//WIN32界面终端使用新接口时回调使用ThreadProcMain转调
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && ( ( defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 0) ) || (! defined(NOT_WIN32_SIP_MSG) ) )
	m_bRunning=true;
	ThreadProcMain();
	return;
#endif

	while (true)
	{
		XDataBuffer*pXDataBuffer=GetDataBuffer();
		if (pXDataBuffer!=NULL)
		{
			StrPacket tStrPacket(pXDataBuffer->GetData(),false);
			std::string strCMD="";
			tStrPacket.Get("CMD",strCMD);
			if (strCMD=="CALL")
			{
				std::string strState;
				std::string strCallID="";
				std::string strUserID="";
				std::string strUserName="";
				tStrPacket.Get("STATE",strState);
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("UID",strUserID);
				tStrPacket.Get("UNAME",strUserName);
				if (strState=="IN")
				{
					SIPMediaCall*pSIPMediaCall=NULL;
					{
						XAutoLock l(m_csMapThrSIPMediaCall);
						MapSIPMediaCall::iterator iter=m_MapThrSIPMediaCall.find(strCallID);
						if (iter!=m_MapThrSIPMediaCall.end())
						{
							pSIPMediaCall=iter->second;
						}
					}
					if(pSIPMediaCall!=NULL)
					{
						pSIPMediaCall->OnCallRingIn();
					}
					if(pSIPMediaCall!=NULL)
					{
						m_MapSIPMediaCall[strCallID]=pSIPMediaCall;
					}

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#else
					CallMgr::Instance().OnCallRingIn(strCallID,strUserID,strUserName,"SIP");
#endif

				}
				else if (strState=="OUT")
				{
					CallMgr::Instance().OnCallRingOut(strCallID,strUserID,strUserName);
				}
				else if (strState=="OK")
				{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#else
					CallMgr::Instance().OnCallEstablished(strCallID,strUserID,strUserName);
#endif
				}
				else if (strState=="BYE")
				{
					unsigned int nReason =ModeDisconnectedUnknown;
					tStrPacket.Get("RSN",nReason);

					SIPMediaCall*pSIPMediaCall=NULL;
					{
						//XAutoLock l(m_csMapSIPMediaCall);
						MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
						if (iter!=m_MapSIPMediaCall.end())
						{
							pSIPMediaCall=iter->second;
							m_MapSIPMediaCall.erase(iter);
						}
						{
							XAutoLock l(m_csMapThrSIPMediaCall);
							MapSIPMediaCall::iterator iter=m_MapThrSIPMediaCall.find(strCallID);
							if (iter!=m_MapThrSIPMediaCall.end())
							{
								m_MapThrSIPMediaCall.erase(iter);
							}
						}
					}

					int callNum=0;
					if(pSIPMediaCall) 
					{
						callNum=pSIPMediaCall->GetCallNum();
						pSIPMediaCall->Close();
						pSIPMediaCall->CloseBFCPTransports();
						delete pSIPMediaCall;
						pSIPMediaCall=NULL;
					}
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#else
					CallMgr::Instance().OnCallDestroyed(strCallID,strUserID,strUserName,(IMXEC_CALL_END_REASON)nReason,callNum);
#endif
				}
				else if (strState=="SDPNEG")
				{
					SIPMediaCall*pSIPMediaCall=NULL;
					{
						//XAutoLock l(m_csMapSIPMediaCall);

						MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
						if (iter!=m_MapSIPMediaCall.end())
						{
							pSIPMediaCall=iter->second;
						}
					}
					if(pSIPMediaCall)
					{
						pSIPMediaCall->CloseMedia();
						pSIPMediaCall->OnMediaUpdate();
					}
				}
				else if (strState=="BFCP")
				{
					SIPMediaCall*pSIPMediaCall=NULL;
					{
						//XAutoLock l(m_csMapSIPMediaCall);

						MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
						if (iter!=m_MapSIPMediaCall.end())
						{
							pSIPMediaCall=iter->second;
						}
					}
					if(pSIPMediaCall) 
					{
						pSIPMediaCall->CreateBFCPTransports();
					}
				}
				//retrieveCall test
				else if (strState=="RECALL")
				{
					SIPMediaCall*pSIPMediaCall=NULL;
					{
						//XAutoLock l(m_csMapSIPMediaCall);

						MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
						if (iter!=m_MapSIPMediaCall.end())
						{
							pSIPMediaCall=iter->second;
						}
					}
					if(pSIPMediaCall) 
					{
						//retrieveCall(pSIPMediaCall->m_nCallId);
						//holdCall(pSIPMediaCall->m_nCallId);
						pSIPMediaCall->SetSipCallHold(pSIPMediaCall->m_nCallId);
					}
				}
				else if (strState=="NONS")
				{
					std::string strPacket="";
					std::string strMime ="";
					tStrPacket.Get("INFO",strPacket);
					tStrPacket.Get("MIME",strMime);
					CallMgr::Instance().OnDAP_RecvNonstandardData(strCallID.c_str(),strPacket.c_str(),strPacket.length(),strMime.c_str());
				}
			}
			else if (strCMD=="H239")
			{
				std::string strCallID="";
				std::string strState="";
				std::string strType="";
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("STATE",strState);
				tStrPacket.Get("TYPE",strType);

				SIPMediaCall*pSIPMediaCall=NULL;
				{
					//XAutoLock l(m_csMapSIPMediaCall);
					MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
					if (iter!=m_MapSIPMediaCall.end())
					{
						pSIPMediaCall=iter->second;
					}
				}
				if (strState=="ON")
				{
					if( (strType=="MEDIA") && (pSIPMediaCall!=NULL) )
					{
						pSIPMediaCall->OpenLocalSecondVideo();
					}
					CallMgr::Instance().OnCallSecondVideoStart(strCallID);
				}
				else if (strState=="OFF")
				{
					if( (strType=="MEDIA") && (pSIPMediaCall!=NULL) )
					{
						pSIPMediaCall->OnSecondVideoSendChannelDestroyed();
					}

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

					SetSecondVideoWnd(strCallID.c_str(),NULL);

#endif

					CallMgr::Instance().OnCallSecondVideoStop(strCallID);
				}
			}
			else if (strCMD=="REG")
			{
				std::string strType="";
				std::string strErrorCode="";
				unsigned int nLineID=0;
				tStrPacket.Get("TYPE",strType);
				tStrPacket.Get("LINE",nLineID);
				if(strType=="OK") 
				{
					CallMgr::Instance().OnSIPVirtualLineRegistrationSuccess(nLineID);
				}
				if(strType=="FAILED")
				{
					tStrPacket.Get("ERROR",strErrorCode);
					CallMgr::Instance().OnSIPVirtualLineRegistrationError(nLineID,strErrorCode.c_str());
				}
			}
			//added by jiangdingfeng,增加SIP支持呼叫过程中修改视频源
			else if (strCMD == "LOCAL_MAIN_VIDEO")
			{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#else
				std::string strCallID="";
				std::string strState="";
				std::string strType="";
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("STATE",strState);
				SIPMediaCall*pSIPMediaCall=NULL;
				if (strState=="ON")
				{
					MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
					if (iter!=m_MapSIPMediaCall.end())
					{
						pSIPMediaCall=iter->second;
					}
				}
				if (pSIPMediaCall)
				{
					pSIPMediaCall->OpenLocalMainVideoCapDev();					
				}
#endif
			}
			else if (strCMD == "LOCAL_SECOND_VIDEO")
			{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#else
				std::string strCallID="";
				std::string strState="";
				std::string strType="";
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("STATE",strState);
				SIPMediaCall*pSIPMediaCall=NULL;
				if (strState=="ON")
				{
					MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
					if (iter!=m_MapSIPMediaCall.end())
					{
						pSIPMediaCall=iter->second;
					}
				}
				if (pSIPMediaCall)
				{
					pSIPMediaCall->OpenLocalSecondVideo();					
				}
#endif
			}

			delete pXDataBuffer;
			pXDataBuffer=NULL;
		}
		else
		{
			break;
		}
	}

	unsigned long nTimestamp=XGetTimestamp();
	if (m_nLastBFCPCheckTimestamp>nTimestamp)
	{
		m_nLastBFCPCheckTimestamp=nTimestamp;
	}
	else if (nTimestamp-m_nLastBFCPCheckTimestamp>=30000) //20000
	{
		m_nLastBFCPCheckTimestamp=nTimestamp;
		KeepConnectedSendBfcpInfo();
	}

	if (m_nLastPacketCheckTimestamp>nTimestamp)
	{
		m_nLastPacketCheckTimestamp=nTimestamp;
	}
	else if (nTimestamp-m_nLastPacketCheckTimestamp>=10000)//10s check once
	{
		m_nLastPacketCheckTimestamp=nTimestamp;
		DoCheckMediaStop();
	}

	//检测注册结果
	if (m_nSIPCheckLastTimestamp > nTimestamp)
	{
		m_nSIPCheckLastTimestamp=nTimestamp;
	}
	else if (nTimestamp - m_nSIPCheckLastTimestamp >= 20 * 1000 ) //20s check
	{
		TimerRegAccountCheck(nTimestamp);
	}

	//check empty set call num zero
	if (m_nLastCheckCallNumTS>nTimestamp)
	{
		m_nLastCheckCallNumTS=nTimestamp;
	}
	else if (nTimestamp-m_nLastCheckCallNumTS >= 3000)//3s check once
	{
		m_nLastCheckCallNumTS=nTimestamp;
		CheckMapCallEmpty();
	}
}

void SIPCallMgr::KeepConnectedSendBfcpInfo()
{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPMediaCall);
#endif

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.begin();
	while (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		if(pSIPMediaCall) pSIPMediaCall->SendBFCPHello();
		++iter;
	}
}

void SIPCallMgr::OnCallEstablished (const std::string& strCallID)
{
	std::string strUserID;
	std::string strUserName;

	SIPMediaCall*pSIPMediaCall=NULL;
	{
		XAutoLock l(m_csMapThrSIPMediaCall);

		MapSIPMediaCall::iterator iter=m_MapThrSIPMediaCall.find(strCallID);
		if (iter!=m_MapThrSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
			//pSIPMediaCall->OnCallEstablished();
		}
	}
	if(!pSIPMediaCall) return;
	strUserID=pSIPMediaCall->GetUserID();
	strUserName=pSIPMediaCall->GetUserName();
	unsigned char nUserIDIndex=pSIPMediaCall->GetUserIDIndex();
	if (nUserIDIndex>0)
	{
		char szIndex[128]="";
		sprintf(szIndex,"(%u)",nUserIDIndex);
		strUserID+=szIndex;
	}
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","OK");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

//注意 外面不要再调用该函数
void SIPCallMgr::OnCallDestroyed (const std::string& strCallID,IMXEC_CALL_END_REASON discReason)
{
	std::string strUserID;
	std::string strUserName;
	SIPMediaCall*pSIPMediaCall=NULL;
	{
		XAutoLock l(m_csMapThrSIPMediaCall);

		MapSIPMediaCall::iterator iter=m_MapThrSIPMediaCall.find(strCallID);
		if (iter!=m_MapThrSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
			strUserID=pSIPMediaCall->GetUserID();
			strUserName=pSIPMediaCall->GetUserName();
		}
	}

	if(pSIPMediaCall==NULL)
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif

		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(strCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
			strUserID=pSIPMediaCall->GetUserID();
			strUserName=pSIPMediaCall->GetUserName();
		}
	}

	if(pSIPMediaCall==NULL) return;

	pSIPMediaCall->CloseRingTone();
	unsigned char nUserIDIndex=pSIPMediaCall->GetUserIDIndex();
	if (nUserIDIndex>0)
	{
		char szIndex[128]="";
		sprintf(szIndex,"(%u)",nUserIDIndex);
		strUserID+=szIndex;
	}
	//先挂断协议call
	if(pSIPMediaCall != NULL)
		pSIPMediaCall->HangupCall();
	//发出媒体关闭通知
	unsigned int nReason =discReason;
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","BYE");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);
		tStrPacket.Set("RSN",nReason);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

bool SIPCallMgr::GetVideoLocalCodecTypeFromPalyloadType(VIDEC_CODEC_TYPE &type,int pt)
{
	bool ret=true;
	if(pt==31)              type=VIDEC_CODEC_H261;
	else if(pt==34)         type=VIDEC_CODEC_H263;
	else if(pt==m_H264PT)   type=VIDEC_CODEC_H264;
	else if(pt==m_H264SVCPT)   type=VIDEC_CODEC_H264_SVC;
	else if(pt==m_H263PPT)  type=VIDEC_CODEC_H263P;
	else if(pt==m_H263PPPT) type=VIDEC_CODEC_H263PP;
	else ret=false;
	return ret;
}

//当接收到的对方发来的媒体数据与本端打开的解码CODEC不同时，以PT匹配重新打开解码CODEC
//对端发来的媒体数据是以本端PT进行RTP封包，参数pt为接收的媒体数据的PalyloadType
bool SIPCallMgr::GetDualVideoLocalCodecTypeFromPalyloadType(VIDEC_CODEC_TYPE &type,int pt)
{
	bool ret=true;
	if(pt==31)              type=VIDEC_CODEC_H261;
	else if(pt==34)         type=VIDEC_CODEC_H263;
	else if(pt==m_DualH264PT)   type=VIDEC_CODEC_H264;
	else if(pt==m_DualH264SVCPT)   type=VIDEC_CODEC_H264_SVC;
	else if(pt==m_DualH263PPT)  type=VIDEC_CODEC_H263P;
	else if(pt==m_DualH263PPPT) type=VIDEC_CODEC_H263PP;
	else ret=false;
	return ret;
}
//当接收到的对方发来的媒体数据与本端打开的解码CODEC不同时，以PT匹配重新打开解码CODEC
//对端发来的媒体数据是以本端PT进行RTP封包，参数pt为接收的媒体数据的PalyloadType
bool SIPCallMgr::GetDataLocalCodecTypeFromPalyloadType(DATA_CODEC_ID &type,int pt)
{
	bool ret=true;
	if(pt==m_H224PT)              type=DATA_CODEC_H224;
	else ret=false;
	return ret;
}

bool SIPCallMgr::GetAudioLocalCodecTypeFromPalyloadType(X_AUDIO_CODEC_ID &type,int pt)
{
	bool ret=true;
	if(pt==0)       type=X_AUDIO_CODEC_G711U;
	else if(pt==3)  type=X_AUDIO_CODEC_GSM;
	else if(pt==8)  type=X_AUDIO_CODEC_G711A;
	else if(pt==9)  type=X_AUDIO_CODEC_G722_64;
	else if(pt==18) type=X_AUDIO_CODEC_G729;
	else if(pt==m_G7221_24k_PT)  type=X_AUDIO_CODEC_G7221_7_24;
	else if(pt==m_G7221_32k_PT)  type=X_AUDIO_CODEC_G7221_7_32;
	else if(pt==m_G7221C_24k_PT) type=X_AUDIO_CODEC_G7221_14_24;
	else if(pt==m_G7221C_32k_PT) type=X_AUDIO_CODEC_G7221_14_32;
	else if(pt==m_G7221C_48k_PT) type=X_AUDIO_CODEC_G7221_14_48;
	else ret=false;
	return ret;
}

//set bfcp local info
void SIPCallMgr::SetBFCPConfId(int confId)
{
	m_nRequestConfId=confId;
}

void SIPCallMgr::SetBFCPFloorId(int floorId)
{
	m_nRequestFloorId=floorId;
}

void SIPCallMgr::SetBFCPUserId(int userId)
{
	m_nRequestUserId=userId;
}

void SIPCallMgr::SetBFCPRequestPriority(unsigned short priority)
{
	m_nRequestPriority=priority;
}

void SIPCallMgr::SetBFCPFloorControlType(EBFCPFloorControlType type)
{
	m_nRequestFloorControlType=type;
}

void SIPCallMgr::SetBFCPSetupType(EBFCPSetupType type)
{
	m_nRequestSetupType=type;
}

void SIPCallMgr::SetBFCPTransportType(EBFCPTransportType type)
{
	m_nRequestTransportType=type;
}

//set sip bfcp call local info
void SIPCallMgr::SetSIPCallBFCPLocalInfo(SIPMediaCall* pSIPMediaCall,int nDir)
{
	pSIPMediaCall->SetBFCPConfId(m_nRequestConfId);
	pSIPMediaCall->SetBFCPUserId(m_nRequestUserId);
	pSIPMediaCall->SetBFCPFloorId(m_nRequestFloorId);
	pSIPMediaCall->SetBFCPFloorControlType(m_nRequestFloorControlType);
	pSIPMediaCall->SetBFCPSetupType(m_nRequestSetupType);
	pSIPMediaCall->SetBFCPTransportType(m_nRequestTransportType);
	if(nDir==IMXEC_CALL_DIR_IN)
	{
		pSIPMediaCall->SetBFCPConfId(m_nRequestConfIdCalled);
		pSIPMediaCall->SetBFCPUserId(m_nRequestUserIdCalled);
		pSIPMediaCall->SetBFCPFloorId(m_nRequestFloorIdCalled);
		pSIPMediaCall->SetBFCPFloorControlType(m_nRequestFloorControlTypeCalled);
		pSIPMediaCall->SetBFCPSetupType(m_nRequestSetupTypeCalled);
	}
}

//set sip bfcp sdp local info
void SIPCallMgr::SetSDPBFCPLocalInfo(SIPCallCapability & sdpCap,int nDir)
{
	sdpCap.SetBFCPConfId(m_nRequestConfId);
	sdpCap.SetBFCPUserId(m_nRequestUserId);
	sdpCap.SetBFCPFloorId(m_nRequestFloorId);
	sdpCap.SetBFCPFloorControlType(m_nRequestFloorControlType);
	sdpCap.SetBFCPSetupType(m_nRequestSetupType);
	sdpCap.SetBFCPTransportType(m_nRequestTransportType);
	if(nDir==IMXEC_CALL_DIR_IN)
	{
		sdpCap.SetBFCPConfId(m_nRequestConfIdCalled);
		sdpCap.SetBFCPUserId(m_nRequestUserIdCalled);
		sdpCap.SetBFCPFloorId(m_nRequestFloorIdCalled);
		sdpCap.SetBFCPFloorControlType(m_nRequestFloorControlTypeCalled);
		sdpCap.SetBFCPSetupType(m_nRequestSetupTypeCalled);
	}
}

//set sip bfcp net local info
void SIPCallMgr::SetSIPBFCPLocalInfo(SIP_BFCP * pSipBfcp,int nDir)
{
	pSipBfcp->SetBFCPConfId(m_nRequestConfId);
	pSipBfcp->SetBFCPUserId(m_nRequestUserId);
	pSipBfcp->SetBFCPFloorId(m_nRequestFloorId);
	pSipBfcp->SetBFCPFloorControlType(m_nRequestFloorControlType);
	pSipBfcp->SetBFCPSetupType(m_nRequestSetupType);
	pSipBfcp->SetBFCPTransportType(m_nRequestTransportType);
	if(nDir==IMXEC_CALL_DIR_IN)
	{
		pSipBfcp->SetBFCPConfId(m_nRequestConfIdCalled);
		pSipBfcp->SetBFCPUserId(m_nRequestUserIdCalled);
		pSipBfcp->SetBFCPFloorId(m_nRequestFloorIdCalled);
		pSipBfcp->SetBFCPFloorControlType(m_nRequestFloorControlTypeCalled);
		pSipBfcp->SetBFCPSetupType(m_nRequestSetupTypeCalled);
	}
}

void SIPCallMgr::SipSetting()
{
	int nECTail=200;
	int nExpires=300;
	bool nImsEnabled=true;
	bool nImsIPSecHeaders=true;
	bool nImsIPSecTransport=true;
	int nListenPort=m_nSIPListenPort;
	int nLogLevel=0;
	bool nUseTCP=false;
	bool nUseUDP=true;
	bool nVADEnabled=true;
	char strLogFileName[256]={0};
	char strUserAgentName[256]={0};
	CString strPath = _T("");
	GetModuleFileName(NULL,strPath.GetBufferSetLength(512 + 1),512);
	int n = strPath.ReverseFind('\\');
	strPath = strPath.Left(n);
	strPath += _T("\\siptele.ini");
	m_nOutboundProxyCount=0;
	m_nForcelrParam=0;
	m_nUseRegReqLine=0;
	m_nUseInvReqLine=0;
	m_strRegReqLineAddr="";
	m_strInvReqLineAddr="";

	std::string strFormat="";
	if(GetSIPTransportType() ==IMXEC_SIP::TRANS_TCP)
		strFormat=";transport=tcp";
	if(GetSIPTransportType() ==IMXEC_SIP::TRANS_TLS)
		strFormat=";transport=tls";

	for(int j=0;j<8;j++)
	{
		m_strOutboundProxy[j][0]='\0';
	}
	char strLineAddr1[256]={0};
	char strLineAddr2[256]={0};	
	char proxyName[64]={0};
	int actualCount=0;
	char strDateTime[256]={0};
	m_nOutboundProxyCount=GetPrivateProfileInt("OUTBOUND","OutboundCount",0,strPath);
	m_nForcelrParam=GetPrivateProfileInt("OUTBOUND","ForcelrParam",0,strPath);
	GetPrivateProfileString("OUTBOUND", "CentralNumber", "",m_strConfCentralNum,255, strPath );
	m_nUseRegReqLine=GetPrivateProfileInt("OUTBOUND","UseRegLineURI",0,strPath);
	m_nUseInvReqLine=GetPrivateProfileInt("OUTBOUND","UseInvLineURI",0,strPath);
	GetPrivateProfileString("OUTBOUND", "RegLineADDR", "",strLineAddr1,255, strPath );
	GetPrivateProfileString("OUTBOUND", "InvLineADDR", "",strLineAddr2,255, strPath );

	if(m_nOutboundProxyCount>8) m_nOutboundProxyCount=8;
	for(int i=0;i<m_nOutboundProxyCount;i++)
	{
		strcpy(m_strOutboundProxy[i],"sip:");
		sprintf(proxyName,"OutboundProxy%d",i+1);
		GetPrivateProfileString("OUTBOUND", proxyName, "",m_strOutboundProxy[i]+4,255, strPath );
		strcat(m_strOutboundProxy[i],strFormat.c_str());
	}

	m_strRegReqLineAddr=strLineAddr1;
	m_strInvReqLineAddr=strLineAddr2;

	nUseTCP=GetPrivateProfileInt("SETTING","UseTCP",0,strPath);
	nUseUDP=GetPrivateProfileInt("SETTING","UseUDP",1,strPath);
	nExpires=GetPrivateProfileInt("SETTING","Expires",nExpires,strPath);
	nECTail=GetPrivateProfileInt("SETTING","ECTail",nECTail,strPath);
	nListenPort=GetPrivateProfileInt("SETTING","ListenPort",m_nSIPListenPort,strPath);
	nLogLevel=GetPrivateProfileInt("SETTING","LogLevel",0,strPath);
	GetPrivateProfileString("SETTING", "LogFileName", "AvconSIP.log",strLogFileName,254, strPath );
	if( strnlen(strLogFileName,255)<=0 )
	{
		strcpy(strLogFileName,"AvconSIP.log");
	}
	GetPrivateProfileString("SETTING", "AgentName", m_strUserAgentName,strUserAgentName,254, strPath );
	if( strnlen(strUserAgentName,255)<=0 )
	{
		strcpy(strUserAgentName,m_strUserAgentName);
	}

	GetPrivateProfileString("SETTING", "DateTime", "",strDateTime,255, strPath );
	m_nUseDualVideoToken=GetPrivateProfileInt("SETTING","DualVideoToken",1,strPath);
	m_nSDPOrignIPConverse=GetPrivateProfileInt("SETTING","SDPOIPConverse",0,strPath);
	m_nSendSubscribre=GetPrivateProfileInt("SETTING","SendSubscribre",0,strPath);
	m_nSendPicFastUpdate=GetPrivateProfileInt("SETTING","SendPicFastUpdate",1,strPath);
	m_nSendMultipartSDP=GetPrivateProfileInt("SETTING","SendMultipartSDP",0,strPath);
	m_nUseDualVideoTokenType=GetPrivateProfileInt("SETTING","DualVideoTokenType",0,strPath);
	m_nProxySendRtp=GetPrivateProfileInt("SETTING","ProxySendRtp",1,strPath); //notice use ? 
	m_nProxyUseFEC=GetPrivateProfileInt("SETTING","ProxyUseFEC",0,strPath);   //notice use ? 
	m_nRTPUseType=GetPrivateProfileInt("SETTING","RTPUseType",1,strPath);   //notice use ? 
	m_nRequestTransportType=( EBFCPTransportType) GetPrivateProfileInt("SETTING","BFCPTransType",1,strPath); //udp 1 ,tcp 2

	if(m_nRequestTransportType <0 || m_nRequestTransportType >2) m_nRequestTransportType =e_BFCP_Transport_UDP;

	m_strDateTime=strDateTime;

	if(m_nSIPLogLevel >0) nLogLevel=m_nSIPLogLevel;
	if(m_nRegExpires >0) nExpires=m_nRegExpires;
	if(m_strSIPLogFile[0]!='\0') strcpy(strLogFileName,m_strSIPLogFile);

	//初始化SipConfig
	memset(&m_sipConfig,0,sizeof(SipConfigStruct));
	m_sipConfig.ECTail=nECTail;
	m_sipConfig.expires=nExpires;
	m_sipConfig.imsEnabled=nImsEnabled;
	m_sipConfig.imsIPSecHeaders=nImsIPSecHeaders;
	m_sipConfig.imsIPSecTransport=nImsIPSecTransport;
	m_sipConfig.listenPort=nListenPort;
	m_sipConfig.logLevel=nLogLevel;
	m_sipConfig.noTCP= !nUseTCP;
	m_sipConfig.noUDP= !nUseUDP;
	m_sipConfig.VADEnabled=nVADEnabled;
	strcpy(m_sipConfig.logFileName,strLogFileName);
	strcpy(m_strUserAgentName,strUserAgentName);
	m_sipConfig.useTLS=GetPrivateProfileInt("TLS","useTLS",0,strPath);
	if(m_sipConfig.useTLS >0 )
	{
		m_sipConfig.method=GetPrivateProfileInt("TLS","tls_Method",0,strPath);
		m_sipConfig.verify_server=GetPrivateProfileInt("TLS","verify_server",0,strPath);
		m_sipConfig.verify_client=GetPrivateProfileInt("TLS","verify_client",0,strPath);
		m_sipConfig.require_client_cert=GetPrivateProfileInt("TLS","require_client_cert",0,strPath);
		m_sipConfig.timeout_sec=GetPrivateProfileInt("TLS","tls_timeout_sec",0,strPath);
		m_sipConfig.timeout_msec=GetPrivateProfileInt("TLS","tls_timeout_msec",0,strPath);
		m_sipConfig.qos_type=GetPrivateProfileInt("TLS","tls_qos_type",0,strPath);
		m_sipConfig.qos_ignore_error=GetPrivateProfileInt("TLS","tls_qos_ignore_error",1,strPath);
		GetPrivateProfileString("TLS", "ca_list_file", "",m_sipConfig.ca_list_file,255, strPath );
		GetPrivateProfileString("TLS", "cert_file", "",m_sipConfig.cert_file,255, strPath );
		GetPrivateProfileString("TLS", "privkey_file", "",m_sipConfig.privkey_file,255, strPath );
		GetPrivateProfileString("TLS", "password", "",m_sipConfig.password,255, strPath );
		//GetPrivateProfileString("TLS", "ciphers", "",m_sipConfig.ciphers,255, strPath );
		m_sipConfig.ciphers_num=GetPrivateProfileInt("TLS","ciphers_num",0,strPath);
		m_sipConfig.ciphers=(pj_ssl_cipher)GetPrivateProfileInt("TLS","ciphers",0,strPath);
		//GetPrivateProfileString("TLS", "server_name", "",m_sipConfig.server_name,255, strPath );
	}
	//notice use interface set
	SetSIPTransportType(m_nSIPTransType);
}

/******************************************************************************
* SetSIPTransportType
*描述：设置SIP信令传输类型
*输入：type			-类型
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetSIPTransportType(IMXEC_SIP::TRANS_TYPE type)
{
	m_nSIPTransType=type;
	switch(type)
	{
	case IMXEC_SIP::TRANS_UDP:
		m_sipConfig.noTCP = true;
		m_sipConfig.noUDP = false;
		m_sipConfig.useTLS = PJ_FALSE;
		break;
	case IMXEC_SIP::TRANS_TCP:
		m_sipConfig.noTCP = false;
		m_sipConfig.noUDP = true;
		m_sipConfig.useTLS = PJ_FALSE;
		break;
	case IMXEC_SIP::TRANS_TLS:
		m_sipConfig.noTCP = false;
		m_sipConfig.noUDP = true;
		m_sipConfig.useTLS = PJ_TRUE;
		break;
	default:
		m_sipConfig.noTCP = true;
		m_sipConfig.noUDP = false;
		m_sipConfig.useTLS = PJ_FALSE;
		break;
	}
	return 0;
}

//use to SIPUser::Open, 0 udp 1 tcp 2 tls 
int SIPCallMgr::GetSIPTransportType()
{
	int type=IMXEC_SIP::TRANS_UDP;
	if( (m_sipConfig.noTCP == true) && (m_sipConfig.noUDP == false) )
		type=IMXEC_SIP::TRANS_UDP;
	if( (m_sipConfig.noTCP == false) && (m_sipConfig.noUDP == true) )
		type=IMXEC_SIP::TRANS_TCP;
	if(m_sipConfig.useTLS == PJ_TRUE)
		type=IMXEC_SIP::TRANS_TLS;
	return type;
}
//参考 pjsua_acc_find_for_incoming
SIPUser * SIPCallMgr::GetSIPUserFromSipUri(pjsip_uri *uri,pjsip_transport * transport)
{
	pjsip_sip_uri *sip_uri;
	char userid[128]={0};
	char siphost[256]={0};
	std::string strDomain="";
	pjsip_transport_type_e type=PJSIP_TRANSPORT_UNSPECIFIED;
	IMXEC_SIP::TRANS_TYPE nTransType=IMXEC_SIP::TRANS_UDP;
	int nFind=0;
	// Just return default account if To URI is not SIP:
	if (!PJSIP_URI_SCHEME_IS_SIP(uri) && 
		!PJSIP_URI_SCHEME_IS_SIPS(uri)) 
	{
		return NULL;
	}
	sip_uri = (pjsip_sip_uri*)pjsip_uri_get_uri(uri);
	strncpy(userid,sip_uri->user.ptr,sip_uri->user.slen);
	strncpy(siphost,sip_uri->host.ptr,sip_uri->host.slen);
	if(transport)
	{
		pj_str_t type_name=pj_str(transport->type_name);
		type = pjsip_transport_get_type_from_name(&type_name);
	}
	if (type == PJSIP_TRANSPORT_UNSPECIFIED) type = PJSIP_TRANSPORT_UDP;
	switch(type)
	{
	case PJSIP_TRANSPORT_UDP:
		nTransType=IMXEC_SIP::TRANS_UDP;
		break;
	case PJSIP_TRANSPORT_TCP:
		nTransType=IMXEC_SIP::TRANS_TCP;
		break;
	case PJSIP_TRANSPORT_TLS:
		nTransType=IMXEC_SIP::TRANS_TLS;
		break;
	}
	//find sipuser from incoming data 
	SIPUser * pReturnSIPUser=NULL;
	{
		XAutoLock l(m_csMapThrSIPUser);
		MapSIPUser::iterator iter=m_MapThrSIPUser.begin();
		while (iter!=m_MapThrSIPUser.end())
		{
			SIPUser * pSIPUser=iter->second;
			++iter;
			//if find not siphost   ,the contact have not userid
			//strDomain=pSIPUser->m_strDomainRealm;
			//if(strDomain == "*" || strDomain.size()==0 )
			//	strDomain=pSIPUser->m_strSIPProxy;
			//nFind=strDomain.find(siphost);
			//if( (nTransType==this->m_nSIPTransType) && (pSIPUser->m_strUserID==userid) && (nFind>=0) )
			//{
			//	pReturnSIPUser=pSIPUser;
			//	break;	
			//}

			//notice ,not find siphost 
			if( (nTransType==this->m_nSIPTransType) && (pSIPUser->m_strUserID==userid)   )
			{
				pReturnSIPUser=pSIPUser;
				break;	
			}

		}
	}
	return pReturnSIPUser;
}

//wait for unregistrations to complete
void SIPCallMgr::WaitSIPUserDelete(SIPUser *pSIPUser)
{
	int nMaxWait = 50;
	if(pSIPUser)
	{
		if(!pSIPUser->m_bValid) sip_busy_sleep(nMaxWait);
		delete pSIPUser;
		pSIPUser=NULL;
	}
}

//wait for unregistrations to complete
void SIPCallMgr::WaitSIPUserUnRegComplete() 
{
	//notice prevent add lock to lock
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPUser);
#endif
		while(true)
		{
			SIPUser *pSIPUser=NULL;
			MapSIPUser::iterator iter=m_MapSIPUser.begin();
			if(iter!=m_MapSIPUser.end())
			{
				pSIPUser=iter->second;
				m_MapSIPUser.erase(iter);
			}
			else break;

			if(pSIPUser)
			{
				WaitSIPUserDelete(pSIPUser);
				pSIPUser=NULL;
			}
		}
	}

	{
		XAutoLock l(m_csMapThrSIPUser);
		MapSIPUser::iterator iter=m_MapThrSIPUser.begin();
		while (iter!=m_MapThrSIPUser.end())
		{
			m_MapThrSIPUser.erase(iter);
			iter=m_MapThrSIPUser.begin();
		}
	}
}

/******************************************************************************
* SetSTUNServerIPPort
*描述：设置STUN服务器地址,也可设为域名地址
*输入：	cszStunIPPort		-STUN服务器地址 形如IP:Port
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetSTUNServerIPPort(const char*cszStunIPPort)
{
	m_strStunSrv=cszStunIPPort;
	return 0;
}

/******************************************************************************
* SetTURNServerIPPort
*描述：设置TURN服务器地址,也可设为域名地址
*输入：	cszTurnIPPort		-TURN服务器地址 形如IP:Port
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetTURNServerIPPort(const char*cszTurnIPPort)
{
	m_strTurnSrv=cszTurnIPPort;
	return 0;
}

/******************************************************************************
* SetDNSServerIP
*描述：设置DNS服务器地址
*输入：	cszDNSIP		-设置DNS服务器地址 形如IP
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetDNSServerIP(const char*cszDNSIP)
{
	m_strDNSSrv=cszDNSIP;
	return 0;
}

/******************************************************************************
* SetDNSServerIP2
*描述：设置DNS服务器地址2
*输入：	cszDNSIP2		-设置DNS服务器地址 形如IP
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetDNSServerIP2(const char*cszDNSIP2)
{
	m_strDNSSrv2=cszDNSIP2;
	return 0;
}

/******************************************************************************
* SetTURNLoginAuth
*描述：设置登录TURN服务器账户
*输入：	cszTurnUser		-TURN服务器账户名
        cszTurnPWD		-TURN服务器账户密码
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetTURNLoginAuth(const char*cszTurnUser,const char*cszTurnPWD)
{
	m_strTurnUser=cszTurnUser;
	m_strTurnPWD=cszTurnPWD;
	return 0;
}

/******************************************************************************
* SetICEEnable
*描述：设置ICE应用状态
*输入：	bEnable		-ICE应用状态 true使用，false不使用，默认false
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetICEEnable(bool bEnable)
{
	m_bICEEnable=bEnable;
	return 0;
}

/******************************************************************************
* SetICETransportCount
*描述：设置创建的每种媒体流ICE通道数
*输入：	nCount		-每种媒体流ICE通道数(即如只有RTP则设为1，如还需要有RTCP则设为2)
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetICETransportCount(int nCount)
{
	m_nICETransportCount=nCount;
	return 0;
}

/******************************************************************************
* SetTurnTransportType
*描述：设置TURN传输通道类型
*输入：	nType		-TURN传输通道类型 17 udp, 6 tcp, 255 tls, 具体可参看pj_turn_tp_type
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::SetTurnTransportType(int nType)
{
	m_nTurnTPType=nType;
	return 0;
}

/******************************************************************************
* ICEInstanceInit
*描述：初始化ICE应用，注意在SipInit后调用或放于SipInit中最后调用
*输入：	无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCallMgr::ICEInstanceInit()
{
	int status=-1;
	if (m_nICEInit !=0) return 0;
	if(m_bICEEnable==false) return 0;
	int transCount=m_nICETransportCount;
	int nMaxHost=-1;
	pj_str_t strStunSrv;
	pj_str_t strTurnSrv;
	pj_str_t strDNSSrv;
	pj_str_t strDNSSrv2;
	pj_str_t turnUserName;
	pj_str_t turnUserPWD;
	pj_bool_t   bRegular=PJ_FALSE;
	pj_caching_pool	* pICECachPool=NULL;
	char createName[128]={0};
	pj_str_t strDNS[2];
	int nDNSCount=0;

	pICECachPool=GetSipCachingPool();
	if(pICECachPool==NULL) return -1;
	m_pICEPool=GetSipPool();
	if(m_pICEPool==NULL) return -1;
	sprintf(createName,"ICE%p",this);

	strStunSrv.ptr=(char *)m_strStunSrv.c_str();
	strStunSrv.slen=m_strStunSrv.length();
	strTurnSrv.ptr=(char *)m_strTurnSrv.c_str();
	strTurnSrv.slen=m_strTurnSrv.length();

	strDNSSrv.ptr=(char *)m_strDNSSrv.c_str();
	strDNSSrv.slen=m_strDNSSrv.length();
	strDNSSrv2.ptr=(char *)m_strDNSSrv2.c_str();
	strDNSSrv2.slen=m_strDNSSrv2.length();

	turnUserName.ptr=(char *)m_strTurnUser.c_str();
	turnUserName.slen=m_strTurnUser.length();
	turnUserPWD.ptr=(char *)m_strTurnPWD.c_str();
	turnUserPWD.slen=m_strTurnPWD.length();

	pj_ice_strans_cfg_default(&m_ICEConfig);
	m_ICEConfig.stun_cfg.res_cache_msec=1000;
	m_ICEConfig.stun_cfg.rto_msec=100;
	m_ICEConfig.af = pj_AF_INET();
	m_ICEConfig.stun_cfg.pf =&(pICECachPool->factory);

	status=pj_timer_heap_create(m_pICEPool, 100, &m_ICEConfig.stun_cfg.timer_heap);
	if (status!=PJ_SUCCESS)
	{
		return status;
	}
	status=pj_ioqueue_create(m_pICEPool, m_nMaxICESocketCount, &m_ICEConfig.stun_cfg.ioqueue);
	if (status!=PJ_SUCCESS)
	{
		return status;
	}
	status=pj_thread_create(m_pICEPool, createName, &ICEWorkerThread, this,0, 0, &m_pICEThread);
	if (status!=PJ_SUCCESS)
	{
		ICEInstanceExit();
		return status;
	}
	nDNSCount=0;
	if(strDNSSrv.slen>0)
	{
		strDNS[nDNSCount]=strDNSSrv;
		nDNSCount++;
	}
	if(strDNSSrv2.slen>0)
	{
		strDNS[nDNSCount]=strDNSSrv2;
		nDNSCount++;
	}
	if (nDNSCount>0)
	{
		status=pj_dns_resolver_create(&(pICECachPool->factory),"resolver",0,
			m_ICEConfig.stun_cfg.timer_heap,m_ICEConfig.stun_cfg.ioqueue,&m_ICEConfig.resolver) ;
		if (status!=PJ_SUCCESS)
		{ 
			ICEInstanceExit();
			return status;
		}
		status=pj_dns_resolver_set_ns(m_ICEConfig.resolver, nDNSCount,strDNS, NULL) ;
		if (status!=PJ_SUCCESS)
		{ 
			ICEInstanceExit();
			return status;
		}
	}

	if (nMaxHost != -1)
		m_ICEConfig.stun.max_host_cands = nMaxHost;
	if (bRegular)
		m_ICEConfig.opt.aggressive = PJ_FALSE;
	else
		m_ICEConfig.opt.aggressive = PJ_TRUE;

	if (strStunSrv.slen)
	{
		char *pos;
		if ((pos=pj_strchr(&strStunSrv, ':')) != NULL)
		{
			m_ICEConfig.stun.server.ptr = strStunSrv.ptr;
			m_ICEConfig.stun.server.slen = (pos - strStunSrv.ptr);

			m_ICEConfig.stun.port = (pj_uint16_t)atoi(pos+1);
		}
		else
		{
			m_ICEConfig.stun.server = strStunSrv;
			m_ICEConfig.stun.port = PJ_STUN_PORT;
		}
		m_ICEConfig.stun.cfg.ka_interval = KA_INTERVAL;
	}
	if (strTurnSrv.slen)
	{
		char *pos;
		if ((pos=pj_strchr(&strTurnSrv, ':')) != NULL) 
		{
			m_ICEConfig.turn.server.ptr = strTurnSrv.ptr;
			m_ICEConfig.turn.server.slen = (pos - strTurnSrv.ptr);

			m_ICEConfig.turn.port = (pj_uint16_t)atoi(pos+1);
		}
		else
		{
			m_ICEConfig.turn.server = strTurnSrv;
			m_ICEConfig.turn.port = PJ_STUN_PORT+1;
		}
		m_ICEConfig.turn.auth_cred.type = PJ_STUN_AUTH_CRED_STATIC;
		m_ICEConfig.turn.auth_cred.data.static_cred.username = turnUserName;
		m_ICEConfig.turn.auth_cred.data.static_cred.data_type = PJ_STUN_PASSWD_PLAIN;
		m_ICEConfig.turn.auth_cred.data.static_cred.data = turnUserPWD;
		m_ICEConfig.turn.conn_type =(pj_turn_tp_type) m_nTurnTPType;
		m_ICEConfig.turn.alloc_param.ka_interval = KA_INTERVAL;
	}
	//resolve_stun_server
	m_nICEInit=1;
	return 0;
}

int SIPCallMgr::ICEInstanceExit()
{
	if (m_nICEInit==0) return 0;
	m_nICEInit=0;
	if (m_pICEThread) pj_thread_sleep(500);
	m_bICEThreadQuitFlag = PJ_TRUE;
	if (m_pICEThread)
	{
		pj_thread_join(m_pICEThread);
		pj_thread_destroy(m_pICEThread);
		m_pICEThread=NULL;
	}
	//if (m_ICEConfig.stun_cfg.ioqueue)
	//	pj_ioqueue_destroy(m_ICEConfig.stun_cfg.ioqueue);
	//if (m_ICEConfig.stun_cfg.timer_heap)
	//	pj_timer_heap_destroy(m_ICEConfig.stun_cfg.timer_heap);
	return 0;
}

int SIPCallMgr::ICEWorkerThread(void *unused) 
{
	sip_register_thread();
	while (!((SIPCallMgr*)unused)->m_bICEThreadQuitFlag) 
	{
		((SIPCallMgr*)unused)->ICEHandleEvents(500, NULL);
	}
	return 0;
}

int SIPCallMgr::ICEHandleEvents(unsigned max_msec, unsigned *p_count)
{
	enum { MAX_NET_EVENTS = 1 };
	pj_time_val max_timeout = {0, 0};
	pj_time_val timeout = { 0, 0};
	unsigned count = 0, net_event_count = 0;
	int c;
	max_timeout.msec = max_msec;
	timeout.sec = timeout.msec = 0;
	//notice ice test wait debug
	timeout.msec = 10;
	c = pj_timer_heap_poll( m_ICEConfig.stun_cfg.timer_heap, &timeout );
	if (c > 0) count += c;
	pj_assert(timeout.sec >= 0 && timeout.msec >= 0);
	if (timeout.msec >= 1000) timeout.msec = 999;
	if (PJ_TIME_VAL_GT(timeout, max_timeout)) timeout = max_timeout;
	do {
		c = pj_ioqueue_poll( m_ICEConfig.stun_cfg.ioqueue, &timeout);
		if (c < 0)
		{
			pj_status_t err = pj_get_netos_error();
			pj_thread_sleep(PJ_TIME_VAL_MSEC(timeout));
			if (p_count)
				*p_count = count;
			return err;
		}
		else if (c == 0)
		{
			break;
		}
		else
		{
			net_event_count += c;
			timeout.sec = timeout.msec = 0;
		}
	} while (c > 0 && net_event_count < MAX_NET_EVENTS);
	count += net_event_count;
	if (p_count) *p_count = count;
	return PJ_SUCCESS;
}

pj_ice_strans_cfg * SIPCallMgr::GetICEInstanceConfig()
{
	if(m_nICEInit!=0) return &m_ICEConfig;
	else return NULL;
}

int SIPCallMgr::CreateMediaEndpoint()
{
	pj_status_t	status;
	pj_caching_pool	* pCachingPool=GetSipCachingPool();
	pjsip_endpoint * pSipEndPoint=GetSipEndpoint();
	if( (pCachingPool==NULL) || (pSipEndPoint==NULL) || (m_pMediaEndpoint!=NULL) ) return -1;
	pj_pool_factory *pf=&(pCachingPool->factory);
	pj_ioqueue_t *ioqueue=pjsip_endpt_get_ioqueue(pSipEndPoint);
	status = pjmedia_endpt_create(pf,ioqueue,0, &m_pMediaEndpoint);
	if(status != PJ_SUCCESS) 
	{
		m_pMediaEndpoint=NULL;
	}
	return status;
}

void SIPCallMgr::CloseMediaEndpoint()
{
	if (m_pMediaEndpoint) 
	{
		pjmedia_endpt_destroy(m_pMediaEndpoint);
		m_pMediaEndpoint=NULL;
	}
}

pjmedia_endpt * SIPCallMgr::GetMediaEndpoint()
{
	return m_pMediaEndpoint;
}

int SIPCallMgr::CreateICEMediaTrans(int nCount)
{
	int ret=-1;
	if(m_nICEInit==0) return ret;
	pjmedia_transport *pMediaTransport=NULL;
	pjmedia_ice_cb icecb;
	icecb.on_ice_complete=OnPJSIPICENegComplete;
	for(int i=0;i<nCount;i++)
	{
		ret=pjmedia_ice_create3(m_pMediaEndpoint,"Tran%p",m_nICETransportCount,&m_ICEConfig,&icecb,0,this,&pMediaTransport);
		if(ret==0)
		{
			XAutoLock l(m_csMapSIPMediaICETrans);
			m_MapSIPMediaICETrans.push_back(pMediaTransport);
		}
	}
	return ret;
}

pjmedia_transport* SIPCallMgr::GetICEMediaTrans()
{
	pjmedia_transport *pMediaTransport=NULL;
	GetICEMediaTrans(&pMediaTransport);
	return pMediaTransport;
}

int SIPCallMgr::GetICEMediaTrans(pjmedia_transport** pIceTrans)
{
	int ret=-1;
	int count=0;
	pjmedia_transport *pTrans=NULL;
	pjmedia_transport *pTransSel=NULL;
	int enableCount=0;
	int i=0;
	{
		XAutoLock l(m_csMapSIPMediaICETrans);
		count=m_MapSIPMediaICETrans.size();
		enableCount=count;
		if(count>0)
		{
			MapSIPICE::iterator iter= m_MapSIPMediaICETrans.begin();
			pTrans=* iter;
			iter=m_MapSIPMediaICETrans.erase(iter);		
			if(pTrans!=NULL) 
			{
				pTransSel=pTrans;
			}
		}
		//if(count>0)
		//{
		//	pTrans=m_MapSIPMediaICETrans.at(count-1);
		//	m_MapSIPMediaICETrans.pop_back();//后取
		//	if(pTrans!=NULL) 
		//	{
		//		pTransSel=pTrans;
		//	}
		//}
	}
	if(pTransSel!=NULL)
	{					
		* pIceTrans=pTransSel;
		enableCount--;
		ret=0;
	}
	if(enableCount<1) ret=CreateICEMediaTrans(3*m_nICETransportCount);
	return ret;
}

int SIPCallMgr::DestroyICEMediaTrans(pjmedia_transport* pMediaTransport)
{
	int ret=-1;
	int i=0;
	if (pMediaTransport) 
	{
		ret=pjmedia_transport_close(pMediaTransport);
		{
			XAutoLock l(m_csMapSIPMediaICETrans);
			for(MapSIPICE::iterator iter= m_MapSIPMediaICETrans.begin();iter!=m_MapSIPMediaICETrans.end();iter++)
			{		
				if(* iter==pMediaTransport) 
				{
					iter=m_MapSIPMediaICETrans.erase(iter);
				}
				if(iter == m_MapSIPMediaICETrans.end()) break;
			}
		}
		pMediaTransport = NULL;
	}
	return ret;
}

int SIPCallMgr::DestroyAllICEMediaTrans()
{
	int ret=-1;
	int count=0;
	int i=0;

	while(true)
	{
		pjmedia_transport *pMediaTransport=NULL;
		{
			XAutoLock l(m_csMapSIPMediaICETrans);
			count=m_MapSIPMediaICETrans.size();
			if(i>=count) break;
			pMediaTransport=m_MapSIPMediaICETrans.at(i);
			i++;
		}
		if(pMediaTransport!=NULL) 
		{
			ret=pjmedia_transport_close(pMediaTransport);
			pMediaTransport=NULL;
		}
		else 
		{
			break;
		}
	}
	{
		XAutoLock l(m_csMapSIPMediaICETrans);
		m_MapSIPMediaICETrans.clear();
	}

	return ret;
}

void SIPCallMgr::SetNatDetectCallback() 
{
	if(m_nICEInit==0) return ;
	pj_sockaddr_in addr;
	pj_str_t serv=m_ICEConfig.stun.server;
	pj_sockaddr_in_init(&addr,&serv,m_ICEConfig.stun.port);
	pj_stun_detect_nat_type(&addr,&m_ICEConfig.stun_cfg,this,stun_nat_detect_cb);
}

void SIPCallMgr::OnICENegotiationComplete(pjmedia_transport *tp,pj_ice_strans_op op,pj_status_t status)
{
	if(op==PJ_ICE_STRANS_OP_NEGOTIATION && status==PJ_SUCCESS)
	{
		int nMediaType=0;
		XAutoLock l(m_csMapThrSIPMediaCall);
		MapSIPMediaCall::iterator iter=m_MapThrSIPMediaCall.begin();
		while (iter!=m_MapThrSIPMediaCall.end())
		{
			SIPMediaCall*pSIPMediaCall=iter->second;
			nMediaType=pSIPMediaCall->CompareMediaTransport(tp);
			if(nMediaType>0)
			{
				pSIPMediaCall->OnICETransCheckComplete(tp,nMediaType);
				break;
			}
			++iter;
		}
	}
}

void SIPCallMgr::DoCheckMediaStop(void)
{
	if(CallMgr::Instance().m_nUseCheckMedia <=0 ) return;

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPUser);
#endif

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.begin();
	while (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		//if (pSIPMediaCall->m_bCallState && pSIPMediaCall->IsExpired())
		//1009解决问题，终端不接收呼叫时不会自动挂断
		if (pSIPMediaCall->IsExpired())
		{
			pSIPMediaCall->CheckMediaHangupCall();
			break;
		}
		++iter;
	}
}

void SIPCallMgr::TimerRegAccountCheck(unsigned long nTimestamp)
{
	//notice prevent lock to lock add
	int nLineID=0;
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPUser);
#endif

	while(true)
	{
		SIPUser *pSIPUser=NULL;
		{
			MapSIPUser::iterator iter=m_MapSIPUser.find(nLineID);
			if(iter!=m_MapSIPUser.end())
			{
				pSIPUser=iter->second;
				nLineID++;
			}
		}
		if(pSIPUser)
		{
			pSIPUser->SIPRegCheckStatus(nTimestamp);
		}
		else break;
	}
}

void SIPCallMgr::SetCapabilityParam(SIPCall * pSIPCall,SIPCallCapability & sdpCap,int nSupportDual,std::string strLocalIP,int nLocalAudioPort,int nLocalVideoPort,int nLocalDualVideoPort,int nLocalBFCPTransPort,int nDir)
{
	sdpCap.SetSDPOwnerUsername(m_strUserAgentName);
	sdpCap.SetPayloadType(e_Video_H264,m_H264PT);
	sdpCap.SetPayloadType(e_Video_H264SVC,m_H264SVCPT);
	sdpCap.SetPayloadType(e_Video_H263P,m_H263PPT);
	sdpCap.SetPayloadType(e_Video_H263PP,m_H263PPPT);
	if(pSIPCall) sdpCap.SetSDPMediaBandwidth(64,pSIPCall->GetMgrMainVideoMaxBitrate(),pSIPCall->GetMgrSecondVideoMaxBitrate());
	//set sip net address
	sdpCap.SetSDPMediaIP((char * )strLocalIP.c_str());
	sdpCap.SetSDPMediaPort(e_Media_Audio,nLocalAudioPort);
	sdpCap.SetSDPMediaPort(e_Media_Video,nLocalVideoPort);
	if(nSupportDual >0)
	{
		sdpCap.SetSDPMediaPort(e_Media_DualVideo,nLocalDualVideoPort);
		sdpCap.SetBFCPTransportPort(nLocalBFCPTransPort);
	}

////notice mcu not use wait stable
//#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
//		sdpCap.SetRtpFECEnableAttribute(false);
//#endif

	SetSDPBFCPLocalInfo(sdpCap,nDir);
}

void SIPCallMgr::OnDAP_RecvRemoteAudioRTPPacket(const char*cszCallID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_RecvRemoteAudioRTPPacket(cszCallID,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
#endif
}

void SIPCallMgr::OnDAP_RecvRemoteMainVideoRTPPacket(const char*cszCallID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_RecvRemoteMainVideoRTPPacket(cszCallID,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
#endif
}

void SIPCallMgr::OnDAP_RecvRemoteSecondVideoRTPPacket(const char*cszCallID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_RecvRemoteSecondVideoRTPPacket(cszCallID,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
#endif
}

void SIPCallMgr::OnDAP_RecvCallCustomMsg(const std::string& strCallID,const char * cszContent,const char * cszMimeType)
{
	//int len=strlen(cszContent);
//#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	//CallMgr::Instance().OnDAP_RecvNonstandardData(strCallID.c_str(),cszContent,len,cszMimeType);
//#endif

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		std::string strMime =cszMimeType;
		std::string strInfo =cszContent;
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","NONS");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("INFO",strInfo);
		tStrPacket.Set("MIME",strMime);
		std::string strText="";
		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

}

void SIPCallMgr::OnDAP_RecvCallDualVideoControl(const std::string& strCallID,int nType,int transactionId,int userId,int floorId,int status)
{
	LogFile(NULL,NULL,"","","SIPCallMgr::OnDAP_RecvCallDualVideoControl         111");

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_RecvCallDualVideoControl(strCallID.c_str(),nType,transactionId,userId,floorId,status);
#endif

	LogFile(NULL,NULL,"","","SIPCallMgr::OnDAP_RecvCallDualVideoControl         222");

}


#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

void SIPCallMgr::StartVideoToCall(const char*cszCallID,unsigned long nVideoID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif

		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	
	if(pSIPMediaCall) 
		pSIPMediaCall->StartVideoToCall(nVideoID,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort);
}
}

void SIPCallMgr::StopVideoToCall(const char*cszCallID)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif

		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	
	if(pSIPMediaCall) 
		pSIPMediaCall->StopVideoToCall();
}
}

void SIPCallMgr::StartH239ToCall(const char*cszCallID,unsigned long nVideoID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort)
{
	LogFile(NULL,NULL,"","","SIPCallMgr::StartH239ToCall         111");

	SIPMediaCall*pSIPMediaCall=NULL;
	{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif

		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	
	if(pSIPMediaCall) 
		pSIPMediaCall->StartH239ToCall(nVideoID,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort);
	}

	LogFile(NULL,NULL,"","","SIPCallMgr::StartH239ToCall         222");
}

void SIPCallMgr::StopH239ToCall(const char*cszCallID)
{
	LogFile(NULL,NULL,"","","SIPCallMgr::StopH239ToCall         111");

	SIPMediaCall*pSIPMediaCall=NULL;
	{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif

		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	
	if(pSIPMediaCall) 
		pSIPMediaCall->StopH239ToCall();
	}

	LogFile(NULL,NULL,"","","SIPCallMgr::StopH239ToCall         222");

}

void SIPCallMgr::StartAudioToCall(const char*cszCallID,unsigned long nAudioID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif

		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	
	if(pSIPMediaCall) 
		pSIPMediaCall->StartAudioToCall(nAudioID,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort);
}
}

void SIPCallMgr::StopAudioToCall(const char*cszCallID)
{
	SIPMediaCall*pSIPMediaCall=NULL;
	{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif

		MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
		if (iter!=m_MapSIPMediaCall.end())
		{
			pSIPMediaCall=iter->second;
		}
	
	if(pSIPMediaCall) 
		pSIPMediaCall->StopAudioToCall();
}
}

#endif

void SIPCallMgr::AllowToSendSecondVideo(const char*cszCallID, bool bAllow)
{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPMediaCall);
#endif

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		pSIPMediaCall->AllowToSendSecondVideo(bAllow);
	}
}

// 发流控命令
void SIPCallMgr::SendFlowControlCommand(const char*cszCallID, IMXEC_CALL_MEDIA_TYPE emMediaType, int nMaximumBitRate)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
	XAutoLock l(m_csMapSIPMediaCall);
#endif

	MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.find(cszCallID);
	if (iter!=m_MapSIPMediaCall.end())
	{
		SIPMediaCall*pSIPMediaCall=iter->second;
		//pSIPMediaCall->SendFlowControlCommand(emMediaType, nMaximumBitRate);
	}
}

void SIPCallMgr::OpenSIPMediaChannel(SIPMediaCall* pSIPMediaCall)
{
	if(pSIPMediaCall && pSIPMediaCall->IsCallEstablished())
	{
		std::string & strText =pSIPMediaCall->GetCallMediaUpdatePacket();
		if(strText.size() >0)
		{
			XDataBuffer*pXDataBuffer=new XDataBuffer();
			if (pXDataBuffer!=NULL)
			{
				pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
				SetDataBuffer(pXDataBuffer);
			}
			strText ="";
		}
	}
}

void SIPCallMgr::HungupAll(int nLineID)
{
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) && defined(NOT_WIN32_SIP_MSG) && (NOT_WIN32_SIP_MSG == 1)
		XAutoLock l(m_csMapSIPMediaCall);
#endif
		while (true)
		{
			SIPMediaCall*pSIPMediaCall=NULL;
			MapSIPMediaCall::iterator iter=m_MapSIPMediaCall.begin();
			if (iter!=m_MapSIPMediaCall.end())
			{
				pSIPMediaCall=iter->second;
				m_MapSIPMediaCall.erase(iter);
			}
			else
				break;
			if(pSIPMediaCall)
			{
				if(nLineID >=0 && pSIPMediaCall->m_nLineId != nLineID) continue;

				const std::string & strCallID=pSIPMediaCall->GetCallID();
				const std::string & strUserID=pSIPMediaCall->GetUserID();
				const std::string & strUserName=pSIPMediaCall->GetUserName();
				pSIPMediaCall->HangupSIPCall();
				pSIPMediaCall->HangupCall();
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
				std::string strDisconnectReason="DisconnectedLocal";
				CallMgr::Instance().OnCallDestroyed(strCallID,strUserID,strUserName,strDisconnectReason.c_str(),strDisconnectReason.length());

#else
				CallMgr::Instance().OnCallDestroyed(strCallID.c_str(),strUserID,strUserName,ModeDisconnectedLocal,pSIPMediaCall->GetCallNum());
#endif

				pSIPMediaCall->Close();
				delete pSIPMediaCall;
				pSIPMediaCall=NULL;
			}
			else break;
		}
	}

	{
		XAutoLock ll(m_csMapThrSIPMediaCall);
		while(true)
		{
			MapSIPMediaCall::iterator iter=m_MapThrSIPMediaCall.begin();
			if (iter!=m_MapThrSIPMediaCall.end())
			{
				SIPMediaCall*pSIPMediaCall=iter->second;
				m_MapThrSIPMediaCall.erase(iter);
			}
			else break;
		}
	}
}

void SIPCallMgr::OnDAP_RecvMainVideoRequestKeyFrame(const char*cszCallID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_RecvMainVideoRequestKeyFrame(cszCallID);
#endif
}

void SIPCallMgr::OnDAP_RecvSecondVideoRequestKeyFrame(const char*cszCallID)
{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_RecvSecondVideoRequestKeyFrame(cszCallID);
#endif
}

unsigned short SIPCallMgr::GetCallRTPPort(void)
{
	unsigned short nRTPPort=CallMgr::Instance().GetMediaReleasePort(m_MediaPortRelease);
	if(nRTPPort >0) return nRTPPort;
	nRTPPort=GetRTPPort(CallMgr::Instance().GetRTPBasePort());
	CallMgr::Instance().SetRTPBasePort(nRTPPort+2);
	return nRTPPort;
}

void SIPCallMgr::SaveMediaReleasePort(unsigned short rtpPort)
{
	CallMgr::Instance().SaveMediaReleasePort(m_MediaPortRelease,rtpPort);
}

void SIPCallMgr::GetTerminalInfo(SIPMediaCall* pSIPMediaCall,pjsip_rx_data *rdata)
{
	if(pSIPMediaCall && rdata)
	{
		char uagent[128]={0};
		pjsip_generic_string_hdr * ua_hdr;
		const pj_str_t STR_UserAgent = { "User-Agent", 10};
		pjsip_msg * msg = rdata->msg_info.msg;
		ua_hdr = (pjsip_generic_string_hdr*)
			pjsip_msg_find_hdr_by_name(msg, &STR_UserAgent, NULL); //PJSIP_H_USER_AGENT_UNIMP
		if (ua_hdr)
		{
			strncpy(uagent,ua_hdr->hvalue.ptr,( ua_hdr->hvalue.slen >127 ? 127 : ua_hdr->hvalue.slen ) );
		}
		EM_ENTITY_TYPE emEntityType = EM_ENTITY_UNKNOWN;
		char * sub1=strstr(uagent,"AVCON-SIP");
		char * sub2=strstr(uagent,"AVCONMCU-SIP");
		if(sub1) emEntityType = EM_ENTITY_TERMINAL;
		if(sub2) emEntityType = EM_ENTITY_MCU;

		CallParamSet callParam;
		memset(&callParam,0,sizeof(callParam));
		callParam.m_nEntityType=emEntityType;
		callParam.m_nCallDir=pSIPMediaCall->m_nCallDir;
		memcpy(callParam.m_strCallID,pSIPMediaCall->GetCallID().c_str(),pSIPMediaCall->GetCallID().length());
		memcpy(callParam.m_strUserID,pSIPMediaCall->GetUserID().c_str(),pSIPMediaCall->GetUserID().length());
		memcpy(callParam.m_strProductID,uagent,strlen(uagent));

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) 
#else 

			callParam.m_nMainVideoBitrate = SIPCallMgr::Instance().m_nMainVideoBitrate ;
			callParam.m_nDualVideoBitrate = SIPCallMgr::Instance().m_nSecondVideoMaxBitrate ;

#endif

		CallMgr::Instance().OnPeerEntityInfo(&callParam);
	}
}

void SIPCallMgr::GetServerInfo(pjsip_rx_data *rdata)
{
	//Server: kamailio (3.1.2 (i386/linux))
	if(rdata)
	{
		char uagent[128]={0};
		pjsip_generic_string_hdr * ua_hdr;
		const pj_str_t STR_UserAgent = { "Server", 6};
		pjsip_msg * msg = rdata->msg_info.msg;
		ua_hdr = (pjsip_generic_string_hdr*)
			pjsip_msg_find_hdr_by_name(msg, &STR_UserAgent, NULL); //PJSIP_H_USER_AGENT_UNIMP
		if (ua_hdr)
		{
			strncpy(uagent,ua_hdr->hvalue.ptr,( ua_hdr->hvalue.slen >127 ? 127 : ua_hdr->hvalue.slen ) );
		}
		//char * sub1=strstr(uagent,"kamailio");
		//char * sub2=strstr(uagent,"kamailio (3.1.2 (i386/linux))");
		////notice tcp timeout ,not use 
		//if(sub1) m_nSIPServerType = 1;
	}
}

void SIPCallMgr::SetMediaPortArrayEmpty()
{
	memset(&m_MediaPortRelease,0,sizeof(m_MediaPortRelease) );
}

//回调挂断状态时再检测下是不是因为网络收不到媒体流原因挂断的
bool SIPCallMgr::DisconnectCheckRoundTripDalay(SIPMediaCall * pSIPMediaCall)
{
	if(pSIPMediaCall->m_nLocalHungUP == 1 || pSIPMediaCall->m_nLocalHungUP ==3) return false;
	if(pSIPMediaCall->IsExpired(5000) )  //5s
	{
		return true;
	}
	return false;
}

bool SIPCallMgr::CheckMapCallEmpty(void)
{
	bool bRet1=false;
	bool bRet2=false;
	{
	XAutoLock l(m_csMapThrSIPMediaCall);
	if(m_MapThrSIPMediaCall.empty()) bRet1=true;
	}

	if(m_MapSIPMediaCall.empty()) bRet2=true;

	return (bRet1 && bRet2 ) ? true:false;
}
