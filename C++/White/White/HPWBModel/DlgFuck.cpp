// DlgFuck.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgFuck.h"
#include "DlgToolBar.h"
#include "ResourceDef.h"
// CDlgFuck 对话框
HWND CDlgFuck::g_toolBoxHWND = NULL;
IMPLEMENT_DYNAMIC(CDlgFuck, CDialog)

CDlgFuck::CDlgFuck(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFuck::IDD, pParent)
	, m_pBitmap(NULL)

{
	m_btnClose = NULL;
	m_btnA = NULL;
	m_btnB = NULL;
	m_btnC = NULL;
	m_btnD = NULL;
	m_btnE = NULL;
	m_btnF = NULL;
	m_btnG = NULL;
	m_btnH = NULL;
	m_btnI = NULL;
	m_btnJ = NULL;
	m_btnK = NULL;
	m_btnL = NULL;
	m_btnM = NULL;
	m_btnN = NULL;
	m_btnO = NULL;
}

CDlgFuck::~CDlgFuck()
{
	if(NULL != m_pBitmap)
	{
		delete m_pBitmap;
	}
	if(NULL != m_btnClose)
	{
		delete m_btnClose;
	}
	if(NULL != m_btnA)
	{
		delete m_btnA;
	}
	if(NULL != m_btnB)
	{
		delete m_btnB;
	}
	if(NULL != m_btnC)
	{
		delete m_btnC;
	}
	if(NULL != m_btnD)
	{
		delete m_btnD;
	}
	if(NULL != m_btnE)
	{
		delete m_btnE;
	}
	if(NULL != m_btnF)
	{
		delete m_btnF;
	}
	if(NULL != m_btnG)
	{
		delete m_btnG;
	}
	if(NULL != m_btnH)
	{
		delete m_btnH;
	}
	if(NULL != m_btnI)
	{
		delete m_btnI;
	}
	if(NULL != m_btnJ)
	{
		delete m_btnJ;
	}
	if(NULL != m_btnK)
	{
		delete m_btnK;
	}
	if(NULL != m_btnL)
	{
		delete m_btnL;
	}
	if(NULL != m_btnM)
	{
		delete m_btnM;
	}
	if(NULL != m_btnN)
	{
		delete m_btnN;
	}
	if(NULL != m_btnO)
	{
		delete m_btnO;
	}
}

void CDlgFuck::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	
}


BEGIN_MESSAGE_MAP(CDlgFuck, CDialog)

	ON_BN_CLICKED(IDC_BTN_CLOSE, &CDlgFuck::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_A, &CDlgFuck::OnBnClickedBtnA)
	ON_BN_CLICKED(IDC_BTN_B, &CDlgFuck::OnBnClickedBtnB)
	ON_BN_CLICKED(IDC_BTN_C, &CDlgFuck::OnBnClickedBtnC)
	ON_BN_CLICKED(IDC_BTN_D, &CDlgFuck::OnBnClickedBtnD)
	ON_BN_CLICKED(IDC_BTN_E, &CDlgFuck::OnBnClickedBtnE)
	ON_BN_CLICKED(IDC_BTN_F, &CDlgFuck::OnBnClickedBtnF)
	ON_BN_CLICKED(IDC_BTN_G, &CDlgFuck::OnBnClickedBtnG)
	ON_BN_CLICKED(IDC_BTN_H, &CDlgFuck::OnBnClickedBtnH)
	//ON_BN_CLICKED(IDC_BTN_I, &CDlgFuck::OnBnClickedBtnI)
	//ON_BN_CLICKED(IDC_BTN_J, &CDlgFuck::OnBnClickedBtnJ)
	//ON_BN_CLICKED(IDC_BTN_K, &CDlgFuck::OnBnClickedBtnK)
	//ON_BN_CLICKED(IDC_BTN_M, &CDlgFuck::OnBnClickedBtnM)
	//ON_BN_CLICKED(IDC_BTN_N, &CDlgFuck::OnBnClickedBtnN)
	//ON_BN_CLICKED(IDC_BTN_O, &CDlgFuck::OnBnClickedBtnO)

	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_L, &CDlgFuck::OnBnClickedBtnL)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgFuck 消息处理程序






BOOL CDlgFuck::OnInitDialog()
{

	CDialog::OnInitDialog();
	Relayout();
    CString strPath = _T("");
	strPath = GetAppPath() + _T("\\SKIN\\HPWBModel\\72-72按钮");	
	m_btnClose->SetButtonImage(strPath + _T("\\关闭.png"));
	if (!m_pBitmap)
	{
		m_pBitmap = new Bitmap(strPath);
	}
    if (m_pBitmap)
	{
		CRect rect;
		GetClientRect(&rect);
		CClientDC dc(this);
		Graphics graph(dc.GetSafeHdc());
		Rect Rt(0, 0, rect.Width(), rect.Height());
		graph.DrawImage(m_pBitmap, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);	
	}


	m_ToolTip.Create(this,TTS_ALWAYSTIP | WS_POPUP);
	m_ToolTip.SetDelayTime(500);
	m_ToolTip.SetMaxTipWidth(300);
	//按钮文本,按钮注释
	CString btnText(_T("这是一个按钮"));

	m_btnA->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnA,btnText);
	//
	m_btnB->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnB,btnText);
	//
	m_btnC->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnC,btnText);
	//
	m_btnD->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnD,btnText);
	//
	m_btnE->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnE,btnText);
	//
	m_btnF->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnF,btnText);
	//
	m_btnG->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnG,btnText);
	//
	m_btnH->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnH,btnText);
	
    //
	//m_btnI->GetWindowText(btnText);
	//m_ToolTip.AddTool(m_btnI,btnText);
	////
	//m_btnJ->GetWindowText(btnText);
	//m_ToolTip.AddTool(m_btnJ,btnText);
	////
	//m_btnK->GetWindowText(btnText);
	//m_ToolTip.AddTool(m_btnK,btnText);
	////
	//m_btnL->GetWindowText(btnText);
	//m_ToolTip.AddTool(m_btnL,btnText);
	
	return TRUE;  
	// 异常: OCX 属性页应返回 FALSE
}




void CDlgFuck::Relayout()
{
	int iSpace = 5;
	int iType = -1;
	CString btnText = _T("");
	CRect rc;
	GetParent()->GetWindowRect(rc);
	CRect screenRC(0,0, GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
	CRect rect(20,20,70,70);//任意给的位置
	if (rc.right + WIDTH_TOOLBOX > screenRC.right)
	{
		rc.right = screenRC.right - WIDTH_TOOLBOX;
	}
	if (rc.bottom>screenRC.bottom)
	{
		rc.bottom = screenRC.bottom;
	}
	if ((rc.bottom - HEIGHT_TOOLBOX) < 0 )
	{
		rc.bottom = HEIGHT_TOOLBOX;
	}
	SetWindowPos(&wndTop,rc.right,rc.bottom-HEIGHT_TOOLBOX,WIDTH_TOOLBOX,HEIGHT_TOOLBOX,SWP_SHOWWINDOW);
	//
    m_btnClose = new CTransparentButtonEx;
	m_btnClose->Create(_T("关闭"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_CLOSE);
	m_btnClose->SetWindowPos(&wndTop,WIDTH_TOOLBOX - (WIDTH_SMALLBTN + iSpace),2,WIDTH_SMALLBTN,HEIGHT_SMALLBTN,SWP_SHOWWINDOW);

	int iWidthBtnSpace = (WIDTH_TOOLBOX - COLUMN_NUM*WIDTH_BTN)/(COLUMN_NUM + 1);
	int iHeightBtnSpace = (HEIGHT_TOOLBOX - HEAD_TOOLBOX - ROW_NUM*HEIGHT_BTN)/(ROW_NUM + 1);
    //
    m_btnA = new CTransparentButtonEx;
    m_btnA->Create(_T("重做"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_A);
	m_btnA->SetName(_T("btnA"));
	m_btnA->SetWindowPos(&wndTop,iWidthBtnSpace,HEAD_TOOLBOX + iHeightBtnSpace,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(m_btnA->GetName())) != -1)
	{
		m_btnA->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnA->SetWindowText(btnText);
	}			
	else
	{
	    m_btnA->SetType(TYPE_REDO);
	}
	//
    m_btnB = new CTransparentButtonEx;
    m_btnB->Create(_T("撤销"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_B);
	m_btnB->SetName(_T("btnB"));
	m_btnB->SetWindowPos(&wndTop,iWidthBtnSpace + (WIDTH_BTN + iWidthBtnSpace),HEAD_TOOLBOX + iHeightBtnSpace,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(m_btnB->GetName())) != -1)
	{
		m_btnB->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnB->SetWindowText(btnText);
	}		
	else
	{
		m_btnB->SetType(TYPE_UNDO);
	}
	
	//
    m_btnC = new CTransparentButtonEx;
    m_btnC->Create(_T("删除"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_C);
	m_btnC->SetName(_T("btnC"));
	m_btnC->SetWindowPos(&wndTop,iWidthBtnSpace + 2*(WIDTH_BTN + iWidthBtnSpace),HEAD_TOOLBOX + iHeightBtnSpace,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(m_btnC->GetName())) != -1)
	{
        m_btnC->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnC->SetWindowText(btnText);
	}		
	else
	{
		m_btnC->SetType(TYPE_DEL);
	}
	
	//
	m_btnD = new CTransparentButtonEx;
    m_btnD->Create(_T("全屏"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_D);
	m_btnD->SetName(_T("btnD"));
	m_btnD->SetWindowPos(&wndTop,iWidthBtnSpace + 3*(WIDTH_BTN + iWidthBtnSpace),HEAD_TOOLBOX + iHeightBtnSpace,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(m_btnD->GetName())) != -1)
	{
		m_btnD->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnD->SetWindowText(btnText);
	}			
	else
	{
		m_btnD->SetType(TYPE_FULLSCREEN);
	}
	
	//
	m_btnE = new CTransparentButtonEx;
    m_btnE->Create(_T("文本"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_E);
	m_btnE->SetName(_T("btnE"));
	m_btnE->SetWindowPos(&wndTop,iWidthBtnSpace + 4*(WIDTH_BTN + iWidthBtnSpace),HEAD_TOOLBOX + iHeightBtnSpace,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(m_btnE->GetName())) != -1)
	{
		m_btnE->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnE->SetWindowText(btnText);
	}			
	else
	{
		m_btnE->SetType(TYPE_T);
	}			
	
	//
    m_btnF = new CTransparentButtonEx;
    m_btnF->Create(_T("字体"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_F);
	m_btnF->SetName(_T("btnF"));
	m_btnF->SetWindowPos(&wndTop,iWidthBtnSpace,HEAD_TOOLBOX + iHeightBtnSpace + (HEIGHT_BTN + iHeightBtnSpace),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(m_btnF->GetName())) != -1)
	{
		m_btnF->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnF->SetWindowText(btnText);
	}
	else
	{
		m_btnF->SetType(TYPE_FONT);
	}
	
	//
	m_btnG = new CTransparentButtonEx;
    m_btnG->Create(_T("功能面板"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_G);
	m_btnG->SetName(_T("btnG"));
	m_btnG->SetWindowPos(&wndTop,iWidthBtnSpace + (WIDTH_BTN + iWidthBtnSpace),HEAD_TOOLBOX + iHeightBtnSpace + (HEIGHT_BTN + iHeightBtnSpace),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(m_btnG->GetName())) != -1)
	{
		m_btnG->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnG->SetWindowText(btnText);
	}			
	else
	{
		m_btnG->SetType(TYPE_ATTRIBUTE);
	}			
	
	//
	m_btnH = new CTransparentButtonEx;
    m_btnH->Create(_T("截屏"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_H);
	m_btnH->SetName(_T("btnH"));
	m_btnH->SetWindowPos(&wndTop,iWidthBtnSpace + 2*(WIDTH_BTN + iWidthBtnSpace),HEAD_TOOLBOX + iHeightBtnSpace + (HEIGHT_BTN + iHeightBtnSpace),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(m_btnH->GetName())) != -1)
	{
		m_btnH->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnH->SetWindowText(btnText);
	}			
	else
	{
		m_btnH->SetType(TYPE_SCREENSHOTS);
	}	
	/*
	//
	m_btnI = new CTransparentButtonEx;
    m_btnI->Create(_T("放大镜"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_I);
	m_btnI->SetName(_T("btnI"));
	m_btnI->SetWindowPos(&wndTop,iWidthBtnSpace + 3*(WIDTH_BTN + iWidthBtnSpace),HEAD_TOOLBOX + iHeightBtnSpace + (HEIGHT_BTN + iHeightBtnSpace),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(m_btnI->GetName())) != -1)
	{
		m_btnI->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnI->SetWindowText(btnText);
	}			
	else
	{
		m_btnI->SetType(TYPE_MAQN); //放大镜
	}
	//
	m_btnJ = new CTransparentButtonEx;
    m_btnJ->Create(_T("聚光灯"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_J);
	m_btnJ->SetName(_T("btnJ"));
	m_btnJ->SetWindowPos(&wndTop,iWidthBtnSpace + 4*(WIDTH_BTN + iWidthBtnSpace),HEAD_TOOLBOX + iHeightBtnSpace + (HEIGHT_BTN + iHeightBtnSpace),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(m_btnJ->GetName())) != -1)
	{
		m_btnJ->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnJ->SetWindowText(btnText);
	}			
	else
	{
		m_btnJ->SetType(TYPE_SPOTLIGHT);//聚光灯
	}
	//
	m_btnK = new CTransparentButtonEx;
    m_btnK->Create(_T("桌面绘图"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_K);
	m_btnK->SetName(_T("btnK"));
	m_btnK->SetWindowPos(&wndTop,iWidthBtnSpace,HEAD_TOOLBOX + iHeightBtnSpace + 2*(HEIGHT_BTN + iHeightBtnSpace),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(m_btnK->GetName())) != -1)
	{
		m_btnK->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnK->SetWindowText(btnText);
	}			
	else
	{
		m_btnK->SetType(TYPE_DESKTOPDRAW);//框架桌面绘图
	}
	//
	m_btnL = new CTransparentButtonEx;
    m_btnL->Create(_T("功能面板"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN_L);
	m_btnL->SetName(_T("btnL"));
	m_btnL->SetWindowPos(&wndTop,iWidthBtnSpace + (WIDTH_BTN + iWidthBtnSpace),HEAD_TOOLBOX + iHeightBtnSpace + 2*(HEIGHT_BTN + iHeightBtnSpace),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(m_btnL->GetName())) != -1)
	{
		m_btnL->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnL->SetWindowText(btnText);
	}			
	else
	{
		m_btnL->SetType(TYPE_FUNCTIONDLG);//功能面板
	}
	*/
	//m_btnM.SetWindowPos(&wndTop,iWidthBtnSpace + 2*(WIDTH_BTN + iWidthBtnSpace),HEAD_TOOLBOX + iHeightBtnSpace + 2*(HEIGHT_BTN + iHeightBtnSpace),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	//m_btnN.SetWindowPos(&wndTop,iWidthBtnSpace + 3*(WIDTH_BTN + iWidthBtnSpace),HEAD_TOOLBOX + iHeightBtnSpace + 2*(HEIGHT_BTN + iHeightBtnSpace),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	//m_btnO.SetWindowPos(&wndTop,iWidthBtnSpace + 4*(WIDTH_BTN + iWidthBtnSpace),HEAD_TOOLBOX + iHeightBtnSpace + 2*(HEIGHT_BTN + iHeightBtnSpace),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    g_toolBoxHWND = m_hWnd;
}

CString CDlgFuck::GetCaptureFromType(eType btnEmumType)
{
	switch(btnEmumType)
	{
     case TYPE_SELECT:
		{
			return _T("选择");
		}
		break;
	case TYPE_FULLSCREEN:
		{
			return _T("全屏");
		}
		break;
	case TYPE_BIGHT:
		{
			return _T("曲线");
		}
		break;
	case TYPE_LINE:
		{
			return _T("直线");
		}
		break;
	case TYPE_RESOURCE: 
		{
		    return _T("资源目录");	
		}
		break;
	case TYPE_DEL:
		{
            return _T("删除");
		}
		break;
	case TYPE_T:
		{
			return _T("文本");
		}
		break;
	case TYPE_REDO:
		{
			return _T("重做");
		}
		break;
	case TYPE_UNDO:
		{
			return _T("撤销");
		}
		break;
	case TYPE_FONT:
		{
            return _T("字体");
		}
		break;
	case TYPE_ATTRIBUTE:
		{
			return _T("功能面板");
		}
		break;
	case TYPE_SCREENSHOTS:
		{
            return _T("截屏");				
		}
		break;
	case TYPE_DLG_RESOURCE:   
		{

			return _T("资源工具条");
		}
	    break;	
    case TYPE_DLG_RUBBER:    
		{
			return _T("擦子工具条"); 
		}
	    break;
	case TYPE_DLG_GRAPHICS:   
		{
			return _T("图形工具条");
		}
	    break;	
    case TYPE_DLG_FILE:   
		{
			return _T("文件工具条");
		}
	    break;
	case TYPE_DLG_PAGE:   
		{
			return _T("资源目录");
		}
	    break;
		/*case TYPE_MAQN:
		{
            return _T("放大镜");				
		}
		break;
	case TYPE_SPOTLIGHT:
		{
            return _T("聚光灯");				
		}
		break;
	case TYPE_DESKTOPDRAW:
		{
            return _T("桌面绘图");				
		}
		break;
	case TYPE_FUNCTIONDLG:   
		{
			return _T("功能面板");
		}
	    break;*/
	default:
		break;
	}
}

void CDlgFuck::OnBnClickedBtnClose()
{
	ShowWindow(SW_HIDE);

}
void CDlgFuck::SetParentWnd(CWnd* pWnd)
{
	m_pWndParent = pWnd;
}
void CDlgFuck::ChangButton(CTransparentButtonEx *pDownBtn,int downX,int downY,CTransparentButtonEx * pUpBtn,int upX,int upY)
{
	pDownBtn->SetWindowPos(&wndTop,upX,upY,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	pUpBtn->SetWindowPos(&wndTop,downX,downY,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
}
void CDlgFuck::DelAndNewBtn(CTransparentButtonEx *pBtn,int btnX,int btnY,eType btnEmumType,LPCTSTR lpNewCaption)
{
    if(NULL != pBtn)
	{
		CRect rect(20,20,70,70); //没有什么作用
		int BtnID;
		CString strBtnName = pBtn->GetName();//获得按钮名称
        BtnID = pBtn->GetDlgCtrlID();
		pBtn->DestroyWindow();
		delete pBtn;
	    pBtn = new CTransparentButtonEx;
	    pBtn->Create(lpNewCaption,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,BtnID);
	    pBtn->SetWindowPos(&wndTop,btnX,btnY,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
        pBtn->SetType(btnEmumType);
		WriteBtnType(strBtnName,btnEmumType);//写配置文件
		m_ToolTip.AddTool(pBtn,lpNewCaption);
		switch (BtnID)
		{
			 case IDC_BTN_A:  
				{
	               m_btnA = pBtn;
				   m_btnA->SetName(_T("btnA"));
				}
			    break;	
			case IDC_BTN_B:   
				{
					m_btnB = pBtn;
					m_btnB->SetName(_T("btnB"));
				}
				break;
			case IDC_BTN_C:  
				{
					m_btnC = pBtn;
					m_btnC->SetName(_T("btnC"));
				}
			    break;	
			case IDC_BTN_D:   
				{
					m_btnD = pBtn;
					m_btnD->SetName(_T("btnD"));
				}
				break;
			 case IDC_BTN_E:  
				{
					m_btnE = pBtn;
					m_btnE->SetName(_T("btnE"));
				}
			    break;	
			case IDC_BTN_F:   
				{
					m_btnF = pBtn;
					m_btnF->SetName(_T("btnF"));
				}
				break;
			 case IDC_BTN_G:  
				{
				    m_btnG = pBtn;
					m_btnG->SetName(_T("btnG"));
				}
			    break;	
			case IDC_BTN_H:   
				{
					m_btnH = pBtn;
					m_btnH->SetName(_T("btnH"));
				}
				break;
			/*case IDC_BTN_I:   
				{
					m_btnI = pBtn;
					m_btnI->SetName(_T("btnI"));
				}
				break;
			case IDC_BTN_J:   
				{
					m_btnJ = pBtn;
					m_btnJ->SetName(_T("btnJ"));
				}
				break;
			case IDC_BTN_K:   
				{
					m_btnK = pBtn;
					m_btnK->SetName(_T("btnK"));
				}
				break;
			case IDC_BTN_L:   
				{
					m_btnL = pBtn;
					m_btnL->SetName(_T("btnL"));
				}
				break;*/
			default:
				break;
		}		

	}
	
}

void CDlgFuck::OnBnClickedBtnA()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	if (pToolBar)
	{
		eType btnType;
		btnType = m_btnA->GetType();
		if(!((TYPE_REDO == btnType) | (TYPE_UNDO == btnType)))
		{
            pToolBar->CloseAllChildToolBox();
		}
		pToolBar->ProcessCmd(btnType);
	}
}

void CDlgFuck::OnBnClickedBtnB()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	if (pToolBar)
	{
		eType btnType;
		btnType = m_btnB->GetType();
		if(!((TYPE_REDO == btnType) | (TYPE_UNDO == btnType)))
		{
            pToolBar->CloseAllChildToolBox();
		}
		pToolBar->ProcessCmd(btnType);
	}
}

void CDlgFuck::OnBnClickedBtnC()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	if (pToolBar)
	{
		eType btnType;
		btnType = m_btnC->GetType();
		if(!((TYPE_REDO == btnType) | (TYPE_UNDO == btnType)))
		{
            pToolBar->CloseAllChildToolBox();
		}
		pToolBar->ProcessCmd(btnType);
	}
}

void CDlgFuck::OnBnClickedBtnD()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	if (pToolBar)
	{
		eType btnType;
		btnType = m_btnD->GetType();
		if(!((TYPE_REDO == btnType) | (TYPE_UNDO == btnType)))
		{
            pToolBar->CloseAllChildToolBox();
		}
		pToolBar->ProcessCmd(btnType);
	}
}

void CDlgFuck::OnBnClickedBtnE()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	if (pToolBar)
	{
		eType btnType;
		btnType = m_btnE->GetType();
		if(!((TYPE_REDO == btnType) | (TYPE_UNDO == btnType)))
		{
            pToolBar->CloseAllChildToolBox();
		}
		pToolBar->ProcessCmd(btnType);
	}
}

void CDlgFuck::OnBnClickedBtnF()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	if (pToolBar)
	{
		eType btnType;
		btnType = m_btnF->GetType();
		if(!((TYPE_REDO == btnType) | (TYPE_UNDO == btnType)))
		{
            pToolBar->CloseAllChildToolBox();
		}
		pToolBar->ProcessCmd(btnType);
	}
}

void CDlgFuck::OnBnClickedBtnG()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	if (pToolBar)
	{
		eType btnType;
		btnType = m_btnG->GetType();
		if(!((TYPE_REDO == btnType) | (TYPE_UNDO == btnType)))
		{
            pToolBar->CloseAllChildToolBox();
		}
		pToolBar->ProcessCmd(btnType);
	}
}

void CDlgFuck::OnBnClickedBtnH()
{
	CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	if (pToolBar)
	{
		eType btnType;
		btnType = m_btnH->GetType();
		if(!((TYPE_REDO == btnType) | (TYPE_UNDO == btnType)))
		{
            pToolBar->CloseAllChildToolBox();
		}
		pToolBar->ProcessCmd(btnType);
	}
}


void CDlgFuck::OnBnClickedBtnI()
{
	/*CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	if (pToolBar)
	{
		eType btnType;
		btnType = m_btnI->GetType();
		if(!((TYPE_REDO == btnType) | (TYPE_UNDO == btnType)))
		{
            pToolBar->CloseAllChildToolBox();
		}
		pToolBar->ProcessCmd(btnType);
	}*/
}

void CDlgFuck::OnBnClickedBtnJ()
{
    /*CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	if (pToolBar)
	{
		eType btnType;
		btnType = m_btnJ->GetType();
		if(!((TYPE_REDO == btnType) | (TYPE_UNDO == btnType)))
		{
            pToolBar->CloseAllChildToolBox();
		}
		pToolBar->ProcessCmd(btnType);
	}*/

}

void CDlgFuck::OnBnClickedBtnK()
{ 
	/*CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	if (pToolBar)
	{
		eType btnType;
		btnType = m_btnK->GetType();
		if(!((TYPE_REDO == btnType) | (TYPE_UNDO == btnType)))
		{
            pToolBar->CloseAllChildToolBox();
		}
		pToolBar->ProcessCmd(btnType);
	}*/

}

void CDlgFuck::OnBnClickedBtnL()
{
	/*CDlgToolBar* pToolBar = (CDlgToolBar*)GetParent();
	if (pToolBar)
	{
		eType btnType;
		btnType = m_btnL->GetType();
		if(!((TYPE_REDO == btnType) | (TYPE_UNDO == btnType)))
		{
            pToolBar->CloseAllChildToolBox();
		}
		pToolBar->ProcessCmd(btnType);
	}*/

}
void CDlgFuck::OnBnClickedBtnM()
{

}

void CDlgFuck::OnBnClickedBtnN()
{

}

void CDlgFuck::OnBnClickedBtnO()
{

}

BOOL CDlgFuck::OnEraseBkgnd(CDC* pDC)
{
	CString strPath = _T("");
	strPath = GetAppPath() + _T("\\SKIN\\HPWBModel\\72-72按钮\\TOOLBOX.png");
	if (!m_pBitmap)
	{
		m_pBitmap = new Bitmap(strPath);
	}
    if (m_pBitmap)
	{
		CRect rect;
		GetClientRect(&rect);
		CClientDC dc(this);
		Graphics graph(dc.GetSafeHdc());
		Rect Rt(0, 0, rect.Width(), rect.Height());
		graph.DrawImage(m_pBitmap, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);	
	}
	return true;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CDlgFuck::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
}

BOOL CDlgFuck::PreTranslateMessage(MSG* pMsg)
{
	 m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}
CWnd* CDlgFuck::GetParentWnd()
{
     return m_pWndParent;
}