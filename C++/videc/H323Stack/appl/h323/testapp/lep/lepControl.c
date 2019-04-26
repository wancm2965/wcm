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
epControl.c

H.245 control handling of the endpoint.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "lepSecurity.h"
#include "lepHash.h"
#include "lepUtils.h"
#include "lepConversions.h"
#include "lepControl.h"
#include "lepRtp.h"
#include "lepFwNat.h"


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




/*-----------------------------------------------------------------------*/
/*                           CALLBACK FUNCTIONS                          */
/*-----------------------------------------------------------------------*/


int RVCALLCONV cmEvCallCapabilities(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      cmCapStruct*        capabilities[])
{
    EpCallObj *call = (EpCallObj *)haCall;
    int i;

    RV_UNUSED_ARG(hsCall);

    if (call->ep->cfg.bScanNodeIDs)
    {
        for (i=0; (i<3) && RV_PVT_NODEID_IS_VALID(capabilities[i]->capabilityHandle); i++)
        {
            epScanPvtNode(call->ep, capabilities[i]->capabilityHandle);
        }
    }

    epUtilsEvent(call->ep, "H245", call, "cmEvCallCapabilities");

    return 0;
}


int RVCALLCONV cmEvCallCapabilitiesExt(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      cmCapStruct***      capabilities[])
{
    EpCallObj *call = (EpCallObj *)haCall;
    int i;

    RV_UNUSED_ARG(haCall);
    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(capabilities);

    if (call->ep->cfg.bScanNodeIDs)
    {
        for (i=0; (i<3) && RV_PVT_NODEID_IS_VALID(capabilities[0][0][i]->capabilityHandle); i++)
        {
            epScanPvtNode(call->ep, capabilities[0][0][i]->capabilityHandle);
        }
    }

    return 0;
}


int RVCALLCONV cmEvCallCapabilitiesResponse(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      RvUint32            status)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);

    epUtilsEvent(call->ep, "H245", call, "cmEvCallCapabilitiesResponse.%d", status);

    return 0;
}


int RVCALLCONV cmEvCallMasterSlaveStatus(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      RvUint32            status)
{
    EpCallObj *call = (EpCallObj *)haCall;
    const RvChar *message;
    RvInt32 termType = -1;

    RV_UNUSED_ARG(hsCall);

    epUtilsEvent(call->ep, "H245", call, "cmEvCallMasterSlaveStatus.%d", status);

    switch (status)
    {
        case cmMSMaster:
            termType = 255;
            message = "MSD: terminal is Master";
            break;
        case cmMSSlave:
            termType = 0;
            message = "MSD: terminal is Slave";
            break;
        case cmMSError:
            message = "MSD: ERROR";
            break;
        default:
            message = "MSD: ???";
    }
    epUtilsLog(call->ep, call, "%s", message);

    if ((termType != -1) && !call->isIncoming)
    {
        epUtilsEvent(call->ep, "Tag", call, "\"msdCall%d\" %d", call->id, termType);
#ifdef USE_H450
        epUtilsEvent(call->ep, "Tag", call, "\"h450msdCall%d\" %d", call->h450outCall, termType);
#endif
    }

    return 0;
}


int RVCALLCONV cmEvCallRoundTripDelay(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      RvInt32             delay)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(delay);

    epUtilsEvent(call->ep, "H245", call, "cmEvCallRoundTripDelay");
    epUtilsLog(call->ep, call, "Round trip delay of %d", delay);

    return 0;
}


int RVCALLCONV cmEvCallUserInput(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      RvInt32             userInputId)
{
    EpCallObj *call = (EpCallObj *)haCall;
    EpObj *ep;
    RvChar display[128];
    RvInt32 len = 128;
    cmNonStandardParam nsData;
    cmUserInputData userData;
    RvPvtNodeId nodeId;
    cmUserInputIndication indication = (cmUserInputIndication)-1;
    cmUserInputSupportIndication supportIndication = (cmUserInputSupportIndication)-1;
    cmUserInputSignalStruct signalStruct;
    RvChar userDataStr[128];

    userData.data = userDataStr;
    userData.length = 128;

    RV_UNUSED_ARG(hsCall);
    
    ep = call->ep;
    nodeId = cmUserInputGetDetail(ep->hApp, userInputId, &indication);

    switch (indication)
    {
    case cmUserInputNonStandard:
    case cmUserInputAlphanumeric:
        if (cmUserInputGet(ep->hApp, userInputId, &nsData.info, display, &len, &userData) >= 0)
        {
            if ((len == 0) && (userData.data[0] == 0x1b) && (userData.data[1] == userData.length - 2))
            {
                /* Seems like we're dealing with a valid alphanumeric GeneralString as should be.
                   Remove the 1st 2 bytes so we get a nice string. You should note that the length
                   indicator here is probably not handled correctly for large strings. */
                userData.length -= 2;
                userData.data = userData.data + 2; /* Skip the BER encoding header */
            }
            else
            {
                /* This means we got a non standard UII or a string and not an alphanumeric
                   GeneralString, as is the case with many applications currently available.
                   For interoperability reasons, we'd rather ignore this problem and handle
                   this fault of the remote terminal properly. */
            }

            if (userData.length > 0)
            {
				epUtilsEvent(ep, "H245", call, "cmEvCallUserInput.%d.%0.20s", indication, userData.data);
                epUtilsLog(ep, call, "UII received: %s", userData.data);
            }
            if (len > 0)
            {
                epUtilsEvent(ep, "H245", call, "cmEvCallUserInput.%d.%0.20s", indication, display);
                epUtilsLog(ep, call, "Display: %s", display);
            }
        }
        break;

    case cmUserInputSupport:
        cmUserInputSupportGet(ep->hApp, nodeId, &supportIndication);
        epUtilsEvent(ep, "H245", call, "cmEvCallUserInput.%d.%d", indication, supportIndication);
        epUtilsLog(ep, call, "UII received: Support type (%d)", supportIndication);
        break;

    case cmUserInputSignal:
        cmUserInputGetSignal(ep->hApp, nodeId, &signalStruct);

        /* Make sure this signal is indicated as others do */
        memset((void *)(userData.data), 0, 128);
        strncpy(userData.data, "SIGNAL", 6);
        userData.data[6] = signalStruct.signalType;

        epUtilsEvent(ep, "H245", call, "cmEvCallUserInput.%d.%c.%d",
            indication, signalStruct.signalType, signalStruct.duration);
        epUtilsLog(ep, call, "UII received: Signal '%c' %d,%d,%d,%d",
            signalStruct.signalType, signalStruct.duration, signalStruct.cmUserInputSignalRtp.timestamp,
            signalStruct.cmUserInputSignalRtp.expirationTime, signalStruct.cmUserInputSignalRtp.logicalChannelNumber);
        break;

    case cmUserInputSignalUpdate:
    case cmUserInputExtendedAlphanumeric:
    case cmUserInputEncryptedAlphanumeric:
    default:
        epUtilsEvent(ep, "H245", call, "cmEvCallUserInput.%d", indication);
        epUtilsLog(ep, call, "UII received: Other type...");
        break;
    }

    return 0;
}


int RVCALLCONV cmEvCallRequestMode(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      cmReqModeStatus     status,
    IN      RvInt32             nodeId)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(nodeId);

    epUtilsEvent(call->ep, "H245", call, "cmEvCallRequestMode.%d", status);

    return 0;
}


int RVCALLCONV cmEvCallMiscStatus(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      cmMiscStatus        status)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);

    epUtilsEvent(call->ep, "H245", call, "cmEvCallMiscStatus.%d", status);

    return 0;
}


int RVCALLCONV cmEvCallControlStateChanged(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      cmControlState      state,
    IN      cmControlStateMode  stateMode)
{
    EpCallObj *call = (EpCallObj *)haCall;
    RvBool origin;
    int i = 0, node = 1;
    RvStatus status;

    epUtilsEvent(call->ep, "H245", call, "cmEvCallControlStateChanged.%d.%d", state, stateMode);

    if (state == cmControlStateFastStart)
    {
        call->isFastConnectChannels = RV_TRUE;

        cmCallGetOrigin(hsCall, &origin);
        if (!origin)
            return 0;

        if ((node = cmFastStartGetByIndex(hsCall, i)) >= 0)
        {
            lepLockCall(call);
            do
            {
                int                         sessionId;
                cmChannelDirection          chanDir;
                cmFastStartChannel          fsChan;

#ifdef USE_FWNAT
                RvBool                      bKeepAliveExpected = RV_FALSE;
#endif
                epSecurityChannelGetEncryptionSync(call, node);
                /*get FS channel info*/
                lepUnlockCallForAPI(call, cmFastStartGet(hsCall, node, &sessionId, &chanDir, &fsChan));
                /*if chanDir == dirBoth and the initiator is a client, keepAlive channel address is expected*/

#ifdef USE_FWNAT
				if (call->ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigClient &&
                    chanDir == dirBoth)
                    bKeepAliveExpected = RV_TRUE;
                /*get h46019 info from ack*/

                /*When a FS call initiator receives FS response, 
                  the direction received from the stack
                  is in relation to the called party - let's fix it*/ 
                if (chanDir == dirReceive || chanDir == dirBoth) 
                    chanDir = dirTransmit;
                else if (chanDir == dirTransmit) 
                    chanDir = dirReceive;
#endif            
                /*get fwnat and mux information*/
                status = epFwNatGetH46019InfoFromMessage(call, NULL, bKeepAliveExpected, node, &fsChan, sessionId, RV_TRUE, chanDir);
                i++;

            } while((node = cmFastStartGetByIndex(hsCall, i)) >= 0);

            lepUnlockCall(call);
        }
    }
    else if (state == cmControlStateFastStartComplete)
    {
        for (i = 0; i < 3; i++)
        {
            if (call->rtpSessions[i] >= 0)
            {
                epRtpSessionClose(call->ep->rtp, call->rtpSessions[i], RV_FALSE);
                call->rtpSessions[i] = -1;
            }
            if (call->muxedRtpSessions[i] >= 0)
            {
                epRtpSessionClose(call->ep->rtp, call->muxedRtpSessions[i], RV_TRUE);
                call->muxedRtpSessions[i] = -1;
            }
        }
        call->isFastConnectChannels = RV_FALSE;
        if (stateMode == cmControlStateModeFastStartRejected)
            call->isFastConnectCall = RV_FALSE;
    }

    return 0;
}


int RVCALLCONV cmEvCallMasterSlave(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      RvUint32            terminalType,
    IN      RvUint32            statusDeterminationNumber)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(terminalType);
    RV_UNUSED_ARG(statusDeterminationNumber);

    epUtilsEvent(call->ep, "H245", call, "cmEvCallMasterSlave");

    return 0;
}


int RVCALLCONV cmEvCallControlMessage(
    IN  HAPPCALL            haCall,
    IN  HCALL               hsCall,
    IN  HAPPCHAN            haChan,
    IN  HCHAN               hsChan,
    IN  RvPvtNodeId         message,
    IN  cmH245MessageType   messageType)
{
    EpCallObj *call = (EpCallObj *)haCall;
    EpChannelObj *chan = (EpChannelObj *)haChan;
    RvInt32 chanId = 0;

    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(hsChan);

    /* this event may be received without a related channel */
    if (chan != NULL)
        chanId = chan->id;

    if (call->ep->cfg.bScanNodeIDs)
        epScanPvtNode(call->ep, message);

    epUtilsEvent(call->ep, "H245", call, "cmEvCallControlMessage.%d.%d", chanId, messageType);

    return 0;
}


int RVCALLCONV cmEvCallTerminalNumberAssign(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      RvUint8             mcuNumber,
    IN      RvUint8             terminalNumber)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);

    epUtilsEvent(call->ep, "H245", call, "cmEvCallTerminalNumberAssign.%d.%d", mcuNumber, terminalNumber);

    return 0;
}





/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * epCallCreateH245
 * ----------------------------------------------------------------------------
 * General: Create a listening H.245 connection for the call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         addr         - Listening address to use. Can be NULL.
 * Output: None.
 *****************************************************************************/
RvStatus epCallCreateH245(
    IN EpCallObj            *call,
    IN cmTransportAddress   *addr)
{
    RvStatus status;

    /* create/listen control session */
    status = cmCallCreateControlSession(call->hsCall, addr);

    epUtilsLog(call->ep, call, "CallCreateControlSession. result=%s", Status2String(status));

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallConnectControl
 * ----------------------------------------------------------------------------
 * General: Connect H.245 control channel for a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallConnectControl(
    IN EpCallObj            *call)
{
    RvStatus status;

    status = cmCallConnectControl(call->hsCall);

    if (status < 0)
    {
        epUtilsError(call->ep, call, "Error connecting control. result=%s", Status2String(status));
    }

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallSeparateControl
 * ----------------------------------------------------------------------------
 * General: Separate H.245 control channel for a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSeparateControl(
    IN EpCallObj            *call)
{
    RvStatus status;

    status = cmCallSw2SeparateH245(call->hsCall);
    if (status < 0)
    {
        epUtilsError(call->ep, call, "Error separating control. result=%s", Status2String(status));
    }

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallCloseControl
 * ----------------------------------------------------------------------------
 * General: Close H.245 control channel for a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallCloseControl(
    IN EpCallObj            *call)
{
    RvStatus status;

    status = cmCallCloseControlSession(call->hsCall);

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallMediaLoopOff
 * ----------------------------------------------------------------------------
 * General: Indicate all channels of a call to stop loopback.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallMediaLoopOff(
    IN EpCallObj            *call)
{
    RvStatus status;

    /* Send LoopOff message */
    status = cmCallMediaLoopOff(call->hsCall);

    epUtilsLog(call->ep, call, "LoopOff. result=%s", Status2String(status));

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallSendCapabilities
 * ----------------------------------------------------------------------------
 * General: Send a TCS message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         nodeId       - RV_PVT_INVALID_NODEID to send the capability set from
 *                        the configuration.
 *         isEmpty      - RV_TRUE to send an empty capability set.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSendCapabilities(
    IN EpCallObj            *call,
    IN RvPvtNodeId          nodeId,
    IN RvBool               isEmpty)
{
    RvStatus status;

    if (isEmpty)
    {
        /* Create an empty H.245 message to send */
        nodeId = pvtAddRoot(call->ep->hVal, cmGetSynTreeByRootName(call->ep->hApp, "h245"), 0, NULL);
    }
    else
    {
        if (!RV_PVT_NODEID_IS_VALID(nodeId))
        {
            /* Get the configuration node id for the capability struct */
            nodeId = pvtGetNodeIdByPath(call->ep->hVal, cmGetH245ConfigurationHandle(call->ep->hApp),
                "capabilities.terminalCapabilitySet");
        }
    }

    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return nodeId;

    /* Sends the capability struct */
    status = cmCallSendCapability(call->hsCall, nodeId);

    if (isEmpty)
    {
        /* Since we created the empty TCS message, we should also get rid of it */
        pvtDelete(call->ep->hVal, nodeId);
    }

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallSendCapabilitiesAck
 * ----------------------------------------------------------------------------
 * General: Send a TCS ack message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSendCapabilitiesAck(
    IN EpCallObj            *call)
{
    RvStatus status;

    /* Sends the capability ack */
    status = cmCallCapabilitiesAck(call->hsCall);

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallSendMasterSlave
 * ----------------------------------------------------------------------------
 * General: Send an MSD message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         terminalType - Terminal type to use. -1 to use the default from
 *                        the configuration.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSendMasterSlave(
    IN EpCallObj            *call,
    IN RvInt32              terminalType)
{
    RvStatus status;

/* Make sure we don't modify the terminal type if not requested

    RvPvtNodeId nodeId;
    if (terminalType < 0)
    {
        nodeId = pvtGetNodeIdByPath(call->ep->hVal, cmGetH245ConfigurationHandle(call->ep->hApp),
            "masterSlave.terminalType");
        pvtGet(call->ep->hVal, nodeId, NULL, NULL, &terminalType, NULL);
    }
*/

    status = cmCallMasterSlaveDetermine(call->hsCall, terminalType);

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallSendRoundTripDelay
 * ----------------------------------------------------------------------------
 * General: Send an RTD message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         seconds      - Maximum delay in seconds.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSendRoundTripDelay(
    IN EpCallObj            *call,
    IN RvInt32              maxDelay)
{
    RvStatus status;

    epUtilsEvent(call->ep, "Rec", NULL, "roundTripDelay entry= %d delay= %d", call->id, maxDelay);

    status = cmCallRoundTripDelay(call->hsCall, maxDelay);

    if (status >= 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epCallSendUserInputIndication
 * ----------------------------------------------------------------------------
 * General: Send a user input indication message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         uii          - User input indication.
 *         nsd          - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallSendUserInputIndication(
    IN EpCallObj            *call,
    IN const RvChar         *uii,
    IN cmNonStandardParam   *nsd)
{
    EpObj *ep;
    RvStatus status;
    RvPvtNodeId nodeId;

    ep = call->ep;
    if (nsd != NULL)
    {
        nodeId = cmUserInputBuildNonStandard(ep->hApp, &nsd->info, uii, (int)strlen(uii));
    }
    else
    {
        if (strncmp("SIGNAL", uii, 6) == 0)
        {
            /* create a signal node ID */
            cmUserInputSignalStruct sig;
            static int sigTimestampCounter = 0;

            sigTimestampCounter++;
            sig.signalType = uii[7];
            sig.duration = atoi(uii + 8);
            sig.cmUserInputSignalRtp.expirationTime = -1;
            sig.cmUserInputSignalRtp.logicalChannelNumber = 1;
            sig.cmUserInputSignalRtp.timestamp = sigTimestampCounter;

            nodeId = cmUserInputSignalBuild(ep->hApp, &sig);
        }
        else
        {
            /* create the alphanumeric UII node ID - generate a GeneralString encoding first... */
            cmUserInputData uid;
            RvChar uiiStr[128];

            uid.data = uiiStr;
            uid.length = (int)strlen(uii) + 2;
            if (uid.length > (int)sizeof(uiiStr) - 2)
                uid.length = (int)sizeof(uiiStr);

            uiiStr[0] = 0x1b; /* Starting Escape character - Tag indication of GeneralString */
            uiiStr[1] = (RvChar)(uid.length - 2); /* Actual length of the character string */
            memcpy(uiiStr + 2, uii, (RvSize_t)(uid.length - 2));

            nodeId = cmUserInputBuildAlphanumeric(ep->hApp, &uid);
        }
    }

    if (nodeId < 0)
        return nodeId;

    /* Sends the UII */
    status = cmCallSendUserInput(call->hsCall, nodeId);

    if ((status == RV_ERROR_DESTRUCTED) || (status == RV_ERROR_NULLPTR))
        pvtDelete(ep->hVal, nodeId);

    if (status >= 0)
        status = RV_OK;
    return status;
}








/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/










#ifdef __cplusplus
}
#endif
