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
#include "cmH245GeneralDefs.h"
#include "cmictrl.h"
#include "cmConf.h"
#include "caputils.h"
#include "strutils.h"
#include "prnutils.h"
#include "cmchan.h"
#include "h245.h"
#include "cmCtrlCap.h"
#include "cmCtrlMSD.h"
#include "cmCtrlMES.h"
#include "cmCtrlRTD.h"
#include "cmCtrlMMode.h"
#include "cmCtrlRMode.h"
#include "cmCtrlMPoint.h"
#include "cmChanOperations.h"
#include "pvaltree.h"
#include "cmiParams.h"
#include "cmH245Object.h"
#include "cmH225Params.h"
#include "cmH223Params.h"

#ifdef __cplusplus
extern "C" {
#endif

#define nprn(s) ((s)?(s):"(null)")

int userInput(H245Control*ctrl, int message);


/* General __________________________________________________________________________________*/

/* an array that holds pointers to the H.225 and H.223 functions */
H22XFunctions h22xFunctions[] = {
    {h225ParamsStartEstablishment,
     h225cmcCallAddressCallbacks,
     h225ParamsOpenLogicalChannelEvent,
     h225ParamsDynamicRTPPayloadType,
     h225MaximumSkewIndication,
     h225communicationModeCommand,
     h225AllocateChannel,
     h225ChannelFreeMemory,
     h255ParamsChannelAnswer,
     h255ParamsOpenLogicalChannelAck,
     h255ParamsOpenLogicalChannelReject,
     h255ParamsOpenLogicalChannelConfirm,
     h225ParamsChannelOpenDynamic,
     h225ParamsCallChannelParametersCallback,
     h225ParamsChannelClose,
     h225ParamsChannelReject,
     h225ParamsDeriveChannels,
     h225ParamsSessionOpenDynamic,
     h225MultiplexReconfiguration,
     h225EndSessionCommand},
    {h223ParamsStartEstablishment,
     h223cmcCallAddressCallbacks,
     h223ParamsOpenLogicalChannelEvent,
     h223ParamsDynamicRTPPayloadType,
     h223MaximumSkewIndication,
     h223communicationModeCommand,
     h223AllocateChannel,
     h223ChannelFreeMemory,
     h223ParamsChannelAnswer,
     h223ParamsOpenLogicalChannelAck,
     h223ParamsOpenLogicalChannelReject,
     h223ParamsOpenLogicalChannelConfirm,
     h223ParamsChannelOpenDynamic,
     h223ParamsCallChannelParametersCallback,
     h223ParamsChannelClose,
     h223ParamsChannelReject,
     h223ParamsDeriveChannels,
     h223ParamsSessionOpenDynamic,
     h223MultiplexReconfiguration,
     h223EndSessionCommand}
};


int cmiReportControl(
    IN HCONTROL             ctrl,
    IN cmControlState       state,
    IN cmControlStateMode   stateMode)
{
    HCALL      hsCall;
    HAPPCALL   hAppCall;
    H245Object *app;
    int        nesting;

    hsCall = cmiGetByControl((HCONTROL)ctrl);
    hAppCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall);

    /* Make sure we need this callback at all */
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    if (app->cmMySessionEvent.cmEvCallControlStateChanged != NULL)
    {
#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
        static const char *stateModes[]=
        {   "cmControlStateModeNull",
            "cmControlStateModeFastStartNoMoreChannels",
            "cmControlStateModeFastStartRejected"
        };
        static const char *states[]=
        {
            "cmControlStateConnected",
            "cmControlStateConference",
            "cmControlStateTransportConnected",
            "cmControlStateTransportDisconnected",
            "cmControlStateFastStart",
            "cmControlStateFastStartComplete"
        };

        H245CBEnter((app->pAppObject, "cmEvCallControlStateChanged(haCall=%p,hsCall=%p,state=%s,stateMode=%s)",
            hAppCall, hsCall, nprn(states[state]), nprn(stateModes[(int)stateMode])));

#endif

        nesting=emaPrepareForCallback((EMAElement)hsCall);
        app->cmMySessionEvent.cmEvCallControlStateChanged(hAppCall,hsCall,state,stateMode);
        emaReturnFromCallback((EMAElement)hsCall,nesting);

#if (RV_LOGMASK & RV_LOGLEVEL_LEAVE)
        H245CBExit((app->pAppObject, "cmEvCallControlStateChanged"));
#endif
    }

    if (!emaWasDeleted((EMAElement)hsCall))
    {
        H245Control* _ctrl = (H245Control*)ctrl;
        /*set bSaveCallIndication if control in ctrlMesFinished state*/
        if (_ctrl->eState == ctrlMesFinished)
        {
            cmiSetSaveCallIndication(ctrl, RV_TRUE);
        }

    }

    /* The check for the Call State Connected is done inside this function. If the state
       is connected than the function does nothing */
    if (app->evHandlers.pfnNotifyStateEv != NULL)
        app->evHandlers.pfnNotifyStateEv(hsCall, ((H245Control*)ctrl)->eState);

#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
    if ((app->mibEvent.h341AddControl != NULL) && ((state == cmControlStateConnected) || (state == cmControlStateFastStart)))
          app->mibEvent.h341AddControl(app->mibHandle, hsCall);
#endif

    return 0;
}


RVAPI int RVCALLCONV
cmSetSessionEventHandler(
             /* Set user callbacks functions for control session. */
             IN HAPP            hApp,
             IN CMSESSIONEVENT  cmSessionEvent,
             IN int             size)
{
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!app) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmSetSessionEventHandler: hApp=%p, cmSessionEvent=%p, size=%d.",
        hApp, cmSessionEvent, size));

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    app->dynamicPayloadNumber = 0;
#endif

    memset(&(app->cmMySessionEvent), 0, sizeof(app->cmMySessionEvent));
    memcpy(&(app->cmMySessionEvent), cmSessionEvent, (RvSize_t)RvMin((int)sizeof(app->cmMySessionEvent), size));

    H245APIExit((app->pAppObject, "cmSetSessionEventHandler=0"));
    return 0;
}


RVAPI int RVCALLCONV
cmGetControlEventHandler(
             /* Set user callbacks functions for control session. */
             IN HAPP        hApp,
             OUT    CMSESSIONEVENT  cmSessionEvent,
             IN     int     size)
{
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!app) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmGetControlEventHandler: hApp=%p, cmSessionEvent=%p, size=%d.",
        hApp, cmSessionEvent, size));

    memset(cmSessionEvent, 0, (RvSize_t)size);
    memcpy(cmSessionEvent,&(app->cmMySessionEvent),  (RvSize_t)RvMin((int)sizeof(app->cmMySessionEvent), size));

    H245APIExit((app->pAppObject, "cmGetControlEventHandler=0"));
    return 0;
}


/* returns the value-tree node id of h245 configuration root node */
RVAPI
RvInt32 RVCALLCONV cmGetH245ConfigurationHandle(
                IN  HAPP             hApp)
{
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);
    if (!app) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmGetH245ConfigurationHandle: hApp=%p.", hApp));

    H245APIExit((app->pAppObject, "cmGetH245ConfigurationHandle=%d", app->h245Conf));
    return app->h245Conf;
}


int
cmcReadyEvent(
          /* Check if session reached ready state and if so inform application. */
          H245Control* ctrl
          )
{
    HCALL   hCall=(HCALL)cmiGetByControl((HCONTROL)ctrl);
    H245Object *hH245Object = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hCall));
    RvBool bIsFastStartFinished = RV_TRUE;

    if (!hH245Object) return RV_ERROR_UNKNOWN;

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    bIsFastStartFinished = ctrl->bIsFastStartFinished;
#endif

    RvLogInfo(&hH245Object->log, (&hH245Object->log,
        "cmcReadyEvent hCall=%p: state=%d, OutCap=%d, InCap=%d, IsMS=%d, IsFSFin=%d", hCall, ctrl->eState,
        ctrl->eOutCapStatus, ctrl->eInCapStatus, ctrl->bIsMasterSlave, bIsFastStartFinished));

    if ((ctrl->eOutCapStatus == CapStatusAcknowledged) && (ctrl->eInCapStatus == CapStatusAcknowledged) &&
        ctrl->bIsMasterSlave && (ctrl->eState == ctrlInitialized) && bIsFastStartFinished)
    {
        if (ctrl->bIsConference)
        {
            ctrl->eState=ctrlConference;
            cmiReportControl((HCONTROL)ctrl, cmControlStateConnected, (cmControlStateMode)0);
        }
        else
        {
            ctrl->eState=ctrlConnected;
            cmiReportControl((HCONTROL)ctrl, cmControlStateConnected, (cmControlStateMode)0);
        }
        if (RV_PVT_NODEID_IS_VALID(ctrl->incomingOLCs[0]))
        {
            openLogicalChannel(ctrl, ctrl->incomingOLCs[0], pvtChild(hH245Object->hVal, pvtChild(hH245Object->hVal, ctrl->incomingOLCs[0])));
            pvtDelete(hH245Object->hVal, ctrl->incomingOLCs[0]);
            ctrl->incomingOLCs[0] = RV_PVT_INVALID_NODEID;
        }
        if (RV_PVT_NODEID_IS_VALID(ctrl->incomingOLCs[1]))
        {
            openLogicalChannel(ctrl, ctrl->incomingOLCs[1], pvtChild(hH245Object->hVal, pvtChild(hH245Object->hVal, ctrl->incomingOLCs[1])));
            pvtDelete(hH245Object->hVal, ctrl->incomingOLCs[1]);
            ctrl->incomingOLCs[1] = RV_PVT_INVALID_NODEID;
        }
    }
    return RV_TRUE;
}


#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
H245ControlState controlGetState(HCONTROL ctrl)
{
    H245Control*ctrlE=(H245Control*)ctrl;
    return ctrlE->eState;
}
#endif

void clearControl(HCONTROL ctrl, RvBool callInitiator, int controlOffset)
{
    H245Control*ctrlE=(H245Control*)ctrl;

    memset((void *)ctrl,0,sizeof(H245Control));
    ctrlE->controlOffset = controlOffset;
    ctrlE->eState = ctrlNotInitialized;

#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
    ctrlE->callInitiator = callInitiator;
#else
    RV_UNUSED_ARG(callInitiator);
#endif
}


void initControl(HCONTROL ctrl, RvH245ChannelParamsType eParamsType, RvInt32 terminalType)
{
    H245Control*ctrlE=(H245Control*)ctrl;

    ctrlE->eOutCapStatus = CapStatusReleased;
    ctrlE->eInCapStatus = CapStatusReleased;
    ctrlE->bIsMasterSlave = RV_FALSE;
    ctrlE->bIsMaster = RV_FALSE;
    ctrlE->conflictChannels = 0;

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    ctrlE->bIsFastStartFinished = RV_TRUE;
    ctrlE->myTerminalLabel.mcuNumber=255;
    ctrlE->myTerminalLabel.terminalNumber=255;
#endif  /* (RV_H245_SUPPORT_H225_PARAMS == 1) */

    ctrlE->eParamsType = eParamsType;
    capInit(ctrlE);
    msdInit(ctrlE, terminalType);
    rtdInit(ctrlE);
    rmInit(ctrlE);
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
    ctrlE->bResetInitiator = RV_FALSE;
    ctrlE->bReset = RV_FALSE;
    mesInit(ctrlE);
#endif

    ctrlE->multiPointMode=0;
    ctrlE->eState=ctrlInitialized;
    ctrlE->outRequestMode.timer = NULL;
    ctrlE->incomingOLCs[0] = RV_PVT_INVALID_NODEID;
    ctrlE->incomingOLCs[1] = RV_PVT_INVALID_NODEID;
#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    if ((ctrlE->fastStartState == fssNo) || (ctrlE->fastStartState == fssRej))
    {
        ctrlE->logicalChannelOut=0;
        ctrlE->hFirstChannel = NULL;
    }
#endif  /* (RV_H245_SUPPORT_H225_PARAMS == 1) */
}

void startControl(HCONTROL ctrl)
{
    H245Control*ctrlE=(H245Control*)ctrl;
    H245Object* app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl(ctrl))));
    HPVT hVal=app->hVal;
    int capNodeId, res = 0;

    ctrlE->bEndSessionSent = RV_FALSE;

    if (pvtGetChild2(hVal,app->h245Conf,__h245(capabilities),__h245(manualOperation))<0 &&
        (capNodeId=(pvtGetChild2(hVal,app->h245Conf,__h245(capabilities),__h245(terminalCapabilitySet)))))
    {
        int nodeId=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        int tcsNodeId=pvtAddBranch2(hVal,nodeId,__h245(request),__h245(terminalCapabilitySet));
        res = pvtSetTree(hVal,tcsNodeId,hVal,capNodeId);
        if (res >= 0)
            res = outCapTransferRequest(ctrlE, nodeId);
        else
            pvtDelete(hVal,nodeId);
    }
    if ((res >= 0) && (pvtGetChild2(hVal,app->h245Conf,__h245(masterSlave),__h245(manualOperation))<0))
        msdDetermineRequest(ctrlE, -1, -1);
}


/************************************************************************
 * stopControl
 * purpose: Stop the H245 Control connection for a call.
 * input  : ctrl    - Control object
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
void stopControl(IN HCONTROL ctrl)
{
    resetControl(ctrl, RV_TRUE);
}


/******************************************************************************
 * endControl
 * ----------------------------------------------------------------------------
 * General: Stop the timers used by the control object of a given call,
 *          without sending anything on to the network.
 *          This is used by the stopControl() function and when we want to
 *          switch from a dummy control to a real one in the GK.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ctrl     - Control object to use.
 * Output: None.
 *****************************************************************************/
void endControl(IN HCONTROL ctrl)
{
    H245Control *ctrlE = (H245Control *)ctrl;

    /* Finish with the capabilities, MSD and RTD procedures */
    capEnd(ctrlE);
    msdEnd(ctrlE);
    rtdEnd(ctrlE);
    rmEnd(ctrlE);
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
    mesEnd(ctrlE);
#endif
}


/******************************************************************************
 * freeControl
 * ----------------------------------------------------------------------------
 * General: Clear up any PVT resources used by the control object of a given call,
 *          without sending anything on to the network.
 *          This is used by the stopControl() function and when we want to
 *          switch from a dummy control to a real one in the GK.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ctrl     - Control object to use.
 * Output: None.
 *****************************************************************************/
void freeControl(IN HCONTROL ctrl)
{
    H245Control *ctrlE = (H245Control *)ctrl;

    if (ctrlE->eState == ctrlNotInitialized)
        return;

    /* Finish with the capabilities, MSD and RTD procedures */
    capFree(ctrlE);
}

/************************************************************************
 * resetControl
 * purpose: Reset the H245 Control connection for a call.
 * input  : ctrl         - Control object
 *          bDropControl - Defines whether the control is totally dropped
 *          and only reset. The function stopControl() should call this function
 *          with RV_TRUE.
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
void resetControl(IN HCONTROL ctrl, RvBool bDropControl)
{
    H245Control *ctrlE=(H245Control*)ctrl;
    H245Object  *app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl(ctrl))));
    RvPvtNodeId nodeId;
    RvPvtNodeId message = RV_PVT_INVALID_NODEID;
    HPVT        hVal = app->hVal;
    RvBool      hasControl = ((ctrlE->eState != ctrlNotInitialized) && (ctrlE->eState != ctrlEndSession) &&
                               ctrlE->bIsControl);

    /* First update the status so we don't get into resetControl() twice from 2 different
       threads: one on the incoming endSessionCommand and one for an outgoing one */
    ctrlE->bIsControl = RV_FALSE;

    /* if there was never any control, there is nothing to do. */
    if (ctrlE->eState == ctrlNotInitialized)
        return;

    /* Make sure we're in the endSession state */
    ctrlE->eState = ctrlEndSession;

#if (RV_H245_SUPPORT_H223_PARAMS == 1)
    /*reset ACP flag so channels are not reopened if we work with autocaps*/
    ctrlE->bACPinUse = RV_FALSE;
#endif /* #if (RV_H245_SUPPORT_H223_PARAMS == 1) */

    /* Make sure we close all the channels */
    closeChannels(ctrl);

    /* Send an endSession message if we have to */
    if (hasControl)
    {
        message = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);
        nodeId = pvtAddBranch2(hVal, message, __h245(command), __h245(endSessionCommand));

        if (bDropControl == RV_TRUE)
        {
            /* Create an endSession command message */
            pvtAdd(hVal, nodeId, __h245(disconnect), 0, NULL, NULL);
        }
        else
        {
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
            /* Create an endSession command message */

            __pvtBuildByFieldIds(nodeId, hVal, nodeId, {_h245(gstnOptions) _h245(v34H324) LAST_TOKEN}, RV_TRUE, NULL);
            ctrlE->bReset = RV_TRUE;
#endif /* (RV_H245_SUPPORT_H223_PARAMS == 1) */
        }

        /* Send the endSession message */
        sendMessageH245(ctrl, message, RV_TRUE, NULL);
        ctrlE->bEndSessionSent = RV_TRUE;
    }

    endControl(ctrl);

    /* Clean up any pending OLCs that were never processed */
    if (RV_PVT_NODEID_IS_VALID(ctrlE->incomingOLCs[0]))
    {
        pvtDelete(hVal, ctrlE->incomingOLCs[0]);
        ctrlE->incomingOLCs[0] = RV_PVT_INVALID_NODEID;
    }
    if (RV_PVT_NODEID_IS_VALID(ctrlE->incomingOLCs[1]))
    {
        pvtDelete(hVal, ctrlE->incomingOLCs[1]);
        ctrlE->incomingOLCs[1] = RV_PVT_INVALID_NODEID;
    }

    if (bDropControl == RV_TRUE)
    {
        /* report that the control has disconnected */
        if (hasControl == RV_TRUE
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
            || ctrlE->bReset != RV_FALSE
#endif /* (RV_H245_SUPPORT_H223_PARAMS == 1) */
        )
        {
            cmiReportControl(ctrl, cmControlStateTransportDisconnected, (cmControlStateMode)0);
        }
    }
}


/************************************************************************
 * endSessionCommand
 * purpose: Handle an incoming endSessionCommand message.
 *          This should stop the control of the call.
 * input  : ctrl    - Control object
 *          message - endSessionCommand message received
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
static int endSessionCommand(IN H245Control* ctrl, IN int message)
{
    RV_UNUSED_ARG(message);

    h22xFunctions[ctrl->eParamsType].EndSessionCommand(ctrl, message);

    return RV_TRUE;
}


HCHAN cmH245GetChannelList(HCONTROL hControlChan)
{
    return ((H245Control*)hControlChan)->hFirstChannel;
}

void cmH245SetChannelList(HCONTROL hControlChan,HCHAN ch)
{
    ((H245Control*)hControlChan)->hFirstChannel = ch;
}



void h245ProcessIncomingMessage(HCONTROL ctrl, int message)
{
    H245Control*ctrlE=(H245Control*)ctrl;
    H245Object* app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl(ctrl))));
    HPVT hVal=app->hVal;
    RvPvtNodeId child = pvtChild(hVal, message);
    RvPvtNodeId grandchild = pvtChild(hVal, child);
    RvPstFieldId typeFieldId, messageFieldId;

    if (!ctrlE->bIsControl)
    {
        /* no control - don't let anything go through */
        return;
    }

    pvtGet(hVal, child, &typeFieldId, NULL, NULL, NULL);
    pvtGet(hVal, grandchild, &messageFieldId, NULL, NULL, NULL);

    switch (typeFieldId)
    {
        case    __h245(request):
            switch (messageFieldId)
            {
                case __h245(nonStandard)                :break;
                case __h245(masterSlaveDetermination)   :  masterSlaveDetermination(ctrlE,grandchild);          break;
                case __h245(terminalCapabilitySet)      :  terminalCapabilitySet(ctrlE,grandchild);             break;
                case __h245(openLogicalChannel)         :  openLogicalChannel(ctrlE,message,grandchild);        break;
                case __h245(closeLogicalChannel)        :  closeLogicalChannel(ctrlE,message,grandchild);       break;
                case __h245(requestChannelClose)        :  requestChannelClose(ctrlE,message,grandchild);       break;
                case __h245(multiplexEntrySend)         :  multiplexEntrySend(ctrlE,grandchild);                break;
                case __h245(requestMultiplexEntry)      :break;
                case __h245(requestMode)                :  requestMode(ctrlE,grandchild);                       break;
                case __h245(roundTripDelayRequest)      :  roundTripDelayRequest(ctrlE,grandchild);             break;
                case __h245(maintenanceLoopRequest)     :  maintenanceLoopRequest(ctrlE,message,grandchild);    break;
#if (RV_H245_LEAN_H223 == RV_NO)
                case __h245(communicationModeRequest)   :break;
                case __h245(conferenceRequest)          :
                    {
                        RvPstFieldId fieldEnum;
                        RvPvtNodeId nodeId = pvtChild(hVal, grandchild);
                        pvtGet(hVal, nodeId, &fieldEnum, NULL, NULL, NULL);
                        switch (fieldEnum)
                        {
                            case __h245(terminalListRequest)        : conferenceMessage(ctrlE,0,grandchild,h245crqTerminalListRequest); break;
                            case __h245(makeMeChair)                : conferenceMessage(ctrlE,0,grandchild,h245crqMakeMeChair); break;
                            case __h245(cancelMakeMeChair)          : conferenceMessage(ctrlE,0,grandchild,h245crqCancelMakeMeChair); break;
                            case __h245(dropTerminal)               : conferenceMessage(ctrlE,0,grandchild,h245crqDropTerminal); break;
                            case __h245(requestTerminalID)          : conferenceMessage(ctrlE,0,grandchild,h245crqRequestTerminalID); break;
                            case __h245(enterH243Password)          : conferenceMessage(ctrlE,0,grandchild,h245crqEnterH243Password); break;
                            case __h245(enterH243TerminalID)        : conferenceMessage(ctrlE,0,grandchild,h245crqEnterH243TerminalID); break;
                            case __h245(enterH243ConferenceID)      : conferenceMessage(ctrlE,0,grandchild,h245crqEnterH243ConferenceID); break;
                            case __h245(requestChairTokenOwner)     : conferenceMessage(ctrlE,0,grandchild,h245crqRequestChairTokenOwner); break;
                            case __h245(requestTerminalCertificate) : conferenceMessage(ctrlE,0,grandchild,h245crqRequestTerminalCertificate); break;
                            case __h245(broadcastMyLogicalChannel)  : conferenceMessage(ctrlE,0,grandchild,h245crqBroadcastMyLogicalChannel); break;
                            case __h245(makeTerminalBroadcaster)    : conferenceMessage(ctrlE,0,grandchild,h245crqMakeTerminalBroadcaster); break;
                            case __h245(sendThisSource)             : conferenceMessage(ctrlE,0,grandchild,h245crqSendThisSource); break;
                            case __h245(requestAllTerminalIDs)      : conferenceMessage(ctrlE,0,grandchild,h245crqRequestAllTerminalIDs); break;
                            case __h245(remoteMCRequest)            : conferenceMessage(ctrlE,0,grandchild,h245crqRemoteMCRequest); break;
                            default:break;
                        }
                    }
                    break;
                case __h245(multilinkRequest)           :break;
                case __h245(logicalChannelRateRequest)  :break;
#endif
                case __h245(genericRequest)             : conferenceMessage(ctrlE,0,grandchild,h245grqGenericRequest); break;
                default:break;
            }
        break;
        case    __h245(response):
            switch (messageFieldId)
            {
                case __h245(nonStandard)                    :break;
                case __h245(masterSlaveDeterminationAck)    :  masterSlaveDeterminationAck(ctrlE,grandchild);    break;
                case __h245(masterSlaveDeterminationReject) :  masterSlaveDeterminationReject(ctrlE,grandchild); break;
                case __h245(terminalCapabilitySetAck)       :  terminalCapabilitySetAck(ctrlE,grandchild);       break;
                case __h245(terminalCapabilitySetReject)    :  terminalCapabilitySetReject(ctrlE,grandchild);    break;
                case __h245(openLogicalChannelAck)          :  openLogicalChannelAck(ctrlE,message,grandchild);  break;
                case __h245(openLogicalChannelReject)       :  openLogicalChannelReject(ctrlE,message,grandchild);break;
                case __h245(closeLogicalChannelAck)         :  closeLogicalChannelAck(ctrlE,message,grandchild); break;
                case __h245(requestChannelCloseAck)         :  requestChannelCloseAck(ctrlE,message,grandchild); break;
                case __h245(requestChannelCloseReject)      :  requestChannelCloseReject(ctrlE,message,grandchild);break;
                case __h245(multiplexEntrySendAck)          :  multiplexEntrySendAck(ctrlE, grandchild);         break;
                case __h245(multiplexEntrySendReject)       :  multiplexEntrySendReject(ctrlE, grandchild);      break;
                case __h245(requestMultiplexEntryAck)       :break;
                case __h245(requestMultiplexEntryReject)    :break;
                case __h245(requestModeAck)                 :  requestModeAck(ctrlE,grandchild);                 break;
                case __h245(requestModeReject)              :  requestModeReject(ctrlE,grandchild);              break;
                case __h245(roundTripDelayResponse)         :  roundTripDelayResponse(ctrlE,grandchild);         break;
                case __h245(maintenanceLoopAck)             :  maintenanceLoopAck(ctrlE,message,grandchild);     break;
                case __h245(maintenanceLoopReject)          :  maintenanceLoopReject(ctrlE,message,grandchild);  break;
#if (RV_H245_LEAN_H223 == RV_NO)
                case __h245(communicationModeResponse)      :break;
                case __h245(conferenceResponse)             :
                    {
                        RvPstFieldId fieldEnum;
                        RvPvtNodeId nodeId = pvtChild(hVal, grandchild);
                        pvtGet(hVal, nodeId, &fieldEnum, NULL, NULL, NULL);
                        switch (fieldEnum)
                        {
                            case __h245(terminalIDResponse)                 : conferenceMessage(ctrlE,0,grandchild,h245crsTerminalIDResponse); break;
                            case __h245(conferenceIDResponse)               : conferenceMessage(ctrlE,0,grandchild,h245crsConferenceIDResponse); break;
                            case __h245(passwordResponse)                   : conferenceMessage(ctrlE,0,grandchild,h245crsPasswordResponse); break;
                            case __h245(terminalListResponse)               : conferenceMessage(ctrlE,0,grandchild,h245crsTerminalListResponse); break;
                            case __h245(videoCommandReject)                 : conferenceMessage(ctrlE,0,grandchild,h245crsVideoCommandReject); break;
                            case __h245(terminalDropReject)                 : conferenceMessage(ctrlE,0,grandchild,h245crsTerminalDropReject); break;
                            case __h245(makeMeChairResponse)                : conferenceMessage(ctrlE,0,grandchild,h245crsMakeMeChairResponse); break;
                            case __h245(chairTokenOwnerResponse)            : conferenceMessage(ctrlE,0,grandchild,h245crsChairTokenOwnerResponse); break;
                            case __h245(terminalCertificateResponse)        : conferenceMessage(ctrlE,0,grandchild,h245crsTerminalCertificateResponse); break;
                            case __h245(broadcastMyLogicalChannelResponse)  : conferenceMessage(ctrlE,0,grandchild,h245crsBroadcastMyLogicalChannelResponse); break;
                            case __h245(makeTerminalBroadcasterResponse)    : conferenceMessage(ctrlE,0,grandchild,h245crsMakeTerminalBroadcasterResponse); break;
                            case __h245(sendThisSourceResponse)             : conferenceMessage(ctrlE,0,grandchild,h245crsSendThisSourceResponse); break;
                            case __h245(requestAllTerminalIDsResponse)      : conferenceMessage(ctrlE,0,grandchild,h245crsRequestAllTerminalIDsResponse); break;
                            case __h245(remoteMCResponse)                   : conferenceMessage(ctrlE,0,grandchild,h245crsRemoteMCResponse); break;
                            default:break;
                        }
                    }
                    break;
                case __h245(multilinkResponse)              :break;
                case __h245(logicalChannelRateAcknowledge)  :break;
                case __h245(logicalChannelRateReject)       :break;
#endif
                case __h245(genericResponse)                : conferenceMessage(ctrlE,0,grandchild,h245grsGenericResponse); break;
                default:break;
            }
        break;
        case    __h245(command):
            switch (messageFieldId)
            {
                case __h245(nonStandard)                    :break;
                case __h245(maintenanceLoopOffCommand)      :maintenanceLoopOffCommand(ctrlE,grandchild);    break;
                case __h245(sendTerminalCapabilitySet)      :break;/*sendTerminalCapabilitySet(ctrlE, grandchild);   break;*/
#if (RV_H245_LEAN_H223 == RV_NO)
                case __h245(encryptionCommand)              :break;
#endif
                case __h245(flowControlCommand)             :flowControlCommand(ctrlE,message,grandchild);   break;
                case __h245(endSessionCommand)              :endSessionCommand(ctrlE,grandchild);            break;
                case __h245(miscellaneousCommand)           :
                {
                    RvInt32 lcn;
                    RvPvtNodeId grandgrandchild=pvtChild(hVal,pvtGetChild(hVal,grandchild,__h245(type),NULL));
                    RvPstFieldId mcFieldId;
                    pvtGet(hVal,grandgrandchild, &mcFieldId, NULL, NULL, NULL);
                    pvtGetChildByFieldId(hVal,grandchild,__h245(logicalChannelNumber),&lcn,NULL);
                    switch(mcFieldId)
                    {
                        case __h245(equaliseDelay)                       :  miscellaneousCommand(ctrlE,lcn,grandchild);               break;
                        case __h245(zeroDelay)                           :  miscellaneousCommand(ctrlE,lcn,grandchild);               break;
#if (RV_H245_LEAN_H223 == RV_NO)
                        case __h245(multipointModeCommand)               :  multipointModeCommand(ctrlE,lcn);                         break;
                        case __h245(cancelMultipointModeCommand)         :  cancelMultipointModeCommand(ctrlE,lcn);                   break;
#endif
                        case __h245(videoFreezePicture)                  :  videoFreezePicture(ctrlE,message,lcn);                    break;
                        case __h245(videoFastUpdatePicture)              :  videoFastUpdatePicture(ctrlE,message,lcn);                break;
                        case __h245(videoFastUpdateGOB)                  :  videoFastUpdateGOB(ctrlE,message,grandgrandchild,lcn);    break;
                        case __h245(videoTemporalSpatialTradeOff)        :  videoTemporalSpatialTradeOff(ctrlE,message,grandgrandchild,lcn,1);break;
                        case __h245(videoSendSyncEveryGOB)               :  videoSendSyncEveryGOB(ctrlE,message,lcn);                 break;
                        case __h245(videoSendSyncEveryGOBCancel)         :  videoSendSyncEveryGOBCancel(ctrlE,message,lcn);           break;
                        case __h245(videoFastUpdateMB)                   :  videoFastUpdateMB(ctrlE,message,grandgrandchild,lcn);     break;
                        case __h245(maxH223MUXPDUsize)                   :  miscellaneousCommand(ctrlE,lcn,grandchild);
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
                                                                            maxH223MUXPDUsize(ctrlE, grandgrandchild);
#endif /* RV_H245_SUPPORT_H223_PARAMS == 1 */
                                                                            break;
#if (RV_H245_LEAN_H223 == RV_NO)
                        case __h245(encryptionUpdate)                    :  miscellaneousCommand(ctrlE,lcn,grandchild);               break;
//{{gaoshizhong 20131122 Add
						case __h245(encryptionUpdateRequest)             :  miscellaneousCommand(ctrlE,lcn,pvtGetChild(hVal,grandchild,__h245(type),NULL)/*grandchild*/);               break;
						case __h245(encryptionUpdateCommand)             :  miscellaneousCommand(ctrlE,lcn,pvtGetChild(hVal,grandchild,__h245(type),NULL)/*grandchild*/);               break;
						case __h245(encryptionUpdateAck)                 :  miscellaneousCommand(ctrlE,lcn,pvtGetChild(hVal,grandchild,__h245(type),NULL)/*grandchild*/);               break;
//}}gaoshizhong 20131122 Add

#endif
                        case __h245(switchReceiveMediaOff)               :  switchReceiveMediaOff(ctrlE, message, lcn);               break;
                        case __h245(switchReceiveMediaOn)                :  switchReceiveMediaOn(ctrlE, message, lcn);                break;
                        case __h245(progressiveRefinementStart)          :break;
                        case __h245(progressiveRefinementAbortOne)       :break;
                        case __h245(progressiveRefinementAbortContinuous):break;
                        case __h245(videoBadMBs)                         :break;
                        case __h245(lostPicture)                         :break;
                        case __h245(lostPartialPicture)                  :break;
                        case __h245(recoveryReferencePicture)            :break;
                        default:break;
                    }
                }
                break;
#if (RV_H245_LEAN_H223 == RV_NO)
                case __h245(communicationModeCommand)       :  h22xFunctions[ctrlE->eParamsType].communicationModeCommand(ctrlE,grandchild);  break;
                case __h245(conferenceCommand)              :  conferenceCommand(ctrlE,grandchild);         break;
#endif
                case __h245(h223MultiplexReconfiguration)   :  h22xFunctions[ctrlE->eParamsType].MultiplexReconfiguration(ctrlE, grandchild); break;
#if (RV_H245_LEAN_H223 == RV_NO)
                case __h245(newATMVCCommand)                :break;
                case __h245(mobileMultilinkReconfigurationCommand):break;
#endif
                case __h245(genericCommand)                 : conferenceMessage(ctrlE,0,grandchild,h245gcGenericCommand); break;
                default:break;
            }
        break;
        case    __h245(indication):
            switch(messageFieldId)
            {
                case __h245(nonStandard)                        :break;
                case __h245(functionNotUnderstood)              :break;
                case __h245(masterSlaveDeterminationRelease)    :  masterSlaveDeterminationRelease(ctrlE,grandchild);   break;
                case __h245(terminalCapabilitySetRelease)       :  terminalCapabilitySetRelease(ctrlE, grandchild);     break;
                case __h245(openLogicalChannelConfirm)          :  openLogicalChannelConfirm(ctrlE,message,grandchild); break;
                case __h245(requestChannelCloseRelease)         :  requestChannelCloseRelease(ctrlE,message,grandchild); break;
                case __h245(multiplexEntrySendRelease)          :  multiplexEntrySendRelese(ctrlE, grandchild);         break;
                case __h245(requestMultiplexEntryRelease)       :break;
                case __h245(requestModeRelease)                 :  requestModeRelease(ctrlE, grandchild);               break;
                case __h245(miscellaneousIndication):
                {
                    RvInt32 lcn;
                    RvPvtNodeId grandgrandchild=pvtChild(hVal,pvtGetChild(hVal,grandchild,__h245(type),NULL));
                    RvPstFieldId mcFieldId;
                    pvtGet(hVal,grandgrandchild, &mcFieldId, NULL, NULL, NULL);
                    pvtGetChildByFieldId(hVal,grandchild,__h245(logicalChannelNumber),&lcn,NULL);
                    switch(mcFieldId)
                    {
                        case __h245(logicalChannelActive)           :logicalChannelActive(ctrlE,message,lcn);break;
                        case __h245(logicalChannelInactive)         :logicalChannelInactive(ctrlE,message,lcn);break;
#if (RV_H245_LEAN_H223 == RV_NO)
                        case __h245(multipointConference)           :multipointConference(ctrlE,lcn); break;
                        case __h245(cancelMultipointConference)     :cancelMultipointConference(ctrlE,lcn); break;
#endif
                        case __h245(multipointZeroComm)             :conferenceMessage (ctrlE,lcn,grandgrandchild,h245miMultipointZeroComm);  break;
                        case __h245(cancelMultipointZeroComm)       :conferenceMessage (ctrlE,lcn,grandgrandchild,h245miCancelMultipointZeroComm);  break;
                        case __h245(multipointSecondaryStatus)      :conferenceMessage (ctrlE,lcn,grandgrandchild,h245miMultipointSecondaryStatus);  break;
                        case __h245(cancelMultipointSecondaryStatus):conferenceMessage (ctrlE,lcn,grandgrandchild,h245miCancelMultipointSecondaryStatus);  break;
                        case __h245(videoIndicateReadyToActivate)   :conferenceMessage (ctrlE,lcn,grandgrandchild,h245miVideoIndicateReadyToActivate);  break;
                        case __h245(videoTemporalSpatialTradeOff)   :videoTemporalSpatialTradeOff(ctrlE,message,grandgrandchild,lcn,0); break;
                        case __h245(videoNotDecodedMBs)             :conferenceMessage (ctrlE,lcn,grandgrandchild,h245miVideoNotDecodedMBs);  break;
#if (RV_H245_LEAN_H223 == RV_NO)
                        case __h245(transportCapability)            :transportCapability(ctrlE,message,grandgrandchild,lcn); break;
#endif
                        default:break;
                    }
                }
                break;
#if (RV_H245_LEAN_H223 == RV_NO)
                case __h245(jitterIndication)               :  indication(ctrlE,0,grandchild);                  break;
#endif
                case __h245(h223SkewIndication)             :  h22xFunctions[ctrlE->eParamsType].maximumSkewIndication(ctrlE,grandchild);    break;
#if (RV_H245_LEAN_H223 == RV_NO)
                case __h245(newATMVCIndication)             :break;
#endif
                case __h245(userInput)                      :  userInput(ctrlE,grandchild);                     break;
#if (RV_H245_SUPPORT_H225_PARAMS == 1)
                case __h245(h2250MaximumSkewIndication)     :  h22xFunctions[ctrlE->eParamsType].maximumSkewIndication(ctrlE,grandchild);    break;
                case __h245(mcLocationIndication)           :  mcLocationIndication(ctrlE,grandchild);          break;
#endif  /* (RV_H245_SUPPORT_H225_PARAMS == 1) */
#if (RV_H245_LEAN_H223 == RV_NO)
                case __h245(conferenceIndication)           :  conferenceIndication(ctrlE,grandchild);          break;
#endif
                case __h245(vendorIdentification)           :  vendorIdentification(ctrlE, grandchild);         break;
                case __h245(functionNotSupported)           :  indication(ctrlE,0,grandchild);                  break;
#if (RV_H245_LEAN_H223 == RV_NO)
                case __h245(multilinkIndication)            :break;
                case __h245(logicalChannelRateRelease)      :break;
#endif

//{{gaoshizhong 20130705 Add
                //case __h245(flowControlIndication)          :  indication(ctrlE,0,grandchild);                  break;
				case __h245(flowControlIndication)          :  indication(ctrlE,0,child);                  break;
//}}gaoshizhong 20130705 Add

#if (RV_H245_LEAN_H223 == RV_NO)
                case __h245(mobileMultilinkReconfigurationIndication):break;
#endif
                case __h245(genericIndication)              :  conferenceMessage(ctrlE,0,grandchild,h245giGenericIndication); break;
                default:break;
            }
        break;
        default:break;
    }
}


/************************************************************************
 * sendMessageH245
 * purpose: Send H245 message for a given call.
 * input  : hsCall  - Stack handle for the H245 call control
 *          message - root node ID of the message to send.
 *                    This function won't delete this node ID.
 *          bDelete - Defines whether to delete the message nodeId
 *                    inside the function or not.
 * output : none
 * return : RV_OK on success
 *          Other on failure
 ************************************************************************/
RvStatus sendMessageH245(
    IN  HCONTROL     ctrl,
    IN  RvPvtNodeId  message,
    IN  RvBool       bDelete,
    OUT RvBool*      sentOnDummyHost)
{
    HCALL             hCall=(HCALL)cmiGetByControl(ctrl);
    HAPP              hApp=(HAPP)emaGetInstance((EMAElement)hCall);
    H245Object        *pH245 = (H245Object*)cmiGetH245Handle(hApp);
    HPVT              hVal = pH245->hVal;
    RvStatus          status = RV_ERROR_UNINITIALIZED;

    /* Make sure we have a message to send (we might not have one due to some resource problems) */
    if (!RV_PVT_NODEID_IS_VALID(message))
        return RV_ERROR_OUTOFRESOURCES;

    /* Assume that all went well */
    if (sentOnDummyHost)
        *sentOnDummyHost = RV_FALSE;

    if ((((H245Control*)ctrl)->bIsControl) ||
         /* no control - only allow end session command to go through */
        (RV_PVT_NODEID_IS_VALID(pvtGetChild2(hVal, message,__h245(command), __h245(endSessionCommand)))))
    {
        status = pH245->evHandlers.sendMessageForH245(hCall, message, sentOnDummyHost);
    }

    if (bDelete == RV_TRUE)
    {
        pvtDelete(hVal, message);
    }

    return status;
}


/************************************************************************
 * sendMessageH245Chan
 * purpose: Send H245 message for a given channel.
 * input  : ctrl    - Stack handle for the H245 call control
 *          hsChan  - Channel handle for the H245 channel
 *          message - root node ID of the message to send
 *          bDelete - Defines whether to delete the message nodeId
 *                    inside the function or not.
 * output : none
 * return : RV_OK on success
 *          Other on failure
 ************************************************************************/
RvStatus sendMessageH245Chan(
    IN  HCONTROL     ctrl,
    IN  HCHAN        hsChan,
    IN  RvPvtNodeId  message,
    IN  RvBool       bDelete,
    OUT RvBool*      sentOnDummyHost)
{
    HCALL             hCall=(HCALL)cmiGetByControl(ctrl);
    HAPP              hApp=(HAPP)emaGetInstance((EMAElement)hCall);
    H245Object        *pH245 = (H245Object*)cmiGetH245Handle(hApp);
    HPVT              hVal = pH245->hVal;
    RvStatus          status = RV_OK;

    if (((H245Control*)ctrl)->bEndSessionSent)
        return RV_ERROR_DESTRUCTED;

    outgoingChannelMessage(pH245, (H245Channel*)hsChan, message);

    if (pH245->evHandlers.sendMessageForH245Chan != NULL)
    {
        status = pH245->evHandlers.sendMessageForH245Chan(hCall, hsChan, message, sentOnDummyHost);
    }
    if (bDelete == RV_TRUE)
    {
        pvtDelete(hVal, message);
    }

    return status;
}
#if (RV_3G324M_USE_MONA == RV_YES)
/************************************************************************
 * cmiOpenMonaMPCChannels
 * purpose: Opens a H245 channel object for a MPC channel.
 *          This is done to enable sending and receiving of H245 channel 
 *          massages even when only MPC channels are active.
 * input  : ctrl    - Stack handle for the H245 call control
 *          mpcTypes - bitmask of MPC channels to open.
 *          isOutGoing  -   RV_TRUE if an outgoing channel.
 * output : none
 * return : RV_OK on success
 *          Other on failure
 ************************************************************************/
RvStatus cmiOpenMonaMPCChannels(
         IN HCONTROL    ctrl, 
         IN RvUint16    mpcTypes, 
         IN RvBool      isOutGoing)
{
    HCALL             hsCall=(HCALL)cmiGetByControl(ctrl);
    HAPP              hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    H245Object        *app;
    RvUint16          mpcChannelType =  RV_H245_MPC_AMR_SUPPORTED;
    RvUint16          chanLcn = (RvUint16)1;

    if (((H245Control*)ctrl)->bEndSessionSent)
        return RV_ERROR_DESTRUCTED;
    if ((hsCall == NULL) || (hApp == NULL))
        return RV_ERROR_NULLPTR;

    app = (H245Object*)cmiGetH245Handle(hApp);

    /*If we can't notify the application about the MPC channel - no point creating it*/
    if (app->cmMySessionEvent.cmEvCallNewChannel == NULL)
        return RV_ERROR_ILLEGAL_ACTION;

    if (emaLock((EMAElement)hsCall))
    {
        while(chanLcn < RvH245MonaMPCChannelTypeLAST)
        {
            H245Channel       *channel = NULL;
            
            if (mpcTypes & mpcChannelType)
            {
                /*check if we need to open an H245Channel object*/
                if (isOutGoing)
                    channel = getOutChanByLCN(ctrl, chanLcn);
                else
                    channel = getInChanByLCN(ctrl, chanLcn);
                if (channel == NULL)
                {
                    channel=allocateChannel(ctrl);
                    if (!channel)
                    {
                        emaUnlock((EMAElement)hsCall);
                        return RV_ERROR_OUTOFRESOURCES;
                    }
                    channel->bOrigin = isOutGoing;
                    channel->hCtrl=ctrl;
                    channel->myLogicalChannelNum = chanLcn;
                    channel->reverseLogicalChannelNum = chanLcn;
                    --((H245Control*)ctrl)->logicalChannelOut;
                    channel->data.h223Params.bIsMPCChannel = RV_TRUE;
                    channel->eState = ChannelStateEstablished;
                    /*notify the application*/
                    H245CBEnter((app->pAppObject, "cmEvCallNewChannel:(for MPC phantomChannel): haCall=%p, hsCall=%p. hsChan=%p", emaGetApplicationHandle((EMAElement)cmiGetByControl(ctrl)),hsCall,(HCHAN)channel));
                    {
                        HAPPCHAN haChan=NULL;
                        int nesting = emaPrepareForCallback((EMAElement)channel);
                        app->cmMySessionEvent.cmEvCallNewChannel((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl(ctrl)), hsCall,(HCHAN)channel, &haChan);
                        emaReturnFromCallback((EMAElement)channel, nesting);
                        emaSetApplicationHandle((EMAElement)channel,(void*)haChan);
                    }
                    H245CBExit((app->pAppObject, "cmEvCallNewChannel:(for MPC phantomChannel): haChan=%p.", emaGetApplicationHandle((EMAElement)channel)));
                }
            }
            else
            {
                /*remove an existing MPC h245 channel if exist*/
                if (isOutGoing)
                    channel = getOutChanByLCN(ctrl, chanLcn);
                else
                    channel = getInChanByLCN(ctrl, chanLcn);
                if (channel != NULL)
                {
                    emaLock((EMAElement)channel);
                    if (channel->data.h223Params.bIsMPCChannel == RV_TRUE)
                    {
                        RvLogInfo(&app->log, (&app->log, "cmiOpenMonaMPCChannels: Removing MPC H245 channel, lcn = %d (outgoing = %d)", channel->myLogicalChannelNum, isOutGoing));
                        if (channel->pNext)
                            channel->pNext->pPrev = channel->pPrev;
                        if (channel->pPrev)
                            channel->pPrev->pNext = channel->pNext;
                        else
                            cmH245SetChannelList(ctrl, (HCHAN)channel->pNext);
                        emaDelete((EMAElement)channel);
                    }


                    emaUnlock((EMAElement)channel);
                }
            }
            mpcChannelType <<= 1;
            chanLcn++;
        }
        emaUnlock((EMAElement)hsCall);
    }

    return RV_OK;
}

/************************************************************************
 * cmiRemoveMonaMPCChannel
 * purpose: Remove a MPC channel object.
 * input  : ctrl            - Stack handle for the H245 call control
 *          mpcChannelType   - The MPC channel type.
 *          isOutGoing  -   RV_TRUE if an outgoing channel.
 * output : none
 * return : RV_OK on success
 *          Other on failure
 ************************************************************************/
RvStatus cmiRemoveMonaMPCChannel(
         IN HCONTROL    ctrl, 
         IN RvUint16    mpcChannelType, 
         IN RvBool      isOutGoing)
{
    HCALL           hsCall=(HCALL)cmiGetByControl(ctrl);
    HAPP            hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    H245Object      *app;
    H245Control     *ctrlE = (H245Control *)ctrl;

    if (ctrlE->bEndSessionSent)
        return RV_ERROR_DESTRUCTED;
    if ((hsCall == NULL) || (hApp == NULL))
        return RV_ERROR_NULLPTR;
    
    app = (H245Object*)cmiGetH245Handle(hApp);

    if (emaLock((EMAElement)hsCall))
    {
        H245Channel       *channel = NULL;

        if (isOutGoing == RV_TRUE)
            channel = getOutChanByLCN(ctrl, mpcChannelType);
        else
            channel = getInChanByLCN(ctrl, mpcChannelType);
        if (channel != NULL)
        {
            if (channel->data.h223Params.bIsMPCChannel == RV_FALSE)
            {
                emaUnlock((EMAElement)hsCall);
                return RV_ERROR_ILLEGAL_ACTION;
            }
            emaLock((EMAElement)channel);
            /* stop timers */
            app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &channel->pTimer);
            if (channel->pNext)
                channel->pNext->pPrev = channel->pPrev;
            if (channel->pPrev)
                channel->pPrev->pNext = channel->pNext;
            else
                cmH245SetChannelList(ctrl, (HCHAN)channel->pNext);
            emaDelete((EMAElement)channel);
            emaUnlock((EMAElement)channel);
        }
        emaUnlock((EMAElement)hsCall);
        return RV_OK;
    }

    return RV_ERROR_UNKNOWN;
}
#endif /* USE_MONA */
/************************************************************************
 * cmEmGetH245Syntax
 * purpose: Returns the syntax of h245.asn.
 *          This syntax holds the H245 standard's ASN.1 syntax
 *          The syntax is the buffer passed to pstConstruct as syntax.
 * input  : none
 * output : none
 * return : Syntax of h245.asn
 ************************************************************************/
RVAPI
RvUint8* RVCALLCONV cmEmGetH245Syntax(void)
{
    return h245GetSyntax();
}


//注意 code add 设置SecondLogicalChannel参数 20100929
RvInt32 GetSecondLogicalChannelNum(IN  HCHAN hsChan,IN  RvInt32 sessionId)
{
	H245Channel * channel=NULL;
	RvInt32	chanNumReqSend=-1;
	channel=(H245Channel *)hsChan;

	if(channel) {
		chanNumReqSend=channel->myLogicalChannelNum;

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
			channel->data.h225Params.sessionId=sessionId;
			channel->data.h225Params.pPartner=channel;
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */

	}

	return chanNumReqSend;

}

#ifdef __cplusplus
}
#endif
