#pragma once
#include "TransparentButtonEx.h"
#include "afxwin.h"

// CWBTextDlg �Ի���

class CWBTextDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBTextDlg)

public:
	CWBTextDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBTextDlg();

// �Ի�������
	enum { IDD = IDD_WB_TEXTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	void Relayout();
	void SetSkinImage(CString strPath);
private:
	CWnd* m_pParentDlg;
	Bitmap* m_pBitmap;
	CTransparentButtonEx *m_pParentBtn;
	CTransparentButtonEx m_HTextBtn;
	CTransparentButtonEx m_VTextBtn;
public:
	afx_msg void OnBnClickedHTextbtn();
	afx_msg void OnBnClickedVTextbtn();
	afx_msg void OnDestroy();
};
