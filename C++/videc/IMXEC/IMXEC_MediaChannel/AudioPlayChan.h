//AudioPlayChan.h
#ifndef __AUDIOPLAYCHAN_H__
#define __AUDIOPLAYCHAN_H__

#include <map>
#include <NETEC/NETEC_MediaReceiver.h>
#include <AUDEC/AUDEC_Engine.h>

#include "XAutoLock.h"

//Add WebRTC by zys 20160929
#define USE_WEBRTC 1
#include <WEBRTC/X86/my_voice_engine_interface.h>
using namespace voice_engine;

extern VOICE_ENGINE_CODEC_ID g_emAudioCodecID;
class AudioPlayChanUI
{
public:
	AudioPlayChanUI(void){};
	virtual~AudioPlayChanUI(void){};
public:
	virtual void OnAudioPlayChanUIRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)=0;
	virtual void OnAudioPlayChanUIAudioPacket(unsigned char*pData,int nLen,unsigned long nRefCount,bool&bLoop)=0;
	virtual void OnAudioPlayChanUIGetSynFlag(int&nSynFlag,unsigned long**ppnVideoSynTime,unsigned long**ppnAudioSynTime)=0;
	virtual void OnAudioPlayChanUIGetAudioEnable(int&nEnable)=0;
	virtual void OnAudioPlayChanUIGetScreenIndex(int&nScreenIndex)=0;
	virtual void OnAudioPlayChanUIGetAudioCast(bool&bAudioCast)=0;
};

class AudioPlayChan
	: public NETEC_MediaReceiverCallback
	, public AUDEC_RecordSamples
{
public:
	AudioPlayChan(void);
	virtual~AudioPlayChan(void);
public:
	virtual int Open(unsigned long nAudChanID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU);
	virtual void Close(void);
	virtual int SetSynFlag(int nSynFlag,unsigned long*pnVideoSynTime,unsigned long*pnAudioSynTime);
	virtual unsigned long*GetVideoSynTime(void);
	virtual unsigned long*GetAudioSynTime(void);

	virtual int SetVolume(int nVolume);
	virtual int GetVolume();
	virtual int GetLevel(int&nLevel);
	virtual void SetMute(bool bMute);

	virtual int StartAudio(void);
	virtual int EnableAudio(int nEnable/*0:�����գ�1:����*/);
	virtual int StopAudio(void);
	virtual int SetAudioCast(bool bAudioCast);

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

	virtual void AddAudioPlayChanUI(AudioPlayChanUI*pAudioPlayChanUI);
	virtual void RemoveAudioPlayChanUI(AudioPlayChanUI*pAudioPlayChanUI);
	virtual int GetAudioPlayChanUICount(void);

	AUDEC_PlayChan*GetPlayChan(int nMixerScreenIndex,bool bMixing);
protected:
	/******************************************************************************
	* OnNETEC_MediaReceiverCallbackAudioPacket
	*���������յ���Ƶ���ݰ��Ļص�
	*���룺	pData			-���ݰ�ָ��
			nLen			-���ݰ�����
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MediaReceiverCallbackAudioPacket(unsigned char*pData,int nLen);

	/******************************************************************************
	* OnNETEC_MediaReceiverCallbackVideoPacket
	*���������յ���Ƶ���ݰ��Ļص�
	*���룺	pData			-���ݰ�ָ��
			nLen			-���ݰ�����
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MediaReceiverCallbackVideoPacket(unsigned char*pData,int nLen){};

	virtual void OnRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);

	//Add WebRTC by zys 20160929
protected:
	virtual int AudecHeaderToRTPHeader(unsigned char* pRTPData,void const* pAudecData,size_t nAudecLen);

protected:
	NETEC_MediaReceiver*		m_pMediaReceiver;
	AUDEC_PlayChan*				m_pAudPlayChan;

	//Add WebRTC by zys 20160929
	MyVoiceEngineInterface* m_pWebRTCAudio;
	int m_nWebRTCPlayChan;
	int m_nRTPTimestamp;
	int m_nRTPSeqNo;

	typedef std::map<void*,AudioPlayChanUI*>	MapAudioPlayChanUI;
	MapAudioPlayChanUI	m_MapAudioPlayChanUI;
	XCritSec					m_csMapAudioPlayChanUI;

	int							m_nSynFlag;
	unsigned long*				m_pnVideoSynTime;
	unsigned long*				m_pnAudioSynTime;

	unsigned long				m_nAudioID;

	int m_nAudioMixInChannel;

	int m_nAudioMixChanInChannel;
};

#endif