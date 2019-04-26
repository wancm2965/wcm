#pragma once
#include "afxwin.h"
#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <IMXEC/IMXEC_MediaChannel.h>
#include "CameraData.h"
#include "DlgCamera.h"

// CDlgCameraVideoSetting 

// CString ReadConfigTextVideo(CString strSelection, LPCTSTR ControlName);//配置文本读写
// 
// CString ReadConfigTextVideoIni(CString strSelection, LPCTSTR ControlName);//配置文本读写
// void WriteConfigTextVideoIni(CString strSelection, LPCTSTR ControlName,int iValue);//配置文本读写


class CDlgCameraVideoSetting : public CDialog
{
	DECLARE_DYNAMIC(CDlgCameraVideoSetting)

public:
	CDlgCameraVideoSetting(CWnd* pParent = NULL, CameraSet *pData = NULL);   // 标准构造函数
	virtual ~CDlgCameraVideoSetting();
public:
	
	CameraSet *GetCameraPtr(){return m_pVideoCameraSet;}
	void FillFormat(void);
// 对话框数据
	//enum { IDD = IDD_DIALOG_CAMERA_VIDEO_SETTING};
private:
	//设置数据指针
	CameraSet  *m_pVideoCameraSet;
	//摄像头视频设置是否被更改的标识变量
	BOOL   m_bVideoSettingUpdated;

	int    m_UpdataVideoCodecType;
	int    m_UpdataFrameRate;
	int    m_UpdatamBitrate;


	//视频编码（）
	int nVideoDevID;
	int nViedoFrameRate;
	int nVideoBitrate;
	int nVideoCodec;
	int nVideoFormat;


	CWnd *m_DlgCamera;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	//void UpdataDataVideoSetting(BOOL UpdataVideoSet){m_UpdataVideoSetting = UpdataVideoSet;}
	//void SetUpdataVideoCode(int bUpdataVideoCode) {m_UpdataVideoCodecType = bUpdataVideoCode;}
	//void SetUpdataFrameRate(int bUpdataFrameR)  {m_UpdataFrameRate = bUpdataFrameR ;} 
	//void SetUpdataBitrate(int bUpdataBitrate)  {m_UpdatamBitrate = bUpdataBitrate;}
	DECLARE_MESSAGE_MAP()
public:
	void LoadCameraVideoSetting();

	void GetCameraVideoSetting();

	afx_msg void OnBnClickedOk();
	CComboBox m_cbxDev;
	CComboBox m_cbxFrameRate;
	CComboBox m_cbxBitrate;
	CComboBox m_cbxCodec;
	CComboBox m_cbxFormat;
	BOOL IsVideoSetUpdated(){return m_bVideoSettingUpdated;}
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboDev();

	void VideoSettingLanguage();

};
