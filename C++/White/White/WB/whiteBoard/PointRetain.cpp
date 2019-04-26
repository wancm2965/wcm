#include "StdAfx.h"
#include "PointRetain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
PointRetain::PointRetain(void)
{
	PointRetainX = 0;
	PointRetainY = 0;
}

PointRetain::PointRetain( double x,double y )
{
	PointRetainX = x;
	PointRetainY = y;
}


PointRetain::~PointRetain(void)
{
}
