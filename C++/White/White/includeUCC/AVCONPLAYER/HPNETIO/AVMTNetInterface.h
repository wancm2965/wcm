// AVMTNetInterface.h: interface for the AVMTNetInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVMTNETINTERFACE_H__
#define __AVMTNETINTERFACE_H__

#include "AVMTSocket.h"
#include <map>
class AVMTNetInterface;

typedef std::map<void*,AVMTNetInterface*>	MapAVMTNetInterface;

class AFX_EXT_CLASS AVMTNetInterface  
{
public:
	AVMTNetInterface(const char*cszDottedDecimal);
	virtual ~AVMTNetInterface(void);
public:
	const char*GetDottedDecimal(void);
	static void Interfaces(MapAVMTNetInterface&rMapAVMTNetInterface);
protected:
	char		m_szDottedDecimal[128];
};

#endif 