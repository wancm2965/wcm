// DSOutputFilter.cpp: implementation of the DSOutputFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include "DSOutputFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


struct _GUID g_guidDSOutputFilterNULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DSOutputFilter::DSOutputFilter(void)
:CBaseFilter(NAME("DSOutputFilter"), NULL, this, g_guidDSOutputFilterNULL)
,m_OutputPin(NAME("DSOutputPin"), this, NULL, L"Video Output")
{
}

DSOutputFilter::~DSOutputFilter()
{

}

CBasePin *DSOutputFilter::GetPin(int n)
{
	if (n!=0)
		return NULL;

	return &m_OutputPin;
}

int DSOutputFilter::GetPinCount()
{
	return 1;
}

int DSOutputFilter::SetTitle(const char*cszTitle,unsigned long nColor)
{
	return m_OutputPin.SetTitle(cszTitle,nColor);
}

HRESULT DSOutputFilter::GetImageFormat(int&nWidth,int&nHeight)
{
	return m_OutputPin.GetImageFormat(nWidth,nHeight);
}

HRESULT DSOutputFilter::SetImageFormat(int nWidth, int nHeight)
{
	return m_OutputPin.SetImageFormat(nWidth,nHeight);
}

bool DSOutputFilter::DisplayFrame(unsigned char*pYUV420Data[3],int nStride[3])
{
	return m_OutputPin.DisplayFrame(pYUV420Data,nStride);
}
