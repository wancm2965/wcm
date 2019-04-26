#include "StdAfx.h"
#include "SheetMusicOptions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CSheetMusicOptions::CSheetMusicOptions()
{
	//tracks = NULL;
	larg_note_size = false;
	two_staffs = false;
	timeshift = 0;
	shiftkey = 0;
	ptime = NULL;
}

CSheetMusicOptions::~CSheetMusicOptions( void )
{
	if (ptime != NULL)
	{
		delete ptime;
		ptime = NULL;
	}
	if (!tracks.empty())
	{
		tracks.clear();
	}


}
