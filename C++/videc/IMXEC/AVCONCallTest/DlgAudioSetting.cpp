// DlgAudioSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "AVCONCallTest.h"
#include "DlgAudioSetting.h"


// DlgAudioSetting 对话框

IMPLEMENT_DYNAMIC(DlgAudioSetting, CDialog)

DlgAudioSetting::DlgAudioSetting(CWnd* pParent /*=NULL*/)
	: CDialog(DlgAudioSetting::IDD, pParent)
{
	m_nMicDevID=-1;
	m_nSpkDevID=-1;
	m_nAEC=1;
	m_nVAD=1;
	m_nAGC=1;
	m_nNS=1;
}

DlgAudioSetting::~DlgAudioSetting()
{
}

void DlgAudioSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MIC, m_cbxMic);
	DDX_Control(pDX, IDC_COMBO_SPK, m_cbxSpk);
	DDX_Control(pDX, IDC_CHECK_AEC, m_btnAEC);
	DDX_Control(pDX, IDC_CHECK_VAD, m_btnVAD);
	DDX_Control(pDX, IDC_CHECK_AGC, m_btnAGC);
	DDX_Control(pDX, IDC_CHECK_NS, m_btnNS);
}


BEGIN_MESSAGE_MAP(DlgAudioSetting, CDialog)
	ON_BN_CLICKED(IDOK, &DlgAudioSetting::OnBnClickedOk)
END_MESSAGE_MAP()


// DlgAudioSetting 消息处理程序

BOOL DlgAudioSetting::OnInitDialog()
{
	CDialog::OnInitDialog();


	{
		int nDevID=m_nMicDevID+1;
		char szDevName[128]="";
		int nDevCount=IMXEC_AVCONCall::GetMicDevCount();
			
		m_cbxMic.AddString("默认设备");

		for (int i=0;i<nDevCount;i++)
		{
			if (IMXEC_AVCONCall::GetMicDevName(i,szDevName,127)<0)
			{
				break;
			}
			m_cbxMic.AddString(szDevName);
		}
		if (nDevID<0 || nDevID>=nDevCount)
		{
			nDevID=0;
		}		

		m_cbxMic.SetCurSel(nDevID);
	}

	{
		int nDevID=m_nSpkDevID+1;
		char szDevName[128]="";
		int nDevCount=IMXEC_AVCONCall::GetSpkDevCount();
			
		m_cbxSpk.AddString("默认设备");

		for (int i=0;i<nDevCount;i++)
		{
			if (IMXEC_AVCONCall::GetSpkDevName(i,szDevName,127)<0)
			{
				break;
			}
			m_cbxSpk.AddString(szDevName);
		}
		if (nDevID<0 || nDevID>=nDevCount)
		{
			nDevID=0;
		}		

		m_cbxSpk.SetCurSel(nDevID);
	}

	m_btnAEC.SetCheck(m_nAEC);
	m_btnVAD.SetCheck(m_nVAD);
	m_btnAGC.SetCheck(m_nAGC);
	m_btnNS.SetCheck(m_nNS);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void DlgAudioSetting::OnBnClickedOk()
{
	m_nMicDevID=m_cbxMic.GetCurSel()-1;
	m_nSpkDevID=m_cbxSpk.GetCurSel()-1;
	m_nAEC=m_btnAEC.GetCheck();
	m_nVAD=m_btnVAD.GetCheck();
	m_nAGC=m_btnAGC.GetCheck();
	m_nNS=m_btnNS.GetCheck();

	OnOK();
}
