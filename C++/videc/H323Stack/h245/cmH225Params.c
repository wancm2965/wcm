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
#include "cmH225Params.h"
#include "cmH245Object.h"
#include "cmH245GeneralDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)

#include "cm.h"
#include "cmictrl.h"
#include "rvinternal.h"
#include "rvaddress.h"
#include "cmiParams.h"

extern H22XFunctions h22xFunctions[];
#define nprn(s) ((s)?(s):"(null)")
char * getIP(IN cmTransportAddress* cmAddress, OUT RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE]);
RvBool cmTaHasIp(IN cmTransportAddress* ta);

#define CONFLICT_CHANNELS_ARRAY_SIZE 32


int notifyChannelState(H245Channel*channel,cmChannelState_e state, cmChannelStateMode_e stateMode);
/************************************************************************
 * h225ParamsStartEstablishment
 * purpose: Start establishing a channel. This function adds h255 parameters into
 *          an OLC message from the channel element.
 * input  : hVal          - handle to the PVT.
 *          olcID         - node id for the open logical channel.
 *          forwardLCP_ID - node id for the forward logical channel parameters.
 *          reverseLCP_ID - node id for the reverse logical channel parameters.
 *          dataType      - node id for the data type.
 *          pChannel      - Channel element to add h225 parameters OLC for.
 * output : none
 * return : none
 ************************************************************************/
void h225ParamsStartEstablishment(IN HPVT        hVal,
                                  IN RvPvtNodeId olcID,
                                  IN RvPvtNodeId forwardLCP_ID,
                                  IN RvPvtNodeId reverseLCP_ID,
                                  IN RvPvtNodeId dataType,
                                  IN H245Channel *pChannel)
{
    int h225ID;
    int tmpID;

    /* - H225 parameters start here */
    __pvtBuildByFieldIds(h225ID, hVal, forwardLCP_ID, {_h245(multiplexParameters) _h245(h2250LogicalChannelParameters) LAST_TOKEN}, 0, NULL);

    /* Add the dynamic payload type if we have one */
    if (pChannel->data.h225Params.dynamicPayloadNumber > 0)
    {
        pvtAdd(hVal, h225ID, __h245(dynamicRTPPayloadType), pChannel->data.h225Params.dynamicPayloadNumber, NULL, NULL);
    }
    /* Set session ID in message */
    pvtAdd(hVal, h225ID, __h245(sessionID), pChannel->data.h225Params.sessionId, NULL, NULL);

    /* Add association if we have any */
    if (pChannel->data.h225Params.pAssociated != NULL)
        pvtAdd(hVal, h225ID, __h245(associatedSessionID), pChannel->data.h225Params.pAssociated->data.h225Params.sessionId, NULL, NULL);

    /* Add redundancy encoding information for the pChannel */
    if (pChannel->data.h225Params.redEncID >= 0)
        pvtSetTree(hVal, pvtAddBranch(hVal, h225ID, __h245(redundancyEncoding)), hVal, pChannel->data.h225Params.redEncID);

    /* Set transport capability information if we have */
    if (pChannel->data.h225Params.transCapID >= 0)
        pvtSetTree(hVal, pvtAddBranch(hVal, h225ID, __h245(transportCapability)), hVal, pChannel->data.h225Params.transCapID);

    /* Add RTP address */
    if (pChannel->data.h225Params.recvRtpAddressID >= 0)
        pvtSetTree(hVal, pvtAddBranch(hVal, h225ID, __h245(mediaChannel)), hVal, pChannel->data.h225Params.recvRtpAddressID);

    /* Set ATM port number for this pChannel if we have one */
    if ((pChannel->data.h225Params.portNumber > 0) && (pChannel->data.h225Params.portNumber < 65536))
        pvtAdd(hVal, forwardLCP_ID, __h245(portNumber), pChannel->data.h225Params.portNumber, NULL, NULL);

    if (pChannel->data.h225Params.source.mcuNumber != 0xff)
    {
        /* We have an MCU number on the side of the source */
        int sID = pvtAddBranch(hVal, h225ID, __h245(source));
        int iMCU = pChannel->data.h225Params.source.mcuNumber;
        int iTerminal = pChannel->data.h225Params.source.terminalNumber;
        pvtAdd(hVal, sID, __h245(mcuNumber), iMCU, NULL, NULL);
        pvtAdd(hVal, sID, __h245(terminalNumber), iTerminal, NULL, NULL);
    }
    if (pChannel->data.h225Params.destination.mcuNumber!=0xff)
    {
        /* We have an MCU number on the side of the destination */
        int sID = pvtAddBranch(hVal, h225ID, __h245(source));
        int iMCU = pChannel->data.h225Params.destination.mcuNumber;
        int iTerminal = pChannel->data.h225Params.destination.terminalNumber;
        pvtAdd(hVal, sID, __h245(mcuNumber), iMCU, NULL, NULL);
        pvtAdd(hVal, sID, __h245(terminalNumber), iTerminal, NULL, NULL);
    }

    /* Add separate stack information if we have to */
    if (pChannel->data.h225Params.separateStackID >= 0)
    {
        int separateStack, tmpNodeId;

        /* Put the separateStack local area address */
        separateStack = pvtAddBranch(hVal, olcID, __h245(separateStack));

        __pvtBuildByFieldIds(tmpNodeId, hVal, separateStack, {_h245(networkAddress) _h245(localAreaAddress) LAST_TOKEN}, 0, NULL);
        pvtSetTree(hVal, tmpNodeId, hVal, pChannel->data.h225Params.separateStackID);

        /* Put externalReference information if we have */
        if (pChannel->data.h225Params.externalReferenceLength > 0)
            pvtAdd(hVal, separateStack, __h245(externalReference), pChannel->data.h225Params.externalReferenceLength, pChannel->data.h225Params.externalReference, NULL);

        /* Put association information */
        pvtAdd(hVal, separateStack, __h245(associateConference), (RvInt32)pChannel->data.h225Params.isAssociated, NULL, NULL);

        /* Add the right t120 setup procedure */
        if (((int)pChannel->data.h225Params.t120SetupProcedure >= (int)cmOriginateCall) &&
            ((int)pChannel->data.h225Params.t120SetupProcedure <= (int)cmIssueQuery))
        {
            static int t120s[] = {__h245(originateCall), __h245(waitForCall), __h245(issueQuery)};
            pvtAddBranch2(hVal, separateStack, __h245(t120SetupProcedure), t120s[(int)pChannel->data.h225Params.t120SetupProcedure-(int)cmOriginateCall]);
        }
    }

    if (pChannel->bIsDuplex)
    {
        /* duplex default parameter setting.*/

        /* Add reverse channel's parameters */
        if ((tmpID = pvtAddBranch(hVal, reverseLCP_ID, __h245(dataType))) >= 0)
            pvtSetTree(hVal, tmpID, hVal, dataType);

        /* Put the session ID here also */
        __pvtBuildByFieldIds(tmpID, hVal, reverseLCP_ID, {_h245(multiplexParameters) _h245(h2250LogicalChannelParameters) _h245(sessionID) LAST_TOKEN}, pChannel->data.h225Params.sessionId, NULL);
    }
    else
    {
        /* uni-directional channel */
        /* See from where do we get the RTCP address */
        int srcRtcpID;

        if ((pChannel->data.h225Params.pPartner != NULL) &&
            (pChannel->data.h225Params.pPartner->data.h225Params.recvRtcpAddressID >= 0))
        {
            /* If we have a partner ID with a receive RTCP address, we should use it instead of
               our own address, since most chances are that we don't even have such an address. */
            srcRtcpID = pChannel->data.h225Params.pPartner->data.h225Params.recvRtcpAddressID;
        }
        else
            srcRtcpID = pChannel->data.h225Params.recvRtcpAddressID;

        if (srcRtcpID >= 0)
            pvtSetTree(hVal, pvtAddBranch(hVal, h225ID, __h245(mediaControlChannel)), hVal, srcRtcpID);
    }
}

/************************************************************************
 * h225cmcCallAddressCallbacks
 * purpose: Call the callbacks of the h225 addresses.
 * input  : pApp            - application handle for the cm.
 *          pChannel        - Channel element to set h225 parameters for.
 *          h225ParamNodeId - node id for the h225 parameters.
 * output : none
 * return : none
 * NOTE: origin should be the origin of the MESSAGE - as the receive and
 *       transmit address association is
 * determined by the EP to send the message.
 ************************************************************************/
int h225cmcCallAddressCallbacks(
    IN H245Object*  app,
    IN H245Channel* pChannel,
    IN RvPvtNodeId  messageNodeId)
{
    RvPvtNodeId tmpNodeId, h225ParamNodeId, nullDataType;
    HPVT hVal = app->hVal;

    __pvtGetByFieldIds(h225ParamNodeId, hVal, messageNodeId,
        {_h245(forwardLogicalChannelParameters) _h245(multiplexParameters) _h245(h2250LogicalChannelParameters) LAST_TOKEN }, NULL, NULL, NULL);

    /* maybe it's an ACK message, so the node is somewhere else. */
    if (!RV_PVT_NODEID_IS_VALID(h225ParamNodeId))
    {
        __pvtGetByFieldIds(h225ParamNodeId, hVal, messageNodeId,
            {_h245(forwardMultiplexAckParameters) _h245(h2250LogicalChannelAckParameters) LAST_TOKEN }, NULL, NULL, NULL);
    }

    /* maybe it's a fast-connect reverse channel, look for it in the reverse parameters. */
    __pvtGetByFieldIds(nullDataType, hVal, messageNodeId,
        {_h245(forwardLogicalChannelParameters) _h245(dataType) _h245(nullData) LAST_TOKEN }, NULL, NULL, NULL);
    if ((!RV_PVT_NODEID_IS_VALID(h225ParamNodeId)) && (RV_PVT_NODEID_IS_VALID(nullDataType)))
    {
        __pvtGetByFieldIds(h225ParamNodeId, hVal, messageNodeId,
            {_h245(reverseLogicalChannelParameters) _h245(multiplexParameters) _h245(h2250LogicalChannelParameters) LAST_TOKEN }, NULL, NULL, NULL);
    }

    tmpNodeId = pvtGetChild(hVal, h225ParamNodeId, __h245(mediaControlChannel), NULL);
    if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
    { /* -- RTCP */
        cmTransportAddress ta;
        if (cmVtToTA_H245(hVal, tmpNodeId, &ta) < 0)
        {
            RvLogWarning(&app->log, (&app->log,
                "H245Channel:establishIndication:RTCP: no IP address."));
        }
#if (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY)
        else
        {
            int nesting;
            if (app->cmMyChannelEvent.cmEvChannelSetRTCPAddress != NULL)
            {
                H245CBEnter((app->pAppObject, "cmEvChannelSetRTCPAddress: haChan=%p, hsChan=%p, ip=0x%x, port=%d.",
                    emaGetApplicationHandle((EMAElement)pChannel), pChannel, ta.ip, ta.port));
                nesting = emaPrepareForCallback((EMAElement)pChannel);
                app->cmMyChannelEvent.cmEvChannelSetRTCPAddress((HAPPCHAN)emaGetApplicationHandle((EMAElement)pChannel), (HCHAN)pChannel, ta.ip, ta.port);
                emaReturnFromCallback((EMAElement)pChannel, nesting);
                H245CBExit((app->pAppObject, "cmEvChannelSetRTCPAddress."));
            }
        }
#endif /* (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY) */
        if (!emaWasDeleted((EMAElement)pChannel))
        {
            if (!RV_PVT_NODEID_IS_VALID(pChannel->data.h225Params.sendRtcpAddressID))
                pChannel->data.h225Params.sendRtcpAddressID = pvtAddRoot(hVal, NULL, 0, NULL);
            pvtSetTree(hVal, pChannel->data.h225Params.sendRtcpAddressID, hVal, tmpNodeId);
        }
    }

    tmpNodeId = pvtGetChild(hVal, h225ParamNodeId, __h245(mediaChannel), NULL);
    if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
    { /* -- RTP */
        cmTransportAddress ta;
        if (cmVtToTA_H245(hVal, tmpNodeId, &ta) < 0)
        {
            RvLogWarning(&app->log, (&app->log,
                "H245Channel:establishIndication:RTP: no IP address."));
        }
#if (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY)
        else
        {
            int nesting;
            switch (ta.type)
            {
                case cmTransportTypeIP:
                    if (app->cmMyChannelEvent.cmEvChannelSetAddress != NULL)
                    {
                        HAPPCHAN haChan = emaGetApplicationHandle((EMAElement)pChannel);
                        H245CBEnter((app->pAppObject, "cmEvChannelSetAddress(haChan=%p,hsChan=%p,ip=0x%x,port=%d)", haChan, pChannel, ta.ip, ta.port));
                        nesting = emaPrepareForCallback((EMAElement)pChannel);
                        app->cmMyChannelEvent.cmEvChannelSetAddress(haChan, (HCHAN)pChannel, ta.ip, ta.port);
                        emaReturnFromCallback((EMAElement)pChannel, nesting);
                        H245CBExit((app->pAppObject, "cmEvChannelSetAddress"));
                    }
                break;
#if (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_IPV4_ONLY)
                case cmTransportTypeNSAP:
                {
                    /* ToDo: maybe fix for TRANSPORT_ADDRESS_ANY, either here or in cmChannelGetParam */
                    int iLength;

                    if (app->cmMyChannelEvent.cmEvChannelSetNSAPAddress != NULL)
                    {
                        HAPPCHAN haChan = emaGetApplicationHandle((EMAElement)pChannel);
                        H245CBEnter((app->pAppObject, "cmEvChannelSetNSAPAddress(haChan=%p,hsChan=%p,addressLength=%d)", haChan, pChannel, ta.length));
                        nesting = emaPrepareForCallback((EMAElement)pChannel);
                        iLength = ta.length;
                        app->cmMyChannelEvent.cmEvChannelSetNSAPAddress(haChan, (HCHAN)pChannel, (char *)ta.route,iLength,
                                                                    ta.distribution==cmDistributionMulticast);
                        emaReturnFromCallback((EMAElement)pChannel, nesting);
                        H245CBExit((app->pAppObject, "cmEvChannelSetNSAPAddress"));
                    }

                    if (!emaWasDeleted((EMAElement)pChannel) && (pChannel->remotePortNumber >= 0) && (app->cmMyChannelEvent.cmEvChannelSetATMVC != NULL))
                    {
                        HAPPCHAN haChan = emaGetApplicationHandle((EMAElement)pChannel);
                        H245CBEnter((app->pAppObject, "cmEvChannelSetATMVC(haChan=%p,hsChan=%p,portNumber=%d)", haChan, pChannel, pChannel->remotePortNumber));
                        nesting = emaPrepareForCallback((EMAElement)pChannel);
                        app->cmMyChannelEvent.cmEvChannelSetATMVC(haChan, (HCHAN)pChannel, pChannel->remotePortNumber);
                        emaReturnFromCallback((EMAElement)pChannel, nesting);
                        H245CBExit((app->pAppObject, "cmEvChannelSetATMVC"));
                    }
                }
                break;
#endif /* (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_IPV4_ONLY) */
                default:
                break;
            }
        }
#endif /* (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY) */
        if (!emaWasDeleted((EMAElement)pChannel))
        {
            if (!RV_PVT_NODEID_IS_VALID(pChannel->data.h225Params.sendRtpAddressID))
                pChannel->data.h225Params.sendRtpAddressID = pvtAddRoot(hVal, NULL, 0, NULL);
            pvtSetTree(hVal, pChannel->data.h225Params.sendRtpAddressID, hVal, tmpNodeId);
        }
    }

    /* now check if this is a duplex channel, and if we have the reverse RTP address as well */
    if (pChannel->bIsDuplex)
    {
        __pvtGetByFieldIds(tmpNodeId, hVal, messageNodeId,
            {_h245(reverseLogicalChannelParameters) _h245(multiplexParameters)
             _h245(h2250LogicalChannelParameters) _h245(mediaChannel) LAST_TOKEN }, NULL, NULL, NULL);

        if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
        { /* -- RTP */
            cmTransportAddress ta;
            if (cmVtToTA_H245(hVal, tmpNodeId, &ta) < 0)
            {
                RvLogWarning(&app->log, (&app->log,
                    "H245Channel:establishIndication:RTP: bad reverse IP address."));
            }
#if (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY)
            else if (app->cmMyChannelEvent.cmEvChannelSetAddress != NULL)
            {
                int nesting;
                HAPPCHAN haChan = emaGetApplicationHandle((EMAElement)pChannel);
                H245CBEnter((app->pAppObject, "cmEvChannelSetAddress(haChan=%p,hsChan=%p,ip=0x%x,port=%d)", haChan, pChannel, ta.ip, ta.port));
                nesting = emaPrepareForCallback((EMAElement)pChannel);
                app->cmMyChannelEvent.cmEvChannelSetAddress(haChan, (HCHAN)pChannel, ta.ip, ta.port);
                emaReturnFromCallback((EMAElement)pChannel, nesting);
                H245CBExit((app->pAppObject, "cmEvChannelSetAddress"));
            }
#endif /* (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY) */
            if (!emaWasDeleted((EMAElement)pChannel))
            {
                if (!RV_PVT_NODEID_IS_VALID(pChannel->data.h225Params.sendRtpAddressID))
                    pChannel->data.h225Params.sendRtpAddressID = pvtAddRoot(hVal, NULL, 0, NULL);
                pvtSetTree(hVal, pChannel->data.h225Params.sendRtpAddressID, hVal, tmpNodeId);
            }
        }
    }
    return RV_TRUE;
}



/************************************************************************
 * h225ParamsOpenLogicalChannelEvent
 * purpose: Open logical channel event.
 * input  : pApp            - application handle for the cm.
 *          hVal            - handle for the PVT.
 *          forwardLCP_ID   - node id for the forward open logical channel parameters.
 *          reverseLCP_ID   - node id for the reverse data type.
 *          message         - node id for the message.
 *          pChannel        - Channel element to set h225 parameters for.
 * output : h225ID          - the node id of the h225 parameters.
 *          bIsReturn       - if true then there was a return value, otherwise there wasn't.
 * return : int
 ************************************************************************/
int h225ParamsOpenLogicalChannelEvent(
    IN H245Object    *app,
    IN HPVT          hVal,
    IN RvPvtNodeId   forwardLCP_ID,
    IN RvPvtNodeId   reverseLCP_ID,
    IN RvPvtNodeId   message,
    IN H245Channel   *pChannel,
    OUT RvPvtNodeId  *h225ID,
    OUT RvBool       *bIsReturn)
{
    int          nesting;
    H245Control  *ctrl = (H245Control*)pChannel->hCtrl;
    *bIsReturn = RV_FALSE;

    RV_UNUSED_ARG(reverseLCP_ID);

    /* separateStack */
    if (pChannel->bIsDuplex)
    {
        RvPvtNodeId seperateStackNode, tmpId;

        if (pvtGetChild(hVal, message, __h245(separateStack), &seperateStackNode) >= 0)
        {
            __pvtGetNodeIdByFieldIds(tmpId, hVal, seperateStackNode, {_h245(networkAddress) _h245(localAreaAddress) LAST_TOKEN});
            if (RV_PVT_NODEID_IS_VALID(tmpId))
            {
                if (!RV_PVT_NODEID_IS_VALID(pChannel->data.h225Params.separateStackID))
                    pChannel->data.h225Params.separateStackID = pvtAddRoot(hVal, NULL, 0, NULL);
                pvtMoveTree(hVal, pChannel->data.h225Params.separateStackID, tmpId);
            }

            __pvtGetNodeIdByFieldIds(tmpId, hVal, seperateStackNode, {_h245(t120SetupProcedure) _anyField LAST_TOKEN});
            if (RV_PVT_NODEID_IS_VALID(tmpId))
            {
                RvPstFieldId fieldId = 0;
                pvtGet(hVal, tmpId, &fieldId, NULL, NULL, NULL);
                if (fieldId == __h245(originateCall))
                    pChannel->data.h225Params.t120SetupProcedure = cmOriginateCall;
                else if (fieldId == __h245(waitForCall))
                    pChannel->data.h225Params.t120SetupProcedure = cmWaitForCall;
                else if (fieldId == __h245(issueQuery))
                    pChannel->data.h225Params.t120SetupProcedure = cmIssueQuery;
                else
                    pChannel->data.h225Params.t120SetupProcedure = (cmT120SetupProcedure)RV_ERROR_UNKNOWN;
            }
            else
                pChannel->data.h225Params.t120SetupProcedure = (cmT120SetupProcedure)RV_ERROR_UNKNOWN;

            if(pvtGetChild(hVal, seperateStackNode, __h245(associateConference), &tmpId) >= 0)
                pvtGet(hVal, tmpId, NULL, NULL, (RvInt32*)&pChannel->data.h225Params.isAssociated, NULL);
            if(pvtGetChild(hVal, seperateStackNode, __h245(externalReference), &tmpId) >= 0)
            {
                pChannel->data.h225Params.externalReferenceLength =
                    pvtGetString(hVal, tmpId, 256, pChannel->data.h225Params.externalReference);
            }
            else
            {
                pChannel->data.h225Params.externalReferenceLength = 0;
                pChannel->data.h225Params.externalReference[0] = 0;
            }
        }
    }

    /* H.225 section. */
    *h225ID=pvtGetChild2(hVal,forwardLCP_ID, __h245(multiplexParameters), __h245(h2250LogicalChannelParameters));
    if (*h225ID>=0)
    {
        /* handle session id.*/
        RvInt32 sidValue = 0;
        pvtGetChildByFieldId(hVal, *h225ID, __h245(sessionID), &sidValue, NULL);
        pChannel->data.h225Params.sessionId = (RvUint8)sidValue;
        if (sidValue == 0)
        {
            int appSetSid = 0;

            if (!pChannel->bIsDuplex)
            {
                if (ctrl->conflictChannels)
                {
                    HCHAN confChannels[CONFLICT_CHANNELS_ARRAY_SIZE];
                    H245Channel * channel;
                    confDataType inDataType;
                    void * ptr = NULL;
                    int i = 0;

                    /* first, find out which type of channel is causing the conflict */
                    confGetDataTypeName(hVal, pChannel->dataTypeID, 0, NULL, &inDataType, NULL);

                    /* collect the conflicting channels by data type */
                    channel = getNextOutChan((HCONTROL)ctrl, &ptr);
                    while ((channel != NULL) && (i < CONFLICT_CHANNELS_ARRAY_SIZE))
                    {
                        if (channel->eState == ChannelStateAwaitingEstablishment)
                        {
                            confDataType outDataType;
                            confGetDataTypeName(hVal, channel->dataTypeID, 0, NULL, &outDataType, NULL);
                            if (outDataType == inDataType)
                            {
                                /* add to the array */
                                confChannels[i] = (HCHAN)channel;
                                i++;
                            }
                        }
                        channel = getNextOutChan((HCONTROL)ctrl, &ptr);
                    }

                    /* check if there really is a conflict */
                    if (i != 0)
                    {
                        if (app->cmMyChannelEvent.cmEvChannelMasterSlaveConflict != NULL)
                        {
                            HAPPCHAN haChan = (HAPPCHAN)emaGetApplicationHandle((EMAElement)pChannel);
                            /* let the application solve the conflict */
                            H245CBEnter((app->pAppObject,
                                "cmEvChannelMasterSlaveConflict (Unanswered Proposals): haChan=%p, hsChan=%p.", haChan, pChannel));
                            nesting = emaPrepareForCallback((EMAElement)pChannel);

                            appSetSid = app->cmMyChannelEvent.cmEvChannelMasterSlaveConflict(
                                haChan, (HCHAN)pChannel, cmH245ConflictUnansweredProposals, confChannels, i, message);

                            emaReturnFromCallback((EMAElement)pChannel, nesting);
                            H245CBExit((app->pAppObject, "cmEvChannelMasterSlaveConflict."));
                        }
                        else
                            /* reject with masterSlaveConflict */
                            appSetSid = -1;
                    }
                }

                if (appSetSid < 0)
                {
                    /* conflict, reject */
                    *bIsReturn = RV_TRUE;
                    return rejectChannel(pChannel, __h245(masterSlaveConflict),
                        cmChannelStateModeDisconnectedMasterSlaveConflict);
                }
                else if (appSetSid > 0)
                {
                    /* conflict resolved by user, SID consistency and partner channels maintained
                       in the code below */
                    pChannel->data.h225Params.sessionId = (RvUint8)appSetSid;
                }
            }

            if (appSetSid == 0)
            {
                /* new session */
                if (ctrl->bIsMaster || pChannel->bIsDuplex)
                { /* master*/
                    RvPstFieldId fieldId;
                    int nodeId = pvtGet(hVal, pvtChild(hVal, pChannel->dataTypeID), &fieldId, NULL, NULL, NULL);
                    if (fieldId==__h245(h235Media))
                    {
                        pvtGet(hVal, pvtChild(hVal, pvtGetChild(hVal, nodeId, __h245(mediaType), NULL)),
                            &fieldId, NULL, NULL, NULL);
                    }

                    if (pChannel->data.h225Params.bForceSecondary)
                    {
                        if ((pChannel->data.h225Params.sessionId =
                                getFreeSID((HCONTROL)ctrl, RV_FALSE, pChannel->bIsDuplex)) == 0)
                        {
                            *bIsReturn = RV_TRUE;
                            return rejectChannel(pChannel, __h245(invalidSessionID),
                                cmChannelStateModeDisconnectedLocal);
                        }
                    }
                    else if (ctrl->bFirstAudioIn && fieldId==__h245(audioData))
                    {
                        ctrl->bFirstAudioIn = RV_FALSE;
                        if (pChannel->bIsDuplex)
                            ctrl->bFirstAudioOut = RV_FALSE;
                        pChannel->data.h225Params.sessionId = 1;
                    }
                    else if (ctrl->bFirstVideoIn && fieldId==__h245(videoData))
                    {
                        ctrl->bFirstVideoIn = RV_FALSE;
                        if (pChannel->bIsDuplex)
                            ctrl->bFirstVideoOut = RV_FALSE;
                        pChannel->data.h225Params.sessionId = 2;
                    }
                    else if (ctrl->bFirstDataIn && fieldId==__h245(data))
                    {
                        ctrl->bFirstDataIn = RV_FALSE;
                        if (pChannel->bIsDuplex)
                            ctrl->bFirstDataOut = RV_FALSE;
                        pChannel->data.h225Params.sessionId = 3;
                    }
                    else
                    {
                        if ((pChannel->data.h225Params.sessionId =
                                getFreeSID((HCONTROL)ctrl, RV_FALSE, pChannel->bIsDuplex)) == 0)
                        {
                            *bIsReturn = RV_TRUE;
                            return rejectChannel(pChannel, __h245(invalidSessionID),
                                cmChannelStateModeDisconnectedLocal);
                        }
                    }
                }
                else
                {
                    /* slave */
                    *bIsReturn = RV_TRUE;
                    return rejectChannel(pChannel, __h245(invalidSessionID),
                        cmChannelStateModeDisconnectedLocal);
                }
            }
        }
        /* do not use "else" here, but instead check if we miraculously got a session ID from somewhere */
        if (pChannel->data.h225Params.sessionId != 0)
        {
            /* existing session association: check db consistency */
            H245Channel *partnerChannel;

            if (checkChanSIDConsistency((HCONTROL)ctrl, pChannel, pChannel->data.h225Params.sessionId, pChannel->bOrigin, pChannel->bIsDuplex))
            {
                /* was the bad SID given by the application or was it in the offer? */
                if (sidValue != 0)
                {
                    /* incoming channel SID exists */
                    *bIsReturn = RV_TRUE;
                    return rejectChannel(pChannel,__h245(invalidSessionID),cmChannelStateModeDisconnectedLocal);
                }
                else
                {
                    /* this SID was given by the application. we must ignore it and threat this as a
                       master slave conflict */
                    *bIsReturn = RV_TRUE;
                    return rejectChannel(pChannel, __h245(masterSlaveConflict),
                        cmChannelStateModeDisconnectedMasterSlaveConflict);
                }
            }

            /* update db.first */
            if (pChannel->data.h225Params.sessionId==1)
            {
                ctrl->bFirstAudioIn=RV_FALSE;
                if (pChannel->bIsDuplex)
                    ctrl->bFirstAudioOut=RV_FALSE;
            }
            if (pChannel->data.h225Params.sessionId==2)
            {
                ctrl->bFirstVideoIn=RV_FALSE;
                if (pChannel->bIsDuplex)
                    ctrl->bFirstVideoOut=RV_FALSE;
            }
            if (pChannel->data.h225Params.sessionId==3)
            {
                ctrl->bFirstDataIn=RV_FALSE;
                if (pChannel->bIsDuplex)
                    ctrl->bFirstDataOut=RV_FALSE;
            }

            /* link partner rtcp address */
            partnerChannel = getOutChanBySID((HCONTROL)ctrl, pChannel->data.h225Params.sessionId);
            if (partnerChannel != NULL)
            {
                /* Link back */
                pChannel->data.h225Params.pPartner = partnerChannel;
                partnerChannel->data.h225Params.pPartner = pChannel;
            }
        }
        /* link associated session */
        {
            RvInt32 sid;
            if (pvtGetChildByFieldId(hVal,*h225ID,__h245(associatedSessionID),&sid,NULL)>=0)
                pChannel->data.h225Params.pAssociated = getInChanBySID((HCONTROL)ctrl, (RvUint8)sid);
        }

    } /* handleSessionId() */

    {
        int tmpId;
        tmpId=pvtGetChild(hVal,*h225ID,__h245(transportCapability),NULL);
        if (tmpId>=0)
        {
            if (pChannel->data.h225Params.transCapID<0)
                pChannel->data.h225Params.transCapID=pvtAddRoot(hVal,NULL,0,NULL);
            pvtMoveTree(hVal,pChannel->data.h225Params.transCapID,tmpId);
        }
        else if (pChannel->data.h225Params.transCapID>=0)
        {
            pvtDelete(hVal, pChannel->data.h225Params.transCapID);
            pChannel->data.h225Params.transCapID = RV_PVT_INVALID_NODEID;
        }
        tmpId=pvtGetChild(hVal,*h225ID,__h245(redundancyEncoding),NULL);
        if (tmpId>=0)
        {
            if (pChannel->data.h225Params.redEncID<0)
                pChannel->data.h225Params.redEncID=pvtAddRoot(hVal,NULL,0,NULL);
            pvtMoveTree(hVal,pChannel->data.h225Params.redEncID,tmpId);
        }
        tmpId=pvtGetChild(hVal,*h225ID,__h245(source),NULL);
        if (tmpId>=0)
        {
            int tmp;
            pvtGetChildByFieldId(hVal,tmpId,__h245(mcuNumber), &(tmp),NULL);
            pChannel->data.h225Params.source.mcuNumber = (RvUint8)tmp;
            pvtGetChildByFieldId(hVal,tmpId,__h245(terminalNumber), &(tmp),NULL);
            pChannel->data.h225Params.source.terminalNumber = (RvUint8)tmp;
        }
        tmpId=pvtGetChild(hVal,*h225ID,__h245(destination),NULL);
        if (tmpId>=0)
        {
            int tmp;
            pvtGetChildByFieldId(hVal,tmpId,__h245(mcuNumber), &(tmp),NULL);
            pChannel->data.h225Params.destination.mcuNumber = (RvUint8)tmp;
            pvtGetChildByFieldId(hVal,tmpId,__h245(terminalNumber), &(tmp),NULL);
            pChannel->data.h225Params.destination.terminalNumber = (RvUint8)tmp;
        }
    }

    /* set the media-channel and media-control-channel addresses */
    h22xFunctions[RvH245ChannelParamsH225].cmcCallAddressCallbacks(app, pChannel, message);

    if (!emaWasDeleted((EMAElement)pChannel))
    {
        if (pChannel->pReplacementCh && app->cmMyChannelEvent.cmEvChannelReplace)
        {
            H245CBEnter((app->pAppObject, "cmEvChannelReplace: haChan=%p, hsChan=%p, haReplaceChan=%p, hsReplaceChan=%p.",
                emaGetApplicationHandle((EMAElement)pChannel), pChannel,
                emaGetApplicationHandle((EMAElement)pChannel->pReplacementCh), pChannel->pReplacementCh));
            nesting = emaPrepareForCallback((EMAElement)pChannel);
            app->cmMyChannelEvent.cmEvChannelReplace((HAPPCHAN)emaGetApplicationHandle((EMAElement)pChannel),               (HCHAN)pChannel,
                                                     (HAPPCHAN)emaGetApplicationHandle((EMAElement)pChannel->pReplacementCh),(HCHAN)pChannel->pReplacementCh);
            emaReturnFromCallback((EMAElement)pChannel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelReplace."));

        }
    }
    return RV_OK;
}

/************************************************************************
 * h225ParamsDynamicRTPPayloadType
 * purpose: Dynamic RTP payload type event.
 * input  : pApp            - application handle for the cm.
 *          hVal            - handle for the PVT.
 *          h225ID          - node id for the h225 parameters.
 *          pChannel        - Channel element to set h225 parameters for.
 * output : none
 * return : none
 ************************************************************************/
void h225ParamsDynamicRTPPayloadType(
    IN H245Object*  app,
    IN HPVT         hVal,
    IN RvPvtNodeId  h225ID,
    IN H245Channel* pChannel)
{
    int nesting;

    if (!pChannel->bIsDuplex)
    { /* -- dynamic RTP payload type */
        RvInt32 num;

        if((pvtGetChildByFieldId(hVal,h225ID,__h245(dynamicRTPPayloadType),&num,NULL)>=0)&&
           (app->cmMyChannelEvent.cmEvChannelRTPDynamicPayloadType))
        {
            H245CBEnter((app->pAppObject, "cmEvChannelRTPDynamicPayloadType: haCall=%p, hsCall=%p, type=%d.", emaGetApplicationHandle((EMAElement)pChannel), pChannel, num));
            nesting = emaPrepareForCallback((EMAElement)pChannel);
            app->cmMyChannelEvent.cmEvChannelRTPDynamicPayloadType((HAPPCHAN)emaGetApplicationHandle((EMAElement)pChannel), (HCHAN)pChannel, (RvInt8)num);
            emaReturnFromCallback((EMAElement)pChannel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelRTPDynamicPayloadType."));
        }
    }
}


/************************************************************************
 * h225MaximumSkewIndication
 * purpose: notifing the application on the maximumSkew for a channel.
 * input  : ctrl    - the h245 control object of the call.
 *          message - incoming h2250MaximumSkewIndication message.
 * output : none
 * return : int - negative number will be error.
 *                positive number will be success.
 ************************************************************************/
int h225MaximumSkewIndication(H245Control* ctrl,RvPvtNodeId message)
{
    HPVT hVal;
    EMAElement callE;
    H245Object* app;
    H245Channel* channel1,* channel2;
    RvInt32 lcn1, lcn2, maxSkew;
    int nesting;

    callE = (EMAElement)cmiGetByControl((HCONTROL)ctrl);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance(callE));

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelMaxSkew != NULL)
    {
        hVal=app->hVal;

        if (emaLock(callE))
        {
            pvtGetChildByFieldId(hVal, message, __h245(logicalChannelNumber1), &lcn1, NULL);
            pvtGetChildByFieldId(hVal, message, __h245(logicalChannelNumber2), &lcn2, NULL);
            pvtGetChildByFieldId(hVal, message, __h245(maximumSkew), &maxSkew, NULL);

            channel1=getInChanByLCN((HCONTROL)ctrl,lcn1);
            channel2=getInChanByLCN((HCONTROL)ctrl,lcn2);

            H245CBEnter((app->pAppObject, "cmEvChannelMaxSkew: haCall=%p, hsCall=%p, chan1=%p, chan2=%p, skew=%d.",
                emaGetApplicationHandle((EMAElement)channel1), channel1, emaGetApplicationHandle((EMAElement)channel2), channel2, maxSkew));

            nesting = emaPrepareForCallback(callE);
            app->cmMyChannelEvent.cmEvChannelMaxSkew(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel1), (HCHAN)channel1,
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel2), (HCHAN)channel2, (RvUint32)maxSkew);
            emaReturnFromCallback(callE, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMaxSkew."));

            emaUnlock(callE);
        }
    }
    return 0;
}

/************************************************************************
 * h225communicationModeCommand
 * purpose: getting h225 parameters from communicationModeCommand message.
 * input  : pCtrl    - the H245 control object.
 *          message  - the communicationModeCommand message node id.
 * output : none
 * return : int - negative number will be error.
 *                positive number will be success.
 ************************************************************************/
int h225communicationModeCommand(H245Control* pCtrl, RvPvtNodeId message)
{
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)pCtrl)));
    H245Object * app = (H245Object *)hApp;
    HPVT hVal = app->hVal;
    void* ptr=NULL;
    H245Channel* channel;
    RvPvtNodeId tableNodeId, elemNodeId;


    if (pCtrl->bIsMaster) return RV_TRUE;

    tableNodeId = pvtGetChild(hVal,message, __h245(communicationModeTable), NULL);

    /* go over all outgoing channels */
    channel = getNextOutChan((HCONTROL)pCtrl, &ptr);
    while (channel != NULL)
    {
        if (emaLock((EMAElement)channel))
        {
            /* try to match each channel to an entry in the communication mode table */
            elemNodeId = pvtChild(hVal,tableNodeId); /* elemNodeId = communicationModeTable.1 */
            while (elemNodeId >= 0)
            {
                /* first try to match Session ID */
                RvInt32 sid;
                pvtGetChildByFieldId(hVal, elemNodeId, __h245(sessionID), &sid, NULL);
                if (sid == (RvInt32)channel->data.h225Params.sessionId)
                {
                    /* now see if the data type is identical */
                    int dataTypeId=pvtGetChild(hVal, elemNodeId, __h245(dataType), NULL);
                    if (pvtCompareTree(hVal, pvtChild(hVal,dataTypeId), hVal, pvtChild(hVal,channel->dataTypeID))==RV_TRUE)
                    {
                        /* and make sure the RTCP address is identical */
                        int mediaControlChannelId=pvtGetChild(hVal, elemNodeId, __h245(mediaControlChannel), NULL);
                        if (pvtCompareTree(hVal, mediaControlChannelId, hVal, channel->data.h225Params.sendRtcpAddressID)==RV_TRUE)
                        {
                            /* and the redundancy encoding */
                            int redundancyEncodingId=pvtGetChild(hVal, elemNodeId, __h245(redundancyEncoding), NULL);
                            if (pvtCompareTree(hVal, redundancyEncodingId, hVal, channel->data.h225Params.redEncID)==RV_TRUE)
                            {
                                /* and lastly, make sure the session dependency is the same */
                                RvInt32 sessionDependency;
                                int sdPresent=(pvtGetChildByFieldId(hVal, elemNodeId, __h245(sessionDependency), &sessionDependency, NULL)>=0);
                                if (!sdPresent && !channel->pBase)
                                    /* Found */
                                    break;
                                if (channel->pBase && (RvInt32)channel->pBase->data.h225Params.sessionId == sessionDependency)
                                    /* Found */
                                    break;
                            }
                        }
                    }
                }
                elemNodeId = pvtBrother(hVal,elemNodeId); /* elemNodeId = communicationModeTable.(i++) */
            }

            if (elemNodeId < 0)/* Not Found */
            {
                /* Drop the channel that is not in the table of the communicationModeCommand */
                cmChannelDrop((HCHAN)channel);
            }
            emaUnlock((EMAElement)channel);
        }

        /* Next... */
        channel = getNextOutChan((HCONTROL)pCtrl, &ptr);
    }

    elemNodeId = pvtChild(hVal,tableNodeId); /* elemNodeId = communicationModeTable.1 */
    while (RV_PVT_NODEID_IS_VALID(elemNodeId))
    {
        RvInt32 sid;
        int dataTypeId;
        int terminalLabelId;
        int mediaControlChannelId, mediaChannelId;
        int redundancyEncodingId;
        RvInt32 sessionDependency = -1;

        cmTerminalLabel termLabel= {0, 0};

        /* get the terminal label the current table entry is referring to, if present */
        if ((terminalLabelId=pvtGetChild(hVal, elemNodeId, __h245(terminalLabel), NULL))>=0)
        {
            int tmp;
            pvtGetChildByFieldId(hVal, terminalLabelId, __h245(mcuNumber), &tmp, NULL);
            termLabel.mcuNumber=(RvUint8)tmp;
            pvtGetChildByFieldId(hVal, terminalLabelId, __h245(terminalNumber), &tmp, NULL);
            termLabel.terminalNumber=(RvUint8)tmp;
        }

        /* also get the session ID, data type, RTCP, redundancy encoding and dependency */
        pvtGetChildByFieldId(hVal, elemNodeId, __h245(sessionID), &sid, NULL);
        dataTypeId=pvtGetChild(hVal, elemNodeId, __h245(dataType), NULL);
        mediaControlChannelId=pvtGetChild(hVal, elemNodeId, __h245(mediaControlChannel), NULL);
        redundancyEncodingId=pvtGetChild(hVal, elemNodeId, __h245(redundancyEncoding), NULL);
        pvtGetChildByFieldId(hVal, elemNodeId, __h245(sessionDependency), &sessionDependency, NULL);

        /* see if this entry refers to this terminal */
        if (terminalLabelId<0 || (termLabel.mcuNumber==pCtrl->myTerminalLabel.mcuNumber && termLabel.terminalNumber==pCtrl->myTerminalLabel.terminalNumber))
        {
            /* try to find the channel it refers to */
            ptr=NULL;
            channel = getNextOutChan((HCONTROL)pCtrl, &ptr);
            while (channel != NULL)
            {
                if (sid == (RvInt32)channel->data.h225Params.sessionId)
                    if (pvtCompareTree(hVal, pvtChild(hVal,dataTypeId), hVal, pvtChild(hVal,channel->dataTypeID))==RV_TRUE)
                        if (pvtCompareTree(hVal, mediaControlChannelId, hVal, channel->data.h225Params.sendRtcpAddressID)==RV_TRUE)
                            if (pvtCompareTree(hVal, redundancyEncodingId, hVal, channel->data.h225Params.redEncID)==RV_TRUE)
                                if ((sessionDependency==-1 && !channel->pBase) ||
                                    (channel->pBase && (RvInt32)channel->pBase->data.h225Params.sessionId==sessionDependency))
                                    /* Found */
                                    break;
                channel=getNextOutChan((HCONTROL)pCtrl, &ptr);
            }

            if (channel==NULL) /* Not Found */
            {
                H245Channel* newChannel=allocateChannel((HCONTROL)pCtrl);

                if (!newChannel)
                {
                    RvLogError(&app->log,
                        (&app->log, "communicationModeCommand: Unable to allocate channel"));
                    return RV_TRUE;
                }

                if (emaLock((EMAElement)newChannel))
                {
                    newChannel->hCtrl=(HCONTROL)pCtrl;
                    newChannel->bOrigin=RV_TRUE;

                    if (app->cmMySessionEvent.cmEvCallNewChannel)
                    {
                        H245CBEnter((app->pAppObject, "cmEvCallNewChannel:IN: haCall=%p, hsCall=%p. hsChan=%p", emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)pCtrl)),cmiGetByControl((HCONTROL)pCtrl),newChannel));
                        {
                            HAPPCHAN haChan=NULL;
                            int nesting = emaPrepareForCallback((EMAElement)newChannel);
                            app->cmMySessionEvent.cmEvCallNewChannel((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)pCtrl)), (HCALL)cmiGetByControl((HCONTROL)pCtrl),(HCHAN)newChannel, &haChan);
                            emaReturnFromCallback((EMAElement)newChannel, nesting);
                            emaSetApplicationHandle((EMAElement)newChannel,(void*)haChan);
                        }
                        H245CBExit((app->pAppObject, "cmEvCallNewChannel:IN: haChan=%p.", emaGetApplicationHandle((EMAElement)newChannel)));
                    }
                    if (emaWasDeleted((EMAElement)newChannel))
                    {
                        emaUnlock((EMAElement)newChannel);
                        continue;
                    }
                }

                if (dataTypeId >= 0)
                {
                    confDataType type;

                    newChannel->dataTypeID=pvtAddRoot(hVal,NULL,0,NULL);
                    pvtMoveTree(hVal,newChannel->dataTypeID,dataTypeId);

                    cmcCallChannelParametersCallback(hApp, (HCHAN)newChannel, newChannel->dataTypeID, &type);
                    if (!emaWasDeleted((EMAElement)newChannel))
                        cmcCallDataTypeHandleCallback(hApp, (HCHAN)newChannel, newChannel->dataTypeID, type);
                    if (emaWasDeleted((EMAElement)newChannel))
                    {
                        emaUnlock((EMAElement)newChannel);
                        continue;
                    }
                }
                else
                {
                    RvLogError(&app->log,
                        (&app->log, "communicationModeCommand: no data type."));
                    newChannel->eState = ChannelStateReleased;
                    notifyChannelState(newChannel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedReasonUnknown);
                    notifyChannelState(newChannel,cmChannelStateIdle, cmChannelStateModeOff);
                    emaUnlock((EMAElement)newChannel);
                    continue;
                }

                if (mediaControlChannelId>=0)
                {
                    newChannel->data.h225Params.recvRtcpAddressID=pvtAddRoot(hVal,NULL,0,NULL);
                    pvtMoveTree(hVal,newChannel->data.h225Params.recvRtcpAddressID,mediaControlChannelId);
                }

                mediaChannelId=pvtGetChild(hVal, elemNodeId, __h245(mediaChannel), NULL);
                if (mediaChannelId>=0)
                {
                    newChannel->data.h225Params.recvRtpAddressID=pvtAddRoot(hVal,NULL,0,NULL);
                    pvtMoveTree(hVal,newChannel->data.h225Params.recvRtpAddressID,mediaChannelId);
                }

                if (redundancyEncodingId >= 0)
                {
                    newChannel->data.h225Params.redEncID=pvtAddRoot(hVal,NULL,0,NULL);
                    pvtMoveTree(hVal,newChannel->data.h225Params.redEncID,redundancyEncodingId);
                }

                if (sessionDependency >= 0)
                {
                    int elemNodeId1 = pvtChild(hVal, tableNodeId); /* elemNodeId1 = communicationModeTable.1 */
                    while (elemNodeId1 >= 0)
                    {
                        RvInt32 sid1;
                        pvtGetChildByFieldId(hVal, elemNodeId1, __h245(sessionID), &sid1, NULL);
                        if (sid1==sessionDependency) newChannel->pBase=getOutChanBySID((HCONTROL)pCtrl,(RvUint8)sid1);
                        elemNodeId1 = pvtBrother(hVal,elemNodeId1); /* elemNodeId = communicationModeTable.(i++) */
                    }
                }
                newChannel->data.h225Params.sessionId = (RvUint8)sid;
                startEstablishment(hApp, (HCHAN)newChannel);

                emaUnlock((EMAElement)newChannel);
            }
        }
        elemNodeId = pvtBrother(hVal,elemNodeId); /* elemNodeId = communicationModeTable.(i++) */
    }
    return RV_TRUE;
}

/************************************************************************
 * h225AllocateChannel
 * purpose: initiating h225 parameters while allocating the channel.
 * input  : pChannel - the channel object.
 * output : none
 * return : none
 ************************************************************************/
void h225AllocateChannel(H245Channel* pChannel)
{
    pChannel->eType = RvH245ChannelParamsH225;
    pChannel->data.h225Params.sessionId=0;
    pChannel->data.h225Params.pPartner=NULL;
    pChannel->data.h225Params.pAssociated=NULL;
    pChannel->data.h225Params.fastStartChannelIndex=-1;
    pChannel->data.h225Params.dynamicPayloadNumber=0; /* 0 is invalid enough for us */
    pChannel->data.h225Params.bFlowControlToZero =0;
    pChannel->data.h225Params.recvRtcpAddressID=RV_PVT_INVALID_NODEID;
    pChannel->data.h225Params.recvRtpAddressID=RV_PVT_INVALID_NODEID;
    pChannel->data.h225Params.sendRtcpAddressID=RV_PVT_INVALID_NODEID;
    pChannel->data.h225Params.sendRtpAddressID=RV_PVT_INVALID_NODEID;
    pChannel->data.h225Params.redEncID=RV_PVT_INVALID_NODEID;
    pChannel->data.h225Params.transCapID=RV_PVT_INVALID_NODEID;
    pChannel->data.h225Params.separateStackID=RV_PVT_INVALID_NODEID;
    pChannel->data.h225Params.t120SetupProcedure=(cmT120SetupProcedure)RV_ERROR_UNKNOWN;
    pChannel->data.h225Params.portNumber=RV_ERROR_UNKNOWN;
    pChannel->data.h225Params.bForceSecondary = RV_FALSE;

    if (((H245Control*)(pChannel->hCtrl))->logicalChannelOut<1)
	    ((H245Control*)(pChannel->hCtrl))->logicalChannelOut=1;

}

/************************************************************************
 * h225ChannelFreeMemory
 * purpose: freeing h225 parameters while freeing the channel.
 * input  : pChannel - the channel object.
 *          hVal     - handle to the PVT.
 * output : none
 * return : none
 ************************************************************************/
void h225ChannelFreeMemory(H245Channel* pChannel, HPVT hVal)
{
    pvtDelete(hVal,pChannel->data.h225Params.redEncID);
    pChannel->data.h225Params.redEncID=RV_PVT_INVALID_NODEID;

    pvtDelete(hVal,pChannel->data.h225Params.transCapID);
    pChannel->data.h225Params.transCapID=RV_PVT_INVALID_NODEID;

    pvtDelete(hVal,pChannel->data.h225Params.recvRtpAddressID);
    pChannel->data.h225Params.recvRtpAddressID=RV_PVT_INVALID_NODEID;

    pvtDelete(hVal,pChannel->data.h225Params.recvRtcpAddressID);
    pChannel->data.h225Params.recvRtcpAddressID=RV_PVT_INVALID_NODEID;

    pvtDelete(hVal,pChannel->data.h225Params.sendRtpAddressID);
    pChannel->data.h225Params.sendRtpAddressID=RV_PVT_INVALID_NODEID;

    pvtDelete(hVal,pChannel->data.h225Params.sendRtcpAddressID);
    pChannel->data.h225Params.sendRtcpAddressID=RV_PVT_INVALID_NODEID;

    pvtDelete(hVal,pChannel->data.h225Params.separateStackID);
    pChannel->data.h225Params.separateStackID=RV_PVT_INVALID_NODEID;
}


/************************************************************************
 * h255ParamsChannelAnswer
 * purpose: Start establishing a channel. This function adds h255 parameters into
 *          an OLC message from the channel element.
 * input  : hVal          - handle to the PVT.
 *          pChannel      - Channel element to add h225 parameters OLC for.
 *          pCtrl         - Control element.
 *          olcID         - node id for the open logical channel.
 * output : none
 * return : RV_OK on success, other on failure
 ************************************************************************/
RvStatus h255ParamsChannelAnswer(
    IN HPVT        hVal,
    IN H245Channel *pChannel,
    IN H245Control *pCtrl,
    IN RvPvtNodeId olcaID)
{
    RvPvtNodeId h225ID;
    RvPvtNodeId reverseLCN_ID;

    h225ID=pvtAddBranch2(hVal,olcaID, __h245(forwardMultiplexAckParameters), __h245(h2250LogicalChannelAckParameters));
    {
      /* Link partner rtcp address */
        H245Channel *partnerChannel = pChannel->data.h225Params.pPartner;

        if ((partnerChannel != NULL) && (partnerChannel->data.h225Params.recvRtcpAddressID >= 0))
            pvtSetTree(hVal, pvtAddBranch(hVal, h225ID, __h245(mediaControlChannel)), hVal, partnerChannel->data.h225Params.recvRtcpAddressID);
        else if (pChannel->data.h225Params.recvRtcpAddressID >= 0)
            pvtSetTree(hVal, pvtAddBranch(hVal, h225ID, __h245(mediaControlChannel)), hVal, pChannel->data.h225Params.recvRtcpAddressID);

        if (pChannel->data.h225Params.recvRtpAddressID >= 0)
            pvtSetTree(hVal, pvtAddBranch(hVal, h225ID, __h245(mediaChannel)), hVal, pChannel->data.h225Params.recvRtpAddressID);

        if ((pChannel->data.h225Params.portNumber > 0) && (pChannel->data.h225Params.portNumber < 65536))
            pvtAdd(hVal, h225ID, __h245(portNumber), pChannel->data.h225Params.portNumber, NULL, NULL);

        pvtAdd(hVal, h225ID, __h245(sessionID), pChannel->data.h225Params.sessionId, NULL, NULL);
    }

    /* -- dynamic payload */
    if (pChannel->data.h225Params.dynamicPayloadNumber > 0)
        pvtAdd(hVal,h225ID,__h245(dynamicRTPPayloadType) ,pChannel->data.h225Params.dynamicPayloadNumber ,NULL,NULL);
    /* -- flow control to 0 */
    if (pChannel->data.h225Params.bFlowControlToZero)
        pvtAdd(hVal,h225ID,__h245(flowControlToZero),(RvInt32)pChannel->data.h225Params.bFlowControlToZero,NULL,NULL);

    if (pChannel->bIsDuplex)
    { /* (for duplex channels). Declare reverse pChannel with lcn. */
        pCtrl->logicalChannelOut++;
        pChannel->reverseLogicalChannelNum=pCtrl->logicalChannelOut;
        reverseLCN_ID=pvtAddBranch(hVal,olcaID, __h245(reverseLogicalChannelParameters));
        pvtAdd(hVal,reverseLCN_ID, __h245(reverseLogicalChannelNumber), pChannel->reverseLogicalChannelNum,NULL,NULL);

        if (pChannel->pReplacementCh && pChannel->pReplacementCh->reverseLogicalChannelNum)
          pvtAdd(hVal,reverseLCN_ID, __h245(replacementFor), pChannel->pReplacementCh->reverseLogicalChannelNum, NULL,NULL);
        if (pChannel->data.h225Params.separateStackID>=0)
        {
            int separateStack;
            separateStack=pvtAddBranch(hVal,olcaID,__h245(separateStack));

            if (pChannel->data.h225Params.separateStackID>=0)
            {
                if (pChannel->data.h225Params.redEncID<0)
                    pChannel->data.h225Params.redEncID=pvtAddRoot(hVal,NULL,0,NULL);
                pvtMoveTree(hVal,pvtAddBranch2(hVal,separateStack,__h245(networkAddress), __h245(localAreaAddress)), pChannel->data.h225Params.separateStackID);
                pChannel->data.h225Params.separateStackID=RV_ERROR_UNKNOWN;
                if (pChannel->data.h225Params.externalReferenceLength>0)
                    pvtAdd(hVal,separateStack,__h245(externalReference),pChannel->data.h225Params.externalReferenceLength,pChannel->data.h225Params.externalReference,NULL);
                pvtAdd(hVal,separateStack,__h245(associateConference),(RvInt32)pChannel->data.h225Params.isAssociated,NULL,NULL);
            }

            if (((int)pChannel->data.h225Params.t120SetupProcedure >= (int)cmOriginateCall) &&
                ((int)pChannel->data.h225Params.t120SetupProcedure <= (int)cmIssueQuery))
            {
                int t120s[]={__h245(originateCall),__h245(waitForCall),__h245(issueQuery)};
                pvtAddBranch2(hVal,separateStack,__h245(t120SetupProcedure),t120s[(int)pChannel->data.h225Params.t120SetupProcedure-(int)cmOriginateCall]);
            }
        }
    }

    return RV_OK;
}

static
void establishConfirmed(H245Channel*channel, RvPvtNodeId message)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)channel);
    HPVT hVal;
    H245Object* app=(H245Object*)hApp;
    int nesting;
    RvPvtNodeId tmpId;
    RvPvtNodeId h225ID;

    hVal = app->hVal;

    app->evHandlers.pfnTimerCancelEv(channel->hCtrl, &channel->pTimer);

    /* set the media-channel and media-control-channel addresses */
    h22xFunctions[RvH245ChannelParamsH225].cmcCallAddressCallbacks(app, channel, message);

    __pvtGetByFieldIds(h225ID, hVal, message,
        {_h245(forwardMultiplexAckParameters) _h245(h2250LogicalChannelAckParameters) LAST_TOKEN }, NULL, NULL, NULL);

    tmpId=pvtGetChild(hVal,h225ID,__h245(transportCapability),NULL);
    if (tmpId>=0)
    {
        if (channel->data.h225Params.transCapID<0)
            channel->data.h225Params.transCapID=pvtAddRoot(hVal,NULL,0,NULL);
        pvtMoveTree(hVal,channel->data.h225Params.transCapID,tmpId);
    }
    else if (channel->data.h225Params.transCapID>=0)
    {
        pvtDelete(hVal, channel->data.h225Params.transCapID);
        channel->data.h225Params.transCapID = RV_PVT_INVALID_NODEID;
    }

    { /* -- dynamic RTP payload type */
        RvInt32 num;

        if((pvtGetChildByFieldId(hVal,h225ID,__h245(dynamicRTPPayloadType),&num,NULL)>=0)&&(app->cmMyChannelEvent.cmEvChannelRTPDynamicPayloadType))
        {
            H245CBEnter((app->pAppObject, "cmEvChannelRTPDynamicPayloadType: haCall=%p, hsCall=%p, type=%d.", emaGetApplicationHandle((EMAElement)channel), channel, num));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelRTPDynamicPayloadType((HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, (RvInt8)num);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelRTPDynamicPayloadType."));
        }
    }

    if (!emaWasDeleted((EMAElement)channel))
    { /* -- Parameters */
      /* Report the same session association handles */

        HCHAN hsSameSessionH=(HCHAN)channel->data.h225Params.pPartner;
        HAPPCHAN haSameSessionH=NULL;

        if (channel->data.h225Params.pPartner)
            haSameSessionH=(HAPPCHAN)emaGetApplicationHandle((EMAElement)channel->data.h225Params.pPartner);

        /* -- same session handle */
        if (app->cmMyChannelEvent.cmEvChannelParameters != NULL)
        {
            HAPPCHAN haChan = emaGetApplicationHandle((EMAElement)channel);
            H245CBEnter((app->pAppObject, "cmEvChannelParameters(haChan=%p,hsChan=%p,channelName=NULL,AppSes=%p,Ses=%p,NULL,NULL,rate=-1)",
                haChan, channel, haSameSessionH, hsSameSessionH));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelParameters(haChan, (HCHAN)channel, NULL, haSameSessionH, hsSameSessionH, NULL, NULL, (RvUint32)RV_ERROR_UNKNOWN);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelParameters"));
        }
    }

    notifyChannelState(channel, cmChannelStateConnected, cmChannelStateModeOn);

    if (!emaWasDeleted((EMAElement)channel) &&
        channel->data.h225Params.bFlowControlToZero &&
        (app->cmMyChannelEvent.cmEvChannelFlowControlToZero != NULL))
    {
        HAPPCHAN haChan = emaGetApplicationHandle((EMAElement)channel);
        H245CBEnter((app->pAppObject, "cmEvChannelFlowControlToZero(haChan=%p,hsChan=%p)", haChan, channel));
        nesting = emaPrepareForCallback((EMAElement)channel);
        app->cmMyChannelEvent.cmEvChannelFlowControlToZero(haChan, (HCHAN)channel);
        emaReturnFromCallback((EMAElement)channel, nesting);
        H245CBExit((app->pAppObject, "cmEvChannelFlowControlToZero"));
    }
}

/************************************************************************
 * h255ParamsOpenLogicalChannelAck
 * purpose: handle received OpenLogicalChannelAck message.
 * input  : pApp          - application handle for the cm element.
 *          hVal          - handle to the PVT.
 *          pChannel      - Channel element.
 *          pCtrl         - Control element.
 *          message       - node id for the OpenLogicalChannelAck message.
 * output : none
 * return : none
 ************************************************************************/
void h255ParamsOpenLogicalChannelAck(IN H245Object  *pApp,
                                     IN HPVT        hVal,
                                     IN H245Channel *pChannel,
                                     IN H245Control *pCtrl,
                                     IN RvPvtNodeId message)
{
    RvPvtNodeId forwardLCP_ID;
    RvPvtNodeId reverseLCP_ID;
    RvPvtNodeId h225ID;
    void        *ptr=NULL;
    H245Channel *pDependent;
    RvInt32     value;

    while ((pDependent = getNextOutChanByBase((HCONTROL)pCtrl, pChannel, &ptr)) != NULL)
        startEstablishment((HH245)pApp, (HCHAN)pDependent);
    if (!emaWasDeleted((EMAElement)pChannel))
    {
        forwardLCP_ID=pvtGetChild(hVal,message, __h245(forwardMultiplexAckParameters), NULL);
        h225ID=pvtGetChild(hVal,forwardLCP_ID, __h245(h2250LogicalChannelAckParameters), NULL);
        if (RV_PVT_NODEID_IS_VALID(h225ID))
        {
            reverseLCP_ID=pvtGetChild(hVal,message, __h245(reverseLogicalChannelParameters),NULL);
            pvtGetChildByFieldId(hVal,reverseLCP_ID,__h245(portNumber),&pChannel->remotePortNumber,NULL);

            value = 0;
            if (RV_PVT_NODEID_IS_VALID(pvtGetChildByFieldId(hVal, h225ID, __h245(sessionID), &value, NULL)))
                pChannel->data.h225Params.sessionId = (RvUint8)value;

            /* link partner channel */
            if (pChannel->data.h225Params.pPartner == NULL)
            {
                H245Channel *partnerChannel = getInChanBySID((HCONTROL)pCtrl, pChannel->data.h225Params.sessionId);
                if (partnerChannel != NULL)
                {
                    /* Link back */
                    pChannel->data.h225Params.pPartner = partnerChannel;
                    partnerChannel->data.h225Params.pPartner = pChannel;
                }
            }

            if (pChannel->bIsDuplex)
            {
                value = 0;
                pvtGetChildByFieldId(hVal,reverseLCP_ID, __h245(reverseLogicalChannelNumber),&value,NULL);
                pChannel->reverseLogicalChannelNum = (RvUint16)value;
            }

            /* -- get dynamic payload type */
            value = 0;
            pvtGetChildByFieldId(hVal, h225ID, __h245(dynamicRTPPayloadType), &value, NULL);
            pChannel->data.h225Params.dynamicPayloadNumber = (RvUint8)value;

            value = 0;
            pvtGetChildByFieldId(hVal, h225ID, __h245(flowControlToZero), &value, NULL);
            pChannel->data.h225Params.bFlowControlToZero = (RvBool)value;
        }

        if (pChannel->bIsDuplex)
        {
            /* Update separateStack information of this pChannel if necessary */
            RvPvtNodeId tmpId, seperateStackNode;
            seperateStackNode = pvtGetChild(hVal, message, __h245(separateStack), NULL);

            __pvtGetNodeIdByFieldIds(tmpId, hVal, seperateStackNode, {_h245(networkAddress) _h245(localAreaAddress) LAST_TOKEN});
            if (tmpId >= 0)
            {
                if (pChannel->data.h225Params.separateStackID < 0)
                    pChannel->data.h225Params.separateStackID = pvtAddRoot(hVal, NULL, 0, NULL);
                pvtMoveTree(hVal, pChannel->data.h225Params.separateStackID, tmpId);
            }
            else if (pChannel->data.h225Params.separateStackID >= 0)
            {
                pvtDelete(hVal, pChannel->data.h225Params.separateStackID);
                pChannel->data.h225Params.separateStackID = RV_PVT_INVALID_NODEID;
            }

            __pvtGetNodeIdByFieldIds(tmpId, hVal, seperateStackNode, {_h245(t120SetupProcedure) _anyField LAST_TOKEN});
            if (tmpId >= 0)
            {
                RvPstFieldId fieldId = 0;
                pvtGet(hVal, tmpId, &fieldId, NULL, NULL, NULL);
                if (fieldId == __h245(originateCall))
                    pChannel->data.h225Params.t120SetupProcedure = cmOriginateCall;
                else if (fieldId == __h245(waitForCall))
                    pChannel->data.h225Params.t120SetupProcedure = cmWaitForCall;
                else if (fieldId == __h245(issueQuery))
                    pChannel->data.h225Params.t120SetupProcedure = cmIssueQuery;
                else
                    pChannel->data.h225Params.t120SetupProcedure = (cmT120SetupProcedure)RV_ERROR_UNKNOWN;
            }
            else
                pChannel->data.h225Params.t120SetupProcedure = (cmT120SetupProcedure)RV_ERROR_UNKNOWN;

            if(pvtGetChild(hVal, seperateStackNode, __h245(associateConference), &tmpId) >= 0)
                pvtGet(hVal, tmpId, NULL, NULL, (RvInt32*)&pChannel->data.h225Params.isAssociated, NULL);
            else pChannel->data.h225Params.isAssociated = RV_FALSE;

            if(pvtGetChild(hVal, seperateStackNode, __h245(externalReference), &tmpId) >= 0)
            {
                pChannel->data.h225Params.externalReferenceLength =
                    pvtGetString(hVal, tmpId, 256, pChannel->data.h225Params.externalReference);
            }
            else
            {
                pChannel->data.h225Params.externalReferenceLength = 0;
                pChannel->data.h225Params.externalReference[0] = 0;
            }
        }

        establishConfirmed(pChannel, message);
    }

}

/************************************************************************
 * h255ParamsOpenLogicalChannelReject
 * purpose: handle received OpenLogicalChannelreject message.
 * input  : pApp          - application handle for the cm element.
 *          pChannel      - Channel element.
 *          pCtrl         - Control element.
 *          message       - node id for the OpenLogicalChannelAck message.
 *          rejectReason  - Reject reason received on an outgoing channel.
 * output : none
 * return : none
 ************************************************************************/
void h255ParamsOpenLogicalChannelReject(
    IN  H245Object              *pApp,
    IN  H245Channel             *pChannel,
    IN  H245Control             *pCtrl,
    IN  RvPvtNodeId             message,
    IN  cmRejectLcnReason       rejectReason,
    OUT cmChannelStateMode_e    *stateMode)
{
    RV_UNUSED_ARG(pApp);
    RV_UNUSED_ARG(pCtrl);
    RV_UNUSED_ARG(pChannel);
    RV_UNUSED_ARG(message);

    if (rejectReason == cmRejectReasonMasterSlaveConflict)
        *stateMode = cmChannelStateModeDisconnectedMasterSlaveConflict;
}

/************************************************************************
 * h255ParamsOpenLogicalChannelConfirm
 * purpose: handling OpenLogicalChannelConfirm message.
 * input  : hVal          - handle to the PVT.
 *          pChannel      - Channel element.
 *          message       - node id for the OpenLogicalChannelAck message.
 * output : none
 * return : none
 ************************************************************************/
void h255ParamsOpenLogicalChannelConfirm(IN HPVT        hVal,
                                         IN H245Channel *pChannel,
                                         IN RvPvtNodeId message)
{
    RV_UNUSED_ARG(hVal);
    establishConfirmed(pChannel, message);
}





/* ------------------------------ API functions ---------------------------- */

RVAPI int RVCALLCONV
cmChannelSameSession(
             /* get the same session opposite channel of hsChan */
             IN     HCHAN           hsChan,
             OUT    HAPPCHAN*       haSameSession,
             OUT    HCHAN*          hsSameSession
             )
{
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (!app || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSameSession: hsChan=%p, haSameSession=%p, hsSameSession=%p.", hsChan, haSameSession, hsSameSession));

    if (emaLock((EMAElement)hsChan))
    {
        if (channel->data.h225Params.pPartner)
        {
            if (hsSameSession)  *hsSameSession=(HCHAN)channel->data.h225Params.pPartner;
            if (haSameSession)
            {
                if (channel->data.h225Params.pPartner != NULL)
                    *haSameSession = (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel->data.h225Params.pPartner);
                else
                    *haSameSession = NULL;
            }
        }
        else
        {
            if (haSameSession) *haSameSession=NULL;
            if (hsSameSession) *hsSameSession=NULL;
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSameSession: ha=%p hs=%p [0].", (haSameSession)?*haSameSession:0, (hsSameSession)?*hsSameSession:0));
    return 0;
}


RVAPI int RVCALLCONV /* returns the session id for this channel */
cmChannelSessionId(
           /* get the session id of channel */
           IN   HCHAN           hsChan
           )
{
    H245Channel*channel=(H245Channel*)hsChan;
    int sid=RV_ERROR_UNKNOWN;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (!app || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSessionId: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        sid = channel->data.h225Params.sessionId;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSessionId=%d", sid));
    return sid;
}


RVAPI int RVCALLCONV
cmChannelSetAddress(
            IN  HCHAN       hsChan,
            IN  RvUint32    ip,
            IN  RvUint16    port)
{
    cmTransportAddress ta;
    memset(&ta, 0, sizeof(&ta));
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    ta.addr.v4.ip = ip;
#else
    ta.ip = ip;
#endif
    ta.port = port;
    ta.type = cmTransportTypeIP;
    ta.distribution = cmDistributionUnicast;
    return cmChannelSetRtpTransportAddress(hsChan, &ta);
}


RVAPI int RVCALLCONV
cmChannelSetRTCPAddress(
            IN  HCHAN       hsChan,
            IN  RvUint32    ip,
            IN  RvUint16    port)
{
    cmTransportAddress ta;
    memset(&ta, 0, sizeof(ta));
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    ta.addr.v4.ip = ip;
#else
    ta.ip = ip;
#endif
    ta.port = port;
    ta.type = cmTransportTypeIP;
    ta.distribution = cmDistributionUnicast;
    return cmChannelSetRtcpTransportAddress(hsChan, &ta);
}


RVAPI int RVCALLCONV
cmChannelSetT120Setup(
         IN      HCHAN               hsChan,
         IN      cmT120SetupProcedure t120SetupProc)
{
    H245Channel *channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (!app || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSetT120Setup: hsChan=%p, cmT120SetupProcedure=%d", hsChan, t120SetupProc));

    if (emaLock((EMAElement)hsChan))
    {
        if (((int)t120SetupProc >= (int)cmOriginateCall) && ((int)t120SetupProc <= (int)cmIssueQuery))
        {
            channel->data.h225Params.t120SetupProcedure = t120SetupProc;
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelSetT120Setup=1"));
            return RV_TRUE;
        }
        else
        {
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelSetT120Setup: invlid parameter"));
            return RV_ERROR_UNKNOWN;
        }
    }

    H245APIExit((app->pAppObject, "cmChannelSetT120Setup: channel does not exist"));
    return RV_ERROR_UNKNOWN;
}


RVAPI int RVCALLCONV
cmChannelGetT120Setup(
             IN     HCHAN                   hsChan,
             OUT    cmT120SetupProcedure*   t120SetupProc)
{
    H245Channel *channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);
    cmT120SetupProcedure t120sp=(cmT120SetupProcedure)RV_ERROR_UNKNOWN;

    if (!app || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelGetT120Setup: hsChan=%p", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        t120sp = channel->data.h225Params.t120SetupProcedure;
        emaUnlock((EMAElement)hsChan);
    }
    if (t120SetupProc)
        *t120SetupProc = t120sp;

    H245APIExit((app->pAppObject, "cmChannelGetT120Setup=%d", t120sp));
    return (int)t120sp;
}


RVAPI int RVCALLCONV
cmChannelSetFlowControlToZero(
         IN      HCHAN               hsChan,
         IN      RvBool              flowControl)
{
    H245Channel *channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (!app || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSetFlowControlToZero: hsChan=%p, flowControl=%d", hsChan, flowControl));

    if (emaLock((EMAElement)hsChan))
    {
        channel->data.h225Params.bFlowControlToZero=flowControl;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetFlowControlToZero=1"));
    return RV_TRUE;
}


RVAPI int RVCALLCONV
cmChannelSetTransportCapability(
             IN HCHAN   hsChan,
             IN int     transportCapId)
{
    H245Channel *channel=(H245Channel*)hsChan;
    HPVT        hVal;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (!app || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSetTransportCapability: hsChan=%p transpCapId=[%d] ", hsChan,transportCapId));

    if (emaLock((EMAElement)hsChan))
    {
        hVal = app->hVal;

        if (channel->data.h225Params.transCapID<0)
            channel->data.h225Params.transCapID = pvtAddRoot(hVal, app->h245TransCap, 0, NULL);
        pvtMoveTree(hVal,channel->data.h225Params.transCapID,transportCapId);
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetTransportCapability=1"));
    return RV_TRUE;

}


RVAPI int RVCALLCONV
cmChannelGetTransportCapabilityId(
             IN      HCHAN               hsChan)
{

    H245Channel *channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);
    int         nodeId=RV_ERROR_UNKNOWN;

    if (!app || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelGetTransportCapabilityId: hsChan=%p ", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        nodeId = channel->data.h225Params.transCapID;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelGetTransportCapabilityId=%d", nodeId));
    return nodeId;
}


RVAPI int RVCALLCONV
cmChannelSetRedundancyEncoding(
         IN     HCHAN               hsChan,
         IN     cmRedundancyEncoding * redundancyEncoding)

{
    H245Channel *channel=(H245Channel*)hsChan;
    HPVT        hVal;
    int         encMethodId,secEncodingId;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (!app || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSetRedundancyEncoding: hsChan=%p redEncMethodId=[%d] dataTypeHandle = [%d]",
            hsChan,redundancyEncoding->redundancyEncodingMethodId,redundancyEncoding->secondaryEncodingId));

    if (emaLock((EMAElement)hsChan))
    {
        hVal = app->hVal;

        if (channel->data.h225Params.redEncID<0)
            channel->data.h225Params.redEncID = pvtAddRoot(hVal, app->h245TransCap, 0, NULL);
        encMethodId = pvtAdd(hVal, channel->data.h225Params.redEncID, __h245(redundancyEncodingMethod),0, NULL, NULL);
        pvtMoveTree(hVal,encMethodId,redundancyEncoding->redundancyEncodingMethodId);
        if (redundancyEncoding->secondaryEncodingId >=0)
        {
          secEncodingId = pvtAdd(hVal, channel->data.h225Params.redEncID, __h245(secondaryEncoding),0, NULL, NULL);
          pvtMoveTree(hVal,secEncodingId,redundancyEncoding->secondaryEncodingId );
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetRedundancyEncoding=1"));
    return RV_TRUE;

}


RVAPI int RVCALLCONV
cmChannelGetRedundancyEncoding(
         IN      HCHAN                  hsChan,
         OUT     cmRedundancyEncoding * redundancyEncoding)
{
    H245Channel *channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);
    HPVT        hVal;
    int         nodeId=-1;

    if (!app || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelGetRedundancyEncoding: hsChan=%p ", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        hVal = app->hVal;

        if (channel->data.h225Params.redEncID >=0)
        {
            redundancyEncoding->redundancyEncodingMethodId = pvtGetChild(hVal,channel->data.h225Params.redEncID,__h245(redundancyEncodingMethod), NULL);
            redundancyEncoding->secondaryEncodingId = pvtGetChild(hVal,channel->data.h225Params.redEncID,__h245(secondaryEncoding),NULL);
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelGetRedundancyEncoding: redEncId [%d] redEncMethodId [%d] secondaryEncId [%d]",
        channel->data.h225Params.redEncID, redundancyEncoding->redundancyEncodingMethodId,redundancyEncoding->secondaryEncodingId));
    return nodeId;

}


RVAPI int RVCALLCONV
cmChannelSetSource(
         IN     HCHAN                   hsChan,
         IN     cmTerminalLabel         *terminalLabel)

{
    H245Channel *channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (!app || !channel || !terminalLabel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSetSource: hsChan=%p mcuNumber=[%d] terminalNumber = [%d]",
            hsChan,terminalLabel->mcuNumber,terminalLabel->terminalNumber));

    if (emaLock((EMAElement)hsChan))
    {
        channel->data.h225Params.source=*terminalLabel;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetSource=1"));
    return RV_TRUE;

}


RVAPI int RVCALLCONV
cmChannelGetSource(
         IN      HCHAN               hsChan,
         OUT     cmTerminalLabel     *terminalLabel)
{
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (!app || !channel || !terminalLabel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelGetSource: hsChan=%p ", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        *terminalLabel=channel->data.h225Params.source;
        emaUnlock((EMAElement)hsChan);
    }
    H245APIExit((app->pAppObject, "cmChannelGetSource: mcuNumber [%d] terminalNumber [%d]",
        terminalLabel->mcuNumber,terminalLabel->terminalNumber));
    return RV_TRUE;

}


RVAPI int RVCALLCONV
cmChannelSetDestination(
             IN      HCHAN               hsChan,
         IN     cmTerminalLabel *terminalLabel)

{
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (!app || !channel || !terminalLabel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSetDestination: hsChan=%p mcuNumber=[%d] terminalNumber = [%d]",
        hsChan,terminalLabel->mcuNumber,terminalLabel->terminalNumber));

    if (emaLock((EMAElement)hsChan))
    {
        channel->data.h225Params.destination=*terminalLabel;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetDestination=1"));
    return RV_TRUE;

}


RVAPI int RVCALLCONV
cmChannelGetDestination(
         IN      HCHAN               hsChan,
         OUT     cmTerminalLabel     *terminalLabel)
{
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (!app || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelGetDestination: hsChan=%p ", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        if (terminalLabel)
            *terminalLabel=channel->data.h225Params.destination;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelGetDestination: mcuNumber [%d] terminalNumber [%d]",
        terminalLabel->mcuNumber,terminalLabel->terminalNumber));
    return RV_TRUE;
}

/*
cmChannelSetNSAPAddress pass atm address for channel to state machine
Parameters:
hsChan          handle of channel
address         octet string atm nsap address
length          length of nsap address in bytes
multicast       RV_TRUE if it is multicast address.
Return:         in case of error return negative value

*/

RVAPI
int RVCALLCONV cmChannelSetNSAPAddress(
        IN      HCHAN               hsChan,
        IN      const RvChar*       address,
        IN      int                 length,
        IN      RvBool              multicast)
{
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_IPV4_ONLY)
#if (RV_LOGMASK & RV_LOGLEVEL_ERROR)
    H245Object* app = (H245Object *)emaGetUserData((EMAElement)hsChan);
#endif

    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(address);
    RV_UNUSED_ARG(length);
    RV_UNUSED_ARG(multicast);

    RvLogError(&app->log, (&app->log,
        "cmChannelSetNSAPAddress() not supported: RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_IPV4_ONLY"));
    return RV_ERROR_UNKNOWN;

#else
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    HPVT hVal;
    cmTransportAddress ta;
    H245Object* app=(H245Object*)hApp;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSetNSAPAddress: hsChan=%p, length=%d.", hsChan, length));

    if (emaLock((EMAElement)hsChan))
    {
        hVal = app->hVal;
        if (channel->data.h225Params.recvRtpAddressID<0)
            channel->data.h225Params.recvRtpAddressID = pvtAddRoot(hVal, app->hAddrSynH245, 0, NULL);

        ta.type=cmTransportTypeNSAP;
        ta.distribution=(multicast)?cmDistributionMulticast:cmDistributionUnicast;
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
        memcpy(ta.addr.v4.route, address, (RvSize_t)RvMin(length,(int)sizeof(ta.addr.v4.route)));
        ta.addr.v4.length=(RvUint16)length;
#else
        memcpy(ta.route, address, (RvSize_t)RvMin(length,(int)sizeof(ta.route)));
        ta.length=(RvUint16)length;
#endif
        cmTAToVt_H245(hVal,channel->data.h225Params.recvRtpAddressID,&ta);
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetNSAPAddress=1"));
    return RV_TRUE;
#endif
}

/*
cmChannelSetATMVC pass port number  to state machine
Parameters:
hsChan          handle of channel
portNumber      portNumber acording to asn 2 bytes length
Return:         in case of error return negative value

*/


RVAPI
int RVCALLCONV cmChannelSetATMVC(
        IN      HCHAN   hsChan,
        IN      int     portNumber)
{
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (!app || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSetATMVC: hsChan=%p, port=%d.", hsChan, portNumber));

    if (emaLock((EMAElement)hsChan))
    {
        channel->data.h225Params.portNumber=portNumber;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetATMVC=1"));
    return RV_TRUE;
}


RVAPI
int RVCALLCONV cmChannelSetDuplexAddress(
                       /* Set address of duplex connection */
         IN     HCHAN               hsChan,
         IN     cmTransportAddress  address,
         IN     int                 externalReferenceLength,
         IN     const RvChar*       externalReference, /* NULL if not exists */
         IN     RvBool              isAssociated /* RV_TRUE if associated */
         )
{
    HH245       hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    HPVT        hVal;
    H245Object* app=(H245Object*)hApp;
    RvChar      addr[55];

    if (!hApp || !hsChan)
        return RV_ERROR_UNKNOWN;

    if (cmTransportAddress2String(&address, sizeof(addr), addr) != RV_OK)
        addr[0] = '\0';
    H245APIEnter((app->pAppObject, "cmChannelSetDuplexAddress: hsChan=%p, addr=%s, %s, ext=%d '%s' %s.",
          hsChan, addr,
          (address.distribution==cmDistributionUnicast)?"unicast":"multicast",
          externalReferenceLength, (externalReference==NULL)?"(null)":externalReference,
          (isAssociated==RV_TRUE)?"Associated":"Not Associated"));

    if (emaLock((EMAElement)hsChan))
    {
        H245Channel* channel=(H245Channel*)hsChan;
        hVal = app->hVal;

        /* check if we were given an address */
        if (cmTaHasIp(&address) && (address.port == 0))
        {
            /* address was zero - delete the information */
            if (RV_PVT_NODEID_IS_VALID(channel->data.h225Params.separateStackID))
                pvtDelete(hVal, channel->data.h225Params.separateStackID);
            channel->data.h225Params.separateStackID = RV_PVT_INVALID_NODEID;
            channel->data.h225Params.externalReferenceLength = 0;
            channel->data.h225Params.externalReference[0] = '\0';
            channel->data.h225Params.isAssociated = RV_FALSE;
        }
        else
        {
            /* address was given, update information */
            if (!RV_PVT_NODEID_IS_VALID(channel->data.h225Params.separateStackID))
                channel->data.h225Params.separateStackID=pvtAddRoot(hVal,app->hAddrSynH245,0,NULL);
            getGoodAddressForCtrl(channel->hCtrl,&address);
            cmTAToVt_H245(hVal,channel->data.h225Params.separateStackID,&address);
            channel->data.h225Params.externalReferenceLength=RvMin(externalReferenceLength,255);
            memcpy(channel->data.h225Params.externalReference,externalReference, (RvSize_t)channel->data.h225Params.externalReferenceLength);
            channel->data.h225Params.isAssociated=isAssociated;
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetDuplexAddress=1"));
    return RV_TRUE;
}


RVAPI
int /* actual size of external reference or RV_ERROR_UNKNOWN */
RVCALLCONV cmChannelGetDuplexAddress(
           /* Get address of duplex connection */
           IN     HCHAN               hsChan,
           OUT    cmTransportAddress* address, /* User allocated structure */
           IN     int                 externalReferenceLength, /* size of extRef buffer */
           OUT    char*               externalReference, /* User allocated buffer */
           OUT    RvBool*             isAssociated /* RV_TRUE if associated */
                   )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)hApp;
    HPVT hVal;
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER | RV_LOGLEVEL_LEAVE))
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif
    int extRefLen = 0;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelGetDuplexAddress: hsChan=%p, address=%p, ext=%d:%p, isAssociated=%p",
          hsChan, address, externalReferenceLength, externalReference, isAssociated));

    if (emaLock((EMAElement)hsChan))
    {
        if (channel->data.h225Params.separateStackID <0)
        {
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelGetDuplexAddress: Error: No separate stack id"));
            return RV_ERROR_UNKNOWN;
        }

        hVal = app->hVal;

        if (address)
            cmVtToTA_H245(hVal, channel->data.h225Params.separateStackID, address);

        if( (externalReference) && (externalReferenceLength > channel->data.h225Params.externalReferenceLength) )
            memcpy(externalReference, channel->data.h225Params.externalReference, (RvSize_t)channel->data.h225Params.externalReferenceLength+1);
        extRefLen = channel->data.h225Params.externalReferenceLength;

        if (isAssociated)
            *isAssociated= channel->data.h225Params.isAssociated;

        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelGetDuplexAddress: ip=%s, port=%u, %s, ext=%d '%s' %s.",
         getIP(address, buff), (address)?address->port:0,
         (address)?((address->distribution==cmDistributionUnicast)?"unicast":"multicast"):("N/A"),
         0, nprn(externalReference),
         (isAssociated)?((*isAssociated==RV_TRUE)?"Associated":"Not Associated"):"N/A"));
    return extRefLen;
}

RVAPI int RVCALLCONV
cmChannelSetDynamicRTPPayloadType(
                  IN    HCHAN       hsChan,
                  IN    RvInt8      dynamicPayloadType)
{
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (!app || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSetDynamicRTPPayloadType: hsChan=%p, dynamicPayloadType=%d.",
          hsChan, dynamicPayloadType));

    if (emaLock((EMAElement)hsChan))
    {
        if (dynamicPayloadType <96)
        {
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelSetDynamicRTPPayloadType: [Invalid Parameter]"));
            return RV_ERROR_UNKNOWN;
        }
        channel->data.h225Params.dynamicPayloadNumber = (RvUint8)dynamicPayloadType;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetDynamicRTPPayloadType=1"));
    return RV_TRUE;
}


RVAPI int RVCALLCONV
cmChannelMaxSkew(
         IN     HCHAN           hsChan1,
         IN     HCHAN           hsChan2,
         IN     RvUint32        skew)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan1);
    H245Channel* channel1=(H245Channel*)hsChan1;
    H245Channel* channel2=(H245Channel*)hsChan2;
    int message, nodeId, res = RV_ERROR_UNKNOWN;
    H245Object*app=(H245Object*)hApp;

    if (!hApp)   return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelMaxSkew: hsChan1=%p, hsChan2=%p, skew=%d.", hsChan1, hsChan2, skew));

    if (emaLock((EMAElement)hsChan1))
    {
        if (emaLock((EMAElement)hsChan2))
        {
            HPVT hVal = app->hVal;

            if (!channel1->bOrigin || !channel2->bOrigin)
            {
                emaUnlock((EMAElement)hsChan2);
                emaUnlock((EMAElement)hsChan1);
                H245APIExit((app->pAppObject, "cmChannelMaxSkew: [Invalid Circumstances]"));
                return RV_ERROR_UNKNOWN;
            }

            message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
            nodeId=pvtAddBranch2(hVal,message, __h245(indication),__h245(h2250MaximumSkewIndication));
            pvtAdd(hVal,nodeId,__h245(logicalChannelNumber1),channel1->myLogicalChannelNum,NULL,NULL);
            pvtAdd(hVal,nodeId,__h245(logicalChannelNumber2),channel2->myLogicalChannelNum,NULL,NULL);
            pvtAdd(hVal,nodeId,__h245(maximumSkew),(RvInt32)skew,NULL,NULL);

            res = sendMessageH245(channel1->hCtrl, message, RV_TRUE, NULL);
            emaUnlock((EMAElement)hsChan2);
        }
        emaUnlock((EMAElement)hsChan1);
    }

    H245APIExit((app->pAppObject, "cmChannelMaxSkew=%d", res));
    return res;
}


/******************************************************************************
 * cmCallGetChannelBySessionId
 * ----------------------------------------------------------------------------
 * General: Find a channel of a call from its sessionId and direction.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall       - The call handle.
 *         sessionId    - Session ID to look for.
 *         origin       - RV_TRUE if we are the origin of the searched channel.
 *                        RV_FALSE if we are not the origin of the searched
 *                        channel.
 * Output: hsChan       - Channel that was found.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmCallGetChannelBySessionId(
       IN   HCALL       hsCall,
       IN   int         sessionId,
       IN   RvBool      origin,
       OUT  HCHAN       *hsChan)
{
    RvStatus status = RV_ERROR_DESTRUCTED;
    H245Object *app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

    if (app == NULL)
        return RV_ERROR_NULLPTR;

    H245APIEnter((app->pAppObject, "cmCallGetChannelBySessionId(hsCall=%p, sessionId=%d, origin=%d)",
        hsCall, sessionId, origin));

    if (((sessionId & 0xff) == sessionId) && emaLock((EMAElement)hsCall))
    {
        if (origin)
            *hsChan = (HCHAN)getOutChanBySID(cmiGetControl(hsCall), (RvUint8)sessionId);
        else
            *hsChan = (HCHAN)getInChanBySID(cmiGetControl(hsCall), (RvUint8)sessionId);

        if (*hsChan != NULL)
            status = RV_OK;
        else
            status = RV_ERROR_UNKNOWN;

        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallGetChannelBySessionId(hsChan=%p)=%d", *hsChan, status));
    return status;
}


/************************************************************************
 * h225ParamsChannelOpenDynamic
 * purpose: Sets the data type.
 * input  : pChannel    - Channel element to set h223 parameters for.
 *          hVal        - PVT handle.
 *          dataTypeId  - Data type node id.
 * output : none
 * return : none
 ************************************************************************/
void h225ParamsChannelOpenDynamic(H245Channel *pChannel, HPVT hVal, RvPvtNodeId dataTypeId)
{
    RV_UNUSED_ARG(pChannel);
    RV_UNUSED_ARG(hVal);
    RV_UNUSED_ARG(dataTypeId);
    return;
}



/************************************************************************
 * h225ParamsCallChannelParametersCallback
 * purpose: Gets same session and associated channel handles.
 * input  : hsChan        - The channel handle.
 * output : hsAssociatedH - The associated channel handle.
 *          haAssociatedH - The associated application channel handle.
 *          hsAssociatedH - The same channel handle.
 *          hsAssociatedH - The same application channel handle.
 * return : none
 ************************************************************************/
void h225ParamsCallChannelParametersCallback(HCHAN    hsChan,
                                             HCHAN    *hsAssociatedH,
                                             HAPPCHAN *haAssociatedH,
                                             HCHAN    *hsSameSessionH,
                                             HAPPCHAN *haSameSessionH)
{
    H245Channel* channel = (H245Channel *)hsChan;

    /* Get same session and associated channel handles */
    *hsAssociatedH = (HCHAN)channel->data.h225Params.pAssociated;
    if (*hsAssociatedH != NULL)
        *haAssociatedH = (HAPPCHAN)emaGetApplicationHandle((EMAElement)*hsAssociatedH);
    *hsSameSessionH = (HCHAN)channel->data.h225Params.pPartner;
    if (*hsSameSessionH != NULL)
        *haSameSessionH = (HAPPCHAN)emaGetApplicationHandle((EMAElement)*hsSameSessionH);
}


/************************************************************************
 * h225ParamsChannelClose
 * purpose: Make sure the default used session Id's are updated if we close them.
 * input  : ctrl        - The control object.
 *          channel     - The channel object.
 * output : none.
 * return : none
 ************************************************************************/
void h225ParamsChannelClose(H245Control *ctrl, H245Channel *channel)
{
    if (ctrl != NULL)
    {
        if (channel->bIsDuplex)
        {
            /* For duplex channels, we must update both sides... */
            switch (channel->data.h225Params.sessionId)
            {
            case 1: ctrl->bFirstAudioIn = RV_TRUE; ctrl->bFirstAudioOut = RV_TRUE; break;
            case 2: ctrl->bFirstVideoIn = RV_TRUE; ctrl->bFirstVideoOut = RV_TRUE; break;
            case 3: ctrl->bFirstDataIn = RV_TRUE; ctrl->bFirstDataOut = RV_TRUE; break;
            default: break; /* Do nothing */
            }
        }
        else if (channel->bOrigin)
        {
            /* Make sure the default used session Id's are updated if we close them */
            switch (channel->data.h225Params.sessionId)
            {
            case 1: ctrl->bFirstAudioOut = RV_TRUE; break;
            case 2: ctrl->bFirstVideoOut = RV_TRUE; break;
            case 3: ctrl->bFirstDataOut = RV_TRUE; break;
            default: break; /* Do nothing */
            }
        }
        else
        {
            /* Make sure the default used session Id's are updated if we close them */
            switch (channel->data.h225Params.sessionId)
            {
            case 1: ctrl->bFirstAudioIn = RV_TRUE; break;
            case 2: ctrl->bFirstVideoIn = RV_TRUE; break;
            case 3: ctrl->bFirstDataIn = RV_TRUE; break;
            default: break; /* Do nothing */
            }
        }
    }
}

/******************************************************************************
 * h223ParamsChannelReject
 * ----------------------------------------------------------------------------
 * General: Handles outgoing openLogicalChannelReject.
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ctrl                 - H245Control object of the call.
 *         channel              - rejected Channel.
 * Output: None.
 *****************************************************************************/
void h225ParamsChannelReject(H245Control *ctrl, H245Channel *channel)
{
    RV_UNUSED_ARG(ctrl);
    RV_UNUSED_ARG(channel);
    return;
}

/****************************************************************************************************************/
/* This function shall be called before the first channel is opened using the H.245 procedures.                 */
/* It is protected from the second call so it may be called each time the new channel is about to be established*/
/* The main purpose of the function is to mark well-known session IDs used by the fast start and                */
/* prevent in this way usage of them in H.245                                                                   */
/****************************************************************************************************************/
void h225ParamsDeriveChannels(HCONTROL ctrl)
{
    H245Control *ctrlE=(H245Control*)ctrl;
    H245Channel *channel;
    void* ptr=NULL;
    if (!ctrlE->bIsDerived)
    {
        ctrlE->bFirstAudioIn=RV_TRUE;
        ctrlE->bFirstVideoIn=RV_TRUE;
        ctrlE->bFirstAudioOut=RV_TRUE;
        ctrlE->bFirstVideoOut=RV_TRUE;
        ctrlE->bFirstDataIn=RV_TRUE;
        ctrlE->bFirstDataOut=RV_TRUE;
        channel = getNextChan(ctrl, &ptr);
        while (channel != NULL)
        {
            switch (channel->data.h225Params.sessionId)
            {
            case 1:
                if (channel->bOrigin || channel->bIsDuplex)
                    ctrlE->bFirstAudioOut=RV_FALSE;
                if (!channel->bOrigin || channel->bIsDuplex)
                    ctrlE->bFirstAudioIn=RV_FALSE;
                break;
            case 2:
                if (channel->bOrigin || channel->bIsDuplex)
                    ctrlE->bFirstVideoOut=RV_FALSE;
                if (!channel->bOrigin || channel->bIsDuplex)
                    ctrlE->bFirstVideoIn=RV_FALSE;
                break;
            case 3:
                if (channel->bOrigin || channel->bIsDuplex)
                    ctrlE->bFirstDataOut=RV_FALSE;
                if (!channel->bOrigin || channel->bIsDuplex)
                    ctrlE->bFirstDataIn=RV_FALSE;
                break;
            default:
                break;
            }
            channel = getNextChan(ctrl, &ptr);
        }
        ctrlE->bIsDerived = RV_TRUE;
    }
}

/************************************************************************
 * h225ParamsSessionOpenDynamic
 * purpose: Handle the same session and associated channels.
 * input  : channel             - The channel object.
 *          ctrlE               - The control object.
 *          hsChanSameSession   - The same session channel handle.
 *          hsChanAssociated    - The associated channel handle.
 * output : none.
 * return : none
 ************************************************************************/
void h225ParamsSessionOpenDynamic(H245Channel *channel,
                                  H245Control *ctrlE,
                                  HCHAN       hsChanSameSession,
                                  HCHAN       hsChanAssociated,
                                  RvBool      isDynamicPayloadType)
{
    HH245        hApp=(HH245)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrlE))));
    H245Object * app = (H245Object *)hApp;
    HPVT         hVal = app->hVal;

    RvPstFieldId fieldId;
    RvPvtNodeId nodeId = pvtGet(hVal, pvtChild(hVal, channel->dataTypeID), &fieldId, NULL, NULL, NULL);
    if (fieldId == __h245(h235Media))
        pvtGet(hVal,pvtChild(hVal,pvtGetChild(hVal,nodeId,__h245(mediaType),NULL)),&fieldId,NULL,NULL,NULL);

    h225ParamsDeriveChannels((HCONTROL)ctrlE);
    /*h22xFunctions[ctrlE->eParamsType].DeriveChannels((HCONTROL)ctrlE);*/

    if (hsChanAssociated != NULL)
        channel->data.h225Params.pAssociated=(H245Channel*)hsChanAssociated;/* -- associate session */

    if (hsChanSameSession != NULL)
    {
        H245Channel *partnerChannel = (H245Channel *)hsChanSameSession;
        channel->data.h225Params.pPartner = partnerChannel;  /* -- join session */
        channel->data.h225Params.sessionId =
            partnerChannel->data.h225Params.sessionId;
        partnerChannel->data.h225Params.pPartner = channel;
    }

    if (isDynamicPayloadType)
    {
        RvLockGet(&app->lock, app->logMgr);
        app->dynamicPayloadNumber = (RvUint8)((app->dynamicPayloadNumber+1)%32);
        channel->data.h225Params.dynamicPayloadNumber = (RvUint8)(app->dynamicPayloadNumber+96);
        RvLockRelease(&app->lock, app->logMgr);
    }

    if (!channel->data.h225Params.pPartner)
    { /* new id */
        channel->data.h225Params.sessionId=0;
    }

    if (!channel->bIsDuplex)
    {
        if (channel->data.h225Params.pPartner == NULL)
        { /* new id */
            RvBool gotSid = RV_FALSE;

            if (channel->data.h225Params.bForceSecondary)
            {
                if (ctrlE->bIsMaster)
                {
                    if ((channel->data.h225Params.sessionId = getFreeSID((HCONTROL)ctrlE, channel->bOrigin, RV_FALSE)) == 0)
                    {
                        channel->eState = ChannelStateReleased;
                        notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
                        notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
                        H245APIExit((app->pAppObject, "cmChannelOpenDynamic: Out of session IDs [Resource Problem]"));
                        return;
                    }
                }
            }

            if (!channel->data.h225Params.bForceSecondary)
            {
                if (ctrlE->bFirstAudioOut && (fieldId == __h245(audioData)))
                {
                    ctrlE->bFirstAudioOut=RV_FALSE; channel->data.h225Params.sessionId=1;
                    channel->data.h225Params.pPartner=getInChanBySID((HCONTROL)ctrlE,channel->data.h225Params.sessionId);
                    gotSid = RV_TRUE;
                }
                else if (ctrlE->bFirstVideoOut && (fieldId == __h245(videoData)))
                {
                    ctrlE->bFirstVideoOut=RV_FALSE; channel->data.h225Params.sessionId=2;
                    channel->data.h225Params.pPartner=getInChanBySID((HCONTROL)ctrlE,channel->data.h225Params.sessionId);
                    gotSid = RV_TRUE;
                }
                else if (ctrlE->bFirstDataOut && (fieldId == __h245(data)))
                {
                    ctrlE->bFirstDataOut=RV_FALSE;
                    channel->data.h225Params.sessionId=3;
                    channel->data.h225Params.pPartner=getInChanBySID((HCONTROL)ctrlE,channel->data.h225Params.sessionId);
                    gotSid = RV_TRUE;
                }
            }
            if (!gotSid && ctrlE->bIsMaster)
            {
                if ((channel->data.h225Params.sessionId = getFreeSID((HCONTROL)ctrlE, channel->bOrigin, RV_FALSE)) == 0)
                {
                    channel->eState = ChannelStateReleased;
                    notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
                    notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
                    H245APIExit((app->pAppObject, "cmChannelOpenDynamic: Out of session IDs [Resource Problem]"));
                    return;
                }
            }
            if (channel->data.h225Params.pPartner != NULL)
            {
                /* Reverse link as well */
                channel->data.h225Params.pPartner->data.h225Params.pPartner = channel;
            }
        }
        else
            /* check database for partner lcn. In case of first audio or video */
        { /* use partner sid */
            switch (channel->data.h225Params.sessionId)
            {
            case 1: ctrlE->bFirstAudioOut=RV_FALSE; break;
            case 2: ctrlE->bFirstVideoOut=RV_FALSE; break;
            case 3: ctrlE->bFirstDataOut=RV_FALSE; break;
            default: break;
            }
        }
    }
    else
    {
        /* Bi-directional channel */
        if (channel->data.h225Params.pPartner == NULL)
        { /* new id */
            /* TODO: do we really need to check for partners in bi-channels? */
            if (channel->data.h225Params.bForceSecondary)
            {
                if (ctrlE->bIsMaster)
                {
                    if ((channel->data.h225Params.sessionId = getFreeSID((HCONTROL)ctrlE, channel->bOrigin, RV_TRUE)) == 0)
                    {
                        channel->eState = ChannelStateReleased;
                        notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
                        notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
                        H245APIExit((app->pAppObject, "cmChannelOpenDynamic: Out of session IDs [Resource Problem]"));
                        return;
                    }
                }
            }
            else if (ctrlE->bFirstAudioOut && ctrlE->bFirstAudioIn && (fieldId == __h245(audioData)))
            {
                ctrlE->bFirstAudioOut=RV_FALSE;
                ctrlE->bFirstAudioIn=RV_FALSE;
                channel->data.h225Params.sessionId=1;
            }
            else if (ctrlE->bFirstVideoOut && ctrlE->bFirstVideoIn && (fieldId == __h245(videoData)))
            {
                ctrlE->bFirstVideoOut=RV_FALSE;
                ctrlE->bFirstVideoIn=RV_FALSE;
                channel->data.h225Params.sessionId=2;
            }
            else if (ctrlE->bFirstDataOut && ctrlE->bFirstDataIn && (fieldId == __h245(data)))
            {
                ctrlE->bFirstDataOut=RV_FALSE;
                ctrlE->bFirstDataIn=RV_FALSE;
                channel->data.h225Params.sessionId=3;
            }
            else if (ctrlE->bIsMaster)
            {
                if ((channel->data.h225Params.sessionId = getFreeSID((HCONTROL)ctrlE, channel->bOrigin, RV_TRUE)) == 0)
                {
                    channel->eState = ChannelStateReleased;
                    notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
                    notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
                    H245APIExit((app->pAppObject, "cmChannelOpenDynamic: Out of session IDs [Resource Problem]"));
                    return;
                }
            }
        }
        else
        /* check database for partner lcn. In case of first audio or video */
        { /* use partner sid */
            switch (channel->data.h225Params.sessionId)
            {
            case 1: ctrlE->bFirstAudioOut=RV_FALSE; ctrlE->bFirstAudioIn=RV_FALSE; break;
            case 2: ctrlE->bFirstVideoOut=RV_FALSE; ctrlE->bFirstVideoIn=RV_FALSE; break;
            case 3: ctrlE->bFirstDataOut=RV_FALSE;  ctrlE->bFirstDataIn=RV_FALSE; break;
            default: break;
            }
        }
    }
}

/************************************************************************
 * h225MultiplexReconfiguration
 * purpose: Handle an incoming h223MultiplexReconfiguration message.
 * input  : ctrl    - Control object
 *          message - h223MultiplexReconfiguration message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int h225MultiplexReconfiguration(IN H245Control* ctrl, IN int message)
{
    RV_UNUSED_ARG(ctrl);
    RV_UNUSED_ARG(message);
    return RV_OK;
}

/************************************************************************
 * h225EndSessionCommand
 * purpose: Handle an incoming endSessionCommand message.
 *          This should stop the control of the call.
 * input  : ctrl    - Control object
 *          message - endSessionCommand message received
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int h225EndSessionCommand(IN H245Control* ctrl, IN int message)
{
    RV_UNUSED_ARG(message);

    /* See if we need to stop the control at all - it might have been closed already by an
       outgoing endSessionCommand message - check is done inside */
    stopControl((HCONTROL)ctrl);

    return RV_TRUE;
}


int cmChannelSetRtpTransportAddress(
                                    IN  HCHAN               hsChan,
                                    IN  cmTransportAddress* ta)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    HPVT  hVal;
    H245Object* app=(H245Object*)hApp;
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER | RV_LOGLEVEL_LEAVE))
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    if (!hApp)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSetRtpTransportAddress: hsChan=%p, ip=%s, port=%u.",
        hsChan, getIP(ta, buff), ta->port));

    if (emaLock((EMAElement)hsChan))
    {
        H245Channel* channel = (H245Channel*)hsChan;
        H245Control* ctrl = (H245Control*)channel->hCtrl;
        HCALL hsCall = cmiGetByControl((HCONTROL)ctrl);

        hVal = app->hVal;

        /* Create a new root for the RTP address if necessary */
        if (channel->data.h225Params.recvRtpAddressID < 0)
            channel->data.h225Params.recvRtpAddressID = pvtAddRoot(hVal, app->hAddrSynH245, 0, NULL);

        /* Make sure the address is good */
        getGoodAddressForCtrl(channel->hCtrl, ta);

        /* Update the PVT of the RTP address in the channel element */
        cmTAToVt_H245(hVal, channel->data.h225Params.recvRtpAddressID, ta);

        /* Check to see if we are in the middle of answering a FS proposal */
        if ((ctrl->fastStartState == fssAcptSend) && (app->evHandlers.cmCallInAnsweringState(hsCall) ||
                                (ctrl->eExtendedFastConnectState == cmExtendedFastConnectApproved)))
        {
            /* now set it in the OLC */
            int tmpNode=pvtGetChild(hVal, ctrl->fastStartNodesAcptSend[channel->data.h225Params.fastStartChannelIndex], __h245(reverseLogicalChannelParameters),NULL);

            if (tmpNode < 0)
            {
                tmpNode=pvtGetChild(hVal, ctrl->fastStartNodesAcptSend[channel->data.h225Params.fastStartChannelIndex], __h245(forwardLogicalChannelParameters),NULL);
            }

            __pvtBuildByFieldIds(tmpNode, hVal, tmpNode, {_h245(multiplexParameters) _h245(h2250LogicalChannelParameters) _h245(mediaChannel) LAST_TOKEN}, 0, NULL );
            if (cmTaHasIp(ta) || ta->port)
                cmTAToVt_H245(hVal, tmpNode, ta);
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetRtpTransportAddress=1"));
    return RV_TRUE;
}


int cmChannelSetRtcpTransportAddress(
                                     IN  HCHAN               hsChan,
                                     IN  cmTransportAddress* ta)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    HPVT  hVal;
    int ret = RV_ERROR_UNKNOWN;
    H245Object* app=(H245Object*)hApp;
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER | RV_LOGLEVEL_LEAVE))
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    if (!hApp)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSetRtcpTransportAddress: hsChan=%p, ip=%s, port=%u.",
        hsChan, getIP(ta, buff), ta->port));

    if (emaLock((EMAElement)hsChan))
    {
        H245Channel* channel = (H245Channel*)hsChan;
        H245Control* ctrl = (H245Control*)channel->hCtrl;
        HCALL hsCall = cmiGetByControl((HCONTROL)ctrl);

        hVal = app->hVal;
        /* Make sure the address is good */
        getGoodAddressForCtrl(channel->hCtrl, ta);

        if ((channel->data.h225Params.pPartner == NULL) ||
            (channel->data.h225Params.pPartner->data.h225Params.recvRtcpAddressID < 0))
        {
            /* Create a new root for the RTCP address if necessary */
            if (channel->data.h225Params.recvRtcpAddressID < 0)
                channel->data.h225Params.recvRtcpAddressID = pvtAddRoot(hVal, app->hAddrSynH245, 0, NULL);

            /* Update the PVT of the RTP address */
            ret = cmTAToVt_H245(hVal, channel->data.h225Params.recvRtcpAddressID, ta);
        }
        else
        {
            int tmpNode = pvtAddRoot(hVal, app->hAddrSynH245, 0, NULL);

            /* Update the PVT of the RTP address */
            ret = cmTAToVt_H245(hVal, tmpNode, ta);

            if(ret >= 0)
                ret = pvtCompareTree(hVal, channel->data.h225Params.pPartner->data.h225Params.recvRtcpAddressID, hVal, tmpNode);

            pvtDelete(hVal, tmpNode);
        }

        /* Check to see if we are in the middle of answering a FS proposal */
        if ((ctrl->fastStartState == fssAcptSend) && (app->evHandlers.cmCallInAnsweringState(hsCall) ||
                                (ctrl->eExtendedFastConnectState == cmExtendedFastConnectApproved)))
        {
            /* now set it in the OLC */
            int tmpNode=pvtGetChild(hVal, ctrl->fastStartNodesAcptSend[channel->data.h225Params.fastStartChannelIndex], __h245(reverseLogicalChannelParameters),NULL);
            if (tmpNode < 0)
            {
                tmpNode=pvtGetChild(hVal, ctrl->fastStartNodesAcptSend[channel->data.h225Params.fastStartChannelIndex], __h245(forwardLogicalChannelParameters),NULL);
            }

            __pvtBuildByFieldIds(tmpNode, hVal, tmpNode, {_h245(multiplexParameters) _h245(h2250LogicalChannelParameters) _h245(mediaControlChannel) LAST_TOKEN}, 0, NULL );
            if (cmTaHasIp(ta) || ta->port)
                cmTAToVt_H245(hVal, tmpNode, ta);
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetRtcpTransportAddress=%d", ret));
    return ret;
}



#else
  int h225Paramsdummy= 1;
#endif /*#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)*/


#ifdef __cplusplus
}
#endif
