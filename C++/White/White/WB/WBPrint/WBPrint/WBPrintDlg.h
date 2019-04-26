
// WBPrintDlg.h : ͷ�ļ�
//

#pragma once


// CWBPrintDlg �Ի���
class CWBPrintDlg : public CDialogEx
{
// ����
public:
	CWBPrintDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_WBPRINT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	
// ʵ��
protected:
	HICON m_hIcon;
	HWND hWndWhiteBoard;
	// ���ɵ���Ϣӳ�亯��
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
