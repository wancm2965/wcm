#pragma once
#include "subject.h"

class CConcreteSubject :
	public CSubject
{
public:
	CConcreteSubject(void);
	~CConcreteSubject(void);

	virtual int getState();
	virtual void SetState(int nState);

private:
	int m_nstate;
};
