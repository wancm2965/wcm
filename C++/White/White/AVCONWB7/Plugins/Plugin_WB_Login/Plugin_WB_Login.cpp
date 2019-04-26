// Plugin_WB_Login.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Plugin_WB_Login.h"
#include "IPlugin/Plugin_WB_Main/IPlugin_WB_Main.h"
//#include "IPlugin/IU7_IM/IPlugin_U7_IM_Login.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

AVCONPlugin::CPlugin_WB_Login::CPlugin_WB_Login()
{
	m_pWBLoginDlg = NULL;
}

AVCONPlugin::CPlugin_WB_Login::~CPlugin_WB_Login()
{
	
}

std::string AVCONPlugin::CPlugin_WB_Login::Name()
{
	return "Plugin_WB_Login";
}

std::string AVCONPlugin::CPlugin_WB_Login::ExtendPoint()
{
	return "AVCONPlugin::IPlugin_WB_Login";
}

bool AVCONPlugin::CPlugin_WB_Login::InitPlugin()
{
	return true;
}

bool AVCONPlugin::CPlugin_WB_Login::ClosePlugin()
{
	if (m_pWBLoginDlg != NULL)
	{
		DestroyWindow(m_pWBLoginDlg->GetHWND());
		delete m_pWBLoginDlg;
		m_pWBLoginDlg = NULL;
	}
	return false;
}


void AVCONPlugin::CPlugin_WB_Login::Show(bool bshow)
{
	GetLangPath();
	if (m_pWBLoginDlg == NULL)
	{
		m_pWBLoginDlg = new CWBLoginDlg;
		UINT style = UI_WNDSTYLE_FRAME;
		style&=~WS_MAXIMIZEBOX;
		HWND hWnd = NULL;
		IPlugin_WB_Main* pPlugin_main = dynamic_cast<IPlugin_WB_Main*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Main"));
		
		if (pPlugin_main)
		{
			hWnd = pPlugin_main->GetExeParentWnd();
		}
		m_pWBLoginDlg->Create(hWnd, _T("AVCON WB  Login"), style, WS_EX_TOOLWINDOW , 0, 0, 400, 300);
		::SetWindowPos(m_pWBLoginDlg->GetHWND(),HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		m_pWBLoginDlg->CenterWindow(::GetDesktopWindow());

	}
	else
	{
		if (m_pWBLoginDlg->GetButtonUI() != NULL)
		{
			CString strOnlineLogin;
			strOnlineLogin = ReadConfigText(_T("PluginText"), _T("LOGIN_OnlineLogin"));
			if (strOnlineLogin.IsEmpty())
			{
				strOnlineLogin = _T("联机登录");
			}
			m_pWBLoginDlg->GetButtonUI()->SetText(strOnlineLogin);

		}
		if (m_pWBLoginDlg->GetHWND()&& ::IsWindowVisible(m_pWBLoginDlg->GetHWND()))
		{
			m_pWBLoginDlg->ShowWindow(SW_HIDE);
		}
		else
		{
			m_pWBLoginDlg->ShowWindow(SW_SHOW);
		}
		m_pWBLoginDlg->SetLoginCancle(true);
		if (m_pWBLoginDlg->GetLabelUI())
		{
			CString strOnlineLogin = _T("");
			m_pWBLoginDlg->GetLabelUI()->SetText(strOnlineLogin);

		}

	}
}

void AVCONPlugin::CPlugin_WB_Login::Destroy()
{
	if (m_pWBLoginDlg != NULL)
	{
		m_pWBLoginDlg->DisConnect();
		::DestroyWindow(m_pWBLoginDlg->GetHWND());
		delete m_pWBLoginDlg;
		m_pWBLoginDlg = NULL;
	}

}
