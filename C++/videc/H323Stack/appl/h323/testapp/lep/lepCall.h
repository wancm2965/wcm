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

#ifndef _RV_EP_CALL_H_
#define _RV_EP_CALL_H_

/***********************************************************************
epCall.h

Call handling of the endpoint.
Handles the call procedures.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "lepDefs.h"




#ifdef __cplusplus
extern "C" {
#endif



/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/


/* Type of facility message to send out */
typedef enum
{
    EpCallFacilityForward,
    EpCallFacilityEmpty,
    EpCallFacilityReason,
    EpCallFacilityDefault
} EpCallFacilityType;



/*-----------------------------------------------------------------------*/
/*                           CALLBACK HEADERS                            */
/*-----------------------------------------------------------------------*/
int RVCALLCONV cmEvNewCall(
    IN  HAPP        hApp,
    IN  HCALL       hsCall,
    OUT LPHAPPCALL  lphaCall);
int RVCALLCONV cmEvCallStateChanged(
    IN HAPPCALL             haCall,
    IN HCALL                hsCall,
    IN cmCallState_e        state,
    IN cmCallStateMode_e    stateMode);
int RVCALLCONV cmEvCallNewRate(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN RvUint32 rate);
int RVCALLCONV cmEvCallInfo(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN char*    display,
    IN char*    userUser,
    IN int      userUserSize);
int RVCALLCONV cmEvCallFacility(
    IN      HAPPCALL haCall,
    IN      HCALL    hsCall,
    IN      int      handle,
    OUT IN  RvBool   *proceed);
int RVCALLCONV cmEvCallFastStartSetup(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    OUT IN  cmFastStartMessage  *fsMessage);
int RVCALLCONV cmEvCallStatus(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    OUT IN  cmCallStatusMessage *callStatusMsg);
int RVCALLCONV cmEvCallUserInfo(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN int      handle);
int RVCALLCONV cmEvCallH450SupplServ(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      cmCallQ931MsgType   msgType,
    IN      int                 nodeId,
    IN      int                 size);
int RVCALLCONV cmEvCallIncompleteAddress(
    IN  HAPPCALL    haCall,
    IN  HCALL       hsCall);
int RVCALLCONV cmEvCallAdditionalAddress(
    IN HAPPCALL        haCall,
    IN HCALL           hsCall,
    IN char*           address,
    IN RvBool          sendingComplete);
int RVCALLCONV cmEvCallFastStart(
    IN  HAPPCALL                haCall,
    IN  HCALL                   hsCall);
int RVCALLCONV cmEvCallProgress(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN int      handle);
int RVCALLCONV cmEvCallNotify(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN int      handle);
int RVCALLCONV cmEvCallNewAnnexLMessage(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      int                 annexLElement,
    IN      int                 msgType);
int RVCALLCONV cmEvCallNewAnnexMMessage(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      int                 annexMElement,
    IN      int                 msgType);
int RVCALLCONV cmEvCallRecvMessage(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN int      message);
int RVCALLCONV cmEvCallSendMessage(
    IN HAPPCALL haCall,
    IN HCALL    hsCall,
    IN int      message);
void RVCALLCONV cmEvCallStatusEnquiryTimeout(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall);
int RVCALLCONV cmEvCallNatRequest(
    IN      HAPPCALL             haCall,
    IN      HCALL                hsCall,
    IN      cmProtocol           addrType,
    INOUT   cmTransportAddress*  address);





/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * epCallCreate
 * ----------------------------------------------------------------------------
 * General: Create a complete call object, and sets call parameters for it.
 *
 * Return Value: Call object created.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep       - Endpoint object to use.
 *         hsCall   - Incoming call handle, or NULL for outgoing calls.
 *         id       - Id to give to the new call. If 0, will be allocated by
 *                    the allocation resource callback.
 * Output: None.
 *****************************************************************************/
EpCallObj *epCallCreate(
    IN EpObj    *ep,
    IN HCALL    hsCall,
    IN RvInt32  id);


/******************************************************************************
 * epCallDestruct
 * ----------------------------------------------------------------------------
 * General: Close a call object.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallDestruct(
    IN EpCallObj            *call);


/******************************************************************************
 * epCallGetExtraData
 * ----------------------------------------------------------------------------
 * General: Get the extra data of the call object.
 *
 * Return Value: Pointer to the extra data of the call.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
void *epCallGetExtraData(
    IN EpCallObj            *call);


/******************************************************************************
 * epCallSetDestAddress
 * ----------------------------------------------------------------------------
 * General: Add a destination address to the call before dialing it.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         aliasStr     - Alias to add, in string format.
 *         index        - Index of the alias.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSetDestAddress(
    IN EpCallObj            *call,
    IN const RvChar         *aliasStr,
    IN int                  index);


/******************************************************************************
 * epCallDial
 * ----------------------------------------------------------------------------
 * General: Dial out a call to a given IP address (optional).
 *          All settings for this call are done using cmCallSetParam() before
 *          calling this function.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         destIpStr    - Destination IP address. Can be NULL.
 * Output: None.
 *****************************************************************************/
RvStatus epCallDial(
    IN EpCallObj            *call,
    IN const RvChar         *destIpStr);


/******************************************************************************
 * epCallMake
 * ----------------------------------------------------------------------------
 * General: Send a setup message, starting an outgoing call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         to           - Call's destination string to use.
 *         bandwidth    - Bandwidth for this call.
 *         display      - Display information. Can be NULL.
 *         userUser     - User user information (only printable string is
 *                        supported here). Can be NULL.
 *         nsd          - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallMake(
    IN EpCallObj            *call,
    IN const RvChar         *to,
    IN RvInt                bandwidth,
    IN const RvChar         *display,
    IN const RvChar         *userUser,
    IN cmNonStandardParam   *nsd);


/******************************************************************************
 * epCallProceeding
 * ----------------------------------------------------------------------------
 * General: Send a call proceeding message on an incoming call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call handle to use.
 *         nsd      - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallProceeding(
    IN EpCallObj            *call,
    IN cmNonStandardParam   *nsd);


/******************************************************************************
 * epCallAlerting
 * ----------------------------------------------------------------------------
 * General: Send an alerting message on an incoming call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call handle to use.
 *         nsd      - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallAlerting(
    IN EpCallObj            *call,
    IN cmNonStandardParam   *nsd);


/******************************************************************************
 * epCallConnect
 * ----------------------------------------------------------------------------
 * General: Send an alerting message on an incoming call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         display      - Display information.
 *         userUser     - User user information (only printable string is
 *                        supported here).
 *         nsd          - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallConnect(
    IN EpCallObj            *call,
    IN const RvChar         *display,
    IN const RvChar         *userUser,
    IN cmNonStandardParam   *nsd);


/******************************************************************************
 * epCallProgress
 * ----------------------------------------------------------------------------
 * General: Send a progress message on an incoming call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         display      - Display information.
 *         nsd          - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallProgress(
    IN EpCallObj            *call,
    IN const RvChar         *display,
    IN cmNonStandardParam   *nsd);


/******************************************************************************
 * epCallNotify
 * ----------------------------------------------------------------------------
 * General: Send a notify message on a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         description  - Notification description to use.
 *         display      - Display information.
 *         nsd          - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallNotify(
    IN EpCallObj            *call,
    IN RvInt                description,
    IN const RvChar         *display,
    IN cmNonStandardParam   *nsd);


/******************************************************************************
 * epCallUserInformation
 * ----------------------------------------------------------------------------
 * General: Send a user information message on an incoming call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         display      - Display information.
 *         nsd          - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallUserInformation(
    IN EpCallObj            *call,
    IN const RvChar         *display,
    IN cmNonStandardParam   *nsd);


/******************************************************************************
 * epCallStatusEnquiry
 * ----------------------------------------------------------------------------
 * General: Send a status enquiry message on a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         display      - Display information.
 * Output: None.
 *****************************************************************************/
RvStatus epCallStatusEnquiry(
    IN EpCallObj            *call,
    IN const RvChar         *display);


/******************************************************************************
 * epCallFacility
 * ----------------------------------------------------------------------------
 * General: Send a facility message on a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         reason       - Facility reason. Applicable when fType==EpCallFacilityReason.
 *         fwd          - Forwarding address. If exists, then a forwarding
 *                        facility is sent (and the nsd parameter is ignored).
 *         nsd          - Non standard data to use. Can be NULL.
 *         fType        - Type of facility message to send.
 * Output: None.
 *****************************************************************************/
RvStatus epCallFacility(
    IN EpCallObj            *call,
    IN cmReasonType         reason,
    IN const RvChar         *fwd,
    IN cmNonStandardParam   *nsd,
    IN EpCallFacilityType   fType);


/******************************************************************************
 * epCallDrop
 * ----------------------------------------------------------------------------
 * General: Drop a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call handle to use.
 *         reason   - Drop reason to use. -1 if we don't want to set it.
 *         nsd      - Non standard data to use. Can be NULL.
 * Output: None.
 *****************************************************************************/
RvStatus epCallDrop(
    IN EpCallObj            *call,
    IN cmReasonType         reason,
    IN cmNonStandardParam   *nsd);


/******************************************************************************
 * epCallDropAll
 * ----------------------------------------------------------------------------
 * General: Drop all calls.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep       - Endpoint object to use.
 *         reason   - Drop reason to use. -1 if we don't want to set it.
 *         waitDrop - RV_TRUE if we want this function to return only after all
 *                    calls have been dropped.
 * Output: None.
 *****************************************************************************/
RvStatus epCallDropAll(
    IN EpObj                *ep,
    IN cmReasonType         reason,
    IN RvBool               waitDrop);


/******************************************************************************
 * epCallOverlapSending
 * ----------------------------------------------------------------------------
 * General: Send indication of overlap sending.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 *         to               - Destination string to use from the call's origin.
 *                            If NULL, then an indication from the destination
 *                            if the address is complete will be given.
 *         sendingComplete  - 0,1 for indication of address completion.
 *                            -1 for no indication at all.
 * Output: None.
 *****************************************************************************/
RvStatus epCallOverlapSending(
    IN EpCallObj    *call,
    IN const RvChar *to,
    IN RvInt        sendingComplete);


/******************************************************************************
 * epCallGenerateFaststartRequest
 * ----------------------------------------------------------------------------
 * General: Generate an automatic faststart proposal.
 *          This creates 2 channel proposals for an incoming and an outgoing
 *          audio channel for static audio.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         staticAudio  - RV_TRUE to generate only a static audio proposal.
 *                        RV_FALSE to use whatever we have in the configuration.
 * Output: None.
 *****************************************************************************/
RvStatus epCallGenerateFaststartRequest(
    IN EpCallObj *call,
    IN RvBool    staticAudio);


/******************************************************************************
 * epCallFaststartAdd
 * ----------------------------------------------------------------------------
 * General: Add a faststart proposal to a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 *         sessionId        - Session id for the channel's proposal.
 *         channelDirection - Direction of the proposed channel.
 *         bAccept          - True if this is a reconfiguration channel
 *         channelName      - Name of the channel in configuration (can be NULL).
 *         dataTypeNodeId   - Node id to used instead of a channelName.
 *         rtp              - RTP address to use (can be NULL for outgoing
 *                            channels).
 *         rtcp             - RTCP address to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallFaststartAdd(
    IN EpCallObj            *call,
    IN RvInt32              sessionId,
    IN cmChannelDirection   channelDirection,
    IN RvBool               bAccept,
    IN const RvChar         *channelName,
    IN RvPvtNodeId          dataTypeNodeId,
    IN cmTransportAddress   *rtp,
    IN cmTransportAddress   *rtcp);


/******************************************************************************
 * epCallFaststartAckIndex
 * ----------------------------------------------------------------------------
 * General: Acknowledge a faststart proposal.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 *         index            - Index of proposal to acknowledge.
 *         rtp              - RTP address to use (can be NULL for incoming
 *                            channels).
 *         rtcp             - RTCP address to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallFaststartAckIndex(
    IN EpCallObj            *call,
    IN RvInt32              index,
    IN cmTransportAddress   *rtp,
    IN cmTransportAddress   *rtcp);


/******************************************************************************
 * epCallFaststartReady
 * ----------------------------------------------------------------------------
 * General: Indicate that all faststart proposals have been processed.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 *         bAccept          - True if we are sending an acceptance.
 * Output: None.
 *****************************************************************************/
RvStatus epCallFaststartReady(
    IN EpCallObj            *call,
    IN RvBool               bAccept);


/******************************************************************************
 * epCallFaststartRefuse
 * ----------------------------------------------------------------------------
 * General: Refuse incoming faststart proposals.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallFaststartRefuse(
    IN EpCallObj            *call);


/******************************************************************************
 * epCallExtFastConnectCloseAllChannels
 * ----------------------------------------------------------------------------
 * General: Close all fast-connect channels.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallExtFastConnectCloseAllChannels(
    IN EpCallObj            *call);


/******************************************************************************
 * epCallFaststartAutoAck
 * ----------------------------------------------------------------------------
 * General: Automatically acknowledge feasible proposals.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallFaststartAutoAck(
    IN EpCallObj            *call);


/******************************************************************************
 * epCallInitRtpSessions
 * ----------------------------------------------------------------------------
 * General: Initiate call RTP and RTCP sessions.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 * Output: audioPort    - audio port used.
 *         videoPort    - video port used.
 *         dataPort     - data port used.
 *****************************************************************************/
void epCallInitRtpSessions(
    IN EpCallObj            *call,
    OUT int                 *audioPort,
    OUT int                 *videoPort,
    OUT int                 *dataPort);


/******************************************************************************
 * epCallSetOptions
 * ----------------------------------------------------------------------------
 * General: Set general call options from a given configuration struct.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call handle to use.
 *         cfg              - Configuration options to set.
 *                            Negative value parameters in this struct are
 *                            ignored.
 *                            Parameters are set if they are relevant for the
 *                            call's direction only.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSetOptions(
    IN EpCallObj            *call,
    IN EpCallConfig         *cfg);


/******************************************************************************
 * epCallColorIt
 * ----------------------------------------------------------------------------
 * General: Calor a call by the call's conference id and connection handle.
 *          This helps when we want to display it to the GUI and see how calls
 *          relate to each other and on which connections are they being sent.
 *
 * Return Value: The id of the connection of the call.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call object to use.
 * Output: None.
 *****************************************************************************/
RvInt32 epCallColorIt(
    IN EpCallObj *call);


/******************************************************************************
 * epCallGetById
 * ----------------------------------------------------------------------------
 * General: Find a call object by its id.
 *
 * Return Value: The call object if found, NULL otherwise.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep       - Endpoint object to use.
 *         id       - Id of the call to look for.
 * Output: None.
 *****************************************************************************/
EpCallObj *epCallGetById(
    IN EpObj    *ep,
    IN RvInt32  id);


/******************************************************************************
 * epCallGetByCallId
 * ----------------------------------------------------------------------------
 * General: Find a call object by its call identifier.
 *
 * Return Value: The call object if found, NULL otherwise.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         callId       - Call identifier to look for.
 *         isIncoming   - RV_TRUE for incoming call.
 * Output: None.
 *****************************************************************************/
EpCallObj *epCallGetByCallId(
    IN EpObj    *ep,
    IN RvUint8  *callId,
    IN RvBool   isIncoming);


/******************************************************************************
 * epCallSendAnnexLMessage
 * ----------------------------------------------------------------------------
 * General: Sen an annex L message on the call. will force a facility message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         str          - Annex L string.
 *         strLen       - Length of str.
 * Output: None.
 *****************************************************************************/
RvStatus RVCALLCONV epCallSendAnnexLMessage(
    IN      EpCallObj *         call,
    IN      const RvChar *      str,
    IN      int                 strLen);




#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_CALL_H_ */
