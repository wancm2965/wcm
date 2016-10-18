#pragma once

class CSingleton
{
private:
	CSingleton(void);
	~CSingleton(void);

	

public:
	static CSingleton* Instance();
};
