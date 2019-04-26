#pragma once
#include <vector>
#include "stdafx.h"
#include "MidiDef.h"
#include "Mididata.h"

class CMidiTrack
{

public:
	CMidiTrack(int track = 0);
	~CMidiTrack(void);
private:
	int tracknum;
	vectoreMidinote m_notes;
	CString instrument;

public:
	void Setmidivector(vectoreMidinote vecnote){m_notes = vecnote;}
	vectoreMidinote GetMidinoteVector();
	void Settracknum(int tra){tracknum = tra;}
	CString GetInstrument();
	void SetInstrument(CString str){instrument = str;}
	void AddNote(CMidiNote* m);
	void NoteOff(int chan,int notenum,int edtime);
	void Clone(CMidiTrack* other);
	bool HasMultipleChannels();
	int GetVectorLength(){return m_notes.size();}
};

	



