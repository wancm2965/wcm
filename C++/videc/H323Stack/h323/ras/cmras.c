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

#include "cmCrossReference.h"
#include "rasdef.h"
#include "rasutils.h"
#include "rasin.h"
#include "rasout.h"
#include "rasirr.h"
#include "rasparams.h"
#include "cmras.h"
#include "cmdebprn.h"
#include "cmutils.h"

#ifdef __cplusplus
extern "C" {
#endif



#ifdef RV_DEBUG
#define RV_RAS_DEBUG
#endif




/************************************************************************
 *
 *                              Public functions
 *
 ************************************************************************/

/************************************************************************
 * cmRASStartTransaction
 * purpose: Create a RAS transaction.
 *          This function exchanges handles with the application and connects
 *          between the transaction and the call (if applicable).
 * input  : hApp        - Application's handle for a stack instance
 *          haRas       - Application's handle for the RAS transaction
 *          transaction - The transaction type we want to start
 *          destAddress - Address of the destination.
 *                        If set to NULL, then it's for the gatekeeper
 *          hsCall      - Stack's call handle if the transaction is related
 *                        to a call. NULL otherwise.
 * output : lphsRas     - The stack's RAS transaction handle that was
 *                        created.
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASStartTransaction(
    IN  HAPP                hApp,
    IN  HAPPRAS             haRas,
    OUT LPHRAS              lphsRas,
    IN  cmRASTransaction    transaction,
    IN  cmTransportAddress  *destAddress,
    IN  HCALL               hsCall)
{
    rasModule* ras = (rasModule *)cmiGetRasHandle(hApp);
    rasOutTx* tx;
    int ret = 0;

    if (lphsRas != NULL) *lphsRas = NULL;

    cmiAPIEnter((hApp, "cmRASStartTransaction(hApp=%p,haRas=%p,transaction=%d,hsCall=%p)",
                         hApp, haRas, transaction, hsCall));

    /* Create the transaction */
    tx = rasCreateOutTx(ras, haRas, transaction, destAddress);

    /* See what happened */
    if (tx != NULL)
    {
        /* Transaction was created successfully */
        tx->hsCall = hsCall;

        if (hsCall != NULL)
        {
            switch (transaction)
            {
            case cmRASAdmission:
            case cmRASDisengage:
            case cmRASBandwidth:
            case cmRASInfo:
            case cmRASServiceControl:
                {
                    /* See if we have to set any call related information */
                    char    callid[16];
                    RvInt32 rasCrv, callidLen;

                    callidLen = sizeof(callid);
                    if (cmCallGetParam(hsCall, cmParamCallID, 0, &callidLen, callid) >= 0)
                        rasSetParam(ras, (HRAS)tx, cmRASTrStageRequest, cmRASParamCallID, 0, callidLen, callid);

                    callidLen = sizeof(callid);
                    if (cmCallGetParam(hsCall, cmParamCID, 0, &callidLen, callid) >= 0)
                        rasSetParam(ras, (HRAS)tx, cmRASTrStageRequest, cmRASParamCID, 0, callidLen, callid);

                    if (transaction != cmRASServiceControl)
                    {
                        if (cmCallGetParam(hsCall, cmParamRASCRV, 0, &rasCrv, NULL) >= 0)
                            rasSetParam(ras, (HRAS)tx, cmRASTrStageRequest, cmRASParamCRV, 0, rasCrv, NULL);
                    }

                    rasSetParam(ras, (HRAS)tx, cmRASTrStageRequest, cmRASParamAnswerCall, 0, !cmCallGetOrigin(hsCall,NULL), NULL);
                    break;
                }

            case cmRASUnsolicitedIRR:
            {
                /* IRR transactions get some special treatment */
                RvPvtNodeId irrNode = pvtAdd(ras->hVal, tx->txProperty, __q931(request), 0, NULL, NULL);
                ret = rasSetIrrFields(ras, (HRAS)tx, irrNode, hsCall);
                break;
            }

            case cmRASGatekeeper:
            case cmRASRegistration:
            case cmRASUnregistration:
            case cmRASLocation:
            case cmRASNonStandard:
            case cmRASUnknown:
            case cmRASResourceAvailability:
            case cmRASMaxTransaction:
                /* Nothing to do */
                break;
            }
        }
    }
    else
        ret = RV_ERROR_UNKNOWN;

    /* Check the return value and clear the transaction if we failed somewhere */
    if (ret < 0)
    {
        if (tx != NULL)
        {
            rasCloseOutTx(ras, tx);
            tx = NULL;
        }
    }
    else
    {
        /* Success value should always be 0 for this function */
        ret = 0;
    }

    /* Return this transaction to the application */
    if (lphsRas != NULL) *lphsRas = (HRAS)tx;

    cmiAPIExit((hApp, "cmRASStartTransaction(haRas=%p,hsRas=%p)=%d", haRas, tx, ret));
    return ret;
}


/************************************************************************
 * cmRASSetHandle
 * purpose: Sets or changes the application handle for a RAS transaction.
 * input  : hsRas   - Stack's handle for the RAS transaction
 *          haRas   - Application's handle for the RAS transaction to be set
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASSetHandle(
    IN  HRAS            hsRas,
    IN  HAPPRAS         haRas)
{
    rasModule* ras;
    int ret;
    if (hsRas == NULL) return RV_ERROR_UNKNOWN;

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);
    cmiAPIEnter((ras->app, "cmRASSetHandle(hsRas=%p,haRas=%p)", hsRas, haRas));

    /* Update the application's handle */
    ret = emaSetApplicationHandle((EMAElement)hsRas, (void*)haRas);

    cmiAPIExit((ras->app, "cmRASSetHandle(hsRas=%p)=%d", hsRas, ret));
    return ret;
}


/************************************************************************
 * cmRASGetParam
 * purpose: Get a parameter about the RAS transaction
 * input  : hsRas   - Stack's handle for the RAS transaction
 *          trStage - The transaction stage the parameters
 *          param   - Type of the RAS parameter
 *          index   - If the parameter has several instances, the index
 *                    that identifies the specific instance (0-based).
 *                    0 otherwise.
 *          value   - If the parameter value is a structure, the value
 *                    represents the length of the parameter.
 * output : value   - For a simple integer - the parameter's value.
 *                    For a structure - the length of the parameter.
 *          svalue  - For a structure - svalue represents the parameter
 *                    itself. Can be set to NULL if we're only interested
 *                    in its length.
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASGetParam(
    IN  HRAS             hsRas,
    IN  cmRASTrStage     trStage,
    IN  cmRASParam       param,
    IN  int              index,
    IN  OUT RvInt32*     value,
    IN  char*            svalue)
{
    rasModule* ras;
    int status = RV_ERROR_UNKNOWN;
    if (hsRas == NULL) return RV_ERROR_UNKNOWN;

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);
    cmiAPIEnter((ras->app, "cmRASGetParam(hsRas=%p,trStage=%d,%s[%d])",
        hsRas, trStage, rasGetParamName(param), index));

    if(emaLock((EMAElement)hsRas))
    {
        status = rasGetParam(ras, hsRas, trStage, param, index, value, svalue);
        emaUnlock((EMAElement)hsRas);
    }

    cmiAPIExit((ras->app, "cmRASGetParam(hsRas=%p)=%d", hsRas, status));
    return status;
}


/************************************************************************
 * cmRASSetParam
 * purpose: Set a parameter about the RAS transaction
 * input  : hsRas   - Stack's handle for the RAS transaction
 *          trStage - The transaction stage the parameters
 *          param   - Type of the RAS parameter
 *          index   - If the parameter has several instances, the index
 *                    that identifies the specific instance (0-based).
 *                    0 otherwise.
 *          value   - For a simple integer - the parameter's value.
 *                    For a structure - the length of the parameter.
 *          svalue  - For a structure - svalue represents the parameter
 *                    itself.
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASSetParam(
    IN  HRAS             hsRas,
    IN  cmRASTrStage     trStage,
    IN  cmRASParam       param,
    IN  int              index,
    IN  RvInt32          value,
    IN  char*            svalue)
{
    rasModule* ras;
    int status = RV_ERROR_UNKNOWN;
    if (hsRas == NULL) return RV_ERROR_UNKNOWN;

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);
    cmiAPIEnter((ras->app,"cmRASSetParam(hsRas=%p,trStage=%d,%s[%d]=%d)",
                         hsRas, trStage, rasGetParamName(param), index, value));

    if(emaLock((EMAElement)hsRas))
    {
        status = rasSetParam(ras, hsRas, trStage, param, index, value, svalue);
        emaUnlock((EMAElement)hsRas);
    }

    if (status > 0)
        status = 0; /* We want the gatekeeper to work, so we set successful return values to 0 */

    cmiAPIExit((ras->app, "cmRASSetParam(hsRas=%p)=%d", hsRas, status));
    return status;
}


/************************************************************************
 * cmRASGetNumOfParams
 * purpose: Returns the number of params in sequences on the property
 *          tree.
 * input  : hsRas   - Stack's handle for the RAS transaction
 *          trStage - The transaction stage the parameters
 *          param   - Type of the RAS parameter
 * output : none
 * return : Number of params in sequence on the property tree on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASGetNumOfParams(
    IN  HRAS             hsRas,
    IN  cmRASTrStage     trStage,
    IN  cmRASParam       param)
{
    rasModule* ras;
    int status = RV_ERROR_UNKNOWN;
    if (hsRas == NULL) return RV_ERROR_UNKNOWN;

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);
    cmiAPIEnter((ras->app, "cmRASGetNumOfParams(hsRas=%p,trStage=%d,%s)",
        hsRas, trStage, rasGetParamName(param)));

    if(emaLock((EMAElement)hsRas))
    {
        status = rasGetNumOfParams(ras, hsRas, trStage, param);
        emaUnlock((EMAElement)hsRas);
    }

    cmiAPIExit((ras->app, "cmRASGetNumOfParams(hsRas=%p)=%d", hsRas, status));
    return status;
}


/************************************************************************
 * cmRASRequest
 * purpose: Send an outgoing RAS transaction
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASRequest(
    IN  HRAS             hsRas)
{
    rasModule*  ras;
    rasOutTx*   tx;
    int         status;

    if (hsRas == NULL) return RV_ERROR_NULLPTR;
    if (!emaLock((EMAElement)hsRas)) return RV_ERROR_DESTRUCTED;

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);
    cmiAPIEnter((ras->app, "cmRASRequest(hsRas=%p)", hsRas));

    /* Get the transaction */
    tx = rasGetOutgoing(hsRas);
#ifdef RV_RAS_DEBUG
    if (tx != NULL)
    {
        /* Reset the retryCount */
        tx->retryCount = rasCfgGetRetries(ras);

        status = rasSendRequestMessage(ras, tx);
    }
    else
    {
        RvLogError(&ras->log,
            (&ras->log, "cmRASRequest: Bad outgoing transaction handle (%p)", hsRas));
        status = RV_ERROR_UNKNOWN;
    }
#else
    /* Reset the retryCount */
    tx->retryCount = rasCfgGetRetries(ras);
    status = rasSendRequestMessage(ras, tx);
#endif

    cmiAPIExit((ras->app, "cmRASRequest(hsRas=%p)=%d", hsRas, status));
    emaUnlock((EMAElement)hsRas);
    return status;
}


/************************************************************************
 * cmRASDummyRequest
 * purpose: Called after cmRASStartTransaction() on cmRASUnsolicitedIRR.
 *          It allows the application to wait for an unsolicited IRR on
 *          a specific call.
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASDummyRequest(
    IN  HRAS         hsRas)
{
    rasModule*  ras;
    rasOutTx*   tx;
    int         status;
    if (hsRas == NULL) return RV_ERROR_UNKNOWN;

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);
    cmiAPIEnter((ras->app, "cmRASDummyRequest(hsRas=%p)", hsRas));

    /* Get the transaction */
    tx = rasGetOutgoing(hsRas);
#ifdef RV_RAS_DEBUG
    if (tx != NULL)
        status = rasDummyRequest(ras, tx);
    else
    {
        RvLogError(&ras->log,
            (&ras->log, "cmRASDummyRequest: Bad outgoing transaction handle (%p)", hsRas));
        status = RV_ERROR_UNKNOWN;
    }
#else
    status = rasDummyRequest(ras, tx);
#endif

    cmiAPIExit((ras->app, "cmRASDummyRequest(hsRas=%p)=%d", hsRas, status));
    return status;
}


/************************************************************************
 * cmRASConfirm
 * purpose: Sends a confirm response on an incoming RAS request
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASConfirm(
    IN  HRAS             hsRas)
{
    rasModule*  ras;
    int         status;
    if (hsRas == NULL) return RV_ERROR_UNKNOWN;

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);
    cmiAPIEnter((ras->app, "cmRASConfirm(hsRas=%p)", hsRas));

    /* Get the transaction */
    if (emaGetType((EMAElement)hsRas) == RAS_IN_TX)
    {
        rasInTx*    tx;

        tx = rasGetIncoming(hsRas);
        status = rasSendConfirmMessage(ras, tx);
    }
    else
    {
        rasOutTx* tx = rasGetOutgoing(hsRas);
        /* check if this is an unsolicited IRR without an incoming transaction */
#ifdef RV_RAS_DEBUG
        if ((tx != NULL) && (tx->transactionType == cmRASUnsolicitedIRR))
#else
        if (tx->transactionType == cmRASUnsolicitedIRR)
#endif
        {
            /* use "cmRASRequest()" instead, done for backwards compatability */
            status = cmRASRequest(hsRas);
        }
        else
        {
            RvLogError(&ras->log,
                (&ras->log, "cmRASConfirm: Bad outgoing transaction handle (%p)", hsRas));
            status = RV_ERROR_UNKNOWN;
        }
    }

    cmiAPIExit((ras->app, "cmRASConfirm(hsRas=%p)=%d", hsRas, status));
    return status;
}


/************************************************************************
 * cmRASReject
 * purpose: Sends a reject response on an incoming RAS request
 * input  : hsRas       - Stack's handle for the RAS transaction
 *          reason      - The reject reason to use
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASReject(
    IN  HRAS             hsRas,
    IN  cmRASReason      reason)
{
    rasModule*  ras;
    rasInTx*    tx;
    int         status;
    if (hsRas == NULL) return RV_ERROR_UNKNOWN;

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);
    cmiAPIEnter((ras->app, "cmRASReject(hsRas=%p)", hsRas));

    /* Get the transaction */
    tx = rasGetIncoming(hsRas);
#ifdef RV_RAS_DEBUG
    if (tx != NULL)
        status = rasSendRejectMessage(ras, tx, reason);
    else
    {
        RvLogError(&ras->log,
            (&ras->log, "cmRASReject: Bad outgoing transaction handle (%p)", hsRas));
        status = RV_ERROR_UNKNOWN;
    }
#else
    status = rasSendRejectMessage(ras, tx, reason);
#endif

    cmiAPIExit((ras->app, "cmRASReject(hsRas=%p)=%d", hsRas, status));
    return status;
}


/************************************************************************
 * cmRASInProgress
 * purpose: Sends a RIP (ReplyInProgress) message on a transaction
 * input  : hsRas       - Stack's handle for the RAS transaction
 *          delay       - Delay to use in RIP message (in milliseconds)
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASInProgress(
    IN  HRAS         hsRas,
    IN  int          delay)
{
    rasModule*  ras;
    rasInTx*    tx;
    int         status = 0;
    if (hsRas == NULL) return RV_ERROR_UNKNOWN;

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);
    cmiAPIEnter((ras->app, "cmRASInProgress(hsRas=%p, delay=%d)", hsRas, delay));

    /* Get the transaction */
    tx = rasGetIncoming(hsRas);
#ifdef RV_RAS_DEBUG
    if (tx != NULL)
        status = rasSendRIP(ras, tx, delay, RV_TRUE);
    else
    {
        RvLogError(&ras->log,
            (&ras->log, "cmRASInProgress: Bad outgoing transaction handle (%p)", hsRas));
        status = RV_ERROR_UNKNOWN;
    }
#else
    status = rasSendRIP(ras, tx, delay, RV_TRUE);
#endif

    cmiAPIExit((ras->app, "cmRASInProgress(hsRas=%p)=%d", hsRas, status));
    return status;
}


/************************************************************************
 * cmRASClose
 * purpose: Close a RAS transaction
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASClose(
    IN  HRAS             hsRas)
{
    rasModule*  ras;
    int         status = 0;

    if (hsRas == NULL) return RV_ERROR_NULLPTR;
    if (!emaLock((EMAElement)hsRas)) return RV_ERROR_DESTRUCTED;

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);
    cmiAPIEnter((ras->app, "cmRASClose(hsRas=%p)", hsRas));

#if defined(RV_RAS_DEBUG)
    /* Make sure the application is not calling this function twice for the same handle */
    if (emaWasDeleted((EMAElement)hsRas))
    {
        RvLogError(&ras->log,
            (&ras->log, "cmRASClose: This transaction was already closed (%p)", hsRas));
        status = RV_ERROR_UNINITIALIZED;
    }
    else
#endif /* defined(RV_RAS_DEBUG) */

    /* Check if it's an incoming or an outgoing transaction */
    switch (emaGetType((EMAElement)hsRas))
    {
        case RAS_OUT_TX:
        {
            rasOutTx* tx;
            tx = rasGetOutgoing(hsRas);

#ifdef RV_RAS_DEBUG
            if (tx != NULL)
                status = rasCloseOutTx(ras, tx);
            else
            {
                RvLogError(&ras->log,
                    (&ras->log, "cmRASClose: Bad outgoing transaction handle (%p)", hsRas));
                status = RV_ERROR_UNKNOWN;
            }
#else
            status = rasCloseOutTx(ras, tx);
#endif
            break;
        }
        case RAS_IN_TX:
        {
            rasInTx* tx;
            tx = rasGetIncoming(hsRas);

#ifdef RV_RAS_DEBUG
            if (tx != NULL)
                status = rasCloseInTx(ras, tx);
            else
            {
                RvLogError(&ras->log,
                    (&ras->log, "cmRASClose: Bad incoming transaction handle (%p)", hsRas));
                status = RV_ERROR_UNKNOWN;
            }
#else
            status = rasCloseInTx(ras, tx);
#endif
            break;
        }
        default:
            status = RV_ERROR_UNKNOWN;
    }

    cmiAPIExit((ras->app, "cmRASClose(hsRas=%p)=%d", hsRas, status));
    emaUnlock((EMAElement)hsRas);
    return status;
}






/************************************************************************
 * cmRASGetHandle
 * purpose: Returns the stack's handle of the transaction from the
 *          application's handle.
 *          This function is slow and should not be used frequently
 * input  : hApp        - Application's handle for a stack instance
 *          haRas       - Application's handle for the RAS transaction
 * output : lphsRas     - The stack's RAS transaction handle
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASGetHandle(
    IN  HAPP    hApp,
    IN  HAPPRAS haRas,
    OUT LPHRAS  hsRas)
{
    rasModule* ras = (rasModule *)cmiGetRasHandle(hApp);
    HRAS hRas;

    /* Search in incoming transactions */
    hRas = (HRAS)emaGetNext(ras->inRa, NULL);
    while (hRas != NULL)
    {
        if ((HAPPRAS)emaGetApplicationHandle((EMAElement)hRas) == haRas)
        {
            if (hsRas) *hsRas = hRas;
            return 0;
        }
        hRas = (HRAS)emaGetNext(ras->inRa, (EMAElement)hRas);
    }

    /* Search in outgoing transactions */
    hRas = (HRAS)emaGetNext(ras->outRa, NULL);
    while (hRas != NULL)
    {
        if ((HAPPRAS)emaGetApplicationHandle((EMAElement)hRas) == haRas)
        {
            if (hsRas) *hsRas = hRas;
            return 0;
        }
        hRas = (HRAS)emaGetNext(ras->outRa, (EMAElement)hRas);
    }

    /* If we're here, then we haven't found a matching transaction */
    return RV_ERROR_UNKNOWN;
}


/************************************************************************
 * cmRASGetTransaction
 * purpose: Returns the type of RAS transaction
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : transaction - The type of transaction
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASGetTransaction(
    IN  HRAS                hsRas,
    OUT cmRASTransaction*   transaction)
{
    rasModule*  ras;
    int         status = 0;

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);
    cmiAPIEnter((ras->app, "cmRASGetTransaction(hsRas=%p)", hsRas));

    /* Get the transaction type */
    if(emaLock((EMAElement)hsRas))
    {
        /* Check if it's an incoming or an outgoing transaction */
        switch (emaGetType((EMAElement)hsRas))
        {
        case RAS_OUT_TX:
            {
                rasOutTx* tx;
                tx = rasGetOutgoing(hsRas);

                if (tx != NULL)
                    *transaction = tx->transactionType;
                else
                {
                    RvLogError(&ras->log,
                        (&ras->log, "cmRASGetTransaction: Bad outgoing transaction handle (%p)", hsRas));
                    status = RV_ERROR_UNKNOWN;
                }
                break;
            }
        case RAS_IN_TX:
            {
                rasInTx* tx;
                tx = rasGetIncoming(hsRas);

                if (tx != NULL)
                    *transaction = tx->transactionType;
                else
                {
                    RvLogError(&ras->log,
                        (&ras->log, "cmRASGetTransaction: Bad incoming transaction handle (%p)", hsRas));
                    status = RV_ERROR_UNKNOWN;
                }
                break;
            }
        default:
            status = RV_ERROR_UNKNOWN;
        }

        emaUnlock((EMAElement)hsRas);
    }

    cmiAPIExit((ras->app, "cmRASGetTransaction(hsRas=%p,tx=%d)=%d", hsRas, *transaction, status));
    return status;
}


/************************************************************************
 * cmRASGetLastError
 * purpose: This function does absolutely nothing.
 *          It is only here for backward compatibility.
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : 0
 ************************************************************************/
RVAPI
cmRASError RVCALLCONV cmRASGetLastError(
    IN  HRAS             hsRas)
{
    RV_UNUSED_ARG(hsRas);
    return (cmRASError)0;
}



/************************************************************************
 * cmRASSetEventHandler
 * purpose: Sets the callbacks the application wishes to use
 * input  : hApp        - Application's handle for a stack instance
 *          cmRASEvent  - RAS callbacks to set
 *          size        - Size of callbacks struct (*CMRASEVENT)
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRASSetEventHandler(
    IN  HAPP hApp,
    IN  CMRASEVENT cmRASEvent,
    IN  int size)
{
    rasModule* ras = (rasModule *)cmiGetRasHandle(hApp);

    if ((hApp == NULL) || (ras == NULL)) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmRASSetEventHandler(hApp=%p,size=%d)", hApp, size));

    memset(&ras->evApp, 0, sizeof(SCMRASEVENT));
    memcpy(&ras->evApp, cmRASEvent, (RvSize_t)RvMin(size, (int)sizeof(SCMRASEVENT)));

    cmiAPIExit((hApp, "cmRASSetEventHandler(hApp=%p)=0", hApp));
    return 0;
}


/************************************************************************
 * cmAutoRASSetEventHandler
 * purpose: Sets the callbacks the application wishes to use for automatic
 *          RAS. Catching these callbacks allows the application to
 *          know about the messages that the automatic RAS receives.
 *          It doesn't allow the application to act on them - only to
 *          know about them.
 * input  : hApp            - Application's handle for a stack instance
 *          cmAutoRASEvent  - Automatic RAS callbacks to set
 *          size            - Size of callbacks struct (*CMRASEVENT)
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmAutoRASSetEventHandler(
    IN  HAPP            hApp,
    IN  CMAUTORASEVENT  cmAutoRASEvent,
    IN  int             size)
{
    rasModule* ras = (rasModule *)cmiGetRasHandle(hApp);

    if ((hApp == NULL) || (ras == NULL)) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmAutoRASSetEventHandler(hApp=%p,size=%d)", hApp, size));

    memset(&ras->evAutoRas, 0, sizeof(SCMAUTORASEVENT));
    memcpy(&ras->evAutoRas, cmAutoRASEvent, (RvSize_t)RvMin(size, (int)sizeof(SCMAUTORASEVENT)));

    cmiAPIExit((hApp, "cmAutoRASSetEventHandler(hApp=%p)=0", hApp));
    return 0;
}




RVAPI
RvInt32 RVCALLCONV cmGetGKCallSignalAddress(
                                        IN  HAPP             hApp,
                                        OUT     cmTransportAddress*  tr)
{
    int rc;
    rasModule* ras = (rasModule *)cmiGetRasHandle(hApp);

    if ((hApp == NULL) || (ras == NULL)) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmGetGKCallSignalAddress: hApp=%p.", hApp));
    rc= cmVtToTA(ras->hVal, ras->gatekeeperCallSignalAddress, tr);
    cmiAPIExit((hApp, "cmGetGKCallSignalAddress=%d", rc));
    return rc;
}

RVAPI
RvInt32 RVCALLCONV cmGetGKRASAddress(
        IN  HAPP                hApp,
        OUT cmTransportAddress* tr)
{
    rasModule* ras = (rasModule *)cmiGetRasHandle(hApp);
    int rc;
    if ((hApp == NULL)  || (ras == NULL)) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp,  "cmGetGKRASAddress: hApp=%p.", hApp));
    rc = cmVtToTA(ras->hVal, ras->gatekeeperRASAddress, tr);
    cmiAPIExit((hApp, "cmGetGKRASAddress=%d", rc));
    return rc;
}

RVAPI
HPROTCONN RVCALLCONV cmGetRASConnectionHandle(
                IN  HAPP             hApp)
{
    cmElem *app = (cmElem *)hApp;
    HPROTCONN hConn;

    if (hApp == NULL)
        return NULL;

    cmiAPIEnter((hApp, "cmGetRASConnectionHandle: hApp=%p.", hApp));
    hConn = (HPROTCONN)emaGetByIndex(app->rasListeningSockets, 0);
    cmiAPIExit((hApp, "cmGetRASConnectionHandle=%p", hConn));

    return hConn;
}


RVAPI
HPROTCONN RVCALLCONV cmGetUdpChanHandle(IN HCALL hsCall,cmUdpChanHandleType udpChanHandleType)
{
    HPROTCONN hProtCon = NULL;
    HAPP hApp = (HAPP)emaGetInstance((EMAElement)hsCall);
    cmElem *app = (cmElem *)hApp;

    if (hApp == NULL)
        return NULL;

    cmiAPIEnter((hApp, "cmGetUdpChanHandle(hsCall=%p,udpChanHandleType=%d)", hsCall, udpChanHandleType));
    switch(udpChanHandleType)
    {
        case cmRasUdpChannel:
            hProtCon = (HPROTCONN)emaGetByIndex(app->rasListeningSockets, 0);
        break;
        case cmRasUdpChannelMCast:
            hProtCon = (HPROTCONN)emaGetByIndex(app->rasListeningSockets, 1);
        break;
    }
    cmiAPIExit((hApp, "cmGetUdpChanHandle=%p", hProtCon));

    return hProtCon;
}

RVAPI
int RVCALLCONV cmSetUdpChanApplHandle(IN HPROTCONN hCon, HAPPCONN hAppConn)
{
    /* hCon is actually a pointer to (ras->unicastAppHandle) or (ras->multicastAppHandle). if we want
    to set the value of the uni/multicastAppHandle, we should set the value of *hCon to hAppCon. */
    emaSetApplicationHandle((EMAElement)hCon, (void*)hAppConn);
    return RV_TRUE;
}

RVAPI
int RVCALLCONV cmGetUdpChanApplHandle(IN HPROTCONN hCon,HAPPCONN * hAppConn)
{
    /* hCon points to (ras->unicastAppHandle) or (ras->multicastAppHandle). if we want the Application
    handle, we should set *hAppCon to the value of *hCon. */
    if (hCon == NULL)
    {
        *hAppConn = NULL;
        return RV_ERROR_NULLPTR;
    }

    *hAppConn = (HAPPCONN)emaGetApplicationHandle((EMAElement)hCon);

    return RV_OK;
}


/******************************************************************************
 * cmProtocolRecvRawMessageFrom
 * ----------------------------------------------------------------------------
 * General: Simulate receiving a raw message on a given RAS connection.
 *          This can be used for testing or for support of listening to several
 *          different RAS addresses (since the Stack does not support this
 *          feature).
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * See Also: cmGetUdpChanHandle, cmGetRASConnectionHandle.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp         - Stack handle for the application.
 *         hConn        - Connection to use (can be the unicast or the
 *                        multicast RAS connection handle).
 *         buffer       - Buffer of received message.
 *         bufferSize   - Size of buffer.
 *         fromAddress  - Address this message is "received" from.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolRecvRawMessageFrom(
        IN  HAPP                hApp,
        IN  HPROTCONN           hConn,
        IN  RvUint8             *buffer,
        IN  RvSize_t            bufferSize,
        IN  cmTransportAddress  *fromAddress)
{
    RvStatus status = RV_ERROR_BADPARAM;
    rasModule* ras = (rasModule *)cmiGetRasHandle(hApp);
    RasConnection *connection;

    cmiAPIEnter((hApp, "cmProtocolRecvRawMessageFrom(hApp=%p,hConn=%p,size=%d)", hApp, hConn, bufferSize));

    /* First find out which handle is it */
    connection = (RasConnection *)hConn;

    if (emaGetType((EMAElement)hConn) == RAS_PROTCONN)
    {
        /* Receive */
        status = cmiRASReceiveMessage((HRASMGR)ras, connection->index, connection->isMulticast, fromAddress, buffer, bufferSize);
    }

    cmiAPIExit((hApp, "cmProtocolRecvRawMessageFrom=%d", status));

    return status;
}


#ifdef __cplusplus
}
#endif


