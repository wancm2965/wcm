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

/***********************************************************************
epUtils.c

Utility functions widely used by this module.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "lepUtils.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifndef USE_ATT
#if (RV_OS_TYPE == RV_OS_TYPE_WIN32) || (RV_OS_TYPE == RV_OS_TYPE_WINCE)
#include <stdio.h>
#define RvVsnprintf _vsnprintf
#else
#include <stdarg.h>
#define RvVsnprintf vsnprintf
#endif
#else
#include "rvansi.h"
#endif



/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/





/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/





/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
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
    IN const RvChar *eventStr, ...)
{
    RvChar msg[1024];
    va_list varArg;

    va_start(varArg, eventStr);
    RvVsnprintf(msg, sizeof(msg), eventStr, varArg);
    va_end(varArg);

    ep->cb.epEventIndication(ep, eventType, call, msg);
}


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
    IN const RvChar *logStr, ...)
{
    RvChar msg[2048];
    va_list varArg;

    va_start(varArg, logStr);
    RvVsnprintf(msg, sizeof(msg), logStr, varArg);
    va_end(varArg);

    ep->cb.epLog(ep, call, msg);
}


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
    IN const RvChar *logStr, ...)
{
    RvChar msg[2048];
    va_list varArg;

    strcpy(msg, "Error: ");
    va_start(varArg, logStr);
    RvVsnprintf(msg+7, (RvSize_t)(sizeof(msg) - 7), logStr, varArg);
    RvVsnprintf(ep->lastError, sizeof(ep->lastError), logStr, varArg);
    va_end(varArg);

    ep->resetError = RV_FALSE;
    ep->lastError[sizeof(ep->lastError)-1] = '\0';
    ep->cb.epLog(ep, call, msg);
}


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
    OUT const RvChar    **errStr)
{
    if (ep->resetError)
    {
        *errStr = NULL;
    }
    else
    {
        *errStr = ep->lastError;
        ep->resetError = RV_TRUE;
    }
}


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
    IN  RvPvtNodeId     node)
{
    RvPvtNodeId lastGoodNode;
    RvUint8 actions[10];
    RvUint32 random;
    int i;

    node = pvtChild(ep->hVal, node);
    if (!RV_PVT_NODEID_IS_VALID(node)) return;
    lastGoodNode = node;

    random = (RvUint32)(rand()*rand());
    for (i=0; i<10; i++)
    {
        actions[i] = (RvUint8)((random & 0x3) % 3);
        random = (random >> 2);
    }

    for (i=0; i<10; i++)
    {
        if (actions[i] == 0)
        {
            node = pvtChild(ep->hVal, node);
        }
        else if (actions[i] == 1)
        {
            node = pvtBrother(ep->hVal, node);
        }
        else if (actions[i] == 2)
        {
            node = pvtParent(ep->hVal, node);
        }

        if (RV_PVT_NODEID_IS_VALID(node))
            lastGoodNode = node;
        else
            node = lastGoodNode;
    }
}

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
     IN cmTransportAddress* ta)
{
    RV_UNUSED_ARG(ep);
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    if (ta->type == cmTransportTypeIPv6)
    {
        static char ipv6Null[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        return (memcmp(ta->addr.v6.ip, ipv6Null, 16) != 0);
    }
    if (ta->type == cmTransportTypeIP)
        return (ta->addr.v4.ip != 0);

    return RV_FALSE;
#else
    return (ta->ip != 0);
#endif
}




/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/






#ifdef __cplusplus
}
#endif
