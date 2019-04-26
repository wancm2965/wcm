#ifndef __IHPEVENTSPUMP_H__
#define __IHPEVENTSPUMP_H__

//#include "AVAutoLock.h"

class AFX_EXT_CLASS IHPEventsQueue
{
public:
	IHPEventsQueue(void){};
	virtual~IHPEventsQueue(void){};
public:
	virtual bool PumpEvents(void)=0;
};

class AFX_EXT_CLASS IHPEventsPump
{
public:
	IHPEventsPump(void){};
	virtual ~IHPEventsPump(void){};
public:
	static void CreateInstance(void);
	static void DestroyInstance(void);
	static IHPEventsPump&Instance(void);
public:
	virtual void ReleaseConnections(void)=0;
	virtual void AddIHPEventsQueue(IHPEventsQueue*pIHPEventsQueue)=0;
	virtual void RemoveIHPEventsQueue(IHPEventsQueue*pIHPEventsQueue)=0;
	virtual bool PumpEvents(void)=0;
	//virtual AVCritSec&GetAVCritSec(void)=0;
};

#endif