#pragma once


#include <map>
#include <vector>
#include "Mididata.h"
#include "stdafx.h"
#include "MidiDef.h"

class CSymbolWidths
{
public:
	CSymbolWidths(vecvecMusicSymbol tracks);
	~CSymbolWidths(void);


private:
	MapIntWidth maxwidths;
	VecMapIntWidth widths;
	VecIntwidth starttimes;

public:
	VecIntwidth GetSTarttimes();
	MapIntWidth GetTrackWidths(vecMusicSymbol symbols);
	int GetEctraWidth(int track,int start);
	int GetWidthBetween(int start,int end);
};




