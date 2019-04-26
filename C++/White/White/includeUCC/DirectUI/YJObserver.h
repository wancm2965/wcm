#ifndef	YJOBSERVER_H
#define YJOBSERVER_H

#include "fileex.h"

#include <deque>

class CObjectBase;

class DIRECTUI_API YJObserver
{
public:
	virtual void Notify( CObjectBase* pObj, int nEvent) = 0;
};

#endif //YJOBSERVER_H


