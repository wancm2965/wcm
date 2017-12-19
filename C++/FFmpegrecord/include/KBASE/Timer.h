#pragma once

#include "KBASE/KBASE_Export.h"
#include "Thread.h"
#include "CritSec.h"
#include <map>

//---------------------------------------------------------------------------------------
typedef struct _TIMER_EVENT
{
	unsigned long elapse;
	unsigned long timestamp;
}TIMER_EVENT;

//---------------------------------------------------------------------------------------
typedef std::map<unsigned int,TIMER_EVENT>	TIMER_EVENT_MAP;

//---------------------------------------------------------------------------------------
class KBASE_API KTimer : public KThread
{
public:
	KTimer(void);
	~KTimer(void);

	/* 启动计时器 */
	bool StartTimer(void);

	/* 停止计时器 */
	void StopTimer(void);

	/* 设置计时器事件: nEventID=事件ID; nElapse=事件间隔(ms) */
	void SetTimerEvent(unsigned int nEventID,unsigned long ulElapse);

	/* 删除计时器时间: nEvnetID=指定删除的事件ID */
	void KillTimerEvent(unsigned int nEventID);
        
	//判断计时器线程是否在运行中
    bool IsTimerThreadRun();

protected:
	/* 事件触发回调,继承该方法处理事件(虚函数) */
	virtual void OnTimerEvent(unsigned int nEventID){};
	
private:
	bool				m_bWantToStop;
	KCritSec			m_secTimerEvent;
	TIMER_EVENT_MAP		m_mapTimerEvent;
    unsigned long       m_nCurTimeStamp;
	virtual void ThreadProcMain(void);
	bool ProcessTimerEvent(void);
};
