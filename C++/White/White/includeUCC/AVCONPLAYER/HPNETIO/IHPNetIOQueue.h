//IHPNetIOQueue.h
#ifndef __IHPNETIOQUEUE_H__
#define __IHPNETIOQUEUE_H__

#include "AVMTSocket.h"
#include "IHPEventsPump.h"

#define HPNETIOEVENT_READ		0x00000001
#define HPNETIOEVENT_WRITE		0x00000002

class AFX_EXT_CLASS IHPNetIOQueue;
class AFX_EXT_CLASS IHPNetIOEvent
{
public:
	IHPNetIOEvent(void);
	virtual~IHPNetIOEvent(void);
public:
	virtual void AddEvent(void);
	virtual void RemoveEvent(void);
	virtual void ModifyEvent(void);
	virtual void EnableRead(void);
	virtual void DisableRead(void);
	virtual void EnableWrite(void);
	virtual void DisableWrite(void);
	virtual unsigned long GetEvents(void);
	virtual operator SOCKET(void)=0;
	virtual void OnRead(void)=0;
	virtual void OnWrite(void)=0;
	virtual void OnError(void)=0;
protected:
	IHPNetIOQueue&	m_rIHPNetIOQueue;
	unsigned long	m_ulEvents;
};

class AFX_EXT_CLASS IHPNetIOQueue
	: public IHPEventsQueue
{
public:
	IHPNetIOQueue(void){};
	virtual~IHPNetIOQueue(void){};
public:
	static void CreateInstance(void);
	static void DestroyInstance(void);
	static IHPNetIOQueue&Instance(void);
public:
	virtual void ReleaseConnections(void)=0;
	virtual void AddEvent(IHPNetIOEvent*pIHPNetIOEvent)=0;
	virtual void RemoveEvent(IHPNetIOEvent*pIHPNetIOEvent)=0;
	virtual void ModifyEvent(IHPNetIOEvent*pIHPNetIOEvent)=0;
	virtual bool PumpEvents(void)=0;
};

//AFX_EXT_CLASS IHPNetIOQueue*CreateIHPNetIOQueue(void);


#endif