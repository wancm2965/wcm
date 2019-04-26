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

/***********************************************************************
epHooks.c

Handle stack hooks.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/

#include "q931asn1.h"
#include "h245.h"

#include "lepUtils.h"
#include "lepHooks.h"
#include "lepFwNat.h"


#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/





/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/


static EpConnectionInfo * allocateEpConnection(EpObj * ep, HPROTCONN hConn, RvBool bIncoming);

static void reportEpConnection(EpObj * ep, EpConnectionInfo * appCon, RvBool bIncoming, int error);



/*-----------------------------------------------------------------------*/
/*                           CALLBACK FUNCTIONS                          */
/*-----------------------------------------------------------------------*/


int RVCALLCONV hookInConn(
    IN      HPROTCONN           hConn,
    IN      int                 addrFrom,
    IN      int                 addrTo)
{
    EpObj *ep;
    EpConnectionInfo    *appCon;
	
    RV_UNUSED_ARG(addrFrom);
    RV_UNUSED_ARG(addrTo);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

    if ((cmGetTpktChanApplHandle(hConn, (HAPPCONN*)&appCon) < 0) || (appCon == NULL))
    {
        appCon = allocateEpConnection(ep, hConn, RV_TRUE);
    }

    reportEpConnection(ep, appCon, RV_TRUE, 0);

    return 0;
}


int RVCALLCONV hookOutConn(
    IN      HPROTCONN           hConn,
    IN      int                 addrFrom,
    IN      int                 addrTo,
    IN      RvBool              error)
{
    EpObj *ep;
    EpConnectionInfo    *appCon;
    RvStatus status;

    RV_UNUSED_ARG(addrFrom);
    RV_UNUSED_ARG(addrTo);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

    if ((cmGetTpktChanApplHandle(hConn, (HAPPCONN*)&appCon) < 0) || (appCon == NULL))
    {
        appCon = allocateEpConnection(ep, hConn, RV_FALSE);
    }

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);
    status = epFwNatHookOutConn(hConn, addrFrom, addrTo, error);
    epUtilsEvent(ep, "Hooks", NULL, "hookOutConn.%d.%d", appCon->id, error);
    reportEpConnection(ep, appCon, RV_FALSE, error);

    return 0;
}


void RVCALLCONV hookClose(
    IN      HPROTCONN           hConn)
{
    EpObj *ep;
    EpConnectionInfo    *appCon;
    RvStatus status;

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

    status = epFwNatHookClose(hConn);

    if (cmGetTpktChanApplHandle(hConn, (HAPPCONN*)&appCon) >= 0)
    {
        if (appCon != NULL)
        {
            epUtilsEvent(ep, "Hooks", NULL, "hookClose.%s.%d", (appCon->inConn ? "in" : "out"), appCon->id);
            ep->cb.epFree(ep, appCon);
        }
    }
}


RvBool RVCALLCONV hookSend(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      RvBool              error)
{
    EpObj *ep;
    RvBool suppress = RV_FALSE;

    RV_UNUSED_ARG(error);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

    if (ep->cfg.bScanNodeIDs)
        epScanPvtNode(ep, nodeId);

    if (ep->cfg.bSuppressOutgoingMessages)
        return RV_TRUE;


    if (ep->cfg.bSuppressReleaseComplete || ep->cfg.bSuppressEndSessionCommand ||
        ep->cfg.bSuppressQ931 ||  ep->cfg.bSuppressH245)
    {
        cmProtocol prot = cmProtocolGetProtocol(ep->hApp, nodeId);
        RvPvtNodeId tmpNodeId = RV_PVT_INVALID_NODEID;

        if (prot == cmProtocolQ931)
        {
            if (ep->cfg.bSuppressQ931)
            {
                tmpNodeId = nodeId; /* A valid node id */
            }
            else if (ep->cfg.bSuppressReleaseComplete)
            {
                __pvtGetNodeIdByFieldIds(tmpNodeId, ep->hVal, nodeId, {_q931(message) _q931(releaseComplete) LAST_TOKEN});
            }
        }
        else if (prot == cmProtocolH245)
        {
            if (ep->cfg.bSuppressH245)
            {
                tmpNodeId = nodeId; /* A valid node id */
            }
            else if (ep->cfg.bSuppressEndSessionCommand)
            {
                __pvtGetNodeIdByFieldIds(tmpNodeId, ep->hVal, nodeId, {_h245(command) _h245(endSessionCommand) LAST_TOKEN});
            }
        }

        suppress = RV_PVT_NODEID_IS_VALID(tmpNodeId);
    }

    return suppress;
}


RvBool RVCALLCONV hookRecv(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      RvBool              error)
{
    EpObj *ep;
    RvBool suppress = RV_FALSE;
    RvBool bIgnore = RV_FALSE;

    RV_UNUSED_ARG(bIgnore);
    RV_UNUSED_ARG(error);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

    bIgnore = epFwNatHookRecv(hConn, nodeId, error);

    if (ep->cfg.bScanNodeIDs)
        epScanPvtNode(ep, nodeId);

    if (ep->cfg.bSuppressIncomingMessages)
        return RV_TRUE;

    if (ep->cfg.ignoreIncomingFid >= 0)
    {
        cmProtocol prot = cmProtocolGetProtocol(ep->hApp, nodeId);
        RvPvtNodeId tmpNodeId = RV_PVT_INVALID_NODEID;

        if (prot == cmProtocolQ931)
        {
            tmpNodeId = pvtGetChild(ep->hVal, pvtGetChild(ep->hVal, nodeId, __q931(message), NULL), ep->cfg.ignoreIncomingFid, NULL);
        }
        else if (prot == cmProtocolH245)
        {
            tmpNodeId = pvtGetChild(ep->hVal, pvtChild(ep->hVal, nodeId), ep->cfg.ignoreIncomingFid, NULL);
        }

        suppress = RV_PVT_NODEID_IS_VALID(tmpNodeId);
    }

    return suppress;
}


RvBool RVCALLCONV hookSendTo(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      int                 nodeIdTo,
    IN      RvBool              error)
{
    EpObj *ep;
    RvBool suppress = RV_FALSE;

    RV_UNUSED_ARG(nodeId);
    RV_UNUSED_ARG(nodeIdTo);
    RV_UNUSED_ARG(error);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

    if (ep->cfg.bScanNodeIDs)
        epScanPvtNode(ep, nodeId);

    if (ep->cfg.bSuppressRAS)
    {
        suppress = RV_TRUE;
    }
    else
    {
        if (ep->cfg.typeOfService > 0)
            cmProtocolSetTypeOfService(ep->hApp, hConn, ep->cfg.typeOfService);
    }

    /* If we're trying to send a URQ - see if it's partial */
    if (!strcmp("unregistrationRequest", cmGetProtocolMessageName(ep->hApp, nodeId)))
    {
        if (RV_PVT_NODEID_IS_VALID(ep->partialUrqAliases))
        {
            /* We need to send a partial URQ only */
            RvPvtNodeId tmpNodeId;
            __pvtBuildByFieldIds(tmpNodeId, ep->hVal, nodeId,
                {_q931(unregistrationRequest) _q931(endpointAlias) LAST_TOKEN}, 0, NULL);
            pvtMoveTree(ep->hVal, tmpNodeId, ep->partialUrqAliases);
            ep->partialUrqAliases = RV_PVT_INVALID_NODEID;
        }
    }
    return suppress;
}


RvBool RVCALLCONV hookRecvFrom(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      int                 addrFrom,
    IN      RvBool              multicast,
    IN      RvBool              error)
{
    RvChar      *messageStr;
    EpObj       *ep;
    RvStatus    status = RV_OK;
    RvBool      bIgnore = RV_FALSE;

    RV_UNUSED_ARG(addrFrom);
    RV_UNUSED_ARG(multicast);
    RV_UNUSED_ARG(error);
    RV_UNUSED_ARG(status);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);


    /* Reset the H460.18 if we have to */
    bIgnore = epFwNatHookRecvFrom(hConn, nodeId, addrFrom, multicast, error);

    messageStr = cmGetProtocolMessageName(ep->hApp, nodeId);
    if (strcmp(messageStr, "requestInProgress") == 0)
    {
        epUtilsLog(ep, NULL, "New message recv <- requestInProgress on RAS");
    }
    return RV_FALSE;
}


int RVCALLCONV hookConnecting(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId)
{
    EpObj *ep;
    EpConnectionInfo    *appCon;

    RV_UNUSED_ARG(nodeId);

    if ((cmGetTpktChanApplHandle(hConn, (HAPPCONN*)&appCon) >= 0) && (appCon != NULL))
        return 0;

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

    appCon = allocateEpConnection(ep, hConn, RV_FALSE);

    if (ep->cfg.typeOfService > 0)
        cmProtocolSetTypeOfService(ep->hApp, hConn, ep->cfg.typeOfService);
    
    return RV_FALSE;
}


int RVCALLCONV hookListening(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      RvBool              error)
{
    RV_UNUSED_ARG(nodeId);

    if (!error)
    {
        EpObj *ep;
        EpConnectionInfo    *appCon;

        cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

        if (ep->cfg.typeOfService > 0)
            cmProtocolSetTypeOfService(ep->hApp, hConn, ep->cfg.typeOfService);

        if ((cmGetTpktChanApplHandle(hConn, (HAPPCONN*)&appCon) >= 0) && (appCon != NULL))
            return 0;

        cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

        appCon = allocateEpConnection(ep, hConn, RV_TRUE);
    }

    return RV_FALSE;
}


RvBool RVCALLCONV hookSendRaw(
    IN      HPROTCONN           hConn,
    IN      const RvUint8 *     rawMessage,
    IN      RvInt32             messageLen)
{
    RvBool bStop = RV_FALSE;
    EpObj *ep;

    RV_UNUSED_ARG(rawMessage);
    RV_UNUSED_ARG(messageLen);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);
    bStop = epFwNatHookSendRaw(hConn, rawMessage, messageLen);

    return bStop;
}


RvBool RVCALLCONV hookSendRawTo(
    IN      HPROTCONN           hConn,
    IN      const RvUint8 *     rawMessage,
    IN      RvInt32             messageLen,
    IN      cmTransportAddress* addrTo)
{
    RvBool bStop = RV_FALSE;
    EpObj *ep;

    RV_UNUSED_ARG(rawMessage);
    RV_UNUSED_ARG(messageLen);
    RV_UNUSED_ARG(addrTo);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);
    bStop = epFwNatHookSendRawTo(hConn, rawMessage, messageLen, addrTo);

    return bStop;
}



/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/************************************************************************
 * allocateEpConnection
 * purpose: Allocates and Initializes an EP connection object
 * input  : ep        - The endpoint object
 *          hConn     - The stack connection handle
 *          bIncoming - RV_TRUE iff is an incoming connection
 * output : none
 * return : The application connection object
 ************************************************************************/
static EpConnectionInfo * allocateEpConnection(EpObj * ep, HPROTCONN hConn, RvBool bIncoming)
{
    EpConnectionInfo * appCon = (EpConnectionInfo *)ep->cb.epMalloc(ep, sizeof(*appCon), __FILE__, __LINE__);
    cmSetTpktChanApplHandle(hConn, (HAPPCONN)appCon);

    appCon->color = 0;
    appCon->inConn = bIncoming;
    appCon->reported = RV_FALSE;

#ifdef USE_FWNAT
    appCon->hFwNatConn = NULL;
#endif
    return appCon;
}


/************************************************************************
 * reportEpConnection
 * purpose: Reports a connection to the AT
 * input  : ep        - The endpoint object
 *          appConn   - The application connection object
 *          bIncoming - RV_TRUE iff is an incoming connection
 * output : none
 * return : none
 ************************************************************************/
static void reportEpConnection(EpObj * ep, EpConnectionInfo * appCon, RvBool bIncoming, int error)
{
    if (!appCon->reported)
    {
        if (bIncoming)
        {
            lepLockEpForHash(ep, appCon->id = ep->cb.epAllocateResourceId(ep, EpResourceInConnection));
            epUtilsEvent(ep, "Hooks", NULL, "hookInConn.%d", appCon->id);
        }
        else
        {
            lepLockEpForHash(ep, appCon->id = ep->cb.epAllocateResourceId(ep, EpResourceOutConnection));
            epUtilsEvent(ep, "Hooks", NULL, "hookOutConn.%d.%d", appCon->id, error);
        }
        appCon->reported = RV_TRUE;
    }
}




#ifdef __cplusplus
}
#endif
