#pragma once
#include <string>
#include "PointRetain.h"
using namespace std;
typedef vector<PointRetain>  PointPath;
typedef PointPath::iterator PointPathIterator;
class MatchTemplate
{
public:
	MatchTemplate();
	MatchTemplate(string name,PointPath pointpath);
	~MatchTemplate(void);
	string Matchname;
	
	PointPath ptpath;
};


