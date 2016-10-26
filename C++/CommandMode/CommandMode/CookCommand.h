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

	//执行请求命令类型nType
	virtual void DoCooker(int nType);
	

private:
	std::map<int,CCooker*> m_mapCookers;
};
