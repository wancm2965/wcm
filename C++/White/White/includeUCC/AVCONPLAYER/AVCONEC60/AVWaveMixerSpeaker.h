// AVWaveMixerSpeaker.h: interface for the AVWaveMixerSpeaker class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVWAVEMIXERSPEAKER_H__
#define __AVWAVEMIXERSPEAKER_H__

class AVCONEC60_AFX_EXT_CLASS AVWaveMixerSpeakerNotify
{
public:
	AVWaveMixerSpeakerNotify(void);
	virtual~AVWaveMixerSpeakerNotify(void);
public:
	virtual void AttachToSpeaker(int nDevID);
	virtual void DetachFromSpeaker(void);
	virtual int GetAttachedSpeakerDevID(void);

	//
	//静音改变的回调，nMute[0:非静音,1:静音]
	//
	virtual void OnMixerSpeakerNotifyMuteChanged(int nMute)=0;
	//
	//音量改变的回调，nVolume[0-100,0:最小音量 100:最大音量]
	//
	virtual void OnMixerSpeakerNotifyVolumeChanged(int nVolume)=0;
protected:
	int	m_nSpeakerDevID;
};

class AVWaveMixerSpeaker
{
public:
	AVWaveMixerSpeaker(void){}
	virtual~AVWaveMixerSpeaker(void){};
public:
	virtual int Connect(int nDevID)=0;
	virtual void ReleaseConnections(void)=0;

	//
	//设置静音，nMute[0:非静音,1:静音]，出错返回-1
	//
	virtual int SetMute(int nMute)=0;
	//
	//取静音，返回[0:非静音,1:静音]
	//
	virtual int GetMute(void)=0;
	//
	//设置音量，nVolume[0-100,0:最小音量 100:最大音量]，出错返回-1
	//
	virtual int SetVolume(int nVolume)=0;
	//
	//取音量，返回[0-100,0:最小音量 100:最大音量]，出错返回-1
	//
	virtual int GetVolume(void)=0;
};

AVCONEC60_AFX_EXT_CLASS AVWaveMixerSpeaker*AVWaveMixerSpeakerCreate(void);

//0:no dev,>0:dev count <0:error
AVCONEC60_AFX_EXT_CLASS int XGetSpeakerNumDevs(void);
//0:ok,<0:error
AVCONEC60_AFX_EXT_CLASS int XGetSpeakerDevDesc(int dev,TCHAR desc[]);

#endif