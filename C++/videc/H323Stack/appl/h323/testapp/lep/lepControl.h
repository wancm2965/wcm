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

#ifndef _RV_EP_CONTROL_H_
#define _RV_EP_CONTROL_H_

/***********************************************************************
epControl.h

H.245 control handling of the endpoint.
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
int RVCALLCONV cmEvCallCapabilities(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      cmCapStruct*        capabilities[]);
int RVCALLCONV cmEvCallCapabilitiesExt(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      cmCapStruct***      capabilities[]);
int RVCALLCONV cmEvCallCapabilitiesResponse(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      RvUint32            status);
int RVCALLCONV cmEvCallMasterSlaveStatus(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      RvUint32            status);
int RVCALLCONV cmEvCallRoundTripDelay(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      RvInt32             delay);
int RVCALLCONV cmEvCallUserInput(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      RvInt32             userInputId);
int RVCALLCONV cmEvCallRequestMode(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      cmReqModeStatus     status,
    IN      RvInt32             nodeId);
int RVCALLCONV cmEvCallMiscStatus(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      cmMiscStatus        status);
int RVCALLCONV cmEvCallControlStateChanged(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      cmControlState      state,
    IN      cmControlStateMode  stateMode);
int RVCALLCONV cmEvCallMasterSlave(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      RvUint32            terminalType,
    IN      RvUint32            statusDeterminationNumber);
int RVCALLCONV cmEvCallControlMessage(
    IN  HAPPCALL            haCall,
    IN  HCALL               hsCall,
    IN  HAPPCHAN            haChan,
    IN  HCHAN               hsChan,
    IN  RvPvtNodeId         message,
    IN  cmH245MessageType   messageType);
int RVCALLCONV cmEvCallTerminalNumberAssign(
    IN      HAPPCALL            haCall,
    IN      HCALL               hsCall,
    IN      RvUint8             mcuNumber,
    IN      RvUint8             terminalNumber);





/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * epCallCreateH245
 * ----------------------------------------------------------------------------
 * General: Create a listening H.245 connection for the call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         addr         - Listening address to use. Can be NULL.
 * Output: None.
 *****************************************************************************/
RvStatus epCallCreateH245(
    IN EpCallObj            *call,
    IN cmTransportAddress   *addr);


/******************************************************************************
 * epCallConnectControl
 * ----------------------------------------------------------------------------
 * General: Connect H.245 control channel for a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallConnectControl(
    IN EpCallObj            *call);


/******************************************************************************
 * epCallSeparateControl
 * ----------------------------------------------------------------------------
 * General: Separate H.245 control channel for a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSeparateControl(
    IN EpCallObj            *call);


/******************************************************************************
 * epCallCloseControl
 * ----------------------------------------------------------------------------
 * General: Close H.245 control channel for a call.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallCloseControl(
    IN EpCallObj            *call);


/******************************************************************************
 * epCallMediaLoopOff
 * ----------------------------------------------------------------------------
 * General: Indicate all channels of a call to stop loopback.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallMediaLoopOff(
    IN EpCallObj            *call);


/******************************************************************************
 * epCallSendCapabilities
 * ----------------------------------------------------------------------------
 * General: Send a TCS message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         nodeId       - RV_PVT_INVALID_NODEID to send the capability set from
 *                        the configuration.
 *         isEmpty      - RV_TRUE to send an empty capability set.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSendCapabilities(
    IN EpCallObj            *call,
    IN RvPvtNodeId          nodeId,
    IN RvBool               isEmpty);


/******************************************************************************
 * epCallSendCapabilitiesAck
 * ----------------------------------------------------------------------------
 * General: Send a TCS ack message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSendCapabilitiesAck(
    IN EpCallObj            *call);


/******************************************************************************
 * epCallSendMasterSlave
 * ----------------------------------------------------------------------------
 * General: Send an MSD message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         terminalType - Terminal type to use. -1 to use the default from
 *                        the configuration.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSendMasterSlave(
    IN EpCallObj            *call,
    IN RvInt32              terminalType);


/******************************************************************************
 * epCallSendRoundTripDelay
 * ----------------------------------------------------------------------------
 * General: Send an RTD message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         seconds      - Maximum delay in seconds.
 * Output: None.
 *****************************************************************************/
RvStatus epCallSendRoundTripDelay(
    IN EpCallObj            *call,
    IN RvInt32              maxDelay);


/******************************************************************************
 * epCallSendUserInputIndication
 * ----------------------------------------------------------------------------
 * General: Send a user input indication message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - Call handle to use.
 *         uii          - User input indication.
 *         nsd          - Non standard parameter to use (can be NULL).
 * Output: None.
 *****************************************************************************/
RvStatus epCallSendUserInputIndication(
    IN EpCallObj            *call,
    IN const RvChar         *uii,
    IN cmNonStandardParam   *nsd);








#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_CONTROL_H_ */
