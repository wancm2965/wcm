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
#include "cmutils.h"
#include "cmictrl.h"
#include "transport.h"
#include "cmdebprn.h"
#include "cmCall.h"
#include "cmutils.h"


#ifdef __cplusplus
extern "C" {
#endif


void getGoodAddressForCall(HCALL hCall, cmTransportAddress* ta)
{
    callElem* call=(callElem*)hCall;

    /* if we already have a full address, we assume it's good, and just return */
    if ( ((ta->type == cmTransportTypeIP) || (ta->type == cmTransportTypeIPv6)) && cmTaHasIp(ta) && (ta->port != 0) )
        return;

    if (call != NULL)
    {
        RvUint16 port = ta->port;
        cmTransGetGoodAddressForH245(call->hsTransSession,ta);

        /* If we had a port in the original address, we should just leave it as it was */
        if (port != 0)
            ta->port = port;
    }
}

void getGoodAddressForCtrl(HCONTROL ctrl, cmTransportAddress* ta)
{
    getGoodAddressForCall(cmiGetByControl(ctrl), ta);
}

RVAPI int RVCALLCONV
cmCallCreateControlSession(
          IN     HCALL           hsCall,
          IN OUT cmTransportAddress* addr)
{
    int ret=0;
    callElem* call=(callElem*)hsCall;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp,"cmCallCreateControlSession hsCall=%p",hsCall));
    if (emaLock((EMAElement)call))
    {
        if (addr)
            ret = (int)cmTransCreateControlSession(call->hsTransSession, addr,
                (!cmTaHasIp(addr) || (addr->port == 0)), RV_FALSE);
        else
        {
            cmTransportAddress dummyAddr;
            ret = (int)cmTransCreateControlSession(call->hsTransSession, &dummyAddr, RV_FALSE, RV_TRUE);
        }
        emaUnlock((EMAElement)call);
    }
    cmiAPIExit((hApp,"cmCallCreateControlSession=%d", ret));
    return ret;
}


RVAPI int RVCALLCONV
cmCallCloseControlSession(
              /* Close the H.245 session for the call */
              IN     HCALL               hsCall)
{
    callElem*call=(callElem*)hsCall;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp,"cmCallCloseControlSession: hsCall=%p",hsCall));
    if (emaLock((EMAElement)call))
    {
        if (cmTransHasControlSession(call->hsTransSession))
        {
            stopControl(cmiGetControl(hsCall));
            closeChannels(cmiGetControl(hsCall));
        }
        cmTransCloseControlSession(call->hsTransSession);
        emaUnlock((EMAElement)call);
    }
    cmiAPIExit((hApp,"cmCallCloseControlSession=1"));
    return RV_TRUE;
}


RVAPI int RVCALLCONV
cmCallHasControlSession(
              /* Does call has the H.245 protocol? */
              IN     HCALL               hsCall)
{
    callElem*call=(callElem*)hsCall;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    int has=0;
    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp,"cmCallHasControlSession: hsCall=%p",hsCall));
    if (emaLock((EMAElement)call))
    {
        has=cmTransHasControlSession(call->hsTransSession);
        emaUnlock((EMAElement)call);
    }
    cmiAPIExit((hApp,"cmCallHasControlSession(hsCall=%p)=%d", hsCall, has));
    return has;
}


RVAPI
int RVCALLCONV cmCallConnectControl(
                IN      HCALL               hsCall
                                  )
{
    callElem* call=(callElem*)hsCall;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    int res = RV_ERROR_UNKNOWN;

    if ((hsCall == NULL) || (hApp == NULL))
        return RV_ERROR_NULLPTR;

    cmiAPIEnter((hApp, "cmCallConnectControl(hsCall=%p)",hsCall));
    if (emaLock((EMAElement)call))
    {
        TRANSERR transRes;
        if ((call->state == cmCallStateInit) && (cmiGetIsControl(hsCall)))
        {
            /* dummy call already has control - fix for MC: tunneling rejected, reset the control */
            RvInt32 terminalType;
            if (m_callget(call, setTerminalType))
                terminalType = call->msdTerminalType;
            else
                terminalType = -1;
            /* Make sure we clean up any used resources */
            endControl(cmiGetControl(hsCall));
            freeControl(cmiGetControl(hsCall));
            initControl(cmiGetControl(hsCall), RvH245ChannelParamsH225, terminalType);
        }
        transRes = cmTransEstablishControl(call->hsTransSession);
        if (transRes == cmTransOK)
            res = RV_OK;
        emaUnlock((EMAElement)call);
    }
    cmiAPIExit((hApp, "cmCallConnectControl=%d", res));
    return res;
}

RVAPI int RVCALLCONV
cmCallSw2SeparateH245(IN      HCALL               hsCall)
{
    callElem*call=(callElem*)hsCall;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    int res = RV_ERROR_UNKNOWN;

    if ((hsCall == NULL) || (hApp == NULL))
        return RV_ERROR_NULLPTR;

    cmiAPIEnter((hApp, "cmCallSw2SeparateH245(hsCall=%p)",hsCall));
    if (emaLock((EMAElement)call))
    {
        TRANSERR transRes = cmTransSwitchToSeparate(call->hsTransSession);
        if (transRes == cmTransOK)
            res = RV_OK;
        emaUnlock((EMAElement)call);
    }
    cmiAPIExit((hApp, "cmCallSw2SeparateH245=%d", res));
    return res;
}

/******************************************************************************
 * cmCallSetControlRemoteAddress
 * ----------------------------------------------------------------------------
 * General: Manually sets the remote address to which the stack will try
 *          to connect the H.245 connection.
 *
 * Return Value: RV_OK - if successful.
 *               Negative on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall   - TStack handle for the call
 *         addr     - The remote address
 * Output: none
 *****************************************************************************/
RVAPI
int RVCALLCONV cmCallSetControlRemoteAddress(
                IN      HCALL               hsCall,
                IN      cmTransportAddress  *addr
                                  )
{
    callElem* call=(callElem*)hsCall;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    TRANSERR transErr = cmTransErr;
    int res;

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallSetControlRemoteAddress(hsCall=%p)",hsCall));
    if (emaLock((EMAElement)call))
    {
        HSTRANSHOST host = NULL;

        cmTransGetSessionParam(call->hsTransSession,
                               cmTransParam_H245Connection,
                               (void *)&host);
        if (host)
            transErr = cmTransSetHostParam(host,
                                           cmTransHostParam_remoteAddress,
                                           (void *)addr,
                                           RV_FALSE);
        else
            transErr = cmTransErr;

        emaUnlock((EMAElement)call);
    }

    if (transErr != cmTransOK)
        res = RV_ERROR_UNKNOWN;
    else
        res = RV_OK;

    cmiAPIExit((hApp, "cmCallSetControlRemoteAddress=%d", res));
    return res;
}


#ifdef __cplusplus
}
#endif
