#pragma once
#include "afxwin.h"
#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <IMXEC/IMXEC_MediaChannel.h>


// DlgVideoSetting �Ի���

class DlgVideoSetting : public CDialog
{
	DECLARE_DYNAMIC(DlgVideoSetting)

public:
	DlgVideoSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgVideoSetting();
public:
	int					m_nDevID;
	VIDEC_CODEC_TYPE	m_VideoCodecType;
	int					m_nFrameRate;
	int					m_nBitrate;
	int					m_nWidth;
	int					m_nHeight;

	void FillFormat(void);
// �Ի�������
	enum { IDD = IDD_DIALOG_VIDEO_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CComboBox m_cbxDev;
	CComboBox m_cbxFrameRate;
	CComboBox m_cbxBitrate;
	CComboBox m_cbxCodec;
	CComboBox m_cbxFormat;

	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboDev();
};
