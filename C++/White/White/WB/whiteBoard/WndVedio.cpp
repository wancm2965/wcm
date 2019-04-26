// WndVedio.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WndVedio.h"
#include "afxdialogex.h"
#include "tstring.h"


// CWndVedio �Ի���

IMPLEMENT_DYNAMIC(CWndVedio, CDialog)

CWndVedio::CWndVedio(CWnd* pParent /*=NULL*/)
	: CDialog(CWndVedio::IDD, pParent)
{
	m_CurHwnd    = NULL;      //������Ż�ȡ�Ŀؼ��ľ��
	m_PreHwnd    = NULL;      //���������ǰ��ȡ�Ŀؼ��ľ��
	m_LeftIsDown = FALSE;      //TRUE��ʾ����������
	m_hDLL       = NULL;
	m_GetCurHwnd = NULL;
	g_hVedioHwnd = NULL;
	m_MoveCursor = AfxGetApp()->LoadCursor(IDC_CURSOR10);
}

CWndVedio::~CWndVedio()
{
	g_hVedioHwnd = NULL;
}

void CWndVedio::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWndVedio, CDialog)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_STN_CLICKED(IDC_CATCH, &CWndVedio::OnStnClickedCatch)
	ON_WM_INITMENU()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CWndVedio ��Ϣ�������


void CWndVedio::OnLButtonUp(UINT nFlags, CPoint point)
{
	/*----------------------------------------------------------------------------------*/
	if (!g_bMedioScreenWndEnd)
	{
		EndDialog(10);
		return;
	}
	ReleaseCapture();
	CString strPrompt;
	strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
	if (strPrompt.IsEmpty())
	{
		strPrompt = _T("��ʾ");
	}
	typedef void (*lpDrawWindow)(HWND hwnd);

	//ȷ����̬���صĿ�·����ȷ
	HMODULE hModule = GetModuleHandle(TEXT("whiteboard.dll"));
	if (!hModule) return;

	TCHAR strName[MAX_PATH];
	GetModuleFileName(hModule, strName, MAX_PATH);
	CString strPath = strName;
	int nPos = strPath.ReverseFind('\\');
	if(-1 == nPos) nPos = strPath.ReverseFind('/');
	if(-1 == nPos) return;
	strPath = strPath.Left(nPos);

	CString tempPath = _T("");
	SetCurrentDirectory(strPath.GetBuffer());

	CString strResPath = _T("");

	strResPath.Format(TEXT("%s\\WBSCREC.dll"), strPath);

	m_hDLL = LoadLibrary(strResPath.GetBuffer());
	strPath.ReleaseBuffer();
	strResPath.ReleaseBuffer();

	if (NULL == m_hDLL)
	{
		CString strLoadDllFail;
		strLoadDllFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("LoadDllFail"));
		if (strLoadDllFail.IsEmpty())
		{
			strLoadDllFail = _T("����DLL�ļ�ʧ�ܣ�");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,strPrompt, strLoadDllFail, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strLoadDllFail);
		}
		else
		{
			MessageBox(strLoadDllFail);
		}
		return;
	}
	lpDrawWindow pDrawWindow = (lpDrawWindow)GetProcAddress(m_hDLL,ToStr(_T("DrawWindow")).c_str());
	if (NULL == pDrawWindow)
	{
		CString strGetDrawWinFail;
		strGetDrawWinFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("GetDrawWinFail"));
		if (strGetDrawWinFail.IsEmpty())
		{
			strGetDrawWinFail = _T("��ȡ(DrawWindow)������ַʧ�ܣ�");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,strPrompt, strGetDrawWinFail, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strGetDrawWinFail);
		}
		else
		{
			MessageBox(strGetDrawWinFail);
		}
		FreeLibrary(m_hDLL);
		return ;
	}

	if ((m_CurHwnd == GetDlgItem(IDC_CATCH)->m_hWnd) || (m_CurHwnd == this->m_hWnd))
	{
		CString strCannotRecWin;
		strCannotRecWin = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CannotRecWin"));
		if (strCannotRecWin.IsEmpty())
		{
			strCannotRecWin = _T("����¼�Ʊ����ڣ�");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,strPrompt, strCannotRecWin, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strCannotRecWin);
		}
		else
		{
			MessageBox(strCannotRecWin);
		}
		return;
	}
	pDrawWindow(m_CurHwnd);

	EndDialog(100);
	/*----------------------------------------------------------------------------------*/

	CDialog::OnLButtonUp(nFlags, point);
}


void CWndVedio::OnMouseMove(UINT nFlags, CPoint point)
{

	POINT MousePoint;
	HWND  hWnd;

	/*----------------------------------------------------------------------------------*/
	CString strPrompt;
	strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
	if (strPrompt.IsEmpty())
	{
		strPrompt = _T("��ʾ");
	}
	typedef void (*lpDrawWindow)(HWND hwnd);
	if ((m_LeftIsDown)&&(MK_LBUTTON == nFlags))
	{
		::SetCursor(m_MoveCursor);
		GetCursorPos(&MousePoint);
		m_CurHwnd = ::WindowFromPoint(MousePoint);

		if (NULL == m_CurHwnd)
		{
			return ;
		}
		hWnd = ::GetDlgItem(m_hWnd,IDC_CATCH);
		if ((m_CurHwnd != m_PreHwnd)&&(m_CurHwnd != hWnd) && (m_CurHwnd != m_hWnd))
		{
			/*����ؼ������仯���ҿؼ���Ϊ����*/
			//ȷ����̬���صĿ�·����ȷ
			HMODULE hModule = GetModuleHandle(TEXT("whiteboard.dll"));
			if (!hModule) return;

			TCHAR strName[MAX_PATH];
			GetModuleFileName(hModule, strName, MAX_PATH);
			CString strPath = strName;
			int nPos = strPath.ReverseFind('\\');
			if(-1 == nPos) nPos = strPath.ReverseFind('/');
			if(-1 == nPos) return;
			strPath = strPath.Left(nPos);

			CString tempPath = _T("");
			SetCurrentDirectory(strPath.GetBuffer());

			CString strResPath = _T("");

			strResPath.Format(TEXT("%s\\WBSCREC.dll"), strPath);

			m_hDLL = LoadLibrary(strResPath.GetBuffer());
			strPath.ReleaseBuffer();
			strResPath.ReleaseBuffer();
			
			if (NULL == m_hDLL)
			{
				CString strLoadDllFail;
				strLoadDllFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("LoadDllFail"));
				if (strLoadDllFail.IsEmpty())
				{
					strLoadDllFail = _T("����DLL�ļ�ʧ�ܣ�");
				}
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(this->m_hWnd,strPrompt, strLoadDllFail+_T("(lpDrawWindow)!"), MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strLoadDllFail+_T("(lpDrawWindow)!"));
				}
				else
				{
					MessageBox(strLoadDllFail+_T("(lpDrawWindow)!"));
				}
				return ;
			}
			lpDrawWindow pDrawWindow = (lpDrawWindow)GetProcAddress(m_hDLL,ToStr(_T("DrawWindow")).c_str());
			if (NULL == pDrawWindow)
			{
				CString strGetDrawWinFail;
				strGetDrawWinFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("GetDrawWinFail"));
				if (strGetDrawWinFail.IsEmpty())
				{
					strGetDrawWinFail = _T("��ȡ(DrawWindow)������ַʧ�ܣ�");
				}
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(this->m_hWnd,strPrompt, strGetDrawWinFail, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strGetDrawWinFail);
				}
				else
				{
					MessageBox(strGetDrawWinFail);
				}

				FreeLibrary(m_hDLL);
				return ;
			}
			//ȥ��ǰһ�����ڵĺڿ�
			if (NULL !=m_PreHwnd)
			{
				pDrawWindow(m_PreHwnd);
			}
			pDrawWindow(m_CurHwnd);
			m_PreHwnd = m_CurHwnd;
		}
	}

	/*----------------------------------------------------------------------------------*/

	CDialog::OnMouseMove(nFlags, point);
}
HWND  CWndVedio::GetWndHwnd()
{
	m_GetCurHwnd = m_CurHwnd;
	return m_GetCurHwnd;
}

void CWndVedio::OnStnClickedCatch()
{
	/*----------------------------------------------------------------------------------*/

	m_CurHwnd = NULL;
	m_PreHwnd = NULL;
	m_LeftIsDown = TRUE;
	  SetCapture() ;
 

	/*----------------------------------------------------------------------------------*/
}


void CWndVedio::OnInitMenu(CMenu* pMenu)
{
	CDialog::OnInitMenu(pMenu);

	// TODO: �ڴ˴������Ϣ����������
}


int CWndVedio::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	return 0;
}




BOOL CWndVedio::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (this->m_hWnd)
	{
		if (g_hVedioHwnd)
		{
			g_hVedioHwnd = NULL;
		}
		g_hVedioHwnd = this->m_hWnd;
	}
	CString strWinREC;
	strWinREC = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("WinREC"));
	if (strWinREC.IsEmpty())
	{
		strWinREC = _T("����¼��");
	}
	SetWindowText(strWinREC);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
