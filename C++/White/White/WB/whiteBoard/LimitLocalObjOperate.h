#ifndef  _LIMITL_OACALOBJ_OPERATE_H_
#define _LIMITL_OACALOBJ_OPERATE_H_
#pragma  once
#include "stdafx.h"
#include <time.h>
#include <map>
#include <list>
#define MaxLimitObjTime		5*CLOCKS_PER_SEC //����ƶ������ʱ��
class CLimitLocalObjOperate
{
public:
	CLimitLocalObjOperate();
	~CLimitLocalObjOperate();
public:
	//�²�����޸����ƶ���id�����Ĺ㲥�޸�ʱ��
	void AddLimitObjId(unsigned long ulObjId);
	//ɾ���޶�����id
	bool RemoveLimitObjId(unsigned long ulObjId);
	//�ж�����id�Ƿ������ƶ���id
	bool JudgeLimitObjId(unsigned long ulObjId) const;
	bool LimitObjEmpty(); 
	//��ʱ���Ķ��� �ص�����
//	static void CALLBACK  OnCallbackReliefObj(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);
	void _ReliefObjId(std::list<unsigned long> & EraseList);
	void _ReliefAllLimitObjs();

	///����ģʽ
public:
// 	static CLimitLocalObjOperate& _GetInstance()
// 	{
// 		return m_instance;
// 	};
protected:
	///Ψһ����
/*	static CLimitLocalObjOperate m_instance;*/

	std::map<unsigned long, clock_t> m_mapLimitObj;//keyΪ����id��secondΪ����㲥�޸�ʱ��
	XCritSec m_csmap;
	clock_t	m_clockNearest; //������ж�ʱ��
};
#endif