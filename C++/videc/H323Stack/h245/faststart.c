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

#include "cmH245GeneralDefs.h"
#if (RV_H245_SUPPORT_H225_PARAMS == 1)

#include "pvaltree.h"
#include "pvaltreeStackApi.h"
#include "psyntreeStackApi.h"
#include "cmH245GeneralDefs.h"
#include "cmictrl.h"
#include "cmConf.h"
#include "q931asn1.h"
#include "h245.h"
#include "cmutils.h"
#include "copybits.h"
#include "prnutils.h"
#include "cmchan.h"
#include "cmChanGet.h"
#include "cmH245Object.h"
#include "cmiParams.h"
#include "stkutils.h"


#ifdef __cplusplus
extern "C" {
#endif

extern H22XFunctions h22xFunctions[];


/* These numbers are used in the lcn allocation procedure and is assumed to never be changed */
#define MAX_FASTSTART_CHANNELS  511
#define MAX_LCN_S               128

/* These numbers are used in the Extanded Fast Connect (EFC) feature*/
#define EFC_CHANNEL_PROPOSALS               1
#define EFC_CLOSE_ALL_MEDIA_CHANNELS        2
#define EFC_RERUEST_NEW_CHANNEL_PROPOSALS   3


/**************************************************************************************************
  What I tried to do is to find big enough range of LCNs which does not include any of
  forwardlogicalChannelNumber received in proposed channels or used in approved channels.

  So, I assumed that there is less than MAX_FASTSTART_CHANNELS (511) proposals and divided the
  1..65535 range into 512 subranges, with MAX_LCN_S (128) values in each subrange.

  setBit(lcnAllocationBuff,lcn/MAX_LCN_S,1); marks the particular subrange as occupied by some LCN

  There are 512 subranges and only 511 numbers so one subrange will be for sure empty.

  get1st0BitNumber(lcnAllocationBuff, 0, MAX_FASTSTART_CHANNELS)*MAX_LCN_S; gives me the value of
  the first number of this subrange. The entire subrange may be now used for fLCN allocation.

                        - Sasha
**************************************************************************************************/


/* Maximum number of session ids that fast start can handle */
#define MAX_FS_SESSION_ID 10
#define MAX_SESSION_ID    256


/* function definitions */

int notifyChannelState(H245Channel*channel,cmChannelState_e state, cmChannelStateMode_e stateMode);

int  cmcReadyEvent  (H245Control* ctrl);

void deleteOutgoingFastStartOffer(HCALL hsCall);
void deleteIncomingFastStartOffer(HCALL hsCall);


/******************************************************************************
 * buildLCParameters
 * ----------------------------------------------------------------------------
 * General: Builds the parameters for a FS channel node.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsCall      - Stack handle for the call
 *         elemNodeId  - FastStart channel node
 *         sessionId   - Session id of the proposed channel
 *         fsChannel   - Pointer to structure containing channel information.
 *                       May be null if we want a Null-OLC.
 * Output: None.
 *****************************************************************************/
static void buildLCParameters(HCALL hsCall, RvPvtNodeId elemNodeId, int sessionId, cmFastStartChannel* fsChannel)
{
    RvPvtNodeId paramNodeId, nodeId;
    H245Object*app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    HPVT hVal;

    hVal=app->hVal;

    /* first, see if this a Null-OLC */
    if (fsChannel == NULL)
    {
        paramNodeId = pvtAddBranch2(hVal, elemNodeId, __h245(multiplexParameters), __h245(h2250LogicalChannelParameters));
        pvtAdd(hVal, paramNodeId, __h245(sessionID), sessionId, NULL, NULL);

        /* Write nullData in the dataType part */
        pvtAddBranch2(hVal, elemNodeId, __h245(dataType), __h245(nullData));
        return;
    }

    /* Not a Null-OLC. Build and fill the dataType either from the configuration or from the given
       structure */
    nodeId=pvtAddBranch(hVal,elemNodeId,__h245(dataType));

    if (fsChannel->dataTypeHandle<0)
        confGetDataType(hVal, app->h245Conf,fsChannel->channelName, nodeId, NULL, RV_FALSE);
    else
        pvtSetTree(hVal,nodeId,hVal,fsChannel->dataTypeHandle);

    paramNodeId=pvtAddBranch2(hVal,elemNodeId,__h245(multiplexParameters),__h245(h2250LogicalChannelParameters));

    /* Build and set the mediaChannel subtree (rtp) */
    if (cmTaHasIp(&fsChannel->rtp) || fsChannel->rtp.port)
    {
        nodeId=pvtAddBranch(hVal,paramNodeId,__h245(mediaChannel));
        getGoodAddressForCall(hsCall,&fsChannel->rtp);
        cmTAToVt_H245(hVal, nodeId, &fsChannel->rtp);
    }

    /* Build and set the mediaControlChannel subtree (rtcp) */
    if (cmTaHasIp(&fsChannel->rtcp) || fsChannel->rtcp.port)
    {
        nodeId=pvtAddBranch(hVal,paramNodeId,__h245(mediaControlChannel));
        getGoodAddressForCall(hsCall, &fsChannel->rtcp);
        cmTAToVt_H245(hVal, nodeId, &fsChannel->rtcp);
    }

    /* fill in the session id */
    pvtAdd(hVal, paramNodeId, __h245(sessionID), sessionId, NULL, NULL);
}


/******************************************************************************
 * isNullOLC
 * ----------------------------------------------------------------------------
 * General: Determine if a channel node is a Null-OLC.
 *
 * Return Value: RV_TRUE if this is a Null-OLC, RV_FALSE o.w.
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hVal        - VT tree handle
 *         chanMsgId   - FastStart channel node
 *         bOrigin     - TRUE iff we received an ACK message or are sending an offer
 * output: sessionId   - Session Id for the channel node (even if it is not a Null-OLC)
 *         dir         - Direction of the channel, if not a Null-OLC (optional)
 *****************************************************************************/
static RvBool isNullOLC(
    IN  HPVT hVal,
    IN  RvPvtNodeId chanMsgId,
    IN  RvBool bOrigin,
    OUT RvInt32 *sessionId,
    OUT cmChannelDirection *dir)
{
    RvBool ret = RV_FALSE;
    RvPvtNodeId fwdNodeId = pvtGetChild(hVal, chanMsgId,__h245(forwardLogicalChannelParameters),NULL);
    RvPvtNodeId rvsNodeId = pvtGetChild(hVal, chanMsgId,__h245(reverseLogicalChannelParameters),NULL);

    /* assume this is a forward channel at first */
    if (dir != NULL)
    {
        if (bOrigin) /* fwd = out */
            *dir = dirTransmit;
        else         /* fwd = in  */
            *dir = dirReceive;
    }

    if (RV_PVT_NODEID_IS_VALID(pvtGetChild2(hVal, fwdNodeId, __h245(dataType),__h245(nullData))))
    {
        /* forward logical channel is null-data. check if we have a reverse */
        if (RV_PVT_NODEID_IS_VALID(rvsNodeId))
        {
            /* null data, but with a reverse - reverse channel */
            if (dir != NULL)
            {
                if (bOrigin) /* rev = in */
                    *dir = dirReceive;
                else         /* rev = out  */
                    *dir = dirTransmit;
            }
        }
        else
        {
            /* null data with no reverse channel */
            ret = RV_TRUE;
        }
    }
    else if (RV_PVT_NODEID_IS_VALID(rvsNodeId))
    {
        /* both reverse and fwd exist - duplex channel */
        if (dir != NULL)
            *dir = dirBoth;
    }

    /* get the session ID, if needed */
    if (sessionId != NULL)
    {
        RvPvtNodeId tmpNode = RV_PVT_NODEID_IS_VALID(rvsNodeId) ? rvsNodeId : fwdNodeId;
        tmpNode = pvtGetChild2(hVal, tmpNode, __h245(multiplexParameters), __h245(h2250LogicalChannelParameters));

        pvtGetChildByFieldId(hVal, tmpNode, __h245(sessionID), sessionId, NULL);
    }
    return ret;
}


#define getFwdChanBySID(_chan,_ctrl,_sid) {\
    if (bOrigin) /* fwd = out */ {_chan = getOutChanBySID(_ctrl,_sid);} \
    else         /* fwd = in  */ {_chan = getInChanBySID(_ctrl,_sid);}}

#define getRevChanBySID(_chan,_ctrl,_sid) {\
    if (bOrigin) /* rev = in  */ {_chan = getInChanBySID(_ctrl,_sid);} \
    else         /* rev = out */ {_chan = getOutChanBySID(_ctrl,_sid);}}


/******************************************************************************
 * isReferringToExistingChannel
 * ----------------------------------------------------------------------------
 * General: Determine if a FS OLC refers to an existing channel.
 *
 * Return Value: RV_TRUE if OLC refers to an existing channel, RV_FALSE o.w.
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hVal        - VT tree handle
 *         ctrl        - Call control object
 *         chanMsgId   - FastStart channel node
 *         bOrigin     - TRUE iff we received an ACK message or are sending an offer
 * output: sessionId   - Session Id for the channel node (even if it is not a Null-OLC)
 *         chan        - The channel referred to
 *         dataNode    - The new data-type node, negative on a Null OLC
 *****************************************************************************/
static RvBool isReferringToExistingChannel(
    IN  HPVT hVal,
    IN  H245Control *ctrl,
    IN  RvPvtNodeId chanMsgId,
    IN  RvBool bOrigin,
    OUT RvInt32 *sessionId,
    OUT H245Channel **chan,
    OUT RvPvtNodeId * dataNode)
{
    RvPvtNodeId fwdNodeId, revNodeId;

    /* try to see if we are referring to a forward channel */
    __pvtGetByFieldIds(fwdNodeId, hVal, chanMsgId,
        { _h245(forwardLogicalChannelParameters) _h245(multiplexParameters)
          _h245(h2250LogicalChannelParameters) _h245(sessionID) LAST_TOKEN }, NULL, sessionId, NULL);

    __pvtGetByFieldIds(revNodeId, hVal, chanMsgId,
        { _h245(reverseLogicalChannelParameters) _h245(multiplexParameters)
          _h245(h2250LogicalChannelParameters) _h245(sessionID) LAST_TOKEN }, NULL, sessionId, NULL);

    if (RV_PVT_NODEID_IS_VALID(fwdNodeId) && RV_PVT_NODEID_IS_VALID(revNodeId))
    {
        /* check if such a duplex channel exists */
        getFwdChanBySID(*chan, (HCONTROL)ctrl, (RvUint8)*sessionId);
        if ((*chan == NULL) || (!(*chan)->bIsDuplex))
        {
            *chan = NULL;
            getRevChanBySID(*chan, (HCONTROL)ctrl, (RvUint8)*sessionId);
            if ((*chan != NULL) && (!(*chan)->bIsDuplex))
                *chan = NULL;
        }

        /* check if this is a Null-OLC */
        __pvtGetByFieldIds(*dataNode, hVal, chanMsgId,
            { _h245(forwardLogicalChannelParameters) _h245(dataType) LAST_TOKEN }, NULL, NULL, NULL);
        if (RV_PVT_NODEID_IS_VALID(pvtGetChildByFieldId(hVal, *dataNode, __h245(nullData), NULL, NULL)))
            *dataNode = RV_PVT_INVALID_NODEID;
        return (*chan != NULL);
    }

    if (RV_PVT_NODEID_IS_VALID(fwdNodeId))
    {
        /* check if such a forward channel exists */
        getFwdChanBySID(*chan, (HCONTROL)ctrl, (RvUint8)*sessionId);

        /* check if this is a Null-OLC */
        __pvtGetByFieldIds(*dataNode, hVal, chanMsgId,
            { _h245(forwardLogicalChannelParameters) _h245(dataType) LAST_TOKEN }, NULL, NULL, NULL);
        if (RV_PVT_NODEID_IS_VALID(pvtGetChildByFieldId(hVal, *dataNode, __h245(nullData), NULL, NULL)))
            *dataNode = RV_PVT_INVALID_NODEID;
        return (*chan != NULL);
    }

    if (RV_PVT_NODEID_IS_VALID(revNodeId))
    {
        /* check if such a reverse channel exists */
        getRevChanBySID(*chan, (HCONTROL)ctrl, (RvUint8)*sessionId);

        /* check if this is a Null-OLC */
        __pvtGetByFieldIds(*dataNode, hVal, chanMsgId,
            { _h245(reverseLogicalChannelParameters) _h245(dataType) LAST_TOKEN }, NULL, NULL, NULL);
        if (RV_PVT_NODEID_IS_VALID(pvtGetChildByFieldId(hVal, *dataNode, __h245(nullData), NULL, NULL)))
            *dataNode = RV_PVT_INVALID_NODEID;
        return (*chan != NULL);
    }
    return RV_FALSE;
}

/************************************************************************
 * cmFastStartBuild
 * purpose: Build a single OpenLogicalChannel message for use in fast start
 *          proposals
 * input  : hsCall      - Stack handle for the call
 *          sessionId   - Session id of the proposed channel
 *          direction   - Direction of the proposed channel
 *          fsChannel   - FastStart channel information
 * return : Node ID of created OpenLogicalChannel on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV
cmFastStartBuild(
    IN HCALL                hsCall,
    IN int                  sessionId,
    IN cmChannelDirection   direction,
    IN cmFastStartChannel*  fsChannel)
{
    H245Control* ctrl = (H245Control*)cmiGetControl(hsCall);
    H245Object* app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    HPVT hVal;
    int outElemId,openMsgId=RV_ERROR_UNKNOWN;


    if ((hsCall == NULL) || (app == NULL))
        return RV_ERROR_UNKNOWN;
    hVal = app->hVal;

    H245APIEnter((app->pAppObject, "cmFastStartBuild: hsCall=%p",hsCall));
    if (emaLock((EMAElement)hsCall))
    {
        RvBool notNull = RV_TRUE;
        /* Build a OLC message for the channel */
        openMsgId = pvtAddRoot(hVal, app->synOLC, 0, NULL);
        /* Build the forward logical channel subtree (should be dummy for receiving channel) */
        outElemId=pvtAddBranch(hVal,openMsgId,__h245(forwardLogicalChannelParameters));

        if (direction==dirBoth)
        {
            buildLCParameters(hsCall, outElemId, sessionId, fsChannel);
            /* Now build the reverse logical channel subtree */
            outElemId=pvtAddBranch(hVal,openMsgId,__h245(reverseLogicalChannelParameters));
        }
        if (direction==dirReceive)
        {
            pvtAddBranch2(hVal,outElemId, __h245(multiplexParameters),__h245(none));

            /* Write nullData in the dataType part and dummy number as lcn */
            pvtAddBranch2(hVal,outElemId,__h245(dataType),__h245(nullData));
            pvtAdd(hVal,openMsgId,__h245(forwardLogicalChannelNumber),323,NULL,NULL);

            /* Now get to business and build the reverse logical channel subtree */
            outElemId=pvtAddBranch(hVal,openMsgId,__h245(reverseLogicalChannelParameters));
        }

        if (notNull)
        {
            buildLCParameters(hsCall, outElemId, sessionId, fsChannel);

            if (direction!=dirReceive)
            {
                if (fsChannel != NULL)
                {
                    /* allocate a logical channel number for the current channel and write it into the
                       appEvent message */
                    pvtAdd(hVal,openMsgId, __h245(forwardLogicalChannelNumber), ++ctrl->logicalChannelOut, NULL, NULL);
                }
                else
                {
                    /* Add a dummy number as lcn */
                    pvtAdd(hVal,openMsgId,__h245(forwardLogicalChannelNumber),323,NULL,NULL);
                }
            }
        }

        if (fsChannel != NULL)
        {
            pvtDelete(hVal, fsChannel->dataTypeHandle);
            fsChannel->dataTypeHandle = RV_PVT_INVALID_NODEID;
        }
        emaUnlock((EMAElement)hsCall);
    }
    H245APIExit((app->pAppObject, "cmFastStartBuild=%d", openMsgId));
    return openMsgId;
}


/************************************************************************
 * cmExtFastStartBuildReconf
 * purpose: Build a single OpenLogicalChannelAck message for use in fast
 *          start reconfiguration, rerouting or suspending
 * input  : hsCall      - Stack handle for the call
 *          sessionId   - Session id of the proposed channel
 *          direction   - Direction of the proposed channel
 *          fsChannel   - FastStart channel information
 * return : Node ID of created OpenLogicalChannel on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV
cmExtFastStartBuildAck(
    IN HCALL                hsCall,
    IN int                  sessionId,
    IN cmChannelDirection   direction,
    IN cmFastStartChannel*  fsChannel)
{
    if (direction == dirReceive)
        direction = dirTransmit;
    else if (direction == dirTransmit)
        direction = dirReceive;
    return cmFastStartBuild(hsCall,sessionId, direction, fsChannel);
}


/************************************************************************
 * cmCallAddFastStartMessage
 * purpose: Add an OpenLogicalChannel message to the fast start proposal
 *          on the origin side of the call.
 * input  : hsCall      - Stack handle for the call
 *          fsMessageId - Node ID of the OpenLogicalChannel to add
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV
cmCallAddFastStartMessage(IN HCALL hsCall, IN int fsMessageId)
{
    H245Control*ctrl=(H245Control*)cmiGetControl(hsCall);
    HH245      hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Object *app = (H245Object*)hApp;
    RvStatus   addedOk = RV_ERROR_UNKNOWN;

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmCallAddFastStartMessage: hsCall=%p, fastStartState=%d, ctrl=0x%x, efcState=%d",
        hsCall, ctrl->fastStartState,  ctrl, ctrl->eExtendedFastConnectState));

    if (ctrl->fastStartNodesPrplSendCount == app->maxFsProposed)
    {
        /* array is maxFsProposed+1 size, must leave the last one -1 */
        H245APIExit((app->pAppObject, "cmCallAddFastStartMessage: no more room [-5]"));
        return RV_ERROR_OUTOFRANGE;
    }

    if (emaLock((EMAElement)hsCall))
    {
        RvInt32 sessionId;
        RvPvtNodeId dataNode;
        H245Channel *pChan;
        /* make use the message is for a good session ID */
        if (isReferringToExistingChannel(app->hVal, ctrl, fsMessageId, RV_TRUE, &sessionId, &pChan, &dataNode) &&
            (dataNode != RV_PVT_INVALID_NODEID))
        {
            /* attempt to send a non-null OLC for an existing session ID */
            emaUnlock((EMAElement)hsCall);
            H245APIExit((app->pAppObject, "cmCallAddFastStartMessage: session ID %d already exists (%p) [-10]",
                sessionId, pChan));
            return RV_ERROR_ILLEGAL_ACTION;
        }

        if (((ctrl->fastStartState == fssNo) && (ctrl->callInitiator)) ||
            (ctrl->fastStartState == fssIdle) || (ctrl->fastStartState == fssPrplSend) ||
            /* EFC */
            (ctrl->eExtendedFastConnectState == cmExtendedFastConnectSupported) ||
            (ctrl->eExtendedFastConnectState == cmExtendedFastConnectApproved)
            )
        {
            /* did we already offer EFC? */
            if ((ctrl->eExtendedFastConnectState == cmExtendedFastConnectApproved) &&
                ctrl->bFSWaitForReply && (ctrl->fastStartNodesNewPrplSendIndex == 0))
            {
                /* we already proposed EFC, but we want to propose new channels.
                   mark the starting location of the new offer. */
                ctrl->fastStartNodesNewPrplSendIndex = ctrl->fastStartNodesPrplSendCount;
            }
            /* add the new node */
            ctrl->fastStartNodesPrplSend[ctrl->fastStartNodesPrplSendCount++] = fsMessageId;
            ctrl->fastStartState = fssPrplSend;
            addedOk = RV_OK;
        }
        emaUnlock((EMAElement)hsCall);
    }
    H245APIExit((app->pAppObject, "cmCallAddFastStartMessage=%d", addedOk));

    return addedOk;
}


/******************************************************************************
 * cmCallAddExtFastConnectApproval
 * ----------------------------------------------------------------------------
 * General: Add an OpenLogicalChannel message to the fast start approval.
 *          used to send pause and reconfigure when using EFC.
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsCall      - Stack handle for the call
 *         fsMessageId - Node ID of the OpenLogicalChannel to add
 * Output: None.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmCallAddExtFastConnectApproval(IN HCALL hsCall, IN RvPvtNodeId fsMessageId)
{
    H245Control*ctrl=(H245Control*)cmiGetControl(hsCall);
    HH245      hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Object *app = (H245Object*)hApp;

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmCallAddFastStartMessage: hsCall=%p",hsCall));
    if (ctrl->fastStartNodesAcptSendCount == app->maxFsAccepted)
    {
        /* array is maxFsAccepted+1 size, must leave the last one -1 */
        H245APIExit((app->pAppObject, "cmCallAddExtFastConnectApproval: no more room [-1]"));
        return RV_ERROR_OUTOFRANGE;
    }

    if (emaLock((EMAElement)hsCall))
    {
        if ((ctrl->eExtendedFastConnectState == cmExtendedFastConnectApproved) &&
            ((ctrl->fastStartState == fssIdle) || (ctrl->fastStartState == fssAcptSend)))
        {
            ctrl->fastStartNodesAcptSend[ctrl->fastStartNodesAcptSendCount++] = fsMessageId;
            ctrl->fastStartState = fssAcptSend;
        }
        emaUnlock((EMAElement)hsCall);
    }
    H245APIExit((app->pAppObject, "cmCallAddExtFastConnectApproval=1"));

    return RV_TRUE;
}


/******************************************************************************************
 * cmFastStartOpenChannels
 *
 * Purpose:  This API function enables the caller to supply a structure with data about
 *           the offered logical channels for fast start procedure. The structure includes
 *           all offered channels, both incoming and outgoing, arranged by their type, i.e.
 *           Audio channels, Video channels, etc.
 *
 * Input:    hsCall - A handle to the call whose setup message shall carry the
 *                    fast start offer.
 *
 *           fsMessage - A pointer to the structure containing the channels data.
 *
 * Return Value: RV_TRUE or negative value on failure.
 *
 ****************************************************************************************/
RVAPI int RVCALLCONV
cmFastStartOpenChannels(IN HCALL hsCall, IN cmFastStartMessage* fsMessage)
{
    int openMsgId,index,channelType,ret=1;
    HH245 hApp;
    H245Object *app;

    if (!hsCall || fsMessage->partnerChannelsNum<=0) return RV_ERROR_UNKNOWN;
    hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    if (!hApp) return RV_ERROR_UNKNOWN;

    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmFastStartOpenChannels: hsCall=%p",hsCall));

    if (emaLock((EMAElement)hsCall))
    {
        /* first, delete previous FS offers, if they exist. */
        deleteOutgoingFastStartOffer(hsCall);
         /* This is the main loop that goes over the offered channels in the given structure
         and build from it the sub-tree to be saved in the H245 machine and attached to the
         SETUP message. The order is according to the channel type (Audio, Video etc.) */
        for (channelType = 0; channelType< fsMessage->partnerChannelsNum ;channelType++)
        {
            cmAlternativeChannels* aChannel;
            /* We currently handle only audio and video channels in faststart */
            if ( ((int)fsMessage->partnerChannels[channelType].type < (int)cmCapEmpty) || ((int)fsMessage->partnerChannels[channelType].type > (int)cmCapData) )
                continue;

            aChannel=&fsMessage->partnerChannels[channelType].receive;

            /* Go over the offered receive channels */
            for (index=0;index<aChannel->altChannelNumber;index++)
            {
                /* Build logicalChannel message */
                openMsgId = cmFastStartBuild(hsCall, (int)fsMessage->partnerChannels[channelType].type, dirReceive, &aChannel->channels[index]);
                /* The OLC is ready for the receive channel, encode it */
                if (cmCallAddFastStartMessage(hsCall, openMsgId) != RV_OK)
                {
                    pvtDelete(app->hVal, openMsgId);
                }
            }

            aChannel=&fsMessage->partnerChannels[channelType].transmit;

            /* Now go over the offered transmit channels */
            for (index=0;index<aChannel->altChannelNumber;index++)
            {
                /* Build logicalChannel message */
                openMsgId = cmFastStartBuild(hsCall, (int)fsMessage->partnerChannels[channelType].type, dirTransmit, &aChannel->channels[index]);
                /* The OLC is ready for the receive channel, encode it */
                if (cmCallAddFastStartMessage(hsCall, openMsgId) != RV_OK)
                {
                    pvtDelete(app->hVal, openMsgId);
                }
            }
        }
        emaUnlock((EMAElement)hsCall);
    }
    H245APIExit((app->pAppObject, "cmFastStartOpenChannels=%d", ret));

    return ret;
}


/************************************************************************
 * cmFastStartGetByIndex
 * purpose: Get faststart information of a single OpenLogicalChannel
 *          message from the faststart proposal string
 * input  : hsCall  - Stack handle for the call
 *          index   - Index of the faststart proposal to get (0-based)
 * return : Node ID of the OpenLogicalChannel message proposal on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV
cmFastStartGetByIndex(IN HCALL hsCall, IN int index)
{
    int   nodeId = RV_ERROR_UNKNOWN;
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Object *app;
    H245Control *ctrl;

    if (!hApp || !hsCall) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;
    ctrl = (H245Control*)cmiGetControl(hsCall);

    H245APIEnter((app->pAppObject, "cmFastStartGetByIndex: hsCall=%p",hsCall));

    if (emaLock((EMAElement)hsCall))
    {
        /* get the right channel for the state */
        if (ctrl->fastStartState == fssPrplRcvd)
        {
            /* bounds check */
            if ((index >= 0) && (index < ctrl->fastStartNodesPrplRcvdCount))
                nodeId = ctrl->fastStartNodesPrplRcvd[index];
        }
        else if (ctrl->fastStartState == fssAcptRcvd)
        {
            /* bounds check */
            if ((index >= 0) && (index < ctrl->fastStartNodesAcptRcvdCount))
                nodeId = ctrl->fastStartNodesAcptRcvd[index];
        }
        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmFastStartGetByIndex=%d", nodeId));
    return nodeId;
}


/************************************************************************
 * cmFastStartGet
 * purpose: Get faststart information of a single OpenLogicalChannel
 *          message from the faststart proposal string
 * input  : hsCall      - Stack handle for the call
 *          fsChannelId - PVT node ID of the root of the faststart proposal
 *                        of one of the channels
 * output : sessionId   - Session ID of the proposed channel
 *          direction   - Direction of the proposed channel
 *          fsChannel   - FastStart channel information
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV
cmFastStartGet(
        IN  HCALL               hsCall,
        IN  int                 fsChannelId,
        OUT int*                sessionId,
        OUT cmChannelDirection* direction,
        OUT cmFastStartChannel* fsChannel)
{
    RvPvtNodeId dirNodeId,paramNodeId,tempNodeId, tmpNodeId;
    RvInt32     sid=0;
    HH245       hApp;
    HPVT        hVal;
    H245Object *app;

    if (!hsCall) return RV_ERROR_UNKNOWN;
    hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    if (!hApp) return RV_ERROR_UNKNOWN;

    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmFastStartGet: hsCall=%p",hsCall));
    if (emaLock((EMAElement)hsCall))
    {
        int dataTypeIndex = 0;
        hVal = app->hVal;

        /* Get the reverse part of the OLC message to determine whether it is
         a receiving channel or a transmitting channel (from the viewpoint of
         the remote that sent the SETUP) */
        dirNodeId=pvtGetChild(hVal, fsChannelId, __h245(reverseLogicalChannelParameters),NULL);

        /* in or bidirectional channel */
        /* Get the forward parameters */
        tmpNodeId=pvtGetChild(hVal, fsChannelId,__h245(forwardLogicalChannelParameters),NULL);
        if (pvtGetChild2(hVal, tmpNodeId, __h245(dataType),__h245(nullData))<0)
        {                        /* Forward DataType is OK */
            if (dirNodeId >= 0) /* and there are reverse parameters */
            {   /* bidirectional channel */
                if (direction)
                    *direction=dirBoth;
            }
            else                /* no reverse parameters */
            {
                /* in channel */
                dirNodeId = tmpNodeId;
                if (direction)
                    *direction=dirReceive;
            }
        }
        else                    /* no forward DataType */
        {   /* out channel */

            if (direction)
                *direction=dirTransmit;
        }

        /* Get the H2550 parameters of the channel */
        paramNodeId = pvtGetChild2(hVal, dirNodeId, __h245(multiplexParameters), __h245(h2250LogicalChannelParameters));

        /* set the PVT of the dataType subtree */
        fsChannel->dataTypeHandle = pvtGetChild(hVal, dirNodeId, __h245(dataType), NULL);

        pvtGet(hVal, pvtGetChild(hVal, paramNodeId, __h245(sessionID), NULL), NULL, NULL, &sid, NULL);

        if (sid == 0)
        {
            /* according to the dataType determine if it's an Audio or a Video
            channel and set the session Id accordingly.
            Note: This part needs to be enhanced for support of other types (data, fax etc.)*/
            static char const SID_From_Index[]={0,0,0,2,1,3,0,0,0};

            tempNodeId = pvtChild(hVal, fsChannel->dataTypeHandle);
            dataTypeIndex = pvtGetSyntaxIndex(hVal,tempNodeId);
            if (dataTypeIndex>=0)
                sid=SID_From_Index[dataTypeIndex];
        }

        if (sessionId)
            *sessionId = sid;

        /* Get the codec type name and fill it into the table */
        {
            RvPvtNodeId capNodeId;
            RvPstFieldId fieldId;

            /* For Data capabilities, we have to go one step further... */
            capNodeId = pvtChild(hVal, pvtChild(hVal, fsChannel->dataTypeHandle));
            if (dataTypeIndex == 5) /* 5 is the syntax index of data inside DataType */
                capNodeId = pvtChild(hVal, capNodeId);

            pvtGet(hVal, capNodeId, &fieldId, NULL, NULL, NULL);
            if (RV_PVT_NODEID_IS_VALID(capNodeId))
            {
                fsChannel->channelName = pstGetFieldNamePtr(pvtGetSynTree(hVal, dirNodeId),fieldId);
            }
            else
            {
                fsChannel->channelName = NULL;
            }
        }

        /* Get the rtcp ip address and fill that into the table */
        cmVtToTA_H245(hVal,pvtGetChild(hVal,paramNodeId, __h245(mediaControlChannel), NULL) , &fsChannel->rtcp);

        /* Get the rtp ip address and fill that into the table */
        cmVtToTA_H245(hVal,pvtGetChild(hVal,paramNodeId, __h245(mediaChannel), NULL) , &fsChannel->rtp);

        /* We have no way of knowing the channel index when we are here, so we'll just put in with -1 */
        fsChannel->index = -1;

        emaUnlock((EMAElement)hsCall);
    }
    H245APIExit((app->pAppObject, "cmFastStartGet=0"));
    return 0;
}


/******************************************************************************************
 * analyzeFastStartMsg
 *
 * Purpose:  This function is called upon receipt of a setup message or when EFC is used.
 *           The function checks for any fast-start channels in the message,
 *           decodes the data and builds it into a structure that is passed in a CallBack
 *           to the application, such that the application may ack some of them.
 *
 * Input:    hsCall     - call object instance
 *           message    - The node id to the setup message
 *
 * Return Value: Non-negative value on success
 *                Negative value on failure or NullOLC FS channel proposal
 ****************************************************************************************/
int
analyzeFastStartMsg(IN  HCALL       hsCall,
                    IN  int         message)
{
    H245Control* ctrl = (H245Control*)cmiGetControl(hsCall);
    cmFastStartMessage fsMessage;
    RvPvtNodeId nodeId;
    int nesting;

    RvUint8 lcnAllocationBuff[(MAX_FASTSTART_CHANNELS+1)/sizeof(char)];

    int decoded_length, buff_len, freeLoc=0;
    int index=0;
    RvInt8 sessLoc[MAX_SESSION_ID];
    cmAlternativeChannels *pAltChannel;
    HPVT hVal;
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Object* app=(H245Object*)hApp;

    if (!app->h225CallEvent.cmEvCallFastStartSetup && !app->h225CallEvent.cmEvCallFastStart)
        return RV_TRUE;
    /* if EFC is approved we can proceed */
    if (ctrl->eExtendedFastConnectState != cmExtendedFastConnectApproved)
    {
        if ((ctrl->fastStartState==fssAcptSend) ||
            (ctrl->fastStartState==fssPrplRcvd) ||
            (ctrl->fastStartState==fssRej))
        {
            return RV_TRUE;
        }
    }

    hVal=app->hVal;

    /* Find out if we've got faststart at all in this message (and that it contains something) */
    __pvtGetNodeIdByFieldIds(nodeId, hVal, message,
        { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu)
          _q931(h323_message_body) _anyField _q931(fastStart) _anyField LAST_TOKEN});
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
    {
        ctrl->bIsFastStartFinished = RV_TRUE;
        return RV_ERROR_UNKNOWN;
    }
    ctrl->fastStartState = fssPrplRcvd;

    /* Initialized the table to be built */
    memset(&fsMessage,0,sizeof(cmFastStartMessage));
    memset(sessLoc,0xff,sizeof(sessLoc));
    memset(lcnAllocationBuff,0,sizeof(lcnAllocationBuff));

    /* Go over the received encoded OLC message from the SETUP,
       decode them for subsequent analysis */
    while (RV_PVT_NODEID_IS_VALID(nodeId))
    {
        RvUint8* encodeBuffer;

        /* Get a buffer to work with */
        getEncodeDecodeBuffer(app->encodeBufferSize, &encodeBuffer);
        if (encodeBuffer != NULL)
        {
            buff_len = pvtGetString(hVal, nodeId, app->encodeBufferSize, (char*)encodeBuffer);
        }
        else
            buff_len = -1; /* Make sure we don't continue */

        if (buff_len > 0)
        {
            /* Decode an OpenLogicalChannel faststart message */
            int decodingResult, lcn;
            int chanMsgId=ctrl->fastStartNodesPrplRcvd[index]=pvtAddRoot(hVal, app->synOLC, 0, NULL);

            ctrl->fastStartNodesPrplRcvdCount++;

            decodingResult = cmEmDecode(hVal, chanMsgId, encodeBuffer, buff_len, &decoded_length);
            if (decodingResult < 0)
                chanMsgId = RV_PVT_INVALID_NODEID;

            RvH323CmPrintMessage(&app->logFastStart, "Suggested faststart channel decoded:",
                hVal, chanMsgId, encodeBuffer, decoded_length, RV_TRUE);

            if (decodingResult < 0)
            {
                 /* Got an error - try next fs proposal */
                nodeId = pvtBrother(hVal, nodeId);

                pvtDelete(hVal, ctrl->fastStartNodesPrplRcvd[index]);
                ctrl->fastStartNodesPrplRcvd[index] = RV_PVT_INVALID_NODEID;
                ctrl->fastStartNodesPrplRcvdCount--;
                continue;
            }

            /* Make the logical channel's number as taken */
            pvtGetChildByFieldId(hVal, chanMsgId,__h245(forwardLogicalChannelNumber),&lcn,NULL);
            setBitTrue(lcnAllocationBuff, lcn/MAX_LCN_S);

            if (ctrl->eExtendedFastConnectState != cmExtendedFastConnectNo)
            {
                RvInt32 sessionId = 0;
                H245Channel *channel = NULL;
                RvBool bNull, bDeleteNode = RV_FALSE;
                cmChannelDirection dir;

                /* check if this is a NULL-OLC */
                bNull = isNullOLC(hVal, chanMsgId, RV_FALSE, &sessionId, &dir);

                /* see if the session ID refers to an existing channel */
                channel = getOutChanBySID((HCONTROL)ctrl, (RvUint8)sessionId);
                if (emaLock((EMAElement)channel))
                {
                    /* node refers to existing channel delete it even if it is not a Null-OLC */
                    if (bNull)
                    {
                        /* drop the channel */
                        channel->eState = ChannelStateReleased;
                        notifyChannelState(channel, cmChannelStateDisconnected, cmChannelStateModeDisconnectedLocal);
                        notifyChannelState(channel, cmChannelStateIdle, cmChannelStateModeOff);
                    }
                    else if ((dir == dirTransmit) || (dir == dirBoth))
                    {
                        /* we already have a channel with this node ID and direction. delete the offer */
                        bDeleteNode = RV_TRUE;
                    }
                    emaUnlock((EMAElement)channel);
                }
                channel = getInChanBySID((HCONTROL)ctrl, (RvUint8)sessionId);
                if (emaLock((EMAElement)channel))
                {
                    /* node refers to existing channel delete it even if it is not a Null-OLC */
                    if (bNull)
                    {
                        /* drop the channel */
                        channel->eState = ChannelStateReleased;
                        notifyChannelState(channel, cmChannelStateDisconnected, cmChannelStateModeDisconnectedLocal);
                        notifyChannelState(channel, cmChannelStateIdle, cmChannelStateModeOff);
                    }
                    else if ((dir == dirReceive) || (dir == dirBoth))
                    {
                        /* we already have a channel with this node ID and direction. delete the offer */
                        bDeleteNode = RV_TRUE;
                    }
                    emaUnlock((EMAElement)channel);
                }
                if (ctrl->fastStartNodesPrplSendCount != 0)
                {
                    int i,j;
                    RvInt32 proposalSessionId;
                    RvBool bProposalNull, bSlave;

                    /* determine if we are the masters or the slaves */
                    if ((ctrl->eState == ctrlConnected) || (ctrl->eState == ctrlConference))
                    {
                        bSlave = !ctrl->bIsMaster;
                    }
                    else
                    {
                        bSlave = ctrl->callInitiator;
                    }

                    for (i=0; i<ctrl->fastStartNodesPrplSendCount; i++)
                    {
                        proposalSessionId = -1;
                        bProposalNull = isNullOLC(hVal, ctrl->fastStartNodesPrplSend[i], RV_TRUE, &proposalSessionId, NULL);

                        if (proposalSessionId == sessionId)
                        {
                            /* if this is a Null-OLC, or we are the slaves of this call, delete our
                            proposal in favor of the master's */
                            if (bNull || bSlave)
                            {
                                if ((app->h225CallEvent.cmH245EvExtFastConnectProposalCanceled != NULL) &&
                                    !bProposalNull)
                                {
                                    app->h225CallEvent.cmH245EvExtFastConnectProposalCanceled(
                                        (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall), (HCALL)hsCall,
                                        sessionId, ctrl->fastStartNodesPrplSend[i]);
                                }

                                pvtDelete(hVal, ctrl->fastStartNodesPrplSend[i]);

                                for (j=i; j<(ctrl->fastStartNodesPrplSendCount-1); j++)
                                {
                                    ctrl->fastStartNodesPrplSend[j] = ctrl->fastStartNodesPrplSend[j+1];
                                }
                                ctrl->fastStartNodesPrplSend[j] = RV_PVT_INVALID_NODEID;
                                i--;
                                ctrl->fastStartNodesPrplSendCount--;
                            }
                            /* if we are the masters, delete the remote's proposal node, it's irrelevant */
                            if (!bSlave)
                            {
                                bDeleteNode = RV_TRUE;
                            }
                        }
                    }
                }

                /* see if we should delete the current proposal */
                if (bNull || bDeleteNode)
                {
                    /* Go to next fs proposal */
                    nodeId = pvtBrother(hVal, nodeId);

                    pvtDelete(hVal, ctrl->fastStartNodesPrplRcvd[index]);
                    ctrl->fastStartNodesPrplRcvd[index] = RV_PVT_INVALID_NODEID;
                    ctrl->fastStartNodesPrplRcvdCount--;
                    continue;
                }
            }
            else if (app->h225CallEvent.cmEvCallFastStartSetup)
            {
                cmFastStartChannel fsChannel;
                int sessionId;
                cmChannelDirection direction;
                int loc;

                /* We now have an offered channel */
                /* Go over the OLC message and analyze it */

                cmFastStartGet(hsCall, chanMsgId, &sessionId, &direction, &fsChannel);

                /* Fill the session Id into the channel table being prepared for the user */
                if (sessLoc[sessionId] == -1)
                {
                    if (freeLoc != MAX_FS_CHANNELS)
                    {
                        loc = freeLoc++;
                        sessLoc[sessionId] = (RvInt8)loc;
                        fsMessage.partnerChannels[loc].type = (cmCapDataType)sessionId;
                        fsMessage.partnerChannelsNum++;
                    }
                    else
                        loc = -1;
                }
                else
                    loc = sessLoc[sessionId];

                if (loc >= 0)
                {
                    /* Set a pointer to the right part of the channel tables
                     according to the channels direction */
                    if (direction==dirTransmit)
                                        pAltChannel    = &(fsMessage.partnerChannels[loc].transmit);
                    else                pAltChannel    = &(fsMessage.partnerChannels[loc].receive);


                    /* Now fill all the data from the OLC message to the table */
                    if (pAltChannel->altChannelNumber < MAX_ALTERNATIVE_CHANNEL)
                    {
                        /* Give the channel an index */
                        fsChannel.index = index;

                        pAltChannel->channels[pAltChannel->altChannelNumber]=fsChannel;

                        /* Advance the channels counter in the table */
                        pAltChannel->altChannelNumber++;
                    }
                }
            }
        }
        nodeId = pvtBrother(hVal, nodeId);
        index++;
        if (index == app->maxFsProposed)
        {
            /* we were offered more channels than we have room for - ignore the rest */
            /* array is maxFsProposed+1 size, must leave the last one -1 */
            break;
        }
    }

    if (ctrl->fastStartNodesPrplSendCount == 0)
    {
        /* all our proposals got canceled or counter-proposed - we no longer
           wait for reply */
        ctrl->bFSWaitForReply = RV_FALSE;
    }
    if (ctrl->fastStartNodesPrplRcvdCount == 0)
    {
        /* no real offer was received. this was either a null-OLC, or a counter
           proposal when the local side is the call master. */
        if (ctrl->fastStartNodesPrplSendCount == 0)
        {
            /* either we were not proposing or we received a null-OLC that
               canceled all our proposals */
            ctrl->fastStartState = fssIdle;
            if (!ctrl->bIsFastStartFinished)
            {
                ctrl->bIsFastStartFinished = RV_TRUE;
                cmcReadyEvent(ctrl);
            }
        }
        else
        {
            /* maybe just some of our proposals were canceled, we still have
               valid proposals */
            ctrl->fastStartState = fssPrplSend;
        }
        /* no proposals - no need to raise callbacks */
        return RV_TRUE;
    }

    if (ctrl->logicalChannelOut == 0)
    {
        /*If the number of fast start proposals was less then MAX_FASTSTART_CHANNELS then at least
        one range will be available - see the beginning of the file */
        ctrl->logicalChannelOut = (RvUint16)(get1st0BitNumber(lcnAllocationBuff, 0, MAX_FASTSTART_CHANNELS)*MAX_LCN_S);
    }

    if ((app->h225CallEvent.cmEvCallFastStartSetup) &&
        (ctrl->eExtendedFastConnectState != cmExtendedFastConnectApproved))
    {
        /* We call the callback with the table and supply it to the user.
         The user is supposed to acknowledge the desired channels from that callback */
        H245CBEnter((app->pAppObject, "cmEvCallFastStartSetup(haCall=%p,hsCall=%p)", (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall), (HCALL)hsCall));
        nesting=emaPrepareForCallback((EMAElement)hsCall);
        app->h225CallEvent.cmEvCallFastStartSetup((HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall), (HCALL)hsCall,&fsMessage);
        emaReturnFromCallback((EMAElement)hsCall,nesting);
        H245CBExit((app->pAppObject, "cmEvCallFastStartSetup"));
    }

    if (!emaWasDeleted((EMAElement)hsCall) && app->h225CallEvent.cmEvCallFastStart)
    {
        H245CBEnter((app->pAppObject, "cmEvCallFastStart(haCall=%p,hsCall=%p)", (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall), (HCALL)hsCall));
        nesting=emaPrepareForCallback((EMAElement)hsCall);
        app->h225CallEvent.cmEvCallFastStart((HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall), (HCALL)hsCall);
        emaReturnFromCallback((EMAElement)hsCall,nesting);
        H245CBExit((app->pAppObject, "cmEvCallFastStart"));
    }

    return RV_TRUE;
}


/******************************************************************************
 * cmFastStartChannelsAckIndex
 * ----------------------------------------------------------------------------
 * General: Builds and adds an ACK-OLC message to the given channel index.
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsCall      - Stack handle for the call
 *         index       - Index of the FS offer to acknowledge
 *         rtcp        - Pointer to the RTCP address for the OLC-ACK
 *         rtp         - Pointer to the RTP address for the OLC-ACK
 * Output: None.
 *****************************************************************************/
RVAPI int RVCALLCONV
cmFastStartChannelsAckIndex(
    IN HCALL                hsCall,
    IN int                  index,
    IN cmTransportAddress*  rtcp,
    IN cmTransportAddress*  rtp)
{
    int ret = RV_ERROR_DESTRUCTED;
    RvPvtNodeId lcpNodeId;
    RvPvtNodeId h2250ParametersNodeId;
    RvPvtNodeId outOLCNodeId;
    HH245 hApp;
    HPVT hVal;
    H245Object* app;
    H245Control* ctrl;

    if (emaLock((EMAElement)hsCall))
    {
        hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        ctrl=(H245Control*)cmiGetControl(hsCall);
        if (!hApp || !ctrl)
        {
            emaUnlock((EMAElement)hsCall);
            return RV_ERROR_UNKNOWN;
        }
        app=(H245Object*)hApp;
        hVal=app->hVal;

        H245APIEnter((app->pAppObject, "cmFastStartChannelsAckIndex: hsCall=%p,index = %d",hsCall,index));

        if (ctrl->fastStartNodesAcptSendCount == app->maxFsAccepted)
        {
            /* array is maxFsAccepted+1 size, must leave the last one -1 */
            H245APIExit((app->pAppObject, "cmFastStartChannelsAckIndex: out of room [-1]"));
            emaUnlock((EMAElement)hsCall);
            return RV_ERROR_OUTOFRANGE;
        }

        /* Start build OLC for ack */
        outOLCNodeId = pvtAddRoot(hVal, app->synOLC, 0, NULL);

        if (outOLCNodeId >= 0)
        {
            ctrl->fastStartIndexes[ctrl->fastStartNodesAcptSendCount] = (RvUint8) index;
            ctrl->fastStartNodesAcptSend[ctrl->fastStartNodesAcptSendCount++] = outOLCNodeId;

            /* Copy the OLC*/
            pvtSetTree(hVal,outOLCNodeId, hVal, ctrl->fastStartNodesPrplRcvd[index]);

            /* check the channel direction, only proposal for outgoing and bi-directional channels have reverse parameters*/
            lcpNodeId=pvtGetChild(hVal, outOLCNodeId, __h245(reverseLogicalChannelParameters),NULL);

            if (lcpNodeId >= 0)
            {
                RvPvtNodeId tempNode;
                __pvtGetNodeIdByFieldIds(tempNode, hVal, outOLCNodeId,
                    {_h245(forwardLogicalChannelParameters) _h245(dataType) _h245(nullData) LAST_TOKEN});

                if (RV_PVT_NODEID_IS_VALID(tempNode))
                {
                    /* outgoing (reverse) channel */
                    /* Overwrite the forward LCN */
                    pvtAdd(hVal, outOLCNodeId, __h245(forwardLogicalChannelNumber),++ctrl->logicalChannelOut, NULL,NULL);
                }
                else
                {
                    /* bi-directional channel */
                }
            }
            else
            {
                /* incoming (forward) channel */
                lcpNodeId=pvtGetChild(hVal, outOLCNodeId, __h245(forwardLogicalChannelParameters),NULL);
            }

            h2250ParametersNodeId=pvtGetChild2(hVal, lcpNodeId, __h245(multiplexParameters), __h245(h2250LogicalChannelParameters));

            if (h2250ParametersNodeId>=0)
            {
                int tmpNodeId;
                tmpNodeId = pvtAddBranch(hVal, h2250ParametersNodeId, __h245(mediaChannel));
                if ((rtp != NULL) && (cmTaHasIp(rtp) || rtp->port))
                {
                    getGoodAddressForCall(hsCall,rtp);
                    cmTAToVt_H245(hVal,tmpNodeId, rtp);
                }
                else
                    pvtDelete(hVal, tmpNodeId);

                tmpNodeId = pvtAddBranch(hVal, h2250ParametersNodeId, __h245(mediaControlChannel));
                if ((rtcp != NULL) && (cmTaHasIp(rtcp) || rtcp->port))
                {
                    getGoodAddressForCall(hsCall,rtcp);
                    cmTAToVt_H245(hVal,tmpNodeId, rtcp);
                }
                else
                    pvtDelete(hVal, pvtGetChild(hVal, h2250ParametersNodeId, __h245(mediaControlChannel), NULL));
            }
        }
        ret = outOLCNodeId;

        H245APIExit((app->pAppObject, "cmFastStartChannelsAckIndex=%d", ret));
        emaUnlock((EMAElement)hsCall);
    }
    return ret;
}


/******************************************************************************
 * cmFastStartChannelsAck
 * ----------------------------------------------------------------------------
 * General: Builds and adds an ACK-OLC message to the given channel structure.
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsCall       - Stack handle for the call
 *         pFastChannel - Pointer to the FS channel structure to acknowledge
 * Output: None.
 *****************************************************************************/
RVAPI int RVCALLCONV
cmFastStartChannelsAck(
    IN HCALL                hsCall,
    IN cmFastStartChannel*  pFastChannel)
{
    int ret=1;
    H245Object *app;

    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    if (app == NULL)
        return RV_ERROR_DESTRUCTED;

    H245APIEnter((app->pAppObject, "cmFastStartChannelsAck: hsCall=%p",hsCall));
    if (emaLock((EMAElement)hsCall))
    {
        ret = cmFastStartChannelsAckIndex(hsCall,pFastChannel->index, &(pFastChannel->rtcp), &(pFastChannel->rtp));
        emaUnlock((EMAElement)hsCall);
    }
    H245APIExit((app->pAppObject, "cmFastStartChannelsAck=%d", ret));

    return ret;
}


/******************************************************************************
 * cmFastStartChannelsReady
 * ----------------------------------------------------------------------------
 * General: Indicate to the stack that the channel acknowledgments are all
 *          done, and that the OLC-ACK messages can be sent.
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsCall      - Stack handle for the call
 * Output: None.
 *****************************************************************************/
RVAPI int RVCALLCONV
cmFastStartChannelsReady(IN HCALL hsCall)
{
    H245Control *ctrl;
    H245Object *app;

    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    ctrl=(H245Control *)cmiGetControl(hsCall);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    if (app == NULL)
        return RV_ERROR_DESTRUCTED;

    H245APIEnter((app->pAppObject, "cmFastStartChannelsReady: hsCall=%p",hsCall));
    if (emaLock((EMAElement)hsCall))
    {
        RvPvtNodeId fastStartPrpd[2*MAX_FS_SESSION_ID + 1];
        int i;
        int limit = RvMin(ctrl->fastStartNodesAcptSendCount, (2*MAX_FS_SESSION_ID));

        if (limit > 0)
        {
            memset(fastStartPrpd, 0xff, sizeof(fastStartPrpd));
            for(i=0; i<limit; i++)
            {
                fastStartPrpd[i] = ctrl->fastStartNodesPrplRcvd[ctrl->fastStartIndexes[i]];
            }

            if (cmCallFastStartOpenChannels((HCALL)hsCall, fastStartPrpd, ctrl->fastStartNodesAcptSend, RV_FALSE) >= 0)
            {
                if (ctrl->eExtendedFastConnectState != cmExtendedFastConnectApproved)
                {
                    /* Notify the application that the fast start procedure is complete */
                    cmiReportControl((HCONTROL)ctrl,cmControlStateFastStartComplete, cmControlStateModeFastStartNoMoreChannels);
                }
            }
        }

        ctrl->bIsFastStartFinished = RV_TRUE;
        cmcReadyEvent(ctrl);
        emaUnlock((EMAElement)hsCall);
    }
    H245APIExit((app->pAppObject, "cmFastStartChannelsReady=1"));

    return RV_TRUE;
}


/******************************************************************************
 * cmFastStartChannelsRefuse
 * ----------------------------------------------------------------------------
 * General: Indicate to the stack that we do not want any of the offered
 *          channels
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsCall      - Stack handle for the call
 * Output: None.
 *****************************************************************************/
RVAPI int RVCALLCONV
cmFastStartChannelsRefuse(IN HCALL hsCall)
{
    H245Object *app;

    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    if (app == NULL)
        return RV_ERROR_DESTRUCTED;

    H245APIEnter((app->pAppObject, "cmFastStartChannelsRefuse: hsCall=%p",hsCall));
    if (emaLock((EMAElement)hsCall))
    {
        H245Control *ctrl = (H245Control *)cmiGetControl(hsCall);

        if (ctrl->eExtendedFastConnectState != cmExtendedFastConnectApproved)
        {
            ctrl->fastStartState = fssRej;
            ctrl->bIsFastStartFinished = RV_TRUE;
            cmcReadyEvent(ctrl);
        }
        else if (ctrl->fastStartState == fssPrplRcvd)
        {
            RvInt32 sessionId = -1;
            RvPvtNodeId nodeId;
            RvBool sessRej[MAX_SESSION_ID];
            int i;

            ctrl->fastStartState = fssPrplSend;
            memset(sessRej, 0, sizeof(sessRej));

            /* send Null-OLCs for each session Id offered */
            for (i = 0; i < ctrl->fastStartNodesPrplRcvdCount; i++)
            {
                if ((!isNullOLC(app->hVal, ctrl->fastStartNodesPrplRcvd[i], RV_FALSE, &sessionId, NULL)) &&
                    (!sessRej[sessionId-1]))
                {
                    nodeId = cmFastStartBuild(hsCall, sessionId, dirTransmit, NULL);
                    cmCallAddFastStartMessage(hsCall, nodeId);
                    sessRej[sessionId-1] = RV_TRUE;
                }
            }
            deleteIncomingFastStartOffer(hsCall);
        }
        emaUnlock((EMAElement)hsCall);
    }
    H245APIExit((app->pAppObject, "cmFastStartChannelsRefuse=1"));
    return RV_TRUE;
}


/******************************************************************************************
 * cmFastStartReply
 *
 * Purpose:  This function is called from the CM whenever a response message after SETUP
 *           is received (CallProceeding, Alerting, Connect, Facility and Progress).
 *           It checks for FastStart response. If such exists, it processes it and opens
 *           the relevant channels.
 *
 * Input:    hsCall     - call object instance
 *           message    - node ID of the message
 *           isConnect  - True iff this is a connect message
 *
 * Returned Value: Non-negative value on success
 *                 Negative value on failure
 ****************************************************************************************/
int cmFastStartReply(
    IN HCALL        hsCall,
    IN RvPvtNodeId  message,
    IN RvBool       isConnect)
{
    H245Object*app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Control *ctrl=(H245Control *)cmiGetControl(hsCall);
    HPVT hVal=app->hVal;
    RvPvtNodeId uuNodeId;
    RvPvtNodeId fsNodeId;
    RvBool bEfcReconf = RV_FALSE;
    int  rc;

    __pvtGetNodeIdByFieldIds(uuNodeId, hVal, message,
        { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu)
          _q931(h323_message_body) _anyField LAST_TOKEN});

    /* first check if fast start was refused */
    if ((ctrl->fastStartState == fssPrplSend) &&
        RV_PVT_NODEID_IS_VALID(pvtGetChild(hVal, uuNodeId, __q931(fastConnectRefused), NULL)))
    {
        /* fast connection was refused. set it as finished and see if control can be connected */
        ctrl->fastStartState = fssRej;
        cmiReportControl((HCONTROL)ctrl, cmControlStateFastStartComplete, cmControlStateModeFastStartRejected);
        ctrl->bIsFastStartFinished = RV_TRUE;
        ctrl->bFSWaitForReply = RV_FALSE;
        cmcReadyEvent(ctrl);
        return RV_OK;
    }

    /* In this test, don't check for the Approved state, since we may get an EFC
       style rejection. we're checking the EFC generic data inside, so we won't
       treat it as a proposal by mistake. */
    if (ctrl->eExtendedFastConnectState != cmExtendedFastConnectNo)
    {
        RvInt32 prameter;
        /* this might actually be a proposal. check it out */
        if (cmGetExtFastConnectGenericData(hsCall, message, &prameter) == RV_OK)
        {
            if (prameter == 1)
            {
                /* this is actually a proposal. call the correct function */
                analyzeFastStartMsg(hsCall, message);

                /* no need to delete the outgoing FS proposal */
                return RV_ERROR_UNKNOWN;
            }
            else if (prameter == 2)
            {
                /* drop all was requested */
                H245Channel * channel = (H245Channel *)ctrl->hFirstChannel;
                while (channel != NULL)
                {
                    emaLock((EMAElement)channel);
                    channel->eState = ChannelStateReleased;
                    notifyChannelState(channel, cmChannelStateDisconnected, cmChannelStateModeDisconnectedLocal);
                    notifyChannelState(channel, cmChannelStateIdle, cmChannelStateModeOff);
                    emaUnlock((EMAElement)channel);
                    channel = channel->pNext;
                }
            }
            else
            {
                /* some unknown parameter */
                return RV_ERROR_UNKNOWN;
            }
        }
        else
        {
            /* no generic data parameter, this may still be a reconfiguration */
            bEfcReconf = RV_TRUE;
        }
    }

    /* Are we in the right fast-start state? */
    if ((ctrl->fastStartState != fssPrplSend) && (!bEfcReconf))
        return RV_ERROR_UNKNOWN;

    /* Make sure we've got fast start information here and get the first one */
    __pvtGetNodeIdByFieldIds(fsNodeId, hVal, uuNodeId, {_q931(fastStart) _anyField LAST_TOKEN});

    /* Loop through all of the acknowledgments and put them into the call's structure */
    while (RV_PVT_NODEID_IS_VALID(fsNodeId))
    {
        RvUint8* encodeBuffer;
        int length;
        int decoded_length;
        RvPvtNodeId chanMsgId;
        int decodingResult;

        /* Decode a single OLC.Ack message */
        chanMsgId = pvtAddRoot(hVal, app->synOLC, 0, NULL);
        getEncodeDecodeBuffer(app->encodeBufferSize, &encodeBuffer);
        length=pvtGetString(hVal, fsNodeId, app->encodeBufferSize, (char*)encodeBuffer);

        decodingResult = cmEmDecode(hVal, chanMsgId, encodeBuffer, length, &decoded_length);
        if (decodingResult < 0)
        {
            pvtDelete(hVal, chanMsgId);
            chanMsgId = RV_PVT_INVALID_NODEID;
        }

        RvH323CmPrintMessage(&app->logFastStart, "Accepted faststart channel decoded:",
            hVal, chanMsgId, encodeBuffer, decoded_length, RV_TRUE);

        if (decodingResult >= 0)
        {
            if (bEfcReconf)
            {
                RvInt32 sessionId = 0;
                H245Channel *channel = NULL;
                RvPvtNodeId dataNode = RV_PVT_INVALID_NODEID;
                /* check if this OLC refers an existing channel */
                if (isReferringToExistingChannel(hVal, ctrl, chanMsgId, RV_TRUE, &sessionId, &channel, &dataNode))
                {
                    if (emaLock((EMAElement)channel))
                    {
                        if (RV_PVT_NODEID_IS_VALID(dataNode))
                        {
                            /* the channel is reconfigured - set the new data type */
                            if (!RV_PVT_NODEID_IS_VALID(channel->dataTypeID))
                            {
                                channel->dataTypeID = pvtAddRoot(hVal, NULL, 0, NULL);
                            }
                            pvtSetTree(hVal, channel->dataTypeID, hVal, dataNode);

                            /* notify that the channel is active */
                            notifyChannelState(channel, cmChannelStateConnected, cmChannelStateModeReconfigured);
                        }
                        else
                        {
                            /* the channel is paused */
                            notifyChannelState(channel, cmChannelStateConnected, cmChannelStateModeOff);
                        }
                        emaUnlock((EMAElement)channel);
                    }
                    fsNodeId=pvtBrother(hVal, fsNodeId);
                    pvtDelete(hVal, chanMsgId);
                    chanMsgId = RV_PVT_INVALID_NODEID;
                    continue;
                }
                else if (!RV_PVT_NODEID_IS_VALID(dataNode))
                {
                    /* NULL-Reconfigure-OLC for no existing channel - get rid of it */
                    fsNodeId=pvtBrother(hVal, fsNodeId);
                    pvtDelete(hVal, chanMsgId);
                    chanMsgId = RV_PVT_INVALID_NODEID;
                    continue;
                }
            }
            if (ctrl->fastStartNodesPrplSendCount > 0)
            {
                /* Add the decoded OLC.Ack into the fast start acknowledgments of the call */
                ctrl->fastStartNodesAcptRcvd[ctrl->fastStartNodesAcptRcvdCount++] = chanMsgId;
            }
            else
            {
                pvtDelete(hVal, chanMsgId);
            }
        }

        /* Goto the next one in the next entry to the while loop */
        if (ctrl->fastStartNodesAcptRcvdCount == app->maxFsAccepted)
        {
            /* array is maxFsAccepted+1 size, must leave the last one -1 */
            break;
        }
        fsNodeId = pvtBrother(hVal, fsNodeId);
    }

    if (ctrl->fastStartNodesAcptRcvdCount > 0)
    {
        ctrl->fastStartState = fssAcptRcvd;
        rc = cmCallFastStartOpenChannels(hsCall, ctrl->fastStartNodesPrplSend, ctrl->fastStartNodesAcptRcvd, RV_TRUE);

        ctrl->bIsFastStartFinished = RV_TRUE;
        if (ctrl->eExtendedFastConnectState != cmExtendedFastConnectApproved)
        {
            /* Notify the application that the fast start procedure is complete */
            cmiReportControl((HCONTROL)ctrl, cmControlStateFastStartComplete, cmControlStateModeFastStartNoMoreChannels);
        }
        cmcReadyEvent(ctrl);

        if (ctrl->fastStartNodesPrplSendCount == 0)
        {
            if (ctrl->fastStartNodesPrplRcvdCount > 0)
                ctrl->fastStartState = fssPrplRcvd;
            else
                ctrl->fastStartState = fssIdle;
        }
        else
        {
            if (ctrl->eExtendedFastConnectState == cmExtendedFastConnectApproved)
                /* indicate not to delete the FS proposal */
                rc = RV_ERROR_UNKNOWN;
        }
    }
    else
    {
        if ((isConnect) && (ctrl->eExtendedFastConnectState != cmExtendedFastConnectApproved))
        {
            /* fast connection was refused. set it as finished and see if control can be connected */
            ctrl->fastStartState = fssRej;
            cmiReportControl((HCONTROL)ctrl, cmControlStateFastStartComplete, cmControlStateModeFastStartRejected);
            ctrl->bIsFastStartFinished = RV_TRUE;
            cmcReadyEvent(ctrl);
        }
        rc = RV_ERROR_UNKNOWN;
    }

    /* if rc is non-negative - we got a FS reply */
    if (rc >= 0)
      ctrl->bFSWaitForReply = RV_FALSE;

    return rc;
}


void deleteFastStart(HCALL hsCall)
{
    H245Object *app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Control *ctrl=(H245Control *)cmiGetControl(hsCall);
    int i;

    for (i=0;i<ctrl->fastStartNodesPrplSendCount;i++)
    {
        pvtDelete(app->hVal,ctrl->fastStartNodesPrplSend[i]);
        ctrl->fastStartNodesPrplSend[i]=RV_ERROR_UNKNOWN;
    }
    ctrl->fastStartNodesPrplSendCount=0;

    for (i=0;i<ctrl->fastStartNodesAcptRcvdCount;i++)
    {
        pvtDelete(app->hVal,ctrl->fastStartNodesAcptRcvd[i]);
        ctrl->fastStartNodesAcptRcvd[i]=RV_ERROR_UNKNOWN;
    }
    ctrl->fastStartNodesAcptRcvdCount=0;

    for (i=0;i<ctrl->fastStartNodesPrplRcvdCount;i++)
    {
        pvtDelete(app->hVal,ctrl->fastStartNodesPrplRcvd[i]);
        ctrl->fastStartNodesPrplRcvd[i]=RV_ERROR_UNKNOWN;
    }
    ctrl->fastStartNodesPrplRcvdCount=0;

    for (i=0;i<ctrl->fastStartNodesAcptSendCount;i++)
    {
        pvtDelete(app->hVal,ctrl->fastStartNodesAcptSend[i]);
        ctrl->fastStartNodesAcptSend[i]=RV_ERROR_UNKNOWN;
    }
    ctrl->fastStartNodesAcptSendCount=0;
}


void deleteIncomingFastStartOffer(HCALL hsCall)
{
    H245Object *app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Control *ctrl=(H245Control *)cmiGetControl(hsCall);
    int i;

    for (i=0;i<ctrl->fastStartNodesPrplRcvdCount;i++)
    {
        pvtDelete(app->hVal,ctrl->fastStartNodesPrplRcvd[i]);
        ctrl->fastStartNodesPrplRcvd[i]=RV_ERROR_UNKNOWN;
    }
    ctrl->fastStartNodesPrplRcvdCount=0;

    for (i=0;i<ctrl->fastStartNodesAcptSendCount;i++)
    {
        pvtDelete(app->hVal,ctrl->fastStartNodesAcptSend[i]);
        ctrl->fastStartNodesAcptSend[i]=RV_ERROR_UNKNOWN;
    }
    ctrl->fastStartNodesAcptSendCount=0;
}


void deleteOutgoingFastStartOffer(HCALL hsCall)
{
    H245Object *app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Control *ctrl=(H245Control *)cmiGetControl(hsCall);
    int i;

    for (i=0;i<ctrl->fastStartNodesPrplSendCount;i++)
    {
        pvtDelete(app->hVal,ctrl->fastStartNodesPrplSend[i]);
        ctrl->fastStartNodesPrplSend[i]=RV_ERROR_UNKNOWN;
    }
    ctrl->fastStartNodesPrplSendCount=0;

    for (i=0;i<ctrl->fastStartNodesAcptRcvdCount;i++)
    {
        pvtDelete(app->hVal,ctrl->fastStartNodesAcptRcvd[i]);
        ctrl->fastStartNodesAcptRcvd[i]=RV_ERROR_UNKNOWN;
    }
    ctrl->fastStartNodesAcptRcvdCount=0;
}


/******************************************************************************************
 * addFastStart
 *
 * Purpose:  This function adds the fast start messages that are stored on the call
 *           to an outgoing message
 *
 * Input:    hsCall     - call object instance
 *           message    - message's root node ID to add fast start information to
 *
 * Returned Value: none
 ****************************************************************************************/
void addFastStart(
    IN HCALL    hsCall,
    IN int      message)
{
    H245Object *app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Control *ctrl=(H245Control *)cmiGetControl(hsCall);
    HPVT hVal=app->hVal;
    int nodeCount, startNode, *nodes;
    RvPstFieldId messageFieldId;
    RvPvtNodeId tmpNode;

    __pvtGetByFieldIds(tmpNode, hVal, message, {_q931(message) _anyField LAST_TOKEN}, &messageFieldId, NULL, NULL);

    /* check if this is the case of an outgoing setup supporting EFC, but without FS */
    if ((messageFieldId == __q931(setup)) &&
        ((ctrl->eExtendedFastConnectState == cmExtendedFastConnectRequired) ||
         (ctrl->eExtendedFastConnectState == cmExtendedFastConnectSupported)) &&
        ((ctrl->fastStartState != fssPrplSend) ||
         (ctrl->fastStartNodesPrplSendCount == 0)))
    {
        /* cancel EFC */
        cmCallSetParam(hsCall, cmParamExtendedFastConnectState, 0, cmExtendedFastConnectNo, NULL);
        return;
    }

    /* See which side is it - we need to know which fast start node to add */
    if (ctrl->fastStartState == fssPrplSend)
    {
        /* Initiator of the call */
        nodeCount = ctrl->fastStartNodesPrplSendCount;
        startNode = ctrl->fastStartNodesNewPrplSendIndex; /* initial value = 0 */
        nodes = ctrl->fastStartNodesPrplSend;

        if (ctrl->eExtendedFastConnectState != cmExtendedFastConnectApproved)
        {
            /* Make sure it's a setup message - otherwise we shouldn't add the fast start nodes */
            if (messageFieldId != __q931(setup))
                return;
        }
        else
        {
            /* Make sure it's not a progress message - otherwise we cannot use EFC */
            if (messageFieldId == __q931(progress))
                return;
            /* did we already offer this? do we have new proposals since the
               last time we offered EFC? */
            if (ctrl->bFSWaitForReply && (ctrl->fastStartNodesNewPrplSendIndex == 0))
                return;
        }
    }
    else if (ctrl->fastStartState == fssAcptSend)
    {
        /* make sure all the accepting is done before running off with the channels */
        if (!ctrl->bIsFastStartFinished)
            return;

        /* Destination of the call sends Ack nodes */
        nodeCount = ctrl->fastStartNodesAcptSendCount;
        startNode = 0;
        nodes = ctrl->fastStartNodesAcptSend;
    }
    else if(ctrl->fastStartState == fssRej)
    {
        /* reject the fast start - was approved with no channels */
        static RvInt16 connRefusePath[] =
            { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu)
              _q931(h323_message_body) _anyField _q931(fastConnectRefused) LAST_TOKEN };
        pvtBuildByFieldIds(hVal, message, connRefusePath, 0, NULL);
        return;
    }
    else
    {
        return;
    }

    if (nodeCount > 0)
    {
        /* We have fast start nodes - go through them and add them in */
        int i;
        RvPvtNodeId tmpNodeId;
        int msgType;

        msgType = pvtGetChildTagByPath(hVal, message, "message", 1);

        /* first, we'll check if we already have a fast start field in the message */
        __pvtGetByFieldIds(tmpNodeId, hVal, message,
            {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu)
            _q931(h323_message_body) _anyField _q931(fastStart) LAST_TOKEN},NULL,NULL,NULL);
        if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
        {
            /* we have a FS element in the message already. this may be because the call was transferred,
            or it may be because the user used setParam() to set a faststart element. we can do one of
            two things: either delete the node and put our own FS content, or we could keep the existing
            FS and return. adding to the current FS message is out of the question. */
#if 1
            /* we assume that the user will not use both setParam() and FS functions, or that this is a
            transfer, and we better update our information */
            pvtDelete(hVal, tmpNodeId);
#else
            /* we assume that we should keep whatever was set, and leave things up to the user */
            return;
#endif
        }

            /* Build the fast start SEQUENCE OF node */
        __pvtBuildByFieldIds(tmpNodeId, hVal, message,
            { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu)
              _q931(h323_message_body) _anyField _q931(fastStart) LAST_TOKEN}, 0, NULL);

        for (i = startNode; i < nodeCount; i++)
        {
            int iBufLen;
            RvUint8* encodeBuffer;

            getEncodeDecodeBuffer(app->encodeBufferSize, &encodeBuffer);

            /* Encode a FastStart proposal/acceptance */
            if (cmEmEncode(hVal, nodes[i], encodeBuffer, app->encodeBufferSize, &iBufLen) >= 0)
            {
                /* That's it, now we can add the encoded OLC message to the SETUP message */
                pvtAdd(hVal, tmpNodeId, -800, iBufLen, (char*)encodeBuffer, NULL);

                RvH323CmPrintMessage(&app->logFastStart,
                    (ctrl->fastStartState == fssPrplSend) ? "Suggested faststart channel encoded:":
                        "Accepted faststart channel encoded:",
                    hVal, nodes[i], encodeBuffer, iBufLen, RV_FALSE);
            }
            else
            {
                /* bad node-id - delete it */
                int j;
                iBufLen = -1;

                RvH323CmPrintMessage(&app->logFastStart,
                    (ctrl->fastStartState == fssPrplSend) ? "Suggested faststart channel failed encoding:":
                        "Accepted faststart channel failed encoding:",
                    hVal, nodes[i], encodeBuffer, iBufLen, RV_FALSE);

                pvtDelete(hVal, nodes[i]);

                for (j=i; j<(nodeCount-1); j++)
                {
                    nodes[j] = nodes[j+1];
                }
                nodes[j] = RV_PVT_INVALID_NODEID;
                i--;
                nodeCount--;

                if (ctrl->fastStartState == fssPrplSend)
                {
                    /* Sends proposal node, nodes = ctrl->fastStartNodesPrplSend */
                    ctrl->fastStartNodesPrplSendCount--;
                }
                else if (ctrl->fastStartState == fssAcptSend)
                {
                    /* Sends acceptance nodes, nodes = ctrl->fastStartNodesAcptSend */
                    ctrl->fastStartNodesAcptSendCount--;
                }

                if (nodeCount == 0)
                {
                    pvtDelete(hVal, tmpNodeId);
                    if (ctrl->eExtendedFastConnectState == cmExtendedFastConnectApproved)
                        ctrl->fastStartState = fssIdle;
                    else
                        ctrl->fastStartState = fssNo;
                }
            }
        }
        if ((ctrl->eExtendedFastConnectState == cmExtendedFastConnectApproved) ||
            ((msgType == cmQ931setup) && (ctrl->eExtendedFastConnectState != cmExtendedFastConnectNo)))
        {
            if (ctrl->fastStartState == fssAcptSend)
            {
                /* mark this as an acceptance */
                cmAddExtFastConnectGenericData(hsCall, message, 0);
                /* clean the incoming FS nodes. we don't need them */
                deleteIncomingFastStartOffer(hsCall);
                /* done with transaction, move to idle state */
                ctrl->fastStartState = fssIdle;
            }
            else if (ctrl->fastStartState == fssPrplSend)
            {
                /* mark this as a proposal */
                cmAddExtFastConnectGenericData(hsCall, message, EFC_CHANNEL_PROPOSALS);

                for (i = ctrl->fastStartNodesNewPrplSendIndex; i < ctrl->fastStartNodesPrplSendCount; i++)
                {
                    RvInt32 sessionId = 0;
                    int j, k;

                    if (isNullOLC(hVal, ctrl->fastStartNodesPrplSend[i], RV_TRUE, &sessionId, NULL))
                    {
                        /* this is a null-OLC proposal, so we won't get an ACK on it */
                        H245Channel *channel = getOutChanBySID((HCONTROL)ctrl, (RvUint8)sessionId);
                        if (emaLock((EMAElement)channel))
                        {
                            channel->eState = ChannelStateReleased;
                            notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
                            notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
                            emaUnlock((EMAElement)channel);
                        }
                        channel = getInChanBySID((HCONTROL)ctrl, (RvUint8)sessionId);
                        if (emaLock((EMAElement)channel))
                        {
                            channel->eState = ChannelStateReleased;
                            notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
                            notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
                            emaUnlock((EMAElement)channel);
                        }

                        /* maybe we're canceling our own proposal? */
                        for (j=0; j<i; j++)
                        {
                            RvInt32 proposalSessionId = -1;
                            if (isNullOLC(hVal, ctrl->fastStartNodesPrplSend[j], RV_TRUE, &proposalSessionId, NULL))
                                /* This shouldn't really happen */
                                continue;

                            if (proposalSessionId == sessionId)
                            {
                                /* delete our own proposal */
                                if (app->h225CallEvent.cmH245EvExtFastConnectProposalCanceled != NULL)
                                {
                                    app->h225CallEvent.cmH245EvExtFastConnectProposalCanceled(
                                        (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall), (HCALL)hsCall,
                                        sessionId, ctrl->fastStartNodesPrplSend[j]);
                                }

                                pvtDelete(hVal, ctrl->fastStartNodesPrplSend[j]);

                                for (k=j; k<(ctrl->fastStartNodesPrplSendCount-1); k++)
                                {
                                    ctrl->fastStartNodesPrplSend[k] = ctrl->fastStartNodesPrplSend[k+1];
                                }
                                ctrl->fastStartNodesPrplSend[k] = RV_PVT_INVALID_NODEID;
                                j--;
                                i--;
                                ctrl->fastStartNodesPrplSendCount--;
                            }
                        }
                        pvtDelete(hVal, ctrl->fastStartNodesPrplSend[i]);

                        for (j=i; j<(ctrl->fastStartNodesPrplSendCount-1); j++)
                        {
                            ctrl->fastStartNodesPrplSend[j] = ctrl->fastStartNodesPrplSend[j+1];
                        }
                        ctrl->fastStartNodesPrplSend[j] = RV_PVT_INVALID_NODEID;
                        i--;
                        ctrl->fastStartNodesPrplSendCount--;
                    }
                    else
                    {
                        ctrl->bFSWaitForReply = RV_TRUE;
                    }
                }
                if (ctrl->fastStartNodesPrplSendCount == 0)
                {
                    ctrl->fastStartState = fssIdle;
                    ctrl->bFSWaitForReply = RV_FALSE;
                }
                ctrl->fastStartNodesNewPrplSendIndex = 0; /* re-initialize this
                to zero, since this proposal was already sent. if we'll send a
                new proposal, we'll set it to the start of the new proposal */
            }
        }
    }
}


/************************************************************************
 * cmCallFastStartOpenChannels
 * purpose: Set the answered information to be sent for a fast start
 *          proposal and open the channels on the destination side of the
 *          call. This function should be called after receiving
 *          cmEvCallFastStart() or cmEvCallFastStartSetup().
 *          It is automatically called on the initiator of the call.
 * input  : hsCall      - Call to use
 *          proposed    - List of proposed channels. This list is searched for
 *                        their RTP and RTCP addresses.
 *                        Each parameter in this list is a root node of an
 *                        OpenLogicalChannel message to propose in faststart
 *                        The last element in this list should be a negative value.
 *          accepted    - List of accepted channels.
 *                        Each parameter in this list is a root node of an
 *                        OpenLogicalChannel message sent to the origin of the call
 *                        by this endpoint.
 *                        The last element in this list should be a negative value.
 *          origin      - RV_TRUE if this application is the origin of the call
 *                        RV_FALSE if this application is not the origin of the call
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV
cmCallFastStartOpenChannels(
    IN HCALL    hsCall,
    IN int*     proposed,
    IN int*     accepted,
    IN RvBool   origin)
{
    H245Object *app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Control *ctrl = (H245Control *)cmiGetControl(hsCall);
    HPVT hVal;

    RvBool controlReported = RV_FALSE;
    int ret = RV_OK;
    int i;
    RvInt32 value;

    /* only used when origin */
    RvPvtNodeId rtcpAddressID[MAX_FS_SESSION_ID];
    RvPvtNodeId rtpAddressID[MAX_FS_SESSION_ID];
    int sidIndex = 0;
    RvInt8 sidMap[MAX_SESSION_ID];
    RvBool openedSid[MAX_SESSION_ID];

    RvUint8 lcnAllocationBuff[(MAX_FASTSTART_CHANNELS+1) / sizeof(char)];
    memset(lcnAllocationBuff, 0, sizeof(lcnAllocationBuff));

    if (!hsCall || !app || !ctrl) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmCallFastStartOpenChannels(hsCall=%p)", hsCall));

    if (emaLock((EMAElement)hsCall))
    {
        hVal = app->hVal;

        if ((ctrl->fastStartState==fssAcptSend) || (ctrl->fastStartState==fssRej))
        {
            emaUnlock((EMAElement)hsCall);
            H245APIExit((app->pAppObject, "cmCallFastStartOpenChannels=%d", RV_ERROR_UNKNOWN));
            return RV_ERROR_UNKNOWN;
        }
        memset(rtcpAddressID, 0xff, sizeof(rtcpAddressID));
        memset(rtpAddressID, 0xff, sizeof(rtpAddressID));
        memset(sidMap, 0xff, sizeof(sidMap));
        memset(openedSid, 0, sizeof(openedSid));

        if (ctrl->fastStartState == fssPrplRcvd)
            ctrl->fastStartState = fssAcptSend;

        if (origin)
        {
            /* Go through all proposed channels and collect RTCP addresses,
               One RTCP address per sessionID */
            for (i=0;proposed[i]>=0;i++)
            {
                int inReverseLCPNodeId,inForwardLCPNodeId;
                int inH2250ParametersNodeId;
                int sessionID = -1;
                int index;

                inReverseLCPNodeId=pvtGetChild(hVal, proposed[i], __h245(reverseLogicalChannelParameters), NULL);
                inForwardLCPNodeId=pvtGetChild(hVal, proposed[i], __h245(forwardLogicalChannelParameters), NULL);

                if (inReverseLCPNodeId >= 0)
                    inH2250ParametersNodeId=pvtGetChild2(hVal, inReverseLCPNodeId, __h245(multiplexParameters), __h245(h2250LogicalChannelParameters));
                else
                    inH2250ParametersNodeId=pvtGetChild2(hVal, inForwardLCPNodeId, __h245(multiplexParameters), __h245(h2250LogicalChannelParameters));
                pvtGetChildByFieldId(hVal, inH2250ParametersNodeId,__h245(sessionID),&sessionID, NULL);

                if (sessionID >= 0)
                {
                    index = (int)sidMap[sessionID];
                    if (index < 0)
                    {
                        if (sidIndex == MAX_FS_SESSION_ID)
                            continue;
                        index = sidIndex;
                        sidMap[sessionID] = (RvInt8)sidIndex;
                        sidIndex++;
                    }

                    /* We've got a session ID - find the remote's RTP and RTCP addresses for it */
                    if (rtpAddressID[index] < 0)
                    {
                        RvPvtNodeId rtpNodeID = pvtGetChild(hVal, inH2250ParametersNodeId, __h245(mediaChannel), NULL);
                        if (RV_PVT_NODEID_IS_VALID(rtpNodeID))
                        {
                            rtpAddressID[index] = pvtAddRoot(hVal, app->hAddrSynH245, 0, NULL);
                            pvtSetTree(hVal, rtpAddressID[index], hVal, rtpNodeID);
                        }
                    }
                    if (rtcpAddressID[index] < 0)
                    {
                        RvPvtNodeId rtcpNodeID = pvtGetChild(hVal, inH2250ParametersNodeId, __h245(mediaControlChannel), NULL);
                        if (RV_PVT_NODEID_IS_VALID(rtcpNodeID))
                        {
                            rtcpAddressID[index] = pvtAddRoot(hVal,app->hAddrSynH245, 0, NULL);
                            pvtSetTree(hVal, rtcpAddressID[index], hVal, rtcpNodeID);
                        }
                    }
                } /* if (sessionID > 0) */
            } /* for (i=0;proposed[i]>=0;i++) */
        } /* if (origin) */

        /* Go through the list of accepted channels and open them locally */

        for (i=0;accepted[i]>=0;i++)
        {
            int inReverseLCPNodeId, inForwardLCPNodeId;
            int inLCPNodeId;
            RvBool nullDataType;
            H245Channel* channel;
            confDataType type = (confDataType)0;
            int index;

            /* Allocate the channel */
            channel = allocateChannel((HCONTROL)ctrl);
            if (!channel)
            {
                /* Don't continue... */
                ret = RV_ERROR_OUTOFRESOURCES;
                break;
            }
            channel->data.h225Params.fastStartChannelIndex = i;

            if (!controlReported)
            {
                /* Notify application, that the call is in cmControlStateFastStart */
                cmiReportControl((HCONTROL)ctrl, cmControlStateFastStart, (cmControlStateMode)0);

                /* We only do that once, and only after we are sure we have a channel that
                   we'll open */
                controlReported = RV_TRUE;
            }

            /* check the channel direction: only proposal for outgoing channels have reverse parameters*/
            inReverseLCPNodeId = pvtGetChild(hVal, accepted[i], __h245(reverseLogicalChannelParameters),NULL);
            inForwardLCPNodeId = pvtGetChild(hVal, accepted[i], __h245(forwardLogicalChannelParameters),NULL);

            nullDataType = (pvtGetChild2(hVal, inForwardLCPNodeId, __h245(dataType),__h245(nullData))>=0);

            if (nullDataType) /*Incoming channel*/
                channel->bOrigin = !origin;
            else
                channel->bOrigin = origin;

            /* Check if this channel is a duplex channel */
            if (inReverseLCPNodeId>=0 &&
                inForwardLCPNodeId>=0 && !nullDataType)
                channel->bIsDuplex=RV_TRUE;

            /* Get the logical channel number */
            value = 0;
            pvtGetChildByFieldId(hVal, accepted[i], __h245(forwardLogicalChannelNumber), &value, NULL);
            channel->myLogicalChannelNum = (RvUint16)value;

            if (channel->bIsDuplex)
                channel->reverseLogicalChannelNum = channel->myLogicalChannelNum;
            setBitTrue(lcnAllocationBuff, channel->myLogicalChannelNum/MAX_LCN_S);

            if (inForwardLCPNodeId >= 0)
                pvtGetChildByFieldId(hVal,inForwardLCPNodeId,__h245(portNumber),&channel->remotePortNumber,NULL);

            inLCPNodeId=(inReverseLCPNodeId>=0)?inReverseLCPNodeId:inForwardLCPNodeId;

            if (inLCPNodeId >= 0)
            {
                /* Now that we're in the right parameters for this channel -
                   let's get down to the business of opening this channel for the application */
                RvPvtNodeId inDataTypeId, tmpNodeId, rtpNode=-1, rtcpNode=-1;
                RvPvtNodeId lcParamsNodeId;

                /* Get the dataType of this channel and store it */
                inDataTypeId=pvtGetChild(hVal, inLCPNodeId,__h245(dataType),NULL);
                channel->dataTypeID=pvtAddRoot(hVal,app->h245DataType,0,NULL);
                pvtSetTree(hVal,channel->dataTypeID,hVal, inDataTypeId);

                /* Get the session ID of this channel */
                value = 0;
                __pvtGetNodeIdByFieldIds(lcParamsNodeId, hVal, inLCPNodeId,
                    {_h245(multiplexParameters) _h245(h2250LogicalChannelParameters)  LAST_TOKEN});
                pvtGetChildByFieldId(hVal, lcParamsNodeId, __h245(sessionID), &value, NULL);
                channel->data.h225Params.sessionId = (RvUint8)value;

                /* Get the dynamic payload number of this channel */
                tmpNodeId = pvtGetChildByFieldId(hVal, lcParamsNodeId, __h245(dynamicRTPPayloadType), &value, NULL);
                if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
                    channel->data.h225Params.dynamicPayloadNumber = (RvUint8)value;

                /* Get the local RTP and RTCP addresses for this channel */
                if (origin && RV_PVT_NODEID_IS_VALID(tmpNodeId))
                {
                    index = (int)sidMap[channel->data.h225Params.sessionId];
                    if (index >= 0)
                    {
                        /* we're the initiator side, get them from the proposed channels */
                        rtpNode = rtpAddressID[index];
                        rtcpNode = rtcpAddressID[index];
                    }
                    else
                    {
                        /* we don't really know this channel, and we didn't propose it.
                           we should ignore it */
                        emaLock((EMAElement)channel);
                        pvtDelete(hVal, channel->dataTypeID);
                        emaDelete((EMAElement)channel);
                        emaUnlock((EMAElement)channel);
                        continue;

                    }
                }
                if (!origin)
                {
                    /* we're the answering side, get them from the accepted channel */
                    rtcpNode = pvtGetChild(hVal, lcParamsNodeId, __h245(mediaControlChannel), NULL);
                    rtpNode = pvtGetChild(hVal, lcParamsNodeId, __h245(mediaChannel), NULL);
                }

                /* insert the local addresses into the channel object */
                if (RV_PVT_NODEID_IS_VALID(rtpNode))
                {
                    if (channel->data.h225Params.recvRtpAddressID < 0)
                    {
                        channel->data.h225Params.recvRtpAddressID =
                            pvtAddRoot(hVal, app->hAddrSynH245, 0, NULL);
                    }
                    pvtSetTree(hVal, channel->data.h225Params.recvRtpAddressID,
                        hVal, rtpNode);
                }
                if (RV_PVT_NODEID_IS_VALID(rtcpNode))
                {
                    if (channel->data.h225Params.recvRtcpAddressID < 0)
                    {
                        channel->data.h225Params.recvRtcpAddressID =
                            pvtAddRoot(hVal, app->hAddrSynH245, 0, NULL);
                    }
                    pvtSetTree(hVal, channel->data.h225Params.recvRtcpAddressID,
                        hVal, rtcpNode);
                }

                /* Find a partner for this channel if there is one */
                if (!channel->bIsDuplex)
                {
                    if (channel->bOrigin)
                    {
                        channel->data.h225Params.pPartner =
                            getInChanBySID((HCONTROL)ctrl, channel->data.h225Params.sessionId);
                    }
                    else
                    {
                        channel->data.h225Params.pPartner =
                            getOutChanBySID((HCONTROL)ctrl, channel->data.h225Params.sessionId);
                    }

                    if (channel->data.h225Params.pPartner != NULL)
                    {
                        /* Link it the other way around as well. No need to lock the second
                           channel, since the channels are locked by their call already */
                        channel->data.h225Params.pPartner->data.h225Params.pPartner = channel;
                    }
                }

                /* Lock this channel - we're about to make callbacks in this place */
                if (emaLock((EMAElement)channel))
                {
                    if (app->cmMySessionEvent.cmEvCallNewChannel)
                    {
                        HAPPCALL haCall = emaGetApplicationHandle((EMAElement)hsCall);
                        HAPPCHAN haChan = NULL;
                        int nesting;

                        H245CBEnter((app->pAppObject, "cmEvCallNewChannel:%s: haCall=%p, hsCall=%p. hsChan=%p",
                            (channel->bOrigin ? "OUT" : "IN"), haCall, hsCall, channel));

                        nesting = emaPrepareForCallback((EMAElement)channel);
                        app->cmMySessionEvent.cmEvCallNewChannel(haCall, hsCall, (HCHAN)channel, &haChan);
                        emaSetApplicationHandle((EMAElement)channel, (void*)haChan);
                        emaReturnFromCallback((EMAElement)channel, nesting);

                        H245CBExit((app->pAppObject, "cmEvCallNewChannel:%s: haChan=%p.",
                            (channel->bOrigin ? "OUT" : "IN"), haChan));
                    }

                    if ((!emaWasDeleted((EMAElement)channel)) && (ctrl->eState != ctrlEndSession))
                    {
                        /* Update the MIB with this new channel */
                        if (app->mibEvent.h341AddNewLogicalChannel)
                            app->mibEvent.h341AddNewLogicalChannel(app->mibHandle, (HCHAN)channel);

                        /* Call channel parameters callback if needed */
                        cmcCallChannelParametersCallback((HH245)app, (HCHAN)channel, channel->dataTypeID, &type);
                    }

                    if ((!emaWasDeleted((EMAElement)channel)) && (ctrl->eState != ctrlEndSession))
                    {
                        /* Now call the dataType callback if we still have this channel */
                        cmcCallDataTypeHandleCallback((HH245)app, (HCHAN)channel, channel->dataTypeID, type);
                    }

                    if ((!emaWasDeleted((EMAElement)channel)) && (ctrl->eState != ctrlEndSession))
                    {
                        /* Find the nodeId of the logical channel parameters  - we want to notify
                        the application about the remote's RTP and RTCP addresses */
                        int msgNodeId;

                        if (origin)
                        {
                            /* This means we're the side that received the CONNECT. We already have
                            the parameters node id from the accepted channels in inLCPNodeId. */
                            msgNodeId = accepted[i];
                        }
                        else
                        {
                            /* This means we're the side that should send the CONNECT - we want the
                            proposed nodeId (the caller to this function has already aligned the proposed
                            and accepted arrays to the same order) */
                            msgNodeId = proposed[i];
                        }
                        /* Notify the application about the remote's RTP and RTCP addresses
                           (this will also set them in the remote address fields) */
                        h22xFunctions[ctrl->eParamsType].cmcCallAddressCallbacks(app, channel, msgNodeId);
                    } /* if (!emaWasDeleted((EMAElement)channel)) */

                    if ((!emaWasDeleted((EMAElement)channel)) && (ctrl->eState != ctrlEndSession))
                    {
                        /* Now that we're done - notify the state of this channel is connected */
                        channel->eState = ChannelStateEstablished;
                        notifyChannelState(channel,cmChannelStateConnected,cmChannelStateModeOn);
                    }

                    /* mark this session ID as open */
                    openedSid[channel->data.h225Params.sessionId] = RV_TRUE;

                    /* We're done with all the callbacks for this channel - unlock it */
                    emaUnlock((EMAElement)channel);
                } /* if (emaLock((EMAElement)channel)) */

                /* If someone deleted this call, we should get out of this for loop -
                   Since the Idle state could be delayed, we check if the control was ended */
                if ((emaWasDeleted((EMAElement)hsCall)) || (ctrl->eState == ctrlEndSession))
                    break;
            } /* if (inLCPNodeId >= 0) */
        } /* for (i=0;accepted[i]>=0;i++) */

        /*If the number of fast start proposals was less then MAX_FASTSTART_CHANNELS then at least
        one range will be available - see the beginning of the file */
        ctrl->logicalChannelOut = (RvUint16)(get1st0BitNumber(lcnAllocationBuff, 0, MAX_FASTSTART_CHANNELS)*MAX_LCN_S);

        /* No need for the addresses anymore - delete them */
        for (i = 0; i < MAX_FS_SESSION_ID; i++)
        {
            pvtDelete(hVal, rtcpAddressID[i]);
            pvtDelete(hVal, rtpAddressID[i]);
        }

        /* if we are using EFC, delete only the proposals that were answered */
        if (origin &&
            (ctrl->eExtendedFastConnectState == cmExtendedFastConnectApproved))
        {
            for (i = 0; i < ctrl->fastStartNodesPrplSendCount; i++)
            {
                /* get this proposal's session ID */
                RvInt32 sessionId;
                RvPvtNodeId fwdNodeId = pvtGetChild(hVal, ctrl->fastStartNodesPrplSend[i],__h245(forwardLogicalChannelParameters),NULL);
                RvPvtNodeId rvsNodeId = pvtGetChild(hVal, ctrl->fastStartNodesPrplSend[i],__h245(reverseLogicalChannelParameters),NULL);
                RvPvtNodeId tmpNode = RV_PVT_NODEID_IS_VALID(rvsNodeId) ? rvsNodeId : fwdNodeId;
                tmpNode = pvtGetChild2(hVal, tmpNode, __h245(multiplexParameters), __h245(h2250LogicalChannelParameters));
                pvtGetChildByFieldId(hVal, tmpNode, __h245(sessionID), &sessionId, NULL);

                /* was this SID already opened? */
                if (openedSid[sessionId])
                {
                    /* yes, it was. delete it. */
                    int j;
                    pvtDelete(hVal, ctrl->fastStartNodesPrplSend[i]);
                    ctrl->fastStartNodesPrplSendCount--;
                    for (j = i; j < ctrl->fastStartNodesPrplSendCount; j++)
                    {
                        ctrl->fastStartNodesPrplSend[j] = ctrl->fastStartNodesPrplSend[j+1];
                    }
                    i--;
                }
            }
            for (i=0;i<ctrl->fastStartNodesAcptRcvdCount;i++)
            {
                pvtDelete(app->hVal,ctrl->fastStartNodesAcptRcvd[i]);
                ctrl->fastStartNodesAcptRcvd[i]=RV_ERROR_UNKNOWN;
            }
            ctrl->fastStartNodesAcptRcvdCount=0;
        }

        emaUnlock((EMAElement)hsCall);
    } /* if (emaLock((EMAElement)hsCall)) */

    H245APIExit((app->pAppObject, "cmCallFastStartOpenChannels=%d", ret));
    return ret;
}


int  fastStartInit(IN HH245 hApp, IN int maxCalls, IN int proposed, IN int accepted)
{
    H245Object *app = (H245Object*)hApp;
    int mult = 1;

    RvLogSourceConstruct(app->logMgr, &app->logFastStart, "FASTSTART", "Fast Start Messages");

    if (app->bSupportEFC) mult = 2;

    /* Allocate the buffer of fast start nodes */
    /* add one to values, last in array must be -1 */
    RvMemoryAlloc(NULL, (RvSize_t)maxCalls*mult*(proposed+accepted+2)*sizeof(int), app->logMgr, (void**)&app->fastStartBuff);
    RvMemoryAlloc(NULL, (RvSize_t)maxCalls*(accepted+1)*sizeof(RvUint8), app->logMgr, (void**)&app->fastStartBuff2);

    /* Set the values in the stack's instance */
    app->maxFsProposed = proposed;
    app->maxFsAccepted = accepted;

    return ((app->fastStartBuff != NULL) && (app->fastStartBuff2 != NULL));
}


void fastStartEnd(IN HH245 hApp)
{
    H245Object *app = (H245Object*)hApp;

    if (app->fastStartBuff != NULL)
        RvMemoryFree(app->fastStartBuff, app->logMgr);
    if (app->fastStartBuff2 != NULL)
        RvMemoryFree(app->fastStartBuff2, app->logMgr);
    RvLogSourceDestruct(&app->logFastStart);
}


void fastStartCallInit(IN HCALL hsCall)
{
    H245Object* app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);
    int callNumber;
    int mult = 1;

    /* Set initial fast start values of the call */
    ctrl->fastStartState=fssNo;
    ctrl->fastStartNodesPrplSendCount=0;
    ctrl->fastStartNodesPrplRcvdCount=0;
    ctrl->fastStartNodesAcptSendCount=0;
    ctrl->fastStartNodesAcptRcvdCount=0;
    ctrl->fastStartNodesNewPrplSendIndex=0;
    ctrl->logicalChannelOut=0;
    ctrl->eExtendedFastConnectState=cmExtendedFastConnectNo;
    ctrl->eRemExtendedFastConnectState=cmExtendedFastConnectUndetermined;

    /* Find out the call's index: We'll use that to access it's fast start channels */
    callNumber = emaGetIndex((EMAElement)hsCall);

    if (app->bSupportEFC) mult = 2;

    /* Set the pointers to the right places inside the fast start buffer - if EFC is not supported,
       we will have just one proposal and one acceptance at any time. */
    ctrl->fastStartNodesPrplSend = app->fastStartBuff + (callNumber * mult * (app->maxFsProposed + app->maxFsAccepted + 2));

    if (app->bSupportEFC)
        ctrl->fastStartNodesPrplRcvd = ctrl->fastStartNodesPrplSend + app->maxFsProposed + 1;
    else
        ctrl->fastStartNodesPrplRcvd = ctrl->fastStartNodesPrplSend;

    ctrl->fastStartNodesAcptSend = ctrl->fastStartNodesPrplRcvd + app->maxFsProposed + 1;

    if (app->bSupportEFC)
        ctrl->fastStartNodesAcptRcvd = ctrl->fastStartNodesAcptSend + app->maxFsAccepted + 1;
    else
        ctrl->fastStartNodesAcptRcvd = ctrl->fastStartNodesAcptSend;

    ctrl->fastStartIndexes = app->fastStartBuff2 + (callNumber * (app->maxFsAccepted + 1));

    /* Clear the nodes to -1 */
    memset(ctrl->fastStartNodesPrplSend, 0xff, (RvSize_t)sizeof(int)*mult*(app->maxFsProposed + app->maxFsAccepted + 2));
    memset(ctrl->fastStartIndexes, 0xff, (RvSize_t)sizeof(RvUint8)*(app->maxFsAccepted + 1));
}


RvStatus buildExtFastConnectGenericData(IN HCALL hsCall, OUT RvPvtNodeId *nodeId)
{
    HAPP         hApp = (HAPP)emaGetInstance((EMAElement)hsCall);
    H245Object  *app  = (H245Object*)cmiGetH245Handle(hApp);
    H245Control *ctrl = (H245Control *)cmiGetControl(hsCall);
    HPVT         hVal = app->hVal;

    char         oid[128];
    int          oidSize;
    RvPvtNodeId tmpNodeId, uuNodeId;

    if (ctrl->eExtendedFastConnectState != cmExtendedFastConnectApproved)
        return RV_ERROR_UNKNOWN;

    *nodeId = pvtAddRoot(hVal, cmGetSynTreeByRootName(hApp, (char *)"q931"), 0, NULL);
    oidSize = sizeof(oid);

    pvtAdd(hVal, *nodeId, __q931(protocolDiscriminator), 8, NULL, NULL);
    __pvtBuildByFieldIds(tmpNodeId, hVal, *nodeId, { _q931(message) _q931(facility)
        _q931(userUser) _q931(protocolDiscriminator) LAST_TOKEN }, 5, NULL);

    oidSize = utlEncodeOID(oidSize, oid, "itu-t recommendation h 2250 0 5");

    uuNodeId = pvtAddBranch2(hVal, pvtParent(hVal, tmpNodeId), __q931(h323_UserInformation), __q931(h323_uu_pdu));
    __pvtBuildByFieldIds(tmpNodeId, hVal, uuNodeId, { _q931(h323_message_body) _q931(facility)
        _q931(protocolIdentifier) LAST_TOKEN }, oidSize, oid);

    if (RV_PVT_NODEID_IS_VALID(*nodeId))
      return RV_OK;
    else
      return RV_ERROR_UNKNOWN;
}

/******************************************************************************
 * cmCallExtFastConnectCloseAllChannels
 * ----------------------------------------------------------------------------
 * General: Close all the channels on a call.
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsCall      - Stack handle for the call
 * Output: None.
 *****************************************************************************/
RVAPI int RVCALLCONV
cmCallExtFastConnectCloseAllChannels(
    IN HCALL    hsCall)
{
    HAPP         hApp = (HAPP)emaGetInstance((EMAElement)hsCall);
    H245Object  *app  = (H245Object*)cmiGetH245Handle(hApp);
    HPVT         hVal = app->hVal;
    H245Control *ctrl = (H245Control *)cmiGetControl(hsCall);
    int          status;
    RvPvtNodeId nodeId=0;

    status=buildExtFastConnectGenericData(hsCall, &nodeId);
    if (status == RV_OK)
    {
        cmAddExtFastConnectGenericData(hsCall, nodeId, EFC_CLOSE_ALL_MEDIA_CHANNELS);

        /* send an undefined facility message */
        status = cmCallFacility(hsCall, nodeId);
        pvtDelete(hVal, nodeId);

        if (status >= 0)
        {
            /* now drop all the channels */
            H245Channel * channel = (H245Channel *)ctrl->hFirstChannel;
            while (channel != NULL)
            {
                emaLock((EMAElement)channel);
                channel->eState = ChannelStateReleased;
                notifyChannelState(channel, cmChannelStateDisconnected, cmChannelStateModeDisconnectedLocal);
                notifyChannelState(channel, cmChannelStateIdle, cmChannelStateModeOff);
                emaUnlock((EMAElement)channel);
                channel = channel->pNext;
            }
        }
    }

    return status;
}


/******************************************************************************
 * cmCallExtFastConnectNewChannelProposals
 * ----------------------------------------------------------------------------
 * General: Request that an endpoint propose new media channels on a call.
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsCall      - Stack handle for the call
 * Output: None.
 *****************************************************************************/
RVAPI int RVCALLCONV
cmCallExtFastConnectRequestNewChannelProposals(
    IN HCALL    hsCall)
{
    HAPP         hApp = (HAPP)emaGetInstance((EMAElement)hsCall);
    H245Object  *app  = (H245Object*)cmiGetH245Handle(hApp);
    HPVT         hVal = app->hVal;
    int          status;
    RvPvtNodeId nodeId=0;

    status=buildExtFastConnectGenericData(hsCall, &nodeId);
    if (status == RV_OK)
    {
        cmAddExtFastConnectGenericData(hsCall, nodeId, EFC_RERUEST_NEW_CHANNEL_PROPOSALS);

        /* send an undefined facility message */
        status = cmCallFacility(hsCall, nodeId);
        pvtDelete(hVal, nodeId);
    }

    return status;
}


#ifdef __cplusplus
}
#endif

#else
  int faststartdummy= 1;
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */



