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

#include "cmictrl.h"
#include "cmConf.h"
#include "caputils.h"
#include "cmchan.h"
#include "h245.h"
#include "cmChanGet.h"
#include "transpcap.h"
#include "cmH245Object.h"


#ifdef __cplusplus
extern "C" {
#endif


#define ctrlGetH245Object(_ctrl) \
    (H245Object*)cmiGetH245Handle(emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)(_ctrl))))




RvBool channelML_TimeoutEventsHandler(IN void* hsChan)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int message;
    H245Object*app=(H245Object*)hApp;
    int nesting;

    if (!hApp)
        return RV_FALSE;

    if (emaLock((EMAElement)hsChan))
    {
        HPVT hVal = ((H245Object *)hApp)->hVal;

        app->evHandlers.pfnTimerClearEv(channel->hCtrl, &channel->pMediaLoopTimer);

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        pvtAddBranch2(hVal,message, __h245(command),__h245(maintenanceLoopOffCommand));

        sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);

        if (app->cmMyChannelEvent.cmEvChannelMediaLoopStatus != NULL)
        {
            HAPPCHAN haChan = (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel);
            H245CBEnter((app->pAppObject, "cmEvChannelMediaLoopStatus(haChan=%p,hsChan=%p,status=reject)", haChan, channel));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelMediaLoopStatus(haChan, (HCHAN)channel, cmMediaLoopOff);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMediaLoopStatus"));
        }

        emaUnlock((EMAElement)hsChan);
    }
    return RV_FALSE;
}

int flowControlCommand(H245Control*ctrl, int base, int message)
{
    RvInt32 lcn;
    HPVT hVal;
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    RvInt32 rate;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelNewRate != NULL)
    {
        hVal=app->hVal;

        pvtGetChildValue2(hVal,message,__h245(scope),__h245(logicalChannelNumber),&lcn,NULL);
        channel=getOutSubChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            incomingChannelMessage(app, channel, base);

            pvtGetChildValue2(hVal,message,__h245(restriction),__h245(maximumBitRate),&rate,NULL);
            H245CBEnter((app->pAppObject, "cmEvChannelNewRate: haChan=%p, hsChan=%p, rate=%d.",
                emaGetApplicationHandle((EMAElement)channel), channel, rate));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelNewRate(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, (RvUint32)rate);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelNewRate."));

            emaUnlock((EMAElement)channel);
        }
    }

    return RV_TRUE;
}


int maintenanceLoopRequest(H245Control *ctrl, int base, int message)
{
    RvInt32 lcn;
    HPVT hVal;
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelMediaLoopStatus != NULL)
    {
        hVal=app->hVal;

        pvtGetChildValue2(hVal,message,__h245(type),__h245(mediaLoop),&lcn,NULL);
        channel=getInChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            incomingChannelMessage(app, channel, base);

            H245CBEnter((app->pAppObject, "cmEvChannelMediaLoopStatus: haChan=%p, hsChan=%p, status=request.",(HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), channel));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelMediaLoopStatus(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmMediaLoopRequest);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMediaLoopStatus."));
            emaUnlock((EMAElement)channel);
        }
    }

    return RV_TRUE;
}

int maintenanceLoopAck(H245Control *ctrl, int base, int message)
{
    RvInt32 lcn;
    HPVT hVal;
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    hVal=app->hVal;

    pvtGetChildValue2(hVal,message,__h245(type),__h245(mediaLoop),&lcn,NULL);
    channel=getOutChanByLCN((HCONTROL)ctrl,lcn);

    if (emaLock((EMAElement)channel))
    {
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &channel->pMediaLoopTimer);

        incomingChannelMessage(app, channel, base);

        if (app->cmMyChannelEvent.cmEvChannelMediaLoopStatus != NULL)
        {
            H245CBEnter((app->pAppObject, "cmEvChannelMediaLoopStatus: haChan=%p, hsChan=%p, status=confirm.",(HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), channel));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelMediaLoopStatus(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmMediaLoopConfirm);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMediaLoopStatus."));
        }

        emaUnlock((EMAElement)channel);
    }

    return RV_TRUE;
}

int maintenanceLoopReject(H245Control*ctrl, int base, int message)
{
    RvInt32 lcn;
    HPVT hVal;
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    hVal=app->hVal;

    pvtGetChildValue2(hVal,message,__h245(type),__h245(mediaLoop),&lcn,NULL);
    channel=getOutChanByLCN((HCONTROL)ctrl,lcn);

    if (emaLock((EMAElement)channel))
    {
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &channel->pMediaLoopTimer);

        incomingChannelMessage(app, channel, base);

        if (app->cmMyChannelEvent.cmEvChannelMediaLoopStatus != NULL)
        {
            H245CBEnter((app->pAppObject, "cmEvChannelMediaLoopStatus: haChan=%p, hsChan=%p, status=off.",(HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), channel));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelMediaLoopStatus(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmMediaLoopOff);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMediaLoopStatus."));
        }

        emaUnlock((EMAElement)channel);
    }

    return RV_TRUE;
}

int maintenanceLoopOffCommand(H245Control*ctrl, int message)
{
    H245Object* app=ctrlGetH245Object(ctrl);
    void* ptr=NULL;
    H245Channel* channel;
    int nesting;

    RV_UNUSED_ARG(message);

    if (app->cmMyChannelEvent.cmEvChannelMediaLoopStatus != NULL)
    {
        channel=getNextInChan((HCONTROL)ctrl,&ptr);
        while (channel != NULL)
        {
            if (emaLock((EMAElement)channel))
            {
                H245CBEnter((app->pAppObject, "cmEvChannelMediaLoopStatus: haChan=%p, hsChan=%p, status=confirm.",(HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), channel));
                nesting = emaPrepareForCallback((EMAElement)channel);
                app->cmMyChannelEvent.cmEvChannelMediaLoopStatus(
                    (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmMediaLoopOff);
                emaReturnFromCallback((EMAElement)channel, nesting);
                H245CBExit((app->pAppObject, "cmEvChannelMediaLoopStatus."));

                emaUnlock((EMAElement)channel);
            }

            channel=getNextInChan((HCONTROL)ctrl,&ptr);
        }
    }
    return RV_TRUE;
}


int videoFreezePicture(H245Control* ctrl, int base, int lcn)
{
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelMiscCommand != NULL)
    {
        channel=getInChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            incomingChannelMessage(app, channel, base);

            H245CBEnter((app->pAppObject, "cmEvChannelMiscCommand: haChan=%p, hsChan=%p,  type=%d",   emaGetApplicationHandle((EMAElement)channel), channel, cmVideoFreezePicture));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelMiscCommand(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmVideoFreezePicture);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMiscCommand."));

            emaUnlock((EMAElement)channel);
        }
    }

    return RV_TRUE;
}

int videoSendSyncEveryGOB(H245Control* ctrl, int base, int lcn)
{
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelMiscCommand != NULL)
    {
        channel=getOutChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            incomingChannelMessage(app, channel, base);

            H245CBEnter((app->pAppObject, "cmEvChannelMiscCommand: haChan=%p, hsChan=%p,  type=%d",   emaGetApplicationHandle((EMAElement)channel), channel, cmVideoSendSyncEveryGOB));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelMiscCommand(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmVideoSendSyncEveryGOB);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMiscCommand."));

            emaUnlock((EMAElement)channel);
        }
    }

    return RV_TRUE;
}

int videoSendSyncEveryGOBCancel(H245Control* ctrl, int base, int lcn)
{
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelMiscCommand != NULL)
    {
        channel=getOutChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            incomingChannelMessage(app, channel, base);

            H245CBEnter((app->pAppObject, "cmEvChannelMiscCommand: haChan=%p, hsChan=%p,  type=%d",   emaGetApplicationHandle((EMAElement)channel), channel, cmVideoSendSyncEveryGOBCancel));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelMiscCommand(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmVideoSendSyncEveryGOBCancel);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMiscCommand."));

            emaUnlock((EMAElement)channel);
        }
    }

    return RV_TRUE;
}

int switchReceiveMediaOff(H245Control* ctrl, int base, int lcn)
{
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelMiscCommand != NULL)
    {
        channel=getOutChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            incomingChannelMessage(app, channel, base);

            H245CBEnter((app->pAppObject, "cmEvChannelMiscCommand: haChan=%p, hsChan=%p,  type=%d",   emaGetApplicationHandle((EMAElement)channel), channel, cmSwitchReceiveMediaOff));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelMiscCommand((HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmSwitchReceiveMediaOff);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMiscCommand."));

            emaUnlock((EMAElement)channel);
        }
    }

    return RV_TRUE;
}

int switchReceiveMediaOn(H245Control* ctrl, int base, int lcn)
{
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelMiscCommand != NULL)
    {
        channel=getOutChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            incomingChannelMessage(app, channel, base);

            H245CBEnter((app->pAppObject, "cmEvChannelMiscCommand: haChan=%p, hsChan=%p,  type=%d",   emaGetApplicationHandle((EMAElement)channel), channel, cmSwitchReceiveMediaOn));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelMiscCommand((HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmSwitchReceiveMediaOn);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMiscCommand."));

            emaUnlock((EMAElement)channel);
        }
    }

    return RV_TRUE;
}


int videoTemporalSpatialTradeOff(H245Control* ctrl, int base, int message, int lcn, RvInt8 isCommand)
{
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    int value;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelTSTO != NULL)
    {
        channel=getOutChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            incomingChannelMessage(app, channel, base);

            pvtGet(app->hVal, message, NULL, NULL, &value, NULL);

            H245CBEnter((app->pAppObject, "cmEvChannelTSTO: haChan=%p, hsChan=%p, isCommand=%d (command), value=%d",
                emaGetApplicationHandle((EMAElement)channel), channel, isCommand, value));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelTSTO(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, isCommand, (RvInt8)value);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelTSTO."));

            emaUnlock((EMAElement)channel);
        }
    }

    return RV_TRUE;
}


int videoFastUpdatePicture(H245Control* ctrl, int base, int lcn)
{
    H245Object* app=NULL;
    H245Channel* channel=NULL;
    int nesting;

    if (emaLock((EMAElement)cmiGetByControl((HCONTROL)ctrl)))
    {
        app = ctrlGetH245Object(ctrl);

        if (app == NULL)
        {
            emaUnlock((EMAElement)cmiGetByControl((HCONTROL)ctrl));
            return RV_ERROR_UNKNOWN;
        }

        channel = getOutChanByLCN((HCONTROL)ctrl, lcn);
        emaUnlock((EMAElement)cmiGetByControl((HCONTROL)ctrl));
    }
    else
        return RV_ERROR_UNKNOWN;

    if (emaLock((EMAElement)channel))
    {
        incomingChannelMessage(app, channel, base);

        if (app->cmMyChannelEvent.cmEvChannelVideoFastUpdatePicture != NULL)
        {
            HAPPCHAN haChan = (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel);
            H245CBEnter((app->pAppObject, "cmEvChannelVideoFastUpdatePicture(haChan=%p,hsChan=%p)", haChan, channel));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelVideoFastUpdatePicture(haChan, (HCHAN)channel);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelVideoFastUpdatePicture"));
        }

        if (!emaWasDeleted((EMAElement)channel) && (app->cmMyChannelEvent.cmEvChannelMiscCommand != NULL))
        {
            HAPPCHAN haChan = (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel);

            H245CBEnter((app->pAppObject, "cmEvChannelMiscCommand(haChan=%p,hsChan=%p,type=%d)", haChan, channel, cmVideoFastUpdatePicture));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelMiscCommand(haChan, (HCHAN)channel, cmVideoFastUpdatePicture);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMiscCommand"));
        }

        emaUnlock((EMAElement)channel);
    }

    return RV_TRUE;
}

int videoFastUpdateGOB(H245Control* ctrl, int base, int message, int lcn)
{
    HPVT hVal;
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    RvInt32 firstGOB, numOfGOBS;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelVideoFastUpdateGOB != NULL)
    {
        hVal=app->hVal;
        channel=getOutChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            incomingChannelMessage(app, channel, base);

            pvtGetChildByFieldId(hVal, message, __h245(firstGOB), &firstGOB, NULL);
            pvtGetChildByFieldId(hVal,  message, __h245(numberOfGOBs), &numOfGOBS, NULL);

            H245CBEnter((app->pAppObject, "cmEvChannelVideoFastUpdateGOB: haChan=%p, hsChan=%p, firstGOB=%d, #GOBS=%d.",
                emaGetApplicationHandle((EMAElement)channel), channel, firstGOB, numOfGOBS));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelVideoFastUpdateGOB(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, (int)firstGOB, (int)numOfGOBS);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelVideoFastUpdateGOB."));

            emaUnlock((EMAElement)channel);
        }
    }

    return RV_TRUE;
}

int videoFastUpdateMB(H245Control* ctrl, int base, int message, int lcn)
{
    HPVT hVal;
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    RvInt32 firstGOB, firstMB, numOfMBS;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelVideoFastUpdateMB != NULL)
    {
        hVal=app->hVal;
        channel=getOutChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            incomingChannelMessage(app, channel, base);

            pvtGetChildByFieldId(hVal, message, __h245(firstGOB), &firstGOB, NULL);
            pvtGetChildByFieldId(hVal,  message, __h245(firstMB), &firstMB, NULL);
            pvtGetChildByFieldId(hVal,  message, __h245(numberOfMBs), &numOfMBS, NULL);
            H245CBEnter((app->pAppObject, "cmEvChannelVideoFastUpdateMB: haChan=%p, hsChan=%p, firstGOB=%d, firstMB=%d, #MS=%d.",
                emaGetApplicationHandle((EMAElement)channel), channel, firstGOB, firstMB, numOfMBS));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelVideoFastUpdateMB(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, (int)firstGOB, (int)firstMB, (int)numOfMBS);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelVideoFastUpdateMB."));

            emaUnlock((EMAElement)channel);
        }
    }

    return RV_TRUE;
}

/* h2250MaximumSkewIndication */

int logicalChannelActive(H245Control* ctrl, int base, int lcn)
{
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelStateChanged != NULL)
    {
        channel = getInChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            if (channel->eState == ChannelStateEstablished)
            {
                incomingChannelMessage(app, channel, base);
                notifyChannelState(channel, cmChannelStateConnected, cmChannelStateModeOn);
            }
            emaUnlock((EMAElement)channel);
        }
    }

    if (ctrl->eState == ctrlMesFinished)
    {
        cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
    }

    return RV_TRUE;
}

int logicalChannelInactive(H245Control* ctrl, int base, int lcn)
{
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelStateChanged)
    {
        channel = getInChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            if (channel->eState == ChannelStateEstablished)
            {
                incomingChannelMessage(app, channel, base);
                notifyChannelState(channel, cmChannelStateConnected, cmChannelStateModeOff);
            }
            emaUnlock((EMAElement)channel);
        }
    }

    if (ctrl->eState == ctrlMesFinished)
    {
        cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
    }

    return RV_TRUE;
}


#if (RV_H245_LEAN_H223 == RV_NO)
int transportCapability(H245Control* ctrl, int base, int message, int lcn)
{
    H245Object* app=ctrlGetH245Object(ctrl);
    H245Channel* channel=NULL;
    int nesting;

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMyChannelEvent.cmEvChannelTransportCapInd != NULL)
    {
        channel=getInChanByLCN((HCONTROL)ctrl,lcn);

        if (emaLock((EMAElement)channel))
        {
            incomingChannelMessage(app, channel, base);

            H245CBEnter((app->pAppObject, "cmEvChannelTransportCapInd: haChan=%p, hsChan=%p,  (indication), nodeId=%d",
                emaGetApplicationHandle((EMAElement)channel), channel, message));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelTransportCapInd(
                (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, message);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelTransportCapInd."));

            emaUnlock((EMAElement)channel);
        }
    }

    return RV_TRUE;
}
#endif


RVAPI int RVCALLCONV
cmChannelOn(
        IN  HCHAN       hsChan)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int message,nodeId;
    H245Object*app=(H245Object*)hApp;
    int ret = RV_ERROR_UNKNOWN;

    if (!hApp)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelOn: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        if (channel->bOrigin)
        {
            HPVT hVal = app->hVal;

            message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
            nodeId=pvtAddBranch2(hVal,message, __h245(indication),__h245(miscellaneousIndication));

            pvtAdd(hVal,nodeId,__h245(logicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
            pvtAddBranch2(hVal,nodeId,__h245(type),__h245(logicalChannelActive));

            ret = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);
        }
        else
            ret = RV_ERROR_UNKNOWN;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelOn: [%d].", ret));
    return ret;
}



RVAPI int RVCALLCONV
cmChannelOff(
         IN     HCHAN       hsChan)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int message,nodeId;
    H245Object*app=(H245Object*)hApp;
    int ret = RV_ERROR_UNKNOWN;

    if (!hApp)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelOff: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        if (channel->bOrigin)
        {
            HPVT hVal = app->hVal;

            message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
            nodeId=pvtAddBranch2(hVal,message, __h245(indication),__h245(miscellaneousIndication));

            pvtAdd(hVal,nodeId,__h245(logicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
            pvtAddBranch2(hVal,nodeId,__h245(type),__h245(logicalChannelInactive));

            ret = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);
        }
        else
            ret = RV_ERROR_UNKNOWN;
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelOff: [%d].", ret));
    return ret;
}

RVAPI int RVCALLCONV
cmChannelFlowControl(
             IN     HCHAN       hsChan,
             IN     RvUint32    rate)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int message, nodeId, res = RV_ERROR_UNKNOWN;
    H245Object*app=(H245Object*)hApp;

    if (!hApp)   return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelFlowControl: hsChan=%p, rate=%d.", hsChan, rate));

    if (emaLock((EMAElement)hsChan))
    {
        HPVT hVal = app->hVal;

        if (channel->bOrigin && !channel->bIsDuplex)
        {
            emaUnlock((EMAElement)hsChan);
            H245APIExit((app->pAppObject, "cmChannelFlowControl: [Invalid circumstances]."));
            return RV_ERROR_UNKNOWN;
        }

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(command),__h245(flowControlCommand));

        pvtAdd(hVal,pvtAddBranch(hVal,nodeId,__h245(restriction)),__h245(maximumBitRate),(RvInt32)rate,NULL,NULL);
        pvtAdd(hVal,pvtAddBranch(hVal,nodeId,__h245(scope)),__h245(logicalChannelNumber), (channel->bOrigin)?channel->reverseLogicalChannelNum:channel->myLogicalChannelNum,NULL,NULL);

        res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelFlowControl=%d", res));
    return res;
}

RVAPI int RVCALLCONV
cmChannelVideoFastUpdatePicture(
                IN  HCHAN           hsChan)
{
    return cmChannelSendMiscCommand(hsChan,cmVideoFastUpdatePicture);
}


RVAPI int RVCALLCONV
cmChannelVideoFastUpdateGOB(
                IN  HCHAN           hsChan,
                IN  int             firstGOB,
                IN  int             numberOfGOBs)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int message, nodeId, id, res = RV_ERROR_UNKNOWN;
    H245Object*app=(H245Object*)hApp;

    if (!hApp)   return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelVideoFastUpdateGOB: hsChan=%p, firstGOB=%d, numberOfGOBs=%d.",hsChan, firstGOB, numberOfGOBs));

    if (emaLock((EMAElement)hsChan))
    {
        HPVT hVal = app->hVal;

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(command),__h245(miscellaneousCommand));
        pvtAdd(hVal,nodeId,__h245(logicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
        id=pvtAddBranch2(hVal,nodeId,__h245(type),__h245(videoFastUpdateGOB));
        pvtAdd(app->hVal, id, __h245(firstGOB), firstGOB, NULL, NULL);
        pvtAdd(app->hVal, id, __h245(numberOfGOBs), numberOfGOBs, NULL, NULL);

        res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelVideoFastUpdateGOB=%d", res));
    return res;
}


RVAPI int RVCALLCONV
cmChannelVideoFastUpdateMB(
               IN   HCHAN           hsChan,
               IN   int             firstGOB,
               IN   int             firstMB,
               IN   int             numberOfMBs)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int message, nodeId, id, res = RV_ERROR_UNKNOWN;
    H245Object*app=(H245Object*)hApp;

    if (!hApp)   return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelVideoFastUpdateMB: hsChan=%p, firstGOB=%d, firstMB=%d, numberOfMBs=%d.",
        hsChan, firstGOB, firstMB, numberOfMBs));

    if (emaLock((EMAElement)hsChan))
    {
        HPVT hVal = app->hVal;

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(command),__h245(miscellaneousCommand));
        pvtAdd(hVal,nodeId,__h245(logicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
        id=pvtAddBranch2(hVal,nodeId,__h245(type),__h245(videoFastUpdateMB));
        pvtAdd(app->hVal, id, __h245(firstGOB), firstGOB, NULL, NULL);
        pvtAdd(app->hVal, id, __h245(firstMB), firstMB, NULL, NULL);
        pvtAdd(app->hVal, id, __h245(numberOfMBs), numberOfMBs, NULL, NULL);

        res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelVideoFastUpdateMB=%d", res));
    return res;
}


/*   temporal spatial trade off__________________________________________________________*/



RVAPI int RVCALLCONV
cmChannelTSTOCommand(
             /* Send temporal spatial trade off command. Request the remote terminal to change
              the tradeoff. */
             IN HCHAN hsChan, /* incoming channel */
             IN int tradeoffValue /* 0-31 */
             )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int message, nodeId, res = RV_ERROR_UNKNOWN;
    H245Object*app=(H245Object*)hApp;

    if (!hsChan || tradeoffValue<0 || tradeoffValue>31) return RV_ERROR_UNKNOWN;
    if (!hApp)   return RV_ERROR_UNKNOWN;

    /* Maybe I need to check the remote terminal capabilities? */

    H245APIEnter((app->pAppObject, "cmChannelTSTOCommand: hsChan=%p, tradeoff=%d.", hsChan, tradeoffValue));

    if (emaLock((EMAElement)hsChan))
    {
        HPVT hVal = app->hVal;

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(command),__h245(miscellaneousCommand));
        pvtAdd(hVal,nodeId,__h245(logicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
        pvtAdd(hVal,pvtAddBranch(hVal,nodeId,__h245(type)),__h245(videoTemporalSpatialTradeOff),tradeoffValue,NULL,NULL);

        res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelTSTOCommand=%d", res));
    return res;
}



RVAPI int RVCALLCONV
cmChannelTSTOIndication(
             /* Send temporal spatial trade off indication. Indicates the current tradeoff value
              of the local terminal. */
             IN HCHAN hsChan, /* outgoing channel */
             IN int tradeoffValue /* 0-31 */
             )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int message, nodeId, res = RV_ERROR_UNKNOWN;
    H245Object*app=(H245Object*)hApp;

    if (!hsChan || tradeoffValue<0 || tradeoffValue>31) return RV_ERROR_UNKNOWN;
    if (!hApp)   return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelTSTOIndication: hsChan=%p, tradeoff=%d.", hsChan, tradeoffValue));

    if (emaLock((EMAElement)hsChan))
    {
        HPVT hVal = app->hVal;

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(indication),__h245(miscellaneousIndication));
        pvtAdd(hVal,nodeId,__h245(logicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
        pvtAdd(hVal,pvtAddBranch(hVal,nodeId,__h245(type)),__h245(videoTemporalSpatialTradeOff),tradeoffValue,NULL,NULL);

        res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelTSTOIndication=%d"));
    return res;
}

/* Media Loop_____________________________________________________________________________*/




RVAPI int RVCALLCONV
cmChannelMediaLoopRequest(
              /* Request media loop on this channel */
              IN HCHAN hsChan /* outgoing channel */
              )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int message;
    int ret = RV_ERROR_UNKNOWN;
    H245Object*app=(H245Object*)hApp;

    if (!hsChan || !hApp)   return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelMediaLoopRequest: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        if (channel->bOrigin)
        {
            HPVT hVal = app->hVal;
            RvBool sentOnDummyHost;

            message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
            __pvtBuildByFieldIds(ret,hVal,message, {_h245(request) _h245(maintenanceLoopRequest) _h245(type) _h245(mediaLoop) LAST_TOKEN},
                                 channel->myLogicalChannelNum,NULL);

            ret = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, &sentOnDummyHost);

            if ((ret >= 0) && !sentOnDummyHost)
            {
                int timeout=10;
                pvtGetChildByFieldId(app->hVal,app->h245Conf,__h245(mediaLoopTimeout),&(timeout),NULL);
                app->evHandlers.pfnTimerCancelEv(channel->hCtrl, &channel->pMediaLoopTimer);
                channel->pMediaLoopTimer = app->evHandlers.pfnTimerStartEv(channel->hCtrl,
                    channelML_TimeoutEventsHandler, (void*)channel, timeout*1000);
            }
        }
        else
        {
            /* This operation is valid only for outgoing channels */
            ret = RV_ERROR_BADPARAM;
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelMediaLoopRequest: [%d].", ret));
    return ret;
}



RVAPI int RVCALLCONV
cmChannelMediaLoopConfirm(
              /* Confirm media loop request on this channel */
              IN HCHAN hsChan /* incoming channel */
              )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int message, res = RV_ERROR_UNKNOWN;
    H245Object*app=(H245Object*)hApp;
    int nesting;

    if (!hsChan || !hApp)   return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelMediaLoopConfirm: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        HPVT hVal = app->hVal;

        if (app->cmMyChannelEvent.cmEvChannelMediaLoopStatus)
        {
            H245CBEnter((app->pAppObject, "cmEvChannelMediaLoopStatus: haChan=%p, hsChan=%p, status=confirm.",(HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), channel));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelMediaLoopStatus((HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel, cmMediaLoopConfirm);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMediaLoopStatus."));
        }
        if (!emaWasDeleted((EMAElement)channel))
        {
            message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
            __pvtBuildByFieldIds(res,hVal,message, {_h245(response) _h245(maintenanceLoopAck) _h245(type) _h245(mediaLoop) LAST_TOKEN},
                                 channel->myLogicalChannelNum,NULL);

            res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelMediaLoopConfirm=%d", res));
    return res;
}


RVAPI int RVCALLCONV
cmChannelMediaLoopReject(
             /* Reject media loop request on this channel */
             IN HCHAN hsChan /* incoming channel */
             )
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int message, nodeId, res = RV_ERROR_UNKNOWN;
    H245Object*app=(H245Object*)hApp;
    int nesting;

    if (!hsChan || !hApp)   return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelMediaLoopReject: hsChan=%p.", hsChan));

    if (emaLock((EMAElement)hsChan))
    {
        HPVT hVal = app->hVal;

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(response), __h245(maintenanceLoopReject));
        pvtAdd(hVal,pvtAddBranch(hVal,nodeId,__h245(type)), __h245(mediaLoop),channel->myLogicalChannelNum,NULL,NULL);
        pvtAddBranch2(hVal,nodeId,__h245(cause), __h245(canNotPerformLoop));

        res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);

        if ((res >= 0) && (app->cmMyChannelEvent.cmEvChannelMediaLoopStatus != NULL))
        {
            HAPPCHAN haChan = (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel);
            H245CBEnter((app->pAppObject, "cmEvChannelMediaLoopStatus(haChan=%p,hsChan=%p,status=off)", haChan, channel));
            nesting = emaPrepareForCallback((EMAElement)channel);
            app->cmMyChannelEvent.cmEvChannelMediaLoopStatus(haChan, (HCHAN)channel, cmMediaLoopOff);
            emaReturnFromCallback((EMAElement)channel, nesting);
            H245CBExit((app->pAppObject, "cmEvChannelMediaLoopStatus"));
        }
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelMediaLoopReject=%d"));
    return res;
}


RVAPI int RVCALLCONV
cmCallMediaLoopOff(
           /* Release all media loops in this call */
           IN HCALL hsCall
           )
{
    int message, res = RV_ERROR_UNKNOWN;
    H245Channel*channel;
    void* ptr=NULL;
    int nesting;
    HPVT hVal;

    if (emaLock((EMAElement)hsCall))
    {
        HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        HCONTROL ctrl=cmiGetControl(hsCall);
        H245Object*app=(H245Object*)hApp;

        H245APIEnter((app->pAppObject, "cmCallMediaLoopOff: hsCall=%p.", hsCall));

        if (hApp == NULL)
        {
            emaUnlock((EMAElement)hsCall);
            return RV_ERROR_UNKNOWN;
        }
        hVal = app->hVal;

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        pvtAddBranch2(hVal,message, __h245(command), __h245(maintenanceLoopOffCommand));

        res = sendMessageH245(ctrl, message, RV_TRUE, NULL);

        if ((res >= 0) && (app->cmMyChannelEvent.cmEvChannelMediaLoopStatus != NULL))
        {
            channel = getNextOutChan(ctrl, &ptr);
            while (channel != NULL)
            {
                if (emaLock((EMAElement)channel))
                {
                    if (channel->eState != ChannelStateReleased)
                    {
                        HAPPCHAN haChan = (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel);
                        H245CBEnter((app->pAppObject, "cmEvChannelMediaLoopStatus(haChan=%p,hsChan=%p,status=off)", haChan, channel));
                        nesting = emaPrepareForCallback((EMAElement)channel);
                        app->cmMyChannelEvent.cmEvChannelMediaLoopStatus(haChan, (HCHAN)channel, cmMediaLoopOff);
                        emaReturnFromCallback((EMAElement)channel, nesting);
                        H245CBExit((app->pAppObject, "cmEvChannelMediaLoopStatus"));
                    }
                    emaUnlock((EMAElement)channel);
                }
                channel = getNextOutChan(ctrl, &ptr);
            }
        }
        H245APIExit((app->pAppObject, "cmCallMediaLoopOff=%d", res));
        emaUnlock((EMAElement)hsCall);
    }

    return res;
}


RVAPI int RVCALLCONV
cmChannelSendMiscCommand(
         IN     HCHAN       hsChan,
         IN     cmMiscellaneousCommand miscCommand)
{
    HH245       hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel *channel=(H245Channel*)hsChan;
    int         message, nodeId, res = RV_ERROR_UNKNOWN;
    H245Object  *app=(H245Object*)hApp;
    int         fieldId;
    RvUint16    lcn = 0;

    if (!hsChan || !hApp)   return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSendMiscCommand: hsChan=%p. command %d", hsChan,miscCommand));

    if (emaLock((EMAElement)hsChan))
    {
        HPVT hVal = app->hVal;

        switch(miscCommand)
        {
            case cmVideoFreezePicture:          fieldId=__h245(videoFreezePicture);         break;
            case cmVideoSendSyncEveryGOB:       fieldId=__h245(videoSendSyncEveryGOB);      break;
            case cmVideoSendSyncEveryGOBCancel: fieldId=__h245(videoSendSyncEveryGOBCancel);break;
            case cmSwitchReceiveMediaOff:       fieldId=__h245(switchReceiveMediaOff);      break;
            case cmSwitchReceiveMediaOn:        fieldId=__h245(switchReceiveMediaOn);       break;
            case cmVideoFastUpdatePicture:      fieldId=__h245(videoFastUpdatePicture);     break;
            default :
                emaUnlock((EMAElement)hsChan);
                H245APIExit((app->pAppObject, "cmChannelSendMiscCommand: [Invalid Parameter]"));
                return RV_ERROR_UNKNOWN;
        }

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(command),__h245(miscellaneousCommand));

        if ((channel->bIsDuplex) && (channel->bOrigin))
		{
			/* The channel is bi-directional(master), the lcn is always  the reverse lcn */
			lcn = channel->reverseLogicalChannelNum;
		}
		else
		{
			/* The channel is uni-directional or bi-directional (slave),   the lcn is always the forward lcn */
			lcn = channel->myLogicalChannelNum;
		}

        pvtAdd(hVal,nodeId,__h245(logicalChannelNumber),lcn,NULL,NULL);

        pvtAddBranch2(hVal,nodeId,__h245(type),fieldId);

        res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSendMiscCommand=%d"));
    return res;
}


#if (RV_H245_LEAN_H223 == RV_NO)
RVAPI int RVCALLCONV
cmChannelSendTransportCapInd(
         IN     HCHAN       hsChan,
         IN     int         transCapNodeId)
{
    HH245 hApp=(HH245)emaGetUserData((EMAElement)hsChan);
    H245Channel*channel=(H245Channel*)hsChan;
    int message, nodeId, res = RV_ERROR_UNKNOWN;
    H245Object*app=(H245Object*)hApp;

    if (!hsChan || !hApp)   return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmChannelSendTransportCapInd: hsChan=%p Capability nodeId %d.", hsChan,transCapNodeId));

    if (emaLock((EMAElement)hsChan))
    {
        HPVT hVal = app->hVal;

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(indication),__h245(miscellaneousIndication));
        pvtAdd(hVal,nodeId,__h245(logicalChannelNumber),channel->myLogicalChannelNum,NULL,NULL);
        pvtMoveTree(app->hVal,pvtAddBranch2(hVal,nodeId,__h245(type),__h245(transportCapability)),transCapNodeId);

        res = sendMessageH245Chan(channel->hCtrl, hsChan, message, RV_TRUE, NULL);
        emaUnlock((EMAElement)hsChan);
    }

    H245APIExit((app->pAppObject, "cmChannelSendTransportCapInd=%d", res));
    return res;
}
#endif


#ifdef __cplusplus
}
#endif
