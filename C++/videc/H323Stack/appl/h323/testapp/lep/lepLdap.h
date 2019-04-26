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

#ifndef _RV_EP_LDAP_H_
#define _RV_EP_LDAP_H_

/***********************************************************************
epLdap.h

LDAP add-on module of the endpoint.
Handles the supplementary services.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/




#ifdef __cplusplus
extern "C" {
#endif



/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/


// TODO: Ran, please add some comments on the struct and its parameters.
typedef struct
{
    EpObj           *ep;
    HPROTCONN       protConn;
    cmRASTransport  srcAddress;
    EpCallObj       *call;
    int             msgLen;
    RvUint8         *message;
} frozenMessage;





/*-----------------------------------------------------------------------*/
/*                           CALLBACK HEADERS                            */
/*-----------------------------------------------------------------------*/





/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/

#ifdef USE_LDAP

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
    IN  const RvChar    *strGroup);


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
RvStatus epLdapEnd(IN  EpObj * ep);


/******************************************************************************
 * epLdapVersion
 * ----------------------------------------------------------------------------
 * General: Get the version string of the LDAP module used.
 *
 * Return Value: Version string.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
const RvChar *epLdapVersion(IN  EpObj * ep);


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
    IN EpObj    *ep);


#else


#define epLdapInit(_ep,_strLdapSeverAddress,_strUsername,_strPassword,_strGroup) RV_ERROR_NOTSUPPORTED
#define epLdapEnd(_ep) RV_OK
#define epLdapVersion(_ep) "0"
#define epLdapRegister(_ep) RV_ERROR_NOTSUPPORTED


#endif


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
    IN char **curSenderDn);


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
    IN EpObj    *ep,
    IN char *senderDn,
    IN frozenMessage * message);



#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_LDAP_H_ */
