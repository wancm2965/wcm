#include "StdAfx.h"
#include "Tea.h"

CTea::CTea(void)
{
	m_bCan =false;
}

CTea::~CTea(void)
{
}

void CTea::brew()
{
	printf("�÷�ˮ���Ҷ5����");
}

void CTea::addCondiments()
{
	printf("�������ʵ�����");
}

bool CTea::bAddCond()
{
	return m_bCan;
}

void CTea::SetAddCon( bool bCan )
{
	m_bCan = bCan;
}