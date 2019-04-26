#ifndef _SKINPPWTL_H_
#define _SKINPPWTL_H_

#ifdef _SKINPP_STATIC
	#define SKINPPWTL_API
#else
	#ifdef SKINPPWTL_EXPORTS
		#define SKINPPWTL_API __declspec(dllexport)
	#else
		#define SKINPPWTL_API __declspec(dllimport)
	#endif
#endif

#define WM_TOOLBARPAINTPRE	(WM_USER + 802)
#define WM_TOOLBARPAINTEND	(WM_USER + 803)
#define SM_LBUTTONUP		(WM_USER + 804)
#define	SM_TITLELBUTTONUP	(WM_USER + 805)
#define	SM_TITLELBUTTONDOWN (WM_USER + 806)

//��ť״̬
enum BUTTONSTATE 
{
	NORMAL	= 0,
	PRESS	= 1,
	DISABLE	= 2,
	HOT		= 3,
	FOCUS   = 4,
	LAST	= 5
};

#define HS_MENU           0x40000000L
#define HS_WINDOW         0x20000000L
#define HS_ALL            HS_MENU | HS_WINDOW

//��������
enum DRAWTYPE
{
	BUTTON = 0,
	SCROLLARROWUP = 1,
	SCROLLARROWDOWN = 2,
	SPLITTERBARHORZ = 3,
	SPLITTERBARVERT = 4,
	SPLITTERBARBORDER = 5,
	LISTHEAD = 6
};

//���Ƥ����Դ������
enum SKINOBJTYPE
{
	DIALOGTYPE = 0,
	BUTTONTYPE = 1,
	CHECKBOXTYPE = 2,
	RADIOBOXTYPE = 3,
	STATICTYPE = 4,
	TRACKBARTYPE = 5,
};

struct ListBoxItem
{
	HIMAGELIST	hImageList;
	int			nImageIndex;

	ListBoxItem()
	{
		hImageList = NULL;
		nImageIndex = -1;
	}
};

#define REST_BITMAP      0x0001 //.bmp
#define REST_ICON        0x0002 //.ico
#define REST_CURSOR      0x0003 //.cur
#define REST_ANIMATE	 0x0004 //.ani

typedef struct _ResourceInfo
{
	HGDIOBJ hGdiObj;//[OUT]
	DWORD   dwType; //[OUT]
	int		nWidth; //[OUT]
	int		nHeight;//[OUT]

	TCHAR	szResImageName[_MAX_FNAME];//[IN]
	BOOL	bHorzSplit;//[IN]
	int		nLength;//[IN]
	int		nCount; //[IN]
	int		nIndex; //[IN]
	
	_ResourceInfo()
	{
		hGdiObj = NULL;
		dwType = REST_BITMAP;
		nWidth = 0;
		nHeight = 0;

		_tcscpy_s(szResImageName,256,_T(""));
		bHorzSplit = TRUE;
		nLength = -1;
		nCount = -1;
		nIndex = -1;
	}
	
}ResInfo,* PRESINFO;

//////////////////////////////////////////////////////////////////////////
SKINPPWTL_API BOOL WINAPI skinppInitializeSkin();
//����Ƥ��
//SkinFile		:Ƥ��·��,ע�������*.ssk,Ҳ������Ƥ��Ŀ¼�е�INI�ļ�.
//bFromIni		:�ò���ָ��Ƥ���ļ��Ǵ�*.ssk��ȡ,���Ǵ�INI�ļ���ȡ.
SKINPPWTL_API BOOL WINAPI skinppLoadSkin(TCHAR* szSkinFile,BOOL bFromIni = FALSE);

SKINPPWTL_API BOOL WINAPI skinppLoadSkinWithoutHook(TCHAR* szSkinFile,BOOL bFromIni = FALSE);

SKINPPWTL_API BOOL WINAPI skinppSetHook(BOOL bHook,DWORD dwHookStyle);

SKINPPWTL_API BOOL WINAPI skinppLoadSkinFromRes(HINSTANCE hInstance,LPCTSTR szResourceName,
										 LPCTSTR szResourceType,TCHAR* szSkinFileName);

//�Ƴ�Ƥ��
SKINPPWTL_API BOOL  WINAPI skinppRemoveSkin();

//�˳�����⣬����������
SKINPPWTL_API BOOL  WINAPI skinppExitSkin();

//���ñ������Զ��尴ť�Ƿ���ʾ
//uID			: ��ť��ResourceID
//bVisible		: �Ƿ�ɼ�
SKINPPWTL_API BOOL	WINAPI skinppSetCustSysBtnVisible(UINT uID,BOOL bVisible);

//����ListBox�ؼ����Ի���Ϣ
//hWnd			: ListBox�ؼ��ľ��
//nIndex		: Item�������
//pListBoxItem	: Item���Ի��Ľṹ��Ϣ
SKINPPWTL_API void WINAPI skinppSetListBoxItemDrawInfo(HWND hWnd,int nIndex,struct ListBoxItem* pListBoxItem);

//��û������ϵͳ��ɫ
//nColorIndex	: Ҫ��ȡ����ɫ����
SKINPPWTL_API COLORREF WINAPI skinppGetSkinSysColor(int nColorIndex);

//���WindowsϵͳĬ�ϵ���ɫ
//nColorIndex	: Ҫ��ȡ����ɫ����
SKINPPWTL_API COLORREF WINAPI skinppGetDefaultSysColor(int nColorIndex);

//hWnd			: �Ի��򴰿ڵľ��
//nResID		: �Ի�����ԴID
SKINPPWTL_API BOOL WINAPI skinppSetWindowResID(HWND hWnd,int nResID);//[������]

//���ò��������ʹ������ԴID
//hWnd     :   �Ի��򴰿ڵľ��
//nResID   :   �Ի�����ԴID��
//���IDֵҪ��Skinbuilder�е�SkinFreeDlg�����ResourceID��ֵ����һ��
SKINPPWTL_API BOOL WINAPI skinppSetFreeDlgID(HWND hWnd,int nResID);

//��C#���������ô������ԴIDֵ,ͬSkinppSetWindowResID
//hWnd			:  �Ի��򴰿ڵľ��
//nResID		:  �Ի�����ԴID
SKINPPWTL_API BOOL WINAPI skinppSetSkinResID(HWND hWnd,int nResID);

//����ListHeader���ڵ�������Ϣ
//hWnd			: ListHeader�Ĵ��ھ��
//nSortColumn	: Ҫ��ListHeader������е�����
//bSortAscending: �Ƿ�Ϊ����
SKINPPWTL_API void WINAPI skinppSetListHeaderSortInfo(HWND hWnd,int nSortColumn,BOOL bSortAscending = TRUE);

//�ڸ�����HDC��,ָ����Ӧ�Ļ������ͺ�״̬,����Ӧ�ľ��������н��л���.
//hdc			:Ŀ��DC
//rect			:��������
//eDrawType		:�������ͣ�Ŀǰ֧��SPLITTERBARHORZ,SPLITTERBARVERT,SPLITTERBARBORDER
//nState		:ѡ�����״̬
SKINPPWTL_API void  WINAPI skinppDrawSkinObject(HDC hdc,RECT rect,DRAWTYPE eDrawType,int nState);

//�ڸ�����HDC��,ָ����Ӧ�Ļ������ͺ�״̬,����Ӧ�ľ��������н��л���.
//hdc			:Ŀ��DC
//rect			:��������
//lpszObjName   :��������
//bStretchAll   :�Ƿ�ȫ������
//nLeftWidth	:���
//nRightWidth	:�ҿ�
//nTopHeight	:����
//nBottomHeight	:�׸�
//clrTrans		:͸��ɫ
//bTrans		:�Ƿ�͸��
SKINPPWTL_API void  WINAPI skinppDrawSkinImage(HDC hdc,RECT rect,LPCTSTR lpszObjName,BOOL bStretchAll,int nLeftWidth,int nRightWidth,int nTopHeight,int nBottomHeight,COLORREF clrTrans,BOOL bTrans);

//ͨ����ԴID,�����Ӧ���͵�Ƥ����Դλͼ���
//nSkinObjType	: Ƥ������,Ŀǰ֧��	DIALOGTYPE,BUTTONTYPE,CHECKBOXTYPE,RADIOBOXTYPE
//nResID		: ��ԴID
//nState		: ״̬,��BUTTONTYPE,CHECKBOXTYPE,RADIOBOXTYPE��Ч
SKINPPWTL_API HBITMAP WINAPI skinppGetResFromID(SKINOBJTYPE nSkinObjType,int nResID,int nState =0 );

//�����Ƿ��Լ����Ի��򱳾�,�÷���������Ҫ�Լ��Ա������д���������.
//hWnd			: �Ի���ľ��
//bErase		: TRUE Ϊ�Լ�������,FALSE ΪSkin++��,���û�е��ø÷���,Skin++�����Ի��򱳾�.
SKINPPWTL_API void WINAPI skinppSetDialogEraseBkgnd(HWND hWnd,BOOL bErase);

//���öԻ��򱳾��Ƿ�����ӿؼ�����
//hWnd			: �Ի�����
//bNoClip		: TRUEΪ����Ҫ����,FALSEΪ��Ҫ��������
//bAllChild		: TRUEΪ�ô���������ӶԻ��򶼼���.
SKINPPWTL_API void WINAPI skinppSetDialogBkClipRgn(HWND hWnd,BOOL bClip,BOOL bAllChild = TRUE);

//ͨ��Ƥ����Դ���ƻ��Ƥ����Դ��λͼ
//szName		: Ƥ����Դ����
//HBITMAP		: ������Դ�е�λͼ
SKINPPWTL_API HBITMAP WINAPI skinppGetBitmapRes(LPCTSTR szName);

//ͨ����Դ����ȡ��Դ���ڴ�ָ��
//szName		: ��Դ����
//nSize			: ��Դ��С
//pByte			: ����ֵ���ɹ����ط�NULL��ʧ�ܷ���NULL
SKINPPWTL_API BYTE*  WINAPI skinppGetSkinResource(LPCTSTR szName,int& nSize);

//ͨ��Ƥ����Դ�����ƻ��λͼ�������������ֵ
//szName		: Ƥ����Դ����
//nTopHeight	: ���ز�������Ķ���
//nBottomHeight : ���ز�������ĵ׸�
//nLeftWidth	: ���ز�����������
//nRightWidth	: ���ز���������ҿ�
SKINPPWTL_API BOOL WINAPI skinppGetBitmapResRect(LPCTSTR szName,int& nTopHeight,int& nBottomHeight,
												   int& nLeftWidth,int& nRightWidth);

//���ô����Ի��Ƿ��Լ�������,�÷��������Ի�������Ҫ�Լ�����������
//hWnd			: Ҫ�Ի��Ĵ��ھ��
//bCustomDraw	: TRUEΪ�Լ������Ի�,FALSEΪ����Skin++�����Ի�
SKINPPWTL_API void WINAPI skinppSetCustomDraw(HWND hWnd,BOOL bCustomDraw);

//���ò˵���Ƥ����ʶ
//hWnd			: ӵ�в˵��Ĵ��ھ��
//nSkinObjectID	: �˵�Ƥ���ı�ʶ
SKINPPWTL_API void WINAPI skinppSetMenuSkinObjectID(HWND hWnd,int nSkinObjectID);

//�����Ƿ���Ի��˵����л���
//bSkin			: TRUEΪ����
SKINPPWTL_API void WINAPI skinppSetSkinOwnerMenu(BOOL bSkin);

//�Բ˵����л�������
//hMenu   : �뻻���Ĳ˵����
//bNoSkin : �Ƿ񻻷�,TRUEΪ������,FALSEΪ����
SKINPPWTL_API void  WINAPI skinppSetDrawMenu(HMENU hMenu,BOOL bNoSkin);

//��ָ���Ĵ���ȥ��Ƥ��,���ұ�֤�����ٱ�����,��ʹʹ��SetSkinHwndҲ���ỻ��.
//hWnd			: ָ���Ĵ��ھ��
//bChildNoSkin	: �Ƿ�Ըô����е��Ӵ���ȥ��Ƥ��
SKINPPWTL_API void WINAPI skinppSetNoSkinHwnd(HWND hWnd,BOOL bChildNoSkin = TRUE);

//��ָ���Ĵ��ڽ��л���
//hWnd			: ָ���Ĵ��ھ��
//szClassName	: Ҫ���໯��Skin���� WC_DIALOGBOX/WC_CONTROLBAR��
SKINPPWTL_API void WINAPI skinppSetSkinHwnd(HWND hWnd,LPCTSTR szClassName = NULL);

//��ָ���Ĵ�����ʱȥ��Ƥ��,����ͨ��SetSkinHwnd�����ٴλ���
SKINPPWTL_API void WINAPI skinppRemoveSkinHwnd(HWND hWnd);

//�Ƕ�SetNoSkinHwnd�Ľ�һ������,���Խ��ʹ��SetNoSkinHwnd�����Debug��Ķ��Դ�
#define SETNOSKINHWND(x) {\
    HWND w=(x).UnsubclassWindow();\
	skinppSetNoSkinHwnd(w);\
	(x).SubclassWindow(w);\
} 

//�Ƕ�RemoveSkinHwnd�Ľ�һ������,���Խ��ʹ��RemoveSkinHwnd�����Debug��Ķ��Դ�
#define REMOVESKINHWND(x){\
    HWND w=(x).UnsubclassWindow();\
	skinppRemoveSkinHwnd(w);\
	(x).SubclassWindow(w);\
}

SKINPPWTL_API HGDIOBJ WINAPI skinppGetResFromID(PRESINFO pResInfo);

SKINPPWTL_API void WINAPI skinppSetRgnEnable(HWND hWnd,BOOL bRgnEnable);

//���Skin++��ϵͳMetrics,����ͬWindows API����GetSystemMetrics,ȡ��Skin++ʵ�ʵ�
//�������߶�,�˵����߶ȵ���ֵ
//hWnd      :   ����ľ�� 
//nIndex    :   ����ͬGetSystemMetrics
SKINPPWTL_API int  WINAPI skinppGetSystemMetrics(HWND hWnd,int nIndex);

//ָ���Զ���ϵͳ��ť��Check״̬
//uResID	: �Զ���ϵͳ��ť����ԴID��ֵ
//bChecked  : �Ƿ���״̬
SKINPPWTL_API BOOL WINAPI skinppSetCustSysBtnStatus(UINT uResID, BOOL bChecked = TRUE);

//���ô����Ƿ��Ի�
//hWnd : ָ���Ĵ���
//bOwnerDraw : �����Ƿ����Ի沿�ֵı�־
SKINPPWTL_API void WINAPI skinppSetOwnerDrawFlag( HWND hWnd, BOOL bOwnerDraw = TRUE);

#endif //_SKINPPWTL_H_

