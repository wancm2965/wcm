#include "StdAfx.h"
#include "WBMidiConvert.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CWBMidiConvert* CWBMidiConvert::pWBMidiconvert = NULL;
CWBMidiConvert::CWBMidiConvert(void)
{
	midifile = NULL;
	sheetmusic = NULL;
	sheetmusicoption = new CSheetMusicOptions();
}

CWBMidiConvert::~CWBMidiConvert(void)
{
	if (midifile != NULL)
	{
		delete midifile;
		midifile = NULL;
	}

	if (sheetmusicoption != NULL)
	{
		delete sheetmusicoption;
		sheetmusicoption = NULL;
	}

	if (sheetmusic != NULL)
	{
		delete sheetmusic;
		sheetmusic = NULL;
	}
}

void CWBMidiConvert::RedrawSheetmusic()
{

	int counts = (int)midifile->GetTracks().size();
	std::vector<bool> optiontrack;
	for (int i=0;i<counts;i++)
	{
		optiontrack.push_back(true);
	}
	sheetmusicoption->Settrack(optiontrack);
	sheetmusicoption->Setlargnotesize(false);
	sheetmusicoption->Settwostaffs(true);
	sheetmusicoption->Settimeshift(0);
	sheetmusicoption->Setshiftkey(0);
	sheetmusicoption->SetTimeSignature(midifile->GetTimeSignature());

	int num = sheetmusicoption->GetTimesignature()->GetNumerator();
	int denom = sheetmusicoption->GetTimesignature()->GetDenomunator();
	int tempo = sheetmusicoption->GetTimesignature()->GetTempo();
	int measure = 480;
	int quarter = measure*sheetmusicoption->GetTimesignature()->GetQuarter()/sheetmusicoption->GetTimesignature()->GetMeasure();
	sheetmusicoption->SetTimeSignature(new CTimeSignature(num,denom,quarter,tempo));


	sheetmusic = new CSheetMusic(midifile,sheetmusicoption);
	sheetmusic->SetZoom(1.0);
	optiontrack.clear();

}

void CWBMidiConvert::OpenMidiFile( CString filename ,int& heiht)
{
	
	midifile = new CMidiFile(filename);	
	
	RedrawSheetmusic();
	int nheight = sheetmusic->GetHeight();
	heiht = nheight;
}

void CWBMidiConvert::Draw( CDC* hc )
{
	if (sheetmusic == NULL)
	{
		return;
	}
	else
	{
		sheetmusic->DrawView(hc);
	}
}

void CWBMidiConvert::SetZoom( float nvalue )
{
	if (sheetmusic == NULL)
	{
		return;
	}
	else
	{
		sheetmusic->SetZoom(nvalue);
	}
}

void CWBMidiConvert::CreateIns()
{
	if (pWBMidiconvert == NULL)
	{
		pWBMidiconvert = new CWBMidiConvert();
	}
}

CWBMidiConvert* CWBMidiConvert::GetIns()
{
	return pWBMidiconvert;
}

void CWBMidiConvert::DestroyIns()
{
	if (pWBMidiconvert != NULL)
	{
		delete pWBMidiconvert;
		pWBMidiconvert = NULL;
	}
}
