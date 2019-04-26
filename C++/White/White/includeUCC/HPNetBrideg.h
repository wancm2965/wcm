
#pragma once

#define USE_HPNETBRIDEG
#undef USE_HPNETBRIDEG

#include "HPDLG_Base.h"

typedef struct  
{
	float	px;
	float	py;
}mpos,lpmpos;

/************************************************************************/
/* mouse event                                                          */
/************************************************************************/

#define MOUSE_FIRST						   0x0200
#define MOUSE_MOVE						   0x0200		//鼠标移动
#define MOUSE_LBUTTONDOWN                  0x0201		//鼠标左键被按下
#define MOUSE_LBUTTONUP                    0x0202		//鼠标左键弹起
#define MOUSE_LBUTTONDBLCLK                0x0203		//鼠标左键双击
#define MOUSE_RBUTTONDOWN                  0x0204		//鼠标右键被按下
#define MOUSE_RBUTTONUP                    0x0205		//鼠标右键弹起
#define MOUSE_RBUTTONDBLCLK                0x0206		//鼠标右键双击
#define MOUSE_MBUTTONDOWN                  0x0207		//鼠标中键被按下
#define MOUSE_MBUTTONUP                    0x0208		//鼠标中键弹起
#define MOUSE_MBUTTONDBLCLK                0x0209		//鼠标中键双击	
#define MOUSE_WHEEL						   0x020A		//鼠标滚轮转动

#if (_WIN32_WINNT >= 0x0500)
#define MOUSE_XBUTTONDOWN                  0x020B		//五键鼠标
#define MOUSE_XBUTTONUP                    0x020C
#define MOUSE_XBUTTONDBLCLK                0x020D


/* XButton values are WORD flags */
#define XBUTTON1      0x0001
#define XBUTTON2      0x0002
/* Were there to be an XBUTTON3, its value would be 0x0004 */
#endif

/************************************************************************/
/* keyboard event                                                       */
/************************************************************************/
#define KEYBD_KEYDOWN							0x0100
#define KEYBD_KEYUP								0x0101
#define KEYBD_SYSKEYDOWN						0x0102
#define KEYBD_SYSKEYUP							0x0103

class AFX_EXT_CLASS HPDLG_NetBridge
{
public:
	HPDLG_NetBridge(void){};
	virtual~ HPDLG_NetBridge(void){};

public:
	void InitNetBridge(void);
	void CreateMsg(short MsgType,mpos pos,short mv=0);
	void InstallHook(void);
	void UnHook(void);
};
// 创建
HPDLG_API HPDLG_NetBridge* CreateHPDLG_NetBridge(void);

// 获得指针
HPDLG_API HPDLG_NetBridge* GetHPDLG_NetBridge(void);

// 销毁
HPDLG_API void DestroyHPDLG_NetBridge();
