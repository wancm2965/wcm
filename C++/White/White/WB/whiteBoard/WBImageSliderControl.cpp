#include "StdAfx.h"
#include "WBImageSliderControl.h"
#include "whiteBoard.h"

//控制控制块在控制条中的相对位置，间隔距离
//const int LEFT_PADDING_SLIDER = 8;
//const int RIGHT_PADDING_SLIDER = 8;
//const int TOP_PADDING_SLIDER = 2;
//const int BOTTOM_PADDING_SLIDER = 5;


CWBImageSliderControl::CWBImageSliderControl(CWBObject *pWBObject)
{
	m_pWBObject = pWBObject;

	m_pImgBk = NULL;
	m_pImgThumb = NULL;
	m_pImgThumbSel = NULL;

	m_nLeftPadding = 0;
	m_nRightPadding = 0;
	m_nTopPadding = 0;
	m_nBottomPadding = 0;

	m_pDC = NULL;

	m_bIsMouseDown = FALSE;
	m_bMousePos = FALSE;
	m_bIsBlockChangeColor = FALSE;
	
	m_ptBeginPoint = CPoint(0,0);
	m_ptCurPoint = CPoint(0,0);

	m_nCurPos = 0;
	m_nMaxPos = 100;
	m_nMinPos = 0;
	m_nSelBeginPos = 0;
	m_nSelEndPos = 0;
	m_nTicFreq = 5;

	m_rtRect = CRect(0,0,0,0);
	m_rtThumbRect = CRect(0,0,0,0);
	m_testValue = 0;
}

CWBImageSliderControl::~CWBImageSliderControl(void)
{
	if (NULL != m_pImgBk)
	{
		delete m_pImgBk;
		m_pImgBk = NULL;
	}
	if (NULL != m_pImgThumb)
	{
		delete m_pImgThumb;
		m_pImgThumb = NULL;
	}
	if (NULL != m_pImgThumbSel)
	{
		delete m_pImgThumbSel;
		m_pImgThumbSel = NULL;
	}
}

//从文件初始化图片
void CWBImageSliderControl::InitImageFromFile(CString bkFileName, CString thumbFileName, CString thumbSelFileName)
{
	//wchar_t tmpFileName[MAX_PATH];
	//MultiByteToWideChar(CP_ACP, 0, bkFileName, -1, tmpFileName, MAX_PATH);
	m_pImgBk = new ImageEx(bkFileName);	
	//m_pImgBk->InitAnimation();

	//MultiByteToWideChar(CP_ACP, 0, thumbFileName, -1, tmpFileName, MAX_PATH);
	m_pImgThumb = new ImageEx(thumbFileName);	
	//m_pImgThumb->InitAnimation();

	//MultiByteToWideChar(CP_ACP, 0, thumbSelFileName, -1, tmpFileName, MAX_PATH);
	m_pImgThumbSel = new ImageEx(thumbSelFileName);	
	//m_pImgThumbSel->InitAnimation();
}

//从资源初始化图片
void CWBImageSliderControl::InitImageFromResource(CString imgType, CString bkID, CString thumbID, CString thumbSelID)
{
	m_pImgBk = new ImageEx(imgType, bkID);	
	m_pImgThumb = new ImageEx(imgType, thumbID);
	m_pImgThumbSel = new ImageEx(imgType, thumbSelID);
	m_pImgBk->GetRect();
}

//按钮按下进行的操作
BOOL CWBImageSliderControl::MouseDown(const CPoint& point)
{
	//根据鼠标点在进度条上的位置进行操作
	m_bMousePos = IsInSliderArea(point);
	if (!m_bMousePos)
	{
		return FALSE;
	}

	m_ptBeginPoint = point;
	m_ptCurPoint = point;
	m_nSelBeginPos = m_nCurPos;
	m_bIsMouseDown = TRUE;
	if (ON_SLIDER == m_bMousePos)
	{
		m_bIsBlockChangeColor = TRUE;
	}
	////
	CString msg;
	switch (m_bMousePos)
	{
	case LEFT_SLIDER:
		//进度条左移
		m_nCurPos -= m_nTicFreq;
		if (m_nCurPos < m_nMinPos)
		{
			m_nCurPos = m_nMinPos;
		}
		//Draw(m_pDC);
		break;

	case RIGHT_SLIDER:
		m_nCurPos += m_nTicFreq;
		if (m_nCurPos > m_nMaxPos)
		{
			m_nCurPos = m_nMaxPos;
		}
		//Draw(m_pDC);
		break;

	case ON_SLIDER:
		//Draw(m_pDC);
		m_testValue++;
		msg.Format(_T("%d current pos is: %d\r\n"),m_testValue,m_nCurPos);
		OutputDebugString((LPCTSTR)msg);
		break;

	default:
		break;
	}
	//如果鼠标点击在有效区域外，返回FALSE，否则返回TRUE
	if (OUT_SLIDER != m_bMousePos && MARGIN_SLIDER != m_bMousePos)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	//g_pWhiteBoardView->Invalidate();
}

//按钮移动进行的操作
BOOL CWBImageSliderControl::MouseMove(const CPoint& point)
{
	if (!m_bIsMouseDown)
	{
		return FALSE;
	}
	//计算当前光标所在的逻辑位置
	int moveLen = point.x - m_rtRect.left - m_nLeftPadding;
	int totalLen = m_rtRect.Width() - m_nLeftPadding - m_nRightPadding;
	if (moveLen < 0 )
	{
		moveLen = 0;
	}
	else if (moveLen > totalLen)
	{
		moveLen = totalLen;
	}
	int nCurPos =  (int)((m_nMaxPos-m_nMinPos)*(moveLen/(totalLen*1.0)));
	////
	/*CString msg;
	msg.Format(_T("当前逻辑位置是：%d\r\n"),nCurPos);
	OutputDebugString(msg);*/

	//更改逻辑位置，移动滑动块
	switch (m_bMousePos)
	{
	case LEFT_SLIDER:
		{
			if (!IsInSliderArea(point))
			{
				return FALSE;
			}

			m_nCurPos -= m_nTicFreq;
			if (m_nCurPos < nCurPos)
			{
				m_nCurPos = nCurPos;
			}
			//Draw(m_pDC);
			//g_pWhiteBoardView->Invalidate();
		}
		break;

	case RIGHT_SLIDER:
		{
			if (!IsInSliderArea(point))
			{
				return FALSE;
			}
			m_nCurPos += m_nTicFreq;
			if (m_nCurPos > nCurPos)
			{
				m_nCurPos = nCurPos;
			}
			//Draw(m_pDC);
			//g_pWhiteBoardView->Invalidate();
		}
		break;

	case ON_SLIDER://鼠标在拖动块上,直接移动到光标位置
		{
			m_nCurPos = nCurPos;
			//Draw(m_pDC);
			//g_pWhiteBoardView->Invalidate();
		}
		break;

	default:
		break;
	}

	//如果鼠标点击在有效区域外，返回FALSE，否则返回TRUE
	if (OUT_SLIDER != m_bMousePos && MARGIN_SLIDER != m_bMousePos)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//按钮弹起进行的操作
BOOL CWBImageSliderControl::MouseUp(const CPoint& point)
{
	if (!m_bIsMouseDown)
	{
		return FALSE;
	}
	m_ptCurPoint = point;
	m_nSelEndPos = m_nCurPos;
	m_bIsMouseDown = FALSE;
	m_bMousePos = FALSE;
	if (m_bIsBlockChangeColor )
	{
		m_bIsBlockChangeColor = FALSE;
		return TRUE;
	}
	return FALSE;
}

//画进度条
void CWBImageSliderControl::Draw(CDC *pDC)
{
	if (NULL == pDC && NULL == m_pDC)
	{
		return;
	}
	HDC hdc;
	if (pDC)
	{
		hdc = pDC->GetSafeHdc();
		m_pDC = pDC;
	}
	else
	{
		hdc = m_pDC->GetSafeHdc();
	}
	
	//假如该控制条有依附对象，则根据依附对象更新其坐标
	/*if (NULL != m_pWBObject)
	{
	GetCurRectFromWBObject();
	}*/
	
	//画控制条背景和控制块
	m_pImgBk->Draw(hdc,m_rtRect);
	if (m_bIsBlockChangeColor)
	{
		m_pImgThumbSel->Draw(hdc,m_rtThumbRect);
	}
	else
	{
		m_pImgThumb->Draw(hdc,m_rtThumbRect);
	}
	
}

//判断鼠标是否在进度条坐标内,在执行这个函数之前，保证当前活动条坐标是最新的
//分为四种状态，分别为在进度条外，进度块左，进度块右，进度块上
BOOL CWBImageSliderControl::IsInSliderArea(const CPoint& pt)
{
	
	/*if (NULL != m_pWBObject)
	{
	GetCurRectFromWBObject();
	}*/
	
	if (!m_rtRect.PtInRect(pt))
	{
		return OUT_SLIDER;
	}
	//三个判断区域限定在滑动块移动的区域
	CRect rtLeft,rtCenter,rtRight;
	rtLeft.top = m_rtRect.top + m_nTopPadding;
	rtLeft.left = m_rtRect.left + m_nLeftPadding;
	rtLeft.bottom  = m_rtRect.bottom - m_nBottomPadding;
	rtLeft.right = m_rtThumbRect.left;

	rtCenter.top = rtLeft.top;;
	rtCenter.left = m_rtThumbRect.left;
	rtCenter.bottom = rtLeft.bottom;
	rtCenter.right =  m_rtThumbRect.right;

	rtRight.top = rtLeft.top;
	rtRight.left = m_rtThumbRect.right;
	rtRight.bottom = rtLeft.bottom;
	rtRight.right = m_rtRect.right - m_nRightPadding;

	if (rtLeft.PtInRect(pt))
	{
		return LEFT_SLIDER;
	}
	if (rtCenter.PtInRect(pt))
	{
		return ON_SLIDER;
	}
	if (rtRight.PtInRect(pt))
	{
		return RIGHT_SLIDER;
	}

	return MARGIN_SLIDER;//在滑动条内，但是不再有效区域内
}

/*
//从依附对象获取控制条坐标
void CWBImageSliderControl::GetCurRectFromWBObject()
{
	
	if (m_pWBObject->GetType() == WB_OBJ_MAQN)
	{
		CRect rect;
		((CWBRectMaqn*)m_pWBObject)->GetRect(rect);

		m_rtRect.top = rect.bottom + 12;
		m_rtRect.bottom = m_rtRect.top + m_pImgBk->GetRect().Height();
		m_rtRect.left = rect.left;
		m_rtRect.right = m_rtRect.left + m_pImgBk->GetRect().Width();

		//得到滑动槽的坐标
		CRect traceBarRect;
		traceBarRect.top = m_rtRect.top + m_nTopPadding;
		traceBarRect.bottom = traceBarRect.top + m_pImgThumb->GetRect().Height();
		traceBarRect.left = m_rtRect.left + m_nLeftPadding;
		traceBarRect.right = m_rtRect.right - m_nRightPadding;

		//滑块在滑动槽中横向滑动
		int leftLen = traceBarRect.Width()*(m_nCurPos/((m_nMaxPos - m_nMinPos)*1.0));
		m_rtThumbRect.top = traceBarRect.top;
		m_rtThumbRect.bottom = traceBarRect.bottom;
		m_rtThumbRect.left = traceBarRect.left + leftLen;
		if (m_rtThumbRect.left > (traceBarRect.right- m_pImgThumb->GetRect().Width()) )
		{
			m_rtThumbRect.left = traceBarRect.right- m_pImgThumb->GetRect().Width();
		}
		m_rtThumbRect.right = m_rtThumbRect.left + m_pImgThumb->GetRect().Width();
		
	}
	

}
*/
//获取进度条的坐标
void CWBImageSliderControl::GetRect(LPRECT lprc)
{
	lprc = m_rtRect;
}

////设置进度条的坐标
void CWBImageSliderControl::SetRect(RECT& lprc)
{
	m_rtRect = lprc;
	//计算滑动块的坐标
	//得到滑动槽的坐标
	CRect traceBarRect;
	traceBarRect.top = m_rtRect.top + m_nTopPadding;
	traceBarRect.bottom = traceBarRect.top + m_pImgThumb->GetRect().Height();
	traceBarRect.left = m_rtRect.left + m_nLeftPadding;
	traceBarRect.right = m_rtRect.right - m_nRightPadding;

	//滑块在滑动槽中横向滑动
	int leftLen = (int)(traceBarRect.Width()*(m_nCurPos/((m_nMaxPos - m_nMinPos)*1.0)));
	m_rtThumbRect.top = traceBarRect.top;
	m_rtThumbRect.bottom = traceBarRect.bottom;
	m_rtThumbRect.left = traceBarRect.left + leftLen;
	if (m_rtThumbRect.left > (traceBarRect.right- m_pImgThumb->GetRect().Width()) )
	{
		m_rtThumbRect.left = traceBarRect.right- m_pImgThumb->GetRect().Width();
	}
	m_rtThumbRect.right = m_rtThumbRect.left + m_pImgThumb->GetRect().Width();
}

////获取拖动图标的坐标
//void CWBImageSliderControl::GetThumbRect(LPRECT lprc)
//{
//	lprc = m_rtThumbRect;
//}
//
////设置拖动图标的坐标,该坐标是相对于滑动槽左上角的坐标
//void CWBImageSliderControl::SetThumbRect(LPRECT lprc)
//{
//	m_rtThumbRect = lprc;
//}

//获取滑块的当前逻辑位置
int CWBImageSliderControl::GetPos() const
{
	return m_nCurPos;
}

//设置滑块的逻辑位置
void CWBImageSliderControl::SetPos(int nPos)
{
	if (nPos < m_nMinPos || nPos > m_nMaxPos)
	{
		OutputDebugString(_T("CWBImageSliderControl::SetPos: 参数不合法！\n"));
		return;
	}
	m_nCurPos = nPos;
	//跟新显示

}

//获取滑块的最大逻辑值
int CWBImageSliderControl::GetRangeMax() const
{
	return m_nMaxPos;
}

//设置滑块的最大逻辑值
void CWBImageSliderControl::SetRangeMax(int nMax)
{
	m_nMaxPos = nMax;
}

//获取滑块的最小逻辑值
int CWBImageSliderControl::GetRangeMin() const
{
	return m_nMinPos;
}

//设置滑块的最小逻辑值
void CWBImageSliderControl::SetRangeMin(int nMin)
{
	m_nMinPos = nMin;
}

//获取滑块的最小和最大逻辑值
void CWBImageSliderControl::GetRange(int& nMin,int& nMax)const
{
	nMin = m_nMinPos;
	nMax = m_nMaxPos;
}

//设置滑块的最小和最大逻辑值
void CWBImageSliderControl::SetRange(int nMin, int nMax)
{
	if (nMin < 0 || nMax < 0 || nMax < nMin)
	{
		OutputDebugString(_T("CWBImageSliderControl::SetRange: 参数不合法！\n"));
		return;
	}
	m_nMinPos = nMin;
	m_nMaxPos = nMax;
}

//获取当前拖动的起点和终点
void CWBImageSliderControl::GetSelection(int& nMin, int& nMax) const
{
	nMin = m_nSelBeginPos;
	nMax = m_nSelEndPos;
}

//设置拖动的起点和终点
void CWBImageSliderControl::SetSelection(int nMin, int nMax)
{
	if (nMin < m_nMinPos || nMax > m_nMaxPos || nMin > nMax)
	{
		OutputDebugString(_T("CWBImageSliderControl::SetSelection: 参数不合法！\n"));
		return;
	}
	m_nSelBeginPos = nMin;
	m_nSelEndPos = nMax;
}

//设置鼠标点击一次移动的的间隔
void CWBImageSliderControl::SetTicFreq(int nFreq)
{
	if (nFreq < 0 || nFreq > m_nMaxPos)
	{
		OutputDebugString(_T("CWBImageSliderControl::SetTicFreq: 参数不合法！\n"));
		return;
	}
	m_nTicFreq = nFreq;
}

//获取总的刻度数
int CWBImageSliderControl::GetNumTics() const
{
	return (m_nMaxPos-m_nMinPos)/m_nTicFreq;
}

//设置padding的值
void CWBImageSliderControl::SetPadding(UINT leftPadding, UINT topPadding, UINT rightPadding, UINT bottomPadding)
{
	m_nLeftPadding = leftPadding;
	m_nTopPadding = topPadding;
	m_nRightPadding = rightPadding;
	m_nBottomPadding = bottomPadding;
}

//获取padding的值
void CWBImageSliderControl::GetPadding(UINT& leftPadding, UINT& topPadding, UINT& rightPadding, UINT& bottomPadding)
{
	leftPadding = m_nLeftPadding;
	topPadding = m_nTopPadding;
	rightPadding = m_nRightPadding;
	bottomPadding = m_nBottomPadding;
}

BOOL CWBImageSliderControl::GetSize(UINT& width, UINT& height)
{
	if (!m_pImgBk)
	{
		return FALSE;
	}
	width = m_pImgBk->GetRect().Width();
	height = m_pImgBk->GetRect().Height();
	return TRUE;
}