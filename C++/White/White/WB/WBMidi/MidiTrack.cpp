#include "StdAfx.h"
#include "MidiTrack.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CString CMidiTrack::GetInstrument()
{
	CString result;
	if (m_notes[0]->GetChannel() == 10)
	{
		result = _T("Percusson");
	}
	else
	{
		result = instrument;
	}
	return result;
}


CMidiTrack::CMidiTrack( int track /*= 0*/ )
{
	tracknum = track;
	instrument = _T("");
}

void CMidiTrack::AddNote( CMidiNote* m )
{
	m_notes.push_back(m);
}

void CMidiTrack::NoteOff( int chan,int notenum,int edtime )
{
	if (!m_notes.empty())
	{
		for (int i = m_notes.size() - 1;i>=0;i--)
		{
			CMidiNote* midinote = m_notes[i];
			if (midinote->GetChannel() == chan && midinote->GetNumber() == notenum && midinote->GetDuration() == 0)
			{
				midinote->NoteOff(edtime);
				break;
			}
		}

	}
}

void CMidiTrack::Clone( CMidiTrack* other )
{
	other->SetInstrument(instrument);
	other->Settracknum(tracknum);
	for (int i = 0;i<(int)m_notes.size();i++)
	{
		CMidiNote* midinote = new CMidiNote;
		((CMidiNote*)m_notes[i])->Clone(midinote);
		other->AddNote(midinote);
	}
}

bool CMidiTrack::HasMultipleChannels()
{
	bool result = false;
	if (!m_notes.empty())
	{
		int chane = ((CMidiNote*)m_notes[0])->GetChannel();
		
		std::vector<CMidiNote*>::iterator it = m_notes.begin();
		while(it != m_notes.end())
		{
			if (((CMidiNote*)*it)->GetChannel() != chane)
			{
				result = true;
				break;
			}
			it++;
		}
		
	}
	return result;
}



vectoreMidinote CMidiTrack::GetMidinoteVector()
{
	return m_notes;
}

CMidiTrack::~CMidiTrack( void )
{
	if (!m_notes.empty())
	{
		for (int i = 0;i<m_notes.size();i++)
		{
			delete m_notes[i];
			m_notes[i] = NULL;
		}
		m_notes.clear();
	}
}




