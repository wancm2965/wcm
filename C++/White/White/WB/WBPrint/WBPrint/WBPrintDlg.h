
// WBPrintDlg.h : 头文件
//

#pragma once


// CWBPrintDlg 对话框
class CWBPrintDlg : public CDialogEx
{
// 构造
public:
	CWBPrintDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WBPRINT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	
// 实现
protected:
	HICON m_hIcon;
	HWND hWndWhiteBoard;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	BOOL DeleteDirectory(CString DirName)  ;
	void  PrintDraw(CDC* pDC, CPrintInfo* pInfo,float fWidth, float fHeight, CRect &rectDraw1 ,int print_x,int print_y);
	void RecalculateRect(const CRect& rectSrc, float fRate, CRect& rectOut);
	void OnPrint1(CDC* pDC, CPrintInfo* pInfo,CRect &rectDraw1,int print_x,int print_y);
	void DrawBMP(CDC* pDC,int iLogPixelX,int iLogPixelY,const char *strFileName);
	void print();

	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
