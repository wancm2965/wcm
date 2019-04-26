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

#ifndef _RV_EP_RTP_H_
#define _RV_EP_RTP_H_

/***********************************************************************
epRtp.h

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



/* Types of implementations we can use */
#define RTP_TYPE_NORMAL 0
#define RTP_TYPE_NEW 1

#ifdef USE_RTP
	
#if !defined(RTP_TYPE)
/* Default implementation is normal */
#define RTP_TYPE RTP_TYPE_NORMAL
#endif

#if ((RTP_TYPE != RTP_TYPE_NORMAL) && (RTP_TYPE != RTP_TYPE_NEW))
/*#error Bad setting of RTP_TYPE*/
#endif

#include "lepDefs.h"
#include "rtp.h"
#include "payload.h"



/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/





/*-----------------------------------------------------------------------*/
/*                           CALLBACK HEADERS                            */
/*-----------------------------------------------------------------------*/


typedef void (*epRtpPayloadEv)(
    IN int      session,
    IN RvUint8  payload,
    IN void*    context,
    IN void*    rtpHeader,
    IN RvUint8* buffer,
    IN RvUint32 bufferLength);





/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
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
    IN EpObj        *ep);


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
    IN RtpHANDLE    rtpH);


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
    IN RtpHANDLE    rtpH);


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
    IN RtpHANDLE    rtpH);


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
    OUT RvUint32 *   muxId);


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
 *         session      - Session to join.
 * Output: None.
 *****************************************************************************/
void epRtpSessionJoin(
    IN RtpHANDLE    rtpH,
    IN RvInt32      session);


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
    IN RvBool       isMultiplexed);


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
    IN RvInt32      session);



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
    IN RvUint32             multiplexId);


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
    IN RvUint32             multiplexId);

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
        OUT cmTransportAddress   *addr);

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
        OUT cmTransportAddress   *addr);

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
    IN EpRtpAction          action);


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
    IN RvInt32              session);

#if (RTP_TYPE == RTP_TYPE_NEW)
/******************************************************************************
 * epRtpSessionHasRemoteAddress
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
     IN RvInt32              session);

/******************************************************************************
 * epRtpSessionHasRemoteAddress
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
      IN RvInt32              session);

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
        IN RvInt32      session);

#else /* USE_FWNAT */

#define epRtpSendKeepAlive(_rtpH, _session) RV_OK

#endif /* USE_FWNAT */

#else /* RTP_TYPE == RTP_TYPE_NEW */

#define epRtpSendKeepAlive(_rtpH, _session) RV_OK
#define epRtpSessionHasRemoteRTPAddress(_rtpH, _session) RV_FALSE
#define epRtpSessionHasRemoteRTCPAddress(_rtpH, _session) RV_FALSE

#endif /* RTP_TYPE == RTP_TYPE_NEW */

#else /* USE_RTP */

#define epRtpInit(_ep) NULL
#define epRtpEnd(_ep)
#define epRtpDisplayInfo(_rtpH)
#define epRtpSessionNew(_call, _isMultiplexed, _muxId) -1
#define epRtpSessionJoin(_rtpH, _session)
#define epRtpSessionClose(_rtpH, _session, _isMultiplexed)
#define epRtpSessionGetLocalPort(_rtpH, _session) 0
#define epRtpDemuxGetLocalRTP(_rtpH, _addr)
#define epRtpDemuxGetLocalRTCP(_rtpH, _addr)
#define epRtpSessionSetRemoteRTP(_rtpH, _session, _addr, _isMultiplexed, _multiplexId)
#define epRtpSessionSetRemoteRTCP(_rtpH, _session, _addr, _isMultiplexed, _multiplexId)
#define epRtpSessionSetAction(_rtpH, _session, _action)
#define epRtpSessionRate(_rtpH, _session) 0
#define epRtpSessionHasRemoteAddress (_rtpH, _session) 0
#define epRtpSendKeepAlive(_rtpH, _session) RV_OK
#define epRtpSessionHasRemoteRTPAddress(_rtpH, _session) RV_FALSE
#define epRtpSessionHasRemoteRTCPAddress(_rtpH, _session) RV_FALSE

#endif /* USE_RTP */

#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_RTP_H_ */
