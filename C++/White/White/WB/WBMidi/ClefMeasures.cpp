#include "StdAfx.h"
#include "ClefMeasures.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CClefMeasures::CClefMeasures( vectoreMidinote notes,int measurelen )
{
	measure = measurelen;
	Clef clef = MainClef(notes);
	int num = measurelen;
	int i = 0 ;
	Clef item = clef;
	while(i < (int)notes.size())
	{
		int num2 = 0;
		int num3 = 0;
		while(i < (int)notes.size() && ((CMidiNote*)notes[i])->GetStarttime() < num)
		{
			num2 += ((CMidiNote*)notes[i])->GetNumber();
			num3++;
			i++;
		}
		if (num3 == 0)
		{
			num3 = 1;
		}
		int num4 = num2/num3;
		if (num4 != 0)
		{
			if (num4 >= 8)
			{
				item = Treble;
			}
			else
			{
				if (num4<= 10)
				{
					item = Bass;
				}
				else
				{
					item = clef;
				}
			}
		}
		clefs.push_back(item);
		num += measurelen;
	}
	clefs.push_back(item);
}

CClefMeasures::~CClefMeasures( void )
{
	if (!clefs.empty())
	{
		clefs.clear();
	}
}

Clef CClefMeasures::getClef( int starttime )
{
	return clefs[starttime/measure];
}

Clef CClefMeasures::MainClef( vectoreMidinote notes )
{
	CWhiteNote*MiddleC = new CWhiteNote(2,4);
	int num = MiddleC->Number();
	int num2 = 0;
	for (int i = 0;i<(int)notes.size();i++)
	{
		CMidiNote* current = notes[i];
		num2 += current->GetNumber();

	}
	Clef result;
	if (notes.empty())
	{
		result = Treble;
	}
	else
	{
		if (num2/(int)notes.size() >= num)
		{
			result = Treble;
		}
		else
		{
			result = Bass;
		}
	}
	delete MiddleC;
	MiddleC = NULL;
	return result;
}


