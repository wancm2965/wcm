#pragma once

class CCooker
{
public:
	CCooker(int nType);
	~CCooker(void);
	
	void cookDrinks();
private:
	int m_nCookType;//泡制类型  茶 咖啡 水
};
