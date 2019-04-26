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
#include "cmictrl.h"
#include "cmConf.h"
#include "caputils.h"
#include "cmchan.h"
#include "h245.h"
#include "cmChanGet.h"
#include "transpcap.h"
#include "pvaltreeStackApi.h"
#include "cmH225Params.h"
#include "cmH223Params.h"
#include "cmiParams.h"
#include "cmH245GeneralDefs.h"
#include "cmctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern H22XFunctions h22xFunctions[];

#define nprn(s) ((s)?(s):"(null)")


#if (RV_H245_SUPPORT_H225_PARAMS == 1)
static void setFirstChannels(IN H245Channel* channel);
#endif



/************************************************************************
 * channelFreeMemory
 * purpose: Free any PVT nodes held by a channel when not necessary
 *          anymore. This function is called only when working without
 *          a properties database.
 * input  : hsChan  - H245Channel object
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
static
int channelFreeMemory(
           IN   HCHAN       hsChan)
{
    H245Object* app=(H245Object *)emaGetUserData((EMAElement)hsChan);

    if (!app)
        return RV_ERROR_UNKNOWN;

    if (emaLock((EMAElement)hsChan))
    {
        H245Channel* pChannel = (H245Channel*)hsChan;

        if (RV_PVT_NODEID_IS_VALID(pChannel->reverseDataTypeID))
        {
            if (pChannel->reverseDataTypeID != pChannel->dataTypeID)
            {
                pvtDelete(app->hVal, pChannel->reverseDataTypeID);
            }
            pChannel->reverseDataTypeID = RV_PVT_INVALID_NODEID;
        }
        if (RV_PVT_NODEID_IS_VALID(pChannel->dataTypeID))
        {
            pvtDelete(app->hVal, pChannel->dataTypeID);
            pChannel->dataTypeID = RV_PVT_INVALID_NODEID;
        }

/*--*/
        h22xFunctions[((H245Control*)pChannel->hCtrl)->eParamsType].channelFreeMemory(pChannel, app->hVal);
/*--*/
        emaUnlock((EMAElement)hsChan);
    }

    return RV_TRUE;
}

/************************************************************************
 * channelConvertConfData2CapData
 * purpose: Convert the confDataType to cmCapDataType.
 * input  : hVal            - The PVT handle.
 *          eConfData       - The confDataType value.
 *          dataTypeHandle  - The dataTypeHandle of the channel. Gets the exact
 *                            dataType.
 * output : dataTypeHandle  - The dataTypeHandle of the channel. Gets the exact
 *                            dataType.
 *          eCcapData        - The cmCapDataType value.
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
static
int channelConvertConfData2CapData(
                      IN    HPVT          hVal,
                      IN    confDataType  eConfData,
                      INOUT RvPvtNodeId   *dataTypeHandle,
                      OUT   cmCapDataType *eCcapData)
{
    /* Get the handle to the data type by the data's type */
    switch (eConfData)
    {
        case confNonStandard:
            *eCcapData = cmCapNonStandard;
            return RV_OK;
        case confNullData:
            *eCcapData = cmCapEmpty;
            return RV_OK;
        case confVideoData:
            *eCcapData = cmCapVideo;
            *dataTypeHandle = pvtChild(hVal, *dataTypeHandle);
            return RV_OK;
        case confAudioData:
            *eCcapData = cmCapAudio;
            *dataTypeHandle = pvtChild(hVal, *dataTypeHandle);
            return RV_OK;
        case confData:
            *eCcapData = cmCapData;
            *dataTypeHandle = pvtChild(hVal, pvtChild(hVal, *dataTypeHandle));
            return RV_OK;
        case confH235Control:
        case confH235Media:
            *eCcapData = cmCapH235;
            return RV_OK;
        case confMultiplexedStream:
            *eCcapData = cmCapMultiplexedStream;
            return RV_OK;
        case confMultiplePayloadStream:
            *eCcapData = cmCapMultiplePayloadStream;
            return RV_OK;
        case confFec:
            *eCcapData = cmCapFEC;
            *dataTypeHandle = pvtChild(hVal, *dataTypeHandle);
            break;

            /* The following types are not quite supported with their types. The dataType handle will give
               a correct nodeId nevertheless */
        case confEncryptionData:
        case confRedundancyEncoding:
            return RV_OK;
        default:
            return RV_OK;
    }
    return RV_ERROR_NOT_FOUND;
}


/************************************************************************
 * dequeueChannelStates
 * purpose: Notify the application about the state of a channel
 * input  : app         - H245Object object
 *          channel     - H245Channel object
 *          state       - State of channel
 *          stateMode   - State mode of channel
 * output : none
 * return : none
 ************************************************************************/
static int dequeueChannelStates(
    IN H245Object*          app,
    IN H245Channel*         channel,
    IN cmChannelState_e     state,
    IN cmChannelStateMode_e stateMode)
{
    int nesting;

    channel->q_numStates++;
    while (channel->q_numStates > 0)
    {
#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
        {
            static const RvChar *cmChannelStateA[] = {
              "cmChannelStateDialtone",
              "cmChannelStateRingBack",
              "cmChannelStateConnected",
              "cmChannelStateDisconnected",
              "cmChannelStateIdle",
              "cmChannelStateOffering",
            };

            static const RvChar *cmChannelStateModeA[] = {
              "cmChannelStateModeOn",
              "cmChannelStateModeOff",
              "cmChannelStateModeDisconnectedLocal",
              "cmChannelStateModeDisconnectedRemote",
              "cmChannelStateModeDisconnectedMasterSlaveConflict",
              "cmChannelStateModeDuplex",
              "cmChannelStateModeDisconnectedReasonUnknown",
              "cmChannelStateModeDisconnectedReasonReopen",
              "cmChannelStateModeDisconnectedReasonReservationFailure",
              "cmChannelStateModeReconfigured"
            };

            H245CBEnter((app->pAppObject, "cmEvChannelStateChanged(haChan=%p, hsChan=%p,state=%s,stateMode=%s)",
                emaGetApplicationHandle((EMAElement)channel), channel, nprn(cmChannelStateA[state]),nprn(cmChannelStateModeA[stateMode])));
        }
#endif

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
        if (state == cmChannelStateIdle)
        {
            /* free the default channel Ids. if needed */
            setFirstChannels(channel);
        }
#endif

        nesting = emaPrepareForCallback((EMAElement)channel);
        if (app->cmMyChannelEvent.cmEvChannelStateChanged != NULL)
        {
            app->cmMyChannelEvent.cmEvChannelStateChanged(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel),
                (HCHAN)channel,
                state, stateMode);
        }
        else if (state == cmChannelStateIdle)
        {
            /* We automatically close the channel if application doesn't handle this callback */
            cmChannelClose((HCHAN)channel);
        }
        emaReturnFromCallback((EMAElement)channel, nesting);

        if ((state == cmChannelStateConnected) && (app->bIsPropertyModeNotUsed == RV_TRUE))
        {
            /* Since we're not using a properties database, we can free information from the
               channel when it gets to the connected state. */
            channelFreeMemory((HCHAN)channel);
        }

        /* set saveCallIndication */
        if (!emaWasDeleted((EMAElement)channel))
        {
            cmiSetSaveCallIndication(channel->hCtrl, RV_TRUE);
        }
#if (RV_LOGMASK & RV_LOGLEVEL_LEAVE)
        H245CBExit((app->pAppObject, "cmEvChannelStateChanged."));
#endif

        /* reduce state number */
        channel->q_numStates--;
        /* this loop will probably run just once or not at all, but sometimes disconnected and idle
           are raised from more than one place at a time */
        while (channel->q_numStates)
        {
            cmChannelState_e oldState = state;

            /* get next state from the queue */
            state = (cmChannelState_e) channel->q_states[channel->q_nextState];
            stateMode = (cmChannelStateMode_e) channel->q_stateModes[channel->q_nextState];
            channel->q_nextState = (RvUint8) ( (channel->q_nextState+1)%RV_3G324M_CHANNEL_STATE_MAX );

            if ( (oldState != cmChannelStateIdle) && /* idle is the "last stop" */
                 ( (oldState != cmChannelStateDisconnected) || (state == cmChannelStateIdle) ) ) /* only idle follows disconnected */
            {
                /* there is no problem of multiple threads raising disconnected and / or idle,
                   continue with states. */
                break;
            }
            /* eat this state, see if there are more */
            channel->q_numStates--;
        }
    }
    return 0;
}


/************************************************************************
 * notifyChannelState
 * purpose: Enqueue a state of a channel, and raise it if no other state
 *          is currently raised.
 * input  : channel     - H245Channel object
 *          state       - State of channel
 *          stateMode   - State mode of channel
 * output : none
 * return : none
 ************************************************************************/
int notifyChannelState(
    IN H245Channel*         channel,
    IN cmChannelState_e     state,
    IN cmChannelStateMode_e stateMode)
{
    HCONTROL ctrl;
    H245Object* app;
    int curState;

    if (channel == NULL)
        return RV_ERROR_NULLPTR;
    if (emaWasDeleted((EMAElement)channel))
        return RV_ERROR_DESTRUCTED;

    ctrl = channel->hCtrl;
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl(ctrl)));

    /* check if a state is already being handled */
    if (channel->q_numStates != 0)
    {
        int newState;
        int oldState;
        if (channel->q_numStates >= RV_3G324M_CHANNEL_STATE_MAX)
        {
            /* we may be in a state-loop - this shouldn't happen! */
            RvLogError(&app->log, (&app->log, "notifyState: We have a loop for hsChan=%p", channel));
            return RV_ERROR_UNKNOWN;
        }

        /* place the new state in the state queue */
        newState = (channel->q_nextState + channel->q_numStates - 1) % RV_3G324M_CHANNEL_STATE_MAX;
        oldState = (newState+RV_3G324M_CHANNEL_STATE_MAX-1)%RV_3G324M_CHANNEL_STATE_MAX;
        if((channel->q_states[oldState] != (RvUint8)state) || (channel->q_stateModes[oldState] != (RvUint8)stateMode))
        {
            /* Seems like this state is not the same state as its predecessor - add it in */
            channel->q_states[newState] = (RvUint8)state;
            channel->q_stateModes[newState] = (RvUint8)stateMode;
            channel->q_numStates++;
        }
        return RV_OK;
    }

    /* No current states - handle it as is */

    /* just update the "current state" to the real current state */
    curState = (channel->q_nextState + RV_3G324M_CHANNEL_STATE_MAX - 1)%RV_3G324M_CHANNEL_STATE_MAX;
    channel->q_states[curState] = (RvUint8)state;
    channel->q_stateModes[curState] = (RvUint8)stateMode;

    /* enter state handle */
    return dequeueChannelStates(app, channel, state, stateMode);
}


/* Call the data type handle callback */
int
cmcCallDataTypeHandleCallback(
      IN  HH245 hApp,
      IN  HCHAN hsChan, /* channel protocol */
      IN  int dataType, /* channel data type node id */
      IN  confDataType type)
{
    H245Object* app=(H245Object*)hApp;
    HAPPCHAN haChan;
    int dataTypeHandle;
    cmCapDataType dataTypeId = cmCapEmpty;
    HPVT hVal = app->hVal;
    int nesting;

    /* See if we have to use this callback at all */
    if (app->cmMyChannelEvent.cmEvChannelHandle == NULL)
        return RV_TRUE;

    /* This entry point is good enough for everyone - even for those that are not supported at all */
    dataTypeHandle = pvtChild(hVal, dataType);

    /* Get the handle to the data type by the data's type */
    channelConvertConfData2CapData(hVal, type, &dataTypeHandle, &dataTypeId);

    haChan = (HAPPCHAN)emaGetApplicationHandle((EMAElement)hsChan);

    H245CBEnter((app->pAppObject, "cmEvChannelHandle(haChan=%p, hsChan=%p, handle=%d, type=%d)", haChan, hsChan, dataTypeHandle, dataTypeId));
    nesting = emaPrepareForCallback((EMAElement)hsChan);
    app->cmMyChannelEvent.cmEvChannelHandle(haChan, hsChan, dataTypeHandle, dataTypeId);
    emaReturnFromCallback((EMAElement)hsChan, nesting);
    H245CBExit((app->pAppObject, "cmEvChannelHandle."));

    return RV_TRUE;
}


int
cmcCallChannelParametersCallback(
                 /* Call the channel parameter callback */
                 IN  HH245  hApp,
                 IN  HCHAN hsChan, /* channel protocol */
                 IN  int dataType, /* channel data type node id */
                 OUT confDataType* type)
{
    H245Object* app=(H245Object *)hApp;
    char channelName[64];
    RvUint32 rate=0;
    confDataType _type;
    HCHAN hsSameSessionH = NULL, hsAssociatedH = NULL;
    HAPPCHAN haChan, haSameSessionH = NULL, haAssociatedH = NULL;
    HPVT hVal=app->hVal;
    int nesting;

    /* if the cmEvChannelParameters callback isn't set, we don't really need anything more
       than the data type of this channel */
    if (app->cmMyChannelEvent.cmEvChannelParameters == NULL)
    {
        if (type != NULL)
            *type = (confDataType)pvtGetSyntaxIndex(hVal, pvtChild(hVal, dataType));
        return 0;
    }

    /* find out the channel's name by its data type */
    strcpy(channelName, "null");
    if(confGetDataTypeName(hVal, dataType, sizeof(channelName), channelName, &_type, NULL) < 0)
    {
        RvLogError(&app->log, (&app->log, "cmcCallChannelParametersCallback: error in confGetDataTypeName(), dataType=%d", dataType));
        if (type != NULL)
            *type = (confDataType)-1;
        return RV_ERROR_UNKNOWN;
    }
    RvLogDebug(&app->log,
        (&app->log, "cmcCallChannelParametersCallback: new channel with name %s", channelName));

    /* -- bit rate */
    if (_type == confData)
    {
        int nid;
        __pvtGetByFieldIds(nid,hVal, dataType,
                            {_h245(data)
                            _h245(maxBitRate)
                            LAST_TOKEN},
                            NULL, (RvInt32*)&rate, NULL);
        if (nid < 0)
            rate = 0;
    }

/*--*/
    h22xFunctions[((H245Control*)((H245Channel*)hsChan)->hCtrl)->eParamsType].CallChannelParametersCallback(hsChan, &hsAssociatedH, &haAssociatedH, &hsSameSessionH, &haSameSessionH);
/*--*/

    haChan = (HAPPCHAN)emaGetApplicationHandle((EMAElement)hsChan);

    H245CBEnter((app->pAppObject, "cmEvChannelParameters: haChan=%p, hsChan=%p, channelName=%s, "
         "AppSes=%p, Ses=%p, AppAso=%p, Aso=%p, rate=%d.",
         haChan, hsChan, channelName, haSameSessionH, hsSameSessionH, haAssociatedH, hsAssociatedH, rate));

    nesting = emaPrepareForCallback((EMAElement)hsChan);
    app->cmMyChannelEvent.cmEvChannelParameters(
        haChan, hsChan, channelName, haSameSessionH, hsSameSessionH, haAssociatedH, hsAssociatedH, rate);
    emaReturnFromCallback((EMAElement)hsChan, nesting);

    H245CBExit((app->pAppObject, "cmEvChannelParameters."));

    if (type) *type = _type;
    return RV_TRUE;
}



/* H245Channel Operations______________________________________________________________________*/




RVAPI int RVCALLCONV
cmSetChannelEventHandler(
             /* Set user callbacks functions for control channels. */
             IN HAPP        hApp,
             IN CMCHANEVENT cmChannelEvent,
             IN int         size)
{
    H245Object *app = (H245Object *)cmiGetH245Handle(hApp);

    if (!app) return RV_ERROR_UNKNOWN;

    H245APIEnter((hApp, "cmSetChannelEventHandler: hApp=%p, cmChannelEvent=%p, size=%d.", hApp, cmChannelEvent, size));

    memset(&app->cmMyChannelEvent, 0, sizeof(app->cmMyChannelEvent));
    memcpy(&app->cmMyChannelEvent, cmChannelEvent, RvMin((int)sizeof(app->cmMyChannelEvent), (RvSize_t)size));

    H245APIExit((hApp, "cmSetChannelEventHandler=0"));
    return 0;
}

RVAPI
int RVCALLCONV cmGetChannelEventHandler(
        IN      HAPP                hApp,
        OUT     CMCHANEVENT         cmChannelEvent,
        IN      int                 size)
{
    H245Object *app = (H245Object *)cmiGetH245Handle(hApp);

    if (!app) return RV_ERROR_UNKNOWN;

    H245APIEnter((hApp, "cmGetChannelEventHandler: hApp=%p, cmChannelEvent=%p, size=%d.", hApp, cmChannelEvent, size));

    memset(cmChannelEvent, 0, sizeof(app->cmMyChannelEvent));
    memcpy(cmChannelEvent, &app->cmMyChannelEvent, RvMin((int)sizeof(app->cmMyChannelEvent), (RvSize_t)size));

    H245APIExit((hApp, "cmGetChannelEventHandler=0"));
    return 0;
}

H245Channel* allocateChannel(HCONTROL ctrl)
{
    H245Object *app;
    H245Channel channel,*ch = NULL;
    memset(&channel,0xff,sizeof(H245Channel));
    channel.pReplacementCh = NULL;
    channel.pBase = NULL;
    channel.bIsDuplex = RV_FALSE; /* by default it is simplex (uni-directional) channel. */
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
    channel.pChannelParams.data.h223Params.bIsDuplexReverseNullData = RV_FALSE;
#endif /*RV_H245_SUPPORT_H223_PARAMS == 1*/


    channel.myLogicalChannelNum=++((H245Control*)ctrl)->logicalChannelOut;
    channel.reverseLogicalChannelNum=0;
    channel.hCtrl=ctrl;
    channel.bOrigin=RV_TRUE;
/*--*/
    h22xFunctions[((H245Control*)ctrl)->eParamsType].allocateChannel(&channel);
/*--*/
    channel.requestCloseParamID=RV_ERROR_UNKNOWN;
    channel.dataTypeID=RV_ERROR_UNKNOWN;
    channel.reverseDataTypeID = RV_ERROR_UNKNOWN;
    channel.eState=ChannelStateIdle;
    channel.remotePortNumber=RV_ERROR_UNKNOWN;
    channel.pTimer = NULL;
    channel.pRequestCloseTimer = NULL;
    channel.pMediaLoopTimer = NULL;
    channel.eRejectReason = (cmRejectLcnReason)-1;
    channel.capTableEntryNum = 0;
    channel.pNext = NULL;
    channel.pPrev = NULL;
    channel.q_nextState = 0;
    channel.q_numStates = 0;
    app = (H245Object*)cmiGetH245Handle((HAPP)(emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    ch = (H245Channel *)emaAdd(app->hChannels,NULL);
    if (ch)
    {
        H245Channel * chanList = (H245Channel *) cmH245GetChannelList(ctrl);

        emaLinkToElement((EMAElement)ch, (EMAElement)cmiGetByControl(ctrl));

        memcpy(ch,&channel,sizeof(H245Channel));
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
        ch->data.h223Params.conflictType = cmH245ConflictNoConflict;
        ch->data.h223Params.internalState = channelInternalStateNULL;
#endif /* (RV_H245_SUPPORT_H223_PARAMS == 1) */
        if (chanList != NULL)
        {
            /* add to channel list */
            ch->pNext = chanList;
            chanList->pPrev = ch;
        }
        cmH245SetChannelList(ctrl, (HCHAN) ch);
    }
    return ch;
}


RVAPI int RVCALLCONV
cmChannelNew (
          IN    HCALL       hsCall,
          IN    HAPPCHAN    haChan,
          OUT   LPHCHAN     lphsChan)
{
    HCONTROL ctrl=cmiGetControl(hsCall);
    H245Channel*channel = NULL;
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Object* app;

    if ((hsCall == NULL) || (hApp == NULL))
        return RV_ERROR_NULLPTR;

    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelNew: hsCall=%p, haChan=%p, lphsChan=%p.", hsCall, haChan, lphsChan));
    if (emaLock((EMAElement)hsCall))
    {
        channel=allocateChannel(ctrl);
        if (!channel)
        {
            *lphsChan = NULL;
            emaUnlock((EMAElement)hsCall);
            H245APIExit((app->pAppObject, "cmChannelNew: [RESOURCE PROBLEM]."));
            return RV_ERROR_OUTOFRESOURCES;
        }
        emaSetApplicationHandle((EMAElement)channel,(void*)haChan);
        channel->bOrigin=RV_TRUE;
        channel->hCtrl=ctrl;
        *lphsChan=(HCHAN)channel;
        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmChannelNew: *lphsChan=%p [OK].", channel));
    return 0;
}



RVAPI int RVCALLCONV
cmChannelGetCallHandles(
            /* Get the stack and application call handles by the channel handle */
            IN  HCHAN hsChan,
            OUT HCALL *hsCall,
            OUT HAPPCALL *haCall
            )
{
    H245Channel*channel=(H245Channel*)hsChan;
    HCONTROL ctrl;
    HCALL call;
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Object *app;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelGetCallHandles:hsChan=%p", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        ctrl=(HCONTROL)channel->hCtrl;
        call=cmiGetByControl(ctrl);

        if (ctrl)
        {
            if (hsCall) *hsCall = call;
            if (haCall) *haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)call);
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelGetCallHandles:hsCall =%p", hsCall));
    return RV_TRUE;
}

RvBool channelTimeoutEventsHandler(void*hsChan)
{
    H245Channel*channel=(H245Channel*)hsChan;
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Object* app=(H245Object*)hApp;
    HPVT hVal;
    int message,nodeId;

    if (!hApp) return RV_FALSE;

    if (emaLock((EMAElement)hsChan))
    {
        hVal = ((H245Object *)hApp)->hVal;

        app->evHandlers.pfnTimerClearEv(channel->hCtrl, &channel->pTimer);
        if (channel->eState!=ChannelStateReleased)
        {
            emaMark((EMAElement)hsChan);
            if (channel->eState==ChannelStateAwaitingEstablishment)
            {
                message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
                nodeId=pvtAddBranch2(hVal,message, __h245(request),__h245(closeLogicalChannel));
                pvtAdd(hVal,nodeId,__h245(forwardLogicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
                pvtAddBranch2(hVal,nodeId,__h245(source),__h245(lcse));

                sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);
                channel->eState=ChannelStateReleased;
                {
                    void* ptr=NULL;
                    H245Channel* dependent;
                    dependent = getNextOutChanByBase(channel->hCtrl,channel,&ptr);
                    while (dependent != NULL)
                    {/* release the dependent channels because the base timed out */
                        if (dependent->eState!=ChannelStateReleased)
                        {
                            ptr=NULL;
                            emaLock((EMAElement)dependent);
                            emaUnlock((EMAElement)hsChan);
                            dependent->eState=ChannelStateReleased;
                            notifyChannelState(dependent,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
                            notifyChannelState(dependent,cmChannelStateIdle, cmChannelStateModeOff);
                            emaLock((EMAElement)hsChan);
                            emaUnlock((EMAElement)dependent);
                        }
                        dependent = getNextOutChanByBase(channel->hCtrl,channel,&ptr);
                    }
                }
                ((H245Control*)channel->hCtrl)->conflictChannels--;
            }
            channel->eState=ChannelStateReleased;
            notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
            notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
            emaRelease((EMAElement)hsChan);
        }
        emaUnlock((EMAElement)hsChan);
    }
    return RV_FALSE;
}

RvBool channelRC_TimeoutEventsHandler(void*hsChan)
{
    HH245       hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel *channel=(H245Channel*)hsChan;
    HPVT        hVal;
    int         message,nodeId;
    H245Object  *app=(H245Object*)hApp;
    int         nesting;

    if (!hApp)   return RV_FALSE;

    if (emaLock((EMAElement)hsChan))
    {
        hVal = ((H245Object *)hApp)->hVal;

        app->evHandlers.pfnTimerClearEv(channel->hCtrl, &channel->pRequestCloseTimer);

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(indication),__h245(requestChannelCloseRelease));
        pvtAdd(hVal,nodeId,__h245(forwardLogicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);

        sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);

        if (app->cmMyChannelEvent.cmEvChannelRequestCloseStatus != NULL)
        {
            H245CBEnter((app->pAppObject, "cmEvChannelRequestCloseStatus: haChan=%p, hsChan=%p, status=reject.",
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), channel));
            nesting = emaPrepareForCallback((EMAElement)hsChan);
            app->cmMyChannelEvent.cmEvChannelRequestCloseStatus((HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmRequestCloseReject);
            emaReturnFromCallback((EMAElement)hsChan, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelRequestCloseStatus."));
        }

        emaUnlock((EMAElement)hsChan);
    }
    return RV_FALSE;
}


/************************************************************************
 * startEstablishment
 * purpose: Start establishing a channel. This function creates an OLC
 *          message from the channel element and sends it.
 * input  : app     - Stack object
 *          channel - H245Channel element to send OLC for
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int startEstablishment(IN HH245 hApp, IN HCHAN hChan)
{
    H245Object* app = (H245Object *)hApp;
    H245Channel* channel = (H245Channel *)hChan;
    HPVT hVal=app->hVal;
    int ret;
    RvPvtNodeId message,olcID, forwardLCP_ID,reverseLCP_ID,dataType;
    H245Control *pCtrl = (H245Control*)channel->hCtrl;

    /* Create an OLC message */
    message = pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
    __pvtBuildByFieldIds(olcID, hVal, message, {_h245(request) _h245(openLogicalChannel) LAST_TOKEN}, 0, NULL);

    forwardLCP_ID = pvtAddBranch(hVal, olcID, __h245(forwardLogicalChannelParameters));

    /* Reverse data type. set only if we're in duplex mode.*/
    if (channel->bIsDuplex)
        reverseLCP_ID = pvtAddBranch(hVal, olcID, __h245(reverseLogicalChannelParameters));
    else
        reverseLCP_ID = RV_ERROR_UNKNOWN;

    /* Set the dataType of this channel */
    dataType = pvtAddBranch(hVal, forwardLCP_ID, __h245(dataType));
    ret = pvtSetTree(hVal, dataType, hVal, channel->dataTypeID);


    if ((channel->pBase != NULL) && (channel->pBase->eState == ChannelStateEstablished))
    {
        /* Add dependency information if we have */
        pvtAdd(hVal, forwardLCP_ID, __h245(forwardLogicalChannelDependency), channel->pBase->myLogicalChannelNum, NULL, NULL);
        if (channel->pBase->reverseLogicalChannelNum > 0)
            pvtAdd(hVal, reverseLCP_ID, __h245(reverseLogicalChannelDependency), channel->pBase->reverseLogicalChannelNum, NULL, NULL);
    }

    if ((channel->pReplacementCh != NULL) && (channel->pReplacementCh->eState == ChannelStateEstablished))
    {
        /* Add replacement information if we have */
        pvtAdd(hVal, forwardLCP_ID, __h245(replacementFor), channel->pReplacementCh->myLogicalChannelNum, NULL, NULL);
        if (channel->pReplacementCh->reverseLogicalChannelNum > 0)
            pvtAdd(hVal, reverseLCP_ID, __h245(replacementFor), channel->pReplacementCh->reverseLogicalChannelNum, NULL, NULL);
    }

    /* Set the Logical H245Channel Number (LCN) */
    pvtAdd(hVal, olcID, __h245(forwardLogicalChannelNumber), channel->myLogicalChannelNum, NULL, NULL);
/*--*/
    /*h22xParamsStartEstablishment*/
    h22xFunctions[pCtrl->eParamsType].startEstablishment(hVal, olcID, forwardLCP_ID, reverseLCP_ID, dataType, channel);
/*--*/
    /* Notify channel's states. These are used to simulate earlier version behavior on channels */
    channel->eState = ChannelStateIdle;
    notifyChannelState(channel, cmChannelStateDialtone, cmChannelStateModeOff);
    notifyChannelState(channel, cmChannelStateRingBack, cmChannelStateModeOff);

    /* Finally, we send message */
    if (!emaWasDeleted((EMAElement)channel))
    {
        RvBool sentOnDummyHost = RV_FALSE;

        /* Send the message and delete it */
        ret = sendMessageH245Chan(channel->hCtrl, (HCHAN)channel, message, RV_TRUE, &sentOnDummyHost);

        if (!sentOnDummyHost)
        {
            /* Set a timer on this channel while we wait for an ack */
            int timeout = 29;
            pvtGetChildByFieldId(hVal, app->h245Conf, __h245(channelsTimeout), &(timeout), NULL);
            app->evHandlers.pfnTimerCancelEv(channel->hCtrl, &channel->pTimer);
            channel->pTimer = app->evHandlers.pfnTimerStartEv(channel->hCtrl,
                channelTimeoutEventsHandler, (void*)channel, timeout * 1000);
        }

        /* Change the state of this channel */
        channel->eState = ChannelStateAwaitingEstablishment;
    }

    return ret;
}


RVAPI int RVCALLCONV
cmChannelOpenDynamic(
                     IN      HCHAN               hsChan,
                     IN      int                 dataTypeHandle, /* data type (HPVT) tree handle */
                     IN      HCHAN               hsChanSameSession,
                     IN      HCHAN               hsChanAssociated,
                     IN      RvBool              isDynamicPayloadType
                     )
{
    H245Object   *app=(H245Object *)emaGetUserData((EMAElement)hsChan);
    HPVT         hVal;

    if (!app)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelOpenDynamic: hsChan=%p, dataTypeHandle=%d, hsChanSameSession=%p, hsChanAssociated=%p, isDynamicPayloadType=%d.",
        hsChan, dataTypeHandle, hsChanSameSession, hsChanAssociated, isDynamicPayloadType));

    if (emaLock((EMAElement)hsChan))
    {
        HCONTROL     ctrl;
        H245Control *ctrlE;
        H245Channel *channel=(H245Channel*)hsChan;
        RvPvtNodeId  dataTypeId;

        hVal = app->hVal;

        ctrl = channel->hCtrl;
        ctrlE = (H245Control*)ctrl;

        /* deciding if to use the node id from the user or from what was set earlier. */
        if (dataTypeHandle >= 0)
        {
            dataTypeId = dataTypeHandle;
        }
        else
        {
            dataTypeId = channel->dataTypeID;
        }

        if (
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
            ((ctrlE->bACPinUse != RV_TRUE) && 
             (ctrlE->bMPCinUse != RV_TRUE)) &&
#endif /* #if (RV_H245_SUPPORT_H223_PARAMS == 1) */
            ((ctrlE->eState != ctrlConnected) &&
             (ctrlE->eState != ctrlConference) &&
             (ctrlE->eState != ctrlMesFinished)))
        {
            channel->eState = ChannelStateReleased;
            notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
            notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
            emaUnlock((EMAElement)hsChan);

            H245APIExit((app->pAppObject, "cmChannelOpenDynamic: Control is not in a connected state, state=%d", ctrlE->eState));
            return RV_ERROR_UNKNOWN;
        }

        h22xFunctions[ctrlE->eParamsType].ChannelOpenDynamic(channel, hVal, dataTypeId);

        if (channel->pBase && channel->pBase->eState==ChannelStateAwaitingRelease)
        {
            channel->eState = ChannelStateReleased;
            notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
            notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelOpenDynamic: Base channel released [Invalid Circumstances]"));
            return RV_ERROR_UNKNOWN;
        }

        if (!RV_PVT_NODEID_IS_VALID(channel->dataTypeID))
            channel->dataTypeID = pvtAddRoot(hVal,app->h245DataType,0,NULL);
        if (channel->dataTypeID != dataTypeId)
            pvtAddTree(hVal,channel->dataTypeID,hVal,dataTypeId);

        if (channel->pBase && channel->pBase->eState==ChannelStateAwaitingEstablishment)
        {
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelOpenDynamic: Waiting for base channel [OK]"));
            return RV_TRUE;
        }
/*--*/
        h22xFunctions[ctrlE->eParamsType].SessionOpenDynamic(channel, ctrlE, hsChanSameSession, hsChanAssociated, isDynamicPayloadType);
/*--*/

        ctrlE->conflictChannels++;
        if (startEstablishment((HH245)app, hsChan) < 0)
        {
            channel->eState = ChannelStateReleased;
            notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
            notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelOpenDynamic: startEstablishment failed"));
            return RV_ERROR_UNKNOWN;
        }
        emaUnlock((EMAElement)hsChan);
    }

#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
    if (app->mibEvent.h341AddNewLogicalChannel)
        app->mibEvent.h341AddNewLogicalChannel(app->mibHandle, hsChan);
#endif

    H245APIExit((app->pAppObject, "cmChannelOpenDynamic=1"));
    return RV_TRUE;
}


int rejectChannel(H245Channel* channel, RvPstFieldId rejectReason, cmChannelStateMode_e rejectMode)
{
    HCONTROL ctrl=channel->hCtrl;
    H245Object* app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl(ctrl)));
    HPVT hVal=app->hVal;
    int message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
    int nodeId=pvtAddBranch2(hVal, message, __h245(response), __h245(openLogicalChannelReject));
    int res;

    pvtAdd(hVal,nodeId,__h245(forwardLogicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
    pvtAddBranch2(hVal, nodeId, __h245(cause), rejectReason);

    res = sendMessageH245Chan(ctrl, (HCHAN)channel, message, RV_TRUE, NULL);

    channel->eState = ChannelStateReleased;
    notifyChannelState(channel, cmChannelStateDisconnected, rejectMode);
    notifyChannelState(channel, cmChannelStateIdle, cmChannelStateModeOff);
    return res;
}


static
int openLogicalChannelEvent(IN H245Object* app, IN HPVT hVal, IN H245Channel* channel, IN int message)
{
    /* This function assumes that the channel element is locked */
    /*openLCNevent*/
    H245Control* ctrl;
    RvInt32      baseLCN,replaceLCN;
    int          forwardLCP_ID,reverseLCP_ID,h225ID;
    RvBool       bIsReturn;
    int          ret;

#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
    if (app->mibEvent.h341AddNewLogicalChannel)
          app->mibEvent.h341AddNewLogicalChannel(app->mibHandle, (HCHAN)channel);
#endif

    ctrl = (H245Control*)channel->hCtrl;
    forwardLCP_ID=pvtGetChild(hVal,message, __h245(forwardLogicalChannelParameters), NULL);
    pvtGetChildByFieldId(hVal,forwardLCP_ID,__h245(portNumber),(RvInt32 *)&channel->remotePortNumber,NULL);

    if (pvtGetChildByFieldId(hVal,forwardLCP_ID, __h245(forwardLogicalChannelDependency), &baseLCN, NULL)>=0)
    {
        channel->pBase = getInChanByLCN((HCONTROL)ctrl, baseLCN);
        if (!channel->pBase)
            return rejectChannel(channel,__h245(invalidDependentChannel),cmChannelStateModeDisconnectedLocal);
    }
    if (pvtGetChildByFieldId(hVal,forwardLCP_ID, __h245(replacementFor), &replaceLCN, NULL)>=0)
    {
        channel->pReplacementCh=getInChanByLCN((HCONTROL)ctrl, replaceLCN);
        if (!channel->pReplacementCh)
            return rejectChannel(channel,__h245(replacementForRejected),cmChannelStateModeDisconnectedLocal);
    }

    reverseLCP_ID=pvtGetChild(hVal,message, __h245(reverseLogicalChannelParameters), NULL);

    /* check for bi-directional channel.*/
    if (RV_PVT_NODEID_IS_VALID(reverseLCP_ID))
    {
        channel->bIsDuplex = RV_TRUE;
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
        /*check if reverse nullData*/
        if (RV_PVT_NODEID_IS_VALID(pvtGetChild2(hVal, reverseLCP_ID, __h245(dataType),__h245(nullData))))
        {
            /*set reverse nullData param*/
            cmChannelSetParam((HCHAN)channel, cmChannelParamH223ReverseNullDataType,
                    (RvInt32)RV_TRUE, NULL);
        }
#endif /*RV_H245_SUPPORT_H223_PARAMS == 1*/
    }

    {
        /* Saving the forward data type id */
        RvPvtNodeId tmpId;
        tmpId = pvtGetChild(hVal,forwardLCP_ID,__h245(dataType),NULL);
        if (tmpId >= 0)
        {
            if (channel->dataTypeID < 0)
                channel->dataTypeID = pvtAddRoot(hVal, NULL, 0, NULL);
            pvtMoveTree(hVal, channel->dataTypeID, tmpId);
        }

        /* Saving the reverse data type id */
        if (channel->bIsDuplex == RV_TRUE)
        {
            tmpId = -1;
            tmpId = pvtGetChild(hVal,reverseLCP_ID,__h245(dataType),NULL);
            if (tmpId >= 0)
            {
                if (channel->reverseDataTypeID < 0)
                    channel->reverseDataTypeID = pvtAddRoot(hVal, NULL, 0, NULL);
                pvtMoveTree(hVal, channel->reverseDataTypeID, tmpId);
            }
        }
    }

/*--*/
    ret = h22xFunctions[ctrl->eParamsType].openLogicalChannelEvent(app, hVal, forwardLCP_ID, channel->reverseDataTypeID, message, channel, &h225ID, &bIsReturn);
    if (bIsReturn == RV_TRUE)
    {
        return ret;
    }
/*--*/
    if (!emaWasDeleted((EMAElement)channel))
    {
        /* -- Parameters */
        if (!RV_PVT_NODEID_IS_VALID(channel->dataTypeID))
        {
            RvLogError(&app->log, (&app->log, "openLogicalChannelEvent: no data type."));
            return RV_ERROR_UNKNOWN;
        }
        else
        {
            confDataType type;
            cmcCallChannelParametersCallback((HH245)app, (HCHAN)channel, channel->dataTypeID, &type);
            if (!emaWasDeleted((EMAElement)channel))
                cmcCallDataTypeHandleCallback((HH245)app, (HCHAN)channel, channel->dataTypeID, type);
        }
    }

/*--*/
    if (!emaWasDeleted((EMAElement)channel))
    {
        h22xFunctions[ctrl->eParamsType].dynamicRTPPayloadType(app, hVal, h225ID, channel);
        channel->eState=ChannelStateAwaitingEstablishment;

        /* -- state changed */
        notifyChannelState(channel,cmChannelStateOffering,(channel->bIsDuplex)?cmChannelStateModeDuplex:cmChannelStateModeOff);
    }

    return RV_OK;
}


/******************************************************************************
 * ConvertFieldId2RejectReason
 * ----------------------------------------------------------------------------
 * General: Convert the PST field id to an OLC reject reason.
 *
 * Return Value: cmRejectLcnReason - if successful
 *               negative on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  rejectReasonFieldId      - The reject reason field id.
 * Output: None.
 *****************************************************************************/
static RvInt32 ConvertFieldId2RejectReason(IN     RvPstFieldId   rejectReasonFieldId)
{
    switch (rejectReasonFieldId)
    {
        case __h245(unspecified):
            return (RvInt32)cmRejectReasonUnspecified;
        case __h245(unsuitableReverseParameters):
            return (RvInt32)cmRejectReasonUnsuitableReverseParameters;
        case __h245(dataTypeNotSupported):
            return (RvInt32)cmRejectReasonDataTypeNotSupported;
        case __h245(dataTypeNotAvailable):
            return (RvInt32)cmRejectReasonDataTypeNotAvailable;
        case __h245(unknownDataType):
            return (RvInt32)cmRejectReasonUnknownDataType;
        case __h245(dataTypeALCombinationNotSupported):
            return (RvInt32)cmRejectReasonDataTypeALCombinationNotSupported;
        case __h245(multicastChannelNotAllowed):
            return (RvInt32)cmRejectReasonMulticastChannelNotAllowed;
        case __h245(insufficientBandwidth):
            return (RvInt32)cmRejectReasonInsufficientBandwidth;
        case __h245(separateStackEstablishmentFailed):
            return (RvInt32)cmRejectReasonSeparateStackEstablishmentFailed;
        case __h245(invalidSessionID):
            return (RvInt32)cmRejectReasonInvalidSessionID;
        case __h245(masterSlaveConflict):
            return (RvInt32)cmRejectReasonMasterSlaveConflict;
        case __h245(waitForCommunicationMode):
            return (RvInt32)cmRejectReasonWaitForCommunicationMode;
        case __h245(invalidDependentChannel):
            return (RvInt32)cmRejectReasonInvalidDependentChannel;
        case __h245(replacementForRejected):
            return (RvInt32)cmRejectReasonReplacementForRejected;
        case __h245(securityDenied):
            return (RvInt32)cmRejectReasonSecurityDenied;
        default:
            break;
    }

    return -1;
}

int openLogicalChannel(H245Control *ctrl, int base, int message)
{
    RvInt32 lcn = 0;
    HPVT hVal;
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    H245Object* app=(H245Object*)hApp;
    H245Channel* channel = NULL;
    int nesting;
    RvBool bRejectChannel = RV_FALSE;
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
#if (RV_3G324M_USE_MONA == RV_YES)
    H245Channel* mpcChannel = NULL;
#endif /* USE_MONA */
#endif /*RV_H245_SUPPORT_H223_PARAMS */

    if (!hApp) return RV_ERROR_NULLPTR;

    hVal = ((H245Object *)hApp)->hVal;

    if (ctrl->eState == ctrlEndSession)
    {
        /* control was not even initialized (could only happen when tunneling is approved, and FastStart
        connected, and other side is working non-standard) or it has already been dropped. Reject the
        channel and exit.*/
        RvLogError(&app->log,
            (&app->log, "Received OLC when control is not in connected state (haCall=%p, hsCall=%p)",
                emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),cmiGetByControl((HCONTROL)ctrl)));
        bRejectChannel = RV_TRUE;
    }
    else if ((ctrl->eState != ctrlConnected) &&
             (ctrl->eState != ctrlConference) &&
             (ctrl->eState != ctrlTransmittedSidePaused) &&
             (ctrl->eState != ctrlMesFinished))
    {
        /* received OLC when control is not connected yet (did not finish MSD and Cap-Ex), keep the
        message for later (if we have room), and exit */
        if (!RV_PVT_NODEID_IS_VALID(ctrl->incomingOLCs[0]))
        {
            ctrl->incomingOLCs[0] = pvtAddRoot(hVal,NULL,0,NULL);
            pvtShiftTree(hVal,ctrl->incomingOLCs[0],base);
            return 0;
        }
        else if (!RV_PVT_NODEID_IS_VALID(ctrl->incomingOLCs[1]))
        {
            ctrl->incomingOLCs[1] = pvtAddRoot(hVal,NULL,0,NULL);
            pvtShiftTree(hVal,ctrl->incomingOLCs[1],base);
            return 0;
        }
        bRejectChannel = RV_TRUE;
    }
    if (!bRejectChannel)
    {
        h22xFunctions[ctrl->eParamsType].DeriveChannels((HCONTROL)ctrl);
        pvtGetChildByFieldId(hVal,message,__h245(forwardLogicalChannelNumber),&lcn,NULL);
        channel=allocateChannel((HCONTROL)ctrl);
        if (!channel)
        {
            RvLogError(&app->log,
                (&app->log, "Unable to allocate channel"));
            bRejectChannel = RV_TRUE;
        }
    }

    if (bRejectChannel)
    {
        RvPvtNodeId rejMessage, nodeId;

        pvtGetChildByFieldId(hVal,message,__h245(forwardLogicalChannelNumber),&lcn,NULL);

        rejMessage=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,rejMessage, __h245(response),__h245(openLogicalChannelReject));
        pvtAdd(hVal,nodeId,__h245(forwardLogicalChannelNumber),lcn,NULL,NULL);
        pvtAddBranch2(hVal,nodeId,__h245(cause),__h245(unspecified));

        sendMessageH245((HCONTROL)ctrl, rejMessage, RV_TRUE, NULL);

        return RV_ERROR_UNKNOWN;
    }

    if(emaLock((EMAElement)channel))
    {
        channel->hCtrl = (HCONTROL)ctrl;
        channel->myLogicalChannelNum = (RvUint16)lcn;
        channel->bOrigin = RV_FALSE;

        if (app->cmMySessionEvent.cmEvCallNewChannel)
        {
            H245CBEnter((app->pAppObject, "cmEvCallNewChannel:IN: haCall=%p, hsCall=%p. hsChan=%p", emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),cmiGetByControl((HCONTROL)ctrl),channel));
            {

				//зЂвт code add  Use:Get dynamicRTPPayloadType
				RvPstFieldId	nField;
				RvInt32			nDynamicRTPPayloadType=0;
				RvBool			bString;
				RvInt32			nRet=-1; 


                HAPPCHAN haChan=NULL;
                nesting = emaPrepareForCallback((EMAElement)channel);
                
				//зЂвт code add  Use:Get dynamicRTPPayloadType
#if (RV_H245_SUPPORT_H225_PARAMS == 1)
				nRet=pvtGetByPath(hVal,message,"forwardLogicalChannelParameters.multiplexParameters.h2250LogicalChannelParameters.dynamicRTPPayloadType",&nField,&nDynamicRTPPayloadType,&bString);
				if( (nRet>=0) && (nDynamicRTPPayloadType>0) )
					channel->data.h225Params.dynamicPayloadNumber = nDynamicRTPPayloadType;                              
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */				
				
				app->cmMySessionEvent.cmEvCallNewChannel((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)), (HCALL)cmiGetByControl((HCONTROL)ctrl),(HCHAN)channel, &haChan);
                emaReturnFromCallback((EMAElement)channel, nesting);
                emaSetApplicationHandle((EMAElement)channel,(void*)haChan);
            }
            H245CBExit((app->pAppObject, "cmEvCallNewChannel:IN: haChan=%p.", emaGetApplicationHandle((EMAElement)channel)));
        }

        incomingChannelMessage(app, channel, base);

        if (!emaWasDeleted((EMAElement)channel))
        {
            RvStatus status;
            status = openLogicalChannelEvent(app, hVal, channel, message);
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
#if (RV_3G324M_USE_MONA == RV_YES)
            if (status == RV_OK)
            {
                /*check if we have a mpcChannel to remove*/
				if (ctrl->bMPCinUse == RV_TRUE && 
                    (channel->data.h223Params.eDataType == cmChannelDataTypeAudio ||
                    channel->data.h223Params.eDataType == cmChannelDataTypeVideo))
                {
					/*look for a MPC channel with the same type (video/audio)*/
                    H245Channel  *current = (H245Channel *)ctrl->hFirstChannel;
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
                }
				/*remove the MPC channel and notify the application that the 
                channel is in connected state*/
				if (mpcChannel != NULL)
                {
					RvLogInfo(&app->log, (&app->log, "openLogicalChannel: Removing MPC H245 channel, lcn = %d ", channel->myLogicalChannelNum));
					emaLock((EMAElement)mpcChannel);
					if (mpcChannel->pNext)
						mpcChannel->pNext->pPrev = mpcChannel->pPrev;
					if (mpcChannel->pPrev)
						mpcChannel->pPrev->pNext = mpcChannel->pNext;
                    else
						cmH245SetChannelList((HCONTROL)ctrl, (HCHAN)mpcChannel->pNext);
					emaDelete((EMAElement)mpcChannel);
					emaUnlock((EMAElement)mpcChannel);
                    /*notify the application that the new channel is in connected state*/
                    notifyChannelState(channel, cmChannelStateConnected, cmChannelStateModeOn);
                }
            }
#endif /* USE_MONA */
#endif /* #if (RV_H245_SUPPORT_H223_PARAMS == 1)  */
        }

        emaUnlock((EMAElement)channel);
    }
    return RV_TRUE;
}



int openLogicalChannelAck(H245Control *ctrl, int base, int message)
{
    RvInt32 lcn;
    HPVT hVal;
    H245Object* app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    H245Channel* channel=NULL;

    if (!app) return RV_ERROR_UNKNOWN;
    hVal=app->hVal;

    pvtGetChildByFieldId(hVal,message,__h245(forwardLogicalChannelNumber),&lcn,NULL);

    channel = getOutChanByLCN((HCONTROL)ctrl, lcn);
    if (!channel)
        return RV_TRUE;

    if (emaLock((EMAElement)channel))
    {
        incomingChannelMessage(app, channel, base);

        switch(channel->eState)
        {
            case ChannelStateReleased:
            case ChannelStateIdle:
                RvLogInfo(&app->log,
                    (&app->log,"H245Channel %p in released state. Message discarded", channel));
                break;

            case ChannelStateAwaitingEstablishment:
            {
                /* complete dependent channel establishment*/
                channel->eState=ChannelStateEstablished;
/*--*/
                h22xFunctions[ctrl->eParamsType].openLogicalChannelAck(app, hVal, channel, ctrl, message);
/*--*/
                if (!emaWasDeleted((EMAElement)channel))
                {
                    ctrl->conflictChannels--;

                    if (channel->bIsDuplex)
                    { /* send confirm message. */
                        int confMsg = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);
                        int nodeId = pvtAddBranch2(hVal, confMsg, __h245(indication), __h245(openLogicalChannelConfirm));
                        pvtAdd(hVal, nodeId, __h245(forwardLogicalChannelNumber), channel->myLogicalChannelNum, NULL, NULL);

                        sendMessageH245Chan((HCONTROL)ctrl, (HCHAN)channel, confMsg, RV_TRUE, NULL);
                    }
                }
                break;
            }

            default:
                RvLogWarning(&app->log,
                    (&app->log, "H245Channel %p in %d state. Message discarded", channel, channel->eState));
                break;
        }

        emaUnlock((EMAElement)channel);
    }
    return RV_TRUE;
}

int openLogicalChannelReject(H245Control *ctrl, int base, int message)
{
    int                  status = 0;
    RvInt32              lcn;
    HPVT                 hVal;
    H245Object           *app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    H245Channel          *channel = NULL;
    cmChannelStateMode_e stateMode = cmChannelStateModeDisconnectedRemote;
    RvInt32              rejectReason = 0;

    if (!app) return RV_ERROR_UNKNOWN;
    hVal=app->hVal;

    pvtGetChildByFieldId(hVal,message,__h245(forwardLogicalChannelNumber),&lcn,NULL);
    channel = getOutChanByLCN((HCONTROL)ctrl, lcn);
    if (!channel) return RV_TRUE;

    /* Get the reject reason */
    status = pvtGetChildByFieldId(hVal, message, __h245(cause), &rejectReason, NULL);
    if (status >= 0)
    {
        channel->eRejectReason = (cmRejectLcnReason)ConvertFieldId2RejectReason(rejectReason);
/*--*/
        h22xFunctions[ctrl->eParamsType].openLogicalChannelReject(app, channel, ctrl, message,channel->eRejectReason, &stateMode);
/*--*/
    }

    if(emaLock((EMAElement)channel))
    {
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &channel->pTimer);

        incomingChannelMessage(app, channel, base);

        if (channel->eState==ChannelStateAwaitingEstablishment)
        {
            ctrl->conflictChannels--;
            {
                void* ptr=NULL;
                H245Channel* dependent;
                dependent=getNextOutChanByBase((HCONTROL)ctrl,channel,&ptr);
                while(dependent != NULL)
                {/* release the dependent channels because the base is rejected */
                    dependent->eState=ChannelStateReleased;
                    notifyChannelState(dependent,cmChannelStateDisconnected,stateMode);
                    notifyChannelState(dependent,cmChannelStateIdle, cmChannelStateModeOff);
                    dependent=getNextOutChanByBase((HCONTROL)ctrl,channel,&ptr);
                }
            }
        }
        channel->eState=ChannelStateReleased;
        notifyChannelState(channel,cmChannelStateDisconnected,stateMode);
        notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);

        emaUnlock((EMAElement)channel);
    }

    return RV_TRUE;
}

int openLogicalChannelConfirm(H245Control *ctrl, int base, int message)
{
    RvInt32 lcn;
    HPVT hVal;
    H245Object* app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    H245Channel* channel=NULL;

    if (!app) return RV_ERROR_UNKNOWN;
    hVal=app->hVal;

    pvtGetChildByFieldId(hVal,message,__h245(forwardLogicalChannelNumber),&lcn,NULL);
    channel = getInChanByLCN((HCONTROL)ctrl, lcn);

    if (!channel) return RV_TRUE;
    if(emaLock((EMAElement)channel))
    {
        incomingChannelMessage(app, channel, base);

        switch(channel->eState)
        {
            case ChannelStateAwaitingEstablishment:
                channel->eState=ChannelStateReleased;
                notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedReasonUnknown);
                notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);

            break;
            case ChannelStateAwaitingConfirmation:
                channel->eState=ChannelStateEstablished;
/*--*/
                h22xFunctions[ctrl->eParamsType].openLogicalChannelConfirm(hVal, channel, message);
/*--*/
            break;
            default:
                break;
        }
        emaUnlock((EMAElement)channel);
    }
    return RV_TRUE;
}


int closeLogicalChannel(H245Control *ctrl, int base, int message)
{
    RvInt32 lcn;
    HPVT hVal;
    H245Object* app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    H245Channel* channel=NULL;
    RvPstFieldId reason;
    cmChannelStateMode_e stateMode=cmChannelStateModeDisconnectedReasonUnknown;
    int rmessage, nodeId, res = RV_ERROR_UNKNOWN;

    if (!app) return RV_ERROR_UNKNOWN;
    hVal=app->hVal;

    pvtGetChildByFieldId(hVal,message,__h245(forwardLogicalChannelNumber),&lcn,NULL);
    channel = getInChanByLCN((HCONTROL)ctrl, lcn);

    if (channel == NULL)
    {
        /* do a quick check if this may be one of the "frozen" channels */
        int inLcn = 0;
        int foundOlc = -1;

        if (RV_PVT_NODEID_IS_VALID(ctrl->incomingOLCs[0]))
        {
            __pvtGetByFieldIds(nodeId, hVal, ctrl->incomingOLCs[0],
                { _anyField _anyField _h245(forwardLogicalChannelNumber) LAST_TOKEN }, NULL, &inLcn, NULL);
            if (RV_PVT_NODEID_IS_VALID(nodeId) && (inLcn == lcn))
            {
                foundOlc = 0;
            }
        }
        if ((foundOlc < 0) && (RV_PVT_NODEID_IS_VALID(ctrl->incomingOLCs[1])))
        {
            __pvtGetByFieldIds(nodeId, hVal, ctrl->incomingOLCs[1],
                { _anyField _anyField _h245(forwardLogicalChannelNumber) LAST_TOKEN }, NULL, &inLcn, NULL);
            if (RV_PVT_NODEID_IS_VALID(nodeId) && (inLcn == lcn))
            {
                foundOlc = 1;
            }
        }
        if (foundOlc >= 0)
        {
            /* the CLC cancels out this OLC, so send a CLC ACK on it, than delete it */
            rmessage = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);
            nodeId = pvtAddBranch2(hVal, rmessage, __h245(response), __h245(closeLogicalChannelAck));
            pvtAdd(hVal,nodeId, __h245(forwardLogicalChannelNumber), lcn, NULL, NULL);
            res = sendMessageH245((HCONTROL)ctrl, rmessage, RV_TRUE, NULL);

            pvtDelete(hVal, ctrl->incomingOLCs[foundOlc]);
            ctrl->incomingOLCs[foundOlc] = RV_PVT_INVALID_NODEID;
            return RV_OK;
        }
        /* ok, we really don't know anything about this CLC. ignore it */
        return RV_TRUE;
    }

    if(emaLock((EMAElement)channel))
    {
        incomingChannelMessage(app, channel, base);

        rmessage=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,rmessage, __h245(response),__h245(closeLogicalChannelAck));
        pvtAdd(hVal,nodeId,__h245(forwardLogicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
        res = sendMessageH245Chan((HCONTROL)ctrl, (HCHAN)channel, rmessage, RV_FALSE, NULL);
        pvtGet(hVal,pvtChild(hVal,pvtGetChild(hVal,message,__h245(reason),NULL)),&reason,NULL,NULL,NULL);
        pvtDelete(hVal, rmessage);

        switch (reason)
        {
            case __h245(unknown)              :stateMode =cmChannelStateModeDisconnectedReasonUnknown;            break;
            case __h245(reopen)               :stateMode =cmChannelStateModeDisconnectedReasonReopen;             break;
            case __h245(reservationFailure)   :stateMode =cmChannelStateModeDisconnectedReasonReservationFailure; break;
            default: break;
        }
        channel->eState=ChannelStateReleased;
        notifyChannelState(channel,cmChannelStateDisconnected,stateMode);
        notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);

        emaUnlock((EMAElement)channel);
    }

    return res;
}


int closeLogicalChannelAck(H245Control *ctrl, int base, int message)
{
    RvInt32 lcn;
    HPVT hVal;
    H245Object* app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    H245Channel* channel=NULL;

    if (!app) return RV_ERROR_UNKNOWN;
    hVal=app->hVal;

    pvtGetChildByFieldId(hVal,message,__h245(forwardLogicalChannelNumber),&lcn,NULL);
    channel=getOutChanByLCN((HCONTROL)ctrl, lcn);
    if (!channel) return RV_TRUE;

    if(emaLock((EMAElement)channel))
    {
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &channel->pTimer);

        incomingChannelMessage(app, channel, base);

        switch(channel->eState)
        {
            case ChannelStateEstablished:
            case ChannelStateAwaitingRelease:
                channel->eState = ChannelStateReleased;
                notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
                notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
            break;
            default:
                break;
        }
        channel->eState=ChannelStateReleased;
        emaUnlock((EMAElement)channel);
    }

    return RV_TRUE;
}



int requestChannelClose(H245Control *ctrl, int base, int message)
{
    RvInt32 lcn;
    HPVT hVal;
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    H245Object* app=(H245Object*)hApp;
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;
    hVal=app->hVal;

    pvtGetChildByFieldId(hVal,message,__h245(forwardLogicalChannelNumber),&lcn,NULL);
    channel=getOutChanByLCN((HCONTROL)ctrl, lcn);
    if (!channel) return RV_TRUE;
    if(emaLock((EMAElement)channel))
    {
        incomingChannelMessage(app, channel, base);

        channel->requestCloseParamID=message;

        if (app->cmMyChannelEvent.cmEvChannelRequestCloseStatus != NULL)
        {
            H245CBEnter((app->pAppObject, "cmEvChannelRequestCloseStatus: haChan=%p, hsChan=%p, status=request.",(HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), channel));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelRequestCloseStatus((HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmRequestCloseRequest);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelRequestCloseStatus."));
        }

        channel->requestCloseParamID=RV_ERROR_UNKNOWN;
        emaUnlock((EMAElement)channel);
    }
    return RV_TRUE;
}

int requestChannelCloseAck(H245Control *ctrl, int base, int message)
{
    int lcn;
    HPVT hVal;
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    H245Object* app=(H245Object*)hApp;
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;
    hVal=app->hVal;

    pvtGetChildByFieldId(hVal,message,__h245(forwardLogicalChannelNumber),&lcn,NULL);
    channel=getInChanByLCN((HCONTROL)ctrl,lcn);
    if (!channel) return RV_TRUE;

    if(emaLock((EMAElement)channel))
    {
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &channel->pRequestCloseTimer);

        incomingChannelMessage(app, channel, base);

        if (app->cmMyChannelEvent.cmEvChannelRequestCloseStatus != NULL)
        {
            H245CBEnter((app->pAppObject, "cmEvChannelRequestCloseStatus: haChan=%p, hsChan=%p, status=confirm.",(HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), channel));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelRequestCloseStatus((HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmRequestCloseConfirm);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelRequestCloseStatus."));
        }

        emaUnlock((EMAElement)channel);
    }

    return RV_TRUE;
}

int requestChannelCloseReject(H245Control *ctrl, int base, int message)
{
    int lcn;
    HPVT hVal;
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    H245Object* app=(H245Object*)hApp;
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;
    hVal=app->hVal;

    pvtGetChildByFieldId(hVal,message,__h245(forwardLogicalChannelNumber),&lcn,NULL);
    channel=getInChanByLCN((HCONTROL)ctrl,lcn);

    if (!channel) return RV_TRUE;

    if(emaLock((EMAElement)channel))
    {
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &channel->pRequestCloseTimer);

        incomingChannelMessage(app, channel, base);

        if (app->cmMyChannelEvent.cmEvChannelRequestCloseStatus != NULL)
        {
            H245CBEnter((app->pAppObject, "cmEvChannelRequestCloseStatus: haChan=%p, hsChan=%p, status=reject.",(HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), channel));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelRequestCloseStatus((HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmRequestCloseReject);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelRequestCloseStatus."));
        }

        emaUnlock((EMAElement)channel);
    }

    return RV_TRUE;
}

int requestChannelCloseRelease(H245Control *ctrl, int base, int message)
{
    RvInt32 lcn;
    HPVT hVal;
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    H245Object* app=(H245Object*)hApp;
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;
    hVal=app->hVal;

    pvtGetChildByFieldId(hVal,message,__h245(forwardLogicalChannelNumber),&lcn,NULL);
    channel=getOutChanByLCN((HCONTROL)ctrl, lcn);

    if (!channel) return RV_TRUE;

    if(emaLock((EMAElement)channel))
    {
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &channel->pRequestCloseTimer);

        incomingChannelMessage(app, channel, base);

        if (app->cmMyChannelEvent.cmEvChannelRequestCloseStatus != NULL)
        {
            H245CBEnter((app->pAppObject, "cmEvChannelRequestCloseStatus: haChan=%p, hsChan=%p, status=reject.",(HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), channel));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelRequestCloseStatus((HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmRequestCloseReject);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelRequestCloseStatus."));
        }
        emaUnlock((EMAElement)channel);
    }

    return RV_TRUE;
}

/*========================================A P I ====================================================================*/

RVAPI int RVCALLCONV /* Real number of channels in configuration or RV_ERROR_UNKNOWN */
cmGetConfigChannels(
/* build array containing the channel names as appear in
the configuration. The strings are copied into array elements */
                    IN  HAPP        hApp,
                    IN  int         arraySize,
                    IN  int         elementLength, /* sizeof each string in array */
                    OUT char*       array[] /* allocated with elements */
)
{
    int result;
    H245Object* app=(H245Object*)cmiGetH245Handle(hApp);

    if (hApp == NULL)
        return RV_ERROR_NULLPTR;

    H245APIEnter((app->pAppObject, "cmGetConfigChannels: hApp=%p", hApp));
    result = confGetChannelNames(app->hVal, app->h245Conf, arraySize, elementLength, array);
    H245APIExit((app->pAppObject, "cmGetConfigChannels=%d", result));

    return result;
}

/******************************************************************************
 * cmGetConfigChannelDataType
 * ----------------------------------------------------------------------------
 * General: Get the DataType PVT sub tree of a specified channel that is
 *          defined in configuration.
 *
 * Return Value: Non negative if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - The handle to the stack.
 *         channelName      - The channel name from configuration.
 * Output: dataTypeNodeId   - The DataType node id of the channel. It is the
 *                            responsibility of the user to delete this node id
 *                            if the function succeeds.
 *****************************************************************************/
RVAPI int RVCALLCONV
cmGetConfigChannelDataType(
             IN    HAPP            hApp,
             IN    const RvChar    *channelName,
             OUT   RvPvtNodeId     *dataTypeNodeId)
{
    HPVT        hVal;
    int         status = RV_OK;
    H245Object  *app = (H245Object*)cmiGetH245Handle(hApp);
    RvBool      isDynamic = RV_FALSE;
    RvPvtNodeId nodeId = -1;

    if (!channelName || !hApp)
    {
        return RV_ERROR_UNKNOWN;
    }

    H245APIEnter((app->pAppObject, "cmGetConfigChannelDataType: hApp=%p, channelName=%s",
        hApp, nprn(channelName)));

    hVal = app->hVal;
    nodeId = pvtAddRoot(hVal, app->h245DataType, 0, NULL);
    if (nodeId<0)
    {
        H245APIExit((app->pAppObject, "cmGetConfigChannelDataType: return %d", nodeId));
        return nodeId;
    }

    status = confGetDataType(hVal, cmGetH245ConfigurationHandle((HAPP)app->pAppObject), channelName, nodeId, &isDynamic, RV_FALSE);
    if (status < RV_OK)
    {
        pvtDelete(hVal, nodeId);
    }
    *dataTypeNodeId = nodeId;
    H245APIExit((app->pAppObject, "cmGetConfigChannelDataType=%d", status));
    return status;
}

/************************************************************************
 * cmBuildH245DataTypeNode
 * purpose: build a H.245 dataType node according to the given data type name
 * input  : hApp            - Stack handle for the application
 *          dataType        - Channel type name
 *          nonH235         - If true means remove h235Media.mediaType level
 * output : dataTypeNode    - The dataType node supplied to the application.
 *                            The application is responsible to delete this
 *                            node.
 * return : RV_OK on success, other on failure.
 ************************************************************************/
RVAPI RvStatus RVCALLCONV cmBuildH245DataTypeNode(
    IN  HAPP            hApp,
    IN  const RvChar    *dataType,
    IN  RvBool          nonH235,
    OUT RvPvtNodeId     *dataTypeNode)
{
    HPVT        hVal;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);
    RvBool     isDynamicPayLoad;
    RvStatus   status = RV_OK;

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmBuildH245DataTypeNode: hApp=%p", hApp));

    hVal = app->hVal;

    /*create dataType node*/
    *dataTypeNode = pvtAddRoot(hVal, app->h245DataType, 0, NULL);

    if (confGetDataType(hVal, app->h245Conf, dataType, *dataTypeNode, &isDynamicPayLoad, nonH235) < 0)
    {
        pvtDelete(hVal, *dataTypeNode);
        status = RV_ERROR_UNKNOWN;
    }

    H245APIExit((app->pAppObject, "cmBuildH245DataTypeNode=%d", status));

    return status;
}

/************************************************************************
 * cmChannelOpen
 * purpose: Opens a new logical channel using the definitions in the
 *          Stack Configuration
 * input  : hsChan              - H245Channel to open
 *          channelName         - Name of the Stack Configuration settings
 *                                entry for the channel to be opened
 *          hsChanSameSession   - Stack handle for channel that belongs to
 *                                the same session (may be NULL)
 *          hsChanAssociated    - Stack handle of outgoing channel that is
 *                                to be associated with the newly created one
 *                                (may be NULL).
 *          rate                - Unused. Reserved for backward compatibility
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV
cmChannelOpen(
          IN    HCHAN           hsChan,
          IN    const RvChar*   channelName,
          IN    HCHAN           hsChanSameSession,
          IN    HCHAN           hsChanAssociated,
          IN    RvUint32        rate)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel* channel=(H245Channel*)hsChan;
    HPVT hVal;
    H245Object *app;

    RvBool isDynamic=RV_FALSE;
    int nodeId=-1, ret=-1;

    RV_UNUSED_ARG(rate);

    if (!channelName || !channel || !hApp) return RV_ERROR_UNKNOWN;

    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelOpen: hsChan=%p, channelName=%s, hsChanSameSession=%p, hsChanAssociated=%p",
        hsChan, nprn(channelName), hsChanSameSession, hsChanAssociated));

    hVal = app->hVal;
    nodeId = pvtAddRoot(hVal, app->h245DataType, 0, NULL);
    if (nodeId<0)
    {
        H245APIExit((app->pAppObject, "cmChannelOpen: return %d",nodeId));
        return nodeId;
    }

    if (emaLock((EMAElement)hsChan))
    {
        confGetDataType(hVal, cmGetH245ConfigurationHandle((HAPP)app->pAppObject), channelName, nodeId, &isDynamic, RV_FALSE);
        ret = cmChannelOpenDynamic(hsChan, pvtChild(hVal, nodeId), hsChanSameSession, hsChanAssociated, isDynamic);
        pvtDelete(hVal, nodeId);
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelOpen=%d", ret));
    return ret;
}


RVAPI int RVCALLCONV
cmChannelSetHandle(
           /* let haChan be the new application handle of this channel */
           IN HCHAN                    hsChan,
           IN HAPPCHAN                 haChan)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel* channel=(H245Channel*)hsChan;
    H245Object *app;

    if (!channel || !hApp)   return RV_ERROR_UNKNOWN;

    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelSetHandle: hsChan=%p, haChan=%p.", hsChan, haChan));
    emaSetApplicationHandle((EMAElement)channel,(void*)haChan);
    H245APIExit((app->pAppObject, "cmChannelSetHandle=%d", 0));
    return 0;
}


RVAPI int RVCALLCONV
cmChannelAnswer(
        /*
           - Acknowledge incoming channel request.
           - Acknowledge outgoing channel close request.
        */
        IN  HCHAN       hsChan
        )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    HPVT hVal;
    H245Object*app=(H245Object*)hApp;
    int message, olcaID, nodeId;
    RvStatus status = RV_ERROR_UNKNOWN;

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelAnswer: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        HCONTROL ctrl;
        H245Control* ctrlE;
        H245Channel* channel=(H245Channel*)hsChan;
        hVal = app->hVal;
        ctrl = channel->hCtrl;
        ctrlE = (H245Control*)ctrl;
        if (channel->eState==ChannelStateEstablished && channel->bOrigin)
        {
            message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
            __pvtBuildByFieldIds(nodeId,hVal,message, {_h245(response) _h245(requestChannelCloseAck) _h245(forwardLogicalChannelNumber) LAST_TOKEN},
                                 channel->myLogicalChannelNum,NULL);
            if (nodeId >= 0)
                status = sendMessageH245Chan((HCONTROL)ctrl, hsChan, message, RV_FALSE, NULL);
            else
                status = nodeId;
            pvtDelete(hVal,message);
        }
        else if (!channel->bOrigin)
        {
            if (channel->eState==ChannelStateAwaitingEstablishment)
            {
                RvBool sentOnDummyHost = RV_FALSE;

                message = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);

                olcaID=pvtAddBranch2(hVal,message, __h245(response), __h245(openLogicalChannelAck));
/*--*/
                status = h22xFunctions[ctrlE->eParamsType].channelAnswer(hVal, channel, ctrlE, olcaID);
/*--*/
                if (status == RV_OK)
                {
                    pvtAdd(hVal,olcaID, __h245(forwardLogicalChannelNumber), channel->myLogicalChannelNum,NULL,NULL);
                    status = sendMessageH245Chan((HCONTROL)ctrl, hsChan, message, RV_FALSE, &sentOnDummyHost);
                }
                pvtDelete(hVal,message);

                if (status == RV_OK)
                {
                    if (channel->bIsDuplex)
                    { /* B-LCSE */
                        if (!sentOnDummyHost)
                        {
                            RvInt32 timeout=29;
                            pvtGetChildByFieldId(hVal,app->h245Conf,__h245(channelsTimeout),&(timeout),NULL);
                            app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &channel->pTimer);
                            channel->pTimer = app->evHandlers.pfnTimerStartEv((HCONTROL)ctrl,
                                channelTimeoutEventsHandler, (void*)channel, timeout * 1000);
                        }
                        channel->eState = ChannelStateAwaitingConfirmation;
                    }
                    else  /* LCSE */
                        channel->eState=ChannelStateEstablished;
                }
            }

            if (!channel->bOrigin && !channel->bIsDuplex && (status == RV_OK))/* simplex connection */
                /*  ==> connected */
                notifyChannelState(channel,cmChannelStateConnected, cmChannelStateModeOn);
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelAnswer=%d", status));
    return status;
}


RVAPI int RVCALLCONV
cmChannelDrop(
          /*
        - Close outgoing channel
        - Reject incoming channel open request
        - New: Request to close incoming channel
          */
          IN    HCHAN       hsChan
          )
{
    return cmChannelDropReason(hsChan,cmCloseReasonUnknown);
}


RVAPI int RVCALLCONV
cmChannelClose(
           IN   HCHAN       hsChan)
{
    H245Object* app=(H245Object *)emaGetUserData((EMAElement)hsChan);
    if (!app) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelClose: hsChan=%p.", hsChan));

#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
    if (app->mibEvent.h341DeleteLogicalChannel)
          app->mibEvent.h341DeleteLogicalChannel(app->mibHandle,hsChan);
#endif

    if (emaLock((EMAElement)hsChan))
    {
        H245Channel* channel=(H245Channel*)hsChan;
        H245Control* ctrl;

        ctrl = (H245Control *)channel->hCtrl;
        {
            H245Channel* someChannel;
            void* ptr=NULL;

            /* Make sure nobody will remember us after death */
            someChannel = getNextChan((HCONTROL)ctrl, &ptr);
            while (someChannel != NULL)
            {
                if(emaLock((EMAElement)someChannel))
                {
#if (RV_H245_SUPPORT_H225_PARAMS == 1)
                    if (someChannel->eType == RvH245ChannelParamsH225)
                    {
                        if (someChannel->data.h225Params.pAssociated == channel)
                            someChannel->data.h225Params.pAssociated = NULL;
                        if (someChannel->data.h225Params.pPartner == channel)
                            someChannel->data.h225Params.pPartner = NULL;
                    }
#endif /* (RV_H245_SUPPORT_H225_PARAMS == 1) */
                    if (someChannel->pBase == channel)
                        someChannel->pBase = NULL;
                    if (someChannel->pReplacementCh == channel)
                        someChannel->pReplacementCh = NULL;
                    emaUnlock((EMAElement)someChannel);
                }
                someChannel = getNextChan((HCONTROL)ctrl, &ptr);
            }

        }

/*--*/
        h22xFunctions[ctrl->eParamsType].ChannelClose(ctrl, channel);
/*--*/

        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &channel->pTimer);
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &channel->pRequestCloseTimer);

        channelFreeMemory(hsChan);

        /* Fix the linked list of channels by deleting this one from its neighbors */
        if (channel->pNext)
            channel->pNext->pPrev = channel->pPrev;
        if (channel->pPrev)
            channel->pPrev->pNext = channel->pNext;
        else
            cmH245SetChannelList((HCONTROL)ctrl, (HCHAN)channel->pNext);

        emaDelete((EMAElement)hsChan);
        emaUnlock((EMAElement)hsChan);
    }
    H245APIExit((app->pAppObject, "cmChannelClose=1"));
    return RV_TRUE;
}





/* cmChannelSetAddress
   cmChannelSetRTCPAddress */


RVAPI int RVCALLCONV
cmChannelGetOrigin(
           IN   HCHAN       hsChan,
           OUT  RvBool*     origin)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int ori=RV_ERROR_UNKNOWN;
    H245Object *app;

    if (!channel)return RV_ERROR_UNKNOWN;
    if (!hApp) return RV_ERROR_UNKNOWN;

    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelGetOrigin: hsChan=%p, origin=%p.", hsChan, origin));

    if (emaLock((EMAElement)hsChan))
    {
        ori = channel->bOrigin;
        emaUnlock((EMAElement)hsChan);
    }
    if (origin)
        *origin = (RvBool)ori;
    H245APIExit((app->pAppObject, "cmChannelGetOrigin: origin=%d. [1]", ori));
    return ori;
}




/* -- Duplex channels -- */

RVAPI
int RVCALLCONV cmChannelDuplex(
                 /* declare channel as duplex. Meaning full duplex. Composed of
                two uni-directional channels in opposite directions. */
        IN      HCHAN               hsChan)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object *app;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;

    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelDuplex: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        channel->bIsDuplex=1;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelDuplex=1"));
    return RV_TRUE;
}

RVAPI
int RVCALLCONV cmChannelIsDuplex(
                   /* Returns RV_TRUE if channel is duplex. RV_FALSE if not. and negative value
                  in case of error */
         IN     HCHAN               hsChan)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int isDuplex=RV_ERROR_UNKNOWN;
    H245Object *app;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelIsDuplex: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        isDuplex = channel->bIsDuplex;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelIsDuplex=%d", isDuplex));
    return isDuplex;
}


RVAPI int RVCALLCONV
cmChannelGetDependency(
             IN     HCHAN           hsChan,
             OUT    HAPPCHAN*       haBaseChannel,
             OUT    HCHAN*          hsBaseChannel
             )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object *app;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelGetDependency: hsChan=%p, haBaseChannel=%p, hsBaseChannel=%p.", hsChan, haBaseChannel, hsBaseChannel));

    if (emaLock((EMAElement)hsChan))
    {
        if (channel->pBase)
        {
            if (hsBaseChannel)  *hsBaseChannel=(HCHAN)channel->pBase;
            if (haBaseChannel && hsBaseChannel && *hsBaseChannel)  *haBaseChannel=(HAPPCHAN)emaGetApplicationHandle((EMAElement)channel->pBase);
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelGetDependency: ha=%p hs=%p [%d].", (haBaseChannel)?*haBaseChannel:0, (hsBaseChannel)?*hsBaseChannel:0, 0));
            return 0;
        }
        else
        {
            if (haBaseChannel) *haBaseChannel=NULL;
            if (hsBaseChannel) *hsBaseChannel=NULL;
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelGetDependency: channel is independent"));
            return 0;
        }
    }
    H245APIExit((app->pAppObject, "cmChannelGetDependency: channel does not exist"));
    return RV_ERROR_UNKNOWN;
}

RVAPI int RVCALLCONV
cmIsChannelReplace(
             IN     HCHAN           hsChan,
             OUT    HAPPCHAN*       haReplaceChannel,
             OUT    HCHAN*          hsReplaceChannel
             )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object *app;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelReplace: hsChan=%p, haReplaceChannel=%p, hsReplaceChannel=%p.", hsChan, haReplaceChannel, hsReplaceChannel));

    if (emaLock((EMAElement)hsChan))
    {
        if (channel->pReplacementCh)
        {
            if (hsReplaceChannel)  *hsReplaceChannel=(HCHAN)channel->pReplacementCh;
            if (haReplaceChannel && hsReplaceChannel && *hsReplaceChannel)  *haReplaceChannel=(HAPPCHAN)emaGetApplicationHandle((EMAElement)channel->pReplacementCh);
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelReplace: ha=%p hs=%p [%d].", (haReplaceChannel)?*haReplaceChannel:0, (hsReplaceChannel)?*hsReplaceChannel:0, 0));
            return 1;
        }
        else
        {
            if (haReplaceChannel) *haReplaceChannel=NULL;
            if (hsReplaceChannel) *hsReplaceChannel=NULL;
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelReplace: "));
            return 0;
        }
    }
    H245APIExit((app->pAppObject, "cmChannelReplace: channel does not exist"));
    return RV_ERROR_UNKNOWN;
}




/*
  add to events to out open log channels proc
  --closeBaseIndication when base channel have got releaserequest .
  -- noBaseChannel when base channel is not exist or have ot releaseRequest or timeout before
    establish proc start.
*/

RVAPI int RVCALLCONV
cmChannelSetDependency(
        IN      HCHAN               hsChan,
        IN      HCHAN               hsChanBase     )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object *app;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelSetDependency: hsChan=%p, hsChanBase=%p", hsChan, hsChanBase));

    if (emaLock((EMAElement)hsChan))
    {
        channel->pBase=(H245Channel*)hsChanBase;
        emaUnlock((EMAElement)hsChan);
    }
    H245APIExit((app->pAppObject, "cmChannelSetDependency=1"));
    return RV_TRUE;
}


RVAPI int RVCALLCONV
cmChannelReplace(
         IN      HCHAN               hsChan,
         IN      HCHAN               hsChanReplace     )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object *app;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelReplace: hsChan=%p, hsChanReplace=%p", hsChan, hsChanReplace));

    if (emaLock((EMAElement)hsChan))
    {
        channel->pReplacementCh=(H245Channel*)hsChanReplace;
        emaUnlock((EMAElement)hsChan);
    }
    H245APIExit((app->pAppObject, "cmChannelReplace=1"));
    return RV_TRUE;
}





static const int dropReasons[]={__h245(unknown),__h245(reopen),__h245(reservationFailure),__h245(normal)};

RVAPI int RVCALLCONV
cmChannelDropReason(
          /*
        - Close outgoing channel
        - Reject incoming channel open request
        - New: Request to close incoming channel
          */
          IN  HCHAN            hsChan,
          IN  cmCloseLcnReason reason
          )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)hApp;
    HPVT hVal;
    int message,nodeId;
    RvStatus res = RV_ERROR_UNKNOWN;

    if (!hApp || !channel || (RvUint32)reason>(RvUint32)cmCloseReasonNormal)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelDropReason(hsChan=%p, reason=%d)", hsChan, reason));

    if (emaLock((EMAElement)hsChan))
    {
        H245Control *ctrl = (H245Control *)channel->hCtrl;
        hVal = app->hVal;

        /* Make sure this isn't a faststart channel without any connected control */
        if (ctrl->eState == ctrlNotInitialized)
        {
            RvLogError(&app->log, (&app->log,
                "cmChannelDropReason: Control is not initialized (probably in faststart mode)"));
            res = RV_ERROR_UNINITIALIZED;
        }

        else if (channel->bOrigin)
        {
            if ((channel->eState==ChannelStateReleased) || (channel->eState==ChannelStateIdle))
            {
                notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
                notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
                emaUnlock((EMAElement)hsChan);
                H245APIExit((app->pAppObject, "cmChannelDropReason=1"));
                return RV_TRUE;
            }
            if (channel->eState==ChannelStateAwaitingRelease)
            {
                emaUnlock((EMAElement)hsChan);
                H245APIExit((app->pAppObject, "cmChannelDropReason=1; CLC already sent."));
                return RV_TRUE;
            }

            if (reason==cmCloseReasonNormal)
                reason = cmCloseReasonUnknown;
            if (channel->eState==ChannelStateAwaitingEstablishment)
                ctrl->conflictChannels--;
            message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
            nodeId=pvtAddBranch2(hVal,message, __h245(request),__h245(closeLogicalChannel));
            pvtAdd(hVal,nodeId,__h245(forwardLogicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
            pvtAddBranch2(hVal,nodeId,__h245(reason),dropReasons[reason]);
            pvtAddBranch2(hVal,nodeId,__h245(source),__h245(user));

            res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);

            if (res >= 0)
            {
                {        /* base channel released -- drop dependent channel*/
                    void* ptr=NULL;
                    H245Channel* dependent;
                    dependent = getNextOutChanByBase(channel->hCtrl,channel,&ptr);
                    while (dependent != NULL)
                    {
                        cmChannelDrop((HCHAN)dependent);
                        dependent = getNextOutChanByBase(channel->hCtrl,channel,&ptr);
                    }
                }
                {
                    RvInt32 timeout=29;
                    pvtGetChildByFieldId(hVal,app->h245Conf,__h245(channelsTimeout),&(timeout),NULL);
                    app->evHandlers.pfnTimerCancelEv(channel->hCtrl, &channel->pTimer);
                    channel->pTimer = app->evHandlers.pfnTimerStartEv(channel->hCtrl,
                        channelTimeoutEventsHandler, (void*)channel, timeout*1000);
                }

                channel->eState=ChannelStateAwaitingRelease;
            }
        }
        else
        switch(channel->eState)
        {
            case ChannelStateAwaitingEstablishment:
                message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
                nodeId=pvtAddBranch2(hVal,message, __h245(response),__h245(openLogicalChannelReject));
                pvtAdd(hVal,nodeId,__h245(forwardLogicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
                pvtAddBranch2(hVal,nodeId,__h245(cause),__h245(unspecified));
                /* the following line just returns error   -Ran
                pvtAddBranch2(hVal,nodeId,__h245(source),__h245(user)); */

                res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);

                if (res >= 0)
                {
                    channel->eState=ChannelStateReleased;
                    notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
                    notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
                }
                break;
            case ChannelStateEstablished:
                res = cmChannelRequestClose(hsChan, reason, NULL);
                break;
            default:
                break;
        }

        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelDropReason=%d", res));
    return res;
}


static const int rejectReasons[]=
    {__h245(unspecified),__h245(unsuitableReverseParameters),__h245(dataTypeNotSupported),
    __h245(dataTypeNotAvailable),__h245(unknownDataType),__h245(dataTypeALCombinationNotSupported),
    __h245(multicastChannelNotAllowed),__h245(insufficientBandwidth),__h245(separateStackEstablishmentFailed),
    __h245(invalidSessionID),__h245(masterSlaveConflict),__h245(waitForCommunicationMode),
    __h245(invalidDependentChannel),__h245(replacementForRejected),__h245(securityDenied)};

/************************************************************************
 * cmChannelRejectReason
 * purpose: Reject incoming OpenLogicaChannel request
 * input  : hsChan      - H245Channel to check
 *          reason      - channel reject reason
 * return : Non-negative value on success
 *          Negative value on failure
 * remark : Created to allow all reasons for OLC rejection.
 *          cmChannelDropReason will still work using reason 'unspecified'
 ************************************************************************/
RVAPI int RVCALLCONV cmChannelRejectReason(
    IN  HCHAN            hsChan,
    IN  cmRejectLcnReason reason)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    H245Control* ctrl = (H245Control *)channel->hCtrl;
    H245Object* app=(H245Object*)hApp;
    HPVT hVal;
    int message,nodeId;

    if (!hApp || !channel || ((int)reason<(int)cmRejectReasonUnspecified) || ((int)reason>(int)cmRejectReasonSecurityDenied))
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelReject: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        if (channel->bOrigin)
        {
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelReject: [ERROR: OUTGOING CHANNEL]"));
            return RV_ERROR_UNKNOWN;
        }
        if(channel->eState != ChannelStateAwaitingEstablishment)
        {
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelReject: [ERROR: WRONG STATE]"));
            return RV_ERROR_UNKNOWN;
        }

        hVal = app->hVal;
        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(response),__h245(openLogicalChannelReject));
        pvtAdd(hVal,nodeId,__h245(forwardLogicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
        pvtAddBranch2(hVal,nodeId,__h245(cause),rejectReasons[reason]);

        sendMessageH245(channel->hCtrl, message, RV_TRUE, NULL);
        channel->eState=ChannelStateReleased;
        channel->eRejectReason = reason;
/*----*/
        h22xFunctions[ctrl->eParamsType].ChannelReject(ctrl, channel);
/*----*/
        notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
        notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);

        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelDrop=1"));
    return RV_TRUE;
}


RVAPI
RvInt32 RVCALLCONV cmChannelGetNumber(IN HCHAN hsChan)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int lcn=RV_ERROR_UNKNOWN;
    H245Object *app;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelGetNumber: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        lcn = channel->myLogicalChannelNum;
        emaUnlock((EMAElement)hsChan);
    }
    H245APIExit((app->pAppObject, "cmChannelGetNumber=%d", lcn));

    return lcn;
}


/*________________________________request channel close handling_____________________________________*/


RVAPI int RVCALLCONV
cmChannelRequestClose(
          /*
        - New: Request to close incoming channel
          */
            IN  HCHAN            hsChan,
            IN  cmCloseLcnReason reason,
            cmQosCapability*     cmQOSCapability
          )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel *channel=(H245Channel*)hsChan;
    H245Object* app=(H245Object*)hApp;
    HPVT hVal;
    int message, nodeId, res = RV_ERROR_UNKNOWN;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelRequestClose: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        H245Control *ctrl = (H245Control *)channel->hCtrl;
        hVal = app->hVal;

        /* Make sure this isn't a faststart channel without any connected control */
        if (ctrl->eState == ctrlNotInitialized)
        {
            RvLogError(&app->log, (&app->log,
                "cmChannelRequestClose: Control is not initialized (probably in faststart mode)"));
            res = RV_ERROR_UNINITIALIZED;
        }

        else
        {
            message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
            nodeId=pvtAddBranch2(hVal,message, __h245(request),__h245(requestChannelClose));
            pvtAdd(hVal,nodeId,__h245(forwardLogicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
            pvtAddBranch2(hVal,nodeId,__h245(reason),dropReasons[reason]);
            if (cmQOSCapability!=NULL)
            {
#if (RV_H245_LEAN_H223 == RV_NO)
                cmNonStandardParam *nonStandardPtr=NULL;
                cmRSVPParameters * rsvpParametersPtr=NULL;
                cmATMParameters  * atmParametersPtr=NULL;

                if (cmQOSCapability->cmQosCapabilityParamUsed & cmQosCapabilityNonStandard)
                    nonStandardPtr = &cmQOSCapability->cmNonStandard;
                if (cmQOSCapability->cmQosCapabilityParamUsed & cmQosCapabilityAtmParameters)
                    atmParametersPtr = &cmQOSCapability->cmAtmParameters;
                if (cmQOSCapability->cmQosCapabilityParamUsed & cmQosCapabilityRsvpParameters)
                    rsvpParametersPtr = &cmQOSCapability->cmRsvpParameters;
                buildQosCapability( hVal,pvtAdd(hVal, nodeId, __h245(qosCapability), 0, NULL, NULL),rsvpParametersPtr,atmParametersPtr,nonStandardPtr);
#endif
            }

            res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);

            if (res >= 0)
            {
                RvInt32 timeout=29;
                pvtGetChildByFieldId(hVal,app->h245Conf,__h245(requestCloseTimeout),&(timeout),NULL);
                app->evHandlers.pfnTimerCancelEv(channel->hCtrl, &channel->pRequestCloseTimer);
                channel->pRequestCloseTimer = app->evHandlers.pfnTimerStartEv(channel->hCtrl,
                    channelRC_TimeoutEventsHandler, (void*)channel, timeout*1000);
            }
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelRequestClose=%d", res));
    return res;
}

RVAPI int RVCALLCONV
cmChannelGetRequestCloseParam(  IN    HCHAN             hsChan,
                                OUT   cmCloseLcnReason  *reason,
                                INOUT cmQosCapability   *cmQOSCapability
                               )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    HPVT hVal;
    H245Object *app;

    if (!hApp || !channel || !channel->pRequestCloseTimer<0) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;
    H245APIEnter((app->pAppObject, "cmChannelGetRequestCloseParam: hsChan=%p haChan=%p", hsChan,emaGetApplicationHandle((EMAElement)channel)));

    if (emaLock((EMAElement)hsChan))
    {
        hVal = app->hVal;

#if (RV_H245_LEAN_H223 == RV_NO)
        getQosParameters(hVal,pvtGetChild(hVal,channel->requestCloseParamID,__h245(qosCapability), NULL),cmQOSCapability);
#else
        RV_UNUSED_ARG(cmQOSCapability);
#endif
        if (reason)
        {
            int iReason = pvtGetSyntaxIndex(hVal,pvtGetChild(hVal,channel->requestCloseParamID,__h245(reason), NULL))-1;
            *reason=(cmCloseLcnReason)iReason;
        }
        emaUnlock((EMAElement)hsChan);
    }
    H245APIExit((app->pAppObject, "cmChannelGetRequestCloseParam=1"));
    return RV_TRUE;
}

RVAPI int RVCALLCONV
cmChannelRequestCloseReject(
                /* Reject the request for closing the outgoing channel. */
                IN  HCHAN       hsChan
                )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel *channel=(H245Channel*)hsChan;
    HPVT hVal;
    H245Object* app=(H245Object*)hApp;
    int message, nodeId, res = RV_ERROR_UNKNOWN;

    if (!hApp || !channel) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelRequestCloseReject: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        hVal = app->hVal;

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(response),__h245(requestChannelCloseReject));
        pvtAdd(hVal,nodeId,__h245(forwardLogicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
        pvtAddBranch2(hVal,nodeId,__h245(cause),__h245(unspecified));

        res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelRequestCloseReject=%d", res));
    return res;
}


/******************************************************************************
 * cmChannelSetParam
 * ----------------------------------------------------------------------------
 * General: Obtains a specific channel parameter from the application.
 *          This function should be called after the call to cmChannelNew() and before the
 *          call to cmChannelOpen().
 *          See also cmChannelParam, cmChannelGetParam.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsChan      - The channel handle whose parameter is to be changed.
 *         eParam      - The parameter to be changed. The parameters are defined
 *                       in cmChannelParam enumeration.
 *         value       - If the parameter value is a simple integer value, this
 *                       is the value itself.
 *         strValue    - If the parameter value is a string, strValue
 *                       represents the parameter value.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmChannelSetParam(
         IN      HCHAN               hsChan,
         IN      cmChannelParam      eParam,
         IN      RvInt32             value,
         IN      const RvUint8       *strValue)
{
    H245Object  *app=(H245Object*)emaGetUserData((EMAElement)hsChan);
    RvStatus    status = RV_OK;
#if (RV_CHECK_MASK & RV_CHECK_RANGE) || (RV_H245_SUPPORT_H223_PARAMS == 1)
    H245Channel *channel=(H245Channel*)hsChan;
#endif

    RV_UNUSED_ARG(strValue);

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if ((app == NULL) || (channel == NULL))
        return RV_ERROR_NULLPTR;
#endif /* (RV_CHECK_MASK & RV_CHECK_NULL) */

    H245APIEnter((app->pAppObject, "cmChannelSetParam(hsChan=%p,eParam=%d,value=%d,strValue=%p",
        hsChan, eParam, value, strValue));

    if (emaLock((EMAElement)hsChan))
    {
#if (RV_CHECK_MASK & RV_CHECK_RANGE)
        /* Make sure we're handling the right type of channel with this parameter */
        switch (eParam)
        {
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
            case cmChannelParamH223AlLevel:
            case cmChannelParamH223Segmentable:
            case cmChannelParamH223Al1Framed:
            case cmChannelParamH223Al2SequenceNumber:
            case cmChannelParamH223Al3ControlFieldSize:
            case cmChannelParamH223Al3SendBufferSizeForRetransmissions:
            case cmChannelParamH223AudioType:
            case cmChannelParamH223BitRate:
            case cmChannelParamH223IgnoreCorruptedAlSdu:
            case cmChannelParamH223ReverseNullDataType:
#if (RV_H223_USE_MUX_LEVEL_3 == RV_YES)
            case cmChannelParamH223AlxMParameters:
#endif
            case cmChannelParamH223IsMPC:
                if (channel->eType != RvH245ChannelParamsH223)
                    status = RV_ERROR_BADPARAM;
                break;
#endif
#if (RV_H245_SUPPORT_H225_PARAMS == 1)
            case cmChannelParamH225LocalRtpAddress:
            case cmChannelParamH225LocalRtcpAddress:
            case cmChannelParamH225RemoteRtpAddress:
            case cmChannelParamH225RemoteRtcpAddress:
            case cmChannelParamH225DynamicRTPPayloadType:
            case cmChannelParamH225ForceSecondary:
            {
                if (channel->eType != RvH245ChannelParamsH225)
                    status = RV_ERROR_BADPARAM;
                break;
            }
#endif
            default:
                break;
        }
#endif /* (RV_CHECK_MASK & RV_CHECK_RANGE) */

        switch (eParam)
        {
            case cmChannelParamDataTypeNodeId:
            {
                /* The dataType node id can be set only in idle state. */
                if (channel->eState == ChannelStateIdle)
                {
                    if (RV_PVT_NODEID_IS_VALID(channel->dataTypeID) &&
                        (channel->reverseDataTypeID != channel->dataTypeID))
                    {
                        pvtDelete(app->hVal, channel->dataTypeID);
                    }
                    channel->dataTypeID = pvtAddRoot(app->hVal, app->h245DataType, 0, NULL);
                    pvtAddTree(app->hVal, channel->dataTypeID, app->hVal, (RvPvtNodeId)value);
                }
                break;
            }
            case cmChannelParamReverseDataTypeNodeId:
            {
                /* The reverse dataType node id can be set only in idle state. */
                if (channel->eState == ChannelStateIdle)
                {
                    if (RV_PVT_NODEID_IS_VALID(channel->reverseDataTypeID) &&
                        (channel->reverseDataTypeID != channel->dataTypeID))
                    {
                        pvtDelete(app->hVal, channel->reverseDataTypeID);
                    }
                    channel->reverseDataTypeID = pvtAddRoot(app->hVal, app->h245DataType, 0, NULL);
                    pvtAddTree(app->hVal, channel->reverseDataTypeID, app->hVal, (RvPvtNodeId)value);
                }
                break;
            }
            case cmChannelParamCapDataType:
            {

                /* These are not parameters that can be set! */
                status = RV_ERROR_BADPARAM;
                break;
            }
            case cmChannelParamRejectReason:
            {
                /* These are not parameters that can be set! */
                status = RV_ERROR_BADPARAM;
                break;
            }
            case cmChannelParamCapabilityTableEntryNumber:
            {
                channel->capTableEntryNum = (RvUint16)value;
                break;
            }
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
            case cmChannelParamH223AlLevel:
            {
                channel->data.h223Params.adaptationLayerType = (cmH223ALLevel)value;
                break;
            }
            case cmChannelParamH223Segmentable:
            {
                channel->data.h223Params.isSegmentable = (RvBool)value;
                break;
            }
            case cmChannelParamH223Al1Framed:
            {
                channel->data.h223Params.adaptationLayerData.adaptationLayer1Params.bIsFramed = (RvBool)value;
                break;
            }
            case cmChannelParamH223Al2SequenceNumber:
            {
                channel->data.h223Params.adaptationLayerData.adaptationLayer2Params.sequenceNumbering = (RvBool)value;
                break;
            }
            case cmChannelParamH223Al3ControlFieldSize:
            {
                channel->data.h223Params.adaptationLayerData.adaptationLayer3Params.controlFieldSize = (cmH223ControlFieldSize)value;
                channel->data.h223Params.adaptationLayerData.adaptationLayer3Params.reverseControlFieldSize = (cmH223ControlFieldSize)value;
                break;
            }
            case cmChannelParamH223Al3SendBufferSizeForRetransmissions:
            {
                channel->data.h223Params.adaptationLayerData.adaptationLayer3Params.sendBufferSize = (RvUint32)value;
                channel->data.h223Params.adaptationLayerData.adaptationLayer3Params.reverseSendBufferSize = (RvUint32)value;
                break;
            }
            case cmChannelParamH223AudioType:
            {
                channel->data.h223Params.eAudioType = (cmChannelAudioType)value;
                break;
            }
            case cmChannelParamH223BitRate:
            {
                channel->data.h223Params.bitRate = (RvUint32)value;
                break;
            }
            case cmChannelParamH223IgnoreCorruptedAlSdu:
            {
                channel->data.h223Params.bIgnoreCorruptedAlSdu = (RvBool)value;
                break;
            }
            case cmChannelParamH223ReverseNullDataType:
            {
                channel->data.h223Params.bIsDuplexReverseNullData = (RvBool)value;
                break;
            }
#if (RV_H223_USE_MUX_LEVEL_3 == RV_YES)
            case cmChannelParamH223AlxMParameters:
            {
                channel->data.h223Params.adaptationLayerData.adaptationLayerxMParams = *(cmH223ALxMConfig *)strValue;
                break;
            }
#endif
            case cmChannelParamH223IsMPC:
            {
                 /* This is not a parameter that can be set! */
                status = RV_ERROR_BADPARAM;
                break;
            }


#endif /* (RV_H245_SUPPORT_H223_PARAMS == 1) */

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
            case cmChannelParamH225LocalRtpAddress:
            {
                status = cmChannelSetRtpTransportAddress(hsChan, (cmTransportAddress*)strValue);
                break;
            }
            case cmChannelParamH225LocalRtcpAddress:
            {
                status = cmChannelSetRtcpTransportAddress(hsChan, (cmTransportAddress*)strValue);
                break;
            }
            case cmChannelParamH225RemoteRtpAddress:
            case cmChannelParamH225RemoteRtcpAddress:
            {

                /* These are not parameters that can be set! */
                status = RV_ERROR_BADPARAM;
                break;
            }
            case cmChannelParamH225DynamicRTPPayloadType:
            {
                status = cmChannelSetDynamicRTPPayloadType(hsChan, (RvInt8)value);
                break;
            }
            case cmChannelParamH225ForceSecondary:
            {
                channel->data.h225Params.bForceSecondary = (RvBool)value;
                break;
            }
#endif /* (RV_H245_SUPPORT_H225_PARAMS == 1) */
            default:
                status = RV_ERROR_UNKNOWN;
                break;
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSetParam=%d", status));
    return status;
}


/******************************************************************************
 * cmChannelGetParam
 * ----------------------------------------------------------------------------
 * General: Gets a specific channel parameter for the application.
 *          See also cmChannelParam, cmChannelSetParam.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsChan      - The channel handle whose parameter has been requested.
 * Output: eParam      - The parameter requested. The list of parameters are
 *                       defined in cmCallParam enumeration.
 *         value       - If the parameter value is a simple integer,
 *                       this is the pointer to the parameter value.
 *         strValue    - If the parameter value is a string, strValue
 *                       represents the parameter itself.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmChannelGetParam(
         IN      HCHAN               hsChan,
         IN      cmChannelParam      eParam,
         OUT     RvInt32             *value,
         OUT     RvUint8             *strValue)
{
    HH245       hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Object  *app;
    H245Channel *channel=(H245Channel*)hsChan;
    RvStatus    status = RV_OK;

    RV_UNUSED_ARG(strValue);

    app = (H245Object*)hApp;
#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if ((hApp == NULL) || (hsChan == NULL) || (value == NULL))
        return RV_ERROR_NULLPTR;
#endif /* (RV_CHECK_MASK & RV_CHECK_NULL) */

    H245APIEnter((app->pAppObject, "cmChannelGetParam(hsChan=%p,eParam=%d)", hsChan, eParam));

    if (emaLock((EMAElement)hsChan))
    {
#if (RV_CHECK_MASK & RV_CHECK_RANGE)
        /* Make sure we're handling the right type of channel with this parameter */
        switch (eParam)
        {
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
            case cmChannelParamH223AlLevel:
            case cmChannelParamH223Segmentable:
            case cmChannelParamH223Al1Framed:
            case cmChannelParamH223Al2SequenceNumber:
            case cmChannelParamH223Al3ControlFieldSize:
            case cmChannelParamH223Al3SendBufferSizeForRetransmissions:
            case cmChannelParamH223AudioType:
            case cmChannelParamH223BitRate:
            case cmChannelParamH223IgnoreCorruptedAlSdu:
            case cmChannelParamH223ReverseNullDataType:
#if (RV_H223_USE_MUX_LEVEL_3 == RV_YES)
            case cmChannelParamH223AlxMParameters:
#endif
            case cmChannelParamH223IsMPC:
            {
                if (channel->eType != RvH245ChannelParamsH223)
                    status = RV_ERROR_BADPARAM;
                break;
            }
#endif /* (RV_H245_SUPPORT_H223_PARAMS == 1) */

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
            case cmChannelParamH225LocalRtpAddress:
            case cmChannelParamH225LocalRtcpAddress:
            case cmChannelParamH225RemoteRtpAddress:
            case cmChannelParamH225RemoteRtcpAddress:
            case cmChannelParamH225DynamicRTPPayloadType:
            case cmChannelParamH225ForceSecondary:
            {
                if (channel->eType != RvH245ChannelParamsH225)
                    status = RV_ERROR_BADPARAM;
                break;
            }
#endif /* (RV_H245_SUPPORT_H225_PARAMS == 1) */
            default:
                break;
        }
#endif /* (RV_CHECK_MASK & RV_CHECK_RANGE) */

        switch (eParam)
        {
            case cmChannelParamDataTypeNodeId:
            {
                *value = (RvInt32)channel->dataTypeID;
                break;
            }
            case cmChannelParamReverseDataTypeNodeId:
            {
                *value = (RvInt32)channel->reverseDataTypeID;
                break;
            }
            case cmChannelParamCapDataType:
            {
                cmCapDataType capData = cmCapEmpty;

                /* Getting the capData from th dataType node id */
                if (channel->dataTypeID >= 0)
                {
                    confDataType  cnfData;
                    RvPvtNodeId   dataTypeId = channel->dataTypeID;

                    confGetDataTypeName(app->hVal, channel->dataTypeID, 0, NULL, &cnfData, NULL);
                    channelConvertConfData2CapData(app->hVal, cnfData, &dataTypeId, &capData);
                }
                *value = (RvInt32)capData;
                break;
            }
            case cmChannelParamRejectReason:
            {
                *value = (RvInt32)channel->eRejectReason;
                break;
            }
            case cmChannelParamCapabilityTableEntryNumber:
            {
                *value = (RvInt32)channel->capTableEntryNum;
                break;
            }
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
            case cmChannelParamH223AlLevel:
            {
                *value = (RvInt32)channel->data.h223Params.adaptationLayerType;
                break;
            }
            case cmChannelParamH223Segmentable:
            {
                *value = (RvInt32)channel->data.h223Params.isSegmentable;
                break;
            }
            case cmChannelParamH223Al1Framed:
            {
                *value = (RvInt32)channel->data.h223Params.adaptationLayerData.adaptationLayer1Params.bIsFramed;
                break;
            }
            case cmChannelParamH223Al2SequenceNumber:
            {
                *value = (RvInt32)channel->data.h223Params.adaptationLayerData.adaptationLayer2Params.sequenceNumbering;
                break;
            }
            case cmChannelParamH223Al3ControlFieldSize:
            {
                *value = (RvInt32)channel->data.h223Params.adaptationLayerData.adaptationLayer3Params.controlFieldSize;
                break;
            }
            case cmChannelParamH223Al3SendBufferSizeForRetransmissions:
            {
                *value = (RvInt32)channel->data.h223Params.adaptationLayerData.adaptationLayer3Params.sendBufferSize;
                break;
            }
            case cmChannelParamH223AudioType:
            {
                *value = (RvInt32)channel->data.h223Params.eAudioType;
                break;
            }
            case cmChannelParamH223BitRate:
            {
                *value = (RvInt32)channel->data.h223Params.bitRate;
                break;
            }
            case cmChannelParamH223IgnoreCorruptedAlSdu:
            {
                *value = (RvInt32)channel->data.h223Params.bIgnoreCorruptedAlSdu;
                break;
            }
            case cmChannelParamH223ReverseNullDataType:
            {
                *value = (RvInt32)channel->data.h223Params.bIsDuplexReverseNullData;
                break;
            }
#if (RV_H223_USE_MUX_LEVEL_3 == RV_YES)
            case cmChannelParamH223AlxMParameters:
            {
                // TODO: check sizes!
                *(cmH223ALxMConfig *)strValue = channel->data.h223Params.adaptationLayerData.adaptationLayerxMParams;
                break;
            }
#endif
#if (RV_3G324M_USE_MONA == RV_YES)
            case cmChannelParamH223IsMPC:
            {
                *value = (RvInt32)channel->data.h223Params.bIsMPCChannel;
                break;
            }
#endif /* USE_MONA */

#endif /* (RV_H245_SUPPORT_H223_PARAMS == 1) */

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
            case cmChannelParamH225LocalRtpAddress:
            {
                if (channel->data.h225Params.recvRtpAddressID < 0)
                {
                    status = RV_ERROR_UNKNOWN;
                    break;
                }
                /* Update the PVT of the RTP address in the channel element */
                status = cmVtToTA_H245(app->hVal, channel->data.h225Params.recvRtpAddressID, (cmTransportAddress*)strValue);
                *value = (RvInt32)sizeof(cmTransportAddress);
                break;
            }
            case cmChannelParamH225LocalRtcpAddress:
            {
                RvPvtNodeId rctpNode = RV_PVT_INVALID_NODEID;

                if ((channel->data.h225Params.pPartner == NULL) ||
                    (channel->data.h225Params.pPartner->data.h225Params.recvRtcpAddressID < 0))
                    rctpNode = channel->data.h225Params.recvRtcpAddressID;
                else
                    rctpNode = channel->data.h225Params.pPartner->data.h225Params.recvRtcpAddressID;

                if (RV_PVT_NODEID_IS_VALID(rctpNode))
                {
                    status = cmVtToTA_H245(app->hVal, rctpNode, (cmTransportAddress*)strValue);
                    *value = (RvInt32)sizeof(cmTransportAddress);
                }
                else
                    status = RV_ERROR_UNKNOWN;

                break;
            }
            case cmChannelParamH225RemoteRtpAddress:
            {
                if (channel->data.h225Params.sendRtpAddressID < 0)
                {
                    status = RV_ERROR_UNKNOWN;
                    break;
                }
                /* Update the PVT of the RTP address in the channel element */
                status = cmVtToTA_H245(app->hVal, channel->data.h225Params.sendRtpAddressID, (cmTransportAddress*)strValue);
                *value = (RvInt32)sizeof(cmTransportAddress);
                break;
            }
            case cmChannelParamH225RemoteRtcpAddress:
            {
                RvPvtNodeId rctpNode = RV_PVT_INVALID_NODEID;

                if ((channel->data.h225Params.pPartner == NULL) ||
                    (channel->data.h225Params.pPartner->data.h225Params.sendRtcpAddressID < 0))
                    rctpNode = channel->data.h225Params.sendRtcpAddressID;
                else
                    rctpNode = channel->data.h225Params.pPartner->data.h225Params.sendRtcpAddressID;

                if (RV_PVT_NODEID_IS_VALID(rctpNode))
                {
                    status = cmVtToTA_H245(app->hVal, rctpNode, (cmTransportAddress*)strValue);
                    *value = (RvInt32)sizeof(cmTransportAddress);
                }
                else
                    status = RV_ERROR_UNKNOWN;

                break;
            }
            case cmChannelParamH225ForceSecondary:
            {
                *value = (RvInt32)channel->data.h225Params.bForceSecondary;
                break;
            }
            case cmChannelParamH225DynamicRTPPayloadType:
            {
                *value = (RvInt32)channel->data.h225Params.dynamicPayloadNumber;
                break;
            }

#endif /* (RV_H245_SUPPORT_H225_PARAMS == 1) */
            default:
                status = RV_ERROR_UNKNOWN;
                break;

        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelGetParam=%d", status));
    return status;
}


/******************************************************************************
 * cmChannelGetHandle
 * ----------------------------------------------------------------------------
 * General: Returns the application handle for a channel from the channel handle.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsChan      - The stack handle for the channel.
 * Output: haChan      - The returned application handle of the channel.
 *****************************************************************************/
RVAPI
int RVCALLCONV cmChannelGetHandle(IN HCHAN hsChan, OUT HAPPCHAN* haChan)
{
    H245Object  *app=(H245Object*)emaGetUserData((EMAElement)hsChan);

    if (hsChan == NULL || app == NULL)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelGetHandle: hsChan=%p", hsChan));
    if (haChan != NULL)
        *haChan=(HAPPCHAN)emaGetApplicationHandle((EMAElement)hsChan);
    H245APIExit((app->pAppObject, "cmChannelGetHandle: 1 (haChan=%p)",*haChan));
    return RV_OK;
}


#if (RV_H245_SUPPORT_H225_PARAMS == 1)
/****************************************************************************************************************/
/* This function shall be called before a channel becomes idle, in order to enable subsequent channels to use   */
/* the default session IDs 1, 2 and 3.                                                                          */
/****************************************************************************************************************/
static void setFirstChannels(IN H245Channel* channel)
{
    H245Control * ctrl = (H245Control *)channel->hCtrl;

    if (ctrl != NULL)
    {
        if (ctrl->eParamsType == RvH245ChannelParamsH225)
        {
            if (channel->bOrigin || channel->bIsDuplex)
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
            if (!channel->bOrigin || channel->bIsDuplex)
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
}
#endif


/************************************************************************
 * closeChannels
 * purpose: Close any channels of a given control object.
 *          This function only notifies the application about closing the
 *          channels due to disconnection.
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
void closeChannels(IN HCONTROL ctrl)
{
    H245Channel* channel;
    void*        ptr=NULL;
    H245Object *app = NULL;
    HPVT hVal = NULL;
    RvPvtNodeId message = RV_PVT_INVALID_NODEID;
    RvPvtNodeId nodeId = RV_PVT_INVALID_NODEID;
    RvBool      isMPCChannel = RV_FALSE;
    H245Control *ctrlH245=(H245Control*)ctrl;

    /* Notify the application about every channel it has about its closing */
    channel = getNextChan(ctrl, &ptr);
#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)

#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */
    while (channel != NULL)
    {
        if(emaLock((EMAElement)channel))
        {
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
#if (RV_3G324M_USE_MONA == RV_YES)
            if (channel->eType == RvH245ChannelParamsH223)
            {
                isMPCChannel = channel->data.h223Params.bIsMPCChannel;
                /* We're closing all the channels only when we're dropping the call. Since
                   we might get some more data on this channel, we had better mark it so
                   the information won't be passed along, and we'll be able to really
                   delete this channel later on. */
           /*     emaMark((EMAElement)channel); */
            }
#endif /* USE_MONA */
#endif
            if (isMPCChannel == RV_FALSE && channel->eState != ChannelStateReleased)
            {
                /* Avoid calling these callbacks if we're already in released state for the channel.
                   Otherwise, we might get an endless loop */
#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
				if (ctrlH245->eExtendedFastConnectState != cmExtendedFastConnectApproved)
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == RV_YES) */
                if ((channel->bOrigin) && (((H245Control*)ctrl)->eState != ctrlNotInitialized) &&
                    (channel->eState != ChannelStateAwaitingRelease))
                {
                    /* We send outgoing CLC messages before we close this call, just for backward compatibility... */
                    if (app == NULL)
                    {
                        app = (H245Object*)emaGetUserData((EMAElement)channel);
                        hVal = app->hVal;

                        /* prepare a CLC message */
                        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
                        nodeId=pvtAddBranch2(hVal,message, __h245(request),__h245(closeLogicalChannel));
                        pvtAddBranch2(hVal,nodeId,__h245(reason),__h245(unknown));
                        pvtAddBranch2(hVal,nodeId,__h245(source),__h245(user));
                    }

                    pvtAdd(hVal,nodeId,__h245(forwardLogicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
                    /* we're the origin of the channel, send CLC */
                    sendMessageH245Chan(channel->hCtrl, (HCHAN)channel, message, RV_FALSE, NULL);
                }
                /* now raise the events */
                channel->eState = ChannelStateReleased;
                notifyChannelState(channel,cmChannelStateDisconnected,cmChannelStateModeDisconnectedLocal);
                notifyChannelState(channel,cmChannelStateIdle, cmChannelStateModeOff);
            }
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
             /*if a MPC channel - its not a real channel - just deallocate it*/
            if (isMPCChannel == RV_TRUE)
            {
                /*remove the H223 part of the MPC channel*/
                channelFreeMemory((HCHAN)channel);

                if (channel->pNext)
                    channel->pNext->pPrev = channel->pPrev;
                if (channel->pPrev)
                    channel->pPrev->pNext = channel->pNext;
                else
                    cmH245SetChannelList(ctrl, (HCHAN)channel->pNext);

                emaDelete((EMAElement)channel);
            }
#endif /*RV_H245_SUPPORT_H223_PARAMS*/
            emaUnlock((EMAElement)channel);
        }
        channel = getNextChan(ctrl, &ptr);
    }
    pvtDelete(hVal,message);
}


/******************************************************************************
 * incomingChannelMessage
 * ----------------------------------------------------------------------------
 * General: Make sure cmEvChannelRecvMessage() is invoked in necessary on
 *          incoming channel related messages.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  app      - H.245 instance used
 *         channel  - Channel this message belongs to
 *         message  - Message received
 * Output: None.
 *****************************************************************************/
void incomingChannelMessage(
    IN H245Object*      app,
    IN H245Channel*     channel,
    IN int              message)
{
    int nesting;

    if (app->cmMyChannelEvent.cmEvChannelRecvMessage != NULL)
    {
        H245CBEnter((app->pAppObject, "cmEvChannelRecvMessage(haChan=%p,hsChan=%p,msg=%d)",
            (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, message));
        nesting = emaPrepareForCallback((EMAElement)channel);
        app->cmMyChannelEvent.cmEvChannelRecvMessage(
            (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel),
            (HCHAN)channel,
            message);
        emaReturnFromCallback((EMAElement)channel, nesting);
        H245CBExit((app->pAppObject, "cmEvChannelRecvMessage"));
    }
}


/******************************************************************************
 * outgoingChannelMessage
 * ----------------------------------------------------------------------------
 * General: Make sure cmEvChannelSendMessage() is invoked in necessary on
 *          outgoing channel related messages.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  app      - H.245 instance used.
 *         channel  - Channel this message belongs to.
 *         message  - Message to send.
 * Output: None.
 *****************************************************************************/
void outgoingChannelMessage(
    IN H245Object*      app,
    IN H245Channel*     channel,
    IN int              message)
{
    int nesting;

    if (app->cmMyChannelEvent.cmEvChannelSendMessage != NULL)
    {
        H245CBEnter((app->pAppObject, "cmEvChannelSendMessage(haChan=%p,hsChan=%p,msg=%d)",
            (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, message));
        nesting = emaPrepareForCallback((EMAElement)channel);
        app->cmMyChannelEvent.cmEvChannelSendMessage(
            (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel),
            (HCHAN)channel,
            message);
        emaReturnFromCallback((EMAElement)channel, nesting);
        H245CBExit((app->pAppObject, "cmEvChannelSendMessage"));
    }
}


#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
/************************************************************************
 * chanGetMibParam
 * purpose: Get channel related MIB parameters
 * input  : hsChan      - H245Channel to check
 *          type        - Parameter type to get
 * output : valueSize   - Value, if numeric
 *                        String's length if string value type
 *          value       - String value if applicable
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int chanGetMibParam(
    IN  HCHAN                   hsChan,
    IN  mibControlParamTypeT    type,
    OUT int*                    valueSize,
    OUT RvUint8*                value)
{
    H245Channel* channel = (H245Channel *)hsChan;

    RV_UNUSED_ARG(value);

    switch (type)
    {
        case enumh245LogChannelsChannelState:
            switch (channel->eState)
            {
                case ChannelStateReleased:
                case ChannelStateIdle:                  return RV_ERROR_UNKNOWN;
                case ChannelStateAwaitingEstablishment: *valueSize = (int)lcseAwaitingEstablishment; break;
                case ChannelStateFaststart:             *valueSize = (int)lcseEstablished;           break;
                case ChannelStateFsAwaitingEstablish:   *valueSize = (int)lcseAwaitingEstablishment; break;
                case ChannelStateEstablished:           *valueSize = (int)lcseEstablished;           break;
                case ChannelStateAwaitingRelease:       *valueSize = (int)lcseAwaitingRelease;       break;
                case ChannelStateAwaitingConfirmation:  *valueSize = (int)lcseAwaitingConfirmation;  break;
                default:                    return RV_ERROR_UNKNOWN;
            }
            return 0;

        case enumh245LogChannelsMediaTableType:
        {
            HPVT            hVal;
            int             nodeId, index;
            RvPstFieldId    fieldId;

            hVal = ((H245Object*)emaGetUserData((EMAElement)channel))->hVal;
            nodeId = pvtChild(hVal, channel->dataTypeID);
            if (pvtGet(hVal, nodeId, &fieldId, NULL, NULL, NULL) < 0)
                return RV_ERROR_UNKNOWN;

            switch (fieldId)
            {
                case __h245(nonStandard):
                    *valueSize = 1;
                    return 0;
                case __h245(videoData):     *valueSize = 1; break;
                case __h245(audioData):     *valueSize = 6; break;
                case __h245(data):          *valueSize = 26; break;
                default:
                    return RV_ERROR_UNKNOWN;
            }

            index = pvtGetSyntaxIndex(hVal, pvtChild(hVal, nodeId));
            (*valueSize) += index;
            return 0;
        }

        case enumh245LogChannelsSessionId:
            *valueSize = channel->data.h225Params.sessionId;
            return 0;

        case enumh245LogChannelsAssociateSessionId:
            if (channel->data.h225Params.pAssociated != NULL)
            {
                *valueSize = channel->data.h225Params.pAssociated->data.h225Params.sessionId;
                return 0;
            }
            break;

        case enumh245LogChannelsMediaChannel:
            if (channel->data.h225Params.recvRtpAddressID >= 0)
            {
                if (cmVtToTA_H245(((H245Object*)emaGetUserData((EMAElement)channel))->hVal, channel->data.h225Params.recvRtpAddressID, (cmTransportAddress *)value) >= 0)
                    return 0;
            }
            break;

        case enumh245LogChannelsMediaControlChannel:
        {
            if (channel->data.h225Params.recvRtcpAddressID >= 0)
            {
                if (cmVtToTA_H245(((H245Object*)emaGetUserData((EMAElement)channel))->hVal, channel->data.h225Params.recvRtcpAddressID, (cmTransportAddress *)value) >= 0)
                    return 0;
            }
            break;
        }

        case enumh245LogChannelsDestination:
            value[0] = (RvUint8)(channel->data.h225Params.destination.mcuNumber & 0xff);
            value[1] = (RvUint8)(channel->data.h225Params.destination.terminalNumber & 0xff);
            *valueSize = 2;
            return 0;

        case enumh245LogChannelsSrcTerminalLabel:
            value[0] = (RvUint8)(channel->data.h225Params.source.mcuNumber & 0xff);
            value[1] = (RvUint8)(channel->data.h225Params.source.terminalNumber & 0xff);
            *valueSize = 2;
            return 0;

        case enumh245LogChannelsDynamicRTPPayloadType:
            if (channel->data.h225Params.dynamicPayloadNumber > 0)
            {
                *valueSize = channel->data.h225Params.dynamicPayloadNumber;
                return 0;
            }
            break;

        case enumh245LogChannelsSilenceSuppression:
        case enumh245LogChannelsMediaGuaranteedDelivery:
        case enumh245LogChannelsMediaControlGuaranteedDelivery:
        case enumh245LogChannelsH261aVideoPacketization:
        case enumh245LogChannelsRTPPayloadDescriptor:
        case enumh245LogChannelsRTPPayloadType:
        case enumh245LogChannelsTransportCapability:
        case enumh245LogChannelsRedundancyEncoding:
            /* parameters not supported */
            break;

        default:
            break;
    }

    return RV_ERROR_UNKNOWN;
}
#endif /* (RV_H245_SUPPORT_H225_PARAMS == RV_YES) */


/* functions for the channels EMA */
HEMA H245InitChannels(
    IN HH245        hH245,
    IN HAPP         hApp,
    IN RvWatchdog   *watchdog,
    IN int          maxCalls,
    IN int          maxChannels)
{
    H245Object* app=(H245Object*)hH245;
    HEMA hChannels;
    RvStatus status;

    hChannels = emaConstruct(sizeof(H245Channel), maxCalls * maxChannels,
        emaLinkedLocks, "H245 Channels", CHAN, (void*)hH245, (void*)hApp, app->logMgr);

    if (hChannels != NULL)
    {
        status = emaAddWatchdogResource(hChannels, watchdog,
            "H245CHANNEL", "H245", "H.245 Channel elements");

        if (status != RV_OK)
        {
            emaDestruct(hChannels);
            return NULL;
        }
    }

    return hChannels;
}

void H245EndChannels(HEMA channels)
{
    emaDestruct(channels);
}


#ifdef __cplusplus
}
#endif
