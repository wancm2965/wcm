//AVCONAVC_VideoDraw.h
#ifndef __AVCONAVC_VIDEO_DRAW_H__
#define __AVCONAVC_VIDEO_DRAW_H__

#include <AVCONAVC/AVCONAVC_Export.h>

class AVCONAVC_API AVCONAVC_VideoDraw
{
public:
	AVCONAVC_VideoDraw(void){};
	virtual~AVCONAVC_VideoDraw(void){};
public:
	virtual int Open(HWND hWndVideo,int nWidth,int nHeight,bool bNV12,bool& bTryEVR)=0;
	virtual void Close(void)=0;
	virtual void Repaint(void)=0;
	virtual void SetWndPos(int x,int y,int cx,int cy)=0;
	virtual int GetWidth(void)=0;
	virtual int GetHeight(void)=0;
	virtual bool GetNV12(void)=0;
	virtual int Draw(unsigned char*pYUV,int nStride)=0;

	static AVCONAVC_VideoDraw*Create(void);
};

#endif
