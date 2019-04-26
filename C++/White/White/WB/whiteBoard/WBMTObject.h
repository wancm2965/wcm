#ifndef _AVCON_WHITEBOARD_WBMTOBJECT_H_
#define _AVCON_WHITEBOARD_WBMTOBJECT_H_
#include "MTGestureImport.h"
#include <map>

#ifdef SUPPORTMTGES
class CWBMTObject 
#else
class CWBMTObject : public AVCON_SDK_MTTouch::CMTObject
#endif
//class CWBMTObject : public AVCON_SDK_MTTouch::CMTObject

{
protected:
	CWBMTObject();
	~CWBMTObject();

protected:
	//计数器进位判断
	bool  DoesCarryCounting()const;
public:
	//最后一个手指弹起时（待子类重写）[添加了一个参数，弹起点的位置判断是否在垃圾箱内]
	virtual void TouchUpAllCursor(bool bchangID, const AVCON_SDK_MTTouch::TouchUp& te){ m_bCounting = 0;}
protected:
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);

protected:
	//触摸弹起
	virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);
	virtual void touchDownHandler(const TouchDown& te);
	virtual void touchMoveHandler(const TouchMove& te);

// 	virtual void addMouseCursor(int xpos, int ypos){};
// 	virtual void removeMouseCursor(int xpos, int ypos){};
// 	virtual void updateMouseCursor(int xpos, int ypos){};

protected:
	//ID到触摸对象的的映射(可用list代替)
	std::map<unsigned long, unsigned long> m_map;
	//计数
	unsigned short m_bCounting;

	//判断触摸操作是否有改变
	bool m_bChangID;
};

#endif