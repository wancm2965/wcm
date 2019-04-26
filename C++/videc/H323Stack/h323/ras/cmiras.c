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

#include "rvinternal.h"
#include "stkutils.h"
#include "intutils.h"
#include "cmCrossReference.h"
#include "rvh323watchdog.h"
#include "rasdef.h"
#include "rasin.h"
#include "rasout.h"
#include "rasutils.h"
#include "rasdecoder.h"
#include "cmiras.h"

#ifdef __cplusplus
extern "C" {
#endif





/************************************************************************
 *
 *                          Private constants
 *
 ************************************************************************/

#define RAS_OID_VERSION H225_PROTOCOL_IDENTIFIER

#define RAS_RPOOL_BLOCK_SIZE (512)


/************************************************************************
 * rasMessageInfo
 * Static information about the messages. This information helps us
 * distinguish between incoming and outgoing messages and to find out
 * the transaction type for the messages.
 * The values are from the point of view of messages coming from the
 * network.
 * NOTES:
 * - IRR messages are handled separately
 * - XRS messages are discarded
 * - RIP messages are handled separately as outgoing transaction replies
 ************************************************************************/
const rasMessageInfoStruct rasMessageInfo[] =
{
    /* rasMsgGatekeeperRequest          */ {RAS_IN_TX,  rasTxMsgRequest, cmRASGatekeeper},
    /* rasMsgGatekeeperConfirm          */ {RAS_OUT_TX, rasTxMsgConfirm, cmRASGatekeeper},
    /* rasMsgGatekeeperReject           */ {RAS_OUT_TX, rasTxMsgReject,  cmRASGatekeeper},
    /* rasMsgRegistrationRequest        */ {RAS_IN_TX,  rasTxMsgRequest, cmRASRegistration},
    /* rasMsgRegistrationConfirm        */ {RAS_OUT_TX, rasTxMsgConfirm, cmRASRegistration},
    /* rasMsgRegistrationReject         */ {RAS_OUT_TX, rasTxMsgReject,  cmRASRegistration},
    /* rasMsgUnregistrationRequest      */ {RAS_IN_TX,  rasTxMsgRequest, cmRASUnregistration},
    /* rasMsgUnregistrationConfirm      */ {RAS_OUT_TX, rasTxMsgConfirm, cmRASUnregistration},
    /* rasMsgUnregistrationReject       */ {RAS_OUT_TX, rasTxMsgReject,  cmRASUnregistration},
    /* rasMsgAdmissionRequest           */ {RAS_IN_TX,  rasTxMsgRequest, cmRASAdmission},
    /* rasMsgAdmissionConfirm           */ {RAS_OUT_TX, rasTxMsgConfirm, cmRASAdmission},
    /* rasMsgAdmissionReject            */ {RAS_OUT_TX, rasTxMsgReject,  cmRASAdmission},
    /* rasMsgBandwidthRequest           */ {RAS_IN_TX,  rasTxMsgRequest, cmRASBandwidth},
    /* rasMsgBandwidthConfirm           */ {RAS_OUT_TX, rasTxMsgConfirm, cmRASBandwidth},
    /* rasMsgBandwidthReject            */ {RAS_OUT_TX, rasTxMsgReject,  cmRASBandwidth},
    /* rasMsgDisengageRequest           */ {RAS_IN_TX,  rasTxMsgRequest, cmRASDisengage},
    /* rasMsgDisengageConfirm           */ {RAS_OUT_TX, rasTxMsgConfirm, cmRASDisengage},
    /* rasMsgDisengageReject            */ {RAS_OUT_TX, rasTxMsgReject,  cmRASDisengage},
    /* rasMsgLocationRequest            */ {RAS_IN_TX,  rasTxMsgRequest, cmRASLocation},
    /* rasMsgLocationConfirm            */ {RAS_OUT_TX, rasTxMsgConfirm, cmRASLocation},
    /* rasMsgLocationReject             */ {RAS_OUT_TX, rasTxMsgReject,  cmRASLocation},
    /* rasMsgInfoRequest                */ {RAS_IN_TX,  rasTxMsgRequest, cmRASInfo},
    /* rasMsgInfoRequestResponse        */ {RAS_OUT_TX, rasTxMsgConfirm, cmRASInfo},
    /* rasMsgNonStandardMessage         */ {RAS_IN_TX,  rasTxMsgRequest, cmRASNonStandard},
    /* rasMsgUnknownMessageResponse     */ {RAS_OUT_TX, rasTxMsgReject,  (cmRASTransaction)0},
    /* rasMsgRequestInProgress          */ {RAS_OUT_TX, rasTxMsgOther,   (cmRASTransaction)0},
    /* rasMsgResourcesAvailableIndicate */ {RAS_IN_TX,  rasTxMsgRequest, cmRASResourceAvailability},
    /* rasMsgResourcesAvailableConfirm  */ {RAS_OUT_TX, rasTxMsgConfirm, cmRASResourceAvailability},
    /* rasMsgInfoRequestAck             */ {RAS_OUT_TX, rasTxMsgConfirm, cmRASUnsolicitedIRR},
    /* rasMsgInfoRequestNak             */ {RAS_OUT_TX, rasTxMsgReject,  cmRASUnsolicitedIRR},
    /* rasMsgServiceControlIndication   */ {RAS_IN_TX,  rasTxMsgRequest, cmRASServiceControl},
    /* rasMsgServiceControlResponse     */ {RAS_OUT_TX, rasTxMsgConfirm, cmRASServiceControl}
};





/************************************************************************
 *
 *                          Private functions
 *
 ************************************************************************/


/************************************************************************
 * rasWatchdogResourceCallback
 * purpose: Get a resource parameter from RAS for the watchdog
 * input  : context     - RAS context to use
 *          resource    - Resource to get
 *          type        - Type of resource
 * output : value       - Returned resource value
 * return : Non-negative value on success, other on failure
 ************************************************************************/
static int RVCALLCONV rasWatchdogResourceCallback(
    IN  void*                       context,
    IN  RvUint32                    resource,
    IN  RvH323WatchdogResourceType  type,
    OUT RvUint32*                   value)
{
    rasModule* ras = (rasModule *)context;

    if (resource == ras->rasTimersResourceVal)
    {
        RvRaStatistics statistics;
        if (RvH323TimerStatistics(ras->timers, &statistics) >= 0)
        {
            switch (type)
            {
            case RvH323WatchdogMaxVal: *value = statistics.max; return 0;
            case RvH323WatchdogMaxUsage: *value = statistics.maxUsage; return 0;
            case RvH323WatchdogCurrent: *value = statistics.cur; return 0;
            default:
                return RV_ERROR_UNKNOWN;
            }
        }
        return RV_ERROR_UNKNOWN;
    }

    return RV_ERROR_UNKNOWN;
}


/************************************************************************
 * emaEvOutRasElementIdle
 * purpose: Event function called when the element is idle - element is
 *          still locked when raised.
 * input  : elem      - the idle element
 *          bDeleting - if true, we are now deleting the element,
 *                      otherwise, it is just idle
 * output : none
 * return : none
 ************************************************************************/
static void RVCALLCONV emaEvOutRasElementIdle(EMAElement * elem, RvBool bDeleting)
{
    rasOutTx* tx = (rasOutTx*)elem;
    rasModule* ras = (rasModule *)emaGetUserData((EMAElement)tx);

    if (bDeleting)
    {
        /* the element is now being deleted. kill everything */
        rasReallyCloseOutTx(ras, tx);
    }
}



/************************************************************************
 *
 *                          Public functions
 *
 ************************************************************************/


/************************************************************************
 * cmiRASInitialize
 * purpose: Initialize a RAS instance for use by the CM
 * input  : hApp            - Stack's instance handle
 *          hCat            - CAT instance handle
 *          hVal            - Value "forest" to use for ASN.1 messages
 *          rasConfNode     - RAS configuration node id
 *          maxIncomingTx   - Maximum number of incoming RAS transactions
 *          maxOutgoingTx   - Maximum number of outgoing RAS transactions
 *          maxBufSize      - Maximum buffer size for encoded messages
 *          isGatekeeper    - Indication if we should act as a GK
 *          logMgr          - Log manager to use
 * output : none
 * return : RAS module handle on success
 *          NULL on failure
 ************************************************************************/
HRASMGR cmiRASInitialize(
    IN  HAPP        hApp,
    IN  RVHCAT      hCat,
    IN  HPVT        hVal,
    IN  RvPvtNodeId rasConfNode,
    IN  RvUint32    maxIncomingTx,
    IN  RvUint32    maxOutgoingTx,
    IN  RvUint32    maxBufSize,
    IN  RvBool      isGatekeeper,
    IN  RvLogMgr*   logMgr)
{
    rasModule* ras;
    int numRpoolBlocks, bufSize;
    int status;
    int i, j;

    if (hApp == NULL) return NULL;

    /* Allocate the main RAS object */
    if (RvMemoryAlloc(NULL, sizeof(rasModule), logMgr, (void**)&ras) != RV_OK)
        return NULL;
    memset(ras, 0, sizeof(rasModule));

    ras->logMgr = logMgr;
    ras->app = hApp;
    RvLogSourceConstruct(logMgr, &ras->log, "RAS", "RAS Transactions");
    RvLogSourceConstruct(logMgr, &ras->logChan, "UDPCHAN", "RAS Message Channels");
    RvLogSourceConstruct(logMgr, &ras->logChanWire, "UDPWIRE", "RAS Message Channels");

    /* Allocate outgoing transaction databases */
    if (maxOutgoingTx > 0)
    {
        ras->outRa =
            emaConstruct(sizeof(rasOutTx), (int)maxOutgoingTx, emaNormalLocks, "RAS OUT TX", RAS_OUT_TX, ras, hApp, logMgr);
        ras->outHash =
            hashConstruct((int)(maxOutgoingTx*2), (int)maxOutgoingTx,
                          rasOutgoingHashFunc,  NULL,
                          sizeof(RvUint32), sizeof(rasOutTx*), "RAS OUT HASH", logMgr);

        emaSetIdleEvent(ras->outRa, emaEvOutRasElementIdle);
        emaAddWatchdogResource(ras->outRa, cmiGetWatchdogHandle(hApp), "OutgoingRasTx", "RAS", "Outgoing RAS transactions");
    }

    /* Allocate incoming transaction databases */
    if (maxIncomingTx > 0)
    {
        ras->inRa =
            emaConstruct(sizeof(rasInTx), (int)maxIncomingTx, emaNormalLocks, "RAS IN TX", RAS_IN_TX, ras, hApp, logMgr);
        ras->inHash =
            hashConstruct((int)(maxIncomingTx*2), (int)maxIncomingTx,
                          rasIncomingHashFunc, rasIncomingHashCompare,
                          sizeof(rasInTxKey), sizeof(rasInTx*), "RAS IN HASH", logMgr);

        emaAddWatchdogResource(ras->inRa, cmiGetWatchdogHandle(hApp), "IncomingRasTx", "RAS", "Incoming RAS transactions");
    }

    /* Allocate RPOOL for message retransmissions */
    if (maxBufSize > RAS_RPOOL_BLOCK_SIZE)
        bufSize = RAS_RPOOL_BLOCK_SIZE;
    else
        bufSize = maxBufSize;
    numRpoolBlocks = (maxIncomingTx + maxOutgoingTx) * (maxBufSize / RAS_RPOOL_BLOCK_SIZE);
    if ((numRpoolBlocks > 0) && (bufSize > 0))
    {
        ras->messages = rpoolConstruct(bufSize, numRpoolBlocks, RV_TRUE, "RAS MESSAGES", logMgr);
        rpoolAddWatchdogResource(ras->messages, cmiGetWatchdogHandle(hApp), "RasMessages", "RAS", "RAS encoded outgoing messages");
    }

    /* Create the timers pool */
    if (maxOutgoingTx > 0)
    {
        ras->timers = RvH323TimerConstruct((int)maxOutgoingTx, logMgr, NULL);
        RvWatchdogAddResource(cmiGetWatchdogHandle(hApp), "RasTimers", "RAS", "RAS timers",
            (RvWatchdogResourceCallback_T)rasWatchdogResourceCallback, NULL, NULL,
            ras, &ras->rasTimersResourceVal);
    }

    /* Make sure all allocations succeeded */
    if ((((ras->outRa == NULL) || (ras->outHash == NULL)) && (maxOutgoingTx > 0)) ||
        (((ras->inRa == NULL) || (ras->inHash == NULL)) && (maxIncomingTx > 0)) ||
        ((ras->messages == NULL) && (numRpoolBlocks > 0) && (bufSize > 0)) ||
        ((ras->timers == NULL) && (maxOutgoingTx > 0)))
    {
        if (ras->outRa != NULL) emaDestruct(ras->outRa);
        if (ras->outHash != NULL) hashDestruct(ras->outHash);
        if (ras->inRa != NULL) emaDestruct(ras->inRa);
        if (ras->inHash != NULL) hashDestruct(ras->inHash);
        if (ras->messages != NULL) rpoolDestruct(ras->messages);
        if (ras->timers != NULL) RvH323TimerDestruct(ras->timers);

        return NULL;
    }

    /* Make sure messages are empty */
    for (i = 0; i < (int)cmRASMaxTransaction; i++)
        for (j = 0; j < 4; j++)
            ras->defaultMessages[i][j] = -1;

    /* Construct the syntax trees */
    ras->synMessage = pstConstruct(q931asn1GetSyntax(), "RasMessage");
    ras->synProperty = pstConstruct(q931asn1GetSyntax(), "RasApplicationMessage");

    /* Create mutexes */
    RvLockConstruct(logMgr, &ras->lockInHash);
    RvLockConstruct(logMgr, &ras->lockOutHash);
    RvLockConstruct(logMgr, &ras->lockGarbage);

    /* Set the rest of the parameters */
    ras->confNode = rasConfNode;
    ras->hVal = hVal;
    ras->hCat = hCat;

    ras->termAliasesNode = RV_PVT_INVALID_NODEID;
    ras->gatekeeperCallSignalAddress = RV_PVT_INVALID_NODEID;
    ras->gatekeeperRASAddress = RV_PVT_INVALID_NODEID;
    ras->bufferSize = maxBufSize;
    ras->isGatekeeper = isGatekeeper;

    /* Set the fast-decoder for first 2 fields in messages */
    status = rasDecoderInit(ras);
    if (status < 0)
        return NULL;

    return (HRASMGR)ras;
}


/************************************************************************
 * cmiRASStart
 * purpose: Start a RAS instance for use by the CM
 *          Should be called after cmiRASInitialize() and before any
 *          other function.
 * input  : hRasMgr             - RAS instance handle
 *          rasAddressesNode    - RAS listening addresses node id
 *          startingSeqNum      - Starting sequence number to use
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASStart(
    IN  HRASMGR     hRasMgr,
    IN  RvPvtNodeId rasAddressesNode,
    IN  RvUint32    startingSeqNum)
{
    rasModule*  ras;
    int         i, j;
    RvPvtNodeId nodeId, chNodeId, srcNodeId;
    int         length;
    char        oid[16];

    /* Steps:
     1. Initialize all the messages
     2. Create the default values
     3. Set configuration related default values
     */

    if (hRasMgr == NULL) return RV_ERROR_UNKNOWN;
    ras = (rasModule *)hRasMgr;

    /* Set the starting sequence number to a random value */
    ras->requestSeqNum = (startingSeqNum % 65535) + 2;

    /* Make sure we're working with RAS at all */
    if ((ras->inRa == NULL) && (ras->outRa == NULL))
        return 0;

    /********************************
     * 1. Initialize all the messages
     ********************************/

    /* Initialize all default messages to some root values */
    for (i = 0; i < (int)cmRASMaxTransaction; i++)
        for (j = 1; j < 4; j++)
            ras->defaultMessages[i][j] = pvtAddRoot(ras->hVal, ras->synMessage, 0, NULL);

    /* Encode the object identifier value we're using for our H225 protocol */
    length = utlEncodeOID(sizeof(oid), oid, RAS_OID_VERSION);

    /****************************
     * 2. Create default messages
     ****************************/

    /* gatekeeperRequest */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageRequest], __q931(gatekeeperRequest), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(protocolIdentifier), length, oid, NULL);
    chNodeId = pvtAdd(ras->hVal, nodeId, __q931(endpointType), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(mc), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(undefinedNode), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(supportsAssignedGK), 0, NULL, NULL);

    /* gatekeeperConfirm */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageConfirm], __q931(gatekeeperConfirm), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(protocolIdentifier),length, oid, NULL);

    /* gatekeeperReject */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageReject], __q931(gatekeeperReject), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(protocolIdentifier), length, oid, NULL);
    __pvtBuildByFieldIds(chNodeId, ras->hVal, nodeId, {_q931(rejectReason) _q931(undefinedReason) LAST_TOKEN}, 0, NULL);

    /* registrationRequest */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], __q931(registrationRequest), 0, NULL, NULL);
    pvtAdd(ras->hVal,nodeId, __q931(protocolIdentifier), length, oid, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(discoveryComplete), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(keepAlive), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(willSupplyUUIEs), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(maintainConnection), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(callSignalAddress), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(supportsAssignedGK), 0, NULL, NULL);

    chNodeId = pvtAdd(ras->hVal, nodeId, __q931(terminalType), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(mc), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(undefinedNode), 0, NULL, NULL);

    /* registrationConfirm */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageConfirm], __q931(registrationConfirm), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(protocolIdentifier), length, oid, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(endpointIdentifier), 4, "\0E\0P", NULL);
    pvtAdd(ras->hVal, nodeId, __q931(willRespondToIRR), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(maintainConnection), 0, NULL, NULL);

    /* registrationReject */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageReject], __q931(registrationReject), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(protocolIdentifier), length, oid, NULL);
    __pvtBuildByFieldIds(chNodeId, ras->hVal, nodeId, {_q931(rejectReason) _q931(undefinedReason) LAST_TOKEN}, 0, NULL);

    /* unregistrationRequest */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASUnregistration][cmRASTrStageRequest], __q931(unregistrationRequest), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(callSignalAddress), 0, NULL, NULL);

    /* unregistrationConfirm */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASUnregistration][cmRASTrStageConfirm], __q931(unregistrationConfirm), 0, NULL, NULL);

    /* unregistrationReject */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASUnregistration][cmRASTrStageReject], __q931(unregistrationReject), 0, NULL, NULL);
    __pvtBuildByFieldIds(chNodeId, ras->hVal, nodeId,{_q931(rejectReason) _q931(undefinedReason) LAST_TOKEN}, 0, NULL );

    /* admissionRequest */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASAdmission][cmRASTrStageRequest], __q931(admissionRequest), 0, NULL, NULL);
    __pvtBuildByFieldIds(chNodeId, ras->hVal, nodeId, {_q931(callType) _q931(pointToPoint) LAST_TOKEN}, 0, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(endpointIdentifier), 4, "\0E\0P", NULL);
    pvtAdd(ras->hVal, nodeId, __q931(bandWidth), 640, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(callReferenceValue), 1, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(activeMC), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(answerCall), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(srcInfo), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(canMapAlias), 1, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(willSupplyUUIEs), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(canMapSrcAlias), 1, NULL, NULL);

    /* admissionConfirm */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASAdmission][cmRASTrStageConfirm], __q931(admissionConfirm), 0, NULL, NULL);
    __pvtBuildByFieldIds(chNodeId, ras->hVal, nodeId, {_q931(callModel) _q931(gatekeeperRouted) LAST_TOKEN}, 0, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(bandWidth), 640, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(willRespondToIRR), 0, NULL, NULL);

    chNodeId = pvtAdd(ras->hVal, nodeId, __q931(uuiesRequested), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(setup), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(callProceeding), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(connect), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(alerting), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(information), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(releaseComplete), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(facility), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(progress), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(empty), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(status), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(statusInquiry), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(setupAcknowledge), 0, NULL, NULL);
    pvtAdd(ras->hVal, chNodeId, __q931(notify), 0, NULL, NULL);

    /* admissionReject */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASAdmission][cmRASTrStageReject], __q931(admissionReject), 0, NULL, NULL);
    __pvtBuildByFieldIds(chNodeId, ras->hVal, nodeId, {_q931(rejectReason) _q931(undefinedReason) LAST_TOKEN}, 0, NULL);

    /* bandwidthRequest */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASBandwidth][cmRASTrStageRequest], __q931(bandwidthRequest), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(endpointIdentifier), 4, "\0E\0P", NULL);
    pvtAdd(ras->hVal, nodeId, __q931(bandWidth), 0, NULL, NULL);

    /* bandwidthConfirm */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASBandwidth][cmRASTrStageConfirm], __q931(bandwidthConfirm), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(bandWidth), 0, NULL, NULL);

    /* bandwidthReject */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASBandwidth][cmRASTrStageReject], __q931(bandwidthReject), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(allowedBandWidth), 0, NULL, NULL);

    /* locationRequest */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASLocation][cmRASTrStageRequest], __q931(locationRequest), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(canMapAlias), 1, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(canMapSrcAlias), 0, NULL, NULL);

    /* locationConfirm */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASLocation][cmRASTrStageConfirm], __q931(locationConfirm), 0, NULL, NULL);

    /* locationReject */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASLocation][cmRASTrStageReject], __q931(locationReject), 0, NULL, NULL);
    __pvtBuildByFieldIds(chNodeId, ras->hVal, nodeId, {_q931(rejectReason) _q931(undefinedReason) LAST_TOKEN}, 0, NULL);

    /* disengageRequest */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASDisengage][cmRASTrStageRequest], __q931(disengageRequest), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(endpointIdentifier), 4,"\0E\0P", NULL);
    __pvtBuildByFieldIds(chNodeId, ras->hVal, nodeId, {_q931(disengageReason) _q931(normalDrop) LAST_TOKEN}, 0, NULL);

    /* disengageConfirm */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASDisengage][cmRASTrStageConfirm], __q931(disengageConfirm), 0, NULL, NULL);

    /* disengageReject */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASDisengage][cmRASTrStageReject], __q931(disengageReject), 0, NULL, NULL);

    /* infoRequest */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASInfo][cmRASTrStageRequest], __q931(infoRequest), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(callReferenceValue), 0, NULL, NULL);
    {
        RvUint8 callId[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        __pvtBuildByFieldIds(chNodeId, ras->hVal, nodeId, {_q931(callIdentifier) _q931(guid) LAST_TOKEN}, 16, (char*)callId);
    }

    /* infoRequestResponse - solicited */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASInfo][cmRASTrStageConfirm], __q931(infoRequestResponse), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(callSignalAddress), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(needResponse), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(unsolicited), 0, NULL, NULL);

    /* nonStandardMessage */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASNonStandard][cmRASTrStageRequest], __q931(nonStandardMessage), 0, NULL, NULL);

    /* resourcesAvailableIndicate */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASResourceAvailability][cmRASTrStageRequest], __q931(resourcesAvailableIndicate), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(protocols), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(protocolIdentifier), length, oid, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(endpointIdentifier), 4, "\0E\0P", NULL);

    /* resourcesAvailableConfirm */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASResourceAvailability][cmRASTrStageConfirm], __q931(resourcesAvailableConfirm), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(protocolIdentifier), length, oid, NULL);

    /* infoRequestResponse - unsolicited */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASUnsolicitedIRR][cmRASTrStageRequest], __q931(infoRequestResponse), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(callSignalAddress), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(needResponse), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(unsolicited), 1, NULL, NULL);

    /* infoRequestAck */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASUnsolicitedIRR][cmRASTrStageConfirm], __q931(infoRequestAck), 0, NULL, NULL);

    /* infoRequestNak */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASUnsolicitedIRR][cmRASTrStageReject], __q931(infoRequestNak), 0, NULL, NULL);

    /* serviceControlIndication */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASServiceControl][cmRASTrStageRequest], __q931(serviceControlIndication), 0, NULL, NULL);
    pvtAdd(ras->hVal, nodeId, __q931(serviceControl), 0, NULL, NULL);

    /* serviceControlResponse */
    nodeId = pvtAdd(ras->hVal, ras->defaultMessages[cmRASServiceControl][cmRASTrStageConfirm], __q931(serviceControlResponse), 0, NULL, NULL);

    /*********************************************
     * 3. Set configuration related default values
     *********************************************/

    /* set RAS address in all relevant messages */
    /* GRQ */
    __pvtGetNodeIdByFieldIds(srcNodeId, ras->hVal, rasAddressesNode, {_anyField _q931(address) LAST_TOKEN});
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageRequest], {_anyField _q931(rasAddress) LAST_TOKEN}, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);
    /* GCF */
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageConfirm], {_anyField _q931(rasAddress) LAST_TOKEN}, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);
    /* RRQ */
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(rasAddress) LAST_TOKEN}, 0, NULL);
    {
        RvPvtNodeId srcAddrNodeId;
        srcAddrNodeId = pvtChild(ras->hVal, rasAddressesNode);
        while (RV_PVT_NODEID_IS_VALID(srcAddrNodeId))
        {
            pvtSetTree(ras->hVal, pvtAdd(ras->hVal, nodeId, __anyField, 0, NULL, NULL), ras->hVal, pvtGetChild(ras->hVal, srcAddrNodeId, __q931(address), NULL));
            srcAddrNodeId = pvtBrother(ras->hVal, srcAddrNodeId);
        }
    }
    /* LRQ */
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASLocation][cmRASTrStageRequest], {_anyField _q931(replyAddress) LAST_TOKEN}, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);
    /* IRQ */
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASInfo][cmRASTrStageRequest], {_anyField _q931(replyAddress) LAST_TOKEN}, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);
    /* IRR - solicited */
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASInfo][cmRASTrStageConfirm], {_anyField _q931(rasAddress) LAST_TOKEN}, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);
    /* IRR - unsolicited */
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASUnsolicitedIRR][cmRASTrStageRequest], {_anyField _q931(rasAddress) LAST_TOKEN}, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);

    /* Set endpointVendor in RRQ */
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(endpointVendor) LAST_TOKEN}, 0, NULL);
    __pvtGetNodeIdByFieldIds(srcNodeId, ras->hVal, ras->confNode, {_q931(registrationInfo) _q931(endpointVendor) LAST_TOKEN});
    if (srcNodeId >= 0)
    {
        /* Get it from configuration - registrationInfo.endpointVendor */
        pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);
    }
    else
    {
        __pvtGetNodeIdByFieldIds(srcNodeId, ras->hVal, ras->confNode, {_q931(registrationInfo) _q931(terminalType) _q931(vendor) LAST_TOKEN});
        if (srcNodeId >= 0)
        {
            /* Get it from configuration - registrationInfo.terminalType.vendor */
            pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);
        }
        else
        {
            /* Nothing in configuration - build some default one */
            chNodeId = pvtAdd(ras->hVal, nodeId, __q931(vendor), 0, NULL, NULL);
            pvtAdd(ras->hVal, chNodeId, __q931(t35CountryCode), 0, NULL, NULL);
            pvtAdd(ras->hVal, chNodeId, __q931(t35Extension), 0, NULL, NULL);
            pvtAdd(ras->hVal, chNodeId, __q931(manufacturerCode), 0, NULL, NULL);
        }
    }

    /* Set registration information inside the default messages and return the status */
    return cmiRASUpdateRegInfo((HRASMGR)ras, RV_TRUE);
}


/************************************************************************
 * cmiRASStop
 * purpose: Stop a RAS instance from running
 * input  : hRasMgr - RAS instance handle
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASStop(
    IN  HRASMGR hRasMgr)
{
    rasModule*  ras;
    int i, j;

    if (hRasMgr == NULL) return RV_ERROR_UNKNOWN;
    ras = (rasModule *)hRasMgr;

    /* Run the garbage collection when we stop - this will help the watchdog know if there
       are any resources we should clean up */
    rasGarbageCollection(ras);

    /* Delete all the default messages that we have */
    for (i = 0; i < (int)cmRASMaxTransaction; i++)
        for (j = 1; j < 4; j++)
            if (RV_PVT_NODEID_IS_VALID(ras->defaultMessages[i][j]))
            {
                pvtDelete(ras->hVal, ras->defaultMessages[i][j]);
                ras->defaultMessages[i][j] = RV_PVT_INVALID_NODEID;
            }

    /* Reset GK Address */
    if (RV_PVT_NODEID_IS_VALID(ras->gatekeeperRASAddress))
    {
        pvtDelete(ras->hVal, ras->gatekeeperRASAddress);
        ras->gatekeeperRASAddress = RV_PVT_INVALID_NODEID;
    }
    if (RV_PVT_NODEID_IS_VALID(ras->gatekeeperCallSignalAddress))
    {
        pvtDelete(ras->hVal, ras->gatekeeperCallSignalAddress);
        ras->gatekeeperCallSignalAddress = RV_PVT_INVALID_NODEID;
    }

    /* Reset terminalAlias list */
    if (RV_PVT_NODEID_IS_VALID(ras->termAliasesNode))
    {
        pvtDelete(ras->hVal, ras->termAliasesNode);
        ras->termAliasesNode = RV_PVT_INVALID_NODEID;
    }

    return 0;
}


/************************************************************************
 * cmiRASEnd
 * purpose: Free any resources taken by a RAS instance and stop its
 *          activities.
 * input  : hRasMgr - RAS instance handle
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASEnd(
    IN  HRASMGR hRasMgr)
{
    rasModule*  ras;

    if (hRasMgr == NULL)
        return RV_ERROR_UNKNOWN;
    ras = (rasModule *)hRasMgr;

    cmiRASStop(hRasMgr);

    rasDecoderEnd(ras);

    /* Remove hash tables and arrays */
    if (ras->outRa != NULL) emaDestruct(ras->outRa);
    if (ras->outHash != NULL) hashDestruct(ras->outHash);
    if (ras->inRa != NULL) emaDestruct(ras->inRa);
    if (ras->inHash != NULL) hashDestruct(ras->inHash);
    if (ras->messages != NULL) rpoolDestruct(ras->messages);
    if (ras->timers != NULL) RvH323TimerDestruct(ras->timers);

    /* Destruct mutexes */
    RvLockDestruct(&ras->lockInHash, ras->logMgr);
    RvLockDestruct(&ras->lockOutHash, ras->logMgr);
    RvLockDestruct(&ras->lockGarbage, ras->logMgr);

    /* Destruct syntax trees used */
    pstDestruct(ras->synMessage);
    pstDestruct(ras->synProperty);

    RvLogSourceDestruct(&ras->log);
    RvLogSourceDestruct(&ras->logChan);
    RvLogSourceDestruct(&ras->logChanWire);

    RvMemoryFree(ras, ras->logMgr);

    return 0;
}


/************************************************************************
 * cmiRASGetEndpointID
 * purpose: Retrieves the EndpointID stored in the ras
 *
 * input  : hRasMgr             - RAS instance handle
 *          eId                 - pointer to the buffer for endpoint ID
 *                                buffer should be big enough for longest EID
 *                                possible (256 byte)
 * output : none
 * return : The length of EID in bytes on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASGetEndpointID(
    IN  HRASMGR hRasMgr,
    IN  void*   eId)
{
    rasModule*  ras;
    if (hRasMgr == NULL) return RV_ERROR_UNKNOWN;
    ras = (rasModule *)hRasMgr;

    if (eId)
        memcpy(eId, ras->epId, (RvSize_t)RvMin(256,ras->epIdLen));
    return ras->epIdLen;
}

/************************************************************************
 * cmiRASUpdateRegInfo
 * purpose: Update the registration information inside the default RAS
 *          messages. This function should be called whenever the
 *          registration status changes.
 *          It takes most of the information from the RAS configuration
 *          message in the PVT database.
 * input  : hRasMgr             - RAS instance handle
 *          beforeRegistration  - RV_TRUE if called before cmRegister()
 *                                RV_FALSE if called after RCF received
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASUpdateRegInfo(
    IN  HRASMGR hRasMgr,
    IN  RvBool  beforeRegistration)
{
    rasModule*  ras;
    RvPvtNodeId destNodeId, srcNodeId;
    char        id[256];
    char*       pId = NULL;
    int         lenId;

    if (hRasMgr == NULL) return RV_ERROR_UNKNOWN;
    ras = (rasModule *)hRasMgr;

    if (beforeRegistration)
    {
        /* Reset GK Address */
        pvtDelete(ras->hVal, ras->gatekeeperRASAddress);
        ras->gatekeeperRASAddress = RV_PVT_INVALID_NODEID;
        pvtDelete(ras->hVal, ras->gatekeeperCallSignalAddress);
        ras->gatekeeperCallSignalAddress = RV_PVT_INVALID_NODEID;

        /* Reset terminalAlias list */
        pvtDelete(ras->hVal, ras->termAliasesNode);
        ras->termAliasesNode = RV_PVT_INVALID_NODEID;
    }

    if (!RV_PVT_NODEID_IS_VALID(ras->termAliasesNode))
    {
        __pvtGetNodeIdByFieldIds(srcNodeId, ras->hVal, ras->confNode, {_q931(registrationInfo) _q931(terminalAlias) LAST_TOKEN});

        /* Get the current aliases in the configuration for the messages */
        if (RV_PVT_NODEID_IS_VALID(srcNodeId))
        {
            /* We have aliases in the configuration */
            ras->termAliasesNode = pvtAddRoot(ras->hVal,NULL,0,NULL);
            pvtSetTree(ras->hVal, ras->termAliasesNode, ras->hVal, srcNodeId);
        }
    }
    if (RV_PVT_NODEID_IS_VALID(ras->termAliasesNode))
    {
        srcNodeId = ras->termAliasesNode;

        /* Set the terminalAlias in RRQ, GRQ, IRR if we have one */
        /* RRQ */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(terminalAlias) LAST_TOKEN}, 0, NULL);
        pvtSetTree(ras->hVal, destNodeId, ras->hVal, srcNodeId);

        /* GRQ */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageRequest], {_anyField _q931(endpointAlias) LAST_TOKEN}, 0, NULL);
        pvtSetTree(ras->hVal, destNodeId, ras->hVal, srcNodeId);

        /* IRR - solicited */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASInfo][cmRASTrStageConfirm], {_anyField _q931(endpointAlias) LAST_TOKEN}, 0, NULL);
        pvtSetTree(ras->hVal, destNodeId, ras->hVal, srcNodeId);

        /* IRR - unsolicited */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASUnsolicitedIRR][cmRASTrStageRequest], {_anyField _q931(endpointAlias) LAST_TOKEN}, 0, NULL);
        pvtSetTree(ras->hVal, destNodeId, ras->hVal, srcNodeId);
    }
    else
    {
        /* RRQ */
        __pvtGetByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(terminalAlias) LAST_TOKEN}, NULL, NULL, NULL);
        pvtDelete(ras->hVal, destNodeId);

        /* GRQ */
        __pvtGetByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageRequest], {_anyField _q931(endpointAlias) LAST_TOKEN}, NULL, NULL, NULL);
        pvtDelete(ras->hVal, destNodeId);

        /* IRR - solicited */
        __pvtGetByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASInfo][cmRASTrStageConfirm], {_anyField _q931(endpointAlias) LAST_TOKEN}, NULL, NULL, NULL);
        pvtDelete(ras->hVal, destNodeId);

        /* IRR - unsolicited */
        __pvtGetByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASUnsolicitedIRR][cmRASTrStageRequest], {_anyField _q931(endpointAlias) LAST_TOKEN}, NULL, NULL, NULL);
        pvtDelete(ras->hVal, destNodeId);
    }

    /* Set the terminalType in RRQ, GRQ, IRR if we have one */
    __pvtGetNodeIdByFieldIds(srcNodeId, ras->hVal, ras->confNode, {_q931(registrationInfo) _q931(terminalType) LAST_TOKEN});
    if (RV_PVT_NODEID_IS_VALID(srcNodeId))
    {
        /* RRQ */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(terminalType) LAST_TOKEN}, 0, NULL);
        pvtSetTree(ras->hVal, destNodeId, ras->hVal, srcNodeId);

        /* GRQ */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageRequest], {_anyField _q931(endpointType) LAST_TOKEN}, 0, NULL);
        pvtSetTree(ras->hVal, destNodeId, ras->hVal, srcNodeId);

        /* IRR - solicited */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASInfo][cmRASTrStageConfirm], {_anyField _q931(endpointType) LAST_TOKEN}, 0, NULL);
        pvtSetTree(ras->hVal, destNodeId, ras->hVal, srcNodeId);

        /* IRR - unsolicited */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASUnsolicitedIRR][cmRASTrStageRequest], {_anyField _q931(endpointType) LAST_TOKEN}, 0, NULL);
        pvtSetTree(ras->hVal, destNodeId, ras->hVal, srcNodeId);
    }

    /* Set the endpointIdentifier in relevant messages if we have one */
    if (!beforeRegistration)
    {
        /* We're registered... get from RAS object */
        pId = ras->epId;
        lenId = ras->epIdLen;

        if (lenId > 0)
        {
            /* RRQ */
            __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN}, lenId, pId);

            /* RCF */
            __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageConfirm], {_anyField _q931(endpointIdentifier) LAST_TOKEN}, lenId, pId);

            /* URQ */
            __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASUnregistration][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN}, lenId, pId);

            /* ARQ */
            __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASAdmission][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN}, lenId, pId);

            /* BRQ */
            __pvtBuildByFieldIds(destNodeId, ras->hVal,ras->defaultMessages[cmRASBandwidth][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN}, lenId, pId);

            /* DRQ */
            __pvtBuildByFieldIds(destNodeId, ras->hVal,ras->defaultMessages[cmRASDisengage][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN}, lenId, pId);

            /* IRR - solicited */
            __pvtBuildByFieldIds(destNodeId, ras->hVal,ras->defaultMessages[cmRASInfo][cmRASTrStageConfirm], {_anyField _q931(endpointIdentifier) LAST_TOKEN}, lenId, pId);

            /* IRR - unsolicited */
            __pvtBuildByFieldIds(destNodeId, ras->hVal,ras->defaultMessages[cmRASUnsolicitedIRR][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN}, lenId, pId);

            /* RAI */
            __pvtBuildByFieldIds(destNodeId, ras->hVal,ras->defaultMessages[cmRASResourceAvailability][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN}, lenId, pId);

            /* SCI */
            __pvtBuildByFieldIds(destNodeId, ras->hVal,ras->defaultMessages[cmRASServiceControl][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN}, lenId, pId);
        }
    }
    else
    {
        /* Before registration - delete EP ID */
        ras->epId[0] = '\0';
        ras->epIdLen = 0;

        /* Make sure we don't have the EP id in any of the messages */
        /* RRQ */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal,ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        /* URQ */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASUnregistration][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        /* ARQ */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASAdmission][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        /* BRQ */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASBandwidth][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        /* DRQ */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASDisengage][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        /* IRR - solicited */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASInfo][cmRASTrStageConfirm], {_anyField _q931(endpointIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        /* IRR - unsolicited */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASUnsolicitedIRR][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        /* RAI */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASResourceAvailability][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        /* SCI */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASServiceControl][cmRASTrStageRequest], {_anyField _q931(endpointIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);
    }

    /* Set the gatekeeperIdentifier in relevant messages if we have one */
    if (beforeRegistration)
    {
        __pvtGetByFieldIds(srcNodeId, ras->hVal, ras->confNode, {_q931(registrationInfo) _q931(gatekeeperIdentifier) LAST_TOKEN}, NULL, &lenId, NULL);
        if (RV_PVT_NODEID_IS_VALID(srcNodeId))
        {
            pvtGetString(ras->hVal, srcNodeId, sizeof(id), id);
            pId = id;
        }
        else
            lenId = 0;
    }
    else
    {
        /* We're registered... get from RAS object */
        pId = ras->gkId;
        lenId = ras->gkIdLen;
    }
    if (lenId > 0)
    {
        /* RRQ */
        __pvtBuildByFieldIds(destNodeId, ras->hVal,ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN}, lenId, pId);

        /* GRQ */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageRequest], {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN}, lenId, pId);

        /* URQ */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASUnregistration][cmRASTrStageRequest], {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN}, lenId, pId);

        /* ARQ */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASAdmission][cmRASTrStageRequest], {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN}, lenId, pId);

        /* BRQ */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASBandwidth][cmRASTrStageRequest], {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN}, lenId, pId);

        /* DRQ */
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASDisengage][cmRASTrStageRequest], {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN}, lenId, pId);
    }
    else
    {
        /* Make sure we don't have the GK id in any of the messages */
        /* RRQ */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal,ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageRequest], {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        /* URQ */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASUnregistration][cmRASTrStageRequest], {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        /* ARQ */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASAdmission][cmRASTrStageRequest], {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        /* BRQ */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASBandwidth][cmRASTrStageRequest], {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);

        /* DRQ */
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASDisengage][cmRASTrStageRequest], {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);
    }

    /* Set timeToLive in default RRQ message */
    __pvtGetNodeIdByFieldIds(srcNodeId, ras->hVal, ras->confNode, {_q931(registrationInfo) _q931(timeToLive) LAST_TOKEN});
    if (RV_PVT_NODEID_IS_VALID(srcNodeId))
    {
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(timeToLive) LAST_TOKEN}, 0, NULL);
        pvtSetTree(ras->hVal, destNodeId, ras->hVal, srcNodeId);
    }
    else
    {
        __pvtGetNodeIdByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(timeToLive) LAST_TOKEN});
        pvtDelete(ras->hVal, destNodeId);
    }

    /* Set supportAltGK in default RRQ/GRQ messages */
    srcNodeId = pvtGetChild(ras->hVal, ras->confNode, __q931(supportAltGK), NULL);
    if (RV_PVT_NODEID_IS_VALID(srcNodeId))
    {
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(supportsAltGK) LAST_TOKEN}, 0, NULL);
        __pvtBuildByFieldIds(destNodeId, ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageRequest], {_anyField _q931(supportsAltGK) LAST_TOKEN}, 0, NULL);
    }

    return 0;
}


/************************************************************************
 * cmiRASUpdateCallSignalingAddress
 * purpose: Update the CallSignalling address inside the default RAS
 *          messages. This function should be called whenever this
 *          address changes.
 *          It takes most of the information from the given nodeId
 * input  : hRasMgr     - RAS instance handle
 *          addressId   - nodeId of the addresses to set
 *          annexEId    - nodeId of the annexE addresses to set
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASUpdateCallSignalingAddress(
    IN  HRASMGR     hRasMgr,
    IN  RvPvtNodeId addressId,
    IN  RvPvtNodeId annexEId)
{
    rasModule*  ras;
    RvPvtNodeId nodeId;
    static RvInt16 q931TpktPathId[] = {_anyField _q931(callSignalAddress) LAST_TOKEN};

    if (hRasMgr == NULL) return RV_ERROR_UNKNOWN;
    ras = (rasModule *)hRasMgr;

    /* RRQ (TPKT) */
    nodeId = pvtGetNodeIdByFieldIds(ras->hVal,ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], q931TpktPathId);
    pvtDelete(ras->hVal, nodeId);
    nodeId = pvtBuildByFieldIds(ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], q931TpktPathId, 0, NULL);
    {
        RvPvtNodeId srcAddrNodeId;
        srcAddrNodeId = pvtChild(ras->hVal, addressId);
        while (RV_PVT_NODEID_IS_VALID(srcAddrNodeId))
        {
            pvtSetTree(ras->hVal, pvtAdd(ras->hVal, nodeId, __anyField, 0, NULL, NULL), ras->hVal, pvtGetChild(ras->hVal, srcAddrNodeId, __q931(address), NULL));
            srcAddrNodeId = pvtBrother(ras->hVal, srcAddrNodeId);
        }
    }

    /* After converting once to an address list without a scopeId, we don't want to do this process
       all over again, so just change the source to the converted node. */
    addressId = nodeId;

    /* RRQ (ANNEX E) */
    if (annexEId >= 0)
    {
        __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(alternateTransportAddresses) _q931(annexE) LAST_TOKEN}, 0, NULL);
        pvtSetTree(ras->hVal, nodeId, ras->hVal, annexEId);
    }

    /* RCF */
    nodeId = pvtBuildByFieldIds(ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageConfirm], q931TpktPathId, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, addressId);

    /* URQ */
    nodeId = pvtBuildByFieldIds(ras->hVal, ras->defaultMessages[cmRASUnregistration][cmRASTrStageRequest], q931TpktPathId, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, addressId);

    /* IRR - solicited */
    nodeId = pvtBuildByFieldIds(ras->hVal, ras->defaultMessages[cmRASInfo][cmRASTrStageConfirm], q931TpktPathId, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, addressId);

    /* IRR - unsolicited */
    nodeId = pvtBuildByFieldIds(ras->hVal, ras->defaultMessages[cmRASUnsolicitedIRR][cmRASTrStageRequest], q931TpktPathId, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, addressId);

    return 0;
}


/************************************************************************
 * cmiRASUpdateRasAddress
 * purpose: Update the RAS address inside the default RAS
 *          messages. This function should be called whenever these
 *          adresses change.
 *          It takes most of the information from the given nodeId
 * input  : hRasMgr     - RAS instance handle
 *          rasAddressesNode   - nodeId of the addresses to set
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASUpdateRasAddresses(
	IN  HRASMGR     hRasMgr,
	IN  RvPvtNodeId rasAddressesNode)
{
	rasModule* ras = (rasModule*)hRasMgr;
	RvPvtNodeId nodeId, srcNodeId;

	 /* set RAS address in all relevant messages */
    /* GRQ */
    __pvtGetNodeIdByFieldIds(srcNodeId, ras->hVal, rasAddressesNode, {_anyField _q931(address) LAST_TOKEN});
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageRequest], {_anyField _q931(rasAddress) LAST_TOKEN}, 0, NULL);
	
	pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);
	/* GCF */
   __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASGatekeeper][cmRASTrStageConfirm], {_anyField _q931(rasAddress) LAST_TOKEN}, 0, NULL);

	pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);
    
	/* RRQ */
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASRegistration][cmRASTrStageRequest], {_anyField _q931(rasAddress) LAST_TOKEN}, 0, NULL);
    /* Delete the old addresses before adding the new ones */
    pvtDeleteChilds(ras->hVal, nodeId);
    {
        RvPvtNodeId srcAddrNodeId;
        srcAddrNodeId = pvtChild(ras->hVal, rasAddressesNode);
        while (RV_PVT_NODEID_IS_VALID(srcAddrNodeId))
        {
            pvtSetTree(ras->hVal, pvtAdd(ras->hVal, nodeId, __anyField, 0, NULL, NULL), ras->hVal, pvtGetChild(ras->hVal, srcAddrNodeId, __q931(address), NULL));
            srcAddrNodeId = pvtBrother(ras->hVal, srcAddrNodeId);
        }
    }
    
	/* LRQ */
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASLocation][cmRASTrStageRequest], {_anyField _q931(replyAddress) LAST_TOKEN}, 0, NULL);
	pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);
    /* IRQ */
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASInfo][cmRASTrStageRequest], {_anyField _q931(replyAddress) LAST_TOKEN}, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);
    
	/* IRR - solicited */
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASInfo][cmRASTrStageConfirm], {_anyField _q931(rasAddress) LAST_TOKEN}, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);
    /* IRR - unsolicited */
    __pvtBuildByFieldIds(nodeId, ras->hVal, ras->defaultMessages[cmRASUnsolicitedIRR][cmRASTrStageRequest], {_anyField _q931(rasAddress) LAST_TOKEN}, 0, NULL);
    pvtSetTree(ras->hVal, nodeId, ras->hVal, srcNodeId);

	return RV_OK;
}


/************************************************************************
 * cmiRASGetHAPP
 * purpose: Get the application's handle of a RAS instance for a
 *          specific stack's RAS transaction handle
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : The application's handle of the RAS instance on success
 *          NULL on failure
 ************************************************************************/
HAPP cmiRASGetHAPP(
    IN  HRAS hsRas)
{
    rasModule*  ras;

    /* Find out if it's incoming or outgoing */
    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);

    return ras->app;
}



/************************************************************************
 * cmiRASGetRequest
 * purpose: Gets the pvt node of the current RAS request
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : pvt node of the current RAS request
 *          or negative value on error
 ************************************************************************/
RvPvtNodeId cmiRASGetRequest(
    IN  HRAS         hsRas)
{
    rasModule* ras;
    RvPvtNodeId propertyDb;

    if (hsRas == NULL) return RV_ERROR_UNKNOWN;

    /* Check if it's an incoming transaction or an outgoing transaction */
    switch (emaGetType((EMAElement)hsRas))
    {
        case RAS_OUT_TX:
        {
            rasOutTx* outTx = (rasOutTx *)hsRas;
            propertyDb = outTx->txProperty;
            break;
        }
        case RAS_IN_TX:
        {
            rasInTx* inTx = (rasInTx *)hsRas;
            propertyDb = inTx->txProperty;
            break;
        }
        default:
            return RV_ERROR_UNKNOWN;
    }

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);

    /* Get the request from the property database */
    return pvtGetChild(ras->hVal, propertyDb, __q931(request), NULL);
}


/************************************************************************
 * cmiRASGetResponse
 * purpose: Gets the pvt node of the current RAS response
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : pvt node of the current RAS response
 *          or negative value on error
 ************************************************************************/
RvPvtNodeId cmiRASGetResponse(
    IN  HRAS         hsRas)
{
    rasModule* ras;
    RvPvtNodeId propertyDb;

    if (hsRas == NULL) return RV_ERROR_UNKNOWN;

    /* Check if it's an incoming transaction or an outgoing transaction */
    switch (emaGetType((EMAElement)hsRas))
    {
        case RAS_OUT_TX:
        {
            rasOutTx* outTx = (rasOutTx *)hsRas;
            propertyDb = outTx->txProperty;
            break;
        }
        case RAS_IN_TX:
        {
            rasInTx* outTx = (rasInTx *)hsRas;
            propertyDb = outTx->txProperty;
            break;
        }
        default:
            return RV_ERROR_UNKNOWN;
    }

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);

    /* Get the request from the property database */
    return pvtGetChild(ras->hVal, propertyDb, __q931(response), NULL);
}


/************************************************************************
 * cmiRASReceiveMessage
 * purpose: Notify the RAS instance that a message was received from the
 *          network and should be handled.
 * input  : hRasMgr         - RAS instance that needs to handle the message
 *          index           - Index of the connection the message was
 *                            received to
 *          isMulticast     - RV_TRUE if message was received on a multicast
 *                            connection
 *          srcAddress      - Address of the sender
 *          message         - The message received
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns RV_OK.
 ************************************************************************/
RvStatus cmiRASReceiveMessage(
    IN HRASMGR              hRasMgr,
    IN RvInt                index,
    IN RvBool               isMulticast,
    IN cmTransportAddress   *srcAddress,
    IN RvUint8*             messageBuf,
    IN RvSize_t             messageLength)
{
    rasModule*  ras = (rasModule *)hRasMgr;
    RvUint32    messageIndex, seqNum;
    RvStatus    status;
    void*       hMsgContext = NULL;

    /* Check out which message is it */
    status = rasDecodePart(ras, messageBuf, (RvUint32)messageLength, &messageIndex, &seqNum);
    if (status != RV_OK)
        return status;
    if (messageIndex >= (RvUint32)rasMsgLast)
    {
        RvLogError(&ras->log,
            (&ras->log, "cmiRASReceiveMessage: Index out of bounds (%d)", messageIndex));
        return RV_ERROR_UNKNOWN;
    }

    /* Route the message to the right transaction type */
    status = rasRouteMessage(ras, index, isMulticast, srcAddress, messageBuf, (RvUint32)messageLength, -1, (rasMessages)messageIndex, seqNum, &hMsgContext);

    /* If we have a message context at this point, then we have to release it... */
    if (hMsgContext != NULL)
        ras->cmiEvRASReleaseMessageContext(hMsgContext);

    return status;
}



/************************************************************************
 * cmiRASSetSendEventHandler
 * purpose: Set the callback function to use for sending messages through
 *          the network.
 * input  : hRasMgr         - RAS module to set
 *          evMessageToSend - Callback function to use
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int cmiRASSetSendEventHandler(
    IN  HRASMGR                 hRasMgr,
    IN  cmiEvRasMessageToSend   evMessageToSend)
{
    rasModule* ras;

    if (hRasMgr == NULL) return RV_ERROR_UNKNOWN;
    ras = (rasModule *)hRasMgr;

    ras->evSendMessage = evMessageToSend;
    return 0;
}


/************************************************************************
 * cmiRASSetTrEventHandler
 * purpose: Set the callback function to use for responses to RAS
 *          transactions.
 *          It is called per transaction if the stack wishes to be
 *          notified of the response.
 * input  : hRas            - The application's RAS transaction handle
 *                            to deal with
 *          cmEvRASResponse - The callback to use
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int cmiRASSetTrEventHandler(
    IN  HRAS                hRas,
    IN  cmiEvRASResponseT   cmEvRASResponse)
{
    rasOutTx*   tx;

    if (hRas == NULL) return RV_ERROR_UNKNOWN;

    tx = (rasOutTx *)hRas;

    if (emaLock((EMAElement)hRas))
    {
#if defined(RV_RAS_DEBUG)
        /* Make sure the pointer is an outgoing transaction */
        if (emaGetType((EMAElement)hRas) != RAS_OUT_TX)
        {
            RvLogSource * log;
            log = &((rasModule *)emaGetUserData((EMAElement)hRas))->log;

            RvLogExcep(log,
                (log, "cmiRASSetTrEventHandler: Not an outgoing transaction %p", hRas));
            return RV_ERROR_UNKNOWN;
        }

        if (tx->state != rasTxStateIdle)
        {
            RvLogSource * log;
            log = &((rasModule *)emaGetUserData((EMAElement)hRas))->log;

            RvLogExcep(log,
                (log, "cmiRASSetTrEventHandler: Called after request was sent for transaction %p", hRas));
            return RV_ERROR_UNKNOWN;
        }
#endif /* defined(RV_RAS_DEBUG) */

        /* Set the event handler */
        tx->evResponse = cmEvRASResponse;
        emaUnlock((EMAElement)hRas);
    }

    return RV_OK;
}


/************************************************************************
 * cmiRASSetEPTrEventHandler
 * purpose: Set the callback function to use for new RAS requests that
 *          are not related to specific calls.
 * input  : hApp            - Application's handle for a stack instance
 *          cmEvRASRequest  - The callback to use
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int cmiRASSetEPTrEventHandler(
    IN  HAPP                hApp,
    IN  cmiEvRASEPRequestT  cmEvRASEPRequest)
{
    rasModule* ras;

    if (hApp == NULL) return RV_ERROR_UNKNOWN;

    ras = (rasModule *)cmiGetRasHandle(hApp);
    ras->evEpRequest = cmEvRASEPRequest;

    return 0;
}


/************************************************************************
 * cmiRASSetCallTrEventHandler
 * purpose: Set the callback function to use for new RAS requests that
 *          are related to specific calls.
 * input  : hApp            - Application's handle for a stack instance
 *          cmEvRASRequest  - The callback to use
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int cmiRASSetCallTrEventHandler(
    IN  HAPP                    hApp,
    IN  cmiEvRASCallRequestT    cmEvRASCallRequest)
{
    rasModule* ras;

    if (hApp == NULL) return RV_ERROR_UNKNOWN;

    ras = (rasModule *)cmiGetRasHandle(hApp);
    ras->evCallRequest = cmEvRASCallRequest;

    return 0;
}


/************************************************************************
 * cmiRASGetProperty
 * purpose: Gets the pvt node of the RAS property DB
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : pvt node of the RAS property DB
 *          or negative value on error
 ************************************************************************/
RvPvtNodeId cmiRASGetProperty(
    IN  HRAS         hsRas)
{
    if (hsRas == NULL) return RV_ERROR_UNKNOWN;

    switch (emaGetType((EMAElement)hsRas))
    {
        case RAS_OUT_TX:
        {
            rasOutTx* outTx = (rasOutTx *)hsRas;
            return outTx->txProperty;
        }
        case RAS_IN_TX:
        {
            rasInTx* inTx = (rasInTx *)hsRas;
            return inTx->txProperty;
        }
        default:
            break;
    }
    return RV_ERROR_UNKNOWN;
}


/************************************************************************
 * cmiRASSetNewCallEventHandler
 * purpose: Set the callback function to use when RAS detects new call
 *
 * input  : hApp            - Application's handle for a stack instance
 *          cmiEvRASNewCall - The callback for new call
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int cmiRASSetNewCallEventHandler(
    IN  HAPP                hApp,
    IN  cmiEvRASNewCallT    cmiEvRASNewCall)
{
    rasModule* ras;

    if (hApp == NULL) return RV_ERROR_UNKNOWN;

    ras = (rasModule *)cmiGetRasHandle(hApp);

    ras->evRASNewCall = cmiEvRASNewCall;

    return 0;
}


/************************************************************************
 * cmiRASSetMessageEventHandler
 * purpose: Sets messages events for the use of the security module.
 *          These events allows the application later on to check on
 *          specific messages if they passed the security or not.
 * input  : hApp                    - Application's handle for a stack instance
 *          cmEviRASNewRawMessage   - Indication of a new incoming buffer for RAS
 *          cmEviRASSendRawMessage  - Indication of an outgoing RAS message
 *          cmEviRASReleaseMessageContext   - Indication that the RAS module
 *                                            is through with a specific message
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmiRASSetMessageEventHandler(
    IN HAPP                             hApp,
    IN cmiEvRASNewRawMessageT            cmiEvRASNewRawMessage,
    IN cmiEvRASSendRawMessageT           cmiEvRASSendRawMessage,
    IN cmiEvRASReleaseMessageContextT    cmiEvRASReleaseMessageContext)
{
    rasModule* ras = (rasModule *)cmiGetRasHandle(hApp);

    if (hApp == NULL) return RV_ERROR_UNKNOWN;

    ras->cmiEvRASNewRawMessage=cmiEvRASNewRawMessage;
    ras->cmiEvRASSendRawMessage=cmiEvRASSendRawMessage;
    ras->cmiEvRASReleaseMessageContext=cmiEvRASReleaseMessageContext;
    return 0;
}


/************************************************************************
 * cmiRASGetMessageContext
 * purpose: Returns the message context for a RAS transaction
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : hMsgContext - Message context for the incoming request/response
 *                        message of the given transaction.
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmiRASGetMessageContext(
    IN  HRAS             hsRas,
    OUT void**           hMsgContext)
{
    int         status = 0;
#if (RV_LOGMASK & RV_LOGLEVEL_ERROR)
    rasModule*  ras;

    ras = (rasModule *)emaGetUserData((EMAElement)hsRas);
#endif /* (RV_LOGMASK & RV_LOGLEVEL_ERROR) */

    /* Check if it's an incoming or an outgong transaction */
    switch (emaGetType((EMAElement)hsRas))
    {
        case RAS_OUT_TX:
        {
            rasOutTx* tx;
            tx = rasGetOutgoing(hsRas);
            if (tx != NULL)
            {
                if (hMsgContext)
                    *hMsgContext = tx->hMsgContext;
            }
            else
            {
                RvLogError(&ras->log,
                    (&ras->log, "cmiRASGetMessageContext: Bad outgoing transaction handle (%p)", hsRas));
                status = RV_ERROR_UNKNOWN;
            }
            break;
        }
        case RAS_IN_TX:
        {
            rasInTx* tx;
            tx = rasGetIncoming(hsRas);
            if (tx != NULL)
            {
                if (hMsgContext)
                    *hMsgContext = tx->hMsgContext;
            }
            else
            {
                RvLogError(&ras->log,
                    (&ras->log, "cmiRASGetMessageContext: Bad incoming transaction handle (%p)", hsRas));
                status = RV_ERROR_UNKNOWN;
            }
            break;
        }
        default:
            status = RV_ERROR_UNKNOWN;
    }

    return status;
}





#ifdef __cplusplus
}
#endif
