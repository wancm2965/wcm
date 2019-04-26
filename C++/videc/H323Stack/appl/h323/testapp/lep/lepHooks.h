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

#ifndef _RV_EP_HOOKS_H_
#define _RV_EP_HOOKS_H_

/***********************************************************************
epHooks.h

Handle stack hooks.
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
int RVCALLCONV hookInConn(
    IN      HPROTCONN           hConn,
    IN      int                 addrFrom,
    IN      int                 addrTo);
int RVCALLCONV hookOutConn(
    IN      HPROTCONN           hConn,
    IN      int                 addrFrom,
    IN      int                 addrTo,
    IN      RvBool              error);
void RVCALLCONV hookClose(
    IN      HPROTCONN           hConn);
RvBool RVCALLCONV hookSend(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      RvBool              error);
RvBool RVCALLCONV hookRecv(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      RvBool              error);
RvBool RVCALLCONV hookSendTo(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      int                 nodeIdTo,
    IN      RvBool              error);
RvBool RVCALLCONV hookRecvFrom(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      int                 addrFrom,
    IN      RvBool              multicast,
    IN      RvBool              error);
int RVCALLCONV hookConnecting(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId);
int RVCALLCONV hookListening(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      RvBool              error);
RvBool RVCALLCONV hookSendRaw(
    IN      HPROTCONN           hConn,
    IN      const RvUint8 *     rawMessage,
    IN      RvInt32             messageLen);
RvBool RVCALLCONV hookSendRawTo(
    IN      HPROTCONN           hConn,
    IN      const RvUint8 *     rawMessage,
    IN      RvInt32             messageLen,
    IN      cmTransportAddress* addrTo);





/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/








#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_HOOKS_H_ */
