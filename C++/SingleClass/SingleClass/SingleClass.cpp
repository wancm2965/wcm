// SingleClass.cpp : �������̨Ӧ�ó������ڵ㡣
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

