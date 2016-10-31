#pragma once
#include "Obeserver.h"
#include <vector>
using namespace std;

class CObeserver;
class CSubject
{
public:
	CSubject(void);
	virtual~CSubject(void);
	

	void attach(CObeserver* pobserver);
	void detach(CObeserver* pobserver);

	void notify();

	virtual int getState()=0;
	virtual void SetState(int nState)=0;

private:
	std::vector<CObeserver*> m_vctObj;

};
