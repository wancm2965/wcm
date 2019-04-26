#pragma once
#include "iduisound.h"
#include "MMSystem.h"

class CDUISound_WAV :
	public IDUISound
{
public:
	CDUISound_WAV(LPCTSTR lpszSound,  UINT fuSound )
	{
		TCHAR* szData = new TCHAR[MAX_PATH];
		_tcscpy(szData, lpszSound);
		m_pszSound = szData;
		m_fuSound = fuSound;
	};
	virtual ~CDUISound_WAV()
	{
		delete [] m_pszSound;
	};

public:
	virtual void	Play() {sndPlaySound(m_pszSound, m_fuSound);}
	virtual void	Stop() {sndPlaySound(NULL, m_fuSound);};

protected:
	LPCTSTR	m_pszSound;
	UINT	m_fuSound;
};
