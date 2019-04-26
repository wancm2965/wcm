#include "LoadPlugins.h"

#include <set>
#include <iostream>

#include <Windows.h>

#include "ISystem/ISystem.h"
#include "CommFun.h"


using namespace AVCONPlugin;

typedef int(*PluginLoad)(ISystem* pSystem);


// 转换一个路径到绝对路径
CString ConvertPath2APath(const CString& strPath)
{
	TCHAR buf1[MAX_PATH];
	::memset(buf1, 0, MAX_PATH);

	GetFullPathName(strPath, MAX_PATH, buf1, NULL);

	return CString(buf1);
}

CLoadPlugins::~CLoadPlugins()
{
	for (int i=0; i<(int)m_vecModules.size(); i++)
		::FreeLibrary(m_vecModules[i]);
}

bool CLoadPlugins::LoadDll(const CString& strDll)
{
	::OutputDebugString(_T("================"));
	::OutputDebugString(strDll);
	::OutputDebugString(_T("================\n"));
	// 加载当前插件
	HMODULE hLibrary = ::LoadLibrary(strDll);
	if(!hLibrary)
	{
		DWORD dwError = GetLastError();
		//	Quit(vecModules);
		//std::cerr<<"Load library Error!"<<dwError<<std::endl;
		CString strError;
		strError.Format(_T("LoadLibrary Error:%u %s\n"), dwError, strDll);
		::OutputDebugString(strError);
		return false;
	}
 
	PluginLoad fun = (PluginLoad)(void*)::GetProcAddress(hLibrary,"LoadPlugin");
	if(!fun)
	{
		DWORD dwError = GetLastError();
		::FreeLibrary(hLibrary);
		//std::cerr<<"Load Error Library!"<<std::endl;
		CString strError;
		strError.Format(_T("GetProcAddress Error:%u %s\n"), dwError, strDll);
		::OutputDebugString(strError);
		return false;
	}

	m_vecModules.push_back(hLibrary);
	fun(g_System);

	return true;
}

bool CLoadPlugins::LoadPlugins(const std::vector<CString>& vecDlls)
{
	CString setDlls;
	CString strCurrentDir = GetCurrentPath();

	for(size_t i=0; i<vecDlls.size(); i++)
	{
		CString strDll = ConvertPath2APath(vecDlls[i]);
		if(setDlls.Find(strDll) != -1)
			continue;

		// LoadPlugin
		// 设置当前路径到插件dll路径
		CString strDllPath = strDll.Left(strDll.ReverseFind(_T('\\')));
		::SetCurrentDirectory(strDllPath);

		if (LoadDll(strDll))
		{
			setDlls += strDll + _T("?");
		}

		// 恢复当前路径
		::SetCurrentDirectory(strCurrentDir);
	}
	return true;
}
