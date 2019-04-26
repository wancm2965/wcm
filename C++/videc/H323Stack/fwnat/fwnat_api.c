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
#include "h32xLog.h"
#include "q931asn1.h"
#include "h245.h"
#include "cmintr.h"
#include "pvaltreeStackApi.h"
#include "h323Version.h"


/********************************************************************************
 * Function Name: RvH323FwNatInitialize
 * Description:  Initialize the H.460.18/19/17 module.
 *
 * Input:   hApp         - Stack application handle.
 *          config       - Configuration to use.
 *          callbacks    - Callbacks to use.
 *          numHosts     - Number of hosts to allocate.
 *          encapMsgSize - Maximum size of an encapsulated message (or 0).
 * Output:  hFwNat       - FwNat module handle.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatInitialize(
    IN  HAPP                        hApp,
    IN  RvH323FwNatConfiguration *  config,
    IN  RvH323FwNatCallbacks *      callbacks,
    IN  RvUint32                    numHosts,
    IN  RvSize_t                    encapMsgSize,
    OUT HH323FWNATMGR *             hFwNat)
{
    FwNatMgr * mgr;
    RvLogMgr * logMgr = H32xLogGet();

    /* Allocate an instance */
    if (RvMemoryAlloc(NULL, sizeof(FwNatMgr), logMgr, (void**)&mgr) != RV_OK)
    {
        return RV_ERROR_OUTOFRESOURCES;
    }
    memset(mgr, 0, sizeof(FwNatMgr));
    mgr->defEncapMsg = RV_PVT_INVALID_NODEID;
    mgr->facilityMessageWithCID = RV_PVT_INVALID_NODEID;

    mgr->logMgr = logMgr;
    if (mgr->logMgr != NULL)
    {
        /* Create a log handle for our use */
        RvLogSourceConstruct(mgr->logMgr, &mgr->log, "FWNAT", "H.460.17/18/19 add-on for firewall/NAT traversal");
    }
    else
    {
        mgr->log = NULL;
    }

    /* set H.460.17 configuration */
    mgr->e17State = RvH323FwNatH46017StateDisabled;
    if (config->e17Config == RvH323FwNatH46017ConfigEncapsulate)
    {
        mgr->e17State = RvH323FwNatH46017StateEnabled;
        RvH323FwNatBuildEncapsulationFacilityMessage(mgr);
    }

    /* set H.460.18 configuration */
    if (config->e18Config == RvH323FwNatH46018ConfigNotUsed)
        mgr->e18State = RvH323FwNatH46018StateNotSupported;
    else if (config->bGatekeeper)
        mgr->e18State = RvH323FwNatH46018StateInitializedGK;
    else
        mgr->e18State = RvH323FwNatH46018StateInitializedEP;

    /* set H.460.19 configuration */
    mgr->e19State = RvH323FwNatH46019StateDisabled;
    if (config->e19Config == RvH323FwNatH46019ConfigClient)
        mgr->e19State = RvH323FwNatH46019StateClient;
    else if (config->e19Config == RvH323FwNatH46019ConfigServer)
        mgr->e19State = RvH323FwNatH46019StateServer;

    /* set H.460.19 media multiplexing mode configuration */
    if (mgr->e19State == RvH323FwNatH46019StateClient)
    {
        mgr->mediaMultiplexMode |= RV_H323_FWNAT_H46019_MULTIPLEX_MODE_TRANSMIT;
        if (config->e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigFull)
            mgr->mediaMultiplexMode |= RV_H323_FWNAT_H46019_MULTIPLEX_MODE_RECEIVE;
    }
    else
    {
        mgr->mediaMultiplexMode = 0;
        if (mgr->e19State == RvH323FwNatH46019StateServer)
        {
            if (config->e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigTransmit ||
                config->e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigFull)
                mgr->mediaMultiplexMode |= RV_H323_FWNAT_H46019_MULTIPLEX_MODE_TRANSMIT;
            if (config->e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigReceive ||
                config->e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigFull)
                mgr->mediaMultiplexMode |= RV_H323_FWNAT_H46019_MULTIPLEX_MODE_RECEIVE;
        }
    }

    mgr->bGatekeeper = config->bGatekeeper;
    mgr->hApp = hApp;
    mgr->IncomingCallIndicationSyn = pstConstruct(cmEmGetQ931Syntax(), "IncomingCallIndication");
    mgr->TraversalParamsSyn = pstConstruct(cmEmGetQ931Syntax(), "TraversalParameters");
    mgr->pGkConn = NULL;

    /* construct the address hash */
    mgr->hAddrHash = hashConstruct(numHosts*2, numHosts,
        hashDefaultFunc, hashDefaultCompare, sizeof(cmTransportAddress), sizeof(FwNatConn*),
        "FwNatAddrHash", logMgr);
    if (mgr->hAddrHash == NULL)
    {
        RvLogError(&mgr->log, (&mgr->log,
            "RvH323FwNatInitialize: could not construct hash"));
        if (mgr->log != NULL)
            RvLogSourceDestruct(&mgr->log);
        RvMemoryFree(mgr, logMgr);
        return RV_ERROR_OUTOFRESOURCES;
    }

    /* construct the array for connections */
    mgr->hFwNatConns = emaConstruct(sizeof(FwNatConn), numHosts, emaNormalLocks,
        "FwNat Conn", 0, NULL, (void*)mgr, logMgr);
    if (mgr->hFwNatConns == NULL)
    {
        RvLogError(&mgr->log, (&mgr->log,
            "RvH323FwNatInitialize: could not construct EMA array"));
        hashDestruct(mgr->hAddrHash);
        if (mgr->log != NULL)
            RvLogSourceDestruct(&mgr->log);
        RvMemoryFree(mgr, logMgr);
        return RV_ERROR_OUTOFRESOURCES;
    }

    /* assign callbacks */
    mgr->callbacks = *callbacks;
    if ((callbacks->fwNatGetProtConnHandleEv == NULL) || (callbacks->fwNatSetProtConnHandleEv == NULL))
    {
        if ((callbacks->fwNatGetProtConnHandleEv != NULL) || (callbacks->fwNatSetProtConnHandleEv != NULL))
        {
            /* one is NULL, but not the other! */
            RvLogError(&mgr->log, (&mgr->log,
                "RvH323FwNatInitialize: One of Set/Get ProtConn handle is NULL (either both must be NULL, or none)"));
            emaDestruct(mgr->hFwNatConns);
            hashDestruct(mgr->hAddrHash);
            if (mgr->log != NULL)
                RvLogSourceDestruct(&mgr->log);
            RvMemoryFree(mgr, logMgr);
            return RV_ERROR_NULLPTR;
        }
        /* set the default handlers */
        mgr->callbacks.fwNatGetProtConnHandleEv = RvH323FwNatDefaultGetProtConnHandleEv;
        mgr->callbacks.fwNatSetProtConnHandleEv = RvH323FwNatDefaultSetProtConnHandleEv;
    }

    if (encapMsgSize)
    {
        /* allocate a decode buffer - and a lock */
        if (RvMemoryAlloc(NULL, encapMsgSize, logMgr, (void**)&(mgr->encapBuffer)) != RV_OK)
        {
            RvLogError(&mgr->log, (&mgr->log,
                "RvH323FwNatInitialize: could not allocate encapsulation buffer"));
            emaDestruct(mgr->hFwNatConns);
            hashDestruct(mgr->hAddrHash);
            if (mgr->log != NULL)
                RvLogSourceDestruct(&mgr->log);
            RvMemoryFree(mgr, logMgr);
            return RV_ERROR_OUTOFRESOURCES;
        }
        if (RvLockConstruct(logMgr, &(mgr->encapLock)) != RV_OK)
        {
            RvLogError(&mgr->log, (&mgr->log,
                "RvH323FwNatInitialize: could not allocate encapsulation lock"));
            RvMemoryFree(mgr->encapBuffer, logMgr);
            emaDestruct(mgr->hFwNatConns);
            hashDestruct(mgr->hAddrHash);
            if (mgr->log != NULL)
                RvLogSourceDestruct(&mgr->log);
            RvMemoryFree(mgr, logMgr);
            return RV_ERROR_OUTOFRESOURCES;
        }
    }
    mgr->encapSize = encapMsgSize;

    /* Print to log */
    RvLogDebug(&mgr->log, (&mgr->log,
        "RvH323FwNatInitialize: FWNAT add-on initialized with %d hosts, encap-size %d", numHosts, encapMsgSize));
    RvLogDebug(&mgr->log, (&mgr->log,
        "RvH323FwNatInitialize: FWNAT add-on configuration: 17(%d) 18(%d) 19(%d)",
        mgr->e17State, mgr->e18State, mgr->e19State));

    *hFwNat = (HH323FWNATMGR)mgr;
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatGetVersion
 * Description:  Get the module version.
 *
 * Input:   none.
 * Output:  none.
 * Return: The version string.
 ********************************************************************************/
RVAPI const RvChar* RVCALLCONV RvH323FwNatGetVersion(void)
{
    return RV_H323_STACK_VERSION;
}


void* hashDeleteFNConn(HHASH hHash, void* elem, void *param)
{
    FwNatMgr * mgr = (FwNatMgr *)param;
    FwNatConn * fnConn;
    fnConn = *(FwNatConn**)hashGetElement(hHash, elem);

    if (emaLock((EMAElement)fnConn)) 
    {
        mgr->callbacks.fwNatSetProtConnHandleEv(fnConn->protConn, NULL);
        cmProtConnClose(mgr->hApp, fnConn->protConn);
        hashDelete(hHash, fnConn->hashLoc);
        emaDelete((EMAElement)fnConn);
        emaUnlock((EMAElement)fnConn);
    }

    return param;
}


/********************************************************************************
 * Function Name: RvH323FwNatReconfigure
 * Description:  Change the H.460.17/18/19 configuration.
 *
 * Input:  hFwNat       - FwNat module handle.
 *         config       - Configuration to use.
 * Output: none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatReconfigure(
    IN  HH323FWNATMGR               hFwNat,
    IN  RvH323FwNatConfiguration *  config)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    if (mgr == NULL)
        return RV_ERROR_NULLPTR;

    /* set H.460.17 configuration */
    if ((config->e17Config == RvH323FwNatH46017ConfigEncapsulate) &&
        (mgr->e17State == RvH323FwNatH46017StateDisabled))
    {
        /* enable H.460.17 */
        mgr->e17State = RvH323FwNatH46017StateEnabled;
        RvH323FwNatBuildEncapsulationFacilityMessage(mgr);
    }
    else if ((config->e17Config == RvH323FwNatH46017ConfigNotUsed) &&
        (mgr->e17State != RvH323FwNatH46017StateDisabled))
    {
        /* disable H.460.17, clear used memory */
        if (mgr->pGkConn != NULL)
        {
            mgr->callbacks.fwNatSetProtConnHandleEv(mgr->pGkConn->protConn, NULL);
            cmProtConnClose(mgr->hApp, mgr->pGkConn->protConn);
            if (emaLock((EMAElement)mgr->pGkConn))
            {
                emaDelete((EMAElement)mgr->pGkConn);
                emaUnlock((EMAElement)mgr->pGkConn);
            }
            mgr->pGkConn = NULL;
        }
        hashDoAll(mgr->hAddrHash, hashDeleteFNConn, mgr);
        mgr->e17State = RvH323FwNatH46017StateDisabled;
    }

    /* set H.460.18 configuration */
    if ((mgr->e18State == RvH323FwNatH46018StateNotSupported) &&
        (config->e18Config != RvH323FwNatH46018ConfigNotUsed))
    {
        /* currently not supporting 18, we are requested to support it */
        if (config->bGatekeeper)
            mgr->e18State = RvH323FwNatH46018StateInitializedGK;
        else
            mgr->e18State = RvH323FwNatH46018StateInitializedEP;
    }
    else if ((mgr->e18State != RvH323FwNatH46018StateNotSupported) &&
        (config->e18Config == RvH323FwNatH46018ConfigNotUsed))
    {
        /* currently supporting 18, we are requested not to support it */
        mgr->e18State = RvH323FwNatH46018StateNotSupported;
    }

    /* set H.460.19 configuration */
    mgr->e19State = RvH323FwNatH46019StateDisabled;
    if (config->e19Config == RvH323FwNatH46019ConfigClient)
        mgr->e19State = RvH323FwNatH46019StateClient;
    else if (config->e19Config == RvH323FwNatH46019ConfigServer)
        mgr->e19State = RvH323FwNatH46019StateServer;

    /* set H.460.19 media multiplexing mode configuration */
    if (mgr->e19State == RvH323FwNatH46019StateClient)
    {
        mgr->mediaMultiplexMode |= RV_H323_FWNAT_H46019_MULTIPLEX_MODE_TRANSMIT;
        if (config->e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigFull)
            mgr->mediaMultiplexMode |= RV_H323_FWNAT_H46019_MULTIPLEX_MODE_RECEIVE;
    }
    else
    {
        mgr->mediaMultiplexMode = 0;
        if (mgr->e19State == RvH323FwNatH46019StateServer)
        {
            if (config->e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigTransmit ||
                config->e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigFull)
                mgr->mediaMultiplexMode |= RV_H323_FWNAT_H46019_MULTIPLEX_MODE_TRANSMIT;
            if (config->e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigReceive ||
                config->e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigFull)
                mgr->mediaMultiplexMode |= RV_H323_FWNAT_H46019_MULTIPLEX_MODE_RECEIVE;
        }
    }

    mgr->bGatekeeper = config->bGatekeeper;

    RvLogDebug(&mgr->log, (&mgr->log,
        "RvH323FwNatReconfigure: 17(%d) 18(%d) 19(%d)", mgr->e18State, mgr->e19State, mgr->e17State));
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatEnd
 * Description:  End the H.460.17/18/19 module.
 *
 * Input:   hFwNat       - FwNat module handle.
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEnd(
    IN  HH323FWNATMGR       hFwNat)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    RvLogMgr * logMgr = mgr->logMgr;
    HPVT       hVal;
    if (mgr == NULL)
        return RV_ERROR_NULLPTR;
    hVal = cmGetValTree(mgr->hApp);

    RvLogDebug(&mgr->log, (&mgr->log,
        "RvH323FwNatEnd: ending FWNAT add-on"));

    pvtDelete(hVal, mgr->defEncapMsg);
    pvtDelete(hVal, mgr->facilityMessageWithCID);

    if (mgr->encapSize > 0)
    {
        RvLockDestruct(&mgr->encapLock, mgr->logMgr);
        RvMemoryFree(mgr->encapBuffer, logMgr);
    }
    emaDestruct(mgr->hFwNatConns);
    hashDestruct(mgr->hAddrHash);

    pstDestruct(mgr->IncomingCallIndicationSyn);
    pstDestruct(mgr->TraversalParamsSyn);

    if (mgr->log != NULL)
        RvLogSourceDestruct(&mgr->log);
    RvMemoryFree(mgr, logMgr);
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatHookRecv
 * Description: This function should be called from hookRecv().
 *              It is used to extract FwNat information from the Q.931 messages.
 *
 * Input:   hFwNat          - FwNat module handle
 *          messageNodeId   - Stack's handle for the RAS transaction
 *          hConn           - Stack's handle for the connection.
 *          bIgnore         - RV_TRUE if the application should ignore the message.
 * Output:  bInfo           - RV_TRUE if the message contained any relevant
 *                            information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookRecv(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn,
    IN  RvPvtNodeId         messageNodeId,
    OUT RvBool              *bIgnore,
    OUT RvBool              *bInfo)
{
    RvPstFieldId fieldId;
    RvPvtNodeId  msgNode, callIDNodeId, paramsNode;
    FwNatMgr     *mgr = (FwNatMgr *)hFwNat;
    HPVT         hVal;
    HCALL        hsCall = NULL;
    FwNatConn  * fnConn = NULL;

    *bIgnore = RV_FALSE;
    *bInfo   = RV_FALSE;
    if (mgr == NULL)
        return RV_ERROR_NULLPTR;
    hVal = cmGetValTree(mgr->hApp);

    RvLogEnter(&mgr->log, (&mgr->log,
        "RvH323FwNatRAShookRecv: e17State = %d e18State = %d", mgr->e17State, mgr->e18State));

    mgr->callbacks.fwNatGetProtConnHandleEv(hConn, (HH323FWNATCONN *)&fnConn);

    /* try to associate the hTpkt element to a call using the call ID */
    if (getGenericIndicationMessage(mgr, messageNodeId, &hsCall) == RV_OK)
    {
        /* we found a call! */
        if (mgr->callbacks.fwNatSetByCallEv != NULL)
        {
            HAPPCALL    haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall);

            mgr->callbacks.fwNatGetByCallEv(mgr->hApp, hFwNat, hsCall, haCall, 0, (HH323FWNATCONN*)&fnConn);
            if ((fnConn != NULL) && (fnConn->connType == RvH323FwNatConnTypeH245MultiHost))
            {
                setH245ConnectionForCall(mgr, hsCall, hConn, fnConn);
                mgr->callbacks.fwNatSetByCallEv(mgr->hApp, hFwNat, hsCall, haCall, NULL, 0);
            }
        }
    }

    /*first check if we're a GK waiting for a facility message after SCI/SCR RAS messages*/
    if (mgr->e18State == RvH323FwNatH46018StateInitializedGK)
    {
        RvBool facilityZeroCRV = RV_FALSE;
        /*get message type*/
        __pvtGetByFieldIds(msgNode, hVal, messageNodeId,
            { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
              _q931(h323_uu_pdu) _q931(h323_message_body) _anyField LAST_TOKEN }, &fieldId, NULL, NULL);

        if (fieldId == __q931(facility))
        {
            RvInt32 crv;
            /* check for zero CRV */
            __pvtGetByFieldIds(callIDNodeId, hVal, messageNodeId, { _q931(callReferenceValue) _q931(twoBytes) LAST_TOKEN }, NULL, &crv, NULL);
            if (RV_PVT_NODEID_IS_VALID(callIDNodeId) && (crv == 0))
            {
                facilityZeroCRV = RV_TRUE;
            }
        }

        if (facilityZeroCRV)
        {
            HH323FWNATCONN hFM17Conn = NULL;
            RvChar  callID[16];
            RvInt32 callIDLen = 16;
            hsCall = NULL;
            /*get callId*/
            __pvtGetByFieldIds(callIDNodeId, hVal, msgNode, { _q931(callIdentifier) _q931(guid) LAST_TOKEN }, NULL, &callIDLen, NULL);
            pvtGetString(hVal, callIDNodeId, callIDLen, callID);

            /*find call handle in the CAT*/
            cmCallCatGetCall(mgr->hApp, callID, RV_FALSE, &hsCall);

            /* find out if we need to set the connection on the call
            and send a setup message to the called endpoint */
            if (hsCall != NULL)
            {
                HAPPCALL    haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall);
                /* get FwNat connection from application */
                if (mgr->callbacks.fwNatSetByCallEv != NULL)
                {
                    mgr->callbacks.fwNatGetByCallEv(mgr->hApp, hFwNat, hsCall, haCall, 0, &hFM17Conn);
                }
                /* lock the connection */
                if (emaLock((EMAElement)hFM17Conn))
                {
                    RvStatus status = RV_ERROR_UNKNOWN;
                    cmElem     *app = (cmElem *)mgr->hApp;
                    FwNatConn  *conn = (FwNatConn *)hFM17Conn;

                    if (conn->connType == RvH323FwNatConnTypeReverseCallOutgoing)
                    {
                        /*tell the application to ignore this message*/
                        *bIgnore = RV_TRUE;
                       /*stop the timer and dial the EP*/
                        RvH323TimerCancel(app->hTimers, &conn->timer);
                        /*set the incoming TCP connection to outgoing*/
                        status = cmProtConnSetParam(mgr->hApp,
                                                    hConn,
                                                    cmProtConnParamIncoming,
                                                    (RvInt32)RV_FALSE,
                                                    NULL);
                        if (status < RV_OK)
                        {
                            emaUnlock((EMAElement)hFM17Conn);
                            return status;
                        }

                        /*set the connection in the call*/
                        status = cmCallSetConnection(mgr->hApp,
                                                     hsCall,
                                                     RV_TRUE,
                                                     hConn);
                        if (status < RV_OK)
                        {
                            emaUnlock((EMAElement)hFM17Conn);
                            return status;
                        }

                        /*send setup message to EP*/
                        cmCallSetParam(hsCall, cmParamH245Stage, 0,cmTransH245Stage_setup,NULL);
                        status = cmCallDial(hsCall);

                        /* remove this fnConn from the call */
                        if (mgr->callbacks.fwNatSetByCallEv != NULL)
                        {
                            mgr->callbacks.fwNatSetByCallEv(mgr->hApp, hFwNat, hsCall, haCall, NULL, 0);
                        }
                        emaDelete((EMAElement)hFM17Conn);
                    }
                    emaUnlock((EMAElement)hFM17Conn);
                    return status;
                }
            }
        }
    }

    /* if we do not have a connected encapsulation state, don't even look */
    if ((!mgr->bGatekeeper && (mgr->e17State != RvH323FwNatH46017StateConnected)) ||
        (mgr->bGatekeeper && (mgr->e17State != RvH323FwNatH46017StateEnabled)))
        return RV_OK;

    /* check if the message contains an encapsulated message */
    if (RvH323FwNatHasEncapsulatedMessage(mgr, messageNodeId, &paramsNode))
    {
        RvUint8 * rasMessage;
        RvInt32 rasMessageLen;
        RvStatus status;

        if (fnConn == NULL)
        {
            fnConn = RvH323FwNatSetEncapsulatedConnection(mgr, hConn);
        }

        /* get the encapsulated message */
        RvLockGet(&mgr->encapLock, mgr->logMgr);
        status = RvH323FwNatGetEncapsulatedMessage(mgr, paramsNode, &rasMessage, &rasMessageLen);
        if (status == RV_OK)
        {
            cmTransportAddress remoteAddr;
            cmProtocolGetRemoteAddress(mgr->hApp, hConn, &remoteAddr);
            cmProtocolRecvRawMessageFrom(mgr->hApp, cmGetRASConnectionHandle(mgr->hApp),
                rasMessage, rasMessageLen, &remoteAddr);
        }
        RvLockRelease(&mgr->encapLock, mgr->logMgr);
    }

    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatEvRASMessageSend
 * Description: This function should be called from cmEvRASMessageSend(). it
 *              may add FwNat information to the message.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - RAS transaction this message belongs to.
 *          rasTrStage   - Stage this message is sent for.
 *          messageNode  - PVT nodeId of the message being sent out.
 * Output:  bStop        - RV_TRUE if the message should not be sent (to be
 *                         returned on exit from cmEvRASMessageSend).
 *          bInfo        - RV_TRUE if the message was added with any information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvRASMessageSend(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas,
    IN  cmRASTrStage        rasTrStage,
    IN  RvPvtNodeId         messageNodeId,
    OUT RvBool *            bStop,
    OUT RvBool *            bInfo)
{
    RvStatus     status = RV_OK;
    RvPstFieldId fieldId;
    RvPvtNodeId  msgNode;
    FwNatMgr     *mgr = (FwNatMgr *)hFwNat;
    HPVT         hVal;

    RV_UNUSED_ARG(rasTrStage);
    RV_UNUSED_ARG(hsRas);

    *bStop = RV_FALSE;
    *bInfo = RV_FALSE;

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;
    if ((mgr->e18State == RvH323FwNatH46018StateNotSupported) && (mgr->e17State == RvH323FwNatH46017StateDisabled))
        return RV_OK;
    hVal = cmGetValTree(mgr->hApp);

    RvLogEnter(&mgr->log, (&mgr->log,
        "RvH323FwNatEvRASMessageSend: e17State = %d e18State = %d", mgr->e17State, mgr->e18State));

    /*find what type of RAS message we're handling*/
    __pvtGetByFieldIds(msgNode, hVal, messageNodeId, { _anyField LAST_TOKEN }, &fieldId, NULL, NULL);

    if (mgr->e18State != RvH323FwNatH46018StateNotSupported)
    {
        switch (fieldId)
        {
        case __q931(gatekeeperRequest):
        case __q931(gatekeeperConfirm):
        case __q931(registrationRequest):
        case __q931(registrationConfirm):
            {
                RvPvtNodeId supportedNode, neededNode, desiredNode;
                /*add H.460.18 to supportedFeatures of RAS message*/
                status = cmAddFeature(mgr->hApp,
                                    messageNodeId,
                                    RV_H323_FWNAT_SIGNALLING_TRAVERSAL_IDENTIFIER_VALUE,
                                    RV_TRUE, RV_FALSE, RV_FALSE,
                                    &supportedNode,
                                    &neededNode,
                                    &desiredNode);

                if(status == RV_OK)
                    *bInfo = RV_TRUE;
            }
            break;

        default:
            /*do nothing*/
            break;
        }
    }

    if (mgr->e17State != RvH323FwNatH46017StateDisabled)
    {
        cmTransportAddress addrTo;
        RvInt32 size = sizeof(addrTo);

        /* check if we need to create an encapsulation connection */
        if (!mgr->bGatekeeper)
        {
            /* get the remote address */
            status = cmRASGetParam(hsRas, rasTrStage, cmRASParamDestinationIpAddress, 0, &size, (char*)&addrTo);

            /* maybe we already have a connection */
            if ((status == RV_OK) &&
                (RvH323FwNatGetEncapsulationConnection(mgr, &addrTo) == NULL))
            {
                if ((fieldId == __q931(registrationRequest)) || (fieldId == __q931(registrationConfirm)))
                {
                    FwNatConn * fnConn;
                    /* no connection - create one */
                    status = RvH323FwNatConnectRasEncapsulation(mgr, &addrTo, hsRas, &fnConn);
                }
                else
                {
                    /* this is NOT the state to create an encapsulation connection in! */
                    status = RV_ERROR_UNINITIALIZED;
                }
            }
        }
        /* we should already have a connection here, so we add the RRQ/CF parameters,
           and delete the addresses */
        if ((status == RV_OK) &&
            ((fieldId == __q931(registrationRequest)) || (fieldId == __q931(registrationConfirm))))
        {
            RvPvtNodeId addresses;
            /* we should add maintain-connection to the RRQ */
            pvtAdd(hVal, msgNode, __q931(maintainConnection), RV_TRUE, NULL, NULL);
            if (fieldId == __q931(registrationRequest))
            {
                /* we also add multiplex, because we think it's best */
                pvtAdd(hVal, msgNode, __q931(multipleCalls), RV_TRUE, NULL, NULL);
            }
            /* remove the RAS and CS addresses */
            addresses = pvtGetChildByFieldId(hVal, msgNode, __q931(rasAddress), NULL, NULL);
            pvtDeleteChilds(hVal, addresses);
            addresses = pvtGetChildByFieldId(hVal, msgNode, __q931(callSignalAddress), NULL, NULL);
            pvtDeleteChilds(hVal, addresses);
        }
    }

    RvLogLeave(&mgr->log, (&mgr->log,
        "RvH323FwNatEvRASMessageSend: result = %d bInfo = %d bStop = %d", status, *bInfo, *bStop));
    return status;
}


/********************************************************************************
 * Function Name: RvH323FwNatHookRecvFrom
 * Description: This function should be called from hookRecvFrom().
 *              It may modify the message.
 *
 * Input:   hFwNat          - FwNat module handle
 *          messageNodeId   - Stack's handle for the RAS transaction
 * Output:  bInfo           - RV_TRUE if the message was modified with any relevant
 *                            information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookRecvFrom(
    IN  HH323FWNATMGR       hFwNat,
    IN  RvPvtNodeId         messageNodeId,
    IN  RvPvtNodeId         addrFromNodeId,
    OUT RvBool *            bInfo)
{
    RvPstFieldId fieldId;
    RvPvtNodeId  msgNode;
    FwNatMgr     *mgr = (FwNatMgr *)hFwNat;
    HPVT         hVal;

    *bInfo = RV_FALSE;
    if (mgr == NULL)
        return RV_ERROR_NULLPTR;
    hVal = cmGetValTree(mgr->hApp);

    if ((mgr->e18State == RvH323FwNatH46018StateNotSupported) && (mgr->e17State == RvH323FwNatH46017StateDisabled))
        return RV_OK;

    /* print if we are actually doing something, otherwise we're just polluting the log */
    RvLogEnter(&mgr->log, (&mgr->log,
        "RvH323FwNatHookRecvFrom: e17State = %d e18State = %d", mgr->e17State, mgr->e18State));

    /*find what type of RAS message we're handling*/
    __pvtGetByFieldIds(msgNode, hVal, messageNodeId, { _anyField LAST_TOKEN }, &fieldId, NULL, NULL);

    switch (fieldId)
    {
    case __q931(gatekeeperRequest):
    case __q931(gatekeeperConfirm):
    case __q931(registrationRequest):
    case __q931(registrationConfirm):
        {
            RvBool bSupported, bDesired, bNeeded;
            RvPvtNodeId supportedNode, neededNode, desiredNode;
            /*check if endpoint supports H.460 */
            cmIsFoundFeature(mgr->hApp,
                             messageNodeId,
                             RV_H323_FWNAT_SIGNALLING_TRAVERSAL_IDENTIFIER_VALUE,
                             &bSupported,
                             &bNeeded,
                             &bDesired,
                             &supportedNode,
                             &neededNode,
                             &desiredNode);
            
            if (bSupported == RV_FALSE)
                break;

            /*gatekeeper - it's a RASRequest - modify ep source address in gatekeeper*/
            if (mgr->e18State == RvH323FwNatH46018StateInitializedGK)
            {
                RvPvtNodeId sourceAddr;

                __pvtGetByFieldIds(sourceAddr, hVal, messageNodeId, { _anyField _q931(rasAddress) LAST_TOKEN }, NULL, NULL, NULL);
                /*delete the current source address*/
                pvtDelete(hVal, sourceAddr);
                /*set the apparent source address in the ras request*/
                __pvtBuildByFieldIds(sourceAddr, hVal, messageNodeId, { _anyField _q931(rasAddress) LAST_TOKEN }, 0, NULL);
                if (fieldId == __q931(registrationRequest))
                {
                    sourceAddr = pvtAdd(hVal, sourceAddr, __nul(0), 0, NULL, NULL);
                }
                pvtSetTree(hVal, sourceAddr, hVal, addrFromNodeId);
            }
            /*endpoint - set e18State properly*/
            else if (mgr->e18State ==  RvH323FwNatH46018StateInitializedEP)
            {
                mgr->e18State = RvH323FwNatH46018StateSupportedEP;
            }
            *bInfo = RV_TRUE;
        }
        break;
    default:
        /*do nothing*/
        break;
    }

    if ((!mgr->bGatekeeper && (mgr->e17State == RvH323FwNatH46017StateConnected)) ||
        (mgr->bGatekeeper && (mgr->e17State == RvH323FwNatH46017StateEnabled)))
    {
        if ((fieldId == __q931(registrationRequest)) || (fieldId == __q931(registrationConfirm)))
        {
            FwNatConn * fnConn = NULL;

            if (mgr->bGatekeeper)
            {
                cmTransportAddress addrFrom;
                cmVt2TransportAddress(hVal, addrFromNodeId, &addrFrom);
                fnConn = RvH323FwNatGetEncapsulationConnection(mgr, &addrFrom);
            }
            else
            {
                fnConn = mgr->pGkConn;
            }

            /* if we have a connection here, get the RRQ/CF parameters */
            if (emaLock((EMAElement)fnConn))
            {
                RvInt32 val;
                RvPvtNodeId addrNode;

                /* get maintain-connection from the RRQ (should always be there */
                pvtGetChildByFieldId(hVal, msgNode, __q931(maintainConnection), &val, NULL);
                cmProtConnSetParam(mgr->hApp, fnConn->protConn, cmProtConnParamMaintain, val, NULL);

                /* also try to get multiple calls */
                pvtGetChildByFieldId(hVal, msgNode, __q931(multipleCalls), &val, NULL);

                /* and try to get the call signaling address node */
                addrNode = pvtGetChildByFieldId(hVal, msgNode, __q931(callSignalAddress), NULL, NULL);
                if (RV_PVT_NODEID_IS_VALID(addrNode))
                {
                    addrNode = pvtChild(hVal, addrNode);
                    if (!RV_PVT_NODEID_IS_VALID(addrNode))
                        /* this also signals a need for multiplexing */
                        val = RV_TRUE;
                }
                cmProtConnSetParam(mgr->hApp, fnConn->protConn, cmProtConnParamMultiplex, val, NULL);

                /* now remove any address that might confuse us */
                addrNode = pvtGetChildByFieldId(hVal, msgNode, __q931(rasAddress), NULL, NULL);
                pvtDelete(hVal, addrNode);
                addrNode = pvtGetChildByFieldId(hVal, msgNode, __q931(replyAddress), NULL, NULL);
                pvtDelete(hVal, addrNode);

                emaUnlock((EMAElement)fnConn);
            }
        }
    }

    RvLogLeave(&mgr->log, (&mgr->log,
        "RvH323FwNatHookRecvFrom: result = %d", RV_OK));
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatHookSendRaw
 * Description: This function should be called from hookSendRaw().
 *              Currently does nothing.
 *
 * Input:   hFwNat          - FwNat module handle.
 *          hConn           - Stack's handle connection.
 *          rawMessage      - Outgoing message buffer.
 *          messageLen      - Message length.
 * Output:  bStop           - RV_TRUE iff the message should not be sent (to be
 *                            returned on exit from hookSendRaw).
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookSendRaw(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn,
    IN  const RvUint8 *     rawMessage,
    IN  RvInt32             messageLen,
    OUT RvBool *            bStop)
{
    RV_UNUSED_ARG(hFwNat);
    RV_UNUSED_ARG(hConn);
    RV_UNUSED_ARG(rawMessage);
    RV_UNUSED_ARG(messageLen);
    *bStop = RV_FALSE;
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatHookSendRawTo
 * Description: This function should be called from hookSendRawTo().
 *              Encapsulates the RAS messages in facility messages.
 *
 * Input:   hFwNat          - FwNat module handle.
 *          hConn           - Stack's handle connection.
 *          rawMessage      - Outgoing message buffer.
 *          messageLen      - Message length.
 *          addrTo          - Address sent to.
 * Output:  bStop           - RV_TRUE iff the message should not be sent (to be
 *                            returned on exit from hookSendRawTo).
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookSendRawTo(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn,
    IN  const RvUint8 *     rawMessage,
    IN  RvInt32             messageLen,
    IN  cmTransportAddress* addrTo,
    OUT RvBool *            bStop)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    FwNatConn * encapConn;
    RvStatus status = RV_OK;
    HPVT hVal;

    RV_UNUSED_ARG(hConn);
    *bStop = RV_FALSE;
    if (mgr == NULL)
        return RV_ERROR_NULLPTR;
    hVal = cmGetValTree(mgr->hApp);

    RvLogEnter(&mgr->log, (&mgr->log,
        "RvH323FwNatHookSendRawTo: e17State = %d", mgr->e17State));

    if (mgr->e17State == RvH323FwNatH46017StateDisabled)
    {
        return RV_OK;
    }

    encapConn = RvH323FwNatGetEncapsulationConnection(mgr, addrTo);

    if (emaLock((EMAElement)encapConn))
    {
        RvPvtNodeId  msgNode;

        msgNode = pvtAddRoot(hVal, cmGetSynTreeByRootName(mgr->hApp, "q931"), 0, NULL);
        RvH323FwNatEncapsulateMessage(mgr, rawMessage, messageLen, msgNode);

        status = cmProtocolSendMessage(mgr->hApp, encapConn->protConn, msgNode);
        if (status == RV_OK)
            *bStop = RV_TRUE;

        pvtDelete(hVal, msgNode);
        emaUnlock((EMAElement)encapConn);
    }

    RvLogLeave(&mgr->log, (&mgr->log,
        "RvH323FwNatHookSendRawTo: result = %d stop = %d", status, *bStop));
    return status;
}


/********************************************************************************
 * Function Name: RvH323FwNatHookClose
 * Description: This function should be called from hookClose().
 *              Allows the FW/NAT module to free related resources.
 *
 * Input:   hFwNat          - FwNat module handle.
 *          hConn           - Stack's handle connection.
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookClose(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    FwNatConn * fnConn = NULL;

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;

    mgr->callbacks.fwNatGetProtConnHandleEv(hConn, (HH323FWNATCONN *)&fnConn);
    if (emaLock((EMAElement)fnConn))
    {
        /* we have a connection related to this host. this is the time to change state according to
        the host's function */
        if (fnConn->connType == RvH323FwNatConnTypeRasEncapsulation)
        {
            /* remove the encapsulated connection */
            if (!mgr->bGatekeeper)
            {
                if (mgr->pGkConn == fnConn)
                {
                    mgr->pGkConn = NULL;
                    if (mgr->e17State == RvH323FwNatH46017StateConnected)
                    {
                        mgr->e17State = RvH323FwNatH46017StateEnabled;

                        /* raise an event */
                        if (mgr->callbacks.fwNatRasEncapStatusEv != NULL)
                        {
                            mgr->callbacks.fwNatRasEncapStatusEv(mgr->hApp, (HH323FWNATMGR)mgr,
                                hConn, (HH323FWNATCONN)fnConn, RvH323FwNatH46017StateEnabled, fnConn->hsRas);
                        }
                    }
                }
            }
            if (fnConn->hashLoc != NULL)
                hashDelete(mgr->hAddrHash, fnConn->hashLoc);
        }
        emaDelete((EMAElement)fnConn);
        emaUnlock((EMAElement)fnConn);
    }
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatHookListen
 * Description: This function should be called from hookListen().
 *              Allows the FW/NAT module prevent unnecessary listening.
 *
 * Input:   hFwNat          - FwNat module handle.
 *          hConn           - Stack's handle connection.
 *          addrNodeId      - Listening address node Id.
 * Output:  bStop           - RV_TRUE iff the message should not be sent (to be
 *                            returned on exit from hookListen).
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookListen(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn,
    IN  RvPvtNodeId         addrNodeId,
    OUT RvBool *            bStop)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    FwNatConn * fnConn = NULL;
    RvInt32 type = -1;
    HCALL hsCall;
    HAPPCALL haCall;

    RV_UNUSED_ARG(addrNodeId);

    *bStop = RV_FALSE;
    if (mgr == NULL)
        return RV_ERROR_NULLPTR;

    if (mgr->e18State != RvH323FwNatH46018StateInitializedGK)
        return RV_OK;

    cmProtConnGetParam(mgr->hApp, hConn, cmProtConnParamType, &type, NULL);
    if (type != cmProtocolH245)
        return RV_OK;

    hsCall = cmGetCallByH245Chan(hConn);
    if (hsCall == NULL)
        return RV_ERROR_UNKNOWN;

    haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall);
    mgr->callbacks.fwNatGetByCallEv(mgr->hApp, hFwNat, hsCall, haCall, 0, (HH323FWNATCONN*)&fnConn);

    if ((fnConn != NULL) && (fnConn->connType == RvH323FwNatConnTypeH245MultiHost))
        *bStop = RV_TRUE;
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatEvRASRequest
 * Description: This function should be called from cmEvRASRequest(). This API
 *              is used in the EP inside the NAT to handle an incoming call (SCI message).
 *
 * Input:   hFwNat       - 181917 module handle
 *          hsRas        - Stack's handle for the RAS transaction
 *          bIgnore      - RV_TRUE if the application should ignore the message.
 * Output:  bInfo        - RV_TRUE if the message was modified with any relevant
 *                         information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvRASRequest(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas,
    OUT RvBool *            bIgnore,
    OUT RvBool *            bInfo)
{
    FwNatMgr     *mgr = (FwNatMgr *)hFwNat;
    HPVT         hVal;

    *bInfo = RV_FALSE;
    *bIgnore = RV_FALSE;
    if (mgr == NULL)
        return RV_ERROR_NULLPTR;
    hVal = cmGetValTree(mgr->hApp);

    if ((mgr->e18State == RvH323FwNatH46018StateSupportedEP) || (mgr->e18State == RvH323FwNatH46018StateInitializedGK))
    {
        RvPvtNodeId messageNodeId, msgNode, addrNodeId;
        RvPstFieldId fieldId;

        /*get the message from the ras transaction*/
        messageNodeId = cmGetProperty((HPROTOCOL)hsRas);

        if (!RV_PVT_NODEID_IS_VALID(messageNodeId))
            return RV_ERROR_UNKNOWN;

        /*Check if it's a SCI message with IncomingCallIndication*/
        __pvtGetByFieldIds(msgNode, hVal, messageNodeId, { _anyField _anyField LAST_TOKEN }, &fieldId, NULL, NULL);

        if (fieldId == __q931(serviceControlIndication))
        {
            RvPvtNodeId         incomingCallIndicationNode;
            RvPvtNodeId         callIdNodeId;
            RvInt32             addrLen, callIDLen;
            cmTransportAddress  addr, localAddr;
            RvChar              callID[16];
            HPROTCONN           hConn;
            FwNatConn *         fnConn;
            RvStatus            status;

            /*send SCR*/
            if (cmRASConfirm(hsRas) < RV_OK)
                return RV_ERROR_UNKNOWN;

            incomingCallIndicationNode = pvtAddRoot(hVal, mgr->IncomingCallIndicationSyn, 0, NULL);

            /*retrieve IncomingCallIndication*/
            if(!RV_PVT_NODEID_IS_VALID(getIncomingCallIndicationFromMessage(mgr, msgNode, &incomingCallIndicationNode)))
                return RV_OK;

            /*retrieve callID from IncomingCallIndication*/
            __pvtGetByFieldIds(callIdNodeId, hVal, incomingCallIndicationNode, { _q931(callID) _q931(guid) LAST_TOKEN }, &fieldId, &callIDLen, NULL);
            pvtGetString(hVal, callIdNodeId, callIDLen, callID);

            /*retrieve the callSignallingAddress from IncomingCallIndication*/
            __pvtGetByFieldIds(addrNodeId, hVal, incomingCallIndicationNode, { _q931(callSignallingAddress) LAST_TOKEN }, &fieldId, &addrLen, NULL);
            cmVt2TransportAddress(hVal, addrNodeId, &addr);

            /*start a tcp connection to the retrieved callSignallingAddress*/
            memset(&localAddr, 0, sizeof(localAddr));
            localAddr.type = addr.type;

            fnConn = (FwNatConn *)emaAdd(mgr->hFwNatConns, NULL);
            if (fnConn != NULL)
            {
                status = cmProtConnNew(mgr->hApp, cmProtocolQ931, RV_FALSE,
                    outConn4incomingCallConnect, fnConn, &addr, &localAddr, &hConn);

                if (status == RV_OK)
                {
                    mgr->callbacks.fwNatSetProtConnHandleEv(hConn, (HH323FWNATCONN)fnConn);
                    memset(fnConn, 0, sizeof(FwNatConn));
                    fnConn->connType = RvH323FwNatConnTypeReverseCallIncoming;
                    fnConn->protConn = hConn;
                    memcpy(fnConn->CID, callID, sizeof(fnConn->CID));
                }
                else
                {
                    emaDelete(fnConn);
                }
            }
            /*delete incomingCallIndicationNode*/
            pvtDelete(hVal, incomingCallIndicationNode);
            cmRASClose(hsRas);
            *bIgnore = RV_TRUE;
        }
    }
    return RV_OK;
}

/********************************************************************************
 * Function Name: RvH323FwNatEvAutoRASRequest
 * Description: This function should be called from cmEvAutoRASRequest(). This API
 *              is used in the EP inside the NAT to handle an incoming call (SCI message).
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - Stack's handle for the RAS transaction
 * Output:  bInfo        - RV_TRUE iff the message contained any relevant
 *                         information.
 *          bIgnore      - RV_TRUE if message should be ignored.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvAutoRASRequest(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas,
    OUT RvBool *            bIgnore,
    OUT RvBool *            bInfo)
{
    return RvH323FwNatEvRASRequest(hFwNat, hsRas, bIgnore, bInfo);
}

/********************************************************************************
 * Function Name: RvH323FwNatEvRASConfirm
 * Description: This function should be called from cmEvRASConfirm().
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - Stack's handle for the RAS transaction
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvRASConfirm(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas)
{
    RV_UNUSED_ARG(hFwNat);
    RV_UNUSED_ARG(hsRas);
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatEvRASReject
 * Description: This function should be called from cmEvRASReject().
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - Stack's handle for the RAS transaction
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvRASReject(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas)
{
    RV_UNUSED_ARG(hFwNat);
    RV_UNUSED_ARG(hsRas);
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatEvRASTimeout
 * Description: This function should be called from cmEvRASTimeout().
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - Stack's handle for the RAS transaction
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvRASTimeout(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas)
{
    RV_UNUSED_ARG(hFwNat);
    RV_UNUSED_ARG(hsRas);
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatOpenReverseCall
 * Description:  Ask the FwNat module in the GK side to start a reverse call
 *               (a call into the NAT).
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsCall       - Stack's call handle
 *          epRasAddr    - RAS address of the EP to open a call to.
 *          haRas        - Application handle for the created SCI Tx.
 * Output:  haRas        - Stack handle for the created SCI Tx.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatOpenReverseCall(
    IN  HH323FWNATMGR       hFwNat,
    IN  HCALL               hsCall,
    IN  cmTransportAddress *epRasAddr,
    IN  HAPPRAS             haRas,
    OUT HRAS               *hRas)
{
    FwNatMgr     *mgr = (FwNatMgr *)hFwNat;
    cmElem       *app;
    FwNatConn    *fnConn;
    HRAS         hsRas;

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;
    if (mgr->e18State != RvH323FwNatH46018StateInitializedGK)
        return RV_OK;
    app = (cmElem *)mgr->hApp;

    /*create a connection object*/
    fnConn = emaAdd(mgr->hFwNatConns, (void *)&fnConn);
    if (fnConn == NULL)
        return RV_ERROR_UNKNOWN;
    fnConn->connType = RvH323FwNatConnTypeReverseCallOutgoing;
    fnConn->hCall = hsCall;
    fnConn->numRelatedCalls = 1;
    fnConn->protConn = NULL;

    /*set the connection object in the call object of application*/
    if (mgr->callbacks.fwNatSetByCallEv != NULL)
    {
        HAPPCALL haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall);
        mgr->callbacks.fwNatSetByCallEv(mgr->hApp,
                                              hFwNat,
                                              hsCall,
                                              haCall,
                                              (HH323FWNATCONN)fnConn,
                                              0);
    }

    /*Create ans send SCI message with IncomingCallIndication to the EP*/
    if (cmRASStartTransaction(mgr->hApp, haRas, &hsRas, cmRASServiceControl, epRasAddr, hsCall) < RV_OK)
        return RV_ERROR_UNKNOWN;

    /*Add IncomingCallIndication to SCI message*/
    addIncomingCallIndication(mgr, hsCall, hsRas);

    /*send ras message*/
    if (cmRASRequest(hsRas) < RV_OK)
    {
        cmRASClose(hsRas);
        return RV_ERROR_UNKNOWN;
    }
    *hRas = hsRas;

    /*Add call to the CAT*/
    cmCallCatAddCall(hsCall, RV_FALSE);

    /*Start a timer and wait for facility message after SCR message*/
    fnConn->timer = RvH323TimerStart(app->hTimers, openReverseCallTimeout, (void *)fnConn, 5000);

    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatOpenEncapsulateConn
 * Description: Checks if an encapsulation connection exists to the given address,
 *              and if not, creates it. returns the existing or created connection.
 *
 * Input:   hFwNat       - FwNat module handle
 *          addrTo       - Transport address to check or connect to.
 * Output:  hConn        - The encapsulated connection
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatOpenEncapsulateConn(
    IN  HH323FWNATMGR       hFwNat,
    IN  cmTransportAddress *addrTo,
    OUT HPROTCONN *         hConn)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    FwNatConn * fnConn;
    RvStatus status = RV_OK;

    fnConn = RvH323FwNatGetEncapsulationConnection(mgr, addrTo);
    /* check if we already have a connection */
    if (fnConn == NULL)
    {
        /* no connection - create one */
        status = RvH323FwNatConnectRasEncapsulation(mgr, addrTo, NULL, &fnConn);
    }
    if (fnConn != NULL)
        *hConn = fnConn->protConn;
    return status;
}


/********************************************************************************
 * Function Name: RvH323FwNatRASIsEncapsulated
 * Description: Checks if a transaction has an encapsulated connection according
 *              to a transport address or a RAS transaction handle.
 *
 * Input:   hFwNat       - FwNat module handle
 *          rasAddr      - RAS transport address to check.
 *          hsRas        - Stack's RAS transaction handle
 * Output:  bEncapsulated- true if an encapsulated connection exists.
 *          hConn        - The encapsulated connection
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatRASIsEncapsulated(
    IN  HH323FWNATMGR       hFwNat,
    IN  cmTransportAddress *rasAddr,
    IN  HRAS                hsRas,
    OUT RvBool *            bEncapsulated,
    OUT HPROTCONN *         hConn)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    cmTransportAddress txAddress;
    RvStatus status = RV_ERROR_UNKNOWN;

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;

    if (rasAddr == NULL)
    {
        RvInt32 size = sizeof(txAddress);
        status = cmRASGetParam(hsRas, cmRASTrStageRequest, cmRASParamDestinationIpAddress, 0,
            &size, (char*)&txAddress);
        if (status == RV_OK)
        {
            rasAddr = &txAddress;
        }
    }
    if (rasAddr != NULL)
    {
        FwNatConn * fnConn = RvH323FwNatGetEncapsulationConnection(mgr, rasAddr);
        if (fnConn == NULL)
        {
            *bEncapsulated = RV_FALSE;
            *hConn = NULL;
        }
        else
        {
            *bEncapsulated = RV_TRUE;
            *hConn = fnConn->protConn;
        }
        return RV_OK;
    }

    return status;
}


/********************************************************************************
 * Function Name: RvH323FwNatCloseEncapsulateConn
 * Description: Set a timer to close an encapsulated connection.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hConn        - The encapsulation connection.
 * Output:  None.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatCloseEncapsulateConn(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    FwNatConn * fnConn = NULL;
    cmElem *app;

    if ((mgr == NULL) || (hConn == NULL))
        return RV_ERROR_NULLPTR;
    app = (cmElem *)mgr->hApp;

    mgr->callbacks.fwNatGetProtConnHandleEv(hConn, (HH323FWNATCONN*)&fnConn);
    if (fnConn != NULL)
        fnConn->timer = RvH323TimerStart(app->hTimers, closeEncapsulationConnection, (void *)fnConn, 1);
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatOpenH245Server
 * Description:  Ask the FwNat module to create an H.245 multi-server
 *               connection. The application must keep the connection handle.
 *
 * Input:   hFwNat       - FwNat module handle
 *          addr         - Address to listen on, may be set to zero.
 * Output:  addr         - Address listening on.
 *          hFwNatConn    - The FwNat H.245 server.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatOpenH245Server(
    IN    HH323FWNATMGR         hFwNat,
    INOUT cmTransportAddress *  addr,
    OUT   HH323FWNATCONN *      hFwNatConn)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    FwNatConn * fnConn;
    HPROTCONN hConn;
    RvStatus status;

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;
    RvLogEnter(&mgr->log, (&mgr->log, "RvH323FwNatOpenH245Server"));

    fnConn = (FwNatConn *)emaAdd(mgr->hFwNatConns, NULL);
    if (fnConn == NULL)
    {
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatOpenH245Server: cannot allocate FwNatConn"));
        return RV_ERROR_OUTOFRESOURCES;
    }

    status = cmProtConnNew(mgr->hApp, cmProtocolH245, RV_TRUE, h245MultiServerAcceptHandler, fnConn,
        NULL, addr, &hConn);

    if (status == RV_OK)
    {
        mgr->callbacks.fwNatSetProtConnHandleEv(hConn, (HH323FWNATCONN)fnConn);
        memset(fnConn, 0, sizeof(FwNatConn));
        fnConn->connType = RvH323FwNatConnTypeH245MultiHost;
        fnConn->protConn = hConn;
        *hFwNatConn = (HH323FWNATCONN)fnConn;
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatOpenH245Server: connection %p created", fnConn));
    }
    else
    {
        emaDelete(fnConn);
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatOpenH245Server: error opening connection (%d)", status));
    }
    return status;
}


/********************************************************************************
 * Function Name: RvH323FwNatSetCallH245Server
 * Description:  Set an H.245 multi-server as the call's H.245 connection.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hFwNatServ    - Handle of the server
 *          hsCall       - Stack's call handle
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatSetCallH245Server(
    IN  HH323FWNATMGR       hFwNat,
    IN  HH323FWNATCONN      hFwNatServ,
    IN  HCALL               hsCall)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    FwNatConn * fnConn = (FwNatConn *)hFwNatServ;

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;
    RvLogEnter(&mgr->log, (&mgr->log, "RvH323FwNatSetCallH245Server"));

    if (!emaLock((EMAElement)fnConn))
    {
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatSetCallH245Server: connection handle is invalid (%p)", fnConn));
        return RV_ERROR_DESTRUCTED;
    }

    if (fnConn->connType != RvH323FwNatConnTypeH245MultiHost)
    {
        /* wrong type */
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatSetCallH245Server: connection %p is of wrong type", fnConn));
        emaUnlock((EMAElement)fnConn);
        return RV_ERROR_UNKNOWN;
    }

    /* mark connection as used by a call */
    fnConn->numRelatedCalls++;
    mgr->callbacks.fwNatSetByCallEv(mgr->hApp, hFwNat, hsCall,
        (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall), hFwNatServ, 0);
    RvLogLeave(&mgr->log, (&mgr->log,
        "RvH323FwNatSetCallH245Server: connection %p set for call %p", fnConn, hsCall));
    emaUnlock((EMAElement)fnConn);
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatConnCallTerminated
 * Description:  Notify the FW/NAT module that a the connection's call has been
 *               terminated.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hFwNatConn   - Handle of the connection
 *          hsCall       - Stack's call handle
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatConnCallTerminated(
    IN  HH323FWNATMGR       hFwNat,
    IN  HH323FWNATCONN      hFwNatServ,
    IN  HCALL               hsCall)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    FwNatConn * fnConn = (FwNatConn *)hFwNatServ;
    RV_UNUSED_ARG(hsCall);

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;
    RvLogEnter(&mgr->log, (&mgr->log, "RvH323FwNatConnCallTerminated"));

    if (!emaLock((EMAElement)fnConn))
    {
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatConnCallTerminated: connection handle is invalid (%p)", fnConn));
        return RV_ERROR_DESTRUCTED;
    }

    if (fnConn->connType == RvH323FwNatConnTypeH245MultiHost)
    {
        /* reduce number of using calls */
        fnConn->numRelatedCalls--;
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatConnCallTerminated: connection %p call %p terminated, %d remaining calls",
            fnConn, hsCall, fnConn->numRelatedCalls));
        emaUnlock((EMAElement)fnConn);
        return RV_OK;
    }

    if ((fnConn->connType == RvH323FwNatConnTypeReverseCallOutgoing) ||
        (fnConn->connType == RvH323FwNatConnTypeReverseCallIncoming))
    {
        /*remove the connection*/
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatConnCallTerminated: connection %p used for reverse call (%d)",
            fnConn, fnConn->connType));
        emaDelete((EMAElement)fnConn);
        emaUnlock((EMAElement)fnConn);
        return RV_OK;
    }

    RvLogLeave(&mgr->log, (&mgr->log,
        "RvH323FwNatConnCallTerminated: connection %p unknown type (%d), not deleted",
        fnConn, fnConn->connType));
    emaUnlock((EMAElement)fnConn);
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatCloseH245Server
 * Description:  Close an H.245 multi-server connection. It is the application's
 *               responsibility to remove the server from the calls it was set to.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hFwNatServ    - Handle of the server
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatCloseH245Server(
    IN  HH323FWNATMGR       hFwNat,
    IN  HH323FWNATCONN      hFwNatServ)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    FwNatConn * fnConn = (FwNatConn *)hFwNatServ;

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;
    RvLogEnter(&mgr->log, (&mgr->log, "RvH323FwNatCloseH245Server"));

    if (!emaLock((EMAElement)fnConn))
    {
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatCloseH245Server: connection handle is invalid (%p)", fnConn));
        return RV_ERROR_DESTRUCTED;
    }

    if (fnConn->connType != RvH323FwNatConnTypeH245MultiHost)
    {
        /* wrong type */
        RvLogLeave(&mgr->log, (&mgr->log,
            "RvH323FwNatCloseH245Server: connection %p is of wrong type", fnConn));
        emaUnlock((EMAElement)fnConn);
        return RV_ERROR_UNKNOWN;
    }

    /* check if the host is in use */
    if (fnConn->numRelatedCalls != 0)
    {
        RvLogError(&mgr->log, (&mgr->log,
            "RvH323FwNatCloseH245Server: connection %p is in use %d times", fnConn, fnConn->numRelatedCalls));
    }
    RvLogLeave(&mgr->log, (&mgr->log,
        "RvH323FwNatCloseH245Server: connection %p terminated", fnConn));
    cmProtConnClose(mgr->hApp, fnConn->protConn);
    emaDelete((EMAElement)fnConn);
    emaUnlock((EMAElement)fnConn);
    return RV_OK;
}


/********************************************************************************
 * Function Name: RvH323FwNatCallSendMessage
 * Description: This function should be called from cmEvCallSendMessage(). it
 *              may add FwNat information to the message.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsCall       - Stack's handle for the call
 *          hFwNatConn   - The FwNat connection related to the call, if one
 *                         was set. NULL o.w.
 *          message      - PVT node ID of the message that is about to be sent
 * Output:  bInfo        - RV_TRUE iff the message was added with any information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatCallSendMessage(
    IN  HH323FWNATMGR       hFwNat,
    IN  HCALL               hsCall,
    IN  HH323FWNATCONN      hFwNatConn,
    IN  RvPvtNodeId         message,
    OUT RvBool *            bInfo)
{
    HPVT hVal;
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    FwNatConn * fnConn = (FwNatConn *)hFwNatConn;
    RvStatus status = RV_OK;

    RV_UNUSED_ARG(hsCall);

    *bInfo = RV_FALSE;

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;

    hVal = cmGetValTree(mgr->hApp);

    if (emaLock((EMAElement)fnConn))
    {
        if (fnConn->connType == RvH323FwNatConnTypeH245MultiHost)
        {
            /* check if we should change an address */
            RvPvtNodeId addrNode;

            __pvtGetNodeIdByFieldIds(addrNode, hVal, message,
                { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
                  _q931(h323_uu_pdu) _q931(h323_message_body)  _anyField _q931(h245Address) LAST_TOKEN});
            if (RV_PVT_NODEID_IS_VALID(addrNode))
            {
                /* replace the address */
                cmTransportAddress replaceAddress;

                *bInfo = RV_TRUE;

                cmProtocolGetLocalAddress(mgr->hApp, fnConn->protConn, &replaceAddress);
                status = cmTransportAddress2Vt(hVal, &replaceAddress, addrNode);
            }
        }
        emaUnlock((EMAElement)fnConn);
    }

    /*Add H.460_19 information*/
    if (mgr->e19State != RvH323FwNatH46019StateDisabled)
    {
        RvPvtNodeId     msgNode, reasonNodeId;
        RvPstFieldId    fieldId, reasonFieldId = RV_PVT_INVALID_NODEID;


        /*find what type of Q931 message we're handling*/
        __pvtGetByFieldIds(msgNode, hVal, message, { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
                         _q931(h323_uu_pdu) _q931(h323_message_body) _anyField LAST_TOKEN}, &fieldId, NULL, NULL);

        if (fieldId == __q931(facility))
        {
            /*get facility reason*/
            __pvtGetByFieldIds(reasonNodeId, hVal, msgNode,
                        { _q931(reason) LAST_TOKEN },
                         &reasonFieldId, NULL, NULL);
        }

        /* add H46019 parameters if we have to*/
        if (fieldId == __q931(setup) || fieldId == __q931(callProceeding) ||
            fieldId == __q931(connect) || fieldId == __q931(alerting) ||
            (fieldId == __q931(facility) && reasonFieldId == __q931(forwardedElements)))
        {

            RvPvtNodeId supportedNode, neededNode, desiredNode;
            /*add mediaNATFWTraversal parameter to supportedFeatures*/
            status = cmAddFeature(mgr->hApp, message,
                                  RV_H323_FWNAT_SIGNALLING_MEDIA_TRAVERSAL_IDENTIFIER_VALUE,
                                  RV_TRUE, RV_FALSE, RV_FALSE,
                                  &supportedNode,
                                  &neededNode,
                                  &desiredNode);

            if (status != RV_OK)
                return status;

            if (mgr->e19State == RvH323FwNatH46019StateServer)
            {
                /*add mediaTraversalServer*/
                status = cmAddFeatureParameter(mgr->hApp,
                                               supportedNode,
                                               RV_H323_FWNAT_SIGNALLING_MEDIA_TRAVERSAL_SEVER_IDENTIFIER_VALUE);

                if (status != RV_OK)
                    return RV_ERROR_UNKNOWN;
            }

            if (mgr->e19State == RvH323FwNatH46019StateClient ||
                (mgr->e19State == RvH323FwNatH46019StateServer &&
                 mgr->mediaMultiplexMode & RV_H323_FWNAT_H46019_MULTIPLEX_MODE_TRANSMIT))
            {
                /*add supportMultiplexedTransmitMedia*/
                status = cmAddFeatureParameter(mgr->hApp,
                                               supportedNode,
                                               RV_H323_FWNAT_SIGNALLING_SUPPORT_TRANSMIT_MULTIPLEXED_MEDIA_VALUE);
                if (status != RV_OK)
                    return status;
            }

            *bInfo = RV_TRUE;
        }
    }

    return status;
}

/********************************************************************************
 * Function Name: RvH323FwNatCallRecvMessage
 * Description: This function should be called from cmEvCallRecvMessage(). It
 *              may retrieve FwNat information from the message.
 *
 * Input:   hFwNat                          - FwNat module handle
 *          hsCall                          - Stack's handle for the call
 *          hFwNatConn                      - The FwNat connection related to the call, if one
 *                                            was set. NULL o.w.
 *          message                         - PVT node ID of the message that is about to be sent
 * Output:  bInfo                           - RV_TRUE if H46019 info received.
 *          bUseH46019                      - will be set to RV_TRUE if the procedures of h46019
 *                                            can be used in this call.
 *          bRemoteMediaMultiplexEnabled    - RV_TRUE if the remote peer supports
 *                                            multiplexed media transmission.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatCallRecvMessage(
    IN  HH323FWNATMGR         hFwNat,
    IN  HCALL                 hsCall,
    IN  HH323FWNATCONN        hFwNatConn,
    IN  RvPvtNodeId           message,
    OUT RvBool *              bInfo,
    OUT RvBool *              bUseH46019,
    OUT RvBool *              bRemoteMediaMultiplexEnabled)
{
    HPVT hVal;
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    *bInfo = RV_FALSE;

    RV_UNUSED_ARG(hFwNatConn);
    RV_UNUSED_ARG(hsCall);

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;

    hVal = cmGetValTree(mgr->hApp);

    if (mgr->e19State != RvH323FwNatH46019StateDisabled)
    {
        RvPvtNodeId     msgNode;
        RvPstFieldId    fieldId, reasonFieldId = RV_PVT_INVALID_NODEID;

        /*check what type of message we're dealing with*/
        __pvtGetByFieldIds(msgNode, hVal, message, { _q931(message) _anyField LAST_TOKEN }, &fieldId, NULL, NULL);

        if (fieldId == __q931(facility))
        {
            /*get facility reason*/
             __pvtGetByFieldIds(msgNode, hVal, message,
                { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
                 _q931(h323_uu_pdu) _q931(h323_message_body) _q931(facility) _q931(reason) LAST_TOKEN },
                 &reasonFieldId, NULL, NULL);
        }

        /* look for H46019 parameters*/
        if (fieldId == __q931(setup) || fieldId == __q931(callProceeding) ||
            fieldId == __q931(connect) || fieldId == __q931(alerting) ||
            (fieldId == __q931(facility) && reasonFieldId == __q931(forwardedElements)))
        {
            RvBool  remoteH46019Enabled     = RV_FALSE;
            RvBool  bDesired                = RV_FALSE;
            RvBool  bNeeded                 = RV_FALSE;
            RvBool  remoteIsServer          = RV_FALSE;
            RvBool  remoteMultiplexEnabled  = RV_FALSE;
            RvPvtNodeId supportedNode, neededNode, desiredNode;

            cmIsFoundFeature(mgr->hApp,
                            message,
                            RV_H323_FWNAT_SIGNALLING_MEDIA_TRAVERSAL_IDENTIFIER_VALUE,
                            &remoteH46019Enabled,
                            &bNeeded,
                            &bDesired,
                            &supportedNode,
                            &neededNode,
                            &desiredNode);

            if (!remoteH46019Enabled)
            {
                /*no signaling of h46019*/
                *bUseH46019 = RV_FALSE;
                *bRemoteMediaMultiplexEnabled = RV_FALSE;
                *bInfo = RV_TRUE;
                return RV_OK;
            }

            /*look for mediaTraversalServer parameter*/
            remoteIsServer = cmIsFoundFeatureParameter(mgr->hApp,
                                                       supportedNode, 
                                                       RV_H323_FWNAT_SIGNALLING_MEDIA_TRAVERSAL_SEVER_IDENTIFIER_VALUE);

            remoteMultiplexEnabled = cmIsFoundFeatureParameter(mgr->hApp,
                                                               supportedNode, 
                                                               RV_H323_FWNAT_SIGNALLING_SUPPORT_TRANSMIT_MULTIPLEXED_MEDIA_VALUE);
            
            if (!remoteIsServer && !remoteMultiplexEnabled)
                /*clients must signal supportTransmitMultiplexedMedia*/
                return RV_ERROR_UNKNOWN;

             /*if we are a client - make sure the other side is a server
               if we're a serve make sure the other side is a client*/
            if ((mgr->e19State == RvH323FwNatH46019StateClient && remoteIsServer) ||
                (mgr->e19State == RvH323FwNatH46019StateServer && !remoteIsServer))
                *bUseH46019 = RV_TRUE;
            else
                *bUseH46019 = RV_FALSE;

            /*if we can not de-multiplex we don't want to know if the other side
              can do it or not*/
            if (!(mgr->mediaMultiplexMode & RV_H323_FWNAT_H46019_MULTIPLEX_MODE_RECEIVE))
                *bRemoteMediaMultiplexEnabled = RV_FALSE;
            else
                *bRemoteMediaMultiplexEnabled = remoteMultiplexEnabled;
            *bInfo = RV_TRUE;
        }
    }
    return RV_OK;
}

/********************************************************************************
 * Function Name: RvH323FwNatChannelSendMessage
 * Description: This function should be called from cmEvChannelSendMessage(). it
 *              may add FwNat information to the message if it is an OLC or
 *              an OLCA.
 * NOTE: The application should supply the correct rtpMuxAddr, rctpMuxAddr and
 *       multiplexID.
 *
 * Input:   hFwNat                      - FwNat module handle
 *          hsChan                      - Stack's handle for the channel.
 *          message                     - PVT node ID of the message that is
 *                                        about to be sent
 *          traversalParams             - Traversal Parameters to add to OLC/OLCA.
 *          bUseH46019                  - Indication if h46019 procedures are used
 *                                        in the call.
 *          bRemoteMultiplexEnabled    - Indication if the remote side supports
 *                                        transmission of multiplexed media.
 * Output:  bInfo        - RV_TRUE iff the message was added with any information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatChannelSendMessage(
    IN  HH323FWNATMGR                   hFwNat,
    IN  HCHAN                           hsChan,
    IN  RvPvtNodeId                     message,
    IN  RvH323FwNatH46019traversalParams * traversalParams,
    IN  RvBool                          bUseH46019,
    IN  RvBool                          bRemoteMultiplexEnabled,
    OUT RvBool *                        bInfo)
{
    return RvH323FwNatAddChannelH46019Info(hFwNat, hsChan, message, traversalParams,
        bUseH46019, bRemoteMultiplexEnabled, RV_FALSE, dirTransmit, bInfo);
}

/********************************************************************************
 * Function Name: RvH323FwNatAddChannelH46019Info
 * Description: This function should be called before sending OLC (proposal/response)
 *              in case of fast connect call it may add FwNat information to the OLC
 *              (proposal/response). This function should be called before
 *              each time cmCallAddFastStartMessage API is called.
 * NOTE: The application should supply the correct Traversal Parameters.
 *
 * Input:   hFwNat                      - FwNat module handle
 *          hsChan                      - Stack's handle for the channel.
 *          message                     - PVT node ID of the OLC/OLCA that is
 *                                        about to be sent
 *          traversalParams             - The Traversal Parameters to add to OLC/OLCA.
 *          bUseH46019                  - Indication if h46019 procedures are used
 *                                        in the call.
 *          bbRemoteMultiplexEnabled    - Indication if the remote side supports
 *                                        transmission of multiplexed media.
 *          bFastConnectCall            - RV_TRUE if call is using fast connect
 *                                        procedures.
 *          channelDirection            - dirTransmit or dirReceive for FS calls.
 * Output:  bInfo        - RV_TRUE iff the message was added with any information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatAddChannelH46019Info(
    IN  HH323FWNATMGR                   hFwNat,
    IN  HCHAN                           hsChan,
    IN  RvPvtNodeId                     message,
    IN  RvH323FwNatH46019traversalParams * traversalParams,
    IN  RvBool                          bUseH46019,
    IN  RvBool                          bRemoteMultiplexEnabled,
    IN  RvBool                          bFastConnectCall,
    IN  cmChannelDirection              channelDirection,
    OUT RvBool *                        bInfo)
{
    HPVT hVal;
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    RvPstFieldId    fieldId = -1;
    RvPvtNodeId     msgNode;

    *bInfo = RV_FALSE;
    RV_UNUSED_ARG(hsChan);

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;

    if (mgr->e19State == RvH323FwNatH46019StateDisabled)
        return RV_OK;

    hVal = cmGetValTree(mgr->hApp);

    if (!bUseH46019 && !bRemoteMultiplexEnabled && !bFastConnectCall)
        return RV_OK;

    /*check the type of the message we are dealing with*/
    if (bFastConnectCall == RV_TRUE)
    {
        msgNode = message;
        if (channelDirection == dirTransmit)
            fieldId = __h245(openLogicalChannel);
        else if (channelDirection == dirReceive)
            fieldId = __h245(openLogicalChannelAck);
    }
    else
    {
        __pvtGetByFieldIds(msgNode, hVal, message,
        {_anyField _anyField LAST_TOKEN}, &fieldId, NULL, NULL);
    }
    
    if (fieldId == __h245(openLogicalChannel) || fieldId == __h245(openLogicalChannelAck))
    {
        RvPvtNodeId traversalParamsNodeId;
        if (traversalParams == NULL)
        {
            /*Nothing to put in the traversal parameters just 
            clean up existing traversal parameters from the messages*/
            removeTraversalParams(hVal, msgNode);
            return RV_OK;

        }

        /*create traversalParameters node*/
        traversalParamsNodeId = pvtAddRoot(hVal, mgr->TraversalParamsSyn, 0, NULL);

        if (!RV_PVT_NODEID_IS_VALID(traversalParamsNodeId))
            return RV_ERROR_UNKNOWN;

        /*Add keepAliveChannel if we have to*/
        if ((mgr->e19State == RvH323FwNatH46019StateServer) &&
            (traversalParams->keepAliveAddr != NULL) 
            && bUseH46019)
        {
            addKeepAliveChannelInfo(hVal,
                                    traversalParamsNodeId,
                                    traversalParams->keepAliveAddr,
                                    traversalParams->keepAlivePayLoadType,
                                    traversalParams->keepAliveInterval);
            *bInfo = RV_TRUE;
        }

        /*check if we need to request a multiplex mode*/
        if ((bRemoteMultiplexEnabled || bFastConnectCall) && (mgr->mediaMultiplexMode & RV_H323_FWNAT_H46019_MULTIPLEX_MODE_RECEIVE))
        {
            addMultiplexInfo(hVal,
                             fieldId,
                             traversalParamsNodeId,
                             traversalParams->multiplexID,
                             traversalParams->rtpMuxAddr,
                             traversalParams->rctpMuxAddr);
            *bInfo = RV_TRUE;
        }
        
        if (*bInfo != RV_FALSE)
        {
            RvLogDebug(&mgr->log, (&mgr->log, "RvH323FwNatAddChannelH46019Info: Added traversalParameters:"));
            pvtPrintInternal(hVal, traversalParamsNodeId, &mgr->log, 2);
            /*encode and add the TraversalParameters node to the message*/
            encodeAndAddTraversalParams(hVal, msgNode, traversalParamsNodeId);
        }

        pvtDelete(hVal, traversalParamsNodeId);
    }

    return RV_OK;
}

/********************************************************************************
 * Function Name: RvH323FwNatChannelRecvMessage
 * Description: This function should be called from cmEvChannelRecvMessage(). it
 *              extracts FwNat information from the message if it is an OLC
 *              or an OLCA. This function should be called even if an h46019
 *              entity does not have multiplexing capabilities.
 *
 * Input:   hFwNat              - FwNat module handle
 *          hsChan              - Stack's handle for the channel.
 *          message             - PVT node ID of the message that is about to be sent
 *          bFastConnectCall    - RV_TRUE if call is using fast connect
 *                                procedures.
 *          channelDirection    - dirTransmit or dirReceive for FS calls.
 * Output:  traversalParams     - Traversal Parameters retrieved from OLC/OLCA
 *          bInfo               - RV_TRUE iff the message contained any relevant
 *                                information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatChannelRecvMessage(
    IN  HH323FWNATMGR                   hFwNat,
    IN  HCHAN                           hsChan,
    IN  RvPvtNodeId                     message,
    IN  RvBool                          bFastConnectCall,
    IN  cmChannelDirection              channelDirection,
    OUT RvH323FwNatH46019traversalParams * traversalParams,
    OUT RvBool *                        bInfo)
{
    return RvH323FwNatGetChannelH46019Info(hFwNat,
                                           hsChan,
                                           message,
                                           bFastConnectCall,
                                           channelDirection,
                                           traversalParams,
                                           bInfo);
}

/********************************************************************************
 * Function Name: RvH323FwNatGetChannelH46019Info
 * Description: This function should be called for eack OLC/OLCA received in a 
 *              fast connect call.
 *              It extracts FwNat information from the OLC (proposal/response) messages
 *              This function should be called even if an h46019
 *              entity does not have multiplexing capabilities.
 *
 * Input:   hFwNat              - FwNat module handle
 *          hsChan              - Stack's handle for the channel.
 *          message             - PVT node ID of the message that is about to be sent
 *          bFastConnectCall    - RV_TRUE if call is using fast connect
 *                                procedures.
 *          channelDirection    - dirTransmit or dirReceive for FS calls.
 * Output:  traversalParams     - Traversal Parameters from OLC/OLCA
 *          bInfo               - RV_TRUE iff the message contained any relevant
 *                         information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatGetChannelH46019Info(
    IN  HH323FWNATMGR                   hFwNat,
    IN  HCHAN                           hsChan,
    IN  RvPvtNodeId                     message,
    IN  RvBool                          bFastConnectCall,
    IN  cmChannelDirection              channelDirection,
    OUT RvH323FwNatH46019traversalParams * traversalParams,
    OUT RvBool *                        bInfo)
{
    HPVT hVal;
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    RvPstFieldId    fieldId = RV_PVT_INVALID_NODEID;
    RvPvtNodeId     msgNode, traversalParamsNodeId;

    *bInfo = RV_FALSE;
    RV_UNUSED_ARG(hsChan);

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;

    if (mgr->e19State == RvH323FwNatH46019StateDisabled)
        return RV_OK;

    hVal = cmGetValTree(mgr->hApp);

    /*find the type of message we're dealing with*/
    if (bFastConnectCall == RV_TRUE)
    {
        msgNode = message;
        if (channelDirection == dirTransmit)
            fieldId = __h245(openLogicalChannelAck);
        else if (channelDirection == dirReceive)
            fieldId = __h245(openLogicalChannel);
    }
    else
    {
        __pvtGetByFieldIds(msgNode, hVal, message,
        {_anyField _anyField LAST_TOKEN}, &fieldId, NULL, NULL);
    }
    
    /*check the type of the message we are dealing with*/
    if (fieldId == __h245(openLogicalChannel) || fieldId == __h245(openLogicalChannelAck))
    {
        traversalParamsNodeId = pvtAddRoot(hVal, mgr->TraversalParamsSyn, 0, NULL);
        if (!RV_PVT_NODEID_IS_VALID(traversalParamsNodeId))
            return RV_ERROR_UNKNOWN;
        /*extract TraversalParameters node from message*/
        decodeAndGetTraversalParams(hVal, msgNode, &traversalParamsNodeId);
        if (!RV_PVT_NODEID_IS_VALID(traversalParamsNodeId))
            return RV_OK;
        *bInfo = RV_TRUE;

        RvLogDebug(&mgr->log, (&mgr->log, "RvH323FwNatGetChannelH46019Info: Received traversalParameters:"));
        pvtPrintInternal(hVal, traversalParamsNodeId, &mgr->log, 2);

        /*extract keepAliveChannel if we have to*/
        if (mgr->e19State == RvH323FwNatH46019StateClient && traversalParams->keepAliveAddr != NULL)
            getKeepAliveChannelInfo(hVal,
                                    traversalParamsNodeId,
                                    traversalParams->keepAliveAddr,
                                    &(traversalParams->keepAlivePayLoadType),
                                    &(traversalParams->keepAliveInterval));

        if (mgr->mediaMultiplexMode & RV_H323_FWNAT_H46019_MULTIPLEX_MODE_TRANSMIT)
        {
            /*extract multiplex information*/
            getMultiplexInfo(hVal,
                             traversalParamsNodeId,
                             &(traversalParams->multiplexID),
                             traversalParams->rtpMuxAddr,
                             traversalParams->rctpMuxAddr);
        }
        pvtDelete(hVal, traversalParamsNodeId);
    }

    return RV_OK;
}

/********************************************************************************
 * Function Name: RvH323FwNatHookOutConn
 * Description: This function should be called from cmHookOutConn().
 *
 * Input:   hFwNat       - FwNat module handle
 *          hConn        - Stack's handle for the connection.
 *          addrFrom     - PVT node ID of the local address
 *          addrTo       - PVT node ID of the remote address
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookOutConn(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn,
    IN  RvPvtNodeId         addrFrom,
    IN  RvPvtNodeId         addrTo)
{
    FwNatMgr * mgr = (FwNatMgr *)hFwNat;
    RvPvtNodeId genIndMsg;
    HCALL hsCall;
    RvUint8 CID[16];
    RvInt32 sz = 16;
    RvBool bOrigin;
    RvStatus status;
    cmProtocol type = cmProtocolUnknown;

    RV_UNUSED_ARG(addrFrom);
    RV_UNUSED_ARG(addrTo);

    if (mgr == NULL)
        return RV_ERROR_NULLPTR;
    if (mgr->e18State != RvH323FwNatH46018StateSupportedEP)
        return RV_OK;

    status = cmProtConnGetParam(mgr->hApp, hConn, cmProtConnParamType, (RvInt32*)&type, NULL);
    if ((status != RV_OK) || (type != cmProtocolH245))
        return RV_OK;

    hsCall = cmGetCallByH245Chan(hConn);
    if (hsCall == NULL)
        return RV_ERROR_UNKNOWN;

    cmCallGetParam(hsCall, cmParamCallID, 0, &sz, (char*)CID);
    cmCallGetOrigin(hsCall, &bOrigin);
    genIndMsg = buildGenericIndicationMessage(mgr, CID, !bOrigin);

    status = cmProtocolSendMessage(mgr->hApp, hConn, genIndMsg);
    pvtDelete(cmGetValTree(mgr->hApp), genIndMsg);
    return status;
}


#ifdef __cplusplus
}
#endif
