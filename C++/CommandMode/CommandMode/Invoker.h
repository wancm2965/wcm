#pragma once
#include "CommandBaseClass.h"
class CInvoker
{
public:
	CInvoker(CCommandBaseClass* pCommandbase);
	~CInvoker(void);

	void call();
private:
	CCommandBaseClass* m_pCommandbase;
};
