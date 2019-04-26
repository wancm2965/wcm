#pragma once
#include <vector>
#include <atlimage.h>
#include "stdafx.h"
#include "MidiDef.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include "WhiteNote.h"
#include "MidiNote.h"
#include "TimeSignature.h"
#include "KeySignature.h"
#include "Stem.h"
#include <map>

using namespace std;


class CKeySignature;
class CWhiteNote;
class CMusicSymbol;
class CAccidSymbol;
class CBarSymbol;
class CBlankSymbol;
class ChordSymbol;
class CMidiNote;
class CStem;
typedef std::vector<CMidiNote*> vectoreMidinote;
class CTimeSignature;
class CClefSymbol;
class CRestSymbol;

struct NoteData
{
public:
	int number;
	CWhiteNote *whitenote;
	NoteDuration duration;
	bool bleftside;
	Accid accid;

};





class CMusicSymbol : public CObject
{
public:
	CMusicSymbol();
	virtual ~CMusicSymbol();
	virtual int GetStartTime() ;
	virtual int GetMinWidth() ;
	virtual int GetWidth() ;
	virtual void SetWidth(int nvalue) ;
	virtual int GetAboveStaff() ;
	virtual int GetBelowStaff() ;
	virtual void Draw(Graphics* pDC,int ytop) ;

	void SetType(SymbolType type){m_nsymboltype = type;}
	SymbolType GetType(){return m_nsymboltype;}
private:
	int mstarttime;
	int minwidth;
	int width;
	int abovestaff;
	int belowstaff;
	SymbolType m_nsymboltype;
};

class CAccidSymbol : public CMusicSymbol
{

public:
	CAccidSymbol(Accid accid, CWhiteNote* note, Clef clef);
	~CAccidSymbol(void);

	CWhiteNote* GetWhitenote(){return m_whitenote;}
	//CMusicSymbol
	virtual int GetStartTime() {return -1;}
	virtual int GetMinWidth(); 
	virtual int GetWidth() {return m_nwidth;};
	virtual void SetWidth(int nvalue) {m_nwidth = nvalue;};
	virtual int GetAboveStaff();
	virtual int GetBelowStaff() ;


	virtual void Draw(Graphics* pDC,int ytop);
	void DrawSharp(Graphics* pDC,int ytop);
	void DrawFlat(Graphics* pDC,int ytop);
	void DrawNatural(Graphics* pDC,int ytop);
	int Abovestaff();
	int Belowstaff();
private:
	Accid m_accid;
	Clef m_clef;
	int m_nwidth;
	CWhiteNote* m_whitenote;
};

class CBarSymbol :public CMusicSymbol
{
public:
	CBarSymbol(int starttime);
	~CBarSymbol(void);

	virtual int GetStartTime(){return m_nstarttime;}
	virtual int GetMinWidth();
	virtual int GetWidth(){return m_nwidth;} ;
	virtual void SetWidth(int nvalue){m_nwidth = nvalue;} ;
	virtual int GetAboveStaff() {return 0;};
	virtual int GetBelowStaff() {return 0;};
	virtual void Draw(Graphics* pDC,int ytop) ;
private:
	int m_nstarttime;
	int m_nwidth;
};

class CBlankSymbol :public CMusicSymbol
{
public:
	CBlankSymbol(int starttime,int width);
	~CBlankSymbol(void);

	virtual int GetStartTime(){return m_nstarttime;};
	virtual int GetMinWidth() {return 0;};
	virtual int GetWidth(){return m_nwidth;};
	virtual void SetWidth(int nvalue) {m_nwidth = nvalue;};
	virtual int GetAboveStaff() {return 0;};
	virtual int GetBelowStaff() {return 0;};
	virtual void Draw(Graphics* pDC,int ytop) {};
private:
	int m_nstarttime;
	int m_nwidth;
};

class ChordSymbol :	public CMusicSymbol
{
public:
	ChordSymbol(vectoreMidinote midinotes,CKeySignature* key,CTimeSignature* time,Clef c);
	~ChordSymbol(void);
private:
	Clef m_clef;
	int m_nstarttime;
	int m_nendtime;
	std::vector<NoteData*> m_vectorNoteData;
	std::vector<CAccidSymbol*> m_vectorAccidSymbol;
	int m_nwidth;
	CStem* stem1;
	CStem* stem2;
	bool hastwostems;
public:
	virtual int GetStartTime() {return m_nstarttime;};
	virtual int GetMinWidth() ;
	virtual int GetWidth(){return m_nwidth;} ;
	virtual void SetWidth(int nvalue) {m_nwidth = nvalue;};
	virtual int GetAboveStaff() ;
	virtual int GetBelowStaff() ;
	virtual void Draw(Graphics* pDC,int ytop) ;
	int DrawAccid(Graphics* pDC,int ytop);
	void DrawNotes(Graphics* pDC,int ytop,CWhiteNote* staff);
	int GetEndTime(){return m_nendtime;}
	Clef Getclef(){return m_clef;}
	bool GetHastwostems(){return hastwostems;}
	CStem* GetStem();

	static std::vector<NoteData*> CreateNoteData(vectoreMidinote midinotes,CKeySignature* key,CTimeSignature* time);
	static std::vector<CAccidSymbol*> CreateAccidSymbol(std::vector<NoteData*> notedata,Clef clef);
	//int StemDirection(CWhiteNote* bottom,CWhiteNote* top,Clef clef);
	bool NoteOverlap(std::vector<NoteData*> notedata,int start,int end);
	int minwidth();
	int NotesGetWidth();
	static void MakeTriplet(ChordSymbol* c1,ChordSymbol* c2,ChordSymbol* c3,int spacing);
	static int StemDirection(CWhiteNote* bottom,CWhiteNote* top,Clef clef);
	static bool CanMakeTriplet(ChordSymbol* c1,ChordSymbol* c2,ChordSymbol* c3,CTimeSignature* time);
	static bool CanMakePair(ChordSymbol* c1,ChordSymbol* c2,CTimeSignature* time);
	static void MakePair(ChordSymbol* c1,ChordSymbol* c2,int spacing);
};

class CClefSymbol :	public CMusicSymbol
{
public:
	CClefSymbol(Clef clef,int sartt,bool bsmall);
	~CClefSymbol(void);
private:
	Bitmap* m_treble;
	Bitmap* m_bass;
	int starttime;
	bool smallsize;
	Clef mclef;
	int width;
public:
	virtual int GetStartTime() {return starttime;};
	virtual int GetMinWidth() ;
	virtual int GetWidth(){return width;} ;
	virtual void SetWidth(int nvalue) {width = nvalue;};
	virtual int GetAboveStaff() ;
	virtual int GetBelowStaff() ;
	virtual void Draw(Graphics* pDC,int ytop) ;
	void LoadImages();
};

class CRestSymbol :	public CMusicSymbol
{
public:
	CRestSymbol(int start,NoteDuration dur);
	~CRestSymbol(void);

	virtual int GetStartTime() {return starttime;};
	virtual int GetMinWidth() ;
	virtual int GetWidth() {return width;}
	virtual void SetWidth(int nvalue) ;
	virtual int GetAboveStaff() ;
	virtual int GetBelowStaff() ;
	virtual void Draw(Graphics* pDC,int ytop) ;

	void DrawWhole(Graphics* pDC,int ytop);
	void DrawHalf(Graphics* pDC,int ytop);
	void DrawQuarter(Graphics* pDC,int ytop);
	void DrawEighth(Graphics* pDC,int ytop);
private:
	int starttime;
	NoteDuration duration;
	int width;

};








typedef std::map<int,int>MapIntWidth;
typedef MapIntWidth::iterator IT_MapIntWidth;
typedef std::vector<MapIntWidth> VecMapIntWidth;
typedef std::vector<int> VecIntwidth;
typedef std::vector<CMusicSymbol*> vecMusicSymbol;
typedef std::vector<vecMusicSymbol> vecvecMusicSymbol;
typedef std::vector<Clef> vectorclef;
typedef std::vector<ChordSymbol*> vectorchordsymbol;


