// FolderSelect.h: interface for the CFolderSelect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOLDERSELECT_H__694D7E46_7DB0_11D5_AD54_00D0F80B6B14__INCLUDED_)
#define AFX_FOLDERSELECT_H__694D7E46_7DB0_11D5_AD54_00D0F80B6B14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFolderSelect  
{
public:
	CString GetPathName();
	void EnableOK(BOOL bEnable);
	virtual int DoModal();
	CFolderSelect(LPCTSTR lpszFolderName,DWORD dwFlags = NULL,CWnd * pParentWnd = NULL);
	CFolderSelect();
	virtual ~CFolderSelect();
	BROWSEINFO m_bi;
	virtual void CallbackFunction(HWND hWnd,UINT uMsg,LPARAM lParam);

protected:
	CString ShortName(const CString &strName);
	void SetStatusText(LPCTSTR pszStatusText);
	void SetSelection(ITEMIDLIST *pId);
	void SetSelection(LPCTSTR pszSelection);
	virtual void OnSelChanged(ITEMIDLIST * pId);
	virtual void OnInitDialog();
	HWND m_hDialogBox;
	TCHAR m_szPath[MAX_PATH];
	TCHAR m_szDisplayName[MAX_PATH];
	CString m_strFinalFolderName;
	CString m_strInitialFolderName;
private:
	CString m_szTitleCurSelect;
};

#endif // !defined(AFX_FOLDERSELECT_H__694D7E46_7DB0_11D5_AD54_00D0F80B6B14__INCLUDED_)
