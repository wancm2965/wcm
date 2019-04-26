#include "stdafx.h"
#include "WBMTDrawObjectTally.h"

CWBMTDrawObjectTally::CWBMTDrawObjectTally()
	: m_ulTouchEventId(GenerateSSRC())
{
}
CWBMTDrawObjectTally::~CWBMTDrawObjectTally()
{
	m_mapTouchId.clear();
}


void CWBMTDrawObjectTally::touchDownHandler( const int nTouchId )
{
	if(m_mapTouchId.empty())//这里事件是在新的一次触摸操作的时候才重新设定
		m_ulTouchEventId = GenerateSSRC();
	std::map<int, unsigned short>::iterator it = m_mapTouchId.find(nTouchId);
	if (it != m_mapTouchId.end())
	{
		m_mapTouchId.erase(it);
	}
	m_mapTouchId.insert(std::map<int,unsigned short>::value_type(nTouchId,0));
}

void CWBMTDrawObjectTally::touchUpHandler( const int nTouchId )
{
	std::map<int,unsigned short>::iterator it = m_mapTouchId.find(nTouchId);
	if (it != m_mapTouchId.end())
	{
		m_mapTouchId.erase(it);
	}
	else
		printf("ulTouchId = %d,TouchUp error",nTouchId);
}
void CWBMTDrawObjectTally::touchMoveHandler( const int nTouchId )
{
	std::map<int,unsigned short>::iterator it = m_mapTouchId.find(nTouchId);
	if (it != m_mapTouchId.end())
	{
		it->second++;
		if (it->second > 1000)
		{
			it->second %=1000; 
		}
	}
	else
		printf("ulTouchId = %d,TouchMove error",nTouchId);
}

bool CWBMTDrawObjectTally::JudgeTouchUpAllCursor()
{
	return m_mapTouchId.empty() ? true : false;
}

bool CWBMTDrawObjectTally::NeedTouchSynchronize(int nTouchId) 
{
	std::map<int,unsigned short>::iterator it = m_mapTouchId.find(nTouchId);
	if (it != m_mapTouchId.end())
	{
		return (0 == it->second%10);
	}
	else
		return false;
}

