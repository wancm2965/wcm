#ifndef WHITEBOARD_MOUSEDLG
#define WHITEBOARD_MOUSEDLG

#include "afxwin.h"

#ifndef _DWMAPI_H_
#define DWM_EC_DISABLECOMPOSITION         0
#define DWM_EC_ENABLECOMPOSITION          1
typedef struct _LMARGINS
{
	int cxLeftWidth;      // width of left border that retains its size
	int cxRightWidth;     // width of right border that retains its size
	int cyTopHeight;      // height of top border that retains its size
	int cyBottomHeight;   // height of bottom border that retains its size
} LMARGINS, *PLMARGINS;
#endif


// CMouseDlg 对话框


class CMouseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMouseDlg)

public:
	CMouseDlg(CWnd* pParent);   // 标准构造函数
	CMouseDlg(); 
	virtual ~CMouseDlg();
	virtual void OnFinalRelease();
	void  GuidesShift(BOOL isTransparent);
	void FullScreenShift(bool isFullScreen);
	void Shift(BOOL isTransparent);
	void DestroyScreenWindow();
	//// 对话框数据
	enum { IDD = IDD_MOUSEDLG };
public:
	///add by aolingwen 2012.12.12
	bool	 PtInScreenDrawDlg(const POINT &pt)const;			///区域判断
	bool	 IsFullScreen(){return m_isFullScreen;}				///供外部调用
	bool     IsTransparent(){return m_isTranspartentOrNot;}		///供外部调用
	CRect&   GetRectSize(){return m_rectSize;}					///供外部调用
	///end
	bool m_MouseDlgLButtonDown;  
	void SetMouseStatue(bool IsDown){m_MouseDlgLButtonDown = IsDown;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);		///调用ScreenDrawDlg的lbuttondown
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);		///调用ScreenDrawDlg的lbuttonup
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);		///调用ScreenDrawDlg的mousemove
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDestroy();
	//覆盖系统的，不做任何处理
	afx_msg LRESULT OnGesture(WPARAM /*wParam*/, LPARAM lParam);
	afx_msg LRESULT OnTouch(WPARAM wParam, LPARAM lParam);

private:
	void SetTransparent();
	void Transpart();
	void Glass();
	void SetGlass();
	void ChangeSizeOrPos();
private:

	bool m_isTranspartentOrNot;			///使用透明还是玻璃效果
	bool m_isFullScreen;				///是否全屏
	CRect m_rClientRect;				///客户区域
	CRect m_rectSize;					///窗口大小
	LPCTSTR m_systemName;				///操作系统名
	const COLORREF m_maskCol;			///窗口背景色(灰色)
	LPCTSTR getSystemName();
	//仅用于wm_touch消息
	std::map<DWORD, MTCursor*>	m_mapTouchCursor;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
#endif