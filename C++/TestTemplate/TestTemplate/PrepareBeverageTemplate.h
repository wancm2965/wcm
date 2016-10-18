#pragma once

class CPrepareBeverageTemplate
{
public:
	CPrepareBeverageTemplate(void);
	~CPrepareBeverageTemplate(void);

	//模板方法
	//主要提供模板化的四个步骤
	//其中有些是公用的方法可以在父类中实现 不同的方法可以virtual延迟到子类实现
	void startBeverageTemplate();

private:
	void boilWater();
	
protected:
	virtual void brew(){};
	
private:
	void pourInCup();	
	
protected:
	virtual void addCondiments(){};

protected:
	//是否添加调料的条件
	virtual bool bAddCond(){return false;};
};
