#pragma once

class CPrepareBeverageTemplate
{
public:
	CPrepareBeverageTemplate(void);
	~CPrepareBeverageTemplate(void);

	//ģ�巽��
	//��Ҫ�ṩģ�廯���ĸ�����
	//������Щ�ǹ��õķ��������ڸ�����ʵ�� ��ͬ�ķ�������virtual�ӳٵ�����ʵ��
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
	//�Ƿ���ӵ��ϵ�����
	virtual bool bAddCond(){return false;};
};
