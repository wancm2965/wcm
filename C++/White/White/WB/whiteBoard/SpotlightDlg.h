#pragma once

// SpotlightDlg.h : 头文件
class CWMBtn;
class CWBSpotlight;

#define BTN_CLOSE_ID WM_USER + 1

// CSpotlightDlg 对话框
class CSpotlightDlg : public CDialog
{
public:
	CSpotlightDlg(CWBSpotlight *pWBSpotlight, CRect screenRect = NULL); // 构造函数
	~CSpotlightDlg();
	
private:
	INT static  m_nCount;
	//BOOL  pos;
	CRect m_ScreenRect; //中心圆坐标
	HWND m_ParenthWnd;	
	INT	m_transparent;// 透明度刻度
	CWMBtn* m_pSpotlightClose; //关闭按钮
	HICON		m_hCrossCursor;// 光标
	CPoint		m_prePoint;
	COLORREF	m_bgColor;// 背景色
	LPTSTR m_pClassName;
	CWBSpotlight *m_pWBSpotlight;
	CRect m_rect;  //主窗体坐标
	BOOL        m_bool;
	int         m_nLargeWidth;
	int         m_nLargeHight;
	//CRgn        m_ScreenRgn;
	int         m_nMousePos;
	BOOL        m_bIsPosChange;
protected:
	
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);	
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
public:
	BOOL CreateMainWnd(CRect rect,HWND hwnd);
	void UpdateWindowRegion();
	BOOL GetScreenRect(CRect &rect);
	void SetScreenRect(CRect &rect);
	void ChangeCloseBtn(CRect rt);
	//调整大小
	void AdjustSize(CRect& newSize,bool bSendNet);
private:
	void GetScreenRgn(CRgn &rgn, CRect rt);
	void UpdateTransparent();
public:
	afx_msg void OnPaint();
	// 设置聚光灯窗口的位置
	void SetSpotlightPos(CRect windowRect);
};