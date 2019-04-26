/***********************************************************************
        Copyright (c) 2005 RADVISION Ltd.
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
#include "strutils.h"
#include "cmchan.h"
#include "h245.h"
#include "cmChanGet.h"
#include "cmCtrlCap.h"
#include "cmH245Object.h"
#include "cmH245.h"

#ifdef __cplusplus
extern "C" {
#endif


int  cmcReadyEvent  (H245Control* ctrl);

/*Out Capibility*/
static RvBool capTimeoutEventsHandler(void*hsCall);

void capInit(H245Control* ctrl)
{
    H245Object* app = (H245Object *)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    OutgoingCapInfo* outcap=&ctrl->outCap;
    IncomingCapInfo* incap=&ctrl->inCap;

    outcap->seqNum = 0;
    outcap->waiting=RV_FALSE;
    outcap->termNodeId=RV_ERROR_UNKNOWN;
    incap->termNodeId=RV_ERROR_UNKNOWN;
    incap->manualResponse=(pvtGetChild2(app->hVal,app->h245Conf,__h245(capabilities),__h245(manualResponse))>=0);
    RvLogInfo(&app->log, (&app->log, "TCS manual response on %p = %d",ctrl,incap->manualResponse));

    outcap->timer = NULL;
    outcap->rejectCause = RV_ERROR_UNKNOWN;
}


static RvStatus inCapTransferRequest(H245Control* ctrl)
{
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT hVal;
    IncomingCapInfo* incap=&ctrl->inCap;
    RvStatus status;
    int nodeId;
    int iSq = incap->seqNum;

    hVal = ((H245Object *)hApp)->hVal;

    nodeId = pvtAddRoot(hVal,((H245Object*)hApp)->hSynProtH245,0,NULL);
    __pvtBuildByFieldIds(status,hVal,nodeId,{_h245(response) _h245(terminalCapabilitySetAck) _h245(sequenceNumber) LAST_TOKEN},iSq,NULL);

    status = sendMessageH245((HCONTROL)ctrl, nodeId, RV_TRUE, NULL);

    if (status == RV_OK)
    {
        ctrl->eInCapStatus = CapStatusAcknowledged;
        cmcReadyEvent(ctrl);
    }

    return status;
}



/************************************************************************
 * capEnd
 * purpose: Reset the capabilities timer
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
void capEnd(IN H245Control* ctrl)
{
    OutgoingCapInfo* outcap=&ctrl->outCap;
    H245Object* app=(H245Object *)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));

    capFree(ctrl);

    /* Reset the outgoing capabilities timer is one exists */
    app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &outcap->timer);
}

/************************************************************************
 * capFree
 * purpose: Free the capabilities node ids
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
void capFree(IN H245Control* ctrl)
{
    H245Object* app=(H245Object *)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));

    /* Delete PVT tree nodes of capabilities */
    pvtDelete(app->hVal,  ctrl->outCap.termNodeId);
    ctrl->outCap.termNodeId = RV_ERROR_UNKNOWN;

    pvtDelete(app->hVal,  ctrl->inCap.termNodeId);
    ctrl->inCap.termNodeId = RV_ERROR_UNKNOWN;
}


/************************************************************************
 * outCapTransferRequest
 * purpose: Request to send out a TCS message
 *          This function also handled the timer to set and adds additional
 *          information to the outgoing TCS
 * input  : ctrl    - Control object
 *          message - TCS message node ID to send
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int outCapTransferRequest(IN H245Control* ctrl, IN int message)
{
    HH245            hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT             hVal;
    H245Object*      app;
    OutgoingCapInfo* outcap=&ctrl->outCap;
    RvPvtNodeId      msgId;
    RvInt32          timeout=9;
    int              res;
    int              iSq;
    RvBool           sentOnDummyHost;

    app = (H245Object *)hApp;
    hVal = app->hVal;

    /* Increment the sequence number for this TCS transaction */
    outcap->seqNum++;

    /* Set the sequenceNumber and protocolIdentifier in the TCS we're about to send */
    msgId = pvtGetChild2(hVal, message, __h245(request), __h245(terminalCapabilitySet));
    iSq = outcap->seqNum;
    pvtAdd(hVal, msgId, __h245(sequenceNumber), iSq, NULL, NULL);
    pvtAdd(hVal, msgId, __h245(protocolIdentifier), app->h245protocolIDLen, app->h245protocolID, NULL);

    /* Update the outgoing capabilities of this terminal */
    if (outcap->termNodeId < 0)
        outcap->termNodeId = pvtAddRoot(hVal, NULL, 0, NULL);
    pvtSetTree(hVal, outcap->termNodeId, hVal, msgId);

    /* Send the TCS message */
    res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, &sentOnDummyHost);
    if (res >= 0)
    {
        if (!sentOnDummyHost)
        {
            /* Set the timer for this transaction from the configuration */
            pvtGetChildValue2(hVal, ((H245Object*)hApp)->h245Conf, __h245(capabilities) ,__h245(timeout), &timeout, NULL);
            app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &outcap->timer);
            outcap->timer = app->evHandlers.pfnTimerStartEv((HCONTROL)ctrl,
                capTimeoutEventsHandler, (void*)ctrl, timeout*1000);
        }

        /* Update control state because of this TCS */
        outcap->waiting = RV_TRUE;
        ctrl->eOutCapStatus = CapStatusSent;
    }
    else
    {
        pvtDelete(hVal, outcap->termNodeId);
        outcap->termNodeId = RV_PVT_INVALID_NODEID;
    }

    return res;
}


/************************************************************************
 * terminalCapabilitySetAck
 * purpose: Handle an incoming TerminalCapabilitySetAck message
 * input  : ctrl    - Control object
 *          message - TCS.Ack message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int terminalCapabilitySetAck(IN H245Control* ctrl, IN int message)
{
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    H245Object* app=(H245Object*)hApp;
    OutgoingCapInfo* outcap=&ctrl->outCap;

    if (outcap->waiting)
    {
        HPVT hVal = app->hVal;
        int sq;
        pvtGetChildByFieldId(hVal, message, __h245(sequenceNumber), &sq, NULL);
        if (sq == outcap->seqNum)
        {
            app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &outcap->timer);
            ctrl->eOutCapStatus = CapStatusAcknowledged;
            outcap->waiting = RV_FALSE;

            emaMark((EMAElement)cmiGetByControl((HCONTROL)ctrl));

            if (app->cmMySessionEvent.cmEvCallCapabilitiesResponse != NULL)
            {
                /* Application's callback for TCS.Ack */
                int nesting;

                H245CBEnter((app->pAppObject, "cmEvCallCapabilitiesResponse(haCall=%p, hsCall=%p, cmCapAccept)", (HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)), cmiGetByControl((HCONTROL)ctrl)));
                nesting = emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
                app->cmMySessionEvent.cmEvCallCapabilitiesResponse((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)), (HCALL)cmiGetByControl((HCONTROL)ctrl), (RvUint32)cmCapAccept);
                emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
                H245CBExit((app->pAppObject, "cmEvCallCapabilitiesResponse"));
            }

            if (!emaWasDeleted((EMAElement)cmiGetByControl((HCONTROL)ctrl)))
            {
                /*set bSaveCallIndication if control in ctrlMesFinished state*/
                if (ctrl->eState == ctrlMesFinished)
                {
                    cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
                }
                cmcReadyEvent(ctrl);
            }
            emaRelease((EMAElement)cmiGetByControl((HCONTROL)ctrl));
        }
    }
    return RV_TRUE;
}


/************************************************************************
 * terminalCapabilitySetReject
 * purpose: Handle an incoming TerminalCapabilitySetReject message
 * input  : ctrl    - Control object
 *          message - TCS.Reject message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int terminalCapabilitySetReject(IN H245Control* ctrl, IN int message)
{
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    OutgoingCapInfo* outcap=&ctrl->outCap;
    H245Object* app=(H245Object*)hApp;

    if (outcap->waiting)
    {
        HPVT hVal = app->hVal;
        int sq;
        pvtGetChildByFieldId(hVal, message, __h245(sequenceNumber), &sq, NULL);
        if (sq == outcap->seqNum)
        {
            int tmpNodeId;
            int nesting;

            app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &outcap->timer);
            ctrl->eOutCapStatus = CapStatusRejected;

            /* Find out the cause */
            __pvtGetByFieldIds(tmpNodeId, hVal, message, {_h245(cause) _anyField LAST_TOKEN}, NULL, NULL, NULL);
            ctrl->outCap.rejectCause = pvtGetSyntaxIndex(hVal, tmpNodeId);

            if (app->cmMySessionEvent.cmEvCallCapabilitiesResponse != NULL)
            {
                H245CBEnter((app->pAppObject, "cmEvCallCapabilitiesResponse: haCall=%p, hsCall=%p, cmCapReject.",(HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),cmiGetByControl((HCONTROL)ctrl)));
                emaMark((EMAElement)cmiGetByControl((HCONTROL)ctrl));
                nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
                app->cmMySessionEvent.cmEvCallCapabilitiesResponse((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)), (HCALL)cmiGetByControl((HCONTROL)ctrl), (RvUint32)cmCapReject);
                emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
                H245CBExit((app->pAppObject, "cmEvCallCapabilitiesResponse."));
                outcap->waiting = RV_FALSE;

                emaRelease((EMAElement)cmiGetByControl((HCONTROL)ctrl));
            }
            else
                outcap->waiting = RV_FALSE;

            /*set bSaveCallIndication if control in ctrlMesFinished state*/
            if (ctrl->eState == ctrlMesFinished)
            {
                cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
            }
        }
    }
    return RV_TRUE;
}

static RvBool capTimeoutEventsHandler(void*_ctrl)
{
    H245Control* ctrl=(H245Control*)_ctrl;
    HCALL hsCall = cmiGetByControl((HCONTROL)ctrl);

    if (emaLock((EMAElement)hsCall))
    {
        OutgoingCapInfo* outcap=&ctrl->outCap;
        H245Object* app;
        HPVT hVal;
        int nesting;
        int nodeId;

        app = (H245Object *)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        hVal = app->hVal;

        app->evHandlers.pfnTimerClearEv((HCONTROL)ctrl, &outcap->timer);
        ctrl->eOutCapStatus = CapStatusRejected;
        ctrl->outCap.rejectCause = RV_ERROR_UNKNOWN; /* No actual cause on timeout */

        nodeId=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        pvtAddBranch2(hVal,nodeId,__h245(indication),__h245(terminalCapabilitySetRelease));
        sendMessageH245((HCONTROL)ctrl, nodeId, RV_TRUE, NULL);

        outcap->waiting = RV_FALSE;

        if (app->cmMySessionEvent.cmEvCallCapabilitiesResponse != NULL)
        {
            H245CBEnter((app->pAppObject, "cmEvCallCapabilitiesResponse: haCall=%p, hsCall=%p, cmCapReject.",(HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall),hsCall));
            nesting=emaPrepareForCallback((EMAElement)hsCall);
            app->cmMySessionEvent.cmEvCallCapabilitiesResponse(
                (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall), (HCALL)hsCall, (RvUint32)cmCapReject);
            emaReturnFromCallback((EMAElement)hsCall,nesting);
            H245CBExit((app->pAppObject, "cmEvCallCapabilitiesResponse."));
        }

        /*set bSaveCallIndication if control in ctrlMesFinished state*/
        if (ctrl->eState == ctrlMesFinished)
        {
            cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
        }

        emaUnlock((EMAElement)hsCall);
    }
    return RV_FALSE;
}


/*In Capability*/



/************************************************************************
 * terminalCapabilitySet
 * purpose: Handle an incoming TerminalCapabilitySet message
 * input  : ctrl    - Control object
 *          message - TCS.Request message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int terminalCapabilitySet(IN H245Control* ctrl, IN int message)
{
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT hVal;
    IncomingCapInfo* incap=&ctrl->inCap;
    H245Object* app=(H245Object*)hApp;
    RvPvtNodeId nodeId;
    RvInt32 sq=0;

    hVal =app->hVal;

    /* Get the parameters from the incoming cap */
    pvtGetChildByFieldId(hVal, message, __h245(sequenceNumber), &sq, NULL);
    incap->seqNum = (RvUint8)sq;
    nodeId = pvtGetChildByFieldId(hVal, message, __h245(protocolIdentifier), &incap->pIDLen, NULL);
    if (RV_PVT_NODEID_IS_VALID(nodeId) && (incap->pIDLen > 0))
        pvtGetString(hVal, nodeId, incap->pIDLen, incap->pID);
    else
        incap->pIDLen = -1;

    emaMark((EMAElement)cmiGetByControl((HCONTROL)ctrl));

    /* set this as the last capabilities received */
    if (!RV_PVT_NODEID_IS_VALID(incap->termNodeId))
        incap->termNodeId = pvtAddRoot(hVal, NULL, 0, NULL);
    pvtShiftTree(hVal, incap->termNodeId, message);

    ctrl->eInCapStatus = CapStatusSent;

    /*if this is TCS0 (it is TCS0 if the node have only 2 children - sequenceNumber and
    protocolIdentifier), we should save the state,change it and close channels*/
    if ( pvtNumChilds(hVal, incap->termNodeId) == 2 )
    {
        /* should we change ctrl->eInCapStatus ? */
        if ((ctrl->eState!=ctrlTransmittedSidePaused))
        {
            H245Channel* channel;
            void* ptr=NULL;

            ctrl->eLastState = ctrl->eState;
            ctrl->eState = ctrlTransmittedSidePaused;

            /* close every channel that was opened by the endPoint */
            channel = getNextChan((HCONTROL)ctrl, &ptr);
            while (channel != NULL)
            {
                if (channel->bOrigin)
                    cmChannelDropReason((HCHAN)channel,cmCloseReasonUnknown);
                channel = getNextChan((HCONTROL)ctrl, &ptr);
            }

            {
                int nesting;
                if (app->cmMySessionEvent.cmEvCallCapabilities != NULL)
                {
                    H245CBEnter((app->pAppObject, "cmEvCallCapabilities: haCall=%p, hsCall=%p, capSet=NULL.",(HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)), cmiGetByControl((HCONTROL)ctrl)));
                    nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
                    app->cmMySessionEvent.cmEvCallCapabilities((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),(HCALL)cmiGetByControl((HCONTROL)ctrl),  NULL);
                    emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
                    H245CBExit((app->pAppObject, "cmEvCallCapabilities."));
                }

                if (!emaWasDeleted((EMAElement)cmiGetByControl((HCONTROL)ctrl)) &&
                    app->cmMySessionEvent.cmEvCallCapabilitiesExt)
                {
                    H245CBEnter((app->pAppObject, "cmEvCallCapabilitiesExt: haCall=%p, hsCall=%p, capDesc=NULL.",(HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
                        cmiGetByControl((HCONTROL)ctrl)));
                    nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
                    app->cmMySessionEvent.cmEvCallCapabilitiesExt((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),(HCALL)cmiGetByControl((HCONTROL)ctrl),(cmCapStruct****)NULL);
                    emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
                    H245CBExit((app->pAppObject, "cmEvCallCapabilitiesExt."));
                }

                /*set bSaveCallIndication if control in ctrlMesFinished state*/
                if (ctrl->eState == ctrlMesFinished)
                {
                    cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
                }
            }

        }
    }
    else
    {
        if (ctrl->eState == ctrlTransmittedSidePaused)
        {
            ctrl->eState = ctrl->eLastState;
            if ((ctrl->eState == ctrlConnected) || (ctrl->eState == ctrlConference) || (ctrl->eState == ctrlMesFinished))
            {
                /* The check for the Call State Connected is done inside this function. If the state
                   is connected than the function does nothing */
                if (app->evHandlers.pfnNotifyStateEv != NULL)
                    app->evHandlers.pfnNotifyStateEv((HCALL)cmiGetByControl((HCONTROL)ctrl), ((H245Control*)ctrl)->eState);
            }
        }

        /* Handle callbacks of capability exchange message if we have to */
        if (app->cmMySessionEvent.cmEvCallCapabilities || app->cmMySessionEvent.cmEvCallCapabilitiesExt)
        {
            int         i;
            cmCapStruct capSet[RV_H245_CAP_SET_SIZE];
            cmCapStruct *pcapSet[RV_H245_CAP_SET_SIZE];
            void        *capDesc[RV_H245_CAP_DESC_SIZE];
            int         nesting;

            for (i=0; i<RV_H245_CAP_SET_SIZE; i++) pcapSet[i] = capSet+i;
            capSetBuild(hVal, incap->termNodeId, RV_H245_CAP_SET_SIZE, pcapSet);

            if (app->cmMySessionEvent.cmEvCallCapabilities != NULL)
            {
                H245CBEnter((app->pAppObject, "cmEvCallCapabilities: haCall=%p, hsCall=%p, capSet=%p.",(HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)), cmiGetByControl((HCONTROL)ctrl), pcapSet));
                nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
                app->cmMySessionEvent.cmEvCallCapabilities((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),(HCALL)cmiGetByControl((HCONTROL)ctrl),  pcapSet);
                emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
                H245CBExit((app->pAppObject, "cmEvCallCapabilities."));
            }

            if (!emaWasDeleted((EMAElement)cmiGetByControl((HCONTROL)ctrl)) &&
                app->cmMySessionEvent.cmEvCallCapabilitiesExt)
            {
                memset(capDesc, 0, sizeof(void*) * RV_H245_CAP_DESC_SIZE);
                capDescBuild(hVal, incap->termNodeId, pcapSet, RV_H245_CAP_SET_SIZE, RV_H245_CAP_DESC_SIZE, capDesc);
                H245CBEnter((app->pAppObject, "cmEvCallCapabilitiesExt: haCall=%p, hsCall=%p, capDesc=%p.",(HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
                    cmiGetByControl((HCONTROL)ctrl), capDesc));
                nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
                app->cmMySessionEvent.cmEvCallCapabilitiesExt((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),(HCALL)cmiGetByControl((HCONTROL)ctrl),(cmCapStruct****)capDesc);
                emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
                H245CBExit((app->pAppObject, "cmEvCallCapabilitiesExt."));
            }

            /*set bSaveCallIndication if control in ctrlMesFinished state*/
            if (ctrl->eState == ctrlMesFinished)
            {
                cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
            }
        }
    }

    if ( (!emaWasDeleted((EMAElement)cmiGetByControl((HCONTROL)ctrl))) &&
         (!incap->manualResponse) )
    {
        RvLogInfo(&app->log, (&app->log, "Invoking automatic TCS response %d",incap->manualResponse));

        inCapTransferRequest(ctrl);
    }

    emaRelease((EMAElement)cmiGetByControl((HCONTROL)ctrl));
    return RV_TRUE;
}


/******************************************************************************
 * cmCallCapabilitiesAck
 * ----------------------------------------------------------------------------
 * General: Replies manually with an acknowledgment to a
 *          terminalCapabilitySet request.
 *
 * Return Value: Non-negative value on success, negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall           -  The Stack handle for the call.
 *****************************************************************************/
RVAPI int RVCALLCONV
cmCallCapabilitiesAck(
               IN HCALL hsCall)
{
    H245Control* ctrl = NULL;
    IncomingCapInfo* incap = NULL;
    int  status;
    H245Object *app;

    /* check validity of data */
    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

        H245APIEnter((app->pAppObject, "cmCallCapabilitiesAck(hsCall=%p)", hsCall));

        ctrl = (H245Control*)cmiGetControl(hsCall);
        if ((ctrl != NULL) && (ctrl->eInCapStatus == CapStatusSent))
        {
            incap = &ctrl->inCap;

            /* check if we are in manual response, otherwise this API is forbidden */
            if (incap->manualResponse)
            {
                /* send the TCS Ack */
                status = inCapTransferRequest(ctrl);
            }
            else
                status = RV_ERROR_NOTSUPPORTED;
        }
        else
            status = RV_ERROR_BADPARAM;

        emaUnlock((EMAElement)hsCall);

        if (status == RV_OK)
            status = 1; /* Done for backward compatibility */
        H245APIExit((app->pAppObject, "cmCallCapabilitiesAck=%d", status));
    }
    else
        status = RV_ERROR_UNINITIALIZED;

    return status;
}


/******************************************************************************
 * cmCallCapabilitiesReject
 * ----------------------------------------------------------------------------
 * General: Replies manually with a rejection to a
 *          terminalCapabilitySet request.
 *
 * Return Value: RV_OK on success, negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall           - The Stack handle for the call.
 *         cause            - Rejection cause for this message.
 *         highestEntry     - Highest number of entry that was processed from
 *                            the incoming terminalCapabilitySet message.
 *                            This parameter is only valid if cause is set to
 *                            cmCapRejectCauseTableEntryExceeded.
 *                            A 0 value indicates that none of the entries
 *                            were processed.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmCallCapabilitiesReject(
               IN HCALL             hsCall,
               IN cmCapRejectCause  cause,
               IN RvInt             highestEntry)
{
    H245Control* ctrl = NULL;
    IncomingCapInfo* incap = NULL;
    RvStatus status = RV_OK;
    H245Object *app;

    /* check validity of data */
    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

        H245APIEnter((app->pAppObject, "cmCallCapabilitiesReject(hsCall=%p)", hsCall));

        ctrl = (H245Control*)cmiGetControl(hsCall);
        if (ctrl != NULL)
        {
            incap = &ctrl->inCap;

            /* check if we are in manual response, otherwise this API is forbidden */
            if (incap->manualResponse)
            {
                /* send the TCS Reject */
                RvPvtNodeId rootNodeId, tcsNodeId;
                HPVT hVal = app->hVal;

                rootNodeId = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);
                __pvtBuildByFieldIds(tcsNodeId, hVal, rootNodeId, {_h245(response) _h245(terminalCapabilitySetReject) LAST_TOKEN}, 0, NULL);
                pvtAdd(hVal, tcsNodeId, __h245(sequenceNumber), incap->seqNum, NULL, NULL);
                tcsNodeId = pvtAdd(hVal, tcsNodeId, __h245(cause), 0, NULL, NULL);
                switch (cause)
                {
                    case cmCapRejectCauseUnspecified:
                        pvtAdd(hVal, tcsNodeId, __h245(unspecified), 0, NULL, NULL);
                        break;
                    case cmCapRejectCauseUndefinedTableEntry:
                        pvtAdd(hVal, tcsNodeId, __h245(undefinedTableEntryUsed), 0, NULL, NULL);
                        break;
                    case cmCapRejectCauseDescriptorExceeded:
                        pvtAdd(hVal, tcsNodeId, __h245(descriptorCapacityExceeded), 0, NULL, NULL);
                        break;
                    case cmCapRejectCauseTableEntryExceeded:
                        tcsNodeId = pvtAdd(hVal, tcsNodeId, __h245(tableEntryCapacityExceeded), 0, NULL, NULL);
                        if (highestEntry == 0)
                            pvtAdd(hVal, tcsNodeId, __h245(noneProcessed), 0, NULL, NULL);
                        else
                            pvtAdd(hVal, tcsNodeId, __h245(highestEntryNumberProcessed), highestEntry, NULL, NULL);
                        break;
                }
                status = sendMessageH245((HCONTROL)ctrl, rootNodeId, RV_TRUE, NULL);

                if (status == RV_OK)
                    ctrl->eInCapStatus = CapStatusRejected;
            }
            else
                status = RV_ERROR_NOTSUPPORTED;
        }
        else
            status = RV_ERROR_BADPARAM;

        emaUnlock((EMAElement)hsCall);

        H245APIExit((app->pAppObject, "cmCallCapabilitiesReject=%d", status));
    }
    else
        status = RV_ERROR_UNINITIALIZED;

    return status;
}



/************************************************************************
 * terminalCapabilitySetRelease
 * purpose: Handle an incoming TerminalCapabilitySetRelease message
 * input  : ctrl    - Control object
 *          message - TCS.Release message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int terminalCapabilitySetRelease(IN H245Control* ctrl, IN int message)
{
    RV_UNUSED_ARG(message);

    ctrl->eInCapStatus = CapStatusReleased;
    return RV_TRUE;
}


RVAPI int RVCALLCONV /* RV_TRUE or RV_ERROR_UNKNOWN */
cmCallSendCapability(
             /* Send terminal capability set tree to remote terminal */
             /* Note: cap tree is not deleted! */
             IN  HCALL hsCall,
             IN  int termCapSet  /* local terminal capability set Id */
             )
{
    int message;
    RvStatus status = RV_ERROR_UNKNOWN;

    if (emaLock((EMAElement)hsCall))
    {
        HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);
        H245Object *app = (H245Object*)hApp;

        RV_UNUSED_ARG(app) /* used only when the log is on */
        H245APIEnter((app->pAppObject, "cmCallSendCapability(hsCall=%p, capSet=%d)", hsCall, termCapSet));

        if (ctrl->bIsControl)
        {
            HPVT hVal = ((H245Object *)hApp)->hVal;

            message = pvtAddRoot(hVal, ((H245Object*)hApp)->hSynProtH245, 0, NULL);
            status = pvtSetTree(hVal, pvtAddBranch2(hVal, message, __h245(request), __h245(terminalCapabilitySet)), hVal, termCapSet);
            if (status >= 0)
                status = outCapTransferRequest(ctrl, message);
            else
                pvtDelete(hVal, message);
        }

        H245APIExit((app->pAppObject, "cmCallSendCapability=%d", status));
        emaUnlock((EMAElement)hsCall);
    }
    return status;
}


RVAPI int RVCALLCONV /* new terminalCapabilitySet node id. */
cmCallCapabilitiesBuild(
            /* Build terminalCapabilitySet value tree including the capability set
               and capability descriptors. */
            IN  HCALL               hsCall,
            IN  cmCapStruct*        capSet[],
            IN  cmCapStruct***      capDesc[]
            )
{
    RvPvtNodeId rootId = RV_PVT_INVALID_NODEID;

    if ((hsCall == NULL) || (capSet == NULL) || (capDesc == NULL))
        return RV_ERROR_UNKNOWN;


    if (emaLock((EMAElement)hsCall))
    {
        HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        H245Object *app = (H245Object*)hApp;
        HPVT hVal = app->hVal;

        H245APIEnter((app->pAppObject, "cmCallCapabilitiesBuild: hsCall=%p, capSet=%p, capDesc=%p.", hsCall, capSet, capDesc));

        rootId = pvtAddRootByPath(hVal, ((H245Object*)hApp)->hSynProtH245, (char*)"request.terminalCapabilitySet", 0, NULL);
        if (rootId >= 0)
        {
            capSetBuildFromStruct(hVal, ((H245Object*)hApp)->h245Conf, rootId, capSet);
            capDescBuildFromStruct(hVal, rootId, capDesc);
        }

        H245APIExit((app->pAppObject, "cmCallCapabilitiesBuild: [%d].", rootId));
        emaUnlock((EMAElement)hsCall);
    }

    return rootId;
}


RVAPI int RVCALLCONV
cmCallCapabilitiesSend(
               IN   HCALL           hsCall,
               IN   cmCapStruct*    capSet[],
               IN   cmCapStruct***  capDesc[])
{
    int status = RV_ERROR_UNKNOWN;

    if ((hsCall == NULL) || (capSet == NULL) || (capDesc == NULL))
        return RV_ERROR_UNKNOWN;

    if (emaLock((EMAElement)hsCall))
    {
        HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        H245Object *app = (H245Object*)hApp;
        RvPvtNodeId rootId;

        H245APIEnter((app->pAppObject, "cmCallCapabilitiesSend: hsCall=%p, capSet=%p, capDesc=%p.", hsCall, capSet, capDesc));

        rootId = cmCallCapabilitiesBuild(hsCall, capSet, capDesc);
        if (RV_PVT_NODEID_IS_VALID(rootId))
        {
            cmCallSendCapability(hsCall, rootId);
            pvtDelete(app->hVal, rootId);
            status = 1;
        }

        H245APIExit((app->pAppObject, "cmCallCapabilitiesSend=%d", status));
        emaUnlock((EMAElement)hsCall);
    }

    return status;
}



RVAPI int RVCALLCONV /* remote terminal capability set node id, or RV_ERROR_UNKNOWN */
cmCallGetRemoteCapabilities(
                /* Get latest remote terminal capability message */
                IN  HCALL       hsCall
                )
{
    RvPvtNodeId termCapId = RV_PVT_INVALID_NODEID;

    if (hsCall == NULL)
        return RV_ERROR_UNKNOWN;

    if (emaLock((EMAElement)hsCall))
    {
        HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);
        H245Object *app = (H245Object*)hApp;

        RV_UNUSED_ARG(app) /* used only when the log is on */
        H245APIEnter((app->pAppObject, "cmCallGetRemoteCapabilities: hsCall=%p.", hsCall));

        if (ctrl->eState != ctrlNotInitialized)
            termCapId = ctrl->inCap.termNodeId;

        H245APIExit((app->pAppObject, "cmCallGetRemoteCapabilities: [%d].", termCapId));
        emaUnlock((EMAElement)hsCall);
    }

    return termCapId;
}



/******************************************************************************
 * cmCallGetLocalCapabilities
 * ----------------------------------------------------------------------------
 * General: Gets the local capabilities which are part of the outgoing
 *          TerminalCapabilitySet message.
 *          Note that if the TCS message has already been sent, then the
 *          capabilities of the message will be processed, otherwise, the
 *          capabilities from configuration will be processed.
 *          An example of usage:
 *
 *          cmCapStruct     capA[capSetASize];
 *          cmCapStruct     *capSetA[capSetASize];
 *          void            *capDescA[capDescASize];
 *          cmCapStruct     **capSet;
 *          cmCapStruct     ****capDesc;
 *          int             i;
 *
 *          for (i=0; i<capSetASize; i++) capSetA[i] = capA+i;
 *          cmCallGetLocalCapabilities(hsCall,
 *          capSetA, capSetASize,
 *          capDescA, capDescASize,
 *          &capSet, &capDesc);
 *
 * Return Value: If successful - The node id of the outgoing
 *               TCS message/capabilities from configuration.
 *               Negative number on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall       - The handle of the call.
 *         capSetA      - An array that will be used to build the capability set
 *                        taken from the capabilityTable field in TCS message.
 *                        This array is allocated by the user.
 *         capSetASize  - Number of elements allocated in capSetA array.
 *         capDescA     - An array that will be used to build the capability
 *                        descriptors taken from the capabilityDescriptors field
 *                        in TCS message.
 *                        This array is allocated by the user.
 *         capDescASize - Number of elements allocated in capDescA array.
 * Output: capSet       - A pointer to the capability set array which was given
 *                        as capSetA parameter.
 *         capDesc      - A pointer to the capability descriptors array which
 *                        was given as capDescA parameter. The array is sorted
 *                        in ascending order according to
 *                        capabilityDescriptorNumbers.
 *                        This array is a 4 dimensional array that organizes
 *                        the capability descriptors in 4 hierarchical levels:
 *                        CapabilityDescriptor -> simultaneousCapabilities ->
 *                        AlternativeCapabilitySet -> CapabilityTableEntry.
 *****************************************************************************/
RVAPI int RVCALLCONV
cmCallGetLocalCapabilities(
               IN      HCALL                hsCall,
               IN      cmCapStruct*         capSetA[],
               IN      int                  capSetASize,
               IN      void*                capDescA[],
               IN      int                  capDescASize,
               OUT     cmCapStruct**        capSet[],
               OUT     cmCapStruct****      capDesc[])
{
    RvPvtNodeId capId = RV_PVT_INVALID_NODEID;
    int         ret = RV_TRUE;
    HPVT        hVal;
	RvInt32     capDescStatus = RV_OK;

    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        HH245       hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        H245Control *ctrl=(H245Control*)cmiGetControl(hsCall);
        H245Object  *app = (H245Object*)hApp;

        H245APIEnter((app->pAppObject, "cmCallGetLocalCapabilities: hsCall=%p, capSetA=%p, capSetASize=%d, capDescA=%p, capDescASize=%d, capSet=%p, capDesc=%p.",
            hsCall, capSetA, capSetASize, capDescA, capDescASize, capSet, capDesc));

        hVal = app->hVal;

        /* Working on existing TCS message or on input from configuration. */
        if ((ctrl->eState != ctrlNotInitialized) && (RV_PVT_NODEID_IS_VALID(ctrl->outCap.termNodeId)))
        {
            capId = ctrl->outCap.termNodeId;
        }
        else
        {
            capId = pvtGetChild2(hVal,app->h245Conf,__h245(capabilities),__h245(terminalCapabilitySet));
        }

        /* Some validity checks */
        if ((capId < 0) || (capSetASize < 1) || (capDescASize < 1))
            ret = RV_ERROR_UNKNOWN;

        if (ret >= 0)
        {
            if (capSetA == NULL)
                ret = RV_TRUE;
            else
            {
                /* We should probably start building the capabilities */
                /* Build the capability set from the capabilityTable of the TCS
                   message. It is done on the allocated array (capSetA parameter)
                   of the user and then returned as a pointer in capSet parameter. */
                capSetBuild(hVal, capId, capSetASize, capSetA);
                if (capSet)
                    *capSet = capSetA;

                /* Build the capability descriptors array from the
                   capabilityDescriptors of the TCS message. It is done on the
                   allocated array (capDescA parameter) of the user and then
                   returned as a pointer in capDesc parameter. */
                if (capDescA != NULL)
                {
                    memset(capDescA, 0, sizeof(void*) * capDescASize);
					capDescStatus = capDescBuild(hVal, capId, capSetA, capSetASize, capDescASize, capDescA);
                    if (capDescStatus < 0 && capDescStatus != RV_ERROR_NOT_FOUND)
                    {
						RvLogError(&app->log, (&app->log,
							"cmcGetLocalCaps:capDescBuild failed."))
                    }
                    if (capDesc)
                        *capDesc = (cmCapStruct****)capDescA;
                }
            }
        }
        H245APIExit((app->pAppObject, "cmCallGetLocalCapabilities: [%d].", ret));
        emaUnlock((EMAElement)hsCall);
    }
    if (ret < 0)
    {
        return ret;
    }

    return capId;
}

/******************************************************************************
 * cmCallGetRemoteCapabilitiesEx
 * ----------------------------------------------------------------------------
 * General: Gets the remote capabilities which are part of the incoming
 *          TerminalCapabilitySet message.
 *          Note that this function should be called after at least one TCS
 *          message has been received, otherwise, it fails.
 *          An example of usage:
 *
 *          cmCapStruct     capA[capSetASize];
 *          cmCapStruct     *capSetA[capSetASize];
 *          void            * capDescA[capDescASize];
 *          cmCapStruct     **capSet;
 *          cmCapStruct     ****capDesc;
 *          int             i;
 *
 *          for (i=0; i<capSetASize; i++) capSetA[i] = capA+i;
 *          cmCallGetRemoteCapabilitiesEx(hsCall,
 *          capSetA, capSetASize,
 *          capDescA, capDescASize,
 *          &capSet, &capDesc);
 *
 * Return Value: If successful - The node id of the incoming TCS.
 *               Negative number on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall       - The handle of the call.
 *         capSetA      - An array that will be used to build the capability set
 *                        taken from the capabilityTable field in TCS message.
 *                        This array is allocated by the user.
 *         capSetASize  - Number of elements allocated in capSetA array.
 *         capDescA     - An array that will be used to build the capability
 *                        descriptors taken from the capabilityDescriptors field
 *                        in TCS message.
 *                        This array is allocated by the user.
 *         capDescASize - Number of elements allocated in capDescA array.
 * Output: capSet       - A pointer to the capability set array which was given
 *                        as capSetA parameter. If an empty TCS message has arrived
 *                        just before calling this function, the first element
 *                        of this array will be set to NULL.
 *         capDesc      - A pointer to the capability descriptors array which
 *                        was given as capDescA parameter. The array is sorted
 *                        in ascending order according to
 *                        capabilityDescriptorNumbers.
 *                        This array is a 4 dimensional array that organizes
 *                        the capability descriptors in 4 hierarchical levels:
 *                        CapabilityDescriptor -> simultaneousCapabilities ->
 *                        AlternativeCapabilitySet -> CapabilityTableEntry.
 *                        If an empty TCS message has arrived just before calling
 *                        this function, the first element of this array will be
 *                        set to NULL.
 *****************************************************************************/
RVAPI int RVCALLCONV
cmCallGetRemoteCapabilitiesEx(
               IN      HCALL                hsCall,
               IN      cmCapStruct*         capSetA[],
               IN      int                  capSetASize,
               IN      void*                capDescA[],
               IN      int                  capDescASize,
               OUT     cmCapStruct**        capSet[],
               OUT     cmCapStruct****      capDesc[])
{
    int          capId=-1;
    HPVT         hVal;
    int          ret = RV_TRUE;
	RvInt32		 capDescStatus = RV_OK;

    if (hsCall == NULL)
        return RV_ERROR_UNKNOWN;

    if (emaLock((EMAElement)hsCall))
    {
        HH245        hApp = cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        H245Control  *ctrl = (H245Control*)cmiGetControl(hsCall);
        H245Object   *app = (H245Object*)hApp;

        H245APIEnter((app->pAppObject, "cmCallGetRemoteCapabilitiesEx: hsCall=%p, capSetA=%p, capSetASize=%d, capDescA=%p, capDescASize=%d, capSet=%p, capDesc=%p.",
        hsCall, capSetA, capSetASize, capDescA, capDescASize, capSet, capDesc));

        hVal = app->hVal;

        if ((ctrl->eState != ctrlNotInitialized) && (ctrl->inCap.termNodeId >= 0))
        {
            capId = ctrl->inCap.termNodeId;
        }
        else
        {
            RvLogError(&app->log, (&app->log,
                "cmCallGetRemoteCapabilitiesEx: no TCS message has been received yet."));
            emaUnlock((EMAElement)hsCall);
            return RV_ERROR_UNKNOWN;
        }

        /* Some validity checks */
        if ((capId < 0) || (capSetASize < 1) || (capDescASize < 1))
            ret = RV_ERROR_UNKNOWN;

        if (ret >= 0)
        {
            if (capSetA == NULL)
                ret = RV_TRUE;
            else
            {
                /* We should probably start building the capabilities */
                /* Build the capability set from the capabilityTable of the TCS
                   message. It is done on the allocated array (capSetA parameter)
                   of the user and then returned as a pointer in capSet parameter. */
                capSetBuild(hVal, capId, capSetASize, capSetA);
                if (capSet)
                    *capSet = capSetA;

                /* Build the capability descriptors array from the
                   capabilityDescriptors of the TCS message. It is done on the
                   allocated array (capDescA parameter) of the user and then
                   returned as a pointer in capDesc parameter. */
                if (capDescA != NULL)
                {
                    memset(capDescA, 0, sizeof(void*) * capDescASize);
					capDescStatus = capDescBuild(hVal, capId, capSetA, capSetASize, capDescASize, capDescA);
                    if (capDescStatus < 0 && capDescStatus != RV_ERROR_NOT_FOUND)
                    {
						RvLogError(&app->log, (&app->log,
							"cmCallGetRemoteCapabilitiesEx:capDescBuild failed."));
                    }
                    if (capDesc)
                        *capDesc = (cmCapStruct****)capDescA;
                }
            }
        }
        H245APIExit((app->pAppObject, "cmCallGetRemoteCapabilitiesEx: [%d].", ret));
        emaUnlock((EMAElement)hsCall);
    }

    if (ret < 0)
    {
        return ret;
    }
    return capId;
}

/* delete pvt of remote capability to reduce memory*/
RVAPI
int RVCALLCONV cmFreeCapability(IN HCALL hsCall)
{
    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);
        H245Object *app = (H245Object*)hApp;

        RV_UNUSED_ARG(app) /* used only when the log is on */
        H245APIEnter((app->pAppObject, "cmFreeCapability: hsCall=%p.", hsCall));

        if (ctrl->eState != ctrlNotInitialized)
        {
            HPVT hVal = ((H245Object *)hApp)->hVal;

            pvtDelete(hVal,  ctrl->outCap.termNodeId);
            ctrl->outCap.termNodeId = RV_ERROR_UNKNOWN;

            pvtDelete(hVal,  ctrl->inCap.termNodeId);
            ctrl->inCap.termNodeId = RV_ERROR_UNKNOWN;
        }
        H245APIExit((app->pAppObject, "cmFreeCapability=1"));
        emaUnlock((EMAElement)hsCall);
    }

    return RV_TRUE;
}



#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
/************************************************************************
 * tcsGetMibParams
 * purpose: Get parameters related to the MIB for a TCS object in the
 *          control
 * input  : ctrl            - Control object
 *          inDirection     - RV_TRUE if incoming TCS should be checked. RV_FALSE for outgoing
 * output : state           - Status of TCS
 *          rejectCause     - Reject cause of TCS.Reject
 * return : Non-negative value on success
 *          Negative value on failure
 * Any of the output parameters can be passed as NULL if not relevant to caller
 ************************************************************************/
int tcsGetMibParams(
    IN  H245Control*    ctrl,
    IN  RvBool          inDirection,
    OUT CapStatus*      status,
    OUT int*            rejectCause)
{
    if (status)
    {
        if (inDirection)
            *status = ctrl->eInCapStatus;
        else
            *status = ctrl->eOutCapStatus;
    }
    if (rejectCause)
    {
        if (inDirection)
            return RV_ERROR_UNKNOWN; /* We never send TCS.Reject */
        else
        {
            if (ctrl->eOutCapStatus == CapStatusRejected)
                *rejectCause = ctrl->outCap.rejectCause;
            else
                return RV_ERROR_UNKNOWN;
        }
    }

    return 0;
}


/************************************************************************
 * tcsGetMibProtocolId
 * purpose: Get the protocol identifier of the capabilities.
 * input  : ctrl            - Control object
 *          inDirection     - RV_TRUE if incoming TCS should be checked. RV_FALSE for outgoing
 * output : valueSize       - Size in bytes of the protocol identifier
 *          value           - The protocol identifier
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int tcsGetMibProtocolId(
    IN  H245Control*    ctrl,
    IN  RvBool          inDirection,
    OUT int*            valueSize,
    OUT RvUint8*        value)
{
    if (inDirection)
    {
        if (ctrl->inCap.pIDLen < 0)
            return RV_ERROR_UNKNOWN;
        *valueSize = ctrl->inCap.pIDLen;
        memcpy(value, ctrl->inCap.pID, (RvSize_t)*valueSize);
    }
    else
    {
        H245Object *app = (H245Object *)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl));

        if (app != NULL)
        {
            *valueSize = app->h245protocolIDLen;
            memcpy(value, app->h245protocolID, (RvSize_t)*valueSize);
        }
    }
    return 0;
}
#endif /* (RV_H245_SUPPORT_H225_PARAMS == RV_YES) */


#if 0
/************************************************************************
 * sendTerminalCapabilitySet
 * purpose: Handle an incoming sendTerminalCapabilitySet message
 * input  : ctrl    - Control object
 *          message - sendTCS.command message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int sendTerminalCapabilitySet(IN H245Control* ctrl, IN int message)
{
    HH245       hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT        hVal;
    H245Object* app;
    RvInt32     status = RV_FALSE;
    RvPvtNodeId nodeId;
    RvPvtNodeId tcsNodeId;
    RvPvtNodeId capNodeId;

    RV_UNUSED_ARG(message);

    app = (H245Object *)hApp;
    hVal = app->hVal;
    if (pvtGetChild2(hVal,app->h245Conf,__h245(capabilities),__h245(manualOperation))<0 &&
        (capNodeId=(pvtGetChild2(hVal,app->h245Conf,__h245(capabilities),__h245(terminalCapabilitySet)))))
    {
        nodeId = pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        tcsNodeId = pvtAddBranch2(hVal,nodeId,__h245(request),__h245(terminalCapabilitySet));
        status = pvtSetTree(hVal,tcsNodeId,hVal,capNodeId);
        if (status == RV_OK)
            status = outCapTransferRequest(ctrl, nodeId);
        else
            pvtDelete(hVal, nodeId);
    }
    return status;
}
#endif


#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
/************************************************************************
 * TerminalCapabilitiesSetTimerStop
 * purpose: stop the TCS timer
 * input  : app     - Application Control object
 *          ctrl    - Call Control object
 * output : none
 * return : None
 ************************************************************************/
void TerminalCapabilitiesSetTimerStop(IN H245Object * app, IN H245Control* ctrl)
{
    app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &ctrl->outCap.timer);
}


/************************************************************************
 * TerminalCapabilitiesTimerRestart
 * purpose: set the TCS timer
 * input  : app     - Application Control object
 *          ctrl    - Call Control object
 * output : none
 * return : None
 ************************************************************************/
void TerminalCapabilitiesSetTimerRestart(IN H245Object * app, IN H245Control* ctrl)
{
    HPVT hVal = app->hVal;
    RvInt32 timeout=9;

    pvtGetChildValue2(hVal, app->h245Conf, __h245(capabilities), __h245(timeout), &timeout, NULL);

    app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &ctrl->outCap.timer);
    ctrl->outCap.timer = app->evHandlers.pfnTimerStartEv((HCONTROL)ctrl,
        capTimeoutEventsHandler, (void*)ctrl, timeout*1000);
}
#endif /* (RV_H245_SUPPORT_H225_PARAMS == RV_YES) */


#ifdef __cplusplus
}
#endif
