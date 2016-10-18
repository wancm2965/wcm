#include "StdAfx.h"
#include "Coffee.h"

CCoffee::CCoffee(void)
{
	m_bCan = false;
}

CCoffee::~CCoffee(void)
{
}

void CCoffee::brew()
{
	printf("泡制咖啡");
}

void CCoffee::addCondiments()
{
	printf("加入糖和牛奶");
}

bool CCoffee::bAddCond()
{
	return m_bCan;
}

void CCoffee::SetbCan( bool bCan )
{
	m_bCan = bCan;
}