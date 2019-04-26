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
#include "cmutils.h"
#include "cmdebprn.h"
#include "rasutils.h"
#include "rasparams.h"
#include "rasout.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 *
 *                              Private functions
 *
 ************************************************************************/


/************************************************************************
 * rasTimeoutEvent
 * purpose: Callback function that is called when the timeout expires
 *          on an outgoing transaction message, making sure that a
 *          retransmission on this transaction will occur if necessary.
 * input  : context - Context to use
 *                    This is the outgoing transaction object
 * output : none
 * return : none
 ************************************************************************/
static RvBool rasTimeoutEvent(IN void* context)
{
    rasModule*          ras;
    rasOutTx*           tx = (rasOutTx *)context;
    HRPOOLELEM          message;
    int                 retryCount;
    RvTimer*            timer = NULL;
    RvBool              cont = RV_TRUE;

    /* Lock the transaction so no one will close it */
    if (emaLock((EMAElement)tx))
    {
        /* This element exists... */
        ras = (rasModule *)emaGetUserData((EMAElement)tx);

        cmCallPreCallBack((HAPP)ras->app);

        RvLogDebug(&ras->log,
            (&ras->log, "rasTimeoutEvent: On outTx=%p, retries=%d", tx, tx->retryCount));

        /* Decrease the retry count */
        tx->retryCount--;

        /* maybe we're an EP and the GK changed, see if this is a transaction to the GK */
        if (!ras->isGatekeeper && 
            (tx->transactionType != cmRASNonStandard) &&
            (tx->transactionType != cmRASUnknown) &&
            (tx->transactionType != cmRASServiceControl))
        {
            cmTransportAddress gkAddress;
            cmVt2TransportAddress(ras->hVal, ras->gatekeeperRASAddress, &gkAddress);
            /* check if the GK changed since we sent this transaction */
            if (!isSameAddress(&tx->destAddress, &gkAddress))
            {
                /* GK changed. reset the retransmit counter and try again */
                memcpy(&tx->destAddress, &gkAddress, sizeof(cmTransportAddress));
                tx->retryCount = rasCfgGetRetries(ras);
            }
        }

        if (tx->retryCount == 0)
        {
            tx->state = rasTxStateTimedOut;
            timer = tx->timer;
            tx->timer = NULL;
        }

        /* Make sure we have all the necessary parameters */
        retryCount = tx->retryCount;
        message = tx->encodedMsg;

        if (retryCount == 0)
        {
            /* Transaction has timed out */
            cmiEvRASResponseT   evResponse;

            /* Stop the timer - we don't need it anymore */
            RvH323TimerClear(ras->timers, &timer);
            cont = RV_FALSE;

            evResponse = tx->evResponse;

            /* Unlock it before we're done */
            emaMark((EMAElement)tx);
            emaUnlock((EMAElement)tx);

            /* See which of the response callbacks we have to call */
            if (evResponse != NULL)
            {
                if (ras->evAutoRas.cmEvAutoRASTimeout != NULL)
                {
                    cmiCBEnter((ras->app, "cmEvAutoRASTimeout(hsRas=%p)", tx));
                    ras->evAutoRas.cmEvAutoRASTimeout((HRAS)tx);
                    cmiCBExit((ras->app, "cmEvAutoRASTimeout(hsRas=%p)", tx));
                }

                if (!emaWasDeleted((EMAElement)tx))
                    evResponse((HAPPRAS)emaGetApplicationHandle((EMAElement)tx), (HRAS)tx, cmRASTrStageTimeout);
            }
            else if (ras->evApp.cmEvRASTimeout != NULL)
            {
                HAPPRAS haRas = (HAPPRAS)emaGetApplicationHandle((EMAElement)tx);
                cmiCBEnter((ras->app, "cmEvRASTimeout(haRas=%p,hsRas=%p)", haRas, tx));
                ras->evApp.cmEvRASTimeout(haRas, (HRAS)tx);
                cmiCBExit((ras->app, "cmEvRASTimeout(haRas=%p,hsRas=%p)", haRas, tx));
            }

            emaRelease((EMAElement)tx);
        }
        else if (message != NULL)
        {
            /* Retransmit */
            if (tx->state == rasTxStateRipReceived)
            {
                RvInt32 timeout;

                /* Reset the timer for regular intervals after RIP */
                RvH323TimerClear(ras->timers, &tx->timer);
                cont = RV_FALSE;
                timeout = ((RvInt)tx->timeout) * 1000;
                tx->timer = RvH323TimerStart(ras->timers, rasTimeoutEvent, tx, timeout);
            }
			/* Send the message */
			rasRetransmit(ras, (HRAS)tx, message, tx->index, &tx->destAddress, "request");
            /* Unlock it before we're done */
            emaUnlock((EMAElement)tx);
        }
        else
        {
            /* No need to leave it locked */
            emaUnlock((EMAElement)tx);
        }
    }
    return cont;
}


/************************************************************************
 * rasHandleIncomingRIP
 * purpose: Handle an incoming RIP on an outgoing request
 * input  : ras             - RAS module to use
 *          tx              - Outgoing transaction we're dealing with
 *          messageNodeId   - Node ID of the RIP message
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
static RvStatus rasHandleIncomingRIP(
    IN rasModule*   ras,
    IN rasOutTx*    tx,
    IN RvPvtNodeId  messageNodeId)
{
    RvInt32 delay; /* in ms */

    /* Make sure we're not in a multicast transaction */
    if (!tx->isMulticast)
    {
        /* Find the timeout */
        pvtGetChildByFieldId(ras->hVal, pvtChild(ras->hVal, messageNodeId), __q931(delay), &delay, NULL);

        /* Set the timeout to its new value */
        RvH323TimerCancel(ras->timers, &tx->timer);
        tx->timer = RvH323TimerStart(ras->timers, rasTimeoutEvent, tx, delay);
        tx->state = rasTxStateRipReceived;
    }
    else
    {
        RvLogWarning(&ras->log,
            (&ras->log, "rasHandleIncomingRIP: outTx=%p is multicast. Ignoring RIP message", tx));
    }

    /* Delete the message and return */
    pvtDelete(ras->hVal, messageNodeId);
    return RV_OK;
}






/************************************************************************
 *
 *                              Public functions
 *
 ************************************************************************/


/************************************************************************
 * rasOutgoingHashFunc
 * purpose: Outgoing transactions hash function
 *          This function returns as the hash key the actual sequence
 *          number of the transaction without fooling around with it
 * Input:   key     - Pointer the key.
 *          length  - The size of the key, in bytes.
 *          initVal - An initial 4-byte value. Can be the previous hash,
 *                    or an arbitrary value. Passed as 0 by the hash
 *                    module.
 * Return : 32bit hash result
 ************************************************************************/
RvUint32 RVCALLCONV rasOutgoingHashFunc(
    IN void     *key,
    IN RvUint32 length,
    IN RvUint32 initVal)
{
    RV_UNUSED_ARG(length);
    RV_UNUSED_ARG(initVal);
    return (*(RvUint32*)key);
}





/************************************************************************
 * rasCreateOutTx
 * purpose: Create an outgoing transaction and return its pointer
 * input  : ras         - RAS module to use
 *          haRas       - Application's handle for the RAS transaction
 *          transaction - The transaction type we want to start
 *          destAddress - Address of the destination.
 *                        If set to NULL, then it's for the gatekeeper
 * output : none
 * return : Pointer to an outgoing RAS transaction on success
 *          NULL on failure
 ************************************************************************/
rasOutTx* rasCreateOutTx(
    IN rasModule            *ras,
    IN HAPPRAS              haRas,
    IN cmRASTransaction     transaction,
    IN cmTransportAddress   *destAddress)
{
    rasOutTx*   tx;
    int         srcNodeId, requestNode;
    int         status; /* = 0 if no errors occured */

    /* Make sure we're working with outgoing transactions */
    if (ras->outRa == NULL)
        return NULL;

    /* Create the RAS transaction */
    tx = (rasOutTx *)emaAdd(ras->outRa, (void*)haRas);
    if (tx == NULL)
        return NULL;
    memset(tx, 0, sizeof(rasOutTx));

    /* Grab a node as root for the property db */
    tx->txProperty = pvtAddRoot(ras->hVal, ras->synProperty, 0, NULL);
    if (!RV_PVT_NODEID_IS_VALID(tx->txProperty))
    {
        emaLock((EMAElement)tx);
        emaDelete(tx);
        emaUnlock((EMAElement)tx);
        return NULL;
    }

    /* Set some of the transaction's values */
    tx->index           = 1;
    tx->isMulticast     = RV_FALSE;
    tx->transactionType = transaction;
    tx->state           = rasTxStateIdle;
    tx->timer           = NULL;

    /* Set the destination address for the transaction */
    status = pvtAdd(ras->hVal, tx->txProperty, __q931(address), 0, NULL, NULL);
    if ((destAddress == NULL) && (status >= 0))
    {
        /* Take it from configuration - the gatekeeper's address */

        srcNodeId = ras->gatekeeperRASAddress;
        if (ras->gatekeeperRASAddress < 0)
            __pvtGetNodeIdByFieldIds(srcNodeId, ras->hVal, ras->confNode, {_q931(manualDiscovery) _q931(defaultGatekeeper) LAST_TOKEN});

        /* Save the GK RAS address*/
        if ((ras->gatekeeperRASAddress < 0) && (srcNodeId >= 0))
        {
            ras->gatekeeperRASAddress = pvtAddRoot(ras->hVal, NULL, 0, NULL);
            pvtSetTree(ras->hVal,ras->gatekeeperRASAddress,ras->hVal,srcNodeId);
        }
        status = 0;
        if (srcNodeId >= 0)
            status = cmVtToTA(ras->hVal, srcNodeId, &tx->destAddress);
        else
            memset(&(tx->destAddress),0,sizeof(cmTransportAddress));
    }
    else if (destAddress != NULL)
    {
        /* Get the address the user gave us */
        tx->destAddress = *destAddress;
    }

    /* Build the default request message */
    if (status >= 0)
    {
        requestNode = pvtAdd(ras->hVal, tx->txProperty, __q931(request), 0, NULL, NULL);
        status = pvtSetTree(ras->hVal, requestNode, ras->hVal, ras->defaultMessages[transaction][cmRASTrStageRequest]);
    }

    /* Calculate the number of retries and timeout for this transaction */
    tx->retryCount = rasCfgGetRetries(ras);
    tx->timeout = rasCfgGetTimeout(ras);

    if (status < 0)
    {
        pvtDelete(ras->hVal, tx->txProperty);
        tx->txProperty = RV_PVT_INVALID_NODEID;
        emaLock((EMAElement)tx);
        emaDelete(tx);
        emaUnlock((EMAElement)tx);
        return NULL;
    }
    return tx;
}


/************************************************************************
 * rasSendRequestMessage
 * purpose: Send an outgoing request message
 * input  : ras             - RAS module to use
 *          tx              - Outgoing transaction to send
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasSendRequestMessage(
    IN rasModule*       ras,
    IN rasOutTx*        tx)
{
    RvUint32    reqSeqNum;
    int         nodeId, reqNodeId;
    int         status = 0;

    /* Set the sequence number of the transaction */
    RvLockGet(&ras->lockOutHash, ras->logMgr);
    if (ras->requestSeqNum == 65535)
        ras->requestSeqNum = 2;
    else
        ras->requestSeqNum++;
    reqSeqNum = ras->requestSeqNum;

    /* Update the outgoing hash table with this transaction */
    if (tx->hashValue != NULL)
        hashDelete(ras->outHash, tx->hashValue);
    tx->hashValue = hashAdd(ras->outHash, &reqSeqNum, &tx, RV_TRUE);
    if (tx->hashValue == NULL)
    {
        RvLogError(&ras->log,
            (&ras->log, "rasSendRequestMessage: Cannot insert to hash %p (requestSeqNum=%d)",
                 tx, reqSeqNum));
        status = RV_ERROR_UNKNOWN;
    }

    /* Lock the transaction and then unlock the hash */
    if (status >= 0)
        if(!emaLock((EMAElement)tx))
            status = RV_ERROR_UNKNOWN;
    RvLockRelease(&ras->lockOutHash, ras->logMgr);
    if (status < 0)
        return status;

    /* Make sure timer is released */
    RvH323TimerCancel(ras->timers, &tx->timer);

    /* Get the request message */
    reqNodeId = pvtGetChild(ras->hVal, tx->txProperty, __q931(request), NULL);
    if (reqNodeId < 0) status = reqNodeId;

    /* Set the sequence number inside the outgoing request message */
    __pvtBuildByFieldIds(nodeId, ras->hVal, reqNodeId, {_anyField _q931(requestSeqNum) LAST_TOKEN}, (RvInt32)reqSeqNum, NULL);

    /* Mark transaction before using any of the callbacks */
    if (status >= 0)
	{
		//{{gaoshizhong 20140423 use to mcu reg more gk account
		RvInt manualRAS =0;
		int status =0;
		status = pvtGetChildByFieldId(ras->hVal, ras->confNode, __q931(manualRAS), &manualRAS, NULL);
		if(status >=0 && manualRAS  > 0 )
			status = rasEncodeAndSend(ras, (HRAS)tx, cmRASTrStageRequest, reqNodeId, tx->index, &tx->destAddress, RV_FALSE, &tx->encodedMsg);
		else
			status = rasEncodeAndSend(ras, (HRAS)tx, cmRASTrStageRequest, reqNodeId, tx->index, &tx->destAddress, RV_TRUE, &tx->encodedMsg);
		//}}gaoshizhong 20140423 use to mcu reg more gk account
	}

    if (status >= 0)
    {
        /* No one closed this transaction - set the timer and finish with it */

        switch (tx->transactionType)
        {
            case cmRASNonStandard:
                /* Non-associated transaction */
                tx->state = rasTxStateIdle;
                tx->retryCount = 0;
                break;

            case cmRASUnsolicitedIRR:
            {
                /* Check if this IRR is waiting for a reply of some kind */
                RvInt32 needResponse;
                __pvtGetByFieldIds(nodeId, ras->hVal, reqNodeId, {_anyField _q931(needResponse) LAST_TOKEN}, NULL, &needResponse, NULL);
                if ((!needResponse) || (nodeId < 0))
                {
                    /* We're not looking for a response - change state and end switch */
                    tx->state = rasTxStateIdle;
                    tx->retryCount = 0;
                    break;
                }

            }
                /* If we got here, then we should wait for the
                   response as the rest of the messages: fall through to default... */

            default:
            {
                RvInt timeout;
                /* We will be waiting for a reply... */
                tx->state = rasTxStateRequestSent;
                /* Set the timer for retries */
                timeout = ((RvInt)tx->timeout) * 1000;
                tx->timer = RvH323TimerStartPeriodic(ras->timers, rasTimeoutEvent, tx, timeout);
            }
        }
    }

    emaUnlock((EMAElement)tx);

    return status;
}


/************************************************************************
 * rasDummyRequest
 * purpose: Handle incoming unsolicited IRRs as responses
 * input  : ras             - RAS module to use
 *          tx              - Outgoing transaction to send
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasDummyRequest(
    IN rasModule*       ras,
    IN rasOutTx*        tx)
{
    /* We only act as if a message was sent so we'll be waiting for responses */

    /* Make sure it's an unsolicited IRR transaction */
    if ((tx->transactionType != cmRASUnsolicitedIRR) || (tx->hsCall == NULL))
    {
        RvLogError(&ras->log,
            (&ras->log, "rasDummyRequest: Not an unsolicited IRR transaction (tx=%p)", tx));
        return RV_ERROR_UNKNOWN;
    }

    if(emaLock((EMAElement)tx))
    {
        tx->state = rasTxStateRequestSent;
        emaUnlock((EMAElement)tx);
    }

    /* Make sure we have this handle in the CAT */
    return catSetUnsolicitedIRR(ras->hCat, cmiGetCatForCall(tx->hsCall), (HRAS)tx);
}


/************************************************************************
 * rasHandleReply
 * purpose: Handle a reply of an outgoing request message
 * input  : ras             - RAS module to use
 *          index           - Index of the local RAS address message was received from
 *          srcAddress      - Address of the sender
 *          messageBuf      - The message buffer to send
 *          messageLength   - The length of the message in bytes
 *          messageNodeId   - Node ID of message root. If negative, then
 *                            message is decoded from given buffer and hook
 *                            is called
 *          messageType     - Message type of the reply
 *          seqNum          - requestSeqNum field value of the message
 * output : hMsgContext     - Incoming message context. Used mostly by security
 *                            If the returned value is different than NULL,
 *                            then the message context is not used by the
 *                            transaction and should be released
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasHandleReply(
    IN  rasModule           *ras,
    IN  RvInt               index,
    IN  cmTransportAddress  *srcAddress,
    IN  RvUint8             *messageBuf,
    IN  RvUint32            messageLength,
    IN  RvPvtNodeId         messageNodeId,
    IN  rasMessages         messageType,
    IN  RvUint32            seqNum,
    OUT void                **hMsgContext)
{
    rasOutTx*       tx;
    void*           hashValue;
    RvStatus        status = RV_OK;

    /* Lock hash to find the transaction */
    RvLockGet(&ras->lockOutHash, ras->logMgr);

    /* Look for this transaction */
    hashValue = hashGetElement(ras->outHash, hashFind(ras->outHash, &seqNum));
    if (hashValue != NULL)
        tx = *((rasOutTx **)hashValue);
    else
        tx = NULL;
    if (tx == NULL)
    {
        RvLogWarning(&ras->log,
            (&ras->log, "rasHandleReply: Transaction not found for seqNum=%d", seqNum));
        if (messageNodeId >= 0)
            pvtDelete(ras->hVal, messageNodeId);
        RvLockRelease(&ras->lockOutHash, ras->logMgr);
        return RV_ERROR_UNKNOWN;
    }

    /* Lock the transaction and unlock the hash table */
    emaLock((EMAElement)tx);
    RvLockRelease(&ras->lockOutHash, ras->logMgr);

    /* Make sure we're still waiting for a reply of some kind */
    if (!((tx->state == rasTxStateRequestSent) || (tx->state == rasTxStateRipReceived) ||
          ((tx->isMulticast == RV_TRUE) && (tx->state == rasTxStateReplyReceived))))
    {
        RvLogWarning(&ras->log,
            (&ras->log, "rasHandleReply: Transaction %p not waiting for replies. Reply is discarded", tx));
        status = RV_ERROR_UNKNOWN;
    }

    /* Make sure this message belongs to the transaction */
    if ((status == RV_OK) && (tx->transactionType != rasMessageInfo[messageType].transaction))
    {
        switch (messageType)
        {
            case rasMsgUnknownMessageResponse:
            case rasMsgRequestInProgress:
                /* We're fine - let these messages continue */
                break;
            default:
                RvLogWarning(&ras->log,
                    (&ras->log, "rasHandleReply: Reply (%d) doesn't match this transaction type (%d)", messageType, tx->transactionType));
                status = RV_ERROR_UNKNOWN;
        }
    }

    if (!RV_PVT_NODEID_IS_VALID(messageNodeId) && (status == RV_OK))
    {
        /* We're in business - decode the whole message if we have to */
        rasMessages newType;
        RvUint32    newSeqNum;
        int         nesting;

        nesting = emaPrepareForCallback((EMAElement)tx);
        status =
            rasDecodeAndRecv(ras, index, messageBuf, messageLength, RV_FALSE, srcAddress,
                             &messageNodeId, &newType, &newSeqNum, hMsgContext);
        emaReturnFromCallback((EMAElement)tx, nesting);

        /* Make sure the application didn't change the sequence number or type of message */
        if ((status >= 0) && ((newSeqNum != seqNum) || (messageType != newType)))
        {
            /* Reroute the message to check it again... */
            emaUnlock((EMAElement)tx);
            return rasRouteMessage(ras, tx->index, RV_FALSE, srcAddress, messageBuf, messageLength, messageNodeId, newType, newSeqNum, hMsgContext);
        }
    }

    if (status < 0)
    {
        /* Probably couldn't decode this message... */
        if (RV_PVT_NODEID_IS_VALID(messageNodeId))
            pvtDelete(ras->hVal, messageNodeId);
        emaUnlock((EMAElement)tx);
        return status;
    }

    if (messageType == rasMsgRequestInProgress)
    {
        /* We have a RIP - handle it differently */
        status = rasHandleIncomingRIP(ras, tx, messageNodeId);
        emaUnlock((EMAElement)tx);
        return status;
    }

    /* We found the transaction - start handling it properly */
    /* - No need to unlock the element - rasHandleTxResponse does that automatically */
    return rasHandleTxResponse(ras, tx, messageNodeId, messageType, hMsgContext);
}


/************************************************************************
 * rasHandleTxResponse
 * purpose: Handle a reply of an outgoing request message, when we already
 *          know the exact transaction. This function is used internally
 *          by rasHandleReply() and when we have an unsolicited IRR on
 *          a dummy request.
 *          This function doesn't lock the transaction when handling it.
 *          It assumes that the transaction is already locked from somewhere
 *          else. It will unlock the transaction when finished.
 * input  : ras             - RAS module to use
 *          tx              - Transaction we're dealing with
 *          messageNodeId   - Node ID of message root. If negative, then
 *                            message is decoded from given buffer and hook
 *                            is called
 *          messageType     - Message type of the reply
 * output : hMsgContext     - Incoming message context. Used mostly by security
 *                            If the returned value is different than NULL,
 *                            then the message context is not used by the
 *                            transaction and should be released
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasHandleTxResponse(
    IN  rasModule*      ras,
    IN  rasOutTx*       tx,
    IN  RvPvtNodeId     messageNodeId,
    IN  rasMessages     messageType,
    OUT void**          hMsgContext)
{
    HAPPRAS         haRas;
    RvPvtNodeId     rootId;
    RvStatus        status;
    cmRASTrStage    stage;

    if (rasMessageInfo[messageType].msgType == rasTxMsgConfirm)
        stage = cmRASTrStageConfirm;
    else
        stage = cmRASTrStageReject;

    /* Add the decoded message to the transaction's DB */
    rootId = pvtAdd(ras->hVal, tx->txProperty, __q931(response), 0, NULL, NULL);
    status = pvtMoveTree(ras->hVal, rootId, messageNodeId);
    messageNodeId = rootId;

    /* If we're in multicast, don't cancel the timer just yet, more answers may come in */
    if (!tx->isMulticast)
    {
        /* Make sure timer is released */
        RvH323TimerCancel(ras->timers, &tx->timer);
    }

    /* Update the transaction's state only if it's not multicast.
       For multicast transactions we want to receive any incoming message */
    if ((status >= 0) && (!tx->isMulticast))
        tx->state = rasTxStateReplyReceived;

    /* Mark the transaction before the callback and unlock it */
    emaMark((EMAElement)tx);

    emaUnlock((EMAElement)tx);

    if (status >= 0)
    {
        haRas = (HAPPRAS)emaGetApplicationHandle((EMAElement)tx);

        /* If we get GCF then save the GK RAS address*/
        if ((messageType == rasMsgGatekeeperConfirm) && (tx->evResponse == NULL))
        {
            /* only if manual RAS */
            RvPvtNodeId srcNodeId;
            if (ras->gatekeeperRASAddress < 0)
                ras->gatekeeperRASAddress = pvtAddRoot(ras->hVal, NULL, 0, NULL);
            __pvtGetNodeIdByFieldIds(srcNodeId, ras->hVal, messageNodeId, {_anyField _q931(rasAddress) LAST_TOKEN});
            pvtSetTree(ras->hVal, ras->gatekeeperRASAddress, ras->hVal, srcNodeId);
        }

        /* Make sure we update the registration information if we have to */
        if (messageType == rasMsgRegistrationConfirm)
        {
            if (tx->evResponse == NULL)
            {
                /* only if manual RAS */
                RvPvtNodeId srcNodeId;

                /* get the GK address */
                if (ras->gatekeeperRASAddress < 0)
                    ras->gatekeeperRASAddress = pvtAddRoot(ras->hVal, ((cmElem*)(ras->app))->hAddrSyn, 0, NULL);
                cmTAToVt(ras->hVal, ras->gatekeeperRASAddress, &tx->destAddress);

                /* also get the GK ID */
                __pvtGetByFieldIds(srcNodeId, ras->hVal, messageNodeId, {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN}, NULL, (RvInt32 *)&ras->gkIdLen, NULL);
                if (RV_PVT_NODEID_IS_VALID(srcNodeId))
                    ras->gkIdLen = pvtGetString(ras->hVal, srcNodeId, sizeof(ras->gkId), ras->gkId);
                else
                    ras->gkIdLen = 0;
            }

            rasUpdateRegInfo(ras, messageNodeId);
        }
        if (messageType == rasMsgUnregistrationConfirm)
        {
            /* Get the original request */
            int requestId = pvtGetChild(ras->hVal, tx->txProperty, __q931(request), NULL);
            int nodeId;
            /* now see if it was a partial unregistration */
            __pvtGetNodeIdByFieldIds(nodeId, ras->hVal, requestId , {_q931(unregistrationRequest) _q931(endpointAlias) LAST_TOKEN});

            if (!RV_PVT_NODEID_IS_VALID(nodeId))
                /* it was a full unregistration */
                rasUpdateRegInfo(ras, -1);
            else
                /* just remove these aliases */
                rasUpdatePartUnreg(ras, requestId);
        }
        /* Check if we have an automatic RAS callback - if we do, this will
           override any manual RAS callback... */

        /* Automatic RAS callback */
        if (tx->evResponse != NULL)
        {
            /* We should override the manual RAS callbacks - the automatic RAS should decide
               if it should be called */

            /* Associate message context with transaction */
            tx->hMsgContext = *hMsgContext;
            *hMsgContext = NULL;

            /* Callback... */
            if ((rasMessageInfo[messageType].msgType == rasTxMsgConfirm) && (ras->evAutoRas.cmEvAutoRASConfirm != NULL))
            {
                cmiCBEnter((ras->app, "cmEvAutoRASConfirm(hsRas=%p)", tx));
                ras->evAutoRas.cmEvAutoRASConfirm((HRAS)tx);
                cmiCBExit((ras->app, "cmEvAutoRASConfirm(hsRas=%p)", tx));
            }
            else if ((rasMessageInfo[messageType].msgType == rasTxMsgReject) && (ras->evAutoRas.cmEvAutoRASReject != NULL))
            {
                /* Find out the reject reason and then set the callback */
                RvInt32 reason;
                if (messageType == rasMsgUnknownMessageResponse)
                    reason = (RvInt32)cmRASReasonUnknownMessageResponse; /* Reason is XRS */
                else
                {
                    /* Get the reason field from the reject message */
                    status = rasGetParam(ras, (HRAS)tx, cmRASTrStageReject, cmRASParamRejectReason, 0, &reason, NULL);
                }
                cmiCBEnter((ras->app, "cmEvAutoRasReject(hsRas=%p)", tx));
                ras->evAutoRas.cmEvAutoRASReject((HRAS)tx, (cmRASReason)reason);
                cmiCBExit((ras->app, "cmEvAutoRasReject(hsRas=%p)", tx));
            }

            if (!emaWasDeleted((EMAElement)tx))
                tx->evResponse(haRas, (HRAS)tx, stage);
        }
        else
        {
            /* Manual RAS callbacks */

            /* Start with the callbacks */
            if ((rasMessageInfo[messageType].msgType == rasTxMsgConfirm) && (ras->evApp.cmEvRASConfirm != NULL))
            {
                /* Associate message context with transaction */
                tx->hMsgContext = *hMsgContext;
                *hMsgContext = NULL;

                /* xCF message */
                cmiCBEnter((ras->app, "cmEvRASConfirm(haRas=%p,hsRas=%p)", haRas, tx));
                ras->evApp.cmEvRASConfirm(haRas, (HRAS)tx);
                cmiCBExit((ras->app, "cmEvRASConfirm(haRas=%p,hsRas=%p)", haRas, tx));
            }
            else if ((rasMessageInfo[messageType].msgType == rasTxMsgReject) && (ras->evApp.cmEvRASReject != NULL))
            {
                /* xRJ message */

                /* Find out the reject reason and then set the callback */
                RvInt32 reason;

                if (messageType == rasMsgUnknownMessageResponse)
                    reason = (RvInt32)cmRASReasonUnknownMessageResponse; /* Reason is XRS */
                else
                {
                    /* Get the reason field from the reject message */
                    status = rasGetParam(ras, (HRAS)tx, cmRASTrStageReject, cmRASParamRejectReason, 0, &reason, NULL);
                }

                /* Associate message context with transaction */
                tx->hMsgContext = *hMsgContext;
                *hMsgContext = NULL;

                /* Make sure we notify the application with a callback... */
                cmiCBEnter((ras->app, "cmEvRasReject(haRas=%p,hsRas=%p)", haRas, tx));
                ras->evApp.cmEvRASReject(haRas, (HRAS)tx, (cmRASReason)reason);
                cmiCBExit((ras->app, "cmEvRasReject(haRas=%p,hsRas=%p)", haRas, tx));
            }
        }
    }

    /* Release the transaction and we're done */
    emaRelease((EMAElement)tx);

    if (status > 0)
        status = RV_OK;
    return status;
}


/************************************************************************
 * rasCloseOutTx
 * purpose: Close an outgoing transaction
 * input  : ras             - RAS module to use
 *          tx              - Outgoing transaction to close
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasCloseOutTx(
    IN rasModule*       ras,
    IN rasOutTx*        tx)
{
    void*   hMsgContext;

    /* Lock the hash table first */
    RvLockGet(&ras->lockOutHash, ras->logMgr);

    /* Lock the transaction itself and mark it - we'll delete it before unlocking it... */
    if(emaLock((EMAElement)tx))
    {
        /* Delete the hash entry */
        if (tx->hashValue != NULL)
            hashDelete(ras->outHash, tx->hashValue);

        /* Unlock the hash table before killing the transaction */
        RvLockRelease(&ras->lockOutHash, ras->logMgr);

        /* Get all the handle we need to remove after unlocking */
        hMsgContext = tx->hMsgContext;

        /* Kill the timer if there is one - before unlocking... */
        RvH323TimerCancel(ras->timers, &tx->timer);

        /* Remove RPOOL element */
        if (tx->encodedMsg != NULL)
            rpoolFree(ras->messages, tx->encodedMsg);

        /* Delete, Unlock and Release */
        emaDelete((EMAElement)tx);
        emaUnlock((EMAElement)tx);

        /* Dispose the message context for this transaction if we have one */
        if (hMsgContext != NULL)
            ras->cmiEvRASReleaseMessageContext(hMsgContext);
    }
    else
    {
        RvLockRelease(&ras->lockOutHash, ras->logMgr);
    }

    return 0;
}


/************************************************************************
 * rasReallyCloseOutTx
 * purpose: Finish cleanup of an outgoing transaction
 * input  : ras             - RAS module to use
 *          tx              - Outgoing transaction to close
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
void rasReallyCloseOutTx(
    IN rasModule*       ras,
    IN rasOutTx*        tx)
{
    /* Free some nodes in the PVT db */
    pvtDelete(ras->hVal, tx->txProperty);
}




#ifdef __cplusplus
}
#endif

