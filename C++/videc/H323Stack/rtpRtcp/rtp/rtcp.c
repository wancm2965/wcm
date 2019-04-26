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
#include "rvlock.h"
#include "rvselect.h"
#include "rvsocket.h"
#include "rvmemory.h"
#include "rvclock.h"
#include "rvntptime.h"
#include "rvtime.h"
#include "rvtimer.h"
#include "rvhost.h"
#include "rvrtpbuffer.h"
#include "bitfield.h"
#include <string.h>
#include "rtputil.h"
#include "rtcp.h"

#ifdef __cplusplus
extern "C" {
#endif


#define MAXSDES                   255
#define MAXRTPSESSIONS            10
#define MAXRTPSESSIONMEMBERS      50
#define MAXRTCPPACKET             1470

#define MAXIPS                    20
#define MAX_DROPOUT               3000
#define MAX_MISORDER              100
#define MIN_SEQUENTIAL            2
#define RTP_SEQ_MOD               0x10000

#define ALIGNMENT                 0x10

/* RTCP header bit locations - see the standard */
#define HEADER_V                  30      /* version                       */
#define HEADER_P                  29      /* padding                       */
#define HEADER_RC                 24      /* reception report count        */
#define HEADER_PT                 16      /* packet type                   */
#define HEADER_len                0       /* packet length in 32-bit words */

/* RTCP header bit field lengths - see the standard */
#define HDR_LEN_V                 2       /* version                       */
#define HDR_LEN_P                 1       /* padding                       */
#define HDR_LEN_RC                5       /* reception report count        */
#define HDR_LEN_PT                8       /* packet type                   */
#define HDR_LEN_len               16      /* packet length in 32-bit words */


/* used to overcome byte-alignment issues */
#define SIZEOF_RTCPHEADER         (sizeof(RvUint32) * 2)
#define SIZEOF_SR                 (sizeof(RvUint32) * 5)
#define SIZEOF_RR                 (sizeof(RvUint32) * 6)

#define SIZEOF_SDES(sdes)         (((sdes).length + 6) & 0xfc)

/* initial bit field value for RTCP headers: V=2,P=0,RC=0,PT=0,len=0 */
#define RTCP_HEADER_INIT          0x80000000

typedef enum
{
   RTCP_SR   = 200,               /* sender report            */
   RTCP_RR   = 201,               /* receiver report          */
   RTCP_SDES = 202,               /* source description items */
   RTCP_BYE  = 203,               /* end of participation     */
   RTCP_APP  = 204,               /* application specific     */
   RTCP_LAST_LEGAL = RTCP_APP
} rtcpType;

typedef enum
{
   RTCP_SDES_END   = 0,
   RTCP_SDES_CNAME = 1,
   RTCP_SDES_NAME  = 2,
   RTCP_SDES_EMAIL = 3,
   RTCP_SDES_PHONE = 4,
   RTCP_SDES_LOC   = 5,
   RTCP_SDES_TOOL  = 6,
   RTCP_SDES_NOTE  = 7,
   RTCP_SDES_PRIV  = 8
} rtcpSDesType;

typedef struct
{
   RvUint64     tNNTP;
   RvUint32     tRTP;

   RvUint32     nPackets;
   RvUint32     nBytes;
} rtcpSR;

typedef struct
{
   RvUint32 ssrc;
   RvUint32 bfLost;      /* 8Bit fraction lost and 24 bit cumulative lost */
   RvUint32 nExtMaxSeq;
   RvUint32 nJitter;
   RvUint32 tLSR;
   RvUint32 tDLSR;
} rtcpRR;

typedef struct
{
   RvUint16 max_seq;               /* highest seq. number seen */
   RvUint32 cycles;                /* shifted count of seq. number cycles */
   RvUint32 base_seq;              /* base seq number */
   RvUint32 bad_seq;               /* last 'bad' seq number + 1 */
   RvUint32 probation;             /* sequ. packets till source is valid */
   RvUint32 received;              /* packets received */
   RvUint32 expected_prior;        /* packet expected at last interval */
   RvUint32 received_prior;        /* packet received at last interval */
   RvUint32 transit;               /* relative trans time for prev packet */
   RvUint32 jitter;                /* estimated jitter */
   /* ... */
} rtpSource;

typedef struct
{
   RvUint8  type;
   RvUint8  length;
   char     value[MAXSDES + 1];     /* leave a place for an asciiz */
} rtcpSDES;

typedef struct
{
   int        invalid;
   RvBool     active;
   rtpSource  src;

   RvUint32   ssrc;
   RvUint32   tLSRmyTime;
   rtcpSR     eSR;
   rtcpRR     eToRR;
   rtcpRR     eFromRR;
   rtcpSDES   eCName;
} rtcpInfo;

typedef struct
{
   RvUint32 bits;
   RvUint32 ssrc;
} rtcpHeader;


typedef struct
{
   RvBool    active;
   int       collision;
   RvUint32  ssrc;
   RvUint32  timestamp;
   rtcpSR    eSR;
   rtcpSDES  eCName;
} rtcpMyInfo;

typedef struct
{
    RvBool              isAllocated;
    RvSocket            socket;
    RvSelectEngine      *selectEngine; /* Select engine used by this fd */
    RvSelectFd          selectFd;
    rtcpMyInfo          myInfo;
    RvAddress           remoteAddress;
    RvBool              remoteAddressSet;
    RvTimer             timer; /* Timer of this RTCP session */
    RvBool              isTimerSet; /* RV_TRUE if we started a timer for this session */
    rtcpInfo*           participantsArray;
    int                 sessionMembers;
    int                 maxSessionMembers;
    LPRTCPEVENTHANDLER  rtcpRecvCallback;
    void                *haRtcp;
    RvLock              lock; /* Lock of this session. Used to protect the session members */
} rtcpSession;


#define logMgr NULL

#define RV_RTCP_MAXIPS RvUint32Const(20)



/* Internal RTCP instance used. There a single such object */
typedef struct
{
    RvSelectEngine*     selectEngine; /* select engine used for RTP/RTCP messages */

    RvUint32            localIp; /* IP to bind to */
    RvUint32            hostIPs[RV_RTCP_MAXIPS]; /* Local host IPs */
    RvUint32            timesInitialized; /* Times the RTP was initialized */

    RvTimerQueue*       timersQueue; /* Timers queue to use */

} RvRtcpInstance;



/* RTCP instance to use */
static RvRtcpInstance rvRtcpInstance;



#define reduceNNTP(a) (RvUint64ToRvUint32(RvUint64ShiftRight(a, 16)) & 0xffffffff)

#define W32Len(l)  ((l + 3) / 4)  /* length in 32-bit words */


/* local functions */
static RvUint64 getNNTPTime(void);
static RvBool rtcpTimerCallback(IN void* key);
static RvBool isMyIP(RvUint32 ip);
static void   setSDES(rtcpSDesType type, rtcpSDES* sdes, RvUint8 *data,
                      int length);
static void   init_seq  (rtpSource *s, RvUint16 seq);
static int    update_seq(rtpSource *s, RvUint16 seq, RvUint32 ts, RvUint32 arrival);

static RvUint32 getLost    (rtpSource *s);
static RvUint32 getJitter  (rtpSource *s);
static RvUint32 getSequence(rtpSource *s);
/*h.e 30.05.01*/
static RvUint32 getSSRCfrom(RvUint8 *);
/*===*/

static rtcpHeader makeHeader(RvUint32 ssrc, RvUint8 count, rtcpType type,
                             RvUint16 dataLen);

static rtcpInfo * findSSrc(rtcpSession *,RvUint32);

static rtcpInfo *insertNewSSRC(rtcpSession *s, RvUint32 ssrc);

static void rtcpEventCallback(
        IN RvSelectEngine*  selectEngine,
        IN RvSelectFd*      fd,
        IN RvSelectEvents   selectEvent,
        IN RvBool           error);



RVAPI
RvInt32 RVCALLCONV rtcpProcessCompoundRTCPPacket(
        IN      HRTCPSESSION  hRTCP,
        IN OUT  RvRtpBuffer*  buf,
        IN      RvUint64      myTime);


RVAPI
RvInt32 RVCALLCONV rtcpProcessRTCPPacket(
        IN  rtcpSession *  s,
        IN  RvUint8 *      data,
        IN  RvInt32        dataLen,
        IN  rtcpType       type,
        IN  RvInt32        reportCount,
        IN  RvUint64       myTime);




                      /* == Basic RTCP Functions == */


/*=========================================================================**
**  == rtcpInit() ==                                                       **
**                                                                         **
**  Initializes the RTCP module.                                           **
**                                                                         **
**  RETURNS:                                                               **
**      A non-negative value upon success, or a negative integer error     **
**      code.                                                              **
**                                                                         **
**=========================================================================*/

RVAPI
RvInt32 RVCALLCONV rtcpInit(void)
{
    RvStatus status;

    status = RvCBaseInit();
    if (status != RV_OK)
        return status;

    status = RvSelectConstruct(2048, 1024, logMgr, &rvRtcpInstance.selectEngine);
    if (status != RV_OK)
    {
        RvCBaseEnd();
        return ERR_RTCP_GENERALERROR;
    }

    /* Find the pool of timers to use */
    status = RvSelectGetTimeoutInfo(rvRtcpInstance.selectEngine, NULL, &rvRtcpInstance.timersQueue);
    if (status != RV_OK)
    {
        RvSelectDestruct(rvRtcpInstance.selectEngine, 1024);
        RvCBaseEnd();
        return ERR_RTCP_GENERALERROR;
    }

    if (rvRtcpInstance.timesInitialized == 0)
    {
        RvAddress addresses[RV_RTCP_MAXIPS];
        RvUint32 numAddrs = RV_RTCP_MAXIPS;
        RvUint32 i;

        rvRtcpInstance.localIp = 0;

        /* Find the list of host addresses we have */
        status = RvHostLocalGetAddress(logMgr, &numAddrs, addresses);
        if (status != RV_OK)
        {
            RvSelectDestruct(rvRtcpInstance.selectEngine, 1024);
            RvCBaseEnd();
            return ERR_RTCP_GENERALERROR;
        }

        for (i = 0; i < numAddrs; i++)
            rvRtcpInstance.hostIPs[i] = RvAddressIpv4GetIp(RvAddressGetIpv4(addresses + i));
    }

    rvRtcpInstance.timesInitialized++;

    return RV_OK;
}


/*=========================================================================**
**  == rtcpInitEx() ==                                                     **
**                                                                         **
**  Initializes the RTCP module.                                           **
**  Parameter: ip - ip address to 'bind' RTCP session                      **
**                                                                         **
**  RETURNS:                                                               **
**      A non-negative value upon success, or a negative integer error     **
**      code.                                                              **
**                                                                         **
**=========================================================================*/

RVAPI
RvInt32 RVCALLCONV rtcpInitEx(RvUint32 ip)
{
    RvInt32 rc;

    if ((rc=rtcpInit()) >= 0)
    {
        rvRtcpInstance.localIp = ip;
    }

    return rc;
}

/*=========================================================================**
**  == rtcpEnd() ==                                                        **
**                                                                         **
**  Shuts down the RTCP module.                                            **
**                                                                         **
**  RETURNS:                                                               **
**      A non-negative value upon success, or a negative integer error     **
**      code.                                                              **
**                                                                         **
**=========================================================================*/

RVAPI
RvInt32 RVCALLCONV rtcpEnd(void)
{
    RvStatus res;

    res = RvSelectDestruct(rvRtcpInstance.selectEngine, 1024);
    RvCBaseEnd();

    rvRtcpInstance.timesInitialized--;

    return res;
}


RVAPI
RvInt32 RVCALLCONV rtcpSetRTCPRecvEventHandler(
    IN HRTCPSESSION         hRTCP,
    IN LPRTCPEVENTHANDLER   rtcpCallback,
    IN void *               context)
{
    rtcpSession *s = (rtcpSession *)hRTCP;
    s->rtcpRecvCallback=rtcpCallback;
    s->haRtcp=context; /*context is Event to inform Ring3 about RTCP arrival*/
    return RV_OK;
}


/*=========================================================================**
**  == rtcpGetAllocationSize()                                             **
**                                                                         **
**  Calculates an allocation size for RTCP session                         **
**                                                                         **
**  PARAMETERS:                                                            **
**      sessionMembers  Maximum number of participants in the session      **
**                                                                         **
**                                                                         **
**  RETURNS:                                                               **
**      If no error occurs, the function returns an allocation size for    **
**      RTCP session.  Otherwise it returns NULL.                          **
**                                                                         **
**=========================================================================*/

RVAPI
int RVCALLCONV rtcpGetAllocationSize(
    IN  int sessionMembers)
{
    return sizeof(rtcpSession) + ALIGNMENT + sizeof(rtcpInfo) * sessionMembers;
}


/************************************************************************************
 * rtcpSetLocalAddress
 * description: Set the local address to use for calls to rtcpOpenXXX functions.
 *              This parameter overrides the value given in rtcpInitEx() for all
 *              subsequent calls.
 * input: ip    - Local IP address to use
 * output: none.
 * return value: Non-negative value on success
 *               Negative value on failure
 ***********************************************************************************/
RVAPI
int RVCALLCONV rtcpSetLocalAddress(IN RvUint32 ip)
{
    rvRtcpInstance.localIp = ip;
    return RV_OK;
}



/*=========================================================================**
**  == rtcpOpenFrom() ==                                                   **
**                                                                         **
**  Opens a new RTCP session in provided buffer.                           **
**                                                                         **
**  PARAMETERS:                                                            **
**      ssrc     The synchronization source value for the RTCP session.    **
**                                                                         **
**      port     The UDP port number to be used for the RTCP session.      **
**                                                                         **
**      cname    A unique name representing the source of the RTP data.    **
**               Must not be NULL.                                         **
**      maxSessionMembers   Maximum number of participants in the session  **
**                                                                         **
**  buffer   pointer to at least rtcpGetAllocationSize byte of memory      **
**                                                                         **
**  bufferSize size of the buffer                                          **
**                                                                         **
**  RETURNS:                                                               **
**      If no error occurs, the function returns a handle for the new      **
**      RTCP session.  Otherwise it returns NULL.                          **
**                                                                         **
**  COMMENTS:                                                              **
**              -----------------------------------------------            **
**     buffer  | rtcpSession |X| participantsArray |X| "mutex" |           **
**              -----------------------------------------------            **
**                            X - alignment area                           **
**=========================================================================*/

RVAPI
HRTCPSESSION RVCALLCONV rtcpOpenFrom(
        IN  RvUint32    ssrc,
        IN  RvUint16    port,
        IN  char *      cname,
        IN  int         maxSessionMembers,
        IN  void *      buffer,
        IN  int         bufferSize)
{
    rtcpSession* s=(rtcpSession*)buffer;
    RvSize_t allocSize=rtcpGetAllocationSize(maxSessionMembers);
    int participantsArrayOfs;
    RvAddress localAddress;
    RvStatus status;

    if ((cname  == NULL) || (strlen(cname) > MAXSDES) || ((int)allocSize > bufferSize))
    {
        return NULL;
    }

    memset(buffer, 0, allocSize);

    s->sessionMembers = 0;
    s->maxSessionMembers = maxSessionMembers;
    s->rtcpRecvCallback = NULL;
    s->isAllocated = RV_FALSE;

    participantsArrayOfs = RvRoundToSize(sizeof(rtcpSession), ALIGNMENT);
    s->participantsArray = (rtcpInfo *) ((char *)s + participantsArrayOfs);
    s->participantsArray[0].ssrc = s->myInfo.ssrc = ssrc;

    RvAddressConstructIpv4(&localAddress, rvRtcpInstance.localIp, port);

    /* Open and bind the socket */
    status = RvSocketConstruct(RV_ADDRESS_TYPE_IPV4, RvSocketProtocolUdp, logMgr, &s->socket);
    if (status == RV_OK)
    {
        status = RvSocketSetBuffers(&s->socket, 8192, 8192, logMgr);
        if (status == RV_OK)
            status = RvSocketSetBroadcast(&s->socket, RV_TRUE, logMgr);
        if (status == RV_OK)
            status = RvSocketSetBlocking(&s->socket, RV_TRUE, logMgr);
        if (status == RV_OK)
            status = RvSocketBind(&s->socket, &localAddress, NULL, logMgr);

        if (status != RV_OK)
            RvSocketDestruct(&s->socket, RV_FALSE, NULL, logMgr);
    }

    RvAddressDestruct(&localAddress);

    if (status != RV_OK)
        return NULL;

    /* Initialize lock in supplied buffer */
    if (RvLockConstruct(logMgr, &s->lock) != RV_OK)
    {
        RvSocketDestruct(&s->socket, RV_FALSE, NULL, logMgr);
        return NULL;
    }

    /* Make sure we wait for RTCP packets */
    RvFdConstruct(&s->selectFd, &s->socket, logMgr);

    status = RvSelectGetThreadEngine(logMgr, &s->selectEngine);
    if ((status != RV_OK) || (s->selectEngine == NULL))
        s->selectEngine = rvRtcpInstance.selectEngine;
    RvSelectAdd(s->selectEngine, &s->selectFd, RvSelectRead, rtcpEventCallback);

    setSDES(RTCP_SDES_CNAME, &(s->myInfo.eCName), (RvUint8*)cname, (int)strlen(cname));

    return (HRTCPSESSION)s;
}

/*=========================================================================**
**  == rtcpOpen() ==                                                       **
**                                                                         **
**  Opens a new RTCP session.                                              **
**                                                                         **
**  PARAMETERS:                                                            **
**      ssrc     The synchronization source value for the RTCP session.    **
**                                                                         **
**      port     The UDP port number to be used for the RTCP session.      **
**                                                                         **
**      cname    A unique name representing the source of the RTP data.    **
**               Must not be NULL.                                         **
**                                                                         **
**  RETURNS:                                                               **
**      If no error occurs, the function returns a handle for the new      **
**      RTCP session.  Otherwise it returns NULL.                          **
**                                                                         **
**=========================================================================*/

RVAPI
HRTCPSESSION RVCALLCONV rtcpOpen(
        IN  RvUint32    ssrc,
        IN  RvUint16    port,
        IN  char *      cname)
{
    rtcpSession* s;
    int allocSize = rtcpGetAllocationSize(MAXRTPSESSIONMEMBERS);

    if (RvMemoryAlloc(NULL, (RvSize_t)allocSize, logMgr, (void**)&s) != RV_OK)
        return NULL;

    if((rtcpSession*)rtcpOpenFrom(ssrc, port, cname, MAXRTPSESSIONMEMBERS, (void*)s, allocSize)==NULL)
    {
        RvMemoryFree(s, logMgr);
        return NULL;
    }

    s->isAllocated = RV_TRUE;
    return (HRTCPSESSION)s;
}


/*=========================================================================**
**  == rtcpClose() ==                                                      **
**                                                                         **
**  Closes an RTCP session.                                                **
**                                                                         **
**  PARAMETERS:                                                            **
**      hRTCP      The handle of the RTCP session.                         **
**                                                                         **
**  RETURNS:                                                               **
**      A non-negative value upon success, or a negative integer error     **
**      code.                                                              **
**                                                                         **
**=========================================================================*/

RVAPI
RvInt32 RVCALLCONV rtcpClose(
                IN  HRTCPSESSION  hRTCP)
{
    rtcpSession *s = (rtcpSession *)hRTCP;

    if (hRTCP == NULL)
        return RV_ERROR_UNKNOWN;

    /* Cancel the timer is one is active */
    if (s->isTimerSet)
        RvTimerCancel(&s->timer, RV_TIMER_CANCEL_WAIT_FOR_CB);

    RvSelectRemove(s->selectEngine, &s->selectFd);
    RvFdDestruct(&s->selectFd);

    /* Close the socket */
    RvSocketDestruct(&s->socket, RV_FALSE, NULL, logMgr);

    /* Destroy the lock */
    RvLockDestruct(&s->lock, logMgr);

    /* free memory allocated for rtcpSession */
    if (s->isAllocated)
        RvMemoryFree(s, logMgr);

    return RV_OK;
}


/*=========================================================================**
**  == rtcpSetRemoteAddress() ==                                           **
**                                                                         **
**  Defines the address of the remote peer or of the multicast group.      **
**                                                                         **
**  PARAMETERS:                                                            **
**      hRTCP    The handle of the RTCP session.                           **
**                                                                         **
**      ip       The IP address to which the RTCP packets will be sent.    **
**                                                                         **
**      port     The UDP port to which the RTCP packets should be sent.    **
**                                                                         **
**  RETURNS:                                                               **
**      A non-negative value upon success, or a negative integer error     **
**      code.                                                              **
**                                                                         **
**=========================================================================*/

RVAPI
void RVCALLCONV rtcpSetRemoteAddress(
                IN  HRTCPSESSION  hRTCP,     /* RTCP Session Opaque Handle */
                IN  RvUint32      ip,        /* target ip address */
                IN  RvUint16      port)      /* target UDP port */
{
    rtcpSession *s = (rtcpSession *)hRTCP;

    if ((ip != 0) && (port != 0))
    {
        if (s->remoteAddressSet)
        {
            RvAddressDestruct(&s->remoteAddress);
        }
        if (RvAddressConstructIpv4(&s->remoteAddress, ip, port) != NULL)
        {
            s->remoteAddressSet = RV_TRUE;

            if (s->isTimerSet == RV_FALSE)
            {
                RvTimerQueue *timerQ = NULL;
                RvStatus status;

                /* Find the timer queue we are working with */
                status = RvSelectGetTimeoutInfo(s->selectEngine, NULL, &timerQ);
                if (status != RV_OK)
                    timerQ = NULL;
                if (timerQ == NULL)
                    timerQ = rvRtcpInstance.timersQueue;

                /* Set a timer */
                /*todo: The timeout should be calculated */
                status = RvTimerStart(&s->timer, timerQ, RV_TIMER_TYPE_PERIODIC,
                    RvInt64Const(1, 1, 0x2a05f200) /* 5 seconds */, rtcpTimerCallback, s);
                if (status == RV_OK)
                    s->isTimerSet = RV_TRUE;
            }
        }
    }
    else
    {
        if (s->remoteAddressSet)
        {
            RvAddressDestruct(&s->remoteAddress);
            s->remoteAddressSet = RV_FALSE;
        }
        rtcpStop(hRTCP);
    }
}


/*=========================================================================**
**  == rtcpStop() ==                                                       **
**                                                                         **
**  Stop RTCP transmissions .                                               **
**                                                                         **
**  PARAMETERS:                                                            **
**      hRTCP    The handle of the RTCP session.                           **
**                                                                         **
**  RETURNS:                                                               **
**      A non-negative value upon success, or a negative integer error     **
**      code.                                                              **
**                                                                         **
**=========================================================================*/

RVAPI
RvInt32 RVCALLCONV rtcpStop(
                IN  HRTCPSESSION  hRTCP)     /* RTCP Session Opaque Handle */
{
    rtcpSession *s = (rtcpSession *)hRTCP;

    /* Cancel the timer if one is active */
    if (s->isTimerSet)
    {
        RvTimerCancel(&s->timer, RV_TIMER_CANCEL_WAIT_FOR_CB);
        s->isTimerSet = RV_FALSE;
    }

    if (s->remoteAddressSet)
    {
        RvAddressDestruct(&s->remoteAddress);
    }
    s->remoteAddressSet = RV_FALSE;

    /* Clear the list*/
    memset(s->participantsArray,0, (RvSize_t)(sizeof(rtcpInfo))*(s->sessionMembers));
    s->myInfo.collision = 0;
    s->myInfo.active = 0;
    s->myInfo.timestamp = 0;
    memset(&(s->myInfo.eSR),0,sizeof(s->myInfo.eSR));

    return RV_OK;
}

/*=========================================================================**
**  == rtcpRTPPacketRecv() ==                                              **
**                                                                         **
**  Informs the RTCP session that a packet was received in the             **
**  corresponding RTP session.                                             **
**                                                                         **
**  PARAMETERS:                                                            **
**      hRTCP      The handle of the RTCP session.                         **
**                                                                         **
**      localTimestamp  The local timestamp for the received packet.       **
**                                                                         **
**      timestamp  The RTP timestamp from the received packet.             **
**                                                                         **
**      sequence   The packet sequence number.                             **
**                                                                         **
**  RETURNS:                                                               **
**      A non-negative value upon success, or a negative integer error     **
**      code.                                                              **
**                                                                         **
**=========================================================================*/

RVAPI
RvInt32 RVCALLCONV rtcpRTPPacketRecv(
                IN  HRTCPSESSION  hRTCP,
                IN  RvUint32      ssrc,
                IN  RvUint32      localTimestamp,
                IN  RvUint32      myTimestamp,
                IN  RvUint16      sequence)

{
    rtcpSession *s = (rtcpSession *)hRTCP;
    rtcpInfo *fInfo;

    if (ssrc == s->myInfo.ssrc)
    {
        s->myInfo.collision = 1;
        return ERR_RTCP_SSRCCOLLISION;
    }

    /* See if we can find this source or not */
    fInfo = findSSrc(s,ssrc);

    /* If we didn't find this SSRC, we lock the RTCP database and search for the
       SSRC again. If we don't find it again - we insert it to the list, and
       finally we unlock... */
    if (!fInfo)  /* New source */
    {
        /* this section is working with threads.*/
        /* Lock the rtcp session.*/
        RvLockGet(&s->lock, logMgr);

        /* check if the ssrc is exist*/
        fInfo = findSSrc(s,ssrc);

        if (!fInfo)
        {
            /* Still no SSRC - we should add it to the list ourselves */
            fInfo = insertNewSSRC(s, ssrc);
        }

        /* unlock the rtcp session.*/
        RvLockRelease(&s->lock, logMgr);
    }


    if (fInfo != NULL)
    {
        if (!fInfo->invalid)
        {
            fInfo->active = RV_TRUE;
            update_seq(&(fInfo->src), sequence, localTimestamp, myTimestamp);
        }
    }

    return RV_OK;
}


/*=========================================================================**
**  == rtcpRTPPacketSent() ==                                              **
**                                                                         **
**  Informs the RTCP session that a packet was sent in the corresponding   **
**  RTP session.                                                           **
**                                                                         **
**  PARAMETERS:                                                            **
**      hRTCP      The handle of the RTCP session.                         **
**                                                                         **
**      bytes      The number of bytes in the sent packet.                 **
**                                                                         **
**      timestamp  The RTP timestamp from the received packet.             **
**                                                                         **
**  RETURNS:                                                               **
**      A non-negative value upon success, or a negative integer error     **
**      code.                                                              **
**                                                                         **
**=========================================================================*/

RVAPI
RvInt32 RVCALLCONV rtcpRTPPacketSent(
                IN  HRTCPSESSION  hRTCP,
                IN  RvInt32       bytes,
                IN  RvUint32      timestamp)
{
    rtcpSession *s = (rtcpSession *)hRTCP;

    s->myInfo.active = RV_TRUE;
    s->myInfo.eSR.nPackets++;
    s->myInfo.eSR.nBytes += bytes;
    s->myInfo.eSR.tNNTP = getNNTPTime();
    s->myInfo.eSR.tRTP = timestamp;

    if (s->myInfo.collision)
      return ERR_RTCP_SSRCCOLLISION;

    return RV_OK;
}


/*=========================================================================**
**  == rtcpGetPort() ==                                                    **
**                                                                         **
**  Gets the UDP port of an RTCP session.                                  **
**                                                                         **
**  PARAMETERS:                                                            **
**      hRTCP      The handle of the RTCP session.                         **
**                                                                         **
**  RETURNS:                                                               **
**      A non-negative value upon success, or a negative integer error     **
**      code.                                                              **
**                                                                         **
**=========================================================================*/

RVAPI
RvUint16 RVCALLCONV rtcpGetPort(
                IN  HRTCPSESSION  hRTCP)
{
    rtcpSession* s = (rtcpSession *)hRTCP;
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

                   /* == ENDS: Basic RTCP Functions == */



                    /* == Accessory RTCP Functions == */


/*=========================================================================**
**  == rtcpCheckSSRCCollision() ==                                         **
**                                                                         **
**  Checks for SSRC collisions in the RTCP session and the corresponding   **
**  RTP session.                                                           **
**                                                                         **
**  PARAMETERS:                                                            **
**      hRTCP      The handle of the RTCP session.                         **
**                                                                         **
**  RETURNS:                                                               **
**      RV_TRUE is returned if a collision was detected, otherwise         **
**      RV_FALSE.                                                          **
**                                                                         **
**=========================================================================*/

RVAPI
RvBool RVCALLCONV rtcpCheckSSRCCollision(
                IN  HRTCPSESSION  hRTCP)
{
    rtcpSession *s = (rtcpSession *)hRTCP;

    return (s->myInfo.collision != 0);
}


/*=========================================================================**
**  == rtcpEnumParticipants() ==                                           **
**                                                                         **
**  Provides information about in the RTCP session and the corresponding   **
**  RTP session.                                                           **
**                                                                         **
**  PARAMETERS:                                                            **
**      hRTCP      The handle of the RTCP session.                         **
**                                                                         **
**      enumerator A pointer to the function that will be called once per  **
**                 SSRC in the session.                                    **
**                                                                         **
**  RETURNS:                                                               **
**      If the enumeration process was stopped by the enumerator, the      **
**      function returns RV_FALSE, otherwise RV_TRUE.                      **
**                                                                         **
**  The prototype of the SSRC enumerator is as follows:                    **
**                                                                         **
**      RvBool                                                             **
**      SSRCENUM(                                                          **
**        IN  HRTPSESSION  hTRCP,                                          **
**        IN  RvUint32     ssrc                                            **
**      );                                                                 **
**                                                                         **
**  The parameters passed to the enumerator are as follows:                **
**      hRTCP      The handle of the RTCP session.                         **
**                                                                         **
**      ssrc       A synchronization source that participates in the       **
**                 session.                                                **
**                                                                         **
**  The enumerator should return RV_FALSE if it wants the enumeration      **
**  process to continue.  Returning RV_TRUE will cause                     **
**  rtcpEnumParticipant() to return immediately.                           **
**                                                                         **
**=========================================================================*/

RVAPI
RvBool RVCALLCONV rtcpEnumParticipants(
                IN HRTCPSESSION hRTCP,
                IN LPSSRCENUM   enumerator)
{
    RvInt32 elem, ssrc=0;

    elem = rtcpGetEnumFirst(hRTCP, &ssrc);

    while (elem >= 0)
    {
        if (enumerator(hRTCP, ssrc))
        {
            return RV_FALSE;
        }

        elem = rtcpGetEnumNext(hRTCP, elem, &ssrc);
    }

    return RV_TRUE;
}


/*=========================================================================**
**  == rtcpGetSourceInfo() ==                                              **
**                                                                         **
**  Provides information about a particular synchronization source.        **
**                                                                         **
**  TBD                                                                    **
**                                                                         **
**=========================================================================*/

RVAPI
RvInt32 RVCALLCONV rtcpGetSourceInfo(
                IN   HRTCPSESSION  hRTCP,
                IN   RvUint32      ssrc,
                OUT  RTCPINFO *    info)


{
    rtcpSession *s = (rtcpSession *)hRTCP;
    rtcpInfo *fInfo;

    if (ssrc == s->myInfo.ssrc)
    {
        info->selfNode          = RV_TRUE;
        info->sr.valid          = s->myInfo.active;
        info->sr.mNTPtimestamp  = RvUint64ToRvUint32(RvUint64ShiftRight(s->myInfo.eSR.tNNTP, 32));
        info->sr.lNTPtimestamp  = RvUint64ToRvUint32(RvUint64And(s->myInfo.eSR.tNNTP, RvUint64FromRvUint32(0xffffffff)));
        info->sr.timestamp      = s->myInfo.eSR.tRTP;
        info->sr.packets        = s->myInfo.eSR.nPackets;
        info->sr.octets         = s->myInfo.eSR.nBytes;

        /* It's our node - receiver reports are not valid */
        info->rrFrom.valid      = RV_FALSE;
        info->rrTo.valid        = RV_FALSE;

        strncpy(info->cname, s->myInfo.eCName.value, sizeof(info->cname));

        return RV_OK;
    }

    fInfo = findSSrc(s,ssrc);

    if (fInfo)
    {
        info->selfNode              = RV_FALSE;

        info->sr.valid              = !fInfo->invalid;
        info->sr.mNTPtimestamp      = RvUint64ToRvUint32(RvUint64ShiftRight(fInfo->eSR.tNNTP, 32));
        info->sr.lNTPtimestamp      = RvUint64ToRvUint32(RvUint64And(fInfo->eSR.tNNTP, RvUint64FromRvUint32(0xffffffff)));
        info->sr.timestamp          = fInfo->eSR.tRTP;
        info->sr.packets            = fInfo->eSR.nPackets;
        info->sr.octets             = fInfo->eSR.nBytes;

        info->rrFrom.valid          = RV_TRUE;
        info->rrFrom.fractionLost   = (fInfo->eFromRR.bfLost >> 24);
        info->rrFrom.cumulativeLost = (fInfo->eFromRR.bfLost & 0xffffff);
        info->rrFrom.sequenceNumber = fInfo->eFromRR.nExtMaxSeq;
        info->rrFrom.jitter         = fInfo->eFromRR.nJitter;
        info->rrFrom.lSR            = fInfo->eFromRR.tLSR;
        info->rrFrom.dlSR           = fInfo->eFromRR.tDLSR;

        info->rrTo.valid            = RV_TRUE;
        info->rrTo.fractionLost     = (fInfo->eToRR.bfLost >> 24);
        info->rrTo.cumulativeLost   = (fInfo->eToRR.bfLost & 0xffffff);
        info->rrTo.sequenceNumber   = fInfo->eToRR.nExtMaxSeq;
        info->rrTo.jitter           = fInfo->eToRR.nJitter;
        info->rrTo.lSR              = fInfo->eToRR.tLSR;
        info->rrTo.dlSR             = fInfo->eToRR.tDLSR;

        strncpy(info->cname, fInfo->eCName.value, sizeof(info->cname));
    }

    return (!fInfo) ? ERR_RTCP_ILLEGALSSRC : 0;
}


/*=========================================================================**
**  == rtcpSetGroupAddress() ==                                            **
**                                                                         **
**  Specifies a multicast IP for an RTCP session.                          **
**                                                                         **
**  PARAMETERS:                                                            **
**      hRTCP      The handle of the RTCP session.                         **
**                                                                         **
**      ip         The multicast IP address for the RTCP session.          **
**                 SSRC in the session.                                    **
**                                                                         **
**  RETURNS:                                                               **
**      If the enumeration process was stopped by the enumerator, the      **
**      function returns RV_FALSE, otherwise RV_TRUE.                      **
**                                                                         **
**=========================================================================*/

RVAPI
RvInt32 RVCALLCONV rtcpSetGroupAddress(
                IN  HRTCPSESSION  hRTCP,
                IN  RvUint32      ip)
{
    rtcpSession *s = (rtcpSession *)hRTCP;
    RvAddress addresses[2];
    RvStatus res;

    RvAddressConstructIpv4(&addresses[0], ip, RV_ADDRESS_IPV4_ANYPORT);
    RvAddressConstructIpv4(&addresses[1], rvRtcpInstance.localIp, RV_ADDRESS_IPV4_ANYPORT);

    /* This function adds the specified address (in network format) to the specified
       Multicast interface for the specified socket.*/
    res = RvSocketJoinMulticastGroup(&s->socket, addresses, addresses + 1, logMgr);

    RvAddressDestruct(&addresses[0]);
    RvAddressDestruct(&addresses[1]);

    return res;
}


/*=========================================================================**
**  == rtcpGetSSRC() ==                                                    **
**                                                                         **
**  Returns the synchronization source value for an RTCP session.          **
**                                                                         **
**  PARAMETERS:                                                            **
**      hRTCP      The handle of the RTCP session.                         **
**                                                                         **
**  RETURNS:                                                               **
**      The synchronization source value for the specified RTCP session.   **
**                                                                         **
**=========================================================================*/

RVAPI
RvInt32 RVCALLCONV rtcpGetSSRC(
                IN  HRTCPSESSION  hRTCP)
{
    rtcpSession *s = (rtcpSession *)hRTCP;

    return s->myInfo.ssrc;
}


/*=========================================================================**
**  == rtcpSetSSRC() ==                                                    **
**                                                                         **
**  Changes the synchronization source value for an RTCP session.          **
**                                                                         **
**  PARAMETERS:                                                            **
**      hRTCP      The handle of the RTCP session.                         **
**                                                                         **
**      ssrc       A synchronization srouce value for the RTCP session.    **
**                                                                         **
**  RETURNS:                                                               **
**      If the enumeration process was stopped by the enumerator, the      **
**      function returns RV_FALSE, otherwise RV_TRUE.                      **
**                                                                         **
**=========================================================================*/

RVAPI
RvInt32 RVCALLCONV rtcpSetSSRC(
                IN  HRTCPSESSION  hRTCP,
                IN  RvUint32      ssrc)
{
    rtcpSession *s = (rtcpSession *)hRTCP;

    s->myInfo.ssrc      = ssrc;
    s->myInfo.collision = 0;

    return RV_OK;
}

                 /* == ENDS: Accessory RTCP Functions == */



                     /* == Internal RTCP Functions == */


RVAPI
RvInt32 RVCALLCONV rtcpGetEnumFirst(
                IN  HRTCPSESSION  hRTCP,
                IN  RvInt32 *     ssrc)
{
    return rtcpGetEnumNext(hRTCP, -1, ssrc);
}

RVAPI
RvInt32 RVCALLCONV rtcpGetEnumNext(
                IN  HRTCPSESSION  hRTCP,
                IN  RvInt32       prev,
                IN  RvInt32 *     ssrc)
{
    rtcpSession *s = (rtcpSession *)hRTCP;
    rtcpInfo* info;
    RvInt32 index, doAgain = 1;

    if (prev < 0)
        index = 0;
    else
        index = prev+1;

    while ((doAgain == 1)&&(index < s->sessionMembers))
    {
        info = &s->participantsArray[index];
        if (!info->invalid)
        {
            doAgain = 0;
            *ssrc = info->ssrc;
        }
        else
        {
            index++;
        }
    }
    if (index < s->sessionMembers)
        return index;
    else
        return -1;
}

/* todo: check reference guide about this function */
RVAPI
RvInt32 RVCALLCONV rtcpCreateRTCPPacket(
                IN      HRTCPSESSION  hRTCP,
                IN OUT  RvRtpBuffer*  buf)
{
    rtcpSession *s = (rtcpSession *)hRTCP;
    rtcpHeader head;
    RvUint32 allocated = 0;
    RvRtpBuffer bufC;
    rtcpType type = RTCP_SR;
    int            index;

    if (buffValid(buf, SIZEOF_RTCPHEADER + SIZEOF_SR))
    {
        RvUint64 myTime = s->myInfo.eSR.tNNTP;
        RvUint8 cc = 0;
        rtcpInfo *info;

        allocated = SIZEOF_RTCPHEADER;

        if (s->myInfo.active)
        {
            s->myInfo.active = RV_FALSE;
            bufC = buffCreate(&(s->myInfo.eSR), SIZEOF_SR);
            buffAddToBuffer(buf, &bufC, allocated);
            ConvertToNetwork(buf->buffer + allocated, 0, W32Len(bufC.length));
            allocated += SIZEOF_SR;
        }
        else
        {
            type = RTCP_RR;
        }


        index = 0;

        while( index < s->sessionMembers)
        {
            info = &s->participantsArray[index];
            if (info->active)
            {
                info->eToRR.bfLost     = getLost    (&(info->src));
                info->eToRR.nJitter    = getJitter  (&(info->src));
                info->eToRR.nExtMaxSeq = getSequence(&(info->src));
                info->eToRR.tDLSR      =
                    (info->tLSRmyTime) ?
                    (reduceNNTP(myTime)-info->tLSRmyTime) :
                    0;

                bufC = buffCreate(&(info->eToRR), SIZEOF_RR);

                if (buffAddToBuffer(buf, &bufC, allocated))
                {
                    cc++;
                    if (cc == 32)
                        break;
                    ConvertToNetwork(buf->buffer + allocated, 0, W32Len(bufC.length));
                    allocated += SIZEOF_RR;
                }
                info->active = RV_FALSE;
            }

           index++;
        }

        head = makeHeader(s->myInfo.ssrc, cc, type, (RvUint16)allocated);
        bufC = buffCreate(&head, SIZEOF_RTCPHEADER);
        buffAddToBuffer(buf, &bufC, 0);

        /* add an CNAME SDES packet to the compound packet */
        if (buffValid(buf,
            allocated + SIZEOF_RTCPHEADER + SIZEOF_SDES(s->myInfo.eCName)))
        {
            RvRtpBuffer sdes_buf;

            /* 'sdes_buf' is inside the compound buffer 'buf' */
            sdes_buf = buffCreate(buf->buffer + allocated,
                (SIZEOF_RTCPHEADER + SIZEOF_SDES(s->myInfo.eCName)));

            head = makeHeader(s->myInfo.ssrc, 1, RTCP_SDES,
              (RvUint16)sdes_buf.length);

            memcpy(sdes_buf.buffer, (char *)&head, SIZEOF_RTCPHEADER);
            memcpy(sdes_buf.buffer + SIZEOF_RTCPHEADER, &(s->myInfo.eCName),
                   (RvSize_t)SIZEOF_SDES(s->myInfo.eCName));

            allocated += sdes_buf.length;
        }

        if (s->myInfo.collision == 1  &&
            buffValid(buf, allocated + SIZEOF_RTCPHEADER))
        {
            head = makeHeader(s->myInfo.ssrc, 1, RTCP_BYE,
                              SIZEOF_RTCPHEADER);

            bufC = buffCreate(&head, SIZEOF_RTCPHEADER);
            buffAddToBuffer(buf, &bufC, allocated);
            s->myInfo.collision = 2;
            allocated += SIZEOF_RTCPHEADER;
        }
    }

    buf->length = allocated;

    return RV_OK;
}


/* todo: check reference guide about this function */
RVAPI
RvInt32 RVCALLCONV rtcpProcessCompoundRTCPPacket(
        IN      HRTCPSESSION  hRTCP,
        IN OUT  RvRtpBuffer*  buf,
        IN      RvUint64      myTime)
{
    rtcpSession *s = (rtcpSession *)hRTCP;
    rtcpHeader *head;
    RvUint8 *currPtr, *dataPtr, *compoundEnd;
    RvInt32 hdr_count, hdr_len;
    rtcpType hdr_type;
    RvBool   hdr_paddingbit = RV_FALSE;
    RvUint32 Padding = 0;

    currPtr = buf->buffer;
    compoundEnd = buf->buffer + buf->length;

    while (currPtr < compoundEnd)
    {
        head = (rtcpHeader*)(currPtr);
        ConvertFromNetwork(currPtr, 0, 1);

        hdr_count = bitfieldGet(head->bits, HEADER_RC, HDR_LEN_RC);
        hdr_type  = (rtcpType)bitfieldGet(head->bits, HEADER_PT, HDR_LEN_PT);
        hdr_paddingbit = bitfieldGet(head->bits, HEADER_P, 1);
        if (hdr_paddingbit)
        {
           Padding = *(compoundEnd-1);
           compoundEnd = buf->buffer + buf->length - Padding;
        }
        hdr_len   = sizeof(RvUint32) *
                    (bitfieldGet(head->bits, HEADER_len, HDR_LEN_len)) - Padding;

        if ((compoundEnd - currPtr) < hdr_len)
        {
            return ERR_RTCP_ILLEGALPACKET;
        }
        if ((hdr_type > RTCP_LAST_LEGAL && hdr_type < RTCP_SR) || (hdr_len>1400 && hdr_len<0))
        {
            return ERR_RTCP_ILLEGALPACKET;
        }
        dataPtr = (RvUint8 *)head + sizeof(RvUint32);

        rtcpProcessRTCPPacket(s, dataPtr, hdr_len, hdr_type, hdr_count, myTime);

        currPtr += (hdr_len + sizeof(RvUint32));
    }

    return RV_OK;
}



RVAPI
RvInt32 RVCALLCONV rtcpProcessRTCPPacket(
        IN  rtcpSession *  s,
        IN  RvUint8 *      data,
        IN  RvInt32        dataLen,
        IN  rtcpType       type,
        IN  RvInt32        reportCount,
        IN  RvUint64       myTime)
{
    unsigned scanned = 0;
    rtcpInfo info, *fInfo=NULL;
    info.ssrc = 0;

    if (dataLen == 0)
        return RV_OK;

    switch (type)
    {
        case RTCP_SR:
        case RTCP_RR:
        {
            ConvertFromNetwork(data, 0, 1);
            info.ssrc = *(RvUint32 *)(data);
            scanned = sizeof(RvUint32);

            if (info.ssrc == s->myInfo.ssrc)
            {
                s->myInfo.collision = 1;

                return ERR_RTCP_SSRCCOLLISION;
            }

            fInfo = findSSrc(s,info.ssrc);

            if (!fInfo) /* New source */
            {
                /* insert the new source */
                RvLockGet(&s->lock, logMgr);
                fInfo = insertNewSSRC(s, *(RvUint32 *)data);
                RvLockRelease(&s->lock, logMgr);
            }
            break;
        }

        default:
            break;
    }

    /* process the information */
    switch (type)
    {
        case RTCP_SR:
        {
            ConvertFromNetwork(data + scanned, 0, W32Len(SIZEOF_SR));

            if (fInfo)
            {
                memcpy(&(fInfo->eSR.tNNTP), (data + scanned), 8);
                memcpy(&(fInfo->eSR.tRTP),  (data + scanned + 8), 4);
                memcpy(&(fInfo->eSR.nPackets),  (data + scanned + 12), 4);
                memcpy(&(fInfo->eSR.nBytes),  (data + scanned + 16), 4);

                fInfo->eToRR.tLSR = reduceNNTP(fInfo->eSR.tNNTP);
                fInfo->tLSRmyTime = reduceNNTP(myTime);
            }

            scanned += SIZEOF_SR;
        }

        /* fall into RR */

        case RTCP_RR:
        {
            if ((fInfo != NULL) && ((RvInt32)scanned < dataLen))
            {
                int i;
                rtcpRR* rr = (rtcpRR*)(data + scanned);

                ConvertFromNetwork(data + scanned, 0,
                    reportCount * W32Len(sizeof(rtcpRR)));

                for (i=0; i < reportCount; i++)
                {
                    if (rr[i].ssrc == s->myInfo.ssrc)
                    {
                        fInfo->eFromRR = rr[i];
                        break;
                    }
                }
            }

            break;
        }

        case RTCP_SDES:
        {
            int i;
            rtcpSDES *sdes;

            for (i = 0; (i < reportCount) && ((RvInt32)scanned < dataLen); i++)
            {
                ConvertFromNetwork(data + scanned, 0, 1);
                info.ssrc = *(RvUint32 *)(data + scanned);

                sdes = (rtcpSDES *)(data + scanned + sizeof(info.ssrc));

                fInfo = findSSrc(s,info.ssrc);
                if (fInfo != NULL)
                {
                    scanned += sizeof(RvUint32);
                    do
                    {
                        switch(sdes->type)
                        {
                        case RTCP_SDES_CNAME:
                            memcpy(&(fInfo->eCName), sdes, (RvSize_t)(sdes->length+2));
                            fInfo->eCName.value[sdes->length] = 0;
                        break;
/* known SDES types that are not handled:
                        case RTCP_SDES_END:
                        case RTCP_SDES_NAME:
                        case RTCP_SDES_EMAIL:
                        case RTCP_SDES_PHONE:
                        case RTCP_SDES_LOC:
                        case RTCP_SDES_TOOL:
                        case RTCP_SDES_NOTE:
                        case RTCP_SDES_PRIV:
                        break;
*/
                        }
                        scanned += (sdes->length+2);
                        sdes = (rtcpSDES *)(data + scanned);
                    }
                    while (*(RvUint8 *)(data + scanned) != 0 && ((RvInt32)scanned < dataLen));
                }
            }
            break;
        }

        case RTCP_BYE:
        {
            RvInt32 i;
            ConvertFromNetwork(data, 0, 1);
            info.ssrc = *(RvUint32 *)(data);
            scanned = sizeof(RvUint32);
            for (i = 0; i < reportCount; i++)
            {
                ConvertFromNetwork(data + scanned, 0, 1);
                info.ssrc = *(RvUint32 *)(data + scanned);
                scanned += sizeof(info.ssrc);

                fInfo = findSSrc(s,info.ssrc);
                if (fInfo != NULL)
                {
                    /* We don't really delete this SSRC, we just mark it as invalid */
                    fInfo->invalid = RV_TRUE;
                    fInfo->ssrc    = 0;
                }
            }

            break;
        }

        case RTCP_APP:
            break;
    }

    return RV_OK;
}



static RvBool rtcpTimerCallback(IN void* key)
{
    rtcpSession* s = (rtcpSession*)key;
    RvUint32 data[MAXRTCPPACKET/sizeof(RvUint32)+1];
    RvRtpBuffer buf;

    buf = buffCreate(data, MAXRTCPPACKET);

    rtcpCreateRTCPPacket((HRTCPSESSION)s, &buf);

    RvSocketSendBuffer(&s->socket, buf.buffer, (RvSize_t)buf.length, &s->remoteAddress, logMgr, NULL);

    return RV_TRUE; /* Make sure the timer is rescheduled */
}


static void rtcpEventCallback(
        IN RvSelectEngine*  selectEngine,
        IN RvSelectFd*      fd,
        IN RvSelectEvents   selectEvent,
        IN RvBool           error)
{
    rtcpSession* s;
    RvUint32 buffer[MAXRTCPPACKET/sizeof(RvUint32)+1];
    RvRtpBuffer buf;
    RvStatus res;
    RvAddress remoteAddress, localAddress;
    RvSize_t bytesReceived;

    RV_UNUSED_ARG(selectEngine);
    RV_UNUSED_ARG(selectEvent);
    RV_UNUSED_ARG(error);

    s = RV_GET_STRUCT(rtcpSession, selectFd, fd);

    buf = buffCreate(buffer, MAXRTCPPACKET);

    res = RvSocketReceiveBuffer(&s->socket, buf.buffer, (RvSize_t)buf.length, logMgr, &bytesReceived, &remoteAddress);
    if (res == RV_OK)
    {
        buf.length = (RvUint32)bytesReceived;

        RvSocketGetLocalAddress(&s->socket, logMgr, &localAddress);

        if ((RvAddressGetIpPort(&remoteAddress) != RvAddressGetIpPort(&localAddress)) ||
            !isMyIP(RvAddressIpv4GetIp(RvAddressGetIpv4(&remoteAddress))))
        {
            RvInt32 res=rtcpProcessCompoundRTCPPacket((HRTCPSESSION)s, &buf, getNNTPTime());
            if ((res==0) && (s->rtcpRecvCallback != NULL))
            {
               RvUint32 ssrc=getSSRCfrom(buf.buffer);
               s->rtcpRecvCallback((HRTCPSESSION)s, s->haRtcp, ssrc);
            }
        }
    }
}


static RvUint64 getNNTPTime(void)
{
    RvTime t;
    RvNtpTime ntptime;
    RvUint64 result;

    RvTimeConstruct(0, 0, &t); /* create and clear time structure */
    RvClockGet(NULL, &t); /* Get current wall-clock time */
    RvNtpTimeConstructFromTime(&t, RV_NTPTIME_ABSOLUTE, &ntptime); /* convert to NTP time */
    result = RvNtpTimeTo64(&ntptime, 32, 32); /* Convert to format getNNTPTime returns */
    RvNtpTimeDestruct(&ntptime); /* clean up */
    RvTimeDestruct(&t); /* clean up */

    return result;
}

static RvBool isMyIP(RvUint32 ip)
{
    int i;

    for (i=0; rvRtcpInstance.hostIPs[i]; i++)
    {
        if (ip == rvRtcpInstance.hostIPs[i])
        {
            return RV_TRUE;
        }
    }

    return RV_FALSE;
}

static void setSDES(rtcpSDesType type, rtcpSDES* sdes, RvUint8 *data, int length)
{
    sdes->type   = (unsigned char)type;
    sdes->length = (unsigned char)length;
    memcpy(sdes->value, data, (RvSize_t)length);
    memset(sdes->value+length, 0, (RvSize_t)( 4-((length+2)%sizeof(RvUint32)) ));
}

static void init_seq(rtpSource *s, RvUint16 seq)
{
   s->base_seq       = seq;
   s->max_seq        = seq;
   s->bad_seq        = RTP_SEQ_MOD + 1;
   s->cycles         = 0;
   s->received       = 0;
   s->received_prior = 0;
   s->expected_prior = 0;
}


static int update_seq(rtpSource *s, RvUint16 seq, RvUint32 ts, RvUint32 arrival)
{
    RvUint16 udelta = (RvUint16)(seq - s->max_seq);

    if (s->probation)
    {
        if (seq == s->max_seq + 1)
        {
            s->probation--;
            s->max_seq = seq;
            if (s->probation == 0)
            {
                init_seq(s, seq);
                s->received++;
                return 1;
            }
        }
        else
        {
            s->probation = MIN_SEQUENTIAL - 1;
            s->max_seq = seq;
        }
        return RV_OK;
    }
    else if (udelta < MAX_DROPOUT)
    {
        if (seq < s->max_seq) s->cycles += RTP_SEQ_MOD;
        s->max_seq = seq;
    }
    else if (udelta <= RTP_SEQ_MOD - MAX_MISORDER)
    {
        if (seq == s->bad_seq)
        {
            init_seq(s, seq);
        }
        else
        {
            s->bad_seq = (seq + 1) & (RTP_SEQ_MOD-1);
            return RV_OK;
        }
    }
    else
    {
   /* duplicate or reordered packet */
    }
    {
        RvInt32 transit = (RvInt32)(arrival - ts);
        RvInt32 d = (RvInt32)(transit - s->transit);
        s->transit = transit;
        if (d < 0) d = -d;
        s->jitter += d - ((s->jitter + 8) >> 4);
    }
    s->received++;
    return 1;
}


/*=========================================================================**
**  == makeHeader() ==                                                     **
**                                                                         **
**  Creates an RTCP packet header.                                         **
**                                                                         **
**  PARAMETERS:                                                            **
**      ssrc       A synchronization source value for the RTCP session.    **
**                                                                         **
**      count      A count of sender and receiver reports in the packet.   **
**                                                                         **
**      type       The RTCP packet type.                                   **
**                                                                         **
**      dataLen    The length of the data in the packet buffer, in         **
**                 octets, including the size of the header.               **
**                                                                         **
**  RETURNS:                                                               **
**      The function returns a header with the appropriate parameters.     **
**                                                                         **
**=========================================================================*/

static rtcpHeader makeHeader(RvUint32 ssrc, RvUint8 count, rtcpType type,
                             RvUint16 dataLen)
{
    rtcpHeader header;

    header.ssrc = ssrc;

    header.bits = RTCP_HEADER_INIT;
    header.bits = bitfieldSet(header.bits, count, HEADER_RC, HDR_LEN_RC);
    header.bits = bitfieldSet(header.bits, type,  HEADER_PT, HDR_LEN_PT);
    header.bits = bitfieldSet(header.bits, W32Len(dataLen) - 1, HEADER_len, HDR_LEN_len);

    ConvertToNetwork(&header, 0, W32Len(SIZEOF_RTCPHEADER));

    return header;
}


static RvUint32 getLost(rtpSource *s)
{
    RvUint32 extended_max;
    RvUint32 expected;
    RvInt32 received_interval;
    RvInt32 expected_interval;
    RvInt32 lost;
    RvInt32 lost_interval;
    RvUint8 fraction;

    extended_max = s->cycles + s->max_seq;
    expected = extended_max - s->base_seq + 1;
    lost = expected - s->received;
    expected_interval = expected - s->expected_prior;
    s->expected_prior = expected;
    received_interval = s->received - s->received_prior;
    s->received_prior = s->received;
    lost_interval = expected_interval - received_interval;

    if (expected_interval == 0  ||  lost_interval <= 0)
        fraction = 0;
    else
        fraction = (RvUint8)((lost_interval << 8) / expected_interval);

    return (fraction << 24) + lost;
}


static RvUint32 getJitter(rtpSource *s)
{
    return s->jitter >> 4;
}

static RvUint32 getSequence(rtpSource *s)
{
    return s->max_seq + s->cycles;
}



static RvUint32 getSSRCfrom(RvUint8 *head)
{
   RvUint8 *ssrcPtr = (RvUint8 *)head + sizeof(RvUint32);
   return *(RvUint32 *)(ssrcPtr);
}


static rtcpInfo *findSSrc(rtcpSession *s, RvUint32 ssrc)
{
    int     index = 0;
    RvBool  doAgain = RV_TRUE;
    rtcpInfo *pInfo;
    if (s == NULL)
        return NULL;

    /* Look for the given SSRC */
    while ((doAgain) && (index < s->sessionMembers))
    {
       if (s->participantsArray[index].ssrc == ssrc)
            doAgain = RV_FALSE;
       else
           index ++;

    }
    if (index < s->sessionMembers )
        pInfo = &s->participantsArray[index];
    else
        pInfo = NULL;

    return pInfo;
}


static rtcpInfo *insertNewSSRC(rtcpSession *s, RvUint32 ssrc)
{
    rtcpInfo* pInfo = NULL;
    int index;

    if (s->sessionMembers >= s->maxSessionMembers)
    {
        /* We've got too many - see if we can remove some old ones */
        index = 0;
        while ((index < s->sessionMembers) &&
               (s->participantsArray[index].invalid && s->participantsArray[index].ssrc == 0))
            index++;
    }
    else
    {
        /* Add it as a new one to the list */
        index = s->sessionMembers;
        s->sessionMembers++;
    }

    if (index < s->sessionMembers)
    {
        /* Got a place for it ! */
        pInfo = &s->participantsArray[index];
        memset(pInfo, 0, sizeof(rtcpInfo));
        pInfo->ssrc             = ssrc;
        pInfo->eToRR.ssrc       = ssrc;
        pInfo->active           = RV_FALSE;
        pInfo->src.probation    = MIN_SEQUENTIAL - 1;
    }

    return pInfo;
}



                  /* == ENDS: Internal RTCP Functions == */
#ifdef __cplusplus
}
#endif



