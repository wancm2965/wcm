#pragma once
#include "WZoomCtrl.h"
#include "WMBtn.h"
class CWZoomCtrl;
class CWMBtn;
#define  BTN_RECTMAQ_CLOSE 9
#define  INSIDERECT     30
class CWBRectMaqn
{
public:
	CWBRectMaqn(void);
	CWBRectMaqn(CRect rc,CWZoomCtrl*&ViewZoomCtrl);
	virtual ~CWBRectMaqn(void);

public: 

	void  DrawRectMaqn(CDC* pDC, CPoint point);
	void  MoveRect(CPoint ptMovePoint);
	int   PointIsInSide(CPoint ptPoint);
	int   PointIn(CPoint ptPoint);
	void  GetViewWZoomCtrl(CWZoomCtrl*& ZWoomCtrl);
	BOOL  IsDeleteWZoomCtrl();
	void  MoveZoom(CPoint ptMovePoint);
	void  MoveCloseBtn(CPoint ptMovePoint);


public:
	int m_nZoom;  //·Å´ó±¶Êý
public:

	CWZoomCtrl*    m_WZoomCtrl;
	CWMBtn* m_CloseRectBtn;
	CRect  m_rect;
	CPoint m_Point;
	CPoint m_prePoint;
	int m_count;

};

