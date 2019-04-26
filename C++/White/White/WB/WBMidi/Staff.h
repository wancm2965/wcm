#pragma once
#include "stdafx.h"
#include "MidiDef.h"
#include<vector>
#include "Mididata.h"
#include "KeySignature.h"

class CMusicSymbol;
class CKeySignature;

class CStaff
{

private:
	
	std::vector<CMusicSymbol*> symbols;
	int ytop;
	//Pen pen;
	CClefSymbol* clefsym;
	std::vector<CAccidSymbol*> keys;
	int height;
	int tracknum;
	int totaltracks;	
public:
	int GetHeight(){return height;}

	CStaff(std::vector<CMusicSymbol*> symbol,CKeySignature* key,int ntrak,int totltrack);
	~CStaff(void);
	Clef FindClef(std::vector<CMusicSymbol*> symbol);
	void CalculateHeight();
	void DrawHorizLines(Graphics* pDC);
	void DrawEndLines(Graphics* pDC);
	void Draw(Graphics* gr);
};


