//VBRControl.h
#ifndef __VBRCONTROL_H__
#define __VBRCONTROL_H__

#include "XBitrateExt.h"

class VBRControl
	: public XBitrateExt
{
public:
	VBRControl(void);
	virtual~VBRControl(void);
public:
	void Reset(int nMinQP,int nMaxQP,int nMaxBitrate);
	int GetNextQP(int&nCanEncThisFrame);
protected:
	int				m_nCurQP;
	int				m_nMaxQP;
	int				m_nMinQP;

	int				m_nCurBitrate;
	int				m_nMaxBitrate;
	int				m_nLastBitrate;

	unsigned long	m_ulLastOverflowTimestamp;
	unsigned long	m_ulAdjustTimestamp;
};

#endif
