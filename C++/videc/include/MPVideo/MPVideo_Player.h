//MPVideo_Player.h
#ifndef __MPVIDEO_PLAYER_H__
#define __MPVIDEO_PLAYER_H__



#include <MPVideo/MPVideo_Export.h>

class MPVideo_PlayerCallback
{
public:
	MPVideo_PlayerCallback(void){}
	virtual~MPVideo_PlayerCallback(void){};
public:
	virtual void OnMPVideo_PlayerCallbackPlayData(void*pPacketData,int nPacketSize){};
};

class MPVIDEO_API MPVideo_Player
{
public:
	MPVideo_Player(void){};
	virtual~MPVideo_Player(void){};
public:
	//打开显示窗口
	//成功返回0，失败返回-1
	virtual int Open(unsigned long*lpnVideoTimestamp=NULL,unsigned long*lpnAudioTimestamp=NULL)=0;
	//关闭显示窗口
	virtual void Close(void)=0;


	virtual void SetSynFlag(int nSynFlag)=0;


	virtual int PlayData(void*pPacketData,int nPacketSize)=0;

	
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;

	static MPVideo_Player*Create(MPVideo_PlayerCallback&rCallback);
};

#endif