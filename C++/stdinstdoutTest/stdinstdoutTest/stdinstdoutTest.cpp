// stdinstdoutTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{

	SECURITY_ATTRIBUTES securityAttribute;
	securityAttribute.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttribute.lpSecurityDescriptor =NULL;
	securityAttribute.bInheritHandle = true;
	HANDLE stdinRead,stdinWrite;
	if (!CreatePipe(&stdinRead,&stdinWrite,&securityAttribute,0))
	{
		std::cerr<<"Create Pipe failed!!"<<std::endl;
		return 0;
	}
	HANDLE stdoutRead,stdoutWrite;
	if (!CreatePipe(&stdoutRead,&stdoutWrite,&securityAttribute,0))
	{
		std::cerr<<"Create Pipe failed!!"<<std::endl;
		return 0;
	}

	STARTUPINFO startupInfo;
	memset(&startupInfo,0,sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	startupInfo.wShowWindow = SW_HIDE;
	startupInfo.hStdInput = stdinRead;
	startupInfo.hStdOutput = stdoutWrite;
	startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	PROCESS_INFORMATION processInformation;
	if (!CreateProcess(L"E:\\git\\wcm\\C++\\stdinstdoutTest\\Debug\\Slaver.exe",NULL,NULL,NULL,TRUE,0,NULL,NULL,&startupInfo,&processInformation))
	{
		std::cerr<<"Create Process failed"<<std::endl;
		return 0;

	}

	std::string str;
	char buffer[4096];
	while(true)
	{
		std::cout<<">>";
		std::getline(std::cin,str);
		str += "\n";
		DWORD writtenByte;
		WriteFile(stdinWrite,str.data(),str.size(),&writtenByte,NULL);

		Sleep(100);

		DWORD readbyte;
		ReadFile(stdoutRead,buffer,sizeof(buffer),&readbyte,NULL);
		(std::cout<<"<<").write(buffer,readbyte)<<std::endl;
	}


	return 0;
}

