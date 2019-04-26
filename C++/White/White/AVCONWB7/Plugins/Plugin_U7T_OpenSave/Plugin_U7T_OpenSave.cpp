#include "stdafx.h"
#include "Plugin_U7T_OpenSave.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace AVCONPlugin;

CPlugin_U7T_OpenSave::CPlugin_U7T_OpenSave(void)
	: m_pDlgOpenSave(NULL)
{
}

CPlugin_U7T_OpenSave::~CPlugin_U7T_OpenSave(void)
{
}

//////////////////////////////////////////////////////////////////////////
//继承IPlugin_U7T_OpenSave接口
HWND AVCONPlugin::CPlugin_U7T_OpenSave::CreateOpenSave( BOOL bOpenSave,LPCTSTR lpszDefExt /*= NULL*/,LPCTSTR lpszFilter /*= NULL*/, LPCTSTR strMemberID/*=_T("Local")*/,HWND hParent/*=NULL */,LPCTSTR strFilePath /*=_T("")*/)
{
	DestroyOpenSave();
	if (m_pDlgOpenSave == NULL)
	{
		m_pDlgOpenSave = new CDlgOpenSave(bOpenSave,lpszDefExt,lpszFilter,strMemberID,strFilePath);
		UINT style = UI_WNDSTYLE_FRAME;
		style&=~WS_MAXIMIZEBOX;
		m_pDlgOpenSave->Create(hParent, _T("DlgOpenSave"), style, /*WS_EX_TOOLWINDOW*/UI_WNDSTYLE_EX_DIALOG | WS_EX_TOPMOST , 0, 0, 650, 570);
	}
	m_pDlgOpenSave->ShowWindow(true,true);
	m_pDlgOpenSave->CenterWindow(hParent);
    AdjustWndWithMenuBar(m_pDlgOpenSave->GetHWND(),145);
	return m_pDlgOpenSave->GetHWND();
}

void AVCONPlugin::CPlugin_U7T_OpenSave::ShowDlg( bool bShow )
{
	if (m_pDlgOpenSave)
	{
		m_pDlgOpenSave->ShowWindow(bShow,false);
	}
	/*m_pDlgOpenSave->ShowWindow(bShow);*/
}

const char* AVCONPlugin::CPlugin_U7T_OpenSave::GetOpenFile( void )
{
	CString strOpenFile = _T("");
	if (m_pDlgOpenSave)
	{
		strOpenFile = m_pDlgOpenSave->OpenFile();
		//m_pDlgOpenSave->ShowWindow(SW_HIDE);
	}
	//在OnFinalMessage中销毁了指针
	//delete m_pDlgOpenSave; 
	m_pDlgOpenSave = NULL;
	m_strFilePathName = CCommFunction::U2A(strOpenFile);
	return m_strFilePathName.c_str();
}

void AVCONPlugin::CPlugin_U7T_OpenSave::DestroyOpenSave()
{
	if (m_pDlgOpenSave)
	{
		DestroyWindow(m_pDlgOpenSave->GetHWND());
		//在OnFinalMessage中销毁了指针
		//delete m_pDlgOpenSave;
		m_pDlgOpenSave = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// 继承IPlugin接口
std::string AVCONPlugin::CPlugin_U7T_OpenSave::Name()
{
	return "Plugin_U7T_OpenSave";
}

std::string AVCONPlugin::CPlugin_U7T_OpenSave::ExtendPoint()
{
	return "AVCONPlugin::IPlugin_U7T_OpenSave";
}

bool AVCONPlugin::CPlugin_U7T_OpenSave::InitPlugin()
{
	return true;
}

bool AVCONPlugin::CPlugin_U7T_OpenSave::ClosePlugin()
{
	DestroyOpenSave();
	return true;
}