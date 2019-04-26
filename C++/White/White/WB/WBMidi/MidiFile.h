#pragma once
#include "stdafx.h"
#include "MidiDef.h"
#include "MidiTrack.h"
#include <vector>
#include <math.h>
#include "MidiNote.h"
#include <fstream>
#include "TimeSignature.h"
#include "Mididata.h"
class CMidiFile;
class CMidiFileReader;
class CMidiTrack;
class CTimeSignature;
class CMidiNote;


typedef std::vector<CMidiTrack*> MidiTrackVector;
class CMidiFile
{
	friend class CMidiFileReader;
public:
	CMidiFile(CString filepathname);
	~CMidiFile(void);


private:
	int EventNoteOff;
	int EventNoteOn;
	int EventKeyPressure;
	int EventControlChange;
	int EventProgramChange;
	int EventChannelPressure;
	int EventPitchBend;
	int SysexEvent1;
	int SysexEvent2;
	int MetaEvent;
	int MetaEventSequence;
	int MetaEventText;
	int MetaEventCopyright;
	int MetaEventSequenceName;
	int MetaEventInstrument;
	int MetaEventLyric;
	int MetaEventMarker;
	int MetaEventEndOfTrack;
	int MetaEventTempo;
	int MetaEventSMPTEOffset;
	int MetaEventTmeSignature;
	int MetaEventKeySignature;
	CString filename;
	MidiTrackVector m_tracks;
	CTimeSignature* pteimsig;
	int tempo;
	int quarternote;
	
	std::vector<CString> instruments;
public:
	MidiTrackVector GetTracks();
	CTimeSignature* GetTimeSignature(){return pteimsig;}
	CString GetFilename(){return filename;}
	CString EventName(int ev);



	CMidiTrack* ReadTrack(CMidiFileReader* pread,int tracknum);
	int ReadMetaEvent(CMidiFileReader* pread,CMidiTrack* midtrack,int starttime);

	MidiTrackVector SPlitChannels(CMidiTrack* miditrack);

	static void TimeShift(MidiTrackVector tracks,int amount);
	static void KeyShift(MidiTrackVector tracks,int amount);
	static int FindHighestNote(vectoreMidinote notes,int starttime,int endtiem);
	static int FindLowestNote(vectoreMidinote notes,int starttime,int endtiem);
	static MidiTrackVector SplitTrack(CMidiTrack * track);
	CMidiTrack* CloneTrack(int tracknum);
	static CMidiTrack* CombineToSingleTrack(MidiTrackVector thetracks);
	static MidiTrackVector CombineToTwoTracks(MidiTrackVector tracks);
	static void CheckStartTimes(MidiTrackVector tracks);
	static void RoundStartTimes(MidiTrackVector tracks,CTimeSignature* tiem);
	static void RoundDurations(MidiTrackVector tracks,int quarernote);
	static MidiTrackVector SplitChannels(CMidiTrack* track);
	std::vector<int> GuessMeasureLength();
};


class CMidiFileReader
{
public:
	CMidiFileReader(CString pathname);
	~CMidiFileReader();
private:
	bool did_peek;
	byte* buf;
	CFile file;
	int file_offset;
public:
	byte Peek();
	void DoRead(int len);
	byte ReadByte();
	short Readshort();
	int ReadInt();
	bool ReadAscii(int len,char* p);
	int ReadVarlen();
	void Skip(int amount);
	int GetOffset(){return file_offset;}
	void CloseFile();
};



