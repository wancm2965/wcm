#pragma once
#include "PointRetain.h"
using namespace std;
typedef vector<PointRetain>  PointPath;
typedef PointPath::iterator PointPathIterator;
class TemplateLibrary
{
public:
	TemplateLibrary(void);
	~TemplateLibrary(void);
	PointPath GetTemplateCircle();
	PointPath GetTemplateRectangle();
	PointPath GetTemplateTriangle();
	PointPath GetTemplateStart();
};

