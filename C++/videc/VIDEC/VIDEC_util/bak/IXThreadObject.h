#ifndef __IXTHREADOBJECT_H__
#define __IXTHREADOBJECT_H__

#include "XAutoLock.h"

class IXThreadObject
{
public:
	IXThreadObject(void){};
	virtual ~IXThreadObject(void){};
public:
	virtual bool Start(void)=0;
	virtual void Stop(void)=0;
	virtual bool IsPending(void)=0;
	virtual bool OnRunOnce(void)=0;
};

unsigned long XTOInitialize(void);
void XTOUnintialize(void);

#endif