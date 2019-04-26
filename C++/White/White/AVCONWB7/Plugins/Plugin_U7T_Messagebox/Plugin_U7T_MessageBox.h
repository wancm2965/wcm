#pragma once
#include "IPlugin\IU7_MessageBox\IPlugin_U7T_MessageBox.h"
#include "GlobalMessageBoxManager.h"

namespace AVCONPlugin
{
	class CPlugin_U7T_MessageBox : public IPlugin_U7T_MessageBox
	{
	public:
		CPlugin_U7T_MessageBox(void);
		~CPlugin_U7T_MessageBox(void);

	// 继承自IPlugin接口
	public:
		// 获取插件名称
		virtual std::string Name();
		// 获取插件功能点
		virtual std::string ExtendPoint();
		// 初始化插件
		virtual bool InitPlugin();
		// 插件退出处理
		virtual bool ClosePlugin();

	// 继承自IPlugin_U7T_MessageBox接口
	public:
		///弹出一个相对模态窗体
		virtual int MessageBox(HWND hOwner,CString strTitle,CString strContent,UINT nStye, bool bAutoDestroy = false);
		///关闭所有弹出式的窗体
		virtual void CloseAllMessageBox();

	private:
		CGlobalMessageBoxManager m_GlobalMessageBoxManager;
	};
}
