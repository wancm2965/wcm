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
	//��������
	virtual void touchDownHandler(const int nTouchId);
	virtual void touchUpHandler(const int nTouchId);
	virtual void touchMoveHandler(const int nTouchId);

	//�����������Ƿ���Ҫִ��ͬ��
	bool  NeedTouchSynchronize(const int nTouchId);
private: 
	//ID����������ĵ�ӳ�� ��map����
	std::map<int,unsigned short> m_mapTouchId;
	//���δ�����ָȫ������֮ǰ�Ĵ����¼�
	unsigned long m_ulTouchEventId;
};
#endif