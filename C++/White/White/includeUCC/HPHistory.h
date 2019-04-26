
#pragma once

#include "HPDLG_Base.h"
#include <string>
#include <map>
using namespace std;

class AFX_EXT_CLASS HPDLG_HistoryNotify
{
public:
	HPDLG_HistoryNotify(void){};
	virtual~ HPDLG_HistoryNotify(void){};

public:
	virtual void OnGetTeam(std::map<unsigned long, std::string>& mapTeam) = 0;
	virtual void OnGetDiscussGroup(std::map<unsigned long, std::string>& mapGroup) = 0;
};

class AFX_EXT_CLASS HPDLG_History
{
public:
	HPDLG_History(void){};
	virtual~ HPDLG_History(void){};

	// 显示/隐藏对话框
	virtual void ShowWindow(int nCmdShow) = 0;

	//当前框口是否已打开
	virtual BOOL IsWindowShow() = 0;
};
// 创建对话框
HPDLG_API HPDLG_History* CreateHPHistoryDlg(CString strUserid, CWnd* pParent, HPDLG_HistoryNotify& rNotify);
// 获得指针
HPDLG_API HPDLG_History* GetHPDLG_History(void);
// 销毁对话框
HPDLG_API void DestroyHPHistoryDlg();
