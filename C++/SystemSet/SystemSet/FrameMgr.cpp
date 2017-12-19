#include "StdAfx.h"
#include "FrameMgr.h"
#include "DlgMsgTip.h"

CDlgMsgTip* g_pDlgMsgTip = NULL;

void mgr_initUI()
{
	if (g_pDlgMsgTip == NULL)
	{
		g_pDlgMsgTip = new CDlgMsgTip;
		g_pDlgMsgTip->Create(NULL, _T("DlgMsgTip"), WS_OVERLAPPEDWINDOW|WS_POPUP, WS_EX_TOPMOST|WS_EX_TOOLWINDOW);
	}
	int nConfig = 0;
	nConfig = g_pDlgMsgTip->GetPrivateProfile();
	if(nConfig != 0){
		g_pDlgMsgTip->StartTimerTest(USETMSG_TESTWNDSHOW, 500);
	}
}

void mgr_unitUI()
{
	if (NULL != g_pDlgMsgTip)
	{
		::DestroyWindow(g_pDlgMsgTip->GetHWND());
		delete g_pDlgMsgTip;
		g_pDlgMsgTip = NULL;
	}
}

