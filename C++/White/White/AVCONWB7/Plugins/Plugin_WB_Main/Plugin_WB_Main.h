#pragma once
#include "MainParentWnd.h"
#include "DlgMainFrame.h"
#include "IPlugin/Plugin_WB_Main/IPlugin_WB_Main.h"
namespace AVCONPlugin
{
	class CPlugin_WB_Main : public IPlugin_WB_Main
	{
	public:
		CPlugin_WB_Main();
		~CPlugin_WB_Main();
		//重写
	public:
		// 获取插件名称
		virtual std::string Name();
		// 获取插件功能点
		virtual std::string ExtendPoint();
		// 初始化插件
		virtual bool InitPlugin();
		// 插件退出处理
		virtual bool ClosePlugin();

		//主插件接口
		// 执行接口
		virtual bool Init();
		virtual bool Run();        // to do   添加命令行参数
		virtual bool Close();

		virtual void Exit(void);
		
		virtual HWND GetMainWnd();
		virtual HWND GetExeParentWnd();
	private:
		CMainParentWnd* m_pMainParent;
		CDlgMainFrame* m_pDlgMainFrame;
	};
}