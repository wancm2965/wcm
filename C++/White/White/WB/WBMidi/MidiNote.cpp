#include "StdAfx.h"
#include "MidiNote.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CMidiNote::CMidiNote( int start /*= 0*/,int chan /*= 0*/,int noten /*= 0*/,int dur /*= 0*/ )
{
	starttime = start;
	channel = chan;
	notenumber = noten;
	duration = dur;
}



void CMidiNote::NoteOff( int endtime )
{
	duration = endtime - starttime;
}

void CMidiNote::Clone( CMidiNote* other )
{
	other->starttime = starttime;
	other->channel = channel;
	other->notenumber = notenumber;
	other->duration = duration;
}

CMidiNote::~CMidiNote()
{

}



