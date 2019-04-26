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

#ifndef _TRANSPARENT_H
#define _TRANSPARENT_H

/*************************************************************************************
 * Transport module
 * ----------------
 *
 * This is the module that interacts with the network on one hand, while on the other
 * it communicates with the other protocol modules, such as Q.931, H.245, H.450 etc.
 *
 * The lower level can handle TPKT, Annex E types of communications in a transparent
 * way to th eupper layers.
 *
 * The upper level exports and imports (by means of APIs and callbacks) messages to the
 * different modules: Mainly Q.931, H.245 (including tunneled messages) and the
 * rest of the tunneled protocols (Annex M, Annex L).
 *
 **************************************************************************************/

#include "transportint.h"
#include "transport.h"

#ifdef __cplusplus
extern "C" {
#endif



/************************************************************************
 * transParentInit
 * purpose: Initialize the parent lock system
 * input  : transGlobals   - handle to the transport globals
 *          numOfLocks     - Ammout of locks to create
 * output : none
 * return : RV_OK on success, negative o.w.
 ************************************************************************/
RvStatus transParentInit(
    IN cmTransGlobals*  transGlobals,
    IN int              numOfSessions);


/************************************************************************
 * transParentEnd
 * purpose: End the parent lock system
 * input  : transGlobals   - handle to the transport globals
 * output : none
 * return : none
 ************************************************************************/
void transParentEnd(
    IN cmTransGlobals*  transGlobals);


/************************************************************************
 * transParentChildSafetyLock
 * purpose: Lock the parent lock another time to be extra safe, also
 *          outputs a pointer to the parent.
 * input  : hTransport     - handle to the transport globals
 *          emaElem        - The EMA whose parent we want safe
 * output : none
 * return : pointer to the parent element.
 ************************************************************************/
void* transParentChildSafetyLock(
    IN HAPPTRANS    hTransport,
    IN void*        emaElem);


/************************************************************************
 * transParentChildSafetyUnlock
 * purpose: Unlock a parent lock another, maybe free it.
 * input  : hTransport     - handle to the transport globals
 *          pParentLock    - parent lock to unlock
 * output : none
 * return : none
 ************************************************************************/
void transParentChildSafetyUnlock(
    IN HAPPTRANS    hTransport,
    IN void*        pParentLock);


/************************************************************************
 * emaEvSessionElementIdle
 * purpose: Event function called when a session element is idle
 * input  : elem      - the idle element
 *          bDeleting - if true, we are now deleting the element,
 *                      otherwise, it is just idle
 * output : none
 * return : none
 ************************************************************************/
void RVCALLCONV emaEvSessionElementIdle(EMAElement * elem, RvBool bDeleting);


/************************************************************************
 * emaEvHostElementIdle
 * purpose: Event function called when a session element is idle
 * input  : elem      - the idle element
 *          bDeleting - if true, we are now deleting the element,
 *                      otherwise, it is just idle
 * output : none
 * return : none
 ************************************************************************/
void RVCALLCONV emaEvHostElementIdle(EMAElement * elem, RvBool bDeleting);


/************************************************************************
 * emaEvTpktElementIdle
 * purpose: Event function called when a session element is idle
 * input  : elem      - the idle element
 *          bDeleting - if true, we are now deleting the element,
 *                      otherwise, it is just idle
 * output : none
 * return : none
 ************************************************************************/
void RVCALLCONV emaEvTpktElementIdle(EMAElement * elem, RvBool bDeleting);


/************************************************************************
 * transParentChange
 * purpose: Change the lock on the session, the call, and the H.245
 *          connection, if it exists, to the new lock (the host was
 *          already locked before, or we wouldn't be here)
 * input  : transGlobals - Transport globals
 *          host      - The host whose parent lock we want to use
 *          session   - The session whose parent lock we want to change
 * output : none
 * return : none
 ************************************************************************/
void RVCALLCONV transParentChange(cmTransGlobals *transGlobals, cmTransHost *host, cmTransSession *session);
    
#ifdef __cplusplus
}
#endif

#endif  /* _TRANSPARENT_H */

