
/*

NOTICE:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*/

#include "cmintr.h"
#include "cm.h"
#include "cmCall.h"
#include "cmQ931.h"
#include "cmchan.h"
#include "cmControl.h"
#include "cmCtrlMSD.h"
#include "cmCtrlCap.h"
#include "transport.h"
#include "rvh323mibstats.h"


#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 *
 *                              Public functions
 *
 ************************************************************************/


/************************************************************************
 * mibGetCallParameters
 * purpose: Get call related MIB parameters
 * input  : hsCall      - Call to check
 *          type        - Parameter type to get
 * output : valueSize   - Value, if numeric
 *                        String's length if string value type
 *          value       - String value if applicable
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV mibGetCallParameters(
    IN  HCALL               hsCall,
    IN  mibCallParamTypeT   type,
    OUT int*                valueSize,
    OUT RvUint8*            value)
{
    RV_UNUSED_ARG(value);

    switch (type)
    {
        case enumconnectionsState:
        {
            switch (q931GetCallState(cmiGetQ931(hsCall)))
            {
                case cs_Call_initiated:             *valueSize = (int)callInitiated; break;
                case cs_Call_delivered:             *valueSize = (int)callDelivered; break;
                case cs_Call_present:               *valueSize = (int)callPresent; break;
                case cs_Call_received:              *valueSize = (int)callReceived; break;
                case cs_Connect_request:            *valueSize = (int)connectRequest; break;
                case cs_Outgoing_call_proceeding:   *valueSize = (int)cscallProceeding; break;
                case cs_Active:                     *valueSize = (int)active; break;
                case cs_Disconnect_request:         *valueSize = (int)disconnectRequest; break;
                case cs_Disconnect_indication:      *valueSize = (int)disconnectIndication; break;
                case cs_Release_request:            *valueSize = (int)releaseRequest; break;
                default:
                    return RV_ERROR_UNKNOWN;
            }
            return 0;
        }
        case enumconnectionsFastCall:
            break;
        case enumconnectionsH245Tunneling:
            break;
    }

    /* No such luck in finding the parameter */
    return RV_ERROR_UNKNOWN;
}


/************************************************************************
 * cmCallGetH245Address
 * purpose: Get H245 addresses for a call
 * input  : hsCall      - Call to check
 * output : trSrc       - Source H245 address
 *          trDest      - Destination H245 address
 *          isConnected - Indication if H245 channel is connected or not
 * return : Non-negative value on success
 *          Negative value on failure
 * Note: If H.245 is not connected through a dedicated connection (i.e
 *       uses fast start or tunneling), then the returned addresses are
 *       the Q931 addresses
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallGetH245Address(
    IN  HCALL               hsCall,
    OUT cmTransportAddress* trSrc,
    OUT cmTransportAddress* trDest,
    OUT int*                isConnected)
{
    HSTRANSSESSION  hSession;
    HSTRANSHOST     hHost;
    int             status;
    RvInt32         isTunneling;
    RvBool          isOrigin;

    *isConnected = 0;

    /* See if we're working with tunneling on this call */
    if (cmCallHasControlSession(hsCall))
    {
        status = cmCallGetParam(hsCall, cmParamH245Tunneling, 0, &isTunneling, NULL);
        if (status < 0) return status;

        if (!isTunneling)
        {
            *isConnected = 1;
        }
    }

    if (!(*isConnected))
    {
        /* Get the Q931 addresses */
        int length = sizeof(cmTransportAddress);
        status = cmCallGetParam(hsCall, cmParamSrcCallSignalAddress, 0, &length, (char *)trSrc);
        if (status < 0)
            return RV_ERROR_UNKNOWN;

        length = sizeof(cmTransportAddress);
        status = cmCallGetParam(hsCall, cmParamDestCallSignalAddress, 0, &length, (char *)trDest);
        if (status < 0)
            return RV_ERROR_UNKNOWN;
        return 0;
    }

    /* Get H245 address */
    if (cmCallGetOrigin(hsCall, &isOrigin) < 0)
        return RV_ERROR_UNKNOWN;

    hSession = callMibGetSession(hsCall);
    if (emaLock((EMAElement)hSession))
    {
        if (cmTransGetSessionParam(hSession, cmTransParam_H245Connection, (void *)&hHost) != cmTransOK)
        {
            emaUnlock((EMAElement)hSession);
            return RV_ERROR_UNKNOWN;
        }

        if (isOrigin)
        {
            cmTransGetHostParam(hHost, cmTransHostParam_localAddress, trSrc);
            cmTransGetHostParam(hHost, cmTransHostParam_remoteAddress, trDest);
        }
        else
        {
            cmTransGetHostParam(hHost, cmTransHostParam_localAddress, trDest);
            cmTransGetHostParam(hHost, cmTransHostParam_remoteAddress, trSrc);
        }
        emaUnlock((EMAElement)hSession);
    }

    return 0;
}


/************************************************************************
 * mibGetControlParameters
 * purpose: Get control related MIB parameters
 * input  : hsCall      - Call to check
 *          inDirection - Direction to get
 *          type        - Parameter type to get
 * output : valueSize   - Value, if numeric
 *                        String's length if string value type
 *          value       - String value if applicable
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV mibGetControlParameters(
    IN  HCALL                   hsCall,
    IN  int                     inDirection,
    IN  mibControlParamTypeT    type,
    OUT int*                    valueSize,
    OUT RvUint8*                value)
{
    int status = RV_ERROR_UNKNOWN;

    if (hsCall == NULL) return RV_ERROR_UNKNOWN;

    switch (type)
    {
        case enumh245ControlChannelMSDState:
        {
            MsdState    state;
            MsdStatus   mStatus;
            status = msdGetMibParams((H245Control *)cmiGetControl(hsCall), &state, &mStatus, NULL, NULL);
            if (status < 0) return status;

            switch (mStatus)
            {
                case MsdStatusIndeterminate:
                    switch (state)
                    {
                        case MsdStateIdle:
                                *valueSize = (int)msdseIdle; return 0;
                        case MsdStateIncomingAwaitingResponse:
                                *valueSize = (int)msdseIncomingWaitingResponse; return 0;
                        case MsdStateIncomingAwaitingManualAcknoledge:
                        case MsdStateOutgoingAwaitingResponse:
                                *valueSize = (int)msdseOutgoingWaitingResponse; return 0;
                    }
                    break;
                case MsdStatusMaster:    *valueSize = (int)msdseMaster; return 0;
                case MsdStatusSlave:     *valueSize = (int)msdseSlave; return 0;
            }
            break;
        }
        case enumh245ControlChannelTerminalType:
        {
            int termType;
            status = msdGetMibParams((H245Control *)cmiGetControl(hsCall), NULL, NULL, NULL, &termType);
            if (status >= 0)
            {
                *valueSize = termType;
                return 0;
            }
            break;
        }
        case enumh245ControlChannelNumberOfMSDRetries:
        {
            int retries;
            status = msdGetMibParams((H245Control *)cmiGetControl(hsCall), NULL, NULL, &retries, NULL);
            if (status >= 0)
            {
                *valueSize = retries;
                return 0;
            }
            break;
        }
        case enumh245ControlChannelIsTunneling:
        {
            RvInt32 isTunneling;
            status = cmCallGetParam(hsCall, cmParamH245Tunneling, 0, &isTunneling, NULL);
            if (status >= 0)
            {
                *valueSize = isTunneling;
                return 0;
            }
            break;
        }
        case enumh245CapExchangeState:
        {
            CapStatus cStatus;
            status = tcsGetMibParams((H245Control *)cmiGetControl(hsCall), (RvBool)inDirection, &cStatus, NULL);
            if (status >= 0)
            {
                switch (cStatus)
                {
                    case CapStatusReleased:       *valueSize = (int)ceseRelease; return 0;
                    case CapStatusSent:           *valueSize = (int)ceseSent; return 0;
                    case CapStatusAcknowledged:   *valueSize = (int)ceseAck; return 0;
                    case CapStatusRejected:       *valueSize = (int)ceseReject; return 0;
                }
            }
            break;
        }
        case enumh245CapExchangeProtocolId:
        {
            status = tcsGetMibProtocolId((H245Control *)cmiGetControl(hsCall), (RvBool)inDirection, valueSize, value);
            if (status >= 0) return 0;
            break;
        }
        case enumh245CapExchangeRejectCause:
        {
            status = tcsGetMibParams((H245Control *)cmiGetControl(hsCall), (RvBool)inDirection, NULL, valueSize);
            if (status >= 0)
            {
                if (*valueSize < 0)
                    status = *valueSize;
                else
                    return 0;
            }
            break;
        }
        default:
            status = RV_ERROR_UNKNOWN;
    }

    return status;
}


/************************************************************************
 * mibGetChannelParameters
 * purpose: Get channel related MIB parameters
 * input  : hsChan      - Channel to check
 *          type        - Parameter type to get
 * output : valueSize   - Value, if numeric
 *                        String's length if string value type
 *          value       - String value if applicable
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV mibGetChannelParameters(
    IN  HCHAN                   hsChan,
    IN  mibControlParamTypeT    type,
    OUT int*                    valueSize,
    OUT RvUint8*                value)
{
    if (hsChan == NULL) return RV_ERROR_UNKNOWN;

    return chanGetMibParam(hsChan, type, valueSize, value);
}



#ifdef __cplusplus
}
#endif

