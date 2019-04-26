#ifndef __INNER_INTERFACE_H__
#define __INNER_INTERFACE_H__
#include <IPlugin\IU7T.Comm\IConfigDlgBase.h>///define IConfigDlgBase interface

//// 界面设置
//IConfigDlgBase* CreateConfigUI(HWND hParent);
//// 屏幕设置
//IConfigDlgBase* CreateConfigScreen(HWND hParent);
//// 视频输入设置
//IConfigDlgBase* CreateConfigVideoInput(HWND hParent);
//// 音频设置
//IConfigDlgBase* CreateConfigAudioSet(HWND hParent);
//// 媒体设置
//IConfigDlgBase* CreateConfigMediaSet(HWND hParent);


//// 主框架功能接口
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
//	//界面调整接口
//	virtual void SetUISize(float fScale) = 0;
//	// 提供给需要调整大小的窗口调用
//	virtual void SetUISize(HWND hWnd, CPaintManagerUI* pPaintManager, float fScale) = 0;
//	virtual void SetButtonSize(float fScale) = 0;
//	virtual void SetFontSize(int nSize) = 0;
//	virtual void SetOpacity(int nOpacity) = 0;
//	virtual void SetWindowVisible(bool IsVisble) = 0;
//};

//IMainFrameFun* GetIMainFrameFun(void);

#endif
