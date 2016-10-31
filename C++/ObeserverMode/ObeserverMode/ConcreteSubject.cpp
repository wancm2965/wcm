#include "StdAfx.h"
#include "ConcreteSubject.h"

CConcreteSubject::CConcreteSubject(void)
{
	m_nstate = 0;
}

CConcreteSubject::~CConcreteSubject(void)
{
}

int CConcreteSubject::getState()
{
	return m_nstate;
}

void CConcreteSubject::SetState( int nState )
{
	m_nstate = nState;
}