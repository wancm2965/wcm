#include "StdAfx.h"
#include "PrepareBeverageTemplate.h"

CPrepareBeverageTemplate::CPrepareBeverageTemplate(void)
{
}

CPrepareBeverageTemplate::~CPrepareBeverageTemplate(void)
{
}

void CPrepareBeverageTemplate::startBeverageTemplate()
{
	boilWater();
	brew();
	pourInCup();
	if(bAddCond())
	{
		addCondiments();
	}
	
}

void CPrepareBeverageTemplate::boilWater()
{
	printf("步骤一");
}

void CPrepareBeverageTemplate::pourInCup()
{
	printf("步骤三");
}