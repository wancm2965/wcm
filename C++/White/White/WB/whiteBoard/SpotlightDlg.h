#pragma once

// SpotlightDlg.h : ͷ�ļ�
class CWMBtn;
class CWBSpotlight;

#define BTN_CLOSE_ID WM_USER + 1

// CSpotlightDlg �Ի���
class CSpotlightDlg : public CDialog
{
public:
	CSpotlightDlg(CWBSpotlight *pWBSpotlight, CRect screenRect = NULL); // ���캯��
	~CSpotlightDlg();
	
private:
	INT static  m_nCount;
	//BOOL  pos;
	CRect m_ScreenRect; //����Բ����
	HWND m_ParenthWnd;	
	INT	m_transparent;// ͸���ȿ̶�
	CWMBtn* m_pSpotlightClose; //�رհ�ť
	HICON		m_hCrossCursor;// ���
	CPoint		m_prePoint;
	COLORREF	m_bgColor;// ����ɫ
	LPTSTR m_pClassName;
	CWBSpotlight *m_pWBSpotlight;
	CRect m_rect;  //����������
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
	//������С
	void AdjustSize(CRect& newSize,bool bSendNet);
private:
	void GetScreenRgn(CRgn &rgn, CRect rt);
	void UpdateTransparent();
public:
	afx_msg void OnPaint();
	// ���þ۹�ƴ��ڵ�λ��
	void SetSpotlightPos(CRect windowRect);
};