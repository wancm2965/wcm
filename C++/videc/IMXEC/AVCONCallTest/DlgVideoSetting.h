#pragma once
#include "afxwin.h"
#include <IMXEC/IMXEC_AVCONCall.h>


// DlgVideoSetting 对话框

class DlgVideoSetting : public CDialog
{
	DECLARE_DYNAMIC(DlgVideoSetting)

public:
	DlgVideoSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgVideoSetting();
public:
	int					m_nDevID;
	int					m_nFrameRate;
	int					m_nBitrate;
	int					m_nCbrOrVbr;

	IMXEC_AVCONCall::VIDEO_FORMAT	m_nVideoFormatType;

// 对话框数据
	enum { IDD = IDD_DIALOG_VIDEO_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CComboBox m_cbxDev;
	CComboBox m_cbxFrameRate;
	CComboBox m_cbxBitrate;
	CComboBox m_cbxFormat;

	virtual BOOL OnInitDialog();
};
