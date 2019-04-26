//H323A7DAPCall.h
#ifndef __XH323MEDIACALL_H__
#define __XH323MEDIACALL_H__

#include "MediaCall.h"
#include "H323Call.h"
//add
class AUDEC_PlayChan;

class H323MediaCall
	: public MediaCall
	, public H323Call
{
public:
	H323MediaCall(H323CallMgr& rH323CallMgr,HCALL hsCall,ConfInfo*pConfInfo=NULL);
	virtual~H323MediaCall(void);
public:
	virtual void Close(void);

	virtual int GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir);

	virtual bool HasAVCONTransport(void);

	virtual void OpenReceiverFromRemote(void);

	virtual bool IsExpired(int nCheckTimer =30000); //notice 30 seconds

	virtual int RequestMainVideoKeyFrame(void);
	virtual int RequestSecondVideoKeyFrame(void);

protected:
	virtual void OnAudioRecvChannelConnected(void);
	virtual void OnAudioRecvChannelDestroyed(void);
	virtual void OnMainVideoRecvChannelConnected(void);
	virtual void OnMainVideoRecvChannelDestroyed(void);
	virtual void OnSecondVideoRecvChannelConnected(void);
	virtual void OnSecondVideoRecvChannelDestroyed(void);

	virtual void OnRecvdRemoteAudioRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdMainVideoRequestKeyFrame(void);
	virtual void OnRecvdSecondVideoRequestKeyFrame(void);

	virtual void OnMainVideoSendChannelConnected(void);
	virtual void OnMainVideoSendChannelDestroyed(void);

	virtual void OnAudioSendChannelConnected(void);
	virtual void OnAudioSendChannelDestroyed(void);
	
	virtual void OnSecondVideoSendChannelConnected(void);
	virtual void OnSecondVideoSendChannelDestroyed(void);
	virtual void OnLocalSecondVideoStarted(void);
	virtual void OnLocalSecondVideoStop(void);

	virtual void SendAudio(char*pRTPPacketData,int nRTPPacketLen);
	virtual void SendMainVideo(char*pRTPPacketData,int nRTPPacketLen);
	virtual void SendSecondVideo(char*pRTPPacketData,int nRTPPacketLen);

protected:
	int				m_nLocalSecondVideoStarted;
	unsigned long	m_nLastRcvdPacketTimestamp;

public:
	//notice netec send ptz 
	virtual int NETECSendCtrl(void*pData,int nLen);
	virtual int NETECSendPTZ(void*pData,int nLen);
	virtual void OnNETEC_RecvdPTZ(void*pData,int nLen);
	virtual void OnNETEC_RecvdCtrl(void*pData,int nLen);

	virtual int OnNETECRequestMainVideoKeyFrame(void);
	virtual int OnNETECRequestSecondVideoKeyFrame(void);

};


#endif
