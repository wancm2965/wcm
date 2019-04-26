/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/


#include "rvstdio.h"
#include "rvmemory.h"
#include "rvtimestamp.h"
#include "rvrandomgenerator.h"
#include "rvhost.h"
#include "rvsocket.h"
#include "rvselect.h"
#include "rvcbase.h"

#include "bitfield.h"
#include "rtputil.h"

#include "rtp.h"
#include "rtcp.h"



#ifdef __cplusplus
extern "C" {
#endif


#define BUILD_VER_NUM(_max, _min, _dot1, _dot2) \
    ((RvUint32)((_max << 24) + (_min << 16) + (_dot1 << 8) + _dot2))

#define VERSION_STR    "4.2.0.0"
#define VERSION_NUM    BUILD_VER_NUM(4, 2, 0, 0)


#define RV_RTP_MAXIPS RvUint32Const(20)


/* Internal RTP instance used. There a single such object */
typedef struct
{
    RvSelectEngine*     selectEngine; /* Select engine used */

    RvUint32            localIp; /* IP to bind to */
    RvUint32            hostIPs[RV_RTP_MAXIPS]; /* Local host IPs */
    RvUint32            timesInitialized; /* Times the RTP was initialized */

    RvRandomGenerator   randomGenerator; /* Random numbers generator to use */

} RvRtpInstance;


#define logMgr NULL

/* RTP instance to use */
static RvRtpInstance rvRtpInstance;








/* local functions */
static RvBool isMyIP(RvUint32 ip);
static void rtpEvent(
        IN RvSelectEngine*  selectEngine,
        IN RvSelectFd*      fd,
        IN RvSelectEvents   selectEvent,
        IN RvBool           error);



                       /* == Basic RTP Functions == */


RVAPI
RvInt32 RVCALLCONV rtpInit(void)
{
    RvStatus status;

    status = RvCBaseInit();
    if (status != RV_OK)
        return status;

    if (rvRtpInstance.timesInitialized == 0)
    {
        RvAddress addresses[RV_RTP_MAXIPS];
        RvUint32 numAddrs = RV_RTP_MAXIPS;
        RvUint32 i;

        /* Find the list of host addresses we have */
        status = RvHostLocalGetAddress(logMgr, &numAddrs, addresses);
        if (status != RV_OK)
        {
            RvCBaseEnd();
            return status;
        }

        for (i = 0; i < numAddrs; i++)
            rvRtpInstance.hostIPs[i] = RvAddressIpv4GetIp(RvAddressGetIpv4(addresses + i));

        while (i > 0)
        {
            i--;
            RvAddressDestruct(addresses+i);
        }

        /* Create a random generator */
        RvRandomGeneratorConstruct(&rvRtpInstance.randomGenerator,
            (RvRandom)RvInt64ToRvUint32(RvInt64ShiftRight(RvTimestampGet(logMgr), 8)));
    }

    status = RvSelectConstruct(2048, 0, logMgr, &rvRtpInstance.selectEngine);
    if (status != RV_OK)
    {
        RvCBaseEnd();
        return status;
    }

    rvRtpInstance.timesInitialized++;

    return RV_OK;
}


RVAPI
RvInt32 RVCALLCONV rtpInitEx(RvUint32 ip)
{
    RvInt32 rc;

    if ((rc=rtpInit()) != RV_ERROR_UNKNOWN)
        rvRtpInstance.localIp = ip;

    return rc;
}


/************************************************************************************
 * rtpSetLocalAddress
 * description: Set the local address to use for calls to rtpOpenXXX functions.
 *              This parameter overrides the value given in rtpInitEx() for all
 *              subsequent calls.
 * input: ip    - Local IP address to use
 * output: none.
 * return value: Non-negative value on success
 *               negative value on failure
 ***********************************************************************************/
RVAPI
int RVCALLCONV rtpSetLocalAddress(IN RvUint32 ip)
{
    rvRtpInstance.localIp = ip;
    return RV_OK;
}


RVAPI
void RVCALLCONV rtpEnd(void)
{
    rvRtpInstance.timesInitialized--;

    if (rvRtpInstance.timesInitialized == 0)
    {
        RvRandomGeneratorDestruct(&rvRtpInstance.randomGenerator);
    }

    RvSelectDestruct(rvRtpInstance.selectEngine, 0);
    RvCBaseEnd();
}


RVAPI
int RVCALLCONV rtpGetAllocationSize(void)
{
    return sizeof(rtpSession);
}



/************************************************************************************
 * rtpOpenFrom
 * description: Opens an RTP session in the memory that the application allocated.
 * input: port        - The UDP port number to be used for the RTP session.
 *        ssrcPattern - Synchronization source Pattern value for the RTP session.
 *        ssrcMask    - Synchronization source Mask value for the RTP session.
 *        buffer      - Application allocated buffer with a value no less than the
 *                      value returned by the function rtpGetAllocationSize().
 *        bufferSize  - size of the buffer.
 * output: none.
 * return value: If no error occurs, the function returns the handle for the opened RTP
 *               session. Otherwise, it returns NULL.
 ***********************************************************************************/
RVAPI
HRTPSESSION RVCALLCONV rtpOpenFrom(
        IN  RvUint16    port,
        IN  RvUint32    ssrcPattern,
        IN  RvUint32    ssrcMask,
        IN  void*       buffer,
        IN  int         bufferSize)
{
    rtpSession *s = (rtpSession *)buffer;
    RvAddress localAddress;
    RvRandom randomValue;
    RvStatus res;

    if (bufferSize < rtpGetAllocationSize())
        return NULL;

    memset(buffer, 0 , (RvSize_t)rtpGetAllocationSize());

    /* Get a random value for the beginning sequence number */
    RvRandomGeneratorGetValue(&rvRtpInstance.randomGenerator, &randomValue);

    s->isAllocated    = RV_FALSE;
    s->sSrcPattern    = ssrcPattern;
    s->sSrcMask       = ssrcMask;
    s->sequenceNumber = (RvUint16)randomValue;

    RvAddressConstructIpv4(&localAddress, rvRtpInstance.localIp, port);

    /* Open and bind the socket */
    res = RvSocketConstruct(RV_ADDRESS_TYPE_IPV4, RvSocketProtocolUdp, logMgr, &s->socket);
    if (res == RV_OK)
    {
        res = RvSocketSetBuffers(&s->socket, 8192, 8192, logMgr);
        if (res == RV_OK)
            res = RvSocketSetBroadcast(&s->socket, RV_TRUE, logMgr);
        if (res == RV_OK)
            res = RvSocketSetBlocking(&s->socket, RV_TRUE, logMgr);
        if (res == RV_OK)
            res = RvSocketBind(&s->socket, &localAddress, NULL, logMgr);

        if (res != RV_OK)
            RvSocketDestruct(&s->socket, RV_FALSE, NULL, logMgr);
    }

    RvAddressDestruct(&localAddress);

    if (res == RV_OK)
    {
        rtpRegenSSRC((HRTPSESSION)s);
        return (HRTPSESSION)s;
    }
    else
    {
        return (HRTPSESSION)NULL;
    }
}


/************************************************************************************
 * rtpOpen
 * description: Opens an RTP session. The RTP Stack allocates an object and the
 *              memory needed for the RTP session. It also opens a socket and waits
 *              for packets. rtpOpen() also returns the handle of this session to
 *              the application.
 * input: port        - The UDP port number to be used for the RTP session.
 *        ssrcPattern - Synchronization source Pattern value for the RTP session.
 *        ssrcMask    - Synchronization source Mask value for the RTP session.
 * output: none.
 * return value: If no error occurs, the function returns the handle for the opened RTP
 *               session. Otherwise, it returns NULL.
 ***********************************************************************************/
RVAPI
HRTPSESSION RVCALLCONV rtpOpen(
        IN  RvUint16    port,
        IN  RvUint32    ssrcPattern,
        IN  RvUint32    ssrcMask)
{
    return rtpOpenEx(port, ssrcPattern, ssrcMask, NULL);
}

/************************************************************************************
 * rtpOpenEx
 * description: Opens an RTP session and an associated RTCP session.
 * input: port        - The UDP port number to be used for the RTP session.
 *        ssrcPattern - Synchronization source Pattern value for the RTP session.
 *        ssrcMask    - Synchronization source Mask value for the RTP session.
 *        cname       - The unique name representing the source of the RTP data.
 * output: none.
 * return value: If no error occurs, the function returns the handle for the open
 *               RTP session. Otherwise, the function returns NULL.
 ***********************************************************************************/
RVAPI
HRTPSESSION RVCALLCONV rtpOpenEx(
        IN  RvUint16    port,
        IN  RvUint32    ssrcPattern,
        IN  RvUint32    ssrcMask,
        IN  char *      cname)
{
    /* allocate rtp session.*/
    rtpSession* s;
    RvStatus res;

    res = RvMemoryAlloc(NULL, (RvSize_t)rtpGetAllocationSize(), logMgr, (void**)&s);
    if (res != RV_OK)
        return NULL;

    if ((rtpSession *)rtpOpenFrom(port, ssrcPattern, ssrcMask, (void*)s, rtpGetAllocationSize())==NULL)
    {
        RvMemoryFree(s, logMgr);
        return NULL;
    }
    s->isAllocated=RV_TRUE;
    if (cname)
    {
        /* Open a new rtcp session.The port for an RTCP session is always
           (RTP port + 1).*/
        s->hRTCP = rtcpOpen(s->sSrc, (RvUint16)((port)?port+1:port), cname);
        if (s->hRTCP == NULL)
        {
            /* Bad RTCP - let's just kill the RTP and be done with it */
            rtpClose((HRTPSESSION)s);
            s = NULL;
        }
    }

    return (HRTPSESSION)s;
}

/************************************************************************************
 * rtpClose
 * description: Close RTP session.
 * input: hRTP - Handle of the RTP session.
 * output: none.
 * return value: If an error occurs, the function returns a negative value.
 *               If no error occurs, the function returns a non-negative value.
 ***********************************************************************************/
RVAPI
RvUint32 RVCALLCONV rtpClose(
        IN  HRTPSESSION  hRTP)
{
    rtpSession *s = (rtpSession *)hRTP;

    if (s)
    {
        if (s->hRTCP)
            rtcpClose(s->hRTCP);

        if (s->eventHandler != NULL)
        {
            /* We're probably also in non-blocking mode */
            RvSelectRemove(s->selectEngine, &s->selectFd);
            RvFdDestruct(&s->selectFd);
        }
        else
        {
            /* Send UDP data through specified socket to the local host */
            rtpResume(hRTP);
        }

        /* This function closes the specified IP socket and all the socket's connections.*/
        RvSocketDestruct(&s->socket, RV_FALSE, NULL, logMgr);

        if (s->remoteAddressSet)
        {
            RvAddressDestruct(&s->remoteAddress);
        }
        if (s->isAllocated)
            RvMemoryFree(s, logMgr);
    }

    return RV_OK;
}

/************************************************************************************
 * rtpGetSSRC
 * description: Returns the current SSRC (synchronization source value) of the RTP session.
 * input: hRTP - Handle of the RTP session.
 * output: none.
 * return value: If no error occurs, the function returns the current SSRC value.
 *               Otherwise, it returns a negative value.
 ***********************************************************************************/
RVAPI
RvUint32 RVCALLCONV rtpGetSSRC(
        IN  HRTPSESSION  hRTP)
{
    rtpSession *s = (rtpSession *)hRTP;

    return s->sSrc;
}

/************************************************************************************
 * rtpSetEventHandler
 * description: Set an Event Handler for the RTP session. The application must set
 *              an Event Handler for each RTP session.
 * input: hRTP          - Handle of the RTP session.
 *        eventHandler  - Pointer to the callback function that is called each time a
 *                        new RTP packet arrives to the RTP session.
 *        context       - The parameter is an application handle that identifies the
 *                        particular RTP session. The application passes the handle to
 *                        the Event Handler.
 * output: none.
 * return value: none.
 ***********************************************************************************/
RVAPI
void RVCALLCONV rtpSetEventHandler(
        IN  HRTPSESSION        hRTP,
        IN  LPRTPEVENTHANDLER  eventHandler,
        IN  void *             context)
{
    rtpSession *s = (rtpSession *)hRTP;
    RvBool hasFd, addFd;

    if (s)
    {
        hasFd = (s->eventHandler != NULL);
        addFd = (eventHandler != NULL);
        s->eventHandler = eventHandler;
        s->context      = context;

        /* Seems like we have to add it to our list of fds */
        if (addFd & (!hasFd))
        {
            RvStatus status;
            status = RvSelectGetThreadEngine(logMgr, &s->selectEngine);
            if ((status != RV_OK) || (s->selectEngine == NULL))
                s->selectEngine = rvRtpInstance.selectEngine;

            RvSocketSetBlocking(&s->socket, RV_FALSE, logMgr);
            RvFdConstruct(&s->selectFd, &s->socket, logMgr);
            RvSelectAdd(s->selectEngine, &s->selectFd, RvSelectRead, rtpEvent);
        }

        /* Seems like we have to remove it from our list of fds */
        if (!addFd & hasFd)
        {
            RvSelectRemove(s->selectEngine, &s->selectFd);
            RvFdDestruct(&s->selectFd);
            RvSocketSetBlocking(&s->socket, RV_TRUE, logMgr);
        }
    }
}

/************************************************************************************
 * rtpSetRemoteAddress
 * description: Defines the address of the remote peer or the address of a multicast
 *              group to which the RTP stream will be sent.
 * input: hRTP  - Handle of the RTP session.
 *        ip    - IP address to which RTP packets should be sent.
 *        port  - UDP port to which RTP packets should be sent.
 * output: none.
 * return value: none.
 ***********************************************************************************/
RVAPI
void RVCALLCONV rtpSetRemoteAddress(
        IN HRTPSESSION  hRTP,   /* RTP Session Opaque Handle */
        IN RvUint32     ip,
        IN RvUint16     port)
{
    rtpSession* s = (rtpSession *)hRTP;

    if (s != NULL)
    {
        if (s->remoteAddressSet)
        {
            RvAddressDestruct(&s->remoteAddress);
        }
        RvAddressConstructIpv4(&s->remoteAddress, ip, port);
        s->remoteAddressSet = RV_TRUE;
    }
}

/************************************************************************************
 * rtpPack
 * description: This routine sets the RTP header.
 * input: hRTP  - Handle of the RTP session.
 *        buf   - Pointer to buffer containing the RTP packet with room before first
 *                payload byte for RTP header.
 *        len   - Length in bytes of buf.
 *        p     - A struct of RTP param.
 * output: none.
 * return value:  If no error occurs, the function returns the non-neagtive value.
 *                Otherwise, it returns a negative value.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpPack(
        IN  HRTPSESSION  hRTP,
        IN  void *       buf,
        IN  RvInt32      len,
        IN  rtpParam *   p)
{
    rtpSession *s = (rtpSession *)hRTP;
    RvUint32 *header;
    RvUint32 seq;

    p->sByte-=12;
    p->len=len - p->sByte;

    if (s->useSequenceNumber)
        s->sequenceNumber=p->sequenceNumber;
    p->sequenceNumber=s->sequenceNumber;
    seq = s->sequenceNumber;

    /* sets the fields inside RTP message.*/
    header=(RvUint32*)((char*)buf + p->sByte);
    header[0]=0;
    header[0]=bitfieldSet(header[0],2,30,2);
    header[0]=bitfieldSet(header[0],p->marker,23,1);
    header[0]=bitfieldSet(header[0],p->payload,16,7);
    header[0]=bitfieldSet(header[0],seq,0,16);
    header[1]=p->timestamp;
    header[2]=s->sSrc;

    /* increment the internal sequence number for this session */
    s->sequenceNumber++;

    /* converts an array of 4-byte integers from host format to network format.*/
    ConvertToNetwork(header, 0, 3);

    return RV_OK;
}

/************************************************************************************
 * rtpWrite
 * description: This routine sends the RTP packet.
 * input: hRTP  - Handle of the RTP session.
 *        buf   - Pointer to buffer containing the RTP packet with room before first
 *                payload byte for RTP header.
 *        len   - Length in bytes of buf.
 *        p     - A struct of RTP param.
 * output: none.
 * return value:  If no error occurs, the function returns the non-neagtive value.
 *                Otherwise, it returns a negative value.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpWrite(
        IN  HRTPSESSION  hRTP,
        IN  void *       buf,
        IN  RvInt32      len,
        IN  rtpParam *   p)
{
    RvStatus res;
    rtpSession *s = (rtpSession *)hRTP;

    res = rtpPack(hRTP, buf, len, p);

    if (res >= 0)
    {
        /* send UDP data through the specified socket to the remote host.*/
        res = RvSocketSendBuffer(&s->socket, (RvUint8*)buf+p->sByte, (RvSize_t)p->len, &s->remoteAddress, logMgr, NULL);
    }

    if (res == RV_OK)
    {
        if ((s->hRTCP != NULL) &&  (res == RV_OK))
        {
            /* inform the RTCP session about a packet that was sent in the corresponding RTP session.*/
            rtcpRTPPacketSent(s->hRTCP, p->len, p->timestamp);
        }
    }

    return res;
}

RVAPI
RvInt32 RVCALLCONV rtpUnpack(
        IN  HRTPSESSION  hRTP,
        IN  void *buf,
        IN  RvInt32 len,
        OUT rtpParam* p)
{
    RvUint32 *header=(RvUint32*)buf;
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(hRTP);

    if (p->len < 12)
    {
        return RV_ERROR_UNKNOWN;
    }
    ConvertFromNetwork(buf, 3, (int)bitfieldGet(header[0], 24, 4));
    p->timestamp=header[1];
    p->sequenceNumber=(RvUint16)bitfieldGet(header[0],0,16);
    p->sSrc=header[2];
    p->marker=bitfieldGet(header[0],23,1);
    p->payload=(unsigned char)bitfieldGet(header[0],16,7);

    p->sByte=12+bitfieldGet(header[0],24,4)*sizeof(RvUint32);

    if (bitfieldGet(header[0],28,1))/*Extension Bit Set*/
    {
        int xStart=p->sByte / sizeof(RvUint32);

        ConvertFromNetwork(buf, xStart, 1);
        p->sByte+=bitfieldGet(header[xStart],0,16)*sizeof(RvUint32);
        if (p->sByte > p->len)
        {
            /* This packet is probably corrupted */
            p->sByte = 12;
            return RV_ERROR_UNKNOWN;
        }
    }

    if (bitfieldGet(header[0],29,1))/*Padding Bit Set*/
    {
        p->len-=((char*)buf)[p->len-1];
    }
    return RV_OK;
}

/************************************************************************************
 * rtpRead
 * description: This routine sets the header of the RTP message.
 * input: hRTP  - Handle of the RTP session.
 *        buf   - Pointer to buffer containing the RTP packet with room before first
 *                payload byte for RTP header.
 *        len   - Length in bytes of buf.
 *
 * output: p    - A struct of RTP param,contain the fields of RTP header.
 * return value: If no error occurs, the function returns the non-negative value.
 *                Otherwise, it returns a negative value.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpRead(
        IN  HRTPSESSION  hRTP,
        IN  void *buf,
        IN  RvInt32 len,
        OUT rtpParam* p)
{
    return rtpReadWithRemoteAddress(hRTP, buf, len, p, NULL, NULL);
}

/************************************************************************************
 * rtpReadEx
 * description: Receives an RTP packet and updates the corresponding RTCP session.
 * input: hRTP      - Handle of the RTP session.
 *        buf       - Pointer to buffer containing the RTP packet with room before first
 *                    payload byte for RTP header.
 *        len       - Length in bytes of buf.
 *        timestamp -
 *        p         - A struct of RTP param,contain the fields of RTP header.
 * output: none.
 * return value: If no error occurs, the function returns the non-neagtive value.
 *               Otherwise, it returns a negative value.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpReadEx(
        IN  HRTPSESSION  hRTP,
        IN  void *       buf,
        IN  RvInt32      len,
        IN  RvUint32     timestamp,
        OUT rtpParam *   p)
{
    rtpSession *s = (rtpSession *)hRTP;
    int retVal;

    retVal = rtpRead(hRTP, buf, len, p);

    if (s->hRTCP  &&  retVal >= 0)
    {
        /* Informs the RTCP session about a packet that was received in the corresponding
           RTP session.*/
        rtcpRTPPacketRecv(s->hRTCP, p->sSrc, timestamp,  p->timestamp, p->sequenceNumber);
    }

    return retVal;
}

/************************************************************************************
 * rtpReadWithRemoteAddress
 * description: This routine reads.RTP packets from the network and passes 
 *                    to the user the remote address
 * input: hRTP  - Handle of the RTP session.
 *        buf   - Pointer to buffer containing the RTP packet with room before first
 *                payload byte for RTP header.
 *        len   - Length in bytes of buf.
 *
 * output: p        - A struct of RTP param,contain the fields of RTP header.
 *         pIP      -pointer to IP
 *         pPort    - pointer to port
 * return value: If no error occurs, the function returns the non-negative value.
 *                Otherwise, it returns a negative value.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpReadWithRemoteAddress(
        IN  HRTPSESSION hRTP,
        IN  void *      buf,
        IN  RvInt32     len,
        OUT rtpParam*   p,
        OUT RvUint32*   pIP,
        OUT RvUint16*   pPort)
{
    rtpSession* s = (rtpSession *)hRTP;
    RvAddress remoteAddress;
    RvSize_t messageSize;
    RvStatus res;
    RvUint32 ip;

    res = RvSocketReceiveBuffer(&s->socket, (RvUint8*)buf, (RvSize_t)len, logMgr, &messageSize, &remoteAddress);
    if (res != RV_OK)
        return res;

    ip = RvAddressIpv4GetIp(RvAddressGetIpv4(&remoteAddress));
    if (pIP != NULL)
    {
        *pIP = ip;
    }
    if (pPort!=NULL)
    {
        *pPort = RvAddressGetIpPort(&remoteAddress);
    }

    RvAddressDestruct(&remoteAddress);

    p->len = (int)messageSize;

    ConvertFromNetwork(buf, 0, 3);

    /* If the SSRC is ours and the is our IP, we should ignore this packet - it probably came
       in as a multicast packet we sent out. */
    if ( (s->sSrc == ((RvUint32*)buf)[2]) && isMyIP(ip) )
        return RV_ERROR_UNKNOWN;

    if (p->len <= 0)
        return RV_ERROR_UNKNOWN;

    return rtpUnpack(hRTP, buf, len, p);
}


/************************************************************************************
 * rtpGetPort
 * description: Returns the current port of the RTP session.
 * input: hRTP - Handle of the RTP session.
 * output: none.
 * return value: If no error occurs, the function returns the current port value.
 *               Otherwise, it returns a negative value.
 ***********************************************************************************/
RVAPI
RvUint16 RVCALLCONV rtpGetPort(
        IN HRTPSESSION  hRTP)   /* RTP Session Opaque Handle */
{
    rtpSession* s = (rtpSession *)hRTP;
    RvAddress localAddress;
    RvUint16 sockPort = 0;
    RvStatus res;

    res = RvSocketGetLocalAddress(&s->socket, logMgr, &localAddress);

    if (res == RV_OK)
    {
        sockPort = RvAddressGetIpPort(&localAddress);

        RvAddressDestruct(&localAddress);
    }

    return sockPort;
}

/************************************************************************************
 * rtpGetVersion
 * description:  Returns the RTP version of the installed RTP Stack.
 * input:  none.
 * output: none.
 * return value: If no error occurs, the function returns the current version value.
 *               Otherwise, it returns a negative value.
 ***********************************************************************************/
RVAPI
char * RVCALLCONV rtpGetVersion(void)
{
    return (char*)VERSION_STR;
}

/************************************************************************************
 * rtpGetVersionNum
 * description:  Returns the RTP version of the installed RTP Stack.
 * input:  none.
 * output: none.
 * return value: If no error occurs, the function returns the current version value.
 *               Otherwise, it returns a negative value.
 ***********************************************************************************/
RVAPI
RvUint32 RVCALLCONV rtpGetVersionNum(void)
{
    return VERSION_NUM;
}


                    /* == ENDS: Basic RTP Functions == */



                     /* == Accessory RTP Functions == */

/************************************************************************************
 * rtpGetRTCPSession
 * description:  Returns the RTCP session.
 * input:  hRTP - Handle of RTP session.
 * output: none.
 * return value: hRTCP - Handle of RTCP session.
 ***********************************************************************************/
RVAPI
HRTCPSESSION RVCALLCONV rtpGetRTCPSession(
        IN  HRTPSESSION  hRTP)
{
    rtpSession *s = (rtpSession *)hRTP;

    return (s->hRTCP);
}

/************************************************************************************
 * rtpSetRTCPSession
 * description:  set the RTCP session.
 * input:  hRTP  - Handle of RTP session.
 *         hRTCP - Handle of RTCP session.
 * output: none.
 * return value:return 0.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpSetRTCPSession(
        IN  HRTPSESSION   hRTP,
        IN  HRTCPSESSION  hRTCP)
{
    rtpSession *s = (rtpSession *)hRTP;

    s->hRTCP = hRTCP;

    return RV_OK;
}

/************************************************************************************
 * rtpGetHeaderLength
 * description:  return the header of RTP message.
 * input:  none.
 * output: none.
 * return value:The return value is twelve.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpGetHeaderLength(void)
{
    return 12;
}

/************************************************************************************
 * rtpRegenSSRC
 * description:  Generates a new synchronization source value for the RTP session.
 *               This function, in conjunction with rtpGetSSRC() may be used to
 *               change the SSRC value when an SSRC collision is detected.
 * input:  hRTP  - Handle of RTP session.
 * output: none.
 * return value: ssrc - If an error occurs, the function returns a negative value.
 *               If no error occurs, the function returns a non-negative value.
 ***********************************************************************************/
RVAPI
RvUint32 RVCALLCONV rtpRegenSSRC(
        IN  HRTPSESSION  hRTP)
{
    rtpSession *s = (rtpSession *)hRTP;
    RvRandom randomValue;

    /* those line is to prevent collision.*/
    RvRandomGeneratorGetValue(&rvRtpInstance.randomGenerator, &randomValue);
    s->sSrc = (RvUint32)randomValue * RvUint64ToRvUint32(RvUint64ShiftRight(RvTimestampGet(logMgr), 16));
    s->sSrc &= ~s->sSrcMask;
    s->sSrc |= s->sSrcPattern;

    return s->sSrc;
}

/************************************************************************************
 * rtpSetGroupAddress
 * description:  Defines a multicast IP for the RTP session.
 * input:  hRTP  - Handle of RTP session.
 *         ip    - Multicast IP address for the RTP session.
 * output: none.
 * return value: return 0.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpSetGroupAddress(
        IN HRTPSESSION hRTP,    /* RTP Session Opaque Handle */
        IN RvUint32     ip)
{
    rtpSession *s = (rtpSession *)hRTP;
    RvAddress addresses[2];
    RvStatus res;

    RvAddressConstructIpv4(&addresses[0], ip, RV_ADDRESS_IPV4_ANYPORT);
    RvAddressConstructIpv4(&addresses[1], rvRtpInstance.localIp, RV_ADDRESS_IPV4_ANYPORT);

    /* This function adds the specified address (in network format) to the specified
       Multicast interface for the specified socket.*/
    res = RvSocketJoinMulticastGroup(&s->socket, addresses, addresses+1, logMgr);

    RvAddressDestruct(&addresses[0]);
    RvAddressDestruct(&addresses[1]);

    return res;
}

/************************************************************************************
 * rtpResume
 * description:  Causes a blocked rtpRead() or rtpReadEx() function running in
 *               another thread to fail.
 * input:  hRTP  - Handle of RTP session.
 * output: none.
 * return value: If an error occurs, the function returns a negative value.
 *               If no error occurs, the function returns a non-negative value.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpResume(
        IN  HRTPSESSION hRTP)
{
    rtpSession *s = (rtpSession *)hRTP;
    RvAddress localAddress;
    RvAddressIpv4* localIpV4;
    RvStatus status;

    status = RvSocketGetLocalAddress(&s->socket, logMgr, &localAddress);

    if (status == RV_OK)
    {
        localIpV4 = (RvAddressIpv4 *)RvAddressGetIpv4(&localAddress);
        if (RvAddressIpv4GetIp(localIpV4) == RV_ADDRESS_IPV4_ANYADDRESS)
        {
            /* No specific IP on this socket - use our default one */
            if (rvRtpInstance.localIp != RV_ADDRESS_IPV4_ANYADDRESS)
                RvAddressIpv4SetIp(localIpV4, rvRtpInstance.localIp);
            else
                RvAddressIpv4SetIp(localIpV4, rvRtpInstance.hostIPs[0]);
        }

        /* We send a dummy buffer to get this connection released from its blocking mode */
        RvSocketSendBuffer(&s->socket, (RvUint8*)"", 1, &localAddress, logMgr, NULL);

        RvAddressDestruct(&localAddress);
    }

    return status;
}

/************************************************************************************
 * rtpUseSequenceNumber
 * description:  Forces the Stack to accept user input for the sequence number of
 *               the RTP packet. The RTP Stack usually determines the sequence number.
 *               However, the application can force its own sequence number.
 *               Call rtpUseSequenceNumber() at the beginning of the RTP session and
 *               then specify the sequence number in the rtpParam structure of the
 *               rtpWrite() function.
 * input:  hRTP  - Handle of RTP session.
 * output: none.
 * return value: return 0.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpUseSequenceNumber(
                IN HRTPSESSION  hRTP)
{
    rtpSession *s = (rtpSession *)hRTP;

    s->useSequenceNumber = 1;

    return RV_OK;
}

/************************************************************************************
 * rtpSetReceiveBufferSize
 * description:  Changes the RTP session receive buffer size.
 * input:  hRTP  - Handle of RTP session.
 * output: none.
 * return value: return 0.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpSetReceiveBufferSize(
        IN HRTPSESSION  hRTP,
        IN int size)
{
    rtpSession* s = (rtpSession *)hRTP;

    /* This function sets the size of the read/write buffers of a socket*/
    return RvSocketSetBuffers(&s->socket, -1, size, logMgr);
}

/************************************************************************************
 * rtpSetTransmitBufferSize
 * description:  Changes the RTP session transmit buffer size.
 * input:  hRTP - Handle of RTP session.
 *         size - The new transmit buffer size.
 * output: none.
 * return value: return 0.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpSetTransmitBufferSize(
                IN HRTPSESSION  hRTP,
                IN int size)
{
    rtpSession* s = (rtpSession *)hRTP;

    /* This function sets the size of the read/write buffers of a socket*/
    return RvSocketSetBuffers(&s->socket, size, -1, logMgr);
}

/************************************************************************************
 * rtpSetTrasmitBufferSize
 * description:  Changes the RTP session transmit buffer size.
 * input:  hRTP - Handle of RTP session.
 *         size - The new transmit buffer size.
 * output: none.
 * return value: return 0.
 * comment     : obsolete function provided for compatibility with prev. version
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpSetTrasmitBufferSize(
                IN HRTPSESSION  hRTP,
                IN int size)
{
  return rtpSetTransmitBufferSize(hRTP, size);
}


/************************************************************************************
 * rtpGetAvailableBytes
 * description:  Gets the number of bytes available for reading in the RTP session.
 * input:  hRTP - Handle of RTP session.
 * output: none.
 * return value: If an error occurs, the function returns a negative value.
 *               If no error occurs, the function returns a non-negative value.
 ***********************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpGetAvailableBytes(
                IN HRTPSESSION  hRTP)
{
    rtpSession *s = (rtpSession *)hRTP;
    RvSize_t bytes;
    RvStatus ret;

    /* This function returns the number of bytes in the specified socket that are
       available for reading.*/

    ret = RvSocketGetBytesAvailable(&s->socket, logMgr, &bytes);

    if (ret != RV_OK)
        return ret;

    return (RvInt32)bytes;
}

                  /* == ENDS: Accessory RTP Functions == */



                     /* == Internal RTP Functions == */

static void rtpEvent(
        IN RvSelectEngine*  selectEngine,
        IN RvSelectFd*      fd,
        IN RvSelectEvents   selectEvent,
        IN RvBool           error)
{
    rtpSession* s;

    RV_UNUSED_ARG(selectEngine);
    RV_UNUSED_ARG(selectEvent);
    RV_UNUSED_ARG(error);

    s = RV_GET_STRUCT(rtpSession, selectFd, fd);

    if (s->eventHandler)
    {
        s->eventHandler((HRTPSESSION)s, s->context);
    }
}


static RvBool isMyIP(RvUint32 ip)
{
    RvUint32 i;

    for (i = 0; (i < RV_RTP_MAXIPS) && (rvRtpInstance.hostIPs[i]); i++)
    {
        if (ip == rvRtpInstance.hostIPs[i])
            return RV_TRUE;
    }

    return RV_FALSE;
}

                  /* == ENDS: Internal RTP Functions == */




#ifdef __cplusplus
}
#endif



