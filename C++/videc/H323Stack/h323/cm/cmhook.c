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
#include "cmintr.h"
#include "cmdebprn.h"
#include "psyntreeStackApi.h"
#include "rasutils.h"
#include "cmutils.h"
#include "rvh323connection.h"
#include "cmCall.h"
#include "transportint.h"
#include "transutil.h"
#include "transnet.h"
#include "tpkt.h"

#ifdef __cplusplus
extern "C" {
#endif




RVAPI
int RVCALLCONV cmSetProtocolEventHandlerReplacement(
        IN  HAPP        hApp,
        IN  CMPROTOCOLEVENT cmProtocolEventReplacement,
        IN  int     size)
{
    cmElem* app=(cmElem*)hApp;
    cmiAPIEnter((hApp,"cmSetProtocolEventHandlerReplacement(hApp=%p,cmProtocolEventReplacement=%p,size=%d)",hApp,cmProtocolEventReplacement,size));
    app->cmMyProtocolEventReplacement=cmProtocolEventReplacement;
    app->cmMyProtocolEventReplacementSize=size;
    cmiAPIExit((hApp,"cmSetProtocolEventHandlerReplacement=0"));
    return 0;
}
RVAPI
int RVCALLCONV cmSetProtocolEventHandler(
        IN  HAPP        hApp,
        IN  CMPROTOCOLEVENT cmProtocolEvent,
        IN  int     size)
{
    cmElem* app=(cmElem*)hApp;
    cmiAPIEnter((hApp,"cmSetProtocolEventHandler(hApp=%p,cmProtocolEvent,size=%d)",hApp,size));
    if (app->cmMyProtocolEventReplacement)
    {
        memset(((cmElem*)hApp)->cmMyProtocolEventReplacement, 0, (RvSize_t)app->cmMyProtocolEventReplacementSize);
        memcpy(((cmElem*)hApp)->cmMyProtocolEventReplacement, cmProtocolEvent, (RvSize_t)RvMin(app->cmMyProtocolEventReplacementSize,size));
    }
    else
    {
        memset(&((cmElem*)hApp)->cmMyProtocolEvent, 0, sizeof(((cmElem*)hApp)->cmMyProtocolEvent));
        memcpy(&((cmElem*)hApp)->cmMyProtocolEvent, cmProtocolEvent, (RvSize_t)RvMin((int)sizeof(((cmElem*)hApp)->cmMyProtocolEvent),size));
    }
    cmiAPIExit((hApp,"cmSetProtocolEventHandler=0"));
    return 0;
}


/******************************************************************************
 * cmProtocolSendRawMessage
 * ----------------------------------------------------------------------------
 * General: Send a raw message through a given connection.
 *          This function should be used for Q931 and H.245 messages. It allows
 *          sending malformed messages to remote clients and is useful for
 *          testing tools development.
 *          Raw messages are packetized using TPKT as any other Q931 and H.245
 *          message.
 *          Note: The raw message must have a clean header of
 *          CM_PROTOCOL_RAW_MESSAGE_HEADER_SIZE bytes at its beginning for the
 *          internal use of the Stack. The bufferSize value must include this
 *          additional header size in its calculation.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp         - Stack handle for the application.
 *         hConn        - Connection to use.
 *         buffer       - Buffer to send as the message.
 *         bufferSize   - Size of buffer to send.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolSendRawMessage(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvUint8     *buffer,
        IN  RvSize_t    bufferSize)
{
    RvStatus status = RV_OK;
    TRANSERR result;

    RV_UNUSED_ARG(hApp);

    cmiAPIEnter((hApp, "cmProtocolSendRawMessage(hApp=%p,hConn=%p,size=%d)", hApp, hConn, bufferSize));
    result = cmTransHostSendRawMessage((HSTRANSHOST)hConn,  buffer, bufferSize);
    if (result != cmTransOK)
        status = RV_ERROR_UNKNOWN;
    cmiAPIExit((hApp, "cmProtocolSendRawMessage=%d", status));

    return status;
}


/******************************************************************************
 * cmProtocolRecvRawMessage
 * ----------------------------------------------------------------------------
 * General: Receives a raw message through a given connection.
 *          This function should be used for Q931 messages only. It allows
 *          re-receiving of messages rejected by H.235.
 *          Raw messages are packetized using TPKT as any other Q931 and H.245
 *          message.
 *          Note: The raw message must have a clean header of
 *          CM_PROTOCOL_RAW_MESSAGE_HEADER_SIZE bytes at its beginning for the
 *          internal use of the Stack. The bufferSize value must include this
 *          additional header size in its calculation.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp         - Stack handle for the application.
 *         hConn        - Connection to use.
 *         buffer       - Buffer to send as the message.
 *         bufferSize   - Size of buffer to send.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolRecvRawMessage(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvUint8     *buffer,
        IN  RvSize_t    bufferSize)
{
    RvStatus status = RV_OK;
    TRANSERR result;

    RV_UNUSED_ARG(hApp);

    cmiAPIEnter((hApp, "cmTransHostReceiveRawMessage(hApp=%p,hConn=%p,size=%d)", hApp, hConn, bufferSize));
    result = cmTransHostReceiveRawMessage((HSTRANSHOST)hConn,  buffer, bufferSize);
    if (result != cmTransOK)
        status = RV_ERROR_UNKNOWN;
    cmiAPIExit((hApp, "cmTransHostReceiveRawMessage=%d", status));

    return (int)result;
}


/******************************************************************************
 * cmProtocolSendMessage
 * ----------------------------------------------------------------------------
 * General: Send a message through a given connection
 *          This function should be used for Q931 and H.245 messages.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp     - Stack handle for the application.
 *         hConn    - Connection to use.
 *         msg      - Node id of the message to send.
 *                    The message is not deleted by the call to this function.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolSendMessage(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvPvtNodeId msg)
{
    RvStatus status = RV_OK;
    TRANSERR result;

    RV_UNUSED_ARG(hApp);

    cmiAPIEnter((hApp, "cmProtocolSendMessage(hApp=%p,hConn=%p,msg=%d)", hApp, hConn, msg));
    result = cmTransHostSendMessage((HSTRANSHOST)hConn,  msg);
    if (result != cmTransOK)
        status = RV_ERROR_UNKNOWN;
    cmiAPIExit((hApp, "cmProtocolSendMessage=%d", status));

    return status;
}


/******************************************************************************
 * cmCallSendMessageExt
 * ----------------------------------------------------------------------------
 * General: Send a message for a call's connection
 *          This function should be used for Q931 and H.245 messages, and
 *          allows sending of tunnled messages.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hCall    - Stack handle for the call.
 *         msgType  - Type of message, Q.931 or H.245
 *         msgNode  - Node id of the message to send.
 *                    The message is not deleted by the call to this function.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmCallSendMessageExt(
        IN  HCALL                   hCall,
        IN  cmTpktChanHandleType    msgType,
        IN  RvPvtNodeId             msgNode)
{
    RvStatus status = RV_OK;
    TRANSERR result;
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER | RV_LOGLEVEL_LEAVE))
    HAPP hApp = emaGetInstance((EMAElement)hCall);
#endif
    callElem*call=(callElem*)hCall;

    if (emaLock((EMAElement)call))
    {
        cmiAPIEnter((hApp, "cmCallSendMessageExt(hCall=%p,msgType=%d,msgNode=%d)", hCall, msgType, msgNode));
        result = cmTransSendMessage(call->hsTransSession, msgNode, 
            (msgType==cmQ931TpktChannel)?cmTransQ931Type:cmTransH245Type);
        if (result != cmTransOK)
            status = RV_ERROR_UNKNOWN;
        cmiAPIExit((hApp, "cmCallSendMessageExt=%d", status));
        emaUnlock((EMAElement)call);
    }
    return status;
}


/******************************************************************************
 * cmProtocolSendMessageTo
 * ----------------------------------------------------------------------------
 * General: Send a RAS message to a given remote address.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp     - Stack handle for the application.
 *         hConn    - Connection to use.
 *         msg      - Node id of the message to send.
 *                    The message is not deleted by the call to this function.
 *         addr     - Node id of TransportAddress type, indicating the
 *                    remote address to send the message to.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolSendMessageTo(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvPvtNodeId msg,
        IN  RvPvtNodeId addr)
{
    int result=RV_ERROR_UNKNOWN;
    cmElem* app=(cmElem*)hApp;
    cmTransportAddress ta;

    cmiAPIEnter((hApp, "cmProtocolSendMessageTo(hApp=%p,hConn=%p,msg=%d)", hApp, hConn, msg));

    cmVtToTA(app->hVal,addr,&ta);
    result = rasEncodeAndSend((rasModule*)app->rasManager, NULL, cmRASTrStageDummy, msg,
                              (RvBool)(hConn==cmGetRASConnectionHandle(hApp)),
                              &ta, RV_FALSE, NULL);

    cmiAPIExit((hApp, "cmProtocolSendMessageTo=%d", result));
    return result;
}


RVAPI
int RVCALLCONV
cmProtocolCreateMessage(
                IN      HAPP                    hApp,
                IN      cmProtocol              protocol)
{
    cmElem* app=(cmElem*)hApp;
    int rootId;
    cmiAPIEnter((hApp,"cmProtocolCreateMessage (hApp=%p)",hApp));

    switch(protocol)
    {
        case cmProtocolQ931:
            rootId = pvtAddRoot(app->hVal,app->synProtQ931,0,NULL);
            break;
        case cmProtocolH245:
            rootId = pvtAddRoot(app->hVal,app->synProtH245,0,NULL);
            break;
        case cmProtocolRAS:
            rootId = pvtAddRoot(app->hVal,app->synProtRAS,0,NULL);
            break;
        default:
            rootId = RV_ERROR_UNKNOWN;
    }

    cmiAPIExit((hApp,"cmProtocolCreateMessage root=%d", rootId));
    return rootId;
}

RVAPI
cmProtocol RVCALLCONV cmProtocolGetProtocol(
                IN      HAPP                    hApp,
                IN      int                     msg)
{
    cmElem* app=(cmElem*)hApp;
    HPST hSyn;
    int synNodeId;
    if (!app || msg<0)
        return (cmProtocol)RV_ERROR_UNKNOWN;

    hSyn = pvtGetSynTree(app->hVal, msg);

    cmiAPIEnter((hApp,"cmProtocolGetProtocol hApp=%p",hApp));

    if (!RV_PVT_NODEID_IS_VALID(pvtGet(app->hVal, msg, NULL, &synNodeId, NULL, NULL)))
    {
        cmiAPIExit((hApp,"cmProtocolGetProtocol hApp=%p ERROR in pvtGet", hApp));
        return (cmProtocol)RV_ERROR_UNKNOWN;
    }

    if (pstAreNodesCongruent(hSyn,synNodeId,app->synProtQ931,pstGetRoot(app->synProtQ931)))
    {
        cmiAPIExit((hApp,"cmProtocolGetProtocol hApp=%p cmProtocolQ931", hApp));
        return cmProtocolQ931;
    }
    if (pstAreNodesCongruent(hSyn,synNodeId,app->synProtRAS,pstGetRoot(app->synProtRAS)))
    {
        cmiAPIExit((hApp,"cmProtocolGetProtocol hApp=%p cmProtocolRAS", hApp));
        return cmProtocolRAS;
    }
    if (pstAreNodesCongruent(hSyn,synNodeId,app->synProtH245,pstGetRoot(app->synProtH245)))
    {
        cmiAPIExit((hApp,"cmProtocolGetProtocol hApp=%p cmProtocolH245", hApp));
        return cmProtocolH245;
    }

    cmiAPIExit((hApp,"cmProtocolGetProtocol hApp=%p cmProtocolUnknown", hApp));
    return cmProtocolUnknown;
}

RVAPI
char* RVCALLCONV cmProtocolGetProtocolName(
                IN      cmProtocol              protocol)
{
    switch(protocol)
    {
        case cmProtocolQ931:return (char*)"Q931";
        case cmProtocolH245:return (char*)"H245";
        case cmProtocolRAS: return (char*)"RAS";
        default:            return (char*)"Unknown";
    }
}


/************************************************************************
 * cmProtocolGetLocalAddress
 * purpose: Get the local address of a given connection of the stack.
 *          This function can be used on Q.931 or H.245 connections.
 * input  : hApp    - Stack handle for the application
 *          hConn   - Connection to use
 * output : address - The local address of this connection
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmProtocolGetLocalAddress(
        IN  HAPP                hApp,
        IN  HPROTCONN           hConn,
        OUT cmTransportAddress* address)
{
    RV_UNUSED_ARG(hApp);

    if (address == NULL)
        return RV_ERROR_BADPARAM;

    if (cmTransGetHostParam( (HSTRANSHOST)hConn, cmTransHostParam_localAddress, address ) != cmTransOK)
        return RV_ERROR_UNKNOWN;

    return RV_OK;
}


/************************************************************************
 * cmProtocolGetRemoteAddress
 * purpose: Get the remote address of a given connection of the stack.
 *          This function can be used on Q.931 or H.245 connections.
 * input  : hApp    - Stack handle for the application
 *          hConn   - Connection to use
 * output : address - The remote address of this connection
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmProtocolGetRemoteAddress(
        IN  HAPP                hApp,
        IN  HPROTCONN           hConn,
        OUT cmTransportAddress* address)
{
    RV_UNUSED_ARG(hApp);

    if (address == NULL)
        return RV_ERROR_BADPARAM;

    if (cmTransGetHostParam( (HSTRANSHOST)hConn, cmTransHostParam_remoteAddress, address ) != cmTransOK)
        return RV_ERROR_UNKNOWN;

    return RV_OK;
}


/************************************************************************
 * cmProtocolSetTypeOfService
 * purpose: Set the connection's DiffServ Code Point value. This is used
 *          for QoS.
 *          This function can be used on RAS, Q.931 or H.245 connections.
 * input  : hApp            - Stack handle for the application
 *          hConn           - Connection to use
 *          typeOfService   - Type of service value
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmProtocolSetTypeOfService(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvInt       typeOfService)
{
    cmElem* app = (cmElem *)hApp;

    if (emaGetType((EMAElement)hConn) == RAS_PROTCONN)
    {
        /* It's a RAS handle */
        RasConnection* connection = (RasConnection *)hConn;

        return RvSocketSetTypeOfService(RvFdGetSocket(&connection->fd), typeOfService, app->logMgr);
    }
    else
    {
        RvH323Connection* connection = NULL;
         
        /*H245*/
        if (cmTransGetHostParam((HSTRANSHOST)hConn, cmTransHostParam_socketh245Listen, (void*)&connection) != cmTransOK)
            return RV_ERROR_UNKNOWN;

        if (connection != NULL)
        {
            RvSocketSetTypeOfService(RvFdGetSocket(&connection->fd), typeOfService, app->logMgr);
            connection = NULL;
        }

        /* Q931/AnnexE */
        if (cmTransGetHostParam((HSTRANSHOST)hConn, cmTransHostParam_socketConnection, (void*)&connection) != cmTransOK)
            return RV_ERROR_UNKNOWN;

        if (connection != NULL)
            return RvSocketSetTypeOfService(RvFdGetSocket(&connection->fd), typeOfService, app->logMgr);
    }

    return RV_ERROR_NOTSUPPORTED;
}


/************************************************************************
 * cmProtocolGetSocket
 * purpose: Get the underlying socket object of the operating system for
 *          a given connection object.
 * input  : hApp            - Stack handle for the application.
 *          hConn           - Connection to use.
 * output : socket          - Socket used for the connection.
 *                            The returned value should be typecast to
 *                            the operating system specific socket type
 *                            (int for most operating systems, SOCKET
 *                            for Windows).
 * return : RV_OK on success, or a negative value on failure.
 ************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolGetSocket(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        OUT void        **sock)
{
    RV_UNUSED_ARG(hApp);

    if (emaGetType((EMAElement)hConn) == RAS_PROTCONN)
    {
        /* It's a RAS handle */
        RasConnection* connection = (RasConnection *)hConn;

        *sock = (void *)*RvFdGetSocket(&connection->fd);
        return RV_OK;
    }
    else
    {
        RvH323Connection* connection = NULL;
         
        if (cmTransGetHostParam((HSTRANSHOST)hConn, cmTransHostParam_socketBestFit, (void*)&connection) != cmTransOK)
            return RV_ERROR_UNKNOWN;

        if (connection != NULL)
        {
            *sock = (void *)*RvFdGetSocket(&connection->fd);
            return RV_OK;
        }
    }

    return RV_ERROR_NOTSUPPORTED;
}


/******************************************************************************
 * cmProtocolFindCallByMessage
 * ----------------------------------------------------------------------------
 * General: Get a call handle related to a given message.
 *          This function can be used from within hook functions to find out
 *          the call handle of a given message.
 *          This function will work only on Q931 messages.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         hConn            - Connection this message was received from (or
 *                            sent on).
 *         messageNodeId    - Q931 message to use.
 * Output: hsCall           - Call object related with the message.
 *                            If this function returns RV_OK, and this handle
 *                            is NULL, then the message is a general message
 *                            on the connection and is not related to any
 *                            specific call.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtocolFindCallByMessage(
    IN  HAPP                    hApp,
    IN  HPROTCONN               hConn,
    IN  RvPvtNodeId             messageNodeId,
    OUT HCALL                   *hsCall)
{
    RV_UNUSED_ARG(hApp);

    if (cmTransGetSessionByMessage((HSTRANSHOST)hConn, messageNodeId, NULL, (HATRANSSESSION*)hsCall) == cmTransOK)
        return RV_OK;
    else
        return RV_ERROR_NOT_FOUND;
}


/******************************************************************************
 * cmProtConnNew
 * ----------------------------------------------------------------------------
 * General: Create a new TCP connection in the stack. the caller is responsible
 *          for the connection's termination.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         protocol         - Type of connection (Q.931 or H.245).
 *         bMultiServer     - should we listen as a multi-server?.
 *         eventHandler     - Network event handler.
 *         context          - Event handler context.
 *         remoteAddr       - Address to connect to (NULL for listening).
 *         localAddr        - Local Address to use (set to zero for random).
 * Output: hConn            - Connection created.
 *         localAddr        - Local address used.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtConnNew(
    IN    HAPP                  hApp,
    IN    cmProtocol            protocol,
    IN    RvBool                bMultiServer,
    IN    hostEventHandler      eventHandler,
    IN    void *                context,
    IN    cmTransportAddress *  remoteAddr,
    INOUT cmTransportAddress *  localAddr,
    OUT   HPROTCONN *           hConn)
{
    cmElem * app = (cmElem *) hApp;
    cmTransGlobals * transGlobals = (cmTransGlobals *)app->hTransport;
    RvH323ConnectionType connType;
    HATRANSHOST haTransHost;
    cmTransHost * host;
    HTPKT hTpkt = NULL;
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif
    TRANSERR reply;

    if (hConn == NULL) return RV_ERROR_NULLPTR;
    if ((remoteAddr != NULL) && (localAddr == NULL)) return RV_ERROR_NULLPTR;

    if (protocol == cmProtocolQ931)
    {
        connType = RvH323ConnectionQ931;
    }
    else if (protocol == cmProtocolH245)
    {
        connType = RvH323ConnectionH245;
    }
    else
        return RV_ERROR_NOTSUPPORTED;

    /* create host */
    reply = cmTransCreateHost((HAPPTRANS)transGlobals, NULL, NULL, (HSTRANSHOST*) &host);
    if (reply != cmTransOK)
    {
        return RV_ERROR_UNKNOWN;
    }

    /* initialize */
    memset(host, 0, sizeof(cmTransHost));
    host->annexEUsageMode = cmTransNoAnnexE;
    host->closeOnNoSessions = RV_TRUE;
    host->hostIsApproved = RV_TRUE;
    host->remoteCloseOnNoSessions = RV_TRUE;
    host->reported = RV_TRUE;
    if (connType == RvH323ConnectionQ931)
        host->type = cmTransQ931Conn;
    else
        host->type = cmTransH245Conn;
    haTransHost = (HATRANSHOST)emaGetApplicationHandle(((EMAElement)host));

    if (!emaLock((EMAElement)host))
        return RV_ERROR_UNKNOWN;

    /* see if we got a remote address to connect to */
    if (remoteAddr != NULL)
    {
        /* connect to the remote address */
        host->incoming = RV_FALSE;

        setRemoteAddress(host, remoteAddr);

        /* open a connection and stuff */
        hTpkt = tpktOpen(transGlobals->tpktCntrl, localAddr, connType,
            tpktClient, (EMAElement)host, (LPTPKTEVENTHANDLER)eventHandler, context);
        if(hTpkt == NULL)
        {
            emaDelete((EMAElement)host);
            emaUnlock((EMAElement)host);
            return RV_ERROR_UNKNOWN;
        }
        host->state = hostConnecting;

        if (transGlobals->hostEventHandlers.cmEvTransHostConnecting)
        {
            RvLogInfo(&transGlobals->hLog, (&transGlobals->hLog,
                "cmEvTransHostConnecting(hsHost = %d(%p), haHost=%p, type=cmTransQ931Conn, address = (ip:%s,port:%u))",
                emaGetIndex((EMAElement)host), host, haTransHost, getIP(&host->remoteAddress, buff), host->remoteAddress.port));

            reply = transGlobals->hostEventHandlers.cmEvTransHostConnecting(
                                    (HSTRANSHOST)host,
                                    haTransHost,
                                    cmTransQ931Conn,
                                    &host->remoteAddress);
            host->reported = RV_TRUE;
        }

        if ((reply != cmTransOK) || (tpktConnect(hTpkt, remoteAddr) != RV_OK))
        {
            transHostClose((HSTRANSHOST)host, RV_TRUE);
        }
    }
    else if (localAddr != NULL)
    {
        /* just listen */
        host->state = hostListenning;
        host->incoming = RV_TRUE;

        /* start listening */
        if (transGlobals->hostEventHandlers.cmEvTransHostListen)
        {
            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                "cmEvTransHostListen(hsHost = %d(%p), haHost=%p, type=cmTransH245Conn, address = (ip:%s,port:%u))",
                emaGetIndex((EMAElement)host), host, haTransHost, getIP(localAddr, buff), localAddr->port));

            reply = transGlobals->hostEventHandlers.cmEvTransHostListen(
                                    (HSTRANSHOST)host,
                                    haTransHost,
                                    cmTransH245Conn,
                                    localAddr);
            host->reported = RV_TRUE;
        }
        if (reply == cmTransOK)
        {
            /* open a connection and stuff */
            hTpkt = tpktOpen(transGlobals->tpktCntrl, localAddr, connType,
                bMultiServer?tpktMultiServer:tpktServer, (EMAElement)host,
                (LPTPKTEVENTHANDLER)eventHandler, context);
        }
        if(hTpkt != NULL)
        {
            tpktGetAddresses(hTpkt, localAddr, NULL);
            cmTransGetGoodAddress(app->hTransport, localAddr);
        }
        else
        {
            transHostClose((HSTRANSHOST)host, RV_TRUE);
        }

        /* notify listening */
        if (transGlobals->hostEventHandlers.cmEvTransHostListening)
        {
            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                "cmEvTransHostListening(hsHost = %d(%p), haHost=%p, type=cmTransH245Conn, address = (ip:%s,port:%u))",
                emaGetIndex((EMAElement)host), host, haTransHost, getIP(localAddr, buff), localAddr->port));

            reply = transGlobals->hostEventHandlers.cmEvTransHostListening(
                                    (HSTRANSHOST)host,
                                    haTransHost,
                                    cmTransH245Conn,
                                    localAddr,
                                    RV_FALSE);
            host->reported = RV_TRUE;
        }
        host->localAddress = *localAddr;
    }

    host->hTpkt = hTpkt;
    *hConn = (HPROTCONN)host;
    emaUnlock((EMAElement)host);
    return RV_OK;
}


/******************************************************************************
 * cmProtConnRelinquish
 * ----------------------------------------------------------------------------
 * General: Relinquish a TCP connection to the default event handler of the stack.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         hConn            - Connection to relinquish.
 * Output: none.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtConnRelinquish(
    IN    HAPP                  hApp,
    IN    HPROTCONN             hConn)
{
    cmTransHost * host = (cmTransHost *)hConn;
    RV_UNUSED_ARG(hApp);

    if (!emaLock((EMAElement)hConn))
        return RV_ERROR_DESTRUCTED;

    if(host->type == cmTransQ931Conn)
    {
        /* register a new callback */
        tpktRecvNotify(host->hTpkt, transQ931Handler, host);
    }
    if(host->type == cmTransH245Conn)
    {
        /* register a new callback */
        tpktRecvNotify(host->hTpkt, transH245Handler, host);
    }

    emaUnlock((EMAElement)hConn);
    return RV_OK;
}


/******************************************************************************
 * cmProtConnHandleEventByDefault
 * ----------------------------------------------------------------------------
 * General: Relayes an event to the default event handlers.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         hConn            - Connection to relinquish.
 *         event            - Event flag.
 * Output: none.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtConnHandleEventByDefault(
    IN    HAPP                  hApp,
    IN    HPROTCONN             hConn,
    IN    RvUint32              event)
{
    cmTransHost * host = (cmTransHost *)hConn;
    RV_UNUSED_ARG(hApp);

    if (!emaLock((EMAElement)hConn))
        return RV_ERROR_DESTRUCTED;

    if(host->type == cmTransQ931Conn)
    {
        transQ931Handler(host->hTpkt, (RvSelectEvents)event, 0, RV_FALSE, (void *)host);
    }
    if(host->type == cmTransH245Conn)
    {
        if (event == RvSelectAccept)
            transH245AcceptHandler(host->hTpkt, (RvSelectEvents)event, 0, RV_FALSE, (void *)host);
        else
            transH245Handler(host->hTpkt, (RvSelectEvents)event, 0, RV_FALSE, (void *)host);
    }

    emaUnlock((EMAElement)hConn);
    return RV_OK;
}

/******************************************************************************
 * cmProtConnGetParam
 * ----------------------------------------------------------------------------
 * General: Get a parameter from a connection.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         hConn            - Connection to get the parameter from.
 *         eParam           - Parameter to get.
 *         iValue           - The length of sValue.
 * Output: iValue           - The value or the length used of sValue.
 *         sValue           - Non-integer value.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtConnGetParam(
    IN    HAPP                  hApp,
    IN    HPROTCONN             hConn,
    IN    cmProtConnParam       eParam,
    INOUT RvInt32 *             iValue,
    OUT   RvUint8 *             sValue)
{
    cmTransHost * host = (cmTransHost *)hConn;
    RvStatus status = RV_OK;
    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(sValue);

    if (!emaLock((EMAElement)hConn))
        return RV_ERROR_DESTRUCTED;

    switch (eParam)
    {
    case cmProtConnParamType:
        if (host->type == cmTransQ931Conn)
            *iValue = (RvInt32)cmProtocolQ931;
        else if (host->type == cmTransH245Conn)
            *iValue = (RvInt32)cmProtocolH245;
        else
            status = RV_ERROR_UNKNOWN;
        break;
    case cmProtConnParamIncoming:
        *iValue = (RvInt32)host->incoming;
        break;
    case cmProtConnParamState:
        *iValue = (RvInt32)host->state;
        break;
    case cmProtConnParamMultiplex:
        *iValue = (host->isMultiplexed && host->remoteIsMultiplexed);
        break;
    case cmProtConnParamMaintain:
        *iValue = !(host->closeOnNoSessions || host->remoteCloseOnNoSessions);
        break;
    case cmProtConnParamNumCalls:
        {
            int i;
            cmTransSession * session = host->firstSession;
            for (i=0; session != NULL; i++)
                session = session->nextSession;
            *iValue = i;
        }
        break;
    case cmProtConnParamTpkt:
        status = RV_ERROR_BADPARAM;
        break;
    default:
        status = RV_ERROR_BADPARAM;
    }

    emaUnlock((EMAElement)hConn);
    return status;
}


/******************************************************************************
 * cmProtConnSetParam
 * ----------------------------------------------------------------------------
 * General: Set a parameter for a connection.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         hConn            - Connection to set the parameter for.
 *         eParam           - Parameter to set.
 *         iValue           - The value to set or the length of sValue.
 *         sValue           - Non-integer value, or NULL.
 * Output: none.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtConnSetParam(
    IN    HAPP                  hApp,
    IN    HPROTCONN             hConn,
    IN    cmProtConnParam       eParam,
    IN    RvInt32               iValue,
    IN    RvUint8 *             sValue)
{
    cmTransHost * host = (cmTransHost *)hConn;
    RvStatus status = RV_OK;

    if (!emaLock((EMAElement)hConn))
        return RV_ERROR_DESTRUCTED;

    switch (eParam)
    {
    case cmProtConnParamType:
        if (iValue == cmProtocolQ931)
            host->type = cmTransQ931Conn;
        else if (iValue == cmProtocolH245)
            host->type = cmTransH245Conn;
        else
            status = RV_ERROR_BADPARAM;
        break;
    case cmProtConnParamIncoming:
        host->incoming = (RvBool)iValue;
        break;
    case cmProtConnParamState:
        if (((cmProtConnState)iValue) > cmProtConnStateConnected)
        {
            /* we do not allow setting busy, closing or closed */
            status = RV_ERROR_BADPARAM;
            break;
        }
        host->state = (HOSTSTATE)iValue;
        break;
    case cmProtConnParamMultiplex:
        host->isMultiplexed = (iValue != 0);
        host->remoteIsMultiplexed = (iValue != 0);
        break;
    case cmProtConnParamMaintain:
        host->closeOnNoSessions = (iValue == 0);
        host->remoteCloseOnNoSessions = (iValue == 0);
        break;
    case cmProtConnParamNumCalls:
        status = RV_ERROR_BADPARAM;
        break;
    case cmProtConnParamTpkt:
        if (iValue == sizeof(void *))
        {
            host->hTpkt = *(HTPKT*)sValue;
            transParentShare(((cmElem*)hApp)->hTransport, (void*)host, (void*)host->hTpkt);
        }
        else
            host->hTpkt = NULL;
        break;
    default:
        status = RV_ERROR_BADPARAM;
    }

    emaUnlock((EMAElement)hConn);
    return status;
}


/******************************************************************************
 * cmProtConnClose
 * ----------------------------------------------------------------------------
 * General: Destroy a TCP connection in the stack.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         hConn            - Connection to destroy.
 * Output: none.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtConnClose(
    IN    HAPP                  hApp,
    IN    HPROTCONN             hConn)
{
    RV_UNUSED_ARG(hApp);
    if (!emaLock((EMAElement)hConn))
        return RV_ERROR_DESTRUCTED;
    transHostClose((HSTRANSHOST)hConn, RV_TRUE);
    emaUnlock((EMAElement)hConn);
    return RV_OK;
}



#ifdef __cplusplus
}
#endif



