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

#include "rvinternal.h"
#include "cm.h"
#include "cmCall.h"
#include "cmintr.h"
#include "cmdebprn.h"

#ifdef __cplusplus
extern "C" {
#endif


RVAPI
HPROTCONN RVCALLCONV cmGetTpktChanHandle(
    IN HCALL                hsCall,
    IN cmTpktChanHandleType tpktChanHandleType)
{
    HPROTCONN hProtCon=NULL;
    callElem*call=(callElem*)hsCall;
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    if (!hsCall) return NULL;
    if (!app) return NULL;

    cmiAPIEnter(((HAPP)app, "cmGetTpktChanHandle: hsCall=%p tpktChanHandleType = %d",hsCall,tpktChanHandleType));
    if (emaLock((EMAElement)hsCall))
    {
        TRANSSESSIONPARAM sessionParam = cmTransParam_host; /* default to cmQ931TpktChannel */

        if (tpktChanHandleType == cmH245TpktChannel)
        {
            /* not cmQ931TpktChannel */
            sessionParam = cmTransParam_H245Connection;
        }
        if (cmTransGetSessionParam(call->hsTransSession, sessionParam, (void*)&hProtCon) != cmTransOK)
            hProtCon = NULL;
        emaUnlock((EMAElement)hsCall);
    }
    cmiAPIExit(((HAPP)app, "cmGetTpktChanHandle=%p", hProtCon));

    return hProtCon;
}


RVAPI
int RVCALLCONV cmSetTpktChanApplHandle(
    IN HPROTCONN    hConn,
    IN HAPPCONN     hAppConn)
{
    if (hConn != NULL)
    {
        if (cmTransSetHostApplHandle((HSTRANSHOST)hConn, (HATRANSHOST)hAppConn) == cmTransOK)
            return RV_TRUE;
    }
    return RV_ERROR_UNKNOWN;
}


RVAPI
int RVCALLCONV cmGetTpktChanApplHandle(
    IN  HPROTCONN   hConn,
    OUT HAPPCONN*   hAppConn)
{
    if (hConn && hAppConn)
    {
        if (cmTransGetHostApplHandle((HSTRANSHOST)hConn, (HATRANSHOST*)hAppConn)==cmTransOK)
            return RV_TRUE;
    }
    return RV_ERROR_UNKNOWN;
}


RVAPI
HCALL RVCALLCONV cmGetCallByH245Chan(
    IN HPROTCONN            hConn)
{
    if (hConn != NULL)
    {
        HCALL hsCall;
        if (cmTransGetHostParam((HSTRANSHOST)hConn, cmTransHostParam_firstCall, (void*)&hsCall) == cmTransOK)
            return hsCall;
    }
    return NULL;
}





#ifdef __cplusplus
}
#endif
