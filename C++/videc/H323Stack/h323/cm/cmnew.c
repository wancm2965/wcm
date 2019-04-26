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
#include "cmdebprn.h"
#include "cmparam.h"
#include "cmCall.h"
#include "psyntreeStackApi.h"
#include "cmCrossReference.h"
#include "q931asn1.h"
#include "rvh323mibstats.h"

/* Syntax definitions */
#include "commonasn.h"
#include "q931asn1.h"


#ifdef __cplusplus
extern "C" {
#endif


RVAPI char* RVCALLCONV
cmGetProtocolMessageName(
                         IN      HAPP                hApp,
                         IN      int                 msg)
{
    RvPstFieldId fieldId=RV_ERROR_UNKNOWN;
    char * ret;
    HPST hSyn;
    cmElem* app=(cmElem*)hApp;
    int synNodeId;
    if (!app || msg<0) return NULL;
    cmiAPIEnter((hApp, "cmGetProtocolMessageName: hApp=%p.", hApp));

    hSyn = pvtGetSynTree(app->hVal, msg);

    if (pvtGet(app->hVal, msg, NULL, &synNodeId, NULL, NULL) < 0)
        ret = NULL;
    else
    {
        if (pstAreNodesCongruent(hSyn,synNodeId,app->synProtRAS,pstGetRoot(app->synProtRAS)))
            pvtGet(app->hVal, pvtChild(app->hVal, msg),&fieldId, NULL, NULL, NULL);
        else
        if (pstAreNodesCongruent(hSyn,synNodeId,app->synProtQ931,pstGetRoot(app->synProtQ931)))
            pvtGet(app->hVal, pvtChild(app->hVal, pvtGetChild(app->hVal, msg,__q931(message), NULL)),&fieldId, NULL, NULL, NULL);
        else
        if (pstAreNodesCongruent(hSyn,synNodeId,app->synProtH245,pstGetRoot(app->synProtH245)))
            pvtGet(app->hVal, pvtChild(app->hVal, pvtChild(app->hVal, msg)),&fieldId, NULL, NULL, NULL);
        ret = pstGetFieldNamePtr(hSyn, fieldId);
    }

    cmiAPIExit((hApp, "cmGetProtocolMessageName: hApp=%p,name %s", hApp, ret));
    return ret;

}


RVAPI
RvInt32 RVCALLCONV cmGetLocalCallSignalAddress(
   IN   HAPP                hApp,
   OUT  cmTransportAddress* tr)
{
    cmElem *app = (cmElem *)hApp;
    int rc;

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if (!app)
        return RV_ERROR_NULLPTR;
#endif

    cmiAPIEnter((hApp, "cmGetLocalCallSignalAddress: hApp=%p.", hApp));
    rc = cmVtToLTA(app->hVal, pvtChild(app->hVal, app->q931TpktAddresses), tr);
    cmiAPIExit((hApp, "cmGetLocalCallSignalAddress=%d", rc));
    return rc;
}

/************************************************************************
 * cmGetLocalAnnexEAddress
 * purpose: Gets the local AnnexE address
 *          Annex E is used to send Q931 messages on UDP.
 * input  : hApp    - Application's stack handle
 * output : tr      - The local Annex E transport address.
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
RvInt32 RVCALLCONV cmGetLocalAnnexEAddress(
    IN   HAPP                   hApp,
    OUT  cmTransportAddress*    tr)
{
    cmElem *app = (cmElem *)hApp;
    int rc;

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if (!app)
        return RV_ERROR_NULLPTR;
#endif

    cmiAPIEnter((hApp, "cmGetLocalAnnexEAddress: hApp=%p.", hApp));
    rc = cmVtToTA(app->hVal, pvtChild(app->hVal, app->q931AnnexEAddresses), tr);
    cmiAPIExit((hApp, "cmGetLocalAnnexEAddress=%d", rc));
    return rc;
}



/******************************************************************************
 * cmGetDefaultQ931Messages
 * ----------------------------------------------------------------------------
 * General: Gets the PVT node ID of all default values of Q931 messages that
 *          are used for new incoming and outgoing calls.
 *          The application can use this API to modify the defaults set by the
 *          Stack automatically to its likings (such a change might be removing
 *          the presentationIndicator and screeningIndicator that are set on
 *          messages by default).
 *          It is strongly advised to modify these values before starting to
 *          dial or receive calls, since the way the modifications are handled
 *          for active calls is indeterminate.
 *
 * Return Value: PVT node ID of the default Q931 messages.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp    - Application's handle of the stack
 * Output: None.
 *****************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV cmGetDefaultQ931Messages(
    IN  HAPP    hApp)
{
    cmElem  *app = (cmElem *)hApp;

    if (app == NULL)
        return RV_PVT_INVALID_NODEID;

    return app->appDB;
}



/******************************************************************************
 * cmGetProperty
 * ----------------------------------------------------------------------------
 * General: Gets the PVT node ID of the Property database of the protocol
 *          created by the Stack.
 *
 * Return Value: PVT node ID of the Property database.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsProtocol   - Protocol handle.
 *                        Should be of one of the following types:
 *                        HCALL, HRAS.
 * Output: None.
 *****************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV cmGetProperty(
    IN  HPROTOCOL   hsProtocol)
{
    RvPvtNodeId nodeId = RV_ERROR_UNKNOWN;

    if (emaLock((EMAElement)hsProtocol))
    {
        switch(emaGetType((EMAElement)hsProtocol))
        {
            case RAS_OUT_TX:
            case RAS_IN_TX:
                nodeId = cmiRASGetProperty((HRAS)hsProtocol);
                break;

            case H225CALL:
                nodeId = getCallProperty((HCALL)hsProtocol);
                break;

            case CHAN:
                break; /*We do not support property for channels*/
        }

        emaUnlock((EMAElement)hsProtocol);
    }

    return nodeId;
}


/******************************************************************************
 * cmGetAHandle
 * ----------------------------------------------------------------------------
 * General: Gets the application handle of the Stack instance associated
 *          with the protocol.
 *
 * Return Value: Application handle of the Stack instance associated with the
 *               protocol.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsProtocol   - Protocol handle.
 *                        Should be of one of the following types:
 *                        HCALL, HRAS, HCHAN, HPROTCONN.
 * Output: None.
 *****************************************************************************/
RVAPI HAPP RVCALLCONV cmGetAHandle(
    IN  HPROTOCOL   hsProtocol)
{
    HAPP hApp;

    hApp = (HAPP)emaGetInstance((EMAElement)hsProtocol);

    return hApp;
}





/************************************************************************
 * mibGetStatistic
 * purpose: Get the value of a statistics parameter for the MIB
 * input  : hApp    - Stack's application handle
 *          type    - Parameter type to get
 * output : none
 * return : Parameter's value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV mibGetStatistic(IN HAPP hApp, IN mibStatisticParamEnumT type)
{
    cmElem* app =     (cmElem* )hApp;
    h341StatisticParametersT * pStatistic = (h341StatisticParametersT *)(app->hStatistic);
    if (pStatistic==NULL)
        return RV_ERROR_UNKNOWN;

    return getStatistic( pStatistic,type);
}


/************************************************************************
 * cmMibEventSet
 * purpose: Set MIB notifications from the stack to an SNMP application
 * input  : hApp        - Stack's application handle
 *          mibEvent    - Event callbacks to set
 *          mibHandle   - Handle of MIB instance for the stack
 * output : none
 * return : Non-negative on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmMibEventSet(IN HAPP hApp, IN MibEventT* mibEvent, IN HMibHandleT mibHandle)
{
    cmElem* app =     (cmElem* )hApp;
    app->mibHandle = mibHandle;
    memset(&app->mibEvent,0,sizeof(MibEventT));
    if (mibEvent!=NULL)
        memcpy (&app->mibEvent,mibEvent,sizeof(MibEventT));
    return 0;
}


/************************************************************************
 * cmEmGetCommonSyntax
 * purpose: Returns the syntax of common.asn.
 *          This syntax holds the system's configuration syntax
 *          The syntax is the buffer passed to pstConstruct as syntax.
 * input  : none
 * output : none
 * return : Syntax of common.asn
 ************************************************************************/
RVAPI
RvUint8* RVCALLCONV cmEmGetCommonSyntax(void)
{
    return commonasnGetSyntax();
}


/************************************************************************
 * cmEmGetQ931Syntax
 * purpose: Returns the syntax of Q931/H225 ASN.1
 *          This syntax holds the Q931 and H225 standard's ASN.1 syntax
 *          The syntax is the buffer passed to pstConstruct as syntax.
 * input  : none
 * output : none
 * return : Syntax of Q931/H225 ASN.1
 ************************************************************************/
RVAPI
RvUint8* RVCALLCONV cmEmGetQ931Syntax(void)
{
    return q931asn1GetSyntax();
}


#ifdef __cplusplus
}
#endif
