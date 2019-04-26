#include "StdAfx.h"
#include "RecognitionResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
RecognitionResult::RecognitionResult(string name,double score)
{
	this->name = name;
	this->score = score;
}


RecognitionResult::~RecognitionResult(void)
{
}
