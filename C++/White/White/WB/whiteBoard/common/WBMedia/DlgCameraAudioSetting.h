#pragma once
#include "afxwin.h"
#include <AUDEC/AUDEC_CodecID.h>
#include <AUDEC/AUDEC_Header.h>
#include "CameraData.h"

// CDlgCameraAudioSetting �Ի���

class CDlgCameraAudioSetting : public CDialog
{
	DECLARE_DYNAMIC(CDlgCameraAudioSetting)

public:
	CDlgCameraAudioSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCameraAudioSetting();

// �Ի�������
	//enum { IDD = IDD_DIALOG_CAMERA_AUDIO_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��


	DECLARE_MESSAGE_MAP()
private:
	//��Ƶ���루��¼λ�ã�
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
