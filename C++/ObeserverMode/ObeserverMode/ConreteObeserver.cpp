#include "StdAfx.h"
#include "ConreteObeserver.h"

CConreteObeserver::CConreteObeserver(std::string strname)
{
	m_strName = strname;
}

CConreteObeserver::~CConreteObeserver(void)
{
}

void CConreteObeserver::update( CSubject* psub )
{
	if (psub)
	{
		int nstate = psub->getState();
		std::cout<<"update obeserver [" << m_strName.c_str()<<"] state:"<<nstate<<endl;
	}
}