// TestReadFile.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <algorithm>
void CaptureFileOutput(CString strFile);
void ReadFFF(CString strPartFile);

static int  nPreReadLine = 0;
int _tmain(int argc, _TCHAR* argv[])
{
	CString strFile = _T("D:\\123.txt");
	//CaptureFileOutput(strFile);
	while (true)
	{
		ReadFFF(strFile);
	}
	
	return 0;
}

void CaptureFileOutput(CString strFile)
{
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{		
		return;
	}
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	TCHAR szFullName[MAX_PATH] = { 0 };
#if (_MSC_VER >= 1500)
	_tcscpy_s(szFullName, MAX_PATH, strFile);
#else
	_tcscpy(szFullName, strBatFile);
#endif

	if (!CreateProcess(NULL, szFullName, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{		
		int nErr = GetLastError();
		CloseHandle(hWrite);
		CloseHandle(hRead);
		return;
	}	

	char buffer[4096] = { 0 };
	DWORD bytesRead;
	while (true)
	{
		if (ReadFile(hRead, buffer, 4095, &bytesRead, NULL) == NULL)
		{
			
			//break;
		}
		//////////////////////////////////////////////////////////////////////////
// 		OnCaptureCallBack(buffer);
// 		if (m_enCaptureStatue == CAPTURE_SUC)
// 		{
// 			break;
// 		}

		int n = 0;
		int m = 0;
	}
	CloseHandle(hRead);
	CloseHandle(hWrite);
}

void ReadFFF(CString strPartFile)
{
	std::string strFile = std::string(CT2A(strPartFile, CP_UTF8));
	ifstream infile;
	infile.open(strFile.data());
	bool bFind = false;
	string s;
	int nLine = 0;
	while (getline(infile, s))
	{
		nLine++;
		if (nLine < nPreReadLine+1)
		{
			continue;
		}
		nPreReadLine = nLine;
		//先检查Error
		std::string strErr = "Error";
		std::size_t err = s.find(strErr);
		if (err != std::string::npos)
		{
			//
			infile.close();
			return;
		}
		else
		{
			strErr = "错误";
			err = s.find(strErr);
			if (err != std::string::npos)
			{
				infile.close();
				return;
			}
		}

		std::string strFind = "[";
		std::size_t found = s.find(strFind);
		if (found != std::string::npos)
		{
			std::string strNext = "]";
			std::size_t found1 = s.find(strNext);
			if (found1 != std::string::npos && found<found1)
			{
				std::string strGet = s.substr(found+1, found1-2);
				if (!strGet.empty())
				{
					int index = 0;
					
					while (1)
					{
						if ((index = strGet.find(' ', index)) != std::string::npos)
						{
							strGet.erase(index, 1);
							continue;
						}
						else
						{
							index = 0;
							if ((index = strGet.find('=', index)) != std::string::npos)
							{
								strGet.erase(index, 1);
								continue;
							}
							else
							{
								index = 0;
								if ((index = strGet.find('%', index)) != std::string::npos)
								{
									strGet.erase(index, 1);
									continue;
								}								
							}
						}
						break;
					}
				}
				int nTest = atoi(strGet.c_str());
				int l = 0;
			}
		}
		
	}
	infile.close();
}