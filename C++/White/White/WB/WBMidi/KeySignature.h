#pragma once
#include <vector>
#include "stdafx.h"
#include "MidiDef.h"
#include "Mididata.h"
#include "WhiteNote.h"

class CWhiteNote;
class CAccidSymbol;
using namespace std;
typedef std::vector<int> VecIntwidth;

class CKeySignature
{
public:
	CKeySignature(int sharps,int flats);
	~CKeySignature(void);

	static void CreateAccidentalMaps();
	void ResetKeyMap();
	void CreateSymbol();
	std::vector<CAccidSymbol*> GetSymbol(Clef clef);

	Accid GetAccidental(int notenumber,int measure);

	CWhiteNote* GetWhiteNote(int notenumber);
	static CKeySignature*  Guess(VecIntwidth notes);
	static void DestroyKeySig();
	bool Equals(CKeySignature* k);
	int Notescale();
	static CString KeyToString(int notescale);
private:
	int C ;
	int G ;
	int D ;
	int A ;
	int E ;
	int B ;
	int F ;
	int Bflat;
	int Eflat ;
	int Aflat;
	int Dflat;
	int Gflat ;
	int num_flats;
	int num_sharps;
public:
	
	static Accid **sharpkeys;
	static Accid **flatkeys;

	std::vector<CAccidSymbol*> m_treble;
	std::vector<CAccidSymbol*> m_bass;
	Accid m_keymap[129];
	int prevmeasure;


};



