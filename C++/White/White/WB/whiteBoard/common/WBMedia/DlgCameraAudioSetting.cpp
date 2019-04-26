// DlgCameraAudioSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "DlgCameraAudioSetting.h"

#include "ResourceWBMedia.h"
#include ".\common\WBMedia\Markup.h"
#include "ResData.h"
#include "CameraSettingIni.h"

// CDlgCameraAudioSetting 对话框


IMPLEMENT_DYNAMIC(CDlgCameraAudioSetting, CDialog)
	CDlgCameraAudioSetting::CDlgCameraAudioSetting(CWnd* pParent /*=NULL*/ )
	: CDialog(IDD_DIALOG_CAMERA_AUDIO_SETTING, pParent)
	,m_AoutCameraSet (NULL)
	,nAudioCodecID(0)
{    
	m_AoutCameraSet= new CameraSet;
}
 

CDlgCameraAudioSetting::~CDlgCameraAudioSetting()
{
	if(m_AoutCameraSet)
	{
		delete m_AoutCameraSet;
		m_AoutCameraSet=NULL;
	}
}

void CDlgCameraAudioSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAMERA_AUDIO_CODEC, m_cbxAudioCodec);
}


BEGIN_MESSAGE_MAP(CDlgCameraAudioSetting, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgCameraAudioSetting::OnBnClickedOk)
END_MESSAGE_MAP()


// DlgCameraAudioSetting 消息处理程序

BOOL CDlgCameraAudioSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	AudioSettingLanguage();

	if (::IsWindow(m_hWnd))
	{
	 	::SetWindowPos(m_hWnd,HWND_TOPMOST,700,1100,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW );
		CWnd *pwnd = FromHandle(m_hWnd);
		pwnd->CenterWindow();
	}

	int nIndex=m_cbxAudioCodec.AddString(L"G.711 ULaw 64Kbps");
	m_cbxAudioCodec.SetItemData(nIndex,X_AUDIO_CODEC_G711U);
	nIndex=m_cbxAudioCodec.AddString(L"G.711 ALaw 64Kbps");
	m_cbxAudioCodec.SetItemData(nIndex,X_AUDIO_CODEC_G711A);
	nIndex=m_cbxAudioCodec.AddString(L"G.729A 8Kbps");
	m_cbxAudioCodec.SetItemData(nIndex,X_AUDIO_CODEC_G729);
	//nIndex=m_cbxAudioCodec.AddString("G.723.1 6.3Kbps");
	//m_cbxAudioCodec.SetItemData(nIndex,X_AUDIO_CODEC_G7231_63);
	nIndex=m_cbxAudioCodec.AddString(L"G.722 64Kbps");
	m_cbxAudioCodec.SetItemData(nIndex,X_AUDIO_CODEC_G722_64);
	nIndex=m_cbxAudioCodec.AddString(L"G.722.1 24Kbps");
	m_cbxAudioCodec.SetItemData(nIndex,X_AUDIO_CODEC_G7221_14_24);
	nIndex=m_cbxAudioCodec.AddString(L"G.722.1 32Kbps");
	m_cbxAudioCodec.SetItemData(nIndex,X_AUDIO_CODEC_G7221_14_32);
	nIndex=m_cbxAudioCodec.AddString(L"AUDEC 24Kbps");
	m_cbxAudioCodec.SetItemData(nIndex,X_AUDIO_CODEC_AUDEC_24);

	switch (m_AoutCameraSet->m_AudioCodecID)
	{
	case X_AUDIO_CODEC_G711U:
		nIndex=0;
		break;
	case X_AUDIO_CODEC_G711A:
		nIndex=1;
		break;
	case X_AUDIO_CODEC_G729:
		nIndex=2;
		break;
	case X_AUDIO_CODEC_G722_64:
		nIndex=3;
		break;
	case X_AUDIO_CODEC_G7221_14_24:
		nIndex=4;
		break;
	case X_AUDIO_CODEC_G7221_14_32:
		nIndex=5;
		break;
	case X_AUDIO_CODEC_AUDEC_24:
		nIndex=6;
		break;
// 	default:
// 		nIndex=0;
		break;
	}

	int naudiocodec = 0;
	naudiocodec = LoadWBCamerAudioSetting();
	m_cbxAudioCodec.SetCurSel(naudiocodec);
	m_AoutCameraSet->m_AudioCodecID=(X_AUDIO_CODEC_ID)m_cbxAudioCodec.GetItemData(naudiocodec);


	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgCameraAudioSetting::OnBnClickedOk()
{
	nAudioCodecID = m_cbxAudioCodec.GetCurSel();
	m_AoutCameraSet->m_AudioCodecID=(X_AUDIO_CODEC_ID)m_cbxAudioCodec.GetItemData(nAudioCodecID);
	SaveWBCamerAudioSetting();
	OnOK();
}

int CDlgCameraAudioSetting::LoadWBCamerAudioSetting()
{
// 	CString strCameraAudioFilePath = g_strAvconIniPath + WB_CARMERA_AUDIAO_FILENAME;
// 
// 	CMarkup xml;
// 	xml.Load(strCameraAudioFilePath);
// 	xml.ResetMainPos();
// 
// 	while(xml.FindChildElem("CameraAudioSetting"))
// 	{
// 		xml.IntoElem();
// 		xml.FindChildElem("AudioCodecID");
// 		nAudioCodecID = atoi(xml.GetChildData());
// 	}
// 
// 	xml.OutOfElem();
// 	xml.ResetPos();
	CString strTemp = _T("nAudioCodecID");
	CString strSetting = _T("CameraAudioSettingINI");
	CString str = _T("");
	str = ReadConfigTextVideoIndexIni(strSetting,strTemp);
	if (str.IsEmpty())
	{
		nAudioCodecID = 0;
	}
	else
	{
		nAudioCodecID = (X_AUDIO_CODEC_ID)_ttoi(str);
	}
	//nAudioCodecID = atoi(str);
	return nAudioCodecID;
}

void CDlgCameraAudioSetting::SaveWBCamerAudioSetting()
{
	//CString sPath = g_strAvconIniPath + WB_CARMERA_AUDIAO_FILENAME;
	//CMarkup xml;
	//首先确保文件夹存在
	if (!PathFileExists(g_strAvconIniPath))
	{
		CreateDirectory(g_strAvconIniPath, NULL);
	}
	//xml.AddElem("CameraSetting");
	//xml.AddChildElem("CameraAudioSetting");
	//xml.IntoElem();

	nAudioCodecID = m_cbxAudioCodec.GetCurSel();
	m_AoutCameraSet->m_VideoCodecType = (VIDEC_CODEC_TYPE)m_cbxAudioCodec.GetItemData(nAudioCodecID);
	if(m_UpdataAudioCodecType != nAudioCodecID)
	{
		m_bAudioSettingUpdated = TRUE ;
	}
	CString strTemp = _T("nAudioCodecID");
	CString strSetting = _T("CameraAudioSettingINI");
	CString strSettingNum = _T("CameraAudioSetting");
	WriteConfigTextVideoIndexIni(strSetting,strTemp,nAudioCodecID);
	WriteConfigTextVideoIni(strSettingNum,strTemp,m_AoutCameraSet->m_AudioCodecID);
// 	xml.AddChildElem("AudioCodecID", nAudioCodecID);
// 
// 	xml.Save(sPath);
}

void CDlgCameraAudioSetting::AudioSettingLanguage()
{
	CString staticText = _T("");
	staticText = ReadConfigTextAUDIO(_T("CameraVideoAudioSetting"),_T("CameraAudioEncode"));
	if (staticText.IsEmpty())
	{
		staticText = _T(" 音频编码:");
	}
	SetDlgItemText(IDC_STATIC_ENCODE_AUDIO,staticText);
	staticText = ReadConfigTextAUDIO(_T("CameraVideoAudioSetting"),_T("CameraVideoOK"));
	if (staticText.IsEmpty())
	{
		staticText = _T("确定");
	}
	SetDlgItemText(IDOK,staticText);

	staticText = ReadConfigTextAUDIO(_T("CameraVideoAudioSetting"),_T("CameraVideoCANCEL"));
	if (staticText.IsEmpty())
	{
		staticText = _T("取消");
	}
	SetDlgItemText(IDCANCEL,staticText);

}
