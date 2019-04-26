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
epConversions.c

Conversion functions widely used by this module.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stkutils.h"

#include "lepConversions.h"


#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/


typedef struct
{
    const RvChar    *strValue;
    RvInt32         enumValue;
} ConvertValue;





/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/


/* Internal string buffer, returned by some of the conversion functions in this module */
static RvChar internalBuffer[10000];




/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/
static RvUint8 hex2byte(
    IN RvChar       ch);
static RvInt32 string2Enum(
    IN const RvChar *string,
    IN ConvertValue *cvTable);
static const RvChar* enum2String(
    IN RvInt32      value,
    IN ConvertValue *cvTable);
static int parseList(
    IN  const RvChar    *string,
    IN  int             from,
    OUT int             *firstChar,
    OUT int             *numChars);
cmPartyNumberType String2PartyNumber(IN const RvChar *string);
const RvChar *PartyNumber2String(IN cmPartyNumberType value);





/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * str2hex
 * ----------------------------------------------------------------------------
 * General: Convert a string of bytes into a hex string.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  buffer   - Buffer to convert.
 *         result   - Resulting string holding the field.
 *         size     - Size of buffer.
 * Output: None.
 *****************************************************************************/
void str2hex(
    IN void     *buffer,
    IN RvChar   *hexStr,
    IN RvSize_t size)
{
    static const char* hexChars = "0123456789abcdef";
    RvUint8* p = (RvUint8 *)buffer;
    RvSize_t i;

    for (i = 0; i < size; i++)
    {
        hexStr[i*2+1] = hexChars[p[i] & 0x0f];
        hexStr[i*2] = hexChars[p[i] >> 4];
    }
    hexStr[size*2] = '\0';
}


/******************************************************************************
 * hex2str
 * ----------------------------------------------------------------------------
 * General: Convert a hex string into a string of raw bytes.
 *
 * Return Value: Number of bytes in buffer.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hexStr   - Hex string to convert.
 *         result   - Resulting string holding the data.
 *         size     - Size of result buffer.
 * Output: None.
 *****************************************************************************/
RvSize_t hex2str(
    IN const RvChar *hexStr,
    IN void         *result,
    IN RvSize_t     size)
{
    RvUint8* p = (RvUint8 *)result;
    RvSize_t i;
    RvSize_t len;
    len = strlen(hexStr) / 2;
    if (len > size) len = size;

    for (i = 0; i < len; i++)
    {
        p[i] = (RvUint8)
            (hex2byte(hexStr[i*2+1]) |
             ((RvUint8)(hex2byte(hexStr[i*2]) << 4)));
    }
    return len;
}



static ConvertValue cvGenStatus[] = {
    {"Success",                 (RvInt32)RV_OK},
    {"Error",                   (RvInt32)RV_ERROR_UNKNOWN},
    {"Resources problem",       (RvInt32)RV_ERROR_OUTOFRESOURCES},
    {"Bad parameter passed",    (RvInt32)RV_ERROR_BADPARAM},
    {"Null pointer",            (RvInt32)RV_ERROR_NULLPTR},
    {"Out of range",            (RvInt32)RV_ERROR_OUTOFRANGE},
    {"Object destructed",       (RvInt32)RV_ERROR_DESTRUCTED},
    {"Operation not supported", (RvInt32)RV_ERROR_NOTSUPPORTED},
    {"Object uninitialized",    (RvInt32)RV_ERROR_UNINITIALIZED},
    {"Try again",               (RvInt32)RV_ERROR_TRY_AGAIN},
    {"Illegal action",          (RvInt32)RV_ERROR_ILLEGAL_ACTION},
    {"Network problem",         (RvInt32)RV_ERROR_NETWORK_PROBLEM},
    {"Invalid handle",          (RvInt32)RV_ERROR_INVALID_HANDLE},
    {"Not found",               (RvInt32)RV_ERROR_NOT_FOUND},
    {"Insufficient buffer",     (RvInt32)RV_ERROR_INSUFFICIENT_BUFFER},
    {NULL, -1}
};

const RvChar *Status2String(IN int status)
{
    static RvChar result[40];

    if (status >= 0)
        sprintf(result, "Success (%d)", status);
    else
    {
        const RvChar* strName = enum2String(RvErrorGetCode(status), cvGenStatus);
        if (strcmp(strName, "-unknown-") != 0)
            sprintf(result, "Failure (%s)", strName);
        else
            sprintf(result, "Failure (%d)", status);
    }

    return result;
}

int String2Status(IN const RvChar *string)
{
    if (strncmp("Success", string, 7) == 0)
        return 0;
    else
        return RV_ERROR_UNKNOWN;
}


static ConvertValue cvCMCallState[] = {
    {"Init",                (RvInt32)cmCallStateInit},
    {"DialTone",            (RvInt32)cmCallStateDialtone},
    {"Proceeding",          (RvInt32)cmCallStateProceeding},
    {"RingBack",            (RvInt32)cmCallStateRingBack},
    {"Connected",           (RvInt32)cmCallStateConnected},
    {"Disconnected",        (RvInt32)cmCallStateDisconnected},
    {"Idle",                (RvInt32)cmCallStateIdle},
    {"Offering",            (RvInt32)cmCallStateOffering},
    {"Transferring",        (RvInt32)cmCallStateTransfering},
    {"WaitAddressAck",      (RvInt32)cmCallStateWaitAddressAck},
    {NULL, -1}};

cmCallState_e String2CMCallState(IN const RvChar *string)
{
    return (cmCallState_e)string2Enum(string, cvCMCallState);
}

const RvChar *CMCallState2String(IN cmCallState_e state)
{
    return enum2String((RvInt32)state, cvCMCallState);
}


static ConvertValue cvCMCallStateMode[] = {
    {"DisconnectedBusy",                    (RvInt32)cmCallStateModeDisconnectedBusy},
    {"DisconnectedNormal",                  (RvInt32)cmCallStateModeDisconnectedNormal},
    {"DisconnectedReject",                  (RvInt32)cmCallStateModeDisconnectedReject},
    {"DisconnectedUnreachable",             (RvInt32)cmCallStateModeDisconnectedUnreachable},
    {"DisconnectedUnknown",                 (RvInt32)cmCallStateModeDisconnectedUnknown},
    {"DisconnectedLocal",                   (RvInt32)cmCallStateModeDisconnectedLocal},
    {"ConnectedCallSetup",                  (RvInt32)cmCallStateModeConnectedCallSetup},
    {"ConnectedCall",                       (RvInt32)cmCallStateModeConnectedCall},
    {"ConnectedConference",                 (RvInt32)cmCallStateModeConnectedConference},
    {"OfferingCreate",                      (RvInt32)cmCallStateModeOfferingCreate},
    {"OfferingInvite",                      (RvInt32)cmCallStateModeOfferingInvite},
    {"OfferingJoin",                        (RvInt32)cmCallStateModeOfferingJoin},
    {"OfferingCapabilityNegotiation",       (RvInt32)cmCallStateModeOfferingCapabilityNegotiation},
    {"CallIndependentSupplementaryService", (RvInt32)cmCallStateModeOfferingCallIndependentSupplementaryService},
    {"DisconnectedIncompleteAddress",       (RvInt32)cmCallStateModeDisconnectedIncompleteAddress},
    {"TransferingForwarded",                (RvInt32)cmCallStateModeTransferingForwarded},
    {"TransferingRouteToMC",                (RvInt32)cmCallStateModeTransferingRouteToMC},
    {"TransferingRouteToGatekeeper",        (RvInt32)cmCallStateModeTransferingRouteToGatekeeper},
    {NULL,-1}};

cmCallStateMode_e String2CMCallStateMode(IN const RvChar *string)
{
    return (cmCallStateMode_e)string2Enum(string, cvCMCallStateMode);
}

const RvChar *CMCallStateMode2String(IN cmCallStateMode_e stateMode)
{
    return enum2String((RvInt32)stateMode, cvCMCallStateMode);
}


static ConvertValue cvCMReasonType[] = {
    {"NoBandwidth",                 (RvInt32)cmReasonTypeNoBandwidth},
    {"GatekeeperResource",          (RvInt32)cmReasonTypeGatekeeperResource},
    {"UnreachableDestination",      (RvInt32)cmReasonTypeUnreachableDestination},
    {"DestinationRejection",        (RvInt32)cmReasonTypeDestinationRejection},
    {"InvalidRevision",             (RvInt32)cmReasonTypeInvalidRevision},
    {"NoPermision",                 (RvInt32)cmReasonTypeNoPermision},
    {"UnreachableGatekeeper",       (RvInt32)cmReasonTypeUnreachableGatekeeper},
    {"GatewayResource",             (RvInt32)cmReasonTypeGatewayResource},
    {"BadFormatAddress",            (RvInt32)cmReasonTypeBadFormatAddress},
    {"AdaptiveBusy",                (RvInt32)cmReasonTypeAdaptiveBusy},
    {"InConf",                      (RvInt32)cmReasonTypeInConf},
    {"UndefinedReason",             (RvInt32)cmReasonTypeUndefinedReason},
    {"RouteCallToGatekeeper",       (RvInt32)cmReasonTypeRouteCallToGatekeeper},
    {"CallForwarded",               (RvInt32)cmReasonTypeCallForwarded},
    {"RouteCallToMC",               (RvInt32)cmReasonTypeRouteCallToMC},
    {"FacilityCallDeflection",      (RvInt32)cmReasonTypeFacilityCallDeflection},
    {"SecurityDenied",              (RvInt32)cmReasonTypeSecurityDenied},
    {"CalledPartyNotRegistered",    (RvInt32)cmReasonTypeCalledPartyNotRegistered},
    {"CallerNotregistered",         (RvInt32)cmReasonTypeCallerNotregistered},
    {"ConferenceListChoice",        (RvInt32)cmReasonTypeConferenceListChoice},
    {"StartH245",                   (RvInt32)cmReasonTypeStartH245},
    {"NewConnectionNeeded",         (RvInt32)cmReasonTypeNewConnectionNeeded},
    {"NoH245",                      (RvInt32)cmReasonTypeNoH245},
    {"NewTokens",                   (RvInt32)cmReasonTypeNewTokens},
    {"FeatureSetUpdate",            (RvInt32)cmReasonTypeFeatureSetUpdate},
    {"ForwardedElements",           (RvInt32)cmReasonTypeForwardedElements},
    {"TransportedInformation",      (RvInt32)cmReasonTypeTransportedInformation},
    {"NeededFeatureNotSupported",   (RvInt32)cmReasonTypeNeededFeatureNotSupported},
    {"HopCountExceeded",            (RvInt32)cmReasonTypeHopCountExceeded},
    {NULL, -1}
};

cmReasonType String2CMReasonType(IN const RvChar *string)
{
    return (cmReasonType)string2Enum(string, cvCMReasonType);
}

const RvChar *CMReasonType2String(IN cmReasonType reason)
{
    return enum2String((RvInt32)reason, cvCMReasonType);
}


static ConvertValue cvCMCallDisconnectionReason[] = {
    {"Unknown",             (RvInt32)cmCallDisconnectionReasonUnknown},
    {"ProtocolError",       (RvInt32)cmCallDisconnectionReasonProtocolError},
    {"LocalDropped",        (RvInt32)cmCallDisconnectionReasonLocalDropped},
    {"RemoteDropped",       (RvInt32)cmCallDisconnectionReasonRemoteDropped},
    {"NotRegistered",       (RvInt32)cmCallDisconnectionReasonNotRegistered},
    {"ReceivedARJ",         (RvInt32)cmCallDisconnectionReasonReceivedARJ},
    {"ReceivedDRQ",         (RvInt32)cmCallDisconnectionReasonReceivedDRQ},
    {"NoRasResponse",       (RvInt32)cmCallDisconnectionReasonNoRasResponse},
    {"NoDialResponse",      (RvInt32)cmCallDisconnectionReasonNoDialResponse},
    {"Q931Closed",          (RvInt32)cmCallDisconnectionReasonQ931Closed},
    {"H245Closed",          (RvInt32)cmCallDisconnectionReasonH245Closed},
    {"TimeoutT301",         (RvInt32)cmCallDisconnectionReasonTimeoutT301},
    {"TimeoutT302",         (RvInt32)cmCallDisconnectionReasonTimeoutT302},
    {"TimeoutT303",         (RvInt32)cmCallDisconnectionReasonTimeoutT303},
    {"TimeoutT304",         (RvInt32)cmCallDisconnectionReasonTimeoutT304},
    {"TimeoutT310",         (RvInt32)cmCallDisconnectionReasonTimeoutT310},
    {NULL, -1}
};

cmCallDisconnectionReason String2CMCallDisconnectionReason(IN const RvChar *string)
{
    return (cmCallDisconnectionReason)string2Enum(string, cvCMCallDisconnectionReason);
}

const RvChar* CMCallDisconnectionReason2String(IN cmCallDisconnectionReason param)
{
    return enum2String((RvInt32)param, cvCMCallDisconnectionReason);
}


static ConvertValue cvCMCallParam[] = {
    {"First",                   (RvInt32)cmParamFirst},
    {"SourceAddress",           (RvInt32)cmParamSourceAddress},
    {"SourceInfo",              (RvInt32)cmParamSourceInfo},
    {"DestinationAddress",      (RvInt32)cmParamDestinationAddress},
    {"DestCallSignalAddress",   (RvInt32)cmParamDestCallSignalAddress},
    {"SourceCallSignalAddr",    (RvInt32)cmParamSrcCallSignalAddress},
    {"DestExtraCallInfo",       (RvInt32)cmParamDestExtraCallInfo},
    {"ActiveMC",                (RvInt32)cmParamActiveMc},
    {"CID",                     (RvInt32)cmParamCID},
    {"ConferenceGoal",          (RvInt32)cmParamConferenceGoal},
    {"CallType",                (RvInt32)cmParamCallType},
    {"SetupH245Addr",           (RvInt32)cmParamSetupH245Address},
    {"H245Addr",                (RvInt32)cmParamH245Address},
    {"DestInfo",                (RvInt32)cmParamDestinationInfo},
    {"ReleaseCompleteReason",   (RvInt32)cmParamReleaseCompleteReason},
    {"ReleaseCompleteCause",    (RvInt32)cmParamReleaseCompleteCause},
    {"Rate",                    (RvInt32)cmParamRate},
    {"RequestedRate",           (RvInt32)cmParamRequestedRate},
    {"InfoTransCap",            (RvInt32)cmParamInformationTransferCapability},
    {"MultiRate",               (RvInt32)cmParamMultiRate},
    {"CalledPartyNumber",       (RvInt32)cmParamCalledPartyNumber},
    {"CalledPartySubAddr",      (RvInt32)cmParamCalledPartySubAddress},
    {"CallingPartyNumber",      (RvInt32)cmParamCallingPartyNumber},
    {"CallingPartySubAddr",     (RvInt32)cmParamCallingPartySubAddress},
    {"Extension",               (RvInt32)cmParamExtension},
    {"AlternativeAddr",         (RvInt32)cmParamAlternativeAddress},
    {"AlternativeAlias",        (RvInt32)cmParamAlternativeAliasAddress},
    {"FacilityReason",          (RvInt32)cmParamFacilityReason},
    {"DestinationIpAddress",    (RvInt32)cmParamDestinationIpAddress},
    {"RemoteIP",                (RvInt32)cmParamRemoteIpAddress},
    {"Q931CRV",                 (RvInt32)cmParamCRV},
    {"RASCRV",                  (RvInt32)cmParamRASCRV},
    {"CallSignalingAddr",       (RvInt32)cmParamCallSignalAddress},
    {"EstablishH245",           (RvInt32)cmParamEstablishH245},
    {"Display",                 (RvInt32)cmParamDisplay},
    {"FacilityCallID",          (RvInt32)cmParamFacilityCID},
    {"ConnectDisplay",          (RvInt32)cmParamConnectDisplay},
    {"UserUser",                (RvInt32)cmParamUserUser},
    {"ConnectUserUser",         (RvInt32)cmParamConnectUserUser},
    {"FullSourceInfo",          (RvInt32)cmParamFullSourceInfo},
    {"FullDestinationInfo",     (RvInt32)cmParamFullDestinationInfo},
    {"SetupNonStandardData",    (RvInt32)cmParamSetupNonStandardData},
    {"CallPrNonStandardData",   (RvInt32)cmParamCallProceedingNonStandardData},
    {"AlertNonStandardData",    (RvInt32)cmParamAlertingNonStandardData},
    {"ConnectNonStandardData",  (RvInt32)cmParamConnectNonStandardData},
    {"RelCompNonStandardData",  (RvInt32)cmParamReleaseCompleteNonStandardData},
    {"SetupNonStandard",        (RvInt32)cmParamSetupNonStandard},
    {"CallProcNonStandard",     (RvInt32)cmParamCallProceedingNonStandard},
    {"AlertNonStandard",        (RvInt32)cmParamAlertingNonStandard},
    {"ConnectNonStandard",      (RvInt32)cmParamConnectNonStandard},
    {"RelCompNonStandard",      (RvInt32)cmParamReleaseCompleteNonStandard},
    {"AltDestExtraCallInfo",    (RvInt32)cmParamAlternativeDestExtraCallInfo},
    {"AltExtension",            (RvInt32)cmParamAlternativeExtension},
    {"SetupFastStart",          (RvInt32)cmParamSetupFastStart},
    {"ConnectFastStart",        (RvInt32)cmParamConnectFastStart},
    {"EarlyH245",               (RvInt32)cmParamEarlyH245},
    {"CallID",                  (RvInt32)cmParamCallID},
    {"PreGrantedARQ",           (RvInt32)cmParamPreGrantedArq},
    {"AlertingFastStart",       (RvInt32)cmParamAlertingFastStart},
    {"CallProcFastStart",       (RvInt32)cmParamCallProcFastStart},
    {"AlertingH245Addr",        (RvInt32)cmParamAlertingH245Address},
    {"CallProcH245Addr",        (RvInt32)cmParamCallProcH245Address},
    {"CanOverlapSending",       (RvInt32)cmParamCanOverlapSending},
    {"IncompleteAddress",       (RvInt32)cmParamIncompleteAddress},
    {"H245Tunneling",           (RvInt32)cmParamH245Tunneling},
    {"FastStartInUseObsolete",  (RvInt32)cmParamFastStartInUseObsolete},
    {"SetupCanOverlapSend",     (RvInt32)cmParamSetupCanOverlapSending},
    {"SetupSendingComplete",    (RvInt32)cmParamSetupSendingComplete},
    {"FullSourceAddress",       (RvInt32)cmParamFullSourceAddress},
    {"FullDestinationAddress",  (RvInt32)cmParamFullDestinationAddress},
    {"RouteCallSignalAddress",  (RvInt32)cmParamRouteCallSignalAddress},
    {"H245Stage",               (RvInt32)cmParamH245Stage},
    {"H245Parallel",            (RvInt32)cmParamH245Parallel},
    {"ShutdownEmptyConnection", (RvInt32)cmParamShutdownEmptyConnection},
    {"IsMultiplexed",           (RvInt32)cmParamIsMultiplexed},
    {"AnnexE",                  (RvInt32)cmParamAnnexE},
    {"DestinationAnnexEAddress",(RvInt32)cmParamDestinationAnnexEAddress},
    {"ConnectedAddress",        (RvInt32)cmParamConnectedAddress},
    {"FullDestExtraCallInfo",   (RvInt32)cmParamFullDestExtraCallInfo},
    {"FullExtension",           (RvInt32)cmParamFullExtension},
    {"SourceIpAddress",         (RvInt32)cmParamSourceIpAddress},
    {"State",                   (RvInt32)cmParamState},
    {"StateMode",               (RvInt32)cmParamStateMode},
    {"AutomaticRAS",            (RvInt32)cmParamAutomaticRAS},
    {"MsdTerminalType",         (RvInt32)cmParamMsdTerminalType},
    {"HopCount",                (RvInt32)cmParamHopCount},
    {"DisconnectionReason",     (RvInt32)cmParamDisconnectionReason},
    {"ExtendedFastConnectState",(RvInt32)cmParamExtendedFastConnectState},
    {NULL, -1}
};

cmCallParam String2CMCallParam(IN const RvChar *string)
{
    return (cmCallParam)string2Enum(string, cvCMCallParam);
}

const RvChar *CMCallParam2String(IN cmCallParam param)
{
    return enum2String((RvInt32)param, cvCMCallParam);
}


static ConvertValue cvCMParam[] = {
    {"LocalCallSignalAddress",  (RvInt32)cmParamLocalCallSignalAddress},
    {"LocalAnnexEAddress",      (RvInt32)cmParamLocalAnnexEAddress},
    {"LocalRASAddress",         (RvInt32)cmParamLocalRASAddress},
    {NULL, -1}
};

cmParam String2CMParam(IN const RvChar *string)
{
    return (cmParam)string2Enum(string, cvCMParam);
}

const RvChar *CMParam2String(IN cmParam param)
{
    return enum2String((RvInt32)param,cvCMParam);
}


static ConvertValue cvRasTransaction[] = {
    {"Gatekeeper",              (RvInt32)cmRASGatekeeper},
    {"Registration",            (RvInt32)cmRASRegistration},
    {"Unregistration",          (RvInt32)cmRASUnregistration},
    {"Admission",               (RvInt32)cmRASAdmission},
    {"Disengage",               (RvInt32)cmRASDisengage},
    {"Bandwidth",               (RvInt32)cmRASBandwidth},
    {"Location",                (RvInt32)cmRASLocation},
    {"Info",                    (RvInt32)cmRASInfo},
    {"NonStandard",             (RvInt32)cmRASNonStandard},
    {"Unknown",                 (RvInt32)cmRASUnknown},
    {"ResourceAvailability",    (RvInt32)cmRASResourceAvailability},
    {"UnsolicitedIRR",          (RvInt32)cmRASUnsolicitedIRR},
    {"ServiceControl",          (RvInt32)cmRASServiceControl},
    {NULL, -1}
};

cmRASTransaction String2RASTransaction(IN const RvChar *string)
{
    return (cmRASTransaction)string2Enum(string, cvRasTransaction);
}

const RvChar *RASTransaction2String(IN cmRASTransaction transaction)
{
    return enum2String((RvInt32)transaction ,cvRasTransaction);
}


static ConvertValue cvRasReason[] = {
    {"ResourceUnavailable",         (RvInt32)cmRASReasonResourceUnavailable},
    {"InsufficientResources",             (RvInt32)cmRASReasonInsufficientResources},
    {"InvalidRevision",                   (RvInt32)cmRASReasonInvalidRevision},
    {"InvalidCallSignalAddress",          (RvInt32)cmRASReasonInvalidCallSignalAddress},
    {"InvalidRASAddress",                 (RvInt32)cmRASReasonInvalidRASAddress},
    {"InvalidTerminalType",               (RvInt32)cmRASReasonInvalidTerminalType},
    {"InvalidPermission",                 (RvInt32)cmRASReasonInvalidPermission},
    {"InvalidConferenceID",               (RvInt32)cmRASReasonInvalidConferenceID},
    {"InvalidEndpointID",                 (RvInt32)cmRASReasonInvalidEndpointID},
    {"CallerNotRegistered",               (RvInt32)cmRASReasonCallerNotRegistered},
    {"CalledPartyNotRegistered",          (RvInt32)cmRASReasonCalledPartyNotRegistered},
    {"DiscoveryRequired",                 (RvInt32)cmRASReasonDiscoveryRequired},
    {"DuplicateAlias",                    (RvInt32)cmRASReasonDuplicateAlias},
    {"TransportNotSupported",             (RvInt32)cmRASReasonTransportNotSupported},
    {"CallInProgress",                    (RvInt32)cmRASReasonCallInProgress},
    {"RouteCallToGateKeeper",             (RvInt32)cmRASReasonRouteCallToGatekeeper},
    {"RequestToDropOther",                (RvInt32)cmRASReasonRequestToDropOther},
    {"NotRegistered",                     (RvInt32)cmRASReasonNotRegistered},
    {"Undefined",                         (RvInt32)cmRASReasonUndefined},
    {"TerminalExcluded",                  (RvInt32)cmRASReasonTerminalExcluded},
    {"NotBound",                          (RvInt32)cmRASReasonNotBound},
    {"NotCurrentlyRegistered",            (RvInt32)cmRASReasonNotCurrentlyRegistered},
    {"RequestDenied",                     (RvInt32)cmRASReasonRequestDenied},
    {"LocationNotFound",                  (RvInt32)cmRASReasonLocationNotFound},
    {"SecurityDenial",                    (RvInt32)cmRASReasonSecurityDenial},
    {"TransportQOSNotSupported",          (RvInt32)cmRASReasonTransportQOSNotSupported},
    {"ResourceUnaveliable",               (RvInt32)cmRASResourceUnavailable},
    {"InvalidAlias",                      (RvInt32)cmRASReasonInvalidAlias},
    {"PermissionDenied",                  (RvInt32)cmRASReasonPermissionDenied},
    {"QOSControlNotSupported",            (RvInt32)cmRASReasonQOSControlNotSupported},
    {"IncompleteAddress",                 (RvInt32)cmRASReasonIncompleteAddress},
    {"FullRegistrationRequired",          (RvInt32)cmRASReasonFullRegistrationRequired},
    {"RouteCallToSCN",                    (RvInt32)cmRASReasonRouteCallToSCN},
    {"AliasesInconsistent",               (RvInt32)cmRASReasonAliasesInconsistent},
    {"AdditiveRegistrationNotSupported",  (RvInt32)cmRASReasonAdditiveRegistrationNotSupported},
    {"InvalidTerminalAliases",            (RvInt32)cmRASReasonInvalidTerminalAliases},
    {"ExceedsCallCapacity",               (RvInt32)cmRASReasonExceedsCallCapacity},
    {"CollectDestination",                (RvInt32)cmRASReasonCollectDestination},
    {"CollectPIN",                        (RvInt32)cmRASReasonCollectPIN},
    {"GenericData",                       (RvInt32)cmRASReasonGenericData},
    {"NeededFeatureNotSupported",         (RvInt32)cmRASReasonNeededFeatureNotSupported},
    {"UnknownMessageResponse",            (RvInt32)cmRASReasonUnknownMessageResponse},
    {"HopCountExceeded",                  (RvInt32)cmRASReasonHopCountExceeded},
    {"SecurityDHmismatch",                (RvInt32)cmRASReasonSecurityDHmismatch},
    {"NoRouteToDestination",              (RvInt32)cmRASReasonNoRouteToDestination},
    {"UnallocatedNumber",                 (RvInt32)cmRASReasonUnallocatedNumber},
    {NULL, -1}
};

cmRASReason String2RASReason(IN const RvChar *string)
{
    return (cmRASReason)string2Enum(string, cvRasReason);
}

const RvChar *RASReason2String(IN cmRASReason reason)
{
    return enum2String((RvInt32)reason ,cvRasReason);
}


static ConvertValue cvRasStage[] = {
    {"Request",     (RvInt32)cmRASTrStageRequest},
    {"Confirm",     (RvInt32)cmRASTrStageConfirm},
    {"Reject",      (RvInt32)cmRASTrStageReject},
    {NULL, -1}
};

cmRASTrStage String2RASTrStage(IN const RvChar *string)
{
    return (cmRASTrStage)string2Enum(string, cvRasStage);
}

const RvChar *RASTrStage2String(IN cmRASTrStage reason)
{
    return enum2String((RvInt32)reason ,cvRasStage);
}


static ConvertValue cvRasParam[] = {
    {"GatekeeperID",            (RvInt32)cmRASParamGatekeeperID},
    {"RASAddress",              (RvInt32)cmRASParamRASAddress},
    {"CallSignalAddress",       (RvInt32)cmRASParamCallSignalAddress},
    {"EndpointType",            (RvInt32)cmRASParamEndpointType},
    {"TerminalType",            (RvInt32)cmRASParamTerminalType},
    {"EndpointAlias",           (RvInt32)cmRASParamEndpointAlias},
    {"TerminalAlias",           (RvInt32)cmRASParamTerminalAlias},
    {"DiscoveryComplete",       (RvInt32)cmRASParamDiscoveryComplete},
    {"EndpointVendor",          (RvInt32)cmRASParamEndpointVendor},
    {"CallType",                (RvInt32)cmRASParamCallType},
    {"CallModel",               (RvInt32)cmRASParamCallModel},
    {"EndpointID",              (RvInt32)cmRASParamEndpointID},
    {"DestInfo",                (RvInt32)cmRASParamDestInfo},
    {"SrcInfo",                 (RvInt32)cmRASParamSrcInfo},
    {"DestExtraCallInfo",       (RvInt32)cmRASParamDestExtraCallInfo},
    {"DestCallSignalAddress",   (RvInt32)cmRASParamDestCallSignalAddress},
    {"SrcCallSignalAddress",    (RvInt32)cmRASParamSrcCallSignalAddress},
    {"Bandwidth",               (RvInt32)cmRASParamBandwidth},
    {"ActiveMC",                (RvInt32)cmRASParamActiveMC},
    {"AnswerCall",              (RvInt32)cmRASParamAnswerCall},
    {"IrrFrequency",            (RvInt32)cmRASParamIrrFrequency},
    {"ReplyAddress",            (RvInt32)cmRASParamReplyAddress},
    {"DisengageReason",         (RvInt32)cmRASParamDisengageReason},
    {"RejectedAlias",           (RvInt32)cmRASParamRejectedAlias},
    {"RejectReason",            (RvInt32)cmRASParamRejectReason},
    {"CID",                     (RvInt32)cmRASParamCID},
    {"DestinationIpAddress",    (RvInt32)cmRASParamDestinationIpAddress},
    {"NonStandard",             (RvInt32)cmRASParamNonStandard},
    {"NonStandardData",         (RvInt32)cmRASParamNonStandardData},
    {"CRV",                     (RvInt32)cmRASParamCRV},
    {"MulticastTransaction",    (RvInt32)cmRASParamMulticastTransaction},
    {"TransportQOS",            (RvInt32)cmRASParamTransportQOS},
    {"KeepAlive",               (RvInt32)cmRASParamKeepAlive},
    {"TimeToLive",              (RvInt32)cmRASParamTimeToLive},
    {"Delay",                   (RvInt32)cmRASParamDelay},
    {"CallID",                  (RvInt32)cmRASParamCallID},
    {"AnsweredCall",            (RvInt32)cmRASParamAnsweredCall},
    {"AlmostOutOfResources",    (RvInt32)cmRASParamAlmostOutOfResources},
    {"AlternateGatekeeper",     (RvInt32)cmRASParamAlternateGatekeeper},
    {"AltGKInfo",               (RvInt32)cmRASParamAltGKInfo},
    {"AltGKisPermanent",        (RvInt32)cmRASParamAltGKisPermanent},
    {"Empty",                   (RvInt32)cmRASParamEmpty},
    {"SourceInfo",              (RvInt32)cmRASParamSourceInfo},
    {"NeedResponse",            (RvInt32)cmRASParamNeedResponse},
    {"MaintainConnection",      (RvInt32)cmRASParamMaintainConnection},
    {"MultipleCalls",           (RvInt32)cmRASParamMultipleCalls},
    {"WillRespondToIRR",        (RvInt32)cmRASParamWillRespondToIRR},
    {"SupportsAltGk",           (RvInt32)cmRASParamSupportsAltGk},
    {"AdditiveRegistration",    (RvInt32)cmRASParamAdditiveRegistration},
    {"SupportsAdditiveRegistration",(RvInt32)cmRASParamSupportsAdditiveRegistration},
    {"SegmentedResponseSupported",  (RvInt32)cmRASParamSegmentedResponseSupported},
    {"NextSegmentRequested",    (RvInt32)cmRASParamNextSegmentRequested},
    {"CapacityInfoRequested",   (RvInt32)cmRASParamCapacityInfoRequested},
    {"HopCount",                (RvInt32)cmRASParamHopCount},
    {"InvalidTerminalAlias",    (RvInt32)cmRASParamInvalidTerminalAlias},
    {"UnregReason",             (RvInt32)cmRASParamUnregReason},
    {"IrrStatus",               (RvInt32)cmRASParamIrrStatus},
    {"CallHandle",              (RvInt32)cmRASParamCallHandle},
    {"Extension",               (RvInt32)cmRASParamExtension},
    {"ApplicationHandle",       (RvInt32)cmRASParamApplicationHandle},
    {"Retries",                 (RvInt32)cmRASParamRetries},
    {"Timeout",                 (RvInt32)cmRASParamTimeout},
    {"LocalAddressIndex",       (RvInt32)cmRASParamLocalAddressIndex},
    {"Restart",                 (RvInt32)cmRASParamRestart},
    {"SourceEndpointInfo",      (RvInt32)cmRASParamSourceEndpointInfo},
    {"ModifiedSrcInfo",         (RvInt32)cmRASParamModifiedSrcInfo},
    {"CanMapSrcAlias",          (RvInt32)cmRASParamCanMapSrcAlias},
    {NULL, -1}
};

cmRASParam String2RASParam(IN const RvChar *string)
{
    return (cmRASParam)string2Enum(string, cvRasParam);
}

const RvChar *RASParam2String(IN cmRASParam reason)
{
    return enum2String((RvInt32)reason ,cvRasParam);
}


static ConvertValue cvCMChannelState[] = {
    {"DialTone",     (RvInt32)cmChannelStateDialtone},
    {"RingBack",     (RvInt32)cmChannelStateRingBack},
    {"Connected",    (RvInt32)cmChannelStateConnected},
    {"Disconnected", (RvInt32)cmChannelStateDisconnected},
    {"Idle",         (RvInt32)cmChannelStateIdle},
    {"Offering",     (RvInt32)cmChannelStateOffering},
    {NULL, -1}
};

cmChannelState_e String2CMChannelState(IN const RvChar *string)
{
    return (cmChannelState_e)string2Enum(string, cvCMChannelState);
}

const RvChar *CMChannelState2String(IN cmChannelState_e state)
{
    return enum2String((RvInt32)state, cvCMChannelState);
}


static ConvertValue cvCMChannelStateMode [] = {
    {"On",                                      (RvInt32)cmChannelStateModeOn},
    {"Off",                                     (RvInt32)cmChannelStateModeOff},
    {"DisconnectedLocal",                       (RvInt32)cmChannelStateModeDisconnectedLocal},
    {"DisconnectedRemote",                      (RvInt32)cmChannelStateModeDisconnectedRemote},
    {"DisconnectedMasterSlaveConflict",         (RvInt32)cmChannelStateModeDisconnectedMasterSlaveConflict},
    {"Duplex",                                  (RvInt32)cmChannelStateModeDuplex},
    {"DisconnectedReasonUnknown",               (RvInt32)cmChannelStateModeDisconnectedReasonUnknown},
    {"DisconnectedReasonReopen",                (RvInt32)cmChannelStateModeDisconnectedReasonReopen},
    {"DisconnectedReasonReservationFailure",    (RvInt32)cmChannelStateModeDisconnectedReasonReservationFailure},
    {"Reconfigured",                            (RvInt32)cmChannelStateModeReconfigured},
    {NULL, -1}
};

cmChannelStateMode_e String2CMChannelStateMode(IN const RvChar *string)
{
    return (cmChannelStateMode_e)string2Enum(string, cvCMChannelStateMode);
}

const RvChar *CMChannelStateMode2String(IN cmChannelStateMode_e stateMode)
{
    return enum2String((RvInt32)stateMode, cvCMChannelStateMode);
}


static ConvertValue cvCMH245Stage[] = {
    {"setup",             (RvInt32)cmTransH245Stage_setup},
    {"call proceeding",   (RvInt32)cmTransH245Stage_callProceeding},
    {"call proc",         (RvInt32)cmTransH245Stage_callProceeding},
    {"alerting",          (RvInt32)cmTransH245Stage_alerting},
    {"connect",           (RvInt32)cmTransH245Stage_connect},
    {"early",             (RvInt32)cmTransH245Stage_early},
    {"facility",          (RvInt32)cmTransH245Stage_facility},
    {"no H245",           (RvInt32)cmTransH245Stage_noH245},
    {NULL, -1}
};

cmH245Stage String2CMH245Stage(IN const RvChar *string)
{
    return (cmH245Stage)string2Enum(string, cvCMH245Stage);
}

const RvChar *CMH245Stage2String(IN cmH245Stage stage)
{
    return enum2String((RvInt32)stage ,cvCMH245Stage);
}


static ConvertValue cvCMAnnexEUsageMode[] = {
    {"AnnexE",      (RvInt32)cmTransUseAnnexE},
    {"TPKT",        (RvInt32)cmTransNoAnnexE},
    {"AnnexE/TPKT", (RvInt32)cmTransPreferedAnnexE},
    {"TPKT/AnnexE", (RvInt32)cmTransPreferedTPKT},
    {NULL, -1}
};

cmAnnexEUsageMode String2CMAnnexEUsageMode(IN const RvChar *string)
{
    return (cmAnnexEUsageMode)string2Enum(string, cvCMAnnexEUsageMode);
}

const RvChar *CMAnnexEUsageMode2String(IN cmAnnexEUsageMode stage)
{
    return enum2String((RvInt32)stage ,cvCMAnnexEUsageMode);
}


static ConvertValue cvCMExtendedFastConnectState[] = {
    {"No",            (RvInt32)cmExtendedFastConnectNo},
    {"Supported",     (RvInt32)cmExtendedFastConnectSupported},
    {"Required",      (RvInt32)cmExtendedFastConnectRequired},
    {"Undetermined",  (RvInt32)cmExtendedFastConnectUndetermined},
    {"Approved",      (RvInt32)cmExtendedFastConnectApproved},
    {NULL, -1}
};

cmExtendedFastConnectState_e String2CMExtendedFastConnectState(const RvChar *string)
{
    return (cmExtendedFastConnectState_e)string2Enum(string, cvCMAnnexEUsageMode);
}

const RvChar *CMExtendedFastConnectState2String(cmExtendedFastConnectState_e state)
{
    return enum2String((RvInt32)state ,cvCMExtendedFastConnectState);
}


static ConvertValue cvCMEndpointType[] = {
    {"Terminal",  (RvInt32)cmEndpointTypeTerminal},
    {"Gateway",   (RvInt32)cmEndpointTypeGateway},
    {"MCU",       (RvInt32)cmEndpointTypeMCU},
    {"GK",        (RvInt32)cmEndpointTypeGK},
    {"Undefined", (RvInt32)cmEndpointTypeUndefined},
    {"SET",       (RvInt32)cmEndpointTypeSET},
    {NULL, -1}
};

cmEndpointType String2EndpointType(IN const RvChar *string)
{
    return (cmEndpointType)string2Enum(string, cvCMEndpointType);
}

const RvChar *EndpointType2String(IN cmEndpointType val)
{
    return enum2String((RvInt32)val, cvCMEndpointType);
}


static ConvertValue cvCMCallType[] = {
    {"P2P",   (RvInt32)cmCallTypeP2P},
    {"One2N", (RvInt32)cmCallTypeOne2N},
    {"N2One", (RvInt32)cmCallTypeN2One},
    {"N2Nw",  (RvInt32)cmCallTypeN2Nw},
    {NULL, -1}
};

cmCallType String2CallType(IN const RvChar *string)
{
    return (cmCallType)string2Enum(string, cvCMCallType);
}

const RvChar *CallType2String(IN cmCallType val)
{
    return enum2String((RvInt32)val, cvCMCallType);
}

static ConvertValue cvCMCallModelType[] = {
    {"Direct",  (RvInt32)cmCallModelTypeDirect},
    {"Routed",  (RvInt32)cmCallModelTypeGKRouted},
    {NULL, -1}
};

cmCallModelType String2CallModelType(IN const RvChar *string)
{
    return (cmCallModelType)string2Enum(string, cvCMCallModelType);
}

const RvChar *CallModelType2String(IN cmCallModelType val)
{
    return enum2String((RvInt32)val, cvCMCallModelType);
}


static ConvertValue cvCMDisengageReason[] = {
    {"Forced",     (RvInt32)cmRASDisengageReasonForcedDrop},
    {"Normal",     (RvInt32)cmRASDisengageReasonNormalDrop},
    {"Undefined",  (RvInt32)cmRASDisengageReasonUndefinedReason},
    {NULL, -1}
};

cmRASDisengageReason String2DisengageReason(IN const RvChar *string)
{
    return (cmRASDisengageReason)string2Enum(string, cvCMDisengageReason);
}

const RvChar *DisengageReason2String(IN cmRASDisengageReason val)
{
    return enum2String((RvInt32)val, cvCMDisengageReason);
}


static ConvertValue cvCMUnregReason[] = {
    {"RegistrationRequired",  (RvInt32)cmRASUnregReasonReregistrationRequired},
    {"TtlExpired",            (RvInt32)cmRASUnregReasonTtlExpired},
    {"SecurityDenial",        (RvInt32)cmRASUnregReasonSecurityDenial},
    {"UndefinedReason",       (RvInt32)cmRASUnregReasonUndefinedReason},
    {"Maintenance",           (RvInt32)cmRASUnregReasonMaintenance},
    {NULL, -1}
};

cmRASUnregReason String2UnregReason(IN const RvChar *string)
{
    return (cmRASUnregReason)string2Enum(string, cvCMUnregReason);
}

const RvChar *UnregReason2String(IN cmRASUnregReason val)
{
    return enum2String((RvInt32)val, cvCMUnregReason);
}


static ConvertValue cvCMTransportQOS[] = {
    {"RegistrationRequired",  (RvInt32)cmTransportQOSEndpointControlled},
    {"TtlExpired",            (RvInt32)cmTransportQOSGatekeeperControlled},
    {"SecurityDenial",        (RvInt32)cmTransportQOSNoControl},
    {NULL, -1}
};

cmTransportQOS String2TransportQOS(IN const RvChar *string)
{
    return (cmTransportQOS)string2Enum(string, cvCMTransportQOS);
}

const RvChar *TransportQOS2String(IN cmTransportQOS val)
{
    return enum2String((RvInt32)val, cvCMTransportQOS);
}


static ConvertValue cvCMIrrStatus[] = {
    {"Complete",     (RvInt32)cmRASIrrComplete},
    {"Incomplete",   (RvInt32)cmRASIrrIncomplete},
    {"InvalidCall",  (RvInt32)cmRASIrrInvalidCall},
    {NULL, -1}
};

int String2IrrStatus(IN const RvChar *string)
{
    return (int)string2Enum(string, cvCMIrrStatus);
}

const RvChar *IrrStatus2String(IN int val)
{
    return enum2String((RvInt32)val, cvCMIrrStatus);
}


#ifdef USE_H450

static ConvertValue cvH450ssAction[] = {
    {"DefaultAck",      (RvInt32)ssDefaultAck},
    {"DefaultNack",     (RvInt32)ssDefaultNack},
    {"Wait",            (RvInt32)ssWait},
    {NULL, -1}
};

ssAction String2ssAction(IN const RvChar *string)
{
    return (ssAction)string2Enum(string, cvH450ssAction);
}

const RvChar *ssAction2String(IN ssAction val)
{
    return enum2String((RvInt32)val, cvH450ssAction);
}

static ConvertValue cvH450proc_e[] = {
    {"Unconditional",   (RvInt32)cfu_p},
    {"Busy",            (RvInt32)cfb_p},
    {"NoResponse",      (RvInt32)cfnr_p},
    {NULL, -1}
};

proc_e String2proc_e(IN const RvChar *string)
{
    return (proc_e)string2Enum(string, cvH450proc_e);
}

const RvChar *proc_e2String(IN proc_e proc)
{
    return enum2String((RvInt32)proc, cvH450proc_e);
}


static ConvertValue cvH450reason_e[] = {
    {"Unknown",         (RvInt32)unknown_r},
    {"Unconditional",   (RvInt32)cfu_r},
    {"Busy",            (RvInt32)cfb_r},
    {"NoResponse",      (RvInt32)cfnr_r},
    {NULL, -1}
};

reason_e String2reason_e(IN const RvChar *string)
{
    return (reason_e)string2Enum(string, cvH450reason_e);
}

const RvChar *reason_e2String(IN reason_e reason)
{
    return enum2String((RvInt32)reason, cvH450reason_e);
}


static ConvertValue cvH450niNameChoice[] = {
    {"Allowed",         (RvInt32)allowed},
    {"Restricted",      (RvInt32)restricted},
    {"NotAvailable",    (RvInt32)notAvailable},
    {"NA",              (RvInt32)notAvailable},
    {NULL, -1}
};

niNameChoice String2H450niNameChoice(IN const RvChar *string)
{
    return (niNameChoice)string2Enum(string, cvH450niNameChoice);
}

const RvChar *H450niNameChoice2String(IN niNameChoice val)
{
    return enum2String((RvInt32)val, cvH450niNameChoice);
}


static ConvertValue cvH450ccIndication[] = {
    {"Activated",   (RvInt32)ccActivated},
    {"Suspended",   (RvInt32)ccSuspended},
    {"Resume",      (RvInt32)ccResume},
    {"Ringout",     (RvInt32)ccRingout},
    {"Rejected",    (RvInt32)ccRejected},
    {"TimeOut",     (RvInt32)ccTimeOut},
    {"CallDropped", (RvInt32)ccCallDropped},
    {NULL, -1}
};

ccIndication String2CCIndication(IN const RvChar *string)
{
    return (ccIndication)string2Enum(string, cvH450ccIndication);
}

const RvChar *CCIndication2String(IN ccIndication indication)
{
    return enum2String((RvInt32)indication,cvH450ccIndication);
}


static ConvertValue cvH450Opcodes[] = {
    {"CIRequest",                   (RvInt32)ssCIRequest},
    {"CIGetCIPL",                   (RvInt32)ssCIGetCIPL},
    {"CIIsolated",                  (RvInt32)ssCIIsolated},
    {"CIForcedRelease",             (RvInt32)ssCIForcedRelease},
    {"CIWOBRequest",                (RvInt32)ssCIWOBRequest},
    {"CISilentMoitor",              (RvInt32)ssCISilentMoitor},
    {"CINotification",              (RvInt32)ssCINotification},
    {"CInotBusy",                   (RvInt32)ssCInotBusy},
    {"CItemporarilyUnavailable",    (RvInt32)ssCItemporarilyUnavailable},
    {"CInotAuthorized",             (RvInt32)ssCInotAuthorized},
    {"CIMakeSilent",                (RvInt32)ssCIMakeSilent},

    {"CMNRequest",                  (RvInt32)ssCMNRequest},
    {"CMNInform",                   (RvInt32)ssCMNInform},
    {"CMNBoth",                     (RvInt32)ssCMNBoth},
    {NULL, -1}
};

int String2SSPrimitive(IN const RvChar *string)
{
    return (int)string2Enum(string, cvH450Opcodes);
}

const RvChar *SSPrimitive2String(IN int opcode)
{
    return enum2String((RvInt32)opcode, cvH450Opcodes);
}


static ConvertValue cvH450Services[] = {
    {"Transfer",            (RvInt32)sseTransfer},
    {"Diversion",           (RvInt32)sseDiversion},
    {"Hold",                (RvInt32)sseHold},
    {"ParkPickup",          (RvInt32)sseParkPickup},
    {"Wait",                (RvInt32)sseWait},
    {"MWI",                 (RvInt32)sseMWI},
    {"Name indication",     (RvInt32)sseNI},
    {"Completion",          (RvInt32)sseCompletion},
    {"Offer",               (RvInt32)sseOffer},
    {"Intrusion",           (RvInt32)sseIntrusion},
    {"CMN",                 (RvInt32)sseCmn},
    {NULL, -1}
};

sseServices String2SSEServices(IN const RvChar *string)
{
    return (sseServices)string2Enum(string, cvH450Services);
}

const RvChar *SSEServices2String(IN sseServices opcode)
{
    return enum2String((RvInt32)opcode, cvH450Services);
}


#endif /* USE_H450 */


RvStatus String2CMNonStandardParam(IN const RvChar *string, OUT cmNonStandardParam *param)
{
    int country, extension, manufacture;
    int next, first, numChars;
    RvChar *p;
    RvChar ch;
    RvChar type;

    strncpy(internalBuffer, string, strlen(string));

    next = parseList(internalBuffer, 0, &first, &numChars);
    if (first < 0)
        return RV_ERROR_BADPARAM;
    type = internalBuffer[0];
    p = internalBuffer + next;
    next = parseList(p, 0, &first, &numChars);
    if (first < 0)
        return RV_ERROR_BADPARAM;

    ch = p[first + numChars];
    p[first + numChars] = '\0';
    if (type == 'H')
    {
        sscanf(p+first, "%d %d %d", &country, &extension, &manufacture);
        param->info.objectLength = 0;
        param->info.t35CountryCode = (RvUint8)country;
        param->info.t35Extension = (RvUint8)extension;
        param->info.manufacturerCode = (RvUint16)manufacture;
    }
    if (type == 'O')
    {
        memcpy(param->info.object, p+first, (RvSize_t)numChars);
        param->info.objectLength = numChars;
        param->info.object[numChars] = 0;
    }
    p[first+numChars] = ch;

    /* Set the data */
    if (next < 0)
    {
        param->data = NULL;
        param->length = 0;
    }
    else
    {
        param->data = (char *)string + (p - internalBuffer) + next;
        param->length = (int)strlen(param->data);
    }

    return RV_OK;
}


const RvChar *CMNonStandardParam2String(IN cmNonStandardParam *param)
{
    char* ptr = internalBuffer;
    int len;

    if (param->length < 0)
        return "";

    if (param->info.objectLength <= 0)
    {
        ptr += sprintf(ptr, "H221 {%d %d %d} ",
                       param->info.t35CountryCode,
                       param->info.t35Extension,
                       param->info.manufacturerCode);
    }
    else
    {
        param->info.object[param->info.objectLength] = '\0';
        ptr += sprintf(ptr, "Object { %s } ", param->info.object);
    }

    /* Add the data */
    len = param->length;
    if (len > (int)sizeof(internalBuffer)-128)
        len = sizeof(internalBuffer)-128;
    memcpy(ptr, param->data, (RvSize_t)len);
    ptr[len] = '\0';

    return internalBuffer;
}


RvStatus String2CMAlternateGatekeeper(IN const RvChar *string, OUT cmAlternateGatekeeper *param)
{
    char addr[100];
    int needToReg, priority;
    int status;
    char * GkId = &internalBuffer[0];
    char * GkIdBmp = &internalBuffer[512];

    if (sscanf(string, "%s %s %d %d", addr, GkId, &needToReg, &priority) != 4)
        return RV_ERROR_BADPARAM;

    status = cmString2TransportAddress(addr, &param->rasAddress);
    if (status < 0) return status;
    utlChr2Bmp(GkId, (RvUint8*)GkIdBmp);

    param->gatekeeperIdentifier.type = cmAliasTypeGatekeeperID;
    param->gatekeeperIdentifier.string = GkIdBmp;
    param->gatekeeperIdentifier.length = (RvUint16)(2*strlen(GkId));

    param->needToRegister = (RvBool)needToReg;
    param->priority = (RvUint8)priority;

    return RV_OK;
}


const RvChar *CMAlternateGatekeeper2String(IN cmAlternateGatekeeper *param)
{
    cmTransportAddress2String(&param->rasAddress, 64, internalBuffer); /* Assume the string we got is long enough */

    sprintf(internalBuffer, " %s %d %d", param->gatekeeperIdentifier.string, param->needToRegister, param->priority);

    return internalBuffer;
}


RvStatus String2Alias(IN EpObj *ep, IN const RvChar *str, OUT cmAlias *alias)
{
    const RvChar* pStr, *partyNumDigits, *hlp;
    cmTransportAddress transportAddress;

    memset(alias, 0, sizeof(*alias));

    /* Check what kind of alias we have */
    if (strncmp("TEL", str, 3) == 0)
        alias->type = cmAliasTypeE164;
    else if (strncmp("NAME", str, 4) == 0)
        alias->type = cmAliasTypeH323ID;
    else if (strncmp("URL", str, 3) == 0)
        alias->type = cmAliasTypeURLID;
    else if (strncmp("TNAME", str, 5) == 0)
        alias->type = cmAliasTypeTransportAddress;
    else if (strncmp("EMAIL", str, 5) == 0)
        alias->type = cmAliasTypeEMailID;
    else if (strncmp("PN", str, 2) == 0)
        alias->type = cmAliasTypePartyNumber;
    else
        return RV_ERROR_UNKNOWN;

    /* Find the position of the alias */
    pStr = strchr(str, ':');
    if (pStr == NULL) return RV_ERROR_UNKNOWN;

    /* Skip ":" */
    pStr++;

    switch (alias->type)
    {
    case cmAliasTypeH323ID:
        {
            int i;
            alias->length = (RvUint16)(strlen(pStr)*2);
            alias->string = (char *)ep->cb.epMalloc(ep, alias->length, __FILE__, __LINE__);
            for (i = 0; i < (int)strlen(pStr); i++)
            {
                alias->string[i*2] = '\0';
                alias->string[i*2+1] = pStr[i];
            }
            break;
        }
    case cmAliasTypeE164:
            {
            alias->length = (RvUint16)strlen(pStr);
            alias->string = (char *)ep->cb.epMalloc(ep, alias->length+1, __FILE__, __LINE__);
            strcpy(alias->string , pStr);
            break;
        }
    case cmAliasTypeURLID:
            {
            alias->length = (RvUint16)strlen(pStr);
            alias->string = (char *)ep->cb.epMalloc(ep, alias->length+1, __FILE__, __LINE__);
            strcpy(alias->string , pStr);
            break;
        }
    case cmAliasTypeEMailID:
        {
            alias->length = (RvUint16)strlen(pStr);
            alias->string = (char *)ep->cb.epMalloc(ep, alias->length+1, __FILE__, __LINE__);
            strcpy(alias->string , pStr);
            break;
        }
    case cmAliasTypeTransportAddress:
        {
            cmString2TransportAddress(pStr, &transportAddress);
            memcpy(&(alias->transport), &transportAddress, sizeof(cmTransportAddress));
            break;
        }
    case cmAliasTypePartyNumber:
        {
            partyNumDigits = strchr(pStr, '$');
            hlp = partyNumDigits;
            partyNumDigits++;
            alias->length = (RvUint16)(hlp - pStr);
            alias->string = (char *)ep->cb.epMalloc(ep, alias->length+1, __FILE__, __LINE__);
            memcpy(alias->string, pStr, (RvSize_t)alias->length);
            alias->pnType = String2PartyNumber(partyNumDigits);
            break;
        }
    default:
        break;
    }

    return RV_OK;
}


RvStatus FreeAlias(IN EpObj *ep, IN cmAlias *alias)
{
    RvStatus status = RV_OK;

    if (alias->string != NULL)
    {
        status = ep->cb.epFree(ep, alias->string);
        alias->string = NULL;
    }

    return status;
}


const RvChar *Alias2String(IN cmAlias *alias, OUT RvChar *string)
{
    int i;

    if (string == NULL)
        string = internalBuffer;
    string[0] = '\0';

    if (alias != NULL)
    {
        switch (alias->type)
        {
        case cmAliasTypeE164:
            {
                strcpy(string, "TEL:");
                for (i = 0; i < (int)alias->length; i++)
                    string[i+4] = alias->string[i];
                string[i+4] = '\0';
                break;
            }
        case cmAliasTypeH323ID:
            {
                strcpy(string, "NAME:");
                for (i = 0; i < alias->length/2; i++)
                    string[i+5] = alias->string[i*2+1];
                string[i+5] = '\0';
                break;
            }
        case cmAliasTypeURLID:
            {
                strcpy(string, "URL:");
                for (i = 0; i < alias->length; i++)
                    string[i+4] = alias->string[i];
                string[i+4] = '\0';
                break;
            }
        case cmAliasTypeEMailID:
            {
                strcpy(string, "EMAIL:");
                for (i = 0; i < alias->length; i++)
                    string[i+6] = alias->string[i];
                string[i+6] = '\0';
                break;
            }
        case cmAliasTypeTransportAddress:
            {
                strcpy(string, "TNAME:");
                cmTransportAddress2String(&(alias->transport), 60, &string[6]);
                break;
            }
        case cmAliasTypePartyNumber:
            {
                char tmpString[128];
                for (i = 0; i < alias->length; i++)
                    tmpString[i] = alias->string[i];
                tmpString[i] = '\0';
                sprintf(string, "PN:%s$%s", tmpString, PartyNumber2String(alias->pnType));
                break;
            }
        case cmAliasTypeEndpointID:
        case cmAliasTypeGatekeeperID:
            {
                for (i = 0; i < alias->length/2; i++)
                    string[i] = alias->string[i*2+1];
                string[i] = '\0';
                break;
            }
        default:
            break;
        }
    }
    return string;
}






/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * hex2byte
 * ----------------------------------------------------------------------------
 * General: Convert a hex character to a byte value.
 *
 * Return Value: Byte value.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ch - Character to convert.
 * Output: None.
 *****************************************************************************/
static RvUint8 hex2byte(
    IN RvChar   ch)
{
    switch (tolower(ch))
    {
        case '0': return 0x0;
        case '1': return 0x1;
        case '2': return 0x2;
        case '3': return 0x3;
        case '4': return 0x4;
        case '5': return 0x5;
        case '6': return 0x6;
        case '7': return 0x7;
        case '8': return 0x8;
        case '9': return 0x9;
        case 'a': return 0xa;
        case 'b': return 0xb;
        case 'c': return 0xc;
        case 'd': return 0xd;
        case 'e': return 0xe;
        case 'f': return 0xf;
        default:
            return 0xff;
    }
}


/******************************************************************************
 * string2Enum
 * ----------------------------------------------------------------------------
 * General: Convert a string value to its enumeration value.
 *          Used as a general conversion function.
 *
 * Return Value: Parameter enumeration value.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  string     - String parameter to convert.
 *         cvTable    - Conversion table to use.
 * Output: None.
 *****************************************************************************/
static RvInt32 string2Enum(
    IN const RvChar *string,
    IN ConvertValue *cvTable)
{
    ConvertValue* val = cvTable;

    while (val->strValue != NULL)
    {
        if (strcmp(string, val->strValue) == 0)
            return val->enumValue;
        val++;
    }

    return -1;
}


/******************************************************************************
 * enum2String
 * ----------------------------------------------------------------------------
 * General: Convert an enumeration value into a string.
 *          Used as a general conversion function.
 *
 * Return Value: Parameter enumeration value.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  value      - Enumeration value to convert.
 *         cvTable    - Conversion table to use.
 * Output: None.
 *****************************************************************************/
static const RvChar* enum2String(
    IN RvInt32      value,
    IN ConvertValue *cvTable)
{
    ConvertValue* val = cvTable;

    while (val->strValue != NULL)
    {
        if (value == val->enumValue)
            return val->strValue;
        val++;
    }

    return "-unknown-";
}


/******************************************************************************
 * parseList
 * ----------------------------------------------------------------------------
 * General: Parse a TCL list, getting an element from it.
 *
 * Return Value: The offset of the next element.
 *               -1 is returned if there's no next element.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  string       - String to parse.
 *         from         - Character to begin parsing from. (0-based index).
 * Output: firstChar    - First character in the string of the parsed element.
 *                        -1 is returned if couldn't find an element.
 *         numChars     - Number of characters in parsed element.
 *                        0 is returned if couldn't find an element.
 *****************************************************************************/
static int parseList(
    IN  const RvChar    *string,
    IN  int             from,
    OUT int             *firstChar,
    OUT int             *numChars)
{
    const RvChar* fromPtr;
    const RvChar* ptr;
    int anyBrackets = 0;
    int brackets = 0;
    ptr = string + from;

    /* Skip blanks */
    while (isspace((int)*ptr))
        ptr++;

    *firstChar = -1;
    *numChars = 0;

    if (*ptr == '\0')
        return -1;

    /* See if we reached a list */
    if (*ptr == '{')
    {
        brackets = 1;
        anyBrackets = 1;
        ptr++;
        *firstChar = ptr - string;
    }
    else
        *firstChar = ptr - string;
    fromPtr = ptr;

    while ((*ptr != '\0') && (!isspace((int)*ptr) || (brackets > 0)))
    {
        if (*ptr == '{')
            brackets++;
        else if (*ptr == '}')
            brackets--;
        ptr++;
    }

    *numChars = (ptr - fromPtr);
    if (anyBrackets)
        (*numChars)--;

    /* Skip blanks again */
    while (isspace((int)*ptr))
        ptr++;

    if (*ptr == '\0')
        return -1;
    else
        return ptr - string;
}


static ConvertValue cvPartyNumber[] = {
        {"PUU",     (RvInt32)cmPartyNumberPublicUnknown},
        {"PUI",     (RvInt32)cmPartyNumberPublicInternationalNumber},
        {"PUN",     (RvInt32)cmPartyNumberPublicNationalNumber},
        {"PUNS",    (RvInt32)cmPartyNumberPublicNetworkSpecificNumber},
        {"PUS",     (RvInt32)cmPartyNumberPublicSubscriberNumber},
        {"PUA",     (RvInt32)cmPartyNumberPublicAbbreviatedNumber},
        {"D",       (RvInt32)cmPartyNumberDataPartyNumber},
        {"T",       (RvInt32)cmPartyNumberTelexPartyNumber},
        {"PRU",     (RvInt32)cmPartyNumberPrivateUnknown},
        {"PRL2",    (RvInt32)cmPartyNumberPrivateLevel2RegionalNumber},
        {"PRL1",    (RvInt32)cmPartyNumberPrivateLevel1RegionalNumber},
        {"PRP",     (RvInt32)cmPartyNumberPrivatePISNSpecificNumber},
        {"PRL",     (RvInt32)cmPartyNumberPrivateLocalNumber},
        {"PRA",     (RvInt32)cmPartyNumberPrivateAbbreviatedNumber},
        {"N",       (RvInt32)cmPartyNumberNationalStandardPartyNumber},
        {NULL, -1}
};

cmPartyNumberType String2PartyNumber(IN const RvChar *string)
{
    return (cmPartyNumberType)string2Enum(string, cvPartyNumber);
}

const RvChar *PartyNumber2String(IN cmPartyNumberType value)
{
    return enum2String((RvInt32)value, cvPartyNumber);
}




#ifdef __cplusplus
}
#endif
