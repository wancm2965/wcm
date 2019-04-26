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

#ifndef _RV_EP_SECURITY_H_
#define _RV_EP_SECURITY_H_

/***********************************************************************
epSecurity.h

Support for H.235 security.
This module deals with all the security issues.
It is full of type definitions, allowing compilation with this module
and without it.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "lepDefs.h"




#ifdef __cplusplus
extern "C" {
#endif



/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/





/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/

#ifdef USE_SECURITY

/******************************************************************************
 * lgkSecurityInit
 * ----------------------------------------------------------------------------
 * General: Initialize the security module of an endpoint object.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epSecurityInit(
    IN EpObj        *ep);


/******************************************************************************
 * epSecurityEnd
 * ----------------------------------------------------------------------------
 * General: Destruct the security module of an endpoint object.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epSecurityEnd(
    IN EpObj        *ep);


/******************************************************************************
 * epSecurityVersion
 * ----------------------------------------------------------------------------
 * General: Get the version string of the H.235 module used.
 *
 * Return Value: Version string.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
const RvChar *epSecurityVersion(
    IN EpObj    *ep);


/******************************************************************************
 * epSecurityUpdateSettings
 * ----------------------------------------------------------------------------
 * General: This function should be called whenever the default security
 *          settings are modified.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epSecurityUpdateSettings(
    IN EpObj    *ep);


/******************************************************************************
 * epSecurityGetSenderId
 * ----------------------------------------------------------------------------
 * General: This Function gets the sender id set by the GK.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
void epSecurityGetSenderId(
    IN EpObj    *ep);


/******************************************************************************
 * epSecurityCallInit
 * ----------------------------------------------------------------------------
 * General: Initialize security services for a call. Namely the Diffie-Hellman
 *          procedures.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epSecurityCallInit(
    IN EpCallObj    *call);


/******************************************************************************
 * epSecurityCallEnd
 * ----------------------------------------------------------------------------
 * General: Finalize security services for a call. Namely the Diffie-Hellman
 *          procedures.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epSecurityCallEnd(
    IN EpCallObj    *call);


/******************************************************************************
 * epSecurityCallGenerateSharedSecret
 * ----------------------------------------------------------------------------
 * General: Generate a DH shared secret. This function should be called after
 *          the entities exchanged DH half-keys.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call object to use.
 * Output: None.
 *****************************************************************************/
void epSecurityCallGenerateSharedSecret(
    IN EpCallObj    *call);


/******************************************************************************
 * epSecurityChannelAddEncryptionSync
 * ----------------------------------------------------------------------------
 * General: Modify an outgoing OLC ack message to a secure channel.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call object to use.
 *         nodeId   - OLC acknowledge message to manipulate.
 * Output: None.
 *****************************************************************************/
void epSecurityChannelAddEncryptionSync(
    IN EpCallObj    *call,
    IN RvPvtNodeId  nodeId);


/******************************************************************************
 * epSecurityChannelGetEncryptionSync
 * ----------------------------------------------------------------------------
 * General: Open an incoming secure channel request.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call object to use.
 *         nodeId   - OLC acknowledge message to manipulate.
 * Output: None.
 *****************************************************************************/
void epSecurityChannelGetEncryptionSync(
    IN EpCallObj    *call,
    IN RvPvtNodeId  nodeId);




#else

#define epSecurityInit(_ep) RV_OK
#define epSecurityEnd(_ep) RV_OK
#define epSecurityUpdateSettings(_ep) RV_OK
#define epSecurityGetSenderId(_ep)
#define epSecurityCallInit(_call) RV_OK
#define epSecurityCallEnd(_call) RV_OK
#define epSecurityCallGenerateSharedSecret(_call)
#define epSecurityChannelAddEncryptionSync(_call, _nodeId)
#define epSecurityChannelGetEncryptionSync(_call, _nodeId)

#endif



#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_SECURITY_H_ */
