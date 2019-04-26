
#include "stdafx.h"
#include "GlobalObjectMng.h"

////class CDataBridge;
//CC3CGestureType::CC3CGestureType()
//	: m_gestureConfig(0)
//{
//}
//
//CC3CGestureType::~CC3CGestureType()
//{
//}
//
//CC3CGestureType CC3CGestureType::m_instance;
CGlobalObjMng CGlobalObjMng::m_instance;
//
//CC3CGestureType * CC3CGestureType::GetInstance()
//{
//	return &m_instance;
//}
//
//void CC3CGestureType::Config( unsigned int type )
//{
//	m_gestureConfig = 0;
//	if (type & Gesture_Translate)
//	{
//		m_gestureConfig |= Gesture_Translate;
//	}
//	
//	//在设置时就让 zoom 和 rotate 相互排斥
//	if (type & Gesture_Zoom)
//	{
//		m_gestureConfig |= Gesture_Zoom;
//	}
//
//	if (type & Gesture_Rotate)
//	{
//		m_gestureConfig |= Gesture_Rotate;
//	}
//}

CGlobalObjMng::CGlobalObjMng()
	: m_hWnd(NULL)
	, m_bMTAuthorize(0)
{

}

void CGlobalObjMng::ConfigWBParentHwnd( HWND hWnd )
{
	m_hWnd = hWnd;
}

CGlobalObjMng * CGlobalObjMng::GetInstance()
{
	return &m_instance;
}

CGlobalObjMng::~CGlobalObjMng()
{

}
