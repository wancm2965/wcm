#include "StdAfx.h"
#include "Custment.h"

CCustment::CCustment(CCommandBaseClass* pCommand)
{
	m_pCommand = pCommand;
}

CCustment::~CCustment(void)
{
}

void CCustment::CallDrink( int nType )
{
	if (m_pCommand)
	{
		m_pCommand->DoCooker(nType);
	}
}