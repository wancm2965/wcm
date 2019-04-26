// WBBackgroundDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBBackgroundDlg.h"
#include "afxdialogex.h"
#include "WBFunctionDlg.h"

// CWBBackgroundDlg 对话框
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CWBBackgroundDlg, CDialogEx)

	CWBBackgroundDlg::CWBBackgroundDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBBackgroundDlg::IDD, pParent)
{
	m_pWBDesignFillPad = NULL;
	m_pColorPallete=NULL;
	//m_pClrList = NULL;
}

CWBBackgroundDlg::~CWBBackgroundDlg()
{
	if (m_pWBDesignFillPad != NULL)
	{
		delete m_pWBDesignFillPad;
		m_pWBDesignFillPad = NULL;
	}
	if(m_pColorPallete)
	{
		CloseColorPaletteDlg();
	}
	if(m_Radio)
	{
		delete m_Radio;
		m_Radio=NULL;
	}
// 	if (m_pClrList != NULL)
// 	{
// 		delete m_pClrList;
// 		m_pClrList = NULL;
// 	}
}

void CWBBackgroundDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_WB_BGDLG_RADIOFORCLRPAD, m_Radio);
}


BEGIN_MESSAGE_MAP(CWBBackgroundDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_WB_BGDLG_RADIOFORCLRPAD, &CWBBackgroundDlg::OnBnClickedWbBgdlgRadioforclrpad)
	ON_BN_CLICKED(IDC_WB_BGDLG_RADIOFODESIGNPAD, &CWBBackgroundDlg::OnBnClickedWbBgdlgRadiofodesignpad)
	ON_BN_CLICKED(IDC_WB_BGDLG_RADIOFORPICPAD, &CWBBackgroundDlg::OnBnClickedWbBgdlgRadioforpicpad)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_WB_BGDLG_CLRLIST, &CWBBackgroundDlg::OnLvnItemchangedSinwbBgdlgClrlist)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CWBBackgroundDlg 消息处理程序

void CWBBackgroundDlg::OnOK()//屏蔽回车键
{
	return;
}

BOOL CWBBackgroundDlg::OnEraseBkgnd(CDC* pDC)
{
	//对话框背景	
	//CString CurPicPath = GetCurrentPath();	//获取当前程序执行的目录
	CImage img;
	CRect clicentRc;
	GetClientRect(&clicentRc);
	CString CurPicPath = GetCurrentPath() + "\\SKIN\\WhiteBoard\\image\\functionDlg\\DlgBG.png";
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw(pDC->m_hDC,clicentRc);
	}	

	return TRUE;
}

BOOL CWBBackgroundDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	m_Radio = new CTransparentButtonEx;
	m_Radio->Create(_T(""),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS,CRect(50,50,156,130),this,IDC_WB_BGDLG_RADIOFORCLRPAD);
	m_Radio->SetType(TYPE_COLORRADIO);
	m_Radio->ShowWindow(SW_HIDE);


	CRect nParentRc;//控制面板显示区域
	((CWBFunctionDlg *)GetParent())->GetClientRect(&nParentRc);
	MoveWindow(nParentRc.left + 10,nParentRc.top + 30 ,WB_FUNCTION_DLG_WIDTH - 15,nParentRc.bottom - nParentRc.top - 65);
	((CButton *)GetDlgItem(IDC_WB_BGDLG_RADIOFORCLRPAD))->SetCheck(false);
    ((CButton *)GetDlgItem(IDC_WB_BGDLG_RADIOFODESIGNPAD))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_WB_BGDLG_RADIOFORCLRPAD))->MoveWindow(WB_FILLPAD_BTN_SPACE,30,70,20);
	((CButton *)GetDlgItem(IDC_WB_BGDLG_RADIOFODESIGNPAD))->MoveWindow(WB_FILLPAD_BTN_SPACE + 80,30,70,20);
	((CButton *)GetDlgItem(IDC_WB_BGDLG_RADIOFORPICPAD))->MoveWindow(WB_FILLPAD_BTN_SPACE,60,70,20);
	//m_pClrList = new CListCtrl;
	RECT rc;
	rc.left = WB_FILLPAD_BTN_SPACE;
	rc.top = 100;
	rc.right = rc.left + WB_FILLPAD_WIDTH;
	rc.bottom = rc.top + WB_FILLPAD_HEIGHT;
// 	m_pClrList->Create(LVS_ICON|LVS_NOSCROLL|LVS_SINGLESEL|LVS_ALIGNTOP|LVS_ALIGNLEFT,rc,this,IDC_WB_RESDLG_LISTCTL);
// 	m_pClrList->ShowWindow(SW_SHOW);
	
	if (m_pWBDesignFillPad == NULL)
	{
		m_pWBDesignFillPad = new CWBDesignFillPad;
		m_pWBDesignFillPad->Create(IDD_SINWB_BGDLG_DESIGNPAD,this);
		m_pWBDesignFillPad->MoveWindow(10,100,WB_FILLPAD_WIDTH,WB_FILLPAD_HEIGHT);
		m_pWBDesignFillPad->ShowWindow(SW_SHOW);
	}
	
	return TRUE;
}

CWBDesignFillPad *CWBBackgroundDlg::GetDesignFillPad()
{
	return m_pWBDesignFillPad;
}
void CWBBackgroundDlg::OnBnClickedWbBgdlgRadioforclrpad()
{
	// TODO: 在此添加控件通知处理程序代码
	if ((m_pWBDesignFillPad != NULL) && (m_pWBDesignFillPad->IsWindowVisible()))
	{
		m_pWBDesignFillPad->ShowWindow(SW_HIDE);
	}
	if (NULL != m_pColorPallete)
	{
		delete m_pColorPallete;
		m_pColorPallete = NULL;
	}
	
	if (!m_pColorPallete)
	{
		m_pColorPallete = new CWBColorPaletteDlg(m_Radio,this);
		m_pColorPallete->Create(CWBColorPaletteDlg::IDD,this);
		m_pColorPallete->ShowWindow(SW_SHOW);
	}
	else
	{
		if(SW_HIDE == m_pColorPallete->IsWindowVisible())
		{

			CRect colPalletrect;
			m_pColorPallete->GetClientRect(&colPalletrect);

			int height=colPalletrect.Height();
			int width=colPalletrect.Width();
			int colorPalletDlgX=0;
			int colorPalletDlgY=0;
			CRect solidColBtnRect;
			m_Radio->GetClientRect(&solidColBtnRect);//获得按钮的位置大小
			ClientToScreen(&solidColBtnRect);//将按钮的位置坐标转换为屏幕坐标
			colorPalletDlgY=solidColBtnRect.bottom+100+200;

			colorPalletDlgX=solidColBtnRect.right-width+80-4;//根据按钮的屏幕坐标设定调色板的坐标

			//ScreenToClient(CRect(colorPalletDlgX,colorPalletDlgY,width,height));
			//m_pAttriButeColorPatetteDlg->SetWindowPos(&wndTop,colorPalletDlgX,colorPalletDlgY,width,height,SWP_SHOWWINDOW);
			m_pColorPallete->MoveWindow(colorPalletDlgX,colorPalletDlgY,width,height,TRUE);
			m_pColorPallete->ShowWindow(SW_SHOW);
			// 			CWnd *pWnd=GetDlgItem(IDC_SINATRIBUTE_CHECK_SOLID);
			// 			pWnd->EnableWindow(FALSE);
		}
		else
		{
			CloseColorPaletteDlg();
		}	
	}

	/*
	CColorDialog   colorDlgBox(0,0,this);
	
	if   (   colorDlgBox.DoModal()  !=   IDOK) 
	{ 
		//ColorDlgBox.SetCurrentColor(g_clrWBCurObjColor);
		return;
	}
	

	COLORREF color=RGB(255, 255, 255);
    color=colorDlgBox.GetColor();
	g_pWBPage->SetBKGColor(color);//在页里面保存背景颜色
	
	CWBObject *pBkgObj=g_pWBPage->GetBkgObj();
	POSITION pos;
	if(pBkgObj)
	{
	   pBkgObj->SetBkg(FALSE);
      // pos=pObjList->Find(pBkgObj);
	   //pObjList->RemoveAt(pos);
	   //delete pBkgObj;
	  // pBkgObj = NULL;
	}
	CDC *dc=GetDC();
	if(dc)
	{
        CDC pDC;
		CBitmap  bitmap;
		bitmap.CreateCompatibleBitmap(dc,WB_VIEWHSCROLL,WB_VIEWVSCROLL);
		pDC.DeleteDC();
		pDC.CreateCompatibleDC(dc);
		pDC.SelectObject(bitmap);
		pDC.FillSolidRect(0,0,WB_VIEWHSCROLL,WB_VIEWVSCROLL,color);
		g_pWBPage->SetPageDC(&pDC);	 

		//触发重绘事件
		g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		g_pWhiteBoardView->Invalidate();
		//g_pWhiteBoardView->UpdateWindow();
	    bitmap.DeleteObject();
		pDC.DeleteDC();
		
		
	}
	*/
	


	
	return;
}

void CWBBackgroundDlg::OnBnClickedWbBgdlgRadiofodesignpad()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_pWBDesignFillPad == NULL)
	{
		m_pWBDesignFillPad = new CWBDesignFillPad;
		m_pWBDesignFillPad->Create(IDD_SINWB_BGDLG_DESIGNPAD,this);
		m_pWBDesignFillPad->MoveWindow(10,100,WB_FILLPAD_WIDTH,WB_FILLPAD_HEIGHT);
		m_pWBDesignFillPad->ShowWindow(SW_SHOW);
	}
	else if (!m_pWBDesignFillPad->IsWindowVisible())
	{
		m_pWBDesignFillPad->ShowWindow(SW_SHOW);
	}
// 	if (m_pClrList->IsWindowVisible())
// 	{
// 		m_pClrList->ShowWindow(SW_HIDE);
// 	}
}


void CWBBackgroundDlg::OnBnClickedWbBgdlgRadioforpicpad()
{
	// TODO: 在此添加控件通知处理程序代码
	if ((m_pWBDesignFillPad != NULL) && (m_pWBDesignFillPad->IsWindowVisible()))
	{
		m_pWBDesignFillPad->ShowWindow(SW_HIDE);
	}
// 	if (m_pClrList->IsWindowVisible())
// 	{
// 		m_pClrList->ShowWindow(SW_HIDE);
// 	}
	
	CWBObject *pBkgObj = g_pWBPage->GetBkgObj();//得到背景对象
	CObList * pList = g_pWBPage->GetWBObjList();//得到对象列表
	
	TCHAR lpszFilter[] = _T("JPEG (*.jpg;*.jpeg;*.jpe;*jfif)|*.jpg;*.jpeg;*.jpe;*jfif")
		_T("|GIF (*.gif)|*.gif|位图文件(*.bmp;*.dib)|*.bmp;*.dib;")
		_T("|TIFF (*.tif;*tiff)|*.tif;*tiff |PNG (*.png)|*.png")
		_T("|ICO (*.ico)|*.ico")
		_T("|所有图片文件|*.jpg;*.jpeg;*.jpe;*jfif;*.gif;*.bmp;*.dib;*.tif;*tiff;*.ico;*.png||");
	CString strFilePath;
	if (g_bSupportU7OpenFile && g_pHPWBDataNotify)
	{
		char* sFilePath = new char[MAX_PATH];
		bool bOpen = g_pHPWBDataNotify->OpenSaveFile(TRUE,sFilePath, NULL, lpszFilter, g_pWhiteBoardView->GetSafeHwnd());
		if (bOpen)
		{
			strFilePath = A2U(sFilePath);
			delete []sFilePath;
		}
		else
		{
			delete []sFilePath;
			return;
		}
	}
	else
	{
		CFileDialog  dlgFile(TRUE, _T("文件"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, lpszFilter, NULL, 0, FALSE);

		dlgFile.m_ofn.nFilterIndex=7;//设置默认类型为所有图片文件
		if (dlgFile.DoModal() == IDOK)
		{
			strFilePath = dlgFile.GetPathName();
		}
	}
	CString strFilePathError;
	strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
	if (strFilePathError.IsEmpty())
	{
		strFilePathError = _T("文件路径输入有误！");
	}
	CString strIsBackGround;
	strIsBackGround = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsBackGround"));
	if (strIsBackGround.IsEmpty())
	{
		strIsBackGround = _T("是否拉伸作为背景！");
	}
	CString strYesOrNot;
	strYesOrNot = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("YesOrNot"));
	if (strYesOrNot.IsEmpty())
	{
		strYesOrNot = _T("拉伸（是）  平铺（否）！");
	}
	CString strPrompt;
	strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
	if (strPrompt.IsEmpty())
	{
		strPrompt = _T("提示");
	}
	if (!PathFileExists(strFilePath))
	{
		if (g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strFilePathError, MB_OK,false);
			g_pHPWBDataNotify->ShowTipWindow(strFilePathError);
		}
		else
		{			
			AfxMessageBox(strFilePathError);
		}
		return ;
	}

	int choose;
	if (g_bSupportU7OpenFile && g_pHPWBDataNotify)
	{
		choose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strIsBackGround + "\n\n" + strYesOrNot,MB_YESNO,false);
	}
	else
	{
		choose=AfxMessageBox(strIsBackGround + "\n\n" + strYesOrNot, MB_YESNOCANCEL);
	}
	//g_pWBPage->SetBkgPicChoose(choose);

	CWBPicture *pFillPicture = new CWBPicture(strFilePath);
	//g_pWBPage->AddWBObject(pFillPicture);
	//CWBObject *obj=(CWBObject*)pFillPicture;//得到当前选择的对象，设为背景，去除已经存在的背景
	//obj->SetBkg(TRUE);
	if(choose == IDYES)
	{
		//CWBPicture *pPic=new CWBPicture(*str);//从对象列表中取出对象并转换成CWBPicture对象
		if(pBkgObj)  //设置背景时删除已有的背景对象
		{
			pBkgObj->SetBkg(FALSE);
			//pos=pList->Find(pBkgObj);
			//pList->RemoveAt(pos);

		}
		ImageEx *pImage=pFillPicture->GetImage();//获得Image对象
		if(NULL==pImage)
		{
			return;
		}

		CDC *pDC=g_pWBPage->GetPageDC();

		CBitmap bitmap ;
		bitmap.CreateCompatibleBitmap(pDC,g_nWidth,g_nHeight);
		pDC->SelectObject(bitmap);
		pImage->Draw(pDC->m_hDC, CRect(0,0,g_nWidth,g_nHeight),255);//将image画到临时dc里面
		g_pWBPage->SetPageDC(pDC);
	}
	if(choose == IDNO)
	{
		if(pBkgObj)  //设置背景时删除已有的背景对象
		{
			pBkgObj->SetBkg(FALSE);
			//pos=pList->Find(pBkgObj);
			//pList->RemoveAt(pos);

		}
		g_pWBPage->CaclBigPic(&strFilePath);
	}
	delete pFillPicture;
	pFillPicture=NULL;
	
	g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
	g_pWhiteBoardView->Invalidate();
	
	
	return;
}


void CWBBackgroundDlg::OnLvnItemchangedSinwbBgdlgClrlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


HBRUSH CWBBackgroundDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	switch (pWnd->GetDlgCtrlID()){
	case IDC_WB_BGDLG_RADIOFORCLRPAD:
		pDC->SetBkMode(TRANSPARENT);//背景透明
		//pDC->SetTextColor(RGB(255,255,255)); //字体颜色
		return   HBRUSH(GetStockObject(HOLLOW_BRUSH));
		break;
	case IDC_WB_BGDLG_RADIOFODESIGNPAD:
		pDC->SetBkMode(TRANSPARENT);//背景透明
		//pDC->SetTextColor(RGB(255,255,255)); //字体颜色
		return   HBRUSH(GetStockObject(HOLLOW_BRUSH));
		break;
	case IDC_WB_BGDLG_RADIOFORPICPAD:

		pDC->SetBkMode(TRANSPARENT);//背景透明
		//pDC->SetTextColor(RGB(255,255,255)); //字体颜色
		return   HBRUSH(GetStockObject(HOLLOW_BRUSH));
		break;
	default:
		break;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CWBBackgroundDlg::CloseColorPaletteDlg()
{
	if(m_pColorPallete)
	{
		delete m_pColorPallete;
		m_pColorPallete = NULL;
	}

}
