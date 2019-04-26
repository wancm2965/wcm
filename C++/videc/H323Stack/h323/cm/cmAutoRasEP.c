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

/************************************************************************
 * cmAutoRasEP
 * -----------
 * This file provides the endpoint part of the automatic RAS module.
 * This includes the following features:
 *
 * - Automatic discovery and registration to a gatekeeper
 * - Unregistration of a gatekeeper
 * - Lightweight RRQs (timeToLive feature)
 * - Sending NSM and RAI messages
 * - Automatic responses to IRQ messages
 * - Enabling manual RAS to work with automatic RAS for endpoint related
 *   transactions
 ************************************************************************/

#include "q931asn1.h"
#include "cmintr.h"
#include "cmras.h"
#include "cmCrossReference.h"
#include "cmAutoRasEP.h"
#include "rasutils.h"
#include "cmdebprn.h"
#include "cmDns.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Private functions declaration */
static RvBool autoRasSendLightweightRRQ(IN void* context);
static RvBool autoRasRetry(IN void* context);
static int autoRasPrepareRRQ(IN autorasEndpoint* autoras, IN int GkNodeId);
static int autoRasTryToRegister(IN autorasEndpoint* autoras);
static void autoRasIdleAndRegister(IN autorasEndpoint* autoras);
static RvBool autorasTryToRegisterOnError(IN void* context);
static int autoRasPrepareGRQ(IN autorasEndpoint* autoras);
static void autoRasRetryOnFail(IN autorasEndpoint* autoras);
static void RVCALLCONV gkIpReceivedCB(
    IN   HAPP                     hApp,
    IN   void*                    context,
    IN   RvStatus                 status,
    IN   RvUint16                 priotity,
    IN   RvUint16                 weight,
    IN   RvUint16                 numOfAddresses,
    IN   RvBool                   islastTx,
    OUT  RvBool*                  txContinue);
static void autoRasSetDestAddress(IN autorasEndpoint* autoras);
void RVCALLCONV autoRasSetPreGrantedARQParameters(
    IN HRAS             hsRas);



/************************************************************************
 *
 *                              Private functions
 *
 ************************************************************************/

/************************************************************************
 * autoRasEpSendToNextGK
 * purpose: Resend a request to the next GK
 * input  : autoras     - Automatic RAS EP module
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
static int autoRasEpSendToNextGK(IN autorasEndpoint * autoras)
{
    /* ok, so we already got everything in, except cmRASParamDestinationIpAddress
       and GK ID. let's go get'em! */
    int ret;
    RvPvtNodeId tmpNode, messageNode, GkNodeId;
    cmTransportAddress GKAddr;
    cmAlias GKID;
    char GKIDstr[128];
    RvInt32 needToRegister = 0;
    RvBool isLwRrq = RV_FALSE, isPartUrq = RV_FALSE, isUrq = RV_FALSE;

    /* check if we were already in search, or is this the first time */
    if (!autoRasIsInSearch(autoras, NULL) && !autoras->isTempRdrn)
    {
        /* notify the application that we started searching for a new GK */
        autoRasChangeState(autoras, cmSearchingAlternateGatekeeper, RV_PVT_INVALID_NODEID);
    }

    GkNodeId = autoRasGetNextAltGkNode(autoras, NULL);
    if (!RV_PVT_NODEID_IS_VALID(GkNodeId))
    {
        /* damn, no more GKs! we failed! */
        return RV_ERROR_UNKNOWN;
    }

    /* first, see if this is a URQ or an RRQ */
    messageNode = cmGetProperty((HPROTOCOL)autoras->registrationTx);
    __pvtGetNodeIdByFieldIds(tmpNode, autoras->hVal, messageNode,
        {_q931(request) _q931(registrationRequest) LAST_TOKEN});
    if (RV_PVT_NODEID_IS_VALID(tmpNode))
    {
        RvInt32 keepAlive = 0;
        tmpNode = pvtGetChildByFieldId(autoras->hVal, tmpNode, __q931(keepAlive), &keepAlive, NULL);
        if (RV_PVT_NODEID_IS_VALID(tmpNode) && keepAlive)
            isLwRrq = RV_TRUE;
    }
    else
    {
        __pvtGetNodeIdByFieldIds(tmpNode, autoras->hVal, messageNode,
            {_q931(request) _q931(unregistrationRequest) LAST_TOKEN});
        if (RV_PVT_NODEID_IS_VALID(tmpNode))
        {
            tmpNode = pvtGetChild(autoras->hVal, tmpNode, __q931(endpointAlias), NULL);
            if (RV_PVT_NODEID_IS_VALID(tmpNode))
                isPartUrq = RV_TRUE;
            else
                isUrq = RV_TRUE;
        }
    }

    /* Skip GKs with needToRegister=true for URQ*/
    tmpNode = pvtGetChildByFieldId(autoras->hVal, GkNodeId, __q931(needToRegister), &needToRegister, (RvBool *)NULL);
    while(isUrq && !autoras->isTempRdrn && (tmpNode>=0) && needToRegister)
    {
        GkNodeId = autoRasGetNextAltGkNode(autoras, NULL);
        if (GkNodeId < 0)
        {
            /* damn, no more GKs! we failed! */
            return RV_ERROR_UNKNOWN;
        }
        tmpNode = pvtGetChildByFieldId(autoras->hVal, GkNodeId, __q931(needToRegister), &needToRegister, (RvBool *)NULL);
    }

    /* OK, now we should have a GK, and know what we are sending */
    /* if we are sending a LWRRQ, register instead */
    if (isLwRrq && !autoras->isTempRdrn && (tmpNode>=0) && needToRegister)
    {
        return autoRasPrepareRRQ(autoras, GkNodeId);
    }
    /* if we are sending a PURQ, register first */
    if (isPartUrq && !autoras->isTempRdrn && (tmpNode>=0) && needToRegister)
    {
        /* ToDo: handle partial URQs when registration is required */
    }

    /* Get GK address */
    tmpNode = pvtGetChild(autoras->hVal, GkNodeId, __q931(rasAddress), NULL);
    if ((tmpNode<0) || (cmVtToTA(autoras->hVal, tmpNode, (cmTransportAddress *) &GKAddr)<0))
    {
        /* failed again ! */
        return RV_ERROR_UNKNOWN;
    }
    cmRASSetParam(autoras->registrationTx, cmRASTrStageRequest, cmRASParamDestinationIpAddress, 0, sizeof(GKAddr), (char *)&GKAddr);

    /* Get GK ID */
    GKID.string = GKIDstr;
    tmpNode = pvtGetChild(autoras->hVal, GkNodeId, __q931(gatekeeperIdentifier), NULL);
    if ((tmpNode>=0) &&
       ((GKID.length = (unsigned short) pvtGetString(autoras->hVal, tmpNode, 128, GKIDstr))>0))
    {
        cmRASSetParam(autoras->registrationTx, cmRASTrStageRequest, cmRASParamGatekeeperID, 0, sizeof(GKID), (char *)&GKID);
    }
    else
    {
        RvPvtNodeId nodeId = cmiRASGetRequest(autoras->registrationTx);
        __pvtGetNodeIdByFieldIds(nodeId, autoras->hVal, nodeId,
            {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN});
        pvtDelete(autoras->hVal, nodeId);
    }

    /* Send the request */
    ret = cmRASRequest(autoras->registrationTx);
    if (ret < 0)
    {
        RvMutexLock(&autoras->autorasLock, autoras->logMgr);
        cmRASClose(autoras->registrationTx);
        autoras->registrationTx = NULL;
        RvMutexUnlock(&autoras->autorasLock, autoras->logMgr);
    }

    return ret;
}

/************************************************************************
 * autoRasGatekeeperResponse
 * purpose: Callback function indicating the response of an GRQ
 * input  : haRas   - Application's handle (autoras in this context)
 *          hsRas   - RAS Transaction handle
 *          trStage - Stage of response
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
static int RVCALLCONV autoRasGatekeeperResponse(
    IN  HAPPRAS          haRas,
    IN  HRAS             hsRas,
    IN  cmRASTrStage     trStage)
{
    autorasEndpoint* autoras = (autorasEndpoint *)haRas;
    RvPvtNodeId nodeID = cmGetProperty((HPROTOCOL)hsRas);
    RvPvtNodeId responseId = cmiRASGetResponse(hsRas);

    /* See what we got */
    switch(trStage)
    {
        case cmRASTrStageConfirm:
            /* GCF - we should continue to RRQ */
            autoras->discoveryComplete = RV_TRUE;

            /* Change the state ant notify application */
            autoras->state = cmDiscovered;
            autoRasChangeState(autoras, cmGatekeeperConfirm, responseId);

            /* this function will set the new GK if needed, and get the new AltGKs list */
            setRasAlternateGatekeeper(autoras, NULL, hsRas, responseId, RV_TRUE);

            /* Go on to RRQ */
            autoRasPrepareRRQ(autoras, RV_ERROR_UNKNOWN);
            break;

        case cmRASTrStageReject:
        {
            /* GRJ - idle... */
            RvInt32 isMulticast = RV_FALSE;
            autoras->discoveryComplete = RV_TRUE;

            /* If we're in multicast - we don't continue... */
            cmRASGetParam(hsRas, cmRASTrStageRequest, cmRASParamMulticastTransaction, 0, &isMulticast, NULL);

            /* Change the state and notify application */
            autoras->state = cmIdle;
            autoRasChangeState(autoras, cmGatekeeperReject, cmiRASGetResponse(hsRas));

            if (!emaWasDeleted((EMAElement)hsRas) && !autoRasGotPermList(autoras))
            {
                /* getting the alternate gatekeepers from the message */
                __pvtGetNodeIdByFieldIds(nodeID, autoras->hVal, nodeID,
                    {_q931(response) _anyField _q931(altGKInfo) _q931(alternateGatekeeper) LAST_TOKEN});
                if (nodeID >= 0)
                { /* if there is AltGKs info in the message, put it in permAltGKs. NOTE: we put it
                    in permAltGK even if it is temporary, because we don't have anything else */
                    autoRasSetPermList(autoras, nodeID);
                }
            }

            if (isMulticast)
            {
                /* Ignore and wait for another reply */
                return 0;
            }

            /* Try all over again */
            autoRasTryToRegister(autoras);
            break;
        }

        case cmRASTrStageTimeout:
        {
            /* We timed out - try again */
            autoRasIdleAndRegister(autoras);
            break;
        }

        default:
            break;
    }

    RvMutexLock(&autoras->autorasLock, autoras->logMgr);
    if (hsRas == autoras->registrationTx)
    {
        /* no one removed this tx yet. kill it. */
        cmRASClose(hsRas);
        autoras->registrationTx = NULL;
    }
    RvMutexUnlock(&autoras->autorasLock, autoras->logMgr);
    return 0;
}


/************************************************************************
 * autoRasRegistrationResponse
 * purpose: Callback function indicating the response of an RRQ
 * input  : haRas   - Application's handle (autoras in this context)
 *          hsRas   - RAS Transaction handle
 *          trStage - Stage of response
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
static int RVCALLCONV autoRasRegistrationResponse(
 IN  HAPPRAS          haRas,
 IN  HRAS             hsRas,
 IN  cmRASTrStage     trStage)
{
    autorasEndpoint* autoras = (autorasEndpoint *)haRas;
    RvPvtNodeId nodeId;
    RvPvtNodeId responseId = cmiRASGetResponse(hsRas);
    RvPvtNodeId messageId;

    /* Skip the type of message and get to the message itself */
    messageId = pvtChild(autoras->hVal, responseId);

    /* See what we got */
    switch(trStage)
    {
    case cmRASTrStageConfirm:
        {
            /* RCF received - update information and we're done */
            RvBool createLightWeight = RV_FALSE;

            if (autoras->isFirstRRQ)
            {
                /* Remove the restart field for any future tries - we got registered! */
                cmRASSetParam(autoras->registrationTx, cmRASTrStageRequest, cmRASParamRestart, 0, RV_FALSE, NULL);
                autoras->isFirstRRQ = RV_FALSE;
            }

            if ((nodeId=pvtGetChild(autoras->hVal, messageId, __q931(timeToLive), NULL)) >= 0)
            {
                /* We got timeToLive - start counting for lightweight RRQs */
                RvInt32 timeToLive, timeout, preTimeToLiveRegistration=1;
                pvtGet(autoras->hVal, nodeId, NULL, NULL, &timeToLive, NULL);
                pvtGet(autoras->hVal, pvtGetChild(autoras->hVal, autoras->confNode, __q931(preTimeToLiveRegistration), NULL), NULL, NULL, &preTimeToLiveRegistration, NULL);

                if (timeToLive <= preTimeToLiveRegistration)
                    timeToLive = 1;
                else
                    timeToLive -= preTimeToLiveRegistration;

                timeout = timeToLive * 1000; /* Calculate the timeout in milliseconds */
                autoras->regTimer = RvH323TimerStart(autoras->hTimers, autoRasSendLightweightRRQ, autoras, timeout);
                createLightWeight = RV_TRUE;
            }

            /* this function will set the new GK if needed, and get the new AltGKs list */
            setRasAlternateGatekeeper(autoras, NULL, hsRas, responseId, RV_TRUE);

            /* Notify the application about the registration */
            autoRasSetPreGrantedARQParameters(hsRas);

            /* Change the state and notify the application */
            autoras->state = cmRegistered;
            autoras->internalState = autorasRegistered; /* We're registered! */

            /* Initialize the number of tries - when we fail, we should start from the beginning */
            autoras->regTries = 0;

            /* Notify the application about the change of state */
            autoRasChangeState(autoras, cmRegistrationConfirm, responseId);

            /* Make sure the lightweight RRQ won't be handled if the
               application tried to call cmRegister() or cmUnregister() from
               inside cmEvRegEvent(). */
            if (createLightWeight && (hsRas == autoras->registrationTx))
            {
                /* Create a Light Weight RRQ from current RRQ */
                RvPvtNodeId newRoot, regRequest, fromNode, toNode;
                int i;
                RvPvtNodeId requestId = cmiRASGetRequest(hsRas);
                /* array of fields to copy */
                short paths[] = {
                    /* fields from the H.225 standard */
                    _q931(rasAddress) _q931(endpointIdentifier) _q931(gatekeeperIdentifier) _q931(timeToLive)
                    /* from here are the non-optional fields */
                    _q931(discoveryComplete) _q931(protocolIdentifier) _q931(terminalType) _q931(endpointVendor)
                    _q931(willSupplyUUIEs) _q931(maintainConnection) _q931(supportsAssignedGK) LAST_TOKEN};
                short path[2] = {LAST_TOKEN, LAST_TOKEN};

                regRequest = pvtChild(autoras->hVal, requestId);

                /* move the current RRQ, and create a new one */
                newRoot = pvtAddRoot(autoras->hVal, ((cmElem*)(autoras->hApp))->synProtRAS, 0, NULL);
                pvtMoveTree(autoras->hVal, newRoot, regRequest);
                regRequest = pvtAdd(autoras->hVal, requestId, __q931(registrationRequest), 0, NULL, NULL);

                /* move the needed nodes into the LWRRQ */
                for (i=0; paths[i] != LAST_TOKEN; i++)
                {
                    path[0] = paths[i];
                    fromNode = pvtGetNodeIdByFieldIds(autoras->hVal, newRoot, path);
                    if (fromNode>=0)
                    {
                        toNode = pvtBuildByFieldIds(autoras->hVal, regRequest, path, 0, NULL);
                        pvtMoveTree(autoras->hVal, toNode, fromNode);
                    }
                }

                /* add the keepAlive value */
                pvtAdd(autoras->hVal, regRequest, __q931(keepAlive), RV_TRUE, NULL, NULL);

                /* build an empty callSignalAddress entry */
                pvtAdd(autoras->hVal, regRequest, __q931(callSignalAddress), 0, NULL, NULL);

                /* now copy the EP ID and the GK ID from the confirmation message */
                fromNode = pvtGetChild(autoras->hVal, messageId, __q931(gatekeeperIdentifier), NULL);
                if (RV_PVT_NODEID_IS_VALID(fromNode))
                {
                    toNode = pvtAdd(autoras->hVal, regRequest, __q931(gatekeeperIdentifier), 0, NULL, NULL);
                    pvtSetTree(autoras->hVal, toNode, autoras->hVal, fromNode);
                }
                fromNode = pvtGetChild(autoras->hVal, messageId, __q931(endpointIdentifier), NULL);
                if (RV_PVT_NODEID_IS_VALID(fromNode))
                {
                    toNode = pvtAdd(autoras->hVal, regRequest, __q931(endpointIdentifier), 0, NULL, NULL);
                    pvtSetTree(autoras->hVal, toNode, autoras->hVal, fromNode);
                }

                /* now delete the copied tree */
                pvtDelete(autoras->hVal, newRoot);
                return 0;
            }
            break;
        }

        case cmRASTrStageReject:
        {
            /* RRJ received */
            int chNodeId;
            autorasRegState oldState;

            /* check if the GK just needs a full registration */
            __pvtGetNodeIdByFieldIds(chNodeId, autoras->hVal, messageId, {_q931(rejectReason) _q931(fullRegistrationRequired) LAST_TOKEN});
            if (RV_PVT_NODEID_IS_VALID(chNodeId))
            {
                /* make sure this really is a light weight RRQ */
                RvPvtNodeId requestId = cmiRASGetRequest(hsRas);
                RvInt32 keepAlive = 0;
                __pvtGetByFieldIds(chNodeId, autoras->hVal, requestId, {_q931(registrationRequest) _q931(keepAlive) LAST_TOKEN}, NULL, &keepAlive, NULL);
                if (RV_PVT_NODEID_IS_VALID(chNodeId) && (keepAlive > 0))
                {
                    /* this really is a light weight RRQ. we assume the GK will keep us registered
                    long enough to complete the full registration, so we won't raise the unregistered
                    event, and just register again */
                    autoRasPrepareRRQ(autoras, RV_ERROR_UNKNOWN);
                    break;
                }
            }

            /* Change the state and notify the application */
            autoras->state = cmIdle;
            /* pretend we failed for now, to prevent URQ from being sent on cmUnregister */
            oldState = autoras->internalState;
            autoras->internalState = autorasRegFailed;
            autoRasChangeState(autoras, cmRegistrationReject, responseId);

            /* Make sure this transaction wasn't killed by the user */
            if (emaWasDeleted((EMAElement)hsRas))
                return 0;
            /* return to the old state */
            autoras->internalState = oldState;

            /* Check out the reason */
            __pvtGetNodeIdByFieldIds(chNodeId, autoras->hVal, messageId, {_q931(rejectReason) _q931(discoveryRequired) LAST_TOKEN});
            if (RV_PVT_NODEID_IS_VALID(chNodeId))
            {
                /* We have to try all over again, this time for GRQ */
                autoras->discoveryRequired = RV_TRUE;
                if (autoRasIsInSearch(autoras, NULL))
                {
                    /* we'll just send a GRQ to the current Alt GK - no need to go Idle */
                    autoRasPrepareGRQ(autoras);
                }
                else
                {
                    /* Idle and try again */
                    cmiRASUpdateRegInfo(cmiGetRasHandle(autoras->hApp), RV_TRUE);
                    autoRasIdleAndRegister(autoras);
                }
                return 0;
            }
            else
            {
                /* See if this is a redirection */
                chNodeId = pvtGetChild(autoras->hVal, messageId, __q931(altGKInfo), NULL);
                if (RV_PVT_NODEID_IS_VALID(chNodeId))
                {
                    /* Se we are redirected to an AltGK.*/
                    RvInt32 isPerm;
                    int newAltGKs;
                    /* maybe we are already working on a redirection */
                    if (!autoRasIsInSearch(autoras, NULL))
                    {
                        /* We're not working on anything right now  - we start redirection */
                        /* Let's see if permanently of temporarily */
                        newAltGKs = pvtGetChild(autoras->hVal, chNodeId, __q931(alternateGatekeeper), NULL);
                        pvtGetChildByFieldId(autoras->hVal, chNodeId, __q931(altGKisPermanent), &isPerm, NULL);
                        if (isPerm)
                            autoRasSetPermList(autoras, newAltGKs);
                        else
                        {
                            autoras->isTempRdrn = RV_TRUE;
                            autoRasSetTempList(autoras, newAltGKs);
                        }
                    }
                    /* ok, let's get a gatekeeper */
                    if (autoRasEpSendToNextGK(autoras) >= 0)
                        /* We have sent an RRQ to the next GK*/
                        return 0;
                }

                /* we got rejected (no AltGKs), or ran out of alternative GKs. get rid of the alternate
                   GK list, if we have one (which is a double good idea, since we failed to register anyway) */
                if (autoras->isTempRdrn)
                {
                    autoras->isTempRdrn = RV_FALSE;
                    autoRasRemoveTempList(autoras);
                }
                else autoRasRemovePermList(autoras);
                autoRasResetSearch(autoras, NULL);

                {
                    /* Timeout and wait a little before trying again */
                    RvInt32 timeout;
                    cmiRASUpdateRegInfo(cmiGetRasHandle(autoras->hApp), RV_TRUE);
                    pvtGet(autoras->hVal, pvtGetChild(autoras->hVal, autoras->confNode, __q931(responseTimeOut), NULL), NULL, NULL, &timeout, NULL);
                    timeout *= 1000;
                    autoras->regTimer = RvH323TimerStart(autoras->hTimers, autoRasRetry, autoras, timeout);
                }
            }
            break;
        }

        case cmRASTrStageTimeout:
            /* let's see if we have alternatives */
            if (autoRasIsInSearch(autoras, NULL) || autoRasGotPermList(autoras))
            {
                /* ok, let's get a gatekeeper */
                if (autoRasEpSendToNextGK(autoras) >= 0)
                    /* We have sent an RRQ to the next GK*/
                    return 0;
                else
                { /* no more GKs in AltGKs list - get rid of it */
                    if (autoras->isTempRdrn)
                    {
                        autoras->isTempRdrn = RV_FALSE;
                        autoRasRemoveTempList(autoras);
                    }
                    else autoRasRemovePermList(autoras);
                    autoRasResetSearch(autoras, NULL);
                }
            }
            else
            {
                cmiRASUpdateRegInfo(cmiGetRasHandle(autoras->hApp), RV_TRUE);
            }

            /* We timed out - try again */
            autoRasIdleAndRegister(autoras);
            return 0;

        default:
            break;
    }

    RvMutexLock(&autoras->autorasLock, autoras->logMgr);
    if (hsRas == autoras->registrationTx)
    {
        /* no one removed this tx yet. kill it. */
        cmRASClose(hsRas);
        autoras->registrationTx = NULL;
    }
    RvMutexUnlock(&autoras->autorasLock, autoras->logMgr);
    return 0;
}


/************************************************************************
 * autoRasUnregistrationResponse
 * purpose: Callback function indicating the response of a URQ
 * input  : haRas   - Application's handle (autoras in this context)
 *          hsRas   - RAS Transaction handle
 *          trStage - Stage of response
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
static int RVCALLCONV autoRasUnregistrationResponse(
    IN  HAPPRAS          haRas,
    IN  HRAS             hsRas,
    IN  cmRASTrStage     trStage)
{
    autorasEndpoint* autoras = (autorasEndpoint *)haRas;
    cmRegEvent event;
    RvPvtNodeId tmpNode;
    RvPvtNodeId requestId = cmiRASGetRequest(hsRas);
    RvPvtNodeId responseId = cmiRASGetResponse(hsRas);
    RvBool isPartial;

    /* See if this was a partial URQ */
    __pvtGetNodeIdByFieldIds(tmpNode, autoras->hVal, requestId, {_q931(unregistrationRequest) _q931(endpointAlias) LAST_TOKEN});
    isPartial = (tmpNode>=0);

    /* See what we got - the event will be set accordingly */
    switch(trStage)
    {
        case cmRASTrStageConfirm:
            event = cmUnregistrationConfirm;
            break;
        case cmRASTrStageReject:
            {
                /* See if this is a redirection */
                __pvtGetNodeIdByFieldIds(tmpNode, autoras->hVal, responseId, {_anyField _q931(altGKInfo) LAST_TOKEN});
                if (tmpNode >= 0)
                {
                    /* Se we are redirected to an AltGK.*/
                    RvInt32 isPerm;
                    int newAltGKs;
                    /* maybe we are already working on a redirection */
                    if (!autoRasIsInSearch(autoras, NULL))
                    {
                        /* We're not working on anything right now  - we start redirection */
                        /* Let's see if permanently of temporeraly */
                        newAltGKs = pvtGetChild(autoras->hVal, tmpNode, __q931(alternateGatekeeper), NULL);
                        pvtGetChildByFieldId(autoras->hVal, tmpNode, __q931(altGKisPermanent), &isPerm, NULL);
                        if (isPerm)
                            autoRasSetPermList(autoras, newAltGKs);
                        else
                        {
                            autoras->isTempRdrn = RV_TRUE;
                            autoRasSetTempList(autoras, newAltGKs);
                        }
                    }
                    /* ok, let's get a gatekeeper */
                    if (autoRasEpSendToNextGK(autoras) >= 0)
                        /* We have sent a URQ to the next GK*/
                        return 0;
                    else
                    { /* no more GKs in AltGKs list - get rid of it */
                        if (autoras->isTempRdrn)
                        {
                            autoras->isTempRdrn = RV_FALSE;
                            autoRasRemoveTempList(autoras);
                        }
                        else autoRasRemovePermList(autoras);
                        autoRasResetSearch(autoras, NULL);
                    }
                }

                event = cmUnregistrationReject;
                break;
            }
        default:
            /* maybe we are already working on a redirection */
            if (autoRasIsInSearch(autoras, NULL))
            {
                /* ok, let's get a gatekeeper */
                if (autoRasEpSendToNextGK(autoras) >= 0)
                    /* We have sent a URQ to the next GK*/
                    return 0;
                else
                { /* no more GKs in AltGKs list - get rid of it */
                    if (autoras->isTempRdrn)
                    {
                        autoras->isTempRdrn = RV_FALSE;
                        autoRasRemoveTempList(autoras);
                    }
                    else autoRasRemovePermList(autoras);
                    autoRasResetSearch(autoras, NULL);
                }
            }

            event = (cmRegEvent)RV_ERROR_UNKNOWN;
            break;
    }

    if (!isPartial)
    {
        /* Set the state to IDLE */
        autoras->state = cmIdle;
        autoras->internalState = autorasRegNotTried;
        /* we have been unregistered, remove the Alternate GK information, as it won't be worth much */
        autoRasRemoveTempList(autoras);
        autoRasRemovePermList(autoras);
        autoras->isTempRdrn = RV_FALSE;
        autoRasResetSearch(autoras, NULL);
    }

    autoRasChangeState(autoras, event, cmiRASGetResponse(hsRas));

    RvMutexLock(&autoras->autorasLock, autoras->logMgr);
    if (hsRas == autoras->registrationTx)
    {
        /* no one removed this tx yet. kill it. */
        cmRASClose(hsRas);
        autoras->registrationTx = NULL;
    }
    RvMutexUnlock(&autoras->autorasLock, autoras->logMgr);

    return 0;
}


/************************************************************************
 * autoRasRaiResponse
 * purpose: Callback function indicating the response of a RAI
 * input  : haRas   - Application's handle (autoras in this context)
 *          hsRas   - RAS Transaction handle
 *          trStage - Stage of response
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
static int RVCALLCONV autoRasRaiResponse(
    IN  HAPPRAS          haRas,
    IN  HRAS             hsRas,
    IN  cmRASTrStage     trStage)
{
    autorasEndpoint* autoras = (autorasEndpoint *)haRas;

    /* See what we got - the event will be set accordingly */
    switch(trStage)
    {
        case cmRASTrStageConfirm:
            autoRasChangeState(autoras, cmResourceAvailabilityConfirmation, cmiRASGetResponse(hsRas));
            break;

        case cmRASTrStageTimeout:
            autoRasChangeState(autoras, cmResourceAvailabilityConfirmation, RV_PVT_INVALID_NODEID);
            break;

        default:
            break;
    }

    /* Close the transaction and we're done */
    cmRASClose(hsRas);

    return 0;
}


/************************************************************************
 * autoRasSendLightweightRRQ
 * purpose: Callback function which sends an RRQ for keepalive messages
 * input  : context - autoras
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
static RvBool autoRasSendLightweightRRQ(IN void* context)
{
    autorasEndpoint* autoras = (autorasEndpoint *)context;
    int requestId, ret;
    RvInt32 keepAlive;

    /* Reset the timer - we don't need it until we get an RCF */
    RvH323TimerClear(autoras->hTimers, &autoras->regTimer);

    /*  maybe we already have a light weight RRQ */
    requestId = cmiRASGetRequest(autoras->registrationTx);
    __pvtGetByFieldIds(ret, autoras->hVal, requestId, {_q931(registrationRequest) _q931(keepAlive) LAST_TOKEN}, NULL, &keepAlive, NULL);
    if ((ret>=0) && keepAlive)
    {
        ret = cmRASRequest(autoras->registrationTx);
        if (ret < 0)
        {
            autoRasRetryOnFail(autoras);
        }
        return RV_FALSE;
    }

    RvMutexLock(&autoras->autorasLock, autoras->logMgr);
    /* Close a ras transaction if we have one */
    if (autoras->registrationTx != NULL)
    {
        cmRASClose(autoras->registrationTx);
        autoras->registrationTx = NULL;
    }
    RvMutexUnlock(&autoras->autorasLock, autoras->logMgr);

    /* Create an RRQ transaction */
    ret = cmRASStartTransaction(autoras->hApp, (HAPPRAS)autoras, &autoras->registrationTx, cmRASRegistration, NULL, NULL);

    if (ret >= 0)
    {
        /* Set the parameters to make sure it's a lightweight RRQ */
        cmRASSetParam(autoras->registrationTx, cmRASTrStageRequest, cmRASParamDiscoveryComplete, 0, (RvInt32)autoras->discoveryComplete, NULL);
        cmRASSetParam(autoras->registrationTx, cmRASTrStageRequest, cmRASParamKeepAlive, 0, RV_TRUE, NULL);
        autoras->discoveryComplete = RV_FALSE;

        cmiRASSetTrEventHandler(autoras->registrationTx, autoRasRegistrationResponse);

        /* Send the request */
        ret = cmRASRequest(autoras->registrationTx);
    }

    if (ret < 0)
    {
        autoRasRetryOnFail(autoras);
    }

    return RV_FALSE;
}


/************************************************************************
 * autoRasRetry
 * purpose: Retry callback. Called after a request transaction timedout
 *          and a specified time has passed.
 * input  : context - Context used (automatic RAS handle)
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
static RvBool autoRasRetry(IN void* context)
{
    autorasEndpoint* autoras = (autorasEndpoint *)context;

    /* Make sure there's a timer to reset */
    RvH323TimerClear(autoras->hTimers, &autoras->regTimer);

    /* Retry after setting to idle */
    autoRasIdleAndRegister(autoras);

    return RV_FALSE;
}


/************************************************************************
 * autoRasRetryOnFail
 * purpose: Retry callback. Called after a RAS request failed (usually
 *          when someone pulls the cable from the back of the computer)
 * input  : autoras - Automatic RAS instance to register
 * output : none
 * return : none
 ************************************************************************/
static void autoRasRetryOnFail(IN autorasEndpoint* autoras)
{
    /* We're not going to get any events on this transaction. We might as well
       just simulate waiting for a result and timing-out on that one to continue
       on trying */
    rasModule * rasMdl = (rasModule *)cmiGetRasHandle(autoras->hApp);
    int timeout;

    timeout = rasCfgGetTimeout(rasMdl) * rasCfgGetRetries(rasMdl) * 1000;

    RvH323TimerCancel(autoras->hTimers, &autoras->regTimer);
    autoras->regTimer = RvH323TimerStart(autoras->hTimers, autorasTryToRegisterOnError, autoras, timeout);
}


/************************************************************************
 * autoRasPrepareGRQ
 * purpose: Send a GRQ message to a gatekeeper trying to register it
 * input  : autoras - Automatic RAS instance to register
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
static int autoRasPrepareGRQ(IN autorasEndpoint* autoras)
{
    int ret;
    int GKnode = RV_ERROR_UNKNOWN;
    cmTransportAddress GKAddr;
    cmTransportAddress * GKAddrPtr = NULL;

    RvMutexLock(&autoras->autorasLock, autoras->logMgr);
    /* Make sure we have no pending transactions */
    if (autoras->registrationTx != NULL)
    {
        cmRASClose(autoras->registrationTx);
        autoras->registrationTx = NULL;
    }
    RvMutexUnlock(&autoras->autorasLock, autoras->logMgr);

    if (autoRasGotPermList(autoras))
    {
        GKnode = autoRasGetCurrentAltGkNode(autoras, NULL);
        if (GKnode < 0)
        { /* no more GKs in AltGKs list - get rid of it */
            autoRasRemovePermList(autoras);
            autoRasResetSearch(autoras, NULL);
        }
        else
        { /* Got a GK from the list - Get it's address */
            int GKAddrNode = pvtGetChild(autoras->hVal, GKnode, __q931(rasAddress), NULL);
            if ((GKAddrNode>=0) && (cmVtToTA(autoras->hVal, GKAddrNode, (cmTransportAddress *) &GKAddr)>=0))
                GKAddrPtr = &GKAddr;
        }
    }
    /* Create the transaction - default destination (=gk) or the one we found */
    ret = cmRASStartTransaction(autoras->hApp, (HAPPRAS)autoras, &autoras->registrationTx, cmRASGatekeeper, GKAddrPtr, NULL);

    if (ret >= 0)
    {
        if (GKnode < 0)
        { /* not using AltGK. send a multicast request */
            /* Set the multicast parameter to indicate this as a multicast message */
            cmRASSetParam(autoras->registrationTx, cmRASTrStageRequest, cmRASParamMulticastTransaction, 0, RV_TRUE, NULL);
        }
        else
        { /* using an AltGK */
            /* Set the Gatekeeper ID to the indicated one (if indicated) */
            cmAlias GKID;
            char GKIDstr[128];
            int GKIDnode = pvtGetChild(autoras->hVal, GKnode, __q931(gatekeeperIdentifier), NULL);
            GKID.string = GKIDstr;
            if ((GKIDnode>=0) &&
                ((GKID.length = (unsigned short) pvtGetString(autoras->hVal, GKIDnode, sizeof(GKIDstr), GKIDstr))>0))
            {
                cmRASSetParam(autoras->registrationTx, cmRASTrStageRequest, cmRASParamGatekeeperID, 0, sizeof(GKID), (char *) &GKID);
            }
            else
            {
                RvPvtNodeId nodeId = cmiRASGetRequest(autoras->registrationTx);
                __pvtGetNodeIdByFieldIds(nodeId, autoras->hVal, nodeId,
                    {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN});
                pvtDelete(autoras->hVal, nodeId);
            }
        }

        /* Set the response handler for this message */
        cmiRASSetTrEventHandler(autoras->registrationTx, autoRasGatekeeperResponse);

        /* Send the request */
        ret = cmRASRequest(autoras->registrationTx);
    }

    if (ret < 0)
    {
        autoRasRetryOnFail(autoras);
    }

    return ret;
}


/************************************************************************
 * autoRasPrepareRRQ
 * purpose: Send an RRQ message to a gatekeeper trying to register it
 * input  : autoras  - Automatic RAS instance to register
 *          GkNodeId - Node Id of Alt GK to try, RV_ERROR_UNKNOWN if none.
 *          ipAddrFromDnsIsValid - indication to take gk ip address from
 *          autorasEndpoint.
 * output : none
 * return : non-negative value on success
 *           negative value on failure
 ************************************************************************/
static int autoRasPrepareRRQ(IN autorasEndpoint* autoras,
                             IN int GkNodeId)
{
    int ret;
    cmTransportAddress GKAddr;
    cmTransportAddress * addrPtr = NULL;

    RvMutexLock(&autoras->autorasLock, autoras->logMgr);
    /* Make sure we have no pending transactions */
    if (autoras->registrationTx != NULL)
    {
        cmRASClose(autoras->registrationTx);
        autoras->registrationTx = NULL;
    }
    RvMutexUnlock(&autoras->autorasLock, autoras->logMgr);

    /* Check to see if this is a redirection */
    if (RV_PVT_NODEID_IS_VALID(GkNodeId))
    {
        /* Got a GK - Get it's address */
        int GKAddrNode = pvtGetChild(autoras->hVal, GkNodeId, __q931(rasAddress), NULL);
        if ((GKAddrNode>=0) && (cmVtToTA(autoras->hVal, GKAddrNode, (cmTransportAddress *) &GKAddr)>=0))
            addrPtr = &GKAddr;
    }

    /* Create the transaction - default destination (=gk) or the AltGK we found */
    ret = cmRASStartTransaction(autoras->hApp, (HAPPRAS)autoras, &autoras->registrationTx, cmRASRegistration, addrPtr, NULL);

    if (ret >= 0)
    {
        cmAlias GKID;
        char GKIDstr[128];
        GKID.string = GKIDstr;

        if (autoras->isFirstRRQ)
            cmRASSetParam(autoras->registrationTx, cmRASTrStageRequest, cmRASParamRestart, 0, RV_TRUE, NULL);

        /* Set the parameters of the message */
        cmRASSetParam(autoras->registrationTx, cmRASTrStageRequest, cmRASParamDiscoveryComplete, 0, (RvInt32)autoras->discoveryComplete, NULL);
        cmRASSetParam(autoras->registrationTx, cmRASTrStageRequest, cmRASParamKeepAlive, 0, 0, NULL);

        /* Check to see if this is a redirection */
        if (GkNodeId >=0)
        {
            /* Set the Gatekeeper ID to the indicated one (if indicated) */
            int GKIDnode = pvtGetChild(autoras->hVal, GkNodeId, __q931(gatekeeperIdentifier), NULL);
            if ((GKIDnode>=0) &&
               ((GKID.length = (unsigned short) pvtGetString(autoras->hVal, GKIDnode, sizeof(GKIDstr), GKIDstr))>0))
            {
                cmRASSetParam(autoras->registrationTx, cmRASTrStageRequest, cmRASParamGatekeeperID, 0, sizeof(GKID), (char *) &GKID);
            }
            else
            {
                RvPvtNodeId nodeId = cmiRASGetRequest(autoras->registrationTx);
                __pvtGetNodeIdByFieldIds(nodeId, autoras->hVal, nodeId,
                    {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN});
                pvtDelete(autoras->hVal, nodeId);
            }
        }

        /* Make sure next time we know nothing about a GCF */
        autoras->discoveryComplete = RV_FALSE;

        /* Set the response handler for this message */
        cmiRASSetTrEventHandler(autoras->registrationTx, autoRasRegistrationResponse);

        /* Send the request */
        ret = cmRASRequest(autoras->registrationTx);
    }

    if (ret < 0)
    {
        autoRasRetryOnFail(autoras);
    }

    return ret;
}


// TODO: New function needs explanation in header!
static RvPvtNodeId addAltGK(IN autorasEndpoint* autoras ,
                            IN RvPvtNodeId altGKRootNode ,
                            IN cmAlternateGatekeeper altGK)
{
    RvPvtNodeId tempId , altGKNode;
    int priority = altGK.priority;
    cmElem* app = (cmElem*)autoras->hApp;
    rasModule* ras = (rasModule*)app->rasManager;

// TODO: No need to check each function. It will just won't work if parameters are not fine...
    /*GK node*/
    if (!RV_PVT_NODEID_IS_VALID(altGKNode = pvtAdd(ras->hVal, altGKRootNode, RV_ERROR_UNKNOWN, 0, NULL, NULL)))
        return altGKNode;

    /*rasaddres*/
    if (!RV_PVT_NODEID_IS_VALID(tempId = pvtAdd(ras->hVal, altGKNode, __q931(rasAddress), 0, NULL, NULL)))
        return tempId;

    if (!RV_PVT_NODEID_IS_VALID(tempId = cmTAToVt(ras->hVal, tempId, &altGK.rasAddress)))
        return tempId;

    if (!RV_PVT_NODEID_IS_VALID(tempId = pvtAdd(ras->hVal, altGKNode, __q931(needToRegister), (RvInt32)altGK.needToRegister, NULL, NULL)))
        return tempId;

    tempId = pvtAdd(ras->hVal, altGKNode, __q931(priority), priority, NULL, NULL);

    return tempId;

}


/******************************************************************************
 * gkIpReceivedCB
 * ----------------------------------------------------------------------------
 * General: A callback of the autoras called after a dns query is completed.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp               - The stack's application handle.
 *         context            - The application context.
 *         status             - of the dns resolution transaction.
 *         priotity           - The priotity value received from srv query.
 *         weight             - The weight value received from srv query.
 *         numOfAddresses     - The number of addresses received for a target.
 *         islastTx           - if RV_TRUE the callback will not be called again
 *                              for the current request.
 * Output: txContinue         - if set by the application to RV_TRUE, the callback
 *                               will not be called again for the current request..
 *****************************************************************************/
static void RVCALLCONV gkIpReceivedCB(
    IN   HAPP                     hApp,
    IN   void*                    context,
    IN   RvStatus                 status,
    IN   RvUint16                 priotity,
    IN   RvUint16                 weight,
    IN   RvUint16                 numOfAddresses,
    IN   RvBool                   islastTx,
    OUT  RvBool*                  txContinue)
{
    autorasEndpoint* autoras = (autorasEndpoint*)context;
    cmElem* app = (cmElem*)hApp;
    rasModule* ras = (rasModule*)app->rasManager;
    cmAlternateGatekeeper currAltGK;

    RV_UNUSED_ARG(weight);

    if (status == RV_OK)
    {
        int i;
        RvChar addrStr[RV_TRANSPORT_ADDRESS_STRINGSIZE];


        if (!RV_PVT_NODEID_IS_VALID(autoras->altGKNodeIdForDns))
        {
            RvPvtNodeId nodeId;

            nodeId = pvtAddRoot(autoras->hVal, ras->synMessage, 0, NULL);
            pvtSetTree(autoras->hVal , nodeId , ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageConfirm]);
            __pvtGetByFieldIds(nodeId, autoras->hVal, nodeId, {_q931(gatekeeperConfirm) LAST_TOKEN}, NULL, NULL, NULL);
            autoras->altGKNodeIdForDns = pvtAdd(autoras->hVal, nodeId, __q931(alternateGatekeeper),0, NULL, NULL);

            if (!RV_PVT_NODEID_IS_VALID(autoras->altGKNodeIdForDns))
            {
                RvLogError(&app->log, (&app->log, "Unable to Build Alternate Gatekeeper Tree"));
                *txContinue = RV_FALSE;
                return;
            }

        }

        currAltGK.priority = (RvUint8)priotity;
        currAltGK.needToRegister = RV_TRUE;

        for (i = 0 ; i < numOfAddresses ; ++i)
        {
            cmTransportAddress2String(&autoras->gkIpAddressesFromDns[i], RV_TRANSPORT_ADDRESS_STRINGSIZE, addrStr);
            /*add to pvt*/
            currAltGK.rasAddress = autoras->gkIpAddressesFromDns[i];
            /*add to tree*/
            if (RV_PVT_NODEID_IS_VALID(addAltGK(autoras , autoras->altGKNodeIdForDns , currAltGK)))
            {
                autoras->totalNumAddresses++;
                RvLogInfo(&app->log, (&app->log, "GK Address: %s Added to List",addrStr));
            }
            else
            {
                RvLogError(&app->log, (&app->log, "Unable to add GK Address: %s to List",addrStr));
            }
        }
    }
    else
    {
        RvLogInfo(&app->log, (&app->log, "Gatekeeper DNS Query returned error status %d",status));
    }

    /*if the last time call back is called for this query*/
    if (islastTx)
    {
        if (!autoras->totalNumAddresses)
        {
            RvLogError(&app->log, (&app->log, "No Gatekeeper was Found" ));
        }

        autoRasSetPermList(autoras, autoras->altGKNodeIdForDns);
        /*remove tree*/
        pvtDelete(autoras->hVal , autoras->altGKNodeIdForDns);
        autoras->altGKNodeIdForDns = RV_PVT_INVALID_NODEID;
        autoras->totalNumAddresses = 0;

        /*try to register through alternate gatekeepers*/
        if (autoRasGotPermList(autoras))
        {
             autoRasPrepareRRQ(autoras, autoRasGetNextAltGkNode(autoras, NULL));
            /* take back that last try */
            autoras->regTries--;
        }
    }
}


/******************************************************************************
 * autoRasSetDestAddress
 * ----------------------------------------------------------------------------
 * General: Sets the destination address in the rasOutTx to the current
 *          alternate GK address.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  autoras            - The auto ras module.
 * Output: None.
 *****************************************************************************/
static void autoRasSetDestAddress(IN autorasEndpoint* autoras)
{
    rasOutTx*   tx = rasGetOutgoing(autoras->registrationTx);
    RvPvtNodeId gkNodeId = autoRasGetCurrentAltGkNode(autoras, NULL), gkAddrNode;

    if (RV_PVT_NODEID_IS_VALID(gkNodeId))
    {
        /* Got a GK - Get it's address */
        gkAddrNode = pvtGetChild(autoras->hVal, gkNodeId, __q931(rasAddress), NULL);
        cmVtToTA(autoras->hVal, gkAddrNode, &(tx->destAddress));
    }
}


/************************************************************************
 * autoRasTryToRegister
 * purpose: Start registration process of the endpoint
 * input  : autoras - Automatic RAS instance to register
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
static int autoRasTryToRegister(IN autorasEndpoint* autoras)
{
    RvInt32 maxFail;
    int status;

    RvMutexLock(&autoras->autorasLock, autoras->logMgr);

    if (autoras->internalState != autorasRegFailed)
        autoras->internalState = autorasRegTrying;
    autoras->regTries++; /* another try... */

    /* Make sure we reset the timer if we have any */
    RvH323TimerCancel(autoras->hTimers, &autoras->regTimer);

    /* Make sure we have no pending transactions */
    if (autoras->registrationTx != NULL)
    {
        cmRASClose(autoras->registrationTx);
        autoras->registrationTx = NULL;
    }

    /* See if we're done trying */
    if ((pvtGet(autoras->hVal, pvtGetChild(autoras->hVal, autoras->confNode, __q931(maxFail), NULL), NULL, NULL, &maxFail, NULL) < 0) || (maxFail == 0))
        maxFail = 1;
    if ((autoras->regTries > maxFail) && (autoras->internalState != autorasRegFailed))
    {
        autoras->internalState = autorasRegFailed;
        /* We continue on trying to register although we failed - here, we just allow calls
           to be created */

        /* Notify the application that we're not registered although the user wanted to */
        status = autoRasChangeState(autoras, cmRegistrationConfirm, RV_PVT_INVALID_NODEID);

        /* In case someone called cmUnregister or cmRegister in the callback, do not send another RRQ */
        if ( (autoras->internalState == autorasRegNotTried) || (autoras->internalState == autorasRegTrying) )
        {
            RvMutexUnlock(&autoras->autorasLock, autoras->logMgr);
            return 0;
        }
    }
    RvMutexUnlock(&autoras->autorasLock, autoras->logMgr);

    /* See if we're in GRQ or RRQ */
    __pvtGetByFieldIds(status, autoras->hVal, autoras->confNode, {_q931(manualDiscovery) _q931(defaultGatekeeper) LAST_TOKEN}, NULL, NULL, NULL)
    if (RV_PVT_NODEID_IS_VALID(status) && !autoras->discoveryRequired)
    {
        /* We have a GK address that didn't reject us yet - start from RRQ */
        return autoRasPrepareRRQ(autoras, RV_ERROR_UNKNOWN);
    }
    else if ((status < 0) && !autoras->discoveryRequired)
    {
        /* check if we have a name and not ip address - if so get DNS address resolution */
        cmElem* app = (cmElem*)autoras->hApp;

        /* see if we have a name and DNS is supported */
        __pvtGetByFieldIds(status, autoras->hVal, autoras->confNode, {_q931(manualDiscovery) _q931(defaultGatekeeperName) LAST_TOKEN}, NULL, NULL, NULL)
        if (RV_PVT_NODEID_IS_VALID(status) && app->dnsManager != NULL)
        {
            RvPvtNodeId tempNodeId;
            RvPstFieldId fieldId = 1;
            RvChar dnsName[MAX_DOMAIN_LEN];
            RvInt32 dnsPort = 0;
            cmDnsQueryType queryType = cmDnsQueryTypeIpv4; /* default */

            memset(dnsName, 0, sizeof(dnsName));
            /* get the name */
            tempNodeId = pvtGetChild(autoras->hVal, status , __q931(gkName) , NULL);
            pvtGetString(autoras->hVal, tempNodeId, sizeof(dnsName), dnsName);

            __pvtGetByFieldIds(tempNodeId, autoras->hVal, status, {_q931(gkPort) LAST_TOKEN}, NULL, &dnsPort, NULL)

            tempNodeId = pvtGetChild(autoras->hVal, status , __q931(queryType) , NULL);

            tempNodeId = pvtChild(autoras->hVal, tempNodeId);
            pvtGet(autoras->hVal, tempNodeId, &fieldId, NULL, NULL, NULL);

            switch(fieldId)
            {
            case __q931(dnsTypeIpv4):
                queryType = cmDnsQueryTypeIpv4;
                break;
            case __q931(dnsTypeIpv6):
                queryType = cmDnsQueryTypeIpv6;
                break;
            case __q931(dnsTypeSrvIpv4):
                queryType = cmDnsQueryTypeSrvIpv4;
                break;
            case __q931(dnsTypeSrvIpv6):
                queryType = cmDnsQueryTypeSrvIpv6;
                break;
            default:
                RvLogError(&app->logConfig,(&app->logConfig,"Unknown query type: %d", fieldId));
                break;
            }

            /* send DNS request */
            return cmDnsAddressRequest(autoras->hApp,
                                       (void*)autoras,
                                       queryType,
                                       dnsName,
                                       (RvUint16)dnsPort,
                                       gkIpReceivedCB,
                                       autoras->gkIpAddressesFromDns,
                                       (RvUint16)MAX_GK_DNS_IP);
        }
    }

    /* We don't have any GK address or name, or were rejected by GK - start from GRQ */
    if (autoRasGotPermList(autoras))
    {
        /* We are using Alt GKs - be sure to try the next one */
        autoRasGetNextAltGkNode(autoras, NULL);
        /* take back that last try */
        autoras->regTries--;
    }
    return autoRasPrepareGRQ(autoras);
}


/************************************************************************
 * autorasTryToRegisterOnError
 * purpose: Timeout function after simulating a timeout on a transaction
 *          that we couldn't get started properly.
 * input  : context - Automatic RAS handle
 * output : none
 * return : none
 ************************************************************************/
static RvBool autorasTryToRegisterOnError(IN void* context)
{
    autorasEndpoint *autoras = (autorasEndpoint *)context;

    /* Reset the timer if we have any */
    RvH323TimerClear(autoras->hTimers, &autoras->regTimer);

    /* Try to register all over again */
    autoRasIdleAndRegister(autoras);

    return RV_FALSE;
}


/************************************************************************
 * autoRasIdleAndRegister
 * purpose: Change the state of the Automatic RAS endpoint handling to
 *          idle and retry registration
 * input  : autoras - Automatic RAS handle
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
static void autoRasIdleAndRegister(IN autorasEndpoint* autoras)
{
    /* Change the state back to Idle if we have to */
    if (autoras->state != cmIdle)
    {
        autoras->state = cmIdle;
        autoras->internalState = autorasRegTrying;
        autoRasChangeState(autoras, (cmRegEvent)-1, RV_PVT_INVALID_NODEID);
        if (autoras->internalState == autorasRegNotTried)
            /* someone decided to unregister in the above event */
            return;
    }

    /* Try to register again */
    autoRasTryToRegister(autoras);
}


/************************************************************************
 * autoRasEPTransaction
 * purpose: Callback function that handles incoming endpoint-related
 *          transactions
 * input  : hApp        - Stack instance handle
 *          hsRas       - RAS transaction handle
 *          transaction - Transaction type of incoming message
 *          srcAddress  - Address of the message's origin
 *          notifyCb    - Callback for application about the request
 * output : none
 * return : RV_TRUE if message was processed by this callback and shouldn't
 *          be processed by the manual RAS callbacks
 *          RV_FALSE if message wasn't processed by this callback
 ************************************************************************/
static RvBool RVCALLCONV autoRasEPTransaction(
    IN  HAPP                hApp,
    IN  HRAS                hsRas,
    IN  cmRASTransaction    transaction,
    IN  cmTransportAddress  *srcAddress,
    IN  cmEvAutoRASRequestT notifyCb)
{
    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(srcAddress);

    /* Tell the application about this request... */
    if (notifyCb != NULL)
    {
        switch (transaction)
        {
        case cmRASUnregistration:
        case cmRASNonStandard:
        case cmRASInfo:
            cmiCBEnter((hApp, "cmEvAutoRASRequest(hsRas=%p,hsCall=0x0,trans=%d)", hsRas, transaction));
            notifyCb(hsRas, NULL, transaction, srcAddress, NULL);
            cmiCBExit((hApp, "cmEvAutoRASRequest(hsRas=%p)", hsRas));
            break;
        default:
            /* Not handled by Automatic RAS... */
            break;

        }
    }

    /* Check the type of transaction we're dealing with */
    switch (transaction)
    {
    case cmRASUnregistration:
        {
            /* URQ message - always send back a UCF */
            autorasEndpoint* autoras = (autorasEndpoint *)cmiGetAutoRasHandle(cmiRASGetHAPP(hsRas));
            int requestId = cmiRASGetRequest(hsRas);
            int nodeId;
            int newAltGKs = RV_ERROR_UNKNOWN;

            /* First - let's move to idle */
            autoras->state = cmIdle;
            autoRasChangeState(autoras, cmUnregistrationRequest, requestId);

            /* now, let's confirm it */
            cmRASConfirm(hsRas);

            /* Now see if we were redirected */
            __pvtGetNodeIdByFieldIds(newAltGKs, autoras->hVal, requestId, {_anyField _q931(alternateGatekeeper) LAST_TOKEN});
            if (newAltGKs >= 0)
            {
                /* Se we are redirected to an AltGK.*/
                /* maybe we are already working on a redirection */
                if (autoRasIsInSearch(autoras,NULL))
                {
                    /* We're working on a redirection, but we'd like to reset it */
                    autoRasSetPermList(autoras, newAltGKs);
                    autoRasResetSearch(autoras, NULL);
                    autoras->isTempRdrn = RV_FALSE;
                    autoRasRemoveTempList(autoras);
                    cmRASClose(hsRas);
                    RvMutexLock(&autoras->autorasLock, autoras->logMgr);
                    if (autoras->registrationTx)
                    {
                        cmRASClose(autoras->registrationTx);
                        autoras->registrationTx = NULL;
                    }
                    RvMutexUnlock(&autoras->autorasLock, autoras->logMgr);
                }
                else
                {
                    autoRasSetPermList(autoras, newAltGKs);
                    autoRasRemoveTempList(autoras);
                }
                /* we're starting redirection: let's get a gatekeeper */
                if ((newAltGKs = autoRasGetNextAltGkNode(autoras, NULL)) >= 0)
                { /* We have an Alt GK */
                    /* Clean this */
                    cmRASClose(hsRas);
                    /* And now we send an RRQ to the next GK*/
                    if (autoRasPrepareRRQ(autoras, newAltGKs) >= 0)
                        return RV_TRUE;
                }
                else
                { /* no more GKs in AltGKs list - get rid of it */
                    autoRasRemovePermList(autoras);
                    autoRasResetSearch(autoras, NULL);
                }
            }

            /* or maybe this was just a partial URQ */
            __pvtGetNodeIdByFieldIds(nodeId, autoras->hVal, requestId, {_q931(unregistrationRequest) _q931(endpointAlias) LAST_TOKEN});
            if (nodeId >= 0)
            {
                /* just remove these aliases */
                rasUpdatePartUnreg((rasModule *)emaGetUserData((EMAElement)hsRas), requestId);
                cmRASClose(hsRas);
                break;
            }

            cmRASClose(hsRas);

            /* We're unregistered - we might try to register again :-) */
            if (autoras->internalState == autorasRegNotTried)
                /* someone decided to unregister in the "unregistered" event above */
                break;
            /* ToDo: remove next - done in manual RAS */
            cmiRASUpdateRegInfo(cmiGetRasHandle(autoras->hApp), RV_TRUE);
            autoRasIdleAndRegister(autoras);
            break;
        }

        case cmRASNonStandard:
        {
            /* NSM message - notify the state to application and close it */
            autorasEndpoint* autoras;
            autoras = (autorasEndpoint *)cmiGetAutoRasHandle(cmiRASGetHAPP(hsRas));
            autoRasChangeState(autoras, cmNonStandardMessage, cmiRASGetResponse(hsRas));
            cmRASClose(hsRas);
            break;
        }

        case cmRASInfo:
        {
            RvInt32 crv;

            /* see if the CRV is really zero */
            cmRASGetParam(hsRas, cmRASTrStageRequest, cmRASParamCRV, 0, &crv, NULL);
            if (crv != 0)
            {
                /* we are asked about a call we do not know. set the invalid call param */
                cmRASSetParam(hsRas, cmRASTrStageConfirm, cmRASParamIrrStatus, 0, cmRASIrrInvalidCall, NULL);
            }
            /* IRQ message - send back an IRR and close the transaction */
            cmRASConfirm(hsRas);
            cmRASClose(hsRas);
            break;
        }

        default:
            /* Not processed by automatic RAS */
            return RV_FALSE;
    }

    /* Make sure manual RAS knowns we have processed this mesage */
    return RV_TRUE;
}




/************************************************************************
 *
 *                              Public functions
 *
 ************************************************************************/


/************************************************************************
 * cmiAutoRASEPStart
 * purpose: Start the endpoint part of the automatic RAS.
 *          This function sets internal callbacks with the RAS module and
 *          initializes some autoRAS related variables.
 * input  : hAutoRas - Automatic RAS instance
 * output : none
 * return : none
 ************************************************************************/
void cmiAutoRASEPStart(IN HAUTORASMGR  hAutoRas)
{
    autorasEndpoint* autoras = (autorasEndpoint *)hAutoRas;

    autoras->isFirstRRQ = RV_TRUE;

    /* Set the event handler for the endpoint related RAS transactions */
    cmiRASSetEPTrEventHandler(autoras->hApp, autoRasEPTransaction);
}



/************************************************************************
 * cmiAutoRASGetEndpointID
 * purpose: Retrieves the EndpointID stored in the ras
 *
 * input  : hApp    - Application's stack handle
 *          eId     - pointer to the buffer for endpoint ID
 *                    buffer should be big enough for longest EID
 *                    possible (256 byte)
 * output : none
 * return : The length of EID in bytes on success
 *          Negative value on failure
 ************************************************************************/
int cmiAutoRASGetEndpointID(
    IN  HAPP    hApp,
    IN  void*   eId)
{
    HRASMGR ras=cmiGetRasHandle(hApp);
    return cmiRASGetEndpointID(ras, eId);
}


/************************************************************************
 * autoRasChangeState
 * purpose: Notify the application about the state of the registration
 * input  : autoras     - Automatic RAS instance
 *          regEvent    - Registration event being notified
 *                        can be a negative value if event is not caused
 *                        by any message but by a timeout
 *          message     - Message that caused the event, or node ID of
 *                        the new permanent alternate GK in such an event.
 *                        Negative value if not applicable
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
int autoRasChangeState(
    IN  autorasEndpoint*    autoRas,
    IN  cmRegEvent          regEvent,
    IN  RvPvtNodeId         message)
{
    /* todo: do not use the cmElem data type */
    cmEvRegEventT   evFunc;

    /* Notify the application through the callback */
    evFunc = ((cmElem *)autoRas->hApp)->cmMyEvent.cmEvRegEvent;
    if (evFunc != NULL)
    {
        int status;
        if ((RvInt32)regEvent < 0)
            message = RV_PVT_INVALID_NODEID;

#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
        {
            static const RvChar *autorasStates[] = {"cmIdle", "cmDiscovered", "cmRegistered"};
            static const RvChar *autorasEvents[] =
            {
                "",
                "cmGatekeeperConfirm",
                "cmGatekeeperReject",
                "cmRegistrationConfirm",
                "cmRegistrationReject",
                "cmUnregistrationRequest",
                "cmUnregistrationConfirm",
                "cmUnregistrationReject",
                "cmNonStandardMessage",
                "cmResourceAvailabilityConfirmation",
                "cmPermanentAlternateGatekeeperConfirmation",
                "cmSearchingAlternateGatekeeper"
            };
            cmiCBEnter((autoRas->hApp,
                       "cmEvRegEvent(hApp=%p, regState=%s, regEvent=%s, regEventHandle=%d)",
                       autoRas->hApp,
                       autorasStates[autoRas->state],
                       autorasEvents[(int)regEvent+1],
                       message));
        }
#endif
        status = evFunc(autoRas->hApp, autoRas->state, regEvent, message);
        cmiCBExit((autoRas->hApp, "cmEvRegEvent=%d", status));
    }

    return 0;
}



/************************************************************************
 *
 *                          Public API functions
 *
 ************************************************************************/

/************************************************************************
 * cmRegister
 * purpose: Registers the endpoint with the gatekeeper.
 *          It is only applicable when the RAS.manualRAS key is not
 *          defined in the configuration (automatic RAS mode).
 *          It reads registration information from RAS.registrationInfo
 *          configuration key.
 * input  : hApp    - Application's handle of the stack
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRegister(
        IN  HAPP        hApp)
{
    autorasEndpoint* autoras = (autorasEndpoint *)cmiGetAutoRasHandle(hApp);
    int status;

    if (!hApp || !autoras) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmRegister(hApp=%p)", hApp));

    RvMutexLock(&autoras->autorasLock, autoras->logMgr);

    autoras->regTries = 0;
    autoras->discoveryRequired = RV_FALSE;

    /* reset DNS related fields */
    pvtDelete(autoras->hVal , autoras->altGKNodeIdForDns);
    autoras->altGKNodeIdForDns = RV_PVT_INVALID_NODEID;
    autoras->totalNumAddresses = 0;

    autoRasRemoveTempList(autoras);
    autoras->isTempRdrn = RV_FALSE;
    autoRasResetSearch(autoras, NULL);

    cmiRASUpdateRegInfo(cmiGetRasHandle(hApp), cmiAutoRASGetRegState(hApp) != cmRegistered);
    autoras->internalState = autorasRegTrying;
    status = autoRasTryToRegister(autoras);

    if (status < 0)
    {
        /* Make sure the timer is not started... */
        RvH323TimerCancel(autoras->hTimers, &autoras->regTimer);
    }

    RvMutexUnlock(&autoras->autorasLock, autoras->logMgr);
    cmiAPIExit((hApp, "cmRegister=%d", status));
    return status;
}


/************************************************************************
 * cmUnregister
 * purpose: Unregisters the endpoint from the Gatekeeper
 *          It is only applicable when the RAS.manualRAS key is not
 *          defined in the configuration (automatic RAS mode).
 *          It reads registration information from RAS.registrationInfo
 *          configuration key.
 * input  : hApp    - Application's handle of the stack
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmUnregister(
        IN  HAPP        hApp)
{
    autorasEndpoint* autoras = (autorasEndpoint *)cmiGetAutoRasHandle(hApp);
    int status = 0;

    if (!hApp || !autoras) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmUnregister(hApp=%p)", hApp));

    /* Stop the registration timer if we have one */
    RvH323TimerCancel(autoras->hTimers, &autoras->regTimer);

    RvMutexLock(&autoras->autorasLock, autoras->logMgr);
    /* Make sure we have no pending transactions */
    if (autoras->registrationTx != NULL)
    {
        cmRASClose(autoras->registrationTx);
        autoras->registrationTx = NULL;
    }
    RvMutexUnlock(&autoras->autorasLock, autoras->logMgr);

    /* If we're registered or currently trying to register - we should unregister... */
    if ((autoras->internalState == autorasRegTrying) ||
        ((autoras->state == cmRegistered) && (autoras->internalState == autorasRegistered)))
    {
        status = cmRASStartTransaction(autoras->hApp, (HAPPRAS)autoras, &autoras->registrationTx, cmRASUnregistration, NULL, NULL);

        /*if there is no dest address and we're in search for a permanent GK, set the dest address to
        the last address RRQ was sent*/
        if (autoRasIsInSearch(autoras, NULL) && autoRasGotPermList(autoras))
        {
            autoRasSetDestAddress(autoras);
        }

        if (status >= 0)
        {
            cmiRASSetTrEventHandler(autoras->registrationTx, autoRasUnregistrationResponse);
            status = cmRASRequest(autoras->registrationTx);
        }
    }
    /* unless we're definitely registered, stop trying to register */
    if (autoras->state != cmRegistered)
    {
        /* reset the state, unless we're registered - if we are, we'll leave that to the UCF */
        autoras->internalState = autorasRegNotTried;
        autoras->regTries = 0;
    }

    cmiAPIExit((hApp, "cmUnregister=%d", status));
    return status;

}


/************************************************************************
 * cmSendNonStandardMessage
 * purpose: Sends a nonstandard message to the Gatekeeper
 *          It is only applicable when the RAS.manualRAS key is not
 *          defined in the configuration (automatic RAS mode).
 *          It reads registration information from RAS.registrationInfo
 *          configuration key.
 * input  : hApp    - Application's handle of the stack
 *          nsParam - Nonstandard parameter to be used in non standard message
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmSendNonStandardMessage(
     IN      HAPP                hApp,
     IN      cmNonStandardParam* nsParam)
{
    autorasEndpoint* autoras = (autorasEndpoint *)cmiGetAutoRasHandle(hApp);
    int status;
    HRAS hRas;

    if (!hApp || !autoras) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmSendNonStandardMessage(hApp=%p)", hApp));

    /* Start the transaction and set the non standard information */
    status = cmRASStartTransaction(autoras->hApp, NULL, &hRas, cmRASNonStandard, NULL, NULL);

    if (status >= 0)
    {
        status = cmRASSetParam(hRas, cmRASTrStageRequest, cmRASParamNonStandard, 0, 0, (char*)nsParam);

        if (status >= 0)
        {
            /* Send the request */
            status = cmRASRequest(hRas);
        }

        /* Close the transaction - it's not waiting for any reply */
        cmRASClose(hRas);
    }


    cmiAPIExit((hApp, "cmSendNonStandardMessage=%d", status));
    return status;
}


/************************************************************************
 * cmSendRAI
 * purpose: Sends a RAI message to the gatekeeper.
 *          It is only applicable when the RAS.manualRAS key is not
 *          defined in the configuration (automatic RAS mode).
 *          It reads registration information from RAS.registrationInfo
 *          configuration key.
 * input  : hApp                    - Application's handle of the stack
 *          almoustOutOfResources   - Indicates that it is or is not almost
 *                                    out of resources
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI int RVCALLCONV
cmSendRAI(
       IN      HAPP             hApp,
       IN      RvBool           almostOutOfResources)
{
    autorasEndpoint* autoras = (autorasEndpoint *)cmiGetAutoRasHandle(hApp);
    int status;

    if (!hApp || !autoras) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmSendRAI(hApp=%p, almostOutOfResources=%d)", hApp, almostOutOfResources));

    /* Make sure we're already registered */
    if (autoras->state == cmRegistered)
    {
        HRAS hRas;

        /* Send a RAI transaction */
        status = cmRASStartTransaction(autoras->hApp, (HAPPRAS)autoras, &hRas, cmRASResourceAvailability, NULL, NULL);
        if (status >= 0)
        {
            cmRASSetParam(hRas, cmRASTrStageRequest, cmRASParamAlmostOutOfResources, 0, (RvInt32)almostOutOfResources, NULL);
            cmiRASSetTrEventHandler(hRas, autoRasRaiResponse);
            status = cmRASRequest(hRas);
            /* solve resource leak in case request was not sent */
			if (status != RV_OK)
			    cmRASClose(hRas);
        }
    }
    else
        status = RV_ERROR_UNKNOWN;

    cmiAPIExit((hApp, "cmSendRAI=%d", status));
    return status;
}


/************************************************************************
 * cmiAutoRASGetRegState
 * purpose: returns the registration state
 * input  : hApp - Application's handle of the stack
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns the registration state.
 ************************************************************************/
cmRegState cmiAutoRASGetRegState(IN HAPP hApp)
{
    autorasEndpoint* autoras = (autorasEndpoint *)cmiGetAutoRasHandle(hApp);

    if (!hApp || !autoras) return (cmRegState)RV_ERROR_UNKNOWN;

    return autoras->state;
}




#ifdef __cplusplus
}
#endif


