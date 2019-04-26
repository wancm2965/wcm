#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"


// CWBResourseBarDlg 对话框

class CWBResourseBarDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBResourseBarDlg)

public:
	CWBResourseBarDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBResourseBarDlg();

// 对话框数据
	enum { IDD = IDD_WB_RESOURSEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	void Relayout();
	void SetSkinImage(CString strPath);
	CString GetCaptureFromType(eType btnEmumType);
private:
	CWnd * m_pParentDlg;
	Bitmap* m_pBitmap;

	CTransparentButtonEx *m_pParentBtn;
	CTransparentButtonEx m_imageResBtn;
	CTransparentButtonEx m_videoResBtn;
	CTransparentButtonEx m_doucumentResBtn;
	CTransparentButtonEx m_flashResBtn;
	CTransparentButtonEx m_cameraHeadRESBtn;
	CTransparentButtonEx m_dicomResBtn;
	CTransparentButtonEx m_dxfResBtn;
	CTransparentButtonEx m_midiResBtn;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedImageResbtn();
	afx_msg void OnBnClickedVideoResbtn();
	afx_msg void OnBnClickedDocumentResbtn();
	afx_msg void OnBnClickedFlashResbtn();
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedMidiResbtn();
	afx_msg void OnBnClickedCameraheadResbtn();
	afx_msg void OnBnClickedDicomResbtn();
	afx_msg void OnBnClickedDxfResbtn();
};
