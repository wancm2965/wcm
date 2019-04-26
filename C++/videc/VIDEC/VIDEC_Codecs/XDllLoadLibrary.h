#ifndef __XDLLLOADLIBRARY_H__
#define __XDLLLOADLIBRARY_H__

#include <winsock2.h>
#include <windows.h>

class XDllLoadLibrary
{
public:
	XDllLoadLibrary(const char *cszDllName)
	{
		//CString strDLLName=cszDllName;
		//::MessageBox(NULL,cszDllName,cszDllName,MB_OK);
		m_hDllModule=LoadLibrary(cszDllName);
		DWORD dwError=GetLastError();
		m_bIsOK=(m_hDllModule!=NULL);
	}
	virtual~XDllLoadLibrary()
	{
		if (m_hDllModule) 
		{
			FreeLibrary(m_hDllModule);
			m_hDllModule=NULL;
		}
	}
public:
	bool IsOK(void)
	{
		return m_bIsOK;
	}

	template<class T>
	void LoadFunction(T &pFunction,const char *cszFunctionName)
	{
		pFunction=m_hDllModule?(T)GetProcAddress(m_hDllModule,cszFunctionName):NULL;
		m_bIsOK&=(pFunction!=NULL);
	}
protected:
	bool	m_bIsOK;
	HMODULE m_hDllModule;
};

#endif
