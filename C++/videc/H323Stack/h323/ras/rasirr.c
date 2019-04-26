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
#include "rasparams.h"
#include "rasutils.h"
#include "rasin.h"
#include "rasout.h"
#include "rasirr.h"

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 *
 *                              Private functions
 *
 ************************************************************************/


/************************************************************************
 * rasFindCallHandleForIrr
 * purpose: Find the call handle for an IRR message with a single call
 *          information inside it. This function is used to determine if
 *          the IRR message is solicited or unsolicited
 * input  : ras         - RAS module to use
 *          rootId      - Node ID of the message to check
 *          srcAddress  - Address of the sender
 * output : none
 * return : Call handle of a call matches the IRR
 *          NULL otherwise
 ************************************************************************/
static HCALL rasFindCallHandleForIrr(
    IN  rasModule           *ras,
    IN  int                 rootId,
    IN  cmTransportAddress  *srcAddress)
{
    catStruct   key;
    int         nodeId, tmpNodeId;
    RVHCATCALL  hCatCall;
    RvInt32     isOrigin;

    /* We surely know the source address of the message */
    key.flags = catRasSrcAddr;
    memcpy(&key.rasSrcAddr, srcAddress, sizeof(cmTransportAddress));

    /* Find the call information... */
    __pvtGetByFieldIds(nodeId, ras->hVal, rootId, {_anyField _q931(perCallInfo) _nul(1) LAST_TOKEN}, NULL, NULL, NULL);

    /* RAS-CRV */
    tmpNodeId = pvtGetChild(ras->hVal, nodeId, __q931(callReferenceValue), NULL);
    if (tmpNodeId >= 0)
    {
        int crv;
        if (pvtGet(ras->hVal, tmpNodeId, NULL, NULL, &crv, NULL) >= 0)
        {
            key.rasCRV = (RvUint32)(0x8000^crv);
            key.flags |= catRasCRV;
        }
    }
    
    /* Originator (answerCall) */
    tmpNodeId = pvtGetChildByFieldId(ras->hVal, nodeId, __q931(originator), &isOrigin, NULL);
    if (tmpNodeId >= 0)
    {
        key.answerCall = !isOrigin;
        key.flags |= catAnswerCall;
    }

    /* CID */
    tmpNodeId = pvtGetChild(ras->hVal, nodeId, __q931(conferenceID), NULL);
    if (tmpNodeId >= 0)
        if (pvtGetString(ras->hVal, tmpNodeId, sizeof(key.cid), (char*)key.cid) >= 0)
            key.flags |= catCID;

    /* CallId */
    __pvtGetByFieldIds(tmpNodeId, ras->hVal, nodeId, {_q931(callIdentifier) _q931(guid) LAST_TOKEN}, NULL, NULL, NULL);
    if (tmpNodeId >= 0)
        if (pvtGetString(ras->hVal, tmpNodeId, sizeof(key.callID), (char*)key.callID) >= 0)
            key.flags |= catCallId;

    /* Find this call using CAT */
    hCatCall = catFind(ras->hCat, &key);

    if (hCatCall != NULL)
        return catGetCallHandle(ras->hCat, hCatCall);

    /* No such luck - no call found */
    return NULL;
}


/************************************************************************
 * rasFindIrqTxAndLock
 * purpose: Find an IRQ transaction related to an incoming IRR and
 *          lock that transaction if found
 * input  : ras             - RAS module to use
 *          requestSeqNum   - Sequence number in decoded message after hook
 * return : Solicited IRR transaction handle if found a possible IRQ matching
 *          NULL if 100% unsolicited IRR
 ************************************************************************/
static rasOutTx* rasFindIrqTxAndLock(
    IN  rasModule*      ras,
    IN  RvUint32        requestSeqNum)
{
    rasOutTx*   tx;
    void*       hashValue;

    /* Check in the hash table */
    RvLockGet(&ras->lockOutHash, ras->logMgr);
    hashValue = hashGetElement(ras->outHash, hashFind(ras->outHash, &requestSeqNum));
    if (hashValue != NULL)
        tx = *((rasOutTx **)hashValue);
    else
        tx = NULL;
    if (tx == NULL)
    {
        /* No outgoing transaction for this one - we can say for certain it's an unsolicited IRR */
        RvLockRelease(&ras->lockOutHash, ras->logMgr);
        return NULL;
    }

    /* Make sure the transaction's type matches an IRQ */
    emaLock((EMAElement)tx);
    RvLockRelease(&ras->lockOutHash, ras->logMgr);

    if (tx->transactionType != cmRASInfo)
    {
        /* Not an IRQ - its certainly an unsolicited IRR */
        emaUnlock((EMAElement)tx);
        return NULL;
    }

    return tx;
}


/************************************************************************
 * rasIsSolicited
 * purpose: Determines if a message is a solicited or an unsolicited IRR
 * input  : ras             - RAS module to use
 *          rootId          - Node ID of the message to check
 *          srcAddress      - Address of the sender
 *          requestSeqNum   - Sequence number in decoded message after hook
 * output : isSolicited     - RV_TRUE if message is a solicited IRR
 *                            RV_FALSE if message is an unsolicted IRR
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
static RvStatus rasIsSolicited(
    IN  rasModule           *ras,
    IN  int                 rootId,
    IN  cmTransportAddress  *srcAddress,
    IN  RvUint32            requestSeqNum,
    OUT RvBool              *isSolicited)
{
    rasOutTx*   tx;
    RvInt32     unsolicited, needResponse;
    int         status;
    int         tmpNodeId, calls;
    HCALL       hsCall;

    /* Check if we're using Version 4: This message includes indication of this is a
       solicited or an unsolicited IRR */
    __pvtGetByFieldIds(status, ras->hVal, rootId, {_anyField _q931(unsolicited) LAST_TOKEN}, NULL, &unsolicited, NULL);
    if (status >= 0)
    {
        *isSolicited = (unsolicited == RV_FALSE);
        return RV_OK;
    }

    /* No such luck - it's not Version 4. Check the amount of callInfo objects we've got.
       Other than 1 means it's a solicited IRR */
    __pvtGetByFieldIds(tmpNodeId, ras->hVal, rootId, {_anyField _q931(perCallInfo) LAST_TOKEN}, NULL, NULL, NULL);
    if (tmpNodeId < 0)
    {
        /* no perCallInfo field - it's a solicited IRR */
        *isSolicited = RV_TRUE;
        return RV_OK;
    }
    calls = pvtNumChilds(ras->hVal, tmpNodeId);
    if (calls < 0) return calls; /* error */
    if (calls != 1)
    {
        /* more than 1 call - it's a solicited IRR */
        *isSolicited = RV_TRUE;
        return RV_OK;
    }

    /* We have a single call - check if needResponse = RV_TRUE */
    __pvtGetByFieldIds(status, ras->hVal, rootId, {_anyField _q931(needResponse) LAST_TOKEN}, NULL, &needResponse, NULL);
    if ((status >= 0) && needResponse)
    {
        /* needResponse=RV_TRUE - it's unsolicited IRR */
        *isSolicited = RV_FALSE;
        return RV_OK;
    }

    /* See if we can find a matching IRQ transaction for the IRR */
    tx = rasFindIrqTxAndLock(ras, requestSeqNum);
    if (tx == NULL)
    {
        *isSolicited = RV_FALSE;
        return RV_OK;
    }

    /* Seems like we have to find out if this call matches the one we've got */
    hsCall = rasFindCallHandleForIrr(ras, rootId, srcAddress);

    /* This is a solicited IRR if the call matches the one this IRQ is looking for, or if
       the IRQ was not called on a specific call. */
    *isSolicited = (tx->hsCall == NULL) || (hsCall == tx->hsCall);

    emaUnlock((EMAElement)tx);

    if (hsCall != NULL)
        return RV_OK;

    RvLogError(&ras->log,
        (&ras->log, "rasIsSolicited: No call matched the IRR message (root=%d)", rootId));
    return RV_ERROR_UNKNOWN;
}



/************************************************************************
 *
 *                              Public functions
 *
 ************************************************************************/


/************************************************************************
 * rasHandleIrr
 * purpose: Handle an incoming IRR message.
 *          This function first has to determine if this is a solicited
 *          or an unsolicited IRR and then handle it through incoming or
 *          outgoing transactions.
 * input  : ras             - RAS module to use
 *          index           - Index of connection this message was received on
 *          srcAddress      - Address of the sender
 *          messageBuf      - The message buffer to send
 *          messageLength   - The length of the message in bytes
 *          messageNodeId   - Node ID of message root. If negative, then
 *                            message is decoded from given buffer and hook
 *                            is called
 *          requestSeqNum   - Sequence number in decoded message after hook
 * output : hMsgContext     - Incoming message context. Used mostly by security
 *                            If the returned value is different than NULL,
 *                            then the message context is not used by the
 *                            transaction and should be released
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasHandleIrr(
    IN  rasModule           *ras,
    IN  RvInt               index,
    IN  cmTransportAddress  *srcAddress,
    IN  RvUint8             *messageBuf,
    IN  RvUint32            messageLength,
    IN  RvPvtNodeId         messageNodeId,
    IN  RvUint32            requestSeqNum,
    OUT void                **hMsgContext)
{
    RvStatus         status = 0;
    RvBool      isSolicited = RV_FALSE;
    rasMessages newType;
    RvUint32    newSeqNum;

    /* See if we have to decode this message or not */
    if (!RV_PVT_NODEID_IS_VALID(messageNodeId))
    {
        status =
            rasDecodeAndRecv(ras, index, messageBuf, messageLength, RV_FALSE, srcAddress,
                             &messageNodeId, &newType, &newSeqNum, hMsgContext);

        /* Make sure the application didn't change the sequence number or type of message */
        if ((status >= 0) && ((newType != rasMsgInfoRequestResponse) || (newSeqNum != requestSeqNum)))
        {
            /* Reroute the message to check it again... */
            return rasRouteMessage(ras, index, RV_FALSE, srcAddress, messageBuf, messageLength, messageNodeId, newType, newSeqNum, hMsgContext);
        }
    }

    /* Check if message is solicited or unsolicited */
    if (status >= 0)
        status = rasIsSolicited(ras, messageNodeId, srcAddress, requestSeqNum, &isSolicited);

    if (status != RV_OK)
    {
        if (RV_PVT_NODEID_IS_VALID(messageNodeId))
            pvtDelete(ras->hVal, messageNodeId);
        return status;
    }

    if (isSolicited)
    {
        /* We have a Solicited IRR - just handle it as a regular reply message */
        return rasHandleReply(ras, index, srcAddress, messageBuf, messageLength, messageNodeId, rasMsgInfoRequestResponse, requestSeqNum, hMsgContext);
    }


    /* If we're here, then we have an unsolicited IRR.
       The problem we're facing is deciding whether this IRR is for a dummy request
       which is handled by the outgoing transactions as responses, or an incoming
       request transaction. */

    {
        HRAS        tx;
        HCALL       hsCall;
        RVHCATCALL  hCatCall;

        /* Find the call handle for this unsolicited IRR */
        hsCall = rasFindCallHandleForIrr(ras, messageNodeId, srcAddress);
        if (hsCall == NULL)
        {
            /* No call was found for this IRR - ignore it at this point */
            RvLogError(&ras->log,
                (&ras->log, "rasHandleIrr: No call found for unsolicited IRR message - ignoring message (root=%d)", messageNodeId));
            pvtDelete(ras->hVal, messageNodeId);
            return RV_ERROR_UNKNOWN;
        }

        hCatCall = cmiGetCatForCall(hsCall);

        /* Find out if this call has a dummy request on it */
        tx = catGetUnsolicitedIRR(ras->hCat, hCatCall);
        if (tx != NULL)
        {
            /* There's a dummy transaction - handle as a response to it */
            emaLock((EMAElement)tx);

            return rasHandleTxResponse(ras, (rasOutTx *)tx, messageNodeId, rasMsgInfoRequestResponse, hMsgContext);
        }
        else
        {
            /* No dummy transaction - handle as an incoming request */
            return rasHandleRequest(ras, index, RV_FALSE, srcAddress, messageBuf, messageLength, messageNodeId, rasMsgInfoRequestResponse, requestSeqNum, hsCall, hMsgContext);
        }
    }
}


/************************************************************************
 * rasSetIrrFields
 * purpose: Set the fields inside IRR messages, to be sent later on
 * input  : ras         - RAS module to use
 *          hsRas       - RAS transaction to set
 *          irrNode     - node on which to build the IRR
 *          hsCall      - Call related with the transaction
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasSetIrrFields(
    IN  rasModule*      ras,
    IN  HRAS            hsRas,
    IN  RvPvtNodeId     irrNode,
    IN  HCALL           hsCall)
{
    char                callid[16];
    RvInt32             rasCrv, callidLen, iVal;
    int                 ret = 0;
    int                 rootId, destNodeId;
    RvBool              isOrigin;
    cmTransportAddress  addr;

    isOrigin = (RvBool)cmCallGetOrigin(hsCall, NULL);

    /* Make sure we got a response node */
    rasSetParam(ras, hsRas, cmRASTrStageConfirm, cmRASParamEmpty, 0, 0, NULL);

    __pvtBuildByFieldIds(rootId, ras->hVal, irrNode,
        {_q931(infoRequestResponse) _q931(perCallInfo) _nul(1) LAST_TOKEN}, 0, NULL);

    /* callReferenceValue */
    if (cmCallGetParam(hsCall, cmParamRASCRV, 0, &rasCrv, NULL) >= 0)
        pvtAdd(ras->hVal, rootId, __q931(callReferenceValue), rasCrv, NULL, NULL);

    /* conferenceID */
    callidLen = sizeof(callid);
    if (cmCallGetParam(hsCall, cmParamCID, 0, &callidLen, callid) >= 0)
        pvtAdd(ras->hVal, rootId, __q931(conferenceID), callidLen, callid, NULL);

    /* callIdentifier */
    callidLen = sizeof(callid);
    if (cmCallGetParam(hsCall, cmParamCallID, 0, &callidLen, callid) >= 0)
    {
        __pvtBuildByFieldIds(ret, ras->hVal, rootId, {_q931(callIdentifier) _q931(guid) LAST_TOKEN}, callidLen, callid);
    }

    /* originator */
    pvtAdd(ras->hVal, rootId, __q931(originator), (RvInt32)isOrigin, NULL, NULL);

    /* callType */
    if (cmCallGetParam(hsCall, cmParamCallType, 0, &iVal, NULL) < 0)
        iVal = (RvInt32)cmCallTypeP2P; /* set default value if none is set */
    destNodeId = pvtAdd(ras->hVal, rootId, __q931(callType), 0, NULL, NULL);
    switch ((cmCallType)iVal)
    {
        case cmCallTypeP2P:     pvtAdd(ras->hVal, destNodeId, __q931(pointToPoint), 0, NULL, NULL); break;
        case cmCallTypeOne2N:   pvtAdd(ras->hVal, destNodeId, __q931(oneToN), 0, NULL, NULL); break;
        case cmCallTypeN2One:   pvtAdd(ras->hVal, destNodeId, __q931(nToOne), 0, NULL, NULL); break;
        case cmCallTypeN2Nw:    pvtAdd(ras->hVal, destNodeId, __q931(nToN), 0, NULL, NULL); break;
    }

    /* bandWidth */
    if (cmCallGetParam(hsCall, cmParamRate, 0, &iVal, NULL) >= 0)
        pvtAdd(ras->hVal, rootId, __q931(bandWidth), iVal/50, NULL, NULL);

    /* callModel */
    destNodeId = pvtAdd(ras->hVal, rootId, __q931(callModel), 0, NULL, NULL);
    if (cmIsRoutedCall(hsCall))
        pvtAdd(ras->hVal, destNodeId, __q931(gatekeeperRouted), 0, NULL, NULL);
    else
        pvtAdd(ras->hVal, destNodeId, __q931(direct), 0, NULL, NULL);

    /* h245, substituteConfIDs */
    pvtAdd(ras->hVal, rootId, __q931(h245), 0, NULL, NULL);
    pvtAdd(ras->hVal, rootId, __q931(substituteConfIDs), 0, NULL, NULL);

    /* callSignaling */
    destNodeId = pvtAdd(ras->hVal, rootId, __q931(callSignaling), 0, NULL, NULL);
    if (isOrigin)
    {
        if (cmCallGetParam(hsCall, cmParamSrcCallSignalAddress, 0, NULL, (char*)&addr) >= 0)
            cmTAToVt(ras->hVal, pvtAdd(ras->hVal, destNodeId, __q931(sendAddress), 0, NULL, NULL), &addr);
        if (cmCallGetParam(hsCall, cmParamDestCallSignalAddress, 0, NULL, (char*)&addr) >= 0)
            cmTAToVt(ras->hVal, pvtAdd(ras->hVal, destNodeId, __q931(recvAddress), 0, NULL, NULL), &addr);
    }
    else
    {
        if (cmCallGetParam(hsCall, cmParamSrcCallSignalAddress, 0, NULL, (char*)&addr) >= 0)
            cmTAToVt(ras->hVal, pvtAdd(ras->hVal, destNodeId, __q931(recvAddress), 0, NULL, NULL), &addr);
        if (cmCallGetParam(hsCall, cmParamDestCallSignalAddress, 0, NULL, (char*)&addr) >= 0)
            cmTAToVt(ras->hVal, pvtAdd(ras->hVal, destNodeId, __q931(sendAddress), 0, NULL, NULL), &addr);
    }

    return ret;
}



#ifdef __cplusplus
}
#endif

