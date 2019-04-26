#ifndef PL_ISYSTEM_H_8998222332342323
#define PL_ISYSTEM_H_8998222332342323

#include <string>
#include <vector>
#include <map>
//#include <boost/shared_ptr.hpp>

#include "IPlugin.h"

// ���ϵͳ���Ľӿ�
// ���в����ע�ᡢ��ȡ��ͨ���ýӿ����
// 
// ���в��������ɺ󣬻�Բ������һ�γ�ʼ�����˽ӿڻ���Ϊ�����ʼ���Ĳ���

namespace AVCONPlugin
{
	class ISystem
	{
	public:
		virtual ~ISystem(){};

	public:
		virtual bool AddPlugin(IPlugin* ptrPlugin) = 0;
		virtual IPlugin* GetPluginByName(const std::string& strPluginName) = 0;
		virtual IPlugin* GetPluginByExtendPoint(const std::string& strPluginName) = 0;
		virtual bool GetPluginByName(const std::string& strPluginName,std::vector<IPlugin*>& vecPtrPlugin) = 0;
		virtual bool GetPluginByExtendPoint(const std::string& strPluginExtendPoint,std::vector<IPlugin*>& vecPtrPlugin) = 0;
		virtual std::vector<IPlugin*> GetAllPlugins() = 0;
		virtual void CloseAllPlugins() = 0;
	};

	extern ISystem* g_System;
}

#endif