#pragma once

//����ģʽ����Ļ��� �ṩ����Ĵ������ӿ�
//û�취 һֱ��ʾ˵����������
class CCommandBaseClass
{
public:
	CCommandBaseClass(void);
	~CCommandBaseClass(void);

	//�������ӿ�
	virtual void execute();

	virtual void DoCooker(int nType);
};
