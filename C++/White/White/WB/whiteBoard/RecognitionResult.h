#pragma once
#include <string>
using namespace std;
class RecognitionResult
{
public:
	RecognitionResult(string name,double score);
	~RecognitionResult(void);
	string name;
	double score;
};

