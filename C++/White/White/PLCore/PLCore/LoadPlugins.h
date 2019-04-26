#ifndef PL_LOADPLUGINS_H_2892233234234434234
#define PL_LOADPLUGINS_H_2892233234234434234

#include <string>
#include <vector>
#include <atlstr.h>

#include <Windows.h>

namespace AVCONPlugin
{
	class CLoadPlugins
	{
	public:
		~CLoadPlugins();

	public:
		// [In]
		//		vecDlls:   插件链接库文件
		bool LoadPlugins(const std::vector<CString>& vecDlls);

	protected:
		//
		bool LoadDll(const CString& strDll);

	protected:
		std::vector<HMODULE> m_vecModules;
	};
}


#endif