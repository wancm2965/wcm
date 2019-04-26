#pragma once
#include "MidiFile.h"
#include "SheetMusic.h"
#include "SheetMusicOptions.h"


class CSheetMusicOptions;
class CMidiFile;
class CSheetMusic;
class CWBMidiConvert
{
public:
	CWBMidiConvert(void);
	~CWBMidiConvert(void);
	static CWBMidiConvert* GetIns();
	static void CreateIns();
	static void DestroyIns();
	void RedrawSheetmusic();
	void OpenMidiFile(CString filename,int& heiht);
	void Draw(CDC* hc);
	void SetZoom(float nvalue);
	CMidiFile* midifile;
	CSheetMusic* sheetmusic;
	CSheetMusicOptions* sheetmusicoption;
	static CWBMidiConvert* pWBMidiconvert;

};

