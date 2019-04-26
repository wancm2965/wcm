// DlgSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "TestAH2K.h"
#include "DlgSetting.h"
#include "DlgMain.h"


// DlgSetting 对话框

IMPLEMENT_DYNAMIC(DlgSetting, CDialog)
DlgSetting::DlgSetting(DlgMain&rDlgMain)
	: CDialog(DlgSetting::IDD, &rDlgMain)
	, m_rDlgMain(rDlgMain)
	, m_bEncode(FALSE)
	, m_bDecode(FALSE)
{
}

DlgSetting::~DlgSetting()
{
}

void DlgSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEV, m_cbxDev);
	DDX_Control(pDX, IDC_COMBO_FRAMERATE, m_cbxFrameRate);
	DDX_Control(pDX, IDC_COMBO_CODEC, m_cbxCodec);
	DDX_Control(pDX, IDC_COMBO_QUALITY, m_cbxQuality);
	DDX_Control(pDX, IDC_COMBO_FORMAT, m_cbxFormat);
	DDX_Check(pDX, IDC_CHECK_ENCODE, m_bEncode);
	DDX_Check(pDX, IDC_CHECK_DECODE, m_bDecode);
}


BEGIN_MESSAGE_MAP(DlgSetting, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_DEV, OnCbnSelchangeComboDev)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// DlgSetting 消息处理程序

BOOL DlgSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	int nIndex=0;
	//取设备个数
#ifdef TEST_AH2K
	{
		int nDevCount= VIDEC_AH2KDev::GetDevCount();
		for (int i=0;i<nDevCount;i++)
		{
			//取设备名称
			char szName[128]="";
			VIDEC_AH2KDev::GetDevName(i,szName,127);
			m_cbxDev.AddString(szName);
		}
	}
#endif
#ifdef TEST_DVC
	{
		int nDevCount= VIDEC_DVCDev::GetDevCount();
		for (int i=0;i<nDevCount;i++)
		{
			//取设备名称
			char szName[128]="";
			VIDEC_DVCDev::GetDevName(i,szName,127);
			m_cbxDev.AddString(szName);
		}
	}
#endif
#ifdef TEST_AH410
	{
		int nDevCount= VIDEC_AH410Dev::GetDevCount();
		for (int i=0;i<nDevCount;i++)
		{
			//取设备名称
			char szName[128]="";
			VIDEC_AH410Dev::GetDevName(i,szName,127);
			m_cbxDev.AddString(szName);
		}
	}
#endif
#ifdef TEST_CAMERA
	{
		int nDevCount= VIDEC_CameraDev::GetDevCount();
		for (int i=0;i<nDevCount;i++)
		{
			//取设备名称
			char szName[128]="";
			VIDEC_CameraDev::GetDevName(i,szName,127);
			m_cbxDev.AddString(szName);
		}
	}
#endif
#ifdef TEST_AH210
	{
		int nDevCount= VIDEC_AH210Dev::GetDevCount();
		for (int i=0;i<nDevCount;i++)
		{
			//取设备名称
			char szName[128]="";
			VIDEC_AH210Dev::GetDevName(i,szName,127);
			m_cbxDev.AddString(szName);
		}
	}
#endif
#ifdef TEST_AH400
	{
		int nDevCount= VIDEC_AH400Dev::GetDevCount();
		for (int i=0;i<nDevCount;i++)
		{
			//取设备名称
			char szName[128]="";
			VIDEC_AH400Dev::GetDevName(i,szName,127);
			m_cbxDev.AddString(szName);
		}
	}
#endif
	m_cbxDev.SetCurSel(m_rDlgMain.m_nDevID);

	nIndex=m_cbxFrameRate.AddString("1");
	m_cbxFrameRate.SetItemData(nIndex,1);
	nIndex=m_cbxFrameRate.AddString("5");
	m_cbxFrameRate.SetItemData(nIndex,5);
	nIndex=m_cbxFrameRate.AddString("10");
	m_cbxFrameRate.SetItemData(nIndex,10);
	nIndex=m_cbxFrameRate.AddString("15");
	m_cbxFrameRate.SetItemData(nIndex,15);
	nIndex=m_cbxFrameRate.AddString("30");
	m_cbxFrameRate.SetItemData(nIndex,30);
	nIndex=m_cbxFrameRate.AddString("60");
	m_cbxFrameRate.SetItemData(nIndex,60);
	if (m_rDlgMain.m_nFrameRate<=1)
	{
		nIndex=0;
	}
	else if (m_rDlgMain.m_nFrameRate<=5)
	{
		nIndex=1;
	}
	else if (m_rDlgMain.m_nFrameRate<=10)
	{
		nIndex=2;
	}
	else if (m_rDlgMain.m_nFrameRate<=15)
	{
		nIndex=3;
	}
	else if (m_rDlgMain.m_nFrameRate<=30)
	{
		nIndex=4;
	}
	else 
	{
		nIndex=5;
	}
	m_cbxFrameRate.SetCurSel(nIndex);


	//nIndex=m_cbxCodec.AddString("H263");
	//m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H263);
	nIndex=m_cbxCodec.AddString("H263+");
	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H263P);
	nIndex=m_cbxCodec.AddString("H263++");
	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H263PP);
	nIndex=m_cbxCodec.AddString("H264");
	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H264);
	nIndex=m_cbxCodec.AddString("H264 SVC");
	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H264_SVC);
	switch (m_rDlgMain.m_CodecType)
	{
	case VIDEC_CODEC_H263P:
		m_cbxCodec.SetCurSel(0);
		break;
	case VIDEC_CODEC_H263PP:
		m_cbxCodec.SetCurSel(1);
		break;
	case VIDEC_CODEC_H264:
		m_cbxCodec.SetCurSel(2);
		break;
	case VIDEC_CODEC_H264_SVC:
		m_cbxCodec.SetCurSel(3);
		break;
	default:
		m_cbxCodec.SetCurSel(3);
		break;
	}

	FillQuality();
	FillFormat();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void DlgSetting::FillQuality(void)
{
	m_cbxQuality.ResetContent();

	for (int i=0;i<GetQualityCount();i++)
	{
		char szName[128];
		GetNameByQuality(i,szName,127);
		m_cbxQuality.AddString(szName);
	}

	m_cbxQuality.SetCurSel(m_rDlgMain.m_nQuality);
}

void DlgSetting::FillFormat(void)
{
	int nCurSel=m_rDlgMain.m_nFormat;
	m_cbxFormat.ResetContent();

	int nCurDevID=m_cbxDev.GetCurSel();
#ifdef TEST_AH2K
	int nDevCount= VIDEC_AH2KDev::GetDevCount();
	if (nCurDevID<nDevCount)
	{
		int nFormatCount= VIDEC_AH2KDev::GetFormatCount();
		if (nCurSel<0 || nCurSel>=nFormatCount)
			nCurSel=0;
		for (int i=0;i<nFormatCount;i++)
		{
			//取名称
			char szName[128]="";
			VIDEC_AH2KDev::GetFormatName(i,szName,127);
			m_cbxFormat.AddString(szName);
		}
	}
#endif
#ifdef TEST_DVC
	int nDevCount= VIDEC_DVCDev::GetDevCount();
	if (nCurDevID<nDevCount)
	{
		int nFormatCount= VIDEC_DVCDev::GetFormatCount();
		if (nCurSel<0 || nCurSel>=nFormatCount)
			nCurSel=0;
		for (int i=0;i<nFormatCount;i++)
		{
			//取名称
			char szName[128]="";
			VIDEC_DVCDev::GetFormatName(i,szName,127);
			m_cbxFormat.AddString(szName);
		}
	}
#endif
#ifdef TEST_CAMERA
	int nDevCount= VIDEC_CameraDev::GetDevCount();
	if (nCurDevID<nDevCount)
	{
		int nFormatCount= VIDEC_CameraDev::GetFormatCount();
		if (nCurSel<0 || nCurSel>=nFormatCount)
			nCurSel=0;
		for (int i=0;i<nFormatCount;i++)
		{
			//取名称
			char szName[128]="";
			VIDEC_CameraDev::GetFormatName(i,szName,127);
			m_cbxFormat.AddString(szName);
		}
	}
#endif
#ifdef TEST_AH410
	int nDevCount= VIDEC_AH410Dev::GetDevCount();
	if (nCurDevID<nDevCount)
	{
		int nFormatCount= VIDEC_AH410Dev::GetFormatCount();
		if (nCurSel<0 || nCurSel>=nFormatCount)
			nCurSel=0;
		for (int i=0;i<nFormatCount;i++)
		{
			//取名称
			char szName[128]="";
			VIDEC_AH410Dev::GetFormatName(i,szName,127);
			m_cbxFormat.AddString(szName);
		}
	}
#endif
#ifdef TEST_AH210
	int nDevCount= VIDEC_AH210Dev::GetDevCount();
	if (nCurDevID<nDevCount)
	{
		int nFormatCount= VIDEC_AH210Dev::GetFormatCount();
		if (nCurSel<0 || nCurSel>=nFormatCount)
			nCurSel=0;
		for (int i=0;i<nFormatCount;i++)
		{
			//取名称
			char szName[128]="";
			VIDEC_AH210Dev::GetFormatName(i,szName,127);
			m_cbxFormat.AddString(szName);
		}
	}
#endif
#ifdef TEST_AH400
	int nDevCount= VIDEC_AH400Dev::GetDevCount();
	if (nCurDevID<nDevCount)
	{
		int nFormatCount= VIDEC_AH400Dev::GetFormatCount();
		if (nCurSel<0 || nCurSel>=nFormatCount)
			nCurSel=0;
		for (int i=0;i<nFormatCount;i++)
		{
			//取名称
			char szName[128]="";
			VIDEC_AH400Dev::GetFormatName(i,szName,127);
			m_cbxFormat.AddString(szName);
		}
	}
#endif

	m_cbxFormat.SetCurSel(nCurSel);
}


void DlgSetting::OnCbnSelchangeComboDev()
{
	FillFormat();
}

void DlgSetting::OnBnClickedOk()
{
	if (!UpdateData())
		return;

	m_rDlgMain.m_nDevID=m_cbxDev.GetCurSel();

	int nIndex=m_cbxFrameRate.GetCurSel();
	m_rDlgMain.m_nFrameRate=m_cbxFrameRate.GetItemData(nIndex);

	m_rDlgMain.m_nQuality=m_cbxQuality.GetCurSel();
	m_rDlgMain.m_nFormat=m_cbxFormat.GetCurSel();
	nIndex=m_cbxCodec.GetCurSel();
	m_rDlgMain.m_CodecType=(VIDEC_CODEC_TYPE)m_cbxCodec.GetItemData(nIndex);
	m_rDlgMain.m_bEncode=m_bEncode;
	m_rDlgMain.m_bDecode=m_bDecode;

	OnOK();
}
