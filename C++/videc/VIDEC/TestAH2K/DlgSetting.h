#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "../../include/VIDEC/VIDEC.h"

#ifdef TEST_AH2K
#include "../../include/VIDEC/VIDEC_AH2KDev.h"
#endif

#ifdef TEST_DVC
#include "../../include/VIDEC/VIDEC_DVCDev.h"
#endif
#ifdef TEST_CAMERA
#include "../../include/VIDEC/VIDEC_CameraDev.h"
#endif
#ifdef TEST_AH410
#include "../../include/VIDEC/VIDEC_AH410Dev.h"
#endif
#ifdef TEST_AH210
#include "../../include/VIDEC/VIDEC_AH210Dev.h"
#endif
#ifdef TEST_AH400
#include "../../include/VIDEC/VIDEC_AH400Dev.h"
#endif

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
	virtual BOOL OnInitDialog();
	CComboBox m_cbxFormat;
	afx_msg void OnCbnSelchangeComboDev();
	afx_msg void OnBnClickedOk();
	BOOL m_bEncode;
	BOOL m_bDecode;
};
