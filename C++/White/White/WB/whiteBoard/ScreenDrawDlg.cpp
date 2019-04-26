// ScreenDrawDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "ScreenDrawDlg.h"
#include "afxdialogex.h"
#include "WhiteBoardView.h"
//#include ".\TouchAPI.h"
#include <MMSystem.h>
#include "ScreenDrawMgr.h"
#include "TouchAPI.h"
#include "ViewMTObject.h"
#pragma  warning(disable:4244)


// CScreenDrawDlg 对话框
//#include "dwmapi.h"
//#include "DwmapiFun.h"
IMPLEMENT_DYNAMIC(CScreenDrawDlg, CDialog)

CScreenDrawDlg::CScreenDrawDlg(CWnd* pParent)
	: CDialog(CScreenDrawDlg::IDD, pParent)
	, m_LButtonDown(false)
	,m_dwTimeGetTime(0)
	, m_bMoveIsNeedSendData(FALSE)
{
	EnableAutomation();
}

CScreenDrawDlg::CScreenDrawDlg(CRect& rcScreenDlg)
	: CDialog(CScreenDrawDlg::IDD, NULL)
	,m_LButtonDown(false)
	,m_dwTimeGetTime(0)
	, m_bMoveIsNeedSendData(FALSE)
	, m_rect(rcScreenDlg)
{
	m_ptPointLineCurve = CPoint(0,0);
}

CScreenDrawDlg::~CScreenDrawDlg()
{
	
}

void CScreenDrawDlg::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。基类将自动
	// 删除该对象。在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CDialog::OnFinalRelease();
}

void CScreenDrawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CScreenDrawDlg, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_DESTROYSCREEN, &CScreenDrawDlg::OnDestroyScreen)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_MESSAGE(WM_GESTURE, OnGesture)
	ON_MESSAGE(WM_TOUCH, OnTouch)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CScreenDrawDlg, CDialog)

END_DISPATCH_MAP()

// 注意: 我们添加 IID_IScreenDrawDlg 支持
//  以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {79156EC4-7B33-416E-B279-0EF2ADE50A3D}
static const IID IID_IScreenDrawDlg =
{ 0x79156EC4, 0x7B33, 0x416E, { 0xB2, 0x79, 0xE, 0xF2, 0xAD, 0xE5, 0xA, 0x3D } };

BEGIN_INTERFACE_MAP(CScreenDrawDlg, CDialog)
	INTERFACE_PART(CScreenDrawDlg, IID_IScreenDrawDlg, Dispatch)
END_INTERFACE_MAP()


// CScreenDrawDlg 消息处理程序
//设置窗口扩展样式，指定要镂空的掩码色为白色，设置对话框初始位置，这是个异形窗口，只负责图形的显示，捕捉不了鼠标事件
BOOL CScreenDrawDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	::MoveWindow(m_hWnd, m_rect.left,m_rect.top, m_rect.Width(), m_rect.Height(),TRUE);
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetExStyle() | WS_EX_LAYERED );
	VERIFY(::SetLayeredWindowAttributes(m_hWnd,RGB(255,255,255), 0, LWA_COLORKEY));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//左键按下执行CWhiteBoardView的鼠标左键按下操作，并将鼠标左键按下标志设为true
void CScreenDrawDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	g_pWhiteBoardView->SetPrePage(0); //翻页后，如果不是Move或者UP状态，就把m_nPrePage的值设为0
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	if (g_systemVersion >= 6)
	{
		CRect rect = CScreenDrawMgr::GetIns()->m_pMouseDlg->GetRectSize();
		CScreenDrawMgr::GetIns()->m_pMouseDlg->SetWindowPos(CScreenDrawMgr::GetIns()->m_pScreenDrawDlg, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	}

	this->m_LButtonDown=true;

	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	//滚动条偏移量
	UINT XnPos = g_pWhiteBoardView->getXnPos();
	UINT YnPos = g_pWhiteBoardView->getYnPos();
	point.x -= XnPos;
	point.y -= YnPos;

	CClientDC dc(this);
	g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&point);//将视图坐标转换为文档作标
	m_dwTimeGetTime = ::timeGetTime();
	if((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH)
	{
		return;
	}

	//发送关闭子工具条的消息
	::PostMessage(g_pWhiteBoardView->m_hTOOLBARWnd,WM_CLOSE_TOOBAR_MES,0,0);
	if ((g_pWhiteBoardView->GetWBFunctionDlg() != NULL)&&(g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg() != NULL))
	{
		::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_hWnd,WM_CLOSE_TOOBAR_MES,0,0);
	}

	switch(g_nDrawStatus)
	{
	case DRAWANYTRIANGLE:
		{		
			CWBArbitraryTriangle *pTriangle = new CWBArbitraryTriangle(point,CPoint(point.x+30,point.y+100),CPoint(point.x+100,point.y+100));
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pTriangle,WB_LAYER_LABEL);
			pTriangle->SetAlpha((int)g_nTranspar);
			g_nDrawStatus = DRAWINGANYTRIANGLE;
		}
		break;
	case DRAW_LINE:	
		{
			CWBLine *pLine = new CWBLine(point, point);
			if (pLine == NULL)
			{
				break;
			}
			m_ptPointLineCurve = point;
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pLine, WB_LAYER_LABEL);
			pLine->SetAlpha(g_nTranspar);
			g_nDrawStatus = DRAWING_LINE;
		}
		break;
	case DRAW_NORMAL:
		{
			//判断点是否在某个选择框或按钮内
			int nTemp;
			nTemp = g_pWBPage->PointInSelectFrame(point);

			if (nTemp != WB_PS_NONE)
			{
				if (nTemp == WB_PS_TRIAMGLEPOINT)
				{
					g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
					g_nDrawStatus = EDITTRIANGLE;
					return;
				}
				if (nTemp == WB_PS_CTRLBUTTON)//如果点在某个控制按钮内
				{	
					g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
					CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
					if (pObj == NULL)
					{
						g_nDrawStatus = DRAW_NORMAL;
						return;
					}
					if (pObj->GetType() == WB_OBJ_COMBINE)
					{
						CWBObject *pCtrlObj = ((CWBCombine *)pObj)->SetControlObj(point);
						if (pCtrlObj == NULL)
						{
							g_nDrawStatus = DRAW_NORMAL;
							return;
						}
						pObj = pCtrlObj;
					}
					(pObj->GetControlButton())->ButtonDown(point);
					g_nDrawStatus = ENTER_CONTROL;
					Invalidate(FALSE);
					break;
				}

				if (nTemp == WB_PS_SELECTBUTTON)	//如果点在某个选择框按钮内
				{
					g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
					CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
					if (pObj == NULL)
					{
						g_nDrawStatus = DRAW_NORMAL;
						return;
					}
					(pObj->GetImageButton())->ButtonDown(point);
					g_nDrawStatus = ENTER_BUTTON;
					g_pWhiteBoardView->m_ptPaste = point;
					break;
				}

				if (nTemp == WB_PS_SELECTFRAME)	//如果点在某个选择框内
				{
					g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
					CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
					if (pObj == NULL)
					{
						g_nDrawStatus = DRAW_NORMAL;
						return;
					}
					if (pObj->IsInfiniteClone() && !pObj->IsLock())
					{
						pObj->SetClone(TRUE);
					}

					g_pWhiteBoardView->m_ptSelect = point;

					//denghong设置移动偏移基点
					g_pWhiteBoardView->m_moveFirstPoint = point;
					g_pWhiteBoardView->m_ulMoveCounts = 0;
					g_nDrawStatus = MOVING_LINE;
					break;
				}

				if (nTemp == WB_PS_SELECTPOINT)	//如果点在标注尖角的点的范围内
				{
					CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurAnnotationsLayerIndex);
					if (pObj == NULL) //如果pObj为NULL，则备注可能属于某一文档或图片
					{
						g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
						CWBObject *pObject = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
						if (pObject != NULL)
						{
							if (pObject->IsLock() || pObject->IsLimit())
							{
								break;
							}
							if (pObject->GetType() == WB_OBJ_DOCUMENT)
							{
								pObj = ((CWBDocument *)pObject)->GetAnn();
							}
							if (pObject->GetType() == WB_OBJ_PICTURE)
							{
								pObj = ((CWBPicture *)pObject)->GetAnn();
							}
						}
					}
					if (pObj != NULL)
					{
						if (pObj->IsLock() || pObj->IsLimit())
						{
							break;
						}
						g_nDrawStatus = CHANGE_ANNOTATIONS;
					}
					break;
				}
			}

			if ((nFlags & MK_CONTROL) != MK_CONTROL)
			{
				BOOL bFlag = FALSE;
				if ((int)((g_pWBPage->GetWBSelectFrameList()).size()) > 0)
				{
					bFlag = TRUE;
				}
				g_pWBPage->ClearSelectFrameList();
				if (bFlag)
				{
					g_pWhiteBoardView->m_pageIsNeedDraw = TRUE;
					Invalidate();
					UpdateWindow();
				}
			}
			g_pWhiteBoardView->m_rectSelect = CRect(point, point);
			g_nDrawStatus = DRAW_SELECTINT;
		}
		break;
	case DRAW_ROUND:
		break;
	case DRAW_POLYLINE: //画任意线
		{
			CWBCurve *pCurve = new CWBCurve();
			if (pCurve == NULL)
			{
				break;
			}
			m_ptPointLineCurve = point;
			pCurve->SetResume_broken_taransfer(true);
			pCurve->SetAlpha(g_nTranspar);
			pCurve->SetLeftTopRightBottomPt(point, point);
			pCurve->AddPoint(point);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pCurve, WB_LAYER_LABEL);
//			g_pWhiteBoardView->m_topLeft = point;
			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			g_nDrawStatus = DRAWING_POLYLINE;
			Invalidate();
			g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
		}
		break;	
	case DRAW_POLYICON:
		{
			CWBIcoCurve * pIcoCurve = new CWBIcoCurve();
			if(pIcoCurve == NULL)
			{
				break;
			}
			pIcoCurve->SetResume_broken_taransfer(true);
			pIcoCurve->SetAlpha((int)g_nTranspar);
			pIcoCurve->AddPoint(point);
			pIcoCurve->SetBeginPoint(point);
			pIcoCurve->SetLeftTopRightBottomPt(point, point);

			pIcoCurve->SetIsLoadPicFromFile(g_bIsWBLoadIcoFromFile); //设置是否从文件加载

			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pIcoCurve,WB_LAYER_LABEL);
			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			g_nDrawStatus = DRAWING_POLYICON;
			Invalidate();
			g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
		}
		break;
	case DRAW_RECT:
		{
			CWBRectangle *pRect = new CWBRectangle(point, point);
			//pRect->SetFillColor(RGB(0, 123, 34));
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pRect, WB_LAYER_LABEL);
			pRect->SetAlpha(g_nTranspar);
			g_nDrawStatus = DRAWING_RECT;
		}
		break;
	case DRAW_PALLELOGAM:
		{
			CWBPallelogam *pPallelogam = new CWBPallelogam(point,point);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pPallelogam,WB_LAYER_LABEL);
			pPallelogam->SetAlpha(g_nTranspar);
			g_nDrawStatus = DRAWING_PALLELOGAM;
		}
		break;
	case DRAWED_TEXT:
		{
			if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				//SendMessage(UWM_DESTROY_RICHEDIT, 0, 0);
				//PostMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			}
			g_pWhiteBoardView->PostMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			g_nDrawStatus = DRAWEUP_TEXT;
		}
		break;
	case DRAW_TEXT:
		{
			g_pWhiteBoardView->m_bIsAnnotations = FALSE;
			g_pWhiteBoardView->m_rectText.left = point.x;
			g_pWhiteBoardView->m_rectText.top = point.y;
			if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				g_pWhiteBoardView->SendMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			}
			CWBRichEditMgr::GetInst_()->SetBeginPoint(point.x, point.y);
			g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			g_nDrawStatus = DRAWING_TEXT;
		}
		break;
	case DRAW_ELLIPSE:
		{	
			g_pWhiteBoardView->m_topLeft = point;
			CWBEllipse *pEllipse = new CWBEllipse(g_pWhiteBoardView->m_topLeft, point);
			if (pEllipse == NULL)
			{
				break;
			}
			pEllipse->SetAlpha(g_nTranspar);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pEllipse, WB_LAYER_LABEL);			
			g_nDrawStatus = DRAWING_ELLIPSE;
		}
		break;
	case DRAW_TRIANGLE:	//画三角形
		{
			CWBTriangle *pTriangle = new CWBTriangle(point, point,g_pWhiteBoardView->GetDrawTriangleType());
			//			pTriangle->SetFillColor(RGB(0, 123, 34));
			if (pTriangle == NULL)
			{
				break;
			}
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pTriangle, WB_LAYER_LABEL);
			pTriangle->SetAlpha(g_nTranspar);
			g_nDrawStatus = DRAWING_TRIANGLE;
		}
		break;
	case DRAW_POLYGON:	//画多边形
		{
			g_pWhiteBoardView->m_topLeft = point;
			CWBPolygon *pPolygon = new CWBPolygon();
			if (pPolygon == NULL)
			{
				break;
			}
			pPolygon->SetAlpha(g_nTranspar);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pPolygon, WB_LAYER_LABEL);
			pPolygon->AddPoint(point);			
		}
		break;
	case DRAW_RHOMBUS:	//菱形
		{
			CWBRhombus *pRhombus = new CWBRhombus(point, point);
			if (pRhombus == NULL)
			{
				break;
			}
			//			pRhombus->SetFillColor(RGB(0, 123, 34));
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pRhombus, WB_LAYER_LABEL);
			pRhombus->SetAlpha(g_nTranspar);
			g_nDrawStatus = DRAWING_RHOMBUS;		
		}
		break;
	case DRAW_PENTAGON: //画五边形
		{
			CWBPentagon *pPentagon = new CWBPentagon(point,point);
			if (pPentagon == NULL)
			{
				break;
			}
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pPentagon, WB_LAYER_LABEL);
			pPentagon->SetAlpha((int)g_nTranspar);
			g_nDrawStatus =DRAWING_PENTAGON;
		}
		break;
	case DRAW_HEXAGON: //画六边形
		{
			CWBHexagon *pHexagon = new CWBHexagon(point,point);
			if (pHexagon == NULL)
			{
				break;
			}
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pHexagon, WB_LAYER_LABEL);
			pHexagon->SetAlpha((int)g_nTranspar);
			g_nDrawStatus =DRAWING_HEXAGON;
		}
		break;
	case DRAW_FIVEPOINTSTART: //画五角星
		{
			CWBFivepointstart *pFivepointstart = new CWBFivepointstart(point,point);
			if (pFivepointstart == NULL)
			{
				break;
			}
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pFivepointstart, WB_LAYER_LABEL);
			pFivepointstart->SetAlpha((int)g_nTranspar);
			g_nDrawStatus =DRAWING_FIVEPOINTSTART;
		}
		break;
	case DRAW_TRAPEZIUM: //画梯形
		{
			CWBTrapezium *pTrapezium = new CWBTrapezium(point,point);
			if (pTrapezium == NULL)
			{
				break;
			}
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pTrapezium, WB_LAYER_LABEL);
			pTrapezium->SetAlpha((int)g_nTranspar);
			g_nDrawStatus =DRAWING_TRAPEZIUM;
		}
		break;
	case DRAW_ERASE: //擦除
		{
			g_nDrawStatus = DRAW_ERASING;
			g_pWhiteBoardView->m_ptErasePre = point;
			g_pWBPage->GetAllObjLayerIndex(g_pWhiteBoardView->m_vecLineLayerIndexBeforeErase, WB_OBJ_LINE); //保存擦除之前所有直线对象的图层号
		}
		break;
	case DRAW_RGNDELETE: //区域删除(貌似明明是曲线删除的????????)
		{
			if (g_pWhiteBoardView->m_pRgnDeleteWBCurve != NULL)
			{
				delete g_pWhiteBoardView->m_pRgnDeleteWBCurve;
				g_pWhiteBoardView->m_pRgnDeleteWBCurve = NULL;
			}
			PWBCurveItem pWBCurveItem = new WBCurveItem;
			pWBCurveItem->SetWidth(2);
			//pWBCurveItem->nWidth = 1;
			pWBCurveItem->SetColor(RGB(0, 255, 0));
			//pWBCurveItem->clrLine = RGB(0, 255, 0);
			g_pWhiteBoardView->m_pRgnDeleteWBCurve = new CWBCurve(pWBCurveItem);
			g_pWhiteBoardView->m_pRgnDeleteWBCurve->AddPoint(point);

			delete pWBCurveItem;
			g_nDrawStatus = DRAW_RGNDELETING;
		}		
		break;
	case DRAW_CLEANUP: //清除
		g_nDrawStatus = DRAW_CLEANUPING;
		break;
	case ADD_ANNOTATIONS:	//添加标注
		{
			g_pWhiteBoardView->m_bIsAnnotations = TRUE;
			g_pWhiteBoardView->m_rectText.left = point.x;
			g_pWhiteBoardView->m_rectText.top = point.y;

			if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				g_pWhiteBoardView->SendMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			}
			CWBRichEditMgr::GetInst_()->SetBeginPoint(point.x, point.y);

			g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			g_nDrawStatus = DRAWING_TEXT;
		}
		break;
	default:
		break;
	}
	g_pWhiteBoardView->m_dwCurOperationObjId = g_pWhiteBoardView->m_dwCurObjLayerIndex;

	CDialog::OnLButtonDown(nFlags, point);
}

//左键松开是执行CWhiteBoardView的鼠标左键松开操作，并让窗口区无效，将鼠标按下标志为设为false
void CScreenDrawDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	//// TODO: 在此添加消息处理程序代码和/或调用默认值
	this->m_LButtonDown=false;

	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	if (g_systemVersion >= 6)
	{
		if (CScreenDrawMgr::GetIns())
		{
			CRect rect = CScreenDrawMgr::GetIns()->m_pMouseDlg->GetRectSize();
			CScreenDrawMgr::GetIns()->m_pMouseDlg->SetWindowPos(this, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
			CScreenDrawMgr::GetIns()->m_pMouseDlg->SetMouseStatue(false);
		}
	}

	if (g_pWhiteBoardView == NULL)
	{
		return;
	}

	CClientDC dc(this);
	dc.DPtoLP(&point);//将视图坐标转换为文档作标

	if ( (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH )
	{
		return ;
	}
	
	//处理放大镜
	if(g_pWhiteBoardView->MouseUpMagn(point))
	{
		return;
	}

	switch(g_nDrawStatus)
	{
	case EDITTRIANGLE:
		{
			CWBArbitraryTriangle *pTriangle = (CWBArbitraryTriangle *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_ARBITRARYTRIANGLE);
			if (pTriangle != NULL)
			{
				pTriangle->EditTrianglePoint(point,true);	
				pTriangle->AdjustObject();
				//pTriangle->SetIsEdit(false);
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pTriangle->SetXmlValue(xmlObj);
				g_pWhiteBoardView->ChangeObj(xmlObj);
			}                    
			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	case DRAWINGANYTRIANGLE:
		{
			CWBArbitraryTriangle *pTriangle = (CWBArbitraryTriangle *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_ARBITRARYTRIANGLE);
			if (pTriangle != NULL)
			{
				pTriangle->Move(point);	
				if (pTriangle->CanbeTriangle())
				{
					pTriangle->CreateSelecrtFrame();
				}
				else
				{
					pTriangle->Move(point,CPoint(point.x +30,point.y+100),CPoint(point.x+100,point.y+100));
					pTriangle->CreateSelecrtFrame();
				}
				pTriangle->AdjustObject();

				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pTriangle->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}                               
			g_nDrawStatus = DRAWANYTRIANGLE;	
		}
		break;
	case DRAW_SELECTINT:
		{	
			g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			//调整画出的选择框矩形
			int nTemp;
			if (g_pWhiteBoardView->m_rectSelect.left >g_pWhiteBoardView->m_rectSelect.right)
			{
				nTemp = g_pWhiteBoardView->m_rectSelect.left;
				g_pWhiteBoardView->m_rectSelect.left = g_pWhiteBoardView->m_rectSelect.right;
				g_pWhiteBoardView->m_rectSelect.right = nTemp;
			}
			if (g_pWhiteBoardView->m_rectSelect.bottom < g_pWhiteBoardView->m_rectSelect.top)
			{
				nTemp = g_pWhiteBoardView->m_rectSelect.top;
				g_pWhiteBoardView->m_rectSelect.top = g_pWhiteBoardView->m_rectSelect.bottom;
				g_pWhiteBoardView->m_rectSelect.bottom = nTemp;
			}

			if ((g_pWhiteBoardView->m_rectSelect.right - g_pWhiteBoardView->m_rectSelect.left) < 10 
				|| (g_pWhiteBoardView->m_rectSelect.bottom - g_pWhiteBoardView->m_rectSelect.top) < 10) //点选
			{
				g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = g_pWBPage->GetSelectObject(point);
				g_pWBPage->AddSelectFrame(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
				g_nDrawStatus = DRAW_NORMAL;
				if (g_pWhiteBoardView->m_dwCurSelectObjLayerIndex > 0)
				{
					g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				}
				Invalidate();
			}
			else //框选
			{
				if (
					g_pWhiteBoardView->m_rectSelect.left >= g_pWhiteBoardView->m_rect.left 
					|| g_pWhiteBoardView->m_rectSelect.bottom >= g_pWhiteBoardView->m_rect.bottom
					)
				{
					if (g_pWBPage->AddSelectFrames(g_pWhiteBoardView->m_rectSelect) > 0)
					{
						g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
					}
					g_nDrawStatus = DRAW_NORMAL;
					Invalidate();
				}
			}
		}
		break;
	case DRAWING_LINE:
		{				
			CWBLine *pLine = (CWBLine *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_LINE);
			if (pLine != NULL)
			{
				//wangxiaobin由于点击一下点太小了，看不到，所以扩大点击只有一点时候点的大小
				if (CalDistancePointToPoint(point,m_ptPointLineCurve) <5.0)
				{
					point.x += g_nWBCurObjWidth;
				}
				pLine->Move(point, FALSE);
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pLine->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}	

			g_nDrawStatus = DRAW_LINE;
		}			
		break;

	case MOVING_LINE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
			if (pObj != NULL)
			{
				pObj->SetClone(FALSE);
			}

			//denghong 移动直线偏移量
			CPoint moveOffset = point - g_pWhiteBoardView->m_moveFirstPoint;
			if ((nFlags & MK_CONTROL) == MK_CONTROL && moveOffset.x == 0 && moveOffset.y == 0)
			{
				g_pWBPage->DeleteSelectFrame(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
				g_nDrawStatus = DRAW_NORMAL;
				Invalidate();
				break;
			}
			else if (moveOffset.x == 0 && moveOffset.y == 0)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			if (point != g_pWhiteBoardView->m_moveFirstPoint)
			{
				g_pWhiteBoardView->MoveMsg(moveOffset,GenerateSSRC());
			}
			g_pWhiteBoardView->m_dwCurOperationObjId = 0; //add by huanglongquan
			g_pWhiteBoardView->m_ulMoveCounts = 0;
			g_nDrawStatus = DRAW_NORMAL;
			//g_pWhiteBoardView->EnableMouseOrTouch(FALSE);
			Invalidate();
		}			
		break;

	case DRAW_NORMAL:

		break;
	case DRAW_RECTMAQN:
		{
			if(g_pWhiteBoardView->MouseUpMagn(point))
			{
				return;
			}
		}
		break;
	case DRAW_ROUND:
		//m_draw = DRAW_NORMAL;
		break;
	case DRAWING_POLYLINE:
		{
			CWBCurve *pCurve = (CWBCurve *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_CURVE);
			if (pCurve != NULL)
			{
				if (CalDistancePointToPoint(point,m_ptPointLineCurve) <5.0)
				{
					point.x += g_nWBCurObjWidth;
				}
				pCurve->SetAlpha(g_nTranspar);
				pCurve->AddPoint(point);					
				pCurve->AdjustObject();
				pCurve->CreateSelecrtFrame();

				pCurve->SetResume_broken_taransfer(false);
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pCurve->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}

			g_nDrawStatus = DRAW_POLYLINE;
		}
		break;
	case DRAWING_POLYICON:
		{
			CWBIcoCurve * pIcoCurve = (CWBIcoCurve*)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_ICOCURVE);
			if(pIcoCurve != NULL)
			{
				pIcoCurve->SetAlpha(g_nTranspar);
				pIcoCurve->SetIsRefush(TRUE);
				pIcoCurve->AdjustObject();
				pIcoCurve->CreateSelecrtFrame();
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();

				//发送同步消息
				pIcoCurve->SetResume_broken_taransfer(false);
				XMemIni xmlObj;
				xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_NONE);

				if(g_bIsWBLoadIcoFromFile) //从文件加载
				{
					xmlObj.Set(KEY_EVENT_ID,g_pWhiteBoardView->m_ulIcoCurEvent);
					xmlObj.Set(KEY_FILE_ID, g_pWhiteBoardView->m_dwIcoCurId);
				}

				pIcoCurve->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			}
			g_nDrawStatus = DRAW_POLYICON;
		}
		break;
	case DRAWING_RECT:
		{
			CWBRectangle *pRect = (CWBRectangle *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_RECT);
			if (pRect != NULL)
			{
				pRect->Move(point);
				pRect->AdjustObject();
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pRect->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}                               
		}

		g_nDrawStatus = DRAW_RECT;
		break;
	case DRAWING_PALLELOGAM:
		{
			CWBPallelogam *pPallelogam = (CWBPallelogam *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex,WB_OBJ_PALLELOGAM);
			if (pPallelogam != NULL)
			{
				pPallelogam->Move(point);
				pPallelogam->AdjustObject();
				Invalidate();

				XMemIni xmlObj;
				pPallelogam->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}
			g_nDrawStatus = DRAW_PALLELOGAM;
		}
		break;
	case DRAWING_ELLIPSE:
		{
			CWBEllipse *pEllipse = (CWBEllipse *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_ELLIPSE);
			if (pEllipse != NULL)
			{
				pEllipse->Move(point);	
				pEllipse->AdjustObject();
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pEllipse->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
				
			}				
			g_nDrawStatus = DRAW_ELLIPSE;
		}
		break;
	case DRAWEUP_TEXT:
		{
			Invalidate();
			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	case DRAWING_TEXT:
		{
			if (!CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				g_pWhiteBoardView->PostMessage(UWM_CREATE_RICHEDIT, (WPARAM)(MOUSE_ID), (LPARAM)(MAKELONG(point.x, point.y)));
			}
			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			g_nDrawStatus = DRAWED_TEXT;
			Invalidate();
		}
		break;
	case DRAWING_TRIANGLE:		//三角形
		{
			CWBTriangle *pTriangle = (CWBTriangle *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_TRIANGLE);
			if (pTriangle != NULL)
			{
				pTriangle->Move(point);	
				pTriangle->AdjustObject();
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pTriangle->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}                               
		}			
		g_nDrawStatus = DRAW_TRIANGLE;			
		break;		
	case DRAW_POLYGON:		//多边形
		{		
			CWBPolygon *pPolygon = (CWBPolygon *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_POLYGON);
			if (pPolygon != NULL)
			{
				pPolygon->AddPoint(point);	
				Invalidate();
			} 
			g_nDrawStatus = DRAWING_POLYGON;				
		}							
		break;	
	case DRAWING_POLYGON:		
		{
			CWBPolygon *pPolygon = (CWBPolygon *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_POLYGON);
			if(point.x < g_pWhiteBoardView->m_topLeft.x+10 && point.x>g_pWhiteBoardView->m_topLeft.x-10 &&point.y < g_pWhiteBoardView->m_topLeft.y+10 && point.y>g_pWhiteBoardView->m_topLeft.y-10)
			{	
				g_nDrawStatus = DRAW_POLYGON;
				pPolygon->DeletePoint();
				pPolygon->SetClosed(TRUE);
				//pPolygon->SetFillColor(RGB(0, 123, 34));
				Invalidate();	

				//发送同步消息
				XMemIni xmlObj;
				pPolygon->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}
			else
			{					
				if (pPolygon != NULL)
				{
					pPolygon->AddPoint(point);	
					Invalidate();
				}     
			}
		}
		break;
	case DRAWING_RHOMBUS:
		{
			CWBRhombus *pRhombus = (CWBRhombus *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_RHOMBUS);
			if (pRhombus != NULL)
			{
				pRhombus->Move(point);
				pRhombus->AdjustObject();
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pRhombus->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}
		}
		g_nDrawStatus = DRAW_RHOMBUS;
		break;
	case DRAWING_PENTAGON: //画五边形
		{
			CWBPentagon *pPentagon = (CWBPentagon*)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex,WB_OBJ_PENTAGON);
			if (pPentagon != NULL)
			{
				pPentagon->Move(point);
				pPentagon->AdjustObject();
				Invalidate();

				XMemIni xmlObj;
				pPentagon->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}
			g_nDrawStatus = DRAW_PENTAGON;
		}
		break;
	case DRAWING_HEXAGON: //画六边形
		{
			CWBHexagon *pHexagon = (CWBHexagon*)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex,WB_OBJ_HEXAGON);
			if (pHexagon != NULL)
			{
				pHexagon->Move(point);
				pHexagon->AdjustObject();
				Invalidate();

				XMemIni xmlObj;
				pHexagon->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}
			g_nDrawStatus = DRAW_HEXAGON;
		}
		break;
	case DRAWING_FIVEPOINTSTART: //画五角星
		{
			CWBFivepointstart *pFivepointstart = (CWBFivepointstart*)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex,WB_OBJ_FIVEPOINTSTART);
			if (pFivepointstart != NULL)
			{
				pFivepointstart->Move(point);
				pFivepointstart->AdjustObject();
				Invalidate();
				XMemIni xmlObj;
				pFivepointstart->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}
			g_nDrawStatus = DRAW_FIVEPOINTSTART;
		}
		break;
	case DRAWING_TRAPEZIUM: //画梯形
		{
			CWBTrapezium *pTrapezium = (CWBTrapezium*)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex,WB_OBJ_TRAPEZIUM);
			if (pTrapezium != NULL)
			{
				pTrapezium->Move(point);
				pTrapezium->AdjustObject();
				Invalidate();
				XMemIni xmlObj;
				pTrapezium->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}
			g_nDrawStatus = DRAW_TRAPEZIUM;
		}
		break;
	case DRAW_ERASING: //擦除
		{				
			//denghong
			//设置发送事件ID
			g_pWhiteBoardView->m_ulSendEventID = GenerateSSRC();

			XMemIni xmlobj;
			xmlobj.Set(KEY_BROADCAST_CMD,_E_BROAD_ERASE_UP);
			xmlobj.Set(KEY_FILE_SENDER,g_ulSessionID);
			g_pWhiteBoardView->Broadcast_Data(xmlobj);

			g_pWhiteBoardView->EraseLineMsg(); //擦除直线协同操作
			for (int i = 0; i < g_pWhiteBoardView->m_vecLineLayerIndexBeforeErase.size();++ i )
			{
				if (g_pWhiteBoardView->m_vecLineLayerIndexBeforeErase[i] != NULL)
				{
					delete g_pWhiteBoardView->m_vecLineLayerIndexBeforeErase[i];
				}
			}
			g_pWhiteBoardView->m_vecLineLayerIndexBeforeErase.clear();

			g_pWhiteBoardView->EraseCurveMsg();//擦除曲线协同操作
			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			g_nDrawStatus = DRAW_ERASE;
			Invalidate();

		}			
		break;
	case DRAW_CLEANUPING:
		{
			//发送同步信息
// 			g_pWhiteBoardView->DeleteObjMsg();
// 			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			g_nDrawStatus = DRAW_CLEANUP;
			/*Invalidate();	*/			
		}			
		break;
	case DRAW_RGNDELETING: //区域删除
		{
			if (g_pWhiteBoardView->m_pRgnDeleteWBCurve != NULL)
			{
				g_pWhiteBoardView->m_pRgnDeleteWBCurve->AddPoint(point);

				CRgn *pRgn = new CRgn();
				if (g_pWhiteBoardView->m_pRgnDeleteWBCurve->CreateRgn(pRgn))
				{
					g_pWBPage->DeleteWBObjects(pRgn);
				}
				delete pRgn;

				delete g_pWhiteBoardView->m_pRgnDeleteWBCurve;
				g_pWhiteBoardView->m_pRgnDeleteWBCurve = NULL;
				//发送同步消息
				g_pWhiteBoardView->DeleteObjMsg();

				Invalidate();
					
			}
			g_nDrawStatus = DRAW_RGNDELETE;
		}
		break;
	case ENTER_BUTTON:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
			if (pObj == NULL)
			{
				UINT nPrepage = g_pWhiteBoardView->GetPrePage();
				CWBPage *pPage = g_pWBFile->GetPage(nPrepage);
				if (pPage == NULL)
				{
					return;
				}
				pObj = pPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
			}
			if (pObj != NULL)
			{
				g_pWhiteBoardView->m_bIsTouch = FALSE;
				(pObj->GetImageButton())->ButtonUp(point);
			}
			g_nDrawStatus = DRAW_NORMAL;
			Invalidate();
		}
		break;
	case ENTER_CONTROL:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
			if (pObj == NULL)
			{
				UINT nPrepage = g_pWhiteBoardView->GetPrePage();
				CWBPage *pPage = g_pWBFile->GetPage(nPrepage);
				if (pPage == NULL)
				{
					return;
				}
				pObj = pPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
			}
			if (pObj != NULL)
			{
				if (pObj->GetType() == WB_OBJ_COMBINE)
				{
					CWBObject *pCtrlObj = ((CWBCombine *)pObj)->GetControlObj();
					pObj = pCtrlObj;
				}
				(pObj->GetControlButton())->ButtonUp(point);
			}

			g_nDrawStatus = DRAW_NORMAL;
			Invalidate();
		}
		break;
	case CHANGE_ANNOTATIONS:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurAnnotationsLayerIndex);
			if (pObj == NULL) //如果pObj为NULL，则备注可能属于某一文档
			{
				g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
				CWBObject *pDoc = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
				if (pDoc != NULL)
				{
					if (pDoc->GetType() == WB_OBJ_DOCUMENT)
					{
						pObj = pDoc;
					}
				}
			}
			if (pObj != NULL)
			{
				//发送同步消息
				XMemIni xmlObj;
				pObj->SetXmlValue(xmlObj);
				g_pWhiteBoardView->ChangeObj(xmlObj);
				Invalidate();
			}
			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	default:
		break;;
	}
	g_pWhiteBoardView->m_dwCurOperationObjId = -1;
	
	CDialog::OnLButtonUp(nFlags, point);
}
//鼠标移动时执行CWhiteBoardView中的鼠标移动操作，并让窗口区无效，检测鼠标左键按下的标志位，避免鼠标滑过时也重画，减少系统开销
void CScreenDrawDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		if (g_nDrawStatus != DRAW_NORMAL)
		{
			g_bAvconLock = FALSE;
			SendMessage(WM_LBUTTONUP, NULL, MAKELPARAM(point.x, point.y));
			g_bAvconLock = TRUE;
			g_nDrawStatus = DRAW_NORMAL;
		}
		return;
	}
	

	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	if(this->m_LButtonDown)     //左键按下再做以下操作，added by 陈常涛
	{
		if (g_systemVersion >= 6)
		{
			if (CScreenDrawMgr::GetIns())
			{
				CRect rect = CScreenDrawMgr::GetIns()->m_pMouseDlg->GetRectSize();
				//CScreenDrawMgr::GetIns()->m_pMouseDlg->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(),TRUE);
				CScreenDrawMgr::GetIns()->m_pMouseDlg->SetWindowPos(this, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
			}
		}
		CClientDC dc(this);
		dc.DPtoLP(&point);//将视图坐标转换为文档作标
		DWORD dwTime_difference = ::timeGetTime();
		if ( dwTime_difference - m_dwTimeGetTime >= MOVEISNEEDSENDDATA_TIME)
		{
			//需要发送同步消息
			m_dwTimeGetTime = dwTime_difference;
			m_bMoveIsNeedSendData = TRUE;
		}

		if( (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH )
		{
			return;
		}

		switch(g_nDrawStatus)
		{
		case EDITTRIANGLE:
			{

				CWBArbitraryTriangle *pTriangle = (CWBArbitraryTriangle *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex,WB_OBJ_ARBITRARYTRIANGLE);
				pTriangle->EditTrianglePoint(point);
				XMemIni xmlObj;					
				pTriangle->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
				g_pWhiteBoardView->Broadcast_Data(xmlObj);
				Invalidate();
			}
			break;
		case DRAWINGANYTRIANGLE:
			{
				CWBArbitraryTriangle *pTriangle = (CWBArbitraryTriangle *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_ARBITRARYTRIANGLE);
				if (pTriangle != NULL)
				{
					pTriangle->Move(point);
					pTriangle->SetAlpha((int)g_nTranspar);
					if ( m_bMoveIsNeedSendData)
					{

						XMemIni xmlObj;					
						pTriangle->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);

						m_bMoveIsNeedSendData = FALSE;
					}
					Invalidate();
				}
			}
			break;
		case DRAW_SELECTINT: //矩形选择
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
				g_pWhiteBoardView->m_rectSelect.right = point.x;
				g_pWhiteBoardView->m_rectSelect.bottom = point.y;
				Invalidate();
			}
			break;
		case DRAWING_LINE:
			{
				CWBLine *pLine = (CWBLine *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_LINE);
				if (pLine != NULL)
				{
					pLine->Move(point, FALSE);
					pLine->SetAlpha(g_nTranspar);
					Invalidate();
					if ( m_bMoveIsNeedSendData)
					{
						//发送曲线同步消息
						XMemIni xmlObj;
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						pLine->SetXmlValue(xmlObj);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
						m_bMoveIsNeedSendData = FALSE;
					}
				}
			}		
			break;
		case MOVING_LINE:
			{			
				CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
				if (pObj == NULL)
				{
					g_nDrawStatus = DRAW_NORMAL;
					return;
				}
				if (pObj->IsClone())
				{
					CPoint ptOffset;
					ptOffset = point - g_pWhiteBoardView->m_ptSelect;
					if (!pObj->IsLock() && !pObj->IsLimit() && ptOffset.x != 0 && ptOffset.y != 0)
					{
						g_pWBPage->ClearSelectFrameList();
						g_pWBPage->AddSelectFrame(pObj->GetLayerIndex());
						g_pWhiteBoardView->CloneObject();
						pObj->SetClone(FALSE);
					}
				}
				else
				{
					CPoint ptOffset;
					ptOffset = point - g_pWhiteBoardView->m_ptSelect;
					g_pWBPage->MoveSelectedObj(ptOffset);
					g_pWhiteBoardView->m_dwCurOperationObjId = 0; //add huanglongquan
					if (g_pWhiteBoardView->m_ulMoveCounts %10 == 0)
					{
						CPoint ptOffsetSend = point - g_pWhiteBoardView->m_ptPrePoint;
						g_pWhiteBoardView->m_ptPrePoint = point;
						g_pWhiteBoardView->SendMoveSelectedObjOffset(ptOffsetSend,GenerateSSRC());
					}
					g_pWhiteBoardView->m_ulMoveCounts ++;

					//g_pWBPage->MoveSelectedObj(m_ptSelect, point);
					g_pWhiteBoardView->m_ptSelect = point;
				}
				Invalidate();
				UpdateWindow();
			}
			break;
		case DRAW_ROUND:
			break;

		case DRAWING_POLYLINE:
			{
				CWBCurve *pCurve = (CWBCurve *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_CURVE);
				if (pCurve != NULL)
				{
					UINT nPtNum = pCurve->AddPoint(point);
					pCurve->SetAlpha(g_nTranspar);

					HDC hDC = ::GetDC(GetSafeHwnd());
					pCurve->DrawLast(hDC, 0, 0, nPtNum);
					::ReleaseDC(GetSafeHwnd(),hDC);

					if ( m_bMoveIsNeedSendData)
					{
						//发送曲线同步消息
						pCurve->SetResume_broken_taransfer(true);
						XMemIni xmlObj;
						xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_ADDENDUMCURVE);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						pCurve->SetXmlValue(xmlObj);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
						m_bMoveIsNeedSendData =FALSE;
						/*
						if (pCurve->GetbSendAddCmd())
						{
							g_pWhiteBoardView->AddObj(xmlObj);
							pCurve->SetbSendAddCmd(false);
						}
						else
						{
							g_pWhiteBoardView->Broadcast_Data(xmlObj);
						}
						*/
					}
				}
			}
			break;
		case DRAWING_POLYICON:
			{
				CWBIcoCurve * pIcoCurve = (CWBIcoCurve *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_ICOCURVE);
				if(pIcoCurve != NULL)
				{		
					pIcoCurve->SetAlpha(g_nTranspar);
					CDC * pDC = GetDC();
					UINT x = g_pWhiteBoardView->getXnPos();
					UINT y = g_pWhiteBoardView->getYnPos();
					pIcoCurve->DrawIcoTrack(pDC, point, x, y);
					ReleaseDC(pDC);

					if (m_bMoveIsNeedSendData)
					{
						//发送曲线同步消息
						pIcoCurve->SetResume_broken_taransfer(true);

						XMemIni xmlObj;
						xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_ADDENDUMCURVE);
						pIcoCurve->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);

						if(g_bIsWBLoadIcoFromFile)
						{
							xmlObj.Set(KEY_FILE_ID, g_pWhiteBoardView->m_dwIcoCurId);
						}
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
						m_bMoveIsNeedSendData = FALSE;
					}
				}
			}
			break;
		case DRAWING_RECT:
			{
				CWBRectangle *pRect = (CWBRectangle *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_RECT);
				if (pRect != NULL)
				{
					pRect->Move(point);
					pRect->SetAlpha(g_nTranspar);
					Invalidate();
				}
				if ( m_bMoveIsNeedSendData)
				{
					//发送曲线同步消息
					XMemIni xmlObj;
					pRect->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
			}

			break;
		case DRAWING_PALLELOGAM:
			{
				CWBPallelogam *pPallelogam = (CWBPallelogam*)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex,WB_OBJ_PALLELOGAM);
				if (pPallelogam != NULL)
				{
					pPallelogam->Move(point);
					pPallelogam->SetAlpha(g_nTranspar);
					Invalidate();
				}
				if (m_bMoveIsNeedSendData)
				{
					XMemIni xmlObj;
					pPallelogam->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
			}
			break;
		case DRAWING_ELLIPSE:
			{
				CWBEllipse *pEllipse = (CWBEllipse *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_ELLIPSE);
				if (g_pWhiteBoardView->m_topLeft != point && pEllipse != NULL)
				{
					pEllipse->Move(point);
					pEllipse->SetAlpha(g_nTranspar);
					Invalidate();
				}
				if ( m_bMoveIsNeedSendData)
				{
					//发送曲线同步消息
					XMemIni xmlObj;
					pEllipse->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
			}
			break;
		case DRAWING_TRIANGLE:	
			{
				CWBTriangle *pTriangle = (CWBTriangle *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_TRIANGLE);
				if (pTriangle != NULL)
				{
					pTriangle->Move(point);	
					pTriangle->SetAlpha(g_nTranspar);				
					Invalidate();
				} 
				if ( m_bMoveIsNeedSendData)
				{
					//发送曲线同步消息
					XMemIni xmlObj;
					pTriangle->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
			}			
			break;
		case DRAWING_POLYGON:		//多边形
			{
				CWBPolygon *pPolygon = (CWBPolygon *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_POLYGON);
				if (pPolygon != NULL)
				{
					pPolygon->Move(point);
					pPolygon->SetAlpha(g_nTranspar);				
					Invalidate();
				} 
				if ( m_bMoveIsNeedSendData)
				{
					//发送曲线同步消息
					XMemIni xmlObj;
					pPolygon->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
			}
			break;
		case DRAWING_RHOMBUS:
			{
				CWBRhombus *pRhombus = (CWBRhombus *)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex, WB_OBJ_RHOMBUS);
				if (pRhombus != NULL)
				{
					pRhombus->Move(point);
					pRhombus->SetAlpha(g_nTranspar);
					Invalidate();
				}
				if ( m_bMoveIsNeedSendData)
				{
					//发送曲线同步消息
					XMemIni xmlObj;
					pRhombus->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
			}
			break;	
		case DRAWING_PENTAGON: //画五边形
			{
				CWBPentagon *pPentagon = (CWBPentagon*)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex,WB_OBJ_PENTAGON);
				if (pPentagon != NULL)
				{
					pPentagon->Move(point);
					pPentagon->SetAlpha((int)g_nTranspar);
					if (m_bMoveIsNeedSendData)
					{
						XMemIni xmlObj;
						pPentagon->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
						m_bMoveIsNeedSendData = FALSE;
					}
					Invalidate();
				}
			}
			break;
		case DRAWING_HEXAGON: //画六边形
			{
				CWBHexagon *pHexagon = (CWBHexagon*)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex,WB_OBJ_HEXAGON);
				if (pHexagon != NULL)
				{
					pHexagon->Move(point);
					pHexagon->SetAlpha((int)g_nTranspar);
					if (m_bMoveIsNeedSendData)
					{
						XMemIni xmlObj;
						pHexagon->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
						m_bMoveIsNeedSendData = FALSE;
					}
					Invalidate();
				}
			}
			break;
		case DRAWING_FIVEPOINTSTART: //画五角星
			{
				CWBFivepointstart *pFivepointstart = (CWBFivepointstart*)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex,WB_OBJ_FIVEPOINTSTART);
				if (pFivepointstart != NULL)
				{
					pFivepointstart->Move(point);
					pFivepointstart->SetAlpha((int)g_nTranspar);
					if (m_bMoveIsNeedSendData)
					{
						XMemIni xmlObj;
						pFivepointstart->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
						m_bMoveIsNeedSendData = FALSE;
					}
					Invalidate();
				}
			}
			break;
		case DRAWING_TRAPEZIUM: //画梯形
			{
				CWBTrapezium *pTrapezium = (CWBTrapezium*)g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurObjLayerIndex,WB_OBJ_TRAPEZIUM);
				if (pTrapezium != NULL)
				{
					pTrapezium->Move(point);
					pTrapezium->SetAlpha((int)g_nTranspar);
					if (m_bMoveIsNeedSendData)
					{
						XMemIni xmlObj;
						pTrapezium->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
						m_bMoveIsNeedSendData = FALSE;
					}
					Invalidate();
				}
			}
			break;
		case DRAWING_TEXT:		//画编辑框
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
				g_pWhiteBoardView->m_rectText.right = point.x;
				g_pWhiteBoardView->m_rectText.bottom = point.y;
				Invalidate();
			}
			break;
		case DRAW_ERASING: //正在擦除
			{
				g_pWhiteBoardView->m_rectBrush = CRect(CPoint(point.x-g_nBrushSize, point.y-g_nBrushSize), 
					CPoint(point.x+g_nBrushSize, point.y+g_nBrushSize));
				g_pWBPage->EraseObjects(g_pWhiteBoardView->m_rectBrush);		
				Invalidate();

				int left,right,top,botton;
				left = point.x-g_nBrushSize;
				right = point.x+g_nBrushSize;
				top = point.y-g_nBrushSize;
				botton = point.y+g_nBrushSize;

				XMemIni xmlobj;
				xmlobj.Set(KEY_BROADCAST_CMD,_E_BROADE_ERASE_LINE);
				xmlobj.Set(KEY_ERASE_RECT_LEFT,left);
				xmlobj.Set(KEY_ERASE_RECT_RIGHT,right);
				xmlobj.Set(KEY_ERASE_RECT_TOP,top);
				xmlobj.Set(KEY_ERASE_RECT_BOTTON,botton);
				xmlobj.Set(KEY_FILE_SENDER,g_ulSessionID);
				g_pWhiteBoardView->Broadcast_Data(xmlobj);
			}
			break;
		case DRAW_CLEANUPING: //清除
			{
				g_pWhiteBoardView->m_rectBrush = CRect(CPoint(point.x-40, point.y-40), CPoint(point.x+40, point.y+40));
				DWORD dwDelCount;
				dwDelCount = g_pWBPage->DeleteWBObjects(g_pWhiteBoardView->m_rectBrush);
				if (dwDelCount > 0)
				{
					g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				}
				g_pWhiteBoardView->DeleteObjMsg();
				Invalidate();
			}
			break;
		case DRAW_RGNDELETING: //区域删除
			{
				if (g_pWhiteBoardView->m_pRgnDeleteWBCurve != NULL)
				{
					//wangxiaobin
					UINT nPtNum = g_pWhiteBoardView->m_pRgnDeleteWBCurve->AddPoint(point);

					HDC hDC = ::GetDC(GetSafeHwnd());
					g_pWhiteBoardView->m_pRgnDeleteWBCurve->DrawLast(hDC, 0, 0, nPtNum);
					::ReleaseDC(GetSafeHwnd(),hDC);	
				}
			}
			break;
		case ADDPICTURE:		//从资源库拖拽出图片
			{			
				if (!PathFileExists(g_pWhiteBoardView->GetResFilePath()))
				{
					break;
				}
				//清除所有创建的选择框
				g_pWBPage->ClearSelectFrameList();

				//创建一个新对象
				CWBPicture *pWBPicture = new CWBPicture(g_pWhiteBoardView->GetResFilePath());			
				pWBPicture->SetAlpha(g_nTranspar);
				//控制图片的最大不能超出view的大小4-21金鹏
				CRect viewRect;
				GetClientRect(&viewRect);
				pWBPicture->SetPictureBigerSzie(viewRect);
				g_pWBPage->AddWBObject(pWBPicture);
				CPoint ptCenter,ptOffset;
				//获得对象中心点
				pWBPicture->GetCenterPoint(ptCenter);
				g_pWhiteBoardView->m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
				//创建选择框，并移动对象到当前位置
				if (g_pWBPage->AddSelectFrame(pWBPicture->GetLayerIndex()))
				{
					g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = pWBPicture->GetLayerIndex();
				}
				g_pObj = pWBPicture;
				if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
				{					
					::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBOBJ,0,0);					
				}
				//g_IsSelectObj = TRUE;
				ptOffset = point - ptCenter;
				g_pWBPage->MoveSelectedObj(ptOffset);
				g_pWhiteBoardView->m_ptSelect = point;
				Invalidate();		

				//发送同步消息
				XMemIni xmlObj;
				pWBPicture->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
				g_nDrawStatus = MOVING_LINE;
			}
			break;

		case ADDFLASH:		//从资源库拖拽出Flash
			{
				//if (g_pWhiteBoardView->m_pResource == NULL)
				//{
				//	g_nDrawStatus = DRAW_NORMAL;
				//	break;
				//}
				if (!PathFileExists(g_pWhiteBoardView->GetResFilePath()))
				{
					break;
				}
				//清除所有创建的选择框
				g_pWBPage->ClearSelectFrameList();
				//创建一个新对象
				CWBFlash *pWBFlash = new CWBFlash(g_pWhiteBoardView->GetResFilePath());
				pWBFlash->SetAlpha(g_nTranspar);
				g_pWBPage->AddWBObject(pWBFlash);
				pWBFlash->init();
				//获得对象中心点
				CPoint ptCenter,ptOffset;
				pWBFlash->GetCenterPoint(ptCenter);		
				g_pWhiteBoardView->m_moveFirstPoint = ptCenter;//将移动起始点设为中心点

				//创建选择框，并移动对象到当前位置		
				if (g_pWBPage->AddSelectFrame(pWBFlash->GetLayerIndex()))
				{
					g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = pWBFlash->GetLayerIndex();
				}
				g_pObj = pWBFlash;
				if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
				{					
					::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBOBJ,0,0);					
				}
				ptOffset = point - ptCenter;
				g_pWBPage->MoveSelectedObj(ptOffset);
				g_pWhiteBoardView->m_ptSelect = point;
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pWBFlash->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);

				g_nDrawStatus = MOVING_LINE;
			}
			break;
		case ADDDOC:		//从资源库拖拽出文档
			{
				//if (g_pWhiteBoardView->m_pResource == NULL)
				//{
				//	g_nDrawStatus = DRAW_NORMAL;
				//	break;
				//}
				if (g_pWhiteBoardView->DocConvertTip()) //文档还在转换过程中
				{
					break;
				}

				if (!PathFileExists(g_pWhiteBoardView->GetResFilePath()))
				{
					break;
				}
				//清除所有创建的选择框
				g_pWBPage->ClearSelectFrameList();
				//创建一个新对象
				CWBDocument *pWBDocument = new CWBDocument(g_pWhiteBoardView->GetResFilePath());
				pWBDocument->SetAlpha(g_nTranspar);
				g_pWBPage->AddWBObject(pWBDocument);

				//获得对象中心点
				CPoint ptCenter,ptOffset;
				pWBDocument->GetCenterPoint(ptCenter);		
				g_pWhiteBoardView->m_moveFirstPoint = ptCenter;//将移动起始点设为中心点

				//创建选择框，并移动对象到当前位置		
				if (g_pWBPage->AddSelectFrame(pWBDocument->GetLayerIndex()))
				{
					g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = pWBDocument->GetLayerIndex();
				}
				g_pObj = pWBDocument;
				if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
				{					
					::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBOBJ,0,0);					
				}
				ptOffset = point - ptCenter;
				g_pWBPage->MoveSelectedObj(ptOffset);
				g_pWhiteBoardView->m_ptSelect = point;
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pWBDocument->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);

				g_nDrawStatus = MOVING_LINE;
			}
			break;
		case ENTER_BUTTON:
			{
				CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
				if (pObj == NULL)
				{
					return;
				}
				(pObj->GetImageButton())->ButtonMove(point);
			}
			break;
		case ENTER_CONTROL:
			{
				CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
				if (pObj == NULL)
				{
					return;
				}
				(pObj->GetControlButton())->ButtonMove(point);
			}
			break;
		case CHANGE_ANNOTATIONS:
			{
				CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurAnnotationsLayerIndex);
				if (pObj == NULL) //如果pObj为NULL，则备注可能属于某一文档或图片
				{
					g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
					CWBObject *pObject = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
					if (pObject != NULL)
					{
						if (pObject->IsLock() || pObject->IsLimit())
						{
							break;
						}
						if (pObject->GetType() == WB_OBJ_DOCUMENT)
						{
							pObj = ((CWBDocument *)pObject)->GetAnn();
						}
						if (pObject->GetType() == WB_OBJ_PICTURE)
						{
							pObj = ((CWBPicture *)pObject)->GetAnn();
						}
					}
				}
				if (pObj != NULL)
				{
					CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
					pAnnotations->SetIsChange(TRUE);
					pAnnotations->SetChangePoint(point);
					Invalidate();
				}
			}
			break;
		case DRAWED_TEXT:
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			break;
		default:
			break;
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}


//使用CWhiteBoardView的OnDraw操作
void CScreenDrawDlg::OnPaint()
{
	XAutoLock lock(m_csDraw);
	if (g_systemVersion >= 6)
	{
		if (CScreenDrawMgr::GetIns())
		{
			CRect rect = CScreenDrawMgr::GetIns()->m_pMouseDlg->GetRectSize();
			CScreenDrawMgr::GetIns()->m_pMouseDlg->SetWindowPos(this, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
		}
	}

	CBitmap bmp;
	CDC* pDC=this->GetDC();
	CRect rect;
	GetClientRect(&rect);
	CDC dcMem;
	//以下是输出位图的标准操作
	CBitmap *pOldBitmap = NULL;
	dcMem.CreateCompatibleDC(NULL);
	bmp.CreateCompatibleBitmap(pDC, g_nWidth, g_nHeight);
	pOldBitmap = dcMem.SelectObject(&bmp);
	BITMAP bmpinfo;
	bmp.GetBitmap(&bmpinfo);
	dcMem.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(255, 255, 255));
	//g_pWhiteBoardView->OnPrepareDC(&dcMem);
	if (g_pWBPage && g_pWhiteBoardView->m_pageIsNeedDraw)
	{ 
		g_pWBPage->DrawLabel(&dcMem);
	}

	::BitBlt(dcMem.m_hDC,0, 0, WB_VIEWHSCROLL, WB_VIEWVSCROLL, g_pWBPage->GetLabelMemDC(), 0, 0, SRCCOPY);

	switch(g_nDrawStatus)
	{
	case DRAW_SELECTINT:
		{
			g_pWhiteBoardView->m_pageIsNeedDraw = TRUE;    //放开开关	
			CPen selectPen(PS_DASHDOT, 1, RGB(0, 0, 0));
			CPen *pselectOldPen = dcMem.SelectObject(&selectPen);
			dcMem.SelectStockObject(NULL_BRUSH);	
			dcMem.Rectangle(&g_pWhiteBoardView->m_rectSelect);
			dcMem.SelectObject(pselectOldPen);
			::DeleteObject(selectPen);
		}
		break;
	case DRAWING_TEXT:
		{
			g_pWhiteBoardView->m_pageIsNeedDraw = TRUE;    //放开开关	
			dcMem.SelectStockObject(NULL_BRUSH);	
			dcMem.Rectangle(&g_pWhiteBoardView->m_rectText);
		}
		break;
	case DRAW_ERASING://画橡皮擦
	case DRAW_CLEANUPING:
		{
			CPen brushPen(PS_SOLID, 2, RGB(0, 0, 0));
			CPen *pBrushOldPen = dcMem.SelectObject(&brushPen);
			dcMem.Rectangle(&g_pWhiteBoardView->m_rectBrush);
			dcMem.SelectObject(pBrushOldPen);
			::DeleteObject(brushPen);
		}
		break;
	default:
		{
			g_pWhiteBoardView->m_pageIsNeedDraw = TRUE;    //放开开关	
		}

	}

	::BitBlt(pDC->m_hDC,0,0,bmpinfo.bmWidth,bmpinfo.bmHeight,dcMem.m_hDC,0,0,SRCCOPY);
	dcMem.SelectObject(pOldBitmap);	
	bmp.DeleteObject();
	dcMem.DeleteDC();
	ReleaseDC(pDC);
    CDialog::OnPaint();
}

BOOL CScreenDrawDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_systemVersion >= 6)
	{
		if (CScreenDrawMgr::GetIns())
		{
			CRect rect = CScreenDrawMgr::GetIns()->m_pMouseDlg->GetRectSize();
			CScreenDrawMgr::GetIns()->m_pMouseDlg->SetWindowPos(this, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
		}
	}

	return true;
}

void CScreenDrawDlg::OnDestroy()
{
	//必须先销毁窗口再创建窗口，不然会中断，不明原因。
	bool bIsTranspartentOrNot = WB_IsScreenDrawTrans();
	CRect rect;
	if (!g_bSupportU7OpenFile)
	{
		rect = CScreenDrawMgr::GetIns()->m_pMouseDlg->GetRectSize();
	}

	g_pWhiteBoardView->DestroyScreenDraw();

	if (!g_bSupportU7OpenFile)
	{
		if (!g_pWhiteBoardView->DrawOnScreen())
		{
			return;
		}	
		g_pWhiteBoardView->ShiftGuise(bIsTranspartentOrNot);
		CScreenDrawMgr::GetIns()->m_pMouseDlg->SetWindowPos(this, rect.left - 7, rect.top - 7, rect.Width() + 14, rect.Height() + 14, SWP_SHOWWINDOW);
	}
#ifndef SUPPORTMT

	//获取触摸数据的协议类型
	EProtocolType eType = AVCON_SDK_MTTouch::GetConnectProtocol();
	//电子白板中是否强制使用系统的触摸
	bool bUseSystemTouch = false;
	OSVERSIONINFOEX os; 
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);   
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		//如果是win8系统强制使用系统触摸
		if((os.dwMajorVersion > 6) || ((os.dwMajorVersion == 6) && (os.dwMinorVersion >= 2)))
			bUseSystemTouch = true;
	}
	if (bUseSystemTouch)	eType = PROTOCOL_WINTOUCH_CONNECT;
	if (PROTOCOL_WINTOUCH_CONNECT == eType)	CEncapsulateTouchAPI::U_UnRegisterTouchWindow(GetSafeHwnd());
#endif
	return;
}


void CScreenDrawDlg::touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te)
{
	////转换成客户区坐标
	//XAutoLock lock(g_pWhiteBoardView->m_csDraw);//避免网络同步时崩溃（赵英文20130423）
	CPoint pt;
	pt.x = te.xpos;
	pt.y = te.ypos;
	//CPoint ptWndBackup(pt.x, pt.y);

	//滚动条偏移量
	UINT XnPos = g_pWhiteBoardView->getXnPos();
	UINT YnPos = g_pWhiteBoardView->getYnPos();
	pt.x -= XnPos;
	pt.y -= YnPos;

	ScreenToClient(&pt);
	CClientDC dc(this);
	g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&pt);//将视图坐标转换为文档作标
	m_dwTimeGetTime = ::timeGetTime();

	DWORD dwTouchID = te.id;

	//发送关闭子工具条的消息
	::PostMessage(g_pWhiteBoardView->m_hTOOLBARWnd,WM_CLOSE_TOOBAR_MES,0,0);
	if ((g_pWhiteBoardView->GetWBFunctionDlg() != NULL)&&(g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg() != NULL))
	{
		::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_hWnd,WM_CLOSE_TOOBAR_MES,0,0);
	}
	m_MTDrawObjectTally.touchDownHandler(te.id); //huanglongquan add
	switch (g_nDrawStatus)
	{
	case DRAWANYTRIANGLE:
		{		
			CWBArbitraryTriangle *pTriangle = new CWBArbitraryTriangle(pt,CPoint(pt.x+30,pt.y+100),CPoint(pt.x+100,pt.y+100));
			pTriangle->SetAlpha((int)g_nTranspar);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pTriangle,WB_LAYER_LABEL);			
			g_nDrawStatus = DRAWINGANYTRIANGLE;
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pTriangle, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;
	case DRAW_LINE:   //TouchDown直线
		{
			CWBLine *pLine = new CWBLine(pt, pt);
			if (pLine == NULL)
			{
				break;
			}
			m_ptPointLineCurve = pt;//wangxiabin保存按下点坐标
			pLine->SetAlpha(g_nTranspar);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pLine, WB_LAYER_LABEL);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pLine, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;
	case DRAW_ROUND:		
		break;
	case DRAW_POLYLINE: //TouchDown曲线
		{
			CWBCurve *pCurve = new CWBCurve();
			if (pCurve == NULL)
			{
				break;
			}
			m_ptPointLineCurve = pt;//wangxiabin保存按下点坐标
			pCurve->SetResume_broken_taransfer(true);
			pCurve->SetAlpha(g_nTranspar);
			pCurve->SetLeftTopRightBottomPt(pt, pt);
			pCurve->AddPoint(pt);
			g_pWhiteBoardView->m_topLeft = pt;		
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pCurve, WB_LAYER_LABEL);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pCurve, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;
	case DRAW_POLYICON:
		{
			CWBIcoCurve *pIcoCure = new CWBIcoCurve();
			if (pIcoCure == NULL)
			{
				break;
			}
			pIcoCure->SetResume_broken_taransfer(true);
			pIcoCure->SetAlpha((int)g_nTranspar);
			pIcoCure->AddPoint(pt);
			pIcoCure->SetBeginPoint(pt);
			pIcoCure->SetLeftTopRightBottomPt(pt, pt);

			pIcoCure->SetIsLoadPicFromFile(g_bIsWBLoadIcoFromFile); //设置是否从文件加载
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pIcoCure,WB_LAYER_LABEL);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID,g_nDrawStatus,pIcoCure,pt,te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD,MTDrawPar*>(dwTouchID,drawPar));

		}
		break;
	case DRAW_RECT: //TouchDown矩形
		{
			CWBRectangle *pRect = new CWBRectangle(pt, pt);
			if (pRect == NULL)
			{
				break;
			}
			pRect->SetAlpha(g_nTranspar);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pRect, WB_LAYER_LABEL);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pRect, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;
	case DRAW_PALLELOGAM:
		{
			CWBPallelogam *pPallelogam = new CWBPallelogam(pt,pt);
			if (pPallelogam != NULL)
			{
				pPallelogam->SetAlpha(g_nTranspar);
				g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pPallelogam,WB_LAYER_LABEL);
				PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pPallelogam, pt, te.time);
				m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));

			}
		}
		break;
	case DRAW_TRIANGLE:	//TouchDown画三角形
		{
			CWBTriangle *pTriangle = new CWBTriangle(pt, pt,g_pWhiteBoardView->GetDrawTriangleType());
			if (pTriangle == NULL)
			{
				break;
			}
			pTriangle->SetAlpha(g_nTranspar);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pTriangle, WB_LAYER_LABEL);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pTriangle, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;

	case DRAW_ELLIPSE:  //TouchDown椭圆
		{
			CWBEllipse *pEllipse = new CWBEllipse(pt, pt);
			if (pEllipse == NULL)
			{
				break;
			}
			pEllipse->SetAlpha(g_nTranspar);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pEllipse, WB_LAYER_LABEL);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pEllipse, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;
	case DRAWED_TEXT:
		{
			if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				//SendMessage(UWM_DESTROY_RICHEDIT, 0, 0);
				//PostMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			}

			g_pWhiteBoardView->PostMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			g_nDrawStatus = DRAWEUP_TEXT;
		}
		break;
	case DRAW_TEXT:    //TouchDown文本
		{
			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			g_pWhiteBoardView->m_bIsAnnotations = FALSE;
			g_pWhiteBoardView->m_rectText.left = pt.x;
			g_pWhiteBoardView->m_rectText.top = pt.y;
			if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				g_pWhiteBoardView->SendMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			}
			CWBRichEditMgr::GetInst_()->SetBeginPoint(pt.x, pt.y, te.id);
			g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			g_nDrawStatus = DRAWING_TEXT;
		}
		break;

	case DRAW_POLYGON:	//TouchDown画多边形的第一个点
		{
			CWBPolygon *pPolygonTouch = new CWBPolygon();

			if (pPolygonTouch == NULL)
			{
				break;
			}
		
			pPolygonTouch->SetAlpha(g_nTranspar);
			pPolygonTouch->AddPoint(pt);//添加起始点
			pPolygonTouch->AddPoint(pt);//添加第二点（以便于touchmove时动态改变第二个点时看到改变的状态）
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pPolygonTouch, WB_LAYER_LABEL);

			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pPolygonTouch);
			drawPar->UpdatePt(pt);
			drawPar->UpdateValidPt(pt);
			drawPar->UpdateTime(te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;

	case DRAW_RHOMBUS:	//TouchDown菱形
		{
			CWBRhombus *pRhombus = new CWBRhombus(pt, pt);
			if (pRhombus == NULL)
			{
				break;
			}
			pRhombus->SetAlpha(g_nTranspar);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pRhombus, WB_LAYER_LABEL);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pRhombus, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));	
		}
		break;
	case DRAW_PENTAGON:	//TouchDown五边形
		{
			CWBPentagon *pPentagon = new CWBPentagon(pt, pt);
			if (pPentagon == NULL)
			{
				break;
			}
			pPentagon->SetAlpha((int)g_nTranspar);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pPentagon, WB_LAYER_LABEL);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pPentagon, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));	
		}
		break;
	case DRAW_HEXAGON:	//TouchDown六边形
		{
			CWBHexagon *pHexagon = new CWBHexagon(pt, pt);
			if (pHexagon == NULL)
			{
				break;
			}
			pHexagon->SetAlpha((int)g_nTranspar);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pHexagon, WB_LAYER_LABEL);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pHexagon, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));	
		}
		break;
	case DRAW_FIVEPOINTSTART:
		{
			CWBFivepointstart *pFivepointstart = new CWBFivepointstart(pt, pt);
			if (pFivepointstart == NULL)
			{
				break;
			}
			pFivepointstart->SetAlpha((int)g_nTranspar);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pFivepointstart, WB_LAYER_LABEL);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pFivepointstart, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));	
		}
		break;
	case DRAW_TRAPEZIUM:
		{
			CWBTrapezium *pTrapezium = new CWBTrapezium(pt, pt);
			if (pTrapezium == NULL)
			{
				break;
			}
			pTrapezium->SetAlpha((int)g_nTranspar);
			g_pWhiteBoardView->m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pTrapezium, WB_LAYER_LABEL);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pTrapezium, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));	
		}
		break;
	case DRAW_ERASE: //TouchDown擦除
		g_pWhiteBoardView->m_dwCurBrushTouchID = te.id;
		g_nDrawStatus = DRAW_ERASING;
		g_pWBPage->GetAllObjLayerIndex(g_pWhiteBoardView->m_vecLineLayerIndexBeforeErase, WB_OBJ_LINE);
		break;
	case DRAW_RGNDELETE: //TouchDown区域删除
		{
			if (g_pWhiteBoardView->m_pRgnDeleteWBCurve != NULL)
			{
				delete g_pWhiteBoardView->m_pRgnDeleteWBCurve;
				g_pWhiteBoardView->m_pRgnDeleteWBCurve = NULL;
			}
			PWBCurveItem pWBCurveItem = new WBCurveItem;
			pWBCurveItem->SetWidth(1);
			pWBCurveItem->SetColor(RGB(0, 255, 0));
			g_pWhiteBoardView->m_pRgnDeleteWBCurve = new CWBCurve(pWBCurveItem);
			g_pWhiteBoardView->m_pRgnDeleteWBCurve->AddPoint(pt);

			delete pWBCurveItem;
			g_nDrawStatus = DRAW_RGNDELETING;
		}		
		break;
	case DRAW_CLEANUP: //TouchDown清除
		g_pWhiteBoardView->m_dwCurBrushTouchID = te.id;
		g_nDrawStatus = DRAW_CLEANUPING;
		break;
	case ADD_ANNOTATIONS:	//添加标注
		{
			g_pWhiteBoardView->m_bIsAnnotations = TRUE;
			g_pWhiteBoardView->m_rectText.left = pt.x;
			g_pWhiteBoardView->m_rectText.top = pt.y;

			if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				g_pWhiteBoardView->SendMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			}
			CWBRichEditMgr::GetInst_()->SetBeginPoint(pt.x, pt.y, te.id);
			g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			g_nDrawStatus = DRAWING_TEXT;
		}
		break;
	default:
		break;
	}
	CWBMTObject::touchDownHandler(te);
}

void CScreenDrawDlg::touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te)
{
	////转换成客户区坐标
	//XAutoLock lock(g_pWhiteBoardView->m_csDraw);//避免网络同步时崩溃（赵英文20130423）
	CPoint pt;
	pt.x = te.xpos;
	pt.y = te.ypos;
	//CPoint ptWndBackup(pt.x, pt.y);

	//滚动条偏移量
	UINT XnPos = g_pWhiteBoardView->getXnPos();
	UINT YnPos = g_pWhiteBoardView->getYnPos();
	pt.x -= XnPos;
	pt.y -= YnPos;

	ScreenToClient(&pt);
	CClientDC dc(this);
	g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&pt);//将视图坐标转换为文档作标

	DWORD dwTouchID = te.id;
	DWORD dwTime_difference = ::timeGetTime();
	if ( dwTime_difference - m_dwTimeGetTime >= MOVEISNEEDSENDDATA_TIME)
	{
		//需要发送同步消息
		m_dwTimeGetTime = dwTime_difference;
		m_bMoveIsNeedSendData = TRUE;
	}
	
	//检验判断是否是画图操作(这里不再依赖于全局状态g_nDrawStatus)
	std::map<DWORD, MTDrawPar*>::iterator itDrawPar = m_mapMTDrawPar.find(dwTouchID);
	UINT nNum = m_mapMTDrawPar.size();
	if ( itDrawPar != m_mapMTDrawPar.end() )
	{
		m_MTDrawObjectTally.touchMoveHandler(te.id); //huanglongquan add
		MTDrawPar* pDrawPar = itDrawPar->second;
		int nDrawType = pDrawPar->GetType();
		switch(nDrawType)
		{
		case EDITTRIANGLE:
			{

				CWBArbitraryTriangle *pTriangle = static_cast<CWBArbitraryTriangle *>(pDrawPar->GetWBObject());
				pTriangle->EditTrianglePoint(pt);
				XMemIni xmlObj;					
				pTriangle->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
				g_pWhiteBoardView->Broadcast_Data(xmlObj);
				Invalidate();
			}
			break;
		case DRAWINGANYTRIANGLE:
			{
				CWBArbitraryTriangle *pTriangle = static_cast<CWBArbitraryTriangle *>(pDrawPar->GetWBObject());
				if (pTriangle != NULL)
				{
					pTriangle->Move(pt);
					pTriangle->SetAlpha((int)g_nTranspar);
					if ( m_bMoveIsNeedSendData)
					{

						XMemIni xmlObj;					
						pTriangle->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);

						m_bMoveIsNeedSendData = FALSE;
					}
					Invalidate();
				}
			}
			break;
		case DRAW_LINE:
			{
				CWBLine *pLine = static_cast<CWBLine *>(pDrawPar->GetWBObject());
				if(pLine)
				{
					pLine->Move(pt, FALSE);
					pLine->SetAlpha(g_nTranspar);
					Invalidate();

					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pLine->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
					}
				}
			}
			break;
		case DRAW_POLYLINE:
			{
				CWBCurve *pCurve = static_cast<CWBCurve *>(pDrawPar->GetWBObject());
				if(pCurve)
				{
					UINT nPtNum = pCurve->AddPoint(pt);
					HDC hDC = ::GetDC(GetSafeHwnd());
					pCurve->DrawLast(hDC, 0, 0,nPtNum);
					::ReleaseDC(GetSafeHwnd(),hDC);
					
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						pCurve->SetResume_broken_taransfer(true);
						XMemIni xmlObj;
						pCurve->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_ADDENDUMCURVE);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
					}
				}
			}
			break;
		case DRAW_POLYICON:
			{
				CWBIcoCurve* pIcoCure = static_cast<CWBIcoCurve*>(pDrawPar->GetWBObject());
				if (pIcoCure != NULL)
				{
					pIcoCure->SetAlpha(g_nTranspar);
					CDC * pDC = GetDC();
					UINT x = g_pWhiteBoardView->getXnPos();
					UINT y = g_pWhiteBoardView->getYnPos();
					pIcoCure->DrawIcoTrack(pDC, pt, x, y);
					ReleaseDC(pDC);

					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送曲线同步消息
						pIcoCure->SetResume_broken_taransfer(true);

						XMemIni xmlObj;
						xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_ADDENDUMCURVE);
						pIcoCure->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);

						if(g_bIsWBLoadIcoFromFile)
						{
							xmlObj.Set(KEY_FILE_ID, g_pWhiteBoardView->m_dwIcoCurId);
						}
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
						/*m_bMoveIsNeedSendData = FALSE;*/
					}
				}
			}
			break;
		case DRAW_RECT:
			{
				CWBRectangle *pRect = static_cast<CWBRectangle *>(pDrawPar->GetWBObject());
				if(pRect)
				{
					pRect->Move(pt);
					pRect->SetAlpha(g_nTranspar);
					Invalidate();
				}
				if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
				{
					//发送同步消息
					XMemIni xmlObj;
					pRect->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
				}
			}
			break;
		case DRAW_PALLELOGAM:
			{
				CWBPallelogam *pPallelogam = static_cast<CWBPallelogam*>(pDrawPar->GetWBObject());
				if (pPallelogam)
				{
					pPallelogam->Move(pt);
					pPallelogam->SetAlpha(g_nTranspar);
					Invalidate();
				}
				if (g_pHPWBDataNotify && m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
				{
					XMemIni xmlObj;
					pPallelogam->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
				}
			}
			break;
		case DRAW_TRIANGLE:
			{
				CWBTriangle *pTriangle = static_cast<CWBTriangle *>(pDrawPar->GetWBObject());
				if(pTriangle)
				{
					pTriangle->Move(pt);
					pTriangle->SetAlpha(g_nTranspar);
					Invalidate();
				}
				if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
				{
					//发送同步消息
					XMemIni xmlObj;
					pTriangle->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
				}
			}
			break;
		case  DRAW_RHOMBUS:
			{
				CWBRhombus *pRhombus = static_cast<CWBRhombus *>(pDrawPar->GetWBObject());
				if(pRhombus)
				{
					pRhombus->Move(pt);
					pRhombus->SetAlpha(g_nTranspar);
					Invalidate();
				}
				if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
				{
					//发送同步消息
					XMemIni xmlObj;
					pRhombus->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
				}
			}
			break;
		case  DRAW_PENTAGON:
			{
				CWBPentagon *pPentagon = static_cast<CWBPentagon *>(pDrawPar->GetWBObject());
				if(pPentagon)
				{
					pPentagon->Move(pt);
					pPentagon->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pPentagon->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case  DRAW_HEXAGON:
			{
				CWBHexagon *pHexagon = static_cast<CWBHexagon *>(pDrawPar->GetWBObject());
				if(pHexagon)
				{
					pHexagon->Move(pt);
					pHexagon->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pHexagon->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case  DRAW_FIVEPOINTSTART:
			{
				CWBFivepointstart *pFivepointstart = static_cast<CWBFivepointstart *>(pDrawPar->GetWBObject());
				if(pFivepointstart)
				{
					pFivepointstart->Move(pt);
					pFivepointstart->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pFivepointstart->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case  DRAW_TRAPEZIUM:
			{
				CWBTrapezium *pTrapezium = static_cast<CWBTrapezium *>(pDrawPar->GetWBObject());
				if(pTrapezium)
				{
					pTrapezium->Move(pt);
					pTrapezium->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pTrapezium->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						g_pWhiteBoardView->Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case DRAW_ELLIPSE:
			{
				CWBEllipse *pEllipse = static_cast<CWBEllipse *>(pDrawPar->GetWBObject());
				if(pEllipse)
				{
					pEllipse->Move(pt);
					pEllipse->SetAlpha(g_nTranspar);
					Invalidate();
				}
				if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
				{
					//发送同步消息
					XMemIni xmlObj;
					pEllipse->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
				}
			}
			break;
		case DRAW_POLYGON:
			{
				CWBPolygon *pPolygon = static_cast<CWBPolygon *>(pDrawPar->GetWBObject());
				if(pPolygon)
				{
					CPoint ptPrev = pDrawPar->GetPt();
					pDrawPar->UpdatePt(pt);
					CPoint ptPrevValid = pDrawPar->GetValidPt();
					
					DWORD dwTimePrev = pDrawPar->GetTime();
					DWORD dwTime = te.time;
					DWORD diffTime = labs(dwTime - dwTimePrev);
					pDrawPar->UpdateTime(dwTime);
					if (CalDistancePointToPoint(pt, ptPrevValid) > 10 && CalDistancePointToPoint(pt,ptPrev) < 2 && diffTime>50)
					{
						pPolygon->AddPoint(pt);
						//pPolygon->DeletePoint();
						pDrawPar->UpdateValidPt(pt);
						Invalidate();
					}
					else
					{
						pPolygon->Move(pt);
						pPolygon->SetAlpha(g_nTranspar);
						Invalidate();
					}
					
				}
				else
				{
					ASSERT(0);
				}

				if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
				{
					//发送同步消息
					XMemIni xmlObj;
					pPolygon->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
				}
			}
			break;
		}
	}


	switch (g_nDrawStatus)
	{
	case MOVING_LINE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
			if (pObj->IsClone())
			{
				if (!pObj->IsLock() && !pObj->IsLimit())
				{
					g_pWBPage->ClearSelectFrameList();
					g_pWBPage->AddSelectFrame(pObj->GetLayerIndex());
					g_pWhiteBoardView->CloneObject();
					pObj->SetClone(FALSE);
				}
			}
			else
			{
				CPoint ptOffset;
				ptOffset = pt - g_pWhiteBoardView->m_ptSelect;
				g_pWBPage->MoveSelectedObj(ptOffset);
				g_pWhiteBoardView->m_ptSelect = pt;
			}
			if ( m_bMoveIsNeedSendData)
			{
				//发送同步消息
				XMemIni xmlObj;
				pObj->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
				g_pWhiteBoardView->Broadcast_Data(xmlObj);
				m_bMoveIsNeedSendData = FALSE;
			}
			Invalidate();
			UpdateWindow();
		}
		break;	
	case DRAWING_TEXT:		//画编辑框
		{
			g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			g_pWhiteBoardView->m_rectText.right = pt.x;
			g_pWhiteBoardView->m_rectText.bottom = pt.y;
			Invalidate();
		}
		break;
	case DRAW_ROUND:
		break;
	case DRAW_ERASING: //TOUCHMOVE正在擦除
		{
			if (te.id != g_pWhiteBoardView->m_dwCurBrushTouchID)
			{
				break;
			}
			g_pWhiteBoardView->m_rectBrush = CRect(CPoint(pt.x-g_nBrushSize, pt.y-g_nBrushSize), 
				CPoint(pt.x+g_nBrushSize, pt.y+g_nBrushSize));
			g_pWBPage->EraseObjects(g_pWhiteBoardView->m_rectBrush);
			Invalidate();

			int left,right,top,botton;
			left = pt.x-g_nBrushSize;
			right = pt.x+g_nBrushSize;
			top = pt.y-g_nBrushSize;
			botton = pt.y+g_nBrushSize;

			XMemIni xmlobj;
			xmlobj.Set(KEY_BROADCAST_CMD,_E_BROADE_ERASE_LINE);
			xmlobj.Set(KEY_ERASE_RECT_LEFT,left);
			xmlobj.Set(KEY_ERASE_RECT_RIGHT,right);
			xmlobj.Set(KEY_ERASE_RECT_TOP,top);
			xmlobj.Set(KEY_ERASE_RECT_BOTTON,botton);
			xmlobj.Set(KEY_FILE_SENDER,g_ulSessionID);
			g_pWhiteBoardView->Broadcast_Data(xmlobj);
		}
		break;
	case DRAW_CLEANUPING: //TOUCHMOVE清除
		{
			if (te.id != g_pWhiteBoardView->m_dwCurBrushTouchID)
			{
				break;
			}
			g_pWhiteBoardView->m_rectBrush = CRect(CPoint(pt.x-40, pt.y-40), CPoint(pt.x+40, pt.y+40));
			DWORD dwDelCount;
			dwDelCount = g_pWBPage->DeleteWBObjects(g_pWhiteBoardView->m_rectBrush);
			if (dwDelCount > 0)
			{
				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			}
			g_pWhiteBoardView->DeleteObjMsg();
			Invalidate();
		}
		break;
	case DRAW_RGNDELETING: //TOUCHMOVE区域删除
		{
			if (g_pWhiteBoardView->m_pRgnDeleteWBCurve != NULL)
			{
				UINT nPtNum = g_pWhiteBoardView->m_pRgnDeleteWBCurve->AddPoint(pt);
				HDC hdc = ::GetDC(GetSafeHwnd());
				g_pWhiteBoardView->m_pRgnDeleteWBCurve->DrawLast(hdc,0,0,nPtNum);
				::ReleaseDC(GetSafeHwnd(),hdc);
				
			}
		}
		break;
	case ADDPICTURE:		//TOUCHMOVE从资源库拖拽出图片
		{	
			if (!PathFileExists(g_pWhiteBoardView->GetResFilePath()))
			{
				g_nDrawStatus = DRAW_NORMAL;
				break;
			}

			g_pWBPage->ClearSelectFrameList();
			g_pWhiteBoardView->m_rectSelect = CRect(pt, pt);			
			CWBPicture *pWBPicture = new CWBPicture(g_pWhiteBoardView->GetResFilePath());			
			pWBPicture->SetAlpha(g_nTranspar);
			g_pWBPage->AddWBObject(pWBPicture);
			CPoint ptCenter,ptOffset;
			pWBPicture->GetCenterPoint(ptCenter);						
			g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = g_pWBPage->GetSelectObject(ptCenter);
			g_pWBPage->AddSelectFrame(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);	
			g_pObj = pWBPicture;
			//g_IsSelectObj = TRUE;
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{					
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBOBJ,0,0);					
			}
			ptOffset = pt - ptCenter;
			g_pWBPage->MoveSelectedObj(ptOffset);
			g_pWhiteBoardView->m_ptSelect = pt;
			Invalidate();

			g_nDrawStatus = MOVING_LINE;

			//发送同步信息
			XMemIni xmlObj;
			pWBPicture->SetXmlValue(xmlObj);
			g_pWhiteBoardView->AddObj(xmlObj);
		}
		break;
	case CHANGE_ANNOTATIONS:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurAnnotationsLayerIndex);
			if (pObj == NULL) //如果pObj为NULL，则备注可能属于某一文档或图片
			{
				g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
				CWBObject *pObject = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
				if (pObject != NULL)
				{
					if (pObject->IsLock() || pObject->IsLimit())
					{
						break;
					}
					if (pObject->GetType() == WB_OBJ_DOCUMENT)
					{
						pObj = ((CWBDocument *)pObject)->GetAnn();
					}
					if (pObject->GetType() == WB_OBJ_PICTURE)
					{
						pObj = ((CWBPicture *)pObject)->GetAnn();
					}
				}
			}
			if (pObj != NULL)
			{
				CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
				pAnnotations->SetIsChange(TRUE);
				pAnnotations->SetChangePoint(pt);
				if ( m_bMoveIsNeedSendData)
				{
					//发送同步消息
					XMemIni xmlObj;
					pAnnotations->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					g_pWhiteBoardView->Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
				Invalidate();
			}
		}
		break;
	case DRAWED_TEXT:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;
	default:
		break;
	}
	CWBMTObject::touchMoveHandler(te);
}


void CScreenDrawDlg::touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te)
{
	//XAutoLock lock(g_pWhiteBoardView->m_csDraw);//避免网络同步时崩溃（赵英文20130423）
	////转换成客户区坐标
	CPoint pt;
	pt.x = te.xpos;
	pt.y = te.ypos;
	//CPoint ptWndBackup(pt.x, pt.y);

	//滚动条偏移量
	UINT XnPos = g_pWhiteBoardView->getXnPos();
	UINT YnPos = g_pWhiteBoardView->getYnPos();
	pt.x -= XnPos;
	pt.y -= YnPos;

	ScreenToClient(&pt);
	CClientDC dc(this);
	g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&pt);//将视图坐标转换为文档作标

	DWORD dwTouchID = te.id;
	//检验判断是否是画图操作(这里不再依赖于全局状态g_nDrawStatus)
	std::map<DWORD, MTDrawPar*>::iterator itDrawPar = m_mapMTDrawPar.find(dwTouchID);
	if ( itDrawPar != m_mapMTDrawPar.end() )
	{
		m_MTDrawObjectTally.touchUpHandler(te.id);
		MTDrawPar* pDrawPar = itDrawPar->second;
		int nDrawType = pDrawPar->GetType();
		switch(nDrawType)
		{
		case EDITTRIANGLE:
			{
				CWBArbitraryTriangle *pTriangle = static_cast<CWBArbitraryTriangle *>(pDrawPar->GetWBObject());
				if (pTriangle != NULL)
				{
					pTriangle->EditTrianglePoint(pt,true);	
					pTriangle->AdjustObject();
					//pTriangle->SetIsEdit(false);
					g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
					Invalidate();

					//发送同步消息
					XMemIni xmlObj;
					pTriangle->SetXmlValue(xmlObj);
					g_pWhiteBoardView->ChangeObj(xmlObj);
				}                    
				g_nDrawStatus = DRAW_NORMAL;
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAWINGANYTRIANGLE:
			{
				CWBArbitraryTriangle *pTriangle = static_cast<CWBArbitraryTriangle *>(pDrawPar->GetWBObject());
				if (pTriangle != NULL)
				{
					pTriangle->Move(pt);	
					if (pTriangle->CanbeTriangle())
					{
						pTriangle->CreateSelecrtFrame();
					}
					else
					{
						pTriangle->Move(pt,CPoint(pt.x +30,pt.y+100),CPoint(pt.x+100,pt.y+100));
						pTriangle->CreateSelecrtFrame();
					}
					pTriangle->AdjustObject();

					g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
					Invalidate();

					//发送同步消息
					XMemIni xmlObj;
					pTriangle->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}                               
				g_nDrawStatus = DRAWANYTRIANGLE;	
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_LINE:
			{
				CWBLine *pLine = static_cast<CWBLine *>(pDrawPar->GetWBObject());
				if(pLine)
				{
					if (CalDistancePointToPoint(pt,m_ptPointLineCurve) <5.0)
					{
						pt.x += g_nWBCurObjWidth;
					}
					pLine->Move(pt, FALSE);
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pLine->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_POLYLINE:
			{
				CWBCurve *pCurve = static_cast<CWBCurve *>(pDrawPar->GetWBObject());
				if(pCurve)
				{
					if (CalDistancePointToPoint(pt,m_ptPointLineCurve) <5.0)
					{
						pt.x += g_nWBCurObjWidth;
					}
					pCurve->SetAlpha(g_nTranspar);
					pCurve->AddPoint(pt);				
					pCurve->SetTouchID(0);
					pCurve->CreateSelecrtFrame();
								
					//当所有正在绘制曲线的手指都弹起时，执行刷新，同步操作
					if( 1 == g_pWhiteBoardView->GetMapSizeOfSpecialDrawTypeObj(nDrawType) )
					{
						Invalidate();

					}
					pCurve->SetResume_broken_taransfer(false);
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pCurve->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}
			
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_POLYICON:
			{
				CWBIcoCurve* pIcoCurve = static_cast<CWBIcoCurve*>(pDrawPar->GetWBObject());
				if (pIcoCurve != NULL)
				{
					pIcoCurve->SetAlpha(g_nTranspar);
					pIcoCurve->SetIsRefush(TRUE);
					pIcoCurve->AdjustObject();
					pIcoCurve->CreateSelecrtFrame();
					g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
					Invalidate();
					UpdateWindow();
					//发送同步消息
					pIcoCurve->SetResume_broken_taransfer(false);
					XMemIni xmlObj;
					xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_NONE);

					if(g_bIsWBLoadIcoFromFile) //从文件加载
					{
						xmlObj.Set(KEY_EVENT_ID,g_pWhiteBoardView->m_ulIcoCurEvent);
						xmlObj.Set(KEY_FILE_ID, g_pWhiteBoardView->m_dwIcoCurId);
					}

					pIcoCurve->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
					g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case  DRAW_TRIANGLE:
			{
				CWBTriangle *pTriangle = static_cast<CWBTriangle *>(pDrawPar->GetWBObject());
				if(pTriangle)
				{
					pTriangle->Move(pt);
					pTriangle->AdjustObject();
					Invalidate();

					//发送同步信息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pTriangle->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_RECT:
			{
				CWBRectangle *pRect = static_cast<CWBRectangle *>(pDrawPar->GetWBObject());
				if(pRect)
				{
					pRect->Move(pt);
					pRect->AdjustObject();
					pRect->SetTouchID(0);
					Invalidate();
					//发送同步信息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pRect->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_PALLELOGAM:
			{
				CWBPallelogam *pPallelogam = static_cast<CWBPallelogam *>(pDrawPar->GetWBObject());
				if(pPallelogam)
				{
					pPallelogam->Move(pt);
					pPallelogam->AdjustObject();
					pPallelogam->SetTouchID(0);
					Invalidate();
					//发送同步信息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pPallelogam->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_ELLIPSE:
			{
				CWBEllipse *pEllipse = static_cast<CWBEllipse *>(pDrawPar->GetWBObject());
				if(pEllipse)
				{
					pEllipse->Move(pt);	
					pEllipse->AdjustObject();
					pEllipse->SetTouchID(0);
					Invalidate();

					//发送同步信息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pEllipse->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_RHOMBUS:
			{
				CWBRhombus *pRhombus = static_cast<CWBRhombus *>(pDrawPar->GetWBObject());
				if(pRhombus)
				{
					pRhombus->Move(pt);
					pRhombus->AdjustObject();
					pRhombus->SetTouchID(0);
					Invalidate();

					//发送同步信息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pRhombus->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_PENTAGON:
			{
				CWBPentagon *pPentagon = static_cast<CWBPentagon *>(pDrawPar->GetWBObject());
				if(pPentagon)
				{
					pPentagon->Move(pt);
					pPentagon->AdjustObject();
					pPentagon->SetTouchID(0);

					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pPentagon->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_HEXAGON:
			{
				CWBHexagon *pHexagon = static_cast<CWBHexagon *>(pDrawPar->GetWBObject());
				if(pHexagon)
				{
					pHexagon->Move(pt);
					pHexagon->AdjustObject();
					pHexagon->SetTouchID(0);

					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pHexagon->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_FIVEPOINTSTART:
			{
				CWBFivepointstart *pFivepointstart = static_cast<CWBFivepointstart *>(pDrawPar->GetWBObject());
				if(pFivepointstart)
				{
					pFivepointstart->Move(pt);
					pFivepointstart->AdjustObject();
					pFivepointstart->SetTouchID(0);

					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pFivepointstart->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_TRAPEZIUM:
			{
				CWBTrapezium *pTrapezium = static_cast<CWBTrapezium *>(pDrawPar->GetWBObject());
				if(pTrapezium)
				{
					pTrapezium->Move(pt);
					pTrapezium->AdjustObject();
					pTrapezium->SetTouchID(0);

					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pTrapezium->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_POLYGON:
			{
				CWBPolygon *pPolygon = static_cast<CWBPolygon *>(pDrawPar->GetWBObject());
				if(pPolygon)
				{
					//如果点在起始点附近，直接close，否则，添加一个点，然后close
					pPolygon->SetClosed();
					Invalidate();

					//发送同步信息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pPolygon->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
	   }
	}
	switch (g_nDrawStatus)
	{
	case MOVING_LINE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
			if (pObj != NULL)
			{
				pObj->SetClone(FALSE);
			}

			//denghong1227移动直线偏移量
			CPoint moveOffset = pt - g_pWhiteBoardView->m_moveFirstPoint;
			g_pWhiteBoardView->MoveMsg(moveOffset, GenerateSSRC());

			g_nDrawStatus = DRAW_NORMAL;

		}
		break;
	case DRAW_NORMAL:
		break;
	case DRAW_ROUND:
		//m_draw = DRAW_NORMAL;
		break;
	case DRAWEUP_TEXT:
		{
			Invalidate();
			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	case DRAWING_TEXT:
		{				
			if (!CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				unsigned long nTouchID = te.id;
				g_pWhiteBoardView->PostMessage( UWM_CREATE_RICHEDIT, (WPARAM)(nTouchID), (LPARAM)(MAKELONG(pt.x, pt.y)) );
			}
			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			g_nDrawStatus = DRAWED_TEXT;
			Invalidate();
		}
		break;
	case DRAW_ERASING: //擦除
		{
			if (te.id != g_pWhiteBoardView->m_dwCurBrushTouchID)
			{
				break;
			}
			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			g_nDrawStatus = DRAW_ERASE;
			Invalidate();

			//denghong1227
			//发送同步信息

			//设置发送事件ID
			g_pWhiteBoardView->m_ulSendEventID = GenerateSSRC();

			XMemIni xmlobj;
			xmlobj.Set(KEY_BROADCAST_CMD,_E_BROAD_ERASE_UP);
			xmlobj.Set(KEY_FILE_SENDER,g_ulSessionID);
			g_pWhiteBoardView->Broadcast_Data(xmlobj);

			g_pWhiteBoardView->EraseLineMsg(); //擦除直线协同操作
			for (int i = 0; i < g_pWhiteBoardView->m_vecLineLayerIndexBeforeErase.size(); ++i )
			{
				if (g_pWhiteBoardView->m_vecLineLayerIndexBeforeErase[i] != NULL)
				{
					delete g_pWhiteBoardView->m_vecLineLayerIndexBeforeErase[i];
				}
			}
			g_pWhiteBoardView->m_vecLineLayerIndexBeforeErase.clear();
			g_pWhiteBoardView->EraseCurveMsg();//擦除曲线协同操作
		}
		break;
	case DRAW_CLEANUPING:
		{
// 			if (te.id != g_pWhiteBoardView->m_dwCurBrushTouchID)
// 			{
// 				break;
// 			}
// 
// 			//发送同步信息
// 			g_pWhiteBoardView->DeleteObjMsg();
// 			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
// 			Invalidate();
			g_nDrawStatus = DRAW_CLEANUP;				
		}			
		break;
	case DRAW_RGNDELETING: //TOUCHUP区域删除
		{
			if (g_pWhiteBoardView->m_pRgnDeleteWBCurve != NULL)
			{
				g_pWhiteBoardView->m_pRgnDeleteWBCurve->AddPoint(pt);

				CRgn *pRgn = new CRgn();
				if (g_pWhiteBoardView->m_pRgnDeleteWBCurve->CreateRgn(pRgn))
				{
					g_pWBPage->DeleteWBObjects(pRgn);
				}
				delete pRgn;

				delete g_pWhiteBoardView->m_pRgnDeleteWBCurve;
				g_pWhiteBoardView->m_pRgnDeleteWBCurve = NULL;

				//发送同步信息
				g_pWhiteBoardView->DeleteObjMsg();

				Invalidate();
				
			}
			g_nDrawStatus = DRAW_RGNDELETE;
		}
		break;
	case CHANGE_ANNOTATIONS:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurAnnotationsLayerIndex);
			if (pObj == NULL) //如果pObj为NULL，则备注可能属于某一文档
			{
				g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
				CWBObject *pDoc = g_pWBPage->FindByLayerIndex(g_pWhiteBoardView->m_dwCurSelectObjLayerIndex);
				if (pDoc != NULL)
				{
					if (pDoc->GetType() == WB_OBJ_DOCUMENT)
					{
						pObj = pDoc;
					}
				}
			}
			if (pObj != NULL)
			{
				unsigned long ulEventID = 0; //事件ID
				//发送同步消息
				XMemIni xmlObj;
				pObj->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
				Invalidate();
			}
			g_nDrawStatus = DRAW_NORMAL;
			
		}
		break;
	default:
		break;
	}
	CWBMTObject::touchUpHandler(te);
}

void CScreenDrawDlg::SetMoveIsNeedSendData( BOOL MoveIsNeedSendData )
{
	m_bMoveIsNeedSendData = MoveIsNeedSendData;
}

BOOL CScreenDrawDlg::GetMoveIsNeedSendData()
{
	return m_bMoveIsNeedSendData;
}

afx_msg LRESULT CScreenDrawDlg::OnDestroyScreen(WPARAM wParam, LPARAM lParam)
{
	CDialog::OnDestroy();

	//if (g_pScreenDlg != NULL)
	//{
	//	delete g_pScreenDlg;
	//	g_pScreenDlg=NULL;
	//}
	g_bIsDrawScreen=FALSE;

	return 0;
}

void CScreenDrawDlg::ScreenClearTouchMap(DWORD dwLayerIndex)
{
	std::map<DWORD, MTDrawPar*>::iterator itDrawPar = m_mapMTDrawPar.begin();
	while( itDrawPar != m_mapMTDrawPar.end() )
	{
		MTDrawPar* pDrawPar = itDrawPar->second;
		int nDrawType = pDrawPar->GetType();
		CWBObject* pObj = static_cast<CWBLine *>(pDrawPar->GetWBObject());
		if (pObj->GetLayerIndex() == dwLayerIndex && nDrawType == DRAW_POLYLINE)
		{
			delete pDrawPar;
			m_mapMTDrawPar.erase(itDrawPar);
			break;
		}
		++itDrawPar ;
	}
}


void CScreenDrawDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_bAvconLock) 
		return;
	UINT XnPos = g_pWhiteBoardView->getXnPos();
	UINT YnPos = g_pWhiteBoardView->getYnPos();
	point.x -= XnPos;
	point.y -= YnPos;
	CClientDC dc(this);
	g_pWhiteBoardView->OnPrepareDC(&dc);
	dc.DPtoLP(&point);
	int nTemp = g_pWBPage->PointInSelectFrame(point);

	DWORD nGetWBObjID = g_pWBPage->GetCurEditingObjLayerIndex();

	if (nTemp == WB_PS_SELECTFRAME)	//如果点在某个选择框内
	{
		CWBObject *pObj = g_pWBPage->FindByLayerIndex(nGetWBObjID);
		if (pObj != NULL)
		{
			if (pObj->GetType() == WB_OBJ_TEXT || pObj->GetType() == WB_OBJ_ANNOTATIONS)
			{
				g_pWhiteBoardView->m_dwMenuLayerIndex = nGetWBObjID;
				if (!pObj->IsLock() && !pObj->IsLimit() && !pObj->IsInfiniteClone())
				{
					g_pWhiteBoardView->m_bIsEditing = TRUE;
					g_pWhiteBoardView->m_pEditObj = pObj;
					if (pObj->GetType() == WB_OBJ_ANNOTATIONS)
					{
						g_pWhiteBoardView->m_bIsAnnotations = TRUE;
					}
					else if (pObj->GetType() == WB_OBJ_TEXT)
					{
						g_pWhiteBoardView->m_bIsAnnotations = FALSE;
					}
					g_pWBPage->DeleteSelectFrame(g_pWhiteBoardView->m_dwMenuLayerIndex);
					g_pWhiteBoardView->setFrameListAndObjLayerIndex();

					CRect rect = g_pWhiteBoardView->GetEditTextRect();
					if (!CWBRichEditMgr::GetInst_()->IsRichEditShow())
					{
						g_pWhiteBoardView->SendMessage( UWM_EDIT_RICHEDIT, (WPARAM)(MAKELONG(rect.left, rect.top)), (LPARAM)(MAKELONG(rect.right, rect.bottom)) );
						g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
						Invalidate();
					}

					g_pWBPage->DelWBObj(g_pWhiteBoardView->m_dwMenuLayerIndex);
					//g_pWhiteBoardView->RemoveObj(g_pWhiteBoardView->m_dwMenuLayerIndex);

					if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
					{
						CScreenDrawMgr::GetIns()->Invalidate();
					}
					else
					{
						Invalidate();
					}

					if (g_pWhiteBoardView->m_bIsAnnotations)
					{
						g_nDrawStatus = ADD_ANNOTATIONS;
					} 
					else
					{
						g_nDrawStatus = DRAW_TEXT;
					}
					g_nDrawStatus = DRAWED_TEXT;

					DRAW_STATE nDrawStatus = g_nDrawStatus;
					g_nDrawStatus = DRAW_TEXT;
					if (NULL != g_pWhiteBoardView->m_pWBFunctionDlg)
					{
						if (::IsWindow(g_pWhiteBoardView->m_pWBFunctionDlg->GetSafeHwnd()))
						{
							::SendMessage(g_pWhiteBoardView->m_pWBFunctionDlg->m_hWnd,WM_SELECTWBPEN,0,0);
						}
					}
					g_nDrawStatus = nDrawStatus;

				}
			}
			if (pObj->GetType() == WB_OBJ_ARBITRARYTRIANGLE)
			{
				
				pObj->SetSelectFrameShow(FALSE);
				((CWBArbitraryTriangle*)pObj)->SetIsEdit(true);
				g_nDrawStatus = DRAW_NORMAL;
				Invalidate();
			}
		}
	}
	__super::OnLButtonDblClk(nFlags, point);
}

/*************************************************************************
*                    CScreenDrawDlg::OnGesture							 *
*                         屏蔽系统手势处理								*
*************************************************************************/
LRESULT CScreenDrawDlg::OnGesture( WPARAM /*wParam*/, LPARAM lParam )
{
	return S_OK;
}


LRESULT CScreenDrawDlg::OnTouch( WPARAM wParam, LPARAM lParam )
{
#ifndef SUPPORTMT

	if (g_bAvconLock) 
	{
		if (!CEncapsulateTouchAPI::U_CloseTouchInputHandle(lParam))
		{
			OutputDebugString(_T("warning, the touch input handle was not closed successfully!"));
		}
		return S_OK;
	}

	int iNumContacts = LOWORD(wParam);
	HTOUCHINPUT hInput = (HTOUCHINPUT)lParam;

#ifdef _DEBUG
	PTOUCHINPUT pInputs = new TOUCHINPUT[iNumContacts];
#else
	PTOUCHINPUT pInputs = new (std::nothrow) TOUCHINPUT[iNumContacts];
#endif


	if(pInputs != NULL)
	{
		//获取每个触摸点信息，然后分发到各个处理容器中。
		//if(GetTouchInputInfo(hInput, iNumContacts, pInputs, sizeof(TOUCHINPUT)))
		if( CEncapsulateTouchAPI::U_GetTouchInputInfo(lParam, iNumContacts, pInputs, sizeof(TOUCHINPUT)) )
		{
			for(int i = 0; i < iNumContacts; ++i)
			{
				//////转换成客户区坐标
				POINT ptInputs;

				ptInputs.x = TOUCH_COORD_TO_PIXEL(pInputs[i].x);
				ptInputs.y = TOUCH_COORD_TO_PIXEL(pInputs[i].y);

				//ClientToScreen(&ptInputs);
				// 				CClientDC dc(this);
				//				OnPrepareDC(&dc);//进行坐标原点的匹配
				// 				dc.DPtoLP(&ptInputs);//将视图坐标转换为文档作标
				pInputs[i].x = ptInputs.x;
				pInputs[i].y = ptInputs.y;

				if ((pInputs[i].dwFlags & TOUCHEVENTF_DOWN) == TOUCHEVENTF_DOWN)// 触摸按下事件
				{
					if (!g_bAvconLock) //如果会议中白板功能没有被锁定
					{
						std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
						if (m_mapTouchCursor.end() != it)
						{
							delete it->second;
							m_mapTouchCursor.erase(it);
						}
						static long session_id = 1;
						MTCursor *pCursor = new MTCursor(MTTime::getSessionTime(),++session_id, pInputs[i].dwID, (float)pInputs[i].x, (float)pInputs[i].y);
						m_mapTouchCursor.insert(std::pair<DWORD, MTCursor*>(pInputs[i].dwID, pCursor));
						CWBMTObjectManager::GetIns_()->addMTCursor(pCursor);
					}
				}               
				else if ((pInputs[i].dwFlags & TOUCHEVENTF_MOVE) == TOUCHEVENTF_MOVE)// 触摸移动事件
				{
					std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
					if (m_mapTouchCursor.end() != it)
					{
						it->second->update(MTTime::getSessionTime(),(float)pInputs[i].x, (float)pInputs[i].y);
						CWBMTObjectManager::GetIns_()->updateMTCursor(it->second); 
					}
				}               
				else if ((pInputs[i].dwFlags & TOUCHEVENTF_UP) == TOUCHEVENTF_UP)// 触摸弹起事件
				{
					std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
					if (m_mapTouchCursor.end() != it)
					{
						it->second->update(MTTime::getSessionTime(),(float)pInputs[i].x, (float)pInputs[i].y);
						CWBMTObjectManager::GetIns_()->removeMTCursor(it->second);
						delete it->second;
						m_mapTouchCursor.erase(it);
					}
				}
			}
		}
		if (!CEncapsulateTouchAPI::U_CloseTouchInputHandle(lParam))
		{
			OutputDebugString(_T("warning, the touch input handle was not closed successfully!"));
		}
	}

	delete [] pInputs;

#endif
	return S_OK;
}

int CScreenDrawDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
#ifndef SUPPORTMT

	// TODO:  在此添加您专用的创建代码
	EProtocolType eType = AVCON_SDK_MTTouch::GetConnectProtocol();
	//电子白板中是否强制使用系统的触摸
	bool bUseSystemTouch = false;
	OSVERSIONINFOEX os; 
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);   
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		//如果是win8系统强制使用系统触摸
		if((os.dwMajorVersion > 6) || ((os.dwMajorVersion == 6) && (os.dwMinorVersion >= 2)))
			bUseSystemTouch = true;
	}
	if (bUseSystemTouch)	eType = PROTOCOL_WINTOUCH_CONNECT;
	if (PROTOCOL_WINTOUCH_CONNECT == eType)	CEncapsulateTouchAPI::U_RegisterTouchWindow(GetSafeHwnd());
#endif
	return 0;
}
