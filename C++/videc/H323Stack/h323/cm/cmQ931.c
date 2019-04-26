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

#include "rvh323timer.h"
#include "cm.h"
#include "cmintr.h"
#include "q931asn1.h"
#include "cmQ931.h"
#include "cmiQ931.h"
#include "cmCrossReference.h"
#include "cmCall.h"
#include "cmparam.h"

#ifdef __cplusplus
extern "C" {
#endif



/***************************/
/*  I  N  I  T  /  E  N  D */
/***************************/
int q931CallCreate(HQ931 call, int t301, int t302, int t303, int t304, int t310, int t322)
{
    q931Elem* callE=(q931Elem*)call;
    callE->t301=t301;
    callE->t302=t302;
    callE->t303=t303;
    callE->t304=t304;
    callE->t310=t310;
    callE->t322=t322;
    callE->callState=cs_Null;
    callE->timer=NULL;
    callE->timerSE=NULL;

    if (t310 <= 0)
        callE->t310 = t301;
    return RV_TRUE;
}

int q931CallClose(HQ931 call)
{
    q931Elem* callE=(q931Elem*)call;
    cmElem* app = (cmElem *)cmGetAHandle((HPROTOCOL)cmiGetByQ931(call));

    RvH323TimerCancel(app->hTimers, &callE->timerSE);
    RvH323TimerCancel(app->hTimers, &callE->timer);
    callE->callState = cs_Null;
    return 0;
}

callStateE q931GetCallState(HQ931 call)
{
    q931Elem* callE=(q931Elem*)call;
    return callE->callState;
}


/**********************/
/*  T  I  M  E  R  S  */
/**********************/

/* t301 - connectTimeOut (outgoing call, after alerting received) */
static RvBool q931T301Timeout(void* context)
{
    HCALL hsCall;
    HQ931 call=(HQ931)context;
    q931Elem* callE=(q931Elem*)call;

    hsCall = cmiGetByQ931(call);

    if (emaLock((EMAElement)hsCall))
    {
        cmElem *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);
        RvH323TimerClear(app->hTimers, &callE->timer);
        q931CallDrop(call, -1, 102);
        cmIndicate(hsCall, -1, (cmCallQ931MsgType)1301);

        if (!emaWasDeleted((EMAElement)hsCall))
            callE->callState = cs_Null;
        emaUnlock((EMAElement)hsCall);
    }
    return RV_FALSE;
}

/* t302 - responseTimeOut or t302 (incoming call waiting for incomplete address to complete) */
static RvBool q931T302Timeout(void* context)
{
    HCALL hsCall;
    HQ931 call=(HQ931)context;
    q931Elem* callE=(q931Elem*)call;

    hsCall = cmiGetByQ931(call);

    if (emaLock((EMAElement)hsCall))
    {
        cmElem *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);
        RvH323TimerClear(app->hTimers, &callE->timer);
        q931CallDrop(call, -1, -1);
        cmIndicate(hsCall, -1, (cmCallQ931MsgType)1302);

        if (!emaWasDeleted((EMAElement)hsCall))
            callE->callState = cs_Null;
        emaUnlock((EMAElement)hsCall);
    }
    return RV_FALSE;
}

/* t303 - responseTimeOut (outgoing call after dial) */
static RvBool q931T303Timeout(void* context)
{
    HCALL hsCall;
    HQ931 call=(HQ931)context;
    q931Elem* callE=(q931Elem*)call;

    hsCall = cmiGetByQ931(call);

    if (emaLock((EMAElement)hsCall))
    {
        cmElem *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);
        RvH323TimerClear(app->hTimers, &callE->timer);
        q931CallDrop(call, -1, 102);
        cmIndicate(hsCall, -1, (cmCallQ931MsgType)1303);

        if (!emaWasDeleted((EMAElement)hsCall))
            callE->callState = cs_Null;
        emaUnlock((EMAElement)hsCall);
    }
    return RV_FALSE;
}

/* t304 - responseTimeOut or t304 (outgoing call in overlap sending process) */
static RvBool q931T304Timeout(void* context)
{
    HCALL hsCall;
    HQ931 call=(HQ931)context;
    q931Elem* callE=(q931Elem*)call;

    hsCall = cmiGetByQ931(call);

    if (emaLock((EMAElement)hsCall))
    {
        cmElem *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);
        RvH323TimerClear(app->hTimers, &callE->timer);
        q931CallDrop(call, -1, -1);
        cmIndicate(hsCall, -1, (cmCallQ931MsgType)1304);

        if (!emaWasDeleted((EMAElement)hsCall))
            callE->callState = cs_Null;
        emaUnlock((EMAElement)hsCall);
    }
    return RV_FALSE;
}

/* t310 - connectTimeOut (outgoing call after callProceeding received) */
static RvBool q931T310Timeout(void* context)
{
    HCALL hsCall;
    HQ931 call=(HQ931)context;
    q931Elem* callE=(q931Elem*)call;

    hsCall = cmiGetByQ931(call);

    if (emaLock((EMAElement)hsCall))
    {
        cmElem *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);
        RvH323TimerClear(app->hTimers, &callE->timer);
        q931CallDrop(call, -1, 102);
        cmIndicate(hsCall, -1, (cmCallQ931MsgType)1310);

        if (!emaWasDeleted((EMAElement)hsCall))
            callE->callState = cs_Null;
        emaUnlock((EMAElement)hsCall);
    }
    return RV_FALSE;
}

 /* t322 - 4 seconds or t322 (outgoing call waiting for reply to status inquiry) */
static RvBool q931T322Timeout(void* context)
{
    HCALL hsCall;
    q931Elem* callE=(q931Elem*)context;

    hsCall = cmiGetByQ931((HQ931)callE);

    if (emaLock((EMAElement)hsCall))
    {
        cmElem *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);
        RvH323TimerClear(app->hTimers, &callE->timerSE);

        /* Indicate we didn't get a status message */
        cmIndicate(hsCall, RV_PVT_INVALID_NODEID, cmQ931status);

        emaUnlock((EMAElement)hsCall);
    }
    return RV_FALSE;
}

/*************************/
/*  A  C  T  I  O  N  S  */
/*************************/
RvStatus q931CallDial(IN HQ931 call, IN RvPvtNodeId message)
{
    q931Elem* callE = (q931Elem*)call;
    RvStatus status = RV_ERROR_UNKNOWN;

    if (callE->callState == cs_Null)
    {
        HCALL hsCall = cmiGetByQ931((HQ931)callE);
        cmElem *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        RvH323TimerCancel(app->hTimers, &callE->timer);
        callE->timer = RvH323TimerStart(app->hTimers, q931T303Timeout, callE, callE->t303);

        status = sendCallMessage(hsCall, message, cmQ931setup);

        callReleaseMessage(hsCall, cmQ931setup);

        if (status == RV_OK)
            callE->callState = cs_Call_initiated;
    }

    return status;
}


int q931CallInfo(HQ931 call, int message)
{
    q931Elem* callE=(q931Elem*)call;
    int ret = RV_ERROR_ILLEGAL_ACTION;

    if (callE->callState == cs_Overlap_sending)
    {
        HCALL hsCall = cmiGetByQ931((HQ931)callE);
        cmElem *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        RvH323TimerCancel(app->hTimers, &callE->timer);
        callE->timer = RvH323TimerStart(app->hTimers, q931T304Timeout, callE, callE->t304);

        ret = sendCallMessage(hsCall, message, cmQ931information);
        callReleaseMessage(hsCall, cmQ931information);
    }
    else
    if (callE->callState!=cs_Null &&
        callE->callState!=cs_Call_initiated &&
        callE->callState!=cs_Call_present)
    {
        HCALL hsCall = cmiGetByQ931((HQ931)callE);

        ret = sendCallMessage(hsCall, message, cmQ931information);
        callReleaseMessage(hsCall, cmQ931information);
    }
    return ret;
}


int q931CallMoreInfo(HQ931 call, int message)
{
    q931Elem* callE=(q931Elem*)call;

    if ((callE->callState == cs_Call_present) || (callE->callState == cs_Overlap_receiving))
    {
        HCALL hsCall = cmiGetByQ931((HQ931)callE);
        cmElem *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        RvH323TimerCancel(app->hTimers, &callE->timer);
        callE->timer = RvH323TimerStart(app->hTimers, q931T302Timeout, callE, callE->t302);

        if (message < 0)
            message = callGetMessage(hsCall, cmQ931setupAcknowledge);
        sendCallMessage(hsCall, message, cmQ931setupAcknowledge);
        callReleaseMessage(hsCall, cmQ931setupAcknowledge);

        callE->callState = cs_Overlap_receiving;
    }
    return 0;
}


/************************************************************************
 * q931CallCallProceeding
 * purpose: Send Q931 CallProceeding message on a call
 * input  : call    - Stack handle for the Q931 call
 *          message - CallProceeding message to send
 *                    if set to -1, then create the message from the call's
 *                    property database
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallCallProceeding(IN HQ931 call, IN int message)
{
    int status = RV_ERROR_UNKNOWN;
    q931Elem* callE=(q931Elem*)call;

    if (callE->callState == cs_Call_present)
    {
        HCALL hsCall = cmiGetByQ931((HQ931)callE);
        cmElem *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        /* Get the message to send */
        if (message < 0)
            message = callGetMessage(hsCall, cmQ931callProceeding);

        /* Reset the call's Q931 timer */
        RvH323TimerCancel(app->hTimers, &callE->timer);

        /* Send the Progress message */
        status = sendCallMessage(hsCall, message, cmQ931callProceeding);

        /* Release the message if not needed anymore */
        callReleaseMessage(hsCall, cmQ931callProceeding);

        /* Change call's Q931 state to IncomingCallProceeding */
        callE->callState = cs_Incoming_call_proceeding;
    }
    return status;
}


/************************************************************************
 * q931CallProgress
 * purpose: Send Q931 Progress message on a call
 * input  : call    - Stack handle for the Q931 call
 *          message - Progress message to send
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallProgress(IN HQ931 call, IN int message)
{
    int         status = RV_ERROR_UNKNOWN;
    q931Elem*   callE  = (q931Elem*)call;

    if (callE->callState==cs_Call_present ||
        callE->callState==cs_Incoming_call_proceeding ||
        callE->callState==cs_Overlap_receiving ||
        callE->callState==cs_Call_received)
    {
        HCALL   hsCall = cmiGetByQ931((HQ931)callE);
        cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);
        HPVT    hVal = app->hVal;

        /* Send the Progress message */
        status = sendCallMessage(hsCall, message, cmQ931progress);

        /* If we failed to send the message, it is up to the application to delete it */
        if (status >= 0)
            pvtDelete(hVal, message);

        /* Release the message if not needed anymore */
        callReleaseMessage(hsCall, cmQ931progress);
    }

    return status;
}


/************************************************************************
 * q931CallNotify
 * purpose: Send Q931 Notify message on a call
 * input  : call    - Stack handle for the Q931 call
 *          message - Notify message to send
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallNotify(IN HQ931 call, IN int message)
{
    int     status = RV_ERROR_UNKNOWN;
    HCALL   hsCall = cmiGetByQ931((HQ931)call);
    cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);
    HPVT    hVal = app->hVal;

    /* Send the Notify message */
    status = sendCallMessage(hsCall, message, cmQ931notify);

    /* If we failed to send the message, it is up to the application to delete it */
    if (status >= 0)
        pvtDelete(hVal, message);

    /* Release the message if not needed anymore */
    callReleaseMessage(hsCall, cmQ931notify);

    return status;
}


/************************************************************************
 * q931CallAccept
 * purpose: Send Q931 Alerting message on a call
 * input  : call    - Stack handle for the Q931 call to accept
 *          message - Alerting message to send
 *                    if set to -1, then create the message from the call's
 *                    property database
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallAccept(IN HQ931 call, IN int message)
{
    int status = RV_ERROR_UNKNOWN;
    q931Elem* callE=(q931Elem*)call;

    if (callE->callState == cs_Call_present ||
        callE->callState == cs_Incoming_call_proceeding)
    {
        HCALL   hsCall = cmiGetByQ931((HQ931)callE);
        cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        /* Get the message to send */
        if (message < 0)
            message = callGetMessage(hsCall, cmQ931alerting);

        /* Reset the call's Q931 timer */
        RvH323TimerCancel(app->hTimers, &callE->timer);

        /* Send the Alerting message */
        status = sendCallMessage(hsCall, message, cmQ931alerting);

        /* Release the message if not needed anymore */
        callReleaseMessage(hsCall, cmQ931alerting);

        /* Change call's Q931 state to CallReceived */
        callE->callState = cs_Call_received;
    }
    return status;
}


/************************************************************************
 * q931CallAnswer
 * purpose: Send Q931 Connect message on a call
 * input  : call    - Stack handle for the Q931 call to connect
 *          message - Connect message to send
 *                    if set to -1, then create the message from the call's
 *                    property database
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallAnswer(IN HQ931 call, IN int message)
{
    int status = RV_ERROR_UNKNOWN;
    q931Elem* callE=(q931Elem*)call;

    if (callE->callState == cs_Call_present ||
        callE->callState == cs_Call_received ||
        callE->callState == cs_Incoming_call_proceeding)
    {
        HCALL   hsCall = cmiGetByQ931((HQ931)callE);
        cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        /* Get the message to send */
        if (message < 0)
            message = callGetMessage(hsCall, cmQ931connect);

        /* Reset the call's Q931 timer */
        RvH323TimerCancel(app->hTimers, &callE->timer);

        /* Send the Connect message */
        status = sendCallMessage(hsCall, message, cmQ931connect);

        /* Release the message if not needed anymore */
        callReleaseMessage(hsCall, cmQ931connect);

        /* Change call's Q931 state to Active */
        callE->callState = cs_Active;
    }
    return status;
}


/************************************************************************
 * q931CallDrop
 * purpose: Drops a Q931 connection of a call
 * input  : call        - Stack handle for the Q931 call
 *          message     - Release message to send
 *                        If -1, then message will be created
 *          causeValue  - Release cause
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallDrop(IN HQ931 call, IN int message, IN int causeValue)
{
    int status = 0;
    q931Elem* callE=(q931Elem*)call;

    /* Do this only if something already has happened on this call */
    if (callE->callState != cs_Null)
    {
        HCALL   hsCall = cmiGetByQ931((HQ931)callE);
        cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        /* Get the message to send */
        if (message < 0)
            message = callGetMessage(hsCall, cmQ931releaseComplete);
        /* Reset the call's Q931 timer */
        RvH323TimerCancel(app->hTimers, &callE->timer);

        if (causeValue >= 0)
        {
            HPVT hVal = app->hVal;
            RvInt32 curCause = 0;
            RvPvtNodeId tmpNodeId;

            /* see if the application asked for a specific cause value */
            __pvtGetByFieldIds(tmpNodeId, hVal, message,
                {_q931(message) _q931(releaseComplete) _q931(cause) _q931(octet4) _q931(causeValue) LAST_TOKEN},
                NULL, &curCause, NULL);
            if (RV_PVT_NODEID_IS_VALID(tmpNodeId) && (curCause == 16))
                pvtSet(hVal, tmpNodeId, __q931(causeValue), causeValue, NULL);
        }

        /* Send the ReleaseComplete message */
        status = sendCallMessage(hsCall, message, cmQ931releaseComplete);

        /* Release the message if not needed anymore */
        callReleaseMessage(hsCall, cmQ931releaseComplete);

        /* Change call's Q931 state to Null */
        callE->callState = cs_Null;
    }
    return status;
}

/************************************************************************
 * q931CallStatusEnquiry
 * purpose: Send a Q931 STATUS ENQUIRY message
 * input  : call        - Stack handle for the Q931 call
 *          message     - StatusEnquiry message to send
 *                        If -1, then message will be created
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallStatusEnquiry(IN HQ931 call, IN int message)
{
    q931Elem* callE=(q931Elem*)call;
    HCALL   hsCall = cmiGetByQ931((HQ931)callE);
    cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

    RvH323TimerCancel(app->hTimers, &callE->timerSE);
    callE->timerSE = RvH323TimerStart(app->hTimers, q931T322Timeout, callE, callE->t322);

    sendCallMessage(hsCall, message, cmQ931statusEnquiry);
    callReleaseMessage(hsCall, cmQ931statusEnquiry);

    return 0;
}

int q931CallFacility(HQ931 call,int message)
{
    int  ret = 0;
    int  q931NodeId, reasonNodeId, facilityNodeId, emptyNodeId;
    int  reasonId;
    HCALL   hsCall = cmiGetByQ931((HQ931)call);
    cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);
    HPVT hVal;
    hVal = app->hVal;

    if (message < 0)
        message = callGetMessage(hsCall, cmQ931facility);

    /* check if we have the Q.931 facility IE */
    __pvtGetNodeIdByFieldIds(facilityNodeId,hVal,message,
                {_q931(message)
                 _q931(facility)
                 _q931(facility)
                 LAST_TOKEN});

    if (facilityNodeId < 0)
    {
        /* build the Q.931 facility IE */
        __pvtBuildByFieldIds(facilityNodeId,hVal,message,
                            {_q931(message)
                             _q931(facility)
                             _q931(facility)
                             LAST_TOKEN},0,NULL);
        if (facilityNodeId < 0)
            return -1;
    }

    /* check if we have a reason (valid only in case of non-empty facility message) */
    q931NodeId = pvtChild(hVal,pvtGetChild(hVal,message,__q931(message),NULL));

    __pvtGetNodeIdByFieldIds(emptyNodeId,hVal,q931NodeId,
                {_q931(userUser) _q931(h323_UserInformation)
                 _q931(h323_uu_pdu) _q931(h323_message_body)
                 _q931(empty) LAST_TOKEN});

    if (emptyNodeId < 0)
    {
        __pvtGetNodeIdByFieldIds(reasonNodeId,hVal,q931NodeId,
                    {_q931(userUser) _q931(h323_UserInformation)
                     _q931(h323_uu_pdu) _q931(h323_message_body)
                     _q931(facility) _q931(reason) LAST_TOKEN});

        /* if we don't have it take the reason from the parameter of the call */
        if (reasonNodeId < 0)
        {
            cmCallGetParam(hsCall, cmParamFacilityReason, 0, &reasonId, NULL);

            if (reasonId >= 0)
            {
                /* if we have a valid reason build the necessary fields in the message */
                __pvtBuildByFieldIds(reasonNodeId,hVal,q931NodeId,
                        {_q931(userUser) _q931(h323_UserInformation)
                         _q931(h323_uu_pdu) _q931(h323_message_body)
                         _q931(facility) _q931(reason) LAST_TOKEN}, 0, NULL);
                if (reasonNodeId >= 0)
                {
                    /* convert the reason to the proper fieldId and build it in place */
                    RvInt32 nameId = getParamFieldName(cmParamFacilityReason)[reasonId].nameId;

                    if (pvtAdd(hVal, reasonNodeId, nameId, 0, NULL, NULL)<0)
                        pvtDelete(hVal,reasonNodeId);
                }
            }
       }
    }

    ret = sendCallMessage(hsCall, message, cmQ931facility);
    callReleaseMessage(hsCall, cmQ931facility);
    return ret;
}


/************************************************************************
 * q931CallAddressComplete
 * purpose: Indication on an incoming call that the Overlap Sending
 *          procedure has finished and we have a full address.
 * input  : call    - Stack handle for the Q931 call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallAddressComplete(IN HQ931 call)
{
    q931Elem* callE = (q931Elem*)call;

    /* Do this only if we're in OVSP */
    if (callE->callState == cs_Overlap_receiving)
    {
        HCALL   hsCall = cmiGetByQ931((HQ931)callE);
        cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        /* Reset the call's Q931 timer - it's the only thing we actually have to do here... */
        RvH323TimerCancel(app->hTimers, &callE->timer);

        callE->callState = cs_Call_present;
    }

    return 0;
}




/****************************/
/*  M  E  S  S  A  G  E  S  */
/****************************/
int q931SimulateSetup(HQ931 call)
{
    q931Elem* callE=(q931Elem*)call;
    if (callE->callState==cs_Null)
    {
        callE->callState=cs_Call_present;
    }
    return 0;
}

int q931Setup(q931Elem*callE, int message)
{
    /* process the setup, even if we already sent call-proceeding or alerting on the call */
    if (callE->callState==cs_Null || callE->callState==cs_Call_present ||
        callE->callState==cs_Incoming_call_proceeding || callE->callState==cs_Call_received)
    {
        if (callE->callState == cs_Null)
            callE->callState=cs_Call_present;
        cmIndicate(cmiGetByQ931((HQ931)callE),message, cmQ931setup);
    }
    return 0;
}

int q931CallProceeding(q931Elem*callE, int message)
{
    if (callE->callState==cs_Call_initiated||
        callE->callState==cs_Overlap_sending)
    {
        HCALL   hsCall = cmiGetByQ931((HQ931)callE);
        cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        callE->callState = cs_Outgoing_call_proceeding;
        RvH323TimerCancel(app->hTimers, &callE->timer);
        callE->timer = RvH323TimerStart(app->hTimers, q931T310Timeout, callE, callE->t310);
        cmIndicate(hsCall, message, cmQ931callProceeding);
    }
    return 0;
}

int q931Alerting(q931Elem*callE, int message)
{
    if (callE->callState==cs_Call_initiated||
        callE->callState==cs_Overlap_sending||
        callE->callState==cs_Outgoing_call_proceeding)
    {
        HCALL   hsCall = cmiGetByQ931((HQ931)callE);
        cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        callE->callState = cs_Call_delivered;
        RvH323TimerCancel(app->hTimers, &callE->timer);
        callE->timer=RvH323TimerStart(app->hTimers, q931T301Timeout, callE, callE->t301);
        cmIndicate(hsCall, message, cmQ931alerting);
    }
    return 0;
}

int q931Connect(q931Elem*callE, int message)
{
    if (callE->callState==cs_Call_initiated||
        callE->callState==cs_Overlap_sending||
        callE->callState==cs_Outgoing_call_proceeding||
        callE->callState==cs_Call_delivered)
    {
        HCALL   hsCall = cmiGetByQ931((HQ931)callE);
        cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        callE->callState = cs_Active;
        RvH323TimerCancel(app->hTimers, &callE->timer);
        cmIndicate(hsCall, message, cmQ931connect);
    }
    return 0;
}

int q931ReleaseComplete(q931Elem*callE, int message)
{
    HCALL   hsCall = cmiGetByQ931((HQ931)callE);
    cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

    callE->callState = cs_Null;
    RvH323TimerCancel(app->hTimers, &callE->timer);
    RvH323TimerCancel(app->hTimers, &callE->timerSE);
    cmIndicate(hsCall, message, cmQ931releaseComplete);
    return 0;
}

int q931Status(q931Elem*callE, int message)
{
    HCALL   hsCall = cmiGetByQ931((HQ931)callE);
    cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

    RvH323TimerCancel(app->hTimers, &callE->timerSE);
    cmIndicate(hsCall, message, cmQ931status);
    return 0;
}

int q931Facility(q931Elem*callE, int message)
{
    HCALL hsCall = cmiGetByQ931((HQ931)callE);
    cmIndicate(hsCall, message, cmQ931facility);
    return 0;
}


/************************************************************************
 * q931StatusEnquiry
 * purpose: Handles incoming STATUS ENQUIRY message
 *          This should cause an automatic send of STATUS message reply
 * input  : callE       - Stack handle for the Q931 call
 *          message     - StatusEnquiry message received
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931StatusEnquiry(IN q931Elem* callE, IN int message)
{
    HCALL hsCall = cmiGetByQ931((HQ931)callE);
    RvPvtNodeId replyMsg;

    RV_UNUSED_ARG(message);

    /* Create a STATUS message as a reply */
    replyMsg = callGetMessage(hsCall, cmQ931status);

    if (replyMsg >= 0)
    {
        /* Set the cause value and call state on the reply */
        RvPvtNodeId tmpNodeId, tmpNodeId1;
        cmElem* app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);
        HPVT hVal;
        hVal = app->hVal;

        __pvtGetNodeIdByFieldIds(tmpNodeId,hVal,replyMsg, {_q931(message) _anyField LAST_TOKEN});
        __pvtBuildByFieldIds(tmpNodeId1, hVal, tmpNodeId,
                            {_q931(cause) _q931(octet4) _q931(causeValue) LAST_TOKEN}, 30, NULL);
        __pvtBuildByFieldIds(tmpNodeId1, hVal, tmpNodeId,
                            {_q931(callState) _q931(callStateValue) LAST_TOKEN}, (RvInt32)callE->callState, NULL);

        /* Send the reply and be done with it */
        sendCallMessage(hsCall, replyMsg, cmQ931status);
        callReleaseMessage(hsCall, cmQ931status);
        RV_UNUSED_ARG(tmpNodeId1);
    }

    return 0;
}


int q931Progress(q931Elem*callE, int message)
{
    if (callE-> callState==cs_Call_initiated ||
        callE->callState==cs_Overlap_sending ||
        callE->callState==cs_Outgoing_call_proceeding ||
        callE->callState==cs_Call_delivered )
    {
        HCALL   hsCall = cmiGetByQ931((HQ931)callE);
        cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        RvH323TimerCancel(app->hTimers, &callE->timer);
        cmIndicate(hsCall, message, cmQ931progress);
    }
    return 0;
}

int q931SetupAcknowledge(q931Elem*callE, int message)
{
    if (callE->callState==cs_Call_initiated)
    {
        HCALL   hsCall = cmiGetByQ931((HQ931)callE);
        cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        callE->callState = cs_Overlap_sending;
        RvH323TimerCancel(app->hTimers, &callE->timer);
        callE->timer = RvH323TimerStart(app->hTimers, q931T304Timeout, callE, callE->t304);
        cmIndicate(hsCall, message, cmQ931setupAcknowledge);
    }
    return 0;
}

int q931Information(q931Elem*callE, int message)
{
    HCALL hsCall = cmiGetByQ931((HQ931)callE);

    if (callE->callState == cs_Overlap_receiving)
    {
        cmElem  *app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);

        emaMark((EMAElement)hsCall);
        cmIndicate(hsCall, message, cmQ931information);
        if (emaWasDeleted((EMAElement)hsCall))
        {
            emaRelease((EMAElement)hsCall);
            return 0;
        }

        RvH323TimerCancel(app->hTimers, &callE->timer);
        callE->timer = RvH323TimerStart(app->hTimers, q931T302Timeout, callE, callE->t302);
        callE->callState = cs_Overlap_receiving;
        emaRelease((EMAElement)hsCall);
    }

    if (callE->callState!=cs_Null &&
        callE->callState!=cs_Call_initiated &&
        callE->callState!=cs_Call_present &&
        callE->callState!=cs_Overlap_receiving)
    {
        cmIndicate(hsCall, message, cmQ931information);
    }
    return 0;
}

int q931Notify(q931Elem*callE, int message)
{
    HCALL hsCall = cmiGetByQ931((HQ931)callE);

    cmIndicate(hsCall, message, cmQ931notify);
    return 0;
}

/************************************************************************
 * q931DecodingFailure
 * purpose: Handle incoming Q931 messages that can't be decoded
 *          This automatically sends back a STATUS message
 * input  : callE       - Stack handle for the Q931 call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931DecodingFailure(IN HQ931 call)
{
    int message;
    q931Elem* callE = (q931Elem *)call;
    HCALL hsCall = cmiGetByQ931((HQ931)callE);

    /* Create a STATUS message to send back */
    message = callGetMessage(hsCall, cmQ931status);

    if (message >= 0)
    {
        /* The causeValue used is 95 - Invalid message, Unspecified */
        int tmpNodeId, tmpNodeId1;
        cmElem* app = (cmElem *)cmGetAHandle((HPROTOCOL)hsCall);
        HPVT hVal;
        hVal = app->hVal;
        __pvtGetNodeIdByFieldIds(tmpNodeId, hVal, message, {_q931(message) _anyField LAST_TOKEN});
        __pvtBuildByFieldIds(tmpNodeId1,hVal, tmpNodeId,
                            {_q931(cause) _q931(octet4) _q931(causeValue) LAST_TOKEN}, 95, NULL);
        __pvtBuildByFieldIds(tmpNodeId1, hVal, tmpNodeId,
                            {_q931(callState) _q931(callStateValue) LAST_TOKEN}, (RvInt32)callE->callState, NULL);

        /* Send the STATUS message and wrap things up */
        sendCallMessage(hsCall, message, cmQ931status);
        callReleaseMessage(hsCall, cmQ931status);
        RV_UNUSED_ARG(tmpNodeId1);
    }

    return 0;
}


/************************************************************************
 * q931ProcessMessage
 * purpose: Handle incoming Q931 messages
 * input  : call    - Stack handle for the Q931 call
 *          message - Node Id of the massage to handle
 *                    This function doesn't delete the message's node Id,
 *                    but modifies its value
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931ProcessMessage(IN HQ931 call, IN int message)
{
    q931Elem* callE=(q931Elem*)call;
    cmElem* app = (cmElem *)cmGetAHandle((HPROTOCOL)cmiGetByQ931(call));
    int msgType;

    msgType = pvtGetChildTagByPath(app->hVal,message,"message",1);

    /*Move the message tree into call database*/
    if (msgType != (int)cmQ931setup)
        message = callSetMessage(cmiGetByQ931(call),(cmCallQ931MsgType)msgType,message);

    switch (msgType)
    {
        case cmQ931setup            :q931Setup(callE,message);              break;
        case cmQ931callProceeding   :q931CallProceeding(callE,message);     break;
        case cmQ931connect          :q931Connect(callE,message);            break;
        case cmQ931alerting         :q931Alerting(callE,message);           break;
        case cmQ931releaseComplete  :q931ReleaseComplete(callE,message);    break;
        case cmQ931status           :q931Status(callE,message);             break;
        case cmQ931facility         :q931Facility(callE,message);           break;
        case cmQ931statusEnquiry    :q931StatusEnquiry(callE,message);      break;
        case cmQ931progress         :q931Progress(callE,message);           break;
        case cmQ931setupAck         :q931SetupAcknowledge(callE,message);   break;
        case cmQ931information      :q931Information(callE,message);        break;
        case cmQ931notify           :q931Notify(callE,message);             break;
        default:
            /* Nothing to do here... We don't handle these types of messages :-) */
            break;
    }
    return RV_TRUE;
}




#ifdef __cplusplus
}
#endif
