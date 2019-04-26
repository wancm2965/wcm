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
#include "pvaltreeStackApi.h"
#include "q931asn1.h"
#include "cmintr.h"
#include "cmCall.h"
#include "cmdebprn.h"
#include "cmutils.h"
#include "stkutils.h"
#include "cm.h"
#include "rasdef.h"

#ifdef __cplusplus
extern "C" {
#endif


void getGoodAddressForCall(HCALL hCall, cmTransportAddress* ta);

typedef enum
{
    placeQ931,
    placePER_Gen,
    placePER_Specific
} paramPlace;

static  fieldNames callTypeNames[]=
{
    {/*cmCallTypeP2P*/              __q931(pointToPoint)},
    {/*cmCallTypeOne2N*/            __q931(oneToN)},
    {/*cmCallTypeN2One*/            __q931(nToOne)},
    {/*cmCallTypeN2Nw*/             __q931(nToN)},
    {LAST_TOKEN}
};

static  fieldNames endpointTypeNames[]=
{
    {/*cmEndpointTypeTerminal*/         __q931(terminal)},
    {/*cmEndpointTypeGateway*/          __q931(gateway)},
    {/*cmEndpointTypeMCU*/          __q931(mcu)},
    {/*cmEndpointTypeGK*/           __q931(gatekeeper)},
    {/*cmEndpointTypeSET*/          __q931(set)},
    {LAST_TOKEN}
};

static  fieldNames conferenceGoalNames[]=
{
    {/*cmCreate*/                       __q931(create)},
    {/*cmJoin*/                         __q931(join)},
    {/*cmInvite*/                       __q931(invite)},
    {/*cmCapabilityNegotiation*/        __q931(capability_negotiation)},
    {/*cmCallIndependentSupplementaryService*/__q931(callIndependentSupplementaryService)},
    {LAST_TOKEN}
};

static  fieldNames reasonNames[]=
{
    {/*cmReasonTypeNoBandwidth*/                __q931(noBandwidth)},
    {/*cmReasonTypeGatekeeperResource*/         __q931(gatekeeperResources)},
    {/*cmReasonTypeUnreachableDestination*/     __q931(unreachableDestination)},
    {/*cmReasonTypeDestinationRejection*/       __q931(destinationRejection)},
    {/*cmReasonTypeInvalidRevision*/            __q931(invalidRevision)},
    {/*cmReasonTypeNoPermision*/                __q931(noPermission)},
    {/*cmReasonTypeUnreachableGatekeeper*/      __q931(unreachableGatekeeper)},
    {/*cmReasonTypeGatewayResource*/            __q931(gatewayResources)},
    {/*cmReasonTypeBadFormatAddress*/           __q931(badFormatAddress)},
    {/*cmReasonTypeAdaptiveBusy*/               __q931(adaptiveBusy)},
    {/*cmReasonTypeInConf*/                     __q931(inConf)},
    {/*cmReasonTypeUndefinedReason*/            __q931(undefinedReason)},
    {/*cmReasonTypeRouteCallToGatekeeper*/      __q931(routeCallToGatekeeper)},
    {/*cmReasonTypeCallForwarded*/              __q931(callForwarded)},
    {/*cmReasonTypeRouteCallToMC*/              __q931(routeCallToMC)},
    {/*cmReasonTypeFacilityCallDeflection*/     __q931(facilityCallDeflection)},
    {/*cmReasonTypeSecurityDenied*/             __q931(securityDenied)},
    {/*cmReasonTypeCalledPartyNotRegistered*/   __q931(calledPartyNotRegistered)},
    {/*cmReasonTypeCallerNotregistered*/        __q931(callerNotRegistered)},
    {/*cmReasonTypeConferenceListChoice*/       __q931(conferenceListChoice)},
    {/*cmReasonTypeStartH245*/                  __q931(startH245)},
    {/*cmReasonTypeNewConnectionNeeded*/        __q931(newConnectionNeeded)},
    {/*cmReasonTypeNoH245*/                     __q931(noH245)},
    {/*cmReasonTypeNewTokens*/                  __q931(newTokens)},
    {/*cmReasonTypeFeatureSetUpdate*/           __q931(featureSetUpdate)},
    {/*cmReasonTypeForwardedElements*/          __q931(forwardedElements)},
    {/*cmReasonTypeTransportedInformation*/     __q931(transportedInformation)},
    {/*cmReasonTypeNeededFeatureNotSupported*/  __q931(neededFeatureNotSupported)},
    {/*cmReasonTypeHopCountExceeded*/           __q931(hopCountExceeded)},
    {LAST_TOKEN}
};

fieldNames* getParamFieldName(
           IN  cmCallParam  param)
{
    switch(param)
    {
    case cmParamSourceInfo          :
    case cmParamDestinationInfo     :return endpointTypeNames;
    case cmParamCallType            :return callTypeNames;
    case cmParamConferenceGoal      :return conferenceGoalNames;
    case cmParamReleaseCompleteReason   :
    case cmParamFacilityReason      :return reasonNames;
    default                 :return NULL;
    }
}

static int getParamFieldNameSize(
           IN  cmCallParam  param)
{
    switch(param)
    {
    case cmParamSourceInfo          :
    case cmParamDestinationInfo     :return sizeof(endpointTypeNames);
    case cmParamCallType            :return sizeof(callTypeNames);
    case cmParamConferenceGoal      :return sizeof(conferenceGoalNames);
    case cmParamReleaseCompleteReason   :
    case cmParamFacilityReason      :return sizeof(reasonNames);
    default                 :return RV_ERROR_UNKNOWN;
    }
}



/************************************************************************
 * initParamSyntax
 * purpose: Initialize any information related to PST that is needed to
 *          hold parameters without a properties database.
 * input  : app         - Stack instance handle
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
int initParamSyntax(IN cmElem*  app)
{
    int i;
    static const char* synNames[] = {
        "Endpoint.aliasAddress",
        "AliasAddress",
        "CalledPartyNumber",
        "CalledPartySubaddress",
        "CallingPartyNumber",
        "CallingPartySubaddress",
        "EndpointType",
        "Setup-UUIE.conferenceGoal",
        "CallType",
        "H323-UserInformation.user-data",
        "NonStandardParameter",
        "ReleaseCompleteReason",
        "FacilityReason"
    };

    for (i = 0; i < (int)cmParamSynLast; i++)
    {
        app->synParams[i] = pstConstruct(cmEmGetQ931Syntax(), synNames[i]);
        if (app->synParams[i] == NULL)
            return RV_ERROR_UNKNOWN;
    }
    return 0;
}


/************************************************************************
 * endParamSyntax
 * purpose: Deinitialize any information related to PST that is needed to
 *          hold parameters without a properties database.
 * input  : app         - Stack instance handle
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
int endParamSyntax(IN cmElem*  app)
{
    int i;
    for (i = 0; i < (int)cmParamSynLast; i++)
    {
        if (app->synParams[i] != NULL)
            pstDestruct(app->synParams[i]);
    }
    return 0;
}



/************************************************************************
 * getParamMessage
 * purpose: Return the message the parameter belongs to
 * input  : param   - Call parameter
 * output : none
 * return : Message fieldID value on success
 *          Negative value on failure
 ************************************************************************/
static RvPstFieldId getParamMessage(IN  cmCallParam       param)
{
    switch (param)
    {
        case cmParamSourceAddress          :
        case cmParamDestinationAddress     :
        case cmParamFullSourceAddress      :
        case cmParamFullDestinationAddress :
        case cmParamDestExtraCallInfo      :
        case cmParamCalledPartyNumber      :
        case cmParamCalledPartySubAddress  :
        case cmParamCallingPartyNumber     :
        case cmParamCallingPartySubAddress :
        case cmParamExtension              :
        case cmParamFullSourceInfo         :
        case cmParamSourceInfo             :
        case cmParamDestCallSignalAddress  :
        case cmParamSrcCallSignalAddress   :
        case cmParamSetupH245Address       :
        case cmParamActiveMc               :
        case cmParamConferenceGoal         :
        case cmParamCallType               :
        case cmParamInformationTransferCapability :
        case cmParamDisplay                :
        case cmParamUserUser               :
        case cmParamSetupNonStandardData   :
        case cmParamSetupNonStandard       :
        case cmParamSetupFastStart         :
        case cmParamSetupCanOverlapSending :
        case cmParamSetupSendingComplete   :
        case cmParamFullDestExtraCallInfo  :
        case cmParamFullExtension          : return __q931(setup);

        case cmParamCallProcH245Address    :
        case cmParamCallProceedingNonStandardData:
        case cmParamCallProceedingNonStandard:
        case cmParamCallProcFastStart      : return __q931(callProceeding);

        case cmParamAlertingH245Address    :
        case cmParamAlertingNonStandardData:
        case cmParamAlertingNonStandard    :
        case cmParamAlertingFastStart      : return __q931(alerting);

        case cmParamH245Address            :
        case cmParamConnectDisplay         :
        case cmParamConnectUserUser        :
        case cmParamConnectNonStandardData :
        case cmParamConnectNonStandard     :
        case cmParamConnectFastStart       :
        case cmParamConnectedAddress       : return __q931(connect);

        case cmParamReleaseCompleteReason  :
        case cmParamReleaseCompleteCause   :
        case cmParamReleaseCompleteNonStandardData:
        case cmParamReleaseCompleteNonStandard
                                           : return __q931(releaseComplete);

        case cmParamAlternativeAddress     :
        case cmParamAlternativeAliasAddress:
        case cmParamAlternativeDestExtraCallInfo:
        case cmParamAlternativeExtension   :
        case cmParamFacilityCID            :
        case cmParamFacilityReason         : return __q931(facility);
        default                            : break;
    }
    return RV_ERROR_UNKNOWN;
}



/************************************************************************
 * getParamPlace
 * purpose: Return the place inside the Q931 message that the parameter
 *          belongs to. This function shortens the amount of PVT nodes that
 *          are accessed when building a message from scratch if the
 *          property DB of the call is not used.
 * input  : param   - Call parameter to check
 * output : none
 * return : Place of parameter in message on success
 *          Negative value on failure
 ************************************************************************/
static paramPlace getParamPlace(IN  cmCallParam       param)
{
    switch(param)
    {
        case cmParamSetupSendingComplete   :
        case cmParamInformationTransferCapability :

        case cmParamDisplay                :
        case cmParamCalledPartyNumber      :
        case cmParamCalledPartySubAddress  :
        case cmParamCallingPartyNumber     :
        case cmParamCallingPartySubAddress :
        case cmParamConnectDisplay         :
        case cmParamReleaseCompleteCause   :
                                             return placeQ931;
        case cmParamSourceAddress          :
        case cmParamDestinationAddress     :
        case cmParamFullSourceAddress      :
        case cmParamFullDestinationAddress :
        case cmParamDestExtraCallInfo      :
        case cmParamExtension              :

        case cmParamFullSourceInfo         :
        case cmParamSourceInfo             :


        case cmParamDestCallSignalAddress  :
        case cmParamSrcCallSignalAddress   :
        case cmParamSetupH245Address       :

        case cmParamActiveMc               :
        case cmParamConferenceGoal         :
        case cmParamCallType               :

        case cmParamSetupFastStart         :
        case cmParamSetupCanOverlapSending :

        case cmParamCallProcFastStart      :


        case cmParamAlertingH245Address    :
        case cmParamCallProcH245Address    :
        case cmParamAlertingFastStart      :
        case cmParamH245Address            :
        case cmParamConnectFastStart       :
        case cmParamReleaseCompleteReason  :
        case cmParamAlternativeAddress     :
        case cmParamAlternativeAliasAddress:
        case cmParamAlternativeDestExtraCallInfo
                                           :
        case cmParamAlternativeExtension   :
        case cmParamFacilityCID            :
        case cmParamFacilityReason         :
        case cmParamConnectedAddress       : return placePER_Specific;

        case cmParamUserUser               :

        case cmParamSetupNonStandardData   :
        case cmParamSetupNonStandard       :

        case cmParamCallProceedingNonStandardData:
        case cmParamCallProceedingNonStandard:


        case cmParamAlertingNonStandardData:
        case cmParamAlertingNonStandard    :
        case cmParamConnectUserUser        :
        case cmParamConnectNonStandardData :
        case cmParamConnectNonStandard     :
        case cmParamReleaseCompleteNonStandardData:
        case cmParamReleaseCompleteNonStandard   : return placePER_Gen;

        default:break;
    }
    return (paramPlace)RV_ERROR_UNKNOWN;
}

/************************************************************************
 * getFirstMessageParameterNumber
 * purpose: Get the enumeration value of the first parameter for the given
 *          type of message.
 *          This function is used when we're not using any property DB.
 * input  : messageType - Type of message to get
 * output : none
 * return : First parameter enumerataion value for the given message type on success
 *          Negative value on failure
 ************************************************************************/
static cmCallParamEnum getFirstMessageParameterNumber(cmCallQ931MsgType messageType)
{
    switch(messageType)
    {
        case cmQ931setup            : return cmEnumSetupSendingComplete;
        case cmQ931callProceeding   : return cmEnumCallProcH245Address;
        case cmQ931connect          : return cmEnumH245Address;
        case cmQ931alerting         : return cmEnumAlertingH245Address;
        case cmQ931releaseComplete  : return cmEnumReleaseCompleteReason;
        case cmQ931facility         : return cmEnumAlternativeAddress;
        default                     : break;
    }
    return (cmCallParamEnum)RV_ERROR_UNKNOWN;
}


/************************************************************************
 * getParamSyntax
 * purpose: Get the syntax tree to use for a specific parameter
 *          enumeration.
 *          This function is used when we're not using any property DB.
 * input  : app         - Stack instance to use
 *          paramNumber - Property enumeration to convert
 * output : none
 * return : Syntax tree handle on success
 *          NULL on failure
 ************************************************************************/
static HPST getParamSyntax(
    IN  cmElem*         app,
    IN  cmCallParamEnum paramNumber)
{
    switch (paramNumber)
    {
        /* Simple types can be handled with any syntax. They will get their syntax
           when we add the parameters themselves to the outgoing messages. Since they
           are the root values, the syntax won't help - so we just ignore it... */

        /* Boolean */
        case cmEnumSetupSendingComplete             :
        case cmEnumActiveMc                         :
        case cmEnumSetupCanOverlapSending           : return app->synProtQ931;

        /* Integer */
        case cmEnumInformationTransferCapability    :
        case cmEnumReleaseCompleteCause             : return app->synProtQ931;

        /* String */
        case cmEnumDisplay                          :
        case cmEnumSetupFastStart                   :
        case cmEnumCallProcFastStart                :
        case cmEnumAlertingFastStart                :
        case cmEnumConnectDisplay                   :
        case cmEnumConnectFastStart                 :
        case cmEnumFacilityCID                      : return app->synProtQ931;

        /* CalledPartyNumber */
        case cmEnumCalledPartyNumber                : return app->synParams[cmParamSynCalledPartyNumber];

        /* CalledPartySubaddress */
        case cmEnumCalledPartySubAddress            : return app->synParams[cmParamSynCalledSubaddress];

        /* CallingPartyNumber */
        case cmEnumCallingPartyNumber               : return app->synParams[cmParamSynCallingPartyNumber];

        /* CallingPartySubaddress */
        case cmEnumCallingPartySubAddress           : return app->synParams[cmParamSynCallingSubaddress];

        /* AliasAddress */
        case cmEnumDestExtraCallInfo                :
        case cmEnumAlternativeAliasAddress          :
        case cmEnumConnectConnectedAddress          :
        case cmEnumAlternativeDestExtraCallInfo     :
        case cmEnumAlternativeExtension             :
        case cmEnumFullSourceAddress                :
        case cmEnumFullDestinationAddress           : return app->synParams[cmParamSynSOAliasAddress];

        case cmEnumExtension                        : return app->synParams[cmParamSynAliasAddress];


        /* EndpointType */
        case cmEnumFullSourceInfo                   : return app->synParams[cmParamSynEndpointType];

        /* TransportAddress */
        case cmEnumDestCallSignalAddress            :
        case cmEnumSrcCallSignalAddress             :
        case cmEnumSetupH245Address                 :
        case cmEnumCallProcH245Address              :
        case cmEnumAlertingH245Address              :
        case cmEnumH245Address                      :
        case cmEnumAlternativeAddress               : return app->hAddrSyn;

        /* Setup-UUIE.conferenceGoal */
        case cmEnumConferenceGoal                   : return app->synParams[cmParamSynConferenceGoal];

        /* CallType */
        case cmEnumCallType                         : return app->synParams[cmParamSynCallType];

        /* H323-UserInformation.user-data */
        case cmEnumUserUser                         :
        case cmEnumConnectUserUser                  : return app->synParams[cmParamSynUserData];

        /* NonStandardParameter */
        case cmEnumSetupNonStandard                 :
        case cmEnumCallProceedingNonStandard        :
        case cmEnumAlertingNonStandard              :
        case cmEnumConnectNonStandard               :
        case cmEnumReleaseCompleteNonStandard       :
        case cmEnumSetupNonStandardData             :
        case cmEnumCallProceedingNonStandardData    :
        case cmEnumAlertingNonStandardData          :
        case cmEnumConnectNonStandardData           :
        case cmEnumReleaseCompleteNonStandardData   : return app->synParams[cmParamSynNonStandardParameter];

        /* ReleaseCompleteReason */
        case cmEnumReleaseCompleteReason            : return app->synParams[cmParamSynReleaseCompleteReason];

        /* FacilityReason */
        case cmEnumFacilityReason                   : return app->synParams[cmParamSynFacilityReason];

        default:break;
    }
    return NULL;
}


/************************************************************************
 * getNumberParam
 * purpose: Get the parameter enumeration to use for a given property
 *          enumeration.
 *          This function is used when we're not using any property DB.
 * input  : paramNumber - Property enumeration to convert
 * output : none
 * return : Call parameter on success
 *          Negative value on failure
 ************************************************************************/
static cmCallParam getNumberParam(IN  cmCallParamEnum       paramNumber)
{
    switch(paramNumber)
    {
        case cmEnumSetupSendingComplete             : return cmParamSetupSendingComplete;
        case cmEnumInformationTransferCapability    : return cmParamInformationTransferCapability;
        case cmEnumDisplay                          : return cmParamDisplay;
        case cmEnumCalledPartyNumber                : return cmParamCalledPartyNumber;
        case cmEnumCalledPartySubAddress            : return cmParamCalledPartySubAddress;
        case cmEnumCallingPartyNumber               : return cmParamCallingPartyNumber;
        case cmEnumCallingPartySubAddress           : return cmParamCallingPartySubAddress;
        case cmEnumFullSourceAddress                : return cmParamFullSourceAddress;
        case cmEnumFullDestinationAddress           : return cmParamFullDestinationAddress;
        case cmEnumDestExtraCallInfo                : return cmParamDestExtraCallInfo;
        case cmEnumExtension                        : return cmParamExtension;

        case cmEnumFullSourceInfo                   : return cmParamFullSourceInfo;

        case cmEnumDestCallSignalAddress            : return cmParamDestCallSignalAddress;
        case cmEnumSrcCallSignalAddress             : return cmParamSrcCallSignalAddress;
        case cmEnumSetupH245Address                 : return cmParamSetupH245Address;

        case cmEnumActiveMc                         : return cmParamActiveMc;
        case cmEnumConferenceGoal                   : return cmParamConferenceGoal;
        case cmEnumCallType                         : return cmParamCallType;

        case cmEnumUserUser                         : return cmParamUserUser;

        case cmEnumSetupNonStandardData             : return cmParamSetupNonStandardData;
        case cmEnumSetupNonStandard                 : return cmParamSetupNonStandard;

        case cmEnumSetupFastStart                   : return cmParamSetupFastStart;
        case cmEnumSetupCanOverlapSending           : return cmParamSetupCanOverlapSending;

        case cmEnumCallProcH245Address              : return cmParamCallProcH245Address;
        case cmEnumCallProceedingNonStandardData    : return cmParamCallProceedingNonStandardData;
        case cmEnumCallProceedingNonStandard        : return cmParamCallProceedingNonStandard;

        case cmEnumCallProcFastStart                : return cmParamCallProcFastStart;


        case cmEnumAlertingH245Address              : return cmParamAlertingH245Address;
        case cmEnumAlertingNonStandardData          : return cmParamAlertingNonStandardData;
        case cmEnumAlertingNonStandard              : return cmParamAlertingNonStandard;
        case cmEnumAlertingFastStart                : return cmParamAlertingFastStart;


        case cmEnumH245Address                      : return cmParamH245Address;
        case cmEnumConnectDisplay                   : return cmParamConnectDisplay;
        case cmEnumConnectUserUser                  : return cmParamConnectUserUser;
        case cmEnumConnectNonStandardData           : return cmParamConnectNonStandardData;
        case cmEnumConnectNonStandard               : return cmParamConnectNonStandard;
        case cmEnumConnectFastStart                 : return cmParamConnectFastStart;
        case cmEnumConnectConnectedAddress          : return cmParamConnectedAddress;

        case cmEnumReleaseCompleteReason            : return cmParamReleaseCompleteReason;
        case cmEnumReleaseCompleteCause             : return cmParamReleaseCompleteCause;
        case cmEnumReleaseCompleteNonStandardData   : return cmParamReleaseCompleteNonStandardData;
        case cmEnumReleaseCompleteNonStandard       : return cmParamReleaseCompleteNonStandard;


        case cmEnumAlternativeAddress               : return cmParamAlternativeAddress;
        case cmEnumAlternativeAliasAddress          : return cmParamAlternativeAliasAddress;
        case cmEnumAlternativeDestExtraCallInfo     : return cmParamAlternativeDestExtraCallInfo;
        case cmEnumAlternativeExtension             : return cmParamAlternativeExtension;
        case cmEnumFacilityCID                      : return cmParamFacilityCID;
        case cmEnumFacilityReason                   : return cmParamFacilityReason;
        default:break;
    }
    return (cmCallParam)RV_ERROR_UNKNOWN;
}


static cmCallParamEnum getParamNumber(IN  cmCallParam       param)
{
    cmCallParamEnum  num=(cmCallParamEnum)RV_ERROR_UNKNOWN;
    switch(param)
    {
        case cmParamSetupSendingComplete            : return cmEnumSetupSendingComplete;
        case cmParamInformationTransferCapability   : return cmEnumInformationTransferCapability;
        case cmParamDisplay                         : return cmEnumDisplay;
        case cmParamCalledPartyNumber               : return cmEnumCalledPartyNumber;
        case cmParamCalledPartySubAddress           : return cmEnumCalledPartySubAddress;
        case cmParamCallingPartyNumber              : return cmEnumCallingPartyNumber;
        case cmParamCallingPartySubAddress          : return cmEnumCallingPartySubAddress;
        case cmParamSourceAddress                   :
        case cmParamFullSourceAddress               : return cmEnumFullSourceAddress;
        case cmParamDestinationAddress              :
        case cmParamFullDestinationAddress          : return cmEnumFullDestinationAddress;
        case cmParamDestExtraCallInfo               : return cmEnumDestExtraCallInfo;
        case cmParamExtension                       : return cmEnumExtension;

        case cmParamSourceInfo                      :
        case cmParamFullSourceInfo                  : return cmEnumFullSourceInfo;


        case cmParamDestCallSignalAddress           : return cmEnumDestCallSignalAddress;
        case cmParamSrcCallSignalAddress            : return cmEnumSrcCallSignalAddress;
        case cmParamSetupH245Address                : return cmEnumSetupH245Address;

        case cmParamActiveMc                        : return cmEnumActiveMc;
        case cmParamConferenceGoal                  : return cmEnumConferenceGoal;
        case cmParamCallType                        : return cmEnumCallType;

        case cmParamUserUser                        : return cmEnumUserUser;

        case cmParamSetupNonStandardData            : return cmEnumSetupNonStandardData;
        case cmParamSetupNonStandard                : return cmEnumSetupNonStandard;

        case cmParamSetupFastStart                  : return cmEnumSetupFastStart;
        case cmParamSetupCanOverlapSending          : return cmEnumSetupCanOverlapSending;

        case cmParamCallProcH245Address             : return cmEnumCallProcH245Address;
        case cmParamCallProceedingNonStandardData   : return cmEnumCallProceedingNonStandardData;
        case cmParamCallProceedingNonStandard       : return cmEnumCallProceedingNonStandard;

        case cmParamCallProcFastStart               : return cmEnumCallProcFastStart;


        case cmParamAlertingH245Address             : return cmEnumAlertingH245Address;
        case cmParamAlertingNonStandardData         : return cmEnumAlertingNonStandardData;
        case cmParamAlertingNonStandard             : return cmEnumAlertingNonStandard;
        case cmParamAlertingFastStart               : return cmEnumAlertingFastStart;


        case cmParamH245Address                     : return cmEnumH245Address;
        case cmParamConnectDisplay                  : return cmEnumConnectDisplay;
        case cmParamConnectUserUser                 : return cmEnumConnectUserUser;
        case cmParamConnectNonStandardData          : return cmEnumConnectNonStandardData;
        case cmParamConnectNonStandard              : return cmEnumConnectNonStandard;
        case cmParamConnectFastStart                : return cmEnumConnectFastStart;
        case cmParamConnectedAddress                : return cmEnumConnectConnectedAddress;


        case cmParamReleaseCompleteReason           : return cmEnumReleaseCompleteReason;
        case cmParamReleaseCompleteCause            : return cmEnumReleaseCompleteCause;
        case cmParamReleaseCompleteNonStandardData  : return cmEnumReleaseCompleteNonStandardData;
        case cmParamReleaseCompleteNonStandard      : return cmEnumReleaseCompleteNonStandard;


        case cmParamAlternativeAddress              : return cmEnumAlternativeAddress;
        case cmParamAlternativeAliasAddress         : return cmEnumAlternativeAliasAddress;
        case cmParamAlternativeDestExtraCallInfo    : return cmEnumAlternativeDestExtraCallInfo;
        case cmParamAlternativeExtension            : return cmEnumAlternativeExtension;
        case cmParamFacilityCID                     : return cmEnumFacilityCID;
        case cmParamFacilityReason                  : return cmEnumFacilityReason;
        default:break;
    }
    return num;

}


static RvInt16 *getParamShortName(
                          IN  cmCallParam       param
                          )
{
    switch(param)
    {
        case cmParamSetupSendingComplete   : { static RvInt16 path[] = {__q931(sendingComplete),LAST_TOKEN} ; return path; }
        case cmParamInformationTransferCapability :
                                             { static RvInt16 path[] =
                                                    {__q931(bearerCapability),__q931(octet3),__q931(informationTransferCapability),LAST_TOKEN} ; return path; }
        case cmParamCalledPartyNumber      : { static RvInt16 path[] = {__q931(calledPartyNumber),LAST_TOKEN} ; return path; }
        case cmParamCalledPartySubAddress  : { static RvInt16 path[] = {__q931(calledPartySubaddress),LAST_TOKEN} ; return path; }
        case cmParamCallingPartyNumber     : { static RvInt16 path[] = {__q931(callingPartyNumber),LAST_TOKEN} ; return path; }
        case cmParamCallingPartySubAddress : { static RvInt16 path[] = {__q931(callingPartySubaddress),LAST_TOKEN} ; return path; }
        case cmParamDisplay                :
        case cmParamConnectDisplay         : { static RvInt16 path[] = {__q931(display),LAST_TOKEN} ; return path; }
        case cmParamReleaseCompleteCause   : { static RvInt16 path[] = {__q931(cause),__q931(octet4),__q931(causeValue),LAST_TOKEN} ; return path; }


        case cmParamUserUser               :
        case cmParamConnectUserUser        : { static RvInt16 path[] = {__q931(user_data),LAST_TOKEN} ; return path; }

        case cmParamReleaseCompleteNonStandardData:
        case cmParamReleaseCompleteNonStandard   :
        case cmParamConnectNonStandardData :
        case cmParamConnectNonStandard     :

        case cmParamSetupNonStandardData   :
        case cmParamSetupNonStandard       :

        case cmParamCallProceedingNonStandardData:
        case cmParamCallProceedingNonStandard
                                           :
        case cmParamAlertingNonStandardData:
        case cmParamAlertingNonStandard    : { static RvInt16 path[] = {__q931(h323_uu_pdu),__q931(nonStandardData),LAST_TOKEN}; return path; }


        case cmParamSourceAddress          : { static RvInt16 path[] = {__q931(sourceAddress),LAST_TOKEN} ; return path; }
        case cmParamDestinationAddress     : { static RvInt16 path[] = {__q931(destinationAddress),LAST_TOKEN} ; return path; }
        case cmParamFullSourceAddress      : { static RvInt16 path[] = {__q931(sourceAddress),LAST_TOKEN} ; return path; }
        case cmParamFullDestinationAddress : { static RvInt16 path[] = {__q931(destinationAddress),LAST_TOKEN} ; return path; }
        case cmParamDestExtraCallInfo      : { static RvInt16 path[] = {__q931(destExtraCallInfo),LAST_TOKEN} ; return path; }
        case cmParamExtension              : { static RvInt16 path[] = {__q931(remoteExtensionAddress),LAST_TOKEN} ; return path; }
        case cmParamConnectedAddress       : { static RvInt16 path[] = {__q931(connectedAddress),LAST_TOKEN}; return path; }

        case cmParamFullSourceInfo         :
        case cmParamSourceInfo             : { static RvInt16 path[] = {__q931(sourceInfo),LAST_TOKEN} ; return path; }


        case cmParamDestCallSignalAddress  : { static RvInt16 path[] = {__q931(destCallSignalAddress),LAST_TOKEN} ; return path; }
        case cmParamSrcCallSignalAddress   : { static RvInt16 path[] = {__q931(sourceCallSignalAddress),LAST_TOKEN} ; return path; }

        case cmParamActiveMc               : { static RvInt16 path[] = {__q931(activeMC),LAST_TOKEN} ; return path; }
        case cmParamConferenceGoal         : { static RvInt16 path[] = {__q931(conferenceGoal),LAST_TOKEN}; return path;  }
        case cmParamCallType               : { static RvInt16 path[] = {__q931(callType),LAST_TOKEN} ; return path; }

        case cmParamSetupCanOverlapSending : { static RvInt16 path[] = {__q931(canOverlapSend),LAST_TOKEN} ; return path; }

        case cmParamSetupH245Address       :
        case cmParamCallProcH245Address    :
        case cmParamAlertingH245Address    :
        case cmParamH245Address            : { static RvInt16 path[] = {__q931(h245Address),LAST_TOKEN} ; return path; }

        case cmParamSetupFastStart         :
        case cmParamCallProcFastStart      :
        case cmParamAlertingFastStart      :
        case cmParamConnectFastStart       : { static RvInt16 path[] = {__q931(fastStart),LAST_TOKEN} ; return path; }


        case cmParamReleaseCompleteReason  :
        case cmParamFacilityReason         : { static RvInt16 path[] = {__q931(reason),LAST_TOKEN} ; return path; }


        case cmParamAlternativeAddress     : { static RvInt16 path[] = {__q931(alternativeAddress),LAST_TOKEN} ; return path; }
        case cmParamAlternativeAliasAddress: { static RvInt16 path[] = {__q931(alternativeAliasAddress),LAST_TOKEN} ; return path; }
        case cmParamAlternativeDestExtraCallInfo
                                           : { static RvInt16 path[] = {__q931(destExtraCallInfo),LAST_TOKEN} ; return path; }
        case cmParamAlternativeExtension   : { static RvInt16 path[] = {__q931(remoteExtensionAddress),LAST_TOKEN} ; return path; }
        case cmParamFacilityCID            : { static RvInt16 path[] = {__q931(conferenceID),LAST_TOKEN} ; return path; }
        default:break;
    }
    return NULL;
}

static RvInt16 *getParamName(
                          IN  cmCallParam       param
                          )
{
    switch(param)
    {
        case cmParamSetupSendingComplete   : { static RvInt16 path[] = {__q931(message),-1,__q931(sendingComplete),LAST_TOKEN} ; return path; }
        case cmParamInformationTransferCapability :
                                             { static RvInt16 path[] = {__q931(message),-1,__q931(bearerCapability),__q931(octet3),__q931(informationTransferCapability),LAST_TOKEN} ; return path; }
        case cmParamDisplay                : { static RvInt16 path[] = {__q931(message),-1,__q931(display),LAST_TOKEN} ; return path; }
        case cmParamCalledPartyNumber      : { static RvInt16 path[] = {__q931(message),-1,__q931(calledPartyNumber),LAST_TOKEN} ; return path; }
        case cmParamCalledPartySubAddress  : { static RvInt16 path[] = {__q931(message),-1,__q931(calledPartySubaddress),LAST_TOKEN} ; return path; }
        case cmParamCallingPartyNumber     : { static RvInt16 path[] = {__q931(message),-1,__q931(callingPartyNumber),LAST_TOKEN} ; return path; }
        case cmParamCallingPartySubAddress : { static RvInt16 path[] = {__q931(message),-1,__q931(callingPartySubaddress),LAST_TOKEN} ; return path; }
        case cmParamSourceAddress          : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(sourceAddress),LAST_TOKEN} ; return path; }
        case cmParamDestinationAddress     : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(destinationAddress),LAST_TOKEN} ; return path; }
        case cmParamFullSourceAddress      : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(sourceAddress),LAST_TOKEN} ; return path; }
        case cmParamFullDestinationAddress : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(destinationAddress),LAST_TOKEN} ; return path; }
        case cmParamFullDestExtraCallInfo  :
        case cmParamDestExtraCallInfo      : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(destExtraCallInfo),LAST_TOKEN} ; return path; }
        case cmParamFullExtension          :
        case cmParamExtension              : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(remoteExtensionAddress),LAST_TOKEN} ; return path; }
        case cmParamConnectedAddress       : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(connectedAddress),LAST_TOKEN} ; return path; }

        case cmParamFullSourceInfo         :
        case cmParamSourceInfo             : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(sourceInfo),LAST_TOKEN} ; return path; }


        case cmParamDestCallSignalAddress  : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(destCallSignalAddress),LAST_TOKEN} ; return path; }
        case cmParamSrcCallSignalAddress   : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(sourceCallSignalAddress),LAST_TOKEN} ; return path; }
        case cmParamSetupH245Address       : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(h245Address),LAST_TOKEN} ; return path; }

        case cmParamActiveMc               : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(activeMC),LAST_TOKEN} ; return path; }
        case cmParamConferenceGoal         : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(conferenceGoal),LAST_TOKEN}; return path;  }
        case cmParamCallType               : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(callType),LAST_TOKEN} ; return path; }
        case cmParamUserUser               : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(user_data),LAST_TOKEN} ; return path; }

        case cmParamSetupNonStandardData   :
        case cmParamSetupNonStandard       : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(nonStandardData),LAST_TOKEN}; return path; }

        case cmParamSetupFastStart         : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(fastStart),LAST_TOKEN} ; return path; }
        case cmParamSetupCanOverlapSending : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(canOverlapSend),LAST_TOKEN} ; return path; }

        case cmParamCallProcH245Address    : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(h245Address),LAST_TOKEN} ; return path; }
        case cmParamCallProceedingNonStandardData:
        case cmParamCallProceedingNonStandard
                                           : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(nonStandardData),LAST_TOKEN}; return path; }

        case cmParamCallProcFastStart      : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(fastStart),LAST_TOKEN} ; return path; }

        case cmParamAlertingH245Address    : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(h245Address),LAST_TOKEN} ; return path; }
        case cmParamAlertingNonStandardData:
        case cmParamAlertingNonStandard    : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(nonStandardData),LAST_TOKEN}; return path;  }
        case cmParamAlertingFastStart      : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(fastStart),LAST_TOKEN} ; return path; }

        case cmParamH245Address            : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(h245Address),LAST_TOKEN} ; return path; }
        case cmParamConnectDisplay         : { static RvInt16 path[] = {__q931(message),-1,__q931(display),LAST_TOKEN} ; return path; }
        case cmParamConnectUserUser        : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(user_data),LAST_TOKEN} ; return path; }
        case cmParamConnectNonStandardData :
        case cmParamConnectNonStandard     : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(nonStandardData),LAST_TOKEN}; return path;  }
        case cmParamConnectFastStart       : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(fastStart),LAST_TOKEN} ; return path; }

        case cmParamReleaseCompleteReason  : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),-1,__q931(reason),LAST_TOKEN} ; return path; }
        case cmParamReleaseCompleteCause   : { static RvInt16 path[] = {__q931(message),-1,__q931(cause),__q931(octet4),__q931(causeValue),LAST_TOKEN} ; return path; }
        case cmParamReleaseCompleteNonStandardData:
        case cmParamReleaseCompleteNonStandard
                                           : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(nonStandardData),LAST_TOKEN}; return path; }


        case cmParamAlternativeAddress     : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),__q931(facility),__q931(alternativeAddress),LAST_TOKEN} ; return path; }
        case cmParamAlternativeAliasAddress: { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),__q931(facility),__q931(alternativeAliasAddress),LAST_TOKEN} ; return path; }
        case cmParamAlternativeDestExtraCallInfo
                                           : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),__q931(facility),__q931(destExtraCallInfo),LAST_TOKEN} ; return path; }
        case cmParamAlternativeExtension   : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),__q931(facility),__q931(remoteExtensionAddress),LAST_TOKEN} ; return path; }
        case cmParamFacilityCID            : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),__q931(facility),__q931(conferenceID),LAST_TOKEN} ; return path; }
        case cmParamFacilityReason         : { static RvInt16 path[] = {__q931(message),-1,__q931(userUser),__q931(h323_UserInformation),__q931(h323_uu_pdu),__q931(h323_message_body),__q931(facility),__q931(reason),LAST_TOKEN} ; return path; }
        default:break;
    }
    return NULL;
}

RVAPI
int RVCALLCONV cmCallSetParam(
    IN HCALL            hsCall,
    IN cmCallParam      param,
    IN int              index,
    IN RvInt32          value,
    IN const RvChar *   svalue)
{
    RvBool      simple = RV_TRUE;
    RvPvtNodeId nodeId;
    RvPvtNodeId chNodeId;
    int         ret = RV_OK;
    RvInt32     length;
    callElem    *call = (callElem*)hsCall;
    HAPP        hApp;
    cmElem*     app;
    HPVT        hVal;

    hApp = (HAPP)emaGetInstance((EMAElement)call);
    app = (cmElem*)hApp;

    if (hApp == NULL)
        return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp,"cmCallSetParam: hsCall=%p,param %d,len=%d",hsCall,param,value));

    if (!emaLock((EMAElement)hsCall))
    {
        cmiAPIExit((hApp,"cmCallSetParam [Call Was Deleted]"));
        return RV_ERROR_UNKNOWN;
    }

    hVal = app->hVal;

    switch (param)
    {
        case cmParamEstablishH245           :
        {
            RvBool establish=!value;
            m_callset(call, notEstablishControl, establish);
            cmTransSetSessionParam(call->hsTransSession, cmTransParam_notEstablishControl,&establish);
            break;
        }
        case cmParamEarlyH245               :
        {
            cmH245Stage stage=(value)?cmTransH245Stage_early:cmTransH245Stage_connect;
            m_callset(call, h245Stage, stage);
            cmTransSetSessionParam(call->hsTransSession, cmTransParam_H245Stage,&stage);
            break;
        }
        case cmParamH245Parallel:
        {
            m_callset(call, isParallelTunneling, value);
            cmTransSetSessionParam(call->hsTransSession, cmTransParam_isParallelTunnelingSupported,&value);
            if (value)
                cmTransSetSessionParam(call->hsTransSession, cmTransParam_isTunnelingSupported,&value);
            break;
        }
        case cmParamH245Tunneling:
            m_callset(call, h245Tunneling, value);
            cmTransSetSessionParam(call->hsTransSession, cmTransParam_isTunnelingSupported,&value);
            if (!value)
                cmTransSetSessionParam(call->hsTransSession, cmTransParam_isParallelTunnelingSupported,&value);
            break;
        case cmParamH245Stage:
            m_callset(call, h245Stage, (cmH245Stage)value);
            cmTransSetSessionParam(call->hsTransSession, cmTransParam_H245Stage,&value);
        break;
        case cmParamShutdownEmptyConnection:
            m_callset(call, shutdownEmptyConnection, value);
            cmTransSetSessionParam(call->hsTransSession, cmTransParam_shutdownEmptyConnection,&value);
        break;
        case cmParamIsMultiplexed:
            m_callset(call, isMultiplexed, value);
            cmTransSetSessionParam(call->hsTransSession, cmTransParam_isMultiplexed,&value);
        break;
        case cmParamAnnexE:
            call->annexE=(cmAnnexEUsageMode)value;
        break;
        case cmParamCRV:
            call->crv=value;
        break;
        case cmParamRASCRV:
            call->rascrv=value;
        break;
        case cmParamRequestedRate:
            call->newRate=value;
        break;
        case cmParamRate:
            call->rate=value;
        break;
        case cmParamMultiRate:
            m_callset(call, multiRate, value);
        break;
        case cmParamCanOverlapSending:
            m_callset(call, enableOverlapSending, value);
        break;
        case cmParamCallID:
            if (svalue != NULL)
               memcpy(call->callId,svalue,16);
        break;
        case cmParamCID:
            if (svalue != NULL)
               memcpy(call->cId,svalue,16);
        break;
        case cmParamPreGrantedArq:
            call->preGrantedArqUse=(cmPreGrantedArqUse)value;
        break;
        case cmParamSourceIpAddress:
            ret = (int)cmTransSetSessionParam(call->hsTransSession, cmTransParam_sourceIpAddress, svalue);
        break;

        case cmParamState:
        case cmParamStateMode:
            ret = RV_ERROR_BADPARAM;
        break;

        case cmParamAutomaticRAS:
            /* Can only be modified when using automatic RAS globally and when no operation was done
               on the call until now. */
            if ((call->state == cmCallStateInit) && !app->manualRAS)
            {
                m_callset(call, automaticRAS, (RvBool)value);
            }
            else
            {
                ret = RV_ERROR_BADPARAM;
            }
            break;

        case cmParamMsdTerminalType:
            m_callset(call, setTerminalType, (value >= 0));
            call->msdTerminalType = (RvUint8)value;
            break;

        case cmParamHopCount:
            call->hopCount = (RvUint8)value;
            break;

        case cmParamDisconnectionReason:
            /* Read-only parameter... */
            ret = RV_ERROR_BADPARAM;
            break;

        case cmParamExtendedFastConnectState:
                /* make sure this is a valid parameter to be set */
                if ((value < (int)cmExtendedFastConnectApproved) &&
                    (value >= (int)cmExtendedFastConnectNo))
                {
                    ret = (int) cmiSetExtendedFastConnectStatus(hsCall, (cmExtendedFastConnectState_e)value);

                    /* notify the transport layer (unless, of course, the above call returned an error) */
                    if (ret == RV_OK)
                    {
                        if (value == cmExtendedFastConnectApproved) value = efcDesire;
                        cmTransSetSessionParam(call->hsTransSession, cmTransParam_extFastConnect, &value);
                    }
                }
                else
                {
                    ret = RV_ERROR_BADPARAM;
                }
            break;

        case cmParamSupportNat:
            {

                if((app->cmNATMode == cmNatModeNoNAT && (RvBool)value) ||
                    (app->cmNATMode == cmNatModeOnlyNAT && !(RvBool)value))
                {
                    /* Inconsistency of NAT parameters */
                    ret = RV_ERROR_BADPARAM;
                }
                else
                {
                    m_callset(call, callSupportNat, (RvBool)value);
                    cmTransSetSessionParam(call->hsTransSession, cmTransParam_callSupportNat ,&value);
                }
            }
            break;

        case cmParamParallelBeforeFS:
            m_callset(call, isMultiplexed, value);
            cmTransSetSessionParam(call->hsTransSession, cmTransParam_parallelBeforeFS, &value);
            break;

        default:
            simple = RV_FALSE;
    }

    if (simple)
    {
        emaUnlock((EMAElement)hsCall);
        cmiAPIExit((hApp, "cmCallSetParam=%d", ret));
        return ret;
    }
    nodeId = call->property;

    switch (param)
    {
        case cmParamDestinationAnnexEAddress:
        {
            if (!RV_PVT_NODEID_IS_VALID(call->annexECallSignalAddress))
                chNodeId = call->annexECallSignalAddress = pvtAddRoot(hVal,app->hAddrSyn,0,NULL);
            else
                chNodeId = call->annexECallSignalAddress;
            break;
        }
        case cmParamDestinationIpAddress:
        {
            if (!RV_PVT_NODEID_IS_VALID(call->callSignalAddress))
                chNodeId = call->callSignalAddress = pvtAddRoot(hVal,app->hAddrSyn,0,NULL);
            else
                chNodeId = call->callSignalAddress;
            break;
        }
        case cmParamDestinationInfo:
        case cmParamFullDestinationInfo:
        {
            if (!RV_PVT_NODEID_IS_VALID(call->destinationInfo))
                chNodeId = call->destinationInfo = pvtAddRoot(hVal,NULL,0,NULL);
            else
                chNodeId = call->destinationInfo;
            break;
        }
        case cmParamRouteCallSignalAddress:
        {
            if (!RV_PVT_NODEID_IS_VALID(call->routeCallSignalAddress))
                chNodeId = call->routeCallSignalAddress = pvtAddRoot(hVal,app->hAddrSyn,0,NULL);
            else
                chNodeId = call->routeCallSignalAddress;
            break;
        }
        default:
            /* Other parameters are taken from the property */
            if (app->callPropertyMode != pmodeDoNotUseProperty)
            {
                RvPvtNodeId messageNodeId;
                messageNodeId = pvtGetChild(hVal,nodeId,getParamMessage(param),NULL);
                if (!RV_PVT_NODEID_IS_VALID(messageNodeId))
                {
                    messageNodeId = pvtAddTree(hVal,nodeId,hVal,pvtGetChild(hVal,app->appDB,getParamMessage(param),NULL));
                }
                chNodeId = pvtBuildByFieldIds(hVal,messageNodeId,getParamName(param), 0, NULL);
            }
            else
            {
                cmCallParamEnum paramEnum;
                callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams(hApp, hsCall);

                paramEnum = getParamNumber(param);
                if ((int)paramEnum < 0)
                {
                    emaUnlock((EMAElement)hsCall);
                    cmiAPIExit((hApp, "cmCallSetParam=%d", paramEnum));
                    return (int)paramEnum;
                }

                if((value < 0) &&
                   ((param == cmParamSetupNonStandardData) ||
                    (param == cmParamCallProceedingNonStandardData) ||
                    (param == cmParamAlertingNonStandardData) ||
                    (param == cmParamConnectNonStandardData) ||
                    (param == cmParamReleaseCompleteNonStandardData) ||
                    (param == cmParamSetupFastStart) ||
                    (param == cmParamConnectFastStart) ||
                    (param == cmParamAlertingFastStart) ||
                    (param == cmParamCallProcFastStart) ||
                    (param == cmParamFullSourceInfo) ||
                    (param == cmParamFullSourceAddress) ||
                    (param == cmParamFullDestinationInfo) ||
                    (param == cmParamFullDestExtraCallInfo) ||
                    (param == cmParamFullExtension) ||
                    (param == cmParamFullDestinationAddress)))
                {
                    pvtDelete(hVal, props->paramNodeId[paramEnum]);
                    props->paramNodeId[paramEnum] = RV_PVT_INVALID_NODEID;
                    emaUnlock((EMAElement)hsCall);
                    cmiAPIExit((hApp, "cmCallSetParam=0"));
                    return 0;
                }

                if (!RV_PVT_NODEID_IS_VALID(props->paramNodeId[paramEnum]))
                    props->paramNodeId[paramEnum] = pvtAddRoot(hVal, getParamSyntax(app, paramEnum), 0, NULL);
                chNodeId = props->paramNodeId[paramEnum];
            }
            break;
    }

    if (!RV_PVT_NODEID_IS_VALID(chNodeId))
    {
        emaUnlock((EMAElement)hsCall);
        cmiAPIExit((hApp, "cmCallSetParam=%d", chNodeId));
        return chNodeId;
    }

    switch (param)
    {
    case cmParamSetupCanOverlapSending :
        pvtSet(hVal,chNodeId,-1, value, 0);  /* set canOverlapSend to RV_TRUE */
        break;


    case cmParamSourceAddress           :
    case cmParamDestinationAddress      :
    case cmParamDestExtraCallInfo       :
    case cmParamAlternativeAliasAddress :
    case cmParamAlternativeDestExtraCallInfo :
    case cmParamAlternativeExtension    :
    case cmParamCalledPartyNumber       :
    case cmParamCalledPartySubAddress   :
    case cmParamCallingPartyNumber      :
    case cmParamCallingPartySubAddress  :
    case cmParamExtension               :
    case cmParamConnectedAddress        :

        length = ((cmAlias*)svalue)->length;

        if ((param!=cmParamCalledPartyNumber &&
            param!=cmParamCalledPartySubAddress &&
            param!=cmParamCallingPartyNumber &&
            param!=cmParamCallingPartySubAddress) ||
            (((cmAlias*)svalue)->type == cmAliasTypeEndpointID) ||
            (((cmAlias*)svalue)->type == cmAliasTypeGatekeeperID))

        {
            RvPstNodeId synNodeId;
            pstNodeType type;
            pvtGet(hVal,chNodeId,NULL,&synNodeId,NULL,NULL);
            type=pstGetNodeType(pvtGetSynTree(hVal,chNodeId),synNodeId);
            if (type==pstSequenceOf || type==pstSetOf)
            {
                RvPvtNodeId tmpNodeId=chNodeId;
                if ((chNodeId=pvtGetByIndex(hVal, chNodeId, index+1, NULL)) < 0)
                {
                    chNodeId=pvtAdd(hVal, tmpNodeId, -800, 0, NULL, 0); /* create node */
                }

                if (!RV_PVT_NODEID_IS_VALID(chNodeId))
                {
                    emaUnlock((EMAElement)hsCall);
                    cmiAPIExit((hApp, "cmCallSetParam=%d", chNodeId));
                    return chNodeId;
                }
            }
            else if (index)
            {
                emaUnlock((EMAElement)hsCall);
                cmiAPIExit((hApp, "cmCallSetParam() invalid index [%d]", index));
                return RV_ERROR_UNKNOWN;
            }
            ret = aliasToVt(hVal,(cmAlias*)svalue,chNodeId);
        }
        else
        {
            if ((param == cmParamCalledPartyNumber) || (param == cmParamCallingPartyNumber))
            {
                if ((ret = pvtAdd(hVal,chNodeId,__q931(numberDigits), length, ((cmAlias*)svalue)->string,NULL))>=0)
                {
                    int typeOfNumber = 2;
                    int numberingPlanIdentification = 1;
                    RvPvtNodeId baseNodeId;
                    ret = baseNodeId = pvtAdd(hVal,chNodeId,__q931(octet3),0, NULL,NULL);

                    if (((cmAlias*)svalue)->type==cmAliasTypePartyNumber)
                    switch(((cmAlias*)svalue)->pnType)
                    {
                            /* the numbering plan is according to
                               Recommendations E.163 and E.164.*/
                        case cmPartyNumberPublicUnknown:
                            /* if used number digits carry prefix indicating type
                               of number according to national recommendations*/
                            typeOfNumber = 0;
                            numberingPlanIdentification = 1;
                        break;
                        case cmPartyNumberPublicInternationalNumber:
                            typeOfNumber = 1;
                            numberingPlanIdentification = 1;
                        break;
                        case cmPartyNumberPublicNationalNumber:
                            typeOfNumber = 2;
                            numberingPlanIdentification = 1;
                        break;
                        case cmPartyNumberPublicNetworkSpecificNumber:
                            /* not used, value reserved */
                            typeOfNumber = 3;
                            numberingPlanIdentification = 1;
                        break;
                        case cmPartyNumberPublicSubscriberNumber:
                            typeOfNumber = 4;
                            numberingPlanIdentification = 1;
                        break;
                        case cmPartyNumberPublicAbbreviatedNumber:
                            /* valid only for called party number at the outgoing
                               access, network substitutes appropriate number. */
                            typeOfNumber = 6;
                            numberingPlanIdentification = 1;
                        break;
                        case cmPartyNumberDataPartyNumber:
                            /* not used, value reserved */
                            typeOfNumber = 0;
                            numberingPlanIdentification = 3;
                        break;
                        case cmPartyNumberTelexPartyNumber:
                            /* not used, value reserved*/
                            typeOfNumber = 0;
                            numberingPlanIdentification = 4;
                        break;
                        case cmPartyNumberPrivateUnknown:
                            typeOfNumber = 0;
                            numberingPlanIdentification = 9;
                        break;
                        case cmPartyNumberPrivateLevel2RegionalNumber:
                            typeOfNumber = 0;
                            numberingPlanIdentification = 9;
                        break;
                        case cmPartyNumberPrivateLevel1RegionalNumber:
                            typeOfNumber = 0;
                            numberingPlanIdentification = 9;
                        break;
                        case cmPartyNumberPrivatePISNSpecificNumber:
                            typeOfNumber = 0;
                            numberingPlanIdentification = 9;
                        break;
                        case cmPartyNumberPrivateLocalNumber:
                            typeOfNumber = 0;
                            numberingPlanIdentification = 9;
                        break;
                        case cmPartyNumberPrivateAbbreviatedNumber:
                            typeOfNumber = 6;
                            numberingPlanIdentification = 9;
                        break;
                        case cmPartyNumberNationalStandardPartyNumber:
                            typeOfNumber = 0;
                            numberingPlanIdentification = 8;
                        break;
                        default:
                            if ((int)((cmAlias*)svalue)->pnType & 0x80)
                            {
                                typeOfNumber = (((int)((cmAlias*)svalue)->pnType)>>4)&0x7;
                                numberingPlanIdentification = ((int)((cmAlias*)svalue)->pnType)&0xf;
                            }
                    }

                    if ((ret  = pvtAdd(hVal, baseNodeId, __q931(typeOfNumber), typeOfNumber, NULL, NULL))>=0)
                    {
                        ret = pvtAdd(hVal, baseNodeId, __q931(numberingPlanIdentification), numberingPlanIdentification, NULL, NULL);
                    }
                }
                if (ret<0)
                {
                    emaUnlock((EMAElement)hsCall);
                    cmiAPIExit((hApp, "cmCallSetParam=%d", ret));
                    return ret;
                }
            }
            if ((param == cmParamCalledPartySubAddress) || (param == cmParamCallingPartySubAddress))
            {
                int typeOfSubaddress = 0;
                int oddEvenIndicator = 0;
                if ((ret = pvtAdd(hVal,chNodeId,__q931(subaddressInformation), length, ((cmAlias*)svalue)->string,NULL))>=0)
                {
                    RvPvtNodeId baseNodeId;
                    ret = baseNodeId = pvtAdd(hVal,chNodeId,__q931(octet3),0, NULL,NULL);

                    if (((cmAlias*)svalue)->type==cmAliasTypePartyNumber)
                    if ((int)((cmAlias*)svalue)->pnType&0x80)
                    {
                        typeOfSubaddress = (((int)((cmAlias*)svalue)->pnType)>>4)&0x7;
                        oddEvenIndicator = (((int)((cmAlias*)svalue)->pnType)>>3)&0x1;
                    }

                    if ((ret  = pvtAdd(hVal, baseNodeId, __q931(typeOfSubaddress), typeOfSubaddress, NULL, NULL))>=0)
                    {
                        ret = pvtAdd(hVal, baseNodeId, __q931(oddEvenIndicator), oddEvenIndicator, NULL, NULL);
                    }
                }
                if (ret<0)
                {
                    emaUnlock((EMAElement)hsCall);
                    cmiAPIExit((hApp, "cmCallSetParam=%d", ret));
                    return ret;
                }
            }
        }
        break;

    case cmParamSetupNonStandardData       :
    case cmParamCallProceedingNonStandardData  :
    case cmParamAlertingNonStandardData    :
    case cmParamConnectNonStandardData     :
    case cmParamReleaseCompleteNonStandardData :
    case cmParamSetupFastStart         :
    case cmParamConnectFastStart       :
    case cmParamAlertingFastStart       :
    case cmParamCallProcFastStart       :
        if (value >= 0)
            pvtShiftTree(hVal,chNodeId,value);
        else
            pvtDelete(hVal,chNodeId);
        break;

    case cmParamFullSourceInfo      :
    case cmParamFullSourceAddress   :
    case cmParamFullDestinationInfo :
    case cmParamFullDestExtraCallInfo :
    case cmParamFullExtension       :
    case cmParamFullDestinationAddress:
        if (value >= 0)
            pvtSetTree(hVal,chNodeId,hVal,value);
        else
        {
            pvtDelete(hVal,chNodeId);
            if (param == cmParamFullDestinationInfo)
            {
                chNodeId=call->destinationInfo = RV_PVT_INVALID_NODEID;
            }
        }
        break;

    case cmParamSetupNonStandard           :
    case cmParamCallProceedingNonStandard  :
    case cmParamAlertingNonStandard        :
    case cmParamConnectNonStandard         :
    case cmParamReleaseCompleteNonStandard :
        setNonStandardParam(hVal,chNodeId,(cmNonStandardParam*)svalue);
        break;

    case cmParamDestCallSignalAddress   :
    case cmParamSrcCallSignalAddress    :
    case cmParamRemoteIpAddress         :
    case cmParamAlternativeAddress      :
    case cmParamDestinationIpAddress    :
    case cmParamDestinationAnnexEAddress:
    case cmParamRouteCallSignalAddress  :
        {
            RvPstNodeId synNodeId;
            pstNodeType type;
            pvtGet(hVal,chNodeId,NULL,&synNodeId,NULL,NULL);
            type=pstGetNodeType(pvtGetSynTree(hVal,chNodeId),synNodeId);
            if (type==pstSequenceOf || type==pstSetOf)
            {
                if ((chNodeId=pvtGetByIndex(hVal, chNodeId, index, NULL)) < 0)
                {
                    chNodeId=pvtAdd(hVal, chNodeId, -800, 0, NULL, 0); /* create node */
                }

                if (!RV_PVT_NODEID_IS_VALID(chNodeId))
                {
                    emaUnlock((EMAElement)hsCall);
                    cmiAPIExit((hApp, "cmCallSetParam=%d", chNodeId));
                    return chNodeId;
                }
            }
            ret=cmTAToVt(hVal,chNodeId, (cmTransportAddress*)svalue);
        }
        break;

    case cmParamSetupH245Address        :
    case cmParamAlertingH245Address     :
    case cmParamCallProcH245Address     :
    case cmParamH245Address         :
        {
            cmTransportAddress* address=(cmTransportAddress*)svalue;
            getGoodAddressForCall(hsCall,address);
            ret=cmTAToVt(hVal,chNodeId, address);
        }
        break;

    case cmParamInformationTransferCapability :
    case cmParamActiveMc            :
    case cmParamReleaseCompleteCause    :
    case cmParamEarlyH245               :
        pvtSet(hVal,chNodeId,RV_ERROR_UNKNOWN,value, NULL);
        break;

    case cmParamConferenceGoal      :
        if (value == (RvInt32)cmCallIndependentSupplementaryService)
        {
            cmH245Stage stage = cmTransH245Stage_facility;
            m_callset(call, h245Stage, stage);
            cmTransSetSessionParam(call->hsTransSession, cmTransParam_H245Stage, &stage);
        }
        /* fall through to the next case - we want to add it to the outgoing message */

    case cmParamSourceInfo          :
    case cmParamDestinationInfo     :
    case cmParamReleaseCompleteReason   :
    case cmParamCallType            :
    case cmParamFacilityReason      :
        ret = pvtAdd(hVal,chNodeId,getEnumNameId(getParamFieldName(param),getParamFieldNameSize(param),(unsigned int)value),0, NULL, NULL);
        if (ret < 0)
        {
            emaUnlock((EMAElement)hsCall);
            cmiAPIExit((hApp, "cmCallSetParam=%d", ret));
            return ret;
        }

        if (param == cmParamConferenceGoal)
            call->conferenceGoal = (cmConferenceGoalType)value;
        break;

    case cmParamCID             :
    case cmParamFacilityCID             :
    case cmParamCallID          :
        pvtSet(hVal,chNodeId, -1, 16, svalue);
        break;
    case cmParamSetupSendingComplete:
        if (value)
            pvtSet(hVal,chNodeId, -1, 0, NULL);
        else
        {
            pvtDelete(hVal, chNodeId);
            chNodeId = RV_PVT_INVALID_NODEID;
        }
        break;
    case cmParamUserUser                :
    case cmParamConnectUserUser         :
        if (value)
        {
            pvtAdd(hVal, chNodeId, __q931(protocol_discriminator), 5, NULL, NULL);
            pvtAdd(hVal, chNodeId, __q931(user_information), value, svalue, NULL);
        }
        else
        {
            pvtDelete(hVal, chNodeId);
            chNodeId = RV_PVT_INVALID_NODEID;
        }
        break;
    case cmParamDisplay             :
    case cmParamConnectDisplay          :
        {
            int len=strlen(svalue);
            if (len)
                pvtSet(hVal, chNodeId, -1, len, svalue);
            else
            {
                pvtDelete(hVal, chNodeId);
                chNodeId = RV_PVT_INVALID_NODEID;
            }
        }
        break;

    default:
        emaUnlock((EMAElement)hsCall);
        cmiAPIExit((hApp, "cmCallSetParam: RV_ERROR_UNKNOWN"));
        return RV_ERROR_UNKNOWN;
    }

    if (!RV_PVT_NODEID_IS_VALID(chNodeId) && (app->callPropertyMode == pmodeDoNotUseProperty))
    {
        int num = (int)getParamNumber(param);
        if (num >= 0)
        {
            callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams(hApp, hsCall);
            props->paramNodeId[num] = RV_PVT_INVALID_NODEID;
        }
    }

    emaUnlock((EMAElement)hsCall);
    cmiAPIExit((hApp, "cmCallSetParam=%d", ret));
    return ret;
}


/******************************************************************************
 * cmGetParam
 * ----------------------------------------------------------------------------
 * General: Gets a specific Stack instance parameter for the application.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp     - The Stack handle for the application.
 *         param    - The requested parameter.
 *         index    - The instance of the parameter specified in param.
 *                    The index of a single-instance parameter is 0.
 *                    The index of a multiple-instance parameter is 1-based.
 *         value    - If the parameter value is a simple integer, the input
 *                    value of this parameter is ignored.
 *                    If the parameter value is a structure, value represents
 *                    the maximum length of svalue in bytes.
 * Output: value    - If the parameter value is a simple integer, value is the
 *                    pointer to the parameter value.
 *                    If the parameter value is a structure, value represents
 *                    the length of the parameter.
 *         svalue   - If the parameter value is a structure, svalue
 *                    (structured value) represents the pointer to the parameter
 *                    itself.
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmGetParam(
    IN      HAPP         hApp,
    IN      cmParam      param,
    IN      RvInt        index,
    INOUT   RvInt32*     value,
    OUT     RvChar*      svalue)
{
    cmElem *app = (cmElem *)hApp;
    RvStatus status = RV_ERROR_BADPARAM;
    RvPvtNodeId tmpNodeId = RV_PVT_INVALID_NODEID;

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if (hApp == NULL)
        return RV_ERROR_NULLPTR;
#endif

    cmiAPIEnter((hApp, "cmGetParam(hApp=%p,param=%d,index=%d)", hApp, param, index));

    switch (param)
    {
    case cmParamLocalCallSignalAddress:
        tmpNodeId = app->q931TpktAddresses;
        status = RV_OK;
        break;

    case cmParamLocalAnnexEAddress:
        tmpNodeId = app->q931AnnexEAddresses;
        status = RV_OK;
        break;

    case cmParamLocalRASAddress:
        tmpNodeId = app->rasAddresses;
        status = RV_OK;
        break;
    }

    if (status == RV_OK)
    {
        if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
        {
#if (RV_CHECK_MASK & RV_CHECK_OTHER)
            if (value == NULL)
                return RV_ERROR_NULLPTR;
            if (*value < (RvInt32)sizeof(cmTransportAddress))
                return RV_ERROR_OUTOFRANGE;
            if (svalue == NULL)
                return RV_ERROR_NULLPTR;
#endif
            /* correction for backwards compatibility */
            if (index == 0) index = 1;

            tmpNodeId = pvtGetByIndex(app->hVal, tmpNodeId, index, NULL);
            if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
            {
                if (RV_PVT_NODEID_IS_VALID(pvtGetChildByFieldId(app->hVal, tmpNodeId, __q931(address), NULL, NULL)))
                {
                    status = cmVtToLTA(app->hVal, tmpNodeId, (cmTransportAddress*)svalue);
                }
                else
                {
                    status = cmVtToTA(app->hVal, tmpNodeId, (cmTransportAddress*)svalue);
                }
                if (status == RV_OK)
                    *value = (RvInt32)sizeof(cmTransportAddress);
            }
            else
                status = RV_ERROR_OUTOFRANGE;
        }
        else
            status = RV_ERROR_NOTSUPPORTED;
    }
    else if (param == cmParamTerminalAliasesNode)
    {
        *value = ((rasModule*)cmiGetRasHandle(hApp))->termAliasesNode;
        status = RV_OK;
    }

    cmiAPIExit((hApp,"cmGetParam: %d", status));
    return status;
}


/******************************************************************************
 * cmSetParam
 * ----------------------------------------------------------------------------
 * General: Sets a specific Stack instance parameter for the application.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp     - The Stack handle for the application.
 *         param    - The requested parameter.
 *         index    - The instance of the parameter specified in param.
 *                    The index of a single-instance parameter is 0.
 *                    The index of a multiple-instance parameter is 1-based.
 *         value    - If the parameter value is a simple integer, value is the
 *                    the parameter value.
 *                    If the parameter value is a structure, value represents
 *                    the length of the parameter.
 *         svalue   - If the parameter value is a structure, svalue
 *                    (structured value) represents the pointer to the parameter
 *                    itself.
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmSetParam(
    IN      HAPP         hApp,
    IN      cmParam      param,
    IN      RvInt        index,
    IN      RvInt32      value,
    IN      RvChar*      svalue)
{
    cmElem *app = (cmElem *)hApp;
    RvStatus status = RV_ERROR_BADPARAM;
    RvPvtNodeId tmpNodeId = RV_PVT_INVALID_NODEID;

    RV_UNUSED_ARG(svalue);
    RV_UNUSED_ARG(app);

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if (hApp == NULL)
        return RV_ERROR_NULLPTR;
#endif
    cmLock(hApp);

    cmiAPIEnter((hApp, "cmSetParam(hApp=%p,param=%d,index=%d,value=%d)", hApp, param, index, value));

    switch (param)
    {
    case cmParamLocalCallSignalAddress:
    case cmParamLocalAnnexEAddress:
    case cmParamLocalRASAddress:
        break;
    case cmParamTerminalAliasesNode:
        tmpNodeId = ((rasModule*)cmiGetRasHandle(hApp))->termAliasesNode;
        pvtDelete(cmGetValTree(hApp), tmpNodeId);
        ((rasModule*)cmiGetRasHandle(hApp))->termAliasesNode = value;
        status = RV_OK;
    }

    cmiAPIExit((hApp,"cmSetParam: %d", status));
    cmUnlock(hApp);
    return status;
}


static
int callGetParam(
    IN callElem*    call,
    IN cmCallParam  param,
    IN int          index,
    IN RvInt32*     value,
    IN char*        svalue)
{
    RvBool simple=RV_TRUE;
    int nodeId;
    int chNodeId;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    cmElem* app=(cmElem*)hApp;
    HPVT hVal;
    int status = 0;

    hVal = app->hVal;

    if (index==0)
    {
        switch(param)
        {
            case cmParamCallSignalAddress:
            {
                cmGetParam(hApp,cmParamLocalCallSignalAddress,index,value,svalue);
                break;
            }
            case cmParamEstablishH245           :
            {
                RvBool establish;
                if (call->hsTransSession)
                    cmTransGetSessionParam(call->hsTransSession, cmTransParam_notEstablishControl,&establish);
                else
                    establish = m_callget(call,notEstablishControl);
                if (value) *value=!establish;
                break;
            }
            case cmParamEarlyH245               :
            {
                cmH245Stage stage;
                if (call->hsTransSession)
                    cmTransGetSessionParam(call->hsTransSession, cmTransParam_H245Stage,&stage);
                else
                    stage = (cmH245Stage)m_callget(call,h245Stage);
                if (value) *value=(stage==cmTransH245Stage_early);
                break;
            }
            case cmParamH245Parallel:
            {
                if (call->hsTransSession)
                    cmTransGetSessionParam(call->hsTransSession, cmTransParam_isParallelTunnelingSupported,value);
                else
                    if (value) *value = m_callget(call,isParallelTunneling);
                break;
            }
            case cmParamH245Tunneling:
                if (call->hsTransSession)
                    cmTransGetSessionParam(call->hsTransSession, cmTransParam_isTunnelingSupported,value);
                else
                    if (value) *value = m_callget(call,h245Tunneling);
                break;
            case cmParamH245Stage:
                if (call->hsTransSession)
                    cmTransGetSessionParam(call->hsTransSession, cmTransParam_H245Stage,value);
                else
                    if (value) *value = m_callget(call,h245Stage);
            break;
            case cmParamShutdownEmptyConnection:
                if (call->hsTransSession)
                    cmTransGetSessionParam(call->hsTransSession, cmTransParam_shutdownEmptyConnection,value);
                else
                    if (value) *value = m_callget(call,shutdownEmptyConnection);
            break;
            case cmParamIsMultiplexed:
                if (call->hsTransSession)
                    cmTransGetSessionParam(call->hsTransSession, cmTransParam_isMultiplexed,value);
                else
                    if (value) *value = m_callget(call,isMultiplexed);
            break;
            case cmParamAnnexE:
                if (value) *value = (RvInt32)call->annexE; break;
            case cmParamCRV:
                if (value) *value=call->crv;break;
            case cmParamRASCRV:
                if (value) *value=call->rascrv;break;
            case cmParamRate:
                if (value) *value=call->rate;break;
            case cmParamRequestedRate:
                if (value) *value=call->newRate;break;
            case cmParamMultiRate:
                if (value) *value=m_callget(call,multiRate);break;
            case cmParamCanOverlapSending:
                if (value) *value=m_callget(call,enableOverlapSending);break;
            case cmParamCallID:
                if (value) *value=16;
                if (svalue)
                   memcpy(svalue,call->callId,16);
                break;
            case cmParamCID:
                if (value) *value=16;
                if (svalue)
                   memcpy(svalue,call->cId,16);
                break;
            case cmParamConferenceGoal:
                *value = (RvInt32)call->conferenceGoal;
                break;
            case cmParamSourceIpAddress:
                if (cmTransGetSessionParam(call->hsTransSession, cmTransParam_sourceIpAddress, svalue) != cmTransOK)
                    return RV_ERROR_UNKNOWN;
                break;
            case cmParamState:
                *value = (RvInt32)call->state;
                break;
            case cmParamStateMode:
                *value = (RvInt32)call->stateMode;
                break;
            case cmParamAutomaticRAS:
                *value = m_callget(call, automaticRAS);
                break;
            case cmParamHopCount:
                *value = (RvInt32)call->hopCount;
                break;
            case cmParamDisconnectionReason:
                *value = (RvInt32)call->disconnectReason;
                break;
            case cmParamExtendedFastConnectState:
                *value = (RvInt32)cmiGetExtendedFastConnectStatus((HCALL)call, m_callget(call,callInitiator));
                break;
            case cmParamSupportNat:
                *value = (RvInt32)m_callget(call,callSupportNat);
                break;
            case cmParamParallelBeforeFS:
                if (call->hsTransSession)
                {
                    cmTransGetSessionParam(call->hsTransSession, cmTransParam_parallelBeforeFS, value);
                    break;
                }
                return RV_ERROR_UNKNOWN;
            case cmParamAltEpProperty:
                *value = call->altEpProperty;
                break;
            case cmParamAltEpNum:
                *value = call->altEpNum;
                break;
            default:
                simple=RV_FALSE;
        }
    }
    else
        simple=RV_FALSE;
    if (simple)
        return 0;

    nodeId=call->property;

    switch (param)
    {
        case cmParamDestinationIpAddress:
            chNodeId=call->callSignalAddress;
            break;
        case cmParamRemoteIpAddress:
            chNodeId=call->remoteCallSignalAddress;
            break;
        case cmParamDestinationAnnexEAddress:
            chNodeId=call->annexECallSignalAddress;
            break;
        case cmParamRouteCallSignalAddress:
            chNodeId=call->routeCallSignalAddress;
            break;
        case cmParamDestinationInfo:
        case cmParamFullDestinationInfo:
            chNodeId=call->destinationInfo;
            break;
        default:
            /* Take the parameter from the property */
            if (app->callPropertyMode != pmodeDoNotUseProperty)
            {
                chNodeId = pvtGetNodeIdByFieldIds(hVal,pvtGetChild(hVal,nodeId,getParamMessage(param),NULL), getParamName(param));
            }
            else
            {
                callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams(hApp, (HCALL)call);
                chNodeId = props->paramNodeId[getParamNumber(param)];
            }
            break;
    }

    if (chNodeId < 0)
        return chNodeId;

    switch (param)
    {
        case cmParamSetupCanOverlapSending :
            pvtGet(hVal, chNodeId, NULL, NULL, value, NULL);
            break;

        case cmParamCalledPartyNumber       :
        case cmParamCalledPartySubAddress   :
        case cmParamCallingPartyNumber      :
        case cmParamCallingPartySubAddress  :
        case cmParamExtension               :
        case cmParamSourceAddress           :
        case cmParamDestinationAddress      :
        case cmParamDestExtraCallInfo       :
        case cmParamAlternativeAliasAddress :
        case cmParamAlternativeDestExtraCallInfo :
        case cmParamAlternativeExtension    :
        case cmParamConnectedAddress        :
            if (!svalue)
                return RV_ERROR_UNKNOWN;
            if (param!=cmParamCalledPartyNumber &&
                param!=cmParamCalledPartySubAddress &&
                param!=cmParamCallingPartyNumber &&
                param!=cmParamCallingPartySubAddress)
            {
                int synNodeId;
                pstNodeType type;
                pvtGet(hVal,chNodeId,NULL,&synNodeId,NULL,NULL);

                type = pstGetNodeType(pvtGetSynTree(hVal,chNodeId), synNodeId);
                if ((type == pstSequenceOf) || (type == pstSetOf))
                    chNodeId = pvtGetByIndex(hVal, chNodeId, index+1, NULL);
                else if (index)
                    return RV_ERROR_UNKNOWN;

                if (chNodeId >= 0)
                    status = vtToAlias(hVal, (cmAlias*)svalue, chNodeId, app->bCheckParamLength);
                else
                    return RV_ERROR_UNKNOWN;
            }
            else
            {
                int length;
                RvPvtNodeId tempNode;
                cmAlias *alias = (cmAlias *)svalue;
                RvPstFieldId fieldId=(param==cmParamCalledPartyNumber || param==cmParamCallingPartyNumber)?
                                __q931(numberDigits):__q931(subaddressInformation);
                if ((tempNode = pvtGetChildByFieldId(hVal,chNodeId,fieldId,&length, NULL))<0)
                    return RV_ERROR_UNKNOWN;

                if (app->bCheckParamLength)
                    pvtGetString(hVal, tempNode, (int)alias->length, alias->string);
                else
                    pvtGetString(hVal, tempNode, length, alias->string);
                alias->length = (RvUint16)length;

                if (alias->type != cmAliasTypePartyNumber)
                    alias->type = cmAliasTypeE164;
                else
                {
                    tempNode = pvtGetChild(hVal, chNodeId, __q931(octet3), NULL);
                    if (param == cmParamCalledPartyNumber || param == cmParamCallingPartyNumber)
                    {
                        int typeOfNumber=2;
                        int numberingPlanIdentification=1;
                        pvtGetChildByFieldId(hVal,tempNode,__q931(typeOfNumber),&typeOfNumber, NULL);
                        pvtGetChildByFieldId(hVal,tempNode,__q931(numberingPlanIdentification),&numberingPlanIdentification, NULL);
                        alias->pnType = (cmPartyNumberType)(0x80+(typeOfNumber<<4)+numberingPlanIdentification);
                    }
                    else
                    {
                        int typeOfSubaddress = 0;
                        int oddEvenIndicator = 0;
                        pvtGetChildByFieldId(hVal,tempNode,__q931(typeOfSubaddress),&typeOfSubaddress, NULL);
                        pvtGetChildByFieldId(hVal,tempNode,__q931(oddEvenIndicator),&oddEvenIndicator, NULL);
                        alias->pnType = (cmPartyNumberType)(0x80+(typeOfSubaddress<<4)+(oddEvenIndicator<<3));
                    }
                }
            }
            break;

        case cmParamFullSourceInfo      :
        case cmParamFullDestinationInfo     :
        case cmParamFullSourceAddress      :
        case cmParamFullDestinationAddress     :
            if (value) *value=chNodeId;
            break;
        case cmParamSetupNonStandardData       :
        case cmParamCallProceedingNonStandardData  :
        case cmParamAlertingNonStandardData    :
        case cmParamConnectNonStandardData     :
        case cmParamReleaseCompleteNonStandardData :
        case cmParamSetupFastStart         :
        case cmParamConnectFastStart       :
        case cmParamAlertingFastStart       :
        case cmParamCallProcFastStart       :
            if (value)
                *value=chNodeId;
            break;
        case cmParamSetupNonStandard           :
        case cmParamCallProceedingNonStandard  :
        case cmParamAlertingNonStandard        :
        case cmParamConnectNonStandard         :
        case cmParamReleaseCompleteNonStandard :
            if (!svalue || index)
                return RV_ERROR_UNKNOWN;
            return getNonStandardParam(hVal,chNodeId,(cmNonStandardParam*)svalue);
        case cmParamDestCallSignalAddress   :
        case cmParamSrcCallSignalAddress    :
        case cmParamRemoteIpAddress         :
        case cmParamCallSignalAddress       :
        case cmParamAlternativeAddress      :
        case cmParamDestinationIpAddress    :
        case cmParamDestinationAnnexEAddress:
        case cmParamRouteCallSignalAddress  :
            {
                int synNodeId;
                int newNodeId;
                pstNodeType type;
                pvtGet(hVal,chNodeId,NULL,&synNodeId,NULL,NULL);
                type=pstGetNodeType(pvtGetSynTree(hVal,chNodeId),synNodeId);
                if (type==pstSequenceOf || type==pstSetOf)
                    newNodeId = pvtGetByIndex(hVal, chNodeId, index+1, NULL);
                else
                {
                    if (index)
                        return RV_ERROR_UNKNOWN;
                    else
                        newNodeId = chNodeId;
                }
                if (svalue)
                {
                    return cmVtToTA(hVal,newNodeId, (cmTransportAddress*)svalue);
                }
            }
            break;
        case cmParamSetupH245Address        :
        case cmParamAlertingH245Address     :
        case cmParamCallProcH245Address     :
        case cmParamH245Address         :
            {
                cmTransportAddress* address=(cmTransportAddress*)svalue;
                if (!svalue || index)
                    return RV_ERROR_UNKNOWN;
                return cmVtToTA(hVal,chNodeId, address);
            }
        case cmParamInformationTransferCapability :
        case cmParamActiveMc            :
        case cmParamReleaseCompleteCause    :
            if (pvtGet(hVal,chNodeId,NULL,NULL,value, NULL)>=0 && !index)
                return 0;
            return RV_ERROR_UNKNOWN;
        case cmParamSourceInfo          :
        case cmParamDestinationInfo     :
            {
                RvPstFieldId nameId;
                int childId;
                int val;
                childId = pvtChild(hVal,chNodeId);
                while ((childId>=0) && !index)
                {
                    pvtGet(hVal,childId,&nameId,NULL,NULL,NULL);
                    {
                        val=getEnumValueByNameId(getParamFieldName(param),nameId);
                        if (val>=0)
                        {
                            *value = val;
                            return 0;
                        }
                        else
                            childId = pvtBrother(hVal,childId);
                    }
                }
            }
            return RV_ERROR_UNKNOWN;

        case cmParamCallType                :
        case cmParamReleaseCompleteReason   :
        case cmParamFacilityReason          :
            {
                RvPstFieldId nameId;
                if (pvtGet(hVal,pvtChild(hVal,chNodeId),&nameId,NULL,NULL,NULL)>=0 && !index)
                {
                    *value=getEnumValueByNameId(getParamFieldName(param),nameId);
                }
                else
                    return RV_ERROR_UNKNOWN;
            }
            break;

        case cmParamFacilityCID             :
            {
                if (!svalue)
                    return RV_ERROR_UNKNOWN;
                if (value) *value=16;
                if (pvtGet(hVal,chNodeId,NULL, NULL, NULL, NULL)>=0 && !index)
                {
                    pvtGetString(hVal,chNodeId,16,svalue);
                }
                else
                    return RV_ERROR_UNKNOWN;
            }
            break;

        case cmParamSetupSendingComplete:
            if (value)
                *value=(chNodeId>=0);
            break;

        case cmParamDisplay                 :
        case cmParamConnectDisplay          :
            {
                RvUint32 num,len;
                if (!svalue)
                    return RV_ERROR_UNKNOWN;
                if (pvtGet(hVal,chNodeId,NULL,NULL, (RvInt32*)&num, NULL)>=0 && !index)
                {
                    len = (RvUint32)((value)?*value:0);
                    pvtGetString(hVal, chNodeId, (RvInt32)len, svalue);
                    if (value) *value=num;
                }
                else
                    return RV_ERROR_UNKNOWN;
            }
            break;

        case cmParamUserUser                :
        case cmParamConnectUserUser         :
            {
                RvUint32 num,len;
                if (!svalue)
                    return RV_ERROR_UNKNOWN;
                chNodeId = pvtGetChildByFieldId(hVal, chNodeId, __q931(user_information), (RvInt32*)&num, NULL);
                if ((chNodeId >= 0) && !index)
                {
                    len = (RvUint32)((value)?*value:0);
                    pvtGetString(hVal, chNodeId, (RvInt32)len, svalue);
                    if (value) *value = num;
                }
                else
                    return RV_ERROR_UNKNOWN;
            }
            break;

        default:
            break;
    }

    return status;
}


RVAPI
int RVCALLCONV cmCallGetParam(
                            IN HCALL        hsCall,
                            IN cmCallParam  param,
                            IN int          index,
                            IN RvInt32*     value,
                            IN char *       svalue)
{
    int ret;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp,"cmCallGetParam: hsCall =%p,param=%d",hsCall,param));

    if (!emaLock((EMAElement)hsCall))
    {
        cmiAPIExit((hApp,"cmCallGetParam [Call Was Deleted]"));
        return RV_ERROR_UNKNOWN;
    }

    ret = callGetParam((callElem *)hsCall, param, index, value, svalue);

    if (ret > 0)
        ret = 0;
    else if (ret < 0)
    {
        /* Make sure we handle error parameters - set their values to the right values... */
        switch (param)
        {
            /* Aliases */
            case cmParamCalledPartyNumber:
            case cmParamCalledPartySubAddress:
            case cmParamCallingPartyNumber:
            case cmParamCallingPartySubAddress:
            case cmParamExtension:
            case cmParamSourceAddress:
            case cmParamDestinationAddress:
            case cmParamDestExtraCallInfo:
            case cmParamAlternativeAliasAddress:
            case cmParamAlternativeDestExtraCallInfo:
            case cmParamAlternativeExtension:
            case cmParamConnectedAddress:
                if (svalue)
                {
                    ((cmAlias *)svalue)->length = 0;
                }
                break;

            /* Transport addresses */
            case cmParamDestCallSignalAddress:
            case cmParamSrcCallSignalAddress:
            case cmParamSetupH245Address:
            case cmParamH245Address:
            case cmParamAlternativeAddress:
            case cmParamDestinationIpAddress:
            case cmParamSourceIpAddress:
            case cmParamRemoteIpAddress:
            case cmParamCallSignalAddress:
            case cmParamAlertingH245Address:
            case cmParamCallProcH245Address:
            case cmParamRouteCallSignalAddress:
            case cmParamDestinationAnnexEAddress:
                if (svalue)
                    memset(svalue, 0, sizeof(cmTransportAddress));
                break;

            /* Integers and enumeration values */
            case cmParamSourceInfo:
            case cmParamActiveMc:
            case cmParamConferenceGoal:
            case cmParamCallType:
            case cmParamDestinationInfo:
            case cmParamReleaseCompleteReason:
            case cmParamReleaseCompleteCause:
            case cmParamRate:
            case cmParamRequestedRate:
            case cmParamInformationTransferCapability:
            case cmParamMultiRate:
            case cmParamFacilityReason:
                if (value)
                    *value = RV_ERROR_UNKNOWN;
                break;

            /* Others */
            case cmParamFacilityCID:
                if (svalue != NULL)
                    memset(svalue, 0, 16);
                break;
        default:
                break;
        }
    }

    emaUnlock((EMAElement)hsCall);
    cmiAPIExit((hApp, "cmCallGetParam=%d", ret));
    return ret;
}


/************************************************************************
 * cmCallDeleteParam
 * purpose: Delete a parameter that was set in the call.
 * input  : hsCall      - Stack handle for the call
 *          param       - Parameter to delete
 *          index       - Index of parameter to delete (1-based)
 *                        Should be 0 for non-array parameters
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallDeleteParam(
                            IN HCALL        hsCall,
                            IN cmCallParam  param,
                            IN int          index)
{
    int chNodeId;
    int* delNodeId = NULL;
    callElem*call=(callElem*)hsCall;

    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    cmElem* app=(cmElem*)hApp;
    HPVT hVal;

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallDeleteParam: hsCall=%p,param %d",hsCall,param));
    if (!emaLock((EMAElement)hsCall))
    {
        cmiAPIExit((hApp, "cmCallDeleteParam [Call Was Deleted]"));
        return RV_ERROR_UNKNOWN;
    }

    hVal = app->hVal;

    switch(param)
    {
        case cmParamDestinationAnnexEAddress:
            chNodeId=call->annexECallSignalAddress;
            delNodeId = &call->annexECallSignalAddress;
        break;
        case cmParamDestinationIpAddress:
            chNodeId=call->callSignalAddress;
            delNodeId = &call->callSignalAddress;
        break;
        case cmParamDestinationInfo:
        case cmParamFullDestinationInfo:
            chNodeId=call->destinationInfo;
            delNodeId = &call->destinationInfo;
        break;
        case cmParamRouteCallSignalAddress:
            chNodeId=call->routeCallSignalAddress;
            delNodeId = &call->routeCallSignalAddress;
        break;
        default:
            if (app->callPropertyMode != pmodeDoNotUseProperty)
            {
                chNodeId=pvtGetNodeIdByFieldIds(hVal,pvtGetChild(hVal,call->property,getParamMessage(param),NULL),getParamName(param));
            }
            else
            {
                callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams(hApp, hsCall);
                chNodeId = props->paramNodeId[getParamNumber(param)];
                delNodeId = &props->paramNodeId[getParamNumber(param)];
            }
            break;
    }

    {
        int synNodeId;
        pstNodeType type;
        pvtGet(hVal,chNodeId,NULL,&synNodeId,NULL,NULL);
        type=pstGetNodeType(pvtGetSynTree(hVal,chNodeId),synNodeId);
        if (type==pstSequenceOf || type==pstSetOf)
            chNodeId=pvtGetByIndex(hVal,chNodeId,index,NULL);
        else if (index)
        {
            emaUnlock((EMAElement)hsCall);
            cmiAPIExit((hApp, "cmCallDeleteParam() invalid index [%d]", index));
            return RV_ERROR_UNKNOWN;
        }
    }

    if (chNodeId >= 0)
    {
        pvtDelete(hVal,chNodeId);
        if (delNodeId != NULL)
            *delNodeId = RV_ERROR_UNKNOWN; /* Mark this nodeId in the call as invalid */
    }

    emaUnlock((EMAElement)hsCall);
    cmiAPIExit((hApp, "cmCallDeleteParam=%d", chNodeId));
    return chNodeId;
}



RVAPI
int RVCALLCONV cmCallGetNumOfParams(
          IN HCALL          hsCall,
          IN cmCallParam    param)
{
    pstNodeType     type;
    int             synNodeId;
    RvPvtNodeId     chNodeId = RV_PVT_INVALID_NODEID;
    callElem        *call = (callElem*)hsCall;
    HAPP            hApp;
    cmElem          *app;
    HPVT            hVal;
    RvBool          havePath = RV_TRUE;
    int             result;

    if (!emaLock((EMAElement)hsCall))
        return RV_ERROR_UNKNOWN;

    hApp = (HAPP)emaGetInstance((EMAElement)call);
    app = (cmElem*)hApp;
    hVal = app->hVal;

    cmiAPIEnter((hApp, "cmCallGetNumOfParams: hsCall=%p", hsCall));

    switch(param)
    {
        case cmParamDestinationAnnexEAddress:
            chNodeId = call->annexECallSignalAddress;
            break;
        case cmParamDestinationIpAddress:
            chNodeId = call->callSignalAddress;
            break;
        case cmParamDestinationInfo:
        case cmParamFullDestinationInfo:
            chNodeId = call->destinationInfo;
            break;
        case cmParamRouteCallSignalAddress:
            chNodeId = call->routeCallSignalAddress;
            break;
        default:
            if (app->callPropertyMode != pmodeDoNotUseProperty)
            {
                RvInt16 *path = getParamName(param);
                if (path != NULL)
                    chNodeId = pvtGetNodeIdByFieldIds(hVal,pvtGetChild(hVal,call->property,getParamMessage(param),NULL), path);
                else
                    havePath = RV_FALSE;
            }
            else
            {
                cmCallParamEnum paramEnum = getParamNumber(param);
                if ((int)paramEnum >= 0)
                {
                    callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams(hApp, hsCall);
                    chNodeId = props->paramNodeId[paramEnum];
                }
                else
                    havePath = RV_FALSE;
            }
            break;
    }

    if (havePath)
    {
        if (pvtGet(hVal, chNodeId, NULL, &synNodeId, NULL, NULL) >= 0)
        {
            type = pstGetNodeType(pvtGetSynTree(hVal, chNodeId), synNodeId);
            if ((type == pstSequenceOf) || (type == pstSetOf))
                result = pvtNumChilds(hVal, chNodeId); /* count'em up... */
            else
                result = 1; /* Not a sequence of - existence means we have a single element */
        }
        else
            result = 0; /* We have a path, but no elements for it... */
    }
    else
        result = 1;

    emaUnlock((EMAElement)hsCall);
    cmiAPIExit((hApp, "cmCallGetNumOfParams=%d", result));
    return result;
}

/************************************************************************
 * callInitParameters
 * purpose: Initialize the property database for the call according to the
 *          mode used in the configuration.
 * input  : hApp    - Application handle for the stack
 *          hsCall  - Stack handle for the call
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int callInitParameters(IN HAPP hApp, IN HCALL hsCall)
{
    callElem*call=(callElem*)hsCall;
    cmElem* app = (cmElem*)hApp;
    HPVT hVal;
    hVal = app->hVal;

    switch (app->callPropertyMode)
    {
        case pmodeFullProperty:
            /* Make sure we create the full database for messages of a call with their
               default values from the beginning */
            if (!RV_PVT_NODEID_IS_VALID(call->property))
                call->property = pvtAddRootShielded(hVal, app->hSyn, 0, NULL);
            pvtSetTree(hVal, call->property, hVal, app->appDB);
            break;
        case pmodeCopySingleMessages:
        case pmodeDeleteSingleMessages:
            /* In these modes we only have to create a root to work with in the future */
            if (!RV_PVT_NODEID_IS_VALID(call->property))
                call->property = pvtAddRootShielded(hVal, app->hSyn, 0, NULL);
            break;
        case pmodeDoNotUseProperty:
            {
                /* No property DB - initialize to invalid nodes */
                callNoPropertiesElem *props = cmiGetNoPropertiesParams(hApp, hsCall);
                props->lastSentMessageNodeId = RV_PVT_INVALID_NODEID;
                memset(props->paramNodeId, RV_PVT_INVALID_NODEID, sizeof(props->paramNodeId));
            }
            break;
    }
    return 0;
}

/************************************************************************
 * callEndParameters
 * purpose: Destroy the property database for the call, with all of its
 *          PVT roots in different modes
 * input  : hsCall  - Stack handle for the call
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int callEndParameters(IN HCALL hsCall)
{
    callElem*call=(callElem*)hsCall;
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    HPVT hVal;
    RvUint32 i;
    if (!call || !app) return RV_ERROR_UNKNOWN;
    hVal=app->hVal;

    /* Delete the main PVT handle for the call's property database */
    if (RV_PVT_NODEID_IS_VALID(call->property))
       pvtDeleteShielded(hVal, call->property);
    call->property = RV_PVT_INVALID_NODEID;

    /* Make sure addresses are deleted */
    pvtDelete(hVal,call->callSignalAddress);
    call->callSignalAddress = RV_PVT_INVALID_NODEID;

    pvtDelete(hVal,call->remoteCallSignalAddress);
    call->remoteCallSignalAddress = RV_PVT_INVALID_NODEID;

    pvtDelete(hVal,call->annexECallSignalAddress);
    call->annexECallSignalAddress = RV_PVT_INVALID_NODEID;

    pvtDelete(hVal,call->destinationInfo);
    call->destinationInfo = RV_PVT_INVALID_NODEID;

    /* No property DB - we should delete the message */
    if (app->callPropertyMode == pmodeDoNotUseProperty)
    {
        callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams((HAPP)app, hsCall);

        pvtDelete(hVal, props->lastSentMessageNodeId);
        props->lastSentMessageNodeId = RV_PVT_INVALID_NODEID;

        /* Delete any properties created in the database */
        for (i = 0; i < (RvUint32)cmEnumLast; i++)
            pvtDelete(hVal, props->paramNodeId[i]);
    }

    return 0;
}

/************************************************************************
 * getFieldIdByMessageType
 * purpose: Return the fieldID of the message in the property DB of the call
 *          by the Q931 message type.
 * input  : messageType - Type of message to get
 * output : none
 * return : FieldID of the message in property DB of call on success
 *          Negative value on failure
 ************************************************************************/
static int getFieldIdByMessageType(cmCallQ931MsgType messageType)
{
    switch(messageType)
    {
        case cmQ931setup            : return __q931(setup);
        case cmQ931callProceeding   : return __q931(callProceeding);
        case cmQ931connect          : return __q931(connect);
        case cmQ931alerting         : return __q931(alerting);
        case cmQ931releaseComplete  : return __q931(releaseComplete);
        case cmQ931status           : return __q931(status);
        case cmQ931facility         : return __q931(facility);
        case cmQ931statusEnquiry    : return __q931(statusEnquiry);
        case cmQ931progress         : return __q931(progress);
        case cmQ931setupAck         : return __q931(setupAck);
        case cmQ931information      : return __q931(information);
        case cmQ931notify           : return __q931(notify);
        default                     : break;
    }
    return RV_ERROR_UNKNOWN;
}


/************************************************************************
 * callGetMessage
 * purpose: Get a Q931 message for the given call. This function will
 *          create the necessary message or just take it from the property
 *          database of the call - all according to the current property
 *          mode used for the stack.
 * input  : hsCall      - Stack's call handle
 *          messageType - Type of message to get
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RvPvtNodeId callGetMessage(IN HCALL hsCall, IN cmCallQ931MsgType messageType)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    int nodeId;
    HPVT hVal;

    if (!call || !app) return RV_ERROR_UNKNOWN;
    hVal=app->hVal;

    /* Get the message by the property mode */
    switch (app->callPropertyMode)
    {
        case pmodeFullProperty:
        {
            RvPvtNodeId origNodeId = RV_PVT_INVALID_NODEID;
            if ((messageType == cmQ931notify) || (messageType == cmQ931progress))
            {
                origNodeId = pvtGetChild(hVal,app->appDB,getFieldIdByMessageType(messageType),NULL);
            }
            /* if no-one deleted the message, we should have it already */
            if ((nodeId=pvtGetChild(hVal,call->property,getFieldIdByMessageType(messageType),NULL)) < 0)
            {
                /* Zounds! we have to add it from the app DB */
                nodeId=pvtAddTree(hVal,call->property,hVal,pvtGetChild(hVal,app->appDB,getFieldIdByMessageType(messageType),NULL));
            }
            else if (RV_PVT_NODEID_IS_VALID(origNodeId))
                pvtSetTree(hVal, nodeId, hVal, origNodeId);
            break;
        }
        case pmodeCopySingleMessages:
        case pmodeDeleteSingleMessages:
        {
            if ((nodeId=pvtGetChild(hVal,call->property,getFieldIdByMessageType(messageType),NULL)) < 0)
            {
                /* We don't really have this message yet - let's get the default one that we have for this stack instance */
                nodeId=pvtAddTree(hVal,call->property,hVal,pvtGetChild(hVal,app->appDB,getFieldIdByMessageType(messageType),NULL));
            }
            break;
        }
        case pmodeDoNotUseProperty:
        {
            /* We don't have a message at all - build it from the call's object */
            callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams((HAPP)app, hsCall);           cmCallParamEnum paramNumber;
            cmCallParam param;
            RvPstFieldId messageFieldId=getFieldIdByMessageType(messageType);
            int q931NodeId,perNodeId, perSpecNodeId;
            int chNodeId=RV_ERROR_UNKNOWN;

            nodeId = pvtAddRoot(hVal, NULL, 0, NULL);

            /* Delete any previously sent message on this call */
            pvtDelete(hVal, props->lastSentMessageNodeId);
            props->lastSentMessageNodeId = nodeId;

            /* Get the default message from the application's default messages */
            pvtSetTree(hVal,nodeId,hVal,pvtGetChild(hVal,app->appDB,messageFieldId,NULL));

            /* Set the parameters that were changed */
            paramNumber = getFirstMessageParameterNumber(messageType);
            if (((int)paramNumber) >= 0)
            {
                /* We have parameters that we'd like to use for this message */

                /* Get some strategic positions in the Q931 message - we will build from there */
                q931NodeId=pvtGetChild2(hVal,nodeId,__q931(message), messageFieldId);
                perNodeId=pvtGetChild2(hVal,q931NodeId,__q931(userUser),__q931(h323_UserInformation));
                perSpecNodeId=pvtGetChild(hVal,pvtGetChild2(hVal,perNodeId,__q931(h323_uu_pdu),__q931(h323_message_body)),messageFieldId,NULL);

                /* Get the parameters until we're dealing with a different message */
                param=getNumberParam(paramNumber);
                while (getParamMessage(param)==messageFieldId)
                {
                    if (RV_PVT_NODEID_IS_VALID(props->paramNodeId[paramNumber]))
                    {
                        /* Build the path to the new parameter */
                        switch (getParamPlace(param))
                        {
                            case placeQ931:
                                chNodeId=pvtBuildByFieldIds(hVal,q931NodeId,getParamShortName(param), 0, NULL);
                            break;
                            case placePER_Gen:
                                chNodeId=pvtBuildByFieldIds(hVal,perNodeId,getParamShortName(param), 0, NULL);
                            break;
                            case placePER_Specific:
                                chNodeId=pvtBuildByFieldIds(hVal,perSpecNodeId,getParamShortName(param), 0, NULL);
                            break;
                        }

                        /* Move the property to the message on the built node */
                        pvtSetTree(hVal,chNodeId,hVal,props->paramNodeId[paramNumber]);
                    }

                    /* Get the next parameter */
                    paramNumber = (cmCallParamEnum)((int)paramNumber+1);
                    param = getNumberParam(paramNumber);
                }
            }
            break;
        }
        default: return RV_ERROR_UNKNOWN;
    }
    {/*Special cases processing*/

        /*Add relaseCompleteCause in case neither cause, no reason present*/
        if (messageType==cmQ931releaseComplete)
        {
            int q931NodeId, reasonNodeId, causeNodeId;
            q931NodeId=pvtChild(hVal,pvtGetChild(hVal,nodeId,__q931(message),NULL));

            __pvtGetNodeIdByFieldIds(reasonNodeId,hVal,q931NodeId,
                {_q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _anyField _q931(reason) LAST_TOKEN});
            /* make sure the choice was set */
            if(pvtChild(hVal, reasonNodeId) < 0)
            {
                pvtDelete(hVal, reasonNodeId);
                reasonNodeId = -1;
            }

            causeNodeId = pvtGetChild(hVal, q931NodeId, __q931(cause), NULL);
            /* make sure the choice was set */
            if(pvtChild(hVal, causeNodeId) < 0)
            {
                pvtDelete(hVal, causeNodeId);
                causeNodeId = -1;
            }

            /* if no reason and no cause were given build at least a cause */
            if ( (reasonNodeId<0) && (causeNodeId < 0) )
                causeNodeId=pvtAdd(app->hVal,q931NodeId,__q931(cause),0,NULL,NULL);

            /* if we have a cause element make sure it is complete */
            if (causeNodeId >= 0)
            {
                int tmpNodeId,tmpNodeId1;

                /* check if octet3 field of the cause is present, if not build a default */
                __pvtGetNodeIdByFieldIds(tmpNodeId,hVal,q931NodeId,
                                        {_q931(cause)
                                         _q931(octet3)
                                         LAST_TOKEN});

                if (tmpNodeId < 0)
                {
                    tmpNodeId=pvtAdd(app->hVal,q931NodeId,__q931(cause),0,NULL,NULL);
                    tmpNodeId1=pvtAdd(app->hVal,tmpNodeId,__q931(octet3),0,NULL,NULL);
                    pvtAdd(app->hVal,tmpNodeId1,__q931(codingStandard),0,NULL,NULL);
                    pvtAdd(app->hVal,tmpNodeId1,__q931(spare),0,NULL,NULL);
                    pvtAdd(app->hVal,tmpNodeId1,__q931(location),0,NULL,NULL);
                }

                /* check if octet4 field of the cause is present, if not build a default (16) */
                __pvtGetNodeIdByFieldIds(tmpNodeId1,hVal,q931NodeId,
                                        {_q931(cause)
                                         _q931(octet4)
                                         LAST_TOKEN});

                if (tmpNodeId1 < 0)
                {
                    tmpNodeId1=pvtAdd(app->hVal,tmpNodeId,__q931(octet4),0,NULL,NULL);
                    pvtAdd(app->hVal,tmpNodeId1,__q931(causeValue),16,NULL,NULL);
                }
            }
        }

        /*Add protocol Identifier in case of facility */
        if (messageType==cmQ931facility)
        {
            char OID[10];
            int length;
            int q931NodeId, facilityNodeId;

            q931NodeId=pvtChild(hVal,pvtGetChild(hVal,nodeId,__q931(message),NULL));

            __pvtGetNodeIdByFieldIds(facilityNodeId,hVal,q931NodeId,
                {_q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(facility) LAST_TOKEN});

            length=utlEncodeOID(sizeof(OID),OID,H225_PROTOCOL_IDENTIFIER);
            pvtAdd(app->hVal,facilityNodeId,__q931(protocolIdentifier),length,OID,NULL);
        }
    }

    cleanMessage(app->hVal, nodeId);

    return nodeId;
}

/************************************************************************
 * callReleaseMessage
 * purpose: This function is called by the stack when it doesn't need a
 *          message for a given call anymore. If the property mode used
 *          allows it, the message will be deleted.
 * input  : hsCall      - Stack's call handle
 *          messageType - Type of message to get
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int callReleaseMessage(IN HCALL hsCall, IN cmCallQ931MsgType messageType)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    HPVT hVal;


    switch (app->callPropertyMode)
    {
    case pmodeDeleteSingleMessages:
        /* Messages are deleted when not needed anymore, but the property DB is used */
        hVal = app->hVal;
        pvtDelete(hVal, pvtGetChild(hVal, call->property, getFieldIdByMessageType(messageType), NULL));
        break;

    case pmodeDoNotUseProperty:
        {
            /* No property DB - we should delete the message */
            callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams((HAPP)app, hsCall);
            hVal = app->hVal;
            pvtDelete(hVal, props->lastSentMessageNodeId);
            props->lastSentMessageNodeId = RV_PVT_INVALID_NODEID;
        }
        break;

    default:
        /* Nothing to delete... */
        break;
    }

    return 0;
}


/************************************************************************
 * callSetMessage
 * purpose: Move a message into the call's database if necessary
 * input  : hsCall      - Stack handle for the Q931 call
 *          messageType - Type of message we're dealing with
 *          message     - Node Id of the massage to handle
 * output : none
 * return : Updated message nodeId to use when handling this message on success
 *          Negative value on failure
 ************************************************************************/
int callSetMessage(IN HCALL hsCall, IN cmCallQ931MsgType messageType, IN int message)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    HPVT hVal;
    int theMessage=RV_ERROR_UNKNOWN;
    int messageFieldId;

    if ((call == NULL) || (app == NULL))
        return RV_ERROR_UNKNOWN;
    hVal = app->hVal;
    messageFieldId = getFieldIdByMessageType(messageType);
    if (messageFieldId < 0)
    {
        /* This type of message is not supported by us */
        return RV_ERROR_UNKNOWN;
    }

    if (app->callPropertyMode!=pmodeDoNotUseProperty)
    {
        /* We have some kind of a property database we can work with - just use it
           to hold this message */
        RvPvtNodeId neNodeId = pvtAdd(hVal, call->property, messageFieldId, 0, NULL, NULL);
        pvtDeleteChilds(hVal, neNodeId);
        pvtShiftTree(hVal,neNodeId,message); /* We use this to leave the message nodeId valid */
        theMessage=neNodeId;
    }
    else
    {
        /* No properties... We need to extract the parameters we can find in this message into
           our call's database. In this case, the resulting message that can be used is the same
           one the user gave us */
        callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams((HAPP)app, hsCall);
        cmCallParamEnum paramNumber;
        cmCallParam param;
        RvPvtNodeId q931NodeId, perNodeId, perSpecNodeId;
        RvPvtNodeId chNodeId = RV_PVT_INVALID_NODEID;

        paramNumber = getFirstMessageParameterNumber(messageType);
        if (((int)paramNumber) >= 0)
        {
            q931NodeId=pvtGetChild2(hVal,message,__q931(message), messageFieldId);
            perNodeId=pvtGetChild2(hVal,q931NodeId,__q931(userUser),__q931(h323_UserInformation));
            perSpecNodeId=pvtGetChild(hVal,pvtGetChild2(hVal,perNodeId,__q931(h323_uu_pdu),__q931(h323_message_body)),messageFieldId,NULL);

            param=getNumberParam(paramNumber);
            while (getParamMessage(param)==messageFieldId)
            {
                if (RV_PVT_NODEID_IS_VALID(props->paramNodeId[paramNumber]))
                {
                    pvtDelete(hVal, props->paramNodeId[paramNumber]);
                    props->paramNodeId[paramNumber] = RV_PVT_INVALID_NODEID;
                }
                {
                    switch (getParamPlace(param))
                    {
                        case placeQ931:
                            chNodeId=pvtGetNodeIdByFieldIds(hVal,q931NodeId,getParamShortName(param));
                        break;
                        case placePER_Gen:
                            chNodeId=pvtGetNodeIdByFieldIds(hVal,perNodeId,getParamShortName(param));
                        break;
                        case placePER_Specific:
                            chNodeId=pvtGetNodeIdByFieldIds(hVal,perSpecNodeId,getParamShortName(param));
                        break;
                    }
                    if (RV_PVT_NODEID_IS_VALID(chNodeId))
                    {
                        if (!RV_PVT_NODEID_IS_VALID(props->paramNodeId[paramNumber]))
                            props->paramNodeId[paramNumber] = pvtAddRoot(hVal,NULL,0,NULL);
                        pvtSetTree(hVal, props->paramNodeId[paramNumber], hVal, chNodeId);
                    }
                }

                paramNumber = (cmCallParamEnum)((int)paramNumber+1);
                param = getNumberParam(paramNumber);
            }
        }
        theMessage=message;
    }

    return theMessage;
}

RvPvtNodeId getCallProperty(HCALL hsCall)
{
    callElem    *call = (callElem *)hsCall;

    if (call == NULL)
        return RV_PVT_INVALID_NODEID;

    return call->property;
}

RVAPI
int RVCALLCONV cmFreeProperty(IN HCALL hsCall)
{
    callElem*call=(callElem*)hsCall;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    int nodeId,nodeDel;

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmFreeProperty: hsCall=%p", hsCall));

    if (emaLock((EMAElement)hsCall))
    {
        HPVT hVal = ((cmElem *)hApp)->hVal;
        if (call->property > 0)
        {
            nodeId = call->property;

            nodeDel = pvtGetChild(hVal,nodeId,__q931(connect),NULL);
            pvtDelete(hVal,nodeDel);
            nodeDel = pvtGetChild(hVal,nodeId,__q931(setup),NULL);
            pvtDelete(hVal,nodeDel);
            nodeDel = pvtGetChild(hVal,nodeId,__q931(alerting),NULL);
            pvtDelete(hVal,nodeDel);
            nodeDel = pvtGetChild(hVal,nodeId,__q931(callProceeding),NULL);
            pvtDelete(hVal,nodeDel);
        }
        else
            if (((cmElem*)hApp)->callPropertyMode==pmodeDoNotUseProperty)
            {
                callEndParameters(hsCall);
            }

        emaUnlock((EMAElement)hsCall);
    }

    cmiAPIExit((hApp, "cmFreeProperty=1"));
    return RV_TRUE;
}


TRANSERR cmEvTransGetMessageNode(IN  HAPPATRANS         hAppATrans,
                                 IN  cmCallQ931MsgType  msgType,
                                 OUT int                *nodeId)
{
    cmElem* app=(cmElem*)hAppATrans;

    if (nodeId)
        *nodeId = pvtGetChild(app->hVal,app->appDB,getFieldIdByMessageType(msgType),NULL);
    return cmTransOK;
}


/************************************************************************
 * cmCallGetMessage
 * purpose: Get a Q931 message for the given call. It creates the necessary
 *          message or just take it from the property database of the call.
 *          This function cannot be used when using DoNotUseProperty
 *          property mode on the calls (determined through the configuration).
 *          It is suggested to use this function when trying to add elements
 *          to an outgoing Q931 message that are not given in cmCallSetParam()
 * input  : hsCall      - Stack's call handle
 *          messageType - Type of message to get
 * output : none
 * return : PVT nodeId of the message on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV cmCallGetMessage(
    IN  HCALL               hsCall,
    IN  cmCallQ931MsgType   messageType)
{
    cmElem* app = (cmElem *)emaGetInstance((EMAElement)hsCall);
    int ret = RV_ERROR_UNKNOWN;

    if ((app == NULL) || (hsCall == NULL))
        return RV_ERROR_UNKNOWN;

    cmiAPIEnter(((HAPP)app, "cmCallGetMessage(hsCall=%p, messageType=%d)", hsCall, messageType));

    if (app->callPropertyMode != pmodeDoNotUseProperty)
    {
        ret = callGetMessage(hsCall, messageType);
        cmiAPIExit(((HAPP)app, "cmCallGetMessage=%d", ret));
    }
    else
    {
        cmiAPIExit(((HAPP)app, "cmCallGetMessage=%d - Bad property mode", ret));
    }

    return ret;
}



#ifdef __cplusplus
}
#endif
