#pragma once
#include "preparebeveragetemplate.h"

class CTea :
	public CPrepareBeverageTemplate
{
public:
	CTea(void);
	~CTea(void);

	void SetAddCon(bool bCan);

	void brew();
	void addCondiments();
	bool bAddCond();

private:
	bool m_bCan;
};
