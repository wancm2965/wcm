//MPScreen.h
#ifndef __MPSCREEN_H__
#define __MPSCREEN_H__

#include <map>
#include <VIDEC/VIDEC.h>
#include "../VIDEC_API/XImageSource.h"
#include "XThreadBase.h"
#include "ImageInput.h"

class MPScreen
	: public VIDEC_MPScreen
	, public XImageSource
	, public XThreadBase
{
public:
	MPScreen(void);
	virtual~MPScreen(void);
public:
	virtual int Open(int nWidth,int nHeight,int nFrameRate);
	virtual void Close(void);
	virtual int SetScreenIndex(int nScreenIndex);
	virtual int GetScreenIndex(void);

	virtual int AddImageSink(VIDEC_ImageSink*pImageSink);
	virtual void RemoveImageSink(VIDEC_ImageSink*pImageSink);
protected:
	void ThreadProcMain(void);
protected:
	bool		m_bRunning;
	int			m_nWidth;
	int			m_nHeight;
	int			m_nFrameRate;
	int			m_nScreenIndex;

	ScreenInput*m_pScreenInput;
};

class MPScreenMgr
{
public:
	MPScreenMgr(void);
	virtual~MPScreenMgr(void);
public:
	static MPScreenMgr&Instance(void);
public:
	void AddMPScreen(MPScreen*pMPScreen);
	void RemoveMPScreen(MPScreen*pMPScreen);
	bool HasMPScreen(int nScreenIndex);
protected:
	typedef std::map<void*,MPScreen*>	MapMPScreen;

	MapMPScreen		m_MapMPScreen;
};

#endif
