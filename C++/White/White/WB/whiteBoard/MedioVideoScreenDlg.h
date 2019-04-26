#pragma once
#include "WndVedio.h"
#include "wbattributestatic.h"

// CMedioVideoScreenDlg �Ի���

class CMedioVideoScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMedioVideoScreenDlg)

public:
	CMedioVideoScreenDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMedioVideoScreenDlg();

// �Ի�������
	enum { IDD = IDD_MEDIOVEDIOSCREEN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtFullScreen();
	afx_msg void OnBnClickedButArea();
	afx_msg void OnBnClickedButWndScreen();
	afx_msg void OnBnClickedButStop();
	afx_msg void OnBnClickedButPause();
	afx_msg void OnBnClickedButSetPath();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	void    GetFileName(CString &stringname);
	void    GetFilePath(CString &stringpath);
	void    SetSkinImage(CString strPath);
	void    StopThread();
	void    OnFullCancel();//ȫ���ر�wangxiaobin    
public:
	CEdit   m_FileName;
	CString m_strFileName;
	CEdit   m_FilePath;
	CString m_strFilePath;
	CEdit   m_Rate;
	int     m_nRate;
	CString m_strPlace;
	BOOL    m_bIsWndowClosing;

	HWND    m_hWndVedio;
public:
	HINSTANCE m_hDLL;
	CPoint m_p1;
	CPoint m_p2;
public:
	BOOL m_IsStop;
	BOOL m_IsPause;
	int  m_nCount;
	HWND m_GetCurWndHwnd;
	int  m_nSize;
	Bitmap* m_pBitmap;
	afx_msg void OnBnClickedButProperty();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CTransparentButtonEx m_bPauseScreen;
	CTransparentButtonEx m_bAreaScreen;
	CTransparentButtonEx m_bPropertyScreen;
	CTransparentButtonEx m_bPathScreen;
	CTransparentButtonEx m_bStopScreen;
	CTransparentButtonEx m_bWndScreen;
	CTransparentButtonEx m_bFullScreen;
	CWBAttributeStatic m_screenFileName;
	CWBAttributeStatic m_screenFrame;
	CWBAttributeStatic m_screenFrameRate;
	CWBAttributeStatic m_screenPathName;
	CWBAttributeStatic m_csPlace;
	CWBAttributeStatic m_csVedioStop;
	//CWBAttributeStatic m_csSavePath;
	// ����ļ����Ƿ�Ϸ�
	BOOL IsFileNameValid(void);
	BOOL IsFileNameValidU7(CString strName);
	static CRect m_rectLarge;
	//CRect m_rectSmall;
//	afx_msg void OnKillfocusEditFile();
	BOOL IsRateValid(void);
	//ע��DLL
// 	BOOL RegistryDll(CString szDllPath);  
	BOOL IsFileExisted(void);
//	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	//static int CALLBACK BrowseCallBackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
//	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKillfocusEditFile();
	afx_msg void OnKillfocusEditRate();
	void GetAbsolute_path( const char* pPath,char* pAbsolute_path );
	void GetCurFileName(char* filename,char* pAbsolute_Name,char ch = '.');

	BOOL GetAreaVideoScreenHwnd();
	BOOL IsFilePathOk(CString str);
	CString GetCurrentFilePath(){return m_strCurrentFileName;}

private:
	CString m_strCurrentFileName; //��¼��ǰ¼�Ƶ�·��
	CString m_strPrompt;
	bool m_bPathisLoad; //�ж��ļ��Ƿ��Ѿ���
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
