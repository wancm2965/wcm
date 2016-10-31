#pragma once
#include "Subject.h"
class CSubject;
class CObeserver
{
public:
	CObeserver(void);
	virtual~CObeserver(void);
	virtual void update(CSubject* psub)=0;
};
