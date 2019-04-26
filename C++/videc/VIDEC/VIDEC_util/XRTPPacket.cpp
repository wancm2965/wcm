// XRTPPacket.cpp: implementation of the XRTPPacket class.
//
//////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include "XRTPPacket.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
XRTPPacket::XRTPPacket (int nNewPayloadSize, int nPadSize, int csrc_count)
{
    m_pPacketData = NULL;
    m_pRTPHeader = NULL;

    // check given paramters
    assert (csrc_count >= 0);
    assert (csrc_count <= 15);
    assert (nNewPayloadSize >= 0);
    assert (nPadSize >= 0);

    // create memory allocation
    m_nPacketAlloc = sizeof(RTPHeader) - sizeof(unsigned long)
                  + 0/*csrc_count * sizeof(unsigned long)*/ + nNewPayloadSize + nPadSize;
    //assert (m_nPacketAlloc < RTP_MTU);
    NEWRECORD( m_pPacketData , new char[m_nPacketAlloc] )
	NULLReturn( m_pPacketData )  
	m_bNewData=true;
    assert (m_pPacketData);
    memset (m_pPacketData, 0, m_nPacketAlloc);

    // set private variables
    m_pRTPHeader = reinterpret_cast < RTPHeader* > (m_pPacketData);
    SetPadbyteSize (nPadSize);
    SetPayloadUsage (0);
    assert (m_nUnusedSize == nNewPayloadSize);

    // set rtp m_pRTPHeader values
    m_pRTPHeader->version = RTP_VERSION;
    m_pRTPHeader->padding = (nPadSize > 0) ? 1 : 0;
    m_pRTPHeader->extension = 0;
    m_pRTPHeader->count = csrc_count;
    m_pRTPHeader->marker = 0;
    m_pRTPHeader->type = rtpPayloadUndefined;
    m_pRTPHeader->sequence = 0;
    m_pRTPHeader->timestamp = 0;

    // set flags
    m_bSequenceSet = false;
    m_bTimestampSet = false;

}


// clones the rtp m_pRTPHeader
XRTPPacket::XRTPPacket (XRTPPacket* pClone, int nNewPayloadSize)
{
    m_pPacketData = NULL;
    m_pRTPHeader = NULL;

    // create memory allocation
    m_nPacketAlloc = sizeof(RTPHeader) - sizeof(unsigned long)
                  + 0/*pClone->GetCSRCcount() * sizeof(unsigned long)*/
                  + nNewPayloadSize + pClone->GetPadbyteSize();
    //assert (m_nPacketAlloc < RTP_MTU);
    NEWRECORD( m_pPacketData , new char[m_nPacketAlloc] )
	NULLReturn( m_pPacketData )  
 	m_bNewData=true;
	assert (m_pPacketData);
    memset (m_pPacketData, 0, m_nPacketAlloc);

    // set private variables
    m_pRTPHeader = reinterpret_cast < RTPHeader* > (m_pPacketData);
    SetPadbyteSize (pClone->GetPadbyteSize());
    SetPayloadUsage (0);
    assert (m_nUnusedSize == nNewPayloadSize);

    // pClone m_pRTPHeader
    SetVersion (pClone->GetVersion());
    SetPaddingFlag (pClone->GetPaddingFlag());
    SetExtFlag (pClone->GetExtFlag());
    SetCSRCcount (pClone->GetCSRCcount());
    SetMarkerFlag (pClone->GetMarkerFlag());
    SetPayloadType (pClone->GetPayloadType());
    SetSequence (pClone->GetSequence());
    SetRTPTime (pClone->GetRTPTime());
    assert (GetSequence() == pClone->GetSequence());

    // set flags
    m_bSequenceSet = false;
    m_bTimestampSet = false;
}

XRTPPacket::XRTPPacket(char*szData,int nLen)
{
	m_bNewData=false;
    m_pPacketData = szData;
    m_pRTPHeader = reinterpret_cast < RTPHeader* > (szData);
	m_nPacketAlloc=nLen;
	m_nUnusedSize=0;
}

XRTPPacket::~XRTPPacket ()
{
    if (m_pPacketData != NULL && m_bNewData)
    {
        delete []m_pPacketData; 
		m_pPacketData = NULL;
    }
    m_pRTPHeader = NULL;
}



/* --- Size and Locations ------------------------------------------ */


/*     payload          */
char* XRTPPacket::GetPayloadLoc ()
{
    assert (m_pRTPHeader);
    return (m_pPacketData + sizeof(RTPHeader) - sizeof(unsigned long)
            + 0/*(m_pRTPHeader->count)*sizeof(unsigned long)*/);
}

int XRTPPacket::GetPayloadSize ()
{
    assert (m_pRTPHeader);
    return (m_nPacketAlloc - sizeof(RTPHeader) + sizeof(unsigned long)
            - 0/*(m_pRTPHeader->count)*sizeof(unsigned long)*/ - GetPadbyteSize());
}

void XRTPPacket::SetPayloadUsage (int size)
{
    assert (size <= GetPayloadSize());
    m_nUnusedSize = GetPayloadSize() - size;
}

int XRTPPacket::GetPayloadUsage ()
{
    return GetPayloadSize() - m_nUnusedSize;
}



/*     padbyte          */
char* XRTPPacket::GetPadbyteLoc ()
{
    return GetPayloadLoc() + GetPayloadSize();
}

void XRTPPacket::SetPadbyteSize (int size)
{
    // future: not implemented
    // ? write size to last octlet of m_pPacketData
}


int XRTPPacket::GetPadbyteSize ()
{
    // future: not implemented
    // ? read last octlet of m_pPacketData
    return 0;
}



/* --- Packet Information ------------------------------------------ */

/*     payload type     */
void XRTPPacket::SetPayloadType (RTPPayloadType type)
{
    //assert (type >= 0);
    //assert (type <= 127);
    m_pRTPHeader->type = type;
}

RTPPayloadType XRTPPacket::GetPayloadType ()
{
    return static_cast < RTPPayloadType > (m_pRTPHeader->type);
}



/*     sequence number  */
void XRTPPacket::SetSequence (unsigned short nseq)
{
    assert (m_pRTPHeader);
    m_bSequenceSet = true;
    m_pRTPHeader->sequence = htons(nseq);
}

unsigned short XRTPPacket::GetSequence ()
{
    assert (m_pRTPHeader);
    return ntohs(m_pRTPHeader->sequence);
}



/*     timestamp        */
void XRTPPacket::SetRTPTime (unsigned long time)
{
    assert (m_pRTPHeader);
    m_bTimestampSet = true;
    m_pRTPHeader->timestamp = htonl(time);
}

unsigned long XRTPPacket::GetRTPTime ()
{
    assert (m_pRTPHeader);
    return ntohl(m_pRTPHeader->timestamp);
}



/*     ssrc             */
void XRTPPacket::SetSSRC (unsigned long src)
{
    assert (m_pRTPHeader);
    m_pRTPHeader->ssrc = htonl(src);
}

unsigned long XRTPPacket::GetSSRC ()
{
    assert (m_pRTPHeader);
    return ntohl(m_pRTPHeader->ssrc);
}



/*     csrc             */
int XRTPPacket::GetCSRCcount ()
{
    assert (m_pRTPHeader);
    return m_pRTPHeader->count;
}

// use with caution
void XRTPPacket::SetCSRCcount (int i)
{
    assert (m_pRTPHeader);
    m_pRTPHeader->count = i;
}

void XRTPPacket::SetCSRC (unsigned long src, unsigned int i)
{
    assert (m_pRTPHeader);
    assert (i >= 1);
    assert (i <= m_pRTPHeader->count);

    unsigned long* srcPtr = &(m_pRTPHeader->startofcsrc);
    *(srcPtr + i - 1) = htonl(src);
}

unsigned long XRTPPacket::GetCSRC (unsigned int i)
{
    assert (m_pRTPHeader);
    assert (i >= 1);
    assert (i <= m_pRTPHeader->count);

    unsigned long* srcPtr = &(m_pRTPHeader->startofcsrc);
    return ntohl(*(srcPtr + i - 1));
}


bool XRTPPacket::IsValid()
{
    // check packet size
    if( GetTotalUsage() <= 0 )
	{
        return false;
	}

    // check version
    if( GetVersion() != RTP_VERSION )
    {
        return false;
    }

    return true;
}



