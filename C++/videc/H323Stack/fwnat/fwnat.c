#ifdef __cplusplus
extern "C" {
#endif
/********************************************************************************
*                                                                               *
* NOTICE:                                                                       *
* This document contains information that is confidential and proprietary to    *
* RADVISION LTD.. No part of this publication may be reproduced in any form     *
* whatsoever without written prior approval by RADVISION LTD..                  *
*                                                                               *
* RADVISION LTD. reserves the right to revise this publication and make changes *
* without obligation to notify any person of such revisions or changes.         *
* Copyright RADVISION 1998.                                                     *
********************************************************************************/


#include "fwnat.h"
#include "q931asn1.h"
#include "h245.h"
#include "rvselect.h"
#include "cmintr.h"
#include "cmCall.h"
#include "stkutils.h"
#include "pvaltreeStackApi.h"


#define H460_18_PROTOCOL_IDENTIFIER "itu-t recommendation h(8) 460 18 0 1"
#define H460_19_PROTOCOL_IDENTIFIER "itu-t recommendation h(8) 460 19 0 1"



/* Stack Event handlers:
 * These should be given to the stack to handle incoming/outoging connections to H.245 multi-server
 * connections, and for Q.931 reveres call (the in-side).
 **********************/


/********************************************************************************
 * Function Name: RvH323FwNatConnectEventHandler (stack event handler)
 * Description: This function will be provided to the transport layer when
 *              creating a new outgoing connection, to replace the default
 *              transport event handler (used to handle the connect event).
 ********************************************************************************/
void RvH323FwNatConnectEventHandler(
    IN  void *          tpkt,
    IN  int             event,
    IN  int             length,
    IN  int             error,
    IN  void *          context);

/********************************************************************************
 * Function Name: RvH323FwNatAcceptEventHandler (stack event handler)
 * Description: This function will be provided to the transport layer when
 *              creating a new listening connection, to replace the default
 *              transport event handler (used to handle the accept event).
 ********************************************************************************/
void RvH323FwNatAcceptEventHandler(
    IN  void *          tpkt,
    IN  int             event,
    IN  int             length,
    IN  int             error,
    IN  void *          context);


/********************************************************************************
 * Type Name: RvH323FwNatDefaultSetProtConnHandleEv
 * Description: default callback, to use when the application does not set
 *              application handle for ProtConn elements,
 *
 * Input:   hApp         - stack application handle.
 *          hFwNat       - FwNat module handle
 *          hProtConn    - Stack's connection handle
 *          hFwNatConn   - The FwNat connection related to the connection
 *                         or NULL to unset.
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RVCALLCONV RvH323FwNatDefaultSetProtConnHandleEv(
    IN  HPROTCONN           hProtConn,
    IN  HH323FWNATCONN      hFwNatConn)
{
    return cmSetTpktChanApplHandle(hProtConn, (HAPPCONN)hFwNatConn);
}


/********************************************************************************
 * Type Name: RvH323FwNatSetProtConnHandleEv_t (callback prototype)
 * Description: default callback, to use when the application does not set
 *              application handle for ProtConn elements,
 *
 * Input:   hApp         - stack application handle.
 *          hFwNat       - FwNat module handle
 *          hProtConn    - Stack's connection handle
 * Output:  hFwNatConn   - The FwNat connection related to the connection
 *                         or NULL to unset.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RVCALLCONV RvH323FwNatDefaultGetProtConnHandleEv(
    IN  HPROTCONN           hProtConn,
    OUT HH323FWNATCONN *    hFwNatConn)
{
    return cmGetTpktChanApplHandle(hProtConn, (HAPPCONN*)hFwNatConn);
}


/********************************************************************************
 * Function Name: RvH323FwNatBuildEncapsulationFacilityMessage
 * Description:  Create a default facility message to use for encapsulation.
 *
 * Input:   mgr          - FwNat module handle.
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RvH323FwNatBuildEncapsulationFacilityMessage(
    IN  FwNatMgr *  mgr)
{
    RvPvtNodeId nodeId;
    RvUint16 crv = 0;
    HPVT hVal;
    RvUint8 facilityString[] = "Encapsulated RAS message";
    RvInt32 len = (RvInt32)strlen((const char *)facilityString);

    if (RV_PVT_NODEID_IS_VALID(mgr->defEncapMsg))
        return RV_OK;

    hVal = cmGetValTree(mgr->hApp);

    mgr->defEncapMsg = pvtAddRoot(hVal, cmGetSynTreeByRootName(mgr->hApp, "q931"), 0, NULL);

    pvtAdd(hVal, mgr->defEncapMsg, __q931(protocolDiscriminator), 8, NULL, NULL);

    __pvtBuildByFieldIds(nodeId, hVal, mgr->defEncapMsg,
        { _q931(callReferenceValue) _q931(twoBytes) LAST_TOKEN }, crv, NULL);

    __pvtBuildByFieldIds(nodeId, hVal, mgr->defEncapMsg,
        {_q931(message) _q931(facility) LAST_TOKEN}, 0, NULL);

    pvtAdd(hVal, nodeId, __q931(facility), len, (const char *)facilityString, NULL);
    nodeId = pvtAdd(hVal, nodeId, __q931(userUser), 0, NULL, NULL);
    pvtAdd(hVal, nodeId, __q931(protocolDiscriminator), 5, NULL, NULL);

    __pvtBuildByFieldIds(nodeId, hVal, nodeId,
        { _q931(h323_UserInformation) _q931(h323_uu_pdu) LAST_TOKEN}, 0, NULL);

    pvtAdd(hVal, nodeId, __q931(h245Tunneling), RV_TRUE, NULL, NULL);

    __pvtBuildByFieldIds(nodeId, hVal, nodeId,
        { _q931(h323_message_body) _q931(empty) LAST_TOKEN}, 0, NULL);

    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_ERROR_UNKNOWN;

    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatEncapsulateMessage
 * Description:  Build a facility message encapsulating the given buffer.
 *
 * Input:   mgr          - FwNat module handle.
 *          rasMessage   - The message to be encapsulated.
 *          msgLen       - Message length.
 * Output:  facilityMessage - root on which the encapsulating message was built.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RvH323FwNatEncapsulateMessage(
    IN  FwNatMgr *      mgr,
    IN  const RvUint8 * rasMessage,
    IN  RvInt32         msgLen,
    OUT RvPvtNodeId     facilityMessage)
{
    RvPvtNodeId nodeId, nodeId2;
    HPVT hVal;

    if (!RV_PVT_NODEID_IS_VALID(mgr->defEncapMsg))
        return RV_ERROR_UNKNOWN;

    hVal = cmGetValTree(mgr->hApp);

    pvtSetTree(hVal, facilityMessage, hVal, mgr->defEncapMsg);

    __pvtBuildByFieldIds(nodeId, hVal, facilityMessage,
        { _q931(message) _q931(facility) _q931(userUser)_q931(h323_UserInformation)
          _q931(h323_uu_pdu) _q931(genericData) _nul(0)
          LAST_TOKEN}, 0, NULL);

    __pvtBuildByFieldIds(nodeId2, hVal, nodeId,
        {_q931(id) _q931(standard) LAST_TOKEN}, 17, NULL);
    __pvtBuildByFieldIds(nodeId, hVal, nodeId,
        {_q931(parameters) _nul(0) LAST_TOKEN}, 0, NULL);
    __pvtBuildByFieldIds(nodeId2, hVal, nodeId,
        {_q931(id) _q931(standard) LAST_TOKEN}, 1, NULL);

    if (!RV_PVT_NODEID_IS_VALID(nodeId) || !RV_PVT_NODEID_IS_VALID(nodeId2))
        return RV_ERROR_UNKNOWN;

    __pvtBuildByFieldIds(nodeId, hVal, nodeId,
        { _q931(content) _q931(raw) LAST_TOKEN}, msgLen, (const char *)rasMessage);

    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_ERROR_UNKNOWN;

    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatHasEncapsulatedMessage
 * Description:  Checks the generic data for encapsulated message identifiers.
 *
 * Input:   mgr          - FwNat module handle.
 *          msgNode      - Root of the message.
 * Output:  paramsNode   - node in which the encapsulation parameters are stored.
 * Return: RV_TRUE if there is an encapsulated message, RV_FALSE o.w.
 ********************************************************************************/
RvBool RvH323FwNatHasEncapsulatedMessage(
    IN  FwNatMgr *      mgr,
    IN  RvPvtNodeId     msgNode,
    OUT RvPvtNodeId *   paramsNode)
{
    RvPvtNodeId nodeId, nodeId2, nodeId3;
    HPVT hVal;

    if (!RV_PVT_NODEID_IS_VALID(msgNode))
        return RV_FALSE;

    hVal = cmGetValTree(mgr->hApp);

    /* does this message contain generic data? */
    __pvtGetByFieldIds(nodeId, hVal, msgNode,
        { _q931(message) _anyField _q931(userUser)_q931(h323_UserInformation)
          _q931(h323_uu_pdu) _q931(genericData) _anyField LAST_TOKEN}, NULL, NULL, NULL);

    while (RV_PVT_NODEID_IS_VALID(nodeId))
    {
        RvInt32 val = 0;

        /* go over the generic data, and try to find one that fits */
        __pvtGetByFieldIds(nodeId2, hVal, nodeId,
            {_q931(id) _q931(standard) LAST_TOKEN}, NULL, &val, NULL);
        if (val == 17)
        {
            __pvtGetByFieldIds(nodeId2, hVal, nodeId,
                {_q931(parameters) _anyField LAST_TOKEN}, NULL, &val, NULL);
            while (RV_PVT_NODEID_IS_VALID(nodeId2))
            {
                __pvtGetByFieldIds(nodeId3, hVal, nodeId2,
                    { _q931(id) _q931(standard) LAST_TOKEN }, NULL, &val, NULL);
                if (RV_PVT_NODEID_IS_VALID(nodeId3) && (val == 1))
                {
                    *paramsNode = nodeId2;
                    return RV_TRUE;
                }
                nodeId2 = pvtBrother(hVal, nodeId2);
            }
        }
        nodeId = pvtBrother(hVal, nodeId);
    }
    return RV_FALSE;
}


/********************************************************************************
 * Function Name: RvH323FwNatGetEncapsulatedMessage
 * Description:  Extracts the encapsulated message.
 *
 * Input:   mgr          - FwNat module handle.
 *          paramsNode   - Node in which the encapsulation parameters are stored.
 * Output:  message      - Pointer to a buffer containing the message
 *          msgLen       - Length of the message
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RvH323FwNatGetEncapsulatedMessage(
    IN  FwNatMgr *      mgr,
    IN  RvPvtNodeId     paramsNode,
    OUT RvUint8 * *     message,
    OUT RvInt32 *       msgLen)
{
    RvPvtNodeId nodeId;
    HPVT hVal;

    if (!RV_PVT_NODEID_IS_VALID(paramsNode))
        return RV_ERROR_UNKNOWN;

    hVal = cmGetValTree(mgr->hApp);

    /* get the message node */
    __pvtGetByFieldIds(nodeId, hVal, paramsNode,
        { _q931(content) _q931(raw) LAST_TOKEN }, NULL, NULL, NULL);

    if (RV_PVT_NODEID_IS_VALID(nodeId))
    {
        RvInt32 len = pvtGetString(hVal, nodeId, (RvInt32)mgr->encapSize, (char *)mgr->encapBuffer);
        if (len > 0)
        {
            *msgLen = len;
            *message = mgr->encapBuffer;
            return RV_OK;
        }
    }
    return RV_ERROR_UNKNOWN;
}


/********************************************************************************
 * Function Name: encapConnectHandler
 * Description:  handle the connect event on the RAS encapsulation connection.
 ********************************************************************************/
void encapConnectHandler(void * tpkt, int event, int length, int error, void*context)
{
    FwNatConn * fnConn = context;
    HPROTCONN hConn = fnConn->protConn;
    FwNatMgr * mgr;
    cmProtConnState eConnState;

    RV_UNUSED_ARG(length);
    RV_UNUSED_ARG(tpkt);

    /* basic NULL pointer checks */
    if (!emaLock((EMAElement)fnConn))
        return;
    mgr = (FwNatMgr *)emaGetInstance((EMAElement)fnConn);
    if (mgr == NULL)
    {
        emaUnlock((EMAElement)fnConn);
        return;
    }

    /* sanity checks */
    if ((fnConn->connType != RvH323FwNatConnTypeRasEncapsulation) ||
        (!mgr->bGatekeeper && (fnConn != mgr->pGkConn)))
    {
        /* very bad, but don't clean the whole thing, we may still get correct events */
        RvLogExcep(&mgr->log, (&mgr->log,
            "encapEventHandler: got event on unknown FNConn %p (type: %d)", fnConn, fnConn->connType));
        mgr->callbacks.fwNatSetProtConnHandleEv(fnConn->protConn, NULL);
        cmProtConnClose(mgr->hApp, fnConn->protConn);
        emaDelete((EMAElement)fnConn);
        emaUnlock((EMAElement)fnConn);

        /* if we are expecting a connection, set the state as error, otherwise don't mess it up */
        if ((mgr->e17State == RvH323FwNatH46017StateEnabled) && (!mgr->bGatekeeper))
            mgr->e17State = RvH323FwNatH46017StateError;
        return;
    }

    /* check for network errors */
    if (error != 0)
    {
        /* report the error */
        RvLogError(&mgr->log, (&mgr->log,
            "encapEventHandler: %p got error from network error = %d", fnConn, error));
        if (mgr->callbacks.fwNatRasEncapStatusEv != NULL)
        {
            mgr->callbacks.fwNatRasEncapStatusEv(mgr->hApp, (HH323FWNATMGR)mgr,
                hConn, (HH323FWNATCONN)fnConn, RvH323FwNatH46017StateError, fnConn->hsRas);
        }
        /* clean the connection */
        mgr->pGkConn = NULL;
        mgr->callbacks.fwNatSetProtConnHandleEv(fnConn->protConn, NULL);
        cmProtConnClose(mgr->hApp, fnConn->protConn);
        if (emaLock((EMAElement)fnConn))
        {
            emaDelete((EMAElement)fnConn);
            emaUnlock((EMAElement)fnConn);
        }
        mgr->e17State = RvH323FwNatH46017StateError;
        emaUnlock((EMAElement)fnConn);
        return;
    }
    /* get the current stage of the connection */
    cmProtConnGetParam(mgr->hApp, hConn, cmProtConnParamState, (RvInt32*)&eConnState, NULL);

    /* we're only expecting to handle the connection event, and relinquish control right after it */
    if ((eConnState != cmProtConnStateConnecting) || ((event & RvSelectConnect) == 0))
    {
        RvLogError(&mgr->log, (&mgr->log,
            "encapEventHandler: %p (stage: %d) unexpected event %x", fnConn, eConnState, event));
        emaUnlock((EMAElement)fnConn);
        return;
    }

    RvLogDebug(&mgr->log, (&mgr->log,
        "encapEventHandler: %p (stage: %d) got event %x", fnConn, eConnState, event));
    cmProtConnSetParam(mgr->hApp, hConn, cmProtConnParamMaintain, RV_TRUE, NULL);

    if (!mgr->bGatekeeper)
        /* change state, so we know we no longer need a connection */
        mgr->e17State = RvH323FwNatH46017StateConnected;

    /* Why relinquish?
    Well, the transport layer already has such wonderful mechanisms for handling busy
    connections and receiving messages into buffers and all that, why re-invent the wheel?
    However, we do pay for it with the need to check every message. We could compensate by
    setting a callback especially for non-call-related messages. */
    cmProtConnRelinquish(mgr->hApp, hConn);
    /* don't forget to let them handle the event */
    cmProtConnHandleEventByDefault(mgr->hApp, hConn, (RvUint32)event);

    /* raise an event */
    if (mgr->callbacks.fwNatRasEncapStatusEv != NULL)
    {
        mgr->callbacks.fwNatRasEncapStatusEv(mgr->hApp, (HH323FWNATMGR)mgr,
            hConn, (HH323FWNATCONN)fnConn, RvH323FwNatH46017StateConnected, fnConn->hsRas);
    }
    emaUnlock((EMAElement)fnConn);
}


/********************************************************************************
 * Function Name: RvH323FwNatConnectRasEncapsulation
 * Description:  Create RAS encapsulation connection.
 *
 * Input:   mgr          - FwNat module handle.
 *          remoteAddr   - Address to create a connection to.
 *          context      - Context to be raised with the connection event.
 * Output:  pFnConn      - Pointer to the created connection.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RvH323FwNatConnectRasEncapsulation(
    IN  FwNatMgr *              mgr,
    IN  cmTransportAddress *    remoteAddr,
    IN  HRAS                    hsRas,
    OUT FwNatConn **            pFnConn)
{
    cmTransportAddress localAddr;
    HPROTCONN hConn;
    FwNatConn * fnConn;
    RvStatus status;

    RvLogEnter(&mgr->log, (&mgr->log,
        "RvH323FwNatConnectRasEncapsulation: state = %d", mgr->e17State));

    if (mgr->e17State == RvH323FwNatH46017StateDisabled)
    {
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatConnectRasEncapsulation: not supported"));
        return RV_ERROR_NOTSUPPORTED;
    }

    if (mgr->e17State == RvH323FwNatH46017StateConnected)
    {
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatConnectRasEncapsulation: already connected"));
        return RV_OK;
    }

    if (mgr->e17State == RvH323FwNatH46017StateError)
        mgr->e17State = RvH323FwNatH46017StateEnabled;

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.type = remoteAddr->type;

    fnConn = (FwNatConn *)emaAdd(mgr->hFwNatConns, NULL);
    if (fnConn == NULL)
    {
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatConnectRasEncapsulation: cannot allocate FwNatConn"));
        return RV_ERROR_OUTOFRESOURCES;
    }

    status = cmProtConnNew(mgr->hApp, cmProtocolQ931, RV_FALSE, encapConnectHandler, fnConn,
        remoteAddr, &localAddr, &hConn);

    if (status == RV_OK)
    {
        mgr->callbacks.fwNatSetProtConnHandleEv(hConn, (HH323FWNATCONN)fnConn);
        memset(fnConn, 0, sizeof(FwNatConn));
        fnConn->connType = RvH323FwNatConnTypeRasEncapsulation;
        fnConn->protConn = hConn;
        fnConn->hsRas = hsRas;
        cmProtConnSetParam(mgr->hApp, hConn, cmProtConnParamMaintain, RV_TRUE, NULL);
        cmProtConnSetParam(mgr->hApp, hConn, cmProtConnParamMultiplex, RV_TRUE, NULL);

        if (mgr->bGatekeeper)
        {
            /* add to the address hash */
            fnConn->hashLoc = hashAdd(mgr->hAddrHash, remoteAddr, &fnConn, RV_FALSE);
        }
        else
        {
            /* set as our gatekeeper connection */
            mgr->pGkConn = fnConn;
        }
        *pFnConn = fnConn;
    }
    else
    {
        emaDelete(fnConn);
    }

    RvLogLeave(&mgr->log, (&mgr->log,
        "RvH323FwNatConnectRasEncapsulation: result = %d", status));
    return status;
}


/********************************************************************************
 * Function Name: RvH323FwNatGetEncapsulationConnection
 * Description:  Extracts the encapsulated message.
 *
 * Input:   mgr          - FwNat module handle.
 *          remoteAddr   - Address to check connection to.
 * Output:  none
 * Return: The encapsulation connection.
 ********************************************************************************/
FwNatConn * RvH323FwNatGetEncapsulationConnection(
    IN  FwNatMgr *              mgr,
    IN  cmTransportAddress *    remoteAddr)
{
    FwNatConn * fnConn = NULL;

    /* if we are a GK, and enabled, check for the address in the hash */
    if (mgr->bGatekeeper && (mgr->e17State == RvH323FwNatH46017StateEnabled))
    {
        void * loc = hashFind(mgr->hAddrHash, remoteAddr);
        if (loc != NULL)
            fnConn = *(FwNatConn **)hashGetElement(mgr->hAddrHash, loc);
    }

    /* if we are an EP, check to GK connection */
    if (!mgr->bGatekeeper &&
        ((mgr->e17State == RvH323FwNatH46017StateEnabled) || (mgr->e17State == RvH323FwNatH46017StateConnected)))
    {
        if (emaLock((EMAElement)mgr->pGkConn))
        {
            EMAElement pUnlock = (EMAElement)mgr->pGkConn;
            cmTransportAddress gkAddress;

            /* make sure that the remote address is the same */
            cmProtocolGetRemoteAddress(mgr->hApp, mgr->pGkConn->protConn, &gkAddress);
            if (!cmCompareTransportAddresses(&gkAddress, remoteAddr))
            {
                RvInt32 numCalls;
                /* this gatekeeper connection
                is wrong for this transaction
                must take some kind of action
                to be rid of this infection */
                mgr->callbacks.fwNatSetProtConnHandleEv(mgr->pGkConn->protConn, NULL);
                /* should we delete this connection? it's relinquished, so the stack could handle
                it when it closes, and it could be multiplexing calls - but I'd rather close it */
                cmProtConnGetParam(mgr->hApp, mgr->pGkConn->protConn, cmProtConnParamNumCalls, &numCalls, NULL);
                if (numCalls == 0)
                    cmProtConnClose(mgr->hApp, mgr->pGkConn->protConn);
                emaDelete(mgr->pGkConn);
                mgr->pGkConn = NULL;
                mgr->e17State = RvH323FwNatH46017StateEnabled;
            }
            emaUnlock(pUnlock);
        }
        fnConn = mgr->pGkConn;
    }
    return fnConn;
}


/********************************************************************************
 * Function Name: RvH323FwNatSetEncapsulatedConnection
 * Description:  Creates an encapsulation connection from an existing connection.
 *
 * Input:   mgr          - FwNat module handle.
 *          hConn        - Stack handle to the connection.
 * Output:  none
 * Return: The encapsulation connection.
 ********************************************************************************/
FwNatConn * RvH323FwNatSetEncapsulatedConnection(
    IN  FwNatMgr *              mgr,
    IN  HPROTCONN               hConn)
{
    cmTransportAddress remoteAddr;
    FwNatConn * fnConn;

    /* sanity check */
    if (!mgr->bGatekeeper)
    {
        RvLogExcep(&mgr->log, (&mgr->log,
            "RvH323FwNatSetEncapsulatedConnection: received encapsulated message on unknown ProtConn %p", hConn));
        return NULL;
    }

    /* get the remote address */
    cmProtocolGetRemoteAddress(mgr->hApp, hConn, &remoteAddr);

    /* create the host */
    fnConn = (FwNatConn *)emaAdd(mgr->hFwNatConns, NULL);
    mgr->callbacks.fwNatSetProtConnHandleEv(hConn, (HH323FWNATCONN)fnConn);
    memset(fnConn, 0, sizeof(FwNatConn));
    fnConn->connType = RvH323FwNatConnTypeRasEncapsulation;
    fnConn->protConn = hConn;

    /* add to the address hash */
    fnConn->hashLoc = hashAdd(mgr->hAddrHash, &remoteAddr, &fnConn, RV_FALSE);

    return fnConn;
}

/********************************************************************************
 * Function Name: addIncomingCallIndication
 * Description:  Adds an IncomingCallIndication to a SCI message in a ras
 *               transaction object.
 *
 * Input:   mgr          - FW/NAT manager.
 *          hsCall       - Handle of call in the stack.
 *          hsRas        - Handle of SCI ras transaction.
 * Output:  None.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
void addIncomingCallIndication(IN FwNatMgr *mgr,
                               IN HCALL hsCall,
                               IN HRAS hsRas)
{
    HPVT                hVal = cmGetValTree(mgr->hApp);
    RvPvtNodeId         SCINodeId, genericDataNodeId, parameterNodeId, tmpNodeId, addrNodeId, inCallIndNodeId;
    RvInt32             addrLen = (RvInt32)sizeof(cmTransportAddress), callIdLen = 16;
    cmTransportAddress  address;
    RvChar              callId[16];
    RvUint8             encodedBuffer[256];
    int                 encodedLen;

    /*get callSignallingAddress of GK*/
    cmGetParam(mgr->hApp, cmParamLocalCallSignalAddress, 0, &addrLen, (RvChar *)&address);
    addrNodeId = pvtAddRoot(hVal, cmGetSynTreeByRootName(mgr->hApp, "addr"), 0, NULL);
    cmTransportAddress2Vt(hVal, &address, addrNodeId);
    /*get call identifier*/
    cmCallGetParam(hsCall, cmParamCallID, 0, &callIdLen, callId);

    /*Create IncomingCallIndication node*/
    inCallIndNodeId = pvtAddRoot(hVal, mgr->IncomingCallIndicationSyn, 0, NULL);
    /*Add callSignallingAddress*/
    tmpNodeId = pvtAdd(hVal, inCallIndNodeId, __q931(callSignallingAddress), 0, NULL, NULL);
    pvtMoveTree(hVal, tmpNodeId, addrNodeId);
    /*add callID*/
    __pvtBuildByFieldIds(tmpNodeId, hVal, inCallIndNodeId, { _q931(callID) _q931(guid) LAST_TOKEN}, callIdLen, callId);

    /* print the message before sending */
    RvLogDebug(&mgr->log, (&mgr->log,
        "addIncomingCallIndication: Adding message to outgoing SCI:"));
    pvtPrintInternal(hVal, inCallIndNodeId, &mgr->log, 2);

    /*perEncode IncomingCallIndication*/
    cmEmEncode(hVal, inCallIndNodeId, encodedBuffer, 256, &encodedLen);
    pvtDelete(hVal, inCallIndNodeId);

    /*add IncomingCallIndication*/
    SCINodeId = pvtGetChild2(hVal, cmGetProperty((HPROTOCOL)hsRas), __q931(request), __q931(serviceControlIndication));

    /*create genericData node*/
    genericDataNodeId = pvtAdd(hVal, SCINodeId, __q931(genericData), 0, NULL, NULL);
    /*add sequence*/
    genericDataNodeId = pvtAdd(hVal, genericDataNodeId, __nul(0), 0, NULL, NULL);

    /*set id (standart, RV_H323_FWNAT_SIGNALLING_TRAVERSAL_IDENTIFIER_VALUE)*/
    __pvtBuildByFieldIds(tmpNodeId, hVal, genericDataNodeId, { _q931(id) _q931(standard) LAST_TOKEN }, RV_H323_FWNAT_SIGNALLING_TRAVERSAL_IDENTIFIER_VALUE, NULL);

    /*create parameters node*/
    __pvtBuildByFieldIds(parameterNodeId, hVal, genericDataNodeId, { _q931(parameters) LAST_TOKEN}, 0, NULL);
    /*add sequence*/
    parameterNodeId = pvtAdd(hVal, parameterNodeId, __nul(0), 0, NULL, NULL);

    /*set id (standard, RV_H323_FWNAT_INCOMING_CALL_INDICATION_IDENTIFIER_VALUE)*/
    __pvtBuildByFieldIds(tmpNodeId, hVal, parameterNodeId, { _q931(id) _q931(standard) LAST_TOKEN }, RV_H323_FWNAT_INCOMING_CALL_INDICATION_IDENTIFIER_VALUE, NULL);

    /*create content*/
    __pvtBuildByFieldIds(tmpNodeId, hVal, parameterNodeId, { _q931(content) _q931(raw) LAST_TOKEN}, encodedLen, (char *)encodedBuffer);

}

/********************************************************************************
 * Function Name: getIncomingCallIndicationFromMessage
 * Description:  Gets the IncomingCallIndication from a SCI message
 *
 * Input:   mgr             - FW/NAT manager.
 *          messageNodeId   - The serviceControlIndication node id.
 * Output:  IncomingCallIndication node.
 * Return:  An IncomingCallIndication node decoded.
 ********************************************************************************/
RvPvtNodeId getIncomingCallIndicationFromMessage(IN FwNatMgr *mgr,
                                                 IN RvPvtNodeId messageNodeId,
                                                 OUT RvPvtNodeId *incomingCallIndicationNode)
{
    HPVT            hVal =  cmGetValTree(mgr->hApp);
    RvPstFieldId    fieldId;
    RvInt32         encodedLen, decoded;
    RvChar          encodedBuffer[256];
    RvPvtNodeId     genericDataNodeId, idNodeId, parameterNodeId, inCallIndNodeId;
    RvInt32         idValue;

    genericDataNodeId = pvtGetChild(hVal, messageNodeId, __q931(genericData), NULL);

    if (!RV_PVT_NODEID_IS_VALID(genericDataNodeId))
        return RV_PVT_INVALID_NODEID;

    genericDataNodeId = pvtChild(hVal, genericDataNodeId);
    if (!RV_PVT_NODEID_IS_VALID(genericDataNodeId))
        return RV_PVT_INVALID_NODEID;

    __pvtGetByFieldIds(idNodeId, hVal, genericDataNodeId, { _q931(id) _q931(standard) LAST_TOKEN }, &fieldId, &idValue, NULL);

    if (idNodeId == RV_PVT_INVALID_NODEID || idValue != RV_H323_FWNAT_SIGNALLING_TRAVERSAL_IDENTIFIER_VALUE)
        return RV_PVT_INVALID_NODEID;

    __pvtGetByFieldIds(parameterNodeId, hVal, genericDataNodeId, { _q931(parameters) LAST_TOKEN }, &fieldId, NULL, NULL);
    if (!RV_PVT_NODEID_IS_VALID(parameterNodeId))
        return RV_PVT_INVALID_NODEID;

    parameterNodeId = pvtChild(hVal, parameterNodeId);
    if (!RV_PVT_NODEID_IS_VALID(parameterNodeId))
        return RV_PVT_INVALID_NODEID;

    __pvtGetByFieldIds(inCallIndNodeId, hVal, parameterNodeId, { _q931(content) _q931(raw) LAST_TOKEN }, &fieldId, &encodedLen, NULL);
    if (!RV_PVT_NODEID_IS_VALID(inCallIndNodeId))
        return RV_PVT_INVALID_NODEID;

    pvtGetString(hVal, inCallIndNodeId, encodedLen, encodedBuffer);

    cmEmDecode(hVal, *incomingCallIndicationNode, (RvUint8 *)encodedBuffer, encodedLen, &decoded);

    /* print the message before processing */
    RvLogDebug(&mgr->log, (&mgr->log,
        "getIncomingCallIndicationFromMessage: Decoded from incoming SCI:"));
    pvtPrintInternal(hVal, *incomingCallIndicationNode, &mgr->log, 2);

    return *incomingCallIndicationNode;
}


/********************************************************************************
 * Function Name: openReverseCallTimeout
 * Description:  TimeOut callback for the GK after sending SCI. This callback
 *               will be invoked within 5 seconds after sending the SCI if the
 *               facility message from the endpoint was not received yet.
 *
 * Input:   context     -   The FwNatConn on which the timeOut occurred.
 * Output:  None.
 * Return:  RV_FALSE.
 ********************************************************************************/
RvBool openReverseCallTimeout(IN void* context)
{
    FwNatConn *fnConn = (FwNatConn *)context;
    FwNatMgr *mgr = (FwNatMgr *)emaGetInstance((EMAElement)fnConn);

    if (emaLock((EMAElement)fnConn))
    {
        RvH323TimerClear(((cmElem *)mgr->hApp)->hTimers, &fnConn->timer);

        /*notify the application that the connection removed*/
        if (mgr->callbacks.fwNatSetByCallEv != NULL)
        {
            HAPPCALL haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)fnConn->hCall);
            mgr->callbacks.fwNatSetByCallEv(mgr->hApp, (HH323FWNATMGR)mgr,
                fnConn->hCall, haCall, (HH323FWNATCONN)NULL, 0);
        }
        /*remove the connection*/
        emaDelete((EMAElement)fnConn);
        emaUnlock((EMAElement)fnConn);

        /*drop the call*/
        cmCallDrop(fnConn->hCall);
    }

    return RV_FALSE;
}


/********************************************************************************
 * Function Name: closeEncapsulationConnection
 * Description:  TimeOut callback for the GK after sending UCF. This callback
 *               will be invoked to close the encapsulation connection, to avoid
 *               closing it from within a callback.
 *
 * Input:   context     -   The FwNatConn on which the timeOut occurred.
 * Output:  None.
 * Return:  RV_FALSE.
 ********************************************************************************/
RvBool closeEncapsulationConnection(IN void* context)
{
    FwNatConn *fnConn = (FwNatConn *)context;
    if (emaLock((EMAElement)fnConn))
    {
        FwNatMgr *mgr = (FwNatMgr *)emaGetInstance((EMAElement)fnConn);

        RvH323TimerClear(((cmElem *)mgr->hApp)->hTimers, &fnConn->timer);
        if (fnConn->hashLoc != NULL)
            hashDelete(mgr->hAddrHash, fnConn->hashLoc);
        cmProtConnClose(mgr->hApp, fnConn->protConn);
        emaDelete((EMAElement)fnConn);
        emaUnlock((EMAElement)fnConn);
    }
    return RV_FALSE;
}


/********************************************************************************
 * Function Name: outConn4incomingCallConnect
 * Description:  Handles the connect event of the tcp connection for an incoming call
 *               (after SCI with IncomingCallIndication from GK).
 * Input:   context     -   The connection handle in the stack.
 * Output:  None.
 * Return:  None.
 ********************************************************************************/
void outConn4incomingCallConnect(void * tpkt, int event, int length, int error, void * context)
{
    FwNatConn * fnConn = (FwNatConn *)context;
    HPROTCONN   hConn = fnConn->protConn;
    FwNatMgr *  mgr = NULL;
    RvStatus    status = RV_OK;

    RV_UNUSED_ARG(error);
    RV_UNUSED_ARG(event);
    RV_UNUSED_ARG(tpkt);
    RV_UNUSED_ARG(length);

     /* basic NULL pointer checks */
    if (fnConn == NULL)
        return;
    mgr = (FwNatMgr *)emaGetInstance((EMAElement)fnConn);
    if (mgr == NULL)
        return;

    /* sanity check */
    if (!(fnConn->protConn == hConn) || !(fnConn->connType == RvH323FwNatConnTypeReverseCallIncoming))
    {
        RvLogExcep(&mgr->log, (&mgr->log,
            "outConn4incomingCallConnect: got event on unknown FNConn %p (type: %d)", fnConn, fnConn->connType));
        status = RV_ERROR_UNKNOWN;
    }
    else if (error != 0)
    {
        /* report the error */
        RvLogError(&mgr->log, (&mgr->log,
            "outConn4incomingCallConnect: %p got error from network error = %d", fnConn, error));
        status = RV_ERROR_UNKNOWN;
    }

    if (status == RV_OK)
    {
        cmProtConnState eConnState;
        /* get the current stage of the connection */
        cmProtConnGetParam(mgr->hApp, hConn, cmProtConnParamState, (RvInt32*)&eConnState, NULL);

        if ((eConnState != cmProtConnStateConnecting) || ((event & RvSelectConnect) == 0))
        {
            RvLogError(&mgr->log, (&mgr->log,
                "outConn4incomingCallConnect: %p (stage: %d) received event %x", fnConn, eConnState, event));
            return;
        }

        /*set connection as connected*/
        status = cmProtConnSetParam(mgr->hApp,
                                    hConn,
                                    cmProtConnParamState,
                                    (RvInt32)cmProtConnStateConnected,
                                    NULL);

        if (status == RV_OK)
            /*set the incoming TCP connection to outgoing*/
            status = cmProtConnSetParam(mgr->hApp,
                                        hConn,
                                        cmProtConnParamIncoming,
                                        (RvInt32)RV_TRUE,
                                        NULL);

        if (status == RV_OK)
        {
            /*build facility message with callID saved in the fnConn and undefinedReason*/
            buildFacilityMessageWithCallID(mgr, fnConn->CID, &mgr->facilityMessageWithCID);
            /*send facility message*/
            status = cmProtocolSendMessage(mgr->hApp, hConn, mgr->facilityMessageWithCID);

            if (status == RV_OK)
            {
                /*relinquish control of the host to the stack*/
                status = cmProtConnRelinquish(mgr->hApp, hConn);
                cmProtConnHandleEventByDefault(mgr->hApp, hConn, event);
            }
        }
    }

    if (status != RV_OK)
    {
        /*notify the application about the protConn*/
        if (mgr->callbacks.fwNatSetProtConnHandleEv != NULL)
            mgr->callbacks.fwNatSetProtConnHandleEv(fnConn->protConn, NULL);

        /*close the connection*/
        cmProtConnClose(mgr->hApp, fnConn->protConn);
        if (emaLock((EMAElement)fnConn))
        {
            emaDelete((EMAElement)fnConn);
            emaUnlock((EMAElement)fnConn);
        }
    }
}


/********************************************************************************
 * Function Name: buildFacilityMessageWithCallID
 * Description:  Builds a facility message with the given callIdentifier with
 *               undefinedReason reason.
 * Input:   mgr                     -   The H460.18 manager.
 *          CID                     -   The callIdentifier.
 * Output:  facilityMessageNodeId   -   The node id of the facility message to send.
 * Return:  None.
 ********************************************************************************/
void buildFacilityMessageWithCallID(IN   FwNatMgr *      mgr,
                                    IN   RvUint8 *       CID,
                                    OUT  RvPvtNodeId *   facilityMessageNodeId)
{
    RvPvtNodeId tmpNodeId, messageNodeId, facilityNodeId, userUserNodeId;
    RvInt32     callIdLen = 16;
    HPVT        hVal = cmGetValTree(mgr->hApp);

    if (*facilityMessageNodeId == RV_PVT_INVALID_NODEID)
    {
        char OID[10];
        int length;

        length=utlEncodeOID(sizeof(OID), OID, H225_PROTOCOL_IDENTIFIER);

        *facilityMessageNodeId = pvtAddRoot(hVal, cmGetSynTreeByRootName(mgr->hApp, "q931"), 0, NULL);

        pvtAdd(hVal ,*facilityMessageNodeId ,__q931(protocolDiscriminator),8,NULL,NULL);

        __pvtBuildByFieldIds(tmpNodeId, hVal, *facilityMessageNodeId,
            { _q931(callReferenceValue) _q931(twoBytes) LAST_TOKEN }, 0, NULL);

        __pvtBuildByFieldIds(messageNodeId , hVal, *facilityMessageNodeId, { _q931(message) LAST_TOKEN }, 0, NULL);

        __pvtBuildByFieldIds(tmpNodeId , hVal, messageNodeId, { _q931(facility) _q931(facility) LAST_TOKEN }, 0, NULL);

        __pvtBuildByFieldIds(userUserNodeId , hVal, messageNodeId, { _q931(facility) _q931(userUser) LAST_TOKEN }, 0, NULL);

        pvtAdd(hVal ,userUserNodeId ,__q931(protocolDiscriminator), 5, NULL, NULL);

        __pvtBuildByFieldIds(facilityNodeId, hVal, userUserNodeId, { _q931(h323_UserInformation)  _q931(h323_uu_pdu) _q931(h323_message_body)  _q931(facility) LAST_TOKEN }, 0, NULL);

        pvtAdd(hVal, facilityNodeId, __q931(protocolIdentifier), length, OID, NULL);

        pvtAdd(hVal, facilityNodeId, __q931(multipleCalls), RV_FALSE, NULL, NULL);

        pvtAdd(hVal, facilityNodeId, __q931(maintainConnection), RV_FALSE, NULL, NULL);

        __pvtBuildByFieldIds(tmpNodeId, hVal, facilityNodeId, { _q931(reason)  _q931(undefinedReason) LAST_TOKEN }, 0, NULL);
    }
    else
    {
        __pvtGetNodeIdByFieldIds(facilityNodeId, hVal, *facilityMessageNodeId,
                { _q931(message) _q931(facility) _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body)  _q931(facility) LAST_TOKEN})
    }

    __pvtBuildByFieldIds(tmpNodeId, hVal, facilityNodeId, { _q931(callIdentifier)  _q931(guid) LAST_TOKEN }, callIdLen, (RvChar *)CID);
}


/********************************************************************************
 * Function Name: h245MultiServerAcceptHandler
 * Description:  handle the accept event on the H.245 Multi-Server connection.
 ********************************************************************************/
void h245MultiServerAcceptHandler(void * tpkt, int event, int length, int error, void*context)
{
    FwNatConn * fnConn = context;
    HPROTCONN hConn = fnConn->protConn;
    FwNatMgr * mgr;

    RV_UNUSED_ARG(length);
    RV_UNUSED_ARG(tpkt);

    /* basic NULL pointer checks */
    if (!emaLock((EMAElement)fnConn))
        return;
    mgr = (FwNatMgr *)emaGetInstance((EMAElement)fnConn);
    if (mgr == NULL)
    {
        emaUnlock((EMAElement)fnConn);
        return;
    }

    /* sanity checks */
    if (fnConn->connType != RvH323FwNatConnTypeH245MultiHost)
    {
        /* very bad, but don't clean the whole thing, we may still get correct events */
        RvLogExcep(&mgr->log, (&mgr->log,
            "h245MultiServerAcceptHandler: got event on unknown FNConn %p (type: %d)", fnConn, fnConn->connType));
        mgr->callbacks.fwNatSetProtConnHandleEv(fnConn->protConn, NULL);
        cmProtConnClose(mgr->hApp, fnConn->protConn);
        emaDelete((EMAElement)fnConn);
        emaUnlock((EMAElement)fnConn);
        return;
    }

    /* check for network errors */
    if (error != 0)
    {
        /* report the error */
        RvLogError(&mgr->log, (&mgr->log,
            "encapEventHandler: %p got error from network error = %d", fnConn, error));
        emaUnlock((EMAElement)fnConn);
        return;
    }

    if ((event & RvSelectAccept) != 0)
    {
        HPROTCONN newHost;
        RvStatus status;
        /* create a new host*/
        status = cmProtConnNew(mgr->hApp, cmProtocolH245, RV_FALSE, NULL, NULL, NULL, NULL, &newHost);
        if (status == RV_OK)
        {
            cmProtConnSetParam(mgr->hApp, newHost, cmProtConnParamTpkt, sizeof(tpkt), (RvUint8*)&tpkt);
            cmProtConnSetParam(mgr->hApp, newHost, cmProtConnParamState, cmProtConnStateListening, NULL);
            cmProtConnRelinquish(mgr->hApp, newHost);
        }
        else
        {
            emaDelete((EMAElement)tpkt);
        }
    }
    if ((event & RvSelectRead) != 0)
    {
        /* first message received. we should read the message, and meet it again in the hook. */
        cmProtConnHandleEventByDefault(mgr->hApp, hConn, event);
    }
    emaUnlock((EMAElement)fnConn);
}


/********************************************************************************
 * Function Name: buildGenericIndicationMessage
 * Description:  Builds a H.460.18 generic indication message with the given
 *               callIdentifier and answerCall values.
 * Input:   mgr         - The H460.18 manager.
 *          CID         - The callIdentifier.
 *          bIncoming   - is the call an incoming one?
 * Output:  None.
 * Return:  The node id of the generic indication message.
 ********************************************************************************/
RvPvtNodeId buildGenericIndicationMessage(IN  FwNatMgr *    mgr,
                                          IN  RvUint8 *     CID,
                                          IN  RvBool        bIncoming)
{
    RvPvtNodeId messageNodeId, tempNodeId, contentNodeId, genParamNodeId;
    HPVT        hVal = cmGetValTree(mgr->hApp);
    char        OID[10];
    int         length;

    cmBuildMultimediaSystemControlMessage(mgr->hApp, h245giGenericIndication, 0, &messageNodeId, &tempNodeId);

    contentNodeId = pvtAdd(hVal, tempNodeId, __h245(messageContent), 1, NULL, NULL);
    pvtAdd(hVal, tempNodeId, __h245(subMessageIdentifier), 1, NULL, NULL);

    length = utlEncodeOID(sizeof(OID), OID, H460_18_PROTOCOL_IDENTIFIER);
    __pvtBuildByFieldIds(tempNodeId, hVal, tempNodeId,
        { _h245(messageIdentifier) _h245(standard) LAST_TOKEN}, length, (char *)OID);

    genParamNodeId = pvtAdd(hVal, contentNodeId, __nul(0), 0, NULL, NULL);
    __pvtBuildByFieldIds(tempNodeId, hVal, genParamNodeId,
        { _h245(parameterIdentifier) _h245(standard) LAST_TOKEN }, 1, NULL);
    __pvtBuildByFieldIds(tempNodeId, hVal, genParamNodeId,
        { _h245(parameterValue) _h245(octetString) LAST_TOKEN}, 16, (char *)CID);

    if (bIncoming)
    {
        genParamNodeId = pvtAdd(hVal, contentNodeId, __nul(0), 0, NULL, NULL);
        __pvtBuildByFieldIds(tempNodeId, hVal, genParamNodeId,
            { _h245(parameterIdentifier) _h245(standard) LAST_TOKEN }, 2, NULL);
        __pvtBuildByFieldIds(tempNodeId, hVal, genParamNodeId,
            { _h245(parameterValue) _h245(logical) LAST_TOKEN }, 0, NULL);
    }
    return messageNodeId;
}


/********************************************************************************
 * Function Name: getGenericIndicationMessage
 * Description:  Checks if this is a H.460.18 generic indication message and gets
 *               the call according to the callIdentifier and answerCall values.
 * Input:   mgr         - The H460.18 manager.
 *          message     - An H.245 message
 *          bIncoming   - is the call an incoming one?
 * Output:  hsCall      - The call referred to by the generic message, if found.
 * Return:  RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus getGenericIndicationMessage(IN  FwNatMgr *    mgr,
                                     IN  RvPvtNodeId   message,
                                     OUT HCALL *       hsCall)
{
    RvPvtNodeId tempNodeId, contentNodeId, genParamNodeId;
    HPVT        hVal = cmGetValTree(mgr->hApp);
    RvChar      OID[10], strVal[32];
    RvInt32     oidLen, intVal, numParams = 0;
    RvBool      bAnswerCall = RV_FALSE;

    /* is this the correct message type? */
    __pvtGetByFieldIds(tempNodeId, hVal, message, { _h245(indication) _h245(genericIndication) LAST_TOKEN }, NULL, NULL, NULL);

    if (!RV_PVT_NODEID_IS_VALID(tempNodeId))
        return RV_ERROR_UNKNOWN;

    /* try to match the OID */
    oidLen = utlEncodeOID(sizeof(OID), OID, H460_18_PROTOCOL_IDENTIFIER);
    __pvtGetByFieldIds(contentNodeId, hVal, tempNodeId,
        { _h245(messageIdentifier) _h245(standard) LAST_TOKEN }, NULL, &intVal, NULL);
    if (intVal != oidLen)
        return RV_ERROR_UNKNOWN;

    pvtGetString(hVal, contentNodeId, sizeof(strVal), (char*)strVal);
    if (memcmp(strVal, OID, oidLen) != 0)
        return RV_ERROR_UNKNOWN;

    /* match the sub ID */
    pvtGetChildByFieldId(hVal, tempNodeId, __h245(subMessageIdentifier), &intVal, NULL);
    if (intVal != 1)
        return RV_ERROR_UNKNOWN;

    /* get the generic parameter */
    __pvtGetByFieldIds(genParamNodeId, hVal, tempNodeId, { _h245(messageContent) _nul(1) LAST_TOKEN }, NULL, NULL, NULL);
    while (RV_PVT_NODEID_IS_VALID(genParamNodeId) && (numParams < 2))
    {
        __pvtGetByFieldIds(tempNodeId, hVal, genParamNodeId,
            { _h245(parameterIdentifier) _h245(standard) LAST_TOKEN }, NULL, &intVal, NULL);
        if (RV_PVT_NODEID_IS_VALID(tempNodeId))
        {
            if (intVal == 1)
            {
                /* call identifier */
                __pvtGetByFieldIds(tempNodeId, hVal, genParamNodeId,
                    { _h245(parameterValue) _h245(octetString) LAST_TOKEN }, NULL, NULL, NULL);
                pvtGetString(hVal, tempNodeId, sizeof(strVal), (char*)strVal);
                numParams++;
            }
            else if (intVal == 2)
            {
                /* answering side */
                bAnswerCall = RV_TRUE;
                numParams++;
            }
        }
        genParamNodeId = pvtBrother(hVal, genParamNodeId);
    }

    /* did we get any? */
    if (numParams == 0)
        return RV_ERROR_UNKNOWN;

    /* find the call */
    if (cmCallCatGetCall(mgr->hApp, strVal, !bAnswerCall, hsCall) != RV_OK)
        return RV_ERROR_UNKNOWN;

    return RV_OK;
}


/********************************************************************************
 * Function Name: setH245ConnectionForCall
 * Description:  Sets the connections accepted from the H.245 multy-server as
 *               listening connection for a call.
 * Input:   mgr         - The H460.18 manager.
 *          hsCall      - Call to set the accepted connection for
 *          hConn       - The connection to set
 *          fnServ      - FW/NAT H.245 server connection
 * Output:  None.
 * Return:  RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus setH245ConnectionForCall(IN  FwNatMgr *    mgr,
                                  IN  HCALL         hsCall,
                                  IN  HPROTCONN     hConn,
                                  IN  FwNatConn *   fnServ)
{
    HPROTCONN hCurConn;

    /* If there already is a connected connection, forget about it */
    hCurConn = cmGetTpktChanHandle(hsCall, cmH245TpktChannel);
    if (hCurConn != NULL)
    {
        cmProtConnState eConnState;
        cmProtConnGetParam(mgr->hApp, hCurConn, cmProtConnParamState, (RvInt32*)&eConnState, NULL);
        if (eConnState == cmProtConnStateConnected)
        {
            /* AL NA TAAKOR NATUA */
            return RV_OK;
        }
    }
    cmCallSetConnection(mgr->hApp, hsCall, RV_FALSE, hConn);
    cmProtConnRelinquish(mgr->hApp, hConn);
    cmProtConnHandleEventByDefault(mgr->hApp, hConn, RvSelectAccept);
    fnServ->numRelatedCalls--;
    return RV_OK;
}

/********************************************************************************
 * Function Name: addKeepAliveChannelInfo
 * Description:  Sets the keepAlive parameters in the TraversalParameters node.
 * Input:   hVal                    - The val tree handle of the stack.
 *          traversalParamsNodeId   - The TraversalParameters node id.
 *          keepAliveAddr           - The keepAliveChannel Address.
 *          keepAlivePayLoadType    - The keepAlivePayLoadType to be added to
 *                                    the TraversalParameters node. Will not
 *                                    be added if a negative value.
 *          keepAliveInterval       - The interval in seconds to send keepAlive messages.
 *                                    Will not be added to the TraversalParameters node
 *                                    if negative will be set to 5 if given value
 *                                    less than 5.
 * Output:  None.
 * Return:  RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus addKeepAliveChannelInfo(IN HPVT                    hVal, 
                                 IN RvPvtNodeId             traversalParamsNodeId, 
                                 IN cmTransportAddress *    keepAliveAddr, 
                                 IN RvInt8                  keepAlivePayLoadType, 
                                 IN RvInt16                 keepAliveInterval)
{
    /*add keepAliveChannel to TraversalParameters*/
    RvPvtNodeId addr = pvtAdd(hVal, traversalParamsNodeId, __q931(keepAliveChannel), 0, NULL, NULL);
    if (!RV_PVT_NODEID_IS_VALID(addr))
        return RV_ERROR_UNKNOWN;

     cmTransportAddress2Vt(hVal, keepAliveAddr, addr);

     if (keepAlivePayLoadType >= 0)
        pvtAdd(hVal, traversalParamsNodeId, __q931(keepAlivePayloadType), (RvInt32)keepAlivePayLoadType, NULL, NULL);


     if (keepAliveInterval > 0)
        pvtAdd(hVal, traversalParamsNodeId, __q931(keepAliveInterval), (RvInt32)keepAliveInterval, NULL, NULL);


     return RV_OK;
}

/********************************************************************************
 * Function Name: addMultiplexInfo
 * Description:  Add multiplex information in traversalParameters node.
 * Input:   hVal                    - The val tree handle of the stack.
 *          fieldId                 - fieldId of the message where multiplexed
 *                                    info is being added.
 *          traversalParamsNodeId   - The TraversalParameters node id.
 *          multiplexID             - The multiplexID for the channel.
 *          rtpMuxAddr              - The rtp Address to send media to.
 *          rctpMuxAddr             - rctp Address to send control media to.
 * Output:  None.
 * Return:  RV_OK on success, negative on failure
 ********************************************************************************/
void addMultiplexInfo(IN HPVT                    hVal,
                      IN RvPstFieldId            fieldId,
                      IN RvPvtNodeId             traversalParamsNodeId, 
                      IN RvUint32                multiplexID, 
                      IN cmTransportAddress *    rtpMuxAddr, 
                      IN cmTransportAddress *    rctpMuxAddr)
{
    /*add multiplexId to TraversalParameters*/
    pvtAdd(hVal, traversalParamsNodeId, __q931(multiplexID), multiplexID, NULL, NULL);
            
    /*add multiplexedMediaControlChannel to TraversalParameters*/
    if (rctpMuxAddr != NULL)
    {
        RvPvtNodeId rctpMuxAddrNodeId = pvtAdd(hVal, traversalParamsNodeId, __q931(multiplexedMediaControlChannel), 0, NULL, NULL);
        cmTransportAddress2Vt(hVal, rctpMuxAddr, rctpMuxAddrNodeId);
    }
    /*if OLC response - add multiplexedMediaChannel to TraversalParameters*/
    if (fieldId == __h245(openLogicalChannelAck) && rtpMuxAddr != NULL)
    {
        RvPvtNodeId rtpMuxAddrNodeId = pvtAdd(hVal, traversalParamsNodeId, __q931(multiplexedMediaChannel), 0, NULL, NULL);
        cmTransportAddress2Vt(hVal, rtpMuxAddr, rtpMuxAddrNodeId);
    }

}

/********************************************************************************
 * Function Name: encodeAndAddTraversalParams
 * Description:  Encode traversalParameters and add to OLC/OLCA.
 * Input:   hVal                    - The val tree handle of the stack.
 *          olcMsg                  - The OLC/OLCA node id.
 *          traversalParamsNodeId   - The TraversalParameters node id.
 * Output:  None.
 * Return:  RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus encodeAndAddTraversalParams(IN HPVT            hVal, 
                                     IN RvPvtNodeId     olcMsg, 
                                     IN RvPvtNodeId     traversalParamsNodeId)
{
    RvUint8             encodedBuffer[256];
    int                 encodedLen;
    RvPvtNodeId         genericInfoNodeId, msgId, paramId, contentId, paramValId;
    int                 length;
    char                OID[64];

     /*first remove an existing traversal parameters node*/
     removeTraversalParams(hVal, olcMsg);

    /*create/get genericInformation nodeId*/
    genericInfoNodeId = pvtAdd(hVal, olcMsg, __h245(genericInformation), 0, NULL, NULL);
    if (!RV_PVT_NODEID_IS_VALID(genericInfoNodeId))
        return RV_ERROR_UNKNOWN;

    /*add sequence*/
    genericInfoNodeId = pvtAdd(hVal, genericInfoNodeId, __nul(0), 0, NULL, NULL);
    if (!RV_PVT_NODEID_IS_VALID(genericInfoNodeId))
        return RV_ERROR_UNKNOWN;

    /*add messageIdentifier*/
    length = utlEncodeOID(sizeof(OID), OID, H460_19_PROTOCOL_IDENTIFIER);
    __pvtBuildByFieldIds(msgId, hVal, genericInfoNodeId,
        { _h245(messageIdentifier) _h245(standard) LAST_TOKEN}, length, (char *)OID);
    if (!RV_PVT_NODEID_IS_VALID(msgId))
        return RV_ERROR_UNKNOWN;

    /*add messageContent*/
    contentId = pvtAdd(hVal, genericInfoNodeId, __h245(messageContent), 0, NULL, NULL);
    if (!RV_PVT_NODEID_IS_VALID(contentId))
        return RV_ERROR_UNKNOWN;

    /*add sequence*/
    contentId = pvtAdd(hVal, contentId, __nul(0), 0, NULL, NULL);
    if (!RV_PVT_NODEID_IS_VALID(contentId))
        return RV_ERROR_UNKNOWN;

    /*add ParameterIdentifier*/
    __pvtBuildByFieldIds(paramId, hVal, contentId,
        { _h245(parameterIdentifier) _h245(standard) LAST_TOKEN }, 1, NULL);
    if (!RV_PVT_NODEID_IS_VALID(paramId))
        return RV_ERROR_UNKNOWN;

    /*encode TraversalParameters and add it to message*/
    cmEmEncode(hVal, traversalParamsNodeId, encodedBuffer, 256, &encodedLen);
    __pvtBuildByFieldIds(paramValId, hVal, contentId,
        { _h245(parameterValue) _h245(octetString) LAST_TOKEN}, encodedLen, (char *)encodedBuffer);
    if (!RV_PVT_NODEID_IS_VALID(paramValId))
        return RV_ERROR_UNKNOWN;

    return RV_OK;
}

/********************************************************************************
 * Function Name: decodeAndGetTraversalParams
 * Description:  Retrieves traversalParameters node from OLC/OLCA.
 * Input:   hVal                    - The val tree handle of the stack.
 *          olcMsg                  - The OLC/OLCA node id.
 * Output:  traversalParamsNodeId   - The TraversalParameters node.
 * Return:  None.
 ********************************************************************************/
void decodeAndGetTraversalParams(IN HPVT            hVal, 
                                 IN RvPvtNodeId     olcMsg, 
                                 IN RvPvtNodeId *   traversalParamsNodeId)
{
    RvPvtNodeId genericIfoNodeId, traversalParamsMsg;

    /*get genericInformation node*/
    genericIfoNodeId = pvtGetChild(hVal, olcMsg, __h245(genericInformation), NULL);

    if (!RV_PVT_NODEID_IS_VALID(genericIfoNodeId))
    {
        pvtDelete(hVal, *traversalParamsNodeId);
        *traversalParamsNodeId = RV_PVT_INVALID_NODEID;
        return;
    }

    /*find traversalParameters*/
    traversalParamsMsg = pvtChild(hVal, genericIfoNodeId);
    if (traversalParamsMsg == RV_PVT_INVALID_NODEID)
    {
        pvtDelete(hVal, *traversalParamsNodeId);
        *traversalParamsNodeId = RV_PVT_INVALID_NODEID;
        return;
    }
    while (traversalParamsMsg != RV_PVT_INVALID_NODEID)
    {
        RvPvtNodeId paramIdNodeId;
        RvChar      OID[10], strVal[10];
        RvInt32     oidLen, intVal;

         /* try to match the OID */
        oidLen = utlEncodeOID(sizeof(OID), OID, H460_19_PROTOCOL_IDENTIFIER);
        __pvtGetByFieldIds(paramIdNodeId, hVal, traversalParamsMsg,
        { _h245(messageIdentifier) _h245(standard) LAST_TOKEN }, NULL, &intVal, NULL);
        if (intVal == oidLen)
        {
            pvtGetString(hVal, paramIdNodeId, sizeof(strVal), (char*)strVal);
            if (memcmp(strVal, OID, oidLen) == 0)
                /*traversalParams found*/
            {
                RvPvtNodeId contentNodeId, paramVal, paramId; 

                contentNodeId = pvtGetChild(hVal, traversalParamsMsg, __h245(messageContent), NULL);
                contentNodeId = pvtChild(hVal, contentNodeId);

                /*get parameterIdentifier*/
                __pvtGetByFieldIds(paramId, hVal, contentNodeId, { _h245(parameterIdentifier) _h245(standard) LAST_TOKEN }, NULL, &intVal, NULL);
                if (intVal == 1)
                {
                    RvInt32 encodedLen;

                    __pvtGetByFieldIds(paramVal, hVal, contentNodeId, { _h245(parameterValue) _h245(octetString) LAST_TOKEN }, NULL, &encodedLen, NULL);
                    if (encodedLen > 0)
                    {
                        RvInt32         decoded;
                        RvChar          encodedBuffer[256];

                        pvtGetString(hVal, paramVal, encodedLen, encodedBuffer);

                        cmEmDecode(hVal, *traversalParamsNodeId, (RvUint8 *)encodedBuffer, encodedLen, &decoded); 
                    }
                }
                break;
            }
        }
        traversalParamsMsg = pvtBrother(hVal, traversalParamsMsg);
    }
}

/********************************************************************************
 * Function Name: getKeepAliveAddr
 * Description:  Retrieves keepAlive address from traverslParameters.
 * Input:   hVal                    - The val tree handle of the stack.
 *          traversalParamsNodeId   - The traversalParameters node id.
 * Output:  addr                    - The keepAlive address.
 *          keepAlivePayLoadType    - The keepAlivePayLoadType value.
 *          keepAliveInterval       - The keepAliveInterval value.
 * Return:  None.
 ********************************************************************************/
void getKeepAliveChannelInfo(IN    HPVT                 hVal, 
                             IN    RvPvtNodeId          traversalParamsNodeId, 
                             OUT   cmTransportAddress * addr,
                             OUT   RvInt8 *             keepAlivePayLoadType, 
                             OUT   RvInt16 *            keepAliveInterval)
{
   RvPvtNodeId keepAliveNode;
   RvInt32 tmpValue;

   *keepAlivePayLoadType = -1;
   *keepAliveInterval = 0;

   keepAliveNode = pvtGetChild(hVal, traversalParamsNodeId, __q931(keepAliveChannel), NULL);
   if (RV_PVT_NODEID_IS_VALID(keepAliveNode))
        cmVt2TransportAddress(hVal, keepAliveNode, addr);

   keepAliveNode = pvtGetChildByFieldId(hVal, 
                                        traversalParamsNodeId, 
                                        __q931(keepAlivePayloadType),
                                        &tmpValue,
                                        NULL);
   if (RV_PVT_NODEID_IS_VALID(keepAliveNode))
        *keepAlivePayLoadType = (RvInt8)tmpValue;

   keepAliveNode = pvtGetChildByFieldId(hVal, 
                                        traversalParamsNodeId, 
                                        __q931(keepAliveInterval),
                                        &tmpValue,
                                        NULL);

   if (RV_PVT_NODEID_IS_VALID(keepAliveNode))
        *keepAliveInterval = (RvInt8)tmpValue;
                                        
}

/********************************************************************************
 * Function Name: getMultiplexInfo
 * Description:  Retrieves multiplex information from traversalParameters.
 * Input:   hVal                    - The val tree handle of the stack.
 *          traversalParamsNodeId   - The traversalParameters node id.
 * Output:  multiplexId             - the multiplexID.  
 *          rtpMuxAddr              - The multiplexedMediaChannel.
 *          rctpMuxAddr             - The multiplexedMediaControlChannel.
 * Return:  None.
 ********************************************************************************/
void getMultiplexInfo(IN    HPVT                 hVal,
                      IN    RvPvtNodeId          traversalParamsNodeId,
                      OUT   RvUint32 *           multiplexId,
                      OUT   cmTransportAddress * rtpMuxAddr,
                      OUT   cmTransportAddress * rctpMuxAddr)
{
    RvPvtNodeId multiplexInfo;
    RvInt32 tmpValue;

    /*extract multiplexedId*/
    multiplexInfo = pvtGetChildByFieldId(hVal, 
                                       traversalParamsNodeId, 
                                       __q931(multiplexID),
                                        &tmpValue,
                                        NULL);
    if (RV_PVT_NODEID_IS_VALID(multiplexInfo))
        *multiplexId = (RvUint32)tmpValue;

    /*extract rctp address*/
    multiplexInfo = pvtGetChild(hVal, traversalParamsNodeId, __q931(multiplexedMediaControlChannel), NULL);
    if (RV_PVT_NODEID_IS_VALID(multiplexInfo))
        cmVt2TransportAddress(hVal, multiplexInfo, rctpMuxAddr);

    multiplexInfo = pvtGetChild(hVal, traversalParamsNodeId, __q931(multiplexedMediaChannel), NULL);
    if (RV_PVT_NODEID_IS_VALID(multiplexInfo))
        cmVt2TransportAddress(hVal, multiplexInfo, rtpMuxAddr);
}

/********************************************************************************
 * Function Name: removeTraversalParams
 * Description:  Remove traversal parameters from OLC/OLCA message.
 * Input:   hVal                    - The val tree handle of the stack.
 *          message                 - The OLC/OLCA node id.
 * Output:  None.
 * Return:  None.
 ********************************************************************************/
void removeTraversalParams(
           IN HPVT            hVal, 
           IN RvPvtNodeId     message)
{

    RvPvtNodeId         genericInfoNodeId, currentNode;

    genericInfoNodeId = pvtGetChild(hVal, message, __h245(genericInformation), NULL);

    if (!RV_PVT_NODEID_IS_VALID(genericInfoNodeId))
        return;
    
    currentNode = pvtChild(hVal, genericInfoNodeId);

    while (currentNode != RV_PVT_INVALID_NODEID)
    {
        RvPvtNodeId paramIdNodeId;
        RvChar      OID[10], strVal[10];
        RvInt32     oidLen, intVal;

         /* try to match the OID */
        oidLen = utlEncodeOID(sizeof(OID), OID, H460_19_PROTOCOL_IDENTIFIER);
        __pvtGetByFieldIds(paramIdNodeId, hVal, currentNode,
        { _h245(messageIdentifier) _h245(standard) LAST_TOKEN }, NULL, &intVal, NULL);
        if (intVal == oidLen)
        {
            pvtGetString(hVal, paramIdNodeId, sizeof(strVal), (char*)strVal);
            if (memcmp(strVal, OID, oidLen) == 0)
            {
                pvtDelete(hVal, currentNode);
                break;
            }
        }
        currentNode = pvtBrother(hVal, currentNode);
    }
}

#ifdef __cplusplus
}
#endif
