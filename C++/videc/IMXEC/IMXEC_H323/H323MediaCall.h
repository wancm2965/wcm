//H323MediaCall.h
#ifndef __XH323MEDIACALL_H__
#define __XH323MEDIACALL_H__

#include "MediaCall.h"
#include "H323Call.h"

class H323MediaCall
	: public MediaCall
	, public H323Call
{
public:
	H323MediaCall(H323CallMgr& rH323CallMgr,HCALL hsCall,ConfInfo*pConfInfo=NULL);
	virtual~H323MediaCall(void);
public:
	virtual void Close(void);

	virtual void OpenLocalMainVideoCapDev(void);
	virtual int GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir);

	virtual void OpenRemoteMainPlayChan(void);
	virtual void OpenRemoteSecondPlayChan(void);
	virtual void CloseRemoteSecondPlayChan();

//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
//
//	virtual void OpenRemoteSecondPlayChan(void);
//
//#endif //#if defined(HAS_H323_AVCON_MP) && HAS_H323_AVCON_MP == 1


	virtual bool IsExpired(int nCheckTimer =30000); //notice 30 seconds

protected:
	virtual void OnAudioRecvChannelConnected(void);
	virtual void OnAudioRecvChannelDestroyed(void);
public:
	virtual void OnMainVideoRecvChannelConnected(void);
	virtual void OnMainVideoRecvChannelDestroyed(void);
	virtual void OnSecondVideoRecvChannelConnected(void);
	virtual void OnSecondVideoRecvChannelDestroyed(void);
protected:
	virtual void OnRecvdRemoteAudioRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdMainVideoRequestKeyFrame(void);
	virtual void OnRecvdSecondVideoRequestKeyFrame(void);

	virtual void OnAudioSendChannelConnected(void);
	virtual void OnAudioSendChannelDestroyed(void);

public:
	virtual void OpenLocalSecondVideo(void);
	virtual void OnSecondVideoSendChannelConnected(void);
protected:
	virtual void OnSecondVideoSendChannelDestroyed(void);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

public:

#endif //#if defined(HAS_H323_AVCON_MP) && HAS_H323_AVCON_MP == 1

	virtual void OnLocalSecondVideoStarted(void);
	virtual void OnLocalSecondVideoStop(void);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

protected:

#endif //#if defined(HAS_H323_AVCON_MP) && HAS_H323_AVCON_MP == 1

public:
	virtual bool HasAVCONTransport(void);

public:

	virtual int RequestMainVideoKeyFrame(void);
	virtual int RequestSecondVideoKeyFrame(void);


	virtual void SendAudio(char*pRTPPacketData,int nRTPPacketLen);
	virtual void SendMainVideo(char*pRTPPacketData,int nRTPPacketLen);

	virtual void SendSecondVideo(char*pRTPPacketData,int nRTPPacketLen);

protected:
	int				m_nLocalSecondVideoStarted;

	unsigned long	m_nMainVideoPacketTimestamp;
	unsigned long	m_nSecondVideoPacketTimestamp;

	unsigned long	m_nLastRcvdPacketTimestamp;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	unsigned long	m_nMainVideoRequestKeyFrameCount;
	unsigned long	m_nSecondVideoRequestKeyFrameCount;

#else

	unsigned long	m_nMainVideoRequestKeyFrameCount;
	unsigned long	m_nSecondVideoRequestKeyFrameCount;
	IMXEC_CALL_TYPE m_nCallType;

#endif //#if defined(HAS_H323_AVCON_MP) && HAS_H323_AVCON_MP == 1

//notice netec send ptz
#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#else

public:

	virtual int NETECSendCtrl(void*pData,int nLen);
	virtual int NETECSendPTZ(void*pData,int nLen);
	virtual void OnNETEC_RecvdPTZ(void*pData,int nLen);
	virtual void OnNETEC_RecvdCtrl(void*pData,int nLen);
	virtual void OnAudioRecvDTMF(char dtmf);//add by Jan for DTMF [20140730]

#endif

public:
	virtual int OnNETECRequestMainVideoKeyFrame(void);
	virtual int OnNETECRequestSecondVideoKeyFrame(void);

};


#endif
