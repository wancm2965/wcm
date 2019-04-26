/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

#ifndef _CMRASINIT_H
#define _CMRASINIT_H

#include "cmintr.h"

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 * rasInit
 * purpose: Initialize the RAS module and all the network related with
 *          RAS.
 * input  : app     - Stack handle
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasInit(IN cmElem* app);


/************************************************************************
 * rasStart
 * purpose: Start the RAS module and all the network related with
 *          RAS.
 * input  : app     - Stack handle
 *          restart - Are we restarting the addresses only?
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasStart(IN cmElem* app, IN RvBool restart);

/************************************************************************
 * rasStop
 * purpose: Stop the RAS module (if not a restart) and all the network related with
 *          RAS.
 *          This makes the RAS ports to be removed and the endpoint to be
 *          unregistered.
 * input  : app     - Stack handle
 *          restart - if RV_TRUE the ras module is not stopped and ras addresses
 *			tree is not destroyed.
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasStop(IN cmElem* app, IN RvBool restart);

/************************************************************************
 * rasEnd
 * purpose: End the RAS module and all the network related with
 *          RAS.
 * input  : app     - Stack handle
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasEnd(IN cmElem* app);




#ifdef __cplusplus
}
#endif

#endif  /* _CMRASINIT_H */
