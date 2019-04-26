#ifndef __PCMCONVERT_H__
#define __PCMCONVERT_H__

#include <MMSystem.h>
#include <mmreg.h>
#include <msacm.h>

class PCMConvert
{
public:
	PCMConvert(void);
	virtual~PCMConvert(void);
public:
	bool Connect(LPWAVEFORMATEX pwfxSrc,LPWAVEFORMATEX pwfxDst);
	void ReleaseConnections(void);
	int Convert(char*pSrc,int nSrc,char*pDst,int&nDst);
protected:
    HACMSTREAM		m_hACMStream;
};

#endif