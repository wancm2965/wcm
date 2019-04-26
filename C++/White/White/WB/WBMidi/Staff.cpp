#include "StdAfx.h"
#include "Staff.h"
#include "KeySignature.h"
#include "SheetMusic.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CStaff::CStaff(std::vector<CMusicSymbol*> symbol ,CKeySignature* key,int ntrak,int totltrack )
{
	tracknum = ntrak;
	totaltracks = totltrack;
	Clef clef = FindClef(symbol);
	clefsym = new CClefSymbol(clef,0,false);
	//m_keysignature = key;
	keys = key->GetSymbol(clef);
	symbols = symbol;
	
	CalculateHeight();
}

Clef CStaff::FindClef(std::vector<CMusicSymbol*> symbol )
{
	Clef result;
	for (int i = 0;i<symbol.size();i++)
	{
		CMusicSymbol* music = symbol[i];
		if (music->GetType() == CHORDSYMBOL)
		{
			result = ((ChordSymbol*)music)->Getclef();
			return result;
		}
	}
	result = Treble;
	return result;
}

void CStaff::CalculateHeight()
{
	int num = 0;
	int num2 = 0;
	for (int i = 0;i<symbols.size();i++)
	{
		CMusicSymbol* current = symbols[i];
		if (num < current->GetAboveStaff())
		{
			num = current->GetAboveStaff();

		}
		if (num2 < current->GetBelowStaff())
		{
			num2 = current->GetBelowStaff();
		}
	}

	if (num < clefsym->GetAboveStaff())
	{
		num = clefsym->GetAboveStaff();
	}
	if (num2 < clefsym->GetBelowStaff())
	{
		num2 = clefsym->GetBelowStaff();
	}
	ytop = num +4;
	height = CSheetMusic::Getsheetmusic()->GetNoteHeight()*4 + ytop + num2;
	if (tracknum == totaltracks -1)
	{
		height += 4;
	}
}
	
void CStaff::Draw( Graphics* gr)
{
	int num = 8;
	gr->TranslateTransform(num,0);
	Pen drawpen(Color(0,0,0),(Gdiplus::REAL)1);
	clefsym->Draw(gr,ytop);
	gr->TranslateTransform(-num,0);
	num += clefsym->GetWidth();
	std::vector<CAccidSymbol*> array1 = keys;
	for (int i = 0;i< (int)keys.size();i++)
	{
		CAccidSymbol* accidsymbol = array1[i];
		gr->TranslateTransform(num,0);
		accidsymbol->Draw(gr,ytop);
		gr->TranslateTransform(-num,0);
		num += accidsymbol->GetWidth();
	}
	for(int j = 0;j<(int)symbols.size();j++)
	{
		CMusicSymbol* current = symbols[j];
		gr->TranslateTransform(num,0);
		current->Draw(gr,ytop);
		gr->TranslateTransform(-num,0);
		num += current->GetWidth();
	}
	DrawHorizLines(gr);
	DrawEndLines(gr);

}

void CStaff::DrawHorizLines( Graphics* pDC )
{
	int num = ytop - 1;
	Pen drawpen(Color(0,0,0),(Gdiplus::REAL)1);
	for (int i = 0;i<5;i++)
	{
		pDC->DrawLine(&drawpen,4,num,799,num);
		num += CSheetMusic::Getsheetmusic()->GetLineSpace() +1;
	}
}

void CStaff::DrawEndLines( Graphics* pDC )
{
	Pen drawpen(Color(0,0,0),(Gdiplus::REAL)1);
	int y = 0;
	if (tracknum == 0)
	{
		y = ytop -1;
	}
	else
	{
		y = 0;
	}
	int y2 ;
	if (tracknum == totaltracks - 1)
	{
		y2 = ytop + 4*CSheetMusic::Getsheetmusic()->GetNoteHeight();
	}
	else
	{
		y2 = height;
	}
	pDC->DrawLine(&drawpen,4,y,4,y2);
	pDC->DrawLine(&drawpen,799,y,799,y2);
}

CStaff::~CStaff( void )
{
	if (!symbols.empty())
	{
		for (int i = 0;i<symbols.size();i++ )
		{
			delete symbols[i];
			symbols[i] = NULL;
		}
		symbols.clear();
	}

	if (clefsym != NULL)
	{
		delete clefsym;
		clefsym = NULL;
	}

	if (!keys.empty())
	{
		keys.clear();
	}

}
