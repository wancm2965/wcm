// XRTPPacket.h: interface for the RTPPacket class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XRTPPACKET_H__
#define __XRTPPACKET_H__

#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>

/// Version of this RTP, always 2
const int RTP_VERSION = 2;

const int RTP_SEQ_MOD = 1 << 16;

/// Maximum UDP packet size, 8129*8
const int RTP_MTU = 1360;

// Supported RTP payloads
typedef enum
{
    rtpPayloadPCMU = 0,
    rtpPayload1016 = 1,
    rtpPayloadG726_32 = 2,
    rtpPayloadGSM = 3,
    rtpPayloadG723 = 4,
    rtpPayloadDVI4_8KHz = 5,
    rtpPayloadDVI4_16KHz = 6,
    rtpPayloadLPC = 7,
    rtpPayloadPCMA = 8,
    rtpPayloadG722 = 9,
    rtpPayloadL16_stereo = 10,
    rtpPayloadL16_mono = 11,
    rtpPayloadQCELP = 12,
    rtpPayloadMPA = 14,
    rtpPayloadG728 = 15,
    rtpPayloadDVI4_11KHz = 16,
    rtpPayloadDVI4_22KHz = 17,
    rtpPayloadG729 = 18,
    rtpPayloadCN = 19,
    rtpPayloadGSMEFR = 20,
    rtpPayloadG726_40 = 21,
    rtpPayloadG726_24 = 22,
    rtpPayloadG726_16 = 23,
    rtpPayloadCelB = 25,
    rtpPayloadJPEG = 26,
    rtpPayloadNV = 28,
    rtpPayloadH261 = 31,
    rtpPayloadMPV = 32,
    rtpPayloadMP2T = 33,
    rtpPayloadH263 = 34,
	rtpPayloadUndefined = 128
} RTPPayloadType;


/// RTP packet header
struct RTPHeaderStruct
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
    /// list of contributing sources
	unsigned long startofcsrc;
};
typedef struct RTPHeaderStruct RTPHeader;

#define RTP_HEADER_LEN sizeof(RTPHeader)-sizeof(unsigned long)

class XRTPPacket  
{
public:
    /** Constructor that allocates RTP packetData
        @param newpayloadSize Payload size not including RTP header 
        @param npadSize Number of pad bytes (not implemented)
        @param csrc_count Number of contributing sources in packet
     **/
	XRTPPacket(int nNewPayloadSize, int nPadSize = 0, int csrc_count = 0);


    /// clones an existing packet to a new size
    XRTPPacket (XRTPPacket* pClonePacket, int nNewPayloadSize);

	XRTPPacket(char*szData,int nLen);

    /// Destructor that deallocates RTP packetData memory
	virtual ~XRTPPacket();


    /// Pointer to packet data
    char* GetPacketData ()
    {
        return m_pPacketData;
    }

    /// Pointer to packet header
    RTPHeader* GetHeader ()
    {
        return m_pRTPHeader;
    }

    /// Pointer to beginning of payload
    char* GetPayloadLoc ();

    /// Maximum payload size
    int GetPayloadSize ();

    /** Sets payload usage
        @param size doesn't include RTP header
     **/
    void SetPayloadUsage (int nSize);

    /// Size of payload stored
    int GetPayloadUsage ();

    /// Pointer to begnning of padbyte (Not implemented)
    char* GetPadbyteLoc ();

    /// Sets size of payload (Not implemented)
    void SetPadbyteSize (int size);

    /// Size of padbyte (Not fully implemented)
    int GetPadbyteSize ();

    /// Entire size of RTP packet including header, unused, and padbyte
    int GetPacketAlloc ()
    {
        return m_nPacketAlloc;
    }

    /// Size of unused memory
    int GetUnused ()
    {
        return m_nUnusedSize;
    }

    /** Sets size of RTP packet including header and padbyte
        Extra memory will be set as unused memory
     **/
    void SetTotalUsage (int nSize)
    {
        assert (nSize <= m_nPacketAlloc);
        m_nUnusedSize = m_nPacketAlloc - nSize;
    }

    /// Size of RTP packet not including unused memory
    int GetTotalUsage ()
    {
        return m_nPacketAlloc - m_nUnusedSize;
    }


    ///
    void SetPayloadType (RTPPayloadType payloadType);
    ///
    RTPPayloadType GetPayloadType ();

    ///
    void SetSequence (unsigned short usNewSeq);
    ///
    unsigned short GetSequence ();

    ///
    unsigned long GetRTPTime();
    ///
    void SetRTPTime (unsigned long ulRTPTime);

    ///
    void SetSSRC (unsigned long ulSSRC);
    ///
    unsigned long GetSSRC ();

    /// Gets number of contributing sources
    void SetCSRCcount (int i);   // use with cuation
    int GetCSRCcount ();

    ///
    void SetCSRC (unsigned long src, unsigned int index);

    /** index value less range 1 to csrc_count
        @param index value less range 1 to csrc_count
     **/
    unsigned long GetCSRC (unsigned int index);


    ///
    void SetVersion (int i)
    {
        m_pRTPHeader->version = i;
    }
    int GetVersion ()
    {
        return m_pRTPHeader->version;
    }
    ///
    void SetPaddingFlag (int i)
    {
        m_pRTPHeader->padding = i;
    }
    int GetPaddingFlag ()
    {
        return m_pRTPHeader->padding;
    }
    ///
    void SetExtFlag (int i)
    {
        m_pRTPHeader->extension = i;
    }
    int GetExtFlag ()
    {
        return m_pRTPHeader->extension;
    }
    ///
    void SetMarkerFlag (int i)
    {
        m_pRTPHeader->marker = i;
    }
    int GetMarkerFlag ()
    {
        return m_pRTPHeader->marker;
    }
    /// valid check
    bool IsValid();
public:
    /// flags
    bool		m_bSequenceSet;
    bool		m_bTimestampSet;
private:
    /// Pointer to raw memory allocation
    char*		m_pPacketData;
	bool		m_bNewData;

    /// Allocated packet size
    int			m_nPacketAlloc;

    /// Size of unused memory
    int			m_nUnusedSize;

    /// Easy access to header information
    RTPHeader*	m_pRTPHeader;
};

#endif 