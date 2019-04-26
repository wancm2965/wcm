//MediaNet_RTP.h
#ifndef __MediaNet_RTP_H__
#define __MediaNet_RTP_H__

#include "MediaNet_RTPSession.h"
#include <common/hpdef.h>
#include <common/hptype.h>

#include <string>

#define  MAX_ADDR_STRING_LEN  46

class MediaNet_RTP
	: public MediaNet_RTPSession
{
public:
	MediaNet_RTP(MediaNet_RTPSessionCallback&rCallback,void * pMediaEndpoint,unsigned nClockRate,int nSamplePerFrame,int nUseICE);

	virtual~MediaNet_RTP(void);

public:
	virtual int Open(const char*cszLocalIP,unsigned short nLocalPort,unsigned char nPayloadType,const char*cszPayloadName,int nDir=3,char * szKeyBuf=NULL,int nAlgoId=0);//nDir 1 recv , 2 send , 3 sendrecv
	virtual void Close(void);
	virtual void AddRemote(const char*cszRemoteIP,unsigned short nRemotePort,void *tmp_pool,void *sdp_local,void *sdp_remote,int media_index,int nCmpCount, int nFlagReliable=0,unsigned short m_nRemoteRTCPPort=0,int nDir =3); //nDir 1 recv , 2 send , 3 sendrecv
	virtual void SendRTPPacket(const void *data,size_t len,unsigned char pt,bool mark,unsigned int timestampinc,unsigned char*pPacketData,int nPacketLen,unsigned char nRepeatCount=0);
	virtual void OnRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	void SetRTPHeaderFromData(TRtpPack & tRtpPack,const void *data,int len);
	void CopyRTPToData(TRtpPack * pRtpPack);
	virtual void AddParam(unsigned char nPayloadType,unsigned nClockRate,int nSamplePerFrame);
	void InitRtpFromParam(unsigned char nPayloadType);
	virtual void InitRtpParam2(int nMaxFrameSize, int nNetBand, int nFrameRate,int nMedia,int nCodecType,int nSSRC ,int nDir =3);//nMedia 1 audio 2 video 3 H239 4 Data //nDir 1 recv , 2 send , 3 sendrecv
	virtual void SendRecvPause(int nDir =3);//nDir 1 recv , 2 send , 3 sendrecv
	virtual void SendRecvStart(int nDir =3,void * pCallBack=NULL);//nDir 1 recv , 2 send , 3 sendrecv ,pCallBack h323 Use

    BOOL32 SetRecvParam();
    BOOL32 SetSendParam();
	void GetKeyParam(int nDir,int algorithmId,int & keySize,int & nMode);
	void StartRTP(int nDir);
	void GetRecvStatistics(THpRcvStatistics &tHpRcvStatistics);
	void GetSendStatistics(THpSndStatistics &tHpSndStatistics);

protected:
	MediaNet_RTPSessionCallback&    m_rRTPCallback;
	void *                          m_pH323ChanCallBack;                //H323 Use
	int                             m_nProtocolType;                    // 1 sip 2 H323

	unsigned						m_nClockRate;						//clock rate		
	unsigned						m_nSamplesPerFrame;					//samples per frame	
	int                             m_nMaxFrameSize;
	int                             m_nNetBand;
	int                             m_nFrameRate;
	int                             m_nRecvMediaType;
    int                             m_nRecvPayloadType;
	int                             m_nSendMediaType;
    int                             m_nSendPayloadType;

	std::string						m_strLocalIP;
	std::string						m_strRemoteIP;
	unsigned short					m_nLocalRTPPort;
	unsigned short					m_nLocalRTCPPort;
	unsigned short					m_nRemoteRTPPort;
	unsigned short					m_nRemoteRTCPPort;
	unsigned char*			        m_pDataBuf;
	int                             m_nMaxRtpBufLen;

    BOOL32                          m_bAllowChangeRemoteAddr;
    BOOL32                          m_bNeedChangeRemoteAddr;

    BOOL32                          m_bNetSndRunning;
    BOOL32                          m_bNetRcvRunning;

    int                             m_nFlagReliable;

public:
	CHpMediaSnd *                   m_pMediaSnd;
	CHpMediaRcv *                   m_pMediaRcv;
	bool                            m_bRecv;
	bool                            m_bSend;
	DWORD                           m_nRecvThreadID;

};

#endif
