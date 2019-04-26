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

#ifdef __cplusplus
extern "C" {
#endif

#include "cmH245GeneralDefs.h"
#if (RV_H245_SUPPORT_H223_PARAMS == 1)

#include "rvstdio.h"
#include "cmH223Params.h"
#include "cmiParams.h"
#include "cmictrl.h"
#include "cmH245Object.h"
#include "cmChanGet.h"
#include "cmctrl.h"


extern H22XFunctions h22xFunctions[];

int notifyChannelState(H245Channel*channel,cmChannelState_e state, cmChannelStateMode_e stateMode);

static RvStatus h223ParamsCheckConflicts(
    IN  H245Channel     *pChannel,
    IN  RvBool          isOutGoingChannel,
    OUT H245Channel     **conflictingChannel,
    OUT H245Channel     **goodChannel);
static RvStatus h223ParamsHandleConflicts(
    IN H245Channel    *pChannel,
    OUT RvBool        *channelRejected);

/******************************************************************************
 * H223getVideoChannelRole
 * ----------------------------------------------------------------------------
 * General: calls the cmEvChannelSetRole event.
 *
 * Return Value: 0 for the primary video channel, 1 for the secondary video channel,
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  pChannel      - The channel object.
 *         app      -      The H245 object.
 * Output: none.
 *****************************************************************************/
RvUint16 H223getVideoChannelRole(IN H245Channel   *pChannel,
                                 IN RvPvtNodeId   dataTypeNode,
                                 IN H245Object    *app)
{
    RvUint16 role = 0;
    int nesting;

    if (emaLock((EMAElement)pChannel))
    {
        if (app->cmMyChannelEvent.cmEvChannelSetRole != NULL)
        {
            H245CBEnter((app->pAppObject, "cmEvChannelSetRole(haChan=%p,hsChan=%p)",
                        (HAPPCHAN)emaGetApplicationHandle((EMAElement)pChannel),
                        (HCHAN)pChannel));
            nesting = emaPrepareForCallback((EMAElement)pChannel);
            app->cmMyChannelEvent.cmEvChannelSetRole(
                                (HAPPCHAN)emaGetApplicationHandle((EMAElement)pChannel),
                                (HCHAN)pChannel,
                                dataTypeNode,
                                &role);
            emaReturnFromCallback((EMAElement)pChannel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelSetRole. Role: %d", role));
        }
        emaUnlock((EMAElement)pChannel);
    }

    return role;
}


/******************************************************************************
 * H223notifyMasterSlaveConflict
 * ----------------------------------------------------------------------------
 * General: calls the cmEvChannelMasterSlaveConflict event.
 *
 * Return Value: the return value of cmEvChannelMasterSlaveConflict event
 *                  (RV_OK if the application wants the stack to ignore
 *                  the conflicts. Other values if the application wants the
 *                  stack to handle the conflict.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  pChannel      - The conflicting channel object that should be handled.
 *                         (incoming channel in the master, outgoing channel in
 *                          the slave).
 *         goodChannel   - The channel object that remains (incoming channel in
 *                         the master, outgoing channel in the slave).
 *         app      -      The H245 object.
 *         conflictType  - The type of the conflict.
 * Output: none.
 *****************************************************************************/
static RvStatus H223notifyMasterSlaveConflict(
    IN H245Channel   *pChannel,
    IN H245Channel   **goodChannel,
    IN H245Object    *app,
    IN cmH245ChannelConflictType conflictType,
    IN RvPvtNodeId    message)
{
    int nesting;
    RvStatus status = RV_ERROR_UNKNOWN;

    H245CBEnter((app->pAppObject, "cmEvChannelMasterSlaveConflict(haChan=%p,hsChan=%p, conflictType=%d)",
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)pChannel),
                (HCHAN)pChannel,
                pChannel->data.h223Params.conflictType));

    if (emaLock((EMAElement)pChannel))
    {

        nesting = emaPrepareForCallback((EMAElement)pChannel);
        status = app->cmMyChannelEvent.cmEvChannelMasterSlaveConflict(
        (HAPPCHAN)emaGetApplicationHandle((EMAElement)pChannel),
        (HCHAN)pChannel,
        conflictType,
        (HCHAN*)goodChannel,
        1,
        message);
        emaReturnFromCallback((EMAElement)pChannel, nesting);
        emaUnlock((EMAElement)pChannel);

    }
    H245CBExit((app->pAppObject, "cmEvChannelMasterSlaveConflict=%d", status));
    return status;
}


/******************************************************************************
 * cmChannelH223MaxSkew
 * ----------------------------------------------------------------------------
 * General: Build and send h223SkewIndication message.
 *          Note that cmEvChannelMaxSkew callback is used to notify the
 *          application that h223SkewIndication message was received.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsChan1      - The first channel to indicate skew for.
 *         hsChan2      - The second channel to indicate skew for.
 *         skew         - The skew number in milliseconds.
 * Output: none.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmChannelH223MaxSkew(
         IN     HCHAN           hsChan1,
         IN     HCHAN           hsChan2,
         IN     RvUint32        skew)
{
    HH245        hApp=(HH245)emaGetUserData((EMAElement)hsChan1);
    H245Channel  *channel1=(H245Channel*)hsChan1;
    H245Channel  *channel2=(H245Channel*)hsChan2;
    RvPvtNodeId  message, nodeId;
    RvStatus     res = RV_ERROR_UNKNOWN;
    H245Object   *app=(H245Object*)hApp;

    if (!hApp)   return RV_ERROR_NULLPTR;

    H245APIEnter((app->pAppObject, "cmChannelH223MaxSkew: hsChan1=%p, hsChan2=%p, skew=%d.", hsChan1, hsChan2, skew));
    if (emaLock((EMAElement)hsChan1))
    {
        if (emaLock((EMAElement)hsChan2))
        {
            HPVT hVal = app->hVal;

            /* The video channel (channel1) could be bi-directional and not only outgoing
               uni-directional. */
            if ((!channel1->bOrigin && !channel1->bIsDuplex) ||
                !channel2->bOrigin)
            {
                emaUnlock((EMAElement)hsChan2);
                emaUnlock((EMAElement)hsChan1);
                H245APIExit((app->pAppObject, "cmChannelH223MaxSkew: [Invalid Circumstances]."));
                return RV_ERROR_UNKNOWN;
            }

            message = pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
            nodeId = pvtAddBranch2(hVal,message, __h245(indication),__h245(h223SkewIndication));
            pvtAdd(hVal,nodeId,__h245(logicalChannelNumber1),channel1->myLogicalChannelNum,NULL,NULL);
            pvtAdd(hVal,nodeId,__h245(logicalChannelNumber2),channel2->myLogicalChannelNum,NULL,NULL);
            pvtAdd(hVal,nodeId,__h245(skew),(RvInt32)skew,NULL,NULL);

            res = sendMessageH245(channel1->hCtrl, message, RV_TRUE, NULL);
            emaUnlock((EMAElement)hsChan2);
        }
        emaUnlock((EMAElement)hsChan1);
    }

   H245APIExit((app->pAppObject, "cmChannelH223MaxSkew=%d", res));
   return res;
}


/************************************************************************
 * h223MultiplexReconfiguration
 * purpose: Handle an incoming h223MultiplexReconfiguration message.
 * input  : ctrl    - Control object
 *          message - h223MultiplexReconfiguration message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int h223MultiplexReconfiguration(IN H245Control* ctrl, IN int message)
{
    HH245               hApp=cmiGetH245Handle((HAPP)emaGetInstance(
                            (EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT                hVal = ((H245Object *)hApp)->hVal;
    H245Object          *app = (H245Object*)hApp;
    RvPvtNodeId         choiceId;
    RvPstFieldId        fieldId;
    RvStatus            status = RV_ERROR_UNINITIALIZED;

    emaMark((EMAElement)cmiGetByControl((HCONTROL)ctrl));

    /* Find out if the message contains the changeMode choice or AnnexADoubleFlag choice */
    __pvtGetByFieldIds(choiceId, hVal, message, {_h245(h223ModeChange) _anyField LAST_TOKEN}, &fieldId, NULL, NULL);
    if (choiceId < 0)
    {
        __pvtGetByFieldIds(choiceId, hVal, message, {_h245(h223AnnexADoubleFlag) _anyField LAST_TOKEN}, &fieldId, NULL, NULL);
        if (choiceId < 0)
        {
            status = RV_ERROR_BADPARAM;
        }
        else
        {
            /* The AnnexADoubleFlag exists */
            if (app->evHandlers.pfnH223MultiplexReconfigurationEv != NULL)
            {
                app->evHandlers.pfnH223MultiplexReconfigurationEv(
                                                   cmiGetByControl((HCONTROL)ctrl),
                                                   fieldId,
                                                   RV_FALSE);
            }
        }
    }
    else
    {
        /* The ModeChange exists */
        if (app->evHandlers.pfnH223MultiplexReconfigurationEv != NULL)
        {
            app->evHandlers.pfnH223MultiplexReconfigurationEv(
                                               cmiGetByControl((HCONTROL)ctrl),
                                               fieldId,
                                               RV_TRUE);
        }
    }


    emaRelease((EMAElement)cmiGetByControl((HCONTROL)ctrl));
    return status;
}

/************************************************************************
 * maxH223MUXPDUsize
 * purpose: Handle an incoming maxH223MUXPDUsize message.
 * input  : ctrl    - Control object
 *          message - maxH223MUXPDUsize message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int maxH223MUXPDUsize(IN H245Control* ctrl, IN int message)
{
    HH245               hApp=cmiGetH245Handle((HAPP)emaGetInstance(
                            (EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT                hVal = ((H245Object *)hApp)->hVal;
    H245Object          *app = (H245Object*)hApp;
    RvInt32             value = 0;
    RvStatus            status = RV_ERROR_UNINITIALIZED;

    emaMark((EMAElement)cmiGetByControl((HCONTROL)ctrl));

    /* Find out what is the max MUX - PDU size */
    pvtGet(hVal, message, NULL, NULL, &value, NULL);
    if (value < 0)
    {
        status = RV_ERROR_BADPARAM;
    }
    else
    {
        if (app->evHandlers.pfnMaxH223MUXPDUsize != NULL)
        {
            app->evHandlers.pfnMaxH223MUXPDUsize(
                                               cmiGetByControl((HCONTROL)ctrl),
                                               (RvUint16)value);
        }
    }

    emaRelease((EMAElement)cmiGetByControl((HCONTROL)ctrl));
    return status;
}


/* This part is not correct. Should write these functions in future versions if they
   will be needed.
RVAPI RvStatus RVCALLCONV
cmChannelSetH223MaxMuxPduSize(
         IN      HCHAN               hsChan,
         IN      RvUint              maxMuxPduSize)
{
    HH245       hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel *channel=(H245Channel*)hsChan;
    H245Object  *app=(H245Object*)hApp;

    if ((hApp == NULL) || (channel == NULL))
        return RV_ERROR_NULLPTR;

    H245APIEnter((app->pAppObject, "cmChannelSetH223MaxMuxPduSize: hsChan=%p, maxMuxPduSize=%d", hsChan, maxMuxPduSize));

    if (emaLock((EMAElement)hsChan))
    {
        channel->data.h223Params.bMaxMuxPduSize = maxMuxPduSize;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetH223MaxMuxPduSize=0"));

    return RV_OK;
}


RVAPI RvStatus RVCALLCONV
cmChannelSetH223SkewIndication(
         IN      HCHAN               hsChan,
         IN      RvBool              skewIndication)
{
    HH245       hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel *channel=(H245Channel*)hsChan;
    H245Object  *app=(H245Object*)hApp;

    if ((hApp == NULL) || (channel == NULL))
        return RV_ERROR_NULLPTR;

    H245APIEnter((app->pAppObject, "cmChannelSetH223SkewIndication: hsChan=%p, skewIndication=%d", hsChan, skewIndication));

    if (emaLock((EMAElement)hsChan))
    {
        channel->data.h223Params.bSkewIndication = skewIndication;
        emaUnlock((EMAElement)hsChan);
    }
    H245APIExit((app->pAppObject, "cmChannelSetH223SkewIndication=0"));

    return RV_OK;
}


RVAPI int RVCALLCONV
cmChannelSetH223MultiplexReconfiguration(
         IN      HCHAN               hsChan,
         IN      RvBool              multiplexReconfiguration)
{
    HH245       hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel *channel=(H245Channel*)hsChan;
    H245Object  *app=(H245Object*)hApp;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSetFlowControlToZero: hsChan=%p, MultiplexReconfiguration=%d", hsChan, multiplexReconfiguration));

    if (emaLock((EMAElement)hsChan))
    {
        channel->data.h223Params.bMultiplexReconfiguration = multiplexReconfiguration;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetFlowControlToZero: [1]."));
    return RV_TRUE;
}

*/




/************************************************************************
 * h223cmcCallAddressCallbacks
 * purpose: Call the callbacks of the h223 parameters.
 * input  : pApp            - application handle for the cm.
 *          pChannel        - Channel element to set h225 parameters for.
 *          h223ParamNodeId - node id for the h225 parameters.
 * output : none
 * return : none
 * NOTE: origin should be the origin of the MESSAGE - as the receive and transmit address association is
 * determined by the EP to send the message.
 ************************************************************************/
int h223cmcCallAddressCallbacks(
    IN H245Object*  pApp,
    IN H245Channel* pChannel,
    IN RvPvtNodeId  h223ParamNodeId)
{
    RV_UNUSED_ARG(pApp);
    RV_UNUSED_ARG(pChannel);
    RV_UNUSED_ARG(h223ParamNodeId);
    return RV_TRUE;
}


static
void establishConfirmed(H245Channel*channel, RvPvtNodeId message)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)channel);
    H245Object* app=(H245Object*)hApp;
#if (RV_3G324M_USE_MONA == RV_YES)
    H245Control *ctrl = (H245Control *)channel->hCtrl;
    H245Channel *mpcChannel = NULL;
#endif /* USE_MONA */
    int nesting;

    app->evHandlers.pfnTimerCancelEv(channel->hCtrl, &channel->pTimer);
    if (!channel->bIsDuplex)
    {
        h22xFunctions[RvH245ChannelParamsH223].cmcCallAddressCallbacks(app, channel, message);
    }

    if ((app->cmMyChannelEvent.cmEvChannelParameters != NULL) && !emaWasDeleted((EMAElement)channel))
    { /* -- Parameters */
      /* Report the same session association handles */

        /* -- same session handle */
        H245CBEnter((app->pAppObject, "cmEvChannelParameters: haCall=%p, hsCall=%p, channelName=NULL, NULL, NULL, NULL, NULL, rate=-1.",
            emaGetApplicationHandle((EMAElement)channel), channel));
        nesting = emaPrepareForCallback((EMAElement)channel);
        app->cmMyChannelEvent.cmEvChannelParameters(
            (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel,
            NULL, NULL, NULL, NULL, NULL, (RvUint32)RV_ERROR_UNKNOWN);
        emaReturnFromCallback((EMAElement)channel, nesting);
        H245CBExit((app->pAppObject, "cmEvChannelParameters."));
    }
#if (RV_3G324M_USE_MONA == RV_YES)
    /* remove MPC channels if exist */
    if (ctrl->bMPCinUse == RV_TRUE &&
        (channel->data.h223Params.eDataType == cmChannelDataTypeAudio ||
         channel->data.h223Params.eDataType == cmChannelDataTypeVideo))
    {
        /*see if we have MPC channel of the same type (Audio/Video) - if so remove it*/
        H245Channel *current = (H245Channel*)ctrl->hFirstChannel;
        while (current != NULL)
        {
            if (current->bOrigin == RV_FALSE &&
                current->data.h223Params.bIsMPCChannel == RV_TRUE)
            {
                if ((channel->data.h223Params.eDataType == cmChannelDataTypeAudio) &&
                    (current->myLogicalChannelNum == (int)RvH245MonaMPCChannelTypeAMR ||
                    current->myLogicalChannelNum == (int)RvH245MonaMPCChannelTypeAMR_WB))
                {
                    mpcChannel = current;
                    break;
                }
                else if ((channel->data.h223Params.eDataType == cmChannelDataTypeVideo) &&
                    (current->myLogicalChannelNum == (int)RvH245MonaMPCChannelTypeH263 ||
                    current->myLogicalChannelNum == (int)RvH245MonaMPCChannelTypeMPEG4 ||
                    current->myLogicalChannelNum == (int)RvH245MonaMPCChannelTypeH264))
                {
                    mpcChannel = current;
                    break;
                }
            }

            current = current->pNext;
        }

        if (mpcChannel != NULL)
        {
            /* remove the MPC h245Channel object */
            RvLogInfo(&app->log, (&app->log, "establishConfirmed: Removing MPC H245 channel, lcn = %d ", channel->myLogicalChannelNum));
            emaLock((EMAElement)mpcChannel);
            if (mpcChannel->pNext)
                mpcChannel->pNext->pPrev = mpcChannel->pPrev;
            if (mpcChannel->pPrev)
                mpcChannel->pPrev->pNext = mpcChannel->pNext;
            else
                cmH245SetChannelList((HCONTROL)ctrl, (HCHAN)mpcChannel->pNext);
            emaDelete((EMAElement)mpcChannel);
            emaUnlock((EMAElement)mpcChannel);
        }
    }
#endif /* USE_MONA */
    notifyChannelState(channel,cmChannelStateConnected, cmChannelStateModeOn);
}

/************************************************************************
 * h223ParamsStartEstablishment
 * purpose: Start establishing a channel. This function adds h223 parameters into
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
void h223ParamsStartEstablishment(IN HPVT        hVal,
                                  IN RvPvtNodeId olcID,
                                  IN RvPvtNodeId forwardLCP_ID,
                                  IN RvPvtNodeId reverseLCP_ID,
                                  IN RvPvtNodeId dataType,
                                  IN H245Channel *pChannel)
{
    H245Control    *ctrl = (H245Control*)pChannel->hCtrl;
    H245Object     *app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    RvPvtNodeId     h223ID;
    RvPvtNodeId     alID;
    RvPvtNodeId     tmpID;
    RvStatus        status;
    H245Channel*    conflictingChannel, *goodChannel;
    RvPstFieldId    fieldId;

    /* - H223 parameters start here */
    __pvtBuildByFieldIds(h223ID, hVal, forwardLCP_ID, {_h245(multiplexParameters) _h245(h223LogicalChannelParameters) LAST_TOKEN}, 0, NULL);

    alID = pvtAdd(hVal, h223ID, __h245(adaptationLayerType), 0, NULL, NULL);

    /* AL parameters */
    switch (pChannel->data.h223Params.adaptationLayerType)
    {
        case cmH223ALLevel1:
        {
            if (pChannel->data.h223Params.adaptationLayerData.adaptationLayer1Params.bIsFramed == RV_TRUE)
                pvtAdd(hVal, alID, __h245(al1Framed), 0, NULL, NULL);
            else
                pvtAdd(hVal, alID, __h245(al1NotFramed), 0, NULL, NULL);
            break;
        }

        case cmH223ALLevel2:
        {
            if (pChannel->data.h223Params.adaptationLayerData.adaptationLayer2Params.sequenceNumbering == RV_TRUE)
                pvtAdd(hVal, alID, __h245(al2WithSequenceNumbers), 0, NULL, NULL);
            else
                pvtAdd(hVal, alID, __h245(al2WithoutSequenceNumbers), 0, NULL, NULL);
            break;
        }

        case cmH223ALLevel3:
        {
            __pvtBuildByFieldIds(tmpID, hVal, alID, {_h245(al3) _h245(controlFieldOctets) LAST_TOKEN},
                    (RvInt32)(pChannel->data.h223Params.adaptationLayerData.adaptationLayer3Params.controlFieldSize), NULL);
            __pvtBuildByFieldIds(tmpID, hVal, alID, {_h245(al3) _h245(sendBufferSize) LAST_TOKEN},
                    (RvInt32)(pChannel->data.h223Params.adaptationLayerData.adaptationLayer3Params.sendBufferSize) , NULL);
            break;
        }

#if (RV_H223_USE_MUX_LEVEL_3 == RV_YES)
        case cmH223ALLevel1M:
        case cmH223ALLevel3M:
        {
            cmH223ALxMConfig *alxM = &pChannel->data.h223Params.adaptationLayerData.adaptationLayerxMParams;

            if (pChannel->data.h223Params.adaptationLayerType == cmH223ALLevel1M)
            {
                alID = pvtAdd(hVal, alID, __h245(al1M), 0, NULL, NULL);

                tmpID = pvtAdd(hVal, alID, __h245(transferMode), 0, NULL, NULL);
                if (alxM->bIsFramed)
                    pvtAdd(hVal, tmpID, __h245(framed), 0, NULL, NULL);
                else
                    pvtAdd(hVal, tmpID, __h245(unframed), 0, NULL, NULL);

                pvtAdd(hVal, alID, __h245(alsduSplitting), alxM->bUseSplitting, NULL, NULL);
                tmpID = pvtAdd(hVal, alID, __h245(headerFEC), 0, NULL, NULL);
            }
            else
            {
                alID = pvtAdd(hVal, alID, __h245(al3M), 0, NULL, NULL);
                tmpID = pvtAdd(hVal, alID, __h245(headerFormat), 0, NULL, NULL);
            }

            if (alxM->bUseGolay)
                pvtAdd(hVal, tmpID, __h245(golay24_12), 0, NULL, NULL);
            else
                pvtAdd(hVal, tmpID, __h245(sebch16_7), 0, NULL, NULL);

            tmpID = pvtAdd(hVal, alID, __h245(crcLength), 0, NULL, NULL);
            switch (alxM->crcLength)
            {
            case 0: fieldId = __h245(crcNotUsed); break;
            case 4: fieldId = __h245(crc4bit); break;
            case 8: fieldId = __h245(crc8bit); break;
            case 12: fieldId = __h245(crc12bit); break;
            case 16: fieldId = __h245(crc16bit); break;
            case 20: fieldId = __h245(crc20bit); break;
            case 28: fieldId = __h245(crc28bit); break;
            case 32: fieldId = __h245(crc32bit); break;
            default:
                // TODO: Tsahi error...
                fieldId = -1;
                break;
            }
            pvtAdd(hVal, tmpID, fieldId, 0, NULL, NULL);

            if (alxM->codeRate >= 128)
            {
                /* H.223 Annex D RS used */
                pvtAdd(hVal, alID, __h245(rsCodeCorrection), alxM->codeRate - 128, NULL, NULL);
                pvtAdd(hVal, alID, __h245(rcpcCodeRate), 8, NULL, NULL);
            }
            else
            {
                /* H.223 Annex C RCPC used */
                pvtAdd(hVal, alID, __h245(rcpcCodeRate), alxM->codeRate, NULL, NULL);
            }

            tmpID = pvtAdd(hVal, alID, __h245(arqType), 0, NULL, NULL);
            switch (alxM->arqType)
            {
            case 0: fieldId = __h245(noArq); break;
            case 1: fieldId = __h245(typeIArq); break;
            case 2: fieldId = __h245(typeIIArq); break;
            default:
                // TODO: Tsahi error...
                break;
            }
            tmpID = pvtAdd(hVal, tmpID, fieldId, 0, NULL, NULL);
            if (alxM->arqType > 0)
            {
                pvtAdd(hVal, tmpID, __h245(sendBufferSize), (RvInt32)alxM->sendBufferSize, NULL, NULL);
                tmpID = pvtAdd(hVal, tmpID, __h245(numberOfRetransmissions), 0, NULL, NULL);
                if (alxM->retransmissions == 255)
                {
                    pvtAdd(hVal, tmpID, __h245(infinite), 0, NULL, NULL);
                }
                else
                {
                    pvtAdd(hVal, tmpID, __h245(finite), (RvInt32)alxM->retransmissions, NULL, NULL);
                }
            }

            pvtAdd(hVal, alID, __h245(alpduInterleaving), alxM->bUseInterleaving, NULL, NULL);

            break;
        }

        case cmH223ALLevel2M:
        {
            cmH223ALxMConfig *alxM = &pChannel->data.h223Params.adaptationLayerData.adaptationLayerxMParams;

            alID = pvtAdd(hVal, alID, __h245(al2M), 0, NULL, NULL);

            tmpID = pvtAdd(hVal, alID, __h245(headerFEC), 0, NULL, NULL);
            if (alxM->bUseGolay)
                pvtAdd(hVal, tmpID, __h245(golay24_12), 0, NULL, NULL);
            else
                pvtAdd(hVal, tmpID, __h245(sebch16_5), 0, NULL, NULL);

            pvtAdd(hVal, alID, __h245(alpduInterleaving), alxM->bUseInterleaving, NULL, NULL);

            break;
        }
#endif /* (RV_H223_USE_MUX_LEVEL_3 == RV_YES) */
        default:
            break;
    }

    /* segmentable or non-segmentable */
    pvtAdd(hVal, h223ID, __h245(segmentableFlag),
        pChannel->data.h223Params.isSegmentable, NULL, NULL);

    /*get the type  of the channel*/
    pvtGet(hVal, pvtChild(hVal, pChannel->dataTypeID), &fieldId, NULL, NULL, NULL);

    if (pChannel->bIsDuplex)
    {
        /* duplex default parameter setting.*/

        /* Add reverse pChannel's parameters */
        if ((tmpID = pvtAddBranch(hVal, reverseLCP_ID, __h245(dataType))) >= 0)
        {
            if (pChannel->data.h223Params.bIsDuplexReverseNullData == RV_FALSE)
            {
                if (pChannel->reverseDataTypeID < 0)
                {
                    /* Use the same dataType for forward and reverse parameters. */
                    pvtSetTree(hVal, tmpID, hVal, dataType);
                    pChannel->reverseDataTypeID = pChannel->dataTypeID;
                }
                else
                {
                    /* Use the dataType that the user set for the reverse parameters. */
                    pvtSetTree(hVal, tmpID, hVal, pChannel->reverseDataTypeID);
                }
            }
            else /*nullData in reverse params*/
            {
                if (pChannel->reverseDataTypeID < 0)
                    pChannel->reverseDataTypeID = pvtAddRoot(hVal, NULL, 0, NULL);
                pvtAdd(hVal, tmpID, __h245(nullData), 0, NULL, NULL);
                pvtSetTree(hVal, pChannel->reverseDataTypeID, hVal, tmpID);
            }
            if ((tmpID = pvtAddBranch(hVal, reverseLCP_ID, __h245(multiplexParameters))) >= 0)
                    pvtSetTree(hVal, tmpID, hVal, pvtParent(hVal, h223ID));
        }

    }
    /* Check if there are conflicts */
    if (app->cmMyChannelEvent.cmEvChannelMasterSlaveConflict)
    {
        /* Set the internal state */
        pChannel->data.h223Params.internalState = channelInternalStateOLCSent;

        /* Check for conflicts */
        status = h223ParamsCheckConflicts(pChannel, RV_TRUE, &conflictingChannel, &goodChannel);
        if (status != RV_OK)
        {
            /* There are conflicts */
            status = H223notifyMasterSlaveConflict(conflictingChannel,
                                                   &goodChannel,
                                                   app,
                                                   conflictingChannel->data.h223Params.conflictType,
                                                   olcID);

            if (status != RV_OK)
            {
                RvBool channelRejected = RV_FALSE;

                if (emaLock((EMAElement)conflictingChannel))
                {
                    /* Handle conflict if we have to */
                    status = h223ParamsHandleConflicts(conflictingChannel, &channelRejected);

                    emaUnlock((EMAElement)conflictingChannel);
                }
            }

        }
    }

    /*ACP - set create the 223 channel*/
    if (ctrl->bACPinUse == RV_TRUE && app->evHandlers.pfnH223LogicalChannelEv != NULL)
    {
        /* first check if this channel is opened after another channel of the same dataType was rejected
           and if so - destruct the 223 layer of the rejected channel. */

        void        *chanPtr = NULL;
        H245Channel *rejectedChannel = getNextOutChan((HCONTROL)ctrl, &chanPtr);

        while (rejectedChannel != NULL)
        {
            if (rejectedChannel != NULL &&
                rejectedChannel != pChannel &&
                (rejectedChannel->data.h223Params.eDataType ==
                                pChannel->data.h223Params.eDataType))
                                break;
            rejectedChannel = getNextOutChan((HCONTROL)ctrl, &chanPtr);
        }

        if (rejectedChannel != NULL &&
            rejectedChannel->data.h223Params.internalState > channelInternalStateAcked &&
            rejectedChannel->data.h223Params.conflictType != cmH245ConflictNoConflict)
        {
            /* remove the 223 layer of the rejected channel */
            status = app->evHandlers.pfnH223LogicalChannelEv(cmiGetByControl(rejectedChannel->hCtrl),
                                                                            (HCHAN)rejectedChannel, RV_FALSE);
            rejectedChannel->data.h223Params.alTxContext = NULL;
        }


        status = app->evHandlers.pfnH223LogicalChannelEv(cmiGetByControl((HCONTROL)ctrl),
                                                        (HCHAN)pChannel, RV_TRUE);
        if (status != RV_OK)
        {
            /*drop the channel*/
            cmChannelDrop((HCHAN)pChannel);
            return;
        }
    }
}


/************************************************************************
 * h223AllocateChannel
 * purpose: initiating h223 parameters while allocating the channel.
 * input  : pChannel - the channel object.
 * output : none
 * return : none
 ************************************************************************/
void h223AllocateChannel(H245Channel* pChannel)
{
    pChannel->eType = RvH245ChannelParamsH223;
    pChannel->data.h223Params.adaptationLayerType = cmH223ALLevelUnknown;
    pChannel->data.h223Params.adaptationLayerData.adaptationLayer1Params.bIsFramed = RV_TRUE;
    pChannel->data.h223Params.adaptationLayerData.adaptationLayer3Params.controlFieldSize = cmH223ControlFieldSizeNone;
    pChannel->data.h223Params.adaptationLayerData.adaptationLayer3Params.sendBufferSize = 0;
    pChannel->data.h223Params.bMaxMuxPduSize = 0;
    pChannel->data.h223Params.bRecvLCBufferBelongsToApplication = RV_FALSE;
    pChannel->data.h223Params.bSkewIndication = RV_FALSE;
    pChannel->data.h223Params.isSegmentable = RV_FALSE;
    pChannel->data.h223Params.alTxContext = NULL;
    pChannel->data.h223Params.alRxContext = NULL;
    pChannel->data.h223Params.eAudioType = cmChannelAudioUnknown;
    pChannel->data.h223Params.bitRate = 0;
    pChannel->data.h223Params.bIgnoreCorruptedAlSdu = RV_TRUE;
    pChannel->data.h223Params.bIsDuplexReverseNullData = RV_FALSE;
#if (RV_3G324M_USE_MONA == RV_YES)
    pChannel->data.h223Params.bIsMPCChannel = RV_FALSE;
#endif /* USE_MONA */

	if (((H245Control*)(pChannel->hCtrl))->logicalChannelOut < 14)
	     ((H245Control*)(pChannel->hCtrl))->logicalChannelOut = 14;

}

/************************************************************************
 * h223ChannelFreeMemory
 * purpose: freeing h223 parameters while freeing the channel.
 * input  : pChannel - the channel object.
 *          hVal     - handle to the PVT.
 * output : none
 * return : none
 ************************************************************************/
void h223ChannelFreeMemory(H245Channel* pChannel, HPVT hVal)
{
    HH245      hApp = (HH245)emaGetUserData((EMAElement)pChannel);
    H245Object *pApp = (H245Object*)hApp;

    RV_UNUSED_ARG(hVal);

    /* delete H.223 objects by the upper layer */
    if (pApp->evHandlers.pfnH223LogicalChannelEv != NULL)
    {
        pApp->evHandlers.pfnH223LogicalChannelEv(cmiGetByControl(pChannel->hCtrl),
                                                 (HCHAN)pChannel,
                                                 RV_FALSE);
    }
    memset(&(pChannel->data.h223Params), 0, sizeof(ChannelH223Params));
}

/************************************************************************
 * h223ParamsOpenLogicalChannelAck
 * purpose: handle received OpenLogicalChannelAck message.
 * input  : pApp          - application handle for the cm element.
 *          hVal          - handle to the PVT.
 *          pChannel      - Channel element.
 *          pCtrl         - Control element.
 *          message       - node id for the OpenLogicalChannelAck message.
 * output : none
 * return : none
 ************************************************************************/
void h223ParamsOpenLogicalChannelAck(IN H245Object  *pApp,
                                     IN HPVT        hVal,
                                     IN H245Channel *pChannel,
                                     IN H245Control *pCtrl,
                                     IN RvPvtNodeId message)
{
    RvPvtNodeId reverseLCP_ID;
    RvInt32     value = 0;

    if (!emaWasDeleted((EMAElement)pChannel))
    {
        RvStatus status;

        reverseLCP_ID = pvtGetChild(hVal,message, __h245(reverseLogicalChannelParameters),NULL);
        if (pChannel->bIsDuplex)
            pvtGetChildByFieldId(hVal,reverseLCP_ID, __h245(reverseLogicalChannelNumber), &value, NULL);
        pChannel->reverseLogicalChannelNum = (RvUint16)value;

        /* create H.223 objects by the upper layer */
        if (pApp->evHandlers.pfnH223LogicalChannelEv != NULL)
        {
            status = pApp->evHandlers.pfnH223LogicalChannelEv(cmiGetByControl((HCONTROL)pCtrl),
                (HCHAN)pChannel, RV_TRUE);
            if (status != RV_OK)
            {
                /*drop the channel*/
                cmChannelDrop((HCHAN)pChannel);
                return;
            }
        }

        /* the nodeID of h223 is 0 because there are no h223 parameters in the ACK
           message */
        /*if a slave - check if it's a conflicting channel that was supposed to be rejected*/
        if (!pCtrl->bIsMaster)
        {
            if (pChannel->data.h223Params.internalState == channelInternalStaterejectPending)
            {
                /*Find the incoming channel handle for the application*/
                H245Channel* goodChannel = getInChanByDataType((HCONTROL)(pChannel->hCtrl), pChannel->data.h223Params.eDataType);
                pChannel->data.h223Params.conflictType = cmH245ConflictMasterNotRejected;
                /*raise master/slave conflict to application*/
                status = H223notifyMasterSlaveConflict(pChannel,
                                                       &goodChannel,
                                                       pApp,
                                                       pChannel->data.h223Params.conflictType,
                                                       message);

                if (status != RV_OK)
                {
                    /*drop the channel*/
                    cmChannelDrop((HCHAN)pChannel);
                    return;
                }
            }
        }

        /*set internalState and conflict*/
        pChannel->data.h223Params.conflictType = cmH245ConflictNoConflict;
        pChannel->data.h223Params.internalState = channelInternalStateAcked;

        establishConfirmed(pChannel, 0);
    }
}


/************************************************************************
 * h223ParamsOpenLogicalChannelreject
 * purpose: handle received OpenLogicalChannelreject message.
 * input  : pApp          - application handle for the cm element.
 *          pChannel      - Channel element.
 *          pCtrl         - Control element.
 *          message       - node id for the OpenLogicalChannelAck message.
 *          rejectReason  - Reject reason received on an outgoing channel.
 * output : none
 * return : none
 ************************************************************************/
void h223ParamsOpenLogicalChannelReject(
    IN  H245Object              *pApp,
    IN  H245Channel             *pChannel,
    IN  H245Control             *pCtrl,
    IN  RvPvtNodeId             message,
    IN  cmRejectLcnReason       rejectReason,
    OUT cmChannelStateMode_e    *stateMode)
{
    RV_UNUSED_ARG(message);
    if (emaLock((EMAElement)pChannel))
    {
        if (rejectReason == cmRejectReasonMasterSlaveConflict)
        {
            *stateMode = cmChannelStateModeDisconnectedMasterSlaveConflict;

            /*if a master - rejects from the slave are never because of ChannelMasterSlaveConflict*/
            if (pCtrl->bIsMaster)
            {
                emaUnlock((EMAElement)pChannel);
                return;
            }

            if (pChannel->data.h223Params.conflictType == cmH245ConflictSlaveBiDirectProposal &&
               pChannel->data.h223Params.internalState == channelInternalStaterejectPending &&
               pApp->cmMyChannelEvent.cmEvChannelMasterSlaveConflict)
            {
                /*Find the incoming channel handle for the application*/
                H245Channel* goodChannel = getInChanByDataType((HCONTROL)(pChannel->hCtrl), pChannel->data.h223Params.eDataType);;
                /*set internal state*/
                pChannel->data.h223Params.internalState = channelInternalStaterejectedReOpenRequired;
                pChannel->data.h223Params.conflictType = cmH245ConflictNoConflict;
                /*master/slave - raise event + do nothing*/
                emaUnlock((EMAElement)pChannel);
                H223notifyMasterSlaveConflict(pChannel,
                                              &goodChannel,
                                              pApp,
                                              cmH245ConflictSlaveBiDirectProposalRejected,
                                              RV_PVT_INVALID_NODEID);

                return;
            }
        }

        pChannel->data.h223Params.internalState = channelInternalStateNULL;
        pChannel->data.h223Params.conflictType = cmH245ConflictNoConflict;
        emaUnlock((EMAElement)pChannel);
    }
}


/************************************************************************
 * h223ParamsChannelAnswer
 * purpose: Build OpenLogicalChannelAck message.
 * input  : hVal          - handle to the PVT.
 *          pChannel      - Channel element to add h225 parameters OLC for.
 *          pCtrl         - Control element.
 *          olcID         - node id for the open logical channel.
 * output : none
 * return : RV_OK on success, other on failure
 ************************************************************************/
RvStatus h223ParamsChannelAnswer(
    IN HPVT        hVal,
    IN H245Channel *pChannel,
    IN H245Control *pCtrl,
    IN RvPvtNodeId olcaID)
{
    HH245       hApp = (HH245)emaGetUserData((EMAElement)pChannel);
    H245Object  *pApp = (H245Object*)hApp;
    RvPvtNodeId reverseLCN_ID;

    if (pChannel->bIsDuplex)
    { /* (for duplex channels). Declare reverse pChannel with lcn. */
        pCtrl->logicalChannelOut++;
        pChannel->reverseLogicalChannelNum=pCtrl->logicalChannelOut;
        reverseLCN_ID=pvtAddBranch(hVal,olcaID, __h245(reverseLogicalChannelParameters));
        pvtAdd(hVal,reverseLCN_ID, __h245(reverseLogicalChannelNumber), pChannel->reverseLogicalChannelNum,NULL,NULL);

        if (pChannel->pReplacementCh && pChannel->pReplacementCh->reverseLogicalChannelNum)
            pvtAdd(hVal,reverseLCN_ID, __h245(replacementFor), pChannel->pReplacementCh->reverseLogicalChannelNum, NULL,NULL);
    }

    /* create H.223 objects by the upper layer */
    if (pApp->evHandlers.pfnH223LogicalChannelEv != NULL)
    {
        return
            pApp->evHandlers.pfnH223LogicalChannelEv(cmiGetByControl((HCONTROL)pCtrl),
                                                 (HCHAN)pChannel,
                                                 RV_TRUE);
    }

    return RV_OK;
}




/************************************************************************
 * h223ParamsOpenLogicalChannelEvent
 * purpose: Open logical channel event.
 * input  : app            - application handle for the cm.
 *          hVal            - handle for the PVT.
 *          forwardLCP_ID   - node id for the forward open logical channel parameters.
 *          reverseLCDataType_ID    - node id for the reverse data type.
 *          message         - node id for the message.
 *          pChannel        - Channel element to set h223 parameters for.
 * output : h223ID          - the node id of the h223 parameters.
 *          bIsReturn       - if true then there was a return value, otherwise there wasn't.
 * return : int
 ************************************************************************/
int h223ParamsOpenLogicalChannelEvent(
    IN H245Object    *app,
    IN HPVT          hVal,
    IN RvPvtNodeId   forwardLCP_ID,
    IN RvPvtNodeId   reverseLCDataType_ID,
    IN RvPvtNodeId   message,
    IN H245Channel   *pChannel,
    OUT RvPvtNodeId  *h223ID,
    OUT RvBool       *bIsReturn)
{
    int                         nesting;
    RvPvtNodeId                 adaptationLayerID;
	RvPvtNodeId					reverseAdaptationLayerID;
    RvPstFieldId                fieldId;
    RvInt32                     tmpVal;
    RvStatus                    status;
    H245Channel                 *conflictingChannel, *goodChannel;
    RvPvtNodeId                 reverseH223ID = RV_PVT_INVALID_NODEID, reverseParamsNodeId = RV_PVT_INVALID_NODEID;

    RV_UNUSED_ARG(message);

    *bIsReturn = RV_FALSE;

    /*get the reverse dataType parameter*/
     pChannel->data.h223Params.bIsDuplexReverseNullData = RV_PVT_NODEID_IS_VALID(pvtGetChild(hVal, reverseLCDataType_ID, __h245(nullData), NULL));

    /* H.223 section. */
    /*if a video channel - set role +*/
    pvtGet(hVal, pvtChild(hVal, pChannel->dataTypeID), &fieldId, NULL, NULL, NULL);
    switch (fieldId)
    {
        case __h245(videoData):
            /*new incoming channel - set channel role*/
            {
                if ((H223getVideoChannelRole(pChannel, pChannel->dataTypeID, app)) == 0)
                    pChannel->data.h223Params.eDataType = cmChannelDataTypeVideo;
                else
                    pChannel->data.h223Params.eDataType = cmChannelDataTypeVideo2;
            }
            break;
        case __h245(audioData):
            pChannel->data.h223Params.eDataType = cmChannelDataTypeAudio;
            break;
        case __h245(data):
            pChannel->data.h223Params.eDataType = cmChannelDataTypeData;
            break;
        case __h245(nonStandard):
        case __h245(nullData):
        default:
            pChannel->data.h223Params.eDataType = cmChannelDataTypeUnknown;
            break;
    }

    /*set reverse nullData dataType*/

    /*set internal state*/
    pChannel->data.h223Params.internalState = channelInternalStateOLCReceived;

    /*incoming new OLC - check conflicts*/
    if (app->cmMyChannelEvent.cmEvChannelMasterSlaveConflict != NULL)
    {
        status = h223ParamsCheckConflicts(pChannel, RV_FALSE, &conflictingChannel, &goodChannel);
        if (status != RV_OK) /*there are conflicts*/
        {
            HAPPCHAN haChan = (HAPPCHAN)emaGetApplicationHandle((EMAElement)pChannel);
            H245CBEnter((app->pAppObject, "cmEvChannelMasterSlaveConflict(haChan=%p,hsChan=%p,conflictType=%d)",
                    haChan, (HCHAN)pChannel, conflictingChannel->data.h223Params.conflictType));
            nesting = emaPrepareForCallback((EMAElement)pChannel);
            status = app->cmMyChannelEvent.cmEvChannelMasterSlaveConflict(
                    haChan, (HCHAN)pChannel,
                    conflictingChannel->data.h223Params.conflictType,
                    (HCHAN*)&goodChannel,
                    1,
                    message);
            emaReturnFromCallback((EMAElement)pChannel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMasterSlaveConflict."));

            if (status != RV_OK)
            {
                /*handle conflict if we have to*/
                status = h223ParamsHandleConflicts(conflictingChannel, bIsReturn);

                /*if I am a master and incoming channel was rejected - return*/
                if (*bIsReturn == RV_TRUE)
                    return (status);
            }
        }
    }

    *h223ID = pvtGetChild2(hVal,forwardLCP_ID, __h245(multiplexParameters), __h245(h223LogicalChannelParameters));
    if (*h223ID >= 0)
    {
        __pvtGetByFieldIds(adaptationLayerID, hVal, *h223ID, {_h245(adaptationLayerType) _anyField LAST_TOKEN}, &fieldId, NULL, NULL);
        if (RV_PVT_NODEID_IS_VALID(adaptationLayerID))
        {
            switch (fieldId)
            {
                case (__h245(al1Framed)):
                {
                    pChannel->data.h223Params.adaptationLayerType = cmH223ALLevel1;
                    pChannel->data.h223Params.adaptationLayerData.adaptationLayer1Params.bIsFramed = RV_TRUE;
                    break;
                }

                case(__h245(al1NotFramed)):
                {
                    pChannel->data.h223Params.adaptationLayerType = cmH223ALLevel1;
                    pChannel->data.h223Params.adaptationLayerData.adaptationLayer1Params.bIsFramed = RV_FALSE;
                    break;
                }

                case(__h245(al2WithoutSequenceNumbers)):
                {
                    pChannel->data.h223Params.adaptationLayerType = cmH223ALLevel2;
                    pChannel->data.h223Params.adaptationLayerData.adaptationLayer2Params.sequenceNumbering = RV_FALSE;
                    break;
                }

                case(__h245(al2WithSequenceNumbers)):
                {
                    pChannel->data.h223Params.adaptationLayerType = cmH223ALLevel2;
                    pChannel->data.h223Params.adaptationLayerData.adaptationLayer2Params.sequenceNumbering = RV_TRUE;
                    break;
                }

                case(__h245(al3)):
                {
                    pChannel->data.h223Params.adaptationLayerType = cmH223ALLevel3;
                    pvtGetChildByFieldId(hVal, adaptationLayerID, __h245(controlFieldOctets), &tmpVal, NULL);
                    pChannel->data.h223Params.adaptationLayerData.adaptationLayer3Params.controlFieldSize = (cmH223ControlFieldSize)tmpVal;
                    pvtGetChildByFieldId(hVal, adaptationLayerID, __h245(sendBufferSize), &tmpVal, NULL);
                    pChannel->data.h223Params.adaptationLayerData.adaptationLayer3Params.sendBufferSize = (RvUint32)tmpVal;
                    /* Get the reverse params */
                    reverseParamsNodeId = pvtGetChild(hVal,message, __h245(reverseLogicalChannelParameters), NULL);
                    reverseH223ID = pvtGetChild2(hVal, reverseParamsNodeId, __h245(multiplexParameters), __h245(h223LogicalChannelParameters));
                    if (reverseH223ID >= 0)
                    {
                        __pvtGetByFieldIds(reverseAdaptationLayerID, hVal, reverseH223ID, {_h245(adaptationLayerType) _anyField LAST_TOKEN}, &fieldId, NULL, NULL);
                        if (RV_PVT_NODEID_IS_VALID(reverseAdaptationLayerID))
                        {
                            pvtGetChildByFieldId(hVal, reverseAdaptationLayerID, __h245(controlFieldOctets), &tmpVal, NULL);
                            pChannel->data.h223Params.adaptationLayerData.adaptationLayer3Params.reverseControlFieldSize = (cmH223ControlFieldSize)tmpVal;
                            pvtGetChildByFieldId(hVal, reverseAdaptationLayerID, __h245(sendBufferSize), &tmpVal, NULL);
                            pChannel->data.h223Params.adaptationLayerData.adaptationLayer3Params.reverseSendBufferSize = (RvUint32)tmpVal;
                        }
                    }

                    break;
                }

#if (RV_H223_USE_MUX_LEVEL_3 == RV_YES)
                case (__h245(al1M)):
                case (__h245(al3M)):
                {
                    RvPvtNodeId      tmpID;
                    cmH223ALxMConfig *alxM = &pChannel->data.h223Params.adaptationLayerData.adaptationLayerxMParams;
                    memset(alxM, 0, sizeof(*alxM));

                    if (fieldId == __h245(al1M))
                    {
                        pChannel->data.h223Params.adaptationLayerType = cmH223ALLevel1M;

                        __pvtGetNodeIdByFieldIds(tmpID, hVal, adaptationLayerID, {_h245(transferMode) _h245(framed) LAST_TOKEN});
                        alxM->bIsFramed = RV_PVT_NODEID_IS_VALID(tmpID);

                        pvtGetChildByFieldId(hVal, adaptationLayerID, __h245(alsduSplitting), &tmpVal, NULL);
                        alxM->bUseSplitting = (tmpVal != 0);
                    }
                    else
                    {
                        pChannel->data.h223Params.adaptationLayerType = cmH223ALLevel3M;
                    }

                    __pvtGetNodeIdByFieldIds(tmpID, hVal, adaptationLayerID, {_h245(headerFEC) _h245(golay24_12) LAST_TOKEN});
                    alxM->bUseGolay = RV_PVT_NODEID_IS_VALID(tmpID);

                    tmpID = pvtChild(hVal, pvtGetChild(hVal, adaptationLayerID, __h245(crcLength), NULL));
                    pvtGet(hVal, tmpID, &fieldId, NULL, NULL, NULL);
                    switch (fieldId)
                    {
                    case __h245(crcNotUsed): alxM->crcLength = 0; break;
                    case __h245(crc4bit): alxM->crcLength = 4; break;
                    case __h245(crc8bit): alxM->crcLength = 8; break;
                    case __h245(crc12bit): alxM->crcLength = 12; break;
                    case __h245(crc16bit): alxM->crcLength = 16; break;
                    case __h245(crc20bit): alxM->crcLength = 20; break;
                    case __h245(crc28bit): alxM->crcLength = 28; break;
                    case __h245(crc32bit): alxM->crcLength = 32; break;
                    default: break;
                    }

                    tmpVal = 0;
                    if (pvtGetChildByFieldId(hVal, adaptationLayerID, __h245(rsCodeCorrection), &tmpVal, NULL) >= 0)
                    {
                        alxM->codeRate = (RvUint8)(tmpVal + 128);
                    }
                    else
                    {
                        pvtGetChildByFieldId(hVal, adaptationLayerID, __h245(rcpcCodeRate), &tmpVal, NULL);
                        alxM->codeRate = (RvUint8)tmpVal;
                    }

                    tmpID = pvtChild(hVal, pvtGetChild(hVal, adaptationLayerID, __h245(arqType), NULL));
                    pvtGet(hVal, tmpID, &fieldId, NULL, NULL, NULL);
                    if (fieldId != __h245(noArq)) /* 0 is the default... */
                    {
                        tmpVal = 0;
                        alxM->arqType = (RvUint8)(1 + (fieldId == __h245(typeIIArq)));
                        pvtGetChildByFieldId(hVal, tmpID, __h245(sendBufferSize), &tmpVal, NULL);
                        alxM->sendBufferSize = (RvUint32)tmpVal;

                        tmpVal = 0;
                        tmpID = pvtChild(hVal, pvtGetChild(hVal, tmpID, __h245(numberOfRetransmissions), NULL));
                        pvtGet(hVal, tmpID, &fieldId, NULL, &tmpVal, NULL);
                        if (fieldId == __h245(infinite))
                            alxM->retransmissions = 255;
                        else
                            alxM->retransmissions = (RvUint8)tmpVal;
                    }

                    tmpVal = 0;
                    pvtGetChildByFieldId(hVal, adaptationLayerID, __h245(alpduInterleaving), &tmpVal, NULL);
                    alxM->bUseInterleaving = (tmpVal != 0);

                    break;
                }

                case (__h245(al2M)):
                {
                    RvPvtNodeId      tmpID;
                    cmH223ALxMConfig *alxM = &pChannel->data.h223Params.adaptationLayerData.adaptationLayerxMParams;
                    memset(alxM, 0, sizeof(*alxM));

                    pChannel->data.h223Params.adaptationLayerType = cmH223ALLevel2M;

                    __pvtGetNodeIdByFieldIds(tmpID, hVal, adaptationLayerID, {_h245(headerFEC) _h245(golay24_12) LAST_TOKEN});
                    alxM->bUseGolay = RV_PVT_NODEID_IS_VALID(tmpID);

                    pvtGetChildByFieldId(hVal, adaptationLayerID, __h245(alpduInterleaving), &tmpVal, NULL);
                    alxM->bUseInterleaving = (tmpVal != 0);
                    break;
                }
#endif /* (RV_H223_USE_MUX_LEVEL_3 == RV_YES) */

                default:
                    break;
            }
        }

        /* segmentable or non-segmentable */
        pvtGetChildByFieldId(hVal, *h223ID, __h245(segmentableFlag), &tmpVal, NULL);
        pChannel->data.h223Params.isSegmentable = (RvBool)tmpVal;
    }


    if (!pChannel->bIsDuplex)
         h22xFunctions[RvH245ChannelParamsH223].cmcCallAddressCallbacks(app, pChannel, message);
    if (!emaWasDeleted((EMAElement)pChannel))
    {
        if (pChannel->pReplacementCh && app->cmMyChannelEvent.cmEvChannelReplace)
        {
            H245CBEnter((app->pAppObject, "cmEvChannelReplace: haChan=%p, hsChan=%p, haReplaceChan=%p, hsReplaceChan=%p.",
                emaGetApplicationHandle((EMAElement)pChannel), pChannel, emaGetApplicationHandle((EMAElement)pChannel->pReplacementCh), pChannel->pReplacementCh));
            nesting = emaPrepareForCallback((EMAElement)pChannel);
            app->cmMyChannelEvent.cmEvChannelReplace((HAPPCHAN)emaGetApplicationHandle((EMAElement)pChannel),               (HCHAN)pChannel,
                                                     (HAPPCHAN)emaGetApplicationHandle((EMAElement)pChannel->pReplacementCh),(HCHAN)pChannel->pReplacementCh);
            emaReturnFromCallback((EMAElement)pChannel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelReplace."));
        }
    }
	/* if ACP call - reset the demux timer for ignoring data */
	if (((H245Control*)pChannel->hCtrl)->bACPinUse == RV_TRUE && cmiGetChannelDataType((HCHAN)pChannel) == cmChannelDataTypeVideo)
		cmiResetH223ACPIgnoreDataTimer((HCHAN)pChannel);
    return RV_OK;
}


/************************************************************************
 * h223ParamsOpenLogicalChannelConfirm
 * purpose: handling OpenLogicalChannelConfirm message.
 * input  : hVal          - handle to the PVT.
 *          pChannel      - Channel element.
 *          message       - node id for the OpenLogicalChannelAck message.
 * output : none
 * return : none
 ************************************************************************/
void h223ParamsOpenLogicalChannelConfirm(IN HPVT        hVal,
                                         IN H245Channel *pChannel,
                                         IN RvPvtNodeId message)
{
    RV_UNUSED_ARG(hVal);
    RV_UNUSED_ARG(message);
    establishConfirmed(pChannel, RV_PVT_INVALID_NODEID);
}

/************************************************************************
 * h223MaximumSkewIndication
 * purpose: notifing the application on the maximumSkew for a channel.
 * input  : ctrl    - the h245 control object of the call.
 *          message - incoming h2250MaximumSkewIndication message.
 * output : none
 * return : int - negative number will be error.
 *                positive number will be success.
 ************************************************************************/
int h223MaximumSkewIndication(H245Control* ctrl,RvPvtNodeId message)
{
    HPVT hVal;
    EMAElement callE;
    H245Object* app;
    H245Channel* channel1,* channel2;
    RvInt32 lcn1, lcn2, skew;
    int nesting;

    callE = (EMAElement)cmiGetByControl((HCONTROL)ctrl);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance(callE));

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelMaxSkew != NULL)
    {
        hVal=app->hVal;

        if (emaLock(callE))
        {
            /* get the skew number */
            pvtGetChildByFieldId(hVal, message, __h245(logicalChannelNumber1), &lcn1, NULL);
            pvtGetChildByFieldId(hVal, message, __h245(logicalChannelNumber2), &lcn2, NULL);
            pvtGetChildByFieldId(hVal, message, __h245(skew), &skew, NULL);

            channel1=getInChanByLCN((HCONTROL)ctrl,lcn1);
            channel2=getInChanByLCN((HCONTROL)ctrl,lcn2);

            if (channel1 == NULL || channel2 == NULL)
            {
                emaUnlock(callE);
                return RV_OK;
            }

            /* notify the application for the maxSkew number */
            H245CBEnter((app->pAppObject, "cmEvChannelMaxSkew: haCall=%p, hsCall=%p, chan1=%p, chan2=%p, skew=%d.",
                    emaGetApplicationHandle((EMAElement)channel1), channel1, emaGetApplicationHandle((EMAElement)channel2), channel2, skew));
            nesting = emaPrepareForCallback(callE);
            app->cmMyChannelEvent.cmEvChannelMaxSkew(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel1), (HCHAN)channel1,
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel2), (HCHAN)channel2, (RvUint32)skew);
            emaReturnFromCallback(callE, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMaxSkew."));
            emaUnlock(callE);
        }
    }
    return RV_OK;
}

/************************************************************************
 * h223communicationModeCommand
 * purpose: getting h223 parameters from communicationModeCommand message.
 * input  : pCtrl    - the H245 control object.
 *          message  - the communicationModeCommand message node id.
 * output : none
 * return : int - negative number will be error.
 *                positive number will be success.
 ************************************************************************/
int h223communicationModeCommand(H245Control* pCtrl, RvPvtNodeId message)
{
    RV_UNUSED_ARG(pCtrl);
    RV_UNUSED_ARG(message);
    return RV_TRUE;
}

/************************************************************************
 * h223ParamsDynamicRTPPayloadType
 * purpose: Dynamic RTP payload type event.
 * input  : pApp            - application handle for the cm.
 *          hVal            - handle for the PVT.
 *          h225ID          - node id for the h225 parameters.
 *          pChannel        - Channel element to set h225 parameters for.
 * output : none
 * return : none
 ************************************************************************/
void h223ParamsDynamicRTPPayloadType(
    IN H245Object*  pApp,
    IN HPVT         hVal,
    IN RvPvtNodeId  h225ID,
    IN H245Channel* pChannel)
{
    RV_UNUSED_ARG(pApp);
    RV_UNUSED_ARG(hVal);
    RV_UNUSED_ARG(h225ID);
    RV_UNUSED_ARG(pChannel);
    return;
}

/************************************************************************
 * h223ParamsChannelOpenDynamic
 * purpose: Sets the data type.
 * input  : pChannel    - Channel element to set h223 parameters for.
 *          hVal        - PVT handle.
 *          dataTypeId  - Data type node id.
 * output : none
 * return : none
 ************************************************************************/
void h223ParamsChannelOpenDynamic(H245Channel *pChannel, HPVT hVal, RvPvtNodeId dataTypeId)
{
    H245Control    *ctrl = (H245Control*)pChannel->hCtrl;
    H245Object     *app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    RvPstFieldId fieldId;

    pvtGet(hVal, dataTypeId, &fieldId, NULL, NULL, NULL);
    switch (fieldId)
    {
        case __h245(nonStandard):
        case __h245(nullData):
            pChannel->data.h223Params.eDataType = cmChannelDataTypeUnknown;
            break;
        case __h245(audioData):
            pChannel->data.h223Params.eDataType = cmChannelDataTypeAudio;
            break;
        case __h245(videoData):
            {
                /*set new outgoing channel role*/
                if ((H223getVideoChannelRole(pChannel, pvtParent(hVal, dataTypeId), app)) == 0)
                    pChannel->data.h223Params.eDataType = cmChannelDataTypeVideo;
                else
                    pChannel->data.h223Params.eDataType = cmChannelDataTypeVideo2;
            }
            break;
        case __h245(data):
            pChannel->data.h223Params.eDataType = cmChannelDataTypeData;
            break;
        default:
            pChannel->data.h223Params.eDataType = cmChannelDataTypeUnknown;
    }

    /*Check if we need to change the lcn of the channel*/
    return;
}

/************************************************************************
 * h223ParamsCallChannelParametersCallback
 * purpose: Gets same session and associated channel handles.
 * input  : hsChan        - The channel handle.
 * output : hsAssociatedH - The associated channel handle.
 *          haAssociatedH - The associated application channel handle.
 *          hsAssociatedH - The same channel handle.
 *          hsAssociatedH - The same application channel handle.
 * return : none
 ************************************************************************/
void h223ParamsCallChannelParametersCallback(HCHAN    hsChan,
                                             HCHAN    *hsAssociatedH,
                                             HAPPCHAN *haAssociatedH,
                                             HCHAN    *hsSameSessionH,
                                             HAPPCHAN *haSameSessionH)
{
    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(hsAssociatedH);
    RV_UNUSED_ARG(haAssociatedH);
    RV_UNUSED_ARG(hsSameSessionH);
    RV_UNUSED_ARG(haSameSessionH);
    return;
}

/************************************************************************
 * h223ParamsChannelClose
 * purpose: Make sure the default used session Id's are updated if we close them.
 * input  : ctrl        - The control object.
 *          channel     - The channel object.
 * output : none.
 * return : none
 ************************************************************************/
void h223ParamsChannelClose(H245Control *ctrl, H245Channel *channel)
{
    RV_UNUSED_ARG(ctrl);
    RV_UNUSED_ARG(channel);
    return;
}

/******************************************************************************
 * h223ParamsChannelReject
 * ----------------------------------------------------------------------------
 * General: Handles outgoing openLogicalChannelReject - Release the videoBuffers
 *          in the demux and set the timer of buffering media - in ACP setup calls.
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ctrl                 - H245Control object of the call.
 *         channel              - rejected Channel.
 * Output: None.
 *****************************************************************************/
void h223ParamsChannelReject(H245Control *ctrl, H245Channel *channel)
{
    RV_UNUSED_ARG(ctrl);

    if (channel->bOrigin == RV_FALSE)
    {
        cmChannelDataType dataType =  cmiGetChannelDataType((HCHAN)channel);

        /*clear demux channel for ACP calls*/
        cmiClearH223ACPChannel((HCHAN)channel, (dataType == cmChannelDataTypeVideo));
    }
}

/****************************************************************************************************************/
/* This function shall be called before the first channel is opened using the H.245 procedures.                 */
/* It is protected from the second call so it may be called each time the new channel is about to be established*/
/* The main purpose of the function is to mark well-known session IDs used by the fast start and                */
/* prevent in this way usage of them in H.245                                                                   */
/****************************************************************************************************************/
void h223ParamsDeriveChannels(HCONTROL ctrl)
{
    RV_UNUSED_ARG(ctrl);
    return;
}

/************************************************************************
 * h223ParamsSessionOpenDynamic
 * purpose: Handle the same session and associated channels.
 * input  : channel             - The channel object.
 *          ctrlE               - The control object.
 *          hsChanSameSession   - The same session channel handle.
 *          hsChanAssociated    - The associated channel handle.
 * output : none.
 * return : none
 ************************************************************************/
void h223ParamsSessionOpenDynamic(H245Channel *channel,
                                  H245Control *ctrlE,
                                  HCHAN       hsChanSameSession,
                                  HCHAN       hsChanAssociated,
                                  RvBool      isDynamicPayloadType)
{
    RV_UNUSED_ARG(channel);
    RV_UNUSED_ARG(ctrlE);
    RV_UNUSED_ARG(hsChanSameSession);
    RV_UNUSED_ARG(hsChanAssociated);
    RV_UNUSED_ARG(isDynamicPayloadType);
    return;
}

/************************************************************************
 * h223EndSessionCommand
 * purpose: Handle an incoming endSessionCommand message.
 *          This should stop the control of the call.
 * input  : ctrl    - Control object
 *          message - endSessionCommand message received
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int h223EndSessionCommand(IN H245Control* ctrl, IN int message)
{
    H245Object   *app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    HPVT         hVal=app->hVal;
    RvPvtNodeId  nodeId = -1;
    RvPstFieldId fieldId;
    RvInt32      result = 0;

    RV_UNUSED_ARG(message);

    /* See if we need to stop the control at all - it might have been closed already by an
       outgoing endSessionCommand message - check is done inside */

    __pvtGetByFieldIds(nodeId, hVal, message, {_anyField LAST_TOKEN}, &fieldId, &result, NULL);
    if (RV_PVT_NODEID_IS_VALID(nodeId) == RV_FALSE)
    {
        return RV_FALSE;
    }

    if (fieldId == __h245(gstnOptions))
    {
        __pvtGetByFieldIds(nodeId, hVal, nodeId, {_anyField LAST_TOKEN}, &fieldId, &result, NULL);
        if (fieldId == __h245(v34H324))
        {
            /* Check who initiated the Reset procedure. If the local side initiated
               it, and the incoming ESC is just the response then we need to ignore it,
               Otherwise we need to invoke a callback to notify the application
               about an incoming Reset request. */
            if (ctrl->bResetInitiator == RV_FALSE)
            {
                if (app->evHandlers.pfnReceivedResetRequestEv != NULL)
                {
                    app->evHandlers.pfnReceivedResetRequestEv(cmiGetByControl((HCONTROL)ctrl));
                }
            }
        }
    }
    else if (fieldId == __h245(disconnect))
    {
        stopControl((HCONTROL)ctrl);
        freeControl((HCONTROL)ctrl);
    }


    return RV_TRUE;
}



/************************************************************************
 * h223ParamsCheckConflicts
 * ----------------------------------------------------------------------------
 * General: Check if there are conflicting channels to the given channel and changes
 *          the conflict type and internal state of conflicting channel.
 *
 *  Return Value: RV_OK  - if no conflicts.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 *          channel                     - The channel object.
 *          isOutGoingChannel           - if RV_TRUE the channel is outgoing.
 * output : conflictingChannel          - the conflicting channel that
 *                                        should be taken care of (incoming in the master
 *                                        outgoing in the slave).
 * output : goodChannel                 - the reverse channel of the conflicting
 *                                        channel (outgoing in the master incoming
 *                                        in the slave.
 ***************************************************************************/
static RvStatus h223ParamsCheckConflicts(
    IN  H245Channel     *pChannel,
    IN  RvBool          isOutGoingChannel,
    OUT H245Channel     **conflictingChannel,
    OUT H245Channel     **goodChannel)
{
    H245Control    *ctrl = (H245Control*)pChannel->hCtrl;
    H245Object     *app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));

    /*if the master/slave conflict event not set
    in the application - dont look for conflics*/
    if (app->cmMyChannelEvent.cmEvChannelMasterSlaveConflict == NULL)
    {
        RvLogInfo(&app->log, (&app->log, "cmEvChannelMasterSlaveConflict is NULL - not checking conflicts"));
        return RV_OK;
    }

    /*look for slave conflicts*/
    if (!ctrl->bIsMaster)
    {
        /*if an outgoing channel*/
        if (isOutGoingChannel)
        {
            /*find an incoming channel with the same role*/
            (*goodChannel) = getInChanByDataType((HCONTROL)ctrl, pChannel->data.h223Params.eDataType);
            if ((*goodChannel) == NULL ||
                (*goodChannel)->eState == ChannelStateReleased ||
                (*goodChannel)->eState == ChannelStateAwaitingRelease)
            {
                /*no conflicts*/
                return RV_OK;
            }

            /*if the incoming channel is bi-directionsl - its a conflict*/
            if ((*goodChannel)->bIsDuplex &&
                !(*goodChannel)->data.h223Params.bIsDuplexReverseNullData)
            {
                /*set the outgoing channel's conflict type and return error*/
                if (pChannel->bIsDuplex &&
                    !pChannel->data.h223Params.bIsDuplexReverseNullData)
                {
                     pChannel->data.h223Params.conflictType = cmH245ConflictMasterSlaveBiDirectProposal;

                }
                else
                {
                    pChannel->data.h223Params.conflictType = cmH245ConflictMasterBiDirectProposal;
                }
                (*conflictingChannel) = pChannel;
                return RV_ERROR_UNKNOWN;
            }
            else
            {
                /*if outgoing channel is bi-directionsl its a conflict*/
                if (pChannel->bIsDuplex &&
                    !pChannel->data.h223Params.bIsDuplexReverseNullData)
                {
                    pChannel->data.h223Params.conflictType = cmH245ConflictSlaveBiDirectProposal;
                    (*conflictingChannel) = pChannel;
                    return RV_ERROR_UNKNOWN;
                }
            }

        }
        /*if an incoming channel*/
        else
        {
            (*goodChannel) = pChannel;
            /*look for an outgoing channel with the same role*/
            (*conflictingChannel) = getOutChanByDataType((HCONTROL)ctrl, pChannel->data.h223Params.eDataType);
            if((*conflictingChannel) == NULL || (*conflictingChannel)->eState == ChannelStateReleased)
            {
                /*no conflict*/
                return RV_OK;
            }
            /*if incoming channel is bi-directionsl -> a conflict*/
            if (pChannel->bIsDuplex &&
                !pChannel->data.h223Params.bIsDuplexReverseNullData)
            {
                if ((*conflictingChannel)->bIsDuplex &&
                    !(*conflictingChannel)->data.h223Params.bIsDuplexReverseNullData)
                {
                    (*conflictingChannel)->data.h223Params.conflictType = cmH245ConflictMasterSlaveBiDirectProposal;
                }
                else
                {
                    (*conflictingChannel)->data.h223Params.conflictType = cmH245ConflictMasterBiDirectProposal;
                }
                return RV_ERROR_UNKNOWN;
            }
            else
            {
                /*if outgoing is bi-directionsl -> a conflict*/
                if ((*conflictingChannel)->bIsDuplex &&
                    !(*conflictingChannel)->data.h223Params.bIsDuplexReverseNullData)
                {
                    (*conflictingChannel)->data.h223Params.conflictType = cmH245ConflictSlaveBiDirectProposal;
                    return RV_ERROR_UNKNOWN;
                }
            }

        }
    }/*slave's conflicts*/
    /*look for master conflicts*/
    else
    {
         /*if an outgoing channel*/
        if (isOutGoingChannel)
        {
            (*goodChannel) = pChannel;
            /*find an incoming channel with the same role*/
            (*conflictingChannel) = getInChanByDataType((HCONTROL)ctrl, pChannel->data.h223Params.eDataType);
            if((*conflictingChannel) == NULL || (*conflictingChannel)->eState == ChannelStateReleased)
            {
                /*no conflicts*/
                return RV_OK;
            }

            /*if the incoming channel is bi-directionsl - its a conflict*/
            if ((*conflictingChannel)->bIsDuplex &&
                !(*conflictingChannel)->data.h223Params.bIsDuplexReverseNullData)
            {
                /*set the incoming channel's conflict type and return error*/
                /****lock?****/
                if (pChannel->bIsDuplex &&
                    !pChannel->data.h223Params.bIsDuplexReverseNullData)
                {
                     (*conflictingChannel)->data.h223Params.conflictType = cmH245ConflictMasterSlaveBiDirectProposal;

                }
                else
                {
                    (*conflictingChannel)->data.h223Params.conflictType = cmH245ConflictSlaveBiDirectProposal;
                }
                return RV_ERROR_UNKNOWN;
            }
            else
            {
                /*if outgoing channel is bi-directionsl its a conflict*/
                if (pChannel->bIsDuplex &&
                    !pChannel->data.h223Params.bIsDuplexReverseNullData)
                {
                    (*conflictingChannel)->data.h223Params.conflictType = cmH245ConflictMasterBiDirectProposal;
                    return RV_ERROR_UNKNOWN;
                }
            }

        }
        /*if an incoming channel*/
        else
        {
            /*look for an outgoing channel with the same role*/
            (*goodChannel) = getOutChanByDataType((HCONTROL)ctrl, pChannel->data.h223Params.eDataType);
            if ((*goodChannel) == NULL)
            {
                /*no conflict*/
                return RV_OK;
            }
            /*if incoming channel is bi-directionsl -> a conflict*/
            if (pChannel->bIsDuplex &&
                !pChannel->data.h223Params.bIsDuplexReverseNullData)
            {
                if ((*goodChannel)->bIsDuplex &&
                    !(*goodChannel)->data.h223Params.bIsDuplexReverseNullData)
                {
                    pChannel->data.h223Params.conflictType = cmH245ConflictMasterSlaveBiDirectProposal;
                }
                else
                {
                    pChannel->data.h223Params.conflictType = cmH245ConflictSlaveBiDirectProposal;
                }
                (*conflictingChannel) = pChannel;
                return RV_ERROR_UNKNOWN;
            }/*incoming is duplex*/
            else
            {
                /*if outgoing is bi-directionsl -> a conflict*/
                if ((*goodChannel)->bIsDuplex &&
                    !(*goodChannel)->data.h223Params.bIsDuplexReverseNullData)
                {
                    pChannel->data.h223Params.conflictType = cmH245ConflictMasterBiDirectProposal;
                    (*conflictingChannel) = pChannel;
                    return RV_ERROR_UNKNOWN;
                }/*outgoing is duplex*/
            }/*else*/

        }/*master, incoming channel*/
    }/*master's conflicts*/
    return RV_OK;
}

/************************************************************************
 * h223ParamsHandleConflicts
 * ----------------------------------------------------------------------------
 * General: stack handles master/slave channel conflicts.
 *
 *  Return Value:
 * ----------------------------------------------------------------------------
 * Arguments:
 * input  : pChannel                     - The conflicting channel object.
 * output : channelRejected              - RV_TRUE if conflicting channel rejected.
 ************************************************************************/
static RvStatus h223ParamsHandleConflicts(
    IN H245Channel    *pChannel,
    OUT RvBool        *channelRejected)
{
    H245Control *pCtrl = (H245Control*)(pChannel->hCtrl);

    if (!pCtrl->bIsMaster)
    {
        /*if a slave - do nothing except updating conflict types and internal states*/
        switch(pChannel->data.h223Params.conflictType)
        {
            case cmH245ConflictMasterSlaveBiDirectProposal:
            case cmH245ConflictMasterBiDirectProposal:
            case cmH245ConflictSlaveBiDirectProposal:
                {
                    pChannel->data.h223Params.internalState = channelInternalStaterejectPending;
                    break;
                }
            default:
                {
                    /*do nothing*/
                    break;
                }
        }
    }
    else
    {
        /*if a master - reject the channel and update conflict types and internal states*/
        switch(pChannel->data.h223Params.conflictType)
        {
            case cmH245ConflictMasterSlaveBiDirectProposal:
            case cmH245ConflictMasterBiDirectProposal:
                {
                    pChannel->data.h223Params.internalState = channelInternalStateNULL;
                    /*reject channel*/
                    *channelRejected = RV_TRUE;
                    return rejectChannel(pChannel,__h245(masterSlaveConflict), cmChannelStateModeDisconnectedMasterSlaveConflict);
                }
            case cmH245ConflictSlaveBiDirectProposal:
                {
                        pChannel->data.h223Params.internalState = channelInternalStaterejectedReOpenExpected;
                        *channelRejected = RV_TRUE;
                        return rejectChannel(pChannel,__h245(masterSlaveConflict), cmChannelStateModeDisconnectedMasterSlaveConflict);
                }
            default:
                {
                    break;
                }
        }
    }

    return RV_OK;

}


#else
  int h223Paramsdummy= 1;
#endif /*#if (RV_H245_SUPPORT_H223_PARAMS == 1)*/

#ifdef __cplusplus
}
#endif
