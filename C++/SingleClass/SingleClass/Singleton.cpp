#include "StdAfx.h"
#include "Singleton.h"
static CSingleton* g_SingleObj = NULL;
CSingleton::CSingleton(void)
{
}

CSingleton::~CSingleton(void)
{
}

CSingleton* CSingleton::Instance()
{
	if (g_SingleObj==NULL)
	{
		g_SingleObj = new CSingleton();
	}
	return g_SingleObj;
}