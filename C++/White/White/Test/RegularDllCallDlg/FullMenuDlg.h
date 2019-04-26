#pragma once


// CFullMenuDlg 对话框

class CFullMenuDlg : public CDialog
{
	DECLARE_DYNAMIC(CFullMenuDlg)

public:
	CFullMenuDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFullMenuDlg();

// 对话框数据
	enum { IDD = IDD_FULLMENU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	// 撤销的总步数   12-25添加jin
	int m_UndoSetpCount;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg LRESULT OnNcHitTest(CPoint point);

	afx_msg void OnWbNewfileb();
	afx_msg void OnWbOpenfileb();
	afx_msg void OnWbSavefileb();
	afx_msg void OnWbResavefileb();
	afx_msg void OnWbPrintfileb();
	afx_msg void OnWbClosefileb();
	afx_msg void OnWbNewpageb();
	afx_msg void OnInsertpageb();
	afx_msg void OnDeletepageb();
	afx_msg void OnWbForepageb();
	afx_msg void OnWbNextpageb();
	afx_msg void OnWbFirstpageb();
	afx_msg void OnWbLastpageb();
	afx_msg void OnPicturelayer();
	afx_msg void OnWbForelayerb();
	afx_msg void OnWbNextlayerb();
	afx_msg void OnWbToplayerb();
	afx_msg void OnWbBottomlayerb();
	afx_msg void OnWbLineb();
	afx_msg void OnWbSelectb();
	afx_msg void OnWbCurveb();

	afx_msg void OnWbLitterrubberb();
	afx_msg void OnWbBigrubberb();
	afx_msg void OnWbSingleobjectrubberb();
	afx_msg void OnWbCurveselectrubberb();

	afx_msg void OnWbRectangleb();
	afx_msg void OnWbRoundb();
	afx_msg void OnWbTriangleb();
	afx_msg void OnWbRhombusb();
	afx_msg void OnWbPolygonb();
	afx_msg void OnWbCharfontb();
	afx_msg void OnWbChartextb();

	afx_msg void OnWbPictureb();
	afx_msg void OnPen();
	afx_msg void OnLinewidthb1();
	afx_msg void OnWbDocumentb();
	afx_msg void OnWbCutscreenb();
	afx_msg void OnWbDeleteb();
	afx_msg void OnWbUccb();
	afx_msg void OnWbResourcecatagoryb();
	afx_msg void OnWbResourceManger();
	afx_msg void OnWbFlashb();
	afx_msg void OnWbNormalb();
	afx_msg void OnWbUndob();
	afx_msg void OnWbRedob();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	afx_msg void OnWbDustbinb();
};
