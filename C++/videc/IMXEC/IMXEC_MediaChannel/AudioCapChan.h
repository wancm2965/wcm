//AudioCapChan.h
#ifndef __AUDIOCAPCHAN_H__
#define __AUDIOCAPCHAN_H__

#include <map>

#include <AUDEC/AUDEC_Engine.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <AUDEC/AUDEC_Conference.h>
#include <NETEC/NETEC_MediaSender.h>
#include "XBitrate.h"
#include "XAutoLock.h"

//Add WebRTC by zys 20160929
#include <WEBRTC/X86/my_voice_engine_interface.h>
using namespace voice_engine;


class AudioCapChanUI
{
public:
	AudioCapChanUI(void){};
	virtual~AudioCapChanUI(void){};
public:
	virtual void OnAudioCapChanUIAudioData(unsigned char*pData,int nLen,unsigned long nRefCount,bool&bLoop)=0;
	virtual void OnAudioCapChanUIRtpData(unsigned char*pData,int nLen)=0;  //webrtc д�ļ���
	virtual void OnAudioCapChanUIRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)=0;
	virtual void OnAudioCapChanUIGetForceEncode(int&nEncode)=0;
};

class AudioCapChan
	: public NETEC_MediaSenderCallback
	, public AUDEC_StreamOut
	, public AUDEC_RecordSamples
	, public Transport           //Add WebRTC by zys 20160929
{
public:
	AudioCapChan(void);
	virtual~AudioCapChan(void);
public:
	virtual int Open(void);
	virtual void Close(void);

	virtual int SetVolume(int nVolume);
	virtual int GetVolume();
	virtual int GetLevel(int&nLevel);

	virtual unsigned long GetAudioID(void);
	//����Ƶͨ��
	//CodecType:��������
	virtual int OpenAudio(int nDevID,X_AUDIO_CODEC_ID CodecType,unsigned long nAudioID);

	/******************************************************************************
	* IsAudioOpen
	*��������ȡ�Ƿ��Ѿ�����Ƶ
	*���룺��
	*�����	nDevID		-��Ƶ�ɼ��豸�ţ�>=-1�������вɼ���Ƶ
			CodecType	-��������
	*����ֵ��Ϊtrueʱ�Ѿ�����Ƶ������û�д�
	*****************************************************************************/
	virtual bool IsAudioOpen(int&nDevID,X_AUDIO_CODEC_ID&CodecType);

	//�ر���Ƶͨ��
	virtual void CloseAudio(void);

	virtual int OpenAudioMixer(AUDEC_PlayChan*pPlayChan=NULL);
	void ExcludePlayChan(AUDEC_PlayChan*pPlayChan);
	void StartMixingWithPlayChan(AUDEC_PlayChan*pPlayChan);
	void StopMixingWithPlayChan(AUDEC_PlayChan*pPlayChan);
	void StartMixingWithXCapChanExternal(AUDEC_CapChanExternal*pXCapChanExternal);
	void StopMixingWithXCapChanExternal(AUDEC_CapChanExternal*pXCapChanExternal);

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

	virtual bool Write(const void*data,int len);
	virtual void OnRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);

	virtual void AddAudioCapChanUI(AudioCapChanUI*pAudioCapChanUI);
	virtual void RemoveAudioCapChanUI(AudioCapChanUI*pAudioCapChanUI);
	virtual int GetAudioCapChanUICount(void);
	virtual int GetMixerScreenIndex(void);

	virtual void OnTimer(void);
protected:
	NETEC_MediaSender*		m_pMediaSender;
	XBitrate				m_XBitrateAudio;

	AUDEC_CapChan*			m_pAudCapChan;
	int						m_nAudioCapDevID;
	X_AUDIO_CODEC_ID		m_nAudioCodecID;

	typedef std::map<void*,AudioCapChanUI*>	MapAudioCapChanUI;

	XCritSec				m_csMapAudioCapChanUI;
	MapAudioCapChanUI		m_MapAudioCapChanUI;

	unsigned long			m_nLastOpenTime;
	bool					m_bAudioOpen;
	int						m_nVolume;

	AUDEC_Mixer*			m_pAudioMixer;
	bool					m_bAudioMixerOpen;
	AUDEC_PlayChan*			m_pExcludePlayChan;

	//Add WebRTC by zys 20160929
	MyVoiceEngineInterface* m_pWebRTCAudio;
	int m_nDecodChanID;
	int m_nAudioSeqNo;
	int m_nRTPTimestamp;
	int m_nRTPSeqNo;

protected:
	virtual bool SendRtp(const uint8_t* packet, size_t length, const PacketOptions& options);
	virtual bool SendRtcp(const uint8_t* packet, size_t length);

	virtual int SetAudecHeader(unsigned char* pData,void const* pSrcData,size_t nlen);


public:
	int m_nAudioMixChanInChannel;
};

#endif
