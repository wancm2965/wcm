#pragma once
#include "CWaterRoutine.h"
#include "DIBSectionLite.h"

// CWaterDlg �Ի���

class CWaterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWaterDlg)

public:
	CWaterDlg(CBitmap *bitmap,CPoint point,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWaterDlg();
	CWaterRoutine	m_myWater;
	//BOOL m_bRunWater;
	CBitmap *m_bitmap;
	CDIBSectionLite	m_bmpRenderSource;
	CDIBSectionLite m_bmpRenderTarget;	
	CPoint m_waterpoint;
// �Ի�������
	enum { IDD = IDD_WATER_DIALOG };
	int m_number;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
public:
	void SetBackgroundBitmap(CBitmap *bitmap,CPoint point);	
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
};
