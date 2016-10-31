// ObeserverMode.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ConcreteSubject.h"
#include "ConreteObeserver.h"

int _tmain(int argc, _TCHAR* argv[])
{

	//////////////////////////////////////////////////////////////////////////
	/*
	�۲���ģʽ
	���۲���notify  ֪ͨ�����۲���
	*/
	CSubject* subject = new CConcreteSubject();
	CObeserver* objA = new CConreteObeserver("A");
	CObeserver* objB = new CConreteObeserver("B");
	
	subject->attach(objA);
	subject->attach(objB);

	subject->SetState(1);
	subject->notify();

	std::cout<<"-------------------------------------"<<endl;
	subject->detach(objB);
	subject->SetState(2);
	subject->notify();

	delete subject;
	delete objA;
	delete objB;



	
	return 0;
}

