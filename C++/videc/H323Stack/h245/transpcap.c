/***********************************************************************
        Copyright (c) 2005 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

#include "pvaltree.h"
#include "cmH245GeneralDefs.h"
#include "cmictrl.h"
#include "h245.h"
#include "transpcap.h"
#include "cmH245Object.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (RV_H245_LEAN_H223 == RV_NO)


int createTranspCapability( IN   HH245            hApp,
                            IN  cmNonStandardIdentifier *identifier,
                            /* non standard param is optional may be equal NULL */
                                  IN  char *data,
                                  IN  int dataLength );/* in bytes */

int addQosCapability( IN     HH245            hApp,
                      IN int capTransport,
                      IN cmRSVPParameters *rsvpParam,
                      IN cmATMParameters * atmParam,
                      IN cmNonStandardParam *nonStandard);

int addMediaChannelCap( IN   HH245            hApp,
                        IN int capTransport,
                        IN  cmMediaChannelCapability * mediaCap
                       );




int buildQosCapability(IN HPVT hVal,
                       IN int qosId,
                       IN cmRSVPParameters *rsvpParam,
                       IN cmATMParameters * atmParam,
                       IN cmNonStandardParam *nonStandard)

{

    int nodeId=0,nonStandardId,rsvpId,atmId;

    if (nonStandard!=NULL)
    {
        nonStandardId=pvtAdd(hVal, qosId, __h245(nonStandard), 0, NULL,NULL);
        nodeId=cmNonStandardParameterCreate(hVal,nonStandardId,&nonStandard->info,nonStandard->data,nonStandard->length);
    }
    if (rsvpParam!=NULL)
    {
        rsvpId = pvtAdd(hVal, qosId, __h245(rsvpParameters), 0, NULL,NULL);
        switch (rsvpParam->cmQosMode)
        {
            case cmGuaranteedQOS:
                __pvtBuildByFieldIds(nodeId,hVal,rsvpId,
                                    {_h245(qosMode) _h245(guaranteedQOS) LAST_TOKEN},
                                    (RvInt32)rsvpParam->cmQosModeValue, NULL);
            break;
            case cmControlledLoad:
                __pvtBuildByFieldIds(nodeId,hVal,rsvpId,
                                    {_h245(qosMode) _h245(controlledLoad) LAST_TOKEN},
                                    (RvInt32)rsvpParam->cmQosModeValue, NULL);
            break;
            default: break;
        }
        if (rsvpParam->cmTokenRate)
            nodeId=pvtAdd(hVal,rsvpId,__h245(tokenRate),rsvpParam->cmTokenRate, NULL, NULL);
        if (rsvpParam->cmBucketSize)
            nodeId=pvtAdd(hVal,rsvpId,__h245(bucketSize),rsvpParam->cmBucketSize, NULL, NULL);
        if (rsvpParam->cmPeakRate)
            nodeId=pvtAdd(hVal,rsvpId,__h245(peakRate),rsvpParam->cmPeakRate, NULL, NULL);
        if (rsvpParam->cmMinPoliced)
            nodeId=pvtAdd(hVal,rsvpId,__h245(minPoliced),rsvpParam->cmMinPoliced, NULL, NULL);
        if (rsvpParam->cmMaxPktSize)
            nodeId=pvtAdd(hVal,rsvpId,__h245(maxPktSize),rsvpParam->cmMaxPktSize, NULL, NULL);

    }
    if (atmParam!=NULL)
    {
        int iMaxNTUSize = atmParam->cmMaxNTUSize;
        atmId = pvtAdd(hVal, qosId, __h245(atmParameters), 0, NULL, NULL);
        nodeId=pvtAdd(hVal,atmId,__h245(maxNTUSize),iMaxNTUSize, NULL, NULL);
        nodeId=pvtAdd(hVal,atmId,__h245(atmUBR), (RvInt32)atmParam->cmAtmUBR, NULL, NULL);
        nodeId=pvtAdd(hVal,atmId,__h245(atmrtVBR), (RvInt32)atmParam->cmAtmrtVBR, NULL, NULL);
        nodeId=pvtAdd(hVal,atmId,__h245(atmnrtVBR), (RvInt32)atmParam->cmAtmnrtVBR, NULL, NULL);
        nodeId=pvtAdd(hVal,atmId,__h245(atmABR), (RvInt32)atmParam->cmAtmABR, NULL, NULL);
        nodeId=pvtAdd(hVal,atmId,__h245(atmCBR), (RvInt32)atmParam->cmAtmCBR, NULL, NULL);
    }

    return nodeId;
}

int addQosCapability( IN HH245            hApp,
                      IN int capTransport,
                      IN cmRSVPParameters *rsvpParam,
                      IN cmATMParameters * atmParam,
                      IN cmNonStandardParam *nonStandard
                       /* in bytes */)
/*   all parameters are optional may be equal NULL */
{
    int nodeId,qosId=-1,ret=1;
    HPVT hVal;
    H245Object *app = (H245Object*)hApp;

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "addQosCapability: hApp=%p",hApp));
    hVal = app->hVal;

    nodeId = pvtGetChild(hVal,capTransport,__h245(qOSCapabilities), NULL);
    /* check if node is already exist*/
    if (nodeId<0)
        nodeId  = pvtAdd(hVal, capTransport, __h245(qOSCapabilities), 0, NULL,NULL);
    qosId = pvtAdd(hVal, nodeId, -800, 0, NULL, NULL);

    ret=buildQosCapability(hVal,qosId,rsvpParam,atmParam,nonStandard);

    H245APIExit((app->pAppObject, "addQosCapability=%d", ret));

    if(ret<0)
        return ret;
    return qosId;
}

int getQosParameters(IN HPVT hVal,
                     IN int qosElemId,
                     INOUT  cmQosCapability * cmQOSCapability)
{
    int nonStandardId ,nodeId,rsvpId,qosModeId,atmId;
    RvInt32 castValue;
    cmNonStandardParam *nonStandard;
    cmRSVPParameters *rsvpParameters;
    cmATMParameters  *atmParameters;

    nonStandard=&cmQOSCapability->cmNonStandard;
    rsvpParameters=&cmQOSCapability->cmRsvpParameters;
    atmParameters=&cmQOSCapability->cmAtmParameters;
    cmQOSCapability->cmQosCapabilityParamUsed = 0;

    if (qosElemId >= 0)
    {
        nonStandardId = pvtGetChild(hVal, qosElemId, __h245(nonStandardData), NULL);
        if (nonStandardId>=0)
        {
            cmNonStandardParameterGet(hVal,nonStandardId,&nonStandard->info,
                nonStandard->data,&(nonStandard->length));
            cmQOSCapability->cmQosCapabilityParamUsed |= cmQosCapabilityNonStandard;
        }

        rsvpId = pvtGetChild(hVal, qosElemId, __h245(rsvpParameters),NULL);
        if (rsvpId>=0)
        {
            nodeId=pvtGetChild(hVal, rsvpId, __h245(tokenRate), NULL);
            if (nodeId>=0)
                pvtGet(hVal,nodeId,NULL,NULL,&rsvpParameters->cmTokenRate,NULL);
            else
                rsvpParameters->cmTokenRate=0;
            nodeId=pvtGetChild(hVal, rsvpId, __h245(bucketSize), NULL);
            if (nodeId>=0)
                pvtGet(hVal,nodeId,NULL,NULL,&rsvpParameters->cmBucketSize,NULL);
            else
                rsvpParameters->cmBucketSize=0;

            nodeId=pvtGetChild(hVal, rsvpId, __h245(peakRate), NULL);
            if (nodeId>=0)
                pvtGet(hVal,nodeId,NULL,NULL,&rsvpParameters->cmPeakRate,NULL);
            else
                rsvpParameters->cmPeakRate=0;

            nodeId=pvtGetChild(hVal, rsvpId, __h245(minPoliced), NULL);
            if (nodeId>=0)
                pvtGet(hVal,nodeId,NULL,NULL,&rsvpParameters->cmMinPoliced,NULL);
            else
                rsvpParameters->cmMinPoliced=0;

            nodeId=pvtGetChild(hVal, rsvpId, __h245(maxPktSize), NULL);
            if (nodeId>=0)
                pvtGet(hVal,nodeId,NULL,NULL,&rsvpParameters->cmMaxPktSize,NULL);
            else
                rsvpParameters->cmMaxPktSize=0;

            nodeId=pvtGetChild(hVal, rsvpId, __h245(qosMode), NULL);
            if (nodeId>=0)
            {
                qosModeId = pvtGetChild(hVal, nodeId, __h245(guaranteedQOS), NULL);
                if (qosModeId>=0)
                {
                    rsvpParameters->cmQosMode=cmGuaranteedQOS;
                    pvtGet(hVal,qosModeId,NULL,NULL,(RvInt32*)&rsvpParameters->cmQosModeValue,NULL);
                }
                else
                {
                    qosModeId = pvtGetChild(hVal, nodeId, __h245(controlledLoad), NULL);
                    rsvpParameters->cmQosMode=cmControlledLoad;
                    pvtGet(hVal,qosModeId,NULL,NULL,(RvInt32*)&rsvpParameters->cmQosModeValue,NULL);
                }

            }
            else
                rsvpParameters->cmQosMode=cmNoneQos;

            cmQOSCapability->cmQosCapabilityParamUsed |= cmQosCapabilityRsvpParameters;

        }

        atmId = pvtGetChild(hVal, qosElemId, __h245(atmParameters), NULL);
        if (atmId>=0)
        {
            pvtGetChildByFieldId(hVal,atmId,__h245(maxNTUSize),&castValue ,NULL);
            atmParameters->cmMaxNTUSize=(RvUint16)castValue;
            pvtGetChildByFieldId(hVal,atmId,__h245(atmUBR),(RvInt32*)&atmParameters->cmAtmUBR,NULL);
            pvtGetChildByFieldId(hVal,atmId,__h245(atmrtVBR),(RvInt32*)&atmParameters->cmAtmrtVBR,NULL);
            pvtGetChildByFieldId(hVal,atmId,__h245(atmnrtVBR),(RvInt32*)&atmParameters->cmAtmnrtVBR,NULL);
            pvtGetChildByFieldId(hVal,atmId,__h245(atmABR),(RvInt32*)&atmParameters->cmAtmABR,NULL);
            pvtGetChildByFieldId(hVal,atmId,__h245(atmCBR),(RvInt32*)&atmParameters->cmAtmCBR,NULL);

            cmQOSCapability->cmQosCapabilityParamUsed |= cmQosCapabilityAtmParameters;
        }
    }

    return RV_TRUE;
}

RVAPI int RVCALLCONV
cmCreateTranspCapability( IN HAPP               hApp,
                          IN cmNonStandardParam *nonStandard)
                            /* non standard param is optional may be equal NULL */
{
    int        ret;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;
    H245APIEnter((app->pAppObject, "cmCreateTranspCapability: hApp=%p",hApp));

    if (nonStandard)
    {
        ret = createTranspCapability((HH245)app,&nonStandard->info,nonStandard->data,nonStandard->length);
        H245APIExit((app->pAppObject, "cmCreateTranspCapability: with nonStandard=%d", ret));
    }
    else
    {
        ret = createTranspCapability((HH245)app,NULL,NULL,0);
        H245APIExit((app->pAppObject, "cmCreateTranspCapability=%d", ret));
    }
    return ret;
}

RVAPI int RVCALLCONV
cmAddQosCapability( IN   HAPP            hApp,
                    IN   int             capTransport,
                    IN   cmQosCapability *qosCapability)

{
    cmRSVPParameters *rsvpParam=NULL;
    cmATMParameters * atmParam=NULL;
    cmNonStandardParam *nonStandard=NULL;
    int        result;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;
    H245APIEnter((app->pAppObject, "cmAddQosCapability: hApp=%p,TransportCapabilityId=%d",hApp,capTransport));

    if (qosCapability->cmQosCapabilityParamUsed & cmQosCapabilityNonStandard)
        nonStandard = &qosCapability->cmNonStandard;
    if (qosCapability->cmQosCapabilityParamUsed & cmQosCapabilityRsvpParameters)
        rsvpParam = &qosCapability->cmRsvpParameters;

    if (qosCapability->cmQosCapabilityParamUsed & cmQosCapabilityAtmParameters)
        atmParam = &qosCapability->cmAtmParameters;
    result =  addQosCapability( (HH245)app,capTransport,rsvpParam,atmParam, nonStandard);

    H245APIExit((app->pAppObject, "cmAddQosCapability=%d", result));
    return result;
}


RVAPI int RVCALLCONV
cmAddMediaChannelCap( IN HAPP            hApp,
                      IN int             capTransport,
                      IN cmMediaChannelCapability * mediaCap
                       )
{
    int        result;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;
    H245APIEnter((app->pAppObject, "cmAddMediaChannelCap: hApp=%p,TransportCapabilityId=%d",
        hApp, capTransport));

    result = addMediaChannelCap( (HH245)app,capTransport,mediaCap);

    H245APIExit((app->pAppObject, "cmAddMediaChannelCap=%d", result));
    return result;

}






/* ================================================== */


int createTranspCapability( IN   HH245            hApp,
                            IN  cmNonStandardIdentifier *identifier,
                            /* non standard param is optional may be equal NULL */
                            IN  char *data,
                            IN  int dataLength /* in bytes */
)
{
    int        nodeId,nonStandardId,ret=0;
    HPVT       hVal;
    H245Object *app=(H245Object*)hApp;

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "createTranspCapability: hApp=%p",hApp));

    hVal = app->hVal;

    nodeId = pvtAddRoot(hVal, app->h245TransCap, 0, NULL);
    if (nodeId<0)
    {
        H245APIExit((app->pAppObject, "createTranspCapability=%d", nodeId));
        return nodeId;
    }
    if (identifier!=NULL)
    {
        nonStandardId=pvtAdd(hVal, nodeId, __h245(nonStandard), 0, NULL, NULL);
        ret=cmNonStandardParameterCreate(hVal,nonStandardId,identifier,data,dataLength);


    }

    H245APIExit((app->pAppObject, "createTranspCapability=%d", ret));
    if (ret<0)
        return ret;
    return nodeId;
}



int addMediaChannelCap( IN HH245       hApp,
                        IN int capTransport,
                        IN cmMediaChannelCapability * mediaCap
                       )
{
    int        nodeId,mediaId=-1,ret=1;
    HPVT       hVal;
    H245Object *app=(H245Object*)hApp;

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "addMediaChannelCap: hApp=%p",hApp));

    hVal = app->hVal;

    nodeId = pvtGetChild(hVal,capTransport,__h245(mediaChannelCapabilities),NULL);
    /* check if node is already exist*/
    if (nodeId<0)
    nodeId  = pvtAdd(hVal, capTransport, __h245(mediaChannelCapabilities), 0, NULL, NULL);
    mediaId = pvtAdd(hVal, nodeId, -800, 0, NULL, NULL);
    mediaId = pvtAdd(hVal,mediaId,__h245(mediaTransport), 0 , NULL, NULL);

    switch(mediaCap->cmTransportType)
    {
    case cmIP_UDP:
        ret=pvtAdd(hVal, mediaId, __h245(ip_UDP), 0, NULL,NULL);
        break;
    case cmIP_TCP:
        ret=pvtAdd(hVal, mediaId, __h245(ip_TCP), 0, NULL, NULL);
        break;
    case cmAtm_AAL5_UNIDIR:
        ret=pvtAdd(hVal, mediaId, __h245(atm_AAL5_UNIDIR), 0, NULL, NULL);
        break;
    case cmAtm_AAL5_BIDIR:
        ret=pvtAdd(hVal, mediaId, __h245(atm_AAL5_BIDIR), 0, NULL, NULL);
        break;
    }

    H245APIExit((app->pAppObject, "addMediaChannelCap=%d", ret));
    if(ret<0)
        return ret;
    return mediaId;
}


RVAPI int RVCALLCONV
cmGetTransportCapabilities(             IN  HCALL       hsCall)
{
    H245Object *app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    HPVT       hVal;

    int capId=RV_ERROR_UNKNOWN,transpId=RV_ERROR_UNKNOWN;
    if (!hsCall) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmGetTransportCapabilities: hsCall=%p",hsCall));

    if (emaLock((EMAElement)hsCall))
    {
        hVal = app->hVal;

        capId = cmCallGetRemoteCapabilities(hsCall);/*terminalCapabilitySet */
        __pvtGetNodeIdByFieldIds(transpId, hVal,capId,
                               {_h245(multiplexCapability)
                                _h245(h2250Capability)
                                _h245(transportCapability)
                                LAST_TOKEN});
        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmGetTransportCapabilities: capId =[%d] transpId = [%d].", capId, transpId));
    return transpId;
}

RVAPI int RVCALLCONV
cmGetTransportCapNonStandard( IN HAPP                hApp,
                              IN int                 transpId,
                              OUT cmNonStandardParam *nonStandard   )



{
    HPVT       hVal;
    int        nonStandardId;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmGetTransportCapNonStandard: hApp=%p",hApp));

    hVal = app->hVal;

    nonStandardId = pvtGetChild(hVal,transpId,__h245(nonStandardData),NULL);
    if (nonStandardId >=0)
    cmNonStandardParameterGet(hVal,nonStandardId,&nonStandard->info,
        nonStandard->data,&(nonStandard->length));

    H245APIExit((app->pAppObject, "cmGetTransportCapNonStandard:  transpId = [%d],nonStandardId = [%d].",
        transpId,nonStandardId));
    return nonStandardId;
}






/******************************************************************************
* cmGetTransportCapQosParameters
* ----------------------------------------------------------------------------
* General: Gets the Qos parameters from the TransportCapability ASN.1 node id.
*
* Return Value: Returns the updated size of the cmQosCapability array.
* ----------------------------------------------------------------------------
* Arguments:
* Input:  hApp                 - Stack handle to use.
*         transpId             - The TransportCapability ASN.1 node id.
* Output: cmQOSCapability      - An array of QOSCapability structs, filled with the
*                                Qos parameters.
*         cmQOSCapabilitySize  - The size of the QosCapabilities array. This size
*                                will be updated to the actual amount of
*                                QOS capabilities.
*****************************************************************************/
RVAPI int RVCALLCONV
cmGetTransportCapQosParameters(IN    HAPP             hApp,
                               IN    int              transpId,
                               INOUT cmQosCapability * cmQOSCapability,
                               INOUT int  * cmQOSCapabilitySize)
{
    HPVT       hVal;
    int        qosId,ii,qosElemId,childs;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmGetTransportCapQosParameters: hApp=%p transpId [%d]",hApp,transpId));

    hVal = app->hVal;

    qosId = pvtGetChild(hVal, transpId, __h245(qOSCapabilities), NULL);

    if (qosId >=0)
    {
        childs = pvtNumChilds(hVal,qosId);
        if (childs< *cmQOSCapabilitySize)
            *cmQOSCapabilitySize = childs;
        if (cmQOSCapability==NULL)
        {
            H245APIExit((app->pAppObject, "cmGetTransportCapQosParameters:  transpId = [%d], childs [%d]", transpId,childs));
            return childs;
        }
        for (ii=1; ii<= *cmQOSCapabilitySize ;ii++)
        {
            pvtGetByIndex(hVal,qosId,ii,&qosElemId);
            if (qosElemId >= 0)
                 getQosParameters(hVal,qosElemId,&cmQOSCapability[ii-1]);
        }
    }
    else
        *cmQOSCapabilitySize = 0;

    H245APIExit((app->pAppObject, "cmGetTransportCapQosParameters:  transpId = [%d], cmQOSCapabilitySize [%d]", transpId, *cmQOSCapabilitySize));
    return (*cmQOSCapabilitySize);
}





/*
  mediaChannelCapability is array,allocated by user .Max size is 256
  mediaChannelCapabilitySize as IN param passes the size of allocated array
  as OUT param passes the result size ,according to the h245 message.
*/
RVAPI int RVCALLCONV
cmGetTransportCapMedia(
    IN    HAPP                          hApp,
    IN    int                           transpId,
    INOUT cmMediaChannelCapability *    mediaChannelCapability ,
    INOUT int *                         mediaChannelCapabilitySize)
{
    HPVT       hVal;
    int        mediaId,ii,mediaElemId,nodeId,mediaTranspId,childs;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmGetTransportCapMedia: hApp=%p transpId [%d]",hApp,transpId));

    hVal = app->hVal;

    mediaId = pvtGetChild(hVal, transpId, __h245(mediaChannelCapabilities), NULL);

    if (mediaId >=0)
    {
        childs = pvtNumChilds(hVal,mediaId);
        if (childs< *mediaChannelCapabilitySize)
            *mediaChannelCapabilitySize = childs;
        for (ii=1;ii<=*mediaChannelCapabilitySize;ii++)
        {
            pvtGetByIndex(hVal,mediaId,ii,&mediaElemId);
            mediaTranspId = pvtGetChild(hVal, mediaElemId, __h245(mediaTransport), NULL);
            if (mediaTranspId>=0)
            {
                nodeId = pvtGetChild(hVal, mediaTranspId, __h245(ip_UDP), NULL);
                if (nodeId>=0)
                    mediaChannelCapability[ii-1].cmTransportType = cmIP_UDP;
                nodeId = pvtGetChild(hVal, mediaTranspId, __h245(ip_TCP), NULL);
                if (nodeId>=0)
                    mediaChannelCapability[ii-1].cmTransportType = cmIP_TCP;
                nodeId = pvtGetChild(hVal, mediaTranspId, __h245(atm_AAL5_UNIDIR), NULL);
                if (nodeId>=0)
                    mediaChannelCapability[ii-1].cmTransportType = cmAtm_AAL5_UNIDIR;
                nodeId = pvtGetChild(hVal, mediaTranspId, __h245(atm_AAL5_BIDIR), NULL);
                if (nodeId>=0)
                    mediaChannelCapability[ii-1].cmTransportType = cmAtm_AAL5_BIDIR;
            }
        }
    }
    else
    *mediaChannelCapabilitySize=0;

    H245APIExit((app->pAppObject, "cmGetTransportCapMedia:  transpId = [%d], mediaChannelCapabilitySize [%d]", transpId,
        mediaChannelCapabilitySize));
    return (*mediaChannelCapabilitySize);
}


#endif /* (RV_H245_LEAN_H223 == RV_NO) */

#ifdef __cplusplus
}
#endif



