// UCC7-T主界面插件通用接口
// 
#pragma once
#include "../../ISystem/IPlugin.h"
 
namespace AVCONPlugin
{
	typedef	struct _U7TBAR_BTN_INFO		// 按钮所用到的所有背景图
	{
		std::string	strBtnName;			// 按钮的名字（与其代表的插件名字一致）
		std::string strBtnText;			// 插件的名称
		std::string	strBtnTooltip;		// 插件名称提示
		std::string strNoSelNormal;		// 按钮未选中状态下的普通状态
		std::string strNoSelHot;		// 按钮未选中状态下的悬停状态
		std::string strNoSelPush;		// 按钮未选中状态下的按下状态
		std::string strNoSelDisable;	// 按钮未选中状态下的禁用状态
		std::string strSelNormal;		// 按钮选中状态下的普通状态
		std::string strSelHot;			// 按钮选中状态下的悬停状态
		std::string strSelPush;			// 按钮选中状态下的按下状态
		std::string strSelDisable;		// 按钮选中状态下的禁用状态
	}U7TBAR_BTN_INFO;

	// UCC7-T主界面插件通用接口
	class IPlugin_U7T_Comm_Notify
	{
	public:
		virtual ~IPlugin_U7T_Comm_Notify(){};

	public:
		// 按钮状态变换回调
		// 参数：strBtnName 按钮名字（与插件名字一致）
		//		 bSel 按钮是否选中
		virtual void OnBtnStatus(std::string strBtnName, bool bSel) = 0;
	};

	// UCC7-T主界面插件通用接口
	class IPlugin_U7T_Comm : virtual public IPlugin
	{
	public:
		virtual void MainFrameHWnd(HWND hWnd) = 0;
		virtual const char* GetButtonImage(void) = 0;
		virtual const char* GetButtonName(void) = 0;
		virtual void OnU7TBtnClick(void) = 0;

		virtual void SetU7TCommNotify(IPlugin_U7T_Comm_Notify* pNotify) = 0;
		// 设置插件对应按钮的名字和背景图
		// 参数：structBtnBGImg 按钮名字及按钮背景 结构体
		virtual	void GetU7TBtnInfo(U7TBAR_BTN_INFO& sBtnInfo) = 0;

		virtual void NotifyShowWnd() = 0;
		virtual void NotifyHideWnd() = 0;
	};
}
