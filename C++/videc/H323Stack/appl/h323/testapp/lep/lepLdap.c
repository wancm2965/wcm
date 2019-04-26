/***********************************************************************************************
  Notice:
  This document contains information that is proprietary to RADVISION LTD..
  No part of this publication may be reproduced in any form whatsoever without
  written prior approval by RADVISION LTD..

    RADVISION LTD. reserves the right to revise this publication and make changes
    without obligation to notify any person of such revisions or changes.

*************************************************************************************************/

/********************************************************************************************
*                                  TAP_ldap.c
*
* This file contains all the functions which enable the use of the LDAP module
*
*
*      Written by                        Version & Date                        Change
*     ------------                       ---------------                      --------
*       Ran Arad                           27-May-2004
*
********************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

#include "lepDefs.h"
#include "lepLdap.h"

#ifdef USE_LDAP

#include "rvh323ldap.h"

#include "lepUtils.h"
#include "lepSecurity.h"
#include "cmH245GeneralDefs.h"
#include "stkutils.h"



/******************************************************************************
 * RvH323LdapResultEv_T
 * ----------------------------------------------------------------------------
 * General: This is the event type reporting an answer on an H.323 LDAP query.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : context           - The context given on creation of the transaction
 *         message           - The related message given
 *         result            - The type of result: success, error or timeout
 *         h235EpId          - The H.235 Endpoint ID received
 *         h235Password      - The H.235 Password received
 *         h235Certificate   - The H.235 Certificate received
 * Output: None.
 *****************************************************************************/
RvStatus RvH235LdapResultEv(
    IN RvH323LdapH235Context * context,
    IN RvH323LdapResultType result,
    IN RvUint8 * h235EpId,
    IN RvInt32 h235EpIdLen,
    IN RvUint8 * h235Password,
    IN RvInt32 h235PasswordLen,
    IN RvUint8 * h235Certificate,
    IN RvInt32 h235CertificateLen)
{
    EpObj * ep = (EpObj *)context->userContext;

    RV_UNUSED_ARG(h235EpId);
    RV_UNUSED_ARG(h235EpIdLen);
    RV_UNUSED_ARG(h235Password);
    RV_UNUSED_ARG(h235PasswordLen);
    RV_UNUSED_ARG(h235Certificate);
    RV_UNUSED_ARG(h235CertificateLen);

    if (result != RvH323LdapResultSuccess)
        return RV_OK;

    epUtilsEvent(ep, "SEC", NULL, "ldapResultEv");

#ifdef USE_SECURITY
    ep->cfg.senderId.length = utlChrn2Bmp((char*)h235EpId, h235EpIdLen, ep->cfg.senderId.data);
    strcpy(ep->cfg.password, (char*)h235Password);
#endif /* USE_SECURITY */
    epSecurityUpdateSettings(ep);

    return RV_OK;
}


/******************************************************************************
 * RvLdapStateChangedEc
 * ----------------------------------------------------------------------------
 * General: This is the event reporting a change in the LDAP state.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hLdapApp          - Application handle to the LDAP instance
 *         hLdap             - The LDAP instance handle
 *         eState            - The new LDAP state
 * Output: None.
 *****************************************************************************/
RvStatus RvLdapStateChangedEv(
    IN H323LDAPAPP hLdapApp,
    IN H323LDAP hLdap,
    IN RvH323LdapState eState)
{
    EpObj * ep = (EpObj *)hLdapApp;

    RV_UNUSED_ARG(hLdap);
    epUtilsEvent(ep, "SEC", NULL, "ldapStateChangedEv.%d", eState);

    switch(eState)
    {
    case RvH323LdapStateConnected:
        epUtilsLog(ep, NULL, "LDAP: connected to server");
        epUtilsEvent(ep, "ldapStateChanged", NULL, "normal");
        ep->genContext.userContext = ep;
        ep->genContext.txEvent = RvH235LdapResultEv;
        RvH323LdapGetLocalH235Parameters(ep->hApp, ep->hLdap, ep->strDn, &ep->genContext);
        break;

    case RvH323LdapStateDisconnected:
        epUtilsLog(ep, NULL, "LDAP: disconnected from server");
        epUtilsEvent(ep, "ldapStateChanged", NULL, "disabled");
        break;

    case RvH323LdapStateError:
        epUtilsLog(ep, NULL, "LDAP: server error");
        epUtilsEvent(ep, "ldapStateChanged", NULL, "disabled");
        break;

    default: break;
    }
    return RV_OK;
}


/******************************************************************************
 * RvLdapTxAnsweredEv
 * ----------------------------------------------------------------------------
 * General: This is the event reporting an answer on an LDAP query. It
 * will be called after all answers arrive, and will provide the number or
 * answers received. The answers are released on exit form this callback.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hAppRequest       - The application handle to the request
 *         hRequest          - The LDAP request handle
 *         arrHMesssage      - The array of messages received
 *         numResults        - The number of results received
 *         bError            - RV_TRUE if an error occured
 * Output: None.
 *****************************************************************************/
RvStatus RvLdapTxAnsweredEv(
    IN H323LDAPAppRequest hAppRequest,
    IN H323LDAPRequest hRequest,
    IN H323LDAPMessage * arrHMesssage,
    IN RvInt32 numResults,
    IN RvH323LdapErrorType errorCode)
{
    EpObj * ep = (EpObj *)hAppRequest;

    RV_UNUSED_ARG(arrHMesssage);

    epUtilsEvent(ep, "SEC", NULL, "ldapTxAnsweredEv.%d.%d", errorCode, numResults);
    epUtilsLog(ep, NULL, "LDAP: transaction answered, err=%d, num=%d", errorCode, numResults);

    RvH323LdapEndTx(hRequest);
    return RV_OK;
}


/******************************************************************************
 * RvLdapTxAbandonedEv
 * ----------------------------------------------------------------------------
 * General: This is the event reporting timer expiry. The application is
 * expected to free the request at this time at the latest.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hAppRequest       - The application handle to the request
 *         hRequest          - The LDAP request handle
 * Output: None.
 *****************************************************************************/
RvStatus RvLdapTxAbandonedEv(
    IN H323LDAPAppRequest hAppRequest,
    IN H323LDAPRequest hRequest)
{
    EpObj * ep = (EpObj *)hAppRequest;

    epUtilsEvent(ep, "SEC", NULL, "ldapTxAbandonedEv");
    epUtilsLog(ep, NULL, "LDAP: transaction abandoned");

    RvH323LdapEndTx(hRequest);
    return RV_OK;
}


/******************************************************************************
 * epLdapInit
 * ----------------------------------------------------------------------------
 * General: Initialize the LDAP module.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep                   - Endpoint object to use.
 *         strLdapSeverAddress  - Host-name of the server to connect to
 *         strUsername          - User name to use
 *         strPassword          - Password to use
 * Output: None.
 *****************************************************************************/
RvStatus epLdapInit(
    IN  EpObj           *ep,
    IN  const RvChar    *strLdapSeverAddress,
    IN  const RvChar    *strUsername,
    IN  const RvChar    *strPassword,
    IN  const RvChar    *strGroup)
{
    H323LDAPConfiguration configuration;
    H323LDAPEvents events;

    strcpy(ep->strDn, strUsername);
    strcpy(ep->strSb, strGroup);

    memset(&configuration, 0, sizeof(configuration));
    configuration.strLdapSeverAddress = (RvChar *)strLdapSeverAddress;//"172.29.53.8";
    configuration.strUsername = (RvChar *)strUsername;//"uid=rvUser,ou=RV,dc=radvision,dc=com";
    configuration.strPassword = (RvChar *)strPassword;//"rvUser";
    configuration.eConnectionType = RvH323LdapConnectionMethodSimple;
    configuration.maxStringSize = 256;
    configuration.maxTransactions = 4;
    configuration.transactionTimeout = 4;

    memset(&events, 0, sizeof(events));
    events.RvH323LdapStateChangedEv = RvLdapStateChangedEv;
    events.RvH323LdapTxAbandonedEv  = RvLdapTxAbandonedEv;
    events.RvH323LdapTxAnsweredEv   = RvLdapTxAnsweredEv;

    return RvH323LdapInit((H323LDAPAPP)ep, 
        &configuration, sizeof(configuration),
        &events, sizeof(events), &ep->hLdap);
}


/******************************************************************************
 * epLdapEnd
 * ----------------------------------------------------------------------------
 * General: End the LDAP module.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epLdapEnd(
    IN  EpObj * ep)
{
    if (ep->hLdap != NULL)
    {
        RvH323LdapEnd(ep->hLdap);
        ep->hLdap = NULL;
        epUtilsLog(ep, NULL, "LDAP: terminated");
        epUtilsEvent(ep, "ldapStateChanged", NULL, "disabled");
    }
    return RV_OK;
}


/******************************************************************************
 * epLdapVersion
 * ----------------------------------------------------------------------------
 * General: Get the version string of the LDAP module used.
 *
 * Return Value: Version string.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object.
 * Output: None.
 *****************************************************************************/
const RvChar *epLdapVersion(
    IN EpObj    *ep)
{
    RV_UNUSED_ARG(ep);
    return "OpenLDAP v2.1.25";
}


/******************************************************************************
 * epLdapRegister
 * ----------------------------------------------------------------------------
 * General: Try to register using the LDAP module.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object.
 * Output: None.
 *****************************************************************************/
RvStatus epLdapRegister(
    IN EpObj    *ep)
{
    return RvH323LdapRegister(ep->hApp, ep->hLdap, ep->strDn);
}


#endif /* USE_LDAP */


/******************************************************************************
 * epLdapIsConnected
 * ----------------------------------------------------------------------------
 * General: Get the LDAP connection status.
 *
 * Return Value: RV_TRUE if LDAP is connected, RV_FALSE o.w.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object.
 * Output: None.
 *****************************************************************************/
RvBool epLdapIsConnected(
    IN EpObj    *ep,
    IN char     **curSenderDn)
{
#ifdef USE_LDAP
    if ((ep->hLdap != NULL) && ep->cfg.bUseLdap)
    {
        RvH323LdapState eState = RvH323LdapStateInitiaized;

        if (curSenderDn != NULL) *curSenderDn = ep->lastCertificateDn;

        RvH323LdapGetParam(ep->hLdap, RvH323LdapParmState, (RvInt32*)&eState, NULL);
        return (eState == RvH323LdapStateConnected);
    }
#else
    RV_UNUSED_ARG(ep);
    RV_UNUSED_ARG(curSenderDn);
#endif
    return RV_FALSE;
}

#ifdef USE_LDAP

RvStatus RvH235LdapGotPublicCertificateEv(
    IN RvH323LdapH235Context * context,
    IN RvH323LdapResultType result,
    IN RvUint8 * h235EpId,
    IN RvInt32 h235EpIdLen,
    IN RvUint8 * h235Password,
    IN RvInt32 h235PasswordLen,
    IN RvUint8 * h235Certificate,
    IN RvInt32 h235CertificateLen)
{
    frozenMessage * pMsg = (frozenMessage *)context->userContext;
    EpObj * ep = pMsg->ep;

    RV_UNUSED_ARG(h235Password);
    RV_UNUSED_ARG(h235PasswordLen);
    epUtilsEvent(ep, "SEC", NULL, "ldapGotPublicCertificateEv.%d", result);

    if (result == RvH323LdapResultSuccess)
    {
        memcpy(ep->lastCertificate, h235Certificate, h235CertificateLen);
        memcpy(ep->lastCertificateDn, h235EpId, h235EpIdLen);

        if (pMsg->srcAddress.port != 0)
        {
            /* this is a RAS message */
            cmProtocolRecvRawMessageFrom(ep->hApp, pMsg->protConn, pMsg->message, pMsg->msgLen, &pMsg->srcAddress);
        }
        else
        {
            /* this is a Q.931 message */
            cmProtocolRecvRawMessage(ep->hApp, pMsg->protConn, pMsg->message, pMsg->msgLen);
        }
    }

    ep->cb.epFree(ep, context);
    ep->cb.epFree(ep, pMsg);
    return RV_OK;
}

#endif

/******************************************************************************
 * epLdapSendH235Quarry
 * ----------------------------------------------------------------------------
 * General: Get the LDAP connection status.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object.
 *         senderDn         - Distinguished name of the sender
 *         message          - ALLOCATED frozen Message struct
 * Output: None.
 *****************************************************************************/
RvStatus epLdapSendH235Quarry(
    IN EpObj         *ep,
    IN char          *senderDn,
    IN frozenMessage *message)
{
#ifdef USE_LDAP
    if ((ep->hLdap != NULL) && ep->cfg.bUseLdap)
    {
        RvH323LdapState eState = RvH323LdapStateInitiaized;
        RvH323LdapH235Context *ctx;

        RvH323LdapGetParam(ep->hLdap, RvH323LdapParmState, (RvInt32*)&eState, NULL);
        if (eState != RvH323LdapStateConnected)
            return RV_ERROR_UNINITIALIZED;

        ctx = ep->cb.epMalloc(ep, sizeof(RvH323LdapH235Context), __FILE__, __LINE__);
        ctx->userContext = message;
        ctx->txEvent = RvH235LdapGotPublicCertificateEv;
        if (RvH323LdapGetRemoteH235Parameters(ep->hApp, ep->hLdap, ep->strSb, senderDn, ctx) != RV_OK)
        {
            ep->cb.epFree(ep, ctx);
            return RV_ERROR_UNKNOWN;
        }
        return RV_OK;
    }
#else
    RV_UNUSED_ARG(ep);
    RV_UNUSED_ARG(senderDn);
    RV_UNUSED_ARG(message);
#endif
    return RV_ERROR_UNINITIALIZED;
}

#ifdef USE_LDAP


#endif


#if defined(__cplusplus)
}
#endif

