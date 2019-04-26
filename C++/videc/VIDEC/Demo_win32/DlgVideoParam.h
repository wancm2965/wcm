#if !defined(AFX_DLGVIDEOPARAM_H__65E3A7F1_89D4_4F3D_8F21_522BA47C97E0__INCLUDED_)
#define AFX_DLGVIDEOPARAM_H__65E3A7F1_89D4_4F3D_8F21_522BA47C97E0__INCLUDED_

#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgVideoParam.h : header file
//

typedef struct _VIDEO_PARAM
{
	int iBrightness;	//亮度
	int iContrast;		//对比度
	int iSaturation;	//饱和度
	int iHue;			//色调
	int iDetail;		//图像增强
	int iDenoise;		//图像去噪

}Video_Param;


class VideoParamNotify
{
public:
	VideoParamNotify(){};
	~VideoParamNotify(){};
public:

	virtual void VideoParamNotifyNewParam(Video_Param param) = 0;
};


// CDlgVideoParam dialog

class CDlgVideoParam : public CDialog
{
// Construction
public:
	CDlgVideoParam(VideoParamNotify& rNotify, Video_Param param,CWnd* pParent = NULL);   // standard constructor
	void UpdataView();

// Dialog Data
	//{{AFX_DATA(CDlgVideoParam)
	enum { IDD = IDD_DIALOG_VIDEO_PARAM };
	CEdit	m_edtSaturation;
	CEdit	m_edtHue;
	CEdit	m_edtContrast;
	CEdit	m_edtBrightness;
	CSliderCtrl	m_sliderSaturation;
	CSliderCtrl	m_sliderHue;
	CSliderCtrl	m_sliderContrast;
	CSliderCtrl	m_sliderBrightness;
	//}}AFX_DATA

	CEdit		m_edtDetail;
	CSliderCtrl m_sliderDetail;

	CEdit		m_edtDenoise;
	CSliderCtrl m_sliderDenoise;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgVideoParam)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void InitParam(Video_Param param);
	void FillEditText();

	VideoParamNotify&		m_rNotify;
	Video_Param				m_Video_Param;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedDefault();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVIDEOPARAM_H__65E3A7F1_89D4_4F3D_8F21_522BA47C97E0__INCLUDED_)
