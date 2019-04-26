#pragma once
#include "afxwin.h"
#include <AUDEC/AUDEC_CodecID.h>
#include <AUDEC/AUDEC_Header.h>


// DlgAudioSetting 对话框

class DlgAudioSetting : public CDialog
{
	DECLARE_DYNAMIC(DlgAudioSetting)

public:
	DlgAudioSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgAudioSetting();

// 对话框数据
	enum { IDD = IDD_DIALOG_AUDIO_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	X_AUDIO_CODEC_ID	m_AudioCodecID;
	CComboBox			m_cbxAudioCodec;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
