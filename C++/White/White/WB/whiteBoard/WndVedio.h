#pragma once


// CWndVedio �Ի���

class CWndVedio : public CDialog
{
	DECLARE_DYNAMIC(CWndVedio)

public:
	CWndVedio(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWndVedio();

// �Ի�������
	enum { IDD = IDD_Vedio };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	HCURSOR    m_MoveCursor;   //�ƶ����
	HWND       m_CurHwnd;      //������Ż�ȡ�Ŀؼ��ľ��
	HWND       m_PreHwnd;      //���������ǰ��ȡ�Ŀؼ��ľ��
	BOOL       m_LeftIsDown;   //TRUE��ʾ����������
	HINSTANCE  m_hDLL;
	HWND       m_GetCurHwnd;  
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	HWND  GetWndHwnd();
	afx_msg void OnStnClickedCatch();
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	virtual BOOL OnInitDialog();
};
