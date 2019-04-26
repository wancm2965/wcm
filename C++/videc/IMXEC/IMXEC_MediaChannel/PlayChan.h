//PlayChan.h
#ifndef __PLAYCHAN_H__
#define __PLAYCHAN_H__

#include <list>
#include <string>

#include <IMXEC/IMXEC_MediaChannel.h>
#include <VIDEC/VIDEC_AVIWriter.h>

#include <NETEC/NETEC_MediaReceiver.h>
#include <AUDEC/AUDEC_Engine.h>

#include "XAutoLock.h"
#include "AudioPlayChanMgr.h"
#include "XBitrate.h"
#include "AudioCapChan.h"

#include <VIDEC/VIDEC_Player.h>

//WEBRTC
using namespace voice_engine;

class PlayChan
	: public NETEC_MediaReceiverCallback
	, public IMXEC_PlayChan
	, public AudioPlayChanUI
	, public AudioCapChanUI
	, public VIDEC_PlayerCallback
	, public AUDEC_RecordSamples
{
public:
	PlayChan(IMXEC_PlayChanCallback&rCallback);
	virtual~PlayChan(void);
public:
	//virtual int Open(HWND hwndParent,unsigned long nAudChanID,unsigned long nVidChanID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU);
	virtual int Open(HWND hwndParent,unsigned long nAudChanID,unsigned long nVidChanID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU,unsigned long nEnableMulticast=0);

	/******************************************************************************
	* Open
	*�������򿪱��ز���
	*���룺	hwndParent		-��Ƶ�����ھ��
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int Open(HWND hwndParent);

	virtual void Close(void);

	//ȡ��Ƶ����
	virtual HWND GetVideoWnd(void);
	//ȡ��Ƶ���
	virtual int GetVideoWidth(void);
	//ȡ��Ƶ�߶�
	virtual int GetVideoHeight(void);

	//same as CWnd::IsWindowVisible
	virtual bool IsWindowVisible(void);
	//same as CWnd::SetWindowPos
	virtual bool SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags); 
	//���ֵ�ǰͼ��BMP�ļ�
	virtual bool CaptureToBMPFile(const char*cszFile);

	//������Ƶ����
	virtual int SetVideoParam(int nBrightness/*����ֵ0-255*/, int nContrast/*�Աȶ�0-127*/, int nSaturation/*���Ͷ�0-127*/, int nHue/*ɫ��0-255*/,int nGamma/*٤��1-400*/,int nSharpen/*��0-127*/);

	//int nGammaR[1-400 100]
	//int nGammaG[1-400 100]
	//int nGammaB[1-400 100]
	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);

	/******************************************************************************
	* SetScreenWnd
	*����������������Ļ����λ��
	*���룺	nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen);
	void ApplyMixerScreenIndexChanged(int nMixerScreenIndex);

	virtual void SetVideoWndBk(const char* szBkImage = NULL, int nVideoWnBkR = -1, int nVideoWnBkG = -1, int nVideoWnBkB = -1);

	virtual int SetVolume(int nVolume);
	virtual int GetVolume();
	virtual int GetLevel(int&nLevel);
	virtual void SetMute(bool bMute);
	virtual bool IsMute();

	virtual int StartAudio(void);
	virtual int EnableAudio(int nEnable/*0:�����գ�1:����*/);
	virtual int SetAudioID(unsigned long nAudioID);
	virtual int StopAudio(void);

	virtual int StartVideo(void);
	virtual int EnableVideo(int nEnable/*0:�����գ�1:����*/);
	virtual int StopVideo(void);
	virtual int SetAudioCast(bool bAudioCast);
	virtual int SetVideoCast(bool bVideoCast);

	/******************************************************************************
	* RequestKeyFrame
	*����������ؼ�֡
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int RequestKeyFrame(void);

	/******************************************************************************
	* SetFrameRateControlMode
	*����������֡�ʿ���ģʽ
	*���룺	nMode			-֡�ʿ���ģʽ
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetFrameRateControlMode(FRAME_RATE_CONTROL_MODE nMode);

	/******************************************************************************
	* SetFrameTypeControlMode
	*����������֡���Ϳ���ģʽ
	*���룺	nMode			-֡���Ϳ���ģʽ
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE nMode);

	/******************************************************************************
	* OnNETEC_MediaReceiverCallbackAudioPacket
	*���������յ���Ƶ���ݰ��Ļص�
	*���룺	pData			-���ݰ�ָ��
			nLen			-���ݰ�����
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MediaReceiverCallbackAudioPacket(unsigned char*pData,int nLen){};

	virtual void OnAudioPlayChanUIRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);
	virtual void OnAudioPlayChanUIAudioPacket(unsigned char*pData,int nLen,unsigned long nRefCount,bool&bLoop);
	virtual void OnAudioPlayChanUIGetSynFlag(int&nSynFlag,unsigned long**ppnVideoSynTime,unsigned long**ppnAudioSynTime);
	virtual void OnAudioPlayChanUIGetAudioEnable(int&nEnable);
	virtual void OnAudioPlayChanUIGetScreenIndex(int&nScreenIndex);
	virtual void OnAudioPlayChanUIGetAudioCast(bool&bAudioCast);

	/******************************************************************************
	* OnNETEC_MediaReceiverCallbackVideoPacket
	*���������յ���Ƶ���ݰ��Ļص�
	*���룺	pData			-���ݰ�ָ��
			nLen			-���ݰ�����
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MediaReceiverCallbackVideoPacket(unsigned char*pData,int nLen);
	virtual void OnVIDEC_PlayerCallbackPlayData(void*pPacketData,int nPacketSize);
	virtual void OnVIDEC_PlayerCallbackVQDResult(unsigned long nResult);
	virtual void OnVIDEC_PlayerCallbackDrawD3D9DC(void* hDC/*HDC*/);


	/******************************************************************************
	* StartRecordToAVI
	*��������ʼ����¼��
	*���룺	cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordToAVI(const char*cszPathName,int nStandardFormat=1, int nCacheTimeMS = 10000);
	
	/******************************************************************************
	* StopRecordToAVI
	*������ֹͣ����¼��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordToAVI(void);

	virtual void SetRecording(int nFlagRecording);

	/******************************************************************************
	* GetAudioPacketStatistics
	*��������ȡ��Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetVideoPacketStatistics
	*��������ȡ��Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* PlayAudioData
	*������������Ƶ����
	*���룺	pData			-����ָ��
			nLen			-���ݳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int PlayAudioData(const void*pData,int nLen);

	/******************************************************************************
	* PlayVideoData
	*������������Ƶ����
	*���룺	pData			-����ָ��
			nLen			-���ݳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int PlayVideoData(const void*pData,int nLen);

	virtual void SendCtrlData(void*pData,int nLen);

	/******************************************************************************
	* SetSynFlag
	*������������Ƶ��Ƶͬ�����
	*���룺	nSynFlag		-��Ƶ��Ƶͬ����ǡ�0����Ƶ��Ƶ��ͬ������0����Ƶ��Ƶͬ����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSynFlag(int nSynFlag);

	virtual void SetImage(VIDEC_Image*pImage);

	virtual unsigned long DoGetTotalBytes(void);

	void OnTimer(void);

	virtual int StartMixingAudio(X_AUDIO_CODEC_ID nCodecID);
	virtual void StopMixingAudio(void);
	virtual int GetMixingAudioID(unsigned long&nAudioID);
	AUDEC_PlayChan*GetPlayChan(void);

	virtual int GetScreenIndex(void){return m_nScreenIndex;};
	virtual bool GetAudioCast(void){return m_bAudioCast;};

	virtual void OnAudioCapChanUIAudioData(unsigned char*pData,int nLen,unsigned long nRefCount,bool&bLoop){};
	virtual void OnAudioCapChanUIRtpData(unsigned char*pData,int nLen){};
	virtual void OnAudioCapChanUIRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
	virtual void OnAudioCapChanUIGetForceEncode(int&nEncode){};

	virtual void OnRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);

	virtual int StartVQD(unsigned long nDetectFlags);
	virtual void StopVQD(void);
	virtual int GetDecoderType(void);
	virtual void SetVideoDelay(int nMilliSecond);	
	virtual int SetVideoSkipFrame(int nInterval,int nSkip);	
	virtual int SetVideoPolledMode(void);	
	virtual int GetVideoShowFrameRate(void);
	virtual int GetVideoDecodeFrameRate(void);
	virtual int GetVideoRecvFrameRate(void);
	virtual int GetVideoShowType(void);

#ifdef _TIANZHU_CACHE_
	int SetRemoteVideoCache(int nCacheFrameCount);
#endif

	//Add WebRTC by zys 20160929
protected:
	virtual int AudecHeaderToRTPHeader(unsigned char* pRTPData,void const* pAudecData,size_t nAudecLen);
protected:
	IMXEC_PlayChanCallback&		m_rCallback;

	NETEC_MediaReceiver*		m_pMediaReceiver;
	AUDEC_PlayChan*				m_pAudPlayChan;

	//Add WebRTC by zys 20160929
	MyVoiceEngineInterface*		m_pWebRTCAudio;
	int							m_nWebRTCPlayChan;
	int							m_nRTPTimestamp;
	int							m_nRTPSeqNo;

#ifdef __VIDEC_PLAYER_H__
	VIDEC_Player*				m_pVideoPlayer;
#else
	VIDEC_PlayChan*				m_pVidPlayChan;
	VIDEC_Wnd*					m_pWnd;
#endif

	HWND						m_hwndParent;
	std::string                 m_strVideoWndBkImage;
	int                         m_nVideoWnBkR;
	int                         m_nVideoWnBkG;
	int                         m_nVideoWnBkB;

	XCritSec					m_csAVIWriter;
	VIDEC_AVIWriter*			m_pAVIWriter;

	unsigned long				m_nAudioSynTime;
	unsigned long				m_nVideoSynTime;
	int							m_nSynFlag;

	unsigned long				m_nVideoID;
	unsigned long				m_nAudioChanID;
	std::string					m_strPeerNodeID;
	std::string					m_strPeerNATIP;
	std::string					m_strPeerLocalIP;
	unsigned short				m_nPeerNATPort;
	unsigned short				m_nPeerLocalPort;
	std::string					m_strPeerMCUID;
	std::string					m_strPeerMCUIP;
	unsigned short				m_nPeerMCUPort;
	int							m_nForceUserMCU;

	int							m_nAudioStarted;
	int							m_nAudioEnable;
	int							m_nAudioVolume;
	bool                        m_bMute;

	unsigned long				m_nTotalBytes;

	int							m_nEnableVideo;
	int							m_nVideoStarted;

	typedef std::list<std::string>	ListPathName;
	ListPathName				m_lstPathNameJPG;
	ListPathName				m_lstPathNameJPGDone;

	FRAME_TYPE_CONTROL_MODE		m_nFrameTypeControlMode;
	FRAME_TYPE_CONTROL_MODE		m_nFrameTypeControlModeCurrent;
	XBitrate					m_XBitrateVideo;

	bool						m_bHardwareError;
	int							m_nVirtualWidth;
	int							m_nVirtualHeight;

	int							m_nScreenIndex;
	bool						m_bAudioCast;
	X_AUDIO_CODEC_ID			m_nMixingAudioCodecID;

	unsigned long				m_nKeyID;

	unsigned long				m_nLastRequestKeyFrameTS;

public:
	//notice netec send ptz 
	virtual int SendCtrl(void*pData,int nLen);
	virtual int SendPTZ(void*pData,int nLen);
	int m_nAudioMixInChannel;

	int m_nAudioMixChanInChannel;
};


#endif
