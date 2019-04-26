#include "stdafx.h"
#include "DlgOpenSave.h"
#include "ExportPlugin.h"
#include "IPlugin/IU7_IM/IPlugin_U7_IM_Friend.h"
#include "IPlugin/IU7_MessageBox/IPlugin_U7T_MessageBox.h"
#include <IPlugin/IU7T.Comm/IPlugin_U7_Common.h>
#include "IPlugin/IU7_Config/IPlugin_U7_Cfg_Comm.h"
#include "IPlugin/IU7T_TipWindow/IU7T_TipWindow.h"

#include <Dbt.h>
#include <ShellAPI.h>
#include <ShlObj.h>
using namespace AVCONPlugin;

#define WM_TO_LONG			WM_USER + 1000
#define WM_IS_NULL			WM_USER + 1001
#define WM_IS_VALID			WM_USER + 1002
#define WM_SAVEFILE			WM_USER + 1003
#define WM_IS_READONLYDEVICE WM_USER + 1004
#define WM_IS_ROOT			WM_USER + 1005
// 移动设备起始ID，以这个ID为起始再加上设备盘号在26个字母中的索引
// 用来标识是哪个盘，例H盘的ID就是REMOVABLE_START+'H'-'A'
#define REMOVABLE_START		CSIDL_FLAG_MASK + 100 

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

const TCHAR* const kComSearchRangeControlName = _T("ComSearchRange");
const TCHAR* const kComFileNameControlName = _T("ComFileName");
const TCHAR* const kComFileTypeControlName = _T("ComFileType");
const TCHAR* const kListSearchRange = _T("ItemList");
const TCHAR* const kRecentOptControlName = _T("Opt_Recent");
const TCHAR* const kPersonalOptControlName = _T("Opt_Person");
const TCHAR* const kDesktopBtnControlName = _T("Btn_Dest");
const TCHAR* const kDrivesBtnControlName = _T("Btn_Computer");
TCHAR* const kCloseBtnControlName = _T("Btn_Colse");
const TCHAR* const kGoBackBtnControlName = _T("Btn_GoBack");
const TCHAR* const kUpwardBtnControlName = _T("Btn_Upward");
const TCHAR* const kNewFolderBtnControlName = _T("Btn_NewFolder");
const TCHAR* const kPreviewsBtnControlName = _T("Btn_Previews");
const TCHAR* const kOpenBtnControlName = _T("Btn_Open");
const TCHAR* const kRenameBtnControlName = _T("Btn_Rename");
const TCHAR* const kDeleteBtnControlName = _T("Btn_Delete");

const TCHAR* const kListContainerElementUIControlName = _T("ListItemUI");
const TCHAR* const kEditFileNameRichEditUIControlName = _T("EditFileName");


#pragma region //实现通用的对话框创建销毁方法
CDlgOpenSave* g_pDlgOpenSave = NULL;
CString g_strFullOutPath = _T("");

void DestroyDlgOpenSave(void)
{
	if (g_pDlgOpenSave)
	{
		DestroyWindow(g_pDlgOpenSave->GetHWND());
		delete g_pDlgOpenSave;
		g_pDlgOpenSave = NULL;
	}
}
#pragma endregion

CString PhrasesFilePath(CString fullPath, UINT limitLenght = 33)
{
	int findedIndex = 0;
	int startIndex = 0;
	UINT fullLength = fullPath.GetLength();
	UINT currentLength = 0;

	CString findedPath;

	std::list<CString> pathQueue;

	// get driver part
	findedIndex = fullPath.Find(':', startIndex);
	if(findedIndex < 0)
	{
		return fullPath;
	}
	findedPath = fullPath.Left(findedIndex + 1);
	pathQueue.push_front(findedPath);

	// get file part
	startIndex = findedIndex + 2;
	currentLength = fullLength;
	while(true)
	{
		int partLength = 0;
		findedIndex = fullPath.Find('\\', startIndex);

		if(currentLength < limitLenght || findedIndex < 0 || (UINT)findedIndex >= fullLength - 1)
		{
			findedPath = fullPath.Right(fullLength - startIndex);
			pathQueue.push_front(findedPath);
			break;
		}

		partLength = findedIndex - startIndex;
		if(partLength > 1)
		{
			findedPath = _T("..");
			pathQueue.push_front(findedPath);
			currentLength = currentLength - partLength + findedPath.GetLength();
		}

		startIndex = findedIndex + 1;
	}

	//re user findedPath
	findedPath.Empty();

	findedPath.Append(pathQueue.back());
	pathQueue.pop_back();

	while(pathQueue.size() > 0)
	{
		findedPath.Append('\\' + pathQueue.back());
		pathQueue.pop_back();
	}
	return findedPath;
}

CDlgOpenSave::CDlgOpenSave(BOOL bOpenSave, LPCTSTR lpszDefExt, LPCTSTR lpszFilter,CString strMemberID/*=_T("Local")*/,CString strFilePath /*= ""*/)
: m_bOpenSave(bOpenSave)
, m_bFirst(true)
, m_strFilter(lpszFilter)
, m_strDefExt(lpszDefExt)
, m_strDesktop(_T(""))
, m_strPersonal(_T(""))
, m_strRecent(_T(""))
, m_strPathName(_T(""))
, m_strCurPath(_T(""))
, m_strComFileName(_T(""))
, m_pLblTitle(NULL)//
, m_pLblSearchRange(NULL)
, m_pLblFileName(NULL)
, m_pLblFileType(NULL)
, m_pBtnClose(NULL)//
//, m_pOptRecent(NULL)
//, m_pOptPerson(NULL)
/*	, m_pBtnDest(NULL)*/
/*	, m_pBtnComputer(NULL)*/
, m_pBtnUpward(NULL)
, m_pBtnNewForlder(NULL)
, m_pBtnGoBack(NULL)
, m_pBtnPreciews(NULL)
, m_pBtnOpen(NULL)
, m_pBtnCancel(NULL)
, m_pComSearchRange(NULL)//
, m_pComFileName(NULL)
, m_pComFileType(NULL)
, m_pEditFileName(NULL)
, m_nCurFolder(-1)
, m_nCardIndex(-1)
, m_bIsClickOK(false)
, m_strFileFullPathOut(_T(""))
, m_nListIndex(-1)
, m_nIndex(-1)
, m_nCurIndexWnd(-1)
, m_eMark(BTN_MARK_RECENT)
, m_strMemberID(strMemberID)
, bRecentFileSelected(false)
, bPercentFileSelected(false)
, m_pItemList(NULL)
, m_bXPE(false)
, m_hDevNotify(NULL)
, m_nEditIndex(-1)
{
	g_strFullOutPath = _T("");
	IPlugin_U7_IM_Friend* pFriend = dynamic_cast<IPlugin_U7_IM_Friend*>(g_System->GetPluginByName("Plugin_U7_IM_Friend"));
	if (pFriend != NULL)
	{
		m_strMemberID = CCommFunction::A2U(pFriend->GetUserIDWithDomain());
	}
	ReleaseAllVector();
	if (bOpenSave == FALSE)
	{
		m_strSourceName = strFilePath;
	}
}

CDlgOpenSave::~CDlgOpenSave(void)
{
	//DestroyWindow(m_hWnd);
}
//////////////////////////////////////////////////////////////////////////
// 继承WindowImplBase接口
#pragma region 实现WindowImplBase接口

CString CDlgOpenSave::OpenFile()
{
	UINT nResult = ShowModal();
	//GetIMainFrameFun().RemoveWindow(this);
	if (nResult != IDOK)
		return _T("");

	return g_strFullOutPath;
}

void CDlgOpenSave::Init()
{
	m_initSize = paint_manager_.GetInitSize();

	IPlugin_U7_Cfg_Comm * pPlugin_U7_Cfg_Comm = dynamic_cast<IPlugin_U7_Cfg_Comm *>(g_System->GetPluginByName("Plugin_U7_Cfg_Comm"));
	if (pPlugin_U7_Cfg_Comm != NULL)
	{
		m_bXPE = pPlugin_U7_Cfg_Comm->IsXPE();
		//m_bXPE = true;
	}

	//////////////////////////////////////////////////////////////////////////
	//2012 9 14 hxl 先删除中间文件
	CString strFilePath;
	if (m_bXPE)
	{
		strFilePath = _T("d:\\AVCON7_XPE\\TempBmp");
		DeleteDirectory(strFilePath);
	}
// 	else
// 	{
		strFilePath.Format(_T("%sskin\\TempBmp"),GetExePath());
		DeleteDirectory(strFilePath);
/*	}*/
	//////////////////////////////////////////////////////////////////////////

	if (m_bFirst)
	{
		if (!InitControl())
		{
			IPlugin_U7T_TipWindow* pMsgBox = dynamic_cast<IPlugin_U7T_TipWindow*>(g_System->GetPluginByName("Plugin_U7T_TipWindow"));
			if (pMsgBox)
			{
				CString strErrorMessage = _T("控件初始化失败!"); 
				CString strMessageTitle = _T("提示"); 
				if (GetPluginU7Lang())
				{
					strErrorMessage	= GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("CONTROL_INIT_FAILED"), _T("控件初始化失败!"));
					strMessageTitle = GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("MESSAGE_TITLE_PROMPT"), _T("提示"));
				}
				pMsgBox->ShowTipWindow(strErrorMessage, 3, GetHWND());
			}
		}
		
		InitBtn();
	}

	m_bFirst = false;

	CString strTitle	= _T("打开文件");
	CString strBtnOpen	= _T("打开");
	if (m_bOpenSave)//true 打开对话框
	{
		if (GetPluginU7Lang())
		{
			strTitle	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("DLG_TITLE_OPEN"), _T("打开文件"));
			strBtnOpen	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("BTN_OPEN"), _T("打开"));
		}
		m_pLblTitle->SetText(strTitle);
		m_pBtnOpen->SetText(strBtnOpen);
		m_pComFileName->SetVisible(true);
	} 
	else//保存对话框
	{
		if (GetPluginU7Lang())
		{
			strTitle	= _T("保存文件");
			strBtnOpen	= _T("保存");
			strTitle	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("DLG_TITLE_SAVE"), _T("保存文件"));
			strBtnOpen	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("BTN_SAVE"), _T("保存"));
		}

		m_pLblTitle->SetText(strTitle);
		m_pBtnOpen->SetText(strBtnOpen);
		m_pEditFileName->SetVisible(true);
		m_pComFileName->SetVisible(false);
		if (!m_strSourceName.IsEmpty())
		{
			//hxl 2012 8 30 另存时只显示文件名
			int nPos1 = m_strSourceName.ReverseFind('\\');
			int nPos2 = m_strSourceName.Find(m_strDefExt);
			if (nPos1==-1)
			{
				nPos1 =  m_strSourceName.ReverseFind('//');
			}
			if (nPos2 > 0)
			{
				CString strSaveName = _T("");
				if (nPos1 == -1)
				{
					strSaveName = m_strSourceName.Left(nPos2);
				}
				else
				{
					strSaveName = m_strSourceName.Mid(nPos1+1,nPos2-nPos1-1);
				}
				m_pEditFileName->SetText(strSaveName);
			}
			else
			{
				CString strSaveName = _T("");
				strSaveName = m_strSourceName.Mid(nPos1+1,m_strSourceName.GetLength()-nPos1-1);
				m_pEditFileName->SetText(strSaveName);
			}
			//m_pBtnOpen->SetEnabled(true);
		}
	}
	CString strFiltertemp = m_strFilter;
	if (!strFiltertemp.IsEmpty())
	{
		int nIndex = strFiltertemp.Find(_T("||"));
		if (nIndex < 0)
		{
			
			IPlugin_U7T_TipWindow* pMsgBox = dynamic_cast<IPlugin_U7T_TipWindow*>(g_System->GetPluginByName("Plugin_U7T_TipWindow"));
			if (pMsgBox)
			{
				CString strErrorMessage = _T("控件初始化失败!"); 
				CString strMessageTitle = _T("提示"); 
				if (GetPluginU7Lang())
				{
					strErrorMessage	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("CONTROL_INIT_FAILED"), _T("文件类型格式错误"));
					strMessageTitle = GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("MESSAGE_TITLE_PROMPT"), _T("提示"));
				}
				pMsgBox->ShowTipWindow(strErrorMessage, 3, GetHWND());
			}
		}
		else
		{
			CString strType, strTypeText;
			nIndex = strFiltertemp.ReverseFind('|');
			strFiltertemp = strFiltertemp.Left(nIndex-1);
			do 
			{
				strType = strFiltertemp.Mid(strFiltertemp.ReverseFind('|')+1);
				strFiltertemp = strFiltertemp.Left(strFiltertemp.ReverseFind('|') );
				strTypeText = strFiltertemp.Mid(strFiltertemp.ReverseFind('|')+1);
				strFiltertemp = strFiltertemp.Left(strFiltertemp.ReverseFind('|'));
				m_vFileType.push_back(strType);
				CString strAdd2Blank;
				strAdd2Blank.Format(_T("  %s"),strTypeText);
				m_pComFileType->Add(strAdd2Blank);
			} while (strFiltertemp.ReverseFind('|') > 0);
		}
	}
	else
	{
		/*m_pComFileType->Add(LoadLangStr(_T("FILESHARE"),_T("ALL_FILE")));*/
		CString strAllFileLand = _T("所有文件(*.*)"); 
		if (GetPluginU7Lang())
		{
			strAllFileLand	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILESHARE_ALL_FILE"), _T("所有文件(*.*)"));
		}
		m_pComFileType->Add(strAllFileLand);
		m_vFileType.push_back(_T("*.*"));
	}
	m_pComFileType->SelectItem(0);
	m_vShowType.clear();
	SetCurFileTypes();

	//将本窗口加入到UI控制队列中
	IPlugin_U7_ComUI* pluginU7ComUI = dynamic_cast<IPlugin_U7_ComUI*>(AVCONPlugin::g_System->GetPluginByName(PLUGIN_NAME_U7_COMUI));
	if (pluginU7ComUI)
	{

		TUISetting uiset = pluginU7ComUI->GetUISetting();
		const TFontInfo* pFontinfo = pluginU7ComUI->GetDefaultFont();
		paint_manager_.SetDefaultFont(pFontinfo->sFontName, uiset.nFontSize, pFontinfo->bBold, pFontinfo->bUnderline, pFontinfo->bItalic);

		pluginU7ComUI->AddObserver(this);
	}

	if(!m_dbOpenRecord.Connect(m_strMemberID))
	{
		IPlugin_U7T_TipWindow* pMsgBox = dynamic_cast<IPlugin_U7T_TipWindow*>(g_System->GetPluginByName("Plugin_U7T_TipWindow"));
		if (pMsgBox)
		{
			CString strMessageTitle = _T("提示"); 
			CString strLang= _T("链接用户数据库失败!"); 
			if (GetPluginU7Lang())
			{
				strMessageTitle = GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("MESSAGE_TITLE_PROMPT"), _T("提示"));
				strLang	= GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("DATABASE_LINK_FAILURE"), _T("链接用户数据库失败!"));
			}
			pMsgBox->ShowTipWindow(strLang, 3, GetHWND());
		}
	}

	OpenLastDir();		// 打开上次目录

	// 注册设备通知监视	wangxin 2012/09/10 11
	// USB设备的GUID	wangxin 2012/09/10 11
	GUID InterfaceClassGuid = {0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED};

	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_classguid = InterfaceClassGuid;

	m_hDevNotify = RegisterDeviceNotification(m_hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

	//开启鼠标手势支持
	//paint_manager_.EnableSysMTParser(MOUSE_GESTURE_PARSER);
}

LRESULT CDlgOpenSave::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ReleaseAllVector();
	m_dbOpenRecord.Disconnect();
	
	// 删除临时文件夹中所有文件	wangxin 2012/09/06 15
	CString strFilePath;
	strFilePath.Format(_T("%sskin\\TempBmp"),GetExePath());
	DeleteDirectory(strFilePath);

	//将本窗口从UI控制队列中移除
	IPlugin_U7_ComUI* pluginU7ComUI = dynamic_cast<IPlugin_U7_ComUI*>(AVCONPlugin::g_System->GetPluginByName(PLUGIN_NAME_U7_COMUI));
	if (pluginU7ComUI)
	{
		pluginU7ComUI->DelObserver(this);
	}

	// 反注册设备监视	wangxin 2012/09/10 11
	if (NULL != m_hDevNotify)
	{
		UnregisterDeviceNotification(m_hDevNotify);
		m_hDevNotify = NULL;
	}

	return WindowImplBase::OnDestroy(uMsg, wParam, lParam, bHandled);
}

LPCTSTR CDlgOpenSave::GetWindowClassName() const
{
	return _T("DlgOpenSave");
}

CString CDlgOpenSave::GetSkinFile()
{
	return _T("DlgOpenSave.xml");
}

CControlUI* CDlgOpenSave::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, _T("ListUIFile")) == 0)
	{
		return new CListUIFile(paint_manager_);
	}

	return NULL;
}

LRESULT CDlgOpenSave::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	if (WM_TO_LONG == uMsg)
	{
		IPlugin *pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_TipWindow");
		IPlugin_U7T_TipWindow *pMessageBox = dynamic_cast<IPlugin_U7T_TipWindow*>(pIPlugin);
		if (pMessageBox != NULL)
		{
			CString strMessageTitle = _T("提示"); 
			CString strLang= _T("文件名太长!"); 
			if (GetPluginU7Lang())
			{
				strMessageTitle = GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("MESSAGE_TITLE_PROMPT"), _T("提示"));
				strLang	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILEERROR_NAMETOOLONG"), _T("文件名太长!"));
			}
			pMessageBox->ShowTipWindow(strLang, 3, GetHWND());
		}
	}
	else if (WM_IS_ROOT == uMsg)
	{
		IPlugin *pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_TipWindow");
		IPlugin_U7T_TipWindow *pTipWindow = dynamic_cast<IPlugin_U7T_TipWindow*>(pIPlugin);
		if (pTipWindow != NULL)
		{
			CString strMessageTitle = _T("提示"); 
			CString strLang= _T("链接用户数据库失败!"); 
			if (GetPluginU7Lang())
			{
				strMessageTitle = GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("MESSAGE_TITLE_PROMPT"), _T("提示"));
				strLang	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILEERROR_UNABLEOPEN"), _T("无法打开该目录"));
			}
			pTipWindow->ShowTipWindow(strLang, 3, GetHWND());
		}
	}
	else if (WM_SAVEFILE == uMsg)
	{
		bHandled = TRUE;
		if (wParam == 0)
		{
			if(!SaveFilePath())
				return 0;
			OnOpenOK();
		}
		else
		{
			if (!m_bOpenSave)
			{
				if(!SaveFilePath())
					return 0;
			}
			OnOpenOK();
			m_vListItem.clear();
			m_pListContent->RemoveAll();
			m_strComFileName.Empty();
		}
	}
	else if (WM_IS_NULL == uMsg)
	{
		IPlugin *pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_TipWindow");
		IPlugin_U7T_TipWindow *pMessageBox = dynamic_cast<IPlugin_U7T_TipWindow*>(pIPlugin);
		if (pMessageBox != NULL)
		{
			CString strMessageTitle = _T("提示"); 
			CString strLang= _T("文件名为空，无法保存!"); 
			if (GetPluginU7Lang())
			{
				strMessageTitle = GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("MESSAGE_TITLE_PROMPT"), _T("提示"));
				strLang	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILEERROR_NAMEEMPTY_UNABLESAVE"), _T("文件名为空，无法保存!"));
			}
			pMessageBox->ShowTipWindow(strLang, 3, GetHWND());
		}
	}
	else if (WM_IS_READONLYDEVICE == uMsg)
	{
		IPlugin *pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_TipWindow");
		IPlugin_U7T_TipWindow *pMessageBox = dynamic_cast<IPlugin_U7T_TipWindow*>(pIPlugin);
		if (pMessageBox != NULL)
		{
			CString strMessageTitle = _T("提示"); 
			CString strLang= _T("无法将文件保存在选择的目录，请重新选择!"); 
			if (GetPluginU7Lang())
			{
				strMessageTitle = GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("MESSAGE_TITLE_PROMPT"), _T("提示"));
				strLang	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILEERROR_UNABLESAVE_RETRAY"), _T("无法将文件保存在选择的目录，请重新选择!"));
			}
			pMessageBox->ShowTipWindow(strLang, 3, GetHWND());
		}
	}
	else if (WM_IS_VALID == uMsg)
	{
		bHandled = TRUE;
		IPlugin *pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_TipWindow");
		IPlugin_U7T_TipWindow *pMessageBox = dynamic_cast<IPlugin_U7T_TipWindow*>(pIPlugin);
		if (pMessageBox != NULL)
		{
			CString strMessageTitle = _T("提示"); 
			CString strLang= _T("无法将文件保存在选择的目录，请重新选择!"); 
			if (GetPluginU7Lang())
			{
				strMessageTitle = GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("MESSAGE_TITLE_PROMPT"), _T("提示"));
				strLang	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILEERROR_NAMEILLEGAL"), _T("文件名不能包含下列字符： *:|\"<>?/"));
			}
			pMessageBox->ShowTipWindow(strLang, 3, GetHWND());
		}
	}
	else if (WM_DEVICECHANGE == uMsg)	// 设备改变
	{
		if (DBT_DEVICEARRIVAL == wParam) // 设备插入
		{
			PDEV_BROADCAST_HDR pDevHdr = (PDEV_BROADCAST_HDR)lParam;
			if(DBT_DEVTYP_VOLUME == pDevHdr->dbch_devicetype)
			{
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)pDevHdr;
				DeviceArrival(lpdbv->dbcv_unitmask);
			}
			// 暂时保留这段代码	wangxin 2012/09/10 15
			//else if (DBT_DEVTYP_DEVICEINTERFACE == pDevHdr->dbch_devicetype)
			//{
			//	PDEV_BROADCAST_DEVICEINTERFACE pS = (PDEV_BROADCAST_DEVICEINTERFACE)pDevHdr;
			//}
		}
		else if(DBT_DEVICEREMOVECOMPLETE == wParam)	// 设备移除
		{
			PDEV_BROADCAST_HDR pDevHdr = (PDEV_BROADCAST_HDR)lParam;

			if(DBT_DEVTYP_VOLUME == pDevHdr->dbch_devicetype)
			{
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)pDevHdr;
				DeviceRemove(lpdbv->dbcv_unitmask);
			}
			/*else if (DBT_DEVTYP_DEVICEINTERFACE == pDevHdr->dbch_devicetype)
			{
				PDEV_BROADCAST_DEVICEINTERFACE pS = (PDEV_BROADCAST_DEVICEINTERFACE)pDevHdr;
			}*/
		}
	}

	return 0;
}

void CDlgOpenSave::Notify(TNotifyUI& msg)
{
	OPENSAVEBTN OpenSaveBtn = OSB_BTN_NULL;
	OPENSAVECOMBOX OpenSaveCom = OSB_COM_NONETYPE;
	if (COUI_MSGTYPE_ITEMCLICK == msg.nType)
	{
		EditFileNameEnd();
		if ((_tcsicmp(msg.pSender->GetName(), kListContainerElementUIControlName)==0) && (m_nListIndex == msg.pSender->GetTag()) && m_eMark == BTN_MARK_PERSONAL) 
		{
			OnBeginEditFileName(m_nListIndex);
		}
// 		else if (_tcsicmp(msg.pSender->GetName(), kListContainerElementUIControlName) ==0)
// 		{
// 			m_nListIndex = msg.pSender->GetTag();
// 			OnListItemchanged(m_nListIndex);
// 		}
	}
	else if (COUI_MSGTYPE_TEXTCHANGED == msg.nType)
	{
		if (_tcsicmp(msg.pSender->GetName(), kEditFileNameRichEditUIControlName) ==0)
		{
			OnUpdateOpenSaveButState(m_pEditFileName->GetText() != _T(""));
		}
	}
	else if (COUI_MSGTYPE_ITEMACTIVATE == msg.nType)
	{
		if (_tcsicmp(msg.pSender->GetName(), kListContainerElementUIControlName) ==0)
		{
			int iIndex = msg.pSender->GetTag();
			OnDblclkList(iIndex);
		}
	}
	else if (COUI_MSGTYPE_ITEMSELECT == msg.nType)
	{
		if (_tcsicmp(msg.pSender->GetName(), kComSearchRangeControlName) == 0)
		{
			OpenSaveCom = OSB_COM_SEARCHRANGE;
			m_pItemList->SelectItem(m_pComSearchRange->GetCurSel());
		}
		else if (_tcsicmp(msg.pSender->GetName(), kComFileNameControlName) == 0)
		{
			OpenSaveCom = OSB_COM_FILENAME;
		}
		else if (_tcsicmp(msg.pSender->GetName(), kComFileTypeControlName) == 0)
		{
			OpenSaveCom = OSB_COM_FILETYPE;
		}
		else if ((_tcsicmp(msg.pSender->GetName(), 	kListSearchRange)==0))
		{
			int nIndex = m_pItemList->GetCurSel();
			if ( -1 == nIndex)
			{
				m_pComSearchRange->SelectItem( m_nCurIndexWnd );
				m_pItemList->SelectItem( m_nCurIndexWnd );
			}
			else
			{
				m_pComSearchRange->SelectItem( nIndex );
				m_nCurIndexWnd = nIndex;
			}
			if ( m_bOpenSave )
			{
				switch (m_nCurIndexWnd)
				{
				case 0:
					m_strCurPath = m_strRecent;
					break;
				case 1:
					m_strCurPath = m_strPersonal;
					break;
				case 2:
					m_strCurPath = m_strDesktop;
					break;
				case 3:
					m_strCurPath = GetPluginsLangString( _T("MY_COMPUTER"), _T("我的电脑"));
					break;
				}
			}
			else
			{
				switch (m_nCurIndexWnd)
				{
				case 0:
					m_strCurPath = m_strPersonal;
					break;
				case 1:
					m_strCurPath = m_strDesktop;
					break;
				case 2:
					m_strCurPath = GetPluginsLangString( _T("MY_COMPUTER"), _T("我的电脑"));
					break;
				}
			}	
			//////////////////////////////////////////////////////////////////////////
			//将当前目录显示到标题栏
			CString strPath = _T("");
			strPath.Format(_T("(%s)"), PhrasesFilePath(m_strCurPath));
			m_pLblPath->SetText(strPath);	// 将当目录显示到窗口上
			m_pLblPath->SetToolTip(m_strCurPath);
			//////////////////////////////////////////////////////////////////////////
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("ListContent")) ==0)
		{
			if (-1 != msg.wParam) 
			{
				m_nListIndex = m_pListContent->GetCurSel();
				OnListItemchanged(m_nListIndex);
			}
			else
			{
				//表示没有项被选中
				m_pBtnOpen->SetEnabled(false);
				m_pBtnRename->SetEnabled(false);
				m_pBtnDelete->SetEnabled(false);
			}
		}
		else
			return;
		OnComboxSelChanged(OpenSaveCom);
		return;
	}
	else if (COUI_MSGTYPE_WINDOWINIT == msg.nType)
	{
	}
	else if (COUI_MSGTYPE_KILLFOCUS == msg.nType)
	{
		EditFileNameEnd();
		if (msg.pSender->GetName() == _T("EditAndRenameFile"))
		{
			OnEndEditFileName();
		}
	}
	else if (COUI_MSGTYPE_SETFOCUS == msg.nType)
	{
	}
	else if (COUI_MSGTYPE_CLICK == msg.nType)
	{
		if( msg.pSender == m_pBtnClose || msg.pSender == m_pBtnCancel) 
		{
			ShowWindow(false, false);
			OpenSaveBtn = OSB_BTN_CLOSE;
		}
		else if (_tcsicmp(msg.pSender->GetName(), kCloseBtnControlName) == 0)
		{
			OpenSaveBtn = OSB_BTN_CLOSE;
		}
		else if (_tcsicmp(msg.pSender->GetName(), kOpenBtnControlName) == 0)
		{
			OpenSaveBtn = OSB_BTN_OPEN;
		}
		else if (_tcsicmp(msg.pSender->GetName(), kGoBackBtnControlName) == 0)
		{
			OpenSaveBtn = OSB_BTN_GOBACK;
		}
		else if (_tcsicmp(msg.pSender->GetName(), kUpwardBtnControlName) == 0)
		{
			OpenSaveBtn = OSB_BTN_UPWARD;
		}
		else if (_tcsicmp(msg.pSender->GetName(), kNewFolderBtnControlName) == 0)
		{
			OpenSaveBtn = OSB_BTN_NEWFOLDER;
		}
		else if (_tcsicmp(msg.pSender->GetName(), kPreviewsBtnControlName) == 0)
		{
			OpenSaveBtn = OSB_BTN_PREVIEWS;
		}
		else if (_tcsicmp(msg.pSender->GetName(), kRenameBtnControlName) == 0)
		{
			OpenSaveBtn = OSB_BTN_RENAME;
		}
		else if (_tcsicmp(msg.pSender->GetName(), kDeleteBtnControlName) == 0)
		{
			OpenSaveBtn = OSB_BTN_DELETE;
		}
		OnOpenSaveButton(OpenSaveBtn);
		return;
	}
	else if (COUI_MSGTYPE_SCROLL == msg.nType)
	{
		EditFileNameEnd();
	}
}

#pragma endregion


bool CDlgOpenSave::InitControl()
{
	CString strPluginName = _T("Plugin_U7T_OpenSave");

	m_pLblTitle =static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LblTitle")));
	if ( NULL == m_pLblTitle )
		return false;

	m_pLblPath = static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LblPath")));
	if (NULL == m_pLblPath)
		return false;
	if (m_bXPE)
		m_pLblPath->SetVisible(false);

	m_pLblSearchRange =static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LblSearchRange")));
	if ( NULL == m_pLblSearchRange )
		return false;
	CString strLbSearch = _T("查找范围：");
	if (GetPluginU7Lang())
		strLbSearch = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("LBL_SEARCH"),_T("查找范围："));
	m_pLblSearchRange->SetText(strLbSearch);

	m_pLblIcon =static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LblIcon")));
	if ( NULL == m_pLblIcon )
		return false;
	CString strLbIcon = _T(" 图标");
	if (GetPluginU7Lang())
		strLbIcon = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("LBL_ICON"),_T(" 图标"));
	m_pLblIcon->SetText(strLbIcon);

	m_pLblFileSize =static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LblFileSize")));
	if ( NULL == m_pLblFileSize )
		return false;
	CString strLbFileSize = _T("文件大小");
	if (GetPluginU7Lang())
		strLbFileSize = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("LBL_FILESIZE"),_T("文件大小"));
	m_pLblFileSize->SetText(strLbFileSize);

	m_pLblFileName =static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LblFileName")));
	if ( NULL == m_pLblFileName )
		return false;
	CString strLbFileName = _T("文件名");
	if (GetPluginU7Lang())
		strLbFileName = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("LBL_FILENAME"),_T("文件名"));
	m_pLblFileName->SetText(strLbFileName);

	m_pLblFileName =static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LblFileName1")));
	if ( NULL == m_pLblFileName )
		return false;
	CString strLbFileName1 = _T("文件名：");
	if (GetPluginU7Lang())
		strLbFileName1 = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("LBL_FILENAME1"),_T("文件名："));
	m_pLblFileName->SetText(strLbFileName1);

	m_pLblFileType =static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LblFileType")));
	if ( NULL == m_pLblFileType )
		return false;
	CString strLbFileType = _T("文件类型：");
	if (GetPluginU7Lang())
		strLbFileType = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("LBL_FILETYPE"),_T("文件类型："));
	m_pLblFileType->SetText(strLbFileType);

	m_pBtnClose  = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Close")));
	if ( NULL == m_pBtnClose )
		return false;
	CString strBtnClose = _T("关闭");
	if (GetPluginU7Lang())
		strBtnClose = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("BTN_CLOSE"),_T("关闭"));
	m_pBtnClose->SetText(strBtnClose);

	m_pItemList = static_cast<CListUI*>(paint_manager_.FindControl(_T("ItemList")));
	if ( NULL == m_pItemList )
		return false;
	
	//m_pItemList->SetFixedWidth(105);
	
	//m_pOptRecent = static_cast<COptionUI*>(paint_manager_.FindControl(_T("Opt_Recent")));
	//if ( NULL == m_pOptRecent )
	//	return false;
	//m_pOptPerson  = static_cast<COptionUI*>(paint_manager_.FindControl(_T("Opt_Person")));
	//if ( NULL == m_pOptPerson )
	//	return false;
	// 	m_pBtnDest  = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Dest")));
	// 	if ( NULL == m_pBtnDest )
	// 		return false;
	// 	m_pBtnComputer  = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Computer")));
	// 	if ( NULL == m_pBtnComputer )
	// 		return false;
	m_pBtnUpward  = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Upward")));
	if ( NULL == m_pBtnUpward )
		return false;
	CString strBtnUpward = _T("上一级");
	if (GetPluginU7Lang())
		strBtnUpward = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("BTN_UPWARD"),_T("上一级"));
	m_pBtnUpward->SetToolTip(strBtnUpward);

	m_pBtnNewForlder  = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_NewFolder")));
	if ( NULL == m_pBtnNewForlder )
		return false;
	CString strBtnNewFolder = _T("新建");
	if (GetPluginU7Lang())
		strBtnNewFolder = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("BTN_NEWF"),_T("新建"));
	m_pBtnNewForlder->SetToolTip(strBtnNewFolder);

	m_pBtnRename = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Rename")));
	if ( NULL == m_pBtnRename )
		return false;
	CString strBtnRename = _T("重命名");
	if (GetPluginU7Lang())
		strBtnRename = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("BTN_RENAME"),_T("重命名"));
	m_pBtnRename->SetToolTip(strBtnRename);
	m_pBtnRename->SetEnabled(false);

	m_pBtnDelete  = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Delete")));
	if ( NULL == m_pBtnDelete )
		return false;
	CString strBtnDelete = _T("删除");
	if (GetPluginU7Lang())
		strBtnDelete = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("BTN_DELETE"),_T("删除"));
	m_pBtnDelete->SetToolTip(strBtnDelete);
	m_pBtnDelete->SetEnabled(false);

	/*m_pBtnGoBack  = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_GoBack")));
	if ( NULL == m_pBtnGoBack )
		return false;*/
	/*m_pBtnPreciews  = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Previews")));
	if ( NULL == m_pBtnPreciews )
		return false;*/
	m_pBtnOpen  = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Open")));
	if ( NULL == m_pBtnOpen )
		return false;

	m_pBtnCancel = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Cancel")));
	if (NULL == m_pBtnCancel)
		return false;
	CString strBtnCancel = _T("取消");
	if (GetPluginU7Lang())
		strBtnCancel = GetPluginU7Lang()->GetPluginsLangString(strPluginName,_T("BTN_CANCEL"),_T("取消"));
	m_pBtnCancel->SetText(strBtnCancel);

	m_pComSearchRange = static_cast<CComboUI*>(paint_manager_.FindControl(_T("ComSearchRange")));
	if ( NULL == m_pComSearchRange )
		return false;
	m_pComSearchRange->SetAutoDropWidth(true);
	m_pComSearchRange->SetFixedHeight(40);

	m_pComFileName = static_cast<CComboUI*>(paint_manager_.FindControl(_T("ComFileName")));
	if ( NULL == m_pComFileName )
		return false;
	m_pComFileName->SetAutoDropWidth(true);
	m_pComFileName->SetFixedHeight(40);
	m_pComFileType = static_cast<CComboUI*>(paint_manager_.FindControl(_T("ComFileType")));
	if ( NULL == m_pComFileType )
		return false;
	m_pComFileType->SetAutoDropWidth(true);
	m_pComFileType->SetFixedHeight(40);
	m_pComFileType->SetAttribute(_T("endellipsis"),_T("true"));
	m_pListContent = static_cast<CListUIFile*>(paint_manager_.FindControl(_T("ListContent")));
	if ( NULL == m_pListContent )
		return false;
	m_pEditFileName = dynamic_cast<CEditUI*>(paint_manager_.FindControl(_T("EditFileName")));
	if ( NULL == m_pEditFileName )
		return false;

	return true;
}

void CDlgOpenSave::InitBtn()
{
	// 初始化按钮	wangxin 2012/09/06 13
	CString strLangRecent	= _T("最近文档");
	CString strLangMy		= _T("我的文档");
	CString strDesktopLang	= _T("桌面");
	CString strMyComputerLang = _T("我的电脑");
	if (GetPluginU7Lang())
	{
		strLangRecent	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILESHARE_RECENTDOC"), _T("最近文档"));
		strLangMy		= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILESHARE_MYDOC"), _T("我的文档"));
		strDesktopLang	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILESHARE_DESKTOP"), _T("桌面"));
		strMyComputerLang = GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILESHARE_MYCOMPUTER"), _T("我的电脑"));
	}
	int nIndex = 0;
	CControlUI* pListItem = NULL;
	if (m_bOpenSave)
	{
		pListItem = CreateBtnItem(_T("OSrecentfile.png"),strLangRecent);
		ASSERT(pListItem != NULL);
		m_mapCombo.insert(std::pair<int, int>(nIndex, CSIDL_RECENT));
		++nIndex;
	}
	pListItem = CreateBtnItem(_T("OSmyfile.png"), strLangMy); 
	ASSERT(pListItem != NULL);
	m_mapCombo.insert(std::pair<int, int>(nIndex, CSIDL_PERSONAL));
	++nIndex;

	if (NULL != m_pComSearchRange)
	{
		if(m_bOpenSave)
		{
			m_pComSearchRange->Add(strLangRecent);
		}
		m_pComSearchRange->Add(strLangMy);
	}

	// 根据	wangxin 2012/09/06 13
	if (m_bXPE)
	{
		InitRemovableDiskBtn();
	}
	else
	{
		if (NULL != m_pComSearchRange)
		{
 			m_pComSearchRange->Add(strDesktopLang);
			pListItem = CreateBtnItem(_T("OSdesktop.png"), strDesktopLang); 
			ASSERT(pListItem != NULL);
			m_mapCombo.insert(std::pair<int, int>(nIndex, CSIDL_DESKTOPDIRECTORY));
			++nIndex;
			m_pComSearchRange->Add(strMyComputerLang);
			pListItem = CreateBtnItem(_T("OScomputer.png"), strMyComputerLang); 
			ASSERT(pListItem != NULL);
			m_mapCombo.insert(std::pair<int, int>(nIndex, CSIDL_DRIVES));
		}
	}
}

void CDlgOpenSave::InitRemovableDiskBtn()
{
	// 遍历所有磁盘	wangxin 2012/09/06 20
	DWORD   dwDrv = ::GetLogicalDrives(); 
	CString   strDrv = _T( "?:\\"); 
	for(int i = 0; dwDrv; ++i)
	{ 
		if(dwDrv & 1) 
		{ 
			strDrv.SetAt(0, _T('A') + i); 
			
			bool bRemovable = false;
			UINT nType = GetDriveType(strDrv);

			if (nType == DRIVE_FIXED) // 移动硬盘及大容量U盘也会是这个属性，所以要判断其总线类型
			{
				CString strDeviceName =  _T("\\\\.\\") + strDrv.Left(2);
				// 获取设备句柄
				HANDLE hDevice = CreateFile(strDeviceName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL); //打开设备，磁盘分区的路径
				PSTORAGE_DEVICE_DESCRIPTOR pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
				pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;

				// 获取磁盘属性
				if(GetDisksProperty(hDevice, pDevDesc))
				{
					if(pDevDesc->BusType == BusTypeUsb) //总线类型是USB
					{
						bRemovable = true;
					}						
				}

				delete []pDevDesc;
			}
			else if (nType == DRIVE_REMOVABLE)	// U盘会进这里
			{
				TCHAR lpDeviceName[1000];
				QueryDosDevice(strDrv.Left(2), lpDeviceName, 1000);
				CString strTemp = lpDeviceName;
				if (strTemp.Find(_T("Floppy")) == -1)// 排除软驱
				{
					bRemovable = true;
				}
			}

			if (bRemovable)
			{
				AddDevice(strDrv);
			}
		} 
		dwDrv  >>= 1; 
	}	
}

void CDlgOpenSave::AddDevice(CString strDeviceName)
{
	DWORD serialNumber, maxComponentLength, fsFlags;
	TCHAR szFileSystem[12];
	TCHAR szVolumeName[24];
	if (!GetVolumeInformation(
		strDeviceName,			// 卷的根路径
		szVolumeName,			// 卷标
		sizeof(szVolumeName),	// 长度
		&serialNumber,			// 磁盘卷序列号
		&maxComponentLength,	
		&fsFlags,				// 文件系统标记
		szFileSystem,			// 文件系统名称
		sizeof(szFileSystem))) 
	{
		OutputDebugString(_T("Failed to retrieve drive information\n"));
	}

	CString strVolumeName = _T("可移动磁盘");
	if (GetPluginU7Lang())
	{
		strVolumeName	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILESHARE_VOLUME"), _T("可移动磁盘"));
	}
	if (_tcsicmp(szVolumeName, _T("")) != 0)
	{
		strVolumeName = szVolumeName;
	}

	if (m_bXPE)
	{
		CControlUI *pBtn = CreateBtnItem(_T("OSremovable.png"), strVolumeName);
		ASSERT(pBtn != NULL);

		ASSERT(NULL != m_pComSearchRange);
		if (NULL != m_pComSearchRange)
		{
			m_pComSearchRange->Add(strVolumeName);
		}
		int nIndex = strDeviceName.GetAt(0) - 'A';
		m_mapCombo.insert(std::pair<int, int>((int)m_mapCombo.size(), REMOVABLE_START + nIndex));
	}
	else if (m_eMark == BTN_MARK_DRIVES)
	{
		SHFILEINFO      fileInfo;
		ListItem        IT;

		CString strVolumeText;
		strVolumeText.Format(_T("%s(%s)"), strVolumeName, strDeviceName.Left(2));

		SHGetFileInfo(strDeviceName.Left(2), FILE_ATTRIBUTE_NORMAL,&fileInfo,
			sizeof(fileInfo),SHGFI_DISPLAYNAME|SHGFI_ICON|SHGFI_USEFILEATTRIBUTES|SHGFI_LARGEICON);//SHGFI_DISPLAYNAME | SHGFI_TYPENAME | SHGFI_ICON | SHGFI_LARGEICON | SHGFI_SYSICONINDEX

		IT.strFilePath = strDeviceName.Left(2);
		IT.dwFileType = F_NORMAL;
		OnNotifyShowListContent(strVolumeText, fileInfo, m_pListContent->GetCount(), IT);
	}

	CString str;
	str.Format(_T("卷标: %s\n分区类型: %s\n卷序列号: %04X-%04X\n"),
		strVolumeName,
		szFileSystem,
		serialNumber >> 16,
		serialNumber & 0xFFFF);
	OutputDebugString(str);
}

void CDlgOpenSave::DeviceArrival(DWORD dwMask)
{
	CString strDrv = _T("C:\\");
	for(int i = 0; dwMask; ++i)
	{ 
		if(dwMask & 1) 
		{
			strDrv.SetAt(0, _T('A') + i); 
			AddDevice(strDrv);
		}
		dwMask  >>= 1;
	}
}

void CDlgOpenSave::DeviceRemove(DWORD dwMask)
{
	for(int i = 0; dwMask; ++i)
	{ 
		if(dwMask & 1) 
		{
			if (m_bXPE)
			{
				MAP_COMBO_IT it = m_mapCombo.begin();
				for (; it != m_mapCombo.end(); ++it)
				{
					if (it->second == (REMOVABLE_START + i))
					{
						// 从combo控件中移除
						m_pComSearchRange->RemoveAt(it->first);
						// 从左侧list中移除
						m_pItemList->RemoveAt(it->first);
						// 从用户打开的历史中移除，即保证后退功能的正常使用
						CString strDevice = _T("?:\\");
						strDevice.SetAt(0, _T('A') + i); 
						std::vector<pair<CString, FILETYPE>>::iterator itVec = m_vFilePath.begin();
						while (itVec != m_vFilePath.end())
						{
							if (itVec->first.Find(strDevice) != -1)
							{
								itVec = m_vFilePath.erase(itVec);
								m_nCurFolder--;
							}
							else
								++itVec;
						}
						// 
						m_mapCombo.erase(it);
						break;
					}
				}
			}
			else if (m_eMark == BTN_MARK_DRIVES)
			{
				m_pListContent->RemoveAll();
				m_vListItem.clear();
				ShowDrivesList();
			}
		}
		dwMask  >>= 1; 
	}	
}

BOOL CDlgOpenSave::GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
{
	STORAGE_PROPERTY_QUERY Query;// input param for query  
	DWORD dwOutBytes;// IOCTL output length  
	BOOL bResult;// IOCTL return val // specify the query type  
	Query.PropertyId = StorageDeviceProperty;
	Query.QueryType = PropertyStandardQuery; // Query using IOCTL_STORAGE_QUERY_PROPERTY  
	bResult = ::DeviceIoControl(hDevice,// device handle
		IOCTL_STORAGE_QUERY_PROPERTY,// info of device property
		&Query, sizeof(STORAGE_PROPERTY_QUERY),//input data buffer
		pDevDesc, pDevDesc->Size, // output data buffer  
		&dwOutBytes, //out length  
		NULL);
	return bResult;
}

void CDlgOpenSave::SetSpecialFolder(DWORD dwType)
{
	m_pListContent->RemoveAll();
	m_vListItem.clear();
	m_strComFileName.Empty();
	switch(dwType)
	{
	case CSIDL_RECENT:
		{
			//显示最近打开的媒体记录		
			UpdateGobackBtnState();
			m_pBtnUpward->SetEnabled(false);
			m_pBtnNewForlder->SetEnabled(false);
			m_eMark = BTN_MARK_RECENT;
			OnNotifyShowRecentDoc();
		}
		break;
	case CSIDL_DESKTOPDIRECTORY:
		{
			m_eMark = BTN_MARK_DEST;
			SetDeskTopList();
			UpdateUpwardBtnState(F_DESKTOP);
		}
		break;
	case CSIDL_PERSONAL:
		{  
			//显示我的终端的媒体库
			m_nCurFolder++;
			CString strPath;
			if (m_bXPE)
			{
				strPath = _T("d:\\AVCON7_XPE\\");
			}
			else
			{
				strPath.Format(_T("%s%s"), GetExePath(), _T("MyTerminalFolder\\"));
			}
			m_vFilePath.push_back(make_pair(strPath, F_PERSONAL));

			BOOL bExist = PathFileExists(strPath);
			if (FALSE == bExist)
			{
				BOOL bCreated = ::CreateDirectory(strPath, NULL);
				if (FALSE == bCreated)
					return;
			}
			m_eMark = BTN_MARK_PERSONAL;
			m_strPersonal = strPath;
			ShowListContent(strPath);
			UpdateButtonState();
			m_nListIndex = -1;
			//UpdateUpwardBtnState(F_PERSONAL);
		}
		break;
	case CSIDL_DRIVES:
		{
			m_nCurFolder++;
			m_vFilePath.push_back(make_pair(_T(""), F_COMPUTER));
			m_eMark = BTN_MARK_DRIVES;
			ShowDrivesList();
			UpdateButtonState();
			m_pBtnNewForlder->SetEnabled(false);
		}
		return;
	default:
		{
			if (dwType >= REMOVABLE_START)
			{
				m_nCurFolder++;
				int nStart = REMOVABLE_START;
				m_strPathName = char('A' + dwType - nStart);
				m_strPathName += _T(":\\");
				m_vFilePath.push_back(make_pair(m_strPathName, F_DEVICE));
				m_eMark = BTN_MARK_REMOVABLE;
				UpdateTableState(F_DEVICE);
				if ((m_strDefExt.IsEmpty())&&(m_strFilter.IsEmpty()))
					OnOpenOK();
			}
		}
		break;
	}
}

void CDlgOpenSave::ShowDrivesList()
{
	DWORD           dwBuffer = 256;
	SHFILEINFO      fileInfo;
	CString         strFileName;
	CString         strFilePath;
	ListItem        IT;
	int				nLength = 0;

	DWORD   dwDrv = ::GetLogicalDrives(); 
	CString   strDrv = _T( "?:\\"); 
	for(int i = 0; dwDrv; ++i)
	{ 
		if(dwDrv & 1) 
		{ 
			strDrv.SetAt(0, _T('A') + i); 

			bool bRemovable = false;
			UINT nType = ::GetDriveType(strDrv);

			TCHAR lpDeviceName[1000];
			QueryDosDevice(strDrv.Left(2), lpDeviceName, 1000);
			CString strTemp = lpDeviceName;
			if (nType == DRIVE_UNKNOWN
				|| nType == DRIVE_REMOTE 
				|| nType == DRIVE_RAMDISK
				|| strTemp.Find(_T("Floppy")) != -1) // 排除一些无法使用的设备
			{
				dwDrv  >>= 1; 
				continue;
			}

			SHGetFileInfo(strDrv,FILE_ATTRIBUTE_NORMAL,&fileInfo,
				sizeof(fileInfo),SHGFI_DISPLAYNAME|SHGFI_ICON|SHGFI_USEFILEATTRIBUTES|SHGFI_LARGEICON);//SHGFI_DISPLAYNAME | SHGFI_TYPENAME | SHGFI_ICON | SHGFI_LARGEICON | SHGFI_SYSICONINDEX

			strFileName.Format(_T("%s"),fileInfo.szDisplayName);
			strFilePath.Format(_T("%s"), strDrv);
			IT.strFilePath = strFilePath;
			IT.dwFileType = F_NORMAL;
			OnNotifyShowListContent(strFileName, fileInfo, nLength, IT);
			nLength++;
		} 
		dwDrv  >>= 1; 
	}	
}

void CDlgOpenSave::OnNotifyShowListContent(CString strName, SHFILEINFO& fileInfo, int nLeg, ListItem it, bool bNeedSel /*= false*/)
{
	CString strPath;
	if (m_bXPE)
	{
		strPath = _T("d:\\AVCON7_XPE\\TempBmp");
	}
	else
	{
		strPath.Format(_T("%sskin\\TempBmp"),GetExePath());
	}
	BOOL bExist = PathFileExists(strPath);
	if (FALSE == bExist)
	{
		BOOL bCreated = ::CreateDirectory(strPath, NULL);
		if (FALSE == bCreated)
			return;
	}
	m_pListContent->AddNode(strName,  fileInfo,  nLeg, it);
	m_pListContent->SetPos(m_pListContent->GetPos());
	m_pListContent->EnsureVisible(nLeg);
	m_vListItem.push_back(it);
	if (bNeedSel)
	{
		m_pListContent->SelectItem(nLeg, true);
	}
}

void CDlgOpenSave::OnUpdateOpenSaveButState(bool bIsEmpty)
{
	m_pBtnOpen->SetEnabled(bIsEmpty);
}

void CDlgOpenSave::OnDblclkList(int iIndex)
{
	if (iIndex > -1)
	{
		OpenNewFolder(iIndex);
	}
}

void CDlgOpenSave::OpenNewFolder(int nIndex)
{
	ListItem pItem;
	if (nIndex >= (int)m_vListItem.size() || nIndex < 0)
		return;
	pItem = m_vListItem[nIndex];
	CString strTemp = m_strPathName;

	m_strPathName = pItem.strFilePath+ pItem.strFileName;// 

	//////////////////////////////////////////////////////////////////////////
	//将当前目录显示到标题栏
	CString strPath = _T("");
	strPath.Format(_T("(%s)"), PhrasesFilePath(m_strPathName));
	//////////////////////////////////////////////////////////////////////////

	m_bIsClickOK = true;

	if (pItem.dwFileType != F_NORMAL && pItem.dwFileType != F_DESKTOP)
	{
		m_vListItem.clear();
		m_pListContent->RemoveAll();
		m_nListIndex = -1;
		m_pBtnRename->SetEnabled(false);
		m_pBtnDelete->SetEnabled(false);
		m_strComFileName.Empty();
		UpdateTableState(pItem.dwFileType);
	} 
	else if(::PathIsDirectory(m_strPathName))
	{
		//需要清空控件信息
		m_vListItem.clear(); 
		m_pListContent->RemoveAll();
		m_nListIndex = -1;
		m_pBtnRename->SetEnabled(false);
		m_pBtnDelete->SetEnabled(false);
		m_strComFileName.Empty();
		if (m_nCurFolder >= (int)m_vFilePath.size() || m_nCurFolder < 0)
			return;
		FILETYPE Ft = m_vFilePath[m_nCurFolder].second;
		if (Ft != F_COMPUTER)
		{
			m_strPathName += _T("\\");
		}
		m_nCurFolder++;
		m_vFilePath.push_back(make_pair(m_strPathName, F_NORMAL));
		UpdateTableState(F_NORMAL);
		UpdateUpwardBtnState(F_NORMAL);
		if ((m_strDefExt.IsEmpty())&&(m_strFilter.IsEmpty()))
			OnOpenOK();
	}
	else if (m_strPathName.GetLength() == 3)	// 驱动器
	{
		// 空的光驱，或者读卡器等
		m_strPathName = strTemp;
		if (!m_bOpenSave)
		{
			PostMessage(WM_IS_READONLYDEVICE, 0);
			return;
		}
		PostMessage(WM_IS_ROOT, 0);
		return;
	}
	else
	{
		PostMessage(WM_SAVEFILE,1);
	}
	m_pLblPath->SetText(strPath);	// 将当目录显示到窗口上
}

void CDlgOpenSave::UpdateTableState(FILETYPE FT)
{
	switch(FT)
	{
	case F_RECENT:
		m_pComSearchRange->SelectItem(0);
		SetSpecialFolder(CSIDL_RECENT);
		break;
	case F_DESKTOP:
		SetSpecialFolder(CSIDL_DESKTOPDIRECTORY);
		break;
	case F_PERSONAL:
		m_pComSearchRange->SelectItem(1);
		SetSpecialFolder(CSIDL_PERSONAL);
		break;
	case F_COMPUTER:
		m_pComSearchRange->SelectItem(3);
		SetSpecialFolder(CSIDL_DRIVES);
		break;
	case F_DEVICE:
	case F_NORMAL:
		//m_pTabCtrl1->SetSelectedItem(-1);
		// 		m_pComSearchRange->SelectItem(3);
		//		m_nFile--;
		//		m_vFilePath.pop_back();
		if (m_nCurFolder >= (int)m_vFilePath.size() || m_nCurFolder < 0)
			return;
		ShowListContent(m_vFilePath[m_nCurFolder].first);
		break;
	}
	UpdateGobackBtnState();
	UpdateButtonState();
}

// void CDlgOpenSave::ShowListContent(CString strPath)
// {
// 	WIN32_FIND_DATA FindFileData;
// 	SHFILEINFO      fileInfo;
// 	ListItem        IT;
// 	CString         strFileName;
// 	CString         strFullPath;
// 	CString         strFilePath;
// 	int             nLength = 0;
// 	int             nFileType = m_pComFileType->GetCurSel();
// 	strFullPath += strPath;
// 	strFullPath += _T("*");
// 	FILETYPE ft = m_vFilePath[m_nCurFolder].second;
// 	if (ft == F_DESKTOP)
// 	{
// 		nLength = 3;
// 	}
// 
// 	HANDLE hListFile = ::FindFirstFile((LPTSTR)(LPCTSTR)strFullPath, &FindFileData);
// 	if (hListFile != INVALID_HANDLE_VALUE)
// 	{
// 		do
// 		{
// 			if ( lstrcmp(FindFileData.cFileName, _T("."))  ==0 || lstrcmp(FindFileData.cFileName, _T("..") ) ==0 )
// 			{
// 				continue;
// 			}
// 			strFilePath = strPath;
// 			strFileName.Format(_T("%s"), FindFileData.cFileName);
// 			strFilePath += strFileName;
// 			::SHGetFileInfo((LPTSTR)(LPCTSTR)strFilePath, FILE_ATTRIBUTE_DIRECTORY, &fileInfo, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_TYPENAME);
// 			IT.strFileName = strFileName;
// 			IT.strFilePath = strFilePath;
// 
// 			if (!IsShowFile(strFileName) && !(::PathIsDirectory(strFilePath + _T("\\"))))
// 				continue;
// 
// 			IT.dwFileType = F_NORMAL;
// 			OnNotifyShowListContent(strFileName, fileInfo, nLength, IT);
// /*			ShowFileToList*/
// 			nLength++;
// 		}while( ::FindNextFile(hListFile, &FindFileData) );
// 
// 		::FindClose(hListFile);
// 	}
// 	UpdateButtonState();
// 	UpdateGobackBtnState();
// }

//显示我的终端媒体库
void CDlgOpenSave::ShowListContent(CString strPath, int nStartIndex)
{

	//hxl 2011 10 21
	int nFileType = m_pComFileType->GetCurSel();
	CString strTypeName = _T("*.*");
	if (nFileType > 0 && nFileType < (int)m_vFileType.size())
	{
		strTypeName = m_vFileType[nFileType];
	}

	CString strFullPath = strPath + strTypeName; //chuli
	WIN32_FIND_DATA FindFileData;
	int nlength = nStartIndex;
	HANDLE hListFile = ::FindFirstFile((LPTSTR)(LPCTSTR)strFullPath, &FindFileData);
	if (hListFile != INVALID_HANDLE_VALUE)
	{
		m_pListContent->SetVisible(false);
		do
		{
			CString strFilePathName;
			strFilePathName = strPath;

			CString strFileName;
			strFileName.Format(_T("%s"), FindFileData.cFileName);
			strFilePathName += strFileName; 
			int nHide = (FindFileData.dwFileAttributes) & FILE_ATTRIBUTE_HIDDEN;//2012 9 14 hxl 过滤隐藏属性
			if (!nHide)
			{
				ShowFileToList(strPath, strFileName,nlength); //ShowFileToList接口
			}
		}while( ::FindNextFile(hListFile, &FindFileData) );

		::FindClose(hListFile);
		m_pListContent->SetVisible();
	}
	UpdateGobackBtnState();
}
BOOL CDlgOpenSave::IsShowFile(CString strName)
{
	CString strLowName = strName.Mid(strName.ReverseFind('.'));
	strLowName.MakeLower();//strLowName = 
	for (unsigned int i = 0; i<m_vShowType.size(); i++)
	{
		CString str = m_vShowType[i];
		if (str == _T("."))
			return TRUE;

		if(m_vShowType[i] == strLowName)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CDlgOpenSave::UpdateButtonState()
{
	/*if (m_nCurFolder > 0)
	{
		m_pBtnGoBack->SetEnabled(TRUE);
	}
	else
	{
		m_pBtnGoBack->SetEnabled(FALSE);
	}*/
	if (m_nCurFolder >= (int)m_vFilePath.size() || m_nCurFolder < 0)
		return;
	FILETYPE ft = m_vFilePath[m_nCurFolder].second;
	UpdateUpwardBtnState(ft);

	m_pComFileName->RemoveAll();
	m_pComFileName->SetText(_T(""));
	if (m_bOpenSave != FALSE)
		m_pEditFileName->SetText(_T(""));
	if(!m_bOpenSave && !m_pEditFileName->GetText().IsEmpty())
	{
		m_pBtnOpen->SetEnabled(true);
	}
	else
	{
		m_pBtnOpen->SetEnabled(false );
	}
}

void CDlgOpenSave::SetCurFileTypes()
{
	int  nFileType = m_pComFileType->GetCurSel();
	if (nFileType < 0)
		return;
	CString strTypeName = m_vFileType[nFileType];
	if (_tcsicmp(strTypeName, _T("*.*")) == 0)
	{
		m_pComFileType->SetToolTip(m_pComFileType->GetText().TrimLeft());
	}
	else
		m_pComFileType->SetToolTip(strTypeName);
	CString str;
	if (strTypeName.Find(';')<0)
	{
		str = strTypeName.Mid(strTypeName.ReverseFind('.'));
		if (str.Find('*') > -1)
		{
			m_vShowType.push_back(_T("."));
			return;
		}
		else
		{
			m_vShowType.push_back(str);
			return;
		}
	}
	do
	{
		str = strTypeName.Mid(strTypeName.ReverseFind('*')+1);
		m_vShowType.push_back(str);
		strTypeName = strTypeName.Left(strTypeName.ReverseFind('*')+1);
		str = strTypeName.Mid(strTypeName.ReverseFind(';'));
		strTypeName = strTypeName.Left(strTypeName.ReverseFind(';'));
	}while (str.ReverseFind(';') > -1);	
}

void CDlgOpenSave::SetDeskTopList()
{
	LPITEMIDLIST   lpItemIDList;
	SHFILEINFO     shinfo;
	ListItem       LI;
	LPWSTR szPath = new TCHAR[MAX_PATH];
	DWORD dwType[4] = {CSIDL_RECENT, CSIDL_PERSONAL, CSIDL_DRIVES, CSIDL_DESKTOPDIRECTORY};
	FILETYPE FT[4] = {F_RECENT, F_PERSONAL, F_COMPUTER, F_DESKTOP};
	CString strDisplayName,strPath;
	int nStartIndex = 0;
	if (!m_bOpenSave)
	{
		nStartIndex = 1;
	}
	for (int i=nStartIndex; i < 4; i++)
	{
		::SHGetSpecialFolderLocation(m_hWnd, dwType[i], &lpItemIDList);
		::SHGetPathFromIDList(lpItemIDList,szPath);
		strPath.Format(_T("%s"), szPath);

		if (i == 0)
			m_strRecent = m_strDesktop = strPath + _T("\\");

		if ( i == 3 )
		{
			m_strDesktop = strPath + _T("\\");
			m_nCurFolder = 0;
			m_vFilePath.clear();
			//m_nCurFolder++;
			m_vFilePath.push_back(make_pair(m_strDesktop, F_DESKTOP));
			ShowListContent(m_strDesktop,i - nStartIndex);
			delete[] szPath;
			return;
		}

		// 获取图标
		::SHGetFileInfo((LPCTSTR)lpItemIDList,NULL, &shinfo, sizeof(shinfo), 
			SHGFI_SYSICONINDEX | SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_PIDL);
		strDisplayName.Format(_T("%s"),shinfo.szDisplayName);

		if ( i== 2 )
		{
			//strDisplayName = LoadLangStr(_T("FILESHARE"),_T("MY_COMPUTER"), strDisplayName);
			strDisplayName = GetPluginsLangString( _T("MY_COMPUTER"), _T("我的电脑"));
		}
		LI.strFileName = strDisplayName;
		LI.strFilePath = strPath;
		LI.dwFileType = FT[i];
		OnNotifyShowListContent(strDisplayName, shinfo, i - nStartIndex, LI);
	}
	delete[] szPath;
}


void CDlgOpenSave::OnOpenSaveButton(OPENSAVEBTN OSBtn)
{
	OnNotifyOpenSaveButton(OSBtn);
	if (OSBtn == OSB_BTN_GOBACK)
	{
		// 		if (m_nCurFolder>0)
		// 		{
		// 			m_vFilePath.pop_back();
		// 			m_nCurFolder--;
		// 			FILETYPE ft = m_vFilePath[m_nCurFolder].second;
		// 			if (ft != F_NORMAL)
		// 			{
		// 				UpdateTableState(ft);
		// 				m_vFilePath.pop_back();
		// 				m_nCurFolder--;
		// 				if (ft == F_RECENT)
		// 				{
		// 					OnNotifyShowRecentDoc();
		// 				}
		// 				UpdateGobackBtnState();
		// 			}
		// 			else
		// 			{
		// 				ShowListContent(m_vFilePath[m_nCurFolder].first);
		// 			}
		// 		}

	}
	else if (OSBtn == OSB_BTN_UPWARD)
	{
		if (m_nCurFolder>0)
		{
			m_vFilePath.pop_back();
			m_nCurFolder--;
			if (m_nCurFolder >= (int)m_vFilePath.size() || m_nCurFolder < 0)
				return;
			FILETYPE FT = m_vFilePath[m_nCurFolder].second;
			switch(FT)
			{
			case F_RECENT:
				{
					OnNotifyShowRecentDoc();
					m_strCurPath = m_strRecent;
				}
				break;
			case F_DESKTOP:
				{
					SetDeskTopList();
					m_strCurPath = m_strDesktop;
				}
				break;
			case F_COMPUTER:
				{
					ShowDrivesList();
					m_strCurPath = GetPluginsLangString( _T("MY_COMPUTER"), _T("我的电脑"));
				}
				break;
			case F_PERSONAL:
				{
					UpdateUpwardBtnState(F_PERSONAL);
					ShowListContent(m_strPersonal, NULL);	// 初始化列表中内容;
					m_strCurPath = m_strPersonal;
				}
				break;
			default:
				{
					m_strCurPath = m_vFilePath[m_nCurFolder].first;
					ShowListContent(m_vFilePath[m_nCurFolder].first);
				}
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//将当前目录显示到标题栏
			CString strPath = _T("");
			strPath.Format(_T("(%s)"), PhrasesFilePath(m_strCurPath));
			m_pLblPath->SetText(strPath);	// 将当目录显示到窗口上
			m_pLblPath->SetToolTip(m_strCurPath);
			//////////////////////////////////////////////////////////////////////////

			UpdateUpwardBtnState(FT);		
		}
		// 		FILETYPE ft = m_vFilePath[m_nCurFolder].second;
		// 		CString strCurPath = m_vFilePath[m_nCurFolder].first;
		// 		if (ft == F_DESKTOP)
		// 		{
		// 			SetSpecialFolder(CSIDL_DESKTOP);
		// 			m_vFilePath.pop_back();
		// 			m_nCurFolder--;
		// 			UpdateButtonState();
		// 		}
		// 		else if (ft == F_COMPUTER || ft == F_PERSONAL || ft == F_RECENT)
		// 		{
		// 			SetSpecialFolder(CSIDL_DESKTOP);
		// 		}
		// 		else
		// 		{
		// 			strCurPath = strCurPath.Left(strCurPath.ReverseFind(_T('\\')));
		// 			strCurPath = strCurPath.Left(strCurPath.ReverseFind(_T('\\')) +1);
		// 			if (strCurPath.IsEmpty())
		// 			{
		// 				SetSpecialFolder(CSIDL_DRIVES);
		// 			}
		// 			else if (strCurPath == m_strDesktop)
		// 			{
		// 				SetSpecialFolder(CSIDL_DESKTOP);
		// 			}
		// 			else
		// 			{
		// 				m_nCurFolder++;
		// 				m_vFilePath.push_back(make_pair(strCurPath, F_NORMAL));
		// 				ShowListContent(strCurPath);
		// 			}
		// 		}
	}
	else if (OSBtn == OSB_BTN_NEWFOLDER)
	{
		if (m_nCurFolder >= (int)m_vFilePath.size() || m_nCurFolder < 0)
			return;
		if (m_vFilePath[m_nCurFolder].second != F_COMPUTER)
		{
			if (m_nCurFolder != -1)
			{
				OnNotifyCreateNewFolder(m_vFilePath[m_nCurFolder].first);
			}
		}
	}
	else if (OSBtn == OSB_BTN_OPEN && !m_bOpenSave)
	{
		// 		m_strPathName = m_vFilePath[m_nCurFolder].first;
		// 		m_strPathName += m_pComFileName->GetCurrText();
		// 		m_strPathName += _T(".");
		// 		m_strPathName += m_strDefExt;
	}
	else if (OSBtn == OSB_BTN_RENAME)
	{
		EditFileName(m_nListIndex);
	}
	else if (OSBtn == OSB_BTN_DELETE)
	{
		DeleteByIndex(m_nListIndex);
	}
}

void CDlgOpenSave::OnNotifyOpenSaveButton(OPENSAVEBTN osb)
{
	int nCurSel;
	ListItem pItem;
	switch(osb)
	{
	case OSB_BTN_CANCEL:
	case OSB_BTN_CLOSE:
		OnOpenCancel();
		break;
	case OSB_BTN_OPEN:
		if (m_pListContent->GetCurSel() > -1)
		{
			nCurSel = m_pListContent->GetCurSel();
			if ( nCurSel > -1)
			{
				OpenNewFolder(nCurSel);
			}
		}
		else if(!m_bOpenSave) //保存时未选中项
		{
			PostMessage(WM_SAVEFILE,0);
		}
		break;
	case OSB_BTN_GOBACK:
		m_pListContent->RemoveAll();
		m_nListIndex = -1;
		m_pBtnRename->SetEnabled(false);
		m_pBtnDelete->SetEnabled(false);
		m_vListItem.clear();
		m_strComFileName.Empty();
		break;
	case OSB_BTN_UPWARD:
		m_pListContent->RemoveAll();
		m_nListIndex = -1;
		m_pBtnRename->SetEnabled(false);
		m_pBtnDelete->SetEnabled(false);
		m_vListItem.clear();
		m_strComFileName.Empty();
		break;
	case OSB_BTN_NEWFOLDER:

		break;
	case OSB_BTN_PREVIEWS:
		break;
	default:
		return;
	}
}

void CDlgOpenSave::OnOpenOK()
{
	//打开媒体记录时间
	SYSTEMTIME currentTime;
	::GetLocalTime(&currentTime);
	CString time=_T("");
	time.Format(_T("%4d年%2d月%2d日 %2d:%2d:%2d"),currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wMilliseconds);
	OPENFILE_RECORD openFileRecord;
	openFileRecord.strDateTime = time;
	//打开媒体记录路径
	openFileRecord.strFilePath = GetFilePath();
	openFileRecord.strFileName = GetFileName();
	if (m_bOpenSave)
	{
		g_strFullOutPath = GetFullPath();
		//判断该文件是否存在
		BOOL bExist = PathFileExists(g_strFullOutPath);
		if(!bExist)
		{
			CString strContent = GetPluginsLangString( _T("FILEEROR_FILEOPEN_FAILE"), _T("当前打开的文件或文件夹不存在。"));
			IPlugin_U7T_TipWindow* pPluginU7Tip = dynamic_cast<IPlugin_U7T_TipWindow*>(g_System->GetPluginByName("Plugin_U7T_TipWindow"));
			if (pPluginU7Tip != NULL)
			{
				pPluginU7Tip->ShowTipWindow(strContent);
			}
			Close(IDCANCEL);
			return;
		}
	}
	else
	{
		g_strFullOutPath = m_strSourceName;
	}
	//将开打媒体的时间和路径插入表中
	m_dbOpenRecord.AddOpenRecord(openFileRecord);
	if ((int)m_vFilePath.size() > m_nCurFolder && m_nCurFolder > 0)
	{
		//初始化时显示我的终端的媒体库
		if (m_nCurFolder >= (int)m_vFilePath.size() || m_nCurFolder < 0)
			return;
		CString str;
		str.Format(_T("%d"),m_vFilePath[m_nCurFolder].second);
		SetCfgStr(_T("LocalPath"),_T("SaveFolderType"),str);
		int nCurSel = m_pComSearchRange->GetCurSel();
		if (!m_bOpenSave)
		{
			++nCurSel;
		}
		SetCfgInt(_T("LocalPath"),_T("SaveFileType"),nCurSel/*m_pComSearchRange->GetCurSel()*/);
		SetCfgStr(_T("LocalPath"),_T("SaveNearest"),GetFilePath());
	}
	else if ((int)m_vFilePath.size() > m_nCurFolder && m_nCurFolder >= 0)
	{
		if (m_nCurFolder >= (int)m_vFilePath.size() || m_nCurFolder < 0)
			return;
		CString str;
		str.Format(_T("%d"),m_vFilePath[m_nCurFolder].second);
		int nCurSel = m_pComSearchRange->GetCurSel();
		if (!m_bOpenSave)
		{
			++nCurSel;
		}
		SetCfgInt(_T("LocalPath"),_T("SaveFileType"),nCurSel/*m_pComSearchRange->GetCurSel()*/);
		SetCfgStr(_T("LocalPath"),_T("SaveNearest"),GetFilePath());
	}
	Close(IDOK);
}

void CDlgOpenSave::OnOpenCancel()
{
	Close(IDCANCEL);
}

void CDlgOpenSave::OnNotifyCreateNewFolder(CString strFullPath)
{
	CString strFileName = _T("新建文件夹"); 
	if (GetPluginU7Lang())
	{
		strFileName = GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILE_NEWFOLDER"), _T("新建文件夹"));
	}
	int nCount = m_pListContent->GetCount();
	CString str, strName;
	strName = strFileName;
	int k = 1;
	while (!(::CreateDirectory((strFullPath+strName), NULL)))
	{
		k++;
		str.Format(_T(" (%d)"), k);
		strName = strFileName;
		strName += str;
	}
	CString strTmp = strFullPath + strName;
	SHFILEINFO fileInfo;
	::SHGetFileInfo((LPTSTR)(LPCTSTR)strTmp, FILE_ATTRIBUTE_DIRECTORY, &fileInfo, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_TYPENAME);

	ListItem IT;
	IT.strFileName = strName;
	IT.strFilePath = strFullPath;
	IT.dwFileType = F_NORMAL;
	OnNotifyShowListContent(IT.strFileName, fileInfo, nCount, IT, true);
}

CString CDlgOpenSave::GetFullPath()
{
	return m_strPathName;
}

CString CDlgOpenSave::GetFilePath()
{
	CString strResult = GetFullPath();
	strResult = strResult.Left(strResult.ReverseFind('\\') +1 );
	return strResult;
}

CString CDlgOpenSave::GetFileName()
{
	CString strResult = GetFullPath();
	strResult = strResult.Mid(strResult.ReverseFind('\\') +1 );
	return strResult;
}

CString CDlgOpenSave::GetFileExt()
{
	CString strResult = GetFullPath();
	LPTSTR pszExtension = ::PathFindExtension(strResult);
	if (pszExtension != NULL)
		return pszExtension+1;
	strResult.Empty();
	return strResult;
}

CString CDlgOpenSave::GetFileTitle()
{
	CString strResult = GetFileName();
	return strResult;
}

void CDlgOpenSave::OnComboxSelChanged(OPENSAVECOMBOX OSCom)
{
	OnNotifyComboxSelChanged(OSCom);
	switch(OSCom)
	{
	case OSB_COM_SEARCHRANGE:
		{
			int nIndex = m_pComSearchRange->GetCurSel();
			MAP_COMBO_IT it = m_mapCombo.find(nIndex);
			if (it != m_mapCombo.end())
			{
				SetSpecialFolder(it->second);
			}
			m_pBtnRename->SetEnabled(false);
			m_pBtnDelete->SetEnabled(false);
		}
		break;
	case OSB_COM_FILETYPE:
		m_vShowType.clear();
		SetCurFileTypes();
		if (m_vFilePath.empty())
			return;
		if (m_nCurFolder >= (int)m_vFilePath.size() || m_nCurFolder < 0)
			return;
		if (m_vFilePath[m_nCurFolder].second != F_NORMAL)
		{
			UpdateTableState(m_vFilePath[m_nCurFolder].second);
			m_vFilePath.pop_back();
			m_nCurFolder--;
		} 
		else
		{
			ShowListContent(m_vFilePath[m_nCurFolder].first);
		}
		break;
	}
}

void CDlgOpenSave::OnNotifyComboxSelChanged(OPENSAVECOMBOX osc)
{
	switch(osc)
	{
	case OSB_COM_SEARCHRANGE:
		break;
	case OSB_COM_FILENAME:
		break;
	case OSB_COM_FILETYPE:
		m_pListContent->RemoveAll();
		m_vListItem.clear();
		m_strComFileName.Empty();
		break;
	}	
}

void CDlgOpenSave::UpdateGobackBtnState()
{
	m_pBtnNewForlder->SetEnabled(TRUE);
	m_pComFileName->RemoveAll();
	m_pComFileName->SetText(_T(""));
	if(!m_bOpenSave && !m_pEditFileName->GetText().IsEmpty())
	{
		m_pBtnOpen->SetEnabled(true);
	}
	else
	{
		m_pBtnOpen->SetEnabled(false );
	}
}

//显示打开的媒体记录
void CDlgOpenSave::ShowOpenFileHistory()
{
	m_mapOpenFileRecord.clear();
	m_dbOpenRecord.ReadOpenFileRecord(m_mapOpenFileRecord);
	m_pListContent->RemoveAll();
	m_vListItem.clear();
	m_vFileListItem.clear();
	ListItem pItem;
	int nlength = 0;
	MAP_OPENFILE_RECORD_IT it = m_mapOpenFileRecord.begin();
	for (; it != m_mapOpenFileRecord.end(); ++it)
	{
		OPENFILE_RECORD &openFileRecord = it->second;
		GetFileList(openFileRecord.strFilePath); 

		for (int nIndex = 0;nIndex!=m_vFileListItem.size();nIndex++)
		{
			if (nIndex >= (int)m_vFileListItem.size() || nIndex < 0)
				return;
			pItem = m_vFileListItem[nIndex];
			if(pItem.strFileName == openFileRecord.strFileName) //判断历史记录中的数据是否依然存在
			{
				ShowFileToList(openFileRecord.strFilePath, openFileRecord.strFileName,nlength);
			}	
		}
		m_vFileListItem.clear();
	}
}

void CDlgOpenSave::GetFileList(CString strPath)
{
	int nFileType = m_pComFileType->GetCurSel();
	CString strTypeName = _T("*.*");
	if (nFileType > 0 && nFileType < (int)m_vFileType.size())
	{
		strTypeName = m_vFileType[nFileType];
	}

	CString strFullPath = strPath + strTypeName; 
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile = ::FindFirstFile((LPTSTR)(LPCTSTR)strFullPath, &FindFileData);
	if (hListFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString strFilePathName;
			strFilePathName = strPath;
			CString strFileName;
			strFileName.Format(_T("%s"), FindFileData.cFileName);
			strFilePathName += strFileName;    
			FindFileData.cAlternateFileName;
			int nHide = (FindFileData.dwFileAttributes) & FILE_ATTRIBUTE_HIDDEN;//2012 9 14 hxl 过滤隐藏属性
			if (!nHide)
			{
				GetFileToList(strPath, strFileName); //ShowFileToList接口
			}
		}while( ::FindNextFile(hListFile, &FindFileData) );

		::FindClose(hListFile);
	}
}

//获得所有文件路经
void CDlgOpenSave::GetFileToList(CString strFilePath, CString strFileName)
{
	if ( strFileName.Compare(_T("."))  ==0 || strFileName.Compare(_T("..") ) ==0 )
		return;
	CString strPathName = strFilePath + strFileName;
	if (IsShowFile(strFileName)||::PathIsDirectory(strPathName + _T("\\")))
	{    
		ListItem itList;
		itList.strFileName	= strFileName;
		itList.strFilePath	= strFilePath;
		itList.dwFileType	= F_NORMAL;
		OnNotifySetFileList( itList);
	}
}

//根据路径将打开的媒体信息显示在对话框列表中
void CDlgOpenSave::ShowFileToList(CString strFilePath, CString strFileName, int &length)
{
	if ( strFileName.Compare(_T("."))  ==0 || strFileName.Compare(_T("..") ) ==0 )
		return;
	CString strPathName = strFilePath + strFileName;
	SHFILEINFO fileInfo;
	if (IsShowFile(strPathName))
	{
		::SHGetFileInfo(strPathName,FILE_ATTRIBUTE_HIDDEN, &fileInfo, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_TYPENAME);
		ListItem itList;
		itList.strFileName	= strFileName;
		itList.strFilePath	= strFilePath;
		itList.dwFileType	= F_NORMAL;
		OnNotifyShowListContent(strFileName, fileInfo,length, itList);
		length++;
	} 
	else if(::PathIsDirectory(strPathName + _T("\\")))
	{
		::SHGetFileInfo(strPathName,FILE_ATTRIBUTE_HIDDEN, &fileInfo, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_TYPENAME);
		ListItem itList;
		itList.strFileName	= strFileName;   //文件名
		itList.strFilePath	= strFilePath;
		itList.dwFileType	= F_NORMAL;
		OnNotifyShowListContent(strFileName, fileInfo, length, itList);
		length++;
	}
}


void CDlgOpenSave::OnNotifySetFileList( ListItem it) 
{
	m_vFileListItem.push_back(it);
}

void CDlgOpenSave::OnNotifyShowRecentDoc(void)
{
	ShowOpenFileHistory();
}

void CDlgOpenSave::UpdateUpwardBtnState(FILETYPE FT)
{
	if (FT != F_NORMAL)
	{
		m_pBtnUpward->SetEnabled(FALSE);
	}
	else
	{
		m_pBtnUpward->SetEnabled(TRUE);
	}
	if (FT == F_COMPUTER || FT == F_RECENT )
	{
		m_pBtnNewForlder->SetEnabled(FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////////////
void CDlgOpenSave::ReleaseAllVector()
{
	m_vFilePath.clear();
	m_vFileType.clear();
	m_vListItem.clear();
	m_vFileListItem.clear();
	m_vShowType.clear();
}

CString CDlgOpenSave::GetFullFilePath()
{
	return g_strFullOutPath;
}

void CDlgOpenSave::OnListItemchanged(int nIndex)
{
	m_pBtnRename->SetEnabled(false);
	m_pBtnDelete->SetEnabled(false);
	ListItem pItem;
	if (nIndex >= (int)m_vListItem.size() || nIndex < 0)
		return;

	pItem = m_vListItem[nIndex];
	m_pComFileName->RemoveAll();
	CString strFileNameTemp;
	strFileNameTemp.Format(_T("%s"),pItem.strFileName);
	if(m_bOpenSave)
	{
		m_pComFileName->Add(strFileNameTemp);
		m_pComFileName->SelectItem(0);
	}
	CString strFileFullPath = pItem.strFilePath+pItem.strFileName;
	if (!m_bOpenSave && !::PathIsDirectory(strFileFullPath) && pItem.dwFileType == F_NORMAL)
	{
		UINT uTyep = GetDriveType(pItem.strFilePath);
		if (uTyep != DRIVE_CDROM)
		{
			m_pEditFileName->SetText(strFileNameTemp);
		}
	}
	m_pBtnOpen->SetEnabled(TRUE);
	m_strComFileName = pItem.strFileName;
	if (::PathFileExists(strFileFullPath) && strFileFullPath.GetLength() > 3)	//路径存在，且排除驱动器
	{
		m_pBtnRename->SetEnabled(true);
		m_pBtnDelete->SetEnabled(true);
	}
}

void CDlgOpenSave::OnBeginEditFileName(int nIndex)
{
	CListLabelElementUI *pElement = (CListLabelElementUI*)m_pListContent->GetItemAt(nIndex);
	if (NULL == pElement)
		return;

	CTextUI* pLblFileName = static_cast<CTextUI*>(paint_manager_.FindSubControlByName(pElement, _T("LblFileName")));
	if (NULL == pLblFileName)
		return;

	RECT pRect = pLblFileName->GetPos();
	pRect.left = pRect.left-3;
	pRect.bottom = pRect.bottom+3;
	pRect.top = pRect.top-3;
	if (nIndex >= (int)m_vListItem.size() || nIndex < 0)
		return;
	ListItem& it = m_vListItem[nIndex];
	m_nIndex = nIndex;
}

void CDlgOpenSave::OnEndEditFileName()
{
	if (m_nIndex >= (int)m_vListItem.size() || m_nIndex < 0)
		return;

	ListItem& it = m_vListItem[m_nIndex];
	//CString strDisplay = m_pEditAndRenameFile->GetText();
	/*CString strOldName = it.strFileName;
	rename(CW2A(it.strFilePath+strOldName), CW2A(it.strFilePath+strDisplay));
	if (strDisplay == _T(""))
	{
		it.strFileName =strOldName;
	}
	else
	{
		it.strFileName = strDisplay;
	}*/

	CListLabelElementUI *pElement = (CListLabelElementUI*)m_pListContent->GetItemAt(m_nIndex);
	if (pElement == NULL)
		return;

	CTextUI* pLblFileName = static_cast<CTextUI*>(paint_manager_.FindSubControlByName(pElement, _T("LblFileName")));
	if (pLblFileName == NULL)
		return;
	pLblFileName->SetText(it.strFileName);
	m_nListIndex = -1;
}

void CDlgOpenSave::EditFileName(int nIndex)
{
	if (-1 == nIndex)
	{
		return;
	}
	CListLabelElementUI *pElement = (CListLabelElementUI*)m_pListContent->GetItemAt(nIndex);
	if (NULL == pElement)
		return;

	CLabelUI* pLbFileName = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pElement, _T("LbFileName")));
	if (NULL == pLbFileName)
		return;

	CEditUI* pEditFileName = static_cast<CEditUI*>(paint_manager_.FindSubControlByName(pElement, _T("EditFileName")));
	if (NULL == pEditFileName)
		return;
	if (nIndex >= (int)m_vListItem.size() || nIndex < 0)
		return;
	ListItem pItem = m_vListItem[nIndex];
	CString strPath = pItem.strFilePath + pItem.strFileName;
	if (::PathFileExists(strPath))
	{
		RECT rcListLabel = pElement->GetPos();
		RECT rcList = m_pListContent->GetPos();
		RECT rcEdit = pLbFileName->GetPos();

		//先将ITEM移动到m_pListContent区域外再编辑，EDIT会出现在m_pListContent外
		if (rcListLabel.top >= rcList.top && rcListLabel.bottom <= rcList.bottom)
		{
			pEditFileName->SetPos(rcEdit);
			pEditFileName->SetText(pItem.strFileName);
			pEditFileName->SetVisible(true);
			pEditFileName->SetFocus();
			pEditFileName->SetSelAll();
			m_nEditIndex = nIndex;
		}
	}
}

void CDlgOpenSave::EditFileNameEnd()
{
	if (-1 == m_nEditIndex)
	{
		return;
	}
	int nIndex = m_nEditIndex;
	m_nEditIndex = -1;
	CListLabelElementUI *pElement = (CListLabelElementUI*)m_pListContent->GetItemAt(nIndex);
	if (NULL == pElement)
		return;

	CLabelUI* pLbFileName = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pElement, _T("LbFileName")));
	if (NULL == pLbFileName)
		return;

	CTextUI* pEditFileName = static_cast<CTextUI*>(paint_manager_.FindSubControlByName(pElement, _T("EditFileName")));
	if (NULL == pEditFileName)
		return;
	if (nIndex >= (int)m_vListItem.size() || nIndex < 0)
		return;
	ListItem pItem = m_vListItem[nIndex];
	pEditFileName->SetVisible(false);
	m_pListContent->SetFocus();
	CString strPath = pItem.strFilePath;
	CString strOldFileName = pLbFileName->GetText();
	CString strNewFileName = pEditFileName->GetText().TrimLeft();
	if (strOldFileName.Compare(strNewFileName) != 0)
	{
		if (!CheckFileName(strNewFileName, strPath+strOldFileName));

		else if (MoveFile(strPath + strOldFileName, strPath + strNewFileName))
		{
			pLbFileName->SetText(strNewFileName);
			ListItem pItem = m_vListItem[nIndex];
			pItem.strFileName = strNewFileName;
			m_vListItem[nIndex] = pItem;
			pLbFileName->SetToolTip(strNewFileName);
		}
		else if((strPath+strNewFileName).GetLength()>=MAX_PATH)
		{
			PostMessage(WM_TO_LONG);
		}
		else
		{
			HANDLE handle = CreateFile(strPath+strNewFileName,GENERIC_READ,FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if(handle!=INVALID_HANDLE_VALUE)
			{
				CloseHandle(handle);
				CString cstrLang = GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILEERROR_FILERENAME_EXISTED"), _T("该文件名已存在，文件重命名失败"));
				IPlugin_U7T_TipWindow* pTipWindow = dynamic_cast<IPlugin_U7T_TipWindow*>(g_System->GetPluginByName(PLUGIN_NAME_U7T_TIPWINDOW));
				pTipWindow->ShowTipWindow(cstrLang,3,this->m_hWnd);
			}
			else
			{
				CString cstrLang = GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILEERROR_FILERENAME_FAILED"), _T("文件重命名失败，正在被其他程序使用"));
				IPlugin_U7T_TipWindow* pTipWindow = dynamic_cast<IPlugin_U7T_TipWindow*>(g_System->GetPluginByName(PLUGIN_NAME_U7T_TIPWINDOW));
				pTipWindow->ShowTipWindow(cstrLang,3,this->m_hWnd);
			}
		}
		OnListItemchanged(nIndex);
	}
}

void CDlgOpenSave::DeleteByIndex(int nIndex)
{
	if (-1 == nIndex)
	{
		return;
	}
	CListLabelElementUI *pElement = (CListLabelElementUI*)m_pListContent->GetItemAt(nIndex);
	if (NULL == pElement)
		return;

	CLabelUI* pLbFileName = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pElement, _T("LbFileName")));
	if (NULL == pLbFileName)
		return;

	CTextUI* pEditFileName = static_cast<CTextUI*>(paint_manager_.FindSubControlByName(pElement, _T("EditFileName")));
	if (NULL == pEditFileName)
		return;

	if (nIndex >= (int)m_vListItem.size() || nIndex < 0)
		return;
	ListItem pItem = m_vListItem[nIndex];
	CString strFileName = pItem.strFileName;
	CString strPath = pItem.strFilePath + pItem.strFileName;
	if (::PathFileExists(strPath))
	{
		DeleteDirectory(strPath, true);
		DeleteFile(strPath);
		if(!::PathFileExists(strPath))
		{
			vector <ListItem>::iterator it = m_vListItem.begin();
			int nCurIndex = 0;
			for (; it!=m_vListItem.end(); ++it)
			{
				if (nCurIndex == nIndex)
				{
					m_vListItem.erase(it);
					break;
				}
				++nCurIndex;
			}
			m_pListContent->SelectItem(-1);
			m_pListContent->Remove(pElement);
		}
		else
		{
			CString cstrLang = GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILEEROR_FILEOCCUPY_FAILE"), _T("文件已被占用!"));
			IPlugin_U7T_TipWindow* pTipWindow = dynamic_cast<IPlugin_U7T_TipWindow*>(g_System->GetPluginByName(PLUGIN_NAME_U7T_TIPWINDOW));
			pTipWindow->ShowTipWindow(cstrLang,3,this->m_hWnd);
		}
	
	}
}

void CDlgOpenSave::SetUISize( float fScale )
{
	RECT rc;
	::GetWindowRect(m_hWnd, &rc);
	int nHCenter = (rc.left + rc.right) / 2;
	int nVCenter = (rc.top + rc.bottom) / 2;

	int nWidth = (int)(m_initSize.cx * fScale);
	int nHeight = (int)(m_initSize.cy * fScale);

	rc.top = nVCenter - nHeight / 2;
	rc.left = nHCenter - nWidth / 2;

	rc.top = (rc.top<0) ? 0 : rc.top;
	//rc.left = (rc.left<0) ? 0 : rc.left;

	::MoveWindow(m_hWnd, rc.left, rc.top, nWidth, nHeight, TRUE);

	paint_manager_.NeedUpdate();
}

void CDlgOpenSave::SetTransparent( int nOpacity )
{
	paint_manager_.SetTransparent(nOpacity);
}

void CDlgOpenSave::SetButtonSize( float fScale )
{
}

void CDlgOpenSave::SetFontSize( int nSize )
{
	TFontInfo* pFontInfo = paint_manager_.GetDefaultFontInfo();
	if( pFontInfo == NULL)
		return;

	pFontInfo->iSize = nSize;

	paint_manager_.SetDefaultFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, 
		pFontInfo->bUnderline, pFontInfo->bItalic);

	GetRoot()->NeedUpdate();
}

void CDlgOpenSave::SetFont( const TFontInfo* pFontInfo )
{
	paint_manager_.SetDefaultFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, 
		pFontInfo->bUnderline, pFontInfo->bItalic);
}


CControlUI* CDlgOpenSave::CreateBtnItem( CString strPic,CString strBtnName )
{
	strPic = GetFolderPath() + strPic;
	CListContainerElementUI* pListElement = NULL;
	paint_manager_.SetResourcePath(GetFolderPath());
	if( !m_dlgBuilder.GetMarkup()->IsValid() )
	{
		//此处paint_manager_.GetInitSize()的值改变了，其他的东西可能也随之改变。
		pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("BtnItem.xml"), (UINT)0, NULL, &paint_manager_));
	}
	else
	{
		pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create((UINT)0, &paint_manager_));
	}

	if (NULL == pListElement)
		return NULL;

	CLabelUI* pPic = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, _T("lbPic")));
	pPic->SetBkImage(strPic);
	CLabelUI* pText = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, _T("lbText")));
	pText->SetText(strBtnName);
	CControlUI* pCtrl = paint_manager_.FindSubControlByName(pListElement,_T("HLayoutItem"));
	if (pCtrl)
	{
		pCtrl->SetToolTip(strBtnName);
	}
	m_pItemList->Add(pListElement);
	return pListElement;
}

bool CDlgOpenSave::SaveFilePath( void )
{
	m_strComFileName = m_pEditFileName->GetText();
	if (m_strComFileName.GetLength()>254)
	{
		PostMessage(WM_TO_LONG);
		return false;
	}
	else if (m_strComFileName.IsEmpty() || m_strComFileName == m_strComFileName.SpanIncluding(_T(" "))/*输入的全为空格*/)
	{
		PostMessage(WM_IS_NULL);
		return false;
	}
	CString strIllegal = _T("*:|\"\\<>?/");
	for (int i=0; i<strIllegal.GetLength();i++)
	{
		char strA = (char)strIllegal.GetAt(i);
		int nPos = m_strComFileName.Find(strA);
		if (nPos != -1)
		{
			PostMessage(WM_IS_VALID);
			return false;
		}
	}
	//给文件名添加后缀
	if (!m_vFileType.empty()) 
	{
		int nFileType = m_pComFileType->GetCurSel();
		CString strFileType = m_vFileType[nFileType];
		strFileType.Delete(0,strFileType.Find('*')+1);
		int nPos = m_strComFileName.Find(strFileType);
		if (nPos < 0)
		{
			m_strComFileName += strFileType;
		}
	}
	//判断该文件是否存在
	CString strNeWFile = _T("");
	if (m_nCurFolder < (int)m_vFilePath.size() && m_nCurFolder >= 0)
	{
		strNeWFile = m_vFilePath[m_nCurFolder].first;
		strNeWFile += m_strComFileName;
	}
	BOOL bExist = ::PathFileExists(strNeWFile);
	if (bExist)
	{
		CString strLang = _T("新建文件夹");
		CString strMessageTitle = _T("提示");
		if (GetPluginU7Lang())
		{
			strLang = GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILEEROR_FILEEXIST_ASK"), _T("文件存在，要替换它吗？"));
			strMessageTitle = GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("MESSAGE_TITLE_PROMPT"), _T("提示"));

		}
		CString strConment = _T("");
		strConment.Format(_T("%s%s"), m_strComFileName, strLang);
		IPlugin *pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox");
		IPlugin_U7T_MessageBox *pMessageBox = dynamic_cast<IPlugin_U7T_MessageBox*>(pIPlugin);
		if (pMessageBox != NULL)
		{
			if (pMessageBox->MessageBox(GetHWND(), strMessageTitle, strConment,MB_OKCANCEL/*,true*/)!=IDOK)
			{
				return false;
			}
			//判断文件是否被其他进程占用
			if(PathFileExists(strNeWFile) && !::DeleteFile(strNeWFile))
			{
				IPlugin_U7T_TipWindow* pMsgBox = dynamic_cast<IPlugin_U7T_TipWindow*>(g_System->GetPluginByName("Plugin_U7T_TipWindow"));
				if (pMsgBox)
				{
					CString strLang= _T("文件已被占用!"); 
					if (GetPluginU7Lang())
					{
						strLang	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("FILEEROR_FILEOCCUPY_FAILE"), _T("文件已被占用!"));
					}
					pMsgBox->ShowTipWindow(strLang, 3, GetHWND());
				}
				return false;
			}
		}
	}

	//OpenSave不做文件拷贝的工作，只返回文件路径
	//BOOL bSuc = CopyFile(m_strSourceName,strNeWFile,FALSE);

// 	if (m_strSourceName.IsEmpty())
// 	{
	m_strSourceName = strNeWFile;
//	}

	return true;
}

void CDlgOpenSave::OpenLastDir()
{
 	SetDeskTopList();
 	SetSpecialFolder(CSIDL_PERSONAL);

	// 读取上次打开的文件类型，并设置为本次的初始值
	int nSel = GetCfgInt(_T("LocalPath"),_T("SaveFileType"),1);
	if (!m_bOpenSave)
		--nSel;
	if (nSel >= 0 && nSel < m_pComSearchRange->GetCount())
		m_pComSearchRange->SelectItem(nSel);
	else
	{
		m_pComSearchRange->SelectItem(1);
		nSel = 1;
	}
	// 上次若是F_RECENT类型，不处理
	// 设置初始列表中的内容

	//解决类似BUG #10614 ////////////////////////////////////////////////////////////////////////
	m_pComSearchRange->SelectItem(-1);
	m_pComSearchRange->SelectItem(nSel);
	//////////////////////////////////////////////////////////////////////////

	if (m_pComSearchRange->GetCurSel() == 0 && m_bOpenSave)
	{
		SetSpecialFolder(CSIDL_RECENT);
		m_strCurPath = m_strRecent;
	}
	else
	{
		CString strPath = GetCfgStr(_T("LocalPath"),_T("SaveNearest"),_T(""));
		m_pListContent->RemoveAll();
		m_vListItem.clear();
		// 以下代码为支持“上一级”操作
		m_vFilePath.clear();

		int nCurPos = 0;
		CString strTemp = _T("");
		CString strPersonalPath = _T("");
		if (!m_bOpenSave)
		{
			++nSel;
		}
		switch (nSel)
		{
		case 1:
			if (m_bXPE)
			{
				strPersonalPath = _T("d:\\AVCON7_XPE\\");
			}
			else
			{
				strPersonalPath.Format(_T("%s%s"), GetExePath(), _T("MyTerminalFolder\\"));
			}
			m_vFilePath.push_back(make_pair(strPersonalPath, F_PERSONAL));
			nCurPos = strPath.Find(m_strPersonal, nCurPos) + m_strPersonal.GetLength();
			nCurPos = strPath.Find(_T("\\"), nCurPos) + 1;
			while (nCurPos != 0) 
			{ 
				strTemp = strPath.Left(nCurPos);
				if (::PathIsDirectory(strTemp))	
					m_vFilePath.push_back(make_pair(strTemp, F_NORMAL));
				else 
					break;
				nCurPos = strPath.Find(_T("\\"), nCurPos) + 1;
			}
			m_nCurFolder = m_vFilePath.size() - 1;
			if (m_nCurFolder == 0)
			{
				UpdateUpwardBtnState(F_PERSONAL);
				m_strCurPath = m_strPersonal;	// 设置当前目录
				ShowListContent(m_strPersonal, NULL);	// 初始化列表中内容
			}
			else
			{
				UpdateUpwardBtnState(F_NORMAL);
				m_strCurPath = m_vFilePath.back().first;	// 设置当前目录
				ShowListContent(m_vFilePath.back().first, NULL);	// 初始化列表中内容
			}
			break;

		case 2:
			m_vFilePath.push_back(make_pair(m_strDesktop, F_DESKTOP));
			//SetSpecialFolder(CSIDL_DESKTOPDIRECTORY);
			nCurPos = strPath.Find(m_strDesktop, nCurPos) + m_strDesktop.GetLength();
			nCurPos = strPath.Find(_T("\\"), nCurPos) + 1;
			while (nCurPos != 0) 
			{ 
				strTemp = strPath.Left(nCurPos);
				if (::PathIsDirectory(strTemp))	
					m_vFilePath.push_back(make_pair(strTemp, F_NORMAL));
				else 
					break;
				nCurPos = strPath.Find(_T("\\"), nCurPos) + 1;
			}
			m_nCurFolder = m_vFilePath.size() - 1;
			if (m_nCurFolder == 0)
			{
				UpdateUpwardBtnState(F_DESKTOP);
				m_strCurPath = m_strDesktop;	// 设置当前目录
				SetDeskTopList();
				//ShowListContent(m_strDesktop, NULL);	// 初始化列表中内容
			}
			else
			{
				UpdateUpwardBtnState(F_NORMAL);
				m_strCurPath = m_vFilePath.back().first;	// 设置当前目录
				ShowListContent(m_vFilePath.back().first, NULL);	// 初始化列表中内容
			}

			if (m_bXPE)	//BUG #11670 终端机上，媒体共享、文档共享、播放录像窗口里面，都能看到“我的电脑”
			{
				OnComboxSelChanged(OSB_COM_SEARCHRANGE);
			}

			break;

		case 3:
			//SetSpecialFolder(CSIDL_DRIVES);
			m_vFilePath.push_back(make_pair(_T(""), F_COMPUTER));
			nCurPos = strPath.Find(_T("\\"), nCurPos) + 1;
			while (nCurPos != 0) 
			{ 
				strTemp = strPath.Left(nCurPos);
				if (::PathIsDirectory(strTemp))	
					m_vFilePath.push_back(make_pair(strTemp, F_NORMAL));
				else 
					break;
				nCurPos = strPath.Find(_T("\\"), nCurPos) + 1;
			}
			m_nCurFolder = m_vFilePath.size() - 1;
			if (m_nCurFolder == 0)
			{
				UpdateUpwardBtnState(F_COMPUTER);
				ShowDrivesList();
				m_strCurPath = GetPluginsLangString( _T("MY_COMPUTER"), _T("我的电脑"));
			}
			else
			{
				UpdateUpwardBtnState(F_NORMAL);
				m_strCurPath = m_vFilePath.back().first;	// 设置当前目录
				ShowListContent(m_vFilePath.back().first, NULL);	// 初始化列表中内容
			}
			break;
		}
	}
	CString strTemp = _T("");
	strTemp.Format(_T("(%s)"), PhrasesFilePath(m_strCurPath));
	m_pLblPath->SetText(strTemp);	// 将当目录显示到窗口上
	m_pLblPath->SetToolTip(m_strCurPath);	// 路径显示不全时提示

}

void CDlgOpenSave::OnFinalMessage( HWND hWnd )
{
	delete this;
}

bool CDlgOpenSave::CheckFileName(CString str, CString strPath)
{
	IPlugin_U7T_TipWindow* pPluginTip = dynamic_cast<IPlugin_U7T_TipWindow*>(g_System->GetPluginByName("Plugin_U7T_TipWindow"));
	IPlugin_U7_Lang* pPluginLang	= GetPluginU7Lang();
	ASSERT(pPluginTip != NULL);
	ASSERT(pPluginLang != NULL);

	//判断是不是文件夹
	BOOL bDirectroy = false;
	WIN32_FIND_DATA FindData;
	HANDLE hError;
	hError = FindFirstFile(strPath, &FindData);
	if (hError != INVALID_HANDLE_VALUE)
	bDirectroy = FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	FindClose(hError);

	if (!bDirectroy)
	{
		int nfind = str.ReverseFind('.');
		if(-1 != nfind)
			str = str.Left(nfind);
	}
	std::string strCkeck = CCommFunction::U2A(str);
	int size = strCkeck.length();

	//文件名不能为空
	char* pstr = new char[size+1];
	CString strError = _T("");
	strcpy_s(pstr , size+1, strCkeck.c_str());
	pstr[size] = 0;
	if(size <= 0)
	{
		strError = pPluginLang->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("ERROR_FILENAME_EMPTY"), _T("必须输入文件名!"));
		pPluginTip->ShowTipWindow(strError, 3, GetHWND());
		delete []pstr;
		return false;
	}
	//文件名不能全为空格
	UINT nSpace = 0;
	for (int i = 0; 0 != pstr[i]; i++)
	{	
		if(isspace(pstr[i]))
			nSpace++;
	}
	if (nSpace == strlen(pstr))
	{
		delete []pstr;
		return false;
	}
	//文件名不能包含特殊字符
	for (int i = 0; i < size; i++)
	{
		if (pstr[i]== '\\'
			|| pstr[i] == '/'
			|| pstr[i] == ':'
			|| pstr[i] == '*'
			|| pstr[i] == '\?'
			|| pstr[i] == '\"'
			|| pstr[i] == '<'
			|| pstr[i] == '>'
			|| pstr[i] == '|')
		{
			strError = pPluginLang->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), _T("ERROR_FILENAME_ERROR"), _T("文件名不能包含下列任何字符:"));
			strError += _T("\n \\ / : * \? \" < > |");
			pPluginTip->ShowTipWindow(strError, 3, GetHWND());
			delete []pstr;
			return false;
		}
	}
	delete []pstr;
	return true;
}