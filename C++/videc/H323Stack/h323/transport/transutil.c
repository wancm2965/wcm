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

#include "transport.h"
#include "transportint.h"
#include "transnet.h"
#include "transutil.h"
#include "transStates.h"
#include "q931asn1.h"
#include "h245.h"
#include "msg.h"
#include "cm.h"
#include "emanag.h"
#include "cat.h"
#include "cmutils.h"
#include "prnutils.h"
#include "pvaltreeStackApi.h"
#include "psyntreeStackApi.h"
#include "rvhost.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus*/



/******************************************************************************
 * updateHostMultiplexStatus
 * ----------------------------------------------------------------------------
 * General: Update the status of a multiplexed host, moving it into or out of
 *          the garbage collection list of empty multiplexed hosts.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  transGlobals - The global data of the module.
 *         host         - Host to update.
 * Output: none
 *****************************************************************************/
RvStatus updateHostMultiplexStatus(
    IN cmTransGlobals   *transGlobals,
    IN cmTransHost      *host)
{
    RvLockGet(&transGlobals->lockGarbage, transGlobals->logMgr);

    if (host->firstSession == NULL)
    {
        /* Should be in the empty hosts list */
        if ((host->prevEmptyHost == NULL) && (host->nextEmptyHost == NULL) && (host != transGlobals->oldestEmptyQ931MuxHost))
        {
            /* Not on the list - add it as the most recent one */
            if (transGlobals->recentEmptyQ931MuxHost)
                transGlobals->recentEmptyQ931MuxHost->nextEmptyHost = host;
            host->prevEmptyHost = transGlobals->recentEmptyQ931MuxHost;
            transGlobals->recentEmptyQ931MuxHost = host;
            if (transGlobals->oldestEmptyQ931MuxHost == NULL)
                transGlobals->oldestEmptyQ931MuxHost = host;
        }
    }
    else
    {
        /* Shouldn't be in the empty hosts list */
        if ((host->prevEmptyHost != NULL) || (host->nextEmptyHost != NULL) || (host == transGlobals->oldestEmptyQ931MuxHost))
        {
            /* In the list - we have to remove it from there */
            if (host->prevEmptyHost != NULL)
                host->prevEmptyHost->nextEmptyHost = host->nextEmptyHost;
            if (host->nextEmptyHost != NULL)
                host->nextEmptyHost->prevEmptyHost = host->prevEmptyHost;
            if (transGlobals->oldestEmptyQ931MuxHost == host)
                transGlobals->oldestEmptyQ931MuxHost = host->nextEmptyHost;
            if (transGlobals->recentEmptyQ931MuxHost == host)
                transGlobals->recentEmptyQ931MuxHost = host->prevEmptyHost;
            host->nextEmptyHost = NULL;
            host->prevEmptyHost = NULL;
        }
    }

    RvLockRelease(&transGlobals->lockGarbage, transGlobals->logMgr);

    return RV_OK;
}


/******************************************************************************
 * garbageCollectMultiplexHost
 * ----------------------------------------------------------------------------
 * General: Check if we need to garbage collect a multiplexed host and drop it
 *          if we have to.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  transGlobals - The global data of the module.
 * Output: none
 *****************************************************************************/
RvStatus garbageCollectMultiplexHost(
    IN cmTransGlobals   *transGlobals)
{
    emaStatistics stats;

    emaGetStatistics(transGlobals->hEmaHosts, &stats);
    if (stats.elems.cur + 1 >= stats.elems.max)
    {
        cmTransHost *host;

        /* We always make sure we have a vacant host to use. */
        RvLockGet(&transGlobals->lockGarbage, transGlobals->logMgr);

        /* We only get the host here and don't close it due to dead-locking issues of the
           host's lock and the garbage collection lock */
        host = transGlobals->oldestEmptyQ931MuxHost;

        RvLockRelease(&transGlobals->lockGarbage, transGlobals->logMgr);

        if (host != NULL)
        {
            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog, "Decided to close host %d-%p by the garbage collection",
                emaGetIndex((EMAElement)host), host));

            /* Close the host... */
            closeHostInternal((HSTRANSHOST)host, RV_TRUE);
        }
    }

    return RV_OK;
}


/**************************************************************************************
 * createMessage
 *
 * Purpose: creates a skeleton message of the given type and fills in the CRV and callId
 *          of the given session.
 * Input:   transGlobals    - The global data of the module.
 *          msgType         - which message to create
 *          CRV             - CRV to use.
 *          callId          - call Identifier to use.
 *
 * Output:  node - the created message.
 *
 * Returned Value:  RV_TRUE - success
 **************************************************************************************/
RvBool createMessage(
    cmTransGlobals    *transGlobals,
    cmCallQ931MsgType msgType,
    RvUint16          CRV,
    RvUint8           *callId,
    int               *node)
{
    int newMessage, messageNode = -1, res;

    /* create a skeleton message */
    if (transGlobals->sessionEventHandlers.cmEvTransGetMessageNode)
    {
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "cmEvTransGetMessageNode(hPvt=%p, msgType=%d)",
                transGlobals->hPvt, msgType));

        transGlobals->sessionEventHandlers.cmEvTransGetMessageNode(
                                                        transGlobals->hAppATrans,
                                                        msgType,
                                                        &messageNode);
    }

    /* copy the message and add session data to the message */
    newMessage = pvtAddRoot(transGlobals->hPvt, NULL, 0, NULL);

    if (newMessage >= 0)
    {
        RvInt32 temp = CRV;
        pvtSetTree(transGlobals->hPvt, newMessage, transGlobals->hPvt, messageNode);

        __pvtBuildByFieldIds(res, transGlobals->hPvt, newMessage,
                                {
                                  _q931(callReferenceValue)
                                  _q931(twoBytes)
                                  LAST_TOKEN
                                },
                             temp, NULL);

        if (res >= 0)
        {
            __pvtBuildByFieldIds(res, transGlobals->hPvt, newMessage,
                                    { _q931(message)
                                      _anyField
                                      _q931(userUser)
                                      _q931(h323_UserInformation)
                                      _q931(h323_uu_pdu)
                                      _q931(h323_message_body)
                                      _anyField
                                      _q931(callIdentifier)
                                      _q931(guid)
                                      LAST_TOKEN
                                    },
                                16, (char *)callId);
        }
        else
        {
            pvtDelete(transGlobals->hPvt, newMessage);
            return RV_FALSE;
        }
    }
    else
    {
        pvtDelete(transGlobals->hPvt, newMessage);
        return RV_FALSE;
    }

    cleanMessage(transGlobals->hPvt, newMessage);

    if (node)
        *node = newMessage;

    return RV_TRUE;
}

/**************************************************************************************
 * findHostInHash
 *
 * Purpose: To look for an existing host with the same address as the given one.
 *          If a suitable host is found, i.e. identical in address and type,
 *          we check it to be connected (the prefered state). If it's not we look for
 *          another one which is connected. If none was found we bring a suitable host
 *          that is not connected. If such a host doesn't exist either, return NULL.
 * Input:   transGlobals    - The global data of the module.
 *          remoteAddress   - The address of the looked for host.
 *          isMultiplexed   - Do we need a host which is multiplexed or not
 *          isAnnexE        - the type of the connection (TPKT or annex E).
 *
 * Output:  None.
 *
 * Returned Value:  A pointer to the found host element or NULL if not found
 **************************************************************************************/
cmTransHost *findHostInHash(IN cmTransGlobals       *transGlobals,
                            IN cmTransportAddress   *remoteAddress,
                            IN cmTransportAddress   *localAddress,
                            IN RvBool               isMultiplexed,
                            IN RvBool               isAnnexE)
{
    void *loc;
    hostKey        key;
    RvBool         found = RV_FALSE;
    cmTransHost    *host = NULL;
    cmTransHost    *lastNonActiveHost = NULL;

    /* build the key to look by */
    key.ipNum = condenseIpTo32bit(remoteAddress);
    key.port = (RvUint32)remoteAddress->port;
    key.type = isAnnexE;

    /* lock the hash for the search */
    RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);

    /* go over all hosts that satisfy the key, i.e. address+type */
    for (loc = hashFind(transGlobals->hHashHosts,(void *)&key);
         (loc != NULL) && (!found);
         loc = hashFindNext(transGlobals->hHashHosts, (void *)&key, loc))
    {
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "hashFind=%p [hostKey: ip=%x port=%d type=%d]",
                loc, key.ipNum, key.port, key.type));

        host = *(cmTransHost **)hashGetElement(transGlobals->hHashHosts, loc);

        RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);

        if (emaLock((EMAElement)host))
        {
            if (isAnnexE)
            {
                if ((host->state == hostClosed) && (host->connectTimer != NULL))
                {
                    /* CLEAR !!! */
                    RvH323TimerCancel(transGlobals->hTimers, &host->connectTimer);
                    /* ZAP ! */
                    host->state = hostConnected;
                    /* blip---blip---blip---blip--- */
                    found = RV_TRUE;
                    /* clear the application handle, we are after hook-close */
                    emaSetApplicationHandle((EMAElement)host, NULL);
                }
                else if (host->state != hostClosing)
                {
                    found = RV_TRUE;
                }
            }
            else
            {
                /* if both host and session are multiplexed, we found our host */
                if ( (host->isMultiplexed) && (host->remoteIsMultiplexed) && (isMultiplexed) )
                    found = RV_TRUE;
                else
                /* if both host and session are not multiplexed, but the host is available for
                   the session, i.e. not servicing another session, we found it */
                if ( ((!host->isMultiplexed) || (!isMultiplexed)) && (!host->firstSession) )
                    found = RV_TRUE;
            }

            /* make sure the remote address is correct (we use only 32 bit in the hash) */
            if (found && !isSameAddress(remoteAddress, &host->remoteAddress))
            {
                found = RV_FALSE;
            }
            /* make sure the local address is ok (if set) */
            if (found && (localAddress != NULL) &&
                !isSameAddress(localAddress, &host->localAddress))
            {
                /* try a zero address */
                cmTransportAddress lza;
                memset(&lza, 0, sizeof(cmTransportAddress));
                lza.distribution = localAddress->distribution;
                lza.port = localAddress->port;
                lza.type = localAddress->type;
                if (!isSameAddress(&lza, &host->localAddress))
                    found = RV_FALSE;
            }
            /* if the host is not yet connected, try to look for a connected one,
               but remember the last non connected one to be used if no connected
               one would be found */
            if (found && (host->state != hostConnected) &&
                (host->state != hostDoubleConnected) && (host->state != hostBusy))
            {
                found = RV_FALSE;
                if ((host->state != hostClosing) && (host->state != hostClosed) && (host->state != hostIdle))
                    lastNonActiveHost = host;
            }
            emaUnlock((EMAElement)host);
        }
        RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
    }

    RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);

    /* if we found a suitable host, return it, else return a NULL */
    if (found)
        return host;
    else
        return lastNonActiveHost;
}

/**************************************************************************************
 * transEncodeMessage
 *
 * Purpose: to get the given message encoded, either by a callback or by the cmEmEncode.
 * Input:   host            - The host on which the message is to be sent
 *          session         - The session on whose behalf the message is to be sent.
 *          transGlobals    - The global data of the transport module.
 *          pvtNode         - the message to be encoded
 *          buffer          - the buffer into which to encode
 *
 * Output:  encMsgSize - The size of the encoded message.
 *
 * Returned Value: cmTransErr - in case that the encoding failed, cmTransOK - otherwise
 **************************************************************************************/
TRANSERR transEncodeMessage(cmTransHost    *host,
                            cmTransSession *session,
                            cmTransGlobals *transGlobals,
                            int            pvtNode,
                            RvUint8        *buffer,
                            int            *encMsgSize)
{
    HATRANSHOST    haTransHost = NULL;
    HATRANSSESSION haTransSession;
    int            encSize;
    int            encRes = 0;
    TRANSERR       encTransRes = cmTransOK;
    RvH323ConnectionType connType;

    /* determine the application handles of the host and session */
    haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);
    if (host)
        haTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)host);

    /* determine the type of message, if there is no host then it's a H.245 tunneled message,
       otherwise it's either a H.245 message or a Q.931 according to the host's type */
    connType = RvH323ConnectionH245;
    if (host)
        if ( (host->type == cmTransQ931Conn) )
            connType = RvH323ConnectionQ931;

    /* Report and encode the given message */
    if (!host)
    {
#if (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG))
        RvPstFieldId fieldId;
        int nodeId;
        char *ret;

        nodeId = pvtChild(transGlobals->hPvt, pvtNode);
        nodeId = pvtChild(transGlobals->hPvt, nodeId);

        pvtGet(transGlobals->hPvt,
               nodeId,
               &fieldId,
               NULL, NULL, NULL);
        ret = pstGetFieldNamePtr(transGlobals->synProtH245, fieldId);

        if (RvLogIsSelected(&transGlobals->hTPKTCHAN, RV_LOGLEVEL_DEBUG))
        {
            RvLogTextDebug(&transGlobals->hTPKTCHAN,
                "New TUNNELED message (call %d-%p) sent --> %s:",
                    emaGetIndex((EMAElement)haTransSession), haTransSession, ret);
            pvtPrintInternal(transGlobals->hPvt, pvtNode, &transGlobals->hTPKTCHAN, msGetDebugLevel());
        }
        else
        {
            RvLogDebug(&transGlobals->hTPKTWIRE, (&transGlobals->hTPKTWIRE,
                "New TUNNELED message (call %d-%p) sent --> %s:",
                    emaGetIndex((EMAElement)haTransSession), haTransSession, ret));
            pvtPrintInternal(transGlobals->hPvt, pvtNode, &transGlobals->hTPKTWIRE, msGetDebugLevel());
        }
#endif /* (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG)) */
    }

    if ( (transGlobals->hostEventHandlers.cmEvTransSendRawMessage) && (host) )
    {
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog,
                "cmEvTransSendRawMessage(host=%d(%p), haHost=%p, session=%d, hsSession=%p, pvtNode=%d)",
                emaGetIndex((EMAElement)host), host, haTransHost, emaGetIndex((EMAElement)session),
                haTransSession, pvtNode));

        encTransRes = transGlobals->hostEventHandlers.cmEvTransSendRawMessage(
                        (HSTRANSHOST)host,
                        haTransHost,
                        (HSTRANSSESSION) session,
                        haTransSession,
                        pvtNode,
                        transGlobals->codingBufferSize,
                        buffer,
                        &encSize);
    }
    else
        encRes = cmEmEncode(transGlobals->hPvt,
                            pvtNode,
                            buffer,
                            transGlobals->codingBufferSize,
                            &encSize);
    if (encMsgSize)
        *encMsgSize = encSize;

    if ( (encRes < 0) || ( (encTransRes != cmTransOK) && (encTransRes != cmTransIgnoreMessage) ) )
    {
        if (transGlobals->sessionEventHandlers.cmEvTransBadMessage)
        {
            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "cmEvTransBadMessage(hsSession = %d(%p), haSession=%p, type=%d,outgoing=RV_TRUE)",
                    emaGetIndex((EMAElement)session), session, haTransSession, connType));

            transGlobals->sessionEventHandlers.cmEvTransBadMessage(
                        (HSTRANSSESSION) session,
                        haTransSession,
                        connType,
                        buffer,
                        encSize,
                        RV_TRUE);
        }

        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "transEncodeMessage Failed on encoding"));
        return cmTransErr;
    }
    else
    if (encTransRes == cmTransIgnoreMessage)
    {
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "transEncodeMessage Mesage rejected by user"));
        return cmTransIgnoreMessage;
    }

    /* print just for tunneled messages here */
    if (!host)
    {
        printHexBuff(buffer, encSize, &transGlobals->hTPKTCHAN);
    }
    return cmTransOK;
 }

/**************************************************************************************
 * addSessionToPendingList
 *
 * Purpose: Add a session to the list of sessions that were unable to send a message
 *          due to lack of rpool resources. Such sessions should be notified when
 *          resources become available again.
 * Input:   transGlobals - The global data of the transport module.
 *          session      - The session which failed to send the message.
 *
 * Output:  None.
 *
 **************************************************************************************/
void addSessionToPendingList(IN cmTransGlobals *transGlobals,
                             IN cmTransSession *session)
{
    cmTransSession *oldFirstSession;

    RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);

    oldFirstSession                 = transGlobals->pendingList;
    session->nextPending            = oldFirstSession;
    session->prevPending            = NULL;
    if (oldFirstSession)
        oldFirstSession->prevPending    = session;
    transGlobals->pendingList       = session;

    RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);

}

/**************************************************************************************
 * removeSessionFromPendingList
 *
 * Purpose: Remove a session from the list of sessions that were unable to send a message
 *          due to lack of rpool resources. Such sessions are removed when notified that
 *          resources become available again.
 * Input:   transGlobals - The global data of the transport module.
 *          session      - The session which failed to send the message.
 *
 * Output:  None.
 *
 **************************************************************************************/
void removeSessionFromPendingList(
    IN cmTransGlobals *transGlobals,
    IN cmTransSession *session)
{
    cmTransSession *previousSession;

    RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);

    previousSession = session->prevPending;

    /* disconnect the session from the pending list */
    if (previousSession)
        previousSession->nextPending    = session->nextPending;
    if (session->nextPending)
        session->nextPending->prevPending = previousSession;

    /* update the list header if session was the head of the list */
    if (transGlobals->pendingList == session)
        transGlobals->pendingList = session->nextPending;

    RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
}

/**************************************************************************************
 * notifyPendingSessions
 *
 * Purpose: This routine notifies that space is free for pending messages.
 * Input:   transGlobals - The global data of the transport module.
 *          numOfSession - The number of sessions that can be released from the list.
 *
 * Output:  None.
 *
 **************************************************************************************/
void notifyPendingSessions(cmTransGlobals *transGlobals, int numOfSessions)
{
    cmTransSession  *session = transGlobals->pendingList;
    int i;

    for (i=0; (i<numOfSessions && (session)); i++,session=session->nextPending)
    {
        if (transGlobals->sessionEventHandlers.cmEvTransWrite)
        {
            HATRANSSESSION haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);
            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog, "cmEvTransWrite hsTransSession=%d(%p) haTransSession=%p",
                    emaGetIndex((EMAElement)session), session,  haTransSession));

            transGlobals->sessionEventHandlers.cmEvTransWrite((HSTRANSSESSION) session,
                                                              haTransSession);

            removeSessionFromPendingList(transGlobals, session);
        }
    }
}

/**************************************************************************************
 * saveMessageToPool
 *
 * Purpose: This routine gets an encoded message and saves it, until it can send it.
 * Input:   transGlobals - The global data of the transport module.
 *          element      - The session which wants to send the message.
 *          buffer       - The encoded message.
 *          encMsgSize   - Its size.
 *          isTunneled   - RV_TRUE: save to the tunneled message queue,
 *                         RV_FALSE: to the sending message queue
 *          addToTop     - RV_TRUE: Add the message to the start of the queue
 *                         RV_FALSE: Add it at the end.
 *          CRV          - The CRV of the call on which behalf the message is sent
 *                         (used for annex E session ID field).
 *
 * Output:  None.
 *
 **************************************************************************************/
HRPOOLELEM saveMessageToPool(cmTransGlobals *transGlobals,
                            void            *element,
                            RvUint8         *buffer,
                            int             encMsgSize,
                            RvBool          isTunneled,
                            RvBool          addToTop,
                            RvUint16        CRV)
{
    HRPOOLELEM      newMsg;
    HRPOOLELEM      firstMsg = NULL;
    HRPOOLELEM      lastMsg = NULL;
    cmTransHost     *host    = (cmTransHost *)element;
    cmTransSession  *session = (cmTransSession *)element;

    /* lock the rpool */
    RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);

    /* get rpool buffer and copy the encoded message to it */
    newMsg  = rpoolAllocCopyExternal(transGlobals->messagesRPool,
        MSG_HEADER_SIZE+TPKT_HEADER_SIZE, buffer, encMsgSize);
    if (newMsg)
    {
        /* determine which queue of messages we work on (tunneled, or to be sent)*/
        if (isTunneled)
        {
            firstMsg = session->firstMessage;
            lastMsg  = session->lastMessage;
        }
        else
        {
            firstMsg = host->firstMessage;
            lastMsg  = host->lastMessage;
        }

        /* update the pointer from the element to the saved messages */
        if (!firstMsg)
        {
            firstMsg = newMsg;
            lastMsg  = newMsg;
        }

        if (addToTop)
        {
            if (firstMsg != newMsg)
            {
                rpoolCopyFromExternal(transGlobals->messagesRPool,
                                      newMsg,
                                      (void *)&firstMsg,
                                      0,
                                      MSG_HEADER_SIZE);
                firstMsg = newMsg;
            }
        }
        else
        {
            /* update the last saved message to point to the new message
               and make the new message to be the last message */
            if (lastMsg != newMsg)
            {
                rpoolCopyFromExternal(transGlobals->messagesRPool,
                                      lastMsg,
                                      (void *)&newMsg,
                                      0,
                                      MSG_HEADER_SIZE);
                lastMsg = newMsg;
            }
        }

        /* update the CRV on the TPKT header */
        rpoolCopyFromExternal(transGlobals->messagesRPool,
                              newMsg,
                              (void *)&CRV,
                              MSG_HEADER_SIZE,
                              sizeof(CRV));

        /* Update the last message pointer in the appropriate element */
        if (isTunneled)
        {
            session->firstMessage = firstMsg;
            session->lastMessage  = lastMsg;
        }
        else
        {
            host->firstMessage = firstMsg;
            host->lastMessage  = lastMsg;
        }


        {
            RvInt32 poolSize, usageSize, allocatedSize;

            if (rpoolStatistics(transGlobals->messagesRPool,
                                &poolSize,
                                &usageSize,
                                &allocatedSize))
            {
                RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                        "saveMessageToPool[isTunneled=%d] statistics: max=%d, usage=%d, allocated=%d",
                        isTunneled, poolSize, usageSize, allocatedSize));
            }
        }

        RvLogDebug(&transGlobals->hLog, (&transGlobals->hLog,
            "saveMessageToPool[isTunneled=%d] new=%p", isTunneled, newMsg));

        /* update num of messages kept in rpool for sending */
        transGlobals->curUsedNumOfMessagesInRpool++;
        if (transGlobals->curUsedNumOfMessagesInRpool > transGlobals->maxUsedNumOfMessagesInRpool)
            transGlobals->maxUsedNumOfMessagesInRpool++;

        /* unlock the rpool */
        RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);
    }
    else
    {
        /* NO BUFFERS */

        {
            RvInt32 poolSize, usageSize, allocatedSize;

            if (rpoolStatistics(transGlobals->messagesRPool,
                                &poolSize,
                                &usageSize,
                                &allocatedSize))
            {
                RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                        "saveMessageToPool (no buffers) statistics: max=%d, usage=%d, allocated=%d",
                        poolSize, usageSize, allocatedSize));
            }
        }

        /* unlock the rpool */
        RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);

        RvLogWarning(&transGlobals->hLog, (&transGlobals->hLog,
            "saveMessageToPool has no buffers"));
        return NULL;
    }
    return newMsg;
}


/**************************************************************************************
 * moveMessagesFromSessionToHost
 *
 * Purpose: This routine sets the tunneled messages on the session to be sent with the
 *          messages on the host.
 * Input:   transGlobals - The global data of the transport module.
 *          session      - The session which wants to send the message.
 *          host         - The host on which the message is to be sent.
 * Output:  None.
 *
 **************************************************************************************/
void moveMessagesFromSessionToHost(cmTransGlobals *transGlobals,
                                   cmTransSession *session,
                                   cmTransHost    *host)
{
    /* see if there are still messages in the host */
    if (host->firstMessage !=  NULL)
    {
        /* lock the rPool */
        RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
        /* set the first message of the host at the end of the last message of the session */
        rpoolCopyFromExternal(transGlobals->messagesRPool,
                              session->lastMessage,
                              (void *)&host->firstMessage,
                              0,
                              MSG_HEADER_SIZE);
        /* unlock the rPool */
        RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);
    }
    else
        host->lastMessage = session->lastMessage;

    if (session->firstMessage != NULL)
    {
        /* set the message pointers */
        host->firstMessage = session->firstMessage;
        session->firstMessage = NULL;
    }
}


/**************************************************************************************
 * extractMessageFromPool
 *
 * Purpose: This routine removes an encoded message from the head of the host list.
 *          The messages extracted here are tunneled messages.
 * Input:   transGlobals - The global data of the transport module.
 *          element      - The host or session which wants to send the message.
 *          isTunneled   -  TRUE: remove from the tunneled message queue,
 *                          FALSE: remove from the sending message queue
 *
 * Output:  None.
 *
 * Return Value: next message.
 *
 **************************************************************************************/
HRPOOLELEM extractMessageFromPool(
                             cmTransGlobals *transGlobals,
                             void           *element,
                             RvBool         isTunneled)
{
     HRPOOLELEM     nextMsg = NULL;
     HRPOOLELEM     firstMsg;
     cmTransHost    *host       = (cmTransHost *)element;
     cmTransSession *session    = (cmTransSession *)element;
     RvInt32        poolSize, usageSize, allocatedSize;

    /* determine which queue of messages we work on (tunneled, or to be sent)*/
    if (isTunneled)
        firstMsg = session->firstMessage;
    else
        firstMsg = host->firstMessage;

    /* lock the rpool */
    RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);

    /* get the pointer to the next message */
    rpoolCopyToExternal(transGlobals->messagesRPool, (void*)&nextMsg, firstMsg, 0, MSG_HEADER_SIZE);

    /* delete the first message */
    rpoolFree(transGlobals->messagesRPool, firstMsg);

    /* set the next message as the new first message */
    if (isTunneled)
        session->firstMessage = nextMsg;
    else
        host->firstMessage = nextMsg;

    if (rpoolStatistics(transGlobals->messagesRPool, &poolSize, &usageSize, &allocatedSize) == RV_OK)
    {

        RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                "extractMessageFromPool[isTunneled=%d] statistics: max=%d, usage=%d, allocated=%d",
                isTunneled, poolSize, usageSize, allocatedSize));

        if (transGlobals->pendingList)
        {
            int numOfSessionsToRelease;
            if ((numOfSessionsToRelease = (poolSize-allocatedSize)/(RvInt32)transGlobals->codingBufferSize) > 0)
                notifyPendingSessions(transGlobals, numOfSessionsToRelease);
        }
    }

    RvLogDebug(&transGlobals->hLog, (&transGlobals->hLog,
                  "extractMessageFromPool[isTunneled=%d] first=%p last=%p",
                  isTunneled, host->firstMessage, host->lastMessage));

    /* update num of messages kept in rpool for sending */
    transGlobals->curUsedNumOfMessagesInRpool--;

    /* unlock the rpool */
    RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);

    return nextMsg;
}

/**************************************************************************************
 * processQ931OutgoingMessage
 *
 * Purpose: This routine gets a decoded outgoing Q.931 message and modifies it according to
 *          the different tasks at hand:
 *          - Add fields such as multiplexing flags.
 *          - Insert H.245 addresses, if necessary to the messages.
 *          - Insert tunneled messages (H.245, Annex M, Annex L).
 * Input:   session     - The session which wants to send the message.
 *          host        - The host on which the message is to be sent.
 *          pvtNode     - The message.
 *
 * Output:  None.
 *
 * Returned Value: RV_TRUE - send the message, RV_FALSE - don't send it.
 *
 **************************************************************************************/
RvBool processQ931OutgoingMessage(
    IN    cmTransSession    *session,
    IN    cmTransHost       *host,
    INOUT int               pvtNode)
{
    cmTransGlobals  *transGlobals;
    int             messageBodyNode;
    int             msgType;

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

    /* check what type of Q.931 message we have here */
    msgType = pvtGetChildTagByPath(transGlobals->hPvt, pvtNode, "message", 1);
    if (msgType < 0)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog,
                "processQ931OutgoingMessage failed to get message type tag"));
        return RV_FALSE;
    }

    /* position on the UU-IE part of the message */
    __pvtGetNodeIdByFieldIds(   messageBodyNode,
                                transGlobals->hPvt,
                                pvtNode,
                                {   _q931(message)
                                    _anyField
                                    _q931(userUser)
                                    _q931(h323_UserInformation)
                                    _q931(h323_uu_pdu)
                                    LAST_TOKEN
                                });
    /* if the message doesn't have UU-IE part, we have nothing to do with it, just
       send it */
    if (messageBodyNode < 0)
        return RV_TRUE;

    /***********************************************************************************/
    /* for first message on the session get the CRV and callID and insert to the hash  */
    /***********************************************************************************/
    if (!m_sessionget(session, firstMessageSent))
    {
        RvPstFieldId tmpReason;
        if (!findSessionByMessage(transGlobals, host, pvtNode, RV_FALSE, &session, &tmpReason))
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "processQ931OutgoingMessage failed to locate session in HASH, ignore emssage"));
            return RV_FALSE;
        }
        else
        if (session)
        {
            /* if session is NULL here this means that we are sending a CRV==0
               message so no session is associated with it */
            if ( (msgType != (int)cmQ931setupAck) &&
                 (msgType != (int)cmQ931information) &&
                 (msgType != (int)cmQ931progress) &&
                 (msgType != (int)cmQ931status) &&
                 (msgType != (int)cmQ931statusInquiry)
                )
                /* mark that we already sent a call that could have approved tunneling */
                m_sessionset(session, firstMessageSent, RV_TRUE);
        }
    }

    /***********************************************************************/
    /* set the multipleCalls and maintainConnection parameters (multiplexing stuff) */
    /***********************************************************************/
    setMultiplexedParams(transGlobals, session, host, pvtNode, msgType);


    /* If we have no session, then we're done here */
    if (session == NULL)
        return RV_TRUE;


    /***********************************************************************/
    /* check the fast start state of the session */
    /***********************************************************************/
    if ( !emaWasDeleted((EMAElement)session) )
        setTheFastStartStateByMessage(transGlobals, session, messageBodyNode, msgType);

    /***********************************************************************/
    /* check the parallel tunneling state of the session  */
    /***********************************************************************/
    if ( !emaWasDeleted((EMAElement)session) )
        setTheParallelTunnelingStateByMessage(transGlobals,
                                              session,
                                              msgType,
                                              messageBodyNode,
                                              RV_TRUE /* outgoing */);

    /***********************************************************************/
    /* handling whether to start listenning or establish connection for H.245 */
    /***********************************************************************/
    if ( !emaWasDeleted((EMAElement)session) )
        if (!m_sessionget(session, notEstablishControl))
            if (!determineIfToOpenH245(RV_TRUE /* outgoing message */,
                                       transGlobals,
                                       session,
                                       messageBodyNode,
                                       msgType))
                return RV_FALSE;

    /***********************************************************************/
    /* handle reporting of a new H.245 connection to the user    */
    /***********************************************************************/
    if ( !emaWasDeleted((EMAElement)session) )
        if (!m_sessionget(session, notEstablishControl))
            reportH245(RV_TRUE /* outgoing message */,
            transGlobals,
            session,
            messageBodyNode,
            msgType);

    /***********************************************************************/
    /* handle insertion of tunneled H.245 messages to the Q.931 message    */
    /***********************************************************************/
    if ( !emaWasDeleted((EMAElement)session) )
    {
        insertH245TunnelingFlag(transGlobals, session, messageBodyNode, msgType);
        if (!m_sessionget(session, notEstablishControl) &&
            transGlobals->sessionEventHandlers.cmEvTransWantsToStartControl(
                (HSTRANSSESSION)session, emaGetApplicationHandle((EMAElement*)session)))
        {
            insertH245TunneledMessages(transGlobals, session, messageBodyNode, msgType);
        }
        insertH450TunneledMessages(transGlobals, session, messageBodyNode, msgType);
        insertAnnexLTunneledMessages(transGlobals, session, messageBodyNode);
        insertAnnexMTunneledMessages(transGlobals, session, messageBodyNode);
    }

    /***********************************************************************/
    /* handle insertion of feature set flags    */
    /***********************************************************************/
    if ( !emaWasDeleted((EMAElement)session) )
    {
        addFeatureSet(transGlobals, session, pvtNode, RV_FALSE);
    }

    return RV_TRUE;
}

/**************************************************************************************
 * processH245OutgoingMessage
 *
 * Purpose: This routine gets a decoded outgoing H.245 message and modifies it according to
 *          the different tasks at hand:
 *          - If tunneling: encodes it and puts it into the H.245 tunneled messages sylo
 *                          and inhibits its sending.
 *          - If not tunneling: do nothing.
 *
 * Input:   session     - The session which wants to send the message.
 *          host        - The host on which the message is to be sent.
 *          pvtNode     - The message.
 *
 * Output:  None.
 *
 * Returned Value: cmTransOK            - Message sent out.
 *                 cmTransIgnoreMessage - Message was not sent out, but all is well.
 *                 other                - Error.
 **************************************************************************************/
TRANSERR processH245OutgoingMessage(
    IN    cmTransSession    *session,
    IN    cmTransHost       *host,
    INOUT int               pvtNode)
{
    cmTransGlobals  *transGlobals;
    TRANSERR        res;
    HRPOOLELEM      msg;
    RvUint8         *buffer;
    int             encMsgSize;

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

    /* find out if we are still in tunneling procedure */
    if (!host)
    {
        RvBool addToTop = RV_FALSE;

        if (session->parallelTunnelingState == parllApproved)
        {
            /* determine if this is a TCS Ack in parallel tunneling response,
               in that case put it first */
            RvPvtNodeId node;

            __pvtGetNodeIdByFieldIds(node, transGlobals->hPvt, pvtNode,
                { _h245(response) _h245(terminalCapabilitySetAck) LAST_TOKEN} );

            if (RV_PVT_NODEID_IS_VALID(node) && !m_sessionget(session, outgoing))
                addToTop = RV_TRUE;
        }

        /* Start the encoding process */
        getEncodeDecodeBuffer(transGlobals->codingBufferSize, &buffer);
        res = transEncodeMessage(host,
                                 session,
                                 transGlobals,
                                 pvtNode,
                                 buffer,
                                 &encMsgSize);
        if (res != cmTransOK)
            return res;

        /* save the tunneled encoded message for later transmit */
        msg = saveMessageToPool(transGlobals,
                                (void *)session,
                                buffer,
                                encMsgSize,
                                RV_TRUE,
                                addToTop,
                                session->CRV);
        if (!msg)
        {
            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "processH245OutgoingMessage failed on tunneled message - no buffers"));
            return cmTransErr;
        }
    }
    else
        return cmTransOK;

    return cmTransIgnoreMessage;
}


/**************************************************************************************
 * processOutgoingMessage
 *
 * Purpose: This routine gets a decoded outgoing message and modifies it according to
 *          the different tasks at hand:
 *          - Add fields to Q.931 messages, such as multiplexing flags.
 *          - Save H.245 tunneled messages and eliminate their actual sending
 *          - Insert H.245 addresses, if necessary to the messages.
 *          - Insert H.245 tunneled messages.
 * Input:   session     - The session which wants to send the message.
 *          host        - The host on which the message is to be sent.
 *          pvtNode     - The message.
 *          type        - The type of the message (Q.931/H.245)
 *
 * Output:  None.
 *
 * Returned Value: cmTransOK            - Message sent out.
 *                 cmTransIgnoreMessage - Message was not sent out, but all is well.
 *                 other                - Error.
 **************************************************************************************/
TRANSERR processOutgoingMessage(
    IN    cmTransSession    *session,
    IN    cmTransHost       *host,
    INOUT int               pvtNode,
    IN    TRANSTYPE         type)
{
    TRANSERR        res = cmTransErr;

    switch (type)
    {
        /* handle Q.931 messages */
        case cmTransQ931Type:
        {
            RvBool sendMessage = processQ931OutgoingMessage(session, host, pvtNode);
            if (sendMessage)
                res = cmTransOK;
            else
                res = cmTransIgnoreMessage;
            break;
        }

        /* handle H.245 messages */
        case cmTransH245Type:
        {
            res = processH245OutgoingMessage(session, host, pvtNode);
            break;
        }

        default:
            break;
    }

    return res;
}

/**************************************************************************************
 * transSessionClose
 *
 * Purpose: This routine closes a session and breaks all its associations.
 *
 * Input/Output:   hsTransSession - The session to be closed.
 *
 * Output:  None.
 *
 **************************************************************************************/
void transSessionClose(cmTransSession *session)
{
    cmTransGlobals *transGlobals;
    HRPOOLELEM     msg;

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

    /* delete session's zero-timer if exist */
    RvH323TimerCancel(transGlobals->hTimers, &session->zeroTimer);

    /* remove it from the pending list, if it's there */
    removeSessionFromPendingList(transGlobals, (cmTransSession *)session);

    /* close the H.245 connection of the session */
    if (session->H245Connection)
        closeHostInternal((HSTRANSHOST)session->H245Connection, RV_TRUE);

    /* Release all nodes held on the session */
    pvtDelete(transGlobals->hPvt, session->h450Element);
    pvtDelete(transGlobals->hPvt, session->h450AlertElement);
    pvtDelete(transGlobals->hPvt, session->annexMElement);
    pvtDelete(transGlobals->hPvt, session->annexLElement);

    /* remove session from host list of sessions */
    {
        cmTransHost    *host;

        /* update, if necessary, the list header for the host */
        if (session->Q931Connection)
        {
            host = session->Q931Connection;

            /* remove the session from the host */
            RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
            if (host->firstSession == session)
                host->firstSession = session->nextSession;
            RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);

            /* if no more sessions on host, close host, if necessary */
            if (host->firstSession == NULL)
            {
                if ((host->closeOnNoSessions) || (host->remoteCloseOnNoSessions) ||
                    (!host->isMultiplexed) || (!host->remoteIsMultiplexed))
                {
                    closeHostInternal((HSTRANSHOST)host, RV_TRUE);
                }
                else
                {
                    updateHostMultiplexStatus(transGlobals, host);
                }
            }
            session->Q931Connection = NULL;
        }

        if (session->annexEConnection)
        {
            host = session->annexEConnection;

            /* remove the session from the host */
            RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
            if (host->firstSession == session)
                host->firstSession = session->nextSession;
            RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);

            /* if no more sessions on host, close host, if necessary */
            if (host->firstSession == NULL)
            {
                if ((host->closeOnNoSessions) || (host->remoteCloseOnNoSessions) ||
                    (!host->isMultiplexed) || (!host->remoteIsMultiplexed))
                {
                    closeHostInternal((HSTRANSHOST)host, RV_TRUE);
                }
                else
                {
                    updateHostMultiplexStatus(transGlobals, host);
                }
            }
            session->annexEConnection = NULL;
        }

        /* remove it from the list */
        RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
        if (session->nextSession)
            session->nextSession->prevSession = session->prevSession;
        if (session->prevSession)
            session->prevSession->nextSession = session->nextSession;
        RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
    }

    /* remove the session from the hash table */
    if (session->hashEntry)
    {
        RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
        hashDelete(transGlobals->hHashSessions, session->hashEntry);
        RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);
        session->hashEntry = NULL;
    }

    /* go over the tunneled messages still stored for the session and free them from the pool */
    msg = session->firstMessage;
    while (msg)
        msg = extractMessageFromPool(transGlobals, session, RV_TRUE);
}

/* todo: RAN: move to a static place in file and add a comment */
static RvBool tryToCloseAnnexE(IN void*  userData)
{
    cmTransHost* host = (cmTransHost*)userData;
    RvBool reuseTimer = RV_FALSE;

    if (emaLock((EMAElement)host))
    {
        cmTransGlobals* transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);
        /* make sure that the host was not revived */
        if (host->state == hostClosed)
        {
            HRPOOLELEM msg = host->firstMessage;

            if (host->counter == 0)
            {
                /* enough waiting, get rid of the message */
                while (msg)
                    msg = extractMessageFromPool(transGlobals, host, RV_FALSE);
            }
            if (msg == NULL)
            {
                /* no messages on the host, clear the timer and close it */
                RvH323TimerClear(transGlobals->hTimers, &host->connectTimer);
                transHostClose((HSTRANSHOST) host, RV_TRUE);
            }
            else
            {
                /* host did not send all messages yet, keep waiting */
                host->counter--;
                reuseTimer = RV_TRUE;
            }
        }
        else
        {
            /* actually, we should never get here. this is just to prevent the accidental loss of
            resources. Print an exception if that happens */
            RvLogExcep(&transGlobals->hLog, (&transGlobals->hLog,
                "tryToCloseAnnexE: host revived, but timer still active. may be a bug.(host %p; timer %p)",
                host, host->connectTimer));
            RvH323TimerClear(transGlobals->hTimers, &host->connectTimer);
        }
        emaUnlock((EMAElement)host);
    }
    return reuseTimer;
}


/**************************************************************************************
 * transHostClose
 *
 * Purpose: This routine reports a host close and tells the sessions connected to
 *          it that the connection has ended and releasing all the messages kept for it
 *          in the pool.
 *
 * Input/Output:   hsTransHost      - The host to be deleted.
 *                 killHost         - Should the host be removed from the hash and close
 *                                    its TPKT element or just disconnect from session?
 *
 * Output:  None.
 *
 **************************************************************************************/
void transHostClose(HSTRANSHOST hsTransHost, RvBool killHost)
{
    cmTransHost     *host;
    cmTransGlobals  *transGlobals;
    void            *sess;
    void            *nextSess;
    HRPOOLELEM      msg;

    if (!hsTransHost)
        return;

    host = (cmTransHost *)hsTransHost;

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);

    /* close the hosts tpkt elements */
    if ( (host->h245Listen) && (killHost) )
    {
        tpktClose(host->h245Listen);
        host->h245Listen = NULL;
    }

    if ( (host->hTpkt) && (killHost) )
    {
        tpktClose(host->hTpkt);
        host->hTpkt = NULL;
    }

    /* close the hosts annex E node */
    if ( (host->annexEUsageMode == cmTransUseAnnexE) && (killHost) && (transGlobals->annexECntrl != NULL) )
    {
        if (host->firstMessage != NULL)
        {
            /* there are still messages on this Annex E host. we should keep trying to send them */
            RvH323TimerCancel(transGlobals->hTimers, &host->connectTimer);
            host->counter = 5;
            host->connectTimer = RvH323TimerStartPeriodic(transGlobals->hTimers, tryToCloseAnnexE, host, 500);
            return;
        }
        /* we'll be wanting to send release complete, so we postpone the actual closing of the
        Annex E node until later */
    }

    /* We have to cancel timer which was set at "accept" time - we're closing this host anyway */
    RvH323TimerCancel(transGlobals->hTimers, &host->connectTimer);

    RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
    sess = (void *)host->firstSession;
    msg = host->firstMessage;
    RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);

    RvLogInfo(&transGlobals->hLog,
        (&transGlobals->hLog, "transHostClose(hsTransHost = %d-%p)",
        emaGetIndex((EMAElement)hsTransHost), hsTransHost));

    /* go over the messages still stored for the host and free them from the pool */
    while (msg)
        msg = extractMessageFromPool(transGlobals, host, RV_FALSE);

    RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
    if ((host->incomingMessage != NULL) || (host->incomingListeningMessage != NULL))
    {
        RvInt32 poolSize, usageSize, allocatedSize;

        if (host->incomingMessage != NULL)
        {
            rpoolFree(transGlobals->messagesRPool, host->incomingMessage);
            host->incomingMessage = NULL;
        }
        if (host->incomingListeningMessage != NULL)
        {
            rpoolFree(transGlobals->messagesRPool, host->incomingListeningMessage);
            host->incomingListeningMessage = NULL;
        }

        if (rpoolStatistics(transGlobals->messagesRPool, &poolSize, &usageSize, &allocatedSize))
        {
            RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                    "rpoolFree[transHostClose] statistics: max=%d, usage=%d, allocated=%d",
                    poolSize, usageSize, allocatedSize));
        }
    }
    RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);

    /* go over all the sessions that are connected to this host */

    while ((sess != NULL) && (emaMarkLocked((EMAElement)sess)))
    {
        /* report to the user that the connection of the session was closed */
        HATRANSSESSION haTransSession;
        cmTransSession *session = (cmTransSession *)sess;
        RvBool         hostReported;
        RvBool         anotherQ931HostExists;


        {
            haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

            RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
            nextSess = session->nextSession;
            RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);

            /* determine whether the closing host was reported to the user, so we need to report its
               closing too */
            hostReported = ( ((session->Q931Connection == host)   && m_sessionget(session, reportedQ931Connect)) ||
                             ((session->annexEConnection == host) && m_sessionget(session, reportedQ931Connect)) ||
                             ((session->H245Connection == host)   && m_sessionget(session, reportedH245Connect)) );

            /* check if it's a Q.931 host and if ther are two (TPKT & annex E) so the closing is due to
               a race between them, and no reporting is needed to the user at this stage */
            anotherQ931HostExists = ( ((session->Q931Connection == host) || (session->annexEConnection == host)) &&
                                      (session->Q931Connection && session->annexEConnection) );

            if ( (transGlobals->sessionEventHandlers.cmEvTransConnectionOnSessionClosed) &&
                 hostReported && (!anotherQ931HostExists))
            {
                /* in case of tunneling report also the close of the H.245 tunneled connection */
                if ( (host->type == cmTransQ931Conn) &&
                     (session->tunnelingState == tunnApproved) &&
                     m_sessionget(session, reportedH245Connect) )
                {
                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog,
                            "cmEvTransConnectionOnSessionClosed(hsSession = %d(%p), haSession=%p type=cmTransH245Conn)",
                            emaGetIndex((EMAElement)session), session, haTransSession));

                    transGlobals->sessionEventHandlers.cmEvTransConnectionOnSessionClosed(
                                                              (HSTRANSSESSION)session,
                                                              haTransSession,
                                                              cmTransH245Conn);
                }

                if (!emaWasDeleted((EMAElement)session))
                {
                    /* report the actual disconnect */
                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog,
                            "cmEvTransConnectionOnSessionClosed(hsSession = %d(%p), haSession=%p type=%d)",
                            emaGetIndex((EMAElement)session), session, haTransSession, host->type));

                    transGlobals->sessionEventHandlers.cmEvTransConnectionOnSessionClosed(
                                                              (HSTRANSSESSION)session,
                                                              haTransSession,
                                                              host->type);
                }
            }


            if (!emaWasDeleted((EMAElement)session))
            {
                /* disconnect the session from the host */
                if (host->type == cmTransH245Conn)
                {
                    session->H245Connection      = NULL;

                    /* for H.245 there is only one session per host and it's the first that the host points too */
                    nextSess = NULL;
                }
                else
                {
                    if (host == session->Q931Connection)
                        session->Q931Connection = NULL;
                    else
                    if (host == session->annexEConnection)
                        session->annexEConnection = NULL;
                }
            }

            emaReleaseLocked((EMAElement)session);
        }

        /* Get the next session */
        sess = (void *)nextSess;
    }

    if ( (host->annexEUsageMode == cmTransUseAnnexE) && (killHost) && (transGlobals->annexECntrl != NULL) )
    {
        /* now, close the annex E node */
        annexEStatus eStat;

        eStat = annexECloseNode(transGlobals->annexECntrl,
                                &host->remoteAddress, &host->localAddress);
        if ((eStat != annexEStatusNormal) && host->hostIsApproved)
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "failed to close annex E node on host=%p", host));
        }
    }

    /* remove it from its hash table */
    if ( (host->hashEntry) && (killHost) )
    {
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "hashDelete = %p", host->hashEntry));

        RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
        hashDelete(transGlobals->hHashHosts, host->hashEntry);
        RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);
        host->hashEntry = NULL;
    }

    if (!emaWasDeleted((EMAElement)host))
    {
        /* Make sure this host is not scheduled for garbage collection - it's dead as it is */
        /* The "ugly" way of doing that is fooling the garbage collection and indicating the
           existence of a session */
        host->firstSession = (cmTransSession*)1;
        updateHostMultiplexStatus(transGlobals, host);
        host->firstSession = NULL;

        /* Now we can delete it */
        emaDelete((EMAElement)host);
    }
}

/**************************************************************************************
 * findSessionByMessage
 *
 * Purpose: looks according to the CRV and callID of the message for an entry of the session
 *          in the host's hash table. If none was found, creates an entry.
 *          In case of non-session related messages (CRV = 0) the routine treats it
 *          as an error and expect the upper layer to handle the case.
 *
 *          If (releaseReason == NULL) && (*session == NULL), then this function will
 *          only look for a session - it will not create a new session for incoming setup
 *          messages, it will not try to determine if Annex E or TPKT won and it will not
 *          handle messages as first outgoing messages on a session.
 *
 * Input:   transGlobals    - The global data of the transport module.
 *          host            - The host on which the message arrived.
 *          pvtNode         - The decoded message.
 *          isAnnexE        - is the host on which the message came is an annex E host?
 *
 * Output:  session         - The session found or created.
 *          releaseReason   - Release complete reason to set if an error occured.
 *
 * returned value: RV_TRUE - all is OK (if session==NULL in this case, then the message
 *                           is a general one to the host and not session-specific).
 *                 RV_FALSE - No session was found/created due to a possible error.
 *                            If this is an incoming setup message, then releaseReason
 *                            holds the reason for releasing this call.
 *
 **************************************************************************************/
RvBool findSessionByMessage(
    IN  cmTransGlobals *transGlobals,
    IN  cmTransHost    *host,
    IN  RvPvtNodeId    pvtNode,
    IN  RvBool         isAnnexE,
    OUT cmTransSession **session,
    OUT RvPstFieldId   *releaseReason)
{
    RvUint16        CRV = 0;
    RvInt32         temp = 0;
    RvUint8         callID[16];
    RvUint8         nullCallID[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int             msgType;
    RvBool          bNullCallId = RV_FALSE;
    int             res;
    RvBool          bSearchOnly;

    void            *loc;
    sessionKey      key;
    RvBool          found;

    bSearchOnly = (releaseReason == NULL);

    /* check what type of Q.931 message we have here */
    msgType = pvtGetChildTagByPath(transGlobals->hPvt, pvtNode, "message", 1);
    if (msgType < 0)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "findSessionByMessage failed to get message type tag"));
        if (releaseReason != NULL)
            *releaseReason = __q931(undefinedReason);
        return RV_FALSE;
    }

    /* get the CRV and callID (if exists) of the message and build the key for CAT */
    __pvtGetByFieldIds(res, transGlobals->hPvt,pvtNode,
                        {
                          _q931(callReferenceValue)
                          _q931(twoBytes)
                          LAST_TOKEN
                        },
                        NULL,(RvInt32 *)&temp,NULL);
    CRV = (RvUint16)temp;

    __pvtGetNodeIdByFieldIds(res, transGlobals->hPvt, pvtNode,
                            { _q931(message)
                              _anyField
                              _q931(userUser)
                              _q931(h323_UserInformation)
                              _q931(h323_uu_pdu)
                              _q931(h323_message_body)
                              _anyField
                              _q931(callIdentifier)
                              _q931(guid)
                              LAST_TOKEN
                            });
    if (res>=0)
    {
        pvtGetString(transGlobals->hPvt, res, sizeof(callID), (char *)callID);
        if (memcmp(nullCallID, callID, sizeof(callID))==0)
            bNullCallId = RV_TRUE;
    }
    else
    {
        bNullCallId = RV_TRUE;
        memcpy(callID, nullCallID, sizeof(callID));
    }

    /* check if this is a message not attached to a particular session,
       i.e. CRV = 0, callID = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} */

    if ( (CRV == 0) && (bNullCallId) )
    {
        *session = NULL;

        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "findSessionByMessage found a general message"));
        return RV_TRUE;
    }

    if (!*session)
    {
        /* no session exists yet, this is an incoming message */

        /* since its an incoming message change the bit direction of the CRV */
        CRV ^= 0x8000;

        /* look for the entry in the hash */
        key.CRV  = CRV;
        found    = RV_FALSE;

        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog,
                "findSessionByMessage incoming CRV (findHash)=%x host=%d(%p)",
                key.CRV, emaGetIndex((EMAElement)host), host));

        /* go over all the sessions with the same CRV from this host, hopefully
           there is only one (in most cases) */
        RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
        for (loc = hashFind(transGlobals->hHashSessions,(void *)&key);
             (loc != NULL) && (!found);
             loc = hashFindNext(transGlobals->hHashSessions, (void *)&key, loc))
        {
            cmTransSession *sess = *(cmTransSession **)hashGetElement(transGlobals->hHashSessions,
                                                                      loc);
            /* check that the session has the same call ID as the message */
            if (memcmp(sess->callId, callID, sizeof(callID)) == 0)
            {
                *session = sess;
                found = RV_TRUE;
            }
            else
            /* if we don't have call ID (version 1 or CRV0 messages), check that
               the session is from our host */
            if (bNullCallId)
            {
                /* check first for the TPKT connection */
                if ((!isAnnexE) &&
                    (sess->Q931Connection) &&
                    (cmCompareTransportAddresses(&sess->Q931Connection->remoteAddress, &host->remoteAddress)))
                {
                    *session = sess;
                    found = RV_TRUE;
                }

                /* our last chance that it's from the annex E connection */
                if ((isAnnexE) &&
                    (sess->annexEConnection) &&
                    (cmCompareTransportAddresses(&sess->annexEConnection->remoteAddress, &host->remoteAddress)))
                {
                    *session = sess;
                    found = RV_TRUE;
                }
            }
        }
        RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);

        if (bSearchOnly)
            return found;

        /* no session was found, this must be a new incoming message */
        if (!found)
        {
            TRANSERR err = cmTransErr;

            /* we may create new session only for new incoming SETUP message */
            switch (msgType)
            {
            case (int)cmQ931setup:
                /* create a new session */
                RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
                err = cmTransCreateSession((HAPPTRANS)transGlobals,
                    NULL, (HSTRANSHOST)host, (HSTRANSSESSION *)session);
                RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
                break;

            case (int)cmQ931statusEnquiry:
                /* message on a non-existing call. we send the reply here so we don't need
                   to pass anything important along */
                {
                    RvPvtNodeId newMessage;
                    if (createMessage(transGlobals, cmQ931status, CRV, callID, &newMessage))
                    {
                        /* PLACE MISSING FIELDS */
                        int res;

                        /* set the reason */
                        __pvtBuildByFieldIds(res, transGlobals->hPvt, newMessage,
                                                { _q931(message)
                                                  _q931(status)
                                                  _q931(cause)
                                                  _q931(octet4)
                                                  _q931(causeValue)
                                                 LAST_TOKEN
                                                },
                                             30, NULL);
                        /* set the call state */
                        __pvtBuildByFieldIds(res, transGlobals->hPvt, newMessage,
                                                { _q931(message)
                                                  _q931(status)
                                                  _q931(callState)
                                                  _q931(callStateValue)
                                                 LAST_TOKEN
                                                },
                                             0, NULL);
                        /* set the current multiplex params into the message */
                        setMultiplexedParams(transGlobals,NULL,host,newMessage,(int)cmQ931status);

                        if (RV_PVT_NODEID_IS_VALID(res) && !encodeAndSend(transGlobals, host, newMessage))
                        {
                            RvLogError(&transGlobals->hLog,
                                (&transGlobals->hLog, "findSessionByMessage Failed on send Status message for host %d(%p)",
                                    emaGetIndex((EMAElement)host), host));
                        }

                        pvtDelete(transGlobals->hPvt, newMessage);
                    }
                }

                return RV_FALSE;

            default:
                if (host->annexEUsageMode != cmTransUseAnnexE)
                {
                    RvLogWarning(&transGlobals->hLog,
                        (&transGlobals->hLog,
                            "findSessionByMessage new incoming message which is not SETUP, type=%d", msgType));
                }
                else
                {
                    RvLogDebug(&transGlobals->hLog,
                        (&transGlobals->hLog,
                            "findSessionByMessage new incoming message which is not SETUP on Annex E host, type=%d", msgType));
                }
                *releaseReason = __q931(undefinedReason);
                return RV_FALSE;
            }

            if (err != cmTransOK)
            {
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "findSessionByMessage failed to create a new session"));
                *releaseReason = __q931(adaptiveBusy);
                return RV_FALSE;
            }
            else
            {
                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "findSessionByMessage created a new session %d(%p)",
                        emaGetIndex((EMAElement)*session), *session));

                /* update the sessions callIs and CRV and mark it as incoming session */
                memcpy((*session)->callId, callID, sizeof(callID));
                (*session)->CRV = CRV;
                m_sessionset(*session, outgoing, RV_FALSE);

                /* add the new session to the hash table */
                key.CRV  = CRV;
                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "findSessionByMessage incoming CRV (AddHash)=%x host=%d(%p) session=%d(%p)",
                        key.CRV,emaGetIndex((EMAElement)host), host, emaGetIndex((EMAElement)*session), *session));
                RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
                loc = hashAdd(transGlobals->hHashSessions,(void *)&key,(void *)session, RV_FALSE);
                RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);
                if (!loc)
                {
                    cmTransCloseSession((HSTRANSSESSION)*session);
                    if ((host->annexEUsageMode == cmTransNoAnnexE) && (host->firstSession == NULL))
                        host->remoteIsMultiplexed = RV_FALSE; /* Make sure this host will not be left open */

                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog,
                            "findSessionByMessage failed to add session to hash table"));
                    *releaseReason = __q931(adaptiveBusy);
                    return RV_FALSE;
                }
                else
                {
                    (*session)->hashEntry = loc;
                }
            }
        }
        else
        if (msgType == (int)cmQ931setup)
        {
            if ((!isAnnexE && ((*session)->annexEConnection != NULL)) ||
                (isAnnexE && ((*session)->Q931Connection != NULL)))
            {
                /* We are using both the Annex E and the TPKT connection. we should send a
                status message, confirming the use of one or the other. */
                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "findSessionByMessage found session %d (%p) for an incoming setup, already using different host",
                        emaGetIndex((EMAElement)*session), *session));
            }
            else
            {
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "findSessionByMessage found session %d (%p) for an incoming setup message !!!!",
                        emaGetIndex((EMAElement)*session), *session));
                *releaseReason = __q931(invalidCID);
            }
            return RV_FALSE;
        }

        /* connect the session to the host - I assume the winning host was already set from the
           TPKT, so no point in changing locks (cross your fingers) */
        if (!isAnnexE)
            (*session)->Q931Connection   = host;
        else
            (*session)->annexEConnection = host;

        /* add the session to the hosts list of session.
           Do it just for the first time, i.e. when the SETUP
           message arrives.
           (in case of TPKT & annex E race, done just for TPKT host,
           for annex E this will be done if and when it will win the race */
        if ( (!isAnnexE) && (msgType == (int)cmQ931setup) )
        {
            RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
            /* connect the session to the host */
            (*session)->nextSession = host->firstSession;
            if ((*session)->nextSession)
                (*session)->nextSession->prevSession = (*session);
            host->firstSession = (*session);
            RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
        }

        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "findSessionByMessage found session %d (%p)",
                emaGetIndex((EMAElement)*session), *session));
    }
    else
    {
        /* if session exists this means this is a first outgoing message on a session
            created by the user, add it to the hash table
        */
        if (!(*session)->hashEntry)
        {
            loc  = NULL;
            key.CRV  = CRV;

            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "findSessionByMessage outgoing CRV (AddHAsh)=%x host=%d(%p) session=%d(%p)",
                    key.CRV, emaGetIndex((EMAElement)host), host, emaGetIndex((EMAElement)*session), *session));

            RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
            loc = hashAdd(transGlobals->hHashSessions,(void *)&key,(void *)session, RV_FALSE);
            RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);

            if (!loc)
            {
                cmTransCloseSession((HSTRANSSESSION)*session);

                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "findSessionByMessage failed to add session to hash table"));
                if (releaseReason != NULL)
                    *releaseReason = __q931(adaptiveBusy);
                return RV_FALSE;
            }
            else
            {
                memcpy((*session)->callId, callID, sizeof(callID));
                (*session)->CRV = CRV;
                (*session)->hashEntry = loc;
            }
        }
    }

    return RV_TRUE;
}

/**************************************************************************************
 * setRemoteAddress
 *
 * Purpose: sets the remote address to the host and adds an entry to the host hash table
 *
 * Input:   host            - The host to which the address is to be set.
 *          remoteAddress   - The address to be set.
 *
 * Return Value: cmTransOK - if all is well; cmTranErr - otherwise
 *
 **************************************************************************************/
TRANSERR setRemoteAddress(cmTransHost *host, cmTransportAddress *remoteAddress)
{
    void *res;
    hostKey key;

    /* retrieve the transport module global area */
    cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);

    if (!remoteAddress)
        return cmTransErr;

    if (host->type == cmTransH245Conn)
    {
        host->remoteAddress = *remoteAddress;
        return cmTransOK;
    }

    if ((((int)remoteAddress->type) >= 0) && (host->hashEntry == NULL))
    {
        host->remoteAddress = *remoteAddress;

        key.ipNum = condenseIpTo32bit(&host->remoteAddress);
        key.port  = (RvUint32)host->remoteAddress.port;
        key.type  = (host->annexEUsageMode != cmTransNoAnnexE);

        RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
        res = hashAdd(transGlobals->hHashHosts,(void *)&key, (void *)&host, RV_FALSE);
        RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "hashAdd=%p [hostKey: ipNum=%x port=%d type=%d]",
                res, key.ipNum, key.port, key.type));

        if (!res)
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "setRemoteAddress failed on updating hosts hash table"));
            return cmTransErr;
        }
        else
            host->hashEntry = res;
    }
    return cmTransOK;
}

/************************************************************************************
 * canWeAcceptTheCall
 *
 * Purpose: to check if this is the first call on this host, or if this host supports
 *          multiplexing. If this host doesn't support multiplexing and already has a
 *          session attached to it, refuse the message and send releaseComplete on
 *          the call.
 *
 * Input: transGlobals - The global variables of the module
 *        host         - the host through which the message was received.
 *        pvtNode      - the decoded message
 *
 * Output: None
 *
 * returned value: RV_TRUE  - accept the message
 *                 RV_FALSE - refuse it and disconnect the call.
 *
 **************************************************************************************/
RvBool canWeAcceptTheCall(cmTransGlobals *transGlobals, cmTransHost *host, int pvtNode)
{
    int         msgType;
    RvUint16    CRV = 0;
    RvUint8     callId[16];
    RvBool      sendReleaseComplete = RV_FALSE;
    RvBool      sendMessage = RV_TRUE;
    RvBool      oor = RV_FALSE;
    int res = 0;

    /* check what type of Q.931 message we have here */
    msgType = pvtGetChildTagByPath(transGlobals->hPvt, pvtNode, "message", 1);
    if (msgType < 0)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog,
                "canWeAcceptTheCall failed to get message type tag"));
        return RV_FALSE;
    }

    /* if first incoming message on the call, check if the new call can be accepted */
    if (msgType == (int)cmQ931setup)
    {
        /* get the call Id */
        __pvtGetNodeIdByFieldIds(res, transGlobals->hPvt, pvtNode,
            { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu)
                _q931(h323_message_body) _anyField _q931(callIdentifier) _q931(guid) LAST_TOKEN });
        if (RV_PVT_NODEID_IS_VALID(res))
        {
            res = pvtGetString(transGlobals->hPvt, res, sizeof(callId), (char *)callId);
        }

        /* if host already has one or more calls on it */
        if ((host->firstSession) &&
        /* and host no longer (or never) supports multiplexing */
            ((!host->isMultiplexed) || (!host->remoteIsMultiplexed)))
        {
            /* first see if this message belongs to this session */
            if (res >= 0)
            {
                /* maybe this is an incoming SETUP on an outgoing connection, check host direction and call ID */
                if (!host->incoming || (memcmp(callId, host->firstSession->callId, 16) != 0))
                {
                    /* bad */
                    sendMessage         = RV_FALSE;
                    sendReleaseComplete = RV_TRUE;
                    oor = RV_FALSE;
                }
            }
        }
        if (!sendReleaseComplete)
        {
            /* See if we have an available session to accept the new call on */
            emaStatistics stats;
            RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
            if( (emaGetStatistics(transGlobals->hEmaSessions, &stats) < 0) ||
                (stats.elems.cur == (stats.elems.max-1)) )
            {
                /* we cannot accept call */
                sendMessage         = RV_FALSE;
                sendReleaseComplete = RV_TRUE;
                oor = RV_TRUE;

            }
            RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
        }
    }

    /* get the callId and CRV from the SETUP message */
    if (sendReleaseComplete)
    {

        RvInt32 temp;

        __pvtGetByFieldIds(res, transGlobals->hPvt,pvtNode,
                            {
                              _q931(callReferenceValue)
                              _q931(twoBytes)
                              LAST_TOKEN
                            },
                            NULL,(RvInt32 *)&temp,NULL);
        if (res>=0)
        {
            CRV = (RvUint16)temp;
            /* it's incoming message, so turn the last bit */
            CRV ^= 0x8000;
        }

        if (res < 0)
        {
            /* something is wrong */
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "canWeAcceptTheCall Failed to extract CRV & callID from SETUP"));
        }
        else
        {
            if(oor)
                sendReleaseCompleteMessage(transGlobals, host, CRV, callId, -1, __q931(adaptiveBusy));
            else
                sendReleaseCompleteMessage(transGlobals, host, CRV, callId, -1, __q931(newConnectionNeeded));
        }
    }

    return sendMessage;
}


/******************************************************************************
 * encodeAndSend
 * ----------------------------------------------------------------------------
 * General: Encodes and sends a given decoded message on a given host.
 *          No checks and no processing is done on the message.
 *          Note: The node of the decoded message remains untouched.
 *
 * Return Value: RV_TRUE: message was sent, RV_FALSE: sending failed.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  transGlobals - The global variables of the module.
 *         host         - The host through which the message is to be sent.
 *         newMessage   - The decoded message to send.
 * Output: none
 *****************************************************************************/
RvBool encodeAndSend(
    IN cmTransGlobals   *transGlobals,
    IN cmTransHost      *host,
    IN RvPvtNodeId      newMessage)
{
    RvUint8     *buffer;
    int         res = 0;
    int         encoded;

    getEncodeDecodeBuffer(transGlobals->codingBufferSize, &buffer);

    if ( (transGlobals->hostEventHandlers.cmEvTransSendRawMessage) && (host) )
    {
        TRANSERR    encTransRes;
        HATRANSHOST haTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)host);

        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog,
                "cmEvTransSendRawMessage(host=%d(%p), haHost=%p, session=NULL, hsSession=NULL, pvtNode=%d)",
                emaGetIndex((EMAElement)host), host, haTransHost, newMessage));

        encTransRes = transGlobals->hostEventHandlers.cmEvTransSendRawMessage(
                        (HSTRANSHOST)host,
                        haTransHost,
                        NULL,
                        NULL,
                        newMessage,
                        transGlobals->codingBufferSize,
                        buffer,
                        &encoded);
        if (encTransRes != cmTransOK)
            res = -1;
    }
    else
        res = cmEmEncode(transGlobals->hPvt,
                         newMessage,
                         buffer,
                         transGlobals->codingBufferSize,
                         &encoded);

    if (res < 0)
    {
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog,
                "encodeAndSend failed to encode for host %d(%p)",emaGetIndex((EMAElement)host), host));
        return RV_FALSE;
    }

    return sendRawMessage(transGlobals, host, buffer, (RvSize_t)encoded);
}


/******************************************************************************
 * sendRawMessage
 * ----------------------------------------------------------------------------
 * General: Sends a given raw buffer as a message on a given host.
 *          No checks and no processing is done on the message.
 *
 * Return Value: RV_TRUE: message was sent, RV_FALSE: sending failed.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  transGlobals - The global variables of the module.
 *         host         - The host through which the message is to be sent.
 *         buffer       - Buffer to send as the message.
 *         bufferSize   - Size of buffer to send.
 * Output: none
 *****************************************************************************/
RvBool sendRawMessage(
    IN cmTransGlobals   *transGlobals,
    IN cmTransHost      *host,
    IN RvUint8          *buffer,
    IN RvSize_t         bufferSize)
{
    HRPOOLELEM  msg;
    TRANSERR    err;

    msg = saveMessageToPool(transGlobals,
                            (void *)host,
                            buffer,
                            (int)bufferSize,
                            RV_FALSE,
                            RV_FALSE,
                            0);

    if (!msg)
    {
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog,
                "encodeAndSend has no buffers for host %d(%p)",emaGetIndex((EMAElement)host), host));
        return RV_FALSE;
    }

    /* initiate the actual send through the appropriate protocol */
    err = sendMessageOnHost(transGlobals, host);
    if ( (err != cmTransOK) && (err != cmTransConnectionBusy) )
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog,
                "encodeAndSend Failed on send for host %d(%p)",emaGetIndex((EMAElement)host), host));
        return RV_FALSE;
    }

    return RV_TRUE;
}


/******************************************************************************
 * receiveRawQ931Message
 * ----------------------------------------------------------------------------
 * General: Receives a given raw buffer as a message on a Q.931 host.
 *
 * Return Value: RV_TRUE: message was sent, RV_FALSE: sending failed.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  transGlobals - The global variables of the module.
 *         host         - The host through which the message is to be sent.
 *         buffer       - Buffer to send as the message.
 *         bufferSize   - Size of buffer to send.
 * Output: none
 *****************************************************************************/
RvBool receiveRawQ931Message(
    IN cmTransGlobals   *transGlobals,
    IN cmTransHost      *host,
    IN RvUint8          *buffer,
    IN RvSize_t         bufferSize);


/************************************************************************************
 * sendGeneralFacility
 *
 * Purpose: creates a general purpose facility message with CRV and callId set to zero.
 *          sets in it the host's current multiplex parameters, encodes it and send it
 *          on the host.
 *
 * Input: transGlobals - The global variables of the module
 *        host         - the host through which the message is to be sent.
 *
 * Output: None
 *
 **************************************************************************************/
void sendGeneralFacility(cmTransGlobals *transGlobals, cmTransHost *host)
{
    int      newMessage = -1;
    RvUint8  nullCallId[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    /* create facility with CRV and callId = 0 */
    if (createMessage(transGlobals, cmQ931facility, 0, nullCallId, &newMessage))
    {
        int res;

        /* set the reason */
        __pvtBuildByFieldIds(res, transGlobals->hPvt, newMessage,
                                { _q931(message)
                                  _q931(facility)
                                  _q931(userUser)
                                  _q931(h323_UserInformation)
                                  _q931(h323_uu_pdu)
                                  _q931(h323_message_body)
                                  _anyField
                                  _q931(facility)
                                  _q931(undefinedReason)
                                 LAST_TOKEN
                                },
                             0, NULL);
        /* set the current multiplex params into the message */
        setMultiplexedParams(transGlobals,NULL,host,newMessage,(int)cmQ931facility);

        if (RV_PVT_NODEID_IS_VALID(res) && !encodeAndSend(transGlobals, host, newMessage))
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "sendGeneralFacility Failed on send for host %d(%p)",
                    emaGetIndex((EMAElement)host), host));
        }

        pvtDelete(transGlobals->hPvt, newMessage);
    }
}

/************************************************************************************
 * initiateTunnelingFacility
 *
 * Purpose: if we are in a 'stable' state we need to initiate a facility send
 *          to get rid of the tunneled messages, and initiate it.
 *              The condition for that are:
 *              a. We have tunneled messages to send
 *              b. At least one message was already sent from this end
 *              c. Tunneling was approved
 *
 * Input: transGlobals - The global variables of the module
 *        session      - the session whose state is tested.
 *
 * Output: None
 *
 **************************************************************************************/
void initiateTunnelingFacility(cmTransGlobals *transGlobals,
                               cmTransSession *session)
{
    RvBool messagesExist = ( (session->h450Element >= 0) ||
                             (session->annexMElement >= 0) ||
                             (session->annexLElement >= 0) ||
                             (
                               (session->firstMessage != NULL) && !m_sessionget(session, holdTunneling) &&
                               transGlobals->sessionEventHandlers.cmEvTransWantsToStartControl(
                                   (HSTRANSSESSION)session, emaGetApplicationHandle((EMAElement*)session))
                             )
                           );

    RvBool tunnelingIsStable = (  (   (session->tunnelingState == tunnApproved) ||
                                      (session->tunnelingState == tunnNo) ) &&
                                  (   (session->parallelTunnelingState == parllNo) ||
                                      (   (session->parallelTunnelingState == parllApproved) &&
                                          (session->faststartState != fsOffered) ) ) );

    if (messagesExist  &&
        m_sessionget(session, firstMessageSent) &&
        (!m_sessionget(session, outgoing) || m_sessionget(session, setupSent)) &&
        !m_sessionget(session, callTerminating) &&
        (tunnelingIsStable || m_sessionget(session, forceTunneledMessage)))
    {
        int      newMessage = -1;
        TRANSERR err;

        if (createMessage(transGlobals, cmQ931facility, session->CRV,
            session->callId, &newMessage))
        {
            int res;
            char remoteVer[8];

            cmGetH225RemoteVersion((HCALL)emaGetApplicationHandle((EMAElement)session),remoteVer);

            if (strcmp(remoteVer, "4") >= 0)
            {
                /* version 4 or later */
                __pvtBuildByFieldIds(res, transGlobals->hPvt, newMessage,
                                        { _q931(message)
                                          _q931(facility)
                                          _q931(userUser)
                                          _q931(h323_UserInformation)
                                          _q931(h323_uu_pdu)
                                          _q931(h323_message_body)
                                          _q931(facility)
                                          _q931(reason)
                                          _q931(transportedInformation)
                                          LAST_TOKEN
                                        },
                                        0, NULL);
            }
            else
            {
                /* pre version 4 */
                __pvtBuildByFieldIds(res, transGlobals->hPvt, newMessage,
                                        { _q931(message)
                                          _q931(facility)
                                          _q931(userUser)
                                          _q931(h323_UserInformation)
                                          _q931(h323_uu_pdu)
                                          _q931(h323_message_body)
                                          _q931(empty)
                                          LAST_TOKEN
                                        },
                                        0, NULL);
            }
            err = cmTransSendMessage((HSTRANSSESSION)session, newMessage, cmTransQ931Type);
            pvtDelete(transGlobals->hPvt, newMessage);
            if (err != cmTransOK)
            {
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "initiateTunnelingFacility failed to send facility (transportedInformation) message session=%d(%p)",
                        emaGetIndex((EMAElement)session), session));
            }
            else
            {
                m_sessionset(session, forceTunneledMessage, RV_FALSE);
            }
            RV_UNUSED_ARG(res);
        }
        else
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "initiateTunnelingFacility failed to create facility (transportedInformation) message session=%d(%p)",
                    emaGetIndex((EMAElement)session), session));
        }
    }
}

/************************************************************************************
 * sendReleaseCompleteMessage
 *
 * Purpose: initiate a send of releaseComplete in case that:
 *              a.  A call was initiated on a non multiplexed connection that
 *                  has already other call(s).
 *              b.  We ran out of resources (mainly sessions)
 *
 * Input: transGlobals - The global variables of the module
 *        host         - the host through which the message is to be sent.
 *        CRV          - the CRV to attach to the message
 *        callId       - the call identifier to attach to the message
 *        cause        - The call release cause
 *        reasonNameId - field Id of the disconnect reason.
 *
 * Output: None
 *
 **************************************************************************************/
void sendReleaseCompleteMessage(cmTransGlobals  *transGlobals,
                                cmTransHost     *host,
                                RvUint16        CRV,
                                RvUint8         *callId,
                                int             cause,
                                RvPstFieldId    reasonNameId)
{
    RvPvtNodeId newMessage = RV_PVT_INVALID_NODEID;
    RvPvtNodeId node = RV_PVT_INVALID_NODEID;

    /* create releaseComplete with the CRV and callID and
       put the disconnection reason into it */
    if (createMessage(transGlobals, cmQ931releaseComplete, CRV, callId, &newMessage))
    {
        if (RV_PVT_NODEID_IS_VALID(reasonNameId))
        {
            __pvtBuildByFieldIds(node,transGlobals->hPvt, newMessage,
                            { _q931(message)
                              _q931(releaseComplete)
                              _q931(userUser)
                              _q931(h323_UserInformation)
                              _q931(h323_uu_pdu)
                              _q931(h323_message_body)
                              _q931(releaseComplete)
                              _q931(reason)
                              LAST_TOKEN
                            },
                            0, NULL);

            if (RV_PVT_NODEID_IS_VALID(node))
                node = pvtAdd(transGlobals->hPvt, node, reasonNameId, 0, NULL, NULL);
        }

        if (cause >= 0)
        {
            RvPvtNodeId octet3;
            __pvtBuildByFieldIds(node, transGlobals->hPvt, newMessage,
                {_q931(message) _q931(releaseComplete) _q931(cause) LAST_TOKEN}, 0, NULL);
            octet3 = pvtAdd(transGlobals->hPvt, node, __q931(octet3), 0, NULL, NULL);
            pvtAdd(transGlobals->hPvt, octet3, __q931(codingStandard), 0, NULL, NULL);
            pvtAdd(transGlobals->hPvt, octet3, __q931(spare), 0, NULL, NULL);
            pvtAdd(transGlobals->hPvt, octet3, __q931(location), 0, NULL, NULL);
            __pvtBuildByFieldIds(node, transGlobals->hPvt, node, {_q931(octet4) _q931(causeValue) LAST_TOKEN}, cause, NULL);
        }
    }

    if (RV_PVT_NODEID_IS_VALID(node))
    {
        /* Start the encoding process */
        if (!encodeAndSend(transGlobals, host, newMessage))
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "sendReleaseCompleteMessage Failed to send releaseComplete message (host=%p)", host));
        }
    }
    else
    {
        /* something is wrong */
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog,
                "sendReleaseCompleteMessage Failed to insert cause and reason to message (host=%p)", host));
    }

    /* delete the message */
    pvtDelete(transGlobals->hPvt, newMessage);
}

/************************************************************************************
 * getGoodAddress
 *
 * Purpose: Calculate the ip address as follows:
 *
 *          a. Try and get the allocated ip address from the TCP/IP stack.
 *          b. if 0, try and get the Q.931 connection ip address.
 *          c. if 0, use the localIPAddress as was given in cmTransStart.
 *          d. if 0, just leave it as 0 since we might want to use a different
 *             LAN interface than the one of the first IP address.
 *
 *          Note: for Q.931 connections, step a is being skipped.
 *
 * Input: transGlobals - The global variables of the module.
 *        hTpkt        - The tpkt element of the socket whose address we want to find.
 *        q931         - The Q.931 connection of a given H.245 socket (relevent only when
 *                       type is cmTransH245Conn).
 *        type         - The type of the connection: Q.931, H.245 or annexE.
 *
 * Output: addr - The calculated good ip address.
 *
 **************************************************************************************/
void getGoodAddress(cmTransGlobals     *transGlobals,
                    HTPKT              hTpkt,
                    cmTransHost        *q931Host,
                    TRANSCONNTYPE      type,
                    cmTransportAddress *addr)
{
    static RvAddress localHostAddrArray[10];
    static RvUint32 numAddrs = RV_UINT32_MAX;
    static RvAddress *localHostAddr = NULL;
    cmTransportAddress localAddr;
    RvUint16 savedPort;
    RvUint i;

    memcpy(&localAddr, addr, sizeof(cmTransportAddress));

    if (addr == NULL)
        return;

    if (numAddrs == RV_UINT32_MAX)
    {
        /* First time around - get the addresses from the OS */
        numAddrs = (RvUint32)(sizeof(localHostAddrArray)/sizeof(localHostAddrArray[0]));
        RvHostLocalGetAddress(transGlobals->logMgr, &numAddrs, localHostAddrArray);

        localHostAddr = &(localHostAddrArray[0]);

#if (RV_NET_TYPE & RV_NET_IPV6)
        if (addr->type == cmTransportTypeIPv6)
        {
            int scopeId = -1;
            /* be sure to ignore the damn loop-back address */
            RvChar loopBackIp[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
            for (i=0; i<numAddrs; i++)
            {
                if (localHostAddrArray[i].addrtype == RV_ADDRESS_TYPE_IPV6)
                {
                    scopeId++;
                    if (memcmp(localHostAddrArray[i].data.ipv6.ip, loopBackIp, 16) == 0)
                        continue;
                    localHostAddr = &(localHostAddrArray[i]);
                    localHostAddr->data.ipv6.scopeId = scopeId;
                    /* if this is the local-link address, keep trying to bind a better address */
                    if ((localHostAddrArray[i].data.ipv6.ip[0] != 0xfe) ||
                        (localHostAddrArray[i].data.ipv6.ip[1] != 0x80))
                        break;
                }
            }
        }
        else
#endif
            if (addr->type == cmTransportTypeIP)
        {
            /* be sure to ignore the damn loop-back address */
            RvChar loopBackIp[4] = {0x7F,0x00,0x00,0x01};
            for (i=0; i<numAddrs; i++)
            {
                if ((localHostAddrArray[i].addrtype == RV_ADDRESS_TYPE_IPV4) &&
                    (memcmp(&localHostAddrArray[i].data.ipv4.ip, loopBackIp, 4) != 0))
                {
                    localHostAddr = &(localHostAddrArray[i]);
                    break;
                }
            }
        }
    }

    if (hTpkt)
    {
        if ((tpktGetAddresses(hTpkt, &localAddr, NULL) != RV_OK) || !cmTaHasIp(&localAddr))
        {
            RvUint16 port = localAddr.port;
            RvH323CoreToCmAddress(localHostAddr, &localAddr);
            if (port != 0)
                localAddr.port = port;
        }
    }

    /* calculate the right ip address - we make sure that unknown IP types are treated as IPv4
       for backward compatibility */
    switch (addr->type)
    {
    case cmTransportTypeIP:
    case cmTransportTypeNSAP:
    case cmTransportTypeIPv6:
        break;
    default:
        addr->type = cmTransportTypeIP;
    }

    if (((addr->type==cmTransportTypeIP) || (addr->type==cmTransportTypeIPv6)) &&
        !cmTaHasIp(&localAddr))
    {
        /* if that failed, just for H.245 sockets try and get the Q.931 allocated ip */
        if (type == cmTransH245Conn)
        {
            if (q931Host)
                memcpy(&localAddr, &q931Host->localAddress, sizeof(localAddr));
        }

        /* if no address yet, use the given localIPAddress */
        if (!cmTaHasIp(&localAddr))
            memcpy(&localAddr, &transGlobals->localAddress, sizeof(localAddr));

        if ((!cmTaHasIp(&localAddr)) && (type == cmTransH245Conn))
        {
            RvH323CoreToCmAddress(localHostAddr, &localAddr);
        }
    }

    savedPort = addr->port;

    /* update the address components */
    memcpy(addr, &localAddr, sizeof(*addr));
    if (!hTpkt)
        addr->port     = savedPort;
    {
        RvAddress rvAddr;
        RvH323CmToCoreAddress(addr, &rvAddr);
        addr->distribution = (RvAddressIsMulticastIp(&rvAddr))?cmDistributionMulticast:cmDistributionUnicast;
        RvAddressDestruct(&rvAddr);
    }
}

/************************************************************************************
 * sendStartH245Facility
 *
 * Purpose: to create and send a facility message with reason startH245.
 *
 *
 * Input: transGlobals - The global variables of the module.
 *        session      - The session on which the facility is to be sent.
 *
 * Output: None.
 *
 **************************************************************************************/
void sendStartH245Facility(cmTransGlobals *transGlobals, cmTransSession *session)
{
    int newMessage = -1;
    int res;
    TRANSERR err;
    RvUint16 CRV;
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    if (m_sessionget(session, needToSendFacilityStartH245))
    {
        m_sessionset(session, needToSendFacilityStartH245, RV_FALSE);

        /* make sure we're listening on the local address */
        /* start a listening process on it (if we don't have a connection already) */
        if (emaMarkLocked((EMAElement)session->H245Connection))
        {
            if (session->H245Connection->state == hostIdle)
            {
                TRANSERR reply = cmTransOK;

                getGoodAddress(transGlobals,
                               session->H245Connection->h245Listen,
                               session->Q931Connection,
                               cmTransH245Conn,
                               &session->H245Connection->localAddress);
                session->H245Connection->localAddress.port = 0;

                /* start listenning */
                if (transGlobals->hostEventHandlers.cmEvTransHostListen)
                {
                    HATRANSHOST haTransHost =
                                 (HATRANSHOST)emaGetApplicationHandle(((EMAElement)session->H245Connection));

                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog,
                            "cmEvTransHostListen(hsHost=%d(%p), haHost=%p, type=cmTransH245Conn, address=(ip:%s,port:%u))",
                            emaGetIndex((EMAElement)session->H245Connection), session->H245Connection,
                            haTransHost, getIP(&session->H245Connection->localAddress, buff), session->H245Connection->localAddress.port));

                    reply = transGlobals->hostEventHandlers.cmEvTransHostListen(
                                            (HSTRANSHOST)session->H245Connection,
                                            haTransHost,
                                            cmTransH245Conn,
                                            &session->H245Connection->localAddress);
                    session->H245Connection->reported = RV_TRUE;
                }

                if (reply == cmTransOK)
                    session->H245Connection->h245Listen =
                            tpktOpen(transGlobals->tpktCntrl,
                                     &session->H245Connection->localAddress,
                                     RvH323ConnectionH245,
                                     tpktServer,
                                     (EMAElement)session->H245Connection,
                                     transH245AcceptHandler,
                                     (void *)session->H245Connection);
                else
                    session->H245Connection->h245Listen = NULL;

                /* get the allocated local address */
                getGoodAddress(transGlobals,
                               session->H245Connection->h245Listen,
                               session->Q931Connection,
                               cmTransH245Conn,
                               &session->H245Connection->localAddress);

                if (transGlobals->hostEventHandlers.cmEvTransHostListening)
                {
                    HATRANSHOST haTransHost =
                                 (HATRANSHOST)emaGetApplicationHandle(((EMAElement)session->H245Connection));

                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog,
                            "cmEvTransHostListening(hsHost = %d(%p), haHost=%p, type=cmTransH245Conn, address = (ip:%s,port:%u) error = %d)",
                            emaGetIndex((EMAElement)session->H245Connection), session->H245Connection, haTransHost,
                            getIP(&session->H245Connection->localAddress, buff), session->H245Connection->localAddress.port,
                            (session->H245Connection->h245Listen == NULL)));

                    transGlobals->hostEventHandlers.cmEvTransHostListening(
                                            (HSTRANSHOST)session->H245Connection,
                                            haTransHost,
                                            cmTransH245Conn,
                                            &session->H245Connection->localAddress,
                                            (session->H245Connection->h245Listen == NULL));
                    session->H245Connection->reported = RV_TRUE;
                }

                if ( (!session->H245Connection->h245Listen) && (reply != cmTransIgnoreMessage) )
                {
                    emaReleaseLocked((EMAElement)session->H245Connection);
                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog,
                             "sendStartH245Facility failed to initiate listen on H.245 TPKT"));
                    return;
                }
                else
                if (reply == cmTransIgnoreMessage)
                {
                    emaReleaseLocked((EMAElement)session->H245Connection);
                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog,
                             "sendStartH245Facility initiate listen on H.245 TPKT refused by user"));
                    return;
                }
                else
                {
                    RvBool nat = RV_FALSE;

                    session->H245Connection->localAddressInMsg = session->H245Connection->localAddress;

                    if ((cmTransGetSessionParam((HSTRANSSESSION)session, cmTransParam_callSupportNat, (RvBool*)(&nat)) == cmTransOK) &&
                        (nat) && (transGlobals->sessionEventHandlers.cmEvTransNatAddressTranslation != NULL))
                    {
                        HATRANSSESSION haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

                        transGlobals->sessionEventHandlers.cmEvTransNatAddressTranslation(
                            (HSTRANSSESSION) session, haTransSession, cmProtocolH245, &(session->H245Connection->localAddressInMsg));
                    }

                    memset(&session->H245Connection->remoteAddress, 0, sizeof(cmTransportAddress));
                    session->H245Connection->remoteAddress.type = (cmTransportType)-1;

                    /* mark that the h245 host is in listening mode */
                    session->H245Connection->state = hostListenning;
                }
            }
            emaReleaseLocked((EMAElement)session->H245Connection);
        }
        else
            return;

        /* send facility */
        CRV = session->CRV;
        if (!m_sessionget(session, outgoing))
            CRV |= 0x8000;
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog,
                 "sendStartH245Facility CRV=%x outgoing=%d", CRV, m_sessionget(session, outgoing)));
        if (createMessage(transGlobals,
                          cmQ931facility,
                          session->CRV,
                          session->callId,
                          &newMessage)
           )
        {
            int nodeId;
            /* set reason */
            __pvtBuildByFieldIds(nodeId, transGlobals->hPvt, newMessage,
                                    { _q931(message)
                                      _q931(facility)
                                      _q931(userUser)
                                      _q931(h323_UserInformation)
                                      _q931(h323_uu_pdu)
                                      _q931(h323_message_body)
                                      _q931(facility)
                                      _q931(reason)
                                      _q931(startH245)
                                     LAST_TOKEN
                                    },
                                 0, NULL);

            /* set address */
            __pvtBuildByFieldIds(nodeId, transGlobals->hPvt, newMessage,
                                    { _q931(message)
                                      _q931(facility)
                                      _q931(userUser)
                                      _q931(h323_UserInformation)
                                      _q931(h323_uu_pdu)
                                      _q931(h323_message_body)
                                      _q931(facility)
                                      _q931(h245Address)
                                     LAST_TOKEN
                                    },
                                    0, NULL);
            res = -1;
            if (nodeId >= 0)
            {
                res = cmTAToVt(transGlobals->hPvt,
                               nodeId,
                               &session->H245Connection->localAddressInMsg);
            }
            if (res < 0)
            {
                if (emaMarkLocked((EMAElement)session->H245Connection))
                {
                    tpktClose(session->H245Connection->h245Listen);
                    session->H245Connection->h245Listen = NULL;
                    emaReleaseLocked((EMAElement)session->H245Connection);
                }
                pvtDelete(transGlobals->hPvt, newMessage);
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog,
                         "sendStartH245Facility failed to create address in facility (startH245) "));
                return;
            }

            err = cmTransSendMessage((HSTRANSSESSION)session, newMessage, cmTransQ931Type);
            pvtDelete(transGlobals->hPvt, newMessage);
            if (err != cmTransOK)
            {
                if (emaMarkLocked((EMAElement)session->H245Connection))
                {
                    tpktClose(session->H245Connection->h245Listen);
                    session->H245Connection->h245Listen = NULL;
                    emaReleaseLocked((EMAElement)session->H245Connection);
                }
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "sendStartH245Facility failed to send facility (startH245) message"));
                return;
            }
        }
        else
        {
            if (emaMarkLocked((EMAElement)session->H245Connection))
            {
                tpktClose(session->H245Connection->h245Listen);
                session->H245Connection->h245Listen = NULL;
                emaReleaseLocked((EMAElement)session->H245Connection);
            }
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "sendStartH245Facility failed to create facility (startH245) message"));
            return;
        }
    }
}


/**************************************************************************************
 * closeHostInternal
 *
 * Purpose: Deletes a host element. Will notify all its associates sessions.
 *
 * Input:   hsTranHost  - An handle to the host element
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR closeHostInternal(IN HSTRANSHOST hsTransHost, IN RvBool reportToUser)
{
    TRANSERR res = cmTransErr;
    cmTransHost *host = (cmTransHost *)hsTransHost;

    /* check if an element exists */
    if ( (hsTransHost) && emaLock((EMAElement)hsTransHost) )
    {
        /* retrieve the transport module global area */
        cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransHost);

        if ( (transGlobals->hostEventHandlers.cmEvTransHostClosed) && (reportToUser) &&
              (host->reported) && (host->state != hostClosing) && (host->state != hostClosed) )
        {
            HATRANSHOST haTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)hsTransHost);

            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                "cmEvTransHostClosed(hsHost = %d(%p), haHost=%p, reported = %d)",
                emaGetIndex((EMAElement)hsTransHost), hsTransHost, haTransHost, host->reported));

            if( (host->state != hostClosing) && (host->state != hostClosed) )
            {
                host->state = hostClosing;
                transGlobals->hostEventHandlers.cmEvTransHostClosed((HSTRANSHOST)hsTransHost,
                                                                    haTransHost,
                                                                    host->reported);
                host->reported = RV_FALSE;
            }
            else
            {
                if( (!emaWasDeleted((EMAElement)host)) && (host->state != hostClosed) )
                {
                    host->state = hostClosed;
                    transHostClose((HSTRANSHOST)host, RV_TRUE);
                }
            }
            res = cmTransOK;
        }
        else
        {
            /* lock the host for this major change */
            if (host->state != hostClosed)
            {
                host->state = hostClosed;
                transHostClose(hsTransHost, RV_TRUE);
            }
            res = cmTransOK;
        }
        emaUnlock((EMAElement)hsTransHost);
    }

    return res;
}


/* e_FeatureSetType
 * ----------------------------------------------------------------------------
 * Definitions feature det types.
 */
typedef enum {
    FeatureSetTypeGeneric,
    FeatureSetTypeSupported,
    FeatureSetTypeDesired,
    FeatureSetTypeNeeded,
    FeatureSetTypeData
} e_FeatureSetType;


/******************************************************************************
 * addFeatureIdentifier
 * ----------------------------------------------------------------------------
 * General: Add a feature identifier
 *
 * Return Value: The feature identifier node, or negative
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hVal           - VT tree handle
 *         baseNode       - Feature set node
 *         fst            - Type of the feature set to get
 *         standardId     - The feature identifier to add, if standard
 *         nonStandardId  - The non-standard feature identifier to add
 * Output: None.
 *****************************************************************************/
RvPvtNodeId addFeatureIdentifier(IN HPVT hVal,
                                 IN RvPvtNodeId baseNode,
                                 IN e_FeatureSetType fst,
                                 IN RvInt32 standardId,
                                 IN RvChar * nonStandardId)
{
    RvPvtNodeId idNode;

    /* for some feature sets, we must go in a level */
    if (fst == FeatureSetTypeSupported)
        baseNode = pvtGetChildByFieldId(hVal, baseNode, __q931(supportedFeatures), NULL, NULL);
    if (fst == FeatureSetTypeDesired)
        baseNode = pvtGetChildByFieldId(hVal, baseNode, __q931(desiredFeatures), NULL, NULL);
    if (fst == FeatureSetTypeNeeded)
        baseNode = pvtGetChildByFieldId(hVal, baseNode, __q931(neededFeatures), NULL, NULL);
    if (fst == FeatureSetTypeData)
        baseNode = pvtAdd(hVal, baseNode, __q931(parameters), 0, NULL, NULL);

    /* add a new child */
    baseNode = pvtAdd(hVal, baseNode, __nul(0), 0, NULL, NULL);

    /* build the Id */
    idNode = pvtAdd(hVal, baseNode, __q931(id), 0, NULL, NULL);
    if (standardId >= 0)
    {
        pvtAdd(hVal, idNode, __q931(standard), standardId, NULL, NULL);
    }
    else if (nonStandardId != NULL)
    {
        pvtAdd(hVal, idNode, __q931(nonStandard), 16, nonStandardId, NULL);
    }
    return baseNode;
}


/******************************************************************************
 * findFeatureIdentifier
 * ----------------------------------------------------------------------------
 * General: Get the feature identifier value of the given value
 *
 * Return Value: The feature identifier node, or negative
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hVal           - VT tree handle
 *         baseNode       - Feature set node
 *         fst            - Type of the feature set to get
 *         standardId2find  - The feature identifier to find, if standard
 *         nonStandardId2find - The non-standard feature identifier to find
 * Output: None.
 *****************************************************************************/
RvPvtNodeId findFeatureIdentifier(IN HPVT hVal,
                                  IN RvPvtNodeId baseNode,
                                  IN e_FeatureSetType fst,
                                  IN RvInt32 standardId2find,
                                  IN RvChar * nonStandardId2find)
{
    RvPvtNodeId idNode;

    /* for some feature sets, we must go in a level */
    if (fst == FeatureSetTypeSupported)
        baseNode = pvtGetChildByFieldId(hVal, baseNode, __q931(supportedFeatures), NULL, NULL);
    if (fst == FeatureSetTypeDesired)
        baseNode = pvtGetChildByFieldId(hVal, baseNode, __q931(desiredFeatures), NULL, NULL);
    if (fst == FeatureSetTypeNeeded)
        baseNode = pvtGetChildByFieldId(hVal, baseNode, __q931(neededFeatures), NULL, NULL);
    if (fst == FeatureSetTypeData)
        baseNode = pvtGetChildByFieldId(hVal, baseNode, __q931(parameters), NULL, NULL);

    /* get the child */
    baseNode = pvtChild(hVal, baseNode);

    while (RV_PVT_NODEID_IS_VALID(baseNode))
    {
        /* get the Id */
        idNode = pvtGetChild(hVal, baseNode, __q931(id), NULL);
        if (standardId2find >= 0)
        {
            RvInt32 standardId = -1;
            pvtGetChildByFieldId(hVal, idNode, __q931(standard), &standardId, NULL);
            if (standardId2find == standardId)
                return baseNode;
        }
        else if (nonStandardId2find != NULL)
        {
            RvChar nonStandardId[128];
            RvInt32 nonStandardIdLen = (RvInt32)sizeof(nonStandardId);

            idNode = pvtGetChild(hVal, idNode, __q931(nonStandard), NULL);
            nonStandardIdLen = pvtGetString(hVal, idNode, nonStandardIdLen, nonStandardId);

            if (strncmp(nonStandardId, nonStandardId2find, (RvSize_t)nonStandardIdLen) == 0)
            {
                return baseNode;
            }
        }
        baseNode = pvtBrother(hVal, baseNode);
    }
    return baseNode;
}

/******************************************************************************
 * getFeatureIdentifier
 * ----------------------------------------------------------------------------
 * General: Get the feature identifier value
 *
 * Return Value: The feature identifier node, or negative
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hVal           - VT tree handle
 *         baseNode       - Feature identifier node
 *         fst            - Type of the feature set to get
 *         index          - Identifier index
 *         nonStandardIdLen - Length of nonStandardId
 * Output: standardId     - The feature identifier, if standard
 *         nonStandardId  - The non-standard feature identifier
 *****************************************************************************/
RvPvtNodeId getFeatureIdentifier(IN  HPVT hVal,
                                 IN  RvPvtNodeId baseNode,
                                 IN  e_FeatureSetType fst,
                                 IN  RvInt32 index,
                                 OUT RvInt32 * standardId,
                                 IN  RvInt32 nonStandardIdLen,
                                 OUT RvChar * nonStandardId)
{
    RvPvtNodeId idNode;

    /* for some feature sets, we must go in a level */
    if (fst == FeatureSetTypeSupported)
        baseNode = pvtGetChildByFieldId(hVal, baseNode, __q931(supportedFeatures), NULL, NULL);
    if (fst == FeatureSetTypeDesired)
        baseNode = pvtGetChildByFieldId(hVal, baseNode, __q931(desiredFeatures), NULL, NULL);
    if (fst == FeatureSetTypeNeeded)
        baseNode = pvtGetChildByFieldId(hVal, baseNode, __q931(neededFeatures), NULL, NULL);
    if (fst == FeatureSetTypeData)
        baseNode = pvtGetChildByFieldId(hVal, baseNode, __q931(parameters), NULL, NULL);

    /* get the child */
    baseNode = pvtGetByIndex(hVal, baseNode, index, NULL);

    if (RV_PVT_NODEID_IS_VALID(baseNode))
    {
        /* get the Id */
        idNode = pvtGetChild(hVal, baseNode, __q931(id), NULL);
        if (standardId != NULL)
        {
            pvtGetChildByFieldId(hVal, idNode, __q931(standard), standardId, NULL);
        }
        else if (nonStandardId != NULL)
        {
            idNode = pvtGetChild(hVal, idNode, __q931(nonStandard), NULL);
            pvtGetString(hVal, idNode, nonStandardIdLen, nonStandardId);
        }
    }
    return baseNode;
}


/******************************************************************************
 * buildGenericData
 * ----------------------------------------------------------------------------
 * General: Build the generic data node.
 *
 * Return Value: The generic data node, or negative
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hVal           - VT tree handle
 *         messageRoot    - Root node of the message
 * Output: None.
 *****************************************************************************/
RvPvtNodeId buildGenericData(IN HPVT hVal,
                             IN RvPvtNodeId messageRoot)
{
    RvPvtNodeId retNode;

    /* get to the sequence of generic data and build a new child */
    __pvtBuildByFieldIds(retNode, hVal, messageRoot,
        { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
          _q931(h323_uu_pdu) _q931(genericData) LAST_TOKEN }, 0, NULL);

    /* return the location for the feature identifiers */
    return retNode;
}


/******************************************************************************
 * getGenericData
 * ----------------------------------------------------------------------------
 * General: Get the generic data node.
 *
 * Return Value: The generic data node, or negative
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hVal           - VT tree handle
 *         messageRoot    - Root node of the message
 * Output: None.
 *****************************************************************************/
RvPvtNodeId getGenericData(IN HPVT hVal,
                           IN RvPvtNodeId messageRoot)
{
    RvPvtNodeId retNode;

    /* get to the sequence of generic data and build a new child */
    __pvtGetByFieldIds(retNode, hVal, messageRoot,
        { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
          _q931(h323_uu_pdu) _q931(genericData) LAST_TOKEN }, NULL, NULL, NULL);

    /* return the location for the feature identifiers */
    return retNode;
}


/******************************************************************************
 * buildFeatureSet
 * ----------------------------------------------------------------------------
 * General: Build a feature set.
 *
 * Return Value: The feature set node, or negative
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hVal           - VT tree handle
 *         messageRoot    - Root node of the message
 *         bReplacement   - True if feature set is a replacement.
 * Output: None.
 *****************************************************************************/
RvPvtNodeId buildFeatureSet(IN HPVT hVal,
                            IN RvPvtNodeId messageRoot,
                            IN EFCSTATE efcState,
                            IN RvBool bReplacement)
{
    RvPvtNodeId retNode;
    RvPstFieldId fieldId = -1; /* Make sure that if retNode won't be valid we return properly */

    /* Check the type of message we need to handle */
    __pvtGetByFieldIds(retNode, hVal, messageRoot,
        { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
          _q931(h323_uu_pdu) _q931(h323_message_body) _anyField LAST_TOKEN }, &fieldId, NULL, NULL);

    switch (fieldId)
    {
    case __q931(setup):
        /* Nothing to do. We're inside the setup message as it is */
        break;

    case __q931(facility):
        {
            if ( bReplacement == RV_FALSE  )
            {
                /* never answer an initial EFC feature negotiation in a FACILITY message (only if not initial) */
                return RV_PVT_INVALID_NODEID;
            }
            else
            {
                /* Simple case, where we need to build a feature set element */
                retNode = pvtAdd(hVal, retNode, __q931(featureSet), 0, NULL, NULL);
                if (!RV_PVT_NODEID_IS_VALID(pvtGetChild(hVal, retNode, __q931(replacementFeatureSet), NULL)))
                {
                    pvtAdd(hVal, retNode, __q931(replacementFeatureSet), bReplacement, NULL, NULL);
                }
            }
        }
        break;

    case __q931(callProceeding):
        if (efcState == efcNo)
          return RV_OK;
        /* else proceed to add node */
    case __q931(alerting):
    case __q931(connect):
    case __q931(releaseComplete):
        /* Simple cases, where we need to build a feature set element */
        retNode = pvtAdd(hVal, retNode, __q931(featureSet), 0, NULL, NULL);
        if (!RV_PVT_NODEID_IS_VALID(pvtGetChild(hVal, retNode, __q931(replacementFeatureSet), NULL)))
        {
            pvtAdd(hVal, retNode, __q931(replacementFeatureSet), bReplacement, NULL, NULL);
        }
        break;

    default:
        return RV_PVT_INVALID_NODEID;
    }

    /* now build the types of feature */
    pvtAdd(hVal, retNode, __q931(supportedFeatures), 0, NULL, NULL);
    pvtAdd(hVal, retNode, __q931(desiredFeatures), 0, NULL, NULL);
    pvtAdd(hVal, retNode, __q931(neededFeatures), 0, NULL, NULL);

    /* return the new feature set node */
    return retNode;
}


/******************************************************************************
 * getFeatureSetNode
 * ----------------------------------------------------------------------------
 * General: Gets the feature set node.
 *
 * Return Value: The feature set node, or negative
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hVal           - VT tree handle
 *         messageRoot    - Root node of the message
 *         bReplacement   - True if feature set is a replacement.
 * Output: None.
 *****************************************************************************/
RvPvtNodeId getFeatureSetNode(IN HPVT hVal,
                              IN RvPvtNodeId messageRoot,
                              IN RvBool *bReplacement)
{
    RvPvtNodeId retNode;
    int msgType;

    /* first, check if this is a setup message, by any chance */
    msgType = pvtGetChildTagByPath(hVal, messageRoot, "message", 1);
    if (msgType == (int)cmQ931setup)
    {
        /* get to the setup message */
        __pvtGetNodeIdByFieldIds(retNode, hVal, messageRoot,
            { _q931(message) _q931(setup) _q931(userUser) _q931(h323_UserInformation)
              _q931(h323_uu_pdu) _q931(h323_message_body) _q931(setup) LAST_TOKEN });
        *bReplacement = RV_TRUE;
    }
    else if ((msgType == (int)cmQ931alerting) || (msgType == (int)cmQ931callProceeding) ||
        (msgType == (int)cmQ931connect) || (msgType == (int)cmQ931releaseComplete) ||
        (msgType == (int)cmQ931facility))
    {
        /* get to the feature set element */
        __pvtGetNodeIdByFieldIds(retNode, hVal, messageRoot,
            { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
              _q931(h323_uu_pdu) _q931(h323_message_body) _anyField _q931(featureSet) LAST_TOKEN });
        pvtGetChildByFieldId(hVal, retNode, __q931(replacementFeatureSet), (RvInt32*)bReplacement, NULL);
    }
    else
    {
        return RV_PVT_INVALID_NODEID;
    }
    /* return the new feature set node */
    return retNode;
}


/******************************************************************************
 * addFeatureSet
 * ----------------------------------------------------------------------------
 * General: Adds the feature set flags, currently for Extended Fast Connect.
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : transGlobals   - The global variables of the module
 *         session        - The session on which the message is to be sent
 *         newMessage     - Message to add the flags to
 *         bReplacement   - true if feature set is a replacement.
 * Output: None.
 *****************************************************************************/
RvStatus addFeatureSet(
    IN cmTransGlobals  *transGlobals,
    IN cmTransSession  *session,
    IN RvPvtNodeId      messageRoot,
    IN RvBool           bReplacement)
{
    RvPvtNodeId featureSetNode;
    EFCSTATE efcState = session->extFastConnectState;

    /* if either we do not support EFC, or we do, but we don't have FS channels, return */
    if (efcState == efcNo)
    {
        if (bReplacement)
        {
            featureSetNode = buildFeatureSet(transGlobals->hPvt, messageRoot,
                efcState, bReplacement);
        }
        return RV_OK;
    }

    featureSetNode = buildFeatureSet(transGlobals->hPvt, messageRoot, efcState, bReplacement);

    /* check if we have an actual offer */
    if ((session->faststartState == fsNo) || (session->faststartState == fsRejected))
        return RV_OK;

    /* state that we support EFC in the feature list */
    addFeatureIdentifier(transGlobals->hPvt, featureSetNode, FeatureSetTypeSupported, 6, NULL);

    if (session->extFastConnectState == efcDesire)
    {
        addFeatureIdentifier(transGlobals->hPvt, featureSetNode, FeatureSetTypeDesired, 6, NULL);
    }
    if (session->extFastConnectState == efcRequire)
    {
        addFeatureIdentifier(transGlobals->hPvt, featureSetNode, FeatureSetTypeNeeded, 6, NULL);
    }
    return RV_OK;
}


/******************************************************************************
 * getFeatureSet
 * ----------------------------------------------------------------------------
 * General: Gets the feature set flags, currently for Extended Fast Connect.
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : transGlobals   - The global variables of the module
 *         session        - The session on which the message is to be sent
 *         messageRoot    - Message to get the flags from
 *         msgType        - The type of the message
 * Output: None.
 *****************************************************************************/
RvStatus getFeatureSet(
    IN cmTransGlobals  *transGlobals,
    IN cmTransSession  *session,
    IN RvPvtNodeId      messageRoot,
    IN int              msgType)
{
    RvBool bReplacement = RV_FALSE;
    RvPvtNodeId featureSetNode = getFeatureSetNode(transGlobals->hPvt, messageRoot, &bReplacement);
    RvPvtNodeId retNode;
    EFCSTATE remoteEFC = efcNo;

    /* make sure we have a feature set */
    if (RV_PVT_NODEID_IS_VALID(featureSetNode))
    {
        m_sessionset(session, receivedRemoteFeatureSet, RV_TRUE);
        /* check if they support EFC in the feature list */
        retNode = findFeatureIdentifier(transGlobals->hPvt, featureSetNode, FeatureSetTypeSupported, 6, NULL);
        if (RV_PVT_NODEID_IS_VALID(retNode))
        {
            /* check if they want or require EFC */
            retNode = findFeatureIdentifier(transGlobals->hPvt, featureSetNode, FeatureSetTypeDesired, 6, NULL);
            if (RV_PVT_NODEID_IS_VALID(retNode))
            {
                remoteEFC = efcDesire;
            }
            retNode = findFeatureIdentifier(transGlobals->hPvt, featureSetNode, FeatureSetTypeNeeded, 6, NULL);
            if (RV_PVT_NODEID_IS_VALID(retNode))
            {
                remoteEFC = efcRequire;
            }
        }
    }
    else
    {
        /* no feature set: if this is the connect message and no feature set was
           received, this is a rejection of EFC. o.w., carry on. */
        if ((msgType != (int)cmQ931connect) ||
            (m_sessionget(session, receivedRemoteFeatureSet)))
        {
            return RV_OK;
        }
    }
    if (transGlobals->sessionEventHandlers.cmEvTransExtFastConnestState != NULL)
    {
        transGlobals->sessionEventHandlers.cmEvTransExtFastConnestState(
            (HSTRANSSESSION)session, (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session),
            (EFCSTATE) remoteEFC);
    }

    if (remoteEFC == efcNo)
        session->extFastConnectState = efcNo;

    return RV_OK;
}


/******************************************************************************
 * addExtFastConnectGenericData
 * ----------------------------------------------------------------------------
 * General: Add an EFC generic parameter value.
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsTransSession - Transport session handle
 *         messageRoot    - Root node of the message
 *         parameter      - The value of the EFC generic parameter to set
 * Output: None.
 *****************************************************************************/
RvStatus addExtFastConnectGenericData(
    IN HSTRANSSESSION   hsTransSession,
    IN RvPvtNodeId      messageRoot,
    IN RvInt32          parameter)
{
    cmTransGlobals *transGlobals;
    cmTransSession *session = (cmTransSession *)hsTransSession;
    RvPvtNodeId genDataEFC;

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

    /* state that we support EFC in generic data */
    genDataEFC = buildGenericData(transGlobals->hPvt, messageRoot);
    genDataEFC = addFeatureIdentifier(transGlobals->hPvt, genDataEFC, FeatureSetTypeGeneric, 6, NULL);
    /* see if we should flag this as a proposal */
    if (parameter != 0)
        addFeatureIdentifier(transGlobals->hPvt, genDataEFC, FeatureSetTypeData, parameter, NULL);
    return RV_OK;
}


/******************************************************************************
 * getExtFastConnectGenericData
 * ----------------------------------------------------------------------------
 * General: Get the EFC generic parameter value.
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsTransSession - Transport session handle
 *         messageRoot    - Root node of the message
 * Output: parameter      - The value of the EFC generic parameter
 *****************************************************************************/
RvStatus getExtFastConnectGenericData(
    IN  HSTRANSSESSION   hsTransSession,
    IN  RvPvtNodeId      messageRoot,
    OUT RvInt32          *parameter)
{
    cmTransGlobals *transGlobals;
    cmTransSession *session = (cmTransSession *)hsTransSession;
    RvPvtNodeId genDataEFC;

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

    /* state that we support EFC in generic data */
    genDataEFC = getGenericData(transGlobals->hPvt, messageRoot);
    if (!RV_PVT_NODEID_IS_VALID(genDataEFC)) return RV_ERROR_UNKNOWN;
    genDataEFC = findFeatureIdentifier(transGlobals->hPvt, genDataEFC, FeatureSetTypeGeneric, 6, NULL);
    if (!RV_PVT_NODEID_IS_VALID(genDataEFC)) return RV_ERROR_UNKNOWN;
    /* see if we should flag this as a proposal */
    if (parameter != NULL)
    {
        if (getFeatureIdentifier(transGlobals->hPvt, genDataEFC, FeatureSetTypeData, 1, parameter, 0, NULL) < 0)
            return RV_ERROR_UNKNOWN;
    }
    return RV_OK;
}


#ifdef __cplusplus
}
#endif /* __cplusplus*/

