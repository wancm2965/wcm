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

#ifndef _RV_EP_H450_H_
#define _RV_EP_H450_H_

/***********************************************************************
epH450.h

H.450 add-on module of the endpoint.
Handles the supplementary services.
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




/*-----------------------------------------------------------------------*/
/*                           CALLBACK HEADERS                            */
/*-----------------------------------------------------------------------*/





/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/

#ifdef USE_H450

/******************************************************************************
 * epH450Init
 * ----------------------------------------------------------------------------
 * General: Initialize the H.450 module.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         configFilename   - Name of configuration file to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450Init(
    IN EpObj        *ep,
    IN const RvChar *configFilename);


/******************************************************************************
 * epH450End
 * ----------------------------------------------------------------------------
 * General: Destruct the H.450 module of the endpoint.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450End(
    IN EpObj    *ep);


/******************************************************************************
 * epH450Version
 * ----------------------------------------------------------------------------
 * General: Get the version string of the H.450 module used.
 *
 * Return Value: Version string.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
const RvChar *epH450Version(
    IN EpObj    *ep);


/******************************************************************************
 * epH450Start
 * ----------------------------------------------------------------------------
 * General: Start using the H.450 module.
 *          This function is used to set the listening addresses to notify to
 *          external entities.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450Start(
    IN EpObj    *ep);


/******************************************************************************
 * epH450Stop
 * ----------------------------------------------------------------------------
 * General: Stop using the H.450 module.
 *          This function is used to clean used PVT resources.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450Stop(
    IN EpObj    *ep);


/******************************************************************************
 * epH450CallStateChanged
 * ----------------------------------------------------------------------------
 * General: Indication that a call's state has changed.
 *          This function is called from the implementation of
 *          cmEvCallStateChanged().
 *          We don't pass the state and stateMode parameters as we already have
 *          them stored in the call's object.
 *
 * Return Value: Version string.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 * Output: None.
 *****************************************************************************/
void epH450CallStateChanged(
    IN EpCallObj    *call);


/******************************************************************************
 * epH450CallIncomingMessage
 * ----------------------------------------------------------------------------
 * General: Notification of an incoming H.450 message that needs to be
 *          processed.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         msgType      - Type of the received Q931 message.
 *         nodeId       - Root node of the H.450 message.
 *         size         - Size of the message.
 * Output: None.
 *****************************************************************************/
void epH450CallIncomingMessage(
    IN EpCallObj            *call,
    IN cmCallQ931MsgType    msgType,
    IN int                  nodeId,
    IN int                  size);


/******************************************************************************
 * epH450CallInit
 * ----------------------------------------------------------------------------
 * General: Initialize the H.450 part of a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallInit(
    IN EpCallObj    *call);


/******************************************************************************
 * epH450CallEnd
 * ----------------------------------------------------------------------------
 * General: Kill the H.450 part of a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallEnd(
    IN EpCallObj    *call);


/******************************************************************************
 * epH450CallTransfer
 * ----------------------------------------------------------------------------
 * General: H450.2: Call transfer request.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 *         secondaryCall    - Secondary call if consultation.
 *         destination      - Destination address if blind.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallTransfer(
    IN EpCallObj    *call,
    IN EpCallObj    *secondaryCall,
    IN const RvChar *destination);


/******************************************************************************
 * epH450CallTransferResponse
 * ----------------------------------------------------------------------------
 * General: H450.2: Call transfer asynchronous response.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 *         opcode           - Opcode of the response
 *         errCode          - 0 for no error, other for error response.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallTransferResponse(
    IN EpCallObj    *call,
    IN RvInt        opcode,
    IN RvInt        errCode);


/******************************************************************************
 * epH450CallReroute
 * ----------------------------------------------------------------------------
 * General: H450.3: Call reroute request.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 *         destination      - Destination address.
 *         rerouteReason    - Rerouting reason to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallReroute(
    IN EpCallObj        *call,
    IN const RvChar     *destination,
    IN reason_e         rerouteReason);


/******************************************************************************
 * epH450ForwardingRequest
 * ----------------------------------------------------------------------------
 * General: H450.3: Activation/deactivation request.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         destination      - Destination address.
 *         fwdCond          - Forwarding condition.
 *         isActivate       - RV_TRUE for activation, RV_FALSE for deactivation.
 * Output: None.
 *****************************************************************************/
RvStatus epH450ForwardingRequest(
    IN EpObj            *ep,
    IN const RvChar     *destination,
    IN proc_e           fwdCond,
    IN RvBool           isActivate);


/******************************************************************************
 * epH450ForwardInterrogate
 * ----------------------------------------------------------------------------
 * General: H450.3: Interrogation request.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         fwdCond          - Forwarding condition.
 * Output: None.
 *****************************************************************************/
RvStatus epH450ForwardInterrogate(
    IN EpObj            *ep,
    IN proc_e           fwdCond);


/******************************************************************************
 * epH450CallHold
 * ----------------------------------------------------------------------------
 * General: H450.4: Initiate the call hold service.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         isNear       - RV_TRUE for near hold, RV_FALSE for far hold.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallHold(
    IN EpCallObj    *call,
    IN RvBool       isNear);


/******************************************************************************
 * epH450CallHoldRetrieve
 * ----------------------------------------------------------------------------
 * General: H450.4: Retrieve a held call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         isNear       - RV_TRUE for near hold, RV_FALSE for far hold.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallHoldRetrieve(
    IN EpCallObj    *call,
    IN RvBool       isNear);


/******************************************************************************
 * epH450CallHoldSendResponse
 * ----------------------------------------------------------------------------
 * General: H450.4: Send a response on a hold request. Used when we have a
 *          non-default response to handle.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         isHold       - Is this a response for hold or for retrieve?
 *         isOk         - RV_TRUE for ack, RV_FALSE for nack.
 *         errCode      - Error code to use if isOk==RV_FALSE.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallHoldSendResponse(
    IN EpCallObj    *call,
    IN RvBool       isHold,
    IN RvBool       isOk,
    IN RvInt        errCode);


/******************************************************************************
 * epH450CallPark
 * ----------------------------------------------------------------------------
 * General: H450.5: Initiates parking service.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         parkAddress  - Parking address used.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallPark(
    IN EpCallObj    *call,
    IN const RvChar *parkAddress);


/******************************************************************************
 * epH450CallPickup
 * ----------------------------------------------------------------------------
 * General: H450.5: Initiates pick up service.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         call             - Call object to use. NULL for remote.
 *         parkIndex        - Parking index used for remote pickup.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallPickup(
    IN EpObj        *ep,
    IN EpCallObj    *call,
    IN RvInt        parkIndex);


/******************************************************************************
 * epH450GroupNotification
 * ----------------------------------------------------------------------------
 * General: H450.5: Send a group notification on an incoming call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 *         parkPosition     - Parking position to notify about.
 *         inAlerting       - RV_TRUE to handle alerting init.
 * Output: None.
 *****************************************************************************/
RvStatus epH450GroupNotification(
    IN EpCallObj    *call,
    IN RvInt32      parkPosition,
    IN RvBool       inAlerting);


/******************************************************************************
 * epH450CallWait
 * ----------------------------------------------------------------------------
 * General: H450.6: Call wait...
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallWait(
    IN EpCallObj    *call);


/******************************************************************************
 * epH450MwiActivate
 * ----------------------------------------------------------------------------
 * General: H450.7: Sends MWI activate/deactivate message/callback.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         to           - Served user (destination of call).
 *         isActivate   - Is this an activate or deactivate message?
 *         isCallback   - Are we dealing with the callback?
 * Output: None.
 *****************************************************************************/
RvStatus epH450MwiActivate(
    IN EpObj            *ep,
    IN const RvChar     *to,
    IN RvBool           isActivate,
    IN RvBool           isCallback);


/******************************************************************************
 * epH450MwiInterrogate
 * ----------------------------------------------------------------------------
 * General: H450.7: Sends interrogation message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         to           - Served user (destination of call).
 * Output: None.
 *****************************************************************************/
RvStatus epH450MwiInterrogate(
    IN EpObj            *ep,
    IN const RvChar     *to);


/******************************************************************************
 * epH450MwiSendResponse
 * ----------------------------------------------------------------------------
 * General: H450.7: Sends a non default response to MWI messages.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         isOk         - RV_TRUE for ack, RV_FALSE for nack.
 *         errCode      - Error code to use if isOk==RV_FALSE.
 * Output: None.
 *****************************************************************************/
RvStatus epH450MwiSendResponse(
    IN EpCallObj        *call,
    IN RvBool           isOk,
    IN RvInt            errCode);


/******************************************************************************
 * epH450CallSendNameID
 * ----------------------------------------------------------------------------
 * General: H450.8: Sends the name indication.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 *         operation    - Operation to send the name id on.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallSendNameID(
    IN EpCallObj    *call,
    IN niOperation  operation);


/******************************************************************************
 * epH450CallCompletion
 * ----------------------------------------------------------------------------
 * General: H450.9: Sends call completion setup message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         destination      - Destination of completion.
 *         originalCall     - Original call being completed. Can be NULL.
 *         isBusy           - Are we busy?
 *         canRetainService - Can we retain the service?
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallCompletion(
    IN EpObj        *ep,
    IN const RvChar *destination,
    IN EpCallObj    *originalCall,
    IN RvBool       isBusy,
    IN RvBool       canRetainService);


/******************************************************************************
 * epH450CallCompletionMessage
 * ----------------------------------------------------------------------------
 * General: H450.9: Sends a call completion message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         destination      - Destination of completion.
 *         originalCall     - Original call being completed. Can be NULL.
 *         isBusy           - Are we busy?
 *         canRetainService - Can we retain the service?
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallCompletionMessage(
    IN EpObj        *ep,
    IN const RvChar *messageStr,
    IN RvInt32      serviceId);


/******************************************************************************
 * epH450CallOffer
 * ----------------------------------------------------------------------------
 * General: H450.10: Sends call offering.
 *
 * Return Value: Call object to dial.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         overrideCFB  - Do we want to override CFB service?
 * Output: None.
 *****************************************************************************/
EpCallObj *epH450CallOffer(
    IN EpObj        *ep,
    IN RvBool       overrideCFB);


/******************************************************************************
 * epH450CallOfferRemoteUserAlerting
 * ----------------------------------------------------------------------------
 * General: H450.10: Sends call offering.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epH450CallOfferRemoteUserAlerting(
    IN EpCallObj    *call);


/******************************************************************************
 * epH450CallIntrusion
 * ----------------------------------------------------------------------------
 * General: H450.11: Sends call intrusion.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         intrusionType    - Type of intrusion to start.
 *         callId           - Call id to monitor if intrusionType==ssCISilentMoitor.
 *         call             - Call to use. Can be NULL.
 * Output: newCall          - Newly created call to dial if any.
 *****************************************************************************/
RvStatus epH450CallIntrusion(
    IN  EpObj               *ep,
    IN  ciPrimitivesEnum    intrusionType,
    IN  RvUint8             *callId,
    IN  EpCallObj           *call,
    OUT EpCallObj           **newCall);


/******************************************************************************
 * epH450CallCommonInformation
 * ----------------------------------------------------------------------------
 * General: H450.12: Sends a Common Information message on the selected call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         actionType       - Type of action to use.
 *         callId           - Call id to monitor if intrusionType==ssCISilentMoitor.
 *         call             - Call to use. Can be NULL.
 * Output: newCall          - Newly created call to dial if any.
 *****************************************************************************/
RvStatus epH450CallCommonInformation(
    IN  EpCallObj           *call,
    IN  cmnPrimitivesEnum   actionType);



#else
#define epH450Init(_ep, _configFilename) RV_OK
#define epH450End(_ep)
#define epH450Start(_ep)
#define epH450CallStateChanged(_call)
#define epH450CallIncomingMessage(_call, _msgType, _nodeId, _size) {RV_UNUSED_ARG(_msgType); RV_UNUSED_ARG(_nodeId); RV_UNUSED_ARG(_size);}
#define epH450CallInit(_call) RV_OK
#define epH450CallEnd(_call)
#define epH450GroupNotification(_call, _parkPosition, _inAlerting)
#define epH450CallSendNameID(_call, _operation)
#define epH450Stop(_ep) RV_OK
#endif



#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_H450_H_ */
