#include "StdAfx.h"
#include "KeySignature.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


Accid** CKeySignature::flatkeys = NULL;
Accid** CKeySignature::sharpkeys = NULL;

CKeySignature::CKeySignature( int sharps,int flats )
{
	C = 0;
	G = 1;
	D = 2;
	A = 3;
	E = 4;
	B = 5;
	F = 1;
	Bflat = 2;
	Eflat = 3;
	Aflat = 4;
	Dflat = 5;
	Gflat = 6;
	num_sharps = sharps;
	num_flats = flats;
	CreateAccidentalMaps();	
	ResetKeyMap();
	CreateSymbol();

}

void CKeySignature::CreateAccidentalMaps()
{
	if (sharpkeys == NULL)
	{
		sharpkeys = new Accid *[8];
		flatkeys = new Accid *[8];
		for (int i = 0;i<8;i++)
		{
			sharpkeys[i] = new Accid [12];
			flatkeys[i] = new Accid [12];
		}
		Accid* array = sharpkeys[0];
		array[0] = None;
		array[1] = Sharp;
		array[2] = None;
		array[3] = None;
		array[4] = Sharp;
		array[5] = None;
		array[6] = Sharp;
		array[7] = None;
		array[8] = None;
		array[9] = Sharp;
		array[10] = None;
		array[11] = Sharp;
		array = sharpkeys[1];
		array[0] = None;
		array[1] = Sharp;
		array[2] = None;
		array[3] = None;
		array[4] = Sharp;
		array[5] = None;
		array[6] = Sharp;
		array[7] = None;
		array[8] = Natural;
		array[9] = None;
		array[10] = None;
		array[11] = Sharp;
		array = sharpkeys[2];
		array[0] = None;
		array[1] = Sharp;
		array[2] = None;
		array[3] = Natural;
		array[4] = None;
		array[5] = None;
		array[6] = Sharp;
		array[7] = None;
		array[8] = Natural;
		array[9] = None;
		array[10] = None;
		array[11] = Sharp;
		array = sharpkeys[3];
		array[0] = None;
		array[1] = Sharp;
		array[2] = None;
		array[3] = Natural;
		array[4] = None;
		array[5] = None;
		array[6] = Sharp;
		array[7] = None;
		array[8] = Natural;
		array[9] = None;
		array[10] = Natural;
		array[11] = None;
		array = sharpkeys[4];
		array[0] = None;
		array[1] = Sharp;
		array[2] = None;
		array[3] = Natural;
		array[4] = None;
		array[5] = Natural;
		array[6] = None;
		array[7] = None;
		array[8] = Natural;
		array[9] = None;
		array[10] = Natural;
		array[11] = None;
		array = sharpkeys[5];
		array[0] = Natural;
		array[1] = None;
		array[2] = None;
		array[3] = Natural;
		array[4] = None;
		array[5] = Natural;
		array[6] = None;
		array[7] = None;
		array[8] = Natural;
		array[9] = None;
		array[10] = Natural;
		array[11] = None;
		array = flatkeys[0];
		array[0] = None;
		array[1] = Flat;
		array[2] = None;
		array[3] = None;
		array[4] = Flat;
		array[5] = None;
		array[6] = Flat;
		array[7] = None;
		array[8] = None;
		array[9] = Flat;
		array[10] = None;
		array[11] = Flat;
		array = flatkeys[1];
		array[0] = None;
		array[1] = None;
		array[2] = Natural;
		array[3] = None;
		array[4] = Flat;
		array[5] = None;
		array[6] = Flat;
		array[7] = None;
		array[8] = None;
		array[9] = Flat;
		array[10] = None;
		array[11] = Flat;
		array = flatkeys[2];
		array[0] = None;
		array[1] = None;
		array[2] = Natural;
		array[3] = None;
		array[4] = Flat;
		array[5] = None;
		array[6] = None;
		array[7] = Natural;
		array[8] = None;
		array[9] = Flat;
		array[10] = None;
		array[11] = Flat;
		array = flatkeys[3];
		array[0] = Natural;
		array[1] = None;
		array[2] = Natural;
		array[3] = None;
		array[4] = Flat;
		array[5] = None;
		array[6] = None;
		array[7] = Natural;
		array[8] = None;
		array[9] = Flat;
		array[10] = None;
		array[11] = None;
		array = flatkeys[4];
		array[0] = Natural;
		array[1] = None;
		array[2] = Natural;
		array[3] = None;
		array[4] = None;
		array[5] = Natural;
		array[6] = None;
		array[7] = Natural;
		array[8] = None;
		array[9] = Flat;
		array[10] = None;
		array[11] = None;
		array = flatkeys[5];
		array[0] = Natural;
		array[1] = None;
		array[2] = Natural;
		array[3] = None;
		array[4] = None;
		array[5] = Natural;
		array[6] = None;
		array[7] = Natural;
		array[8] = None;
		array[9] = None;
		array[10] = Natural;
		array[11] = None;
		array = flatkeys[6];
		array[0] = Natural;
		array[1] = None;
		array[2] = None;
		array[3] = Natural;
		array[4] = None;
		array[5] = Natural;
		array[6] = None;
		array[7] = Natural;
		array[8] = None;
		array[9] = None;
		array[10] = Natural;
		array[11] = None;
	}
}

void CKeySignature::ResetKeyMap()
{
	Accid* array1 ;
	if (num_flats > 0)
	{
		array1 = flatkeys[num_flats];
	}
	else
	{
		array1 = sharpkeys[num_sharps];
	}
	for (int i = 0;i<128;i++)
	{
		int num = (i + 3)%12;
		m_keymap[i] = array1[num];
	}
}

void CKeySignature::CreateSymbol()
{
	int num = num_sharps;
	if (num_flats > num_sharps)
	{
		num = num_flats;
			 
	}
	

	if (num != 0)
	{

		std::vector<CWhiteNote*> array1;
		std::vector<CWhiteNote*> array2;
		if (num_sharps > 0)
		{
			CWhiteNote* pnote;
			pnote = new CWhiteNote(5,5);
			array1.push_back(pnote);
			pnote = new CWhiteNote(2,5);
			array1.push_back(pnote);
			pnote = new CWhiteNote(6,5);
			array1.push_back(pnote);
			pnote = new CWhiteNote(3,5);
			array1.push_back(pnote);
			pnote = new CWhiteNote(0,6);
			array1.push_back(pnote);
			pnote = new CWhiteNote(4,5);
			array1.push_back(pnote);
			pnote = new CWhiteNote(5,3);
			array2.push_back(pnote);
			pnote = new CWhiteNote(2,3);
			array2.push_back(pnote);
			pnote = new CWhiteNote(6,3);
			array2.push_back(pnote);
			pnote = new CWhiteNote(3,3);
			array2.push_back(pnote);
			pnote = new CWhiteNote(0,4);
			array2.push_back(pnote);
			pnote = new CWhiteNote(4,3);
			array2.push_back(pnote);

		}
		else
		{
			if (num_flats > 0)
			{
				CWhiteNote* pnote;
				pnote = new CWhiteNote(1,5);
				array1.push_back(pnote);
				pnote = new CWhiteNote(4,5);
				array1.push_back(pnote);
				pnote = new CWhiteNote(0,5);
				array1.push_back(pnote);
				pnote = new CWhiteNote(3,5);
				array1.push_back(pnote);
				pnote = new CWhiteNote(6,4);
				array1.push_back(pnote);
				pnote = new CWhiteNote(2,5);
				array1.push_back(pnote);
				pnote = new CWhiteNote(1,3);
				array2.push_back(pnote);
				pnote = new CWhiteNote(4,3);
				array2.push_back(pnote);
				pnote = new CWhiteNote(0,3);
				array2.push_back(pnote);
				pnote = new CWhiteNote(3,3);
				array2.push_back(pnote);
				pnote = new CWhiteNote(6,2);
				array2.push_back(pnote);
				pnote = new CWhiteNote(2,3);
				array2.push_back(pnote);
			}
		}

		Accid accid;
		if (num_sharps>0)
		{
			accid = Sharp;
		}
		else
		{
			accid = Flat;
		}
		for (int i = 0;i<num;i++)
		{
			CWhiteNote* pwhitenoteTemp = new CWhiteNote;
			pwhitenoteTemp->SetLetter(array1[i]->GetLetter());
			pwhitenoteTemp->SetOctave(array1[i]->GetOctave());
			CAccidSymbol* paccidsymbol =NULL;
			paccidsymbol = new CAccidSymbol(accid,pwhitenoteTemp,Treble);
			m_treble.push_back(paccidsymbol);
			pwhitenoteTemp = new CWhiteNote;
			pwhitenoteTemp->SetLetter(array2[i]->GetLetter());
			pwhitenoteTemp->SetOctave(array2[i]->GetOctave());
			paccidsymbol = new CAccidSymbol(accid,pwhitenoteTemp,Bass);
			m_bass.push_back(paccidsymbol);
		}

		if (!array1.empty())
		{
			for (int i = 0;i<array1.size();i++)
			{
				delete array1[i];
				array1[i] = NULL;
			}
			array1.clear();
		}
		if (!array2.empty())
		{
			for (int i = 0;i<array2.size();i++)
			{
				delete array2[i];
				array2[i] = NULL;
			}
			array2.clear();
		}


	}
	

}

std::vector<CAccidSymbol*> CKeySignature::GetSymbol( Clef clef )
{
	std::vector<CAccidSymbol*> result;
	if (clef == Treble)
	{
		result = m_treble;
	}
	else
	{
		result = m_bass;
	}
	return result;
}

Accid CKeySignature::GetAccidental( int notenumber,int measure )
{
	if (measure != prevmeasure)
	{
		ResetKeyMap();
		prevmeasure = measure;
	}
	Accid accid = (Accid)m_keymap[notenumber];
	if (accid == Sharp)
	{
		m_keymap[notenumber] = None;
		m_keymap[notenumber -1] = Natural;
	}
	else
	{
		if (accid == Flat)
		{
			m_keymap[notenumber] = None;
			m_keymap[notenumber +1] = Natural;
		}
		else
		{
			if (accid == Natural && num_flats > 0)
			{
				m_keymap[notenumber] = None;
				m_keymap[notenumber -1] = Flat;
			}
			else
			{
				if (accid == Natural && num_sharps >= 0)
				{
					m_keymap[notenumber] = None;
					m_keymap[notenumber +1] = Sharp;
				}
			}
		}
	}
	return accid;
}

CWhiteNote* CKeySignature::GetWhiteNote( int notenumber )
{
	int num = (notenumber +3)%12;
	int num2 = (notenumber +3)/12 -1;
	int array1[12]={0,0,1,2,2,3,3,4,5,5,6,6};
	int array2[12]={0,1,1,2,3,3,4,4,5,6,6,0};
	int num3;
	if (num_flats>0)
	{
		num3 = array2[num];
	}
	else
	{
		num3 = array1[num];
	}
	if (num_flats == 6 && num == 2)
	{
		num3++;
	}
	else
	{
		if (num_flats>0&&num ==11)
		{
			num2++;
		}
	}

	return new CWhiteNote(num3,num2);
}

CKeySignature* CKeySignature::Guess( VecIntwidth notes )
{
	CKeySignature* presult;
	CreateAccidentalMaps();
	int array1[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	for (int i = 0;i<(int)notes.size();i++)
	{
		int num = notes[i];
		int num2 = (num+3)%12;
		array1[num2] +=1;
	}
	int num3 = 0;
	bool flag = true;
	int num4 = notes.size();
	for (int j = 0;j<6;j++)
	{
		int num5 = 0;
		for (int k = 0;k<12;k++)
		{
			if (sharpkeys[j][k] != None)
			{
				num5 += array1[k];
			}
		}
		if (num5 < num4)
		{
			num4 = num5;
			num3 = j;
			flag = true;
		}
	}
	for (int j = 0;j<7;j++)
	{
		int num5 = 0;
		for (int k = 0;k<12;k++)
		{
			if (flatkeys[j][k] != None)
			{
				num5 += array1[k];
			}
		}
		if (num5 < num4)
		{
			num4 = num5;
			num3 = j;
			flag = false;
		}
	}
	if (flag)
	{
		presult = new CKeySignature(num3,0);
	}
	else
	{
		presult = new CKeySignature(0,num3);
	}
	return presult;
}

bool CKeySignature::Equals( CKeySignature* k )
{
	return k->num_sharps == num_sharps &&k->num_flats == num_flats;
}

int CKeySignature::Notescale()
{
	int array1[8]={3,8,1,6,11,4,9,2};
	int array2[10]={3,10,5,0,7,2,9,4,11,6};
	int result;
	if (num_flats>0)
	{
		result = array1[num_flats];
	}
	else
	{
		result = array2[num_sharps];
	}
	return result;
}

CString CKeySignature::KeyToString( int notescale )
{
	CString strresult= _T("");
	switch(notescale)
	{
	case 0:
		{
			strresult= _T("A major");
		}
		break;
	case 1:
		{
			strresult= _T("B-flat major");
		}
		break;
	case 2:
		{
			strresult= _T("B major");
		}
		break;
	case 3:
		{
			strresult= _T("C major");
		}
		break;
	case 4:
		{
			strresult= _T("D-flat major");
		}
		break;
	case 5:
		{
			strresult= _T("D major");
		}
		break;
	case 6:
		{
			strresult= _T("E-flat major");
		}
		break;
	case 7:
		{
			strresult= _T("E major");
		}
		break;
	case 8:
		{
			strresult= _T("F major");
		}
		break;
	case 9:
		{
			strresult= _T("G-flat major");
		}
		break;
	case 10:
		{
			strresult= _T("G major");
		}
		break;
	case 11:
		{
			strresult= _T("A-flat major");
		}
		break;
	default:
		strresult = _T("");
		break;
	}
	return strresult;
}

CKeySignature::~CKeySignature( void )
{
	if (!m_bass.empty())
	{
		for(int i = 0;i<m_bass.size();i++)
		{
			delete m_bass[i];
			m_bass[i] = NULL;
		}

		m_bass.clear();
	}

	if (!m_treble.empty())
	{
		for(int i = 0;i<m_treble.size();i++)
		{
			delete m_treble[i];
			m_treble[i] = NULL;
		}
		m_treble.clear();
	}


	




}

void CKeySignature::DestroyKeySig()
{
	if (sharpkeys != NULL)
	{
		for (int i =0;i<8;i++)
		{
			delete[]  sharpkeys[i];
		}
		delete[] sharpkeys;
		sharpkeys = NULL;
	}

	if (flatkeys != NULL)
	{
		for ( int i=0;i<8;i++)
		{
			delete[] flatkeys[i];
		}
		delete[] flatkeys;
		flatkeys = NULL;
	}
}

