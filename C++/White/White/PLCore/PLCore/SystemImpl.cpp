#include "SystemImpl.h"
#include <Windows.h>
#include <assert.h>

using namespace AVCONPlugin;
static int g_nTest = 0;//保证使用规范临时使用

SystemImpl::SystemImpl()
: m_strMainPlugin("")
{
}

SystemImpl::~SystemImpl()
{
	CloseAllPlugins();

	g_nTest = 1;
	//集体销毁插件，避免某一个插件对象在销毁时去访问其它插件
	m_mapPluginName2Plugins.clear();
	m_mapExtendPoint2Plugins.clear();

	for (size_t i=0; i<m_vecPlugins.size(); i++)
	{
		delete m_vecPlugins[i];
	}
	m_vecPlugins.clear();
}

bool SystemImpl::AddPlugin(IPlugin* ptrPlugin) 
{
	std::string strName = ptrPlugin->Name();
	std::string strExtendPoint = ptrPlugin->ExtendPoint();

	m_mapPluginName2Plugins[strName].push_back(ptrPlugin);
	m_mapExtendPoint2Plugins[strExtendPoint].push_back(ptrPlugin);

	m_vecPlugins.push_back(ptrPlugin);
	return true;
}

IPlugin* SystemImpl::GetPluginByName(const std::string& strPluginName)
{
	std::map<std::string,std::vector<IPlugin*>>::iterator it = m_mapPluginName2Plugins.find(strPluginName);
	if (m_mapPluginName2Plugins.end() == it)
	{
		if (g_nTest)
		{
			OutputDebugString(TEXT("\nWarning!!!!!在插件集体销毁时还存在插件间相互访问,有潜在危险"));
			assert(false);
		}
		return NULL;
	}

	if (it->second.size() <= 0)
		return NULL;

	return it->second[0];
}

IPlugin* SystemImpl::GetPluginByExtendPoint(const std::string& strPluginName)
{
	//if(m_mapExtendPoint2Plugins[strPluginName].size()>0)
	//	return m_mapExtendPoint2Plugins[strPluginName][0];
	std::map<std::string,std::vector<IPlugin*>>::iterator it = m_mapExtendPoint2Plugins.find(strPluginName);
	if (m_mapExtendPoint2Plugins.end() == it)
		return NULL;

	if (it->second.size() <= 0)
		return NULL;

	return it->second[0];
}

bool SystemImpl::GetPluginByName(const std::string& strPluginName,std::vector<IPlugin*>& vecPtrPlugin)
{
	//vecPtrPlugin = m_mapPluginName2Plugins[strPluginName];
	std::map<std::string,std::vector<IPlugin*>>::iterator it = m_mapPluginName2Plugins.find(strPluginName);
	if (m_mapPluginName2Plugins.end() == it)
		return false;

	vecPtrPlugin = it->second;
	return vecPtrPlugin.size()>0;
}

bool SystemImpl::GetPluginByExtendPoint(const std::string& strPluginExtendPoint,std::vector<IPlugin*>& vecPtrPlugin) 
{
	//vecPtrPlugin = m_mapExtendPoint2Plugins[strPluginExtendPoint];
	std::map<std::string,std::vector<IPlugin*>>::iterator it = m_mapExtendPoint2Plugins.find(strPluginExtendPoint);
	if (m_mapExtendPoint2Plugins.end() == it)
		return false;

	vecPtrPlugin = it->second;
	return vecPtrPlugin.size()>0;
}

std::vector<IPlugin*> SystemImpl::GetAllPlugins() 
{
	return m_vecPlugins;
}

void SystemImpl::CloseAllPlugins() 
{
	for(size_t i=0; i<m_vecPlugins.size(); i++)
	{
		if (m_vecPlugins[i]->Name() != m_strMainPlugin)
			m_vecPlugins[i]->ClosePlugin();
	}

	IPlugin* pPlugin = GetPluginByName(m_strMainPlugin);
	if (pPlugin)
		pPlugin->ClosePlugin();
}
