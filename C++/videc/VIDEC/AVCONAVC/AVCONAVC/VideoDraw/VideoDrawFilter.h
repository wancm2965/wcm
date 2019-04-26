// VideoDrawFilter.h: interface for the VideoDrawFilter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VIDEODRAWFILTER_H__
#define __VIDEODRAWFILTER_H__

#include "VideoDrawPin.h"

class VideoDrawFilter : 
	public CCritSec, 
	public CBaseFilter  
{
public:
	DECLARE_IUNKNOWN;
	VideoDrawFilter(int nWidth,int nHeight,bool bNV12);
	~VideoDrawFilter();
public:
    CBasePin *GetPin(int n);
    int GetPinCount();
	int Draw(unsigned char*pYUV,int nStride);
protected:
	VideoDrawPin		m_VideoDrawPin;
};
 
#endif 