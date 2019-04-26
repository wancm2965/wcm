// DlgToolBar.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgToolBar.h"
#include "MainDlgWB.h"

// CDlgToolBar �Ի���
HWND CDlgToolBar::g_toolBarHWND = NULL;
IMPLEMENT_DYNAMIC(CDlgToolBar, CDialog)

CDlgToolBar::CDlgToolBar(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgToolBar::IDD, pParent)
	, m_pBitmap(NULL)
	, m_pToolBox(NULL)
	, m_pResourceDlg(NULL)
	, m_pRubber(NULL)
	, m_pGraphics(NULL)
	, m_pFileDlg(NULL)
	, m_pColor(NULL)
	, m_pPage(NULL)
	, m_pDesktopDraw(NULL)
	, m_bFullScreen(false)
{
	m_btnOne = NULL;
	m_btnTwo = NULL;
	m_btnThree = NULL;
	m_btnFour = NULL;
	m_btnFive = NULL;
	m_btnSix = NULL;
	m_btnSeven = NULL;
	m_btnEight = NULL;
	m_btnNine = NULL;
	m_btnTen = NULL;
	//

}

CDlgToolBar::~CDlgToolBar()
{
	if(NULL != m_btnOne)
	{
		delete m_btnOne;
	}
	if(NULL != m_btnTwo)
	{
		delete m_btnTwo;
	}
	if(NULL != m_btnThree)
	{
		delete m_btnThree;
	}
	if(NULL != m_btnFour)
	{
		delete m_btnFour;
	}
	if(NULL != m_btnFive)
	{
		delete m_btnFive;
	}
	if(NULL != m_btnSix)
	{
		delete m_btnSix;
	}
	if(NULL != m_btnSeven)
	{
		delete m_btnSeven;
	}
	if(NULL != m_btnEight)
	{
		delete m_btnEight;
	}
	if(NULL != m_btnNine)
	{
		delete m_btnNine;
	}
	if(NULL != m_btnTen)
	{
		delete m_btnTen;
	}
	if(NULL != m_pBitmap)
	{
		delete m_pBitmap;
	}
		if(NULL != m_pToolBox)
	{
		delete m_pToolBox;
	}
	if(NULL != m_pResourceDlg)
	{
		delete m_pResourceDlg;
	}
	if(NULL != m_pRubber)
	{
		delete m_pRubber;
	}
	if(NULL != m_pGraphics)
	{
		delete m_pGraphics;
	}
	if(NULL != m_pFileDlg)
	{
		delete m_pFileDlg;
	}
	if(NULL != m_pColor)
	{
		delete m_pColor;
	}
	if(NULL != m_pPage)
	{
		delete m_pPage;
	}
	if(NULL != m_pDesktopDraw)
	{
		delete m_pDesktopDraw;
	}
	

}

void CDlgToolBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);


}


BEGIN_MESSAGE_MAP(CDlgToolBar, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN1, &CDlgToolBar::OnBnClickedBtn1)
	ON_BN_CLICKED(IDC_BTN2, &CDlgToolBar::OnBnClickedBtn2)
	ON_BN_CLICKED(IDC_BTN3, &CDlgToolBar::OnBnClickedBtn3)
	ON_BN_CLICKED(IDC_BTN4, &CDlgToolBar::OnBnClickedBtn4)
	ON_BN_CLICKED(IDC_BTN5, &CDlgToolBar::OnBnClickedBtn5)
	ON_BN_CLICKED(IDC_BTN6, &CDlgToolBar::OnBnClickedBtn6)
	ON_BN_CLICKED(IDC_BTN7, &CDlgToolBar::OnBnClickedBtn7)
	ON_BN_CLICKED(IDC_BTN8, &CDlgToolBar::OnBnClickedBtn8)
	ON_BN_CLICKED(IDC_BTN9, &CDlgToolBar::OnBnClickedBtn9)
	ON_BN_CLICKED(IDC_BTN10, &CDlgToolBar::OnBnClickedBtn10)

 //   ON_MESSAGE(CLOSE_TOOBAR_MES, OnCloseToolbarMessage)
	ON_WM_SHOWWINDOW()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()

// CDlgToolBar ��Ϣ�������

BOOL CDlgToolBar::OnInitDialog()
{
	CDialog::OnInitDialog();
    CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	m_foreWindowRt = rc;
	ReLayout();
	//������ʾ
    m_ToolTip.Create(this,TTS_ALWAYSTIP | WS_POPUP);
	m_ToolTip.SetDelayTime(500);
	m_ToolTip.SetMaxTipWidth(300);
	//��ť�ı�
	CString btnText(_T("����һ����ť"));
	//
	m_btnOne->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnOne,btnText);
	//
	m_btnTwo->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnTwo,btnText);
	//
	m_btnThree->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnThree,btnText);
	//
	m_btnFour->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnFour,btnText);
	//
	m_btnFive->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnFive,btnText);
	//
	m_btnSix->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnSix,btnText);
	//
	m_btnSeven->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnSeven,btnText);
	//
	m_btnEight->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnEight,btnText);
	//
	m_btnNine->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnNine,btnText);
	//
	m_btnTen->GetWindowText(btnText);
	m_ToolTip.AddTool(m_btnTen,btnText);

	SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
void CDlgToolBar::SetWBMain(CWnd* pWnd)
{
	m_pWbWnd = pWnd;
}



void CDlgToolBar::ReLayout()
{

	SetWindowPos(&wndTopMost,10,10,WIDTH - SPACE,WIDTH_HEAD + HEIGHT,SWP_SHOWWINDOW);
	int iNumOfBtn = 10;
	int iChilpHeight = (HEIGHT - 8*SPACE - 10*HEIGHT_BTN)/(iNumOfBtn + 1);
    x = (WIDTH - WIDTH_BTN - SPACE)/2;
    CRect rect(20,20,70,70);
	int iType = -1;
	CString btnText = _T("");

    m_btnOne = new CTransparentButtonEx;
	m_btnOne->Create(_T("ѡ��"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN1);
	m_btnOne->SetName(_T("btnOne"));
	m_btnOne->SetWindowPos(&wndTop,x,WIDTH_HEAD + 2*SPACE + iChilpHeight,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
	if ((iType = ReadBtnType(_T("btnOne"))) != -1)//���������ļ������õ�ǰӦ��ʹ��ʲô��ť����������ļ�û�м�¼��ǰ���趨��������Ĭ�ϵ�
	{
		m_btnOne->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
        m_btnOne->SetWindowText(btnText);//���ݵ�ǰ��ö���������ð�ť�ı����ı�
	}
	else
	{
		 m_btnOne->SetType(TYPE_SELECT); //ѡ��
	}
   
    //
	m_btnTwo = new CTransparentButtonEx;
	m_btnTwo->Create(_T("�ļ�������"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN2);
	m_btnTwo->SetName(_T("btnTwo"));
	m_btnTwo->SetWindowPos(&wndTop,x,WIDTH_HEAD + 2*SPACE + iChilpHeight + (HEIGHT_BTN + iChilpHeight),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
   	if ((iType = ReadBtnType(_T("btnTwo"))) != -1)
	{
		m_btnTwo->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
		m_btnTwo->SetWindowText(btnText);
	}
	else
	{
		m_btnTwo->SetType(TYPE_DLG_FILE);;//�ļ�������
	}
	
	//
	m_btnThree = new CTransparentButtonEx;
	m_btnThree->Create(_T("ҳ������"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN3);
	m_btnThree->SetName(_T("btnThree"));
	m_btnThree->SetWindowPos(&wndTop,x,WIDTH_HEAD + 2*SPACE + iChilpHeight + 2*(HEIGHT_BTN + iChilpHeight),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
   	if ((iType = ReadBtnType(_T("btnThree"))) != -1)
	{
		m_btnThree->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
		m_btnThree->SetWindowText(btnText);
	}
	else
	{
		m_btnThree->SetType(TYPE_DLG_PAGE);;//ҳ������
	}
	
	//
	m_btnFour = new CTransparentButtonEx;
	m_btnFour->Create(_T("ֱ��"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN4);
	m_btnFour->SetName(_T("btnFour"));
	m_btnFour->SetWindowPos(&wndTop,x,WIDTH_HEAD + 2*SPACE + iChilpHeight + 3*(HEIGHT_BTN + iChilpHeight),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    if ((iType = ReadBtnType(_T("btnFour"))) != -1)
	{
		m_btnFour->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType); 
		m_btnFour->SetWindowText(btnText);
	}
	else
	{
		m_btnFour->SetType(TYPE_LINE);;//ֱ��
	}  
	
	//
	m_btnFive = new CTransparentButtonEx;
	m_btnFive->Create(_T("����"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN5);
	m_btnFive->SetName(_T("btnFive"));
	m_btnFive->SetWindowPos(&wndTop,x,WIDTH_HEAD + 2*SPACE + iChilpHeight + 4*(HEIGHT_BTN + iChilpHeight),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);   
	if ((iType = ReadBtnType(_T("btnFive"))) != -1)
	{
		m_btnFive->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
		m_btnFive->SetWindowText(btnText);
	}
	else
	{
		m_btnFive->SetType(TYPE_BIGHT);//����
	}
	
	//
	m_btnSix = new CTransparentButtonEx;
	m_btnSix->Create(_T("��Դ������"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN6);
	m_btnSix->SetName(_T("btnSix"));
	m_btnSix->SetWindowPos(&wndTop,x,WIDTH_HEAD + 2*SPACE + iChilpHeight + 5*(HEIGHT_BTN + iChilpHeight),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    if ((iType = ReadBtnType(_T("btnSix"))) != -1)
	{
		m_btnSix->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
		m_btnSix->SetWindowText(btnText);
	}
	else
	{
		m_btnSix->SetType(TYPE_DLG_RESOURCE);  //��Դ������
	}
	
	//
	m_btnSeven = new CTransparentButtonEx;
	m_btnSeven->Create(_T("���ӹ�����"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN7);
	m_btnSeven->SetName(_T("btnSeven"));	
	m_btnSeven->SetWindowPos(&wndTop,x,WIDTH_HEAD + 2*SPACE + iChilpHeight + 6*(HEIGHT_BTN + iChilpHeight),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    if ((iType = ReadBtnType(_T("btnSeven"))) != -1)
	{
		m_btnSeven->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
		m_btnSeven->SetWindowText(btnText);
	}
	else
	{
		m_btnSeven->SetType(TYPE_DLG_RUBBER); //���ӹ�����
	}
	
	//
	m_btnEight = new CTransparentButtonEx;
	m_btnEight->Create(_T("ͼ�ι�����"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN8);
	m_btnEight->SetName(_T("btnEight"));	
	m_btnEight->SetWindowPos(&wndTop,x,WIDTH_HEAD + 2*SPACE + iChilpHeight + 7*(HEIGHT_BTN + iChilpHeight),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    if ((iType = ReadBtnType(_T("btnEight"))) != -1)
	{
		m_btnEight->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
		m_btnEight->SetWindowText(btnText);
	}
	else
	{
		m_btnEight->SetType(TYPE_DLG_GRAPHICS);//ͼ�ι�����
	}
	
	//
	m_btnNine = new CTransparentButtonEx;
	m_btnNine->Create(_T("��ԴĿ¼"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN9);
	m_btnNine->SetName(_T("btnNine"));
	m_btnNine->SetWindowPos(&wndTop,x,WIDTH_HEAD + 2*SPACE + iChilpHeight + 8*(HEIGHT_BTN + iChilpHeight),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    if ((iType = ReadBtnType(_T("btnNine"))) != -1)
	{
		m_btnNine->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
		m_btnNine->SetWindowText(btnText);
	}
	else
	{
		m_btnNine->SetType(TYPE_RESOURCE);;//��ԴĿ¼
	}   
	
	//
	m_btnTen = new CTransparentButtonEx;
	m_btnTen->Create(_T("������"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTN10);
	m_btnTen->SetName(_T("btnTen"));
	m_btnTen->SetWindowPos(&wndTop,x,WIDTH_HEAD + 2*SPACE + iChilpHeight + 9*(HEIGHT_BTN + iChilpHeight),WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
    if ((iType = ReadBtnType(_T("btnTen"))) != -1)
	{
		m_btnTen->SetType((eType)iType);
		btnText = GetCaptureFromType((eType)iType);
		m_btnTen->SetWindowText(btnText);
	}
	else
	{
		m_btnTen->SetType(TYPE_ALLTOOL);;//������
	}
	
	//
	g_toolBarHWND = m_hWnd;
}

CString CDlgToolBar::GetCaptureFromType(eType btnEmumType)
{
	switch(btnEmumType)
	{
     case TYPE_SELECT:
		{
			return _T("ѡ��");
		}
		break;
	case TYPE_FULLSCREEN:
		{
			return _T("ȫ��");
		}
		break;
	case TYPE_BIGHT:
		{
			return _T("����");
		}
		break;
	case TYPE_LINE:
		{
			return _T("ֱ��");
		}
		break;
	case TYPE_RESOURCE: 
		{
		    return _T("��ԴĿ¼");	
		}
		break;
	case TYPE_DEL:
		{
            return _T("ɾ��");
		}
		break;
	case TYPE_T:
		{
			return _T("�ı�");
		}
		break;
	case TYPE_REDO:
		{
			return _T("����");
		}
		break;
	case TYPE_UNDO:
		{
			return _T("����");
		}
		break;
	case TYPE_FONT:
		{
            return _T("����");
		}
		break;
	case TYPE_ATTRIBUTE:
		{
			return _T("����");
		}
		break;
	case TYPE_SCREENSHOTS:
		{
            return _T("����");				
		}
		break;
	case TYPE_MAQN:
		{
            return _T("�Ŵ�");				
		}
		break;
	case TYPE_SPOTLIGHT:
		{
            return _T("�۹��");				
		}
		break;
	case TYPE_DESKTOPDRAW:
		{
            return _T("�����ͼ");				
		}
		break;
	case TYPE_DLG_RESOURCE:   
		{

			return _T("��Դ������");
		}
	    break;	
    case TYPE_DLG_RUBBER:    
		{
			return _T("���ӹ�����"); 
		}
	    break;
	case TYPE_DLG_GRAPHICS:   
		{
			return _T("ͼ�ι�����");
		}
	    break;	
    case TYPE_DLG_FILE:   
		{
			return _T("�ļ�������");
		}
	    break;
	case TYPE_DLG_PAGE:   
		{
			return _T("��ԴĿ¼");
		}
	    break;
	case TYPE_FUNCTIONDLG:   
		{
			return _T("�������");
		}
	    break;
	default:

		break;
	}
}
 void CDlgToolBar::FlashBackground()
 {
	//���ع���������
	CString strPath = _T("");
	strPath = GetAppPath() + _T("\\SKIN\\HPWBModel\\72-72��ť\\Ƥ��.png");
	if (!m_pBitmap)
	{
		m_pBitmap = new Bitmap(strPath);
	}
	else
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
		m_pBitmap = new Bitmap(strPath);
	}
	if (m_pBitmap)
	{
		CRect rect;
		GetClientRect(&rect);
		CClientDC dc(this);
		Graphics graph(dc.GetSafeHdc());
		Rect Rt(0,0, rect.Width(), rect.Height());
		graph.DrawImage(m_pBitmap, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);	
	}
 }
BOOL CDlgToolBar::OnEraseBkgnd(CDC* pDC)
{
    FlashBackground();
	return true;
	//return CDialog::OnEraseBkgnd(pDC);
}
LRESULT CDlgToolBar::OnNcHitTest(CPoint point)
{
	 CRect rc;
	 GetClientRect(&rc);
	 ClientToScreen(&rc);	 
	 return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);
}
void CDlgToolBar::ProcessCmd(eType cmdType,bool bFromBox)
{
	switch (cmdType)
	{
	case TYPE_SELECT:
		{
			WB_SetDrawStatus(DRAW_NORMAL);
		}
		break;
	case TYPE_PAGE:
		{
			WB_AddNewPage();
		}
		break;
	case TYPE_FULLSCREEN:
		{
			if (!m_bFullScreen)
			{
				WB_SetFullScreen(true);
				m_bFullScreen = true;
			}
			else
			{
				WB_SetFullScreen(false);
				m_bFullScreen = false;
			}

		}
		break;
	case TYPE_EXITFULLSCREEN:
		{
		}
		break;
	case TYPE_ARROWS:
		{

		}
		break;
	case TYPE_BIGHT:
		{
			OpenColor();
			WB_SetDrawStatus(DRAW_POLYLINE);

		}
		break;
	case TYPE_LINE:
		{
			OpenColor();
			WB_SetDrawStatus(DRAW_LINE);

		}
		break;
	case TYPE_RUBBER_BIG: //���
		{
			WB_SetDrawStatus(DRAW_ERASE);
			WB_SetBrushSize(BRUNSH_BIG);
			
		}
		break;
	case TYPE_RUBBER_SMALL: 
		{
			WB_SetDrawStatus(DRAW_ERASE);
			WB_SetBrushSize(BRUSH_SMALL);

		}
		break;
	case TYPE_RUBBER_OBJ:
		{
			WB_SetDrawStatus(DRAW_CLEANUP);

		}
		break;
	case TYPE_RUBBER_CURVE:
		{
			WB_SetDrawStatus(DRAW_RGNDELETE);

		}
		break;
	case TYPE_IMAGE:
		{
			WB_AddPicture();
		}
		break;
	case TYPE_VIDEO:
		{
			WB_AddMovie();
		}
		break;
	case TYPE_DOCUMENT:
		{
			WB_AddDoc();
		}
		break;
	case TYPE_FLASH:
		{
			WB_AddFlash();
		}
		break;
	case TYPE_RESOURCE: //ԭ��Դ�Ի���
		{

			WB_OpenResource();
			
		}
		break;
	case TYPE_DEL:
		{

			WB_DeleteSelectedObjects();
		}
		break;
	case TYPE_PRE_PAGE:
		{
			WB_PrePage();
		}
		break;
	case TYPE_NEXT_PAGE:
		{
			WB_NextPage();
		}
		break;
	case TYPE_T:
		{

			WB_SetDrawStatus(DRAW_TEXT);
		}
		break;
	case TYPE_REDO:
		{
			WB_Redo();
		}
		break;
	case TYPE_UNDO:
		{
			WB_Undo(1);
		}
		break;
	case TYPE_TRIANGLE:		//������
		{

			WB_SetDrawStatus(DRAW_TRIANGLE);
		}
		break;
	case TYPE_RECTANGLE: //����
		{
	
			WB_SetDrawStatus(DRAW_RECT);
			
		}
		break;
	case TYPE_ROUNDNESS:
		{

			WB_SetDrawStatus(DRAW_ELLIPSE);
			
		}
		break;
	case TYPE_RHOMB:
		{
			WB_SetDrawStatus(DRAW_RHOMBUS);
		}
		break;
	case TYPE_POLYGON:
		{
			WB_SetDrawStatus(DRAW_POLYGON);
		}
		break;
	case TYPE_FONT:
		{

			WB_SetCharfont();
		}
		break;
	case TYPE_SAVE:
		{

			WB_SaveWBFile();
		}
		break;
	case TYPE_PRINT:
		{

			WB_Printfile();
		}
		break;
	case TYPE_CLOSE:
		{
			WB_Closefile();
		}
		break;
	case TYPE_OPEN:   
		{
			WB_Openfile();
			
		}
		break;
	case TYPE_RESAVE:
		{
			WB_Resavefile();
		}
		break;
	case TYPE_SCREENSHOTS:
		{

				WB_Cutscreen();

		}
		break;
	case TYPE_DLG_RESOURCE:   //����Դ������
		{

			OpenResource();
		}
	    break;	
    case TYPE_DLG_RUBBER:    //�򿪲��ӶԻ���
		{
			OpenRubber(); 
		}
	    break;
	case TYPE_DLG_GRAPHICS:   //��ͼ�ζԻ���
		{
			OpenGraphics();   
		}
	    break;	
    case TYPE_DLG_FILE:   //���ļ��Ի���
		{
			OpenFile();   
		}
	    break;
	case TYPE_DLG_PAGE:   //��ҳ�Ի���
		{
			OpenPage();   
		}
	    break;
    case TYPE_CLR_REN:   //��ɫ
		{
			WB_SetDefaultLineColoe(255,0,0);
		}
	    break;
    case TYPE_CLR_BLACK:   //��ɫ
		{ 
			WB_SetDefaultLineColoe(0,0,0);
		}
	    break;
	case TYPE_CLR_YELLOW:   //��ɫ
		{ 
			WB_SetDefaultLineColoe(255,255,0);
		}
		break;
	case TYPE_CLR_ORANGE:   //��ɫ
		{
			WB_SetDefaultLineColoe(255,140,26);
		}
		break;
	case TYPE_CLR_BLUE:   //��ɫ
		{
			WB_SetDefaultLineColoe(0,0,255);
		}
     break;
	case TYPE_CLR_PURPLE:   //��ɫ
		{
			WB_SetDefaultLineColoe(170,85,255);
		}
		break;
    case TYPE_MAQN:   //�Ŵ�
		{
			
		}
		break;
    case TYPE_SPOTLIGHT:   //�۹��
		{
			//WB_NewSpotlight();
		}
		break;
    case TYPE_DESKTOPDRAW:   //�����ͼ
		{
			//OpenDesktopDraw();
		}
		break;
    case TYPE_DDOPEN:   //�����ͼ��
		{
			/*CRect *rect=new CRect(200,200,600,600);
	        WB_DrawScreen(rect);*/
		}
		break;
		//ȫ���л�����bool��������������true��ȫ������������false�ص�ȫ��ǰ��λ�úʹ�С��������ݵõ��ĵ�ǰ״̬����ʵ���뵱ǰ�෴��״̬
    case TYPE_DDFULLSCREEN:   //�����ͼȫ��
		{
		//	bool isFull = WB_IsFullScreen();
	    //    WB_FullScreenShift(!isFull);
			
		}
		break;
		//�л�Ч������bool��������������true��͸��������������false��������������ݵõ��ĵ�ǰ״̬����ʵ���뵱ǰ�෴��״̬
    case TYPE_DDEFFECT:   //�����ͼЧ���л�
		{
			/*bool Trans = WB_IsScreenDrawTrans();
	        WB_GuiseShift(!Trans );*/
		}
		break;
    case TYPE_DDCTRL:   //�����ͼ������
		{
			//WB_OnCtrlDlg();
		}
		break;
    case TYPE_DDCLOSE:   //�����ͼ�ر�
		{ 
			//WB_DestroyWnd();
		}
		break;
	case TYPE_FUNCTIONDLG:   //�������
		{
			WB_FunctionPanel();
		}
		break;

	default:
		break;
	}
}

void CDlgToolBar::ChangButton(CTransparentButtonEx *pDownBtn,int downY,CTransparentButtonEx * pUpBtn,int upY)
{	
	pDownBtn->SetWindowPos(&wndTop,x,upY,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);	
	pUpBtn->SetWindowPos(&wndTop,x,downY,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);

}
void CDlgToolBar::DelAndNewBtn(CTransparentButtonEx *pBtn,int btnY,eType btnEmumType,LPCTSTR lpNewCaption)
{
    if(NULL != pBtn)
	{
		CRect rect(20,20,70,70);
		int BtnID;
		CString strBtnName = pBtn->GetName();//��ð�ť����
        BtnID = pBtn->GetDlgCtrlID();
		pBtn->DestroyWindow();
		delete pBtn;
	    pBtn = new CTransparentButtonEx;
	    pBtn->Create(lpNewCaption,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,BtnID);
	    pBtn->SetWindowPos(&wndTop,x,btnY,WIDTH_BTN,HEIGHT_BTN,SWP_SHOWWINDOW);
        pBtn->SetType(btnEmumType);
        WriteBtnType(strBtnName,btnEmumType);//д�����ļ�
		m_ToolTip.AddTool(pBtn,lpNewCaption);//��ӹ�����ʾ
		switch (BtnID)
		{
			 case IDC_BTN1:  
				{
	               m_btnOne = pBtn;
				   m_btnOne->SetName(_T("btnOne"));
				}
			    break;	
			case IDC_BTN2:   
				{
					m_btnTwo = pBtn;
					m_btnTwo->SetName(_T("btnTwo"));
				}
				break;
			case IDC_BTN3:  
				{
					m_btnThree = pBtn;
					m_btnThree->SetName(_T("btnThree"));
				}
			    break;	
			case IDC_BTN4:   
				{
					m_btnFour = pBtn;
					m_btnFour->SetName(_T("btnFour"));
				}
				break;
			 case IDC_BTN5:  
				{
					m_btnFive = pBtn;
					m_btnFive->SetName(_T("btnFive"));
				}
			    break;	
			case IDC_BTN6:   
				{
					m_btnSix = pBtn;
					m_btnSix->SetName(_T("btnSix"));
				}
				break;
			 case IDC_BTN7:  
				{
				    m_btnSeven = pBtn;
					m_btnSeven->SetName(_T("btnSeven"));
				}
			    break;	
			case IDC_BTN8:   
				{
					m_btnEight = pBtn;
					m_btnEight->SetName(_T("btnEight"));
				}
				break;
			case IDC_BTN9:  
				{
					 m_btnNine = pBtn;
					 m_btnNine->SetName(_T("btnNine"));
				}
			    break;	
			default:
				break;
		}		

	}
	
}
void CDlgToolBar::OpenToolBox()
{
	if (!m_pToolBox)
	{
		CWnd* pWnd = GetParent();
		if (pWnd)
		{
			m_pToolBox = new CDlgFuck(this);
			m_pToolBox->Create(CDlgFuck::IDD,this);
			m_pToolBox->SetParentWnd(this);
			m_pToolBox->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		m_pToolBox->ShowWindow(SW_SHOW);
	}
}
void CDlgToolBar::OpenResource()
{
	if (!m_pResourceDlg)
	{
		CWnd* pWnd = GetParent();
		if (pWnd)
		{
			m_pResourceDlg = new CDlgResource(this);
			m_pResourceDlg->Create(CDlgResource::IDD,this);
			m_pResourceDlg->SetParentWnd(this);
			m_pResourceDlg->ShowWindow(SW_SHOW);
		}
	}
}
void CDlgToolBar::OpenRubber()
{
	if (!m_pRubber)
	{
		CWnd* pWnd = GetParent();
		if (pWnd)
		{
			m_pRubber = new CDlgRubber(this);
			m_pRubber->Create(CDlgRubber::IDD,this);
			m_pRubber->SetParentWnd(this);
			m_pRubber->ShowWindow(SW_SHOW);
		}
	}
}
void CDlgToolBar::OpenGraphics()
{
	if (!m_pGraphics)
	{
		CWnd* pWnd = GetParent();
		if (pWnd)
		{
			m_pGraphics = new CDlgGraphics(this);
			m_pGraphics->Create(CDlgGraphics::IDD,this);
			m_pGraphics->SetParentWnd(this);
			m_pGraphics->ShowWindow(SW_SHOW);
		}
	}
}
void CDlgToolBar::OpenFile()
{
	if (!m_pFileDlg)
	{
		CWnd* pWnd = GetParent();
		if (pWnd)
		{
			m_pFileDlg = new CDlgFile(this);
			m_pFileDlg->Create(CDlgFile::IDD,this);
			m_pFileDlg->SetParentWnd(this);
			m_pFileDlg->ShowWindow(SW_SHOW);
		}
	}
}
void CDlgToolBar::OpenColor()
{
	if (!m_pColor)
	{
		CWnd* pWnd = GetParent();
		if (pWnd)
		{
			m_pColor = new CDlgColor(this);
			m_pColor->Create(CDlgColor::IDD,this);
			m_pColor->SetParentWnd(this);
			m_pColor->ShowWindow(SW_SHOW);
		}
	}
}
void CDlgToolBar::OpenPage()
{
	if (!m_pPage)
	{
		CWnd* pWnd = GetParent();
		if (pWnd)
		{
			m_pPage = new CDlgPage(this);
			m_pPage->Create(CDlgPage::IDD,this);
			m_pPage->SetParentWnd(this);
			m_pPage->ShowWindow(SW_SHOW);
		}
	}
}
void CDlgToolBar::OpenDesktopDraw()
{
	if (!m_pDesktopDraw)
	{
		CWnd* pWnd = GetParent();
		if (pWnd)
		{
			m_pDesktopDraw = new CDLGDesktopDraw(this);
			m_pDesktopDraw->Create(CDLGDesktopDraw::IDD,this);
			m_pDesktopDraw->SetParentWnd(this);
			m_pDesktopDraw->ShowWindow(SW_SHOW);
		}
	}
}
void CDlgToolBar::setResourceDlgNULL()
{
	delete m_pResourceDlg;
	m_pResourceDlg = NULL;
}
void CDlgToolBar::setRubberDlgNULL()
{
	delete m_pRubber;
	m_pRubber = NULL;
}
void CDlgToolBar::setGraphicsDlgNULL()
{
	delete m_pGraphics;
	m_pGraphics = NULL;
}
void CDlgToolBar::setFileDlgNULL()
{
	delete m_pFileDlg;
	m_pFileDlg = NULL;
}
void CDlgToolBar::setColorDlgNULL()
{
	delete m_pColor;
	m_pColor = NULL;
}
void CDlgToolBar::setPageDlgNULL()
{
	delete m_pPage;
	m_pPage = NULL;
}
void CDlgToolBar::setDesktopDrawDlgNULL()
{
	delete m_pDesktopDraw;
	m_pDesktopDraw = NULL;
}
void CDlgToolBar::CloseAllChildToolBox()
{
  if(NULL != m_pResourceDlg)
  {
	  m_pResourceDlg->DestroyWindow();
	  delete m_pResourceDlg;
	  m_pResourceDlg = NULL;
  }
  if(NULL != m_pRubber)
  {
	  m_pRubber->DestroyWindow();
	  delete m_pRubber;
	  m_pRubber = NULL;
  }
  if(NULL != m_pGraphics)
  {
	  m_pGraphics->DestroyWindow();
	  delete m_pGraphics;
	  m_pGraphics = NULL;
  }
  if(NULL != m_pFileDlg)
  {
	  m_pFileDlg->DestroyWindow();
	  delete m_pFileDlg;
	  m_pFileDlg = NULL;
  }
  if(NULL != m_pColor)
  {
	  m_pColor->DestroyWindow();
	  delete m_pColor;
	  m_pColor = NULL;
  }
   if(NULL != m_pPage)
  {
	  m_pPage->DestroyWindow();
	  delete m_pPage;
	  m_pPage = NULL;
  }
   if(NULL != m_pDesktopDraw)
  {
	  m_pDesktopDraw->DestroyWindow();
	  delete m_pDesktopDraw;
	  m_pDesktopDraw = NULL;
  }
  if(NULL != m_pToolBox)
  {
	  m_pToolBox->DestroyWindow();
	  delete m_pToolBox;
	  m_pToolBox = NULL;
  }  

}
void CDlgToolBar::OnBnClickedBtn1()
{ 
	CloseAllChildToolBox();
	ProcessCmd(m_btnOne->GetType());
}

void CDlgToolBar::OnBnClickedBtn2()
{
	CloseAllChildToolBox();
    ProcessCmd(m_btnTwo->GetType());
}

void CDlgToolBar::OnBnClickedBtn3()
{
	CloseAllChildToolBox();
    ProcessCmd(m_btnThree->GetType());
}

void CDlgToolBar::OnBnClickedBtn4()
{
	CloseAllChildToolBox();
    ProcessCmd(m_btnFour->GetType());
}

void CDlgToolBar::OnBnClickedBtn5()
{
	CloseAllChildToolBox();
	ProcessCmd(m_btnFive->GetType());
}

void CDlgToolBar::OnBnClickedBtn6()
{
	CloseAllChildToolBox();
    ProcessCmd(m_btnSix->GetType());
}

void CDlgToolBar::OnBnClickedBtn7()
{
	CloseAllChildToolBox();
    ProcessCmd(m_btnSeven->GetType());
}

void CDlgToolBar::OnBnClickedBtn8()
{
	CloseAllChildToolBox();
    ProcessCmd(m_btnEight->GetType());
}

void CDlgToolBar::OnBnClickedBtn9()
{
	CloseAllChildToolBox();
    ProcessCmd(m_btnNine->GetType());
}

void CDlgToolBar::OnBnClickedBtn10()
{
	CloseAllChildToolBox();
    OpenToolBox();
}

void CDlgToolBar::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnOK();
}

void CDlgToolBar::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnCancel();
}




void CDlgToolBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: �ڴ˴������Ϣ����������
}



void CDlgToolBar::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	CloseAllChildToolBox();

	CDialog::OnNcLButtonDown(nHitTest, point);
}

BOOL CDlgToolBar::PreTranslateMessage(MSG* pMsg)
{
    m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
LRESULT CDlgToolBar::OnCloseToolbarMessage(WPARAM wParam, LPARAM lParam)
{
 CloseAllChildToolBox();
 return 0;
}