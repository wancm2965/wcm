#include "WBMTObject.h"
#include <assert.h>

CWBMTObject::CWBMTObject()
	: m_bCounting(1)
	,m_bChangID(false)
{

}

CWBMTObject::~CWBMTObject()
{
	m_map.clear();
}



bool CWBMTObject::DoesCarryCounting() const
{

	return (0 == m_bCounting%15);
}

void CWBMTObject::touchUpHandler( const AVCON_SDK_MTTouch::TouchUp& te )
{
#ifdef SUPPORTMTGES
	return;
#else


	++m_bCounting;
	std::map<unsigned long, unsigned long>::iterator it = m_map.find(te.id);
	if (m_map.end() == it)
	{
#ifdef _DEBUG
		//assert(false);
#endif
	}
	else
	{
		m_map.erase(it);
	}

	if (m_map.empty())
	{
		TouchUpAllCursor(m_bChangID,te);	
		m_bChangID = false;
	}
	#endif
}

void CWBMTObject::touchDownHandler( const TouchDown& te )
{
#ifdef SUPPORTMTGES
	return;
#else

	std::map<unsigned long, unsigned long>::iterator it = m_map.find(te.id);
	if (m_map.end() != it)//正常情况下都会找不到
	{
#ifdef _DEBUG
		//assert(false);
#endif
		m_map.erase(it);
		m_bCounting = 0;//重置
	}
	else
	{
		m_bCounting %= 10;
		++m_bCounting;
	}
	if (m_map.empty())
	{
		m_bChangID = false;
	}
	m_map.insert(std::pair<unsigned long, unsigned long>(te.id, te.time));

	#endif
}

void CWBMTObject::touchMoveHandler( const TouchMove& te )
{
#ifdef SUPPORTMTGES
	return;
#else
	++m_bCounting;

	#endif
}

void CWBMTObject::OnGestureCombineHandle( const AVCON_SDK_MTTouch::GestureCombinePar &gesPar )
{
#ifdef SUPPORTMTGES
	return;
#else
	if (!m_bChangID)
	{
		if ((abs(gesPar.xTransCumulative) > 10) 
			||(abs(gesPar.yTransCumulative) > 10)
			||(gesPar.deltaRatioCumulative > 1.05)
			|| (gesPar.deltaRatioCumulative <0.95)
			|| (abs(gesPar.rotateAngleCumulative) >0.5))
		{
			m_bChangID = true;
		}
	}
#endif
}
