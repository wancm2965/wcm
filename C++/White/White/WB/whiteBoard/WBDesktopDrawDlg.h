#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"

// CWBDesktopDrawDlg �Ի���

class CWBDesktopDrawDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBDesktopDrawDlg)

public:
	CWBDesktopDrawDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBDesktopDrawDlg();

// �Ի�������
	enum { IDD = IDD_WB_SCREENDRAWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CWnd* m_pParentDlg; //������������
	Bitmap* m_pBitmap;
	CTransparentButtonEx *m_pParentBtn;

	CTransparentButtonEx m_DDOpenBtn;
	CTransparentButtonEx m_DDEffectBtn;
//	CTransparentButtonEx m_DDFullScreenBtn;
//	CTransparentButtonEx m_DDCtrlBtn;
	CTransparentButtonEx m_DDCloseBtn;
	virtual void OnOK();
	virtual void OnCancel();
public:
	void Relayout();
	void SetSkinImage(CString strPath);
	CString GetCaptureFromType(eType btnEmumType);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedDdopenbtn();
	afx_msg void OnBnClickedDdeffectbtn();
	afx_msg void OnBnClickedDdfullscreenbtn();
	afx_msg void OnBnClickedDdctrlbtn();
	afx_msg void OnBnClickedDdclosebtn();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
