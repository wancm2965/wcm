#pragma once
#include "commandbaseclass.h"
#include "Cooker.h"
#include <map>
class CCookCommand :
	public CCommandBaseClass
{
public:
	CCookCommand(void);
	~CCookCommand(void);
	void initCooker();
	void unintCooker();

	//ִ��������������nType
	virtual void DoCooker(int nType);
	

private:
	std::map<int,CCooker*> m_mapCookers;
};
