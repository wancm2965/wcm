// WBToolboxDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBToolboxDlg.h"
#include "afxdialogex.h"
#include "WBToolbarDlg.h"


// CWBToolboxDlg 对话框
HWND CWBToolboxDlg::g_toolBoxHWND = NULL;
IMPLEMENT_DYNAMIC(CWBToolboxDlg, CDialogEx)

CWBToolboxDlg::CWBToolboxDlg(CWnd* pParent /*=NULL*/,int nNewBtnNum /* = 0*/)
	: CDialogEx(CWBToolboxDlg::IDD, pParent)
	,m_pBitmap(NULL)
{
    m_closeBtn = NULL;
	m_btnA = NULL;
	m_btnB = NULL;
	m_btnC = NULL;
	m_btnD = NULL;
	m_btnE = NULL;
	m_btnF = NULL;
	m_btnG = NULL;
	m_btnH = NULL;
	//m_btnI = NULL;
	m_btnJ = NULL;
	m_btnK = NULL;
	m_pParentDlg = pParent;
	m_nNewBtnnum = nNewBtnNum;
	m_NewbtnA = NULL;
	m_NewbtnB = NULL;
	m_NewbtnC = NULL;
	m_NewbtnD = NULL;
	m_NewbtnE = NULL;
	m_NewbtnF = NULL;
	m_NewbtnG = NULL;
}

CWBToolboxDlg::~CWBToolboxDlg()
{
	if (NULL != m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	}
	if (m_NewbtnA != NULL)
	{
		delete m_NewbtnA;
		m_NewbtnA = NULL;
	}
	if (m_NewbtnB != NULL)
	{
		delete m_NewbtnB;
		m_NewbtnB = NULL;
	}
	if (m_NewbtnC != NULL)
	{
		delete m_NewbtnC;
		m_NewbtnC = NULL;
	}
	if (m_NewbtnD != NULL)
	{
		delete m_NewbtnD;
		m_NewbtnD = NULL;
	}
	if (m_NewbtnE != NULL)
	{
		delete m_NewbtnE;
		m_NewbtnE = NULL;
	}
	if (m_NewbtnF != NULL)
	{
		delete m_NewbtnF;
		m_NewbtnF = NULL;
	}
	if (m_NewbtnG != NULL)
	{
		delete m_NewbtnG;
		m_NewbtnG = NULL;
	}
	if (NULL != m_btnA)
	{
		delete m_btnA;
		m_btnA = NULL;
	}
	if (NULL != m_btnB)
	{
		delete m_btnB;
		m_btnB = NULL;
	}
	if (NULL != m_btnC)
	{
		delete m_btnC;
		m_btnC = NULL;
	}
	if (NULL != m_btnD)
	{
		delete m_btnD;
		m_btnD = NULL;
	}
	if (NULL != m_btnE)
	{
		delete m_btnE;
		m_btnE = NULL;
	}
	if (NULL != m_btnF)
	{
		delete m_btnF;
		m_btnF = NULL;
	}
	if (NULL != m_btnG)
	{
		delete m_btnG;
		m_btnG = NULL;
	}
	if (NULL != m_btnH)
	{
		delete m_btnH;
		m_btnH = NULL;
	}
// 	if (NULL != m_btnI)
// 	{
// 		delete m_btnI;
// 		m_btnI = NULL;
// 	}
	if (NULL != m_btnJ)
	{
		delete m_btnJ;
		m_btnJ = NULL;
	}
	if (NULL != m_btnK)
	{
		delete m_btnK;
		m_btnK = NULL;
	}
	if (NULL != m_closeBtn)
	{
		delete m_closeBtn;
		m_closeBtn = NULL;
	}
	CWBToolboxDlg::g_toolBoxHWND = NULL;

}

void CWBToolboxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WB_TOOLBOXBTNSET, m_setBtn);
	DDX_Control(pDX, IDC_TOOLBOXSTATIC, m_toolboxStatic);
}


BEGIN_MESSAGE_MAP(CWBToolboxDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_COMMAND(IDC_BTN_CLOSE, &CWBToolboxDlg::OnCloseBtn)
	ON_COMMAND(IDC_BTN_A, &CWBToolboxDlg::OnBtnA)
	ON_COMMAND(IDC_BTN_B, &CWBToolboxDlg::OnBtnB)
	ON_COMMAND(IDC_BTN_C, &CWBToolboxDlg::OnBtnC)
	ON_COMMAND(IDC_BTN_D, &CWBToolboxDlg::OnBtnD)
	ON_COMMAND(IDC_BTN_E, &CWBToolboxDlg::OnBtnE)
	ON_COMMAND(IDC_BTN_F, &CWBToolboxDlg::OnBtnF)
	ON_COMMAND(IDC_BTN_G, &CWBToolboxDlg::OnBtnG)
	ON_COMMAND(IDC_BTN_H, &CWBToolboxDlg::OnBtnH)
/*	ON_COMMAND(IDC_BTN_I, &CWBToolboxDlg::OnBtnI)*/
	ON_COMMAND(IDC_BTN_J, &CWBToolboxDlg::OnBtnJ)
	ON_COMMAND(IDC_BTN_K, &CWBToolboxDlg::OnBtnK)
	ON_COMMAND(IDC_BTNTWO, &CWBToolboxDlg::OnNewBtnG)
	ON_COMMAND(IDC_BTNTHREE, &CWBToolboxDlg::OnNewBtnF)
	ON_COMMAND(IDC_BTNFOUR, &CWBToolboxDlg::OnNewBtnE)
	ON_COMMAND(IDC_BTNFIVE, &CWBToolboxDlg::OnNewBtnD)
	ON_COMMAND(IDC_BTNSIX, &CWBToolboxDlg::OnNewBtnC)
	ON_COMMAND(IDC_BTNSEVEN,&CWBToolboxDlg::OnNewBtnB)
	ON_COMMAND(IDC_BTNEIGHT,&CWBToolboxDlg::OnNewBtnA)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CWBToolboxDlg 消息处理程序


void CWBToolboxDlg::OnCancel()
{
	//	CDialogEx::OnCancel();
}


void CWBToolboxDlg::OnOK()
{
	//	CDialogEx::OnOK();
}


BOOL CWBToolboxDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	Relayout();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CWBToolboxDlg::OnEraseBkgnd(CDC* pDC)
{
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\");
	SetSkinImage(strPath + _T("DlgBG.png"));
	return TRUE;
	//return CDialogEx::OnEraseBkgnd(pDC);
}
void CWBToolboxDlg::Relayout()
{
	CString strSelection = _T("ButtonType");
	CRect ToolbarRc;
	m_pParentDlg->GetWindowRect(ToolbarRc);

 	int iType = -1;
	CString btnText = _T("");
	CRect rect(20,20,70,70);//任意给的位置
	int width,height;//设置对话框的宽度和高度
	int toolYBegin , toolXBegin ;    //对话框X,Y轴的起始坐标
	
	width = WB_WIDTH_SPACE * 14 + WB_TOOLBARBTN_WIDTH * 6;
	int nSetheight = 4;
	if (g_bShowFullScreenBtn)
	{
		nSetheight = 5;
	}
	if (m_nNewBtnnum < nSetheight)
	{
		height = WB_WIDTH_SPACE * 9 + WB_TOOLBARBTN_HEIGHT * 3;
	}
	else
	{
		height = WB_WIDTH_SPACE * 9 + WB_TOOLBARBTN_HEIGHT * 4;
	}
	toolXBegin = ToolbarRc.right + 20;
	toolYBegin = ToolbarRc.bottom - height;	
	CRect rectview;
	g_pWhiteBoardView->GetWindowRect(&rectview);
	if (toolYBegin < rectview.top)
	{
		toolYBegin =  rectview.top;
	}
	
	//toolYBegin = 400;

	SetWindowPos(&wndTop,toolXBegin	,toolYBegin	, width	,height	,SWP_SHOWWINDOW);
	HRGN rgnwind = CreateRoundRectRgn(0,0,width,height,6,6);
	SetWindowRgn(rgnwind,TRUE);
	DeleteObject(rgnwind);
	//工具箱静态控件
	m_toolboxStatic.SetWindowPos(&wndTop,WB_WIDTH_SPACE,WB_WIDTH_SPACE,50,WB_TOOLBOXBTN_SPEC_HEIGHT,SWP_SHOWWINDOW);
	btnText =  ReadConfigText(_T("WhiteBoardControlText"),_T("ToolboxDlgCaptionStatic"));
	if (btnText.IsEmpty())
	{
		btnText = _T("工具箱");
	}
	m_toolboxStatic.SetCaption(btnText);
	//关闭按钮
	m_closeBtn = new CTransparentButtonEx;
	m_closeBtn->Create(_T("关闭"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_CLOSE);
	m_closeBtn->SetName(_T("closeBtn"));
	m_closeBtn->SetWindowPos(&wndTop
		,width - WB_WIDTH_SPACE - WB_TOOLBOXBTN_SPEC_HEIGHT
		,WB_WIDTH_SPACE
		,WB_TOOLBOXBTN_SPEC_HEIGHT,WB_TOOLBOXBTN_SPEC_HEIGHT,SWP_SHOWWINDOW);	
	m_closeBtn->SetType(TYPE_TOOLBOX_CLOSE);
	btnText = GetCaptureFromType(TYPE_TOOLBOX_CLOSE);
	m_closeBtn->SetWindowText(btnText);
	
	//其中A,B,C,D,E,F是同一行
    m_btnA = new CTransparentButtonEx;
    m_btnA->Create(_T("截屏"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_A);
	m_btnA->SetName(_T("btnA"));
	m_btnA->SetWindowPos(&wndTop
		,WB_WIDTH_SPACE*2 + 0 *(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)//在工具箱中按钮间隔是WB_WIDTH_SPACE*2
		,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 0 * (WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_HEIGHT)//顶部是一个WB_WIDTH_SPACE高度
		,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
	LPCTSTR strReadType = m_btnA->GetName();
	if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
	{
		m_btnA->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnA->SetWindowText(btnText);
	}			
	else	
	{
	    m_btnA->SetType(TYPE_SCREENSHOTS);
		btnText = GetCaptureFromType(TYPE_SCREENSHOTS);
		m_btnA->SetWindowText(btnText);
	}
	//
    m_btnB = new CTransparentButtonEx;
    m_btnB->Create(_T("回收站"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_B);
	m_btnB->SetName(_T("btnB"));
	m_btnB->SetWindowPos(&wndTop
		,WB_WIDTH_SPACE*2 + 1 *(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
		,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 0 * (WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
	strReadType = m_btnB->GetName();
	if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
	{
		m_btnB->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnB->SetWindowText(btnText);
	}		
	else
	{
		m_btnB->SetType(TYPE_RECYCLE);
		btnText = GetCaptureFromType(TYPE_RECYCLE);
		m_btnB->SetWindowText(btnText);
	}
	
	//
    m_btnC = new CTransparentButtonEx;
    m_btnC->Create(_T("文件工具条"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_C);
	m_btnC->SetName(_T("btnC"));
	m_btnC->SetWindowPos(&wndTop
		,WB_WIDTH_SPACE*2 + 2 *(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
		,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 0 * (WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
	strReadType = m_btnC->GetName();
	if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
	{
        m_btnC->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnC->SetWindowText(btnText);
	}		
	else
	{
		m_btnC->SetType(TYPE_DLG_FILE);
		btnText = GetCaptureFromType(TYPE_DLG_FILE);
		m_btnC->SetWindowText(btnText);
	}
	
	//
	m_btnD = new CTransparentButtonEx;
    m_btnD->Create(_T("鹰眼"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_D);
	m_btnD->SetName(_T("btnD"));
	m_btnD->SetWindowPos(&wndTop
		,WB_WIDTH_SPACE*2 + 3 *(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
		,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 0 * (WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
	strReadType = m_btnD->GetName();
	if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
	{
		m_btnD->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
		m_btnD->SetWindowText(btnText);
	}			
	else
	{
		m_btnD->SetType(TYPE_EAGLE_EYE);
		btnText = GetCaptureFromType(TYPE_EAGLE_EYE);
		m_btnD->SetWindowText(btnText);
	}
	
	//
	m_btnE = new CTransparentButtonEx;
    m_btnE->Create(_T("文本"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_E);
	m_btnE->SetName(_T("btnE"));
	m_btnE->SetWindowPos(&wndTop
		,WB_WIDTH_SPACE*2 + 4 *(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
		,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 0 * (WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
	strReadType = m_btnE->GetName();
	if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
	{
		m_btnE->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnE->SetWindowText(btnText);
	}			
	else
	{
		m_btnE->SetType(TYPE_DLG_T);
		btnText = GetCaptureFromType(TYPE_DLG_T);
		m_btnE->SetWindowText(btnText);
	}			
	
	//
    m_btnF = new CTransparentButtonEx;
    m_btnF->Create(_T("标注"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_F);
	m_btnF->SetName(_T("btnF"));
	m_btnF->SetWindowPos(&wndTop
		,WB_WIDTH_SPACE*2 + 5 *(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
		,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 0 * (WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
	strReadType = m_btnF->GetName();
	if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
	{
		m_btnF->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnF->SetWindowText(btnText);
	}
	else
	{
		m_btnF->SetType(TYPE_DLG_DESKTOPDRAW);
		btnText = GetCaptureFromType(TYPE_DLG_DESKTOPDRAW);
		m_btnF->SetWindowText(btnText);
	}
	
	//其中G,H,I,J,K,L是同一行
	m_btnG = new CTransparentButtonEx;
    m_btnG->Create(_T("工具"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_G);
	m_btnG->SetName(_T("btnG"));
	m_btnG->SetWindowPos(&wndTop
		,WB_WIDTH_SPACE*2 + 0 *(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
		,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1 * (WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
	strReadType = m_btnG->GetName();
	if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
	{
		m_btnG->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnG->SetWindowText(btnText);
	}			
	else
	{
		m_btnG->SetType(TYPE_DLG_TOOL);
		btnText = GetCaptureFromType(TYPE_DLG_TOOL);
		m_btnG->SetWindowText(btnText);
	}			
	
	//
	m_btnH = new CTransparentButtonEx;
    m_btnH->Create(_T("屏幕录制"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_H);
	m_btnH->SetName(_T("btnH"));
	m_btnH->SetWindowPos(&wndTop
		,WB_WIDTH_SPACE*2 + 1 *(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
		,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1 * (WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_HEIGHT)
		,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
	strReadType = m_btnH->GetName();
	if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
	{
		m_btnH->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnH->SetWindowText(btnText);
	}			
	else
	{
		m_btnH->SetType(TYPE_SCREENREC);
		btnText = GetCaptureFromType(TYPE_SCREENREC);
		m_btnH->SetWindowText(btnText);
	}	
	
	//如果是在U7中就将资源库管理隐藏，同时将全屏往前挪一个位置
// 	m_btnI = new CTransparentButtonEx;
// 	m_btnI->Create(_T("资源库管理"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_I);
// 	m_btnI->SetName(_T("btnI"));
// 	m_btnI->SetWindowPos(&wndTop
// 		,WB_WIDTH_SPACE*2 + 2 *(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
// 		,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1 * (WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_HEIGHT)
// 		,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
// 	if ((iType = ReadBtnType(m_btnI->GetName())) != -1)
// 	{
// 		m_btnI->SetType((eType)iType);
// 		btnText = GetCaptureFromType((eType)iType);
// 		m_btnI->SetWindowText(btnText);
// 	}			
// 	else
// 	{
// 		m_btnI->SetType(TYPE_DLG_RESOURCEMANAGE); 
// 		btnText = GetCaptureFromType(TYPE_DLG_RESOURCEMANAGE);
// 		m_btnI->SetWindowText(btnText);
// 	}
	//如果单机版就不需要全屏按钮
	
 	if (WB_SINGLERVER == g_WBType)
 	{
 		CString csINIPath = _T("C:\\Program Files\\avcon\\ExePath.ini");
 		int nIsXPE = ReadCfgInt(csINIPath, _T("AVCON_SELECTED"), _T("AVCONXPE"), 0);
 
 		m_btnJ = new CTransparentButtonEx;
 		m_btnJ->Create(_T("退出程序"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_J);
 		m_btnJ->SetName(_T("btnJ"));
 		m_btnJ->SetWindowPos(&wndTop
 			 		,WB_WIDTH_SPACE*2 + 2 *(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
 			 		,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1 * (WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_HEIGHT)
 			 		,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
		strReadType = m_btnJ->GetName();
 		if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
 		{
 			m_btnJ->SetType((eType)iType);
 			btnText = GetCaptureFromType((eType)iType);
 			m_btnJ->SetWindowText(btnText);
 		}			
 		else
 		{   //读取终端机配置文件，默认路径固定，nIsXPE为1是，为0否
 			CString csINIPath = _T("C:\\Program Files\\avcon\\ExePath.ini");
 			int nIsXPE = ReadCfgInt(csINIPath, _T("AVCON_SELECTED"), _T("AVCONXPE"), 0);
 			if (nIsXPE)
 			{
 				m_btnJ->SetType(TYPE_SINGLE_SHOTDOWN);// 关机
 				btnText = GetCaptureFromType(TYPE_SINGLE_SHOTDOWN);
 				m_btnJ->SetWindowText(btnText);
 			} 
 			else
 			{
 				m_btnJ->SetType(TYPE_SINGLE_QUIT);//退出程序
 				btnText = GetCaptureFromType(TYPE_SINGLE_QUIT);
 				m_btnJ->SetWindowText(btnText);
 			}
 			
 		}
 	}
	else
	{
		if (g_bShowFullScreenBtn)
		{
			m_btnJ = new CTransparentButtonEx;
			m_btnJ->Create(_T("全屏"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_J);
			m_btnJ->SetName(_T("btnJ"));
			m_btnJ->SetWindowPos(&wndTop,
				WB_WIDTH_SPACE*2 + 2 *(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
				,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1 * (WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_HEIGHT)
				,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
			strReadType = m_btnJ->GetName();
			if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
			{
				m_btnJ->SetType((eType)iType);
				btnText = GetCaptureFromType((eType)iType);
				m_btnJ->SetWindowText(btnText);
			}			
			else
			{
				m_btnJ->SetType(TYPE_FULLSCREEN);//全屏
				btnText = GetCaptureFromType(TYPE_FULLSCREEN);
				m_btnJ->SetWindowText(btnText);
			}
		}
		
	}

	if (g_bShowFullScreenBtn)
	{
		if ( m_nNewBtnnum < 4 && m_nNewBtnnum > 0)
		{
			int nshownewnum = m_nNewBtnnum;
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnA = new CTransparentButtonEx;
				m_NewbtnA->Create(_T("调色板"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNEIGHT);
				m_NewbtnA->SetName(_T("btnEight"));
				m_NewbtnA->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 3*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)				
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnA->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnA->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnA->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnA->SetType(TYPE_DLG_COLORPALETTE,true);
					btnText = GetCaptureFromType(TYPE_DLG_COLORPALETTE);
					m_NewbtnA->SetWindowText(btnText);
				}
			}
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnB = new CTransparentButtonEx;			
				m_NewbtnB->Create(_T("小擦"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNSEVEN);
				m_NewbtnB->SetName(_T("btnSeven"));
				m_NewbtnB->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 4*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnB->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnB->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnB->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnB->SetType(TYPE_DLG_RUBBER,true);
					btnText = GetCaptureFromType(TYPE_DLG_RUBBER);
					m_NewbtnB->SetWindowText(btnText);
				}
			}
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnC = new CTransparentButtonEx;			
				m_NewbtnC->Create(_T("视频"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNSIX);
				m_NewbtnC->SetName(_T("btnSix"));
				m_NewbtnC->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 5*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnC->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnC->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnC->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnC->SetType(TYPE_DLG_RUBBER,true);
					btnText = GetCaptureFromType(TYPE_DLG_RUBBER);
					m_NewbtnC->SetWindowText(btnText);
				}
			}
		}
		else
		{
			if (m_nNewBtnnum>0)
			{
				m_NewbtnA = new CTransparentButtonEx;
				m_NewbtnA->Create(_T("调色板"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNEIGHT);
				m_NewbtnA->SetName(_T("btnEight"));
				m_NewbtnA->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 3*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnA->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnA->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnA->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnA->SetType(TYPE_DLG_COLORPALETTE,true);
					btnText = GetCaptureFromType(TYPE_DLG_COLORPALETTE);
					m_NewbtnA->SetWindowText(btnText);
				}

				m_NewbtnB = new CTransparentButtonEx;			
				m_NewbtnB->Create(_T("小擦"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNSEVEN);
				m_NewbtnB->SetName(_T("btnSeven"));
				m_NewbtnB->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 4*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnB->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnB->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnB->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnB->SetType(TYPE_DLG_RUBBER,true);
					btnText = GetCaptureFromType(TYPE_DLG_RUBBER);
					m_NewbtnB->SetWindowText(btnText);
				}


				m_NewbtnC = new CTransparentButtonEx;			
				m_NewbtnC->Create(_T("视频"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNSIX);
				m_NewbtnC->SetName(_T("btnSix"));
				m_NewbtnC->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 5*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnC->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnC->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnC->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnC->SetType(TYPE_DLG_RESOURCEFILE,true);
					btnText = GetCaptureFromType(TYPE_DLG_RESOURCEFILE);
					m_NewbtnC->SetWindowText(btnText);
				}
			}


			int nshownewnum = m_nNewBtnnum - 3;
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnD = new CTransparentButtonEx;			
				m_NewbtnD->Create(_T("矩形"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNFIVE);
				m_NewbtnD->SetName(_T("btnFive"));
				m_NewbtnD->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 0*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 2*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnD->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnD->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnD->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnD->SetType(TYPE_DLG_GRAPHICS,true);
					btnText = GetCaptureFromType(TYPE_DLG_GRAPHICS);
					m_NewbtnD->SetWindowText(btnText);
				}
			}
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnE = new CTransparentButtonEx;			
				m_NewbtnE->Create(_T("线型"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNFOUR);
				m_NewbtnE->SetName(_T("btnFour"));
				m_NewbtnE->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 1*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 2*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnE->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnE->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnE->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnE->SetType(TYPE_DLG_GRAPHICS,true);
					btnText = GetCaptureFromType(TYPE_DLG_GRAPHICS);
					m_NewbtnE->SetWindowText(btnText);
				}
			}
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnF = new CTransparentButtonEx;			
				m_NewbtnF->Create(_T("新建页"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNTHREE);
				m_NewbtnF->SetName(_T("btnThree"));
				m_NewbtnF->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 2*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 2*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnF->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnF->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnF->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnF->SetType(TYPE_PAGE,true);
					btnText = GetCaptureFromType(TYPE_PAGE);
					m_NewbtnF->SetWindowText(btnText);
				}
			}
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnG = new CTransparentButtonEx;			
				m_NewbtnG->Create(_T("删除"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNTWO);
				m_NewbtnG->SetName(_T("btnTwo"));
				m_NewbtnG->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 3*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 2*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnG->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnG->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnG->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnG->SetType(TYPE_DEL,true);
					btnText = GetCaptureFromType(TYPE_DEL);
					m_NewbtnG->SetWindowText(btnText);
				}
			}
		}
	}
	else
	{
		if ( m_nNewBtnnum < 5 && m_nNewBtnnum > 0)
		{
			int nshownewnum = m_nNewBtnnum;
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnA = new CTransparentButtonEx;
				m_NewbtnA->Create(_T("调色板"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNEIGHT);
				m_NewbtnA->SetName(_T("btnEight"));
				m_NewbtnA->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 2*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)				
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnA->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnA->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnA->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnA->SetType(TYPE_DLG_COLORPALETTE,true);
					btnText = GetCaptureFromType(TYPE_DLG_COLORPALETTE);
					m_NewbtnA->SetWindowText(btnText);
				}
			}
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnB = new CTransparentButtonEx;			
				m_NewbtnB->Create(_T("小擦"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNSEVEN);
				m_NewbtnB->SetName(_T("btnSeven"));
				m_NewbtnB->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 3*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnB->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnB->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnB->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnB->SetType(TYPE_DLG_RUBBER,true);
					btnText = GetCaptureFromType(TYPE_DLG_RUBBER);
					m_NewbtnB->SetWindowText(btnText);
				}
			}
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnC = new CTransparentButtonEx;			
				m_NewbtnC->Create(_T("视频"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNSIX);
				m_NewbtnC->SetName(_T("btnSix"));
				m_NewbtnC->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 4*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnC->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnC->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnC->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnC->SetType(TYPE_DLG_RUBBER,true);
					btnText = GetCaptureFromType(TYPE_DLG_RUBBER);
					m_NewbtnC->SetWindowText(btnText);
				}
			}
			if (nshownewnum > 0)
			{
				nshownewnum--;
				m_NewbtnD = new CTransparentButtonEx;			
				m_NewbtnD->Create(_T("矩形"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNFIVE);
				m_NewbtnD->SetName(_T("btnFive"));
				m_NewbtnD->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 5*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnD->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnD->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnD->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnD->SetType(TYPE_DLG_GRAPHICS,true);
					btnText = GetCaptureFromType(TYPE_DLG_GRAPHICS);
					m_NewbtnD->SetWindowText(btnText);
				}
			}
		}
		else
		{
			if (m_nNewBtnnum>0)
			{
				m_NewbtnA = new CTransparentButtonEx;
				m_NewbtnA->Create(_T("调色板"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNEIGHT);
				m_NewbtnA->SetName(_T("btnEight"));
				m_NewbtnA->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 2*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnA->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnA->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnA->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnA->SetType(TYPE_DLG_COLORPALETTE,true);
					btnText = GetCaptureFromType(TYPE_DLG_COLORPALETTE);
					m_NewbtnA->SetWindowText(btnText);
				}

				m_NewbtnB = new CTransparentButtonEx;			
				m_NewbtnB->Create(_T("小擦"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNSEVEN);
				m_NewbtnB->SetName(_T("btnSeven"));
				m_NewbtnB->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 3*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnB->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnB->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnB->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnB->SetType(TYPE_DLG_RUBBER,true);
					btnText = GetCaptureFromType(TYPE_DLG_RUBBER);
					m_NewbtnB->SetWindowText(btnText);
				}


				m_NewbtnC = new CTransparentButtonEx;			
				m_NewbtnC->Create(_T("视频"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNSIX);
				m_NewbtnC->SetName(_T("btnSix"));
				m_NewbtnC->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 4*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnC->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnC->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnC->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnC->SetType(TYPE_DLG_RESOURCEFILE,true);
					btnText = GetCaptureFromType(TYPE_DLG_RESOURCEFILE);
					m_NewbtnC->SetWindowText(btnText);
				}

				
				m_NewbtnD = new CTransparentButtonEx;			
				m_NewbtnD->Create(_T("矩形"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNFIVE);
				m_NewbtnD->SetName(_T("btnFive"));
				m_NewbtnD->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 5*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 1*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnD->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnD->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnD->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnD->SetType(TYPE_DLG_GRAPHICS,true);
					btnText = GetCaptureFromType(TYPE_DLG_GRAPHICS);
					m_NewbtnD->SetWindowText(btnText);
				}
			}


			int nshownewnum = m_nNewBtnnum - 4;
// 			if (nshownewnum>0)
// 			{
// 				nshownewnum--;
// 				m_NewbtnD = new CTransparentButtonEx;			
// 				m_NewbtnD->Create(_T("矩形"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNFIVE);
// 				m_NewbtnD->SetName(_T("btnFive"));
// 				m_NewbtnD->SetWindowPos(&wndTop,
// 					WB_WIDTH_SPACE*2 + 0*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
// 					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 2*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
// 					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
// 					);
// 				strReadType = m_NewbtnD->GetName();
// 				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
// 				{
// 					m_NewbtnD->SetType((eType)iType,true);
// 					btnText = GetCaptureFromType((eType)iType);
// 					m_NewbtnD->SetWindowText(btnText);
// 				}
// 				else
// 				{
// 					m_NewbtnD->SetType(TYPE_DLG_GRAPHICS,true);
// 					btnText = GetCaptureFromType(TYPE_DLG_GRAPHICS);
// 					m_NewbtnD->SetWindowText(btnText);
// 				}
// 			}
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnE = new CTransparentButtonEx;			
				m_NewbtnE->Create(_T("线型"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNFOUR);
				m_NewbtnE->SetName(_T("btnFour"));
				m_NewbtnE->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 1*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 2*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnE->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnE->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnE->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnE->SetType(TYPE_DLG_GRAPHICS,true);
					btnText = GetCaptureFromType(TYPE_DLG_GRAPHICS);
					m_NewbtnE->SetWindowText(btnText);
				}
			}
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnF = new CTransparentButtonEx;			
				m_NewbtnF->Create(_T("新建页"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNTHREE);
				m_NewbtnF->SetName(_T("btnThree"));
				m_NewbtnF->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 2*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 2*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnF->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnF->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnF->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnF->SetType(TYPE_PAGE,true);
					btnText = GetCaptureFromType(TYPE_PAGE);
					m_NewbtnF->SetWindowText(btnText);
				}
			}
			if (nshownewnum>0)
			{
				nshownewnum--;
				m_NewbtnG = new CTransparentButtonEx;			
				m_NewbtnG->Create(_T("删除"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW|WS_CLIPSIBLINGS,rect,this,IDC_BTNTWO);
				m_NewbtnG->SetName(_T("btnTwo"));
				m_NewbtnG->SetWindowPos(&wndTop,
					WB_WIDTH_SPACE*2 + 3*(WB_WIDTH_SPACE*2 + WB_TOOLBARBTN_WIDTH)
					,(WB_WIDTH_SPACE*3 + WB_TOOLBOXBTN_SPEC_HEIGHT) + 2*(WB_WIDTH_SPACE * 2 + WB_TOOLBARBTN_HEIGHT)
					,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW
					);
				strReadType = m_NewbtnG->GetName();
				if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
				{
					m_NewbtnG->SetType((eType)iType,true);
					btnText = GetCaptureFromType((eType)iType);
					m_NewbtnG->SetWindowText(btnText);
				}
				else
				{
					m_NewbtnG->SetType(TYPE_DEL,true);
					btnText = GetCaptureFromType(TYPE_DEL);
					m_NewbtnG->SetWindowText(btnText);
				}
			}
		}
	}
	g_toolBoxHWND = m_hWnd;
	CString strValue = _T("");
	if (m_btnA)
	{
		strValue.Format(_T("%d"),m_btnA->GetType());
		WriteWBConfigPathINI(strSelection,m_btnA->GetName(),strValue);
	}
	if (m_btnB)
	{
		strValue.Format(_T("%d"),m_btnB->GetType());
		WriteWBConfigPathINI(strSelection,m_btnB->GetName(),strValue);
	}
	if (m_btnC)
	{
		strValue.Format(_T("%d"),m_btnC->GetType());
		WriteWBConfigPathINI(strSelection,m_btnC->GetName(),strValue);
	}
	if (m_btnD)
	{
		strValue.Format(_T("%d"),m_btnD->GetType());
		WriteWBConfigPathINI(strSelection,m_btnD->GetName(),strValue);
	}
	if (m_btnE)
	{
		strValue.Format(_T("%d"),m_btnE->GetType());
		WriteWBConfigPathINI(strSelection,m_btnE->GetName(),strValue);
	}
	if (m_btnF)
	{
		strValue.Format(_T("%d"),m_btnF->GetType());
		WriteWBConfigPathINI(strSelection,m_btnF->GetName(),strValue);
	}
	if (m_btnG)
	{
		strValue.Format(_T("%d"),m_btnG->GetType());
		WriteWBConfigPathINI(strSelection,m_btnG->GetName(),strValue);
	}
	if (m_btnH)
	{
		strValue.Format(_T("%d"),m_btnH->GetType());
		WriteWBConfigPathINI(strSelection,m_btnH->GetName(),strValue);
	}
	if (m_btnJ)
	{
		strValue.Format(_T("%d"),m_btnJ->GetType());
		WriteWBConfigPathINI(strSelection,m_btnJ->GetName(),strValue);
	}
	if (m_NewbtnA)
	{
		strValue.Format(_T("%d"),m_NewbtnA->GetType());
		WriteWBConfigPathINI(strSelection,m_NewbtnA->GetName(),strValue);
	}
	if (m_NewbtnB)
	{
		strValue.Format(_T("%d"),m_NewbtnB->GetType());
		WriteWBConfigPathINI(strSelection,m_NewbtnB->GetName(),strValue);
	}
	if (m_NewbtnC)
	{
		strValue.Format(_T("%d"),m_NewbtnC->GetType());
		WriteWBConfigPathINI(strSelection,m_NewbtnC->GetName(),strValue);
	}
	if (m_NewbtnD)
	{
		strValue.Format(_T("%d"),m_NewbtnD->GetType());
		WriteWBConfigPathINI(strSelection,m_NewbtnD->GetName(),strValue);
	}
	if (m_NewbtnE)
	{
		strValue.Format(_T("%d"),m_NewbtnE->GetType());
		WriteWBConfigPathINI(strSelection,m_NewbtnE->GetName(),strValue);
	}
	if (m_NewbtnF)
	{
		strValue.Format(_T("%d"),m_NewbtnF->GetType());
		WriteWBConfigPathINI(strSelection,m_NewbtnF->GetName(),strValue);
	}
	if (m_NewbtnG)
	{
		strValue.Format(_T("%d"),m_NewbtnG->GetType());
		WriteWBConfigPathINI(strSelection,m_NewbtnG->GetName(),strValue);
	}	
}

void CWBToolboxDlg::SetSkinImage(CString strPath)
{
	if (!m_pBitmap)
	{
		//wchar_t strFilename[MAX_PATH];
		CString picturePath = strPath;
		//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);//将CString类型转换成WCHAR
		m_pBitmap = new Bitmap(picturePath);
	}
	else
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
		//wchar_t strFilename[MAX_PATH];
		CString picturePath = strPath;
		//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
		m_pBitmap = new Bitmap(picturePath);
	}

	if (m_pBitmap)
	{
		CRect rect;
		GetClientRect(&rect);
		CClientDC dc(this);
		Graphics graph(dc.GetSafeHdc());
// 		Rect Rt(0, 0, rect.Width(), rect.Height());
// 		graph.DrawImage(m_pBitmap, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);
		int nBmpWidth = m_pBitmap->GetWidth();
		int nBmpHeight = m_pBitmap->GetHeight();

		SolidBrush brush(Color(255, 255, 255));
		graph.FillRectangle(&brush, 0, 0, rect.Width(), rect.Height());

		const int nRoundWidth = 6;
		//绘制四个圆角
		for (int nRow = 0; nRow != 2; ++nRow)
		{
			for (int nColumn = 0; nColumn != 2; ++nColumn)
				graph.DrawImage(m_pBitmap, nRow * (rect.Width() - nRoundWidth), nColumn * (rect.Height() - nRoundWidth),
				nRow * (nBmpWidth - nRoundWidth), nColumn * (nBmpHeight - nRoundWidth), nRoundWidth, nRoundWidth, UnitPixel);
		}
		//绘制上下边框
		for (int i = 0; i != 2; ++i)
		{
			Rect rt(nRoundWidth, i * (rect.Height() - nRoundWidth), rect.Width() - nRoundWidth * 2, nRoundWidth);
			graph.DrawImage(m_pBitmap, rt, nRoundWidth, i * (nBmpHeight - nRoundWidth), 1, nRoundWidth, UnitPixel);
		}
		//绘制左右边框
		for (int i = 0; i != 2; ++i)
		{
			Rect rt(i * (rect.Width() - nRoundWidth), nRoundWidth, nRoundWidth, rect.Height() - nRoundWidth * 2);
			graph.DrawImage(m_pBitmap, rt, i * (nBmpWidth - nRoundWidth), nRoundWidth, nRoundWidth, 1, UnitPixel);
		}
	}
}

CString CWBToolboxDlg::GetCaptureFromType(eType btnEmumType)
{
	CString BtnText;
	switch(btnEmumType)
	{
	case TYPE_SELECT:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_SELECT"));
			if (BtnText.IsEmpty() )
			{
				return _T("选择");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_RECYCLE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_RECYCLE"));
			if (BtnText.IsEmpty() )
			{
				return _T("回收站");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_PAGE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_PAGE"));
			if (BtnText.IsEmpty() )
			{
				return _T("新建页");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_FULLSCREEN:
		{
			
			CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
			if (pToolBar->GetIsFullScreen())
			{
				BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("DIS_FULLSCREEN"));
				if (BtnText.IsEmpty() )
				{
					return _T("取消全屏");
				} 
				else
				{
					return BtnText;
				}				
			} 
			else
			{
				BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_FULLSCREEN"));
				if (BtnText.IsEmpty() )
				{
					return _T("全屏");
				} 
				else
				{
					return BtnText;
				}
			}
		}
		break;
	
	case TYPE_ALLTOOL:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_FULLSCREEN"));
			if (BtnText.IsEmpty() )
			{
				return _T("工具箱");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case TYPE_RESOURCE: 
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_RESOURCE"));
			if (BtnText.IsEmpty() )
			{
				return _T("资源目录");	
			} 
			else
			{
				return BtnText;
			}

		}
		break;
	case TYPE_DEL:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DEL"));
			if (BtnText.IsEmpty() )
			{
				return _T("删除");
			} 
			else
			{
				return BtnText;
			}

		}
		break;
	case TYPE_SCREENSHOTS:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_SCREENSHOTS"));
			if (BtnText.IsEmpty() )
			{
				return _T("截屏");			
			} 
			else
			{
				return BtnText;
			}

		}
		break;
	case TYPE_SCREENREC:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_SCREENREC"));
			if (BtnText.IsEmpty() )
			{
				return _T("屏幕录制");				
			} 
			else
			{
				return BtnText;
			}

		}
		break;
	
	case TYPE_DLG_FILE:   
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DLG_FILE"));
			if (BtnText.IsEmpty() )
			{
				return _T("文件");
			} 
			else
			{
				return BtnText;
			}
			
		}
		break;
	case TYPE_DLG_T:  
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DLG_T"));
			if (BtnText.IsEmpty() )
			{
				return _T("文本");
			} 
			else
			{
				return BtnText;
			}

		}
		break;
	case TYPE_DLG_COLORPALETTE:   
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DLG_COLORPALETTE"));
			if (BtnText.IsEmpty() )
			{
				return _T("调色板");
			} 
			else
			{
				return BtnText;
			}
			
		}
		break;
	case TYPE_DLG_LINETYPE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DLG_LINETYPE"));
			if (BtnText.IsEmpty() )
			{
				return _T("线型");
			} 
			else
			{
				return BtnText;
			}

		}
		break;
	case TYPE_DLG_RUBBER:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DLG_RUBBER"));
			if (BtnText.IsEmpty() )
			{
				return _T("擦子");
			} 
			else
			{
				return BtnText;
			}

		}
		break;
	case TYPE_DLG_GRAPHICS:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DLG_GRAPHICS"));
			if (BtnText.IsEmpty() )
			{
				return _T("图形");
			} 
			else
			{
				return BtnText;
			}

		}
		break;
	case TYPE_DLG_RESOURCEFILE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DLG_RESOURCEFILE"));
			if (BtnText.IsEmpty() )
			{
				return _T("资源文件");
			} 
			else
			{
				return BtnText;
			}

		}
		break;
	case TYPE_DLG_TOOL:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DLG_TOOL"));
			if (BtnText.IsEmpty() )
			{
				return _T("工具");
			} 
			else
			{
				return BtnText;
			}

		}
		break;
	case TYPE_DLG_DESKTOPDRAW:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DLG_DESKTOPDRAW"));
			if (BtnText.IsEmpty() )
			{
				return _T("标注");
			} 
			else
			{
				return BtnText;
			}

		}
		break;
	case TYPE_TOOLBOX_CLOSE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_TOOLBOX_CLOSE"));
			if (BtnText.IsEmpty() )
			{
				return _T("关闭");
			} 
			else
			{
				return BtnText;
			}
		}

	case TYPE_EAGLE_EYE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_EAGLE_EYE"));
			if (BtnText.IsEmpty() )
			{
				return _T("鹰眼");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_SINGLE_NETWORK:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_SINGLE_NETWORK"));
			if (BtnText.IsEmpty() )
			{
				return _T("联网");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_SINGLE_LOCAL:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_SINGLE_LOCAL"));
			if (BtnText.IsEmpty() )
			{
				return _T("本地");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_SINGLE_SHOTDOWN:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_SINGLE_SHOTDOWN"));
			if (BtnText.IsEmpty() )
			{
				return _T("关机");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_SINGLE_QUIT:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_SINGLE_QUIT"));
			if (BtnText.IsEmpty() )
			{
				return _T("退出程序");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_BROADCAST:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_BROADCAST"));
			if (BtnText.IsEmpty())
			{
				return _T("点名");
			}
			else
			{
				return BtnText;
			}
		}
		break;
	default:
		{
			return _T("");
		}
		break;
	}
	
}

void CWBToolboxDlg::ChangButton(CTransparentButtonEx *pDownBtn,int downX,int downY,CTransparentButtonEx * pUpBtn,int upX,int upY)
{
	pDownBtn->SetWindowPos(&wndTop,upX,upY,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
	pUpBtn->SetWindowPos(&wndTop,downX,downY,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
}

BOOL CWBToolboxDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CWBToolboxDlg::OnCloseBtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	pToolBar->CloseAllChildToolBox();
	ShowWindow(SW_HIDE);
}


void CWBToolboxDlg::OnBtnA()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_btnA->GetType(),m_btnA);
	}

}


void CWBToolboxDlg::OnBtnB()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_btnB->GetType(),m_btnB);
	}
}


void CWBToolboxDlg::OnBtnC()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_btnC->GetType(),m_btnC);
	}
}


void CWBToolboxDlg::OnBtnD()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_btnD->GetType(),m_btnD);
	}
}


void CWBToolboxDlg::OnBtnE()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_btnE->GetType(),m_btnE);
	}
}


void CWBToolboxDlg::OnBtnF()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_btnF->GetType(),m_btnF);
	}
}


void CWBToolboxDlg::OnBtnG()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_btnG->GetType(),m_btnG);
	}
}


void CWBToolboxDlg::OnBtnH()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_btnH->GetType(),m_btnH);
	}
}


// void CWBToolboxDlg::OnBtnI()
// {
// 	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
// 	if (pToolBar)
// 	{		
// 		pToolBar->CloseAllChildToolBox();
// 		pToolBar->ProcessCmd(m_btnI->GetType(),m_btnI);
// 	}
// }

void CWBToolboxDlg::OnBtnJ()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_btnJ->GetType(),m_btnJ);
	}
}


void CWBToolboxDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

int CWBToolboxDlg::ReadCfgInt( CString strFile, CString strSection, CString strKey, int nDefValue /*= 0*/ )
{
	return GetPrivateProfileInt(strSection, strKey, nDefValue, strFile);
}

void CWBToolboxDlg::OnBtnK()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_btnK->GetType(),m_btnK);
	}
}

void CWBToolboxDlg::OnNewBtnA()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_NewbtnA->GetType(),m_NewbtnA);
	}
}

void CWBToolboxDlg::OnNewBtnB()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_NewbtnB->GetType(),m_NewbtnB);
	}
}

void CWBToolboxDlg::OnNewBtnC()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_NewbtnC->GetType(),m_NewbtnC);
	}
}

void CWBToolboxDlg::OnNewBtnD()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_NewbtnD->GetType(),m_NewbtnD);
	}
}

void CWBToolboxDlg::OnNewBtnE()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_NewbtnE->GetType(),m_NewbtnE);
	}
}

void CWBToolboxDlg::OnNewBtnF()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_NewbtnF->GetType(),m_NewbtnF);
	}
}

void CWBToolboxDlg::OnNewBtnG()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
		pToolBar->ProcessCmd(m_NewbtnG->GetType(),m_NewbtnG);
	}
}

void CWBToolboxDlg::OnLButtonUp( UINT nFlags, CPoint point )
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CWBToolboxDlg::OnRButtonUp( UINT nFlags, CPoint point )
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->CloseAllChildToolBox();
	}

	CDialogEx::OnRButtonUp(nFlags, point);
}
