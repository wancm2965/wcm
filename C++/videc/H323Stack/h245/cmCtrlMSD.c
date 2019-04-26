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
#include "rvtimestamp.h"
#include "cmictrl.h"
#include "cmConf.h"
#include "caputils.h"
#include "strutils.h"
#include "cmchan.h"
#include "h245.h"
#include "intutils.h"
#include "cmH245Object.h"
#include "cmCtrlMSD.h"

#ifdef __cplusplus
extern "C" {
#endif


int cmcReadyEvent(H245Control* ctrl);




/**************************************************************************************/
/*           Internal routines                                                        */
/**************************************************************************************/

/************************************************************************
 * reportError
 * purpose: To report of an error condition to the user, and return the state
 *          of the procedure to idle.
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
static void reportError(H245Control* ctrl)
{
    HH245            hApp=(HH245)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    H245Object       *app = (H245Object*)hApp;
    MasterSlaveInfo* msd=&ctrl->msd;
    int              nesting;

    /* return the state to idle, the procedure is over */
    msd->state = MsdStateIdle;

    if (app->cmMySessionEvent.cmEvCallMasterSlaveStatus != NULL)
    {
        H245CBEnter((app->pAppObject, "cmEvCallMasterSlaveStatus(haCall=%p, hsCall=%p, status=error)",(HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)), cmiGetByControl((HCONTROL)ctrl)));
        nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
        app->cmMySessionEvent.cmEvCallMasterSlaveStatus((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
                                                        (HCALL)cmiGetByControl((HCONTROL)ctrl),
                                                        (RvUint32)cmMSError);
        emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
        H245CBExit((app->pAppObject, "cmEvCallMasterSlaveStatus."));
    }
}

/************************************************************************
 * sendErrorResponse
 * purpose: To send a reject message due to an error condition.
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
static void sendErrorResponse(H245Control* ctrl)
{
    H245Object*      app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    HPVT             hVal;
    MasterSlaveInfo* msd=&ctrl->msd;
    int              nodeId;
    int              message;

    hVal = app->hVal;

    /* close the timer, no response is expected to this message */
    app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &msd->timer);

    /* report the error to the user and reset the state to idle */
    reportError(ctrl);

    /* build and send the reject message */
    message = pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
    __pvtBuildByFieldIds(nodeId,hVal,message, {_h245(response) _h245(masterSlaveDeterminationReject)
                                               _h245(cause) _h245(identicalNumbers) LAST_TOKEN}, 0, NULL);
    if (nodeId >= 0)
        sendMessageH245((HCONTROL)ctrl, message, RV_FALSE, NULL);
    pvtDelete(hVal,message);
}

/************************************************************************
 * msdTimeoutEventsHandler
 * purpose: Call back in case that no response was received after timeout has expired
 * input  : ctrl            - Control object
 * output : None.
 * return : None.
 ************************************************************************/
RvBool msdTimeoutEventsHandler(void*_ctrl)
{
    H245Control*     ctrl=(H245Control*)_ctrl;
    HCALL            hsCall;
    MasterSlaveInfo* msd=&ctrl->msd;

    hsCall = cmiGetByControl((HCONTROL)ctrl);

    if (emaLock((EMAElement)hsCall))
    {
        HH245   hApp;
        HPVT    hVal;

        hApp = (HH245)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall)));
        hVal = ((H245Object *)hApp)->hVal;

        ((H245Object*)hApp)->evHandlers.pfnTimerClearEv((HCONTROL)ctrl, &msd->timer);

        /* if it's on the first response from the remote, cancel the procedure */
        if (msd->state == MsdStateOutgoingAwaitingResponse)
        {
            /* send a release message */
            int message = pvtAddRoot(hVal,((H245Object*)hApp)->hSynProtH245,0,NULL);
            pvtAddBranch2(hVal,message, __h245(indication), __h245(masterSlaveDeterminationRelease));
            sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, NULL);

            /* inform the user of the error and return to idle state */
            reportError(ctrl); /* Error A */
        }
        /* if it's after we sent an ACK just report to the user and return to idle state */
        else if (msd->state == MsdStateIncomingAwaitingResponse)
            reportError(ctrl); /* Error A */
        emaUnlock((EMAElement)hsCall);
    }
    return RV_TRUE;
}


/************************************************************************
 * determineStatus
 * purpose: The actual algorithm to determine the master & slave according to
 *          the terminal type and determination number.
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
static void determineStatus(H245Control* ctrl)
{
    MasterSlaveInfo* msd=&ctrl->msd;

    /* if the terminal type is the same we need to decide according to the determination
       number */
    if (msd->myTerminalType == msd->remoteTerminalType)
    {
        /* calculate the difference between the determination numbers using lower 24bits */
        int n=(msd->statusDeterminationNumber - msd->myStatusDeterminationNumber + 0x1000000) % 0x1000000;

        /* if the difference is zero, we can't decide,
           otherwise if the difference is within the 24 bits, i.e. my number is smaller, we are slave
           else we are master */
        msd->status= (n & 0x7fffff) ?  ((n<0x800000) ? MsdStatusMaster : MsdStatusSlave)  : MsdStatusIndeterminate;
    }
    else
        /* if there are different terminal types the bigger is masetr */
        msd->status =  (msd->remoteTerminalType < msd->myTerminalType) ? MsdStatusMaster : MsdStatusSlave;
}

/************************************************************************
 * setAndSendMasterSlave
 * purpose: To set the response into the control data structure,
 *          send an ACK to the remote,
 *          report the result to the user.
 * input  : ctrl    - Control object
 *          isMaster - RV_TRUE if we are master
 * output : none
 * return : none
 ************************************************************************/
static void setAndSendMasterSlave(H245Control* ctrl,int isMaster, int report)
{
    HH245            hApp=(HH245)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    HPVT             hVal;
    HCALL            hCall=(HCALL)cmiGetByControl((HCONTROL)ctrl);
    H245Object*      app = (H245Object*)hApp;
    MasterSlaveInfo* msd=&ctrl->msd;
    int              nodeId, nesting, message, res;

    hVal = ((H245Object *)hApp)->hVal;

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    RV_UNUSED_ARG(hCall);
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */

    /* set the result to the control DB */
    ctrl->bIsMaster = (RvBool)isMaster;
    msd->status=(isMaster) ? MsdStatusMaster : MsdStatusSlave;

    /* send a response to the remote with the result */
    message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
    __pvtBuildByFieldIds(nodeId,hVal,message, {_h245(response) _h245(masterSlaveDeterminationAck)
                                               _h245(decision) LAST_TOKEN}, 0, NULL);
    pvtAddBranch(hVal,nodeId,(isMaster)?__h245(slave):__h245(master));
    res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, NULL);
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
    /*Set default muxer mux table for ACP if we have to*/
    if (ctrl->bACPinUse == RV_TRUE)
    {
        cmiSetH223ACPDefaultMuxTbl(hCall);
    }
#endif /* #if (RV_H245_SUPPORT_H223_PARAMS == 1) */
    /*Raise cmMasterSlaveExpectedStatus to the application*/
    if (app->cmMySessionEvent.cmEvCallMasterSlaveExpectedStatus != NULL)
    {
        int nesting;
        H245CBEnter((app->pAppObject, "cmEvCallMasterSlaveExpectedStatus(haCall=%p, hsCall=%p, status=%s)",(HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),cmiGetByControl((HCONTROL)ctrl), (ctrl->bIsMaster )?"master":"slave"));
        nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
        app->cmMySessionEvent.cmEvCallMasterSlaveExpectedStatus((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
                                                                (HCALL)cmiGetByControl((HCONTROL)ctrl),
                                                                (RvBool)((ctrl->bIsMaster )?cmMSMaster:cmMSSlave));
        emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
        H245CBExit((app->pAppObject, "cmEvCallMasterSlaveExpectedStatus"));
    }

    if (report)
    {
        /* The procedure is finished */
        msd->state = MsdStateIdle;

        /* report the result to the user */
        if ((app->cmMySessionEvent.cmEvCallMasterSlaveStatus != NULL) && (res >= 0))
        {
            H245CBEnter((app->pAppObject, "cmEvCallMasterSlaveStatus(haCall=%p, hsCall=%p, status=%s)",
                (HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),cmiGetByControl((HCONTROL)ctrl), (isMaster)?"master":"slave"));
            nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
            app->cmMySessionEvent.cmEvCallMasterSlaveStatus((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),(HCALL)cmiGetByControl((HCONTROL)ctrl), (RvUint32)((isMaster)?cmMSMaster:cmMSSlave));
            emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
            H245CBExit((app->pAppObject, "cmEvCallMasterSlaveStatus"));
        }

        /*set bSaveCallIndication if control in ctrlMesFinished state*/
        if (ctrl->eState == ctrlMesFinished)
        {
            cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
        }
    }

    /* check if the H.245 opening formalities (i.e. TCS & MSD procedures) have ended,
       if so, report to the user that the H.245 is ready for operation */
    if (res >= 0)
        cmcReadyEvent(ctrl);
}


/************************************************************************
 * countCheckAndSend
 * purpose: Retry mechanism. Check if retries expired, if not send a new
 *          request with a new determination number, else send a reject and
 *          finish the procedure.
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
static void countCheckAndSend(H245Control* ctrl)
{
    HH245            hApp=(HH245)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    H245Object *     app = (H245Object *)hApp;
    MasterSlaveInfo* msd=&ctrl->msd;
    int              nodeId, message, res;

    /* check if retry expired, it's always 3 */
    if (msd->count>=3)
    { /* >= N100 */ /*Error F */
        /* send reject and terminate the procedure */
        sendErrorResponse(ctrl);
    }
    else
    { /* < N100 */
        HPVT hVal = app->hVal;
        RvBool sentOnDummyHost;

        /* regenerate a new random 24 bit determination number */
        {
            RvRandom randomValue;
            HCALL    hsCall = (HCALL)cmiGetByControl((HCONTROL)ctrl);
            HAPP     h3G324m = (HAPP)emaGetInstance((EMAElement)hsCall);

            RvLockGet(&app->lock, app->logMgr);
            app->evHandlers.pfnRandomNumberEv(h3G324m, hsCall, &app->randomGenerator, &randomValue);
            RvLockRelease(&app->lock, app->logMgr);
            msd->myStatusDeterminationNumber =
                (int)(randomValue * RvInt64ToRvUint32(RvTimestampGet(app->logMgr))) & 0xffffff;
        }

        /* build a new request with the new number */
        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message,__h245(request), __h245(masterSlaveDetermination));
        pvtAdd(hVal,nodeId,__h245(statusDeterminationNumber),msd->myStatusDeterminationNumber, NULL,NULL);
        pvtAdd(hVal,nodeId,__h245(terminalType),msd->myTerminalType, NULL, NULL);

        /* advance the retries counter */
        msd->count++;

        /* send the message */
        res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, &sentOnDummyHost);

        if (res >= 0)
        {
            if (!sentOnDummyHost)
            {
                /* set the timer to wait for a response on the request */
                RvInt32 timeout=9;
                pvtGetChildValue2(hVal,((H245Object*)hApp)->h245Conf,__h245(masterSlave),__h245(timeout),&(timeout),NULL);
                app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &msd->timer);
                msd->timer = app->evHandlers.pfnTimerStartEv((HCONTROL)ctrl,
                    msdTimeoutEventsHandler, (void*)ctrl, timeout*1000);
            }

            /* change the state so we know we are waiting for a response to our request */
            msd->state=MsdStateOutgoingAwaitingResponse;
        }
    }
}

/*******************************************************************************
 * processMSD
 *
 * Purpose: To respond to a MSD by building the appropriate message and sending
 *          it according to the status of the procedure
 *
 * Input:   ctrl - The control elemnt
 *
 * Output: None.
 *
 * Return: Non-negative value on success, other on failure
 ******************************************************************************/
static int processMSD(H245Control* ctrl)
{
    H245Object*      app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    HCALL            hCall=(HCALL)cmiGetByControl((HCONTROL)ctrl);
    HPVT             hVal;
    MasterSlaveInfo* msd=&ctrl->msd;
    int              nodeId, message, res = RV_ERROR_UNKNOWN;

    hVal = app->hVal;

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    RV_UNUSED_ARG(hCall);
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */

    /* Initiate THE algorithm to determine master/slave */
    determineStatus(ctrl);


    if (msd->status==MsdStatusIndeterminate)
    { /* indeterminate */
        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        __pvtBuildByFieldIds(nodeId,hVal,message, {_h245(response) _h245(masterSlaveDeterminationReject)
          _h245(cause) _h245(identicalNumbers) LAST_TOKEN}, 0, NULL);
        res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, NULL);

        /* all is over */
        msd->state=MsdStateIdle;
    }
    else
    { /* master or slave */
        RvBool sentOnDummyHost;

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        __pvtBuildByFieldIds(nodeId,hVal,message, {_h245(response) _h245(masterSlaveDeterminationAck)
          _h245(decision) LAST_TOKEN}, 0, NULL);
        pvtAddBranch(hVal,nodeId,(msd->status==MsdStatusMaster) ? __h245(slave) : __h245(master));

        /* Update the control element as well */
        ctrl->bIsMaster = (msd->status==MsdStatusMaster);
        ctrl->bIsMasterSlave = RV_TRUE;

        res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, &sentOnDummyHost);
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
        /*Set default muxer mux table for ACP if we have to*/
        if (ctrl->bACPinUse == RV_TRUE)
        {
            cmiSetH223ACPDefaultMuxTbl(hCall);
        }
#endif /* #if (RV_H245_SUPPORT_H223_PARAMS == 1) */
        /*Raise cmMasterSlaveExpectedStatus to the application*/
        if (app->cmMySessionEvent.cmEvCallMasterSlaveExpectedStatus != NULL)
        {
            int nesting;
            H245CBEnter((app->pAppObject, "cmEvCallMasterSlaveExpectedStatus(haCall=%p, hsCall=%p, status=%s)",(HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),cmiGetByControl((HCONTROL)ctrl), (ctrl->bIsMaster )?"master":"slave"));
            nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
            app->cmMySessionEvent.cmEvCallMasterSlaveExpectedStatus((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
                                                                    (HCALL)cmiGetByControl((HCONTROL)ctrl),
                                                                    (RvBool)((ctrl->bIsMaster )?cmMSMaster:cmMSSlave));
            emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
            H245CBExit((app->pAppObject, "cmEvCallMasterSlaveExpectedStatus"));
        }

        if (res >= 0)
        {
            if (!sentOnDummyHost)
            {
                /* set a response timer since we need an ack back on our decision */
                int timeout=9;
                pvtGetChildValue2(hVal,app->h245Conf,__h245(masterSlave),__h245(timeout),&(timeout),NULL);
                app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &msd->timer);
                msd->timer = app->evHandlers.pfnTimerStartEv((HCONTROL)ctrl,
                    msdTimeoutEventsHandler, (void*)ctrl, timeout*1000);
            }
            /* waiting for the ack on the ack */
            msd->state=MsdStateIncomingAwaitingResponse;
        }
    }

    return res;
}


/**************************************************************************************/
/*           Internal routines used outside this file                                                        */
/**************************************************************************************/

/************************************************************************
 * msdInit
 * purpose: Initialize the master slave determination process on a control channel
 * input  : ctrl            - Control object
 *          terminalType    - Terminal type to use. -1 if we want to use configuration value.
 * output : none
 * return : none
 ************************************************************************/
void msdInit(
    IN H245Control* ctrl,
    IN RvInt32      terminalType)
{
    H245Object*      app = (H245Object *)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT             hVal = app->hVal;
    MasterSlaveInfo* msd = &ctrl->msd;
    RvPvtNodeId      msdConfNodeId;

    msd->status = MsdStatusIndeterminate;
    msd->state = MsdStateIdle;
    msd->timer = NULL;

    msdConfNodeId = pvtGetChild(hVal, app->h245Conf, __h245(masterSlave), NULL);

    if (terminalType < 0)
    {
        if (ctrl->eParamsType == RvH245ChannelParamsH223)
            terminalType = 128;
        else
            terminalType = 50;
        pvtGetChildByFieldId(hVal, msdConfNodeId, __h245(terminalType), &terminalType, NULL);
    }
    msd->myTerminalType = (RvUint8)terminalType;

    msd->myStatusDeterminationNumber = -1;

    msd->manualResponse = (pvtGetChild(hVal, msdConfNodeId, __h245(manualResponse), NULL) >= 0);

    RvLogInfo(&app->log,
        (&app->log, "MSD manual response on %p = %d", ctrl, msd->manualResponse));
}


/************************************************************************
 * msdEnd
 * purpose: Stop the master slave determination process on a control channel
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
void msdEnd(IN H245Control* ctrl)
{
    MasterSlaveInfo* msd=&ctrl->msd;
    H245Object* app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));

    /* Get rid of the timer if one exists */
    app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &msd->timer);
}

/*************************************************************************************
 * msdDetermineRequest
 *
 * Purpose: Main routine to initiate or respond to a MSD request.
 *
 * Input:   ctrl                        - The ctrl element of the call
 *          terminalType                - The given terminal type of the call, may be -1.
 *          statusDeterminationNumber   - The determination numer, may be -1 and then needs to be
 *                                        generated.
 * Output: None.
 *
 * return : Non-negative value on success
 *          Negative value on failure
 ******************************************************************************************/
int msdDetermineRequest(H245Control* ctrl, int terminalType, int statusDeterminationNumber)
{
    HH245            hApp=(HH245)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    H245Object*      app = (H245Object *)hApp;
    HPVT             hVal;
    MasterSlaveInfo* msd=&ctrl->msd;
    int              nodeId, message, res = RV_ERROR_UNKNOWN;

    if((msd->state!=MsdStateIdle) && (msd->state!=MsdStateIncomingAwaitingManualAcknoledge))
        return RV_ERROR_UNKNOWN;

    hVal = app->hVal;

    /* if this is a new MSD process, re-generate the random determination number */
    if (msd->myStatusDeterminationNumber < 0)
    {
        /* if the determination number is -1, generate a new random 24 bit determination number */
        if (statusDeterminationNumber < 0)
        {
            RvRandom randomValue;
            HCALL    hsCall = (HCALL)cmiGetByControl((HCONTROL)ctrl);
            HAPP     h3G324m = (HAPP)emaGetInstance((EMAElement)hsCall);

            RvLockGet(&app->lock, app->logMgr);
            app->evHandlers.pfnRandomNumberEv(h3G324m, hsCall, &app->randomGenerator, &randomValue);
            RvLockRelease(&app->lock, app->logMgr);

            statusDeterminationNumber = (int)(randomValue & 0xffffff);
        }
        msd->myStatusDeterminationNumber=statusDeterminationNumber;
    }

    /* if terminal type is not given, i.e. -1, we just use the one from msdInit */
    if (terminalType >= 0)
        msd->myTerminalType = (RvUint8)terminalType;

    /* if the procedure hasn't started yet initiate it */
    if (msd->state == MsdStateIdle)
    {
        RvBool sentOnDummyHost;

        /* build the request message with the terminaType and determinationNumber */
        message = pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message,__h245(request), __h245(masterSlaveDetermination));
        pvtAdd(hVal,nodeId,__h245(statusDeterminationNumber),msd->myStatusDeterminationNumber, NULL,NULL);
        pvtAdd(hVal,nodeId,__h245(terminalType),msd->myTerminalType, NULL,NULL);

        /* Initialize the retry mechanism and send the message */
        msd->count=1;
        res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, &sentOnDummyHost);

        if (res >= 0)
        {
            /* Make sure message was actually sent and we didn't use a "dummy" H245 (as in the GK's MC) */
            if (!sentOnDummyHost)
            {
                /* We're only here if we really sent the message */
                /* start a timer to wait for response */
                int timeout=9;
                pvtGetChildValue2(hVal,app->h245Conf,__h245(masterSlave),__h245(timeout),&(timeout),NULL);
                app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &msd->timer);
                msd->timer = app->evHandlers.pfnTimerStartEv((HCONTROL)ctrl,
                    msdTimeoutEventsHandler, (void*)ctrl, timeout*1000);
            }
            /* change the state of the procedure */
            msd->state=MsdStateOutgoingAwaitingResponse;
        }
    }
    else if (msd->state==MsdStateIncomingAwaitingManualAcknoledge)
    {
        /* in case that we are in a state waiting to respond to a request, build the response and send it */
        res = processMSD(ctrl);
    }

    return res;
}


#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
/************************************************************************
 * msdGetMibParams
 * purpose: Get parameters related to the MIB for an MSD object in the
 *          control
 * input  : ctrl            - Control object
 * output : state           - State of MSD
 *          status          - Status of MSD
 *          retries         - Number of MSD retries
 *          terminalType    - Local terminal's type in MSD
 * return : Non-negative value on success
 *          Negative value on failure
 * Any of the output parameters can be passed as NULL if not relevant to caller
 ************************************************************************/
int msdGetMibParams(
    IN  H245Control*    ctrl,
    OUT MsdState*       state,
    OUT MsdStatus*      status,
    OUT int*            retries,
    OUT int*            terminalType)
{
    if (state)
        *state = ctrl->msd.state;
    if (status)
        *status = ctrl->msd.status;
    if (retries)
        *retries = ctrl->msd.count;
    if (terminalType)
        *terminalType = ctrl->msd.myTerminalType;

    return 0;
}
#endif /* (RV_H245_SUPPORT_H225_PARAMS == RV_YES) */



/**************************************************************************************/
/*           Handling incoming MSD messages                                           */
/**************************************************************************************/


/************************************************************************
 * masterSlaveDetermination
 * purpose: Handle an incoming MasterSlaveDetermination request,
 *          notifying the application or answering automatically.
 * input  : ctrl    - Control object
 *          message - MSD request message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int masterSlaveDetermination(IN H245Control* ctrl, IN int message)
{
    HH245            hApp=(HH245)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    HPVT             hVal;
    MasterSlaveInfo* msd=&ctrl->msd;
    RvInt32          value = 0;
    int              res = 0;
    H245Object       *app;

    hVal = ((H245Object *)hApp)->hVal;
    app = (H245Object*)hApp;
    /* get the remote numbers from the message */
    pvtGetChildByFieldId(hVal, message, __h245(terminalType) ,&value, NULL);
    msd->remoteTerminalType = (RvUint8)value;
    pvtGetChildByFieldId(hVal, message, __h245(statusDeterminationNumber), &(msd->statusDeterminationNumber), NULL);

    /* if we have not initiated yet an MSD procedure */
    switch (msd->state)
    {
        case MsdStateIdle:
        {
            /* in case of manual response, inform the application of the initiation */
            if (msd->manualResponse && (app->cmMySessionEvent.cmEvCallMasterSlave != NULL))
            {
                int nesting;
                msd->state = MsdStateIncomingAwaitingManualAcknoledge;
                H245CBEnter((app->pAppObject, "cmEvCallMasterSlave(haCall=%p, hsCall=%p, terminalType =%d, statusDeterminationNumber=%d)",
                    (HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),cmiGetByControl((HCONTROL)ctrl), msd->remoteTerminalType, msd->statusDeterminationNumber));
                nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
                app->cmMySessionEvent.cmEvCallMasterSlave((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),(HCALL)cmiGetByControl((HCONTROL)ctrl), (RvUint32)msd->remoteTerminalType, (RvUint32)msd->statusDeterminationNumber);
                emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
                H245CBExit((app->pAppObject, "cmEvCallMasterSlave"));
            }
            else
                /* Automatic response: Determine the master/slave and issue a response */
                res = processMSD(ctrl);
            break;
        }

        case MsdStateOutgoingAwaitingResponse:
        /* We have already initiated an MSD procedure from our side */
        {
            /* close the timer, this message is as good as an ack */
            app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &msd->timer);

            /* activate THE algorithm to determine the master/slave */
            determineStatus(ctrl);

            /* According to the result respond to the remote request */
            if (msd->status==MsdStatusIndeterminate)
            { /* indeterminate: retry sending the request with a new determination number */
                countCheckAndSend(ctrl);
            }
            else
            { /* master or slave */

                /* set the result into the control DB, send ack to the remote with the result
                   and report the result to the user */
                setAndSendMasterSlave(ctrl, msd->status==MsdStatusMaster, RV_FALSE);
                {
                    /* we need an ack on the ack so set a response timer */
                    RvInt32 timeout = 9;
                    pvtGetChildValue2(hVal, app->h245Conf, __h245(masterSlave), __h245(timeout), &timeout, NULL);
                    msd->timer = app->evHandlers.pfnTimerStartEv((HCONTROL)ctrl,
                        msdTimeoutEventsHandler, (void*)ctrl, timeout * 1000);
                }
                /* we are waiting for ack on the ack */
                msd->state = MsdStateIncomingAwaitingResponse;
            }
            break;
        }

        case MsdStateIncomingAwaitingResponse:
            /* the remote has already initiated an MSD procedure with us, this is a mistake */
            sendErrorResponse(ctrl); /* Error C */
            break;

        case MsdStateIncomingAwaitingManualAcknoledge:
            reportError(ctrl);  /* Error C */
            break;
    }

    return res;
}


/************************************************************************
 * masterSlaveDeterminationAck
 * purpose: Handle an incoming MasterSlaveDeterminationAck message
 * input  : ctrl    - Control object
 *          message - MSD.Ack message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int masterSlaveDeterminationAck(IN H245Control* ctrl, IN int message)
{
    H245Object*     app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT             hVal;
    MasterSlaveInfo* msd=&ctrl->msd;
    RvBool           isMaster;

    hVal = app->hVal;

    /* Get the remote decision */
    isMaster = (pvtGetChild2(hVal,message,__h245(decision),__h245(master))>=0);

    /* reset the timer, we got response */
    app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &msd->timer);

    /* we are waiting for first ACK of our request */
    if (msd->state==MsdStateOutgoingAwaitingResponse)
    {
        /* mark that we have finished at least one MSD procedure, so H.245 may start operate */
        ctrl->bIsMasterSlave = RV_TRUE;

        /* Set the result given by the remote to our control DB, send an ack on this ack
           and report the result to the user */
        setAndSendMasterSlave(ctrl, (int)isMaster, RV_TRUE);
    }
    /* This is actually the ack on the ack we sent */
    else if (msd->state==MsdStateIncomingAwaitingResponse)
    {
        RvBool localIsMaster = (msd->status==MsdStatusMaster);

        /* check if the remote agrees with the result we sent it */
        if (isMaster == localIsMaster)
        {   /* There is an agreement, so the procedure is finished */
            int nesting;

            /* mark that we have finished at least one MSD procedure, so H.245 may start operate */
            ctrl->bIsMasterSlave = RV_TRUE;

            /* The procedure is finished - change the state and notify the application */
            msd->state = MsdStateIdle;

            /* Report the result to the user */
            if (app->cmMySessionEvent.cmEvCallMasterSlaveStatus != NULL)
            {
                H245CBEnter((app->pAppObject, "cmEvCallMasterSlaveStatus(haCall=%p, hsCall=%p, status=%s)",(HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),cmiGetByControl((HCONTROL)ctrl), (isMaster)?"master":"slave"));
                nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
                app->cmMySessionEvent.cmEvCallMasterSlaveStatus((HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
                                                                  (HCALL)cmiGetByControl((HCONTROL)ctrl),
                                                                  (RvBool)((isMaster)?cmMSMaster:cmMSSlave));
                emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
                H245CBExit((app->pAppObject, "cmEvCallMasterSlaveStatus"));
            }

            /*set bSaveCallIndication if control in ctrlMesFinished state*/
            if (ctrl->eState == ctrlMesFinished)
            {
                cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
            }
        }
        else
          /* oops! We disagree with the remote, i.e. error state */
          sendErrorResponse(ctrl); /* Error E */
    }

    /* check if the H.245 opening formalities (i.e. TCS & MSD procedures) have ended,
       if so, report to the user that the H.245 is ready for operation */
    cmcReadyEvent(ctrl);
    return RV_TRUE;
}


/************************************************************************
 * masterSlaveDeterminationReject
 * purpose: Handle an incoming MasterSlaveDeterminationReject message
 * input  : ctrl    - Control object
 *          message - MSD.Reject message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int masterSlaveDeterminationReject(IN H245Control* ctrl, IN int message)
{
    H245Object*      app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    MasterSlaveInfo* msd=&ctrl->msd;

    RV_UNUSED_ARG(message);

    /* in case it's a reject on our request */
    if (msd->state==MsdStateOutgoingAwaitingResponse)
    {
        /* reset the timer, we got response */
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &msd->timer);
        /* retry sending the request with a new detrmination number */
        countCheckAndSend(ctrl);
    }
    /* if its a reject on the ack that we've sent, it's an error */
    else if (msd->state==MsdStateIncomingAwaitingResponse)
          sendErrorResponse(ctrl); /* Error D */
    else if (msd->state==MsdStateIncomingAwaitingManualAcknoledge)
          reportError(ctrl);  /* Error D */
    return RV_TRUE;
}


/************************************************************************
 * masterSlaveDeterminationRelease
 * purpose: Handle an incoming MasterSlaveDeterminationRelease message
 * input  : ctrl    - Control object
 *          message - MSD.Release message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int masterSlaveDeterminationRelease(IN H245Control* ctrl, IN int message)
{
    MasterSlaveInfo* msd=&ctrl->msd;

    RV_UNUSED_ARG(message);

    /* this isi always an error state if a release was received during the procedure */
    switch(msd->state)
    {
        case MsdStateOutgoingAwaitingResponse:
        case MsdStateIncomingAwaitingResponse:
          sendErrorResponse(ctrl); /* Error B */
        break;
        case MsdStateIncomingAwaitingManualAcknoledge:
          reportError(ctrl);  /* Error B */
        break;
        default:
            break;
    }
    return RV_TRUE;
}


#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
/************************************************************************
 * masterSlaveTimerStop
 * purpose: stop the MSD timer
 * input  : app     - Application Control object
 *          ctrl    - Call Control object
 * output : none
 * return : None
 ************************************************************************/
void masterSlaveTimerStop(IN H245Object * app, IN H245Control* ctrl)
{
    app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &ctrl->msd.timer);
}


/************************************************************************
 * masterSlaveTimerRestart
 * purpose: set the MSD timer
 * input  : app     - Application Control object
 *          ctrl    - Call Control object
 * output : none
 * return : None
 ************************************************************************/
void masterSlaveTimerRestart(IN H245Object * app, IN H245Control* ctrl)
{
    HPVT hVal = app->hVal;
    RvInt32 timeout=9;

    pvtGetChildValue2(hVal, app->h245Conf, __h245(masterSlave),__h245(timeout),&(timeout),NULL);

    app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &ctrl->msd.timer);
    ctrl->msd.timer = app->evHandlers.pfnTimerStartEv((HCONTROL)ctrl,
        msdTimeoutEventsHandler, (void*)ctrl, timeout*1000);
}
#endif


/************************************************************************/
/*                             API                                      */
/************************************************************************/

/************************************************************************
 * cmCallMasterSlaveDetermine
 * purpose: Start master slave determination request
 * input  : hsCall          - The call handle
 *          terminalType    - Our terminal type
 * output : None
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV
cmCallMasterSlaveDetermine(
               IN   HCALL       hsCall,
               IN   int         terminalType)
{
    int res = RV_ERROR_UNKNOWN;
    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        H245Object  *app=(H245Object*)(cmiGetH245Handle((HAPP)(emaGetInstance((EMAElement)hsCall))));
        H245Control *ctrl=(H245Control*)cmiGetControl(hsCall);

        RV_UNUSED_ARG(app) /* used only when the log is on */
        H245APIEnter((app->pAppObject, "cmMasterSlaveDetermine: hsCall=%p, terminalType=%d.", hsCall, terminalType));
        res = msdDetermineRequest(ctrl, terminalType, -1);

        H245APIExit((app->pAppObject, "cmMasterSlaveDetermine=%d", res));
        emaUnlock((EMAElement)hsCall);
    }
    return res;
}

/************************************************************************
 * cmCallMasterSlaveDetermineExt
 * purpose: Start master slave determination request,
 *          or Acknowledge to master slave determination request
 *          and set terminalType and determinationNumber.
 *          Note that if terminalType is a negative value then the
 *          terminalType value from configuration will be used. If determinationNumber
 *          is negative then the stack will be responsible for supplying
 *          this number.
 * input  : hsCall              - The call handle
 *          terminalType        - A given terminal type
 *          determinationNumber - A given determination number
 * output : None
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV
cmCallMasterSlaveDetermineExt(
               IN   HCALL   hsCall,
               IN   int     terminalType,
               IN   int     determinationNumber)
{
    int res = RV_ERROR_UNKNOWN;

    if (hsCall == NULL) return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        H245Object  *app=(H245Object*)(cmiGetH245Handle((HAPP)(emaGetInstance((EMAElement)hsCall))));
        H245Control *ctrl=(H245Control*)cmiGetControl(hsCall);

        RV_UNUSED_ARG(app) /* used only when the log is on */
        H245APIEnter((app->pAppObject, "cmMasterSlaveDetermineExt: hsCall=%p, terminalType=%d determinationNumber = %d. ", hsCall, terminalType,determinationNumber));
        res = msdDetermineRequest(ctrl, terminalType, determinationNumber);

        H245APIExit((app->pAppObject, "cmMasterSlaveDetermineExt=%d", res));
        emaUnlock((EMAElement)hsCall);
    }
    return res;
}


/************************************************************************
 * cmCallMasterSlaveStatus
 * purpose: Returns the master/slave status of this call
 * input  : hsCall              - The call handle
 * output : None
 * return : 1 - slave, 2 - master
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV
cmCallMasterSlaveStatus(
            IN  HCALL       hsCall
            )
{
    H245Object   *app;
    H245Control  *ctrl;
    int          ret=0;

    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        app=(H245Object*)(cmiGetH245Handle((HAPP)(emaGetInstance((EMAElement)hsCall))));
        ctrl=(H245Control*)cmiGetControl(hsCall);

        H245APIEnter((app->pAppObject, "cmCallMasterSlaveStatus: hsCall=%p.", hsCall));

        if ((ctrl->bIsMasterSlave == RV_FALSE) || (ctrl->eState == ctrlNotInitialized))
            ret = 0;
        else if (ctrl->bIsMaster)
            ret = 2;
        else
            ret = 1;

        {
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER | RV_LOGLEVEL_LEAVE))
            const char *msStatusA[] = {"error", "slave", "master"};
#endif
            H245APIExit((app->pAppObject, "cmCallMasterSlaveStatus=%s", msStatusA[ret]));
        }
        emaUnlock((EMAElement)hsCall);
    }
    if (ret==0)
        return RV_ERROR_UNKNOWN;
    else
        return ret;
}

/******************************************************************************
 * cmCallMasterSlaveExpectedStatus
 * ----------------------------------------------------------------------------
 * General: Returns the msd status as it's known to the stack even before MSD
 *          process is completed.
 *
 * Return Value: 1 - slave, 2 - master, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall            - Call handle in the stack.
 * Output: None.
 ******************************************************************************/
RVAPI int RVCALLCONV
cmCallMasterSlaveExpectedStatus(
            IN  HCALL       hsCall
            )
{
    H245Object   *app;
    H245Control  *ctrl;
    MsdStatus ret = MsdStatusIndeterminate;

    if (emaLock((EMAElement)hsCall))
    {
        app=(H245Object*)(cmiGetH245Handle((HAPP)(emaGetInstance((EMAElement)hsCall))));
        ctrl=(H245Control*)cmiGetControl(hsCall);

        H245APIEnter((app->pAppObject, "cmCallMasterSlaveExpectedStatus: hsCall=%p.", hsCall));

        ret = ctrl->msd.status;

        {
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER | RV_LOGLEVEL_LEAVE))
            const char *msStatusA[] = {"error", "master", "slave"};
#endif
            H245APIExit((app->pAppObject, "cmCallMasterSlaveExpectedStatus=%s", msStatusA[ret]));
        }
        emaUnlock((EMAElement)hsCall);
    }
    if (ret == MsdStatusIndeterminate)
        return RV_ERROR_UNKNOWN;
    else if (ret == MsdStatusMaster)
        return cmMSMaster;
    else
        return cmMSSlave;
}


#ifdef __cplusplus
}
#endif
