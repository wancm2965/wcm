//SIPMediaCall.h
#ifndef __XSIPMEDIACALL_H__
#define __XSIPMEDIACALL_H__

#include "MediaCall.h"
#include "SIPCall.h"

//add
class AUDEC_PlayChan;

class SIPMediaCall
	: public MediaCall
	, public SIPCall
{
public:
	SIPMediaCall(SIPUser&rSIPUser,const std::string&strCallID,const std::string&strLocalIP,unsigned short nLocalAudioPort,unsigned short nLocalVideoPort,ConfInfo*pConfInfo=NULL);
	virtual~SIPMediaCall(void);
public:
	virtual void Close(void);

	virtual void CloseMedia(void);
	virtual int GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir);

	virtual void OpenLocalSecondVideo(void);

	virtual void OnSecondVideoSendChannelConnected(void);
	virtual void OnSecondVideoSendChannelDestroyed(void);

	int	 m_nLocalSecondVideoStarted;

protected:
	virtual int OpenLocalAudioCapDev(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType);
	virtual int OpenRemoteAudioPlayChan(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType);

	virtual int OpenLocalMainVideoCapDev(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,int nNetworkType=VIDEC_NETWORK_PRIVATE);
	virtual int OpenRemoteMainVideoPlayChan(VIDEC_CODEC_TYPE codecType,int nBitrate,int nVideoPayloadType);
	
	virtual void OnSecondVideoRecvChannelConnected(VIDEC_CODEC_TYPE codecType,int nBitrate,int nVideoPayloadType);
	virtual void OnSecondVideoRecvChannelDestroyed(void);
	
	virtual void OnRecvdRemoteAudioRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

	virtual void OnRecvdMainVideoRequestKeyFrame(void);
	virtual void OnRecvdSecondVideoRequestKeyFrame(void);

	virtual void SendAudio(char*pRTPPacketData,int nRTPPacketLen);
	virtual void SendMainVideo(char*pRTPPacketData,int nRTPPacketLen);
	virtual void SendSecondVideo(char*pRTPPacketData,int nRTPPacketLen);

public:
	virtual int RequestMainVideoKeyFrame(void);
	virtual int RequestSecondVideoKeyFrame(void);

protected:
	unsigned long	m_nMainVideoPacketTimestamp;
	unsigned long	m_nLastRcvdPacketTimestamp;

public:
	virtual bool IsExpired(int nCheckTimer =30000); //notice 30 seconds
	virtual int OnNETECRequestMainVideoKeyFrame(void);
	virtual int OnNETECRequestSecondVideoKeyFrame(void);

};


#endif
