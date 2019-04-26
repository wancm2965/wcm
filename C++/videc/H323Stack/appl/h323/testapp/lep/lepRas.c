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
epRas.c

RAS module of the endpoint.
Handles the registrations and all other RAS transactions.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "q931asn1.h"
#include "stkutils.h"

#include "lepConversions.h"
#include "lepSecurity.h"
#include "lepUtils.h"
#include "lepRas.h"
#include "lepLdap.h"
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
static EpCallObj *getCallObjFromRas(
    IN HRAS hRas);
static void indicateIncomingMsg(
    IN EpObj        *ep,
    IN HRAS         hRas);




/*-----------------------------------------------------------------------*/
/*                           CALLBACK FUNCTIONS                          */
/*-----------------------------------------------------------------------*/


int RVCALLCONV cmEvRegEvent(
    IN HAPP               hApp,
    IN cmRegState         state,
    IN cmRegEvent         regEvent,
    IN int                regEventHandle)
{
    EpObj *ep;
    RvStatus status = RV_OK;
    RV_UNUSED_ARG(regEventHandle);

    cmGetHandle(hApp, (HAPPAPP*)&ep);

    epUtilsEvent(ep, "RAS", NULL, "cmEvRegEvent.%d.%d.%d", state, regEvent, regEventHandle >= 0);

    ep->cb.epRegistrationStatus(ep, state);

    switch (state)
    {
        case cmIdle:
            epUtilsLog(ep, NULL, "Not Registered");
            ep->bIsRegistered = RV_FALSE;

            /* reset the security sender ID */
            if (regEvent != cmPermanentAlternateGatekeeperConfirmation)
                status = epSecurityUpdateSettings(ep);

            if (ep->cfg.bPreventRereg)
                status = cmUnregister(ep->hApp);
            break;

        case cmRegistered:
        {
            cmTransportAddress rasAddr, q931Addr;
            char rasAddrStr[100], q931AddrStr[100];

            switch(regEvent)
            {
            case cmUnregistrationConfirm:
                {
                    /* this is a confirmation to a partial URQ */
                    epUtilsLog(ep, NULL, "Partial URQ confirmed");
                    break;
                }

            case cmPermanentAlternateGatekeeperConfirmation:
                {
                    epUtilsLog(ep, NULL, "Gatekeeper found");
                    break;
                }

            default:
                cmGetGKRASAddress(hApp, &rasAddr);
                cmGetGKCallSignalAddress(hApp, &q931Addr);
                cmTransportAddress2String(&rasAddr, sizeof(rasAddrStr), rasAddrStr);
                cmTransportAddress2String(&q931Addr, sizeof(q931AddrStr), q931AddrStr);
                epUtilsLog(ep, NULL, "Registered (RAS=%s, Q931=%s)", rasAddrStr, q931AddrStr);

                epSecurityGetSenderId(ep);

                ep->bIsRegistered = RV_TRUE;
                break;
            }
        }

        case cmDiscovered:
            epUtilsLog(ep, NULL, "Discovered");
            break;
    }

    return status;
}


int RVCALLCONV cmEvRASRequest(
    IN  HRAS             hsRas,
    IN  HCALL            hsCall,
    OUT LPHAPPRAS        lphaRas,
    IN  cmRASTransaction transaction,
    IN  cmRASTransport*  srcAddress,
    IN  HAPPCALL         haCall)
{
    EpObj *ep;
    char buf[128];
    RvBool bInfo = RV_FALSE, bIgnore = RV_FALSE;

    RV_UNUSED_ARG(lphaRas);
    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(bInfo);
    RV_UNUSED_ARG(bIgnore);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsRas), (HAPPAPP *)&ep);

    indicateIncomingMsg(ep, hsRas);

    cmTransportAddress2String(srcAddress, sizeof(buf), buf);

    epUtilsEvent(ep, "Script", (EpCallObj *)haCall, "cb:cmras:EvRequest %p %p %s {%s}",
        hsRas, haCall, RASTransaction2String(transaction), buf);

    /*handle SCI facility message if that's what it is*/
    bIgnore = epFwNatEvRASRequest(hsRas);

    return bIgnore;
}


int RVCALLCONV cmEvRASConfirm(
    IN HAPPRAS haRas,
    IN HRAS    hsRas)
{
    EpObj *ep;

    RV_UNUSED_ARG(haRas);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsRas), (HAPPAPP *)&ep);

    indicateIncomingMsg(ep, hsRas);

    epUtilsEvent(ep, "Script", NULL, "cb:cmras:EvConfirm %p", hsRas);

#ifdef USE_FWNAT
    RvH323FwNatEvRASConfirm(ep->hFwNat, hsRas);
#endif

    return 0;
}


/************************************************************************
 * cmEvRASReject
 * purpose: Callback function that the RAS calls when a reject on a
 *          RAS request is received
 * input  : haRas  - The application's RAS transaction handle
 *          hsRas  - Stack's handle for the RAS transaction
 *          reason - Rejection reason
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int RVCALLCONV cmEvRASReject(IN  HAPPRAS     haRas,
                             IN  HRAS        hsRas,
                             IN  cmRASReason reason)
{
    EpObj *ep;

    RV_UNUSED_ARG(haRas);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsRas), (HAPPAPP *)&ep);

    indicateIncomingMsg(ep, hsRas);

    epUtilsEvent(ep, "Script", NULL, "cb:cmras:EvReject %p %s", hsRas, RASReason2String(reason));

#ifdef USE_FWNAT
    RvH323FwNatEvRASReject(ep->hFwNat, hsRas);
#endif
    return 0;
}


/************************************************************************
 * cmEvRASTimeout
 * purpose: Callback function that the RAS calls when a timeout on a
 *          RAS request occured.
 *          This will be the case when no reply arrives or when an
 *          outgoing multicast transaction is being handled (even if
 *          replies arrived).
 * input  : haRas - The application's RAS transaction handle
 *          hsRas - Stack's handle for the RAS transaction
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int RVCALLCONV cmEvRASTimeout(IN  HAPPRAS          haRas,
                              IN  HRAS             hsRas)
{
    EpObj *ep;

    RV_UNUSED_ARG(haRas);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsRas), (HAPPAPP *)&ep);

#ifdef USE_FWNAT
    RvH323FwNatEvRASTimeout(ep->hFwNat, hsRas);
#endif

    epUtilsEvent(ep, "Script", NULL, "cb:cmras:EvTimeout %p", hsRas);

    return 0;
}


RvBool RVCALLCONV cmEvRASMessageSend(
    IN  HAPPRAS         haRas,
    IN  HRAS            hsRas,
    IN  cmRASTrStage    rasTrStage,
    IN  RvPvtNodeId     messageNodeId)
{
    EpObj *ep;
    EpCallObj *call;
    RvChar *messageStr;
    RvBool bStop = RV_FALSE, bInfo = RV_FALSE;
    RvStatus st;

    RV_UNUSED_ARG(haRas);
    RV_UNUSED_ARG(rasTrStage);
    RV_UNUSED_ARG(bInfo);
    RV_UNUSED_ARG(bStop);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsRas), (HAPPAPP *)&ep);

    /*Add H460.17/18 info and open encapsulation connection if we have to*/
    st = epFwNatEvRASMessageSend(ep, hsRas, rasTrStage, messageNodeId, &bStop, &bInfo);

    call = getCallObjFromRas(hsRas);
    messageStr = cmGetProtocolMessageName(ep->hApp, messageNodeId);

    epUtilsEvent(ep, "RAS", call, "out.%s %d", messageStr, messageNodeId);

    epUtilsLog(ep, call, "New message sent -> %s on RAS", messageStr);

    ep->cb.epMessage(ep, call, messageNodeId, messageStr, RV_TRUE);

    return bStop;
}


int RVCALLCONV cmEvAutoRASRequest(
    IN  HRAS             hsRas,
    IN  HCALL            hsCall,
    IN  cmRASTransaction transaction,
    IN  cmRASTransport*  srcAddress,
    IN  HAPPCALL         haCall)
{
    EpObj *ep;
    char buf[128];
    RvBool bIgnore = RV_FALSE, bInfo = RV_FALSE;

    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(bInfo);
    RV_UNUSED_ARG(bIgnore);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsRas), (HAPPAPP *)&ep);

    indicateIncomingMsg(ep, hsRas);

    cmTransportAddress2String(srcAddress, sizeof(buf), buf);

    epUtilsEvent(ep, "Script", (EpCallObj *)haCall, "cb:cmautoras:EvRequest %p %p %s {%s}",
        hsRas, haCall, RASTransaction2String(transaction), buf);

    /*handle SCI facility message if that's what it is*/
    if (transaction == cmRASServiceControl)
        bIgnore = epFwNatEvAutoRASRequest(hsRas);

    return bIgnore;
}


int RVCALLCONV cmEvAutoRASConfirm(
    IN  HRAS             hsRas)
{
    EpObj *ep;

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsRas), (HAPPAPP *)&ep);

    indicateIncomingMsg(ep, hsRas);

    epUtilsEvent(ep, "Script", NULL, "cb:cmautoras:EvConfirm %p", hsRas);

    return 0;
}


int RVCALLCONV cmEvAutoRASReject(
    IN  HRAS             hsRas,
    IN  cmRASReason      reason)
{
    EpObj *ep;

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsRas), (HAPPAPP *)&ep);

    indicateIncomingMsg(ep, hsRas);

    epUtilsEvent(ep, "Script", NULL, "cb:cmautoras:EvReject %p %s", hsRas, RASReason2String(reason));

    return 0;
}


int RVCALLCONV cmEvAutoRASTimeout(
    IN  HRAS             hsRas)
{
    EpObj *ep;

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsRas), (HAPPAPP *)&ep);

    epUtilsEvent(ep, "Script", NULL, "cb:cmautoras:EvTimeout %p", hsRas);

    return 0;
}




/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * epRasRegister
 * ----------------------------------------------------------------------------
 * General: Register to a gk.
 *          If aliases, gkAddress or gkIdentifier exists, they cause
 *          modifications in the configuration used.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         aliases          - List of aliases to register. Each alias in the
 *                            list is separated using ',' sign. Can be NULL.
 *         hasGkAddress     - RV_TRUE if we want to use gkAddress parameter.
 *                            If this is RV_TRUE and gkAddress==NULL, then no
 *                            address at all shall be used.
 *         gkAddressStr     - Gatekeeper address string. Can be one of the
 *                            following: IP:<ip> or SRV:IPv4:, SRV:IPv6:,
 *                            DNS:IPv4:, DNS:IPv6.
 *         hasGkIdentifier  - RV_TRUE if we want to use gkIdentifier parameter.
 *                            If this is RV_TRUE and gkIdentifier==NULL, then
 *                            no identifier shall be used.
 *         gkIdentifier     - Gatekeeper identifier to use. Can be NULL.
 * Output: None.
 *****************************************************************************/
RvStatus epRasRegister(
    IN EpObj                *ep,
    IN const RvChar         *aliases,
    IN RvBool               hasGkAddress,
    IN const RvChar         *gkAddressStr,
    IN RvBool               hasGkIdentifier,
    IN const RvChar         *gkIdentifier)
{
    RvStatus status;
    RvPvtNodeId cfgNodeId;
    RvPvtNodeId tmpNodeId;
    RvBool isIp, isEncap = RV_FALSE;

    isIp = ((gkAddressStr != NULL) && (strncmp(gkAddressStr, "IP:", 3) == 0));
#ifdef USE_FWNAT
    isEncap = (ep->cfg.fwNatConfig.e17Config == RvH323FwNatH46017ConfigEncapsulate);
#endif

    epUtilsEvent(ep, "Rec", NULL, "register%s%s%s%s%s%s%s%s%s%s",
        (aliases != NULL) ? " aliases= \"" : "",
        (aliases != NULL) ? aliases : "",
        (aliases != NULL) ? "\"" : "",
        hasGkAddress ? " address= \"" : "",
        isIp ? (isEncap? "IP:@DEST@TA:" : "IP:@DEST@RAS:") : "",
        isIp ? gkAddressStr+3 : (gkAddressStr != NULL ? gkAddressStr : ""),
        hasGkAddress ? "\"" : "",
        hasGkIdentifier ? " gk= \"" : "",
        (gkIdentifier != NULL) ? gkIdentifier : "",
        hasGkIdentifier ? "\"" : "");

    cfgNodeId = cmGetRASConfigurationHandle(ep->hApp);

    if (aliases != NULL)
    {
        /* delete aliases first - ask questions later. */
        __pvtGetNodeIdByFieldIds(tmpNodeId, ep->hVal, cfgNodeId, {_q931(registrationInfo) _q931(terminalAlias) LAST_TOKEN});
        pvtDelete(ep->hVal, tmpNodeId);

        if (*aliases != '\0')
        {
            const RvChar *p;
            const RvChar *pStart;
            RvChar aliasStr[600];
            cmAlias alias;

            __pvtBuildByFieldIds(tmpNodeId, ep->hVal, cfgNodeId, {_q931(registrationInfo) _q931(terminalAlias) LAST_TOKEN}, 0, NULL);

            p = aliases;
            while (*p)
            {
                pStart = p;
                while (*p && (*p != ','))
                    p++;
                if ((pStart != p) && ((RvSize_t)(p - pStart) < sizeof(aliasStr)))
                {
                    memcpy(aliasStr, pStart, (RvSize_t)(p - pStart));
                    aliasStr[p - pStart] = '\0';
                    status = String2Alias(ep, aliasStr, &alias);
                    if (status != RV_OK)
                        return status;
                    cmAlias2Vt(ep->hVal, &alias, pvtAdd(ep->hVal, tmpNodeId, -1, 0, NULL, NULL));
                    FreeAlias(ep, &alias);
                }
                if (*p)
                    p++;
            }
        }
    }

    if (hasGkAddress)
    {
        tmpNodeId = pvtGetChild(ep->hVal, cfgNodeId, __q931(manualDiscovery), NULL);
        pvtDelete(ep->hVal, tmpNodeId);

        if (strncmp(gkAddressStr, "IP:", 3) == 0)
        {
            cmTransportAddress addr;

            /* Set the gk's IP address if we have one */
            __pvtBuildByFieldIds(tmpNodeId, ep->hVal, cfgNodeId,
                {_q931(manualDiscovery) _q931(defaultGatekeeper) LAST_TOKEN}, 0, NULL);
            cmString2TransportAddress(gkAddressStr+3, &addr);
            cmTransportAddress2Vt(ep->hVal, &addr, tmpNodeId);
        }
        else
        {
            RvUint16 gkname_len = 0;
            RvChar* p = (RvChar*)(gkAddressStr+9);
            RvUint16 gkport = 0;

            while (*p && (*p != ':'))
            {
                gkname_len++;
                p++;
            }
            if (p != (RvChar*)(gkAddressStr+9))
            {
                if (*p == ':')
                {
                    *p = 0;
                    p++;
                }
                if (*p)
                {
                    gkport = (RvUint16)atoi(p);
                }
            }

            __pvtBuildByFieldIds(tmpNodeId, ep->hVal, cfgNodeId,
                {_q931(manualDiscovery) _q931(defaultGatekeeperName) LAST_TOKEN}, 0, NULL);
            pvtAdd(ep->hVal, tmpNodeId, __q931(gkName), strlen(gkAddressStr+9), gkAddressStr+9, NULL);
            pvtAdd(ep->hVal, tmpNodeId, __q931(gkPort), gkport, NULL, NULL);
            tmpNodeId = pvtAdd(ep->hVal, tmpNodeId, __q931(queryType), 0, NULL, NULL);
            if (strncmp(gkAddressStr, "DNS:IPv4:", 9) == 0)
                pvtAdd(ep->hVal, tmpNodeId, __q931(dnsTypeIpv4), 0, NULL, NULL);
            else if (strncmp(gkAddressStr, "DNS:IPv6:", 9) == 0)
                pvtAdd(ep->hVal, tmpNodeId, __q931(dnsTypeIpv6), 0, NULL, NULL);
            else if (strncmp(gkAddressStr, "SRV:IPv4:", 9) == 0)
                pvtAdd(ep->hVal, tmpNodeId, __q931(dnsTypeSrvIpv4), 0, NULL, NULL);
            else if (strncmp(gkAddressStr, "SRV:IPv6:", 9) == 0)
                pvtAdd(ep->hVal, tmpNodeId, __q931(dnsTypeSrvIpv6), 0, NULL, NULL);
        }
    }

    if (hasGkIdentifier)
    {
        __pvtGetNodeIdByFieldIds(tmpNodeId, ep->hVal, cfgNodeId, {_q931(registrationInfo) _q931(gatekeeperIdentifier) LAST_TOKEN});
        pvtDelete(ep->hVal, tmpNodeId);

        /* Set the gk identifier if we have one */
        if (gkIdentifier != NULL)
        {
            RvUint8 bmpIdentifier[256];
            int length;

            length = utlChr2Bmp(gkIdentifier, bmpIdentifier);
            __pvtBuildByFieldIds(tmpNodeId, ep->hVal, cfgNodeId,
                {_q931(registrationInfo) _q931(gatekeeperIdentifier) LAST_TOKEN}, length, (RvChar*)bmpIdentifier);
        }
    }

    if (epLdapIsConnected(ep, NULL))
    {
        status = epLdapRegister(ep);
    }
    else
    {
        status = cmRegister(ep->hApp);
    }

    if (status > 0)
        status = RV_OK;

    return status;
}


/******************************************************************************
 * epRasUnregister
 * ----------------------------------------------------------------------------
 * General: Unregister from a gk.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         isPartial        - true iff we want partial unregistration.
 *         aliases          - The aliases to unregister.
 * Output: None.
 *****************************************************************************/
RvStatus epRasUnregister(
    IN EpObj        *ep,
    IN RvBool       isPartial,
    IN const RvChar *aliases)
{
    RvStatus status;

    if (isPartial)
        epUtilsEvent(ep, "Rec", NULL, "unregister sec= 0 partial= %d aliases= \"%s\"", isPartial, aliases);
    else
        epUtilsEvent(ep, "Rec", NULL, "unregister sec= 0");

    if (isPartial)
    {
        const RvChar *p;
        const RvChar *pStart;
        RvChar aliasStr[600];
        cmAlias alias;

        if (RV_PVT_NODEID_IS_VALID(ep->partialUrqAliases))
        {
            pvtDeleteChilds(ep->hVal, ep->partialUrqAliases);
        }
        else
        {
            RvPvtNodeId tmpNodeId;
            __pvtGetNodeIdByFieldIds(tmpNodeId, ep->hVal, cmGetRASConfigurationHandle(ep->hApp),
                {_q931(registrationInfo) _q931(terminalAlias) LAST_TOKEN});
            ep->partialUrqAliases = pvtAddRoot(ep->hVal, NULL, 0, NULL);
            pvtSetTree(ep->hVal, ep->partialUrqAliases, ep->hVal, tmpNodeId);
            pvtDeleteChilds(ep->hVal, ep->partialUrqAliases);
        }

        p = aliases;
        while (*p)
        {
            pStart = p;
            while (*p && (*p != ','))
                p++;
            if ((pStart != p) && ((RvSize_t)(p - pStart) < sizeof(aliasStr)))
            {
                memcpy(aliasStr, pStart, (RvSize_t)(p - pStart));
                aliasStr[p - pStart] = '\0';
                status = String2Alias(ep, aliasStr, &alias);
                if (status != RV_OK)
                    return status;
                cmAlias2Vt(ep->hVal, &alias, pvtAdd(ep->hVal, ep->partialUrqAliases, __nul(0), 0, NULL, NULL));
                FreeAlias(ep, &alias);
            }
            if (*p)
                p++;
        }
    }

    status = cmUnregister(ep->hApp);

    if (status > 0)
        status = RV_OK;

    return status;
}


/******************************************************************************
 * epRasSendBRQ
 * ----------------------------------------------------------------------------
 * General: Change a call's rate by sending out a BRQ message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         call             - Call object to use.
 *         rate             - Bandwidth of call to set.
 * Output: None.
 *****************************************************************************/
RvStatus epRasSendBRQ(
    IN EpObj        *ep,
    IN EpCallObj    *call,
    IN RvUint32     rate)
{
    RvStatus status;

    epUtilsEvent(ep, "Rec", NULL, "sendBrq entry= %d rate= %ud", call->id, rate);

    status = cmCallSetRate(call->hsCall, rate);
    if (status > 0)
        status = RV_OK;

    return status;
}


/******************************************************************************
 * epRasSendNSM
 * ----------------------------------------------------------------------------
 * General: Send a non standard data message to the GK.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         nsd              - Non standard data to use.
 * Output: None.
 *****************************************************************************/
RvStatus epRasSendNSM(
    IN EpObj                *ep,
    IN cmNonStandardParam   *nsd)
{
    RvStatus status;

    status = cmSendNonStandardMessage(ep->hApp, nsd);
    if (status > 0)
        status = RV_OK;

    return status;
}


/******************************************************************************
 * epRasSendRAI
 * ----------------------------------------------------------------------------
 * General: Send a RAI message to the GK.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         aoor             - Almost out of resources indication.
 * Output: None.
 *****************************************************************************/
RvStatus epRasSendRAI(
    IN EpObj                *ep,
    IN RvBool               aoor)
{
    RvStatus status;

    epUtilsEvent(ep, "Rec", NULL, "sendRai aoor= %d", aoor);

    status = cmSendRAI(ep->hApp, aoor);
    if (status > 0)
        status = RV_OK;

    return status;
}




/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * getCallObjFromRas
 * ----------------------------------------------------------------------------
 * General: Get the call object associated with a given RAS transaction.
 *
 * Return Value: Call object, or NULL for global RAS transactions.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hRas     - RAS transaction.
 * Output: None.
 *****************************************************************************/
static EpCallObj *getCallObjFromRas(
    IN HRAS hRas)
{
    EpCallObj *call = NULL;
    HCALL hsCall = NULL;

    if (cmRASGetParam(hRas, cmRASTrStageRequest, cmRASParamCallHandle, 0, NULL, (char *)&hsCall) >= 0)
    {
        if (hsCall != NULL)
            cmCallGetHandle(hsCall, (HAPPCALL *)&call);
    }

    return call;
}


static void indicateIncomingMsg(
    IN EpObj        *ep,
    IN HRAS         hRas)
{
    EpCallObj *call;
    RvChar *messageStr;
    RvPvtNodeId messageNodeId;

    messageNodeId = pvtGetChild(ep->hVal, cmGetProperty((HPROTOCOL)hRas), __q931(response), NULL);
    if (!RV_PVT_NODEID_IS_VALID(messageNodeId))
        messageNodeId = pvtGetChild(ep->hVal, cmGetProperty((HPROTOCOL)hRas), __q931(request), NULL);

    call = getCallObjFromRas(hRas);
    messageStr = cmGetProtocolMessageName(ep->hApp, messageNodeId);

    epUtilsEvent(ep, "RAS", call, "in.%s %d", messageStr, messageNodeId);
    epUtilsLog(ep, call, "New message recv <- %s on RAS", messageStr);

    ep->cb.epMessage(ep, getCallObjFromRas(hRas), messageNodeId, messageStr, RV_FALSE);
}




#ifdef __cplusplus
}
#endif
