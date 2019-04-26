// FolderSelect.cpp: implementation of the CFolderSelect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "dsstudent.h"
#include "FolderSelect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//extern CString g_LoadString(CString strID);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static int CALLBACK BrowseDirectoryCallbackProc(HWND hWnd,UINT uMsg,
												LPARAM lParam,LPARAM lpData)
{
	CFolderSelect * pFd = (CFolderSelect *) lpData;
	pFd->CallbackFunction(hWnd,uMsg,lParam);

	return 0;
}

CFolderSelect::CFolderSelect()
{
	m_strInitialFolderName = _T("");

	memset(&m_bi,'\0',sizeof(BROWSEINFO));
	
	m_bi.hwndOwner = 0;

	m_bi.pidlRoot = NULL;
	m_bi.pszDisplayName = m_szDisplayName;
//	m_szTitleCurSelect=g_LoadString("IDS_TITLE_FOLDERSELECT");
	m_bi.lpszTitle = (LPCTSTR)m_szTitleCurSelect;
	m_bi.ulFlags = BIF_STATUSTEXT;
	m_bi.lpfn = BrowseDirectoryCallbackProc;
	m_bi.lParam = (LPARAM)this;
}

CFolderSelect::~CFolderSelect()
{

}

CFolderSelect::CFolderSelect(LPCTSTR lpszFolderName,DWORD dwFlags,CWnd * pParentWnd)
{
	if(lpszFolderName == NULL)
		m_strInitialFolderName = _T("");
	else
		m_strInitialFolderName = lpszFolderName;

	memset(&m_bi,'\0',sizeof(BROWSEINFO));
	
	if(pParentWnd == NULL)
		m_bi.hwndOwner = 0;
	else
		m_bi.hwndOwner = pParentWnd->GetSafeHwnd();
	
	m_bi.pidlRoot = NULL;
	m_bi.pszDisplayName = m_szDisplayName;
//	m_szTitleCurSelect=g_LoadString("IDS_TITLE_FOLDERSELECT");
	m_bi.lpszTitle = (LPCTSTR)m_szTitleCurSelect;
	m_bi.ulFlags = dwFlags | BIF_STATUSTEXT;
	m_bi.lpfn = BrowseDirectoryCallbackProc;
	m_bi.lParam = (LPARAM)this;
}

void CFolderSelect::CallbackFunction(HWND hWnd, UINT uMsg, LPARAM lParam)
{
	m_hDialogBox = hWnd;

	switch(uMsg){
	case BFFM_INITIALIZED:
		OnInitDialog();
		break;
	case BFFM_SELCHANGED:
		OnSelChanged((ITEMIDLIST *)lParam);
		break;
	}
}

void CFolderSelect::OnInitDialog()
{
	SetSelection(m_strInitialFolderName);
	SetStatusText(ShortName(m_strInitialFolderName));
}

void CFolderSelect::OnSelChanged(ITEMIDLIST *pId)
{
	::SHGetPathFromIDList(pId,m_szPath);
	m_strFinalFolderName = m_szPath;
	SetStatusText(ShortName(m_strFinalFolderName));
}

int CFolderSelect::DoModal()
{
	int nReturn = IDOK;

	m_strFinalFolderName = m_strInitialFolderName;
	ITEMIDLIST * pid = NULL;

	pid = ::SHBrowseForFolder(&m_bi);

	if(pid && ::SHGetPathFromIDList(pid,m_szPath)){
		m_strFinalFolderName = m_szPath;
		nReturn = IDOK;
	}else{
		nReturn = IDCANCEL;
	}

	if(pid){
		LPMALLOC lpMalloc;
		VERIFY(::SHGetMalloc(&lpMalloc) == NOERROR);
		lpMalloc->Free(pid);
		lpMalloc->Release();
	}
	return nReturn;
}

void CFolderSelect::EnableOK(BOOL bEnable)
{
	::SendMessage(m_hDialogBox,BFFM_ENABLEOK,0,(bEnable?1:0));
}

void CFolderSelect::SetSelection(LPCTSTR pszSelection)
{
	::SendMessage(m_hDialogBox,BFFM_SETSELECTION,TRUE,(LPARAM) pszSelection);
}

void CFolderSelect::SetSelection(ITEMIDLIST *pId)
{
	::SendMessage(m_hDialogBox,BFFM_SETSELECTION,FALSE,(LPARAM)pId);
}

void CFolderSelect::SetStatusText(LPCTSTR pszStatusText)
{
	::SendMessage(m_hDialogBox,BFFM_SETSTATUSTEXT,0,(LPARAM) pszStatusText);
}

CString CFolderSelect::ShortName(const CString &strName)
{
	CString strShort;
	if(strName.GetLength()<= 35)
		strShort = strName;
	else{
		strShort = strName.Left(20)+ _T(" ... ");
		strShort += strName.Right(20);
//		strShort = strName.Left(35)+ _T("...");
	}
	return strShort;
}

CString CFolderSelect::GetPathName()
{
	return m_strFinalFolderName;
}
