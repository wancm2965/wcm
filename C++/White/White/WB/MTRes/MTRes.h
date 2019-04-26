// MTRes.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include "ResGallery.h"

//������Դ��ק��Ŀ�괰��
extern "C" LIB_AVCON_RESOURCE_DECL bool RegisterReceivedWnd(const HWND &hWnd);
extern "C" LIB_AVCON_RESOURCE_DECL bool UnRegisterReceivedWnd(const HWND &hWnd);

//����������Դ����
extern "C" LIB_AVCON_RESOURCE_DECL HRESULT CreateGalleryWnd(DWORD version, HINSTANCE hInstance, HWND hParent,
	int x, int y, int nWidth, int nHeight, ResTypeCallbackFun pResCallback = NULL, BOOL bAuthorize = FALSE);

//�ƶ���Դ����
extern "C" LIB_AVCON_RESOURCE_DECL BOOL MoveGalleryWnd( HWND hWndInsertAfter,
	int x, int y, int nWidth, int nHeight, UINT uFlags);

//��ʾ������Դ����
extern "C" LIB_AVCON_RESOURCE_DECL int ShowGalleryWnd();
//���ش�����Դ����
extern "C" LIB_AVCON_RESOURCE_DECL int HideGalleryWnd();

//����Դ����ȴ����������ԴĿ¼
extern "C" LIB_AVCON_RESOURCE_DECL void SwitchSpecialDirectory(LPCTSTR strPath);

//������Դ���ھ��
extern "C" LIB_AVCON_RESOURCE_DECL void SetWBResHWnd(HWND hParent);

//������Դ�����Ƿ��Ѿ�����
extern "C" LIB_AVCON_RESOURCE_DECL BOOL IsGalleryWndCreated();

//���ش�����Դ���ڵ���ʾ״̬����ʾ������ʾ��
extern "C" LIB_AVCON_RESOURCE_DECL BOOL IsGalleryWndShow();

//������Դ���ڣ�WBResourceManagerʹ��
extern "C" LIB_AVCON_RESOURCE_DECL void UpdateResGallery();

//�ر���Դ����
extern "C" LIB_AVCON_RESOURCE_DECL void CloseResGallery();

//��ȡ��ǰ������Դ���������ڱ��������ļ���
extern "C" LIB_AVCON_RESOURCE_DECL unsigned int GetCurSelectResIndex();

//������Ȩ���õ���Ȩϵͳ
extern "C" LIB_AVCON_RESOURCE_DECL void SetMTAuthorizeRes(BOOL MT);

extern "C" LIB_AVCON_RESOURCE_DECL void SetAvconLock(BOOL bAvconLock);
extern "C" LIB_AVCON_RESOURCE_DECL void SetAvconIniPath(const LPCTSTR strAvconIniPath);
//extern "C" LIB_AVCON_RESOURCE_DECL void LoadResFromDirectoryWB(const LPCTSTR trAvconTempPath);

extern CWBResGallery *g_pResGallery;
extern BOOL g_bAvconLock;