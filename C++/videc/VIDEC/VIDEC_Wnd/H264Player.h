//H264Player.h
#ifndef __H264PLAYER_H__
#define __H264PLAYER_H__

#include "../VIDECYNC/VIDECYNCAPI.h"

#include <WinSock2.h>

int H264SupportPlayer(void);

int H264Init(void);
void H264Terminate(void);

void H264PlayerSetMaxCount(unsigned int nMaxPlayer);
int H264PlayerTry(void*hPlayer,unsigned long nWidth,unsigned long nHeight);

class H264Player
{
public:
	H264Player(void);
	virtual~H264Player(void);
public:
	int Open(YNCD3D_CB_DC cbdc,void*pUserData,HWND hWnd, int nWidth, int nHeight);
	void Close(void);

	int PlayFrame(const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int&nFlagReset);
	int GetWidth(void);
	int GetHeight(void);
	int Resize(void);
	int SkipDisplay(int nFlagSkipDisplay);
	bool CaptureToBMPFile(const char*cszFile);
	int GetFrameRate(void);
	int GetShowType(void);
	void*GetPlayerHandle(void);
protected:
	void*	m_hPlayer;
	bool	m_bCUDA;
};


#endif
