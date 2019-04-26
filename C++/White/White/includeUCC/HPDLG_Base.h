#pragma once


//========================================================================
//类名：HIDLG基础类
//功能：对话框资源的基本方法
//作者：杜权胜
//时间：2008-11-18
//说明：
//--------------------------------------------------------------------------

#define HPDLG_HAND (void*)

#define HPDLG_EXPORT
#ifdef HPDLG_EXPORT
#define HPDLG_API _declspec(dllexport)
#elif HPDLG_DLL
#define HPDLG_API _declspec(dllimport)
#else
#define HPDLG_API
#endif


class HPDLG_API HPDLG_Base
{
public:
	HPDLG_Base();
	~HPDLG_Base(){};
	HWND GetSafeHwnd() const;
	BOOL ShowWindow(int nCmdShow);
	BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags);
	void GetWindowRect(LPRECT lpRect) const;
	bool IsWindowVisible(void);
	CWnd* SetParent(CWnd* pWndNewParent);
protected:
	HPDLG_HAND m_hWnd;
};
