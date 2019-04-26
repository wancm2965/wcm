// DlgCameraVideoSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "DlgCameraVideoSetting.h"
#include "ResourceWBMedia.h"
#include ".\common\WBMedia\Markup.h"
#include "ResData.h"
#include "CameraSettingIni.h"
// CDlgCameraVideoSetting 对话框

// CString ReadConfigTextVideo( CString strSelection, LPCTSTR ControlName )
// {
// 	CString ControlText;
// 	char Text[256] = {0};
// 	GetPrivateProfileString(strSelection, ControlName, _T(""),Text,256,g_strAvconLanguageFile);
// 	ControlText = Text;
// 	return ControlText;
// }
// 
// CString ReadConfigTextVideoIni( CString strSelection, LPCTSTR ControlName )
// {
// 	CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;
// 	CString ControlText = _T("");
// 	char Text[256] = {0};
// 	GetPrivateProfileString(strSelection, ControlName,_T(""),Text, 256, strPath);
// 	ControlText = Text;
// 	return ControlText;
// }
// 
// void WriteConfigTextVideoIni( CString strSelection, LPCTSTR ControlName ,int iValue)
// {
// 	CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;
// 	CString strValue = _T("");
// 	strValue.Format(_T("%d"),iValue);
// 	WritePrivateProfileString(strSelection,ControlName,strValue, strPath);
// }

IMPLEMENT_DYNAMIC(CDlgCameraVideoSetting, CDialog)
	 
CDlgCameraVideoSetting::CDlgCameraVideoSetting(CWnd* pParent /*=NULL*/, CameraSet *pData)
	: CDialog(IDD_DIALOG_CAMERA_VIDEO_SETTING, pParent)
	,m_pVideoCameraSet (NULL)
	,m_bVideoSettingUpdated(FALSE)
	,m_DlgCamera(pParent)
{ 
	m_pVideoCameraSet = new CameraSet;
	if (pData)
	{
		m_pVideoCameraSet->m_nDevID= pData->m_nDevID;
		m_pVideoCameraSet->m_VideoCodecType=pData->m_VideoCodecType;
		m_pVideoCameraSet->m_nFrameRate=pData->m_nFrameRate;
		m_pVideoCameraSet->m_nBitrate=pData->m_nBitrate;
		m_pVideoCameraSet->m_nWidth=pData->m_nWidth;
		m_pVideoCameraSet->m_nHeight=pData->m_nHeight;
	}
}



CDlgCameraVideoSetting::~CDlgCameraVideoSetting()
{
	if (m_pVideoCameraSet)
	{
		delete m_pVideoCameraSet;
		m_pVideoCameraSet=NULL;
	}
	
}

void CDlgCameraVideoSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAMERA_VIDEO_DEV, m_cbxDev);
	DDX_Control(pDX, IDC_COMBO_CAMERA_VIDEO_FRAMERATE, m_cbxFrameRate);
	DDX_Control(pDX, IDC_COMBO_CAMERA_VIDEO_BITRATE, m_cbxBitrate);
	DDX_Control(pDX, IDC_COMBO_CAMERA_VIDEO_CODEC, m_cbxCodec);
	DDX_Control(pDX, IDC_COMBO_CAMERA_VIDEO_FORMAT, m_cbxFormat);
}

BEGIN_MESSAGE_MAP(CDlgCameraVideoSetting, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgCameraVideoSetting::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERA_VIDEO_DEV, &CDlgCameraVideoSetting::OnCbnSelchangeComboDev)
END_MESSAGE_MAP()


// CDlgCameraVideoSetting 消息处理程序

void CDlgCameraVideoSetting::OnBnClickedOk()
{
	CString sPath = g_strAvconIniPath + WB_CARMERA_VIDEO_FILENAME;
	//首先确保文件夹存在
	if (!PathFileExists(g_strAvconIniPath))
	{
		CreateDirectory(g_strAvconIniPath, NULL);
	}

	//CMarkup xml;
	//xml.AddElem("CameraSetting");
	//xml.AddChildElem("CameraVideoSetting");
	//xml.IntoElem();

	CString strSetting = _T("CameraVideoSetting");
	CString strSettingNum = _T("CameraVideoSettingNum");
	CString strTemp = _T("");
	strTemp = _T("VideoCodec");
	nVideoCodec = m_cbxCodec.GetCurSel();
	m_pVideoCameraSet->m_VideoCodecType = (VIDEC_CODEC_TYPE)m_cbxCodec.GetItemData(nVideoCodec);
 	if(m_UpdataVideoCodecType != nVideoCodec)
 	{
 		m_bVideoSettingUpdated = TRUE ;
 	}
	WriteConfigTextVideoIndexIni(strSetting,strTemp,nVideoCodec);
	WriteConfigTextVideoIni(strSettingNum,strTemp,m_pVideoCameraSet->m_VideoCodecType);
	//xml.AddChildElem("VideoCodec", nVideoCodec);

	strTemp = _T("VideoFrameRate");
	nViedoFrameRate = m_cbxFrameRate.GetCurSel();
	m_pVideoCameraSet->m_nFrameRate = (int)m_cbxFrameRate.GetItemData(nViedoFrameRate);
	if(nVideoBitrate != m_UpdataFrameRate)
	{
		m_bVideoSettingUpdated = TRUE ;
	}
	WriteConfigTextVideoIndexIni(strSetting,strTemp,nViedoFrameRate);
	WriteConfigTextVideoIni(strSettingNum,strTemp,m_pVideoCameraSet->m_nFrameRate);
	//xml.AddChildElem("VideoFrameRate", nViedoFrameRate);

	strTemp = _T("VideoBitrate");
	nVideoBitrate = m_cbxBitrate.GetCurSel();
	m_pVideoCameraSet->m_nBitrate = (int)m_cbxBitrate.GetItemData(nVideoBitrate);
	if(nVideoBitrate != m_UpdatamBitrate)
	{
		m_bVideoSettingUpdated = TRUE ;
	}
	WriteConfigTextVideoIndexIni(strSetting,strTemp,nVideoBitrate);
	WriteConfigTextVideoIni(strSettingNum,strTemp,m_pVideoCameraSet->m_nBitrate);
	//xml.AddChildElem("VideoBitrate", nVideoBitrate);

	strTemp = _T("VideoDevID");
	nVideoDevID = m_cbxDev.GetCurSel();
	m_pVideoCameraSet->m_nDevID = /*(int)m_cbxDev.GetItemData(nVideoDevID)*/nVideoDevID;
	WriteConfigTextVideoIndexIni(strSetting,strTemp,nVideoDevID);
	WriteConfigTextVideoIni(strSettingNum,strTemp,m_pVideoCameraSet->m_nDevID);
	//xml.AddChildElem("VideoDevID", nVideoDevID);

	strTemp = _T("VideoFormat");
	nVideoFormat = m_cbxFormat.GetCurSel();
	WriteConfigTextVideoIndexIni(strSetting,strTemp,nVideoFormat);
	WriteConfigTextVideoIni(strSettingNum,strTemp,0);
	//xml.AddChildElem("VideoFormat", nVideoFormat);

	//xml.Save(sPath);

	VIDEC_DEV_TYPE nDevType = VIDEC_DEV_SD;
	IMXEC_CapChanDev::GetDevType(m_pVideoCameraSet->m_nDevID, nDevType);
	IMXEC_CapChanDev::GetFormatSize(nDevType, nVideoFormat, m_pVideoCameraSet->m_nWidth, m_pVideoCameraSet->m_nHeight);

	CDlgCamera *pCamera = (CDlgCamera*)m_DlgCamera;
	if (pCamera && pCamera->GetMXECCapChanDev())
	{
		pCamera->OnVideoSetting();
	}

	OnOK(); 
	
}


void CDlgCameraVideoSetting::LoadCameraVideoSetting()
{
	CString strCameraVideoFilePath = g_strAvconIniPath + WB_CARMERA_VIDEO_FILENAME;
	CMarkup xml;
	xml.Load(strCameraVideoFilePath);

	while(xml.FindChildElem(L"CameraVideoSetting"))
	{
		xml.IntoElem();
		xml.FindChildElem(L"VideoCodec");
		nVideoCodec = _ttoi(xml.GetChildData());
		m_cbxCodec.SetCurSel(nVideoCodec);

		xml.FindChildElem(L"VideoFrameRate");
		nViedoFrameRate = _ttoi(xml.GetChildData());
		m_cbxFrameRate.SetCurSel(nViedoFrameRate);

		xml.FindChildElem(L"VideoBitrate");
		nVideoBitrate = _ttoi(xml.GetChildData());
		m_cbxBitrate.SetCurSel(nVideoBitrate);

		xml.FindChildElem(L"VideoDevID");
		nVideoDevID = _ttoi(xml.GetChildData());
		m_cbxDev.SetCurSel(nVideoDevID);

		xml.FindChildElem(L"VideoFormat");
		nVideoFormat = _ttoi(xml.GetChildData());
		m_cbxFormat.SetCurSel(nVideoFormat);
	}
	xml.OutOfElem();
	xml.ResetPos();

}
void CDlgCameraVideoSetting::GetCameraVideoSetting()
{
	if (!PathFileExists(g_strAvconIniPath))
	{
		return;
	}
	CString strSetting = _T("CameraVideoSetting");
	CString strTemp = _T("");
	CString strNumber = _T("");
	strTemp = _T("VideoCodec");
	strNumber = ReadConfigTextVideoIndexIni(strSetting,strTemp);
	if (strNumber.IsEmpty())
	{
		nVideoCodec = 0;
	}
	else
	{
		nVideoCodec = (VIDEC_CODEC_TYPE)_ttoi(strNumber);
	}
	//nVideoCodec = atoi(strNumber);
	m_cbxCodec.SetCurSel(nVideoCodec);
	m_pVideoCameraSet->m_VideoCodecType = (VIDEC_CODEC_TYPE)m_cbxCodec.GetItemData(nVideoCodec);

	strTemp = _T("VideoFrameRate");
	strNumber = ReadConfigTextVideoIndexIni(strSetting,strTemp);
	if (strNumber.IsEmpty())
	{
		nViedoFrameRate = 3;
	}
	else
	{
		nViedoFrameRate = (int)_ttoi(strNumber);
	}
	//nViedoFrameRate = atoi(strNumber);
	m_cbxFrameRate.SetCurSel(nViedoFrameRate);
	m_pVideoCameraSet->m_nFrameRate = (int)m_cbxFrameRate.GetItemData(nViedoFrameRate);

	strTemp = _T("VideoBitrate");
	strNumber = ReadConfigTextVideoIndexIni(strSetting,strTemp);
	if (strNumber.IsEmpty())
	{
		nVideoBitrate = 5;
	}
	else
	{
		nVideoBitrate = (int)_ttoi(strNumber);
	}
	//nVideoBitrate = atoi(strNumber);
	m_cbxBitrate.SetCurSel(nVideoBitrate);
	m_pVideoCameraSet->m_nBitrate = (int)m_cbxBitrate.GetItemData(nVideoBitrate);

	strTemp = _T("VideoDevID");
	strNumber = ReadConfigTextVideoIndexIni(strSetting,strTemp);
	if (strNumber.IsEmpty())
	{
		nVideoDevID = 0;
	}
	else
	{
		nVideoDevID = _ttoi(strNumber);
	}
	//nVideoDevID = atoi(strNumber);
	m_cbxDev.SetCurSel(nVideoDevID);
	m_pVideoCameraSet->m_nDevID = /*(int)m_cbxDev.GetItemData(nVideoDevID)*/nVideoDevID;

	strTemp = _T("VideoFormat");
	strNumber = ReadConfigTextVideoIndexIni(strSetting,strTemp);
	if (strNumber.IsEmpty())
	{
		nVideoFormat = 4;
	}
	else
	{
		nVideoDevID = _ttoi(strNumber);
	}
	//nVideoFormat = atoi(strNumber);
	m_cbxFormat.SetCurSel(nVideoFormat);
}

BOOL CDlgCameraVideoSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	VideoSettingLanguage();//主要是语言版本wangxiaobin
	if (::IsWindow(m_hWnd))
	{
		::SetWindowPos(m_hWnd,HWND_TOPMOST,20,20,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		CWnd *pwnd = FromHandle(m_hWnd);
		pwnd->CenterWindow();
	}

	int nIndex=0;

	nIndex=m_cbxCodec.AddString(L"H264 HP");
	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H264_SVC);
	nIndex=m_cbxCodec.AddString(L"H263+");
	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H263P);
// 	nIndex=m_cbxCodec.AddString("H263++");
// 	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H263PP);
	nIndex=m_cbxCodec.AddString(L"H264");
	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H264);
// 	nIndex=m_cbxCodec.AddString("HD264");
// 	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_DH264);


	nIndex=m_cbxFrameRate.AddString(L"1");
	m_cbxFrameRate.SetItemData(nIndex,1);
	nIndex=m_cbxFrameRate.AddString(L"5");
	m_cbxFrameRate.SetItemData(nIndex,5);
	nIndex=m_cbxFrameRate.AddString(L"10");
	m_cbxFrameRate.SetItemData(nIndex,10);
	nIndex=m_cbxFrameRate.AddString(L"15");
	m_cbxFrameRate.SetItemData(nIndex,15);
	nIndex=m_cbxFrameRate.AddString(L"30");
	m_cbxFrameRate.SetItemData(nIndex,30);
	nIndex=m_cbxFrameRate.AddString(L"60");
	m_cbxFrameRate.SetItemData(nIndex,60);

	if (m_pVideoCameraSet->m_nFrameRate<=1)
		m_pVideoCameraSet->m_nFrameRate=1;
	else if (m_pVideoCameraSet->m_nFrameRate<=5)
		m_pVideoCameraSet->m_nFrameRate=5;
	else if (m_pVideoCameraSet->m_nFrameRate<=10)
		m_pVideoCameraSet->m_nFrameRate=10;
	else if (m_pVideoCameraSet->m_nFrameRate<=15)
		m_pVideoCameraSet->m_nFrameRate=15;
	else if (m_pVideoCameraSet->m_nFrameRate<=30)
		m_pVideoCameraSet->m_nFrameRate=30;
	else 
		m_pVideoCameraSet->m_nFrameRate=60;

	char szDevName[128]="";
	int nDevCount=IMXEC_CapChanDev::GetDevCount();
	for (int i=0;i<nDevCount;i++)
	{
		if (IMXEC_CapChanDev::GetDevName(i,szDevName,127)<0)
		{
			break;
		}
		m_cbxDev.AddString(ToDRAWStr(szDevName).c_str());
	}
	/*nVideoDevID = atoi(szDevName);
	m_cbxDev.SetCurSel(nVideoDevID);*/

	nIndex=m_cbxBitrate.AddString(L"32");
	m_cbxBitrate.SetItemData(nIndex,32);
	nIndex=m_cbxBitrate.AddString(L"64");
	m_cbxBitrate.SetItemData(nIndex,64);
	nIndex=m_cbxBitrate.AddString(L"128");
	m_cbxBitrate.SetItemData(nIndex,128);
	nIndex=m_cbxBitrate.AddString(L"256");
	m_cbxBitrate.SetItemData(nIndex,256);
	nIndex=m_cbxBitrate.AddString(L"384");
	m_cbxBitrate.SetItemData(nIndex,384);
	nIndex=m_cbxBitrate.AddString(L"512");
	m_cbxBitrate.SetItemData(nIndex,512);
	nIndex=m_cbxBitrate.AddString(L"768");
	m_cbxBitrate.SetItemData(nIndex,768);
	nIndex=m_cbxBitrate.AddString(L"1024");
	m_cbxBitrate.SetItemData(nIndex,1024);
	nIndex=m_cbxBitrate.AddString(L"1536");
	m_cbxBitrate.SetItemData(nIndex,1536);
	nIndex=m_cbxBitrate.AddString(L"2048");
	m_cbxBitrate.SetItemData(nIndex,2048);
	nIndex=m_cbxBitrate.AddString(L"3072");
	m_cbxBitrate.SetItemData(nIndex,3072);
	nIndex=m_cbxBitrate.AddString(L"4096");
	m_cbxBitrate.SetItemData(nIndex,4096);
	nIndex=m_cbxBitrate.AddString(L"6144");
	m_cbxBitrate.SetItemData(nIndex,6144);
	if (m_pVideoCameraSet->m_nBitrate<=32)
		m_pVideoCameraSet->m_nBitrate=32;
	else if (m_pVideoCameraSet->m_nBitrate<=64)
		m_pVideoCameraSet->m_nBitrate=64;
	else if (m_pVideoCameraSet->m_nBitrate<=128)
		m_pVideoCameraSet->m_nBitrate=128;
	else if (m_pVideoCameraSet->m_nBitrate<=256)
		m_pVideoCameraSet->m_nBitrate=256;
	else if (m_pVideoCameraSet->m_nBitrate<=384)
		m_pVideoCameraSet->m_nBitrate=384;
	else if (m_pVideoCameraSet->m_nBitrate<=512)
		m_pVideoCameraSet->m_nBitrate=512;
	else if (m_pVideoCameraSet->m_nBitrate<=768)
		m_pVideoCameraSet->m_nBitrate=768;
	else if (m_pVideoCameraSet->m_nBitrate<=1024)
		m_pVideoCameraSet->m_nBitrate=1024;
	else if (m_pVideoCameraSet->m_nBitrate<=1536)
		m_pVideoCameraSet->m_nBitrate=1536;
	else if (m_pVideoCameraSet->m_nBitrate<=2048)
		m_pVideoCameraSet->m_nBitrate=2048;
	else if (m_pVideoCameraSet->m_nBitrate<=3072)
		m_pVideoCameraSet->m_nBitrate=3072;
	else if (m_pVideoCameraSet->m_nBitrate<=4096)
		m_pVideoCameraSet->m_nBitrate=4096;
	else if (m_pVideoCameraSet->m_nBitrate<=1536)
		m_pVideoCameraSet->m_nBitrate=1536;
	else 
		m_pVideoCameraSet->m_nBitrate=6144;

	FillFormat();
	//LoadCameraVideoSetting();
	GetCameraVideoSetting();
	

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDlgCameraVideoSetting::FillFormat(void)
{
	m_cbxFormat.ResetContent();
	m_pVideoCameraSet->m_nDevID=/*m_cbxDev.GetCurSel();*/nVideoDevID;

	VIDEC_DEV_TYPE nDevType = VIDEC_DEV_SD;
	IMXEC_CapChanDev::GetDevType(m_pVideoCameraSet->m_nDevID, nDevType);
	
	char szFormatName[128]="";

	int nFormatCount=IMXEC_CapChanDev::GetFormatCount(nDevType);
	int i=0;
	for (i=0;i<nFormatCount;++i)
	{
		if (IMXEC_CapChanDev::GetFormatName(nDevType,i,szFormatName, 127)<0)
		{
			break;
		}
		m_cbxFormat.AddString(ToDRAWStr(szFormatName).c_str());
	}

	int nFormatIndex=0;
	for (i=0;i<nFormatCount;++i)
	{
		int nWidth=0;
		int nHeight=0;
		if (IMXEC_CapChanDev::GetFormatSize(nDevType,i,nWidth,nHeight)<0)
		{
			break;
		}
		
		if (nWidth>m_pVideoCameraSet->m_nWidth || nHeight>m_pVideoCameraSet->m_nHeight)
		{
			break;
		}
		nFormatIndex=i;
	}
	m_cbxFormat.SetCurSel(nFormatIndex);
}

void CDlgCameraVideoSetting::OnCbnSelchangeComboDev()
{
	m_bVideoSettingUpdated = TRUE;
	FillFormat();
}

void CDlgCameraVideoSetting::VideoSettingLanguage()
{
	CString staticText = _T("");
	staticText = ReadConfigTextVideo(_T("CameraVideoAudioSetting"),_T("CameraVideoSetting"));
	if (staticText.IsEmpty())
	{
		staticText = _T("主流视频设置");
	}
	SetDlgItemText(IDC_STATIC_CAPTION_SETTING,staticText);

	staticText = ReadConfigTextVideo(_T("CameraVideoAudioSetting"),_T("CameraVideoDevice"));
	if (staticText.IsEmpty())
	{
		staticText = _T("视频设备：");
	}
	SetDlgItemText(IDC_STATIC_DEVICE,staticText);

	staticText = ReadConfigTextVideo(_T("CameraVideoAudioSetting"),_T("CameraVideoFrame"));
	if (staticText.IsEmpty())
	{
		staticText = _T("视频帧率：");
	}
	SetDlgItemText(IDC_STATIC_FRAME,staticText);

	staticText = ReadConfigTextVideo(_T("CameraVideoAudioSetting"),_T("CameraVideoStrema"));
	if (staticText.IsEmpty())
	{
		staticText = _T("视频码流：");
	}
	SetDlgItemText(IDC_STATIC_STREAM,staticText);

	staticText = ReadConfigTextVideo(_T("CameraVideoAudioSetting"),_T("CameraVideoEncode"));
	if (staticText.IsEmpty())
	{
		staticText = _T("视频编码：");
	}
	SetDlgItemText(IDC_STATIC_ENCODE_VEDIO,staticText);

	staticText = ReadConfigTextVideo(_T("CameraVideoAudioSetting"),_T("CameraVideoFormat"));
	if (staticText.IsEmpty())
	{
		staticText = _T("视频格式：");
	}
	SetDlgItemText(IDC_STATIC_FORMAT,staticText);

	staticText = ReadConfigTextVideo(_T("CameraVideoAudioSetting"),_T("CameraVideoOK"));
	if (staticText.IsEmpty())
	{
		staticText = _T("确定");
	}
	SetDlgItemText(IDOK,staticText);
	
	staticText = ReadConfigTextVideo(_T("CameraVideoAudioSetting"),_T("CameraVideoCANCEL"));
	if (staticText.IsEmpty())
	{
		staticText = _T("取消");
	}
	SetDlgItemText(IDCANCEL,staticText);
	    
	
	//IDC_STATIC_ENCODE   音频编码：
}




// void CDlgCameraVideoSetting::LoadWBResourceSetting()
// {
// 	CString sConfigFilePath = g_strAvconIniPath + WB_RESOURCE_FILENAME;
// 	if (PathFileExists(sConfigFilePath))
// 	{
// 		CMarkup xml;
// 		if (!xml.Load(sConfigFilePath))
// 		{
// 			return;
// 		}
// //		ClearResData();
// 		xml.SetFilePath(sConfigFilePath);
// 		xml.ResetChildPos();
// 		UINT nIndex = 0;
// 		bool bFound = false;
// 		CString sMapName;
// 		CString sMapPath;
// 
// 		if(xml.FindElem(WB_RESOURCE_XML_HEAD))
// 		{
// 			xml.IntoElem();
// 			//获取资源
// 			while (xml.FindElem(WB_RESOURCE_ELEMENT_NAME))
// 			{
// 				//xml.IntoElem();
// 				sMapName = xml.GetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_1);
// 				sMapPath = xml.GetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_2);
// 				//判断路径是否正确(可能还会在此做更多的安全性检查)
// 				if (PathFileExists(sMapPath))
// 				{
// 					PSResourceMap pRes = new SResourceMap;
// 					pRes->SetMapName(sMapName);
// 					pRes->SetMapPath(sMapPath);
// 					m_resMap.push_back(pRes);
// 				}
// 			}
// 			//获取资源索引值
// 			m_nIndexOfCurUsingResource = 0;
// 			if (xml.FindChildElem(WB_RESOURCE_ELEMENT_LAST_USING_ITEM))
// 			{
// 				m_nIndexOfCurUsingResource = atoi(xml.GetChildData());
// 			}
// 			//设置
// 			SetCurResourceIndex(m_nIndexOfCurUsingResource);
// 			xml.OutOfElem();
// 		}
// 		else
// 		{
// 			OutputDebugString(_T("Initial failed"));
// 		}
// 	}
// }

// BOOL CDlgCameraVideoSetting::SaveWBResourceSetting()
// {
// 		BOOL bSaveOK = TRUE;
// 	CString sPath = g_strAvconIniPath + WB_RESOURCE_FILENAME;
// 	//首先确保文件夹存在
// 	if (!PathFileExists(g_strAvconIniPath))
// 	{
// 		bSaveOK = CreateDirectory(g_strAvconIniPath, NULL);
// 	}
// 	if(!bSaveOK)
// 	{
// 		return bSaveOK;//文件路径创建失败
// 	}
// 
// 	CMarkup xml;
// 	xml.AddElem(WB_RESOURCE_XML_HEAD);
// 	xml.IntoElem();
// 	/*-----------------------------保存示例----------------------------
// 	//xml.AddElem(WB_RESOURCE_ELEMENT_NAME);
// 	//xml.SetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_1, _T("图片"));
// 	//xml.SetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_2, _T("d:\\wbtemp"));
// 	//xml.AddElem(WB_RESOURCE_ELEMENT_NAME);
// 	//xml.SetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_1, _T("图片2"));
// 	//xml.SetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_2, _T("d:\\wbtemp23"));
// 	------------------------------------------------------------------*/
// 	SResourceMap * pRes = NULL;
// 	ResMapArrayIter it = m_resMap.begin();
// 	for (; it != m_resMap.end(); it++)
// 	{
// 		xml.AddElem(WB_RESOURCE_ELEMENT_NAME);
// 		xml.SetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_1, (*it)->GetMapName());
// 		xml.SetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_2, (*it)->GetMapPath());
// 	}
// 	//写入索引号
// 	xml.AddChildElem(WB_RESOURCE_ELEMENT_LAST_USING_ITEM, m_nIndexOfCurUsingResource, 0);
// 	xml.OutOfElem();
// 	sPath = GetCurrentPath() + WB_RESOURCE_FILENAME;
// 	bSaveOK = xml.Save(sPath);
// 	return bSaveOK;
// }
