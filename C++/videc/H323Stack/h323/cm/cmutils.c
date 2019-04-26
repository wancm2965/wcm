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
#include "prnutils.h"
#include "msg.h"
#include "h32xLog.h"
#include "pvaltreeStackApi.h"
#include "cm.h"
#include "cmintr.h"
#include "cmutils.h"
#include "q931asn1.h"
#include "h245.h"
#include "hash.h"
#include "cmCall.h"
#include "cmH245.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TMP_LOG H32xLogGet()

RvStatus addExtFastConnectGenericData(
    IN  HSTRANSSESSION   hsTransSession,
    IN  RvPvtNodeId      messageRoot,
    IN  RvInt32          parameter);
RvStatus getExtFastConnectGenericData(
    IN  HSTRANSSESSION   hsTransSession,
    IN  RvPvtNodeId      messageRoot,
    OUT RvInt32         *parameter);

static RvUint32      timesTlsInitialized = 0;
static RvUint32      rvEncoderTlsIndex;

typedef struct
{
    RvUint32    bufferSize;
    RvUint8*    buffer;
} cmThreadCoderLocalStorage;


int setNonStandard(
        IN  HPVT  hVal,
        IN  int   nodeId,
        IN  cmNonStandard*nonStandard
)
{
    int tmp,ret;
    tmp=nonStandard->t35CountryCode;
    if ((ret=pvtBuildByPath(hVal,nodeId,"t35CountryCode",tmp,NULL))<0)
        return ret;
    tmp=nonStandard->t35Extension;
    if ((ret=pvtBuildByPath(hVal,nodeId,"t35Extension",tmp,NULL))<0)
        return ret;
    tmp=nonStandard->manufacturerCode;
    if ((ret=pvtBuildByPath(hVal,nodeId,"manufacturerCode",tmp,NULL))<0)
        return ret;
    return 0;
}


int getNonStandard(
        IN  HPVT  hVal,
        IN  int   nodeId,
        OUT cmNonStandard*nonStandard
)
{
    RvInt32 tmp;
    pvtGetByPath(hVal,nodeId,"t35CountryCode",NULL,&tmp,NULL);
    nonStandard->t35CountryCode=(RvUint8)tmp;
    pvtGetByPath(hVal,nodeId,"t35Extension",NULL,&tmp,NULL);
    nonStandard->t35Extension=(RvUint8)tmp;
    pvtGetByPath(hVal,nodeId,"manufacturerCode",NULL,&tmp,NULL);
    nonStandard->manufacturerCode=(RvUint16)tmp;
    return 0;
}

int setNonStandardParam(
        IN  HPVT  hVal,
        IN  int   vNodeId,
        IN  cmNonStandardParam*nonStandard
)
{
    if (!nonStandard)
        return RV_ERROR_UNKNOWN;
    return cmNonStandardParameterCreate(hVal,vNodeId,
                 &nonStandard->info,
                 nonStandard->data,
                 nonStandard->length);
}


int getNonStandardParam(
        IN  HPVT                hVal,
        IN  RvPvtNodeId         vNodeId,
        OUT cmNonStandardParam  *nonStandard
)
{
    return cmNonStandardParameterGet(hVal,vNodeId,
                    &nonStandard->info,
                    nonStandard->data,
                    &nonStandard->length);
}


RvInt32 getEnumNameId(
          IN    fieldNames* enumFieldNames,
          IN    int     enumFieldNamesSize,
          IN    unsigned    enumValue)
{
    return ((enumValue < (unsigned)enumFieldNamesSize/sizeof(fieldNames))?
        enumFieldNames[enumValue].nameId:LAST_TOKEN);
}

int getEnumValueByNameId(
            IN  fieldNames* enumFieldNames,
            IN  RvInt32     nameId)
{
    int i=0;
    while(enumFieldNames[i].nameId != LAST_TOKEN)
        if (enumFieldNames[i++].nameId==nameId) return i-1;
    return RV_ERROR_UNKNOWN;
}



int aliasToVt(
          IN  HPVT    hVal,
          IN  cmAlias*alias,
          IN  int     nodeId)
{
    /* We can't use _q931() macros here since this function can be used for H450 ASN.1, which
       has different values for those fieldIDs. */
    int ret = 0;

    switch(alias->type)
    {
    case   cmAliasTypeE164          :
    {   int iLen = alias->length;
        ret=pvtBuildByPath(hVal,nodeId,"e164",iLen,alias->string);
    }
    break;
    case   cmAliasTypeH323ID        :
    {   int iLen = alias->length;
        ret=pvtBuildByPath(hVal,nodeId,"h323-ID",iLen,alias->string);
    }
    break;
    case   cmAliasTypeEndpointID        :
    case   cmAliasTypeGatekeeperID      :
    {
        int iLen = alias->length;
        pvtSet(hVal, nodeId, -1, iLen, alias->string);
    }
    break;
    case   cmAliasTypeURLID         :
    {
        int iLen = alias->length;
        ret = pvtBuildByPath(hVal, nodeId, "url-ID", iLen, alias->string);
    }
    break;
    case   cmAliasTypeEMailID       :
    {
        int iLen = alias->length;
        ret = pvtBuildByPath(hVal, nodeId, "email-ID", iLen, alias->string);
    }
    break;
    case   cmAliasTypeTransportAddress  :
    {
        ret = pvtBuildByPath(hVal, nodeId, "transportID", 0, NULL);
        if (ret >= 0)
            ret = cmTAToVt(hVal, ret, &(alias->transport));
    }
    break;
    case   cmAliasTypePartyNumber       :
    {
        int iLen;
        const RvChar *t=NULL,*v=NULL;
        int newNode=pvtBuildByPath(hVal,nodeId,"partyNumber",0,NULL);
        if (newNode<0)
        return newNode;
        switch (alias->pnType)
        {
        case cmPartyNumberPublicUnknown         :
            t="publicNumber.publicTypeOfNumber.unknown";
            v="publicNumber.publicNumberDigits";
        break;
        case cmPartyNumberPublicInternationalNumber :
            t="publicNumber.publicTypeOfNumber.internationalNumber";
            v="publicNumber.publicNumberDigits";
        break;
        case cmPartyNumberPublicNationalNumber      :
            t="publicNumber.publicTypeOfNumber.nationalNumber";
            v="publicNumber.publicNumberDigits";
        break;
        case cmPartyNumberPublicNetworkSpecificNumber   :
            t="publicNumber.publicTypeOfNumber.networkSpecificNumber";
            v="publicNumber.publicNumberDigits";
        break;
        case cmPartyNumberPublicSubscriberNumber    :
            t="publicNumber.publicTypeOfNumber.subscriberNumber";
            v="publicNumber.publicNumberDigits";
        break;
        case cmPartyNumberPublicAbbreviatedNumber   :
            t="publicNumber.publicTypeOfNumber.abbreviatedNumber";
            v="publicNumber.publicNumberDigits";
        break;
        case cmPartyNumberDataPartyNumber       :
            t=NULL;
            v="dataPartyNumber";
        break;
        case cmPartyNumberTelexPartyNumber      :
            t=NULL;
            v="telexPartyNumber";
        break;
        case cmPartyNumberPrivateUnknown        :
            t="privateNumber.privateTypeOfNumber.unknown";
            v="privateNumber.privateNumberDigits";
        break;
        case cmPartyNumberPrivateLevel2RegionalNumber   :
            t="privateNumber.privateTypeOfNumber.level2RegionalNumber";
            v="privateNumber.privateNumberDigits";
        break;
        case cmPartyNumberPrivateLevel1RegionalNumber   :
            t="privateNumber.privateTypeOfNumber.level1RegionalNumber";
            v="privateNumber.privateNumberDigits";
        break;
        case cmPartyNumberPrivatePISNSpecificNumber :
            t="privateNumber.privateTypeOfNumber.pISNSpecificNumber";
            v="privateNumber.privateNumberDigits";
        break;
        case cmPartyNumberPrivateLocalNumber        :
            t="privateNumber.privateTypeOfNumber.localNumber";
            v="privateNumber.privateNumberDigits";
        break;
        case cmPartyNumberPrivateAbbreviatedNumber  :
            t="privateNumber.privateTypeOfNumber.abbreviatedNumber";
            v="privateNumber.privateNumberDigits";
        break;
        case cmPartyNumberNationalStandardPartyNumber   :
            t=NULL;
            v="nationalStandardPartyNumber";
        break;
        }

        if (t)
        {
            ret = pvtBuildByPath(hVal,newNode,t,0,NULL);
            if (ret<0)
                return ret;
        }
        iLen = alias->length;
        ret = pvtBuildByPath(hVal,newNode,v,iLen,alias->string);
    }
    break;
    case cmAliasTypeLast: break; /* Here to remove a compilation warning */
    }

    return ret;
}



#define cmNTPublicNumber 0
#define cmNTDataPartyNumber 1
#define cmNTTelexPartyNumber 2
#define cmNTPrivateNumber 3
#define cmNTNationalStandardPartyNumber 4



RvStatus vtToAlias(
    IN  HPVT        hVal,
    IN  cmAlias     *alias,
    IN  RvPvtNodeId nodeId,
    IN  RvBool      bCheckLength)
{
    /* We can't use _q931() macros here since this function can be used for H450 ASN.1, which
       has different values for those fieldIDs. */
    RvPvtNodeId chNodeId;
    RvPvtNodeId tmpNodeId;
    chNodeId = pvtChild(hVal, nodeId);

    alias->type = (cmAliasType)(pvtGetSyntaxIndex(hVal, chNodeId)+(int)cmAliasTypeE164-1);

    /* Range-checking */
    if (alias->type < cmAliasTypeE164)
        return RV_ERROR_OUTOFRANGE;
    if (alias->type >= cmAliasTypeEndpointID)
        alias->type = (cmAliasType)(2 + (int)alias->type);
    if (alias->type >= cmAliasTypeLast)
        return RV_ERROR_OUTOFRANGE;

    if (alias->type == cmAliasTypeTransportAddress)
    {
        alias->length = 0;
        return cmVtToTA(hVal, chNodeId, &(alias->transport));
    }

    if (alias->type == cmAliasTypePartyNumber)
    {
        chNodeId = pvtChild(hVal, chNodeId);
        switch (pvtGetSyntaxIndex(hVal, chNodeId)-1)
        {
        case cmNTPublicNumber       :
            {
                /* ASN.1:
                    PublicPartyNumber ::= SEQUENCE
                    {
                        publicTypeOfNumber  PublicTypeOfNumber,
                        publicNumberDigits  NumberDigits
                    }
                */
                tmpNodeId = pvtChild(hVal, chNodeId); /* "publicTypeOfNumber" */
                chNodeId = pvtBrother(hVal, tmpNodeId); /* "publicNumberDigits" */
                tmpNodeId = pvtChild(hVal, tmpNodeId); /* "publicTypeOfNumber.*" */
                alias->pnType = (cmPartyNumberType)(pvtGetSyntaxIndex(hVal, tmpNodeId)+(int)cmPartyNumberPublicUnknown-1);
            }
            break;
        case cmNTDataPartyNumber        :
            alias->pnType = cmPartyNumberDataPartyNumber;
            break;
        case cmNTTelexPartyNumber       :
            alias->pnType = cmPartyNumberTelexPartyNumber;
            break;
        case cmNTPrivateNumber      :
            {
                /* ASN.1:
                    PrivatePartyNumber ::= SEQUENCE
                    {
                        privateTypeOfNumber PrivateTypeOfNumber,
                        privateNumberDigits NumberDigits
                    }
                */
                tmpNodeId = pvtChild(hVal, chNodeId); /* "privateTypeOfNumber" */
                chNodeId = pvtBrother(hVal, tmpNodeId); /* privateNumberDigits */
                tmpNodeId = pvtChild(hVal, tmpNodeId); /* "privateTypeOfNumber.*" */
                alias->pnType = (cmPartyNumberType)(pvtGetSyntaxIndex(hVal, tmpNodeId)+(int)cmPartyNumberPrivateUnknown-1);
            }
            break;
        case cmNTNationalStandardPartyNumber:
            alias->pnType = cmPartyNumberNationalStandardPartyNumber;
            break;
        default:
            return RV_ERROR_OUTOFRANGE;
        }
    }

    if (!RV_PVT_NODEID_IS_VALID(chNodeId))
        return chNodeId;

    {
        RvBool  isString;
        RvInt32 length;
        RvInt32 copyLength;
        pvtGet(hVal, chNodeId, NULL, NULL, &length, &isString);

        /* Check if we need to look after the length of the string as well... */
        if (!bCheckLength)
        {
            if ((alias->type == cmAliasTypeE164) || (alias->type == cmAliasTypePartyNumber))
                copyLength = length + 1; /* +1 since we want a NULL termination as well */
            else
                copyLength = length;
        }
        else
            copyLength = RvMin((RvUint16)(length + 1), alias->length);

        /* We always place in the resulting alias->length parameter the actual length of
           the string. The application can check from this value if the actual length is
           longer than what it expected. */
        alias->length = (RvUint16)length;
        if (isString)
            pvtGetString(hVal, chNodeId, copyLength, alias->string);
    }

    return RV_OK;
}

static RvPvtNodeId checkFeatures(
       IN HAPP          hApp,
       IN RvPvtNodeId   messageRoot,
       IN RvInt32       featureIdValue,
       IN RvPstFieldId  featureFieldId,
       IN RvInt16*      path);

static RvPvtNodeId addFeature(
       IN HAPP          hApp,
       IN HPVT          hVal,
       IN RvPvtNodeId   messageRoot,
       IN RvInt32       featureIdValue,
       IN RvPstFieldId  featureFieldId,
       IN RvInt16*      path);


RVAPI int RVCALLCONV cmAlias2Vt(IN  HPVT    hVal,
                                IN  cmAlias*alias,
                                IN  int     nodeId)
{
    return aliasToVt(hVal,alias,nodeId);
}


/************************************************************************
 * cmVt2Alias
 * purpose: Converts a PVT subtree of AliasAddress type into an Alias
 * input  : hVal    - The PVT handle
 *          nodeId  - PVT subtree of AliasAddress type we want to convert
 * output : alias   - Alias struct converted. The application is responsible
 *                    to supply the string field inside the cmAlias struct
 *                    with enough allocated size.
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV cmVt2Alias(
    IN  HPVT       hVal,
    OUT cmAlias*   alias,
    IN  int        nodeId)
{
    return vtToAlias(hVal, alias, nodeId, RV_FALSE);
}


RvStatus cmVtToLTA(HPVT hVal,int nodeId, cmTransportAddress* ta)
{
    RvPvtNodeId         addressNode;
    RvStatus            res;

    addressNode = pvtGetChildByFieldId(hVal, nodeId, __q931(address), NULL, NULL);
    res = cmVtToTA(hVal, addressNode, ta);
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    if ((res== RV_OK) && (ta->type == cmTransportTypeIPv6))
    {
        RvInt32 scId;
        if (RV_PVT_NODEID_IS_VALID(pvtGetChildByFieldId(hVal, nodeId, __q931(scopeId), &scId, NULL)))
        {
            ta->addr.v6.scopeId = (RvInt16)scId;
            return res;
        }
        return RV_ERROR_UNKNOWN;
    }
#endif
    return res;
}


RvStatus cmVtToTA(HPVT hVal,int nodeId, cmTransportAddress* ta)
{
    /* We can't use _q931() macros here since this function can be used for H450 ASN.1, which
       has different values for those fieldIDs. */
    RvUint32 p;
    int vNodeId, addrNodeId;
    int len;
    static const cmTransportType asn2type[]={
        cmTransportTypeIP,
        cmTransportTypeIPStrictRoute,
        (cmTransportType)-1,
        cmTransportTypeIPv6,
        (cmTransportType)-1,
        cmTransportTypeNSAP,
        (cmTransportType)-1};

    memset(ta,0,sizeof(cmTransportAddress));

    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_ERROR_UNKNOWN;

    addrNodeId = pvtChild(hVal, nodeId);
    if (!RV_PVT_NODEID_IS_VALID(addrNodeId))
        return RV_ERROR_UNKNOWN;

    ta->type = asn2type[pvtGetSyntaxIndex(hVal,addrNodeId)-1];
    switch (ta->type)
    {
        case cmTransportTypeIP:
            /* ASN.1:
                ipAddress   SEQUENCE
                {
                    ip          OCTET STRING (SIZE(4)),
                    port            INTEGER(0..65535)
                },
            */
            vNodeId = pvtChild(hVal, addrNodeId); /* ip */
            if (pvtGet(hVal, vNodeId, NULL, NULL, &len, NULL) < 0)
                return RV_ERROR_UNKNOWN;
            if (len != sizeof(RvUint32))
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
                ta->addr.v4.ip = 0;
#else
                ta->ip = 0;
#endif
            else
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
                pvtGetString(hVal, vNodeId, sizeof(RvUint32), (char*)&(ta->addr.v4.ip));
#else
                pvtGetString(hVal, vNodeId, sizeof(RvUint32), (char*)&(ta->ip));
#endif

            vNodeId = pvtBrother(hVal, vNodeId); /* port */
            if (pvtGet(hVal, vNodeId, NULL, NULL, (RvInt32*)&p, NULL) < 0)
                return RV_ERROR_UNKNOWN;
            ta->port = (RvUint16)p;
            break;

#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
        case cmTransportTypeIPv6:
            /* ASN.1:
                ip6Address      SEQUENCE
                {
                    ip              OCTET STRING (SIZE(16)),
                    port            INTEGER(0..65535),
                    ...
                },
            */
            vNodeId = pvtChild(hVal, addrNodeId); /* ip */
            if (pvtGet(hVal, vNodeId, NULL, NULL, &len, NULL) < 0)
                return RV_ERROR_UNKNOWN;
            if (len != sizeof(ta->addr.v6.ip))
                memset(ta->addr.v6.ip, 0, sizeof(ta->addr.v6.ip));
            else
                pvtGetString(hVal, vNodeId, sizeof(ta->addr.v6.ip), (char*)(ta->addr.v6.ip));

            vNodeId = pvtBrother(hVal, vNodeId); /* port */
            if (pvtGet(hVal, vNodeId, NULL, NULL, (RvInt32*)&p, NULL) < 0)
                return RV_ERROR_UNKNOWN;
            ta->port = (RvUint16)p;
            break;
#endif

        default:
            return RV_ERROR_UNKNOWN;
    }
    return RV_OK;
}

int cmLTAToVt(HPVT hVal,int nodeId, cmTransportAddress* ta)
{
    RvPvtNodeId         addressNode;
    int                 res;

    addressNode = pvtAdd(hVal, nodeId, __q931(address), 0, NULL, NULL);
    res = cmTAToVt(hVal, addressNode, ta);
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    if (RV_PVT_NODEID_IS_VALID(res) && (ta->type == cmTransportTypeIPv6))
    {
        res = pvtAdd(hVal, nodeId, __q931(scopeId), ta->addr.v6.scopeId, NULL, NULL);
    }
#endif
    return res;
}


int cmTAToVt(HPVT hVal,int nodeId, cmTransportAddress* ta)
{
    /* We can't use _q931() macros here since this function can be used for H450 ASN.1, which
       has different values for those fieldIDs. */
    int addrNodeId = RV_PVT_INVALID_NODEID;
    int iPort;

#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    if (ta->type == cmTransportTypeIP)
    {
        addrNodeId = pvtBuildByPath(hVal, nodeId, "ipAddress", 0, NULL);
        pvtBuildByPath(hVal, addrNodeId, "ip", sizeof(RvUint32), (ta->addr.v4.ip)?((char*)&(ta->addr.v4.ip)):(char*)"\0\0\0");
    }
    if (ta->type == cmTransportTypeIPv6)
    {
        addrNodeId = pvtBuildByPath(hVal, nodeId, "ip6Address", 0, NULL);
        pvtBuildByPath(hVal, addrNodeId, "ip", sizeof(ta->addr.v6.ip), (char*)(ta->addr.v6.ip));
    }
#else
    addrNodeId = pvtBuildByPath(hVal, nodeId, "ipAddress", 0, NULL);
    pvtBuildByPath(hVal, addrNodeId, "ip", sizeof(RvUint32), (ta->ip)?((char*)&(ta->ip)):(char*)"\0\0\0");
#endif
    iPort = ta->port;
    return pvtBuildByPath(hVal, addrNodeId, "port", iPort, NULL);
}


/******************************************************************************
 * cmTransportAddress2Vt
 * ----------------------------------------------------------------------------
 * General: Converts a cmTransportAddress struct to a PVT node Id.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hVal     - The PVT handle.
 *         address  - Transport address to convert.
 *         nodeId   - The node Id to build the converted transport address in.
 * Output: None.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmTransportAddress2Vt(
    IN  HPVT                hVal,
    IN  cmTransportAddress* address,
    IN  RvPvtNodeId         nodeId)
{
    return cmTAToVt(hVal, nodeId, address);
}


/******************************************************************************
 * cmVt2TransportAddress
 * ----------------------------------------------------------------------------
 * General: Converts a PVT node Id of type TransportAddress to a
 *          cmTransportAddress struct.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hVal     - The PVT handle.
 *         nodeId   - The node Id of the transport address to convert (of type
 *                    TransportAddress).
 * Output: address  - Resulting transport address struct.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmVt2TransportAddress(
    IN  HPVT                hVal,
    IN  RvPvtNodeId         nodeId,
    OUT cmTransportAddress* address)
{
    RvPvtNodeId addressNode;
    RvPstFieldId fieldId;

    /*find out what type of transportAddress we are dealing with*/
    __pvtGetByFieldIds(addressNode, hVal, nodeId, {_anyField LAST_TOKEN}, &fieldId, NULL, NULL);
    
    if ((fieldId == __h245(unicastAddress)) || (fieldId == __h245(multicastAddress)))
        return cmVtToTA_H245(hVal, nodeId, address);
    return cmVtToTA(hVal, nodeId, address);
}


/**************************************************************************************
 * cmAddExtFastConnectGenericData
 *
 * Purpose: Adds extended fast connect generic data nodes to a message
 *
 * Input:   hsCall      - The call handle
 *          msgRoot     - The root node of the message
 *          parameter   - the parameter to add: 0 - none, an acceptance message
 *                                              1 - a proposal
 *                                              2 - close all channels
 * Output:  none
 * Returns: RV_OK on success, other on failure
 **************************************************************************************/
RvStatus cmAddExtFastConnectGenericData(HCALL hsCall, RvPvtNodeId msgRoot, RvInt32 parameter)
{
    callElem *call = (callElem *)hsCall;
    return addExtFastConnectGenericData(call->hsTransSession, msgRoot, parameter);
}


/**************************************************************************************
 * cmGetExtFastConnectGenericData
 *
 * Purpose: Gets the extended fast connect generic data from a message
 *
 * Input:   hsCall      - The call handle
 *          msgRoot     - The root node of the message
 * Output:  parameter   - the parameter: 0 - none, an acceptance message
 *                                       1 - a proposal
 *                                       2 - close all channels
 * Returns: RV_OK on success, other on failure
 **************************************************************************************/
RVAPI RvStatus RVCALLCONV cmGetExtFastConnectGenericData(HCALL hsCall, RvPvtNodeId msgRoot, RvInt32 *parameter)
{
    callElem *call = (callElem *)hsCall;
    return getExtFastConnectGenericData(call->hsTransSession, msgRoot, parameter);
}


/**************************************************************************************
 * RvH323CoreToCmAddress
 *
 * Purpose: Convert a core address into a CM address.
 *
 * Input:   coreAddress - Core address to convert
 * Output:  cmAddress   - CM address to create
 * Returns: RV_OK on success, other on failure
 **************************************************************************************/
RvStatus RvH323CoreToCmAddress(
    IN  RvAddress*          coreAddress,
    OUT cmTransportAddress* cmAddress)
{
    RvStatus res = RV_ERROR_UNKNOWN;

    switch (RvAddressGetType(coreAddress))
    {
    case RV_ADDRESS_TYPE_IPV4:
        {
            const RvAddressIpv4* ipv4;
            cmAddress->type = cmTransportTypeIP;

            ipv4 = RvAddressGetIpv4(coreAddress);
            if (ipv4 != NULL)
            {
                if (RvAddressIsMulticastIp(coreAddress))
                    cmAddress->distribution = cmDistributionMulticast;
                else
                    cmAddress->distribution = cmDistributionUnicast;

#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
                cmAddress->addr.v4.ip = RvAddressIpv4GetIp(ipv4);
#else
                cmAddress->ip = RvAddressIpv4GetIp(ipv4);
#endif
                cmAddress->port = RvAddressGetIpPort(coreAddress);
                res = RV_OK;
            }
            break;
        }
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    case RV_ADDRESS_TYPE_IPV6:
        {
            const RvAddressIpv6* ipv6;
            cmAddress->type = cmTransportTypeIPv6;

            ipv6 = RvAddressGetIpv6(coreAddress);
            if (ipv6 != NULL)
            {
                if (RvAddressIsMulticastIp(coreAddress))
                    cmAddress->distribution = cmDistributionMulticast;
                else
                    cmAddress->distribution = cmDistributionUnicast;
                memcpy(cmAddress->addr.v6.ip, RvAddressIpv6GetIp(ipv6), sizeof(cmAddress->addr.v6.ip));
                cmAddress->port = RvAddressGetIpPort(coreAddress);
                cmAddress->addr.v6.scopeId = (short)RvAddressGetIpv6Scope(coreAddress);
                res = RV_OK;
            }
            break;
        }
#endif /* RV_H323_TRANSPORT_ADDRESS_ANY */
    default:
        break;
    }

    if (res != RV_OK)
    {
        /* Make sure we trash the CM address */
        cmAddress->type = (cmTransportType)0xffff;
    }

    return res;
}


/**************************************************************************************
 * RvH323CmToCoreAddress
 *
 * Purpose: Convert a CM address into a core address.
 *
 * Input:   cmAddress   - CM address to create
 * Output:  coreAddress - Core address to convert. This address must be destructed
 *                        by the caller to this function
 * Returns: RV_OK on success, other on failure
 **************************************************************************************/
RvStatus RvH323CmToCoreAddress(
    IN  cmTransportAddress* cmAddress,
    OUT RvAddress*          coreAddress)
{
    RvStatus res = RV_ERROR_UNKNOWN;

    switch (cmAddress->type)
    {
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    case cmTransportTypeIPv6:
        {
            if (RvAddressConstructIpv6(coreAddress, cmAddress->addr.v6.ip, cmAddress->port,
                cmAddress->addr.v6.scopeId) != NULL)
            {
                res = RV_OK;
            }
        }
        break;
#endif

    case cmTransportTypeIP:
    default:
        {
            /* Any address type is regarded as IPv4 because old applications never tried to
               fill in the type parameter of a cmTransportAddress. */
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
            if (RvAddressConstructIpv4(coreAddress, cmAddress->addr.v4.ip, cmAddress->port) != NULL)
#else
            if (RvAddressConstructIpv4(coreAddress, cmAddress->ip, cmAddress->port) != NULL)
#endif
            {
                res = RV_OK;
            }
        }
        break;
    }

    return res;
}

/**************************************************************************************
 * getIP
 *
 * Purpose: Extract an IP string from a CM address.
 *
 * Input:   cmAddress   - CM address to create
 *          buff        - buffer for the address
 * Output:  none
 * Returns: String of the IP on success, empty string on failure. string is static and
 *          is not to be used accept for printing.
 **************************************************************************************/
char * getIP(IN cmTransportAddress* cmAddress, OUT RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE])
{
    RvAddress cAddr;

    buff[0] = '\0';
    if (cmAddress == NULL)
    {
        strcpy(buff, "0x0");
        return buff;
    }

    RvH323CmToCoreAddress(cmAddress, &cAddr);

    switch (RvAddressGetType(&cAddr))
    {
    case RV_ADDRESS_TYPE_IPV4:
        RvAddressIpv4ToString(buff, RV_TRANSPORT_ADDRESS_STRINGSIZE, RvAddressIpv4GetIp(RvAddressGetIpv4(&cAddr)));
        break;

#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    case RV_ADDRESS_TYPE_IPV6:
        RvAddressIpv6ToString(buff, RV_TRANSPORT_ADDRESS_STRINGSIZE, RvAddressIpv6GetIp(RvAddressGetIpv6(&cAddr)));
        break;
#endif /* (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY) */

    default:
        strcpy(buff, "?Unknown?");
        break;
    }

    RvAddressDestruct(&cAddr);
    return buff;
}


/**************************************************************************************
 * getIPLength
 *
 * Purpose: GetThe length needed for the IP address.
 *
 * Input:   cmAddress   - CM address to create
 * Output:  none
 * Returns: Length needed for the IP address.
 **************************************************************************************/
int getIPLength(IN cmTransportAddress* cmAddress)
{
    if (cmAddress->type == cmTransportTypeIP)
    {
        return 25;
    }
    if (cmAddress->type == cmTransportTypeIPv6)
    {
        return 55;
    }
    return 0x0FFFFFFF;
}


/**************************************************************************************
 * condenseIpTo32bit
 *
 * Purpose: Turn an IP to a 32bit key for use in hashing.
 *
 * Input:   cmAddress   - CM address containing the IP
 * Output:  none
 * Returns: 32bits to be used for hashing
 **************************************************************************************/
RvUint32 condenseIpTo32bit(IN cmTransportAddress* cmAddress)
{
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    switch (cmAddress->type)
    {
    case cmTransportTypeIP:
        return cmAddress->addr.v4.ip;
    case cmTransportTypeIPv6:
        return hashDefaultFunc(cmAddress->addr.v6.ip, (RvUint32)sizeof(cmAddress->addr.v6.ip), 0);
    default:
        return 0;
    }
#else
    return cmAddress->ip;
#endif
}


/************************************************************************
 * addrIntoHashKey
 * purpose: Calculate a hash value for an address. This function can
 *          be used as part of a hash function, but not as the hash
 *          function itself.
 * input  : addr    - Address to hash
 *          hashKey - Starting hash value
 * output : none
 * return : Calculated hash value
 ************************************************************************/
RvUint32 addrIntoHashKey(
    IN cmTransportAddress*  addr,
    IN RvUint32             hashKey)
{
    /* We hash each address type differently */
    switch (addr->type)
    {
        case cmTransportTypeIPStrictRoute:
        case cmTransportTypeIPLooseRoute:
        case cmTransportTypeNSAP:
            /* No hashing for these guys - they're not really supported... */
            break;

        case cmTransportTypeIPv6:
        case cmTransportTypeIP:
        default:
            hashKey = (hashKey << 1) ^ condenseIpTo32bit(addr);
            hashKey = (hashKey << 1) ^ addr->port;
            break;
    }
    return hashKey;
}


/**************************************************************************************
 * isSameAddress
 *
 * Purpose: returns true if the addresses are the same.
 *
 * Input:   addr1   - CM address containing the first address
 *          addr2   - CM address containing the second address
 * Output:  none
 * Returns: true if equal, false otherwise.
 **************************************************************************************/
RvBool isSameAddress(IN cmTransportAddress* addr1, IN cmTransportAddress* addr2)
{
    if (addr1->type != addr2->type)
        return RV_FALSE;
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    if (addr1->type == cmTransportTypeIP)
    {
        return ((addr1->addr.v4.ip == addr2->addr.v4.ip) && (addr1->port == addr2->port));
    }
    if (addr1->type == cmTransportTypeIPv6)
    {
        return ((memcmp(addr1->addr.v6.ip, addr2->addr.v6.ip, sizeof(addr2->addr.v6.ip)) == 0) &&
            (addr1->port == addr2->port));
    }
    return RV_FALSE;
#else
    return ((addr1->ip == addr2->ip) && (addr1->port == addr2->port));
#endif
}


/**************************************************************************************
 * cmTaHasIp
 *
 * Purpose: Verify that a transport address has a non-zero IP.
 *
 * Input:   cmAddress   - CM address to create
 * Output:  none
 * Returns: RV_TRUE if address has nonzero IP, RV_FALSE if it does not.
 **************************************************************************************/
RvBool cmTaHasIp(IN cmTransportAddress* ta)
{
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


/**************************************************************************************
 * RvH323CmPrintMessage
 *
 * Purpose: Print an incoming or an outgoing message to the logfile.
 *
 * Input:   logSource           - Log source to print into
 *          messageStr          - String to use before printing the message
 *          hVal                - Value tree to use
 *          messageNodeId       - Message root node to print. Shout be set to -1 if decoding
 *                                of the buffer failed for incoming messages.
 *          messageBuffer       - Message buffer to print
 *          messageBufferSize   - Size of the message buffer. Should be set to -1 if encoding
 *                                of the nodeId failed for outgoing messages.
 *          isIncoming          - RV_TRUE if the message is incoming, RV_FALSE for outgoing
 * Output:  None
 * Returns: None
 **************************************************************************************/
#if (RV_LOGMASK & (RV_LOGLEVEL_DEBUG | RV_LOGLEVEL_ERROR))
void RvH323CmPrintMessage(
    IN RvLogSource*     logSource,
    IN const RvChar*    messageStr,
    IN HPVT             hVal,
    IN RvPvtNodeId      messageNodeId,
    IN RvUint8*         messageBuffer,
    IN int              messageBufferSize,
    IN RvBool           isIncoming)
{
    int debugLevel;
    RvBool printAll;

    debugLevel = msGetDebugLevel();

    /* Make sure debug level is high enough */
    printAll = ((debugLevel > 1) && RvLogIsSelected(logSource, RV_LOGLEVEL_DEBUG));

    if (isIncoming)
    {
        /* Incoming */
        if (printAll)
        {
            if (messageStr != NULL)
                RvLogTextDebug(logSource, messageStr);
            printHexBuff(messageBuffer, messageBufferSize, logSource);
        }

        if (messageNodeId >= 0)
        {
            if (printAll)
            {
                RvLogTextDebug(logSource, "Message:");
                pvtPrintInternal(hVal, messageNodeId, logSource, debugLevel);
            }
        }
        else
        {
            RvLogError(logSource, (logSource, "Decoding problems (%d)", messageNodeId));
        }
    }
    else
    {
        /* Outgoing */
        if (printAll)
        {
            if (messageStr != NULL)
                RvLogTextDebug(logSource, messageStr);
            pvtPrintInternal(hVal , messageNodeId, logSource, debugLevel);
        }

        if (messageBufferSize > 0)
        {
            if (printAll)
            {
                RvLogTextDebug(logSource, "Binary:");
                printHexBuff(messageBuffer, messageBufferSize, logSource);
            }
        }
        if (messageBufferSize < 0)
        {
            RvLogError(logSource, (logSource, "Encoding problems (nodeId=%d)", messageNodeId));
        }
    }
}
#endif  /* (RV_LOGMASK & (RV_LOGLEVEL_DEBUG | RV_LOGLEVEL_ERROR)) */


/********************************************************************************************
 * freeThreadBuffer
 * An exit callback of a specific thread encode/decode buffer.
 * This function frees the allocation of the encode/decode buffer
 * context  - The pointer to the buffer to free
 ********************************************************************************************/
static void freeThreadBuffer(IN RvThread* thread, IN void * context, IN RvUint32 index)
{
    cmThreadCoderLocalStorage* cmTls = (cmThreadCoderLocalStorage *)context;
    RV_UNUSED_ARG(thread);
    RV_UNUSED_ARG(index);

    if (cmTls != NULL)
    {
        if (cmTls->buffer != NULL)
        {
            RvMemoryFree(cmTls->buffer, TMP_LOG);
            cmTls->buffer = NULL;
        }
        RvMemoryFree(cmTls, TMP_LOG);
        timesTlsInitialized--;
    }

    if (timesTlsInitialized == 0)
    {
        RvThreadDeleteVar(rvEncoderTlsIndex, TMP_LOG);
    }
}


/**************************************************************************************
 * encodeDecodeBufferInit
 *
 * Purpose: To initialize the buffer for coding purposes.
 *
 * Input:   none
 *
 * Output:  none
 *
 **************************************************************************************/
void encodeDecodeBufferInit(void)
{
    if (timesTlsInitialized == 0)
    {
        if (RvThreadCreateVar(freeThreadBuffer, "RvEdmBuffer", TMP_LOG, &rvEncoderTlsIndex) != RV_OK)
            return;
    }
    timesTlsInitialized++;
}


/**************************************************************************************
 * getEncodeDecodeBuffer
 *
 * Purpose: To get the buffer for coding purposes.
 *
 * Input:   bufferSize   - The size of buffer required.
 *          buffer       - A pointer to the buffer.
 *
 *
 * Output:  buffer       - A pointer to the buffer .
 *
 **************************************************************************************/
void getEncodeDecodeBuffer(IN int bufferSize, OUT RvUint8 **buffer)
{
    RvThread* curThread;
    cmThreadCoderLocalStorage*  cmTls;

    if (buffer)
        *buffer = NULL;

    /* First, make sure we have a thread object for this thread. */
    curThread = RvThreadCurrent();
    if (curThread == NULL)
    {
        static int appThreadNum = 0;
        RvChar threadName[32];
        RvStatus status;

        /* Seems like we need to create one on our own. */
        status = RvMemoryAlloc(NULL, sizeof(RvThread), TMP_LOG, (void**)&curThread);
        if (status != RV_OK)
            return;

        status = RvThreadConstructFromUserThread(TMP_LOG, curThread);
        if (status == RV_OK)
        {
            RvThreadSetAutoDelete(curThread, RV_TRUE);
            RvSprintf(threadName, "%d H323BUsr", appThreadNum);
            status = RvThreadSetName(curThread, threadName);
            appThreadNum++;
        }

        if (status != RV_OK)
        {
            RvThreadDestruct(curThread);
            RvMemoryFree(curThread, TMP_LOG);
            return;
        }
    }

    if (bufferSize < 0)
        return;

    if ((timesTlsInitialized <= 0) ||
        (RvThreadGetVar(rvEncoderTlsIndex, TMP_LOG, (void**)&cmTls) != RV_OK))
    {
		//OutputDebugString("getEncodeDecodeBuffer cmTls 1 \n");
        return;
    }

    if (cmTls == NULL)
    {
        RvMemoryAlloc(NULL, sizeof(cmThreadCoderLocalStorage), TMP_LOG, (void**)&cmTls);
        memset((void*)cmTls, 0, sizeof(cmThreadCoderLocalStorage));
        RvThreadSetVar(rvEncoderTlsIndex, (void*)cmTls, TMP_LOG);
        timesTlsInitialized++;
		//OutputDebugString("getEncodeDecodeBuffer cmTls == NULL \n");
    }

    if ((cmTls->buffer != NULL) && ((int)cmTls->bufferSize < bufferSize))
    {
        /* Current allocation too small - make sure we fix this situation... */
        RvMemoryFree(cmTls->buffer, TMP_LOG);
        cmTls->buffer = NULL;
    }

    if (cmTls->buffer == NULL)
    {
        cmTls->bufferSize = (RvUint32)bufferSize;
        if (RvMemoryAlloc(NULL, cmTls->bufferSize, TMP_LOG, (void**)&cmTls->buffer) != RV_OK)
            return;
    }

    if (buffer)
        *buffer = cmTls->buffer;

	/* make the buffer clean */
	memset(*buffer, 0, bufferSize);
}


/**************************************************************************************
 * encodeDecodeBufferEnd
 *
 * Purpose: close down the local storage.
 *
 * Input:   none.
 * Output:  none.
 *
 **************************************************************************************/
void encodeDecodeBufferEnd(void)
{
    cmThreadCoderLocalStorage *cmTls;

    timesTlsInitialized--;

    /* Calling the exit function of the variable while the thread is still active */
    if (RvThreadGetVar(rvEncoderTlsIndex, TMP_LOG, (void**)&cmTls) == RV_OK)
    {
        freeThreadBuffer(NULL, (void*)cmTls, rvEncoderTlsIndex);
        RvThreadSetVar(rvEncoderTlsIndex, NULL, TMP_LOG);
    }
}


/**************************************************************************************
 * cleanMessage
 *
 * Purpose: Deletes all tunneling elements (H.245, H.450, annex M, annex L) from the
 *          given message. This is to avoid resending tunneled messages when reusing
 *          messages from the cm Property tree.
 *
 * Input:   hPvt - handle to the PVT of the message
 *          message  - A PVT node to the message.
 *
 * Output:  None.
 *
 **************************************************************************************/
void cleanMessage(IN HPVT hPvt, IN int message)
{
    int messageBodyNode;
    int node;

    /* position on the UU-IE part of the message */
    __pvtGetNodeIdByFieldIds(   messageBodyNode,
                                hPvt,
                                message,
                                {   _q931(message)
                                    _anyField
                                    _q931(userUser)
                                    _q931(h323_UserInformation)
                                    _q931(h323_uu_pdu)
                                    LAST_TOKEN
                                });
    if (messageBodyNode < 0)
        return;

    /* Clean the parallel tunneling element, if exists */
    __pvtGetNodeIdByFieldIds(node, hPvt, messageBodyNode,
        {_q931(h323_message_body) _q931(setup) _q931(parallelH245Control) LAST_TOKEN});
    if (node >= 0)
        pvtDelete(hPvt, node);

    /* Clean the H.245 tunneling element, if exists */
    if (pvtGetChild(hPvt, messageBodyNode, __q931(h245Control), &node) >= 0)
        pvtDelete(hPvt, node);

    /* Clean the H.450 element, if exists */
    if (pvtGetChild(hPvt, messageBodyNode, __q931(h4501SupplementaryService), &node) >= 0)
        pvtDelete(hPvt, node);

    /* Clean the Annex L element, if exists */
    if (pvtGetChild(hPvt, messageBodyNode, __q931(stimulusControl), &node) >= 0)
        pvtDelete(hPvt, node);

    /* Clean the annex M element, if exists */
    if (pvtGetChild(hPvt, messageBodyNode, __q931(tunnelledSignallingMessage), &node) >= 0)
        pvtDelete(hPvt, node);

}


/******************************************************************************
 * RvH245TimerStartEv
 * ----------------------------------------------------------------------------
 * General: Start a timer for the use of H.245.
 *
 * Return Value: Timer started on success
 *               NULL on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hCtrl        - Control object to start a timer for.
 *         eventHandler - Callback function on timeout.
 *         context      - Context for the callback function.
 *         millisec     - Length of the timer in milliseconds.
 * Output: none.
 *****************************************************************************/
RvTimer* RvH245TimerStartEv(
    IN HCONTROL     hCtrl,
    IN RvTimerFunc  eventHandler,
    IN void*        context,
    IN RvInt32      millisec)
{
    return RvH323TimerStart(
        ((cmElem *)emaGetInstance((EMAElement)cmiGetByControl(hCtrl)))->hTimers,
        eventHandler,
        context,
        millisec);
}


/******************************************************************************
 * RvH245TimerCancelEv
 * ----------------------------------------------------------------------------
 * General: Cancel a timer that wasn't timed-out yet.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hCtrl        - Control object that is associated with the timer.
 *         timer        - Pointer to the timer to cancel.
 * Output: timer        - NULL timer pointer for easy handling.
 *****************************************************************************/
RvStatus RvH245TimerCancelEv(
    IN    HCONTROL  hCtrl,
    INOUT RvTimer   **timer)
{
    return RvH323TimerCancel(
        ((cmElem *)emaGetInstance((EMAElement)cmiGetByControl(hCtrl)))->hTimers,
        timer);
}


/******************************************************************************
 * RvH245TimerClearEv
 * ----------------------------------------------------------------------------
 * General: Clear a timer that has timed-out.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hCtrl        - Control object that is associated with the timer.
 *         timer        - Pointer to the timer to clear.
 * Output: timer        - NULL timer pointer for easy handling.
 *****************************************************************************/
RvStatus RvH245TimerClearEv(
    IN    HCONTROL  hCtrl,
    INOUT RvTimer   **timer)
{
    return RvH323TimerClear(
        ((cmElem *)emaGetInstance((EMAElement)cmiGetByControl(hCtrl)))->hTimers,
        timer);
}


/******************************************************************************
 * RvH245RandomNumberEv
 * ----------------------------------------------------------------------------
 * General: A request of random number from the application.
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hH323            - The stack handle.
 *         hH323Call        - The call handle in the stack.
 *         seed             - The randomGenerator to use.
 * Output: value            - The random number determined by the application.
 *****************************************************************************/
void RvH245RandomNumberEv(
    IN HAPP                 hH323,
    IN HCALL                hH323Call,
    IN RvRandomGenerator    *seed,
    OUT RvRandom*           value)
{
    RV_UNUSED_ARG(hH323);
    RV_UNUSED_ARG(hH323Call);

    RvRandomGeneratorGetValue(seed, value);
}

/******************************************************************************
 * cmAddFeature
 * ----------------------------------------------------------------------------
 * General: Adds a standard feature to the supportedFeatures sequence (in the featureSet 
 *          of a H225 message or to supportedFeatures sequence in a setup message).
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp              - The stack handle.
 *         messageRoot        - The root nodeId of the h225 message.
 *         featureIdValue     - The feature's GenericIdentifier standard value.
 *         bAddSupported      - If RV_TRUE will be added to the supportedFeatures.
 *         bAddNeeded         - If RV_TRUE will be added to the neededFeatures.
 *         bAddDesired        - If RV_TRUE will be added to the desiredFeatures.
 * Output: supportedNode      - supportedFeatures added sequence nodeId.
 *         neededNode         - neededFeatures added sequence nodeId.
 *         desiredNode        - desiredFeatures added sequence nodeId.
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmAddFeature(
    IN  HAPP          hApp,
    IN  RvPvtNodeId   messageRoot,
    IN  RvInt32       featureIdValue,
    IN  RvBool        bAddSupported,
    IN  RvBool        bAddNeeded,
    IN  RvBool        bAddDesired,
    OUT RvPvtNodeId * supportedNode,
    OUT RvPvtNodeId * neededNode,
    OUT RvPvtNodeId * desiredNode)
{
    cmElem     *app = (cmElem *)hApp;
    RvInt16    supportedPath[] = { _anyField _q931(featureSet) _q931(supportedFeatures) LAST_TOKEN};
    RvInt16    neededPath[] = { _anyField _q931(featureSet) _q931(neededFeatures) LAST_TOKEN};
    RvInt16    desiredPath[] = { _anyField _q931(featureSet) _q931(desiredFeatures) LAST_TOKEN};
    HPVT       hVal;
    RvStatus status = RV_OK;

    if (app == NULL)
        return RV_ERROR_UNINITIALIZED;

    hVal = app->hVal;

    if (bAddSupported == RV_TRUE)
    {
        *supportedNode = addFeature(hApp, hVal, messageRoot, featureIdValue, __q931(supportedFeatures), supportedPath);
        if (!RV_PVT_NODEID_IS_VALID(*supportedNode))
            return RV_ERROR_UNKNOWN;
        
    }

    if (bAddNeeded)
    {
        *neededNode = addFeature(hApp, hVal, messageRoot, featureIdValue, __q931(neededFeatures), neededPath);
        if (!RV_PVT_NODEID_IS_VALID(*neededNode))
            return RV_ERROR_UNKNOWN;
    }

    if (bAddDesired)
    {
        *desiredNode = addFeature(hApp, hVal, messageRoot, featureIdValue, __q931(desiredFeatures), desiredPath);
        if (!RV_PVT_NODEID_IS_VALID(*desiredNode))
            return RV_ERROR_UNKNOWN;
    }

    return status;
}

/******************************************************************************
 * cmAddFeatureParameter
 * ----------------------------------------------------------------------------
 * General: Adds a feature parameter.
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp               - The stack handle.
 *         featureNode        - The nodeId of the FeatureDescriptor.
 *         paramIdValue       - The feature's parameter Identifier standard value.
 * Output: None.
 *****************************************************************************/
RVAPI 
RvStatus RVCALLCONV cmAddFeatureParameter(
    IN HAPP          hApp,
    IN RvPvtNodeId   featureNode,
    IN RvInt32       paramIdValue)
{
     RvPvtNodeId parameterNodeId, tmpNodeId;
     cmElem     *app = (cmElem *)hApp;
     HPVT       hVal;
     
     if (app == NULL)
        return RV_ERROR_UNINITIALIZED;

    hVal = app->hVal;

 
    /*create parameters node*/
    __pvtBuildByFieldIds(parameterNodeId, hVal, featureNode, { _q931(parameters) LAST_TOKEN}, 0, NULL);

    if (!RV_PVT_NODEID_IS_VALID(parameterNodeId))
        return RV_ERROR_UNKNOWN;

    /*add sequence*/
    parameterNodeId = pvtAdd(hVal, parameterNodeId, __nul(0), 0, NULL, NULL);

    if (!RV_PVT_NODEID_IS_VALID(parameterNodeId))
        return RV_ERROR_UNKNOWN;

    /*set standard id*/
    __pvtBuildByFieldIds(tmpNodeId, hVal, parameterNodeId, { _q931(id) _q931(standard) LAST_TOKEN }, paramIdValue, NULL);

    if (!RV_PVT_NODEID_IS_VALID(tmpNodeId))
        return RV_ERROR_UNKNOWN;

    return RV_OK;

}

/******************************************************************************
 * cmIsFoundFeature
 * ----------------------------------------------------------------------------
 * General: Checks if a specific standard feature is set in the supported/desired/needed 
 *          sequence in a message.
 * Return Value: RV_TRUE if set.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp              - The stack handle.
 *         messageRoot        - The root nodeId of the h225 message.
 *         featureIdValue     - The feature's GenericIdentifier standard value.
 * Output: bSupported         - RV_TRUE if found under supportedFeatures sequence.
 *         bNeeded            - RV_TRUE if found under neededFeatures sequence.
 *         bDesired           - RV_TRUE if found under desiredFeatures sequence.
 *         supportedNode      - supportedFeatures sequence nodeId.
 *         neededNode         - neededFeatures sequence nodeId.
 *         desiredNode        - desiredFeatures sequence nodeId.
 *****************************************************************************/
RVAPI
void RVCALLCONV cmIsFoundFeature(
    IN  HAPP            hApp,
    IN  RvPvtNodeId     messageRoot,
    IN  RvInt32         featureIdValue,
    OUT RvBool *        bSupported,
    OUT RvBool *        bNeeded,
    OUT RvBool *        bDesired,
    OUT RvPvtNodeId *   supportedNode,
    OUT RvPvtNodeId *   neededNode,
    OUT RvPvtNodeId *   desiredNode)
{
    
    RvInt16         supportedPath[] = { _anyField _q931(featureSet) _q931(supportedFeatures) LAST_TOKEN };
    RvInt16         neededPath[] = { _anyField _q931(featureSet) _q931(neededFeatures) LAST_TOKEN };
    RvInt16         desiredPath[] = { _anyField _q931(featureSet) _q931(desiredFeatures) LAST_TOKEN };

    *bSupported = RV_FALSE;
    *bNeeded = RV_FALSE;
    *bDesired = RV_FALSE;

    *supportedNode = checkFeatures(hApp, messageRoot, featureIdValue, __q931(supportedFeatures), supportedPath);
    *neededNode  = checkFeatures(hApp, messageRoot, featureIdValue, __q931(neededFeatures), neededPath);
    *desiredNode = checkFeatures(hApp, messageRoot, featureIdValue, __q931(desiredFeatures), desiredPath);

    if (RV_PVT_NODEID_IS_VALID(*supportedNode))
        *bSupported = RV_TRUE;
     if (RV_PVT_NODEID_IS_VALID(*neededNode))
        *bNeeded = RV_TRUE;
     if (RV_PVT_NODEID_IS_VALID(*desiredNode))
         *bDesired = RV_TRUE;
}

/******************************************************************************
 * cmIsFoundFeatureParameter
 * ----------------------------------------------------------------------------
 * General: Checks if a parameter exist under a feature.
 * Return Value: RV_TRUE if found.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp               - The stack handle.
 *         featureNode        - The nodeId of the FeatureDescriptor.
 *         paramIdValue       - The feature's parameter Identifier standard value.
 * Output: None.
 *****************************************************************************/
RVAPI 
RvBool RVCALLCONV cmIsFoundFeatureParameter(
    IN HAPP          hApp,
    IN RvPvtNodeId   featureNode,
    IN RvInt32       paramIdValue)
{
    RvPvtNodeId parameterNodeId;
    cmElem     *app = (cmElem *)hApp;
    HPVT       hVal;
    RvPvtNodeId fieldId;
     
     if (app == NULL)
        return RV_FALSE;

    hVal = app->hVal;

    parameterNodeId = pvtGetChild(hVal, featureNode, __q931(parameters), NULL); 

    if (!RV_PVT_NODEID_IS_VALID(parameterNodeId))
        return RV_FALSE;

    parameterNodeId = pvtChild(hVal, parameterNodeId);

    while(RV_PVT_NODEID_IS_VALID(parameterNodeId))
    {
        RvPvtNodeId paramIdNodeId;
        RvInt32     paramId;

        __pvtGetByFieldIds(paramIdNodeId, hVal, parameterNodeId, { _q931(id) _q931(standard) LAST_TOKEN }, &fieldId, &paramId, NULL);
        
        if (paramId == paramIdValue)
            return RV_TRUE;

        parameterNodeId = pvtBrother(hVal, parameterNodeId);
    }

    return RV_FALSE;
}


/******************************************************************************
 * checkFeatures  
 * ----------------------------------------------------------------------------
 * General: Checks if a specific standard feature is set under a specific path
 * Return Value: Node Id of the FeatureDescriptor.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp              - The stack handle.
 *         messageRoot        - The root nodeId of the h225 message.
 *         featureIdValue     - The feature's GenericIdentifier standard value.
 *         featureFieldId     - The feature's fieldId.
 *         path               - The path to check under.
 * Output: none.
 *****************************************************************************/  
static RvPvtNodeId checkFeatures(
       IN HAPP          hApp,
       IN RvPvtNodeId   messageRoot,
       IN RvInt32       featureIdValue,
       IN RvPstFieldId  featureFieldId,
       IN RvInt16*      path)
{
    cmElem          *app = (cmElem *)hApp;
    RvBool          bIsRasMsg = (cmProtocolGetProtocol(hApp, messageRoot) == cmProtocolRAS);
    RvPvtNodeId     supportedFeaturesNodeId, featureDescNodeId, genericIdNodeId;
    RvPstFieldId    fieldId = RV_PVT_INVALID_NODEID;
    RvInt32         value;
    HPVT            hVal;

    if (app == NULL)
        return RV_PVT_INVALID_NODEID;

    hVal = app->hVal;

    if (bIsRasMsg == RV_FALSE)
    {
        /* Check the type of message we need to handle */
        __pvtGetByFieldIds(messageRoot, hVal, messageRoot,
        { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
          _q931(h323_uu_pdu) _q931(h323_message_body) _anyField LAST_TOKEN }, &fieldId, NULL, NULL);
    }

    if (fieldId == __q931(setup))
    {
        supportedFeaturesNodeId = pvtGetChild(hVal, messageRoot, featureFieldId, NULL);
    }
    else
    {
        if (bIsRasMsg == RV_FALSE)
            messageRoot = pvtParent(hVal, messageRoot);
        supportedFeaturesNodeId = pvtGetByFieldIds(hVal, messageRoot, path, NULL, 0, NULL);
    }

   
    if (supportedFeaturesNodeId == RV_PVT_INVALID_NODEID)
        return RV_PVT_INVALID_NODEID;

    featureDescNodeId = pvtChild(hVal, supportedFeaturesNodeId);
    while (featureDescNodeId != RV_PVT_INVALID_NODEID)
    {
        __pvtGetByFieldIds(genericIdNodeId, hVal, featureDescNodeId, {_q931(id) _q931(standard) LAST_TOKEN }, NULL, &value, NULL);
        if (value == featureIdValue)
            return featureDescNodeId;

        featureDescNodeId = pvtBrother(hVal, featureDescNodeId); 
    }

    return RV_PVT_INVALID_NODEID;
}

/******************************************************************************
 * addFeature     
 * ----------------------------------------------------------------------------
 * General: Adds a specific standard feature under a specific path
 * Return Value: Node id of added feature..
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp               - The stack handle.
 *         hVal               - Handle of value Tree.
 *         messageRoot        - The root nodeId of the h225 message.
 *         featureIdValue     - The feature's GenericIdentifier standard value.
 *         featureFieldId     - The feature's fieldId.
 *         path               - The path to add under.
 * Output: none.
 *****************************************************************************/  
static RvPvtNodeId addFeature(
       IN HAPP          hApp,
       IN HPVT          hVal,
       IN RvPvtNodeId   messageRoot,
       IN RvInt32       featureIdValue,
       IN RvPstFieldId  featureFieldId,
       IN RvInt16*      path)
{
    RvPvtNodeId  supportedFeaturesNodeId , featureDescNodeId = RV_PVT_INVALID_NODEID, genericIdNodeId;
    RvPstFieldId fieldId = RV_PVT_INVALID_NODEID;
    RvBool       bIsRasMsg = (cmProtocolGetProtocol(hApp, messageRoot) == cmProtocolRAS);

    if (bIsRasMsg == RV_FALSE)
    {
        /* Check the type of message we need to handle */
        __pvtGetByFieldIds(messageRoot, hVal, messageRoot,
        { _q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
          _q931(h323_uu_pdu) _q931(h323_message_body) _anyField LAST_TOKEN }, &fieldId, NULL, NULL);
    }

    if (fieldId == __q931(setup))
    {
        supportedFeaturesNodeId = pvtAdd(hVal, messageRoot, featureFieldId, 0, NULL, NULL);
    }
    else
    {
        if (bIsRasMsg == RV_FALSE)
            messageRoot = pvtParent(hVal, messageRoot);
        supportedFeaturesNodeId = pvtBuildByFieldIds(hVal, messageRoot, path, 0, NULL);
        pvtAdd(hVal, pvtParent(hVal, supportedFeaturesNodeId), __q931(replacementFeatureSet), RV_TRUE, NULL, NULL);
    }

    if (!RV_PVT_NODEID_IS_VALID(supportedFeaturesNodeId))
    {
        return featureDescNodeId;
    }

    featureDescNodeId = pvtAdd(hVal, supportedFeaturesNodeId, __nul(0), 0, NULL, NULL);
    if (!RV_PVT_NODEID_IS_VALID(featureDescNodeId))
    {
        return featureDescNodeId;
    }

    __pvtBuildByFieldIds(genericIdNodeId, hVal, featureDescNodeId,{ _q931(id) _q931(standard) LAST_TOKEN}, featureIdValue, NULL);
    if (!RV_PVT_NODEID_IS_VALID(genericIdNodeId))
    {
        return RV_PVT_INVALID_NODEID;
    }

    return featureDescNodeId;
}




#ifdef __cplusplus
}
#endif



