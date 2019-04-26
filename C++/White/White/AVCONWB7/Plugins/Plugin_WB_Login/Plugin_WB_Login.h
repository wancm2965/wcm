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
		//��д
	public:
		//�̳�IPlugin�ӿ�
		virtual std::string Name(); 
		virtual std::string ExtendPoint(); //��ȡ������ܵ�
		virtual bool InitPlugin();
		virtual bool ClosePlugin();
		
		//�̳�IPugin_WB_Login�ӿ�
		virtual void Show(bool bshow);
		virtual void Destroy();
		//ʵ������Ĺ���
	private:
		CWBLoginDlg *m_pWBLoginDlg;
	};
}
#endif