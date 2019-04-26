#pragma once
#include "stdafx.h"
#include "Mididata.h"
#include "MidiDef.h"

class CWhiteNote
{
public:
	
	CWhiteNote(int letter = 0,int ocave = 0);
	~CWhiteNote(void);
	int GetLetter(){return mletter;}
	int GetOctave(){return moctave;}
	void SetLetter(int nletter){mletter = nletter;}
	void SetOctave(int noctave){moctave = noctave;}
	int Dist(CWhiteNote* w);
	CWhiteNote* Add(int amount);
	int Number();

	bool operator<(const CWhiteNote* m);

private:
	int mletter;
	int moctave;
public:
	static CWhiteNote* MaxWhiteNote(CWhiteNote* x,CWhiteNote* y);
	static CWhiteNote* MinWhiteNote(CWhiteNote* x,CWhiteNote* y);
	static CWhiteNote* TopWhiteNote(Clef clef);
	static CWhiteNote* BottomWhiteNote(Clef clef);
	
};





	





