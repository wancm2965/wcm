//AVAudio.h
#ifndef __AVAUDIO_H__
#define __AVAUDIO_H__

#include <mmsystem.h>

typedef enum AUDIO_CODEC
{
	AVAUDIO_AVAC_G7231=2,
	AVAUDIO_AVAC_8K=25,
	AVAUDIO_AVAC_16K,
	AVAUDIO_AVAC_24K,
	AVAUDIO_AVAC_32K,
	AVAUDIO_G726H_24K,
	AVAUDIO_G722H_16K
}AUDIO_CODEC;


#define AVUM_AUDIO_CAPTURE_MAX_VOLUME	(WM_USER+310)
#define AVUM_AUDIO_PLAYER_MAX_VOLUME	(WM_USER+311)

/**************************************************************************/
/*��Ƶ�ɼ�����ѹ��
/**************************************************************************/
class IAVAudioCapture : public AVSource
{
public:
	IAVAudioCapture(void){};
	virtual~IAVAudioCapture(void){};
public:
	/***********************************************************************/
	/*���ܣ��ر���Ƶ�豸���ͷ���Ƶ�ɼ���ռ�е�������Դ
	/*������
	/*���أ�
	/*��ע������ɾ���ö���ǰ���ô˺�������ȷ���ͷ�������Դ
	/***********************************************************************/
	virtual void ReleaseConnections(void)=0;

	/***********************************************************************/
	/*���ܣ��Ƿ�ɼ���Ƶ
	/*������bool bCapture[in,�Ƿ�ɼ���Ƶ�����bCapture=true�ɼ�������ֹͣ�ɼ�]
	/*���أ�����Ѿ�����Ƶ�豸�򷵻�true�����򷵻�false
	/*��ע��
	/***********************************************************************/
	virtual bool Capture(bool bCapture)=0;
	
	/***********************************************************************/
	/*���ܣ��ж��Ƿ������ڲɼ���Ƶ
	/*������
	/*���أ�������ڲɼ���Ƶ�򷵻�true�����򷵻�false
	/*��ע�����û�д���Ƶ�豸��Ҳ����false
	/***********************************************************************/
	virtual bool IsCapturing(void)=0;
};


/***********************************************************************/
/*���ܣ�������Ƶ�ɼ�����
/*������HWND hWnd[in,���ھ��,��DirectSound��ʹ��],int nDeviceID[in,��Ƶ�豸����ţ���0��ʼ],
/*		AUDIO_CODEC AudioCodec[in,��Ƶ����������]
/*���أ��������Ƶ�豸�ɹ�������Ƶ����������֧���򷵻���Ƶ�ɼ����󣬷��򷵻�NULL
/*��ע��
/***********************************************************************/
AVCONEC40_AFX_EXT_CLASS IAVAudioCapture*CreateAudioCapture(HWND hWnd,int nDeviceID,AUDIO_CODEC AudioCodec);

#define IAVAudioCaptureEx		IAVAudioCapture
#define CreateAudioCaptureEx	CreateAudioCapture


class IAVAudioPlayerCallback
{
public:
	IAVAudioPlayerCallback(void){};
	virtual~IAVAudioPlayerCallback(void){};
public:
	virtual void OnIAVAudioPlayerCallbackFrameData(char*pFrameData,int nLen){};
};

/***********************************************************************/
/*��Ƶ�����������з������������ͬ�����Ź���
/***********************************************************************/
class IAVAudioPlayer : public AVSink
{
public:
	IAVAudioPlayer(){};
	virtual~IAVAudioPlayer(){};
public:
	/***********************************************************************/
	/*���ܣ��ر���Ƶ�豸���ͷ���Ƶ��������ռ�е�������Դ
	/*������
	/*���أ�
	/*��ע������ɾ���ö���ǰ���ô˺�������ȷ���ͷ�������Դ
	/***********************************************************************/
	virtual void ReleaseConnections(void)=0;
	
	/***********************************************************************/
	/*���ܣ����ò�������
	/*������int nVolume[in,��������0-100��100�������0������С]
	/*���أ�
	/*��ע��
	/***********************************************************************/
	virtual void SetVolume(int nVolume)=0;

	/***********************************************************************/
	/*���ܣ���ȡ��������
	/*������
	/*���أ���������0-100��100�������0������С
	/*��ע��
	/***********************************************************************/
	virtual int GetVolume(void)=0;

	/***********************************************************************/
	/*���ܣ��Ƿ񲥷ž���
	/*������bool bMute[in,�Ƿ��������bMute=true���������򲥷�����]
	/*���أ�
	/*��ע��
	/***********************************************************************/
	virtual void Mute(bool bMute)=0;
	
	/***********************************************************************/
	/*���ܣ��ж��Ƿ񲥷ž���
	/*������
	/*���أ���������򷵻�true�����򷵻�false
	/*��ע��
	/***********************************************************************/
	virtual bool IsMute(void)=0;

	virtual void EnableJitterBuffer(bool bEnableJitterBuffer)=0;
	virtual bool IsEnableJitterBuffer(void)=0;

	virtual void StartIAVAudioPlayerCallback(IAVAudioPlayerCallback*pIAVAudioPlayerCallback)=0;
	virtual void StopIAVAudioPlayerCallback(void)=0;

	virtual void AutoDelay(bool bAutoDelay)=0;
};

/***********************************************************************/
/*���ܣ�������Ƶ����������
/*������WAVEFORMATEX*pwfx[in,��Ƶ����]
/*���أ�����ǲ�֧�ֵ���Ƶ�����򷵻�NULL�����򷵻���Ƶ����������
/*��ע��
/***********************************************************************/
AVCONEC40_AFX_EXT_CLASS IAVAudioPlayer*CreateAudioPlayer(HWND hWnd,AUDIO_CODEC AudioCodec,AVSyn*pAVSyn);

AVCONEC40_AFX_EXT_CLASS int GetAudioCaptureDeviceCount(void);
AVCONEC40_AFX_EXT_CLASS bool GetAudioCaptureDeviceName(int nAudioCaptureDeviceIndex,TCHAR*szAudioCaptureDeviceName);

AVCONEC40_AFX_EXT_CLASS void EnableAEC(bool bEnableAEC);
AVCONEC40_AFX_EXT_CLASS bool IsEnableAEC(void);
AVCONEC40_AFX_EXT_CLASS void EnableANS(bool bEnableANS);
AVCONEC40_AFX_EXT_CLASS bool IsEnableANS(void);
AVCONEC40_AFX_EXT_CLASS void EnableCN(bool bEnableCN);
AVCONEC40_AFX_EXT_CLASS bool IsEnableCN(void);
AVCONEC40_AFX_EXT_CLASS void EnableVAD(bool bEnableVAD);
AVCONEC40_AFX_EXT_CLASS bool IsEnableVAD(void);
AVCONEC40_AFX_EXT_CLASS void EnableAGC(bool bEnableAGC);
AVCONEC40_AFX_EXT_CLASS bool IsEnableAGC(void);


AVCONEC40_AFX_EXT_CLASS bool XMCInitAudio(void);
AVCONEC40_AFX_EXT_CLASS void XMCUnitAudio(void);

class IXPCMFilePlayer
{
public:
	IXPCMFilePlayer(void){};
	virtual~IXPCMFilePlayer(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual bool IsDone(void)=0;
};

AVCONEC40_AFX_EXT_CLASS IXPCMFilePlayer*CreateIXPCMFilePlayer(const char*cszPCMFile,unsigned long ulSampleRate,bool bLoop=false);

AVCONEC40_AFX_EXT_CLASS int GetAECQuality(void);//0:��,1:�Ϻ�,2:��,3:�ϲ�

#endif