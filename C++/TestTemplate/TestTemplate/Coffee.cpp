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
	printf("���ƿ���");
}

void CCoffee::addCondiments()
{
	printf("�����Ǻ�ţ��");
}

bool CCoffee::bAddCond()
{
	return m_bCan;
}

void CCoffee::SetbCan( bool bCan )
{
	m_bCan = bCan;
}