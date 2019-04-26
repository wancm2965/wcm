#pragma once
#include "stdafx.h"
#include "Mididata.h"
#include "MidiDef.h"
class CClefMeasures
{
public:
	CClefMeasures(vectoreMidinote notes,int measurelen);
	~CClefMeasures(void);
	Clef getClef(int starttime);
	static Clef MainClef(vectoreMidinote notes);
private:
	vectorclef clefs;
	int measure;

};



