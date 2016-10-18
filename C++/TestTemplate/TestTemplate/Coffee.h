#pragma once
#include "preparebeveragetemplate.h"

class CCoffee :
	public CPrepareBeverageTemplate
{
public:
	CCoffee(void);
	~CCoffee(void);


	void brew();

	void addCondiments();
	bool bAddCond();

	void SetbCan(bool bCan);

private:
	bool m_bCan;
};
