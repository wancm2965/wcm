// ObeserverMode.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ConcreteSubject.h"
#include "ConreteObeserver.h"

int _tmain(int argc, _TCHAR* argv[])
{

	//////////////////////////////////////////////////////////////////////////
	/*
	观察者模式
	被观察者notify  通知其他观察者
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

