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

#ifndef _RV_EP_H_
#define _RV_EP_H_

/***********************************************************************
ep.h

Sample endpoint module.
This module is used as a sample code for an H.323 endpoint.
It is separate from the GUI to make the life of the programmer easier
when trying to look how to implement code parts.
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


/******************************************************************************
 * epInit
 * ----------------------------------------------------------------------------
 * General: Initialize the endpoint object by a configuration file.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         cb               - Implemented callbacks.
 *         configFilename   - Name of configuration file to use.
 *         extraCallSize    - Extra data to save on each call object.
 *         useSecurity      - RV_TRUE to enable security.
 *         useH450          - RV_TRUE to enable H.450.
 *         useRTP           - RV_TRUE to enable RTP.
 *         useFwNat      - RV_TRUE to enable H.460/17/18/19.
 * Output: None.
 *****************************************************************************/
RvStatus epInit(
    IN EpObj        *ep,
    IN EpCallbacks  *cb,
    IN const RvChar *configFilename,
    IN RvSize_t     extraCallSize,
    IN RvBool       useSecurity,
    IN RvBool       useH450,
    IN RvBool       useRTP,
    IN RvBool       useFwNat);


/******************************************************************************
 * epEnd
 * ----------------------------------------------------------------------------
 * General: Destruct the endpoint object.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epEnd(
    IN EpObj        *ep);


/******************************************************************************
 * epStart
 * ----------------------------------------------------------------------------
 * General: Start running the endpoint.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epStart(
    IN EpObj        *ep);


/******************************************************************************
 * epStop
 * ----------------------------------------------------------------------------
 * General: Stop running the endpoint.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epStop(
    IN EpObj        *ep);


/******************************************************************************
 * epRebind
 * ----------------------------------------------------------------------------
 * General: Rebind all listening addresses.
 *          Can be used to update the NAT address linked to internal listening
 *          addresses.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epRebind(
    IN EpObj        *ep);


/******************************************************************************
 * epReset
 * ----------------------------------------------------------------------------
 * General: Reset recording related information in the endpoint.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epReset(
    IN EpObj        *ep);


/******************************************************************************
 * epGetConfig
 * ----------------------------------------------------------------------------
 * General: Get configuration parameters pointer.
 *          These parameters can be modified by the caller for future
 *          behavior.
 *
 * Return Value: Pointer to configuration parameters.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
EpConfig *epGetConfig(
    IN EpObj        *ep);







#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_H_ */
