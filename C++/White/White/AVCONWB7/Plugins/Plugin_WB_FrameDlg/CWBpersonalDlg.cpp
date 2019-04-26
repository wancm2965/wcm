// WhiteBoardDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "CWBPersonalDlg.h"
#include "NETEC/NETEC_Node.h"
#include "IPlugin/IU7T_TipWindow/IU7T_TipWindow.h"
#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
HHOOK g_hKeyboard=NULL;
HWND g_hWnd=NULL;

LRESULT CALLBACK KeyboardProc(
  int code,      
  WPARAM wParam,  
  LPARAM lParam   
)
{
	if(VK_F4==wParam && (1==(lParam>>29 & 1)))
		return 1;
	else
		return CallNextHookEx(g_hKeyboard,code,wParam,lParam);
	
}

// CWBPersonalDlg 对话框

IMPLEMENT_DYNAMIC(CWBPersonalDlg, CDialog)

CWBPersonalDlg::CWBPersonalDlg( unsigned long ulWBChannelId,std::string strLocalUserId,CWnd* pParent /*=NULL*/ )
: AVMTDataChannel(ulWBChannelId,true,false)
, CDialog(CWBPersonalDlg::IDD, pParent)
, m_ulDataChannelId(ulWBChannelId)
, m_pIAVMTDataSessionClient(NULL)
, m_strLocalUserId(strLocalUserId)
, m_bConnected(false)
{
}

CWBPersonalDlg::CWBPersonalDlg( unsigned long ulWBChannelId,CWnd* pParent /*= NULL*/ )
: AVMTDataChannel(ulWBChannelId,true,false)
, CDialog(CWBPersonalDlg::IDD, pParent)
, m_ulDataChannelId(ulWBChannelId)
, m_pIAVMTDataSessionClient(NULL)
, m_strLocalUserId("Locad")
, m_bConnected(false)
{

}

CWBPersonalDlg::~CWBPersonalDlg()
{
}

void CWBPersonalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWBPersonalDlg, CDialog)
	ON_WM_MOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CWBPersonalDlg 消息处理程序

BOOL CWBPersonalDlg::OnInitDialog()
{
	__super::OnInitDialog();
	std::string strDtsTempPath = CW2A(GetExePath());		
	strDtsTempPath += "HPDTS";
	_tmkdir(CA2W(strDtsTempPath.c_str()));
	strDtsTempPath = strDtsTempPath + "\\" + m_strLocalUserId;
	_tmkdir(CA2W(strDtsTempPath.c_str()));
	HPDTSInit(strDtsTempPath.c_str());
	HPDTSSetSavePath(strDtsTempPath.c_str());
	WB_SetWBTempDir(strDtsTempPath.c_str());
	WB_SetU7FileDir(GetFileSavePath());
	WB_SetTypeOfWB(WB_SINGLERVER);
	
	WB_ShowWhiteBoardDlg((void*)this, this, &CWnd::wndTop, CRect(0, 0, 0, 0), 0);
	WB_SetIsLoacalWhiteboard(true);
	if (WB_BHardwareSupportMTTouch())
	{
		WB_SetTouchAuthorization(TRUE);
	}
	
	g_hWnd=m_hWnd;	
	g_hKeyboard=SetWindowsHookEx(WH_KEYBOARD,KeyboardProc,NULL,GetCurrentThreadId());
	//ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

bool CWBPersonalDlg::Connect( unsigned long ulWBChannelID,const std::string& mcuid,const std::string& mcuaddr,unsigned short mcuport,const std::string& mcutype )
{
	// 不允许连接两次，但下面的操作还是要继续进行	
	if (!m_bConnected && !AVMTDataChannel::Connect())
		return false;

	m_bConnected = true;
	SetWBChannelChanged(ulWBChannelID, mcuid, mcuaddr, mcuport, mcutype);
	return true;
}

void CWBPersonalDlg::ReleaseConnection( void )
{
	if (m_pIAVMTDataSessionClient)
	{
		m_pIAVMTDataSessionClient->ReleaseConnections();
		delete m_pIAVMTDataSessionClient;
		m_pIAVMTDataSessionClient = NULL;
	}
	m_bConnected = false;

	AVMTDataChannel::RemoveAVMTDataChannel(GetDataChannelID());

	AVMTDataChannel::ReleaseConnections();
}

void CWBPersonalDlg::SetWBChannelChanged( unsigned long ulWBChannelID,const std::string& mcuid,const std::string& mcuaddr,unsigned short mcuport,const std::string& mcutype )
{
	if (mcuid == "" || mcuaddr == "")
	{
		return;
	}
	bool bChanged = false;
	if (m_mcuid != mcuid || m_ulDataChannelId != ulWBChannelID)
	{
		m_mcuid		= mcuid;
		m_mcuaddr	= mcuaddr;
		m_mcuport	= mcuport;
		m_mcutype	= mcutype;

		m_ulDataChannelId = ulWBChannelID;
		bChanged = true;
	}

	if (bChanged || m_pIAVMTDataSessionClient == NULL)
	{
		if (m_pIAVMTDataSessionClient)
		{
			m_pIAVMTDataSessionClient->ReleaseConnections();
			delete m_pIAVMTDataSessionClient;
			m_pIAVMTDataSessionClient = NULL;
		}

		AVMTDataChannel::RemoveAVMTDataChannel(GetDataChannelID());
		AVMTDataChannel::SetDataChannelID(ulWBChannelID);
		AVMTDataChannel::AddAVMTDataChannel(this);

		m_pIAVMTDataSessionClient = CreateIAVMTDataSessionClient(*this, m_ulDataChannelId,
			m_mcuid.c_str(),m_mcuaddr.c_str(),m_mcuport,m_mcutype.c_str(),
			NETEC_Node::GetMCUID(),NETEC_Node::GetMCUIP(),NETEC_Node::GetServerPort(),""/*NETEC_Node::GetMCUType()*/);
	}
}

bool CWBPersonalDlg::IsConnect( void )
{
	return m_bConnected;
}

bool CWBPersonalDlg::processWBData( char * pData,long nLen )
{
	WB_ProcessDataIn(pData, nLen);
	return true;
}

void CWBPersonalDlg::SetLocalUserID( std::string strUserID )
{
	m_strLocalUserId = strUserID;
	WB_SetLocalUserID(strUserID.c_str());
}

void CWBPersonalDlg::DestroyWBDlg()
{
	WB_Close();
	ReleaseConnection();
}

void CWBPersonalDlg::OnDataToUI( const char*pData,int nLen )
{
	WB_ProcessDataIn(pData,nLen);
}

void CWBPersonalDlg::SendWbData( const char* pData,int iLen,bool bToNet /*= true*/ )
{
	if (bToNet)
	{
		AVMTDataChannel::ProcessUIData((char*)pData, iLen);
	}
	else
	{
		UINT uCmd = 0;
		AVDataInPacket in((char*)pData,iLen);
		in >>uCmd;
		int i = 0;
		if (uCmd == WB_NEW_COMMAND_DATA)
		{
			UINT uLen = 0;
			in >> uLen;
			unsigned char* pBuff = in.ReadData((int&)uLen);
			char* pNotifyData = new char[uLen];
			memset(pNotifyData,0,uLen);
			memcpy(pNotifyData,pBuff,uLen);		
			std::list<IPlugin_WB_FrameDlg_Observer*>::iterator it = m_lstObserver.begin();
			for (; it != m_lstObserver.end(); it++)
			{
				(*it)->ProcessWBToLocalData(pNotifyData);
				int iCmdType = *((int*)pNotifyData);
				if (iCmdType == CMD_SINGLE_QUIT)
				{
					delete []pNotifyData;
					pNotifyData = NULL;
					return;
				}
			}
			if (pNotifyData)
			{
				delete []pNotifyData;
				pNotifyData = NULL;
			}
		}
	}
}

bool CWBPersonalDlg::OpenSaveFile( BOOL bOpenSave,char* sFilePath,LPCTSTR lpszDefExt /*= NULL*/,LPCTSTR lpszFilter /*= NULL*/,HWND hParent/*=NULL */,LPCTSTR strFilePath /*=_T("")*/ )
{
	HWND hwnd = NULL;
	IPlugin_U7T_OpenSave *pOpenSave = dynamic_cast<IPlugin_U7T_OpenSave*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_OpenSave"));
	if (pOpenSave == NULL)
		return false;
	hwnd = pOpenSave->CreateOpenSave(bOpenSave,A2U(sFilePath),lpszFilter,lpszDefExt,hParent,strFilePath);
	if (hwnd == NULL)
		return false;
	std::string strFile = pOpenSave->GetOpenFile();
	//::ZeroMemory(sFilePath,sizeof(sFilePath));
 	//CString strTmpFile = A2U(strFile.c_str());//解决中文乱码问题
 	//strFile = CW2A(strTmpFile);
	strcpy(sFilePath, strFile.c_str());

	return true;
}

int CWBPersonalDlg::MessageBox(HWND hOwner,const CString & strTitle,const CString & strContent,UINT nStye, bool bAutoDestroy /*= false*/ )
{
	//wchar_t szTitel[MAX_PATH];
	//MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strTitle.GetBuffer(), -1, szTitel, MAX_PATH);

	//wchar_t szContent[MAX_PATH];
	//MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strContent.GetBuffer(), -1, szContent, MAX_PATH);
	int nResult ;
	IPlugin_U7T_MessageBox *pMessage = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
	if (pMessage == NULL)
	{
		nResult = IDCLOSE;
	}
	nResult = pMessage->MessageBox(hOwner,strTitle,strContent,nStye,bAutoDestroy);
	return nResult;
}

void CWBPersonalDlg::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	// TODO: 在此处添加消息处理程序代码
}

void CWBPersonalDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	WB_MoveBorad(NULL, CRect(0, 0, cx, cy-31), SWP_NOACTIVATE);
	WB_CloseAllChildBox();
	WB_DestroyWnd(false);
	// TODO: 在此处添加消息处理程序代码
}

void CWBPersonalDlg::addObserver( IPlugin_WB_FrameDlg_Observer* pObserver )
{
	if (pObserver == NULL)
		return;
	m_lstObserver.remove(pObserver);
	m_lstObserver.push_back(pObserver);
}

void CWBPersonalDlg::DelObserver( IPlugin_WB_FrameDlg_Observer* pObserver )
{
	if (pObserver == NULL)
		return;
	m_lstObserver.remove(pObserver);
}

void CWBPersonalDlg::SetLoaclUserName( CString str )
{
	std::string strname ;
	std::wstring p = str.GetBuffer(str.GetLength());
	CHAR strusename[MAX_PATH] = {0};
	WideCharToMultiByte(CP_ACP,NULL,p.c_str(),p.length(),strusename,MAX_PATH,NULL,NULL);
	strname = std::string(strusename);
	WB_SetLocalUserName(strname.c_str());
}

void CWBPersonalDlg::SetWBLock( bool block )
{
	WB_AvconLock(block);
}

void CWBPersonalDlg::ShowTipWindow( const CString& strTip,int nSeconds /*= 3*/,HWND pParentWnd /*= NULL*/ )
{
	IPlugin_U7T_TipWindow* pPluginTipWindow = dynamic_cast<IPlugin_U7T_TipWindow*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_TipWindow"));
	if (pPluginTipWindow)
	{
		pPluginTipWindow->ShowTipWindow(strTip,nSeconds,pParentWnd);
	}
}

void CWBPersonalDlg::SetSelectObjStatue( MSG msg )
{

}
