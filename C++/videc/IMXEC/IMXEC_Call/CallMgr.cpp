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

//ע�� ���ų��� 0. ��ͷIP
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
*�������������
*���룺	cszUserID		-�Է����û�ID��IP
		cszProtocol		-����Э�飬"H323","SIP","IMXEC"
*�������
*����ֵ�����б�ʶ
*****************************************************************************/
const char*CallMgr::MakeCall(const char*cszUserID,const char*cszProtocol)
{
	//����ͬʱֻ����һ��call�ں���
#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )
#else

	if( IsCanCall() ==false) return m_strEmpty.c_str();
	SetCanCallNot();

#endif

	if (cszUserID==NULL || strlen(cszUserID)==0 || strlen(cszUserID)>128)
	{
		//����ͬʱֻ����һ��call�ں���
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
*���������ܺ�������
*���룺	cszCallID		-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*�������ҶϺ���
*���룺	cszCallID		-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*����������PTZ��������̨�������
*���룺	cszCallID		-���б�ʶ
		nPTZAction		-��̨����
		nSpeed			-�������ٶ�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ʼ����
*���룺	cszCallID		-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*������ֹͣ����
*���룺	cszCallID		-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*���������ñ���������Ƶ��ʾ�Ĵ���
*���룺	cszCallID		-���б�ʶ
		hWnd			-��������Ƶ��ʾ�Ĵ��ھ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*���������ñ���������Ƶ��ʾ�Ĵ���λ��
*���룺	cszCallID		-���б�ʶ
		x				-���ڵ����λ��
		y				-���ڵ��ϱ�λ��
		cx				-���ڵĿ��
		cy				-���ڵĸ߶�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*���������ñ���������Ƶ����
*���룺	cszCallID		-���б�ʶ
		nBrightness		-����ֵ0-255
		nContrast		-�Աȶ�0-127
		nSaturation		-���Ͷ�0-127
		nHue			-ɫ��0-255
		nGamma			-٤��1-400
		nSharpen		-��0-127
*�������
*����ֵ����
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
*���������ñ���������Ƶ�Ƿ�����OSD
*���룺	cszCallID		-���б�ʶ
		nEnable			-�Ƿ�����OSD
*�������
*����ֵ����
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
*���������ñ���������ƵOSD����
*���룺	cszCallID		-���б�ʶ
		cszText			-OSD����
		nAlign			-���뷽ʽ
*�������
*����ֵ����
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
*���������ñ���������ƵOSD����,�ֵ���ɫ�ͱ���ɫ
*���룺	cszCallID		-���б�ʶ
		logfont			-OSD����
		crText			-�ֵ���ɫ
		crBk			-����ɫ
*�������
*����ֵ����
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
*����������Զ��������Ƶ��ʾ�Ĵ���
*���룺	cszCallID		-���б�ʶ
		hWnd			-Զ������Ƶ��ʾ�Ĵ��ھ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*����������Զ��������Ƶ��ʾ�Ĵ���λ��
*���룺	cszCallID		-���б�ʶ
		x				-���ڵ����λ��
		y				-���ڵ��ϱ�λ��
		cx				-���ڵĿ��
		cy				-���ڵĸ߶�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*����������Զ��������Ƶ����
*���룺	cszCallID		-���б�ʶ
		nBrightness		-����ֵ0-255
		nContrast		-�Աȶ�0-127
		nSaturation		-���Ͷ�0-127
		nHue			-ɫ��0-255
		nGamma			-٤��1-400
		nSharpen		-��0-127
*�������
*����ֵ����
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
*���������ø�����Ƶ��ʾ�Ĵ���
*���룺	cszCallID		-���б�ʶ
		hWnd			-Զ�̸�����Ƶ��ʾ�Ĵ��ھ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*���������ø�����Ƶ��ʾ�Ĵ���λ��
*���룺	cszCallID		-���б�ʶ
		x				-���ڵ����λ��
		y				-���ڵ��ϱ�λ��
		cx				-���ڵĿ��
		cy				-���ڵĸ߶�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*���������ø�����Ƶ����
*���룺	cszCallID		-���б�ʶ
		nBrightness		-����ֵ0-255
		nContrast		-�Աȶ�0-127
		nSaturation		-���Ͷ�0-127
		nHue			-ɫ��0-255
		nGamma			-٤��1-400
		nSharpen		-��0-127
*�������
*����ֵ����
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
*���������ø�����Ƶ�Ƿ�����OSD
*���룺	cszCallID		-���б�ʶ
		nEnable			-�Ƿ�����OSD
*�������
*����ֵ����
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
*���������ø�����ƵOSD����
*���룺	cszCallID		-���б�ʶ
		cszText			-OSD����
		nAlign			-���뷽ʽ
*�������
*����ֵ����
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
*���������ø�����ƵOSD����,�ֵ���ɫ�ͱ���ɫ
*���룺	cszCallID		-���б�ʶ
		logfont			-OSD����
		crText			-�ֵ���ɫ
		crBk			-����ɫ
*�������
*����ֵ����
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
*��������ȡ����������Ƶ��Ϣ
*���룺	cszCallID		-���б�ʶ
*�����	nWidth			-ͼ����
		nHeight			-ͼ��߶�
		nVideoCodecType	-��������
		hVideoWnd		-���ھ��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡ��������Ƶ�İ���ͳ������
*���룺	cszCallID			-���б�ʶ
*����� ulTotalPackets		-�ܹ������˶��ٸ���
		ulLostPackets		-�ܹ������˶��ٸ���
		AvgPacketLossRate	-ƽ��������
		CurPacketLossRate	-��ǰ������
		nBitrate			-������bps��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡԶ��������Ƶ��Ϣ
*���룺	cszCallID		-���б�ʶ
*�����	nWidth			-ͼ����
		nHeight			-ͼ��߶�
		nVideoCodecType	-��������
		hVideoWnd		-���ھ��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡԶ������Ƶ�İ���ͳ������
*���룺	cszCallID			-���б�ʶ
*����� ulTotalPackets		-�ܹ������˶��ٸ���
		ulLostPackets		-�ܹ������˶��ٸ���
		AvgPacketLossRate	-ƽ��������
		CurPacketLossRate	-��ǰ������
		nBitrate			-������bps��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡ������Ƶ��Ϣ
*���룺	cszCallID		-���б�ʶ
*�����	nWidth			-ͼ����
		nHeight			-ͼ��߶�
		nVideoCodecType	-��������
		hVideoWnd		-���ھ��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡ������Ƶ�İ���ͳ������
*���룺	cszCallID			-���б�ʶ
*����� ulTotalPackets		-�ܹ������˶��ٸ���
		ulLostPackets		-�ܹ������˶��ٸ���
		AvgPacketLossRate	-ƽ��������
		CurPacketLossRate	-��ǰ������
		nBitrate			-������bps��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡ������ƵLevel��ʵ��������С��
*���룺	cszCallID		-���б�ʶ
*�����	nLevel			-ʵ��������С��0-10��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡ������Ƶ����
*���룺	cszCallID		-���б�ʶ
		nVolume			-����
*�����	��
*����ֵ����
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
*��������ȡ������Ƶ�Ƿ���
*���룺	cszCallID		-���б�ʶ
		nMute			-�Ƿ���
*�����	��
*����ֵ����
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
*��������ȡ������Ƶ������Ϣ
*���룺	cszCallID		-���б�ʶ
*�����	nAudioCodecID	-����ID
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡ������Ƶ�İ���ͳ������
*���룺	cszCallID			-���б�ʶ
*����� ulTotalPackets		-�ܹ������˶��ٸ���
		ulLostPackets		-�ܹ������˶��ٸ���
		AvgPacketLossRate	-ƽ��������
		CurPacketLossRate	-��ǰ������
		nBitrate			-������bps��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡԶ����ƵLevel��ʵ��������С��
*���룺	cszCallID		-���б�ʶ
*�����	nLevel			-ʵ��������С��0-10��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡԶ����Ƶ����
*���룺	cszCallID		-���б�ʶ
		nVolume			-����
*�����	��
*����ֵ����
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
*��������ȡԶ����Ƶ�Ƿ���
*���룺	cszCallID		-���б�ʶ
		nMute			-�Ƿ���
*�����	��
*����ֵ����
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
*��������ȡԶ����Ƶ����
*���룺	cszCallID		-���б�ʶ
*�����	nAudioCodecID	-����ID
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡԶ����Ƶ�İ���ͳ������
*���룺	cszCallID			-���б�ʶ
*����� ulTotalPackets		-�ܹ������˶��ٸ���
		ulLostPackets		-�ܹ������˶��ٸ���
		AvgPacketLossRate	-ƽ��������
		CurPacketLossRate	-��ǰ������
		nBitrate			-������bps��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡ�����Ƿ�֧�ָ�����H239��
*���룺	cszCallID		-���б�ʶ
*�����	��
*����ֵ��֧�ַ��ط�0����֧�ַ���0
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
*���������յ�PTZ�¼������Է�������̨��������ʱ��ϵͳ����ô��¼�
*���룺	cszCallID		-���б�ʶ
		nPTZAction		-��̨����
		nSpeed			-�������ٶ�
*�������
*����ֵ����
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
*���������յ�DTMF�¼������Է�����DTMF����ʱ��ϵͳ����ô��¼�
*���룺	cszCallID		-���б�ʶ
		szDtmf			-DTMF����
*�������
*����ֵ����
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
*������������ʼ�¼�����������ʼʱ��ϵͳ����ô��¼�
*���룺	strCallID		-���б�ʶ
*�������
*����ֵ����
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
*����������ֹͣ�¼���������ֹͣʱ��ϵͳ����ô��¼�
*���룺	strCallID		-���б�ʶ
*�������
*����ֵ����
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
*������SIPע��ɹ��¼�
*���룺	nLineID		-��·��ʶ
*�������
*����ֵ����
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
*������SIPע��ʧ���¼�
*���룺	nLineID			-��·��ʶ
		cszErrorCode	-������
*�������
*����ֵ����
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
*������H323ע��ɹ��¼�
*���룺��
*�������
*����ֵ����
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
*������H323ע��ʧ���¼�
*���룺��
*�������
*����ֵ����
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
*���������ú�������
*���룺	nCallType		-��������
*�����	��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡ��������
*���룺	��
*�����	nCallType		-��������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ʼ��������¼��
*���룺	cszCallID			-���б�ʶ
		cszPathName			-¼���ļ�����
		nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*������ֹͣ��������¼��
*���룺cszCallID			-���б�ʶ
*�������
*����ֵ����
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
*��������ʼԶ������¼��
*���룺	cszCallID			-���б�ʶ
		cszPathName			-¼���ļ�����
		nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*������ֹͣԶ������¼��
*���룺cszCallID			-���б�ʶ
*�������
*����ֵ����
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
*��������ʼ����¼��
*���룺	cszCallID			-���б�ʶ
		cszPathName			-¼���ļ�����
		nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*������ֹͣ����¼��
*���룺cszCallID			-���б�ʶ
*�������
*����ֵ����
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
*��������ȡ������Ƶ��ʶ
*���룺 cszCallID		-���б�ʶ
*����� nMainAudioID	-������Ƶ��ʶ
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡ������Ƶ��ʶ
*���룺 cszCallID		-���б�ʶ
*����� nMainVideoID	-������Ƶ��ʶ
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡ������Ƶ��ʶ
*���룺 cszCallID		-���б�ʶ
*����� nSecondVideoID	-������Ƶ��ʶ
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*����������������Ƶͼ���������
*���룺 cszCallID		-���б�ʶ
		nScreenIndex	-�������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*���������ø�����Ƶͼ���������
*���룺 cszCallID		-���б�ʶ
		nScreenIndex	-�������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*���������ø�����Ƶ������Ļ����λ��
*���룺	cszCallID		-���б�ʶ
		nScreenIndex	-��Ļ��
		nWndIndex		-���ں�
		nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
*�������
*����ֵ����
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
*����������Զ����Ƶ������Ļ����λ��
*���룺	cszCallID		-���б�ʶ
		nScreenIndex	-��Ļ��
		nWndIndex		-���ں�
		nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
*�������
*����ֵ����
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
*���������ñ���¼��ص�
*���룺	cszCallID		-���б�ʶ
		pRecordCallback	-¼��ص�
*�������
*����ֵ����
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
*����������Զ��¼��ص�
*���룺	cszCallID		-���б�ʶ
		pRecordCallback	-¼��ص�
*�������
*����ֵ����
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
*���������ø���¼��ص�
*���룺	cszCallID		-���б�ʶ
		pRecordCallback	-¼��ص�
*�������
*����ֵ����
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
*���������汾��������Ƶ��BMPͼƬ�ļ�
*���룺	cszCallID		-���б�ʶ
		cszPathName		-ͼƬ�ļ�ȫ·��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*����������Զ��������Ƶ��BMPͼƬ�ļ�
*���룺	cszCallID		-���б�ʶ
		cszPathName		-ͼƬ�ļ�ȫ·��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*���������渨����Ƶ��BMPͼƬ�ļ�
*���룺	cszCallID		-���б�ʶ
		cszPathName		-ͼƬ�ļ�ȫ·��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������Ƶ��ʾ֧����ʾ���Ȳ�νӿ�
*���룺	cszCallID		    -���б�ʶ
hWndLocal			-��������Ƶ��ʾ�Ĵ��ھ��
hWndRemote			-Զ������Ƶ��ʾ�Ĵ��ھ��
hWndSecond         	-Զ�̸�����Ƶ��ʾ�Ĵ��ھ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*����������DTMF
*���룺	cszCallID		-���б�ʶ
		nDTMF			-DTMF
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ʼ�Ժ��н���WAVE�ļ��������Է�������WAVE�ļ����ŵ�����
*���룺	cszCallID		-���б�ʶ
		cszPathName		-WAVE�ļ�ȫ·��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*������ֹͣ�Ժ��н���WAVE�ļ�����
*���룺	cszCallID		-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*�����������Ƿ�ֻ֧����Ƶ
*���룺	nAudioOnly		-�Ƿ�ֻ֧����Ƶ��0��֧����Ƶ��Ƶ����0��ֻ֧����Ƶ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡ����Э��
*���룺	cszCallID		-���б�ʶ
*�������
*����ֵ���ɹ����غ���Э�飬ʧ�ܷ��ؿմ�""
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
*��������ȡ�Ƿ�ֻ����Ƶ
*���룺cszCallID		-���б�ʶ
*�������
*����ֵ���������Ƶ����ô���ط�0�����򷵻�0
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
*�����������Ƿ�������Ƶת��
*���룺nEnable		-�Ƿ�������Ƶת�롾0����������Ƶת�룬��0��������Ƶת�롿
*�������
*����ֵ����
*****************************************************************************/
void CallMgr::SetEnableVideoTranscoding(int nEnable)
{
	m_nEnableVideoTranscoding=nEnable;
}

/******************************************************************************
* GetEnableVideoTranscoding
*��������ȡ�Ƿ�������Ƶת��
*���룺��
*�������
*����ֵ�����������Ƶת�룬��ô���ط�0�����򷵻�0
*****************************************************************************/
int CallMgr::GetEnableVideoTranscoding(void)
{
	return m_nEnableVideoTranscoding;
}

/******************************************************************************
* GetSecondVideoDirection
*��������ȡ�����ķ���
*���룺	cszCallID		-���б�ʶ
*����� nDir			-�����ķ���
*����ֵ���ɹ�����0��ʧ�ܷ���-1
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
*��������ȡH235��֤���״̬ ����ȡ������֤��Ϊ����ʱ����֤���
*���룺	cszCallID		  -���б�ʶ
*����� nState -��֤���״̬
nLength -������֤ʧ��ԭ���ַ�������
*����ֵ���ɹ�������֤ʧ��ԭ��ʧ�ܷ��ؿ�
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
*����������ý��ͨ���Ѽ���״̬֪ͨ�����򿪼��ܵĽ���ý��ͨ��ʱ��ϵͳ����ô��¼�
*���룺	cszCallID		-���б�ʶ
		nMediaType		-ý������
*�������
*����ֵ����
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
*����������ý��ͨ���Ѽ���״̬֪ͨ�����򿪼��ܵķ���ý��ͨ��ʱ��ϵͳ����ô��¼�
*���룺	cszCallID		-���б�ʶ
		nMediaType		-ý������
*�������
*����ֵ����
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
*��������������ӳ��IP
*���룺cszExternalIP		-����ӳ��IP
*�������
*����ֵ����
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
*���������յ�H245������Ϣ�ص� ����flowControlIndication H239������� �����Ϣ
*���룺	cszCallID			-���б�ʶ
nMsgType			-������Ϣ����
cszPacket			-��Ϣ��
nPacketLen			-��Ϣ������
*�������
*����ֵ����
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
*��������������I֡
*���룺cszCallID		-���б�ʶ
*�������
*����ֵ����
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
*������������I֡
*���룺cszCallID		-���б�ʶ
*�������
*����ֵ����
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
	*�������Ƿ��к���
	*���룺��
	*�������
	*����ֵ��BOOL
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
*���������ñ�������ƵԴ
*���룺cszCallID		  -���б�ʶ
nMainVideoDevID    -�����豸ID
*�������
*����ֵ����
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
*���������ñ��ظ���ƵԴ
*���룺cszCallID		  -���б�ʶ
nSecondVideoDevID  -�����豸ID
*�������
*����ֵ����
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
*����:	��Զ˷����Զ���Ǳ���Ϣ
*����:	cszCallID		-���б�ʶ
cszContent		-�Զ���Ǳ�׼��Ϣ����
nContentLen		-�Զ���Ǳ�׼��Ϣ����
cszMimeType		-�Զ���Ǳ���Ϣ�������� ��ΪXML ֵ��Ϊapplication/xml,��Ϊһ���ַ�������ΪNULL
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
*���������յ����ն˷���H245�Ǳ���Ϣʱϵͳ����ô��¼�
*���룺	cszCallID		-���б�ʶ
		cszPacket		-�Ǳ�׼��Ϣ����
		nPacketLen		-�Ǳ�׼��Ϣ����
		cszMimeType		-Ŀǰ��SIPʹ�� ��Ϣ�������� ��application/xml
*�������
*����ֵ����
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
*���������յ���Ƶý�����ص�
*���룺	cszCallID			-���б�ʶ
pRTPPacketData		-RTP��
nRTPPacketLen		-RTP������
nPayloadType		-PayloadType
nRTPHeaderLen		-RTP��ͷ����
usSequence			-RTP������
ulTimestamp			-RTP��ʱ���
*�������
*����ֵ����
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
*���������յ�������Ƶý�����ص�
*���룺	cszCallID			-���б�ʶ
pRTPPacketData		-RTP��
nRTPPacketLen		-RTP������
nPayloadType		-PayloadType
nRTPHeaderLen		-RTP��ͷ����
usSequence			-RTP������
ulTimestamp			-RTP��ʱ���
*�������
*����ֵ����
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
*���������յ�������Ƶý�����ص�
*���룺	cszCallID			-���б�ʶ
pRTPPacketData		-RTP��
nRTPPacketLen		-RTP������
nPayloadType		-PayloadType
nRTPHeaderLen		-RTP��ͷ����
usSequence			-RTP������
ulTimestamp			-RTP��ʱ���
*�������
*����ֵ����
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
*���������ضԶ�ʵ����Ϣ
*���룺	pCallParam			-CallParamSet ��Ϣ��
*�������
*����ֵ����
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
*�������Ƿ�ʹ��NETEC˽�д���
*���룺	cszCallID			-���б�ʶ
        nEnable             -�Ƿ�ʹ��,1Ϊʹ��,0Ϊ��ʹ��
*�������
*����ֵ����
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
*��������Ƶ����ͨ�����ӳɹ��¼�
*���룺	cszCallID			-���б�ʶ
AudioCodecID		-��ƵCodecID
nPayloadType		-PayloadType
ip					-���˽���rtp ip
port				-���˽���rtp port
*�������
*����ֵ����
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
*��������Ƶ����ͨ���ر��¼�
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ����
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
*������������Ƶ����ͨ�����ӳɹ��¼�
*���룺	cszCallID			-���б�ʶ
codecType			-��ƵCodecID
nBitrate			-����
nPayloadType		-PayloadType
width				-�ֱ��ʿ�������� ��ѡ
nHeight				-�ֱ��ʸ߶������� ��ѡ
ip					-���˽���rtp ip
port				-���˽���rtp port
nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
*�������
*����ֵ����
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
*������������Ƶ����ͨ���ر��¼�
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ����
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
*������������Ƶ����ͨ�����ӳɹ��¼�
*���룺	cszCallID			-���б�ʶ
codecType			-��ƵCodecID
nBitrate			-����
nPayloadType		-PayloadType
width				-�ֱ��ʿ�������� ��ѡ
nHeight				-�ֱ��ʸ߶������� ��ѡ
ip					-���˽���rtp ip
port				-���˽���rtp port
nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
*�������
*����ֵ����
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
*������������Ƶ����ͨ���ر��¼�
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ����
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
*������FECC���ƽ���ͨ�����ӳɹ��¼�
*���룺	cszCallID			-���б�ʶ
nPayloadType		-PayloadType
ip					-���˽���rtp ip
port				-���˽���rtp port
*�������
*����ֵ����
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
*������FECC���ƽ���ͨ���ر��¼�
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ����
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
*��������Ƶ����ͨ�����ӳɹ��¼�
*���룺	HDAPAPPCALL hdapCall ʵ��call���
cszCallID			-���б�ʶ
AudioCodecID		-��ƵCodecID
nPayloadType		-PayloadType
ip					-�Զ�rtp ip
port				-�Զ�rtp port
*�������
*����ֵ����
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
*��������Ƶ����ͨ���ر��¼�
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ����
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
*������������Ƶ����ͨ�����ӳɹ��¼�
*���룺	HDAPAPPCALL hdapCall ʵ��call���
cszCallID			-���б�ʶ
codecType			-��ƵCodecID
nBitrate			-����
nPayloadType		-PayloadType
width				-�ֱ��ʿ�������� ��ѡ
nHeight				-�ֱ��ʸ߶������� ��ѡ
nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
ip					-�Զ�rtp ip
port				-�Զ�rtp port
*�������
*����ֵ����
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
*������������Ƶ����ͨ�����ӳɹ��¼�
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ����
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
*������������Ƶ����ͨ�����ӳɹ��¼�
*���룺	HDAPAPPCALL hdapCall ʵ��call���
cszCallID			-���б�ʶ
codecType			-��ƵCodecID
nBitrate			-����
nPayloadType		-PayloadType
width				-�ֱ��ʿ�������� ��ѡ
nHeight				-�ֱ��ʸ߶������� ��ѡ
nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
ip					-�Զ�rtp ip
port				-�Զ�rtp port
*�������
*����ֵ����
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
*������������Ƶ����ͨ�����ӳɹ��¼�
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ����
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
*������FECC���Ʒ���ͨ�����ӳɹ��¼�
*���룺	HDAPAPPCALL hdapCall ʵ��call���
cszCallID			-���б�ʶ
ip					-�Զ�rtp ip
port				-�Զ�rtp port
nPayloadType		-PayloadType
*�������
*����ֵ����
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
*������FECC���Ʒ���ͨ���ر��¼�
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ����
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

//�鿴�Ƿ�Ϊ�Ϸ�IP,�ų���ȫ���ֻ�ȫΪ�׶˴���0x��0X��ʮ�����Ƶ�����
bool IsCallIPString(const char * str)
{
	bool bRet=false;
	bool bLicit=false;
	int len=0;
	if(str!=NULL)
	{
		len=strlen(str);
		//�鿴�Ƿ�Ϊ�Ϸ�IP
		if(inet_addr(str) != INADDR_NONE)
			bLicit=true;
	}
	//�鿴�Ƿ���.��: �ų���ȫ���ֻ�ȫΪ�׶˴���0x��0X��ʮ�����Ƶ�����
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