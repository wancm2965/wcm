
// RegularDllCallDlgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RegularDllCallDlg.h"
#include "RegularDllCallDlgDlg.h"
#include "afxdialogex.h"


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
public:
	afx_msg void On32772();
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

// CRegularDllCallDlgDlg 对话框

CRegularDllCallDlgDlg::CRegularDllCallDlgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRegularDllCallDlgDlg::IDD, pParent)
{
	m_FullMenu = NULL;
}


CRegularDllCallDlgDlg::~CRegularDllCallDlgDlg()
{
}


void CRegularDllCallDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRegularDllCallDlgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_BN_CLICKED(IDC_BUTTON1, &CRegularDllCallDlgDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CRegularDllCallDlgDlg::OnBnClickedCancel)

	
	//ON_WM_MOUSEMOVE()
	ON_COMMAND(IDC_WB_NEWFILEB, &CRegularDllCallDlgDlg::OnWbNewfileb)
	ON_COMMAND(IDC_WB_OPENFILEB, &CRegularDllCallDlgDlg::OnWbOpenfileb)
	ON_COMMAND(IDC_WB_SAVEFILEB, &CRegularDllCallDlgDlg::OnWbSavefileb)
	ON_COMMAND(IDC_WB_RESAVEFILEB, &CRegularDllCallDlgDlg::OnWbResavefileb)
	ON_COMMAND(IDC_WB_PRINTFILEB, &CRegularDllCallDlgDlg::OnWbPrintfileb)
	ON_COMMAND(IDC_WB_CLOSEFILEB, &CRegularDllCallDlgDlg::OnWbClosefileb)

	ON_COMMAND(IDC_WB_NEWPAGEB, &CRegularDllCallDlgDlg::OnWbNewpageb)
	ON_COMMAND(IDC_WB_FOREPAGEB, &CRegularDllCallDlgDlg::OnWbForepageb)
	ON_COMMAND(IDC_WB_NEXTPAGEB, &CRegularDllCallDlgDlg::OnWbNextpageb)
	ON_COMMAND(IDC_WB_FIRSTPAGEB, &CRegularDllCallDlgDlg::OnWbFirstpageb)
	ON_COMMAND(IDC_WB_LASTPAGEB, &CRegularDllCallDlgDlg::OnWbLastpageb)


	ON_COMMAND(IDC_WB_FORELAYERB, &CRegularDllCallDlgDlg::OnWbForelayerb)
	ON_COMMAND(IDC_WB_NEXTLAYERB, &CRegularDllCallDlgDlg::OnWbNextlayerb)
	ON_COMMAND(IDC_WB_TOPLAYERB, &CRegularDllCallDlgDlg::OnWbToplayerb)
	ON_COMMAND(IDC_WB_BOTTOMLAYERB, &CRegularDllCallDlgDlg::OnWbBottomlayerb)
	ON_COMMAND(IDC_WB_LINEB, &CRegularDllCallDlgDlg::OnWbLineb)

	ON_COMMAND(IDC_WB_SELECTB, &CRegularDllCallDlgDlg::OnWbSelectb)
	ON_COMMAND(IDC_WB_CURVEB, &CRegularDllCallDlgDlg::OnWbCurveb)
	ON_COMMAND(IDC_WB_LITTERRUBBERB, &CRegularDllCallDlgDlg::OnWbLitterrubberb)
	ON_COMMAND(IDC_WB_BIGRUBBERB, &CRegularDllCallDlgDlg::OnWbBigrubberb)
	ON_COMMAND(IDC_WB_SINGLEOBJECTRUBBERB, &CRegularDllCallDlgDlg::OnWbSingleobjectrubberb)
	ON_COMMAND(IDC_WB_CURVESELECTRUBBERB, &CRegularDllCallDlgDlg::OnWbCurveselectrubberb)

	ON_COMMAND(IDC_WB_RECTANGLEB, &CRegularDllCallDlgDlg::OnWbRectangleb)
	ON_COMMAND(IDC_WB_ROUNDB, &CRegularDllCallDlgDlg::OnWbRoundb)
	ON_COMMAND(IDC_WB_TRIANGLEB, &CRegularDllCallDlgDlg::OnWbTriangleb)
	ON_COMMAND(IDC_WB_RHOMBUSB, &CRegularDllCallDlgDlg::OnWbRhombusb)
	ON_COMMAND(IDC_WB_POLYGONB, &CRegularDllCallDlgDlg::OnWbPolygonb)
	ON_COMMAND(IDC_WB_CHARFONTB, &CRegularDllCallDlgDlg::OnWbCharfontb)
	ON_COMMAND(IDC_WB_CHARTEXTB, &CRegularDllCallDlgDlg::OnWbChartextb)
	ON_COMMAND(IDC_WB_ANNOTATIONS, &CRegularDllCallDlgDlg::OnWbAnnotationsb)

	ON_COMMAND(IDC_WB_PICTUREB, &CRegularDllCallDlgDlg::OnWbPictureb)
	//ON_COMMAND(IDC_LINEWIDTHB1, &CRegularDllCallDlgDlg::OnLinewidthb1)
	ON_COMMAND(IDC_WB_DOCUMENTB, &CRegularDllCallDlgDlg::OnWbDocumentb)
	ON_COMMAND(IDC_WB_CUTSCREENB, &CRegularDllCallDlgDlg::OnWbCutscreenb)
	ON_COMMAND(IDC_WB_DELETEB, &CRegularDllCallDlgDlg::OnWbDeleteb)
	ON_COMMAND(IDC_WB_UCCB, &CRegularDllCallDlgDlg::OnWbUccb)
	ON_COMMAND(IDC_WB_RESOURCECATAGORYB, &CRegularDllCallDlgDlg::OnWbResourcecatagoryb)
	ON_COMMAND(IDC_RESOURCE_MANGER, &CFullMenuDlg::OnWbResourceManger)
	ON_COMMAND(IDC_WB_FLASHB, &CRegularDllCallDlgDlg::OnWbFlashb)
	
	ON_COMMAND(IDC_WB_FULLSCREENB, &CRegularDllCallDlgDlg::OnWbFullscreenb)
	ON_COMMAND(IDC_WB_UNDOB, &CRegularDllCallDlgDlg::OnWbUndob)
	ON_COMMAND(IDC_WB_REDOB, &CRegularDllCallDlgDlg::OnWbRedob)
	ON_COMMAND(IDC_WB_CLOSEB, &CRegularDllCallDlgDlg::OnWbCloseb)

	ON_COMMAND(IDC_WB_RECScreen, &CRegularDllCallDlgDlg::OnRecscreen)
	ON_COMMAND(ID_WB_RECBck, &CRegularDllCallDlgDlg::OnWbRecbck)
	ON_WM_CLOSE()
	ON_COMMAND(IDC_WB_FUNCTIONPANELB, &CRegularDllCallDlgDlg::OnWbFunctionpanelb)
	//ON_COMMAND(IDC_WB_INSERTPAGEB, &CRegularDllCallDlgDlg::OnInsertpageb)
	//ON_COMMAND(IDC_WB_DELETEPAGEB, &CRegularDllCallDlgDlg::OnDeletepageb)
	ON_COMMAND(ID_SCREENDRAW, &CRegularDllCallDlgDlg::OnScreenDraw)
	ON_COMMAND(ID_DESTORY, &CRegularDllCallDlgDlg::OnDestory)
	ON_COMMAND(ID_FULLSCREENSHIFT, &CRegularDllCallDlgDlg::OnFullscreenShift)
	ON_COMMAND(ID_GUISESHIFT, &CRegularDllCallDlgDlg::OnGuiseshift)
	ON_COMMAND(ID_CONTROL, &CRegularDllCallDlgDlg::OnCtrlDlg)
	ON_COMMAND(IDC_WB_MEDIA, &CRegularDllCallDlgDlg::OnWbMedia)
	ON_COMMAND(IDC_WB_CAMERA, &CRegularDllCallDlgDlg::OnWbCamera)
	ON_COMMAND(IDC_WB_DUSTBIN, &CRegularDllCallDlgDlg::OnWbDustbin)
	ON_COMMAND(ID_SINVER, &CRegularDllCallDlgDlg::OnSinver)
	ON_COMMAND(ID_MULTIVER, &CRegularDllCallDlgDlg::OnMultiver)
END_MESSAGE_MAP()


// CRegularDllCallDlgDlg 消息处理程序

BOOL CRegularDllCallDlgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	//创建全屏下的对话框
	m_FullMenu = new CFullMenuDlg;
	m_FullMenu->Create(IDD_FULLMENU);
	m_FullMenu->ShowWindow(SW_HIDE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRegularDllCallDlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRegularDllCallDlgDlg::OnPaint()
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
HCURSOR CRegularDllCallDlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// void CRegularDllCallDlgDlg::OnBnClickedButton1()
// {
// 
// 	WB_ShowWhiteBoardDlg( NULL, NULL, &wndTop,CRect(0,0,1200,700),NULL);
// 	GetMenu()->EnableMenuItem(ID_SINVER,MF_DISABLED|MF_BYCOMMAND);
// 	GetMenu()->EnableMenuItem(ID_MULTIVER,MF_DISABLED|MF_BYCOMMAND);
// 	GetMenu()->EnableMenuItem(IDC_BUTTON1,MF_DISABLED|MF_BYCOMMAND);
// 	SetMenu(GetMenu());	
// } 

void CRegularDllCallDlgDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码

	CDialogEx::OnCancel();
}

//画笔属性
// void CRegularDllCallDlgDlg::OnOpenAttributeDlg()
// {  
// 	WB_OpenAttributeDlg();
// }
//新建
void CRegularDllCallDlgDlg::OnWbNewfileb()
{	
	WB_Newfile();
}

//打开文件
void CRegularDllCallDlgDlg::OnWbOpenfileb()
{

	WB_Openfile();
}

//保存
void CRegularDllCallDlgDlg::OnWbSavefileb()
{

	WB_SaveWBFile();
}

//另存
void CRegularDllCallDlgDlg::OnWbResavefileb()
{

	WB_Resavefile();
}

//打印
void CRegularDllCallDlgDlg::OnWbPrintfileb()
{

	WB_Printfile();
}

//关闭
void CRegularDllCallDlgDlg::OnWbClosefileb()
{

	WB_Closefile();
}

//新建页
void CRegularDllCallDlgDlg::OnWbNewpageb()
{  

	WB_SetDrawStatus( DRAW_NORMAL);
	WB_AddNewPage();
	
}

//上一页
void CRegularDllCallDlgDlg::OnWbForepageb()
{

	WB_SetDrawStatus( DRAW_NORMAL);
	WB_PrePage();
	
}

//下一页
void CRegularDllCallDlgDlg::OnWbNextpageb()
{

	WB_SetDrawStatus( DRAW_NORMAL);
	WB_NextPage();

}

//第一页
void CRegularDllCallDlgDlg::OnWbFirstpageb()
{
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_FirstPage();
}

//最后页
void CRegularDllCallDlgDlg::OnWbLastpageb()
{	
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_LastPage();
}

//上一层
void CRegularDllCallDlgDlg::OnWbForelayerb()
{

	WB_SuperLayer();
}

//下一层
void CRegularDllCallDlgDlg::OnWbNextlayerb()
{

	WB_UnderLayer();
}

//顶层
void CRegularDllCallDlgDlg::OnWbToplayerb()
{

	WB_TopLayer();
}

//底层
void CRegularDllCallDlgDlg::OnWbBottomlayerb()
{

	WB_BottomLayer();
}

//直线
void CRegularDllCallDlgDlg::OnWbLineb()
{

	WB_SetDrawStatus( DRAW_LINE);
}

//选择
void CRegularDllCallDlgDlg::OnWbSelectb()
{

	WB_SetDrawStatus( DRAW_NORMAL);
}

//曲线
void CRegularDllCallDlgDlg::OnWbCurveb()
{

	WB_SetDrawStatus( DRAW_POLYLINE);
}

//小橡皮擦
void CRegularDllCallDlgDlg::OnWbLitterrubberb()
{

	WB_SetDrawStatus( DRAW_ERASE);
	WB_SetBrushSize(BRUSH_SMALL);

}

//大橡皮擦
void CRegularDllCallDlgDlg::OnWbBigrubberb()
{

	WB_SetDrawStatus( DRAW_ERASE);
	WB_SetBrushSize(BRUNSH_BIG);
}

//清除
void CRegularDllCallDlgDlg::OnWbSingleobjectrubberb()
{
	
	WB_SetDrawStatus( DRAW_CLEANUP);
}

//区域删除
void CRegularDllCallDlgDlg::OnWbCurveselectrubberb()
{
	
	WB_SetDrawStatus( DRAW_RGNDELETE);
}

//矩形
void CRegularDllCallDlgDlg::OnWbRectangleb()
{

	WB_SetDrawStatus( DRAW_RECT);
}

//圆
void CRegularDllCallDlgDlg::OnWbRoundb()
{

	WB_SetDrawStatus( DRAW_ELLIPSE);
}

//三角形
void CRegularDllCallDlgDlg::OnWbTriangleb()
{

	WB_SetDrawStatus( DRAW_TRIANGLE);
}

//菱形
void CRegularDllCallDlgDlg::OnWbRhombusb()
{
	WB_SetDrawStatus( DRAW_RHOMBUS);
}

//多边形
void CRegularDllCallDlgDlg::OnWbPolygonb()
{

	WB_SetDrawStatus( DRAW_POLYGON);
}

//选择字体
void CRegularDllCallDlgDlg::OnWbCharfontb()
{

	WB_SetCharfont();
}

//插入文本
void CRegularDllCallDlgDlg::OnWbChartextb()
{

	WB_SetDrawStatus( DRAW_TEXT);
}

//插入标注
void CRegularDllCallDlgDlg::OnWbAnnotationsb()
{
	WB_SetDrawStatus(ADD_ANNOTATIONS);
}

//插入图片
void CRegularDllCallDlgDlg::OnWbPictureb()
{

	WB_AddPicture();
}

void CRegularDllCallDlgDlg::OnLinewidthb1()
{
	//MessageBox(_T("你点击了选择擦除按钮"));
}

//插入文档
void CRegularDllCallDlgDlg::OnWbDocumentb()
{

	WB_AddDoc();
}

//截屏
void CRegularDllCallDlgDlg::OnWbCutscreenb()
{

	WB_Cutscreen();
}



//删除
void CRegularDllCallDlgDlg::OnWbDeleteb()
{

	WB_DeleteSelectedObjects();
}
//
void CRegularDllCallDlgDlg::OnWbUccb()
{
//	MessageBox(_T("UCC"));


}

//加载资源库
void CRegularDllCallDlgDlg::OnWbResourcecatagoryb()
{

	WB_OpenResource();
}

//加载资源库管理 
void CRegularDllCallDlgDlg::OnWbResourceManger()
{

	WB_OpenReManger();
}

//加载flash
void CRegularDllCallDlgDlg::OnWbFlashb()
{	

	WB_AddFlash();
}

//全屏
void CRegularDllCallDlgDlg::OnWbFullscreenb()
{

    WB_SetFullScreen( );

	//显示全屏下的菜单对话框
	m_FullMenu->ShowWindow(SW_SHOW);
}


//退出
void CRegularDllCallDlgDlg::OnWbCloseb()
{
	GetMenu()->EnableMenuItem(ID_SINVER,MF_ENABLED|MF_BYCOMMAND);
	GetMenu()->EnableMenuItem(ID_MULTIVER,MF_ENABLED|MF_BYCOMMAND);
	SetMenu(GetMenu());
	WB_Close( );
}

//Undo的时候产生一个弹出菜单
void CRegularDllCallDlgDlg::OnWbUndob()
{   //12-25添加jin
	CMenu PopupMenu;
	if (!PopupMenu.CreatePopupMenu())
	{
		return;
	}

	CStringList *pUndoNameList = new CStringList;

	//获得UNDO列表名称到pUndoNameList中
	WB_GetUndoNameList(pUndoNameList);
	//统计UNDO的总的步数
	m_UndoSetpCount = pUndoNameList->GetCount();
	//添加弹出菜单的菜单项
	PopupMenu.AppendMenu(MF_STRING, IDC_UNDO, _T("撤销"));
	PopupMenu.AppendMenu(MF_SEPARATOR);
	PopupMenu.AppendMenu(MF_STRING, IDC_CANCEL, _T("取消"));
	CString str ;
	POSITION position;
	UINT MenuId = IDC_UNDO1; //菜单项的ID
	if (m_UndoSetpCount <= 10)  //当菜单项小于10的时候
	{
		for (position = pUndoNameList->GetHeadPosition(); NULL != position ;pUndoNameList->GetNext(position))
		{
			str = pUndoNameList->GetAt(position);
			if (""!=str)
			{
				PopupMenu.InsertMenu(1,MF_BYPOSITION|MF_STRING,MenuId,_T(str));
				MenuId++;
			}

		}
	}
	else
	{
		MenuId +=m_UndoSetpCount-11;
		for (position = pUndoNameList->FindIndex(m_UndoSetpCount-10); NULL != position ;pUndoNameList->GetNext(position))
		{
			str = pUndoNameList->GetAt(position);
			if (""!=str)
			{
				PopupMenu.InsertMenu(1,MF_BYPOSITION|MF_STRING,MenuId,_T(str));
				MenuId++;
			}

		}
	}

	CPoint point;
	GetCursorPos(&point);
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	PopupMenu.DestroyMenu();
}

//Redo
void CRegularDllCallDlgDlg::OnWbRedob()
{

	WB_Redo( );
}
//撤销中的具体步骤
BOOL CRegularDllCallDlgDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{   //12-25添加jin
	UINT   nItemID=LOWORD(wParam);
	if(IDC_CANCEL == nItemID)
	{			      
	}
	else if(IDC_UNDO == nItemID) 
	{
	
		WB_Undo(1 );
	}
	//操作时最多可以对前200步通过点击撤销步骤来进行，但200以后的还可以通过单击“撤销”来进行
	else if((nItemID > IDC_CANCEL)&(nItemID < IDC_CANCEL+200))
	{
	
		int needStepCount = m_UndoSetpCount - (nItemID-IDC_CANCEL) ;
		WB_Undo((int)needStepCount);
	}		
	return CDialogEx::OnCommand(wParam, lParam);
}



void CRegularDllCallDlgDlg::OnRecscreen()
{
	
	WB_RECScreen();
}


void CRegularDllCallDlgDlg::OnWbRecbck()
{
	WB_Baffle();
}


void CRegularDllCallDlgDlg::OnClose()
{
	OnWbCloseb();
	CDialogEx::OnClose();
}

void CRegularDllCallDlgDlg::OnWbFunctionpanelb()
{
	WB_FunctionPanel();
}


void CRegularDllCallDlgDlg::OnInsertpageb()
{
	// TODO: 在此添加命令处理程序代码
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_InsertNewPage();
}

void CRegularDllCallDlgDlg::OnDeletepageb()
{
	// TODO: 在此添加命令处理程序代码
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_DeletePage();
}
//屏幕绘图
void CRegularDllCallDlgDlg::OnScreenDraw()
{
	WB_DrawScreen(NULL);
}
//销毁屏幕绘图窗口
void CRegularDllCallDlgDlg::OnDestory()
{
	// TODO: 在此添加命令处理程序代码
	WB_DestroyWnd();
}

//全屏切换传入bool参数，如若传入true则全屏，如若传入false回到全屏前的位置和大小，这里根据得到的当前状态让其实现与当前相反的状态
void CRegularDllCallDlgDlg::OnFullscreenShift()
{
	// TODO: 在此添加命令处理程序代码
	bool isFull=this->IsFullScreen();
	WB_FullScreenShift(!isFull);
}

//切换效果传入bool参数，如若传入true则透明化，如若传入false则玻璃化，这里根据得到的当前状态让其实现与当前相反的状态
void CRegularDllCallDlgDlg::OnGuiseshift()
{
	// TODO: 在此添加命令处理程序代码
	bool Trans=this->IsScreenDrawTrans();
	WB_GuiseShift(!Trans );
}
//得到是否透明，透明返回true，玻璃返回false
bool CRegularDllCallDlgDlg::IsScreenDrawTrans()
{
	return WB_IsScreenDrawTrans();
}
//得到是否全屏，全屏返回true，否则返回false
bool CRegularDllCallDlgDlg::IsFullScreen()
{
	return WB_IsFullScreen();
}
void CRegularDllCallDlgDlg::OnCtrlDlg()
{
	WB_OnCtrlDlg();
}


void CRegularDllCallDlgDlg::OnWbMedia()
{
	// TODO: 在此添加命令处理程序代码
	WB_AddMedia();
}

void CRegularDllCallDlgDlg::OnWbCamera()
{
	// TODO: 在此添加命令处理程序代码
	WB_AddCamera();
}


void CRegularDllCallDlgDlg::OnWbDustbin()
{
	// TODO: 在此添加命令处理程序代码
	//WB_Maqnifier();
	WB_Dustbin();
}


void CRegularDllCallDlgDlg::OnSinver()
{
	// TODO: 在此添加命令处理程序代码
	WB_SetTypeOfWB(WB_SINGLERVER);
	WB_ShowWhiteBoardDlg( NULL, NULL, &wndTop,CRect(0,0,1200,700),NULL);
	GetMenu()->EnableMenuItem(ID_SINVER,MF_DISABLED|MF_BYCOMMAND);
	GetMenu()->EnableMenuItem(ID_MULTIVER,MF_DISABLED|MF_BYCOMMAND);
	SetMenu(GetMenu());
}


void CRegularDllCallDlgDlg::OnMultiver()
{
	// TODO: 在此添加命令处理程序代码
	WB_SetTypeOfWB(WB_MULTIVER);
	WB_ShowWhiteBoardDlg( NULL, NULL, &wndTop,CRect(0,0,1200,700),NULL);
	GetMenu()->EnableMenuItem(ID_SINVER,MF_DISABLED|MF_BYCOMMAND);
	GetMenu()->EnableMenuItem(ID_MULTIVER,MF_DISABLED|MF_BYCOMMAND);
	SetMenu(GetMenu());
}
