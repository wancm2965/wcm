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


// CMouseDlg �Ի���


class CMouseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMouseDlg)

public:
	CMouseDlg(CWnd* pParent);   // ��׼���캯��
	CMouseDlg(); 
	virtual ~CMouseDlg();
	virtual void OnFinalRelease();
	void  GuidesShift(BOOL isTransparent);
	void FullScreenShift(bool isFullScreen);
	void Shift(BOOL isTransparent);
	void DestroyScreenWindow();
	//// �Ի�������
	enum { IDD = IDD_MOUSEDLG };
public:
	///add by aolingwen 2012.12.12
	bool	 PtInScreenDrawDlg(const POINT &pt)const;			///�����ж�
	bool	 IsFullScreen(){return m_isFullScreen;}				///���ⲿ����
	bool     IsTransparent(){return m_isTranspartentOrNot;}		///���ⲿ����
	CRect&   GetRectSize(){return m_rectSize;}					///���ⲿ����
	///end
	bool m_MouseDlgLButtonDown;  
	void SetMouseStatue(bool IsDown){m_MouseDlgLButtonDown = IsDown;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);		///����ScreenDrawDlg��lbuttondown
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);		///����ScreenDrawDlg��lbuttonup
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);		///����ScreenDrawDlg��mousemove
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDestroy();
	//����ϵͳ�ģ������κδ���
	afx_msg LRESULT OnGesture(WPARAM /*wParam*/, LPARAM lParam);
	afx_msg LRESULT OnTouch(WPARAM wParam, LPARAM lParam);

private:
	void SetTransparent();
	void Transpart();
	void Glass();
	void SetGlass();
	void ChangeSizeOrPos();
private:

	bool m_isTranspartentOrNot;			///ʹ��͸�����ǲ���Ч��
	bool m_isFullScreen;				///�Ƿ�ȫ��
	CRect m_rClientRect;				///�ͻ�����
	CRect m_rectSize;					///���ڴ�С
	LPCTSTR m_systemName;				///����ϵͳ��
	const COLORREF m_maskCol;			///���ڱ���ɫ(��ɫ)
	LPCTSTR getSystemName();
	//������wm_touch��Ϣ
	std::map<DWORD, MTCursor*>	m_mapTouchCursor;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
#endif