#pragma once
#include "resource.h"

// CWBFullScreen �Ի���

class CWBFullScreen : public CDialog
{
	DECLARE_DYNAMIC(CWBFullScreen)

public:
	CWBFullScreen(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBFullScreen();

	virtual void OnFinalRelease();

// �Ի�������
	enum { IDD = IDD_FULLSCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
	CBitmap* m_pBitmap; //����λͼ
	int m_xScreen;
	int m_yScreen;
	int m_nScreenLeft;
	int m_nScreenTop;

public:
	HBITMAP CopyScreenToBitmap(LPRECT lpRect); //�������浽λͼ
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
