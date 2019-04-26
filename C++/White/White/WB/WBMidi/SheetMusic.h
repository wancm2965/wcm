#pragma once
#include "MidiFile.h"
#include "SheetMusicOptions.h"
#include "Staff.h"
#include "MidiDef.h"
#include "Mididata.h"
#include "ClefMeasures.h"
#include "SymbolWidths.h"
#include "TimeSignature.h"



class CTimeSignature;
class CSymbolWidths;
class CClefMeasures;
class CRestSymbol;
class ChordSymbol;
class CStaff;
class CSheetMusicOptions;
class CMidiFile;
class CSheetMusic
{
public:
	CSheetMusic(CMidiFile* file,CSheetMusicOptions* options);
	virtual ~CSheetMusic();
public:
	int LineWidth ;
	int LeftMargin;
	int PageWidth;
	int PageHeight ;
	//static COLORREF clolr;
	static int LineSpace ;
	static int StaffHeight ;
	static int NoteHeight ;
	static int NoteWidth;
	std::vector<CStaff*> staffs;
	int numtracks;
	float zoom;
	CDC m_memDC;
	CBitmap m_bmpBuf;
	CKeySignature* keysignature;
	CSymbolWidths* widths;
public:
	int getNoteWidth(){return NoteWidth;}
	int GetNoteHeight(){return NoteHeight;}
	int GetLineSpace(){return LineSpace;}
	int GetStaffHeight(){return StaffHeight;}
	CKeySignature* GetKeySignature(MidiTrackVector tracks);

	vectorchordsymbol CreateChords(vectoreMidinote midinotes,CKeySignature* key,CTimeSignature* time,CClefMeasures* clefs);

	vecMusicSymbol CreateSymbol(vectorchordsymbol chords,CClefMeasures* clefs,CTimeSignature* time);

	vecMusicSymbol AddBars(vectorchordsymbol chords,CTimeSignature* time);
	vecMusicSymbol AddRests(vecMusicSymbol chords,CTimeSignature* time);

	std::vector<CRestSymbol*> GetRests(CTimeSignature* time,int start,int end);

	vecMusicSymbol AddClefChanges(vecMusicSymbol symbols,CClefMeasures* clefs,CTimeSignature* time);


private:
	void AlignSymbols(vecvecMusicSymbol& allsymbols,CSymbolWidths* widths);
	static bool IsChord(CMusicSymbol* m);

	static void MakeChordTriplets(vecvecMusicSymbol& allsymbols,CTimeSignature* time,CSymbolWidths* widths);
	static void MakeChordPairs(vecvecMusicSymbol& allsymbols,CTimeSignature* time,CSymbolWidths* widths);
	int KeySignatureWidth(CKeySignature* key);

	std::vector<CStaff*> CreateStaffs(vecvecMusicSymbol allsymbols,CKeySignature* key,int measure);
	static void SetNoteSize(bool largenotes);
public:
	void SetZoom(float nvalue);
	void DrawView(CDC* hc);
	int  GetHeight();
public:
	static CSheetMusic* Getsheetmusic();

	static CSheetMusic* gsheetmusic;
};