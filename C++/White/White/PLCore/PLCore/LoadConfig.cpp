#include "LoadConfig.h"

#include <Windows.h>

#include "tinyXml/XmlDocumentWrapper.h"
#include "CommFun.h"

using namespace AVCONPlugin;


// 转换一个路径到绝对路径
CString ConvertPathX2APath(const CString& strPath)
{
	TCHAR buf1[MAX_PATH];
	::memset(buf1, 0, MAX_PATH);

	GetFullPathName(strPath, MAX_PATH, buf1, NULL);

	return CString(buf1);
}

bool  DirectoryList(CString strPath,std::vector<CString>& vecFiles)
{
	WIN32_FIND_DATA FindData;
	HANDLE hError;
	int FileCount = 0;
	// 构造路径
	CString strFullPathName;

	hError = FindFirstFile(strPath + _T("\\*.*"), &FindData);
	if (hError == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	while (::FindNextFile(hError, &FindData))
	{
		// 过虑.和..
		if (_tcscmp(FindData.cFileName, _T(".")) == 0 
			|| _tcscmp(FindData.cFileName, _T("..")) == 0 )
		{
			continue;
		}

		// 构造完整路径
		strFullPathName = strPath + _T("\\") + FindData.cFileName;
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			DirectoryList(strFullPathName,vecFiles);
		}
		else
		{
			unsigned int nSize = strFullPathName.GetLength();
			if (nSize > 4)
			{
				CString strExec = strFullPathName.Right(4);
				if(strExec.Compare(_T(".xml")) == 0)
				{
					vecFiles.push_back(strFullPathName);
				}
			}
		}
	}
	::FindClose(hError);
	return true;
}

std::vector<CString> GetPluginConfig()
{
	CString strPluginPath = GetCurrentPath() + _T("Plugins");

	std::vector<CString> vecFiles;
	if(!DirectoryList(strPluginPath, vecFiles))
	{
		// to do
	}

	return vecFiles;
}

bool CLoadConfig::LoadConfig(const CString& strAppConfig,std::string& strAppPluginName,std::vector<CString>& vecDllNames)
{
	// Main config
	if(strAppPluginName.empty())
	{
		FILE* fp = 0;
		errno_t err = _wfopen_s( &fp, strAppConfig, _T("rb") );
		if ( NULL ==  fp )
			return false;

		TinyXml::CXmlDocumentWrapper xmlDoc;
		bool bLoad = xmlDoc.LoadDocument(fp);
		fclose(fp);
		if(!bLoad)
			return false;

		HANDLE hRoot = xmlDoc.GetRootNode();
		if(xmlDoc.GetNodeName(hRoot) != "Config")
			return false;

		HANDLE hApp;
		if(!xmlDoc.GetChildrenNodeByName(hRoot,"Main",hApp))
			return false;

		strAppPluginName = xmlDoc.GetNodeText(hApp);
	}

	// Plugin config
	{
		// 获取所有配置文件
		std::vector<CString> vecPluginsCfg = GetPluginConfig();

		CString strCurrentDir = GetCurrentPath();

		for(size_t i=0; i<vecPluginsCfg.size(); i++)
		{
			FILE* fp = 0;
			errno_t err = _wfopen_s( &fp, vecPluginsCfg[i], _T("rb") );
			if ( NULL ==  fp )
				continue;

			TinyXml::CXmlDocumentWrapper xmlDoc;
			bool bLoad = xmlDoc.LoadDocument(fp);
			fclose(fp);
			if(!bLoad)
				continue;

			HANDLE hRoot = xmlDoc.GetRootNode();
			if(xmlDoc.GetNodeName(hRoot) != "Config")
				continue;

			std::vector<HANDLE> vecHandles;
			xmlDoc.GetChildrenNodesByName(hRoot,"Plugin",vecHandles);

			// 设置当前路径到插件dll路径
			CString strPluginConfig = ConvertPathX2APath(vecPluginsCfg[i]);

			CString strDllPath = strPluginConfig.Left(strPluginConfig.ReverseFind(_T('\\')));
			::SetCurrentDirectory(strDllPath);

			for(size_t j=0; j<vecHandles.size(); j++)
			{
				HANDLE hDll;
				if(!xmlDoc.GetChildrenNodeByName(vecHandles[j],"Dll",hDll))
					continue;

				vecDllNames.push_back(ConvertPathX2APath(CString(CA2W(xmlDoc.GetNodeText(hDll).c_str()))));
			}

			// 恢复当前路径
			::SetCurrentDirectory(strCurrentDir);
		}
	}
	
	return true;
}