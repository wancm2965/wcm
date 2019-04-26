// AVWaveMixerMic.h: interface for the AVWaveMixerMic class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVWAVEMIXERMIC_H__
#define __AVWAVEMIXERMIC_H__


//
//麦克风事件通知
//
class AVCONEC60_AFX_EXT_CLASS AVWaveMixerMicNotify
{
public:
	AVWaveMixerMicNotify(void);
	virtual~AVWaveMixerMicNotify(void);
public:
	enum {
		MIC=0,
		LINE_IN,
		WAVE_OUT_MIXER,
		UNKOWN
	};
public:
	//
	//绑定事件通知到麦克风设备
	//
	virtual void AttachToMic(int nDevID);
	//
	//解除绑定到麦克风设备的事件通知
	//
	virtual void DetachFromMic(void);
	//
	//取得绑定的麦克风设备
	//
	virtual int GetAttachedMicDevID(void);
	//
	//通知麦克风的Boost发生改变，nBoost[0:disable,1:enable]
	//
	virtual void OnMixerMicNotifyBoostChanged(int nBoost)=0;
	//
	//通知麦克风输入改变，nType[MIC，LINE_IN，WAVE_OUT_MIXER，UNKOWN]
	//
	virtual void OnMixerMicNotifyInputTypeChanged(int nType)=0;
	//
	//通知录音音量改变，nVolume[0（最小）到100（最大）]
	//
	virtual void OnMixerMicNotifyVolumeChanged(int nVolume)=0;
protected:
	int	m_nMicDevID;
};

class AVWaveMixerMic
{
public:
	AVWaveMixerMic(void){};
	virtual~AVWaveMixerMic(void){};
public:
	virtual int Connect(int nDevID)=0;
	virtual void ReleaseConnections(void)=0;

	virtual int SetBoost(int nBoost)=0;
	virtual int GetBoost(void)=0;

	virtual int SetInputType(int nType)=0;
	virtual int GetInputType(void)=0;

	virtual int SetVolume(int nVolume)=0;
	virtual int GetVolume(void)=0;
};

AVCONEC60_AFX_EXT_CLASS AVWaveMixerMic*AVWaveMixerMicCreate(void);

//0:no dev,>0:dev count <0:error
AVCONEC60_AFX_EXT_CLASS int XGetMicNumDevs(void);
//0:ok,<0:error
AVCONEC60_AFX_EXT_CLASS int XGetMicDevDesc(int dev,TCHAR desc[]);


#endif
