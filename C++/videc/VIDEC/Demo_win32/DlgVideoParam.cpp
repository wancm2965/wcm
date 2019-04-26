// DlgVideoParam.cpp : implementation file
//

#include "stdafx.h"
#include "DlgVideoParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgVideoParam dialog



CDlgVideoParam::CDlgVideoParam(VideoParamNotify& rNotify,Video_Param param, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVideoParam::IDD, pParent)
	, m_rNotify(rNotify)
	, m_Video_Param(param)
{
}


void CDlgVideoParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgVideoParam)
	DDX_Control(pDX, IDC_EDIT_BRIGHTNESS,	m_edtBrightness);
	DDX_Control(pDX, IDC_SLIDER_BRIGHTNESS,	m_sliderBrightness);

	DDX_Control(pDX, IDC_EDIT_CONTRAST,		m_edtContrast);
	DDX_Control(pDX, IDC_SLIDER_CONTRAST,	m_sliderContrast);

	DDX_Control(pDX, IDC_EDIT_SATURATION,	m_edtSaturation);
	DDX_Control(pDX, IDC_SLIDER_SATURATION,	m_sliderSaturation);

	DDX_Control(pDX, IDC_EDIT_HUE,			m_edtHue);
	DDX_Control(pDX, IDC_SLIDER_HUE,		m_sliderHue);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SLIDER_DETAIL,		m_sliderDetail);
	DDX_Control(pDX, IDC_EDIT_DETAIL,		m_edtDetail);

	DDX_Control(pDX, IDC_SLIDER_DENOISE,	m_sliderDenoise);
	DDX_Control(pDX, IDC_EDIT_DENOISE,		m_edtDenoise);

}


BEGIN_MESSAGE_MAP(CDlgVideoParam, CDialog)
	//{{AFX_MSG_MAP(CDlgVideoParam)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_DEFAULT, &CDlgVideoParam::OnBnClickedDefault)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgVideoParam message handlers


BOOL CDlgVideoParam::OnInitDialog()
{
	CDialog::OnInitDialog();


	InitParam(m_Video_Param);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgVideoParam::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData(TRUE);

	Video_Param param;

	param.iBrightness = m_sliderBrightness.GetPos();
	param.iContrast   = m_sliderContrast.GetPos();
	param.iSaturation = m_sliderSaturation.GetPos();
	param.iHue        = m_sliderHue.GetPos();
	param.iDetail      = m_sliderDetail.GetPos();
	param.iDenoise    = m_sliderDenoise.GetPos();

	FillEditText();

	m_rNotify.VideoParamNotifyNewParam(param);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgVideoParam::FillEditText()
{
	if (!GetDlgItem(IDC_EDIT_BRIGHTNESS)->GetSafeHwnd())
	{
		return;
	}

	CString strEditText;
	strEditText.Format(_T("%d"),m_sliderBrightness.GetPos());
	GetDlgItem(IDC_EDIT_BRIGHTNESS)->SetWindowText(strEditText);
	strEditText.Format(_T("%d"),m_sliderContrast.GetPos());
	GetDlgItem(IDC_EDIT_CONTRAST)->SetWindowText(strEditText);
	strEditText.Format(_T("%d"),m_sliderSaturation.GetPos());
	GetDlgItem(IDC_EDIT_SATURATION)->SetWindowText(strEditText);
	strEditText.Format(_T("%d"),m_sliderHue.GetPos());
	GetDlgItem(IDC_EDIT_HUE)->SetWindowText(strEditText);
	strEditText.Format(_T("%d"),m_sliderDetail.GetPos());
	GetDlgItem(IDC_EDIT_DETAIL)->SetWindowText(strEditText);
	strEditText.Format(_T("%d"),m_sliderDenoise.GetPos());
	GetDlgItem(IDC_EDIT_DENOISE)->SetWindowText(strEditText);
}

void CDlgVideoParam::OnBnClickedOk()
{
	CDialog::OnOK();
}

void CDlgVideoParam::OnBnClickedCancel()
{
	m_rNotify.VideoParamNotifyNewParam(m_Video_Param);

	CDialog::OnCancel();
}

void CDlgVideoParam::InitParam(Video_Param param)
{
	if (!m_sliderBrightness.GetSafeHwnd())
	{
		return;
	}

	m_sliderBrightness.SetRange(-100,100);
	m_sliderBrightness.SetPos(param.iBrightness);

	m_sliderContrast.SetRange(0,10);
	m_sliderContrast.SetPos(param.iContrast);

	m_sliderSaturation.SetRange(0,10);
	m_sliderSaturation.SetPos(param.iSaturation);

	m_sliderHue.SetRange(-180,180);
	m_sliderHue.SetPos(param.iHue);

	m_sliderDetail.SetRange(0,100);
	m_sliderDetail.SetPos(param.iDetail);

	m_sliderDenoise.SetRange(0,100);
	m_sliderDenoise.SetPos(param.iDenoise);

	FillEditText();
}


void CDlgVideoParam::OnBnClickedDefault()
{
	Video_Param param;
	param.iBrightness		= 0;
	param.iContrast			= 1;
	param.iSaturation		= 1;
	param.iHue				= 0;
	param.iDetail			= 0;
	param.iDenoise			= 0;

	InitParam(param);

	m_rNotify.VideoParamNotifyNewParam(param);
}
