#include "LimitLocalObjOperate.h"

CLimitLocalObjOperate::CLimitLocalObjOperate()
	: m_clockNearest(0)
{
}
CLimitLocalObjOperate::~CLimitLocalObjOperate()
{
	m_mapLimitObj.clear();
}


void CLimitLocalObjOperate::AddLimitObjId( unsigned long ulObjId )
{
	XAutoLock l(m_csmap);
	bool bEmpty = m_mapLimitObj.empty();
	std::map<unsigned long, clock_t>::iterator it = m_mapLimitObj.find(ulObjId);
	if (it != m_mapLimitObj.end())
		it->second = clock();
	else
		m_mapLimitObj.insert(std::map<unsigned long, clock_t>::value_type(ulObjId,clock()));
}


bool CLimitLocalObjOperate::RemoveLimitObjId( unsigned long ulObjId )
{
	XAutoLock l(m_csmap);
	std::map<unsigned long,clock_t>::iterator it = m_mapLimitObj.find(ulObjId);
	if (it != m_mapLimitObj.end())
	{
		m_mapLimitObj.erase(it);
		return true;
	}
	return false;
}


bool CLimitLocalObjOperate::JudgeLimitObjId( unsigned long ulObjId ) const
{
	return (m_mapLimitObj.find(ulObjId) != m_mapLimitObj.end() ? true : false);
}

void CLimitLocalObjOperate::_ReliefObjId(std::list<unsigned long> & EraseList)
{
	XAutoLock l(m_csmap);
	std::map<unsigned long, clock_t>::iterator it = m_mapLimitObj.begin();
	m_clockNearest = clock(); 
	while (it != m_mapLimitObj.end())
	{
		clock_t temp = it->second;
		if(m_clockNearest > temp && (m_clockNearest - temp >= MaxLimitObjTime))
			EraseList.push_back(it->first);
		++it;
	}
	if(EraseList.empty()) return;
	std::list<unsigned long>::iterator _it_list = EraseList.begin();
	while(_it_list != EraseList.end())
	{
		m_mapLimitObj.erase(*_it_list);
		++_it_list;
	}
	return ;
}

bool CLimitLocalObjOperate::LimitObjEmpty()
{
	XAutoLock l(m_csmap);
	return m_mapLimitObj.empty();
}

void CLimitLocalObjOperate::_ReliefAllLimitObjs()
{
	XAutoLock l(m_csmap);
	m_mapLimitObj.clear();
}



