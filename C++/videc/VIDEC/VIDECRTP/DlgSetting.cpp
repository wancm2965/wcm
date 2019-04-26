// DlgSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "VIDECRTP.h"
#include "DlgSetting.h"
#include "DlgMain.h"


// DlgSetting 对话框

IMPLEMENT_DYNAMIC(DlgSetting, CDialog)
DlgSetting::DlgSetting(DlgMain&rDlgMain)
	: CDialog(DlgSetting::IDD, &rDlgMain)
	, m_rDlgMain(rDlgMain)
	, m_nLocalPort(8100)
	, m_nRemotePort(8100)
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
	DDX_Control(pDX, IDC_EDIT_LOCAL_PORT, m_editLocalPort);
	DDX_Control(pDX, IDC_EDIT_REMOTE_PORT, m_editRemotePort);
	DDX_Text(pDX, IDC_EDIT_LOCAL_PORT, m_nLocalPort);
	DDV_MinMaxUInt(pDX, m_nLocalPort, 2000, 50000);
	DDX_Text(pDX, IDC_EDIT_REMOTE_PORT, m_nRemotePort);
	DDV_MinMaxUInt(pDX, m_nRemotePort, 2000, 50000);
	DDX_Control(pDX, IDC_REMOTE_IP, m_ctrlRemoteIP);
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
	{
		int nDevCount= VIDEC_CameraDev::GetDevCount();
		for (int i=0;i<nDevCount;i++)
		{
			//取设备名称
			char szName[128]="";
			VIDEC_CameraDev::GetDevName(i,szName,127);
			//TRACE("DevIndex=%d ,DevName=%s\n",i,szName);
			m_cbxDev.AddString(szName);
		}
	}
	m_cbxDev.SetCurSel(m_rDlgMain.m_nDevID);

	nIndex=m_cbxFrameRate.AddString("5");
	m_cbxFrameRate.SetItemData(nIndex,5);
	nIndex=m_cbxFrameRate.AddString("10");
	m_cbxFrameRate.SetItemData(nIndex,10);
	nIndex=m_cbxFrameRate.AddString("15");
	m_cbxFrameRate.SetItemData(nIndex,15);
	nIndex=m_cbxFrameRate.AddString("30");
	m_cbxFrameRate.SetItemData(nIndex,30);
	if (m_rDlgMain.m_nFrameRate<=5)
		nIndex=0;
	else if (m_rDlgMain.m_nFrameRate<=10)
		nIndex=1;
	else if (m_rDlgMain.m_nFrameRate<=15)
		nIndex=2;
	else
		nIndex=3;

	m_cbxFrameRate.SetCurSel(nIndex);


	nIndex=m_cbxCodec.AddString("H261");
	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H261);
	nIndex=m_cbxCodec.AddString("H263");
	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H263);
	nIndex=m_cbxCodec.AddString("H263+");
	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H263P);
	nIndex=m_cbxCodec.AddString("H263++");
	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H263PP);
	nIndex=m_cbxCodec.AddString("H264");
	m_cbxCodec.SetItemData(nIndex,VIDEC_CODEC_H264);
	m_cbxCodec.SetCurSel(m_rDlgMain.m_CodecType);

	FillQuality();
	FillFormat();

	m_ctrlRemoteIP.SetWindowText(m_rDlgMain.m_strRemoteIP);

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
	int nDevCount= VIDEC_CameraDev::GetDevCount();
	if (nCurDevID<nDevCount)
	{
		int nFormatCount= VIDEC_CameraDev::GetFormatCount();
		if (nCurSel<0 || nCurSel>=nFormatCount)
			nCurSel=2;
		for (int i=0;i<nFormatCount;i++)
		{
			//取名称
			char szName[128]="";
			VIDEC_CameraDev::GetFormatName(i,szName,127);
			m_cbxFormat.AddString(szName);
		}
	}


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

	CString strRemoteIP;
	m_ctrlRemoteIP.GetWindowText(strRemoteIP);

	m_rDlgMain.m_nDevID=m_cbxDev.GetCurSel();

	int nIndex=m_cbxFrameRate.GetCurSel();
	m_rDlgMain.m_nFrameRate=m_cbxFrameRate.GetItemData(nIndex);

	m_rDlgMain.m_nQuality=m_cbxQuality.GetCurSel();
	m_rDlgMain.m_nFormat=m_cbxFormat.GetCurSel();
	m_rDlgMain.m_CodecType=(VIDEC_CODEC_TYPE)m_cbxCodec.GetItemData(m_cbxCodec.GetCurSel());

	m_rDlgMain.m_nLocalPort=m_nLocalPort;
	m_rDlgMain.m_nRemotePort=m_nRemotePort;
	m_rDlgMain.m_strRemoteIP=strRemoteIP;


	OnOK();
}
