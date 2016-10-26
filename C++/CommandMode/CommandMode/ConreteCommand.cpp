#include "StdAfx.h"
#include "ConreteCommand.h"

CConreteCommand::CConreteCommand(CReceiver* pReveivrt)
{
	m_pReveivrt = pReveivrt;
}

CConreteCommand::~CConreteCommand(void)
{
}

void CConreteCommand::execute()
{
	std::cout<<"CConreteCommand::execute"<<endl;
	if (m_pReveivrt)
	{
		m_pReveivrt->action();
	}
}