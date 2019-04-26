#pragma once
#include "afxwin.h"
#include "WBFunctionBtn.h"

// CWBOptionDlg �Ի���

class CWBOptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBOptionDlg)

public:
	CWBOptionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBOptionDlg();

// �Ի�������
	enum { IDD = IDD_WB_OPTIONDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CWBFunctionBtn m_cPreviewButton;
	CWBFunctionBtn m_cAttributeButton;
	CWBFunctionBtn m_cResButton;
	CWBFunctionBtn m_cBGButton;
	CWBFunctionBtn m_cTTButton;
	CWnd* m_pParent;
	CRect m_rect;
	CWBOptionDlg * m_WBOptionDlg;

	bool m_IsSet;  //����Mousemove���޴����ͷ� mouseleave��mousehover����
	bool  m_bIsSetTimer;  //�Ƿ�������ö�ʱ��
public:
	//���û����Ƿ����ʹ�ö�ʱ��
	void SetTime(bool bl) { m_bIsSetTimer= bl;}

	afx_msg void OnBnClickedWbPreviewbtn();
	afx_msg void OnBnClickedWbResbtn();
	afx_msg void OnBnClickedWbAttributebtn();
	afx_msg void OnBnClickedWbBackgroundbtn();
	afx_msg void OnBnClickedWbTeachtool();

	void OnOptionBtnClicked(int BtnId);
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnOK();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void PostNcDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnWM_SHOWOPTIONBTN(WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
