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

#include "rvinternal.h"
#include "transport.h"
#include "transportint.h"
#include "transutil.h"
#include "transStates.h"
#include "q931asn1.h"
#include "h245.h"
#include "transnet.h"
#include "emanag.h"
#include "cmutils.h"
#include "prnutils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus*/


static void reportBadMessage(
    cmTransGlobals         *transGlobals,
    cmTransHost            *host,
    TRANSTYPE               type,
    RvH323ConnectionType    connType,
    RvUint8                *buffer,
    int                     bytesToDecode,
    void                  **hMsgContext);



/******************************************************************************
 * transQ931ReadOrCloseEvent
 * ----------------------------------------------------------------------------
 * General: Handle READ and CLOSE events of Q931 connections.
 *          This function supports both TPKT and Annex E connection types.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  transGlobals - Transport instance used.
 *         host         - Host receiving this event.
 *         tpkt         - TPKT object of this event (NULL if Annex E).
 *         annexEIndex  - Index of local address used by Annex E if applicable.
 *                        -1 for TPKT calls.
 *         length       - Length of the message.
 *         event        - The actual event that we received.
 *         extBuffer    - External message buffer
 * Output: None.
 *****************************************************************************/
static void transQ931ReadOrCloseEvent(
    IN cmTransGlobals   *transGlobals,
    IN cmTransHost      *host,
    IN HTPKT            tpkt,
    IN RvInt            annexEIndex,
    IN RvInt            length,
    IN RvSelectEvents   event,
    IN RvUint8          *extBuffer)
{
    RvPvtNodeId pvtNode = RV_PVT_INVALID_NODEID;
    int         res;
    void        *hMsgContext;
    int         offset = 0;
    RvBool      sessionIsLocked = RV_FALSE;
    cmTransSession *session;
    HATRANSSESSION haTransSession;
	//char msg[256]={0};
	//sprintf(msg,"cmEvTransNewMessage ------------------------%d,close is %d,read is %d,write is %d,\n",event,RvSelectClose,RvSelectRead,RV_SELECT_WRITE);
	//OutputDebugString(msg);
    do {
        session = NULL;
        haTransSession = NULL;

        /* try to read and decode a message */
        res = decodeIncomingMessage(tpkt, &length, &offset, host,
                                    &pvtNode, cmTransQ931Type, &hMsgContext, extBuffer);

        if (res>=0)
        {
            RvBool      ok            = RV_FALSE;
            RvBool      acceptedCall  = RV_FALSE;
            RvPstNodeId releaseReason = __q931(invalidCID);
            int         msgType;

            /* check that we can accept new messages on this host */
            if (host->state != hostIdle)
            {
                acceptedCall = canWeAcceptTheCall(transGlobals, host, pvtNode);
            }

            /* find the session of the call */
            if (acceptedCall)
                ok = findSessionByMessage(transGlobals,
                                          host,
                                          pvtNode,
                                          (host->annexEUsageMode != cmTransNoAnnexE),
                                          &session,
                                          &releaseReason);

            /* we have a decoded Q.931 message, process it */
            if (ok)
            {
                int msgType;

                /* if we found the session of the message, mark it (the host is already locked) */
                if (session)
                {
                    sessionIsLocked = emaMarkLocked((EMAElement)session);
                    if (sessionIsLocked == RV_FALSE)
                    {
                        /* This session was closed somewhere in-between - we
                           should continue as if we have no session */
                        session = NULL;
                    }
                }

                /* check what type of Q.931 message we have here */
                msgType = pvtGetChildTagByPath(transGlobals->hPvt, pvtNode, "message", 1);

                /* determine that the TPKT host has won and got the first response */
                if (session)
                    ok = determineWinningHost(transGlobals, session, (tpkt == NULL));

                /* report the new session */
                if ( (transGlobals->sessionEventHandlers.cmEvTransNewSession) &&
                     (session) &&
                     (ok) &&
                     (msgType == (int)cmQ931setup) )
                {
                    TRANSERR     err;
                    int          cause;
                    RvPstFieldId reasonNameId;

                    /* We have to cancel timer which was set at "accept" time - we
                       got a SETUP message on this call. */
                    RvH323TimerCancel(transGlobals->hTimers, &host->connectTimer);

                    /* Set the local address of this call for Annex E. */
                    if (annexEIndex >= 0)
                        session->annexEListeningIndex = annexEIndex;

                    RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                        "cmEvTransNewSession(hAppTrans=%p, hAppATrans=%p, hsSession=%d-%p)",
                            transGlobals, transGlobals->hAppATrans, emaGetIndex((EMAElement)session), session));

                    err = transGlobals->sessionEventHandlers.cmEvTransNewSession(
                                        (HAPPTRANS) transGlobals,
                                        (HAPPATRANS) transGlobals->hAppATrans,
                                        (HSTRANSSESSION)session,
                                        &haTransSession,
                                        pvtNode,
                                        &cause,
                                        &reasonNameId);

                    if (err != cmTransOK)
                    {
                        sendReleaseCompleteMessage(transGlobals,
                                                   host,
                                                   session->CRV,
                                                   session->callId,
                                                   cause,
                                                   reasonNameId);

                        /* Delete the Session */
                        if (!emaWasDeleted((EMAElement)session))
                            cmTransCloseSession((HSTRANSSESSION)session);

                        RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                            "transQ931ReadOrCloseEvent new session was refused by user"));
                        ok = RV_FALSE;
                    }
                    else
                    {
                        emaSetApplicationHandle((EMAElement)session, (void *)haTransSession);
                        m_sessionset(session, reportedQ931Connect, RV_TRUE);
                    }

                }

                /* process the message being received */
                if(ok)
                {
                    if (session)
                    {
                        /* hold the sending of tunneled messages until all incoming ones are reported */
                        m_sessionset(session, holdTunneling, RV_TRUE);
                    }
                    msgType = processQ931IncomingMessage(host, session, pvtNode);
                }

                if (ok)
                {
                    /* report the message to the user */
                    if ( (session) && (!emaWasDeleted((EMAElement)session)) )
                    {
                        haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

                        /* report the new message on the session */
                        if (transGlobals->sessionEventHandlers.cmEvTransNewMessage)
                        {
                            RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                                "cmEvTransNewMessage(session = %d(%p), haTransSession=%p, type=cmTransQ931Type, pvtNode = %d, hMsgContext=%p)",
                                    emaGetIndex((EMAElement)session), session, haTransSession, pvtNode, hMsgContext));

                            transGlobals->sessionEventHandlers.cmEvTransNewMessage(
                                                        (HSTRANSSESSION) session,
                                                        (HATRANSSESSION) haTransSession,
                                                        RvH323ConnectionQ931,
                                                        pvtNode,
                                                        hMsgContext,
                                                        RV_FALSE,
                                                        emaGetIndex((EMAElement)host));
                        }

                        /* release the hold from sending tunneled messages */
                        m_sessionset(session, holdTunneling, RV_FALSE);

                        /* check if we need to send facility for the newly sent tunneled messages */
                        if (!emaWasDeleted((EMAElement)session))
                            initiateTunnelingFacility(transGlobals, session);
                    }
                    else if (session == NULL)
                    {
                        HATRANSHOST haTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)host);

                        /* report the new message on the host */
                        if (transGlobals->hostEventHandlers.cmEvTransHostNewMessage)
                        {
                            RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                                "cmEvTransHostNewMessage(hsHost = %d(%p), haHost=%p, type=cmTransQ931Conn, pvtNode = %d)",
                                    emaGetIndex((EMAElement)host), host, haTransHost, pvtNode));

                            transGlobals->hostEventHandlers.cmEvTransHostNewMessage(
                                                        (HSTRANSHOST) host,
                                                        haTransHost,
                                                        cmTransQ931Type,
                                                        pvtNode,
                                                        hMsgContext);
                        }
                    }
                }
                if (sessionIsLocked)
                {
                    emaReleaseLocked((EMAElement)session);
                    sessionIsLocked = RV_FALSE;
                }
            }
            else
            /* this message has no session, if it's possible
               terminate the connection, i.e. if it's not a multiplexed host,
               or alternatively it's a multiplexed host that may be closed on
               no sessions, and indeed none is connected to it */
            if (acceptedCall)
            {
                /* check what type of Q.931 message we have here */
                msgType = pvtGetChildTagByPath(transGlobals->hPvt, pvtNode, "message", 1);
                if (msgType != (int)cmQ931statusEnquiry)
                {
                    if (host->firstSession)
                    {
                        /* this connection has at least one call, so just report the bad message */
                        reportBadMessage(transGlobals, host, cmTransQ931Type, RvH323ConnectionQ931, NULL, 0, hMsgContext);
                    }
                    else if ((session != NULL) &&
                        (((host->annexEUsageMode == cmTransNoAnnexE) && (session->annexEConnection != NULL)) ||
                         ((host->annexEUsageMode != cmTransNoAnnexE) && (session->Q931Connection != NULL))))
                    {
                        /* session has both Annex E and TPKT, and this is the second connection.
                           send status on the first one */
                        if (host->annexEUsageMode != cmTransNoAnnexE)
                        {
                            /* send status on TPKT host - lock it first! */
                            if (emaLock((EMAElement)session->Q931Connection))
                            {
                                reportBadMessage(transGlobals, session->Q931Connection,
                                    cmTransQ931Type, RvH323ConnectionQ931, NULL, 0, hMsgContext);
                                emaUnlock((EMAElement)session->Q931Connection);
                            }
                        }
                        else
                        {
                            /* send status on Annex E host - lock it first! */
                            if (emaLock((EMAElement)session->annexEConnection))
                            {
                                reportBadMessage(transGlobals, session->annexEConnection,
                                    cmTransQ931Type, RvH323ConnectionQ931, NULL, 0, hMsgContext);
                                emaUnlock((EMAElement)session->annexEConnection);
                            }
                        }
                    }
                    else
                    {
                        /* no calls on this host. if it's a regular host, send release-complete */
                        if ((!host->isMultiplexed) || (!host->remoteIsMultiplexed) ||
                            (host->closeOnNoSessions) || (host->remoteCloseOnNoSessions))
                        {
                            /* force a close on the host */
                            if ((host->annexEUsageMode != cmTransUseAnnexE) && (session != NULL))
                            {
                                sendReleaseCompleteMessage(transGlobals, host, session->CRV, session->callId, -1,
                                    releaseReason);
                            }
                            event = RvSelectClose;
                            if (session == NULL)
                                /* no one to report to anyway. this case should catch the case where we
                                got a message before any setup, but not harm multiplexed or annex E hosts */
                                res = RV_ERROR_UNKNOWN;
                        }
                    }
                }
            }

            /* get rid of the decoded message */
            pvtDelete(transGlobals->hPvt, pvtNode);
        }
    } while ((event == RvSelectClose) && (res >= 0) && !emaWasDeleted((EMAElement)host));

    if (event != RvSelectClose)
    {
        /* set the read event on tpkt */
        tpktRecvNotify(tpkt, transQ931Handler, host);
    }
    else if ( (!emaWasDeleted((EMAElement)host)) && (host->state != hostClosed) )
    {
        /* the host connection was closed, report it for non annex E hosts */
        if ( (transGlobals->hostEventHandlers.cmEvTransHostClosed) &&
             (host->reported) && (host->state != hostClosing) )
        {
            HATRANSHOST haTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)host);

            host->state = hostClosing;
            RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                "cmEvTransHostClosed(hsHost = %d(%p), haHost=%p, reported = %d)",
                    emaGetIndex((EMAElement)host), host, haTransHost, host->reported));

            transGlobals->hostEventHandlers.cmEvTransHostClosed((HSTRANSHOST)host,
                                                                haTransHost,
                                                                host->reported);
            host->reported = RV_FALSE;
        }
        else
        {
            if (!emaWasDeleted((EMAElement)host))
            {
                host->state = hostClosed;
                transHostClose((HSTRANSHOST)host, RV_TRUE);
            }
        }
    }
}




/**************************************************************************************
 * createHostByType
 *
 * Purpose: To allocate a new host element and fill it with data according to its
 *          type, i.e. Q.931-TPKT, Q.931-Annex E, H.245
 *
 * Input:  transGlobals     - The global variables of the module
 *         hsTranSession    - The session for which the host is created (may be NULL)
 *         type             - The type of the connection (TPKT, H.245, Annex E)
 *         hTpkt            - The connection handle in TPKT for a new incoming connection
 *         annexEUsageMode  - is this an annex E host or other, i.e. TPKT for Q.931 or H.245
 *         annexELocalIndex - the local address index for annex E. 0 o.w.
 *
 * Output: hsTransHost -  A handle for the newly created host element.
 *
 **************************************************************************************/
TRANSERR createHostByType( cmTransGlobals      *transGlobals,
                           HSTRANSSESSION      hsTransSession,
                           HSTRANSHOST         *hsTransHost,
                           TRANSCONNTYPE       type,
                           HTPKT               hTpkt,
                           cmAnnexEUsageMode   annexEUsageMode,
                           int                 annexELocalIndex)
{
    cmTransSession *session = (cmTransSession *)hsTransSession;
    cmTransHost    *host;

    cmTransportAddress local, remote;
    TRANSERR       res;

    /* allocate a new host element */
    *hsTransHost = (HSTRANSHOST)emaAdd(transGlobals->hEmaHosts, NULL);

    if (*hsTransHost == NULL)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "createHostByType failed on allocating host element"));
        return cmTransErr;
    }

    host = (cmTransHost *)*hsTransHost;
    if (hsTransSession != NULL)
    {
        transParentShare((HAPPTRANS)transGlobals, (void*)host, (void*)hsTransSession);
    }
    else if (hTpkt != NULL)
    {
        transParentShare((HAPPTRANS)transGlobals, (void*)host, (void*)hTpkt);
    }
    else
    {
        if (transParentNew((HAPPTRANS)transGlobals, (void*)host) != RV_OK)
        {
            emaDelete(host);
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "createHostByType failed on allocating host lock"));
            return cmTransErr;
        }
    }
    memset(host, 0, sizeof(cmTransHost));
    RvLogInfo(&transGlobals->hLog,
        (&transGlobals->hLog, "createHostByType created host=%d(%p)",emaGetIndex((EMAElement)host),host));

    memset(&local, 0, sizeof(cmTransportAddress));
    local.type  = (cmTransportType)-1;
    memset(&remote, 0, sizeof(cmTransportAddress));
    remote.type = (cmTransportType)-1;

    /* set the params of the new host */
    switch (type)
    {
        case cmTransQ931Conn:
        {
            /* The host doesn't have a session attached to it yet,
               so fill it with default params and data from the new
               incoming connection */
            if (annexEUsageMode != cmTransNoAnnexE)
            {
                annexEStatus eStat;

                /* set parameters for the newly created host */
                host->closeOnNoSessions         = RV_TRUE;
                host->isMultiplexed             = RV_TRUE;
                host->remoteCloseOnNoSessions   = RV_FALSE;
                host->remoteIsMultiplexed       = RV_TRUE; /* Annex E is always multiplexed */
                host->annexEUsageMode           = cmTransUseAnnexE;
                host->state                     = hostConnected;
                host->type                      = type;
                host->hTpkt                     = NULL;

                /* get the local address of the annex E */
                eStat = annexEGetLocalAddress(transGlobals->annexECntrl, annexELocalIndex, &local);
                if (eStat != annexEStatusNormal)
                {
                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog, "createHostByType failed on getting annex E local address"));
                }
            }
            else
            {
                if (session != NULL)
                {
                    host->closeOnNoSessions         = (RvBool)m_sessionget(session, closeOnNoSessions);
                    host->isMultiplexed             = (RvBool)m_sessionget(session, isMultiplexed);
                    host->remoteIsMultiplexed       = !m_sessionget(session, outgoing); /* Assume non-multiplexed for outgoing connections until we know better */
                }
                else
                {
                    /* Since it's a new host with no calls on it yet, we assume that by default,
                       we support multiplexing but the remote side does not.
                       If we will need to reject calls on this host it will be a lot easier just
                       to drop the connection and not get resource leaks. */
                    host->isMultiplexed             = RV_TRUE;
                    host->remoteIsMultiplexed       = RV_FALSE;
                }
                host->remoteCloseOnNoSessions   = host->closeOnNoSessions;
                host->annexEUsageMode           = cmTransNoAnnexE;
                host->state                     = hostIdle;
                host->type                      = type;
                host->hTpkt                     = hTpkt;

                if (hTpkt != NULL)
                {
                    tpktGetAddresses(hTpkt, &local, &remote);
                    host->remoteAddress = remote;
                }
            }

            break;
        }

        case cmTransH245Conn:
        {
            /* must always have a session on whose behalf either the connect or the
               listen was done */
            host->closeOnNoSessions         = RV_TRUE;
            host->isMultiplexed             = RV_FALSE;
            host->remoteCloseOnNoSessions   = RV_TRUE;
            host->remoteIsMultiplexed       = RV_FALSE;
            host->annexEUsageMode           = cmTransNoAnnexE;
            host->firstSession              = session;
            host->state                     = hostIdle;
            host->type                      = type;
            host->hTpkt                     = hTpkt;
            host->remoteH245Version         = 0; /* Unknown version at this point */

            if (hTpkt)
            {
                tpktGetAddresses(hTpkt, &local, &remote);
            }

            break;
        }
    }

    /* update the local address of the host */
    host->localAddress = local;
    res = setRemoteAddress(host, &remote);
    if (res != cmTransOK)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "createHostByType failed on setting host remote address"));
        closeHostInternal(*hsTransHost, RV_TRUE);
        return cmTransErr;
    }

    garbageCollectMultiplexHost(transGlobals);

    /* return the newly created host element */
    if (hsTransHost)
        *hsTransHost = (HSTRANSHOST)host;
    return cmTransOK;
}

/**************************************************************************************
 * determineWinningHost
 *
 * Purpose: check if there was a competition between TPKT and annex E, and if so
 *          close the Annex E.
 *
 * Input:   transGlobals  - The global variables of the module
 *          session       - The session for which the connection was made
 *          annexEWon     - which of the two hosts won..
 *
 **************************************************************************************/
RvBool determineWinningHost(cmTransGlobals *transGlobals, cmTransSession *session, RvBool annexEWon)
{
    cmTransHost *loosingHost = NULL;
    cmTransHost *winningHost = NULL;
    RvBool answer = RV_TRUE;
    HATRANSHOST winHaTransHost = NULL;
    HATRANSHOST looseHaTransHost = NULL;

    if (!transGlobals)
        return answer;

    if (annexEWon)
    {
        winningHost = session->annexEConnection;
        loosingHost = session->Q931Connection;
    }
    else
    {
        winningHost = session->Q931Connection;
        loosingHost = session->annexEConnection;
    }

    /* check if we're not too late */
    if ( (loosingHost) && (loosingHost->hostIsApproved) && (!winningHost->hostIsApproved) )
    {
        cmTransHost *tempHost;

        /* reversal of fortunes */
        tempHost    = loosingHost;
        loosingHost = winningHost;
        winningHost = tempHost;

        annexEWon = !annexEWon;

        answer = RV_FALSE;
    }


    /* print the result */
    if (annexEWon)
    {
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "determineWinningHost: annex E has won (host=%d-%p)",
                emaGetIndex((EMAElement)session->annexEConnection), session->annexEConnection));
    }
    else
    {
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "determineWinningHost: TPKT has won (host=%d-%p)",
                emaGetIndex((EMAElement)session->Q931Connection), session->Q931Connection));
    }

    /* disconnect the loosing host and mark the winning one */
    {
        winningHost->hostIsApproved = RV_TRUE;

        if (emaLock((EMAElement)loosingHost))
        {
            looseHaTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)loosingHost);

            if ( annexEWon )
            {
                RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);

                /* disconnect it from the loosing host */
                loosingHost->firstSession = session->nextSession;
                if (session->nextSession)
                    session->nextSession->prevSession = NULL;

                RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
                updateHostMultiplexStatus(transGlobals, loosingHost);
            }

            if ((!loosingHost->firstSession) &&
                 ((loosingHost->closeOnNoSessions) || (loosingHost->remoteCloseOnNoSessions) ||
                  (!loosingHost->isMultiplexed) || (!loosingHost->remoteIsMultiplexed)))
            {
                /* totally kill the host */
                closeHostInternal((HSTRANSHOST)loosingHost, RV_FALSE);
            }

            emaUnlock((EMAElement)loosingHost);
        }

        RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);

        if ( annexEWon )
        {
            if (!m_sessionget(session, connectedToHost))
            {
                /* connect the session to the winning host */
                session->nextSession = winningHost->firstSession;
                if (session->nextSession)
                    session->nextSession->prevSession = session;
                winningHost->firstSession = session;
                session->annexEUsageMode = cmTransUseAnnexE;
                m_sessionset(session, connectedToHost, RV_TRUE);
            }
        }
        else
            session->annexEUsageMode = cmTransNoAnnexE;

        /* save the application handle of the loosing host */
        winHaTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)winningHost);

        if (!winHaTransHost)
            emaSetApplicationHandle((EMAElement)winningHost, (void *)looseHaTransHost);

        RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
        updateHostMultiplexStatus(transGlobals, winningHost);
    }

    /* clear the loosing host from the session */
    if (session->Q931Connection == loosingHost)
        session->Q931Connection = NULL;
    if (session->annexEConnection == loosingHost)
        session->annexEConnection = NULL;

    return answer;
}

/**************************************************************************************
 * transReportConnect
 *
 * Purpose: To invoke the callbacks for the connected host and all its associated sessions
 *
 * Input: host         - The host which got connected.
 *        session      - The session on which to report the host connection (NULL for all).
 *        isConnected  - RV_TRUE: A connect happened, RV_FALSE: an accept happened.
 *
 **************************************************************************************/
void transReportConnect(cmTransHost *host, cmTransSession *session, RvBool isConnected)
{
    cmTransSession *nextSession;
    HATRANSSESSION haTransSession;

    /* retrieve the transport module global area */
    cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);

    /* report the connection of the host even if its an Annex E connection  */
    if (transGlobals->hostEventHandlers.cmEvTransHostConnected != NULL)
    {
        HATRANSHOST haTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)host);

        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "cmEvTransHostConnected(hsHost = %d(%p), haHost=%p, type=%d)",
                emaGetIndex((EMAElement)host), host, haTransHost,host->type));

        transGlobals->hostEventHandlers.cmEvTransHostConnected(
            (HSTRANSHOST)host, haTransHost, host->type, isConnected);
    }

    /* if reporting was asked for just one session (in case of dummy connect for
       multiplexed hosts just report on that session */

    if ( (session) && (!emaWasDeleted((EMAElement)host)) )
    {
        if (transGlobals->sessionEventHandlers.cmEvTransSessionNewConnection)
        {
            /* report just for the given session (if one is given) */
            if ( !m_sessionget(session, reportedQ931Connect) )
            {
                haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog, "cmEvTransSessionNewConnection(hsSession = %d(%p), haSession=%p, type=%d)",
                        emaGetIndex((EMAElement)session), session, haTransSession, host->type));

                transGlobals->sessionEventHandlers.cmEvTransSessionNewConnection(
                                (HSTRANSSESSION)session, haTransSession, host->type);

                m_sessionset(session, reportedQ931Connect, RV_TRUE);
            }
        }
    }
    else
    /* go over the sessions that are connected to that host and report the connect */
    if (!emaWasDeleted((EMAElement)host))
    {
        switch (host->type)
        {
        case cmTransQ931Conn:
            /* for Q.931 hosts, go over all the sessions connected to them and report them */
            for (nextSession = host->firstSession; nextSession != NULL; nextSession = nextSession->nextSession)
            {
                if (emaMarkLocked((EMAElement)nextSession))
                {
                    /* report the connection */
                    haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)nextSession);

                    /* for the Q.931 type connections (TPKT or annex E) report them always */
                    if (!emaWasDeleted((EMAElement)nextSession))
                        if (transGlobals->sessionEventHandlers.cmEvTransSessionNewConnection)
                        {
                            /* report just for the session */
                            if ( !m_sessionget(nextSession, reportedQ931Connect) )
                            {
                                RvLogInfo(&transGlobals->hLog,
                                    (&transGlobals->hLog, "cmEvTransSessionNewConnection(hsSession = %d(%p), haSession=%p, type=%d)",
                                        emaGetIndex((EMAElement)nextSession), nextSession, haTransSession, cmTransQ931Conn));

                                transGlobals->sessionEventHandlers.cmEvTransSessionNewConnection(
                                        (HSTRANSSESSION)nextSession, haTransSession, cmTransQ931Conn);

                                if (nextSession)
                                {
                                    m_sessionset(nextSession, reportedQ931Connect, RV_TRUE);
                                }
                            }
                        }

                    /* unlock and release the current session */
                    emaReleaseLocked((EMAElement)nextSession);
                }
            }
            break;

        case cmTransH245Conn:
            /* for H.245, check if there is a session attached to it, and if so
               report the connection on that session */
            nextSession = host->firstSession;

            if (nextSession)
            {
                haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)nextSession);

                /* Determine whether to report on H.245 connection */
                if (!m_sessionget(nextSession, reportedH245Connect))
                {
                    /* mark that we already reported on an H.245 connection for this session */
                    m_sessionset(nextSession, reportedH245Connect, RV_TRUE);

                    if (transGlobals->sessionEventHandlers.cmEvTransSessionNewConnection)
                    {
                        RvLogInfo(&transGlobals->hLog,
                            (&transGlobals->hLog, "cmEvTransSessionNewConnection(hsSession = %d(%p), haSession=%p, type=cmTransH245Conn)",
                                emaGetIndex((EMAElement)nextSession), nextSession, haTransSession));

                        transGlobals->sessionEventHandlers.cmEvTransSessionNewConnection(
                                            (HSTRANSSESSION)nextSession, haTransSession, cmTransH245Conn);
                    }
                }
                else if (nextSession->tunnelingState == tunnRejected)
                {
                    transGlobals->sessionEventHandlers.cmEvTransTunnelingWasRejected(
                        (HSTRANSSESSION)nextSession, haTransSession, RV_TRUE);
                }
            }
            break;
        }
    }

    /* mark the host as connected so it will be reported when closed */
    if (!emaWasDeleted((EMAElement)host))
        host->reported = RV_TRUE;
}


/******************************************************************************
 * findH245Version
 * ----------------------------------------------------------------------------
 * General: Find the H.245 version of the remote endpoint by looking inside
 *          his incoming TCS message.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  host     - Host receiving this H.245 message.
 *         hVal     - PVT handle to use.
 *         nodeId   - Incoming message to check.
 * Output: None.
 *****************************************************************************/
static void findH245Version(
    IN cmTransHost  *host,
    IN HPVT         hVal,
    IN RvPvtNodeId  nodeId)
{
    RvPvtNodeId tcsMessage;

    __pvtGetNodeIdByFieldIds(tcsMessage, hVal, nodeId, {_h245(request) _h245(terminalCapabilitySet) LAST_TOKEN});

    if (RV_PVT_NODEID_IS_VALID(tcsMessage))
    {
        RvInt32 pIDLen;
        RvChar pID[128];

        /* Get the parameters from the incoming cap */
        nodeId = pvtGetChildByFieldId(hVal, tcsMessage, __h245(protocolIdentifier), &pIDLen, NULL);
        if (RV_PVT_NODEID_IS_VALID(nodeId) && (pIDLen > 0))
        {
            pvtGetString(hVal, nodeId, pIDLen, pID);
            host->remoteH245Version = (RvUint8)(pID[5]);
        }
    }
}


/**************************************************************************************
 * compareH245Connections
 *
 * Purpose: To decide which one of two H.245 should we use. if the remote address is
 *          bigger or if both address are the same but the remote port is bigger than
 *          use the incoming connection, o.w. use the outgoing one.
 *
 * Input: host         - The h.245 host which has a listen and connecting processes.
 * Return: 1 if we should use the outgoing connection, -1 if we should use the
 *         incoming connection, 0 if there was an error.
 **************************************************************************************/
int compareH245Connections(cmTransHost *host)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff1[RV_TRANSPORT_ADDRESS_STRINGSIZE], buff2[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif
    int cmpRes;

    if (!transGlobals)
        return 0;

    RvLogInfo(&transGlobals->hLog,
        (&transGlobals->hLog, "compareH245Connections on host=%d(%p) local:(ip=%s;port=%u) remote:(ip=%s;port=%u)",
            emaGetIndex((EMAElement)host), host, getIP(&host->localAddress, buff1), host->localAddress.port,
            getIP(&host->remoteAddress, buff2), host->remoteAddress.port));

    /* compare the listening address (local) to the connecting address (remote) */
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    if (host->remoteAddress.type != host->localAddress.type)
    {
        /* ToDo: wait until core has convert function, than convert the v4 to the v6 */
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "compareH245Connections got two different types of addresses"));
        return 0;
    }
    if (host->remoteAddress.type == cmTransportTypeIP)
    {
        cmpRes = memcmp((void *)&host->remoteAddress.addr.v4.ip,
            (void *)&host->localAddress.addr.v4.ip,
            sizeof(host->localAddress.addr.v4.ip));
    }
    else if (host->remoteAddress.type == cmTransportTypeIPv6)
    {
        cmpRes = memcmp((void *)&host->remoteAddress.addr.v6,
            (void *)&host->localAddress.addr.v6,
            sizeof(host->localAddress.addr.v6));
    }
    else
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "compareH245Connections did not get valid addresses"));
        return 0;
    }

#else
    cmpRes = memcmp((void *)&host->remoteAddress.ip,
                    (void *)&host->localAddress.ip,
                    sizeof(host->localAddress.ip));
#endif

    /* if the remote address is bigger or if both address are the same but the remote
       port is bigger, use the accepting connection, else use the connecting one */
    if (cmpRes == 0)
    {
        if (host->remoteAddress.port < host->localAddress.port)
            cmpRes = -1;
        else
            cmpRes = 1;
    }
    return cmpRes;
}


/**************************************************************************************
 * solveH245Conflict
 *
 * Purpose: To decide which one of two H.245 connections will survive.
 *
 * Input: host         - The h.245 host which has a listen and connecting processes.
 *
 **************************************************************************************/
void solveH245Conflict(cmTransHost *host)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);

    if (!transGlobals)
        return;

    /* if we have two H.245 connections, determine which one should be closed */
    if (host->state == hostDoubleConnected)
    {
        int cmpRes;

        /* make sure the listening connection is really used */
        if (!tpktIsConnected(host->h245Listen))
        {
            tpktClose(host->h245Listen);
            host->h245Listen = NULL;
            host->state = hostConnected;
            return;
        }

        /* if the remote address is bigger or if both address are the same but the remote
           port is bigger, then close the connecting host and keep the accepting one,
           else do the opposite */
        cmpRes = compareH245Connections(host);

        if (cmpRes == 0)
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "solveH245Conflict address comparison error"));
            return;
        }

        if (cmpRes < 0)
        {
            /* close the connecting host */
            if (host->hTpkt)
            {
                tpktClose(host->hTpkt);
                host->hTpkt = host->h245Listen;
                if (host->incomingMessage != NULL)
                    rpoolFree(transGlobals->messagesRPool, host->incomingMessage);
                host->incomingMessage = host->incomingListeningMessage;
                host->h245Listen = NULL;
                host->incomingListeningMessage = NULL;
            }
            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog, "solveH245Conflict closed outgoing colliding connection"));
        }
        else
        {
            /* close the accepting socket and don't touch the connecting one */
            if (host->h245Listen)
            {
                tpktClose(host->h245Listen);
                host->h245Listen = NULL;
                if (host->incomingListeningMessage != NULL)
                {
                    rpoolFree(transGlobals->messagesRPool, host->incomingListeningMessage);
                    host->incomingListeningMessage = NULL;
                }
            }
            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog, "solveH245Conflict closed incoming colliding connection"));
        }
        /* the host now is just connected */
        host->state = hostConnected;
    }
}


/**************************************************************************************
 * acceptHostTimeout
 *
 * Purpose: The callback routine for the socket that was accepted, making sure that
 *          no one tries to open a socket without sending any data on it.
 *
 * Input: the host to close
 *
 **************************************************************************************/
static RvBool acceptHostTimeout(IN void*  userData)
{
    cmTransHost *host = (cmTransHost *)userData;
    cmTransGlobals *transGlobals;

    if (emaLock((EMAElement)host))
    {
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);

        RvH323TimerClear(transGlobals->hTimers, &host->connectTimer);

        closeHostInternal((HSTRANSHOST)host, host->reported);

        emaUnlock((EMAElement)host);
    }

    return RV_FALSE;
}


/**************************************************************************************
 * doubleConnectionTimeout
 *
 * Purpose: This timeout functions is called to solve the double connection mode conflict
 *          will either leave the connections open (for older versions) or close the
 *          appropriate one.
 *
 * Input: the host to close
 *
 **************************************************************************************/
static RvBool doubleConnectionTimeout(IN void*  userData)
{
    cmTransHost *host = (cmTransHost *)userData;
    cmTransGlobals *transGlobals;

    if (emaLock((EMAElement)host))
    {
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);

        RvH323TimerClear(transGlobals->hTimers, &host->connectTimer);

        /* We only solve the H.245 conflict of 2 different connections if we are working
           against Stack version 3 or below, since they close the call if we drop any of
           these connections */
        if (host->remoteH245Version > 5)
        {
            solveH245Conflict(host);

            if (host->state == hostConnected)
            {
                /* set the new local address of the host */
                tpktGetAddresses(host->hTpkt, &host->localAddress, NULL);
            }
        }

        emaUnlock((EMAElement)host);
    }
    return RV_FALSE;
}


/**************************************************************************************
 * transQ931AcceptHandler
 *
 * Purpose: The callback routine for the socket that listens for incoming Q.931 requests
 *
 * Input: standard input of TPKT callback
 *
 **************************************************************************************/
void transQ931AcceptHandler(HTPKT tpkt,RvSelectEvents event,int length,int error,void*context)
{
    cmTransHost *host = (cmTransHost *)context;
    cmTransGlobals *transGlobals;

    RV_UNUSED_ARG(length);

    if (!emaMarkLocked((EMAElement)host))
        return;

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);

    /* we don't need this host any more - it's just the default Q.931 listening host */
    emaReleaseLocked((EMAElement)host);
    host = NULL;

    if (transGlobals == NULL)
        return;

    if (error)
    {
        RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
            "transQ931AcceptHandler got error on event %d", event));
        return;
    }

    /* we don't treat the pre-accept event for Q.931 */
    if (event == 0)
        return;

    /* treat the accept event of a Q.931 TPKT connection */
    if (event == RvSelectAccept)
    {
        TRANSERR    res;

        /* create a new connected host element for the new connection */
        res = createHostByType(transGlobals,
                               NULL,
                               (HSTRANSHOST *)&host,
                               cmTransQ931Conn,
                               tpkt,
                               cmTransNoAnnexE, 0);
        if (res != cmTransOK)
        {
            tpktClose(tpkt);
            RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                "transQ931AcceptHandler rejected connection (host wasnt created) res=%d", res));
            return;
        }

        /* register a new callback - we do it first so we won't crash on incoming RvSelectRead events */
        tpktRecvNotify(tpkt, transQ931Handler, host);

        /* Start timer which will initiate a process to release resources in case when:
           1. there were no activity after "accept"
           2. "SETUP" message was rejected by user in the "hook" function */
        RvH323TimerCancel(transGlobals->hTimers, &host->connectTimer);
        host->connectTimer = RvH323TimerStart(transGlobals->hTimers,
                                              acceptHostTimeout, host,
                                              5000); /* Wait for 5 seconds - a lifetime */
        if (host->connectTimer == NULL)
        {
            closeHostInternal((HSTRANSHOST)host, RV_FALSE);
            RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                "transQ931AcceptHandler rejected connection (no free timers)"));
            return;
        }

        /* mark the host as incoming one */
        host->incoming = RV_TRUE;

        /* mark the host before reporting it */
        emaMarkLocked((EMAElement)host);

        host->state = hostConnected;

        transReportConnect(host, NULL, RV_FALSE);

        emaReleaseLocked((EMAElement)host);
    }
    else
    {
        RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
            "transQ931AcceptHandler got event %d (this cannot happen !!!)", event));
    }
}

/**************************************************************************************
 * transH245Handler
 *
 * Purpose: The callback routine for a H245 connection
 *
 * Input: standard input of TPKT callback
 *
 **************************************************************************************/
void transH245Handler(HTPKT tpkt,RvSelectEvents event,int length,int error,void*context)
{
    cmTransHost    *host    = (cmTransHost *)context;
    cmTransSession *session;
    cmTransGlobals *transGlobals;

    if (!emaMarkLocked((EMAElement)host))
        return;

    session = host->firstSession;
    if (!emaMarkLocked((EMAElement)session))
    {
        /* we may need to overlook this, in case we are using H.460.18, so we try to be strict */
        if ((session != NULL) || ((event & RvSelectRead) == 0))
        {
            emaReleaseLocked((EMAElement)host);
            return;
        }
    }

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);

    if (error)
    {
        RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
            "transH245Handler got error on event %d, host state = %d", event, host->state));

        /* in case of a connection error report it so the user may discard the session */
        if (event == RvSelectConnect)
        {
            if ((host->state == hostDoubleConnected) || (host->state == hostListenningConnecting))
            {
                if (tpkt == host->hTpkt)
                {
                    tpktClose(host->hTpkt);
                    host->hTpkt = host->h245Listen;
                    if (host->incomingMessage != NULL)
                        rpoolFree(transGlobals->messagesRPool, host->incomingMessage);
                    host->incomingMessage = host->incomingListeningMessage;
                    host->h245Listen = NULL;
                    host->incomingListeningMessage = NULL;
                }
                if (tpkt == host->h245Listen)
                {
                    tpktClose(host->h245Listen);
                    host->h245Listen = NULL;
                    if (host->incomingListeningMessage != NULL)
                    {
                        rpoolFree(transGlobals->messagesRPool, host->incomingListeningMessage);
                        host->incomingListeningMessage = NULL;
                    }
                }
                if (host->state == hostDoubleConnected)
                    host->state = hostConnected;
                else
                    host->state = hostListenning;
            }
            else
            {
                if (transGlobals->sessionEventHandlers.cmEvTransConnectionOnSessionClosed)
                {
                    HATRANSSESSION haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

                    RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                        "cmEvTransConnectionOnSessionClosed(hsSession = %d(%p), haSessiont=%p, type=cmTransH245Conn)",
                            emaGetIndex((EMAElement)session),session,haTransSession));

                    transGlobals->sessionEventHandlers.cmEvTransConnectionOnSessionClosed(
                                                    (HSTRANSSESSION) session,
                                                    haTransSession,
                                                    cmTransH245Conn);
                }

                /* report the host close itself */
                if ( (transGlobals->hostEventHandlers.cmEvTransHostClosed) && (host->reported) &&
                     (host->state != hostClosing) && (host->state != hostClosed) )
                {
                    HATRANSHOST haTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)host);

                    host->state = hostClosing;
                    RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                        "cmEvTransHostClosed(hsHost = %d(%p), haHost=%p, reported = %d)",
                            emaGetIndex((EMAElement)host), host, haTransHost, host->reported));

                    transGlobals->hostEventHandlers.cmEvTransHostClosed((HSTRANSHOST)host,
                                                                        haTransHost,
                                                                        host->reported);
                    host->reported = RV_FALSE;
                }
            } /*else (host->state == hostDoubleConnected)*/
        } /*(event == RvSelectConnect)*/
    } /*(error)*/
    else
    {
        switch (event)
        {
            case RvSelectConnect:
            {
                TRANSERR res;

                if (host->hTpkt != tpkt)
                    break;

                if (host->state == hostConnecting)
                {
                    /* all ok we can use that connection */
                    host->state = hostConnected;

                    /* set the new local address of the host */
                    tpktGetAddresses(host->hTpkt, &host->localAddress, NULL);
                }
                else if (host->state == hostListenningConnecting)
                {
                    host->state = hostDoubleConnected;

                    RvH323TimerCancel(transGlobals->hTimers, &host->connectTimer);
                    host->connectTimer = RvH323TimerStart(transGlobals->hTimers,
                        doubleConnectionTimeout, host, 500);
                }

                if (m_sessionget(session, isTunnelingSupported))
                    m_sessionset(session, switchToSeparateWasAsked, RV_TRUE);

                /* terminate EFC */
                if (transGlobals->sessionEventHandlers.cmEvTransExtFastConnestState != NULL)
                {
                    transGlobals->sessionEventHandlers.cmEvTransExtFastConnestState(
                        (HSTRANSSESSION)session, (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session),
                        (EFCSTATE) efcNo);
                }

                /* set the receive event */
                tpktRecvNotify(tpkt, transH245Handler, context);

                if ((host->dummyHost) && (host->remoteAddress.port != 0) )
                {
                    /* we connected a dummy host, it is no longer dummy */
                    host->dummyHost = RV_FALSE;
                    /* We are reporting this connection as a NEW H.245 connection, but it will NOT reset
                      control, because host->firstSession->reportedH245Connect is true. We should (for
                      later versions) create a special callback for dummy host connecting */
                }

                /* try and send all the tunneled messages that weren't sent or acked yet */
                moveMessagesFromSessionToHost(transGlobals, session, host);

                res = sendMessageOnHost(transGlobals, host);
                if ( (res != cmTransOK) && (res != cmTransConnectionBusy) )
                {
                    RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                        "transH245Handler Failed on sending ex-tunneled msgs for host %d(%p)",
                            emaGetIndex((EMAElement)host), host));
                }

                /* the H.245 connection is established, notify the user */
                transReportConnect(host, NULL, RV_TRUE);
                break;
            }

            case RvSelectAccept:
            /* can't happen, treated by another callback */
            {
                break;
            }

            case RvSelectClose:
            case RvSelectRead:
            {
                RvPvtNodeId pvtNode = RV_PVT_INVALID_NODEID;
                int         res;
                void        *hMsgContext;
                int         offset = 0;
                RvBool      readMore = (length == 0);

                do
                {
					//gaoshizhong add 针对收到TPKT的打洞包（空包）进行处理 ??
                    //if (readMore && (length ==0) )
					if (readMore)
                    {
                        /* Make sure we try to read from the socket itself */
                        length = 0;
                    }

                    /* try to read and decode a message */
                    res = decodeIncomingMessage(tpkt, &length, &offset, context,
                                                &pvtNode, cmTransH245Type, &hMsgContext, NULL);

                    if (session == NULL)
                    {
                        pvtDelete(transGlobals->hPvt, pvtNode);
                        pvtNode = RV_PVT_INVALID_NODEID;
                        break;
                    }

                    if (res >= 0)
                    {
                        HATRANSSESSION haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

                        /* See if we can find the H.245 version from this decoded message or not */
                        findH245Version(host, transGlobals->hPvt, pvtNode);

                        /* report the message to the user */
                        if ( (transGlobals->sessionEventHandlers.cmEvTransNewMessage)  &&
                             (!emaWasDeleted((EMAElement)session)) )
                        {
                            RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                                "cmEvTransNewMessage(hsSession = %d(%p), haSession=%p, type=cmTransH245Type, pvtNode=%d)",
                                    emaGetIndex((EMAElement)session), session, haTransSession, pvtNode));

                            transGlobals->sessionEventHandlers.cmEvTransNewMessage(
                                                        (HSTRANSSESSION) session,
                                                        (HATRANSSESSION) haTransSession,
                                                        RvH323ConnectionH245,
                                                        pvtNode,
                                                        hMsgContext,
                                                        RV_FALSE,
                                                        emaGetIndex((EMAElement)host));
                        }

                        /* get rid of the decoded message */
                        pvtDelete(transGlobals->hPvt, pvtNode);

                        /* try to read another message that might be waiting on the connection */
                        pvtNode = RV_PVT_INVALID_NODEID;

						//gaoshizhong add 针对收到TPKT的打洞包（空包）进行处理 ??
						/* We will stay in this loop and try to read some more */
						//if(offset >0 && (length > 0)) OutputDebugString("length > 0 offset >0 \n");
						//else if(length > 0) OutputDebugString("length > 0 \n");
                        if (readMore && (length == 0))
                        {
                            /* We will stay in this loop and try to read some more */
                            offset = 0;
                            //length = 1; //gaoshizhong add 
						}
                    }
                } while ( (length > 0)  && (!emaWasDeleted((EMAElement)session)) );

                if (event != RvSelectClose)
                {
                    /* set the read event on tpkt */
                    tpktRecvNotify(tpkt, transH245Handler, host);
                }
                else if ( (!emaWasDeleted((EMAElement)host)) &&
                     (host->state != hostClosing) && (host->state != hostClosed) )
                {
                    if (host->state == hostDoubleConnected)
                    {
                        if (tpkt == host->hTpkt)
                        {
                            tpktClose(host->hTpkt);
                            host->hTpkt = host->h245Listen;
                            if (host->incomingMessage != NULL)
                                rpoolFree(transGlobals->messagesRPool, host->incomingMessage);
                            host->incomingMessage = host->incomingListeningMessage;
                            host->h245Listen = NULL;
                            host->incomingListeningMessage = NULL;
                        }
                        if (tpkt == host->h245Listen)
                        {
                            tpktClose(host->h245Listen);
                            host->h245Listen = NULL;
                            if (host->incomingListeningMessage != NULL)
                            {
                                rpoolFree(transGlobals->messagesRPool, host->incomingListeningMessage);
                                host->incomingListeningMessage = NULL;
                            }
                        }
                        host->state = hostConnected;
                        break;
                    }
                    else
                    {
                        if ( (transGlobals->hostEventHandlers.cmEvTransHostClosed) && (host->reported) )
                        {
                            HATRANSHOST haTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)host);

                            host->state = hostClosing;
                            RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                                "cmEvTransHostClosed(hsHost = %d(%p), haHost=%p, reported = %d)",
                                    emaGetIndex((EMAElement)host), host, haTransHost, host->reported));

                            transGlobals->hostEventHandlers.cmEvTransHostClosed((HSTRANSHOST)host,
                                                                                haTransHost,
                                                                                host->reported);
                            host->reported = RV_FALSE;
                        }
                        else
                        {
                            if (!emaWasDeleted((EMAElement)host))
                            {
                                host->state = hostClosed;
                                transHostClose((HSTRANSHOST)host, RV_TRUE);
                            }
                        }
                    }
                }

                break;
            }
            case RvSelectWrite:
            {
                TRANSERR res;

                /* the last send was completed */
                /* remove the message from the pool */
                extractMessageFromPool(transGlobals, host, RV_FALSE);

                /* mark the host as not busy and try sending the remaining messages */
                if ((host->h245Listen) && (host->hTpkt))
                    host->state = hostDoubleConnected;
                else
                    host->state = hostConnected;

                res = sendMessageOnHost(transGlobals, host);
                if ( (res != cmTransOK) && (res != cmTransConnectionBusy) )
                {
                    RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                        "transH245Handler Failed on send for host %d(%p)",
                            emaGetIndex((EMAElement)host), host));
                }
                break;
            }
            default:
                break;
        }
    }

    /* unlock the host and its session */
    emaReleaseLocked((EMAElement)host);
    emaReleaseLocked((EMAElement)session);
}

/**************************************************************************************
 * transH245AcceptHandler
 *
 * Purpose: The callback routine for the socket that listens for incoming H.245 request
 *
 * Input: standard input of TPKT callback
 *
 **************************************************************************************/
void transH245AcceptHandler(HTPKT tpkt,RvSelectEvents event,int length,int error,void*context)
{
    cmTransHost    *host    = (cmTransHost *)context;
    cmTransSession *session;
    cmTransGlobals *transGlobals;
    TRANSERR       res;

    RV_UNUSED_ARG(length);

    if (!emaMarkLocked((EMAElement)host))
        return;

    session = host->firstSession;
    if (!emaMarkLocked((EMAElement)session))
    {
        emaReleaseLocked((EMAElement)host);
        return;
    }

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);

    if (error)
    {
        RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
            "transH245AcceptHandler got error on event %d", event));
    }
    else
    if (session->H245Connection != host)
    {
        RvLogExcep(&transGlobals->hLog, (&transGlobals->hLog,
            "transH245AcceptHandler mismatched session and host %p %p",session->H245Connection, host));
    }
    else
    /* treat the accept event of a H.245 TPKT connection */
    if ( (event == RvSelectAccept) && (context) )
    {
        if (host->state == hostListenning)
        {
            host->state = hostConnected;

            /* set the H245 connection to point to the accepting socket */
            host->hTpkt      = tpkt;
            host->h245Listen = NULL;

            /* set the new local address of the host */
            tpktGetAddresses(tpkt, &host->localAddress, NULL);
        }
        else if (session->H245Connection->state == hostListenningConnecting)
        {
            host->state = hostDoubleConnected;

            RvH323TimerCancel(transGlobals->hTimers, &host->connectTimer);
            host->connectTimer = RvH323TimerStart(transGlobals->hTimers,
                doubleConnectionTimeout, host, 500);
        }

        if (m_sessionget(session, isTunnelingSupported))
            m_sessionset(session, switchToSeparateWasAsked, RV_TRUE);

        /* terminate EFC */
        if (transGlobals->sessionEventHandlers.cmEvTransExtFastConnestState != NULL)
        {
            transGlobals->sessionEventHandlers.cmEvTransExtFastConnestState(
                (HSTRANSSESSION)session, (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session),
                (EFCSTATE) efcNo);
        }

        /* make sure the stack didn't kill the call here */
        if ((session->H245Connection == NULL) || (emaWasDeleted((EMAElement)session->H245Connection)))
        {
            RvLogDebug(&transGlobals->hLog, (&transGlobals->hLog,
                "transH245AcceptHandler: host was closed"));
            /* unlock the host and its session */
            emaReleaseLocked((EMAElement)session);
            emaReleaseLocked((EMAElement)host);
            return;
        }

        /* try and send all the tunneled messages that weren't sent or acked yet */
        moveMessagesFromSessionToHost(transGlobals, session, session->H245Connection);

        /* set the receive event and change the handler routine */
        tpktRecvNotify(tpkt, transH245Handler, context);

        res = sendMessageOnHost(transGlobals, session->H245Connection);
        if ( (res != cmTransOK) && (res != cmTransConnectionBusy) )
        {
            RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                "transH245AcceptHandler Failed on send of ex-tunneled msgs for host %d(%p)",
                    emaGetIndex((EMAElement)session->H245Connection), session->H245Connection));
        }

        /* the H.245 connection is established, notify the user */
        transReportConnect(session->H245Connection, NULL, RV_FALSE);
    }
    else
    {
        RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
            "transH245AcceptHandler got event %d (this cannot happen !!!)", event));
    }

    /* unlock and release the host and its session */
    emaReleaseLocked((EMAElement)session);
    emaReleaseLocked((EMAElement)host);
}


/**************************************************************************************
 * transQ931Handler
 *
 * Purpose: The callback routine for a Q.931 connection
 *
 * Input: standard input of TPKT callback
 *
 *
 **************************************************************************************/
void transQ931Handler(HTPKT tpkt,RvSelectEvents event,int length,int error,void*context)
{
    cmTransHost *host = (cmTransHost *)context;
    cmTransGlobals *transGlobals;

    if (!emaLock((EMAElement)host))
        return;

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);

    if (error)
    {
        RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
            "transQ931Handler got error on event=%d error=%d", event, error));

        /* in case of a connection error report it so the user may discard the session(s),
           however if the host is in race with an annex E host, no report is needed */
        if (event == RvSelectConnect)
        {
            if ((transGlobals->sessionEventHandlers.cmEvTransConnectionOnSessionClosed != NULL)  ||
                (transGlobals->sessionEventHandlers.cmEvTransAltEpConnection != NULL))
            {
                cmTransSession *session = host->firstSession;

                while (emaMarkLocked((EMAElement)session))
                {
                    cmTransSession *previousSession;
                    HATRANSSESSION  haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

                    if ( (session->Q931Connection == host) && (session->annexEConnection != NULL) )
                    {
                        RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                            "transQ931Handler will no report this error on host (%p-%d) since annex E (%p-%d) may yet succeed",
                                host, emaGetIndex((EMAElement)host),
                                session->annexEConnection, emaGetIndex((EMAElement)session->annexEConnection)));
                    }
                    else if (session->Q931Connection == host) /* no annexEConnection */
                    {
                        TRANSERR res;

                        RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                            "cmEvTransAltEpConnection(hsSession = %d(%p), haSession=%p)",
                                emaGetIndex((EMAElement)session), session, haTransSession));

                        res=transGlobals->sessionEventHandlers.cmEvTransAltEpConnection(
                                                        (HSTRANSSESSION) session,
                                                        haTransSession);

                        /* If the AltEp connection procedure succeeded, we do not have to proceed with a loop.
                           Also no more error treatment is needed. */
                        if (res == cmTransOK)
                        {
                            /* release the session */
                            emaReleaseLocked((EMAElement)session);
                            /* unlock the host */
                            emaUnlock((EMAElement)host);
                            return;
                        }
                        else if (res == cmTransErrNoParty)
                        {
                            RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                                "cmEvTransConnectionOnSessionClosed(hsSession = %d(%p), haSessiont=%p, type=cmTransQ931Conn)",
                                emaGetIndex((EMAElement)session),session, haTransSession));

                            transGlobals->sessionEventHandlers.cmEvTransConnectionOnSessionClosed(
                                (HSTRANSSESSION) session,
                                haTransSession,
                                cmTransQ931Conn);
                        }
                    }
                    else
                    {
                        RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                            "cmEvTransConnectionOnSessionClosed(hsSession = %d(%p), haSessiont=%p, type=cmTransQ931Conn)",
                                emaGetIndex((EMAElement)session),session, haTransSession));

                        transGlobals->sessionEventHandlers.cmEvTransConnectionOnSessionClosed(
                                                        (HSTRANSSESSION) session,
                                                        haTransSession,
                                                        cmTransQ931Conn);
                    }

                    previousSession = session;
                    session         = session->nextSession;

                    /* release the session */
                    emaReleaseLocked((EMAElement)previousSession);
                }
            }

            if ( (transGlobals->hostEventHandlers.cmEvTransHostClosed) && (host->reported) &&
                 (host->state != hostClosing) && (host->state != hostClosed) )
            {
                /* report the host close itself */
                HATRANSHOST haTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)host);

                host->state = hostClosing;
                RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                    "cmEvTransHostClosed(hsHost = %d(%p), haHost=%p, reported = %d)",
                        emaGetIndex((EMAElement)host), host, haTransHost, host->reported));

                transGlobals->hostEventHandlers.cmEvTransHostClosed((HSTRANSHOST)host,
                                                                    haTransHost,
                                                                    host->reported);
                host->reported = RV_FALSE;
            }
        }
    }
    else
    {
        switch (event)
        {
            case RvSelectConnect:
            {
                TRANSERR res;

                /* if the host is in Idle state it does not wait a connection,
                   probably it was Droped by TransDrop. */
                if (host->state != hostIdle)
                {
                    /* a connection was established, notify the user */
                    host->state = hostConnected;

                    /* set the new local address of the host */
                    tpktGetAddresses(host->hTpkt, &host->localAddress, NULL);

                    /* register a new callback */
                    tpktRecvNotify(tpkt, transQ931Handler, host);

                    transReportConnect(host, NULL, RV_TRUE);

                    /* check if we have a stored SETUP message (already sent on Annex E
                       connection) that needs to be sent */
                    if (host->firstMessage)
                    {
                        res = sendMessageOnHost(transGlobals, host);
                        if ( (res != cmTransOK) && (res != cmTransConnectionBusy) )
                        {
                            RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                                "transQ931Handler Failed on send for host %d(%p)",
                                    emaGetIndex((EMAElement)host), host));
                        }
                    }
                }
                break;
            }

            case RvSelectAccept:
            /* can't happen, treated by another callback */
            {
                break;
            }

            case RvSelectClose:
            case RvSelectRead:
            {
                transQ931ReadOrCloseEvent(transGlobals, host, tpkt, -1, length, event, NULL);
                break;
            }

            case RvSelectWrite:
            {
                TRANSERR res;

                /* for TPKT the last send was completed */
                /* remove the message from the pool (for annex E
                   we must resend the last message that got WOULDBLOCK error) */
                if (host->annexEUsageMode == cmTransNoAnnexE)
                    extractMessageFromPool(transGlobals, host, RV_FALSE);

                /* mark the host as not busy and try sending the remaining messages */
                if ((host->state != hostClosing) && (host->state != hostClosed))
                    host->state = hostConnected;
                res = sendMessageOnHost(transGlobals, host);
                if ( (res != cmTransOK) && (res != cmTransConnectionBusy) )
                {
                    RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                        "transQ931Handler Failed on send for host %d(%p)",
                            emaGetIndex((EMAElement)host), host));
                }
                break;
            }

            default:
                break;
        }
    }

    /* unlock and release the marked host */
    emaUnlock((EMAElement)host);
}


/**************************************************************************************
 * connectH245Connection
 *
 * Purpose: Simulates a connect from the network on the H.245 connection. It is used
 *          when a dummy control session is built which does not actually connect to
 *          the network.
 *
 * input   : session - The handle to the session on which we need to report the connect.
 *
 * Output:  None.
 *
 **************************************************************************************/
static void connectH245Connection(
    IN cmTransSession*  session)
{
    cmTransHost *host = session->H245Connection;

    if (emaMarkLocked((EMAElement)host))
    {
        /* if it's an existing and connected host, just report it and return OK */
        if (host->state == hostConnecting)
        {
           /* all ok we can use that connection */
           host->state = hostConnected;

           /* the H.245 connection is established, notify the user */
           transReportConnect(host, NULL, RV_TRUE);
        }
        emaReleaseLocked((EMAElement)host);
    }
}

/**************************************************************************************
 * connectQ931Connection
 *
 * Purpose: Simulates a connect from the network on the Q.931 connection. It is used
 *          when a multiplexed host is already connected and we just want to simulate
 *          its connection event so the call signaling procedure will proceed.
 *
 * input   : session - The handle to the session on which we need to report the connect.
 *
 * Output:  None.
 *
 **************************************************************************************/
static void connectQ931Connection(
    IN cmTransSession*  session)
{
    cmTransHost *host;

    host = session->Q931Connection;

    if (emaMarkLocked((EMAElement)host))
    {
        /* if it's an existing and connected host, just report it and return OK */
        if ( (host->state == hostConnected) || (host->state == hostBusy) )
        {
            /* report to the session that the connection is established, no need
               to do anything with the annex E host or initiate a connect on the
               TPKT one */
            transReportConnect(host, session, RV_TRUE);
        }
        emaReleaseLocked((EMAElement)host);
    }
}

/**************************************************************************************
 * connectAnnexEConnection
 *
 * Purpose: Simulates a connect from the network on the annex E connection.
 *          It is used when an annex E host already exists and we just want to simulate
 *          its connection event so the call signaling procedure will proceed.
 *
 * input   : session - The handle to the session on which we need to report the connect.
 *
 * Output:  None.
 *
 **************************************************************************************/
static void connectAnnexEConnection(
    IN cmTransSession*  session)
{
    cmTransHost *host;

    host = session->annexEConnection;

    if (emaMarkLocked((EMAElement)host))
    {
        /* if it's an existing and connected host, just report it and return OK */
        if ( (host->state == hostConnected) || (host->state == hostBusy) )
        {
            /* report to the session that the connection is established, no need
               to do anything with the annex E host or initiate a connect on the
               TPKT one */
            transReportConnect(host, session, RV_TRUE);
        }
        emaReleaseLocked((EMAElement)host);
    }
}


/**************************************************************************************
 * zeroTimerEvent
 *
 * Purpose: call back routine that simulates a connect from the network on one of the
 *          connections (H245, Q931 or Annex E). It is used when a host is already
 *          connected and we just want to simulate its connection event as if we got
 *          it from the network
 *
 * input   : userData   - The handle to the session on which we need to report the connect.
 *
 * Output:  Always RV_FALSE.
 *
 **************************************************************************************/
RvBool zeroTimerEvent(IN void*  userData)
{
    cmTransSession* session = (cmTransSession *)userData;

    if (emaLock((EMAElement)session))
    {
        cmTransGlobals* transGlobals;

        /* retrieve the transport module global area */
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

        /* Handle all of the zero-timer events that we have for this session */
        if (m_sessionget(session, zeroTimerOnQ931Connection))
        {
            connectQ931Connection(session);
            m_sessionset(session, zeroTimerOnQ931Connection, RV_FALSE);
        }
        if (m_sessionget(session, zeroTimerOnH245Connection))
        {
            connectH245Connection(session);
            m_sessionset(session, zeroTimerOnH245Connection, RV_FALSE);
        }
        if (m_sessionget(session, zeroTimerOnAnnexEConnection))
        {
            connectAnnexEConnection(session);
            m_sessionset(session, zeroTimerOnAnnexEConnection, RV_FALSE);
        }

        /* Clear the zero-timer object */
        RvH323TimerClear(transGlobals->hTimers, &session->zeroTimer);

        emaUnlock((EMAElement)session);
    }

    return RV_FALSE;
}


/**************************************************************************************
 * transHostConnect
 *
 * Purpose: Starts the process of connecting on the given host connection.
 *
 * Input:   hsTransSession  - the handle to the session.
 *          type            - The type of the connection (annex E or TPKT)
 *
 * Output:  None.
 *
 * Returned Value:  cmTransWouldBlock - In case that a connect procedure was instigated but
 *                                      not yet completed.
 *                  cmTransOK         - In case that the connection is already opened.
 *                  cmTransErr        - couldn't create the hosts
 **************************************************************************************/
TRANSERR transHostConnect(IN HSTRANSSESSION hsTransSession,
                          IN TRANSINTCONNTYPE  type)
{
    cmTransSession          *session   = (cmTransSession *)hsTransSession;
    cmTransHost             *host      = NULL;
    cmTransHost             *extraHost = NULL;
    cmTransGlobals          *transGlobals;

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransSession);

    switch (type)
    {
        case cmTransTPKTConn:
        {
            host        = session->Q931Connection;
            break;
        }

        case cmTransAnnexEConn:
        {
            host        = session->annexEConnection;
            break;
        }

        case cmTransTPKTAnnexECompetition:
        {
            host        = session->Q931Connection;
            extraHost   = session->annexEConnection;
            type        = cmTransTPKTConn;
            break;
        }
    }

    if (emaLock((EMAElement)host))
    {
        /* mark the host, it might get deleted when reporting its connect */
        TRANSERR reply = cmTransOK;

        /* if it's an existing and connected host, just report it and return OK */
        if ((host->state == hostConnected) ||
            (host->state == hostDoubleConnected) ||
            (host->state == hostBusy))
        {
            /* report to the session that the connection is established, no need
               to do anything with the annex E host or initiate a connect on the
               TPKT one */

            /* Make sure we're simulating a connection */
            if (type == cmTransAnnexEConn)
                m_sessionset(session, zeroTimerOnAnnexEConnection, RV_TRUE);
            else
                m_sessionset(session, zeroTimerOnQ931Connection, RV_TRUE);

            /* Set a zero timer only if we don't have one set already */
            if (session->zeroTimer == NULL)
                session->zeroTimer = RvH323TimerStartWithType(transGlobals->hTimers, zeroTimerEvent,
                session, RvInt64Const(1, 0, 0), RV_TIMER_TYPE_ONESHOT);

            emaUnlock((EMAElement)host);

            if (session->zeroTimer != NULL)
                return cmTransOK;
            else
                return cmTransErr;
        }

        /* if we have an annex E but not a connected TPKT,
           report that the annex E host is connected */
        if (emaLock((EMAElement)extraHost))
        {
            /* report to the session that the connection is established */

            /* Make sure we're simulating a connection */
            m_sessionset(session, zeroTimerOnAnnexEConnection, RV_TRUE);

            /* Set a zero timer only if we don't have one set already */
            if (session->zeroTimer == NULL)
                session->zeroTimer = RvH323TimerStartWithType(transGlobals->hTimers, zeroTimerEvent,
                session, RvInt64Const(1, 0, 0), RV_TIMER_TYPE_ONESHOT);

            emaUnlock((EMAElement)extraHost);

            if (session->zeroTimer == NULL)
                reply = cmTransErr;
        }

        /* TPKT host is not connected or approved,
           we need to initiate a connect process for it */
        if (host->remoteAddress.port != 0)
        {
            /* allocate a new tpkt element for the connection and connect it*/
            if (host->state == hostIdle)
            {
                getGoodAddress(transGlobals,
                               host->hTpkt,
                               NULL,
                               cmTransQ931Conn,
                               &host->localAddress);

                host->hTpkt = tpktOpen( transGlobals->tpktCntrl,
                                        &host->localAddress,
                                        RvH323ConnectionQ931,
                                        tpktClient,
                                        (EMAElement)host,
                                        transQ931Handler,
                                        (void *)host);
                if (host->hTpkt != NULL)
                    host->state = hostConnecting;
                else
                    reply = cmTransErr;
            }

            if (host->state == hostConnecting)
            {

                if (transGlobals->hostEventHandlers.cmEvTransHostConnecting)
                {
                    HATRANSHOST haTransHost =
                                 (HATRANSHOST)emaGetApplicationHandle(((EMAElement)host));
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
                    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog,
                            "cmEvTransHostConnecting(hsHost = %d(%p), haHost=%p, type=cmTransQ931Conn, address = (ip:%s,port:%u))",
                            emaGetIndex((EMAElement)host), host, haTransHost, getIP(&host->remoteAddress, buff), host->remoteAddress.port));

                    reply = transGlobals->hostEventHandlers.cmEvTransHostConnecting(
                                            (HSTRANSHOST)host,
                                            haTransHost,
                                            cmTransQ931Conn,
                                            &host->remoteAddress);
                    host->reported = RV_TRUE;
                }

                if (reply==cmTransOK)
                {
                    if (tpktConnect(host->hTpkt, &host->remoteAddress) != RV_OK)
                    {
                        /* For some reason, we are not able to connect this socket */
                        reply = cmTransErr;
                    }
                }
                else
                {
                    if (reply == cmTransIgnoreMessage)
                    {
                        RvLogInfo(&transGlobals->hLog,
                            (&transGlobals->hLog, "transHostConnect connect on Q.931 TPKT refused by user"));
                    }

                    /* Make sure we're not in some kind of an error state */
                    reply = cmTransOK;
                }
            }
        }
        else
        {
            /* see if this was our only choice */
            if (extraHost == NULL)
            {
                /* warn that no port was given */
                RvLogWarning(&transGlobals->hLog,
                    (&transGlobals->hLog, "transHostConnect: Remote address port is 0..."));
            }
        }

        emaUnlock((EMAElement)host);

        /* the process has begun but not yet ended, inform the user of that */
        if (reply == cmTransOK)
            return cmTransWouldBlock;
    }

    return cmTransErr;
}

/**************************************************************************************
 * getMsgDataFromRPool
 *
 * Purpose: reads the encoded message from the rpool, gets its size and its CRV
 *          from the message header and then extracts from the encoded message itself its type.
 *
 * Input:   hRpool  - Handle to the messages rpool.
 *          message - the message in the rpool
 *          buffer  - a buffer to put the message in.
 *
 * Output:  CRV     - The crv of the message, as was put on message header in the rpool.
 *          msgSize - The size of the encoded message (including its headers).
 *
 * Returned Value:  mstType - The type of the message as extracted from the encoded message
 *                            [the type is in the first byte after the CRV, and the CRV length
 *                             is in the second byte - offset 1].
 **************************************************************************************/
static int getMsgDataFromRPool(HRPOOL hRpool, HRPOOLELEM message, RvUint8 *buffer, RvUint16 *CRV, int *msgSize)
{
    RvUint8 *encodedMsg;

    /* extract the message from the rpool to the buffer */
    rpoolCopyToExternal(hRpool,
                        buffer,
                        message,
                        0,
                        *msgSize);

    /* get the message size (including headers) */
    *msgSize = rpoolChunkSize(hRpool, message);

    /* extract the CRV from the message header */
    memcpy(CRV, &buffer[MSG_HEADER_SIZE], sizeof(*CRV));

    /* get the encoded message (without headers) */
    encodedMsg = &buffer[MSG_HEADER_SIZE + TPKT_HEADER_SIZE];

    /* return the message type from the encoded message */
    return encodedMsg[2 + encodedMsg[1]];

}


/**************************************************************************************
 * sendMessageOnHost
 *
 * Purpose: send a message on the given host according to its communication protocol.
 *
 * Input:   transGlobal - The global variables of the module.
 *          host        - The host on which to send the messages that await to be sent.
 *
 * Output:  None.
 *
 * Returned Value:  cmTransOK             - All is ok.
 *                  cmTransConnectionBusy - The connection is still sending previous messages.
 **************************************************************************************/
TRANSERR sendMessageOnHost(
    IN cmTransGlobals*  transGlobals,
    IN cmTransHost*     host)
{
    HRPOOLELEM nextMessage;
    int         bytesSent = 0;

    /* check if the host is ready for send (treat Annex E as ready) */
    if ((host->state == hostConnected) || (host->state == hostDoubleConnected) ||
        (host->annexEUsageMode != cmTransNoAnnexE))
    {
        /* go over all stored messages for sending */
        nextMessage = host->firstMessage;
        while (nextMessage)
        {
            /* Check the host type - we need to know it before we start sending stuff */
            switch (host->type)
            {
                case cmTransQ931Conn:
                {
                    /* send according to the type of communications, TPKT or annex E */
                    if (host->annexEUsageMode == cmTransNoAnnexE)
                    {
                        /* Q931 sent on TCP-TPKT */
                        RvLogInfo(&transGlobals->hLog,
                            (&transGlobals->hLog, "sendMessageOnHost sending on TPKT"));

                        bytesSent = tpktSendFromRpool(host->hTpkt,
                            transGlobals->messagesRPool, nextMessage,
                            MSG_HEADER_SIZE, &transGlobals->lockHash);
                        if ((bytesSent > 0) && (host->connectTimer != NULL))
                        {
                            /* cancel the accept timer if we're already sending on this host */
                            RvH323TimerCancel(transGlobals->hTimers, &host->connectTimer);
                        }
                    }
                    else
                    {
                        /* Q931 sent on UDP-AnnexE */
                        RvUint8 *buffer;
                        RvUint16 CRV;
                        int msgType;
                        int msgSize;
                        annexEStatus eStat;

                        getEncodeDecodeBuffer(transGlobals->codingBufferSize, &buffer);
                        msgSize = transGlobals->codingBufferSize;
                        msgType = getMsgDataFromRPool(transGlobals->messagesRPool,
                            nextMessage, buffer, &CRV, &msgSize);

                        /* remove the headers size */
                        msgSize = msgSize - (TPKT_HEADER_SIZE + MSG_HEADER_SIZE);

                        RvLogInfo(&transGlobals->hLog,
                            (&transGlobals->hLog, "sendMessageOnHost sending on annex E, size=%d",msgSize));

                        eStat = annexESendMessage(transGlobals->annexECntrl,
                            &host->remoteAddress, &host->localAddress,
                            CRV, &buffer[TPKT_HEADER_SIZE + MSG_HEADER_SIZE],
                            msgSize, (msgType != (int)cmQ931releaseComplete),
                            ((msgType == (int)cmQ931setup) || (msgType == (int)cmQ931statusEnquiry)),
                            RV_TRUE);

                        if (eStat == annexEStatusWouldBlock)
                        {
                            bytesSent = 0;
                        }
                        else if (eStat != annexEStatusNormal)
                        {
                            RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                                "sendMessageOnHost failed on annex E send err=%d",eStat));
                        }
                        else
                            bytesSent = msgSize;
                        break;
                    }
                    break;
                }

                case cmTransH245Conn:
                {
                    HTPKT tpkt = host->hTpkt;
                    RvBool bInConn = ((host->h245Listen != NULL) && tpktIsConnected(host->h245Listen));
                    RvBool bOutConn = ((host->hTpkt != NULL) && tpktIsConnected(host->hTpkt));

                    if (bInConn && bOutConn)
                    {
                        /* if the remote address is bigger or if both address are the same but the remote
                           port is bigger, then use the connecting host, else do the opposite */
                        if (compareH245Connections(host) < 0)
                            tpkt = host->h245Listen;
                    }
                    else if (bOutConn)
                        tpkt = host->hTpkt;
                    else if (bInConn)
                        tpkt = host->h245Listen;
                    else
                    {
                        bytesSent = -1;
                        if ((host->h245Listen != NULL) && (host->hTpkt != NULL))
                        {
                            RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                                "sendMessageOnHost both H.245 connections are not connected on host %d(%p)",
                                    emaGetIndex((EMAElement)host), host));
                        }
                        break;
                    }

                    bytesSent = tpktSendFromRpool(tpkt,
                        transGlobals->messagesRPool, nextMessage,
                        MSG_HEADER_SIZE, &transGlobals->lockHash);
                    break;
                }
            }

            /* check if the send was successful */
            if (bytesSent > 0)
            {
                nextMessage = extractMessageFromPool(transGlobals, host, RV_FALSE);
            }
            else
            if (bytesSent == 0)
            {
                /* we need to wait for the send to complete */
                if (host->state != hostClosed)
                    host->state = hostBusy;
                nextMessage = NULL;
                RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                    "sendMessageOnHost would block on send for host %d(%p)",
                        emaGetIndex((EMAElement)host), host));
                return cmTransConnectionBusy;
            }
            /* delete any message in this list of RPOOL messages */
            else
            {
                nextMessage = NULL;
                if (tpktIsConnected(host->hTpkt))
                {
                    RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                        "sendMessageOnHost got error on send for host %d(%p)",
                            emaGetIndex((EMAElement)host), host));
                    return cmTransErr;
                }
                else
                {
                    RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                        "sendMessageOnHost: tpkt element is already closed %d(%p)",
                            emaGetIndex((EMAElement)host), host));
                    return cmTransConnectionClosed;
                }
            }
        }
    }
    else
    {
        /* the host is either temporarily unavaliable, or is closing */
        if ((host->state == hostClosing) || (host->state == hostClosed))
        {
            /* if this host is annex E, do nothing */
            if (host->annexEUsageMode != cmTransNoAnnexE)
                return cmTransConnectionClosed;

            /* this host is closing, and there is no hope of ever sending messages on it.
            remove all the messages on it, just to make sure they are freed */
            nextMessage = host->firstMessage;
            while (nextMessage)
            {
                nextMessage = extractMessageFromPool(transGlobals, host, RV_FALSE);
            }
            /* no more messages on host */
            host->firstMessage = NULL;
            return cmTransConnectionClosed;
        }
        /* it is just busy now. keep the messages for later */
        return cmTransConnectionBusy;
    }

    return cmTransOK;
}


/**************************************************************************************
 * receiveExtQ931MessageOnHost
 *
 * Purpose: send a message on the given host according to its communication protocol.
 *
 * Input:   transGlobal - The global variables of the module.
 *          host        - The host on which to send the messages that await to be sent.
 *          msgToGet    - Buffer containing the message to get
 *          msgLen      - Length of the message
 *
 * Output:  None.
 *
 * Returned Value:  None.
 **************************************************************************************/
void receiveExtQ931MessageOnHost(
    IN cmTransGlobals*  transGlobals,
    IN cmTransHost*     host,
    IN RvUint8*         msgToGet,
    IN RvInt            msgLen)
{
    HTPKT tpkt = NULL;
    RvInt index = -1;

    if (host->annexEUsageMode == cmTransNoAnnexE)
        tpkt = host->hTpkt;
    else
    {
        RvUint8 i = 0;
        if (annexEGetLocalAddressIndex(transGlobals->annexECntrl, &host->localAddress, &i) == annexEStatusNormal)
            index = (RvInt)i;
    }

    transQ931ReadOrCloseEvent(transGlobals, host, tpkt, index, msgLen, RvSelectRead, msgToGet);
}


static void reportBadMessage(
    cmTransGlobals         *transGlobals,
    cmTransHost            *host,
    TRANSTYPE               type,
    RvH323ConnectionType    connType,
    RvUint8                *buffer,
    int                     bytesToDecode,
    void                  **hMsgContext)
{
    HATRANSHOST haTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)host);

    /* We can't decode this message - see if we know the session of this message */
    if ((host->firstSession != NULL) &&
        (((type == cmTransQ931Type) && !m_sessionget(host->firstSession, isMultiplexed)) ||
         (host->type == cmTransH245Conn)))
    {
        /* Seems like its an H245 or a non-multiplexed Q931 message. Session is known! */
        if (transGlobals->sessionEventHandlers.cmEvTransBadMessage)
        {
            EMAElement session = host->firstSession;
            HATRANSSESSION haTransSession;

            if (emaMarkLocked((EMAElement)session))
            {
                haTransSession = (HATRANSSESSION)emaGetApplicationHandle(session);
                RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                    "cmEvTransBadMessage(hsSession = %d(%p), haSession=%p, type=%d,outgoing=FALSE)",
                        emaGetIndex((EMAElement)session), session, haTransSession, type));

                transGlobals->sessionEventHandlers.cmEvTransBadMessage(
                            (HSTRANSSESSION) session,
                            haTransSession,
                            connType,
                            buffer,
                            bytesToDecode,
                            RV_FALSE);

                emaReleaseLocked((EMAElement)session);
            }
        }
    }
    else
    {
        /* We don't really know the session - use host callback instead */
        if (transGlobals->hostEventHandlers.cmEvTransHostBadMessage)
        {
            RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                "cmEvTransHostBadMessage(hsHost = %d(%p), haHost=%p, type=%d, outgoing=FALSE)",
                    emaGetIndex((EMAElement)host), host, haTransHost, type));

            transGlobals->hostEventHandlers.cmEvTransHostBadMessage(
                (HSTRANSHOST) host, haTransHost, type, buffer,
                bytesToDecode, RV_FALSE, hMsgContext);
        }
        else if ((host->remoteIsMultiplexed == RV_FALSE) && (host->firstSession == NULL))
        {
            /* First message on a host didn't get decoded properly - we can close it */
            transHostClose((HSTRANSHOST)host, RV_TRUE);
        }
    }
}


/**************************************************************************************
 * decodeIncomingMessage
 *
 * Purpose: tries to receive and decode an incoming message
 *
 * Input:   tpkt    - a handle to the TPKT connection
 *          length  - the length of the received message (0 if no buffers for reading the message)
 *          offset  - Where to start the decoding process in the encoded buffer
 *          context - The context associated with the connection , i.e. the host element
 *          node    - The PVT node into which to decode the message
 *          type    - The type of the message.
 *          extBuffer - External message buffer
 *
 * Output:  hMsgContext - An external context associated with the message.
 *
 * Returned Value:  number of bytes decoded, or <=0 in case of trouble.
 **************************************************************************************/
int  decodeIncomingMessage( HTPKT       tpkt,
                            int         *length,
                            int         *offset,
                            void        *context,
                            int         *node,
                            TRANSTYPE   type,
                            void        **hMsgContext,
                            RvUint8     *extBuffer)
{
    cmTransGlobals *transGlobals;
    cmTransHost *host = (cmTransHost *)context;
    HATRANSHOST haTransHost;
    int bytesToDecode;
    int pvtNode;
    RvUint8 *buffer;
    HPST syntax;
    HRPOOLELEM *inMsg;

    TRANSERR decTransRes = cmTransOK;
    int      decoded;
    int      decResult = 0;

    *hMsgContext = NULL;
    *node = -1;

    if (host == NULL)
    {
        *length = 0;
        return decResult;
    }

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);

    /* retrieve the host's application handle */
    haTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)host);

    /* initialize the number of bytes to decode and the number already decoded */
    decoded         = 0;
    bytesToDecode   = *length;

    if (host->hTpkt == tpkt)
        inMsg = &host->incomingMessage;
    else
        inMsg = &host->incomingListeningMessage;

    /* if no message, supply it through tpktRecvIntoRpool */
    if ( (!*length) && (tpkt != NULL) )
    {
        if (*inMsg == NULL)
        {
            RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
            *inMsg = rpoolAlloc(transGlobals->messagesRPool, rpoolGetElemSize(transGlobals->messagesRPool));
            if (*inMsg == NULL)
            {
                RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                    "rpoolAlloc[decodeIncomingMessage] - Unable to allocate memory"));
                *length = 0;
                RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);
                return -1;
            }

            {
                RvInt32 poolSize, usageSize, allocatedSize;

                if (rpoolStatistics(transGlobals->messagesRPool,
                                    &poolSize,
                                    &usageSize,
                                    &allocatedSize))
                {
                    RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                        "rpoolAlloc[decodeIncomingMessage] statistics: max=%d, usage=%d, allocated=%d",
                            poolSize, usageSize, allocatedSize));
                }
            }
            RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);
        }

        bytesToDecode = tpktRecvIntoRpool(tpkt,
                                          transGlobals->messagesRPool,
                                          *inMsg,
                                          &transGlobals->lockHash);
    }

    if (bytesToDecode > 0)
    {
        /* check if the buffer is sufficient for the message */
        if (transGlobals->codingBufferSize < (RvSize_t)bytesToDecode)
        {
            *length = 0;
            decoded = -1;
            RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                "decodeIncomingMessage has too small a buffer. bufferSize=%d length=%d",
                    transGlobals->codingBufferSize, bytesToDecode));
        }
        else
        {
            RvH323ConnectionType connType;

            if (extBuffer == NULL)
            {
                getEncodeDecodeBuffer(transGlobals->codingBufferSize, &buffer);
                /* copy the message to the buffer */
                rpoolCopyToExternal(transGlobals->messagesRPool,
                    buffer, *inMsg, *offset, bytesToDecode);
            }
            else
            {
                /* use the message already in the external buffer */
                buffer = extBuffer;
            }

            /* decode the message */
            if (type == cmTransQ931Type)
            {
                syntax = transGlobals->synProtQ931;
                connType = RvH323ConnectionQ931;
            }
            else
            {
                syntax = transGlobals->synProtH245;
                connType = RvH323ConnectionH245;
            }
            pvtNode = pvtAddRoot(transGlobals->hPvt, syntax, 0, NULL);

            if (transGlobals->hostEventHandlers.cmEvTransNewRawMessage)
            {
                RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                    "cmEvTransNewRawMessage(hsHost = %d(%p), haHost=%p, type = %d pvt=%d bytesToDecode=%d)",
                        emaGetIndex((EMAElement)host), host, haTransHost, connType, pvtNode, bytesToDecode));

                decTransRes = transGlobals->hostEventHandlers.cmEvTransNewRawMessage(
                    (HSTRANSHOST) host, haTransHost, connType, pvtNode, &buffer[decoded],
                    bytesToDecode, &decoded, hMsgContext);
            }
            else
            {
                decResult = cmEmDecode(transGlobals->hPvt, pvtNode, &buffer[decoded], bytesToDecode, &decoded);
            }

            /* check if we need to ignore the message */
            if (decTransRes == cmTransIgnoreMessage)
            {
                RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                    "decodeIncomingMessage was instructed to ignore the message"));
                pvtDelete(transGlobals->hPvt, pvtNode);
                *length = 0;
                decoded = -1;
            }
            else if ((decResult < 0) || (decTransRes != cmTransOK))
            {
                decoded = -1;
                *length = 0;
                RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                    "decodeIncomingMessage failed on decoding"));
                pvtDelete(transGlobals->hPvt, pvtNode);
                /* We can't decode this message - report it */
                reportBadMessage(transGlobals, host, type, connType, buffer, bytesToDecode, hMsgContext);
            }
            else if (emaWasDeleted((EMAElement)host))
            {
                decoded = -1;
                *length = 0;
                RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
                    "host was deleted while receiving a message"));
                pvtDelete(transGlobals->hPvt, pvtNode);
            }
            else
            {
                *node = pvtNode;
                *length = bytesToDecode - decoded;
                offset += decoded;
            }
        }

        /* exit procedures */
        if (*length <= 0)
        {
            /* get rid of the rpool message */
            RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
            rpoolFree(transGlobals->messagesRPool, *inMsg);
            *inMsg = NULL;
            {
                RvInt32 poolSize, usageSize, allocatedSize;

                if (rpoolStatistics(transGlobals->messagesRPool,
                                    &poolSize,
                                    &usageSize,
                                    &allocatedSize))
                {
                    RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                        "rpoolFree[decodeIncomingMessage] statistics: max=%d, usage=%d, allocated=%d",
                            poolSize, usageSize, allocatedSize));
                }
            }
            RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);
        }
    }
    else
    if (bytesToDecode < 0)
    {
        if (*inMsg != NULL)
        {
            /* get rid of the rpool message */
            RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
            rpoolFree(transGlobals->messagesRPool, *inMsg);
            *inMsg = NULL;
            {
                RvInt32 poolSize, usageSize, allocatedSize;

                if (rpoolStatistics(transGlobals->messagesRPool,
                                    &poolSize,
                                    &usageSize,
                                    &allocatedSize))
                {
                    RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                        "rpoolFree[decodeIncomingMessage-err] statistics: max=%d, usage=%d, allocated=%d",
                            poolSize, usageSize, allocatedSize));
                }
            }
            RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);
        }
        *length = 0;
        decoded = -1;
    }
    else
    {
        *length = 0;
        decoded = -1;
    }

    return decoded;
}


/**************************************************************************************
 * processQ931IncomingMessage
 *
 * Purpose: handles incoming messages:
 *          extract the states of the faststart and tunneling (including parallel one).
 *          updates the multiplexing parameters of the host
 *          decides whether to report the opening of a H.245 connection
 *          decides whether to initiate a connect for H.245
 *          extract all kinds of tunneled messages from the message
 *
 * Input:   host      - The host on which the message arrived
 *          session   - The session on which the message arrived (i.e. the call)
 *          pvtNode   - The decoded message itself
 *
 * Output:  None.
 *
 * Return value: msgType - the message type.
 *
 **************************************************************************************/
int processQ931IncomingMessage(cmTransHost *host, cmTransSession *session, int pvtNode)
{
    cmTransGlobals  *transGlobals;
    int             messageBodyNode;
    int             msgType;

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);

    /* check what type of Q.931 message we have here */
    msgType = pvtGetChildTagByPath(transGlobals->hPvt, pvtNode, "message", 1);
    if (msgType < 0)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog,
                "processQ931IncomingMessage failed to get message type tag"));
        return -1;
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
        return msgType;

    /***********************************************************************/
    /* get the multipleCalls and maintainConnection parameters (multiplexing stuff) */
    /***********************************************************************/
    getMultiplexedParams(transGlobals, host, pvtNode, msgType);

    /***********************************************************************/
    /* check the extended fast connect state of the session */
    /***********************************************************************/
    if ( (session) && !emaWasDeleted((EMAElement)session) && (msgType !=  cmQ931releaseComplete) )
        getFeatureSet(transGlobals, session, pvtNode, msgType);

    /***********************************************************************/
    /* check the fast start state of the session */
    /***********************************************************************/
    if ( (session) && !emaWasDeleted((EMAElement)session) )
        setTheFastStartStateByMessage(transGlobals, session, messageBodyNode, msgType);

    /***********************************************************************/
    /* check the tunneling state of the session  */
    /***********************************************************************/
    if ( (session) && !emaWasDeleted((EMAElement)session) )
        setTheTunnelingStateByMessage(transGlobals, session, messageBodyNode, msgType);

    /***********************************************************************/
    /* check the parallel tunneling state of the session  */
    /***********************************************************************/
    if ( (session) && !emaWasDeleted((EMAElement)session) )
        setTheParallelTunnelingStateByMessage(transGlobals,
                                              session,
                                              msgType,
                                              messageBodyNode,
                                              RV_FALSE /* incoming */);

    /***********************************************************************/
    /* handling whether to start listening or establish connection for H.245 */
    /***********************************************************************/
    if ( (session) && !emaWasDeleted((EMAElement)session) )
        if (!m_sessionget(session, notEstablishControl))
            determineIfToOpenH245(RV_FALSE /* incoming message */,
                                  transGlobals,
                                  session,
                                  messageBodyNode,
                                  msgType);

    /***********************************************************************/
    /* handle reporting of a new H.245 connection to the user    */
    /***********************************************************************/
    if ( (session) && !emaWasDeleted((EMAElement)session) )
        if (!m_sessionget(session, notEstablishControl))
            reportH245(RV_FALSE /* incoming message */,
                       transGlobals,
                       session,
                       messageBodyNode,
                       msgType);

    /***********************************************************************/
    /* handle extracting the tunneled H.245 messages from the Q.931 message    */
    /***********************************************************************/
    if ( (session) && !emaWasDeleted((EMAElement)session) )
    {
        if (!m_sessionget(session, notEstablishControl) && (session->h245Stage != cmTransH245Stage_noH245))
            extractH245Messages(transGlobals, session, host, messageBodyNode, msgType);

        if (!emaWasDeleted((EMAElement)session))
            extractH450Messages(transGlobals, session, pvtNode, msgType);
        if (!emaWasDeleted((EMAElement)session))
            extractAnnexMMessages(transGlobals, session, messageBodyNode, msgType);
        if (!emaWasDeleted((EMAElement)session))
            extractAnnexLMessages(transGlobals, session, messageBodyNode, msgType);
    }

    return msgType;
}


/**************************************************************************************
 * connectH245
 *
 * Purpose: connects if the host state permits, a H.245 connection.
 *
 * Input:   transGlobal - The global variables of the module.
 *          session     - The session for which the connection is done
 *          msgType     - The type of the message that is being checked
 *
 * Output:  None.
 *
 **************************************************************************************/
void connectH245(cmTransGlobals     *transGlobals,
                 cmTransSession     *session,
                 cmCallQ931MsgType  msgType)
{
    if (!emaMarkLocked((EMAElement)session->H245Connection))
        return;

    /* if host is not connecting or connected yet we may start to connect */
    if ((session->H245Connection->state == hostIdle) ||
        (session->H245Connection->state == hostListenning) ||
        (session->H245Connection->dummyHost) )
    {
        /* Check if we have a remote address to open to,
           if we do continue with connecting, if not we need to issue
           a startH245 FACILITY message */
        if (cmTaHasIp(&session->H245Connection->remoteAddress))
        {
            HOSTSTATE oldState = session->H245Connection->state;
            TRANSERR reply = cmTransOK;
            cmTransportAddress localAddr;

            /* mark the host as connecting if it was closed.
               If it was listening, mark it as Listening-Connecting,
               i.e. we both trying to connect and listen
               "and may the best man win" */
            if (oldState == hostListenning)
                session->H245Connection->state = hostListenningConnecting;
            else
                session->H245Connection->state = hostConnecting;

            memset(&localAddr, 0, sizeof(localAddr));
            getGoodAddress(transGlobals, NULL, session->Q931Connection, cmTransH245Conn, &localAddr);
            if (!cmTaHasIp(&localAddr))
            {
                memcpy(&localAddr, &transGlobals->localAddress, sizeof(localAddr));
                localAddr.port = 0;
            }

            /* initiate connect process */
            session->H245Connection->hTpkt = tpktOpen(transGlobals->tpktCntrl,
                             &localAddr,
                             RvH323ConnectionH245,
                             tpktClient,
                             (EMAElement)session->H245Connection,
                             transH245Handler,
                             (void *)session->H245Connection);

            /* start a connect process */
            if (transGlobals->hostEventHandlers.cmEvTransHostConnecting && !session->H245Connection->dummyHost)
            {
                HATRANSHOST haTransHost =
                             (HATRANSHOST)emaGetApplicationHandle(((EMAElement)session->H245Connection));
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
                RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "cmEvTransHostConnecting(hsHost = %d(%p), haHost=%p, type=cmTransH245Conn, address = (ip:%s,port:%u))",
                        emaGetIndex((EMAElement)session->H245Connection), session->H245Connection, haTransHost,
                        getIP(&session->H245Connection->remoteAddress, buff), session->H245Connection->remoteAddress.port));

                reply = transGlobals->hostEventHandlers.cmEvTransHostConnecting(
                                        (HSTRANSHOST)session->H245Connection,
                                        haTransHost,
                                        cmTransH245Conn,
                                        &session->H245Connection->remoteAddress);
                session->H245Connection->reported = RV_TRUE;
            }

            if (reply == cmTransOK)
            {
                if (session->H245Connection->hTpkt)
                {
                    if (tpktConnect(session->H245Connection->hTpkt,
                                &session->H245Connection->remoteAddress) != RV_OK)
                    {
                        /* Unable to connect - we might as well just close the connection */
                        tpktClose(session->H245Connection->hTpkt);
                        session->H245Connection->hTpkt = NULL;
                        reply = cmTransErr;
                    }
                }
            }
            else /* very unusual state: the user does not want to proceed connecting process */
            {
                tpktClose(session->H245Connection->hTpkt);
                session->H245Connection->hTpkt = NULL;
            }

            /* report failures */
            if (reply == cmTransIgnoreMessage)
            {
                session->H245Connection->state = oldState;
                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog, "connectH245 connect on H.245 TPKT refused by user"));
            }
            else if (session->H245Connection->hTpkt == NULL)
            {
                session->H245Connection->state = oldState;
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog, "connectH245 failed to initiate connect on H.245 TPKT"));
            }
        }
        else
        {
            /* If this was a CONNECT message and still no address was given,
               we need to send facility with startH245 */
            if ((msgType == cmQ931connect) &&
                (m_sessionget(session, openControlWasAsked) || m_sessionget(session, switchToSeparateWasAsked)))
            {
                m_sessionset(session, needToSendFacilityStartH245, RV_TRUE);
            }
        }
    }

    emaReleaseLocked((EMAElement)session->H245Connection);
}


/**************************************************************************************
 * reportH245Establishment
 *
 * Purpose: if not done so yet, report to the user about an active H.245 connection.
 *
 * Input:   transGlobal - The global variables of the module.
 *          session     - The session for which the connection is done
 * Output:  None.
 *
 **************************************************************************************/
void reportH245Establishment(cmTransGlobals *transGlobals, cmTransSession *session)
{
    if ( (!m_sessionget(session, reportedH245Connect)) &&
         (session->h245Stage != cmTransH245Stage_noH245) )
    {
        HATRANSSESSION haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);
        RvBool bParallelFSandTunneling, bPostFSTunneling;

        m_sessionset(session, reportedH245Connect, RV_TRUE);

        /* Tunneled H.245 ok if using parallel H.245 */
        bParallelFSandTunneling = (m_sessionget(session, isParallelTunnelingSupported) &&
            (session->parallelTunnelingState != parllNo));

        /* Tunneled H.245 also ok if FS is over */
        bPostFSTunneling = ((session->tunnelingState == tunnApproved) &&
            (session->faststartState == fsApproved));

        if (!bParallelFSandTunneling && !bPostFSTunneling)
        {
            /* terminate EFC */
            if (transGlobals->sessionEventHandlers.cmEvTransExtFastConnestState != NULL)
            {
                transGlobals->sessionEventHandlers.cmEvTransExtFastConnestState(
                    (HSTRANSSESSION)session, (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session),
                    (EFCSTATE) efcNo);
            }
        }

        if (transGlobals->sessionEventHandlers.cmEvTransSessionNewConnection)
        {
            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "cmEvTransSessionNewConnection(hsSession = %d(%p), haSession=%p, type=cmTransH245Conn)",
                    emaGetIndex((EMAElement)session), session, haTransSession));

            transGlobals->sessionEventHandlers.cmEvTransSessionNewConnection(
                                    (HSTRANSSESSION) session,
                                    haTransSession,
                                    cmTransH245Conn);
        }
    }
}

/**************************************************************************************
 * annexEEvNewMessage
 *
 * Purpose: Callback for Annex E upon receiving new incoming message.
 *          The call back looks for the host (or create one if it's new) and
 *          then calls the regular event handler (of TPKT) with event liEvRead.
 *
 * Input:   hAnnexE       - The annex E module handle
 *          hAppAnnexE    - The application handle of the annex E module (actually
 *                          our very own transGlobals structure).
 *          localAddrIndex- Index of the local address that received this message.
 *          remoteAddress - The remote address whence the message arrived.
 *          pMessage      - The encoded message itself.
 *          nSize         - The size of the message.
 *
 * Output:  None.
 *
 **************************************************************************************/
annexEStatus RVCALLCONV annexEEvNewMessage(
    IN  HANNEXE             hAnnexE,
    IN  HAPPANNEXE          hAppAnnexE,
    IN  RvInt               localAddressIndex,
    IN  cmTransportAddress* remoteAddress,
    IN  void*               pMessage,
    IN  int                 nSize)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)hAppAnnexE;
    cmTransHost *host = NULL;
    TRANSERR res;
    RvBool ok = RV_TRUE;
    cmTransportAddress locAddr;

    if ((hAnnexE == NULL) || (transGlobals == NULL))
        return annexEStatusBadParameter;

    annexEGetLocalAddress(hAnnexE, localAddressIndex, &locAddr);
    host = findHostInHash(transGlobals, remoteAddress, &locAddr, RV_TRUE, RV_TRUE);

    if (host == NULL)
    {
        /* create a new connected host element for the new connection */
        res = createHostByType(transGlobals,
                               NULL,
                               (HSTRANSHOST *)&host,
                               cmTransQ931Conn,
                               NULL,
                               cmTransUseAnnexE,
                               localAddressIndex);
        if (res != cmTransOK)
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "annexEEvNewMessage rejected incoming message (host wasnt created) res=%d", res));
            return annexEStatusResourceProblem;
        }

        /* mark the host as incoming one */
        host->incoming = RV_TRUE;

        /* set the remote address to the host */
        setRemoteAddress(host, remoteAddress);

        /* lock and mark the host before reporting it */
        emaLock((EMAElement)host);

        host->state = hostConnected;

        transReportConnect(host, NULL, RV_FALSE);
    }
    else
    {
        ok = RV_FALSE;

        if (!emaWasDeleted((EMAElement)host))
            if (emaLock((EMAElement)host))
                ok = RV_TRUE;
    }

    if (ok)
    {
        HRPOOLELEM msg;

        RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
        msg = rpoolAllocCopyExternal(transGlobals->messagesRPool, 0, pMessage, nSize);

        {
            RvInt32 poolSize, usageSize, allocatedSize;

            if (rpoolStatistics(transGlobals->messagesRPool,
                                &poolSize,
                                &usageSize,
                                &allocatedSize))
            {
                RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                    "rpoolAllocCopyExternal[annexEEvNewMessage] statistics: max=%d, usage=%d, allocated=%d",
                        poolSize, usageSize, allocatedSize));
            }
        }
        RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);

        if (msg != NULL)
        {
            host->incomingMessage = msg;
            transQ931ReadOrCloseEvent(transGlobals, host, NULL, localAddressIndex, nSize, RvSelectRead, NULL);
        }

        if (emaWasDeleted((EMAElement)host))
            ok = RV_FALSE;

        emaUnlock((EMAElement)host);
    }

    if (ok)
        return annexEStatusNormal;
    return annexEStatusBadMessage;
}

/**************************************************************************************
 * annexEEvConnectionBroken
 *
 * Purpose: Callback for Annex E upon expiration of I-AM-ALIVE timer & retries.
 *          The call back looks for the host and then calls the regular event handler
 *          (of TPKT) with event liEvClose.
 *
 * Input:   hAnnexE       - The annex E module handle
 *          hAppAnnexE    - The application handle of the annex E module (actually
 *                          our very own transGlobals structure).
 *          remoteAddress - The remote address whence the message arrived.
 *
 * Output:  None.
 *
 **************************************************************************************/
annexEStatus RVCALLCONV annexEEvConnectionBroken(
    IN  HANNEXE             hAnnexE,
    IN  HAPPANNEXE          hAppAnnexE,
    IN  cmTransportAddress* remoteAddress,
    IN  int                 localAddressIndex
)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)hAppAnnexE;
    cmTransHost *host;
    cmTransportAddress locAddr;

    RV_UNUSED_ARG(hAppAnnexE);

    if (!hAnnexE)
        return annexEStatusBadParameter;

    if (!transGlobals)
        return annexEStatusBadParameter;

    annexEGetLocalAddress(hAnnexE, localAddressIndex, &locAddr);
    host = findHostInHash(transGlobals, remoteAddress, &locAddr, RV_TRUE, RV_TRUE);

    if (host != NULL)
        transQ931Handler(NULL, RvSelectClose, 0, 0, host);

    return annexEStatusNormal;
}

/**************************************************************************************
 * annexEEvUseOtherAddress
 *
 * Purpose: Callback for Annex E upon receiving a request to change the remote address.
 *          The call back looks for the host and then deletes it from the host hash only
 *          to put it there again with the new address.
 *
 * Input:   hAnnexE       - The annex E module handle
 *          hAppAnnexE    - The application handle of the annex E module (actually
 *                          our very own transGlobals structure).
 *          remoteAddress - The remote address whence the message arrived.
 *          pNewAddress   - the new address to set the host to.
 *
 * Output:  None.
 *
 **************************************************************************************/
annexEStatus RVCALLCONV annexEEvUseOtherAddress(
    IN  HANNEXE             hAnnexE,
    IN  HAPPANNEXE          hAppAnnexE,
    IN  cmTransportAddress* remoteAddress,
    IN  cmTransportAddress* pNewAddress,
    IN  int                 localAddressIndex
)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)hAppAnnexE;
    cmTransHost* host;
    annexEStatus res = annexEStatusBadParameter;
    cmTransportAddress locAddr;

    if (!hAnnexE)
        return annexEStatusBadParameter;

    if (!transGlobals)
        return annexEStatusBadParameter;

    annexEGetLocalAddress(hAnnexE, localAddressIndex, &locAddr);
    host = findHostInHash(transGlobals, remoteAddress, &locAddr, RV_TRUE, RV_TRUE);

    if (host != NULL)
    {
        RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
        hashDelete(transGlobals->hHashHosts, host->hashEntry);
        RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);
        res = annexEStatusNormal;
    }

    if (host)
        setRemoteAddress(host, pNewAddress);

    return res;
}

/**************************************************************************************
 * annexEEvWriteable
 *
 * Purpose: Callback for Annex E when resources are available again for sending, after
 *          receiving would block error on send.
 *          The call back looks for the host and then calls the regular event handler
 *          (of TPKT) with event liEvWrite.
 *
 * Input:   hAnnexE       - The annex E module handle
 *          hAppAnnexE    - The application handle of the annex E module (actually
 *                          our very own transGlobals structure).
 *          remoteAddress - The remote address whence the message arrived.
 *
 * Output:  None.
 *
 **************************************************************************************/
annexEStatus RVCALLCONV annexEEvWriteable(
    IN  HANNEXE             hAnnexE,
    IN  HAPPANNEXE          hAppAnnexE,
    IN  cmTransportAddress* remoteAddress,
    IN  int                 localAddressIndex
)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)hAppAnnexE;
    cmTransHost *host;
    cmTransportAddress locAddr;

    if (!hAnnexE)
        return annexEStatusBadParameter;

    if (!transGlobals)
        return annexEStatusBadParameter;

    annexEGetLocalAddress(hAnnexE, localAddressIndex, &locAddr);
    host = findHostInHash(transGlobals, remoteAddress, &locAddr, RV_TRUE, RV_TRUE);

    if (host != NULL)
        transQ931Handler(NULL, RvSelectWrite, 0, 0, host);

    return annexEStatusNormal;
}

#ifdef __cplusplus
}
#endif /* __cplusplus*/
