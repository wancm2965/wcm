#include "StdAfx.h"
#include "Invoker.h"

CInvoker::CInvoker(CCommandBaseClass* pCommandbase)
{
	m_pCommandbase = pCommandbase;
}

CInvoker::~CInvoker(void)
{
}

void CInvoker::call()
{
	std::cout<<"CInvoker::call"<<endl;
	if (m_pCommandbase)
	{
		m_pCommandbase->execute();
	}
}