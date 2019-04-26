//VIDEC_Player.h
#ifndef __VIDEC_PLAYER_H__
#define __VIDEC_PLAYER_H__

#include <winsock2.h>
#include <windows.h>

#include <VIDEC_WCE/VIDEC.h>
#include <VIDEC_WCE/VIDEC_CodecDef.h>

class VIDEC_API VIDEC_Player
{
public:
	VIDEC_Player(void){};
	virtual~VIDEC_Player(void){};
public:
	virtual int Open(HWND hWnd)=0;
	virtual int Open(HWND hWnd,int nWidth,int nHeight,VIDEC_CODEC_TYPE CodecType)=0;
	virtual void Close(void)=0;
	virtual void SetWndPos(int x,int y,int cx,int cy)=0;
	virtual int GetWidth(void)=0;
	virtual int GetHeight(void)=0;
	virtual HWND GetWnd(void)=0;
	virtual int DecodeFramePacket(void*pPacketData,int nPacketSize)=0;
	virtual int DecodeRTPPacket(void*pPacketData,int nPacketSize)=0;

	static VIDEC_Player*Create(void);
};


#endif