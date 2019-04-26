// WBFunctionBtn.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBFunctionBtn.h"
#include "WBFunctionDlg.h"
#include "ScreenDrawMgr.h"
#include "WhiteBoardDlg.h"


// CWBFunctionBtn

IMPLEMENT_DYNAMIC(CWBFunctionBtn, CBitmapButton)

CWBFunctionBtn::CWBFunctionBtn()
{
	m_bMouseIn = FALSE;
	m_isSelect = FALSE;
	m_mouseState = MOUSE_LBTNUP;
	m_size = 0;
}

CWBFunctionBtn::~CWBFunctionBtn()
{
}

BEGIN_MESSAGE_MAP(CWBFunctionBtn, CBitmapButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()

END_MESSAGE_MAP()



// CWBFunctionBtn 消息处理程序


void CWBFunctionBtn::SetIsSelect(BOOL IsSelect)
{
	m_isSelect = IsSelect;
}
BOOL CWBFunctionBtn::GetIsSelect()
{
	return m_isSelect;
}
void CWBFunctionBtn::SetMouseState(WB_MOUSE_STATE MouseState)
{
	m_mouseState = MouseState;
}
WB_MOUSE_STATE CWBFunctionBtn::GetMouseState()
{
	return m_mouseState;
}

BOOL CWBFunctionBtn::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值


//  	CDC dc;
//  	CImage img;
//  	int  BtnId;
//  	//	WB_MOUSE_STATE MouseState;
//  	CString BtnText;
//  	dc.Attach( pDC->m_hDC ); 
//  	CString CurPicPath = GetCurrentPath();	//获取当前程序执行的目录	
//  	BtnId = GetDlgCtrlID();
// // 	//MouseState = GetMouseState();
// // // 	switch(BtnId)
// // // 	{
// // // 	case IDC_WB_FONTBOLDBTN:
// // // 		{
// // // 			if ( m_isSelect )
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_FontBoldBtn_press.png");
// // // 			} 
// // // 			else
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_FontBoldBtn_normal.png");
// // // 			}
// // // 		}
// // // 		break;
// // // 	case IDC_WB_FONTITALICBTN:
// // // 		{
// // // 			if ( m_isSelect )
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_FontItalicBtn_press.png");
// // // 			} 
// // // 			else
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_FontItalicBtn_normal.png");
// // // 			}
// // // 		}
// // // 		break;
// // // 	case IDC_WB_FONTUNDERLINEBTN:
// // // 		{
// // // 			if ( m_isSelect )
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_FontUnderlineBtn_press.png");
// // // 			} 
// // // 			else
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_FontUnderlineBtn_normal.png");
// // // 			}
// // // 		}
// // // 		break;
// // // 	case IDC_WB_VERTICALBTN:
// // // 		{
// // // 			if ( m_isSelect )
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_VerticalBtn_press.png");
// // // 			} 
// // // 			else
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_VerticalBtn_normal.png");
// // // 			}
// // // 		}
// // // 		break;
// // // 	case IDC_ATRIBUTE_CHECK_SOLID:
// // // 		{
// // // 			if( m_isSelect ) 
// // // 			{ 
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_ChexBtn_press.png");
// // // 			}
// // // 			else
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_ChexBtn_normal.png");
// // // 			}
// // // 		}
// // // 		break;
// // // 	case IDC_SINWB_CLOSEFUNCPAD:
// // // 		{
// // // 			switch (m_mouseState)
// // // 			{
// // // 			case MOUSE_LBTNUP:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_normal.png");
// // // 				break;
// // // 			case MOUSE_HOVER:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_press.png");
// // // 				break;
// // // 			case MOUSE_LEAVE:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_normal.png");
// // // 				break;
// // // 			case MOUSE_LBTNDOWN:
// // // 				CurPicPath +=  _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_normal.png");
// // // 				break;
// // // 			default:
// // // 				break;
// // // 			}
// // // 		}
// // // 		break;
// // // 	case IDC_CLOSEWBFUNCTIONDLG:
// // // 		{
// // // 			switch (m_mouseState)
// // // 			{
// // // 			case MOUSE_LBTNDOWN:
// // // 				CurPicPath +=  _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_press.png");
// // // 				break;
// // // 			case MOUSE_LBTNUP:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_normal.png");
// // // 				break;
// // // 			case MOUSE_HOVER:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_press.png");
// // // 				break;
// // // 			case MOUSE_LEAVE:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_normal.png");
// // // 				break;
// // // 			default:
// // // 				CurPicPath +=  _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_normal.png");
// // // 				break;
// // // 			}				
// // // 		}
// // // 		break;
// // // 	case IDC_BTN_PREVIEW:
// // // 		{
// // // 			switch (m_mouseState)
// // // 			{
// // // 			case MOUSE_HOVER:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_BtnLeftDown_press.png");
// // // 				break;
// // // 			case MOUSE_LEAVE:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_BtnLeftDown_normal.png");
// // // 				break;
// // // 			case MOUSE_LBTNDOWN:
// // // 				CurPicPath +=  _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_BtnLeftDown_press.png");
// // // 				break;
// // // 			case MOUSE_LBTNUP:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_BtnLeftDown_normal.png");
// // // 				break;
// // // 			default:
// // // 				CurPicPath +=  _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_BtnLeftDown_normal.png");
// // // 				break;
// // // 			}
// // // 		}
// // // 		break;
// // // 	case IDB_BTN_EAGLECLOSE:
// // // 		{
// // // 			switch(m_mouseState)
// // // 			{
// // // 			case MOUSE_LBTNDOWN:
// // // 				CurPicPath +=  _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cEagleEyeCloseButton_press.png");
// // // 				break;
// // // 			case MOUSE_LBTNUP:
// // // 				CurPicPath +=  _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cEagleEyeCloseButton_normal.png");
// // // 				break;
// // // 			default:
// // // 				CurPicPath +=  _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cEagleEyeCloseButton_normal.png");
// // // 				break;
// // // 			}
// // // 		}
// // // 		break;
// // // 	case IDC_ATRIBUTE_ADDTRANSPARBTN:
// // // 		{
// // // 			switch (m_mouseState)
// // // 			{
// // // 			case MOUSE_LBTNDOWN:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cAddTransparBtn_press.png");
// // // 				break;
// // // 			case MOUSE_LBTNUP:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cAddTransparBtn_normal.png");
// // // 				break;
// // // 			case MOUSE_HOVER:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cAddTransparBtn_press.png");
// // // 				break;
// // // 			case MOUSE_LEAVE:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cAddTransparBtn_normal.png");
// // // 				break;
// // // 			default:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cAddTransparBtn_normal.png");
// // // 				break;
// // // 			}			
// // // 		}
// // // 		break;
// // // 	case IDC_ATRIBUTE_REDUCETRANSPARBTN://透明度滑块的减
// // // 		{
// // // 			switch (m_mouseState)
// // // 			{
// // // 			case MOUSE_LBTNDOWN:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cReduceTransperBtn_press.png");
// // // 				break;
// // // 			case MOUSE_LBTNUP:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cReduceTransperBtn_normal.png");
// // // 				break;
// // // 			case MOUSE_HOVER:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cReduceTransperBtn_press.png");
// // // 				break;
// // // 			case MOUSE_LEAVE:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cReduceTransperBtn_normal.png");
// // // 				break;
// // // 			default:
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cReduceTransperBtn_normal.png");
// // // 				break;
// // // 			}	
// // // 		}
// // // 		break;
// // // 	case IDC_WB_MT_ROTATE:
// // // 		{
// // // 			if( m_isSelect ) 
// // // 			{ 
// // // 				CurPicPath +=  _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cFuncDlgMTRotate_select.png");
// // // 			}
// // // 			else
// // // 			{
// // // 				CurPicPath +=  _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cFuncDlgMTRotate_normal.png");
// // // 			}
// // // 		}
// // // 		break;
// // // 	case IDC_WB_MT_ZOOM:
// // // 		{
// // // 			if( m_isSelect ) 
// // // 			{ 
// // // 				CurPicPath +=  _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cFuncDlgMTZoom_select.png");
// // // 			}
// // // 			else
// // // 			{
// // // 				CurPicPath +=  _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cFuncDlgMTZoom_normal.png");
// // // 			}
// // // 		}
// // // 		break;
// // // 	case IDC_PROMPTFONTBOLDBTN:
// // // 		{
// // // 			if ( m_isSelect )
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\m_PromptBoldBtn_press.png");
// // // 			} 
// // // 			else
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\m_PromptFontBoldBtn_normal.png");
// // // 			}
// // // 		}
// // // 		break;
// // // 	case  IDC_PROMPTFONTITALICBTN:
// // // 		{
// // // 			if ( m_isSelect )
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\m_PromptItalicBtn_press.png");
// // // 			} 
// // // 			else
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\m_PromptItalicBtn_normal.png");
// // // 			}
// // // 		}
// // // 		break;
// // // 	case  IDC_PROMPTFONTUNDERLINEBTN:
// // // 		{
// // // 			if ( m_isSelect )
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\m_PromptUnderlineBtn_press.png");
// // // 			} 
// // // 			else
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\m_PromptUnderlineBtn_normal.png");
// // // 			}
// // // 		}
// // // 		break;
// // // 	case  IDC_PROMPTFONTVERTICALBTN:
// // // 		{
// // // 			if ( m_isSelect )
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\m_PromptVerticalBtn_press.png");
// // // 			} 
// // // 			else
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\m_PromptVerticalBtn_normal.png");
// // // 			}
// // // 		}
// // // 		break;
// // // 	case IDC_WB_HIDEFUNCDLG:
// // // 		{
// // // 			if( m_isSelect ) 
// // // 			{ 
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_HideFuncdlg_press.png");
// // // 			}
// // // 			else
// // // 			{
// // // 				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_HideFuncdlg_normal.png");
// // // 			}
// // // 		}
// // // 		break;
// // // 	default:
// // // 		break;
// // // 	}
// // // 
// // // 	img.Load( CurPicPath );
// // // 	if (!img.IsNull())
// // // 	{
// // // 		img.Draw( dc.m_hDC, lpDS->rcItem );
// // // 	}
//  	if (BtnId == IDC_WB_PREVIEWBTN)
//  	{
//  		dc.SetBkMode(TRANSPARENT);
//  		CFont myfont;
//  		myfont.CreateFont(12,8,2700,0,10,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,_T("@system"));
//  		CFont *oldf = dc.SelectObject(&myfont);
//  		if (m_isSelect)
//  		{
//  			CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_press.png");
//  			img.Load(CurPicPath);
//  			if (!img.IsNull())
//  			{
//  				img.Draw(dc.m_hDC,CRect(0,0,30,60));
//  			}
//  
//  		}
//  		else
//  		{
//  			CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_normal.png");
//  			img.Load(CurPicPath);
//  			if (!img.IsNull())
//  			{
//  				img.Draw(dc.m_hDC,CRect(0,0,30,60));
//  			}
//  
//  		}
//  		CString staticText;
//  		staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("PreView"));
//  		if (staticText.IsEmpty())
//  		{
//  			staticText = _T("预 览");
//  			dc.TextOut(21,15,staticText);
//  		}
//  		else
//  		{
//  
//  			dc.TextOut(21,5,staticText);
//  
//  		}
//  		//dc.TextOut(lpDS->rcItem.left+21,lpDS->rcItem.top+15,staticText);
//  		dc.SelectObject(oldf);
//  		myfont.DeleteObject();
//  	}
//  	if (BtnId ==IDC_WB_ATTRIBUTEBTN)
//  	{
//  		dc.SetBkMode(TRANSPARENT);
//  		CFont myfont;
//  		myfont.CreateFont(12,8,2700,0,10,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,_T("@system"));
//  		CFont *oldf = dc.SelectObject(&myfont);
//  		if (m_isSelect)
//  		{
//  			CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_press.png");
//  			img.Load(CurPicPath);
//  			if (!img.IsNull())
//  			{
//  				img.Draw(dc.m_hDC,CRect(0,0,30,60));
//  			}
//  
//  		}
//  		else
//  		{
//  			CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_normal.png");
//  			img.Load(CurPicPath);
//  			if (!img.IsNull())
//  			{
//  				img.Draw(dc.m_hDC,CRect(0,0,30,60));
//  			}
//  
//  		}
//  		CString staticText;
//  		staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Attribute"));
//  		if (staticText.IsEmpty())
//  		{
//  			staticText = _T("属 性");
//  			dc.TextOut(21,15,staticText);
//  		}
//  		else
//  		{
//  			dc.TextOut(21,5,staticText);
//  		}
//  		//dc.DrawText(staticText,&lpDS->rcItem,DT_CENTER);
//  		// dc.TextOut(lpDS->rcItem.left+21,lpDS->rcItem.top+15,staticText);
//  		dc.SelectObject(oldf);
//  		myfont.DeleteObject();
//  
//  	}
// // 
// // 	if (BtnId == IDC_WB_RESBTN)
// // 	{
// // 		dc.SetBkMode(TRANSPARENT);
// // 		CFont myfont;
// // 		myfont.CreateFont(12,8,2700,0,10,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,_T("@system"));
// // 		CFont *oldf = dc.SelectObject(&myfont);
// // 		if (m_isSelect)
// // 		{
// // 			CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_press.png");
// // 			img.Load(CurPicPath);
// // 			if (!img.IsNull())
// // 			{
// // 				img.Draw(dc.m_hDC,CRect(0,0,30,60));
// // 			}
// // 
// // 		}
// // 		else
// // 		{
// // 			CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_normal.png");
// // 			img.Load(CurPicPath);
// // 			if (!img.IsNull())
// // 			{
// // 				img.Draw(dc.m_hDC,CRect(0,0,30,60));
// // 			}
// // 
// // 		}
// // 		CString staticText;
// // 		staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Resouce"));
// // 		if (staticText.IsEmpty())
// // 		{
// // 			staticText = _T("资 源");
// // 			dc.TextOut(21,15,staticText);
// // 		}
// // 		else
// // 		{
// // 			dc.TextOut(21,5,staticText);
// // 		}
// // 		dc.SelectObject(oldf);
// // 		myfont.DeleteObject();
// // 	}
// // 	if (BtnId == IDC_WB_BACKGROUNDBTN)
// // 	{
// // 		dc.SetBkMode(TRANSPARENT);
// // 		CFont myfont;
// // 		myfont.CreateFont(12,8,2700,0,10,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,_T("@system"));
// // 		CFont *oldf = dc.SelectObject(&myfont);
// // 		if (m_isSelect)
// // 		{
// // 			CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_press.png");
// // 			img.Load(CurPicPath);
// // 			if (!img.IsNull())
// // 			{
// // 				img.Draw(dc.m_hDC,CRect(0,0,30,60));
// // 			}
// // 
// // 		}
// // 		else
// // 		{
// // 			CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_normal.png");
// // 			img.Load(CurPicPath);
// // 			if (!img.IsNull())
// // 			{
// // 				img.Draw(dc.m_hDC,CRect(0,0,30,60));
// // 			}
// // 
// // 		}
// // 		CString staticText;
// // 		staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("BackGround"));
// // 		if (staticText.IsEmpty())
// // 		{
// // 			staticText = _T("背 景");
// // 			dc.TextOut(21,15,staticText);
// // 		}
// // 		else
// // 		{
// // 			dc.TextOut(21,0,staticText);
// // 		}
// // 		dc.SelectObject(oldf);
// // 		myfont.DeleteObject();
// // 	}
// // 	if (BtnId == IDC_WB_TEACHTOOL)
// // 	{
// // 		dc.SetBkMode(TRANSPARENT);
// // 		CFont myfont;
// // 		myfont.CreateFont(12,8,2700,0,10,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,_T("@system"));
// // 		CFont *oldf = dc.SelectObject(&myfont);
// // 		if (m_isSelect)
// // 		{
// // 			CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_press.png");
// // 			img.Load(CurPicPath);
// // 			if (!img.IsNull())
// // 			{
// // 				img.Draw(dc.m_hDC,CRect(0,0,30,60));
// // 			}
// // 
// // 		}
// // 		else
// // 		{
// // 			CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_normal.png");
// // 			img.Load(CurPicPath);
// // 			if (!img.IsNull())
// // 			{
// // 				img.Draw(dc.m_hDC,CRect(0,0,30,60));
// // 			}
// // 
// // 		}
// // 		CString staticText;
// // 		staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Tool"));
// // 		if (staticText.IsEmpty())
// // 		{
// // 			staticText = _T("工 具");
// // 			dc.TextOut(21,15,staticText);
// // 		}
// // 		else
// // 		{
// // 			dc.TextOut(21,20,staticText);
// // 		}
// // 
// // 		dc.SelectObject(oldf);
// // 		myfont.DeleteObject();
// // 	}
//  	dc.Detach(); 
// 
// 
// 	//CString CurPicPath = GetCurrentPath();	//获取当前程序执行的目录
// 	//CImage img;
// 	//CRect clicentRc;
// 	//GetClientRect(&clicentRc);
// 	//CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\NormalBtn.png");
// 	//img.Load( CurPicPath );
// 	//if (!img.IsNull())
// 	//{
// 	//	img.Draw(pDC->m_hDC,clicentRc);
// 	//}	
	return TRUE;
	return CBitmapButton::OnEraseBkgnd(pDC);
}

void CWBFunctionBtn::DrawItem(LPDRAWITEMSTRUCT lpDS/*lpDrawItemStruct*/)
{
	CDC dc;
	CImage img;
	int  BtnId;
	//CString BtnText;
	dc.Attach( lpDS->hDC ); 
	CString CurPicPath ;	//获取当前程序执行的目录	
	BtnId = GetDlgCtrlID();
	CFont *myfont = NULL;
	CFont *oldf = NULL;
	CString staticText = _T(""); 
	if (BtnId == IDC_WB_PREVIEWBTN || BtnId ==IDC_WB_ATTRIBUTEBTN ||BtnId== IDC_WB_RESBTN|| BtnId == IDC_WB_BACKGROUNDBTN || BtnId == IDC_WB_TEACHTOOL)
	{
 		myfont = new CFont;
 		if (myfont)
 		{
 			dc.SetBkMode(TRANSPARENT);
 			myfont->CreateFont(12,8,2700,0,10,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,_T("@system"));
 			oldf = dc.SelectObject(myfont);
 		}
	}
	switch(BtnId)
	{
	case IDC_WB_FONTBOLDBTN:
		{
			if ( m_isSelect )
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_FontBoldBtn_press.png");
			} 
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_FontBoldBtn_normal.png");
			}
		}
		break;
	case IDC_WB_FONTITALICBTN:
		{
			if ( m_isSelect )
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_FontItalicBtn_press.png");
			} 
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_FontItalicBtn_normal.png");
			}
		}
		break;
	case IDC_WB_FONTUNDERLINEBTN:
		{
			if ( m_isSelect )
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_FontUnderlineBtn_press.png");
			} 
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_FontUnderlineBtn_normal.png");
			}
		}
		break;
	case IDC_WB_VERTICALBTN:
		{
			if ( m_isSelect )
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_VerticalBtn_press.png");
			} 
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_VerticalBtn_normal.png");
			}
		}
		break;
	case IDC_ATRIBUTE_CHECK_SOLID:
		{
			if( m_isSelect ) 
			{ 
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_ChexBtn_press.png");
			}
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_ChexBtn_normal.png");
			}
		}
		break;
	//case IDC_SINWB_CLOSEFUNCPAD:
	//	{
	//		switch (m_mouseState)
	//		{
	//		case MOUSE_LBTNUP:
	//			CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_normal.png");
	//			break;
	//		case MOUSE_HOVER:
	//			CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_press.png");
	//			break;
	//		case MOUSE_LEAVE:
	//			CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_normal.png");
	//			break;
	//		case MOUSE_LBTNDOWN:
	//			CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_normal.png");
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//	break;
	case IDC_CLOSEWBFUNCTIONDLG:
			{
				switch (m_mouseState)
				{
				case MOUSE_LBTNDOWN:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_press.png");
					break;
				case MOUSE_LBTNUP:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_normal.png");
					break;
				case MOUSE_HOVER:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_press.png");
					break;
				case MOUSE_LEAVE:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_normal.png");
					break;
				default:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cCloseButton_normal.png");
					break;
				}				
			}
		    break;
	case IDC_BTN_PREVIEW:
		{
			switch (m_mouseState)
			{
			case MOUSE_HOVER:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_BtnLeftDown_press.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_BtnLeftDown_normal.png");
				break;
			case MOUSE_LBTNDOWN:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_BtnLeftDown_press.png");
				break;
			case MOUSE_LBTNUP:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_BtnLeftDown_normal.png");
				break;
			default:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_BtnLeftDown_normal.png");
				break;
			}
		}
		break;
	case IDB_BTN_EAGLECLOSE:
		{
			switch(m_mouseState)
			{
			case MOUSE_LBTNDOWN:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cEagleEyeCloseButton_press.png");
					break;
			case MOUSE_LBTNUP:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cEagleEyeCloseButton_normal.png");
					break;
			default:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cEagleEyeCloseButton_normal.png");
					break;
			}
		}
		break;
	case IDC_ATRIBUTE_ADDTRANSPARBTN:
		{
			switch (m_mouseState)
			{
				case MOUSE_LBTNDOWN:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cAddTransparBtn_press.png");
					break;
				case MOUSE_LBTNUP:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cAddTransparBtn_normal.png");
					break;
				case MOUSE_HOVER:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cAddTransparBtn_press.png");
					break;
				case MOUSE_LEAVE:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cAddTransparBtn_normal.png");
					break;
				default:
					CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cAddTransparBtn_normal.png");
					break;
			}			
		}
		break;
	case IDC_ATRIBUTE_REDUCETRANSPARBTN://透明度滑块的减
		{
			switch (m_mouseState)
			{
			case MOUSE_LBTNDOWN:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cReduceTransperBtn_press.png");
				break;
			case MOUSE_LBTNUP:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cReduceTransperBtn_normal.png");
				break;
			case MOUSE_HOVER:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cReduceTransperBtn_press.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cReduceTransperBtn_normal.png");
				break;
			default:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cReduceTransperBtn_normal.png");
				break;
			}	
		}
		break;
	case IDC_WB_MT_ROTATE:
		{
			if( m_isSelect ) 
			{ 
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cFuncDlgMTRotate_select.png");
			}
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cFuncDlgMTRotate_normal.png");
			}
		}
		break;
	case IDC_WB_MT_ZOOM:
		{
			if( m_isSelect ) 
			{ 
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cFuncDlgMTZoom_select.png");
			}
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cFuncDlgMTZoom_normal.png");
			}
		}
		break;
	case IDC_PROMPTFONTBOLDBTN:
		{
			if ( m_isSelect )
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\m_PromptBoldBtn_press.png");
			} 
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\m_PromptFontBoldBtn_normal.png");
			}
		}
		break;
	case  IDC_PROMPTFONTITALICBTN:
		{
			if ( m_isSelect )
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\m_PromptItalicBtn_press.png");
			} 
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\m_PromptItalicBtn_normal.png");
			}
		}
		break;
	case  IDC_PROMPTFONTUNDERLINEBTN:
		{
			if ( m_isSelect )
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\m_PromptUnderlineBtn_press.png");
			} 
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\m_PromptUnderlineBtn_normal.png");
			}
		}
		break;
	case  IDC_PROMPTFONTVERTICALBTN:   //字体格式对话框中的垂直按钮
		{
			if ( m_isSelect )
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\m_PromptVerticalBtn_press.png");
			} 
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\m_PromptVerticalBtn_normal.png");
			}
		}
		break;
	//case IDC_WB_HIDEFUNCDLG:
	//	{
	//		if( m_isSelect ) 
	//		{ 
	//			CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_HideFuncdlg_press.png");
	//		}
	//		else
	//		{
	//			CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_HideFuncdlg_normal.png");
	//		}
	//	}
	//	break;
	case IDC_WB_PREVIEWBTN:
		{
			if (m_isSelect)
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_press.png");
			}
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_normal.png");
			}
		}
		break;
	case IDC_WB_ATTRIBUTEBTN:
		{
			if (m_isSelect)
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_press.png");			 
			}
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_normal.png"); 			
			}
		}
		break;
	case IDC_WB_RESBTN:
		{
			if (m_isSelect)
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_press.png");			 
			}
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_normal.png"); 			
			}
		}
		break;
	case IDC_WB_BACKGROUNDBTN:
		{
			if (m_isSelect)
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_press.png");
			}
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_normal.png");
			}
		}
		break;
	case IDC_WB_TEACHTOOL:
		{
			if (m_isSelect)
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_press.png");
			}
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_normal.png");
			}
		}
		break;
	case IDC_RADIO_CUR_PAGE:
	case IDC_RADIO_ALL_PAGE:
	case IDC_RADIO_SELE_PAGE:
		{
			if (m_isSelect)
			{
				CurPicPath = GetCurrentPath() + _T("\\skin\\WhiteBoard\\image\\WBSavePictureDlgSel.png");
			}
			else
			{
				CurPicPath = GetCurrentPath() + _T("\\skin\\WhiteBoard\\image\\WBSavePictureDlgUSel.png");
			}
		}
		break;
	case IDC_DEFAULTBACKGROUND:
		{
			switch (m_mouseState)
			{
			case MOUSE_LBTNDOWN:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\CancelBGBtn3.png");
					
				break;
			case MOUSE_LBTNUP:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\CancelBGBtn4.png"); 
				break;
			case MOUSE_HOVER:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\CancelBGBtn2.png"); 
				break;
			case MOUSE_LEAVE:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\CancelBGBtn1.png"); 
				break;
			default:
				CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\CancelBGBtn1.png"); 
				break;
			}	
		}
		break;
	default:
		break;
	}
	
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw( dc.m_hDC, lpDS->rcItem );
	}

	switch(BtnId)
	{
	case IDC_DEFAULTBACKGROUND:
		{
			LOGFONT logfont;
			memset(&logfont, 0, sizeof(LOGFONT));
			logfont.lfHeight = 14;
			logfont.lfCharSet = 134;
			_tcscpy_s(logfont.lfFaceName, _T("宋体"));
			
			CFont* myfont1 = new CFont;
			if (myfont1)
			{
				dc.SetBkMode(TRANSPARENT);
				staticText = _T("删除背景");
				dc.SelectObject(oldf);
				myfont1->CreateFontIndirect(&logfont);
				oldf = dc.SelectObject(myfont1);
				dc.TextOut(28,8,staticText);
			}
			myfont1->DeleteObject();
			delete myfont1;
			myfont1 = NULL;
			dc.SelectObject(oldf);
		}
		break;
	case IDC_WB_PREVIEWBTN:
		{
			staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("PreView"));
			if (staticText.IsEmpty())
			{
				staticText = _T("预 览");
			}
			dc.TextOut(lpDS->rcItem.left+21,lpDS->rcItem.top+5,staticText);
		}
		break;
	case IDC_WB_ATTRIBUTEBTN:
		{
			staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Attribute"));
			if (staticText.IsEmpty())
			{
				staticText = _T("属 性");
			}
			dc.TextOut(lpDS->rcItem.left+21,lpDS->rcItem.top+5,staticText);
		}
		break;
	case IDC_WB_RESBTN:
		{
			staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Resouce"));
			if (staticText.IsEmpty())
			{
				staticText = _T("资 源");
				dc.TextOut(lpDS->rcItem.left+21,lpDS->rcItem.top+15,staticText);
			}
			else
			{
				dc.TextOut(lpDS->rcItem.left+21,lpDS->rcItem.top+5,staticText);
			}
		}
		break;
	case IDC_WB_BACKGROUNDBTN:
		{
			staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("BackGround"));
			if (staticText.IsEmpty())
			{
				staticText = _T("背 景");
				dc.TextOut(lpDS->rcItem.left+21,lpDS->rcItem.top+5,staticText);
			}
			else
			{
				dc.TextOut(lpDS->rcItem.left+21,lpDS->rcItem.top + 5,staticText);
			}
		}
		break;
	case IDC_WB_TEACHTOOL:
		{
			staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Tool"));
			if (staticText.IsEmpty())
			{
				staticText = _T("工 具");
				dc.TextOut(lpDS->rcItem.left+21,lpDS->rcItem.top+5,staticText);
			}
			else
			{
				dc.TextOut(lpDS->rcItem.left+21,lpDS->rcItem.top + 11,staticText);
			}
		}
		break;
	case IDC_RADIO_CUR_PAGE:
	case IDC_RADIO_ALL_PAGE:
	case IDC_RADIO_SELE_PAGE:
		{
			CString strSavePicBtn = _T("");
			GetWindowText(strSavePicBtn);
			dc.SetBkMode(TRANSPARENT);
			dc.DrawText(strSavePicBtn,&lpDS->rcItem,DT_CENTER);
		}
 		break;
	default:
		break;
	}

	if (myfont != NULL)
	{
		myfont->DeleteObject();
		delete myfont;
		myfont = NULL;
	}
	if (oldf != NULL)
	{
		dc.SelectObject(oldf);
		oldf = NULL;
	}

	dc.Detach(); 
	return;
	
// 	if (BtnId == IDC_WB_RESBTN)
// 	{
// 		dc.SetBkMode(TRANSPARENT);
// 		CFont myfont;
// 		myfont.CreateFont(12,8,2700,0,10,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,_T("@system"));
// 		CFont *oldf = dc.SelectObject(&myfont);
// 		if (m_isSelect)
// 		{
// 			CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_press.png");
// 			img.Load(CurPicPath);
// 			if (!img.IsNull())
// 			{
// 				img.Draw(dc.m_hDC,lpDS->rcItem);
// 			}
// 
// 		}
// 		else
// 		{
// 			CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cBackGround_normal.png");
// 			img.Load(CurPicPath);
// 			if (!img.IsNull())
// 			{
// 				img.Draw(dc.m_hDC,lpDS->rcItem);
// 			}
// 
// 		}
// 		CString staticText;
// 		staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Resouce"));
// 		if (staticText.IsEmpty())
// 		{
// 			staticText = _T("资 源");
// 			dc.TextOut(lpDS->rcItem.left+21,lpDS->rcItem.top+15,staticText);
// 		}
// 		else
// 		{
// 			dc.TextOut(lpDS->rcItem.left+21,lpDS->rcItem.top+5,staticText);
// 		}
// 		dc.SelectObject(oldf);
// 		myfont.DeleteObject();
// 		dc.Detach(); 
// 		return;
// 	}
	
}

void CWBFunctionBtn::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}

	//获取预览中菜单点击后菜单的区域
	ClientToScreen(&point);//按钮客户区坐标转换成屏幕坐标
	CRect rect = CRect(point.x, point.y, point.x + 100, point.y + 175);
	if (((CWhiteBoardDlg *)g_whiteboardDlg)->GetFuntDlg())
	{
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetFuntDlg()->SetPreviewMenuRect(rect);
	}
	else
	{
		return;
	}

	if ((!g_bIsSupportMTTouch) && g_bNoSupportTouchInWB)//如果没有授权按钮就不响应触摸屏
	{
		if((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH)
		{
			return;
		}
	}
	int  BtnId;
	BtnId = GetDlgCtrlID();
	switch (BtnId)
	{
	case IDC_BTN_PREVIEW:
		((CWBPreviewDlg *)GetParent())->SetBtnState(BtnId,MOUSE_LBTNDOWN);
		break;
	case IDB_BTN_EAGLECLOSE:
		((CWBPreviewSingleView *)GetParent())->SetBtnState(BtnId,MOUSE_LBTNDOWN);
		break;
	case IDC_WB_PREVIEWBTN:
		{
			((CWBOptionDlg *)GetParent())->OnBnClickedWbPreviewbtn();		
		}
		break;
	case IDC_WB_ATTRIBUTEBTN:
		{
			((CWBOptionDlg *)GetParent())->OnBnClickedWbAttributebtn();		
		}		
		break;
	case IDC_WB_RESBTN:
		{
			((CWBOptionDlg *)GetParent())->OnBnClickedWbResbtn();
		}		
		break;
	case IDC_WB_BACKGROUNDBTN:
		{
			((CWBOptionDlg *)GetParent())->OnBnClickedWbBackgroundbtn();
		}		
		break;
	case IDC_CLOSEWBFUNCTIONDLG:
		((CWBFunctionDlg *)GetParent())->SetBtnState(BtnId,MOUSE_LBTNDOWN);
		break;
	case IDC_ATRIBUTE_REDUCETRANSPARBTN:
		((CWBFunctionAttributeDlg *)GetParent())->SetBtnState(BtnId,MOUSE_LBTNDOWN);
		break;
	case IDC_ATRIBUTE_ADDTRANSPARBTN:
		((CWBFunctionAttributeDlg *)GetParent())->SetBtnState(BtnId,MOUSE_LBTNDOWN);
		break;
	//case IDC_WB_HIDEFUNCDLG:
	//		{
	//			if(!m_isSelect)
	//			{
	//				m_isSelect = TRUE;
	//				//g_isFunctionDlgHide = TRUE;
	//			}
	//			else
	//			{
	//				m_isSelect = FALSE;			
	//				//g_isFunctionDlgHide = FALSE;
	//			}
	//		}
	//		break;
	case IDC_DEFAULTBACKGROUND:
		((CWBFunctionAttributeDlg *)GetParent())->SetBtnState(BtnId,MOUSE_LBTNDOWN);
		break;
	default:
		break;
	}


	if (CWBToolTip::GetIns())
	{
		CWBToolTip::GetIns()->Restore();
	}
	CBitmapButton::OnLButtonDown(nFlags, point);
}

void CWBFunctionBtn::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	int  BtnId;
	m_UpPoint = point;
	BtnId = GetDlgCtrlID();
	switch (BtnId)
	{
	case IDC_CLOSEWBFUNCTIONDLG:
		((CWBFunctionDlg *)GetParent())->SetBtnState(BtnId,MOUSE_LBTNUP);
		break;
	case IDC_ATRIBUTE_REDUCETRANSPARBTN:
	case IDC_ATRIBUTE_ADDTRANSPARBTN:
		((CWBFunctionAttributeDlg *)GetParent())->SetBtnState(BtnId,MOUSE_LBTNUP);
		break;
	case IDC_BTN_PREVIEW:
		((CWBPreviewDlg *)GetParent())->SetBtnState(BtnId,MOUSE_LBTNUP);
		break;

	case IDB_BTN_EAGLECLOSE:
		((CWBPreviewSingleView *)GetParent())->SetBtnState(BtnId,MOUSE_LBTNUP);
		break;
	case IDC_DEFAULTBACKGROUND:
		((CWBFunctionAttributeDlg *)GetParent())->SetBtnState(BtnId,MOUSE_LBTNUP);
		break;
//	case IDC_PROMPTFONTVERTICALBTN:
//		{
//			//如果鼠标弹起时，点不在按钮的范围内，则不作响应。//luowei 2012-04-10
//			if (point.x <= 0 || point.x >= 48 || point.y <= 0 || point.y >= 28)
//			{
//				break;
//			}
//
//			if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg() && NULL != g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox())
//			{
//				CString str;
//				CComboBoxEx *pTempCom = g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox();
//				pTempCom->GetWindowText(str);
//				if (_tcscmp(str, _T("")) == 0)
//				{
//					pTempCom->SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
//					g_pWhiteBoardView->GetPromptFontSetDlg()->UpdateData(FALSE);
//				}
//				pTempCom = NULL;
//			}
//			if(!m_isSelect)
//			{
//				m_isSelect = TRUE;
//				g_bIsVertical = TRUE;
//			}
//			else
//			{
//				m_isSelect = FALSE;
//				g_bIsVertical = FALSE;
//			}
//
//			//按了竖排输出按钮后改变编辑框
//			if (g_pWhiteBoardView->GetCurEdit())
//			{
//				CWBRichEdit *pEdit = g_pWhiteBoardView->GetCurEdit();
//				if (pEdit == NULL)
//				{
//					break;
//				}
//				CString str;
//				pEdit->GetWindowText(str);
//
//				CRect rectText = g_pWhiteBoardView->GetEditRect();
//				
//				CPoint pt;
//				if (g_bIsVertical)//从横变竖
//				{
//					pt.x = rectText.right;
//					pt.y = rectText.top;
//				} 
//				else//从竖变横
//				{
//					pt = rectText.TopLeft();
//				}
//				g_pWhiteBoardView->SetEditTopLeft(pt);
//
//				if (g_pWhiteBoardView->GetIsEditing())//如果是编辑状态
//				{
//					PWBTextItem pTextItem = new WBTextItem;
//					CWBText *pText = NULL;
//					if ((g_pWhiteBoardView->GetEditObj())->GetType() == WB_OBJ_ANNOTATIONS)
//					{
//						CWBAnnotations *pAnnotations = (CWBAnnotations *)(g_pWhiteBoardView->GetEditObj());
//						pText = pAnnotations->GetText();
//					} 
//					else
//					{
//						pText = (CWBText *)(g_pWhiteBoardView->GetEditObj());
//					}
//
//					pText->GetItem(pTextItem);
//					pTextItem->ptTopLeft = pt;
//					pText->SetItem(pTextItem);
//					pText->SetIsVertical(g_bIsVertical);
//					pText->SetCalRectText();
//					delete pTextItem;
//				}
//
//				CWBRichEditMgr *ptempM = CWBRichEditMgr::GetInst_();
//				ptempM->SetRichEditShow(false);
//				ptempM->SetBeginPoint(rectText.left, rectText.top, MOUSE_ID);
//				ptempM->SetEndPoint(rectText.right, rectText.bottom, MOUSE_ID);
//				ptempM->SetRichEditShow(true);
//
//				CWnd *pParent = NULL;
//				if(NULL != CScreenDrawMgr::GetIns()) //如果是在桌面绘图，则将文本框加载CMouseDlg上，否则加在CWhiteBoardView上
//				{
//					///设置文本框的父窗口,如果是win7 vista以上则是mousedlg,否则为screendrawdlg
//					CScreenDrawMgr::GetIns()->SetRichEditParent(&pParent);
//				}
//				else
//				{
//					pParent = g_pWhiteBoardView;
//				}
//				ptempM->SetIsSaveText(false);
//				ptempM->ShowWBRichEdit(MOUSE_ID, g_bIsVertical, pParent, true);
//				ptempM->SetIsSaveText(true);
//
//				pEdit = g_pWhiteBoardView->GetCurEdit();
////				pEdit->MoveWindow(&rectText);
//				pEdit->SetWindowText(str);
//				pEdit->SetFocus();
//				(pEdit->GetRichEditCtrl()).LimitText(1024);
//				(pEdit->GetRichEditCtrl()).SetSel(-1, -1);//光标到末尾
//
//				//设置文字字体样式
//				int iFontWeight;
//				if (g_isBold)
//				{
//					iFontWeight = FW_BOLD;
//				}
//				else
//				{
//					iFontWeight = FW_NORMAL;
//				}
//				CFont font;
//				font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
//					, OUT_DEFAULT_PRECIS
//					, CLIP_DEFAULT_PRECIS
//					, DEFAULT_QUALITY,FF_DONTCARE,g_nWBCurObjFontFaceName);		
//				pEdit->SetFont(&font);
//
//				g_pWhiteBoardView->SetEditCharFormat(pEdit);
//
//				if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && NULL != g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg())
//				{
//					CWBFunctionAttributeDlg *pTemFunADlg = g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg();
//					pTemFunADlg->GetFontVerticalBtn()->SetIsSelect(g_bIsVertical);
//					pTemFunADlg->GetFontVerticalBtn()->Invalidate();
//					if (_tcscmp(pTemFunADlg->m_strFontSize, L"") == 0)
//					{
//						pTemFunADlg->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
//						pTemFunADlg->UpdateData(FALSE);
//					}
//					pTemFunADlg = NULL;
//				}
//				
//				font.DeleteObject();
//			}
//		}
//		break;
	default:
		break;
	}

	CBitmapButton::OnLButtonUp(nFlags, point);
}

void CWBFunctionBtn::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(!m_bMouseIn)
	{
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.dwHoverTime = 1;
		tme.hwndTrack = m_hWnd;
		m_bMouseIn = TrackMouseEvent(&tme);
	}

	//((CWhiteBoardDlg *) g_whiteboardDlg)->GetFuntDlg()->SetIsHideOrShow(true);
	CRect rc;
	if (((CWhiteBoardDlg*)g_whiteboardDlg) && ((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg())
	{
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()->GetWindowRect(rc);
	}
	else
	{
		return;
	}
	
	ClientToScreen(&point);
	if (PtInRect(&rc, point))
	{
		if (((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg())
		{
			if (((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg()->GetWindowIsHide())
			{
				((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()->SetTimer(ID_TIMER_SHOW, 10, NULL);	
				((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg()->SetWindowIsHide(false);
			}
			else
			{
				((CWhiteBoardDlg *)g_whiteboardDlg)->GetFuntDlg()->SetMouseLeaveTime(GetTickCount());
			}
		}
	}

	CButton::OnMouseMove(nFlags, point);
}

void CWBFunctionBtn::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_mouseState = MOUSE_HOVER;
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	Invalidate();
	ShowToolText();
	CButton::OnMouseHover(nFlags, point);
}

void CWBFunctionBtn::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bMouseIn = FALSE;
	m_mouseState = MOUSE_LEAVE;
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	Invalidate();

	if (CWBToolTip::GetIns())
	{
		CWBToolTip::GetIns()->Restore();
	}
	CButton::OnMouseLeave();
}

void CWBFunctionBtn::MoveWindowBtn( int x,int y )
{
	bool bFlag = false;
	int BtnId = GetDlgCtrlID();
	CFont *myfont = NULL;
	CFont *oldf = NULL;
	CString staticText = _T(""); 
	CDC *pdc = NULL;
	pdc= GetDC();
	myfont = new CFont;
	if (myfont)
	{
		pdc->SetBkMode(TRANSPARENT);
		myfont->CreateFont(12,8,2700,0,10,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,_T("@system"));
		oldf = pdc->SelectObject(myfont);
	}
	switch(BtnId)
	{
	case IDC_WB_PREVIEWBTN:
		{
			staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("PreView"));
			if (staticText.IsEmpty())
			{
				staticText = _T("预 览");					
			}
		}
		break;
	case IDC_WB_ATTRIBUTEBTN:
		{
			staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Attribute"));
			if (staticText.IsEmpty())
			{
				staticText = _T("属 性");
			}
		}
		break;
	case IDC_WB_RESBTN:
		{
			staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Resouce"));
			if (staticText.IsEmpty())
			{
				staticText = _T("资 源");
			}
		}
		break;
	case IDC_WB_BACKGROUNDBTN:
		{
			staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("BackGround"));
			if (staticText.IsEmpty())
			{
				staticText = _T("背 景");
			}
		}
		break;
	case IDC_WB_TEACHTOOL:
		{
			staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Tool"));
			if (staticText.IsEmpty())
			{
				staticText = _T("工 具");
			}
		}
		break;
	default:
		break;
	}
	m_size = pdc->GetTextExtent(staticText);
	if (myfont != NULL)
	{
		myfont->DeleteObject();
		delete myfont;
		myfont = NULL;
	}
	if (oldf != NULL)
	{
		pdc->SelectObject(oldf);
		oldf = NULL;
	}
	m_size.cx += 10;
	if (m_size.cx < 50)
	{
		m_size.cx = 50;
	}
	MoveWindow(x,y,30,m_size.cx);
	ReleaseDC(pdc);
}

CSize CWBFunctionBtn::GetBtnSizeHeight()
{
	return m_size;
}



void CWBFunctionBtn::ShowToolText()
{
	if (g_bAvconLock)
	{
		return;
	}
	int BtnId = GetDlgCtrlID();
	if (BtnId ==IDC_WB_PREVIEWBTN || BtnId == IDC_WB_ATTRIBUTEBTN
		||BtnId ==IDC_WB_BACKGROUNDBTN || BtnId == IDC_WB_TEACHTOOL||BtnId == IDC_WB_RESBTN)
	{
		CString staticText = _T(""); 
		CFont *myfont = NULL;
		CFont *oldf = NULL;
		CDC *pdc = NULL;
		pdc= GetDC();
		CSize msize;
		myfont = new CFont;
		if (myfont)
		{
			pdc->SetBkMode(TRANSPARENT);
			myfont->CreateFont(12,8,2700,0,10,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,_T("@system"));
			oldf = pdc->SelectObject(myfont);
		}
		switch(BtnId)
		{
		case IDC_WB_PREVIEWBTN:
			{
				staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("PreView"));
				if (staticText.IsEmpty())
				{
					staticText = _T("预 览");					
				}
			}
			break;
		case IDC_WB_ATTRIBUTEBTN:
			{
				staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Attribute"));
				if (staticText.IsEmpty())
				{
					staticText = _T("属 性");
				}
			}
			break;
		case IDC_WB_RESBTN:
			{
				staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Resouce"));
				if (staticText.IsEmpty())
				{
					staticText = _T("资 源");
				}
			}
			break;
		case IDC_WB_BACKGROUNDBTN:
			{
				staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("BackGround"));
				if (staticText.IsEmpty())
				{
					staticText = _T("背 景");
				}
			}
			break;
		case IDC_WB_TEACHTOOL:
			{
				staticText = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("Tool"));
				if (staticText.IsEmpty())
				{
					staticText = _T("工 具");
				}
			}
			break;
		default:
			break;
		}
		msize = pdc->GetTextExtent(staticText);
		if (myfont != NULL)
		{
			myfont->DeleteObject();
			delete myfont;
			myfont = NULL;
		}
		if (oldf != NULL)
		{
			pdc->SelectObject(oldf);
			oldf = NULL;
		}

		CWBToolTip::GetIns()->SetText(staticText);
		RECT rcDlg = { 0 };
		::GetWindowRect(this->GetSafeHwnd(), &rcDlg);
		rcDlg.right -= (msize.cx + 20);
		CWBToolTip::GetIns()->SetPosition(CPoint(rcDlg.right, rcDlg.bottom));
		CWBToolTip::GetIns()->Display();
		ReleaseDC(pdc);
	}
}

CPoint CWBFunctionBtn::GetMouseUpPoint()
{
	return  m_UpPoint; 
}
