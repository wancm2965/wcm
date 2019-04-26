#pragma once


// CWBPromptFontSetDlg 对话框

class CWBPromptFontSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBPromptFontSetDlg)

public:
	CWBPromptFontSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBPromptFontSetDlg();
	virtual BOOL OnInitDialog();
	void InsertStr(CComboBoxEx *comboxData,CString str);

	CComboBoxEx *GetFontFaceComBox();
	CComboBoxEx *GetFontSizeComBox();
	CWBFunctionBtn *GetFontBoldBtn();
	CWBFunctionBtn *GetFontItalicBtn();
	CWBFunctionBtn *GetFontUnderlineBtn();
	CWBFunctionBtn *GetFontVerticalBtn();

	// 对话框数据
	enum { IDD = IDD_PROMPTFONTSETDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CComboBoxEx m_FontFaceComBox;
	CComboBoxEx m_FontSizeComBox;
	CWBFunctionBtn m_FontBoldBtn;
	CWBFunctionBtn m_FontItalicBtn;
	CWBFunctionBtn m_FontUnderlineBtn;
	CWBFunctionBtn m_FontVerticalBtn;
public:
	afx_msg void OnCbnSelchangePromptfontfacecombox();
	afx_msg void OnCbnSelchangePromptfontsizecombox();
	afx_msg void OnBnClickedPromptfontboldbtn();
	afx_msg void OnBnClickedPromptfontitalicbtn();
	afx_msg void OnBnClickedPromptfontunderlinebtn();
	afx_msg void OnBnClickedPromptfontVerticalbtn();
	afx_msg void OnCbnEditchangeWbFontsizecombox();
	virtual void OnOK();
	virtual void OnCancle();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnWM_TEXTARRIBUTE(WPARAM wParam, LPARAM lParam);
	void CreateFontAndSetFont();
};
