#pragma once
class IHPNetNotify
{
public:
	virtual void SendWbData(const char* pData,int iLen,bool bToNet = true) = 0;
	virtual bool OpenSaveFile(BOOL bOpenSave,char* sFilePath,LPCTSTR lpszDefExt = NULL,LPCTSTR lpszFilter = NULL,HWND hParent=NULL ,LPCTSTR strFilePath =_T("")) = 0;
	virtual int MessageBox(HWND hOwner,const CString & strTitle,const CString & strContent,UINT nStye, bool bAutoDestroy = false) = 0;
	virtual void ShowTipWindow(const CString& strTip,int nSeconds = 3,HWND pParentWnd = NULL) = 0;
	virtual void SetSelectObjStatue(MSG msg) =0;
};