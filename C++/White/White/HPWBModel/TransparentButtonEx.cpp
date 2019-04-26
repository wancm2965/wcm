// TransparentButtonEx.cpp : 实现文件
//

#include "stdafx.h"
#include "TransparentButtonEx.h"
#include "DlgFuck.h"
#include "DlgToolBar.h"


// CTransparentButtonEx
eType CTransparentButtonEx::g_buttonType = TYPE_NONE;


IMPLEMENT_DYNAMIC(CTransparentButtonEx, CButton)

CTransparentButtonEx::CTransparentButtonEx()
{
	m_bMouseIn = FALSE;
    m_pImg = NULL;
	Init();	
	m_pParentWnd = NULL;
}

CTransparentButtonEx::~CTransparentButtonEx()
{
	if (NULL != m_pImg )
	{
		delete m_pImg;
	}
}


BEGIN_MESSAGE_MAP(CTransparentButtonEx, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()



// CTransparentButtonEx 消息处理程序



void CTransparentButtonEx::Init() //加载图片路径
{
	m_mapPath.clear();
	string strPath = CW2A(GetAppPath());
	strPath += "\\SKIN\\HPWBModel\\72-72按钮\\";
	m_mapPath[TYPE_SELECT] = strPath + "选择.png";
	m_mapPath[TYPE_PAGE] = strPath + "页.png";
	m_mapPath[TYPE_FULLSCREEN] = strPath + "全屏.png";
	m_mapPath[TYPE_ARROWS] = strPath + "直线和箭头.png";
	m_mapPath[TYPE_BIGHT] = strPath + "曲线.png";
	m_mapPath[TYPE_LINE] = strPath + "直线.png";
	m_mapPath[TYPE_RUBBER] = strPath  + "橡皮擦.png";
	m_mapPath[TYPE_IMAGE] = strPath + "插入图片.png";
	m_mapPath[TYPE_VIDEO] = strPath + "视频.png";
	m_mapPath[TYPE_DOCUMENT] = strPath + "文档.png";
	m_mapPath[TYPE_ALLTOOL] = strPath + "工具箱.png";
	m_mapPath[TYPE_RESOURCE] = strPath + "资源目录.png";
	m_mapPath[TYPE_DEL] = strPath + "删除.png";
	m_mapPath[TYPE_PRE_PAGE] = strPath + "上一页面.png";
	m_mapPath[TYPE_NEXT_PAGE] = strPath  + "下一页面.png";
	m_mapPath[TYPE_T] = strPath + "文字.png";
	m_mapPath[TYPE_REDO] = strPath + "redo.png";
	m_mapPath[TYPE_UNDO] = strPath + "undo.png";
	m_mapPath[TYPE_EXITFULLSCREEN] = strPath + "退出全屏.png";
	m_mapPath[TYPE_RUBBER_BIG] = strPath + "大橡皮擦.png";
	m_mapPath[TYPE_RUBBER_SMALL] = strPath + "小橡皮擦.png";
	m_mapPath[TYPE_RUBBER_CURVE] = strPath + "曲线擦.png";
	m_mapPath[TYPE_RUBBER_OBJ] = strPath + "对象擦.png";
	m_mapPath[TYPE_TRIANGLE] = strPath + "三角形.png";
	m_mapPath[TYPE_RECTANGLE] = strPath + "矩形.png";
	m_mapPath[TYPE_ROUNDNESS] = strPath + "圆形.png";
	m_mapPath[TYPE_RHOMB] = strPath + "菱形.png";
	m_mapPath[TYPE_POLYGON] = strPath + "多边形.png";
	m_mapPath[TYPE_FONT] = strPath + "字体.png";
	m_mapPath[TYPE_ATTRIBUTE] = strPath + "属性.png";
	m_mapPath[TYPE_SCREENSHOTS] = strPath + "截屏.png";
	m_mapPath[TYPE_FLASH] = strPath + "flash.png";
	m_mapPath[TYPE_SAVE] = strPath + "保存.png";
	m_mapPath[TYPE_CLOSE] = strPath + "退出.png";
	m_mapPath[TYPE_PRINT] = strPath + "打印机.png";
	m_mapPath[TYPE_OPEN] = strPath + "打开.png";
	m_mapPath[TYPE_RESAVE] = strPath + "另存.png";
	m_mapPath[TYPE_DLG_RESOURCE] = strPath + "资源工具条.png";
	m_mapPath[TYPE_DLG_RUBBER] = strPath + "擦子工具条.png";
	m_mapPath[TYPE_DLG_GRAPHICS] = strPath + "图形工具条.png";
	m_mapPath[TYPE_DLG_FILE] = strPath + "文件工具条.png";
    m_mapPath[TYPE_DLG_PAGE] = strPath + "页工具条.png";
	m_mapPath[TYPE_CLR_REN] = strPath + "红色.png";
	m_mapPath[TYPE_CLR_BLACK] = strPath + "黑色.png";
	m_mapPath[TYPE_CLR_YELLOW] = strPath + "黄色.png";
	m_mapPath[TYPE_CLR_ORANGE] = strPath + "橙色.png";
	m_mapPath[TYPE_CLR_BLUE] = strPath + "蓝色.png";
	m_mapPath[TYPE_CLR_PURPLE] = strPath + "紫色.png";
	m_mapPath[TYPE_MAQN] = strPath + "放大镜.png";
	m_mapPath[TYPE_SPOTLIGHT] = strPath + "聚光灯.png";
	m_mapPath[TYPE_DESKTOPDRAW] = strPath + "桌面绘图.png";
    m_mapPath[TYPE_DDOPEN] = strPath + "屏绘打开.png";
	m_mapPath[TYPE_DDFULLSCREEN] = strPath + "屏绘全屏.png";
	m_mapPath[TYPE_DDEFFECT] = strPath + "屏绘切换.png";
	m_mapPath[TYPE_DDCTRL] = strPath + "屏绘控制条.png";
	m_mapPath[TYPE_DDCLOSE] = strPath + "屏绘关闭.png";
	m_mapPath[TYPE_FUNCTIONDLG] = strPath + "功能面板.png";

}
void CTransparentButtonEx::SetButtonImage(CString filename)
{
	if(m_pImg)
	{
		delete m_pImg;
		m_pImg = NULL;
	}

	m_pImg = new Bitmap(filename);
	ASSERT(m_pImg);
}
void CTransparentButtonEx::SetType(eType enumType)
{
	m_btnType = enumType;
	string strPath = m_mapPath[enumType];
	m_strPath = CA2W(strPath.c_str());
	SetButtonImage(m_strPath);
    ShowWindow(SW_SHOW);

}
eType CTransparentButtonEx::GetType()
{
	return m_btnType;
}
void CTransparentButtonEx::OnLButtonDown(UINT nFlags, CPoint point)
{

	CButton::OnLButtonDown(nFlags, point);
}

void CTransparentButtonEx::OnLButtonUp(UINT nFlags, CPoint point)
{
    CDlgToolBar  *pToolbar;
	CDlgFuck     *pToolbox;
	CWnd* upWnd;
	CString downBtnText(_T("这是DOWN按钮的标题")),upBtnText(_T("这是UP按钮的标题"));
    CTransparentButtonEx *upBtn;  //拖拽移动按钮时UP的按钮指针
    m_upPoint = point;
    ClientToScreen(&m_upPoint);  //将UP的点坐标转换为屏幕坐标
	upWnd = WindowFromPoint(m_upPoint);
    upBtn =(CTransparentButtonEx *)WindowFromPoint(m_upPoint);
	GetWindowText(downBtnText);
	upBtn->GetWindowText(upBtnText);
	//如果是DOWN和UP是同一个按钮、工具箱、是关闭按钮什么都不做
	if((this->m_hWnd == upBtn->m_hWnd) 
		||(m_btnID == IDC_BTN10) ||(upBtn->m_btnID == IDC_BTN10) 
		||(m_btnID == IDC_BTN_CLOSE) ||(upBtn->m_btnID == IDC_BTN_CLOSE) 
		) 
	{

	}
	else
	{
		//如果都是工具条中的按钮,同时upBtn的窗口不是工具箱
		if( (m_pParentWnd->m_hWnd == CDlgToolBar::g_toolBarHWND) 
			&& (m_pParentWnd->m_hWnd == (upWnd->GetParent())->m_hWnd) 
			&& (upBtn->m_hWnd != CDlgFuck::g_toolBoxHWND) )
		{
           CRect downBtnRt,upBtnRt,barRt;
		   int downPtRelativetoBarY,upPtRelativetoBarY;
           pToolbar = (CDlgToolBar *)m_pParentWnd;
		   pToolbar->GetWindowRect(&barRt);
           GetWindowRect(&downBtnRt);
		   upBtn->GetWindowRect(&upBtnRt);
		   downPtRelativetoBarY = downBtnRt.top - barRt.top;
           upPtRelativetoBarY = upBtnRt.top - barRt.top;
		   pToolbar->CloseAllChildToolBox();
		   pToolbar->ChangButton(this,downPtRelativetoBarY,upBtn,upPtRelativetoBarY);
		  
		}
			//如果都是工具箱中的按钮
		else if(  (m_pParentWnd->m_hWnd == CDlgFuck::g_toolBoxHWND) 
			&& (m_pParentWnd->m_hWnd == (upWnd->GetParent())->m_hWnd)
			&& (upBtn->m_hWnd != CDlgFuck::g_toolBoxHWND))
		{
           CRect downBtnRt,upBtnRt,boxRt;
		   int downPtRelativetoBarX,downPtRelativetoBarY,upPtRelativetoBarX,upPtRelativetoBarY;
           pToolbox = (CDlgFuck *)m_pParentWnd;
		   pToolbox->GetWindowRect(&boxRt);
          // pToolbar = (CDlgToolBar *)pToolbox->GetParentWnd();
           GetWindowRect(&downBtnRt);
		   upBtn->GetWindowRect(&upBtnRt);
		   downPtRelativetoBarX = downBtnRt.left - boxRt.left;
		   downPtRelativetoBarY = downBtnRt.top - boxRt.top;
           upPtRelativetoBarX = upBtnRt.left - boxRt.left;
           upPtRelativetoBarY = upBtnRt.top - boxRt.top;
		   pToolbox->ChangButton(this,downPtRelativetoBarX,downPtRelativetoBarY,upBtn,upPtRelativetoBarX,upPtRelativetoBarY);
	   //  pToolbar->CloseAllChildToolBox();
		}
		//如果DOWN是工具箱，UP是工具条
		else if((m_pParentWnd->m_hWnd == CDlgFuck::g_toolBoxHWND) 
			&& ( (upWnd->GetParent())->m_hWnd == CDlgToolBar::g_toolBarHWND)
			&& (upBtn->m_hWnd != CDlgFuck::g_toolBoxHWND))
		{
           CRect downBtnRt,upBtnRt,boxRt,barRt;
		   int downPtRelativetoBoxX,downPtRelativetoBoxY,upPtRelativetoBarX,upPtRelativetoBarY;
           eType upBtnType;
           pToolbox = (CDlgFuck *)m_pParentWnd;//如果DOWN是工具箱
		   pToolbox->GetWindowRect(&boxRt);
           pToolbar = (CDlgToolBar *)(upBtn->m_pParentWnd);//UP是工具条
		   pToolbar->GetWindowRect(&barRt);
           GetWindowRect(&downBtnRt); //DOWN按钮
		   upBtn->GetWindowRect(&upBtnRt);//UP按钮
		   downPtRelativetoBoxX = downBtnRt.left - boxRt.left;
		   downPtRelativetoBoxY = downBtnRt.top - boxRt.top;
           upPtRelativetoBarX = upBtnRt.left - barRt.left;
           upPtRelativetoBarY = upBtnRt.top - barRt.top;
		   upBtnType = upBtn->GetType();
		   //先执行UP的按钮
		   pToolbar->DelAndNewBtn(upBtn,upPtRelativetoBarY,m_btnType,downBtnText);//up的传down的类型和标题
		   pToolbox->DelAndNewBtn(this,downPtRelativetoBoxX,downPtRelativetoBoxY,upBtnType,upBtnText);    //down的up传的类型和标题  
		   //  pToolbar->CloseAllChildToolBox();
		   return ;

		}
		//如果DOWN是工具条，UP是工具箱
		else if((m_pParentWnd->m_hWnd == CDlgToolBar::g_toolBarHWND) 
			&& ( (upWnd->GetParent())->m_hWnd == CDlgFuck::g_toolBoxHWND))
		{
           CRect downBtnRt,upBtnRt,boxRt,barRt;
		   int downPtRelativetoBoxX,downPtRelativetoBoxY,upPtRelativetoBarX,upPtRelativetoBarY;
           eType upBtnType;
           pToolbar = (CDlgToolBar *)m_pParentWnd;//如果DOWN是工具条
		   pToolbar->GetWindowRect(&barRt);
           pToolbox = ( CDlgFuck*)(upBtn->m_pParentWnd);//UP是工具箱
		   pToolbox->GetWindowRect(&boxRt);
           GetWindowRect(&downBtnRt); //DOWN按钮
		   upBtn->GetWindowRect(&upBtnRt);//UP按钮
		   downPtRelativetoBoxX = downBtnRt.left - barRt.left;
		   downPtRelativetoBoxY = downBtnRt.top - barRt.top;
           upPtRelativetoBarX = upBtnRt.left - boxRt.left;
           upPtRelativetoBarY = upBtnRt.top - boxRt.top;
		   upBtnType = upBtn->GetType();
		   //先执行UP的按钮		  
		   pToolbox->DelAndNewBtn(upBtn,upPtRelativetoBarX,upPtRelativetoBarY,m_btnType,downBtnText);
           pToolbar->DelAndNewBtn(this,downPtRelativetoBoxY,upBtnType,upBtnText);
		   // pToolbar->CloseAllChildToolBox();
		   return;

		}


	}

	CButton::OnLButtonUp(nFlags, point);
}

void CTransparentButtonEx::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bMouseIn)
	{
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.dwHoverTime = 1;
		tme.hwndTrack = m_hWnd;
		m_bMouseIn = TrackMouseEvent(&tme);
	}
	CButton::OnMouseMove(nFlags, point);
}

LRESULT  CTransparentButtonEx::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	    
		CRect rect;
		GetClientRect(&rect);
		CClientDC dc(this);
		Graphics graph(dc.GetSafeHdc());
	//	int nMode = GetCurrentMode();
		if (m_pImg)
		{
			Rect Rt(0, 0, rect.Width(), rect.Height());
			graph.DrawImage(m_pImg, Rt, rect.Width()*0, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片
		}
        m_bMouseIn = FALSE;
	return 0;
}
LRESULT  CTransparentButtonEx::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
		CRect rect;
		GetClientRect(&rect);
		CClientDC dc(this);
		Graphics graph(dc.GetSafeHdc());
	//	int nMode = GetCurrentMode();
		if (m_pImg)
		{
			Rect Rt(0, 0, rect.Width(), rect.Height());
			graph.DrawImage(m_pImg, Rt, rect.Width()*1, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片
		}
    return 0;
	//CButton::OnMouseHover(nFlags, point);
}

BOOL CTransparentButtonEx::OnEraseBkgnd(CDC* pDC)
{
	return true;

	//return CButton::OnEraseBkgnd(pDC);
}
void CTransparentButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//模式0正常1鼠标move2鼠标按下去
	{
		CRect rect;
		GetClientRect(&rect);
		Graphics graph(lpDrawItemStruct->hDC);
		CDC *pDc = CDC::FromHandle(lpDrawItemStruct->hDC);
	//	int nMode = GetCurrentMode();
		if (m_pImg)
		{
			Rect Rt(0, 0, rect.Width(), rect.Height());
			graph.DrawImage(m_pImg, Rt, rect.Width()*0, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片
		}

	}
	
}
BOOL CTransparentButtonEx::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	m_pParentWnd = pParentWnd;
	m_btnID = nID;
	return CButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}
void CTransparentButtonEx::SetParentWnd(CWnd* pWnd)
{
	m_pParentWnd = pWnd;
}
void CTransparentButtonEx::SetName(CString strName)
{
	m_strName = strName;
}
CString CTransparentButtonEx::GetName()
{
	return m_strName;
}



