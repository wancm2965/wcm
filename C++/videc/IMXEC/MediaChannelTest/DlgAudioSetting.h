#pragma once
#include "afxwin.h"
#include <AUDEC/AUDEC_CodecID.h>
#include <AUDEC/AUDEC_Header.h>


// DlgAudioSetting �Ի���

class DlgAudioSetting : public CDialog
{
	DECLARE_DYNAMIC(DlgAudioSetting)

public:
	DlgAudioSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgAudioSetting();

// �Ի�������
	enum { IDD = IDD_DIALOG_AUDIO_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	X_AUDIO_CODEC_ID	m_AudioCodecID;
	CComboBox			m_cbxAudioCodec;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
