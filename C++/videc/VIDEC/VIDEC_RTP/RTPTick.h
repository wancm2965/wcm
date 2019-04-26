//RTPTick.h
#ifndef __RTPTICK_H__
#define __RTPTICK_H__

#include <map>
#include <NETEC/XThreadBase.h>
#include <NETEC/XAutoLock.h>

class RTPTick
{
public:
	RTPTick(void);
	virtual ~RTPTick(void);
public:
	virtual int Open(void);
	virtual void Close(void);

	virtual bool DoTick(void)=0;
};

#endif