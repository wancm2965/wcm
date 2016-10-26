#include "StdAfx.h"
#include "CommandBaseClass.h"

CCommandBaseClass::CCommandBaseClass(void)
{
}

CCommandBaseClass::~CCommandBaseClass(void)
{
}

void CCommandBaseClass::execute()
{
	std::cout<<"CCommandBaseClass::execute"<< endl;
}

void CCommandBaseClass::DoCooker( int nType )
{

}