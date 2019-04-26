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
#include "cmdebprn.h"
#include "cmCall.h"
#include "cmparam.h"
#include "cmutils.h"


#ifdef __cplusplus
extern "C" {
#endif

RVAPI
int RVCALLCONV cmCallJoin(
                        IN      HCALL               hsCall,
                        IN      HCALL               hsSameConferenceCall)
{
    HAPP hApp=cmGetAHandle((HPROTOCOL)hsCall);
    char CID[16];
    cmTransportAddress qAddress;
    RvInt32 val = (RvInt32)sizeof(qAddress);
    char number[256];
    cmAlias alias;
    int status = 0;
    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallJoin(hsCall=0x%lx,hsSameConferenceCall=%p)", hsCall, hsSameConferenceCall));
    alias.string = number;
    cmCallGetParam(hsSameConferenceCall,cmParamCID,0,0,CID);
    if (!cmCallGetOrigin(hsCall,NULL))
    {
        switch (cmCallMasterSlaveStatus(hsSameConferenceCall))
        {
            case cmMSSlave:
            if (cmCallGetOrigin(hsSameConferenceCall,NULL))
            {
                int i = 0;
                alias.length = sizeof(number);
                if (cmCallGetParam(hsSameConferenceCall,cmParamCalledPartyNumber,0,0,(char *)&alias)>=0)
                    cmCallSetParam(hsCall,cmParamCalledPartyNumber,0,sizeof(cmAlias),(char *)&alias);
                alias.length = sizeof(number);
                while (cmCallGetParam(hsSameConferenceCall,cmParamDestinationAddress,i,0,(char *)&alias)>=0)
                {
                    cmCallSetParam(hsCall,cmParamAlternativeAliasAddress,i++,sizeof(cmAlias),(char *)&alias);
                    alias.length = sizeof(number);
                }

                if (i == 0)
                {
                    if (cmCallGetParam(hsSameConferenceCall,cmParamDestCallSignalAddress,0,&val,(char*)&qAddress)>=0)
                        cmCallSetParam(hsCall,cmParamAlternativeAddress,0,sizeof(cmTransportAddress),(char*)&qAddress);
                    else
                    {
                        cmiAPIExit((hApp, "cmCallJoin=-1"));
                        return RV_ERROR_UNKNOWN;
                    }
                }
            }
            else
            {
                memset(&qAddress, 0, sizeof(qAddress));
                if (cmCallGetMcTransportAddress(hsSameConferenceCall,&qAddress)<0)
                {
                    cmiAPIExit((hApp, "cmCallJoin=-1"));
                    return RV_ERROR_UNKNOWN;
                }
                /* if we are not using an MC, just tell the other side to join this call,
                slightly non-standard, but not that much */
                if (cmTaHasIp(&qAddress) && (qAddress.port != 0))
                {
                    /* we are using an MC */
                    cmCallSetParam(hsCall,cmParamAlternativeAddress,0, sizeof(qAddress),(char*)&qAddress);
                }
            }

            break;

            case cmMSMaster:
            cmCallGetParam(hsSameConferenceCall,cmParamCallSignalAddress,0,&val,(char*)&qAddress);
            cmCallSetParam(hsCall,cmParamAlternativeAddress,0,sizeof(cmTransportAddress),(char*)&qAddress);
            break;

            case cmMSError:
            default:
            {
                cmiAPIExit((hApp, "cmCallJoin=-1"));
                return RV_ERROR_UNKNOWN;
            }
        }
        cmCallSetParam(hsCall,cmParamFacilityCID,0,0,CID);
        {
            cmCallSetParam(hsCall,cmParamFacilityReason,0,(RvInt32)cmReasonTypeRouteCallToMC,NULL);

            /* When sending this FACILITY message, we don't want the stack to set the CID of
               it to that of the call, since the whole point in sending this message is the
               new CID that we set */
            m_callset((callElem*)hsCall, overrideCID, RV_FALSE);
            status = cmCallFacility(hsCall,RV_ERROR_UNKNOWN);
            m_callset((callElem*)hsCall, overrideCID, RV_TRUE);
        }
    }
    cmiAPIExit((hApp, "cmCallJoin=%d", status));
    return status;
}

RVAPI
int RVCALLCONV cmCallInvite(
                          IN      HCALL               hsCall,
                          IN      HCALL               hsSameConferenceCall)
{
#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
    HAPP hApp=cmGetAHandle((HPROTOCOL)hsCall);
#endif
    char CID[16];
    cmiAPIEnter((hApp, "cmCallInvite(hsCall=%p,hsSameConferenceCall=%p)", hsCall, hsSameConferenceCall));
    cmCallGetParam(hsSameConferenceCall,cmParamCID,0,0,CID);
    if (cmCallGetOrigin(hsCall,NULL))
    {
        cmCallSetParam(hsCall,cmParamCID,0,0,CID);
        cmCallSetParam(hsCall,cmParamConferenceGoal,0,(RvInt32)cmInvite,0);
    }
    cmiAPIExit((hApp, "cmCallInvite=0"));
    return 0;
}


RVAPI
int RVCALLCONV cmCallIsSameConference(
                                    IN  HCALL       hsCall,
                                    IN  HCALL       hsAnotherCall
                                    )
{
    HAPP hApp=cmGetAHandle((HPROTOCOL)hsCall);
    char cid1[16],cid2[16];
    RvBool same;

    if (!hApp || !hsCall || !hsAnotherCall)
        return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallIsSameConference(hsCall=%p,hsAnotherCall=%p)", hsCall, hsAnotherCall));

    cmCallGetParam(hsCall,cmParamCID,0,NULL,cid1);
    cmCallGetParam(hsAnotherCall,cmParamCID,0,NULL,cid2);
    same=!memcmp(cid1,cid2,16);

    cmiAPIExit((hApp, "cmCallIsSameConference=0"));
    return same;
}

#ifdef __cplusplus
}
#endif
