//ImageInput.h
#ifndef __IMAGEINPUT_H__
#define __IMAGEINPUT_H__

#include <VIDEC/VIDEC.h>
#include "XListPtr.h"
#include "ImageMixer.h"
#include "XAutoLock.h"

class ImageInput
{
public:
	ImageInput(void);
	virtual~ImageInput(void);
public:
	virtual int Open(void);
	virtual void Close(void);
	
	virtual void SetScreenIndex(int nScreenIndex);
	virtual int GetScreenIndex(void);
	virtual void SetWndIndex(int nWndIndex);
	virtual int GetWndIndex(void);
	virtual void SetFullScreen(int nFullScreen);
	virtual int GetFullScreen(void);
	virtual VIDEC_Image*GetImage(void)=0;
protected:
	int			m_nScreenIndex;
	int			m_nWndIndex;
	int			m_nFullScreen;
};

class ScreenInput
	: public ImageMixer
{
public:
	ScreenInput(int x,int y,int cx,int cy);
	virtual~ScreenInput(void);
public:
	void SetImageSize(int nWidth,int nHeight);
	void SetScreenPos(int x,int y,int cx,int cy);
	void SetScreenWndCount(int nWndCount);
	void SetScreenWndPos(int nWndIndex,int x,int y,int cx,int cy);
	
	void AddImageInput(ImageInput*pImageInput);
	void RemoveImageInput(ImageInput*pImageInput);
	
	void OnImageInputFullScreenChanged(ImageInput*pImageInput);
	void OnImageInputWndIndexChanged(ImageInput*pImageInput);
	
	VIDEC_Image*GetImage(int nForce,int nFrameRate);
protected:
	XCritSec		m_csImageInput;
	ImageInput*		m_pFullScreenImageInput;
	XListPtr		m_listImageInput;

	int				m_nScreenX;
	int				m_nScreenY;
	int				m_nScreenCX;
	int				m_nScreenCY;

	int				m_nWndCount;

#define MAX_WND_COUNT	128
	RECT			m_aWndRect[MAX_WND_COUNT];
	RECT			m_aMappedWndRect[MAX_WND_COUNT];
};

class ImageInputMgr
{
public:
	ImageInputMgr(void);
	virtual~ImageInputMgr(void);
public:
	static ImageInputMgr&Instance(void);
public:
	int SetScreenImageSize(int nScreenIndex,int nScreenWidth,int nScreenHeight);
	int SetScreenPos(int nScreenIndex,int x,int y,int cx,int cy);
	int SetScreenWndCount(int nScreenIndex,int nWndCount);
	int SetScreenWndPos(int nScreenIndex,int nWndIndex,int x,int y,int cx,int cy);
	
	void AddImageInput(ImageInput*pImageInput);
	void RemoveImageInput(ImageInput*pImageInput);
	
	void OnImageInputScreenIndexChanged(ImageInput*pImageInput,int nOldScreenIndex);
	void OnImageInputFullScreenChanged(ImageInput*pImageInput);
	void OnImageInputWndIndexChanged(ImageInput*pImageInput);

	ScreenInput*GetScreenInput(int nScreenIndex);
protected:

#define MAX_SCREEN_COUNT	16
	ScreenInput*	m_apScreenInput[MAX_SCREEN_COUNT];

	XCritSec		m_csImageInput;
	XListPtr		m_listImageInput;
};

#endif
