// Slaver.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
int _tmain(int argc, _TCHAR* argv[])
{
	std::string str;
	while(std::getline(std::cin,str))
		std::cout<<str<<std::endl;
	return 0;
}

