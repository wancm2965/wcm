/***********************************************************************
Copyright (c) 2003 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..
RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

/***********************************************************************
epRtp.c

RTP media handling module.

It contains the use of 2 different packages:
 1. rv32rtp - The regular package bundled with the H.323 Protocol Stack
 2. rvrtp - The new RTP Toolkit

 Definitions
 -----------
 USE_RTP                  - Compilation uses some kind of an RTP package
 RTP_TYPE                 - The type of the rtp package:
 - RTP_TYPE_NORMAL            - Regular package
 - RTP_TYPE_NEW               - New RTP Protocol Stack
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif
	
#include "lepRtp.h"
	
	
#ifdef USE_RTP
#include "mti.h"
#include "rtp.h"
#include "rtcp.h"
#endif

#include <stdio.h>

#include "lepUtils.h"
#include "lepHash.h"


#ifdef USE_RTP
#if (RTP_TYPE == RTP_TYPE_NEW)
#ifdef USE_FWNAT
#define __H323_NAT_FW__
#include "rvrtpnatfw.h"
#endif
#endif




/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/

#define EP_RTP_MAX_SESSIONS (50) /* Maximum number of concurrent sessions we support */
#define EP_RTP_KEEP_ALIVE_PAYLOAD_TYPE 97


typedef struct RtpSessionObj_tag RtpSessionObj;
typedef struct RtpObj_tag RtpObj;


/* Single media session object */
struct RtpSessionObj_tag
{
    RvInt32             id; /* Session's index */
    RtpObj              *rtp; /* Pointer to the used RTP object */
    RvBool              allocated; /* Indicates if this one is allocated and used or not */
#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtpSession        hRTP; /* RTP session instance to use */
#else
    HRTPSESSION         hRTP;       /* RTP session handle to use */
#endif

    EpRtpAction         action;     /* Action to take on the session (none, replay, etc.) */
    RvUint32            bytes;      /* Number of bytes received on the session (used to calculate
                                       the rate) */
    RvUint32            lastTime;   /* Last time rate was checked (used to calculate the rate) */
    int                 channels;   /* Number of channels on the session (we must have 2
                                       channels to allow any action on the session). */
    RvBool              hasRemoteRTP;  /* RV_TRUE if we have the remote rtp address for this session */
    RvBool              hasRemoteRTCP; /* RV_TRUE if we have the remote rtcp address for this session */

    /* The following deals with different payload types. It is not the best
       way of doing things - only the fastest one for a working sample code. */
    epRtpPayloadEv      payloadCB[128];        /* Callback function for each payload type */
    void                *payloadContext[128];   /* Context to use for each payload function */
};


/* RTP object */
struct RtpObj_tag
{
    EpObj           *ep; /* Endpoint object used */
    RvUint32        curTime; /* Current start time */

    RvInt32         nextAlloc; /* Next vacant allocation to use */
    RtpSessionObj   session[EP_RTP_MAX_SESSIONS]; /* The actual sessions */
    RvInt32         numCurrentSessions;
#ifdef USE_FWNAT
#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtpDemux      hDemux;
#endif
#endif
};




/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/


/* Ugly, but works with a single thread. Some callbacks in RTCP don't allow us
   anything easier than this. */
static EpObj *global_ep = NULL;




/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/

static void CALLCONVC rtp_eventHandler(
#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtpSession     hRTP,
#else
    HRTPSESSION      hRTP,
#endif
    IN void         *context);

#if (RTP_TYPE == RTP_TYPE_NEW)
#ifdef USE_FWNAT
static void CALLCONVC muxedRtp_eventHandler(
    IN RvRtpDemux   hDemux,
    IN void         *context);
#endif
#endif

static void RVCALLCONV rtcpRecvCallback(
#if (RTP_TYPE == RTP_TYPE_NEW)
    IN RvRtcpSession hRCTP,
#else
    IN HRTCPSESSION  hRCTP,
#endif
    IN void         *context,
    IN RvUint32      ssrc);

static RvBool RVCALLCONV rtp_displayParticipant(
#if (RTP_TYPE == RTP_TYPE_NEW)
    IN RvRtcpSession hRCTP,
#else
    IN HRTCPSESSION  hRCTP,
#endif
    IN  RvUint32     ssrc);

#if (RTP_TYPE == RTP_TYPE_NEW)
#ifdef USE_FWNAT

static RvStatus epRtpGetSessionCall(
     IN  EpObj *         ep,
     IN  RvInt32         session,
     OUT EpCallObj **    call,
     OUT int *           rtpSessionIndexOnCall);

static RvStatus epRtpCallFindRemoteMultiplexId(
     IN  EpCallObj *    call,
     IN  RvInt32        sessionIdx,
     OUT RvUint32 *     remoteMultiplexId,
     OUT int *          rtpSessioIndexOnCall);

static RvStatus epRtpNetAddress2TransportAddress(
        IN  RvNetAddress         netAddress,
        OUT cmTransportAddress * addr);

static void epRtpTransportAddress2NetAddress(
        IN  cmTransportAddress * addr,
        OUT RvNetAddress *       netAddr);

#endif
#endif


/*-----------------------------------------------------------------------*/
/*                           CALLBACK FUNCTIONS                          */
/*-----------------------------------------------------------------------*/






/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * epRtpInit
 * ----------------------------------------------------------------------------
 * General: Initialize the RTP module for use.
 *
 * Return Value: RTP object created, NULL on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep                   - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RtpHANDLE epRtpInit(
    IN EpObj        *ep)
{
    RtpObj *rtp;
#if (RTP_TYPE == RTP_TYPE_NEW)
#ifdef USE_FWNAT
    RvRtpDemux hDemux;
#endif /* USE_FWNAT */

    /* Initialize RTP and RTCP packages (new package) */
    if (RvRtpInit() != RV_OK)
    {
        epUtilsLog(ep, NULL, "!!! RTP initialization failed !!!");
        return NULL;
    }
    if (RvRtcpInit() != RV_OK)
    {
        RvRtpEnd();
        epUtilsLog(ep, NULL, "!!! RCTP initialization failed !!!");
        return NULL;
    }

#ifdef USE_FWNAT
    hDemux = RvRtpDemuxConstruct(EP_RTP_MAX_SESSIONS);
    if (hDemux == NULL)
    {
        RvRtcpEnd();
        RvRtpEnd();
        epUtilsLog(ep, NULL, "!!! Could not construct DeMux !!!");
        return NULL;
    }
#endif /* USE_FWNAT */

    rtp = (RtpObj *)ep->cb.epMalloc(ep, sizeof(*rtp), __FILE__, __LINE__);
    if (rtp == NULL)
    {
#ifdef USE_FWNAT
        RvRtpDemuxDestruct(hDemux);
#endif /* USE_FWNAT */
        RvRtcpEnd();
        RvRtpEnd();
        epUtilsLog(ep, NULL, "!!! Unable to allocate RTP object !!!");
        return NULL;
    }

#else /* RTP_TYPE == RTP_TYPE_NEW */
    if (rtpInit() < 0)
    {
        epUtilsLog(ep, NULL, "!!! RTP initialization failed !!!");
        return NULL;
    }
    if (rtcpInit() < 0)
    {
        rtpEnd();
        epUtilsLog(ep, NULL, "!!! RTCP initialization failed !!!");
        return NULL;
    }

    rtp = (RtpObj *)ep->cb.epMalloc(ep, sizeof(*rtp), __FILE__, __LINE__);
    if (rtp == NULL)
    {
        rtcpEnd();
        rtpEnd();
        epUtilsLog(ep, NULL, "!!! Unable to allocate RTP object !!!");
        return NULL;
    }
#endif /* RTP_TYPE == RTP_TYPE_NEW */

    memset(rtp, 0, sizeof(*rtp));
    rtp->ep = ep;
    rtp->curTime = timerGetTimeInSeconds();
#ifdef USE_FWNAT
#if (RTP_TYPE == RTP_TYPE_NEW)
    rtp->hDemux = hDemux;
#endif
#endif
    return (RtpHANDLE)rtp;
}


/******************************************************************************
 * epRtpEnd
 * ----------------------------------------------------------------------------
 * General: Stop using the RTP module.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH         - Handle to RTP object.
 * Output: None.
 *****************************************************************************/
void epRtpEnd(
    IN RtpHANDLE    rtpH)
{
    RtpObj *rtp = (RtpObj *)rtpH;
    if (rtp == NULL) return;

#if (RTP_TYPE == RTP_TYPE_NEW)
#ifdef USE_FWNAT
    RvRtpDemuxDestruct(rtp->hDemux);
#endif
    RvRtcpEnd();
    RvRtpEnd();
#else
    rtcpEnd();
    rtpEnd();
#endif

    rtp->ep->cb.epFree(rtp->ep, rtp);
}


/******************************************************************************
 * epRtpVersion
 * ----------------------------------------------------------------------------
 * General: Get the version string of the RTP used.
 *
 * Return Value: Version string.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH         - Handle to RTP object.
 * Output: None.
 *****************************************************************************/
const RvChar *epRtpVersion(
    IN RtpHANDLE    rtpH)
{
    static RvChar buf[200];

    if (rtpH == NULL)
        return "0";

#if (RTP_TYPE == RTP_TYPE_NEW)
    sprintf(buf, "RTP/RTCP Stack v%s", RvRtpGetVersion());
#else
    sprintf(buf, "Internal RTP v%s", rtpGetVersion());
#endif

    return buf;
}


/******************************************************************************
 * epRtpDisplayInfo
 * ----------------------------------------------------------------------------
 * General: Display RTP information on currently active sessions.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH         - Handle to RTP object.
 * Output: None.
 *****************************************************************************/
void epRtpDisplayInfo(
    IN RtpHANDLE    rtpH)
{
    RtpObj *rtp = (RtpObj *)rtpH;
    int i;

#if (RTP_TYPE == RTP_TYPE_NEW)
    /* Loop through all of the sessions */
    for (i = 0; i < EP_RTP_MAX_SESSIONS; i++)
    {
        if ((rtp->session[i].allocated) && (rtp->session[i].hRTP != NULL))
        {
            /* Found an active session - get the info */
            RvRtcpEnumParticipants(RvRtpGetRTCPSession(rtp->session[i].hRTP), rtp_displayParticipant);
        }
    }
#else
    global_ep = rtp->ep;

    /* Loop through all of the sessions */
    for (i = 0; i < EP_RTP_MAX_SESSIONS; i++)
    {
        if ((rtp->session[i].allocated) && (rtp->session[i].hRTP != NULL))
        {
            /* Found an active session - get the info */
            rtcpEnumParticipants(rtpGetRTCPSession(rtp->session[i].hRTP), rtp_displayParticipant);
        }
    }

    global_ep = NULL;
#endif
}


/******************************************************************************
 * epRtpSessionNew
 * ----------------------------------------------------------------------------
 * General: Create a new RTP session.
 *
 * Return Value: Session number allocated, or a negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 *         isMultiplexed    - Is this session multiplexed?
 * Output: muxId            - multiplexID of session.
 *****************************************************************************/
RvInt32 epRtpSessionNew(
    IN  EpCallObj *  call,
    IN  RvBool       isMultiplexed,
    OUT RvUint32 *   muxId)
{
    EpObj *     ep = call->ep;
    RtpObj      *rtp = (RtpObj *)call->ep->rtp;
    RvBool      isSecured = call->secureMedia;
    RvBool      loop = RV_FALSE;
    RvInt32     s = 0;
    int         rtpPort;
    int         numberOfTries = 10;

#ifdef USE_FWNAT
    void * sessionContext;
#endif
#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtpSession hRTP = NULL, althRTP = NULL;
#else
    HRTPSESSION hRTP = NULL;
    HRTPSESSION althRTP = NULL;
#endif

    RV_UNUSED_ARG(isSecured);
    RV_UNUSED_ARG(muxId);
    RV_UNUSED_ARG(isMultiplexed);
    RV_UNUSED_ARG(ep);

    /* Find an empty session to allocate */
    while (rtp->session[rtp->nextAlloc].allocated)
    {
        rtp->nextAlloc++;
        if (rtp->nextAlloc == EP_RTP_MAX_SESSIONS)
        {
            rtp->nextAlloc = 0;
            if (loop)
                return -1; /* Couldn't find an empty session... */
            loop = RV_TRUE;
        }
    }

    memset(&rtp->session[rtp->nextAlloc], 0, sizeof(rtp->session[rtp->nextAlloc]));
    s = rtp->nextAlloc;
    rtp->session[rtp->nextAlloc].id = rtp->nextAlloc;
    rtp->session[rtp->nextAlloc].rtp = rtp;
    rtp->session[rtp->nextAlloc].allocated = RV_TRUE;
#ifdef USE_FWNAT
    sessionContext = (void *)&rtp->session[rtp->nextAlloc];
#endif
    rtp->nextAlloc = (rtp->nextAlloc + 1) % EP_RTP_MAX_SESSIONS;

    /* Add a channel to the session */
    epRtpSessionRate(call->ep->rtp, s);

    /* Create an RTP handle for the session */
    rtpPort = 0;
    do
    {
        /* We make sure we have an even number for the opened port.
           This way of doing it isn't encouraged */
        numberOfTries--;

        /* rtpPort not equal 0 means that we have already opened RTP with
           "bad" odd port number. rtpPort+1 is assigned to rtcpPort which is also wrong.
            Trying rtpPort+2 again give us wrong ports.
            So we have to try rtpPort+3 for RTP stream */

        if (rtpPort)
           rtpPort += 3;

#if (RTP_TYPE == RTP_TYPE_NEW)
        {
            RvNetAddress        addr;
            cmTransportAddress  sourceAddr;
            RvInt32             addrLen = (RvInt32)sizeof(cmTransportAddress);

            /*get the ip address of the call*/
            memset(&sourceAddr, 0, sizeof(sourceAddr));
            cmCallGetParam(call->hsCall, cmParamSourceIpAddress, 0, &addrLen, (RvChar *)&sourceAddr);
            if (!epTaHasIp(ep, &sourceAddr))
                cmGetLocalCallSignalAddress(call->ep->hApp, &sourceAddr);
            sourceAddr.port = (RvUint16)rtpPort;
            epRtpTransportAddress2NetAddress(&sourceAddr, &addr);
#ifdef USE_FWNAT
            if ((isMultiplexed) && (rtp->hDemux != NULL))
            {
                hRTP = RvRtpDemuxOpenSession(rtp->hDemux, &addr, 1, 0xff, (char*)"EP_RTP_MUX", sessionContext, muxId);
            }
            else
#endif
            {
                hRTP = RvRtpOpenEx(&addr, 1, 0xff, (char*)"EP_RTP");
            }
        }
#else
        hRTP = rtpOpenEx((RvUint16)rtpPort, 1, 0xff, (char*)"EP_RTP");
#endif

        /* Close a previous RTP object if we have one allocated */
        if (althRTP != NULL)
        {
#if (RTP_TYPE == RTP_TYPE_NEW)

#ifdef USE_FWNAT
            if ((isMultiplexed) && (rtp->hDemux != NULL))
            {
                RvRtpDemuxCloseSession(althRTP);
            }
            else
#endif
            {
                RvRtpClose(althRTP);
            }
#else
            rtpClose(althRTP);
#endif
        }
        /* save this handle */
        althRTP = hRTP;

        /* See if we succeeded in making an RTP object */
        if (hRTP != NULL)
        {
#if (RTP_TYPE == RTP_TYPE_NEW)
            rtpPort = RvRtpGetPort(hRTP);
#else
            rtpPort = rtpGetPort(hRTP);
#endif
        }
        else
        {
            /* Couldn't find a good port - make sure we try the next one */
            if ((rtpPort % 2) == 0)
                rtpPort++;
        }
    }
    while (((rtpPort % 2) != 0) && (numberOfTries > 0));

    if (hRTP == NULL)
    {
        /* Seems like we couldn't allocate an RTP session fast enough - let's just skip it */
        rtp->session[s].allocated = RV_FALSE;
        return -1;
    }

    rtp->session[s].action = epRtpActionNone;

    rtp->session[s].hRTP = hRTP;

#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtpUseSequenceNumber(rtp->session[s].hRTP);
    RvRtcpSetRTCPRecvEventHandler(RvRtpGetRTCPSession(rtp->session[s].hRTP), rtcpRecvCallback, &rtp->session[s]);
    rtp->session[s].channels = 1;
    /* Set the event handler for RTP */
#ifdef USE_FWNAT
    if ((isMultiplexed) && (rtp->hDemux != NULL))
        RvRtpDemuxSetEventHandler(rtp->hDemux, muxedRtp_eventHandler, (void*)call);
    else
#endif
        RvRtpSetEventHandler(rtp->session[s].hRTP, rtp_eventHandler, (void*)&rtp->session[s]);
#else
    rtpUseSequenceNumber(rtp->session[s].hRTP);

    rtcpSetRTCPRecvEventHandler(rtpGetRTCPSession(rtp->session[s].hRTP), rtcpRecvCallback, &rtp->session[s]);

    rtp->session[s].channels = 1;

    /* Set the event handler for RTP */
    rtpSetEventHandler(rtp->session[s].hRTP, rtp_eventHandler, &rtp->session[s]);
#endif

    rtp->numCurrentSessions++;

    /* Return the port used */
    return s;
}


/******************************************************************************
 * epRtpSessionJoin
 * ----------------------------------------------------------------------------
 * General: Join a new RTP channel to the same session. This is used to make
 *          sure that 2 uni-directional channels are connected to a single
 *          RTP session for loopback purposes.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH         - Handle to RTP object.
 *         channel      - Session to join.
 * Output: None.
 *****************************************************************************/
void epRtpSessionJoin(
    IN RtpHANDLE        rtpH,
    IN RvInt32          session)
{
    RtpObj *rtp = (RtpObj *)rtpH;

    if (session < 0) return;

    rtp->session[session].channels++;
}


/******************************************************************************
 * epRtpSessionClose
 * ----------------------------------------------------------------------------
 * General: Close an RTP session.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH             - Handle to RTP object.
 *         session          - Session to close.
 *         isMultiplexed    - RV_TRUE if session is multiplexed.
 * Output: None.
 *****************************************************************************/
void epRtpSessionClose(
    IN RtpHANDLE    rtpH,
    IN RvInt32      session,
    IN RvBool       isMultiplexed)
{
    RtpObj *rtp = (RtpObj *)rtpH;
    RV_UNUSED_ARG(isMultiplexed);

    if (session < 0) return;
    if (rtp->session[session].channels == 0) return;

    /* Remove a channel from the session */
    rtp->session[session].channels--;

    if (rtp->session[session].channels == 0)
    {
        rtp->numCurrentSessions--;
        /* Close the session */
        if (rtp->session[session].hRTP != NULL)
        {
#if (RTP_TYPE == RTP_TYPE_NEW)
            if (isMultiplexed)
            {
#ifdef USE_FWNAT
                {
                    if (rtp->numCurrentSessions == 0)
                    {
                        /*remove event handler from the demux if no sessions are left*/
                        RvRtpDemuxSetEventHandler(rtp->hDemux, NULL, NULL);
                    }
                    /*close the session*/
                    RvRtpDemuxCloseSession(rtp->session[session].hRTP);
                }
#endif
            }
            else
                RvRtpClose(rtp->session[session].hRTP);
#else
            rtpClose(rtp->session[session].hRTP);
#endif
            rtp->session[session].hRTP  = NULL;
        }
        rtp->session[session].allocated = RV_FALSE;
        rtp->session[session].rtp = NULL;
    }
}


/******************************************************************************
 * epRtpSessionGetLocalPort
 * ----------------------------------------------------------------------------
 * General: Return the local port used by the session.
 *
 * Return Value: The local port.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH         - Handle to RTP object.
 *         session      - Session to use.
 * Output: None.
 *****************************************************************************/
RvUint16 epRtpSessionGetLocalPort(
    IN RtpHANDLE    rtpH,
    IN RvInt32      session)
{
    RtpObj *rtp = (RtpObj *)rtpH;
    RvUint16 port = 0;

    if (session >= 0)
    {
#if (RTP_TYPE == RTP_TYPE_NEW)
        port = RvRtpGetPort(rtp->session[session].hRTP);
#else
        port = rtpGetPort(rtp->session[session].hRTP);
#endif
    }
    return port;
}



/******************************************************************************
 * epRtpSessionSetRemoteRTP
 * ----------------------------------------------------------------------------
 * General: Set the remote address for an RTP session.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 *         session          - Session to use.
 *         addr             - Remote address to set.
 *         isMultiplexed    - RV_TRUE if session is multiplexed.
 *         multiplexId      - The multiplexId Received - if session is multiplexed.
 * Output: None.
 *****************************************************************************/
void epRtpSessionSetRemoteRTP(
    IN EpCallObj *          call,
    IN RvInt32              session,
    IN cmTransportAddress   *addr,
    IN RvBool               isMultiplexed,
    IN RvUint32             multiplexId)
{
    RtpObj *rtp = (RtpObj *)call->ep->rtp;

    RV_UNUSED_ARG(isMultiplexed);
    RV_UNUSED_ARG(multiplexId);

    if (session < 0)
        return;

    rtp->session[session].hasRemoteRTP = RV_TRUE;
    if (rtp->session[session].hRTP == NULL)
        return;

#if (RTP_TYPE == RTP_TYPE_NEW)
    /* use the new RTP stack to set the address */
    {
        RvNetAddress netAddr;
        memset(&netAddr, 0, sizeof(netAddr));
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
        if (addr->type == cmTransportTypeIP)
        {
            RvNetIpv4 netV4;
            memset(&netV4, 0, sizeof(netV4));
            netV4.port = addr->port;
            netV4.ip = addr->addr.v4.ip;
            RvNetCreateIpv4(&netAddr, &netV4);
        }
        else if (addr->type == cmTransportTypeIPv6)
        {
            RvNetIpv6 netV6;
            memset(&netV6, 0, sizeof(netV6));
            netV6.port = addr->port;
            netV6.scopeId = addr->addr.v6.scopeId;
            memcpy(netV6.ip, addr->addr.v6.ip, 16);
            RvNetCreateIpv6(&netAddr, &netV6);
        }
#else
        {
            RvNetIpv4 netV4;
            memset(&netV4, 0, sizeof(netV4));
            netV4.port = addr->port;
            netV4.ip = addr->ip;
            RvNetCreateIpv4(&netAddr, &netV4);
        }
#endif
#ifdef USE_FWNAT
        if (isMultiplexed)
        {
            RvStatus status = RvRtpMultiplexingAddRemoteAddress(rtp->session[session].hRTP, &netAddr, multiplexId);
            epUtilsEvent(call->ep, "H460", call, "RvRtpMultiplexingAddRemoteAddress called for session %d", session);
            if (status != RV_OK)
                return;
        }
        else
#endif
        {
            RvRtpSetRemoteAddress(rtp->session[session].hRTP, &netAddr);
            epUtilsEvent(call->ep, "H460", call, "RvRtpSetRemoteAddress called for session %d", session);
        }
        if (addr->distribution == cmDistributionMulticast)
            RvRtpSetGroupAddress(rtp->session[session].hRTP, &netAddr);
#ifdef USE_FWNAT
        if (isMultiplexed)
            RvRtpDemuxSetEventHandler(rtp->hDemux, muxedRtp_eventHandler, (void*)call);
        else
#endif
        RvRtpSetEventHandler(rtp->session[session].hRTP, rtp_eventHandler, (void*)&rtp->session[session]);
    }
#else /* RTP_TYPE_NORMAL */
    {
        RvUint32 ip;
        /* the old RTP stack only supports IPv4 */
        if (addr->type != cmTransportTypeIP)
            return;

#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
        ip = addr->addr.v4.ip;
#else
        ip = addr->ip;
#endif
        rtpSetRemoteAddress(rtp->session[session].hRTP, ip, addr->port);
        if (addr->distribution == cmDistributionMulticast)
            rtpSetGroupAddress(rtp->session[session].hRTP, ip);

        rtpSetEventHandler(rtp->session[session].hRTP, rtp_eventHandler, (void*)&rtp->session[session]);
    }
#endif
}


/******************************************************************************
 * epRtpSessionSetRemoteRTCP
 * ----------------------------------------------------------------------------
 * General: Set the remote address for an RTCP session.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 *         session          - Session to use.
 *         addr             - Remote address to set.
 *         isMultiplexed    - RV_TRUE if session is multiplexed.
 *         multiplexId      - The multiplexId Received - if session is multiplexed.
 * Output: None.
 *****************************************************************************/
void epRtpSessionSetRemoteRTCP(
    IN EpCallObj *          call,
    IN RvInt32              session,
    IN cmTransportAddress   *addr,
    IN RvBool               isMultiplexed,
    IN RvUint32             multiplexId)
{
    RtpObj *rtp = (RtpObj *)call->ep->rtp;

    RV_UNUSED_ARG(isMultiplexed);
    RV_UNUSED_ARG(multiplexId);

    if (session < 0)
        return;

    rtp->session[session].hasRemoteRTCP = RV_TRUE;

    if (rtp->session[session].hRTP == NULL)
        return;

#if (RTP_TYPE == RTP_TYPE_NEW)
    /* use the new RTP stack to set the address */
    {
        RvNetAddress netAddr;
        RvRtcpSession hRTCP = RvRtpGetRTCPSession(rtp->session[session].hRTP);

        memset(&netAddr, 0, sizeof(netAddr));
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
        if (addr->type == cmTransportTypeIP)
        {
            RvNetIpv4 netV4;
            memset(&netV4, 0, sizeof(netV4));
            netV4.port = addr->port;
            netV4.ip = addr->addr.v4.ip;
            RvNetCreateIpv4(&netAddr, &netV4);
        }
        else if (addr->type == cmTransportTypeIPv6)
        {
            RvNetIpv6 netV6;
            memset(&netV6, 0, sizeof(netV6));
            netV6.port = addr->port;
            netV6.scopeId = addr->addr.v6.scopeId;
            memcpy(netV6.ip, addr->addr.v6.ip, 16);
            RvNetCreateIpv6(&netAddr, &netV6);
        }
#else
        {
            RvNetIpv4 netV4;
            memset(&netV4, 0, sizeof(netV4));
            netV4.port = addr->port;
            netV4.ip = addr->ip;
            RvNetCreateIpv4(&netAddr, &netV4);
        }
#endif
#ifdef USE_FWNAT
        if (isMultiplexed)
        {
            RvStatus status = RvRtcpMultiplexingAddRemoteAddress(hRTCP, &netAddr, multiplexId);
            if (status != RV_OK)
                return;
        }
        else
#endif
        RvRtcpSetRemoteAddress(hRTCP, &netAddr);
        if (addr->distribution == cmDistributionMulticast)
            RvRtcpSetGroupAddress(hRTCP, &netAddr);
    }
#else /* RTP_TYPE_NORMAL */
    /* the old RTP stack only supports IPv4 */
    if (addr->type == cmTransportTypeIP)
    {
        RvUint32 ip;
        HRTCPSESSION hRTCP = rtpGetRTCPSession(rtp->session[session].hRTP);

#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
        ip = addr->addr.v4.ip;
#else
        ip = addr->ip;
#endif
        rtcpSetRemoteAddress(hRTCP, ip, addr->port);
        if (addr->distribution == cmDistributionMulticast)
            rtcpSetGroupAddress(hRTCP, ip);
    }
#endif
}

/******************************************************************************
 * epRtpDemuxGetLocalRTP
 * ----------------------------------------------------------------------------
 * General: Retrieve the local rtp address of a session.
 *
 * Return Value: RV_OK on success, RV_ERROR values on failure .
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH         - Handle to RTP object.
 * Output: addr         - The local rtp address.
 *****************************************************************************/
RvStatus epRtpDemuxGetLocalRTP(
        IN  RtpHANDLE            rtpH,
        OUT cmTransportAddress *addr)
{
    RvStatus status = RV_OK;
#if (RTP_TYPE == RTP_TYPE_NEW)
#ifdef USE_FWNAT
    RtpObj *rtp = (RtpObj *)rtpH;
    RvNetAddress    netAddr;

    status = RvRtpDemuxGetLocalAddress(rtp->hDemux, &netAddr);
    if (status == RV_OK)
    {
        epRtpNetAddress2TransportAddress(netAddr, addr);
    }
#endif
#endif
    RV_UNUSED_ARG(addr);
    RV_UNUSED_ARG(rtpH);
    return status;
}

/******************************************************************************
 * epRtpDemuxGetLocalRTCP
 * ----------------------------------------------------------------------------
 * General: Retrieve the local rtcp address of a session.
 *
 * Return Value: RV_OK on success, RV_ERROR values on failure .
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH         - Handle to RTP object.
 * Output: addr         - The local rtcp address.
 *****************************************************************************/
RvStatus epRtpDemuxGetLocalRTCP(
        IN  RtpHANDLE            rtpH,
        OUT cmTransportAddress   *addr)
{
    RvStatus status = RV_OK;
#if (RTP_TYPE == RTP_TYPE_NEW)
#ifdef USE_FWNAT
    RtpObj *rtp = (RtpObj *)rtpH;
    RvNetAddress    netAddr;

    status = RvRtcpDemuxGetLocalAddress(rtp->hDemux, &netAddr);
    if (status == RV_OK)
    {
        epRtpNetAddress2TransportAddress(netAddr, addr);
    }
#endif
#endif
    RV_UNUSED_ARG(addr);
    RV_UNUSED_ARG(rtpH);
    return status;
}

/******************************************************************************
 * epRtpSessionSetAction
 * ----------------------------------------------------------------------------
 * General: Set the action for a given session: replay or none.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH         - Handle to RTP object.
 *         session      - Session to use.
 *         action       - Action to use.
 * Output: None.
 *****************************************************************************/
void epRtpSessionSetAction(
    IN RtpHANDLE            rtpH,
    IN RvInt32              session,
    IN EpRtpAction          action)
{
    RtpObj *rtp = (RtpObj *)rtpH;

    if (session < 0) return;

    rtp->session[session].action = action;
}



/******************************************************************************
 * epRtpSessionRate
 * ----------------------------------------------------------------------------
 * General: Calculate the rate of a session.
 *
 * Return Value: Rate of the session.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH         - Handle to RTP object.
 *         session      - Session to use.
 * Output: None.
 *****************************************************************************/
int epRtpSessionRate(
    IN RtpHANDLE            rtpH,
    IN RvInt32              session)
{
    RtpObj *rtp = (RtpObj *)rtpH;
    int rate, curtime;

    if (session < 0) return 0;

    curtime = (int)timerGetTimeInMilliseconds();

    /* Calculate the rate */
    rate = (rtp->session[session].bytes * 8) / (curtime - rtp->session[session].lastTime + 1);

    /* Set session information */
    rtp->session[session].lastTime = (RvUint32)curtime;
    rtp->session[session].bytes = 0;

    return rate;
}

#if (RTP_TYPE == RTP_TYPE_NEW)
/******************************************************************************
 * epRtpSessionHasRemoteRTPAddress
 * ----------------------------------------------------------------------------
 * General: Returns RV_TRUE if remote RTP address already set for this session.
 *
 * Return Value: RV_TRUE if remote address already set.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH         - Handle to RTP object.
 *         session      - Session to use.
 * Output: None.
 *****************************************************************************/
RvBool epRtpSessionHasRemoteRTPAddress(
     IN RtpHANDLE            rtpH,
     IN RvInt32              session)
{
    RtpObj *rtp = (RtpObj *)rtpH;

    return rtp->session[session].hasRemoteRTP;
}

/******************************************************************************
 * epRtpSessionHasRemoteRTCPAddress
 * ----------------------------------------------------------------------------
 * General: Returns RV_TRUE if remote RTCP address already set for this session.
 *
 * Return Value: RV_TRUE if remote address already set.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH         - Handle to RTP object.
 *         session      - Session to use.
 * Output: None.
 *****************************************************************************/
RvBool epRtpSessionHasRemoteRTCPAddress(
     IN RtpHANDLE            rtpH,
     IN RvInt32              session)
{
    RtpObj *rtp = (RtpObj *)rtpH;

    return rtp->session[session].hasRemoteRTCP;
}


#ifdef USE_FWNAT
/******************************************************************************
 * epRtpSendKeepAlive
 * ----------------------------------------------------------------------------
 * General: Handle an incoming RTP packet on a muxed session.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rtpH         - Handle to RTP object.
 *         session      - Session to use.
 * Output: RV_OK upon success, RV_ERROR_UNKNOWN upon failure.
 *****************************************************************************/
RvStatus epRtpSendKeepAlive(
        IN RtpHANDLE    rtpH,
        IN RvInt32      session)
{
    RtpObj *     rtp        = (RtpObj *)rtpH;
    RvRtpSession rtpSession = rtp->session[session].hRTP;
    RvRtpParam   rtpParam;

    if (session < 0)
        return RV_OK;

      RvRtpParamConstruct(&rtpParam);
      rtpParam.payload = EP_RTP_KEEP_ALIVE_PAYLOAD_TYPE;
      return RvRtpSendKeepAlivePacket(rtpSession, &rtpParam);
}
#endif
#endif

/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * rtp_eventHandler
 * ----------------------------------------------------------------------------
 * General: Handle an incoming RTP packet.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hRTP     - RTP session handle.
 *         context  - Context passed.
 * Output: None.
 *****************************************************************************/
static void CALLCONVC rtp_eventHandler(
#if (RTP_TYPE == RTP_TYPE_NEW)
    IN RvRtpSession hRTP,
#else
    IN HRTPSESSION  hRTP,
#endif
    IN void         *context)
{
    RtpSessionObj   *session = (RtpSessionObj *)context;
    static char     buff[8000];
    int             error = 0;
    RvUint8         payload = 0;
    int             sByte = 0;
    int             len = 0;
#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtpParam      p;
    RvNetAddress    remoteAddress;
#else
    rtpParam        p;
#endif

    if (session->rtp == NULL)
        return;

#if (RTP_TYPE == RTP_TYPE_NEW)
    memset(&remoteAddress, 0, sizeof(remoteAddress));
    error = RvRtpReadWithRemoteAddress(hRTP, buff, sizeof(buff), &p, &remoteAddress);
#else
    error = rtpReadEx(hRTP, buff, sizeof(buff), (timerGetTimeInSeconds() - session->rtp->curTime) * 8, &p);
#endif
    if (!error)
    {
        payload = p.payload;
        sByte = p.sByte;
        len = p.len;
    }

    if (error)
    {
        return;
    }
#if (RTP_TYPE == RTP_TYPE_NEW)
#ifdef USE_FWNAT
    /*let's see if we got keepAlive message in the server and need to set the remote address*/
    if (payload == EP_RTP_KEEP_ALIVE_PAYLOAD_TYPE)
    {
        EpObj *         ep = session->rtp->ep;
        EpCallObj *     call = NULL;
        RvUint32 remoteMultiplexId;
        RvInt32  sessionIdx = session->id;
        RvBool   remoteIsMultiplexed;
        cmTransportAddress remoteTransportAddress;
        int                rtpSessioIndexOnCall;

        /*find the call the session belongs to*/
        epRtpGetSessionCall(ep, sessionIdx, &call, &rtpSessioIndexOnCall);
        if ((call != NULL) &&
            (call->bUseH46019) &&
            (call->ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigServer) &&
            !session->hasRemoteRTP)
        {
            epRtpNetAddress2TransportAddress(remoteAddress, &remoteTransportAddress);
            /*save rtp address if we have to*/
            if ((rtpSessioIndexOnCall >= 0) &&
                (call->bMultiplexParamsRcvd[rtpSessioIndexOnCall] == RV_FALSE) &&
                (!epTaHasIp(ep, &call->rtpSessionsAddr[rtpSessioIndexOnCall])))
            {
                epUtilsLog(ep, call, "rtpSession %d remote address saved", sessionIdx);
                memcpy(&call->rtpSessionsAddr[rtpSessioIndexOnCall], &remoteTransportAddress, sizeof(cmTransportAddress));
            }
            else
            {
                /*find remote multiplexId*/
                epRtpCallFindRemoteMultiplexId(call, sessionIdx, &remoteMultiplexId, NULL);
                remoteIsMultiplexed = remoteMultiplexId != (RvUint32)-1;
                /*set remote rtp address*/
                epRtpSessionSetRemoteRTP(call, sessionIdx, &remoteTransportAddress, remoteIsMultiplexed, remoteMultiplexId);
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
                epUtilsLog(ep, call, "5.remote address %d:%d set on session %d, remoteIsMultiplexed=%d, muxId=%d",
                    remoteTransportAddress.addr.v4.ip, remoteTransportAddress.port, sessionIdx, remoteIsMultiplexed, remoteMultiplexId);
#endif
                /*set remote rtcp address*/
                if (!session->hasRemoteRTCP)
                {
                    remoteTransportAddress.port++;
                    epRtpSessionSetRemoteRTCP(call, sessionIdx, &remoteTransportAddress, remoteIsMultiplexed, remoteMultiplexId);
                }
            }
            return;
        }
    }
#endif
#endif

    /*do not replay keepAlive messages*/
    if (payload == EP_RTP_KEEP_ALIVE_PAYLOAD_TYPE)
        return;

    /* Let's see if we want to deal with this type of payload separately before we
       actually try to replay it back to the sender */
    if ((payload < 128) && (session->payloadCB[payload] != NULL))
    {
        session->payloadCB[payload](session->id, payload,
            session->payloadContext[payload], &p, (RvUint8*)buff, (RvUint32)len);
    }

    /* Loop back what we received... */
    if ((session->channels == 2) && (session->hasRemoteRTP))
    {
        /* Add the bytes we received to the session's information */
#if (RTP_TYPE == RTP_TYPE_NEW)
        session->bytes += len - sByte - (((RvRtpPayloadType)payload == RV_RTP_PAYLOAD_H261) ? 4 : 0);
#else
        session->bytes += len - sByte - (((payload_e)payload == H261) ? 4 : 0);
#endif

        /* Make sure our own sequence number is synchronized, but make it a little bit
           different than that of the other side. */
        p.sequenceNumber += 29000;

#if (RTP_TYPE == RTP_TYPE_NEW)
        error = RvRtpWrite(hRTP, buff, p.len, &p);
#else
        error = rtpWrite(hRTP, buff, p.len, &p);
#endif

        if (error < 0)
        {
            epUtilsError(session->rtp->ep, NULL, "rtp_eventHandler: rtpWrite returned error");
        }
    }
}

#if (RTP_TYPE == RTP_TYPE_NEW)
#ifdef USE_FWNAT
/******************************************************************************
 * muxedRtp_eventHandler
 * ----------------------------------------------------------------------------
 * General: Handle an incoming RTP packet on a muxed session.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hDemux     - Demux object handle.
 *         context    - Context passed.
 * Output: None.
 *****************************************************************************/
static void CALLCONVC muxedRtp_eventHandler(
    IN RvRtpDemux   hDemux,
    IN void         *context)
{
    EpCallObj *     call = (EpCallObj *)context;
    EpObj *         ep = call->ep;
    RtpSessionObj   *session;
    RvRtpSession    hRTP;
    static char     buff[8000];
    int             error = 0;
    RvUint8         payload = 0;
    int             sByte = 0;
    int             len = 0;
    RvRtpParam      p;
    RvNetAddress    remoteAddress;

    if (ep == NULL)
        return;

    memset(&remoteAddress, 0, sizeof(remoteAddress));

    error = RvRtpDemuxReadWithRemoteAddress(hDemux, buff, sizeof(buff), &hRTP, (void **)&session, &p, &remoteAddress);

    if (!error)
    {
        payload = p.payload;
        sByte = p.sByte;
        len = p.len;
    }
    else
    {
        return;
    }

    /*let's see if we got keepAlive message in the server and need to set the remote address*/
#ifdef USE_FWNAT
    if (call->bUseH46019 &&
        (ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigServer) &&
        (payload == EP_RTP_KEEP_ALIVE_PAYLOAD_TYPE) &&
        !session->hasRemoteRTP)
    {
        RvUint32           remoteMultiplexId , invalidMuxId = (RvUint32)-1;
        RvInt32            sessionId;
        RvBool             remoteIsMultiplexed;
        cmTransportAddress remoteTransportAddress;
        int                rtpSessioIndexOnCall;

        sessionId = session->id;
        /*find remote multiplexId*/
        epRtpCallFindRemoteMultiplexId(call, sessionId, &remoteMultiplexId, &rtpSessioIndexOnCall);
        epRtpNetAddress2TransportAddress(remoteAddress, &remoteTransportAddress);
        /*save rtp address if we have to*/
        if ((rtpSessioIndexOnCall >= 0) &&
            (call->bMultiplexParamsRcvd[rtpSessioIndexOnCall] == RV_FALSE) &&
            (!epTaHasIp(ep, &call->rtpSessionsAddr[rtpSessioIndexOnCall])))
        {
            epUtilsLog(ep, call, "rtpSession %d remote address saved", sessionId);
            memcpy(&call->rtpSessionsAddr[rtpSessioIndexOnCall], &remoteTransportAddress, sizeof(cmTransportAddress));
        }
        else
        {
            remoteIsMultiplexed = !(remoteMultiplexId == invalidMuxId);
            /*set session's remote address*/

            /*set remote rtp address*/
            epRtpSessionSetRemoteRTP(call, sessionId, &remoteTransportAddress, remoteIsMultiplexed, remoteMultiplexId);
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
            epUtilsLog(ep, call, "remote address %d:%d set on session %d, remoteIsMultiplexed=%d, muxId=%d",
                remoteTransportAddress.addr.v4.ip, remoteTransportAddress.port, sessionId, remoteIsMultiplexed, remoteMultiplexId);
#endif
            /*set remote rtcp address*/
            if (!session->hasRemoteRTCP)
            {
                remoteTransportAddress.port++;
                epRtpSessionSetRemoteRTCP(call, sessionId, &remoteTransportAddress, remoteIsMultiplexed, remoteMultiplexId);
            }
        }
        return;
    }
#endif

    /*do not replay keepAlive messages*/
    if (payload == EP_RTP_KEEP_ALIVE_PAYLOAD_TYPE)
        return;

    /* Let's see if we want to deal with this type of payload separately before we
       actually try to replay it back to the sender */
    if ((payload < 128) && (session->payloadCB[payload] != NULL))
    {
        session->payloadCB[payload](session->id, payload,
            session->payloadContext[payload], &p, (RvUint8*)buff, (RvUint32)len);
    }

    /* Loop back what we received... */
    if ((session->channels == 2) && (session->hasRemoteRTP))
    {
        /* Add the bytes we received to the session's information */

        session->bytes += len - sByte - (((RvRtpPayloadType)payload == RV_RTP_PAYLOAD_H261) ? 4 : 0);

        /* Make sure our own sequence number is synchronized, but make it a little bit
           different than that of the other side. */
        p.sequenceNumber += 29000;

        error = RvRtpWrite(hRTP, buff, p.len, &p);

        if (error < 0)
        {
            epUtilsError(session->rtp->ep, NULL, "muxedRtp_eventHandler: rtpWrite returned error");
        }
    }

}

#endif
#endif

/******************************************************************************
 * rtcpRecvCallback
 * ----------------------------------------------------------------------------
 * General: Incoming RTCP packet was received.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hRTCP    - RTCP session handle.
 *         context  - Context passed.
 *         ssrc     - SSRC of the sender.
 * Output: None.
 *****************************************************************************/
static void RVCALLCONV rtcpRecvCallback(
#if (RTP_TYPE == RTP_TYPE_NEW)
    IN RvRtcpSession hRTCP,
#else
    IN HRTCPSESSION  hRTCP,
#endif
    IN void         *context,
    IN RvUint32     ssrc)
{
    /* Got an RTCP packet. Here we can just check to see what we got...*/
    /*RTCPINFO info;

    rtcpGetSourceInfo(hRTCP, ssrc, &info);*/
    RV_UNUSED_ARG(hRTCP);
    RV_UNUSED_ARG(context);
    RV_UNUSED_ARG(ssrc);
}


/******************************************************************************
 * rtp_displayParticipant
 * ----------------------------------------------------------------------------
 * General: Display participant information for a given SSRC.
 *
 * Return Value: Always RV_FALSE.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hRTCP    - RTCP session handle.
 *         ssrc     - SSRC of the participant.
 * Output: None.
 *****************************************************************************/
static RvBool RVCALLCONV rtp_displayParticipant(
#if (RTP_TYPE == RTP_TYPE_NEW)
    IN RvRtcpSession hRTCP,
#else
    IN HRTCPSESSION  hRTCP,
#endif
    IN  RvUint32      ssrc)
{
#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtcpINFO info;

    if (global_ep == NULL)
        return RV_FALSE;

    if (RvRtcpGetSourceInfo(hRTCP, ssrc, &info) >= 0)
    {
        epUtilsEvent(global_ep, "logRtp", NULL, "SSRC=%x SEQ=%d LOST=%d CNAME=%s",
            ssrc, info.rrTo.sequenceNumber, info.rrTo.cumulativeLost, info.cname);
    }
#else
    RTCPINFO info;

    if (global_ep == NULL)
        return RV_FALSE;

    if (rtcpGetSourceInfo(hRTCP, ssrc, &info) >= 0)
    {
        epUtilsEvent(global_ep, "logRtp", NULL, "SSRC=%x SEQ=%d LOST=%d CNAME=%s",
            ssrc, info.rrTo.sequenceNumber, info.rrTo.cumulativeLost, info.cname);
    }
#endif
    return RV_FALSE;
}

#if (RTP_TYPE == RTP_TYPE_NEW)
#ifdef USE_FWNAT
/******************************************************************************
 * epRtpGetSessionCall
 * ----------------------------------------------------------------------------
 * General: Finds the call of a given session.
 *
 * Return Value: RV_OK if call found. RV_ERROR_UNKNOWN else.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep                       - endpoint object to use.
 *         session                  - Session id.
 * Output: call                     - Call object the session belongs to.
 *         rtpSessionIndexOnCall    -   The rtpSession index in the rtpSessions or muxedRtpSessions
 *                                      on the call (-1 if not there)
 *****************************************************************************/
static RvStatus epRtpGetSessionCall(
     IN  EpObj *         ep,
     IN  RvInt32         session,
     OUT EpCallObj **    call,
     OUT int *           rtpSessionIndexOnCall)
{
    void *key = NULL;
    *rtpSessionIndexOnCall = -1;

    do
    {
        lepLockEpForHash(ep, *call = (EpCallObj *)epHashGetAny(ep->calls, &key));
        if (*call != NULL)
        {
            EpChannelObj *  channel = (*call)->firstChannel;

            /*first let's look for the session in the call channels*/
            while(channel != NULL)
            {
                if (channel->rtpSession == session)
                    return RV_OK;
                channel = channel->nextChannel;
            }

            /*if session was not found in the channel let's try in the call*/
            if ((*call)->isFastConnectCall == RV_TRUE)
            {
                int i;
                for(i = 0; i < 3; ++i)
                {
                    if (((*call)->rtpSessions[i] == session) ||
                        ((*call)->muxedRtpSessions[i] == session))
                    {
                        *rtpSessionIndexOnCall = i;
                        return RV_OK;
                    }
                }
            }
        }
    } while (call != NULL);

    *call = NULL;
    return RV_ERROR_UNKNOWN;
}

/******************************************************************************
 * epRtpCallFindRemoteMultiplexId
 * ----------------------------------------------------------------------------
 * General: Find the remote multipleID of a given session.
 * Return Value: RV_OK upon success.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call               - Call object to use.
 *         sessionIdx         - The rtpSession of the channel/call.
 * Output: remoteMultiplexId  - The remote multipleID.
 *         rtpSessionIndexOnCall    -   The rtpSession index in the rtpSessions or muxedRtpSessions
 *                                      on the call (-1 if not there).
 *****************************************************************************/
static RvStatus epRtpCallFindRemoteMultiplexId(
     IN  EpCallObj *    call,
     IN  RvInt32        sessionIdx,
     OUT RvUint32 *     remoteMultiplexId,
     OUT int *          rtpSessioIndexOnCall)
{
    EpChannelObj *  channel;
    *remoteMultiplexId = (RvUint32)-1;

    if (rtpSessioIndexOnCall != NULL)
        *rtpSessioIndexOnCall = -1;

    /*first let's look for the session in the call channels*/
    channel = call->firstChannel;
    while(channel != NULL)
    {
        if (channel->rtpSession == sessionIdx)
        {
            *remoteMultiplexId = channel->remoteMultiplexID;
            return RV_OK;
        }
        channel = channel->nextChannel;
    }

    /*if session was not found in the channel let's try in the call*/
    if (call->isFastConnectCall == RV_TRUE)
    {
        int i;
        for(i = 0; i < 3; ++i)
        {
            if ((call->muxedRtpSessions[i]  == sessionIdx) ||
                (call->rtpSessions[i] == sessionIdx))
            {
                *remoteMultiplexId = call->remoteMultiplexID[i];
                if (rtpSessioIndexOnCall != NULL)
                    *rtpSessioIndexOnCall = i;
                return RV_OK;
            }
        }
    }

    /*session not found*/
    return RV_ERROR_UNKNOWN;

}

/******************************************************************************
 * cmRvNetAddress2TransportAddress
 * ----------------------------------------------------------------------------
 * General: Convert a RvNetAddress into a cmTransportAddress.
 * Return Value: RV_OK upon success.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  netAddress         - RvNetAddress to convert.
 * Output: addr               - Converted cmTransportAddress.
 *****************************************************************************/
static RvStatus epRtpNetAddress2TransportAddress(
        IN  RvNetAddress         netAddress,
        OUT cmTransportAddress * addr)
{
    RvNetAddressType addrType;

    memset((void *)addr, 0, sizeof(cmTransportAddress));

    /*get address type*/
    addrType = RvNetGetAddressType(&netAddress);
    if (addrType == RVNET_ADDRESS_NONE)
        return RV_ERROR_UNKNOWN;
    if (addrType == RVNET_ADDRESS_IPV4)
    {
        RvNetIpv4 IpV4;
        RvNetGetIpv4(&IpV4, &netAddress);
        addr->type = cmTransportTypeIP;
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
        addr->addr.v4.ip = IpV4.ip;
#else
        addr->ip = IpV4.ip;
#endif
        addr->port = IpV4.port;
    }
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    else
    {
        RvNetIpv6 IpV6;
        RvNetGetIpv6(&IpV6, &netAddress);
        addr->type = cmTransportTypeIPv6;
        memcpy(addr->addr.v6.ip, IpV6.ip, 16);
        addr->port = IpV6.port;
    }
#endif

    return RV_OK;
}


/******************************************************************************
 * epRtpTransportAddress2NetAddress
 * ----------------------------------------------------------------------------
 * General: Convert a cmTransportAddress into a RvNetAddress.
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  addr         - cmTransportAddress to convert.
 * Output: netAddress   - Converted RvNetAddress.
 *****************************************************************************/
static void epRtpTransportAddress2NetAddress(
        IN  cmTransportAddress * addr,
        OUT RvNetAddress *       netAddr)
{
#if (RTP_TYPE == RTP_TYPE_NEW)
    /* use the new RTP stack to set the address */
    memset(netAddr, 0, sizeof(RvNetAddress));
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    if (addr->type == cmTransportTypeIP)
    {
        RvNetIpv4 netV4;
        memset(&netV4, 0, sizeof(netV4));
        netV4.port = addr->port;
        netV4.ip = addr->addr.v4.ip;
        RvNetCreateIpv4(netAddr, &netV4);
    }
    else if (addr->type == cmTransportTypeIPv6)
    {
        RvNetIpv6 netV6;
        memset(&netV6, 0, sizeof(netV6));
        netV6.port = addr->port;
        netV6.scopeId = addr->addr.v6.scopeId;
        memcpy(netV6.ip, addr->addr.v6.ip, 16);
        RvNetCreateIpv6(netAddr, &netV6);
    }
#else
    {
        RvNetIpv4 netV4;
        memset(&netV4, 0, sizeof(netV4));
        netV4.port = addr->port;
        netV4.ip = addr->ip;
        RvNetCreateIpv4(netAddr, &netV4);
    }
#endif
#else
    /* the old RTP stack only supports IPv4 */
    if (addr->type != cmTransportTypeIP)
        return;
    else
    {
        RvUint32 ip;
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
        ip = addr->addr.v4.ip;
#else
        ip = addr->ip;
#endif
    }
#endif
}
#endif
#endif

#else

/* RTP not supported... */

#endif

#if defined(__cplusplus)
}
#endif

