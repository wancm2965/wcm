#pragma once
#include "commandbaseclass.h"
#include "Receiver.h"

class CConreteCommand :
	public CCommandBaseClass
{
public:
	CConreteCommand(CReceiver* pReveivrt);
	virtual~CConreteCommand(void);

	virtual void execute();

private: 
	CReceiver* m_pReveivrt;
};
