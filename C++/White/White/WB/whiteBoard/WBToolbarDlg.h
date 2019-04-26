#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"
#include "WBLineDlg.h"
#include "WBRubberDlg.h"
#include "WBGraphicsDlg.h"
#include "WBResourseBarDlg.h"
#include "WBTextDlg.h"
#include "WBColorPaletteDlg.h"
#include "WBToolboxDlg.h"
#include "WBFileDlg.h"
#include "WBDesktopDrawDlg.h"
#include "WBToolDlg.h"

// CWBToolbarDlg 对话框

class CWBToolbarDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBToolbarDlg)

public:
	CWBToolbarDlg(CWnd* pParent = NULL , int nToolNum = 11);   // 标准构造函数
	virtual ~CWBToolbarDlg();
	COLORREF m_dlgBackgroundClr;
// 对话框数据
	enum { IDD = IDD_WB_TOOLBARDLG };

	DECLARE_MESSAGE_MAP()
public:
	static HWND g_toolBarHWND;
private:  //自定义变量
	BOOL m_bFullScreen;
	

	//子对话框指针变量
	CWBLineDlg      * m_pLineDlg;   
	CWBRubberDlg    * m_pRubberDlg;
	CWBGraphicsDlg  * m_pGraphicsDlg;
	CWBResourseBarDlg  *m_pResourseBarDlg;
	CWBTextDlg      * m_pTextDlg;
	CWBColorPaletteDlg *m_pColorPatetteDlg;
	CWBToolboxDlg  *m_pToolboxdlg;
	CWBFileDlg *m_pFileDlg;
	CWBDesktopDrawDlg * m_pDesktopDrawDlg;
	CWBToolDlg   *m_pToolDlg;

	CTransparentButtonEx *m_pBtnOne;
	CTransparentButtonEx *m_pBtnTwo;
	CTransparentButtonEx *m_pbtnThree;
	CTransparentButtonEx *m_pBtnFour;
	CTransparentButtonEx *m_pBtnFive;
	CTransparentButtonEx *m_pBtnSix;
	CTransparentButtonEx *m_pBtnSeven;
	CTransparentButtonEx *m_pBtnEight;
	CTransparentButtonEx *m_pBtnNine;
	CTransparentButtonEx *m_pBtnTen;
	CTransparentButtonEx *m_pBtnEleven;
	CTransparentButtonEx *m_ptChooseBtn;
	Bitmap* m_pBitmap;
	XCritSec			m_csToolBarLock;
	int m_nToolbarNum;	
	std::map<int,CString> *m_pCheckBtnInfo;
	void CheckBtnInfo();
public://自定义函数
	void setToolbarNum(int num);
	void ReLayout();                //布局对话框和按钮位置
	void CloseAllChildToolBox();    //关闭不是本次选择的子工具条
	void CloseToolbox();         //关闭工具箱
	void OpenLineDlg(CTransparentButtonEx *pParentBtn); //打开线型子工具条
	void OpenRubberDlg(CTransparentButtonEx *pParentBtn);//
	void OpenGraphicsDlg(CTransparentButtonEx *pParentBtn);//
	void OpenResourseBarDlg(CTransparentButtonEx *pParentBtn);//
	void OpenTextDlg(CTransparentButtonEx *pParentBtn);//
	void OpenColorPatetteDlg(CTransparentButtonEx *pParentBtn);
	void OpenFileDlg(CTransparentButtonEx *pParentBtn);
	void OpenDesktopDrawDlg(CTransparentButtonEx *pParentBtn);
	void OpenToolDlg(CTransparentButtonEx *pParentBtn);
	void OpenToolboxDlg();
	void SetLineDlgNUll();
	void SetRubberDlgNUll();
	void SetGraphicsDlgNUll();
	void SetResourseBarDlgNUll();
	void SetTextDlgNUll();
	void SetColorPatetteDlgNULL();
	void SetToolboxDlgNULL();
	void SetFileDlgNULL();
	void SetDesktopDrawDlgNULL();
	void SetToolDlgNULL();
	CString GetCaptureFromType(eType btnEmumType);//获得按钮的标题
	void ProcessCmd(eType cmdType,CTransparentButtonEx *pParentBtn = NULL);//

	
	void ReflashBtn(eType btnType);//让按钮对象刷新
	void ChangButton(CTransparentButtonEx *pDownBtn,int downY,CTransparentButtonEx * pUpBtn,int upY); //交换自身内部按钮的位置
	void CloseAllChildBox();//关闭子工具栏
	BOOL GetIsFullScreen();
	void SetIsFullScreen(BOOL isFullScreen);
	void SetNotifyIsSuccessLoad(BOOL bLoadSuccess);
	void SetWBFullScreen(bool isFull);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnone();
	afx_msg void OnBtntwo();
	afx_msg void OnBtnthree();
	afx_msg void OnBtnfour();
	afx_msg void OnBtnfive();
	afx_msg void OnBtnsix();
	afx_msg void OnBtnseven();
	afx_msg void OnBtneight();
	afx_msg void OnBtnnine();
	afx_msg void OnBtnten();
	afx_msg void OnBtneleven();//用于单机版时显示联网和本地
	afx_msg LRESULT OnCloseToolbarMessage(WPARAM wParam, LPARAM lParam);
	void SetChooseBtn(CTransparentButtonEx *mptchoosebtn);
};
