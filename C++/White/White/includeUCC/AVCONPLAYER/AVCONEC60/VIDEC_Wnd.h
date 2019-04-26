//VIDEC_Wnd.h
#ifndef __VIDEC_WND_H__
#define __VIDEC_WND_H__

class VIDEC_WndNotify
{
public:
	VIDEC_WndNotify(void){};
	virtual~VIDEC_WndNotify(void){};
public:
	virtual void OnVIDEC_WndNotifyVideoResized(void)=0;
};

class VIDEC_Wnd
{
public:
	VIDEC_Wnd(void){};
	virtual~VIDEC_Wnd(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual int GetVideoWidth(void)=0;
	virtual int GetVideoHeight(void)=0;
	virtual int GetWindowWidth(void)=0;
	virtual int GetWindowHeight(void)=0;
	virtual void ShowWindow(bool bShow)=0;
	virtual bool IsWindowVisible(void)=0;
	virtual void SetWindowPos(int X,int Y,int cx,int cy)=0;
	virtual void PaintFrame(unsigned char*pData,int nLen,int nWidth,int nHeight)=0;
	virtual void RepaintFrame(void)=0;

	virtual void EnableTitle(bool bEnable)=0;
	virtual bool IsTitleEnable(void)=0;
	virtual void SetTitleText(const TCHAR*cszText,int nAlign=0)=0;//Align==0 TOP_LEFT 1:BOTOM_LEFT

	virtual void SetFrameRate(unsigned short usFrameRate)=0;
	virtual bool CaptureToBMPFile(const char*cszFile)=0;

	virtual bool HasVideo(void)=0;
};

AFX_EXT_CLASS VIDEC_Wnd*CreateVIDEC_Wnd(VIDEC_WndNotify&rVIDEC_WndNotify,HWND hwndParent,bool bJitter,bool bI420=false);


#endif