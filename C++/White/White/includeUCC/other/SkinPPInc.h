//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the Skin++ Library
// Copyright (C) 2002-2006 YongJinSoft Ltd.(http://www.uipower.com)
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************

#ifndef __SKINPPINC_H
#define __SKINPPINC_H

#if _MSC_VER >= 1000
#pragma once 
#endif // _MSC_VER >= 1000

#pragma pack(push, 8)

//����	:	����Ƥ��
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppLoadSkin(
	LPCTSTR strSkinFile,			//Ƥ��·��
	BOOL bWithoutHook = FALSE);		//�ò���ָ������Ƥ��ʱ�Ƿ���Hook����.

//����	:	�����Ƿ����Hook
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetHookMessage(
	BOOL bHook,							//TrueΪ����Hook����,FalseΪ�ر�Hook����.
	DWORD dwHookStyle = 0x00000000L);	//HS_MENU�����˵��Զ���������,
										//HS_WINDOW���������Զ���������,
										//HS_NONE�ر�����Hook,HS_ALL���������Զ���������.
										   
//����	:	����Դ����Ƥ��
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppLoadSkinFromRes(
	HINSTANCE hInstance,			//��Դʵ�����
	LPCTSTR szResourceName,			//��Դ����
	LPCTSTR szResourceType,			//��Դ����
	LPCTSTR szSkinName,				//Ƥ������
	BOOL bWithoutHook = FALSE);		//�Ƿ�Hook
											
//����	:	����HookAPI������
//����ֵ:	���ص�ǰAPI����
DWORD __stdcall sppModifyHookAPIStyle(
	DWORD dwRemove,					//�Ƴ�API����
	DWORD dwAdd);					//���API����

//����	:	��õ�ǰAPI����
//����ֵ:	���ص�ǰAPI����
DWORD __stdcall sppGetHookAPIStyle();

//����	:	�����Ƿ�HookAPI
//����ֵ:	��������ǰ��HookAPI״̬
BOOL __stdcall sppSetHookAPI(
	BOOL bHookAPI);					//TrueΪAPI Hook,FalseΪȥ��API Hook

//����	:	���HookAPI״̬
//����ֵ:	True��ʾ��ǰΪHookAPI,False��ʾ��ǰû��HookAPI
BOOL __stdcall sppGetHookAPI();

//����	:	�Ƴ�Ƥ��
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppRemoveSkin();

//����	:	���ñ������Զ��尴ť�Ƿ���ʾ
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetCustSysBtnVisible(
	UINT uResID,					//��ť��ResourceID
	BOOL bVisible);					//�Ƿ�ɼ�

//����	:	ָ���Զ���ϵͳ��ť��Check״̬
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetCustSysBtnStatus(
	UINT uResID,					//�Զ���ϵͳ��ť����ԴID��ֵ
	BOOL bChecked = TRUE);			//�Ƿ���״̬

//����	:	����Ƥ��ɫ��
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppColorize(
	COLORREF clr,					//Ƥ��ɫ�� ���clr==-1,�ͻָ�ԭ����Ч��
	double blend = 0.5,				//���͸���� 0-1 , 0Ϊ��Ч��,1Ϊ��͸��
	BOOL bShowDialog = FALSE);		//�Ƿ���ʾ��ɫѡ��Ի���

//����	:	�����ļ�ѡ���ѡ��Ƥ��
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSelectSkin(); 

//����	:	�ڸ�����HDC��,��λͼ��
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppDrawSkinBitmap(
	HDC hdc,						//Ŀ��DC
	RECT rect,						//��������
	HBITMAP hBitmap,				//Ҫ���Ƶ�λͼ���
	BOOL bStretchAll,				//�Ƿ�����
	int nTopHeight = 0,				//��������Ķ���
	int nBottomHeight = 0,			//��������ĵ׸�
	int nLeftWidth = 0,				//������������
	int nRightWidth = 0,			//����������ҿ�
	COLORREF clrTrans = -1,			//͸��ɫ
	BOOL bTrans = FALSE				//�Ƿ�͸��
	);

//����	:	�ڸ�����HDC��,��λͼ
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppDrawSkinImage(
	HDC hdc,						//Ŀ��DC
	RECT rect,						//��������
	LPCTSTR lpszObjName,			//Ҫ����Ƥ��ͼƬ����
	BOOL bStretchAll,				//�Ƿ�����
	int nLeftWidth,					//������������
	int nRightWidth,				//����������ҿ�
	int nTopHeight,					//��������Ķ���
	int nBottomHeight,				//��������ĵ׸�
	COLORREF clrTrans = -1,			//͸��ɫ
	BOOL bTrans = FALSE				//�Ƿ�͸��
	);

//����	:	��û������ϵͳ��ɫ
//����ֵ:	�ɹ����ص�ǰƤ������ɫ,ʧ�ܷ���-1
COLORREF __stdcall sppGetSkinSysColor(
	int nColorIndex);				//Ҫ��ȡ����ɫ����

//����	:	���WindowsϵͳĬ�ϵ���ɫ
//����ֵ:	�ɹ�����Windowsϵͳ��ɫ,ʧ�ܷ���-1
COLORREF __stdcall sppGetDefaultSysColor(
	int nColorIndex);				//Ҫ��ȡ����ɫ����

//����	:	���ô��ڵ���ԴID,��ID���ڶ����԰汾���õ�
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetWindowResID(
	HWND hWnd,						//�Ի��򴰿ڵľ��
	int nResID);					//�Ի�����ԴID

//����	:	��ô��ڵ���ԴID
//����ֵ:	�ɹ�������ԴID,ʧ�ܷ���-1
int __stdcall sppGetWindowResID(
	HWND hWnd);						//�Ի��򴰿ڵľ��

//����	:	�������ʹ��ڵ���ԴID
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetFreeWindowID(
	HWND hWnd,						//�Ի��򴰿ڵľ��
	int nResID);					//��ԴID

//����	:	������ʹ��ڵ���ԴID
//����ֵ:	�ɹ�������ԴID,ʧ�ܷ���-1
int __stdcall sppGetFreeWindowID(
	HWND hWnd);						//�Ի��򴰿ڵľ��

//����	:	���ô����Ի��Ƿ��Լ�������,�÷��������Ի�������Ҫ�Լ�����������
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetCustomDraw(
	HWND hWnd,						//�Ի����ھ��
	BOOL bCustomDraw);				//TRUEΪ�Լ������Ի�,FALSEΪ����Skin++�����Ի�

//����	:	���ô����Ƿ����RGN����
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetRgnEnable(
	HWND hWnd,						//���ھ��
	BOOL bRgnEnable);				//TrueΪ��������,FALSEΪ����������

//����	:	����ListHeader���ڵ�������Ϣ
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetListHeaderSortInfo(
	HWND hWnd,						//ListHeader�Ĵ��ھ��
	int nSortColumn,				//Ҫ��ListHeader������е�����
	BOOL bSortAscending = TRUE);	//�Ƿ�Ϊ����

//����	:	�����Ƿ��Լ����Ի��򱳾�,�÷���������Ҫ�Լ��Ա������д���������.
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetDialogEraseBkgnd(
	HWND hWnd,						//�Ի�����
	BOOL bErase);					//TRUE Ϊ�Լ�������,FALSE ΪSkin++��,
									//���û�е��ø÷���,Skin++�����Ի��򱳾�.

//����	:	���öԻ��򱳾��Ƿ�����ӿؼ�����
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetDialogBkClipRgn(
	HWND hWnd,						//�Ի�����
	BOOL bClip,						//TRUEΪ����Ҫ����,FALSEΪ��Ҫ��������
	BOOL bAllChild = TRUE);			//TRUEΪ�ô���������ӶԻ��򶼼���.

//����	:	���ð�ť����ʾ
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetButtonTooltip(
	HWND hWnd,						//��ť���
	LPCTSTR szTooltip);				//��ť��ʾ�ı�
	
//����	:	��ָ���Ĵ���ȥ��Ƥ��,���ұ�֤�����ٱ�����,��ʹʹ��SetSkinHwndҲ���ỻ��.
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetNoSkinHwnd(
	HWND hWnd,						//���ھ��
	BOOL bChildNoSkin = TRUE);		//�Ƿ�Ըô����е��Ӵ���ȥ��Ƥ��

//����	:	��ָ���Ĵ��ڽ��л���
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetSkinHwnd(
	HWND hWnd,						//���ھ��
	LPCTSTR szClassName = NULL,		//Ҫ���໯��Skin���� WC_DIALOGBOX/WC_CONTROLBAR��
	BOOL bChildSkin = TRUE);		//�Ƿ�Ըô����е��Ӵ��ڼ���Ƥ��

//����	:	��ָ���Ĵ�����ʱȥ��Ƥ��,����ͨ��SetSkinHwnd�����ٴλ���
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppRemoveSkinHwnd(
	HWND hWnd);						//���ھ��

//����	:	���Skin++��ϵͳMetrics,����ͬWindows API����GetSystemMetrics,
//			ȡ��Skin++ʵ�ʵı������߶�,�˵����߶ȵ���ֵ
//����ֵ:	�ɹ�����Skin++��Metrics,ʧ�ܷ���-1
int __stdcall sppGetSystemMetrics(
	HWND hWnd,						//������
	int nIndex);					//����ͬGetSystemMetrics

//����	:	���̻߳�����
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppInitializeSkin();

//����	:	�˳������,����������
//����ֵ:	�ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppExitSkin();

//����	:	���ô����Ƿ�ˢ��
//����ֵ		: �ɹ�����True,ʧ�ܷ���False
//��ע			: �����ݴ����������ʱ�ᵼ��ˢ�±Ƚ���,
//���ʱ�����������ӿ�����ˢ��,���ݲ�����ɺ������¿���ˢ��
BOOL __stdcall sppSetRedraw(
	HWND hWnd,						//���ھ��
	BOOL bRedraw);					//TrueΪˢ��,FalseΪ����ˢ��

//����		: ��֤Skin++�汾�Ƿ��뵱ǰ����Ŀ���������ƥ��
//����ֵ	: �ɹ�����True,ʧ�ܷ���False
BOOL _stdcall sppValidateDevTools();

//[For PowerBuilder]
//����			: ����Tab�ؼ��ı�ǩҳ��λ��
//����ֵ		: �ɹ�����True,ʧ�ܷ���False
enum TABPOSITION
{
	TABPOS_UNKNOWN				=   0x00,
	TABPOS_TABSONTOP			=	0x01,
	TABPOS_TABSONTOPANDBOTTOM	=	0x02,
	TABPOS_TABSONLEFT			=	0x03,
	TABPOS_TABSONLEFTANDRIGHT	=	0x04,
	TABPOS_TABSONBOTTOM			=	0x05,
	TABPOS_TABSONBOTTOMANDTOP	=	0x06,
	TABPOS_TABSONRIGHT			=	0x07,
	TABPOS_TABSONRIGHTANDLEFT	=	0x08
};

BOOL __stdcall sppSetTabCtrlPosition(
	HWND	hWnd,						//Tab�ؼ��Ĵ��ھ��
	TABPOSITION enumPosition,			//TABPOSITION��ö��ֵ
	BOOL	bPictureOnRight = FALSE);	//ͼ���Ƿ���ʾ���ı����Ҳ�

//[For PowerBuilder]
//����			: ����Tab�ؼ��ı�ǩҳ��ʾ������ֵ
//����ֵ		: �ɹ�����True,ʧ�ܷ���False
BOOL __stdcall sppSetTabCtrlItemsStatus(
	HWND    hWnd,						//Tab�ؼ��Ĵ��ھ��
	int*	pIndexs,					//��ʾ��ǩҳ��Index����
	int		nCount						//��ʾ��ǩҳ���ܸ���
	);

//����	:	�Ƕ�SetNoSkinHwnd�Ľ�һ������,���Խ��ʹ��sppSetNoSkinHwnd�����Debug��Ķ��Դ�
#define SETNOSKINHWND(x) {\
    HWND w=(x).UnsubclassWindow();\
	sppSetNoSkinHwnd(w);\
	(x).SubclassWindow(w);\
}

#ifdef _SKINPP_DYNAMICCALLDLL

class CSkinPPDynamicCallDll
{
public:
	typedef BOOL	(__stdcall *SPPLOADSKIN)(LPCTSTR strSkinFile,BOOL bWithoutHook);
	typedef BOOL	(__stdcall *SPPSETHOOKMESSAGE)(BOOL bHook,DWORD dwHookStyle);
	typedef BOOL	(__stdcall *SPPLOADSKINFROMRES)(HINSTANCE hInstance,LPCTSTR szResourceName,LPCTSTR szResourceType,LPCTSTR szSkinName,BOOL bWithoutHook);
	typedef DWORD	(__stdcall *SPPMODIFYHOOKAPISTYLE)(DWORD dwRemove,DWORD dwAdd);				
	typedef DWORD	(__stdcall *SPPGETHOOKAPISTYLE)();
	typedef BOOL	(__stdcall *SPPSETHOOKAPI)(BOOL bHookAPI);
	typedef BOOL	(__stdcall *SPPGETHOOKAPI)();
	typedef BOOL	(__stdcall *SPPREMOVESKIN)();
	typedef BOOL	(__stdcall *SPPSETCUSTSYSBTNVISIBLE)(UINT uResID,BOOL bVisible);
	typedef BOOL	(__stdcall *SPPSETCUSTSYSBTNSTATUS)(UINT uResID,BOOL bChecked);
	typedef BOOL	(__stdcall *SPPCOLORIZE)(COLORREF clr,double blend,BOOL bShowDialog);
	typedef BOOL	(__stdcall *SPPSELECTSKIN)(); 
	typedef BOOL	(__stdcall *SPPDRAWSKINBITMAP)(HDC hdc,RECT rect,HBITMAP hBitmap,BOOL bStretchAll,
		int nTopHeight,int nBottomHeight,int nLeftWidth,
		int nRightWidth,COLORREF clrTrans,BOOL bTrans);
	typedef BOOL	(__stdcall *SPPDRAWSKINIMAGE)(HDC hdc,RECT rect,LPCTSTR lpszObjName,BOOL bStretchAll,
		int nLeftWidth,int nRightWidth,int nTopHeight,
		int nBottomHeight,COLORREF clrTrans,BOOL bTrans);
	typedef COLORREF	(__stdcall *SPPGETSKINSYSCOLOR)(int nColorIndex);	
	typedef COLORREF	(__stdcall *SPPGETDEFAULTSYSCOLOR)();
	typedef BOOL		(__stdcall *SPPSETWINDOWRESID)(HWND hWnd,int nResID);	
	typedef int			(__stdcall *SPPGETWINDOWRESID)(HWND hWnd);	
	typedef BOOL		(__stdcall *SPPSETFREEWINDOWID)(HWND hWnd,int nResID);
	typedef int			(__stdcall *SPPGETFREEWINDOWID)(HWND hWnd);	
	typedef BOOL		(__stdcall *SPPSETCUSTOMDRAW)(HWND hWnd,BOOL bCustomDraw);
	typedef BOOL		(__stdcall *SPPSETRGNENABLE)(HWND hWnd,BOOL bRgnEnable);
	typedef BOOL		(__stdcall *SPPSETLISTHEADERSORTINFO)(HWND hWnd,int nSortColumn,BOOL bSortAscending);
	typedef BOOL		(__stdcall *SPPSETDIALOGERASEBKGND)(HWND hWnd,BOOL bErase);
	typedef BOOL		(__stdcall *SPPSETDIALOGBKCLIPRGN)(HWND hWnd,BOOL bClip,BOOL bAllChild);
	typedef BOOL		(__stdcall *SPPSETBUTTONTOOLTIP)(HWND hWnd,LPCTSTR szTooltip);	
	typedef BOOL		(__stdcall *SPPSETNOSKINHWND)(HWND hWnd,BOOL bChildNoSkin);
	typedef BOOL		(__stdcall *SPPSETSKINHWND)(HWND hWnd,LPCTSTR szClassName,BOOL bChildSkin);
	typedef BOOL		(__stdcall *SPPREMOVESKINHWND)(HWND hWnd);
	typedef int			(__stdcall *SPPGETSYSTEMMETRICS)(HWND hWnd,int nIndex);
	typedef BOOL		(__stdcall *SPPINITIALIZESKIN)();
	typedef BOOL		(__stdcall *SPPEXITSKIN)();
	typedef BOOL		(__stdcall *SPPSETREDRAW)(HWND hWnd,BOOL bRedraw);
	typedef BOOL		(__stdcall *SPPVALIDATEDEVTOOLS)();
		
	SPPLOADSKIN					LoadSkin;
	SPPSETHOOKMESSAGE			SetHookMessage;
	SPPLOADSKINFROMRES			LoadSkinFromRes;
	SPPMODIFYHOOKAPISTYLE		ModifyHookAPIStyle;
	SPPGETHOOKAPISTYLE			GetHookAPIStyle;
	SPPSETHOOKAPI				SetHookAPI;
	SPPGETHOOKAPI				GetHookAPI;
	SPPREMOVESKIN				RemoveSkin;
	SPPSETCUSTSYSBTNVISIBLE		SetCustSysBtnVisible;
	SPPSETCUSTSYSBTNSTATUS		SetCustSysBtnStatus;
	SPPCOLORIZE					Colorize;
	SPPSELECTSKIN				SelectSkin;
	SPPDRAWSKINBITMAP			DrawSkinBitmap;
	SPPDRAWSKINIMAGE			DrawSkinImage;
	SPPGETSKINSYSCOLOR			GetSkinSysColor;
	SPPGETDEFAULTSYSCOLOR		GetDefaultSysColor;
	SPPSETWINDOWRESID			SetWindowResID;
	SPPGETWINDOWRESID			GetWindowResID;
	SPPSETFREEWINDOWID			SetFreeWindowID;
	SPPGETFREEWINDOWID			GetFreeWindowID;
	SPPSETCUSTOMDRAW			SetCustomDraw;
	SPPSETRGNENABLE				SetRgnEnable;
	SPPSETLISTHEADERSORTINFO	SetListHeaderSortInfo;
	SPPSETDIALOGERASEBKGND		SetDialogEraseBkgnd;
	SPPSETDIALOGBKCLIPRGN		SetDialogBkClipRgn;
	SPPSETBUTTONTOOLTIP			SetButtonTooltip;
	SPPSETNOSKINHWND			SetNoSkinHwnd;
	SPPSETSKINHWND				SetSkinHwnd;
	SPPREMOVESKINHWND			RemoveSkinHwnd;
	SPPGETSYSTEMMETRICS			GetSystemMetrics;
	SPPINITIALIZESKIN			InitializeSkin;
	SPPEXITSKIN					ExitSkin;
	SPPSETREDRAW				SetRedraw;
	SPPVALIDATEDEVTOOLS			ValidateDevTools;

	CSkinPPDynamicCallDll() 
	{

	}

	CSkinPPDynamicCallDll(LPCTSTR strDllPathName)
	{
		Initialize(strDllPathName);
	}

	virtual ~CSkinPPDynamicCallDll()
	{
		Exit();
	}


	BOOL Initialize(LPCTSTR strDllPathName) 
	{
		if (m_hModuleDll)
			return FALSE;
		
		if (strDllPathName == NULL/* || _tcslen(strDllPathName) == 0*/)
		{
#ifdef _UNICODE
			m_hModuleDll = ::LoadLibraryW(L"SkinPlusPlusU.dll");
#else
			m_hModuleDll = ::LoadLibraryA("SkinPlusPlus.dll");
#endif
		}
		else
			m_hModuleDll = ::LoadLibrary(strDllPathName);
		
		if (!m_hModuleDll)
			return FALSE;
		
		LoadSkin = (SPPLOADSKIN)GetProcAddress(m_hModuleDll,"sppLoadSkin");
		
		SetHookMessage = (SPPSETHOOKMESSAGE)GetProcAddress(m_hModuleDll,"sppSetHookMessage");
		
		LoadSkinFromRes = (SPPLOADSKINFROMRES)GetProcAddress(m_hModuleDll,"sppLoadSkinFromRes");
		
		ModifyHookAPIStyle = (SPPMODIFYHOOKAPISTYLE)GetProcAddress(m_hModuleDll,"sppModifyHookAPIStyle");
		
		GetHookAPIStyle = (SPPGETHOOKAPISTYLE)GetProcAddress(m_hModuleDll,"sppGetHookAPIStyle");
		
		SetHookAPI = (SPPSETHOOKAPI)GetProcAddress(m_hModuleDll,"sppSetHookAPI");
		
		GetHookAPI = (SPPGETHOOKAPI)GetProcAddress(m_hModuleDll,"sppGetHookAPI");
		
		RemoveSkin = (SPPREMOVESKIN)GetProcAddress(m_hModuleDll,"sppRemoveSkin");
		
		SetCustSysBtnVisible = (SPPSETCUSTSYSBTNVISIBLE)GetProcAddress(m_hModuleDll,"sppSetCustSysBtnVisible");
		
		SetCustSysBtnStatus = (SPPSETCUSTSYSBTNSTATUS)GetProcAddress(m_hModuleDll,"sppSetCustSysBtnStatus");
				
		Colorize = (SPPCOLORIZE)GetProcAddress(m_hModuleDll,"sppColorize");
		
		SelectSkin = (SPPSELECTSKIN)GetProcAddress(m_hModuleDll,"sppSelectSkin");
		
		DrawSkinBitmap = (SPPDRAWSKINBITMAP)GetProcAddress(m_hModuleDll,"sppDrawSkinBitmap");
		
		DrawSkinImage = (SPPDRAWSKINIMAGE)GetProcAddress(m_hModuleDll,"sppDrawSkinImage");
		
		GetSkinSysColor = (SPPGETSKINSYSCOLOR)GetProcAddress(m_hModuleDll,"sppGetSkinSysColor");
		
		GetDefaultSysColor = (SPPGETDEFAULTSYSCOLOR)GetProcAddress(m_hModuleDll,"sppGetDefaultSysColor");
		
		SetWindowResID = (SPPSETWINDOWRESID)GetProcAddress(m_hModuleDll,"sppSetWindowResID");
		
		GetWindowResID = (SPPGETWINDOWRESID)GetProcAddress(m_hModuleDll,"sppGetWindowResID");
		
		SetFreeWindowID = (SPPSETFREEWINDOWID)GetProcAddress(m_hModuleDll,"sppSetFreeWindowID");
		
		GetFreeWindowID = (SPPGETFREEWINDOWID)GetProcAddress(m_hModuleDll,"sppGetFreeWindowID");
		
		SetCustomDraw = (SPPSETCUSTOMDRAW)GetProcAddress(m_hModuleDll,"sppSetCustomDraw");
		
		SetRgnEnable = (SPPSETRGNENABLE)GetProcAddress(m_hModuleDll,"sppSetRgnEnable");
		
		SetListHeaderSortInfo = (SPPSETLISTHEADERSORTINFO)GetProcAddress(m_hModuleDll,"sppSetListHeaderSortInfo");
		
		SetDialogEraseBkgnd = (SPPSETDIALOGERASEBKGND)GetProcAddress(m_hModuleDll,"sppSetDialogEraseBkgnd");
		
		SetDialogBkClipRgn = (SPPSETDIALOGBKCLIPRGN)GetProcAddress(m_hModuleDll,"sppSetDialogBkClipRgn");
		
		SetButtonTooltip = (SPPSETBUTTONTOOLTIP)GetProcAddress(m_hModuleDll,"sppSetButtonTooltip");
		
		SetNoSkinHwnd = (SPPSETNOSKINHWND)GetProcAddress(m_hModuleDll,"sppSetNoSkinHwnd");
		
		SetSkinHwnd = (SPPSETSKINHWND)GetProcAddress(m_hModuleDll,"sppSetSkinHwnd");
		
		RemoveSkinHwnd = (SPPREMOVESKINHWND)GetProcAddress(m_hModuleDll,"sppRemoveSkinHwnd");
		
		GetSystemMetrics = (SPPGETSYSTEMMETRICS)GetProcAddress(m_hModuleDll,"sppGetSystemMetrics");
		
		InitializeSkin = (SPPINITIALIZESKIN)GetProcAddress(m_hModuleDll,"sppInitializeSkin");
		
		ExitSkin = (SPPEXITSKIN)GetProcAddress(m_hModuleDll,"sppExitSkin");				
		
		SetRedraw = (SPPSETREDRAW)GetProcAddress(m_hModuleDll,"sppSetRedraw");

		ValidateDevTools = (SPPVALIDATEDEVTOOLS)GetProcAddress(m_hModuleDll,"sppValidateDevTools");				

		return TRUE;
	}

	BOOL Exit()
	{
		if (!m_hModuleDll)
			return FALSE;
	
		::FreeLibrary(m_hModuleDll);
		
		m_hModuleDll				= NULL;
		
		LoadSkin					= NULL;
		SetHookMessage				= NULL;
		LoadSkinFromRes				= NULL;
		ModifyHookAPIStyle			= NULL;
		GetHookAPIStyle				= NULL;
		SetHookAPI					= NULL;
		GetHookAPI					= NULL;
		RemoveSkin					= NULL;
		SetCustSysBtnVisible		= NULL;
		SetCustSysBtnStatus			= NULL;
		Colorize					= NULL;
		SelectSkin					= NULL;
		DrawSkinBitmap				= NULL;
		DrawSkinImage				= NULL;
		GetSkinSysColor				= NULL;
		GetDefaultSysColor			= NULL;
		SetWindowResID				= NULL;
		GetWindowResID				= NULL;
		SetFreeWindowID				= NULL;
		GetFreeWindowID				= NULL;
		SetCustomDraw				= NULL;
		SetRgnEnable				= NULL;
		SetListHeaderSortInfo		= NULL;
		SetDialogEraseBkgnd			= NULL;
		SetDialogBkClipRgn			= NULL;
		SetButtonTooltip			= NULL;
		SetNoSkinHwnd				= NULL;
		SetSkinHwnd					= NULL;
		RemoveSkinHwnd				= NULL;
		GetSystemMetrics			= NULL;
		InitializeSkin				= NULL;
		ExitSkin					= NULL;
		SetRedraw					= NULL;
		ValidateDevTools			= NULL;
		
		return TRUE;
	}

private:
	HMODULE m_hModuleDll;

};
 
#endif //_SKINPP_DYNAMICCALLDLL

#ifdef _SKINPP_STATIC //static 
	#ifdef _AFXDLL 
	//---------------------------------------------
	// MFC shared DLL, Skin++ static library
	//---------------------------------------------
		#ifdef _DEBUG
			#ifdef _UNICODE
				#pragma comment(lib, "SkinPPStaticDDU.lib")
				#pragma message("Automatically linking with SkinPPStaticDDU.lib")
			#else
				#pragma comment(lib, "SkinPPStaticDDA.lib")
				#pragma message("Automatically linking with SkinPPStaticDDA.lib")
			#endif //UNICODE
		#else //Release
			#ifdef _UNICODE
				#pragma comment(lib, "SkinPPStaticDRU.lib")
				#pragma message("Automatically linking with SkinPPStaticDRU.lib")
			#else
				#pragma comment(lib, "SkinPPStaticDRA.lib")
				#pragma message("Automatically linking with SkinPPStaticDRA.lib")
			#endif
		#endif //_DEBUG
	#else //MFC static library
	//--------------------------------------------------
	// MFC static library, Skin++ static library
	//--------------------------------------------------
		#ifdef _DEBUG
			#ifdef _UNICODE
				#pragma comment(lib, "SkinPPStaticSDU.lib")
				#pragma message("Automatically linking with SkinPPStaticSDU.lib")
			#else
				#pragma comment(lib, "SkinPPStaticSDA.lib")
				#pragma message("Automatically linking with SkinPPStaticSDA.lib")
			#endif
		#else
			#ifdef _UNICODE
				#pragma comment(lib, "SkinPPStaticSRU.lib")
				#pragma message("Automatically linking with SkinPPStaticSRU.lib")
			#else
				#pragma comment(lib, "SkinPPStaticSRA.lib")
				#pragma message("Automatically linking with SkinPPStaticSRA.lib")
			#endif
		#endif //_DEBUG
	#endif //_AFXDLL
#elif !defined _SKINPP_DYNAMICCALLDLL //DLL 
	//------------------------------------------
	// MFC shared DLL, Skin++ shared DLL
	//------------------------------------------
#if 0
	#ifdef _UNICODE
		#pragma comment(lib,"SkinPlusPlusU.lib")  
		#pragma message("Automatically linking with SkinPlusPlusU.dll")
	#else
		#pragma comment(lib,"SkinPlusPlus.lib")
		#pragma message("Automatically linking with SkinPlusPlus.dll") 
	#endif
#endif
#endif //_SKINPP_STATIC

#ifndef WM_TOOLBARPAINTPRE
	#define WM_TOOLBARPAINTPRE		WM_USER + 0x0322
#endif
#ifndef WM_TOOLBARPAINTEND
	#define WM_TOOLBARPAINTEND		WM_USER + 0x0323
#endif
#ifndef SM_LBUTTONUP
	#define SM_LBUTTONUP			WM_USER + 0x0324
#endif
#ifndef SM_TITLELBUTTONUP
	#define	SM_TITLELBUTTONUP		WM_USER + 0x0325
#endif
#ifndef SM_TITLELBUTTONDOWN
	#define	SM_TITLELBUTTONDOWN		WM_USER + 0x0326
#endif

//HookMsg
#define HS_MENU		0x80000000L
#define HS_WINDOW	0x40000000L
#define HS_NONE		0x00000000L
#define HS_ALL		HS_MENU | HS_WINDOW

//HookAPI
#define HOOKAPI_SCROLLBAR		0x80000000L
#define HOOKAPI_GETSYSCOLOR		0x40000000L
#define HOOKAPI_FILLRECT		0x20000000L
#define HOOKAPI_OTHER			0x10000000L
#define HOOKAPI_NONE			0x00000000L
#define HOOKAPI_ALL				HOOKAPI_SCROLLBAR | HOOKAPI_GETSYSCOLOR | HOOKAPI_FILLRECT | HOOKAPI_OTHER

#pragma pack(pop)

#endif // __SKINPPINC_H
