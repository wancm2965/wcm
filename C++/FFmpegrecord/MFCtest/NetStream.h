#pragma once
#include "NETECReceiveStream/INETECRecieveStream.h"
#include "FFmpegrecord/IFFmpegrecord.h"
#include "VIDEC/VIDEC_Header.h"
#include "AUDEC/AUDEC_Header.h"
#include "AUDEC/AUDEC_CodecID.h"
typedef struct tagRTP_HEADER
{
	unsigned short count : 4;              // csrc count
	unsigned short extension : 1;          // header extension flag
	unsigned short padding : 1;            // padding flag - for encryption
	unsigned short version : 2;            // protocol version


	unsigned short type : 7;               // payload type
	unsigned short marker : 1;             // marker bit - for profile

	unsigned short sequence;          // sequence number of this packet 
	// (needs endian conversion here, 
	// done in the access methods )

	/// timestamp of this packet
	unsigned long timestamp;
	/// source of packet
	unsigned long ssrc;
}RTP_HEADER, *PRTP_HEADER;


class CNetStream :
	public CNETECRecievestreamCallback
{
public:
	CNetStream();
	~CNetStream();
	void startNete();
	int openNete();
	void SetReadStreamNotify(IFFmpegrecord*precord);
	virtual void OnNETECRecievestreamCallbackVideoData(unsigned char*pData, int nLen);
	virtual void OnNETECRecievestreamCallbackAudioData(unsigned char*pData, int nLen);

	int AudecHeaderToRTPHeader(char* pRTPPacket, char* pData, int nLen);
private:
	INETECRecievestream* m_pNeteStream;
	IFFmpegrecord*       m_precord;
	bool				 m_bAudioStop;
	bool				 m_bVideoStop;
	bool                 m_bGetStream;
	int					 m_nRtpTimestamp;
	int					 m_nRtpSeqNo;
};

