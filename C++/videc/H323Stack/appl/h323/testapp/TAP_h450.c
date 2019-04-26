/***********************************************************************************************************************

  Notice:
  This document contains information that is proprietary to RADVISION LTD..
  No part of this publication may be reproduced in any form whatsoever without
  written prior approval by RADVISION LTD..

    RADVISION LTD. reserves the right to revise this publication and make changes
    without obligation to notify any person of such revisions or changes.

*************************************************************************************************************************/

/********************************************************************************************
*                                TAP_h450.c
*
* This file contains all the functions which enable the use of h450 functions. The functions
* are activated either in the H450 tab in the TestApp, of in the incoming call window (only
* some may be thus activated).
*
*
*      Written by                        Version & Date                        Change
*     ------------                       ---------------                      --------
*       Ran Arad                          18-Dec-2000
*
********************************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "lepConversions.h"
#include "lepCall.h"
#include "lepH450.h"

#include "TAP_h450.h"


int H450_callMake(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call;
    RvBool dial;
    RvInt serviceName;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Get Call Handle */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%p", &call) != 1) return TCL_OK;

    dial = (RvBool)atoi(argv[2]);
    serviceName = atoi(argv[3]);

    if (dial)
    {
        status = epCallMake(call, NULL, 0, NULL, NULL, NULL);
    }
    else
    {
        if (serviceName == (RvInt)sseTransfer)
            status = epH450CallTransferResponse(call->linkedCall, (RvInt)ssCTInitiate, (RvInt)ssCTUnspecified);
        if (status == RV_OK)
            status = epCallDrop(call, cmReasonTypeUndefinedReason, NULL);
    }
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_callTransfer(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call1;
    EpCallObj *call2;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Get Call Handle */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call1) != 1) return TCL_OK;

    if ((argv[2] == NULL) || (strlen(argv[2]) == 0))
    {
        /* Get the "transferred to" call handle from argument 3 */
        if ((argv[3] == NULL) || (strlen(argv[3]) == 0)) return TCL_OK;
        if (sscanf(argv[3], "%*d %p:", &call2) != 1) return TCL_OK;

        status = epH450CallTransfer(call1, call2, NULL);
    }
    else
    {
        /* we have a dest address, this is a blind transfer */
        status = epH450CallTransfer(call1, NULL, argv[2]);
    }
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_TransferNonDefaultResponse(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call;
    int opcode;
    RvBool isOk;
    int errCode;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Get Call Handle */
    if (sscanf(argv[2], "%p", &call) != 1) return TCL_OK;

    opcode = atoi(argv[1]);
    isOk = (RvBool)atoi(argv[3]);
    errCode = atoi(argv[4]);
    if (isOk)
        errCode = 0;

    status = epH450CallTransferResponse(call, opcode, errCode);
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_callReroute(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call;
    reason_e reason;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Get Call Handle */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0) ||
        (argv[2] == NULL) || (strlen(argv[2]) == 0) ) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    /* Setting the fwdCond */
    reason = String2reason_e(argv[3]);

    status = epH450CallReroute(call, argv[2], reason);
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_forwardRequest(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    proc_e fwdCond;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Get dest Address */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0) ||
        (argv[2] == NULL) || (strlen(argv[2]) == 0)) return TCL_OK;

    fwdCond = String2proc_e(argv[2]);

    /* send forward request */
    status = epH450ForwardingRequest(ep, argv[1], fwdCond, (RvBool)atoi(argv[3]));
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_forwardInterrogate(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    proc_e fwdCond;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Get dest Address */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0))
        return TCL_OK;

    fwdCond = String2proc_e(argv[1]);

    /* send forward request */
    status = epH450ForwardInterrogate(ep, fwdCond);
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_callHold(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call;
    RvBool isNear;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;
    isNear = (argv[2][0] == '1');

    status = epH450CallHold(call, isNear);
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_callHoldRtrv(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call;
    RvChar remark[32];
    RvBool isNear;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p: %*s %*s %s", &call, remark) != 2) return TCL_OK;
    isNear = (remark[0] == 'N');

    status = epH450CallHoldRetrieve(call, isNear);
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_HoldSendNonDefaultResponse(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call;
    RvBool isHold;
    RvBool isOk;
    RvInt errCode = 0;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[2], "%p", &call) != 1) return TCL_OK;
    isHold = (argv[1][0] == 'h');
    isOk = atoi(argv[3]);
    if (!isOk)
        errCode = atoi(argv[4]);

    status = epH450CallHoldSendResponse(call, isHold, isOk, errCode);
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_callPark(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    status = epH450CallPark(call, argv[2]);
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_callPick(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call;
    RvInt isLocal;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /*check if local pickup*/
    isLocal = atoi(argv[1]);
    if (isLocal)
    {
        /* Get a call to send with */
        if ((argv[2] == NULL) || (strlen(argv[2]) == 0)) return TCL_OK;
        if (sscanf(argv[2], "%*d %p:", &call) != 1) return TCL_OK;

        status = epH450CallPickup(ep, call, 0);
    }
    else
    {
        /* Make sure a parked call is selected */
        if (!strcmp(argv[3], "Pick")) return TCL_OK;

        status = epH450CallPickup(ep, NULL, atoi(argv[3]));
    }
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_callWait(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    status = epH450CallWait(call);
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


/* Sends Message Indication to the Served User */
int H450_MC_Activate(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

#ifdef USE_H450
    /* Check dest Address */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;

    status = epH450MwiActivate(ep, argv[1], (argv[2][0] == '1'), (argv[3][0] == '1'));
#else
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


/* Sends Interrogation Message to the Message Center */
int H450_SU_Interogate(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

#ifdef USE_H450
    /* Check dest Address */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;

    status = epH450MwiInterrogate(ep, argv[1]);
#else
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


/* General function for sending non default response to MWI messages */
int H450_MWISendNonDefaultResponse(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%p", &call) != 1) return TCL_OK;

    status = epH450MwiSendResponse(call, (RvBool)atoi(argv[2]), atoi(argv[3]));
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


/* Sends a CallCompletion Setup message */
int H450_callCompletion(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *originalCall = NULL;
    RvBool canRetainService;
    RvBool isBusy;

    RV_UNUSED_ARG(interp);

    /* Validity checks */
    if ((argv[3] == NULL) || (strlen(argv[3]) == 0)) return TCL_OK;

    /* Get the original call if we have one */
    if ((argc > 4) && (argv[4] != NULL) && (strlen(argv[4]) > 0))
    {
        if (sscanf(argv[4], "%*d %p", &originalCall) != 1)
            return TCL_OK;
    }

    isBusy = atoi(argv[1]);
    canRetainService = atoi(argv[2]);

    status = epH450CallCompletion(ep, argv[3], originalCall, isBusy, canRetainService);
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}

/* Sends a CallCompletion message other than Setup */
int H450_callCompletionAction(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    RvInt32 serviceId;

    RV_UNUSED_ARG(interp);

    /* Validity checks */
    if ((argc != 3) || (strlen(argv[2]) == 0)) return TCL_OK;

    /* Get the service for this call */
    if (sscanf(argv[2], "%d", &serviceId) != 1) return TCL_OK;

    status = epH450CallCompletionMessage(ep, argv[1], serviceId);
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


/* Sends a CallOffer Setup message */
int H450_callOffer(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
#ifdef USE_H450
    EpObj *ep = (EpObj *)clientData;
    EpCallObj *call;
    int                   overrideCFB;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure received the arguments */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if ((argv[2] == NULL) || (strlen(argv[2]) == 0)) return TCL_OK;

    /* converting from a string to useable variable */
    overrideCFB = (argv[2][0] != '0');

    call = epH450CallOffer(ep, overrideCFB);

    if (call != NULL)
    {
        TclExecute("Call.Make {%s} %p", argv[1], call);
    }
#else
    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TCL_OK;
}


int H450_remoteUserAlerting(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p", &call) != 1) return TCL_OK;

    status = epH450CallOfferRemoteUserAlerting(call);
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_callIntrusion(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    ciPrimitivesEnum intrusionType = (ciPrimitivesEnum)0;
    EpCallObj *call = NULL;
    RvChar *pCallId = NULL;
    RvChar callId[16];
    RvChar *callIdStr;
    RvInt i;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure received the arguments */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;

    /* convert the argument from a string to usable variable */
    switch(argv[2][0])
    {
    case('f'):
        intrusionType = ssCIForcedRelease;
        break;
    case('i'):
        if (argv[3] == NULL) return TCL_OK;
        if (strlen(argv[3])==0)
        {
            intrusionType = ssCIRequest;
        }
        else
        {
            switch(argv[3][0])
            {
            case('f'):
                intrusionType = ssCIForcedRelease;
                break;
            case('w'):
                intrusionType = ssCIWOBRequest;
                break;
            case('i'):
                intrusionType = ssCIIsolated;
                break;
            }

            if ((argv[5] == NULL) || (strlen(argv[5]) == 0)) return TCL_OK;
            if (sscanf(argv[5], "%*d %p:", &call) != 1) return TCL_OK;
        }
        break;
    case('s'):
        intrusionType = ssCISilentMoitor;

        callIdStr = argv[6];
        for (i = 0; i < 16; i++)
        {
            RvUint32 tmpVal;
            sscanf(callIdStr+2*i, "%02X", &tmpVal);
            callId[i] = (RvChar)tmpVal;
        }
        pCallId = callId;
        break;
    }

    if (argv[4][0] != '0') /* argv[4] is Use Waiting Call checkbox */
    {
        if ((argv[5] == NULL) || (strlen(argv[5]) == 0)) return TCL_OK;
        if (sscanf(argv[5], "%*d %p:", &call) != 1) return TCL_OK;
    }

    status = epH450CallIntrusion(ep, intrusionType, (RvUint8 *)pCallId, call, &call);

    if ((status == RV_OK) && (call != NULL))
    {
        TclExecute("Call.Make {%s} %p", argv[1], call);
    }
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


int H450_CMN_Activate(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus status = RV_OK;
#ifdef USE_H450
    EpCallObj *call;
    RvChar *addr;

    int i;
    cmnPrimitivesEnum action = ssCMNBoth;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    i= (cmnPrimitivesEnum) atoi(argv[2]);
    if (i == 0)
        action = ssCMNRequest;
    else
        if (i == 1)
            action = ssCMNInform;
        else
            if (i == 2)
                action = ssCMNBoth;

    addr = argv[3];

    if (addr[0])
    {
        call = epCallCreate(ep, NULL, 0);

        status = epH450CallCommonInformation(call, action);
        if (status < 0)
            cmCallDrop(call->hsCall);
        else
            TclExecute("Call.Make {%s} %p", addr, call);
    }
    else
    {
        /* Get Call Handle */
        if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
        if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

        status = epH450CallCommonInformation(call, action);
    }
#else
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#endif
    return TclReturnValue(ep, status);
}


#ifdef __cplusplus
}
#endif
