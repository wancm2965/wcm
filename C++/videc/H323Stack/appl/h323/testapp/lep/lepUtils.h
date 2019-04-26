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

#ifndef _RV_EP_UTILS_H_
#define _RV_EP_UTILS_H_

/***********************************************************************
epUtils.h

Utility functions widely used by this module.
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
 * epUtilsEvent
 * ----------------------------------------------------------------------------
 * General: Send indication of an event to the application that uses this
 *          module.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         eventType        - Type of the event indicated.
 *         call             - Call this event belongs to (NULL if none).
 *         eventStr         - Event string, in printf() formatting.
 * Output: None.
 *****************************************************************************/
void epUtilsEvent(
    IN EpObj        *ep,
    IN const RvChar *eventType,
    IN EpCallObj    *call,
    IN const RvChar *eventStr, ...);


/******************************************************************************
 * epUtilsLog
 * ----------------------------------------------------------------------------
 * General: Indicate the application that it can log a message for its user.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         call             - Call this log message belongs to (NULL if none).
 *         logStr           - Log string, in printf() formatting.
 * Output: None.
 *****************************************************************************/
void epUtilsLog(
    IN EpObj        *ep,
    IN EpCallObj    *call,
    IN const RvChar *logStr, ...);


/******************************************************************************
 * epUtilsError
 * ----------------------------------------------------------------------------
 * General: Indicate the application that it can log an error message for its
 *          user.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         call             - Call this log message belongs to (NULL if none).
 *         logStr           - Log string, in printf() formatting.
 * Output: None.
 *****************************************************************************/
void epUtilsError(
    IN EpObj        *ep,
    IN EpCallObj    *call,
    IN const RvChar *logStr, ...);


/******************************************************************************
 * epUtilsGetError
 * ----------------------------------------------------------------------------
 * General: Get and reset the last known error that occurred in the endpoint.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: errStr           - Last known error string. This string is reset
 *                            by the call to this function.
 *****************************************************************************/
void epUtilsGetError(
    IN  EpObj           *ep,
    OUT const RvChar    **errStr);


/******************************************************************************
 * epScanPvtNode
 * ----------------------------------------------------------------------------
 * General: Run a scan on the given PVT node.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         node             - PVT node to scan.
 *****************************************************************************/
void epScanPvtNode(
    IN  EpObj           *ep,
    IN  RvPvtNodeId     node);

/******************************************************************************
 * epTaHasIp
 * ----------------------------------------------------------------------------
 * General: Checks the validity of a transport address.
 *
 * Return Value: RV_TRUE if transport address is valid.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         ta               - The transport address to validate.
 *****************************************************************************/
RvBool epTaHasIp(
     IN EpObj *             ep,
     IN cmTransportAddress* ta);



#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_UTILS_H_ */
