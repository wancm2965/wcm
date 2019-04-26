// WBLoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_WB_Main.h"
#include "WBLoginDlg.h"
#include "RWIniFile.h"
#include <atltime.h>
//#include "FindFile.h"
#include "ISystem/ISystem.h"
#include "ISystem/IPlugin.h"
/*#include "IPlugin/IU7D_MCUSet/IPlugin_U7D_MCUSet.h"*/
#include "IPlugin/IU7_IM/IPlugin_U7_IM_Login.h"
#include "IPlugin/WB_Main/IPlugin_WB_Main.h"
#include "WBMainDlg.h"

// CWBLoginDlg 对话框

IMPLEMENT_DYNAMIC(CWBLoginDlg, CDialog)

CWBLoginDlg::CWBLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWBLoginDlg::IDD, pParent)
	,m_pBitmap(NULL)
	, m_strLoginID(_T(""))
	, m_strPassword(_T(""))
	//, m_strLoginID(_T(""))
	, m_strServerHost(_T(""))
	, m_pMeetingListDlg(NULL)
	, m_indexUserInfoPos(0)
	, m_bNeedToCheckTouch(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	InitLoginUserInfoStruct();
	m_pBitmap = NULL;

}

CWBLoginDlg::~CWBLoginDlg()
{
	if (m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	}

	DeleteMeetingListDlg();

	IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
	if (pPlugin_Login == NULL)
	{
		return ;
	}
	pPlugin_Login->DelObserver(this);
	
}

void CWBLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLOSELOGINDLG_BTN, m_closeLoginDlgBtn);
	DDX_Control(pDX, IDC_LOGINID_COMBOX, m_longinIDCombox);
	DDX_Control(pDX, IDC_SERVERADDHOST_EDIT, m_serverHostEdit);
	DDX_Text(pDX, IDC_SERVERADDHOST_EDIT, m_strServerHost);
	DDV_MaxChars(pDX, m_strServerHost, 32);
	DDX_Control(pDX, IDC_LOGINPASSWORD_EDIT, m_passwordEdit);
	DDX_Text(pDX, IDC_LOGINPASSWORD_EDIT, m_strPassword);
	DDX_Control(pDX, IDC_CANCEL_BTN, m_cancelBtn);
	DDX_Control(pDX, IDC_SINGLELAND_BTN, m_singleLandBtn);
	DDX_CBString(pDX, IDC_LOGINID_COMBOX, m_strLoginID);
	DDX_Control(pDX, IDC_SHOWLANDMESSAGE_STATIC, m_showLandMessageSTATIC);
	DDX_Control(pDX, IDC_ONLINE_BTN, m_onlineBtn);
	DDX_Control(pDX, IDC_CHECKMTFUNCTION, m_CheckMTFunctionBtn);
}

BEGIN_MESSAGE_MAP(CWBLoginDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ONLINE_BTN, &CWBLoginDlg::OnBnClickedOnlineBtn)
	ON_BN_CLICKED(IDC_SINGLELAND_BTN, &CWBLoginDlg::OnBnClickedSinglelandBtn)
	ON_BN_CLICKED(IDC_CLOSELOGINDLG_BTN, &CWBLoginDlg::OnBnClickedCloselogindlgBtn)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_LOGINID_COMBOX, &CWBLoginDlg::OnCbnSelchangeLoginidCombox)
	ON_BN_CLICKED(IDC_CANCEL_BTN, &CWBLoginDlg::OnBnClickedCancelBtn)
	ON_BN_CLICKED(IDC_CHECKMTFUNCTION, &CWBLoginDlg::OnBnClickedCheckMTFunctionBtn)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CWBLoginDlg 消息处理程序

void CWBLoginDlg::SetSkinImage( CString strPath )
{
	if ( NULL == m_pBitmap)
	{
		m_pBitmap =Bitmap::FromFile(strPath.GetBuffer());
	}
	else
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
		m_pBitmap = Bitmap::FromFile(strPath.GetBuffer());
	}

	if (NULL != m_pBitmap)
	{
		CRect rect;
		GetClientRect(&rect);
		CClientDC dc(this);
		Graphics graph(dc.GetSafeHdc());
		Rect Rt(0, 0, rect.Width(), rect.Height());
		graph.DrawImage(m_pBitmap, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);	
	}

}

void CWBLoginDlg::ChangeLoginOrCancel( void )
{
	//显示连线按钮，隐藏取消按钮

	m_onlineBtn.ShowWindow(SW_SHOW);
	m_cancelBtn.ShowWindow(SW_HIDE);

}

void CWBLoginDlg::OnConnectStatus( unsigned long ulStatus, int nErrCode )
{
	CString strStatus;
	switch (ulStatus)
	{
	case SERVER_CONNECTING:
		{
			strStatus = _T("正在连接...");
		}
		break;
	case SERVER_RECONNECTED:
		{
			strStatus = _T("重新连接...");
		}
		break;
	case SERVER_BUSY:
		{
			strStatus = _T("服务器忙");
		}
		break;
	case SERVER_FAILED:
		{
			strStatus =  _T("连接失败");
		}
		break;
	case SERVER_DISCONNECTED:
		{
			strStatus = _T("连接断开");
		}
		break;
	case SERVER_CONNECTED:
		{
			//strStatus = _T("已连接");
			strStatus = _T("");
		}
		break;
	default:
		break;

	}
	m_showLandMessageSTATIC.SetCaption(strStatus);

}

void CWBLoginDlg::OnLogin( int nErrCode )
{
	if (nErrCode != ERR_NO)
	{
		// 登陆失败
		ShowWindow(true);
		CString strError = CErrorCode::Instance().GetErrCodeStr((unsigned long)nErrCode);
		
		ShowLoginError((unsigned long)nErrCode);
		ChangeLoginOrCancel();
        DeleteMeetingListDlg();
	    return;
	}

	CWBMainDlg::g_pMainDlg->SetIsOnline(TRUE);
	//保存服务器和用户名：
	SaveLoginInfo();

/*	WB_CloseFuntion();//关闭单机下功能面板*/

	IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room *>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (pPlugin_Room == NULL)
	{
		return;
	}
	pPlugin_Room->LoadRoomList();//加了这句就有内存泄露
}

 void CWBLoginDlg::OnLogin( void )
 {
 	if ( !CheckData() )
 	{
 		return;
 	}

 	//保存服务器和用户名：
  	//SaveLoginInfo();
	IPlugin_U7_IM_Login* pPlugin = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
	if (pPlugin == NULL)
	{
		return ;
	}
 	pPlugin->Login(U2A(m_strServerHost).c_str(), 4222, U2A(m_strLoginID).c_str(), U2A(m_strPassword).c_str(), /*U2A(m_strLoginState).c_str()*/CMD_STR_USER_ONLINE);

	//创建会议列表对话框
	if (m_pMeetingListDlg != NULL)
	{
		DeleteMeetingListDlg();
	}
	m_pMeetingListDlg = new CWBMeetingListDlg;
	m_pMeetingListDlg->Create(CWBMeetingListDlg::IDD, this);
	m_pMeetingListDlg->ShowWindow(HIDE_WINDOW);

	WB_IsSuccessLoad(true);

//   	IPlugin_WB_Main* pPluginMain = (IPlugin_WB_Main*)g_System->GetPluginByName("Plugin_WB_Main");
//   	if (pPluginMain == NULL)
//  		return ;
 	//ShowWindow(false);
 	//pPluginMain->ShowWaitUI(true);	
 }

void CWBLoginDlg::OnLogout( int nErrCode )
{
	IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
	if (pPlugin_Function == NULL)
	{
		return ;
	}
	//	if (m_isOnline)
	{
		IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
		if (NULL == pPlugin_Room)
		{
			return ;
		}
		pPlugin_Room->ExitRoom();
		CWBMainDlg::g_pMainDlg->SetIsReceiveBoardcast(FALSE);
		if (CWBMainDlg::g_pMainDlg->GetIsNeedRealseTouchScreenAuthorize())//释放服务器触摸授权
		{
			pPlugin_Function->ReleaseTouchScreenAuthorize();
			CWBMainDlg::g_pMainDlg->SetIsNeedRealseTouchScreenAuthorize(FALSE);
		}
		//放开单机版的白板内授权，如果硬件支持
		bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //检测当前硬件是否支持多点触摸功能(包括对avcon指定的硬件触摸屏的检测）
		if (!bHardwareTouch) //硬件不支持触摸
		{
			WB_SetTouchAuthorization(FALSE);
		} 
		else
		{
			WB_SetTouchAuthorization(TRUE);
		}
		IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
		if (pPlugin_Login == NULL)
		{
			return ;
		}
		DeleteMeetingListDlg();
		pPlugin_Login->Logout();
		CWBMainDlg::g_pMainDlg->SetIsOnline(FALSE);
	}

	CString strItem;
	strItem =(CString)"AVCONWB电子白板";
	CWBMainDlg::g_pMainDlg->SetWindowText(strItem);//设置白板对话框的标题文字

	WB_CloseAllChildBox();//wangxiaobin
	CWBMainDlg::g_pMainDlg->SetHPDTSNotify(NULL);

	CWBMainDlg::g_pMainDlg->SetWBStatic();

	CWBMainDlg::g_pMainDlg->SetisLocalBoardCastWB(false);

	ChangeLoginOrCancel();
	ShowWindow(SW_SHOW);

	if (nErrCode == 20005)
	{
		CString strStatus;
		strStatus = _T("登陆失败！相同账户的用户在其他地方登陆！");
		m_showLandMessageSTATIC.SetCaption(strStatus);
	}

	CMenu * tempSubMenw;
	//文件子菜单	
	tempSubMenw = (CWBMainDlg::g_pMainDlg->GetMenu())->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_LOAD_BTN,MF_ENABLED);
}

BOOL CWBLoginDlg::CheckServerIP( CString IPAddr )
{
	char ch[]=".";   
	char *token,*dot[10];   
	int iplen = 0, digit = 0, isdig = 0;  

	char   szTemp[500]; 
	memset(szTemp,0,sizeof(szTemp)); 
	strcpy(szTemp, U2A(IPAddr).c_str());

	token =strtok(szTemp,ch);   
	while(token!=NULL)
	{   
		dot[iplen]=token;   
		iplen++;   
		token=strtok(NULL,ch);   
	}  

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
		isdig = 0;
	}
	if (digit == iplen)
	{
		if (digit != 4)
		{
			MessageBox(_T("IP填写不正确！"));
			return FALSE;
		}
		else if(atoi(dot[0]) > 255 || atoi(dot[1]) > 255 || atoi(dot[2]) > 255 || atoi(dot[3]) > 255 || atoi(dot[0]) == 0 || atoi(dot[3]) == 0)
		{
			MessageBox(_T("IP填写不正确！"));
			return FALSE;
		}
	}
	return TRUE;

}

BOOL CWBLoginDlg::CheckData( void )
{

	if ( m_strServerHost.IsEmpty()  )
	{
		MessageBox(_T("服务器地址不能为空！"));
		m_serverHostEdit.SetFocus();
		return FALSE;
	}
	else
	{
		if(!CheckServerIP(m_strServerHost))
		{
			m_serverHostEdit.SetFocus();
			return FALSE;
		}
	}

	if ( m_strLoginID.IsEmpty() )
	{
		MessageBox(_T("账号不能为空！"));
		m_longinIDCombox.SetFocus();
		return FALSE;
	}
	else 
	{
		char temp = m_strLoginID.GetAt(m_strLoginID.GetLength() - 1);
		if (m_strLoginID.GetLength() < 2)
		{
			MessageBox(_T("账户名长度不能少于2位！"));
			m_longinIDCombox.SetFocus();
			return FALSE;
		}
		if (temp == '.' || temp == '_' || temp == '-')
		{
			MessageBox(_T("账户名不能以“.”、“—”、“_”结尾！"));
			m_longinIDCombox.SetFocus();
			return FALSE;
		}
	}
	// 	if ( m_strAccount.GetLength() > 16 || m_strAccount.GetLength() < 4 )
	// 	{
	// 		UCCMessageBox(this, _T("用户名长度在4-16位！"));
	// 		m_editAccount.SetFocus();
	// 		return FALSE;
	// 	}
	if ( m_strPassword.IsEmpty() )
	{
		MessageBox(_T("密码不能为空！"));
		m_passwordEdit.SetFocus();
		return FALSE;
	}
	

	//修改用户名相同但更换服务器地址时机构加载不正确 2011.09.15 niezhiren
	/*USES_CONVERSION;
	CString csDBFile = _T("");
	std::string strRootDBFile = W2A(GetExePath()) + string("AVCON6.db");
	csDBFile.Format(_T("%suser\\%s"), GetExePath(), A2U(GetDataHandle().GetUserID()));
	std::string strDBFile = W2A(csDBFile);
	if (CFileMgr::FileOrDirectoryExist(strDBFile.c_str()))
	{
	CString strTemp = _T("");
	CString strDest = _T("");
	m_editServerIP.GetWindowText(strTemp);
	csDBFile.Format(_T("%suser\\%s\\%s"), GetExePath(), A2U(GetDataHandle().GetUserID()), LOGIN_CONFIG_FILE);
	strDest = CRWIniFile::ReadOptionConfigStr(SERVER_SECTION, SERVER_NAME, DEFAULT_SERVER_NAME, csDBFile);
	if (StrCmp(strTemp, strDest) != 0)
	{
	csDBFile.Format(_T("%suser\\%s"), GetExePath(), A2U(GetDataHandle().GetUserID()));	
	strDBFile = W2A(csDBFile);
	// 			CUccDatabase uccDataBase(CUccDatabase::GetDataBaseName(GetDataHandle().GetUserID()));
	// 			if (uccDataBase.IsDBOpen() == TRUE)
	// 			{
	// 				uccDataBase.CloseDatabase();
	// 			}
	CFileMgr::DelFileOrDir(strDBFile.c_str(), TRUE);
	csDBFile.Format(_T("%suser\\%s\\AVCON6.db"), GetExePath(), A2U(GetDataHandle().GetUserID()));
	strDBFile = W2A(csDBFile);
	CFileMgr::CopyFileDir(strRootDBFile.c_str(), strDBFile.c_str());
	return TRUE;
	}
	}
	*/

	return TRUE;

}

void CWBLoginDlg::ShowLoginError( unsigned long ulErrorCode )
{
	CString strErrCode = CErrorCode::Instance().GetErrCodeStr(ulErrorCode);
	if (!strErrCode.IsEmpty())
	{
		CString str;//完整错误码
		CString strErrorCode;//错误码
		CString strErrorStr;//错误提示
		strErrorCode.Format(_T("(Error: %u)"), ulErrorCode);
		strErrorStr.Format(_T("%s"),strErrCode);
		str = strErrorCode + _T("\r\n") + strErrorStr;//错误码和错误提示分行显示
		m_showLandMessageSTATIC.SetCaption(str);
	}

}

void CWBLoginDlg::InitLoginUserInfoStruct()
{
	CString SelectionName = LASTSELECTIONNAME;//最后一次的节名
	CString ServerHost = _T("");//服务器地址
	CString LoginID =  _T("");//登录ID
	CString Password = _T("");//登录密码
	BOOL IsAutoLand = FALSE;//是否是自动登录

	GetExePath();

	//最后一次登录的用户数据


	ServerHost = ReadOptionConfigStr(SelectionName ,SERVER_KEYNAME);
	if ( ServerHost.IsEmpty())
	{
		m_userInfoArr[SAVEUSERINFOLONG - 1].serverHost = _T("");
	} 
	else
	{
		m_userInfoArr[SAVEUSERINFOLONG - 1].serverHost = ServerHost;
		m_strServerHost = ServerHost;
	}
	LoginID = ReadOptionConfigStr(SelectionName ,LOGINID_KEYNAME);
	if ( LoginID.IsEmpty())
	{
		m_userInfoArr[SAVEUSERINFOLONG - 1].longinID = _T("");
	} 
	else
	{
		m_userInfoArr[SAVEUSERINFOLONG - 1].longinID = LoginID;
		m_strLoginID = LoginID;
	}
	Password = ReadOptionConfigStr(SelectionName ,PASSWORD_KEYNAME);
	if ( Password.IsEmpty())
	{
		m_userInfoArr[SAVEUSERINFOLONG - 1].password = _T("");
	} 
	else
	{
		m_userInfoArr[SAVEUSERINFOLONG - 1].password = Password;
	}
	IsAutoLand = _wtoi(ReadOptionConfigStr(SelectionName ,ISHIDELAND_KEYNAME));
	if (IsAutoLand )
	{
		m_userInfoArr[SAVEUSERINFOLONG - 1].isAutoLand = IsAutoLand;
	} 
	else
	{
		m_userInfoArr[SAVEUSERINFOLONG - 1].isAutoLand = 0;
	}


	//一般的保存的SAVEUSERINFOLONG - 1个用户登录的记录
	for (int i= 0;i< SAVEUSERINFOLONG - 1; i++)
	{ 
		SelectionName = SELECTIONNAME; //节名的打头部分

		CString SelectionNameCur = _T("");
		SelectionNameCur.Format(_T("%s%d"),SelectionName,i);
		ServerHost = ReadOptionConfigStr(SelectionNameCur ,SERVER_KEYNAME);

		//SelectionName.Format(_T("%s%d"),SelectionName,i);
		//ServerHost = ReadOptionConfigStr(SelectionName ,SERVER_KEYNAME);

		if ( ServerHost.IsEmpty())
		{
			m_userInfoArr[i].serverHost = _T("");
		} 
		else
		{
			m_userInfoArr[i].serverHost = ServerHost;

		}
		LoginID = ReadOptionConfigStr(SelectionNameCur ,LOGINID_KEYNAME);
		if ( LoginID.IsEmpty())
		{
			m_userInfoArr[i].longinID = _T("");
		} 
		else//从文件读取非空情况
		{
			m_userInfoArr[i].longinID = LoginID;
			//用来统计保存的节位置，如果用户名不为空说明是有效地登录记录,同时当前记录不是最后一次登录的用户名相同，
			//如果相同说明下一条记录就是最久没有使用的记录，下一次覆盖的就它了
			if (LoginID == m_userInfoArr[SAVEUSERINFOLONG - 1].longinID )
			{
				m_indexUserInfoPos = (i + 1)%(SAVEUSERINFOLONG - 1);
			} 

		}
		Password = ReadOptionConfigStr(SelectionNameCur ,PASSWORD_KEYNAME);
		if ( Password.IsEmpty())
		{
			m_userInfoArr[i].password = _T("");
		} 
		else
		{
			m_userInfoArr[i].password = Password;
		}
		IsAutoLand = _wtoi(ReadOptionConfigStr(SelectionNameCur ,ISHIDELAND_KEYNAME));
		if (IsAutoLand )
		{
			m_userInfoArr[i].isAutoLand = IsAutoLand;
		} 
		else
		{
			m_userInfoArr[i].isAutoLand = 0;
		}

	}
}

void CWBLoginDlg::OnSysCommand( UINT nID, LPARAM lParam )
{
	if ((nID & 0xFFF0) == SC_MOVE)
	{
		//设置背景图片
		CString strPath = _T("");
		strPath = GetExePath() + _T("SKIN\\LoginImage");
		SetSkinImage(strPath + _T("\\LoginDlgBG.png"));	
		Invalidate();
		UpdateWindow();
		CDialog::OnSysCommand(nID, lParam);
	} 
	else

	{
		CDialog::OnSysCommand(nID, lParam);
	}

}

BOOL CWBLoginDlg::ShowWindow( int nCmdShow )
{
	if ( SW_SHOW == nCmdShow )
	{		
		if ( m_strServerHost.IsEmpty() )
		{
			m_serverHostEdit.SetFocus();
		}
		else
		{
			m_longinIDCombox.SetFocus();

		}
 	//	else if ( m_strLoginID.IsEmpty() )
 	//	{
 	//		m_longinIDCombox.SetFocus();
 	//	}
		//else if ( m_strPassword.IsEmpty() )
		//{
		//	m_passwordEdit.SetFocus();

		//}
		//else
		//{
		//	m_passwordEdit.SetSel(m_strPassword.GetAllocLength(), m_strPassword.GetAllocLength());
		//	m_passwordEdit.SetFocus();
		//}

	}

	return __super::ShowWindow(nCmdShow);

}

BOOL CWBLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//显示最后一次登录的数据
	SetWindowText(_T("AVCONWB-电子白板"));
 	m_serverHostEdit.SetWindowText(m_userInfoArr[SAVEUSERINFOLONG - 1].serverHost);
 	m_longinIDCombox.SetWindowText(m_userInfoArr[SAVEUSERINFOLONG - 1].longinID);
	InitLoginIDCombox(&m_longinIDCombox);
   //  m_passwordEdit.SetWindowText(m_userInfoArr[SAVEUSERINFOLONG - 1].password);
	this->MoveWindow(0,0,400,300);
	this->CenterWindow(NULL);
	GetClientRect(&m_rect);

 	m_onlineBtn.MoveWindow(m_rect.left + (m_rect.right - m_rect.left)/3 - 40,m_rect.top + 235,80,25);
	m_onlineBtn.ShowWindow(SW_SHOW);
	m_singleLandBtn.MoveWindow(m_rect.left + 210,m_rect.top + 270,80,25);
	m_closeLoginDlgBtn.MoveWindow(m_rect.right - 45,m_rect.top + 3,42,25);
	m_cancelBtn.MoveWindow(m_rect.left + (m_rect.right - m_rect.left)/3 - 40,m_rect.top + 235,80,25);
	m_cancelBtn.ShowWindow(SW_HIDE);

	m_serverHostEdit.MoveWindow(m_rect.left + 145,m_rect.top + 100,160,25);
	m_longinIDCombox.MoveWindow(m_rect.left + 145,m_rect.top + 135,160,175);
	m_passwordEdit.MoveWindow(m_rect.left + 145,m_rect.top + 165,160,25);
	m_showLandMessageSTATIC.MoveWindow(m_rect.left + 20,m_rect.top + 200,360,30);
	m_CheckMTFunctionBtn.MoveWindow(m_rect.left + 2 * (m_rect.right - m_rect.left)/3 - 62,m_rect.top + 235,129,26);

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//m_eUICmdReceiver.Initialize();//出错的一句话

	/*IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
	if (pPlugin_Login == NULL)
	{
		return FALSE;
	}
	pPlugin_Login->AddObserver(this);*/

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BOOL CWBLoginDlg::PreTranslateMessage( MSG* pMsg )
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnBnClickedOnlineBtn();
		return TRUE;	
	}

	return CDialog::PreTranslateMessage(pMsg);
}

CString * CWBLoginDlg::GetLoginID()
{
	return &m_strLoginID;

}
void CWBLoginDlg::OnOK()
{

	//  CDialog::OnOK();
}
void CWBLoginDlg::OnCancel()
{	
	int i = 10;
	//CDialog::OnCancel();
}
void CWBLoginDlg::InitLoginIDCombox(CComboBox *comBoxData)
{
	for (int i = 0;i < SAVEUSERINFOLONG;i ++)
	{
		if (m_userInfoArr[i].longinID == "")
		{
			continue;
		}
		comBoxData->InsertString(i,m_userInfoArr[i].longinID);

	}
}
void CWBLoginDlg::OnBnClickedOnlineBtn()
{
	// 联机登录,添加观察者
	IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
	if (pPlugin_Login == NULL)
	{
		return ;
	}
	pPlugin_Login->AddObserver(this);

	m_onlineBtn.ShowWindow(SW_HIDE);
	m_cancelBtn.ShowWindow(SW_SHOW);
	m_showLandMessageSTATIC.SetCaption(CString(""));

	//是否勾选了启动触摸功能
	m_bNeedToCheckTouch = m_CheckMTFunctionBtn.GetIsSelect();

	UpdateData();//更新登录窗口上的数据，以便于能获取文本框上所输入的数据
	if ( !CheckData() )
	{
		ChangeLoginOrCancel();
		return;
	}

	OnLogin();
	CErrorCode::Instance().InitErrorCode();

	CMenu * tempSubMenw;
	//文件子菜单	
	tempSubMenw = (CWBMainDlg::g_pMainDlg->GetMenu())->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_LOAD_BTN,MF_GRAYED);
}

void CWBLoginDlg::OnBnClickedSinglelandBtn()
{
	// 单机登录
	//如果之前登陆过网络版本，则肯定白板装载对话框已经打开，所以调用单机版时要关闭以前打开的白板装载对话框
}
void CWBLoginDlg::OnBnClickedCloselogindlgBtn()
{
/*	CDialog::OnClose();*/
	CDialog::DestroyWindow();
	CWBMainDlg::g_pMainDlg->DeleteLoginDlg();
	//PostQuitMessage(0);	
}

void CWBLoginDlg::OnSize(UINT nType, int cx, int cy)
{
	//GetClientRect(&m_rect);
	//	CDialogEx::OnSize(nType, cx, cy);
}
void CWBLoginDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}
void CWBLoginDlg::OnBnClickedCancelBtn()
{
	m_cancelBtn.ShowWindow(SW_HIDE);
	m_onlineBtn.ShowWindow(SW_SHOW);

	DeleteMeetingListDlg();

	IPlugin_U7_IM_Login* pPlugin = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
	if (pPlugin == NULL)
	{
		return ;
	}
	pPlugin->Logout();
	pPlugin->DelObserver(this);

	ShowWindow(SW_SHOW);

	CMenu * tempSubMenw;
	//文件子菜单	
	tempSubMenw = (CWBMainDlg::g_pMainDlg->GetMenu())->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_LOAD_BTN,MF_ENABLED);

	OnConnectStatus(6,0);

	return;
}
HCURSOR CWBLoginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
BOOL CWBLoginDlg::OnEraseBkgnd(CDC* pDC)
{
	//设置背景图片
	CString strPath = _T("");
	strPath = GetExePath() + _T("SKIN\\LoginImage");
	SetSkinImage(strPath + _T("\\LoginDlgBG.png"));
	return TRUE;
	// return CDialog::OnEraseBkgnd(pDC);
}
void CWBLoginDlg::OnCbnSelchangeLoginidCombox()
{
	// TODO: 在此添加控件通知处理程序代码
	int i = m_longinIDCombox.GetCurSel();
	m_longinIDCombox.GetLBText(i,m_strLoginID);
}
LRESULT CWBLoginDlg::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);	 
	return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);

}

void CWBLoginDlg::SaveLoginInfo( void )
{
	CString SelectionName;
	CString SelectionNameCur = _T("");
	//当前登录和最后一次相同
	if (m_strLoginID == m_userInfoArr[SAVEUSERINFOLONG - 1].longinID )
	{      
		return;
	}
	//当前的登录和以前的某次相同
	for (int i = 0;i < SAVEUSERINFOLONG - 1; i++)
	{
		if (m_strLoginID == m_userInfoArr[i].longinID)
		{   //填写最后一次的配置文件信息
			SelectionName = LASTSELECTIONNAME;//最近一次打开的节名	
			WriteOptionConfigStr(SelectionName, SERVER_KEYNAME, m_strServerHost);
			WriteOptionConfigStr(SelectionName, LOGINID_KEYNAME, m_strLoginID);
			WriteOptionConfigStr(SelectionName, PASSWORD_KEYNAME, m_strPassword);
			WriteOptionConfigStr(SelectionName, ISHIDELAND_KEYNAME, _T("0"));
			//m_indexUserInfoPos = (i + 1)%(SAVEUSERINFOLONG - 1);

			//修改当前数组数据
			m_userInfoArr[SAVEUSERINFOLONG - 1].serverHost = m_strServerHost;
			m_userInfoArr[SAVEUSERINFOLONG - 1].longinID = m_strLoginID;
			m_userInfoArr[SAVEUSERINFOLONG - 1].password = m_strPassword;
			m_userInfoArr[SAVEUSERINFOLONG - 1].isAutoLand = 0;
			return;
		}
	}
	SelectionName = SELECTIONNAME;//用户配置信息节名的打头部分
	if (SelectionName =_T(""))
	{
		SelectionName = _T("user");

	}

	//SelectionName.Format(_T("%s%d"),SelectionName,m_indexUserInfoPos); 
    SelectionNameCur.Format(_T("%s%d"),SelectionName,m_indexUserInfoPos);

	//填写循环部分的
	WriteOptionConfigStr(SelectionNameCur, SERVER_KEYNAME, m_strServerHost);
	WriteOptionConfigStr(SelectionNameCur, LOGINID_KEYNAME, m_strLoginID);
	WriteOptionConfigStr(SelectionNameCur, PASSWORD_KEYNAME, m_strPassword);
	WriteOptionConfigStr(SelectionNameCur, ISHIDELAND_KEYNAME, _T("0"));
	//修改当前数组数据
	m_userInfoArr[m_indexUserInfoPos].serverHost = m_strServerHost;
	m_userInfoArr[m_indexUserInfoPos].longinID = m_strLoginID;
	m_userInfoArr[m_indexUserInfoPos].password = m_strPassword;
	m_userInfoArr[m_indexUserInfoPos].isAutoLand = 0;

	//用来统计保存的节位置，现在添加一条，所以下一次填写的位置要循环往下一个位置走
	m_indexUserInfoPos = (m_indexUserInfoPos + 1)%(SAVEUSERINFOLONG - 1);

	SelectionNameCur = LASTSELECTIONNAME;//最近一次打开的节名	
	WriteOptionConfigStr(SelectionNameCur, SERVER_KEYNAME, m_strServerHost);
	WriteOptionConfigStr(SelectionNameCur, LOGINID_KEYNAME, m_strLoginID);
	WriteOptionConfigStr(SelectionNameCur, PASSWORD_KEYNAME, m_strPassword);
	WriteOptionConfigStr(SelectionNameCur, ISHIDELAND_KEYNAME, _T("0"));
	//修改当前数组数据
	m_userInfoArr[SAVEUSERINFOLONG - 1].serverHost = m_strServerHost;
	m_userInfoArr[SAVEUSERINFOLONG - 1].longinID = m_strLoginID;
	m_userInfoArr[SAVEUSERINFOLONG - 1].password = m_strPassword;
	m_userInfoArr[SAVEUSERINFOLONG - 1].isAutoLand = 0;
	

}

int CWBLoginDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

void CWBLoginDlg::OnBnClickedCheckMTFunctionBtn()
{
	
	if(!m_CheckMTFunctionBtn.GetIsSelect())
	{
		m_CheckMTFunctionBtn.SetIsSelect(TRUE);
	}
	else
	{
		m_CheckMTFunctionBtn.SetIsSelect(FALSE);
	}
	m_CheckMTFunctionBtn.Invalidate();
}

CWBMeetingListDlg* CWBLoginDlg::GetMeetingListDlg(void)
{
	if (NULL == m_pMeetingListDlg)
	{
		m_pMeetingListDlg = new CWBMeetingListDlg;
		m_pMeetingListDlg->Create(CWBMeetingListDlg::IDD, this);
	}
	return m_pMeetingListDlg;
}

void CWBLoginDlg::DeleteMeetingListDlg()
{
	if (NULL != m_pMeetingListDlg)
	{
		m_pMeetingListDlg->DestroyWindow();
		delete m_pMeetingListDlg;
		m_pMeetingListDlg = NULL;
	}
}

void CWBLoginDlg::OnDestroy()
{
	__super::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

BOOL CWBLoginDlg::GetIsNeedToCheckTouch()
{
	return m_bNeedToCheckTouch;
}