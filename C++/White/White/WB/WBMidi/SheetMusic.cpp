#include "StdAfx.h"
#include "SheetMusic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif



int CSheetMusic::LineSpace  = 0;
int  CSheetMusic::StaffHeight =0;
int  CSheetMusic::NoteHeight =0;
int  CSheetMusic::NoteWidth =0;
CSheetMusic* CSheetMusic::gsheetmusic = NULL;
CSheetMusic::CSheetMusic( CMidiFile* file,CSheetMusicOptions* options )
{
	LineWidth = 1;
	LeftMargin = 4;
	PageWidth = 800;
	PageHeight = 1050;
	zoom = 1.0;	
	keysignature = NULL;	
	widths = NULL;

	
 	std::vector<CMidiTrack*> list1;
 	for (int i = 0;i<(int)file->GetTracks().size();i++)
 	{
 		std::vector<bool> optiontrack = options->Gettrack();
 		if (!optiontrack.empty())
 		{
 			if (optiontrack[i])
 			{
 				list1.push_back(file->CloneTrack(i));
 			}
 		}
 	}
 // 	if (options->Gettwostaffs())
 // 	{
 // 		list1 = CMidiFile::CombineToTwoTracks(list1);
 // 	}
 	SetNoteSize(options->GetLargnotesize());
//  	if (options->GetTimeshift() != 0)
//  	{
//  		CMidiFile::TimeShift(list1,options->GetTimeshift());
//  	}
//  	if (options->GetKeyShift() != 0)
//  	{
//  		CMidiFile::KeyShift(list1,options->GetKeyShift());
//  	}
 	CTimeSignature* time = file->GetTimeSignature();
 	if (options->GetTimesignature() != NULL)
 	{
 		time = options->GetTimesignature();
 	}
 	CMidiFile::RoundStartTimes(list1,file->GetTimeSignature());
 	CMidiFile::RoundDurations(list1,time->GetQuarter());
 	keysignature = GetKeySignature(list1);
 	numtracks = list1.size();
 	vecvecMusicSymbol array1;
 	for (int j = 0;j<numtracks;j++)
 	{
 		CMidiTrack* miditrack = list1[j];
 		if (miditrack->GetVectorLength() <1)
 		{
 			continue;
 		}
 		CClefMeasures* clefs = new CClefMeasures(miditrack->GetMidinoteVector(),time->GetMeasure());
 		std::vector<ChordSymbol*> chords = CreateChords(miditrack->GetMidinoteVector(),keysignature,time,clefs);
 		vecMusicSymbol tempmuscvec = CreateSymbol(chords,clefs,time);
 		array1.push_back(tempmuscvec);
 
 
 		delete clefs;
 	}
 
 	widths = new CSymbolWidths(array1);
 	AlignSymbols(array1,widths);
 	MakeChordTriplets(array1,time,widths);
 	MakeChordPairs(array1,time,widths);
 	staffs = CreateStaffs(array1,keysignature,time->GetMeasure());
 	SetZoom(1.0);
 	gsheetmusic = this;
 	if (!list1.empty())
 	{
 		for (int i=0;i<list1.size();i++)
 		{
 			delete list1[i];
 			list1[i] = NULL;
 		}
 		list1.clear();
 	}
}

CSheetMusic::~CSheetMusic()
{
	if (!staffs.empty())
	{
		for (int i= 0;i<staffs.size();i++)
		{
			delete staffs[i];
			staffs[i] = NULL;
				
		}
		staffs.clear();
	}

	if (m_memDC.m_hDC != NULL)
	{
		m_memDC.DeleteDC(); 
	}
	if (keysignature != NULL)
	{
		delete keysignature;
		keysignature = NULL;
	}
	if (widths != NULL)
	{
		delete widths;
		widths = NULL;
	}
	CKeySignature::DestroyKeySig();
}


CKeySignature* CSheetMusic::GetKeySignature( MidiTrackVector tracks )
{
	std::vector<int> list1;
	if (!tracks.empty())
	{
		for (int i = 0;i<(int)tracks.size();i++)
		{
			CMidiTrack* current = tracks[i];
			vectoreMidinote notes = current->GetMidinoteVector();
			if (!notes.empty())
			{
				for (int j = 0;j<(int)notes.size();j++)
				{
					CMidiNote* current2 = notes[j];
					list1.push_back(current2->GetNumber());
				}
			}
		}
	}
	CKeySignature* result;
	result =CKeySignature::Guess(list1);
	return result;
}

vectorchordsymbol CSheetMusic::CreateChords( vectoreMidinote midinotes,CKeySignature* key,CTimeSignature* time,CClefMeasures* clefs )
{
	int i =0;
	vectorchordsymbol list1;
	int count = midinotes.size();
	while(i < count)
	{
		int starttime = midinotes[i]->GetStarttime();
		Clef clef = clefs->getClef(starttime);
		vectoreMidinote list2;
		list2.push_back(midinotes[i]);
		i++;
		while(i < count && midinotes[i]->GetStarttime() == starttime)
		{
			list2.push_back(midinotes[i]);
			i++;
		}
		ChordSymbol* item = new ChordSymbol(list2,key,time,clef);
		list1.push_back(item);

	}
	return list1;
}

vecMusicSymbol CSheetMusic::CreateSymbol( vectorchordsymbol chords,CClefMeasures* clefs,CTimeSignature* time )
{
	vecMusicSymbol symbols;
	symbols = AddBars(chords,time);
	symbols = AddRests(symbols,time);
	return AddClefChanges(symbols,clefs,time);


}

vecMusicSymbol CSheetMusic::AddBars( vectorchordsymbol chords,CTimeSignature* time )
{
	vecMusicSymbol list1;
	int num = 0;
	int i =0;
	while(i< (int)chords.size())
	{
		if (num <= chords[i]->GetStartTime())
		{
			list1.push_back(new CBarSymbol(num));
			num += time->GetMeasure();
		}
		else
		{
			list1.push_back(chords[i]);
			i++;
		}
	}
	list1.push_back(new CBarSymbol(num));
	return list1;
}

vecMusicSymbol CSheetMusic::AddRests( vecMusicSymbol chords,CTimeSignature* time )
{
	int num = 0;
	vecMusicSymbol list1 ;
	if (!chords.empty())
	{
		for (int i = 0;i<(int)chords.size();i++)
		{
			CMusicSymbol* current = chords[i];
			int starttime = current->GetStartTime();
			std::vector<CRestSymbol*> rests;
			rests = GetRests(time,num,starttime);
			if (!rests.empty())
			{
				std::vector<CRestSymbol*> aray = rests;
				for (int j = 0;j<(int)rests.size();j++)
				{
					CRestSymbol* item = aray[j];
					list1.push_back(item);
				}
			}
			list1.push_back(current);
			if (current->GetType() == CHORDSYMBOL)
			{
				ChordSymbol* chordsymbol = (ChordSymbol*)current;
				if (chordsymbol->GetEndTime() > num)
				{
					num = chordsymbol->GetEndTime();
				}				
			}
			else
			{
				if (starttime > num)
				{
					num = starttime;
				}
			}
		}
	}
	return list1;
}

std::vector<CRestSymbol*> CSheetMusic::GetRests( CTimeSignature* time,int start,int end )
{
	std::vector<CRestSymbol*> result;
	if (end - start < 0)
	{

	}
	else
	{
		NoteDuration noteduration = time->GetNoteDuration(end - start);
		switch(noteduration)
		{
		case Eighth:
		case Quarter:
		case Half:
		case Whole:
			{
				CRestSymbol* restsymbol = new CRestSymbol(start,noteduration);
				std::vector<CRestSymbol*> aray;
				aray.push_back(restsymbol);
				result = aray;
				break;
			}
		case DottedEighth:
			{
				CRestSymbol* restsymbol = new CRestSymbol(start,Eighth);
				CRestSymbol* restsymbol2 = new CRestSymbol(start + time->GetQuarter()/2,Sixteenth);
				std::vector<CRestSymbol*> aray;
				aray.push_back(restsymbol);
				aray.push_back(restsymbol2);
				result = aray;
				break;
			}
		case DottedQuarter:
			{
				CRestSymbol* restsymbol = new CRestSymbol(start,Quarter);
				CRestSymbol* restsymbol2 = new CRestSymbol(start + time->GetQuarter(),Eighth);
				std::vector<CRestSymbol*> aray;
				aray.push_back(restsymbol);
				aray.push_back(restsymbol2);
				result = aray;
				break;
			}
		case DottedHalf:
			{
				CRestSymbol* restsymbol = new CRestSymbol(start,Half);
				CRestSymbol* restsymbol2 = new CRestSymbol(start + time->GetQuarter()*2,Quarter);
				std::vector<CRestSymbol*> aray;
				aray.push_back(restsymbol);
				aray.push_back(restsymbol2);
				result = aray;
				break;
			}
		default:
			break;
		}
	}
	return result;
}

vecMusicSymbol CSheetMusic::AddClefChanges( vecMusicSymbol symbols,CClefMeasures* clefs,CTimeSignature* time )
{
	vecMusicSymbol list1;
	Clef clef = clefs->getClef(0);
	if (!symbols.empty())
	{
		for (int i = 0;i<(int)symbols.size();i++)
		{
			CMusicSymbol* current = symbols[i];
			if (current->GetType() == BARSYMBOL)
			{
				Clef clef2 = clefs->getClef(current->GetStartTime());
				if (clef2 != clef)
				{
					list1.push_back(new CClefSymbol(clef2,current->GetStartTime()-1,true));
				}
				clef = clef2;

			}
			list1.push_back(current);
		}
	}
	return list1;
}

void CSheetMusic::AlignSymbols( vecvecMusicSymbol& allsymbols,CSymbolWidths* widths )
{
	for (int i = 0;i<(int)allsymbols.size();i++)
	{
		vecMusicSymbol list1 = allsymbols[i];
		vecMusicSymbol list2;
		int j = 0;
		std::vector<int> startTimes = widths->GetSTarttimes();
		for (int k = 0;k<(int)startTimes.size();k++)
		{
			int num = startTimes[k];
			if (j < (int)list1.size() && list1[j]->GetStartTime() == num)
			{
				while(j < (int)list1.size() && list1[j]->GetStartTime() == num)
				{
					list2.push_back(list1[j]);
					j++;
				}
			}
			else
			{
				list2.push_back(new CBlankSymbol(num,0));
			}
		}
		j = 0;
		while(j < (int)list2.size())
		{
			int num = list2[j]->GetStartTime();
			int extraWidth = widths->GetEctraWidth(i,num);
			list2[j]->SetWidth(list2[j]->GetWidth()+extraWidth);
			while(j < (int)list2.size() && list2[j]->GetStartTime() == num)
			{
				j++;
			}
			
		}
		allsymbols[i] = list2;
	}
}

bool CSheetMusic::IsChord( CMusicSymbol* m )
{
	if (m->GetType() == CHORDSYMBOL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CSheetMusic::MakeChordTriplets( vecvecMusicSymbol& allsymbols,CTimeSignature* time,CSymbolWidths* widths )
{
	for (int i = 0;i<(int)allsymbols.size();i++)
	{
		vecMusicSymbol list1 = allsymbols[i];
		vecMusicSymbol list2;
		for (int k = 0;k<(int)list1.size();k++)
		{
			CMusicSymbol* m = list1[k];
			if (IsChord(m))
			{
				list2.push_back(m);
			}
		}
		int j = 0;
		while(j<(int)list2.size()-2)
		{
			ChordSymbol* chordsymbol = (ChordSymbol*)list2[j];
			ChordSymbol* c = (ChordSymbol*)list2[j+1];
			ChordSymbol* chordsymbol2 = (ChordSymbol*)list2[j+2];
			if (ChordSymbol::CanMakeTriplet(chordsymbol,c,chordsymbol2,time))
			{
				int widthbetween = widths->GetWidthBetween(chordsymbol->GetStartTime(),chordsymbol2->GetStartTime());
				ChordSymbol::MakeTriplet(chordsymbol,c,chordsymbol2,widthbetween);
				j +=3;
			}
			else
			{
				j++;
			}
		}
	}
}

void CSheetMusic::MakeChordPairs( vecvecMusicSymbol& allsymbols,CTimeSignature* time,CSymbolWidths* widths )
{
	for (int i = 0;i<(int)allsymbols.size();i++)
	{
		vecMusicSymbol list1 = allsymbols[i];
		vecMusicSymbol list2;
		for (int k = 0;k<(int)list1.size();k++)
		{
			CMusicSymbol* m = list1[k];
			if (IsChord(m))
			{
				list2.push_back(m);
			}
		}
		int j = 0;
		while(j<(int)list2.size()-1)
		{
			ChordSymbol* chordsymbol = (ChordSymbol*)list2[j];
			ChordSymbol* c = (ChordSymbol*)list2[j+1];
			if (ChordSymbol::CanMakePair(chordsymbol,c,time))
			{
				int widthbetween = widths->GetWidthBetween(chordsymbol->GetStartTime(),c->GetStartTime());
				ChordSymbol::MakePair(chordsymbol,c,widthbetween);
				j +=2;
			}
			else
			{
				j++;
			}
		}
	}
}

int CSheetMusic::KeySignatureWidth( CKeySignature* key )
{
	CClefSymbol* clefsymbol = new CClefSymbol(Treble,0,false);
	int num = clefsymbol->GetMinWidth();
	std::vector<CAccidSymbol*> symbols = key->GetSymbol(Treble);
	for (int i =0;i<(int)symbols.size(); i++)
	{
		CAccidSymbol* accidsymbol = symbols[i];
		num += accidsymbol->GetMinWidth(); 
	}
	delete clefsymbol;
	clefsymbol = NULL;
	return num +9;
}

std::vector<CStaff*> CSheetMusic::CreateStaffs( vecvecMusicSymbol allsymbols,CKeySignature* key,int measure )
{
	std::vector<CStaff*> array2;
	std::vector<std::vector<CStaff*>> array1;
	int num = KeySignatureWidth(key);
	int num2 = allsymbols.size();
	std::vector<CStaff*> vecstaff;
	for (int i =0;i<num2;i++)
	{
		/*array1[]*/
		vecMusicSymbol list1 = allsymbols[i];
		int num3 ;
		for (int j = 0;j<(int)list1.size();j = num3 +1)
		{
			num3 = j;
			int num4 = num;
			while(num3 < (int)list1.size() && num4 + list1[num3]->GetWidth() < 800)
			{
				num4 += list1[num3]->GetWidth();
				num3++;
			}
			num3--;
			if (num3 != list1.size() -1)
			{
				if (list1[j]->GetStartTime()/measure != list1[num3]->GetStartTime()/measure)
				{
					int num5 = list1[num3 + 1]->GetStartTime()/measure;
					while(list1[num3]->GetStartTime()/measure == num5)
					{
						num3--;
					}
				}
			}
			int size = num3 +1 - j;
			vecMusicSymbol list2;
			for (int count = 0;count <size;count++)
			{
				list2.push_back(list1[j+count]);
			}
			CStaff* item = new CStaff(list2,key,i,num2);
			
			vecstaff.push_back(item);
			
			
		}
		array1.push_back(vecstaff);
		vecstaff.clear();
	}

	int num6 = 0;
	for (int k = 0;k<(int)array1.size();k++)
	{
		
		if (num6 <(int)array1[k].size())
		{
			num6 = array1[k].size();
		}
	}
	std::vector<CStaff*> list2;
	for (int l = 0;l<num6;l++)
	{
		for (int h = 0;h<(int)array1.size();h++)
		{
			array2 = array1[h];
			if (l<(int)array2.size())
			{
				list2.push_back(array2[l]);
			}
		}
	}
	return list2;

}

void CSheetMusic::SetZoom( float nvalue )
{
	zoom = nvalue;

}

void CSheetMusic::SetNoteSize( bool largenotes )
{
	if (largenotes)
	{
		LineSpace = 7;
	}
	else
	{
		LineSpace = 5;
	}
	StaffHeight = LineSpace*4 +5;
	NoteHeight = LineSpace+1;
	NoteWidth = 3*LineSpace/2;

}

void CSheetMusic::DrawView( CDC* hc )
{	
	GdiplusStartupInput gdiplusStartupinput;
	
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken,&gdiplusStartupinput,NULL);
	if (m_memDC.m_hDC != NULL)
	{
		m_memDC.DeleteDC();
	}
	if(m_bmpBuf.m_hObject != NULL)
	{
		m_bmpBuf.DeleteObject();
	}
	m_memDC.CreateCompatibleDC(hc);
	int nheightStaffs = GetHeight();
	int num = 10/zoom;
	int num2 = num + nheightStaffs/zoom;
	BITMAPINFOHEADER bmih;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = 800;
	bmih.biHeight = nheightStaffs;
	bmih.biPlanes = 1;
	bmih.biCompression = BI_RGB;
	bmih.biBitCount=24;

	BITMAPINFO bmi;
	memset(&bmi,0,sizeof(BITMAPINFO));
	bmi.bmiHeader = bmih;

	void* p;
	HBITMAP hBitmap = ::CreateDIBSection(hc->GetSafeHdc(),&bmi,DIB_RGB_COLORS,&p,NULL,0);
	m_bmpBuf.Attach(hBitmap);
	if (m_bmpBuf.m_hObject == NULL)
	{
		return;
	}

	CBitmap* poldbitmap = m_memDC.SelectObject(&m_bmpBuf);
	if (::DeleteObject(hBitmap))
	{
		hBitmap = NULL;
	}
	
	BITMAP bmpinfo;
	m_bmpBuf.GetBitmap(&bmpinfo);
	m_memDC.FillSolidRect(0,0,800,nheightStaffs,RGB(255,255,255));


	
	Graphics* g = Graphics::FromHDC(m_memDC.m_hDC);
	g->SetSmoothingMode(SmoothingModeHighQuality);
	int num3 =0;
	for (int i = 0;i<(int)staffs.size();i++)
	{
		
		CStaff* staff = staffs[i];
		if (num3 + staff->GetHeight() >= num && num3 <= num2)
		{
			g->TranslateTransform(0,num3);
			staff->Draw(g);
			g->TranslateTransform(0,-num3);
		}
		num3 += staff->GetHeight();
	}

	delete g;
	g = NULL;
	::BitBlt(hc->m_hDC,0,0,bmpinfo.bmWidth,bmpinfo.bmHeight,m_memDC.m_hDC,0,0,SRCCOPY);

	m_memDC.SelectObject(poldbitmap);
	m_bmpBuf.DeleteObject();
	if (m_memDC.m_hDC && m_memDC.GetSafeHdc())
	{
		DeleteDC(m_memDC.GetSafeHdc());
	}
	GdiplusShutdown(gdiplusToken);
}

CSheetMusic* CSheetMusic::Getsheetmusic()
{
	return gsheetmusic;
}

int CSheetMusic::GetHeight()
{
	int num3 =0;
	for (int i = 0;i<(int)staffs.size();i++)
	{

		CStaff* staff = staffs[i];		
		num3 += staff->GetHeight();
	}

	return num3;

}