// WBResourseBarDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBResourseBarDlg.h"
#include "afxdialogex.h"
#include "WBToolbarDlg.h"
#include "WBToolboxDlg.h"

// CWBResourseBarDlg 对话框

IMPLEMENT_DYNAMIC(CWBResourseBarDlg, CDialog)

CWBResourseBarDlg::CWBResourseBarDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent /*=NULL*/)
	: CDialog(CWBResourseBarDlg::IDD, pParent)
	, m_pBitmap(NULL)
{
	m_pParentBtn = pParentBtn;
	m_pParentDlg = pParent;
}

CWBResourseBarDlg::~CWBResourseBarDlg()
{
	if (NULL != m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap=NULL;
	}
}

void CWBResourseBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_RESBTN, m_imageResBtn);
	DDX_Control(pDX, IDC_VIDEO_RESBTN, m_videoResBtn);
	DDX_Control(pDX, IDC_DOCUMENT_RESBTN, m_doucumentResBtn);
	DDX_Control(pDX, IDC_FLASH_RESBTN, m_flashResBtn);
	DDX_Control(pDX, IDC_CAMERAHEAD_RESBTN, m_cameraHeadRESBtn);
	DDX_Control(pDX, IDC_DICOM_RESBTN, m_dicomResBtn);
	DDX_Control(pDX, IDC_DXF_RESBTN, m_dxfResBtn);
	DDX_Control(pDX,IDC_MIDI_RESBTN,m_midiResBtn);
}


BEGIN_MESSAGE_MAP(CWBResourseBarDlg, CDialog)
	ON_BN_CLICKED(IDC_IMAGE_RESBTN, &CWBResourseBarDlg::OnBnClickedImageResbtn)
	ON_BN_CLICKED(IDC_VIDEO_RESBTN, &CWBResourseBarDlg::OnBnClickedVideoResbtn)
	ON_BN_CLICKED(IDC_DOCUMENT_RESBTN, &CWBResourseBarDlg::OnBnClickedDocumentResbtn)
	ON_BN_CLICKED(IDC_FLASH_RESBTN, &CWBResourseBarDlg::OnBnClickedFlashResbtn)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_CAMERAHEAD_RESBTN, &CWBResourseBarDlg::OnBnClickedCameraheadResbtn)
	ON_BN_CLICKED(IDC_DICOM_RESBTN, &CWBResourseBarDlg::OnBnClickedDicomResbtn)
	ON_BN_CLICKED(IDC_DXF_RESBTN, &CWBResourseBarDlg::OnBnClickedDxfResbtn)
	ON_BN_CLICKED(IDC_MIDI_RESBTN,&CWBResourseBarDlg::OnBnClickedMidiResbtn)
END_MESSAGE_MAP()


// CWBResourseBarDlg 消息处理程序
 BOOL CWBResourseBarDlg:: OnEraseBkgnd(CDC* pDC)
 {
	 CString strPath = _T("");

	 //strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	 //if (g_IsMedicalVersion)
	 //	strPath += _T("\\328DlgBG.png");
	 //else
	 //	strPath += _T("\\278DlgBG.png");

	 strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\DlgBG.png");

	 SetSkinImage(strPath);
	 return TRUE;
 }
void CWBResourseBarDlg::OnOK()
{
	//CDialog::OnOK();
}
void CWBResourseBarDlg::OnCancel()
{
	//CDialog::OnCancel();
}
BOOL CWBResourseBarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	Relayout();
	// 设置对话框背景图和画背景图
	CString strPath = _T("");

	//strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard");
	//if (g_IsMedicalVersion)
	//	strPath += _T("\\328DlgBG.png");
	//else
	//	strPath += _T("\\278DlgBG.png");

	strPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\DlgBG.png");

	SetSkinImage(strPath);
	m_imageResBtn.SetType(TYPE_IMAGE);
	m_imageResBtn.SetWindowText(GetCaptureFromType(TYPE_IMAGE));
	m_videoResBtn.SetType(TYPE_VIDEO);
	m_videoResBtn.SetWindowText(GetCaptureFromType(TYPE_VIDEO));
	m_doucumentResBtn.SetType(TYPE_DOCUMENT);
	m_doucumentResBtn.SetWindowText(GetCaptureFromType(TYPE_DOCUMENT));
	m_flashResBtn.SetType(TYPE_FLASH);
	m_flashResBtn.SetWindowText(GetCaptureFromType(TYPE_FLASH));
	m_cameraHeadRESBtn.SetType(TYPE_CAMERAHEAD);
	m_cameraHeadRESBtn.SetWindowText(GetCaptureFromType(TYPE_CAMERAHEAD));
	m_dicomResBtn.SetType(TYPE_DICOM);
	m_dicomResBtn.SetWindowText(GetCaptureFromType(TYPE_DICOM));
	m_dxfResBtn.SetType(TYPE_DXF);
	m_dxfResBtn.SetWindowText(GetCaptureFromType(TYPE_DXF));
	m_midiResBtn.SetType(TYPE_MIDI);
	m_midiResBtn.SetWindowText(GetCaptureFromType(TYPE_MIDI));
	return TRUE;  
}
void CWBResourseBarDlg::Relayout()
{
	CRect BtnRc,ToolbarRc;
	m_pParentBtn->GetWindowRect(BtnRc);
	m_pParentDlg->GetWindowRect(ToolbarRc);

	int width,height;//设置对话框的宽度和高度
	int toolYBegin , toolXBegin ;    //对话框X,Y轴的起始坐标
	int nBtnCount = 0;

	if (g_bSimplifyWB)
	{
		nBtnCount = 2;
		width = WB_CTRLBTNDLG_WIDTH;   //子工具条的宽度
		height = WB_TOOLBARBTN_SPACE * (nBtnCount + 1) + WB_TOOLBARBTN_HEIGHT * nBtnCount;

		toolXBegin = BtnRc.right +  WB_TOOLBARBTN_SPACE;
		toolYBegin = BtnRc.top;	
		if ((toolYBegin + height) > ToolbarRc.bottom)
		{
			toolYBegin = ToolbarRc.bottom - height;
		}
		SetWindowPos(&wndTop,toolXBegin	,toolYBegin	, width	, height,SWP_SHOWWINDOW);
		HRGN rgnwind = CreateRoundRectRgn(0,0,width,height,6,6);
		SetWindowRgn(rgnwind,TRUE);
		DeleteObject(rgnwind);

		m_imageResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
			,WB_TOOLBARBTN_SPACE + 0 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
			,WB_TOOLBARBTN_WIDTH    //x轴宽
			,WB_TOOLBARBTN_HEIGHT   //y轴高
			,SWP_SHOWWINDOW);
		if (ZSYDZ)
		{
			m_doucumentResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + 1 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
				,WB_TOOLBARBTN_WIDTH    //x轴宽
				,WB_TOOLBARBTN_HEIGHT   //y轴高
				,SWP_SHOWWINDOW);
		}
		else
		{
			m_flashResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + 1 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
				,WB_TOOLBARBTN_WIDTH    //x轴宽
				,WB_TOOLBARBTN_HEIGHT   //y轴高
				,SWP_SHOWWINDOW);
		}

		return;
	}

	if (g_bShowWBNewFun)
	{
		nBtnCount = g_IsMedicalVersion? 6:5 /*8:7*/;
	}
	else
	{
		nBtnCount = g_IsMedicalVersion? 5:4 ;
	}
	width = WB_CTRLBTNDLG_WIDTH;   //子工具条的宽度
	height = WB_TOOLBARBTN_SPACE * (nBtnCount + 1) + WB_TOOLBARBTN_HEIGHT * nBtnCount;

	toolXBegin = BtnRc.right +  WB_TOOLBARBTN_SPACE;
	toolYBegin = BtnRc.top;	
	if ((toolYBegin + height) > ToolbarRc.bottom)
	{
		toolYBegin = ToolbarRc.bottom - height;
	}

	SetWindowPos(&wndTop,toolXBegin	,toolYBegin	, width	, height,SWP_SHOWWINDOW);
	HRGN rgnwind = CreateRoundRectRgn(0,0,width,height,6,6);
	SetWindowRgn(rgnwind,TRUE);
	DeleteObject(rgnwind);
	m_imageResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 0 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_videoResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 1 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_doucumentResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 2 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
	m_flashResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
		,WB_TOOLBARBTN_SPACE + 3 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
		,WB_TOOLBARBTN_WIDTH    //x轴宽
		,WB_TOOLBARBTN_HEIGHT   //y轴高
		,SWP_SHOWWINDOW);
//  	if (!g_bSupportU7OpenFile&&g_IsMedicalVersion)
//  	{
// 		m_cameraHeadRESBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
// 			,WB_TOOLBARBTN_SPACE + 4 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
// 			,WB_TOOLBARBTN_WIDTH    //x轴宽
// 			,WB_TOOLBARBTN_HEIGHT   //y轴高
// 			,SWP_SHOWWINDOW);
// 		m_dicomResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
// 			,WB_TOOLBARBTN_SPACE + 5 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
// 			,WB_TOOLBARBTN_WIDTH    //x轴宽
// 			,WB_TOOLBARBTN_HEIGHT   //y轴高
// 			,SWP_SHOWWINDOW);
//  	}
// 	if (g_bSupportU7OpenFile&&g_IsMedicalVersion)
// 	{
// 		m_cameraHeadRESBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
// 			,WB_TOOLBARBTN_SPACE + 4 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
// 			,WB_TOOLBARBTN_WIDTH    //x轴宽
// 			,WB_TOOLBARBTN_HEIGHT   //y轴高
// 			,SWP_SHOWWINDOW);
// 		m_dicomResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
// 			,WB_TOOLBARBTN_SPACE + 5 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
// 			,WB_TOOLBARBTN_WIDTH    //x轴宽
// 			,WB_TOOLBARBTN_HEIGHT   //y轴高
// 			,SWP_SHOWWINDOW);
// 	}
// 	if (!g_bSupportU7OpenFile&&!g_IsMedicalVersion)
// 	{
// 		m_cameraHeadRESBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
// 			,WB_TOOLBARBTN_SPACE + 4 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
// 			,WB_TOOLBARBTN_WIDTH    //x轴宽
// 			,WB_TOOLBARBTN_HEIGHT   //y轴高
// 			,SWP_SHOWWINDOW);
// // 		m_dicomResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
// // 			,WB_TOOLBARBTN_SPACE + 5 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
// // 			,WB_TOOLBARBTN_WIDTH    //x轴宽
// // 			,WB_TOOLBARBTN_HEIGHT   //y轴高
// // 			,SWP_SHOWWINDOW);
// 	}
// 	m_cameraHeadRESBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
// 		,WB_TOOLBARBTN_SPACE + 4 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
// 		,WB_TOOLBARBTN_WIDTH    //x轴宽
// 		,WB_TOOLBARBTN_HEIGHT   //y轴高
// 		,SWP_SHOWWINDOW);
// 	m_midiResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
// 		,WB_TOOLBARBTN_SPACE + 5 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
// 		,WB_TOOLBARBTN_WIDTH    //x轴宽
// 		,WB_TOOLBARBTN_HEIGHT   //y轴高
// 		,SWP_SHOWWINDOW);
	if (g_bShowWBNewFun)
	{
		m_dxfResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
			,WB_TOOLBARBTN_SPACE + 4 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
			,WB_TOOLBARBTN_WIDTH    //x轴宽
			,WB_TOOLBARBTN_HEIGHT   //y轴高
			,SWP_SHOWWINDOW);
	}
	if (g_IsMedicalVersion)
	{
		if (g_bShowWBNewFun)
		{
			m_dicomResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + /*5*/ 5 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
				,WB_TOOLBARBTN_WIDTH    //x轴宽
				,WB_TOOLBARBTN_HEIGHT   //y轴高
				,SWP_SHOWWINDOW);
		}
		else
		{
			m_dicomResBtn.SetWindowPos(&wndTop	,WB_WIDTH_SPACE
				,WB_TOOLBARBTN_SPACE + 4 *(WB_TOOLBARBTN_SPACE + WB_TOOLBARBTN_HEIGHT )
				,WB_TOOLBARBTN_WIDTH    //x轴宽
				,WB_TOOLBARBTN_HEIGHT   //y轴高
				,SWP_SHOWWINDOW);
		}
	}
	
}
void CWBResourseBarDlg::SetSkinImage(CString strPath)
{
	if (!m_pBitmap)
	{
	//	wchar_t strFilename[MAX_PATH];
		CString picturePath = strPath;
		//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);//将CString类型转换成WCHAR
		m_pBitmap = new Bitmap(picturePath);
	}
	else
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	//	wchar_t strFilename[MAX_PATH];
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
CString CWBResourseBarDlg::GetCaptureFromType(eType btnEmumType)
{
	CString BtnText;
	switch(btnEmumType)
	{
	case TYPE_IMAGE:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_IMAGE"));
			if (BtnText.IsEmpty() )
			{
				return _T("图片");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_VIDEO:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_VIDEO"));
			if (BtnText.IsEmpty() )
			{
				return _T("视频");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_DOCUMENT:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DOCUMENT"));
			if (BtnText.IsEmpty() )
			{
				return _T("文档");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_FLASH:    
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_FLASH"));
			if (BtnText.IsEmpty() )
			{
				return _T("FLASH");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case TYPE_CAMERAHEAD:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_CAMERAHEAD"));
			if (BtnText.IsEmpty() )
			{
				return _T("摄像头");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_DICOM:    
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DICOM"));
			if (BtnText.IsEmpty() )
			{
				return _T("DICOM图片");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case TYPE_DXF:    
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_DXF"));
			if (BtnText.IsEmpty() )
			{
				return _T("DXF");
			} 
			else
			{
				return BtnText;
			}			
		}
		break;
	case TYPE_MIDI:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_MIDI"));
			if (BtnText.IsEmpty())
			{
				return _T("MIDI");
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
void CWBResourseBarDlg::OnBnClickedImageResbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetResourseBarDlgNUll();
		pToolBar->CloseToolbox();	
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_IMAGE,m_pParentBtn);
	}
}
void CWBResourseBarDlg::OnBnClickedVideoResbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetResourseBarDlgNUll();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_VIDEO,m_pParentBtn);
	}
}
void CWBResourseBarDlg::OnBnClickedDocumentResbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetResourseBarDlgNUll();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_DOCUMENT,m_pParentBtn);
	}
}
void CWBResourseBarDlg::OnBnClickedFlashResbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetResourseBarDlgNUll();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_FLASH,m_pParentBtn);
	}
}

void CWBResourseBarDlg::OnBnClickedCameraheadResbtn()
{

	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetResourseBarDlgNUll();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_CAMERAHEAD,m_pParentBtn);
	}
}

void CWBResourseBarDlg::OnBnClickedDicomResbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetResourseBarDlgNUll();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_DICOM,m_pParentBtn);
	}
}

void CWBResourseBarDlg::OnBnClickedDxfResbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetResourseBarDlgNUll();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_DXF,m_pParentBtn);
	}
}

void CWBResourseBarDlg::OnDestroy()
{
	CDialog::OnDestroy();

	delete this;
}


BOOL CWBResourseBarDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CWBResourseBarDlg::OnBnClickedMidiResbtn()
{
	CWBToolbarDlg* pToolBar = (CWBToolbarDlg*)m_pParentDlg;
	if (pToolBar)
	{		
		pToolBar->SetResourseBarDlgNUll();
		pToolBar->CloseToolbox();
		OnDestroy();
		pToolBar->ProcessCmd(TYPE_MIDI,m_pParentBtn);
		//pToolBar->ProcessCmd(TYPE_CAMERAHEAD,m_pParentBtn);
	}
}


