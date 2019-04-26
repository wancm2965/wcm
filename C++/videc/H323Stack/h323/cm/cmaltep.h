/***********************************************************************
        Copyright (c) 2006 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

#ifndef _CMALTEP_H
#define _CMALTEP_H


#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 * cmCallGetAltEpCallSignalAddress
 * purpose: get AltEP callSignalAddress from CALL's altEpProperty
 * input  : hsCall     - Stack call handle
 *        : hVal       - PVT handle
 * output : tpktAddress - the call Signal address.
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus cmCallGetAltEpCallSignalAddress(
    IN  callElem* call,
    OUT cmTransportAddress *tpktAddress);


int cmCallSetAlternateEndpoints(
    IN HCALL        hsCall,
    IN RvPvtNodeId  altEpsNode);

int cmCallSetAltEpDestination(
    IN  HCALL       hsCall);

#ifdef __cplusplus
}
#endif

#endif  /* _CMALTEP_H */
