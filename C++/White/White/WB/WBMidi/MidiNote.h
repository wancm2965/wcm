#pragma once
#include "stdafx.h"
#include "MidiDef.h"
class CMidiNote
{
public:
	CMidiNote(int start = 0,int chan = 0,int noten = 0,int dur = 0);

	~CMidiNote();
private:
	int starttime;
	int channel;
	int notenumber;
	int duration;
public:
	int GetStarttime(){return starttime;}
	void SetStarttime(int nvalue){starttime = nvalue;}
	int GetEndTime(){return starttime + duration;}
	int GetChannel(){return channel;}
	int GetNumber(){return notenumber;}
	void SetNumber(int nvalue){notenumber = nvalue;}
	int GetDuration(){return duration;}
	void SetDuration(int nvalue){duration = nvalue;}
	void NoteOff(int endtime);
	void Clone(CMidiNote* other);


};



