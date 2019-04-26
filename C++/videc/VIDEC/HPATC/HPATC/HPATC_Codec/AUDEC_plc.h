//AUDEC_plc.h
#ifndef __AUDEC_PLC_H__
#define __AUDEC_PLC_H__

#include "plc.h"

class AUDEC_plc
{
public:
	AUDEC_plc(void);
	virtual~AUDEC_plc(void);
public:
	virtual int Open(int nSampleRate,int nSamplesPerFrame);
	virtual void Close(void);
	virtual int Save(short*pSamples);
	virtual int Recover(short*pSamples);
protected:
	int				m_nSamplesPerFrame;
	short*			m_pSamplesBuf;
	short*			m_pSamplesTmpBuf;
	PLC				m_plc;
};


#endif
