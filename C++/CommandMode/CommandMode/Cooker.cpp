#include "StdAfx.h"
#include "Cooker.h"

CCooker::CCooker(int nType)
{
	m_nCookType = nType;
}

CCooker::~CCooker(void)
{
}

void CCooker::cookDrinks()
{
	switch(m_nCookType)
	{
	case ActionType::Tea:
		std::cout<<"cook Tea Drinks"<<endl;

		break;
	case ActionType::Coffee:
		std::cout<<"cook Coffee Drinks"<<endl;

		break;
	case ActionType::Water:
		std::cout<<"cook Water Drinks"<<endl;

		break;
	default:
		std::cout<<"cook none!"<<endl;

		break;
	}
}