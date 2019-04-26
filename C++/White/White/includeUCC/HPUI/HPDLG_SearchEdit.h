//========================================================================
//类名：查询编辑筐
//功能：
//作者：杜权胜
//时间：2008-11-18
//说明：
//--------------------------------------------------------------------------


#pragma once

#include "HPDLG_Base.h"


class HPDLG_API HPDLG_SrhEditNotify
{
public:
	HPDLG_SrhEditNotify(){};
	~HPDLG_SrhEditNotify(){};
public:	
	virtual void OnSrhEditNotifyBtnSel(unsigned long ulSessionID){};
	virtual void OnSrhEditNotifyUpData(unsigned long ulSessionID){};
	virtual void OnSrhEditNotifyEnter(unsigned long ulSessionID){};
	virtual void OnSrhEditNotifyFocus(unsigned long ulSessionID){};
	virtual void OnSrhEditNotifyKillFocus(unsigned long ulSessionID){};
};

class HPDLG_API HPDLG_SrhEdit : public HPDLG_Base
{
public:
	HPDLG_SrhEdit();
	~HPDLG_SrhEdit();

	void Create(HPDLG_SrhEditNotify& rNotify,CWnd* pParent = NULL,unsigned long ulSessionID = 0);
	void Destroy();
	void GetWindowText(CString& rString) const;
	BOOL EnableWindow(BOOL bEnable = TRUE);
	void SetToolTip(CString strToolTip);
	void SetBtnBitmapAndTip(CString strBitMapFile,CString strToolTip);
	void SetEditDefText(CString strText);
	void SetWindowText(LPCTSTR lpszString);
	CWnd* SetFocus();
};