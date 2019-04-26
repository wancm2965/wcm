#pragma once

#include "MTGestureImport.h"
#include <string>
#include "ResData.h"
#define  ID_COMMAND_CHANGE_RES_DIRECTORY_START	(12045)
#define		UWM_CHANGE_NUME	(WM_USER + 3879)
//特殊定制类（单独提出来，是为了避免和非MFC代码冲突，编译通不过）

#define CLICKSTATE_MOUSELEAVE (0x00)
#define CLICKSTATE_MOUSEHOVE (0x01)
#define CLICKSTATE_LBUTTONDOWN (0x02)

class CWBTouchClick : public AVCON_SDK_MTTouch::CMTObject
{
public:
	CWBTouchClick(HWND hWnd, ResMapArray *pResData);
	~CWBTouchClick();

public:
	void GetRectRegion(RECT *rct)const;
	//继承基类CMTObject 虚函数
	ID2D1Bitmap *m_pBitmap;
	Bitmap* m_pGdiPlusBmp;
	int m_State;
private:
	virtual void OnCallbackGesture(const unsigned int &nMessage, 
		const unsigned int &nAppendMessage = 0);
	virtual void touchDownHandler(const TouchDown& te);
	virtual void touchUpHandler(const TouchUp& te);
	virtual bool BInRegion(const POINT &pt)const;
private:
	//所关联的窗口句柄
	HWND m_hWnd;
	int m_bDown;
	//资源对象指针
	ResMapArray *m_pResData;
};