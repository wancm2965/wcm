#pragma once

#include "KMMSDefine.h"

typedef std::map<std::string, MMS_ROOM_LST> MAP_MMS_ROOM;
typedef MAP_MMS_ROOM::iterator MAP_MMS_ROOM_IT;

typedef struct _DOMAININFO_EX : public DOMAININFO
{
	int	nLevel;

	_DOMAININFO_EX()
	{
		nLevel	= 0;
	}
}DOMAININFO_EX,*PDOMAININFO_EX;

typedef std::list<PDOMAININFO_EX> DOMAININFOLIST_EX;
