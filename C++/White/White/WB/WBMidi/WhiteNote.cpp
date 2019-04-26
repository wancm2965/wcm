#include "StdAfx.h"
#include "WhiteNote.h"
#include "Mididata.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CWhiteNote::CWhiteNote( int letter /*= 0*/,int ocave /*= 0*/ )
{
	mletter = letter;
	moctave = ocave;
}

int CWhiteNote::Dist( CWhiteNote* w )
{
	return (moctave - w->moctave)*7 + (mletter - w->mletter);
}

int CWhiteNote::Number()
{
	int notescale;
	switch(mletter)
	{
	case 0:
		notescale = 0;
		break;
	case 1:
		notescale = 2;
		break;
	case 2:
		notescale = 3;
		break;
	case 3:
		notescale = 5;
		break;
	case 4:
		notescale = 7;
		break;
	case 5:
		notescale = 8;
		break;
	case 6:
		notescale = 10;
	default:
		notescale = 0;
		break;

	}
	return 9+notescale + moctave*12;
	

}



CWhiteNote* CWhiteNote::MaxWhiteNote(CWhiteNote* x,CWhiteNote* y)
{
	CWhiteNote* result;
	if (x->Dist(y) > 0)
	{
		result = x;
	}
	else
	{
		result = y;
	}
	return result;
}


CWhiteNote* CWhiteNote::MinWhiteNote(CWhiteNote* x,CWhiteNote* y)
{
	CWhiteNote* result;
	if (x->Dist(y) < 0)
	{
		result = x;
	}
	else
	{
		result = y;
	}
	return result;
}

CWhiteNote* CWhiteNote::TopWhiteNote(Clef clef)
{
	CWhiteNote* result;
	if (clef == Treble)
	{
		result = new CWhiteNote(4,5);
	}
	else
	{
		result = new CWhiteNote(6,3);
	}
	return result;
}

CWhiteNote* CWhiteNote::BottomWhiteNote(Clef clef)
{
	CWhiteNote* result;
	if (clef == Treble)
	{
		result = new CWhiteNote(5,4);
	}
	else
	{
		result =new CWhiteNote(0,3);
	}
	return result;
}

bool CWhiteNote::operator<( const CWhiteNote* m )
{
	return true;
}

CWhiteNote::~CWhiteNote( void )
{

}

CWhiteNote* CWhiteNote::Add( int amount )
{
	CWhiteNote* note;
	int num = moctave*7+mletter;
	num += amount;

	note = new CWhiteNote(num%7,num/7);
	return note;
}


