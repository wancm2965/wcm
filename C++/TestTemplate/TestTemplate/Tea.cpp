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
	printf("用沸水煮茶叶5分钟");
}

void CTea::addCondiments()
{
	printf("加入柠檬到茶里");
}

bool CTea::bAddCond()
{
	return m_bCan;
}

void CTea::SetAddCon( bool bCan )
{
	m_bCan = bCan;
}