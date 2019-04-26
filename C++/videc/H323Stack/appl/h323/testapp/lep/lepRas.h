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

#ifndef _RV_EP_RAS_H_
#define _RV_EP_RAS_H_

/***********************************************************************
epRas.h

RAS module of the endpoint.
Handles the registrations and all other RAS transactions.
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
/*                           CALLBACK HEADERS                            */
/*-----------------------------------------------------------------------*/
int RVCALLCONV cmEvRegEvent(
    IN HAPP               hApp,
    IN cmRegState         state,
    IN cmRegEvent         regEvent,
    IN int                regEventHandle);
int RVCALLCONV cmEvRASRequest(
    IN  HRAS             hsRas,
    IN  HCALL            hsCall,
    OUT LPHAPPRAS        lphaRas,
    IN  cmRASTransaction transaction,
    IN  cmRASTransport*  srcAddress,
    IN  HAPPCALL         haCall);
int RVCALLCONV cmEvRASConfirm(
    IN HAPPRAS  haRas,
    IN HRAS     hsRas);
int RVCALLCONV cmEvRASReject(
    IN  HAPPRAS     haRas,
    IN  HRAS        hsRas,
    IN  cmRASReason reason);
int RVCALLCONV cmEvRASTimeout(
    IN  HAPPRAS          haRas,
    IN  HRAS             hsRas);
RvBool RVCALLCONV cmEvRASMessageSend(
    IN  HAPPRAS         haRas,
    IN  HRAS            hsRas,
    IN  cmRASTrStage    rasTrStage,
    IN  RvPvtNodeId     messageNodeId);
int RVCALLCONV cmEvAutoRASRequest(
    IN  HRAS             hsRas,
    IN  HCALL            hsCall,
    IN  cmRASTransaction transaction,
    IN  cmRASTransport*  srcAddress,
    IN  HAPPCALL         haCall);
int RVCALLCONV cmEvAutoRASConfirm(
    IN  HRAS             hsRas);
int RVCALLCONV cmEvAutoRASReject(
    IN  HRAS             hsRas,
    IN  cmRASReason      reason);
int RVCALLCONV cmEvAutoRASTimeout(
    IN  HRAS             hsRas);





/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
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
    IN const RvChar         *gkIdentifier);


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
    IN const RvChar *aliases);


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
    IN RvUint32     rate);


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
    IN cmNonStandardParam   *nsd);


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
    IN RvBool               aoor);







#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_RAS_H_ */
