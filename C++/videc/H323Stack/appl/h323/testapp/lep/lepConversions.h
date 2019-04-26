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

#ifndef _RV_EP_CONVERSIONS_H_
#define _RV_EP_CONVERSIONS_H_

/***********************************************************************
epConversions.h

Conversion functions widely used by this module.
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
/*                           FUNCTIONS HEADERS                           */
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
 *         hexStr   - Resulting string holding the field.
 *         size     - Size of buffer.
 * Output: None.
 *****************************************************************************/
void str2hex(
    IN void     *buffer,
    IN RvChar   *hexStr,
    IN RvSize_t size);


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
    IN RvSize_t     size);



/* Enumeration conversions */

const RvChar *Status2String(IN int status);
int String2Status(IN const RvChar *string);

cmCallState_e String2CMCallState(IN const RvChar *string);
const RvChar *CMCallState2String(IN cmCallState_e state);

cmCallStateMode_e String2CMCallStateMode(IN const RvChar *string);
const RvChar *CMCallStateMode2String(IN cmCallStateMode_e stateMode);

cmReasonType String2CMReasonType(IN const RvChar *string);
const RvChar *CMReasonType2String(IN cmReasonType reason);

cmCallDisconnectionReason String2CMCallDisconnectionReason(IN const RvChar *string);
const RvChar *CMCallDisconnectionReason2String(IN cmCallDisconnectionReason param);

cmCallParam String2CMCallParam(IN const RvChar *string);
const RvChar *CMCallParam2String(IN cmCallParam param);

cmParam String2CMParam(IN const RvChar *string);
const RvChar *CMParam2String(IN cmParam param);

cmRASTransaction String2RASTransaction(IN const RvChar *string);
const RvChar *RASTransaction2String(IN cmRASTransaction transaction);

cmRASReason String2RASReason(IN const RvChar *string);
const RvChar *RASReason2String(IN cmRASReason reason);

cmRASTrStage String2RASTrStage(IN const RvChar *string);
const RvChar *RASTrStage2String(IN cmRASTrStage reason);

cmRASParam String2RASParam(IN const RvChar *string);
const RvChar *RASParam2String(IN cmRASParam reason);

cmChannelState_e String2CMChannelState(IN const RvChar *string);
const RvChar *CMChannelState2String(IN cmChannelState_e state);

cmChannelStateMode_e String2CMChannelStateMode(IN const RvChar *string);
const RvChar *CMChannelStateMode2String(IN cmChannelStateMode_e stateMode);

cmH245Stage String2CMH245Stage(IN const RvChar *string);
const RvChar *CMH245Stage2String(IN cmH245Stage stage);

cmAnnexEUsageMode String2CMAnnexEUsageMode(IN const RvChar *string);
const RvChar *CMAnnexEUsageMode2String(IN cmAnnexEUsageMode stage);

cmExtendedFastConnectState_e String2CMExtendedFastConnectState(const RvChar *string);
const RvChar *CMExtendedFastConnectState2String(cmExtendedFastConnectState_e state);

cmEndpointType String2EndpointType(IN const RvChar *string);
const RvChar *EndpointType2String(IN cmEndpointType val);

cmCallType String2CallType(IN const RvChar *string);
const RvChar *CallType2String(IN cmCallType val);

cmCallModelType String2CallModelType(IN const RvChar *string);
const RvChar *CallModelType2String(IN cmCallModelType val);

cmRASDisengageReason String2DisengageReason(IN const RvChar *string);
const RvChar *DisengageReason2String(IN cmRASDisengageReason val);

cmRASUnregReason String2UnregReason(IN const RvChar *string);
const RvChar *UnregReason2String(IN cmRASUnregReason val);

cmTransportQOS String2TransportQOS(IN const RvChar *string);
const RvChar *TransportQOS2String(IN cmTransportQOS val);

int String2IrrStatus(IN const RvChar *string);
const RvChar *IrrStatus2String(IN int val);


#ifdef USE_H450
/* H.450 conversions */

ssAction String2ssAction(IN const RvChar *string);
const RvChar *ssAction2String(IN ssAction val);

proc_e String2proc_e(IN const RvChar *string);
const RvChar *proc_e2String(IN proc_e proc);

reason_e String2reason_e(IN const RvChar *string);
const RvChar *reason_e2String(IN reason_e reason);

niNameChoice String2H450niNameChoice(IN const RvChar *string);
const RvChar *H450niNameChoice2String(IN niNameChoice val);

ccIndication String2CCIndication(IN const RvChar *string);
const RvChar *CCIndication2String(IN ccIndication indication);

int String2SSPrimitive(IN const RvChar *string);
const RvChar *SSPrimitive2String(IN int opcode);

sseServices String2SSEServices(IN const RvChar *string);
const RvChar *SSEServices2String(IN sseServices opcode);

#endif





/* General conversions */

RvStatus String2CMNonStandardParam(IN const RvChar *string, OUT cmNonStandardParam *param);
const RvChar *CMNonStandardParam2String(IN cmNonStandardParam *param);

RvStatus String2CMAlternateGatekeeper(IN const RvChar *string, OUT cmAlternateGatekeeper *param);
const RvChar *CMAlternateGatekeeper2String(IN cmAlternateGatekeeper *param);

RvStatus String2Alias(IN EpObj *ep, IN const RvChar *str, OUT cmAlias *alias); /* Allocates dynamic memory! */
RvStatus FreeAlias(IN EpObj *ep, IN cmAlias *alias);
const RvChar *Alias2String(IN cmAlias *alias, OUT RvChar *string /* can be NULL */);


#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_CONVERSIONS_H_ */
