#include "StdAfx.h"
#include "CookCommand.h"

CCookCommand::CCookCommand(void)
{
	initCooker();
}

CCookCommand::~CCookCommand(void)
{
	unintCooker();
}

void CCookCommand::initCooker()
{
	CCooker* pCooker = new CCooker(ActionType::Tea);
	m_mapCookers.insert(make_pair(ActionType::Tea,pCooker));
	pCooker = new CCooker(ActionType::Coffee);
	m_mapCookers.insert(make_pair(ActionType::Coffee,pCooker));
	pCooker = new CCooker(ActionType::Water);
	m_mapCookers.insert(make_pair(ActionType::Water,pCooker));
}

void CCookCommand::DoCooker(int nType)
{
	std::map<int,CCooker*>::iterator it = m_mapCookers.find(nType);
	if (it != m_mapCookers.end())
	{
		CCooker*pCooker = it->second;
		pCooker->cookDrinks();
	}
}

void CCookCommand::unintCooker()
{
	std::map<int,CCooker*>::iterator it = m_mapCookers.begin();
	for(;it != m_mapCookers.end();++it)
	{
		CCooker*pCooker = it->second;
		if (pCooker)
		{
			delete pCooker;
			pCooker = NULL;
		}
	}
	m_mapCookers.clear();
}
