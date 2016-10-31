#include "StdAfx.h"
#include "Subject.h"
#include <algorithm>

CSubject::CSubject(void)
{
}

CSubject::~CSubject(void)
{
}

void CSubject::attach( CObeserver* pobserver )
{
	std::vector<CObeserver*>::iterator it =  std::find(m_vctObj.begin(),m_vctObj.end(),pobserver);
	if (it == m_vctObj.end())
	{
		m_vctObj.push_back(pobserver);
	}
}

void CSubject::detach( CObeserver* pobserver )
{
	for(vector<CObeserver*>::iterator itr = m_vctObj.begin();itr != m_vctObj.end(); itr++)
	{
		if(*itr == pobserver)
		{
			m_vctObj.erase(itr);
			return;
		}			
	}
}

void CSubject::notify()
{
	for(vector<CObeserver*>::iterator itr = m_vctObj.begin();itr != m_vctObj.end(); ++itr)
	{
		CObeserver* pobserver = (CObeserver* )*itr;
		if(NULL != pobserver)
		{
			pobserver->update(this);
		}			
	}
}