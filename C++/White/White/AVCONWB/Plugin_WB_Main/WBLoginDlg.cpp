// WBLoginDlg.cpp : ʵ���ļ�
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

// CWBLoginDlg �Ի���

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


// CWBLoginDlg ��Ϣ�������

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
	//��ʾ���߰�ť������ȡ����ť

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
			strStatus = _T("��������...");
		}
		break;
	case SERVER_RECONNECTED:
		{
			strStatus = _T("��������...");
		}
		break;
	case SERVER_BUSY:
		{
			strStatus = _T("������æ");
		}
		break;
	case SERVER_FAILED:
		{
			strStatus =  _T("����ʧ��");
		}
		break;
	case SERVER_DISCONNECTED:
		{
			strStatus = _T("���ӶϿ�");
		}
		break;
	case SERVER_CONNECTED:
		{
			//strStatus = _T("������");
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
		// ��½ʧ��
		ShowWindow(true);
		CString strError = CErrorCode::Instance().GetErrCodeStr((unsigned long)nErrCode);
		
		ShowLoginError((unsigned long)nErrCode);
		ChangeLoginOrCancel();
        DeleteMeetingListDlg();
	    return;
	}

	CWBMainDlg::g_pMainDlg->SetIsOnline(TRUE);
	//������������û�����
	SaveLoginInfo();

/*	WB_CloseFuntion();//�رյ����¹������*/

	IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room *>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (pPlugin_Room == NULL)
	{
		return;
	}
	pPlugin_Room->LoadRoomList();//�����������ڴ�й¶
}

 void CWBLoginDlg::OnLogin( void )
 {
 	if ( !CheckData() )
 	{
 		return;
 	}

 	//������������û�����
  	//SaveLoginInfo();
	IPlugin_U7_IM_Login* pPlugin = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
	if (pPlugin == NULL)
	{
		return ;
	}
 	pPlugin->Login(U2A(m_strServerHost).c_str(), 4222, U2A(m_strLoginID).c_str(), U2A(m_strPassword).c_str(), /*U2A(m_strLoginState).c_str()*/CMD_STR_USER_ONLINE);

	//���������б�Ի���
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
		if (CWBMainDlg::g_pMainDlg->GetIsNeedRealseTouchScreenAuthorize())//�ͷŷ�����������Ȩ
		{
			pPlugin_Function->ReleaseTouchScreenAuthorize();
			CWBMainDlg::g_pMainDlg->SetIsNeedRealseTouchScreenAuthorize(FALSE);
		}
		//�ſ�������İװ�����Ȩ�����Ӳ��֧��
		bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //��⵱ǰӲ���Ƿ�֧�ֶ�㴥������(������avconָ����Ӳ���������ļ�⣩
		if (!bHardwareTouch) //Ӳ����֧�ִ���
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
	strItem =(CString)"AVCONWB���Ӱװ�";
	CWBMainDlg::g_pMainDlg->SetWindowText(strItem);//���ðװ�Ի���ı�������

	WB_CloseAllChildBox();//wangxiaobin
	CWBMainDlg::g_pMainDlg->SetHPDTSNotify(NULL);

	CWBMainDlg::g_pMainDlg->SetWBStatic();

	CWBMainDlg::g_pMainDlg->SetisLocalBoardCastWB(false);

	ChangeLoginOrCancel();
	ShowWindow(SW_SHOW);

	if (nErrCode == 20005)
	{
		CString strStatus;
		strStatus = _T("��½ʧ�ܣ���ͬ�˻����û��������ط���½��");
		m_showLandMessageSTATIC.SetCaption(strStatus);
	}

	CMenu * tempSubMenw;
	//�ļ��Ӳ˵�	
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
			MessageBox(_T("IP��д����ȷ��"));
			return FALSE;
		}
		else if(atoi(dot[0]) > 255 || atoi(dot[1]) > 255 || atoi(dot[2]) > 255 || atoi(dot[3]) > 255 || atoi(dot[0]) == 0 || atoi(dot[3]) == 0)
		{
			MessageBox(_T("IP��д����ȷ��"));
			return FALSE;
		}
	}
	return TRUE;

}

BOOL CWBLoginDlg::CheckData( void )
{

	if ( m_strServerHost.IsEmpty()  )
	{
		MessageBox(_T("��������ַ����Ϊ�գ�"));
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
		MessageBox(_T("�˺Ų���Ϊ�գ�"));
		m_longinIDCombox.SetFocus();
		return FALSE;
	}
	else 
	{
		char temp = m_strLoginID.GetAt(m_strLoginID.GetLength() - 1);
		if (m_strLoginID.GetLength() < 2)
		{
			MessageBox(_T("�˻������Ȳ�������2λ��"));
			m_longinIDCombox.SetFocus();
			return FALSE;
		}
		if (temp == '.' || temp == '_' || temp == '-')
		{
			MessageBox(_T("�˻��������ԡ�.��������������_����β��"));
			m_longinIDCombox.SetFocus();
			return FALSE;
		}
	}
	// 	if ( m_strAccount.GetLength() > 16 || m_strAccount.GetLength() < 4 )
	// 	{
	// 		UCCMessageBox(this, _T("�û���������4-16λ��"));
	// 		m_editAccount.SetFocus();
	// 		return FALSE;
	// 	}
	if ( m_strPassword.IsEmpty() )
	{
		MessageBox(_T("���벻��Ϊ�գ�"));
		m_passwordEdit.SetFocus();
		return FALSE;
	}
	

	//�޸��û�����ͬ��������������ַʱ�������ز���ȷ 2011.09.15 niezhiren
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
		CString str;//����������
		CString strErrorCode;//������
		CString strErrorStr;//������ʾ
		strErrorCode.Format(_T("(Error: %u)"), ulErrorCode);
		strErrorStr.Format(_T("%s"),strErrCode);
		str = strErrorCode + _T("\r\n") + strErrorStr;//������ʹ�����ʾ������ʾ
		m_showLandMessageSTATIC.SetCaption(str);
	}

}

void CWBLoginDlg::InitLoginUserInfoStruct()
{
	CString SelectionName = LASTSELECTIONNAME;//���һ�εĽ���
	CString ServerHost = _T("");//��������ַ
	CString LoginID =  _T("");//��¼ID
	CString Password = _T("");//��¼����
	BOOL IsAutoLand = FALSE;//�Ƿ����Զ���¼

	GetExePath();

	//���һ�ε�¼���û�����


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


	//һ��ı����SAVEUSERINFOLONG - 1���û���¼�ļ�¼
	for (int i= 0;i< SAVEUSERINFOLONG - 1; i++)
	{ 
		SelectionName = SELECTIONNAME; //�����Ĵ�ͷ����

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
		else//���ļ���ȡ�ǿ����
		{
			m_userInfoArr[i].longinID = LoginID;
			//����ͳ�Ʊ���Ľ�λ�ã�����û�����Ϊ��˵������Ч�ص�¼��¼,ͬʱ��ǰ��¼�������һ�ε�¼���û�����ͬ��
			//�����ͬ˵����һ����¼�������û��ʹ�õļ�¼����һ�θ��ǵľ�����
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
		//���ñ���ͼƬ
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

	//��ʾ���һ�ε�¼������
	SetWindowText(_T("AVCONWB-���Ӱװ�"));
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//m_eUICmdReceiver.Initialize();//�����һ�仰

	/*IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
	if (pPlugin_Login == NULL)
	{
		return FALSE;
	}
	pPlugin_Login->AddObserver(this);*/

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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
	// ������¼,��ӹ۲���
	IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
	if (pPlugin_Login == NULL)
	{
		return ;
	}
	pPlugin_Login->AddObserver(this);

	m_onlineBtn.ShowWindow(SW_HIDE);
	m_cancelBtn.ShowWindow(SW_SHOW);
	m_showLandMessageSTATIC.SetCaption(CString(""));

	//�Ƿ�ѡ��������������
	m_bNeedToCheckTouch = m_CheckMTFunctionBtn.GetIsSelect();

	UpdateData();//���µ�¼�����ϵ����ݣ��Ա����ܻ�ȡ�ı����������������
	if ( !CheckData() )
	{
		ChangeLoginOrCancel();
		return;
	}

	OnLogin();
	CErrorCode::Instance().InitErrorCode();

	CMenu * tempSubMenw;
	//�ļ��Ӳ˵�	
	tempSubMenw = (CWBMainDlg::g_pMainDlg->GetMenu())->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_LOAD_BTN,MF_GRAYED);
}

void CWBLoginDlg::OnBnClickedSinglelandBtn()
{
	// ������¼
	//���֮ǰ��½������汾����϶��װ�װ�ضԻ����Ѿ��򿪣����Ե��õ�����ʱҪ�ر���ǰ�򿪵İװ�װ�ضԻ���
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
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
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
	//�ļ��Ӳ˵�	
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
	//���ñ���ͼƬ
	CString strPath = _T("");
	strPath = GetExePath() + _T("SKIN\\LoginImage");
	SetSkinImage(strPath + _T("\\LoginDlgBG.png"));
	return TRUE;
	// return CDialog::OnEraseBkgnd(pDC);
}
void CWBLoginDlg::OnCbnSelchangeLoginidCombox()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	//��ǰ��¼�����һ����ͬ
	if (m_strLoginID == m_userInfoArr[SAVEUSERINFOLONG - 1].longinID )
	{      
		return;
	}
	//��ǰ�ĵ�¼����ǰ��ĳ����ͬ
	for (int i = 0;i < SAVEUSERINFOLONG - 1; i++)
	{
		if (m_strLoginID == m_userInfoArr[i].longinID)
		{   //��д���һ�ε������ļ���Ϣ
			SelectionName = LASTSELECTIONNAME;//���һ�δ򿪵Ľ���	
			WriteOptionConfigStr(SelectionName, SERVER_KEYNAME, m_strServerHost);
			WriteOptionConfigStr(SelectionName, LOGINID_KEYNAME, m_strLoginID);
			WriteOptionConfigStr(SelectionName, PASSWORD_KEYNAME, m_strPassword);
			WriteOptionConfigStr(SelectionName, ISHIDELAND_KEYNAME, _T("0"));
			//m_indexUserInfoPos = (i + 1)%(SAVEUSERINFOLONG - 1);

			//�޸ĵ�ǰ��������
			m_userInfoArr[SAVEUSERINFOLONG - 1].serverHost = m_strServerHost;
			m_userInfoArr[SAVEUSERINFOLONG - 1].longinID = m_strLoginID;
			m_userInfoArr[SAVEUSERINFOLONG - 1].password = m_strPassword;
			m_userInfoArr[SAVEUSERINFOLONG - 1].isAutoLand = 0;
			return;
		}
	}
	SelectionName = SELECTIONNAME;//�û�������Ϣ�����Ĵ�ͷ����
	if (SelectionName =_T(""))
	{
		SelectionName = _T("user");

	}

	//SelectionName.Format(_T("%s%d"),SelectionName,m_indexUserInfoPos); 
    SelectionNameCur.Format(_T("%s%d"),SelectionName,m_indexUserInfoPos);

	//��дѭ�����ֵ�
	WriteOptionConfigStr(SelectionNameCur, SERVER_KEYNAME, m_strServerHost);
	WriteOptionConfigStr(SelectionNameCur, LOGINID_KEYNAME, m_strLoginID);
	WriteOptionConfigStr(SelectionNameCur, PASSWORD_KEYNAME, m_strPassword);
	WriteOptionConfigStr(SelectionNameCur, ISHIDELAND_KEYNAME, _T("0"));
	//�޸ĵ�ǰ��������
	m_userInfoArr[m_indexUserInfoPos].serverHost = m_strServerHost;
	m_userInfoArr[m_indexUserInfoPos].longinID = m_strLoginID;
	m_userInfoArr[m_indexUserInfoPos].password = m_strPassword;
	m_userInfoArr[m_indexUserInfoPos].isAutoLand = 0;

	//����ͳ�Ʊ���Ľ�λ�ã��������һ����������һ����д��λ��Ҫѭ������һ��λ����
	m_indexUserInfoPos = (m_indexUserInfoPos + 1)%(SAVEUSERINFOLONG - 1);

	SelectionNameCur = LASTSELECTIONNAME;//���һ�δ򿪵Ľ���	
	WriteOptionConfigStr(SelectionNameCur, SERVER_KEYNAME, m_strServerHost);
	WriteOptionConfigStr(SelectionNameCur, LOGINID_KEYNAME, m_strLoginID);
	WriteOptionConfigStr(SelectionNameCur, PASSWORD_KEYNAME, m_strPassword);
	WriteOptionConfigStr(SelectionNameCur, ISHIDELAND_KEYNAME, _T("0"));
	//�޸ĵ�ǰ��������
	m_userInfoArr[SAVEUSERINFOLONG - 1].serverHost = m_strServerHost;
	m_userInfoArr[SAVEUSERINFOLONG - 1].longinID = m_strLoginID;
	m_userInfoArr[SAVEUSERINFOLONG - 1].password = m_strPassword;
	m_userInfoArr[SAVEUSERINFOLONG - 1].isAutoLand = 0;
	

}

int CWBLoginDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

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

	// TODO: �ڴ˴������Ϣ����������
}

BOOL CWBLoginDlg::GetIsNeedToCheckTouch()
{
	return m_bNeedToCheckTouch;
}