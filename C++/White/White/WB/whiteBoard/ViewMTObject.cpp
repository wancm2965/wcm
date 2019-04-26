#include "stdafx.h"
#include "ViewMTObject.h"
#include "WhiteBoardView.h"
#include "../WB/whiteBoard/common/WBMedia/WBGlobalMediaCtrl.h"
#include "ScreenDrawMgr.h"
#include "TouchAPI.h"
#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define VIRTUALWND_CLASS_NAME TEXT("VirtualWindowClass")
#define UWM_RECEIVE_TOUCH_MSG (WM_USER+ 4444)
#define UWM_TOUCH_DOWN		(WM_USER + 11)
#define UWM_TOUCH_UPDATE	(WM_USER + 12)
#define UWM_TOUCH_UP		(WM_USER + 13)

///按钮类型
enum touchState{
	BUTTON_TYPE_NONE,				///无类型
	BUTTON_TYPE_IMAGEBUTTON,		///按钮类型为图片按钮
	BUTTON_TYPE_CTRLBUTTON,			///按钮类型为控制按钮
	BUTTON_TYPE_INMEDIACTRL			///按钮类型为视频播放器上的按钮
};

using namespace  AVCON_SDK_MTTouch;
CWBMTObjectManager::CWBMTObjectManager(HWND hViewWnd, CWBMTObject * pViewMTObject)
	: m_pClickObject(NULL)
	, m_pFrameMoveObject(NULL)
	, m_pViewMTObject(pViewMTObject)
	, m_pObjectMap(NULL)
	, m_hViewWnd(hViewWnd)
	, m_bMTAuthorize(FALSE)
	,m_bIsTouchDustbin(FALSE)
	,m_bMTUserCursor(FALSE)
{
	m_pViewMTObject = pViewMTObject;

	m_pFrameMoveObject = new CWBMTObjectFramePan(hViewWnd);
	m_pClickObject = new CWBMTObjectButtonManager(hViewWnd);
	m_nFirstID = 0;
	m_nEditTriangleTouchID = 0;
	m_pObjectMap = new std::map<unsigned long, CWBMTObject*>;
}

CWBMTObjectManager::~CWBMTObjectManager()
{
#ifdef SUPPORTMTGES
	return;
#else
	bool bOK = AVCON_SDK_MTTouch::RemoveMTObject(this);
	if (NULL != m_pObjectMap)
	{
		m_pObjectMap->clear();
		delete m_pObjectMap;
		m_pObjectMap = NULL;
	}
	if (NULL != m_pFrameMoveObject)
	{
		delete m_pFrameMoveObject;
		m_pFrameMoveObject = NULL;
	}
	if (NULL != m_pClickObject)
	{
		delete m_pClickObject;
		m_pClickObject = NULL;
	}
	m_pViewMTObject = NULL;
#endif
}

void CWBMTObjectManager::touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te)
{
#ifdef SUPPORTMTGES
	return;
#else
	if(!CScreenDrawMgr::GetIns())
	{
		CWBDustbin *pWBDustbin = g_pWhiteBoardView->GetDustbin();
		CPoint pt((int)te.xpos, (int)te.ypos);
		::ScreenToClient(m_hViewWnd, &pt);
		CClientDC dc(g_pWhiteBoardView);
		g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
		dc.DPtoLP(&pt);//将视图坐标转换为文档作标
		if (pWBDustbin)
		{
			if (pWBDustbin->PointIn(pt))
			{
				m_ptPreMoveDustbin = pt;
				m_bIsTouchDustbin = TRUE;
			}
		}
		
		if (DRAW_NORMAL == g_nDrawStatus)
		{
			///先清除所有的选择框并重画
			g_pWBPage->ClearSelectFrameList();
			/*g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);*/
			//发送关闭子工具条的消息
			CWBToolbarDlg *  pWbToolBarDlg = ((CWhiteBoardDlg *)g_whiteboardDlg)->GetToolbarDlg();
			if(NULL != pWbToolBarDlg)
			{
				::PostMessage(pWbToolBarDlg->m_hWnd,WM_CLOSE_TOOBAR_MES,0,0);	
			}
				
			g_pWhiteBoardView->Invalidate();
			//g_pWhiteBoardView->UpdateWindow();
			//g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			m_rectSelect = CRect(pt,pt);
			g_pWhiteBoardView->SetRectSelect(m_rectSelect);
			g_nDrawStatus = DRAW_SELECTINT;
		}
		
	}
	else
	{
		///先清除所有的选择框并重画
		g_pWBPage->ClearSelectFrameList();
		//发送关闭子工具条的消息
		CWBToolbarDlg *  pWbToolBarDlg = ((CWhiteBoardDlg *)g_whiteboardDlg)->GetToolbarDlg();
		if(NULL != pWbToolBarDlg)
		{
			::PostMessage(pWbToolBarDlg->m_hWnd,WM_CLOSE_TOOBAR_MES,0,0);
		}	
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	CWBMTObject::touchDownHandler(te);
#endif
}

void CWBMTObjectManager::touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te)
{
#ifdef SUPPORTMTGES
	return;
#else
	if (!CScreenDrawMgr::GetIns())
	{
		///如果触摸点在垃圾箱上则移动垃圾箱,不进行框选
		CWBDustbin *pWBDustbin = g_pWhiteBoardView->GetDustbin();
		CPoint pt((int)te.xpos, (int)te.ypos);
		::ScreenToClient(m_hViewWnd, &pt);
		CClientDC dc(g_pWhiteBoardView);
		g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
		dc.DPtoLP(&pt);//将视图坐标转换为文档作标
		if (pWBDustbin)
		{
			
			if (pWBDustbin->PointIn(pt))
			{
				if (m_bIsTouchDustbin == FALSE)
				{
					return;
				}
				const CPoint offset(pt.x - m_ptPreMoveDustbin.x, pt.y - m_ptPreMoveDustbin.y);
				pWBDustbin->MoveOffset(offset);
				m_ptPreMoveDustbin = pt;
				g_pWhiteBoardView->Invalidate();
				return;
			}
		}
		
		if (DRAW_SELECTINT == g_nDrawStatus)
		{
			g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
				m_rectSelect.right = pt.x;
			m_rectSelect.bottom = pt.y;

			///将触摸获得的选择矩形传给view
			g_pWhiteBoardView->SetRectSelect(m_rectSelect);

			g_pWhiteBoardView->Invalidate();
		}
	}
	CWBMTObject::touchMoveHandler(te);
#endif
}

void CWBMTObjectManager::touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te)
{
#ifdef SUPPORTMTGES
	return;
#else
	if (!CScreenDrawMgr::GetIns())
	{
		CWBDustbin *pWBDustbin = g_pWhiteBoardView->GetDustbin();
		CPoint pt((int)te.xpos, (int)te.ypos);
		::ScreenToClient(m_hViewWnd, &pt);
		if (pWBDustbin)
		{
			m_ptPreMoveDustbin = pt;
			m_bIsTouchDustbin = FALSE;
		}
		
		if (DRAW_SELECTINT == g_nDrawStatus)
		{
			//g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			//调整画出的选择框矩形
			int nTemp;
			if (m_rectSelect.left > m_rectSelect.right)
			{
				nTemp = m_rectSelect.left;
				m_rectSelect.left = m_rectSelect.right;
				m_rectSelect.right = nTemp;
			}
			if (m_rectSelect.bottom < m_rectSelect.top)
			{
				nTemp = m_rectSelect.top;
				m_rectSelect.top = m_rectSelect.bottom;
				m_rectSelect.bottom = nTemp;
			}

			///将触摸获得的选择矩形传给view
			g_pWhiteBoardView->SetRectSelect(m_rectSelect);

			CRect rect;
			g_pWhiteBoardView->GetClientRect(&rect);
			if (m_rectSelect.left >= rect.left || m_rectSelect.bottom >= rect.bottom)
			{
				if (g_pWBPage->AddSelectFrames(m_rectSelect) > 0)
				{
					g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
				}
			}	
			
			g_nDrawStatus = DRAW_NORMAL;
			//解决BUG#31797，触控框选选中对象后无法剪切，删除，组合等操作
			if (g_pHPWBDataNotify && g_bSupportI7)
			{
				int nSelectCpunt = 0;
				if(g_pWBPage != NULL)
				{			
					nSelectCpunt = g_pWBPage->GetWBSelectFrameList().size();
				}
				if (nSelectCpunt > 0)
				{
					MSG msg;
					memset(&msg, 0, sizeof(MSG));
					msg.hwnd = NULL;
					CObList * ptemplist = g_pWBPage->GetSelectObjLst();
					POSITION pos;
					for (int i = 0; i < nSelectCpunt; ++i)//遍历选中对象列表
					{
						pos = ptemplist->FindIndex(i);
						if (pos == NULL)
							continue;
						CWBObject* pObject = (CWBObject*)ptemplist->GetAt(pos);
						if (pObject->IsLock() || pObject->IsLimit())
						{
							msg.lParam = (LPARAM)1;
						}
						else
							msg.lParam = (LPARAM)0;
					}
					msg.message = 1;
					msg.wParam = (WPARAM)nSelectCpunt;
					g_pHPWBDataNotify->SetSelectObjStatue(msg);
				}					
			}
			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			g_pWhiteBoardView->Invalidate();
			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		}
	}
	CWBMTObject::touchUpHandler(te);
#endif
}

bool CWBMTObjectManager::BInRegion(const POINT &pt)const
{
#ifdef SUPPORTMTGES
	return false;
#else
	///触摸时获得的窗口句柄
	HWND hwnd = WindowFromPoint(pt);
	if (!hwnd)
	{
		return false;
	}

	///若开启了屏幕绘制，并且触摸点在该窗口上则交给屏幕绘制处理
	if (CScreenDrawMgr::GetIns())
	{
		if (CScreenDrawMgr::GetIns()->PtInScreenDrawDlg(pt))
		{
			return true;
		}
	}
	///因为当触摸点击到视频播放器窗口时，所得到的窗口句柄为view窗口的子窗口,所以需要判断
	if (hwnd == m_hViewWnd /*|| ::GetParent( hwnd ) == m_hViewWnd*/ )
	{
		RECT rect;
		::GetClientRect(m_hViewWnd, &rect );
		POINT ptTemp;
		ptTemp.x = pt.x;
		ptTemp.y = pt.y;
		::ScreenToClient(m_hViewWnd, &ptTemp);
		if(::PtInRect(&rect, ptTemp))
		{
			return true;
		}
	}

	return false;
#endif
}


void CWBMTObjectManager::OnCallbackGesture( const unsigned int &nMessage, const unsigned int &nAppendMessage /*= 0*/ )
{
#ifdef SUPPORTMTGES
	return;
#else
	WORD lWord = LOWORD(nMessage);
	if (TGT_DIRECTION == lWord)
	{
		if( (MTOG_RIGHT == nAppendMessage)
			|| (MTOG_DOWN == nAppendMessage) )
		{
			g_pWhiteBoardView->NextPage();
		}
		else if( (MTOG_LEFT == nAppendMessage)
			|| (MTOG_UP == nAppendMessage) )
		{
			g_pWhiteBoardView->PrePage();
		}
	}
	if (AVCON_SDK_MTTouch::TGT_ACLICK == lWord)
	{
		WORD hWord = HIWORD(nMessage);
		if(TGC_CLICK == hWord || TGC_TAP == hWord)
		{
			g_pWBPage->ClearSelectFrameList();
			g_pWhiteBoardView->Invalidate();
		}
	}
#endif
}

bool CWBMTObjectManager::AddObjectMap( const unsigned long &dID, CWBMTObject *pObject )
{
#ifdef SUPPORTMTGES
	return false;
#else
	std::map<unsigned long, CWBMTObject*>::iterator it = m_pObjectMap->find(dID);
	if (m_pObjectMap->end() == it)
	{
		if (pObject == m_pClickObject)
		{
			for (std::map<unsigned long, CWBMTObject*>::iterator itInner = m_pObjectMap->begin();
				m_pObjectMap->end() != itInner; ++itInner)
			{
				if (itInner->second == m_pClickObject)
				{
					//m_pObjectMap->erase(itInner);
					return false;
				}
			}
		}

		m_pObjectMap->insert(std::pair<unsigned long, CWBMTObject*>(dID, pObject));
		return true;
	}
	else
	{
#ifdef _DEBUG
		assert(false);
#endif
		m_pObjectMap->erase(it);
		return false;
	}
#endif
}

bool CWBMTObjectManager::RemoveObjectMap( const unsigned long &dID )
{
#ifdef SUPPORTMTGES
	return false;
#else
	std::map<unsigned long, CWBMTObject*>::const_iterator it = m_pObjectMap->find(dID);
	if (m_pObjectMap->end() != it)
	{
		m_pObjectMap->erase(it);
		return true;
	}
	else
	{
		return false;
	}
#endif
}

CWBMTObjectManager * CWBMTObjectManager::GetIns_()
{
	if (NULL == m_instance)
	{
#ifdef _DEBUG
		assert(false);//没有初始化
#endif
	}
	return m_instance;
}


HRESULT CWBMTObjectManager::Init(HWND hViewWnd, CWBMTObject * pViewMTObject )
{
#ifdef SUPPORTMTGES
	return S_OK;
#else
	if(NULL != m_instance)
	{
		return S_OK;
	}
	m_instance = new CWBMTObjectManager(hViewWnd, pViewMTObject);
	m_instance->SetMTAuthorize(g_bIsSupportMTTouch);//设置白板授权
#ifndef SUPPORTMT
	//m_instance->AddGesture(AVCON_SDK_MTTouch::TGT_DIRECTION);//添加手势支持
#endif
//	
	return S_OK;
#endif
}

void CWBMTObjectManager::Destroy()
{
#ifdef SUPPORTMTGES
	return;
#else
	if(m_bMTAuthorize)
	{
		switch (m_gType)
		{
		case  PROTOCOL_WINTOUCH_CONNECT:
			{
				CEncapsulateTouchAPI::U_UnRegisterTouchWindow(m_hViewWnd);
			}
			break;
		case  PROTOCOL_TUIO_CONNECT:
		case  PROTOCOL_HID_CONNECT:
			{
				AVCON_SDK_MTTouch::RemoveMTObject(m_instance);
			}
			break;
		default:
			{
				assert(false);
			}
			break;

		}
	}
	if (NULL != m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
#endif
}

bool CWBMTObjectManager::RemoveMTObject( const CMTObject *pObject )
{
#ifdef SUPPORTMTGES
	return false;
#else
#ifdef _DEBUG
	assert(NULL != pObject);
#endif
	//XAutoLock lock(m_csMTObjectOperation);//白板调用函数由白板控制线程同步//避免网络同步时崩溃（赵英文20130423）
	bool bReturnValue = false;

	while (true)
	{
		bool bRemoveOK = false;
		std::map<unsigned long, CWBMTObject*>::iterator it = m_pObjectMap->begin();
		for (; m_pObjectMap->end() != it; ++it)
		{
			if (pObject == it->second)
			{
				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->ClearMTDrawMap(it->first);
				}
				m_pFrameMoveObject->ClearMTDrawMap(it->first);

				m_pObjectMap->erase(it);
				bRemoveOK = true;
			}
		}
		if (bRemoveOK)
		{
			bReturnValue = (bReturnValue || bRemoveOK);
		}
		else
		{
			break;
		}
	}

	return bReturnValue;
#endif
}

bool CWBMTObjectManager::RemoveMTObject( const DWORD &dwObjectIndex )
{
#ifdef SUPPORTMTGES
	return false;
#else
	//XAutoLock lock(m_csMTObjectOperation);//避免网络同步时崩溃（赵英文20130423）
	bool bReturnValue = false;

	while (true)
	{
		bool bRemoveOK = false;
		std::map<unsigned long, CWBMTObject*>::iterator it = m_pObjectMap->begin();
		for (; m_pObjectMap->end() != it; ++it)
		{
			CWBObject *pWBObject = dynamic_cast<CWBObject*>(it->second);
			if ( pWBObject && (pWBObject->GetLayerIndex() == dwObjectIndex))
			{
				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->ClearMTDrawMap(it->first);
				}
				m_pFrameMoveObject->ClearMTDrawMap(it->first);

				m_pObjectMap->erase(it);
				bRemoveOK = true;
			}
		}
		if (bRemoveOK)
		{
			bReturnValue = (bReturnValue || bRemoveOK);
		}
		else
		{
			break;
		}
	}

	return bReturnValue;
#endif
}

void CWBMTObjectManager::RemoveAllMTObject()
{
#ifdef SUPPORTMTGES
	return;
#else
	//XAutoLock lock(m_csMTObjectOperation);//避免网络同步时崩溃（赵英文20130423）
	m_pObjectMap->clear();
	if (g_pWhiteBoardView)
	{
		g_pWhiteBoardView->ClearAllMTDrawMap();
	}
	m_pFrameMoveObject->ClearAllMTDrawMap();
#endif
}

void CWBMTObjectManager::addMTCursor( MTCursor *tcur )
{
#ifdef SUPPORTMTGES
	return;
#else
	if (g_bAvconLock) 
		return;

	XAutoLock lock(m_csMTObjectOperation);

	const DWORD dwCursorID = tcur->getCursorID();
	CPoint pt((int)tcur->getX(), (int)tcur->getY());

	if (CScreenDrawMgr::GetIns())
	{
		///屏幕坐标转换成ScreenDrawDlg的客户坐标
		CScreenDrawMgr::GetIns()->GetScreenDrawDlgClientPosition(pt);
	}
	else
	{
		::ScreenToClient(m_hViewWnd,&pt);
		CClientDC dc(g_pWhiteBoardView);
		g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
		dc.DPtoLP(&pt);//将视图坐标转换为文档作标
	}
	if (m_pObjectMap->empty())
	{
		CTransparent_Dlg* ptra = g_pWhiteBoardView->GetTransparentDlg();
		if (ptra)
		{
			if (g_pWhiteBoardView->GetBroadcastStatus())
			{
				m_bMTUserCursor = TRUE;
				m_nFirstID = dwCursorID;
				::PostMessage(m_hViewWnd, WM_MTSENDCURSOR, (WPARAM)(tcur->getX()), (LPARAM)(tcur->getY()));
			}
		}
	}
	///向放大镜发送触摸数据
	CWBMagnifier *pWBMagnifier = g_pWhiteBoardView->GetWBMagnifier();
	if (pWBMagnifier)
	{
		if (TRUE == pWBMagnifier->MouseDown(pt))
		{
			if (AddObjectMap(dwCursorID, pWBMagnifier))
			{
				pWBMagnifier->addMTCursor(tcur);
			}
			return;
		}
	}

	///当前状态
	if (DRAW_NORMAL == g_nDrawStatus)
	{
		CObList *pList = NULL;

		///屏幕绘制状态和非屏幕绘制状态时list不同
		if (CScreenDrawMgr::GetIns())
		{
			pList = g_pWBPage->GetWBLabelObjList();
		}  
		else
		{
			pList = g_pWBPage->GetWBObjList();
		}

		POSITION pos;
		CWBObject *pWbObj = NULL;
		BOOL bFound = FALSE;
		WB_OBJ_AREA state =WB_PS_NONE;

		for ( pos = pList->GetTailPosition(); NULL != pos; )
		{
			pWbObj = dynamic_cast<CWBObject *>(pList->GetPrev(pos));
			if (pWbObj == NULL)
			{
				continue;
			}
			if (pWbObj->IsBkg())
			{
				continue;
			}
 			if (pWbObj->GetType() == WB_OBJ_ARBITRARYTRIANGLE)
 			{
 				state = (WB_OBJ_AREA)((CWBArbitraryTriangle*)pWbObj)->IsInSelectArea(pt);
 				if (state != WB_PS_TRIAMGLEPOINT)
 				{
 					((CWBArbitraryTriangle*)pWbObj)->SetIsEdit(false);
 				}
 				else
 				{
 					//g_pWhiteBoardView->SetCurobjLayerIndex(g_pWBPage->GetCurEditingObjLayerIndex()) ;
 					if (AddObjectMap(dwCursorID, pWbObj))
 					{//通过此函数调用触摸基类的OnGestureCombineHandle函数，主要用来传递点的偏移量、缩放量、旋转角度等
 						pWbObj->addMTCursor(tcur);	
 					}
 					m_nEditTriangleTouchID = dwCursorID;
 					g_nDrawStatus = EDITINGTRIANGLE;
 					break;
 					//return;
 				}
 
 			}
 			else
			{
				state = (WB_OBJ_AREA)( pWbObj->IsInSelectArea(pt));
			}
			if (WB_PS_NONE != state)
			{
				bFound = TRUE;
				break;
			}
		}
		if (bFound )
		{
			if(pWbObj)
			{
				int nTemp = g_pWBPage->PointInSelectFrame(pt);

				if (nTemp == WB_PS_SELECTPOINT)
				{
					if( AddObjectMap(dwCursorID, g_pWhiteBoardView) )
					{
						m_pViewMTObject->addMTCursor(tcur);
						g_nDrawStatus = CHANGE_ANNOTATIONS;
					}
				}
			}
			if (WB_PS_OBJ == state)
			{
				if (AddObjectMap(dwCursorID, pWbObj))
				{//通过此函数调用触摸基类的OnGestureCombineHandle函数，主要用来传递点的偏移量、缩放量、旋转角度等
					pWbObj->addMTCursor(tcur);	
				}
				//文档手势缩放或旋转前，记录点数据
				if (pWbObj->GetType() == WB_OBJ_DOCUMENT)
				{
					CWBDocument *pDoc = (CWBDocument *)pWbObj;

					if (pDoc->GetIsMTRotate()) //如果是旋转
					{
						pDoc->GetSrcAnnPt();
						CPoint ptRotationCenter;
						pDoc->GetSrcCenterPt();
						//pDoc->GetCenterPoint(g_ptDocRotCenter);
						//pDoc->RotateByCenter(0);
						pDoc->GetRotationCenter(g_ptDocRotCenter);
						pDoc->RotateByPoint(g_ptDocRotCenter,0);
					}

					if (pDoc->GetIsMTZoom()) //如果是缩放
					{
						pDoc->GetSrcAnnPt();


						pDoc->GetSrcCenterPt();
						//pDoc->GetCenterPoint(g_ptDocRotCenter);
						//pDoc->RotateByCenter(0);	
						pDoc->GetRotationCenter(g_ptDocRotCenter);
						pDoc->RotateByPoint(g_ptDocRotCenter,0);

						//pDoc->GetCenterPoint(g_ptDocRotCenter);
						pDoc->GetRotationCenter(g_ptDocRotCenter);
						g_dbDocRotAngle = pDoc->GetRotationAngle();
						pDoc->GetSrcZoomCenterPt();

						g_ptDocZoomCenter = g_ptDocRotCenter;
					}
				}
				//视频截图手势缩放或旋转前，记录点数据
				if (pWbObj->GetType() == WB_OBJ_PICTURE)
				{
					CWBPicture *pPic = (CWBPicture *)pWbObj;
					if (pPic->GetIsPrintScreen())
					{
						if (pPic->GetIsMTRotate()) //如果是旋转
						{
							pPic->GetSrcAnnPt();
							CPoint ptRotationCenter;
							pPic->GetSrcCenterPt();
							//pDoc->GetCenterPoint(g_ptDocRotCenter);
							//pDoc->RotateByCenter(0);
							pPic->GetRotationCenter(g_ptDocRotCenter);
							pPic->RotateByPoint(g_ptDocRotCenter,0);
						}

						if (pPic->GetIsMTZoom()) //如果是缩放
						{
							pPic->GetSrcAnnPt();
							pPic->GetSrcCenterPt();
							//pDoc->GetCenterPoint(g_ptDocRotCenter);
							//pDoc->RotateByCenter(0);	
							pPic->GetRotationCenter(g_ptDocRotCenter);
							pPic->RotateByPoint(g_ptDocRotCenter,0);

							//pDoc->GetCenterPoint(g_ptDocRotCenter);
							pPic->GetRotationCenter(g_ptDocRotCenter);
							g_dbDocRotAngle = pPic->GetRotationAngle();
							pPic->GetSrcZoomCenterPt();

							g_ptDocZoomCenter = g_ptDocRotCenter;
						}
					}
				}
				return;
			}
			else if ( WB_PS_SELECTFRAME == state)
			{
				if( AddObjectMap(dwCursorID, m_pFrameMoveObject))
				{
					m_pFrameMoveObject->PreAddMTCursor(tcur, pWbObj);
				}

				if (pWbObj->IsInfiniteClone() && !pWbObj->IsLock())
				{
					pWbObj->SetClone(TRUE);
				}

				//文档手势缩放或旋转前，记录点数据
				if (pWbObj->GetType() == WB_OBJ_DOCUMENT)
				{
					CWBDocument *pDoc = (CWBDocument *)pWbObj;

					if (pDoc->GetIsMTRotate()) //如果是旋转
					{
						pDoc->GetSrcAnnPt();
						CPoint ptRotationCenter;

                        ((CWBImageButton*)pDoc->GetImageButton())->ChangeRationCenterOperator();
						pDoc->GetSrcCenterPt();
						//pDoc->GetCenterPoint(g_ptDocRotCenter);
						//pDoc->RotateByCenter(0);
                        pDoc->GetRotationCenter(g_ptDocRotCenter);
						pDoc->RotateByPoint(g_ptDocRotCenter,0);
					}

					if (pDoc->GetIsMTZoom()) //如果是缩放
					{
						pDoc->GetSrcAnnPt();
						((CWBImageButton*)pDoc->GetImageButton())->ChangeRationCenterOperator();
						pDoc->GetSrcCenterPt();
						//pDoc->GetCenterPoint(g_ptDocRotCenter);
						//pDoc->RotateByCenter(0);	
						pDoc->GetRotationCenter(g_ptDocRotCenter);
						pDoc->RotateByPoint(g_ptDocRotCenter,0);

						//pDoc->GetCenterPoint(g_ptDocRotCenter);
						pDoc->GetRotationCenter(g_ptDocRotCenter);
						g_dbDocRotAngle = pDoc->GetRotationAngle();
						pDoc->GetSrcZoomCenterPt();

						g_ptDocZoomCenter = g_ptDocRotCenter;
					}
				}
				//视频截图手势缩放或旋转前，记录点数据
				if (pWbObj->GetType() == WB_OBJ_PICTURE)
				{
					CWBPicture *pPic = (CWBPicture *)pWbObj;
					if (pPic->GetIsPrintScreen())
					{
						if (pPic->GetIsMTRotate()) //如果是旋转
						{
							pPic->GetSrcAnnPt();
							CPoint ptRotationCenter;
							((CWBImageButton*)pPic->GetImageButton())->ChangeRationCenterOperator();
							pPic->GetSrcCenterPt();
							//pDoc->GetCenterPoint(g_ptDocRotCenter);
							//pDoc->RotateByCenter(0);
							pPic->GetRotationCenter(g_ptDocRotCenter);
							pPic->RotateByPoint(g_ptDocRotCenter,0);
						}

						if (pPic->GetIsMTZoom()) //如果是缩放
						{
							pPic->GetSrcAnnPt();
							((CWBImageButton*)pPic->GetImageButton())->ChangeRationCenterOperator();
							pPic->GetSrcCenterPt();
							//pDoc->GetCenterPoint(g_ptDocRotCenter);
							//pDoc->RotateByCenter(0);	
							pPic->GetRotationCenter(g_ptDocRotCenter);
							pPic->RotateByPoint(g_ptDocRotCenter,0);

							//pDoc->GetCenterPoint(g_ptDocRotCenter);
							pPic->GetRotationCenter(g_ptDocRotCenter);
							g_dbDocRotAngle = pPic->GetRotationAngle();
							pPic->GetSrcZoomCenterPt();

							g_ptDocZoomCenter = g_ptDocRotCenter;
						}
					}
				}
				return;
			}
			else if ( WB_PS_SELECTBUTTON == state)
			{
				g_pWhiteBoardView->SetPastePt(pt);
				if( AddObjectMap(dwCursorID, m_pClickObject) )
				{
					m_pClickObject->SetAssociatedObject(pWbObj, BUTTON_TYPE_IMAGEBUTTON);
					m_pClickObject->addMTCursor(tcur);
				}
				return;
			}
			else if ( (WB_PS_CTRLBUTTON == state) && (NULL == CScreenDrawMgr::GetIns()) )
			{
				if (pWbObj->GetType() == WB_OBJ_COMBINE)
				{
					CWBObject *pCtrlObj = ((CWBCombine *)pWbObj)->SetControlObj(pt);
					if (pCtrlObj == NULL)
					{
						g_nDrawStatus = DRAW_NORMAL;
						return;
					}
					pWbObj = pCtrlObj;
				}
				if( AddObjectMap(dwCursorID, m_pClickObject) )
				{
					m_pClickObject->SetAssociatedObject(pWbObj, BUTTON_TYPE_CTRLBUTTON);
					m_pClickObject->addMTCursor(tcur);
				}
				return;
			}
			///播放器上的按钮
			else if ( (WB_PS_INMEDIACTRL == state) && (NULL == CScreenDrawMgr::GetIns()) )
			{
				if (AddObjectMap(dwCursorID, m_pClickObject))
				{
					m_pClickObject->SetAssociatedObject(pWbObj, BUTTON_TYPE_INMEDIACTRL);
					m_pClickObject->addMTCursor(tcur);
				}
				return;
			}
			
		}
		else
		{	
			if (0 == m_pObjectMap->size())
			{
				if( AddObjectMap(dwCursorID, this) )
				{
					CMTObject::addMTCursor(tcur);
				}
			}
		}
	}
	else
	{
		///有屏幕绘制就交给屏幕绘制处理，否则交给view处理
		if (CScreenDrawMgr::GetIns())
		{
			//非操作状态的处理都交给screendraw
			if( AddObjectMap(dwCursorID, CScreenDrawMgr::GetIns()) )
			{
				CScreenDrawMgr::GetIns()->addMTCursor(tcur);
			}
		}
		else
		{
			//非操作状态的处理都交给view
			if( AddObjectMap(dwCursorID, g_pWhiteBoardView) )
			{
				m_pViewMTObject->addMTCursor(tcur);
			}
		}
	}
#endif
}

void CWBMTObjectManager::removeMTCursor( MTCursor *tcur )
{
#ifdef SUPPORTMTGES
	return;
#else
 	XAutoLock lock(m_csMTObjectOperation);
 	CPoint pt;
 	pt.x = (LONG)tcur->getX();
 	pt.y = (LONG)tcur->getY();
 	::ScreenToClient(m_hViewWnd, &pt);
 
 	CWBMagnifier *pWBMagnifier = dynamic_cast<CWBMagnifier*>(g_pWhiteBoardView->GetWBMagnifier());
 	if (pWBMagnifier)
 	{
 		
 		BOOL bState = pWBMagnifier->MouseUp(pt);
 		if(MAQN_NEED_REFRESH == bState)
 		{
 			g_pWhiteBoardView->SendMagn();
 			g_pWhiteBoardView->Invalidate(TRUE);			
 		}
 		else if (TRUE == bState)
 		{	
 			g_pWhiteBoardView->SendMagn();		
 		}
 	}
 	if (tcur->getCursorID() == m_nFirstID)
 	{
 		m_bMTUserCursor = FALSE;
 	}
 	const DWORD dwCursorID = tcur->getCursorID();
 	std::map<DWORD, CWBMTObject*>::iterator it = m_pObjectMap->find(dwCursorID);
 	if (m_pObjectMap->end() != it)
 	{
 		
 		
 		if (it->second == this)
 		{
 			CMTObject::removeMTCursor(tcur);
 		}
 		else
 		{
 			it->second->removeMTCursor(tcur);
 		}
 		if (m_nEditTriangleTouchID == tcur->getCursorID())
 		{
 			if (EDITINGTRIANGLE == g_nDrawStatus)
 			{
 
  				CWBArbitraryTriangle *pTriangle = (CWBArbitraryTriangle *)it->second;
  				pTriangle->EditTrianglePoint(pt,true);	
  				pTriangle->AdjustObject();
  				/*pTriangle->SetIsEdit(false);*/
  				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
  				g_pWhiteBoardView->Invalidate();
  
  				//发送同步消息
  				XMemIni xmlObj;
  				pTriangle->SetXmlValue(xmlObj);
  				g_pWhiteBoardView->ChangeObj(xmlObj);
 
  				g_nDrawStatus = DRAW_NORMAL;
 
 			}
 		}
 		RemoveObjectMap(dwCursorID);
 	}
 	if(m_pObjectMap->empty())
 	{
 		if (g_bRunWater)
 		{
 			::PostMessage(m_hViewWnd, WM_CREATE_WATER, (WPARAM)(pt.x), (LPARAM)(pt.y));
 		}
 	}
#endif
}

void CWBMTObjectManager::updateMTCursor( MTCursor *tcur )
{
#ifdef SUPPORTMTGES
	return;
#else
	if (m_bMTUserCursor &&tcur->getCursorID() == m_nFirstID)
	{
		CPoint pt((int)tcur->getX(), (int)tcur->getY());
		::PostMessage(m_hViewWnd, WM_MTSENDCURSOR, (WPARAM)(pt.x), (LPARAM)(pt.y));
		//g_pWhiteBoardView->ProcessMTUserCursor(pt);
	}
	//XAutoLock lock(m_csMTObjectOperation);//避免网络同步时崩溃（赵英文20130423）
	std::map<DWORD, CWBMTObject*>::iterator it = m_pObjectMap->find(tcur->getCursorID());
	if (m_pObjectMap->end() != it)
	{
		//CWBObject *pWBObj = dynamic_cast<CWBObject*>(it->second);
		//if (pWBObj)
		//{
		//	if (pWBObj->GetSelectFrameShow() && pWBObj->IsClone() && (!pWBObj->IsLock()))
		//	{
		//		g_pWhiteBoardView->CloneObject();
		//		pWBObj->SetClone(FALSE);
		//		return;
		//	}
		//}
		
		
		if (it->second == this)
		{
			CMTObject::updateMTCursor(tcur);
		}
		else
		{
			it->second->updateMTCursor(tcur);
		}
		if (tcur->getCursorID() == m_nEditTriangleTouchID)
		{
			if (EDITINGTRIANGLE == g_nDrawStatus)
			{

 				CPoint pt((int)tcur->getX(),(int)tcur->getY());
 				::ScreenToClient(m_hViewWnd,&pt);
 				CClientDC dc(g_pWhiteBoardView);
 				g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
 				dc.DPtoLP(&pt);//将视图坐标转换为文档作标
 				CWBArbitraryTriangle *pTriangle = (CWBArbitraryTriangle *)it->second;
 				pTriangle->EditTrianglePoint(pt);
 				XMemIni xmlObj;					
 				pTriangle->SetXmlValue(xmlObj);
 				xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
 				g_pWhiteBoardView->Broadcast_Data(xmlObj);
 				g_pWhiteBoardView->Invalidate();


			}
		}
	}
#endif
}

BOOL CWBMTObjectManager::SetMTAuthorize( BOOL bMTAuthorize)
{
#ifdef SUPPORTMTGES
	return false;
#else
	BOOL bNotSame = ((!m_bMTAuthorize)&&bMTAuthorize)||(m_bMTAuthorize&&(!bMTAuthorize));
	if(bNotSame)
	{
		//获取触摸数据的协议类型
		m_gType = AVCON_SDK_MTTouch::GetConnectProtocol();
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

		if (bUseSystemTouch)
		{
			m_gType = PROTOCOL_WINTOUCH_CONNECT;
		}

		switch (m_gType)
		{
		case  PROTOCOL_WINTOUCH_CONNECT:
			{
				if(bMTAuthorize)
				{
					CEncapsulateTouchAPI::U_RegisterTouchWindow(m_instance->m_hViewWnd);
				}
				else
				{
					CEncapsulateTouchAPI::U_UnRegisterTouchWindow(m_instance->m_hViewWnd);
				}
			}
			break;
		case  PROTOCOL_TUIO_CONNECT:
		case  PROTOCOL_HID_CONNECT:
			{
				if(bMTAuthorize)
				{
					AVCON_SDK_MTTouch::AddMTObject(m_instance, true);
				}
				else
				{
					AVCON_SDK_MTTouch::RemoveMTObject(m_instance);
				}
			}
			break;
		default:
			{
				assert(false);
			}
			break;
		}
		m_bMTAuthorize = bMTAuthorize;
	}
	return bNotSame;
#endif
}
EProtocolType CWBMTObjectManager::m_gType = PROTOCOL_WINTOUCH_CONNECT;

CWBMTObjectManager * CWBMTObjectManager::m_instance = NULL;


CWBMTObjectFramePan::CWBMTObjectFramePan(HWND hWnd)
	: m_hViewWnd(hWnd)
	, m_pMapSelectObj(NULL)
	, m_bPanTogether(false)
	, m_ptOffset(0,0)
{
	m_pMapSelectObj = new std::map<unsigned long, SObjectCursor>;

#ifndef SUPPORTMT

		AVCON_SDK_MTTouch::CMTObject::AddGesture(AVCON_SDK_MTTouch::TGT_DYNAMIC, (void*)(TGC_BASIC_DYNAMIC));
		AVCON_SDK_MTTouch::CMTObject::AddGesture(AVCON_SDK_MTTouch::TGT_ACLICK, (void*)(TGC_CLICK | TGC_TAP));	
#endif
	
}

CWBMTObjectFramePan::~CWBMTObjectFramePan()
{
	if (NULL != m_pMapSelectObj)
	{
		delete  m_pMapSelectObj;
		m_pMapSelectObj = NULL;
	}
}

void CWBMTObjectFramePan::touchMoveHandler( const AVCON_SDK_MTTouch::TouchUp& te )
{
#ifdef SUPPORTMTGES
	return;
#else
	if (g_pWhiteBoardView->MTInfiniteClone())
	{
		m_bPanTogether = true;
	}

	if (!CScreenDrawMgr::GetIns())
	{
		CWBDustbin *pWBDustbin = g_pWhiteBoardView->GetDustbin();
		CPoint point((int)te.xpos,(int)te.ypos);
		::ScreenToClient(m_hViewWnd,&point);
		CClientDC dc(g_pWhiteBoardView);
		g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
		dc.DPtoLP(&point);//将视图坐标转换为文档作标
		
		if (NULL != pWBDustbin)
		{
			g_pWhiteBoardView->MoveObjToDustbin(point,te.id);
		}
	}
#endif
}

void CWBMTObjectFramePan::touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te)
{
#ifdef SUPPORTMTGES
	return;
#else
	CWBMTObject::touchUpHandler(te);

	if (!CScreenDrawMgr::GetIns() && m_map.empty())
	{
		///用于触摸使用垃圾桶删除对象
		CWBDustbin *pWBDustbin = g_pWhiteBoardView->GetDustbin();
		CPoint point((int)te.xpos, (int)te.ypos);
		::ScreenToClient(m_hViewWnd, &point);
		CClientDC dc(g_pWhiteBoardView);
		g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
		dc.DPtoLP(&point);//将视图坐标转换为文档作标
		if (NULL != pWBDustbin) //假如创建了垃圾箱
		{
			g_pWhiteBoardView->SetDustbinTouchID();
			if (MOUSE_OUTSIDE != pWBDustbin->PointIn(point))
			{				
				///主要用于让view来画对象被垃圾桶删除时的动画
				g_nDrawStatus = MOVING_LINE;
				g_pWhiteBoardView->touchUpHandler(te);

				///删除被选中的对象,并重置状态
				g_pWhiteBoardView->SetTouchDustbinDelete(true);
				g_pWhiteBoardView->DeleteSelectedObjects();
				g_pWhiteBoardView->SetTouchDustbinDelete(false);
				g_pWhiteBoardView->SetTouchEventId(0);
				g_pWhiteBoardView->Invalidate();
				g_pWhiteBoardView->UpdateWindow();
				g_nDrawStatus = DRAW_NORMAL;
				g_pWhiteBoardView->Invalidate();
				g_pWhiteBoardView->UpdateWindow();
			}
			else
			{
				g_pWhiteBoardView->SetObjectoutDustbin();
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_pWhiteBoardView->AddComListToObj();
	}

	///当是网络版并且所有在选择框上的触摸点弹起时进行同步
	if(m_map.empty())
	{
		if (g_pHPWBDataNotify && !g_pWhiteBoardView->GetIsLocalWhiteboard())
		{
			g_pWhiteBoardView->MoveMsg(CPoint(0,0), GenerateSSRC());
		}
		else
		{
			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			g_pWhiteBoardView->Invalidate();
		}
	}
#endif
}

void CWBMTObjectFramePan::OnGestureCombineHandle( const GestureCombinePar &gesPar )
{
#ifdef SUPPORTMTGES
	return;
#else
	if (m_bPanTogether)
	{
		//view处理所有有选择框对象的平移操作
		g_pWBPage->MoveSelectedObj(CPoint((int)gesPar.xtrans, (int)gesPar.ytrans));

		///当是网络版时，没移动10次就实时发送对象偏移量进行同步
		if (g_pHPWBDataNotify)
		{
			if (DoesCarryCounting())
			{
				POINT pt;
				pt.x = (LONG)gesPar.xanchor;
				pt.y = (LONG)gesPar.yanchor;
				::ScreenToClient(m_hViewWnd, &pt);
				if (m_ptOffset.x == 0)
				{
					m_ptOffset = pt;
				}
				CPoint point = pt;
				SendMoveObjOffset(point - m_ptOffset,GenerateSSRC());
				m_ptOffset = point;
			}
		}

		///开启了屏幕绘制则重绘屏幕绘制，否则重绘view
		if (CScreenDrawMgr::GetIns())
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
			g_pWhiteBoardView->Invalidate();
		}
	}
#endif
}

void CWBMTObjectFramePan::PreAddMTCursor( MTCursor *tcur, CWBMTObject *pObject )
{
#ifdef SUPPORTMTGES
	return;
#else
	unsigned long dwTouchID = tcur->getCursorID();
	std::map<unsigned long, SObjectCursor>::iterator it = m_pMapSelectObj->find(dwTouchID);
	if(m_pMapSelectObj->end() != it)
	{
#ifdef _DEBUG
		assert(false);
#endif
		m_pMapSelectObj->erase(it);
		return;
	}

	if (!m_bPanTogether && (1 == g_pWBPage->GetSelectObjLst()->GetCount()))
	{
		pObject->addMTCursor(tcur);
		if (NULL != g_pWhiteBoardView->GetDustbin())
		{
			CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
			int iSelectObjCounts = pSelectObjLst->GetSize();
			POSITION pos;
			CWBObject *pObj = NULL;
			for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
			{
				pos = pSelectObjLst->FindIndex(i);
				if (pos == NULL)
				{
					continue;
				}
				pObj = pSelectObjLst->GetAt(pos);
				if (pObj == NULL)
				{
					continue;
				}
				if (pObj->IsLock() || pObj->IsLimit())
				{
					continue;
				}
				else
				{
					//pObj->m_preAlpha = pObj->GetAlpha();
					pObj->SetPreAlpha(pObj->GetAlpha());
				}
			}
		}
		///当触摸选中对象且有且只有1个选择框时对象允许旋转缩放平移 touchdown
	}
	else
	{
		if (!m_bPanTogether)
		{
			PopChildCursorAndPush2Frame();
			m_bPanTogether = true;
		}
	}
	
	CMTObject::addMTCursor(tcur);
	///当触摸选中对象且有多个选择框时对象只能平移 touchdown

	SObjectCursor objectCursor(pObject, tcur);
	m_pMapSelectObj->insert(std::pair<unsigned long, SObjectCursor>(dwTouchID, objectCursor));
#endif
}

void CWBMTObjectFramePan::removeMTCursor( MTCursor *tcur )
{
#ifdef SUPPORTMTGES
	return;
#else
	std::map<unsigned long, SObjectCursor>::iterator it = m_pMapSelectObj->find(tcur->getCursorID());
	if (m_pMapSelectObj->end() != it)
	{
		if (!m_bPanTogether)
		{
			it->second.pObject->removeMTCursor(tcur);
		}
		CMTObject::removeMTCursor(tcur);
		m_pMapSelectObj->erase(it);
	}

	if (m_pMapSelectObj->empty())
	{
		m_bPanTogether = false;
	}
#endif
}

void CWBMTObjectFramePan::updateMTCursor( MTCursor *tcur )
{
#ifdef SUPPORTMTGES
	return;
#else
	if (g_bAvconLock) 
		return;

	std::map<unsigned long, SObjectCursor>::iterator it = m_pMapSelectObj->find(tcur->getCursorID());
	if (m_pMapSelectObj->end() != it)
	{
		if (!m_bPanTogether)
		{
			if (1 < g_pWBPage->GetSelectObjLst()->GetCount())//时刻检查
			{
				PopChildCursorAndPush2Frame();
				m_bPanTogether = true;
			}
			else
			{
				it->second.pObject->updateMTCursor(tcur);
			}
		}
		CMTObject::updateMTCursor(tcur);
	}
#endif
}

void CWBMTObjectFramePan::SendMoveObjOffset(CPoint cpoffset,unsigned long ulEventId)
{
#ifdef SUPPORTMTGES
	return;
#else
	CWBObject *pObj = NULL;
	int nIndex = 0;
	CString strPointItem;
	strPointItem.Format(_T("%04d%04d"), 
		cpoffset.x,
		cpoffset.y);
	string strPoints = ToStr(strPointItem.GetBuffer(strPointItem.GetLength()));
	std::vector<DWORD> vecSelectFrameList;
	vecSelectFrameList = g_pWBPage->GetWBSelectFrameList();
	int nCount = vecSelectFrameList.size();
	for (nIndex = 0; nIndex < nCount; ++nIndex )
	{
		pObj = g_pWBPage->FindByLayerIndex(vecSelectFrameList[nIndex]);
		if (pObj == NULL)
		{
			continue;
		}
		XMemIni xmlObj;
		xmlObj.Set(KEY_OBJ_ID, pObj->GetLayerIndex());
		xmlObj.Set(KEY_OBJ_TYPE, pObj->GetType());
		xmlObj.Set(KEY_MOVE_OFFSET, strPoints);
		xmlObj.Set(KEY_EVENT_OBJNUMBER, nCount);
		xmlObj.Set(KEY_EVENT_ID, ulEventId);
		xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJ_MOVE);
		g_pWhiteBoardView->Broadcast_Data(xmlObj);
	}
	strPointItem.ReleaseBuffer();
#endif
}

void CWBMTObjectFramePan::PopChildCursorAndPush2Frame()
{
#ifdef SUPPORTMTGES
	return;
#else
	//先弹起所有自行处理对象的cursor,然后添加整体对象中来
	std::map<unsigned long, SObjectCursor>::iterator itInner = m_pMapSelectObj->begin();
	for (; m_pMapSelectObj->end() != itInner; ++itInner)
	{
		SObjectCursor objectCursor(itInner->second);
		objectCursor.pObject->removeMTCursor(objectCursor.pCursor);
		CMTObject::addMTCursor(objectCursor.pCursor);
	}
#endif
}

void CWBMTObjectFramePan::ClearAllMTDrawMap()
{
#ifdef SUPPORTMTGES
	return;
#else
	m_pMapSelectObj->clear();
	if (!m_map.empty())
	{
		m_map.clear();
	}
#endif
}

bool CWBMTObjectFramePan::ClearMTDrawMap( const DWORD &dwTouchID )
{
#ifdef SUPPORTMTGES
	return false;
#else
	std::map<unsigned long, SObjectCursor>::iterator it = m_pMapSelectObj->find(dwTouchID);
	if (m_pMapSelectObj->end() != it)
	{
		m_pMapSelectObj->erase(it);
		m_map.erase(it->first);
		return true;
	}
	return false;
#endif
}



CWBMTObjectButtonManager::CWBMTObjectButtonManager(HWND hWnd)
	: m_hViewWnd(hWnd)
	, m_pAssociatedObject(NULL)
	, m_nButtonState(0)
	,m_dwTimeGetTime(0)
{
}

CWBMTObjectButtonManager::~CWBMTObjectButtonManager()
{

}


void CWBMTObjectButtonManager::touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te)
{
#ifdef SUPPORTMTGES
	return;
#else
	if (m_pAssociatedObject)
	{
		m_dwTimeGetTime = ::timeGetTime();
		CPoint pt((int)te.xpos, (int)te.ypos);
		if(CScreenDrawMgr::GetIns() == NULL || g_pWhiteBoardView->GetIsRationButton() == TRUE ) 
		{
			::ScreenToClient(m_hViewWnd, &pt);
			CClientDC dc(g_pWhiteBoardView);
			g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
			dc.DPtoLP(&pt);//将视图坐标转换为文档作标
		}
		CWBObject *pWBObj = dynamic_cast<CWBObject*>(m_pAssociatedObject);
		if (pWBObj)
		{
			if (BUTTON_TYPE_IMAGEBUTTON == m_nButtonState)
			{
				if (pWBObj->GetImageButton() && pWBObj->GetSelectFrameShow())
				{
					pWBObj->GetImageButton()->ButtonDown(pt);
				}
				else
				{
#ifdef _DEBUG
					assert(false);
#endif
				}
			}
			else if (BUTTON_TYPE_CTRLBUTTON == m_nButtonState)
			{
				if (pWBObj->GetControlButton())
				{
					pWBObj->GetControlButton()->ButtonDown(pt);
					g_pWhiteBoardView->Invalidate();
				}
				else
				{
#ifdef _DEBUG
					assert(false);
#endif
				}
			}
			else if (BUTTON_TYPE_INMEDIACTRL == m_nButtonState)
			{
				CWBMedia *pWBMedia = dynamic_cast<CWBMedia*>(pWBObj);
				if (pWBMedia)
				{
					if (pWBMedia->GetMediaCtrl())
					{
						pWBMedia->GetMediaCtrl()->CtrlDown(pt);
						g_pWhiteBoardView->Invalidate();
					}
				}
				else
				{
#ifdef _DEBUG
					assert(false);
#endif
				}
			}
			else
			{
#ifdef _DEBUG
				assert(false);
#endif
			}
		}
	}
	else
	{
#ifdef _DEBUG
		assert(false);
#endif
	}
#endif
}

void CWBMTObjectButtonManager::touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te)
{
#ifdef SUPPORTMTGES
	return;
#else
	if (m_pAssociatedObject && g_pWhiteBoardView)
	{
			DWORD dwTime_difference = ::timeGetTime();
		if ( dwTime_difference - m_dwTimeGetTime >= MOVEISNEEDSENDDATA_TIME)
		{
			//需要发送同步消息
			m_dwTimeGetTime = dwTime_difference;
			g_pWhiteBoardView->SetMoveIsNeedSendData(TRUE);
		}
		CPoint pt((int)te.xpos, (int)te.ypos);
		if(CScreenDrawMgr::GetIns() == NULL || g_pWhiteBoardView->GetIsRationButton() == TRUE ) 
		{
			::ScreenToClient(m_hViewWnd, &pt);
			CClientDC dc(g_pWhiteBoardView);
			g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
			dc.DPtoLP(&pt);//将视图坐标转换为文档作标
		}
		//::ScreenToClient(m_hViewWnd, &pt);
		//CClientDC dc(g_pWhiteBoardView);
		//g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
		//dc.DPtoLP(&pt);//将视图坐标转换为文档作标
		CWBObject *pWBObj = dynamic_cast<CWBObject*>(m_pAssociatedObject);
		if (pWBObj)
		{
			if (BUTTON_TYPE_IMAGEBUTTON == m_nButtonState)
			{
				if (pWBObj->GetImageButton())
				{
					pWBObj->GetImageButton()->ButtonMove(pt);
				}
				else
				{
#ifdef _DEBUG
					assert(false);
#endif
				}
			}
			else if (BUTTON_TYPE_CTRLBUTTON == m_nButtonState)
			{
				if (pWBObj->GetControlButton())
				{
					pWBObj->GetControlButton()->ButtonMove(pt);
					g_pWhiteBoardView->Invalidate();
				}
				else
				{
#ifdef _DEBUG
					assert(false);
#endif
				}
			}
			else if (BUTTON_TYPE_INMEDIACTRL == m_nButtonState)
			{
			}
			else
			{
#ifdef _DEBUG
				assert(false);
#endif
			}
		}
	}
	else
	{
#ifdef _DEBUG
		assert(false);
#endif
	}
#endif
}

void CWBMTObjectButtonManager::touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te)
{
#ifdef SUPPORTMTGES
	return;
#else
	if (m_pAssociatedObject)
	{
		CPoint pt((int)te.xpos, (int)te.ypos);
		::ScreenToClient(m_hViewWnd, &pt);
		CClientDC dc(g_pWhiteBoardView);
		g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
		dc.DPtoLP(&pt);//将视图坐标转换为文档作标
		CWBObject *pWBObj = dynamic_cast<CWBObject*>(m_pAssociatedObject);
		if (BUTTON_TYPE_IMAGEBUTTON == m_nButtonState)
		{
			if (pWBObj->GetImageButton() && pWBObj->GetSelectFrameShow())
			{
				DWORD lowInfo = 0;
				DWORD higInfo = 0;
				DWORD nAppendMessage = 0;
				CPoint pt((int)te.xpos, (int)te.ypos);
				//::ScreenToClient(m_hViewWnd, &pt);
				if (CScreenDrawMgr::GetIns())
				{
					::ClientToScreen(m_hViewWnd,&pt);
					///屏幕坐标转换成ScreenDrawDlg的客户坐标
					CScreenDrawMgr::GetIns()->GetScreenDrawDlgClientPosition(pt);
				}
				else
				{
					//::ScreenToClient(m_hViewWnd,&pt);
					CClientDC dc(g_pWhiteBoardView);
					g_pWhiteBoardView->OnPrepareDC(&dc);//进行坐标原点的匹配
					dc.DPtoLP(&pt);//将视图坐标转换为文档作标
					//::ScreenToClient(m_hViewWnd, &pt);
				}

				lowInfo = pt.x;
				higInfo = pt.y;
				higInfo <<= 16;
				nAppendMessage = higInfo | lowInfo;

				::PostMessage(m_hViewWnd, UWM_TRACKPOPUPMENU, (WPARAM)(m_pAssociatedObject), (LPARAM)(nAppendMessage));
			}
			else
			{
#ifdef _DEBUG
				assert(false);
#endif
			}
		}
		else if (BUTTON_TYPE_CTRLBUTTON == m_nButtonState)
		{
			if (pWBObj->GetControlButton())
			{
				pWBObj->GetControlButton()->ButtonUp(pt);
				g_pWhiteBoardView->Invalidate();
			}
			else
			{
#ifdef _DEBUG
				assert(false);
#endif
			}
		}
		else if (BUTTON_TYPE_INMEDIACTRL == m_nButtonState)
		{
			CWBMedia *pWBMedia = dynamic_cast<CWBMedia*>(pWBObj);
			if (pWBMedia)
			{
				if (pWBMedia->GetMediaCtrl())
				{
					pWBMedia->GetMediaCtrl()->CtrlUp(pt);
					g_pWhiteBoardView->Invalidate();
				}
			}
			else
			{
#ifdef _DEBUG
				assert(false);
#endif
			}
		}
		else
		{
#ifdef _DEBUG
			assert(false);
#endif
		}
	}
	else
	{
#ifdef _DEBUG
		assert(false);
#endif
	}
	m_nButtonState = BUTTON_TYPE_NONE;
#endif
}

void CWBMTObjectButtonManager::SetAssociatedObject( CMTObject *pObject, const int &nButtonState )
{
#ifdef SUPPORTMTGES
	return;
#else
	m_pAssociatedObject = pObject;
	m_nButtonState = nButtonState;
#endif
}
