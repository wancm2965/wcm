// WBToolbarDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBToolbarDlg.h"
#include "afxdialogex.h"


// CWBToolbarDlg 对话框
HWND CWBToolbarDlg::g_toolBarHWND = NULL;
IMPLEMENT_DYNAMIC(CWBToolbarDlg, CDialog)

CWBToolbarDlg::CWBToolbarDlg(CWnd* pParent /*=NULL*/ , int nToolNum /* = 11*/)
	: CDialog(CWBToolbarDlg::IDD, pParent)
	, m_pLineDlg(NULL)
	, m_pRubberDlg(NULL)
	, m_pGraphicsDlg(NULL)
	, m_pResourseBarDlg(NULL)
	, m_pTextDlg(NULL)
	,m_pColorPatetteDlg(NULL)
	,m_pToolboxdlg(NULL)
	,m_pFileDlg(NULL)
	,m_pDesktopDrawDlg(NULL)
	,m_pToolDlg(NULL)
	, m_pBitmap(NULL)		 
{
	m_pBtnOne = NULL;
	m_pBtnTwo = NULL;
	m_pbtnThree = NULL;
	m_pBtnFour = NULL;
	m_pBtnFive = NULL;
    m_pBtnSix = NULL;
	m_pBtnSeven = NULL;
	m_pBtnEight = NULL;
	m_pBtnNine = NULL;
	m_pBtnTen = NULL;
	m_pBtnEleven = NULL;
	m_ptChooseBtn = NULL;
	m_bFullScreen = FALSE;
	m_nToolbarNum = nToolNum;
	m_dlgBackgroundClr = RGB(1,1,218);
	m_pCheckBtnInfo = new std::map<int,CString>;	
}

CWBToolbarDlg::~CWBToolbarDlg()
{
	if (m_pCheckBtnInfo != NULL)
	{
		m_pCheckBtnInfo->clear();
		delete m_pCheckBtnInfo;
		m_pCheckBtnInfo = NULL;
	}
	if (g_toolBarHWND)
	{
		g_toolBarHWND = NULL;
	}
	if (NULL != m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	}
	if (NULL != m_pBtnOne)  //按钮
	{
		delete m_pBtnOne;
		m_pBtnOne =NULL;
	}
	if (NULL != m_pBtnTwo)
	{
		delete m_pBtnTwo;
		m_pBtnTwo = NULL;
	}
	if (NULL != m_pbtnThree)
	{
		delete m_pbtnThree;
		m_pbtnThree = NULL;
	}
	if (NULL != m_pBtnFour)
	{
		delete m_pBtnFour;
		m_pBtnFour = NULL;
	}
	if (NULL != m_pBtnFive)
	{
		delete m_pBtnFive;
		m_pBtnFive = NULL;
	}
	if (NULL != m_pBtnSix)
	{
		delete m_pBtnSix;
		m_pBtnSix = NULL;
	}
	if (NULL != m_pBtnSeven)
	{
		delete m_pBtnSeven;
		m_pBtnSeven = NULL;
	}
	if (NULL != m_pBtnEight)
	{
		delete m_pBtnEight;
		m_pBtnEight = NULL;
	}
	if (NULL != m_pBtnNine)
	{
		delete m_pBtnNine;
		m_pBtnNine = NULL;
	}
	if (NULL != m_pBtnTen)
	{
		delete m_pBtnTen;
		m_pBtnTen = NULL;
	}
	if (NULL != m_pBtnEleven)
	{
		delete m_pBtnEleven;
		m_pBtnEleven = NULL;
	}
	if (m_ptChooseBtn != NULL)
	{
		m_ptChooseBtn = NULL;
	}
	if (NULL != m_pLineDlg)  //线条子工具条
	{
		delete m_pLineDlg;
		m_pLineDlg = NULL;
	}
	if (NULL != m_pRubberDlg)  //擦子子工具条
	{
		delete m_pRubberDlg;
		m_pRubberDlg = NULL;
	}
	if (NULL != m_pGraphicsDlg)  //擦子子工具条
	{
		delete m_pGraphicsDlg;
		m_pGraphicsDlg = NULL;
	}
	if (NULL != m_pResourseBarDlg)  //资源子工具条
	{
		delete m_pResourseBarDlg;
		m_pResourseBarDlg = NULL;
	}
	if (NULL != m_pTextDlg)  //文字子工具条
	{
		delete m_pTextDlg;
		m_pTextDlg = NULL;
	}
	if (NULL != m_pColorPatetteDlg)//调色板对话框
	{
		delete m_pColorPatetteDlg;
		m_pColorPatetteDlg = NULL;
	}
	if (NULL != m_pToolboxdlg)//工具箱对话框
	{
		delete m_pToolboxdlg;
		m_pToolboxdlg =  NULL;
	}
	if (NULL != m_pFileDlg) //文件子工具条
	{
		delete m_pFileDlg;
		m_pFileDlg = NULL;
	}
	if (NULL != m_pDesktopDrawDlg) //桌面绘图子工具条
	{
		delete m_pDesktopDrawDlg;
		m_pDesktopDrawDlg = NULL;
	}
	if (NULL != m_pToolDlg) //工具子工具条
	{
		delete m_pToolDlg;
		m_pToolDlg = NULL;
	}
}



BEGIN_MESSAGE_MAP(CWBToolbarDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_COMMAND(IDC_BTNONE, &CWBToolbarDlg::OnBtnone)
	ON_COMMAND(IDC_BTNTWO, &CWBToolbarDlg::OnBtntwo)
	ON_COMMAND(IDC_BTNTHREE, &CWBToolbarDlg::OnBtnthree)
	ON_COMMAND(IDC_BTNFOUR, &CWBToolbarDlg::OnBtnfour)
	ON_COMMAND(IDC_BTNFIVE, &CWBToolbarDlg::OnBtnfive)
	ON_COMMAND(IDC_BTNSIX, &CWBToolbarDlg::OnBtnsix)
	ON_COMMAND(IDC_BTNSEVEN, &CWBToolbarDlg::OnBtnseven)
	ON_COMMAND(IDC_BTNEIGHT, &CWBToolbarDlg::OnBtneight)
	ON_COMMAND(IDC_BTNNINE, &CWBToolbarDlg::OnBtnnine)
	ON_COMMAND(IDC_BTNTEN, &CWBToolbarDlg::OnBtnten)
	ON_COMMAND(IDC_BTNELEVEN, &CWBToolbarDlg::OnBtneleven)
	ON_MESSAGE(WM_CLOSE_TOOBAR_MES, OnCloseToolbarMessage)
END_MESSAGE_MAP()


// CWBToolbarDlg 消息处理程序


BOOL CWBToolbarDlg::OnEraseBkgnd(CDC* pDC)
{
	
	CString strPath = _T("");
	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\ToolbarDlgBG.png");
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
		int nBmpWidth = m_pBitmap->GetWidth();
		int nBmpHeight = m_pBitmap->GetHeight();

		SolidBrush brush(Color(153, 183, 217));
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
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}
BOOL CWBToolbarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	ReLayout();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CWBToolbarDlg::ReLayout()
{
	if (!g_bU7NewStyle)
	{
		CheckBtnInfo();
	}
	CRect rect(20,20,70,70);
	int iType = -1;
	CString btnText = _T("");
	int CountButtonNum = -1;
	CString strSelection = _T("ButtonType");
	if (WB_SINGLERVER == g_WBType )
	{
		if (m_pBtnEleven == NULL)
		{
			m_pBtnEleven = new CTransparentButtonEx;
			m_pBtnEleven->Create(_T("联网"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNELEVEN);
			m_pBtnEleven->SetName(_T("btnEleven"));
		}
		m_pBtnEleven->SetWindowPos(&wndTop
			,WB_WIDTH_SPACE
			,WB_TOOLBARBTN_SPACE  + (++CountButtonNum) * (WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
			,WB_TOOLBARBTN_WIDTH
			,WB_TOOLBARBTN_HEIGHT
			,SWP_SHOWWINDOW);
	
		m_pBtnEleven->SetType(TYPE_SINGLE_NETWORK); //选择
		btnText = GetCaptureFromType(TYPE_SINGLE_NETWORK);
		m_pBtnEleven->SetWindowText(btnText);//依据当前的枚举类型设置按钮的标题文本
		
	} 

	if (m_pBtnOne == NULL)
	{
		m_pBtnOne = new CTransparentButtonEx;
		m_pBtnOne->Create(_T("选择"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNONE);
		m_pBtnOne->SetName(_T("btnOne"));
	}
	
	m_pBtnOne->SetWindowPos(&wndTop
		,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE  + (++CountButtonNum) * (WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
		,WB_TOOLBARBTN_WIDTH
		,WB_TOOLBARBTN_HEIGHT
		,SWP_SHOWWINDOW);	
	m_pBtnOne->SetType(TYPE_SELECT); //选择
	btnText = GetCaptureFromType(TYPE_SELECT);
	m_pBtnOne->SetWindowText(btnText);//依据当前的枚举类型设置按钮的标题文本
	

	
	LPCTSTR strReadType = _T("btnTwo");
	//
	int nshownum = m_nToolbarNum;
	if (g_bU7NewStyle)
	{
		if (nshownum > 2)
		{
			nshownum--;
			if (m_pBtnTwo == NULL)
			{
				m_pBtnTwo = new CTransparentButtonEx;
				m_pBtnTwo->Create(_T("删除"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNTWO);
				m_pBtnTwo->SetName(_T("btnTwo"));
			}
			m_pBtnTwo->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			m_pBtnTwo->SetType(TYPE_DEL);//删除
			btnText = GetCaptureFromType(TYPE_DEL);
			m_pBtnTwo->SetWindowText(btnText);
		}
		if (nshownum > 2)
		{
			nshownum--;
			if (m_pBtnFour == NULL)
			{
				m_pBtnFour = new CTransparentButtonEx;
				m_pBtnFour->Create(_T("线型"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNFOUR);
				m_pBtnFour->SetName(_T("btnFour"));
			}
			m_pBtnFour->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			m_pBtnFour->SetType(TYPE_DLG_LINETYPE);//线型
			btnText = GetCaptureFromType(TYPE_DLG_LINETYPE); 
			m_pBtnFour->SetWindowText(btnText);
		}
		if (nshownum > 2)
		{
			nshownum--;
			if (m_pBtnFive == NULL)
			{
				m_pBtnFive = new CTransparentButtonEx;
				m_pBtnFive->Create(_T("矩形"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNFIVE);
				m_pBtnFive->SetName(_T("btnFive"));
			}
			m_pBtnFive->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			m_pBtnFive->SetType(TYPE_DLG_GRAPHICS);//图形
			btnText = GetCaptureFromType(TYPE_DLG_GRAPHICS);
			m_pBtnFive->SetWindowText(btnText);
		}
		if (nshownum > 2)
		{
			nshownum--;
			if (m_pBtnSeven == NULL)
			{
				m_pBtnSeven = new CTransparentButtonEx;
				m_pBtnSeven->Create(_T("小擦"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNSEVEN);
				m_pBtnSeven->SetName(_T("btnSeven"));
			}

			m_pBtnSeven->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				, WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			m_pBtnSeven->SetType(TYPE_DLG_RUBBER); //擦子
			btnText = GetCaptureFromType(TYPE_DLG_RUBBER);
			m_pBtnSeven->SetWindowText(btnText);
			
		}
		if (nshownum > 2)
		{
			nshownum--;
			if (m_pBtnEight == NULL)
			{
				m_pBtnEight = new CTransparentButtonEx;
				m_pBtnEight->Create(_T("调色板"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNEIGHT);
				m_pBtnEight->SetName(_T("btnEight"));
			}

			m_pBtnEight->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				, WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			m_pBtnEight->SetType(TYPE_DLG_COLORPALETTE);//调色板
			btnText = GetCaptureFromType(TYPE_DLG_COLORPALETTE);
			m_pBtnEight->SetWindowText(btnText);
		}
		if (nshownum > 2)
		{
			nshownum--;
			if (m_pBtnSix == NULL)
			{
				m_pBtnSix = new CTransparentButtonEx;
				m_pBtnSix->Create(_T("视频"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNSIX);
				m_pBtnSix->SetName(_T("btnSix"));
			}
			m_pBtnSix->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				, WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			strReadType = _T("btnSix");
			if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
			{
				m_pBtnSix->SetType((eType)iType);
				btnText = GetCaptureFromType((eType)iType);
				m_pBtnSix->SetWindowText(btnText);
			}
			else
			{
				m_pBtnSix->SetType(TYPE_DLG_RESOURCEFILE);  //资源
				btnText = GetCaptureFromType(TYPE_DLG_RESOURCEFILE);
				m_pBtnSix->SetWindowText(btnText);
			}
		}
		if (nshownum > 2)
		{
			nshownum--;
			if (m_pBtnNine == NULL)
			{
				m_pBtnNine = new CTransparentButtonEx;
				m_pBtnNine->Create(_T("文件工具条"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNNINE);
				m_pBtnNine->SetName(_T("btnEight"));
			}

			m_pBtnNine->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				, WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			m_pBtnNine->SetType(TYPE_DLG_FILE);
			btnText = GetCaptureFromType(TYPE_DLG_FILE);
			m_pBtnNine->SetWindowText(btnText);
		}
		if (WB_SINGLERVER == g_WBType)
		{
			m_pBtnTen = new CTransparentButtonEx;
			m_pBtnTen->Create(_T("退出程序"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNTEN);
			m_pBtnTen->SetName(_T("btnTen"));
			m_pBtnTen->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				, WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			
			
			{   //读取终端机配置文件，默认路径固定，nIsXPE为1是，为0否
				CString csINIPath = _T("C:\\Program Files\\avcon\\ExePath.ini");
				int nIsXPE = GetPrivateProfileInt( _T("AVCON_SELECTED"), _T("AVCONXPE"), 0, csINIPath);
				//int nIsXPE = ReadCfgInt(csINIPath, _T("AVCON_SELECTED"), _T("AVCONXPE"), 0);
				if (nIsXPE)
				{
					m_pBtnTen->SetType(TYPE_SINGLE_SHOTDOWN);// 关机
					btnText = GetCaptureFromType(TYPE_SINGLE_SHOTDOWN);
					m_pBtnTen->SetWindowText(btnText);
				} 
				else
				{
					m_pBtnTen->SetType(TYPE_SINGLE_QUIT);//退出程序
					btnText = GetCaptureFromType(TYPE_SINGLE_QUIT);
					m_pBtnTen->SetWindowText(btnText);
				}

			}
		}
		else
		{
			if (m_pBtnTen == NULL)
			{
				m_pBtnTen = new CTransparentButtonEx;
				m_pBtnTen->Create(_T("全屏"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNTEN);
				m_pBtnTen->SetName(_T("btnTen"));
			}
			m_pBtnTen->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				, WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			m_pBtnTen->SetType(TYPE_FULLSCREEN);
			btnText = GetCaptureFromType(TYPE_FULLSCREEN);
			m_pBtnTen->SetWindowText(btnText);
		}

		
		
	}
	else
	{
		if (nshownum > 3)
		{
			nshownum--;
			if (m_pBtnTwo == NULL)
			{
				m_pBtnTwo = new CTransparentButtonEx;
				m_pBtnTwo->Create(_T("删除"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNTWO);
				m_pBtnTwo->SetName(_T("btnTwo"));
			}
			m_pBtnTwo->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);

			if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)//根据配置文件来设置当前应该使用什么按钮，如果配置文件没有记录当前得设定就用下面默认的
			{			
				m_pBtnTwo->SetType((eType)iType);
				btnText = GetCaptureFromType((eType)iType);
				m_pBtnTwo->SetWindowText(btnText);//依据当前的枚举类型设置按钮的标题文本
			}
			else
			{
				m_pBtnTwo->SetType(TYPE_DEL);//删除
				btnText = GetCaptureFromType(TYPE_DEL);
				m_pBtnTwo->SetWindowText(btnText);
			}
		}
		if (nshownum > 3)
		{
			nshownum--;
			if (m_pbtnThree == NULL)
			{
				m_pbtnThree = new CTransparentButtonEx;
				m_pbtnThree->Create(_T("新建页"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNTHREE);
				m_pbtnThree->SetName(_T("btnThree"));
			}
			m_pbtnThree->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + (++CountButtonNum)* (WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			strReadType = _T("btnThree");
			if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
			{
				m_pbtnThree->SetType((eType)iType);
				btnText = GetCaptureFromType((eType)iType);
				m_pbtnThree->SetWindowText(btnText);
			}
			else
			{
				m_pbtnThree->SetType(TYPE_PAGE);//新建页
				btnText = GetCaptureFromType(TYPE_PAGE);
				m_pbtnThree->SetWindowText(btnText);
			}
		}
		if (nshownum > 3)
		{
			nshownum--;
			if (m_pBtnFour == NULL)
			{
				m_pBtnFour = new CTransparentButtonEx;
				m_pBtnFour->Create(_T("线型"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNFOUR);
				m_pBtnFour->SetName(_T("btnFour"));
			}
			m_pBtnFour->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			strReadType = _T("btnFour");
			if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
			{
				m_pBtnFour->SetType((eType)iType);
				btnText = GetCaptureFromType((eType)iType); 
				m_pBtnFour->SetWindowText(btnText);
			}
			else
			{
				m_pBtnFour->SetType(TYPE_DLG_LINETYPE);//线型
				btnText = GetCaptureFromType(TYPE_DLG_LINETYPE); 
				m_pBtnFour->SetWindowText(btnText);
			}  
		}
		if (nshownum > 3)
		{
			nshownum--;
			if (m_pBtnFive == NULL)
			{
				m_pBtnFive = new CTransparentButtonEx;
				m_pBtnFive->Create(_T("矩形"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNFIVE);
				m_pBtnFive->SetName(_T("btnFive"));
			}
			m_pBtnFive->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			strReadType = _T("btnFive");
			if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
			{
				m_pBtnFive->SetType((eType)iType);
				btnText = GetCaptureFromType((eType)iType);
				m_pBtnFive->SetWindowText(btnText);
			}
			else
			{
				m_pBtnFive->SetType(TYPE_DLG_GRAPHICS);//图形
				btnText = GetCaptureFromType(TYPE_DLG_GRAPHICS);
				m_pBtnFive->SetWindowText(btnText);
			}
		}
		if (nshownum > 3)
		{
			nshownum--;
			if (m_pBtnSix == NULL)
			{
				m_pBtnSix = new CTransparentButtonEx;
				m_pBtnSix->Create(_T("视频"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNSIX);
				m_pBtnSix->SetName(_T("btnSix"));
			}
			m_pBtnSix->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				, WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			strReadType = _T("btnSix");
			if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
			{
				m_pBtnSix->SetType((eType)iType);
				btnText = GetCaptureFromType((eType)iType);
				m_pBtnSix->SetWindowText(btnText);
			}
			else
			{
				m_pBtnSix->SetType(TYPE_DLG_RESOURCEFILE);  //资源
				btnText = GetCaptureFromType(TYPE_DLG_RESOURCEFILE);
				m_pBtnSix->SetWindowText(btnText);
			}
		}
		if (nshownum > 3)
		{
			nshownum--;
			if (m_pBtnSeven == NULL)
			{
				m_pBtnSeven = new CTransparentButtonEx;
				m_pBtnSeven->Create(_T("小擦"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNSEVEN);
				m_pBtnSeven->SetName(_T("btnSeven"));
			}

			m_pBtnSeven->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				, WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			strReadType = _T("btnSeven");
			if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
			{
				m_pBtnSeven->SetType((eType)iType);
				btnText = GetCaptureFromType((eType)iType);
				m_pBtnSeven->SetWindowText(btnText);
			}
			else
			{
				m_pBtnSeven->SetType(TYPE_DLG_RUBBER); //擦子
				btnText = GetCaptureFromType(TYPE_DLG_RUBBER);
				m_pBtnSeven->SetWindowText(btnText);
			}
		}
		if (nshownum > 3)
		{
			nshownum--;
			if (m_pBtnEight == NULL)
			{
				m_pBtnEight = new CTransparentButtonEx;
				m_pBtnEight->Create(_T("调色板"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNEIGHT);
				m_pBtnEight->SetName(_T("btnEight"));
			}

			m_pBtnEight->SetWindowPos(&wndTop
				,WB_WIDTH_SPACE
				, WB_TOOLBARBTN_SPACE + (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
				,WB_TOOLBARBTN_WIDTH
				,WB_TOOLBARBTN_HEIGHT
				,SWP_SHOWWINDOW);
			strReadType = _T("btnEight");
			if ((iType = ReadWBConfigPathINI(strSelection,strReadType)) != -1)
			{
				m_pBtnEight->SetType((eType)iType);
				btnText = GetCaptureFromType((eType)iType);
				m_pBtnEight->SetWindowText(btnText);
			}
			else
			{
				m_pBtnEight->SetType(TYPE_DLG_COLORPALETTE);//调色板
				btnText = GetCaptureFromType(TYPE_DLG_COLORPALETTE);
				m_pBtnEight->SetWindowText(btnText);
			}
		}
		if (m_pBtnNine == NULL)
		{
			m_pBtnNine = new CTransparentButtonEx;
			m_pBtnNine->Create(_T("资源目录"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNNINE);
			m_pBtnNine->SetName(_T("btnNine"));
		}
		m_pBtnNine->SetWindowPos(&wndTop
			,WB_WIDTH_SPACE
			, WB_TOOLBARBTN_SPACE + (++CountButtonNum) *(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
			,WB_TOOLBARBTN_WIDTH
			,WB_TOOLBARBTN_HEIGHT
			,SWP_SHOWWINDOW);
		m_pBtnNine->SetType(TYPE_RESOURCE);;//资源目录
		btnText = GetCaptureFromType(TYPE_RESOURCE);
		m_pBtnNine->SetWindowText(btnText);


		if (m_pBtnTen == NULL)
		{
			m_pBtnTen = new CTransparentButtonEx;
			m_pBtnTen->Create(_T("工具箱"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,rect,this,IDC_BTNTEN);
			m_pBtnTen->SetName(_T("btnTen"));
		}
		m_pBtnTen->SetWindowPos(&wndTop
			,WB_WIDTH_SPACE
			,WB_TOOLBARBTN_SPACE +  (++CountButtonNum)*(WB_TOOLBARBTN_HEIGHT + WB_TOOLBARBTN_SPACE)
			,WB_TOOLBARBTN_WIDTH
			,WB_TOOLBARBTN_HEIGHT
			,SWP_SHOWWINDOW);
		m_pBtnTen->SetType(TYPE_ALLTOOL);//工具箱
		btnText = GetCaptureFromType(TYPE_ALLTOOL);
		m_pBtnTen->SetWindowText(btnText);
	}
	g_toolBarHWND = m_hWnd;
	CString strValue = _T("");
	if (m_pBtnTwo)
	{
		strValue.Format(_T("%d"),m_pBtnTwo->GetType());
		WriteWBConfigPathINI(strSelection,m_pBtnTwo->GetName(),strValue);
	}
	if (m_pbtnThree)
	{
		strValue.Format(_T("%d"),m_pbtnThree->GetType());
		WriteWBConfigPathINI(strSelection,m_pbtnThree->GetName(),strValue);
	}
	if (m_pBtnFour)
	{
		strValue.Format(_T("%d"),m_pBtnFour->GetType());
		WriteWBConfigPathINI(strSelection,m_pBtnFour->GetName(),strValue);
	}
	if (m_pBtnFive)
	{
		strValue.Format(_T("%d"),m_pBtnFive->GetType());
		WriteWBConfigPathINI(strSelection,m_pBtnFive->GetName(),strValue);
	}
	if (m_pBtnSix)
	{
		strValue.Format(_T("%d"),m_pBtnSix->GetType());
		WriteWBConfigPathINI(strSelection,m_pBtnSix->GetName(),strValue);
	}
	if (m_pBtnSeven)
	{
		strValue.Format(_T("%d"),m_pBtnSeven->GetType());
		WriteWBConfigPathINI(strSelection,m_pBtnSeven->GetName(),strValue);
	}
	if (m_pBtnEight)
	{
		strValue.Format(_T("%d"),m_pBtnEight->GetType());
		WriteWBConfigPathINI(strSelection,m_pBtnEight->GetName(),strValue);
	}
	SetChooseBtn(m_pBtnOne);
}

void CWBToolbarDlg::ProcessCmd(eType cmdType,CTransparentButtonEx *pParentBtn)
{
	switch (cmdType)
	{
	case TYPE_DICOM:
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			CloseToolbox();
			if (g_bSupportU7OpenFile)
			{
				//wchar_t szFilter[MAX_PATH];
				CString strFilter;
				strFilter = ReadConfigText(_T("WhiteBoardControlText"), _T("TYPE_DICOM"));
				if (strFilter.IsEmpty())
				{
					strFilter = _T("DICOM文件");
				}
				strFilter += _T("|*.*||");
				//MultiByteToWideChar(CP_ACP, 0, strFilter+"|*.*||", -1, szFilter, MAX_PATH);

				char* sFilePath = new char[MAX_PATH];
				bool bOpen = g_pHPWBDataNotify->OpenSaveFile(TRUE, sFilePath, NULL, strFilter, this->GetSafeHwnd());
				if (g_pWhiteBoardView == NULL)
				{
					return;
				}
				if (bOpen)
				{
					WB_AddDICOM(sFilePath);
				}
				delete[] sFilePath;
			} 
			else
			{
				WB_AddDICOM();
			}
		}
		break;

	case TYPE_SELECT:
		{
			CloseToolbox();
			WB_SetDrawStatus(DRAW_NORMAL);
			ClipCursor(NULL);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			//g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
		}
		break;
	case TYPE_RECYCLE: //回收站
		{
			CloseToolbox();
			WB_Dustbin();
		}
		break;
	case TYPE_PAGE:
		{
			CloseToolbox();
			WB_AddNewPage();
		}
		break;
	case TYPE_FULLSCREEN:
		{
			if (!m_bFullScreen)
			{
				WB_SetFullScreen(TRUE);
				m_bFullScreen = TRUE;
			}
			else
			{
				WB_SetFullScreen(FALSE);
				m_bFullScreen = FALSE;
			}
			//关闭工具箱
			CloseToolbox();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			//g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
		}
		break;
	case TYPE_BIGHT:   //曲线,在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_POLYLINE);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
           // g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		}
		break;
	case TYPE_LINE:    //直线,在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_LINE);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			//g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
		}
		break;
	case TYPE_ICOCURVE:  //图标曲线,在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_POLYICON);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
		}
		break;
	case TYPE_MAGCURVE:
		{
			WB_SetDrawStatus(DRAW_MAGICLINE);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
		}
		break;
	case TYPE_RUBBER_BIG: //大擦,在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_ERASE);			
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				if (NULL != g_pWhiteBoardView->GetWBFunctionDlg())
				{
					if (::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
					{
						::SendMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBERASER,0,0);
					}
				}
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			//g_pWhiteBoardView->OpenAttributeDlg();
			//g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
		}
		break;
	case TYPE_RUBBER_OBJ:  //擦对象,在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_CLEANUP);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				if (NULL != g_pWhiteBoardView->GetWBFunctionDlg())
				{
					if (::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
					{
						::SendMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBERASER,0,0);
					}
				}
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			//g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
		}
		break;
	case TYPE_RUBBER_CURVE: //曲擦,在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_RGNDELETE);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				if (NULL != g_pWhiteBoardView->GetWBFunctionDlg())
				{
					if (::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
					{
						::SendMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBERASER,0,0);
					}
				}
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		}
		break;
	case TYPE_IMAGE:  //在子工具条中已经调用关闭工具箱
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}			
			if (g_bSupportU7OpenFile)
			{
				//wchar_t szFilter[MAX_PATH];
				CString strFilter;
				strFilter = ReadConfigText(_T("WhiteBoardControlText"), _T("TYPE_IMAGE"));
				if (strFilter.IsEmpty())
				{
					strFilter = _T("图片");
				}
				strFilter += _T("(*.jpg;*.jpeg;*.jpe;*.jfif;*.gif;*.bmp;*.dib;*.tif;*.tiff;*.png;*.ico)|*.jpg;*.jpeg;*.jpe;*.jfif;*.gif;*.bmp;*.dib;*.tif;*.tiff;*.png;*.ico||");
				//MultiByteToWideChar(CP_ACP, 0,  , -1, szFilter, MAX_PATH);

				char* sFilePath = new char[MAX_PATH];
				memset(sFilePath,0,MAX_PATH);
				CRect rect = CRect(0,0,0,0);
				
				bool bOpen = g_pHPWBDataNotify->OpenSaveFile(TRUE,sFilePath, NULL, (LPCTSTR)strFilter, this->GetSafeHwnd());
				if (g_pWhiteBoardView == NULL)
				{
					return;
				}
				if (bOpen)
				{
					CString strfilename = A2U(sFilePath);
					WB_AddPicture(CW2A(strfilename),rect);
				}
				delete[] sFilePath;
			} 
			else
			{
				WB_AddPicture();
			}
		}
		break;
	case TYPE_VIDEO:  //在子工具条中已经调用关闭工具箱
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			//WB_AddMovie(); 老视频
			//新视频
			if (g_bSupportU7OpenFile)
			{
				//wchar_t szFilter[MAX_PATH];
				//CString strFilter = _T("视频 (*.avi;*.wmv;*.mpeg;*.mpg;*.asf;*.mov;*.dat;*.rmvb;*.vob;*.mp4;*.f4v;*.mp3;*.wm;*.wma;*.rm;*.mts;*.mkv)|*.avi;*.wmv;*.mpeg;*.mpg;*.asf;*.mov;*.dat;*.rmvb;*.vob;*.mp4;*.f4v;*.mp3;*.wm;*.wma;*.rm;*.mts;*.mkv||");
				CString strFilter;
				strFilter = ReadConfigText(_T("WhiteBoardControlText"), _T("TYPE_VIDEO"));
				if (strFilter.IsEmpty())
				{
					strFilter = _T("视频");
				}
				strFilter += _T("所有媒体格式|*.mkv;*.avms;*.avi;*.dat;*.mpeg;*.mpg;*.mov;*.vob;*.asf;*.wmv;*.wm;*.wma;*.rm;*.rmvb;*.mp3;*.mp4;*.flv;*.f4v||")/*_T(" (*.avi;*.wmv;*.mpeg;*.mpg;*.mkv;)|*.avi;*.wmv;*.mpeg;*.mpg;*.mkv||")*/;
				//MultiByteToWideChar(CP_ACP, 0, strFilter+" (*.avi;*.wmv;*.mpeg;*.mpg)|*.avi;*.wmv;*.mpeg;*.mpg||", -1, szFilter, MAX_PATH);

				char* sFilePath = new char[MAX_PATH];
				memset(sFilePath,0,MAX_PATH);
				bool bOpen = g_pHPWBDataNotify->OpenSaveFile(TRUE, sFilePath, NULL, strFilter, this->GetSafeHwnd());

				if (g_pWhiteBoardView == NULL)
				{
					return;
				}
				if (bOpen)
				{
					WB_AddMedia(sFilePath);
				}
				delete[] sFilePath;
			} 
			else
			{
				WB_AddMedia();
			}
		}
		break;
	case TYPE_DOCUMENT:   //在子工具条中已经调用关闭工具箱
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}			
			if (g_bSupportU7OpenFile)
			{
				//wchar_t szFilter[MAX_PATH];
			
				CString strFilter = _T("Print Files(*.pdf;*.doc;*.xls;*.ppt;*.docx;*.xlsx;*.pptx)|*.pdf;*.doc;*.xls;*.ppt;*.docx;*.xlsx;*.pptx||");
			
				if (ZSYDZ)
				{
					strFilter = _T("Print Files(*.pdf)|*.pdf||");
				}
				
				
				//MultiByteToWideChar(CP_ACP, 0, strFilter, -1, szFilter, MAX_PATH);

				CRect rect = CRect(0,0,0,0);
				char* sFilePath = new char[MAX_PATH];
				memset(sFilePath,0,MAX_PATH);
				bool bOpen = g_pHPWBDataNotify->OpenSaveFile(TRUE, sFilePath, NULL,strFilter, this->GetSafeHwnd());
				if (g_pWhiteBoardView == NULL)
				{
					return;
				}
				if (bOpen)
				{
					WB_AddDoc(sFilePath,rect);
				}
				delete[] sFilePath;
			} 
			else
			{
				WB_AddDoc();
			}
		}
		break;
	case TYPE_FLASH:   //在子工具条中已经调用关闭工具箱
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}			
			if (g_bSupportU7OpenFile)
			{
				//wchar_t szFilter[MAX_PATH];
				CString strFilter;
				strFilter = ReadConfigText(_T("WhiteBoardControlText"), _T("TYPE_VIDEO"));
				if (strFilter.IsEmpty())
				{
					strFilter = _T("视频");
				}
				strFilter += _T(" (*.swf)|*.swf||");
				//MultiByteToWideChar(CP_ACP, 0, strFilter+" (*.swf)|*.swf||", -1, szFilter, MAX_PATH);

				CRect rect = CRect(0,0,0,0);
				char* sFilePath = new char[MAX_PATH];
				memset(sFilePath,0,MAX_PATH);
				bool bOpen = g_pHPWBDataNotify->OpenSaveFile(TRUE, sFilePath, NULL, strFilter, this->GetSafeHwnd());
				if (g_pWhiteBoardView == NULL)
				{
					return;
				}
				if (bOpen)
				{
					WB_AddFlash(sFilePath,rect);
				}
				delete[] sFilePath;
			} 
			else
			{
				WB_AddFlash();
			}
		}
		break;
	case TYPE_DXF:
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}			
			if (g_bSupportU7OpenFile)
			{
				CString strFilter;
				strFilter = ReadConfigText(_T("WhiteBoardControlText"), _T("TYPE_DXF"));
				if (strFilter.IsEmpty())
				{
					strFilter = _T("Dxf");
				}
				strFilter += _T(" (*.dxf)|*.dxf||");

				CRect rect = CRect(0,0,0,0);
				char* sFilePath = new char[MAX_PATH];
				memset(sFilePath,0,MAX_PATH);
				bool bOpen = g_pHPWBDataNotify->OpenSaveFile(TRUE, sFilePath, NULL, strFilter, this->GetSafeHwnd());
				if (g_pWhiteBoardView == NULL)
				{
					return;
				}
				if (bOpen)
				{
					WB_AddDxf(sFilePath, rect);
				}
				delete[] sFilePath;
			} 
			else
			{
				//WB_AddDxf();
			}
		}
		break;
	case TYPE_CAMERAHEAD:  //摄像头
		{
			CloseToolbox();
			WB_AddCamera();
		}
		break;

	case TYPE_RESOURCE: //资源目录
		{
			CloseToolbox();
			WB_OpenResource();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
		}
		break;
	case TYPE_DEL:
		{
			CloseToolbox();
			WB_DeleteSelectedObjects();		
		}
		break;
	case TYPE_PRE_PAGE:
		{
			CloseAllChildBox();
			WB_PrePage();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		}
		break;
	case TYPE_NEXT_PAGE:
		{
			CloseAllChildBox();
			WB_NextPage();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
		}
		break;
	case TYPE_DLG_T:
		{
			CloseToolbox();
			WB_SetDrawStatus(DRAW_TEXT);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_REDO:
		{
			CloseAllChildBox();
			WB_Redo();			
		}
		break;
	case TYPE_UNDO:
		{
			CloseAllChildBox();
			WB_Undo(1);			
		}
		break;
	case TYPE_DYTRIANGLE:
		{
			g_pWhiteBoardView->SetDrawTriangleType(2);
			WB_SetDrawStatus(DRAW_TRIANGLE);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_DYZJTRIANGLE:
		{
			g_pWhiteBoardView->SetDrawTriangleType(3);
			WB_SetDrawStatus(DRAW_TRIANGLE);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_PALLELOGAM:
		{
			WB_SetDrawStatus(DRAW_PALLELOGAM);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			
		}
		break;
	case TYPE_ZJTRIANGLE:
		{
			g_pWhiteBoardView->SetDrawTriangleType(1);
			WB_SetDrawStatus(DRAW_TRIANGLE);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_ATRTRIANGLE:
		{
			WB_SetDrawStatus(DRAWANYTRIANGLE);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_TRIANGLE:		//三角形  ,在子工具条中已经调用关闭工具箱
		{
			g_pWhiteBoardView->SetDrawTriangleType(0);
			WB_SetDrawStatus(DRAW_TRIANGLE);
			//WB_SetDrawStatus(DRAWANYTRIANGLE);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			
		}
		break;
	case TYPE_RECTANGLE: //矩形  ,在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_RECT);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_ROUNDNESS:    //在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_ELLIPSE);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			
		}
		break;
	case TYPE_RHOMB:    //在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_RHOMBUS);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			
		}
		break;
	case TYPE_PENTAGON:    //在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_PENTAGON);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
		}
		break;
	case TYPE_HEXAGON:    //在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_HEXAGON);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
		}
		break;
	case TYPE_FIVEPOINTSTART:    //在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_FIVEPOINTSTART);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
		}
		break;
	case TYPE_TRAPEZIUM:    //在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_TRAPEZIUM);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
		}
		break;
	case TYPE_POLYGON:      //在子工具条中已经调用关闭工具箱
		{
			WB_SetDrawStatus(DRAW_POLYGON);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			
		}
		break;
	case TYPE_SAVE:  //在子工具条中已经调用关闭工具箱
		{
			WB_SaveWBFile();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_SAVE_PIC:
		{
			WB_SaveWBFileToPicture();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_PRINT: //在子工具条中已经调用关闭工具箱
		{
			WB_Printfile();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_CLOSE: //在子工具条中已经调用关闭工具箱
		{
			WB_Closefile();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
		}
		break;
	case TYPE_OPEN:    //在子工具条中已经调用关闭工具箱
		{
			WB_Openfile();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}

		}
		break;
		/************************************************************************/
		/* 此处代码用来处理收到合并时进行的处理								        */
	case TYPE_COMBINE:
		{
			WB_Combinefile();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
		}
		break;
		/************************************************************************/

	case TYPE_RESAVE: //在子工具条中已经调用关闭工具箱
		{
			WB_Resavefile();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			
		}
		break;
	case TYPE_SCREENSHOTS:
		{
			CloseToolbox();
			//刷新相关窗口
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->UpdateWindow();
				g_whiteboardDlg->UpdateWindow();
				WB_Cutscreen();
				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				}
				
			}
		}
		break;		
	case TYPE_DLG_FILE:       //打开文件对话框
		{
			OpenFileDlg(pParentBtn);  
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_MAQN:   //放大镜，在子工具条中已经调用关闭工具箱
		{
			WB_Maqnifier();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			
		}
		break;
	case TYPE_SPOTLIGHT:   //聚光灯，在子工具条中已经调用关闭工具箱
		{	
			WB_NewSpotlight();	
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_BAFFLEPLATE:   //挡板，在子工具条中已经调用关闭工具箱
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			WB_Baffle();
		}
		break;
	case  TYPE_PROTRACTOR:   //量角器
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_COMPASSES:    //圆规
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_RULER: //直尺
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			WB_Ruler();
		}
		break;
	case TYPE_SCREENREC:   //屏幕录制
		{	
			CloseToolbox();
			WB_RECScreen();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_DDOPEN:   //桌面绘图打开，在子工具条中已经调用关闭工具箱
		{
			WB_DrawScreen(NULL);
		}
		break;
		//全屏切换传入bool参数，如若传入true则全屏，如若传入false回到全屏前的位置和大小，这里根据得到的当前状态让其实现与当前相反的状态
	case TYPE_DDFULLSCREEN:   //桌面绘图全屏，在子工具条中已经调用关闭工具箱
		{
			//	bool isFull = WB_IsFullScreen();
			//    WB_FullScreenShift(!isFull);

		}
		break;
		//切换效果传入bool参数，如若传入true则透明化，如若传入false则玻璃化，这里根据得到的当前状态让其实现与当前相反的状态
	case TYPE_DDEFFECT:   //桌面绘图效果切换，在子工具条中已经调用关闭工具箱
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			//g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			bool Trans = WB_IsScreenDrawTrans();
			WB_GuiseShift(!Trans );
		}
		break;
	case TYPE_DDCTRL:   //桌面绘图控制条，在子工具条中已经调用关闭工具箱
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			WB_OnCtrlDlg();
		}
		break;
	case TYPE_DDCLOSE:   //桌面绘图关闭，在子工具条中已经调用关闭工具箱
		{   
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			WB_DestroyWnd();
		}
		break;
	case TYPE_FUNCTIONDLG:   //功能面板
		{
			CloseAllChildBox();
			WB_FunctionPanel();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_DLG_COLORPALETTE:   //调色板对话框
		{
			OpenColorPatetteDlg(pParentBtn);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
		}
		break;
	case TYPE_ALLTOOL:   //工具箱对话框
		{
			OpenToolboxDlg();
			WB_SetDrawStatus(DRAW_NORMAL);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_DLG_LINETYPE://线型
		{

			
			switch (CTransparentButtonEx::g_LineTypebuttonType)
			{
			case  TYPE_BIGHT:
				{
					WB_SetDrawStatus(DRAW_POLYLINE);
					if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
					{
						g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
					}
				
				}
				break;
			case  TYPE_LINE:
				{
					WB_SetDrawStatus(DRAW_LINE);
					if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
					{
						g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
					}
					
				}
				break;
			case TYPE_ICOCURVE:
				{
					WB_SetDrawStatus(DRAW_POLYICON);
					if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
					{
						g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
					}
				}
				break;
			case TYPE_MAGCURVE:
				{
					WB_SetDrawStatus(DRAW_MAGICLINE);
					if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
					{
						g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
					}
				}
				break;
			default:
				{
					WB_SetDrawStatus(DRAW_POLYLINE);
					CTransparentButtonEx::g_LineTypebuttonType = TYPE_BIGHT;
					if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
					{
						g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
					}
				}				
			}
			OpenLineDlg(pParentBtn);
			
		}
		break;
	case TYPE_DLG_RUBBER:   //擦子
		{ 
			switch (CTransparentButtonEx::g_RubberbuttonType)
			{
			case  TYPE_RUBBER_OBJ:
				{
					WB_SetDrawStatus(DRAW_CLEANUP);
					if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
					{
						g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
					}
					
				}
				break;
			case  TYPE_RUBBER_CURVE:
				{
					WB_SetDrawStatus(DRAW_RGNDELETE);
					if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
					{
						g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
					}
					
				}
				break;
			case  TYPE_RUBBER_BIG:
				{
					WB_SetDrawStatus(DRAW_ERASE);
					//WB_SetBrushSize(BRUNSH_BIG);
					if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
					{
						if (NULL != g_pWhiteBoardView->GetWBFunctionDlg())
						{
							if (::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
							{
								::SendMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBERASER,0,0);
							}
						}
						g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
					}
					
					//g_pWhiteBoardView->OpenAttributeDlg();
					
				}
				break;
			default:
				{
					WB_SetDrawStatus(DRAW_CLEANUP);
					CTransparentButtonEx::g_RubberbuttonType = TYPE_RUBBER_OBJ;
					if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
					{
						g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
					}
					
				}
			}
			OpenRubberDlg(pParentBtn);
			
			
		}
		break;
	case TYPE_DLG_GRAPHICS:   //图形
		{
			
			switch (CTransparentButtonEx::g_GraphicsbuttonType)
			{
			case  TYPE_POLYGON:
				{
					WB_SetDrawStatus(DRAW_POLYGON);
				}
				break;
			case  TYPE_RECTANGLE:
				{
					WB_SetDrawStatus(DRAW_RECT);
				}
				break;
			case  TYPE_RHOMB:
				{
					WB_SetDrawStatus(DRAW_RHOMBUS);
				}
				break;
			case TYPE_PENTAGON:
				{
					WB_SetDrawStatus(DRAW_PENTAGON);
				}
				break;
			case TYPE_HEXAGON:
				{
					WB_SetDrawStatus(DRAW_HEXAGON);
				}
				break;
			case TYPE_FIVEPOINTSTART:
				{
					WB_SetDrawStatus(DRAW_FIVEPOINTSTART);
				}
				break;
			case TYPE_TRAPEZIUM:
				{
					WB_SetDrawStatus(DRAW_TRAPEZIUM);
				}
				break;
			case  TYPE_ROUNDNESS:
				{
					WB_SetDrawStatus(DRAW_ELLIPSE);
				}
				break;
			case  TYPE_TRIANGLE:
				{
					g_pWhiteBoardView->SetDrawTriangleType(0);
					WB_SetDrawStatus(DRAW_TRIANGLE);
				}
				break;
			case  TYPE_ATRTRIANGLE:
				{
					WB_SetDrawStatus(DRAWANYTRIANGLE);					
				}
				break;
			case TYPE_DYTRIANGLE:
				{
					g_pWhiteBoardView->SetDrawTriangleType(2);
					WB_SetDrawStatus(DRAW_TRIANGLE);
				}
				break;
			case TYPE_DYZJTRIANGLE:
				{
					g_pWhiteBoardView->SetDrawTriangleType(3);
					WB_SetDrawStatus(DRAW_TRIANGLE);
				}
				break;
			case TYPE_ZJTRIANGLE:
				{
					g_pWhiteBoardView->SetDrawTriangleType(1);
					WB_SetDrawStatus(DRAW_TRIANGLE);
				}
				break;
			case TYPE_PALLELOGAM:
				{
					WB_SetDrawStatus(DRAW_PALLELOGAM);
				}
				break;
			default:
				{
					WB_SetDrawStatus(DRAW_RECT);
					CTransparentButtonEx::g_GraphicsbuttonType = TYPE_RECTANGLE;
				}
			}
			OpenGraphicsDlg(pParentBtn);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
		}
		break;
	case TYPE_DLG_RESOURCEFILE:   //资源文件
		{
			OpenResourseBarDlg(pParentBtn);
			WB_SetDrawStatus(DRAW_NORMAL);
			//g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_DLG_TOOL:   //工具
		{
			OpenToolDlg(pParentBtn);   
			WB_SetDrawStatus(DRAW_NORMAL);
			//g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
		}
		break;
	case TYPE_DLG_DESKTOPDRAW://标注
		{
			OpenDesktopDrawDlg(pParentBtn);
			WB_SetDrawStatus(DRAW_NORMAL);
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			
		}
		break;
	case TYPE_EAGLE_EYE:
		{
			CloseToolbox();
			WB_EagleEye();//调用鹰眼的打开函数
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			//g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
		}
		break;
	case TYPE_SINGLE_NETWORK:
		{
			CloseToolbox();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{

				g_pWhiteBoardView->SendLocalMessage(CMD_SINGLE_NETWORK,FALSE);
			}
		}
		break;
	case TYPE_SINGLE_LOCAL:
		{
			CloseToolbox();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{

				g_pWhiteBoardView->SendLocalMessage(CMD_SINGLE_LOCAL,FALSE);

			}
		}
		break;
	case TYPE_SINGLE_SHOTDOWN:
		{
			CloseToolbox();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
		
				g_pWhiteBoardView->SendLocalMessage(CMD_SINGLE_SHOTDOWN,FALSE);

				
			}
		}
		break;
	case TYPE_SINGLE_QUIT:
		{
			CloseToolbox();
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
			
				g_pWhiteBoardView->SendLocalMessage(CMD_SINGLE_QUIT,FALSE);

			}
		}
		break;
	case TYPE_BROADCAST:
		{
			CloseToolbox();
			if(g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SendRollCallBroadCast(0);
			}
		}
		break;
	case TYPE_MIDI:
		{
			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			if (g_bSupportU7OpenFile)
			{
				CString strFilter;
				strFilter = _T("Midi文件(*mid)|*.mid||");
				char* sFilePath = new char[MAX_PATH];
				memset(sFilePath,0,MAX_PATH);
				CRect rect = CRect(0,0,0,0);

				bool bOpen = g_pHPWBDataNotify->OpenSaveFile(TRUE,sFilePath, NULL, (LPCTSTR)strFilter, this->GetSafeHwnd());
				if (g_pWhiteBoardView == NULL)
				{
					return;
				}
				if (bOpen)
				{
					CString strfilename = A2U(sFilePath);
					
					g_pWhiteBoardView->AddMidi(strfilename);
				}
				delete[] sFilePath;
			}
		}
		break;
	default:
		break;
	}
}
CString CWBToolbarDlg::GetCaptureFromType(eType btnEmumType)
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
			if (m_bFullScreen)
			{
				BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("DIS_FULLSCREEN"));
				if (BtnText.IsEmpty())
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
				if (BtnText.IsEmpty())
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
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_ALLTOOL"));
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
	default:
		{
			return _T("");
		}
		break;
	}
}

void CWBToolbarDlg::CloseAllChildToolBox()
{
	if(NULL != m_pLineDlg)  //线
	{
		delete m_pLineDlg;
		m_pLineDlg = NULL;
	}
	if(NULL != m_pRubberDlg)  //擦子
	{
		delete m_pRubberDlg;
		m_pRubberDlg = NULL;
	}
	if(NULL != m_pGraphicsDlg)  //图形
	{
		delete m_pGraphicsDlg;
		m_pGraphicsDlg = NULL;
	}
	if(NULL != m_pResourseBarDlg)  //资源
	{
		delete m_pResourseBarDlg;
		m_pResourseBarDlg = NULL;
	}
	if(NULL != m_pColorPatetteDlg)  //调色板
	{
		delete m_pColorPatetteDlg;
		m_pColorPatetteDlg = NULL;
	}
	if (NULL != m_pFileDlg)  //文件
	{
		delete m_pFileDlg;
		m_pFileDlg = NULL;
	}
	if (NULL != m_pDesktopDrawDlg)  //桌面绘图
	{
		delete m_pDesktopDrawDlg;
		m_pDesktopDrawDlg = NULL;
	//	m_pDesktopDrawDlg->ShowWindow(SW_HIDE);
	
	}
	if (NULL != m_pToolDlg)  //工具
	{
		delete m_pToolDlg;
		m_pToolDlg = NULL;
	}
	
}
void CWBToolbarDlg::OpenLineDlg(CTransparentButtonEx *pParentBtn)
{
	if (!m_pLineDlg)
	{

		m_pLineDlg = new CWBLineDlg(pParentBtn,this);
		m_pLineDlg->Create(CWBLineDlg::IDD,this);
		m_pLineDlg->ShowWindow(SW_SHOW);
		
	}
}
void CWBToolbarDlg::OpenRubberDlg(CTransparentButtonEx *pParentBtn)
{
	if (!m_pRubberDlg)
	{

		m_pRubberDlg = new CWBRubberDlg(pParentBtn,this);
		m_pRubberDlg->Create(CWBRubberDlg::IDD,this);
		m_pRubberDlg->ShowWindow(SW_SHOW);
		
	}
}
void CWBToolbarDlg::OpenGraphicsDlg(CTransparentButtonEx *pParentBtn)
{
	if (!m_pGraphicsDlg)
	{

		m_pGraphicsDlg = new CWBGraphicsDlg(pParentBtn,this);
		m_pGraphicsDlg->Create(CWBGraphicsDlg::IDD,this);
		m_pGraphicsDlg->ShowWindow(SW_SHOW);
		
	}

}
void CWBToolbarDlg::OpenResourseBarDlg(CTransparentButtonEx *pParentBtn)
{
	if (!m_pResourseBarDlg)
	{
		m_pResourseBarDlg = new CWBResourseBarDlg(pParentBtn,this);
		m_pResourseBarDlg->Create(CWBResourseBarDlg::IDD,this);
		m_pResourseBarDlg->ShowWindow(SW_SHOW);
	}

}
void CWBToolbarDlg::OpenTextDlg(CTransparentButtonEx *pParentBtn)
{
	if (!m_pTextDlg)
	{
		m_pTextDlg = new CWBTextDlg(pParentBtn,this);
		m_pTextDlg->Create(CWBTextDlg::IDD,this);
		m_pTextDlg->ShowWindow(SW_SHOW);
	}

}
void CWBToolbarDlg::OpenColorPatetteDlg(CTransparentButtonEx *pParentBtn)
{
	if (!m_pColorPatetteDlg)
	{
		m_pColorPatetteDlg = new CWBColorPaletteDlg(pParentBtn,this);
		m_pColorPatetteDlg->Create(CWBColorPaletteDlg::IDD,this);
		m_pColorPatetteDlg->ShowWindow(SW_SHOW);

	}

}
void CWBToolbarDlg::OpenToolboxDlg()
{
	if (!m_pToolboxdlg)
	{
		
		if (g_WBType != WB_SINGLERVER)
		{
			m_pToolboxdlg = new CWBToolboxDlg(this,(10-m_nToolbarNum));
		}
		else
			m_pToolboxdlg = new CWBToolboxDlg(this);
		m_pToolboxdlg->Create(CWBToolboxDlg::IDD,this);
		m_pToolboxdlg->ShowWindow(SW_SHOW);
		
	}
	else
	{
		m_pToolboxdlg->ShowWindow(SW_SHOW);
	}

}
void CWBToolbarDlg::OpenDesktopDrawDlg(CTransparentButtonEx *pParentBtn)
{
	if (!m_pDesktopDrawDlg)
	{	
			m_pDesktopDrawDlg = new CWBDesktopDrawDlg(pParentBtn,this);
			m_pDesktopDrawDlg->Create(CWBDesktopDrawDlg::IDD,this);
			m_pDesktopDrawDlg->ShowWindow(SW_SHOW);		
	}
	/*else
	{
		m_pDesktopDrawDlg->ShowWindow(SW_SHOW);	
	}*/

}
void CWBToolbarDlg::OpenToolDlg(CTransparentButtonEx *pParentBtn)
{
	if (!m_pToolDlg)
	{	
			m_pToolDlg = new CWBToolDlg(pParentBtn,this);
			m_pToolDlg->Create(CWBToolDlg::IDD,this);
			m_pToolDlg->ShowWindow(SW_SHOW);		
	}
}
void CWBToolbarDlg::OpenFileDlg(CTransparentButtonEx *pParentBtn)
{
	if (!m_pFileDlg)
	{
		m_pFileDlg = new CWBFileDlg(pParentBtn,this);
		m_pFileDlg->Create(CWBFileDlg::IDD,this);
		m_pFileDlg->ShowWindow(SW_SHOW);

	}

}
void CWBToolbarDlg::SetLineDlgNUll()
{
	m_pLineDlg = NULL;

}
void CWBToolbarDlg::SetRubberDlgNUll()
{
	m_pRubberDlg = NULL;
}
void CWBToolbarDlg::SetGraphicsDlgNUll()
{
	m_pGraphicsDlg = NULL;
}
void CWBToolbarDlg::SetResourseBarDlgNUll()
{
	m_pResourseBarDlg = NULL;
}
void CWBToolbarDlg::SetTextDlgNUll()
{
	m_pTextDlg = NULL;
}
void CWBToolbarDlg::SetColorPatetteDlgNULL()
{
	m_pColorPatetteDlg = NULL;
}
void CWBToolbarDlg::SetToolboxDlgNULL()
{
	m_pToolboxdlg = NULL;
}
void CWBToolbarDlg::SetFileDlgNULL()
{
	m_pFileDlg = NULL;
}
void CWBToolbarDlg::SetDesktopDrawDlgNULL()
{
	m_pDesktopDrawDlg = NULL;
}
void CWBToolbarDlg::SetToolDlgNULL()
{
	m_pToolDlg = NULL;
}
BOOL CWBToolbarDlg::PreTranslateMessage(MSG* pMsg)
{	
	return CDialog::PreTranslateMessage(pMsg);
}
void CWBToolbarDlg::OnOK()
{
	//CDialog::OnOK();
}
void CWBToolbarDlg::OnCancel()
{
	//CDialog::OnCancel();
}
void CWBToolbarDlg::OnBtnone()
{
	CloseAllChildToolBox();
	CloseToolbox();
	SetChooseBtn(m_pBtnOne);
	ProcessCmd(m_pBtnOne->GetType(),m_pBtnOne);
		
}
void CWBToolbarDlg::OnBtntwo()
{
	CloseAllChildToolBox();
	CloseToolbox();
	SetChooseBtn(m_pBtnTwo);
	ProcessCmd(m_pBtnTwo->GetType(),m_pBtnTwo);

}
void CWBToolbarDlg::OnBtnthree()
{
	CloseAllChildToolBox();
	CloseToolbox();
	SetChooseBtn(m_pbtnThree);
	ProcessCmd(m_pbtnThree->GetType(),m_pbtnThree);

}
void CWBToolbarDlg::OnBtnfour()
{
	CloseAllChildToolBox();
	CloseToolbox();
	SetChooseBtn(m_pBtnFour);
	ProcessCmd(m_pBtnFour->GetType(),m_pBtnFour);

}
void CWBToolbarDlg::OnBtnfive()
{
	CloseAllChildToolBox();
	CloseToolbox();
	SetChooseBtn(m_pBtnFive);
	ProcessCmd(m_pBtnFive->GetType(),m_pBtnFive);

}
void CWBToolbarDlg::OnBtnsix()
{
	CloseAllChildToolBox();
	CloseToolbox();
	SetChooseBtn(m_pBtnSix);
	ProcessCmd(m_pBtnSix->GetType(),m_pBtnSix);

}
void CWBToolbarDlg::OnBtnseven()
{
	CloseAllChildToolBox();
	CloseToolbox();
	SetChooseBtn(m_pBtnSeven);
	ProcessCmd(m_pBtnSeven->GetType(),m_pBtnSeven);

}
void CWBToolbarDlg::OnBtneight()
{
	CloseAllChildToolBox();
	CloseToolbox();
	SetChooseBtn(m_pBtnEight);
	ProcessCmd(m_pBtnEight->GetType(),m_pBtnEight);

}
void CWBToolbarDlg::OnBtnnine()
{
	CloseAllChildToolBox();
	CloseToolbox();
	SetChooseBtn(m_pBtnNine);
	ProcessCmd(m_pBtnNine->GetType(),m_pBtnNine);
	
}
void CWBToolbarDlg::OnBtnten()
{
	CloseAllChildToolBox();
	CloseToolbox();
	SetChooseBtn(m_pBtnTen);
	ProcessCmd(m_pBtnTen->GetType(),m_pBtnTen);

}


void CWBToolbarDlg::OnBtneleven()
{
	CloseAllChildToolBox();
	CloseToolbox();
	SetChooseBtn(m_pBtnEleven);
	ProcessCmd(m_pBtnEleven->GetType(),m_pBtnEleven);
		
}

void CWBToolbarDlg::ReflashBtn(eType btnType)
{
	//修改当与会者打开一个模态对话框时，被请出会议，程序崩溃的BUG。
	//在void CWBToolbarDlg::ReflashBtn()函数中添加窗口句柄判断。
	if (!::IsWindow(this->GetSafeHwnd()))
	{
		return;
	}
	if (NULL != m_pBtnEight &&btnType == m_pBtnEight->GetType())
	{
		m_pBtnEight->Invalidate();
	} 
	else if(NULL != m_pBtnTwo &&btnType == m_pBtnTwo->GetType())
		{
			m_pBtnTwo->Invalidate();
		}
	    else if (NULL != m_pbtnThree && btnType == m_pbtnThree->GetType())
			{
				m_pbtnThree->Invalidate();
			} 
			else if (NULL != m_pBtnFour && btnType == m_pBtnFour->GetType())
				{
					m_pBtnFour->Invalidate();
				}
		        else if (NULL != m_pBtnFive && btnType == m_pBtnFive->GetType())
					{
						m_pBtnFive->Invalidate();
					} 
					else if (NULL != m_pBtnSix && btnType == m_pBtnSix->GetType())
						{
							m_pBtnSix->Invalidate();
						} 
						else if(NULL != m_pBtnSeven)
						{
							m_pBtnSeven->Invalidate();
					    } 
				
					
}
void CWBToolbarDlg::ChangButton(CTransparentButtonEx *pDownBtn,int downY,CTransparentButtonEx * pUpBtn,int upY)
{	
	pDownBtn->SetWindowPos(&wndTop,WB_WIDTH_SPACE,upY,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);	
	pUpBtn->SetWindowPos(&wndTop,WB_WIDTH_SPACE,downY,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);

}

LRESULT CWBToolbarDlg::OnCloseToolbarMessage(WPARAM wParam, LPARAM lParam)
{
	XAutoLock lock(m_csToolBarLock);//wangxiaobin
	CloseAllChildToolBox();
	CloseToolbox();	
	return 0;
}
void CWBToolbarDlg::CloseToolbox()
{
	if (NULL !=m_pToolboxdlg)
	{
		delete m_pToolboxdlg;
		m_pToolboxdlg = NULL;
	}
}

void CWBToolbarDlg::CloseAllChildBox()
{
	CloseAllChildToolBox();
	CloseToolbox();	
}

BOOL CWBToolbarDlg::GetIsFullScreen()
{
	return m_bFullScreen;
}

void CWBToolbarDlg::SetIsFullScreen(BOOL isFullScreen)
{
	m_bFullScreen = isFullScreen;
}

void CWBToolbarDlg::SetNotifyIsSuccessLoad( BOOL bLoadSuccess )
{
	CString btnText = _T("");
	if (bLoadSuccess)
	{
		m_pBtnEleven->SetType(TYPE_SINGLE_LOCAL);//当前是联网状态，显示可以单机
		btnText = GetCaptureFromType(TYPE_SINGLE_LOCAL);
		m_pBtnEleven->SetWindowText(btnText);	
		WB_SetIsLoacalWhiteboard(false);
	} 
	else
	{		
		m_pBtnEleven->SetType(TYPE_SINGLE_NETWORK);//当前是单机状态，显示可以联网
		btnText = GetCaptureFromType(TYPE_SINGLE_NETWORK);
		m_pBtnEleven->SetWindowText(btnText);
		WB_SetIsLoacalWhiteboard(true);
	}
	m_pBtnEleven->Invalidate();
}

void CWBToolbarDlg::setToolbarNum( int num )
{
	m_nToolbarNum = num;
}

void CWBToolbarDlg::SetChooseBtn( CTransparentButtonEx *mptchoosebtn )
{

	if (g_nDrawStatus ==DRAWING_LINE||g_nDrawStatus == DRAWING_ELLIPSE||g_nDrawStatus ==DRAWING_PALLELOGAM||g_nDrawStatus ==DRAW_RECTMAQN||g_nDrawStatus ==DRAWING_POLYLINE
		||g_nDrawStatus ==DRAWING_RECT||g_nDrawStatus ==DRAWING_TRIANGLE||g_nDrawStatus ==DRAWING_POLYGON||g_nDrawStatus ==DRAWING_RHOMBUS||g_nDrawStatus ==DRAWING_PENTAGON||g_nDrawStatus ==DRAWING_HEXAGON||g_nDrawStatus ==DRAWING_FIVEPOINTSTART||g_nDrawStatus ==DRAWING_TRAPEZIUM
		||g_nDrawStatus == ENTER_BUTTON||g_nDrawStatus == ENTER_CONTROL||g_nDrawStatus ==DRAWING_TEXT||g_nDrawStatus == DRAW_SELECTINT
		||g_nDrawStatus ==DRAW_ERASING||g_nDrawStatus ==DRAW_CLEANUPING||g_nDrawStatus == DRAW_RGNDELETING||g_nDrawStatus ==CHANGE_ANNOTATIONS
		||g_nDrawStatus == DRAWINGANYTRIANGLE||g_nDrawStatus ==EDITTRIANGLE)
	{
		CPoint ptcursor;
		GetCursorPos(&ptcursor);
		::SendMessage(g_pWhiteBoardView->GetSafeHwnd(),WM_LBUTTONUP,0,0);
	}

	eType ntp = mptchoosebtn->GetType();
	if (ntp == TYPE_PAGE)
	{
		return;
	}
	if (ntp == TYPE_DLG_LINETYPE|| ntp == TYPE_DLG_RUBBER|| ntp == TYPE_DLG_GRAPHICS)
	{
		if (m_pBtnOne)
		{
			m_pBtnOne->SetButtonStatus(FALSE);
			m_pBtnOne->Invalidate();
			m_pBtnOne->UpdateWindow();
		}
		if (m_ptChooseBtn)
		{
			m_ptChooseBtn->SetButtonStatus(FALSE);
			m_ptChooseBtn->Invalidate();
			m_ptChooseBtn->UpdateWindow();
		}
		m_ptChooseBtn = mptchoosebtn;
		m_ptChooseBtn->SetButtonStatus(TRUE);
		m_ptChooseBtn->Invalidate();
		
	}
	else
	{
		if (ntp == TYPE_DLG_COLORPALETTE)
		{
			return;
		}

		if (m_ptChooseBtn)
		{
			m_ptChooseBtn->SetButtonStatus(FALSE);
			m_ptChooseBtn->Invalidate();
			m_ptChooseBtn->UpdateWindow();
		}

		
		g_nDrawStatus = DRAW_NORMAL;
		m_pBtnOne->SetButtonStatus(TRUE);
		m_pBtnOne->Invalidate();
	}
// 	if (m_ptChooseBtn)
// 	{
// 		m_ptChooseBtn->SetButtonStatus(FALSE);
// 		m_ptChooseBtn->Invalidate();
// 		m_ptChooseBtn->UpdateWindow();
// 	}
// 	m_ptChooseBtn = mptchoosebtn;
// 
// 	m_ptChooseBtn->SetButtonStatus(TRUE);
// 	m_ptChooseBtn->Invalidate();
}

void CWBToolbarDlg::CheckBtnInfo()
{
	CString strSelection = _T("ButtonType");
	CString strPosition[16] = {_T("btnTwo"),_T("btnThree"),_T("btnFour"),_T("btnFive"),_T("btnSix"),_T("btnSeven"),_T("btnEight"),
		_T("btnA"),_T("btnB"),_T("btnC"),_T("btnD"),_T("btnE"),_T("btnF"),_T("btnG"),_T("btnH"),_T("btnJ")};
	int iValue = 0 ;
	int AllValue[16] = {22,7,56,58,59,57,64,36,4,62,69,63,61,60,44,85};
	CString errString[16] = {_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T("")};
	if (g_WBType == WB_SINGLERVER)
	{
		CString csINIPath = _T("C:\\Program Files\\avcon\\ExePath.ini");
		int nIsXPE = 0;
		nIsXPE = GetPrivateProfileInt(_T("AVCON_SELECTED"),_T("AVCONXPE"),0,csINIPath);
		if (nIsXPE)
		{
			AllValue[15] = 84;
		}
		else
		{
			AllValue[15] = 85;
		}
		
	}
	else
		AllValue[15] = 8;
	int nErrStr = 0;
	for (int i = 0 ; i<16;i++)
	{
		iValue = ReadWBConfigPathINI(strSelection,strPosition[i]);
		bool isfind = false;
		for (int j = 0;j<16;j++)
		{
			if (!isfind)
			{
				if (iValue == AllValue[j])
				{
					if (m_pCheckBtnInfo->empty())
					{
						m_pCheckBtnInfo->insert(pair<int,CString>(iValue,strPosition[i]));
						isfind = true;
					}
					else
					{
						std::map<int,CString>::iterator it = m_pCheckBtnInfo->find(iValue);
						if (it!= m_pCheckBtnInfo->end())
						{

							errString[nErrStr] = strPosition[i];
							nErrStr++;
							isfind = true;

						}
						else
						{
							m_pCheckBtnInfo->insert(pair<int,CString>(iValue,strPosition[i]));
							isfind = true;
						}
					}

				}
				else
				{
					if (j==15)
					{
						errString[nErrStr] = strPosition[i];
						nErrStr++;	
						isfind = true;
					}
				}			
			}
		}
		iValue = 0;
		
	}
	if (nErrStr != 0)
	{
		int errorValue[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		int pos = 0;
		for (int i = 0;i<16;i++)
		{
			std::map<int,CString>::iterator it = m_pCheckBtnInfo->find(AllValue[i]);
			if (it != m_pCheckBtnInfo->end())
			{
			}
			else
			{
				errorValue[pos] = AllValue[i];
				pos++;
			}
		}
		
		CString Newstring = _T("");
		for (int i = 0;i<pos;i++)
		{			
			Newstring.Format(_T("%d"),errorValue[i]);
			WriteWBConfigPathINI(strSelection,errString[i],Newstring);				
					
		}		
	}	
}

void CWBToolbarDlg::SetWBFullScreen( bool isFull )
{
	
	m_bFullScreen = isFull;
	ReflashBtn(TYPE_FULLSCREEN);
}
