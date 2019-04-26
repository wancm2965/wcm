#include "StdAfx.h"
#include "MidiFile.h"
#include <algorithm>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif



CMidiFile::CMidiFile( CString filepathname )
{
	EventNoteOff = 128;
	EventNoteOn = 144;
	EventKeyPressure = 160;
	EventControlChange = 176;
	EventProgramChange = 192;
	EventChannelPressure = 208;
	EventPitchBend = 224;
	SysexEvent1 = 240;
	SysexEvent2 = 247;
	MetaEvent = 255;
	MetaEventSequence = 0;
	MetaEventText = 1;
	MetaEventCopyright = 2;
	MetaEventSequenceName = 3;
	MetaEventInstrument = 4;
	MetaEventLyric = 5;
	MetaEventMarker = 6;
	MetaEventEndOfTrack = 47;
	MetaEventTempo = 81;
	MetaEventSMPTEOffset = 84;
	MetaEventTmeSignature = 88;
	MetaEventKeySignature = 89;
	tempo = 0;
	filename = filepathname;
	CMidiFileReader* preade = new CMidiFileReader(filepathname);
	pteimsig = NULL;
	bool btrue = preade->ReadAscii(4,"MThd");
	if (btrue == false)
	{
	}
	int num = preade->ReadInt();
	if (num != 6)
	{
	}
	preade->Skip(2);
	int num2 = (int)preade->Readshort();
	quarternote = preade->Readshort();
	for (int i = 0;i<num2;i++)
	{
		CMidiTrack* miditrack = ReadTrack(preade,i);
		if (miditrack->GetVectorLength() > 0)
		{
			m_tracks.push_back(miditrack);
		}
		else
		{
			delete miditrack;
			miditrack = NULL;
		}

	}
	if (m_tracks.size() == 1 && ((CMidiTrack*)m_tracks[0])->HasMultipleChannels())
	{
		m_tracks = SPlitChannels(m_tracks[0]);
	}
	CheckStartTimes(m_tracks);
	if (tempo == 0)
	{
		tempo = 500000;
	}
	if (pteimsig == NULL)
	{
		pteimsig = new CTimeSignature(4,4,quarternote,tempo);
	}
	else
	{
		int num = pteimsig->GetNumerator();
		int den = pteimsig->GetDenomunator();
		delete pteimsig;
		pteimsig = new CTimeSignature(num,den,quarternote,tempo);
	}
	preade->CloseFile();

	if (preade != NULL)
	{
		delete preade;
		preade = NULL;
	}

	instruments.push_back(_T("Acoustic Grand Piano"));
	instruments.push_back(_T("Bright Acoustic Piano"));
	instruments.push_back(_T("Electric Grand Piano"));
	instruments.push_back(_T("Honky-tonk Piano"));
	instruments.push_back(_T("Electric Piano 1"));
	instruments.push_back(_T("Electric Piano 2"));
	instruments.push_back(_T("Harpsichord"));
	instruments.push_back(_T("Clavi"));
	instruments.push_back(_T("Celesta"));
	instruments.push_back(_T("Glockenspiel"));
	instruments.push_back(_T("Music Box"));
	instruments.push_back(_T("Vibraphone"));
	instruments.push_back(_T("Marimba"));
	instruments.push_back(_T("Xylophone"));
	instruments.push_back(_T("Tubular Bells"));
	instruments.push_back(_T("Dulcimer"));
	instruments.push_back(_T("Drawbar Organ"));
	instruments.push_back(_T("Percussive Organ"));
	instruments.push_back(_T("Rock Organ"));
	instruments.push_back(_T("Church Organ"));
	instruments.push_back(_T("Reed Organ"));
	instruments.push_back(_T("Accordion"));
	instruments.push_back(_T("Harmonica"));
	instruments.push_back(_T("Tango Accordion"));
	instruments.push_back(_T("Acoustic Guitar (nylon)"));
	instruments.push_back(_T("Acoustic Guitar (steel)"));
	instruments.push_back(_T("Electric Guitar (jazz)"));
	instruments.push_back(_T("Electric Guitar (clean)"));
	instruments.push_back(_T("Electric Guitar (muted)"));
	instruments.push_back(_T("Overdriven Guitar"));
	instruments.push_back(_T("Distortion Guitar"));
	instruments.push_back(_T("Guitar harmonics"));
	instruments.push_back(_T("Acoustic Bass"));
	instruments.push_back(_T("Electric Bass (finger)"));
	instruments.push_back(_T("Electric Bass (pick)"));
	instruments.push_back(_T("Fretless Bass"));
	instruments.push_back(_T("Slap Bass 1"));
	instruments.push_back(_T("Slap Bass 2"));
	instruments.push_back(_T("Synth Bass 1"));
	instruments.push_back(_T("Synth Bass 2"));
	instruments.push_back(_T("Violin"));
	instruments.push_back(_T("Viola"));
	instruments.push_back(_T("Cello"));
	instruments.push_back(_T("Contrabass"));
	instruments.push_back(_T("Tremolo Strings"));
	instruments.push_back(_T("Pizzicato Strings"));
	instruments.push_back(_T("Orchestral Harp"));
	instruments.push_back(_T("Timpani"));
	instruments.push_back(_T("String Ensemble 1"));
	instruments.push_back(_T("String Ensemble 2"));
	instruments.push_back(_T("SynthStrings 1"));
	instruments.push_back(_T("SynthStrings 2"));
	instruments.push_back(_T("Choir Aahs"));
	instruments.push_back(_T("Voice Oohs"));
	instruments.push_back(_T("Synth Voice"));
	instruments.push_back(_T("Orchestra Hit"));
	instruments.push_back(_T("Trumpet"));
	instruments.push_back(_T("Trombone"));
	instruments.push_back(_T("Tuba"));
	instruments.push_back(_T("Muted Trumpet"));
	instruments.push_back(_T("French Horn"));
	instruments.push_back(_T("Brass Section"));
	instruments.push_back(_T("SynthBrass 1"));
	instruments.push_back(_T("SynthBrass 2"));
	instruments.push_back(_T("Soprano Sax"));
	instruments.push_back(_T("Alto Sax"));
	instruments.push_back(_T("Tenor Sax"));
	instruments.push_back(_T("Baritone Sax"));
	instruments.push_back(_T("Oboe"));
	instruments.push_back(_T("English Horn"));
	instruments.push_back(_T("Bassoon"));
	instruments.push_back(_T("Clarinet"));
	instruments.push_back(_T("Piccolo"));
	instruments.push_back(_T("Flute"));
	instruments.push_back(_T("Recorder"));
	instruments.push_back(_T("Pan Flute"));
	instruments.push_back(_T("Blown Bottle"));
	instruments.push_back(_T("Shakuhachi"));
	instruments.push_back(_T("Whistle"));
	instruments.push_back(_T("Ocarina"));
	instruments.push_back(_T("Lead 1 (square)"));
	instruments.push_back(_T("Lead 2 (sawtooth)"));
	instruments.push_back(_T("Lead 3 (calliope)"));
	instruments.push_back(_T("Lead 4 (chiff)"));
	instruments.push_back(_T("Lead 5 (charang)"));
	instruments.push_back(_T("Lead 6 (voice)"));
	instruments.push_back(_T("Lead 7 (fifths)"));
	instruments.push_back(_T("Lead 8 (bass + lead)"));
	instruments.push_back(_T("Pad 1 (new age)"));
	instruments.push_back(_T("Pad 2 (warm)"));
	instruments.push_back(_T("Pad 3 (polysynth)"));
	instruments.push_back(_T("Pad 4 (choir)"));
	instruments.push_back(_T("Pad 5 (bowed)"));
	instruments.push_back(_T("Pad 6 (metallic)"));
	instruments.push_back(_T("Pad 7 (halo)"));
	instruments.push_back(_T("Pad 8 (sweep)"));
	instruments.push_back(_T("FX 1 (rain)"));
	instruments.push_back(_T("FX 2 (soundtrack)"));
	instruments.push_back(_T("FX 3 (crystal)"));
	instruments.push_back(_T("FX 4 (atmosphere)"));
	instruments.push_back(_T("FX 5 (brightness)"));
	instruments.push_back(_T("FX 6 (goblins)"));
	instruments.push_back(_T("FX 7 (echoes)"));
	instruments.push_back(_T("FX 8 (sci-fi)"));
	instruments.push_back(_T("Sitar"));
	instruments.push_back(_T("Banjo"));
	instruments.push_back(_T("Shamisen"));
	instruments.push_back(_T("Koto"));
	instruments.push_back(_T("Kalimba"));
	instruments.push_back(_T("Bag pipe"));
	instruments.push_back(_T("Fiddle"));
	instruments.push_back(_T("Shanai"));
	instruments.push_back(_T("Tinkle Bell"));
	instruments.push_back(_T("Agogo"));
	instruments.push_back(_T("Steel Drums"));
	instruments.push_back(_T("Woodblock"));
	instruments.push_back(_T("Taiko Drum"));
	instruments.push_back(_T("Melodic Tom"));
	instruments.push_back(_T("Synth Drum"));
	instruments.push_back(_T("Reverse Cymbal"));
	instruments.push_back(_T("Guitar Fret Noise"));
	instruments.push_back(_T("Breath Noise"));
	instruments.push_back(_T("Seashore"));
	instruments.push_back(_T("Bird Tweet"));
	instruments.push_back(_T("Telephone Ring"));
	instruments.push_back(_T("Helicopter"));
	instruments.push_back(_T("Gunshot"));
	instruments.push_back(_T("Applause"));
	instruments.push_back(_T("Gunshot"));

}



MidiTrackVector CMidiFile::GetTracks()
{
	return m_tracks;
}

CString CMidiFile::EventName( int ev )
{
	CString result = _T("");
	if (ev >= 128 && ev< 144)
	{
		result = _T("NoteOff");
	}
	else
	{
		if (ev >= 144 &&ev< 160)
		{
			result = _T("NoteOn");
		}
		else
		{
			if (ev >= 160 && ev<176)
			{
				result = _T("KeyPressure");
			}
			else
			{
				if (ev >= 176 &&ev<192)
				{
					result = _T("ControlChange");
				}
				else
				{
					if (ev>=192&&ev< 208)
					{
						result = _T("ProgramChange");
					}
					else
					{
						if (ev >= 208 &&ev<224)
						{
							result = _T("ChannekPressure");
						}
						else
						{
							if (ev >=224&&ev<240)
							{
								result = _T("PitchBend");
							}
							else
							{
								if (ev == 255)
								{
									result = _T("MetaEvent");
								}
								else
								{
									if (ev == 240 ||ev == 247)
									{
										result = _T("SysexEvent");
									}
									else
									{
										result = _T("Unknow");
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return result;
}

CMidiTrack* CMidiFile::ReadTrack( CMidiFileReader* pread,int tracknum )
{
	CMidiTrack* miditrack = new CMidiTrack(tracknum);
	int num = 0;
	bool btrue = pread->ReadAscii(4,"MTrk");
	if (btrue == false)
	{
		return NULL;
	}
	int num2 =pread->ReadInt();
	int num3 = num2 + pread->GetOffset();
	int num4 = 0;
	while(pread->GetOffset() < num3)
	{
		int offset = pread->GetOffset();
		int num5 = pread->ReadVarlen();
		num +=num5;
		byte b = pread->Peek();
		if (b >= 128 )
		{
			num4 = (int)pread->ReadByte();
		}
		if (num4 >= 144&&num4<160)
		{
			int channel = num4 -144;
			int notenumber = (int)pread->ReadByte();
			int num6 = (int)pread->ReadByte();
			if (num6 > 0)
			{
				CMidiNote* m = new CMidiNote(num,channel,notenumber,0);
				miditrack->AddNote(m);
			}
			else
			{
				miditrack->NoteOff(channel,notenumber,num);
			}
		}
		else
		{
			if (num4 >= 128 && num4<144)
			{
				int channel = num4 -128;
				int notenumber2 = (int)pread->ReadByte();
				pread->Skip(1);
				miditrack->NoteOff(channel,notenumber2,num);
			}
			else
			{
				if (num4>=160&&num4<176)
				{
					pread->Skip(2);
				}
				else
				{
					if (num4>=176&&num4<192)
					{
						pread->Skip(2);
					}
					else
					{
						if (num4 >= 192&&num4<208)
						{
							int num7 =(int)pread->ReadByte();
							if (num7 >=0 && num7< instruments.size())
							{
								miditrack->SetInstrument(instruments[num7]);
							}
						}
						else
						{
							if (num4 >=208&&num4 < 224)
							{
								pread->Skip(1);
							}
							else
							{
								if (num4 >= 224 &&num4<240)
								{
									pread->Skip(2);
								}
								else
								{
									if (num4 == 240 || num4 == 247)
									{
										int amount = pread->ReadVarlen();
										pread->Skip(amount);
									}
									else
									{
										if (num4 != 255)
										{
										}
										int num8 = ReadMetaEvent(pread,miditrack,num);
										if (num8 == 47)
										{
											break;
										}
									}
								}
							}
						}
					}
				}

			}
		}
	}
	return miditrack;
}

int CMidiFile::ReadMetaEvent( CMidiFileReader* pread,CMidiTrack* midtrack,int starttime )
{
	int num = (int)pread->ReadByte();
	int num2 = pread->ReadVarlen();
	int num3 = num;
	if (num3 != 81)
	{
		if (num3 != 88)
		{
			pread->Skip(num2);
		}
		else
		{
			if (num2 != 4)
			{

			}
			int numerator = (int)pread->ReadByte();
			byte b= pread->ReadByte();
			int denominator = (int)pow(2.0,(double)b);
			pread->Skip(2);
			pteimsig = new CTimeSignature(numerator,denominator,quarternote,0);
		}
	}
	else
	{
		if (num2 != 3)
		{

		}
		int num4 = (int)(pread->ReadByte()<<16);
		num4 |=(int)(pread->ReadByte()<<8);
		num4 |=(int)(pread->ReadByte());
		if (tempo != 0)
		{
			tempo = (tempo + num4)/2;
		}
		else
		{
			tempo = num4;
		}
	}
	return num;
}

MidiTrackVector CMidiFile::SPlitChannels( CMidiTrack* miditrack )
{
	MidiTrackVector tracks;
	for(int i = 0;i< (int)((CMidiTrack*)miditrack)->GetVectorLength();i++)
	{

		bool flag = false;
		CMidiNote* midinote = (CMidiNote*)((vectoreMidinote)miditrack->GetMidinoteVector())[i];
		for (int j = 0;j<(int)(tracks.size());j++)
		{
			CMidiTrack* mdtrack = tracks[j];
			if ((int)(midinote->GetChannel()) == (int)((CMidiNote*)((vectoreMidinote)mdtrack->GetMidinoteVector())[0]->GetChannel()))
			{
				flag = true;
				mdtrack->AddNote(midinote);
				
			}
			
		}
		if (!flag)
		{
			CMidiTrack* mtrack = new CMidiTrack(tracks.size()+1);
			mtrack->AddNote(midinote);
			tracks.push_back(mtrack);
		}
	}
	return tracks;
}

void CMidiFile::TimeShift( MidiTrackVector tracks,int amount )
{
	CMidiTrack* ptrack;
	if (!tracks.empty())
	{
		for(int i = 0;i<(int)tracks.size();i++)
		{
			ptrack = tracks[i];
			vectoreMidinote note = ptrack->GetMidinoteVector();
			if (!note.empty())
			{
				CMidiNote* pnote;
				for (int j = 0;j<(int)note.size();j++)
				{
					pnote = note[j];
					int a =pnote->GetStarttime();
					pnote->SetStarttime(a + amount);
				}
			}

		}
	}
}

void CMidiFile::KeyShift( MidiTrackVector tracks,int amount )
{
	CMidiTrack* ptrack;
	if (!tracks.empty())
	{
		for(int i = 0;i<(int)tracks.size();i++)
		{
			ptrack = tracks[i];
			vectoreMidinote note = ptrack->GetMidinoteVector();
			if (!note.empty())
			{
				CMidiNote* pnote;
				for (int j = 0;j<(int)note.size();j++)
				{
					pnote = note[j];
					int a =pnote->GetNumber();
					pnote->SetNumber(a + amount);
					if (pnote->GetNumber() < 0)
					{
						pnote->SetNumber(0);
					}
				}
			}

		}
	}
}

int CMidiFile::FindHighestNote( vectoreMidinote notes,int starttime,int endtiem )
{
	int result;
	if (notes.empty())
	{
		result = 76;
	}
	else
	{
		int num = 0;
		int endtime = (int)((CMidiNote*)(notes[num])->GetEndTime());
		while((num < (int)(notes.size()))&&(endtime <= starttime))
		{
			
			num++;
			endtime = (int)((CMidiNote*)(notes[num])->GetEndTime());
		}
		if (num == notes.size())
		{
			result = notes[num-1]->GetNumber();
		}
		else
		{
			int number = notes[num]->GetNumber();
			while(num < (int)notes.size()&&notes[num]->GetStarttime() < endtiem)
			{
				if (number < notes[num]->GetNumber())
				{
					number = notes[num]->GetNumber();
				}
				num++;
			}
			result = number;

		}
	}
	return result;
}

int CMidiFile::FindLowestNote( vectoreMidinote notes,int starttime,int endtiem )
{
	int result;
	if (notes.empty())
	{
		result = 45;
	}
	else
	{
		int num = 0;
		while(num < (int)notes.size()&& (int)((CMidiNote*)(notes[num])->GetEndTime()) <= starttime)
		{
			num++;
		}
		if (num == notes.size())
		{
			result = notes[num -1]->GetNumber();
		}
		else
		{
			int  number = notes[num]->GetNumber();
			while(num <(int)notes.size()&&notes[num]->GetStarttime() < endtiem)
			{
				if (number > notes[num]->GetNumber())
				{
					number = notes[num]->GetNumber();
				}
				num++;

			}
			result = number;
		}
	}
	return result;
}

MidiTrackVector CMidiFile::SplitTrack( CMidiTrack * track )
{
	vectoreMidinote notes = track->GetMidinoteVector();
	int count = notes.size();
	CMidiTrack* pmiditrack = new CMidiTrack(1);
	CMidiTrack* pmiditrack2 = new CMidiTrack(2);
	std::vector<CMidiNote*> list;
	std::vector<CMidiTrack*> list2;
	list2.push_back(pmiditrack);
	list2.push_back(pmiditrack2);
	MidiTrackVector result;
	if (count == 0)
	{
		result = list2;
	}
	else
	{
		for(int i = 0;i<(int)notes.size();i++)
		{
			CMidiNote* current = notes[i];
			int num = FindHighestNote(notes,current->GetStarttime(),current->GetEndTime());
			int num2 = FindLowestNote(notes,current->GetStarttime(),current->GetEndTime());
			if ((num - current->GetNumber()) >12 && (current->GetNumber() - num2) > 12)
			{
				if ((num - current->GetNumber()) <= (current->GetNumber() -num2))
				{
					pmiditrack->AddNote(current);
				}
				else
				{
					pmiditrack2->AddNote(current);
				}
			}
			else
			{
				if (num - current->GetNumber() > 12)
				{
					pmiditrack2->AddNote(current);
				}
				else
				{
					if (current->GetNumber() -num2 > 12)
					{
						pmiditrack->AddNote(current);
					}
					else
					{
						list.push_back(current);
					}
				}
			}
		}
		if (!list.empty())
		{
			for (int j = 0;j<(int)list.size();j++)
			{
				CMidiNote* current = list[j];
				int num = FindHighestNote(pmiditrack->GetMidinoteVector(),current->GetStarttime(),current->GetEndTime());
				int num2 = FindLowestNote(pmiditrack2->GetMidinoteVector(),current->GetStarttime(),current->GetEndTime());
				if ((num - current->GetNumber()) <= (current->GetNumber() - num2))
				{
					pmiditrack->AddNote(current);
				}
				else
				{
					pmiditrack2->AddNote(current);
				}
			}
		}
		vectoreMidinote tempvect = pmiditrack->GetMidinoteVector();
		
		std::sort(tempvect.begin(),tempvect.end(),CompareNote);
		pmiditrack->Setmidivector(tempvect);
		vectoreMidinote tempvect2 = pmiditrack2->GetMidinoteVector();
		std::sort(tempvect2.begin(),tempvect2.end(),CompareNote);
		pmiditrack2->Setmidivector(tempvect2);
		
		result = list2;
	}

	return result;
}

CMidiTrack* CMidiFile::CloneTrack( int tracknum )
{
	CMidiTrack* ptrack = new CMidiTrack(tracknum);
	((CMidiTrack*)m_tracks[tracknum])->Clone(ptrack);
	return ptrack;
}

CMidiTrack* CMidiFile::CombineToSingleTrack( MidiTrackVector thetracks )
{
	CMidiTrack* miditrack = new CMidiTrack(1);
	CMidiTrack* result;
	if (thetracks.size() == 0)
	{
		result = miditrack;
	}
	else
	{
		for (int i = 0;i<(int)thetracks.size();i++)
		{
			CMidiTrack* current = thetracks[i];
			vectoreMidinote notes = current->GetMidinoteVector();

			if (!notes.empty())
			{
				for (int j = 0;j<(int)notes.size();j++)
				{
					CMidiNote* midinot = notes[j];
					miditrack->AddNote(midinot);
				}
			}
		}
		vectoreMidinote tempmidinotevec = miditrack->GetMidinoteVector();
		std::sort(tempmidinotevec.begin(),tempmidinotevec.end(),CompareNote);
		for (int k = 0;k<(int)tempmidinotevec.size();k++)
		{
			CMidiNote* midinote1 = tempmidinotevec[k];
			for (int l =k+1;l<(int)tempmidinotevec.size();l++)
			{
				CMidiNote* midinote2 = tempmidinotevec[l];
				if (midinote1->GetStarttime() != midinote2->GetStarttime())
				{
					break;
				}
				if (midinote1->GetStarttime() == midinote2->GetStarttime() && midinote1->GetNumber() == midinote2->GetNumber())
				{

						std::vector<CMidiNote*>::iterator it = tempmidinotevec.begin();
						while (it != tempmidinotevec.end())
						{
							if (*it == midinote2)
							{
								tempmidinotevec.erase(it);
								break;
							}
							else
							{
								it++;
							}
						}
					l--;
				}
			}
		}
		miditrack->Setmidivector(tempmidinotevec);
		result = miditrack;

	}
	return result;
}

MidiTrackVector CMidiFile::CombineToTwoTracks( MidiTrackVector tracks )
{
	CMidiTrack* track = CombineToSingleTrack(tracks);
	return SplitTrack(track);
}

void CMidiFile::CheckStartTimes( MidiTrackVector tracks )
{
	if (!tracks.empty())
	{
		for (int i = 0;i<(int)tracks.size();i++)
		{
			CMidiTrack* current = tracks[i];
			int num = -1;
			vectoreMidinote notes = current->GetMidinoteVector();
			if (!notes.empty())
			{
				for(int j = 0;j<(int)notes.size();j++)
				{
					CMidiNote* current2 = notes[j];
					num = current2->GetStarttime();
				}
			}
		}
	}
}

void CMidiFile::RoundStartTimes( MidiTrackVector tracks,CTimeSignature* tiem )
{
	std::vector<int> list;
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
					CMidiNote* midinote = notes[j];
					list.push_back(midinote->GetStarttime());
				}
			}
		}
		std::sort(list.begin(),list.end());
		int num = tiem->GetQuarter() * 60000/tiem->GetTempo();
		for (int k = 0;k<(int)list.size()-1;k++)
		{
			if (list[k+1]-list[k]<= num)
			{
				list[k+1] = list[k];
			}
		}
		CheckStartTimes(tracks);
		for (int l = 0;l<(int)tracks.size();l++)
		{
			CMidiTrack* current = tracks[l];
			vectoreMidinote notes = current->GetMidinoteVector();
			int number = 0;
			if (!notes.empty())
			{
				for (int h = 0;h<(int)notes.size();h++)
				{
					CMidiNote* current2 = notes[h];
					while(number < (int)list.size() && current2->GetStarttime() - num > list[number])
					{
						number++;
					}
					if (current2->GetStarttime() > list[number]&&current2->GetStarttime() - list[number] <= num)
					{
						current2->SetStarttime(list[number]);
					}
				}
			}
		}
	}
}

void CMidiFile::RoundDurations( MidiTrackVector tracks,int quarernote )
{
	if (!tracks.empty())
	{
		for (int i =0;i<(int)tracks.size();i++)
		{
			CMidiTrack* current = tracks[i];
			vectoreMidinote notes = current->GetMidinoteVector();
			if (!notes.empty())
			{
				for (int j = 0;j<(int)notes.size();j++)
				{
					CMidiNote* midinote = notes[j];
					CMidiNote* midinote2 = midinote;
					for (int k = j+1; k<(int)notes.size();k++)
					{
						midinote2 = notes[k];
						if (midinote->GetStarttime() < midinote2->GetStarttime())
						{
							break;
						}
					}
					int num = midinote2->GetStarttime() - midinote->GetStarttime();
					int num2 = 0;
					if (quarernote <= num)
					{
						num2 = quarernote;
					}
					else
					{
						if (quarernote/2 <= num)
						{
							num2 = quarernote/2;
						}
						else
						{
							if (quarernote/3 <= num)
							{
								num2 = quarernote/3;
							}
							else
							{
								if (quarernote/4 <= num)
								{
									num2 = quarernote/4;
								}
							}
						}
					}
					if (num2 < midinote->GetDuration())
					{
						num2 = midinote->GetDuration();
					}
					midinote->SetDuration(num2);
				}
			}			
		}
	}
}

MidiTrackVector CMidiFile::SplitChannels( CMidiTrack* track )
{
	MidiTrackVector list1;
	vectoreMidinote notes = track->GetMidinoteVector();
	if (!notes.empty())
	{
		for (int i = 0;i<(int)notes.size();i++)
		{
			CMidiNote* current = notes[i];
			bool flag = false;
			if (!list1.empty())
			{
				for (int j = 0;j<(int)list1.size();j++)
				{
					CMidiTrack* miditrack = list1[j];
					if (current->GetChannel() == miditrack->GetMidinoteVector()[0]->GetChannel())
					{
						flag = true;
						miditrack->AddNote(current);
					}
				}
			}
			if (!flag)
			{
				CMidiTrack* mditrack = new CMidiTrack(list1.size() +1);
				mditrack->AddNote(current);
				list1.push_back(mditrack);
			}
		}
	}
	return list1;
	
}

std::vector<int> CMidiFile::GuessMeasureLength()
{
	std::vector<int> list1;
	int num = (int)(1000000.0/(double)pteimsig->GetTempo()*(double)pteimsig->GetQuarter());
	int num2 = num/2;
	int num3 = num*4;
	int num4 = pteimsig->GetMeasure()*5;
	for (int i = 0;i<(int)m_tracks.size();i++)
	{
		CMidiTrack* current = m_tracks[i];
		if (num4 > current->GetMidinoteVector()[0]->GetStarttime())
		{
			num4 = current->GetMidinoteVector()[0]->GetStarttime();
		}
	}
	int num5 = pteimsig->GetQuarter()*60000/pteimsig->GetTempo();
	for (int j = 0;j<(int)m_tracks.size();j++)
	{
		CMidiTrack* current2 = m_tracks[j];
		int num6 = 0;
		vectoreMidinote notes = current2->GetMidinoteVector();
		if (!notes.empty())
		{
			for (int k = 0;k<(int)notes.size();k++)
			{
				CMidiNote* midinote = notes[k];
				if (midinote->GetStarttime() - num6 > num5)
				{
					num6 = midinote->GetStarttime();
					int num7 = midinote->GetStarttime() - num4;
					if (num7 >= num2)
					{
						if (num7 > num3)
						{
							break;
						}
						bool flag = false;
						if (!list1.empty())
						{
							for (int l = 0;l<(int)list1.size();l++)
							{

								if (num7 == list1[l])
								{
									flag = true;
									break;
								}
							}
						}
						
						if (!flag)
						{
							list1.push_back(num7);
						}
						
					}
				}
			}
		}
	}

	std::sort(list1.begin(),list1.end());
	return list1;
}

CMidiFile::~CMidiFile( void )
{
	instruments.clear();
	if (pteimsig != NULL)
	{
		delete pteimsig;
		pteimsig = NULL;
	}
	if (!m_tracks.empty())
	{
		for (int i= 0;i<m_tracks.size();i++)
		{
			delete m_tracks[i];
			m_tracks[i] = NULL;
		}
		m_tracks.clear();
	}

}

CMidiFileReader::CMidiFileReader( CString pathname )
{
	
	if (file != INVALID_HANDLE_VALUE)
	{
		file.Close();
	}
	if (!file.Open(pathname,CFile::modeRead))
	{
		return;
	}
	
	buf = new byte[8];
	file_offset = 0;
	did_peek = false;
}

byte CMidiFileReader::Peek()
{
	if (did_peek)
	{
	}
	did_peek = true;
	file.Read(buf,1);
	return buf[0];
}

void CMidiFileReader::DoRead( int len )
{
	int offset = 0;
	file_offset += len;
	if (did_peek)
	{
		did_peek = false;
		offset = 1;
		len--;
	}
	if (len >0)
	{
		if (len > 8)
		{
			byte* marray = new byte[len +1];
			marray[0] = buf[0];
			buf = marray;
		}
		file.Read(buf,len);
	}
}

byte CMidiFileReader::ReadByte()
{
	DoRead(1);
	return buf[0];
}

short CMidiFileReader::Readshort()
{
	DoRead(2);
	return (short)((int)buf[0]<<8|buf[1]);
}

int CMidiFileReader::ReadInt()
{
	DoRead(4);
	return (int)(buf[0]<<24|buf[1]<<16|buf[2]<<8|buf[3]);
}

bool CMidiFileReader::ReadAscii( int len,char* p )
{
	DoRead(len);
	char s;
	for (int i = 0;i<len;i++)
	{
		char bte = buf[i];
		s = *p;
		if (bte != s)
		{
			return false;
		}
		p++;
	}
	

	return true;
}

int CMidiFileReader::ReadVarlen()
{
	byte b = ReadByte();
	UINT num = (UINT)(b&127);
	for (int i= 0;i<3;i++)
	{
		if ((b&128) == 0)
		{
			break;
		}
		b = ReadByte();
		num = (UINT)(((unsigned long)((unsigned long)num<<7) + ((unsigned long)(long)(b&127))));
	}
	return (int)num;
}

void CMidiFileReader::Skip( int amount )
{
	if (amount != 0)
	{
		int num =amount;
		while(amount >0)
		{
			
			int length = 8;
			num =amount;
			if (length < amount)
			{
				num = length;
			}
			DoRead(num);
			amount -= num;
		}
	}
}

void CMidiFileReader::CloseFile()
{
	file.Close();
}

CMidiFileReader::~CMidiFileReader()
{
	if (buf != NULL)
	{
 		delete buf;
 		buf = NULL;
	}
}

