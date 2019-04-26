
// WBPrintDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WBPrint.h"
#include "WBPrintDlg.h"
#include "afxdialogex.h"
#include <string>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWBPrintDlg 对话框




CWBPrintDlg::CWBPrintDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBPrintDlg::IDD, pParent)
{
	//hWndWhiteBoard = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	
}

void CWBPrintDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWBPrintDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CWBPrintDlg 消息处理程序

BOOL CWBPrintDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//HWND hWnd = CWnd::FindWindow(NULL,_T("AVCONWB"))->GetSafeHwnd();
	if (hWndWhiteBoard)
	{
		::EnableWindow(hWndWhiteBoard, FALSE);

	}
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	print();


	if (hWndWhiteBoard)
	{
		::EnableWindow(hWndWhiteBoard,TRUE);
	}
	CDialogEx::OnCancel();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWBPrintDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWBPrintDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWBPrintDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//lvjie
BOOL CWBPrintDlg:: DeleteDirectory(CString DirName)  
{
	DirName += '\0';
	SHFILEOPSTRUCT FileOp ;
	FileOp.fFlags = FOF_NOCONFIRMATION;
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom = DirName;
	FileOp.pTo = NULL;
	FileOp.wFunc = FO_DELETE;
	BOOL nResult = SHFileOperation(&FileOp);
	return TRUE;

}
void CWBPrintDlg::print()//lvie
{   	

	CFileFind   FindFile; 
	BOOL   FindResult; 
	CString path1;
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH); 
	CString CurrentstrPath = szPath;
	int nPos; 
	nPos = CurrentstrPath.ReverseFind('\\'); 
	CurrentstrPath = CurrentstrPath.Left(nPos);
	//char *pStr =  (LPSTR)(LPCTSTR)strPath ;
	//int s=strPath.GetLength();
	//int s=strlen(pStr);
	//int s2=s-19;
	CString path3;
	//path0= strPath.Mid( 0,s2);
	// AfxMessageBox(path0);
	path3.Format(_T("\\*.bmp"));
	path1=CurrentstrPath+"\\HPDTS\\PrintPicture"+path3;
	FindResult   =   FindFile.FindFile( path1); 
	//AfxMessageBox(FindResult);
	int   ii=0; 
	while(FindResult) 
	{ 
		FindResult   =   FindFile.FindNextFile(); 
		ii++; 
	}
	CString fName,fName1;

	CDC *pdc = GetDC();
	UINT FromPage,ToPage;
	int ScreenX = pdc->GetDeviceCaps(LOGPIXELSX);
	int ScreenY = pdc->GetDeviceCaps(LOGPIXELSY);
	CPrintDialog m_PrintDlg(FALSE,PD_ALLPAGES| PD_USEDEVMODECOPIES  ,NULL);
	FromPage = m_PrintDlg.m_pd.nMinPage = m_PrintDlg.m_pd.nFromPage = 1;
	ToPage = m_PrintDlg.m_pd.nMaxPage = m_PrintDlg.m_pd.nToPage = ii;	
	PRINTDLG   pd; 
	LPDEVMODE   lpmode; 
	if(AfxGetApp()-> GetPrinterDeviceDefaults(&pd))
	{ 
		lpmode=(LPDEVMODE)GlobalLock(pd.hDevMode); 
		if(lpmode) 
		{ 
			lpmode-> dmOrientation=DMORIENT_LANDSCAPE; 
			m_PrintDlg.m_pd.hDevMode = lpmode;
		} 
		GlobalUnlock(pd.hDevMode); 
	}

	if(m_PrintDlg.DoModal() == IDOK)
	{
		CDC pDC;
		if (!pDC.Attach(m_PrintDlg.GetPrinterDC()))
		{
			AfxMessageBox(_T("No printer found!")); 
			return; 

		}
		pDC.m_bPrinting = TRUE; 
		if(m_PrintDlg.m_pd.nFromPage>m_PrintDlg.m_pd.nMinPage)
			FromPage = m_PrintDlg.m_pd.nFromPage;
		if(m_PrintDlg.m_pd.nToPage < m_PrintDlg.m_pd.nMaxPage)
			ToPage =  m_PrintDlg.m_pd.nToPage;
		for(UINT PageContr = FromPage;PageContr <= ToPage; PageContr++)
		{  
			fName1.Format(_T("\\%d.bmp"), PageContr);
			fName=CurrentstrPath+"\\HPDTS\\PrintPicture"+fName1;

			CDC dc;
			dc.CreateCompatibleDC(pdc);
			//从外部文件导入
			HBITMAP b=(HBITMAP)::LoadImage(NULL,fName,IMAGE_BITMAP,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);
			CBitmap bitmap;
			bitmap.Attach(b);
			BITMAP bm;
			bitmap.GetBitmap(&bm);
			dc.SelectObject(&bitmap);
			//	pWBPrintPage = g_pWBFile->GetPage(PageContr);
			int m_PrintX,m_PrintY;
			//通过将图片放入内存DC来达到图片与设备不兼容问题 12-25添加jin
			CRect rect(0,0,2400,1600);
			CDC dcMem;         //以下是输出位图的标括准操作
			CBitmap *pOldBitmap = NULL;
			CBitmap BmpBuf;
			dcMem.CreateCompatibleDC(NULL);
			BmpBuf.CreateCompatibleBitmap(&pDC,rect.Width(),rect.Height());
			pOldBitmap = dcMem.SelectObject(&BmpBuf);
			BITMAP bmpinfo;
			BmpBuf.GetBitmap(&bmpinfo);
			dcMem.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(255, 255, 255));

			if( !1)
			{
				//ratex = ratey =1;
			}
			else
			{
				SetMapMode(pDC,MM_ISOTROPIC);	
				//SetMapMode(*pDC,MM_TEXT);
				m_PrintX = pDC.GetDeviceCaps(LOGPIXELSX);
				m_PrintY = pDC.GetDeviceCaps(LOGPIXELSY);
				int ratex = (int)((double)(m_PrintX)/ScreenX);
				int ratey = (int)((double)(m_PrintY)/ScreenY);

				// 设置
				float showRatex,showRatey;
				CRect viewRect;
				// GetSystemMetrics(SM_CXSCREEN)是获取屏幕水平方向像素点个数 
				//showRage是设置view像素点个数和当前屏幕像素点个数比率
				GetClientRect(&viewRect);
				//showRate = (float)viewRect.Width()/GetSystemMetrics(SM_CXSCREEN);
				showRatex = (float)2400L/GetSystemMetrics(SM_CXSCREEN);
				showRatey=(float)1600L/GetSystemMetrics(SM_CYSCREEN);
				//把设备坐标的原点移动到逻辑坐标的(X, Y)处
				pDC.SetWindowOrg((int)(-80*showRatex),(int)(-63*showRatey));
				//将1个逻辑单位映射成1/(30*ratex*showRate)英寸	
				pDC.SetWindowExt((int)(22*ratex*showRatex),(int)(22*ratey*showRatey));	
				pDC.SetViewportExt(m_PrintX,m_PrintY);


				if (pDC.StartDoc(_T("printimage")) == SP_ERROR)// 启动打印工作 
				{  
					DeleteDC(pDC);
					return ;				//ExitProcess(0);
				}
			}	
			if (b)
			{  
				//pWBPrintPage->Draw(&dcMem);
				::BitBlt(dcMem.m_hDC,0, 0, 2400, 1600,dc, 0, 0, SRCCOPY);
				pDC.BitBlt(0,0,bmpinfo.bmWidth,bmpinfo.bmHeight,&dcMem,0,0,SRCCOPY);
				dcMem.SelectObject(pOldBitmap);	
				BmpBuf.DeleteObject();
				dcMem.DeleteDC(); 
			}
			//
			if(pDC.IsPrinting())
			{               CPrintInfo* pInfo=NULL;
			CRect rectDraw2;
			OnPrint1(pdc,pInfo, rectDraw2,m_PrintX,m_PrintY);

			// pDC.StretchBlt(rectDraw2.left, rectDraw2.top, rectDraw2.Width(), rectDraw2.Height(),&dc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);

			}
			pDC.EndDoc();

		}
		::ReleaseDC(this->m_hWnd, pDC.m_hDC);
		CString picturepath;
		picturepath.Format(_T("\\HPDTS\\PrintPicture"));
		picturepath = CurrentstrPath + picturepath;
		//ss.Format(_T("C:\\a"));
		DeleteDirectory(picturepath);
	}
	else
	{

		return;
	}

}
void CWBPrintDlg::  OnPrint1(CDC* pDC, CPrintInfo* pInfo,CRect &rectDraw1,int print_x,int print_y)
{
	pDC->SetMapMode(MM_ANISOTROPIC); //转化坐标映射模式
	int 
		nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //得到屏幕分辨率的宽和高(我的是1280*1024)
	int 
		nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	CSize 
		size = CSize(nScreenWidth, nScreenHeight);
	pDC->SetWindowExt(size); //设置视窗大小
	int xLogPixPerInch = pDC->GetDeviceCaps(LOGPIXELSX);  
	//得到设备每逻辑英寸的像素数量
	int yLogPixPerInch = pDC->GetDeviceCaps(LOGPIXELSY);
	float fWidth = (float)xLogPixPerInch / 96 ; 
	//得到电脑屏幕映射到视窗大小比率
	float fHeight = (float)yLogPixPerInch / 96; //一般得到的fWidth = fHeight 


	long xExt = (long)(fWidth * size.cx); //得到视窗大小
	long yExt = (long)(fHeight * size.cy) ;
	pDC->SetViewportExt((int)xExt, (int)yExt); //设置视窗大小(由电脑屏幕映射到视窗大小)

	PrintDraw(pDC, pInfo, fWidth, fHeight, rectDraw1,print_x,print_y); 

}
void CWBPrintDlg::  PrintDraw(CDC* pDC, CPrintInfo* pInfo,float fWidth, float fHeight, CRect &rectDraw1,int print_x,int print_y )
{
	CBitmap saveBmp, *pOldBmp = NULL;
	CRect rectDraw; //得到view中要绘图的区域(可能派生自CScrollView,所以不一定小于
	GetClientRect(&rectDraw); //屏幕大小)

	//得到要进行缩放的比例
	float 
		fw = 0.0;
	BOOL 
		bw = FALSE;
	float 
		fh = 0.0;
	BOOL 
		bh = FALSE;
	float 
		fr = 1.0;
	int PrintX = pDC->GetDeviceCaps(LOGPIXELSX);
	int PrintY = pDC->GetDeviceCaps(LOGPIXELSY);	
	if ((long)(fWidth * rectDraw.Width()) > print_x) //如果长度由屏幕映射到打印机后大于打印机的可绘图区域
	{ 
		//(pInfo->m_rectDraw), 则需要进行缩放操作,缩放后不是图形变形
		fw = fWidth * rectDraw.Width() / print_x;
		bw = TRUE;
	}
	if ((long)(fHeight * rectDraw.Height()) >print_y) //同上
	{
		fh = fHeight * rectDraw.Height() / print_y;
		bh = TRUE;
	}
	if (bw || bh) //缩放比率应使纸张能容纳下图片大小,且尽可能的按原图大小显示
		fr = fw > fh ? fw : fh;


	//Recalculate rect of draw
	RecalculateRect(rectDraw, fr, rectDraw1); 

}
void  CWBPrintDlg::RecalculateRect(const CRect& rectSrc, float fRate, CRect& rectOut)
{
	rectOut = rectSrc;
	if (fRate == 0.0)
		return ;


	rectOut.left = (long)((float)rectSrc.left / fRate);
	rectOut.top = (long)((float)rectSrc.top / fRate);
	rectOut.right = (long)((float)rectSrc.right / fRate);
	rectOut.bottom = (long)((float)rectSrc.bottom / fRate);
}


