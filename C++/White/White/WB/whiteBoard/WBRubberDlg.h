#pragma once
#include "TransparentButtonEx.h"
#include "afxwin.h"

// CWBRubberDlg �Ի���

class CWBRubberDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBRubberDlg)

public:
	CWBRubberDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBRubberDlg();

// �Ի�������
	enum { IDD = IDD_WB_RUBBERDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
public:
	void Relayout();
	void SetSkinImage(CString strPath);
	CString GetCaptureFromType(eType btnEmumType);
public:
	virtual BOOL OnInitDialog();
private:
	CWnd* m_pParentDlg;
	Bitmap* m_pBitmap;
	CTransparentButtonEx *m_pParentBtn;
	CTransparentButtonEx m_BigRubberBtn;
	CTransparentButtonEx m_CurveRubberBtn;
	CTransparentButtonEx m_ObjRubberBtn;
public:
			afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClickedObjRubberbtn();
	afx_msg void OnClickedLitterRubberbtn();
	afx_msg void OnClickedCurveRubberbtn();
	afx_msg void OnClickedBigRubberbtn();
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
