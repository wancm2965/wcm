// AVSCTPHeader.h: interface for the AVSCTPHeader class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVSCTPHEADER_H__
#define __AVSCTPHEADER_H__

#include "AVBufferPool.h"


#define AVSCTP_HEADER_LENGTH				(12)
#define AVSCTP_VERSION						(0)

enum
{
	AVSCTP_COMMAND_STREAM=0,
	AVSCTP_COMMAND_STREAM_ACK
};

typedef struct tagAVSCTPHEADER
{
    unsigned short	SEQ;            // sequence(0-65535)
#if (1)
    unsigned char	CMD:5;			// command(0-31)
    unsigned char	FLG:1;          // flag - for send data ack(0-1)
    unsigned char	VER:2;          // protocol version(0-3)
#else
    unsigned char	VER:2;          // protocol version(0-3)
    unsigned char	FLG:1;          // flag - for send data ack(0-1)
	unsigned char	CMD:5;    		// command(0-31)
#endif
#if (1)
	unsigned char	KEY:1;         	// key
	unsigned char	END:1;         	// end
    unsigned char	BGN:1;          // begin 
    unsigned char	WND:5;          // windowsize(0-31)
#else
    unsigned char	WND:5;          // windowsize(0-31)
    unsigned char	BGN:1;          // begin 
	unsigned char	END:1;         	// end
	unsigned char	KEY:1;         	// key
#endif
}AVSCTPHEADER,*PAVSCTPHEADER;


void SetAVSCTPHeaderVersion(PAVSCTPHEADER pAVSCTPHeader,unsigned char ucVersion);
void SetAVSCTPHeaderFlag(PAVSCTPHEADER pAVSCTPHeader,bool bFlag);
void SetAVSCTPHeaderCommand(PAVSCTPHEADER pAVSCTPHeader,unsigned char ucCommand);
void SetAVSCTPHeaderSequence(PAVSCTPHEADER pAVSCTPHeader,unsigned short usSequence);
unsigned char GetAVSCTPHeaderVersion(PAVSCTPHEADER pAVSCTPHeader);
bool GetAVSCTPHeaderFlag(PAVSCTPHEADER pAVSCTPHeader);
unsigned char GetAVSCTPHeaderCommand(PAVSCTPHEADER pAVSCTPHeader);
unsigned short GetAVSCTPHeaderSequence(PAVSCTPHEADER pAVSCTPHeader);
void SetAVSCTPHeaderWindowSize(PAVSCTPHEADER pAVSCTPHeader,unsigned char ucWindowSize);
void SetAVSCTPHeaderBegin(PAVSCTPHEADER pAVSCTPHeader,bool bBegin);
void SetAVSCTPHeaderEnd(PAVSCTPHEADER pAVSCTPHeader,bool bEnd);
void SetAVSCTPHeaderKey(PAVSCTPHEADER pAVSCTPHeader,bool bKey);
unsigned char GetAVSCTPHeaderWindowSize(PAVSCTPHEADER pAVSCTPHeader);
bool GetAVSCTPHeaderBegin(PAVSCTPHEADER pAVSCTPHeader);
bool GetAVSCTPHeaderEnd(PAVSCTPHEADER pAVSCTPHeader);
bool GetAVSCTPHeaderKey(PAVSCTPHEADER pAVSCTPHeader);
unsigned char GetAVSCTPHeaderChannelCommand(PAVSCTPHEADER pAVSCTPHeader);

class AFX_EXT_CLASS AVSCTPPacket
{
public:
	AVSCTPPacket(const char*pPacketData,int nPacketDataLen);
	AVSCTPPacket(int nPacketDataLen);
	AVSCTPPacket(AVSCTPPacket*pAVSCTPPacket);
	virtual~AVSCTPPacket(void);
public:
	unsigned char GetVersion(void);
	unsigned char GetCommand(void);
	unsigned short GetSequence(void);
	bool GetFlag(void);
	bool GetBegin(void);
	bool GetEnd(void);
	bool GetKey(void);
	unsigned char GetWindowSize(void);

	void SetVersion(unsigned char ucVersion);
	void SetCommand(unsigned char ucCommand);
	void SetSequence(unsigned short usSequence);
	void SetFlag(bool bFlag);
	void SetBegin(bool bBegin);
	void SetEnd(bool bEnd);
	void SetKey(bool bKey);
	void SetWindowSize(unsigned char ucWindowSize);

	char*GetPacketData(void);
	int GetPacketDataLen(void);
	bool ShouldBeResent(unsigned long ulRTT);
	void SetPacketDataLen(int nPacketDataLen);
	unsigned long GetLastSentTimestamp(void);
	unsigned long GetResentCount(void);
protected:
	PAVSCTPHEADER	m_pAVSCTPHeader;
	unsigned long	m_ulResentCount;
	unsigned long	m_ulLastResentTimestamp;
	char*			m_pPacketData;
	int				m_nPacketDataLen;
	bool			m_bNewBuffer;
};



#endif 
