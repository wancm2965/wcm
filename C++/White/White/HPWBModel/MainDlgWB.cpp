// MainDlgWB.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MainDlgWB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainDlgWB �Ի���
IHPWBModel* IHPWBModel::CreateWBModel(IHPNetNotify* pNotify,IHPWBNotify* pWBNotify,CWnd* pParentWnd,bool bNeedToCheckTouchScreen,const std::string& strLanguageFile,const std::string&  strIniPath,const std::string&  strTempPath)
{
	IHPWBModel* pIHPWBModel = NULL;
	CMainDlgWB* pWnd = new CMainDlgWB(pNotify,pWBNotify,bNeedToCheckTouchScreen,strLanguageFile,strIniPath,strTempPath,pParentWnd);
	if (pWnd)
	{
		pWnd->Create(CMainDlgWB::IDD,pParentWnd);
		pIHPWBModel = (IHPWBModel*)pWnd;
	}
	return pIHPWBModel;
}


IMPLEMENT_DYNAMIC(CMainDlgWB, CDialog)

CMainDlgWB::CMainDlgWB(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlgWB::IDD, pParent)
	,m_pNetNotify(NULL)
	,m_pParentWnd(NULL)
	,m_bTouchScreen(false)
	,m_bNeedToCheckTouch(false)
{
	m_oldRect = CRect(0,0,0,0);
}

CMainDlgWB::CMainDlgWB(IHPNetNotify* pNotify,IHPWBNotify* pWBNotify,bool bNeedToCheckTouchScreen,const std::string& strLanguageFile,const std::string&  strIniPath,const std::string&  strTempPath,CWnd* pParent)
	: CDialog(CMainDlgWB::IDD, pParent)
	,m_pNetNotify(pNotify)
	,m_pWBNotify(pWBNotify)
	,m_pParentWnd(NULL)
	,m_bTouchScreen(false)
	,m_bNeedToCheckTouch(bNeedToCheckTouchScreen)
	,m_strLanguageFile(strLanguageFile)
	,m_strIniPath(strIniPath)
	,m_strTempPath(strTempPath)
{
}

CMainDlgWB::~CMainDlgWB()
{
	
}

void CMainDlgWB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDlgWB, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CMainDlgWB ��Ϣ�������
void CMainDlgWB::Init()
{
	CWnd *pWnd = GetParent();   //avcon���ھ��
	CRect rc;

	if (pWnd)
	{
		pWnd->GetClientRect(rc);
		this->SetWindowPos(&wndTop,rc.left,rc.top,rc.Width(),rc.Height(),SWP_SHOWWINDOW);
		WB_ShowWhiteBoardDlg(this,(IHPNetNotify*)m_pNetNotify,&wndTop,rc,NULL);
		WB_SetConferenceRecordNotify((IHPWBNotify *)m_pWBNotify);

		//����·��
		char *pszLanguageFile = new char[MAX_PATH];
		memcpy(pszLanguageFile, m_strLanguageFile.c_str(), m_strLanguageFile.size());
		pszLanguageFile[m_strLanguageFile.size()] = '\0';

		char *pszIniPath = new char[MAX_PATH];
		memcpy(pszIniPath, m_strIniPath.c_str(), m_strIniPath.size());
		pszIniPath[m_strIniPath.size()] = '\0';

		char *pszTempPath = new char[MAX_PATH];
		memcpy(pszTempPath, m_strTempPath.c_str(), m_strTempPath.size());
		pszTempPath[m_strTempPath.size()] = '\0';	

		WB_SetFilePathsByavcon(pszLanguageFile, pszIniPath, pszTempPath);

		delete[] pszLanguageFile;
		delete[] pszIniPath;
		delete[] pszTempPath;
	}


}

void CMainDlgWB::ReleaseModelResource()
{
	
	if (m_bTouchScreen && m_pWBNotify)
	{
		m_pWBNotify->NotifyStopTouchScreen();
		m_bTouchScreen = false;
	}
	WB_Close();
}

//IHPWBDataNotify
void CMainDlgWB::ProcessWBDataOut(const char* pData,int iLen)
{
	if (m_pNetNotify)
	{
		m_pNetNotify->SendWbData(pData,iLen);
	}
}
//IHPWBModel
void CMainDlgWB::SetCheckTouchScreen(bool bCheck)
{
	m_bNeedToCheckTouch = bCheck;
// 	if (m_bHardwareTouch && m_bNeedToCheckTouch)
// 	{   //Ӳ����֧��
// 		WB_SetTouchAuthorization(TRUE);
// 	}
	
}

BOOL CMainDlgWB::OpenPic(CString strPath)
{
	char *pszPath = new char[MAX_PATH];
//	wcstombs(pszPath, strPath, MAX_PATH);
    WideCharToMultiByte( CP_ACP, 0, strPath.GetBuffer(), -1, pszPath, MAX_PATH, NULL, NULL );
	BOOL bR = WB_AddPicture(pszPath);
	delete[] pszPath;

	return bR;
}

void CMainDlgWB::Start(bool bTouchScreen)
{
	m_bTouchScreen = bTouchScreen;
    WB_SetTouchAuthorization(bTouchScreen);
}

void CMainDlgWB::Release(void)
{
	ReleaseModelResource();
}

void CMainDlgWB::SetParentWnd(CWnd* pWnd)
{
	m_pParentWnd = pWnd;
	SetParent(pWnd);
}

void CMainDlgWB::RegistNetNotify(IHPNetNotify* pNotify)
{
	if (pNotify)
	{
		m_pNetNotify = pNotify;
		WB_SetDataNotify((void*)m_pNetNotify);
	}
}
void CMainDlgWB::ProcessNetDataIn(const char* pData,int iLen)
{

	if (pData)
	{
		WB_ProcessDataIn(pData,iLen);
	}
}

void CMainDlgWB::SetWBWndPos(CWnd * pWndInsertAfter,int x,int y,int cx,int cy,UINT nFlag)
{
	SetWindowPos(pWndInsertAfter, x, y, cx, cy, nFlag);
}
void CMainDlgWB::ShowWBWnd(bool bShow)
{
	if (bShow)
	{
		ShowWindow(SW_SHOW);

	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}
void CMainDlgWB::RequestAllObj()
{
	WB_ReqAllObj();
}

BOOL CMainDlgWB::OnInitDialog()
{
	__super::OnInitDialog();
	Init();
    bool BHardwareTouch = WB_BHardwareSupportMTTouch();//�ж�Ӳ���Ƿ�֧��
	if (!BHardwareTouch)
	{   //Ӳ����֧��
		Start(false);
	}
	else
	{
		//Ӳ��֧��
		if (!m_bNeedToCheckTouch)
		{//��avcon6��û�����ö�㴥������
			Start(false);
		}
		else
		{//��avcon6�����ö�㴥������
			if (m_pWBNotify)
			{   //ͨ��m_pWBNotify->NotifyStartTouchScreen()��������Start���������жϷ����Ƿ���Ȩ,��ʱ�������Ĳ�����������ȨҲ��������Ȩ
				m_pWBNotify->NotifyStartTouchScreen();
			}
		}
	}
	
	 // �öԻ������͸��Ч��
	//ModifyStyleEx(0, 0x80000);
	////����User32.DLL��̬���ӿ�
	//HMODULE hModule = LoadLibrary(_T("User32.DLL")); 
	//if (hModule != NULL) 
	//{ 
	//	typedef BOOL (WINAPI *FUNC)(HWND, COLORREF, BYTE, DWORD); 
	//	//���SetLayeredWindowAttributes����ָ�� 
	//	FUNC func = (FUNC)GetProcAddress(hModule, "SetLayeredWindowAttributes");
	//	if(func != NULL)
	//	{
	//		func(GetSafeHwnd(), 0, 0, 2); 
	//	}
	//	FreeLibrary(hModule); 
	//}
	//Init();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CMainDlgWB::OnDestroy()
{
	__super::OnDestroy();
}

void CMainDlgWB::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	CRect rc;
		GetClientRect(rc);
	WB_MoveBorad(&wndTop,rc,NULL);
}


void CMainDlgWB::ShowMainWnd(bool bShow)
{
	if (m_pWBNotify)
	{
		m_pWBNotify->NotifyShowMainWnd(bShow);
	}

	//CString str = _T("`1");
	//WB_SetLocalUserID((LPCTSTR)str);
}
BOOL CMainDlgWB::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return __super::OnEraseBkgnd(pDC);
}

void CMainDlgWB::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//__super::OnOK();
}

void CMainDlgWB::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	//__super::OnCancel();
}

void CMainDlgWB::SetLocalUserID(CString strUserID)
{
	char *pszUserID = new char[MAX_PATH];
	//wcstombs(pszUserID, strUserID, MAX_PATH);
    WideCharToMultiByte( CP_ACP, 0, strUserID.GetBuffer(), -1, pszUserID, MAX_PATH, NULL, NULL );
	WB_SetLocalUserID(pszUserID);
	delete[] pszUserID;
}

void CMainDlgWB::MemberOffline(CString strUserID)
{
	char *pszUserID = new char[MAX_PATH];
	//wcstombs(pszUserID, strUserID, MAX_PATH);
    WideCharToMultiByte( CP_ACP, 0, strUserID.GetBuffer(), -1, pszUserID, MAX_PATH, NULL, NULL );
	WB_MemberOffline(pszUserID);
	delete[] pszUserID;
}

void CMainDlgWB::ShowToolBar(bool bShow)
{
}

BOOL CMainDlgWB::StartConferenceRecord()	// ��ʼ����¼��
{
	return WB_StartConferenceRecord(true);
}

void CMainDlgWB::StopConferenceRecord() 	// ֹͣ����¼��	
{
	WB_StartConferenceRecord(false);
}

void CMainDlgWB::Lock(bool bLock)
{
	WB_AvconLock(bLock);
}

//����ӵĽӿڴ���
/*****************************************************/
void CMainDlgWB::ShowBroadCast( bool bShow )
{
	/**************************************/
	//tureΪ��ʾ����ˮ���ƣ�falseΪ�ر�
	/*************************************/
	WB_ShowBroadCast(bShow);

}

void CMainDlgWB::CloseAllChildBox()
{

	/********************************     ******/
	//�رչ������ӶԻ��򣬲���Ҫ��ʾ��ʱ�������
	/**************************      ***********/
	WB_CloseAllChildBox();
}

void CMainDlgWB::DestroyWnd( bool bIsSend )
{
	//���������ͼ����tureȷ������
	WB_DestroyWnd(bIsSend);
}


//�װ��Ƿ���С���ˣ���С����ΪTRUE������ΪFALSE
void CMainDlgWB::SetIsMinSize(BOOL bIsMinSize)
{
	WB_SetIsMinSize(bIsMinSize);
}
/******************************************************/
