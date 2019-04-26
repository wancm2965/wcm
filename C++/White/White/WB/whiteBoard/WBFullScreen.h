#pragma once
#include "resource.h"

// CWBFullScreen 对话框

class CWBFullScreen : public CDialog
{
	DECLARE_DYNAMIC(CWBFullScreen)

public:
	CWBFullScreen(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBFullScreen();

	virtual void OnFinalRelease();

// 对话框数据
	enum { IDD = IDD_FULLSCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
	CBitmap* m_pBitmap; //背景位图
	int m_xScreen;
	int m_yScreen;
	int m_nScreenLeft;
	int m_nScreenTop;

public:
	HBITMAP CopyScreenToBitmap(LPRECT lpRect); //拷贝桌面到位图
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
