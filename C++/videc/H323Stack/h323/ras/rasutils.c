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

#include "psyntreeStackApi.h"
#include "pvaltreeStackApi.h"
#include "pvaltree.h"
#include "prnutils.h"
#include "cmutils.h"
#include "rasdef.h"
#include "rasin.h"
#include "rasout.h"
#include "rasirr.h"
#include "rasparams.h"
#include "rasutils.h"
#include "cmdebprn.h"
#include "cmintr.h"
#include "msg.h"


#ifdef __cplusplus
extern "C" {
#endif



/************************************************************************
 * rasGetOutgoing
 * purpose: Get the outgoing RAS transaction from its handle
 * input  : hsRas   - Application handle
 * output : none
 * return : RAS outgoing transaction on success
 *          NULL on failure
 ************************************************************************/
#ifdef RV_RAS_DEBUG
rasOutTx* rasGetOutgoing(IN HRAS hsRas)
{
    rasOutTx*   tx;

    if (hsRas == NULL) return NULL;

    tx = (rasOutTx *)hsRas;

    /* Make sure the pointer is an outgoing transaction */
    if (emaGetType((EMAElement)hsRas) != RAS_OUT_TX)
    {
        RvLogSource * log;
        log = &((rasModule *)emaGetUserData((EMAElement)hsRas))->log;

        RvLogExcep(log,
            (log, "rasGetOutgoing: Not an outgoing transaction %p", hsRas));
        return NULL;
    }

    return tx;
}
#endif  /* RV_RAS_DEBUG */



/************************************************************************
 * rasGetIncoming
 * purpose: Get the incoming RAS transaction from its handle
 * input  : hsRas   - Application handle
 * output : none
 * return : RAS incoming transaction on success
 *          NULL on failure
 ************************************************************************/
#ifdef RV_RAS_DEBUG
rasInTx* rasGetIncoming(IN HRAS hsRas)
{
    rasInTx*   tx;

    if (hsRas == NULL) return NULL;

    tx = (rasInTx *)hsRas;

    /* Make sure the pointer is an incoming transaction */
    if (emaGetType((EMAElement)hsRas) != RAS_IN_TX)
    {
        RvLogSource * log;
        log = &((rasModule *)emaGetUserData((EMAElement)hsRas))->log;

        RvLogExcep(log,
            (log, "rasGetIncoming: Not an incoming transaction %p", hsRas));
        return NULL;
    }

    return tx;
}
#endif  /* RV_RAS_DEBUG */


#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
/************************************************************************
 * rasGetParamName
 * purpose: Get the parameter's name
 * input  : param   - Parameter enumeration value
 * output : none
 * return : Parameter's name on success
 *          Empty string on failure
 ************************************************************************/
const char *rasGetParamName(IN cmRASParam param)
{
    switch(param)
    {
        case cmRASParamGatekeeperID:                 return "gatekeeperIdentifier";
        case cmRASParamRASAddress:                   return "rasAddress";
        case cmRASParamCallSignalAddress:            return "callSignalAddress";
        case cmRASParamEndpointType:                 return "endpointType";
        case cmRASParamTerminalType:                 return "terminalType";
        case cmRASParamEndpointAlias:                return "endpointAlias";
        case cmRASParamTerminalAlias:                return "terminalAlias";
        case cmRASParamDiscoveryComplete:            return "discoveryComplete";
        case cmRASParamEndpointVendor:               return "endpointVendor";
        case cmRASParamCallType:                     return "callType";
        case cmRASParamCallModel:                    return "callModel";
        case cmRASParamEndpointID:                   return "endpointIdentifier";
        case cmRASParamDestInfo:                     return "destinationInfo";
        case cmRASParamSrcInfo:                      return "srcInfo";
        case cmRASParamDestExtraCallInfo:            return "destExtraCallInfo";
        case cmRASParamDestCallSignalAddress:        return "destCallSignalAddress";
        case cmRASParamSrcCallSignalAddress:         return "srcCallSignalAddress";
        case cmRASParamBandwidth:                    return "bandWidth";
        case cmRASParamActiveMC:                     return "activeMC";
        case cmRASParamAnswerCall:                   return "answerCall";
        case cmRASParamIrrFrequency:                 return "irrFrequency";
        case cmRASParamReplyAddress:                 return "replyAddress";
        case cmRASParamDisengageReason:              return "disengageReason";
        case cmRASParamRejectedAlias:                return "rejectReason.duplicateAlias";
        case cmRASParamRejectReason:                 return "rejectReason";
        case cmRASParamCID:                          return "conferenceID";
        case cmRASParamDestinationIpAddress:         return "destinationIpAddress";
        case cmRASParamNonStandard:                  return "nonStandard";
        case cmRASParamNonStandardData:              return "nonStandardData";
        case cmRASParamCRV:                          return "callReferenceValue";
        case cmRASParamMulticastTransaction:         return "multicast";
        case cmRASParamTransportQOS:                 return "transportQOS";
        case cmRASParamKeepAlive:                    return "keepAlive";
        case cmRASParamTimeToLive:                   return "timeToLive";
        case cmRASParamDelay:                        return "delay";
        case cmRASParamCallID:                       return "callIdentifier.guid";
        case cmRASParamAnsweredCall:                 return "answeredCall";
        case cmRASParamAlmostOutOfResources:         return "almostOutOfResources";
        case cmRASParamAlternateGatekeeper:          return "alternateGatekeeper";
        case cmRASParamAltGKInfo:                    return "altGKInfo.alternateGatekeeper";
        case cmRASParamAltGKisPermanent:             return "altGKInfo.altGKisPermanent";
        case cmRASParamEmpty:                        return "empty";
        case cmRASParamSourceInfo:                   return "sourceInfo";
        case cmRASParamNeedResponse:                 return "needResponse";
        case cmRASParamMaintainConnection:           return "maintainConnection";
        case cmRASParamMultipleCalls:                return "multipleCalls";
        case cmRASParamWillRespondToIRR:             return "willRespondToIRR";
        case cmRASParamSupportsAltGk:                return "supportsAltGk";
        case cmRASParamAdditiveRegistration:         return "additiveRegistration";
        case cmRASParamSupportsAdditiveRegistration: return "supportsAdditiveRegistration";
        case cmRASParamSegmentedResponseSupported:   return "segmentedResponseSupported";
        case cmRASParamNextSegmentRequested:         return "nextSegmentRequested";
        case cmRASParamCapacityInfoRequested:        return "capacityInfoRequested";
        case cmRASParamHopCount:                     return "hopCount";
        case cmRASParamInvalidTerminalAlias:         return "invalidTerminalAlias";
        case cmRASParamUnregReason:                  return "unregReason";
        case cmRASParamIrrStatus:                    return "irrStatus";
        case cmRASParamCallHandle:                   return "callHandle";
        case cmRASParamExtension:                    return "extension";
        case cmRASParamApplicationHandle:            return "applicationHandle";
        case cmRASParamRetries:                      return "retries";
        case cmRASParamTimeout:                      return "timeout";
        case cmRASParamLocalAddressIndex:            return "localAddressIndex";
        default:                                     break;
    }

    return "*ERROR*";
}
#endif  /* (RV_LOGMASK & RV_LOGLEVEL_ENTER) */


/************************************************************************
 * rasCfgGetTimeout
 * purpose: Get the timeout of a transaction before a retry is sent from
 *          the configuration
 * input  : ras     - RAS instance handle
 * output : none
 * return : Timeout in seconds
 ************************************************************************/
RvInt16 rasCfgGetTimeout(IN rasModule* ras)
{
    int status;
    RvInt32 timeout;

    /* Check in the configuration */
    status = pvtGetChildByFieldId(ras->hVal, ras->confNode, __q931(responseTimeOut), &timeout, NULL);

    if (status >= 0)
        return (RvInt16)timeout;
    else
        return 4; /* Default of 4 seconds */
}


/************************************************************************
 * rasCfgGetRetries
 * purpose: Get the number of retries for a transaction before a timeout
 *          from the configuration
 * input  : ras     - RAS instance handle
 * output : none
 * return : Number of retries
 ************************************************************************/
RvInt16 rasCfgGetRetries(IN rasModule* ras)
{
    int status;
    RvInt32 retries;

    /* Set the number of retries */
    status = pvtGetChildByFieldId(ras->hVal, ras->confNode, __q931(maxRetries), &retries, NULL);

    if (status >= 0)
    {
        if (retries > 0)
            return (RvInt16)retries;
        RvLogExcep(&ras->log,
            (&ras->log, "Invalid number of RAS.maxRetries in Stack configuration: %d. using 3.", retries));
    }
    return 3; /* 3 retries on default */
}


/************************************************************************
 * rasRetransmit
 * purpose: Retransmit a message to the other side
 *          This function is used for both requests and responses.
 * input  : ras         - RAS instance handle
 *          rasTx       - RAS transaction of the message
 *          rpootMsg    - RPOOL message handle to retransmit
 *          index       - Index of connection to use
 *          destAddr    - Destination address
 *          typeStr     - Type of retransmission
 * output : none
 * return : RV_OK value on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasRetransmit(
    IN  rasModule*          ras,
    IN  HRAS                rasTx,
    IN  HRPOOLELEM          rpoolMsg,
    IN  RvInt               index,
    IN  cmTransportAddress* destAddr,
    IN  const RvChar*       typeStr)
{
    int messageSize;
    RvStatus status;
    RvUint8* buffer;

    RV_UNUSED_ARG(rasTx);
    RV_UNUSED_ARG(typeStr);

    RvLogDebug(&ras->log,
        (&ras->log, "rasRetransmit: Retransmitting %s on %p", typeStr, rasTx));

    getEncodeDecodeBuffer((int)ras->bufferSize, &buffer);

    /* Get message from RPOOL to a single buffer */
    messageSize = rpoolChunkSize(ras->messages, rpoolMsg);
    rpoolCopyToExternal(ras->messages, buffer, rpoolMsg, 0, messageSize);

    /* Send the message. We always send through the unicast port */
    status = ras->evSendMessage(ras->app, index, destAddr, buffer, (RvSize_t)messageSize);
    if (status != RV_OK)
    {
        RvLogError(&ras->log,
            (&ras->log, "rasRetransmit: Couldn't send the message %p: %d", rasTx, status));
    }

    return status;
}


/************************************************************************
 * rasEncodeAndSend
 * purpose: Encode and send a message on the net
 * input  : ras         - RAS instance handle
 *          rasTx       - RAS transaction of the message
 *          stage       - Stage of the RAS transaction
 *          nodeId      - nodeId of the message to send out
 *          index       - Index of connection to use for sending the message
 *          destAddr    - Destination address
 *          storeInRPOOL- Indicate if we want the message to be stored in RPOOL
 * output : rpoolHandle - RPOOL handle storing the message
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasEncodeAndSend(
    IN  rasModule*          ras,
    IN  HRAS                rasTx,
    IN  cmRASTrStage        stage,
    IN  RvPvtNodeId         nodeId,
    IN  RvInt               index,
    IN  cmTransportAddress* destAddr,
    IN  RvBool              storeInRPOOL,
    OUT HRPOOLELEM*         rpoolHandle)
{
    cmHookSendToT   sendHook;
    int             status = 0;
    RvPvtNodeId     addrNodeId;
    int             encodedSize = 0;
    RvBool          process = RV_TRUE, printMsg, printWireMsg;
    cmElem          *app;
    int             nesting;

    sendHook = cmiGetRasHooks(ras->app)->hookSendTo;
    printMsg = RvLogIsSelected(&ras->logChan, RV_LOGLEVEL_DEBUG);
    printWireMsg = RvLogIsSelected(&ras->logChanWire, RV_LOGLEVEL_DEBUG);

    app = (cmElem *)(ras->app);

    if (printMsg || (sendHook != NULL))
    {
        /* Create a transport address node id */
        /* todo: Don't use cmElem */
        addrNodeId = pvtAddRoot(ras->hVal, app->hAddrSyn, 0, NULL);
        if (addrNodeId < 0) return addrNodeId;
        cmTAToVt(ras->hVal, addrNodeId, destAddr);
    }
    else
        addrNodeId = RV_ERROR_UNKNOWN;

#if (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG))
    {
        /* Print the message that the stack has prepared */
        RvPstFieldId fieldId;
        pvtGet(ras->hVal, pvtChild(ras->hVal, nodeId), &fieldId, NULL, NULL, NULL);
        RvLogInfo(&ras->logChan,
            (&ras->logChan, "New message (channel %d) to send --> %s:",
                index, pstGetFieldNamePtr(ras->synMessage, fieldId)));

        if (printMsg)
        {
            RvLogTextDebug(&ras->logChan, "Address:");
            pvtPrintInternal(ras->hVal, addrNodeId, &ras->logChan, msGetDebugLevel());
            RvH323CmPrintMessage(&ras->logChan, "Message:",
                ras->hVal, nodeId, NULL, 0, RV_FALSE);
        }
    }
#endif /* (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG)) */

    /* Make sure to notify the application of an outgoing RAS message */
    if (ras->evApp.cmEvRASMessageSend != NULL)
    {
        /* We first want to see if this is a manual RAS or automatic RAS transaction */
        RvBool isManual;
        HAPPRAS haRas = NULL;

        switch (stage)
        {
        case cmRASTrStageDummy:
            isManual = RV_FALSE;
            break;
        case cmRASTrStageRequest:
            {
                /* Outgoing */
                rasOutTx *outTx = (rasOutTx *)rasTx;
                isManual = (outTx->evResponse == NULL);
            }
            break;
        default:
            {
                /* Incoming */
                rasInTx *inTx = (rasInTx *)rasTx;
                isManual = (inTx->isAutomaticRAS == 0);
            }
            break;
        }
        if (isManual)
            haRas = (HAPPRAS)emaGetApplicationHandle((EMAElement)rasTx);
        nesting = emaPrepareForCallback((EMAElement)rasTx);
        cmiCBEnter((ras->app, "cmEvRasMessageSend(haRas=%p,hsRas=%p,stage=%d,nodeId=%d)", haRas, rasTx, stage, nodeId));
        process = !ras->evApp.cmEvRASMessageSend(haRas, rasTx, stage, nodeId);
        cmiCBExit((ras->app, "cmEvRasMessageSend=%d", !process));
        emaReturnFromCallback((EMAElement)rasTx, nesting);
    }

    if ((rasTx != NULL) && emaWasDeleted((EMAElement)rasTx))
        return RV_ERROR_DESTRUCTED;

    if (process)
    {
        /* Call the hook before we encode this message */
        if (sendHook != NULL)
        {
            HPROTCONN hProtCon = (HPROTCONN)emaGetByIndex(app->rasListeningSockets, index-1);
            nesting = emaPrepareForCallback((EMAElement)rasTx);
            process = !sendHook(hProtCon, nodeId, addrNodeId, RV_FALSE);
            emaReturnFromCallback((EMAElement)rasTx, nesting);
        }
    }

    /* Make sure we're still sending out this message */
    if (process)
    {
        RvUint8*           buffer;

        getEncodeDecodeBuffer((int)ras->bufferSize, &buffer);

        /* Encode the message in PER */
        if (ras->cmiEvRASSendRawMessage)
        {
			memset(buffer, 0, ras->bufferSize);
            /* We have an encoding callback - probably from security app. Call it */
            nesting = emaPrepareForCallback((EMAElement)rasTx);
            status = ras->cmiEvRASSendRawMessage(
                ras->app,
                (HPROTCONN)emaGetByIndex(app->rasListeningSockets, index-1),
                rasTx,
                (HAPPRAS)emaGetApplicationHandle((EMAElement)rasTx),
                nodeId,
                (int)ras->bufferSize,
                buffer,
                &encodedSize);
            emaReturnFromCallback((EMAElement)rasTx, nesting);

            if ((rasTx != NULL) && emaWasDeleted((EMAElement)rasTx))
                return RV_ERROR_DESTRUCTED;
        }
        else
        {
            /* We encode the message on our own */
            status = cmEmEncode(ras->hVal, nodeId, buffer, (int)ras->bufferSize, &encodedSize);
        }

        if (status >= 0)
        {
            if (storeInRPOOL)
            {
                /* Make sure we store this message somewhere */
                rpoolFree(ras->messages, *rpoolHandle);
                *rpoolHandle = rpoolAllocCopyExternal(ras->messages, 0, buffer, encodedSize);
            }

#if (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG))
            if (printWireMsg)
            {
                /* Print the message that the stack has prepared*/
                RvPstFieldId fieldId;
                pvtGet(ras->hVal, pvtChild(ras->hVal, nodeId), &fieldId, NULL, NULL, NULL);
                if (!RvLogIsSelected(&ras->logChan, RV_LOGLEVEL_INFO))
                {
                    RvLogInfo(&ras->logChanWire,
                        (&ras->logChanWire, "New message (channel %d) to send --> %s:",
                            index, pstGetFieldNamePtr(ras->synMessage, fieldId)));
                }
                RvLogTextDebug(&ras->logChanWire, "Address:");
                pvtPrintInternal(ras->hVal, addrNodeId, &ras->logChanWire, msGetDebugLevel());
                RvH323CmPrintMessage(&ras->logChanWire, "Message:",
                    ras->hVal, nodeId, buffer, encodedSize, RV_FALSE);
            }

            if (printMsg)
            {
                RvH323CmPrintMessage(&ras->logChan, NULL,
                    ras->hVal, -1, buffer, encodedSize, RV_FALSE);
            }
#endif /* (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG)) */

            /* Send the message. We always send through the unicast port */
            status = ras->evSendMessage(ras->app, index, destAddr, buffer, (RvSize_t)encodedSize);
            if (status < 0)
            {
                RvLogError(&ras->log,
                    (&ras->log, "rasEncodeAndSend: Couldn't send the message %p: %d", rasTx, status));
            }
        }
        else
        {
#if (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG))
            RvPstFieldId fieldId;
            pvtGet(ras->hVal, pvtChild(ras->hVal, nodeId), &fieldId, NULL, NULL, NULL);
            RvLogInfo(&ras->logChan,
                (&ras->logChan, "New message (channel %d) Not sent (error) --> %s:",
                     index, pstGetFieldNamePtr(ras->synMessage, fieldId)));

            if (printMsg)
            {
                RvLogTextDebug(&ras->logChanWire, "Address:");
                pvtPrintInternal(ras->hVal, addrNodeId, &ras->logChanWire, msGetDebugLevel());
                RvH323CmPrintMessage(&ras->logChan, "Message:",
                    ras->hVal, nodeId, buffer, -1, RV_FALSE);
            }
#endif /* (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG)) */

            RvLogError(&ras->log,
                (&ras->log, "rasEncodeAndSend: Failed to encode the message %p: %d", rasTx, status));
        }
    }
    else
    {
        RvLogDebug(&ras->log,
            (&ras->log, "rasEncodeAndSend: Application doesn't want this message processed (root=%d)", nodeId));
    }

    /* Make sure we clear any nodes needed for address printing */
    pvtDelete(ras->hVal, addrNodeId);

    if (status > 0)
        status = 0; /* Make sure return value is not positive - gatekeeper depends on it */
    return status;
}


/************************************************************************
 * rasDecodeAndRecv
 * purpose: Decode and receive a message from the net
 *          This function is called after we already know if its an incoming
 *          or outgoing message.
 *          It returns as parameters the sequence number and message type,
 *          allowing the caller to know if these parameters were changed in
 *          the hook function to the application
 * input  : ras             - RAS instance handle
 *          index           - Index of the local RAS address message was received from
 *          messageBuf      - Message buffer
 *          messageLength   - Length of received message
 *          isMulticast     - Are we sending it to a multicast address
 *          srcAddr         - Source address
 * output : srcAddr         - Reply address if found inside the message
 *          nodeId          - Root where we placed the message
 *          messageType     - Message type after hook
 *          requestSeqNum   - Sequence number in decoded message after hook
 * return : RV_TRUE if message should be processed
 *          RV_FALSE if message shouldn't be processed
 *          Negative value on failure
 ************************************************************************/
int rasDecodeAndRecv(
    IN     rasModule*          ras,
    IN     RvInt               index,
    IN     RvUint8*            messageBuf,
    IN     RvUint32            messageLength,
    IN     RvBool              isMulticast,
    INOUT  cmTransportAddress* srcAddr,
    OUT    int*                nodeId,
    OUT    rasMessages*        messageType,
    OUT    RvUint32*           requestSeqNum,
    OUT    void**              hMsgContext)
{
    cmHookRecvFromT recvHook;
    HPROTCONN       protConn;
    int             status = 0;
    int             addrNodeId, replyNodeId, msgNodeId;
    int             bytesDecoded = 0;
    RvBool          process, printMsg, printWireMsg;
    cmElem          *app;

    RV_UNUSED_ARG(index);

    *nodeId = pvtAddRoot(ras->hVal, ras->synMessage, 0, NULL);
    if ((*nodeId) < 0)
        return *nodeId;

    app = (cmElem*)(ras->app);
    protConn = (HPROTCONN)emaGetByIndex(app->rasListeningSockets, index-1);

    /* Decode the message */
    if (ras->cmiEvRASNewRawMessage)
    {
        /* We have a decoding callback - probably from security app. Call it */
        RvLogInfo(&ras->log,
            (&ras->log, "cmEvRASNewRawMessage(protConn = %p, pvt=%d)",
                protConn, *nodeId));

        status = ras->cmiEvRASNewRawMessage((HAPP)ras->app, protConn, *nodeId,
            messageBuf, (int)messageLength, srcAddr, &bytesDecoded, hMsgContext);
    }
    else
    {
        /* Let's decode it on our own */
        status = cmEmDecode(ras->hVal, *nodeId, messageBuf, (int)messageLength, &bytesDecoded);
    }

    if ((status >= 0) && ((int)messageLength != bytesDecoded))
    {
        RvH323CmPrintMessage(&ras->logChan, "rasDecodeAndRecv: Bad message length:",
            ras->hVal, -1, messageBuf, (int)messageLength, RV_TRUE);
        pvtDelete(ras->hVal, *nodeId);
        *nodeId = RV_PVT_INVALID_NODEID;
        return RV_ERROR_UNKNOWN;
    }
    if (status < 0)
    {
        pvtDelete(ras->hVal, *nodeId);
        *nodeId = RV_PVT_INVALID_NODEID;
        RvH323CmPrintMessage(&ras->logChan, "Binary:",
            ras->hVal, -1, messageBuf, bytesDecoded, RV_TRUE);
        return status;
    }

    recvHook = cmiGetRasHooks(ras->app)->hookRecvFrom;
    printMsg = RvLogIsSelected(&ras->logChan, RV_LOGLEVEL_DEBUG);
    printWireMsg = RvLogIsSelected(&ras->logChanWire, RV_LOGLEVEL_DEBUG);

    if (printMsg || (recvHook != NULL))
    {
        /* Create a transport address node id */
        addrNodeId = pvtAddRoot(ras->hVal, app->hAddrSyn, 0, NULL);
        if (addrNodeId < 0)
        {
            pvtDelete(ras->hVal, *nodeId);
            *nodeId = RV_PVT_INVALID_NODEID;
            return addrNodeId;
        }
        cmTAToVt(ras->hVal, addrNodeId, srcAddr);
    }
    else
        addrNodeId = RV_ERROR_UNKNOWN;

#if (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG))
    /* print what we got from wire */
    {
        /* Print the message */
        RvPstFieldId    fieldId;
        pvtGet(ras->hVal, pvtChild(ras->hVal, *nodeId), &fieldId, NULL, NULL, NULL);
        RvLogInfo(&ras->logChanWire,
            (&ras->logChanWire, "New message (channel %d)  recv <-- %s:",
                isMulticast, pstGetFieldNamePtr(ras->synMessage, fieldId)));

        if (printWireMsg)
        {
            RvLogTextDebug(&ras->logChanWire, "Address:");
            pvtPrintInternal(ras->hVal, addrNodeId, &ras->logChanWire, msGetDebugLevel());
            RvH323CmPrintMessage(&ras->logChanWire, "Binary:",
                ras->hVal, *nodeId, messageBuf, bytesDecoded, RV_TRUE);
        }
    }
#endif /* (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG)) */

    /* Call the hook after decoding this message */
    if (recvHook != NULL)
    {
        process = !recvHook(protConn, *nodeId, addrNodeId, isMulticast, RV_FALSE);
    }
    else
        process = RV_TRUE;

    /* Make sure we still want to receive this message */
    if (process)
    {
        int             tmpNodeId;
#if (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG))
        RvPstFieldId    fieldId;
        pvtGet(ras->hVal, pvtChild(ras->hVal, *nodeId), &fieldId, NULL, NULL, NULL);
        /* Print what we pass to UDP channel */
        if (!RvLogIsSelected(&ras->logChanWire, RV_LOGLEVEL_INFO))
        {
            RvLogInfo(&ras->logChan,
                (&ras->logChan, "New message (channel %d)  recv <-- %s:",
                     isMulticast, pstGetFieldNamePtr(ras->synMessage, fieldId)));
        }

        if (printMsg)
        {
            RvLogTextDebug(&ras->logChan, "Address:");
            pvtPrintInternal(ras->hVal, addrNodeId, &ras->logChan, msGetDebugLevel());
            RvH323CmPrintMessage(&ras->logChan, "Binary:",
                ras->hVal, *nodeId, messageBuf, bytesDecoded, RV_TRUE);
        }
#endif /* (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG)) */

        /* Get the index and sequence number again */
        msgNodeId = pvtGetByIndex(ras->hVal, *nodeId, 1, NULL);
        if (msgNodeId >= 0)
            *messageType = (rasMessages)(pvtGetSyntaxIndex(ras->hVal, msgNodeId) - 1);
        tmpNodeId = pvtGetChild(ras->hVal, msgNodeId, __q931(requestSeqNum), NULL);
        pvtGet(ras->hVal, tmpNodeId, NULL, NULL, (RvInt32*)requestSeqNum, NULL);

        /* See if we have any specific reply address in the message */
        switch (*messageType)
        {
            case rasMsgRegistrationRequest:
                __pvtGetNodeIdByFieldIds(replyNodeId, ras->hVal, msgNodeId, {_q931(rasAddress) _nul(1) LAST_TOKEN});
                break;

            case rasMsgGatekeeperRequest:
            case rasMsgInfoRequestResponse:
                replyNodeId = pvtGetChild(ras->hVal, msgNodeId, __q931(rasAddress), NULL);
                break;

            case rasMsgLocationRequest:
            case rasMsgInfoRequest:
                replyNodeId = pvtGetChild(ras->hVal, msgNodeId, __q931(replyAddress), NULL);
                break;

            default:
                replyNodeId = -1;
        }
        if (replyNodeId >= 0)
            cmVtToTA(ras->hVal, replyNodeId, srcAddr);
    }
    else
    {
        RvLogDebug(&ras->log,
            (&ras->log, "rasDecodeAndRecv: Application doesn't want this message processed (root=%d)", *nodeId));
        pvtDelete(ras->hVal, *nodeId);
        *nodeId = RV_PVT_INVALID_NODEID;
    }

    if (addrNodeId >= 0)
        pvtDelete(ras->hVal, addrNodeId);

    return (process?0:-1);   /* Look! I made a smiley-thingy! */
}


/************************************************************************
 * rasRouteMessage
 * purpose: Route the message to the right transaction, making sure if
 *          it's incoming, outgoing or IRR.
 * input  : ras             - RAS instance handle
 *          srcAddr         - Source address
 *          index           - Index of the connection the message was
 *                            received to
 *          isMulticast     - RV_TRUE if message was received on a multicast
 *                            connection
 *          messageBuf      - Message buffer
 *          messageLength   - Length of received message
 *          messageNodeId   - Node ID of the message.
 *                            If this value is negative, the message is
 *                            decoded and checked, otherwise, the decoded
 *                            message will be processed without calling the
 *                            hook functions.
 *          messageType     - Message type after hook
 *          requestSeqNum   - Sequence number in decoded message after hook
 * output : hMsgContext     - Incoming message context. Used mostly by security
 *                            If the returned value is different than NULL,
 *                            then the message context is not used by the
 *                            transaction and should be released
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasRouteMessage(
    IN  rasModule*          ras,
    IN  RvInt               index,
    IN  RvBool              isMulticast,
    IN  cmTransportAddress* srcAddr,
    IN  RvUint8*            messageBuf,
    IN  RvUint32            messageLength,
    IN  int                 messageNodeId,
    IN  rasMessages         messageType,
    IN  RvUint32            requestSeqNum,
    OUT void**              hMsgContext)
{
    /* IRR messages are handled differently */
    if (messageType == rasMsgInfoRequestResponse)
        return rasHandleIrr(ras, index, srcAddr, messageBuf, messageLength, messageNodeId, requestSeqNum, hMsgContext);

    /* Check if it's an incoming or outgoing message */
    switch (rasMessageInfo[messageType].trType)
    {
        case RAS_OUT_TX:
            /* Outgoing transaction reply */
            return rasHandleReply(ras, index, srcAddr, messageBuf, messageLength, messageNodeId, messageType, requestSeqNum, hMsgContext);
        case RAS_IN_TX:
            /* Incoming transaction request */
            return rasHandleRequest(ras, index, isMulticast, srcAddr, messageBuf, messageLength, messageNodeId, messageType, requestSeqNum, NULL, hMsgContext);
        default:
            RvLogExcep(&ras->log,
                (&ras->log, "rasRouteMessage: Transaction type unknown for %d", messageType));
            return RV_ERROR_UNKNOWN;
    }
}


/************************************************************************
 * rasCreateCatKey
 * purpose: Create the key struct for CAT from an incoming message
 *          transaction.
 * input  : ras     - RAS instance handle
 *          tx      - Incoming transaction to use
 * output : catKey  - Filled CAT key struct for this transaction
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasCreateCatKey(
    IN  rasModule*  ras,
    IN  rasInTx*    tx,
    OUT catStruct*  catKey)
{
    /* Make sure we start empty handed with this CAT key */
    memset(catKey, 0, sizeof(catStruct));

    /* Clear the flags */
    catKey->flags = catRasSrcAddr;

    /* RAS source address */
    memcpy(&catKey->rasSrcAddr, &tx->destAddress, sizeof(cmTransportAddress));

    /* RAS-CRV */
    switch (tx->transactionType)
    {
        case cmRASAdmission:
        case cmRASDisengage:
        case cmRASBandwidth:
        case cmRASInfo:
        {
            int crv;
            if (rasGetParam(ras, (HRAS)tx, cmRASTrStageRequest, cmRASParamCRV,
                            0, &crv, NULL) >= 0)
            {
                catKey->rasCRV = (RvUint32)(0x8000^crv);
                catKey->flags |= catRasCRV;
            }
        }
        break;

        case cmRASServiceControl:
        default:
            break;
    }

    /* CallID */
    if (rasGetParam(ras, (HRAS)tx, cmRASTrStageRequest, cmRASParamCallID,
                    0, NULL, (char*)catKey->callID) >= 0)
        catKey->flags |= catCallId;

    if (tx->transactionType == cmRASAdmission)
    {
        /* SourceCallSignalAddress */
        if (rasGetParam(ras, (HRAS)tx, cmRASTrStageRequest, cmRASParamSrcCallSignalAddress,
                        0, NULL, (char*)&catKey->srcCallSignalAddr) >= 0)
            catKey->flags |= catSrcCallSignalAddr;

        /* DestCallSignalAddress */
        if (rasGetParam(ras, (HRAS)tx, cmRASTrStageRequest, cmRASParamDestCallSignalAddress,
                        0, NULL, (char*)&catKey->destCallSignalAddr) >= 0)
            catKey->flags |= catDestCallSignalAddr;
    }

    switch (tx->transactionType)
    {
        case cmRASAdmission:
        case cmRASDisengage:
        case cmRASBandwidth:
        case cmRASServiceControl:
        {
            RvInt32 bAnswerCall;

            /* CID */
            if (rasGetParam(ras, (HRAS)tx, cmRASTrStageRequest, cmRASParamCID,
                            0, NULL, (char*)catKey->cid) >= 0)
                catKey->flags |= catCID;

            /* answerCall */
            if (rasGetParam(ras, (HRAS)tx, cmRASTrStageRequest, cmRASParamAnswerCall,
                            0, &bAnswerCall, NULL) >= 0)
            {
                if (ras->isGatekeeper)
                {
                    catKey->answerCall = (RvBool)!bAnswerCall;
                }
                else
                {
                    /* If we're not a GK, we should take the side the GK is informing us */
                    catKey->answerCall = (RvBool)bAnswerCall;
                }
                catKey->flags |= catAnswerCall;
            }
        }
        break;

        case cmRASInfo:
        default:
            break;
    }

    /* Just an address will not be considered as CAT keys by us */
    if (catKey->flags == catRasSrcAddr)
        return RV_ERROR_UNKNOWN;

    return 0;
}


/************************************************************************
 * rasUpdateRegInfo
 * purpose: Update the registration information of our RAS configuration
 *          from an incoming RCF message
 * input  : ras             - RAS instance handle
 *          messageNodeId   - Incoming message that caused this call
 *                            For unregistration, this value will be negative
 * output : none
 * return : none
 ************************************************************************/
void rasUpdateRegInfo(
    IN  rasModule*  ras,
    IN  int         messageNodeId)
{
    int srcNode;

    /* GK addresses - remove if we have to */
    if (!RV_PVT_NODEID_IS_VALID(messageNodeId))
    {
        pvtDelete(ras->hVal, ras->gatekeeperRASAddress);
        ras->gatekeeperRASAddress = RV_PVT_INVALID_NODEID;

        pvtDelete(ras->hVal, ras->gatekeeperCallSignalAddress);
        ras->gatekeeperCallSignalAddress = RV_PVT_INVALID_NODEID;

        pvtDelete(ras->hVal, ras->termAliasesNode);
        ras->termAliasesNode = RV_PVT_INVALID_NODEID;

        ras->epIdLen = 0;
        ras->gkIdLen = 0;
    }
    else
    {
        /* GK callSignalAddress*/
        if (!RV_PVT_NODEID_IS_VALID(ras->gatekeeperCallSignalAddress))
            ras->gatekeeperCallSignalAddress = pvtAddRoot(ras->hVal, NULL, 0, NULL);
        __pvtGetNodeIdByFieldIds(srcNode,ras->hVal,messageNodeId, {_anyField _q931(callSignalAddress) _anyField LAST_TOKEN});
        pvtSetTree(ras->hVal, ras->gatekeeperCallSignalAddress, ras->hVal, srcNode);

        if (!RV_PVT_NODEID_IS_VALID(ras->termAliasesNode))
            ras->termAliasesNode = pvtAddRoot(ras->hVal, NULL, 0, NULL);
        __pvtGetNodeIdByFieldIds(srcNode, ras->hVal, messageNodeId, {_anyField _q931(terminalAlias) LAST_TOKEN});
        if (RV_PVT_NODEID_IS_VALID(srcNode))
            pvtSetTree(ras->hVal, ras->termAliasesNode, ras->hVal, srcNode);
        else
        {
            __pvtGetNodeIdByFieldIds(srcNode, ras->hVal, ras->confNode, {_q931(registrationInfo) _q931(terminalAlias) LAST_TOKEN});
            if (RV_PVT_NODEID_IS_VALID(srcNode))
                pvtSetTree(ras->hVal, ras->termAliasesNode, ras->hVal, srcNode);
        }

        /* endpointIdentifier */
        __pvtGetByFieldIds(srcNode, ras->hVal, messageNodeId, {_anyField _q931(endpointIdentifier) LAST_TOKEN}, NULL, &ras->epIdLen, NULL);
        if (RV_PVT_NODEID_IS_VALID(srcNode))
            pvtGetString(ras->hVal, srcNode, ras->epIdLen, ras->epId);
        else
            ras->epIdLen = 0;

        /* gatekeeperIdentifier */
        __pvtGetByFieldIds(srcNode, ras->hVal, ras->confNode, {_q931(registrationInfo) _q931(gatekeeperIdentifier) LAST_TOKEN}, NULL, (RvInt32 *)&ras->gkIdLen, NULL);
        if (RV_PVT_NODEID_IS_VALID(srcNode))
            pvtGetString(ras->hVal, srcNode, ras->gkIdLen, ras->gkId);
        else
            ras->gkIdLen = 0;
    }

    /* Make sure the default messages of the RAS are also updated */
    cmiRASUpdateRegInfo((HRASMGR)ras, RV_FALSE);
}


/************************************************************************
 * rasUpdatePartUnreg
 * purpose: Update the registration information of our RAS configuration
 *          from an outgoing partial URQ message.
 * input  : ras             - RAS instance handle
 *          messageNodeId   - Outgoing message whose approval caused this
 *                            function to be called.
 * output : none
 * return : none
 ************************************************************************/
void rasUpdatePartUnreg(
    IN  rasModule*  ras,
    IN  int         messageNodeId)
{
    int removedAliases, currentAlias, aliasToRemove;

    if (ras->termAliasesNode < 0)
        /* no aliases, one wonders how we got here. anyway, let's return before anyone notices */
        return;
    __pvtGetNodeIdByFieldIds(removedAliases, ras->hVal, messageNodeId, {_q931(unregistrationRequest) _q931(endpointAlias) LAST_TOKEN});

    for(aliasToRemove=pvtChild(ras->hVal, removedAliases); aliasToRemove>=0; aliasToRemove=pvtBrother(ras->hVal, aliasToRemove))
        for(currentAlias=pvtChild(ras->hVal, ras->termAliasesNode); currentAlias>=0; currentAlias=pvtBrother(ras->hVal, currentAlias))
            if(pvtCompareTree(ras->hVal, currentAlias, ras->hVal, aliasToRemove)>=0)
            {
                pvtDelete(ras->hVal, currentAlias);
                break; /* only from the first loop */
            }
}


/************************************************************************
 * rasUpdateGkRasAddress
 * purpose: Update the gatekeeperRASAddress (needed when using AltGK).
 * input  : ras             - RAS instance handle
 *          addrNodeId      - Node with the address.
 * output : none
 * return : none
 ************************************************************************/
void rasUpdateGkRasAddress(
    IN  rasModule*      ras,
    IN  RvPvtNodeId     addrNodeId,
    IN  char            gkId[256],
    IN  int             gkIdLen)
{
    /* update address */
    if (RV_PVT_NODEID_IS_VALID(addrNodeId))
    {
        if (ras->gatekeeperRASAddress < 0)
            ras->gatekeeperRASAddress = pvtAddRoot(ras->hVal, ((cmElem*)(ras->app))->hAddrSyn, 0, NULL);
        pvtSetTree(ras->hVal, ras->gatekeeperRASAddress, ras->hVal, addrNodeId);
    }
    else
    {
        if (ras->gatekeeperRASAddress >= 0)
            pvtDelete(ras->hVal, ras->gatekeeperRASAddress);
        ras->gatekeeperRASAddress = RV_PVT_INVALID_NODEID;
    }

    /* update GK ID */
    if (gkIdLen > 0)
    {
        memcpy(ras->gkId, gkId, (RvSize_t)gkIdLen);
        ras->gkIdLen = gkIdLen;
    }
    else
        ras->gkIdLen = 0;

    /* Make sure the default messages of the RAS are also updated */
    cmiRASUpdateRegInfo((HRASMGR)ras, RV_FALSE);
}


#ifdef __cplusplus
}
#endif


