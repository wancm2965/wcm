#include "StdAfx.h"
#include "Mididata.h"
#include "SheetMusic.h"
#include <algorithm>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CMusicSymbol::CMusicSymbol()
{
	mstarttime =0;
	minwidth = 0;
	width = 0;
	abovestaff = 0;
	belowstaff =0;
	m_nsymboltype = ACCIDSYMBOL;
}

CMusicSymbol::~CMusicSymbol()
{

}

int CMusicSymbol::GetStartTime()
{
	return mstarttime;
}

int CMusicSymbol::GetMinWidth()
{
	return minwidth;
}



int CMusicSymbol::GetWidth()
{
	return width;
}

void CMusicSymbol::SetWidth( int nvalue )
{
	width = nvalue;
}

int CMusicSymbol::GetAboveStaff()
{
	return abovestaff;
}

int CMusicSymbol::GetBelowStaff()
{
	return belowstaff;
}

void CMusicSymbol::Draw( Graphics* pDC,int ytop )
{
	return;
}


CAccidSymbol::CAccidSymbol(Accid accid, CWhiteNote* note, Clef clef )
{
	m_accid = accid;
	m_whitenote = note;
	m_clef = clef;
	m_nwidth = GetMinWidth();
	SetType(ACCIDSYMBOL);
}

CAccidSymbol::~CAccidSymbol( void )
{
	if (m_whitenote != NULL)
	{
		delete m_whitenote;
		m_whitenote = NULL;
	}
}

int CAccidSymbol::GetAboveStaff()
{
	return Abovestaff();
}

int CAccidSymbol::GetBelowStaff()
{
	return Belowstaff();
}

void CAccidSymbol::Draw( Graphics* pDC,int ytop )
{
	
	CWhiteNote* ptemp = CWhiteNote::TopWhiteNote(m_clef);
	int ynote = ytop + ptemp->Dist(m_whitenote)*CSheetMusic::Getsheetmusic()->GetNoteHeight()/2;
	if (m_accid == Sharp)
	{
		DrawSharp(pDC,ynote);
	}
	else
	{
		if (m_accid == Flat)
		{
			DrawFlat(pDC,ynote);
		}
		else
		{
			if (m_accid == Natural)
			{
				DrawNatural(pDC,ynote);
			}
		}
	}
	if (ptemp != NULL)
	{
		delete ptemp;
		ptemp = NULL;
	}
	

}

void CAccidSymbol::DrawSharp( Graphics* pDC,int ytop )
{
	int noteheight = CSheetMusic::Getsheetmusic()->GetNoteHeight();
	int linespace = CSheetMusic::Getsheetmusic()->GetLineSpace();
	int num = ytop - noteheight;
	int num2 = ytop+ 2*noteheight;
	int num3 = noteheight/2;

	Pen drawpen(Color(0,0,0),(Gdiplus::REAL)1);
	pDC->DrawLine(&drawpen,num3,num+2,num3,num2);
	num3 += noteheight/2;
	pDC->DrawLine(&drawpen,num3,num,num3,num2-2);
	int x = noteheight/4;
	int x2 = noteheight*5/4;
	num = ytop +1;
	num2 = num -1-linespace/4;
	Pen drawpen1(Color(0,0,0),(Gdiplus::REAL)(linespace/2));
	pDC->DrawLine(&drawpen1,x,num,x2,num2);
	num += linespace;
	num2 += linespace;
	pDC->DrawLine(&drawpen1,x,num,x2,num2);



}

void CAccidSymbol::DrawFlat( Graphics* pDC,int ytop )
{
	int noteheight = CSheetMusic::Getsheetmusic()->GetNoteHeight();
	int linespace = CSheetMusic::Getsheetmusic()->GetLineSpace();
	int num = linespace/4;
	Pen pendraw(Color(0,0,0),(Gdiplus::REAL)1);
	int x = linespace;
	pDC->DrawLine(&pendraw,num,ytop-noteheight*3/2,num,ytop+noteheight);
	pDC->DrawBezier(&pendraw,num,ytop + x/4,num+x/2,ytop-x/2,num+x,ytop+x/3,num,ytop+x+2);
	pDC->DrawBezier(&pendraw,num,ytop + x/4,num+x/2,ytop-x/2,num+x*5/4,ytop+x/12,num,ytop+x+2);
	pDC->DrawBezier(&pendraw,num,ytop + x/4,num+x/2,ytop-x/2,num+x*3/2,ytop-x/6,num,ytop+x+2);

}

void CAccidSymbol::DrawNatural( Graphics* pDC,int ytop )
{
	int x = CSheetMusic::Getsheetmusic()->GetLineSpace();
	int num = ytop - x-1;
	int num2 = ytop +x+1;
	int num3 = x/2;
	Pen pendraw(Color(0,0,0),(Gdiplus::REAL)1);
	pDC->DrawLine(&pendraw,num3,num,num3,num2);
	num3 += x*3/4;
	num = ytop - x/4;
	num2 = ytop +7*x/4+1;
	pDC->DrawLine(&pendraw,num3,num,num3,num2);
	int num4 = x/2;
	int y = num4+x*3/4;
	num = ytop +1;
	num2 = num -1-x/4;
	Pen pendraw1(Color(0,0,0),(Gdiplus::REAL)(x/2));
	pDC->DrawLine(&pendraw1,num4,num,x,num2);
	num += x;
	num2 += x;
	pDC->DrawLine(&pendraw1,num4,num,x,num2);

}

int CAccidSymbol::Abovestaff()
{
	int noteheight = CSheetMusic::Getsheetmusic()->GetNoteHeight();
	CWhiteNote* pTemp = CWhiteNote::TopWhiteNote(this->m_clef);
	int num = (pTemp->Dist(m_whitenote))*(noteheight/2);
	if (m_accid == Sharp || m_accid == Natural)
	{
		num -= noteheight;
	}
	else
	{
		if (m_accid == Flat)
		{
			num -= 3*(noteheight/2);
		}
	}
	int result;
	if (num < 0)
	{
		result = -num;
	}
	else
	{
		result = 0;
	}
	if (pTemp != NULL)
	{
		delete pTemp;
		pTemp = NULL;
	}
	return result;
}

int CAccidSymbol::Belowstaff()
{
	CWhiteNote* ptemp = CWhiteNote::BottomWhiteNote(m_clef);
	if (ptemp == NULL)
	{
		return 1;
	}
	int noteheight = CSheetMusic::Getsheetmusic()->GetNoteHeight();
	int num = (ptemp->Dist(m_whitenote))*(noteheight/2) + noteheight;
	if (m_accid == Sharp || m_accid == Natural)
	{
		num += noteheight;		
	}
	int result;
	if (num > 0)
	{
		result = num;
	}
	else
	{
		result = 0;
	}
	if (ptemp != NULL)
	{
		delete ptemp;
		ptemp = NULL;
	}
	return result;
}

int CAccidSymbol::GetMinWidth()
{
	return 3*CSheetMusic::Getsheetmusic()->GetNoteHeight()/2 ;
}



CBarSymbol::CBarSymbol( int starttime )
{
	m_nstarttime = starttime;
	m_nwidth = GetMinWidth();
	SetType(BARSYMBOL);
}

CBarSymbol::~CBarSymbol( void )
{

}

void CBarSymbol::Draw( Graphics* pDC,int ytop )
{
	int notewidth = CSheetMusic::Getsheetmusic()->getNoteWidth();
	int y = ytop + CSheetMusic::Getsheetmusic()->GetLineSpace()*4+4;
	Pen pendraw(Color(0,0,0),(Gdiplus::REAL)1);
	pDC->DrawLine(&pendraw,notewidth/2,ytop,notewidth/2,y);
}

int CBarSymbol::GetMinWidth()
{
	return CSheetMusic::Getsheetmusic()->getNoteWidth();
}


CBlankSymbol::CBlankSymbol( int starttime,int width )
{
	m_nstarttime = starttime;
	m_nwidth = width;
	SetType(BLANKSYMBOL);
}

CBlankSymbol::~CBlankSymbol( void )
{

}

CStem* ChordSymbol::GetStem()
{
	CStem* result;
	if (stem1 == NULL)
	{
		result = stem2;
	}
	else
	{
		if (stem2 == NULL)
		{
			result = stem1;
		}
		else
		{
			if (stem1->GetDuration() < stem2->GetDuration())
			{
				result = stem1;
			}
			else
			{
				result = stem2;
			}
		}
	}
	return result;
}

int ChordSymbol::GetMinWidth()
{
	return minwidth();
}

int ChordSymbol::GetAboveStaff()
{
	CWhiteNote* whitenote = ((NoteData*)m_vectorNoteData[m_vectorNoteData.size() -1])->whitenote;
	if (stem1 != NULL)
	{
		whitenote = CWhiteNote::MaxWhiteNote(whitenote,stem1->GetEnd());

	}
	if (stem2 != NULL)
	{
		whitenote = CWhiteNote::MaxWhiteNote(whitenote,stem2->GetEnd());
	}
	CWhiteNote* ptemp = CWhiteNote::TopWhiteNote(m_clef);
	if (ptemp == NULL)
	{
		return 1;
	}
	int num = (whitenote->Dist(ptemp))*(CSheetMusic::Getsheetmusic()->GetNoteHeight()/2);
	int num2 = 0;
	if (num > 0)
	{
		num2 = num;
	}
	std::vector<CAccidSymbol*> array1 = m_vectorAccidSymbol;
	for (int i= 0;i<(int)array1.size();i++)
	{
		CAccidSymbol* accidsymbol = array1[i];
		if (accidsymbol->GetAboveStaff() >num2)
		{
			num2 = accidsymbol->GetAboveStaff();

		}
	}
	if (ptemp != NULL)
	{
		delete ptemp;
		ptemp = NULL;
	}
	return num2;
}

int ChordSymbol::GetBelowStaff()
{
	CWhiteNote* whitenote = ((NoteData*)m_vectorNoteData[0])->whitenote;
	if (stem1 != NULL)
	{
		whitenote = CWhiteNote::MinWhiteNote(whitenote,stem1->GetEnd());
	}
	if (stem2 != NULL)
	{
		whitenote = CWhiteNote::MinWhiteNote(whitenote,stem2->GetEnd());
	}
	CWhiteNote* ptemp = CWhiteNote::BottomWhiteNote(m_clef);
	if (ptemp == NULL)
	{
		return 1;
	}
	int num = (ptemp->Dist(whitenote))*(CSheetMusic::Getsheetmusic()->GetNoteHeight()/2);
	int num2 = 0;
	if (num > 0)
	{
		num2 = num;
	}
	std::vector<CAccidSymbol*> array1 = m_vectorAccidSymbol;
	for(int i = 0;i<(int)array1.size();i++)
	{
		CAccidSymbol* accidsymbol = array1[i];
		if (accidsymbol->GetBelowStaff() >num2)
		{
			num2 = accidsymbol->GetBelowStaff();
		}
	}
	if (ptemp != NULL)
	{
		delete ptemp;
		ptemp = NULL;
	}
	return num2;
}

ChordSymbol::ChordSymbol( vectoreMidinote midinotes,CKeySignature* key,CTimeSignature* time,Clef c )
{
	int num = midinotes.size();
	hastwostems = false;
	m_clef = c;


	std::sort(midinotes.begin(),midinotes.end(),SortNotes);

	m_nstarttime = ((CMidiNote*)midinotes[0])->GetStarttime();
	m_nendtime = ((CMidiNote*)midinotes[0])->GetEndTime();
	m_vectorNoteData = CreateNoteData(midinotes,key,time);
	m_vectorAccidSymbol = CreateAccidSymbol(m_vectorNoteData,m_clef);
	NoteDuration stemDuration = time->GetStemDuration(((NoteData*)m_vectorNoteData[0])->duration);
	NoteDuration noteDuration = stemDuration;
	int num2 = -1;
	for (int i = 0;i<(int)m_vectorNoteData.size();i++)
	{
		noteDuration = time->GetStemDuration(((NoteData*)m_vectorNoteData[i])->duration);
		if (stemDuration != noteDuration)
		{
			num2 = i;
			break;
		}
	}

	if (stemDuration != noteDuration)
	{
		hastwostems = true;
		CWhiteNote* pwhitenote1 = new CWhiteNote;
		CWhiteNote* pwhitenote2 = new CWhiteNote;
		pwhitenote1->SetLetter(((NoteData*)m_vectorNoteData[0])->whitenote->GetLetter());
		pwhitenote1->SetOctave(((NoteData*)m_vectorNoteData[0])->whitenote->GetOctave());
		pwhitenote2->SetLetter(((NoteData*)m_vectorNoteData[num2 -1])->whitenote->GetLetter());
		pwhitenote2->SetOctave(((NoteData*)m_vectorNoteData[num2 -1])->whitenote->GetOctave());
		stem1 = new CStem(pwhitenote1,pwhitenote2,stemDuration,2,NoteOverlap(m_vectorNoteData,0,num2));

		CWhiteNote* pwhitenote3 = new CWhiteNote;
		CWhiteNote* pwhitenote4 = new CWhiteNote;
		pwhitenote3->SetLetter(((NoteData*)m_vectorNoteData[num2])->whitenote->GetLetter());
		pwhitenote3->SetOctave(((NoteData*)m_vectorNoteData[num2])->whitenote->GetOctave());
		pwhitenote4->SetLetter(((NoteData*)m_vectorNoteData[m_vectorNoteData.size() -1])->whitenote->GetLetter());
		pwhitenote4->SetOctave(((NoteData*)m_vectorNoteData[m_vectorNoteData.size() -1])->whitenote->GetOctave());
		stem2 = new CStem(pwhitenote3,pwhitenote4,noteDuration,1,NoteOverlap(m_vectorNoteData,num,m_vectorNoteData.size()));
	}
	else
	{
		int direction = StemDirection(((NoteData*)m_vectorNoteData[0])->whitenote,((NoteData*)m_vectorNoteData[m_vectorNoteData.size() -1])->whitenote,m_clef);
		CWhiteNote* pwhitenote1 = new CWhiteNote;
		pwhitenote1->SetLetter(((NoteData*)m_vectorNoteData[0])->whitenote->GetLetter());
		pwhitenote1->SetOctave(((NoteData*)m_vectorNoteData[0])->whitenote->GetOctave());
		CWhiteNote* pwhitenote2 = new CWhiteNote;
		pwhitenote2->SetLetter(((NoteData*)m_vectorNoteData[m_vectorNoteData.size() -1])->whitenote->GetLetter());
		pwhitenote2->SetOctave(((NoteData*)m_vectorNoteData[m_vectorNoteData.size() -1])->whitenote->GetOctave());
		stem1 = new CStem(pwhitenote1,pwhitenote2,stemDuration,direction,NoteOverlap(m_vectorNoteData,0,m_vectorNoteData.size()));
		stem2 = NULL;
	}

	if (stemDuration == Whole)
	{
		delete stem1;
		stem1 = NULL;
	}
	if (noteDuration == Whole)
	{
		delete stem2;
		stem2 = NULL;
	}

	m_nwidth = GetMinWidth();
	SetType(CHORDSYMBOL);
}

std::vector<NoteData*> ChordSymbol::CreateNoteData(vectoreMidinote midinotes,CKeySignature* key,CTimeSignature* time )
{
	int num = midinotes.size();
	std::vector<NoteData*> array1;
	for (int i = 0;i<num;i++)
	{
		CMidiNote* midinote = midinotes[i];
		NoteData* pnotedata = new NoteData();
		array1.push_back(pnotedata);
		array1[i]->number = midinote->GetNumber();
		array1[i]->bleftside = true;
		array1[i]->whitenote = key->GetWhiteNote(midinote->GetNumber());
		array1[i]->duration = time->GetNoteDuration(midinote->GetEndTime() - midinote->GetStarttime());
		array1[i]->accid = key->GetAccidental(midinote->GetNumber(),midinote->GetStarttime()/time->GetMeasure());

		if (i >0 && array1[i]->whitenote->Dist(array1[i-1]->whitenote) == 1)
		{
			if (array1[i-1]->bleftside)
			{
				array1[i]->bleftside = false;
			}
			else
			{
				array1[i]->bleftside = true;
			}
		}
		else
		{
			array1[i]->bleftside = true;
		}
	}
	return array1;
}

std::vector<CAccidSymbol*> ChordSymbol::CreateAccidSymbol( std::vector<NoteData*> notedata,Clef clef )
{
	int num = 0;
	for (int i = 0; i<(int) notedata.size();i++)
	{
		NoteData* ndata = notedata[i];
		if (ndata->accid != None)
		{
			num++;
		}
	}
	std::vector<CAccidSymbol*> array1;

	int num2 = 0;
	for (int i = 0;i<(int)notedata.size();i++)
	{
		NoteData* ndata = notedata[i];
		if (ndata->accid != None)
		{
			CWhiteNote* pwhitenotetemp = new CWhiteNote;
			pwhitenotetemp->SetLetter(ndata->whitenote->GetLetter());
			pwhitenotetemp->SetOctave(ndata->whitenote->GetOctave());
			CAccidSymbol* paccidsymbol = new CAccidSymbol(ndata->accid,pwhitenotetemp,clef);

			array1.push_back(paccidsymbol);
			num2++;
		}
	}
	return array1;
}



bool ChordSymbol::NoteOverlap( std::vector<NoteData*> notedata,int start,int end )
{

	for (int i = start;i< end;i++)
	{
		if (!notedata[i]->bleftside)
		{			
			return true;
		}
	}

	return false;
}

int ChordSymbol::minwidth()
{
	int num = NotesGetWidth();
	std::vector<CAccidSymbol*> array1 = m_vectorAccidSymbol;
	if (array1.size()>0)
	{
		num += ((CAccidSymbol*)array1[0])->GetMinWidth();
		for (int i = 1;i<(int)array1.size();i++)
		{
			if ((((CAccidSymbol*)array1[i])->GetWhitenote()->Dist(((CAccidSymbol*)array1[i-1])->GetWhitenote())) < 6)
			{
				num += ((CAccidSymbol*)array1[0])->GetMinWidth();
			}
		}
	}
	return num;
}

int ChordSymbol::NotesGetWidth()
{
	return 2*CSheetMusic::Getsheetmusic()->GetNoteHeight()+CSheetMusic::Getsheetmusic()->GetNoteHeight()/2;
}

bool ChordSymbol::CanMakeTriplet(ChordSymbol* c1,ChordSymbol* c2,ChordSymbol* c3,CTimeSignature* time)
{
	CStem* stem = c1->GetStem();
	CStem* stem2 = c2->GetStem();
	CStem* stem3 = c3->GetStem();
	int Stime1 = c1->GetStartTime();
	int etime1 = c1->GetEndTime();
	int Stime2 = c2->GetStartTime();
	int etime2 = c2->GetEndTime();
	int Stime3 = c3->GetStartTime();
	int etime3 = c3->GetEndTime();
	int timemeasure = time->GetMeasure();
	return (Stime1/timemeasure == Stime2/timemeasure)&&(Stime2/timemeasure == Stime3/timemeasure)&&(Stime2-etime1<time->GetQuarter()/4)&&(Stime3 - etime2 <time->GetQuarter()/4)&&(stem != NULL&&stem2!= NULL&&stem3!= NULL)&&(stem->GetDuration() == Treble)&&(Stime1%time->GetQuarter() <= time->GetQuarter()/4)&&(stem->GetDuration() == stem2->GetDuration())&&(stem2->GetDuration() == stem3->GetDuration())&&(!stem->GetReceiver()&&!stem2->GetReceiver()&&!stem3->GetReceiver())&&(stem->GetDirection() == stem2->GetDirection()||!c1->GetHastwostems()||!c2->GetHastwostems())&&(stem2->GetDirection() == stem3->GetDirection()||!c2->GetHastwostems()||!c3->GetHastwostems())&&(stem->GetDirection() == stem3->GetDirection()||!c1->GetHastwostems()||!c3->GetHastwostems());
}

int ChordSymbol::StemDirection(CWhiteNote* bottom,CWhiteNote* top,Clef clef)
{

	CWhiteNote* whitenote;
	if (clef == Treble)
	{
		whitenote = new CWhiteNote(1,5);
	}
	else
	{
		whitenote = new CWhiteNote(3,3);
	}

	int num = whitenote->Dist(bottom) + whitenote->Dist(top);
	int result;
	if (num >= 0)
	{
		result = 1;

	}
	else
	{
		result = 2;
	}
	delete whitenote;
	whitenote = NULL;
	return result;

}

void ChordSymbol::MakeTriplet(ChordSymbol* c1,ChordSymbol* c2,ChordSymbol* c3,int spacing)
{
	CStem* stem = c1->GetStem();
	CStem* stem2 = c2->GetStem();
	CStem* stem3 = c3->GetStem();
	int num;
	if (c1->GetHastwostems())
	{
		num = stem->GetDirection();
	}
	else
	{
		if (c2->GetHastwostems())
		{
			num = stem2->GetDirection();
		}
		else
		{
			if(c3->GetHastwostems())
			{
				num = stem3->GetDirection();
			}
			else
			{
				CWhiteNote* bottom;
				if(stem->GetDirection() ==1)
				{
					bottom =stem->GetTop();
				}
				else
				{
					bottom =stem->GetBottom();
				}

				CWhiteNote* top ;
				if(stem3->GetDirection() ==1)
				{
					top =stem3->GetTop();
				}
				else
				{
					top =stem3->GetBottom();
				}

				num = StemDirection(bottom,top,c1->Getclef());
			}
		}
	}
	stem->SetDirection(num);
	stem2->SetDirection(num);
	stem3->SetDirection(num);
	if (num == 1)
	{
		CWhiteNote* whitenote = CWhiteNote::MaxWhiteNote(stem->GetEnd(),stem2->GetEnd());
		whitenote = CWhiteNote::MaxWhiteNote(whitenote,stem3->GetEnd());
		stem->SetEnd(whitenote);
		stem2->SetEnd(whitenote);
		stem3->SetEnd(whitenote);
	}
	else
	{
		CWhiteNote* whitenote = CWhiteNote::MinWhiteNote(stem->GetEnd(),stem2->GetEnd());
		whitenote = CWhiteNote::MinWhiteNote(whitenote,stem3->GetEnd());
		stem->SetEnd(whitenote);
		stem2->SetEnd(whitenote);
		stem3->SetEnd(whitenote);
	}
	stem->SetPair(stem3,spacing);
	stem2->SetReceiver(true);
	stem3->SetReceiver(true);
}

bool ChordSymbol::CanMakePair( ChordSymbol* c1,ChordSymbol* c2,CTimeSignature* time )
{
	CStem* stem1 = c1->GetStem();
	CStem* stem2 = c2->GetStem();
	return (c1->GetStartTime()/time->GetMeasure() == c2->GetStartTime()/time->GetMeasure()) &&((c2->GetStartTime()-c1->GetEndTime())<time->GetQuarter()/4)&&(stem1 != NULL)&&(stem2!= NULL)&&(stem1->GetDuration() != Quarter)&&(stem1->GetDuration() == stem2->GetDuration())&&(!stem1->GetReceiver())&&(!stem2->GetReceiver())&&(stem1->GetDirection() == stem2->GetDirection()|| !c1->GetHastwostems()||!c2->GetHastwostems());

}

void ChordSymbol::MakePair( ChordSymbol* c1,ChordSymbol* c2,int spacing )
{
	CStem* stem = c1->GetStem();
	CStem* stem2 = c2->GetStem();
	int direction;
	if (c1->GetHastwostems())
	{
		direction = stem->GetDirection();
	}
	else
	{
		if (c2->GetHastwostems())
		{
			direction = stem2->GetDirection();
		}
		else
		{
			CWhiteNote* bottom;
			if(stem->GetDirection() ==1)
			{
				bottom = stem->GetTop();
			}
			else
			{
				bottom =stem->GetBottom();
			}
			CWhiteNote* top;
			if (stem2->GetDirection() == 1)
			{
				top = stem2->GetTop();
			}
			else
			{
				top =stem2->GetBottom();
			}
			direction = StemDirection(bottom,top,c1->Getclef());
		}
	}
	stem->SetDirection(direction);
	stem2->SetDirection(direction);
	stem->SetPair(stem2,spacing);
	stem2->SetReceiver(true);
}

void ChordSymbol::Draw(Graphics* pDC,int ytop)
{
	
	CWhiteNote* topstaff = CWhiteNote::TopWhiteNote(m_clef);
	int num = DrawAccid(pDC,ytop);
	pDC->TranslateTransform(num,0);

	DrawNotes(pDC,ytop,topstaff);
	if (stem1 != NULL)
	{
		stem1->Draw(pDC,ytop,topstaff);
	}
	if (stem2 != NULL)
	{
		stem2->Draw(pDC,ytop,topstaff);
	}
	pDC->TranslateTransform(-num,0);
	if (topstaff != NULL)
	{
		delete topstaff;
		topstaff = NULL;
	}
}

ChordSymbol::~ChordSymbol( void )
{
	if (stem1 != NULL)
	{
		delete stem1;
		stem1 = NULL;
	}
	if (stem2 != NULL)
	{
		delete stem2;
		stem2 = NULL;
	}
	if (!m_vectorNoteData.empty())
	{
		for (int i = 0;i<m_vectorNoteData.size();i++)
		{
			delete m_vectorNoteData[i]->whitenote;
			m_vectorNoteData[i]->whitenote = NULL;
			delete m_vectorNoteData[i];
			m_vectorNoteData[i] = NULL;
		}
		m_vectorNoteData.clear();
	}

	if (!m_vectorAccidSymbol.empty())
	{
		for(int i =0;i<m_vectorAccidSymbol.size();i++)
		{
			delete m_vectorAccidSymbol[i];
			m_vectorAccidSymbol[i] = NULL;
		}
		m_vectorAccidSymbol.clear();
	}




}

int ChordSymbol::DrawAccid( Graphics* pDC,int ytop )
{
	int num = 0;
	CAccidSymbol* accidsymbol = NULL;
	std::vector<CAccidSymbol*> vecaccid = m_vectorAccidSymbol;
	for (int i = 0;i<(int)vecaccid.size();i++)
	{
		CAccidSymbol* accidsymbol2 = vecaccid[i];
		if (accidsymbol!= NULL && accidsymbol2->GetWhitenote()->Dist(accidsymbol->GetWhitenote()) < 6)
		{
			num += accidsymbol2->GetWidth();
		}
		pDC->TranslateTransform(num,0);
		accidsymbol2->Draw(pDC,ytop);
		pDC->TranslateTransform(-num,0);
		accidsymbol = accidsymbol2;
	}
	if (accidsymbol != NULL)
	{
		num += accidsymbol->GetWidth();
	}
	return num;

}

void ChordSymbol::DrawNotes( Graphics* pDC,int ytop,CWhiteNote* staff )
{
	int noteheight = CSheetMusic::Getsheetmusic()->GetNoteHeight();
	int linespace = CSheetMusic::Getsheetmusic()->GetLineSpace();
	int notewidth = CSheetMusic::Getsheetmusic()->getNoteWidth();
	std::vector<NoteData*> vecnotedata = m_vectorNoteData;
	for (int i=0;i<(int)vecnotedata.size();i++)
	{
		NoteData* notedata = vecnotedata[i];
		int num = ytop + staff->Dist(notedata->whitenote)*noteheight/2;
		Pen pendraw(Color(0,0,0),(Gdiplus::REAL)1);

		int num2 = linespace/4;
		if (!notedata->bleftside)
		{
			num2 += notewidth;
		}
		int xoffset =num2 + notewidth/2;
		int yoffset = num - 1+ noteheight/2;
		pDC->TranslateTransform(xoffset,yoffset);
		pDC->RotateTransform(-45);
		SolidBrush brush(Color(0,0,0));
		if (notedata->duration == Whole || notedata->duration == Half||notedata->duration == DottedHalf)
		{
			pDC->DrawEllipse(&pendraw,-notewidth/2,-(noteheight/2),notewidth,noteheight);
			pDC->DrawEllipse(&pendraw,-notewidth/2,-(noteheight/2)+1,notewidth,noteheight-2);
			pDC->DrawEllipse(&pendraw,-notewidth/2,-(noteheight/2)+1,notewidth,noteheight-3);

		}
		else
		{
			
			pDC->FillEllipse(&brush,-(notewidth/2),-(noteheight/2),notewidth,noteheight);

		}
		pDC->DrawEllipse(&pendraw,-(notewidth/2),-(noteheight/2),notewidth,noteheight);
		pDC->RotateTransform(45);
		pDC->TranslateTransform(-xoffset,-yoffset);
		//pDC->TranslateTransform(-(num2+CSheetMusic::Getsheetmusic()->getNoteWidth()/2),-(num -1+CSheetMusic::Getsheetmusic()->GetNoteHeight()));
		if (notedata->duration == DottedHalf||notedata->duration == DottedQuarter || notedata->duration== DottedEighth)
		{
			pDC->FillEllipse(&brush,num2 + notewidth+linespace/3,num+linespace/3,4,4);
			
		}
		CWhiteNote* whitenote;
		whitenote = staff->Add(1);
		int num3 = notedata->whitenote->Dist(whitenote);
		int num4 = ytop -1;
		if (num3 >= 2)
		{
			for(int j = 2;j<= num3;j +=2)
			{
				num4 -= noteheight;
				pDC->DrawLine(&pendraw,num2-linespace/4,num4,num2+notewidth+linespace/4,num4);
			}
		}
		CWhiteNote* whitenote2 ;
		whitenote2 = whitenote->Add(-8);
		num4 = ytop + (linespace+1)*4 -1;
		num3 = whitenote2->Dist(notedata->whitenote);
		if (num3>=2)
		{
			for (int k =2;k<=num3;k +=2 )
			{
				num4 +=noteheight;
				pDC->DrawLine(&pendraw,num2-linespace/4,num4,num2 + notewidth+linespace/4,num4);
			}
		}

		delete whitenote;
		delete whitenote2;

	}
}



CClefSymbol::CClefSymbol( Clef clef,int sartt,bool bsmall )
{
	mclef = clef;
	starttime = sartt;
	smallsize = bsmall;
	m_treble = NULL;
	m_bass = NULL;
	LoadImages();
	width = GetMinWidth();
	SetType(CLEFSYMBOL);
}

void CClefSymbol::LoadImages()
{
	
	m_treble = DrawtrableClefImage();
	m_bass = DrawbassClefImage();
}

int CClefSymbol::GetMinWidth()
{
	int nresult = 0;
	if (smallsize)
	{
		nresult = CSheetMusic::Getsheetmusic()->getNoteWidth()*2;
	}
	else
	{
		nresult = CSheetMusic::Getsheetmusic()->getNoteWidth()*3;
	}
	return nresult;
}

int CClefSymbol::GetAboveStaff()
{
	int nresult = 0;
	if (mclef == Treble && !smallsize)
	{
		nresult = CSheetMusic::Getsheetmusic()->GetNoteHeight()*2;
	}
	return nresult;
}

int CClefSymbol::GetBelowStaff()
{
	int nresult;
	if (mclef == Treble && !smallsize)
	{
		nresult = CSheetMusic::Getsheetmusic()->GetNoteHeight()*2;
	}
	else
	{
		if (mclef == Treble && smallsize)
		{
			nresult = CSheetMusic::Getsheetmusic()->GetNoteHeight();
		}
		else
		{
			nresult = 0;
		}
	}
	return nresult;
}

CClefSymbol::~CClefSymbol( void )
{
	if (m_bass != NULL)
	{
		delete m_bass;
		m_bass = NULL;
	}
	if (m_treble != NULL)
	{
		delete m_treble;
		m_treble = NULL;
	}
}

void CClefSymbol::Draw( Graphics* pDC,int ytop )
{

	int y= ytop;
	if (m_treble == NULL ||m_bass == NULL)
	{
		return;
	}
	int num;
	int staffheight = CSheetMusic::Getsheetmusic()->GetStaffHeight();
	int noteheight = CSheetMusic::Getsheetmusic()->GetNoteHeight();
	if (mclef == Treble)
	{
		if (smallsize)
		{
			num = staffheight*5/4;
		}
		else
		{
			num = 3*staffheight/2 + noteheight/2;
			y = ytop - noteheight;
		}
		int num2 = 77*num/202;
		
		pDC->DrawImage(m_treble,0,y,num2,num);
	}
	else
	{
		if (smallsize)
		{
			num = staffheight - 3*noteheight/2;
		}
		else
		{
			num = staffheight - noteheight;
		}
		int num2 = 108*num/130;
		pDC->DrawImage(m_bass,0,y,num2,num);
	}
	
}



CRestSymbol::CRestSymbol( int start,NoteDuration dur )
{
	starttime = start;
	duration = dur;
	width = GetMinWidth();
	SetType(RESTSYMBOL);
}

int CRestSymbol::GetMinWidth()
{
	int nresult;
	nresult = (CSheetMusic::Getsheetmusic()->GetNoteHeight()*3)/2;
	return nresult;
}

int CRestSymbol::GetAboveStaff()
{
	return 0;
}

int CRestSymbol::GetBelowStaff()
{
	return 0;
}

void CRestSymbol::Draw( Graphics* pDC,int ytop )
{
	
	int xoffset = CSheetMusic::Getsheetmusic()->GetNoteHeight()/2;
	pDC->TranslateTransform(xoffset,0);
	if (duration == Whole)
	{
		DrawWhole(pDC,ytop);
	}
	else
	{
		if (duration == Half)
		{
			DrawHalf(pDC,ytop);
		}
		else
		{
			if (duration == Quarter)
			{
				DrawQuarter(pDC,ytop);
			}
			else
			{
				if (duration == Eighth)
				{
					DrawEighth(pDC,ytop);
				}
			}
		}
	}
	pDC->TranslateTransform(-xoffset,0);
	
}

void CRestSymbol::DrawWhole( Graphics* pDC,int ytop )
{
	int y = ytop + CSheetMusic::Getsheetmusic()->GetNoteHeight();
	SolidBrush brush(Color(0,0,0));
	pDC->FillRectangle(&brush,0,y,CSheetMusic::Getsheetmusic()->getNoteWidth(),CSheetMusic::Getsheetmusic()->GetNoteHeight()/2);
}

void CRestSymbol::DrawHalf( Graphics* pDC,int ytop )
{
	int y =ytop + CSheetMusic::Getsheetmusic()->GetNoteHeight()*3/2;
	SolidBrush brush(Color(0,0,0));
	pDC->FillRectangle(&brush,0,y,CSheetMusic::Getsheetmusic()->getNoteWidth(),CSheetMusic::Getsheetmusic()->GetNoteHeight()/2);
}

void CRestSymbol::DrawQuarter( Graphics* pDC,int ytop )
{
	int noteheight = CSheetMusic::Getsheetmusic()->GetNoteHeight();
	Pen pendraw(Color(0,0,0));
	pendraw.SetEndCap((Gdiplus::LineCap)LineCapFlat);
	int num = ytop + noteheight/2;
	int num2 = 2;
	int num3 = num2 + 2*noteheight/3;
	pDC->DrawLine(&pendraw,num2,num,num3-1,num+noteheight-1);

	pendraw.SetWidth(CSheetMusic::Getsheetmusic()->GetLineSpace()/2);
	num = ytop + noteheight+1;
	pDC->DrawLine(&pendraw,num3-2,num,num2,num+noteheight);

	pendraw.SetWidth(1);
	num = ytop + noteheight*2-1;
	pDC->DrawLine(&pendraw,0,num,num3+2,num+noteheight);

	pendraw.SetWidth(CSheetMusic::Getsheetmusic()->GetLineSpace()/2);
	pDC->DrawLine(&pendraw,num3,num+3*noteheight/4,num2/2,num+3*noteheight/4);

	pendraw.SetWidth(1);
	pDC->DrawLine(&pendraw,0,num+2*noteheight/3+1,num3-1,num3+3*noteheight/2);

}

void CRestSymbol::DrawEighth( Graphics* pDC,int ytop )
{
	int linespace = CSheetMusic::Getsheetmusic()->GetLineSpace();
	int num = ytop + CSheetMusic::Getsheetmusic()->GetNoteHeight() -1;
	SolidBrush brush(Color(0,0,0));
	pDC->FillEllipse(&brush,0,num+1,linespace-1,linespace-1);

	Pen pendraw(Color(0,0,0));
	pendraw.SetWidth(1);

	pDC->DrawLine(&pendraw,(linespace -2)/2,num + linespace-1,3*linespace/2,num+linespace/2);
	pDC->DrawLine(&pendraw,3*linespace/2,num+linespace/2,3*linespace/4,num+CSheetMusic::Getsheetmusic()->GetNoteHeight()*2);

	
}

CRestSymbol::~CRestSymbol( void )
{

}

void CRestSymbol::SetWidth( int nvalue )
{
	width = nvalue;
}
