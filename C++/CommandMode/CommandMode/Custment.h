#pragma once
#include "CommandBaseClass.h"
class CCustment
{
public:
	CCustment(CCommandBaseClass* pCommand);
	~CCustment(void);

	void CallDrink(int nType);

private:
	CCommandBaseClass* m_pCommand;
};
