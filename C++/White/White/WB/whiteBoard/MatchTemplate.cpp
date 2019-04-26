#include "StdAfx.h"
#include "MatchTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
MatchTemplate::MatchTemplate(string name,PointPath pointpath)
{
	Matchname = name;
	ptpath = pointpath;
}

MatchTemplate::MatchTemplate()
{
	Matchname = "";
	
}


MatchTemplate::~MatchTemplate(void)
{
}
