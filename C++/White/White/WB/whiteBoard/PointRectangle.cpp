#include "StdAfx.h"
#include "PointRectangle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
PointRectangle::PointRectangle(void)
{
	RectangleX = 0.00;
	RectangleY = 0.00;
	RectangleWidth = 0.00;
	RectangleHeight = 0.00;
}

PointRectangle::PointRectangle( double x,double y,double width,double height )
{
	RectangleX = x;
	RectangleY = y;
	RectangleWidth = width;
	RectangleHeight = height;
}


PointRectangle::~PointRectangle(void)
{
}
