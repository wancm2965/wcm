#ifndef __INNER_INTERFACE_H__
#define __INNER_INTERFACE_H__
#include <IPlugin\IU7T.Comm\IConfigDlgBase.h>///define IConfigDlgBase interface

//// ��������
//IConfigDlgBase* CreateConfigUI(HWND hParent);
//// ��Ļ����
//IConfigDlgBase* CreateConfigScreen(HWND hParent);
//// ��Ƶ��������
//IConfigDlgBase* CreateConfigVideoInput(HWND hParent);
//// ��Ƶ����
//IConfigDlgBase* CreateConfigAudioSet(HWND hParent);
//// ý������
//IConfigDlgBase* CreateConfigMediaSet(HWND hParent);


//// ����ܹ��ܽӿ�
//class IMainFrameFun
//{
//public:
//	IMainFrameFun(void){};
//	~IMainFrameFun(void){};
//
//public:
//	virtual HWND GetMainFrameHWND(void) = 0;
//	virtual void ExitMeeting(void) = 0;
//
//	virtual void SetActiveScreen(int nAvctiveTM) = 0;
//
//	//virtual void AddWindow(CString strClassName, AVCONPlugin::IPlugin_U7_ComUI_Observer* pWnd) = 0;
//	//virtual void RemoveWindow(AVCONPlugin::IPlugin_U7_ComUI_Observer* pWnd) = 0;
//
//	virtual TFontInfo* GetFontInfo(void) = 0;
//	virtual TUISetting GetUISetting() = 0;
//
//	//��������ӿ�
//	virtual void SetUISize(float fScale) = 0;
//	// �ṩ����Ҫ������С�Ĵ��ڵ���
//	virtual void SetUISize(HWND hWnd, CPaintManagerUI* pPaintManager, float fScale) = 0;
//	virtual void SetButtonSize(float fScale) = 0;
//	virtual void SetFontSize(int nSize) = 0;
//	virtual void SetOpacity(int nOpacity) = 0;
//	virtual void SetWindowVisible(bool IsVisble) = 0;
//};

//IMainFrameFun* GetIMainFrameFun(void);

#endif
