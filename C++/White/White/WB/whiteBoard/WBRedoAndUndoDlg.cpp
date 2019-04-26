// WBRedoAndUndoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBRedoAndUndoDlg.h"
#include "afxdialogex.h"



// CWBRedoAndUndoDlg 对话框

IMPLEMENT_DYNAMIC(CWBRedoAndUndoDlg, CDialogEx)

CWBRedoAndUndoDlg::CWBRedoAndUndoDlg(CWBToolbarDlg *pToolbar,CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBRedoAndUndoDlg::IDD, pParent)
{
	m_pToolbar = pToolbar;
}

CWBRedoAndUndoDlg::~CWBRedoAndUndoDlg()
{
}

void CWBRedoAndUndoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UNDOBTN, m_undoBtn);
	DDX_Control(pDX, IDC_REDOBTN, m_redoBtn);
	DDX_Control(pDX,IDC_CANTOUCHBTN,m_imagectrl);
}


BEGIN_MESSAGE_MAP(CWBRedoAndUndoDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_UNDOBTN, &CWBRedoAndUndoDlg::OnBnClickedUndobtn)
	ON_BN_CLICKED(IDC_REDOBTN, &CWBRedoAndUndoDlg::OnBnClickedRedobtn)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CWBRedoAndUndoDlg 消息处理程序


void CWBRedoAndUndoDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}


void CWBRedoAndUndoDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnCancel();
}


BOOL CWBRedoAndUndoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect clientRc;
	GetClientRect(&clientRc);
	CRect rcbtn;
	CRgn rgnbtn;
		 
	m_undoBtn.SetWindowPos(&wndTop,0,0,WB_TOOLBOXBTN_SPEC_WIDTH,WB_TOOLBOXBTN_SPEC_HEIGHT,SWP_SHOWWINDOW);
	
	m_redoBtn.SetWindowPos(&wndTop,WB_TOOLBOXBTN_SPEC_WIDTH + 8 ,0 ,WB_TOOLBOXBTN_SPEC_WIDTH,WB_TOOLBOXBTN_SPEC_HEIGHT,SWP_SHOWWINDOW);
	
	m_undoBtn.SetType(TYPE_UNDO);
	m_undoBtn.SetWindowText(GetCaptureFromType(TYPE_UNDO));
	m_redoBtn.SetType(TYPE_REDO);
	m_redoBtn.SetWindowText(GetCaptureFromType(TYPE_REDO));
	//触摸授权状态图标按钮
	m_imagectrl.SetWindowPos(&wndTop,WB_TOOLBOXBTN_SPEC_WIDTH + WB_TOOLBOXBTN_SPEC_WIDTH + 16,0,WB_TOOLBOXBTN_SPEC_WIDTH,WB_TOOLBOXBTN_SPEC_HEIGHT,SWP_SHOWWINDOW);
	
	//判断是否授权以显示图标	
	m_imagectrl.SetType(TYPE_CANTOUCH);
	m_imagectrl.SetWindowText(GetCaptureFromType(TYPE_CANTOUCH));

	/*m_imagectrl.GetWindowRect(&rcbtn);*/
// 	rgnbtn.CreateRoundRectRgn(WB_TOOLBOXBTN_SPEC_WIDTH + WB_TOOLBOXBTN_SPEC_WIDTH + 16,0,WB_TOOLBOXBTN_SPEC_WIDTH,WB_TOOLBOXBTN_SPEC_HEIGHT,5,5);
// 	::SetWindowRgn(m_imagectrl.m_hWnd,rgnbtn,TRUE);
// 	rgnbtn.DeleteObject();
// 
// 	CRgn redorgn;
// 	/*m_redoBtn.GetWindowRect(&rcbtn);*/
// 	redorgn.CreateRoundRectRgn(WB_TOOLBOXBTN_SPEC_WIDTH + 8 ,0 ,WB_TOOLBOXBTN_SPEC_WIDTH,WB_TOOLBOXBTN_SPEC_HEIGHT,5,5);
// 	::SetWindowRgn(m_redoBtn.m_hWnd,redorgn,TRUE);
// 	redorgn.DeleteObject();
// 
// 	CRgn undorgn;
// 	//m_undoBtn.GetWindowRect(&rcbtn);
// 	undorgn.CreateRoundRectRgn(0,0,WB_TOOLBOXBTN_SPEC_WIDTH,WB_TOOLBOXBTN_SPEC_HEIGHT,5,5);
// 	::SetWindowRgn(m_undoBtn.m_hWnd,undorgn,TRUE);
// 	undorgn.DeleteObject();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
CString CWBRedoAndUndoDlg::GetCaptureFromType(eType btnEmumType)
{
	CString BtnText;
	switch(btnEmumType)
	{
	case TYPE_UNDO:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_UNDO"));
			if (BtnText.IsEmpty() )
			{
				return _T("撤销");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_REDO:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_REDO"));
			if (BtnText.IsEmpty() )
			{
				return _T("重做");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	case TYPE_CANTOUCH:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_CANTOUCH"));
			if (BtnText.IsEmpty() )
			{
				return _T("触摸");
			} 
			else
			{
				return BtnText;
			}
		}
		break;
	/*case TYPE_NOTOUCH:
		{
			BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_NOTOUCH"));
			if (BtnText.IsEmpty() )
			{
				return _T("非触摸");
			} 
			else
			{
				return BtnText;
			}
		}
		break;*/
	default:
		{
			return _T("");
		}
		break;
	}
}

BOOL CWBRedoAndUndoDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CDialogEx::OnEraseBkgnd(pDC);
}


void CWBRedoAndUndoDlg::OnBnClickedUndobtn()
{
	m_pToolbar->ProcessCmd(TYPE_UNDO);

}


void CWBRedoAndUndoDlg::OnBnClickedRedobtn()
{
	m_pToolbar->ProcessCmd(TYPE_REDO);

}


void CWBRedoAndUndoDlg::OnPaint()
{
	CRect rtClient;
	GetClientRect(&rtClient);



	CDialogEx::OnPaint();

	CRgn rtClientRgn;
	rtClientRgn.CreateRectRgnIndirect(&rtClient);

	CRgn undorgn;
	undorgn.CreateRoundRectRgn(0,0,WB_TOOLBOXBTN_SPEC_WIDTH+1,WB_TOOLBOXBTN_SPEC_HEIGHT+1,5,5);

	CRgn redorgn;
	redorgn.CreateRoundRectRgn(WB_TOOLBOXBTN_SPEC_WIDTH + 8 ,0 ,WB_TOOLBOXBTN_SPEC_WIDTH + 9+WB_TOOLBOXBTN_SPEC_WIDTH,WB_TOOLBOXBTN_SPEC_HEIGHT+1,5,5);

	CRgn touchrgn;
	touchrgn.CreateRoundRectRgn(WB_TOOLBOXBTN_SPEC_WIDTH + WB_TOOLBOXBTN_SPEC_WIDTH + 16,0,WB_TOOLBOXBTN_SPEC_WIDTH + WB_TOOLBOXBTN_SPEC_WIDTH + 17+WB_TOOLBOXBTN_SPEC_WIDTH,WB_TOOLBOXBTN_SPEC_HEIGHT+1,5,5);

	//rtClientRgn.CombineRgn(&undorgn,&redorgn,RGN_XOR);
	CRgn rgnnew;
	rgnnew.CreateRectRgnIndirect(&rtClient);
	rgnnew.CombineRgn(&rgnnew,&undorgn,RGN_XOR);
	rgnnew.CombineRgn(&rgnnew,&redorgn,RGN_XOR);
	rgnnew.CombineRgn(&rgnnew,&touchrgn,RGN_XOR);
	rtClientRgn.CombineRgn(&rtClientRgn,&rgnnew,RGN_XOR);

	SetWindowRgn((HRGN)rtClientRgn,TRUE);

	rgnnew.DeleteObject();
	rtClientRgn.DeleteObject();
	redorgn.DeleteObject();
	undorgn.DeleteObject();
	touchrgn.DeleteObject();

// 	RECT re;
// 	re.left=WB_TOOLBOXBTN_SPEC_WIDTH ;
// 	re.top=0;
// 	re.right=WB_TOOLBOXBTN_SPEC_WIDTH + 8;
// 	re.bottom=WB_TOOLBOXBTN_SPEC_HEIGHT;
// 	//CRect rtClient;
// 	//GetClientRect(&rtClient);
// 	CRgn rtClientRgn;
// 	rtClientRgn.CreateRectRgnIndirect(&rtClient);
// 	CRgn rectRgn;
// 	rectRgn.CreateRoundRectRgn(re.left,re.top,re.right-re.left,re.bottom - re.top,6,6);
// 	//rectRgn.CreateRectRgnIndirect(&re);
// 	rtClientRgn.CombineRgn(&rtClientRgn, &rectRgn, RGN_XOR);
// 
// 	RECT ri;
// 	ri.left= WB_TOOLBOXBTN_SPEC_WIDTH + 8 + WB_TOOLBOXBTN_SPEC_WIDTH;
// 	ri.top = 0;
// 	ri.right = WB_TOOLBOXBTN_SPEC_WIDTH + 8 + WB_TOOLBOXBTN_SPEC_WIDTH + 8;
// 	ri.bottom = WB_TOOLBOXBTN_SPEC_HEIGHT;
// 	CRgn rictRgn;
// 	//rictRgn.CreateRoundRectRgn(ri.left,ri.top,ri.right-ri.left,ri.bottom-ri.top,6,6);
// 	rictRgn.CreateRectRgnIndirect(&ri);
// 	rtClientRgn.CombineRgn(&rtClientRgn,&rictRgn,RGN_XOR);
// 	SetWindowRgn((HRGN)rtClientRgn, TRUE);
// 
// 	rtClientRgn.DeleteObject();
// 	rectRgn.DeleteObject();
// 	rictRgn.DeleteObject();
}


BOOL CWBRedoAndUndoDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}
