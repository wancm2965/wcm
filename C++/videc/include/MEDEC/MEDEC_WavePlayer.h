//MEDEC_WavePlayer.h
#ifndef __MEDEC_WAVEPLAYER_H__
#define __MEDEC_WAVEPLAYER_H__

#include <MEDEC/MEDEC_Export.h>

class MEDEC_API MEDEC_WavePlayer
{
public:
	MEDEC_WavePlayer(void){};
	virtual~MEDEC_WavePlayer(void){};
public:
	virtual int Open(const char*cszPathName)=0;
	virtual void Close(void)=0;
	virtual int GetLevel(void)=0;
	virtual void SetVolume(int nVolume)=0;//0-10-20

	static MEDEC_WavePlayer*Create(void);
};

#endif
