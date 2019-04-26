//HPMP_Player.h
#ifndef __HPMP_PLAYER_H__
#define __HPMP_PLAYER_H__



#include <HPMP/HPMP_Export.h>


class HPMP_API HPMP_Player
{
public:
	HPMP_Player(void){};
	virtual~HPMP_Player(void){};
public:
	//����ʾ����
	//�ɹ�����0��ʧ�ܷ���-1
	virtual int Open(unsigned long*lpnVideoTimestamp=NULL,unsigned long*lpnAudioTimestamp=NULL)=0;
	//�ر���ʾ����
	virtual void Close(void)=0;


	virtual void SetSynFlag(int nSynFlag)=0;


	virtual int PlayData(void*pPacketData,int nPacketSize)=0;

	
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;

	static HPMP_Player*Create(void);
};

#endif