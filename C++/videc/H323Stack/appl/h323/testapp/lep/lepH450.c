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
epH450.c

H.450 add-on module of the endpoint.
Handles the supplementary services.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/

#include "lep.h"
#include "lepHash.h"
#include "lepCall.h"
#include "lepUtils.h"
#include "lepConversions.h"
#include "lepH450.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifdef USE_H450

#include "h450asn.h"

/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/


/* Used to hold information about a CallCompletion service (H450.9) */
typedef struct
{
    RvInt32             id; /* Service id */
    EpObj               *ep; /* Endpoint object used */
    HSSESERVICE         hService; /* SSE service handle */
    RvChar              destAddr[600]; /* Destination address */
    RvBool              connectionRetained; /* Indication if call signaling connection is retained or not */
} CallCompletionStruct;





/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/
static EpCallObj *createUnmarkedCall(
    IN EpObj    *ep);
static void convertToH450Address(
    IN  const RvChar    *inString,
    OUT RvChar          *str);
static void convertAddrToStr(
    IN  cmTransportAddress  *addr,
    OUT RvChar              *str);
static RvStatus pickupNotifyGroup(
    IN EpObj                *ep,
    IN EpCallObj            *inCall,
    IN const RvChar         *callIdentifier,
    IN RvBool               isOn,
    IN RvInt                position);
static RvInt findParkedCall(
    IN  EpObj       *ep,
    IN  HPVT        hVal,
    IN  RvPvtNodeId paramsNodeId,
    OUT RvPvtNodeId *prkNodeId);
static void countH450Apdu(
    IN EpObj        *ep,
    IN RvPvtNodeId  nodeId);





/*-----------------------------------------------------------------------*/
/*                           CALLBACK FUNCTIONS                          */
/*-----------------------------------------------------------------------*/


static int RVCALLCONV sseEvCallMake(
    IN  HSSEAPPCALL     hSSEaCallPri,
    IN  HSSECALL        hSSECallPri,
    IN  sseServices     serviceName)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    call->h450outCall = call->ep->h450outCounter;
    call->ep->h450outCounter++;
    epUtilsEvent(call->ep, "Tag", call, "\"h450msdCall%d\" 0-1", call->h450outCall);
    epUtilsEvent(call->ep, "H450", call, "sseEvCallMake.%d", serviceName);
    epUtilsEvent(call->ep, "Rec", call, "@setH450Params terminalType= @TAG@h450msdCall%d", call->h450outCall);

    if (call->ep->cfg.H450_terminalType >= 0)
        cmCallSetParam(call->hsCall, cmParamMsdTerminalType, 0, call->ep->cfg.H450_terminalType, NULL);

    /* Set the call parameters */
    if (call->ep->cfg.bAutomaticFaststart && call->ep->cfg.bSupportFaststart)
        epCallGenerateFaststartRequest(call, RV_TRUE);

#if 0
    /* see if Ad-Hoc conferencing is indicated */
    if (call->ep->cfg.bAdHocUse)
    {
        CallInfo * SameConferenceCall;
        char * selectedCall = call->ep->cfg.strAdHocCall;

        if (sscanf(selectedCall, "%*d %p:", &SameConferenceCall) == 1)
            cmCallInvite(SecCall->hsCall, SameConferenceCall->hsCall);
        TclSetVariable("tmp(adHoc,use)", "0");
    }
#endif

    switch (call->ep->cfg.H450_responseType)
    {
    case ssDefaultAck:
        /* We return the value from cmCallDial() so that if it returns an error, SSE will
           get rid of this call on its own. */
        return cmCallDial(call->hsCall);

    case ssDefaultNack:
        return RV_ERROR_UNKNOWN;

    case ssWait:
        /* Notify User as to choice */
        epUtilsEvent(call->ep, "Catch", call,
            "gui:askQuestion {%d} {%s service dialing out} "
            "{Would you like to comply?} "
            "{H450.CallMake %p 1 %d} "
            "{H450.CallMake %p 0 %d}",
            call->id, SSEServices2String(serviceName), call, serviceName, call, serviceName);
        epUtilsEvent(call->ep, "Catch", call, "Call:addWindowToList %p .q%d", call, call->id);
    }

    return RV_OK;
}


static int RVCALLCONV sseEvCallTransfer(
    IN  HSSEAPPCALL     hSSEaCallPri,
    IN  HSSECALL        hSSECallPri,
    OUT HSSECALL        *hSSECallSec)
{
    EpCallObj *callPri = (EpCallObj *)hSSEaCallPri;
    EpCallObj *callSec;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(callPri->ep, "H450", callPri, "sseEvCallTransfer");

    /* Create a new call handle */
    callSec = epCallCreate(callPri->ep, NULL, 0);
    if (callSec == NULL)
        return (int)ssDefaultNack;

    /* Change state of the primary call */
    epUtilsEvent(callPri->ep, "Catch", callPri, "Call:AddRemark %d Transfered", callPri->id);

    /* output the call handle */
    *hSSECallSec = callSec->hSSECall;

    /* we link these 2 calls - we might need from the GUI to have both handles */
    callSec->linkedCall = callPri;

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCallTransferIdentify(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvCallTransferIdentify");

    /* We send the call proceeding message so we don't have a problem sending the facility
       message with the transfer reply on it */
    cmCallSendCallProceeding(call->hsCall);

    switch (call->ep->cfg.H450_responseType)
    {
    case ssDefaultAck:
        break;

    case ssDefaultNack:
        epUtilsEvent(call->ep, "Catch", call, "after idle {Call:RemoveRemark %d}", call->id);
        break;

    case ssWait:
        if (call->ep->cfg.H450_responseType == ssWait)
        {
            /* Notify User as to choice */
            epUtilsEvent(call->ep, "Catch", call,
                "gui:askQuestion {%d} {Transfer identify for call %d} "
                "{We were asked for our identity on the consultaiton call. Would you like to comply?} "
                "{H450.TransferNonDefaultResponse 7 %p 1 -1} "
                "{H450.TransferNonDefaultResponse 7 %p 0 1008}",
                call->id, call->id, call, call);
            epUtilsEvent(call->ep, "Catch", call, "Call:addWindowToList %p .q%d", call, call->id);
        }
        break;
    }

    return (int)call->ep->cfg.H450_responseType;
}


static void RVCALLCONV sseEvCallTransferResponse(
        IN      HSSEAPPCALL         hSSEaCallPri,
        IN      HSSECALL            hSSECallPri,
        IN      ctPrimitivesEnum    opcode,
        IN      int                 errCode,
        IN      RvBool              ok,
        IN      RvPvtNodeId         nodeId)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(nodeId);
    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvCallTransferResponse.%d.%d.%d", opcode, errCode, ok);

    switch (opcode)
    {
    case ssCTIdentify:
        epUtilsLog(call->ep, call, "Got response from transferred-to EP for ctIdentify");
        break;
    case ssCTInitiate:
        epUtilsLog(call->ep, call, "Got response from transferred EP for ctInitiate");
        break;
    case ssCTSetup:
        epUtilsLog(call->ep, call, "Got response from transferred EP for ctSetup");
        break;
    case ssCTActive:
        epUtilsLog(call->ep, call, "Got ctActive (probably from GK)");
        break;
    case ssCTComplete:
        epUtilsLog(call->ep, call, "Got ctComplete (probably from GK)");
        break;
    case ssCTAbandon:
        epUtilsLog(call->ep, call, "Got ctAbandon - someone wants out");
        break;
    case ssCTTimeout:
        epUtilsLog(call->ep, call, "Got a timeout on this service");
        break;
    default:
        epUtilsLog(call->ep, call, "Bad opcode in transfer response callback");
        break;
    }

    if (!ok)
    {
        epUtilsEvent(call->ep, "Catch", call, "after idle {Call:RemoveRemark %d}", call->id);
    }
}


static int RVCALLCONV sseEvCallTransferSetup(
        IN      HSSEAPPCALL     hSSEaCallPri,
        IN      HSSECALL        hSSECallPri)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvCallTransferSetup");

    if (call->ep->cfg.H450_responseType == ssWait)
    {
        /* Notify User as to choice */
        epUtilsEvent(call->ep, "Catch", call,
            "gui:askQuestion {%d} {Transfer setup for call %d} "
            "{We were asked to connect an incoming transferred call (we're TRTSE). Would you like to comply?} "
            "{H450.TransferNonDefaultResponse 10 %p 1 -1} "
            "{H450.TransferNonDefaultResponse 10 %p 0 1008}",
            call->id, call->id, call, call);
        epUtilsEvent(call->ep, "Catch", call, "Call:addWindowToList %p .q%d", call, call->id);
    }

    return (int)call->ep->cfg.H450_responseType;
}


static int RVCALLCONV sseEvCallReroute(
    IN  HSSEAPPCALL hSSEaCallPri,
    IN  HSSECALL    hSSECallPri,
    OUT HSSECALL    *hSSECallSec)
{
    EpCallObj *callPri = (EpCallObj *)hSSEaCallPri;
    EpCallObj *callSec = NULL;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(callPri->ep, "H450", callPri, "sseEvCallReroute");

    if (callPri->ep->cfg.H450_responseType == ssDefaultNack)
        return (int)ssDefaultNack;

    /* Create a new call handle */
    callSec = epCallCreate(callPri->ep, NULL, 0);
    if (callSec == NULL)
        return (int)ssDefaultNack;

    /* Change state of the primary call */
    epUtilsEvent(callPri->ep, "Catch", callPri, "Call:AddRemark %d Rerouted", callPri->id);

    /* output the call handle */
    *hSSECallSec = callSec->hSSECall;

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvForwardActivated(
    IN HSSEAPPCALL  hSSEaCall,
    IN HSSECALL     hSSECall)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvForwardActivated");

    /* Close the call we used */
    epCallDrop(call, cmReasonTypeUndefinedReason, NULL);

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvForwardDeactivated(
    IN HSSEAPPCALL  hSSEaCall,
    IN HSSECALL     hSSECall)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvForwardDeactivated");

    /* Close the call we used */
    epCallDrop(call, cmReasonTypeUndefinedReason, NULL);

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvRemoteHoldInd(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri)
{
    /* Signals that Remote Hold Request has been received. will either answer or ask user
       what to do */
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvRemoteHoldInd");

    switch (call->ep->cfg.H450_responseType)
    {
    case ssDefaultAck:
        epUtilsEvent(call->ep, "Catch", call, "after idle {Call:AddRemark %d FarHeld}", call->id);
        break;

    case ssDefaultNack:
        break;

    case ssWait:
        /* Notify User as to choice */
        epUtilsEvent(call->ep, "Catch", call,
            "gui:askQuestion {%d} {Remote Hold for call %d} "
            "{Other side has signaled Remote Hold request. Would you like to comply?} "
            "{H450.HoldSendNonDefaultResponse hold %p 1 -1} "
            "{H450.HoldSendNonDefaultResponse hold %p 0 1008}",
            call->id, call->id, call, call);
        epUtilsEvent(call->ep, "Catch", call, "Call:addWindowToList %p .q%d", call, call->id);
        break;
    }

    return (int)call->ep->cfg.H450_responseType;
}


static void RVCALLCONV sseEvRemoteHoldConfirm(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri,
    IN int          errCode,
    IN RvUint32     ok)
{
    /* Signals that the remote side has either confirmed the request or has returned an error */
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvRemoteHoldConfirm.%d.%d", errCode, ok);

    if (ok)
        epUtilsEvent(call->ep, "Catch", call, "Call:AddRemark %d FarHold", call->id);
    else
    {
        if (errCode == -1)
            epUtilsLog(call->ep, call, "Hold request has timed out");
        else
            epUtilsLog(call->ep, call, "Hold request responded with errCode=%d", errCode);
    }
}


static int RVCALLCONV sseEvRemoteHoldRetrieveInd(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri)
{
    /* Signals that Remote Hold Retrieve has been received. will either answer or ask user
       what to do */
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvRemoteHoldRetrieveInd");

    switch (call->ep->cfg.H450_responseType)
    {
    case ssDefaultAck:
        epUtilsEvent(call->ep, "Catch", call, "after idle {Call:RemoveRemark %d}", call->id);
        break;

    case ssDefaultNack:
        break;

    case ssWait:
        /* Notify User as to choice */
        epUtilsEvent(call->ep, "Catch", call,
            "gui:askQuestion {%d} {Remote Hold for call %d} "
            "{Other side has signaled Remote Hold retrieve. Would you like to comply?} "
            "{H450.HoldSendNonDefaultResponse retr %p 1 -1} "
            "{H450.HoldSendNonDefaultResponse retr %p 0 1008}",
            call->id, call->id, call, call);
        epUtilsEvent(call->ep, "Catch", call, "Call:addWindowToList %p .q%d", call, call->id);
        break;
    }

    return (int)call->ep->cfg.H450_responseType;
}


static void RVCALLCONV sseEvRemoteRetrieveConfirm(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri,
    IN int          errCode,
    IN RvUint32     ok)
{
    /* Signals that the remote side has either confirmed the request or has returned an error */
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvRemoteHoldRetrieveConfirm.%d.%d", errCode, ok);

    if (ok)
        epUtilsEvent(call->ep, "Catch", call, "Call:RemoveRemark %d", call->id);
    else
    {
        if (errCode == -1)
            epUtilsLog(call->ep, call, "Hold retrieve has timed out");
        else
            epUtilsLog(call->ep, call, "Hold retrieve responded with errCode=%d", errCode);
    }
}


static int RVCALLCONV sseEvNearHoldInd(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri)
{
    /* Signals that Near Hold Request has been received. */
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvNearHoldInd");

    epUtilsEvent(call->ep, "Catch", call, "Call:AddRemark %d NearHeld", call->id);

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvNearHoldRetrieveInd(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri)
{
    /* Signals that Near Hold Retrieve has been received. */
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvNearHoldRetrieveInd");

    epUtilsEvent(call->ep, "Catch", call, "Call:RemoveRemark %d", call->id);

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCParkRequestInd(
    IN  HSSEAPPCALL hSSEaCallPri,
    IN  HSSECALL    hSSECallPri,
    OUT HSSECALL*   callSec)
{
    EpCallObj *call1 = (EpCallObj *)hSSEaCallPri;
    EpCallObj *call2 = NULL;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call1->ep, "H450", call1, "sseEvCParkRequestInd");

    /*check if park is possible*/
    if (!call1->ep->cfg.bH4505_enablePark)
        return RV_ERROR_UNKNOWN;

    /* Create a new call handle */
    call2 = epCallCreate(call1->ep, NULL, 0);
    if (call2 == NULL)
        return RV_ERROR_OUTOFRESOURCES;

    /* Add remarks to the calls */
    epUtilsEvent(call1->ep, "Catch", call1, "Call:AddRemark %d ToPark", call1->id);
    epUtilsEvent(call1->ep, "Catch", call2, "after idle {Call:AddRemark %d Parking}", call2->id);

    /* output the call handle */
    *callSec = call2->hSSECall;

    return RV_OK;
}


static int RVCALLCONV sseEvCPickExeInd(
    IN  HSSEAPPCALL hSSEaCall,
    IN  HSSECALL    hSSECall,
    IN  char*       callIdentifier,
    OUT int*        reason)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);
    RV_UNUSED_ARG(callIdentifier);
    RV_UNUSED_ARG(reason);

    epUtilsEvent(call->ep, "H450", call, "sseEvCPickExeInd");

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCPickupRequestInd(
    IN  HSSEAPPCALL hSSEaCall,
    IN  HSSECALL    hSSECall,
    IN  char*       callIdentifier)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);
    RV_UNUSED_ARG(callIdentifier);

    epUtilsEvent(call->ep, "H450", call, "sseEvCPickupRequestInd");

    cmCallSendCallProceeding(call->hsCall);

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCPickupFindCall(
    IN  HSSEAPPCALL hSSEaCall,
    IN  HSSECALL    hSSECall,
    IN  char*       callIdentifier,
    IN  RvBool      isIncoming,
    OUT HSSECALL  * oldHSSCall)
{
    EpCallObj *prkdCall = (EpCallObj *)hSSEaCall;
    EpCallObj *pickedCall;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(prkdCall->ep, "H450", prkdCall, "sseEvCPickupFindCall.%d", isIncoming);

    epUtilsEvent(prkdCall->ep, "Catch", prkdCall, "after idle {Call:AddRemark %d Pickup-Call}", prkdCall->id);

    if (!isIncoming)
    {
        *oldHSSCall = prkdCall->ep->pickupSseCall;
        return 0;
    }

    pickedCall = epCallGetByCallId(prkdCall->ep, (RvUint8 *)callIdentifier, isIncoming);
    if (pickedCall == NULL)
        return (int)ssDefaultNack;

    *oldHSSCall = pickedCall->hSSECall;
    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCPickupInd(
    IN  HSSEAPPCALL hSSEaCall,
    IN  HSSECALL    hSSECall,
    OUT char*       locAddr,
    OUT HSSECALL*   pickupCall)
{
    EpCallObj *pickingCall = (EpCallObj *)hSSEaCall;
    EpCallObj *pickedCall;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(pickingCall->ep, "H450", pickingCall, "sseEvCPickupInd");

    if (pickingCall->state != cmCallStateConnected) /*in case of remote pickup*/
    {
        cmTransportAddress csAddr;

        /* Create a new call handle */
        pickedCall = epCallCreate(pickingCall->ep, NULL, 0);
        if (pickedCall == NULL)
        {
            epUtilsError(pickingCall->ep, NULL, "Cannot create picked-up call");
            return (int)ssDefaultNack;
        }

        epUtilsEvent(pickedCall->ep, "Catch", pickedCall, "after idle {Call:AddRemark %d Picked}", pickedCall->id);
        epUtilsEvent(pickingCall->ep, "Catch", pickingCall, "after idle {Call:AddRemark %d Picking}", pickingCall->id);

        *pickupCall = pickedCall->hSSECall;

        /* get local address */
        cmGetLocalCallSignalAddress(pickingCall->ep->hApp, &csAddr);
        convertAddrToStr(&csAddr, locAddr);
    }

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCPSetupInd(
    IN  HSSEAPPCALL       hSSEaCall,
    IN  HSSECALL          hSSECall,
    OUT int             * parkPosition,
    OUT cpCallCondition * parkCondition)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;
    RvStatus status;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvCPSetupInd");

    /*check if park is possible*/
    if (!call->ep->cfg.bH4505_enablePark)
        return RV_ERROR_UNKNOWN;

    *parkPosition = call->id;
    *parkCondition = parkedToGroup;

    cmCallSendCallProceeding(call->hsCall);

    status = epH450GroupNotification(call, *parkPosition, RV_FALSE);

    if (status < 0)
        return (int)ssDefaultNack;
    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCPGroupIndication(
    IN HSSEAPPCALL hSSEaCall,
    IN HSSECALL    hSSECall,
    IN int         parameters,
    IN RvBool      isIndOn)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;
    HPVT hVal;

    RV_UNUSED_ARG(hSSECall);

    hVal = ssGetValTree(sseGetSSHandle(call->ep->hSSEApp));

    if (isIndOn)
    {
        cmAlias alias;
        RvChar strData[513];
        RvChar callAddr[600], parkAddr[600];
        RvInt32 position;
        int index = 0;
        RvPvtNodeId tmpNode;

        epUtilsEvent(call->ep, "H450", call, "sseEvCPGroupIndicationOn");

        alias.string = strData;
        alias.length = sizeof(strData);
        callAddr[0] = '\0';
        parkAddr[0] = '\0';

        tmpNode = pvtAdd(hVal, call->ep->parkedCallsNodeId, __nul(0), 0, NULL, &index);
        pvtSetTree(hVal, tmpNode, hVal, parameters);

        if (cmVt2Alias(hVal, &alias, pvtGetNodeIdByPath(hVal, parameters, "partyToRetrieve.destinationAddress.1")) >= 0)
            Alias2String(&alias, callAddr);

        if (cmVt2Alias(hVal, &alias, pvtGetNodeIdByPath(hVal, parameters, "retrieveAddress.destinationAddress.1")) >= 0)
            Alias2String(&alias, parkAddr);

        pvtGetChildByFieldId(hVal, parameters, __h450(parkPosition), &position, NULL);

        epUtilsEvent(call->ep, "Catch", NULL, "$tmp(h450Tab).service.prkpku.calls.01 add radiobutton -indicatoron 0 -value %d -variable tmp(h450,callPku) -label {%d %s %s}",
            index, position, callAddr, parkAddr);
    }
    else
    {
        RvPvtNodeId nodeId;
        int index;

        index = findParkedCall(call->ep, hVal, parameters, &nodeId);
        pvtDelete(hVal, nodeId);

        epUtilsEvent(call->ep, "H450", call, "sseEvCPGroupIndicationOff.%d", index);

        epUtilsEvent(call->ep, "Catch", NULL, "h4505DelGroupInd %d", index);
    }

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCPRequestConfirm(
    IN HSSEAPPCALL hSSEaCallPri,
    IN HSSECALL    hSSECallPri,
    IN int         errCode,
    IN RvBool      ok)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvCPRequestConfirm.%d", (ok ? 0 : errCode));

    epUtilsLog(call->ep, call, "Received Call Park request confirmation: err=%d, ok=%d", errCode, ok);

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCPPickRequConfirm(
    IN HSSEAPPCALL hSSEaCall,
    IN HSSECALL    hSSECall,
    IN int         errCode,
    IN int         ok,
    IN char*       callIdentifier)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);
    RV_UNUSED_ARG(callIdentifier);

    epUtilsEvent(call->ep, "H450", call, "sseEvCPPickRequConfirm.%d", (ok ? 0 : errCode));

    epUtilsLog(call->ep, call, "Received Call PickRequ confirmation: err=%d, ok=%d", errCode, ok);

    return (int)ssDefaultAck;

}


static int RVCALLCONV sseEvCPPickupConfirm(
    IN HSSEAPPCALL    hSSEaCall,
    IN HSSECALL       hSSECall,
    IN int            errCode,
    IN int            ok,
    IN char*          callIdentifier)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;
    RvStatus status;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvCPPickupConfirm.%d", (ok ? 0 : errCode));

    epUtilsLog(call->ep, call, "Received Call Pick request confirmation: err=%d, ok=%d", errCode, ok);

    if (ok)
    {
        status = pickupNotifyGroup(call->ep, call, callIdentifier, RV_FALSE, 0);
        if (status != RV_OK)
            return (int)ssDefaultNack;
        call->bParkNotifiedGroup = RV_FALSE;
    }

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCPSetupConfirm(
    IN HSSEAPPCALL hSSEaCall,
    IN HSSECALL    hSSECall,
    IN int         errCode,
    IN int         ok,
    IN int*        reason)
{
    /* if the OK is true but the error code is 0
       so the application will know that in that case
       it means the timer had finished and no pickup
       was done */
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);
    RV_UNUSED_ARG(reason);

    epUtilsEvent(call->ep, "H450", call, "sseEvCPSetupConfirm.%d", (ok ? 0 : errCode));

    epUtilsLog(call->ep, call, "Received Call Park setup confirmation: err=%d, ok=%d", errCode, ok);

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCPGroupIndicationConfirm(
    IN HSSEAPPCALL hSSEaCall,
    IN HSSECALL    hSSECall,
    IN int         errCode,
    IN int         ok,
    IN RvBool      indOn)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvCPGroupIndicationConfirm.%d.%d", (ok ? 0 : errCode), indOn);

    epUtilsLog(call->ep, call, "Received Call Park group indication confirmation: err=%d, ok=%d, ind=%d",
        errCode, ok, indOn);

    cmCallDrop(call->hsCall);

    return (int)ssDefaultAck;
}


static void RVCALLCONV sseEvWaitInd(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri)
{
    /* called when Wait service has been activated, notifies user. */
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvWaitInd");

    epUtilsEvent(call->ep, "Catch", call, "after 400 {Call:AddRemark %d Waiting}", call->id);
}


static void RVCALLCONV sseEvWaitTimerExpired(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri)
{
    /* Called when Wait service's timer has expired. drops call. */
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvWaitTimerExpired");

    cmCallDrop(call->hsCall);
}


static int RVCALLCONV sseEvMWIActivateInd(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri,
    IN char *       servedUserNr,
    IN int          numOfMsgs,
    IN char *       service,
    IN MsgCtrIdT *  msgCtrId,
    IN char *       origNum,
    IN char *       time,
    IN int          priority)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);
    RV_UNUSED_ARG(servedUserNr);
    RV_UNUSED_ARG(msgCtrId);
    RV_UNUSED_ARG(origNum);
    RV_UNUSED_ARG(time);

    epUtilsEvent(call->ep, "H450", call, "sseEvMWIActivateInd.%d.%s.%d", numOfMsgs, service, priority);

    epUtilsLog(call->ep, call, "Received MWI Activate Indication: service: %s, %d messages", service, numOfMsgs);

    switch (call->ep->cfg.H450_responseType)
    {
    case ssDefaultAck:
    case ssDefaultNack:
        break;

    case ssWait:
        /* Notify User as to choice */
        epUtilsEvent(call->ep, "Catch", call,
            "gui:askQuestion {%d} {MWI Activate} "
            "{MessageCenter has activated a Message Waiting indication. Confirm?} "
            "{H450.MWISendNonDefaultResponse %p 1 -1} "
            "{H450.MWISendNonDefaultResponse %p 0 1008}",
            call->id, call, call);
        epUtilsEvent(call->ep, "Catch", call, "Call:addWindowToList %p .q%d", call, call->id);
        break;
    }

    return (int)call->ep->cfg.H450_responseType;
}


static void RVCALLCONV sseEvMWIActivateConfirm(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri,
    IN int          errCode,
    IN RvUint32     ok)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvMWIActivateConfirm.%d", (ok ? 0 : errCode));

    if (ok)
        epUtilsLog(call->ep, call, "Received MWI Activate Confirmation");
    else if (errCode == -1)
        epUtilsLog(call->ep, call, "MWI Activate has timed out");
    else
        epUtilsLog(call->ep, call, "MWI Activate responded with errCode = %d", errCode);
}


static int RVCALLCONV sseEvMWIDeactivateInd(
    IN HSSEAPPCALL      hSSEaCallPri,
    IN HSSECALL         hSSECallPri,
    IN char *           servedUserNr,
    IN char *           service,
    IN MsgCtrIdT *      msgCtrId,
    IN int              isCallBackReq)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);
    RV_UNUSED_ARG(servedUserNr);
    RV_UNUSED_ARG(msgCtrId);

    epUtilsEvent(call->ep, "H450", call, "sseEvMWIDeactivateInd.%s.%d", service, isCallBackReq);

    epUtilsLog(call->ep, call, "Received MWI Deactivate Indication: service: %s", service);

    switch (call->ep->cfg.H450_responseType)
    {
    case ssDefaultAck:
    case ssDefaultNack:
        break;

    case ssWait:
        /* Notify User as to choice */
        if (isCallBackReq)
        {
            epUtilsEvent(call->ep, "Catch", call,
                "gui:askQuestion {%d} {MWI Deactivate} "
                "{MessageCenter has Deactivated a CallBack indication. Confirm?} "
                "{H450.MWISendNonDefaultResponse %p 1 -1} "
                "{H450.MWISendNonDefaultResponse %p 0 1008}",
                call->id, call, call);
        }
        else
        {
            epUtilsEvent(call->ep, "Catch", call,
                "gui:askQuestion {%d} {MWI Deactivate} "
                "{MessageCenter has Deactivated a Message Waiting indication. Confirm?} "
                "{H450.MWISendNonDefaultResponse %p 1 -1} "
                "{H450.MWISendNonDefaultResponse %p 0 1008}",
                call->id, call, call);
        }
        epUtilsEvent(call->ep, "Catch", call, "Call:addWindowToList %p .q%d", call, call->id);
        break;
    }

    return (int)call->ep->cfg.H450_responseType;
}


static void RVCALLCONV sseEvMWIDeactivateConfirm(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri,
    IN int          errCode,
    IN RvUint32     ok)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvMWIDeactivateConfirm.%d", (ok ? 0 : errCode));

    if (ok)
        epUtilsLog(call->ep, call, "Received MWI Deactivate Confirmation");
    else if (errCode == -1)
        epUtilsLog(call->ep, call, "MWI Deactivate has timed out");
    else
        epUtilsLog(call->ep, call, "MWI Deactivate responded with errCode = %d", errCode);
}


static void RVCALLCONV sseEvMWIInterrogateConfirm(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri,
    IN int          resultElements,
    IN int          errCode,
    IN int          ok)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);
    RV_UNUSED_ARG(resultElements);

    epUtilsEvent(call->ep, "H450", call, "sseEvMWIInterrogateConfirm.%d", (ok ? 0 : errCode));

    if (ok)
        epUtilsLog(call->ep, call, "Received MWI Interrogate Confirmation");
    else if (errCode == -1)
        epUtilsLog(call->ep, call, "MWI Interrogate has timed out");
    else
        epUtilsLog(call->ep, call, "MWI Interrogate responded with errCode = %d", errCode);
}


static int RVCALLCONV sseEvMWIInterrogateInd(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri,
    IN char *       servedUserNr,
    IN char *       service,
    IN MsgCtrIdT *  msgCtrId,
    IN int          isCallBackReq)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);
    RV_UNUSED_ARG(servedUserNr);
    RV_UNUSED_ARG(msgCtrId);
    RV_UNUSED_ARG(isCallBackReq);

    epUtilsEvent(call->ep, "H450", call, "sseEvMWIInterrogateInd.%s.%d", service, isCallBackReq);

    epUtilsLog(call->ep, call, "Received MWI Interrogation Indication: service: %s", service);

    switch (call->ep->cfg.H450_responseType)
    {
    case ssDefaultAck:
    case ssDefaultNack:
        break;

    case ssWait:
        /* Notify User as to choice */
        epUtilsEvent(call->ep, "Catch", call,
            "gui:askQuestion {%d} {MWI Interrogate} "
            "{Client had sent an interrogation message. Confirm?} "
            "{H450.MWISendNonDefaultResponse %p 1 -1} "
            "{H450.MWISendNonDefaultResponse %p 0 1008}",
            call->id, call, call);
        epUtilsEvent(call->ep, "Catch", call, "Call:addWindowToList %p .q%d", call, call->id);
        break;
    }

    return (int)call->ep->cfg.H450_responseType;
}


static void RVCALLCONV sseEvNIAlertingNameInd(
    IN HSSEAPPCALL  hSSEaCall,
    IN HSSECALL     hSSECall,
    IN char         *simple_name,
    IN char         *character_set,
    IN niNameChoice choice)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);
    RV_UNUSED_ARG(character_set);

    if (choice == notAvailable)
        simple_name = "";

    epUtilsEvent(call->ep, "H450", call, "sseEvNIAlertingNameInd.%d.%.20s%s",
        choice, simple_name, (strlen(simple_name)>20)?"...":"");

    epUtilsLog(call->ep, call, "Received %s name: %s",
        H450niNameChoice2String(choice), simple_name);
}


static void RVCALLCONV sseEvNIBusyNameInd(
    IN HSSEAPPCALL  hSSEaCall,
    IN HSSECALL     hSSECall,
    IN char         *simple_name,
    IN char         *character_set,
    IN niNameChoice choice)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);
    RV_UNUSED_ARG(character_set);

    if (choice == notAvailable)
        simple_name = "";

    epUtilsEvent(call->ep, "H450", call, "sseEvNIBusyNameInd.%d.%.20s%s",
        choice, simple_name, (strlen(simple_name)>20)?"...":"");

    epUtilsLog(call->ep, call, "Received %s name: %s",
        H450niNameChoice2String(choice), simple_name);
}


static void RVCALLCONV sseEvNICallingNameInd(
    IN HSSEAPPCALL  hSSEaCall,
    IN HSSECALL     hSSECall,
    IN char         *simple_name,
    IN char         *character_set,
    IN niNameChoice choice)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);
    RV_UNUSED_ARG(character_set);

    if (choice == notAvailable)
        simple_name = "";

    epUtilsEvent(call->ep, "H450", call, "sseEvNICallingNameInd.%d.%.20s%s",
        choice, simple_name, (strlen(simple_name)>20)?"...":"");

    epUtilsLog(call->ep, call, "Received %s name: %s",
        H450niNameChoice2String(choice), simple_name);
}


static void RVCALLCONV sseEvNIConnectedNameInd(
    IN HSSEAPPCALL  hSSEaCall,
    IN HSSECALL     hSSECall,
    IN char         *simple_name,
    IN char         *character_set,
    IN niNameChoice choice)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);
    RV_UNUSED_ARG(character_set);

    if (choice == notAvailable)
        simple_name = "";

    epUtilsEvent(call->ep, "H450", call, "sseEvNIConnectedNameInd.%d.%.20s%s",
        choice, simple_name, (strlen(simple_name)>20)?"...":"");

    epUtilsLog(call->ep, call, "Received %s name: %s",
        H450niNameChoice2String(choice), simple_name);
}


static int RVCALLCONV sseEvServiceHandleCreated(
        IN      sseServiceType  servType,
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      HSSESERVICE     hSSEService,
        OUT     HSSEAPPSERVICE* hSSEaService)
{
    RvStatus status;
    RvInt32 id;
    CallCompletionStruct* ccInfo;
    EpCallObj *call = (EpCallObj *)hSSEaCall;
    EpObj *ep;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvServiceHandleCreated.%d", servType);

    /* Make sure it's the only service we can handle */
    if (servType != sseServiceCallCompletion)
        return RV_ERROR_UNKNOWN;

    ep = call->ep;

    /* Ask for an ID for this service */
    lepLockEpForHash(ep, id = ep->cb.epAllocateResourceId(ep, EpResourceCompletionService));
    if (id <= 0)
    {
        /* Call was rejected ??? */
        return RV_ERROR_BADPARAM;
    }

    /* Allocate a service object */
    lepLockEpForHash(ep, status = epHashAdd(ep->completionServices, id, (void**)&ccInfo));
    if (status != RV_OK)
    {
        /* No internal resources ??? */
        return status;
    }

    memset(ccInfo, 0, sizeof(*ccInfo));
    ccInfo->id = id;
    ccInfo->ep = call->ep;
    ccInfo->hService = hSSEService;
    ccInfo->connectionRetained = RV_TRUE;

    {
        /* We're the destination of the service - see who is the caller if we need to
           contact him later on a different call */
        cmAlias             alias;
        RvChar              aliasStr[513];
        RvInt32             ccSize = (RvInt32)sizeof(alias);

        alias.string = aliasStr;
        alias.length = (RvUint16)sizeof(aliasStr);
        if (cmCallGetParam(call->hsCall, cmParamSourceAddress, 0, &ccSize, (RvChar*)&alias) >= 0)
        {
            /* Get only the first alias of the source - because we're lazy */
            Alias2String(&alias, ccInfo->destAddr + strlen(ccInfo->destAddr));
        }
    }

    epUtilsEvent(call->ep, "Catch", call, "h4509AddService {%d - Incoming}", ccInfo->id);
    epUtilsLog(call->ep, call, "Creating CallCompletion service %d}", ccInfo->id);

    *hSSEaService = (HSSEAPPSERVICE)ccInfo;
    return 0;
}


static int RVCALLCONV sseEvServiceHandleIdle(
        IN      HSSEAPPSERVICE  hSSEaService,
        IN      HSSESERVICE     hSSEService,
        IN      sseServiceType  servType)
{
    CallCompletionStruct *ccInfo = (CallCompletionStruct *)hSSEaService;

    RV_UNUSED_ARG(hSSEService);

    epUtilsEvent(ccInfo->ep, "H450", NULL, "sseEvServiceHandleIdle.%d.%d", ccInfo->id, servType);

    switch (servType)
    {
        case sseServiceCallCompletion:
        {
            /* Let's clean the application's information about this service */
            epUtilsLog(ccInfo->ep, NULL, "Killing CallCompletion service %d", ccInfo->id);
            epUtilsEvent(ccInfo->ep, "Catch", NULL, "h4509RemoveService %d", ccInfo->id);

            lepLockEpForHash(ccInfo->ep, epHashRemove(ccInfo->ep->completionServices, ccInfo->id));
            break;
        }

        default:
            /* This option should never happen */
            return RV_ERROR_UNKNOWN;
    }

    return 0;
}


static void RVCALLCONV sseEvCCFindService(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      HSSEAPPSERVICE  hSSEaService,
        INOUT   HSSESERVICE*    hSSEService,
        IN      int             argNodeId)
{
    CallCompletionStruct *ccInfo;
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSEaCall);
    RV_UNUSED_ARG(hSSECall);
    RV_UNUSED_ARG(hSSEaService);
    RV_UNUSED_ARG(argNodeId);

    epUtilsEvent(call->ep, "H450", call, "sseEvCCFindService");

    if (*hSSEService != NULL)
    {
        /* SSE found a matching service - no need for the application to trouble itself */
        return;
    }

    /* The way we do it in the test application is just see if the GUI has a selected service.
       Otherwise, we just clear it up. */
    lepLockEpForHash(call->ep, ccInfo = (CallCompletionStruct *)epHashGet(call->ep->completionServices, call->ep->cfg.H4509_foundService));

    if (ccInfo != NULL)
        *hSSEService = ccInfo->hService;
    else
    {
        epUtilsLog(call->ep, call, "Ignoring message received for CallCompletion (as decided by the user)");
        *hSSEService = NULL;
    }
}


static int RVCALLCONV sseEvCCRequestInd(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      HSSEAPPSERVICE  hSSEaService,
        IN      HSSESERVICE     hSSEService,
        IN      RvBool          isBusy,
        IN      ccRetentionType retentionType,
        INOUT   RvBool*         retainService,
        OUT     RvBool*         retainConnection)
{
    static const RvChar *ccServiceStr[] = {"NR", "BS"};
    CallCompletionStruct *ccInfo;
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);
    RV_UNUSED_ARG(hSSEService);

    epUtilsEvent(call->ep, "H450", call, "sseEvCCRequestInd.%d.%d.%d", isBusy, retentionType, *retainService);

    /* See if we should retain the service or not */
    *retainService = call->ep->cfg.bH4509_retainService;

    /* See if we'd like to retain the connection or not */
    if (retentionType == ccRetainConnection)
        *retainConnection = RV_TRUE;
    else if (retentionType == ccDontRetainConnection)
        *retainConnection = RV_FALSE;
    else
        *retainConnection = call->ep->cfg.bH4509_retainConnection;

    epUtilsLog(call->ep, call, "Received CC%s Request Indication: retention: %d", ccServiceStr[isBusy], retentionType);

    ccInfo = (CallCompletionStruct *)hSSEaService;
    ccInfo->connectionRetained = *retainConnection;

    epUtilsEvent(call->ep, "Catch", call, "after idle {Call:AddRemark %d Completion-B}", call->id);

    if (call->ep->cfg.H450_responseType == ssDefaultNack)
        return (int)ssDefaultNack;

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCCExecPossibleInd(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      HSSEAPPSERVICE  hSSEaService,
        IN      HSSESERVICE     hSSEService,
        OUT     RvBool*         isBusy)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;
    CallCompletionStruct *ccInfo = (CallCompletionStruct *)hSSEaService;

    RV_UNUSED_ARG(hSSEaCall);
    RV_UNUSED_ARG(hSSECall);
    RV_UNUSED_ARG(hSSEService);

    epUtilsEvent(call->ep, "H450", call, "sseEvCCExecPossibleInd");

    epUtilsLog(call->ep, call, "Received ExecPossible for CallCompletion on service %d", ccInfo->id);

    /* We only need to state if we're busy or not - the result doesn't matter in this case... */
    *isBusy = call->ep->cfg.bH4509_isBusy;

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCCStageInd(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      HSSEAPPSERVICE  hSSEaService,
        IN      HSSESERVICE     hSSEService,
        IN      ccIndication    indication)
{
    int status = 0;
    EpObj *ep;
    EpCallObj *call = (EpCallObj *)hSSEaCall;
    CallCompletionStruct *ccInfo = (CallCompletionStruct *)hSSEaService;

    RV_UNUSED_ARG(hSSECall);
    RV_UNUSED_ARG(hSSEService);

    sseGetHandle((HSSEAPP)cmGetAHandle((HPROTOCOL)hSSEService), (HSSEAPPAPP*)&ep);

    epUtilsEvent(ep, "H450", call, "sseEvCCStageInd.%d", indication);

    epUtilsLog(ep, call, "Received %s for CallCompletion on service %d",
               CCIndication2String(indication), ccInfo->id);

    switch (indication)
    {
        case ccRingout:
        {
            /* Check if we're busy - if we are, we should return an error message back
               to the SSE */
            if (ep->cfg.bH4509_isBusy)
                status = RV_ERROR_UNKNOWN;
            break;
        }

        case ccTimeOut:
        {
            /* Cancel the service on timeout */
            status = epH450CallCompletionMessage(ep, "Cancel", ccInfo->id);
            break;
        }

        case ccCallDropped:
        {
            /* Call was dropped, but service exists. This means that we don't use
               connection retention on this service anymore, so just update the
               service information about it */
            ccInfo->connectionRetained = RV_FALSE;
            break;
        }

        default:
            break;
    }
    return status;
}


static int RVCALLCONV sseEvCallOfferInd(
    IN      HSSEAPPCALL     hSSEaCall,
    IN      HSSECALL        hSSECall)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvCallOfferInd");

    epUtilsLog(call->ep, call, "Received a call offer indication");

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCfbOverrideInd(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvCfbOverrideInd");

    epUtilsEvent(call->ep, "Catch", call, "after idle {Call:AddRemark %d {Call Offer: Overide CFB indicated}}", call->id);

    return (int)ssDefaultAck;
}


static void RVCALLCONV sseEvRemoteUserAlertingInd(
    IN HSSEAPPCALL  hSSEaCallPri,
    IN HSSECALL     hSSECallPri)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvRemoteUserAlertingInd");

    epUtilsEvent(call->ep, "Catch", call, "after idle {Call:AddRemark %d {Remote User Alerting}}", call->id);
}


static int RVCALLCONV sseEvCIInitialRequestInd(
    IN  HSSEAPPCALL      hSSEaCall,
    IN  HSSECALL         hSSECall,
    IN  ciPrimitivesEnum opcode,
    IN  char*            callIdentifier,
    OUT int *            reason,
    OUT RvBool *         possible,
    OUT RvBool *         isIsolated,
    OUT HSSECALL *       estCall)
{
    EpObj *ep;
    EpCallObj *call = (EpCallObj *)hSSEaCall;
    EpCallObj *intrusionCall;
    const RvChar *primitiveStr;

    RV_UNUSED_ARG(hSSECall);

    ep = call->ep;
    primitiveStr = SSPrimitive2String((int)opcode);

    epUtilsEvent(call->ep, "H450", call, "sseEvCIInitialRequestInd.%s", primitiveStr);

    epUtilsLog(call->ep, call, "Received call intrusion request of type %s", primitiveStr);
    epUtilsEvent(call->ep, "Catch", call, "after idle {Call:AddRemark %d {%s}}", call->id, primitiveStr);

    /* RAN - have to do it only if the intrCalll is not connected*/
    /* Send CallProceeding - without it, the SSE won't be able to send a FACILITY
       message on this call. */
    cmCallSendCallProceeding(call->hsCall);

    *possible = RV_FALSE;
    *isIsolated = call->ep->cfg.bH45011_isIsolated;
    *reason = 0;

    if (opcode == ssCISilentMoitor)
    {
        /* Call not found */
        *reason = (int)ssCInotBusy;

        /* Start searching for call in both directions - we might be responsible for both of them */
        intrusionCall = epCallGetByCallId(ep, (RvUint8*)callIdentifier, RV_FALSE);
        if (intrusionCall != NULL)
        {
            /* make sure this is the master */
            if (cmCallMasterSlaveStatus(intrusionCall->hsCall) != (int)cmMSMaster)
            {
                *reason = (int)ssCItemporarilyUnavailable;
                intrusionCall = epCallGetByCallId(ep, (RvUint8*)callIdentifier, RV_FALSE);
            }
        }
        if (intrusionCall == NULL)
            return RV_OK;

        /* make sure this is the master */
        if (cmCallMasterSlaveStatus(intrusionCall->hsCall) != (int)cmMSMaster)
        {
            *reason = (int)ssCItemporarilyUnavailable;
            return RV_OK;
        }
    }
    else
    {
        RvBool callOk = RV_FALSE;
        void *key = NULL;

        /* get any connected call */
        lepLockEpForHash(ep, intrusionCall = (EpCallObj *)epHashGetAny(ep->calls, &key));

        if (intrusionCall == NULL)
        {
            *reason = (int)ssCInotBusy;
            return RV_OK;
        }
        while (!callOk && (intrusionCall != NULL))
        {
            /* make sure this is the master of the call */
            if ((opcode == ssCIForcedRelease) ||
                (cmCallMasterSlaveStatus(intrusionCall->hsCall) == (int)cmMSMaster))
            {
                callOk = RV_TRUE;
                break;
            }

            /* not the master, try another call */
            lepLockEpForHash(ep, intrusionCall = (EpCallObj *)epHashGetAny(ep->calls, &key));
        }

        /* if we didn't find a call in which we are the master, intrusion is not possible */
        if (!callOk)
        {
            *reason = (int)ssCItemporarilyUnavailable;
            return RV_OK;
        }
    }

    /* get the SSE call */
    *estCall = intrusionCall->hSSECall;

    *possible = call->ep->cfg.bH45011_isPossible;

    /* keep this handle only if the call is likely to come back to us - if we are the slave side,
       the master will handle it */
    if ((opcode == ssCIRequest) && (!*isIsolated) &&
        (cmCallMasterSlaveStatus(intrusionCall->hsCall) != (int)cmMSSlave))
    {
        ep->intrudedCallId = intrusionCall->id;
        epUtilsEvent(ep, "Catch", NULL, "$tmp(h450Tab).service.intr.callHandle configure -text %p", intrusionCall);
    }

    return RV_OK;
}


static int RVCALLCONV sseEvCIIsolatedInd(
    IN  HSSEAPPCALL hSSEaCall,
    IN  HSSECALL    hSSECall,
    OUT int *       parameter)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvCIIsolatedInd");

    epUtilsLog(call->ep, call, "Received call intrusion isolated indication");

    *parameter = RV_PVT_INVALID_NODEID;

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCIForcedReleaseInd(
    IN  HSSEAPPCALL hSSEaCall,
    IN  HSSECALL    hSSECall,
    OUT RvBool *    possible,
    OUT int *       parameter)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvCIForcedReleaseInd");

    epUtilsLog(call->ep, call, "Received call intrusion forced release indication");

    *possible = call->ep->cfg.bH45011_isPossible;
    *parameter = RV_PVT_INVALID_NODEID;

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCIWOBInd(
    IN  HSSEAPPCALL hSSEaCall,
    IN  HSSECALL    hSSECall,
    OUT RvBool *    possible,
    OUT int *       parameter)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvCIWOBInd");

    epUtilsLog(call->ep, call, "Received call intrusion wait on busy indication");

    *possible = call->ep->cfg.bH45011_isPossible;
    *parameter = RV_PVT_INVALID_NODEID;

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCIMakeSilentInd(
    IN HSSEAPPCALL            hSSEaCall,
    IN HSSECALL               hSSECall)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvCIMakeSilentInd");

    cmCallAnswer(call->hsCall);

    epUtilsEvent(call->ep, "Catch", call, "Call:AddRemark %d monitoring", call->id);

    return (int)ssDefaultAck;
}


static int RVCALLCONV sseEvCINotificationInd(
    IN HSSEAPPCALL  hSSEaCall,
    IN HSSECALL     hSSECall)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvCINotificationInd");

    epUtilsLog(call->ep, call, "Received call intrusion notification indication");

    return (int)ssDefaultAck;
}


static void RVCALLCONV sseEvCIRequestConfirm(
    IN  HSSEAPPCALL     hSSEaCallPri,
    IN  HSSECALL        hSSECallPri,
    IN  int             errCode,
    IN  int             ok)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvCIRequestConfirm.%d.%d", ok, errCode);

    epUtilsLog(call->ep, call, "Received call intrusion Request confirm. errCode:%d, ok:%d", errCode, ok);
}


static void RVCALLCONV sseEvCISilentMonitorConfirm(
    IN  HSSEAPPCALL     hSSEaCallPri,
    IN  HSSECALL        hSSECallPri,
    IN  int             errCode,
    IN  int             ok)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvCISilentMonitorConfirm.%d.%d", ok, errCode);

    epUtilsLog(call->ep, call, "Received call intrusion Silent Monitor confirm. errCode:%d, ok:%d", errCode, ok);
}


static void RVCALLCONV sseEvCIWOBRequestConfirm(
    IN  HSSEAPPCALL     hSSEaCallPri,
    IN  HSSECALL        hSSECallPri,
    IN  int             errCode,
    IN  int             ok)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvCIWOBRequestConfirm.%d.%d", ok, errCode);

    epUtilsLog(call->ep, call, "Received call intrusion WOB Request confirm. errCode:%d, ok:%d", errCode, ok);
}


static void RVCALLCONV sseEvCIForcedReleaseConfirm(
    IN  HSSEAPPCALL     hSSEaCallPri,
    IN  HSSECALL        hSSECallPri,
    IN  int             errCode,
    IN  int             ok)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvCIForcedReleaseConfirm.%d.%d", ok, errCode);

    epUtilsLog(call->ep, call, "Received call intrusion Forced Release confirm. errCode:%d, ok:%d", errCode, ok);
}


static void RVCALLCONV sseEvCIIsolatedConfirm(
    IN  HSSEAPPCALL     hSSEaCallPri,
    IN  HSSECALL        hSSECallPri,
    IN  int             errCode,
    IN  int             ok)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);

    epUtilsEvent(call->ep, "H450", call, "sseEvCIForcedReleaseConfirm.%d.%d", ok, errCode);

    epUtilsLog(call->ep, call, "Received call intrusion Isolated confirm. errCode:%d, ok:%d", errCode, ok);
}


static int RVCALLCONV sseEvCIFindService(
    IN  HSSEAPPCALL  hSSEaCall,
    IN  HSSECALL     hSSECall,
    OUT HSSECALL*    oldHSSECall)
{
    EpCallObj *call = (EpCallObj *)hSSEaCall;
    EpCallObj *oldCall;

    RV_UNUSED_ARG(hSSECall);

    epUtilsEvent(call->ep, "H450", call, "sseEvCIFindService");

    lepLockEpForHash(call->ep, oldCall = (EpCallObj *)epHashGet(call->ep->calls, call->ep->intrudedCallId));
    if (oldCall == NULL)
        return RV_OK;

    *oldHSSECall = oldCall->hSSECall;

    epUtilsLog(call->ep, call, "Received call intrusion conference join to call %d", oldCall->id);
    epUtilsEvent(call->ep, "Catch", call, "$tmp(h450Tab).service.intr.callHandle configure -text \"\"");

    return RV_OK;
}


static void RVCALLCONV sseEvCMNInfo(
        IN HSSEAPPCALL      hSSEaCallPri,
        IN HSSECALL         hSSECallPri,
        IN int              parameters,
        IN int              ok)
{
    EpCallObj *call = (EpCallObj *)hSSEaCallPri;

    RV_UNUSED_ARG(hSSECallPri);
    RV_UNUSED_ARG(parameters);

    epUtilsEvent(call->ep, "H450", call, "sseEvCMNInfo.%d", ok);

    if (ok)
        epUtilsEvent(call->ep, "Catch", call, "after 400 {Call:AddRemark %d Informed}", call->id);
    else
        epUtilsLog(call->ep, call, "No response on CMN");
}






/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * epH450Init
 * ----------------------------------------------------------------------------
 * General: Initialize the H.450 module.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         configFilename   - Name of configuration file to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450Init(
    IN EpObj        *ep,
    IN const RvChar *configFilename)
{
    SSSECALLEVENT sseCallEvent;
    RvInt32 maxCalls = -1;
    RvStatus status;

    /* See how many calls we're supposed to support by the configuration */
    ciGetValue(cmGetConfiguration(ep->hApp), "h450system.maxCalls", NULL, &maxCalls);
    if (maxCalls < 0)
        maxCalls = 100;

    /* Initializing the H450 service */
    status = sseInit(configFilename, &ep->hSSEApp, (HSSEAPPAPP)ep, ep->hApp, maxCalls);
    if (status < 0)
        return status;

    /* Setting the Callback functions */
    memset(&sseCallEvent, 0, sizeof(sseCallEvent));
    sseCallEvent.sseEvCallMake                 = sseEvCallMake;

    /* H.450-2 */
    sseCallEvent.sseEvCallTransfer             = sseEvCallTransfer;
    sseCallEvent.sseEvCallTransferIdentify     = sseEvCallTransferIdentify;
    sseCallEvent.sseEvCallTransferResponse     = sseEvCallTransferResponse;
    sseCallEvent.sseEvCallTransferSetup        = sseEvCallTransferSetup;

    /* H.450-3 */
    sseCallEvent.sseEvCallReroute              = sseEvCallReroute;
    sseCallEvent.sseEvForwardActivated         = sseEvForwardActivated;
    sseCallEvent.sseEvForwardDeactivated       = sseEvForwardDeactivated;

    /* H.450-4 */
    sseCallEvent.sseEvRemoteHoldInd            = sseEvRemoteHoldInd;
    sseCallEvent.sseEvRemoteHoldConfirm        = sseEvRemoteHoldConfirm;
    sseCallEvent.sseEvRemoteHoldRetrieveInd    = sseEvRemoteHoldRetrieveInd;
    sseCallEvent.sseEvRemoteRetrieveConfirm    = sseEvRemoteRetrieveConfirm;
    sseCallEvent.sseEvNearHoldInd              = sseEvNearHoldInd;
    sseCallEvent.sseEvNearHoldRetrieveInd      = sseEvNearHoldRetrieveInd;

    /* H.450-5 */
    sseCallEvent.sseEvCParkRequestInd          = sseEvCParkRequestInd;
    sseCallEvent.sseEvCPGroupIndication        = sseEvCPGroupIndication;
    sseCallEvent.sseEvCPGroupIndicationConfirm = sseEvCPGroupIndicationConfirm;
    sseCallEvent.sseEvCPPickRequConfirm        = sseEvCPPickRequConfirm;
    sseCallEvent.sseEvCPPickupConfirm          = sseEvCPPickupConfirm;
    sseCallEvent.sseEvCPRequestConfirm         = sseEvCPRequestConfirm;
    sseCallEvent.sseEvCPSetupInd               = sseEvCPSetupInd;
    sseCallEvent.sseEvCPSetupConfirm           = sseEvCPSetupConfirm;
    sseCallEvent.sseEvCPickExeInd              = sseEvCPickExeInd;
    sseCallEvent.sseEvCPickupInd               = sseEvCPickupInd;
    sseCallEvent.sseEvCPickupRequestInd        = sseEvCPickupRequestInd;
    sseCallEvent.sseEvCPickupFindCall          = sseEvCPickupFindCall;

    /* H.450-6 */
    sseCallEvent.sseEvWaitInd                  = sseEvWaitInd;
    sseCallEvent.sseEvWaitTimerExpired         = sseEvWaitTimerExpired;

    /* H.450-7 */
    sseCallEvent.sseEvMWIActivateConfirm       = sseEvMWIActivateConfirm;
    sseCallEvent.sseEvMWIActivateInd           = sseEvMWIActivateInd;
    sseCallEvent.sseEvMWIDeactivateConfirm     = sseEvMWIDeactivateConfirm;
    sseCallEvent.sseEvMWIDeactivateInd         = sseEvMWIDeactivateInd;
    sseCallEvent.sseEvMWIInterrogateConfirm    = sseEvMWIInterrogateConfirm;
    sseCallEvent.sseEvMWIInterrogateInd        = sseEvMWIInterrogateInd;

    /* H.450-8 */
    sseCallEvent.sseEvNIAlertingNameInd        = sseEvNIAlertingNameInd;
    sseCallEvent.sseEvNIBusyNameInd            = sseEvNIBusyNameInd;
    sseCallEvent.sseEvNICallingNameInd         = sseEvNICallingNameInd;
    sseCallEvent.sseEvNIConnectedNameInd       = sseEvNIConnectedNameInd;

    /* H.450-9 */
    sseCallEvent.sseEvServiceHandleCreated     = sseEvServiceHandleCreated;
    sseCallEvent.sseEvServiceHandleIdle        = sseEvServiceHandleIdle;
    sseCallEvent.sseEvCCFindService            = sseEvCCFindService;
    sseCallEvent.sseEvCCRequestInd             = sseEvCCRequestInd;
    sseCallEvent.sseEvCCExecPossibleInd        = sseEvCCExecPossibleInd;
    sseCallEvent.sseEvCCStageInd               = sseEvCCStageInd;

    /* H.450-10 */
    sseCallEvent.sseEvCallOfferInd             = sseEvCallOfferInd;
    sseCallEvent.sseEvCfbOverrideInd           = sseEvCfbOverrideInd;
    sseCallEvent.sseEvRemoteUserAlertingInd    = sseEvRemoteUserAlertingInd;

    /* H.450-11 */
    sseCallEvent.sseEvCIInitialRequestInd      = sseEvCIInitialRequestInd;
    sseCallEvent.sseEvCIForcedReleaseConfirm   = sseEvCIForcedReleaseConfirm;
    sseCallEvent.sseEvCIForcedReleaseInd       = sseEvCIForcedReleaseInd;
    sseCallEvent.sseEvCIIsolatedConfirm        = sseEvCIIsolatedConfirm;
    sseCallEvent.sseEvCIIsolatedInd            = sseEvCIIsolatedInd;
    sseCallEvent.sseEvCIMakeSilentInd          = sseEvCIMakeSilentInd;
    sseCallEvent.sseEvCINotificationInd        = sseEvCINotificationInd;
    sseCallEvent.sseEvCIRequestConfirm         = sseEvCIRequestConfirm;
    sseCallEvent.sseEvCISilentMonitorConfirm   = sseEvCISilentMonitorConfirm;
    sseCallEvent.sseEvCIWOBInd                 = sseEvCIWOBInd;
    sseCallEvent.sseEvCIWOBRequestConfirm      = sseEvCIWOBRequestConfirm;
    sseCallEvent.sseEvCIFindService            = sseEvCIFindService;
    sseCallEvent.sseEvCMNInfo                  = sseEvCMNInfo;

    status = sseSetCallEventHandler(ep->hSSEApp, &sseCallEvent, sizeof(sseCallEvent));
    if (status < 0)
    {
        sseEnd(ep->hSSEApp);
        return status;
    }

    ep->completionServices = epHashInit(ep, sizeof(CallCompletionStruct));

    {
        HSSAPP hssApp = sseGetSSHandle(ep->hSSEApp);
        HPVT hVal = ssGetValTree(hssApp);
        RvPvtNodeId tmpNodeId;

        ep->parkedCallsSyn = pstConstruct(ssGetH450Syntax(), "GroupIndOnSeq");
        ep->parkedCallsNodeId = pvtAddRoot(hVal, ep->parkedCallsSyn, 0, NULL);

        /* Initialize the CMN node */
        ep->cmnNodeId = pvtAddRoot(hVal, ssGetSynTreeByRootName(hssApp, "cmnArg"), 0, NULL);

        tmpNodeId = pvtAdd(hVal, ep->cmnNodeId, __h450(featureList), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCFreRoutingSupported), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCTreRoutingSupported), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCCBSPossible), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCCNRPossible), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCOSupported), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCIForcedReleaseSupported), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCIIsolationSupported), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCIWaitOnBusySupported), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCISilentMonitoringSupported), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCIConferenceSupported), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCHFarHoldSupportedssCHFarSupported), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssMWICallbackSupported), 0, NULL, NULL);

        tmpNodeId = pvtAdd(hVal, ep->cmnNodeId, __h450(featureValues), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCIprotectionLevel), 0, NULL, NULL);
        tmpNodeId = pvtAdd(hVal, tmpNodeId, __h450(partyCategory), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(unknown), 0, NULL, NULL);

        tmpNodeId = pvtAdd(hVal, ep->cmnNodeId, __h450(featureControl), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCHDoNotHold), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCTDoNotTransfer), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssMWICallbackCall), 0, NULL, NULL);
        pvtAdd(hVal, tmpNodeId, __h450(ssCISilentMonitorPermitted), 0, NULL, NULL);
    }

    status = sseWatchdogInit(ep->hSSEApp, ep->hApp);
    if (status < 0)
    {
        sseEnd(ep->hSSEApp);
        return status;
    }

    ep->hH450PduPst = pstConstruct(ssGetH450Syntax(), "H4501SupplementaryService");

    return RV_OK;
}


/******************************************************************************
 * epH450End
 * ----------------------------------------------------------------------------
 * General: Destruct the H.450 module of the endpoint.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450End(
    IN EpObj    *ep)
{
    HSSAPP hssApp;
    HPVT hVal;

    if (ep->hSSEApp == NULL)
        return RV_OK;

    pstDestruct(ep->hH450PduPst);

    hssApp = sseGetSSHandle(ep->hSSEApp);
    hVal = ssGetValTree(hssApp);

    if (ep->cfg.H4505_parkGroup != NULL)
    {
        ep->cb.epFree(ep, ep->cfg.H4505_parkGroup);
        ep->cfg.H4505_parkGroup = NULL;
    }

    pvtDelete(hVal, ep->parkedCallsNodeId);
    ep->parkedCallsNodeId = RV_PVT_INVALID_NODEID;
    pvtDelete(hVal, ep->cmnNodeId);
    ep->cmnNodeId = RV_PVT_INVALID_NODEID;

    pstDestruct(ep->parkedCallsSyn);

    sseEnd(ep->hSSEApp);
    epHashEnd(ep->completionServices);
    ep->hSSEApp = NULL;

    return RV_OK;
}


/******************************************************************************
 * epH450Version
 * ----------------------------------------------------------------------------
 * General: Get the version string of the H.450 module used.
 *
 * Return Value: Version string.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
const RvChar *epH450Version(
    IN EpObj    *ep)
{
    static RvChar h450versionStr[16];

    if (ep->hSSEApp != NULL)
    {
        ssGetVersion(h450versionStr, sizeof(h450versionStr));
        return h450versionStr;
    }
    else
        return "0";
}


/******************************************************************************
 * epH450Start
 * ----------------------------------------------------------------------------
 * General: Start using the H.450 module.
 *          This function is used to set the listening addresses to notify to
 *          external entities.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450Start(
    IN EpObj    *ep)
{
    if (ep->hSSEApp != NULL)
    {
        cmTransportAddress addr;
        RvChar addrStr[128];

        /* Setting the default local address */
        cmGetLocalCallSignalAddress(ep->hApp, &addr);
        convertAddrToStr(&addr, addrStr);

        sseSetDefaultAddress(ep->hSSEApp, addrStr);
    }

    return RV_OK;
}


/******************************************************************************
 * epH450Stop
 * ----------------------------------------------------------------------------
 * General: Stop using the H.450 module.
 *          This function is used to clean used PVT resources.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450Stop(
    IN EpObj    *ep)
{
    if (ep->hSSEApp != NULL)
        ssSetDefaultAddress(sseGetSSHandle(ep->hSSEApp), RV_PVT_INVALID_NODEID);

    return RV_OK;
}


/******************************************************************************
 * epH450CallStateChanged
 * ----------------------------------------------------------------------------
 * General: Indication that a call's state has changed.
 *          This function is called from the implementation of
 *          cmEvCallStateChanged().
 *          We don't pass the state and stateMode parameters as we already have
 *          them stored in the call's object.
 *
 * Return Value: Version string.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 * Output: None.
 *****************************************************************************/
void epH450CallStateChanged(
    IN EpCallObj    *call)
{
    HSSECALL hSSECall = call->hSSECall;
    cmCallState_e state = call->state;
    cmCallStateMode_e stateMode = call->stateMode;

    if (hSSECall != NULL)
    {
        lepUnlockCallForAPI(call, sseCallStateChanged(hSSECall, state, stateMode));
    }
}


/******************************************************************************
 * epH450CallIncomingMessage
 * ----------------------------------------------------------------------------
 * General: Notification of an incoming H.450 message that needs to be
 *          processed.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         msgType      - Type of the received Q931 message.
 *         nodeId       - Root node of the H.450 message.
 *         size         - Size of the message.
 * Output: None.
 *****************************************************************************/
void epH450CallIncomingMessage(
    IN EpCallObj            *call,
    IN cmCallQ931MsgType    msgType,
    IN int                  nodeId,
    IN int                  size)
{
    epUtilsLog(call->ep, call, "<H450 message received>");

    if (call->ep->cfg.bH450_countPdu)
    {
        countH450Apdu(call->ep, nodeId);
    }

    if (call->hSSECall != NULL)
    {
        HSSECALL hSSECall = call->hSSECall;
        lepUnlockCallForAPI(call, sseCallH450SupplServ(hSSECall, msgType, nodeId, size));
    }
    else
    {
        epUtilsLog(call->ep, call, "<NO H450 OBJECT - H450 MESSAGE IGNORED>");
    }
}


/******************************************************************************
 * epH450CallInit
 * ----------------------------------------------------------------------------
 * General: Initialize the H.450 part of a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallInit(
    IN EpCallObj    *call)
{
    RvStatus status;
    EpObj *ep;

    ep = call->ep;
    call->h450outCall = -1;
    call->bParkNotifiedGroup = RV_FALSE;

    if (ep->hSSEApp == NULL)
        return RV_OK;

    status = sseCreateCall(ep->hSSEApp, &call->hSSECall, (HSSEAPPCALL)call, call->hsCall);
    if (status < 0)
    {
        call->hSSECall = NULL;
        return status;
    }

    sseCallImplementForward(call->hSSECall);
    sseCallImplementHold(call->hSSECall);
    sseCallImplementMWIServed(call->hSSECall);
    sseCallImplementMWIMsgCtr(call->hSSECall);
    sseCallImplementNI(call->hSSECall);
    sseCallImplementTransfer(call->hSSECall, 0);
    sseCallImplementCallOffer(call->hSSECall);
    sseCallImplementCompletion(call->hSSECall);
    sseCallImplementIntrusion(call->hSSECall);
    sseCallImplementParkPickup(call->hSSECall);
    if (!ep->cfg.bH4504_disableWait)
        sseCallImplementWait(call->hSSECall);
    if (RV_PVT_NODEID_IS_VALID(ep->cmnNodeId))
        sseCallImplementCMN(call->hSSECall, ep->cmnNodeId);

    return RV_OK;
}


/******************************************************************************
 * epH450CallEnd
 * ----------------------------------------------------------------------------
 * General: Kill the H.450 part of a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallEnd(
    IN EpCallObj    *call)
{
    if (call->hSSECall != NULL)
    {
        if (call->bParkNotifiedGroup)
        {
            char callIdentifier[16];
            RvInt32 size = (RvInt32)sizeof(callIdentifier);

            cmCallGetParam(call->hsCall, cmParamCallID, 0, &size, (char*)callIdentifier);
            pickupNotifyGroup(call->ep, call, callIdentifier, RV_FALSE, 0);
            call->bParkNotifiedGroup = RV_FALSE;
        }

        sseCallClose(call->hSSECall);
        call->hSSECall = NULL;
    }
    return RV_OK;
}


/******************************************************************************
 * epH450CallTransfer
 * ----------------------------------------------------------------------------
 * General: H450.2: Call transfer request.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 *         secondaryCall    - Secondary call if consultation.
 *         destination      - Destination address if blind.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallTransfer(
    IN EpCallObj    *call,
    IN EpCallObj    *secondaryCall,
    IN const RvChar *destination)
{
    RvStatus status = RV_ERROR_BADPARAM;

    if (call->hSSECall == NULL)
        return RV_ERROR_NOTSUPPORTED;

    if ((destination == NULL) && (secondaryCall != NULL))
    {
        /* This is consultation */
        if (secondaryCall->hSSECall == NULL)
            return RV_ERROR_NOTSUPPORTED;

        epUtilsEvent(call->ep, "Rec", NULL, "transferCall entry= %d secondary= %d", call->id, secondaryCall->id);

        /* and go a-go-go */
        status = sseCallTransfer(call->hSSECall, secondaryCall->hSSECall, NULL);
    }
    else
    {
        /* we have a dest address, this is a blind transfer */
        /* Cosmetics the dest Address, if needed */
        RvChar destAddr[512];
        convertToH450Address(destination, destAddr);

        epUtilsEvent(call->ep, "Rec", NULL, "transferCall entry= %d to= \"@DEST@%s\"", call->id, destAddr);

        /* and go a-go-go */
        status = sseCallTransfer(call->hSSECall, NULL, destAddr);
    }

    if (status < 0)
        epUtilsError(call->ep, call, "Cannot initiate Call Transfer service (probably problem with the call)");

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epH450CallTransferResponse
 * ----------------------------------------------------------------------------
 * General: H450.2: Call transfer asynchronous response.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 *         opcode           - Opcode of the response
 *         errCode          - 0 for no error, other for error response.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallTransferResponse(
    IN EpCallObj    *call,
    IN RvInt        opcode,
    IN RvInt        errCode)
{
    RvStatus status;

    epUtilsEvent(call->ep, "Rec", NULL, "transferResponse entry= %d type= %d errCode= %d", call->id, opcode, errCode);

    status = sseCallTransferResponse(call->hSSECall, (ctPrimitivesEnum)opcode, (errCode == 0), errCode, RV_PVT_INVALID_NODEID);

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epH450CallReroute
 * ----------------------------------------------------------------------------
 * General: H450.3: Call reroute request.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 *         destination      - Destination address.
 *         rerouteReason    - Rerouting reason to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallReroute(
    IN EpCallObj        *call,
    IN const RvChar     *destination,
    IN reason_e         rerouteReason)
{
    static const RvChar *rerouteReasonStr[] = {"unknown", "cfu", "cfb", "cfnr"};
    RvStatus status;
    RvChar destAddr[512];
    RvChar locAddr[128];
    RvChar srcAddr[600];
    cmTransportAddress csAddr;
    RvInt32 len;

    /* Cosmetics the dest (deflect to) Address, if needed */
    convertToH450Address(destination, destAddr);

    epUtilsEvent(call->ep, "Rec", NULL, "rerouteCall entry= %d type= \"%s\" to= \"@DEST@%s\"",
        call->id, rerouteReasonStr[(int)rerouteReason], destAddr);

    /* Get local address */
    cmGetLocalCallSignalAddress(call->ep->hApp, &csAddr);
    convertAddrToStr(&csAddr, locAddr);

    /* Get calling EP address or alias */
    len = (RvInt32)sizeof(csAddr);
    if (cmCallGetParam(call->hsCall, cmParamSrcCallSignalAddress, 0, &len, (char *)&csAddr) < 0)
    {
        cmAlias srcAlias;
        RvChar strData[513];
        srcAlias.string = strData;
        srcAlias.length = (RvUint16)sizeof(strData);
        len = (RvInt32)sizeof(srcAlias);
        cmCallGetParam(call->hsCall, cmParamSourceAddress, 0, &len, (char *)&srcAlias);
        Alias2String(&srcAlias, srcAddr);
    }
    else
        convertAddrToStr(&csAddr, srcAddr);

    /* must send alerting/call-proceeding before invocation */
    if ((rerouteReason == cfnr_r) || (rerouteReason == unknown_r))
        cmCallAccept(call->hsCall);
    else
        cmCallSendCallProceeding(call->hsCall);

    /* and go a-go-go */
    status = sseCallReroute(call->hSSECall, rerouteReason, destAddr, locAddr, srcAddr);

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epH450ForwardingRequest
 * ----------------------------------------------------------------------------
 * General: H450.3: Activation/deactivation request.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         destination      - Destination address.
 *         fwdCond          - Forwarding condition.
 *         isActivate       - RV_TRUE for activation, RV_FALSE for deactivation.
 * Output: None.
 *****************************************************************************/
RvStatus epH450ForwardingRequest(
    IN EpObj            *ep,
    IN const RvChar     *destination,
    IN proc_e           fwdCond,
    IN RvBool           isActivate)
{
    static const RvChar *fwdCondStr[] = {"cfu", "cfb", "cfnr"};
    EpCallObj *call;
    RvStatus status;
    RvChar destAddr[512];
    char gkAddr[128];
    cmTransportAddress csAddr;

    /* Cosmetics the dest (forward to) Address, if needed */
    convertToH450Address(destination, destAddr);

    epUtilsEvent(ep, "Rec", NULL, "forwardingRequest type= \"%s\" to= \"@DEST@%s\" activate= %d",
        fwdCondStr[(int)fwdCond], destAddr, isActivate);

    /* Get the Server address */
    cmGetGKCallSignalAddress(ep->hApp, &csAddr);
    convertAddrToStr(&csAddr, gkAddr);

    /* Make a call to send with */
    call = createUnmarkedCall(ep);
    if (call == NULL)
        return RV_ERROR_UNKNOWN;

    sseCallImplementForward(call->hSSECall);

    /* send forward request */
    if (isActivate)
        status = sseForwardActivate(call->hSSECall, destAddr, fwdCond, (char *)"", (char *)"", gkAddr);
    else
        status = sseForwardDeactivate(call->hSSECall, fwdCond, (char*)"", destAddr, gkAddr);

    if (status > 0)
        status = RV_OK;
    return RV_OK;
}


/******************************************************************************
 * epH450ForwardInterrogate
 * ----------------------------------------------------------------------------
 * General: H450.3: Interrogation request.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         fwdCond          - Forwarding condition.
 * Output: None.
 *****************************************************************************/
RvStatus epH450ForwardInterrogate(
    IN EpObj            *ep,
    IN proc_e           fwdCond)
{
    static const RvChar *fwdCondStr[] = {"cfu", "cfb", "cfnr"};
    EpCallObj *call;
    RvStatus status;
    RvChar destAddr[512];
    char gkAddr[128];
    char locAddr[128];
    cmTransportAddress csAddr;

    epUtilsEvent(ep, "Rec", NULL, "interrogateRequest type= \"%s\" to= \"@DEST@%s\"",
        fwdCondStr[(int)fwdCond], destAddr);

    /* Get local address */
    cmGetLocalCallSignalAddress(ep->hApp, &csAddr);
    convertAddrToStr(&csAddr, locAddr);

    /* Get the Server address */
    cmGetGKCallSignalAddress(ep->hApp, &csAddr);
    convertAddrToStr(&csAddr, gkAddr);

    /* Make a call to send with */
    call = createUnmarkedCall(ep);
    if (call == NULL)
        return RV_ERROR_UNKNOWN;

    sseCallImplementForward(call->hSSECall);

    /* send forward request */
    status = sseForwardInterogate(call->hSSECall, locAddr, fwdCond, locAddr, gkAddr);

    if (status > 0)
        status = RV_OK;
    return RV_OK;
}


/******************************************************************************
 * epH450CallHold
 * ----------------------------------------------------------------------------
 * General: H450.4: Initiate the call hold service.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         isNear       - RV_TRUE for near hold, RV_FALSE for far hold.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallHold(
    IN EpCallObj    *call,
    IN RvBool       isNear)
{
    RvStatus status;

    epUtilsEvent(call->ep, "Rec", NULL, "holdCall entry= %d near= %d", call->id, isNear);

    status = sseCallHoldInit(call->hSSECall, isNear);

    if (status < 0)
    {
        epUtilsError(call->ep, call, "Couldn't initiate Call Hold service (probably wrong state)");
        return status;
    }

    if (isNear)
        epUtilsEvent(call->ep, "Catch", call, "Call:AddRemark %d NearHold", call->id);

    return RV_OK;
}


/******************************************************************************
 * epH450CallHoldRetrieve
 * ----------------------------------------------------------------------------
 * General: H450.4: Retrieve a held call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         isNear       - RV_TRUE for near hold, RV_FALSE for far hold.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallHoldRetrieve(
    IN EpCallObj    *call,
    IN RvBool       isNear)
{
    RvStatus status;

    epUtilsEvent(call->ep, "Rec", NULL, "holdRetrieveCall entry= %d near= %d", call->id, isNear);

    status = sseCallHoldRetrieve(call->hSSECall, isNear);

    if (status < 0)
    {
        epUtilsError(call->ep, call, "Couldn't retrieve hold service (do you have one?)");
        return status;
    }

    if (isNear)
        epUtilsEvent(call->ep, "Catch", call, "Call:RemoveRemark %d", call->id);

    return RV_OK;
}


/******************************************************************************
 * epH450CallHoldSendResponse
 * ----------------------------------------------------------------------------
 * General: H450.4: Send a response on a hold request. Used when we have a
 *          non-default response to handle.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         isHold       - Is this a response for hold or for retrieve?
 *         isOk         - RV_TRUE for ack, RV_FALSE for nack.
 *         errCode      - Error code to use if isOk==RV_FALSE.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallHoldSendResponse(
    IN EpCallObj    *call,
    IN RvBool       isHold,
    IN RvBool       isOk,
    IN RvInt        errCode)
{
    RvStatus status;

    epUtilsEvent(call->ep, "Rec", NULL, "holdResponseCall entry= %d hold= %d errCode= %d", call->id, isHold, (isOk ? 0 : errCode));

    if (isOk)
    {
        if (isHold)
            epUtilsEvent(call->ep, "Catch", call, "after idle {Call:AddRemark %d FarHeld}", call->id);
        else
            epUtilsEvent(call->ep, "Catch", call, "after idle {Call:RemoveRemark %d}", call->id);
    }

    status = sseHoldSendNonDefaultResponse(call->hSSECall, isOk, errCode);

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epH450CallPark
 * ----------------------------------------------------------------------------
 * General: H450.5: Initiates parking service.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         parkAddress  - Parking address used.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallPark(
    IN EpCallObj    *call,
    IN const RvChar *parkAddress)
{
    RvStatus status;
    cmTransportAddress csAddr;
    RvChar locAddr[128];
    RvChar callAddr[128];
    RvChar parkAddr[600];
    RvInt32 addrLen = (RvInt32)sizeof(csAddr);

    cmGetLocalCallSignalAddress(call->ep->hApp, &csAddr);
    convertAddrToStr(&csAddr, locAddr);

    cmCallGetParam(call->hsCall, cmParamCallSignalAddress, 0, &addrLen, (RvChar *)&csAddr);
    convertAddrToStr(&csAddr, callAddr);

    convertToH450Address(parkAddress, parkAddr);

    epUtilsEvent(call->ep, "Rec", NULL, "callPark entry= %d to= \"@DEST@%s\"", call->id, parkAddr);

    status = sseCallParkInit(call->hSSECall, locAddr, callAddr, parkAddr, 1);
    if (status > 0)
        status = RV_OK;

    if (status == RV_OK)
        epUtilsEvent(call->ep, "Catch", call, "Call:AddRemark %d parking", call->id);

    return status;
}


/******************************************************************************
 * epH450CallPickup
 * ----------------------------------------------------------------------------
 * General: H450.5: Initiates pick up service.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         call             - Call object to use. NULL for remote.
 *         parkIndex        - Parking index used for remote pickup.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallPickup(
    IN EpObj        *ep,
    IN EpCallObj    *call,
    IN RvInt        parkIndex)
{
    RvStatus status;
    cmTransportAddress csAddr;
    RvChar locAddr[128];
    RvChar callId[16];

    cmGetLocalCallSignalAddress(ep->hApp, &csAddr);
    convertAddrToStr(&csAddr, locAddr);

    if (call != NULL)
    {
        RvInt32 len = (RvInt32)sizeof(callId);

        epUtilsEvent(ep, "Rec", NULL, "callPickup entry= %d", call->id);

        cmCallGetParam(call->hsCall, cmParamCallID, 0, &len, (RvChar *)callId);

        status = sseCallLocalPickupInit(call->hSSECall, callId, locAddr);
    }
    else
    {
        HPVT hVal;
        RvPvtNodeId nodeId;
        RvPvtNodeId tmpNodeId;
        RvChar strData[513];
        RvChar callAddr[600];
        RvChar parkAddr[600];
        cmAlias alias;
        RvInt32 position;
        alias.string = strData;
        alias.length = (RvUint16)sizeof(strData);

        epUtilsEvent(ep, "Rec", NULL, "callPickup index= %d", parkIndex);

        /* Get all the necessary parameters */
        hVal = ssGetValTree(sseGetSSHandle(ep->hSSEApp));
        nodeId = pvtGetByIndex(hVal, ep->parkedCallsNodeId, parkIndex, NULL);

        if (!RV_PVT_NODEID_IS_VALID(nodeId))
            return nodeId;

        __pvtGetNodeIdByFieldIds(tmpNodeId, hVal, nodeId, {_h450(callPickupId) _h450(guid) LAST_TOKEN});
        if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
            pvtGetString(hVal, tmpNodeId, (RvInt32)sizeof(callId), (RvChar*)callId);
        else
            memset(callId, 0, sizeof(callId));

        if (cmVt2Alias(hVal, &alias, pvtGetNodeIdByPath(hVal, nodeId, "partyToRetrieve.destinationAddress.1")) >= 0)
            Alias2String(&alias, callAddr);

        if (cmVt2Alias(hVal, &alias, pvtGetNodeIdByPath(hVal, nodeId, "retrieveAddress.destinationAddress.1")) >= 0)
            Alias2String(&alias, parkAddr);

        if (strncmp(parkAddr, "TNAME:", 6) == 0)
        {
            int i, len = strlen(parkAddr)-2;
            parkAddr[1] = 'A';
            for (i=2; i<len; i++)
            {
                parkAddr[i] = parkAddr[i+3];
            }
        }

        pvtGetChildByFieldId(hVal, nodeId, __h450(parkPosition), &position, NULL);

        /* Make a call to send with */
        call = createUnmarkedCall(ep);
        if (call == NULL)
            return RV_ERROR_UNKNOWN;

        ep->pickupSseCall = call->hSSECall;

        status = sseCallPickupInit(call->hSSECall, callId, locAddr, callAddr, parkAddr, position);
        if (status >= 0)
            status = cmCallMake(call->hsCall, 0, 0, parkAddr, locAddr, "picking up call", NULL, 0);

        if (status < 0)
        {
            /* Simulate call closing from the network */
            cmCallDrop(call->hsCall);
        }
    }

    if (status > 0)
        status = RV_OK;

    if (status == RV_OK)
        epUtilsEvent(ep, "Catch", call, "Call:AddRemark %d {Picking Up}", call->id);

    return status;
}


/******************************************************************************
 * epH450GroupNotification
 * ----------------------------------------------------------------------------
 * General: H450.5: Send a group notification on an incoming call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 *         parkPosition     - Parking position to notify about.
 *         inAlerting       - RV_TRUE to handle alerting init.
 * Output: None.
 *****************************************************************************/
RvStatus epH450GroupNotification(
    IN EpCallObj    *call,
    IN RvInt32      parkPosition,
    IN RvBool       inAlerting)
{
    RvStatus status;
    RvChar callId[16];
    RvInt32 len = (RvInt32)sizeof(callId);

    lepUnlockCallForAPI(call, cmCallGetParam(call->hsCall, cmParamCallID, 0, &len, (RvChar *)callId));

    status = pickupNotifyGroup(call->ep, call, callId, RV_TRUE, parkPosition);
    call->bParkNotifiedGroup = RV_TRUE;

    if (inAlerting)
    {
        lepUnlockCallForAPI(call, sseCallAlertParkInt(call->hSSECall));
    }

    return status;
}


/******************************************************************************
 * epH450CallWait
 * ----------------------------------------------------------------------------
 * General: H450.6: Call wait...
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallWait(
    IN EpCallObj    *call)
{
    RvStatus status;

    epUtilsEvent(call->ep, "Rec", NULL, "callWait entry= %d", call->id);

    status = sseCallWaitInit(call->hSSECall);
    if (status < 0)
    {
        epUtilsError(call->ep, call, "Cannot execute Call Waiting for some reason");
        return status;
    }

    epUtilsEvent(call->ep, "Catch", call, "Call:AddRemark %d Wait", call->id);

    return RV_OK;
}


/******************************************************************************
 * epH450MwiActivate
 * ----------------------------------------------------------------------------
 * General: H450.7: Sends MWI activate/deactivate message/callback.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         to           - Served user (destination of call).
 *         isActivate   - Is this an activate or deactivate message?
 *         isCallback   - Are we dealing with the callback?
 * Output: None.
 *****************************************************************************/
RvStatus epH450MwiActivate(
    IN EpObj            *ep,
    IN const RvChar     *to,
    IN RvBool           isActivate,
    IN RvBool           isCallback)
{
    EpCallObj *call;
    RvChar servedUser[600];
    RvStatus status;

    /* Cosmetics the dest (Served User) Address, if needed */
    convertToH450Address(to, servedUser);

    epUtilsEvent(ep, "Rec", NULL, "mwiActivate to= \"@DEST@%s\" activate= %d callback= %d", servedUser, isActivate, isCallback);

    /* Make a call to send with */
    call = createUnmarkedCall(ep);
    if (call == NULL)
        return RV_ERROR_UNKNOWN;

    /* send the request */
    if (isActivate)
    {
        if (isCallback)
        {
            status = sseMWIActivateCallBack(call->hSSECall, servedUser, (char*)"allServices", (char*)"TA:127.0.0.1:1271", NULL, NULL, 0);
        }
        else
        {
            status = sseMWIActivate(call->hSSECall, servedUser, (char*)"allServices", NULL, 1, NULL, NULL, 0);
        }
    }
    else
    {
        status = sseMWIDeactivate(call->hSSECall, servedUser, (char*)"allServices", NULL, isCallback, 1);
    }

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epH450MwiInterrogate
 * ----------------------------------------------------------------------------
 * General: H450.7: Sends interrogation message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         to           - Served user (destination of call).
 * Output: None.
 *****************************************************************************/
RvStatus epH450MwiInterrogate(
    IN EpObj            *ep,
    IN const RvChar     *to)
{
    EpCallObj *call;
    RvChar messageCenter[600];
    RvStatus status;

    /* Cosmetics the dest (Served User) Address, if needed */
    convertToH450Address(to, messageCenter);

    epUtilsEvent(ep, "Rec", NULL, "mwiInterrogate to= \"@DEST@%s\"", messageCenter);

    /* Make a call to send with */
    call = createUnmarkedCall(ep);
    if (call == NULL)
        return RV_ERROR_UNKNOWN;

    /* send forward request */
    status = sseMWIInterrogate(call->hSSECall, messageCenter, (char*)"allServices", NULL, 0, 0);

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epH450MwiSendResponse
 * ----------------------------------------------------------------------------
 * General: H450.7: Sends a non default response to MWI messages.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         isOk         - RV_TRUE for ack, RV_FALSE for nack.
 *         errCode      - Error code to use if isOk==RV_FALSE.
 * Output: None.
 *****************************************************************************/
RvStatus epH450MwiSendResponse(
    IN EpCallObj        *call,
    IN RvBool           isOk,
    IN RvInt            errCode)
{
    RvStatus status;
    interResT garb[1] = { { "allServices", { 0, "", "", "" }, 0, "", "", 9 } };

    epUtilsEvent(call->ep, "Rec", NULL, "mwiSendResponse entry= %d errCode= %d", call->id, (isOk ? 0 : errCode));

    status = sseMWISendNonDefaultResponse(call->hSSECall, isOk, 1, garb, errCode);

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epH450CallSendNameID
 * ----------------------------------------------------------------------------
 * General: H450.8: Sends the name indication.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         operation    - Operation to send the name id on.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallSendNameID(
    IN EpCallObj    *call,
    IN niOperation  operation)
{
    EpConfig *cfg;
    RvStatus status;
    HSSECALL hSSECall = call->hSSECall;

    if (call->hSSECall == NULL)
        return RV_OK;

    cfg = epGetConfig(call->ep);

    if ((int)cfg->H4508_niNameChoice < 0)
        return RV_OK; /* None probably */

    if (operation == connected)
    {
        lepUnlockCallForAPI(call, cmCallAddressComplete(call->hsCall));
    }

    lepUnlockCall(call);
    if (cfg->H4508_niNameChoice == notAvailable)
        status = sseNISend(hSSECall, operation, (char*)"", (char*)"", notAvailable, RV_FALSE);
    else
    {
        if (!cfg->H4508_name[0])
        {
            lepLockCall(call);
            return RV_OK;
        }

        status = sseNISend(hSSECall, operation, cfg->H4508_name, (char*)"", cfg->H4508_niNameChoice, RV_FALSE);
    }
    lepLockCall(call);

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epH450CallCompletion
 * ----------------------------------------------------------------------------
 * General: H450.9: Sends call completion setup message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         destination      - Destination of completion.
 *         originalCall     - Original call being completed. Can be NULL.
 *         isBusy           - Are we busy?
 *         canRetainService - Can we retain the service?
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallCompletion(
    IN EpObj        *ep,
    IN const RvChar *destination,
    IN EpCallObj    *originalCall,
    IN RvBool       isBusy,
    IN RvBool       canRetainService)
{
    CallCompletionStruct *ccInfo;
    EpCallObj *call;
    HSSESERVICE hService;
    RvStatus status;
    RvInt32 id;
    RvChar srcAddr[128];
    RvChar destAddr[600];
    cmTransportAddress locAddr;
    HCALL hsOrigCall = NULL;

    convertToH450Address(destination, destAddr);

    if (originalCall == NULL)
    {
        epUtilsEvent(ep, "Rec", NULL, "callCompletion to= \"@DEST@%s\" busy= %d retain= %d",
            destAddr, isBusy, canRetainService);
    }
    else
    {
        epUtilsEvent(ep, "Rec", NULL, "callCompletion to= \"@DEST@%s\" entry= %d busy= %d retain= %d",
            destAddr, originalCall->id, isBusy, canRetainService);
    }

    /* Create a call for this service */
    call = epCallCreate(ep, NULL, 0);
    if (call == NULL)
        return RV_ERROR_OUTOFRESOURCES;

    /* Ask for an ID for this service */
    lepLockEpForHash(ep, id = ep->cb.epAllocateResourceId(ep, EpResourceCompletionService));
    if (id <= 0)
    {
        /* Call was rejected ??? */
        return RV_ERROR_BADPARAM;
    }

    /* Allocate a service object */
    lepLockEpForHash(ep, status = epHashAdd(ep->completionServices, id, (void**)&ccInfo));
    if (status != RV_OK)
    {
        /* No internal resources ??? */
        cmCallDrop(call->hsCall);
        return status;
    }

    memset(ccInfo, 0, sizeof(*ccInfo));
    ccInfo->id = id;
    ccInfo->ep = ep;
    ccInfo->connectionRetained = RV_TRUE;
    strncpy(ccInfo->destAddr, destAddr, sizeof(ccInfo->destAddr));

    cmGetLocalCallSignalAddress(ep->hApp, &locAddr);
    convertAddrToStr(&locAddr, srcAddr);

    if (originalCall != NULL)
        hsOrigCall = originalCall->hsCall;

    /* Complete the call... */
    hService = sseCallCompletion(call->hSSECall, (HSSEAPPSERVICE)ccInfo, srcAddr,
        ccInfo->destAddr, hsOrigCall, "allServices", isBusy, canRetainService, ccRetentionDontCare);
    if (hService == NULL)
    {
        lepLockEpForHash(ep, epHashRemove(ep->completionServices, ccInfo->id));
        cmCallDrop(call->hsCall);
        return RV_ERROR_OUTOFRESOURCES;
    }
    ccInfo->hService = hService;

    epUtilsEvent(ep, "Catch", NULL, "h4509AddService {%d - Outgoing}", ccInfo->id);

    return RV_OK;
}


/******************************************************************************
 * epH450CallCompletionMessage
 * ----------------------------------------------------------------------------
 * General: H450.9: Sends a call completion message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         destination      - Destination of completion.
 *         originalCall     - Original call being completed. Can be NULL.
 *         isBusy           - Are we busy?
 *         canRetainService - Can we retain the service?
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallCompletionMessage(
    IN EpObj        *ep,
    IN const RvChar *messageStr,
    IN RvInt32      serviceId)
{
    CallCompletionStruct *ccInfo;
    EpCallObj *call;
    RvStatus status = RV_OK;
    HSSECALL hsseCall;
    RvChar srcAddr[128];
    cmTransportAddress locAddr;
    RvChar *pSrcAddr;
    RvBool newCall;

    epUtilsEvent(ep, "Rec", NULL, "callCompletionMessage msg= \"%s\" service= %d", messageStr, serviceId);

    lepLockEpForHash(ep, ccInfo = (CallCompletionStruct *)epHashGet(ep->completionServices, serviceId));

    if (ccInfo == NULL)
        return RV_ERROR_NOT_FOUND;

    if (!ccInfo->connectionRetained || (strcmp("Ringout", messageStr) == 0))
    {
        /* We have to create and use a new call for this one */

        /* Create a call for this service */
        call = epCallCreate(ep, NULL, 0);
        if (call == NULL)
            return RV_ERROR_OUTOFRESOURCES;
        hsseCall = call->hSSECall;
        cmGetLocalCallSignalAddress(ep->hApp, &locAddr);
        convertAddrToStr(&locAddr, srcAddr);
        pSrcAddr = srcAddr;

        newCall = RV_TRUE;
    }
    else
    {
        /* We have a call in SSE already... */
        call = NULL;
        hsseCall = NULL;
        pSrcAddr = NULL;
        newCall = RV_FALSE;
    }

    if (strcmp("Cancel", messageStr) == 0)
    {
        status = sseCallCompletionCancel(ccInfo->hService, hsseCall, pSrcAddr, ccInfo->destAddr);
    }
    else if (strcmp("Resume", messageStr) == 0)
    {
        status = sseCallCompletionResume(ccInfo->hService, hsseCall, pSrcAddr, ccInfo->destAddr);
    }
    else if (strcmp("Ringout", messageStr) == 0)
    {
        status = sseCallCompletionRingout(ccInfo->hService, hsseCall, pSrcAddr, ccInfo->destAddr);
    }
    else if (strcmp("ExecPossible", messageStr) == 0)
    {
        status = sseCallCompletionExecPossible(ccInfo->hService, hsseCall, pSrcAddr, ccInfo->destAddr);
    }

    if (status > 0)
        status = RV_OK;

    if (status < 0)
    {
        if (newCall)
            cmCallDrop(call->hsCall);
    }
    return status;
}


/******************************************************************************
 * epH450CallOffer
 * ----------------------------------------------------------------------------
 * General: H450.10: Sends call offering.
 *
 * Return Value: Call object to dial.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         overrideCFB  - Do we want to override CFB service?
 * Output: None.
 *****************************************************************************/
EpCallObj *epH450CallOffer(
    IN EpObj        *ep,
    IN RvBool       overrideCFB)
{
    EpCallObj *call;
    RvStatus status;

    epUtilsEvent(ep, "Rec", NULL, "callOffer overrideCFB= %d", overrideCFB);

    call = epCallCreate(ep, NULL, 0);
    if (call == NULL)
        return NULL;

    status = sseCallOfferInit(call->hSSECall, overrideCFB);
    if (status < 0)
    {
        cmCallDrop(call->hsCall);
        return NULL;
    }

    return call;
}


/******************************************************************************
 * epH450CallOfferRemoteUserAlerting
 * ----------------------------------------------------------------------------
 * General: H450.10: Sends call offering.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallOfferRemoteUserAlerting(
    IN EpCallObj    *call)
{
    RvStatus status;

    epUtilsEvent(call->ep, "Rec", NULL, "callOfferRemoteUserAlerting entry= %d", call->id);

    status = sseRemoteUserAlerting(call->hSSECall);

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epH450CallIntrusion
 * ----------------------------------------------------------------------------
 * General: H450.11: Sends call intrusion.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         intrusionType    - Type of intrusion to start.
 *         callId           - Call id to monitor if intrusionType==ssCISilentMoitor.
 *         call             - Call to use. Can be NULL.
 * Output: newCall          - Newly created call to dial if any.
 *****************************************************************************/
RvStatus epH450CallIntrusion(
    IN  EpObj               *ep,
    IN  ciPrimitivesEnum    intrusionType,
    IN  RvUint8             *callId,
    IN  EpCallObj           *call,
    OUT EpCallObj           **newCall)
{
    RvStatus status;

    if (call != NULL)
        epUtilsEvent(call->ep, "Rec", NULL, "callIntrusion type= %d entry= %d", intrusionType, call->id);
    else
        epUtilsEvent(ep, "Rec", NULL, "callIntrusion type= %d", intrusionType);

    if (call != NULL)
    {
        /* Prepare a call intrusion invoke apdu to be sent in the setup */
        if (callId == NULL)
            status = sseCallIntrusion2(call->hSSECall, intrusionType);
        else
            status = sseCallIntrusionInit(call->hSSECall, intrusionType, (RvChar *)callId);

        *newCall = NULL;
    }
    else
    {
        /* Seems like we need to create a call on our own */
        call = epCallCreate(ep, NULL, 0);
        if (call == NULL)
            return RV_ERROR_OUTOFRESOURCES;

        /* Prepare a call intrusion invoke apdu to be send in the setup */
        status = sseCallIntrusionInit(call->hSSECall, intrusionType, (RvChar *)callId);

        if (status >= 0)
        {
            if (call->ep->cfg.bAutomaticFaststart && call->ep->cfg.bSupportFaststart)
                epCallGenerateFaststartRequest(call, RV_TRUE);

            if (newCall != NULL)
                *newCall = call;
        }
        else
        {
            cmCallDrop(call->hsCall);

            if (newCall != NULL)
                *newCall = NULL;
        }
    }

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epH450CallCommonInformation
 * ----------------------------------------------------------------------------
 * General: H450.12: Sends a Common Information message on the selected call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         actionType       - Type of action to use.
 *         callId           - Call id to monitor if intrusionType==ssCISilentMoitor.
 *         call             - Call to use. Can be NULL.
 * Output: newCall          - Newly created call to dial if any.
 *****************************************************************************/
RvStatus epH450CallCommonInformation(
    IN  EpCallObj           *call,
    IN  cmnPrimitivesEnum   actionType)
{
    RvStatus status;

    if (call->state == cmCallStateInit)
    {
        epUtilsEvent(call->ep, "Rec", NULL, "newCall entry= %d", call->id);
        epUtilsEvent(call->ep, "Rec", NULL, "setCallOpt entry= %d terminalType= @TAG@msdCall%d", call->id, call->id);
        epUtilsEvent(call->ep, "Tag", NULL, "msdCall%d 0-1", call->id);
        epUtilsEvent(call->ep, "Tag", NULL, "eventRemoval%d OneCall", call->id);
    }
    epUtilsEvent(call->ep, "Rec", NULL, "callCommonInformation entry= %d type= %d", call->id, actionType);

    if (call->hSSECall == NULL)
        return RV_ERROR_BADPARAM;

    status = sseCallCMNInit(call->hSSECall, actionType);

    if (status > 0)
        status = RV_OK;
    return status;
}





/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * createUnmarkedCall
 * ----------------------------------------------------------------------------
 * General: Create a call object, complete with stack handle and H.450, but
 *          without any H.245 on it.
 *
 * Return Value: Call object created, or NULL on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep   - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
static EpCallObj *createUnmarkedCall(
    IN EpObj    *ep)
{
    EpCallObj *call;

    call = epCallCreate(ep, NULL, 0);
    if (call == NULL)
        return NULL;

    call->isIndependent = ep->cfg.bIndependent;
    cmCallSetParam(call->hsCall, cmParamH245Stage, 0, (RvInt32)cmTransH245Stage_noH245, NULL);
    cmCallSetParam(call->hsCall, cmParamEstablishH245, 0, RV_FALSE, NULL);
    cmCallSetIndependentSupplementaryService(call->hsCall);

    return call;
}


/******************************************************************************
 * convertToH450Address
 * ----------------------------------------------------------------------------
 * General: Convert an address represented by a string to an address string
 *          that H450 recognizes.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  inString - String to convert.
 * Output: str      - String returned.
 *****************************************************************************/
static void convertToH450Address(
    IN  const RvChar    *inString,
    OUT RvChar          *str)
{
    if (inString[0] == '[')
    {
        strcpy(str, "TA:");
        strcpy(str + 3, inString);
    }
    else if (isdigit((int)inString[0]))
    {
        /* If it's a transport address, we'll add the TA: prefix...
           Otherwise, we should add the TEL: prefix... */
        int a, b, c, d, e;
        if (sscanf(inString, "%d.%d.%d.%d:%d", &a, &b, &c, &d, &e) == 5)
        {
            strcpy(str, "TA:");
            strcpy(str + 3, inString);
        }
        else
        {
            strcpy(str, "TEL:");
            strcpy(str + 4, inString);
        }
    }
    else
        strcpy(str, inString);
}


/******************************************************************************
 * convertAddrToStr
 * ----------------------------------------------------------------------------
 * General: Convert an address struct into its string form, with a leading
 *          "TA:".
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  addr     - Address to convert.
 * Output: str      - String returned.
 *****************************************************************************/
static void convertAddrToStr(
    IN  cmTransportAddress  *addr,
    OUT RvChar              *str)
{
    strcpy(str, "TA:");
    cmTransportAddress2String(addr, 100, str + 3);
}


/******************************************************************************
 * pickupNotifyGroup
 * ----------------------------------------------------------------------------
 * General: Notify the parking lot group about a call.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         inCall           - Call object causing this issue.
 *         callIdentifier   - Call to notify about.
 *         isOn             - RV_TRUE for an on indication , RV_FALSE for off.
 *         position         - Parking position when isOn==RV_TRUE.
 * Output: None.
 *****************************************************************************/
static RvStatus pickupNotifyGroup(
    IN EpObj                *ep,
    IN EpCallObj            *inCall,
    IN const RvChar         *callIdentifier,
    IN RvBool               isOn,
    IN RvInt                position)
{
    RvStatus status;
    EpCallObj *call;
    cmTransportAddress csAddr;
    RvChar groupAddrStr[540];
    RvChar cvtAddrStr[600];
    RvChar locAddr[128];
    RvChar callAddr[128];
    RvChar *p;
    RvChar *pStart;

    p = ep->cfg.H4505_parkGroup;
    if (p == NULL)
    {
        /* No one to notify to */
        return RV_OK;
    }

    cmGetLocalCallSignalAddress(ep->hApp, &csAddr);
    convertAddrToStr(&csAddr, locAddr);

    if (isOn)
    {
        RvInt32 addrLen = (RvInt32)sizeof(csAddr);

        lepUnlockCallForAPI(inCall, cmCallGetParam(inCall->hsCall, cmParamCallSignalAddress, 0,
            &addrLen, (RvChar *)&csAddr));
        convertAddrToStr(&csAddr, callAddr);
    }
    else
    {
        callAddr[0] = '\0';
    }

    while (*p)
    {
        pStart = p;
        while (*p && (*p != ','))
            p++;
        if ((pStart != p) && ((RvSize_t)(p - pStart) < sizeof(groupAddrStr)))
        {
            memcpy(groupAddrStr, pStart, (RvSize_t)(p - pStart));
            groupAddrStr[p - pStart] = '\0';
            convertToH450Address(groupAddrStr, cvtAddrStr);

            /* Create a new call handle */
            call = epCallCreate(ep, NULL, 0);
            if (call == NULL)
                return RV_ERROR_UNKNOWN;

            if (call->ep->cfg.bAutomaticFaststart && call->ep->cfg.bSupportFaststart)
                epCallGenerateFaststartRequest(call, RV_TRUE);

            if (isOn)
            {
                status = sseCallGroupIndOn(call->hSSECall, callIdentifier, cvtAddrStr, callAddr, locAddr, parkedCall, position);
            }
            else
            {
                status = sseCallGroupIndOff(call->hSSECall, callIdentifier, cvtAddrStr);
            }
            if (status >= 0)
                status = cmCallMake(call->hsCall, 0, 0, cvtAddrStr, locAddr, "Parked Call", NULL, 0);
            if (status < 0)
            {
                cmCallDrop(call->hsCall);
                return status;
            }
        }
        if (*p)
            p++;
    }

    return RV_OK;
}


/******************************************************************************
 * findParkedCall
 * ----------------------------------------------------------------------------
 * General: Notify the parking lot group about a call.
 *
 * Return Value: Index of parked call on success, 0 if not found.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         hVal             - PVT to use.
 *         paramsNodeId     - Indication message to search for.
 * Output: None.
 *****************************************************************************/
static RvInt findParkedCall(
    IN  EpObj       *ep,
    IN  HPVT        hVal,
    IN  RvPvtNodeId paramsNodeId,
    OUT RvPvtNodeId *prkNodeId)
{
    RvInt index;
    RvPvtNodeId nodeId;
    RvPvtNodeId tmpNodeId;
    RvUint8 searchedCallId[16];

    /* Get the call Identifier */
    __pvtGetNodeIdByFieldIds(tmpNodeId, hVal, paramsNodeId, {_h450(callPickupId) _h450(guid) LAST_TOKEN});
    if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
    {
        pvtGetString(hVal, tmpNodeId, (RvInt32)sizeof(searchedCallId), (char *)searchedCallId);
    }
    else
    {
        memset(searchedCallId, 0, sizeof(searchedCallId));
    }

    index = 0;
    nodeId = pvtChild(hVal, ep->parkedCallsNodeId);
    while (RV_PVT_NODEID_IS_VALID(nodeId))
    {
        RvUint8 callId[16];

        index++;

        __pvtGetNodeIdByFieldIds(tmpNodeId, hVal, nodeId, {_h450(callPickupId) _h450(guid) LAST_TOKEN});
        if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
        {
            pvtGetString(hVal, tmpNodeId, (RvInt32)sizeof(callId), (char *)callId);
        }
        else
        {
            memset(callId, 0, sizeof(callId));
        }

        if (memcmp(callId, searchedCallId, sizeof(callId)) == 0)
        {
            *prkNodeId = nodeId;
            return index;
        }

        nodeId = pvtBrother(hVal, nodeId);
    }

    *prkNodeId = RV_PVT_INVALID_NODEID;
    return index;
}


/******************************************************************************
 * countH450Apdu
 * ----------------------------------------------------------------------------
 * General: Count incoming H.450 PDUs that are received.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         nodeId           - Incoming Q931 message to check.
 * Output: None.
 *****************************************************************************/
static void countH450Apdu(
    IN EpObj        *ep,
    IN RvPvtNodeId  nodeId)
{
    RvUint8         buf[2048];
    RvPvtNodeId     rootNodeId;
    RvPvtNodeId     apdu;
    RvPstFieldId    fieldId;
    RvInt32         msgLength;
    int             decodedLength;
    RvStatus        status;

    msgLength = pvtGetString(ep->hVal, nodeId, (RvInt32)sizeof(buf), (RvChar *)buf);
    if (msgLength < 0)
        return;

    rootNodeId = pvtAddRoot(ep->hVal, ep->hH450PduPst, 0, NULL);
    status = cmEmDecode(ep->hVal, rootNodeId, buf, msgLength, &decodedLength);
    if ((status < 0) || (decodedLength != msgLength))
        return;

    __pvtGetNodeIdByFieldIds(apdu, ep->hVal, rootNodeId, {_h450(serviceApdu) _h450(rosApdus) _nul(1) _anyField LAST_TOKEN});
    if (!RV_PVT_NODEID_IS_VALID(apdu))
        return;

    pvtGet(ep->hVal, apdu, &fieldId, NULL, NULL, NULL);
    switch (fieldId)
    {
        case __h450(invoke):
            {
                RvInt32 opcode = 0;
                epUtilsEvent(ep, "Count", NULL, "h450invoke");
                __pvtGetByFieldIds(apdu, ep->hVal, apdu, {_h450(opcode) _h450(local) LAST_TOKEN}, NULL, &opcode, NULL);
                if (RV_PVT_NODEID_IS_VALID(apdu))
                    epUtilsEvent(ep, "Count", NULL, "h450opcode%d", opcode);
            }
            break;

        case __h450(returnResult):
            epUtilsEvent(ep, "Count", NULL, "h450returnResult");
            break;

        case __h450(returnError):
            epUtilsEvent(ep, "Count", NULL, "h450returnError");
            break;

        case __h450(reject):
            epUtilsEvent(ep, "Count", NULL, "h450reject");
            break;

        default:
            break;
    }

    pvtDelete(ep->hVal, rootNodeId);
}




#endif /* USE_H450 */


#ifdef __cplusplus
}
#endif
