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
#include <stdlib.h>
#include "rvstdio.h"
#include "cm.h"
#include "stkutils.h"
#include "cmdebprn.h"
#include "q931asn1.h"
#include "cmAutoRasCall.h"
#include "cmCall.h"
#include "strutils.h"
#include "cmiFastStart.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 * stringToCoreAddress
 * purpose: Convert a string into a core address
 * input  : string  - String to convert in one of two formas:
 *                     IPv4: xxx.xxx.xxx.xxx:xxxx
 *                     IPv6: [HHHH:HHHH:HHHH::HHHH:HHHH]:xxxx%x
 * output : addr    - Address struct constructed from string
 * return : RV_OK on success, or negative value on failure
 ************************************************************************/
static RvStatus stringToCoreAddress(IN const RvChar* string, OUT RvAddress* addr)
{
    /* todo: fix the colon thing here */
    RvStatus res = RV_ERROR_UNKNOWN;

    if (string[0] != '[')
    {
        /* this appears to be an IPv4 address */
        RvChar* colonPos;
        RvAddressConstruct(RV_ADDRESS_TYPE_IPV4, addr);

        colonPos = (char*)strchr(string, ':');

        if (RvAddressSetString(string, addr) != NULL)
            res = RV_OK;

        if (colonPos != NULL)
        {
            if (res == RV_OK)
            {
                /* Handle port number as well... */
                RvAddressSetIpPort(addr, (RvUint16)atoi(colonPos+1));
            }
        }
    }
	else
    {
        /* this is an IPv6 address: [IPv6]:port%scopeId */
        RvChar* closeBracePos;
        RvChar* portPos;
        RvChar* scopeIdPos;
        RvChar addrString[64];
        RvChar portString[32];

        closeBracePos=(char*)strchr(string, ']');
        scopeIdPos = (char*)strchr(string, '%');

        if (closeBracePos == NULL)
		{
			return RV_ERROR_UNKNOWN;
		}

		RvAddressConstruct(RV_ADDRESS_TYPE_IPV6, addr);
        memset(addrString, 0, sizeof(addrString));
        memset(portString, 0, sizeof(portString));

        portPos = (char*)closeBracePos + 2;
        memcpy(addrString, string+1, (RvSize_t)(closeBracePos-string-1));

		if (scopeIdPos != NULL)
			memcpy(portString, portPos, (RvSize_t)(scopeIdPos - portPos));
		else
			strncpy(portString, portPos, sizeof(portString) - 1);

        if (RvAddressSetString(addrString, addr) != NULL)
        {
            /* Handle port number as well... */
            RvAddressSetIpPort(addr, (RvUint16)atoi(portString));
            /* also Scope Id ... */
			if (scopeIdPos != NULL)
				RvAddressSetIpv6Scope(addr, (RvUint16)atoi(scopeIdPos+1));

            res = RV_OK;
        }
    }

    return res;
}


/************************************************************************
 * cmString2TransportAddress
 * purpose: Converts a regular string to a cmTransportAddress struct
 * input  : string   - Regular string, in one of two formas:
 *                     IPv4: xxx.xxx.xxx.xxx:xxxx
 *                     IPv6: [HHHH:HHHH:HHHH::HHHH:HHHH]:xxxx%x
 * output : addr     - address matching (parsing) the given string
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV cmString2TransportAddress(
    IN  const RvChar*       string,
    OUT cmTransportAddress* addr)
{
    RvStatus res;
    RvAddress rvAddress;

    res = stringToCoreAddress(string, &rvAddress);
    if (res == RV_OK)
        res = RvH323CoreToCmAddress(&rvAddress, addr);
    RvAddressDestruct(&rvAddress);
    return res;
}


/************************************************************************
 * cmTransportAddress2String
 * purpose: Converts a cmTransportAddress struct to a regular string
 * input  : addr     - address to be turnes into a string
 *          len      - the length of buffer "string" in bytes
 * output : string   - string representation, in one of two formas:
 *                     IPv4: xxx.xxx.xxx.xxx:xxxx
 *                     IPv6: [HHHH:HHHH:...:HHHH:HHHH]:xxxx
 *                     string is expected to be of enough size to hold
 *                     a full IPv6 address (55 bytes should do it)
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV cmTransportAddress2String(
    IN  cmTransportAddress* addr,
    IN  RvSize_t            len,
    OUT RvChar*             string)
{
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];

    if (addr->type == cmTransportTypeIP)
    {
        /*first check if the size of string is big enough. IPv4 is 16 bytes
          and U can be up to 10 bytes */
        if (len < (16 + 6))
        {
            return RV_ERROR_OUTOFRESOURCES;
        }
        /* IPv4 address */
        RvSprintf(string, "%s:%u", getIP(addr, buff), addr->port);
        return RV_OK;
    }
    if (addr->type == cmTransportTypeIPv6)
    {
        /*first check if the size of string is big enough. IPv6 is 46 bytes
          and U can be up to 10 bytes*/
        if (len < (46 + 8))
        {
            return RV_ERROR_OUTOFRESOURCES;
        }
        /* IPv6 address */
        RvSprintf(string, "[%s]:%u", getIP(addr, buff), addr->port);
        return RV_OK;
    }
    return RV_ERROR_BADPARAM;
}


/************************************************************************************
 * cmCompareTransportAddresses
 *
 * Purpose: Compare two addresses according to their type.
 *
 * Input: addr1, addr2 - the cmTransportAddress to compare.
 *
 * Output: None.
 *
 * Returned Value: RV_TRUE - the addresses are identical; RV_FALSE - Otherwise
 **************************************************************************************/
RVAPI RvBool RVCALLCONV
cmCompareTransportAddresses(
    IN cmTransportAddress *addr1,
    IN cmTransportAddress *addr2)
{
    RvBool res = RV_FALSE;
    RvAddress rvAddr1, rvAddr2;
    RvH323CmToCoreAddress(addr1, &rvAddr1);
    RvH323CmToCoreAddress(addr2, &rvAddr2);

    res = RvAddressCompare(&rvAddr1, &rvAddr2, RV_ADDRESS_FULLADDRESS);

    RvAddressDestruct(&rvAddr1);
    RvAddressDestruct(&rvAddr2);

    return res;
}


/************************************************************************
 * insertAlias
 * purpose: Convert a string into an alias struct
 * input  : numPtr  - Pointer to the string to convert
 * output : alias   - Alias representation of the string
 *          bmpStr  - String value of the alias
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
static int insertAlias(IN char **numPtr, OUT cmAlias * alias, OUT char* bmpStr)
{
    RvChar* ptr;

    if (!strncmp("TEL:",*numPtr,4))
    {
        /* e164 alias type */
        *numPtr+=4;
        alias->string=*numPtr;
        alias->length=(RvUint16)strlen(alias->string);
        alias->type=cmAliasTypeE164;

    }
    else if (!strncmp("URL:",*numPtr,4))
    {
        /* URL */
        *numPtr+=4;
        alias->string=*numPtr;
        alias->length=(RvUint16)strlen(alias->string);
        alias->type=cmAliasTypeURLID;

    }
    else if (!strncmp("EMAIL:",*numPtr,6))
    {
        /* eMail */
        *numPtr+=6;
        alias->string=*numPtr;
        alias->length=(RvUint16)strlen(alias->string);
        alias->type=cmAliasTypeEMailID;

    }
    else if (!strncmp("PN:",*numPtr,3))
    {
        /* partyNumber */
        *numPtr+=3;
        alias->string=*numPtr;
        alias->type=cmAliasTypePartyNumber;
        alias->pnType=(cmPartyNumberType )cmPartyNumberPublicUnknown;
        if ((ptr=(RvChar*)strchr(*numPtr,'$'))!=NULL)
        {
            switch(ptr[1])
            {
            case 'P':
                switch(ptr[2])
                {
                case 'U':
                    switch(ptr[3])
                    {
                    case 'U':alias->pnType=cmPartyNumberPublicUnknown;break;
                    case 'I':alias->pnType=cmPartyNumberPublicInternationalNumber;break;
                    case 'N':
                        switch(ptr[4])
                        {
                        case   0:alias->pnType=cmPartyNumberPublicNationalNumber;break;
                        case 'S':alias->pnType=cmPartyNumberPublicNetworkSpecificNumber;break;
                        default: break;
                        }
                        break;
                    case 'S':alias->pnType=cmPartyNumberPublicSubscriberNumber;break;
                    case 'A':alias->pnType=cmPartyNumberPublicAbbreviatedNumber;break;
                    default: break;
                    }
                    break;
                case 'R':
                    switch(ptr[3])
                    {
                    case 'U':alias->pnType=cmPartyNumberPrivateUnknown;break;
                    case 'L':
                        switch(ptr[4])
                        {
                        case '2':alias->pnType=cmPartyNumberPrivateLevel2RegionalNumber;break;
                        case '1':alias->pnType=cmPartyNumberPrivateLevel1RegionalNumber;break;
                        case   0:alias->pnType=cmPartyNumberPrivateLocalNumber;break;
                        default: break;
                        }
                        break;
                    case 'P':alias->pnType=cmPartyNumberPrivatePISNSpecificNumber;break;
                    case 'A':alias->pnType=cmPartyNumberPrivateAbbreviatedNumber;break;
                    default: break;
                    }
                    break;
                default: break;
                }
                break;
            case 'D':alias->pnType=cmPartyNumberDataPartyNumber;break;
            case 'T':alias->pnType=cmPartyNumberTelexPartyNumber;break;
            case 'N':alias->pnType=cmPartyNumberNationalStandardPartyNumber;break;
            default: break;
            }
            if (((int)alias->pnType) < 0)
                alias->pnType = (cmPartyNumberType )atoi(ptr+1);
            *ptr=0;
        }
        alias->length=(RvUint16)strlen(alias->string);
    }
    else if (!strncmp("TNAME:",*numPtr,6))
    {
        /* transport address */
        RvAddress addr;

        *numPtr+=6;

        if (stringToCoreAddress(*numPtr, &addr) != RV_OK)
            return RV_ERROR_UNKNOWN;
        if (RvH323CoreToCmAddress(&addr, &alias->transport) != RV_OK)
            return RV_ERROR_UNKNOWN;

        alias->type = cmAliasTypeTransportAddress;
        alias->string = NULL;
        alias->length = 0;
    }
    else if (!strncmp("NAME:",*numPtr,5))
    {
        /* h323ID */
        *numPtr+=5;
        alias->type=cmAliasTypeH323ID;
        alias->length=(RvUint16)utlChr2Bmp(*numPtr, (RvUint8*)bmpStr);
        alias->string=bmpStr;
    }
    else return RV_ERROR_UNKNOWN;

    return 0;
}


/************************************************************************
 * cmAliasString2PVT
 * purpose: Converts an alias string to a PVT node
 * input  : hVal      - handle to the PVT
 *          string    - NULL terminated string of the form "TYPE:alias"
 * output : aliasNode - a valid PVT node on which to build the alias
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV cmAliasString2PVT(
    IN  HPVT                hVal,
    IN  const RvChar*       string,
    OUT RvPvtNodeId         aliasNode)
{
    RvChar bmpString[512];
    RvChar **pString = (RvChar**)&string;
    cmAlias alias;
    RvStatus ret;

    memset(bmpString, 0, sizeof(bmpString));
    memset(&alias, 0, sizeof(alias));

    if ((ret=insertAlias(pString, &alias, bmpString)) < 0)
        return ret;
    ret = aliasToVt(hVal, &alias, aliasNode);
    return ret;
}


/************************************************************************
 * cmCallMake
 * purpose: Starts a new call setup with the given parameters
 * input  : hsCall      - Stack handle for the new call
 *          maxRate     - Maximum rate allowed for the new call
 *          minRate     - Minimum rate allowed for the new call.
 *                        This parameter is not currently in use, so set it to zero (0).
 *          destAddress - Called party address list
 *          srcAddress  - Calling party address list
 *          display     - String representing display information for reporting
 *                        to the called party
 *          userUser    - String representing user-to-user information for reporting
 *                        to the called party
 *          userUserSize- Length of the string representing user-to-user information
 *                        to report to the called party
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallMake(
        IN  HCALL           hsCall,
        IN  RvUint32        maxRate,
        IN  RvUint32        minRate,
        IN  const RvChar*   destAddress,
        IN  const RvChar*   srcAddress,
        IN  const RvChar*   display,
        IN  const RvChar*   userUser,
        IN  int             userUserSize)
{
    HAPP hApp;
    RvStatus status;

    RV_UNUSED_ARG(minRate);

    hApp = emaGetInstance((EMAElement)hsCall);

    if (!hApp || !hsCall) return RV_ERROR_NULLPTR;

    cmiAPIEnter((hApp,"cmCallMake(hsCall=%p,maxRate=%ld,minRate=%ld,display=%.100s,userUser=%.100s,userUserSize=%d)",
        hsCall,maxRate,minRate,nprn(display),nprn(userUser),userUserSize));

    /* Set other parameters - rate, display, userUser */
    cmCallSetParam(hsCall, cmParamRequestedRate, 0, (RvInt32)maxRate, 0);

    status = cmCallSetAddressesFromStrings(hsCall, destAddress, srcAddress);

    if (status == RV_OK)
    {
        if (display != NULL)
            cmCallSetParam(hsCall, cmParamDisplay, 0, 0, display);
        if (userUser != NULL)
            cmCallSetParam(hsCall, cmParamUserUser, 0, userUserSize, userUser);

        /* Make sure we dial */
        status = cmCallDial(hsCall);
    }

    cmiAPIExit((hApp, "cmCallMake=%d", status));
    return status;
}


/******************************************************************************
 * cmCallSetAddressesFromStrings
 * ----------------------------------------------------------------------------
 * General: This function can be used to set the call object's dialing
 *          properties from string variables, in the same manner that
 *          cmCallMake() parses and sets these parameters.
 *
 * Return Value: If an error occurs, the function returns a negative value.
 *               If no error occurs, the function returns RV_OK.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall       - Call object to modify.
 *         destAddress  - Called party address list.
 *                        If this parameter's value is NULL, it is ignored.
 *         srcAddress   - Calling party address list.
 *                        If this parameter's value is NULL, it is ignored.
 * Output: None.
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmCallSetAddressesFromStrings(
        IN  HCALL           hsCall,
        IN  const RvChar*   destAddress,
        IN  const RvChar*   srcAddress)
{
    HAPP hApp;
    RvBool fta,eta,wasTA=RV_FALSE,wasETA=RV_FALSE,firstETA=RV_FALSE;
    RvChar bmpStr[512];
    RvChar parsingStr[512];
    RvChar* numPtr=NULL;
    RvChar* nextPtr=NULL;
    int id=0,i;
    RvBool namedID=RV_TRUE;
    RvChar delimiter;

    hApp = emaGetInstance((EMAElement)hsCall);

    if (!hApp || !hsCall)
        return RV_ERROR_NULLPTR;

    cmiAPIEnter((hApp,"cmCallSetAddressesFromStrings(hsCall=%p,destAddr=%.100s,srcAddr=%.100s)",
        hsCall,nprn(destAddress),nprn(srcAddress)));

    delimiter = cmGetDelimiter(hApp);

    /* Set the source addresses */
    if ((srcAddress != NULL) && srcAddress[0])
    {
        strncpy(parsingStr, srcAddress, sizeof(parsingStr));
        numPtr = parsingStr;

        while (numPtr != NULL)
        {
            int iDelimiter = delimiter;
            nextPtr = (RvChar*) strchr(numPtr,iDelimiter);
            {
                cmAlias alias;
                while (isspace((int)*numPtr))
                    numPtr++;
                if (nextPtr)
                {
                    *nextPtr = 0;
                    nextPtr++;
                }
                if (insertAlias(&numPtr, &alias, bmpStr) >= 0)
                {
                    cmCallSetParam(hsCall, cmParamSourceAddress, id++, 0, (char *)&alias);
                }
                else
                {
                    /* No specific type - take as e164 */
                    alias.string = numPtr;
                    alias.length = (RvUint16)strlen(alias.string);
                    alias.type = cmAliasTypeE164;

                    cmCallSetParam(hsCall, cmParamCallingPartyNumber, 0, 0, (char *)&alias);
                    break;
                }
            }
            numPtr = nextPtr;
        }
    }

    /* Set destination address parameters */
    if ((destAddress != NULL) && destAddress[0])
    {
        strncpy(parsingStr, destAddress, sizeof(parsingStr));
        numPtr = parsingStr;
        id = 0;

        while((numPtr != NULL) && namedID)
        {
            int iDelimiter = delimiter;
            namedID = RV_TRUE;
            fta = 0;
            nextPtr = (RvChar*)strchr(numPtr, iDelimiter);

            {
                cmAlias alias;
                while (isspace((int)*numPtr))
                    numPtr++;
                if (nextPtr)
                {
                    *nextPtr = 0;
                    nextPtr++;
                }
                if (insertAlias(&numPtr ,&alias, bmpStr) >= 0)
                {
                    cmCallSetParam(hsCall, cmParamDestinationAddress, id++, 0, (char *)&alias);
                }
                else if (!strncmp("SUB:", numPtr, 4))
                {
                    numPtr += 4;
                    alias.string = numPtr;
                    alias.length = (RvUint16)strlen(alias.string);
                    alias.type = cmAliasTypeE164;

                    cmCallSetParam(hsCall, cmParamCalledPartySubAddress, 0, 0, (char *)&alias);
                }
                else if (!strncmp("EXT:", numPtr, 4))
                {
                    numPtr += 4;
                    alias.string = numPtr;
                    alias.length = (RvUint16)strlen(alias.string);
                    alias.type = cmAliasTypeE164;

                    cmCallSetParam(hsCall, cmParamExtension, 0, 0, (char *)&alias);
                }
                else if (!strncmp("EXTID:", numPtr, 6))
                {
                    numPtr += 6;
                    alias.type = cmAliasTypeH323ID;
                    alias.length = (RvUint16)utlChr2Bmp(numPtr, (RvUint8*)bmpStr);
                    alias.string = bmpStr;

                    cmCallSetParam(hsCall, cmParamExtension, 0, 0, (char *)&alias);
                }
                else if (!strncmp("TA:",numPtr,3) || !strncmp("ETA:",numPtr,3) || !strncmp("FTA:",numPtr,3))
                {
                    cmTransportAddress qAddress;
                    RvAddress addr;
                    memset(&qAddress, 0, sizeof(qAddress));

                    fta=(strncmp("FTA:",numPtr,4)==0);
                    eta=(strncmp("ETA:",numPtr,4)==0);

                    numPtr+=3+(fta||eta);

                    if (stringToCoreAddress(numPtr, &addr) == RV_OK)
                    {
                        RvH323CoreToCmAddress(&addr, &qAddress);
                        RvAddressDestruct(&addr);

                        if (eta && !wasTA)      firstETA=RV_TRUE;
                        if (eta)                wasETA=RV_TRUE;
                        if (!eta && !fta)       wasTA=RV_TRUE;


                        if (!fta)
                        {

                            if (qAddress.port <= 0)
                            {
                                if (eta)
                                    qAddress.port = 2517;
                                else
                                    qAddress.port = 1720;
                            }

                            if (!eta)
                                cmCallSetParam((HCALL)hsCall,cmParamDestinationIpAddress,0, sizeof(cmTransportAddress),(char*)&qAddress);
                            else
                                cmCallSetParam((HCALL)hsCall,cmParamDestinationAnnexEAddress,0, sizeof(cmTransportAddress),(char*)&qAddress);
                        }
                        else
                        {
                            if (qAddress.port <= 0)
                                qAddress.port = 1720;
                        }

                        if (!eta)
                            cmCallSetParam((HCALL)hsCall,cmParamDestCallSignalAddress,0, sizeof(cmTransportAddress),(char*)&qAddress);
                    }
                }
                else
                {
                    namedID=RV_FALSE;
                }
            }

            if (namedID)
                numPtr=nextPtr;
        }

        /* Deal with extra call info (used for calls that need 2 numbers) */
        i=-1;
        while (numPtr)
        {
            cmAlias alias;

            alias.string=numPtr;
            numPtr=(RvChar*) strchr(numPtr,';');
            if (numPtr)
            {
                *numPtr=0;
                numPtr++;
            }
            alias.type=cmAliasTypeE164;
            alias.length=(RvUint16)strlen(alias.string);
            cmCallSetParam((HCALL)hsCall,(i<0)?cmParamCalledPartyNumber:cmParamDestExtraCallInfo,(i<0)?0:i,
                sizeof(cmAlias),(char*)&alias);
            i++;
        }
    }

    /*Set annexE type*/
    {
        cmAnnexEUsageMode annexE=cmTransNoAnnexE;
        if (wasTA && wasETA && firstETA)
            annexE=cmTransPreferedAnnexE;
        else if (wasTA && wasETA && !firstETA)
            annexE=cmTransPreferedTPKT;
        else if (wasETA && !wasTA)
            annexE=cmTransUseAnnexE;

        cmCallSetParam((HCALL)hsCall,cmParamAnnexE,0,(RvInt32)annexE,NULL);
    }

    cmiAPIExit((hApp,"cmCallSetAddressesFromStrings=0"));

    return RV_OK;
}



/*-----------------------------------------------------------------------------------------
 * alias2str
 *
 * convert from cmAlias to string with the alias (with the appropriate prefix of the alias)
 *
 * Input:  alias - alias struct.
 *         len   - length of str (how much space was allocated).
 * Output: str   - string with alias. must be allocated before calling the function.
 *
 * Return: the length of the string that was copied to str.
 *-----------------------------------------------------------------------------------------*/
static int alias2str(
    IN  cmAlias * alias,
    OUT char * str,
    IN  int len)
{
    switch(alias->type)
    {
    case(cmAliasTypeE164):
        {
            if (strncmp("EXT:",str,4))
            {
                if(len>(alias->length+4))
                {
                    RvSprintf(str,"TEL:%s",alias->string);
                    return 4+alias->length;
                }
            }
            else    /* if str begins with "EXT:", it is e164 because of EXT and not TEL, */
                /* then don't write the "TEL:", only the name and return only length */
            {
                if(len>alias->length)
                    RvSprintf(str,"%s",alias->string);
                return alias->length;
            }
            return 0;
        }
    case(cmAliasTypeH323ID):
        {
            char nameId[257];
            int res = utlBmp2Chr(nameId, (RvUint8* )alias->string, (int)alias->length);
            if ((res >= 0) && (len > ((alias->length/2)+5)))
            {
                RvSprintf(str,"NAME:%s",nameId);
                return 5+(alias->length/2);
            }
            return 0;
        }
    case(cmAliasTypeURLID):
        {
            if(len>(alias->length+4))
            {
                RvSprintf(str,"URL:%s",alias->string);
                return 4+alias->length;
            }
            return 0;
        }
    case(cmAliasTypeEMailID):
        {
            if(len>(alias->length+6))
            {
                RvSprintf(str,"EMAIL:%s",alias->string);
                return 6+alias->length;
            }
            return 0;
        }
    case(cmAliasTypeTransportAddress):
        {
            RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
            if (len>getIPLength(&alias->transport))
            {
                int length=RvSprintf(str,"TNAME:%s:%u",
                    getIP(&alias->transport, buff),
                    alias->transport.port);
                return length;
            }
            return 0;
        }
    case(cmAliasTypePartyNumber):
        {
            if(len>(alias->length+7))
            {
                static char pnType[15][6] = { "PUU", "PUI", "PUN", "PUNS", "PUS", "PUA", "D", "T", "PRU", "PRL2", "PRL1", "PRP", "PRL", "PRA", "N" };
                RvSprintf(str,"PN:%s$%s",alias->string,pnType[alias->pnType]);
                return 6+alias->length;
            }
            return 0;
        }
    case(cmAliasTypeEndpointID):
    case(cmAliasTypeGatekeeperID):
    case(cmAliasTypeLast):
        return RV_ERROR_UNKNOWN;
    }
    return 0;
}


/************************************************************************
 * cmAliasPVT2String
 * purpose: Converts a PVT node to an alias string
 * input  : hVal      - handle to the PVT
 *          aliasNode - a valid PVT node on which to build the alias
 *          strLen    - length of the given string
 * output : string    - allocated buffer of length strLen
 * return : The length of the string that was copied
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV cmAliasPVT2String(
    IN  HPVT                hVal,
    IN  RvPvtNodeId         aliasNode,
    IN  RvInt               strLen,
    OUT RvChar*             string)
{
    cmAlias alias;
    RvChar aliasString[512];
    int ret;

    memset(&alias, 0, sizeof(alias));
    alias.string = aliasString;
    alias.length = sizeof(aliasString);

    if ((ret=vtToAlias(hVal, &alias, aliasNode, RV_TRUE)) < 0)
        return ret;
    ret = alias2str(&alias, string, strLen);
    return ret;
}


RVAPI
int RVCALLCONV cmCallGetCallingPartyId(
                                     IN     HCALL       hsCall,
                                     OUT    char*       partyId,
                                     IN     int         len)
{
    cmTransportAddress tr;
    cmAlias alias;
    char number[513];
    HAPP hApp;
    int i;
    char delimiter[2];
    int error=RV_ERROR_UNKNOWN;

    if (hsCall == NULL) return RV_ERROR_NULLPTR;

    strcpy(delimiter, ",");
    hApp=cmGetAHandle((HPROTOCOL)hsCall);
    if (!hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallGetCallingPartyId(hsCall=%p,partyId,len=%d)", hsCall, len));
    memset(&alias, 0, sizeof(alias));
    alias.string = number;
    delimiter[0] = cmGetDelimiter(hApp);

    if (len >= getIPLength(&alias.transport))
    {
        partyId[0]=0;
        if (cmCallGetParam(hsCall,cmParamSrcCallSignalAddress,0,0,(char*)&tr) >= 0)
        {
            RvAddress addr;
            char* strPtr = partyId + 3;

            if (RvH323CmToCoreAddress(&tr, &addr) == RV_OK)
            {
                strcpy(partyId, "TA:");
                if (RvAddressGetString(&addr, (RvSize_t)len-8, strPtr) == NULL)
                    partyId[0] = '\0';
                RvSprintf(partyId+strlen(partyId), ":%d", RvAddressGetIpPort(&addr));
                RvAddressDestruct(&addr);
            }
        }

        i=0;
        alias.length = sizeof(number);
        while (cmCallGetParam(hsCall,cmParamSourceAddress,i++,0,(char *)&alias)>=0)
        {
            int length = (int)strlen(partyId);
            RvBool addedDelimiter = RV_FALSE;
            if ((length) && ((len-length) > 1))
            {
                strcat(partyId, delimiter);
                length++;
                addedDelimiter = RV_TRUE;
            }
            alias.string[alias.length] = 0;
            if (alias2str(&alias, &partyId[length], len-length-1) <= 0)
            {
                /* remove the delimiter */
                if (addedDelimiter)
                    partyId[length-1] = 0;
            }
            alias.length = sizeof(number);
        }

        alias.length = sizeof(number);
        if (cmCallGetParam(hsCall,cmParamCallingPartySubAddress,0,0,(char *)&alias)>=0)
        {
            int length = (int)strlen(partyId);
            if ((len-length-5-(int)strlen(number)) > 0)
            {
                if (length) strcat(partyId,delimiter);
                strcat(partyId,"SUB:");
                strcat(partyId,number);
            }
        }

        alias.length = sizeof(number);
        if (cmCallGetParam(hsCall,cmParamCallingPartyNumber,0,0,(char *)&alias)>=0)
        {
            int length = (int)strlen(partyId);
            if ((len-length-1-(int)strlen(number)) > 0)
            {
                if (length) strcat(partyId,delimiter);
                alias.string[alias.length] = 0;
                strcat(partyId,number);
            }
        }
        error=0;
    }

    cmiAPIExit((hApp, "cmCallGetCallingPartyId(partyId=%.100s)=%d", nprn(partyId), error));
    return error;
}


RVAPI
int RVCALLCONV cmCallGetCalledPartyId(
                                    IN      HCALL       hsCall,
                                    OUT     char*       partyId,
                                    IN      int         len)
{
    cmTransportAddress tr;
    cmAlias alias;
    char number[513];
    int i;
    char delimiter[2];
    int error=RV_ERROR_UNKNOWN;
    HAPP hApp;

	memset(number, 0, sizeof (number));

    strcpy(delimiter, ",");
    if (!hsCall) return RV_ERROR_UNKNOWN;
    hApp=cmGetAHandle((HPROTOCOL)hsCall);
    if (!hApp) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmCallGetCalledPartyId(hsCall=%p,partyId,len=%d)", hsCall, len));
    alias.string = number;
    delimiter[0] = cmGetDelimiter(hApp);

    if (len >= RV_ADDRESS_MAXSTRINGSIZE)
    {
        partyId[0]=0;
        if (cmCallGetParam(hsCall,cmParamDestCallSignalAddress,0,0,(char*)&tr) >= 0)
        {
            RvAddress addr;
            char* strPtr = partyId + 3;

            if (RvH323CmToCoreAddress(&tr, &addr) == RV_OK)
            {
                strcpy(partyId, "TA:");
                if (RvAddressGetString(&addr, (RvSize_t)len-8, strPtr) == NULL)
                    partyId[0] = '\0';
                RvSprintf(partyId+(int)strlen(partyId), ":%d", RvAddressGetIpPort(&addr));
                RvAddressDestruct(&addr);
            }
        }

        i=0;
        alias.length = sizeof(number);
        while (cmCallGetParam(hsCall,cmParamDestinationAddress,i++,0,(char *)&alias)>=0)
        {
            int length = (int)strlen(partyId);
            RvBool addedDelimiter = RV_FALSE;
            if ((length) && ((len-length) > 1))
            {
                strcat(partyId,delimiter);
                length++;
                addedDelimiter = RV_TRUE;
            }
            alias.string[alias.length] = 0;
            if (alias2str(&alias, &partyId[length], len-length-1) <= 0)
            {
                /* remove the delimiter */
                if (addedDelimiter)
                    partyId[length-1] = 0;
            }
            alias.length = sizeof(number);
        }

        alias.length = sizeof(number);
        if (cmCallGetParam(hsCall,cmParamCalledPartySubAddress,0,0,(char *)&alias)>=0)
        {
            int length = (int)strlen(partyId);
            if ((len-length-5-(int)strlen(number)) > 0)
            {
                if (length) strcat(partyId,delimiter);
                strcat(partyId,"SUB:");
                strcat(partyId,number);
            }
        }

        alias.length = sizeof(number);
        if (cmCallGetParam(hsCall,cmParamExtension,0,0,(char *)&alias)>=0)
        {
            int length = (int)strlen(partyId);

            if (alias.type==cmAliasTypeH323ID)
            {
                char str[257];
                int iLength = alias.length;
                utlBmp2Chr(str,(RvUint8*)number,iLength);
                strncpy(number, str, 256);
                number[256] = 0;
            }

            if ((len-length-1-((alias.type==cmAliasTypeE164)?4:6)-(int)strlen(number)) > 0)
            {
                if (length) strcat(partyId,delimiter);
                strcat(partyId,(alias.type==cmAliasTypeE164)?"EXT:":"EXTID:");
                strcat(partyId,number);
            }
        }

        alias.length = sizeof(number);
        if (cmCallGetParam(hsCall,cmParamCalledPartyNumber,0,0,(char *)&alias)>=0)
        {
            int length = (int)strlen(partyId);
            if ((len-length-1-(int)strlen(number)) > 0)
            {
                if (length) strcat(partyId,delimiter);
                alias.string[alias.length] = 0;
                strcat(partyId,number);
            }

            i = 0;
            alias.length = sizeof(number);
            while (cmCallGetParam(hsCall,cmParamDestExtraCallInfo,i++,0,(char *)&alias)>=0)
            {
                length = (int)strlen(partyId);
                if ((len-length-1-(int)strlen(number)) > 0)
                {
                    strcat(partyId,";");
                    strcat(partyId,number);
                }
                alias.length = sizeof(number);
            }
        }
        error=0;
    }
    cmiAPIExit((hApp, "cmCallGetCalledPartyId(callerId=%.100s)=%d", nprn(partyId), error));
    return error;
}

RVAPI
int RVCALLCONV cmCallSetIndependentSupplementaryService(
                                                      IN      HCALL               hsCall
                                                      )
{
    HAPP hApp;
    hApp=cmGetAHandle((HPROTOCOL)hsCall);
    if (!hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallIndependentSupplementaryService(hsCall=%p)", hsCall));
    if (cmCallGetOrigin(hsCall,NULL))
    {
        cmCallSetParam(hsCall,cmParamMultiRate,0,0,0);
        cmCallSetParam(hsCall,cmParamConferenceGoal,0,(RvInt32)cmCallIndependentSupplementaryService,0);
        cmCallSetParam(hsCall,cmParamInformationTransferCapability,0,(RvInt32)cmITCUnrestricted,0);
        cmCallSetParam(hsCall,cmParamRequestedRate,0,0,0);
    }
    cmiAPIExit((hApp, "cmCallIndependentSupplementaryService=0"));
    return 0;
}

RVAPI
int RVCALLCONV cmCallAnswerExt(
        IN  HCALL           hsCall,
        IN  const RvChar*   display,
        IN  const RvChar*   userUser,
        IN  int             userUserSize)
{
    int status = RV_ERROR_DESTRUCTED;
    callElem*call=(callElem*)hsCall;

    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)call))
    {
        RvBool bSetDisplay = RV_TRUE;
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER | RV_LOGLEVEL_LEAVE))
        cmElem* app=(cmElem*)emaGetInstance((EMAElement)hsCall);
#endif
        cmiAPIEnter(((HAPP)app, "cmCallAnswerExt(hsCall=%p,display=%.100s,userUserSize=%d)", hsCall, nprn(display), userUserSize));

        if (display != NULL)
        {
            bSetDisplay = RV_FALSE;
            if (display[0] != '\0')
                cmCallSetParam((HCALL)hsCall, cmParamConnectDisplay, 0, 0, display);
        }
        if (userUser != NULL)
            cmCallSetParam((HCALL)hsCall, cmParamConnectUserUser, 0, userUserSize, userUser);

        /* since call answer does a lot before checking the call state (unlike accept or proceeding)
           we should make sure we're not in idle state */
        if (!m_callget(call,callInitiator) && (call->state!=cmCallStateIdle))
        {
            /* We're the destination side of this call */
            cmCallState_e oldState;

            if( (oldState = call->state) == cmCallStateWaitAddressAck )
                cmCallAddressComplete(hsCall);

            if(((oldState == cmCallStateInit) && (call->state == cmCallStateConnected)) ||
                /* This happens when we're working with H450.9.
                   In this case, the call to cmCallAnswer() from the SSE will force the
                   StateOffering to be generated by the call to cmCallAddressComplete above.
                   If the application on top of it decides to call cmCallAnswer() from there,
                   then the original call to cmCallAnswer() will fail.
                   This if statement, makes sure that the original call to cmCallAnswer() succeeds. */
                ((oldState == cmCallStateWaitAddressAck) && (call->state == cmCallStateConnected)))
                /* This happens when we are on auto answer mode, and when we choose to
                   complete the address, we also answer the call automatically */
            {
                status = 0;
            }
            else
            {
                /* This is the regular path for such actions. We'll take it most of the times */

                /* Open any Accepted fast start channels */
                cmFastStartChannelsReady(hsCall);

                /* Connect the call */
                status = callAnswer(call, bSetDisplay);

                if (cmiGetExtendedFastConnectStatus((HCALL)call, RV_TRUE) != cmExtendedFastConnectApproved)
                {
                    /* Delete fast start information - we won't need it any more */
                    deleteIncomingFastStartOffer((HCALL)call);
                }
            }
        }
        cmiAPIExit(((HAPP)app, "cmCallAnswerExt=%d", status));
        emaUnlock((EMAElement)call);
    }

    return status;
}


RVAPI
int RVCALLCONV cmCallAnswerDisplay(
        IN  HCALL           hsCall,
        IN  const RvChar*   display)
{
    HAPP hApp;
    hApp=cmGetAHandle((HPROTOCOL)hsCall);
    if (!hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallAnswerDisplay(hsCall=%p,display=%.100s)", hsCall, nprn(display)));
    cmCallAnswerExt(hsCall,display,NULL,0);
    cmiAPIExit((hApp, "cmCallAnswerDisplay=0"));
    return 0;
}


RVAPI
int RVCALLCONV cmCallGetDisplayInfo(
                                  IN    HCALL       hsCall,
                                  OUT   char*       display,
                                  IN    int         displaySize)
{
    RvInt32 size=displaySize-1;
    HAPP hApp;
    hApp=cmGetAHandle((HPROTOCOL)hsCall);
    if (!hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallGetDisplayInfo(hsCall=%p,display,displaySize=%d)", hsCall, displaySize));
    if (display && displaySize>0)
    {
        if (size)
            display[0]=0;
        cmCallGetParam((HCALL)hsCall,(cmCallGetOrigin(hsCall,NULL)?cmParamConnectDisplay:cmParamDisplay),0,(RvInt32*)&size,display);
        if (size<displaySize)
            display[size]=0;
    }
    cmiAPIExit((hApp, "cmCallGetDisplayInfo(%.100s)=%d", nprn(display), size));
    return size;
}

RVAPI
int RVCALLCONV cmCallGetUserUserInfo(
                                   IN   HCALL       hsCall,
                                   OUT  char*       userUser,
                                   IN   int         userUserSize)
{
    RvUint32 size = (RvUint32)(userUserSize-1);
    HAPP hApp;
    hApp=cmGetAHandle((HPROTOCOL)hsCall);
    if (!hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallGetUserUserInfo(hsCall=%p,userUser,userUserSize=%d)", hsCall, userUserSize));
    if (userUser)
    {
        cmCallGetParam((HCALL)hsCall,(cmCallGetOrigin(hsCall,NULL)?cmParamConnectUserUser:cmParamUserUser),0,(RvInt32*)&size,userUser);
    }
    cmiAPIExit((hApp, "cmCallGetUserUserInfo(%.16s)=%d", nprn(userUser), size));
    return size;
}


RVAPI
int RVCALLCONV cmCallGetRemoteEndpointInfoHandle(
                                               IN   HCALL       hsCall)
{

    RvInt32 endpointInfo = RV_PVT_INVALID_NODEID;
    HAPP hApp;
    cmCallParam param;
    hApp=cmGetAHandle((HPROTOCOL)hsCall);
    if (hApp == NULL) return RV_PVT_INVALID_NODEID;

    cmiAPIEnter((hApp, "cmCallGetRemoteEndpointInfoHandle(hsCall=%p)", hsCall));

    if (cmCallGetOrigin(hsCall, NULL))
        param = cmParamFullDestinationInfo;
    else
        param = cmParamFullSourceInfo;
    cmCallGetParam(hsCall, param, 0, &endpointInfo, NULL);

    cmiAPIExit((hApp, "cmCallGetRemoteEndpointInfoHandle=%d", endpointInfo));
    return endpointInfo;
}

RVAPI
int RVCALLCONV cmCallGetLocalEndpointInfoHandle(
                                              IN    HCALL       hsCall)
{
    RvInt32 endpointInfo = RV_PVT_INVALID_NODEID;
    HAPP hApp;
    cmCallParam param;
    hApp=cmGetAHandle((HPROTOCOL)hsCall);
    if (hApp == NULL) return RV_PVT_INVALID_NODEID;

    cmiAPIEnter((hApp, "cmCallGetLocalEndpointInfoHandle(hsCall=%p)", hsCall));

    if (cmCallGetOrigin(hsCall, NULL))
        param = cmParamFullSourceInfo;
    else
        param = cmParamFullDestinationInfo;
    cmCallGetParam(hsCall, param, 0, &endpointInfo, NULL);

    cmiAPIExit((hApp, "cmCallGetLocalEndpointInfoHandle=%d", endpointInfo));
    return endpointInfo;
}

RVAPI
int RVCALLCONV cmGetEndpointInfoHandle(
                                     IN     HAPP        hApp)
{
    int endpointInfo = RV_PVT_INVALID_NODEID;
    if (hApp == NULL) return RV_PVT_INVALID_NODEID;

    cmiAPIEnter((hApp,"cmGetEndpointInfoHandle(hApp=%p)",hApp));
    __pvtGetNodeIdByFieldIds(endpointInfo,((cmElem*)hApp)->hVal,cmGetRASConfigurationHandle(hApp),
                            {_q931(registrationInfo)
                             _q931(terminalType)
                             LAST_TOKEN });
    cmiAPIExit((hApp,"cmGetEndpointInfoHandle=%d",endpointInfo));
    return endpointInfo;
}

RVAPI
int RVCALLCONV cmCallGetRate(
                           IN   HCALL       hsCall,
                           OUT  RvUint32*   rate)
{
    HAPP hApp;
    RvInt32 _rate = 0;
    hApp=cmGetAHandle((HPROTOCOL)hsCall);
    if (!hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp,"cmCallGetRate(hsCall=%p,rate)", hsCall));
    cmCallGetParam(hsCall,cmParamRate,0,&_rate,NULL);
    cmiAPIExit((hApp, "cmCallGetRate(rate=%ld)=%d", _rate, 0));
    if (rate != NULL)
        *rate = (RvUint32)_rate;
    return _rate;
}

RVAPI
RvStatus RVCALLCONV cmCallSetRate(
                           IN   HCALL       hsCall,
                           IN   RvUint32    rate)
{
    HAPP hApp;
    callElem*call=(callElem*)hsCall;

    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)call))
    {
        hApp=cmGetAHandle((HPROTOCOL)hsCall);
        if (!hApp)
        {
            emaUnlock((EMAElement)call);
            return RV_ERROR_UNKNOWN;
        }
        cmiAPIEnter((hApp, "cmCallSetRate(hsCall=%p,rate=%ld)", hsCall, rate));
        cmCallSetParam(hsCall,cmParamRequestedRate,0,(RvInt32)rate,NULL);
        if (cmiAutoRAS(hApp))
        {
            cmiAutoRASCallSetRate(hsCall,(int)rate);
        }
        else
        {
            if (!((cmElem *)hApp)->gatekeeper)
            {
                /* We're not a GK, and there's no automatic ras - notify application about it */
                cmCallSetParam(hsCall,cmParamRate,0,(RvInt32)rate,NULL);
                callNewRate((callElem *)hsCall);
            }
        }
        cmiAPIExit((hApp, "cmCallSetRate=0"));
        emaUnlock((EMAElement)call);
        return RV_OK;
    }

    return RV_ERROR_DESTRUCTED; 
}



RVAPI
int RVCALLCONV cmCallOverlapSending(
    IN HCALL        hsCall,
    IN const RvChar *address)
{
    return cmCallOverlapSendingExt(hsCall, address,RV_FALSE);
}

RVAPI
int RVCALLCONV cmCallForward(
        IN  HCALL           hsCall,
        IN  const RvChar    *destAddress)
{
    RvChar destA[512];
    RvChar bmpStr[512];
    RvChar* numPtr=NULL;
    int id=0;
    int i;
    RvBool namedID=RV_TRUE;
    RvChar* nextPtr=NULL;
    HAPP hApp=cmGetAHandle((HPROTOCOL)hsCall);
    RvChar delimiter;

    if (!hApp || !hsCall) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmCallForward(hsCall=%p,destAddr=%.100s)", hsCall, nprn(destAddress)));

    delimiter=cmGetDelimiter(hApp);
    strncpy(destA,destAddress,sizeof(destA));
    numPtr=destA;
    id=0;
    while(numPtr&&namedID)
    {
        int iDelimiter = delimiter;
        namedID=RV_FALSE;
        nextPtr=(RvChar*) strchr(numPtr,iDelimiter);
        {
            cmAlias alias;
            while(isspace((int)*numPtr)) numPtr++;
            if (nextPtr)
            {
                *nextPtr=0;
                nextPtr++;
            }

            if (insertAlias(&numPtr,&alias,bmpStr) >= 0)
            {
                namedID=RV_TRUE;
                cmCallSetParam(hsCall,cmParamAlternativeAliasAddress,id++,0,(char *)&alias);
            }
            else    if (!strncmp("EXT:",numPtr,4))
                    {
                        namedID=RV_TRUE;
                        numPtr+=4;
                        alias.string=numPtr;
                        alias.length=(RvUint16)strlen(alias.string);
                        alias.type=cmAliasTypeE164;

                        cmCallSetParam(hsCall,cmParamAlternativeExtension,0,0,(char *)&alias);
                    }
                    else
                        if (!strncmp("EXTID:",numPtr,6))
                        {
                            namedID=RV_TRUE;
                            numPtr+=6;
                            alias.type=cmAliasTypeH323ID;
                            alias.length=(RvUint16)utlChr2Bmp(numPtr, (RvUint8*)bmpStr);
                            alias.string=bmpStr;
                            cmCallSetParam(hsCall,cmParamAlternativeExtension,0,0,(char *)&alias);
                        }
                        else
                            if (strlen(numPtr)==strspn(numPtr,"0123456789,#*;"))
                            {
                                i=-1;
                                while(numPtr)
                                {
                                    alias.string=numPtr;
                                    numPtr=(RvChar*) strchr(numPtr,';');
                                    if (numPtr)
                                    {
                                        *numPtr=0;
                                        numPtr++;
                                    }
                                    alias.type=cmAliasTypeE164;
                                    alias.length=(RvUint16)strlen(alias.string);
                                    cmCallSetParam((HCALL)hsCall,(i<0)?cmParamAlternativeAliasAddress:cmParamAlternativeDestExtraCallInfo,(i<0)?id++:i,sizeof(cmAlias),(char*)&alias);
                                    i++;
                                }

                                alias.string=numPtr;
                                break;
                            }
        }
        if (!strncmp("TA:",numPtr,3))
        {
            cmTransportAddress qAddress;
            RvAddress addr;
            namedID=RV_TRUE;
            numPtr+=3;

            memset(&qAddress, 0, sizeof(qAddress));
            qAddress.type = cmTransportTypeIP;
            qAddress.port = 1720;

            if (stringToCoreAddress(numPtr, &addr) == RV_OK)
            {
                RvH323CoreToCmAddress(&addr, &qAddress);
                RvAddressDestruct(&addr);
            }

            if (qAddress.port == 0)
                qAddress.port = 1720;

            cmCallSetParam((HCALL)hsCall,cmParamAlternativeAddress,0,
                sizeof(cmTransportAddress),(char*)&qAddress);
        }
        if (namedID)
            numPtr=nextPtr;
    }

    cmCallSetParam((HCALL)hsCall,cmParamFacilityReason,0,(RvInt32)cmReasonTypeCallForwarded,NULL);
    cmCallFacility(hsCall,-1);
    cmiAPIExit((hApp, "cmCallForward()=0"));
    return 0;
}


#ifdef __cplusplus
}
#endif
