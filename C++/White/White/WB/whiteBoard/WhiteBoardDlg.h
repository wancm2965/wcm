#pragma once

#include "WhiteBoardView.h"
#include "atltypes.h"
#include "WBFullDlg.h"
#include "WBToolbarCtrlDlg.h"
#include "WBToolbarDlg.h"
#include "WBRedoAndUndoDlg.h"
#include "WBPageDlg.h"
//#include "MultiWBFunctionDlg.h"
#include "WBEagelEye.h"
#include "WBAboutDlg.h"
#include "WBOptionDlg.h"


// CWhiteBoardDlg 对话框
class CWhiteBoardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWhiteBoardDlg)
	
public:
	CWhiteBoardDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWhiteBoardDlg();
	//打开全屏对话框
	void setFullDlg();
	//关闭前全屏对话框
	void closeFullDlg();

	virtual BOOL OnInitDialog();
	//工具条界面的相关函数
	//设置view的可视大小
	void MoveCWBView();
	void MoveToolbar();
	void ShowIsTouchStatic();
	void HideShowToolbar();  //显示和隐藏工具条
	CWBFunctionDlg *GetWBFunctionDlg();
	CWBOptionDlg *CreateOptionDlg();
	void MoveWBFunctionDlg(BOOL isShow);
	void MoveOptionDlg(BOOL isShow);
	void DeleteFuncPad();
//	void ChangeToolbarPos();
	//创建触摸资源窗体
	HRESULT CreateResDlg(WPARAM wPar, LPARAM lPar);

	void OpenPic(CString strFilePath);
	void ShowAllToolbars(bool bShow = true);

	CWBFunctionDlg* GetSinFunctionDlg();//获得m_sinFunctiondlg指针
	CWBOptionDlg *GetOptionDlg();//获得optiondlg指针
// 对话框数据
	enum { IDD = IDD_WHITEBOARD_DLG };

	CWhiteBoardView* m_pWhiteboard_view;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	DECLARE_MESSAGE_MAP()
	



private:
	CWBFullDlg m_wbFullDlg;
	CRect m_rect;
	CRect m_WinRect;
	CString m_strCurFilePath;
	BOOL m_bSinFuncDlgShow;//设置function是否显示
	BOOL m_bSinOptDlgShow;//设置option是否显示
	//工具条的相关变量
	CWBToolbarCtrlDlg *m_toolbarCtrlDlg;
	CWBToolbarDlg *m_toolbarDlg;
	CWBRedoAndUndoDlg * m_redoDlg;
	CWBPageDlg  *m_pPageDlg;	
	CWBOptionDlg *m_pWBOptionDlg;
	CWBFunctionDlg *m_pWBFuncDlg;
	CWBEagelEye  *m_pWBEagelEye;
	int m_nToolNum; //工具条显示按钮个数
	bool m_IsCloseScreenDraw;//全屏状态改变时的桌面绘图打开的情况下，先在view的SendLocalMessage函数中关闭桌面绘图，在此对话框的onsize函数中打开

	DWORD m_dFirstTime;  //用于（功能面板窗口第一次创建要延迟N秒，以缓冲文件的页面预览页）第一次创建功能面板的运行时间
public:
	int m_winX,m_winY;
/*private:*/

	//获取资源窗体的窗体位置
	BOOL GetResGalleryRect(CRect &rct);
	//CWBGlobalMediaCtrl* m_pWBGlobalMediaCtrl;
	//void CreateMediaCtrl();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//HRESULT OnReceiveDragResouce(WPARAM wPar, LPARAM lPar);
	//工具栏按钮事件
	virtual void OnOK();
	virtual void OnCancel();
	CWBToolbarCtrlDlg *GetToolbarCtrDlg();
	CWBToolbarDlg * GetToolbarDlg();
	CWBRedoAndUndoDlg *GetRedoAndUndoDlg();
	CWBPageDlg *GetPageDlg();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CString GetCurFilePath();
	/*CWBGlobalMediaCtrl* GetGlobalMediaCtrl();*/
	CWBEagelEye* GetWBEagleEyeDlg(); //获得鹰眼对话框的指针
	BOOL SetAndShowEagleEye(CRect rect);//显示设置鹰眼的位置
	void CreateAboutDlg();
	void SetIsOpenScreenDraw(bool IsCloseScreenDraw);


	void SetSinFuncShow(BOOL show);
	BOOL GetSinFuncShow();
	void SetSinOptShow(BOOL show);
	BOOL GetSinOptShow();
	
	void SetViewPos(CRect rc);//调整view的位置
	CWBFunctionDlg * GetFuntDlg();
	void DeleteWBFunctionDlg();
};
