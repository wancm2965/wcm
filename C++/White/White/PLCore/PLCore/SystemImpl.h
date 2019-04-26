#ifndef PL_SYSTEMIMPL_H_2892232323332234234
#define PL_SYSTEMIMPL_H_2892232323332234234

#include <string>
#include <vector>
#include <map>

#include "ISystem/ISystem.h"

namespace AVCONPlugin
{
	class SystemImpl : public ISystem
	{
	public:
		SystemImpl();
		~SystemImpl();

	public:
		bool AddPlugin(IPlugin* ptrPlugin) ;
		bool GetPluginByName(const std::string& strPluginName,std::vector<IPlugin*>& vecPtrPlugin) ;
		bool GetPluginByExtendPoint(const std::string& strPluginExtendPoint,std::vector<IPlugin*>& vecPtrPlugin) ;
		IPlugin* GetPluginByName(const std::string& strPluginName);
		IPlugin* GetPluginByExtendPoint(const std::string& strPluginName);
		std::vector<IPlugin*> GetAllPlugins() ;
		void CloseAllPlugins();

		void SetMainPlugin(const std::string& strMainPlugin) { m_strMainPlugin = strMainPlugin; };

	protected:
		std::vector<IPlugin*> m_vecPlugins;
		std::map<std::string,std::vector<IPlugin*>> m_mapPluginName2Plugins;
		std::map<std::string,std::vector<IPlugin*>> m_mapExtendPoint2Plugins;
		std::string m_strMainPlugin;
	};
}

#endif