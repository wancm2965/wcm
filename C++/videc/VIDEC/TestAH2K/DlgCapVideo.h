#pragma once


#include "../../include/VIDEC/VIDEC.h"
#include <list>
#ifdef TEST_MEDIAFILE1
#include "../../include/VIDEC/VIDEC_MediaFile.h"
#endif

// DlgCapVideo 对话框
class DlgMain;
class DlgCapVideo 
	: public CDialog
	,VIDEC_WndCallback 
{
	DECLARE_DYNAMIC(DlgCapVideo)

public:
	DlgCapVideo(DlgMain&rDlgMain);   // 标准构造函数
	virtual ~DlgCapVideo();

#ifdef TEST_MEDIAFILE1
	VIDEC_MediaFile*	m_pMediaFile;
#else
	VIDEC_Wnd*		m_pWnd;

	typedef std::list<VIDEC_Wnd*>	ListWnd;
	ListWnd			m_ListWnd;
#endif

	DlgMain&		m_rDlgMain;

	void SetImageSource(VIDEC_ImageSource*pImageSource);

	void ResizeControls(void);

// 对话框数据
	enum { IDD = IDD_DIALOG_CAP_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
