// WhiteBoardDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WhiteBoardDlg.h"
#include "CatchScreenDlg.h"
#include "afxdialogex.h"
#include "../WB/whiteBoard/common/WBMedia/WBGlobalMediaCtrl.h"
//#include "ResManage.h"
#include "WBResFunc.h"
#include "WhiteBoardView.h"
#include "ScreenDrawMgr.h"
#include <fstream>

// CWhiteBoardDlg 对话框
//IMPLEMENT_DYNAMIC(CWhiteBoardDlg, CDialogEx)
IMPLEMENT_DYNAMIC(CWhiteBoardDlg, CDialogEx)

CWhiteBoardDlg::CWhiteBoardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWhiteBoardDlg::IDD, pParent)
	//, m_pResGallery(NULL)
	//, m_pWBGlobalMediaCtrl(NULL)
{
	m_pWhiteboard_view = NULL;
    m_toolbarCtrlDlg = NULL;
	m_toolbarDlg = NULL;
	m_redoDlg = NULL;
	m_pPageDlg = NULL;
	m_pWBEagelEye = NULL;
	m_pWBFuncDlg = NULL;
	m_pWBFuncDlg = NULL;
	m_pWBOptionDlg = NULL;
	m_IsCloseScreenDraw = false;
	CTransparentButtonEx::Init();
	m_nToolNum = 0;
	CWBToolTip::CreateIns();			               //创建按钮提示类实例
	CWBToolTip::GetIns()->InitCToolTipTemplateDlg(this);	//创建提示类中的窗口对话框
 	m_bSinFuncDlgShow=FALSE;
	m_bSinOptDlgShow=FALSE;
	m_dFirstTime = 0;
	
}

CWhiteBoardDlg::~CWhiteBoardDlg() 
{
	if (NULL != m_toolbarCtrlDlg)
	{
		delete m_toolbarCtrlDlg;
		m_toolbarCtrlDlg = NULL;
	}
	if (NULL != m_toolbarDlg)
	{
		m_toolbarDlg->CloseAllChildBox();
		delete m_toolbarDlg;
		m_toolbarDlg = NULL;
	}
	if (NULL != m_redoDlg)
	{
		delete m_redoDlg;
		m_redoDlg = NULL;
	}
	if (NULL != m_pPageDlg)
	{
		delete  m_pPageDlg;
		m_pPageDlg = NULL;
	}
	if (NULL != m_pWBEagelEye)
	{
		delete m_pWBEagelEye;
		m_pWBEagelEye = NULL;
	}
	if (m_pWBFuncDlg != NULL)
	{
		delete m_pWBFuncDlg;
		m_pWBFuncDlg =NULL;
	}
	WBRESWND::UnInitWBResModule();
	CWBToolTip::DestroyIns();//销毁按钮提示类实例

}

void CWhiteBoardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWhiteBoardDlg, CDialogEx)
	//ON_MESSAGE(UWM_RESGALLERY_BOUNDARY, OnReceiveDragResouce)
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_MESSAGE(UWM_CREATE_RESOURCE_DLG, CreateResDlg)
	ON_WM_MOVE()
END_MESSAGE_MAP()

// CWhiteBoardDlg 消息处理程序


BOOL CWhiteBoardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 将"关于 "菜单项添加到系统菜单中。
	// TODO: 在此添加额外的初始化代码
	m_pWhiteboard_view = (CWhiteBoardView*) RUNTIME_CLASS(CWhiteBoardView)->CreateObject();
	//在目标位置动态创建视图
	if (NULL == m_pWhiteboard_view)  
	{  
		return FALSE;  
	} 
	m_rect =  CRect(0,0,WB_VIEWHSCROLL,WB_VIEWVSCROLL);
	m_pWhiteboard_view->Create(NULL, NULL,   WS_VISIBLE | AFX_WS_DEFAULT_VIEW | WS_VSCROLL| WS_HSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS , m_rect, this, NULL); 
	m_pWhiteboard_view->setViewScroll();
	
	//if (!g_bYunNanDeviceSupported || (g_bYunNanDeviceSupported && g_bAuthorizedByHandWare ))//判断是否是云南公安的版本
	if ((!g_bYunNanDeviceSupported || (g_bYunNanDeviceSupported && g_bAuthorizedByHandWare ))&&g_bWBWndIsneedShowToolBtn)//判断是否是云南公安的版本
	{
		m_toolbarCtrlDlg = new CWBToolbarCtrlDlg();
		m_toolbarCtrlDlg->Create(IDD_WB_CTRLBTNDLG,this);
		m_toolbarCtrlDlg->ShowWindow(SW_SHOW);
		//生成工具条对话框
		if (g_WBType == WB_MULTIVER)
		{
			CRect WBDLGrect;
			GetClientRect(&WBDLGrect);
			if (WBDLGrect.Height() != 0)
			{
				int WBHeight = WBDLGrect.Height();
				m_nToolNum = (int)(WBHeight - WB_TOOLBARBTN_SPACE*11 - WB_CTRLBTNDLG_HEIGHT)/WB_TOOLBARBTN_HEIGHT;
				if (g_bU7NewStyle)
				{
					if (m_nToolNum > 8)
					{
						m_nToolNum = 8;
					}
				}
				else
				{
					if (m_nToolNum > 9)
					{
						m_nToolNum = 10;
					}
				}
				
			}
		}
		if (m_nToolNum != 0)
		{
			m_toolbarDlg = new CWBToolbarDlg(this , m_nToolNum);
		}
		else
		{
			m_toolbarDlg = new CWBToolbarDlg(this);
		}
		
		if (NULL != m_toolbarDlg)
		{
			m_toolbarDlg->Create(IDD_WB_TOOLBARDLG,this);
			m_toolbarDlg->ShowWindow(SW_SHOW);
			//redo 和 UNdo 对话框
			m_redoDlg = new CWBRedoAndUndoDlg(m_toolbarDlg,this);
			m_redoDlg->Create(CWBRedoAndUndoDlg::IDD,this);
			m_redoDlg->ShowWindow(SW_SHOW);
			//页面对话框
			m_pPageDlg = new CWBPageDlg(m_toolbarDlg,this);
			m_pPageDlg->Create(CWBPageDlg::IDD,this);
			m_pPageDlg->ShowWindow(SW_SHOW);
		}


		// 在view中设置工具条对话框句柄
		m_pWhiteboard_view->SetToolbarHWND(m_toolbarDlg->m_hWnd);
	} 
	//加载资源窗口dll（不创建窗体）
	WBRESWND::InitWBResModule();
	if (g_bSupportU7OpenFile)
	{
		//WBRESWND::SetSupportU7OpenFile(TRUE);
		//WBRESWND::LoadResFromDirectoryWB(g_strAvconTempPath);
	}
	WBRESWND::SetAvconIniPath(g_strAvconIniPath);
	if (!g_bWBWndIsneedShowToolBtn)
	{
		if (m_toolbarCtrlDlg)
		{
			m_toolbarCtrlDlg->ShowWindow(SW_HIDE);
		}
		if (m_toolbarDlg)
		{
			m_toolbarDlg->ShowWindow(SW_HIDE);
		}
		if (m_pPageDlg)
		{
			m_pPageDlg->ShowWindow(SW_HIDE);
		}
		if (m_pWBEagelEye)
		{
			m_pWBEagelEye->ShowWindow(SW_HIDE);
		}
		if (m_pWBFuncDlg)
		{
			m_pWBFuncDlg->ShowWindow(SW_HIDE);
		}
		if (m_pWBOptionDlg)
		{
			m_pWBOptionDlg->ShowWindow(SW_HIDE);
		}
		if (m_redoDlg)
		{
			m_redoDlg->ShowWindow(SW_HIDE);
		}
	}
	g_bMedioScreenWndEnd = TRUE;

	//创建功能选择卡
	m_pWBOptionDlg = new CWBOptionDlg;
	m_pWBOptionDlg->Create(IDD_WB_OPTIONDLG,this);	
	
	GetFuntDlg();
    return TRUE;  // 除非将焦点设置到控件，否则返回TRUE
}

void CWhiteBoardDlg::ShowAllToolbars(bool bShow /* = true */)
{
	if (bShow)
	{
		if (m_toolbarCtrlDlg != NULL && ::IsWindow(m_toolbarCtrlDlg->GetSafeHwnd()))
		{
			m_toolbarCtrlDlg->ShowWindow(SW_SHOW);
		}

		if (m_toolbarDlg != NULL && ::IsWindow(m_toolbarDlg->GetSafeHwnd()))
		{
			m_toolbarDlg->ShowWindow(SW_SHOW);
		}

		if (m_redoDlg != NULL && ::IsWindow(m_redoDlg->GetSafeHwnd()))
		{
			m_redoDlg->ShowWindow(SW_SHOW);;
		}
		
		if (m_pPageDlg != NULL && ::IsWindow(m_pPageDlg->GetSafeHwnd()))
		{
			m_pPageDlg->ShowWindow(SW_SHOW);
		}		
	}
	else
	{
		if (m_toolbarCtrlDlg != NULL && ::IsWindow(m_toolbarCtrlDlg->GetSafeHwnd()))
		{
			m_toolbarCtrlDlg->ShowWindow(SW_HIDE);
		}

		if (m_toolbarDlg != NULL && ::IsWindow(m_toolbarDlg->GetSafeHwnd()))
		{
			m_toolbarDlg->ShowWindow(SW_HIDE);
		}

		if (m_redoDlg != NULL && ::IsWindow(m_redoDlg->GetSafeHwnd()))
		{
			m_redoDlg->ShowWindow(SW_HIDE);;
		}

		if (m_pPageDlg != NULL && ::IsWindow(m_pPageDlg->GetSafeHwnd()))
		{
			m_pPageDlg->ShowWindow(SW_HIDE);
		}		
	}
}


void CWhiteBoardDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	//修改更改白板大小时无法SHOW出功能面板
	if (GetWBFunctionDlg())
	{
		GetWBFunctionDlg()->SetWindowIsHide(true);
		::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_NOSELECTWBOBJ,0,0);
	}

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(&rect);

	if (g_bSupportI7 ||(rect.Width()<470)||(rect.Height()<250))
	{
		g_bWBWndIsneedShowToolBtn = false;
	}
	else
	{
		g_bWBWndIsneedShowToolBtn = true;
	}
	
	if (g_bWBWndIsneedShowToolBtn)
	{
		if(WBRESWND::IsGalleryWndCreated())
		{
			CRect rctWndClient;
			BOOL bGetOK = GetResGalleryRect(rctWndClient);
			if (bGetOK)
			{
				BOOL bVisible = WBRESWND::IsGalleryWndShow();
				UINT nFlag = (bVisible)?SWP_SHOWWINDOW:SWP_HIDEWINDOW;

				WBRESWND::MoveGalleryWndEx(HWND_TOP, rctWndClient.left, rctWndClient.top, 
					rctWndClient.Width(), rctWndClient.Height(), nFlag);
			}
		}
		//设置控制面板的图层
		if (g_pWhiteBoardView)
		{
			CWBFunctionDlg *pMulFuncPad =  g_pWhiteBoardView->GetWBFunctionDlg();
			if (pMulFuncPad)
			{
				::SetWindowPos(pMulFuncPad->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW);
			}
		}
		if ((NULL != m_pWhiteboard_view) && (::IsWindow(m_pWhiteboard_view->m_hWnd)))
		{
			MoveCWBView();
		}		
		if (m_toolbarCtrlDlg != NULL && ::IsWindow(m_toolbarCtrlDlg->m_hWnd))
		{
			MoveToolbar();
		}
		if (m_pWBEagelEye)
		{
			::SetWindowPos(m_pWBEagelEye->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		}

		if( m_IsCloseScreenDraw)
		{   
			WB_DrawScreen(NULL);
			m_IsCloseScreenDraw = false;

		}
	}
	else
	{
		if ((NULL != m_pWhiteboard_view) && (::IsWindow(m_pWhiteboard_view->m_hWnd)))
		{
			GetClientRect(&m_rect);
			m_pWhiteboard_view->MoveWindow(m_rect);
		}
		if(WBRESWND::IsGalleryWndCreated())
		{
			CRect rctWndClient;
			BOOL bGetOK = GetResGalleryRect(rctWndClient);
			if (bGetOK)
			{
				//BOOL bVisible = WBRESWND::IsGalleryWndShow();
				UINT nFlag = /*(bVisible)?SWP_SHOWWINDOW:*/SWP_HIDEWINDOW;

				WBRESWND::MoveGalleryWndEx(HWND_TOP, rctWndClient.left, rctWndClient.top, 
					rctWndClient.Width(), rctWndClient.Height(), nFlag);
			}
		}
		if (m_toolbarCtrlDlg)
		{
			m_toolbarCtrlDlg->ShowWindow(SW_HIDE);
		}
		if (m_toolbarDlg)
		{
			m_toolbarDlg->ShowWindow(SW_HIDE);
		}
		if (m_pPageDlg)
		{
			m_pPageDlg->ShowWindow(SW_HIDE);
		}
		if (m_pWBEagelEye)
		{
			m_pWBEagelEye->ShowWindow(SW_HIDE);
		}
		if (m_pWBFuncDlg)
		{
			m_pWBFuncDlg->ShowWindow(SW_HIDE);
		}
		if (m_pWBOptionDlg)
		{
			m_pWBOptionDlg->ShowWindow(SW_HIDE);
		}
		if (m_redoDlg)
		{
			m_redoDlg->ShowWindow(SW_HIDE);
		}
		
	}
}
void CWhiteBoardDlg::setFullDlg()
{   
	m_wbFullDlg.Create(IDD_WB_FULLDLG);
	m_wbFullDlg.ShowWindow(SW_SHOW);
	g_pWhiteBoardView = m_wbFullDlg.m_whiteboard_fullview;
}

void CWhiteBoardDlg::closeFullDlg()
{
	m_wbFullDlg.DestroyWindow();
}

void CWhiteBoardDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	//CDialogEx::OnOK();
}


void CWhiteBoardDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnCancel();
}

void CWhiteBoardDlg::OpenPic(CString strFilePath)
{
	if (g_pWhiteBoardView != NULL)
	{
		CRect rect = CRect(0,0,0,0);
		g_pWhiteBoardView->AddPicture(strFilePath,rect);
	}
}
void CWhiteBoardDlg::MoveToolbar()
{
	if (NULL == m_toolbarCtrlDlg || NULL == m_toolbarDlg || NULL == m_pPageDlg)
	{
		return;
	}
	m_toolbarCtrlDlg->SetWindowPos(&wndTop
		,0 
		,0 
		,WB_CTRLBTNDLG_WIDTH 
		,WB_CTRLBTNDLG_HEIGHT
		,SWP_SHOWWINDOW );
	if (WB_SINGLERVER == g_WBType )
	{
// 		m_toolbarDlg->SetWindowPos(m_toolbarCtrlDlg	,0,WB_CTRLBTNDLG_HEIGHT ,WB_CTRLBTNDLG_WIDTH
// 			,WB_TOOLBARBTN_HEIGHT*11 + WB_TOOLBARBTN_SPACE*12
// 			,SWP_SHOWWINDOW );
		CRect WBDLGrect;
		GetClientRect(&WBDLGrect);
		if (WBDLGrect.Height() != 0)
		{
			int WBHeight = WBDLGrect.Height();
			m_nToolNum = (int)(WBHeight - WB_TOOLBARBTN_SPACE*11 - WB_CTRLBTNDLG_HEIGHT)/WB_TOOLBARBTN_HEIGHT;

			if (g_bU7NewStyle)
			{
				if (m_nToolNum > 10)
				{
					m_nToolNum = 10;
				}
				else if (m_nToolNum < 4)
				{
					m_nToolNum = 4;
				}
			}
			else
			{
				if (m_nToolNum > 11)
				{
					m_nToolNum = 11;
				}
				else if (m_nToolNum < 3)
				{
					m_nToolNum = 3;
				}
			}
		}
		if (m_nToolNum != 0)
		{			
			m_toolbarDlg->setToolbarNum(m_nToolNum);
			m_toolbarDlg->ReLayout();
			m_toolbarDlg->SetWindowPos(m_toolbarCtrlDlg	,0,WB_CTRLBTNDLG_HEIGHT ,WB_CTRLBTNDLG_WIDTH
				,WB_TOOLBARBTN_HEIGHT*m_nToolNum + WB_TOOLBARBTN_SPACE*(m_nToolNum + 1)
				,SWP_SHOWWINDOW );

			// 			HRGN rgnwind = CreateRoundRectRgn(0,0,WB_CTRLBTNDLG_WIDTH,WB_TOOLBARBTN_HEIGHT*m_nToolNum + WB_TOOLBARBTN_SPACE*(m_nToolNum + 1),4,4);
			// 			m_toolbarDlg->SetWindowRgn(rgnwind,TRUE);
			// 			DeleteObject(rgnwind);
		}

// 		HRGN rgnwind = CreateRoundRectRgn(0,0,WB_CTRLBTNDLG_WIDTH,WB_TOOLBARBTN_HEIGHT*11 + WB_TOOLBARBTN_SPACE*12,4,4);
// 		m_toolbarDlg->SetWindowRgn(rgnwind,TRUE);
// 		DeleteObject(rgnwind);
				CRect rt;
	GetClientRect(&rt);
	ShowIsTouchStatic();
    if(g_reduce>0.00)
		{
			double nViewWidth=WB_VIEWHSCROLL*g_reduce;
			double nViewHight=WB_VIEWVSCROLL*g_reduce;
			double nWindowWidth=m_rect.right;
			double nWindowHight=m_rect.bottom;
			int nLeftTopX=0;
			int nLeftTopY=0;
			nLeftTopX=(nWindowWidth-nViewWidth)/2;
			nLeftTopY=(nWindowHight-nViewHight)/2;
		
			if((nViewWidth>=nWindowWidth)&&(nViewHight>=nWindowHight))
			{
				m_redoDlg->SetWindowPos(&wndTop
				,/*nLeftTopX +*/0+ WB_CTRLBTNDLG_WIDTH + 20 /*WB_CTRLBTNDLG_WIDTH是工具条的宽度，30是REDO对话框离工具条的距离 */
				,/*nLeftTopY +*/0+ 10
				,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 8
				,WB_TOOLBOXBTN_SPEC_HEIGHT
				,SWP_SHOWWINDOW	);
				
			}
			 if((nViewHight<nWindowHight)&&(nViewWidth<nWindowWidth))
			{
				
				m_redoDlg->SetWindowPos(&wndTop
					,nLeftTopX /*+ WB_CTRLBTNDLG_WIDTH */+ 20 /*WB_CTRLBTNDLG_WIDTH是工具条的宽度，30是REDO对话框离工具条的距离 */
					,nLeftTopY + 10
					,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 8
					,WB_TOOLBOXBTN_SPEC_HEIGHT
					,SWP_SHOWWINDOW	);
			}
			if((nViewHight<nWindowHight)&&(nViewWidth>nWindowWidth))
			{
				
				m_redoDlg->SetWindowPos(&wndTop
					,/*nLeftTopX*/0 + WB_CTRLBTNDLG_WIDTH + 20 /*WB_CTRLBTNDLG_WIDTH是工具条的宽度，30是REDO对话框离工具条的距离 */
					,nLeftTopY + 10
					,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 8
					,WB_TOOLBOXBTN_SPEC_HEIGHT
					,SWP_SHOWWINDOW	);

			}
			 if((nViewHight>nWindowHight)&&(nViewWidth<nWindowWidth))
			{
				
				m_redoDlg->SetWindowPos(&wndTop
					,nLeftTopX /*+ WB_CTRLBTNDLG_WIDTH */+ 20 /*WB_CTRLBTNDLG_WIDTH是工具条的宽度，30是REDO对话框离工具条的距离 */
					,/*nLeftTopY*/0 + 10
					,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 8
					,WB_TOOLBOXBTN_SPEC_HEIGHT
					,SWP_SHOWWINDOW	);
			}
		}
		else
		{
			m_redoDlg->SetWindowPos(&wndTop
				,rt.left + WB_CTRLBTNDLG_WIDTH + 20 /*WB_CTRLBTNDLG_WIDTH是工具条的宽度，30是REDO对话框离工具条的距离 */
				,rt.top + 10
				,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 8
				,WB_TOOLBOXBTN_SPEC_HEIGHT
				,SWP_SHOWWINDOW	);
		}
	}
	else
	{
		CRect WBDLGrect;
		GetClientRect(&WBDLGrect);
		if (WBDLGrect.Height() != 0)
		{
			int WBHeight = WBDLGrect.Height();
			m_nToolNum = (int)(WBHeight - WB_TOOLBARBTN_SPACE*11 - WB_CTRLBTNDLG_HEIGHT)/WB_TOOLBARBTN_HEIGHT;
			if (g_bU7NewStyle)
			{
				if (m_nToolNum > 9)
				{
					m_nToolNum = 9;
				}
				else if (m_nToolNum < 3)
				{
					m_nToolNum = 3;
				}
			}
			else
			{
				if (m_nToolNum > 10)
				{
					m_nToolNum = 11;
				}
				else if (m_nToolNum < 3)
				{
					m_nToolNum = 3;
				}
			}
			
			
		}
		if (m_nToolNum != 0)
		{			
			m_toolbarDlg->setToolbarNum(m_nToolNum);
			m_toolbarDlg->ReLayout();
			m_toolbarDlg->SetWindowPos(m_toolbarCtrlDlg	,0,WB_CTRLBTNDLG_HEIGHT ,WB_CTRLBTNDLG_WIDTH
				,WB_TOOLBARBTN_HEIGHT*m_nToolNum + WB_TOOLBARBTN_SPACE*(m_nToolNum + 1)
				,SWP_SHOWWINDOW );
			
// 			HRGN rgnwind = CreateRoundRectRgn(0,0,WB_CTRLBTNDLG_WIDTH,WB_TOOLBARBTN_HEIGHT*m_nToolNum + WB_TOOLBARBTN_SPACE*(m_nToolNum + 1),4,4);
// 			m_toolbarDlg->SetWindowRgn(rgnwind,TRUE);
// 			DeleteObject(rgnwind);
		}
	}
	CRect rt;
	GetClientRect(&rt);
	ShowIsTouchStatic();
			
	//if (m_pWBFuncDlg != NULL)
	//{
	//	if (m_pWBFuncDlg->IsWindowVisible())
	//	{
	//		m_pPageDlg->SetWindowPos(&wndTop
	//			,rt.right - WB_FUNCTION_DLG_WIDTH - WB_OPTIONDLG_WIDTH - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 30)  /*WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70是按钮的宽度，30是页对话框离右边框的距离 */
	//			,rt.top + 10
	//			,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
	//			,WB_TOOLBOXBTN_SPEC_HEIGHT
	//			,SWP_SHOWWINDOW	);
	//	}
	//	else
	//	{
	//		m_pPageDlg->SetWindowPos(&wndTop
	//			,rt.right - WB_OPTIONDLG_WIDTH - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 30)  /*WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70是按钮的宽度，30是页对话框离右边框的距离 */
	//			,rt.top + 10
	//			,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
	//			,WB_TOOLBOXBTN_SPEC_HEIGHT
	//			,SWP_SHOWWINDOW	);
	//	}
	//}
	//else
	//{
	if(g_reduce>0.00000000000000000)//view缩小的时候,调整按钮的显示
					{
						
						double nViewWidth=WB_VIEWHSCROLL*g_reduce;
						double nViewHight=WB_VIEWVSCROLL*g_reduce;
						double nWindowWidth=m_rect.right;
						double nWindowHight=m_rect.bottom;
						int nLeftTopX=0;
						int nRightBottomX=0;
						int nLeftTopY=0;
						int nRightBottomY=0;
						nLeftTopX=(nWindowWidth-nViewWidth)/2;
						nLeftTopY=(nWindowHight-nViewHight)/2;
						nRightBottomX=nWindowWidth-nLeftTopX+5;
						nRightBottomY=nWindowHight-nLeftTopY+5;
						//CWBPageDlg *pPageDlg=((CWhiteBoardDlg*)g_whiteboardDlg)->GetPageDlg();

						if((nViewWidth>=nWindowWidth)&&(nViewHight>=nWindowHight))
						{
							//rc.right -= (WB_FUNCTION_DLG_WIDTH + WB_OPTIONDLG_WIDTH);

							//g_pWhiteBoardView->MoveWindow(0,0,nWindowWidth,nWindowHight);
							m_pPageDlg->MoveWindow(nWindowWidth - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 30), 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
							,WB_TOOLBOXBTN_SPEC_HEIGHT);

						}
						else if((nViewHight<nWindowHight)&&(nViewWidth<nWindowWidth))
						{
							//g_pWhiteBoardView->MoveWindow(CRect(nLeftTopX,nLeftTopY,nRightBottomX,nRightBottomY));
							m_pPageDlg->MoveWindow(nRightBottomX - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 10),nLeftTopY + 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
							,WB_TOOLBOXBTN_SPEC_HEIGHT);
						}
						else if((nViewHight<nWindowHight)&&(nViewWidth>nWindowWidth))
						{
							//g_pWhiteBoardView->MoveWindow(CRect(0,nLeftTopY,nWindowWidth,nRightBottomY));
							m_pPageDlg->MoveWindow(nWindowWidth - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 30),nLeftTopY + 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
							,WB_TOOLBOXBTN_SPEC_HEIGHT);

						}
						else if((nViewHight>nWindowHight)&&(nViewWidth<nWindowWidth))
						{
							//g_pWhiteBoardView->MoveWindow(CRect(nLeftTopX,0,nRightBottomX,nWindowHight));
							m_pPageDlg->MoveWindow(nRightBottomX - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 10), 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
							,WB_TOOLBOXBTN_SPEC_HEIGHT);
						}
					}
		else{
		m_pPageDlg->SetWindowPos(&wndTop
			,rt.right -30 - (WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 70+ 30 )  /*WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70是按钮的宽度，30是页对话框离右边框的距离 */
			,rt.top + 10
			,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 50
			,WB_TOOLBOXBTN_SPEC_HEIGHT
			,SWP_SHOWWINDOW	);
			}
	//}								
}

void CWhiteBoardDlg::ShowIsTouchStatic()
{
	if (NULL == m_redoDlg||!g_bWBWndIsneedShowToolBtn)
	{
		return;
	}

	CRect rt;
	GetClientRect(&rt);
// 	if (g_bIsSupportMTTouch)
// 	{
// 		m_redoDlg->SetWindowPos(&wndTop
// 			,rt.left + WB_CTRLBTNDLG_WIDTH + 20 /*WB_CTRLBTNDLG_WIDTH是工具条的宽度，30是REDO对话框离工具条的距离 */
// 			,rt.top + 10
// 			,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 61
// 			,WB_TOOLBOXBTN_SPEC_HEIGHT
// 			,SWP_SHOWWINDOW	);
// 	}
// 	else
	{
		m_redoDlg->SetWindowPos(&wndTop
			,rt.left + WB_CTRLBTNDLG_WIDTH + 20 /*WB_CTRLBTNDLG_WIDTH是工具条的宽度，30是REDO对话框离工具条的距离 */
			,rt.top + 10
			,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 8
			,WB_TOOLBOXBTN_SPEC_HEIGHT
			,SWP_SHOWWINDOW	);

	}

	
}
void CWhiteBoardDlg::HideShowToolbar()
{
	if (NULL != m_toolbarDlg)
	{ 
		//工具条隐藏的就显示
		int ToolbarMoveCount = (m_rect.bottom - WB_CTRLBTNDLG_HEIGHT)/(WB_TOOLBARHIDESHOW_SPEED * WB_CTRLBTNDLG_HEIGHT) ;//计算工具条往上移动的次数
		CRect ToolbarWinRc,whitebroadWinRc;
		int ToolbarPosY;
		m_toolbarDlg->GetWindowRect(&ToolbarWinRc);
		GetWindowRect(&whitebroadWinRc);
		ToolbarPosY = ToolbarWinRc.top - whitebroadWinRc.top ;             

		if (!m_toolbarDlg->IsWindowVisible())//隐藏工具条
		{
			m_toolbarDlg->ShowWindow(SW_SHOW);
			for(int i = 0; i < ToolbarMoveCount; ++i)
			{
				ToolbarPosY += WB_TOOLBARHIDESHOW_SPEED * WB_TOOLBARBTN_HEIGHT;                      //每次往下移WB_TOOLBARHIDESHOW_SPEED个按钮的位置
				if ( ToolbarPosY > (whitebroadWinRc.top + WB_CTRLBTNDLG_HEIGHT))
				{
					break;
				}
				if (WB_SINGLERVER == g_WBType )
				{
					m_toolbarDlg->MoveWindow(ToolbarWinRc.left - whitebroadWinRc.left   
						,ToolbarPosY
						,WB_CTRLBTNDLG_WIDTH
						//,m_rect.bottom - WB_CTRLBTNDLG_HEIGHT -19); //减去水平滚动条的宽度19
						,WB_TOOLBARBTN_HEIGHT*11 + WB_TOOLBARBTN_SPACE*12);

				}
				else
				{
					if (m_nToolNum != 0)
					{
						m_toolbarDlg->MoveWindow(ToolbarWinRc.left - whitebroadWinRc.left   
							,ToolbarPosY
							,WB_CTRLBTNDLG_WIDTH
							//	,m_rect.bottom - WB_CTRLBTNDLG_HEIGHT - 19); //减去水平滚动条的宽度19
							,WB_TOOLBARBTN_HEIGHT*m_nToolNum + WB_TOOLBARBTN_SPACE*(m_nToolNum + 1));
					}
				}
				
				UpdateWindow();
				//Sleep(1);
			}
		}
		else
		{  //工具条显示就隐藏
			m_toolbarDlg->CloseToolbox();
			m_toolbarDlg->CloseAllChildToolBox();
			for(int i = 0; i < ToolbarMoveCount; ++i)
			{
				ToolbarPosY -= WB_TOOLBARHIDESHOW_SPEED * WB_TOOLBARBTN_HEIGHT;  	//每次往上移WB_TOOLBARHIDESHOW_SPEED个按钮的位置
				if (WB_SINGLERVER == g_WBType )
				{
					m_toolbarDlg->MoveWindow(ToolbarWinRc.left - whitebroadWinRc.left   
						,ToolbarPosY
						,WB_CTRLBTNDLG_WIDTH
						//,m_rect.bottom - WB_CTRLBTNDLG_HEIGHT -19); //减去水平滚动条的宽度19
						,WB_TOOLBARBTN_HEIGHT*11 + WB_TOOLBARBTN_SPACE*12);

				}
				else
				{
					if (m_nToolNum != 0)
					{
						m_toolbarDlg->MoveWindow(ToolbarWinRc.left - whitebroadWinRc.left   
							,ToolbarPosY
							,WB_CTRLBTNDLG_WIDTH
							//,m_rect.bottom - WB_CTRLBTNDLG_HEIGHT -19); //减去水平滚动条的宽度19
							,WB_TOOLBARBTN_HEIGHT*m_nToolNum + WB_TOOLBARBTN_SPACE*(m_nToolNum + 1));
					}
				}
				UpdateWindow();
			}
			m_toolbarDlg->ShowWindow(SW_HIDE);	

		}	   

	}	

}

HRESULT CWhiteBoardDlg::CreateResDlg(WPARAM wPar, LPARAM lPar)
{
	if(WBRESWND::IsGalleryWndCreated())//已经创建，直接返回
	{
		return S_FALSE;
	}

	CRect rctView;
	BOOL bGetOK = GetResGalleryRect(rctView);
	if (!bGetOK)
	{
		return S_FALSE;
	}

	HRESULT hr = WBRESWND::CreateGalleryWndEx(g_systemVersion, AfxGetInstanceHandle(), this->m_hWnd, 
		rctView.left, rctView.top, 
		rctView.Width(), rctView.Height(), g_bIsSupportMTTouch);
	WBRESWND::SetMTAuthorizeRes(g_bIsSupportMTTouch);
	ASSERT(SUCCEEDED(hr));
	WBRESWND::MoveGalleryWndEx(HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);	
	//如下为调整其它窗口为相对于资源窗口的上层窗口
	if (g_pWhiteBoardView)
	{
		WBRESWND::RegisterReceivedWnd(g_pWhiteBoardView->GetSafeHwnd());
		CWBFunctionDlg *pDlgFunction =  g_pWhiteBoardView->GetWBFunctionDlg();
		if (pDlgFunction)
		{
			if (pDlgFunction->IsWindowVisible())
			{
				::SetWindowPos(pDlgFunction->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
			}
			else
			{
				::SetWindowPos(pDlgFunction->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_HIDEWINDOW);
			}
		}
	}
	if (m_toolbarCtrlDlg)
	{
		::SetWindowPos(m_toolbarCtrlDlg->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		if (m_toolbarDlg && m_toolbarDlg->IsWindowVisible())
		{
			::SetWindowPos(m_toolbarDlg->GetSafeHwnd(), m_toolbarCtrlDlg->GetSafeHwnd(), 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		}
		if (m_redoDlg)
		{
			::SetWindowPos(m_redoDlg->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		}
		if (m_pPageDlg)
		{
			::SetWindowPos(m_pPageDlg->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		}
	}
	if (m_pWBEagelEye )
	{
		if (m_pWBEagelEye->IsWindowVisible())
		{
			::SetWindowPos(m_pWBEagelEye->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		}
		else
		{
			::SetWindowPos(m_pWBEagelEye->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_HIDEWINDOW);
		}
	}	
	WBRESWND::SetWBResHWndEx(m_pWhiteboard_view->GetSafeHwnd());
	return S_OK;
}

CWBOptionDlg *CWhiteBoardDlg::CreateOptionDlg()
{
	m_pWBOptionDlg = new CWBOptionDlg;
	m_pWBOptionDlg->Create(IDD_WB_OPTIONDLG,this);	
	::SetWindowPos(m_pWBOptionDlg->GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	return m_pWBOptionDlg;
}
CWBFunctionDlg *CWhiteBoardDlg::GetWBFunctionDlg()
{
// 	m_pWBFuncDlg = new CWBFunctionDlg;
// 	m_pWBFuncDlg->Create(IDD_SINWB_FUNCTIONDLG,this);
// 	if (m_pWBEagelEye)
// 	{
// 		//m_pWBFuncDlg->Create(IDD_SINWB_FUNCTIONDLG,GetDlgItem(IDD_EAGLEEYE));
// 		//m_pWBFuncDlg->SetParent(GetDlgItem(IDD_EAGLEEYE));
// 		::SetWindowPos(m_pWBFuncDlg->GetSafeHwnd(), m_pWBEagelEye->GetSafeHwnd(), 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
// 	}
// 	else
// 	{
// 		//m_pWBFuncDlg->Create(IDD_SINWB_FUNCTIONDLG,this);
// 		::SetWindowPos(m_pWBFuncDlg->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
// 	}
// 	if (g_pWhiteBoardView->GetSpotLightHWND())
// 	{
// 		::SetWindowPos(g_pWhiteBoardView->GetSpotLightHWND(), m_pWBFuncDlg->GetSafeHwnd(), 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
// 	}
	return m_pWBFuncDlg;
}

void CWhiteBoardDlg::MoveWBFunctionDlg(BOOL isShow)
{
	CRect rc;

	if (isShow)//显示控制面板和选项卡上对应的按钮
	{
		//GetClientRect(&rc);//获取白板对话框的显示区域
		//m_pWBFuncDlg->MoveWindow(rc.right - WB_FUNCTION_DLG_WIDTH - WB_OPTIONDLG_WIDTH - 20 
		//	,rc.top+ 50
		//	,WB_FUNCTION_DLG_WIDTH
		//	,rc.bottom - rc.top - 265);	
		//m_pWBFuncDlg->ShowWindow(SW_SHOW);
		GetWindowRect(rc);
		m_pWBFuncDlg->SetWindowPos(&CWnd::wndTop,rc.right - WB_FUNCTION_DLG_WIDTH - WB_OPTIONDLG_WIDTH - 20
			,rc.top + 50
			,WB_FUNCTION_DLG_WIDTH
			,rc.bottom - rc.top - 265
			,SWP_SHOWWINDOW);
		//if (m_pWBFuncDlg->GetPreviewDLG() != NULL)//显示预览面板和预览按钮
		//{
		//	if (m_pWBFuncDlg->GetPreviewDLG()->IsWindowVisible())
		//	{
		//		::SendMessage(m_pWBOptionDlg->m_hWnd,WM_SHOWOPTIONBTN,1,0);
		//		if (NULL != m_pWBFuncDlg->GetPreviewDLG())
		//		{
		//			if (m_pWBFuncDlg->GetPreviewDLG()->bLoadPreviews())
		//			{
		//				m_pWBFuncDlg->GetPreviewDLG()->RefurbishPreview();
		//		}
		//		}
		//	}
		//}
		//if (m_pWBFuncDlg->GetAttributeDlg() != NULL)//显示属性面板和属性按钮
		//{
		//	if (m_pWBFuncDlg->GetAttributeDlg()->IsWindowVisible())
		//	{
		//		::SendMessage(m_pWBOptionDlg->m_hWnd,WM_SHOWOPTIONBTN,2,0);
		//	}
		//}
		////if (m_pWBFuncDlg->GetResDlg() != NULL)//显示资源面板和资源按钮
		////{
		////	if (m_pWBFuncDlg->GetResDlg()->IsWindowVisible())
		////	{
		////		::SendMessage(m_pWBOptionDlg->m_hWnd,WM_SHOWOPTIONBTN,3,0);
		////	}
		////}
		//if (m_pWBFuncDlg->GetBackGroundDlg() != NULL)//显示背景面板和背景按钮
		//{
		//	if (m_pWBFuncDlg->GetBackGroundDlg()->IsWindowVisible())
		//	{
		//		::SendMessage(m_pWBOptionDlg->m_hWnd,WM_SHOWOPTIONBTN,4,0);
		//	}
		//}
		//if (m_pWBFuncDlg->GetTeachtoolDlg() != NULL) //显示工具面板和工具按钮
		//{
		//	if (m_pWBFuncDlg->GetTeachtoolDlg()->IsWindowVisible())
		//	{
		//		::SendMessage(m_pWBOptionDlg->m_hWnd,WM_SHOWOPTIONBTN,5,0);
		//	}
		//}
		//把WhiteboardView往左边移动了选项卡对话框的宽度
		//rc.right -= (/*WB_FUNCTION_DLG_WIDTH +*/ WB_OPTIONDLG_WIDTH);
		//m_pWhiteboard_view->MoveWindow(rc);
		//m_pPageDlg->MoveWindow(rc.right - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 30),rc.top + 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
		//	,WB_TOOLBOXBTN_SPEC_HEIGHT);
		if (m_pWBEagelEye != NULL)
		{
			::SendMessage(m_pWBOptionDlg->m_hWnd,WM_SHOWOPTIONBTN,5,0);
		}
		if(g_reduce>0.00000000000000000)
		{
			rc.right=rc.right-(WB_FUNCTION_DLG_WIDTH + WB_OPTIONDLG_WIDTH);
			SetViewPos(rc);
			//SetViewPosWhenFunction(rc);//调整view的位置
		}
		else
		{
			//g_pWhiteBoardView->SetWindowPos((CWnd *)dlg,0,0,1200,700,SWP_SHOWWINDOW);
			
		 }
		rc.right -= (WB_FUNCTION_DLG_WIDTH + WB_OPTIONDLG_WIDTH);
		m_pWhiteboard_view->MoveWindow(rc);
		m_pPageDlg->MoveWindow(rc.right - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 30),rc.top + 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
			,WB_TOOLBOXBTN_SPEC_HEIGHT);
		if (m_pWBEagelEye != NULL && m_pWBEagelEye->IsWindowVisible())
		{
			SetAndShowEagleEye(rc);
		}

		if (WBRESWND::IsGalleryWndCreated())
		{
			CRect rctResWnd;
			BOOL bGet = GetResGalleryRect(rctResWnd);
			if (bGet)
			{
				WBRESWND::MoveGalleryWndEx(0, 0, 0, rctResWnd.Width(), rctResWnd.Height(), SWP_NOMOVE|SWP_NOZORDER);
			}
		}
	} 
	//else
	//{		
	//	GetClientRect(rc);
	//	//rc.right -= 30;
	//	m_pWhiteboard_view->MoveWindow(rc);
	//	if (m_pWBEagelEye != NULL)
	//	{
	//		if (m_pWBEagelEye->IsWindowVisible())
	//		{
	//			SetAndShowEagleEye(rc);
	//		}
	//	}
	//	m_pPageDlg->MoveWindow(rc.right - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 30),rc.top + 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
	//		,WB_TOOLBOXBTN_SPEC_HEIGHT);
	//	m_pWBFuncDlg->ShowWindow(SW_HIDE);
	//	::SendMessage(m_pWBOptionDlg->m_hWnd,WM_SHOWOPTIONBTN,0,0);

	//	if (WBRESWND::IsGalleryWndCreated())
	//	{
	//		CRect rctResWnd;
	//		BOOL bGet = GetResGalleryRect(rctResWnd);
	//		if (bGet)
	//		{
	//			WBRESWND::MoveGalleryWndEx(0, 0, 0, rctResWnd.Width() + 30, rctResWnd.Height(), SWP_NOMOVE|SWP_NOZORDER);
	//		}
	//	}
	//}
	
	
}
void CWhiteBoardDlg::MoveOptionDlg(BOOL isShow)
{
	if (isShow)
	{
		CRect rc;	
		GetWindowRect(rc);
		//GetClientRect(rc);
		m_pWBOptionDlg->MoveWindow(rc.right - WB_OPTIONDLG_WIDTH -1
			,rc.top + 50
			,WB_OPTIONDLG_WIDTH
			,246/*122*/);//四个按钮的情况
		m_pWBOptionDlg->ShowWindow(SW_SHOW);
		//GetWindowRect(rc);
		//m_pWBOptionDlg->SetWindowPos(&CWnd::wndTop,rc.right - WB_OPTIONDLG_WIDTH - 1
		//		,rc.top + 50
		//		,WB_OPTIONDLG_WIDTH
		//		,246/*122*/,/*SWP_NOSIZE|*/SWP_SHOWWINDOW);//四个按钮的情况
		//	    //,308/*246*/)
	} 
	else
	{		
		if (m_pWBOptionDlg->IsWindowVisible())
		{
			m_pWBOptionDlg->ShowWindow(SW_HIDE);
		}
		
	}
}

void CWhiteBoardDlg::OnDropFiles(HDROP hDropInfo)//2011-9-22实现外部对象拖入式打开---caoyuqin
{
	//int Dicom_Return;     //转换文件后返回的值；
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	wchar_t szFilePathName[_MAX_PATH+1]   =   {0}; //外部拖入的对象的地址
	DragQueryFile(hDropInfo, 0, (LPTSTR)szFilePathName, _MAX_PATH);//得到外部拖入的对象的地址
	m_strCurFilePath.Format(L"%s",szFilePathName);
	CString sExt = PathFindExtension(szFilePathName);
	sExt.MakeLower();

	if ( (sExt == ".bmp") || (sExt == ".jpg") || (sExt == ".gif") || (sExt == ".png") || (sExt == ".dib") 
		|| (sExt == ".tiff") || (sExt == ".jfif") || (sExt == ".ico") || (sExt == ".jpeg") )//拖拽图片进白板时
	{
		g_nDrawStatus = ADDPICTURE;
		g_pWhiteBoardView->SetResFilePath();
		if (::IsWindow(g_pWhiteBoardView->GetSafeHwnd()))
		{
			g_pWhiteBoardView->SetCurrentCursorPos();
			g_pWhiteBoardView->OnMouseMove((UINT)WM_MOUSEMOVE,g_pWhiteBoardView->GetCurrentCursorPos());
		}		
	}
	else if ((sExt == ".doc") || (sExt == ".ppt") || (sExt == ".xls") || (sExt == ".pdf"))//拖拽文档进白板时
	{
		g_nDrawStatus = ADDDOC;
		g_pWhiteBoardView->SetResFilePath();
		if (::IsWindow(g_pWhiteBoardView->GetSafeHwnd()))
		{
			g_pWhiteBoardView->SetCurrentCursorPos();
			g_pWhiteBoardView->OnMouseMove((UINT)WM_MOUSEMOVE,g_pWhiteBoardView->GetCurrentCursorPos());
		}
	}
	else if ((sExt == ".avi") || (sExt == ".wmv") || (sExt == ".mpeg") || (sExt == ".mpg") || (sExt == ".mkv") || (sExt == ".avms") 
	|| (sExt == ".wma") || (sExt == ".rmvb") || (sExt == ".mov") || (sExt == ".vob") || (sExt == ".vm") || (sExt == ".wma")
	|| (sExt == ".rm") || (sExt == ".mp3") || (sExt == ".mp4") || (sExt == ".flv") || (sExt == ".f4v") || (sExt == ".dat"))//拖拽视频进白板时
	{
		g_nDrawStatus = ADDNEWMOVIE;
		g_pWhiteBoardView->SetResFilePath();
		if (::IsWindow(g_pWhiteBoardView->GetSafeHwnd()))
		{
			g_pWhiteBoardView->SetCurrentCursorPos();
			g_pWhiteBoardView->OnMouseMove((UINT)WM_MOUSEMOVE,g_pWhiteBoardView->GetCurrentCursorPos());
		}
	}
	else if ((sExt == ".swf") )//拖拽flash进白板时
	{
		g_nDrawStatus = ADDFLASH;
		g_pWhiteBoardView->SetResFilePath();
		if (::IsWindow(g_pWhiteBoardView->GetSafeHwnd()))
		{
			g_pWhiteBoardView->SetCurrentCursorPos();
			g_pWhiteBoardView->OnMouseMove((UINT)WM_MOUSEMOVE,g_pWhiteBoardView->GetCurrentCursorPos());
		}
	}
	else if ( (sExt == ".dxf") )//拖拽dxf进白板时
	{
		g_nDrawStatus = ADDDXF;
		g_pWhiteBoardView->SetResFilePath();
		if (::IsWindow(g_pWhiteBoardView->GetSafeHwnd()))
		{
			g_pWhiteBoardView->SetCurrentCursorPos();
			g_pWhiteBoardView->OnMouseMove((UINT)WM_MOUSEMOVE,g_pWhiteBoardView->GetCurrentCursorPos());
		}
	}

	fstream files;
	//WCHAR filen[256];
	//::MultiByteToWideChar(CP_ACP,0,szFilePathName, -1,filen, 256);//一个字符串到一个宽字符（unicode）的字符串

	files.open(szFilePathName);
	char cdicom[4];
	files.seekg(128);
	files.read(cdicom,4);
	if (cdicom[0] != 'D'
		||cdicom[1] != 'I'
		||cdicom[2] != 'C'
		||cdicom[3] != 'M')
	{
		//return NO_DICOM_FIEL;
		return;
	}
	else
	{
		g_nDrawStatus = ADDDICOM;
		g_pWhiteBoardView->SetResFilePath();
		if (::IsWindow(g_pWhiteBoardView->GetSafeHwnd()))
		{
			g_pWhiteBoardView->SetCurrentCursorPos();
			g_pWhiteBoardView->OnMouseMove((UINT)WM_MOUSEMOVE,g_pWhiteBoardView->GetCurrentCursorPos());
		}
	}
	::DragFinish(hDropInfo);//释放为实现文件拖拽而分配的内存
	CDialogEx::OnDropFiles(hDropInfo);
}

CString CWhiteBoardDlg::GetCurFilePath()
{
	return m_strCurFilePath;
}

BOOL CWhiteBoardDlg::GetResGalleryRect( CRect &rct )
{
	BOOL bReturn = FALSE;
	if (m_pWhiteboard_view)
	{
		GetClientRect(rct);
		rct.right -= GetSystemMetrics(SM_CXVSCROLL);//上滑动条
// 		if (m_pWBFuncDlg != NULL)
// 		{
// 			if (m_pWBFuncDlg->IsWindowVisible())
// 			{
// 				rct.right -= (WB_OPTIONDLG_WIDTH + WB_FUNCTION_DLG_WIDTH);
// 			}
//  			else
//  			{
//  				rct.right -= WB_OPTIONDLG_WIDTH;
//  			}			
// 		}		
		rct.right -= WB_OPTIONDLG_WIDTH;
		rct.bottom -= GetSystemMetrics(SM_CYHSCROLL);//下滑动条
		rct.top = rct.bottom - 180;//高度
		bReturn = TRUE;
	}
	return bReturn;
}

CWBToolbarCtrlDlg * CWhiteBoardDlg::GetToolbarCtrDlg()
{
	return m_toolbarCtrlDlg;
}

CWBToolbarDlg * CWhiteBoardDlg::GetToolbarDlg()
{
	return m_toolbarDlg;
}

CWBRedoAndUndoDlg * CWhiteBoardDlg::GetRedoAndUndoDlg()
{
	return m_redoDlg;
}

CWBPageDlg * CWhiteBoardDlg::GetPageDlg()
{
	return m_pPageDlg;
}

CWBEagelEye* CWhiteBoardDlg::GetWBEagleEyeDlg()
{
	return m_pWBEagelEye;
}


BOOL CWhiteBoardDlg::SetAndShowEagleEye( CRect rect )
{
	if (g_bU7NewStyle)
	{
		return FALSE;
	}
	if (m_pWBEagelEye == NULL)
	{
		m_pWBEagelEye = new CWBEagelEye();
		if (!m_pWBEagelEye->Create(IDD_EAGLEEYE,this))
		{
			return FALSE;
		}	
	}
	CRect EageleRect;
	m_pWBEagelEye->GetClientRect(&EageleRect);
	int nEagelEyeX = rect.right - EageleRect.Width()-20;
	int nEagelEyeY = rect.bottom - EageleRect.Height()-20;
	m_pWBEagelEye->GetPreviewSingleView()->SetRatio();
	m_pWBEagelEye->GetPreviewSingleView()->SetRectInPreview();
/*	CMultiWBFunctionDlg *pDlgFunction =  g_pWhiteBoardView->GetWBFunctionDlg();*/
	::SetWindowPos(m_pWBEagelEye->GetSafeHwnd(), HWND_TOP, nEagelEyeX, nEagelEyeY, 0, 0, /*SWP_NOMOVE|*/SWP_NOSIZE | SWP_SHOWWINDOW);
	return TRUE;
}

void CWhiteBoardDlg::CreateAboutDlg()
{
	CWBAboutDlg m_pWBAboutDlg;
	m_pWBAboutDlg.DoModal();
}

//根据版本类型决定主视图窗口和控制面板的显示
void CWhiteBoardDlg::MoveCWBView()
{
	CRect rc;
	GetWindowRect(rc);
	ScreenToClient(&rc);//U7-T下转换为客户区坐标，调整VIEW的位置错乱
	//CWhiteboardview 往右移动一个功能选项卡宽
	rc.right -= WB_OPTIONDLG_WIDTH;
	m_pWhiteboard_view->MoveWindow(rc); 

	GetWindowRect(rc);
	ScreenToClient(rc);//U7-T下转换为客户区坐标，调整功能选项卡的位置错乱
	m_pWBOptionDlg->SetWindowPos(&wndTop, rc.right - WB_OPTIONDLG_WIDTH - 1
		, rc.top
		, WB_OPTIONDLG_WIDTH
		, rc.Height()
		, SWP_SHOWWINDOW);

	if (m_pWBEagelEye != NULL)
	{
		SetAndShowEagleEye(m_rect);
	}

	MoveToolbar();

	if (WBRESWND::IsGalleryWndCreated())
	{
		CRect rctResWnd;
		BOOL bGet = GetResGalleryRect(rctResWnd);
		if (bGet)
		{
			WBRESWND::MoveGalleryWndEx(0, 0, 0, rctResWnd.Width(), rctResWnd.Height(), SWP_NOMOVE|SWP_NOZORDER);
		}
	}
}

void CWhiteBoardDlg::DeleteFuncPad()
{
	if (m_pWBFuncDlg != NULL)
	{
		delete m_pWBFuncDlg;
		g_pWhiteBoardView->SetWBfunction(NULL);
		m_pWBFuncDlg = NULL;
	}

	//if (m_pWBOptionDlg != NULL)
	//{
	//	delete m_pWBOptionDlg;
	//	m_pWBOptionDlg = NULL;
	//}
}
CWBFunctionDlg*  CWhiteBoardDlg::GetSinFunctionDlg()
{
	return m_pWBFuncDlg;
}

CWBOptionDlg* CWhiteBoardDlg::GetOptionDlg()
{
	return m_pWBOptionDlg;
}

void CWhiteBoardDlg::SetSinFuncShow(BOOL show)
{
	m_IsCloseScreenDraw = show;
}
BOOL CWhiteBoardDlg::GetSinFuncShow()
{
	return m_IsCloseScreenDraw;
}
void CWhiteBoardDlg::SetSinOptShow(BOOL show)
{
	m_bSinOptDlgShow=show;
}

BOOL CWhiteBoardDlg::GetSinOptShow()
{
	return m_bSinOptDlgShow;
}
void CWhiteBoardDlg::SetViewPos(CRect rc)//调整view 的显示位置
{
	double nViewWidth=WB_VIEWHSCROLL*g_reduce;
	double nViewHight=WB_VIEWVSCROLL*g_reduce;
	double nWindowWidth=rc.right;
	double nWindowHight=rc.bottom;
	int nLeftTopX=0;
	int nRightBottomX=0;
	int nLeftTopY=0;
	int nRightBottomY=0;
	nLeftTopX=(nWindowWidth-nViewWidth)/2;
	nLeftTopY=(nWindowHight-nViewHight)/2;
	nRightBottomX=nWindowWidth-nLeftTopX+4;
	nRightBottomY=nWindowHight-nLeftTopY+4;
	BOOL isSinOptShow=((CWhiteBoardDlg*)g_whiteboardDlg)->GetSinOptShow();
	

	if((nViewWidth>=nWindowWidth)&&(nViewHight>=nWindowHight))
	{
		//rc.right -= (WB_FUNCTION_DLG_WIDTH + WB_OPTIONDLG_WIDTH);
		m_redoDlg->SetWindowPos(&wndTop
			,rc.left+ WB_CTRLBTNDLG_WIDTH + 20 /*WB_CTRLBTNDLG_WIDTH是工具条的宽度，30是REDO对话框离工具条的距离 */
			,rc.top + 10
			,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 8
			,WB_TOOLBOXBTN_SPEC_HEIGHT
			,SWP_SHOWWINDOW	);
		if(isSinOptShow)
		{
			g_pWhiteBoardView->MoveWindow(0,0,nWindowWidth,nWindowHight);//
			m_pPageDlg->MoveWindow(nWindowWidth - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 30/*+30*/), 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
				,WB_TOOLBOXBTN_SPEC_HEIGHT);//调整PageDlg位置
			
		
		}
		else
		{
			g_pWhiteBoardView->MoveWindow(0,0,nWindowWidth+WB_OPTIONDLG_WIDTH,nWindowHight);
			m_pPageDlg->MoveWindow(nWindowWidth - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+1/*+ 30*/), 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
				,WB_TOOLBOXBTN_SPEC_HEIGHT);
			
		}



	}
	if((nViewHight<=nWindowHight)&&(nViewWidth<=nWindowWidth))
	{
		g_pWhiteBoardView->MoveWindow(CRect(nLeftTopX,nLeftTopY,nRightBottomX,nRightBottomY));
		m_pPageDlg->MoveWindow(nRightBottomX - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 10/* 30*/),nLeftTopY + 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
			,WB_TOOLBOXBTN_SPEC_HEIGHT);
		m_redoDlg->SetWindowPos(&wndTop
			,nLeftTopX/* + WB_CTRLBTNDLG_WIDTH */+ 20 /*WB_CTRLBTNDLG_WIDTH是工具条的宽度，30是REDO对话框离工具条的距离 */
			,nLeftTopY + 10
			,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 8
			,WB_TOOLBOXBTN_SPEC_HEIGHT
			,SWP_SHOWWINDOW	);
	}
	if((nViewHight<=nWindowHight)&&(nViewWidth>=nWindowWidth))
	{
		if(isSinOptShow)
		{
			g_pWhiteBoardView->MoveWindow(CRect(0,nLeftTopY,nWindowWidth,nRightBottomY));
			m_pPageDlg->MoveWindow(nWindowWidth - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 30/* 30*/),nLeftTopY + 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
				,WB_TOOLBOXBTN_SPEC_HEIGHT);
			m_redoDlg->SetWindowPos(&wndTop
				,nLeftTopX + WB_CTRLBTNDLG_WIDTH + 20 /*WB_CTRLBTNDLG_WIDTH是工具条的宽度，30是REDO对话框离工具条的距离 */
				,nLeftTopY + 10
				,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 8
				,WB_TOOLBOXBTN_SPEC_HEIGHT
				,SWP_SHOWWINDOW	);


		}
		else
		{
			g_pWhiteBoardView->MoveWindow(CRect(0,nLeftTopY,nWindowWidth+WB_OPTIONDLG_WIDTH,nRightBottomY));
			m_pPageDlg->MoveWindow(nWindowWidth - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+1/*+30*/),nLeftTopY + 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
				,WB_TOOLBOXBTN_SPEC_HEIGHT);
			m_redoDlg->SetWindowPos(&wndTop
				,nLeftTopX + WB_CTRLBTNDLG_WIDTH + 20 /*WB_CTRLBTNDLG_WIDTH是工具条的宽度，30是REDO对话框离工具条的距离 */
				,nLeftTopY + 10
				,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 8
				,WB_TOOLBOXBTN_SPEC_HEIGHT
				,SWP_SHOWWINDOW	);
		}


	}
	if((nViewHight>=nWindowHight)&&(nViewWidth<=nWindowWidth))
	{
		g_pWhiteBoardView->MoveWindow(CRect(nLeftTopX,0,nRightBottomX+15+2,nWindowHight));
		m_pPageDlg->MoveWindow(nRightBottomX - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 10/* 30*/), 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
			,WB_TOOLBOXBTN_SPEC_HEIGHT);
		m_redoDlg->SetWindowPos(&wndTop
			,nLeftTopX +/* WB_CTRLBTNDLG_WIDTH + */20 /*WB_CTRLBTNDLG_WIDTH是工具条的宽度，30是REDO对话框离工具条的距离 */
			,nLeftTopY + 10
			,WB_TOOLBOXBTN_SPEC_WIDTH * 2 + 8
			,WB_TOOLBOXBTN_SPEC_HEIGHT
			,SWP_SHOWWINDOW	);
	}
}
void CWhiteBoardDlg::SetIsOpenScreenDraw( bool IsCloseScreenDraw )
{
	m_IsCloseScreenDraw = IsCloseScreenDraw;
}

CWBFunctionDlg * CWhiteBoardDlg::GetFuntDlg()
{
	if (NULL ==m_pWBFuncDlg)
	{
		if (m_dFirstTime == 0)
		{
			m_dFirstTime = GetTickCount();
			return NULL;
		}
		else
		{
			if ((GetTickCount() - m_dFirstTime) < 3000)
				return NULL;
		}
		
		CRect rc;
		GetWindowRect(rc);
		//GetClientRect(rc);
		m_pWBFuncDlg = new CWBFunctionDlg;
		m_pWBFuncDlg->Create(IDD_SINWB_FUNCTIONDLG,this);
		//::SetWindowPos(m_pWBFuncDlg->GetSafeHwnd(),HWND_TOPMOST,rc.right  
		//	,rc.top + 50
		//	,WB_FUNCTION_DLG_WIDTH
		//	,rc.bottom - rc.top - 265,SWP_HIDEWINDOW | SWP_NOSIZE |SWP_NOMOVE);
		//m_pWBFuncDlg->ShowWindow(SW_HIDE);
// 		m_pWBFuncDlg->SetWindowPos(&CWnd::wndTop,rc.right  
// 			,rc.top + 50
// 			,WB_FUNCTION_DLG_WIDTH
// 			,rc.bottom - rc.top - 219,SWP_SHOWWINDOW|SWP_NOACTIVATE);
// 		m_pWBFuncDlg->ShowWindow(SW_HIDE);
		m_pWBFuncDlg->MoveWindow(rc.right  
			,rc.top + 50
			,WB_FUNCTION_DLG_WIDTH
			,rc.bottom - rc.top - 219,SWP_SHOWWINDOW|SWP_NOACTIVATE);
		m_pWhiteboard_view->SetWBfunction(m_pWBFuncDlg);
	}


	return m_pWBFuncDlg;
}

void CWhiteBoardDlg::DeleteWBFunctionDlg()
{
	if (m_pWBFuncDlg)
	{
		delete m_pWBFuncDlg;
		m_pWBFuncDlg = NULL;
		m_dFirstTime = 0;
	}
}
