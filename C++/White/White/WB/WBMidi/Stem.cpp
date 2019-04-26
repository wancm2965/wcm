#include "StdAfx.h"
#include "Stem.h"
#include "SheetMusic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CStem::CStem( CWhiteNote* bottomnote,CWhiteNote* topnote,NoteDuration duration,int direction,bool overlap )
{
	Up = 1;
	Down = 2;
	LeftSide = 1;
	RightSide = 2;
	m_top = topnote;
	m_bottom = bottomnote;
	m_duration = duration;
	m_direction = direction;
	m_notesoverlap = overlap;	
	if (m_direction == 1 || m_notesoverlap)
	{
		m_side = 2;
	}
	else
	{
		m_side = 1;
	}
	m_endnote = CalculateEnd();
	m_pair = NULL;
	m_width_to_pair = 0;
	m_receiver_in_pair = false;
}

CWhiteNote* CStem::CalculateEnd()
{
	CWhiteNote* reslut = NULL;
	if (m_direction == 1)
	{
		CWhiteNote* pwhitenote1 = m_top;
		CWhiteNote* pwhitenote2;
		pwhitenote1 = pwhitenote1->Add(6);
		pwhitenote2 = pwhitenote1;
		if (m_duration == Sixteenth)
		{
			pwhitenote1 = pwhitenote2->Add(2);
			
 			delete pwhitenote2;
 			pwhitenote2 = NULL;
		}
		else
		{
			if (m_duration == ThirtySecond)
			{
				pwhitenote1 = pwhitenote2->Add(4);
 				delete pwhitenote2;
 				pwhitenote2 = NULL;
			}
		}
		reslut = pwhitenote1;
	}
	else if (m_direction == 2)
	{
		CWhiteNote* pwhitenote1 = m_bottom;
		CWhiteNote* pwhitenote2;
		pwhitenote1 = pwhitenote1->Add(-6);
		pwhitenote2 = pwhitenote1;
		if (m_duration == Sixteenth)
		{
			pwhitenote1 = pwhitenote2->Add(-2);
 			delete pwhitenote2;
 			pwhitenote2 = NULL;
			
		}
		else
		{
			if (m_duration == ThirtySecond)
			{
				pwhitenote1 = pwhitenote2->Add(-4);
 				delete pwhitenote2;
 				pwhitenote2 = NULL;
			}
		}
		reslut = pwhitenote1;
	}
	else
	{
		reslut = NULL;
	}
	return reslut;
}

void CStem::SetDirection( int nvalue )
{
	ChangeDirection(nvalue);
}

void CStem::ChangeDirection( int newdirection )
{
	m_direction = newdirection;
	if (m_direction == 1|| m_notesoverlap)
	{
		m_side = 2;

	}
	else
	{
		m_side =1;
	}	
	if (m_endnote != NULL)
	{
		delete m_endnote;
	}
	m_endnote = CalculateEnd();
}

void CStem::SetPair( CStem* paird,int width_to_pai )
{
	if (m_pair!= NULL)
	{
		delete m_pair;
		m_pair = NULL;
	}
	m_pair = paird;
	m_width_to_pair = width_to_pai;
	if (m_direction == 1)
	{
		if (paird->GetEnd()->Dist(m_endnote)>7)
		{
			CWhiteNote* pwhitenote;
			pwhitenote = m_endnote->Add(2);
			delete m_endnote;
			m_endnote = pwhitenote;
		}
		else
		{
			if (m_endnote->Dist(paird->GetEnd()) > 7)
			{

				CWhiteNote* pwhitenote;
				pwhitenote = paird->m_endnote->Add(2);
				delete paird->m_endnote;

				paird->SetEnd(pwhitenote);
			}
		}
	}
	else
	{
		if (m_direction == 2)
		{
			if (paird->GetEnd()->Dist(m_endnote)>7)
			{
				CWhiteNote* pwhitenote;
				pwhitenote =m_endnote->Add(-2);
				delete m_endnote;
				m_endnote = pwhitenote;

			}
			else
			{
				if (m_endnote->Dist(paird->GetEnd()) > 7)
				{
					CWhiteNote* pwhitenote;
					pwhitenote = paird->m_endnote->Add(-2);
					delete paird->m_endnote;

					paird->SetEnd(pwhitenote);
				}
			}
		}
	}
}

void CStem::Draw( Graphics* gdc,int ytop,CWhiteNote* staff )
{
	if (m_duration != Whole)
	{
		DrawVerticalLine(gdc,ytop,staff);
		if (m_duration != Quarter && m_duration != Half && !m_receiver_in_pair)
		{
			if (m_pair != NULL)
			{
				DrawHorizBarStem(gdc,ytop,staff);
			}
			else
			{
				DrawCurvyStem(gdc,ytop,staff);
			}
		}
	}
}

void CStem::DrawVerticalLine( Graphics* gdc,int ytop,CWhiteNote* staff )
{
	int num;
	if (m_side ==1)
	{
		num = CSheetMusic::Getsheetmusic()->GetLineSpace()/4+1;
	}
	else
	{
		num = CSheetMusic::Getsheetmusic()->GetLineSpace()/4+CSheetMusic::Getsheetmusic()->getNoteWidth();
	}
	if (m_direction == 1)
	{
		int num2 = ytop + staff->Dist(m_bottom)*CSheetMusic::Getsheetmusic()->GetNoteHeight()/2+CSheetMusic::Getsheetmusic()->GetNoteHeight()/4;
		int y = ytop+staff->Dist(m_endnote)*CSheetMusic::Getsheetmusic()->GetNoteHeight()/2;
		Pen pendraw(Color(0,0,0));
		gdc->DrawLine(&pendraw,num,num2,num,y);
	}
	else
	{
		if (m_direction == 2)
		{
			int num2 = ytop + staff->Dist(m_top)*CSheetMusic::Getsheetmusic()->GetNoteHeight()/2+CSheetMusic::Getsheetmusic()->GetNoteHeight();
			if (m_side == 1)
			{
				num2 -= CSheetMusic::Getsheetmusic()->GetNoteHeight()/4; 
			}
			else
			{
				num2 -= CSheetMusic::Getsheetmusic()->GetNoteHeight()/2;
			}
			int y = ytop + staff->Dist(m_endnote)*CSheetMusic::Getsheetmusic()->GetNoteHeight()/2+CSheetMusic::Getsheetmusic()->GetNoteHeight();
			Pen pendraw(Color(0,0,0));
			gdc->DrawLine(&pendraw,num,num2,num,y);
		}
	}



}

void CStem::DrawCurvyStem( Graphics* gdc,int ytop,CWhiteNote* staff )
{
	Pen pendraw(Color(0,0,0));
	pendraw.SetWidth(2);
	int num;
	if (m_side == 1)
	{
		num = CSheetMusic::Getsheetmusic()->GetLineSpace()/4+1;
	}
	else
	{
		num = CSheetMusic::Getsheetmusic()->GetLineSpace()/4 + CSheetMusic::Getsheetmusic()->getNoteWidth();
	}

	if (m_direction == 1)
	{
		int num2 = ytop + staff->Dist(m_endnote)*CSheetMusic::Getsheetmusic()->GetNoteHeight()/2;
		if (m_duration == Eighth || m_duration == Triplet||m_duration == Sixteenth||m_duration == ThirtySecond)
		{
			gdc->DrawBezier(&pendraw,num,num2,num,(num2+3*CSheetMusic::Getsheetmusic()->GetLineSpace()/2),(num+ CSheetMusic::Getsheetmusic()->GetLineSpace()*2),(num2+CSheetMusic::Getsheetmusic()->GetNoteHeight()*2),(num+CSheetMusic::Getsheetmusic()->GetLineSpace()/2),(num2+CSheetMusic::Getsheetmusic()->GetNoteHeight()*3));
		}
		num2 += CSheetMusic::Getsheetmusic()->GetNoteHeight();
		if (m_duration == Sixteenth || m_duration == ThirtySecond)
		{
			gdc->DrawBezier(&pendraw,num,num2,num,(num2+3*CSheetMusic::Getsheetmusic()->GetLineSpace()/2),(num+CSheetMusic::Getsheetmusic()->GetLineSpace()*2),(num2+CSheetMusic::Getsheetmusic()->GetNoteHeight()*2),(num+CSheetMusic::Getsheetmusic()->GetLineSpace()/2),(num2+CSheetMusic::Getsheetmusic()->GetNoteHeight()*3));
		}
		num2 += CSheetMusic::Getsheetmusic()->GetNoteHeight();
		if ( m_duration == ThirtySecond)
		{
			gdc->DrawBezier(&pendraw,num,num2,num,(num2+3*CSheetMusic::Getsheetmusic()->GetLineSpace()/2),(num+CSheetMusic::Getsheetmusic()->GetLineSpace()*2),(num2+CSheetMusic::Getsheetmusic()->GetNoteHeight()*2),(num+CSheetMusic::Getsheetmusic()->GetLineSpace()/2),(num2+CSheetMusic::Getsheetmusic()->GetNoteHeight()*3));
		}
	}
	else
	{
		if (m_direction == 2)
		{
			int num2 = ytop + staff->Dist(m_endnote)*CSheetMusic::Getsheetmusic()->GetNoteHeight()/2+CSheetMusic::Getsheetmusic()->GetNoteHeight();
			if (m_duration == Eighth || m_duration == Triplet||m_duration == Sixteenth||m_duration == ThirtySecond)
			{
				gdc->DrawBezier(&pendraw,num,num2,num,(num2-CSheetMusic::Getsheetmusic()->GetLineSpace()),(num+ CSheetMusic::Getsheetmusic()->GetLineSpace()*2),(num2-CSheetMusic::Getsheetmusic()->GetNoteHeight()*2),(num+CSheetMusic::Getsheetmusic()->GetLineSpace()),(num2-CSheetMusic::Getsheetmusic()->GetNoteHeight()*2-CSheetMusic::Getsheetmusic()->GetLineSpace()/2));
			
			}
			num2 -= CSheetMusic::Getsheetmusic()->GetNoteHeight();
			if (m_duration == Sixteenth||m_duration == ThirtySecond)
			{
				gdc->DrawBezier(&pendraw,num,num2,num,(num2-CSheetMusic::Getsheetmusic()->GetLineSpace()),(num+ CSheetMusic::Getsheetmusic()->GetLineSpace()*2),(num2-CSheetMusic::Getsheetmusic()->GetNoteHeight()*2),(num+CSheetMusic::Getsheetmusic()->GetLineSpace()),(num2-CSheetMusic::Getsheetmusic()->GetNoteHeight()*2-CSheetMusic::Getsheetmusic()->GetLineSpace()/2));

			}
			num2 -= CSheetMusic::Getsheetmusic()->GetNoteHeight();
			if (m_duration == ThirtySecond)
			{
				gdc->DrawBezier(&pendraw,num,num2,num,(num2-CSheetMusic::Getsheetmusic()->GetLineSpace()),(num+ CSheetMusic::Getsheetmusic()->GetLineSpace()*2),(num2-CSheetMusic::Getsheetmusic()->GetNoteHeight()*2),(num+CSheetMusic::Getsheetmusic()->GetLineSpace()),(num2-CSheetMusic::Getsheetmusic()->GetNoteHeight()*2-CSheetMusic::Getsheetmusic()->GetLineSpace()/2));

			}
		}
	}

}

void CStem::DrawHorizBarStem( Graphics* gdc,int ytop,CWhiteNote* staff )
{
	Pen pendraw(Color(0,0,0));
	pendraw.SetWidth(CSheetMusic::Getsheetmusic()->GetNoteHeight()/2);
	int x = 0;
	int num = 0;
	if (m_side ==1)
	{
		x = CSheetMusic::Getsheetmusic()->GetLineSpace()/4+1;
	}
	else
	{
		if (m_side == 2)
		{
			x = CSheetMusic::Getsheetmusic()->GetLineSpace()/4+CSheetMusic::Getsheetmusic()->getNoteWidth();
		}

	}

	if (m_pair->m_side == 1)
	{
		num = CSheetMusic::Getsheetmusic()->GetLineSpace()/4+1;
	}
	else
	{
		if (m_pair->m_side ==2)
		{
			num = CSheetMusic::Getsheetmusic()->GetLineSpace()/4+CSheetMusic::Getsheetmusic()->getNoteWidth();
		}
	}

	if (m_direction == 1)
	{
		int x2 = m_width_to_pair + num;
		int num2 = ytop + staff->Dist(m_endnote)*CSheetMusic::Getsheetmusic()->GetNoteHeight()/2;
		int num3 = ytop + staff->Dist(m_pair->GetEnd())*CSheetMusic::Getsheetmusic()->GetNoteHeight()/2;
		if (m_duration == Eighth||m_duration == Triplet||m_duration == Sixteenth|m_duration == ThirtySecond )
		{
			gdc->DrawLine(&pendraw,x,num2,x2,num3);
		}
		num2 += CSheetMusic::Getsheetmusic()->GetNoteHeight();
		num3 += CSheetMusic::Getsheetmusic()->GetNoteHeight();
		if (m_duration == Sixteenth||m_duration== ThirtySecond)
		{
			gdc->DrawLine(&pendraw,x,num2,x2,num3);
		}
		num2 += CSheetMusic::Getsheetmusic()->GetNoteHeight();
		num3 += CSheetMusic::Getsheetmusic()->GetNoteHeight();
		if (m_duration== ThirtySecond)
		{
			gdc->DrawLine(&pendraw,x,num2,x2,num3);
		}
	}
	else
	{
		int x2 = m_width_to_pair + num;
		int num2 = ytop + staff->Dist(m_endnote)*CSheetMusic::Getsheetmusic()->GetNoteHeight()/2+CSheetMusic::Getsheetmusic()->GetNoteHeight();
		int num3 = ytop + staff->Dist(m_pair->GetEnd())*CSheetMusic::Getsheetmusic()->GetNoteHeight()/2 + CSheetMusic::Getsheetmusic()->GetNoteHeight();
		if (m_duration == Eighth||m_duration == Triplet||m_duration == Sixteenth|m_duration == ThirtySecond )
		{
			gdc->DrawLine(&pendraw,x,num2,x2,num3);
		}
		num2 -= CSheetMusic::Getsheetmusic()->GetNoteHeight();
		num3 -= CSheetMusic::Getsheetmusic()->GetNoteHeight();
		if (m_duration == Sixteenth||m_duration== ThirtySecond)
		{
			gdc->DrawLine(&pendraw,x,num2,x2,num3);
		}
		num2 -= CSheetMusic::Getsheetmusic()->GetNoteHeight();
		num3 -= CSheetMusic::Getsheetmusic()->GetNoteHeight();
		if (m_duration== ThirtySecond)
		{
			gdc->DrawLine(&pendraw,x,num2,x2,num3);
		}

	}

}

CStem::~CStem( void )
{
	if (m_top != NULL)
	{
		delete m_top;
		m_top = NULL;
	}
	if (m_bottom != NULL)
	{
		delete m_bottom;
		m_bottom = NULL;
	}
	if (m_endnote != NULL)
	{
		delete m_endnote;
		m_endnote = NULL;
	}
	
}
