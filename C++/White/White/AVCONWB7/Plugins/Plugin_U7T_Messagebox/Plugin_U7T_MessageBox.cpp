#include "StdAfx.h"
#include "Plugin_U7T_MessageBox.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace AVCONPlugin;

CPlugin_U7T_MessageBox::CPlugin_U7T_MessageBox(void)
{
}

CPlugin_U7T_MessageBox::~CPlugin_U7T_MessageBox(void)
{
}

std::string CPlugin_U7T_MessageBox::Name()
{
	return "Plugin_U7T_MessageBox";
}

std::string CPlugin_U7T_MessageBox::ExtendPoint()
{
	return "";
}

bool CPlugin_U7T_MessageBox::InitPlugin()
{
	return true;
}

bool CPlugin_U7T_MessageBox::ClosePlugin()
{
	return true;
}

int CPlugin_U7T_MessageBox::MessageBox(HWND hOwner,CString strTitle,CString strContent,UINT nStye, bool bAutoDestroy/* = false*/)
{
	CUCCMessageBox dlgMsgins(CString(strTitle),CString(strContent), nStye, bAutoDestroy);
	UINT style = UI_WNDSTYLE_FRAME;
	style&=~WS_MAXIMIZEBOX;
	dlgMsgins.Create(hOwner, _T("UCCMessageBox"), style/*UI_WNDSTYLE_FRAME*//*UI_WNDSTYLE_DIALOG*/, WS_EX_TOOLWINDOW, 0, 0, 470, 165);
	m_GlobalMessageBoxManager.AddMessageBox(&dlgMsgins);

	//////////////////////////////////////////////////////////////////////////
	// 使窗口在当前屏幕居中
	POINT pt;
	::GetCursorPos(&pt);
	MONITORINFO oMonitor = {}; 
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	UICRect rcWork = oMonitor.rcWork;
	RECT rcDlg = { 0 };
	::GetWindowRect(dlgMsgins.GetHWND(), &rcDlg);
	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;
	int xLeft = (rcWork.left + rcWork.right) / 2 - DlgWidth / 2;
	int yTop = (rcWork.top + rcWork.bottom) / 2 - DlgHeight / 2;
	::SetWindowPos(dlgMsgins.GetHWND(), HWND_TOPMOST, xLeft, yTop, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
	//////////////////////////////////////////////////////////////////////////

	int nRet = dlgMsgins.ShowModal();

	m_GlobalMessageBoxManager.DelMessageBox(&dlgMsgins);
	return nRet;
}

void CPlugin_U7T_MessageBox::CloseAllMessageBox()
{
	m_GlobalMessageBoxManager.DeleteAllMessageBox();
}
