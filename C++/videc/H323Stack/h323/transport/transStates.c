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

#include "transStates.h"
#include "transportint.h"
#include "transutil.h"
#include "q931asn1.h"
#include "transnet.h"
#include "cmutils.h"
#include "emanag.h"
#include "prnutils.h"
#include "pvaltreeStackApi.h"
#include "psyntreeStackApi.h"
#include "msg.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus*/



/**************************************************************************************
 * setMultiplexedParams
 *
 * purpose: to set the multiplexed parameters into an outgoing message. The parameters
 *           are taken from the host parameters which are constatntly updated by incoming
 *           messages and user API calls.
 *
 * Input: transGlobals - The global data of the module.
 *        session      - the session from which the message is sent.
 *        host         - the host on which the message is to be sent.
 *        pvtNode      - the message into which the parameters are to be set.
 *        msgType      - which message we are dealing with.
 *
 ***************************************************************************************/
void setMultiplexedParams(cmTransGlobals *transGlobals,
                          cmTransSession *session,
                          cmTransHost *host,
                          int pvtNode,
                          int msgType)
{
    RvBool multipleCallsValue;
    RvBool maintainConnectionValue;
    RvPvtNodeId uuieNodeId;
    RvPstFieldId uuieFieldId;

    switch (msgType)
    {
        case cmQ931alerting:
        case cmQ931callProceeding:
        case cmQ931connect:
        case cmQ931setup:
        case cmQ931facility:
        case cmQ931progress:
        {

            /* we want to put theses parameters just to non-annex E messages */
            if (host->annexEUsageMode == cmTransNoAnnexE)
            {
                /* regular TPKT connection */
                multipleCallsValue = host->isMultiplexed;
                maintainConnectionValue =  !host->closeOnNoSessions;
            }
            else
            if ( (msgType == (int)cmQ931setup) && (session->Q931Connection != NULL) )
            {
                /* for SETUP message we want to put the parameters also when having both annex E host
                   and TPKT host, since we don't know who will win the race. We use the TPKT host params */
                multipleCallsValue = ((session->Q931Connection->isMultiplexed &&
                                       (session->Q931Connection->remoteIsMultiplexed || msgType == (int)cmQ931setup)));
                maintainConnectionValue =  ((!session->Q931Connection->closeOnNoSessions) &&
                                            (!session->Q931Connection->remoteCloseOnNoSessions));
            }
            else
            {
                /* annex E host */
                multipleCallsValue      = RV_FALSE;
                maintainConnectionValue = RV_TRUE;
            }

            __pvtGetByFieldIds(uuieNodeId, transGlobals->hPvt, pvtNode,
                                    { _q931(message)
                                      _anyField
                                      _q931(userUser)
                                      _q931(h323_UserInformation)
                                      _q931(h323_uu_pdu)
                                      _q931(h323_message_body)
                                      _anyField
                                      LAST_TOKEN
                                    },
                                    &uuieFieldId, NULL, NULL);

            /* An empty FACILITY doesn't have these fields, so we won't bother trying to build
               them and get a warning in the log on it */
            if (RV_PVT_NODEID_IS_VALID(uuieNodeId) && (uuieFieldId != __q931(empty)))
            {
                /* set the isMultiplexed element according to the host settings */
                pvtAdd(transGlobals->hPvt, uuieNodeId, __q931(multipleCalls),
                    multipleCallsValue, NULL, NULL);

                /* set the maintainConnection element according to the host settings */
                pvtAdd(transGlobals->hPvt, uuieNodeId, __q931(maintainConnection),
                    maintainConnectionValue, NULL, NULL);
            }
        }

        break;

        default:
            break;
    }
}

/**************************************************************************************
 * setTheFastStartStateByMessage
 *
 * purpose: to determine the faststart state of the session according to its previous
 *           state and the data within the outgoing message. This routine handles all
 *           outgoing messages (setup for outgoing calls and all the rest for incoming
 *           calls. The routine modifies the session state variables!
 *
 * Input: transGlobals      - The global data of the module.
 *        session           - the session on which the message is to be sent.
 *        messageBodyNode   - the UU-IE part of the message to be inspected.
 *        msgType           - The type of the message (setup or other)
 *
 ***************************************************************************************/
void setTheFastStartStateByMessage(cmTransGlobals *transGlobals,
                                   cmTransSession *session,
                                   RvPvtNodeId    messageBodyNode,
                                   int            msgType)
{
    int res;

    /* get the fastConnectRefuse element */
    __pvtGetNodeIdByFieldIds(res, transGlobals->hPvt, messageBodyNode,
        { _q931(h323_message_body) _anyField _q931(fastConnectRefused) LAST_TOKEN });

    if (res < 0)
    {
        /* there was NO refusal, check if there was a fastStart element in the message
           (and that it contains something) */
        __pvtGetNodeIdByFieldIds(res, transGlobals->hPvt, messageBodyNode,
            { _q931(h323_message_body) _anyField _q931(fastStart) _anyField LAST_TOKEN });

        if (msgType == (int)cmQ931setup)
        {
            if (res < 0)
                /* there was NO fastStart element in the message */
                /* for SETUP that means that no fast start on the call was even offered */
                session->faststartState = fsNo;
            else
                /* there WAS a fastStart element in the message */
                /* that means for SETUP that fast start was offered on the call */
                session->faststartState = fsOffered;
        }
        else
        {
            /* for all other messages */
            if (res < 0)
            {
                /* there was NO fastStart element in the message */
                /* this means that the fast start was not answered yet,
                   however, if we're already in the CONNECT message
                   and no response was given, that means a reject */
                if (msgType == (int)cmQ931connect)
                {
                    if ((session->faststartState == fsOffered) && (session->extFastConnectState == efcNo))
                        session->faststartState = fsRejected;
                }
            }
            else if (session->faststartState == fsOffered)
                /* there WAS a fastStart element in the message */
                /* this means that the fast start was approved */
                session->faststartState = fsApproved;
        }
    }
    else
    {
        /* There was a refusal, no fast start for this call */
        session->faststartState = fsRejected;
    }
}

/**************************************************************************************
 * setTheTunnelingStateByMessage
 *
 * purpose: to determine the tunneling state of the session according to its previous
 *           state and the data within the incoming message. This routine handles all
 *           incoming messages (setup for incoming calls and all the rest for outgoing
 *           calls. The routine modifies the session state variables!
 *
 * Input: transGlobals      - The global data of the module.
 *        session           - the session on which the message is to be sent.
 *        messageBodyNode   - the UU-IE part of the message to be inspected.
 *        msgType           - The type of the message (setup or other)
 *
 ***************************************************************************************/
void setTheTunnelingStateByMessage(cmTransGlobals *transGlobals,
                                   cmTransSession *session,
                                   int            messageBodyNode,
                                   int            msgType)
{
    int res;
    int bTunneling, bProvisional;
    RvBool tunnelingWasRejected = RV_FALSE;

    /* get the provisionalRespToH245Tunneling element */
    res = pvtGetChild(transGlobals->hPvt, messageBodyNode,
        __q931(provisionalRespToH245Tunneling), NULL);

    if (res < 0)
        bProvisional = RV_FALSE;
    else
        bProvisional = RV_TRUE;

    if (!bProvisional)
    {
        /* get the h245Tunneling element */
        res = pvtGetChildByFieldId(transGlobals->hPvt, messageBodyNode,
            __q931(h245Tunneling), (RvInt32 *)&bTunneling, NULL);

        if (res < 0)
            bTunneling = RV_FALSE;
    }
    else
        bTunneling = RV_FALSE;

    /* for incoming setup message: if tunneling exists in the message and the
       session supports it, approve the tunneling */
    if (msgType == (int)cmQ931setup)
    {
        if (m_sessionget(session, isTunnelingSupported))
        {
            if (bTunneling)
                session->tunnelingState = tunnApproved;
            else
                session->tunnelingState = tunnRejected;
        }
        else
            session->tunnelingState = tunnNo;
    }
    else
    {
        /* for all other messages (outgoing calls): if we offered tunneling and the message
           has it, approve it */
        if (!bProvisional)
        {
            if (bTunneling)
            {
                if (session->tunnelingState == tunnOffered)
                {
                    HRPOOLELEM nextMsg;
                    /* the element exists and is RV_TRUE */
                    session->tunnelingState = tunnApproved;

                    /* clean the saved but sent tunneled messages */
                    if (session->parallelTunnelingState != parllOffered)
                    {
                        nextMsg = session->firstMessage;
                        while ((nextMsg != NULL) && (session->sentButNotAckedTunneledMessages > 0))
                        {
                            nextMsg = extractMessageFromPool(transGlobals, session, RV_TRUE);
                            session->sentButNotAckedTunneledMessages--;
                        }
                    }
                }
            }
            else
            {
                /* the element exists and is RV_FALSE */
                if ((session->tunnelingState != tunnNo) && (session->tunnelingState != tunnRejected))
                {
                    tunnelingWasRejected = RV_TRUE;
                    session->tunnelingState = tunnRejected;
                }
            }
        }
    }

    /* if we reached a CONNECT with no response that means a reject */
    if ( (session->tunnelingState == tunnOffered) && (msgType == (int)cmQ931connect) )
    {
        tunnelingWasRejected = RV_TRUE;
        session->tunnelingState = tunnRejected;
    }

    if (tunnelingWasRejected)
    {
        /* raise a callback to let the CM know that the sent tunneled messages will not be ACKed soon */
        transGlobals->sessionEventHandlers.cmEvTransTunnelingWasRejected((HSTRANSSESSION)session,
            emaGetApplicationHandle((EMAElement*)session), RV_FALSE);
    }
}

/**************************************************************************************
 * setTheParallelTunnelingStateByMessage
 *
 * purpose: to determine the parallel tunneling state of the session according to its previous
 *          state and the faststart and tunneling states of the session.
 *          This routine handles all outgoing messages (setup for outgoing calls and all
 *          the rest for incoming calls. The routine modifies the session state variables!
 *
 * Input: transGlobals      - The global data of the module.
 *        session           - the session on which the message is to be sent.
 *        msgType           - The type of the message (setup or other)
 *        msgBody           - the node to the message UU-IE part
 *        outgoing          - RV_TRUE: an outgoing message; RV_FALSE - an incoming message.
 *
 ***************************************************************************************/
void setTheParallelTunnelingStateByMessage(cmTransGlobals *transGlobals,
                                           cmTransSession *session,
                                           int            msgType,
                                           int            msgBody,
                                           RvBool         outgoing)
{
    /* for SETUP check if we have both tunneling and faststart and we support
       the parallel mode. */
    if (msgType == (int)cmQ931setup)
    {
        if (outgoing)
        {
            if ((session->faststartState == fsOffered) &&
                ( (session->tunnelingState == tunnOffered) ||
                  (session->tunnelingState == tunnPossible) )&&
                (m_sessionget(session, isParallelTunnelingSupported))
               )
                session->parallelTunnelingState = parllOffered;
            else
                session->parallelTunnelingState = parllNo;
        }
        else
        {   /* incoming setup */
            int res = -1;

            /* check if parallel tunneling was offered */
            __pvtGetNodeIdByFieldIds(res, transGlobals->hPvt, msgBody,
                                        {   _q931(h323_message_body)
                                            _q931(setup)
                                            _q931(parallelH245Control)
                                            LAST_TOKEN
                                        });

            if ( m_sessionget(session, isParallelTunnelingSupported) && (res >= 0) )
                session->parallelTunnelingState = parllApproved;
        }
    }
}

/**************************************************************************************
 * needToOpenH245
 *
 * purpose: to determine according to the session params whether
 *           we shall want to open an H.245 (actually doing the connect).
 * Input: session           - the session on which the message is to be sent.
 *        forceOpen         - we must open (due to a startH245 facility).
 *
 ***************************************************************************************/
RvBool needToOpenH245(cmTransSession *session, RvBool forceOpen)
{
    RvBool needToOpen = RV_FALSE;

    {
        if (forceOpen)
            /* we were forced, so we obey */
            needToOpen = RV_TRUE;
        else
        {
            /* in case of no forcing, we need to check if we may open an
               H.245 connection according to the session's tunneling state
               and fast start state */

            switch (session->tunnelingState)
            {
                case tunnNo:
                case tunnRejected:
                {
                    if ( (session->faststartState == fsNo) ||
                         (session->faststartState == fsRejected) )
                        /* this the normal case (no tunneling, no faststart),
                           go ahead and try to open an H.245 connection */
                        needToOpen = RV_TRUE;
                    else
                    {
                        /* we don't have tunneling but we do have a fast start
                           on the session so only an explicit user request may
                           open a real H.245 connection, and even that only
                           when FS is stable */
                        if ( (m_sessionget(session, openControlWasAsked) || m_sessionget(session, switchToSeparateWasAsked)) &&
                            (session->faststartState != fsOffered) )
                            needToOpen = RV_TRUE;
                    }
                    break;
                }
                case tunnApproved:
                {
                    /* we have tunneling,
                       only user request for a separate connection may open */
                    if (m_sessionget(session, switchToSeparateWasAsked))
                        needToOpen = RV_TRUE;
                    break;
                }
                default:
                {
                    /* all other cases mean that we are still not in a stable tunneling state
                       so we shouldn't open H.245 yet */
                    needToOpen = RV_FALSE;
                    break;
                }
            }
        }
    }

    return needToOpen;
}
/**************************************************************************************
 * determineIfToOpenH245
 *
 * purpose: This routine checks if it's time to start a H.245 connection either by:
 *              - listenning if we have an H.245 address and it's time according to the
 *                H.245 stage to send that address. If so, the address is sent and the
 *                module starts listenning on that address.
 *              - trying to connect, if we have the remotes address.
 *
 * Input: outgoing          - RV_TRUE - if an outgoing message, RV_FALSE - if incoming one.
 *        transGlobals      - The global data of the module.
 *        session           - the session on which the message is to be sent.
 *        messageBodyNode   - the UU-IE part of the message to be inspected.
 *        msgType           - The type of the message (setup or other)
 *        manualRAS         - The RAS mode of the application
 *
 ***************************************************************************************/
RvBool determineIfToOpenH245(RvBool         outgoing,
                           cmTransGlobals *transGlobals,
                           cmTransSession *session,
                           int            messageBodyNode,
                           int            msgType)
{
    int     res;
    RvBool  forceOpen = RV_FALSE;
    RvBool  needToOpen;
    RvBool  earlyStage = RV_FALSE;

    /* Phase 1: Determine if the stage allows any H.245 operation */

    /* according to the message type and the H245 stage parameter
       determine whether we can do any operation now */
    switch (msgType)
    {
        case cmQ931setup:
        {
            if (session->h245Stage == cmTransH245Stage_setup)
                m_sessionset(session, openH245Stage, RV_TRUE);
            break;
        }

        case cmQ931callProceeding:
        {
            if ( (session->h245Stage == cmTransH245Stage_setup) ||
                 (session->h245Stage == cmTransH245Stage_callProceeding) )
                m_sessionset(session, openH245Stage, RV_TRUE);
            break;
        }
        case cmQ931alerting:
        {
            if ( (session->h245Stage == cmTransH245Stage_setup) ||
                 (session->h245Stage == cmTransH245Stage_callProceeding) ||
                 (session->h245Stage == cmTransH245Stage_alerting) )
                m_sessionset(session, openH245Stage, RV_TRUE);
            break;
        }
        case cmQ931connect:
        {
            if ( (session->h245Stage == cmTransH245Stage_setup) ||
                 (session->h245Stage == cmTransH245Stage_callProceeding) ||
                 (session->h245Stage == cmTransH245Stage_alerting) ||
                 (session->h245Stage == cmTransH245Stage_connect) )
                m_sessionset(session, openH245Stage, RV_TRUE);
            break;
        }

        case cmQ931releaseComplete:
        case cmQ931setupAck:
        case cmQ931information:
        case cmQ931status:
        case cmQ931statusInquiry:
        case cmQ931notify:
            /* these messages cannot hold an address, so we don't even try */
            return RV_TRUE;

        default:
            break;
    }

    /* if we are working in early H.245 mode, do it differently */
    if ( (session->h245Stage == cmTransH245Stage_early) &&
         ( (msgType == (int)cmQ931setup) || (msgType == (int)cmQ931connect) ) )
        earlyStage = RV_TRUE;

    /* Phase 2: Check for incoming startH245 FACILITY message */

    /* first check if we are forced to open H.245 by an incoming
       FACILITY with startH245 reason */

    if ( (!outgoing) && (msgType == (int)cmQ931facility) )
    {
        int node;

        /* get the reason */
        __pvtGetNodeIdByFieldIds(node, transGlobals->hPvt, messageBodyNode,
                                { _q931(h323_message_body)
                                  _q931(facility)
                                  _q931(reason)
                                  _q931(startH245)
                                  LAST_TOKEN
                                });
        if (node >= 0)
        {
            /* if we don't support H.245 we must reply with a FACILITY with noH245 */
            if (session->h245Stage == cmTransH245Stage_noH245)
            {
                int newMessage;

                /* we do not support H.245, report it with facility with NoH245 reason */
                if (createMessage(transGlobals,
                                  cmQ931facility,
                                  session->CRV,
                                  session->callId,
                                  &newMessage)
                   )
                {
                    /* set the reason */
                    TRANSERR err;
                    /* set reason */
                    __pvtBuildByFieldIds(node, transGlobals->hPvt, newMessage,
                                            { _q931(message)
                                              _q931(facility)
                                              _q931(userUser)
                                              _q931(h323_UserInformation)
                                              _q931(h323_uu_pdu)
                                              _q931(h323_message_body)
                                              _q931(facility)
                                              _q931(reason)
                                              _q931(noH245)
                                             LAST_TOKEN
                                            },
                                         0, NULL);

                    err = cmTransSendMessage((HSTRANSSESSION)session, newMessage, cmTransQ931Type);
                    pvtDelete(transGlobals->hPvt, newMessage);
                    if (err != cmTransOK)
                    {
                        RvLogError(&transGlobals->hLog,
                            (&transGlobals->hLog, "connectH245 failed to send facility (noH245) message"));
                        return RV_FALSE;
                    }
                }
                else
                {
                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog, "connectH245 failed to create facility (noH245)"));
                    return RV_FALSE;
                }

                return RV_TRUE;
            }
            else
            {
                m_sessionset(session, openH245Stage, RV_TRUE);
                forceOpen = RV_TRUE;
            }
        }
    }

    /* Phase 3: Start an H.245 connection if necessary, either as a listener or
                as a connector */


    /* if we're in the right stage we start the process of dealing with H.245 */
    /* check if we need to issue a connect on the H.245 */
    needToOpen = needToOpenH245(session, forceOpen);

    /* if the H245 host already exists, lock it */
    if (session->H245Connection)
        if (!emaMarkLocked((EMAElement)session->H245Connection))
            return RV_FALSE;

    /* if it's an outgoing message */
    if (outgoing)
    {
        if (m_sessionget(session, openH245Stage) || earlyStage)
        {
            int   addressNode;

            /* check if we are allowed to start H.245 */
            if (!transGlobals->sessionEventHandlers.cmEvTransWantsToStartControl((HSTRANSSESSION)session,
                emaGetApplicationHandle((EMAElement*)session)))
            {
                if (session->H245Connection != NULL)
                    emaReleaseLocked((EMAElement)session->H245Connection);
                return RV_TRUE;
            }

            /* check if extended fast connect procedures delay posting of the H.245 address */
            if ((session->extFastConnectState != efcNo) && (session->faststartState == fsOffered) &&
                (msgType != (int)cmQ931setup))
            {
                if (session->H245Connection != NULL)
                    emaReleaseLocked((EMAElement)session->H245Connection);
                return RV_TRUE;
            }

            /* if we need to open the connection and we do have the remote address
               to open to, just do it! */
            if (needToOpen)
            {
                if ( (session->H245Connection) &&
                     cmTaHasIp(&session->H245Connection->remoteAddress) )
                {
                    connectH245(transGlobals, session, (cmCallQ931MsgType)msgType);

                    /* check if the above message caused a startH245 facility message, if so send it */
                    if (!emaWasDeleted((EMAElement)session))
                        sendStartH245Facility(transGlobals, session);
                }
            } /*(needToOpen)*/

            /* if we don't have yet an H.245 host (and also no remote address)
               create such a host and start a listen on it */
            if (!session->H245Connection)
            {
                TRANSERR err;
                cmTransportAddress h245LocalAddr;

                /* get a local address for the listening host */
                memset(&h245LocalAddr, 0, sizeof(cmTransportAddress));
                getGoodAddress(transGlobals,
                               0,
                               session->Q931Connection,
                               cmTransH245Conn,
                               &h245LocalAddr);
                h245LocalAddr.port  = 0;

                /* this will allocate the H.245 host and allocate a listening port */
                err = cmTransSetAddress((HSTRANSSESSION)session,
                                         &h245LocalAddr,
                                         NULL,
                                         NULL,
                                         NULL,
                                         cmTransH245Conn,
                                         RV_TRUE);
                if (err != cmTransOK)
                {
                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog, "determineIfToOpenH245 failed to create a H.245 host"));
                    return RV_FALSE;
                }
                else
                /* we have a new host, lock it */
                if (!emaMarkLocked((EMAElement)session->H245Connection))
                    return RV_FALSE;
            } /*(!session->H245Connection)*/

            /* If the host is still closed and we don't have a remote address,
               that means that we need to start a listening process on it */
            if ( (session->H245Connection->state == hostIdle) &&
                 !cmTaHasIp(&session->H245Connection->remoteAddress) )
            {
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
                RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif
                TRANSERR    reply = cmTransOK;

                /* mark that the h245 host is in listening mode, so we won't enter again */
                session->H245Connection->state = hostListenning;

                /* Call the call back that we are about to start listening */
                if (transGlobals->hostEventHandlers.cmEvTransHostListen)
                {
                    HATRANSHOST haTransHost =
                                 (HATRANSHOST)emaGetApplicationHandle(((EMAElement)session->H245Connection));

                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmEvTransHostListen(hsHost = %d(%p), haHost=%p, type=cmTransH245Conn, address = (ip:%s,port:%u))",
                            emaGetIndex((EMAElement)session->H245Connection),session->H245Connection, haTransHost,
                            getIP(&session->H245Connection->localAddress, buff), session->H245Connection->localAddress.port));

                    reply = transGlobals->hostEventHandlers.cmEvTransHostListen(
                                            (HSTRANSHOST)session->H245Connection,
                                            haTransHost,
                                            cmTransH245Conn,
                                            &session->H245Connection->localAddress);
                    session->H245Connection->reported = RV_TRUE;
                }

                if (reply == cmTransOK)
                {
                    /* actual listening initiation */
                    session->H245Connection->h245Listen =
                            tpktOpen(transGlobals->tpktCntrl,
                                     &session->H245Connection->localAddress,
                                     RvH323ConnectionH245,
                                     tpktServer,
                                     (EMAElement)session->H245Connection,
                                     transH245AcceptHandler,
                                     (void *)session->H245Connection);
                }
                else
                {
                    session->H245Connection->state = hostIdle;
                    session->H245Connection->h245Listen = NULL;
                }

                /* get the allocated full local address (IP and port) */
                getGoodAddress(transGlobals,
                               session->H245Connection->h245Listen,
                               session->Q931Connection,
                               cmTransH245Conn,
                               &session->H245Connection->localAddress);

                /* call the call back that listening has initiated */
                if (transGlobals->hostEventHandlers.cmEvTransHostListening)
                {
                    HATRANSHOST haTransHost =
                                 (HATRANSHOST)emaGetApplicationHandle(((EMAElement)session->H245Connection));

                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmEvTransHostListening(hsHost = %d(%p), haHost=%p, type=cmTransH245Conn, address = (ip:%s,port:%u) error = %d)",
                            emaGetIndex((EMAElement)session->H245Connection),session->H245Connection, haTransHost,
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

                /* in case of failure or user abortion report to log,
                   upon real error exit */
                if (reply == cmTransIgnoreMessage)
                {
                    session->H245Connection->state = hostIdle;
                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog, "determineIfToOpenH245 listen on H.245 TPKT refused by user"));
                }
                else if (session->H245Connection->h245Listen == NULL)
                {
                    session->H245Connection->state = hostIdle;
                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog, "determineIfToOpenH245 failed to initiate listen on H.245 TPKT"));
                    emaReleaseLocked((EMAElement)session->H245Connection);
                    return RV_FALSE;
                }
                else
                {
                    RvBool nat = RV_FALSE;

                    /*set localAddressInMsg properly*/
                    session->H245Connection->localAddressInMsg = session->H245Connection->localAddress;

                    if ((cmTransGetSessionParam((HSTRANSSESSION)session, cmTransParam_callSupportNat, (RvBool*)(&nat)) == cmTransOK) &&
                        (nat) && (transGlobals->sessionEventHandlers.cmEvTransNatAddressTranslation != NULL))
                    {
                        HATRANSSESSION haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

                        transGlobals->sessionEventHandlers.cmEvTransNatAddressTranslation(
                            (HSTRANSSESSION) session, haTransSession, cmProtocolH245, &(session->H245Connection->localAddressInMsg));
                    }
                }
            } /*((hostIdle) && (cmTaHasIp(remoteAddress) == 0))*/

            /* for listening hosts we need to insert the address to the outgoing
               message so do it */
            if ( ((session->H245Connection->state == hostListenning) ||
                 (session->H245Connection->state == hostListenningConnecting)) &&
                 (msgType != (int)cmQ931releaseComplete) )
            {
                RvPvtNodeId uuieNodeId;
                RvPstFieldId uuieFieldId;
                __pvtGetByFieldIds(uuieNodeId, transGlobals->hPvt, messageBodyNode,
                    { _q931(h323_message_body) _anyField LAST_TOKEN }, &uuieFieldId, NULL, NULL);

                /* An empty FACILITY doesn't have these fields, so we won't bother trying to build
                   them and get a warning in the log on it */
                if (RV_PVT_NODEID_IS_VALID(uuieNodeId) && (uuieFieldId == __q931(empty)))
                {
                    emaReleaseLocked((EMAElement)session->H245Connection);
                    return RV_TRUE;
                }

                addressNode = pvtAddBranch(transGlobals->hPvt, uuieNodeId, __q931(h245Address));

                res = -1;
                if (addressNode >= 0)
                    res = cmTAToVt(transGlobals->hPvt,
                                   addressNode,
                                   &session->H245Connection->localAddressInMsg);
                if (res < 0)
                {
                    /* if we can't transfer the address to the other side there
                       is no much use in leaving the poor connection listening,
                       so kill it */
                    if (session->H245Connection->h245Listen)
                    {
                        tpktClose(session->H245Connection->h245Listen);
                        session->H245Connection->h245Listen = NULL;
                    }
                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog, "determineIfToOpenH245 failed on building h245Address"));
                    emaReleaseLocked((EMAElement)session->H245Connection);
                    return RV_FALSE;
                }
            } /*(((hostListenning) || (hostListenningConnecting)) && (msgType != cmQ931releaseComplete))*/
        } /*(m_sessionget(session, openH245Stage) || earlyStage)*/
    }
    else /* handle incoming messages */
    {
        int  addressNode = -1;
        RvBool ok = RV_FALSE;

        /* if we don't have the remote H.245 address yet let's try and get one */
        if ( (!session->H245Connection) ||
             ( (session->H245Connection) && !cmTaHasIp(&session->H245Connection->remoteAddress) ) )
        {
            /* check if the incoming message has H245 address */
            __pvtGetNodeIdByFieldIds(addressNode, transGlobals->hPvt, messageBodyNode,
                { _q931(h323_message_body) _anyField _q931(h245Address) LAST_TOKEN });

            /* if we got a remote address (or if it's a CONNECT message, which
               means that we won't have another chance to get the address) try
               to connect the H.245 to it */
            if ((addressNode >= 0) ||
                ((msgType == (int)cmQ931connect) && (m_sessionget(session, openH245Stage) || earlyStage)))
            {
                /* we have the remote address, do we have a H245 host at all? */
                if (!session->H245Connection)
                {
                    /* just create a non-listening host */
                    res = (int)createHostByType(transGlobals,
                        (HSTRANSSESSION)session, (HSTRANSHOST *)&session->H245Connection,
                        cmTransH245Conn, NULL, cmTransNoAnnexE, 0);

                    if (res != (int)cmTransOK)
                    {
                        RvLogError(&transGlobals->hLog,
                            (&transGlobals->hLog, "determineIfToOpenH245 failed on allocating host element"));
                        return RV_FALSE;
                    }
                    else
                    {
                        /* create h245 local host address */
                        cmTransportAddress h245LocalAddr;

                        memset(&h245LocalAddr, 0, sizeof(cmTransportAddress));
                        getGoodAddress(transGlobals, 0, session->Q931Connection,
                            cmTransH245Conn, &h245LocalAddr);
                        h245LocalAddr.port  = 0;

                        session->H245Connection->localAddress = h245LocalAddr;
                    }

                   /* this is a new host, lock it */
                   if (!emaMarkLocked((EMAElement)session->H245Connection))
                        return RV_FALSE;
                } /*(!session->H245Connection)*/

                /* Now we surly have a host, if we have a remote address,
                   put the address into the host */
                if (addressNode >= 0)
                    cmVtToTA(transGlobals->hPvt, addressNode, &session->H245Connection->remoteAddress);

                /* if we receive a start H.245 message while already listening, we should simply do both! */

                /* check if we are in the right stage to open H.245 (we have a new host and a remote address) */
                ok = (m_sessionget(session, openH245Stage) || earlyStage);
            } /*((addressNode >= 0) || (msgType == cmQ931connect))*/
            else
            {
                /* we didn't manage to get an address but it's not CONNECT yet so maybe in the next message */
                ok = RV_FALSE;
            }
        } /*((!H245Connection) || ((H245Connection) && !cmTaHasIp(remoteAddress)))*/
        else
        {
            /* check if we are in the right stage to open H.245 (we have a new host and a remote address) */
            ok = (m_sessionget(session, openH245Stage) || earlyStage);
        }

        /* if we need to open an H.245 connection and can do it, just do it, except for setup, in which we
        delay until after ACF */
        if (needToOpen && ok &&
            transGlobals->sessionEventHandlers.cmEvTransWantsToStartControl((HSTRANSSESSION)session,
                emaGetApplicationHandle((EMAElement*)session)))
        {
            /* if we put a remote address into the host the connection will
               happen, if we didn't, i.e. this is a CONNECT message and we don't
               have a remote address yet, it will initiate a facility with startH245 */
            connectH245(transGlobals, session, (cmCallQ931MsgType)msgType);

            /* check if the above message caused a startH245 facility message, if so send it */
            if (!emaWasDeleted((EMAElement)session))
                sendStartH245Facility(transGlobals, session);
        }
    } /*else(outgoing)*/

    /* unlock the h245 host */
    emaReleaseLocked((EMAElement)session->H245Connection);

    return RV_TRUE;
}


/**************************************************************************************
 * reportH245
 *
 * purpose: This routine checks if it is necessary to report to the user that an H.245 is opened.
 *           It bases its decision accrding to the state of the session in regard to tunneling
 *           and parallel tunneling and to the faststart state of the session as well.
 *           If the connection was already reported, we eliminate the notification.
 *           The routine handles both incoming and outgoing messages.
 *
 * Input: outgoing          - RV_TRUE - if an outgoing message, RV_FALSE - if incoming one.
 *        transGlobals      - The global data of the module.
 *        session           - the session on which the message is to be sent.
 *        messageBodyNode   - The body of the message (incoming or outgoing)
 *        msgType           - The type of the message (setup or other)
 *
 ***************************************************************************************/
void reportH245(RvBool          outgoing,
                cmTransGlobals  *transGlobals,
                cmTransSession  *session,
                int             messageBodyNode,
                int             msgType)
{
    RvBool needToReport = RV_FALSE;

    if (outgoing)
    {
        /* handling outgoing messages */
        if (msgType == (int)cmQ931setup)
        {
            /* in case of outgoing setup (outgoing call), we need to tell
               the user of a H.245 connection if we have tunneling without
               faststart, or in case of parallel tunneling, so the user
               may send H.245 tunneled messages before the setup is sent */
            if (session->parallelTunnelingState == parllOffered)
                needToReport = RV_TRUE;
            else
            switch (session->tunnelingState)
            {
                case tunnPossible:
                case tunnOffered:
                case tunnApproved:
                {
                    if (session->faststartState == fsNo)
                        needToReport = RV_TRUE;
                }
                break;

                default:
                    break;
            }
        }
        else
        {
            /* all other outgoing messages (incoming calls) */

            /* if we got some response for our faststart offering and tunneling
               was accepted by the remote we need to check if there is a need to
               report the H.245.
               in case of parallel we may notify the user. If not, that means
               that it's a 'simple' tunneling after faststart and we need to report
               only if the user explicitly requested to open a control session or
               if the faststart was refused */
            if ( (
                   (session->faststartState == fsApproved) ||
                   (session->faststartState == fsRejected)
                 )
                 &&
                 (session->tunnelingState == tunnApproved)
               )
            {
                if (session->parallelTunnelingState == parllApproved)
                    needToReport = RV_TRUE;
                else
                    if ( (session->faststartState == fsRejected) || m_sessionget(session, openControlWasAsked) )
                        needToReport = RV_TRUE;
            }
        }
    }
    else
    {
        /* incoming messages */
        if (msgType == (int)cmQ931setup)
        {
            /* in case of incoming SETUP message (new incoming call)
               we need to report H.245 in case that parallel was offered */
            if (session->parallelTunnelingState == parllApproved)
            {
                needToReport = RV_TRUE;
            }
            else
            /* in case that no parallel was offered BUT we have a 'normal'
               tunneling WITHOUT faststart we can report that H.245 too */
            if ( (session->tunnelingState == tunnApproved) &&
                (session->faststartState != fsOffered) &&
                (session->faststartState != fsApproved) )
            {
                needToReport = RV_TRUE;
            }
        }
        else
        {
            int tunnMsgNode;

            /* check if there are normal tunneled messages in the message */
            tunnMsgNode = pvtGetChild(transGlobals->hPvt, messageBodyNode, __q931(h245Control), NULL);

            /* all other incoming messages (outgoing calls) */
            /* if the tunneling was approved and no fast start
               or the other side started sending tunneling messages,
               we need to open the H.245 */
            if (session->tunnelingState == tunnApproved)
            {
                if ((session->faststartState == fsRejected) ||
                    (session->faststartState == fsNo)       ||
                    (tunnMsgNode >= 0)
                   )
                needToReport = RV_TRUE;
            }
        }
    }

    /* if we need to report the H.245 connection and we haven't done so yet */
    if (needToReport)
        reportH245Establishment(transGlobals, session);
}

/**************************************************************************************
 * insertH245TunnelingFlag
 *
 * purpose: This routine fills the H.245 tunneling flag 'h245Tunneling' into the outgoing Q.931 message.
 *
 * Input: transGlobals      - The global data of the module.
 *        session           - the session on which the message is to be sent.
 *        msgBodyNode       - The messages UU-IE part.
 *        msgType           - The message type into which the tunneled messages are inserted
 *
 ***************************************************************************************/
void insertH245TunnelingFlag(cmTransGlobals  *transGlobals,
                             cmTransSession  *session,
                             int             messageBodyNode,
                             int             msgType)
{
    /* set tunneling flag only if we are in a good tunneling state,
       i.e. it's allowed and not rejected yet, and we never offered it yet,
       and we didn't ask to connect a separate H.245 connection yet */
    if ( (session->tunnelingState != tunnNo) &&
         (session->tunnelingState != tunnRejected) &&
         (session->tunnelingState != tunnOffered) &&
         !m_sessionget(session, switchToSeparateWasAsked) )
    {
        /* mark the message as having tunneled messages */
        pvtAdd(transGlobals->hPvt, messageBodyNode, __q931(h245Tunneling), RV_TRUE, NULL, NULL);

        /* for the following messages, do not insert tunneled messages but
           mark as provisional, so that the other side will not know yet whether
           we approved the tunneling (just like previous version are when receiving
           these messages) */
        if ( (msgType == (int)cmQ931setupAck) ||
             (msgType == (int)cmQ931information) ||
             ((msgType == (int)cmQ931progress) && (session->parallelTunnelingState != parllApproved)) ||
             (msgType == (int)cmQ931status) ||
             (msgType == (int)cmQ931statusInquiry)
           )
        {
            /* set the provisionalRespToH245Tunneling element */
            pvtAdd(transGlobals->hPvt, messageBodyNode, __q931(provisionalRespToH245Tunneling), 0, NULL, NULL);
            return;
        }

        /* after sending for the first time, we must wait to see if the other
           side supports tunneling, until it does we may not continue using tunneling  */
        if (session->tunnelingState == tunnPossible)
            session->tunnelingState = tunnOffered;
    }
    else
    {
        RvBool useTunneling;

        if( ((session->tunnelingState == tunnOffered) ||
             ((msgType == (int)cmQ931setup) && (session->tunnelingState == tunnNo) && m_sessionget(session, isTunnelingSupported))) &&
            !m_sessionget(session, switchToSeparateWasAsked) )
        {
            /* mark the message as supporting tunneled messages (we are either in tunnOffered
               state that was not answered yet, or supporting tunneling without having processed
               any messages yet. */
            useTunneling = RV_TRUE;
        }
        else
        {
            /* mark the message as not supporting tunneled messages */
            useTunneling = RV_FALSE;
        }

        pvtAdd(transGlobals->hPvt, messageBodyNode, __q931(h245Tunneling), useTunneling, NULL, NULL);
    }
}

/**************************************************************************************
 * insertH245TunneledMessages
 *
 * purpose: This routine fills the H.245 tunneled messages into the outgoing Q.931 message.
 *
 * Input: transGlobals      - The global data of the module.
 *        session           - the session on which the message is to be sent.
 *        msgBodyNode       - The messages UU-IE part.
 *        msgType           - The message type into which the tunneled messages are inserted
 *
 ***************************************************************************************/
void insertH245TunneledMessages(cmTransGlobals  *transGlobals,
                                cmTransSession  *session,
                                int             messageBodyNode,
                                int             msgType)
{
    HRPOOLELEM nextMsg = session->firstMessage;
    int  node;
    RvBool goodEfcStage;

    /* See if we have a cool message to send tunneling on */
    if ((msgType == (int)cmQ931setupAck) ||
        (msgType == (int)cmQ931information) ||
        ((msgType == (int)cmQ931progress) && (session->parallelTunnelingState != parllApproved)) ||
        (msgType == (int)cmQ931status) ||
        (msgType == (int)cmQ931statusInquiry))
    {
        /* the above messages suck. No tunneling for you! */
        return;
    }

    /* make sure the EFC state allows sending of tunneled messages */
    goodEfcStage = (
        /* if no EFC at all, ok */
        (session->extFastConnectState == efcNo) ||
        /* if there is EFC, but parallel control, ok */
        (session->parallelTunnelingState != parllNo) || 
        /* EFC, no parallel control, but the initial offer is over, and tunneling is supported */
        ( (session->faststartState != fsOffered) && (session->tunnelingState == tunnApproved) ) );

    /* send tunneled messages only if we are in a good tunneling state,
       i.e. it's allowed and not rejected yet, and not offered (unless we
       were offering it in this very message which may be only first outgoing
       setup message */
    if ( (session->tunnelingState != tunnNo) && (session->tunnelingState != tunnRejected) &&
         ((session->tunnelingState != tunnOffered) || (msgType == (int)cmQ931setup)) &&
         goodEfcStage && !m_sessionget(session, switchToSeparateWasAsked) )
    {
        /* go over all saved tunneled messages and put them into the Q.931 message */
        while (nextMsg)
        {
            RvUint8 *buffer;
            int  msgSize;

            /* read the next tunneled message */
            getEncodeDecodeBuffer(transGlobals->codingBufferSize, &buffer);

            rpoolCopyToExternal(transGlobals->messagesRPool,
                                (void *)buffer,
                                nextMsg,
                                0,
                                transGlobals->codingBufferSize);

            msgSize = rpoolChunkSize(transGlobals->messagesRPool, nextMsg)
                                     -MSG_HEADER_SIZE-TPKT_HEADER_SIZE;

            /* if it was not sent yet, insert it to the Q.931 message */
            node = 0;

            if ( !m_sessionget(session,parallelBeforeFS) &&
                 ( (m_sessionget(session,outgoing) && (session->parallelTunnelingState == parllOffered) && (msgType != cmQ931setup)) ||
                   (!m_sessionget(session,outgoing) && (session->parallelTunnelingState == parllApproved)) ) && 
                 (session->faststartState == fsOffered) )
            {
                /* hopefully, this will cover the case where we work in parallel mode, but did not
                ack or receive an ack for the FS channels yet (unless it's a setup message). In
                such a case, we should delay the sending of tunneled H.245 messages */
                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog, "insertH245TunneledMessages delaying parallel tunneling untill FS ack"));
                return;
            }

            /* for parallel tunneling we insert it in a different place in the
               setup message than normal */
            if ( (session->parallelTunnelingState == parllOffered) && (msgType == (int)cmQ931setup) )
            {

                __pvtBuildByFieldIds(node,
                                     transGlobals->hPvt,
                                     messageBodyNode,
                                    {   _q931(h323_message_body)
                                        _q931(setup)
                                        _q931(parallelH245Control)
                                        _nul(0)
                                        LAST_TOKEN
                                    },
                                    msgSize,
                                    (char *)&buffer[MSG_HEADER_SIZE + TPKT_HEADER_SIZE]);
            }
            else
            {
                __pvtBuildByFieldIds(node,
                                     transGlobals->hPvt,
                                     messageBodyNode,
                                    {   _q931(h245Control)
                                        _nul(0)
                                        LAST_TOKEN
                                    },
                                    msgSize,
                                    (char *)&buffer[MSG_HEADER_SIZE + TPKT_HEADER_SIZE]);
            }

            if (node < 0)
            {
                nextMsg = NULL;
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog, "insertH245TunneledMessages failed on inserting tunneled message (%d)",node));
            }
            else
            {
                /* if tunneling is established we need not keep this messgae */
                if (session->tunnelingState == tunnApproved)
                    nextMsg = extractMessageFromPool(transGlobals, session, RV_TRUE);
                else
                {
                    /* get the next message */
                    memcpy((void*)&nextMsg, &buffer[0], MSG_HEADER_SIZE);
                    session->sentButNotAckedTunneledMessages++;
                }
            }
        }

        /* after sending for the first time, we must wait to see if the other
           side supports tunneling */
        if (session->tunnelingState == tunnPossible)
            session->tunnelingState = tunnOffered;
    }
}

/**************************************************************************************
 * insertH450TunneledMessages
 *
 * purpose: This routine fills the H.450 tunneled messages into the outgoing Q.931 message.
 *
 * Input: transGlobals      - The global data of the module.
 *        session           - the session on which the message is to be sent.
 *        msgBodyNode       - The messages UU-IE part.
 *        msgType           - The message type into which the tunneled messages are inserted
 *
 ***************************************************************************************/
void insertH450TunneledMessages(cmTransGlobals  *transGlobals,
                                cmTransSession  *session,
                                int             messageBodyNode,
                                int             msgType)
{
    int nodeId;
    RvBool isAlert = ((msgType == (int)cmQ931alerting) || (msgType == (int)cmQ931connect));

    if (RV_PVT_NODEID_IS_VALID(session->h450Element) ||
        (isAlert && RV_PVT_NODEID_IS_VALID(session->h450AlertElement)))
    {
        nodeId = pvtAdd(transGlobals->hPvt, messageBodyNode, __q931(h4501SupplementaryService), 0, NULL, NULL);

        if(!RV_PVT_NODEID_IS_VALID(nodeId))
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "insertH450TunneledMessages failed on inserting tunneled message (session=%d-%p)",
                emaGetIndex((EMAElement)session), session));
            return;
        }
    }
    else
        return;

    if (RV_PVT_NODEID_IS_VALID(session->h450Element))
    {
        pvtMoveTree(transGlobals->hPvt,nodeId, session->h450Element);
        session->h450Element = RV_PVT_INVALID_NODEID;
    }

    if (isAlert && RV_PVT_NODEID_IS_VALID(session->h450AlertElement))
    {
        pvtAddChilds(transGlobals->hPvt, nodeId, transGlobals->hPvt, session->h450AlertElement);
        pvtDelete(transGlobals->hPvt, session->h450AlertElement);
        session->h450AlertElement = RV_PVT_INVALID_NODEID;
    }
}


/**************************************************************************************
 * insertAnnexLTunneledMessages
 *
 * purpose: This routine fills the annex L tunneled messages into the outgoing Q.931 message.
 *
 * Input: transGlobals      - The global data of the module.
 *        session           - the session on which the message is to be sent.
 *        msgBodyNode       - The messages UU-IE part.
 *
 ***************************************************************************************/
void insertAnnexLTunneledMessages(cmTransGlobals    *transGlobals,
                                  cmTransSession    *session,
                                  int               messageBodyNode)
{
    int nodeId;

    if (session->annexLElement >= 0)
    {
        nodeId = pvtAdd(transGlobals->hPvt, messageBodyNode, __q931(stimulusControl), 0, NULL, NULL);

        if (nodeId>=0)
        {
            pvtMoveTree(transGlobals->hPvt,nodeId, session->annexLElement);
            session->annexLElement = -1;
        }
        else
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "insertAnnexLTunneledMessages failed on inserting tunneled message (session=%d-%p)",
                    emaGetIndex((EMAElement)session), session));
        }
    }
}


/**************************************************************************************
 * insertAnnexMTunneledMessages
 *
 * purpose: This routine fills the annex M tunneled messages into the outgoing Q.931 message.
 *
 * Input: transGlobals      - The global data of the module.
 *        session           - the session on which the message is to be sent.
 *        msgBodyNode       - The messages UU-IE part.
 *
 ***************************************************************************************/
void insertAnnexMTunneledMessages(cmTransGlobals    *transGlobals,
                                  cmTransSession    *session,
                                  int               messageBodyNode)
{
    int nodeId;

    if (session->annexMElement >= 0)
    {
        nodeId = pvtAdd(transGlobals->hPvt, messageBodyNode, __q931(tunnelledSignallingMessage), 0, NULL, NULL);

        if (nodeId>=0)
        {
            pvtMoveTree(transGlobals->hPvt,nodeId, session->annexMElement);
            session->annexMElement = -1;
        }
        else
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "insertAnnexMTunneledMessages failed on inserting tunneled message (session=%d-%p)",
                    emaGetIndex((EMAElement)session), session));
        }
    }
}


/**************************************************************************************
 * getMultiplexedParams
 *
 * purpose: to get the multiplexed parameters from an incoming message. The parameters
 *           are taken from the message and set into the host parameters which are constatntly
 *           updated by the incoming messages and user API calls.
 *
 * Input: transGlobals - The global data of the module.
 *        host         - the host on which the message was received.
 *        pvtNode      - the message from which the parameters are to be get.
 *        msgType      - The type of the message.
 ***************************************************************************************/
void getMultiplexedParams(cmTransGlobals *transGlobals, cmTransHost *host, int pvtNode, int msgType)
{
    RvInt32 value;
    RvBool supportsMultiplexing = RV_FALSE;
    RvBool oldSupportsMultiplexing = RV_FALSE;
    RvPvtNodeId uuieNodeId;
    RvPstFieldId uuieFieldId;

    switch (msgType)
    {
        case cmQ931alerting:
        case cmQ931callProceeding:
        case cmQ931connect:
        case cmQ931setup:
        case cmQ931facility:
        case cmQ931progress:
        {
            if (host->annexEUsageMode == cmTransNoAnnexE)
            {
                /* we are interested in these parameters just for none annex E connections */
                if ((host->isMultiplexed) && (host->remoteIsMultiplexed))
                     oldSupportsMultiplexing = RV_TRUE;

                /* look for the UUIE node ID first */
                __pvtGetByFieldIds(uuieNodeId, transGlobals->hPvt, pvtNode,
                                        { _q931(message)
                                          _anyField
                                          _q931(userUser)
                                          _q931(h323_UserInformation)
                                          _q931(h323_uu_pdu)
                                          _q931(h323_message_body)
                                          _anyField
                                          LAST_TOKEN
                                        },
                                        &uuieFieldId, NULL, NULL);

                if (uuieFieldId == __q931(empty))
                {
                    /* ignore empty facility */
                    break;
                }

                host->remoteIsMultiplexed = RV_FALSE;
                host->remoteCloseOnNoSessions = RV_TRUE;
                if (RV_PVT_NODEID_IS_VALID(uuieNodeId))
                {
                    /* get the isMultiplexed element */
                    if (RV_PVT_NODEID_IS_VALID(pvtGetChildByFieldId(transGlobals->hPvt, uuieNodeId, __q931(multipleCalls), &value, NULL)))
                        host->remoteIsMultiplexed = (value != 0);

                    /* get the maintainConnection element */
                    if (RV_PVT_NODEID_IS_VALID(pvtGetChildByFieldId(transGlobals->hPvt, uuieNodeId, __q931(maintainConnection), &value, NULL)))
                        host->remoteCloseOnNoSessions = (value == 0);
                }

                if ((host->isMultiplexed) && (host->remoteIsMultiplexed))
                     supportsMultiplexing = RV_TRUE;

                /* report if the multiplex state has changed */
                if (oldSupportsMultiplexing != supportsMultiplexing)
                {
                    HATRANSHOST haTransHost =
                                      (HATRANSHOST)emaGetApplicationHandle((EMAElement)host);

                    if (transGlobals->hostEventHandlers.cmEvTransHostMultiplexChangeState)
                    {
                        RvLogInfo(&transGlobals->hLog,
                            (&transGlobals->hLog, "cmEvTransHostMultiplexChangeState(hsHost = %d(%p), haHost=%p, isMultiplex=%d",
                                emaGetIndex((EMAElement)host), host, haTransHost,supportsMultiplexing));

                        (transGlobals->hostEventHandlers.cmEvTransHostMultiplexChangeState)(
                                                            (HSTRANSHOST) host,
                                                            haTransHost,
                                                            supportsMultiplexing);
                    }
                }
            }
        }

        break;

        default:
            break;
    }
}

/**************************************************************************************
 * extractH245Messages
 *
 * purpose: extracts, decodes and reports the tunneled H.245 messages from the given message.
 *
 * Input: transGlobals      - The global data of the module.
 *        session           - the session on which the message was received.
 *        host              - the host on which the message was received.
 *        messageBodyNode   - the UU-IE part of the message.
 *        msgType           - The type of the message from which we extract the tunneled msgs.
 ***************************************************************************************/
void extractH245Messages(cmTransGlobals *transGlobals,
                         cmTransSession *session,
                         cmTransHost    *host,
                         int            messageBodyNode,
                         int            msgType)
{
    HATRANSSESSION haTransSession;
    int     encodedOctedStrings;
    int     nodeId;
    RvUint8 *buffer;
    int     msgLen;
    int     pvtNode;

    /* first verify that we are interested in the tunneled messages */
    if ( (session->tunnelingState == tunnRejected) || (session->tunnelingState == tunnNo) )
         return;

    haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

    /* position yourself on the octed strings of the tunneled messages according to
       the place where they are stored in the message. (in SETUP with parallel offering
       it's a different place than the regular tunneling place). */
    if ( (session->parallelTunnelingState == parllApproved) && (msgType == (int)cmQ931setup) )
    {
        __pvtGetNodeIdByFieldIds(encodedOctedStrings, transGlobals->hPvt, messageBodyNode,
                                { _q931(h323_message_body)
                                  _q931(setup)
                                  _q931(parallelH245Control)
                                  LAST_TOKEN });
    }
    else
    {
        encodedOctedStrings = pvtGetChild(transGlobals->hPvt, messageBodyNode, __q931(h245Control), NULL);
    }

    /* loop over all the tunneled messages */
    nodeId = pvtChild(transGlobals->hPvt, encodedOctedStrings);
    while ( (RV_PVT_NODEID_IS_VALID(nodeId)) && (!emaWasDeleted((EMAElement)session)) )
    {
        /* get the coding buffer for decoding */
        getEncodeDecodeBuffer(transGlobals->codingBufferSize, &buffer);

        /* read the encoded message from the PVT into the buffer */
        msgLen = pvtGetString(transGlobals->hPvt, nodeId, transGlobals->codingBufferSize, (char *)buffer);
        if (msgLen > 0)
        {
            int decoded_length = 0;

            /* build an H.245 message tree and decode the message into it */
            pvtNode = pvtAddRoot(transGlobals->hPvt, transGlobals->synProtH245, 0, NULL);

            decoded_length = cmEmDecode(transGlobals->hPvt, pvtNode, buffer, msgLen, &decoded_length);

            if (decoded_length > 0)
            {
#if (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG))
                RvPstFieldId fieldId;
                RvPvtNodeId tmpNode;
                char *ret;

                tmpNode = pvtChild(transGlobals->hPvt, pvtNode);
                tmpNode = pvtChild(transGlobals->hPvt, tmpNode);

                pvtGet(transGlobals->hPvt,
                       tmpNode,
                       &fieldId,
                       NULL, NULL, NULL);
                ret = pstGetFieldNamePtr(transGlobals->synProtH245, fieldId);

                RvLogDebug(&transGlobals->hTPKTWIRE,
                    (&transGlobals->hTPKTWIRE, "New TUNNELED message (call %d-%p) recv <-- %s:",
                        emaGetIndex((EMAElement)haTransSession),haTransSession,ret));
                pvtPrintInternal(transGlobals->hPvt, pvtNode, &transGlobals->hTPKTWIRE, msGetDebugLevel());
                printHexBuff(buffer, msgLen, &transGlobals->hTPKTWIRE);
#endif /* (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG)) */

                /* check if parallel tunneling was approved */
                if ( m_sessionget(session, isParallelTunnelingSupported) &&
                     (session->parallelTunnelingState == parllOffered) &&
                     (msgType != (int)cmQ931setup) &&
                     !m_sessionget(session, notFirstTunnelingMessage) )
                {
                    /* check if it's TCS Ack message */
                    int child=pvtChild(transGlobals->hPvt,pvtNode);
                    int grandchild=pvtChild(transGlobals->hPvt,child);
                    int type=pvtGetSyntaxIndex(transGlobals->hPvt,child);
                    int messageId=pvtGetSyntaxIndex(transGlobals->hPvt,grandchild);
                    int h245Response = 2;
                    int h245terminalCapabilitySetAck = 4;

                    if ( (type == h245Response) && (messageId == h245terminalCapabilitySetAck) )
                    {
                        HRPOOLELEM nextMsg;

                        session->parallelTunnelingState = parllApproved;

                        /* clean the saved but sent tunneled messages */
                        nextMsg = session->firstMessage;
                        while ((nextMsg != NULL) && (session->sentButNotAckedTunneledMessages > 0))
                        {
                            nextMsg = extractMessageFromPool(transGlobals, session, RV_TRUE);
                            session->sentButNotAckedTunneledMessages--;
                        }
                    }
                    else
                    {
                        /* close, but no cigar. other side did not understand parallel */
                        session->parallelTunnelingState = parllNo;
                    }
                }

                /* at any rate we have passed the first tunneled response for the session */
                m_sessionset(session, notFirstTunnelingMessage, RV_TRUE);


                /* if we succeeded, report the message on the session to he user */
                if (transGlobals->sessionEventHandlers.cmEvTransNewMessage)
                {
                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmEvTransNewMessage(hsSession = %d(%p), haSession=%p, type=cmTransH245Type, pvtNode=%d)",
                            emaGetIndex((EMAElement)session), session, haTransSession, pvtNode));

                    (transGlobals->sessionEventHandlers.cmEvTransNewMessage)(
                                                    (HSTRANSSESSION) session,
                                                    haTransSession,
                                                    RvH323ConnectionH245,
                                                    pvtNode,
                                                    NULL,
                                                    RV_TRUE,
                                                    emaGetIndex((EMAElement)host));
                }
            }
            else
            {
                /* can't decode, report it to the user on the session */
                if (transGlobals->sessionEventHandlers.cmEvTransBadMessage)
                {
                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmEvTransBadMessage(hsSession = %d(%p), haSession=%p, type=cmTransH245Type, outgoing=FALSE)",
                            emaGetIndex((EMAElement)session), session, haTransSession));

                    transGlobals->sessionEventHandlers.cmEvTransBadMessage(
                                                    (HSTRANSSESSION) session,
                                                    haTransSession,
                                                    RvH323ConnectionH245,
                                                    buffer,
                                                    msgLen,
                                                    RV_FALSE);
                }
            }

            /* get rid of the decoded message */
            pvtDelete(transGlobals->hPvt, pvtNode);
        }

        /* go to the next encoded tunneled message */
        nodeId = pvtBrother(transGlobals->hPvt, nodeId);
    }

    /* if this was an incoming CONNECT and no response yet ther is no tunneling
       if EFC is supported, the parallel response may be delayed to after the connect,
       if it did not arrive already */
    if ( (msgType == (int)cmQ931connect) && (session->parallelTunnelingState == parllOffered) &&
         ( (session->extFastConnectState == efcNo) || (session->faststartState != fsOffered) ) )
        session->parallelTunnelingState = parllNo;
}

/**************************************************************************************
 * extractH450Messages
 *
 * purpose: extracts and reports the tunneled H.450 messages from the given message.
 *
 * Input: transGlobals      - The global data of the module.
 *        session           - the session on which the message was received.
 *        pvtNode           - The Q.931 message.
 *        msgType           - The type of the message from which we extract the tunneled msgs.
 ***************************************************************************************/
void extractH450Messages(cmTransGlobals *transGlobals,
                         cmTransSession *session,
                         int            pvtNode,
                         int            msgType)
{
    int encodedOctedStrings;

    HATRANSSESSION haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

    /* position on the H.450 part of the message */
    __pvtGetNodeIdByFieldIds(   encodedOctedStrings,
                                transGlobals->hPvt,
                                pvtNode,
                                {   _q931(message)
                                    _anyField
                                    _q931(userUser)
                                    _q931(h323_UserInformation)
                                    _q931(h323_uu_pdu)
                                    _q931(h4501SupplementaryService)
                                    LAST_TOKEN
                                });

    if (encodedOctedStrings >= 0 && transGlobals->sessionEventHandlers.cmEvTransNewH450Message)
    {
        /* loop over all the tunneled messages */
        int nodeId = pvtChild(transGlobals->hPvt, encodedOctedStrings);
        while ( (RV_PVT_NODEID_IS_VALID(nodeId)) && (!emaWasDeleted((EMAElement)session)) )
        {
            int size;

            /* get the size of the current octed string */
            pvtGet(transGlobals->hPvt, nodeId, NULL, NULL, &size, NULL);

            /* if we succeeded, report the message on the session to the user */
            {
                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "cmEvTransNewH450Message(hsSession = %d(%p), haSession=%p, msgNode=%d, length=%d, type=%d)",
                        emaGetIndex((EMAElement)session), session, haTransSession, nodeId, size, msgType));

                (transGlobals->sessionEventHandlers.cmEvTransNewH450Message)(
                                                (HSTRANSSESSION) session,
                                                haTransSession,
                                                nodeId,
                                                size, msgType);
            }

            /* go to the next encoded tunneled message */
            nodeId = pvtBrother(transGlobals->hPvt, nodeId);
        }
    }
}

/**************************************************************************************
 * extractAnnexMMessages
 *
 * purpose: extracts and reports the tunneled Annex M messages from the given message.
 *
 * Input: transGlobals      - The global data of the module.
 *        session           - the session on which the message was received.
 *        messageBodyNode   - the UU-IE part of the message.
 *        msgType           - The type of the message from which we extract the tunneled msgs.
 ***************************************************************************************/
void extractAnnexMMessages(cmTransGlobals *transGlobals,
                           cmTransSession *session,
                           int            messageBodyNode,
                           int            msgType)
{
    int annexMElement;
    HATRANSSESSION haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

    /* check if we have an annex M tunneling elemnt in the message */
    annexMElement = pvtGetChild(transGlobals->hPvt, messageBodyNode, __q931(tunnelledSignallingMessage), NULL);

    if (annexMElement >= 0)
    {
        /* report the message on the session to the user */
        if (transGlobals->sessionEventHandlers.cmEvTransNewAnnexMMessage)
        {
            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,  "cmEvTransNewAnnexMMessage(hsSession = %d(%p), haSession=%p, annexMElement=%d, type=%d)",
                    emaGetIndex((EMAElement)session), session, haTransSession, annexMElement, msgType));

            (transGlobals->sessionEventHandlers.cmEvTransNewAnnexMMessage)(
                                            (HSTRANSSESSION) session,
                                            haTransSession,
                                            annexMElement,
                                            msgType);
        }
    }
}

/**************************************************************************************
 * extractAnnexLMessages
 *
 * purpose: extracts and reports the tunneled Annex L messages from the given message.
 *
 * Input: transGlobals      - The global data of the module.
 *        session           - the session on which the message was received.
 *        messageBodyNode   - the UU-IE part of the message.
 *        msgType           - The type of the message from which we extract the tunneled msgs.
 ***************************************************************************************/
void extractAnnexLMessages(cmTransGlobals *transGlobals,
                           cmTransSession *session,
                           int            messageBodyNode,
                           int            msgType)
{
    int annexLElement;
    HATRANSSESSION haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

    /* check if we have an annex L tunneling element in the message */
    annexLElement = pvtGetChild(transGlobals->hPvt, messageBodyNode, __q931(stimulusControl), NULL);

    if (annexLElement >= 0)
    {
        /* report the message on the session to the user */
        if (transGlobals->sessionEventHandlers.cmEvTransNewAnnexLMessage)
        {
            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog, "cmEvTransNewAnnexLMessage(hsSession = %d(%p), haSession=%p, annexMElement=%d, type=%d)",
                    emaGetIndex((EMAElement)session), session,  haTransSession, annexLElement, msgType));

            (transGlobals->sessionEventHandlers.cmEvTransNewAnnexLMessage)(
                                            (HSTRANSSESSION) session,
                                            haTransSession,
                                            annexLElement,
                                            msgType);
        }
    }
}


#ifdef __cplusplus
}
#endif /* __cplusplus*/
