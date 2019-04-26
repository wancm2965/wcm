#ifndef  _LIMITL_OACALOBJ_OPERATE_H_
#define _LIMITL_OACALOBJ_OPERATE_H_
#pragma  once
#include "stdafx.h"
#include <time.h>
#include <map>
#include <list>
#define MaxLimitObjTime		5*CLOCKS_PER_SEC //最长限制对象操作时间
class CLimitLocalObjOperate
{
public:
	CLimitLocalObjOperate();
	~CLimitLocalObjOperate();
public:
	//新插入或修改限制对象id及最后的广播修改时间
	void AddLimitObjId(unsigned long ulObjId);
	//删除限定对象id
	bool RemoveLimitObjId(unsigned long ulObjId);
	//判定对象id是否是限制对象id
	bool JudgeLimitObjId(unsigned long ulObjId) const;
	bool LimitObjEmpty(); 
	//定时更改对象 回调函数
//	static void CALLBACK  OnCallbackReliefObj(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);
	void _ReliefObjId(std::list<unsigned long> & EraseList);
	void _ReliefAllLimitObjs();

	///单例模式
public:
// 	static CLimitLocalObjOperate& _GetInstance()
// 	{
// 		return m_instance;
// 	};
protected:
	///唯一单例
/*	static CLimitLocalObjOperate m_instance;*/

	std::map<unsigned long, clock_t> m_mapLimitObj;//key为对象id，second为最近广播修改时间
	XCritSec m_csmap;
	clock_t	m_clockNearest; //最近的判定时间
};
#endif