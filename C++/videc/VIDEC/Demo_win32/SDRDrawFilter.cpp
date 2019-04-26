// SDRDrawFilter.cpp: implementation of the SDRDrawFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include "SDRDrawFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


struct _GUID g_guidAVVideoPlayerNULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SDRDrawFilter::SDRDrawFilter(void)
:CBaseFilter(_T("SDRDrawFilter"), NULL, this, g_guidAVVideoPlayerNULL)
,m_SDRDrawPin(("SDRDrawPin"), this, NULL, _T("Video Output"))
{
}

SDRDrawFilter::~SDRDrawFilter()
{

}

CBasePin *SDRDrawFilter::GetPin(int n)
{
	if (n!=0)
		return NULL;

	return &m_SDRDrawPin;
}

int SDRDrawFilter::GetPinCount()
{
	return 1;
}

void SDRDrawFilter::SetImageFormat(int nWidth,int nHeight)
{
	m_SDRDrawPin.SetImageFormat(nWidth,nHeight);
}

bool SDRDrawFilter::DeliverData(unsigned char*szData,int nLen)
{
	return m_SDRDrawPin.DisplayFrame(szData,nLen);
}
