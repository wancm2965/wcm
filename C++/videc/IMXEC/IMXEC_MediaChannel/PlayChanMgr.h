//PlayChanMgr.h
#ifndef __PLAYCHANMGR_H__
#define __PLAYCHANMGR_H__

#include <map>
#include "PlayChan.h"
#include "XBitrate.h"

class PlayChanMgr
{
public:
	PlayChanMgr(void);
	virtual~PlayChanMgr(void);
public:
	static PlayChanMgr&Instance(void);
public:
	void AddPlayChan(PlayChan*pPlayChan);
	void RemovePlayChan(PlayChan*pPlayChan);
	void OnTimer(void);
	void ApplyMixerScreenIndexChanged(int nMixerScreenIndex);

	unsigned long GetTotalKBytes(void);
	unsigned long GetTotalBytes(void);
	int GetBitrate(void);
	void ClearTotalBytes(void);
	
	void SetForcibleReceiving(bool bForcible);
	bool GetForcibleReceiving(void);
protected:
	typedef std::map<void*,PlayChan*>	MapPlayChan;

	MapPlayChan		m_MapPlayChan;

	unsigned long	m_nTotalKBytes;
	unsigned long	m_nTotalBytes;
	XBitrate		m_XBitrateTotal;
	unsigned int	m_nTimerID;

	bool			m_bForcibleReceiving;
};

#endif