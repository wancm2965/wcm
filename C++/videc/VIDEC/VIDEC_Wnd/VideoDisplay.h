//VideoDisplay.h
#ifndef __VIDEODISPLAY_H__
#define __VIDEODISPLAY_H__

#include <VIDEC/VIDEC.h>
#include "../VIDECYNC/VIDECYNCAPI.h"

class VideoDisplay
{
public:
	VideoDisplay(void){};
	virtual~VideoDisplay(void){};
public:
	virtual int Open(void*pParent)=0;
	virtual void Close(void)=0;

	virtual int GetWidth(void)=0;
	virtual int GetHeight(void)=0;

	virtual void DisplayImage(VIDEC_Image*pImage)=0;

	virtual void SetEnableD3D(bool bEnabled)=0;
	virtual void PausePlay(bool bPause,bool bClear)=0;

	virtual int GetVideoShowFrameRate(void)=0;
	virtual int GetVideoShowType(void)=0;

	static VideoDisplay*Create(YNCD3D_CB_DC cbdc,void*pUserData);

public:
	virtual void ScreenChangeReopenDraw(void)=0;

};


void DestoryOverLayWnd(void);
void CreateOverLayWnd(void);

#endif