#pragma once
#include "stdafx.h"
#include "MidiDef.h"
class CTimeSignature
{
public:
	CTimeSignature(int num,int den,int qua,int tem);
	~CTimeSignature(void);


public:
	int GetNumerator(){return numerator;}
	int GetDenomunator(){return denominator;}
	int GetQuarter(){return quarternote;}
	int GetMeasure(){return measure;}
	int GetTempo(){return tempo;}
	int GetMeasure(int time){return time/measure;}
	NoteDuration GetNoteDuration(int duration);
	NoteDuration GetStemDuration(NoteDuration dur);
	int DurationToTime(NoteDuration dur);
private:
	int numerator;
	int denominator;
	int quarternote;
	int measure;
	int tempo;

};



