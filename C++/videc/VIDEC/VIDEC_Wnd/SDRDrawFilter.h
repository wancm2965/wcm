// SDRDrawFilter.h: interface for the SDRDrawFilter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SDRDRAWFILTER_H__
#define __SDRDRAWFILTER_H__

#include "SDRDrawPin.h"

class SDRDrawFilter : 
	public CCritSec, 
	public CBaseFilter  
{
public:
	DECLARE_IUNKNOWN;
	SDRDrawFilter(void);
	~SDRDrawFilter();
public:
    CBasePin *GetPin(int n);
    int GetPinCount();
	void SetImageFormat(int nWidth,int nHeight);
	bool DeliverData(unsigned char*szData,int nLen);
protected:
	SDRDrawPin		m_SDRDrawPin;
};
 
#endif 