#if !defined(AFX_MYB_H__3832DDEF_0C12_11D5_B6BE_00E07D8144D0__INCLUDED_)
#define AFX_MYB_H__3832DDEF_0C12_11D5_B6BE_00E07D8144D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_CXSHADE_RADIO WM_USER+0x100
/////////////////////////////////////////////////////////////////////////////
// CxSkinButton window
class CxSkinButton : public CButton
{
// Construction
public:
	CxSkinButton();

// Attributes
private:
	BOOL m_bDispText;//��ť����ʾ������

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxSkinButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
// Implementation
public:
	void AddTool(LPCTSTR strTip);
	void SetDispText(BOOL btext);//���ð�ť����ʾ������
	void SetToolTipText(CString s);//���ð�����������ʾ������
	COLORREF SetTextColor(COLORREF new_color);//����������ɫ
	//���ð�ť����״̬�µ�λͼ��ʹ����Դid��
	void SetSkin(UINT normal,UINT down, UINT over=0, UINT disabled=0, UINT focus=0,UINT mask=0,
				short drawmode=1,short border=1,short margin=4);
	//���ð�ť����״̬�µ�λͼ��ʹ��λͼ��
	void SetSkin(HBITMAP normal,HBITMAP down, HBITMAP mask,HBITMAP over=0,  HBITMAP focus=0,HBITMAP disabled=0,
				short drawmode=0,short border=0,short margin=0);
	virtual ~CxSkinButton();

	void LoadBmpFromFile(CString bmpnormal,CString bmpfocus,CString bmpmask);
	// Generated message map functions
	void    SetButtonFocus();
	void    SetButtonNormal();

	HBITMAP  m_hBitmapNormal;
	HBITMAP  m_hBitmapFocus;
	HBITMAP  m_hBitmapMask;

	CString  m_strbmpNormal;
	CString  m_strbmpFocus;
	CString  m_strbmpMask;
	
protected:
	CString strTipText;
	bool	m_Checked;	//��ť�Ƿ�λcheck��״̬	//radio & check buttons
	DWORD	m_Style;	//��ť�ķ��			//radio & check buttons
    bool m_tracking;	//׷������Ƿ��ڰ�ť������
    bool m_button_down;	//��ť�Ƿ����ڱ�����
	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);//׷�������ʾ��ʾ���� ����
	CToolTipCtrl m_tooltip;	//��ʾ��ʾ���ݵĴ�����
	//��ť�ļ���״̬λͼ��������ѹ�¡�ʧЧ�����롢����Ƶ����桢��ý���
	CBitmap m_bNormal,m_bDown,m_bDisabled,m_bMask,m_bOver,m_bFocus; //skin bitmaps
	short	m_FocusRectMargin;		//������margin offset//dotted margin offset
	COLORREF m_TextColor;			//��ť����ʾ��������ɫ//button text color
	HRGN	hClipRgn;				//clipping region
	BOOL	m_Border;				//0=flat; 1=3D;
	short	m_DrawMode;				//����ģʽ 0=normal; 1=stretch; 2=tiled;
	HRGN	CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color);//������λͼ��״ת��Ϊ��ť��״
	void	FillWithBitmap(CDC* dc, HBITMAP hbmp, RECT r);//��2�ֻ���ģʽ�������λͼ
	void	DrawBitmap(CDC* dc, HBITMAP hbmp, RECT r, int DrawMode);//����λͼ
	int		GetBitmapWidth (HBITMAP hBitmap);//���λͼ���
	int		GetBitmapHeight (HBITMAP hBitmap);//���λͼ�߶�
	//{{AFX_MSG(CxSkinButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnClicked();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);//����뿪��ť������Ϣ��Ӧ
	afx_msg LRESULT OnRadioInfo(WPARAM, LPARAM); //radio button ʹ��
	afx_msg LRESULT OnBMSetCheck(WPARAM, LPARAM);//����״̬check
	afx_msg LRESULT OnBMGetCheck(WPARAM, LPARAM);//����Ƿ���check ״̬
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_MYB_H__3832DDEF_0C12_11D5_B6BE_00E07D8144D0__INCLUDED_)
