#pragma once

//命令模式处理的基类 提供父类的处理函数接口
//没办法 一直提示说保留的类名
class CCommandBaseClass
{
public:
	CCommandBaseClass(void);
	~CCommandBaseClass(void);

	//处理函数接口
	virtual void execute();

	virtual void DoCooker(int nType);
};
