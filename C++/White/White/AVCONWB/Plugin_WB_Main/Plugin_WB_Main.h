// Plugin_WB_Main.h : Plugin_WB_Main DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "IPlugin\WB_Main\IPlugin_WB_Main.h"



// CPlugin_WB_MainApp
// 有关此类实现的信息，请参阅 Plugin_WB_Main.cpp
//
class CPlugin_WB_MainApp : public CWinApp
{
public:
	CPlugin_WB_MainApp();

	// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

namespace AVCONPlugin
{
	class CPlugin_WB_Main : public IPlugin_WB_Main
	{
	public:
		CPlugin_WB_Main();
		~CPlugin_WB_Main();

	// 重写
	public:

		// 继承IPlugin_WB_Main接口
		// 获取插件名称
		virtual std::string Name();
		// 获取插件功能点
		virtual std::string ExtendPoint();
		// 初始化插件
		virtual bool InitPlugin();
		// 插件退出处理
		virtual bool ClosePlugin();

		// 执行接口
		virtual bool Init();
		virtual bool Run();
		virtual bool Close();

		virtual void Exit(void);

		//DECLARE_MESSAGE_MAP()
	};
}
