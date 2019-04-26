#ifndef __AVPULLPLAYER_H__
#define __AVPULLPLAYER_H__

#include <AVCONAVC/AVCONAVC_Export.h>


#define AVPULL_PLAYER_ON_PLAY		0
#define AVPULL_PLAYER_ON_PAUSE		1

class IAVPullPlayer
{
public:
	IAVPullPlayer(){};
	virtual ~IAVPullPlayer(){};
public:
	virtual void	ReleaseConnections(void)		=0;
	virtual bool	HasAudio(void)					=0;
	virtual void	SetVolume(int nVolume)			=0;/*0-100*/
	virtual int		GetVolume(void)					=0;/*0-100*/
	virtual void	SetMute(bool bMute=true)		=0;
	virtual bool	GetMute(void)					=0;
	virtual void	SetBalance(int nBalance)		=0;/*-1 left,0 both,1 right*/
	virtual int		GetBalance(void)				=0;/*-1 left,0 both,1 right*/

	virtual bool	HasVideo(void)							=0;
	virtual int		GetWidth(void)							=0;
	virtual int		GetHeight(void)							=0;
	virtual bool	SetWindowPos(int x,int y,int cx,int cy)	=0; 
	virtual void	SetVideoQuality(int nVideoQuality)		=0;

	virtual unsigned long GetDuration(void)			=0;
	virtual unsigned long GetPosition(void)			=0;
	virtual void	Seek(unsigned long ulSeconds)	=0;
	virtual void	Seek(unsigned long ulTimeStamp,bool bToKeyFrame)	=0;
	virtual void	Pause(void)						=0;
	virtual void	Play(void)						=0;
	virtual bool	IsPlaying(void)					=0;
};

AVCONAVC_API IAVPullPlayer*CreateAVPullPlayer(HWND hWnd,unsigned long ulUserMsg,const char*cszFileName,int nType=0,DWORD dwOffset=0);

#endif