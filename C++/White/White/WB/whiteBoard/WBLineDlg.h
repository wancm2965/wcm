#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"
#include "resource.h"


// CWBLineDlg �Ի���

class CWBLineDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBLineDlg)

public:
	CWBLineDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBLineDlg();

// �Ի�������
	enum { IDD = IDD_WB_LINEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	void Relayout();
	void SetSkinImage(CString strPath);
	CString GetCaptureFromType(eType btnEmumType);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLinebtn();
	afx_msg void OnBnClickedCurvvebtn();
	afx_msg void OnBnClickedIcoCurvebtn();
private:
	CWnd* m_pParentDlg;
	Bitmap* m_pBitmap;
	CTransparentButtonEx *m_pParentBtn;
	CTransparentButtonEx m_lineBtn;//ֱ�߰�ť
	CTransparentButtonEx m_curveBtn;//���߰�ť
	CTransparentButtonEx m_icocurveBtn;//ͼ�����߰�ť
	CTransparentButtonEx m_magcurveBtn;//ħ���߰�ť
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedMagcurvebtn();
};
