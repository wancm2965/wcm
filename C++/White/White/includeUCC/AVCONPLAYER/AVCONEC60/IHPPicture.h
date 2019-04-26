#ifndef __HPPICTURE_H__
#define __HPPICTURE_H__

class IHPPicture
{
public:
	IHPPicture(void){};
	virtual~IHPPicture(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual void ShowWindow(bool bShow)=0;
	virtual bool IsWindowVisible(void)=0;
	virtual void SetWindowPosition(int x,int y,int cx,int cy)=0;
	virtual int GetWidth(void)=0;
	virtual int GetHeight(void)=0;
};

AFX_EXT_CLASS IHPPicture*CreateIHPPicture(HWND hWnd,const TCHAR*cszPathName,const TCHAR*cszCaption);

#endif