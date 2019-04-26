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
ep.c

Sample endpoint module.
This module is used as a sample code for an H.323 endpoint.
It is separate from the GUI to make the life of the programmer easier
when trying to look how to implement code parts.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "cm.h"
#include "seli.h"

#include "lepRtp.h"
#include "lepH450.h"
#include "lepLdap.h"
#include "lepFwNat.h"
#include "lepHash.h"
#include "lepHooks.h"
#include "lepUtils.h"
#include "lepSecurity.h"
#include "lepChannel.h"
#include "lepControl.h"
#include "lepCall.h"
#include "lepRas.h"
#include "lep.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifndef USE_ATT
#if (RV_OS_TYPE == RV_OS_TYPE_WIN32) || (RV_OS_TYPE == RV_OS_TYPE_WINCE)
#include <stdio.h>
#define RvSnprintf _snprintf
#define RvVsnprintf _vsnprintf
#else
#define RvSnprintf snprintf
#define RvVsnprintf vsnprintf
#endif
#endif


/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/





/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/





/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/

// TODO: move someplace else
int
RVCALLCONV cmEvNatRequest(
        IN      HAPP                 hApp,
        IN      cmProtocol           addrType,
        INOUT   cmTransportAddress*  address)
{
    EpObj       *ep;
    RvUint16       port = address->port;

    RV_UNUSED_ARG(addrType)

    cmGetHandle(hApp, (HAPPAPP *)&ep);

    *address = ep->cfg.natAddress;
    address->port = port;

    return 0;
}


/******************************************************************************
 * epInit
 * ----------------------------------------------------------------------------
 * General: Initialize the endpoint object by a configuration file.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         cb               - Implemented callbacks.
 *         configFilename   - Name of configuration file to use.
 *         extraCallSize    - Extra data to save on each call object.
 *         useSecurity      - RV_TRUE to enable security.
 *         useH450          - RV_TRUE to enable H.450.
 *         useRTP           - RV_TRUE to enable RTP.
 *         useFwNat         - RV_TRUE to enable H.460.17/18/19.
 * Output: None.
 *****************************************************************************/
RvStatus epInit(
    IN EpObj        *ep,
    IN EpCallbacks  *cb,
    IN const RvChar *configFilename,
    IN RvSize_t     extraCallSize,
    IN RvBool       useSecurity,
    IN RvBool       useH450,
    IN RvBool       useRTP,
    IN RvBool       useFwNat)
{
    int status;
    SCMEVENT            cmEvent;
    SCMCALLEVENT        cmCallEvent;
    SCMCHANEVENT        cmChanEvent;
    SCMCONTROLEVENT     cmControlEvent;
    SCMPROTOCOLEVENT    cmProtocolEvent;
    SCMRASEVENT         cmRasEvent;
    SCMAUTORASEVENT     cmAutoRasEvent;
    HCFG                hCfg;

    /* cmStartUp() and cmShutdown() are called by the caller of this function */

    memset(ep, 0, sizeof(*ep));
    memset(&ep->cfg.callConfig, -1, sizeof(ep->cfg.callConfig));
    ep->cb = *cb;

    if (ep->cb.epLock != NULL)
    {
        ep->cb.epInitLock(ep, &ep->lock);
    }

    /* Initialize the stack */
    status = cmInitialize(configFilename, &ep->hApp);
    if (status < 0)
        return status;

    hCfg = cmGetConfiguration(ep->hApp);

    ep->hVal = cmGetValTree(ep->hApp);

    /* Get the limits of calls and configurations */
    ep->maxCalls = 10;
    ciGetValue(hCfg, "system.maxCalls", NULL,  &ep->maxCalls);

    if (useH450)
    {
        /* Initialize SupServ (does nothing if not h450 enabled) */
        status = epH450Init(ep, configFilename);
        if (status != RV_OK)
            return status;
    }

    if (useRTP)
    {
        /* Initialize rtp/rtcp (does nothing if not enabled) */
        ep->rtp = epRtpInit(ep);
    }

    if (useSecurity)
    {
        /* Initialize security (does nothing if not security enabled) */
        status = epSecurityInit(ep);
        if (status != RV_OK)
            return status;
    }

    if (useFwNat)
    {
        /* Initialize H.460.17/18/19 (does nothing if not enabled) */
        status = epFwNatInit(ep);
        if (status != RV_OK)
            return status;
    }

    cmSetHandle(ep->hApp, (HAPPAPP)ep);

    memset(&cmEvent, 0, sizeof(cmEvent));
    cmEvent.cmEvNewCall     = cmEvNewCall;
    cmEvent.cmEvRegEvent    = cmEvRegEvent;
    cmEvent.cmEvNatRequest  = cmEvNatRequest;
    cmSetGenEventHandler(ep->hApp, &cmEvent, sizeof(cmEvent));

    memset (&cmCallEvent, 0, sizeof(cmCallEvent));
    cmCallEvent.cmEvCallStateChanged            = cmEvCallStateChanged;
    cmCallEvent.cmEvCallNewRate                 = cmEvCallNewRate;
    cmCallEvent.cmEvCallInfo                    = cmEvCallInfo;
    cmCallEvent.cmEvCallFacility                = cmEvCallFacility;
    cmCallEvent.cmEvCallFastStart               = cmEvCallFastStart;
    cmCallEvent.cmEvCallFastStartSetup          = cmEvCallFastStartSetup;
    cmCallEvent.cmEvCallStatus                  = cmEvCallStatus;
    cmCallEvent.cmEvCallUserInfo                = cmEvCallUserInfo;
    cmCallEvent.cmEvCallH450SupplServ           = cmEvCallH450SupplServ;
    cmCallEvent.cmEvCallIncompleteAddress       = cmEvCallIncompleteAddress;
    cmCallEvent.cmEvCallAdditionalAddress       = cmEvCallAdditionalAddress;
    cmCallEvent.cmEvCallProgress                = cmEvCallProgress;
    cmCallEvent.cmEvCallNotify                  = cmEvCallNotify;
    cmCallEvent.cmEvCallNewAnnexLMessage        = cmEvCallNewAnnexLMessage;
    cmCallEvent.cmEvCallNewAnnexMMessage        = cmEvCallNewAnnexMMessage;
    cmCallEvent.cmEvCallRecvMessage             = cmEvCallRecvMessage;
    cmCallEvent.cmEvCallSendMessage             = cmEvCallSendMessage;
    cmCallEvent.cmEvCallStatusEnquiryTimeout    = cmEvCallStatusEnquiryTimeout;
    cmCallEvent.cmEvCallNatRequest              = cmEvCallNatRequest;
    cmSetCallEventHandler(ep->hApp, &cmCallEvent, sizeof(cmCallEvent));

    memset (&cmChanEvent, 0, sizeof(cmChanEvent));
    cmChanEvent.cmEvChannelStateChanged             = cmEvChannelStateChanged;
    cmChanEvent.cmEvChannelNewRate                  = cmEvChannelNewRate;
    cmChanEvent.cmEvChannelMaxSkew                  = cmEvChannelMaxSkew;
#if (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY)
    cmChanEvent.cmEvChannelSetAddress               = cmEvChannelSetAddress;
    cmChanEvent.cmEvChannelSetRTCPAddress           = cmEvChannelSetRTCPAddress;
#endif
    cmChanEvent.cmEvChannelParameters               = cmEvChannelParameters;
    cmChanEvent.cmEvChannelRTPDynamicPayloadType    = cmEvChannelRTPDynamicPayloadType;
    cmChanEvent.cmEvChannelVideoFastUpdatePicture   = cmEvChannelVideoFastUpdatePicture;
    cmChanEvent.cmEvChannelVideoFastUpdateGOB       = cmEvChannelVideoFastUpdateGOB;
    cmChanEvent.cmEvChannelVideoFastUpdateMB        = cmEvChannelVideoFastUpdateMB;
    cmChanEvent.cmEvChannelHandle                   = cmEvChannelHandle;
    cmChanEvent.cmEvChannelGetRTCPAddress           = cmEvChannelGetRTCPAddress;
    cmChanEvent.cmEvChannelRequestCloseStatus       = cmEvChannelRequestCloseStatus;
    cmChanEvent.cmEvChannelTSTO                     = cmEvChannelTSTO;
    cmChanEvent.cmEvChannelMediaLoopStatus          = cmEvChannelMediaLoopStatus;
    cmChanEvent.cmEvChannelReplace                  = cmEvChannelReplace;
    cmChanEvent.cmEvChannelFlowControlToZero        = cmEvChannelFlowControlToZero;
    cmChanEvent.cmEvChannelMiscCommand              = cmEvChannelMiscCommand;
    cmChanEvent.cmEvChannelTransportCapInd          = cmEvChannelTransportCapInd;
    cmChanEvent.cmEvChannelSetNSAPAddress           = cmEvChannelSetNSAPAddress;
    cmChanEvent.cmEvChannelSetATMVC                 = cmEvChannelSetATMVC;
    cmChanEvent.cmEvChannelRecvMessage              = cmEvChannelRecvMessage;
    cmChanEvent.cmEvChannelSendMessage              = cmEvChannelSendMessage;
    cmChanEvent.cmEvChannelMasterSlaveConflict      = cmEvChannelMasterSlaveConflict;
    cmSetChannelEventHandler(ep->hApp, &cmChanEvent, sizeof(cmChanEvent));

    memset (&cmControlEvent, 0, sizeof(cmControlEvent));
    cmControlEvent.cmEvCallCapabilities             = cmEvCallCapabilities;
    cmControlEvent.cmEvCallCapabilitiesExt          = cmEvCallCapabilitiesExt;
    cmControlEvent.cmEvCallNewChannel               = cmEvCallNewChannel;
    cmControlEvent.cmEvCallCapabilitiesResponse     = cmEvCallCapabilitiesResponse;
    cmControlEvent.cmEvCallMasterSlaveStatus        = cmEvCallMasterSlaveStatus;
    cmControlEvent.cmEvCallRoundTripDelay           = cmEvCallRoundTripDelay;
    cmControlEvent.cmEvCallUserInput                = cmEvCallUserInput;
    cmControlEvent.cmEvCallRequestMode              = cmEvCallRequestMode;
    cmControlEvent.cmEvCallMiscStatus               = cmEvCallMiscStatus;
    cmControlEvent.cmEvCallControlStateChanged      = cmEvCallControlStateChanged;
    cmControlEvent.cmEvCallMasterSlave              = cmEvCallMasterSlave;
    cmControlEvent.cmEvCallControlMessage           = cmEvCallControlMessage;
    cmControlEvent.cmEvCallTerminalNumberAssign     = cmEvCallTerminalNumberAssign;
    cmSetControlEventHandler(ep->hApp, &cmControlEvent, sizeof(cmControlEvent));

    memset (&cmProtocolEvent, 0, sizeof(cmProtocolEvent));
    /*cmProtocolEvent.hookConnecting  = hookConnecting;*/
    cmProtocolEvent.hookInConn      = hookInConn;
    cmProtocolEvent.hookOutConn     = hookOutConn;
    cmProtocolEvent.hookClose       = hookClose;
    cmProtocolEvent.hookSend        = hookSend;
    cmProtocolEvent.hookRecv        = hookRecv;
    cmProtocolEvent.hookSendTo      = hookSendTo;
    cmProtocolEvent.hookRecvFrom    = hookRecvFrom;
    cmProtocolEvent.hookConnecting  = hookConnecting;
    cmProtocolEvent.hookListening   = hookListening;
    cmProtocolEvent.hookSendRaw     = hookSendRaw;
    cmProtocolEvent.hookSendRawTo   = hookSendRawTo;
    cmSetProtocolEventHandler(ep->hApp, &cmProtocolEvent, sizeof(cmProtocolEvent));

    memset(&cmRasEvent, 0, sizeof(cmRasEvent));
    cmRasEvent.cmEvRASRequest               = cmEvRASRequest;
    cmRasEvent.cmEvRASConfirm               = cmEvRASConfirm;
    cmRasEvent.cmEvRASReject                = cmEvRASReject;
    cmRasEvent.cmEvRASTimeout               = cmEvRASTimeout;
    cmRasEvent.cmEvRASMessageSend           = cmEvRASMessageSend;
    cmRASSetEventHandler(ep->hApp, &cmRasEvent, sizeof(cmRasEvent));

    memset(&cmAutoRasEvent, 0, sizeof(cmAutoRasEvent));
    cmAutoRasEvent.cmEvAutoRASRequest   = cmEvAutoRASRequest;
    cmAutoRasEvent.cmEvAutoRASConfirm   = cmEvAutoRASConfirm;
    cmAutoRasEvent.cmEvAutoRASReject    = cmEvAutoRASReject;
    cmAutoRasEvent.cmEvAutoRASTimeout   = cmEvAutoRASTimeout;
    cmAutoRASSetEventHandler(ep->hApp, &cmAutoRasEvent, sizeof(cmAutoRasEvent));

    ep->hTimers = mtimerInit(10, NULL);
    ep->dropAllTimer = HTI_NULL_TIMER;
    ep->cfg.ignoreIncomingFid = -1;

    ep->bIsManualRAS = (ciGetValue(hCfg, "ras.manualRAS", NULL, NULL) == ERR_CI_NOERROR);
    ep->partialUrqAliases = RV_PVT_INVALID_NODEID;

    /* See if port-range feature is supported */
    if ((ciGetValue(hCfg, "system.portFrom", NULL, NULL) != ERR_CI_NOERROR) &&
        (ciGetValue(hCfg, "system.portTo", NULL, NULL) != ERR_CI_NOERROR))
    {
        ciGetValue(hCfg, "system.portFrom", NULL, &ep->portFrom);
        ciGetValue(hCfg, "system.portTo", NULL, &ep->portTo);
    }
    else
    {
        /* No port-range feature */
        ep->portFrom = 1;
        ep->portTo = 0;
    }

    /* Create hash tables */
    ep->calls = epHashInit(ep, RvRoundToSize(sizeof(EpCallObj), RV_ALIGN_SIZE) + extraCallSize);
    ep->channels = epHashInit(ep, sizeof(EpChannelObj));

    return RV_OK;
}


/******************************************************************************
 * epEnd
 * ----------------------------------------------------------------------------
 * General: Destruct the endpoint object.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epEnd(
    IN EpObj        *ep)
{
    RvStatus status = RV_OK;

    /* Unregister the endpoint if necessary */
    if (ep->bIsRegistered)
    {
        ep->bIsRegistered = RV_FALSE;
        if (cmUnregister(ep->hApp) >= 0)
        {
            seliSelectUntil(100);
        }
    }

    if (ep->hApp != NULL)
    {
        /* first stop the stack */
        cmStop(ep->hApp);
    }

    if (ep->dropAllTimer != HTI_NULL_TIMER)
    {
        mtimerReset(ep->hTimers, ep->dropAllTimer);
        ep->dropAllTimer = HTI_NULL_TIMER;
    }
    mtimerEnd(ep->hTimers);

    status = epFwNatEnd(ep);

    status = epLdapEnd(ep);

    status = epSecurityEnd(ep);

    epRtpEnd(ep->rtp);

    epH450End(ep);

    if (ep->hApp != NULL)
        cmEnd(ep->hApp);

    if (ep->calls != NULL)
        epHashEnd(ep->calls);
    if (ep->channels != NULL)
        epHashEnd(ep->channels);

    if (ep->cb.epLock != NULL)
    {
        LockHANDLE lock = ep->lock;
        ep->cb.epLock(ep, lock);
        ep->lock = NULL;
        ep->cb.epUnlock(ep, lock);
        ep->cb.epEndLock(ep, lock);
    }

    return RV_OK;
}


/******************************************************************************
 * epStart
 * ----------------------------------------------------------------------------
 * General: Start running the endpoint.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epStart(
    IN EpObj        *ep)
{
    RvStatus status;

    status = cmStart(ep->hApp);
    epH450Start(ep);
    if (status > 0)
        status = RV_OK;

    return status;
}


/******************************************************************************
 * epStop
 * ----------------------------------------------------------------------------
 * General: Stop running the endpoint.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epStop(
    IN EpObj        *ep)
{
    RvStatus status;

    ep->bIsRegistered = RV_FALSE;

    status = cmStop(ep->hApp);
    if (status > 0)
        status = RV_OK;

    if (status == RV_OK)
        status = epH450Stop(ep);

    if (status == RV_OK)
        status = epFwNatEnd(ep);

    return status;
}


/******************************************************************************
 * epRebind
 * ----------------------------------------------------------------------------
 * General: Rebind all listening addresses.
 *          Can be used to update the NAT address linked to internal listening
 *          addresses.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epRebind(
    IN EpObj        *ep)
{
    RvStatus status;

    epUtilsEvent(ep, "Rec", NULL, "rebindStack");

    status = cmRestartListen(ep->hApp);

    return status;
}


/******************************************************************************
 * epReset
 * ----------------------------------------------------------------------------
 * General: Reset recording related information in the endpoint.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epReset(
    IN EpObj        *ep)
{
#ifdef USE_H450
    ep->h450outCounter = 0;
#else
    RV_UNUSED_ARG(ep);
#endif
    return RV_OK;
}


/******************************************************************************
 * epGetConfig
 * ----------------------------------------------------------------------------
 * General: Get configuration parameters pointer.
 *          These parameters can be modified by the caller for future
 *          behavior.
 *
 * Return Value: Pointer to configuration parameters.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
EpConfig *epGetConfig(
    IN EpObj        *ep)
{
    return &ep->cfg;
}






/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/







#ifdef __cplusplus
}
#endif
