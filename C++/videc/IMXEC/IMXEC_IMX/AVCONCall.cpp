#include "stdafx.h"
#include "AVCONCall.h"
#include "../IMXEC_Util/StrPacket.h"
#include "../IMXEC_Util/XUtil.h"
#include <NETEC/NETEC_Setting.h>

static AVCONCall g_AVCONCall;

AVCONCall::AVCONCall(void)
:m_strUserID("")
,m_strPassword("")
,m_strServerIP("")
,m_nServerPort(0)

,m_nLoginStatus(LS_IDLE)
{
	m_nIDEventTimer=0;
}

AVCONCall::~AVCONCall(void)
{

}

AVCONCall&AVCONCall::Instance(void)
{
	return g_AVCONCall;
}

VOID CALLBACK AVCONCallTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	AVCONCall::Instance().OnTimer(idEvent);
}

int AVCONCall::Login(const char*cszUserID,const char*cszPassword,const char*cszServerIP,unsigned short nServerPort)
{
	std::string strUserID="";
	if (cszUserID!=NULL)
	{
		strUserID=cszUserID;
	}

	std::string strPassword="";
	if (cszPassword!=NULL)
	{
		strPassword=cszPassword;
	}

	std::string strServerIP="";
	if (cszServerIP!=NULL)
	{
		strServerIP=cszServerIP;
	}

	m_strPassword=strPassword;

	if (strUserID!=m_strUserID ||
		strServerIP!=m_strServerIP ||
		nServerPort!=m_nServerPort)
	{
		SendLogout();
	
		m_strUserID=strUserID;
		m_strServerIP=strServerIP;
		m_nServerPort=nServerPort;
	}

	SetLocalUserID(m_strUserID.c_str());
	SetLocalUserName(m_strUserID.c_str());
	SetServerIP(m_strServerIP.c_str());
	SetServerPort(m_nServerPort);

	NETEC_Setting::SetMCUOnly(1);
	NETEC_Core::Start(0);
	NETEC_Node::Start();

	IMXEC_Call::SetCallType(IMXEC_PERSONAL_CALL);
	IMXEC_Call::Start();

	IMXEC_IMX::Start();

	SendLogin();

	if (m_nIDEventTimer==0)
	{
		m_nIDEventTimer=SetTimer(NULL,1,1,AVCONCallTimerProc);
	}

	return AVCON_CALL_ERROR_OK;
}

void AVCONCall::Logout(void)
{
	SendLogout();

	m_strUserID="";

	if (m_nIDEventTimer!=0)
	{
		KillTimer(NULL,m_nIDEventTimer);
		m_nIDEventTimer=0;
	}

	IMXEC_Call::Stop();
	IMXEC_IMX::Stop();
	NETEC_Node::Stop();
	NETEC_Core::Stop();
}

int AVCONCall::MakeCall(const char*cszUserID)
{
	if (cszUserID==NULL || strlen(cszUserID)==0)
	{
		return AVCON_CALL_ERROR_USERID;
	}

	if (m_nLoginStatus!=LS_LOGINED)
	{
		return AVCON_CALL_ERROR_USER_NOT_LOGGED_IN;
	}

	NETEC_Session::CONNECT_STATUS nStatus=GetConnectStatus();
	if (nStatus==NETEC_Session::CS_CONNECTED ||
		nStatus==NETEC_Session::CS_RECONNECTED ||
		nStatus==NETEC_Session::CS_RESTARTED)
	{

	}
	else
	{
		return AVCON_CALL_ERROR_NOT_CONNECTED_TO_MCU;
	}

	{
		MapPendingCall::iterator iter=m_MapPendingCall.find(cszUserID);
		if (iter!=m_MapPendingCall.end())
		{
			return AVCON_CALL_ERROR_USER_ALREADY_EXISTS;
		}
	}

	PENDING_CALL tPendingCall;
	tPendingCall.nTimestamp=XGetTimestamp();
	tPendingCall.strCallID="";
	tPendingCall.strNodeID="";
	tPendingCall.strUserID=cszUserID;

	m_MapPendingCall[tPendingCall.strUserID]=tPendingCall;

	SendSubscribe(tPendingCall.strUserID);

	{
		MapAVCONCall::iterator iter=m_MapAVCONCall.begin();
		while (iter!=m_MapAVCONCall.end())
		{
			iter->second->OnIMXEC_AVCONCallTrying(cszUserID,cszUserID);
			++iter;
		}
	}

	return AVCON_CALL_ERROR_OK;
}

int AVCONCall::AcceptCall(const char*cszUserID)
{
	std::string strCallID="";
	{
		MapPendingCall::iterator iter=m_MapPendingCall.find(cszUserID);
		if (iter!=m_MapPendingCall.end())
		{
			strCallID=iter->second.strCallID;
		}
	}

	if (strCallID.empty()==false)
	{
		if (0==IMXEC_Call::AcceptCall(strCallID.c_str()))
		{
			return AVCON_CALL_ERROR_OK;
		}
	}

	return AVCON_CALL_ERROR_USERID;
}

void AVCONCall::EndCall(const char*cszUserID)
{
	std::string strCallID="";
	std::string strUserName="";
	{
		MapPendingCall::iterator iter=m_MapPendingCall.find(cszUserID);
		if (iter!=m_MapPendingCall.end())
		{
			strCallID=iter->second.strCallID;
			strUserName=iter->second.strUserID;

			m_MapPendingCall.erase(iter);
		}
	}

	if (strCallID.empty()==false)
	{
		HangupCall(strCallID.c_str());

		MapAVCONCall::iterator iter=m_MapAVCONCall.begin();
		while (iter!=m_MapAVCONCall.end())
		{
			iter->second->OnIMXEC_AVCONCallDestroyed(cszUserID,strUserName.c_str(),IMXEC_AVCONCall::CDR_HANGUP_BY_LOCAL);
			++iter;
		}
	}
}

int AVCONCall::SetCallLocalWnd(const char*cszUserID,HWND hWnd)
{
	std::string strCallID="";
	{
		MapPendingCall::iterator iter=m_MapPendingCall.find(cszUserID);
		if (iter!=m_MapPendingCall.end())
		{
			strCallID=iter->second.strCallID;
		}
	}

	if (strCallID.empty()==false)
	{
		int nRet = SetLocalMainVideoWnd(strCallID.c_str(),hWnd);
		if (nRet==0)
		{
			return AVCON_CALL_ERROR_OK;
		}
	}

	return AVCON_CALL_ERROR_USERID;
}

int AVCONCall::SetCallLocalWndPos(const char*cszUserID,int x,int y,int cx,int cy)
{
	std::string strCallID="";
	{
		MapPendingCall::iterator iter=m_MapPendingCall.find(cszUserID);
		if (iter!=m_MapPendingCall.end())
		{
			strCallID=iter->second.strCallID;
		}
	}

	if (strCallID.empty()==false)
	{
		int nRet = SetLocalMainVideoWndPos(strCallID.c_str(),x,y,cx,cy);
		if (nRet==0)
		{
			return AVCON_CALL_ERROR_OK;
		}
	}

	return AVCON_CALL_ERROR_USERID;
}

int AVCONCall::SetCallRemoteWnd(const char*cszUserID,HWND hWnd)
{
	std::string strCallID="";
	{
		MapPendingCall::iterator iter=m_MapPendingCall.find(cszUserID);
		if (iter!=m_MapPendingCall.end())
		{
			strCallID=iter->second.strCallID;
		}
	}

	if (strCallID.empty()==false)
	{
		int nRet = SetRemoteMainVideoWnd(strCallID.c_str(),hWnd);
		if (nRet==0)
		{
			return AVCON_CALL_ERROR_OK;
		}
	}

	return AVCON_CALL_ERROR_USERID;
}

int AVCONCall::SetCallRemoteWndPos(const char*cszUserID,int x,int y,int cx,int cy)
{
	std::string strCallID="";
	{
		MapPendingCall::iterator iter=m_MapPendingCall.find(cszUserID);
		if (iter!=m_MapPendingCall.end())
		{
			strCallID=iter->second.strCallID;
		}
	}

	if (strCallID.empty()==false)
	{
		int nRet = SetRemoteMainVideoWndPos(strCallID.c_str(),x,y,cx,cy);
		if (nRet==0)
		{
			return AVCON_CALL_ERROR_OK;
		}
	}

	return AVCON_CALL_ERROR_USERID;
}

void AVCONCall::AddAVCONCall(IMXEC_AVCONCall*pAVCONCall)
{
	m_MapAVCONCall[pAVCONCall]=pAVCONCall;
}

void AVCONCall::RemoveAVCONCall(IMXEC_AVCONCall*pAVCONCall)
{
	m_MapAVCONCall.erase(pAVCONCall);
}

void AVCONCall::SendLogin(void)
{
	if (m_nLoginStatus!=LS_IDLE)
	{
		return ;
	}

	NETEC_Session::CONNECT_STATUS nStatus=GetConnectStatus();
	if (nStatus==NETEC_Session::CS_CONNECTED ||
		nStatus==NETEC_Session::CS_RECONNECTED ||
		nStatus==NETEC_Session::CS_RESTARTED)
	{
		m_nLoginStatus = LS_LOGINING;

		StrPacket tStrPacketOut(false);
		tStrPacketOut.Set("CMD","LOGIN");
		tStrPacketOut.Set("UID",m_strUserID);
		tStrPacketOut.Set("LIP",GetLocalIP());
		tStrPacketOut.Set("NIP",GetNATIP());

		std::string strText="";
		tStrPacketOut.GetString(strText);

		SendDataToRouter(GetDomain(),strText.c_str(),strText.size()+1);
	}
}

void AVCONCall::SendLogout(void)
{
	if (m_nLoginStatus!=LS_IDLE && m_nLoginStatus!=LS_LOGOUTING)
	{
		NETEC_Session::CONNECT_STATUS nStatus=GetConnectStatus();
		if (nStatus==NETEC_Session::CS_CONNECTED ||
			nStatus==NETEC_Session::CS_RECONNECTED ||
			nStatus==NETEC_Session::CS_RESTARTED)
		{
			m_nLoginStatus = LS_LOGOUTING;

			StrPacket tStrPacketOut(false);
			tStrPacketOut.Set("CMD","LOGOUT");
			tStrPacketOut.Set("UID",m_strUserID);

			std::string strText="";
			tStrPacketOut.GetString(strText);

			SendDataToRouter(GetDomain(),strText.c_str(),strText.size()+1);
		}
	}
}

void AVCONCall::SendSubscribe(const std::string&strUserID)
{
	NETEC_Session::CONNECT_STATUS nStatus=GetConnectStatus();
	if (nStatus==NETEC_Session::CS_CONNECTED ||
		nStatus==NETEC_Session::CS_RECONNECTED ||
		nStatus==NETEC_Session::CS_RESTARTED)
	{
		StrPacket tStrPacketOut(false);
		tStrPacketOut.Set("CMD","SUBSCRIBE");
		tStrPacketOut.Set("UID",strUserID);

		std::string strText="";
		tStrPacketOut.GetString(strText);

		SendDataToRouter(GetDomain(),strText.c_str(),strText.size()+1);
	}
}


void AVCONCall::OnTimer(unsigned int nTimerID)
{
	XDataBuffer*pXDataBuffer=GetDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		const char*pData=(const char*)pXDataBuffer->GetData();
		int nLen=pXDataBuffer->GetLen();

		if (pData!=NULL && nLen>0 && *(pData+nLen-1)=='\0')
		{
			StrPacket tStrPacketIn(pData,false);
			std::string strCMD="";
			tStrPacketIn.Get("CMD",strCMD);
			if (strCMD=="LOGIN")
			{
				std::string strError="";
				tStrPacketIn.Get("ERR",strError);
				if (strError=="ERR_OK")
				{
					m_nLoginStatus=LS_LOGINED;

					MapAVCONCall::iterator iter=m_MapAVCONCall.begin();
					while (iter!=m_MapAVCONCall.end())
					{
						iter->second->OnIMXEC_AVCONCallLoginOK(m_strUserID.c_str());
						++iter;
					}
				}
				else if (strError=="ERR_UID")
				{
					m_nLoginStatus=LS_IDLE;

					MapAVCONCall::iterator iter=m_MapAVCONCall.begin();
					while (iter!=m_MapAVCONCall.end())
					{
						iter->second->OnIMXEC_AVCONCallLoginFailed(m_strUserID.c_str(),AVCON_CALL_ERROR_USERID);
						++iter;
					}
				}
				else if (strError=="ERR_LICENCE")
				{
					m_nLoginStatus=LS_IDLE;

					MapAVCONCall::iterator iter=m_MapAVCONCall.begin();
					while (iter!=m_MapAVCONCall.end())
					{
						iter->second->OnIMXEC_AVCONCallLoginFailed(m_strUserID.c_str(),AVCON_CALL_ERROR_LICENCE);
						++iter;
					}
				}
			}
			else if (strCMD=="LOGOUT")
			{
				std::string strError="";
				tStrPacketIn.Get("ERR",strError);
				if (strError=="ERR_OK")
				{

				}
			}
			else if (strCMD=="SUBSCRIBE")
			{
				std::string strUserID="";
				std::string strNodeID="";
				std::string strLocalIP="";
				std::string strNATIP="";
				std::string strError="";

				tStrPacketIn.Get("ERR",strError);
				tStrPacketIn.Get("UID",strUserID);
				tStrPacketIn.Get("NID",strNodeID);
				tStrPacketIn.Get("LIP",strLocalIP);
				tStrPacketIn.Get("NIP",strNATIP);

				MapPendingCall::iterator iter=m_MapPendingCall.find(strUserID);
				if (iter!=m_MapPendingCall.end())
				{
					iter->second.strNodeID=strNodeID;
					const char*cszCallID=IMXEC_Call::MakeCall(strUserID.c_str(),"IMX");
					if (cszCallID!=NULL && strlen(cszCallID)>0)
					{
						iter->second.strCallID=cszCallID;
					}
					else
					{
						m_MapPendingCall.erase(iter);

						MapAVCONCall::iterator iterCall=m_MapAVCONCall.begin();
						while (iterCall!=m_MapAVCONCall.end())
						{
							iterCall->second->OnIMXEC_AVCONCallDestroyed(strUserID.c_str(),strUserID.c_str(),IMXEC_AVCONCall::CDR_USER_NOT_FOUND);
							++iterCall;
						}
					}
				}

			}
		}

		pXDataBuffer->Release();
		pXDataBuffer=NULL;
	}
}

/******************************************************************************
* OnNETEC_NodeConnectStatusChanged
*��������������״̬�Ļص�
*���룺	cs					-��������״̬
*����� ��
*����ֵ����
*****************************************************************************/
void AVCONCall::OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs)
{
	switch (cs)
	{
	case NETEC_Session::CS_CONNECTING:		//��������
		{
			//OutputDebugString("CS_CONNECTING\n");
		}
		break;
	case NETEC_Session::CS_FAILED:				//�޷�����
		{
			//OutputDebugString("CS_FAILED\n");
		}
		break;
	case NETEC_Session::CS_CONNECTED:			//�Ѿ�����
		{
			//OutputDebugString("CS_CONNECTED\n");
			SendLogin();
		}
		break;
	case NETEC_Session::CS_DISCONNECTED:		//�Ͽ�����
		{
			//OutputDebugString("CS_DISCONNECTED\n");
		}
		break;
	case NETEC_Session::CS_BUSY:				//����æ(�ѶϿ�������)
		{
			//OutputDebugString("CS_BUSY\n");
		}
		break;
	case NETEC_Session::CS_RECONNECTED:			//�����ɹ�
		{
			//OutputDebugString("CS_RECONNECTED\n");
		}
		break;
	case NETEC_Session::CS_IDLE:				//����
		{
			//OutputDebugString("CS_IDLE\n");
		}
		break;
	case NETEC_Session::CS_RESTARTED:			//���������ˡ����ӶϿ��ˣ������������������ˣ����ǻ���һ�������ӡ�
		{
			//OutputDebugString("CS_RESTARTED\n");
			SendLogin();
		}
		break;
	}
}


/******************************************************************************
* OnNETEC_NodeReceivedFromRouter
*�������յ�����·�����ݰ��Ļص�
*���룺	cszDomain				-����·�����ڵ���
		pData					-����
		nLen					-���ݳ���
*����� ��
*����ֵ����
*****************************************************************************/
void AVCONCall::OnNETEC_NodeReceivedFromRouter(const char*cszDomain,const char* pData,int nLen)
{
	//OutputDebugString("OnNETEC_NodeReceivedFromRouter:");
	//OutputDebugString(pData);
	//OutputDebugString("\n");

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer->StoreData((char*)pData,nLen)==false)
	{
		delete pXDataBuffer;
		pXDataBuffer=NULL;
		return ;
	}

	SetDataBuffer(pXDataBuffer);
}

/******************************************************************************
* OnNETEC_NodeGetUserInfo
*��������ȡ�û���Ϣ�Ļص�
*���룺	cszUserID				-�û���ʶ
*����� szNodeID				-�û�������ڵ��ʶ
*����ֵ���ɹ�����0��ʧ�ܷ���-1����ʾ���û�������
*****************************************************************************/
int AVCONCall::OnNETEC_NodeGetUserInfo(const char*cszUserID,char*szNodeID)
{
	MapPendingCall::iterator iter=m_MapPendingCall.find(cszUserID);
	if (iter!=m_MapPendingCall.end())
	{
		strcpy(szNodeID,iter->second.strNodeID.c_str());
		return 0;
	}

	return -1;	
}

/******************************************************************************
* OnIMXEC_CallRingIn
*�����������¼������ն˺��б�����ʱ��ϵͳ����ô��¼�
*���룺	cszCallID		-���б�ʶ
		cszUserID		-�û���ʶ���û�IP
		cszUserName		-�û�����
*�������
*����ֵ����
*****************************************************************************/
void AVCONCall::OnIMXEC_CallRingIn(const char*cszCallID,const char*cszUserID,const char*cszUserName)
{
	{
		MapPendingCall::iterator iter=m_MapPendingCall.find(cszUserID);
		if (iter!=m_MapPendingCall.end())
		{
			HangupCall(cszCallID);

			return;
		}
	}

	{
		PENDING_CALL tPendingCall;
		tPendingCall.nTimestamp=XGetTimestamp();
		tPendingCall.strCallID=cszCallID;
		tPendingCall.strNodeID="";
		tPendingCall.strUserID=cszUserID;

		m_MapPendingCall[tPendingCall.strUserID]=tPendingCall;
	}

	MapAVCONCall::iterator iter=m_MapAVCONCall.begin();
	while (iter!=m_MapAVCONCall.end())
	{
		iter->second->OnIMXEC_AVCONCallRingIn(cszUserID,cszUserName);
		++iter;
	}
}

/******************************************************************************
* OnIMXEC_CallRingOut
*�����������¼�������������гɹ���ϵͳ����ô��¼�
*���룺	cszCallID		-���б�ʶ
		cszUserID		-�û���ʶ���û�IP
		cszUserName		-�û�����
*�������
*����ֵ����
*****************************************************************************/
void AVCONCall::OnIMXEC_CallRingOut(const char*cszCallID,const char*cszUserID,const char*cszUserName)
{
	MapAVCONCall::iterator iter=m_MapAVCONCall.begin();
	while (iter!=m_MapAVCONCall.end())
	{
		iter->second->OnIMXEC_AVCONCallRingOut(cszUserID,cszUserName);
		++iter;
	}
}

/******************************************************************************
* OnIMXEC_CallEstablished
*���������н����¼�����˫������ͨ���󡾱���������Զ�˵ĺ��������Զ�˽����˱����ĺ�������ϵͳ����ô��¼�
*���룺	cszCallID		-���б�ʶ
		cszUserID		-�û���ʶ���û�IP
		cszUserName		-�û�����
*�������
*����ֵ����
*****************************************************************************/
void AVCONCall::OnIMXEC_CallEstablished(const char*cszCallID,const char*cszUserID,const char*cszUserName)
{
	MapAVCONCall::iterator iter=m_MapAVCONCall.begin();
	while (iter!=m_MapAVCONCall.end())
	{
		iter->second->OnIMXEC_AVCONCallEstablished(cszUserID,cszUserName);
		++iter;
	}
}

/******************************************************************************
* OnIMXEC_CallDestroyed
*���������������¼�����ͨ�������󡾱����Ҷϻ�Զ�˹Ҷϡ�ϵͳ����ô��¼�
*���룺	cszCallID		-���б�ʶ
		cszUserID		-�û���ʶ���û�IP
		cszUserName		-�û�����
		nReason			-����ԭ��
*�������
*����ֵ����
*****************************************************************************/
void AVCONCall::OnIMXEC_CallDestroyed(const char*cszCallID,const char*cszUserID,const char*cszUserName,IMXEC_CALL_END_REASON nReason)
{
	{
		MapPendingCall::iterator iter=m_MapPendingCall.find(cszUserID);
		if (iter==m_MapPendingCall.end())
		{
			return;
		}

		if (iter->second.strCallID!=cszCallID)
		{
			return;
		}

		m_MapPendingCall.erase(iter);
	}

	IMXEC_AVCONCall::CALL_DESTROYED_REASON nNewReason=IMXEC_AVCONCall::CDR_UNKOWN;
	if (nReason==IMXEC_CALL_END_BY_TIMEOUT)
	{
		nNewReason=IMXEC_AVCONCall::CDR_TIMEOUT;
	}
	else if (nReason==IMXEC_CALL_END_BY_REMOTE_HANGUP)
	{
		nNewReason=IMXEC_AVCONCall::CDR_HANGUP_BY_REMOTE;
	}
	else if (nReason==IMXEC_CALL_END_BY_LOCAL_HANGUP)
	{
		nNewReason=IMXEC_AVCONCall::CDR_HANGUP_BY_LOCAL;
	}

	MapAVCONCall::iterator iter=m_MapAVCONCall.begin();
	while (iter!=m_MapAVCONCall.end())
	{
		iter->second->OnIMXEC_AVCONCallDestroyed(cszUserID,cszUserName,nNewReason);
		++iter;
	}
}
