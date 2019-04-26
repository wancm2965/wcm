#pragma once
#include "afxwin.h"
#include <IMXEC/IMXEC_AVCONCall.h>


// DlgAudioSetting 对话框

class DlgAudioSetting : public CDialog
{
	DECLARE_DYNAMIC(DlgAudioSetting)

public:
	DlgAudioSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgAudioSetting();

	int					m_nMicDevID;
	int					m_nSpkDevID;
	int					m_nAEC;
	int					m_nVAD;
	int					m_nAGC;
	int					m_nNS;

// 对话框数据
	enum { IDD = IDD_DIALOG_AUDIO_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CComboBox m_cbxMic;
	CComboBox m_cbxSpk;
	CButton m_btnAEC;
	CButton m_btnVAD;
	CButton m_btnAGC;
	CButton m_btnNS;
};
