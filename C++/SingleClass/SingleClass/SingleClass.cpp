// SingleClass.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Singleton.h"

int _tmain(int argc, _TCHAR* argv[])
{
	if(CSingleton::Instance())
	{
		printf("instance the singleclass obj");
	}

	return 0;
}

