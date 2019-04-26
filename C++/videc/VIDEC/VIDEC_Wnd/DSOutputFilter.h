// DSOutputFilter.h: interface for the DSOutputFilter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DSOUTPUTFILTER_H__
#define __DSOUTPUTFILTER_H__

#include "DSOutputPin.h"

class DSOutputFilter : 
	public CCritSec, 
	public CBaseFilter  
{
public:
	DECLARE_IUNKNOWN;
	DSOutputFilter(void);
	~DSOutputFilter();
public:
    CBasePin *GetPin(int n);
    int GetPinCount();

	int SetTitle(const char*cszTitle,unsigned long nColor);

	HRESULT GetImageFormat(int&nWidth,int&nHeight);
	HRESULT SetImageFormat(int nWidth, int nHeight);

	bool DisplayFrame(unsigned char*pYUV420Data[3],int nStride[3]);
protected:
	DSOutputPin		m_OutputPin;
};
 
#endif 