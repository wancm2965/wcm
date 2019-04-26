#ifndef _AVCON_WBMTDRAWOBJECTTALLY_H_
#define _AVCON_WBMTDRAWOBJECTTALLY_H_
#pragma once
#include <map>
using namespace std;
class CWBMTDrawObjectTally
{
public:
	CWBMTDrawObjectTally();
	~CWBMTDrawObjectTally();
public:
	bool JudgeTouchUpAllCursor();
	unsigned long GenerateEvent() {	return m_ulTouchEventId;		};
	//触摸弹起
	virtual void touchDownHandler(const int nTouchId);
	virtual void touchUpHandler(const int nTouchId);
	virtual void touchMoveHandler(const int nTouchId);

	//触摸过程中是否需要执行同步
	bool  NeedTouchSynchronize(const int nTouchId);
private: 
	//ID到触摸对象的的映射 用map容器
	std::map<int,unsigned short> m_mapTouchId;
	//本次触摸手指全部弹起之前的触摸事件
	unsigned long m_ulTouchEventId;
};
#endif