#ifndef _INTERFACE_PLUGIN_U7_CHILD_WINDOW_H_
#define _INTERFACE_PLUGIN_U7_CHILD_WINDOW_H_
#include <list>
#include "IConfigDlgBase.h"


/*----------------------------------------------------------------------------------
									[功      能]
						插件中提供子窗体的供其它插件使用
----------------------------------------------------------------------------------*/

namespace AVCONPlugin
{

	///子窗口信息结构
	struct SConfigWndInfo{
		IDlgBase	*pDlg;		//窗口句柄
		std::string sControlID;		//子窗口标识名称
		std::string sControlText;	//子窗口显示名称
		HWND		hParentWnd;		//子窗口的父窗口句柄
		std::string sInternalID;	//保留ID
		unsigned int nInternalID;	//保留ID
	};


	/*----------------------------------------------------------------------------------
	向其它插件提供子窗体的插件接口[该插件提供的某个窗体将作为其他插件中某个窗体的子窗体]
	----------------------------------------------------------------------------------*/
	class IPlugin_U7_ConfigWnd
	{
	public:
		virtual ~IPlugin_U7_ConfigWnd(void){};
	public:
		/*------------------------获取子窗体-------------------------------
		参数一: 父窗口句柄
		参数二: U7状态（如会议内，会议外等）
		参数三:	所获取(或创建)的窗口结构信息[输出项]
		------------------------------------------------------------------*/
		virtual bool GetConfigWnd(const HWND hParent, const std::string& strName,
			const unsigned int nU7State, std::list<SConfigWndInfo> &pLstChildWnd) = 0;
		///移除子窗体(参数为父窗口句柄，成功返回true，否则返回false)
		virtual bool RemoveConfigWnd(const HWND hParentWnd) = 0;
	};
	/////针对提供设置项的插件子窗体
	//class IPlugin_U7_ChildWnd : public IPlugin_U7_ChildWnd
	//{
	//public:
	//	///应用更改
	//	virtual void OnApply(const HWND hParentWnd) = 0;
	//	///回滚到上次应用更改
	//	virtual void OnCancel(const HWND hParentWnd) = 0;
	//};
	
}//end namespace
#endif