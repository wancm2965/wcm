
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
#define MOUSE_MOVE						   0x0200		//����ƶ�
#define MOUSE_LBUTTONDOWN                  0x0201		//������������
#define MOUSE_LBUTTONUP                    0x0202		//����������
#define MOUSE_LBUTTONDBLCLK                0x0203		//������˫��
#define MOUSE_RBUTTONDOWN                  0x0204		//����Ҽ�������
#define MOUSE_RBUTTONUP                    0x0205		//����Ҽ�����
#define MOUSE_RBUTTONDBLCLK                0x0206		//����Ҽ�˫��
#define MOUSE_MBUTTONDOWN                  0x0207		//����м�������
#define MOUSE_MBUTTONUP                    0x0208		//����м�����
#define MOUSE_MBUTTONDBLCLK                0x0209		//����м�˫��	
#define MOUSE_WHEEL						   0x020A		//������ת��

#if (_WIN32_WINNT >= 0x0500)
#define MOUSE_XBUTTONDOWN                  0x020B		//������
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
// ����
HPDLG_API HPDLG_NetBridge* CreateHPDLG_NetBridge(void);

// ���ָ��
HPDLG_API HPDLG_NetBridge* GetHPDLG_NetBridge(void);

// ����
HPDLG_API void DestroyHPDLG_NetBridge();
