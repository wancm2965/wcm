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

#include "rvinternal.h"
#include "rvtimestamp.h"
#include "cmintr.h"
#include "cmdebprn.h"
#include "rasutils.h"
#include "rasparams.h"
#include "rasirr.h"
#include "rasin.h"
#include "cmutils.h"


#ifdef __cplusplus
extern "C" {
#endif



/************************************************************************
 *
 *                              Private functions
 *
 ************************************************************************/


/************************************************************************
 * rasKillInTx
 * purpose: Kill an incoming transaction through the garbage collection
 *          mechanism.
 * input  : ras             - RAS module to use
 *          tx              - Incoming transaction to kill
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
static int rasKillInTx(
    IN rasModule*       ras,
    IN rasInTx*         tx)
{
    /* Lock the transaction itself - we delete it before unlocking it... */
    if (emaLock((EMAElement)tx))
    {
        /* We assume that the hash table is already locked from outside */

        /* Delete the hash entry - locked from outside */
        hashDelete(ras->inHash, tx->hashValue);

        /* Delete, Unlock and Release */
        emaDelete((EMAElement)tx);

        /* Free RPOOL block */
        if (tx->encodedMsg != NULL)
            rpoolFree(ras->messages, tx->encodedMsg);

        emaUnlock((EMAElement)tx);
    }

    return 0;
}


/************************************************************************
 * rasNewTx
 * purpose: Create a new transaction for an incoming message
 * input  : ras             - RAS module to use
 *          rootId          - Message node for transaction
 * output : newRoot         - New message node for transaction
 * return : Transaction's handle on success
 *          NULL on failure
 ************************************************************************/
static rasInTx* rasNewTx(
    IN  rasModule*  ras,
    IN  int         rootId,
    OUT int*        newRoot)
{
    rasInTx*    tx = NULL;
    int         status = 0;
    int         propId, requestId;
    *newRoot = RV_ERROR_UNKNOWN;

    /* Make sure we're supporting incoming transactions */
    if (ras->inRa == NULL)
        return NULL;

    /* Create a root and decode this message */
    propId = pvtAddRoot(ras->hVal, ras->synProperty, 0, NULL);
    requestId = pvtAdd(ras->hVal, propId, __q931(request), 0, NULL, NULL);
    if (requestId >= 0)
        status = pvtMoveTree(ras->hVal, requestId, rootId);
    else
    {
        pvtDelete(ras->hVal, rootId);
        status = requestId;
    }

    /* New transaction - allocate a place for it */
    if (status >= 0)
    {
        *newRoot = requestId;

        tx = (rasInTx *)emaAdd(ras->inRa, NULL);
        if (tx == NULL)
        {
            /* Let's see if we have a closed transaction we can discard of */
            if (ras->firstTx != NULL)
            {
                /* Kill this one and take its place */
                RvLockGet(&ras->lockInHash, ras->logMgr);
                RvLockGet(&ras->lockGarbage, ras->logMgr);
                tx = ras->firstTx;
                ras->firstTx = tx->next;
                if (ras->firstTx == NULL) ras->lastTx = NULL;
                rasKillInTx(ras, tx);
                RvLockRelease(&ras->lockGarbage, ras->logMgr);
                RvLockRelease(&ras->lockInHash, ras->logMgr);

                /* Try to allocate a space again */
                tx = (rasInTx *)emaAdd(ras->inRa, NULL);
            }

            if (tx == NULL)
                status = RV_ERROR_OUTOFRESOURCES;
        }
    }
    if (status < 0)
    {
        pvtDelete(ras->hVal, propId);
        RvLogError(&ras->log,
            (&ras->log, "rasNewTx: Not enough memory to allocate a new incoming transaction"));
        return NULL;
    }
    memset(tx, 0, sizeof(rasInTx));
    tx->txProperty = propId;

    return tx;
}


/************************************************************************
 * rasLookupCall
 * purpose: Find a call matching the incoming transaction
 * input  : ras         - RAS module to use
 *          tx          - Incoming transaction
 * output : hsCall      - Call handle for the call
 *                        NULL if message is not related to any call
 *          callRelated - Indication if this transaction is call related or not
 * return : RV_OK on success (even if call wasn't found)
 *          Negative value on failure
 ************************************************************************/
static RvStatus rasLookupCall(
    IN  rasModule*  ras,
    IN  rasInTx*    tx,
    OUT HCALL*      hsCall,
    OUT RvBool*     callRelated)
{
    RVHCATCALL  hCatCall;
    catStruct   key;
    *hsCall = NULL;

    switch (tx->transactionType)
    {
        case cmRASGatekeeper:
        case cmRASRegistration:
        case cmRASUnregistration:
        case cmRASUnknown:
        case cmRASNonStandard:
        case cmRASResourceAvailability:
        case cmRASUnsolicitedIRR:
        case cmRASLocation:
            /* These are never related to calls */
            *callRelated = RV_FALSE;
            return RV_OK;

        case cmRASAdmission:
        case cmRASDisengage:
        case cmRASBandwidth:
        case cmRASInfo:
        case cmRASServiceControl:
        {
            /* Get the keys for CAT */
            if (rasCreateCatKey(ras, tx, &key) < 0)
            {
                *callRelated = RV_FALSE; /* No actual key for CAT was found */
                return RV_OK;
            }

            /* Make sure the transaction is call specific (not IRQ with CRV=0) */
            if ((tx->transactionType == cmRASInfo) && (key.rasCRV == 0x8000))
            {
                *callRelated = RV_FALSE; /* IRQ not on a specific call */
                return RV_OK;
            }

            break;
        }

        default:
            RvLogExcep(&ras->log,
                (&ras->log, "rasLookupCall: Bad transaction type %d", tx->transactionType));
            return RV_ERROR_UNKNOWN;
    }

    /* Search for the call */
    *callRelated = RV_TRUE;
    hCatCall = catFind(ras->hCat, &key);
    if (hCatCall != NULL)
    {
        /* Update the list of keys on ARQs */
        if (tx->transactionType == cmRASAdmission)
            catUpdate(ras->hCat, hCatCall, &key);

        *hsCall = catGetCallHandle(ras->hCat, hCatCall);

        /* call was found */
        return RV_OK;
    }

    if (tx->transactionType == cmRASAdmission)
    {
        /* The New call arrived */
        if (ras->evRASNewCall(ras->app, &hCatCall, &key) >= 0)
        {
            *hsCall = catGetCallHandle(ras->hCat, hCatCall);
            return RV_OK;
        }
    }

    if (tx->transactionType == cmRASServiceControl)
    {
        RvLogInfo(&ras->log,
            (&ras->log, "rasLookupCall: Can't find call for this transaction (%d)", tx->transactionType));
    }
    else
    {
        RvLogWarning(&ras->log,
            (&ras->log, "rasLookupCall: Can't find call for this transaction (%d)", tx->transactionType));
    }

    /* Even without a call for this transaction we're going to give it to the user */
    return RV_OK;
}


/************************************************************************
 * rasSendResponse
 * purpose: Sends a response message on an incoming request, whether its
 *          a confirm or a reject message
 * input  : ras             - RAS module to use
 *          tx              - Incoming transaction
 *          trStage         - Transaction stage. Indicates if it's a reject
 *                            or a confirm
 *          nodeId          - Message to send if not done through stage
 *          storeInRPOOL    - Indicate if we want to store this message
 *                            for future retransmissions
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
static int rasSendResponse(
    IN rasModule*   ras,
    IN rasInTx*     tx,
    IN cmRASTrStage trStage,
    IN int          nodeId,
    IN RvBool       storeInRPOOL)
{
    rasInTxKey* key;
    int responseNodeId;
    int status = 0;

    if (nodeId < 0)
    {
        /* No specific message - send the response in the property database */
        if (tx->responseSet == rasNoResponseSet)
        {
            /* The application didn't set any response - let's take the default one */
            responseNodeId = pvtAddBranch(ras->hVal, tx->txProperty, __q931(response));
            pvtSetTree(ras->hVal, responseNodeId, ras->hVal, ras->defaultMessages[tx->transactionType][trStage]);

            switch(trStage)
            {
            case (cmRASTrStageConfirm) : tx->responseSet = rasConfirmSet; break;
            case (cmRASTrStageReject)  : tx->responseSet = rasRejectSet; break;
            default                    : break;
            }
        }
        else
            responseNodeId = pvtGetChild(ras->hVal, tx->txProperty, __q931(response), NULL);
    }
    else
        responseNodeId = nodeId;

    if (responseNodeId < 0)
    {
        RvLogError(&ras->log,
            (&ras->log, "rasSendResponse: Problems in handling the response message (transaction %p)", tx));
        return responseNodeId;
    }

    if (trStage == cmRASTrStageConfirm)
    {
        int         destNodeId = 1;
        cmRASParam  param = (cmRASParam)-1;

        /* See if we have to create Q931 addresses in confirm messages */
        switch (tx->transactionType)
        {
            case cmRASRegistration:
                param = cmRASParamCallSignalAddress;
                __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, tx->txProperty, {_q931(response) _anyField _q931(callSignalAddress) LAST_TOKEN});
                break;

            case cmRASAdmission:
                param = cmRASParamDestCallSignalAddress;
                __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, tx->txProperty, {_q931(response) _anyField _q931(destCallSignalAddress) LAST_TOKEN});
                break;

            default:
                destNodeId = 1;
        }

        if (destNodeId < 0)
        {
            cmTransportAddress addr;
            if (cmGetLocalCallSignalAddress(ras->app, &addr) >= 0)
                rasSetParam(ras, (HRAS)tx, cmRASTrStageConfirm, param, 0, sizeof(addr), (char*)&addr);
        }
    }

    /* Set the sequence number in the response to match the one in the request */
    key = (rasInTxKey *)hashGetKey(ras->inHash, tx->hashValue);
    __pvtBuildByFieldIds(status, ras->hVal, responseNodeId, {_anyField _q931(requestSeqNum) LAST_TOKEN}, (RvInt32)(key->seqNumber), NULL);
    if (status < 0) return status;

    /* Send the message. We always send through the unicast port */
    status = rasEncodeAndSend(ras, (HRAS)tx, trStage, responseNodeId, tx->index, &tx->destAddress, storeInRPOOL, &tx->encodedMsg);
    if (status < 0)
    {
        RvLogError(&ras->log,
            (&ras->log, "rasSendResponse: Couldn't send the message %p: %d", tx, status));
        return status;
    }

    /* Change the state of this transaction */
    tx->state = rasTxStateReplySent;

    return 0;
}








/************************************************************************
 *
 *                              Public functions
 *
 ************************************************************************/


/************************************************************************
 * rasGarbageCollection
 * purpose: Clean the incoming transaction list from old transactions that
 *          are no more relevant to us.
 * input  : ras             - RAS module to use
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasGarbageCollection(
    IN rasModule*   ras)
{
    RvInt32 currTime; /* in seconds */
    rasInTx* tx;

    RvLockGet(&ras->lockGarbage, ras->logMgr);

    /* We go through the list of transactions until we find a transaction whose
       stopTime is greater than the current time - this means that this transaction
       should not be closed and so are all of the other transactions after it */
    currTime = RvInt64ToRvInt32(RvInt64Div(RvTimestampGet(ras->logMgr), RV_TIME64_NSECPERSEC));
    tx = ras->firstTx;

    /* Continue until there are no transactions left - we stop in the middle
       if we have to */
    while (tx != NULL)
    {
        if (tx->stopTime > currTime)
        {
            /* We shouldn't continue anymore */
            break;
        }

        /* Deallocate this transaction */
        ras->firstTx = tx->next;
        rasKillInTx(ras, tx);

        tx = ras->firstTx;
    }

    if (ras->firstTx == NULL)
        ras->lastTx = NULL;

    RvLockRelease(&ras->lockGarbage, ras->logMgr);
    return RV_OK;
}


/************************************************************************
 * rasIncomingHashFunc
 * purpose: Incoming transactions hash function
 *          This function returns as the hash key the actual sequence
 *          number of the transaction without fooling around with it
 * Input:   key     - Pointer the key.
 *          length  - The size of the key, in bytes.
 *          initVal - An initial 4-byte value. Can be the previous hash,
 *                    or an arbitrary value. Passed as 0 by the hash
 *                    module.
 * Return : 32bit hash result
 ************************************************************************/
RvUint32 RVCALLCONV rasIncomingHashFunc(
    IN void     *key,
    IN RvUint32 length,
    IN RvUint32 initVal)
{
    rasInTxKey* inKey = (rasInTxKey *)key;

    RV_UNUSED_ARG(length);
    RV_UNUSED_ARG(initVal);

    return addrIntoHashKey(&inKey->address, inKey->seqNumber);
}


/************************************************************************
 * rasIncomingHashCompare
 * purpose: Compare keys in the outgoing hash table
 * input  : key1, key2  - Keys to compare
 *          keySize     - Size of each key
 * return : RV_TRUE if elements are the same
 *          RV_FALSE otherwise
 ************************************************************************/
RvBool RVCALLCONV rasIncomingHashCompare(IN void *key1, IN void* key2, IN RvUint32 keySize)
{
    rasInTxKey* txKey1 = (rasInTxKey *)key1;
    rasInTxKey* txKey2 = (rasInTxKey *)key2;

    RV_UNUSED_ARG(keySize);

    return
        ((txKey1->seqNumber == txKey2->seqNumber) &&
         (txKey1->msgType == txKey2->msgType) &&
         cmCompareTransportAddresses(&txKey1->address, &txKey2->address));
}


/************************************************************************
 * rasHandleRequest
 * purpose: Handle an incoming request message
 * input  : ras             - RAS module to use
 *          index           - Index of connection this message arrived on
 *          isMulticast     - RV_TRUE if message was received no a multicast socket
 *          srcAddress      - Address of the sender
 *          messageBuf      - The message buffer to send
 *          messageLength   - The length of the message in bytes
 *          messageNodeId   - Node ID of message root. If negative, then
 *                            message is decoded from given buffer and hook
 *                            is called
 *          messageType     - Message type of the reply
 *          seqNum          - requestSeqNum field value of the message
 *          hsCall          - Call related to this transaction if it's an unsolicited IRR
 *                            NULL on any other case
 * output : hMsgContext     - Incoming message context. Used mostly by security
 *                            If the returned value is different than NULL,
 *                            then the message context is not used by the
 *                            transaction and should be released
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasHandleRequest(
    IN  rasModule*          ras,
    IN  RvInt               index,
    IN  RvBool              isMulticast,
    IN  cmTransportAddress  *srcAddress,
    IN  RvUint8*            messageBuf,
    IN  RvUint32            messageLength,
    IN  RvPvtNodeId         messageNodeId,
    IN  rasMessages         messageType,
    IN  RvUint32            seqNum,
    IN  HCALL               hsCall,
    OUT void**              hMsgContext)
{
    rasInTx*    tx;
    rasInTxKey  key;
    void*       hashValue;
    RvStatus    status;

    RvLockGet(&ras->lockInHash, ras->logMgr);

    /* Make sure we clean up the incoming database */
    status = rasGarbageCollection(ras);

    /* Find the transaction */
    memcpy(&key.address, srcAddress, sizeof(cmTransportAddress));
    key.seqNumber = seqNum;
    key.msgType = messageType;
    hashValue = hashGetElement(ras->inHash, hashFind(ras->inHash, &key));
    if (hashValue != NULL)
        tx = *((rasInTx **)hashValue);
    else
        tx = NULL;

    if (tx == NULL)
    {
        /* New message to handle */
        HAPPRAS     haRas = NULL;
        RvBool      callRelated = RV_FALSE;
        rasMessages newType;
        RvUint32    newSeqNum;
        RvBool      notifyApp = RV_TRUE;
        HAPPCALL    haCall = NULL;
        RvBool      bOurGK = RV_FALSE;

        RvLockRelease(&ras->lockInHash, ras->logMgr);

        if (!RV_PVT_NODEID_IS_VALID(messageNodeId))
        {
            /* Decode the message - it's first time we're here */
            status =
                rasDecodeAndRecv(ras, index, messageBuf, messageLength, isMulticast, srcAddress,
                                 &messageNodeId, &newType, &newSeqNum, hMsgContext);

            /* Make sure the application didn't change the sequence number or type of message */
            if ((status >= 0) && ((newSeqNum != seqNum) || (messageType != newType)))
            {
                /* Reroute the message to check it again... */
                return rasRouteMessage(ras, index, isMulticast, srcAddress, messageBuf, messageLength, messageNodeId, newType, newSeqNum, hMsgContext);
            }
        }
        else
            status = RV_OK;

        if (status == RV_OK)
        {
            /* Decode the message and put it in a new transaction */
            tx = rasNewTx(ras, messageNodeId, &messageNodeId);
        }

        if (tx == NULL)
            return RV_ERROR_UNKNOWN;

        /* Insert the key to the hash and unlock it */
        RvLockGet(&ras->lockInHash, ras->logMgr);
        tx->hashValue = hashAdd(ras->inHash, &key, &tx, RV_FALSE);
        RvLockRelease(&ras->lockInHash, ras->logMgr);

        if (tx->hashValue == NULL)
        {
            pvtDelete(ras->hVal, tx->txProperty);
            emaDelete((EMAElement)tx);
            RvLogError(&ras->log,
                (&ras->log, "rasHandleRequest: Error inserting hash value for a new incoming transaction"));
            return RV_ERROR_OUTOFRESOURCES;
        }

        /* Set known values for this transaction */
        tx->index = (RvInt16)index;
        tx->isMulticast = (RvInt8)(isMulticast != 0);
        memcpy(&tx->destAddress, srcAddress, sizeof(cmTransportAddress));
        tx->responseSet = rasNoResponseSet;
        tx->state = rasTxStateIdle;

        if (hsCall == NULL)
        {
            /* This transaction is not an unsolicited IRR - check the table for this
               transaction's type */
            tx->transactionType = rasMessageInfo[messageType].transaction;

            /* Check if the TX belongs to a call */
            status = rasLookupCall(ras, tx, &tx->hsCall, &callRelated);
        }
        else
        {
            /* We have an incoming unsolicited IRR and a request transaction */
            tx->transactionType = cmRASUnsolicitedIRR;
            tx->hsCall = hsCall;
            callRelated = RV_TRUE;
        }

        /* Lock the call object if we have one */
        if ((status != RV_OK) || (callRelated && (tx->hsCall != NULL) && !emaLock((EMAElement)tx->hsCall)))
        {
            /* Remove this transaction from the hash table */
            RvLockGet(&ras->lockInHash, ras->logMgr);
            hashDelete(ras->inHash, tx->hashValue);
            RvLockRelease(&ras->lockInHash, ras->logMgr);

            /* Kill the transaction and get the hell out of here */
            pvtDelete(ras->hVal, tx->txProperty);
            emaDelete((EMAElement)tx);
            return status;
        }

        /* Calculate the time this transaction will timeout */
        tx->stopTime = RvInt64ToRvInt32(RvInt64Div(RvTimestampGet(ras->logMgr), RV_TIME64_NSECPERSEC)) +
                       (rasCfgGetRetries(ras) * rasCfgGetTimeout(ras));

        /* Use callback to notify the application about the incoming request */
        emaMark((EMAElement)tx);

        if (ras->isGatekeeper == RV_TRUE)
        {
            /* We're practically a GK - let's receive any message */
            bOurGK = RV_TRUE;
        }
        else if (ras->epIdLen != 0)
        {
            /* make sure we are registered, not much point in answering requests if we are not */
            char msgId[256];
            RvInt32 msgIdLen = 0;
            RvPvtNodeId tmpNode;
            bOurGK = RV_TRUE;

            /* make sure the EP ID (if it exists) matches. */
            __pvtGetByFieldIds(tmpNode, ras->hVal, messageNodeId, {_anyField _q931(endpointIdentifier) LAST_TOKEN}, NULL, &msgIdLen, NULL);
            if ((tmpNode >= 0) && (msgIdLen > 0))
            {
                pvtGetString(ras->hVal, tmpNode, msgIdLen, msgId);
                if (memcmp(ras->epId, msgId, (RvSize_t)ras->epIdLen))
                    bOurGK = RV_FALSE;
            }

            /* make sure the GK ID (if it exists) matches. */
            if (bOurGK && (ras->gkIdLen > 0))
            {
                __pvtGetByFieldIds(tmpNode, ras->hVal, messageNodeId, {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN}, NULL, &msgIdLen, NULL);
                if ((tmpNode >= 0) && (msgIdLen > 0))
                {
                    pvtGetString(ras->hVal, tmpNode, msgIdLen, msgId);
                    if (memcmp(ras->gkId, msgId, (RvSize_t)ras->gkIdLen))
                        bOurGK = RV_FALSE;
                }
            }
        }

        if (bOurGK)
        {
            /* Make sure we update the registration information if we have to */
            if (messageType == rasMsgUnregistrationRequest)
            {
                rasUpdateRegInfo(ras, -1);
            }

            if (callRelated && (tx->hsCall != NULL))
            {
                haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)tx->hsCall);
                emaUnlock((EMAElement)tx->hsCall);

                /* For IRQ - we set the perCallInfo inside IRR by the call information */
                if (tx->transactionType == cmRASInfo)
                {
                    RvPvtNodeId irrNode = pvtAdd(ras->hVal, tx->txProperty, __q931(response), 0, NULL, NULL);
                    rasSetIrrFields(ras, (HRAS)tx, irrNode, tx->hsCall);
                }

                if (ras->evCallRequest != NULL)
                {
                    /* Associate message context with transaction */
                    tx->hMsgContext = *hMsgContext;
                    *hMsgContext = NULL;

                    notifyApp = !ras->evCallRequest(ras->app,
                                                    (HRAS)tx,
                                                    tx->hsCall,
                                                    tx->transactionType,
                                                    srcAddress,
                                                    haCall,
                                                    ras->evAutoRas.cmEvAutoRASRequest);
                }
            }
            else
            {
                if (ras->evEpRequest != NULL)
                {
                    /* Associate message context with transaction */
                    tx->hMsgContext = *hMsgContext;
                    *hMsgContext = NULL;
                    notifyApp = !ras->evEpRequest(ras->app,
                                                  (HRAS)tx,
                                                  tx->transactionType,
                                                  srcAddress,
                                                  ras->evAutoRas.cmEvAutoRASRequest);
                }
            }
        }
        else
        {
            if (callRelated && (tx->hsCall != NULL))
                emaUnlock((EMAElement)tx->hsCall);
            RvLogWarning(&ras->log,
                (&ras->log, "rasHandleRequest: Incoming request when not registered or from another GK"));
        }

        if (notifyApp)
        {
            if (ras->evApp.cmEvRASRequest != NULL)
            {
                /* Associate message context with transaction */
                tx->hMsgContext = *hMsgContext;
                *hMsgContext = NULL;
                cmiCBEnter((ras->app, "cmEvRASRequest(hsRas=%p,hsCall=%p,trans=%d)", tx, tx->hsCall, tx->transactionType));
                ras->evApp.cmEvRASRequest((HRAS)tx,
                                          tx->hsCall,
                                          &haRas,
                                          tx->transactionType,
                                          srcAddress,
                                          haCall);
                cmiCBExit((ras->app, "cmEvRASRequest(hsRas=%p,haRas=%p)", tx, haRas));

                /* Update the application's handle */
                emaSetApplicationHandle((EMAElement)tx, (void*)haRas);
            }
            else
            {
                /* Automatic RAS mode, and there's no callback for manual RAS messages.
                   We should kill this transaction. */
                status = rasCloseInTx(ras, tx);
            }
        }
        else
        {
            tx->isAutomaticRAS = RV_TRUE;
        }

        /* Release the transaction when we're done */
        emaRelease((EMAElement)tx);
    }
    else if (emaLock((EMAElement)tx))
    {
        /* This is a retransmission - see if we have a reply for it */
        RvLockRelease(&ras->lockInHash, ras->logMgr);

        switch (tx->state)
        {
            case rasTxStateReplySent:
                /* Retransmit the reply to the sender */
                status = rasRetransmit(ras, (HRAS)tx, tx->encodedMsg, tx->index, &tx->destAddress, "response");
                break;

            case rasTxStateRipSent:
            {
                RvInt timeLeft =
                    RvInt64ToRvInt(
                        RvInt64Sub(RvInt64Mul(RvInt64FromRvInt32(tx->ripStopTime), RvInt64Const(1, 0, 1000)),
                            RvInt64Div(RvTimestampGet(ras->logMgr), RV_TIME64_NSECPERMSEC)));
                if (timeLeft > 0)
                {
                    /* Let's send a RIP retransmission after calculating the delay */
                    status = rasSendRIP(ras, tx, timeLeft, RV_FALSE);
                }
                else
                    status = RV_ERROR_UNKNOWN;
                break;
            }

            default:
                /* Do nothing - retransmission of something that the application didn't
                   Reply to yet */
                RvLogDebug(&ras->log,
                    (&ras->log, "rasHandleRequest: Retrasmission ignored for transaction %p", tx));
        }

        emaUnlock((EMAElement)tx);

        pvtDelete(ras->hVal, messageNodeId);
    }
    else
    {
        RvLockRelease(&ras->lockInHash, ras->logMgr);
    }

    return status;
}


/************************************************************************
 * rasSendConfirmMessage
 * purpose: Sends a confirm response on an incoming RAS request
 * input  : ras             - RAS module to use
 *          tx              - Incoming transaction
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int rasSendConfirmMessage(
    IN rasModule*   ras,
    IN rasInTx*     tx)
{
    int status = RV_ERROR_UNKNOWN;

    /* Lock the transaction before dealing with it */
    if(emaLock((EMAElement)tx))
    {
        status = rasSendResponse(ras, tx, cmRASTrStageConfirm, -1, RV_TRUE);

        /* Unlock and release the transaction when we're done */
        emaUnlock((EMAElement)tx);
    }

    return status;
}


/************************************************************************
 * rasSendRejectMessage
 * purpose: Sends a reject response on an incoming RAS request
 * input  : ras             - RAS module to use
 *          tx              - Incoming transaction
 *          reason          - The reject reason to use
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int rasSendRejectMessage(
    IN rasModule*   ras,
    IN rasInTx*     tx,
    IN cmRASReason  reason)
{
    int status = RV_ERROR_UNKNOWN;

    /* Lock the transaction before dealing with it */
    if(emaLock((EMAElement)tx))
    {
        if (reason == cmRASReasonUnknownMessageResponse)
        {
            RvPvtNodeId nodeId;

            /* Create an XRS message */
            __pvtBuildByFieldIds(nodeId, ras->hVal, tx->txProperty,
                {_q931(response) _q931(unknownMessageResponse) LAST_TOKEN}, 0, NULL);
            tx->responseSet = rasRejectSet;

            if (!RV_PVT_NODEID_IS_VALID(nodeId))
                status = (int)nodeId;
        }
        else
        {
            /* Set the reject reason of this message */
            status = rasSetParam(ras, (HRAS)tx, cmRASTrStageReject, cmRASParamRejectReason, 0, (RvInt32)reason, NULL);
        }

        status = rasSendResponse(ras, tx, cmRASTrStageReject, -1, RV_TRUE);

        /* Unlock and release the transaction when we're done */
        emaUnlock((EMAElement)tx);
    }

    return status;
}


/************************************************************************
 * rasSendRIP
 * purpose: Sends a RIP response on an incoming RAS request
 * input  : ras     - RAS module to use
 *          tx      - Incoming transaction
 *          delay   - Delay for RIP message in milliseconds
 *          updateStopTime  - Indicate if we're updating the stop time
 *                            and increasing the transaction's time or not
 * output : none
 * return : If an error occurs, the function returns RV_OK.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RvStatus rasSendRIP(
    IN rasModule*   ras,
    IN rasInTx*     tx,
    IN int          delay,
    IN RvBool       updateStopTime)
{
    RvStatus status;
    RvPvtNodeId nodeId, progressNodeId;

    /* Make sure delay is within range */
    if ((delay < 1) || (delay > 65535))
    {
        RvLogError(&ras->log,
            (&ras->log, "rasSendRIP: Delay %d out of range for tx=%p (1..65535)", delay, tx));
        return RV_ERROR_UNKNOWN;
    }

    /* Build the RIP message */
    nodeId = pvtAddRoot(ras->hVal, ras->synMessage, 0, NULL);
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return nodeId;
    __pvtBuildByFieldIds(status, ras->hVal, nodeId, {_q931(requestInProgress) _q931(delay) LAST_TOKEN}, delay, NULL);
    if (status < 0)
    {
        pvtDelete(ras->hVal, nodeId);
        return status;
    }

    /* Lock the transaction before dealing with it */
    status = RV_ERROR_UNKNOWN;
    if (emaLock((EMAElement)tx))
    {
        /* Link the RIP message to the transaction's database */
        progressNodeId = pvtAdd(ras->hVal, tx->txProperty, __q931(progress), 0, NULL, NULL);
        status = pvtMoveTree(ras->hVal, progressNodeId, nodeId);
        if (status < 0)
            pvtDelete(ras->hVal, nodeId);

        /* Send this message */
        if (status >= 0)
            status = rasSendResponse(ras, tx, cmRASTrStageConfirm, progressNodeId, RV_FALSE);

        if (status == RV_OK)
        {
            tx->state = rasTxStateRipSent;

            if (updateStopTime)
            {
                /* Recalculate the time this transaction will timeout */
                RvInt32 secDelay = (delay + 999) / 1000;
                tx->stopTime += secDelay;
                tx->ripStopTime = RvInt64ToRvInt32(RvInt64Div(RvTimestampGet(NULL), RV_TIME64_NSECPERSEC)) +
                                  secDelay; /* Our timeouts for the RAS transactions are calculated in seconds and not milliseconds... */
            }
        }

        /* Unlock and release the transaction when we're done */
        emaUnlock((EMAElement)tx);
    }

    return status;
}


/************************************************************************
 * rasCloseInTx
 * purpose: Close an incoming transaction
 *          This won't actually free resources, only mark the transaction
 *          as a possibility for removal in the garbage collection.
 * input  : ras             - RAS module to use
 *          tx              - Incoming transaction to close
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasCloseInTx(
    IN rasModule*       ras,
    IN rasInTx*         tx)
{
    RvPvtNodeId nodeId = RV_PVT_INVALID_NODEID;
    void* hMsgContext = NULL;

    RvLockGet(&ras->lockGarbage, ras->logMgr);
    if(!emaLock((EMAElement)tx))
    {
        RvLockRelease(&ras->lockGarbage, ras->logMgr);
        return RV_ERROR_DESTRUCTED;
    }
    /* make sure we do not add this to the garbage disposal twice */
    if (tx->bGarbage)
    {
        RvLogExcep(&ras->log,
            (&ras->log, "rasCloseInTx: incoming transaction (%p) deleted wtice", tx));
        emaUnlock((EMAElement)tx);
        RvLockRelease(&ras->lockGarbage, ras->logMgr);
        return RV_ERROR_DESTRUCTED;
    }

    /* Set garbage collection list to match the required order */
    if (ras->lastTx != NULL)
        ras->lastTx->next = tx;
    ras->lastTx = tx;
    if (ras->firstTx == NULL)
        ras->firstTx = tx;
    tx->bGarbage = RV_TRUE;

    nodeId = tx->txProperty;
    tx->txProperty = RV_PVT_INVALID_NODEID;
    hMsgContext = tx->hMsgContext;

    emaUnlock((EMAElement)tx);
    RvLockRelease(&ras->lockGarbage, ras->logMgr);

    /* Free some nodes in the PVT db */
    pvtDelete(ras->hVal, nodeId);

    /* Dispose of the message context for this transaction if we have any */
    if (hMsgContext != NULL)
        ras->cmiEvRASReleaseMessageContext(hMsgContext);

    return RV_OK;
}



#ifdef __cplusplus
}
#endif


