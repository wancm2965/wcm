#ifndef _AVCON_CUSTOM_TOUCH_API_H_
#define _AVCON_CUSTOM_TOUCH_API_H_
//
//Ϊ�˼���֮ǰ��windows SDK �������µ�Ԥ���붨��
#if(WINVER < 0x0601)//����XP�ı���				                   
#define WM_GESTURE                      (0x0119)	//Windows SDK 7.0�еĶ���ֵ������ͬ
#define WM_TOUCH                        (0x0240)	//

DECLARE_HANDLE(HTOUCHINPUT);
typedef struct tagTOUCHINPUT {
    LONG x;
    LONG y;
    HANDLE hSource;
    DWORD dwID;
    DWORD dwFlags;
    DWORD dwMask;
    DWORD dwTime;
    ULONG_PTR dwExtraInfo;
    DWORD cxContact;
    DWORD cyContact;
} TOUCHINPUT, *PTOUCHINPUT;
typedef TOUCHINPUT const * PCTOUCHINPUT;

/*
 * Conversion of touch input coordinates to pixels
 */
#define TOUCH_COORD_TO_PIXEL(l)         ((l) / 100)

/*
 * Touch input flag values (TOUCHINPUT.dwFlags)
 */
#define TOUCHEVENTF_MOVE            (0x0001)
#define TOUCHEVENTF_DOWN            (0x0002)
#define TOUCHEVENTF_UP              (0x0004)
//#define TOUCHEVENTF_INRANGE         0x0008
// #define TOUCHEVENTF_PRIMARY         0x0010
//#define TOUCHEVENTF_NOCOALESCE      0x0020
//#define TOUCHEVENTF_PEN             0x0040
//#define TOUCHEVENTF_PALM            0x0080

/*
 * Touch input mask values (TOUCHINPUT.dwMask)
 */
//#define TOUCHINPUTMASKF_TIMEFROMSYSTEM  0x0001  // the dwTime field contains a system generated value
//#define TOUCHINPUTMASKF_EXTRAINFO       0x0002  // the dwExtraInfo field is valid
//#define TOUCHINPUTMASKF_CONTACTAREA     0x0004  // the cxContact and cyContact fields are valid

#define SM_DIGITIZER            (94)
#define SM_MAXIMUMTOUCHES       (95)
//#define MOUSEEVENTF_FROMTOUCH	(0xFF515700)
#endif

namespace CEncapsulateTouchAPI
{
	//��ȡ��������
	BOOL U_GetTouchInputInfo(const unsigned long &hTouchInput,
		UINT cInputs, PTOUCHINPUT pInputs, int cbSize);
	//�رվ��
	BOOL U_CloseTouchInputHandle(const unsigned long &hTouchInput);
	//ע�ᴥ������
	BOOL U_RegisterTouchWindow(HWND hwnd, ULONG ulFlags = 0);
	//��ע�ᴥ������
	BOOL U_UnRegisterTouchWindow(HWND hwnd);
	//�Ƿ��Ѿ�ע���˴�������
	BOOL U_IsTouchWindow(HWND hWnd);
	//����һЩ��������
	void DisableTheTabFeedback(HWND hWnd);
};

#endif//head file