//AudioChan.h
#ifndef __AUDIOCHAN_H__
#define __AUDIOCHAN_H__

#include <map>
#include "XAutoLock.h"
#include "XThreadBase.h"

class AH3KDev;
class AudioChan
	: public XThreadBase
{
public:
	AudioChan(int nPhyicalDevID);
	virtual~AudioChan(void);
public:
	int Open(const char*cszChanPath);
	void Close(void);

	void AddAH3KDev(AH3KDev*pAH3KDev);
	void RemoveAH3KDev(AH3KDev*pAH3KDev);
	int GetAH3KDevCount(void);
	int GetPhyicalDevID(void){return m_nPhyicalDevID;}
protected:
	void ThreadProcMain(void);
protected:
	typedef std::map<void*,AH3KDev*>	MapAH3KDev;

	MapAH3KDev	m_MapAH3KDev;
	XCritSec	m_csMapAH3KDev;

	bool			m_bRunning;
	OVERLAPPED		m_Overlapped;
	HANDLE			m_hDevice;

	int				m_nPhyicalDevID;
};

#endif
