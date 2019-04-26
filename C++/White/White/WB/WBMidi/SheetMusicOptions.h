#pragma once
#include "Mididata.h"
#include "MidiDef.h"
#include "TimeSignature.h"
class CTimeSignature;

class CSheetMusicOptions
{
public:
	CSheetMusicOptions();
	~CSheetMusicOptions(void);
	std::vector<bool> Gettrack(){return tracks;}
	bool Gettwostaffs(){return two_staffs;}
	bool GetLargnotesize(){return larg_note_size;}
	int GetTimeshift(){return timeshift;}
	int GetKeyShift(){return shiftkey;}
	CTimeSignature* GetTimesignature(){return ptime;}
	void Settrack(std::vector<bool> track){tracks = track;}
	void Settwostaffs(bool btwo){two_staffs = btwo;}
	void Setlargnotesize(bool blarg){larg_note_size = blarg;}
	void Settimeshift(int nvalue){timeshift = nvalue;}
	void Setshiftkey(int nvalue){shiftkey = nvalue;}
	void SetTimeSignature(CTimeSignature* time){ptime = time;}
private:
	
	std::vector<bool> tracks;
	bool larg_note_size;
	bool two_staffs;
	int timeshift;
	int shiftkey;
	CTimeSignature* ptime;
};



