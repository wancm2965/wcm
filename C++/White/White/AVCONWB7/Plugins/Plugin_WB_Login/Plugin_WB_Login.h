#ifndef	_PLUGIN_WB_LOGIN_H__
#define _PLUGIN_WB_LOGIN_H__

#include "stdafx.h"
#include "IPlugin/Plugin_WB_Login/IPlugin_WB_Login.h"
#include "WBLoginDlg.h"

namespace	AVCONPlugin
{
	class CPlugin_WB_Login : public IPlugin_WB_Login
	{
	public:
		CPlugin_WB_Login();
		~CPlugin_WB_Login();
		//重写
	public:
		//继承IPlugin接口
		virtual std::string Name(); 
		virtual std::string ExtendPoint(); //获取插件功能点
		virtual bool InitPlugin();
		virtual bool ClosePlugin();
		
		//继承IPugin_WB_Login接口
		virtual void Show(bool bshow);
		virtual void Destroy();
		//实现自身的功能
	private:
		CWBLoginDlg *m_pWBLoginDlg;
	};
}
#endif