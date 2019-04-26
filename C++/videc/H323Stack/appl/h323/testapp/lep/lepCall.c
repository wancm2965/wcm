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
epCall.c

Call handling of the endpoint.
Handles the call procedures.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "stkutils.h"
#include "h245.h"
#include "seli.h"
#include "mti.h"

#include "lep.h"
#include "lepRtp.h"
#include "lepH450.h"
#include "lepSecurity.h"
#include "lepHash.h"
#include "lepUtils.h"
#include "lepConversions.h"
#include "lepCall.h"
#include "lepFwNat.h"
#include "lepRtp.h"

#ifdef USE_PLAYER
#include "lepPlayMedia.h"
#endif


#ifdef __cplusplus
extern "C" {
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

static void RVCALLCONV dropSingleCall(IN void* context);

static int callDialAltEp(EpCallObj   *call);


/*-----------------------------------------------------------------------*/
/*                           CALLBACK FUNCTIONS                          */
/*-----------------------------------------------------------------------*/


int RVCALLCONV cmEvNewCall(
    IN  HAPP        hApp,
    IN  HCALL       hsCall,
    OUT LPHAPPCALL  lphaCall)
{
    EpObj       *ep;
    EpCallObj   *call;

    cmGetHandle(hApp, (HAPPAPP *)&ep);

    call = epCallCreate(ep, hsCall, 0);
    if (call == NULL)
    {
        /* Call was rejected ??? */
        cmCallDrop(hsCall);
        return 0;
    }

    /* Set the application's call handle */
    *lphaCall = (HAPPCALL)call;
    epUtilsEvent(call->ep, "Q931", call, "cmEvNewCall");

#ifdef USE_PLAYER
    PlayerInitTheCall(call);
#endif

    return 0;
}


int RVCALLCONV cmEvCallStateChanged(
    IN HAPPCALL             haCall,
    IN HCALL                hsCall,
    IN cmCallState_e        state,
    IN cmCallStateMode_e    stateMode)
{
    EpObj           *ep;
    EpCallObj       *call = (EpCallObj *)haCall;

    if (call == NULL)
    {
        /* No application handle means this is a closing call that was never handled from our point of view */
        return 0;
    }

    ep = call->ep;
    lepLockCall(call);

    epUtilsEvent(ep, "callState", call, "%s %s", CMCallState2String(state), CMCallStateMode2String(stateMode));

    call->state = state;
    call->stateMode = stateMode;

    if (state == cmCallStateConnected)
    {
        if (stateMode == cmCallStateModeConnectedCallSetup)
        {
            call->wasConnected = RV_TRUE;
            call->wasConnectedControl = RV_FALSE;
        }
        else if (stateMode == cmCallStateModeConnectedCall)
            call->wasConnectedControl = RV_TRUE;
    }

    /* Notify H450 state machines as to call state change.
       does nothing if H450 is not enabled */
    epH450CallStateChanged(call);

    if ((state != cmCallStateDisconnected) && (stateMode == cmCallStateModeDisconnectedBusy))
    {
        epUtilsEvent(ep, "callStateDisplay", call, CMCallState2String(state));
    }
    else
    {
        epUtilsEvent(ep, "callStateDisplay", call, CMCallStateMode2String(stateMode));
    }

    switch (state)
    {
        case  cmCallStateIdle:
        {
            RvInt32 reason = -1;
            cmCallGetParam(hsCall, cmParamDisconnectionReason, 0, &reason, NULL);

            epUtilsLog(ep, call, "Closed (%s).", CMCallDisconnectionReason2String((cmCallDisconnectionReason)reason));

            /* We should close and kill this call */
            ep->cb.epCallClose(call);

            if (!call->isIndependent)
            {
                lepUnlockCall(call);
                epCallDestruct(call);
                return 0;
            }
            break;
        }

        case cmCallStateConnected:
        {
            if (stateMode == cmCallStateModeConnectedCallSetup)
            {
                int connCnt;

                if(!call->sendComplete)
                {
                    /* in case the configuration uses auto-answer, offering will not be passed through */
                    call->sendComplete = RV_TRUE;
                }

                connCnt = epCallColorIt(call);
                epUtilsEvent(ep, "Hooks", call, "conCall.%d", call->id, connCnt);

                epSecurityCallGenerateSharedSecret(call);
            }
            break;
        }

        case cmCallStateDialtone:
        {
            int connCnt = epCallColorIt(call);
            epUtilsEvent(ep, "Hooks", call, "outCall.%d", call->id, connCnt);

            /* send calling name if we have to */
            epH450CallSendNameID(call, calling);

            if (call->initFsOnDialTone)
            {
                epCallGenerateFaststartRequest(call, RV_TRUE);
            }

            if (stateMode == cmCallStateModeSetupNoReply)
            {
	            callDialAltEp(call);
            }
            else if (stateMode == cmCallStateModeSetupGotReleaseComplete)
            {
	            callDialAltEp(call);
            }
            break;
        }

        case cmCallStateProceeding:
        {
            epUtilsEvent(ep, "TCL", call, "Window delete .redial%p", call->hsCall);
            if (stateMode == cmCallStateModeSetupGotReleaseComplete)
            {
	            callDialAltEp(call);
            }
            break;
        }
        case cmCallStateRingBack:
            break;

        case cmCallStateOffering:
            {
                int connCnt = epCallColorIt(call);
                epUtilsEvent(ep, "Hooks", call, "inCall.%d", call->id, connCnt);

                call->sendComplete = RV_TRUE; /* We're done with OVSP at this stage */

                if (!call->isIndependent && ep->cfg.bAutoAnswer)
                {
                    /* We automatically answer this call */
                    epH450CallSendNameID(call, connected);
                    lepUnlockCallForAPI(call, cmCallAnswer(call->hsCall));
                }
                break;
            }

        case cmCallStateDisconnected:
            {
                /* Signal to the channel Idle procedure not to reprint the channels */
                call->numOfChannels = -1;
                break;
            }

        case cmCallStateWaitAddressAck:
            {
                if (ep->cfg.bAutoAnswer)
                {
                    /* Calls are automatically answered in this mode - we're not going to wait for more digits */
                    lepUnlockCallForAPI(call, cmCallAddressComplete(hsCall));
                }
                else
                {
                    call->sendComplete = RV_FALSE;
                    epUtilsLog(ep, call, "Call waiting for OVSP completion");
                }
                break;
            }

        case cmCallStateTransfering:
            /* Call's direction might have changed, and we also need to reset the connection handle */
            if (call->isIncoming && call->isFastConnectCall)
            {
                /* we need to initiate FS, but only in offering state */
                call->initFsOnDialTone = RV_TRUE;
            }
            call->isIncoming = RV_FALSE;
            call->hCon = NULL;
            break;

        default:
            break;
    }

    if (call->isIndependent)
    {
        /* Advanced script handling */
        epUtilsEvent(ep, "Script", call, "cb:cm:EvCallStateChanged %p %s %s", call, CMCallState2String(state), CMCallStateMode2String(stateMode));
    }
    lepUnlockCall(call);
    return 0;
}


int RVCALLCONV cmEvCallNewRate(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN RvUint32 rate)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);

    lepLockCall(call);
    epUtilsEvent(call->ep, "Q931", call, "cmEvCallNewRate.%d", rate);
    epUtilsLog(call->ep, call, "Rate changed. new rate: %d", rate);
    lepUnlockCall(call);

    return 0;
}


int RVCALLCONV cmEvCallInfo(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN char*    display,
    IN char*    userUser,
    IN int      userUserSize)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(display);
    RV_UNUSED_ARG(userUser);
    RV_UNUSED_ARG(userUserSize);

    lepLockCall(call);
    epUtilsEvent(call->ep, "Q931", call, "cmEvCallInfo");
    lepUnlockCall(call);

    return 0;
}


int RVCALLCONV cmEvCallFacility(
    IN      HAPPCALL haCall,
    IN      HCALL    hsCall,
    IN      int      handle,
    OUT IN  RvBool   *proceed)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);
    lepLockCall(call);

    epUtilsEvent(call->ep, "Q931", call, "cmEvCallFacility.%d", (handle >= 0));

    /* We always let the stack process incoming Facility messages */
    *proceed = RV_TRUE;

    if (call->ep->cfg.bScanNodeIDs)
        epScanPvtNode(call->ep, handle);

    lepUnlockCall(call);
    return 0;
}


int RVCALLCONV cmEvCallFastStartSetup(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    OUT IN  cmFastStartMessage  *fsMessage)
{
    /* This is not the best of callbacks - it has limitations to the number of channels it can handle...
       It is advisable to use cmEvCallFastStart() instead. */
    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(haCall);
    RV_UNUSED_ARG(fsMessage);

    return 0;
}


int RVCALLCONV cmEvCallStatus(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    OUT IN  cmCallStatusMessage *callStatusMsg)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);
    lepLockCall(call);

    epUtilsEvent(call->ep, "Q931", call, "cmEvCallStatus");
    epUtilsLog(call->ep, call, "Received STATUS (state=%d %d, cause=%d %d %d %d %d, display=%s)",
        callStatusMsg->callStateInfo.cmCallStateValue, callStatusMsg->callStateInfo.cmCodingStandard,
        callStatusMsg->callCauseInfo.cmCauseValue , callStatusMsg->callCauseInfo.cmCodingStandard,
        callStatusMsg->callCauseInfo.cmLocation, callStatusMsg->callCauseInfo.cmRecomendation,
        callStatusMsg->callCauseInfo.cmSpare, callStatusMsg->display);

    lepUnlockCall(call);
    return 0;
}


int RVCALLCONV cmEvCallUserInfo(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN int      handle)
{
    EpCallObj *call = (EpCallObj *)haCall;
    char display[128], data[64];
    cmNonStandardParam param;
    int status;
    param.data = data;
    param.length = 64;

    RV_UNUSED_ARG(hsCall);
    lepLockCall(call);

    epUtilsEvent(call->ep, "Q931", call, "cmEvCallUserInfo");

    lepUnlockCall(call);
    status =  cmCallUserInformationGet(call->ep->hApp, handle, display, 128, &param);
    lepLockCall(call);
    if (status >= 0)
    {
        epUtilsLog(call->ep, call, "Received User Information: display=%s", display);
        if (param.length >= 0)
            epUtilsLog(call->ep, call, "NSD: %s", CMNonStandardParam2String(&param));
    }

    lepUnlockCall(call);
    return status;
}


int RVCALLCONV cmEvCallH450SupplServ(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      cmCallQ931MsgType   msgType,
    IN      int                 nodeId,
    IN      int                 size)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);

    lepLockCall(call);
    epUtilsEvent(call->ep, "Q931", call, "cmEvCallH450SupplServ.%d", msgType); /* GUI */

    if (call != NULL)
        epH450CallIncomingMessage(call, msgType, nodeId, size);

    lepUnlockCall(call);
    return 0;
}


int RVCALLCONV cmEvCallIncompleteAddress(
    IN  HAPPCALL    haCall,
    IN  HCALL       hsCall)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);

    lepLockCall(call);
    epUtilsEvent(call->ep, "Q931", call, "cmEvCallIncompleteAddress"); /* GUI */

    if (call->isIndependent)
        epUtilsEvent(call->ep, "Script", call, "cb:cm:EvCallIncompleteAddress %p", call);

    lepUnlockCall(call);
    return 0;
}


int RVCALLCONV cmEvCallAdditionalAddress(
    IN HAPPCALL        haCall,
    IN HCALL           hsCall,
    IN char*           address,
    IN RvBool          sendingComplete)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);
    lepLockCall(call);

    epUtilsEvent(call->ep, "Q931", call, "cmEvCallAdditionalAddress.%s.%d", address, sendingComplete); /* GUI */
    epUtilsLog(call->ep, call, "Additional digits: %s", address);

    if (call->isIndependent)
    {
        epUtilsEvent(call->ep, "Script", call, "cb:cm:EvCallAdditionalAddress %p %s %d",
            call, address, sendingComplete);
    }

    lepUnlockCall(call);
    return 0;
}


int RVCALLCONV cmEvCallFastStart(
    IN  HAPPCALL                haCall,
    IN  HCALL                   hsCall)
{
    EpCallObj                       *call = (EpCallObj *)haCall;
    EpObj                           *ep = call->ep;
    RvInt32                         intValue = -1;
    cmExtendedFastConnectState_e    efcState;

    RV_UNUSED_ARG(hsCall)
    lepLockCall(call);

    cmCallGetParam(hsCall, cmParamExtendedFastConnectState, 0, &intValue, NULL);
    efcState = (cmExtendedFastConnectState_e)intValue;

    epUtilsEvent(ep, "Q931", call, "cmEvCallFastStart"); /* GUI */
    if (efcState == cmExtendedFastConnectApproved)
        epUtilsEvent(ep, "Q931", call, "efcApproved");

    call->isFastConnectCall = RV_TRUE;
    if (!ep->cfg.bSupportFaststart)
    {
        /* we do not support FS, reject */
        ep->bIgnoreRec = RV_TRUE;
        epCallFaststartRefuse(call);
        ep->bIgnoreRec = RV_FALSE;
        lepUnlockCall(call);
        return 0;
    }

    if (ep->cfg.bAutoAnswer && !(ep->cfg.bAutomaticFaststart && ep->cfg.bSupportFaststart))
    {
        lepUnlockCall(call);
        return 0; /* We ignore FastStart if it's not automatic when AutoAnswer is on */
    }

    if (ep->cfg.bAutomaticFaststart && ep->cfg.bSupportFaststart)
        epCallFaststartAutoAck(call);

    lepUnlockCall(call);
    return 0;
}


int RVCALLCONV cmEvCallProgress(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN int      handle)
{
    EpCallObj *call = (EpCallObj *)haCall;
    char display[128], data[64];
    int cause = -1, progressDescription = -1, notificationDescription = -1;
    progressInd_IE pi_IE = (progressInd_IE)-1;
    cmNonStandardParam param;
    int status;
    param.data = data;
    param.length = 64;

    RV_UNUSED_ARG(hsCall);

    status =  cmCallProgressGet(call->ep->hApp, handle, display, 128, &cause, &pi_IE,
        &progressDescription, &notificationDescription, &param);

    lepLockCall(call);
    epUtilsEvent(call->ep, "Q931", call, "cmEvCallProgress.%d.%d.%d.%d",
        progressDescription, notificationDescription, cause, pi_IE);

    if (status >= 0)
    {
        epUtilsLog(call->ep, call, "Received Progress: display=%s; cause=%d; pi_IE=%d", display, cause, pi_IE);
        if (param.length >= 0)
            epUtilsLog(call->ep, call, "NSD: %s", CMNonStandardParam2String(&param));
    }

    lepUnlockCall(call);
    return status;
}


int RVCALLCONV cmEvCallNotify(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN int      handle)
{
    EpCallObj *call = (EpCallObj *)haCall;
    char display[128], data[64];
    int notificationDescription = -1;
    cmNonStandardParam param;
    int status;
    param.data = data;
    param.length = 64;

    RV_UNUSED_ARG(hsCall);

    status =  cmCallNotifyGet(call->ep->hApp, handle, display, 128, &notificationDescription, &param);

    lepLockCall(call);
    epUtilsEvent(call->ep, "Q931", call, "cmEvCallNotify.%d", notificationDescription);

    if (status >= 0)
    {
        epUtilsLog(call->ep, call, "Received Notify: display=%s; description=%d", display, notificationDescription);
        if (param.length >= 0)
            epUtilsLog(call->ep, call, "NSD: %s", CMNonStandardParam2String(&param));
    }
    lepUnlockCall(call);
    return status;
}


int RVCALLCONV cmEvCallNewAnnexLMessage(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      int                 annexLElement,
    IN      int                 msgType)
{
    EpCallObj *call = (EpCallObj *)haCall;
    char message[100];
    int size=100;
    char NonStandardData[100];
    RvBool isText;
    cmNonStandardParam param;

    RV_UNUSED_ARG(hsCall);
    lepLockCall(call);

    epUtilsEvent(call->ep, "Q931", call, "cmEvCallNewAnnexLMessage.%d", msgType);

    param.length = (int)sizeof(NonStandardData);
    param.data = (char*)NonStandardData;
    strcpy(param.info.object, "1 1 1 1");
    param.info.objectLength = 7;

    epUtilsLog(call->ep, call, "AnnexL message received");

    lepUnlockCallForAPI(call, cmCallGetAnnexLMessage(call->ep->hApp, annexLElement, &isText, message, &size, &param));
    if (isText)
    {
        if (size < 100)
            message[size] = 0;
        epUtilsLog(call->ep, call, " \"%s\"", message);
    }
    epUtilsLog(call->ep, call, "NSD: %s", CMNonStandardParam2String(&param));

    lepUnlockCall(call);
    return 0;
}


int RVCALLCONV cmEvCallNewAnnexMMessage(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      int                 annexMElement,
    IN      int                 msgType)
{
    EpCallObj *call = (EpCallObj *)haCall;
    EpObj *ep = call->ep;
    char message[100];
    int size=100;
    int i=1;
    char NonStandardData[100];
    RvBool tunnelingRequired;
    cmTunnelledProtocolID tunnelledProtocolID;
    cmNonStandardParam param;

    RV_UNUSED_ARG(hsCall);
    lepLockCall(call);

    epUtilsEvent(call->ep, "Q931", call, "cmEvCallNewAnnexMMessage.%d", msgType);

    param.length = (int)sizeof(NonStandardData);
    param.data = (char*)NonStandardData;
    strcpy(param.info.object, "1 1 1 1");
    param.info.objectLength = 7;

    epUtilsLog(call->ep, call, "AnnexM message received");

    lepUnlockCallForAPI(call, cmCallGetAnnexMMessage(ep->hApp, annexMElement, &tunnelingRequired,
        &tunnelledProtocolID, &param));
    while (cmCallGetAnnexMMessageContent(ep->hApp, annexMElement, i++, &size, message) >= 0)
    {
        if (size < 100)
            message[size] = 0;
        epUtilsLog(call->ep, call, " \"%s\"", message);
        size = 100;
    }
    epUtilsLog(call->ep, call, "NSD: %s", CMNonStandardParam2String(&param));

    lepUnlockCall(call);
    return 0;
}


int RVCALLCONV cmEvCallSendMessage(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN int      message)
{
    EpCallObj *call = (EpCallObj *)haCall;
    EpObj *ep;
    RvChar *messageStr;
    RvChar *protocolStr;
    cmProtocol prot;
    RvBool bInfo = RV_FALSE;
    RvStatus status;

    RV_UNUSED_ARG(bInfo);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsCall), (HAPPAPP*)&ep);

    status = epFwNatCallSendMessage(ep, call, message, &bInfo);

    messageStr = cmGetProtocolMessageName(ep->hApp, message);
    prot = cmProtocolGetProtocol(ep->hApp, message);
    protocolStr = cmProtocolGetProtocolName(prot);

    epUtilsEvent(ep, protocolStr, call, "out.%s %d", messageStr, message);
    epUtilsLog(ep, call, "New message sent -> %s on %s", messageStr, protocolStr);

    if (call != NULL)
    {
        lepLockCall(call);
        if (!call->separateH245 && (prot == cmProtocolH245))
        {
            /* Check if it's a separate connection */
            RvInt32 value = 1;
            RvBool hasControl;
            RvStatus res;

            lepUnlockCallForAPI(call, hasControl=cmCallHasControlSession(hsCall));
            lepUnlockCallForAPI(call, res=cmCallGetParam(hsCall, cmParamH245Tunneling, 0, &value, NULL));

            if (hasControl && (res >= 0) && (value == 0))
            {
                call->separateH245 = RV_TRUE;
                epUtilsEvent(ep, "Tag", call, "\"eventRemoval%d\" \"ByType\"", call->id);
                epUtilsEvent(ep, "Catch", call, "record:separateCall %d", call->id);
            }
        }
        lepUnlockCall(call);
    }

    ep->cb.epMessage(ep, call, message, messageStr, RV_TRUE);

    return 0;
}


void RVCALLCONV cmEvCallStatusEnquiryTimeout(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);
    lepLockCall(call);

    epUtilsEvent(call->ep, "Q931", call, "cmEvCallStatusEnquiryTimeout");
    epUtilsLog(call->ep, call, "StatusEnquiry has timed out");
    lepUnlockCall(call);
}


int RVCALLCONV cmEvCallNatRequest(
        IN      HAPPCALL             haCall,
        IN      HCALL                hsCall,
        IN      cmProtocol           addrType,
        INOUT   cmTransportAddress*  address)
{
    EpCallObj *call = (EpCallObj *)haCall;
    RvUint16 port = address->port;

    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(addrType);
    lepLockCall(call);

    *address = call->ep->cfg.natAddress;
    address->port = port;

    lepUnlockCall(call);
    return 0;
}





/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * epCallCreate
 * ----------------------------------------------------------------------------
 * General: Create a complete call object, and sets call parameters for it.
 *
 * Return Value: Call object created.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep       - Endpoint object to use.
 *         hsCall   - Incoming call handle, or NULL for outgoing calls.
 *         id       - Id to give to the new call. If 0, will be allocated by
 *                    the allocation resource callback.
 * Output: None.
 *****************************************************************************/
EpCallObj *epCallCreate(
    IN EpObj    *ep,
    IN HCALL    hsCall,
    IN RvInt32  id)
{
    EpCallObj *call;
    RvStatus status;
    RvBool isIncoming = (hsCall != NULL);

    /* Ask for an ID for this call */
    if (id <= 0)
    {
        lepLockEpForHash(ep, id = ep->cb.epAllocateResourceId(ep, EpResourceCall));
    }
    if (id <= 0)
    {
        /* Call was rejected ??? */
        return NULL;
    }

    /* Allocate a call object */
    lepLockEpForHash(ep, status = epHashAdd(ep->calls, id, (void**)&call));
    if (status != RV_OK)
    {
        /* No internal resources ??? */
        return NULL;
    }

    if (hsCall == NULL)
    {
        status = cmCallNew(ep->hApp, (HAPPCALL)call, &hsCall);
        if (status < 0)
        {
            lepLockEpForHash(ep, epHashRemove(ep->calls, id));
            return NULL;
        }
    }

    memset(call, 0, sizeof(*call));
    call->ep = ep;
    call->id = id;
    call->hsCall = hsCall;
    call->isIndependent = (isIncoming && ep->cfg.bIndependent);
    call->isIncoming = isIncoming;
    call->state = cmCallStateInit;
    call->stateMode = (cmCallStateMode_e)-1;
    memset(call->rtpSessions, 0xff, (sizeof(call->rtpSessions)));
    call->muxedRtpSessions[0] = call->muxedRtpSessions[1] = call->muxedRtpSessions[2] = -1;
    call->multiplexID[0] = call->multiplexID[1] = call->multiplexID[2] = (RvUint32)-1;
    call->remoteMultiplexID[0] = call->remoteMultiplexID[1] = call->remoteMultiplexID[2] = (RvUint32)-1;
#ifdef USE_FWNAT
    call->bUseH46019 = (ep->cfg.fwNatConfig.e19Config != RvH323FwNatH46019ConfigNotUsed);
    call->bRemoteMediaMultiplexEnabled = RV_FALSE;
    if (ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigServer)
        call->bRemoteMediaMultiplexEnabled = RV_TRUE;
#endif

    if (ep->cb.epLock != NULL)
    {
        ep->cb.epInitLock(ep, &call->lock);
    }

    epUtilsEvent(ep, "Tag", call, "eventRemoval%d OneCall", call->id);

    /* Sets up SupServ, does nothing if H450 is not enabled */
    status = epH450CallInit(call);
    if (status != RV_OK)
    {
        epUtilsError(ep, call, "Unable to initialize the H.450 part of the call");
        cmCallDrop(hsCall);
        return NULL;
    }

    /* Sets up Security, does nothing if H235 is not enabled */
    status = epSecurityCallInit(call);
    if (status != RV_OK)
    {
        epUtilsError(ep, call, "Unable to initialize the H.235 part of the call");
        cmCallDrop(hsCall);
        return NULL;
    }

    if (call->isIndependent)
    {
        /* Advanced script handling */
        epUtilsEvent(ep, "Script", call, "cb:cm:EvNewCall %p", call);
    }
    else
    {
        epCallSetOptions(call, &ep->cfg.callConfig);
    }

    /* Some outgoing specific parameters */
    if (!isIncoming)
    {
        call->isFastConnectCall = ep->cfg.bSupportFaststart;

        if (ep->cfg.dialFromIp[0])
        {
            cmTransportAddress fromTa;
            if (cmString2TransportAddress(ep->cfg.dialFromIp, &fromTa) == RV_OK)
            {
                if (cmCallSetParam(hsCall, cmParamSourceIpAddress, 1, (RvInt32)sizeof(fromTa), (RvChar*)&fromTa) < 0)
                    epUtilsError(ep, call, "Unable to set source address - using whatever we get from the stack");
            }
            else
            {
                epUtilsError(ep, call, "Bad dialFrom address given");
            }
        }
    }

    /* Set call media, if supported */
    call->action = ep->cfg.mediaAction;

    ep->cb.epCallNew(call, !isIncoming);
    {
        char printString[64];
        sprintf(printString, "Associated call [%p] with id [%d]", hsCall, id);
        ep->cb.epLog(ep, call, printString);
    }
    epUtilsEvent(ep, "callStateDisplay", call, CMCallState2String(cmCallStateInit));

#ifdef USE_PLAYER
    PlayerInitTheCall(call);
#endif

    return call;
}


/******************************************************************************
 * epCallClose
 * ----------------------------------------------------------------------------
 * General: Close a call object.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallDestruct(
    IN EpCallObj            *call)
{
    RvStatus status;
    EpObj *ep;
    int i;

    ep = call->ep;
    lepLockCall(call);

    for (i = 0; i < 3; i++)
    {
        if (call->rtpSessions[i] >= 0)
        {
            epRtpSessionClose(ep->rtp, call->rtpSessions[i], RV_FALSE);
        }

        if (call->muxedRtpSessions[i] >= 0)
        {
            epRtpSessionClose(ep->rtp, call->muxedRtpSessions[i], RV_FALSE);
        }
    }

    status = epSecurityCallEnd(call);
    epH450CallEnd(call);

#ifdef USE_PLAYER
    PlayerCallDestruct(call);
#endif

    cmCallClose(call->hsCall);
    call->hsCall = NULL; /* Just to be on the safe side */
    epHashRemove(ep->calls, call->id); /* Bye bye... */

    if (ep->cb.epLock != NULL)
    {
        LockHANDLE lock = call->lock;
        call->lock = NULL;
        ep->cb.epUnlock(ep, lock);
        ep->cb.epEndLock(ep, lock);
    }

    return RV_OK;
}


/******************************************************************************
 * epCallGetExtraData
 * ----------------------------------------------------------------------------
 * General: Get the extra data of the call object.
 *
 * Return Value: Pointer to the extra data of the call.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
void *epCallGetExtraData(
    IN EpCallObj            *call)
{
    void *ret;
    lepLockCall(call);
    ret = (((RvUint8*)call) + RvRoundToSize(sizeof(*call), RV_ALIGN_SIZE));
    lepUnlockCall(call);
    return ret;
}


/******************************************************************************
 * epCallSetDestAddress
 * ----------------------------------------------------------------------------
 * General: Add a destination address to the call before dialing it.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         aliasStr     - Alias to add, in string format.
 *         index        - Index of the alias.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSetDestAddress(
    IN EpCallObj            *call,
    IN const RvChar         *aliasStr,
    IN int                  index)
{
    RvStatus status;
    cmAlias alias;

    lepLockCall(call);
    epUtilsEvent(call->ep, "Rec", NULL, "setCallAlias entry= %d dest= \"%s\" index= %d", call->id, aliasStr, index);

    status = String2Alias(call->ep, aliasStr, &alias);
    if (status == RV_OK)
    {
        lepUnlockCallForAPI(call, status=cmCallSetParam(call->hsCall, cmParamDestinationAddress, index,
            sizeof(alias), (char *)&alias));

        if (status >= 0)
        {
            if (!call->setCalledPartNumber && (alias.type == cmAliasTypeE164))
            {
                lepUnlockCallForAPI(call, cmCallSetParam(call->hsCall, cmParamCalledPartyNumber, 0,
                    sizeof(alias), (char *)&alias));
                call->setCalledPartNumber = RV_TRUE;
            }

            status = RV_OK;
        }

        FreeAlias(call->ep, &alias);
    }

    lepUnlockCall(call);
    return status;
}


/******************************************************************************
 * epCallDial
 * ----------------------------------------------------------------------------
 * General: Dial out a call to a given IP address (optional).
 *          All settings for this call are done using cmCallSetParam() before
 *          calling this function.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         destIpStr    - Destination IP address. Can be NULL.
 * Output: None.
 *****************************************************************************/
RvStatus epCallDial(
    IN EpCallObj            *call,
    IN const RvChar         *destIpStr)
{
    RvStatus status = RV_OK;
    RvBool anyDest;
    HCALL hsCall = call->hsCall;

    lepLockCall(call);

    anyDest = ((destIpStr != NULL) && destIpStr[0]);

    if (anyDest)
        epUtilsEvent(call->ep, "Rec", NULL, "makeCall entry= %d dest= \"@DEST@TA:%s\"", call->id, destIpStr);
    else
        epUtilsEvent(call->ep, "Rec", NULL, "makeCall entry= %d", call->id);

    while ((status >= 0) && (destIpStr != NULL) && (strlen(destIpStr) > 0))
    {
        cmTransportAddress addr;

        /* now parse the address, in case Amit decided to use ETA: */
        RvBool ta = (strncmp("TA:", destIpStr, 3)==0);
        RvBool eta = (strncmp("ETA:", destIpStr, 4)==0);

        /* move the pointer to the address itself, if needed */
        if (ta)
            destIpStr += 3;
        if (eta)
            destIpStr += 4;

        /* get the TA */
        status = cmString2TransportAddress(destIpStr, &addr);

        if (status >= 0)
        {
            lepUnlockCall(call);
            /* set the param */
            if (eta)
            {
                /* Set the destAnnexEAddress */
                status = cmCallSetParam(hsCall, cmParamDestinationAnnexEAddress, 0, sizeof(addr), (char*)&addr);
            }
            else
            {
                /* Set the destCallSignalAddress */
                status = cmCallSetParam(hsCall, cmParamDestCallSignalAddress, 0, sizeof(addr), (char*)&addr);
                status = cmCallSetParam(hsCall, cmParamDestinationIpAddress, 0, sizeof(addr), (char*)&addr);
            }
            lepLockCall(call);
        }
        /* see if there are more addresses */
        if ((destIpStr = (char*)strchr(destIpStr, (int)',')) != NULL)
            destIpStr++;
    }

    if (status >= 0)
    {
        /* Dial out */
        lepUnlockCallForAPI(call, status=cmCallDial(hsCall));
        if (status > 0)
            status = RV_OK;
    }

    lepUnlockCall(call);
    return status;
}


/******************************************************************************
 * epCallMake
 * ----------------------------------------------------------------------------
 * General: Send a setup message, starting an outgoing call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         to           - Call's destination string to use.
 *         bandwidth    - Bandwidth for this call.
 *         display      - Display information. Can be NULL.
 *         userUser     - User user information (only printable string is
 *                        supported here). Can be NULL.
 *         nsd          - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallMake(
    IN EpCallObj            *call,
    IN const RvChar         *to,
    IN RvInt                bandwidth,
    IN const RvChar         *display,
    IN const RvChar         *userUser,
    IN cmNonStandardParam   *nsd)
{
    RvStatus status;
    RvInt userUserLength;
    HCALL hsCall = call->hsCall;

    lepLockCall(call);

    if (to == NULL)
    {
        epUtilsEvent(call->ep, "Rec", NULL, "makeCall entry= %d", call->id);

        /* dial */
        lepUnlockCall(call);
        status=cmCallDial(hsCall);
    }
    else
    {
        epUtilsEvent(call->ep, "Rec", NULL, "makeCall entry= %d to= \"@DEST@%s\" bw= %d display= \"%s\" userUser= \"%s\"",
            call->id, to, bandwidth, (display != NULL) ? display : "", (userUser != NULL) ? userUser : "");

        /* make */
        if (nsd != NULL)
        {
            lepUnlockCallForAPI(call, status=cmCallSetParam(hsCall, cmParamSetupNonStandard, 0,
                sizeof(*nsd), (RvChar *)nsd));
            if (status < 0)
            {
                epUtilsError(call->ep, call, "Unable to set non standard parameter");
                lepUnlockCall(call);
                return status;
            }
        }

        if (userUser != NULL)
            userUserLength = (RvInt)strlen(userUser);
        else
            userUserLength = 0;

        lepUnlockCall(call);
        status=cmCallMake(hsCall, (RvUint32)bandwidth, 0, to, NULL,
            display, userUser, userUserLength);
    }

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallProceeding
 * ----------------------------------------------------------------------------
 * General: Send a call proceeding message on an incoming call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call handle to use.
 *         nsd      - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallProceeding(
    IN EpCallObj            *call,
    IN cmNonStandardParam   *nsd)
{
    RvStatus status;

    if (nsd != NULL)
    {
        status = cmCallSetParam(call->hsCall, cmParamCallProceedingNonStandard, 0, sizeof(*nsd), (RvChar *)nsd);
        if (status < 0)
        {
            epUtilsError(call->ep, call, "Unable to set non standard parameter");
            return status;
        }
    }

    status = cmCallSendCallProceeding(call->hsCall);
    epUtilsLog(call->ep, call, "Proceeding. result=%s", Status2String(status));

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallAlerting
 * ----------------------------------------------------------------------------
 * General: Send an alerting message on an incoming call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call handle to use.
 *         nsd      - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallAlerting(
    IN EpCallObj            *call,
    IN cmNonStandardParam   *nsd)
{
    RvStatus status;

    if (nsd != NULL)
    {
        status = cmCallSetParam(call->hsCall, cmParamAlertingNonStandard, 0, sizeof(*nsd), (RvChar *)nsd);
        if (status < 0)
        {
            epUtilsError(call->ep, call, "Unable to set non standard parameter");
            return status;
        }
    }

    lepLockCall(call);
    epH450CallSendNameID(call, alerting);

    status = cmCallAccept(call->hsCall);
#ifdef USE_H450
    if ((status >= 0) && call->ep->cfg.bH4505_alertingPrk)
        epH450GroupNotification(call, call->id, RV_TRUE);
#endif
    epUtilsLog(call->ep, call, "Alerting. result=%s", Status2String(status));

    if (status >= 0)
        status = RV_OK;
    lepUnlockCall(call);
    return status;
}


/******************************************************************************
 * epCallConnect
 * ----------------------------------------------------------------------------
 * General: Send an alerting message on an incoming call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         display      - Display information.
 *         userUser     - User user information (only printable string is
 *                        supported here).
 *         nsd          - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallConnect(
    IN EpCallObj            *call,
    IN const RvChar         *display,
    IN const RvChar         *userUser,
    IN cmNonStandardParam   *nsd)
{
    RvStatus status;
    int userUserSize;
    HCALL hsCall = call->hsCall;

    lepLockCall(call);
    if (nsd != NULL)
    {
        lepUnlockCallForAPI(call, status=cmCallSetParam(hsCall, cmParamConnectNonStandard, 0,
            sizeof(*nsd), (RvChar *)nsd));
        if (status < 0)
        {
            epUtilsError(call->ep, call, "Unable to set non standard parameter");
            return status;
        }
    }

    /* Send connect message */
    epH450CallSendNameID(call, connected);

    if (userUser == NULL)
        userUserSize = 0;
    else
        userUserSize = (int)strlen(userUser);
    lepUnlockCallForAPI(call, status=cmCallAnswerExt(hsCall, display, userUser, userUserSize));

    epUtilsLog(call->ep, call, "Connect. result=%s", Status2String(status));

    if (status >= 0)
        status = RV_OK;
    lepUnlockCall(call);
    return status;
}


/******************************************************************************
 * epCallProgress
 * ----------------------------------------------------------------------------
 * General: Send a progress message on an incoming call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         display      - Display information.
 *         nsd          - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallProgress(
    IN EpCallObj            *call,
    IN const RvChar         *display,
    IN cmNonStandardParam   *nsd)
{
    RvStatus status;
    RvPvtNodeId nodeId;
    HCALL hsCall = call->hsCall;

    /* Create Progress message */
    nodeId = cmCallProgressCreate(hsCall, display, -1, use30asPI_IE, -1, -1, nsd);

    /* Send Progress message */
    status = cmCallProgress(hsCall, nodeId);
    if (status < 0)
        pvtDelete(call->ep->hVal, nodeId);

    epUtilsLog(call->ep, call, "Progress. result=%s", Status2String(status));

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallNotify
 * ----------------------------------------------------------------------------
 * General: Send a notify message on a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         description  - Notification description to use.
 *         display      - Display information.
 *         nsd          - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallNotify(
    IN EpCallObj            *call,
    IN RvInt                description,
    IN const RvChar         *display,
    IN cmNonStandardParam   *nsd)
{
    RvStatus status;
    RvPvtNodeId nodeId;
    HCALL hsCall = call->hsCall;

    /* Create Notify message */
    nodeId = cmCallNotifyCreate(hsCall, display, description, nsd);

    /* Send Notify message */
    status = cmCallNotify(hsCall, nodeId);
    if (status < 0)
        pvtDelete(call->ep->hVal, nodeId);

    epUtilsLog(call->ep, call, "Notify. result=%s", Status2String(status));

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallUserInformation
 * ----------------------------------------------------------------------------
 * General: Send a user information message on an incoming call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         display      - Display information.
 *         nsd          - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallUserInformation(
    IN EpCallObj            *call,
    IN const RvChar         *display,
    IN cmNonStandardParam   *nsd)
{
    RvPvtNodeId nodeId;
    RvStatus status;
    HCALL hsCall = call->hsCall;

    /* Create Notify message */
    nodeId = cmCallUserInformationCreate(hsCall,  display, nsd);

    /* Send Notify message */
    status = cmCallUserInformationSend(hsCall, nodeId);
    if ((status == RV_ERROR_NULLPTR) || (status == RV_ERROR_DESTRUCTED))
        pvtDelete(call->ep->hVal, nodeId);

    epUtilsLog(call->ep, call, "UserInfo. result=%s", Status2String(status));

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallStatusEnquiry
 * ----------------------------------------------------------------------------
 * General: Send a status enquiry message on a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         display      - Display information.
 * Output: None.
 *****************************************************************************/
RvStatus epCallStatusEnquiry(
    IN EpCallObj            *call,
    IN const RvChar         *display)
{
    RvStatus status;

    /* Send Status Inquiry message */
    status = cmCallStatusEnquiry(call->hsCall, (RvUint8 *)display);

    epUtilsLog(call->ep, call, "StatusInquiry. result=%s", Status2String(status));

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallFacility
 * ----------------------------------------------------------------------------
 * General: Send a facility message on a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         reason       - Facility reason. Applicable when fType==EpCallFacilityReason.
 *         fwd          - Forwarding address. If exists, then a forwarding
 *                        facility is sent (and the nsd parameter is ignored).
 *         nsd          - Non standard data to use. Can be NULL.
 *         fType        - Type of facility message to send.
 * Output: None.
 *****************************************************************************/
RvStatus epCallFacility(
    IN EpCallObj            *call,
    IN cmReasonType         reason,
    IN const RvChar         *fwd,
    IN cmNonStandardParam   *nsd,
    IN EpCallFacilityType   fType)
{
    RvStatus status;
    HCALL hsCall = call->hsCall;
    HPVT hVal = call->ep->hVal;;

    if (fType == EpCallFacilityForward)
    {
        /* Forwarding facility */
        status = cmCallForward(hsCall, fwd);
    }
    else if (fType == EpCallFacilityDefault)
    {
        /* delete the old facility message - it might be from an incoming message... */
        RvPvtNodeId delNode = cmCallGetMessage(hsCall, cmQ931facility);
        pvtDelete(hVal, delNode);

        status = cmCallFacility(hsCall, RV_PVT_INVALID_NODEID);
    }
    else
    {
        RvPvtNodeId nodeId;
        RvPvtNodeId uuNodeId;

        nodeId = pvtAddRoot(hVal, cmGetSynTreeByRootName(call->ep->hApp, "q931"), 0, NULL);
        pvtBuildByPath(hVal, nodeId, "protocolDiscriminator", 8, NULL);
        uuNodeId = pvtBuildByPath(hVal, nodeId, "message.facility.userUser", 0, NULL);
        pvtBuildByPath(hVal, uuNodeId, "protocolDiscriminator", 5, NULL);

        if (fType == EpCallFacilityReason)
        {
            RvPvtNodeId tmpNodeId;
            char   oid[128];
            int    oidSize;

            oidSize = utlEncodeOID((int)sizeof(oid), oid, "itu-t recommendation h 2250 0 5");

            uuNodeId = pvtBuildByPath(hVal, uuNodeId, "h323-UserInformation.h323-uu-pdu", 0, NULL);
            tmpNodeId = pvtBuildByPath(hVal, uuNodeId, "h323-message-body.facility.protocolIdentifier", oidSize, oid);

            cmCallSetParam(hsCall, cmParamFacilityReason, 0, (RvInt32)reason, NULL);

            if (nsd != NULL)
            {
                tmpNodeId = pvtBuildByPath(hVal, uuNodeId, "nonStandardData", 0, NULL);
                cmNonStandardParameterCreate(hVal, tmpNodeId, &nsd->info, nsd->data, nsd->length);
            }
        }
        if (fType == EpCallFacilityEmpty)
        {
            pvtBuildByPath(hVal, uuNodeId, "h323-UserInformation.h323-uu-pdu.h323-message-body.empty", 0, NULL);
        }

        /* send the facility message */
        status = cmCallFacility(hsCall, nodeId);
        pvtDelete(hVal, nodeId);
    }

    epUtilsLog(call->ep, call, "Facility. result=%s", Status2String(status));

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallDrop
 * ----------------------------------------------------------------------------
 * General: Drop a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call handle to use.
 *         reason   - Drop reason to use. -1 if we don't want to set it.
 *         nsd      - Non standard data to use. Can be NULL.
 * Output: None.
 *****************************************************************************/
RvStatus epCallDrop(
    IN EpCallObj            *call,
    IN cmReasonType         reason,
    IN cmNonStandardParam   *nsd)
{
    RvStatus status;
    int hadControl;
    int result;
    HCALL hsCall = call->hsCall;

    lepLockCall(call);

    if (nsd != NULL)
    {
        lepUnlockCallForAPI(call, status=cmCallSetParam(hsCall, cmParamReleaseCompleteNonStandard,
            0, sizeof(*nsd), (RvChar *)nsd));
        if (status < 0)
        {
            epUtilsError(call->ep, call, "Unable to set non standard parameter");
            return status;
        }
    }

    if (reason == cmReasonTypeAdaptiveBusy)
    {
        epH450CallSendNameID(call, busy);
    }

    lepUnlockCallForAPI(call, hadControl=cmCallHasControlSession(hsCall));

    /* Signal to the channel Idle procedure not the reprint the channels */
    call->numOfChannels = -1;

#ifdef USE_PLAYER
    PlayerPlayFinish (call);
#endif

    /* Drop call from the stack with the given reason */
    if (reason != (cmReasonType)-1)
    {
        epUtilsLog(call->ep, call, "Dropping. reason=%s", CMReasonType2String(reason));
        lepUnlockCallForAPI(call, result=cmCallDropParam(hsCall, reason));
    }
    else
    {
        lepUnlockCallForAPI(call, result=cmCallDrop(call->hsCall));
    }

    if (result >= 0)
    {
        /* This part is not really used. It is here as counting purposes */
        epUtilsEvent(call->ep, "Catch", call, "incr tmp(record,cnt,releaseComplete,min)");
        if (hadControl)
        {
            epUtilsEvent(call->ep, "Catch", call, "incr tmp(record,cnt,endSessionCommand,min)");
        }
    }

    lepUnlockCall(call);
    return RV_OK;
}


/******************************************************************************
 * epCallDropAll
 * ----------------------------------------------------------------------------
 * General: Drop all calls.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep       - Endpoint object to use.
 *         reason   - Drop reason to use. -1 if we don't want to set it.
 *         waitDrop - RV_TRUE if we want this function to return only after all
 *                    calls have been dropped.
 * Output: None.
 *****************************************************************************/
RvStatus epCallDropAll(
    IN EpObj                *ep,
    IN cmReasonType         reason,
    IN RvBool               waitDrop)
{
    /* Ignore if we already have a timer running */
    if (ep->dropAllTimer != HTI_NULL_TIMER)
        return RV_OK;

    ep->dropAllReason = reason;
    ep->dropAllTimer = mtimerSet(ep->hTimers, dropSingleCall, ep, 10);
    if (ep->dropAllTimer == HTI_NULL_TIMER)
        return RV_ERROR_UNKNOWN;

    if (waitDrop)
    {
        while (ep->dropAllTimer != HTI_NULL_TIMER)
            seliSelect();
    }

    return RV_OK;
}


/******************************************************************************
 * epCallOverlapSending
 * ----------------------------------------------------------------------------
 * General: Send indication of overlap sending.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 *         to               - Destination string to use from the call's origin.
 *                            If NULL, then an indication from the destination
 *                            if the address is complete will be given.
 *         sendingComplete  - 0,1 for indication of address completion.
 *                            -1 for no indication at all.
 * Output: None.
 *****************************************************************************/
RvStatus epCallOverlapSending(
    IN EpCallObj    *call,
    IN const RvChar *to,
    IN RvInt        sendingComplete)
{
    RvStatus status;
    HCALL hsCall = call->hsCall;

    lepLockCall(call);

    if (to == NULL)
    {
        if (sendingComplete)
        {
            lepUnlockCallForAPI(call, status=cmCallAddressComplete(hsCall));
        }
        else
        {
            lepUnlockCallForAPI(call, status=cmCallIncompleteAddress(hsCall));
        }
    }
    else
    {
        if (sendingComplete >= 0)
        {
            lepUnlockCallForAPI(call, status=cmCallOverlapSendingExt(hsCall, to, (sendingComplete != 0)));
        }
        else
        {
            lepUnlockCallForAPI(call, status=cmCallOverlapSending(hsCall, to));
        }
    }

    if (status > 0)
        status = RV_OK;
    lepUnlockCall(call);
    return status;
}


/******************************************************************************
 * epCallGenerateFaststartRequest
 * ----------------------------------------------------------------------------
 * General: Generate an automatic faststart proposal.
 *          This creates 2 channel proposals for an incoming and an outgoing
 *          audio channel for static audio.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         staticAudio  - RV_TRUE to generate only a static audio proposal.
 *                        RV_FALSE to use whatever we have in the configuration.
 * Output: None.
 *****************************************************************************/
RvStatus epCallGenerateFaststartRequest(
    IN EpCallObj *call,
    IN RvBool    staticAudio)
{
    RvStatus status = RV_OK;
    HCALL hsCall = call->hsCall;

    lepLockCall(call);

    /* Prepare media RTP for faststart proposals */
    call->isFastConnectCall = RV_TRUE;
    if (call->action == epRtpActionReplay)
    {
        /* init all the rtp sessions, we may need them later */
        epCallInitRtpSessions(call, NULL, NULL, NULL);
    }

    epUtilsEvent(call->ep, "Rec", NULL, "faststartAutoAdd entry= %d onlyAudio= %d", call->id, staticAudio);

    if (staticAudio)
    {
        /* Note that this part shows how to use the OLDER faststart API functions */
        cmFastStartMessage fsMessage;
        RvUint16 audioPort;

        if (call->action == epRtpActionReplay)
        {
            audioPort = epRtpSessionGetLocalPort(call->ep->rtp, call->rtpSessions[0]);
        }
        else
            audioPort = 2536;

        memset(&fsMessage.partnerChannels[0], 0, sizeof(fsMessage.partnerChannels[0]));

        fsMessage.partnerChannelsNum = 1;

        fsMessage.partnerChannels[0].receive.altChannelNumber = 1;
        fsMessage.partnerChannels[0].receive.channels[0].rtp.port= audioPort;
        fsMessage.partnerChannels[0].receive.channels[0].rtcp.port= ++audioPort;
        fsMessage.partnerChannels[0].receive.channels[0].dataTypeHandle = RV_PVT_INVALID_NODEID;
        fsMessage.partnerChannels[0].receive.channels[0].channelName = (char *)"g711Alaw64k";

        fsMessage.partnerChannels[0].transmit.altChannelNumber = 1;
        fsMessage.partnerChannels[0].transmit.channels[0].rtp.port= 0;
        fsMessage.partnerChannels[0].transmit.channels[0].rtcp.port= audioPort;
        fsMessage.partnerChannels[0].transmit.channels[0].dataTypeHandle = RV_PVT_INVALID_NODEID;
        fsMessage.partnerChannels[0].transmit.channels[0].channelName = (char *)"g711Alaw64k";

        fsMessage.partnerChannels[0].type = cmCapAudio;

        /* Opens Fast Start channels */
        lepUnlockCallForAPI(call, status=cmFastStartOpenChannels(hsCall, &fsMessage));
    }
    else
    {
        /* This part shows the newer faststart API functions */
        HPVT hVal;
        cmFastStartChannel fsChannel;
        RvPvtNodeId nameNodeId;
        RvPvtNodeId fsNodeId;
        RvChar channelName[130];
        int sessionId;
        int rtpPort;
        RvBool bInfo = RV_FALSE;

        RV_UNUSED_ARG(bInfo);

        hVal = call->ep->hVal;
        nameNodeId = pvtChild(hVal, pvtGetChild(hVal, cmGetH245ConfigurationHandle(call->ep->hApp), __h245(channels), NULL));

        while (RV_PVT_NODEID_IS_VALID(nameNodeId))
        {
            /* Get all channels of a given type */
            pvtGetString(hVal, pvtGetChild(hVal, nameNodeId, __h245(name), NULL), (RvInt32)sizeof(channelName), channelName);

            memset(&fsChannel, 0, sizeof(fsChannel));
            fsChannel.channelName = channelName;
            fsChannel.dataTypeHandle = RV_PVT_INVALID_NODEID;

            switch (tolower(channelName[0]))
            {
            case 'g':
                sessionId = (int)cmCapAudio;
                rtpPort = epRtpSessionGetLocalPort(call->ep->rtp, call->rtpSessions[0]);
                break;
            case 'h':
                sessionId = (int)cmCapVideo;
                rtpPort = epRtpSessionGetLocalPort(call->ep->rtp, call->rtpSessions[1]);
                break;
            case 't':
                sessionId = (int)cmCapData;
                rtpPort = epRtpSessionGetLocalPort(call->ep->rtp, call->rtpSessions[2]);
                break;
            default:
                sessionId = (int)cmCapEmpty;
                rtpPort = 0;
                break;
            }
            if (rtpPort == 0)
                rtpPort = 2536;
            if (sessionId != (int)cmCapData)
            {
                fsChannel.rtcp.port = (RvUint16)(rtpPort + 1);
                lepUnlockCallForAPI(call, fsNodeId=cmFastStartBuild(hsCall, sessionId, dirTransmit, &fsChannel));
                if (!RV_PVT_NODEID_IS_VALID(fsNodeId))
                {
                    lepUnlockCall(call);
                    return fsNodeId;
                }
                /*Add H.460.19 information to fsNodeId*/
                status = epFwNatAddH46019Info2Message(call, NULL, RV_FALSE, &fsChannel, fsNodeId, RV_TRUE, dirTransmit);
                lepUnlockCallForAPI(call, status=cmCallAddFastStartMessage(hsCall, fsNodeId));
                if (status < 0)
                {
                    pvtDelete(hVal, fsNodeId);
                    lepUnlockCall(call);
                    return status;
                }

                fsChannel.rtp.port= (RvUint16)rtpPort;
                lepUnlockCallForAPI(call, fsNodeId=cmFastStartBuild(hsCall, sessionId, dirReceive, &fsChannel));
                if (!RV_PVT_NODEID_IS_VALID(fsNodeId))
                {
                    lepUnlockCall(call);
                    return fsNodeId;
                }
                /*Add H.460.19 information to fsNodeId*/
                status = epFwNatAddH46019Info2Message(call, NULL, RV_FALSE, &fsChannel, fsNodeId, RV_TRUE, dirReceive);
                lepUnlockCallForAPI(call, status=cmCallAddFastStartMessage(hsCall, fsNodeId));
                if (status < 0)
                {
                    pvtDelete(hVal, fsNodeId);
                    lepUnlockCall(call);
                    return status;
                }
            }
            else
            {
                fsChannel.rtp.port= (RvUint16)rtpPort;
                fsChannel.rtcp.port= (RvUint16)(rtpPort + 1);
                lepUnlockCallForAPI(call, fsNodeId=cmFastStartBuild(hsCall, sessionId, dirBoth, &fsChannel));
                if (!RV_PVT_NODEID_IS_VALID(fsNodeId))
                {
                    lepUnlockCall(call);
                    return fsNodeId;
                }
                /*Add H.460.19 information to fsNodeId*/
                status = epFwNatAddH46019Info2Message(call, NULL, RV_FALSE, &fsChannel, fsNodeId, RV_TRUE, dirTransmit);
                lepUnlockCallForAPI(call, status=cmCallAddFastStartMessage(hsCall, fsNodeId));
                if (status < 0)
                {
                    pvtDelete(hVal, fsNodeId);
                    lepUnlockCall(call);
                    return status;
                }
            }

            nameNodeId = pvtBrother(hVal, nameNodeId);
        }
    }

    if (status > 0)
        status = RV_OK;
    lepUnlockCall(call);
    return status;
}


/******************************************************************************
 * epCallFaststartAdd
 * ----------------------------------------------------------------------------
 * General: Add a faststart proposal to a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 *         sessionId        - Session id for the channel's proposal.
 *         channelDirection - Direction of the proposed channel.
 *         bAccept          - True if this is a reconfiguration channel
 *         channelName      - Name of the channel in configuration (can be NULL).
 *         dataTypeNodeId   - Node id to used instead of a channelName.
 *         rtp              - RTP address to use (can be NULL for outgoing
 *                            channels).
 *         rtcp             - RTCP address to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallFaststartAdd(
    IN EpCallObj            *call,
    IN RvInt32              sessionId,
    IN cmChannelDirection   channelDirection,
    IN RvBool               bAccept,
    IN const RvChar         *channelName,
    IN RvPvtNodeId          dataTypeNodeId,
    IN cmTransportAddress   *rtp,
    IN cmTransportAddress   *rtcp)
{
    cmFastStartChannel fsChannel;
    RvPvtNodeId nodeId, tmpNode;
    RvStatus status;
    RvChar rtpStr[200];
    RvChar rtcpStr[200];
    RvBool bNull, bInfo = RV_FALSE;
    HCALL hsCall = call->hsCall;
    cmChannelDirection  tmpChanDir = channelDirection;

    RV_UNUSED_ARG(bInfo);

    lepLockCall(call);

    rtpStr[0] = '\0';
    rtcpStr[0] = '\0';
    if (rtp != NULL)
    {
        strcpy(rtpStr, " rtp= \"");
        cmTransportAddress2String(rtp, sizeof(rtpStr)-10, rtpStr+7);
        strcat(rtpStr, "\"");
    }
    if (rtcp != NULL)
    {
        strcpy(rtcpStr, " rtcp= \"");
        cmTransportAddress2String(rtcp, sizeof(rtcpStr)-10, rtcpStr+8);
        strcat(rtcpStr, "\"");
    }
    epUtilsEvent(call->ep, "Rec", NULL, "faststartAdd entry= %d sid= %d direction= %d accept= %d name= \"%s\"%s%s",
        call->id, sessionId, channelDirection, bAccept, channelName, rtpStr, rtcpStr);

    /* Prepare media RTP for faststart proposals */
    call->isFastConnectCall = RV_TRUE;
    if (call->action == epRtpActionReplay)
    {
        /* init all the rtp sessions, we may need them later */
        epCallInitRtpSessions(call, NULL, NULL, NULL);
    }

    if ((call->action == epRtpActionReplay) && (sessionId >= 1) && (sessionId <= 3))
    {
        /* Replace the suggested addresses given to this function with those we're really going to work with */
        cmTransportAddress addr;
        memset(&addr, 0, sizeof(addr));
        cmProtocolGetLocalAddress(call->ep->hApp, cmGetTpktChanHandle(call->hsCall, cmQ931TpktChannel), &addr);
        addr.port = epRtpSessionGetLocalPort(call->ep->rtp, call->rtpSessions[sessionId-1]);
        if (rtp != NULL)
            *rtp = addr;
        if (rtcp != NULL)
        {
            *rtcp = addr;
            rtcp->port++;
        }
    }

    if ((channelName == NULL) || (strcmp(channelName, "nullData") != 0))
    {
        memset(&fsChannel, 0, sizeof(fsChannel));
        fsChannel.channelName = (RvChar *)channelName;
        fsChannel.dataTypeHandle = dataTypeNodeId;
        if (rtp != NULL)
            fsChannel.rtp = *rtp;
        if (rtcp != NULL)
            fsChannel.rtcp = *rtcp;

        bNull = RV_FALSE;
    }
    else
        bNull = RV_TRUE;

    if (!bAccept && bNull)
    {
        channelDirection = dirTransmit;
        tmpChanDir = channelDirection;
    }

    if (bAccept)
    {
        nodeId = cmExtFastStartBuildAck(call->hsCall, sessionId, channelDirection, bNull ? NULL : &fsChannel);
    }
    else
    {
        nodeId = cmFastStartBuild(call->hsCall, sessionId, channelDirection, bNull ? NULL : &fsChannel);
    }

    if (!RV_PVT_NODEID_IS_VALID(nodeId))
    {
        lepUnlockCall(call);
        return nodeId;
    }

    /* Add H.460.19 information to nodeId */
    if (channelDirection == dirBoth)
        tmpChanDir = dirTransmit;
    status = epFwNatAddH46019Info2Message(call, NULL, RV_FALSE, &fsChannel, nodeId, RV_TRUE, tmpChanDir);

    if (!bAccept)
    {
        lepUnlockCallForAPI(call, status=cmCallAddFastStartMessage(hsCall, nodeId));
    }
    else
    {
        lepUnlockCallForAPI(call, status=cmCallAddExtFastConnectApproval(hsCall, nodeId));
        if (status >= 0)
        {
            EpChannelObj * channel;
            HCHAN hsChan;

            lepUnlockCallForAPI(call, cmCallGetChannelBySessionId(hsCall,
                sessionId, channelDirection != dirReceive, &hsChan));
            if (channelDirection == dirBoth)
            {
                if (hsChan == NULL)
                {
                    /* try the other way */
                    lepUnlockCallForAPI(call, cmCallGetChannelBySessionId(hsCall,
                        sessionId, RV_FALSE, &hsChan));
                }
                if (!cmChannelIsDuplex(hsChan))
                    hsChan = NULL;
            }

            if (hsChan != NULL)
            {
                cmChannelGetHandle(hsChan, (HAPPCHAN*)&channel);
                if (bNull)
                {
                    strcpy(channel->dataTypeName, "<PAUSED>");
                }
                else
                {
                    if (strcmp(channel->dataTypeName, channelName) != 0)
                    {
                        /* this is a replacement channel. mark it as such */
                        if (channelDirection != dirTransmit)
                        {
                            __pvtBuildByFieldIds(tmpNode, call->ep->hVal, nodeId,
                                { _h245(forwardLogicalChannelParameters) _h245(replacementFor) LAST_TOKEN },
                                cmChannelGetNumber(hsChan), NULL);
                        }
                        else
                        {
                            __pvtBuildByFieldIds(tmpNode, call->ep->hVal, nodeId,
                                { _h245(reverseLogicalChannelParameters) _h245(replacementFor) LAST_TOKEN },
                                cmChannelGetNumber(hsChan), NULL);
                        }
                        strcpy(channel->dataTypeName, channelName);
                    }
                }
                /* Redisplay the list of channels */
                epUtilsEvent(call->ep, "chanStateDisplay", call, "");
            }
        }
    }

    if (status > 0)
        status = RV_OK;
    else if (status < 0)
        pvtDelete(call->ep->hVal, nodeId);

    lepUnlockCall(call);
    return status;
}


/******************************************************************************
 * epCallFaststartAckIndex
 * ----------------------------------------------------------------------------
 * General: Acknowledge a faststart proposal.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 *         index            - Index of proposal to acknowledge.
 *         rtp              - RTP address to use (can be NULL for incoming
 *                            channels).
 *         rtcp             - RTCP address to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallFaststartAckIndex(
    IN EpCallObj            *call,
    IN RvInt32              index,
    IN cmTransportAddress   *rtp,
    IN cmTransportAddress   *rtcp)
{
    RvStatus                    status = RV_ERROR_UNKNOWN;
    int                         sessionId;
    cmChannelDirection          chanDir;
    cmFastStartChannel          fsChan;
    RvPvtNodeId                 recNode, ackNode;
    RvBool                      bInfo = RV_FALSE;

    RV_UNUSED_ARG(bInfo);

    epUtilsEvent(call->ep, "Rec", NULL, "faststartAckIndex entry= %d index= %d", call->id, index);

    ackNode = cmFastStartChannelsAckIndex(call->hsCall, index, rtp, rtcp);
    if (ackNode >= 0)
    {
#ifdef USE_FWNAT
        RvBool bKeepAliveNeeded = RV_FALSE;
#endif
        /*get proposal nodeId*/
        recNode = cmFastStartGetByIndex(call->hsCall, index);
        /*get FS channel info*/
        cmFastStartGet(call->hsCall, recNode, &sessionId, &chanDir, &fsChan);
#ifdef USE_FWNAT
        lepLockCall(call);
        if (chanDir == dirBoth)
        {
            chanDir = dirReceive;
            /*if it's a server - add a keep alive in the traversalParams of the ack*/
            if (call->ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigServer)
            {
                bKeepAliveNeeded = RV_TRUE;
            }
        }
        /*get proposed channel multiplexing info*/
        epFwNatGetH46019InfoFromMessage(call, NULL, RV_FALSE, recNode, &fsChan, sessionId, RV_TRUE, chanDir);
        /*set multiplexing info in message*/
        epFwNatAddH46019Info2Message(call, NULL, bKeepAliveNeeded, &fsChan, ackNode, RV_TRUE, chanDir);
        lepUnlockCall(call);
#endif
        status = RV_OK;
    }
    return status;
}


/******************************************************************************
 * epCallFaststartReady
 * ----------------------------------------------------------------------------
 * General: Indicate that all faststart proposals have been processed.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 *         bAccept          - True if we are sending an acceptance.
 * Output: None.
 *****************************************************************************/
RvStatus epCallFaststartReady(
    IN EpCallObj            *call,
    IN RvBool               bAccept)
{
    RvStatus status = RV_OK;
    cmExtendedFastConnectState_e efcState;
    HCALL hsCall = call->hsCall;

    epUtilsEvent(call->ep, "Rec", NULL, "faststartReady entry= %d accept= %d", call->id, bAccept);

    if (bAccept)
        status = cmFastStartChannelsReady(hsCall);

    /* check if we are in EFC mode, and if the call is connected */
    cmCallGetParam(hsCall, cmParamExtendedFastConnectState, 0, (RvInt32*)&efcState, NULL);
    if (efcState == cmExtendedFastConnectApproved)
    {
        if (call->state == cmCallStateOffering)
        {
            /* make sure call proceeding was already sent */
            if (cmCallSendCallProceeding(call->hsCall) == RV_OK)
            {
                /* we sent CP, it took the EFC messages with it, no need to send
                   a facility message now */
                return RV_OK;
            }
            /* CP was not sent, because the call state has passed that point.
               that means it's ok to send a facility message. */
        }
        /* send a facility message */
        epCallFacility(call, cmReasonTypeTransportedInformation, NULL, NULL, EpCallFacilityReason);
    }

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallFaststartRefuse
 * ----------------------------------------------------------------------------
 * General: Refuse incoming faststart proposals.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallFaststartRefuse(
    IN EpCallObj            *call)
{
    RvStatus status;
    cmExtendedFastConnectState_e efcState;
    HCALL hsCall = call->hsCall;

    epUtilsEvent(call->ep, "Rec", NULL, "faststartRefuse entry= %d", call->id);

    status = cmFastStartChannelsRefuse(hsCall);

    /* check if we are in EFC mode, and if the call is connected */
    cmCallGetParam(hsCall, cmParamExtendedFastConnectState, 0, (RvInt32*)&efcState, NULL);
    if (efcState == cmExtendedFastConnectApproved)
    {
        /* make sure call proceeding was already sent */
        if (cmCallSendCallProceeding(call->hsCall) == RV_OK)
        {
            /* we sent CP, it took the EFC messages with it, no need to send
               a facility message now */
            return RV_OK;
        }
        /* CP was not sent, because the call state has passed that point.
           that means it's ok to send a facility message. */
        epCallFacility(call, cmReasonTypeTransportedInformation, NULL, NULL, EpCallFacilityReason);
    }

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallExtFastConnectCloseAllChannels
 * ----------------------------------------------------------------------------
 * General: Close all fast-connect channels.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallExtFastConnectCloseAllChannels(
    IN EpCallObj            *call)
{
    RvStatus status;

    epUtilsEvent(call->ep, "Rec", NULL, "faststartDropAll entry= %d", call->id);

    status = cmCallExtFastConnectCloseAllChannels(call->hsCall);
    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallFaststartAutoAck
 * ----------------------------------------------------------------------------
 * General: Automatically acknowledge feasible proposals.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallFaststartAutoAck(
    IN EpCallObj            *call)
{
    EpObj                       *ep = call->ep;
    int                         i;
    cmTransportAddress          ownCSAddr;
    int                         channelId = 0;
    int                         sessionId;
    cmChannelDirection          chanDir, tmpChanDir;
    cmFastStartChannel          fsChan;
    RvBool                      channels[512];
    RvStatus                    status;
    HCALL                       hsCall = call->hsCall;

    lepLockCall(call);
    memset(channels, 0, sizeof(channels));

    cmProtocolGetLocalAddress(ep->hApp, cmGetTpktChanHandle(hsCall, cmQ931TpktChannel), &ownCSAddr);

    /* answer all channels automatically */
    for (i = 0; channelId >= 0; i++)
    {
        RvPvtNodeId ackChan;
        RvBool add;

        lepUnlockCallForAPI(call, channelId=cmFastStartGetByIndex(call->hsCall, i));
        if (channelId < 0)
            break; /* No more channel proposals */

        lepUnlockCallForAPI(call, cmFastStartGet(hsCall, channelId, &sessionId, &chanDir, &fsChan));
        if (chanDir == dirBoth)
            add = ((channels[sessionId] || channels[256+sessionId]) == RV_FALSE);
        else
            add = (channels[sessionId + ((chanDir==dirTransmit)?256:0)] == RV_FALSE);

        if (add)
        {
#ifdef USE_FWNAT
            RvBool bKeepAliveNeeded = RV_FALSE;
#endif
            tmpChanDir = chanDir;
            
            memcpy(&fsChan.rtcp, &ownCSAddr, sizeof(fsChan.rtcp));
            fsChan.rtcp.port = 0;

            if (chanDir == dirReceive)
                memcpy(&fsChan.rtp, &ownCSAddr, sizeof(fsChan.rtp));
            else
                memset(&fsChan.rtp, 0, sizeof(fsChan.rtp));
            fsChan.rtp.port = 0;

            lepUnlockCallForAPI(call, ackChan=cmFastStartChannelsAckIndex(hsCall, i, &fsChan.rtcp, &fsChan.rtp));
            if (ackChan < 0)
            {
                lepUnlockCall(call);
                return ackChan;
            }

            epSecurityChannelAddEncryptionSync(call, ackChan);
            if (tmpChanDir == dirBoth)
            {
                tmpChanDir = dirReceive;
#ifdef USE_FWNAT
                if (ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigServer)
                {
                    bKeepAliveNeeded = RV_TRUE;
                }
#endif
            }
            status = epFwNatGetH46019InfoFromMessage(call, NULL, RV_FALSE, channelId, &fsChan, sessionId, RV_TRUE, tmpChanDir);
            status = epFwNatAddH46019Info2Message(call, NULL, bKeepAliveNeeded, &fsChan, ackChan, RV_TRUE, tmpChanDir);

            channels[sessionId + ((chanDir==dirTransmit)?256:0)] = RV_TRUE;
            if (chanDir == dirBoth)
                channels[sessionId + 256] = RV_TRUE;
        }
    }

    lepUnlockCall(call);
    ep->bIgnoreRec = RV_TRUE;
    status = epCallFaststartReady(call, RV_TRUE);
    ep->bIgnoreRec = RV_FALSE;
    if (status > 0)
        status = RV_OK;

    return status;
}


/******************************************************************************
 * epCallInitRtpSessions
 * ----------------------------------------------------------------------------
 * General: Initiate call RTP and RTCP sessions.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 * Output: audioPort    - audio port used.
 *         videoPort    - video port used.
 *         dataPort     - data port used.
 *****************************************************************************/
void epCallInitRtpSessions(
    IN  EpCallObj           *call,
    OUT int                 *audioPort,
    OUT int                 *videoPort,
    OUT int                 *dataPort)
{
    RvBool isMultiplexed;
    RvUint32 muxId = (RvUint32)-1;

    RV_UNUSED_ARG(muxId);

    /* init all the rtp sessions, we may need them later. */
    if (call->rtpSessions[0] < 0)
        call->rtpSessions[0] = epRtpSessionNew(call, RV_FALSE, &muxId);
    if (call->rtpSessions[1] < 0)
        call->rtpSessions[1] = epRtpSessionNew(call, RV_FALSE, &muxId);
    if (call->rtpSessions[2] < 0)
        call->rtpSessions[2] = epRtpSessionNew(call, RV_FALSE, &muxId);

    /*open multiplexed sessions if we have to*/
    if ((isMultiplexed = epFwNatIsCallMultiplexed(call)) == RV_TRUE)
    {
        epFwNatCreateCallMuxedRtpSessions(call);
    }

    if (audioPort != NULL)
        *audioPort = epRtpSessionGetLocalPort(call->ep->rtp, call->rtpSessions[0]);
    if (videoPort != NULL)
        *videoPort = epRtpSessionGetLocalPort(call->ep->rtp, call->rtpSessions[1]);
    if (dataPort != NULL)
        *dataPort = epRtpSessionGetLocalPort(call->ep->rtp, call->rtpSessions[2]);
}


/******************************************************************************
 * epCallSendAnnexLMessage
 * ----------------------------------------------------------------------------
 * General: Send an annex L message on the call. will force a facility message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         str          - Annex L string.
 *         strLen       - Length of str.
 * Output: None.
 *****************************************************************************/
RvStatus RVCALLCONV epCallSendAnnexLMessage(
    IN      EpCallObj *         call,
    IN      const RvChar *      str,
    IN      int                 strLen)
{
    RvStatus ret;
    RvPvtNodeId msg = cmCallCreateAnnexLMessage(call->ep->hApp, RV_TRUE, str, strLen, NULL);
    ret = cmCallSendAnnexLMessage(call->hsCall, msg, RV_TRUE);
    if (ret < 0)
        pvtDelete(call->ep->hVal, msg);
    return ret;
}


int RVCALLCONV cmEvCallRecvMessage(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN int      message)
{
    EpCallObj *call = (EpCallObj *)haCall;
    EpObj *ep;
    RvChar *messageStr;
    RvChar *protocolStr;
    cmProtocol prot;
    RvBool  bInfo = RV_FALSE;
    RvStatus status;

    RV_UNUSED_ARG(bInfo);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsCall), (HAPPAPP*)&ep);

    messageStr = cmGetProtocolMessageName(ep->hApp, message);
    prot = cmProtocolGetProtocol(ep->hApp, message);
    protocolStr = cmProtocolGetProtocolName(prot);

    epUtilsEvent(ep, protocolStr, call, "in.%s %d", messageStr, message);
    epUtilsLog(ep, call, "New message recv <- %s on %s", messageStr, protocolStr);


    if (call != NULL)
    {
        lepLockCall(call);
        
        /*retrieve h46019 info from message*/
        status = epFwNatCallRecvMessage(ep, call, message, &bInfo);

        if (!call->separateH245 && (prot == cmProtocolH245))
        {
            /* Check if it's a separate connection */
            RvInt32 value = 1;
            RvBool hasControl;
            RvStatus res;

            lepUnlockCallForAPI(call, hasControl=cmCallHasControlSession(hsCall));
            lepUnlockCallForAPI(call, res=cmCallGetParam(hsCall, cmParamH245Tunneling, 0, &value, NULL));

            if (hasControl && (res >= 0) && (value == 0))
            {
                call->separateH245 = RV_TRUE;
                epUtilsEvent(ep, "Tag", call, "\"eventRemoval%d\" \"ByType\"", call->id);
                epUtilsEvent(ep, "Catch", call, "record:separateCall %d", call->id);
            }
        }
        lepUnlockCall(call);
    }

    ep->cb.epMessage(ep, call, message, cmGetProtocolMessageName(ep->hApp, message), RV_FALSE);

    return 0;
}


/******************************************************************************
 * epCallSetOptions
 * ----------------------------------------------------------------------------
 * General: Set general call options from a given configuration struct.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 *         cfg              - Configuration options to set.
 *                            Negative value parameters in this struct are
 *                            ignored.
 *                            Parameters are set if they are relevant for the
 *                            call's direction only.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSetOptions(
    IN EpCallObj            *call,
    IN EpCallConfig         *cfg)
{
    /* Set some default call parameters */
    if (cfg->h245Stage >= 0)
    {
        cmCallSetParam(call->hsCall, cmParamH245Stage, 0, cfg->h245Stage, NULL);
        epFwNatSetCallH245Stage(call, cfg);
    }
    if (cfg->tunneling >= 0)
        cmCallSetParam(call->hsCall, cmParamH245Tunneling, 0, cfg->tunneling, NULL);
    if (cfg->parellel >= 0)
        cmCallSetParam(call->hsCall, cmParamH245Parallel, 0, cfg->parellel, NULL);
    if (cfg->efc >= 0)
        cmCallSetParam(call->hsCall, cmParamExtendedFastConnectState, 0, cfg->efc, NULL);
    if (cfg->canOverlapSend >= 0)
        cmCallSetParam(call->hsCall, cmParamCanOverlapSending, 0, cfg->canOverlapSend, NULL);
    if (cfg->requestedBandwidth >= 0)
        cmCallSetParam(call->hsCall, cmParamRate, 0, cfg->requestedBandwidth, NULL);
    if (cfg->multiplexing >= 0)
        cmCallSetParam(call->hsCall, cmParamIsMultiplexed, 0, cfg->multiplexing, NULL);
    if (cfg->maintainConnection >= 0)
        cmCallSetParam(call->hsCall, cmParamShutdownEmptyConnection, 0, !cfg->maintainConnection, NULL);
    if (cfg->terminalType >= 0)
        cmCallSetParam(call->hsCall, cmParamMsdTerminalType, 0, cfg->terminalType, NULL);

    /* manual RAS in automatic mode */
    if (!call->ep->bIsManualRAS)
    {
        if (cfg->manualRasCalls >= 0)
            cmCallSetParam(call->hsCall, cmParamAutomaticRAS, 0, !cfg->manualRasCalls, NULL);
    }

    if (call->ep->cfg.bUseNatAddress)
    {
        cmCallSetParam(call->hsCall, cmParamSupportNat, 0, (RvUint32)RV_TRUE, NULL);
    }

    /* Only for outgoing calls */
    if (!call->isIncoming)
    {
        if (cfg->setupSendingComplete >= 0)
            cmCallSetParam(call->hsCall, cmParamSetupSendingComplete, 0, cfg->setupSendingComplete, NULL);
        if (cfg->hopCount > 0)
            cmCallSetParam(call->hsCall, cmParamHopCount, 0, cfg->hopCount, NULL);

        if (cfg->requestedBandwidth >= 0)
            cmCallSetParam(call->hsCall, cmParamRequestedRate, 0, cfg->requestedBandwidth, NULL);
        if (cfg->useMultirate >= 0)
            cmCallSetParam(call->hsCall, cmParamMultiRate, 0, cfg->useMultirate, NULL);
    }

    return RV_OK;
}


/******************************************************************************
 * epCallColorIt
 * ----------------------------------------------------------------------------
 * General: Calor a call by the call's conference id and connection handle.
 *          This helps when we want to display it to the GUI and see how calls
 *          relate to each other and on which connections are they being sent.
 *
 * Return Value: The id of the connection of the call.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call object to use.
 * Output: None.
 *****************************************************************************/
RvInt32 epCallColorIt(
    IN EpCallObj *call)
{
    HPROTCONN           hCon;
    EpConnectionInfo    *appCon;
    static RvUint32     randSeed = 0;
    static RvBool       firstEntry = RV_TRUE;

    if (firstEntry)
    {
        randSeed = timerGetTimeInMilliseconds();
        firstEntry = RV_FALSE;
    }

    /* Match the connection with a random number */
    hCon = cmGetTpktChanHandle(call->hsCall, cmQ931TpktChannel);
    if (hCon == NULL)
        return 0; /* not found */
    cmGetTpktChanApplHandle(hCon, (HAPPCONN*)&appCon);
    if (appCon == NULL)
        return -1; /* Not colored yet */

    if (call->hCon != hCon)
    {
        /* Need to set a new color */
        call->hCon = hCon;
        if (appCon->color == 0)
        {
            randSeed = randSeed * 1103515245 + 12345;
            appCon->color = randSeed;
        }
    }

    call->hCon = hCon;

    /* create the colors */
    epUtilsEvent(call->ep, "callColor", call, "");

    return appCon->id;
}


/******************************************************************************
 * epCallGetById
 * ----------------------------------------------------------------------------
 * General: Find a call object by its id.
 *
 * Return Value: The call object if found, NULL otherwise.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep       - Endpoint object to use.
 *         id       - Id of the call to look for.
 * Output: None.
 *****************************************************************************/
EpCallObj *epCallGetById(
    IN EpObj    *ep,
    IN RvInt32  id)
{
    EpCallObj *call;

    lepLockEpForHash(ep, call = (EpCallObj *)epHashGet(ep->calls, id));

    return call;
}


/******************************************************************************
 * epCallGetByCallId
 * ----------------------------------------------------------------------------
 * General: Find a call object by its call identifier.
 *
 * Return Value: The call object if found, NULL otherwise.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         callId       - Call identifier to look for.
 *         isIncoming   - RV_TRUE for incoming call.
 * Output: None.
 *****************************************************************************/
EpCallObj *epCallGetByCallId(
    IN EpObj    *ep,
    IN RvUint8  *callId,
    IN RvBool   isIncoming)
{
    EpCallObj *call;
    void *key = NULL;
    RvUint8 curCallId[16];
    RvInt32 callIdLen;

    do
    {
        lepLockEpForHash(ep, call = (EpCallObj *)epHashGetAny(ep->calls, &key));
        if ((call != NULL) && (call->isIncoming == isIncoming))
        {
            callIdLen = (RvInt32)sizeof(curCallId);
            if (cmCallGetParam(call->hsCall, cmParamCallID, 0, &callIdLen, (char *)curCallId) >= 0)
            {
                if (memcmp(curCallId, callId, sizeof(curCallId)) == 0)
                    return call;
            }
        }
    } while (call != NULL);

    return NULL;
}






/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * dropSingleCall
 * ----------------------------------------------------------------------------
 * General: Timer callback that drops all opened called one at a time.
 *          We use a timer so we don't get the network's buffers get congested.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  context  - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
static void RVCALLCONV dropSingleCall(IN void* context)
{
    EpObj *ep = (EpObj *)context;
    EpCallObj *call;
    void *key = NULL;

    lepLockEpForHash(ep, call = (EpCallObj *)epHashGetAny(ep->calls, &key));
    while (call != NULL)
    {
        if (call->numOfChannels >= 0)
            break; /* Found a call we need to drop */
        lepLockEpForHash(ep, call = (EpCallObj *)epHashGetAny(ep->calls, &key));
    }
    if (call == NULL)
    {
        /* No more calls - we're done here */
        if (ep->dropAllTimer != HTI_NULL_TIMER)
            mtimerReset(ep->hTimers, ep->dropAllTimer);
        ep->dropAllTimer = HTI_NULL_TIMER;
        return;
    }

    epCallDrop(call, ep->dropAllReason, NULL);
}


/********************************************************************************************
 * Call_DialAltEp
 * purpose : Get new number for a call
 * input   : CurrentCall    - Current call information to use
 * output  : none
 * return  : Non-negative value on success, negative on failure
 ********************************************************************************************/
static int callDialAltEp(EpCallObj   *call)
{
	int altEpNum;

    /* get info about Alternate endpoints */
	cmCallGetParam(call->hsCall, cmParamAltEpNum, 0, &altEpNum, NULL);

	if (altEpNum < 0)
	{
		/* no altEp were connected. Let's connect the first one */
		altEpNum = 1;
	}
	else
    {
        /* let's connect the "next" altEp */
		altEpNum++;
	}
	return cmCallDialAltEp(call->hsCall, altEpNum);
}



#ifdef __cplusplus
}
#endif
