#pragma once
#include "afxwin.h"
#include <AUDEC/AUDEC_CodecID.h>
#include <AUDEC/AUDEC_Header.h>
#include "CameraData.h"

// CDlgCameraAudioSetting 对话框

class CDlgCameraAudioSetting : public CDialog
{
	DECLARE_DYNAMIC(CDlgCameraAudioSetting)

public:
	CDlgCameraAudioSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCameraAudioSetting();

// 对话框数据
	//enum { IDD = IDD_DIALOG_CAMERA_AUDIO_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


	DECLARE_MESSAGE_MAP()
private:
	//音频编码（记录位置）
	int nAudioCodecID;
public:
	/*X_AUDIO_CODEC_ID	m_AudioCodecID;*/
	CameraSet   *m_AoutCameraSet;
	int    m_UpdataAudioCodecType;
	BOOL   m_bAudioSettingUpdated;

	CComboBox	m_cbxAudioCodec;

	CameraSet *GetCameraPtr(){return m_AoutCameraSet;}

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	int LoadWBCamerAudioSetting();
	void SaveWBCamerAudioSetting();

	void AudioSettingLanguage();
};
