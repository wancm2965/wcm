#include "StdAfx.h"
#include "resource.h"
#include "DlgMsgTip.h"

#define USERMSG_TIMER  (COUI_MSG_USER+10023)
#define MODULE_CONFIGRESTART  _T("Config_Restart")
#define CONFIG_NAME  _T("Config.ini")
CDlgMsgTip::CDlgMsgTip()
	: m_plbTipInfo(NULL)
	, m_uTipType(type_msgbox_showset)
	, m_pInfo(_T(""))
	, m_nCount(3)
	/*, m_nHotKeyId(0)*/
{
}

CDlgMsgTip::~CDlgMsgTip(void)
{
	StopTimerTest(USERMSG_TIMER);
	StopTimerTest(USETMSG_TESTWNDSHOW);
	StopTimerTest(USETMSG_TESTWNDCLOSE);
	//::UnregisterHotKey(GetHWND(),m_nHotKeyId);
}

void CDlgMsgTip::Init()
{
	//HotKeyId的合法取值范围是0x0000到0xBFFF之间，而GlobalAddAtom函数得到的值在0xC000到0xFFFF之间，所以减掉0xC000来满足调用要求
// 	m_nHotKeyId = ::GlobalAddAtom(_T("MyHotKey")) - 0xC000;
// 	::RegisterHotKey(GetHWND(),m_nHotKeyId,MOD_ALT|MOD_CONTROL,'I');
	m_plbTipInfo = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(GetRoot(), _T("lbTipInfo")));
	ASSERT(m_plbTipInfo);

	int nConfig = -1;
	nConfig = GetPrivateProfile();
	if (nConfig == 0)
	{
		SetMsgTipType(type_msgbox_showset);
		SetMsgTipInfo(_T("是否要进行显示设置配置？"));
		ShowWindow(true);
		CenterWindow();
	}
}

void CDlgMsgTip::Notify( TNotifyUI& msg )
{
	bool bChange = false;
	UINT uType = 0;
	if (COUI_MSGTYPE_CLICK == msg.nType)
	{
		if (_tcscmp(msg.pSender->GetName(), _T("BtnOk")) == 0)
		{
			if (type_msgbox_showset == m_uTipType)
			{
				CString  strVersionPath = _T("D:\\AVCONXPE\\HPVersion.ini");
				CString strVal = _T("1");				
				WritePrivateProfileString(_T("Config_Restart"),_T("Show_Restart"),strVal,strVersionPath);
				SetMsgTipType(type_msgnormal);
				SetMsgTipInfo(_T("系统即将重启"), true);
			}
			else if (type_msgbox_restart == m_uTipType)
			{
				CString  strVersionPath = _T("D:\\AVCONXPE\\HPVersion.ini");
				CString strVal = _T("0");
				WritePrivateProfileString(_T("Config_Restart"),_T("Show_Restart"),strVal,strVersionPath);
				WinExec("uwfmgr filter enable",SW_HIDE);
				SetPrivateProfile(0);
				::PostQuitMessage(0L);
				//TODO 重启系统
				
				ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
				WinExec("shutdown -r -t 0", SW_HIDE);
				::ExitWindowsEx(EWX_REBOOT,0);
			}
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("BtnCancel")) == 0)
		{
			if (type_msgbox_showset == m_uTipType)
			{
				ShowWindow(false);
			}
			else if (type_msgbox_restart == m_uTipType)
			{
				ShowWindow(false);
				StartTimerTest(USETMSG_TESTWNDSHOW, 500);
			}
		}
	}
	else if (COUI_MSGTYPE_TIMER == msg.nType)
	{
		if (msg.wParam == USERMSG_TIMER)
		{
			m_nCount--;
			if (m_nCount == 0)
			{
				SetPrivateProfile(1);
				CoUI_KillTimer(USERMSG_TIMER);
				::PostQuitMessage(0L);
				//TODO 重启系统
				::ExitWindowsEx(EWX_REBOOT,0);
				ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
				WinExec("shutdown -r -t 0", SW_HIDE);
				return;
			}
			CString csInfo;
			csInfo.Format(_T("%s  %d"), m_pInfo, m_nCount);
			if(NULL != m_plbTipInfo){
				m_plbTipInfo->SetText(csInfo);
			}
		}
		else if (msg.wParam == USETMSG_TESTWNDSHOW)
		{
			HWND hWnd = ::FindWindow(NULL, _T("Intel(R) Graphics Control Panel"));
			if (::IsWindow(hWnd))
			{
				StopTimerTest(USETMSG_TESTWNDSHOW);
				CoUI_SetTimer(USETMSG_TESTWNDCLOSE, 500);
			}
		}
		else if (msg.wParam == USETMSG_TESTWNDCLOSE)
		{
			HWND hWnd = ::FindWindow(NULL, _T("Intel(R) Graphics Control Panel"));
			if (!::IsWindow(hWnd))
			{
				StopTimerTest(USETMSG_TESTWNDCLOSE);
				SetMsgTipType(type_msgbox_restart);
				SetMsgTipInfo(_T("配置完成,重启系统？"));
				ShowWindow(true);
				CenterWindow();
			}
		}
	}
}

CStdString CDlgMsgTip::GetSkinFolder()
{
	return _T("");
}

CStdString CDlgMsgTip::GetSkinFile()
{
	return _T("DlgMsgTip.xml");
}

LPCTSTR CDlgMsgTip::GetWindowClassName( void ) const
{
	return _T("DlgMsgTip");
}

EDialogResType CDlgMsgTip::GetResType() const
{
	return DRT_ZIP_INTERNAL;
}

HINSTANCE CDlgMsgTip::GetResouceInstance()
{
	return CPaintManagerUI::GetInstance();
}

unsigned int CDlgMsgTip::GetResourceID() const
{
	return IDR_ZIPRES2;
}

void CDlgMsgTip::SetMsgTipType( const MsgTipType& tipType )
{
	CHorizontalLayoutUI* pHorFunControl = static_cast<CHorizontalLayoutUI*>(paint_manager_.FindSubControlByName(GetRoot(), _T("HorFunControl")));
	ASSERT(pHorFunControl);
	if(tipType == m_uTipType) return;
	m_uTipType = tipType;
	pHorFunControl->SetVisible(!(tipType == type_msgnormal));
}

void CDlgMsgTip::SetMsgTipInfo( LPCTSTR strTipInfo, bool bTimer /*= false*/)
{
	m_pInfo = strTipInfo;
	CString csInfo = strTipInfo;
	if (bTimer)
	{
		csInfo.Format(_T("%s  %d"), strTipInfo, m_nCount);
		CoUI_SetTimer(USERMSG_TIMER, 1000);
		//关闭系统保护
		WinExec("uwfmgr filter disable",SW_HIDE);
	}
	if(NULL != m_plbTipInfo){
		m_plbTipInfo->SetText(csInfo);
	}
}

void CDlgMsgTip::StartTimerTest( UINT uID, UINT uTime )
{
	keybd_event(VK_LCONTROL,0,0,0);	//按下左ctrl
	keybd_event(VK_LMENU,0,0,0);	//按下左alt
	keybd_event(VK_F12,0,0,0);	
	keybd_event(VK_LCONTROL,0,KEYEVENTF_KEYUP,0);	//松开ctrl
	keybd_event(VK_LMENU,0,KEYEVENTF_KEYUP,0);	//松开alt
	keybd_event(VK_F12,0,KEYEVENTF_KEYUP,0);	//松开F12
	CoUI_SetTimer(uID, uTime);
}

void CDlgMsgTip::StopTimerTest( UINT uID )
{
	CoUI_KillTimer(uID);
}

int CDlgMsgTip::GetPrivateProfile()
{
	CString  strVersionPath = _T("D:\\AVCONXPE\\HPVersion.ini");
	return ::GetPrivateProfileInt(MODULE_CONFIGRESTART, MODULE_CONFIGRESTART, 0, strVersionPath);
}

void CDlgMsgTip::SetPrivateProfile( int nReStart )
{
	CString  strVersionPath = _T("D:\\AVCONXPE\\HPVersion.ini");
	CString strReStart;
	strReStart.Format(_T("%d"), nReStart);
	::WritePrivateProfileString(MODULE_CONFIGRESTART, MODULE_CONFIGRESTART, strReStart, strVersionPath);
}

LRESULT CDlgMsgTip::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled )
{
	/*if (WM_HOTKEY == uMsg)
	{
		if(wParam == m_nHotKeyId)
		{
 			int nConfig = -1;
 			nConfig = GetPrivateProfile();
 			if (nConfig == 0)
 			{
 				SetMsgTipType(type_msgbox_showset);
 				SetMsgTipInfo(_T("是否要进行显示设置配置？"));
 				ShowWindow(true);
 				CenterWindow();
 			}
		}
	}*/
	return WindowImplBaseBase::MessageHandler(uMsg, wParam, lParam, bHandled);
}

