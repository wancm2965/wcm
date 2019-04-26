//RTPHeader.h
#ifndef __RTPHEADER_H__
#define __RTPHEADER_H__


#ifdef RTP_VERSION
#undef RTP_VERSION
#endif
#define RTP_VERSION			0x02

#ifdef RTP_HEADER_LEN
#undef RTP_HEADER_LEN
#endif
#define RTP_HEADER_LEN		0x0C

/// RTP packet header
typedef struct tagRTP_HEADER
{
#if (1)
    unsigned short count:4;              // csrc count
    unsigned short extension:1;          // header extension flag
    unsigned short padding:1;            // padding flag - for encryption
    unsigned short version:2;            // protocol version


    unsigned short type:7;               // payload type
    unsigned short marker:1;             // marker bit - for profile
#else

    unsigned char version:2;             // protocol version
    unsigned char padding:1;             // padding flag - for encryption
    unsigned char extension:1;           // header extension flag
    unsigned char count:4;               // csrc count

    unsigned char marker:1;              // marker bit - for profile
    unsigned char type:7;                // payload type


#endif

    unsigned short sequence;          // sequence number of this packet 
                                    // (needs endian conversion here, 
                                    // done in the access methods )

    /// timestamp of this packet
    unsigned long timestamp;
    /// source of packet
    unsigned long ssrc;
}RTP_HEADER,*PRTP_HEADER;

enum {
	RTP_PAYLOAD_G711U=0,
	RTP_PAYLOAD_GSM=3,
	RTP_PAYLOAD_G723=4,
	RTP_PAYLOAD_G711A=8,
	RTP_PAYLOAD_G722=9,
	RTP_PAYLOAD_G729=18,
	RTP_PAYLOAD_G7221_7_24=121,
	RTP_PAYLOAD_G7221_7_32=122,
	RTP_PAYLOAD_G7221_14_24=123,
	RTP_PAYLOAD_G7221_14_32=124,
	RTP_PAYLOAD_G7221_14_48=125,
};

#endif
