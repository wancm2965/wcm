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

/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "rvtimestamp.h"
#include "cmictrl.h"
#include "cmConf.h"
#include "caputils.h"
#include "strutils.h"
#include "cmchan.h"
#include "h245.h"
#include "intutils.h"
#include "cmH245Object.h"
#include "cmCtrlMES.h"


#ifdef __cplusplus
extern "C" {
#endif

#if (RV_H245_SUPPORT_H223_PARAMS == 1)


/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/
RvBool mesTimeoutEventsHandler(void*_ctrl);
static RvStatus multiplexEntryGetNumbers(
    IN  HPVT        hVal,
    IN  RvPvtNodeId numbersNodeId,
    IN  RvBool      use2levels,
    OUT RvUint16    *entries);






/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * mesInit
 * ----------------------------------------------------------------------------
 * General: Initialize a multiplexEntrySend object of the control.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ctrl - Control object
 *****************************************************************************/
RvStatus mesInit(IN H245Control* ctrl)
{
    H245Object* app = (H245Object *)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    OutgoingMESInfo* outMes=&ctrl->outMES;
    IncomingMESInfo* inMes=&ctrl->inMES;
    RvPvtNodeId tmpNodeId;

    outMes->seqNum = 0;
    outMes->tableInd = 0;
    outMes->timer = NULL;
    outMes->waiting = RV_FALSE;

    __pvtGetNodeIdByFieldIds(tmpNodeId, app->hVal, app->h245Conf, {_h245(multiplexEntryTable) _h245(manualResponse) LAST_TOKEN});
    inMes->manualResponse = RV_PVT_NODEID_IS_VALID(tmpNodeId);
    RvLogInfo(&app->log, (&app->log, "MES manual response on %p = %d", ctrl, inMes->manualResponse));

    return RV_OK;
}


/******************************************************************************
 * mesEnd
 * ----------------------------------------------------------------------------
 * General: Destruct a multiplexEntrySend object of the control.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ctrl - Control object
 *****************************************************************************/
RvStatus mesEnd(IN H245Control* ctrl)
{
    H245Object* app = (H245Object *)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    OutgoingMESInfo* outMes=&ctrl->outMES;

    /* Reset the outgoing multiplex entry timer is one exists */
    app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &outMes->timer);

    return RV_OK;
}


/*************************The MTSEO Functions**************************************/
    /* The MTSEO side is the initiating entity, the side that sends the
       multiplexEntrySend message*/


/******************************************************************************
 * cmCallMultiplexEntrySend
 * ----------------------------------------------------------------------------
 * General: Start multiplexEntrySend request.
 *          message NodeId is not deleted and not modified by this function.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall  - The call handle
 *         message - The root nodeId of the PVT tree to send.
 *                   Of type MultiplexEntrySend.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmCallMultiplexEntrySend(
               IN   HCALL       hsCall,
               IN   RvPvtNodeId message)
{
    RvPvtNodeId nodeId, rootNodeId;
    RvStatus status = RV_ERROR_UNKNOWN;

    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        H245Object  *app=(H245Object*)(cmiGetH245Handle((HAPP)(emaGetInstance((EMAElement)hsCall))));
        H245Control *ctrl=(H245Control*)cmiGetControl(hsCall);
        HPVT hVal = app->hVal;
        OutgoingMESInfo* outMes=&ctrl->outMES;

        H245APIEnter((app->pAppObject, "cmCallMultiplexEntrySend: hsCall=%p, message=%d.",
            hsCall, message));

        if (outMes->waiting == RV_TRUE)
            app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &outMes->timer);

        rootNodeId = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);

        if (RV_PVT_NODEID_IS_VALID(rootNodeId))
        {
            RvBool sentOnDummyHost;

            /* Copy the user's nodeId into our constructed message */
            __pvtBuildByFieldIds(nodeId, hVal, rootNodeId,
                {_h245(request) _h245(multiplexEntrySend) LAST_TOKEN}, 0, NULL);
            pvtSetTree(hVal, pvtAdd(hVal, nodeId, __h245(multiplexEntryDescriptors), 0, NULL, NULL),
                hVal, message);

            /* add a sequence number to the message we got */
            outMes->seqNum++;
            pvtAdd(hVal, nodeId, __h245(sequenceNumber), outMes->seqNum, NULL, NULL);

            status = sendMessageH245((HCONTROL)ctrl, rootNodeId, RV_TRUE, &sentOnDummyHost);

            if (status == RV_OK)
            {
                if (!sentOnDummyHost)
                {
                    RvInt32 timeout = 9;

                    /* Set the timer for this transaction from the configuration */
                    pvtGetChildByFieldId(hVal, app->h245Conf, __h245(multiplexEntrySendTimeout), &timeout, NULL);

                    /* start a timer to wait for response */
                    outMes->timer = app->evHandlers.pfnTimerStartEv((HCONTROL)ctrl,
                        mesTimeoutEventsHandler, (void*)ctrl, timeout*1000);

                    /* Find out which entries are being used and remember them */
                    multiplexEntryGetNumbers(hVal, message, RV_TRUE, &outMes->tableInd);
                }
                /* change the state of the procedure */
                outMes->waiting = RV_TRUE;
            }
        }

        H245APIExit((app->pAppObject, "cmCallMultiplexEntrySend=%d", status));
        emaUnlock((EMAElement)hsCall);
    }

    return status;
}


/******************************************************************************
 * cmCallMultiplexEntrySendResponse
 * ----------------------------------------------------------------------------
 * General: Send multiplexEntrySend response (ack or reject).
 *          message NodeId is not deleted and not modified by this function.
 *          This function can only be called if the H245 configuration
 *          states the use of manual response for multiplex entry table
 *          (i.e: h245.multiplexEntryTable.manualResponse is set).
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall  - The call handle
 *         isOk    - RV_TRUE if multiplexEntrySendAck is to be sent.
 *                   RV_FALSE if multiplexEntrySendReject is to be sent.
 *         message - The root nodeId of the PVT tree to send.
 *                   Should hold the MultiplexEntrySendAck.multiplexTableEntryNumber
 *                   if isOk==RV_TRUE,
 *                   or MultiplexEntrySendReject.rejectionDescriptions
 *                   if isOk==RV_FALSE.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmCallMultiplexEntrySendResponse(
               IN   HCALL       hsCall,
               IN   RvBool      isOk,
               IN   RvPvtNodeId message)
{
    RvStatus status = RV_ERROR_UNKNOWN;

    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        H245Object  *app=(H245Object*)(cmiGetH245Handle((HAPP)(emaGetInstance((EMAElement)hsCall))));
        H245Control *ctrl=(H245Control*)cmiGetControl(hsCall);
        HPVT hVal = app->hVal;
        IncomingMESInfo* inMes=&ctrl->inMES;
        RvPvtNodeId rootNodeId, nodeId;

        H245APIEnter((app->pAppObject,
            "cmCallMultiplexEntrySendResponse: hsCall=%p, isOk=%d, message=%d.",
            hsCall, isOk, message));

        /* Make sure we actually received a MultiplexEntrySend message and that we're
           using manualResponse for it */
        if ((ctrl->eInMesStatus == MesStatusSent) && ctrl->inMES.manualResponse)
        {
            rootNodeId = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);

            if (isOk)
            {
                /* build the response message with the descriptors tree we got */
                __pvtBuildByFieldIds(nodeId, hVal, rootNodeId,
                    {_h245(response) _h245(multiplexEntrySendAck) LAST_TOKEN}, 0, NULL);
                pvtAdd(hVal, nodeId, __h245(sequenceNumber), inMes->seqNum, NULL, NULL);

                pvtMoveTree(hVal,
                    pvtAdd(hVal, nodeId, __h245(multiplexTableEntryNumber), 0, NULL, NULL),
                    message);

                /* change the state of the procedure */
                ctrl->eInMesStatus = MesStatusAcknowledged;
            }
            else
            {
                /* build the response message with the descriptors tree we got */
                __pvtBuildByFieldIds(nodeId, hVal, rootNodeId,
                    {_h245(response) _h245(multiplexEntrySendReject) LAST_TOKEN}, 0, NULL);
                pvtAdd(hVal, nodeId, __h245(sequenceNumber), inMes->seqNum, NULL, NULL);

                pvtMoveTree(hVal,
                    pvtAdd(hVal, nodeId, __h245(rejectionDescriptions), 0, NULL, NULL),
                    message);

                /* change the state of the procedure */
                ctrl->eInMesStatus = MesStatusRejected;

            }

            /* Send the message and delete it */
            status = sendMessageH245((HCONTROL)ctrl, rootNodeId, RV_TRUE, NULL);
        }

        H245APIExit((app->pAppObject, "cmCallMultiplexEntrySendResponse=%d", status));

        emaUnlock((EMAElement)hsCall);
    }

    return status;
}



/************************************************************************
 * multiplexEntrySendAck
 * purpose: Handle an incoming multiplexEntrySendAck message
 * input  : ctrl    - Control object
 *          message - MES.Ack message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int multiplexEntrySendAck(IN H245Control* ctrl, IN int message)
{
    HH245            hApp=cmiGetH245Handle((HAPP)emaGetInstance(
                            (EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT             hVal;
    OutgoingMESInfo* outMes=&ctrl->outMES;
    int sq;

    hVal = ((H245Object *)hApp)->hVal;
    /* get the sequence number*/
    pvtGetChildByFieldId(hVal, message, __h245(sequenceNumber), &sq, NULL);
    if ((outMes->waiting) && (sq == outMes->seqNum))
    {
        H245Object *app = (H245Object*)hApp;
        int nesting;

        /* reset the timer, we got response */
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &outMes->timer);

        /* We're done waiting */
        outMes->waiting = RV_FALSE;

        /* Report the result to the user */
        if (app->cmMySessionEvent.cmEvCallMultiplexEntryResponse != NULL)
        {
            RvUint16 entries;
            RvPvtNodeId numbersNodeId;

            numbersNodeId = pvtGetChild(hVal, message, __h245(multiplexTableEntryNumber), NULL);
            multiplexEntryGetNumbers(hVal, numbersNodeId, RV_FALSE, &entries);

            H245CBEnter((app->pAppObject,
                "cmEvCallMultiplexEntryResponse: haCall=%p, hsCall=%p, isAck=TRUE, entries=%d,nodeId=%d.",
                (HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
                cmiGetByControl((HCONTROL)ctrl), entries, numbersNodeId));
            nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
            app->cmMySessionEvent.cmEvCallMultiplexEntryResponse(
                (HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
                (HCALL)cmiGetByControl((HCONTROL)ctrl),
                RV_TRUE,
                entries,
                numbersNodeId);
            emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
            H245CBExit((app->pAppObject,"cmEvCallMultiplexEntryResponse."));
        }

        /* Notify the upper layer that MultiplexEntrySend mechanism is finished */
        if (app->evHandlers.pfnNotifyStateEv != NULL)
        {
            ctrl->eState = ctrlMesFinished;
            app->evHandlers.pfnNotifyStateEv(cmiGetByControl((HCONTROL)ctrl),
                ctrlMesFinished);
        }

        /*set bSaveCallIndication if control in ctrlMesFinished state*/
        if (ctrl->eState == ctrlMesFinished)
        {
            cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
        }
    }

    return RV_TRUE;
}

/************************************************************************
 * multiplexEntrySendReject
 * purpose: Handle an incoming multiplexEntrySendReject message
 * input  : ctrl    - Control object
 *          message - MES.Reject message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int multiplexEntrySendReject(IN H245Control* ctrl, IN int message)
{
    HH245            hApp=cmiGetH245Handle((HAPP)emaGetInstance(
                            (EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT             hVal;
    OutgoingMESInfo* outMes=&ctrl->outMES;
    int sq;

    hVal = ((H245Object *)hApp)->hVal;
    /* get the sequence number*/
    pvtGetChildByFieldId(hVal, message, __h245(sequenceNumber), &sq, NULL);
    if ((outMes->waiting) && (sq == outMes->seqNum))
    {
        H245Object *app = (H245Object*)hApp;
        int nesting;

        /* reset the timer, we got response */
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &outMes->timer);

        /* We're done waiting */
        outMes->waiting = RV_FALSE;

        /* Report the result to the user */
        if (app->cmMySessionEvent.cmEvCallMultiplexEntryResponse != NULL)
        {
            RvUint16 entries;
            RvPvtNodeId numbersNodeId;

            numbersNodeId = pvtGetChild(hVal, message, __h245(rejectionDescriptions), NULL);
            multiplexEntryGetNumbers(hVal, numbersNodeId, RV_TRUE, &entries);

            H245CBEnter((app->pAppObject,"cmEvCallMultiplexEntryResponse: haCall=%p, hsCall=%p, isAck=FALSE, entries=%d, nodeId=%d.",
                (HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
                cmiGetByControl((HCONTROL)ctrl), entries, numbersNodeId));
            nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
            app->cmMySessionEvent.cmEvCallMultiplexEntryResponse(
                (HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
                (HCALL)cmiGetByControl((HCONTROL)ctrl),
                RV_FALSE,
                entries,
                numbersNodeId);
            emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
            H245CBExit((app->pAppObject,"cmEvCallMultiplexEntryResponse."));
        }

        /*set bSaveCallIndication if control in ctrlMesFinished state*/
        if (ctrl->eState == ctrlMesFinished)
        {
            cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
        }
    }

    return RV_TRUE;
}




/*************************The MTSEI Functions**************************************/
    /* The MTSEKI side is the receiving entity, the side that gets the
       multiplexEntrySend message*/


/************************************************************************
 * multiplexEntrySend
 * purpose: Handle an incoming multiplexEntrySend message
 * input  : ctrl    - Control object
 *          message - MES message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int multiplexEntrySend(IN H245Control* ctrl, IN int message)
{
    HH245               hApp=cmiGetH245Handle((HAPP)emaGetInstance(
                            (EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HCALL               call;
    HPVT                hVal = ((H245Object *)hApp)->hVal;
    H245Object          *app = (H245Object*)hApp;
    IncomingMESInfo*    inMes= &ctrl->inMES;
    RvInt32             sq;
    RvInt32             nesting;
    RvPvtNodeId         descriptors;
    RvUint16            entries = 0;
    RvBool              tooComplex = RV_TRUE;
    RvStatus            status = RV_OK;

    /* get the parameters from the incoming MES */
    pvtGetChildByFieldId(hVal, message, __h245(sequenceNumber), &sq, NULL);
    inMes->seqNum = (RvUint8)sq;

    ctrl->eInMesStatus = MesStatusSent;
    emaMark((EMAElement)cmiGetByControl((HCONTROL)ctrl));

    /* Get the entries we're dealing with here */
    descriptors = pvtGetChild(hVal, message, __h245(multiplexEntryDescriptors), NULL);

    /* Notify internally that we have some work to do */
    call = (HCALL)cmiGetByControl((HCONTROL)ctrl);
    if (app->evHandlers.pfnH223MEStoDEMUXEv != NULL)
    {
        status = app->evHandlers.pfnH223MEStoDEMUXEv(call, app->pAppObject,
            descriptors, &entries, &tooComplex);
    }

    if (app->cmMySessionEvent.cmEvCallMultiplexEntry != NULL)
    {
        H245CBEnter((app->pAppObject,"cmEvCallMultiplexEntry: haCall=%p, hsCall=%p, nodeId=%d.",
            (HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
            cmiGetByControl((HCONTROL)ctrl), descriptors));
        nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
        app->cmMySessionEvent.cmEvCallMultiplexEntry(
            (HAPPCALL)emaGetApplicationHandle((EMAElement)call),
            call,
            descriptors);
        emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
        H245CBExit((app->pAppObject,"cmEvCallMultiplexEntry."));
    }

    if (!inMes->manualResponse)
    {
        /* Handle the response automatically */
        if (status == RV_OK)
        {
            /* Multiplex entries were processed successfully */
            RvPvtNodeId     nodeId;
            RvInt32         i;
            RvInt32         mask = 1;

            /* Acknowledge this message as is */
            nodeId = pvtAddRootByPath(hVal, app->hSynProtH245,
                "response.multiplexEntrySendAck.multiplexTableEntryNumber", 0, NULL);
            for (i = 0; i <= 15; i++)
            {
                if ((entries & (mask << i)) != 0)
                    pvtAdd(hVal, nodeId, -1, i + 1, NULL, NULL);
            }

            ctrl->inMES.manualResponse = RV_TRUE;
            status = cmCallMultiplexEntrySendResponse(call, RV_TRUE, nodeId);
            ctrl->inMES.manualResponse = RV_FALSE;

            ctrl->eInMesStatus = MesStatusAcknowledged;
        }
        else
        {
            /* We had trouble processing the multiplex entries - reject them... */
            ctrl->eInMesStatus = MesStatusRejected;

            /* todo: write it down... */
#if 0
            HPST hPst;
            RvPvtNodeId nodeId;

            hPst=pstConstruct(cmEmGetH245Syntax(),
                "MultiplexEntrySendReject.rejectionDescriptions");
            nodeId = pvtAddRoot(app->hVal,hPst, 0, NULL);
            pvtBuildByPath(app->hVal,nodeId,"1.multiplexTableEntryNumber",entryNum[0],NULL);
            if (IsComp)
                pvtBuildByPath(app->hVal,nodeId,"1.cause.descriptorTooComplex",0,NULL);
            else
                pvtBuildByPath(app->hVal,nodeId,"1.cause.unspecifiedCause",0,NULL);

            ctrl->inMES.manualResponse = RV_TRUE;
            cmCallMultiplexEntrySendResponse(h3G324mCall, RV_FALSE, nodeId);
            ctrl->inMES.manualResponse = RV_FALSE;
#endif
        }
    }

    /*set bSaveCallIndication if control in ctrlMesFinished state*/
    if (ctrl->eState == ctrlMesFinished)
    {
        cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
    }

    emaRelease((EMAElement)cmiGetByControl((HCONTROL)ctrl));
    return status;
}


/************************************************************************
 * multiplexEntrySendRelese
 * purpose: Handle an incoming multiplexEntrySendRelese message
 * input  : ctrl    - Control object
 *          message - MES message node
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int multiplexEntrySendRelese(IN H245Control* ctrl, IN int message)
{
    RV_UNUSED_ARG(message);

    ctrl->eInMesStatus = MesStatusReleased;
    return RV_TRUE;
}







/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/************************************************************************
 * mesTimeoutEventsHandler
 * purpose: Call back in case that no response was received after timeout has expired
 * input  : ctrl            - Control object
 * output : None.
 * return : None.
 ************************************************************************/
RvBool mesTimeoutEventsHandler(void*_ctrl)
{
    H245Control*     ctrl=(H245Control*)_ctrl;
    OutgoingMESInfo* outMes=&ctrl->outMES;
    RvStatus         status;
    int              message, nodeId;
    int              nesting;

    if (emaLock((EMAElement)cmiGetByControl((HCONTROL)ctrl)))
    {
        HH245            hApp=(HH245)(cmiGetH245Handle((HAPP)emaGetInstance(
                                (EMAElement)cmiGetByControl((HCONTROL)ctrl))));
        H245Object      *app = (H245Object*)hApp;
        HPVT hVal = ((H245Object *)hApp)->hVal;
        int i =0;
        app->evHandlers.pfnTimerClearEv((HCONTROL)ctrl, &outMes->timer);

        /* build the request message with the descriptors tree we got */
        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        __pvtBuildByFieldIds(nodeId,hVal,message,
            {_h245(indication) _h245(multiplexEntrySendRelease) _h245(multiplexTableEntryNumber)
            LAST_TOKEN}, 0, NULL);
        for (i = 0;i<15;i++)
        {
            if ((outMes->tableInd & (1<<i))!= 0)
                pvtAdd(hVal, nodeId, -1, i+1, NULL, NULL);
        }

        status = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, NULL);
        if (status == RV_OK &&
            app->cmMySessionEvent.cmEvCallMultiplexEntryResponse != NULL)
        {
            H245CBEnter((app->pAppObject,"cmEvCallMultiplexEntryResponse: haCall=%p, hsCall=%p, timeout.",
                (HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
                cmiGetByControl((HCONTROL)ctrl)));
            nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
            app->cmMySessionEvent.cmEvCallMultiplexEntryResponse(
                (HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
                (HCALL)cmiGetByControl((HCONTROL)ctrl),
                RV_FALSE,
                0,
                RV_PVT_INVALID_NODEID);
            emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
            H245CBExit((app->pAppObject,"cmEvCallMultiplexEntryResponse."));
        }

        /*set bSaveCallIndication if control in ctrlMesFinished state*/
        if (ctrl->eState == ctrlMesFinished)
        {
            cmiSetSaveCallIndication((HCONTROL)ctrl, RV_TRUE);
        }

        emaUnlock((EMAElement)cmiGetByControl((HCONTROL)ctrl));
    }

    return RV_FALSE;
}


/******************************************************************************
 * multiplexEntryGetNumbers
 * ----------------------------------------------------------------------------
 * General: Get a bitfield of the multiplex entries that are given in a
 *          nodeId of a message.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hVal             - PVT instance to use
 *         numbersNodeId    - NodeId to check
 *         use2levels       - RV_TRUE if you want to check an array of descriptors
 *                            RV_FALSE if dealing with a SEQUENCE OF mux-entry numbers
 * Output: entries          - Bitmask of the entries that are stated in the nodeId
 *****************************************************************************/
static RvStatus multiplexEntryGetNumbers(
    IN  HPVT        hVal,
    IN  RvPvtNodeId numbersNodeId,
    IN  RvBool      use2levels,
    OUT RvUint16    *entries)
{
    RvPvtNodeId entryNodeId, valueNodeId;
    RvInt32     entryNum;
    RvUint16    mask = 1;

    /* Erase the list */
    *entries = 0;

    /* Get the first one */
    entryNodeId = pvtChild(hVal, numbersNodeId);

    while (RV_PVT_NODEID_IS_VALID(entryNodeId))
    {
        /* We have a valid entry - look for the number */

        /* First, see if we have to look for a number if we're not pointing to one already */
        if (use2levels)
            valueNodeId = pvtGetChild(hVal, entryNodeId, __h245(multiplexTableEntryNumber), NULL);
        else
            valueNodeId = entryNodeId;

        /* Get the entry number */
        if (pvtGet(hVal, valueNodeId, NULL, NULL, &entryNum, NULL) >= 0)
        {
            /* Add it to the list of entries */
            *entries |= (RvUint16)(mask << (entryNum - 1));
        }

        /* Go to the next entry */
        entryNodeId = pvtBrother(hVal, entryNodeId);
    }

    return RV_OK;
}


#endif /*RV_H245_SUPPORT_H223_PARAMS == 1*/


#ifdef __cplusplus
}
#endif

