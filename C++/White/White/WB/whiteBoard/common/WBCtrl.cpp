// WBCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "../whiteBoard.h"
#include "WBCtrl.h"
#include "WBData.h"
#include "../WhiteBoardView.h"
#include "ScreenDrawMgr.h"
/************************************************************************/
/*				           CWBImageButton类								*/
/*					      生成对象的操作按钮							*/
/************************************************************************/
CWBImageButton::CWBImageButton(CWBObject *pWBObject/* = NULL*/)
	: m_bRotation(FALSE)
	, m_bZoom(FALSE)
	, m_ptObjCenter(CPoint(0, 0))
	, m_pDC(NULL)
	,m_bMoveRotionCenter(FALSE)
	,m_bIsImageMove(false)
	,m_onlyone(true)
{
	m_nType = 0;
	m_pWBObject = pWBObject;

	if (m_pWBObject != NULL)
	{
		m_pWBObject->GetRotationCenter(m_ptObjCenter);
		if (m_ptObjCenter == CPoint(0,0))
		{
			m_pWBObject->GetCenterPoint(m_ptObjCenter);
		}
		m_ptRotationCenter = m_ptObjCenter;
	}
	m_ptOffsetCenter = CPoint(0,0);
	m_ptcenterone = CPoint(0,0);
	m_bCenterMove = false;
	m_pImageLock = NULL;
	m_pImageMenu = NULL;
	m_pImageRotation = NULL;
	m_pImageUnLock = NULL;
	m_pImageZoom = NULL;
	m_pImageClone = NULL;
	m_pImageAnnotations = NULL;
	m_pImageRotationCenter = NULL;
	m_ptMoveRotationCenter = m_ptObjCenter;
	CPoint DrawPt;
	m_pWBObject->GetDrawRotationCenter(DrawPt);
	if (DrawPt == CPoint(0,0))
	{
		m_pWBObject->SetDrawRotationCenter(m_ptObjCenter);
	}
	m_pWBObject->SetRotationCenter(m_ptObjCenter);
	//InitImageFromResource();
	InitImageFromFile();
	CString PictureDir = GetCurrentPath();
	CString PicturePath = PictureDir;
	PicturePath = PictureDir + _T("\\SKIN\\WhiteBoard\\image\\circle.png");
	m_pImageRotationCenter = new ImageEx(PicturePath);
	m_ptIsRorationType = TRUE; 
}

CWBImageButton::~CWBImageButton()
{
	ClearImage();
}

bool CWBImageButton::InitImageFromFile()
{

	//wchar_t strFilename[MAX_PATH];

	CString PictureDir = GetCurrentPath();	//获取当前程序执行的目录
	CString picturePath = PictureDir;

	//初始化锁定按钮图片，默认为解锁状态	
	picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\Lock.png");			
	//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
	m_pImageLock = new ImageEx(picturePath);	

	//初始化锁定按钮图片，此时为锁定样式		
	picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\Unlock.png");
	//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
	m_pImageUnLock = new ImageEx(picturePath);		


	//初始化旋转按钮图片		
	picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\Rotation.png");
	//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
	m_pImageRotation = new ImageEx(picturePath);	

	//初始化菜单按钮图片
	picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\Set.png");
	//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
	m_pImageMenu = new ImageEx(picturePath);	

	//初始化缩放按钮图片
	picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\Zoom.png");
	//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
	m_pImageZoom = new ImageEx(picturePath);	

	//初始化无限克隆按钮图片
	picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\Clone.png");
	//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
	m_pImageClone = new ImageEx(picturePath);	

	//初始化标注尖角图片
	picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\Annotations.png");
	//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
	m_pImageAnnotations = new ImageEx(picturePath);	

	return true;
}

bool CWBImageButton::InitImageFromResource()
{
	m_pImageLock = new ImageEx(_T("PNG"), _T("IDB_LOCK"));	

	m_pImageUnLock = new ImageEx(_T("PNG"), _T("IDB_UNLOCK"));	

	m_pImageRotation = new ImageEx(_T("PNG"), _T("IDB_ROTATION"));	

	m_pImageMenu = new ImageEx(_T("PNG"), _T("IDB_MENU"));	

	m_pImageZoom = new ImageEx(_T("PNG"), _T("IDB_ZOOM"));	

	m_pImageClone = new ImageEx(_T("PNG"), _T("IDB_INFINITECLONE"));

	m_pImageAnnotations = new ImageEx(_T("PNG"), _T("IDB_ANNOTATIONS"));

	return true;
}

void CWBImageButton::ButtonDown(CPoint point)
{
	g_nDrawStatus = MOVE_CENTER_POINT;

	if (m_pWBObject == NULL)
	{
		return;
	}
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	WB_OBJ_TYPE WBObjectTYPE = (WB_OBJ_TYPE)m_pWBObject->GetType();
	if ( WBObjectTYPE == WB_OBJ_LINE)
	{
		m_ptZoomPre = point;
		return;
	}
	if (m_nType == BTN_TYPE_ROTATION/* && m_pWBObject->GetType() != WB_OBJ_DOCUMENT*/ && WBObjectTYPE != WB_OBJ_ANNOTATIONS) //旋转(标注不旋转)
	{
		m_bRotation = TRUE;	
		CPoint ptTempCenter;
		m_pWBObject->GetRotationCenter(ptTempCenter);
		CPoint DrawPt;
		m_pWBObject->GetDrawRotationCenter(DrawPt);
		if (DrawPt != ptTempCenter)
		{
			m_bCenterMove = true;
			m_pWBObject->SetRotationCenter(DrawPt);
		}

		if (WB_OBJ_COMBINE == WBObjectTYPE)//如果是组合
		{
			((CWBCombine *)m_pWBObject)->GetSrcLinePoint();
		}

		if (  WB_OBJ_DOCUMENT == WBObjectTYPE)//如果是文档
		{
			((CWBDocument *)m_pWBObject)->GetSrcAnnPt();
		}
		if (  WB_OBJ_PICTURE == WBObjectTYPE)//如果是图片
		{
			((CWBPicture *)m_pWBObject)->GetSrcAnnPt();
		}

		CPoint ptcentertwo;
		m_pWBObject->GetRotationCenter(ptcentertwo);
		m_pWBObject->GetPreRotationCenter(m_ptcenterone);
		if (m_bCenterMove == true && ptcentertwo!=m_ptcenterone&&WB_OBJ_COMBINE != WBObjectTYPE)
		{
			CPoint ptlefttop = (m_pWBObject->GetObjSelectFrame())->GetLeftTop();
			double angle = m_pWBObject->GetRotationAngle();
			CPoint ptrolefttop,ptcelefttop;
			PointRotationPoint(m_ptcenterone,-angle,ptlefttop,ptcelefttop);
			PointRotationPoint(ptcentertwo,-angle,ptlefttop,ptrolefttop);
			CPoint pt = ptrolefttop - ptcelefttop;
			if (WB_OBJ_DOCUMENT == WBObjectTYPE)
			{
				m_pWBObject->SetIsDocument(true);
			}
			if (WB_OBJ_PICTURE == WBObjectTYPE)
			{
				((CWBPicture*)m_pWBObject)->SetIsEditpic(true);
			}
			m_pWBObject->MoveOffset(pt);
			m_pWBObject->RotateByPoint(ptcentertwo,0);
			m_ptcenterone = ptcentertwo;
			m_bCenterMove = false;
		}





		//当对象是文本、生成刻度或者对象缩放且旋转后，因坐标系变化原因会产生偏移，在这里纠正偏移
		if ((  /*WB_OBJ_TEXT == WBObjectTYPE||*//* WB_OBJ_ANGLE == WBObjectTYPE||*/m_pWBObject->GetIsZoom()) && m_pWBObject->GetRotationAngle() != 0&&WB_OBJ_DOCUMENT != WBObjectTYPE
			&&WB_OBJ_RECT != WBObjectTYPE &&WB_OBJ_TRIANGLE != WBObjectTYPE &&WB_OBJ_RHOMBUS != WBObjectTYPE &&WB_OBJ_ELLIPSE != WBObjectTYPE &&WB_OBJ_PICTURE != WBObjectTYPE &&WB_OBJ_FLASH != WBObjectTYPE)
		{
			CPoint pt;			
			CWBSelectRectangle *pWBSelectRectangle = m_pWBObject->GetObjSelectFrame();

			m_ptRotationCenter = m_ptMoveRotationCenter;
			m_pWBObject->SetRotationCenter(m_ptMoveRotationCenter);
			/*m_pWBObject->GetCenterPoint(m_ptRotationCenter);*/
			if (pWBSelectRectangle != NULL)
			{
				pWBSelectRectangle->GetMapCenter(pt);
			}
			pt -= m_ptRotationCenter;
			//使对象中心点偏移                                  
			m_ptRotationCenter += pt;
			m_pWBObject->RotateByPoint(m_ptRotationCenter,0);

			//移动对象
			m_pWBObject->MoveOffset(pt);

			m_pWBObject->SetIsZoom(FALSE);
		}

		if (WB_OBJ_COMBINE == WBObjectTYPE)//如果是组合
		{
			m_pWBObject->CreateSelecrtFrame();
			((CWBCombine *)m_pWBObject)->GetSrcCenterPoint();
			m_pWBObject->GetRotationCenter(g_ptComRotCenter);
			m_pWBObject->GetRotationCenter(m_ptRotationCenter);
			m_pWBObject->RotateByPoint(m_ptRotationCenter,0);
			XMemIni xmlObj;
			
			m_pWBObject->SetXmlValue(xmlObj);
			g_pWhiteBoardView->ChangeObj(xmlObj,true);
		}

		if ( WB_OBJ_DOCUMENT == WBObjectTYPE) //如果是文档
		{
			((CWBDocument *)m_pWBObject)->GetSrcCenterPt();
			m_pWBObject->GetRotationCenter(g_ptDocRotCenter);
			m_pWBObject->RotateByPoint(g_ptDocRotCenter,0);
		}

		if ( WB_OBJ_PICTURE == WBObjectTYPE) //如果是图片
		{
			((CWBPicture *)m_pWBObject)->GetSrcCenterPt();
			m_pWBObject->GetRotationCenter(g_ptDocRotCenter);
			m_pWBObject->RotateByPoint(g_ptDocRotCenter,0);
		}

		m_pWBObject->GetRotationCenter(m_ptObjCenter);
		m_ptRotationPre = point;		
	}
	else if (m_nType == BTN_TYPE_ZOOM) //缩放
	{
		g_pWhiteBoardView->SetIsRationButton(TRUE);
		if (  WB_OBJ_COMBINE == WBObjectTYPE)//如果是组合
		{
			((CWBCombine *)m_pWBObject)->GetSrcLinePoint();

			//当对象缩放且旋转后，因坐标系变化原因会产生偏移，在这里纠正偏移
			if (m_pWBObject->GetIsZoom() && m_pWBObject->GetRotationAngle() != 0)
			{
				CPoint pt;			
				CWBSelectRectangle *pWBSelectRectangle = m_pWBObject->GetObjSelectFrame();

				m_pWBObject->GetCenterPoint(m_ptRotationCenter);
				if (pWBSelectRectangle != NULL)
				{
					pWBSelectRectangle->GetMapCenter(pt);
				}
				pt -= m_ptRotationCenter;
				//使对象中心点偏移                                  
				m_ptRotationCenter += pt;
				m_pWBObject->RotateByPoint(m_ptRotationCenter,0);

				//移动对象
				m_pWBObject->MoveOffset(pt);

				m_pWBObject->SetIsZoom(FALSE);
			}

			m_pWBObject->GetCenterPoint(m_ptRotationCenter);
			//滚动条偏移量
			UINT XnPos = g_pWhiteBoardView->getXnPos();
			UINT YnPos = g_pWhiteBoardView->getYnPos();
			m_ptRotationCenter.x -= XnPos;
			m_ptRotationCenter.y -= YnPos;

			((CWBCombine *)m_pWBObject)->GetSrcCenterPoint();
			//m_pWBObject->GetCenterPoint(g_ptComRotCenter);
			m_pWBObject->GetRotationCenter(g_ptComRotCenter);
			m_pWBObject->RotateByCenter(0);	


			m_pWBObject->GetCenterPoint(m_ptObjCenter);	
			GetCursorPos(&m_ptRotationPre); 		
			if(CScreenDrawMgr::GetIns() != NULL) //如果是桌面绘图，则进行桌面窗口坐标转换，否则进行视图窗口坐标转换
			{
				CScreenDrawMgr::GetIns()->GetMouseDlgScreenPosition(m_ptRotationCenter);
			}
			else
			{
				g_pWhiteBoardView->ClientToScreen(&m_ptRotationCenter);
			}
			//replaced by 陈常涛 end		
			m_pWBObject->GetRotationCenter(g_ptComRotCenter);
			g_dbComRotAngle = m_pWBObject->GetRotationAngle();
			((CWBCombine *)m_pWBObject)->GetSrcZoomCenter();

			//得到选择框数据
			if (m_pWBObject->GetObjSelectFrame() == NULL)
			{
				m_pWBObject->CreateSelecrtFrame();
			}
			g_ptComZoomCenter = (m_pWBObject->GetObjSelectFrame())->GetLeftTop();
			XMemIni xmlObj;

			m_pWBObject->SetXmlValue(xmlObj);
			g_pWhiteBoardView->ChangeObj(xmlObj,true);
		}

		if ( WB_OBJ_DOCUMENT == WBObjectTYPE)//如果是文档
		{
			((CWBDocument *)m_pWBObject)->GetSrcAnnPt();

			m_pWBObject->GetRotationCenter(m_ptRotationCenter);
			//滚动条偏移量
			UINT XnPos = g_pWhiteBoardView->getXnPos();
			UINT YnPos = g_pWhiteBoardView->getYnPos();
			m_ptRotationCenter.x -= XnPos;
			m_ptRotationCenter.y -= YnPos;

			((CWBDocument *)m_pWBObject)->GetSrcCenterPt();

			m_pWBObject->GetRotationCenter(g_ptDocRotCenter);
			//m_pWBObject->RotateByCenter(0);	
			m_pWBObject->RotateByPoint(g_ptDocRotCenter,0);


			//m_pWBObject->GetCenterPoint(m_ptObjCenter);	
			m_pWBObject->GetRotationCenter(m_ptObjCenter);
			GetCursorPos(&m_ptRotationPre); 		
			//replaced by 陈常涛 begin
			//befor replaced 
			/*g_pWhiteBoardView->ClientToScreen(&m_ptRotationCenter);*/
			//after replaced
			if(CScreenDrawMgr::GetIns() != NULL) //如果是桌面绘图，则进行桌面窗口坐标转换，否则进行视图窗口坐标转换
			{
				CScreenDrawMgr::GetIns()->GetMouseDlgScreenPosition(m_ptRotationCenter);
			}
			else
			{
				g_pWhiteBoardView->ClientToScreen(&m_ptRotationCenter);
			}
			//replaced by 陈常涛 end		

			m_pWBObject->GetRotationCenter(g_ptDocRotCenter);
			g_dbDocRotAngle = m_pWBObject->GetRotationAngle();
			((CWBDocument *)m_pWBObject)->GetSrcZoomCenterPt();

			//得到选择框数据
			if (m_pWBObject->GetObjSelectFrame() == NULL)
			{
				m_pWBObject->CreateSelecrtFrame();
			}
			g_ptDocZoomCenter = (m_pWBObject->GetObjSelectFrame())->GetLeftTop();;
		}

		if ( WB_OBJ_PICTURE == WBObjectTYPE)//如果是图片
		{
			((CWBPicture *)m_pWBObject)->GetSrcAnnPt();

			m_pWBObject->GetRotationCenter(m_ptRotationCenter);
			//滚动条偏移量
			UINT XnPos = g_pWhiteBoardView->getXnPos();
			UINT YnPos = g_pWhiteBoardView->getYnPos();
			m_ptRotationCenter.x -= XnPos;
			m_ptRotationCenter.y -= YnPos;

			((CWBPicture *)m_pWBObject)->GetSrcCenterPt();

			m_pWBObject->GetRotationCenter(g_ptDocRotCenter);
			m_pWBObject->RotateByPoint(g_ptDocRotCenter,0);

			m_pWBObject->GetRotationCenter(m_ptObjCenter);
			GetCursorPos(&m_ptRotationPre);

			if(CScreenDrawMgr::GetIns() != NULL) //如果是桌面绘图，则进行桌面窗口坐标转换，否则进行视图窗口坐标转换
			{
				CScreenDrawMgr::GetIns()->GetMouseDlgScreenPosition(m_ptRotationCenter);
			}
			else
			{
				g_pWhiteBoardView->ClientToScreen(&m_ptRotationCenter);
			}

			m_pWBObject->GetRotationCenter(g_ptDocRotCenter);
			g_dbDocRotAngle = m_pWBObject->GetRotationAngle();
			((CWBPicture *)m_pWBObject)->GetSrcZoomCenterPt();

			//得到选择框数据
			if (m_pWBObject->GetObjSelectFrame() == NULL)
			{
				m_pWBObject->CreateSelecrtFrame();
			}
			g_ptDocZoomCenter = (m_pWBObject->GetObjSelectFrame())->GetLeftTop();
		}

		if (WB_OBJ_ANNOTATIONS == WBObjectTYPE)//如果是标注
		{
			((CWBAnnotations *)m_pWBObject)->SetSrcPoint();
		}
		if (m_pWBObject->GetType() == WB_OBJ_MEDIA)//如果是视频
		{
			if (((CWBMedia*)m_pWBObject)->GetMediaPlayState() == Media_playing)
			{
				m_bIsImageMove = false; 
			}
		}

		m_bZoom = TRUE;
		m_ptZoomPre = point;
		//replaced by 陈常涛 begin
		//befor replaced 
		/*g_pWhiteBoardView->ClientToScreen(&m_ptZoomPre);*/
		//after replaced
		if(CScreenDrawMgr::GetIns() != NULL) //如果是桌面绘图，则进行桌面窗口坐标转换，否则进行视图窗口坐标转换
		{
			CScreenDrawMgr::GetIns()->GetMouseDlgScreenPosition(m_ptZoomPre);
		}
		else
		{
			g_pWhiteBoardView->ClientToScreen(&m_ptZoomPre);
		}
		//replaced by 陈常涛 end
	}
	else if (m_nType == BTN_TYPE_ROTATIONCENTER)
	{
		m_ptMoveRotationCenter = point;
		m_pWBObject->SetDrawRotationCenter(point);
		m_ptRotationCenter = point;
		m_bMoveRotionCenter = TRUE;
	}
}

void CWBImageButton::ButtonUp(CPoint point)
{

	g_bRunWater = FALSE;
	g_pWhiteBoardView->SetIsRationButton(FALSE);

	if (g_pWhiteBoardView == NULL || m_pWBObject == NULL)
	{
		return;
	}
	int nPageIndex = g_pWhiteBoardView->GetPrePage();
	//对直线对象进行的操作
	if (m_nType == BTN_TYPE_BEGIN || m_nType == BTN_TYPE_END)	
	{
		XMemIni xmlObj;
		m_pWBObject->SetXmlValue(xmlObj);
		if (nPageIndex != 0)
		{
			xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
			g_pWhiteBoardView->SetPrePage(0);
		}
		g_pWhiteBoardView->ChangeObj(xmlObj);
		return;
	}
	WB_OBJ_TYPE WBObjectTYPE = (WB_OBJ_TYPE)m_pWBObject->GetType();
	//对除直以外的对象进行的操作
	if (m_nType == BTN_TYPE_ROTATION) //旋转
	{
		m_bRotation = FALSE;

		if  (WB_OBJ_COMBINE == WBObjectTYPE)
		{
			m_pWBObject->CreateSelecrtFrame();

			//修正选择框中心点
			//  			CPoint pt;
			//  			CWBSelectRectangle *pWBSelectRectangle = m_pWBObject->GetObjSelectFrame();
			//  			CPoint ptRotationCenter;
			//  
			//  			m_pWBObject->GetRotationCenter(ptRotationCenter);
			//  			if (pWBSelectRectangle != NULL)
			//  			{
			//  				pWBSelectRectangle->GetMapCenter(pt);
			//  
			//  				pt -= ptRotationCenter;
			//  				//使选择框中心点偏移                                  
			//  				ptRotationCenter += pt;
			//  				pWBSelectRectangle->RotateByPoint(ptRotationCenter,0);
			//  
			//  				//移动选择框
			//  				pWBSelectRectangle->MoveOffset(pt);
			//  			}
		}

		//发送同步消息
		XMemIni xmlObj;
		m_pWBObject->SetXmlValue(xmlObj);
		if (nPageIndex != 0)
		{
			xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
			g_pWhiteBoardView->SetPrePage(0);
		}
		g_pWhiteBoardView->ChangeObj(xmlObj,true);
	}
	else if (m_nType == BTN_TYPE_LOCK && m_pWBObject != NULL) //锁定
	{
		if (m_pWBObject->IsLock())
		{
			m_pWBObject->Lock(FALSE);
	

			POSITION pos;
			int i = g_pWBPage->GetSelectObjLst()->GetSize();
			if (i == 1)//选中一个对象时
			{
				for (int j = 0;j < i;j ++)//遍历选中对象列表
				{
					pos = g_pWBPage->GetSelectObjLst()->FindIndex(j);
					if (pos == NULL)
					{
						continue;
					}
					g_pObj = (CWBObject *)g_pWBPage->GetSelectObjLst()->GetAt(pos);
				}
				if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
				{
					::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBOBJ,0,i);
				}
			}
		}
		else
		{
			m_pWBObject->Lock(TRUE);
		}
		//m_pWBObject->Lock(!m_pWBObject->IsLock());

		//发送同步消息
		XMemIni xmlObj;
		m_pWBObject->SetXmlValue(xmlObj);
		if (nPageIndex != 0)
		{
			xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
			g_pWhiteBoardView->SetPrePage(0);
		}
		g_pWhiteBoardView->ChangeObj(xmlObj);		
	}
	else if (m_nType == BTN_TYPE_ZOOM) //缩放
	{
		m_bZoom = FALSE;

		if (m_pWBObject->GetType() == WB_OBJ_COMBINE)//如果是组合
		{
			((CWBCombine *)m_pWBObject)->GetSrcLinePoint();
		}

		if (m_pWBObject->GetType() == WB_OBJ_DOCUMENT)//如果是文档
		{
			((CWBDocument *)m_pWBObject)->GetSrcAnnPt();
		}
		if (m_pWBObject->GetType() == WB_OBJ_PICTURE)//如果是图片
		{
			((CWBPicture *)m_pWBObject)->GetSrcAnnPt();
		}
		if (m_pWBObject->GetType() == WB_OBJ_MEDIA)//如果是视频
		{
			if (m_bIsImageMove)
			{
				if (((CWBMedia*)m_pWBObject)->GetMediaPlayState() == Media_pausing
					&& ((CWBMedia*)m_pWBObject)->GetMediaWindow())
				{
					((CWBMedia*)m_pWBObject)->GetMediaWindow()->MediaPauseToPlay();
				}
				m_bIsImageMove = false;
			}
			else
			{
				//修改缩放弹起时候
				if (((CWBMedia*)m_pWBObject)->GetMediaPlayState() == Media_pausing)
				{
					((CWBMedia*)m_pWBObject)->MediaPlay();
					Sleep(1);
					((CWBMedia*)m_pWBObject)->MediaPause();
				}
				if (((CWBMedia*)m_pWBObject)->GetMediaPlayState() == Media_stop)
				{
					((CWBMedia*)m_pWBObject)->MediaPlay();
					Sleep(1);
					((CWBMedia*)m_pWBObject)->MediaStop();
				}
			}
		}

		//当对象缩放且旋转后，因坐标系变化原因会产生偏移，在这里纠正偏移
		if (m_pWBObject->GetIsZoom() && m_pWBObject->GetRotationAngle() != 0)
		{
			CPoint pt;			
			CWBSelectRectangle *pWBSelectRectangle = m_pWBObject->GetObjSelectFrame();

			m_pWBObject->GetCenterPoint(m_ptRotationCenter);
			if (pWBSelectRectangle != NULL)
			{
				pWBSelectRectangle->GetMapCenter(pt);
			}
			m_pWBObject->GetRotationCenter(m_ptRotationCenter);
			m_pWBObject->RotateByPoint(m_ptRotationCenter,0);

// 			if (m_pWBObject->GetType() == WB_OBJ_DOCUMENT)//如果是文档
// 			{
// 				CPoint ptAnn = -pt;
// 				((CWBDocument *)m_pWBObject)->MoveAnn(ptAnn);
// 			}
// 			if (m_pWBObject->GetType() == WB_OBJ_PICTURE)//如果是图片
// 			{
// 				CPoint ptAnn = -pt;
// 				((CWBPicture *)m_pWBObject)->MoveAnn(ptAnn);
// 			}
			m_pWBObject->SetIsZoom(FALSE);
		}
		if (m_pWBObject->GetType() == WB_OBJ_COMBINE)
		{
			XMemIni xmlObj;
			m_pWBObject->SetXmlValue(xmlObj);
			if (nPageIndex != 0)
			{
				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				g_pWhiteBoardView->SetPrePage(0);
			}
			g_pWhiteBoardView->ChangeObj(xmlObj);
			return; 
		}
		m_pWBObject->GetCenterPoint(m_ptRotationCenter);
		//滚动条偏移量
		UINT XnPos = g_pWhiteBoardView->getXnPos();
		UINT YnPos = g_pWhiteBoardView->getYnPos();
		m_ptRotationCenter.x -= XnPos;
		m_ptRotationCenter.y -= YnPos;

		if (m_pWBObject->GetType() == WB_OBJ_COMBINE)//如果是组合
		{
			((CWBCombine *)m_pWBObject)->GetSrcCenterPoint();

			m_pWBObject->GetRotationCenter(g_ptComRotCenter);
			m_pWBObject->RotateByCenter(0);	
		}
		if (m_pWBObject->GetType() == WB_OBJ_DOCUMENT)//如果是文档
		{
			((CWBDocument *)m_pWBObject)->GetSrcCenterPt();

			m_pWBObject->GetRotationCenter(g_ptDocRotCenter);
			m_pWBObject->RotateByPoint(g_ptDocRotCenter,0);
		}
		if (m_pWBObject->GetType() == WB_OBJ_PICTURE)//如果是图片
		{
			((CWBPicture *)m_pWBObject)->GetSrcCenterPt();

			m_pWBObject->GetRotationCenter(g_ptDocRotCenter);
			m_pWBObject->RotateByPoint(g_ptDocRotCenter,0);
		}
		//发送同步消息
		XMemIni xmlObj;
		m_pWBObject->SetXmlValue(xmlObj);
		if (nPageIndex != 0)
		{
			xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
			g_pWhiteBoardView->SetPrePage(0);
		}
		g_pWhiteBoardView->ChangeObj(xmlObj);
	}
	else if (m_nType == BTN_TYPE_MENU) //菜单
	{
		if (m_pWBObject != NULL)
		{
			//wangxiaobin

			int i = g_pWBPage->GetSelectObjLst()->GetSize();
			if (i == 1)//选中一个对象时
			{

				g_pObj = m_pWBObject;
			}
			/**************************************************************/
			DWORD dwLayerIndex = m_pWBObject->GetLayerIndex();
			if (!m_pWBObject->IsLock())
			{
				g_pWhiteBoardView->PostMessage(WM_POPMENU, dwLayerIndex, 0);
			}
			else
			{
				if (m_pWBObject->GetType() == WB_OBJ_LINE)
				{
					g_pWhiteBoardView->PostMessage(WM_POPMENU, dwLayerIndex, 0);
				}
			}
		}
	}
	else if (m_nType == BTN_TYPE_ROTATIONCENTER && m_pWBObject != NULL)
	{
		//CPoint pt,ptReserve;
		//pt = point;
		//m_pWBObject->GetRotationCenter(m_ptRotationCenter);
		//PointRotationPoint(m_ptRotationCenter, -m_pWBObject->GetRotationAngle(),pt,ptReserve);
		//m_ptMoveRotationCenter = ptReserve;
		//m_ptRotationCenter = point;
		//m_pWBObject->SetRotationCenter(point);
		//m_ptDrawRotationCenter = point;
		m_pWBObject->SetDrawRotationCenter(point);
		m_ptMoveRotationCenter = point;
		//((CWBCurve*)m_pWBObject)->SetIsDraw(TRUE);
		//((CWBCurve*)m_pWBObject)->CreateSelecrtFrame();
		m_bMoveRotionCenter = FALSE;
		m_pWBObject->SetIsCenterpoint(true);
		//m_pWBObject->SetRotationCenter(m_ptMoveRotationCenter);
		//CPoint ptcenter;
		//CPoint ptrocenter;
		//CPoint ptlefttop = (m_pWBObject->GetObjSelectFrame())->GetLeftTop();
		//m_pWBObject->GetCenterPoint(ptcenter);
		//m_pWBObject->GetRotationCenter(ptrocenter);
		//double angle = m_pWBObject->GetRotationAngle();
		//CPoint ptrolefttop,ptcelefttop;
		//PointRotationPoint(ptcenter,-angle,ptlefttop,ptcelefttop);
		//PointRotationPoint(ptrocenter,-angle,ptlefttop,ptrolefttop);
		//CPoint pt = ptrolefttop - ptcelefttop;
		//m_pWBObject->MoveOffset(pt);
		//m_pWBObject->RotateByPoint(ptrocenter,0);
		XMemIni xmlObj;
		m_pWBObject->SetXmlValue(xmlObj);
		if (nPageIndex != 0)
		{
			xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
			g_pWhiteBoardView->SetPrePage(0);
		}
		g_pWhiteBoardView->ChangeObj(xmlObj);
	}
}

void CWBImageButton::ButtonMove(CPoint point)
{
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	if (m_pWBObject == NULL)
	{
		return;
	}
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	int nPageIndex = g_pWhiteBoardView->GetPrePage();
	if (m_nType == BTN_TYPE_BEGIN || m_nType == BTN_TYPE_END)
	{
		if (m_pWBObject != NULL)
		{
			if (m_pWBObject->IsLock())//如果对象锁定
			{
				return;
			}

			CWBLine *pLine = (CWBLine *)m_pWBObject;
			CPoint ptOffset = point - m_ptZoomPre;


			if (m_nType == BTN_TYPE_BEGIN)
			{

				pLine->MoveOffset(ptOffset, TRUE);
			}
			else
			{
				pLine->MoveOffset(ptOffset, FALSE);
			}
			if (g_pWhiteBoardView->GetMoveIsNeedSendData())

			{			


				XMemIni xmlObj;
				m_pWBObject->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
				if (nPageIndex != 0)
				{
					xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
					g_pWhiteBoardView->SetPrePage(0);
				}

				g_pWhiteBoardView->Broadcast_Data(xmlObj);

				//	g_pWhiteBoardView->ChangeObj(xmlObj);
				g_pWhiteBoardView->SetMoveIsNeedSendData(FALSE);
			}

			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			} 
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
			m_ptZoomPre = point;
		}
	}

	if (m_nType == BTN_TYPE_ROTATION && m_bRotation && m_pWBObject != NULL) //旋转
	{
		if (m_pWBObject->IsLock()) //如果对象锁定
		{
			return;
		}


		m_bRotation = FALSE;

		/*CPoint ptRotationEnd;
		GetCursorPos(&ptRotationEnd);*/	
		m_pWBObject->GetRotationCenter(m_ptRotationCenter);
		double dbAbgleOffset = PointRotationPoint(m_ptRotationCenter, m_ptRotationPre, point);
		if (m_pWBObject->GetType() == WB_OBJ_COMBINE||m_pWBObject->GetType() == WB_OBJ_DOCUMENT ||m_pWBObject->GetType() == WB_OBJ_PICTURE)//如果是组合或文档
		{

			m_pWBObject->RotateByCenter(dbAbgleOffset);
		}
		else
		{
			//m_pWBObject->GetCenterPoint(m_ptRotationCenter);
			//m_pWBObject->SetRotationCenter(m_ptRotationCenter);
			m_pWBObject->RotateByPoint(m_ptRotationCenter,dbAbgleOffset);
		}


		//m_pWBObject->RotateByCenter(dbAbgleOffset);	
		if (g_pWhiteBoardView == NULL)
		{
			return;
		}
		if (g_pWhiteBoardView->GetMoveIsNeedSendData())
		{

			//发送同步消息
			XMemIni xmlObj;
			m_pWBObject->SetXmlValue(xmlObj);
			if (nPageIndex != 0)
			{

				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				g_pWhiteBoardView->SetPrePage(0);
			}

			xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
			g_pWhiteBoardView->Broadcast_Data(xmlObj);
			//	g_pWhiteBoardView->ChangeObj(xmlObj);
			g_pWhiteBoardView->SetMoveIsNeedSendData(FALSE);
		}
		if (CScreenDrawMgr::GetIns() != NULL)
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		} 
		else
		{
			g_pWhiteBoardView->Invalidate();
		}
		m_ptRotationPre = point;
		m_bRotation = TRUE;
		if (m_pWBObject->GetType() == WB_OBJ_COMBINE)
		{
			XMemIni xmlObj;
			if (nPageIndex != 0)
			{
				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				g_pWhiteBoardView->SetPrePage(0);
			}
			m_pWBObject->SetXmlValue(xmlObj);
			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
	}
	else if (m_nType == BTN_TYPE_ZOOM && m_bZoom && m_pWBObject != NULL) //缩放
	{
		if (m_pWBObject->IsLock()) //如果对象锁定
		{			
			return;
		}


		CPoint ptZoom = point;
		//replaced by 陈常涛 begin
		//befor replaced 
		/*g_pWhiteBoardView->ClientToScreen(&ptZoom);*/
		//after replaced
		if(CScreenDrawMgr::GetIns() != NULL) //如果是桌面绘图，则进行桌面窗口坐标转换，否则进行视图窗口坐标转换
		{
			CScreenDrawMgr::GetIns()->GetMouseDlgScreenPosition(ptZoom);
		}
		else
		{
			g_pWhiteBoardView->ClientToScreen(&ptZoom);
		}
		//replaced by 陈常涛 end
		CPoint ptOffset;		


		//添加旋转特性
		CPoint ptRotationZoom, ptRotationZoomPre, ptRotationLeftTop;
		m_pWBObject->ReservePoint(ptZoom, ptRotationZoom);
		m_pWBObject->ReservePoint(m_ptZoomPre, ptRotationZoomPre);

		ptOffset = ptRotationZoom - ptRotationZoomPre;		

		int nWidth = 0;
		int nHeight = 0;
		CWBSelectRectangle *pSelectRectangle = m_pWBObject->GetObjSelectFrame();
		if (pSelectRectangle != NULL)
		{
			nWidth = pSelectRectangle->Width();
			nHeight = pSelectRectangle->Height();
		}
		else
		{
#ifdef _DEBUG

			assert(false);
#endif

		}


		//多线程造成的必要检查，需使用lock从根本上解决此问题
		//jiabangping 20121206
		if ((nWidth <= 0) || (nHeight <= 0))
		{
			return;
		}


		//如果是同比例缩放
		if (m_pWBObject->IsEqualScaleZoom())
		{
			double fWnidthHeightScale = m_pWBObject->GetEqualScale(); //对象宽度与长度比例
			double dbTemp;

			if (abs(ptOffset.x) > abs(ptOffset.y))
			{

				if (m_pWBObject->GetType() == WB_OBJ_ANNOTATIONS || m_pWBObject->GetType() == WB_OBJ_TEXT)//如果是标注或文本
				{

					dbTemp = ptOffset.x * nHeight / nWidth;
				}
				else
				{

					dbTemp = ptOffset.x / fWnidthHeightScale;
				}
				ptOffset.y = RoundingOnePt(dbTemp, FALSE);


				if ((m_pWBObject->GetType() == WB_OBJ_TEXT && ((CWBText *)m_pWBObject)->GetIsVertical())
					|| (m_pWBObject->GetType() == WB_OBJ_ANNOTATIONS && (((CWBAnnotations *)m_pWBObject)->GetText())->GetIsVertical()))
				{
					ptOffset.y = -ptOffset.y;
				}
			}

			else
			{

				if (m_pWBObject->GetType() == WB_OBJ_ANNOTATIONS || m_pWBObject->GetType() == WB_OBJ_TEXT)//如果是标注或文本
				{

					dbTemp = ptOffset.y * nWidth / nHeight;
				}

				else
				{

					dbTemp = ptOffset.y * fWnidthHeightScale;
				}
				ptOffset.x = RoundingOnePt(dbTemp, TRUE);


				if ((m_pWBObject->GetType() == WB_OBJ_TEXT && ((CWBText *)m_pWBObject)->GetIsVertical())
					|| (m_pWBObject->GetType() == WB_OBJ_ANNOTATIONS && (((CWBAnnotations *)m_pWBObject)->GetText())->GetIsVertical()))
				{

					ptOffset.x = -ptOffset.x;
				}

			}
		}

		//限制缩放最小值 wangxin 2011-01-20
		if (m_pWBObject->GetType() == WB_OBJ_FLASH || m_pWBObject->GetType() == WB_OBJ_MOVIE
			|| m_pWBObject->GetType() == WB_OBJ_DOCUMENT || m_pWBObject->GetType() == WB_OBJ_CAMERA)
		{
			int min = 250;
			switch (m_pWBObject->GetType())
			{

			case WB_OBJ_DOCUMENT:
				min = 290;
				break;
			default:
				break;
			}
			//如果是同比例缩放
			if (m_pWBObject->IsEqualScaleZoom())
			{
				if (nWidth + ptOffset.x < min || nHeight + ptOffset.y < min)
				{

					return;
				}
			}


			if (ptOffset.x < 0 && nWidth + ptOffset.x < min)
			{
				ptOffset.x = -(nWidth - min);
			}
			if (ptOffset.y < 0 && nHeight + ptOffset.y < min)
			{
				ptOffset.y = -(nHeight - min);
			}
		} 
		else
		{
			int nMin = 30;
			if (m_pWBObject->GetType() == WB_OBJ_TEXT)
			{
				nMin = 20;
			}
			if (m_pWBObject->GetType() == WB_OBJ_COMBINE)
			{
				nMin = 50;
			}
			if (m_pWBObject->GetType() == WB_OBJ_PICTURE)
			{
				nMin = 100;
				int nRealWidth = ((CWBPicture *)m_pWBObject)->GetWidth();
				int nRealHeight = ((CWBPicture *)m_pWBObject)->GetHeight();
				nMin = nMin > nRealWidth ? nRealWidth : nMin;
				nMin = nMin > nRealHeight ? nRealHeight : nMin;
			}
			if (m_pWBObject->GetType() == WB_OBJ_RECT || m_pWBObject->GetType() == WB_OBJ_TRIANGLE || m_pWBObject->GetType() == WB_OBJ_RHOMBUS)
			{
				((CWBPolygon *)m_pWBObject)->GetItemInfo(nWidth, nHeight);
			}
			else if (m_pWBObject->GetType() == WB_OBJ_ELLIPSE)
			{
				int nLineWidth = ((CWBEllipse *)m_pWBObject)->GetLineWidth();
				nMin += nLineWidth;
			}


			//如果是同比例缩放
			if (m_pWBObject->IsEqualScaleZoom()&&m_pWBObject->GetType() != WB_OBJ_TEXT&&m_pWBObject->GetType() != WB_OBJ_ANNOTATIONS)
			{
				//double WnidthHeightScale = m_pWBObject->GetEqualScale(); //对象宽度与长度比例

				if (nWidth + ptOffset.x < nMin || nHeight + ptOffset.y < nMin)
				{

					return;
				}

			}

			if ((m_pWBObject->GetType() == WB_OBJ_TEXT && ((CWBText *)m_pWBObject)->GetIsVertical())
				|| (m_pWBObject->GetType() == WB_OBJ_ANNOTATIONS && (((CWBAnnotations *)m_pWBObject)->GetText())->GetIsVertical()))
			{
				int nMin;
				nMin = 20;
				if (nWidth - ptOffset.x < nMin )
				{
					return;
				}
				if(nHeight + ptOffset.y < nMin)
				{
			        return;
				}

			}
			else
			{
				int nMin;
				nMin = 20;
				if (nWidth + ptOffset.x < nMin)
				{
					return;
				}
				if (nHeight + ptOffset.y < nMin)
				{
					return;
				}
			}

			if ((m_pWBObject->GetType() == WB_OBJ_TEXT && ((CWBText *)m_pWBObject)->GetIsVertical())
				|| (m_pWBObject->GetType() == WB_OBJ_ANNOTATIONS && (((CWBAnnotations *)m_pWBObject)->GetText())->GetIsVertical()))
			{
				if (ptOffset.x > 0 && nWidth - ptOffset.x < nMin)
				{

					ptOffset.x = nWidth - nMin;
				}
			} 
			else
			{
				if (ptOffset.x < 0 && nWidth + ptOffset.x < nMin)
				{
					ptOffset.x = -(nWidth - nMin);
				}
			}

			if (m_pWBObject->GetType()== WB_OBJ_DXF) //add by jiangchao 2013.08.23
			{
				int xmin=600;
				int ymin=400;
				if (ptOffset.x < 0 && nWidth + ptOffset.x < xmin)
				{
					ptOffset.x = -(nWidth - xmin);
				}
				if (ptOffset.y < 0 && nHeight + ptOffset.y < ymin)
				{
					ptOffset.y = -(nHeight - ymin);
				}
				if (m_pWBObject->IsEqualScaleZoom()&&(ptOffset.x==0||ptOffset.y==0))
				{
					ptOffset.x=ptOffset.y=0;
				}
			}

			if (ptOffset.y <0 && nHeight + ptOffset.y <nMin)
			{
				ptOffset.y = -(nHeight - nMin);
			}
		}


		m_ptZoomPre = ptZoom;

		if (m_pWBObject->GetType() == WB_OBJ_COMBINE || m_pWBObject->GetType() == WB_OBJ_DOCUMENT || m_pWBObject->GetType() == WB_OBJ_PICTURE)//如果是组合或者文档或者图片
		{
			//得到选择框数据
			if (m_pWBObject->GetObjSelectFrame() == NULL)
			{

				m_pWBObject->CreateSelecrtFrame();
			}


			int nWidth1, nHeight1; //原选择框的宽度与高度
			int nWidth2, nHeight2; //缩放后的选择框宽度与高度
			CPoint ptLeftTop, ptRightBottom1; //原选择框的左上角与右下角坐标
			CPoint ptRightBottom2; //缩放后的选择框右下角坐标


			ptLeftTop = (m_pWBObject->GetObjSelectFrame())->GetLeftTop();
			ptRightBottom1 = (m_pWBObject->GetObjSelectFrame())->GetRightBottom();
			ptRightBottom2 = ptRightBottom1 + ptOffset;


			nWidth1 = ptRightBottom1.x - ptLeftTop.x;
			nHeight1 = ptRightBottom1.y - ptLeftTop.y;
			nWidth2 = ptRightBottom2.x - ptLeftTop.x;
			nHeight2 = ptRightBottom2.y - ptLeftTop.y;


			if (nWidth2 == 0 || nHeight2 == 0)
			{
				return;
			}


			if (m_pWBObject->GetType() == WB_OBJ_COMBINE)
			{
				//计算本次缩放比例
				g_dbXScale = (double)nWidth2 / nWidth1;
				g_dbYScale = (double)nHeight2 / nHeight1;


				((CWBCombine *)m_pWBObject)->GetEachSrcZoomCenter();
			} 
			else if (m_pWBObject->GetType() == WB_OBJ_DOCUMENT)
			{
				//计算本次缩放比例
				g_dbDocXScale = (double)nWidth2 / nWidth1;
				g_dbDocYScale = (double)nHeight2 / nHeight1;


				((CWBDocument *)m_pWBObject)->GetEachSrcZoomCenterPt();
			}
			else
			{
				//计算本次缩放比例
				g_dbDocXScale = (double)nWidth2 / nWidth1;
				g_dbDocYScale = (double)nHeight2 / nHeight1;


				((CWBPicture *)m_pWBObject)->GetEachSrcZoomCenterPt();
			}
		}

		if (m_pWBObject->GetType() == WB_OBJ_MEDIA
			&& ((CWBMedia*)m_pWBObject)->GetMediaPlayState() == Media_playing)//如果是视频
		{
			if (((CWBMedia*)m_pWBObject)->GetMediaWindow())
			{

				((CWBMedia*)m_pWBObject)->GetMediaWindow()->MediaPlayToPause();
			}
			m_bIsImageMove = true;
		}

		if (m_pWBObject->GetType() == WB_OBJ_RULER)
		{
			int max = 1000;
			int min = 500;
			if (nWidth + ptOffset.x <min ||nWidth + ptOffset.x >max)
			{
				return;
			}
		}

		BOOL bResult = m_pWBObject->ZoomByButton(ptOffset);



		if (m_pWBObject->GetType() == WB_OBJ_COMBINE)//如果是组合
		{
			if (bResult)
			{

				//缩放选择框
				(m_pWBObject->GetObjSelectFrame())->ZoomByButton(ptOffset);
			}
		}

		if (m_pWBObject->GetType() == WB_OBJ_DXF)//判断是否重画DXF
		{
			((CWBDxf *)m_pWBObject)->SetIsNeedReDraw();
		}

		m_pWBObject->SetIsZoom(TRUE);


		if (CScreenDrawMgr::GetIns() != NULL)
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		} 
		else
		{
			g_pWhiteBoardView->Invalidate();
		}
		if (g_pWhiteBoardView->GetMoveIsNeedSendData())
		{			
			//发送同步消息
			XMemIni xmlObj;
			m_pWBObject->SetXmlValue(xmlObj);
			if (nPageIndex != 0)
			{

				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				g_pWhiteBoardView->SetPrePage(0);
			}
			xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
			g_pWhiteBoardView->Broadcast_Data(xmlObj);
			/*			g_pWhiteBoardView->ChangeObj(xmlObj);*/
			g_pWhiteBoardView->SetMoveIsNeedSendData(FALSE);
		}
		if (m_pWBObject->GetType() == WB_OBJ_COMBINE)
		{
			XMemIni xmlObj;
			if (nPageIndex != 0)
			{
				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				g_pWhiteBoardView->SetPrePage(0);
			}
			m_pWBObject->SetXmlValue(xmlObj);
			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		if (CScreenDrawMgr::GetIns() &&CScreenDrawMgr::GetIns()->GetMoveIsNeedSendData())
		{			
			//发送同步消息
			XMemIni xmlObj;
			m_pWBObject->SetXmlValue(xmlObj);
			if (nPageIndex != 0)
			{

				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				g_pWhiteBoardView->SetPrePage(0);
			}

			xmlObj.Set(KEY_EVENT_ID,GenerateSSRC());
			xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
			g_pWhiteBoardView->Broadcast_Data(xmlObj);
			//	g_pWhiteBoardView->ChangeObj(xmlObj);

			CScreenDrawMgr::GetIns()->SetMoveIsNeedSendData(FALSE);
		}
	}
	else if (m_nType == BTN_TYPE_ROTATIONCENTER && m_bMoveRotionCenter && m_pWBObject != NULL)
	{
		if (m_pWBObject->IsLock())
		{
			return ;
		}
		m_bMoveRotionCenter = FALSE;
		CPoint pt,ptReserve;
		pt = point;

		/*m_pWBObject->GetRotationCenter(m_ptRotationCenter);*/
		//PointRotationPoint(m_ptRotationCenter, -m_pWBObject->GetRotationAngle(),pt,ptReserve);
		//m_ptRotationCenter = point;
		/*m_pWBObject->SetRotationCenter(m_ptRotationCenter);*/
		/*m_bCenterMove = true;*/
		m_ptMoveRotationCenter = point;
		m_pWBObject->SetDrawRotationCenter(point);
		//CPoint DrawPt = point;
		//m_pWBObject->SetDrawRotationCenter(DrawPt);
		//m_ptDrawRotationCenter = point;
		m_bMoveRotionCenter =TRUE;
		if (g_pWhiteBoardView == NULL)
		{
			return;
		}
		if (CScreenDrawMgr::GetIns() != NULL)
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			g_pWhiteBoardView->Invalidate();
		}



		if (g_pWhiteBoardView->GetMoveIsNeedSendData())
		{			
			//发送同步消息
			XMemIni xmlObj;
			m_pWBObject->SetXmlValue(xmlObj);
			if (nPageIndex != 0)
			{
				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				g_pWhiteBoardView->SetPrePage(0);
			}
			xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);

			g_pWhiteBoardView->Broadcast_Data(xmlObj);
			/*			g_pWhiteBoardView->ChangeObj(xmlObj);*/
			g_pWhiteBoardView->SetMoveIsNeedSendData(FALSE);
		}
	}


}

void CWBImageButton::Draw(CDC *pDC)
{
	if (pDC == NULL || m_pWBObject == NULL)
	{
		return;
	}


	if (m_pWBObject->IsCombineMember()) //如果是组合的成员
	{
		return;
	}

	HDC hdc = pDC->GetSafeHdc();
	m_pDC = pDC;
	//旋转坐标系
	int nGraphicsMode = RotateXForm(m_pWBObject->GetRotationAngle());
	BOOL bTrue = g_pWhiteBoardView->bIsInCamInfoMap(m_pWBObject); //查找是否是拖拽的图片
	if (bTrue)
	{
		return;
	}
	if (m_pWBObject->GetType() == WB_OBJ_LINE)
	{
		CRect rect;
		WBLineItem ObjectItem;

		((CWBLine *)m_pWBObject)->GetItem(&ObjectItem);

		if (m_pWBObject->IsLock())
		{
			//设置直线锁定按钮的位置并画出
			rect.top = (ObjectItem.ptBegin.y + ObjectItem.ptEnd.y) / 2 - BUTTONSIZE / 2;
			rect.left = (ObjectItem.ptBegin.x + ObjectItem.ptEnd.x) / 2- BUTTONSIZE / 2;
			rect.bottom = rect.top + BUTTONSIZE;
			rect.right = rect.left + BUTTONSIZE;
			m_pImageLock->Draw(hdc,rect);
		}
		else
		{
			//设置直线起点按钮的位置并画出
			rect.top = ObjectItem.ptBegin.y - BUTTONSIZE / 2;
			rect.left = ObjectItem.ptBegin.x - BUTTONSIZE / 2;
			rect.bottom = rect.top + BUTTONSIZE;
			rect.right = rect.left + BUTTONSIZE;

			m_pImageZoom->Draw(hdc,rect);

			//设置直线菜单按钮的位置并画出
			rect.top = (ObjectItem.ptBegin.y + ObjectItem.ptEnd.y) / 2 - BUTTONSIZE / 2;
			rect.left = (ObjectItem.ptBegin.x + ObjectItem.ptEnd.x) / 2- BUTTONSIZE / 2;
			rect.bottom = rect.top + BUTTONSIZE;
			rect.right = rect.left + BUTTONSIZE;
			if (m_pWBObject->IsInfiniteClone())
			{
				m_pImageClone->Draw(hdc,rect);
			} 
			else
			{
				if(!g_bSupportI7)
				{
					m_pImageMenu->Draw(hdc,rect);
				}
				
			}

			//设置直线终点按钮的位置并画出
			rect.top = ObjectItem.ptEnd.y - BUTTONSIZE / 2;
			rect.left = ObjectItem.ptEnd.x - BUTTONSIZE / 2;
			rect.bottom = rect.top + BUTTONSIZE;
			rect.right = rect.left + BUTTONSIZE;	
			m_pImageZoom->Draw(hdc,rect);
		}
	}
	else if (m_pWBObject->GetType() == WB_OBJ_MEDIA || m_pWBObject->GetType() == WB_OBJ_CAMERA)
	{
		CRect rect;

		if (m_pWBObject->GetObjSelectFrame() == NULL)
		{
			m_pWBObject->CreateSelecrtFrame();
		}

		CPoint ptLeftTop = (m_pWBObject->GetObjSelectFrame())->GetLeftTop();
		CPoint ptRightTop = (m_pWBObject->GetObjSelectFrame())->GetRightTop();
		CPoint ptRightBottom = (m_pWBObject->GetObjSelectFrame())->GetRightBottom();

		//设置锁定按钮图片的位置
		rect.top = (LONG)(ptLeftTop.y - BUTTONSIZE/1.1) ;
		rect.left = (LONG)(ptLeftTop.x - BUTTONSIZE/1.1);
		rect.bottom = rect.top + BUTTONSIZE;
		rect.right = rect.left + BUTTONSIZE;

		if (m_pWBObject->IsLock())
		{
			m_pImageLock->Draw(hdc,rect);
		}
		else
		{
			m_pImageUnLock->Draw(hdc,rect);
			//设置菜单按钮图片的位置
			rect.top = (LONG)(ptRightTop.y - BUTTONSIZE*0.88) ;
			rect.left = ptRightTop.x - 5;
			rect.bottom = rect.top + BUTTONSIZE;
			rect.right = rect.left + BUTTONSIZE;
			if (m_pWBObject->IsInfiniteClone())
			{
				m_pImageClone->Draw(hdc,rect);
			} 
			else
			{
				if (m_pWBObject->GetType() != WB_OBJ_CAMERA)
				{
					if(!g_bSupportI7)
					{
						m_pImageMenu->Draw(hdc,rect);
					}
				}
				
			}
			//设置缩放按钮图片的位置
			rect.top = ptRightBottom.y - BUTTONSIZE / 4;
			rect.left = ptRightBottom.x - BUTTONSIZE / 4;
			rect.bottom = rect.top + BUTTONSIZE;
			rect.right = rect.left + BUTTONSIZE;
			m_pImageZoom->Draw(hdc,rect);

			//设置标注尖角图片的位置
			if (m_pWBObject->GetType() == WB_OBJ_ANNOTATIONS)
			{
				CPoint ptChange;
				ptChange = ((CWBAnnotations *)m_pWBObject)->GetChangePoint();
				rect.top = ptChange.y - BUTTONSIZE / 4;
				rect.left = ptChange.x - BUTTONSIZE / 4;
				rect.bottom = rect.top + BUTTONSIZE / 2;
				rect.right = rect.left + BUTTONSIZE / 2;
				m_pImageAnnotations->Draw(hdc,rect);
			}
		}
	}
	else
	{
		CRect rect;

		if (m_pWBObject->GetObjSelectFrame() == NULL)
		{
			m_pWBObject->CreateSelecrtFrame();
		}

		CPoint ptLeftTop = (m_pWBObject->GetObjSelectFrame())->GetLeftTop();
		CPoint ptRightTop = (m_pWBObject->GetObjSelectFrame())->GetRightTop();
		CPoint ptRightBottom = (m_pWBObject->GetObjSelectFrame())->GetRightBottom();

		if (m_pWBObject->IsLock())
		{
			//设置锁定按钮图片的位置
			rect.top = ptLeftTop.y - BUTTONSIZE / 2;
			rect.left = ptLeftTop.x - BUTTONSIZE / 2;
			rect.bottom = rect.top + BUTTONSIZE;
			rect.right = rect.left + BUTTONSIZE;
			m_pImageLock->Draw(hdc,rect);
		}
		else
		{
			//设置旋转按钮图片的位置
			rect.top = ptLeftTop.y - BUTTONSIZE + 5;
			rect.left = (ptLeftTop.x + ptRightTop.x) / 2 - BUTTONSIZE / 2;
			rect.bottom = rect.top + BUTTONSIZE;
			rect.right = rect.left + BUTTONSIZE;
			CRect RotationRect = rect;
			if (m_pWBObject->GetType() != WB_OBJ_ANNOTATIONS&&m_pWBObject->GetType() != WB_OBJ_MIDI)	//标注不画旋转按钮
			{
				m_pImageRotation->Draw(hdc,rect);
			}

			//设置锁定按钮图片的位置
			rect.top = ptLeftTop.y - BUTTONSIZE / 2;
			rect.left = ptLeftTop.x - BUTTONSIZE / 2;
			rect.bottom = rect.top + BUTTONSIZE;
			rect.right = rect.left + BUTTONSIZE;
			m_pImageUnLock->Draw(hdc,rect);

			//设置菜单按钮图片的位置
			rect.top = ptRightTop.y - BUTTONSIZE / 2;
			rect.left = ptRightTop.x - BUTTONSIZE / 2;
			rect.bottom = rect.top + BUTTONSIZE;
			rect.right = rect.left + BUTTONSIZE;
			if (m_pWBObject->IsInfiniteClone())
			{
				m_pImageClone->Draw(hdc,rect);
			} 
			else
			{
				if(!g_bSupportI7)
				{
					m_pImageMenu->Draw(hdc,rect);
				}
			}

			//设置缩放按钮图片的位置
			if (m_pWBObject->GetType() != WB_OBJ_ARBITRARYTRIANGLE&&m_pWBObject->GetType() != WB_OBJ_MIDI)
			{
				rect.top = ptRightBottom.y - BUTTONSIZE / 2;
				if (rect.top - BUTTONSIZE < RotationRect.top)
					rect.left = ptRightBottom.x - BUTTONSIZE / 2;
				rect.bottom = rect.top + BUTTONSIZE;
				rect.right = rect.left + BUTTONSIZE;
				m_pImageZoom->Draw(hdc,rect);
			}

			//设置标注尖角图片的位置
			if (m_pWBObject->GetType() == WB_OBJ_ANNOTATIONS)
			{
				CPoint ptChange;
				ptChange = ((CWBAnnotations *)m_pWBObject)->GetChangePoint();
				rect.top = ptChange.y - BUTTONSIZE / 4;
				rect.left = ptChange.x - BUTTONSIZE / 4;
				rect.bottom = rect.top + BUTTONSIZE / 2;
				rect.right = rect.left + BUTTONSIZE / 2;
				m_pImageAnnotations->Draw(hdc,rect);
			}
		}
	}

	//恢复坐标系
	if (nGraphicsMode >= 0)
	{
		ResumeXForm(nGraphicsMode);
	}	

	BOOL bisLock =m_pWBObject->IsLock();
	if (m_pWBObject->GetType() != WB_OBJ_MIDI&&m_pWBObject->GetType() != WB_OBJ_ANNOTATIONS&&m_pWBObject->GetType() != WB_OBJ_LINE&&!bisLock &&m_pWBObject->GetType() != WB_OBJ_MEDIA)
	{
		CRect rc = CRect(0,0,0,0);
		if(m_pWBObject->IsRorationCenterZoom() == TRUE)
		{
			CPoint DrawPt;
			m_pWBObject->GetDrawRotationCenter(DrawPt);
			if (DrawPt == CPoint(0,0))
			{
				m_pWBObject->GetCenterPoint(DrawPt);
				m_pWBObject->SetDrawRotationCenter(DrawPt);
			}
			rc.left = DrawPt.x - 8.5;
			rc.top = DrawPt.y - 7.5;
			rc.right = rc.left + 17;
			rc.bottom = rc.top + 15;
			m_pImageRotationCenter->Draw(hdc,rc);


		}
	}



}

BOOL CWBImageButton::IsInButtonArea(const CPoint& pt)
{
	if (m_pWBObject == NULL)
	{
		return FALSE;
	}

	if (m_pWBObject->GetType() == WB_OBJ_LINE)
	{
		//因为只保存了2个图片的位置，没有保存起点按钮的位置，所以先计算出起点按钮的位置
		CRect rect;
		WBLineItem ObjectItem;

		((CWBLine *)m_pWBObject)->GetItem(&ObjectItem);

		rect.top = ObjectItem.ptBegin.y - BUTTONSIZE / 2;
		rect.left = ObjectItem.ptBegin.x - BUTTONSIZE / 2;
		rect.bottom = rect.top + BUTTONSIZE;
		rect.right = rect.left + BUTTONSIZE;	
		//判断点是否在图片的区域内
		if (rect.PtInRect(pt))
		{
			m_nType = BTN_TYPE_BEGIN;
			return TRUE;
		}
		if (m_pImageZoom->m_rect.PtInRect(pt))
		{
			m_nType = BTN_TYPE_END;
			return TRUE;
		}
		if (m_pWBObject->IsInfiniteClone())
		{
			if (m_pImageClone->m_rect.PtInRect(pt))
			{
				m_nType = BTN_TYPE_MENU;
				return TRUE;
			}
		} 
		else if (m_pWBObject->IsLock())
		{
			if (m_pImageLock->m_rect.PtInRect(pt))
			{
				m_nType = BTN_TYPE_MENU;
				return TRUE;
			}
		}
		else
		{			
			if (m_pImageMenu->m_rect.PtInRect(pt))
			{
				if(!g_bSupportI7)
				{
					m_nType = BTN_TYPE_MENU;
					return TRUE;
				}
				return FALSE;
			}
		}
	}
	else
	{
		CPoint ptReserve, point, ptParentCenter;
		point = pt;
		//根据对象的旋转角度，换算鼠标击中的点
		m_pWBObject->GetRotationCenter(m_ptRotationCenter);
		PointRotationPoint(m_ptRotationCenter, -m_pWBObject->GetRotationAngle(), point, ptReserve);

		if (m_pImageRotationCenter->m_rect.PtInRect(point))
		{

			BOOL bisLock =m_pWBObject->IsLock();
			if (m_pWBObject->GetType() != WB_OBJ_ANNOTATIONS&&m_pWBObject->GetType() != WB_OBJ_LINE&&!bisLock)
			{
				if(m_pWBObject->IsRorationCenterZoom() == TRUE)
				{
					m_nType = BTN_TYPE_ROTATIONCENTER;
					return TRUE;
				}
			}
			else
			{
				return FALSE;
			}
			return FALSE;
		}
		//判断点是否在图片的区域内
		if (m_pWBObject->IsLock())
		{
			if (m_pImageLock->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_LOCK;
				return TRUE;
			}
		} 
		else
		{			
			if (m_pImageUnLock->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_LOCK;
				return TRUE;
			}
		}

		if (m_pImageZoom->m_rect.PtInRect(ptReserve))
		{
			m_nType = BTN_TYPE_ZOOM;
			return TRUE;
		}
		if (m_pImageRotation->m_rect.PtInRect(ptReserve))
		{
			m_nType = BTN_TYPE_ROTATION;
			return TRUE;
		}
		
		if (m_pWBObject->IsInfiniteClone())
		{
			if (m_pImageClone->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_MENU;
				return TRUE;
			}
		} 
		else
		{			
			if (m_pImageMenu->m_rect.PtInRect(ptReserve))
			{
				if(!g_bSupportI7)
				{
					m_nType = BTN_TYPE_MENU;
					return TRUE;
				}
				return FALSE;
			}
		}
	}
	return FALSE;
}

int CWBImageButton::RotateXForm(double dbAngle)
{
	if (m_pDC == NULL || dbAngle == 0)
	{
		return -1;
	}

	HDC hDC = m_pDC->GetSafeHdc();

	//取对象旋转中心点
	CPoint ptCenter;
	m_pWBObject->GetRotationCenter(ptCenter);

	//旋转
	int nGraphicsMode = SetGraphicsMode(hDC, GM_ADVANCED);

	XFORM xform;
	double fangle = -dbAngle / 180. * PI;

	//加缩放因子
	xform.eM11 = (float)cos(fangle);
	xform.eM12 = (float)sin(fangle); 
	xform.eM21 = (float)-sin(fangle);
	xform.eM22 = (float)cos(fangle);
	xform.eDx = (float)(ptCenter.x - cos(fangle)*ptCenter.x + sin(fangle)*ptCenter.y);
	xform.eDy = (float)(ptCenter.y - cos(fangle)*ptCenter.y - sin(fangle)*ptCenter.x);

	SetWorldTransform(hDC, &xform); 

	return nGraphicsMode;
}

void CWBImageButton::ResumeXForm(int nGraphicsMode)
{
	if (m_pDC == NULL)
	{
		return;
	}

	HDC hDC = m_pDC->GetSafeHdc();

	// 恢复DC	
	XFORM xform;

	xform.eM11 = (float)1.0; 
	xform.eM12 = (float)0;
	xform.eM21 = (float)0;
	xform.eM22 = (float)1.0;
	xform.eDx = (float)0;
	xform.eDy = (float)0;            
	SetWorldTransform(hDC, &xform);

	SetGraphicsMode(hDC, nGraphicsMode);
}

void CWBImageButton::ClearImage()
{
	if (m_pImageLock != NULL)
	{
		delete m_pImageLock;
		m_pImageLock = NULL;
	}
	if (m_pImageUnLock != NULL) 
	{
		delete m_pImageUnLock;
		m_pImageUnLock = NULL;
	}
	if (m_pImageRotation != NULL)
	{
		delete m_pImageRotation;
		m_pImageRotation = NULL;
	}
	if (m_pImageMenu != NULL)
	{
		delete m_pImageMenu;
		m_pImageMenu = NULL;
	}
	if (m_pImageZoom != NULL)
	{
		delete m_pImageZoom;
		m_pImageZoom = NULL;
	}
	if (m_pImageClone != NULL)
	{
		delete m_pImageClone;
		m_pImageClone = NULL;
	}
	if (m_pImageAnnotations != NULL)
	{
		delete m_pImageAnnotations;
		m_pImageAnnotations = NULL;
	}
	if (m_pImageRotationCenter != NULL)
	{
		delete m_pImageRotationCenter;
		m_pImageRotationCenter = NULL;
	}
}

CPoint CWBImageButton::GetMenuPt()
{
	if (m_pWBObject->IsInfiniteClone())
	{
		CPoint point;
		point = m_pImageClone->m_rect.CenterPoint();

		//根据对象的旋转角度，换算鼠标击中的点
		m_pWBObject->GetRotationCenter(m_ptRotationCenter);
		PointRotationPoint(m_ptRotationCenter, m_pWBObject->GetRotationAngle(), point, m_ptMenu);
	} 
	else if (m_pWBObject->IsLock())
	{
		CPoint point;
		point = m_pImageLock->m_rect.CenterPoint();

		//根据对象的旋转角度，换算鼠标击中的点
		m_pWBObject->GetRotationCenter(m_ptRotationCenter);
		PointRotationPoint(m_ptRotationCenter, m_pWBObject->GetRotationAngle(), point, m_ptMenu);
	}
	else
	{
		CPoint point;
		point = m_pImageMenu->m_rect.CenterPoint();

		//根据对象的旋转角度，换算鼠标击中的点
		m_pWBObject->GetRotationCenter(m_ptRotationCenter);
		PointRotationPoint(m_ptRotationCenter, m_pWBObject->GetRotationAngle(), point, m_ptMenu);
	}
	return m_ptMenu;
}

void CWBImageButton::setOffsetCenter( CPoint ptoffset )
{
	//CPoint ptCenter = CPoint(0,0);
	//m_pWBObject->GetRotationCenter(ptCenter);
	//if (ptCenter != m_ptDrawRotationCenter)
	//{
	//	m_ptDrawRotationCenter = ptCenter;
	//}

	m_ptOffsetCenter = ptoffset;
	if (m_pWBObject->GetIsDoInside() != false)
	{
		m_ptOffsetCenter = CPoint(0,0);
		m_pWBObject->SetIsDocInside(false);
	} 
	CPoint DrawPt;
	m_pWBObject->GetDrawRotationCenter(DrawPt);
	//m_ptDrawRotationCenter += m_ptOffsetCenter;
	DrawPt += m_ptOffsetCenter;
	m_ptcenterone = DrawPt;
	m_ptOffsetCenter = CPoint(0,0);
	m_pWBObject->SetDrawRotationCenter(DrawPt);
}

void CWBImageButton::ChangeRationCenterOperator()
{




	m_bRotation = TRUE;	
	CPoint ptTempCenter;
	m_pWBObject->GetRotationCenter(ptTempCenter);
	CPoint DrawPt;
	m_pWBObject->GetDrawRotationCenter(DrawPt);
	if (DrawPt != ptTempCenter)
	{
		m_bCenterMove = true;
		m_pWBObject->SetRotationCenter(DrawPt);
	}




	CPoint ptcentertwo;
	m_pWBObject->GetRotationCenter(ptcentertwo);
	m_pWBObject->GetPreRotationCenter(m_ptcenterone);
	if (m_bCenterMove == true && ptcentertwo!=m_ptcenterone)
	{
		CPoint ptlefttop = (m_pWBObject->GetObjSelectFrame())->GetLeftTop();
		double angle = m_pWBObject->GetRotationAngle();
		CPoint ptrolefttop,ptcelefttop;
		PointRotationPoint(m_ptcenterone,-angle,ptlefttop,ptcelefttop);
		PointRotationPoint(ptcentertwo,-angle,ptlefttop,ptrolefttop);
		CPoint pt = ptrolefttop - ptcelefttop;
		m_pWBObject->MoveOffset(pt);
		m_pWBObject->RotateByPoint(ptcentertwo,0);
		m_ptcenterone = ptcentertwo;
		m_bCenterMove = false;
	}
	m_pWBObject->GetRotationCenter(m_ptObjCenter);
}

void CWBImageButton::SetObjRationCenterpt( CPoint pt )
{
	//m_ptDrawRotationCenter = pt;
	m_pWBObject->SetDrawRotationCenter(pt);
	m_pWBObject->SetRotationCenter(pt);
}

void CWBImageButton::GetObjRationCenterPt()
{   
	CPoint DrawPt = CPoint(0,0);
	m_pWBObject->GetRotationCenter(DrawPt);
	m_pWBObject->SetDrawRotationCenter(DrawPt);
}



/*-----------------------------------------------------------------------------------
// CWBImageButton
------------------------------------------------------------------------------------*/



/************************************************************************/
/*				          CWBControlButton类							*/
/*					      生成对象的控制按钮							*/
/*                        视频、FLASH、挡板和文档								*/
/************************************************************************/
CWBControlButton::CWBControlButton(CWBObject *pWBObject)
{
	m_pWBObject = pWBObject;
	m_nType = 0;
	m_bIsButtonDown = FALSE;
	m_pDC = NULL;

	m_pImageBackground = NULL;
	m_pImageStart = NULL;
	m_pImagePushStart = NULL;
	m_pImagePause = NULL;
	m_pImagePushPause = NULL;
	m_pImageStop = NULL;
	m_pImagePushStop = NULL;
	m_pImageIsStop = NULL;
	m_pImageCircle = NULL;
	m_pImagePushCircle = NULL;
	m_pImageIsCircle = NULL;
	m_pImageCatch = NULL;

	m_pImageFirstPage = NULL;
	m_pImageLastPage = NULL;
	m_pImagePrePage = NULL;
	m_pImageNextPage = NULL;
	m_pImagePrintCur = NULL;

	//挡板图片指针
	m_pImageTopHandle = NULL;
	m_pImageLeftHandle = NULL;
	m_pImageRightHandle = NULL;
	m_pImageBottomHandle = NULL;
	m_pImageBaffleClose = NULL;
	//放大镜关闭按键
	m_pImageMaqnClose = NULL;
	m_bShowDocTip = 0;
	//Dxf按钮
	m_pImageDxfZoomIn=NULL;
	m_pImageDxfZoomOut=NULL;
	m_pImageDxfMove=NULL;
	//初始化图片

	m_pImageMIDIDown = NULL;
	m_pImageMIDIUp = NULL;
	m_pImageMidiPre = NULL;
	m_pImageMideNext = NULL;
	m_pImageMidiFirst = NULL;
	m_pImageMidiLast = NULL;
	m_pImageMidiPrint = NULL;
	InitImageFromResource();
}

CWBControlButton::~CWBControlButton()
{
	if (m_pImageBackground != NULL)
	{
		delete m_pImageBackground;
		m_pImageBackground = NULL;
	}
	if (m_pImageStart != NULL)
	{
		delete m_pImageStart;
		m_pImageStart = NULL;
	}
	if (m_pImagePushStart != NULL) 
	{
		delete m_pImagePushStart;
		m_pImagePushStart = NULL;
	}
	if (m_pImagePause != NULL)
	{
		delete m_pImagePause;
		m_pImagePause = NULL;
	}
	if (m_pImagePushPause != NULL)
	{
		delete m_pImagePushPause;
		m_pImagePushPause = NULL;
	}
	if (m_pImageStop != NULL)
	{
		delete m_pImageStop;
		m_pImageStop = NULL;
	}
	if (m_pImagePushStop != NULL)
	{
		delete m_pImagePushStop;
		m_pImagePushStop = NULL;
	}
	if (m_pImageIsStop != NULL)
	{
		delete m_pImageIsStop;
		m_pImageIsStop = NULL;
	}
	if (m_pImageCircle != NULL)
	{
		delete m_pImageCircle;
		m_pImageCircle = NULL;
	}
	if (m_pImagePushCircle != NULL)
	{
		delete m_pImagePushCircle;
		m_pImagePushCircle = NULL;
	}
	if (m_pImageIsCircle != NULL)
	{
		delete m_pImageIsCircle;
		m_pImageIsCircle = NULL;
	}
	if (m_pImageCatch != NULL)
	{
		delete m_pImageCatch;
		m_pImageCatch = NULL;
	}

	if (m_pImageFirstPage != NULL)
	{
		delete m_pImageFirstPage;
		m_pImageFirstPage = NULL;
	}
	if (m_pImageLastPage != NULL)
	{
		delete m_pImageLastPage;
		m_pImageLastPage = NULL;
	}
	if (m_pImagePrePage != NULL)
	{
		delete m_pImagePrePage;
		m_pImagePrePage = NULL;
	}
	if (m_pImageNextPage != NULL)
	{
		delete m_pImageNextPage;
		m_pImageNextPage = NULL;
	}
	if (m_pImagePrintCur != NULL)
	{
		delete m_pImagePrintCur;
		m_pImagePrintCur = NULL;
	}
	if (m_pImageTopHandle != NULL)
	{
		delete m_pImageTopHandle;
		m_pImageTopHandle = NULL;
	}
	if (m_pImageLeftHandle != NULL)
	{
		delete m_pImageLeftHandle;
		m_pImageLeftHandle = NULL;
	}
	if (m_pImageRightHandle != NULL)
	{
		delete m_pImageRightHandle;
		m_pImageRightHandle = NULL;
	}
	if (m_pImageBottomHandle != NULL)
	{
		delete m_pImageBottomHandle;
		m_pImageBottomHandle = NULL;
	}
	if (m_pImageBaffleClose != NULL)
	{
		delete m_pImageBaffleClose;
		m_pImageBaffleClose = NULL;
	}
	if (m_pImageMaqnClose != NULL)
	{
		delete m_pImageMaqnClose;
		m_pImageMaqnClose = NULL;
	}
	if (m_pImageDxfZoomIn!=NULL)
	{
		delete m_pImageDxfZoomIn;
		m_pImageDxfZoomIn=NULL;
	}
	if (m_pImageDxfZoomOut!=NULL)
	{
		delete m_pImageDxfZoomOut;
		m_pImageDxfZoomOut=NULL;
	}
	if (m_pImageDxfMove!=NULL)
	{
		delete m_pImageDxfMove;
		m_pImageDxfMove=NULL;
	
	}
	if (m_pImageMIDIDown != NULL)
	{
		delete m_pImageMIDIDown;
		m_pImageMIDIDown = NULL;
	}
	if (m_pImageMIDIUp != NULL)
	{
		delete m_pImageMIDIUp;
		m_pImageMIDIUp = NULL;
	}
	if (m_pImageMidiPre != NULL)
	{
		delete m_pImageMidiPre;
		m_pImageMidiPre = NULL;
	}
	if (m_pImageMideNext != NULL)
	{
		delete m_pImageMideNext;
		m_pImageMideNext = NULL;
	}
	if (m_pImageMidiFirst != NULL)
	{
		delete m_pImageMidiFirst;
		m_pImageMidiFirst = NULL;
	}
	if (m_pImageMidiLast != NULL)
	{
		delete m_pImageMidiLast;
		m_pImageMidiLast = NULL;
	}
	if (m_pImageMidiPrint != NULL)
	{
		delete m_pImageMidiPrint;
		m_pImageMidiPrint = NULL;
	}
}

static BOOL infrect = FALSE;

void CWBControlButton::InitImageFromFile()
{
	if (m_pWBObject == NULL)
	{
		return;
	}
	wchar_t strFilename[MAX_PATH];	

	CString PictureDir = GetCurrentPath();	//获取当前程序执行的目录
	CString picturePath = PictureDir;
	int nType = m_pWBObject->GetType();
	switch(nType)
	{
	case WB_OBJ_DOCUMENT://判断对象是否为文档
		{
			//初始化文档首页按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\FirstPage.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageFirstPage = new ImageEx(strFilename);	
			m_pImageFirstPage->InitAnimation();

			//初始化文档尾页按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\BottomPage.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageLastPage = new ImageEx(strFilename);	
			m_pImageLastPage->InitAnimation();

			//初始化文档上一页按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\PrePage.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImagePrePage = new ImageEx(strFilename);	
			m_pImagePrePage->InitAnimation();
			//初始化文档下一页按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\NextPage.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageNextPage = new ImageEx(strFilename);	
			m_pImageNextPage->InitAnimation();

			//初始化打印文档当前页的图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\PrintCurPage.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImagePrintCur = new ImageEx(strFilename);	
			m_pImagePrintCur->InitAnimation();
		}
		break;
	case WB_OBJ_MOVIE://判断对象是否为视屏
		{
			//初始化播放背景图片	
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\Skin0.png");			
			//	MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageBackground = new ImageEx(strFilename);	

			//初始化播放按钮图片	
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\Play.png");			
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageStart = new ImageEx(strFilename);	

			//初始化按下播放按钮时的图片		
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\PushPlay.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImagePushStart = new ImageEx(strFilename);	
			//初始化暂停按钮图片		
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\Pause.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImagePause = new ImageEx(strFilename);	
			//初始化按下暂停按钮时的图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\PushPause.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImagePushPause = new ImageEx(strFilename);	
			//初始化停止按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\Stop.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageStop = new ImageEx(strFilename);	
			//初始化按下停止按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\PushStop.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImagePushStop = new ImageEx(strFilename);	
			//初始化已经停止按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\AlreadyStop.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageIsStop = new ImageEx(strFilename);	
			//初始化已经循环按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\Loop.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageCircle = new ImageEx(strFilename);	
			//初始化按下循环按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\PushLoop.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImagePushCircle = new ImageEx(strFilename);	
			//初始化已经循环按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\AlreadyLoop.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageIsCircle = new ImageEx(strFilename);
			//初始化截图按钮
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\FlashFrameCapture.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageCatch = new ImageEx(strFilename);
		}
		break;
	case WB_OBJ_BAFFLE://判断对象是否为挡板
		{
			//初始化文档首页按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\TopHandle.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageTopHandle = new ImageEx(strFilename);	
			m_pImageTopHandle->InitAnimation();

			//初始化文档尾页按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\RightHandle.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageRightHandle = new ImageEx(strFilename);	
			m_pImageRightHandle->InitAnimation();
			//初始化文档上一页按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\BottomHandle.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageBottomHandle = new ImageEx(strFilename);	
			m_pImageBottomHandle->InitAnimation();
			//初始化文档下一页按钮图片
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\LeftHandle.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageLeftHandle = new ImageEx(strFilename);	
			m_pImageLeftHandle->InitAnimation();
		}
		break;
	case WB_OBJ_MAQN:
		{
			picturePath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\MAQNClose2.png");
			//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
			m_pImageMaqnClose = new ImageEx(strFilename);	
			m_pImageMaqnClose->InitAnimation();
		}
		break;
	default:
		break;
	}

}

void CWBControlButton::InitImageFromResource()
{
	if (m_pWBObject == NULL)
	{
		return;
	}
	int nType = m_pWBObject->GetType();
	switch(nType)
	{
	case WB_OBJ_DOCUMENT://判断对象是否为文档
		{
			m_pImageFirstPage = new ImageEx(_T("PNG"), _T("IDB_FIRSTPAGE"));	

			m_pImagePrePage = new ImageEx(_T("PNG"), _T("IDB_PREPAGE"));	

			m_pImageNextPage = new ImageEx(_T("PNG"), _T("IDB_NEXTPAGE"));	

			m_pImageLastPage = new ImageEx(_T("PNG"), _T("IDB_LASTPAGE"));

			m_pImagePrintCur = new ImageEx(_T("PNG"), _T("IDB_PRINTCUR"));
		}
		break;
	case WB_OBJ_MOVIE:
	case WB_OBJ_FLASH:
		{
			//初始化播放背景图片
			m_pImageBackground = new ImageEx(_T("PNG"), _T("IDB_BKGROUNDBLACK"));	
			//初始化播放按钮图片	
			m_pImageStart = new ImageEx(_T("PNG"), _T("IDB_START"));
			//初始化按下播放按钮时的图片		
			m_pImagePushStart = new ImageEx(_T("PNG"), _T("IDB_PUSHSTART"));
			//初始化暂停按钮图片
			m_pImagePause = new ImageEx(_T("PNG"), _T("IDB_PAUSE"));	

			//初始化按下暂停按钮时的图片
			m_pImagePushPause = new ImageEx(_T("PNG"), _T("IDB_PUSHPAUSE"));	

			//初始化停止按钮图片
			m_pImageStop = new ImageEx(_T("PNG"), _T("IDB_STOP"));	

			//初始化按下停止按钮图片
			m_pImagePushStop = new ImageEx(_T("PNG"), _T("IDB_PUSHSTOP"));	

			//初始化已经停止按钮图片
			m_pImageIsStop = new ImageEx(_T("PNG"), _T("IDB_STOPED"));

			//初始化循环按钮图片
			m_pImageCircle = new ImageEx(_T("PNG"), _T("IDB_CIRCLE"));	

			//初始化按下循环按钮图片
			m_pImagePushCircle = new ImageEx(_T("PNG"), _T("IDB_PUSHCIRCLE"));	

			//初始化已经循环按钮图片
			m_pImageIsCircle = new ImageEx(_T("PNG"), _T("IDB_ISCIRCLE"));

			//初始化截图按钮
			m_pImageCatch = new ImageEx(_T("PNG"), _T("IDB_CATCH"));
		}
		break;
	case WB_OBJ_BAFFLE://判断对象是否为挡板
		{
			m_pImageTopHandle = new ImageEx(_T("PNG"), _T("IDB_TOPHANDLE")); //顶部把手
			m_pImageLeftHandle = new ImageEx(_T("PNG"), _T("IDB_LEFTHANDLE"));   //左边把手  
			m_pImageRightHandle = new ImageEx(_T("PNG"), _T("IDB_RIHGTHANDLE")); //右边把手
			m_pImageBottomHandle = new ImageEx(_T("PNG"), _T("IDB_BOTTOMHANDLE")); //底部把手
			m_pImageBaffleClose = new ImageEx(_T("PNG"), _T("IDB_BAFFLE_CLOSE")); //关闭
		}
		break;
	case WB_OBJ_MAQN:
		{
			m_pImageMaqnClose = new ImageEx(_T("PNG"), _T("IDB_MAQN_CLOSE")); //关闭
		}
		break;
	case WB_OBJ_DXF:
		{
			m_pImageDxfZoomOut = new ImageEx(_T("PNG"), _T("IDB_DXF_ZOOMOUT"));	
			m_pImageDxfZoomIn = new ImageEx(_T("PNG"), _T("IDB_DXF_ZOOMIN"));	
			m_pImageDxfMove = new ImageEx(_T("PNG"), _T("IDB_DXF_MOVE"));	
		}
		break;
	case WB_OBJ_MIDI:
		{
			m_pImageMIDIUp = new ImageEx(_T("PNG"), _T("IDB_DXF_ZOOMOUT"));	
			m_pImageMIDIDown = new ImageEx(_T("PNG"), _T("IDB_DXF_ZOOMIN"));
			m_pImageMidiPre = new ImageEx(_T("PNG"), _T("IDB_PREPAGE"));
			m_pImageMideNext = new ImageEx(_T("PNG"), _T("IDB_NEXTPAGE"));
			m_pImageMidiFirst =  new ImageEx(_T("PNG"), _T("IDB_FIRSTPAGE"));
			m_pImageMidiLast =new ImageEx(_T("PNG"), _T("IDB_LASTPAGE"));
			m_pImageDxfZoomOut = new ImageEx(_T("PNG"), _T("IDB_DXF_ZOOMOUT"));	
			m_pImageDxfZoomIn = new ImageEx(_T("PNG"), _T("IDB_DXF_ZOOMIN"));	
			m_pImageDxfMove = new ImageEx(_T("PNG"), _T("IDB_DXF_MOVE"));
			m_pImageMidiPrint = new ImageEx(_T("PNG"), _T("IDB_PRINTCUR"));
		}
		break;
	default:
		break;
	}
}

void CWBControlButton::ButtonDown(CPoint point)
{
	switch(m_nType)
	{
	case BTN_TYPE_TOP:
		{
			m_baffleBeginPt = point;
		}
		break;
	case BTN_TYPE_LETE:
		{
			m_baffleBeginPt = point;
		}
		break;
	case BTN_TYPE_RIGHT:
		{
			m_baffleBeginPt = point;
		}
		break;
	case BTN_TYPE_BOTTOM:
		{
			m_baffleBeginPt = point;
		}
		break;
	case BTN_TYPE_CLOSEPATETTE:
		{
			m_bIsButtonDown = TRUE;
		}
		break;
	case BTN_TYPE_START:
		{
			m_bIsButtonDown = TRUE;
		}
		break;
	case BTN_TYPE_STOP:
		{
			m_bIsButtonDown = TRUE;
		}
		break;
	case BTN_TYPE_CIRCLE:
		{
			m_bIsButtonDown = TRUE;
		}
		break;
	case BTN_TYPE_CATCH:
		{
			m_bIsButtonDown = TRUE;
		}
		break;

		//case BTN_TYPE_MAQNCLOSE:
		//	{
		//		g_pWhiteBoardView->SetMaqnState(FALSE);
		//		g_pWhiteBoardView->SendMagn();
		//		g_pWhiteBoardView->CloseRectMagn();
		//	}
		//	break;

	default:
		return;
	}
}
void CWBControlButton::ButtonMove(CPoint point) //在view的MouseMOve中调用
{
	if (g_pWhiteBoardView == NULL || NULL == m_pWBObject)
	{
		return;
	}
	switch(m_nType)
	{
	case BTN_TYPE_TOP:
		{
			CPoint ptOffset = point - m_baffleBeginPt;
			if ((((CWBBafflePlate*)m_pWBObject)->GetItem()->ptRightBottom.y - point.y) > 5 * BAFFLE_CLOSEBTN_HIGHT) 
			{
				((CWBBafflePlate*)m_pWBObject)->ChangeTopHandle(ptOffset);
				g_pWhiteBoardView->Invalidate();
			}
			m_baffleBeginPt = point;		
		}
		break;
	case BTN_TYPE_LETE:
		{
			CPoint ptOffset = point - m_baffleBeginPt;
			if ((((CWBBafflePlate*)m_pWBObject)->GetItem()->ptRightBottom.x - point.x) > 5 * BAFFLE_CLOSEBTN_WIDTH)
			{
				((CWBBafflePlate*)m_pWBObject)->ChangeLeftHandle(ptOffset);
				g_pWhiteBoardView->Invalidate();
			}
			m_baffleBeginPt = point;


		}
		break;
	case BTN_TYPE_RIGHT:
		{
			CPoint ptOffset = point - m_baffleBeginPt;
			if ((point.x - ((CWBBafflePlate*)m_pWBObject)->GetItem()->ptLeftTop.x) > 5 * BAFFLE_CLOSEBTN_WIDTH)
			{
				((CWBBafflePlate*)m_pWBObject)->ChangeRightHandle(ptOffset);
				g_pWhiteBoardView->Invalidate();
			}
			m_baffleBeginPt = point;

		}
		break;
	case BTN_TYPE_BOTTOM:
		{
			CPoint ptOffset = point - m_baffleBeginPt;
			if ((point.y - ((CWBBafflePlate*)m_pWBObject)->GetItem()->ptLeftTop.y) > 5 * BAFFLE_CLOSEBTN_HIGHT)
			{					
				((CWBBafflePlate*)m_pWBObject)->ChangeBottomHandle(ptOffset);
				g_pWhiteBoardView->Invalidate();					
			}
			m_baffleBeginPt = point;

		}
		break;
	default:
		return;

	}
}

void CWBControlButton::ButtonUp(CPoint point)
{
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	if (m_pWBObject == NULL)
	{
		return;
	}


	m_bShowDocTip = 0;
	if (CWBToolTip::GetIns() != NULL)
	{
		CWBToolTip::GetIns()->Restore();
	}
	int nPageIndex = g_pWhiteBoardView->GetPrePage();

	switch(m_nType)
	{
	case BTN_TYPE_FIRSTPAGE:
		{
			((CWBDocument *)m_pWBObject)->FirstPage();
			if (!m_pWBObject->IsCombineMember())
			{
				//文档翻页同步
				XMemIni xmlObj;
				if (nPageIndex != 0)
				{
					xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
					g_pWhiteBoardView->SetPrePage(0);
				}
				((CWBDocument *)m_pWBObject)->SetXmlValue(xmlObj);
				g_pWhiteBoardView->ChangeObj(xmlObj);
			}
		}
		break;
	case BTN_TYPE_LASTPAGE:
		{
			((CWBDocument *)m_pWBObject)->LastPage();
			if (!m_pWBObject->IsCombineMember())
			{
				//文档翻页同步
				XMemIni xmlObj;
				if (nPageIndex != 0)
				{
					xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
					g_pWhiteBoardView->SetPrePage(0);
				}
				((CWBDocument *)m_pWBObject)->SetXmlValue(xmlObj);
				g_pWhiteBoardView->ChangeObj(xmlObj);
			}
		}
		break;
	case BTN_TYPE_NEXTPAGE:
		{
			((CWBDocument *)m_pWBObject)->NextPage();

			//发送同步信息
			if (!m_pWBObject->IsCombineMember())
			{
				//文档翻页同步
				XMemIni xmlObj;
				if (nPageIndex != 0)
				{
					xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
					g_pWhiteBoardView->SetPrePage(0);
				}
				((CWBDocument *)m_pWBObject)->SetXmlValue(xmlObj);
				g_pWhiteBoardView->ChangeObj(xmlObj);
			}
		}
		break;
	case BTN_TYPE_PREPAGE:
		{
			((CWBDocument *)m_pWBObject)->PrePage();

			//发送同步信息
			if (!m_pWBObject->IsCombineMember())
			{
				//文档翻页同步
				XMemIni xmlObj;
				if (nPageIndex != 0)
				{
					xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
					g_pWhiteBoardView->SetPrePage(0);
				}
				((CWBDocument *)m_pWBObject)->SetXmlValue(xmlObj);
				g_pWhiteBoardView->ChangeObj(xmlObj);
			}
		}
		break;
	case BTN_TYPE_PRINTPAGE:
		{
			if (m_pWBObject->GetType() == WB_OBJ_MIDI)
			{
				((CWBMidi*)m_pWBObject)->SaveAsJpg();
			}
			else
			{
				((CWBDocument *)m_pWBObject)->PrintCurPage();
			}
			
		}
		break;
	case BTN_TYPE_START:
		{
			if (m_pWBObject->GetType() == WB_OBJ_FLASH && !((CWBFlash*)m_pWBObject)->m_bIsRunning )
			{
				infrect = FALSE;
				((CWBFlash *)m_pWBObject)->Start();
				//发送同步信息
				XMemIni xmlObj;
				if (nPageIndex != 0)
				{
					xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
					g_pWhiteBoardView->SetPrePage(0);
				}
				((CWBFlash*)m_pWBObject)->SetXmlValue(xmlObj);
				/*m_pWBObject->SetXmlValue(xmlObj);*/
				xmlObj.Set(KEY_MEDIA_OPERATION, WB_MOVIE_START);
				//g_pWhiteBoardView->MediaOperation(xmlObj);	
				g_pWhiteBoardView->ChangeObj(xmlObj);
			}
			else if (m_pWBObject->GetType() == WB_OBJ_FLASH && ((CWBFlash*)m_pWBObject)->m_bIsRunning && !((CWBFlash*)m_pWBObject)->m_bIsPause)
			{
				infrect = FALSE;
				((CWBFlash *)m_pWBObject)->Pause();
				//发送同步信息
				XMemIni xmlObj;
				if (nPageIndex != 0)
				{
					xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
					g_pWhiteBoardView->SetPrePage(0);
				}
				/*m_pWBObject->SetXmlValue(xmlObj);*/
				((CWBFlash*)m_pWBObject)->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_MEDIA_OPERATION, WB_MOVIE_PAUSE);
				//g_pWhiteBoardView->MediaOperation(xmlObj);	
				g_pWhiteBoardView->ChangeObj(xmlObj);
			}
			m_bIsButtonDown = FALSE;
		}
		break;
	case BTN_TYPE_STOP:
		{
			if (m_pWBObject->GetType() == WB_OBJ_FLASH)
			{
				((CWBFlash *)m_pWBObject)->Stop();
			}		
			m_bIsButtonDown = FALSE;

			//发送同步信息
			XMemIni xmlObj;
			if (nPageIndex != 0)
			{
				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				g_pWhiteBoardView->SetPrePage(0);
			}
			//m_pWBObject->SetXmlValue(xmlObj);
			((CWBFlash*)m_pWBObject)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_OPERATION, WB_MOVIE_STOP);
			/*g_pWhiteBoardView->MediaOperation(xmlObj);*/
			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		break;
	case BTN_TYPE_CIRCLE:
		{
			if (m_pWBObject->GetType() == WB_OBJ_FLASH )
			{
				BOOL isLoop = ((CWBFlash*)m_pWBObject)->GetCircle();
				if (isLoop)
				{
					infrect = FALSE;
					((CWBFlash *)m_pWBObject)->SetSinglePlay();
					m_bIsButtonDown = FALSE;

					//发送同步信息
					XMemIni xmlObj;
					if (nPageIndex != 0)
					{
						xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
						g_pWhiteBoardView->SetPrePage(0);
					}
					//m_pWBObject->SetXmlValue(xmlObj);

					((CWBFlash*)m_pWBObject)->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_MEDIA_OPERATION, WB_MOVIE_SINGLEPLAY);
					/*g_pWhiteBoardView->MediaOperation(xmlObj);*/
					g_pWhiteBoardView->ChangeObj(xmlObj);
				}
				else
				{
					infrect = FALSE;
					((CWBFlash *)m_pWBObject)->SetCircle();
					m_bIsButtonDown = FALSE;

					//发送同步信息
					XMemIni xmlObj;
					if (nPageIndex != 0)
					{
						xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
						g_pWhiteBoardView->SetPrePage(0);
					}
					//m_pWBObject->SetXmlValue(xmlObj);
					((CWBFlash *)m_pWBObject)->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_MEDIA_OPERATION, WB_MOVIE_LOOP);
					/*g_pWhiteBoardView->MediaOperation(xmlObj);*/
					g_pWhiteBoardView->ChangeObj(xmlObj);
				}
			}		
		}
		break;
	case BTN_TYPE_CATCH:
		{

			CString strFileName;

			if (m_pWBObject->GetType() == WB_OBJ_FLASH)
			{
				strFileName = ((CWBFlash *)m_pWBObject)->CatchFlash();		
				CWBPicture *pPictrue = new CWBPicture(strFileName);

				//	pPictrue->SetIsPath();
				g_pWBPage->AddWBObject(pPictrue);
				CPoint FlashPoint(72, 44);
				pPictrue->MoveOffset(FlashPoint);
				//发送同步消息
				XMemIni xmlObj;
				if (nPageIndex != 0)
				{
					xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
					g_pWhiteBoardView->SetPrePage(0);
				}
				pPictrue->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);
			}
		}
		break;
	case BTN_TYPE_TOP:
		{
			//挡板把手移动同步
			XMemIni xmlObj;
			if (nPageIndex != 0)
			{
				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				g_pWhiteBoardView->SetPrePage(0);
			}
			((CWBBafflePlate *)m_pWBObject)->SetXmlValue(xmlObj);
			g_pWhiteBoardView->ChangeObj(xmlObj);//将数据发送出去
		}
		break;
	case BTN_TYPE_LETE:
		{
			//挡板把手移动同步
			XMemIni xmlObj;
			if (nPageIndex != 0)
			{
				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				g_pWhiteBoardView->SetPrePage(0);
			}
			((CWBBafflePlate *)m_pWBObject)->SetXmlValue(xmlObj);
			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		break;
	case BTN_TYPE_RIGHT:
		{
			//挡板把手移动同步
			XMemIni xmlObj;
			if (nPageIndex != 0)
			{
				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				g_pWhiteBoardView->SetPrePage(0);
			}
			((CWBBafflePlate *)m_pWBObject)->SetXmlValue(xmlObj);
			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		break;
	case BTN_TYPE_BOTTOM:
		{
			//挡板把手移动同步
			XMemIni xmlObj;
			if (nPageIndex != 0)
			{
				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				g_pWhiteBoardView->SetPrePage(0);
			}
			((CWBBafflePlate *)m_pWBObject)->SetXmlValue(xmlObj);
			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		break;
	case BTN_TYPE_CLOSEPATETTE://挡板的关闭按钮
		{
			DWORD BaffleIndexID= ((CWBBafflePlate *)m_pWBObject)->GetLayerIndex();
			g_pWBPage->DeleteWBObject(BaffleIndexID);//依据对象的ID号删除对象
			g_pWhiteBoardView->RemoveObj(BaffleIndexID);
		}
		break;
	case BTN_TYPE_DXF_ZOOMIN: //DXF放大按钮
		{
			if (m_pWBObject->GetType() == WB_OBJ_MIDI)
			{
				if (m_pWBObject->IsLock())
				{
					break;
				}
				((CWBMidi*)m_pWBObject)->SetZoomMagn();
				XMemIni xmlobj;
				m_pWBObject->SetXmlValue(xmlobj);
				g_pWhiteBoardView->ChangeObj(xmlobj);
			}
			else if(m_pWBObject->GetSelectFrameShow())
			{
				((CWBDxf*)m_pWBObject)->ZoomIn(1.2);
				((CWBDxf *)m_pWBObject)->SetIsNeedReDraw(); 
				XMemIni xmlObj;
				m_pWBObject->SetXmlValue(xmlObj);
				g_pWhiteBoardView->ChangeObj(xmlObj);
			}
		}
		break;
	case BTN_TYPE_DXF_ZOOMOUT: //DXF缩小按钮
		{
			if (m_pWBObject->GetType() == WB_OBJ_MIDI)
			{
				if (m_pWBObject->IsLock())
				{
					break;
				}
				((CWBMidi*)m_pWBObject)->SetZoomReduce();
				XMemIni xmlobj;
				m_pWBObject->SetXmlValue(xmlobj);
				g_pWhiteBoardView->ChangeObj(xmlobj);
			}
			else if(m_pWBObject->GetSelectFrameShow())
			{
				((CWBDxf*)m_pWBObject)->ZoomOut(1.2);
				((CWBDxf *)m_pWBObject)->SetIsNeedReDraw(); 
				XMemIni xmlObj;
				m_pWBObject->SetXmlValue(xmlObj);
				g_pWhiteBoardView->ChangeObj(xmlObj);
			}
		}
		break;
	case BTN_TYPE_DXF_MOVE:
		{
			if (m_pWBObject->GetType() == WB_OBJ_MIDI)
			{
				if (m_pWBObject->IsLock())
				{
					break;
				}
				g_nDrawStatus=MIDI_DRAG;
				SetCursor(LoadCursor(NULL,IDC_HAND));
			}
			else if(m_pWBObject->GetSelectFrameShow())
			{
				g_nDrawStatus=DXF_PREDRAG;
			}
		}
		break;
	case BTN_TYPE_MIDI_UP:
		{
// 			if (m_pWBObject->GetType() == WB_OBJ_MIDI)
// 			{
// 				
// 			}
// 			g_pWhiteBoardView->SetTimer(ID_TIMER_MDIDATUO,1000,NULL);
		}
		break;
	case BTN_TYPE_MIDI_DOWN:
		{
// 			if (m_pWBObject->GetType() == WB_OBJ_MIDI)
// 			{
// 
// 			}
// 			g_pWhiteBoardView->SetTimer(ID_TIMER_MDIDATUO,1000,NULL);
		}
		break;
	case BTN_TYPE_MIDI_PRE:
		{

// 			if (m_pWBObject->GetType() != WB_OBJ_MIDI)
// 			{
// 				break;
// 			}
// 			((CWBMidi*)m_pWBObject)->SetPrePage();
// 			XMemIni xmlObj;
// 			if (nPageIndex != 0)
// 			{
// 				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
// 				g_pWhiteBoardView->SetPrePage(0);
// 			}
// 			((CWBMidi*)m_pWBObject)->SetXmlValue(xmlObj);
// 			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		break;
	case BTN_TYPE_MIDI_NEXT:
		{

// 			if (m_pWBObject->GetType() != WB_OBJ_MIDI)
// 			{
// 				break;
// 			}
// 			((CWBMidi*)m_pWBObject)->SetNextPage();
// 			XMemIni xmlObj;
// 			if (nPageIndex != 0)
// 			{
// 				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
// 				g_pWhiteBoardView->SetPrePage(0);
// 			}
// 			((CWBMidi*)m_pWBObject)->SetXmlValue(xmlObj);
// 			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		break;
	case BTN_TYPE_MIDI_FIRST:
		{
// 			if (m_pWBObject->GetType() != WB_OBJ_MIDI)
// 			{
// 				break;
// 			}
// 			((CWBMidi*)m_pWBObject)->SetFirstPage();
// 			XMemIni xmlObj;
// 			if (nPageIndex != 0)
// 			{
// 				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
// 				g_pWhiteBoardView->SetPrePage(0);
// 			}
// 			((CWBMidi *)m_pWBObject)->SetXmlValue(xmlObj);
// 			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		break;
	case BTN_TYPE_MIDI_LAST:
		{
// 			if (m_pWBObject->GetType() != WB_OBJ_MIDI)
// 			{
// 				break;
// 			}
// 			((CWBMidi*)m_pWBObject)->SetLastPage();
// 			XMemIni xmlObj;
// 			if (nPageIndex != 0)
// 			{
// 				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
// 				g_pWhiteBoardView->SetPrePage(0);
// 			}
// 			((CWBMidi *)m_pWBObject)->SetXmlValue(xmlObj);
// 			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		break;
	default:
		return;
	}
}

void CWBControlButton::Draw(CDC *pDC)
{
	if (pDC == NULL || m_pWBObject == NULL)
	{
		return;
	}

	HDC hdc = pDC->GetSafeHdc();
	m_pDC = pDC;
	CRect rect;
	//addby zhuyuyuan do not call my name.
	if (m_pWBObject->GetType() == WB_OBJ_MAQN)
	{
		//获得放大框的坐标
		CRect rcMaqn;
		((CWBMagnifier*)m_pWBObject)->GetRect(rcMaqn);

		rect.left = rcMaqn.right - MAQN_BTN_WIDTH - MAQN_BTN_RIGHT_INTERVAL;
		rect.top = rcMaqn.bottom + MAQN_BTN_TOP_INTERVAL;
		rect.right =  rect.left + MAQN_BTN_WIDTH;
		rect.bottom = rect.top + MAQN_BTN_HEIGHT;

		//m_pImageMaqnClose->m_rect = rect;
		//m_pImageMaqnClose->Draw(hdc,m_pWBObject->GetAlpha());
		m_pImageMaqnClose->Draw(hdc,rect);
		return;
	}

	//旋转坐标系
	int nGraphicsMode = RotateXForm(m_pWBObject->GetRotationAngle());	

	if (m_pWBObject->GetObjSelectFrame() == NULL)
	{
		m_pWBObject->CreateSelecrtFrame();
	}
	CPoint ptLeftTop = (m_pWBObject->GetObjSelectFrame())->GetLeftTop();
	CPoint ptRightTop = (m_pWBObject->GetObjSelectFrame())->GetRightTop();
	CPoint ptRightBottom = (m_pWBObject->GetObjSelectFrame())->GetRightBottom();


	if (m_pWBObject->GetType() == WB_OBJ_BAFFLE) //挡板
	{
		CRect rtBaffle = CRect(ptLeftTop, ptRightBottom);//挡板对象的外部矩形
		//挡板顶部按钮的位置
		rect.top = rtBaffle.top + BAFFLE_BTN_SPACE;
		rect.left = rtBaffle.left + rtBaffle.Width()/2 - BAFFLE_BTN_WIDTH/2;
		rect.bottom = rect.top + BAFFLE_BTN_HIGHT;
		rect.right = rect.left + BAFFLE_BTN_WIDTH ;
		m_pImageTopHandle->m_rect = rect;

		//挡板底部按钮的位置
		rect.top = rtBaffle.bottom - BAFFLE_BTN_SPACE - BAFFLE_BTN_HIGHT;
		rect.left = rtBaffle.left + rtBaffle.Width()/2 - BAFFLE_BTN_WIDTH/2;
		rect.bottom = rect.top + BAFFLE_BTN_HIGHT;
		rect.right = rect.left + BAFFLE_BTN_WIDTH ;
		m_pImageBottomHandle->m_rect = rect;

		//挡板左边按钮的位置
		rect.left = rtBaffle.left + BAFFLE_BTN_SPACE;
		rect.top = rtBaffle.top + rtBaffle.Height()/2 -  BAFFLE_BTN_WIDTH/2;	
		rect.right = rect.left + BAFFLE_BTN_HIGHT;
		rect.bottom = rect.top + BAFFLE_BTN_WIDTH;		
		m_pImageLeftHandle->m_rect = rect;

		//挡板右部按钮的位置
		rect.left = rtBaffle.left + rtBaffle.Width() - BAFFLE_BTN_SPACE - BAFFLE_BTN_HIGHT;
		rect.top = rtBaffle.top + rtBaffle.Height()/2 - BAFFLE_BTN_WIDTH/2;
		rect.right = rect.left + BAFFLE_BTN_HIGHT;
		rect.bottom = rect.top + BAFFLE_BTN_WIDTH;
		m_pImageRightHandle->m_rect = rect;

		//关闭按钮的位置
		rect.left = rtBaffle.left + rtBaffle.Width() - BAFFLE_BTN_SPACE - BAFFLE_CLOSEBTN_WIDTH;
		rect.top = rtBaffle.top + BAFFLE_BTN_SPACE;
		rect.right = rect.left + BAFFLE_CLOSEBTN_WIDTH;
		rect.bottom = rect.top + BAFFLE_CLOSEBTN_HIGHT;
		m_pImageBaffleClose->m_rect = rect;

		//显示按钮
		m_pImageTopHandle->Draw(hdc, m_pWBObject->GetAlpha());
		m_pImageBottomHandle->Draw(hdc, m_pWBObject->GetAlpha());
		m_pImageLeftHandle->Draw(hdc, m_pWBObject->GetAlpha());
		m_pImageRightHandle->Draw(hdc, m_pWBObject->GetAlpha());
		m_pImageBaffleClose->Draw(hdc, m_pWBObject->GetAlpha());

	}
	else
	{
		if (m_pWBObject->GetType() == WB_OBJ_DOCUMENT) //文档
		{		
			int nCenter = (ptLeftTop.x + ptRightTop.x) / 2;

			//计算首页按钮图片的位置		
			rect.top = ptLeftTop.y - BUTTONSIZE;
			rect.bottom = rect.top + BUTTONSIZE;
			rect.left = nCenter - 3 * BUTTONSIZE - BUTTONSIZE / 2;
			rect.right =  rect.left + BUTTONSIZE;
			m_pImageFirstPage->m_rect = rect;

			//计算上一页按钮图片的位置
			rect.left = nCenter - BUTTONSIZE - BUTTONSIZE / 2;
			rect.right = rect.left + BUTTONSIZE;
			m_pImagePrePage->m_rect = rect;

			//计算下一页按钮图片的位置
			rect.left = nCenter + BUTTONSIZE / 2;
			rect.right = rect.left + BUTTONSIZE;
			m_pImageNextPage->m_rect = rect;


			//计算尾页按钮图片的位置
			rect.left = nCenter + 2 * BUTTONSIZE + BUTTONSIZE /2;
			rect.right = rect.left + BUTTONSIZE;
			m_pImageLastPage->m_rect = rect;

			//计算打印按钮图片的位置
			rect.left = nCenter + 4 * BUTTONSIZE + BUTTONSIZE /2;
			rect.right = rect.left + BUTTONSIZE;
			m_pImagePrintCur->m_rect = rect;

			CWBDocument *pDoc = (CWBDocument *)m_pWBObject;
			if (pDoc->GetPageCount() > 1)
			{
				if (pDoc->GetCurPageIndex() <= 1) //如果当前页是第一页
				{
					m_pImageNextPage->Draw(hdc, m_pWBObject->GetAlpha());
					m_pImageLastPage->Draw(hdc, m_pWBObject->GetAlpha());
				}
				else if (pDoc->GetCurPageIndex() >= pDoc->GetPageCount()) //如果当前页是最后一页
				{
					m_pImageFirstPage->Draw(hdc, m_pWBObject->GetAlpha());
					m_pImagePrePage->Draw(hdc, m_pWBObject->GetAlpha());
				}
				else
				{
					m_pImageFirstPage->Draw(hdc, m_pWBObject->GetAlpha());
					m_pImagePrePage->Draw(hdc, m_pWBObject->GetAlpha());
					m_pImageNextPage->Draw(hdc, m_pWBObject->GetAlpha());
					m_pImageLastPage->Draw(hdc, m_pWBObject->GetAlpha());			
				}	
			}
			if (!g_bSupportU7OpenFile) //在U7中不显示打印按钮
			{
				m_pImagePrintCur->Draw(hdc, m_pWBObject->GetAlpha());
			}
		}
		else if(m_pWBObject->GetType() == WB_OBJ_DXF )
		{
			int nCenter = (ptLeftTop.x + ptRightTop.x) / 2;
			rect.top= ptLeftTop.y - BUTTONSIZE;
			rect.left = nCenter - 5*BUTTONSIZE;
			rect.right = rect.left + BUTTONSIZE;
			rect.bottom = rect.top + BUTTONSIZE;
			m_pImageDxfZoomOut->m_rect = rect;

			rect.left = nCenter -3*BUTTONSIZE;
			rect.right = rect.left+BUTTONSIZE;
			m_pImageDxfZoomIn->m_rect = rect;

			rect.left = nCenter - 7*BUTTONSIZE;
			rect.right = rect.left+BUTTONSIZE;
			m_pImageDxfMove->m_rect = rect;
			
			m_pImageDxfZoomOut->Draw(hdc, m_pWBObject->GetAlpha());
			m_pImageDxfZoomIn->Draw(hdc, m_pWBObject->GetAlpha());	
			m_pImageDxfMove->Draw(hdc, m_pWBObject->GetAlpha());
		}
		else if (m_pWBObject->GetType() == WB_OBJ_MIDI)
		{
			int nCenter = (ptLeftTop.x + ptRightTop.x) / 2;
			rect.top= ptLeftTop.y - BUTTONSIZE;
			rect.left = nCenter + 4*BUTTONSIZE;
			rect.right = rect.left + BUTTONSIZE;
			rect.bottom = rect.top + BUTTONSIZE;
			m_pImageDxfZoomOut->m_rect = rect;

			rect.left = nCenter -4*BUTTONSIZE- BUTTONSIZE ;
			rect.right = rect.left+BUTTONSIZE;
			m_pImageDxfZoomIn->m_rect = rect;

			

			//计算首页按钮图片的位置		
// 			rect.top = ptLeftTop.y - BUTTONSIZE;
// 			rect.bottom = rect.top + BUTTONSIZE;
// 			rect.left = nCenter - 3 * BUTTONSIZE - BUTTONSIZE / 2;
// 			rect.right =  rect.left + BUTTONSIZE;
// 			m_pImageMidiFirst->m_rect = rect;
// 
// 			//计算上一页按钮图片的位置
// 			rect.left = nCenter - BUTTONSIZE - BUTTONSIZE / 2;
// 			rect.right = rect.left + BUTTONSIZE;
// 			m_pImageMidiPre->m_rect = rect;
// 
// 			//计算下一页按钮图片的位置
// 			rect.left = nCenter + BUTTONSIZE / 2;
// 			rect.right = rect.left + BUTTONSIZE;
// 			m_pImageMideNext->m_rect = rect;
// 
// 
// 			//计算尾页按钮图片的位置
// 			rect.left = nCenter + 2 * BUTTONSIZE + BUTTONSIZE /2;
// 			rect.right = rect.left + BUTTONSIZE;
// 			m_pImageMidiLast->m_rect = rect;
			rect.left = nCenter - 7*BUTTONSIZE;
			rect.right = rect.left+BUTTONSIZE;
			m_pImageDxfMove->m_rect = rect;


			rect.left = nCenter + 6 * BUTTONSIZE ;
			rect.right = rect.left + BUTTONSIZE;
			m_pImageMidiPrint->m_rect = rect;

			if (!m_pWBObject->IsLock())
			{
				
				m_pImageMidiPrint->Draw(hdc,m_pWBObject->GetAlpha());
// 				int npageindex = ((CWBMidi*)m_pWBObject)->GetPageIndex();
// 				int npagetotal = ((CWBMidi*)m_pWBObject)->GetPageTotal();
// 				if (1 == npagetotal)
// 				{
// 
// 				}
// 				else if (npageindex <= 1)
// 				{
// 					m_pImageMideNext->Draw(hdc,m_pWBObject->GetAlpha());
// 					m_pImageMidiLast->Draw(hdc,m_pWBObject->GetAlpha());
// 				}
// 				else if (npageindex >= npagetotal)
// 				{
// 					m_pImageMidiFirst->Draw(hdc,m_pWBObject->GetAlpha());
// 					m_pImageMidiPre->Draw(hdc,m_pWBObject->GetAlpha());
// 				}
// 				else
// 				{
// 					m_pImageMidiFirst->Draw(hdc,m_pWBObject->GetAlpha());
// 					m_pImageMidiPre->Draw(hdc,m_pWBObject->GetAlpha());
// 					m_pImageMideNext->Draw(hdc,m_pWBObject->GetAlpha());
// 					m_pImageMidiLast->Draw(hdc,m_pWBObject->GetAlpha());
// 				}
				if (((CWBMidi*)m_pWBObject)->GetShowChangeViewBtn())
				{
					if (((CWBMidi*)m_pWBObject)->Getshowredubtn())
					{
						m_pImageDxfZoomOut->Draw(hdc, m_pWBObject->GetAlpha());
					}
					if (((CWBMidi*)m_pWBObject)->Getshowmagbtn())
					{
						m_pImageDxfZoomIn->Draw(hdc, m_pWBObject->GetAlpha());
					}
					m_pImageDxfMove->Draw(hdc, m_pWBObject->GetAlpha());
				}
				
			}
		}
		else
		{
			CRect rcMovie = CRect(ptLeftTop, ptRightBottom);
			//按钮位置相对于控制框最左边的偏移量

			int offset = (rcMovie.Width() - 300)/2;
			//播放背景图片的位置
			m_pImageBackground->m_rect = rcMovie;
			//计算播放及暂停按钮的位置
			rect.bottom = rcMovie.bottom - 10;
			rect.top = rect.bottom - FLASHBUTTONSIZE;		
			rect.left = rcMovie.left + offset ;
			rect.right =  rect.left + FLASHBUTTONSIZE;
			m_pImageStart->m_rect = rect;
			m_pImagePushStart->m_rect = rect;
			m_pImagePause->m_rect = rect;
			m_pImagePushPause->m_rect = rect;

			//计算停止按钮的位置
			rect.bottom = rcMovie.bottom - 10;
			rect.top = rect.bottom - FLASHBUTTONSIZE;
			rect.left = rect.right + FLASHBUTTONOFFSET;
			rect.right = rect.left + FLASHBUTTONSIZE;
			m_pImageStop->m_rect = rect;
			m_pImagePushStop->m_rect = rect;
			m_pImageIsStop->m_rect = rect;

			//计算循环按钮图片的位置
			rect.bottom = rcMovie.bottom - 10;
			rect.top = rect.bottom - FLASHBUTTONSIZE;
			rect.left = rect.right + FLASHBUTTONOFFSET;
			rect.right = rect.left + FLASHBUTTONSIZE ;
			m_pImageCircle->m_rect = rect;
			m_pImageIsCircle->m_rect = rect;
			m_pImagePushCircle->m_rect = rect;

			//计算截图按钮的位置
			rect.bottom = rcMovie.bottom - 10;
			rect.top = rect.bottom - FLASHBUTTONSIZE;
			rect.left = rect.right + FLASHBUTTONOFFSET;
			rect.right = rect.left + FLASHBUTTONSIZE ;
			m_pImageCatch->m_rect = rect;

			m_pImageBackground->Draw(hdc, m_pWBObject->GetAlpha());

			m_pImageCatch->Draw(hdc, m_pWBObject->GetAlpha());

			if (m_nType == BTN_TYPE_CIRCLE && m_bIsButtonDown)
			{
				m_pImagePushCircle->Draw(hdc, m_pWBObject->GetAlpha());
			}
			else 
			{
				if (m_pWBObject->GetType() == WB_OBJ_FLASH)
				{
					if (((CWBFlash *)m_pWBObject)->GetCircle())
					{
						m_pImageCircle->Draw(hdc, m_pWBObject->GetAlpha());
					}
					else
					{
						m_pImageIsCircle->Draw(hdc, m_pWBObject->GetAlpha());


					}
				}
			}

			if (m_nType == BTN_TYPE_STOP && m_bIsButtonDown)
			{
				m_pImagePushStop->Draw(hdc, m_pWBObject->GetAlpha());
			}
			else 
			{
				if (m_pWBObject->GetType() == WB_OBJ_FLASH)
				{
					if (((CWBFlash *)m_pWBObject)->m_bIsStop == FALSE)
					{
						m_pImageStop->Draw(hdc, m_pWBObject->GetAlpha());
					}

					else 
					{
						m_pImageIsStop->Draw(hdc, m_pWBObject->GetAlpha());
					}
				}
			}


			if (m_nType == BTN_TYPE_START && m_bIsButtonDown)
			{
				if (m_pWBObject->GetType() == WB_OBJ_FLASH)
				{
					if (((CWBFlash *)m_pWBObject)->m_bIsPause)
					{
						m_pImagePushStart->Draw(hdc, m_pWBObject->GetAlpha());				
					} 
					else
					{
						m_pImagePushPause->Draw(hdc, m_pWBObject->GetAlpha());
					}
				}
			}
			else 
			{
				if (m_pWBObject->GetType() == WB_OBJ_FLASH)
				{

					if (((CWBFlash *)m_pWBObject)->m_bIsPause || !((CWBFlash *)m_pWBObject)->m_bIsRunning)
					{
						m_pImageStart->Draw(hdc, m_pWBObject->GetAlpha());
					}
					else 
					{
						m_pImagePause->Draw(hdc, m_pWBObject->GetAlpha());				
					}
				}
			}
		}
	}
	//恢复坐标系
	if (nGraphicsMode >= 0)
	{
		ResumeXForm(nGraphicsMode);
	}
}

BOOL CWBControlButton::IsInButtonArea(const CPoint& pt)
{
	if (m_pWBObject == NULL)
	{
		return FALSE;
	}

	if (m_pWBObject->GetType() == WB_OBJ_BAFFLE)
	{
		//判断点是否在图片的区域内
		if (m_pImageTopHandle->m_rect.PtInRect(pt))
		{
			m_nType = BTN_TYPE_TOP;
			return TRUE;
		}
		if (m_pImageLeftHandle->m_rect.PtInRect(pt))
		{
			m_nType = BTN_TYPE_LETE;
			return TRUE;
		}
		if (m_pImageRightHandle->m_rect.PtInRect(pt))
		{
			m_nType = BTN_TYPE_RIGHT;
			return TRUE;
		}
		if (m_pImageBottomHandle->m_rect.PtInRect(pt))
		{
			m_nType = BTN_TYPE_BOTTOM;
			return TRUE;
		}
		if (m_pImageBaffleClose->m_rect.PtInRect(pt))
		{
			m_nType = BTN_TYPE_CLOSEPATETTE;
			return TRUE;
		}

	}
	else if (m_pWBObject->GetType() == WB_OBJ_MAQN)
	{
		//判断点是否在图片的区域内
		if (m_pImageMaqnClose->m_rect.PtInRect(pt))
		{
			m_nType = BTN_TYPE_MAQNCLOSE;
			return TRUE;
		}
	}
	else
	{
		CPoint ptReserve, point, ptCenter;
		point = pt;
		//根据对象的旋转角度，换算鼠标击中的点
		m_pWBObject->GetRotationCenter(ptCenter);
		PointRotationPoint(ptCenter, -m_pWBObject->GetRotationAngle(), point, ptReserve);

		if (m_pWBObject->GetType() == WB_OBJ_DOCUMENT)
		{
			//判断点是否在图片的区域内
			if (m_pImageFirstPage->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_FIRSTPAGE;
				return TRUE;
			}
			if (m_pImagePrePage->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_PREPAGE;
				return TRUE;
			}
			if (m_pImageNextPage->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_NEXTPAGE;
				return TRUE;
			}
			if (m_pImageLastPage->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_LASTPAGE;
				return TRUE;
			}
			if (!g_bSupportU7OpenFile) //在U7中不显示打印按钮
			{
				if (m_pImagePrintCur->m_rect.PtInRect(ptReserve))
				{
					m_nType = BTN_TYPE_PRINTPAGE;
					return TRUE;
				}
			}
		}
		else if (m_pWBObject->GetType() == WB_OBJ_DXF)
		{
			//判断点是否在图片的区域内
			if (m_pImageDxfZoomIn->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_DXF_ZOOMIN;
				return TRUE;
			}
			if (m_pImageDxfZoomOut->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_DXF_ZOOMOUT;
				return TRUE;
			}
			if (m_pImageDxfMove->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_DXF_MOVE;
				return TRUE;
			}
		}
		else if (m_pWBObject->GetType() == WB_OBJ_MIDI)
		{
			if (m_pImageDxfZoomIn->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_DXF_ZOOMIN;
				return TRUE;
			}
			if (m_pImageDxfZoomOut->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_DXF_ZOOMOUT;
				return TRUE;
			}
			if (m_pImageDxfMove->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_DXF_MOVE;
				return TRUE;
			}
			if (m_pImageMidiPrint->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_PRINTPAGE;
				return TRUE;
			}
			if (m_pImageMidiFirst->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_MIDI_FIRST;
				return TRUE;
			}
			if (m_pImageMidiPre->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_MIDI_PRE;
				return TRUE;
			}
			if (m_pImageMideNext->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_MIDI_NEXT;
				return TRUE;

			}
			if (m_pImageMidiLast->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_MIDI_LAST;
				return TRUE;
			}
		}
		else
		{
			//判断点是否在图片的区域内
			if (m_pImageStart->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_START;
				return TRUE;
			}
			if (m_pWBObject->GetType() == WB_OBJ_FLASH)
			{
				if (((CWBFlash *)m_pWBObject)->m_bIsRunning)
				{
					if (m_pImageStop->m_rect.PtInRect(ptReserve))
					{
						m_nType = BTN_TYPE_STOP;
						return TRUE;
					}
				}
				if (m_pImageCatch->m_rect.PtInRect(ptReserve))
				{
					m_nType = BTN_TYPE_CATCH;
					return TRUE;
				}
			}

			if (m_pImageCircle->m_rect.PtInRect(ptReserve))
			{
				m_nType = BTN_TYPE_CIRCLE;
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CWBControlButton::FlashTip(CPoint point)
{	
	if (m_pImageStart->m_rect.PtInRect(point))
	{
		if (!infrect)
		{
			if (!((CWBFlash *)m_pWBObject)->m_bIsRunning || ((CWBFlash *)m_pWBObject)->m_bIsStop)
			{
				CString str = _T("");
				str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_play"));
				if (str.IsEmpty())
				{
					str = _T("播放");
				}

				CWBToolTip::GetIns()->SetText(str);
				POINT pt;
				::GetCursorPos(&pt);
				CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
				CWBToolTip::GetIns()->Display();
			}
			if (!((CWBFlash *)m_pWBObject)->m_bIsPause&&((CWBFlash *)m_pWBObject)->m_bIsRunning)
			{
				CString str = _T("");
				str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_pause"));
				if (str.IsEmpty())
				{
					str = _T("暂停");
				}


				CWBToolTip::GetIns()->SetText(str);
				POINT pt;
				::GetCursorPos(&pt);
				CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
				CWBToolTip::GetIns()->Display();
			}
		}
		infrect = TRUE;


	}


	else if (m_pImageStop->m_rect.PtInRect(point)||m_pImageIsStop->m_rect.PtInRect(point)||m_pImagePushStop->m_rect.PtInRect(point))
	{
		if (!infrect)
		{
			CString str = _T("");
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_stop"));
			if (str.IsEmpty())
			{
				str = _T("停止");
			}


			CWBToolTip::GetIns()->SetText(str);
			POINT pt;
			::GetCursorPos(&pt);
			CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
			CWBToolTip::GetIns()->Display();
		}

		infrect = TRUE;
	}

	else if(m_pImageIsCircle->m_rect.PtInRect(point))
	{
		if (!infrect)
		{
			if (((CWBFlash *)m_pWBObject)->m_bIsCircle)
			{
				CString str = _T("");
				str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_loop"));
				if (str.IsEmpty())
				{
					str = _T("循环");
				}


				CWBToolTip::GetIns()->SetText(str);
				POINT pt;
				::GetCursorPos(&pt);
				CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
				CWBToolTip::GetIns()->Display();
			}
			else
			{
				CString str = _T("");
				str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_singlePlay"));
				if (str.IsEmpty())
				{
					str = _T("单放");
				}

				CWBToolTip::GetIns()->SetText(str);
				POINT pt;
				::GetCursorPos(&pt);
				CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
				CWBToolTip::GetIns()->Display();
			}
		}

		infrect = TRUE;


	}

	else if (m_pImageCatch->m_rect.PtInRect(point))
	{

		if (!infrect)
		{
			CString str = _T("");
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_photograph"));
			if (str.IsEmpty())
			{
				str = _T("拍照");
			}

			CWBToolTip::GetIns()->SetText(str);
			POINT pt;
			::GetCursorPos(&pt);
			CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
			CWBToolTip::GetIns()->Display();
		}
		infrect = TRUE;
	}

	else
	{
		infrect = FALSE ;
		CWBToolTip::GetIns()->Restore();
	}

}

int CWBControlButton::RotateXForm(double dbAngle)
{
	if (m_pDC == NULL || dbAngle == 0)
	{
		return -1;
	}

	HDC hDC = m_pDC->GetSafeHdc();

	//取对象旋转中心点
	CPoint ptCenter;
	m_pWBObject->GetRotationCenter(ptCenter);

	//旋转
	int nGraphicsMode = SetGraphicsMode(hDC, GM_ADVANCED);

	XFORM xform;
	double fangle = -dbAngle / 180. * PI;

	//加缩放因子
	xform.eM11 = (float)cos(fangle);
	xform.eM12 = (float)sin(fangle); 
	xform.eM21 = (float)-sin(fangle);
	xform.eM22 = (float)cos(fangle);
	xform.eDx = (float)(ptCenter.x - cos(fangle)*ptCenter.x + sin(fangle)*ptCenter.y);
	xform.eDy = (float)(ptCenter.y - cos(fangle)*ptCenter.y - sin(fangle)*ptCenter.x);

	SetWorldTransform(hDC, &xform); 

	return nGraphicsMode;
}

void CWBControlButton::ResumeXForm(int nGraphicsMode)
{
	if (m_pDC == NULL)
	{
		return;
	}

	HDC hDC = m_pDC->GetSafeHdc();

	// 恢复DC	
	XFORM xform;

	xform.eM11 = (float)1.0; 
	xform.eM12 = (float)0;
	xform.eM21 = (float)0;
	xform.eM22 = (float)1.0;
	xform.eDx = (float)0;
	xform.eDy = (float)0;            
	SetWorldTransform(hDC, &xform);

	SetGraphicsMode(hDC, nGraphicsMode);
}

void CWBControlButton::DocTip( CPoint point )
{
	CPoint ptReserve,ptCenter;	
	m_pWBObject->GetRotationCenter(ptCenter);
	PointRotationPoint(ptCenter,-m_pWBObject->GetRotationAngle(),point,ptReserve);
	bool bshowtip = true;
	CString str = _T("");
	bool bfirstpage,blastpage,bprepage,bnextpage,bprintpage;
	bfirstpage = blastpage = bprepage = bnextpage = bprintpage = false;
	CWBDocument *pDoc = (CWBDocument*)m_pWBObject;
	if (pDoc->GetPageCount() >= 1)
	{
		if (pDoc->GetCurPageIndex() <= 1)
		{
			bnextpage = blastpage = true;
		}
		else if (pDoc->GetCurPageIndex() >= pDoc->GetPageCount())
		{
			bfirstpage = bprepage = true;
		}
		else
		{
			bfirstpage = blastpage = bprepage = bnextpage = true;
		}
	}
	if (!g_bSupportU7OpenFile)
	{
		bprintpage = true;
	}
	if (m_pImageFirstPage->m_rect.PtInRect(ptReserve))
	{
		if (m_bShowDocTip == 1)
		{
			return;
		}
		m_bShowDocTip = 1;
		if (bfirstpage)
		{

			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("doc_firstPage"));
			if (str.IsEmpty())
			{
				str = _T("首页");
			}

		}
		else
		{
			bshowtip = false;
		}		
	}
	else if (m_pImageLastPage->m_rect.PtInRect(ptReserve))
	{
		if (m_bShowDocTip == 2)
		{
			return;
		}
		m_bShowDocTip = 2;
		if (blastpage)
		{
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("doc_endPage"));
			if (str.IsEmpty())
			{
				str = _T("末页");
			}
		}
		else
		{
			bshowtip = false;
		}
	}
	else if (m_pImagePrePage->m_rect.PtInRect(ptReserve))
	{
		if (m_bShowDocTip == 3)
		{
			return;
		}
		m_bShowDocTip = 3;
		if (bprepage)
		{
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("doc_pageUp"));
			if (str.IsEmpty())
			{
				str = _T("上一页");
			}

		}
		else
		{
			bshowtip = false;
		}
	}
	else if (m_pImageNextPage->m_rect.PtInRect(ptReserve))
	{
		if (m_bShowDocTip == 4)
		{
			return;
		}
		m_bShowDocTip = 4;
		if (bnextpage)
		{
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("doc_nextPage"));
			if (str.IsEmpty())
			{
				str = _T("下一页");
			}			
		}
		else
		{
			bshowtip = false;
		}
	}
	else if (m_pImagePrintCur->m_rect.PtInRect(ptReserve))
	{
		if (m_bShowDocTip == 5)
		{
			return;
		}
		m_bShowDocTip = 5;
		if (bprintpage)
		{
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("docprint_thisPage"));
			if (str.IsEmpty())
			{
				str = _T("打印当前页");
			}
		}
		else
		{
			bshowtip = false;
		}
	}
	else
	{
		m_bShowDocTip = 0;
		bshowtip = false;
	}

	if (bshowtip)
	{
		CWBToolTip::GetIns()->SetText(str);
		POINT pt;
		::GetCursorPos(&pt);
		CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
		CWBToolTip::GetIns()->Display();
	}
	else
	{
		CWBToolTip::GetIns()->Restore();
	}
}

void CWBControlButton::MidiTip( CPoint point )
{
	CPoint ptReserve,ptCenter;	
	m_pWBObject->GetRotationCenter(ptCenter);
	PointRotationPoint(ptCenter,-m_pWBObject->GetRotationAngle(),point,ptReserve);
	bool bshowtip = true;
	CString str = _T("");
	
	CWBMidi *pDoc = (CWBMidi*)m_pWBObject;
	
	if (m_pImageMidiPrint->m_rect.PtInRect(ptReserve))
	{
		str = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_PRINT"));
		if (str.IsEmpty())
		{
			str = _T("打印");
		}				
	}	
	else if (m_pImageDxfZoomIn->m_rect.PtInRect(ptReserve))
	{		
		str = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_MAG"));
		if (str.IsEmpty())
		{
			str = _T("放大");
		}		
	}
	else if (m_pImageDxfZoomOut->m_rect.PtInRect(ptReserve))
	{
		str = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_REG"));
		if (str.IsEmpty())
		{
			str = _T("缩小");
		}		
	}
	else if (m_pImageDxfMove->m_rect.PtInRect(ptReserve))
	{
		str = ReadConfigText(_T("WhiteBoardControlText"),_T("TYPE_MOVE"));
		if (str.IsEmpty())
		{
			str = _T("移动");
		}
	}
	else
	{		
		bshowtip = false;
	}

	if (bshowtip)
	{
		CWBToolTip::GetIns()->SetText(str);
		POINT pt;
		::GetCursorPos(&pt);
		CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
		CWBToolTip::GetIns()->Display();
	}
	else
	{
		CWBToolTip::GetIns()->Restore();
	}
}

/*-----------------------------------------------------------------------------------
// CWBControlButton
------------------------------------------------------------------------------------*/

