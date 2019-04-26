
// Demo_win32Dlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#include "VideoWnd.h"
#include "CaptureWnd.h"

typedef std::map<unsigned long, CVideoWnd*> MAP_Wnd;
typedef std::map<CCaptureWnd*, CCaptureWnd*> MAP_Capture;

#define MENU_VIDEO_WND			(WM_USER + 2900)
#define MSG_SET_FULLWND			(WM_USER + 3000)

enum VIDEO_WND_SIZE
{
	VWND_CHILD_NORMAL = 0,
	//VWND_CHILD_FULL,
	VWND_NOMAL,
	VWND_NORMAL_FULL,
	VWND_SIZE_COUNT,
};


struct XSurfaceBuffer
{
	XSurfaceBuffer(void)
		: pData(NULL)
		, nDatalen(0)
	{
	}
	virtual~XSurfaceBuffer(void)
	{
		if(pData)
		{
			free(pData);
			pData=NULL;
		}
	}
	void*			pData;
	int				nDatalen;
};



// CDemo_win32Dlg 对话框
class CDemo_win32Dlg
	: public CDialog
	, public NETEC_Node
	, public XThreadBase
{
// 构造
public:
	CDemo_win32Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DEMO_WIN32_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	MAP_Wnd					m_mapWnd;
	MAP_Capture				m_mapCapture;
	int						m_nVideoCount;
	CEdit					m_editAV;
	CButton 				m_btnRecv;
	CButton 				m_btnStop;
	unsigned long			m_ulAudioID;
	unsigned long			m_ulVideoID;
	bool					m_bStartCapture;

	CStatic m_stacMCUIP;
	CEdit m_editMCUIP;
	CStatic m_stacRate;
	CEdit m_editRate;
	CEdit m_editCapture;
	CButton m_btnCapture;
	CButton m_bntCaptureStop;
	CStatic m_stacBitrate;
	CEdit m_editBitrate;
	bool							m_bWaitStop;
	XListPtr						m_listPacketData;
	XCritSec						m_csListPacketData;

public:
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);

	void ReSize(int* pVideoIndex=NULL);
	void StopAll(void);
	void InitChans(void);
	void CaptureLocalDev(int nDev,int nWidth,int nHeight,int nFrameRate,int nBitrate);
	void CaptureFile(const std::string& strFile,int nFrameRate,int nBitrate);
	std::string BrowseFile();

	int PlayData(void*pPacketData,int nPacketSize);

	static CDemo_win32Dlg* Instance(void);

	virtual void ThreadProcMain(void);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRecv();
	afx_msg void OnBnClickedButtonStop();
	afx_msg LRESULT	OnSetFullWnd(WPARAM wParam, LPARAM lParam);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedButtonCapture();
	afx_msg void OnBnClickedButtonCaptureStop();
	afx_msg void OnBnClickedCheckShow();

	CButton m_btnShow;
	CButton m_btnShow2;
	CButton m_btnPG_SDK;
	CButton m_btnEncode;
	CComboBox m_cbxVideoChan;
	CComboBox m_cbxVideoShow;
};


