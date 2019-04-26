// TransparentButtonEx.cpp : ʵ���ļ�
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



// CTransparentButtonEx ��Ϣ�������



void CTransparentButtonEx::Init() //����ͼƬ·��
{
	m_mapPath.clear();
	string strPath = CW2A(GetAppPath());
	strPath += "\\SKIN\\HPWBModel\\72-72��ť\\";
	m_mapPath[TYPE_SELECT] = strPath + "ѡ��.png";
	m_mapPath[TYPE_PAGE] = strPath + "ҳ.png";
	m_mapPath[TYPE_FULLSCREEN] = strPath + "ȫ��.png";
	m_mapPath[TYPE_ARROWS] = strPath + "ֱ�ߺͼ�ͷ.png";
	m_mapPath[TYPE_BIGHT] = strPath + "����.png";
	m_mapPath[TYPE_LINE] = strPath + "ֱ��.png";
	m_mapPath[TYPE_RUBBER] = strPath  + "��Ƥ��.png";
	m_mapPath[TYPE_IMAGE] = strPath + "����ͼƬ.png";
	m_mapPath[TYPE_VIDEO] = strPath + "��Ƶ.png";
	m_mapPath[TYPE_DOCUMENT] = strPath + "�ĵ�.png";
	m_mapPath[TYPE_ALLTOOL] = strPath + "������.png";
	m_mapPath[TYPE_RESOURCE] = strPath + "��ԴĿ¼.png";
	m_mapPath[TYPE_DEL] = strPath + "ɾ��.png";
	m_mapPath[TYPE_PRE_PAGE] = strPath + "��һҳ��.png";
	m_mapPath[TYPE_NEXT_PAGE] = strPath  + "��һҳ��.png";
	m_mapPath[TYPE_T] = strPath + "����.png";
	m_mapPath[TYPE_REDO] = strPath + "redo.png";
	m_mapPath[TYPE_UNDO] = strPath + "undo.png";
	m_mapPath[TYPE_EXITFULLSCREEN] = strPath + "�˳�ȫ��.png";
	m_mapPath[TYPE_RUBBER_BIG] = strPath + "����Ƥ��.png";
	m_mapPath[TYPE_RUBBER_SMALL] = strPath + "С��Ƥ��.png";
	m_mapPath[TYPE_RUBBER_CURVE] = strPath + "���߲�.png";
	m_mapPath[TYPE_RUBBER_OBJ] = strPath + "�����.png";
	m_mapPath[TYPE_TRIANGLE] = strPath + "������.png";
	m_mapPath[TYPE_RECTANGLE] = strPath + "����.png";
	m_mapPath[TYPE_ROUNDNESS] = strPath + "Բ��.png";
	m_mapPath[TYPE_RHOMB] = strPath + "����.png";
	m_mapPath[TYPE_POLYGON] = strPath + "�����.png";
	m_mapPath[TYPE_FONT] = strPath + "����.png";
	m_mapPath[TYPE_ATTRIBUTE] = strPath + "����.png";
	m_mapPath[TYPE_SCREENSHOTS] = strPath + "����.png";
	m_mapPath[TYPE_FLASH] = strPath + "flash.png";
	m_mapPath[TYPE_SAVE] = strPath + "����.png";
	m_mapPath[TYPE_CLOSE] = strPath + "�˳�.png";
	m_mapPath[TYPE_PRINT] = strPath + "��ӡ��.png";
	m_mapPath[TYPE_OPEN] = strPath + "��.png";
	m_mapPath[TYPE_RESAVE] = strPath + "���.png";
	m_mapPath[TYPE_DLG_RESOURCE] = strPath + "��Դ������.png";
	m_mapPath[TYPE_DLG_RUBBER] = strPath + "���ӹ�����.png";
	m_mapPath[TYPE_DLG_GRAPHICS] = strPath + "ͼ�ι�����.png";
	m_mapPath[TYPE_DLG_FILE] = strPath + "�ļ�������.png";
    m_mapPath[TYPE_DLG_PAGE] = strPath + "ҳ������.png";
	m_mapPath[TYPE_CLR_REN] = strPath + "��ɫ.png";
	m_mapPath[TYPE_CLR_BLACK] = strPath + "��ɫ.png";
	m_mapPath[TYPE_CLR_YELLOW] = strPath + "��ɫ.png";
	m_mapPath[TYPE_CLR_ORANGE] = strPath + "��ɫ.png";
	m_mapPath[TYPE_CLR_BLUE] = strPath + "��ɫ.png";
	m_mapPath[TYPE_CLR_PURPLE] = strPath + "��ɫ.png";
	m_mapPath[TYPE_MAQN] = strPath + "�Ŵ�.png";
	m_mapPath[TYPE_SPOTLIGHT] = strPath + "�۹��.png";
	m_mapPath[TYPE_DESKTOPDRAW] = strPath + "�����ͼ.png";
    m_mapPath[TYPE_DDOPEN] = strPath + "�����.png";
	m_mapPath[TYPE_DDFULLSCREEN] = strPath + "����ȫ��.png";
	m_mapPath[TYPE_DDEFFECT] = strPath + "�����л�.png";
	m_mapPath[TYPE_DDCTRL] = strPath + "���������.png";
	m_mapPath[TYPE_DDCLOSE] = strPath + "����ر�.png";
	m_mapPath[TYPE_FUNCTIONDLG] = strPath + "�������.png";

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
	CString downBtnText(_T("����DOWN��ť�ı���")),upBtnText(_T("����UP��ť�ı���"));
    CTransparentButtonEx *upBtn;  //��ק�ƶ���ťʱUP�İ�ťָ��
    m_upPoint = point;
    ClientToScreen(&m_upPoint);  //��UP�ĵ�����ת��Ϊ��Ļ����
	upWnd = WindowFromPoint(m_upPoint);
    upBtn =(CTransparentButtonEx *)WindowFromPoint(m_upPoint);
	GetWindowText(downBtnText);
	upBtn->GetWindowText(upBtnText);
	//�����DOWN��UP��ͬһ����ť�������䡢�ǹرհ�ťʲô������
	if((this->m_hWnd == upBtn->m_hWnd) 
		||(m_btnID == IDC_BTN10) ||(upBtn->m_btnID == IDC_BTN10) 
		||(m_btnID == IDC_BTN_CLOSE) ||(upBtn->m_btnID == IDC_BTN_CLOSE) 
		) 
	{

	}
	else
	{
		//������ǹ������еİ�ť,ͬʱupBtn�Ĵ��ڲ��ǹ�����
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
			//������ǹ������еİ�ť
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
		//���DOWN�ǹ����䣬UP�ǹ�����
		else if((m_pParentWnd->m_hWnd == CDlgFuck::g_toolBoxHWND) 
			&& ( (upWnd->GetParent())->m_hWnd == CDlgToolBar::g_toolBarHWND)
			&& (upBtn->m_hWnd != CDlgFuck::g_toolBoxHWND))
		{
           CRect downBtnRt,upBtnRt,boxRt,barRt;
		   int downPtRelativetoBoxX,downPtRelativetoBoxY,upPtRelativetoBarX,upPtRelativetoBarY;
           eType upBtnType;
           pToolbox = (CDlgFuck *)m_pParentWnd;//���DOWN�ǹ�����
		   pToolbox->GetWindowRect(&boxRt);
           pToolbar = (CDlgToolBar *)(upBtn->m_pParentWnd);//UP�ǹ�����
		   pToolbar->GetWindowRect(&barRt);
           GetWindowRect(&downBtnRt); //DOWN��ť
		   upBtn->GetWindowRect(&upBtnRt);//UP��ť
		   downPtRelativetoBoxX = downBtnRt.left - boxRt.left;
		   downPtRelativetoBoxY = downBtnRt.top - boxRt.top;
           upPtRelativetoBarX = upBtnRt.left - barRt.left;
           upPtRelativetoBarY = upBtnRt.top - barRt.top;
		   upBtnType = upBtn->GetType();
		   //��ִ��UP�İ�ť
		   pToolbar->DelAndNewBtn(upBtn,upPtRelativetoBarY,m_btnType,downBtnText);//up�Ĵ�down�����ͺͱ���
		   pToolbox->DelAndNewBtn(this,downPtRelativetoBoxX,downPtRelativetoBoxY,upBtnType,upBtnText);    //down��up�������ͺͱ���  
		   //  pToolbar->CloseAllChildToolBox();
		   return ;

		}
		//���DOWN�ǹ�������UP�ǹ�����
		else if((m_pParentWnd->m_hWnd == CDlgToolBar::g_toolBarHWND) 
			&& ( (upWnd->GetParent())->m_hWnd == CDlgFuck::g_toolBoxHWND))
		{
           CRect downBtnRt,upBtnRt,boxRt,barRt;
		   int downPtRelativetoBoxX,downPtRelativetoBoxY,upPtRelativetoBarX,upPtRelativetoBarY;
           eType upBtnType;
           pToolbar = (CDlgToolBar *)m_pParentWnd;//���DOWN�ǹ�����
		   pToolbar->GetWindowRect(&barRt);
           pToolbox = ( CDlgFuck*)(upBtn->m_pParentWnd);//UP�ǹ�����
		   pToolbox->GetWindowRect(&boxRt);
           GetWindowRect(&downBtnRt); //DOWN��ť
		   upBtn->GetWindowRect(&upBtnRt);//UP��ť
		   downPtRelativetoBoxX = downBtnRt.left - barRt.left;
		   downPtRelativetoBoxY = downBtnRt.top - barRt.top;
           upPtRelativetoBarX = upBtnRt.left - boxRt.left;
           upPtRelativetoBarY = upBtnRt.top - boxRt.top;
		   upBtnType = upBtn->GetType();
		   //��ִ��UP�İ�ť		  
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
			graph.DrawImage(m_pImg, Rt, rect.Width()*0, 0, rect.Width(), rect.Height(), UnitPixel);//����ģʽ�л��ư�ťͼƬ
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
			graph.DrawImage(m_pImg, Rt, rect.Width()*1, 0, rect.Width(), rect.Height(), UnitPixel);//����ģʽ�л��ư�ťͼƬ
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
	//ģʽ0����1���move2��갴��ȥ
	{
		CRect rect;
		GetClientRect(&rect);
		Graphics graph(lpDrawItemStruct->hDC);
		CDC *pDc = CDC::FromHandle(lpDrawItemStruct->hDC);
	//	int nMode = GetCurrentMode();
		if (m_pImg)
		{
			Rect Rt(0, 0, rect.Width(), rect.Height());
			graph.DrawImage(m_pImg, Rt, rect.Width()*0, 0, rect.Width(), rect.Height(), UnitPixel);//����ģʽ�л��ư�ťͼƬ
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



