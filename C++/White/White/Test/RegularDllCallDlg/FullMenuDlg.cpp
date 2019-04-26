// FullMenuDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RegularDllCallDlg.h"

#include "FullMenuDlg.h"
#include "afxdialogex.h"


// CFullMenuDlg 对话框

IMPLEMENT_DYNAMIC(CFullMenuDlg, CDialog)

CFullMenuDlg::CFullMenuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFullMenuDlg::IDD, pParent)
{

}

CFullMenuDlg::~CFullMenuDlg()
{
}

void CFullMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFullMenuDlg, CDialog)

	ON_WM_NCHITTEST()

	ON_COMMAND(IDC_WB_NEWFILEB, &CFullMenuDlg::OnWbNewfileb)
	ON_COMMAND(IDC_WB_OPENFILEB, &CFullMenuDlg::OnWbOpenfileb)
	ON_COMMAND(IDC_WB_SAVEFILEB, &CFullMenuDlg::OnWbSavefileb)
	ON_COMMAND(IDC_WB_RESAVEFILEB, &CFullMenuDlg::OnWbResavefileb)
	ON_COMMAND(IDC_WB_PRINTFILEB, &CFullMenuDlg::OnWbPrintfileb)
	ON_COMMAND(IDC_WB_CLOSEFILEB, &CFullMenuDlg::OnWbClosefileb)

	ON_COMMAND(IDC_WB_NEWPAGEB, &CFullMenuDlg::OnWbNewpageb)
	ON_COMMAND(IDC_WB_FOREPAGEB, &CFullMenuDlg::OnWbForepageb)
	ON_COMMAND(IDC_WB_NEXTPAGEB, &CFullMenuDlg::OnWbNextpageb)
	ON_COMMAND(IDC_WB_FIRSTPAGEB, &CFullMenuDlg::OnWbFirstpageb)
	ON_COMMAND(IDC_WB_LASTPAGEB, &CFullMenuDlg::OnWbLastpageb)


	ON_COMMAND(IDC_WB_FORELAYERB, &CFullMenuDlg::OnWbForelayerb)
	ON_COMMAND(IDC_WB_NEXTLAYERB, &CFullMenuDlg::OnWbNextlayerb)
	ON_COMMAND(IDC_WB_TOPLAYERB, &CFullMenuDlg::OnWbToplayerb)
	ON_COMMAND(IDC_WB_BOTTOMLAYERB, &CFullMenuDlg::OnWbBottomlayerb)
	ON_COMMAND(IDC_WB_LINEB, &CFullMenuDlg::OnWbLineb)

	ON_COMMAND(IDC_WB_SELECTB, &CFullMenuDlg::OnWbSelectb)
	ON_COMMAND(IDC_WB_CURVEB, &CFullMenuDlg::OnWbCurveb)
	ON_COMMAND(IDC_WB_LITTERRUBBERB, &CFullMenuDlg::OnWbLitterrubberb)
	ON_COMMAND(IDC_WB_BIGRUBBERB, &CFullMenuDlg::OnWbBigrubberb)
	ON_COMMAND(IDC_WB_SINGLEOBJECTRUBBERB, &CFullMenuDlg::OnWbSingleobjectrubberb)
	ON_COMMAND(IDC_WB_CURVESELECTRUBBERB, &CFullMenuDlg::OnWbCurveselectrubberb)

	ON_COMMAND(IDC_WB_RECTANGLEB, &CFullMenuDlg::OnWbRectangleb)
	ON_COMMAND(IDC_WB_ROUNDB, &CFullMenuDlg::OnWbRoundb)
	ON_COMMAND(IDC_WB_TRIANGLEB, &CFullMenuDlg::OnWbTriangleb)
	ON_COMMAND(IDC_WB_RHOMBUSB, &CFullMenuDlg::OnWbRhombusb)
	ON_COMMAND(IDC_WB_POLYGONB, &CFullMenuDlg::OnWbPolygonb)
	ON_COMMAND(IDC_WB_CHARFONTB, &CFullMenuDlg::OnWbCharfontb)
	ON_COMMAND(IDC_WB_CHARTEXTB, &CFullMenuDlg::OnWbChartextb)

	ON_COMMAND(IDC_WB_PICTUREB, &CFullMenuDlg::OnWbPictureb)
	//ON_COMMAND(IDC_LINEWIDTHB1, &CFullMenuDlg::OnLinewidthb1)
	ON_COMMAND(IDC_WB_DOCUMENTB, &CFullMenuDlg::OnWbDocumentb)
	ON_COMMAND(IDC_WB_CUTSCREENB, &CFullMenuDlg::OnWbCutscreenb)
	ON_COMMAND(IDC_WB_DELETEB, &CFullMenuDlg::OnWbDeleteb)
	ON_COMMAND(IDC_WB_UCCB, &CFullMenuDlg::OnWbUccb)
	ON_COMMAND(IDC_WB_RESOURCECATAGORYB, &CFullMenuDlg::OnWbResourcecatagoryb)
	ON_COMMAND(IDC_RESOURCE_MANGER, &CFullMenuDlg::OnWbResourceManger)

	ON_COMMAND(IDC_WB_FLASHB, &CFullMenuDlg::OnWbFlashb)

	ON_COMMAND(IDC_WB_NORMALB, &CFullMenuDlg::OnWbNormalb)
	ON_COMMAND(IDC_WB_UNDOB, &CFullMenuDlg::OnWbUndob)
	ON_COMMAND(IDC_WB_REDOB, &CFullMenuDlg::OnWbRedob)
	ON_COMMAND(IDC_WB_DUSTBIN, &CFullMenuDlg::OnWbDustbinb)
END_MESSAGE_MAP()


// CFullMenuDlg 消息处理程序


BOOL CFullMenuDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//画笔属性
// void CFullMenuDlg::OnPen()
// {  
// 	WB_OpenAttributeDlg();
// 
// }
//新建
void CFullMenuDlg::OnWbNewfileb()
{	
	WB_Newfile();
}

//打开文件
void CFullMenuDlg::OnWbOpenfileb()
{
	WB_Openfile();
}

//保存
void CFullMenuDlg::OnWbSavefileb()
{
	WB_SaveWBFile();
}

//另存
void CFullMenuDlg::OnWbResavefileb()
{
	WB_Resavefile();
}

//打印
void CFullMenuDlg::OnWbPrintfileb()
{
	WB_Printfile();
}

//关闭
void CFullMenuDlg::OnWbClosefileb()
{
	WB_Closefile();
}

//新建页
void CFullMenuDlg::OnWbNewpageb()
{  
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_AddNewPage();

}

//插入页
void CFullMenuDlg::OnInsertpageb()
{  
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_InsertNewPage();

}

//删除页
void CFullMenuDlg::OnDeletepageb()
{  
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_DeletePage();

}

//上一页
void CFullMenuDlg::OnWbForepageb()
{
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_PrePage();
}

//下一页
void CFullMenuDlg::OnWbNextpageb()
{
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_NextPage();
}

//第一页
void CFullMenuDlg::OnWbFirstpageb()
{
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_FirstPage();
}

//最后页
void CFullMenuDlg::OnWbLastpageb()
{
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_LastPage();
}

//上一层
void CFullMenuDlg::OnWbForelayerb()
{
	WB_SuperLayer();
}

//下一层
void CFullMenuDlg::OnWbNextlayerb()
{
	WB_UnderLayer();
}

//顶层
void CFullMenuDlg::OnWbToplayerb()
{
	WB_TopLayer();
}

//底层
void CFullMenuDlg::OnWbBottomlayerb()
{
	WB_BottomLayer();
}

//直线
void CFullMenuDlg::OnWbLineb()
{
	WB_SetDrawStatus( DRAW_LINE);
}

//选择
void CFullMenuDlg::OnWbSelectb()
{
	WB_SetDrawStatus( DRAW_NORMAL);
}

//曲线
void CFullMenuDlg::OnWbCurveb()
{
	WB_SetDrawStatus( DRAW_POLYLINE);
}

//小橡皮擦
void CFullMenuDlg::OnWbLitterrubberb()
{
	WB_SetDrawStatus( DRAW_ERASE);
	WB_SetBrushSize(BRUSH_SMALL);

}

//大橡皮擦
void CFullMenuDlg::OnWbBigrubberb()
{
	WB_SetDrawStatus( DRAW_ERASE);
	WB_SetBrushSize(BRUNSH_BIG);
}

//清除
void CFullMenuDlg::OnWbSingleobjectrubberb()
{
	WB_SetDrawStatus( DRAW_CLEANUP);
}

//区域删除
void CFullMenuDlg::OnWbCurveselectrubberb()
{
	WB_SetDrawStatus( DRAW_RGNDELETE);
}

//矩形
void CFullMenuDlg::OnWbRectangleb()
{WB_SetDrawStatus( DRAW_RECT);
}

//圆
void CFullMenuDlg::OnWbRoundb()
{
	WB_SetDrawStatus( DRAW_ELLIPSE);
}

//三角形
void CFullMenuDlg::OnWbTriangleb()
{
	WB_SetDrawStatus( DRAW_TRIANGLE);
}

//菱形
void CFullMenuDlg::OnWbRhombusb()
{
	WB_SetDrawStatus( DRAW_RHOMBUS);
}

//多边形
void CFullMenuDlg::OnWbPolygonb()
{
	WB_SetDrawStatus( DRAW_POLYGON);
}

//选择字体
void CFullMenuDlg::OnWbCharfontb()
{
	WB_SetCharfont();
}

//插入文本
void CFullMenuDlg::OnWbChartextb()
{
	WB_SetDrawStatus( DRAW_TEXT);
}

//插入图片
void CFullMenuDlg::OnWbPictureb()
{
	WB_AddPicture();
}

void CFullMenuDlg::OnLinewidthb1()
{
	//MessageBox(_T("你点击了选择擦除按钮"));
}


//插入文档
void CFullMenuDlg::OnWbDocumentb()
{
	WB_AddDoc();
}

//截屏
void CFullMenuDlg::OnWbCutscreenb()
{
	WB_Cutscreen();
}



//删除
void CFullMenuDlg::OnWbDeleteb()
{
	WB_DeleteSelectedObjects();
}
//
void CFullMenuDlg::OnWbUccb()
{
	//	MessageBox(_T("UCC"));


}

//加载资源库
void CFullMenuDlg::OnWbResourcecatagoryb()
{
	WB_OpenResource();
}

//加载资源库管理 
void CFullMenuDlg::OnWbResourceManger()
{
	 WB_OpenReManger();
}


//加载flash
void CFullMenuDlg::OnWbFlashb()
{	
	WB_AddFlash();
}


//还原白板，去除全屏
void CFullMenuDlg::OnWbNormalb()
{
	WB_SetNormalScreen( );
	ShowWindow(SW_HIDE);
}

void CFullMenuDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}


void CFullMenuDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	//CDialog::OnCancel();
}

//拖动对话框
LRESULT CFullMenuDlg::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);

	//return CDialog::OnNcHitTest(point);
}
//Undo的时候产生一个弹出菜单
void CFullMenuDlg::OnWbUndob()
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


void CFullMenuDlg::OnWbRedob()
{
	WB_Redo( );
}


BOOL CFullMenuDlg::OnCommand(WPARAM wParam, LPARAM lParam)
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
	return CDialog::OnCommand(wParam, lParam);
}


void CFullMenuDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::PostNcDestroy();
	delete this;
}


void CFullMenuDlg::OnWbDustbinb()
{
	WB_AddPicture();
}
