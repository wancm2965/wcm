#pragma once

#include "HPDLG_Base.h"

//用以区别下拉列表文件类型
typedef enum _BROWSER_TYPE
{
	BROWSER_TYPE_ALL = 0,
	BROWSER_TYPE_MEDIA,       //AVMPlayer 打开文件类型
	BROWSER_TYPE_CONFERENCE,  //ConferencePlayer 打开文件类型
}BROWSER_TYPE;

class AFX_EXT_CLASS HPDLG_FileBrowser
	: public HPDLG_Base
{
public:
	HPDLG_FileBrowser(void);
	virtual~ HPDLG_FileBrowser(void);

	/*
	函数功能：非模态对话框，打开为直接启动文件
	参    数：pParent	父窗口
	返 回 值：创建是否成功
	*/
	bool Create(CWnd* pParent = NULL, BROWSER_TYPE nBrowserType = BROWSER_TYPE_ALL);

	/*
	函数功能：模态对话框，打开返回文件路径
	参    数：无
	返 回 值：
	*/
	INT_PTR DoModal(BROWSER_TYPE nBrowserType = BROWSER_TYPE_ALL);

	/* 强制关闭对话框*/
	void Destroy();

	//返回文件路径
	CString GetCurSelFilePath();

private:
	CString m_strFilePath;
};
