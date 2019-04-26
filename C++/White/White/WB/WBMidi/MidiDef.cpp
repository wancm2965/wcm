#include "stdafx.h"
#include "MidiDef.h"


bool CompareNote(CMidiNote*x,CMidiNote*y)
{
	return (x->GetStarttime() < y->GetStarttime()); 
}

bool SortNotes( CMidiNote*x,CMidiNote*y )
{
	return (x->GetNumber() < y->GetNumber());
}

Bitmap* DrawtrableClefImage()
{
	TCHAR szpath[MAX_PATH];
	GetModuleFileName(NULL,szpath,MAX_PATH);
	CString strpath = szpath;
	int nPos;
	nPos = strpath.ReverseFind('\\');
	strpath = strpath.Left(nPos);
	strpath +=_T("\\skin\\WhiteBoard\\");
	CString temppath = strpath;
	Bitmap* treble = NULL;
	
	strpath += _T("treble.bmp");

	treble = new Bitmap(strpath);
	return treble;
	
}
Bitmap* DrawbassClefImage()
{
	TCHAR szpath[MAX_PATH];
	GetModuleFileName(NULL,szpath,MAX_PATH);
	CString strpath = szpath;
	int nPos;
	nPos = strpath.ReverseFind('\\');
	strpath = strpath.Left(nPos);
	strpath +=_T("\\skin\\WhiteBoard\\");
	CString temppath = strpath;
	Bitmap* bass = NULL;
	
	temppath += _T("bass.bmp");
	bass = new Bitmap(temppath);
	return bass;
	
}