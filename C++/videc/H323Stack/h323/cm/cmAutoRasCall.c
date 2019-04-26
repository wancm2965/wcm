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
 * cmAutoRasCall
 * -------------
 * This file provides the calls part of the automatic RAS module.
 * This includes the following features:
 *
 * - Handling incoming DRQs
 * - Responding automatically to IRQ with IRR on calls
 * - Responding automatically to BRQ with BCF on calls
 ************************************************************************/

#include "q931asn1.h"
#include "cmintr.h"
#include "cmdebprn.h"
#include "cmras.h"
#include "cmCrossReference.h"
#include "cmAutoRasCall.h"
#include "cmAutoRasAltGK.h"
#include "cmAutoRasEP.h"
#include "cmCall.h"
#include "rasdef.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Private functions declaration */
static int autoRasPrepareARQ(IN autorasEndpoint* autoras, IN HCALL hsCall);





/************************************************************************
 *
 *                              Private functions
 *
 ************************************************************************/




/************************************************************************
 * autoRasWaitRegistration
 * purpose: Wait to see if endpoint got registered before beginning to
 *          dial this call. This function is called for the call every
 *          second to see if the registration status has changed.
 * input  : context - autoras call handle
 * output : none
 * return : none
 ************************************************************************/
static RvBool autoRasWaitRegistration(IN void* context)
{
    autorasEndpoint* autoras;
    autorasCall* call = (autorasCall *)context;
    HCALL hsCall;
    RvBool cont = RV_TRUE; /* continue with the timer */

    hsCall = cmiGetByAutoRas((HAUTORASCALL)call);
    if (emaLock((EMAElement)hsCall))
    {
        autoras = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));

        /* Let's check the registration status once more */
        switch (autoras->internalState)
        {
            case autorasRegistered:
                /* We're registered - try dialing the call again, we may be using preGranted ARQ */
                cont = RV_FALSE;
                RvH323TimerClear(autoras->hTimers, &call->timer);
                if (callDial((callElem*)hsCall) < 0)
                {
                    /* Notify that we can't make this call */
                    autoras->event(hsCall, NULL, cmiAutoRasEvFailedOnARQ);
                }
                break;

            case autorasRegFailed:
                /* Endpoint failed to register... */
                cont = RV_FALSE;
                RvH323TimerClear(autoras->hTimers, &call->timer);
                autoras->event(hsCall, NULL, cmiAutoRasEvCantRegister);
                break;

            case autorasRegNotTried:
                /* This case should never relly happen */

            case autorasRegTrying:
                /* Do nothing in this case - we'll get back to here in a second with the timer */
                break;
        }

        emaUnlock((EMAElement)hsCall);
    }
    return cont;
}

/************************************************************************
 * autoRasCallIdleState
 * purpose: Set the automatic RAS information of the call to the idle state
 *          This will close any pending transactions for the call.
 * input  : autoras - Automatic RAS handle
 *          call    - Automatic RAS information for the call
 * output : none
 * return : none
 ************************************************************************/
void autoRasCallIdleState(IN autorasEndpoint* autoras, IN autorasCall*   call)
{
    if (call->tx != NULL)
    {
        /* Remove the transaction */
        cmRASClose(call->tx);
        call->tx = NULL;
    }

    if (call->irrTx != NULL)
    {
        /* Remove the transaction */
        cmRASClose(call->irrTx);
        call->irrTx = NULL;
    }

    /* Stop the IRR timer */
    RvH323TimerCancel(autoras->hTimers, &call->timer);

    call->callState = autorasCallIdle;
}

int autoRasRegAndSend
(
 IN autorasEndpoint* autorasE,
 IN autorasCall* autorasC,
 IN cmTransportAddress * GkAddr,
 IN cmAlias * GkIdPtr
);

/************************************************************************
 * autoRasCallSendToNextGK
 * purpose: Resend a request to the next GK
 * input  : autorasE    - Automatic RAS EP module
 *          autorasC    - Automatic RAS call module
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int autoRasCallSendToNextGK(IN autorasEndpoint * autorasE, IN autorasCall * autorasC)
{
    /* ok, so we already got everything in, except cmRASParamDestinationIpAddress
       and GK ID. let's go get'em! */
    RvPvtNodeId tmpNode, GkNodeId;
    cmTransportAddress GKAddr;
    cmAlias GKID;
    char GKIDstr[128];
    RvInt32 needToRegister = 0;
    int ret;

    /* check if we were already in search, or is this the first time */
    if (!autoRasIsInSearch(autorasE, autorasC) && !autorasC->isTempRdrn)
    {
        /* notify the application that we started searching for a new GK */
        autoRasChangeState(autorasE, cmSearchingAlternateGatekeeper, RV_PVT_INVALID_NODEID);
    }

    GkNodeId = autoRasGetNextAltGkNode(autorasE, autorasC);
    if (!RV_PVT_NODEID_IS_VALID(GkNodeId))
    {
        /* damn, no more GKs! we failed! */
        return RV_ERROR_UNKNOWN;
    }

    /* Get GK address */
    tmpNode = pvtGetChild(autorasE->hVal, GkNodeId, __q931(rasAddress), NULL);
    if((tmpNode<0) || (cmVtToTA(autorasE->hVal, tmpNode, (cmTransportAddress *) &GKAddr)<0))
    {
        /* failed again ! */
        return RV_ERROR_UNKNOWN;
    }
    cmRASSetParam(autorasC->tx, cmRASTrStageRequest, cmRASParamDestinationIpAddress, 0, sizeof(GKAddr), (char *)&GKAddr);

    /* Get GK ID */
    GKID.string = GKIDstr;
    tmpNode = pvtGetChild(autorasE->hVal, GkNodeId, __q931(gatekeeperIdentifier), NULL);
    if((tmpNode>=0) &&
        ((GKID.length = (unsigned short) pvtGetString(autorasE->hVal, tmpNode, 128, GKIDstr))>0))
    {
        cmRASSetParam(autorasC->tx, cmRASTrStageRequest, cmRASParamGatekeeperID, 0, sizeof(GKID), (char *)&GKID);
    }
    else
    {
        RvPvtNodeId nodeId = cmiRASGetRequest(autorasC->tx);
        __pvtGetNodeIdByFieldIds(nodeId, autorasE->hVal, nodeId,
            {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN});
        pvtDelete(autorasE->hVal, nodeId);
    }

    /* Do we need to register first ? */
    tmpNode = pvtGetChildByFieldId(autorasE->hVal, GkNodeId, __q931(needToRegister), &needToRegister, (RvBool *)NULL);
    if(!autorasC->isTempRdrn && (tmpNode>=0) && needToRegister)
    {
        /* ok, relax, it's just a minor setback, we'll quickly register and get back in business */
        autoRasRegAndSend(autorasE, autorasC, &GKAddr, &GKID);
        /* don't close the Tx, we'll re-use it */
        return 0;
    }

    /* Send the request */
    ret = cmRASRequest(autorasC->tx);
    if (ret < 0)
    {
        cmRASClose(autorasC->tx);
        autorasC->tx = NULL;
    }

    return ret;
}

int autoRasDiscRegAndSend
(
 IN autorasEndpoint* autorasE,
 IN autorasCall* autorasC
);

/************************************************************************
 * autoRasSendAftReg
 * purpose: Callback function indicating the response of an RRQ sent to
 *          an Alt GK in order to send another transaction to it.
 * input  : haRas   - Application's handle (autoras in this context)
 *          hsRas   - RAS Transaction handle
 *          trStage - Stage of response
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
int RVCALLCONV autoRasSendAftReg
(
 IN  HAPPRAS          haRas,
 IN  HRAS             hsRas,
 IN  cmRASTrStage     trStage
)
{
    int ret = RV_ERROR_UNKNOWN;
    autorasCall* autorasC = (autorasCall *)haRas;
    HCALL hsCall = cmiGetByAutoRas((HAUTORASCALL)autorasC);
    autorasEndpoint* autorasE = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));
    RvBool closeNewTx = RV_TRUE; /*an indication to close the ras transaction*/

    if (emaLock((EMAElement*)hsCall))
    {
        switch(trStage)
        {
        case cmRASTrStageConfirm:
            {
                /* cool, now let's send the ARQ. be sure to set the new EP ID first */
                cmAlias alias;
                char epid[256];
                memset(&alias, 0, sizeof(alias));
                alias.length = (RvUint16)cmiRASGetEndpointID(cmiGetRasHandle(autorasE->hApp), epid);
                alias.string = epid;
                cmRASSetParam(autorasC->tx, cmRASTrStageRequest, cmRASParamEndpointID, 0, sizeof(alias), (char *)&alias);

                ret = cmRASRequest(autorasC->tx);
                if (ret >= 0)
                    autorasC->callState = autorasCallConnecting;
                else
                {
                    cmRASClose(autorasC->tx);
                    autorasC->tx = NULL;
                }
                break;
            }
        case cmRASTrStageReject:
            {
                int chNodeId;
                int responseId = cmiRASGetResponse(hsRas);

                autorasC->wasRejected = RV_TRUE; /* was rejected by at least one GK */

                /* Check out the reason */
                __pvtGetNodeIdByFieldIds(chNodeId, autorasE->hVal, responseId, {_anyField _q931(rejectReason) _q931(discoveryRequired) LAST_TOKEN});
                if (chNodeId >= 0)
                {
                    /* We have to try all over again, this time for GRQ */
                    autoRasDiscRegAndSend(autorasE, autorasC);
                    /* don't close the new Tx */
                    closeNewTx = RV_FALSE;
                    break;
                }

                /* Let's try the next GK */
                if ((ret=autoRasCallSendToNextGK(autorasE, autorasC)) >= 0)
                {
                    /* don't close the new Tx */
                    closeNewTx = RV_FALSE;
                    break;
                }

                /* Drek. could not send the RRQ, maybe because there are no more GKs. */
                autorasC->wasRejected = RV_FALSE;
                autorasC->isTempRdrn = RV_FALSE;
                autorasE->event(hsCall, hsRas, cmiAutoRasEvCallRejected);
                break;
            }
        case cmRASTrStageTimeout:
            {
                /* Let's try the next GK */
                if ((ret=autoRasCallSendToNextGK(autorasE, autorasC)) < 0)
                {
                    /* Drek. could not send the RRQ, maybe because there are no more GKs. */
                    if(autorasC->wasRejected)
                    {
                        /* ARJ - notify the CM about it */
                        autorasC->wasRejected = RV_FALSE;
                        autorasC->isTempRdrn = RV_FALSE;
                        autorasE->event(hsCall, hsRas, cmiAutoRasEvCallRejected);
                        break;
                    }

                    autorasE->event(hsCall, hsRas, cmiAutoRasEvTimedout);

                    /* See if we should try to register again */
                    if(autorasC->isTempRdrn)
                    {
                        /* just a temp redirection. forget it. */
                        autorasC->isTempRdrn = RV_FALSE;
                        break;
                    }
                    if (pvtGetChild(autorasE->hVal, autorasE->confNode, __q931(dontRegisterOnTimeout), NULL) < 0)
                    {
                        /* We should change the status to not registered */
                        autorasE->state = cmIdle;
                        autoRasCallIdleState(autorasE, autorasC);

                        /* Make sure we're trying to register again */
                        if ((autorasE->internalState != autorasRegTrying) && (autorasE->internalState != autorasRegFailed))
                        {
                            cmRegister(autorasE->hApp);
                            /* don't close the new Tx */
                            closeNewTx = RV_FALSE;
                            break;
                        }
                    }
                    break;
                }
                /* succeeded in sending to next GK, don't delete the tx */
                closeNewTx = RV_FALSE;
                break;
            }
        default:
            /* This shouldn't happen... */
            break;
        }

        emaUnlock((EMAElement*)hsCall);
    }

    RvMutexLock(&autorasE->autorasLock, autorasE->logMgr);
    if ((closeNewTx == RV_TRUE) && (hsRas == autorasE->registrationTx))
    {
        cmRASClose(hsRas);
        autorasE->registrationTx = NULL;
    }
    RvMutexUnlock(&autorasE->autorasLock, autorasE->logMgr);

    if (closeNewTx == RV_FALSE)
        ret = RV_OK;
    
    return ret;
}

/************************************************************************
 * autoRasRegAndSend
 * purpose: Make a registration to an AltGK, with the haRas of autorasC
 * input  : autorasE    - Automatic RAS module for an EP
 *          autorasC    - Automatic RAS module for a call
 *          GkAddr      - Pointer to the address of the GK used
 *          GkIdPtr     - Pointer to the GK ID of the GK used
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int autoRasRegAndSend
(
    IN autorasEndpoint* autorasE,
    IN autorasCall* autorasC,
    IN cmTransportAddress * GkAddr,
    IN cmAlias * GkIdPtr
)
{
    int ret = 0;

    RvMutexLock(&autorasE->autorasLock, autorasE->logMgr);
    /* get rid of the old tx before creating the new one */
    if (autorasE->registrationTx != NULL)
    {
        cmRASClose(autorasE->registrationTx);
        autorasE->registrationTx = NULL;
    }
    ret = cmRASStartTransaction(autorasE->hApp, (HAPPRAS)autorasC,
        &autorasE->registrationTx, cmRASRegistration, GkAddr, NULL);
    RvMutexUnlock(&autorasE->autorasLock, autorasE->logMgr);

    if (ret >= 0)
    {
        /* Set the parameters of the message */
        cmRASSetParam(autorasE->registrationTx, cmRASTrStageRequest, cmRASParamDiscoveryComplete, 0, (RvInt32)autorasE->discoveryComplete, NULL);
        cmRASSetParam(autorasE->registrationTx, cmRASTrStageRequest, cmRASParamKeepAlive, 0, 0, NULL);
        if(GkIdPtr)
        {
            cmRASSetParam(autorasE->registrationTx, cmRASTrStageRequest, cmRASParamGatekeeperID, 0, sizeof(*GkIdPtr), (char *) GkIdPtr);
        }
        else
        {
            RvPvtNodeId nodeId = cmiRASGetRequest(autorasE->registrationTx);
            __pvtGetNodeIdByFieldIds(nodeId, autorasE->hVal, nodeId,
                {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN});
            pvtDelete(autorasE->hVal, nodeId);
        }

        /* Make sure next time we know nothing about a GCF */
        autorasE->discoveryComplete = RV_FALSE;

        /* Set the response handler for this message */
        cmiRASSetTrEventHandler(autorasE->registrationTx, autoRasSendAftReg);

        /* Send the request */
        ret = cmRASRequest(autorasE->registrationTx);
    }

    return ret;
}

/************************************************************************
 * autoRasRegAndSendAftDisc
 * purpose: Callback function indicating the response to a GRQ sent to
 *          an Alt GK.
 * input  : haRas   - Application's handle (autoras in this context)
 *          hsRas   - RAS Transaction handle
 *          trStage - Stage of response
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
int RVCALLCONV autoRasRegAndSendAftDisc
(
 IN  HAPPRAS          haRas,
 IN  HRAS             hsRas,
 IN  cmRASTrStage     trStage
)
{
    int ret = RV_ERROR_UNKNOWN;
    RvPvtNodeId responseId = cmiRASGetResponse(hsRas);
    autorasCall* autorasC = (autorasCall *)haRas;
    HCALL hsCall = cmiGetByAutoRas((HAUTORASCALL)autorasC);
    autorasEndpoint* autorasE = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));
    RvBool closeNewTx = RV_TRUE; /*an indication to close the ras transaction*/

    if (emaLock((EMAElement*)hsCall))
    {
        switch(trStage)
        {
        case cmRASTrStageConfirm:
            {
                /* cool, now let's send the RRQ. first we have to get the address */
                int tmpNodeId;
                cmTransportAddress GkAddr;
                cmRASAlias * GkIdPtr = NULL;
                cmRASAlias GkId;
                char IdSrt[128];
                GkId.string = IdSrt;
                GkId.length = 0;

                __pvtGetByFieldIds(tmpNodeId, autorasE->hVal, responseId,
                    { _q931(gatekeeperConfirm) _q931(rasAddress) LAST_TOKEN }, NULL, NULL, NULL);
                cmVtToTA(autorasE->hVal, tmpNodeId, &GkAddr);

                __pvtGetByFieldIds(tmpNodeId, autorasE->hVal, responseId,
                    { _q931(gatekeeperConfirm) _q931(gatekeeperIdentifier) LAST_TOKEN }, NULL, NULL, NULL);
                if(tmpNodeId>=0)
                {
                    int gkIdLen = pvtGetString(autorasE->hVal, tmpNodeId, 128, IdSrt);
                    if (gkIdLen > 0) GkId.length = (RvUint16)gkIdLen;
                    GkIdPtr = &GkId;
                }

                cmRASClose(hsRas);
                RvMutexLock(&autorasE->autorasLock, autorasE->logMgr);
                autorasE->registrationTx = NULL;
                RvMutexUnlock(&autorasE->autorasLock, autorasE->logMgr);
                ret = autoRasRegAndSend(autorasE, autorasC, &GkAddr, GkIdPtr);
                return ret;
            }
        case cmRASTrStageReject:
            autorasC->wasRejected = RV_TRUE;
            /* Fall through to the next case */
        case cmRASTrStageTimeout:
            {
                /* Let's register to the next GK */
                if((ret = autoRasCallSendToNextGK(autorasE, autorasC)) < 0)
                {
                    /* Drek. could not send the ARQ, maybe because there are no more GKs. */
                    if(autorasC->wasRejected)
                    {
                        autorasC->wasRejected = RV_FALSE;
                        autorasC->isTempRdrn = RV_FALSE;
                        autorasE->event(hsCall, hsRas, cmiAutoRasEvCallRejected);
                        break;
                    }

                    autorasE->event(hsCall, hsRas, cmiAutoRasEvTimedout);

                    /* See if we should try to register again */
                    if(autorasC->isTempRdrn)
                    {
                        /* just a temp redirection, forget it */
                        autorasC->isTempRdrn = RV_FALSE;
                        break;
                    }
                    if (pvtGetChild(autorasE->hVal, autorasE->confNode, __q931(dontRegisterOnTimeout), NULL) < 0)
                    {
                        /* We should change the status to not registered */
                        autorasE->state = cmIdle;
                        autoRasCallIdleState(autorasE, autorasC);

                        /* Make sure we're trying to register again */
                        if ((autorasE->internalState != autorasRegTrying) && (autorasE->internalState != autorasRegFailed))
                        {
                            cmRegister(autorasE->hApp);
                            /* don't close the new Tx */
                            closeNewTx = RV_FALSE;
                            break;
                        }
                    }
                    break;
                }
                /* succeeded in sending to next GK, don't delete the tx */
                closeNewTx = RV_FALSE;
                break;
            }

        default:
            /* This shouldn't happen... */
            break;
        }

        emaUnlock((EMAElement*)hsCall);
    }

    RvMutexLock(&autorasE->autorasLock, autorasE->logMgr);
    if ((closeNewTx == RV_TRUE) && (hsRas == autorasE->registrationTx))
    {
        cmRASClose(hsRas);
        autorasE->registrationTx = NULL;
    }
    RvMutexUnlock(&autorasE->autorasLock, autorasE->logMgr);

    if (closeNewTx == RV_FALSE)
        ret = 0;

    return ret;
}

/************************************************************************
 * autoRasDiscRegAndSend
 * purpose: Send GRQ for an Alt Gk which answered with discoveryRequired
 * input  : autorasE    - Automatic RAS module for an EP
 *          autorasC    - Automatic RAS module for a call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int autoRasDiscRegAndSend
(
    IN autorasEndpoint* autorasE,
    IN autorasCall* autorasC
)
{
    int ret, tmpNode;
    cmTransportAddress GkAddr;
    int GKnode = autoRasGetCurrentAltGkNode(autorasE, autorasC);

    if (GKnode<0)
        /* SHOULD NOT BE HERE !! */
        return RV_ERROR_UNKNOWN;
    tmpNode = pvtGetChild(autorasE->hVal, GKnode, __q931(rasAddress), NULL);
    if(tmpNode>=0) cmVtToTA(autorasE->hVal, tmpNode, (cmTransportAddress *) &GkAddr);


    RvMutexLock(&autorasE->autorasLock, autorasE->logMgr);
    if (autorasE->registrationTx != NULL)
    {
        cmRASClose(autorasE->registrationTx);
        autorasE->registrationTx = NULL;
    }
    RvMutexUnlock(&autorasE->autorasLock, autorasE->logMgr);
    ret = cmRASStartTransaction(autorasE->hApp, (HAPPRAS)autorasC,
        &autorasE->registrationTx, cmRASGatekeeper, &GkAddr, NULL);

    if (ret >= 0)
    {
        cmAlias GKID;
        char GKIDstr[128];
        int GKIDnode = pvtGetChild(autorasE->hVal, GKnode, __q931(gatekeeperIdentifier), NULL);
        GKID.string = GKIDstr;

        if((GKIDnode>=0) &&
            ((GKID.length = (unsigned short) pvtGetString(autorasE->hVal, GKIDnode, 128, GKIDstr))>0))
        {
            cmRASSetParam(autorasE->registrationTx, cmRASTrStageRequest, cmRASParamGatekeeperID, 0, sizeof(GKID), (char *)&GKID);
        }
        else
        {
            RvPvtNodeId nodeId = cmiRASGetRequest(autorasE->registrationTx);
            __pvtGetNodeIdByFieldIds(nodeId, autorasE->hVal, nodeId,
                {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN});
            pvtDelete(autorasE->hVal, nodeId);
        }

        /* Set the parameters of the message */
        cmRASSetParam(autorasE->registrationTx, cmRASTrStageRequest, cmRASParamDiscoveryComplete, 0, (RvInt32)autorasE->discoveryComplete, NULL);
        cmRASSetParam(autorasE->registrationTx, cmRASTrStageRequest, cmRASParamKeepAlive, 0, 0, NULL);

        /* Make sure next time we know nothing about a GCF */
        autorasE->discoveryComplete = RV_FALSE;

        /* Set the response handler for this message */
        cmiRASSetTrEventHandler(autorasE->registrationTx, autoRasRegAndSendAftDisc);

        /* Send the request */
        ret = cmRASRequest(autorasE->registrationTx);
    }

    return ret;
}

/************************************************************************
 * autoRasIrrTimeout
 * purpose: Callback function which sends an unsolicited IRR on a call
 * input  : context - autoras call handle
 * output : none
 * return : none
 ************************************************************************/
RvBool autoRasIrrTimeout(IN void* context)
{
    autorasCall* call = (autorasCall *)context;

    cmRASRequest(call->irrTx);

    return RV_TRUE;
}


/************************************************************************
 * autoRasCallTransaction
 * purpose: Callback function used for automatic RAS for dealing with
 *          incoming transactions related to specific calls
 * input  : hApp        - Stack instance handle
 *          hsRas       - Stack's handle for the RAS transaction
 *          hsCall      - Stack's call handle
 *          transaction - The type of transaction that arrived
 *          srcAddress  - Address of the source
 *          haCall      - Application's call handle
 *          notifyCb    - Callback for application about the request
 * output : none
 * return : RV_TRUE if message was processed by this callback and shouldn't
 *          be processed by the manual RAS callbacks
 *          RV_FALSE if message wasn't processed by this callback
 ************************************************************************/
RvBool RVCALLCONV autoRasCallTransaction(
    IN  HAPP                hApp,
    IN  HRAS                hsRas,
    IN  HCALL               hsCall,
    IN  cmRASTransaction    transaction,
    IN  cmTransportAddress  *srcAddress,
    IN  HAPPCALL            haCall,
    IN  cmEvAutoRASRequestT notifyCb)
{
    autorasCall* call = (autorasCall *)cmiGetAutoRas(hsCall);

    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(haCall);
    RV_UNUSED_ARG(srcAddress);

    /* Notify the application if we have to */
    if (notifyCb != NULL)
    {
        switch (transaction)
        {
            case cmRASDisengage:
            case cmRASBandwidth:
            case cmRASInfo:
                cmiCBEnter((hApp, "cmEvAutoRASRequest(hsRas=%p,hsCall=%p,trans=%d)", hsRas, hsCall, transaction));
                notifyCb(hsRas, hsCall, transaction, srcAddress, haCall);
                cmiCBExit((hApp, "cmEvAutoRASRequest(hsRas=%p)", hsRas));
                break;
            default:
                /* Not through automatic RAS... */
                break;
        }
    }

    /* See what we have */
    if (emaLock((EMAElement)hsCall))
    {
        switch (transaction)
        {
            case cmRASDisengage:
            {
                /* DRQ - automatically confirm it */
                autorasEndpoint* autoras;
                autorasCallState state = call->callState;
                call->callState = autorasCallDisconnecting;
                autoras = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));

                /* Always send DCF on this one and close the transaction */
                cmRASConfirm(hsRas);
                cmRASClose(hsRas);

                /* We should get back to the idle state of the transaction */
                autoRasCallIdleState(autoras, call);

                if (state != autorasCallIdle)
                {
                    /* Notify application about this situation */
                    autoras->event(hsCall, hsRas, cmiAutoRasEvCallDropForced);
                }
                break;
            }

            case cmRASBandwidth:
            {
                /* BRQ - always confirm it */
                autorasEndpoint* autoras;
                autoras = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));
                autoras->event(hsCall, hsRas, cmiAutoRasEvRateChanged);
                cmRASConfirm(hsRas);
                cmRASClose(hsRas);
                break;
            }

            case cmRASInfo:
            {
                /* IRQ on call - always confirm it */
                cmRASConfirm(hsRas);
                cmRASClose(hsRas);
                break;
            }

            default:
                /* Not processed by automatic RAS */
                emaUnlock((EMAElement)hsCall);
                return RV_FALSE;
        }
        emaUnlock((EMAElement)hsCall);
    }

    /* Make sure manual RAS knowns we have processed this mesage */
    return RV_TRUE;
}

/************************************************************************
 * autoRasBandwidthResponse
 * purpose: Callback function invoked when a response for BRQ arrives
 * input  : haRas   - Application's handle (autoras in this context)
 *          hsRas   - RAS Transaction handle
 *          trStage - Stage of response
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
int RVCALLCONV autoRasBandwidthResponse
(
    IN  HAPPRAS          haRas,
    IN  HRAS             hsRas,
    IN  cmRASTrStage     trStage)
{
    autorasEndpoint* autoras;
    autorasCall* call = (autorasCall *)haRas;
    HCALL hsCall = cmiGetByAutoRas((HAUTORASCALL)call);
    int rv = RV_ERROR_UNKNOWN;
    int responseId = cmiRASGetResponse(hsRas);

    if (emaLock((EMAElement)hsCall))
    {

        autoras = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));

        /* See what we've got for the BRQ */
        switch(trStage)
        {
            case cmRASTrStageConfirm:
            {
                /* this function will set the new GK if needed, and get the new AltGKs list */
                setRasAlternateGatekeeper(autoras, call, hsRas, -1, RV_FALSE);
                break;
            }

            case cmRASTrStageReject:
            {
                int chNodeId;

                /* See if this is a redirection */
                __pvtGetNodeIdByFieldIds(chNodeId, autoras->hVal, responseId, {_anyField _q931(altGKInfo) LAST_TOKEN});
                if (chNodeId >= 0)
                {
                    /* Se we are redirected to an AltGK.*/
                    RvInt32 isPerm;
                    int newAltGKs;

                    call->wasRejected = RV_TRUE;

                    /* maybe we are already working on a redirection */
                    if(!autoRasIsInSearch(autoras,call))
                    {
                        /* We're not working on anything right now  - we start redirection */
                        /* Let's see if permanently of temporarily */
                        newAltGKs = pvtGetChild(autoras->hVal, chNodeId, __q931(alternateGatekeeper), NULL);
                        pvtGetChildByFieldId(autoras->hVal, chNodeId, __q931(altGKisPermanent), &isPerm, NULL);
                        if(isPerm)
                            autoRasSetPermList(autoras, newAltGKs);
                        else
                        {
                            call->isTempRdrn = RV_TRUE;
                            autoRasSetTempList(autoras, newAltGKs);
                        }
                    }
                    /* ok, let's send the old transaction to a new GK */
                    if(autoRasCallSendToNextGK(autoras, call) >= 0)
                    {
                        /* Sent ok */
                        emaUnlock((EMAElement)hsCall);
                        /* don't close the Tx, we're re-using it */
                        return 0;
                    }
                    else
                    { /* Otherwise we are rejected for good */
                        call->isTempRdrn = RV_FALSE;
                        call->wasRejected = RV_FALSE;
                        autoRasResetSearch(autoras,NULL);
                    }
                }
                else
                    setRasAlternateGatekeeper(autoras, call, hsRas, -1, RV_FALSE);
                break;
            }

            case cmRASTrStageTimeout:
            {
                /* timedout... */
                cmElem* app = (cmElem *)emaGetInstance((EMAElement)hsCall);

                /* maybe we are already working on a redirection, or
                   we're not working on anything right now, but we may have perm Alt GKs */
                if (autoRasIsInSearch(autoras,call) || autoRasGotPermList(autoras))
                {
                    /* So let's try the next GK */
                    if(autoRasCallSendToNextGK(autoras, call) >= 0)
                    {
                        emaUnlock((EMAElement)hsCall);
                        /* don't close the Tx, we're re-using it */
                        return 0;
                    }
                }

                if(call->wasRejected)
                {
                    /* We got SOME response. */
                    call->wasRejected = RV_FALSE;
                    call->isTempRdrn = RV_FALSE;
                    break;
                }

                /* See if we should try to register again */
                if(call->isTempRdrn)
                {
                    /* It's just a temporary redirection. forget it. */
                    call->isTempRdrn = RV_FALSE;
                }
                else if (pvtGetChild(app->hVal, app->rasConf, __q931(dontRegisterOnTimeout), NULL) < 0)
                {
                    /* We should change the status to not registered */
                    autoras->state = cmIdle;
                    autoRasCallIdleState(autoras, call);

                    /* Make sure we're trying to register again */
                    if ((autoras->internalState != autorasRegTrying) && (autoras->internalState != autorasRegFailed))
                        cmRegister(autoras->hApp);
                }

                /* Make sure we close this transaction if it wasn't already closed */
                if (call->tx == hsRas)
                {
                    cmRASClose(hsRas);
                    call->tx = NULL;
                }
                emaUnlock((EMAElement)hsCall);
                /* exit here, do not raise cmiAutoRasEvRateChanged - it needs a response node */
                return 0;
            }

            default:
                /* Shouldn't happen */
                return RV_ERROR_UNKNOWN;
        }

        /* Notify the application about this one */
        rv = autoras->event(hsCall, hsRas, cmiAutoRasEvRateChanged);

        /* Make sure we close this transaction if it wasn't already closed */
        if (call->tx == hsRas)
        {
            cmRASClose(hsRas);
            call->tx = NULL;
        }
        emaUnlock((EMAElement)hsCall);
    }
    return rv;
}



/************************************************************************
 * autoRasDisengageResponse
 * purpose: Callback function invoked when a response for DRQ arrives
 * input  : haRas   - Application's handle (autoras in this context)
 *          hsRas   - RAS Transaction handle
 *          trStage - Stage of response
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
int RVCALLCONV autoRasDisengageResponse
(
    IN  HAPPRAS          haRas,
    IN  HRAS             hsRas,
    IN  cmRASTrStage     trStage)
{
    autorasEndpoint* autoras;
    autorasCall* call = (autorasCall *)haRas;
    HCALL hsCall;
    int rv = RV_ERROR_UNKNOWN;
    int responseId = cmiRASGetResponse(hsRas);

    RV_UNUSED_ARG(trStage);

    hsCall = cmiGetByAutoRas((HAUTORASCALL)call);
    if (emaLock((EMAElement)hsCall))
    {
        autoras = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));

        /* See what we've got for the BRQ */
        switch(trStage)
        {
            case cmRASTrStageConfirm:
            {
                /* this function will set the new GK if needed, and get the new AltGKs list */
                setRasAlternateGatekeeper(autoras, call, hsRas, -1, RV_FALSE);
                break;
            }

            case cmRASTrStageReject:
            {
                int chNodeId;

                /* See if this is a redirection */
                __pvtGetNodeIdByFieldIds(chNodeId, autoras->hVal, responseId, {_anyField _q931(altGKInfo) LAST_TOKEN});
                if (chNodeId >= 0)
                {
                    /* Se we are redirected to an AltGK.*/
                    RvInt32 isPerm;
                    int newAltGKs;

                    call->wasRejected = RV_TRUE;

                    /* maybe we are already working on a redirection */
                    if(!autoRasIsInSearch(autoras,call))
                    {
                        /* We're not working on anything right now  - we start redirection */
                        /* Let's see if permanently of temporarily */
                        newAltGKs = pvtGetChild(autoras->hVal, chNodeId, __q931(alternateGatekeeper), NULL);
                        pvtGetChildByFieldId(autoras->hVal, chNodeId, __q931(altGKisPermanent), &isPerm, NULL);
                        if(isPerm)
                            autoRasSetPermList(autoras, newAltGKs);
                        else
                        {
                            call->isTempRdrn = RV_TRUE;
                            autoRasSetTempList(autoras, newAltGKs);
                        }
                    }
                    /* ok, let's send the old transaction to a new GK */
                    if(autoRasCallSendToNextGK(autoras, call) >= 0)
                    {
                        /* Sent ok */
                        emaUnlock((EMAElement)hsCall);
                        /* don't close the Tx, we're re-using it */
                        return 0;
                    }
                    else
                    { /* Otherwise we are rejected for good */
                        call->isTempRdrn = RV_FALSE;
                        call->wasRejected = RV_FALSE;
                        autoRasResetSearch(autoras,NULL);
                    }
                }
                else
                    setRasAlternateGatekeeper(autoras, call, hsRas, -1, RV_FALSE);
                break;
            }

            case cmRASTrStageTimeout:
            {
                /* timedout... */
                cmElem* app = (cmElem *)emaGetInstance((EMAElement)hsCall);

                /* maybe we are already working on a redirection, or
                   we're not working on anything right now, but we may have perm Alt GKs */
                if( autoRasIsInSearch(autoras,call) || autoRasGotPermList(autoras))
                {
                    /* So let's try the next GK */
                    if(autoRasCallSendToNextGK(autoras, call) >= 0)
                    {
                        emaUnlock((EMAElement)hsCall);
                        /* don't close the Tx, we're re-using it */
                        return 0;
                    }
                }

                if(call->wasRejected)
                {
                    /* We got SOME response. */
                    call->wasRejected = RV_FALSE;
                    call->isTempRdrn = RV_FALSE;
                    break;
                }

                /* See if we should try to register again */
                if(call->isTempRdrn)
                {
                    /* It's just a temporary redirection. forget it. */
                    call->isTempRdrn = RV_FALSE;
                    break;
                }
                if (pvtGetChild(app->hVal, app->rasConf, __q931(dontRegisterOnTimeout), NULL) < 0)
                {
                    /* We should change the status to not registered */
                    autoras->state = cmIdle;
                    autoRasCallIdleState(autoras, call);

                    /* Make sure we're trying to register again */
                    if ((autoras->internalState != autorasRegTrying) && (autoras->internalState != autorasRegFailed))
                        cmRegister(autoras->hApp);
                }
                break;
            }

            default:
                /* This shouldn't happen... */
                break;
        }

        /* Make sure call is IDLE */
        autoRasCallIdleState(autoras, call);

        /* Notify the application about this one - ras was closed, we don't need it */
        rv = autoras->event(hsCall, NULL, cmiAutoRasEvCallDropped);
        emaUnlock((EMAElement)hsCall);
    }
    return rv;
}


/************************************************************************
 * autoRasAdmissionResponse
 * purpose: Callback function invoked when a response for ARQ arrives
 * input  : haRas   - Application's handle (autoras in this context)
 *          hsRas   - RAS Transaction handle
 *          trStage - Stage of response
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
int RVCALLCONV autoRasAdmissionResponse(
    IN  HAPPRAS          haRas,
    IN  HRAS             hsRas,
    IN  cmRASTrStage     trStage)
{
    autorasEndpoint* autoras;
    autorasCall* call = (autorasCall *)haRas;
    HCALL hsCall;
    int responseId = cmiRASGetResponse(hsRas);

    hsCall = cmiGetByAutoRas((HAUTORASCALL)call);

    if (emaLock((EMAElement)hsCall))
    {
        autoras = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));

        /* See what we've got for the ARQ */
        switch(trStage)
        {
            case cmRASTrStageConfirm:
            {
                /* ACF - go on with the call */
                int irrFrequency;

                /* Notify the CM */
                autoras->event(hsCall, hsRas, cmiAutoRasEvGotACF);

                /* Make sure the application was kind enough to leave this transaction and
                   not drop this call barbarically. */
                if (!emaWasDeleted((EMAElement)hsRas))
                {
                    /* Get inside the request message */
                    responseId = pvtChild(autoras->hVal, responseId);

                    /* Check if we have an IRR frequency to work with */
                    if (pvtGetChildByFieldId(autoras->hVal, responseId, __q931(irrFrequency), &irrFrequency, NULL) >= 0)
                    {
                        cmiAutoRASCallSetUnsolicitedIRR(hsCall, irrFrequency);
                    }

                    /* this function will set the new GK if needed, and get the new AltGKs list */
                    setRasAlternateGatekeeper(autoras, call, hsRas, -1, RV_FALSE);

                    call->callState = autorasCallConnected;

                    if (((callElem*)hsCall)->state != cmCallStateWaitAddressAck)
                        cmSetupEnd((callElem *)hsCall);
                }
                break;
            }

            case cmRASTrStageReject:
            {
                int chNodeId;

                /* See if this is a redirection */
                __pvtGetNodeIdByFieldIds(chNodeId, autoras->hVal, responseId, {_anyField _q931(altGKInfo) LAST_TOKEN});
                if (chNodeId >= 0)
                {
                    /* Se we are redirected to an AltGK.*/
                    RvInt32 isPerm;
                    int newAltGKs;

                    call->wasRejected = RV_TRUE;

                    /* maybe we are already working on a redirection */
                    if(!autoRasIsInSearch(autoras,call))
                    {
                        /* We're not working on anything right now  - we start redirection */
                        /* Let's see if permanently of temporarily */
                        newAltGKs = pvtGetChild(autoras->hVal, chNodeId, __q931(alternateGatekeeper), NULL);
                        pvtGetChildByFieldId(autoras->hVal, chNodeId, __q931(altGKisPermanent), &isPerm, NULL);
                        if(isPerm)
                            autoRasSetPermList(autoras, newAltGKs);
                        else
                        {
                            call->isTempRdrn = RV_TRUE;
                            autoRasSetTempList(autoras, newAltGKs);
                        }
                    }
                    /* ok, let's send the old transaction to a new GK */
                    if(autoRasCallSendToNextGK(autoras, call) >= 0)
                    {
                        /* Sent ok */
                        emaUnlock((EMAElement)hsCall);
                        /* don't close the Tx, we're re-using it */
                        return 0;
                    }
                    else
                    { /* Otherwise we are rejected for good */
                        call->isTempRdrn = RV_FALSE;
                        call->wasRejected = RV_FALSE;
                        autoRasResetSearch(autoras,NULL);
                    }
                }
                else
                    setRasAlternateGatekeeper(autoras, call, hsRas, -1, RV_FALSE);
                /* ARJ - notify the CM about it */
                autoras->event(hsCall, hsRas, cmiAutoRasEvCallRejected);
                break;
            }

            case cmRASTrStageTimeout:
            {
                /* timedout... */
                cmElem* app = (cmElem *)emaGetInstance((EMAElement)hsCall);

                /* maybe we are already working on a redirection, or
                   we're not working on anything right now, but we may have perm Alt GKs */
                if( autoRasIsInSearch(autoras,call) || autoRasGotPermList(autoras))
                {
                    /* So let's try the next GK */
                    if(autoRasCallSendToNextGK(autoras, call)>=0)
                    {
                        emaUnlock((EMAElement)hsCall);
                        /* don't close the Tx, we're re-using it */
                        return 0;
                    }
                }

                if(call->wasRejected)
                {
                    /* ARJ - notify the CM about it */
                    call->wasRejected = RV_FALSE;
                    call->isTempRdrn = RV_FALSE;
                    autoras->event(hsCall, hsRas, cmiAutoRasEvCallRejected);
                    break;
                }

                autoras->event(hsCall, hsRas, cmiAutoRasEvTimedout);
                if (call->tx == hsRas)
                {
                    cmRASClose(hsRas);
                    call->tx = NULL;
                }

                /* See if we should try to register again */
                if(call->isTempRdrn)
                {
                    /* It's just a temporary redirection. forget it. */
                    call->isTempRdrn = RV_FALSE;
                    break;
                }
                /* If we're here, we were either not redirected, or redirected permanently, and timed
                   out. we may need to register again */
                if (pvtGetChild(app->hVal, app->rasConf, __q931(dontRegisterOnTimeout), NULL) < 0)
                {
                    /* We should change the status to not registered */
                    autoras->state = cmIdle;
                    autoRasCallIdleState(autoras, call);

                    /* Make sure we're trying to register again */
                    if ((autoras->internalState != autorasRegTrying) && (autoras->internalState != autorasRegFailed))
                        cmRegister(autoras->hApp);
                }

                break;
            }

            default:
                /* Shouldn't happen */
                break;
        }

        /* Make sure we close this transaction if it wasn't already closed */
        if (call->tx == hsRas)
        {
            cmRASClose(hsRas);
            call->tx = NULL;
        }
        emaUnlock((EMAElement)hsCall);
    }

    return 0;
}



/************************************************************************
 * autoRasPrepareARQ
 * purpose: Prepare and send an ARQ message to start a call (or continue
 *          after receiving a Setup)
 * input  : autoras     - Automatic RAS module
 *          hsCall      - Stack's call handle
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
static int autoRasPrepareARQ(IN autorasEndpoint* autoras, IN HCALL hsCall)
{
    autorasCall* call = (autorasCall *)cmiGetAutoRas(hsCall);
    cmCallType callType;
    cmAlias alias;
    cmTransportAddress ta;
    char buff[512];
    char partyNumber[256];
    int pnLength = 0;
    int rate;
    int i=0,j=0;
    int ret;
    RvBool outgoingCall = RV_FALSE;

    if (call->tx != NULL)
        cmRASClose(call->tx);

    /* Start an ARQ transaction - default destination (=gk) or the AltGK we found */
    if (cmRASStartTransaction(autoras->hApp, (HAPPRAS)call, &call->tx, cmRASAdmission, NULL, hsCall) < 0)
        return RV_ERROR_UNKNOWN;

    /*Copy the required parameters from the call object */

    if (cmCallGetParam(hsCall, cmParamCallType, 0, (int*)&callType, NULL) >= 0)
        cmRASSetParam(call->tx, cmRASTrStageRequest, cmRASParamCallType, 0, (RvInt32)callType, NULL);

    cmRASSetParam(call->tx, cmRASTrStageRequest, cmRASParamCallModel, 0, (RvInt32)cmCallModelTypeGKRouted, NULL);

    alias.type = (cmAliasType)0;
    alias.string = buff;

    /* Put the party number in the destination if we're the origin of the call */
    cmCallGetOrigin(hsCall, &outgoingCall);
    if (outgoingCall)
    {
        if (cmCallGetParam(hsCall,cmParamFullSourceAddress,0,NULL,NULL)<0)
        {
            /* no aliases set by the user, use terminalAliases */
            cmCallSetParam(hsCall,cmParamFullSourceAddress,0,
                ((rasModule*)cmiGetRasHandle(autoras->hApp))->termAliasesNode, NULL);
        }

        alias.length = sizeof(buff);
        if (cmCallGetParam(hsCall, cmParamCalledPartyNumber, 0, NULL, (char*)&alias)>=0 && alias.type==cmAliasTypeE164)
        {
            pnLength = RvMin(alias.length, 256); /* CalledPartyNumber cannot be larger than 256 anyway */
            cmRASSetParam(call->tx, cmRASTrStageRequest, cmRASParamDestInfo, i++, 0, (char*)&alias);
            memcpy(partyNumber, alias.string, (RvSize_t)pnLength);
        }
    }

    if (!outgoingCall && (i == 0))
    {
        /* didn't put any aliases in the ARQ - use terminalAliases */
        cmCallSetParam(hsCall,cmParamFullDestinationAddress,0,
            ((rasModule*)cmiGetRasHandle(autoras->hApp))->termAliasesNode, NULL);
    }

    alias.length = sizeof(buff);
    while (cmCallGetParam(hsCall, cmParamDestinationAddress, j++, NULL, (char*)&alias)>=0)
    {
        /* make sure we are not inserting the same number twice */
        if ((pnLength != 0) && (alias.type==cmAliasTypeE164))
        {
            if (!memcmp(partyNumber, alias.string, (RvSize_t)pnLength))
            {
                /* found the same one twice. don't add this one, and stop checking */
                alias.length = sizeof(buff);
                pnLength = 0;
                continue;
            }
        }
        cmRASSetParam(call->tx, cmRASTrStageRequest, cmRASParamDestInfo, i++, 0, (char*)&alias);
        alias.length = sizeof(buff);
    }

    if (cmCallGetParam(hsCall, cmParamRouteCallSignalAddress, 0, NULL, (char*)&ta)>=0)
        cmRASSetParam(call->tx, cmRASTrStageRequest, cmRASParamDestCallSignalAddress, 0, 0, (char*)&ta);
    else if (cmCallGetParam(hsCall, cmParamDestCallSignalAddress, 0, NULL, (char*)&ta)>=0)
        cmRASSetParam(call->tx, cmRASTrStageRequest, cmRASParamDestCallSignalAddress, 0, 0, (char*)&ta);
    else if (cmCallGetParam(hsCall, cmParamDestinationAnnexEAddress, 0, NULL, (char*)&ta)>=0)
        cmRASSetParam(call->tx, cmRASTrStageRequest, cmRASParamDestCallSignalAddress, 0, 0, (char*)&ta);

    i=0; j=0;
    alias.length = sizeof(buff);
    while (cmCallGetParam(hsCall, cmParamDestExtraCallInfo, i++, NULL, (char*)&alias)>=0)
    {
        cmRASSetParam(call->tx, cmRASTrStageRequest, cmRASParamDestExtraCallInfo, j++, 0, (char*)&alias);
        alias.length = sizeof(buff);
    }

    i=0; j=0;
    alias.length = sizeof(buff);
    while (cmCallGetParam(hsCall, cmParamSourceAddress, i++, NULL, (char*)&alias)>=0)
    {
        cmRASSetParam(call->tx, cmRASTrStageRequest, cmRASParamSrcInfo, j++, 0, (char*)&alias);
        alias.length = sizeof(buff);
    }

    if (cmCallGetParam(hsCall, cmParamSrcCallSignalAddress, 0, NULL, (char*)&ta)>=0)
        cmRASSetParam(call->tx, cmRASTrStageRequest, cmRASParamSrcCallSignalAddress, 0, 0, (char*)&ta);

    /* Set the bandwidth for this call */
    if (cmCallGetParam(hsCall, cmParamRequestedRate, 0, &rate, NULL) >= 0)
        cmRASSetParam(call->tx, cmRASTrStageRequest, cmRASParamBandwidth, 0, rate * 2, NULL);

    /* Make sure automatic RAS will get the responses */
    cmiRASSetTrEventHandler(call->tx, autoRasAdmissionResponse);

    /* Send the request */
    ret = cmRASRequest(call->tx);
    if (ret >= 0)
        call->callState = autorasCallConnecting;
    else
    {
        cmRASClose(call->tx);
        call->tx = NULL;
    }

    return ret;
}








/************************************************************************
 *
 *                              Public functions
 *
 ************************************************************************/


/************************************************************************
 * cmiAutoRASCallStart
 * purpose: Start the calls part of the automatic RAS.
 *          This function sets internal callbacks with the RAS module and
 *          initializes some autoRAS related variables.
 * input  : hAutoRas - Automatic RAS instance
 * output : none
 * return : none
 ************************************************************************/
void cmiAutoRASCallStart(IN HAUTORASMGR  hAutoRas)
{
    autorasEndpoint* autoras = (autorasEndpoint *)hAutoRas;

    /* Set the event handler for the endpoint related RAS transactions */
    cmiRASSetCallTrEventHandler(autoras->hApp, autoRasCallTransaction);
}


/************************************************************************
 * cmiAutoRASCallCreate
 * purpose: Create the call information needed for automatic RAS for
 *          a specified call
 * input  : hsCall  - Stack's call handle
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiAutoRASCallCreate(IN HCALL   hsCall)
{
    autorasCall* call = (autorasCall *)cmiGetAutoRas(hsCall);
    memset(call, 0, sizeof(autorasCall));
    call->timer = NULL;
    call->callState = autorasCallIdle;
    call->altGKInd = -1;
    call->isTempRdrn = RV_FALSE;

    return 0;
}


/************************************************************************
 * cmiAutoRASCallDial
 * purpose: Send an ARQ on a call to start the call with the GK
 * input  : hsCall  - Stack's call handle
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiAutoRASCallDial(IN HCALL   hsCall)
{
    int ret = 0;
    autorasEndpoint* autoras = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));

	/* disableRAS */
	if (autoras == NULL)
      return RV_ERROR_UNINITIALIZED;

    switch (autoras->internalState)
    {
        case autorasRegistered:
            /* We're registered - send ARQ */
            ret = autoRasPrepareARQ(autoras, hsCall);
            if (ret < 0)
            {
                /* Notify that we can't make this call */
                autoras->event(hsCall, NULL, cmiAutoRasEvFailedOnARQ);
            }
            break;

        case autorasRegFailed:
            /* Endpoint failed to register... */
            autoras->event(hsCall, NULL, cmiAutoRasEvCantRegister);
            break;

        case autorasRegNotTried:
            /* Make sure we're trying to register... */
            ret = cmRegister(autoras->hApp);

            /* No break statement needed here - we want to wait with this call for a while... */

        case autorasRegTrying:
        {
            /* Let's wait a little bit */
            autorasCall* call = (autorasCall *)cmiGetAutoRas(hsCall);

            RvH323TimerCancel(autoras->hTimers, &call->timer);

            /* Make sure to recheck the registration status every second */
            call->timer = RvH323TimerStartPeriodic(autoras->hTimers, autoRasWaitRegistration, (void*)call, 1000);
            break;
        }
    }

    return ret;
}


/************************************************************************
 * cmiAutoRASCallSetRate
 * purpose: Send a BRQ on a call to change the call bandwidth
 * input  : hsCall   - Stack's call handle
 *          rate     - requested rate for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiAutoRASCallSetRate(IN HCALL hsCall, int rate)
{
    int res = 0;
    autorasEndpoint* autoras = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));
    autorasCall* call = (autorasCall *)cmiGetAutoRas(hsCall);

    /* Make sure we're dealing with a connected call */
    if (call->callState == autorasCallConnected)
    {
        /* We should initiate a BRQ */
        if (call->tx != NULL)
        {
            /* Remove the transaction */
            cmRASClose(call->tx);
            call->tx = NULL;
        }

        res = cmRASStartTransaction(autoras->hApp, (HAPPRAS)call, &call->tx, cmRASBandwidth, NULL, hsCall);
        if (res >= 0)
        {
            /* Make sure automatic RAS will get the responses */
            cmiRASSetTrEventHandler(call->tx, autoRasBandwidthResponse);

            cmRASSetParam(call->tx, cmRASTrStageRequest, cmRASParamBandwidth, 0, rate * 2, NULL);

            /* Send the request */
            res = cmRASRequest(call->tx);
        }
    }
    else
        res = RV_ERROR_UNKNOWN;

    return res;
}

/************************************************************************
 * cmiAutoRASCallSetUnsolicitedIRR
 * purpose: Initilises send of unsolicited IRRs (used for pregranted ARQ only
 * input  : hsCall - Stack's call handle
 *          irrFrequency - requested frequency ofIRRs
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiAutoRASCallSetUnsolicitedIRR(IN HCALL hsCall, int irrFrequency)
{
    autorasEndpoint* autoras = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));
    autorasCall* call = (autorasCall *)cmiGetAutoRas(hsCall);

    if (irrFrequency>0)
    {
        int res;

        /* Create an IRR transaction and fill it in with the call's information
           We're leaving this transaction open as we're sending it on each timeout
           event for the IRR */
        if (call->irrTx != NULL)
            cmRASClose(call->irrTx);

        res = cmRASStartTransaction(autoras->hApp, (HAPPRAS)call, &call->irrTx,
            cmRASUnsolicitedIRR, NULL, hsCall);

        if (res >= 0)
        {
            /* Set a timer for IRR messages sent to the GK */
            irrFrequency *= 1000;
            call->timer = RvH323TimerStartPeriodic(autoras->hTimers, autoRasIrrTimeout, (void*)call, irrFrequency);
        }
    }
    return 0;
}



/************************************************************************
 * cmiAutoRASCallDrop
 * purpose: Send a DRQ on a call to start disengaging the call from the GK
 * input  : hsCall  - Stack's call handle
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiAutoRASCallDrop(IN HCALL hsCall)
{
    int ret = 0;
    autorasEndpoint* autoras = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));
    autorasCall* call = (autorasCall *)cmiGetAutoRas(hsCall);

    /* Make sure we haven't already handled a DRQ for this call - we should ignore it */
    switch(call->callState)
    {
    case autorasCallConnected:
    case autorasCallConnecting:
        /* We should initiate a DRQ, but first let's close all outgoing transactions related
           to this call, as they are not needed anymore and they might cause us to loose
           resources when we are working stressfully. */
        if (call->tx != NULL)
        {
            /* Remove the transaction */
            cmRASClose(call->tx);
            call->tx = NULL;
        }
        /* also remove the IRR tx, to free up RAS transactions. */
        if (call->irrTx != NULL)
        {
            /* Remove the transaction */
            cmRASClose(call->irrTx);
            call->irrTx = NULL;
        }

        ret = cmRASStartTransaction(autoras->hApp, (HAPPRAS)call, &call->tx, cmRASDisengage, NULL, hsCall);

        if (ret >= 0)
        {
            /* Make sure automatic RAS will get the responses */
            cmiRASSetTrEventHandler(call->tx, autoRasDisengageResponse);

            /* Send the request */
            ret = cmRASRequest(call->tx);
        }

        if (ret >= 0)
            call->callState = autorasCallDisconnecting;
        else if (call->tx != NULL)
        {
            cmRASClose(call->tx);
            call->tx = NULL;
        }
        break;
    case autorasCallDisconnecting:
        /* do not close the call, wait for DCF */
        return 0;
    case autorasCallIdle:
        /* close the call */
        return RV_ERROR_UNKNOWN;
    }

    return ret;
}

/************************************************************************
 * cmiAutoRASCallClose
 * purpose: Free auto ras resources for the call
 * input  : hsCall  - Stack's call handle
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiAutoRASCallClose(IN HCALL hsCall)
{
    autorasCall* call = (autorasCall *)cmiGetAutoRas(hsCall);
    autorasEndpoint* autoras = (autorasEndpoint *)cmiGetAutoRasHandle((HAPP)emaGetInstance((EMAElement)hsCall));

	/* disableRAS */
	if (autoras != NULL)
		autoRasCallIdleState(autoras, call);
    return 0;
}



#ifdef __cplusplus
}
#endif


