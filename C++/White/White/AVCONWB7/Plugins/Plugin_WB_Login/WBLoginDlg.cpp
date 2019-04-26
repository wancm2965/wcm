#include "stdafx.h"
#include "WBLoginDlg.h"
#include "ErrorCode.h"
#include "RWIniFile.h"
#include "IPlugin/IU7_MessageBox/IPlugin_U7T_MessageBox.h"
#include "IPlugin/Plugin_WB_Meeting/IPlugin_WB_Meeting.h"
#include "IPlugin/Plugin_Meeting_Member/IPlugin_Meeting_Member.h"
#include "IPlugin/IU7T_TipWindow/IU7T_TipWindow.h"
#define  TIME_START 1005
#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

CWBLoginDlg::CWBLoginDlg( void )
{
	m_pEditServe = NULL;
	m_pEditPassWord = NULL;
	m_pEditUserName = NULL;
	m_pBtnNetStatus = NULL;
	m_strServer = _T("DEFAULT_SERVER_NAME");
	m_strUserName = _T("");
	m_strPassWord = _T("");
	m_bLogout = false;
	m_indexUserInfoPos = 0;
	m_strLanguageFile = _T("");
	m_pLblTitle = NULL;
	m_bIsLoginCancle = true;

}

CWBLoginDlg::~CWBLoginDlg( void )
{
	IPlugin_U7_IM_Login *pU7Login = static_cast<IPlugin_U7_IM_Login*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_IM_Login"));
	if (pU7Login != NULL)
	{
		pU7Login->DelObserver(this);
	}
}

void CWBLoginDlg::OnConnectStatus( unsigned long ulStatus, int nErrCode )
{
	CString strConnect;
	strConnect = ReadConfigText(_T("PluginText"), _T("LOGIN_Connecting"));
	if (strConnect.IsEmpty())
	{
		strConnect = _T("正在连接...");
	}
	CString strReConnect;
	strReConnect = ReadConfigText(_T("PluginText"), _T("LOGIN_ReConnect"));
	if (strReConnect.IsEmpty())
	{
		strReConnect = _T("重新连接...");
	}
	CString strServerBusy;
	strServerBusy = ReadConfigText(_T("PluginText"), _T("LOGIN_ServerBusy"));
	if (strServerBusy.IsEmpty())
	{
		strServerBusy = _T("服务器忙");
	}
	CString strConnectFail;
	strConnectFail = ReadConfigText(_T("PluginText"), _T("LOGIN_ConnectFail"));
	if (strConnectFail.IsEmpty())
	{
		strConnectFail = _T("连接失败");
	}
	CString strConnectCut;
	strConnectCut = ReadConfigText(_T("PluginText"), _T("LOGIN_ConnectCut"));
	if (strConnectCut.IsEmpty())
	{
		strConnectCut = _T("连接断开");
	}
	CString strStatus = _T("");
	switch(ulStatus)
	{
	case SERVER_CONNECTING:
		strStatus = strConnect;
		break;
	case SERVER_RECONNECTED:
		strStatus = strReConnect;
		break;
	case SERVER_BUSY:
		strStatus = strConnectCut;
		if (!m_bLogout)
		{
			strStatus = strConnectCut + CErrorCode::Instance().GetErrCodeStr((unsigned long) nErrCode);
			IPlugin_Meeting_Member* pluginMeetingMember= dynamic_cast<IPlugin_Meeting_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_Meeting_Member"));
			if (pluginMeetingMember != NULL)
			{
				pluginMeetingMember->DestroyUI();
			}	
			OnLogout(nErrCode);
			m_bLogout = true;

		}
		break;
	case SERVER_FAILED:
		strStatus = strConnectFail;
		break;
	case SERVER_DISCONNECTED:
		{
			
		}
		break;
	case SERVER_CONNECTED:
		{
			m_bLogout = false;
		}
		break;
	default:
		return;
	}
	//m_pBtnNetStatus->SetText(strStatus);
	if (m_pLblTitle)
	{
		m_pLblTitle->SetText(strStatus);
	}
}

void CWBLoginDlg::OnLogin( int nErrCode , HRESULT hr)
{
 	if (nErrCode != ERR_NO)
	{
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("提示");
		}
		CString strReLogin;
		strReLogin = ReadConfigText(_T("PluginText"), _T("LOGIN_ReLogin"));
		if (strReLogin.IsEmpty())
		{
			strReLogin = _T("重新登录");
		}
		//登录失败
		m_pBtnNetStatus->SetText(strReLogin);
		ShowWindow(SW_SHOW);
		CString strError = _T("");
		strError = CErrorCode::Instance().GetErrCodeStr((unsigned long)nErrCode);
// 		IPlugin_U7T_TipWindow* pPluginTipWindow = dynamic_cast<IPlugin_U7T_TipWindow*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_TipWindow"));
// 		if (pPluginTipWindow)
// 		{
// 			pPluginTipWindow->ShowTipWindow(strError,3,this->GetHWND());
// 		}
		IPlugin_U7T_MessageBox *pMesssgeBox = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
		if (pMesssgeBox == NULL)
			return;
		pMesssgeBox->MessageBox(m_hWnd,strPrompt,strError,MB_OK);

		CString strTip = _T("");
		m_pLblTitle->SetText(strTip);
		m_bIsLoginCancle = true;
		return;

	}
	if (m_pEditPassWord == NULL || m_pEditServe == NULL || m_pEditUserName == NULL)
	{
		return;
	}
	//保存服务器和用户名：
	SaveLoginInfo();


	IPlugin_WB_Meeting* pluginMeeting= dynamic_cast<IPlugin_WB_Meeting*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Meeting"));
	if (pluginMeeting != NULL)
	{
 		pluginMeeting->ShowUIMeetingList(true);
 		OnHideDlg(false);//隐藏登录窗口
	}
}
//注销的响应函数，根据错误的类型，弹出错误提示框
void CWBLoginDlg::OnLogout( int nErrCode )
{
	if (nErrCode != ERR_NO)
	{
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("提示");
		}
		CString strError = _T("");
		strError = CErrorCode::Instance().GetErrCodeStr((unsigned long)nErrCode);
		
		IPlugin_U7T_TipWindow* pPluginTipwindow = dynamic_cast<IPlugin_U7T_TipWindow*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_TipWindow"));
		if (pPluginTipwindow)
		{
			pPluginTipwindow->ShowTipWindow(strError,3,this->GetHWND());
		}
// 		IPlugin_U7T_MessageBox *pMesssgeBox = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
// 		if (pMesssgeBox == NULL)
// 			return;
// 		pMesssgeBox->MessageBox(m_hWnd,strPrompt,strError,MB_OK);
	}
// 	else
// 	{
		IPlugin_WB_Meeting* pluginMeeting= dynamic_cast<IPlugin_WB_Meeting*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Meeting"));
		if (pluginMeeting != NULL)
		{
			pluginMeeting->ShowUIMeetingList(false);
		}	
		ShowWindow();
//	}
	CString strReLogin;
	strReLogin = ReadConfigText(_T("PluginText"), _T("LOGIN_ReLogin"));
	if (strReLogin.IsEmpty())
	{
		strReLogin = _T("重新登录");
	}
	m_pBtnNetStatus->SetText(strReLogin);
	m_bIsLoginCancle = true;
	
}

void CWBLoginDlg::Init()
{
	IPlugin_U7_IM_Login* pU7Login = static_cast<IPlugin_U7_IM_Login*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_IM_Login"));
	if (pU7Login == NULL)
		return;
	pU7Login->AddObserver(this);
	
// 	IPlugin_U7_MM_Room* pPlugin_Room = dynamic_cast<IPlugin_U7_MM_Room*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room"));
// 	if (pPlugin_Room)
// 	{
// 		pPlugin_Room->AddObserver(this);
// 	}
	//关联编辑框
	m_pEditServe = static_cast<CRichEditUI*>(paint_manager_.FindControl(_T("EditServer")));
	if (m_pEditServe == NULL)
		return;
	m_pEditServe->OnEvent += MakeDelegate(this,&CWBLoginDlg::OnEditEvent);
	m_pEditUserName = static_cast<CRichEditUI*>(paint_manager_.FindControl(_T("EditAccount")));
	if (m_pEditUserName == NULL)
		return;
	m_pEditUserName->OnEvent += MakeDelegate(this,&CWBLoginDlg::OnEditEvent);
	m_pEditPassWord = static_cast<CRichEditUI*>(paint_manager_.FindControl(_T("EditPassword")));
	if (m_pEditPassWord == NULL)
		return;
	m_pEditPassWord->OnEvent += MakeDelegate(this,&CWBLoginDlg::OnEditEvent);

 	m_pBtnNetStatus = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("btnNetLogin")));
 	if (m_pBtnNetStatus == NULL)
 		return;
	m_pLblTitle = static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LabMessage")));
	if (m_pLblTitle == NULL)
		return;
	CString strTip = _T("");
	m_pLblTitle->SetText(strTip);

	//初始化登录框参数
	m_pEditServe->SetText(DEFAULT_SERVER_NAME);
	m_pEditServe->SetLimitText(30);
	m_pEditUserName->SetLimitText(30);
	m_pEditPassWord->SetLimitText(30);

	InitLoginInfo();//读取配置文件初始化
	//设置关标位置
	if (m_strUserName.IsEmpty())
	{
		m_pEditServe->SetFocus();
		
	}
	else if (m_strPassWord.IsEmpty())
	{
		m_pEditUserName->SetFocus();
	}
	
	CErrorCode::Instance().InitErrorCode();

}

LPCTSTR CWBLoginDlg::GetWindowClassName() const
{
	return _T("WBLoginDlg");

}

CString CWBLoginDlg::GetSkinFile()
{
	GetSkinPath();
	return m_strLanguageFile;
	//return _T("WBLoginDlg.xml");
}

void CWBLoginDlg::Notify( TNotifyUI& msg )
{
	if (COUI_MSGTYPE_CLICK == msg.nType)
	{
		if (_tcsicmp(msg.pSender->GetName(),_T("btnNetLogin")) == 0)
		{
			if (m_bIsLoginCancle)
			{
				OnLoginNet();
			}
			else
			{
				OnCancleLogin();
			}
			m_bIsLoginCancle = !m_bIsLoginCancle;
			
		}
		else if (_tcsicmp(msg.pSender->GetName(),_T("btnClose")) == 0)
		{
			OnHideDlg();
		}
	}
					
}

LRESULT CWBLoginDlg::OnKeyDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			if (VK_TAB == wParam)
			{
				if (m_pEditServe->IsFocused())
				{					
					m_pEditUserName->SetFocus();
					if (m_pEditUserName->GetText().GetLength() > 0)
					{
						m_pEditUserName->SetSel(0,-1);
					}
				}
				else if (m_pEditUserName->IsFocused())
				{
					m_pEditPassWord->SetFocus();
					if (m_pEditPassWord->GetText().GetLength() > 0)
					{
						m_pEditPassWord->SetSel(0,-1);
					}
				}
				else if (m_pEditPassWord->IsFocused())
				{
					m_pEditServe->SetFocus();
					if (m_pEditServe->GetText().GetLength() > 0)
					{
						m_pEditServe->SetSel(0,-1);
					}
				}
				return 0;
			}

			//保留一些快捷键
// 			if (m_pEditServe->IsFocused() && (GetKeyState(VK_CONTROL) & 0xFF00) == 0xFF00)
// 			{
// 				//全选
// 				if (wParam == 'A' || wParam == 'a')
// 				{
// 					m_pEditServe->SetSel(0,-1);
// 					return 0;
// 				}
// 				//拷贝
// 				if (wParam == 'C'|| wParam == 'c')
// 				{
// 					m_pEditServe->Copy();
// 					return 0;
// 				}
// 				//剪切
// 				if (wParam == 'X' || wParam == 'x' )
// 				{
// 					m_pEditServe->Cut();
// 					return 0;
// 				}
// 				//撤销
// 				if (wParam == 'Z'|| wParam == 'z')
// 				{
// 					m_pEditServe->Undo();
// 					return 0;
// 				}
// 
// 			}
			//使用回车键
			if (VK_RETURN == wParam)
			{
				ResponseDefaultKeyEvent(wParam);
				return 0;

			}
			break;

		}
		
	}
	return WindowImplBase::OnKeyDown(uMsg,wParam,lParam,bHandled);


}

LRESULT CWBLoginDlg::ResponseDefaultKeyEvent( WPARAM wParam )
{
	if (wParam == VK_RETURN)
	{
		OnLoginNet();	
	}
	else if (wParam == VK_ESCAPE)
	{
		OnCancleLogin();
	}
	m_bIsLoginCancle = !m_bIsLoginCancle;
	return FALSE;

}

void CWBLoginDlg::OnLoginNet( void )
{
	if (!CheckData())
		return;
	

	IPlugin_U7_IM_Login *pU7Login = static_cast<IPlugin_U7_IM_Login*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_IM_Login"));
	if (pU7Login == NULL)
		return;

	CString strOnlineLogin;
	strOnlineLogin = ReadConfigText(_T("PluginText"), _T("LOGIN_OnlineCancle"));
	if (strOnlineLogin.IsEmpty())
	{
		strOnlineLogin = _T("取消登录");
	}
	m_pBtnNetStatus->SetText(strOnlineLogin);
	pU7Login->Login(U2A(m_strServer).c_str(),4222,U2A(m_strUserName).c_str(),U2A(m_strPassWord).c_str(),"online","SINGLEWB");

}
void  CWBLoginDlg::OnCancleLogin(void)
{
	::ReleaseCapture();
	DisConnect();
	CString strOnlineLogin = _T("");
	m_pLblTitle->SetText(strOnlineLogin);
	strOnlineLogin = ReadConfigText(_T("PluginText"), _T("LOGIN_ReLogin"));
	if (strOnlineLogin.IsEmpty())
	{
		strOnlineLogin = _T("重新登录");
	}
	m_pBtnNetStatus->SetText(strOnlineLogin);

}

void CWBLoginDlg::OnHideDlg(bool bflag /*= true*/)
{
	ShowWindow(false,false);
	::ReleaseCapture();
	if (bflag)
	{
		DisConnect();
	}
	CString strOnlineLogin;
	strOnlineLogin = ReadConfigText(_T("PluginText"), _T("LOGIN_OnlineLogin"));
	if (strOnlineLogin.IsEmpty())
	{
		strOnlineLogin = _T("联机登录");
	}
	m_pBtnNetStatus->SetText(strOnlineLogin);

}

BOOL CWBLoginDlg::CheckData( void )
{
	m_strServer = (LPCTSTR)m_pEditServe->GetText();
	m_strUserName = (LPCTSTR)m_pEditUserName->GetText();
	m_strPassWord = (LPCTSTR)m_pEditPassWord->GetText();

	CString strPrompt;
	strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
	if (strPrompt.IsEmpty())
	{
		strPrompt = _T("提示");
	}
	IPlugin_U7T_TipWindow* pPluginTipWindow = dynamic_cast<IPlugin_U7T_TipWindow*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_TipWindow"));
	if (m_strServer.IsEmpty())
	{
		//IPlugin_U7D_MessageBox
// 		IPlugin_U7T_MessageBox *pMesssgeBox = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
// 		if (pMesssgeBox == NULL)
// 			return FALSE;
		CString strServerNotNull;
		strServerNotNull = ReadConfigText(_T("PluginText"), _T("LOGIN_ServerNotNull"));
		if (strServerNotNull.IsEmpty())
		{
			strServerNotNull = _T("服务器地址不能为空");
		}
		/*pMesssgeBox->MessageBox(this->m_hWnd,strPrompt,strServerNotNull,MB_OK);*/
		if (pPluginTipWindow == NULL)
		{
			return FALSE;
		}
		pPluginTipWindow->ShowTipWindow(strServerNotNull,3,this->GetHWND());
		m_pEditServe->SetFocus();
		return FALSE;
	}
	else
	{
		if (!CheckServerIP(m_strServer))
		{
			m_pEditServe->SetFocus();
			return FALSE;
		}
	}
	if (m_strUserName.IsEmpty())
	{
// 		IPlugin_U7T_MessageBox *pMesssgeBox = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
// 		if (pMesssgeBox == NULL)
// 			return FALSE;
		CString strNameNotNull;
		strNameNotNull = ReadConfigText(_T("PluginText"), _T("LOGIN_NameNotNull"));
		if (strNameNotNull.IsEmpty())
		{
			strNameNotNull = _T("账户名称不能为空");
		}
		//pMesssgeBox->MessageBox(this->m_hWnd,strPrompt,strNameNotNull,MB_OK);
		if (pPluginTipWindow == NULL)
		{
			return FALSE;
		}
		pPluginTipWindow->ShowTipWindow(strNameNotNull,3,this->GetHWND());
		m_pEditUserName->SetFocus();
		return FALSE;
	}
	else
	{
		char temp = m_strUserName.GetAt(m_strUserName.GetLength() - 1);
			if (m_strUserName.GetLength() < 2)
			{
// 				IPlugin_U7T_MessageBox *pMesssgeBox = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
// 				if (pMesssgeBox == NULL)
// 					return FALSE;
				CString strNameMin2;
				strNameMin2 = ReadConfigText(_T("PluginText"), _T("LOGIN_NameMin2"));
				if (strNameMin2.IsEmpty())
				{
					strNameMin2 = _T("账户长度不能少于2位");
				}
				//pMesssgeBox->MessageBox(this->m_hWnd,strPrompt,strNameMin2,MB_OK);
				if (pPluginTipWindow == NULL)
				{
					return FALSE;
				}
				pPluginTipWindow->ShowTipWindow(strNameMin2,3,this->GetHWND());
				m_pEditUserName->SetFocus();
				return FALSE;

			}
			if (temp == '.' || temp == '_'|| temp == '-' || temp == '+')
			{
// 				IPlugin_U7T_MessageBox *pMesssgeBox = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
// 				if (pMesssgeBox == NULL)
// 					return FALSE;
				CString strNameNotEnd;
				strNameNotEnd = ReadConfigText(_T("PluginText"), _T("LOGIN_NameNotEnd"));
				if (strNameNotEnd.IsEmpty())
				{
					strNameNotEnd = _T("账户不能以“.”、“—”、“_”结尾！");
				}
				//pMesssgeBox->MessageBox(this->m_hWnd,strPrompt,strNameNotEnd,MB_OK);
				if (pPluginTipWindow == NULL)
				{
					return FALSE;
				}
				pPluginTipWindow->ShowTipWindow(strNameNotEnd,3,this->GetHWND());
				m_pEditUserName->SetFocus();
				return FALSE;
			}
			if (m_strPassWord.IsEmpty())
			{
// 				IPlugin_U7T_MessageBox *pMesssgeBox = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
// 				if (pMesssgeBox == NULL)
// 					return FALSE;
				CString strCodeNotNull;
				strCodeNotNull = ReadConfigText(_T("PluginText"), _T("LOGIN_CodeNotNull"));
				if (strCodeNotNull.IsEmpty())
				{
					strCodeNotNull = _T("输入密码不能为空");
				}
				//pMesssgeBox->MessageBox(this->m_hWnd,strPrompt,strCodeNotNull,MB_OK);
				if (pPluginTipWindow == NULL)
				{
					return FALSE;
				}
				pPluginTipWindow->ShowTipWindow(strCodeNotNull,3,this->GetHWND());
				m_pEditPassWord->SetFocus();
				return FALSE;
			}
			return TRUE;
	}

}

BOOL CWBLoginDlg::CheckServerIP( CString IPAddr )
{
	char ch[]=".";   
	char *token,*dot[10];   
	int iplen = 0, digit = 0, isdig = 0;  

	char   szTemp[500]; 
	memset(szTemp,0,sizeof(szTemp)); 
	strcpy_s(szTemp, U2A(IPAddr).c_str());

	token =strtok(szTemp,ch);//用.分割字符串   
	while(token!=NULL)
	{   
		dot[iplen]=token;   
		iplen++;   
		token=strtok(NULL,ch);   
	}  
	IPlugin_U7T_TipWindow* pPluginTipWindow = dynamic_cast<IPlugin_U7T_TipWindow*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_TipWindow"));
	for(int i=0; i < iplen; i++)   
	{ 
		char* strTemp = dot[i];
		int len = strlen(strTemp);
		for(int j = 0; j < len; j++)   
		{  
			if (isdigit(strTemp[j]))
			{
				isdig++;
			}
		}
		if (len == isdig)
		{
			digit++;
		}
		else
		{
// 			CString strPrompt;
// 			strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
// 			if (strPrompt.IsEmpty())
// 			{
// 				strPrompt = _T("提示");
// 			}
			CString strIPWrong;
			strIPWrong = ReadConfigText(_T("PluginText"), _T("LOGIN_IPWrong"));
			if (strIPWrong.IsEmpty())
			{
				strIPWrong = _T("IP填写不正确！！");
			}
			if (pPluginTipWindow == NULL)
			{
				return FALSE;
			}
// 			IPlugin_U7T_MessageBox *pMesssgeBox = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
// 			if (pMesssgeBox == NULL)
// 				return FALSE;
// 			pMesssgeBox->MessageBox(this->m_hWnd,strPrompt,strIPWrong,MB_OK);
			pPluginTipWindow->ShowTipWindow(strIPWrong,3,this->GetHWND());
			return FALSE;

		}
		isdig = 0;
	}
	if (digit == iplen)
	{
// 		CString strPrompt;
// 		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
// 		if (strPrompt.IsEmpty())
// 		{
// 			strPrompt = _T("提示");
// 		}
		CString strIPWrong;
		strIPWrong = ReadConfigText(_T("PluginText"), _T("LOGIN_IPWrong"));
		if (strIPWrong.IsEmpty())
		{
			strIPWrong = _T("IP填写不正确！！");
		}
		if (digit != 4)
		{
// 			IPlugin_U7T_MessageBox *pMesssgeBox = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
// 			if (pMesssgeBox == NULL)
// 				return FALSE;
// 			pMesssgeBox->MessageBox(this->m_hWnd,strPrompt,strIPWrong,MB_OK);
			if (pPluginTipWindow == NULL)
			{
				return FALSE;
			}
			pPluginTipWindow->ShowTipWindow(strIPWrong,3,this->GetHWND());
			return FALSE;
		}

		if((string(dot[0]).length())>3||(string(dot[1]).length())>3||(string(dot[2]).length())>3||(string(dot[3]).length())>3)
		{
// 			IPlugin_U7T_MessageBox *pMesssgeBox = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
// 			if (pMesssgeBox == NULL)
// 				return FALSE;
// 			pMesssgeBox->MessageBox(this->m_hWnd,strPrompt,strIPWrong,MB_OK);
			if (pPluginTipWindow == NULL)
			{
				return FALSE;
			}
			pPluginTipWindow->ShowTipWindow(strIPWrong,3,this->GetHWND());
			return FALSE;

		}
		else if(atoi(dot[0]) > 255 || atoi(dot[1]) > 255 || atoi(dot[2]) > 255 || atoi(dot[3]) > 255 || atoi(dot[0]) == 0 || atoi(dot[3]) == 0)
		{

// 			IPlugin_U7T_MessageBox *pMesssgeBox = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
// 			if (pMesssgeBox == NULL)
// 				return FALSE;
// 			pMesssgeBox->MessageBox(this->m_hWnd,strPrompt,strIPWrong,MB_OK);
			if (pPluginTipWindow == NULL)
			{
				return FALSE;
			}
			pPluginTipWindow->ShowTipWindow(strIPWrong,3,this->GetHWND());
			return FALSE;
		}
	}
	return TRUE;

}

void CWBLoginDlg::SaveLoginInfo()
{
	CString strFilePath = _T("");
	CString SectionName = _T("");
	strFilePath = GetSaveLoginPath();

	SectionName = SELECTIONNAME;

	CRWIniFile::WriteOptionConfigStr(SectionName,SEVER_KEYNAME,m_strServer,strFilePath);
	CRWIniFile::WriteOptionConfigStr(SectionName,USER_KEYNAME,m_strUserName,strFilePath);
	//CRWIniFile::WriteOptionConfigStr(SectionName,PASSWORD_KEYNAME,m_strPassWord,strFilePath);
	
}

void CWBLoginDlg::InitLoginInfo()
{
	CString strFilePath = _T("");
	strFilePath = GetSaveLoginPath();

	CString SectionName = SELECTIONNAME;
	CString strDef = _T("");
	CString strServer = _T("");
	CString strUserName = _T("");
	CString strPassWord = _T("");
	strServer = CRWIniFile::ReadOptionConfigStr(SectionName,SEVER_KEYNAME,strDef,strFilePath);
	strUserName = CRWIniFile::ReadOptionConfigStr(SectionName,USER_KEYNAME,strDef,strFilePath);
	//strPassWord = CRWIniFile::ReadOptionConfigStr(SectionName,PASSWORD_KEYNAME,strDef,strFilePath);
	if (strServer != _T("") && m_pEditServe)
	{
		m_strServer = strServer;
		m_pEditServe->SetText(strServer);
		m_pEditUserName->SetFocus();

	}
	if (strUserName != _T("") && m_pEditUserName)
	{
		m_strUserName = strUserName;
		m_pEditUserName->SetText(strUserName);
		m_pEditPassWord->SetFocus();

	}
}

CString CWBLoginDlg::GetSaveLoginPath()
{

	CString strpath = _T("");
	CString strFilePath = _T("");
	int nValue = 0;
	nValue = GetPrivateProfileInt(_T("AVCON_SELECTED"),
		_T("AVCONXPE"),
		0,
		_T("C:\\Program Files\\avcon\\ExePath.ini"));
	if (1 == nValue)
	{
		CString strpath = _T("");
		strpath = _T("D:\\AVCON6_XPE");

		//判断资源库文件夹是否存在，没有则创建
		if(!PathIsDirectory(strpath))
		{
			if(!CreateDirectory(strpath, NULL))
			{
				Reboot();
			}
		}
		strFilePath = strpath + _T("\\") + CONFIG_FILEPATH_OF_LOGIN;
		return strFilePath;

	}
	else
	{
		strpath = GetExePath();
		if (!strpath.IsEmpty())
		{
			CString str = _T("");
			strpath += _T("config");
			str = strpath + _T("\\");
			strFilePath = str + CONFIG_FILEPATH_OF_LOGIN;

			return strFilePath;
		}
	}

	
	return _T("");

}

CButtonUI* CWBLoginDlg::GetButtonUI()
{
	if (m_pBtnNetStatus != NULL)
	{
		return m_pBtnNetStatus;

	}
	return NULL;

}

void CWBLoginDlg::DisConnect()
{
	IPlugin_U7_IM_Login *pluginLogin = static_cast<IPlugin_U7_IM_Login*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_IM_Login"));
	if (pluginLogin !=NULL)
	{
		pluginLogin->Disconnect();
	}

}

void CWBLoginDlg::GetSkinPath()
{
	CString strCfgFilePath = GetExePath() + _T("\U7Config.ini");
	wchar_t strValue[MAX_PATH] = {0};
	::GetPrivateProfileString(_T("LANGUAGE"), _T("Language"), _T("cs"), strValue, MAX_PATH, strCfgFilePath);
	CString str  = _T("_WBLoginDlg.xml");
	m_strLanguageFile = strValue	+ str;
}

bool CWBLoginDlg::OnEditEvent( void* p )
{
	TEventUI *pEvent = static_cast<TEventUI*>(p);
	if (pEvent)
	{
		if (pEvent->Type == UIEVENT_KILLFOCUS)
		{
			CRichEditUI* pControl = static_cast<CRichEditUI*>(pEvent->pSender);
			if (pControl != NULL) pControl->SetSelNone();
		}
	}
	return true;
}

