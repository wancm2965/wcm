//CapChanDevMgr.h
#ifndef __CAPCHANDEVMGR_H__
#define __CAPCHANDEVMGR_H__

#include <map>
#include "CapChanDev.h"

class CapChanDevMgr
{
public:
	CapChanDevMgr(void);
	virtual~CapChanDevMgr(void);
public:
	static CapChanDevMgr&Instance(void);
public:
	void AddCapChanDev(CapChanDev*pCapChanDev);
	void ReleaseCapChanDev(CapChanDev*pCapChanDev);
	void OnTimer(void);

	void EnableAudioSend(bool bEnable);
	bool IsEnableAudioSend(void);
	void EnableVideoSend(bool bEnable);
	bool IsEnableVideoSend(void);
	
	void SetVideoAutoFormat(int nFlagAuto);
	int GetVideoAutoFormat(void);

	void SetSpeakerMute(int nMute);
	int GetSpeakerMute(void);

	unsigned long GetTotalKBytes(void);
	unsigned long GetTotalBytes(void);
	int GetBitrate(void);
	void ClearTotalBytes(void);

	int GetMixerScreenIndex(void);
	void ApplyMixerScreenIndexChanged(int nMixerScreenIndex);
protected:
	typedef std::map<void*,CapChanDev*>	MapCapChanDev;

	MapCapChanDev	m_MapCapChanDev;
	unsigned int	m_nTimerID;

	bool			m_bEnableAudioSend;
	bool			m_bEnableVideoSend;

	unsigned long	m_nTotalKBytes;
	unsigned long	m_nTotalBytes;
	XBitrate		m_XBitrateTotal;

	int				m_nFlagVideoAutoFormat;

	int				m_nSpeakerMute;
};

#endif
