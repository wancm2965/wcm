#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include <VIDEC/VIDEC.h>

// DlgSetting 对话框
class DlgMain;
class DlgSetting : public CDialog
{
	DECLARE_DYNAMIC(DlgSetting)

public:
	DlgSetting(DlgMain&rDlgMain);   // 标准构造函数
	virtual ~DlgSetting();

	void FillQuality(void);
	void FillFormat(void);

	DlgMain&		m_rDlgMain;

// 对话框数据
	enum { IDD = IDD_DIALOG_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbxDev;
	CComboBox m_cbxFrameRate;
	CComboBox m_cbxCodec;
	CComboBox m_cbxQuality;
	CComboBox m_cbxFormat;
	CEdit m_editLocalPort;
	CEdit m_editRemotePort;
	UINT m_nLocalPort;
	UINT m_nRemotePort;
	CIPAddressCtrl m_ctrlRemoteIP;
	DWORD m_ulRemoteIP;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboDev();
	afx_msg void OnBnClickedOk();
};
