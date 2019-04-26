// VideoDrawFilter.cpp: implementation of the VideoDrawFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include "VideoDrawFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


struct _GUID g_guidVideoDrawFilterNULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VideoDrawFilter::VideoDrawFilter(int nWidth,int nHeight,bool bNV12)
:CBaseFilter(NAME("VideoDrawFilter"), NULL, this, g_guidVideoDrawFilterNULL)
,m_VideoDrawPin(nWidth,nHeight,"VideoDrawPin", this, NULL, L"Video Output",bNV12)
{
}

VideoDrawFilter::~VideoDrawFilter()
{

}

CBasePin *VideoDrawFilter::GetPin(int n)
{
	if (n!=0)
		return NULL;

	return &m_VideoDrawPin;
}

int VideoDrawFilter::GetPinCount()
{
	return 1;
}

int VideoDrawFilter::Draw(unsigned char*pYUV,int nStride)
{
	return m_VideoDrawPin.Draw(pYUV,nStride);
}
