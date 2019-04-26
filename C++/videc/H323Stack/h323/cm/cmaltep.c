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

#include "cmdebprn.h"
#include "cmCall.h"
#include "rasdef.h"
#include "cmutils.h"
#include "cmaltep.h"


#ifdef __cplusplus
extern "C" {
#endif




/************************************************************************
 *
 *                          Private functions
 *
 ************************************************************************/


/****************************************************************************
 * cmCallGetAltEpDestinationAddress
 * purpose: Get alternate EP destination address alias
 * input  : call - Stack handle for the call
 *          type - type of address alias
 *          alias - address of the alias structure to story the result
 * output : address alias found
 * return : RV_OK on success
 *          Negative value on failure
 ****************************************************************************/
int cmCallGetAltEpDestinationAddress(
    IN  callElem *  call,
    IN  cmAliasType type,
    OUT cmAlias *   alias)
{
    cmElem *app = (cmElem *)emaGetInstance((EMAElement)call);
    RvPvtNodeId altEpNode;
    RvPvtNodeId adrNode;
    RvStatus res=RV_ERROR_UNKNOWN;

    /* get node of altEp that was connected (call->altEpNum) */
    altEpNode = pvtGetByIndex(app->hVal, call->altEpProperty, call->altEpNum, &adrNode);

    /* get its data */
    __pvtGetNodeIdByFieldIds(adrNode, app->hVal, altEpNode, {_q931(aliasAddress) _anyField LAST_TOKEN});

    while (RV_PVT_NODEID_IS_VALID(adrNode) && (res != RV_OK))
    {
        cmVt2Alias(app->hVal, alias, adrNode);
        if (alias->type == type)
            res=RV_OK;
        else
            adrNode = pvtBrother(app->hVal, adrNode);
    }

    return res;
}


/****************************************************************************
 * cmCallGetAltEpCallSignalAddress
 * purpose: Get alternate EP call signal address
 * input  : call - Stack handle for the call
 *          tpktAddress - pointer to the structure to story the result
 * output : address found
 * return : Non negative on success
 *          Negative value on failure
 ****************************************************************************/
RvStatus cmCallGetAltEpCallSignalAddress(IN callElem* call, OUT cmTransportAddress *tpktAddress)
{
    cmElem *app = (cmElem *)emaGetInstance((EMAElement)call);
    RvPvtNodeId altEpNode;
    RvPvtNodeId adrNode;
    RvPvtNodeId tmpNode;
    RvStatus res=RV_ERROR_UNKNOWN;

    altEpNode = pvtGetByIndex(app->hVal, call->altEpProperty, call->altEpNum, &adrNode);
    if (!RV_PVT_NODEID_IS_VALID(adrNode))
    {
        /* no more Alt EPs */
        return RV_ERROR_NOT_FOUND;
    }

    tmpNode = pvtGetChild(app->hVal, altEpNode, __q931(callSignalAddress), NULL);
    adrNode = pvtChild(app->hVal, tmpNode);
    if (RV_PVT_NODEID_IS_VALID(adrNode))
    {
        res=cmVtToTA(app->hVal, adrNode, tpktAddress);
    }
    return res;
}



/****************************************************************************
 * cmCallSetAlternateEndpoints
 * purpose: copy alternateEndpoints to the CALL
 * input  : hsCall - Stack handle for the call
 *          altEpsNode - the VT node root of altEP data (obtained from ACF)
 * output : no
 * return : Non negative on success
 *          Negative value on failure
 ****************************************************************************/
int cmCallSetAlternateEndpoints(IN HCALL        hsCall,
                                IN RvPvtNodeId  altEpsNode)
{
    callElem* call = (callElem *)hsCall;
    HAPP hApp =(HAPP)emaGetInstance((EMAElement)hsCall);
    cmElem* app;
    int res;

    app=(cmElem*)hApp;
    call->altEpProperty = pvtAddRoot(app->hVal,app->hSyn,0,NULL);
    res=pvtSetTree(app->hVal, call->altEpProperty, app->hVal, altEpsNode);

    return res;
}


/****************************************************************************
 * cmCallSetAltEpDestination
 * purpose: modify the outgoing calledPartyNumber and destinationAddress (E164)
 *          in the H.225 setup message, based on the dialedDigits information
 *          in the aliasAddress parameter in the alternate endpoints field.
 *
 * input  : hsCall - Stack handle for the call
 * output : no
 * return : Non negative on success
 *          Negative value on failure
 ****************************************************************************/
int cmCallSetAltEpDestination(IN HCALL hsCall)
{
    callElem* call = (callElem *)hsCall;
    char  buf[128];
    cmAlias alias;
    cmTransportAddress tpkt;
    RvInt aliasLen;
    RvInt i=0;
    int res = RV_OK;

    alias.string = buf; /* prepare a place for an alias */

    while (res==RV_OK)
    {
        res=cmCallGetParam(hsCall, cmParamDestinationAddress, i, &aliasLen, (char *)&alias);
        if ((res == RV_OK) && (alias.type == cmAliasTypeE164))
        {
            /* add new alt Ep alias */
            cmCallGetAltEpDestinationAddress(call, cmAliasTypeE164, &alias);
            res = cmCallSetParam(hsCall, cmParamDestinationAddress, i, sizeof(cmAlias),(char *)&alias);

            /* also substitute called party Number */
            res = cmCallSetParam(hsCall, cmParamCalledPartyNumber, 0, sizeof(alias), (char *)&alias);

            /* done! */
            break;
        }
        i++;
    }

    res = cmCallGetAltEpCallSignalAddress(call, &tpkt);
    if (res == RV_OK)
    {
        res = cmCallSetParam(hsCall, cmParamDestCallSignalAddress, 0, sizeof(tpkt),(char *)&tpkt);
    }

    return res;
}


#ifdef __cplusplus
}
#endif


