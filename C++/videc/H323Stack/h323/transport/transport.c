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


/*************************************************************************************
 * Transport module
 * ----------------
 *
 * This is the module that interacts with the network on one hand, while on the other
 * it communicates with the other protocol modules, such as Q.931, H.245, H.450 etc.
 *
 * The lower level can handle TPKT, Annex E types of communications in a transparent
 * way to the upper layers.
 *
 * The upper level exports and imports (by means of APIs and callbacks) messages to the
 * different modules: Mainly Q.931, H.245 (including tunneled messages) and the
 * rest of the tunneled protocols (Annex M, Annex L).
 *
 **************************************************************************************/
#include "rvhost.h"
#include "rvinternal.h"
#include "transport.h"
#include "transportint.h"
#include "transnet.h"
#include "transStates.h"
#include "transutil.h"
#include "transparent.h"
#include "cmutils.h"
#include "ema.h"
#include "hash.h"
#include "tpkt.h"
#include "annexe.h"

#include "q931asn1.h"
#include "h245.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus*/

#define justLocal           1
#define justRemote          2
TRANSERR setLocalAddressForQ931(cmTransSession *session, cmTransportAddress *localAddress);


/*---------------------------------------------------------------------------------------
 * transWatchdogResourceCallback :
 *
 * this function sets the resources of transport module  at the watchdog internal table
 *
 * Input:  context - the watchdog handle of the ss instance.
 *         resource   - the requested resource's enumeration
 *         type       - the type of info requested;
 *
 * Output: value - The actual resource value
 * Return: non negative value -  if success.
 *         negative value     -  if failed
 *
 *---------------------------------------------------------------------------------------*/
static int RVCALLCONV transWatchdogResourceCallback(
    IN  void*                       context,
    IN  RvUint32                    resource,
    IN  RvH323WatchdogResourceType  type,
    OUT RvUint32*                   value)
{
    cmTransGlobals* app=(cmTransGlobals*)context;
    int res = RV_ERROR_UNKNOWN;

    if (resource == app->timersResourceVal)
    {
        RvRaStatistics statistics;
        if (RvH323TimerStatistics(app->hTimers, &statistics) >= 0)
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
    }
    else
         res = RV_ERROR_UNKNOWN;

    return res;
}


/******************************************************************************
 * transAddListeningAddress
 * ----------------------------------------------------------------------------
 * General: Add a listening address to the list of supported TPKT listening
 *          addresses.
 *
 * Return Value: cmTransOK on success.
 *               cmTransIgnoreMessage on success, when we are not going to use
 *                                    the given address.
 *               cmTransErr on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  transGlobals     - The global variables of the module.
 *         pvtAddress       - PVT nodeId of a TransportAddress to use.
 * Output: None.
 *****************************************************************************/
static TRANSERR transAddListeningAddress(
    IN      cmTransGlobals   *transGlobals,
    IN      RvPvtNodeId      pvtAddress)
{
    cmTransHost         *newHost;
    cmTransportAddress  listeningAddress;
    TRANSERR            reply = cmTransOK;

    if (transGlobals->numTPKTListen == transGlobals->maxListeningAddresses)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "Too many listening addresses - cannot add another one"));
        return cmTransErr;
    }

    /* Convert the PVT node into a struct */
    if (cmVtToLTA(transGlobals->hPvt, pvtAddress, &listeningAddress) < 0)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "Error in transport address (pvtNode=%d)", pvtAddress));
        return cmTransErr;
    }

    /* Allocate a new host object for this listening address */
    newHost = (cmTransHost *)emaAdd(transGlobals->hEmaHosts, NULL);
    if (newHost == NULL)
        return cmTransErr;

    if (transParentNew((HAPPTRANS)transGlobals, (void*)newHost) != RV_OK)
    {
        emaDelete(newHost);
        return cmTransErr;
    }

    emaLock((EMAElement)newHost);
    memset(newHost, 0, sizeof(cmTransHost));
    newHost->annexEUsageMode = cmTransNoAnnexE; /* This is a TPKT host */

    if (!cmTaHasIp(&listeningAddress))
    {
        /* There's no IP in the address - get the local address for this purpose */
        RvUint16 port = listeningAddress.port;
        memcpy(&listeningAddress, &transGlobals->localAddress, sizeof(listeningAddress));
        listeningAddress.port = port;
    }

    /* See if there's a listening callback we need to handle */
    if (transGlobals->hostEventHandlers.cmEvTransHostListen != NULL)
    {
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "cmEvTransHostListen(hsHost = %d(%p), haHost=NULL, type=cmTransQ931Conn))",
                emaGetIndex((EMAElement)newHost), newHost));
        reply = transGlobals->hostEventHandlers.cmEvTransHostListen(
            (HSTRANSHOST)newHost, NULL, cmTransQ931Conn, &listeningAddress);
        newHost->reported = RV_TRUE;
    }

    /* if user rejected the listen */
    if (reply == cmTransIgnoreMessage)
    {
        closeHostInternal((HSTRANSHOST)newHost, RV_TRUE);

        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "transAddListeningAddress: TPKT listening refused by user"));

        /* Indicate that there's no need for this address */
        return cmTransIgnoreMessage;
    }

    /* Open the TPKT connection */
    newHost->hTpkt = tpktOpen(transGlobals->tpktCntrl, &listeningAddress, RvH323ConnectionQ931,
        tpktMultiServer, (EMAElement)newHost, transQ931AcceptHandler, (void *)newHost);

    /* if listening started OK, get the allocated address for listen */
    if (newHost->hTpkt != NULL)
        getGoodAddress(transGlobals, newHost->hTpkt, NULL, cmTransQ931Conn, &listeningAddress);

    /* report that the listen started (or failed) */
    if (transGlobals->hostEventHandlers.cmEvTransHostListening != NULL)
    {
        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "cmEvTransHostListening(hsHost = %d(%p), haHost=NULL, type=cmTransQ931Conn, error=%d)",
                emaGetIndex((EMAElement)newHost), newHost, (newHost->hTpkt == NULL)));

        transGlobals->hostEventHandlers.cmEvTransHostListening(
            (HSTRANSHOST)newHost, NULL, cmTransQ931Conn, &listeningAddress, (newHost->hTpkt == NULL));
        newHost->reported = RV_TRUE;
    }

    /* if we had an error starting the listening, call the whole thing off */
    if (newHost->hTpkt == NULL)
    {
        closeHostInternal((HSTRANSHOST)newHost, RV_TRUE);

        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "transAddListeningAddress failed to initiate listen on TPKT"));
        emaUnlock((EMAElement)newHost);
        return cmTransErr;
    }

    /* We're ok... */
    transGlobals->hTPKTListen[transGlobals->numTPKTListen] = newHost;
    transGlobals->numTPKTListen++;

    /* Get the actual address we're listening on */
    tpktGetAddresses(newHost->hTpkt, &listeningAddress, NULL);
    if (!cmTaHasIp(&listeningAddress))
    {
        RvAddress localAddr;
        RvUint16 port = listeningAddress.port;
        RvUint32 numAddrs = 1;
        RvHostLocalGetAddress(transGlobals->logMgr, &numAddrs, &localAddr);
        RvH323CoreToCmAddress(&localAddr, &listeningAddress);
        listeningAddress.port = port;
    }
    cmLTAToVt(transGlobals->hPvt, pvtAddress, &listeningAddress);

	//{{gaoshizhong 20141105 Add IP 127.0.0.1 rule out
	{
		char ipstring[128]={0};
		cmTransportAddress2String(&listeningAddress,127,ipstring);
		if(ipstring[0]=='\0' || strstr(ipstring,"127.0.0.1"))
		{
			emaUnlock((EMAElement)newHost);
			return cmTransErr;
		}
	}
	//}}gaoshizhong 20141105 Add IP 127.0.0.1 rule out

    emaUnlock((EMAElement)newHost);
    return cmTransOK;
}




/******************************************************************************
 * cmTransInit
 * ----------------------------------------------------------------------------
 * General: Initialize the entire transport module
 *
 * Return Value: A handle to the entire transport module or NULL on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hAppATrans              - The application handle to the entire transport module.
 *         hPvt                    - The handle to the stack's PVT.
 *         selEngine               - select engine to use.
 *         numOfSessions           - The maximal amount of session elements to allocate.
 *         extraConnections        - Number for extra connections to allocate.
 *         poolSizeInKB            - maximum amount of storage for messages before send.
 *         poolElemSize            - maximum non-fragmented storage for messages.
 *         maxMessageSize          - Maximal size of encoded message.
 *         annexESupported         - Do we support annex E.
 *         numOfListeningAddresses - Number of listening addresses of each type.
 *         portRange               - Port range to use.
 *         logMgr                  - Log manager to use.
 * Output: None.
 *****************************************************************************/
HAPPTRANS cmTransInit(IN HAPPATRANS      hAppATrans,
                      IN HPVT            hPvt,
                      IN RvSelectEngine* selEngine,
                      IN int             numOfSessions,
                      IN int             extraConnections,
                      IN int             poolSizeInKB,
                      IN int             poolElemSize,
                      IN int             maxMessageSize,
                      IN RvBool          annexESupported,
                      IN RvInt           numOfListeningAddresses,
                      IN RvPortRange*    portRange,
                      IN RvLogMgr*       logMgr)
{
    annexEStatus eStat;
    int status;
    cmTransGlobals * transGlobals;
    int numOfHostConnections;

    /* Allocate global element of the module */
    if(RvMemoryAlloc(NULL, sizeof(cmTransGlobals), logMgr, (void**)&transGlobals) != RV_OK)
        return NULL;
    memset(transGlobals, 0, sizeof(cmTransGlobals));
    transGlobals->logMgr = logMgr;

    /* set the application handle to the module's instance */
    transGlobals->hAppATrans = hAppATrans;

    /* Allocate the global locks */
    RvMutexConstruct(logMgr, &transGlobals->lockSessionsList);
    RvLockConstruct(logMgr, &transGlobals->lockHash);
    RvLockConstruct(logMgr, &transGlobals->lockGarbage);

    /* Start the log */
    RvLogSourceConstruct(logMgr, &transGlobals->hLog, "Transport", "Transport Layer");
    RvLogSourceConstruct(logMgr, &transGlobals->hTPKTCHAN, "TPKTCHAN", "Transport message print");
    RvLogSourceConstruct(logMgr, &transGlobals->hTPKTWIRE, "TPKTWIRE", "Transport message print");

    /* Save the parameters we will need */
    transGlobals->hPvt = hPvt;
    transGlobals->maxListeningAddresses = numOfListeningAddresses;

    RvLogInfo(&transGlobals->hLog,
        (&transGlobals->hLog, "Transport layer initializing with: hAppATrans=%p, hPvt=%p, numOfSessions=%d, extraConnections=%d, poolSizeInKB=%d, maxMessageSize=%d",
              hAppATrans, hPvt, numOfSessions, extraConnections, poolSizeInKB, maxMessageSize));

    /* Allocate listening host pointers */
    if (RvMemoryAlloc(NULL, sizeof(cmTransHost*)*numOfListeningAddresses, logMgr, (void**)&transGlobals->hTPKTListen) != RV_OK)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "Transport layer can't allocate listening addresses."));
        cmTransEnd((HAPPTRANS)transGlobals);
        return NULL;
    }

    /* allocate the parent locks */
    /* TODO: find a better estimate of the amount of extra locks needed */
    transParentInit(transGlobals, numOfSessions + numOfListeningAddresses + extraConnections + 5);

    /* Allocate reservoir of sessions */
    transGlobals->hEmaSessions =  emaConstruct( sizeof(cmTransSession),
                                                numOfSessions + 1,
                                                emaLockPointers,
                                                "TRANSPORT Sessions",
                                                0,
                                                transGlobals,
                                                hAppATrans,
                                                logMgr);
    if (transGlobals->hEmaSessions == NULL)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "Transport layer can't allocate sessions pool."));

        /* release all */
        cmTransEnd( (HAPPTRANS)transGlobals );

        return NULL;
    }
    emaSetIdleEvent(transGlobals->hEmaSessions, emaEvSessionElementIdle);

    /* Calculate the host quantity in the case user did not provide us with one*/
    if (annexESupported)
        numOfHostConnections = numOfSessions * 3 + numOfListeningAddresses + extraConnections;
    else
    {
        numOfHostConnections = numOfSessions * 2 + numOfListeningAddresses + 1 + extraConnections;
        /* The additional host in the end of the above calculation is added for
           better support of garbage collection of hosts when maxCalls is very
           low and we make calls to a single destination all the time. */
    }

    /* Allocate reservoir of hosts    */
    transGlobals->hEmaHosts = emaConstruct(sizeof(cmTransHost),
                                            numOfHostConnections,
                                            emaLockPointers,
                                            "TRANSPORT Hosts",
                                            0,
                                            transGlobals,
                                            hAppATrans,
                                            logMgr);
    if (transGlobals->hEmaHosts == NULL)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "Transport layer can't allocate hosts pool."));

        /* release all */
        cmTransEnd( (HAPPTRANS)transGlobals );

        return NULL;
    }
    emaSetIdleEvent(transGlobals->hEmaHosts, emaEvHostElementIdle);

    status = emaAddWatchdogResource(transGlobals->hEmaSessions, cmiGetWatchdogHandle((HAPP)hAppATrans), "TransportSessions", "Transport", "Session elements");
    if (status < 0) return NULL;

    status = emaAddWatchdogResource(transGlobals->hEmaHosts, cmiGetWatchdogHandle((HAPP)hAppATrans), "TransportHosts", "Transport", "Host elements");
    if (status < 0) return NULL;


    /* build the hash table for the host by remoteAddress + type */
    transGlobals->hHashHosts = hashConstruct(   numOfHostConnections*2 + 1,
                                                numOfHostConnections,
                                                NULL,
                                                NULL,
                                                sizeof_hostKey,
                                                sizeof(HSTRANSHOST),
                                                "Transport Hosts hash",
                                                logMgr);

    if (transGlobals->hHashHosts == NULL)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "Transport layer can't build hosts hash table."));

        /* release all */
        cmTransEnd( (HAPPTRANS)transGlobals );

        return NULL;
    }


    /* build the hash table for the sessions by CRV + host */
    transGlobals->hHashSessions = hashConstruct(numOfSessions*2 + 1,
                                                numOfSessions,
                                                NULL,
                                                NULL,
                                                sizeof_sessionKey,
                                                sizeof(HSTRANSSESSION),
                                                "Transport Sessions hash",
                                                logMgr);

    if (transGlobals->hHashSessions == NULL)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "Transport layer can't build sessions hash table."));

        /* release all */
        cmTransEnd( (HAPPTRANS)transGlobals );

        return NULL;
    }

    /* build the rpool reservoir for outgoing messages */
    transGlobals->curUsedNumOfMessagesInRpool = 0;
    transGlobals->maxUsedNumOfMessagesInRpool = 0;

    transGlobals->messagesRPool = rpoolConstruct( poolElemSize,
                                                  (RvMax(poolSizeInKB, 3) * 1024)/poolElemSize,
                                                  RV_TRUE,
                                                  "outgoing/tunneled messages",
                                                  logMgr);
    status = rpoolAddWatchdogResource(transGlobals->messagesRPool, cmiGetWatchdogHandle((HAPP)hAppATrans),
        "MessagesPool", "Transport", "Transport messages pool");

    if ((transGlobals->messagesRPool == NULL) || (status < 0))
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "Transport layer can't build rPool of messages."));

        /* release all */
        cmTransEnd( (HAPPTRANS)transGlobals );

        return NULL;
    }

    /* allocate the encoding buffer (the extra bytes are for TPKT header and pointer
       to next message */
    {
        RvUint8 *buffer;

        transGlobals->codingBufferSize = maxMessageSize+MSG_HEADER_SIZE+TPKT_HEADER_SIZE;
        getEncodeDecodeBuffer(transGlobals->codingBufferSize, &buffer);

        if (buffer == NULL)
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "Transport layer can't allocate coding buffer."));

            /* release all */
            cmTransEnd( (HAPPTRANS)transGlobals );

            return NULL;
        }
    }


    /* create timers pool (one for annex E and one for each zero-timer of a session) */
    transGlobals->hTimers = RvH323TimerConstruct(numOfSessions * 2 + 1, logMgr, NULL);

    /* start the network TPKT module */

    /* for each session there is one Q.931 connecting socket and two H.245 sockets,
       one for listening and one for connecting. The extra TPKT element is for
       the Q.931 listening socket.
       Note: We may statistically allocate less TPKT elements since simultaneous
             listen and connect in H.245 is rare */
    transGlobals->tpktCntrl = tpktInit((HAPP)hAppATrans, selEngine,
        (numOfSessions * 3 + numOfListeningAddresses + extraConnections), portRange, transGlobals, logMgr);
    if (transGlobals->tpktCntrl == NULL)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "Transport layer can't open TPKT package."));

        /* release all */
        cmTransEnd( (HAPPTRANS)transGlobals );

        return NULL;
    }

    status = RvWatchdogAddResource(cmiGetWatchdogHandle((HAPP)hAppATrans), "TransportTimers", "Transport", "Host elements",
        (RvWatchdogResourceCallback_T)transWatchdogResourceCallback, NULL, NULL,
        (void*)transGlobals, &transGlobals->timersResourceVal);
    if ((transGlobals->hTimers == NULL) || (status < 0))
    {
         RvLogError(&transGlobals->hLog,
             (&transGlobals->hLog, "Transport layer can't allocate timers."));

        /* release all */
        cmTransEnd( (HAPPTRANS)transGlobals );

        return NULL;
    }

    /* start the network annex E module */
    if (annexESupported)
    {
        transGlobals->annexESupported = RV_TRUE;

        eStat = annexEInit(&(transGlobals->annexECntrl),
                          (HAPPANNEXE)transGlobals,
                          numOfSessions + 1, /*  - allocate one more, just to reject incoming calls */
                          maxMessageSize,
                          numOfSessions,
                          numOfListeningAddresses,
                          transGlobals->hTimers,
                          selEngine,
                          portRange,
                          logMgr);

        if (eStat != annexEStatusNormal)
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "Transport layer can't open Annex E package."));

            /* release all */
            cmTransEnd( (HAPPTRANS)transGlobals );

            return NULL;
       }
       else
       {
         annexEEvents annexEEventHandlers = {   NULL /*annexEEvRestart*/,
                                                annexEEvNewMessage,
                                                annexEEvConnectionBroken,
                                                annexEEvUseOtherAddress,
                                                NULL /*annexEEvNotSupported*/,
                                                annexEEvWriteable
                                            };

         eStat = annexESetEventHandler(transGlobals->annexECntrl,
                                       &annexEEventHandlers);
         if (eStat != annexEStatusNormal)
         {
             RvLogError(&transGlobals->hLog,
                 (&transGlobals->hLog, "Transport layer can't set Annex E callbacks."));
         }
       }
    }
    else
        transGlobals->annexESupported = RV_FALSE;

    /* create syntax trees for decoding routines */
    transGlobals->synProtQ931 = pstConstruct(q931asn1GetSyntax(), "Q931Message");
    transGlobals->synProtH245 = pstConstruct(h245GetSyntax(), "MultimediaSystemControlMessage");
    transGlobals->synProtH450 = pstConstruct(q931asn1GetSyntax(),
                   "Q931Message.message.setup.userUser.h323-UserInformation.h323-uu-pdu.h4501SupplementaryService");

    /* return the handle to the module global element */
    RvLogInfo(&transGlobals->hLog,
             (&transGlobals->hLog, "Transport layer initialized. hAppTrans=%p",transGlobals));
    return (HAPPTRANS)transGlobals;
}


/******************************************************************************
 * cmTransStart
 * ----------------------------------------------------------------------------
 * General: To start the listening process on TPKT and/or Annex E.
 *
 * Return Value: cmTransOK on success.
 *               cmTransErr on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  happTrans        - The handle to the instance of the transport module.
 *         tpktAddresses    - PVT node holding Q.931 TPKT listening addresses.
 *                            Of type "SEQUENCE OF TransportAddress".
 *         annexEAddresses  - List of annex E listening addresses.
 *                            Of type "SEQUENCE OF TransportAddress".
 *         localAddress     - The local address as was received from the configuration.
 *         annexEParams     - Annex E related parameters.
 * Output: None.
 *****************************************************************************/
TRANSERR cmTransStart(IN HAPPTRANS          happTrans,
                      IN RvPvtNodeId        tpktAddresses,
                      IN RvPvtNodeId        annexEAddresses,
                      IN cmTransportAddress *localAddress,
                      IN CMTRANSANNEXEPARAM *annexEParams)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)happTrans;
    RvPvtNodeId     curAddress;
    RvPvtNodeId     lastAddress;
    TRANSERR        status = cmTransOK;

    if (!transGlobals)
        return cmTransErr;

    /* lock the module */
    RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);

    /* save the local IP into the global area */
    memcpy(&transGlobals->localAddress, localAddress, sizeof(transGlobals->localAddress));

    if (RV_PVT_NODEID_IS_VALID(tpktAddresses))
    {
        /* Start listening for TPKT addresses */
        curAddress = pvtChild(transGlobals->hPvt, tpktAddresses);

        while ((RV_PVT_NODEID_IS_VALID(curAddress)) && (status == cmTransOK))
        {
            /* We have a TransportAddress PVT node - process it */
            status = transAddListeningAddress(transGlobals, curAddress);

            /* Get the next one in the list */
            lastAddress = curAddress;
            curAddress = pvtBrother(transGlobals->hPvt, curAddress);

            if (status == cmTransIgnoreMessage)
            {
                pvtDelete(transGlobals->hPvt, lastAddress);
                status = cmTransOK;
            }
        }
    }

    /* set the annex E parameters */
    if ((transGlobals->annexESupported == RV_TRUE) && (RV_PVT_NODEID_IS_VALID(annexEAddresses)) && (status == cmTransOK))
    {
        cmTransportAddress  tmpAddr;
        annexEStatus        eStat;
        RvInt               i;

        eStat = annexESetProtocolParams(transGlobals->annexECntrl,
                                        (int)annexEParams->t_R1,
                                        (int)annexEParams->t_R2,
                                        (int)annexEParams->n_R1,
                                        (int)annexEParams->t_IMA1,
                                        (int)annexEParams->n_IMA1,
                                        (int)annexEParams->t_DT);
        if (eStat != annexEStatusNormal)
        {
            RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog,
                     "cmTransStart failed to set annex E protocol params eStat=%d", eStat));
            return cmTransErr;
        }

        /* Start listening for Annex E addresses */
        curAddress = pvtChild(transGlobals->hPvt, annexEAddresses);

        while ((RV_PVT_NODEID_IS_VALID(curAddress)) && (eStat == annexEStatusNormal))
        {
            /* We have a TransportAddress PVT node - process it */
            cmVtToTA(transGlobals->hPvt, curAddress, &tmpAddr);
            if (!cmTaHasIp(&tmpAddr))
            {
                /* There's no IP in the address - get the local address for this purpose */
                RvUint16 port = tmpAddr.port;
                memcpy(&tmpAddr, &transGlobals->localAddress, sizeof(tmpAddr));
                tmpAddr.port = port;
            }

            eStat = annexEAddLocalAddress(transGlobals->annexECntrl, &tmpAddr);
            if (eStat == annexEStatusNormal)
                cmTAToVt(transGlobals->hPvt, curAddress, &tmpAddr);

            /* Get the next one in the list */
            curAddress = pvtBrother(transGlobals->hPvt, curAddress);
        }

        if (eStat != annexEStatusNormal)
        {
            RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog,
                     "cmTransStart failed to set Local address for annex E eStat=%d", eStat));
            return cmTransErr;
        }

        eStat = annexEStart(transGlobals->annexECntrl);
        if (eStat != annexEStatusNormal)
        {
            RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "cmTransStart failed annexEStart eStat=%d", eStat));
            return cmTransErr;
        }

        /* Update list if listening addresses of Annex E */
        i = 0;
        curAddress = pvtChild(transGlobals->hPvt, annexEAddresses);
        while (RV_PVT_NODEID_IS_VALID(curAddress))
        {
            annexEGetLocalAddress(transGlobals->annexECntrl, i, &tmpAddr);

            /* We use an H.245 connection so the IP address will be changed if it is indicated as "any" */
            getGoodAddress(transGlobals, NULL, NULL, cmTransH245Conn, &tmpAddr);

            cmTAToVt(transGlobals->hPvt, curAddress, &tmpAddr);
            curAddress = pvtBrother(transGlobals->hPvt, curAddress);
            i++;
        }
    }

    RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
    return status;
}

/**************************************************************************************
 * cmTransStop
 *
 * Purpose: Stops all the listening processes that were started by cmTransStart.
 *
 * Input:   hAppTrans - A handle to the entire transport module
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR cmTransStop(IN HAPPTRANS hAppTrans)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)hAppTrans;

    if (!transGlobals)
        return cmTransErr;

    /* close all garbage-collected multiplexed hosts with no calls on them */
    RvLockGet(&transGlobals->lockGarbage, transGlobals->logMgr);
    while (transGlobals->oldestEmptyQ931MuxHost != NULL)
    {
        cmTransHost *host = transGlobals->oldestEmptyQ931MuxHost;
        RvLockRelease(&transGlobals->lockGarbage, transGlobals->logMgr);
        closeHostInternal((HSTRANSHOST)host, RV_TRUE);
        RvLockGet(&transGlobals->lockGarbage, transGlobals->logMgr);
    }
    RvLockRelease(&transGlobals->lockGarbage, transGlobals->logMgr);

    /* lock the module */
    RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);

    /* close the listening process of TPKT for Q.931 connections */
    while (transGlobals->numTPKTListen > 0)
    {
        transGlobals->numTPKTListen--;
        closeHostInternal((HSTRANSHOST)transGlobals->hTPKTListen[transGlobals->numTPKTListen], RV_TRUE);
        transGlobals->hTPKTListen[transGlobals->numTPKTListen] = NULL;
    }

    /* stop the annex E */
    if (transGlobals->annexESupported)
        annexEStop(transGlobals->annexECntrl);

    RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);

    return cmTransOK;
}

/**************************************************************************************
 * cmTransEnd
 *
 * Purpose: Shuts down the module and deallocates all its components.
 *
 * Input:   hAppTrans - A handle to the entire transport module
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR cmTransEnd(IN HAPPTRANS hAppTrans)
{

    cmTransGlobals *transGlobals = (cmTransGlobals *)hAppTrans;

    if (hAppTrans)
    {
        /* lock the module */
        RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);

        /* close the annex E package */
        if (transGlobals->annexECntrl)
            annexEEnd(transGlobals->annexECntrl);

        /* close the TPKT package */
        if (transGlobals->tpktCntrl)
            tpktEnd(transGlobals->tpktCntrl);

        /* release the messages rPool */
        if (transGlobals->messagesRPool)
            rpoolDestruct(transGlobals->messagesRPool);

        /* release the sessions hash table */
        if (transGlobals->hHashSessions)
            hashDestruct(transGlobals->hHashSessions);

        /* release the hosts hash table */
        if (transGlobals->hHashHosts)
            hashDestruct(transGlobals->hHashHosts);

        /* release the reservoir of hosts */
        if (transGlobals->hEmaHosts)
            emaDestruct(transGlobals->hEmaHosts);

        /* release the reservoir of sessions */
        if (transGlobals->hEmaSessions)
            emaDestruct(transGlobals->hEmaSessions);

        /* release the pointers to the listening sockets */
        if (transGlobals->hTPKTListen != NULL)
            RvMemoryFree(transGlobals->hTPKTListen, transGlobals->logMgr);

        /* destroy timers pool */
        if (transGlobals->hTimers)
            RvH323TimerDestruct(transGlobals->hTimers);

        /* destroy the parent locks */
        transParentEnd(transGlobals);

        /* destruct the syntax trees */
        pstDestruct(transGlobals->synProtH450);
        pstDestruct(transGlobals->synProtH245);
        pstDestruct(transGlobals->synProtQ931);

        /* unlock the module */
        RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);

        /* release the global locks */
        RvLockDestruct(&transGlobals->lockGarbage, transGlobals->logMgr);
        RvLockDestruct(&transGlobals->lockHash, transGlobals->logMgr);
        RvMutexDestruct(&transGlobals->lockSessionsList, transGlobals->logMgr);

        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog, "Transport layer was shutdown"));

        RvLogSourceDestruct(&transGlobals->hLog);
        RvLogSourceDestruct(&transGlobals->hTPKTCHAN);
        RvLogSourceDestruct(&transGlobals->hTPKTWIRE);

        /* deallocate the global element of the module */
        RvMemoryFree((void *)hAppTrans, transGlobals->logMgr);
    }

    return cmTransOK;
}

/**************************************************************************************
 * cmTransCreateSession
 *
 * Purpose: Creates a new session element according to its parameters.
 *
 * Input:   hAppTrans       - A handle to the entire transport module
 *          haTranSession   - An application handle to be set to the session, usually
 *                            would be the call handle that is associated with this session.
 *          hsTransHost     - handle of the host related to this session
 *
 * Output:  hsTranSession   - An handle to the created session element
 *
 **************************************************************************************/
TRANSERR cmTransCreateSession(  IN  HAPPTRANS       hAppTrans,
                                IN  HATRANSSESSION  haTransSession,
                                IN  HSTRANSHOST     hsTransHost,
                                OUT HSTRANSSESSION  *hsTransSession)
{
    cmTransGlobals  *transGlobals = (cmTransGlobals *)hAppTrans;
    EMAElement      newSession;

    emaStatistics stat;

    /* if the module was initialized */
    if (hAppTrans)
    {
        /* allocate a new session element */
        newSession = emaAdd(transGlobals->hEmaSessions, (void*)haTransSession);

        /* decide which lock to use */
        if (hsTransHost != NULL)
            transParentShare(hAppTrans, (void*)newSession, (void*)hsTransHost);
        else if (haTransSession != NULL)
            transParentShare(hAppTrans, (void*)newSession, (void*)haTransSession);

        emaGetStatistics(transGlobals->hEmaSessions, &stat);

        /* set the output parameter to the allocated element (maybe NULL) */
        if (hsTransSession)
            *hsTransSession = (HSTRANSSESSION)newSession;

        /* if an element was indeed allocated return OK */
        if (newSession)
        {
            cmTransSession *session = (cmTransSession *)newSession;
            memset(session, 0, sizeof(cmTransSession));
            session->h245Stage              = cmTransH245Stage_connect;
            session->tunnelingState         = tunnNo;
            session->faststartState         = fsNo;
            session->h450Element            = RV_PVT_INVALID_NODEID;
            session->h450AlertElement       = RV_PVT_INVALID_NODEID;
            session->annexLElement          = RV_PVT_INVALID_NODEID;
            session->annexMElement          = RV_PVT_INVALID_NODEID;
            session->annexEListeningIndex   = RV_PVT_INVALID_NODEID;
            session->annexEUsageMode        = cmTransNoAnnexE;
            m_sessionset(session, outgoing, RV_TRUE);
            m_sessionset(session, closeOnNoSessions, RV_TRUE);

            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "cmTransCreateSession = %d(%p)", emaGetIndex((EMAElement)session), session));

            return cmTransOK;
        }
    }

    /* in cases that OK was NOT sent, return error indication */
    RvLogError(&transGlobals->hLog,
        (&transGlobals->hLog,
            "cmTransCreateSession [FAILURE](haSession=%p)", haTransSession));

    return cmTransErr;
}

/**************************************************************************************
 * cmTransCloseSession
 *
 * Purpose: Deletes a session element.
 *
 * Input:   hsTranSession   - An handle to the session element
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR cmTransCloseSession(IN HSTRANSSESSION hsTransSession)
{
    TRANSERR res = cmTransErr;
    /* check if an element exists */
    if (hsTransSession)
    {
        /* lock the whole module for this major change */
        if (emaMarkLocked((EMAElement)hsTransSession))
        {
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
            /* retrieve the transport module global area */
            cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransSession);

            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "cmTransCloseSession = %d(%p)", emaGetIndex((EMAElement)hsTransSession), hsTransSession));
#endif

            transSessionClose((cmTransSession *)hsTransSession);

            /* delete the host element */
            if (!emaWasDeleted((EMAElement)hsTransSession))
            {
                res = ((emaDelete( (EMAElement)hsTransSession )!= 0)?
                                cmTransErr:cmTransOK);
            }
            else
                res = cmTransOK;

            emaReleaseLocked((EMAElement)hsTransSession);
        }
    }
    return res;
}


/**************************************************************************************
 * cmTransDrop
 *
 * Purpose: Drop a session (put its state in Idle).
 *
 * Input:   hsTranSession   - An handle to the session element
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR cmTransDrop(IN HSTRANSSESSION hsTransSession)
{
    cmTransSession *session=(cmTransSession *)hsTransSession;

    if (emaMarkLocked((EMAElement)hsTransSession))
    {
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
        cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog,
            "cmTransDrop = %d(%p),q931=%p,h245=%p", emaGetIndex((EMAElement)hsTransSession), hsTransSession,
            session->Q931Connection, session->H245Connection));
#endif
        if (session->Q931Connection != NULL)
        {
            cmTransHost* host = session->Q931Connection;

            if (emaMarkLocked((EMAElement)host))
            {
                /* see if this host is handling other sessions as well */
                if ((host->firstSession == NULL) ||
                    ((host->firstSession == session) && (host->firstSession->nextSession == NULL)))
                {
                    /* host handles no other sessions than the current one (or none at all) */
                    if (host->closeOnNoSessions || host->remoteCloseOnNoSessions)
                        host->state = hostIdle;
                }
                emaReleaseLocked((EMAElement)host);
            }
        }
        if ((session->H245Connection != NULL) && (session->H245Connection->h245Listen != NULL))
        {
            /* if we're listening on H.245, we should stop doing that at this point in time */
            if (!tpktIsConnected(session->H245Connection->h245Listen))
            {
                tpktClose(session->H245Connection->h245Listen);
                session->H245Connection->h245Listen = NULL;
            }
        }
        emaReleaseLocked((EMAElement)hsTransSession);
    }
    return cmTransOK;
}


/**************************************************************************************
 * cmTransCreateHost
 *
 * Purpose: Creates a new host element according to its parameters.
 *
 * Input:   hAppTrans   - A handle to the entire transport module
 *          haTranHost  - An application handle to be set to the host element.
 *          relatedSession - handle to a related session, may be NULL.
 *
 * Output:  hsTranHost  - An handle to the created host element
 *
 **************************************************************************************/
TRANSERR cmTransCreateHost( IN  HAPPTRANS       hAppTrans,
                            IN  HSTRANSSESSION  relatedSession,
                            IN  HATRANSHOST     haTransHost,
                            OUT HSTRANSHOST    *hsTransHost)
{
    cmTransGlobals  *transGlobals = (cmTransGlobals *)hAppTrans;
    EMAElement      newHost;

    /* if the module was initialized */
    if (hAppTrans)
    {
        /* allocate a new host element */
        newHost = emaAdd(transGlobals->hEmaHosts, (void*)haTransHost);
        if (relatedSession == NULL)
        {
            if (transParentNew((HAPPTRANS)transGlobals, (void*)newHost) != RV_OK)
            {
                emaDelete(newHost);
                return cmTransErr;
            }
        }
        else
        {
            cmTransSession *session = (cmTransSession *)relatedSession;
            session->H245Connection = newHost;
            transParentShare((HAPPTRANS)transGlobals, (void*)newHost, (void*)relatedSession);
        }

        /* set the output parameter to the allocated element (maybe NULL) */
        if (hsTransHost)
            *hsTransHost = (HSTRANSHOST)newHost;

        /* if an element was indeed allocated return OK */
        if (newHost)
        {
            cmTransHost *host = (cmTransHost *)newHost;

            memset(host, 0, sizeof(cmTransHost));

            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "cmTransCreateHost created host=%d(%p)",emaGetIndex((EMAElement)host), host));

            garbageCollectMultiplexHost(transGlobals);

            return cmTransOK;
        }
    }

    /* in cases that OK was NOT sent, return error indication */
    return cmTransErr;
}

/**************************************************************************************
 * cmTransCloseHost
 *
 * Purpose: Deletes a host element. Will notify all its associates sessions.
 *
 * Input:   hsTranHost  - An handle to the host element
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR cmTransCloseHost(IN HSTRANSHOST hsTransHost)
{
    return closeHostInternal(hsTransHost, RV_FALSE);
}

/**************************************************************************************
 * cmTransSetSessionParam
 *
 * Purpose: Sets a specific parameter for the given session element.
 *
 * Input:   hsTranSession   - An handle to the session element
 *          param           - The name of the parameter to be set.
 *          value           - An integer or pointer to set as the new parameter.
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR cmTransSetSessionParam(IN HSTRANSSESSION       hsTransSession,
                                IN TRANSSESSIONPARAM    param,
                                IN const void           *value)
{
    cmTransSession *session = (cmTransSession *)hsTransSession;
    TRANSERR result = cmTransOK;

    if (emaMarkLocked((EMAElement)session))
    {
        switch (param)
        {
            case cmTransParam_isTunnelingSupported:
                {
                    m_sessionset(session, isTunnelingSupported, (*(RvBool*)value) != 0);
                }
                break;

            case cmTransParam_notEstablishControl:
                {
                    m_sessionset(session, notEstablishControl, (*(RvBool*)value) != 0);
                }
                break;

            case cmTransParam_H245Stage:
                {
                    session->h245Stage = *(cmH245Stage *)value;
                }
                break;

            case cmTransParam_isParallelTunnelingSupported:
                {
                    m_sessionset(session, isParallelTunnelingSupported, (*(RvBool*)value) != 0);
                }
                break;

            case cmTransParam_shutdownEmptyConnection:
                {
                    m_sessionset(session, closeOnNoSessions, (*(RvBool*)value) != 0);
                    if (session->Q931Connection != NULL)
                        cmTransSetHostParam((HSTRANSHOST)session->Q931Connection,
                                             cmTransHostParam_shutdownEmptyConnection,
                                             value,
                                             RV_FALSE);
                }
                break;

            case cmTransParam_isMultiplexed:
                {
                    m_sessionset(session, isMultiplexed, (*(RvBool*)value) != 0);
                    if (session->Q931Connection)
                        cmTransSetHostParam((HSTRANSHOST)session->Q931Connection,
                                             cmTransHostParam_isMultiplexed,
                                             value,
                                             RV_FALSE);
                }
                break;

            case cmTransParam_annexEMode:
                {
                    session->annexEUsageMode = *(cmAnnexEUsageMode *)value;
                }
                break;

            case cmTransParam_sourceIpAddress:
                {
                    result = setLocalAddressForQ931(session, (cmTransportAddress *)value);
                }
                break;

            case cmTransParam_callTerminating:
                {
                    m_sessionset(session, callTerminating, (*(RvBool*)value));
                }
                break;

            case cmTransParam_extFastConnect:
                {
                    EFCSTATE newState = *(EFCSTATE*)value;
                    EFCSTATE oldState = session->extFastConnectState;

                    session->extFastConnectState = newState;

                    if ((session->faststartState != fsNo))
                    {
                        if (((newState == efcNo) && (oldState != efcNo)) ||
                            ((newState != efcNo) && (oldState == efcNo)))
                        {
                            cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);
                            RvPvtNodeId newMessage;
                            /* must notify the other side about change in EFC state */
                            if (createMessage(transGlobals, cmQ931facility, session->CRV, session->callId, &newMessage))
                            {
                                addFeatureSet(transGlobals, session, newMessage, RV_TRUE);
                                result = cmTransSendMessage((HSTRANSSESSION)session, newMessage, cmTransQ931Type);
                                pvtDelete(transGlobals->hPvt, newMessage);
                            }
                            else result = cmTransErr;
                        }
                    }
                }
                break;

            case cmTransParam_callSupportNat:
                {
                    m_sessionset(session, callSupportNat, (*(RvBool*)value));
                }
                break;

            case cmTransParam_parallelBeforeFS:
                {
                    m_sessionset(session, parallelBeforeFS, (*(RvBool*)value));
                }
                break;

            default:
                break;
        }

        /* unlock the session */
        emaReleaseLocked((EMAElement)session);
        return result;
    }

    return cmTransErr;
}

/**************************************************************************************
 * cmTransGetSessionParam
 *
 * Purpose: Gets a specific parameter from a given session element.
 *
 * Input:   hsTranSession   - An handle to the session element
 *          param           - The name of the parameter to get.
 *
 * Output:  value           - An integer or pointer which is the value of the parameter.
 *
 **************************************************************************************/
TRANSERR cmTransGetSessionParam(IN  HSTRANSSESSION      hsTransSession,
                                IN  TRANSSESSIONPARAM   param,
                                OUT void                *value)
{
    TRANSERR status = cmTransErr;
    cmTransSession *session = (cmTransSession *)hsTransSession;

    if (emaMarkLocked((EMAElement)session))
    {
        switch (param)
        {
            case cmTransParam_host:
                {
                    cmTransHost *host;

                    if (session->annexEUsageMode != cmTransNoAnnexE)
                        host = session->annexEConnection;
                    else
                        host = session->Q931Connection;

                    /* lock the host */
                    if (host)
                    {
                        if (emaMarkLocked((EMAElement)host))
                        {
                            *(cmTransHost **)value = host;
                            status = cmTransOK;
                            emaReleaseLocked((EMAElement)host);
                        }
                    }
                }
                break;

            case cmTransParam_H245Connection:
                {
                    *(cmTransHost **)value = session->H245Connection;
                    status = cmTransOK;
                }
                break;

            case cmTransParam_isTunnelingSupported:
                {
                    if ((session->tunnelingState == tunnRejected) || m_sessionget(session, switchToSeparateWasAsked))
                    {
                        /* Tunneling was rejected - no way we're going to
                           have a tunneled call here */
                        *(RvBool *)value = RV_FALSE;
                    }
                    else
                    {
                        /* We're not sure if we're going tunneled or not, so we're just
                           assuming the user will be happy to know what his side is
                           doing. */
                        *(RvBool *)value = (RvBool)m_sessionget(session, isTunnelingSupported);
                    }
                    status = cmTransOK;
                }
                break;

            case cmTransParam_notEstablishControl:
                {
                    *(RvBool *)value = (RvBool)m_sessionget(session, notEstablishControl);
                    status = cmTransOK;
                }
                break;

            case cmTransParam_H245Stage:
                {
                    *(cmH245Stage *)value = session->h245Stage;
                    status = cmTransOK;
                }
                break;

            case cmTransParam_isParallelTunnelingSupported:
                {
                    *(RvBool *)value = (m_sessionget(session, isParallelTunnelingSupported) &&
                                       (session->parallelTunnelingState != parllNo));
                    status = cmTransOK;
                }
                break;

            case cmTransParam_shutdownEmptyConnection:
                {
                    *(RvBool *)value = (RvBool)m_sessionget(session, closeOnNoSessions);
                    status = cmTransOK;
                }
                break;

            case cmTransParam_isMultiplexed:
                {
                    *(RvBool *)value = (RvBool)m_sessionget(session, isMultiplexed);
                    status = cmTransOK;
                }
                break;

            case cmTransParam_annexEMode:
                {
                    *(cmAnnexEUsageMode *)value = session->annexEUsageMode;
                    status = cmTransOK;
                }
                break;

            case cmTransParam_sourceIpAddress:
                {
                    if (session->Q931Connection != NULL)
                    {
                        status = cmTransGetHostParam((HSTRANSHOST)session->Q931Connection,
                                             cmTransHostParam_localAddress,
                                             value);
                    }
                    else if (session->annexEConnection != NULL)
                    {
                        cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

                        if (annexEGetLocalAddress(transGlobals->annexECntrl, session->annexEListeningIndex, (cmTransportAddress*)value) == annexEStatusNormal)
                        {
                            /* We use an H.245 connection so the IP address will be changed if it is indicated as "any" */
                            getGoodAddress(transGlobals, NULL, NULL, cmTransH245Conn, (cmTransportAddress*)value);
                            status = cmTransOK;
                        }
                    }
                }
                break;

            case cmTransParam_callTerminating:
                {
                    *(RvBool *)value = (RvBool)m_sessionget(session, callTerminating);
                    status = cmTransOK;
                }
                break;

            case cmTransParam_extFastConnect:
                {
                    *(EFCSTATE*)value = session->extFastConnectState;
                    status = cmTransOK;
                }
                break;

            case cmTransParam_callSupportNat:
                {
                    *(RvBool *)value = (RvBool)m_sessionget(session, callSupportNat);
                    status = cmTransOK;
                }
                break;

            case cmTransParam_parallelBeforeFS:
                {
                    *(RvBool *)value = (RvBool)m_sessionget(session, parallelBeforeFS);
                    status = cmTransOK;
                }
                break;
        }

        /* release the lock on the session */
        emaReleaseLocked((EMAElement)session);
    }

    return status;
}

/**************************************************************************************
 * cmTransSetHostParam
 *
 * Purpose: Sets a specific parameter for the given host element.
 *
 * Input:   hsTranHost  - An handle to the host element
 *          param       - The name of the parameter to be set.
 *          value       - An integer or pointer to set as the new parameter.
 *          force       - In case of multiplexing parameters force the sending of FACILITY
 *                        to the remote with the new parameters.
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR cmTransSetHostParam(   IN HSTRANSHOST          hsTransHost,
                                IN TRANSHOSTPARAM       param,
                                IN const void           *value,
                                IN RvBool               force)
{
    cmTransHost     *host = (cmTransHost *)hsTransHost;
    cmTransGlobals  *transGlobals;
    TRANSERR res = cmTransOK;

    if (emaMarkLocked((EMAElement)host))
    {
        switch (param)
        {
            case cmTransHostParam_shutdownEmptyConnection:
                {
                    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransHost);

                    host->closeOnNoSessions = *(RvBool *)value;

                    /* if we have a connected host, which is TPKT and we are
                       forced to send a facility about the change, do it */
                    if ( ( (host->state == hostConnected) || (host->state == hostBusy) ) &&
                         (force) &&
                         (host->annexEUsageMode == cmTransNoAnnexE) )
                        sendGeneralFacility(transGlobals, host);
                }
                break;

            case cmTransHostParam_isMultiplexed:
                {
                    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransHost);

                    host->isMultiplexed = *(RvBool *)value;
                    /* if we have a connected host, which is TPKT and we are
                       forced to send a facility about the change, do it */
                    if ( ( (host->state == hostConnected) || (host->state == hostBusy) ) &&
                         (force) &&
                         (host->annexEUsageMode == cmTransNoAnnexE) )
                        sendGeneralFacility(transGlobals, host);
                }
                break;

            case cmTransHostParam_remoteAddress:
                {
                    if (setRemoteAddress((cmTransHost *)hsTransHost,
                                         (cmTransportAddress *)value) == cmTransErr)
                    {
                        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransHost);

                        emaReleaseLocked((EMAElement)host);
                        RvLogError(&transGlobals->hLog,
                            (&transGlobals->hLog,
                                "cmTransSetHostParam failed on updating hosts hash table"));
                        return cmTransErr;
                    }
                }
                break;

            case cmTransHostParam_localAddress:
                {
                    memcpy((void *)&(host->localAddress),
                           value,
                           sizeof(cmTransportAddress));
                    /*it is the responsibility of whoever is setting the
                      local address, to set the right localAddressInMsg field
                      according to the nat mode of the call*/
                }
                break;

            case cmTransHostParam_localAddressInMsg:
                {
                    memcpy((void *)&(host->localAddressInMsg),
                           value,
                           sizeof(cmTransportAddress));
                }
                break;

            case cmTransHostParam_annexEMode:
                {
                    host->annexEUsageMode = *(cmAnnexEUsageMode *)value;
                }
                break;

            case cmTransHostParam_socketConnection:
            case cmTransHostParam_socketh245Listen:
            case cmTransHostParam_socketBestFit:
            case cmTransHostParam_callSupportNat:
            case cmTransHostParam_firstCall:
                {
                    res = cmTransErr;
                }
                break;
        }

        emaReleaseLocked((EMAElement)host);
        return res;
    }

    return cmTransErr;
}

/**************************************************************************************
 * cmTransGetHostParam
 *
 * Purpose: Gets a specific parameter from a given host element.
 *
 * Input:   hsTranHost  - An handle to the host element
 *          param       - The name of the parameter to get.
 *
 * Output:  value       - An integer or pointer which is the value of the parameter.
 *
 **************************************************************************************/
TRANSERR cmTransGetHostParam(   IN  HSTRANSHOST     hsTransHost,
                                IN  TRANSHOSTPARAM  param,
                                OUT void            *value)
{
    cmTransHost *host = (cmTransHost *)hsTransHost;
    TRANSERR res = cmTransOK;

    if (emaLock((EMAElement)host))
    {
        switch (param)
        {
            case cmTransHostParam_shutdownEmptyConnection:
                {
                    *(RvBool *)value = host->closeOnNoSessions;
                }
                break;

            case cmTransHostParam_isMultiplexed:
                {
                    *(RvBool *)value = host->isMultiplexed;
                }
                break;

            case cmTransHostParam_remoteAddress:
                if (host->hTpkt && (host->state >= hostConnected))
                {
                    cmTransportAddress remote;
                    memset(&remote, 0, sizeof(cmTransportAddress));

                    if (tpktGetAddresses(host->hTpkt, NULL, &remote) == RV_OK)
                        memcpy(value, (void *)&(remote), sizeof(cmTransportAddress));
                    else
                        res = cmTransErr;
                }
                else
                {
                    memcpy(value, (void *)&(host->remoteAddress), sizeof(cmTransportAddress));
                }
                break;

            case cmTransHostParam_localAddress:
                if ((host->hTpkt != NULL) && (host->state >= hostConnected))
                {
                    /* TPKT host */
                    if (tpktGetAddresses(host->hTpkt, (cmTransportAddress *)value, NULL) != RV_OK)
                        res = cmTransErr;
                }
                else
                {
                    memcpy(value, (void*)(&host->localAddress), sizeof(cmTransportAddress));
                }
                break;

            case cmTransHostParam_localAddressInMsg:
                memcpy(value, (void *)&(host->localAddressInMsg), sizeof(cmTransportAddress));
                break;

            case cmTransHostParam_annexEMode:
                {
                    *(cmAnnexEUsageMode *)value = host->annexEUsageMode;
                }
                break;

            case cmTransHostParam_socketBestFit:
                {
                    if ((host->type != cmTransH245Conn) || (host->state != hostDoubleConnected))
                    {
                        *(RvH323Connection **)value = tpktGetConnection(host->hTpkt);
                    }
                    else
                    {
                        /* if the remote address is bigger or if both address are the same but the remote
                           port is bigger, then it's the better one */
                        if (compareH245Connections(host) >= 0)
                        {
                            *(RvH323Connection **)value = tpktGetConnection(host->hTpkt);
                        }
                        else
                        {
                            *(RvH323Connection **)value = tpktGetConnection(host->h245Listen);
                        }
                    }
                }
                break;

            case cmTransHostParam_socketConnection:
                {
                    *(RvH323Connection **)value = tpktGetConnection(host->hTpkt);
                }
                break;

            case cmTransHostParam_socketh245Listen:
                {
                    *(RvH323Connection **)value = tpktGetConnection(host->h245Listen);
                }
                break;

            case cmTransHostParam_callSupportNat:
                res = cmTransErr;
                break;

            case cmTransHostParam_firstCall:
                *(HATRANSSESSION *)value = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)host->firstSession);
                break;
        }

        emaUnlock((EMAElement)host);
        return res;
    }

    return cmTransErr;
}

/**************************************************************************************
 * cmTransSetSessionEventHandler
 *
 * Purpose: Sets the event handlers' pointers for session related callbacks
 *
 * Input:   hAppTrans           - A handle to the entire transport module
 *          transSessionEvents  - A structure with the pointers to the callbacks
 *          size                - The size of that structure.
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR cmTransSetSessionEventHandler( IN HAPPTRANS            hAppTrans,
                                        IN TRANSSESSIONEVENTS   *transSessionEvents,
                                        IN int                  size)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)hAppTrans;

    /* lock the entire module before installing the handlers */
    RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
    memcpy((void *)&(transGlobals->sessionEventHandlers),
           (void *)transSessionEvents,
           (RvSize_t)size);
    RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);

    return cmTransOK;
}

/**************************************************************************************
 * cmTransGetSessionEventHandler
 *
 * Purpose: Gets the event handlers' pointers for session related callbacks
 *
 * Input:   hAppTrans           - A handle to the entire transport module
 *          size                - The size of the given structure.
 *
 * Output:  transSessionEvents  - A structure with the pointers to the callbacks
 *
 **************************************************************************************/
TRANSERR cmTransGetSessionEventHandler( IN  HAPPTRANS           hAppTrans,
                                        OUT TRANSSESSIONEVENTS  *transSessionEvents,
                                        IN  int                 size)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)hAppTrans;

    /* lock the entire module before getting the handlers */
    RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
    memcpy( (void *)transSessionEvents,
            (void *)&(transGlobals->sessionEventHandlers),
            (RvSize_t)size);
    RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);

    return cmTransOK;
}

/**************************************************************************************
 * cmTransSetHostEventHandler
 *
 * Purpose: Sets the event handlers' pointers for host related callbacks
 *
 * Input:   hAppTrans       - A handle to the entire transport module
 *          transHostEvents - A structure with the pointers to the callbacks
 *          size            - The size of that structure.
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR cmTransSetHostEventHandler(    IN HAPPTRANS        hAppTrans,
                                        IN TRANSHOSTEVENTS  *transHostEvents,
                                        IN int              size)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)hAppTrans;

    /* lock the entire module before installing the handlers */
    RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
    memcpy((void *)&(transGlobals->hostEventHandlers),
           (void *)transHostEvents,
           (RvSize_t)size);
    RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);

    return cmTransOK;
}

/**************************************************************************************
 * cmTransGetHostEventHandler
 *
 * Purpose: Gets the event handlers' pointers for host related callbacks
 *
 * Input:   hAppTrans       - A handle to the entire transport module
 *          size            - The size of the given structure.
 *
 * Output:  transHostEvents - A structure with the pointers to the callbacks
 *
 **************************************************************************************/
TRANSERR cmTransGetHostEventHandler(    IN  HAPPTRANS       hAppTrans,
                                        OUT TRANSHOSTEVENTS *transHostEvents,
                                        IN  int             size)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)hAppTrans;

    /* lock the entire module before getting the handlers */
    RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
    memcpy( (void *)transHostEvents,
            (void *)&(transGlobals->hostEventHandlers),
            (RvSize_t)size);
    RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);

    return cmTransOK;
}


/**************************************************************************************
 * setQ931Host
 *
 * Purpose: chooses a Q.931 host (existing or new) and sets its local address.
 *
 * Input:   hsTransSession  - the handle to the session.
 *          localAddress    - the local address to connect from.
 *          remoteAddress   - The remote address to connect to.
 *          createNewHost   - RV_TRUE:  create new host even if multiplexed
 *                            RV_FALSE: for multiplex use existing host, if exists.
 *
 * Output:  None.
 *
 * Returned Value:  cmTransErr            - In case that an error occured.
 *                  cmTransNotMultiplexed - The given host is not multiplexed.
 *                  cmTransOK             - In case that the connection is already opened.
 *
 **************************************************************************************/
TRANSERR setQ931Host(   IN    cmTransSession        *session,
                        INOUT cmTransportAddress    *localAddress,
                        IN    cmTransportAddress    *remoteAddress,
                        IN    cmTransHost           *oldHost,
                        IN    RvBool                createNewHost)
{
    RvBool      hostIsLocked = RV_FALSE;
    cmTransHost *host = NULL;
    TRANSERR    res = cmTransOK;

    cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

    /* If allowed, look for a suitable host in the host's hash */
    if ( !createNewHost && m_sessionget(session, isMultiplexed) )
        host = findHostInHash(transGlobals, remoteAddress, localAddress, RV_TRUE, RV_FALSE);

    /* in case we got a host, make sure it's still good */
    if (host != NULL)
    {
        /* first, make sure we are able to lock the new host */
        if (emaLock((EMAElement) host))
        {
            hostIsLocked = RV_TRUE;
            if ((host->state == hostClosing) || (host->state == hostClosed) || (host->state == hostIdle))
            {
                /* the host was already closed - forget it */
                emaUnlock((EMAElement) host);
                hostIsLocked = RV_FALSE;
                host = NULL;
            }
        }
        else
        {
            /* the host was deleted from under our grasp - forget we had it */
            host = NULL;
        }
    }

    /* if host was not found and we have an old host, use it */
    if ( (host == NULL) && (oldHost != NULL) )
    {
        /* use the existing host (created when the local address was given) */
        host = oldHost;

        /* lock the host */
        if(emaLock((EMAElement) host))
            hostIsLocked = RV_TRUE;
        else
            return cmTransErr;

        /* Ignore the previous session, it is the same as this one */
        host->firstSession = NULL;

        /* set the given remote address into the hash, if none was determined yet */
        if (!cmTaHasIp(&host->remoteAddress))
            setRemoteAddress(host, remoteAddress);
    }
    else
    {
        if (oldHost != NULL)
        {
            /* We have a new host, get rid of the old one which was created just for the local address
               (what a waste, next time use diffSrcAddressInSetupAndARQ in the configuration). */
            closeHostInternal((HSTRANSHOST)oldHost, RV_TRUE);
        }
        if (host != NULL)
        {
            /* Change the lock on the session, the call, and the H.245 connection, if it exists, to the
               new lock (the host was already locked above, or we wouldn't be here) */
            transParentChange(transGlobals, host, session);
        }
    }

    /* Still no host? create a new host */
    if (host == NULL)
    {
        res = createHostByType(transGlobals,
                               (HSTRANSSESSION)session,
                               (HSTRANSHOST *)&host,
                               cmTransQ931Conn,
                               NULL,
                               cmTransNoAnnexE, 0);
        if (res != cmTransOK)
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "allocateQ931Host failed on allocating host element"));
            return cmTransErr;
        }

        /* update the given address to the host */
        memset(&host->remoteAddress, 0, sizeof(cmTransportAddress));
        host->remoteAddress.type = (cmTransportType)-1;
        memset(&host->localAddress, 0, sizeof(cmTransportAddress));
        host->localAddress.type  = (cmTransportType)-1;

        /* Write the remote address, if exists, to the hash */
        if (remoteAddress)
        {
            if (setRemoteAddress(host, remoteAddress) == cmTransErr)
            {
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog, "allocateQ931Host failed on updating hosts hash table"));
                return cmTransErr;
            }
        }

        /* Update the local address, if exists */
        if (localAddress)
        {
            host->localAddress = *localAddress;
        }
        else
        {
            memset(&host->localAddress, 0, sizeof(cmTransportAddress));
#if (RV_NET_TYPE & RV_NET_IPV6)
            /* if the remote address is an IPv6 type, make sure we use an IPv6 local address */
            if (remoteAddress != NULL)
            {
                if (remoteAddress->type == cmTransportTypeIPv6)
                    host->localAddress.type = cmTransportTypeIPv6;
            }
#endif
        }
    }

    /* lock the host */
    if (!hostIsLocked)
    {
        /* lock the host */
        if(emaLock((EMAElement) host))
            hostIsLocked = RV_TRUE;
        else
            return cmTransErr;
    }

    session->Q931Connection = host;

    /* add the session to the host */
    RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
    session->nextSession = host->firstSession;
    if (session->nextSession)
        session->nextSession->prevSession = session;
    RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
    host->firstSession = session;
    updateHostMultiplexStatus(transGlobals, host);

    /* allocate a new TPKT element for the connection */
    if (host->state == hostIdle)
    {
        getGoodAddress(transGlobals,
                       host->hTpkt,
                       NULL,
                       cmTransQ931Conn,
                       &host->localAddress);

        host->hTpkt = tpktOpen(transGlobals->tpktCntrl,
                               &host->localAddress,
                               RvH323ConnectionQ931,
                               tpktClient,
                               (EMAElement)host,
                               transQ931Handler,
                               (void *)host);
        if (host->hTpkt != NULL)
            host->state = hostConnecting;
        else
            res = cmTransErr;
    }

    /* update selected local address on the host */
    if (host->hTpkt)
    {
        getGoodAddress(transGlobals,
                       session->Q931Connection->hTpkt,
                       NULL,
                       cmTransQ931Conn,
                       &host->localAddress);

        if (localAddress)
            *localAddress = host->localAddress;
    }

    /* Unlock the host */
    emaUnlock((EMAElement) host);

    return res;
}

/**************************************************************************************
 * setAddressForH245Host
 *
 * Purpose: allocates a new H.245 host and start listening on it.
 *
 * Input:   hsTransSession  - the handle to the session.
 *          localAddress    - the local address to listen on.ng host, if exists.
 *
 * Output:  None.
 *
 * Returned Value:  cmTransErr            - In case that an error occured.
 *                  cmTransOK             - All OK
 *
 **************************************************************************************/
TRANSERR setAddressForH245Host( IN    cmTransSession      *session,
                                IN    cmTransportAddress  *localAddress)
{
    TRANSERR res;
    cmTransHost *host = NULL;
    cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    res = createHostByType(transGlobals,
                           (HSTRANSSESSION)session,
                           (HSTRANSHOST *)&host,
                           cmTransH245Conn,
                           NULL,
                           cmTransNoAnnexE, 0);
    if (res != cmTransOK)
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "setAddressForH245Host failed on allocating host element"));
        return cmTransErr;
    }
    else
    {
        TRANSERR reply = cmTransOK;

        /* lock the host */
        emaLock((EMAElement)host);
        /* mark that the h245 host is in listening mode */
        host->state = hostListenning;

        /* start listening */
        if (transGlobals->hostEventHandlers.cmEvTransHostListen)
        {
            HATRANSHOST haTransHost =
                (HATRANSHOST)emaGetApplicationHandle(((EMAElement)host));

            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                "cmEvTransHostListen(hsHost = %d(%p), haHost=%p, type=cmTransH245Conn, address = (ip:%s,port:%u))",
                emaGetIndex((EMAElement)host), host, haTransHost, getIP(localAddress, buff), localAddress->port));

            /* see if we need to get a good IP address */
            if (!cmTaHasIp(localAddress))
            {
                RvUint16 port;
                cmTransportAddress ta;
                memset(&ta, 0, sizeof(cmTransportAddress));

                if (session->Q931Connection != NULL)
                {
                    getGoodAddress(transGlobals,
                        0,
                        session->Q931Connection,
                        cmTransH245Conn,
                        &ta);
                }
                if (session->annexEConnection != NULL)
                {
                    getGoodAddress(transGlobals,
                        0,
                        session->annexEConnection,
                        cmTransH245Conn,
                        &ta);
                }
                port = localAddress->port;
                memcpy(localAddress, &ta, sizeof(*localAddress));
                localAddress->port = port;
            }

            reply = transGlobals->hostEventHandlers.cmEvTransHostListen(
                                    (HSTRANSHOST)host,
                                    haTransHost,
                                    cmTransH245Conn,
                                    localAddress);
            host->reported = RV_TRUE;
        }

        if (reply == cmTransOK)
            host->h245Listen =
                    tpktOpen(transGlobals->tpktCntrl,
                             localAddress,
                             RvH323ConnectionH245,
                             tpktServer,
                             (EMAElement)host,
                             transH245AcceptHandler,
                             (void *)host);
        else
            host->h245Listen = NULL;

        /* get the allocated local address */
        getGoodAddress(transGlobals,
                       host->h245Listen,
                       NULL,
                       cmTransH245Conn,
                       localAddress);

        session->H245Connection = host;

        if (transGlobals->hostEventHandlers.cmEvTransHostListening)
        {
            HATRANSHOST haTransHost =
                         (HATRANSHOST)emaGetApplicationHandle(((EMAElement)host));

            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                    "cmEvTransHostListening(hsHost = %d(%p), haHost=%p, type=cmTransH245Conn, address = (ip:%s,port:%u) error = %d)",
                    emaGetIndex((EMAElement)host), host, haTransHost, getIP(localAddress, buff), localAddress->port, (host->h245Listen == NULL)));

            transGlobals->hostEventHandlers.cmEvTransHostListening(
                                    (HSTRANSHOST)host,
                                    haTransHost,
                                    cmTransH245Conn,
                                    localAddress,
                                    (host->h245Listen == NULL));
            host->reported = RV_TRUE;
        }

        if ( ((host->h245Listen == NULL) && (reply != cmTransIgnoreMessage)) || emaWasDeleted((EMAElement)host) )
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog,
                     "setAddressForH245Host failed to initiate listen on H.245 TPKT"));
            host->state = hostIdle;
            emaUnlock((EMAElement)host);
            return cmTransErr;
        }
        if (reply == cmTransIgnoreMessage)
        {
            RvLogInfo(&transGlobals->hLog,
                (&transGlobals->hLog,
                     "setAddressForH245Host initiate listen on H.245 TPKT refused by user"));
        }
        else
        {
            RvBool nat = RV_FALSE;

            host->localAddress = *localAddress;
            host->localAddressInMsg = *localAddress;

            if ((cmTransGetSessionParam((HSTRANSSESSION)session, cmTransParam_callSupportNat, &nat) == cmTransOK) &&
                (nat))
            {
                if (transGlobals->sessionEventHandlers.cmEvTransNatAddressTranslation)
                {
                    HATRANSSESSION haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

                    transGlobals->sessionEventHandlers.cmEvTransNatAddressTranslation(
                        (HSTRANSSESSION) session, haTransSession, cmProtocolH245, &(host->localAddressInMsg));
                }
                *localAddress = host->localAddressInMsg;
            }

            memset(&host->remoteAddress, 0, sizeof(cmTransportAddress));
            host->remoteAddress.type = (cmTransportType)-1;
        }

        host->firstSession = session;
        emaUnlock((EMAElement)host);
    }

    return res;
}

/**************************************************************************************
 * setLocalAddressForQ931
 *
 * Purpose: allocates a new Q.931 host on a given local address.
 *
 * Input:   hsTransSession  - the handle to the session.
 *          localAddress    - the local address.
 *
 * Output:  None.
 *
 * Returned Value:  cmTransErr            - In case that an error occured.
 *                  cmTransOK             - All OK
 *
 **************************************************************************************/
TRANSERR setLocalAddressForQ931(cmTransSession *session, cmTransportAddress *localAddress)
{
    cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

    /* if we already have a host, just get its local address */
    if (session->Q931Connection)
    {
        if (emaLock((EMAElement)session->Q931Connection))
        {
            memcpy(localAddress,
                   &session->Q931Connection->localAddress,
                   sizeof(cmTransportAddress));
            emaUnlock((EMAElement)session->Q931Connection);
        }
        return cmTransOK;
    }
    else
    if (session->annexEConnection)
    {
        if (emaLock((EMAElement)session->annexEConnection))
        {
            memcpy(localAddress,
                   &session->annexEConnection->localAddress,
                   sizeof(cmTransportAddress));
            emaUnlock((EMAElement)session->annexEConnection);
        }
        return cmTransOK;
    }
    else
    {
        TRANSERR result;
        cmTransHost* host;
        RvBool nat = RV_FALSE;
        RvChar addrStr[RV_TRANSPORT_ADDRESS_STRINGSIZE];

        /* we need to allocate a new host,
           go and find/create a Q.931 host for the given address */
        result = setQ931Host(session, localAddress, NULL, NULL, RV_TRUE);

        /*nat support - set address for message*/
        if(session->Q931Connection)
            host = session->Q931Connection;
        else
            host = session->annexEConnection;
        if(emaLock((EMAElement)host))
        {
            host->localAddressInMsg = host->localAddress;
            if ((cmTransGetSessionParam((HSTRANSSESSION)session , cmTransParam_callSupportNat , &nat) == cmTransOK) &&
                (nat)) /*nat - get translated address from the application and set it in localAddressInMsg*/
            {
                if (transGlobals->sessionEventHandlers.cmEvTransNatAddressTranslation)
                {
                    HATRANSSESSION haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

                    cmTransportAddress2String(&(host->localAddressInMsg) , sizeof(addrStr) , addrStr);
                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmEvTransNatAddressTranslation hsTransSession=%d(%p) haTransSession=%p , address=%s",
                            emaGetIndex((EMAElement)session), session,  haTransSession , addrStr));

                    transGlobals->sessionEventHandlers.cmEvTransNatAddressTranslation(
                        (HSTRANSSESSION) session, haTransSession, cmProtocolQ931, &(host->localAddressInMsg));
                }
                *localAddress = host->localAddressInMsg;
            }
            emaUnlock((EMAElement)host);
        }
        return result;
    }
}

/**************************************************************************************
 * setRemoteAddressForQ931
 *
 * Purpose: sets the remote address to a Q.931, assumes that a host exists..
 *
 * Input:   hsTransSession  - the handle to the session.
 *          localAddress    - the local address.
 *          remoteAddress   = the remote address.
 *          createNewHost   -   RV_TRUE:  create new host even if multiplexed
 *                              RV_FALSE: for multiplex use existing host, if exists.
 *
 * Output:  None.
 *
 * Returned Value:  cmTransErr            - In case that an error occured.
 *                  cmTransOK             - All OK
 *
 **************************************************************************************/
TRANSERR setRemoteAddressForQ931(cmTransSession *session,
                                 cmTransportAddress *localAddress,
                                 cmTransportAddress *remoteAddress,
                                 RvBool             createNewHost)
{
    cmTransHost *oldHost = NULL;

    cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

    /* in case of multiplexing we need first to see if we have already a suitable host
       for the given address */
    if (m_sessionget(session, isMultiplexed))
    {
        /* if the existing host is for the requested address,
           this is the host we need so do nothing */
        if (emaLock((EMAElement)session->Q931Connection))
        {
            if (cmCompareTransportAddresses(&session->Q931Connection->remoteAddress,
                                            remoteAddress))
            {
                emaUnlock((EMAElement)session->Q931Connection);
                return cmTransOK;
            }

            /* we need to look for a new host, so remember the old one,
               and look for a new one matching the address */
            oldHost                 = session->Q931Connection;
            session->Q931Connection = NULL;
            emaUnlock((EMAElement)oldHost);
        }

        /* Go and find/create a Q.931 host for the given address */
        return setQ931Host(session, localAddress, remoteAddress, oldHost, createNewHost);
    }
    else
    {
        /* for normal (non multiplexed) host, create one if not exists already */
        TRANSERR answer;

        if (!session->Q931Connection)
        {
            TRANSERR err;

            err = createHostByType(transGlobals,
                                   (HSTRANSSESSION)session,
                                   (HSTRANSHOST *)&session->Q931Connection,
                                   cmTransQ931Conn,
                                   NULL,
                                   cmTransNoAnnexE, 0);

            if (err != cmTransOK)
            {
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog, "setRemoteAddressForQ931 failed on creating a new host"));
                return cmTransErr;
            }
        }

        /* now we surely have a host, set its addresses */
        if (emaLock((EMAElement)session->Q931Connection))
        {
            /* set the local address, if given */
            if (localAddress)
            {
                memcpy(&session->Q931Connection->localAddress, localAddress, sizeof(cmTransportAddress));
            }
            else
            {
                memset(&session->Q931Connection->localAddress, 0, sizeof(cmTransportAddress));
            }

            /* Add the remote address to the host's hash */
            if (setRemoteAddress(session->Q931Connection, remoteAddress) == cmTransErr)
            {
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog, "setRemoteAddressForQ931 failed on updating hosts hash table [1]"));
                answer = cmTransErr;
            }
            else
            {
                /* all is well */
                session->Q931Connection->firstSession = session;
                updateHostMultiplexStatus(transGlobals, session->Q931Connection);
                answer = cmTransOK;
            }
            emaUnlock((EMAElement) session->Q931Connection);
        }
        else
            /* no host in this stage means a problem */
            answer = cmTransErr;

        return answer;
    }
}

/**************************************************************************************
 * allocateAnnexEHost
 *
 * Purpose: looks for an existing annex E host to the given address, if one doesn't
 *          exist allocate it.
 *
 * Input:   hsTransSession  - the handle to the session.
 *          remoteAddress   - The remote address to connect to.
 *          localAddress    - The local address to connect from.
 *
 * Output:  None.
 *
 * Returned Value:  cmTransErr            - In case that an error occured.
 *                  cmTransOK             - All OK.
 *
 **************************************************************************************/
static void allocateAnnexEHost(cmTransSession *session,
                               cmTransportAddress *remoteAddress,
                               cmTransportAddress *localAddress)
{
    cmTransHost *host = session->annexEConnection;
    cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);
    RvUint8 index = 0;

    /* if the given local address complies with any of the annex E listening addresses, use that
    address. otherwise, ignore the listening address */
    if (localAddress != NULL)
        if (annexEGetLocalAddressIndex(transGlobals->annexECntrl, localAddress, &index) != annexEStatusNormal)
            localAddress = NULL;

    /* If no annex E host, try first to look for an existing one in the hash */
    if (!host)
        host = findHostInHash(transGlobals, remoteAddress, localAddress, RV_TRUE, RV_TRUE);
    /* Still no host, we must create a new one */
    if (!host)
    {
        TRANSERR res = createHostByType(transGlobals,
                                        (HSTRANSSESSION)session,
                                        (HSTRANSHOST *)&host,
                                        cmTransQ931Conn,
                                        NULL,
                                        cmTransUseAnnexE,
                                        index);
        if (res != cmTransOK)
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "allocateAnnexEHost failed on allocating new host"));
            return;
        }
        else
        {
            /* Write the new host to the hash and get its local address */
            setRemoteAddress(host, remoteAddress);
            annexEGetLocalAddress(transGlobals->annexECntrl, index, &host->localAddress);
        }
    }
    else
    {
        /* Change the lock on the session, the call, and the H.245 connection, if it exists, to the
           new lock */
        transParentChange(transGlobals, host, session);
    }

    session->annexEConnection = host;
}

/**************************************************************************************
 * cmTransSetAddress
 *
 * Purpose: sets the address, local and remote, or a host for a given session.
 *          This routine serves both Q.931 (TPKT & annex E) and H.245 connections.
 *
 * Input:   hsTransSession          - the handle to the session.
 *          localAddress            - the local address to connect from or listen on.
 *          remoteAddress           - The remote TPKT address to connect to.
 *          annexEremoteAddress     - The remote annex E address to connect to.
 *          hsTransHost             - An optional host that overrides the given addresses.
 *          type                    - Q.931 TPKT or H.245 addresses.
 *          newMultiplex            -   RV_TRUE:  create new host even if multiplexed
 *                                      RV_FALSE: for multiplex use existing host, if exists.
 *
 * Output:  None.
 *
 * Returned Value:  cmTransErr            - In case that an error occured.
 *                  cmTransNotMultiplexed - The given host is not multiplexed.
 *                  cmTransOK             - In case that the connection is already opened.
 *
 **************************************************************************************/
TRANSERR cmTransSetAddress(IN    HSTRANSSESSION      hsTransSession,
                           INOUT cmTransportAddress  *localAddress,
                           IN    cmTransportAddress  *remoteAddress,
                           IN    cmTransportAddress  *annexEremoteAddress,
                           IN    HSTRANSHOST         hsTransHost,
                           IN    TRANSCONNTYPE       type,
                           IN    RvBool              newMultiplex)
{
    cmTransSession *session = (cmTransSession *)hsTransSession;
    cmTransHost    *host    = (cmTransHost *)hsTransHost;
    cmTransGlobals *transGlobals;
    TRANSERR       answer = cmTransOK;
    int            givenAddresses = 0;

    if (!hsTransSession)
        return cmTransErr;

    /* lock the session */
    if(emaLock((EMAElement) session))
    {
        /* get the global data of the module */
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

        /* determine which type of addresses were given */
        if ( (localAddress) && (!remoteAddress) && (!annexEremoteAddress) )
            givenAddresses = justLocal;
        else
            givenAddresses = justRemote;

        /* according to the connection type, set the addresses */
        switch (type)
        {
            case cmTransH245Conn:
            {
                if ( (session->H245Connection) || (remoteAddress) )
                {
                    emaUnlock((EMAElement) session);
                    return cmTransErr;
                }
                else
                    answer = setAddressForH245Host(session, localAddress);
            }
            break;

            case cmTransQ931Conn:
            {
                /* treat the Q.931 according to the parameters given */

                /* In case that a host was given rather than addresses */
                if ((host != NULL) && emaLock((EMAElement)host))
                    /* the host must be locked, since we're about to change locks (lint forces the NULL check) */
                {
                    if ( (!host->isMultiplexed) || (!host->remoteIsMultiplexed) )
                    {
                        RvLogInfo(&transGlobals->hLog,
                            (&transGlobals->hLog, "allocateQ931Host can't use the given host, its not multiplexed"));
                        emaUnlock((EMAElement) host);
                        emaUnlock((EMAElement) session);
                        return cmTransHostNotMultiplexed;
                    }
                    else
                    if (host->annexEUsageMode != cmTransNoAnnexE)
                    {
                        session->annexEConnection = host;
                        session->Q931Connection   = NULL;
                    }
                    else
                    {
                        session->Q931Connection   = host;
                        session->annexEConnection = NULL;

                        if (session != host->firstSession)
                        {
                            /* add the session to the host */
                            RvMutexLock(&transGlobals->lockSessionsList, transGlobals->logMgr);
                            session->nextSession = host->firstSession;
                            if (session->nextSession)
                                session->nextSession->prevSession = session;
                            RvMutexUnlock(&transGlobals->lockSessionsList, transGlobals->logMgr);
                            host->firstSession = session;
                        }
                        updateHostMultiplexStatus(transGlobals, host);
                    }

                    /* Change the lock on the session, the call, and the H.245 connection, if it exists, to the
                       new lock */
                    transParentChange(transGlobals, host, session);

                    m_sessionset(session, hostWasSet, RV_TRUE);
                    emaUnlock((EMAElement) host);
                    answer = cmTransOK;
                }
                else
                /* Treat setting a local address for Q.931 */
                if (givenAddresses == justLocal)
                {
                    answer = setLocalAddressForQ931(session, localAddress);
                }
                else
                /* treat setting a remote address for Q.931 */
                if (givenAddresses == justRemote)
                {
                    /* If a host was given it overrides the address setting */
                    if (m_sessionget(session, hostWasSet))
                    {
                        emaUnlock((EMAElement)session);
                        return cmTransOK;
                    }
                    /* do we need an annex E host:
                       if we have a session with a potential annex E connection
                       we need to create an annex E host */
                    if (session->annexEUsageMode != cmTransNoAnnexE)
                    {
                        if ( (annexEremoteAddress) && (transGlobals->annexESupported) )
                        {
                            if ((localAddress == NULL) && (session->Q931Connection != NULL))
                            {
                                allocateAnnexEHost(session, annexEremoteAddress,
                                    &session->Q931Connection->localAddress);
                            }
                            else
                                allocateAnnexEHost(session, annexEremoteAddress, localAddress);
                        }
                        else
                        {
                            RvLogError(&transGlobals->hLog,
                                (&transGlobals->hLog, "cmTransSetAddress needs annex E remote address session=%d(%p)",
                                    emaGetIndex((EMAElement)session), session));
                            answer = cmTransErr;
                        }
                    }
                    else
                    {
                        /* if we have annex E host, close it (probably will never occur) */
                        if (session->annexEConnection)
                        {
                            cmTransHost * annexEConnection = session->annexEConnection;
                            if (emaLock((EMAElement)annexEConnection))
                            {
                                transHostClose((HSTRANSHOST)annexEConnection, RV_FALSE);
                                emaUnlock((EMAElement)annexEConnection);
                            }
                        }
                    }

                    /* do we need a TPKT host:
                       if we have a session with a potential TPKT connection
                       we need to create a TPKT host too */
                    if (session->annexEUsageMode != cmTransUseAnnexE)
                    {
                        if (remoteAddress)
                            answer = setRemoteAddressForQ931(session,
                                                             localAddress,
                                                             remoteAddress,
                                                             newMultiplex);
                        else
                        {
                            RvLogError(&transGlobals->hLog,
                                (&transGlobals->hLog, "cmTransSetAddress needs tpkt remote address session=%d(%p)",
                                    emaGetIndex((EMAElement)session), session));
                            answer = cmTransErr;
                        }
                    }
                    else
                    {
                        /* if we have TPKT host, close it (it may have been created for the local
                           address) */
                        if (session->Q931Connection)
                            closeHostInternal((HSTRANSHOST)session->Q931Connection, RV_TRUE);
                    }
                }
                else
                {
                    /* No local address nor remote one nor host, must be a mistake */
                    emaUnlock((EMAElement)session);
                    return cmTransErr;
                }
            }
            break;
        }
        emaUnlock((EMAElement)hsTransSession);
        return answer;
    }
    else
        /* no session was given !!! */
        return cmTransErr;
}


/**************************************************************************************
 * cmTransHostTpktSetAddress
 *
 * Purpose: sets (change!!!) the remote address. This routine serves Q.931 (TPKT only)
 *
 * Input:   remoteAddress           - The remote TPKT address to connect to.
 *          hsTransHost             - An optional host that overrides the given addresses.
 *
 * Output:  None.
 *
 * Returned Value:  cmTransErr            - In case that an error occured.
 *                  cmTransNotMultiplexed - The given host is not multiplexed.
 *                  cmTransOK             - In case that the connection is already opened.
 **************************************************************************************/
TRANSERR cmTransHostTpktSetAddress(IN    HSTRANSHOST         hsTransHost,
                                   IN    cmTransportAddress  *remoteAddress)
{
    TRANSERR       res = cmTransOK;
    cmTransHost    *host    = (cmTransHost *)hsTransHost;
    cmTransGlobals *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar         buff[RV_ADDRESS_MAXSTRINGSIZE];
#endif

    RvLogInfo(&transGlobals->hLog,
        (&transGlobals->hLog, "cmTransHostTpktSetAddress:addr=[%s:%u]",
        getIP(remoteAddress, buff), remoteAddress->port));

    if (remoteAddress)
    {
        /* Add the remote address to the host's hash */

        /* first make it possible*/
        if (host->hashEntry != NULL)
        {
            RvLockGet(&transGlobals->lockHash, transGlobals->logMgr);
            hashDelete(transGlobals->hHashHosts, host->hashEntry);
            RvLockRelease(&transGlobals->lockHash, transGlobals->logMgr);
        }

        host->hashEntry = NULL;
        if (setRemoteAddress(host, remoteAddress) == cmTransErr)
        {
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "setRemoteAddress failed on updating hosts hash table [1]"));
            res = cmTransErr;
        }
    }
    else
    {
        RvLogError(&transGlobals->hLog,
            (&transGlobals->hLog, "cmTransSetAddress needs tpkt remote address session"));
        res = cmTransErr;
    }

    return res;
}


/**************************************************************************************
 * cmTransQ931Connect
 *
 * Purpose: Starts the process of connecting on behalf of the given session.
 *          For regular connection this will initiate a connect procedure,
 *          for multiplexed operation on an existing connection, this will
 *          report that a connection was established.
 *
 * Input:   hsTransSession  - the handle to the session.
 *
 * Output:  None.
 *
 * Returned Value:  cmTransWouldBlock - In case that a connect procedure was instigated but
 *                                      not yet completed.
 *                  cmTransOK         - In case that the connection is already opened.
 *
 **************************************************************************************/
TRANSERR cmTransQ931Connect(IN HSTRANSSESSION   hsTransSession)
{
    cmTransSession      *session = (cmTransSession *)hsTransSession;
    cmTransGlobals      *transGlobals;
    TRANSINTCONNTYPE    type = (TRANSINTCONNTYPE)-1;
    TRANSERR            res;

    /* lock the session */
    if(emaLock((EMAElement) hsTransSession))
    {
        cmTransHost *q931host   = session->Q931Connection;
        cmTransHost *annexEhost = session->annexEConnection;

        /* retrieve the transport module global area */
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransSession);

        /* if we have both connections for TPKT and Annex E we need
           to see if they are connected.
           If one is connected       - use it
           if both are connected     - choose according to the annexEUsageMode flag
           if both are NOT connected - make a competition
        */
        if ( (q931host) && (annexEhost) )
        {
            if (emaLock((EMAElement)q931host))
            {
                if (emaLock((EMAElement)annexEhost))
                {
                    if ((q931host->hostIsApproved) && (annexEhost->hostIsApproved))
                    {
                        /* both are connected. we must make sure that we are using the correct
                           parent lock (needed especially when changing to Annex E */
                        if (session->annexEUsageMode == cmTransPreferedAnnexE)
                        {
                            type = cmTransAnnexEConn;
                            determineWinningHost(transGlobals, session, RV_TRUE);
                            transParentChange(transGlobals, session->annexEConnection, session);
                        }
                        else
                        {
                            type = cmTransTPKTConn;
                            determineWinningHost(transGlobals, session, RV_FALSE);
                        }
                    }
                    else
                    if (q931host->hostIsApproved)
                    {
                        type = cmTransTPKTConn;
                        determineWinningHost(transGlobals, session, RV_FALSE);
                    }
                    else
                    if (annexEhost->hostIsApproved)
                    {
                        type = cmTransAnnexEConn;
                        determineWinningHost(transGlobals, session, RV_TRUE);
                    }
                    else
                        type = cmTransTPKTAnnexECompetition;

                    emaUnlock((EMAElement)q931host);
                    emaUnlock((EMAElement)annexEhost);
                }
                else
                {
                    emaUnlock((EMAElement)q931host);
                    emaUnlock((EMAElement)hsTransSession);
                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmTransQ931Connect failed on lock of host %d(%p)",
                            emaGetIndex((EMAElement)session->annexEConnection), session->annexEConnection));
                    return cmTransErr;
                }
            }
            else
            {
                emaUnlock((EMAElement)hsTransSession);
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog, "cmTransQ931Connect failed on lock of host %d(%p)",
                        emaGetIndex((EMAElement)session->Q931Connection), session->Q931Connection));
                return cmTransErr;
            }
        }
        else
        if (q931host)
        {
            type = cmTransTPKTConn;
            determineWinningHost(transGlobals, session, RV_FALSE);
        }
        else
        if (annexEhost)
        {
            type = cmTransAnnexEConn;
            determineWinningHost(transGlobals, session, RV_TRUE);
        }
        else
        {
            /* A problem! We have no addresses, no given host, we
               can't connect */
            emaUnlock((EMAElement)hsTransSession);
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "cmTransQ931Connect received no host and no addresses"));
            return cmTransErr;
        }

        /* set the tunneling state of the session */
        if (m_sessionget(session, isTunnelingSupported))
            session->tunnelingState = tunnPossible;
        else
            session->tunnelingState = tunnNo;

        /* connect the required host according to type */
        res = transHostConnect(hsTransSession, type);

        emaUnlock((EMAElement)hsTransSession);

        return res;
    }
    else
        return cmTransErr;

}


/**************************************************************************************
 * cmTransHostTpktConnect
 *
 * Purpose: Used to connect alternate EPs when primary connection fails.
 *          Starts the process of connecting on behalf of the given session.
 *          For regular connection only!
 *
 * Input:   hsTransSession  - the handle to the session.
 *
 * Output:  None.
 *
 * Returned Value:  cmTransWouldBlock - In case that a connect procedure was instigated but
 *                                      not yet completed.
 *                  cmTransOK         - In case that the connection is already opened.
 *
 **************************************************************************************/
TRANSERR cmTransHostTpktConnect(IN HSTRANSSESSION   hsTransSession)
{
  cmTransSession      *session = (cmTransSession *)hsTransSession;
  TRANSINTCONNTYPE    type =  cmTransTPKTConn;
  TRANSERR            res;

  /* lock the session */
  if(emaLock((EMAElement) hsTransSession))
  {
    cmTransHost     *host = session->Q931Connection;

    /* set host state to Idle. This will cause the Tpkt to be opened again. */
    host->state = hostIdle;

    /* reset reportedQ931Connected. This will permit to send Setup to altEp */
    m_sessionset(session, reportedQ931Connect, RV_FALSE);
    /* RESET the flag that we already reported on an H.245 connection for this session */
    m_sessionset(session, reportedH245Connect, RV_FALSE);
    /* RESET the flag that we have passed the first tunneled response for the session */
    m_sessionset(session, notFirstTunnelingMessage, RV_FALSE);

    /* clean UNCONDITIONALY the saved tunneled messages */
    {
        cmTransGlobals  *transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransSession);
        HRPOOLELEM      msg = session->firstMessage;

        while (msg != NULL)
        {
            msg = extractMessageFromPool(transGlobals, session, RV_TRUE);
        }
    }


    /* set the tunneling state of the session */
    if (m_sessionget(session, isTunnelingSupported))
      session->tunnelingState = tunnPossible;
    else
      session->tunnelingState = tunnNo;



    /* connect the required host according to type */
    res = transHostConnect(hsTransSession, type);

    emaUnlock((EMAElement)hsTransSession);

    return res;
  }
  else
    return cmTransErr;
}


/**************************************************************************************
 * cmTransSendMessage
 *
 * Purpose: Decodes and sends a message for the given session thru its host connection.
 *
 * Input:   hsTransSession  - the handle to the session.
 *          pvtNode         - The pvt of the given decoded message.
 *          type            - The type of the message (Q.931/H.245)
 *
 * Output:  None.
 *
 * Returned Value:  cmTransWouldBlock - In case that there aren't enough buffers.
 *                  cmTransOK         - In case that the send was successful.
 *                  cmTransErr        - In case of fatal error.
 *
 **************************************************************************************/
TRANSERR cmTransSendMessage(IN HSTRANSSESSION   hsTransSession,
                            IN int              pvtNode,
                            IN TRANSTYPE        type)
{
    cmTransSession  *session    = (cmTransSession *)hsTransSession;
    cmTransHost     *host = NULL;
    cmTransGlobals  *transGlobals;
    RvUint8         *buffer;
    int             encMsgSize;
    TRANSERR        res;
    RvBool          needToSaveSetupMessage = RV_FALSE;
    int             msgType = -1;
    HRPOOLELEM      savedMessage;
	int             nSendRC= 0 ; //注意 1 终端用 0 MCU用

    /* lock the session */
    if(emaMarkLocked((EMAElement) hsTransSession))
    {
        /* retrieve the transport module global area */
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransSession);

        if (type == cmTransQ931Type)
            msgType = pvtGetChildTagByPath(transGlobals->hPvt, pvtNode, "message", 1);

        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog,
                "cmTransSendMessage called(hsTransSession=%d(%p), pvtNode=%d (type=%d), type=%d",
                emaGetIndex((EMAElement)hsTransSession), hsTransSession, pvtNode, msgType, type));

        if (pvtNode < 0)
        {
            /* unlock the session */
            emaReleaseLocked((EMAElement) hsTransSession);
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "cmTransSendMessage called with wrong pvtNode"));
            return cmTransErr;
        }

		//{{gaoshizhong add 20140923 判断网线是否拔掉,拔掉则不在发挂断信令
		if ( nSendRC && (type == cmTransQ931Type) && (msgType == (int)cmQ931releaseComplete) )
		{
			if(GetNetStateEnable() ==0)
			{
				emaReleaseLocked((EMAElement) hsTransSession);
				RvLogError(&transGlobals->hLog,(&transGlobals->hLog, "cmTransSendMessage called with wrong net state"));
				OutputDebugString("GetNetStateEnable State:Not available\n");
				return cmTransErr;
			}
		}
		//}}gaoshizhong add 20140923 判断网线是否拔掉,拔掉则不在发挂断信令

        /* find the host to be used */
        switch (type)
        {
            case cmTransQ931Type:
            {
                if (msgType == (int)cmQ931setup)
                {
                    if (session->annexEConnection)
                    {
                        host = session->annexEConnection;

                        /* if we also have TPKT host, we're in a race, so the SETUP will need to be sent again */
                        if (session->Q931Connection)
                            needToSaveSetupMessage = RV_TRUE;
                    }
                    else
                        host = session->Q931Connection;
                }
                else
                {
                    if ( (session->annexEConnection) &&
                        (session->annexEConnection->hostIsApproved || (msgType == (int)cmQ931releaseComplete)) )
                        host = session->annexEConnection;
                    else
                    if ( (session->Q931Connection) && (session->Q931Connection->hostIsApproved) )
                        host = session->Q931Connection;
                    else
                    {
                        /* unlock the session */
                        emaReleaseLocked((EMAElement) hsTransSession);
                        RvLogInfo(&transGlobals->hLog,
                            (&transGlobals->hLog,
                                "cmTransSendMessage no available host for non-setup message (%d-%p) [%p - %d, %p - %d]",
                                emaGetIndex((EMAElement)hsTransSession), hsTransSession,
                                session->Q931Connection, (session->Q931Connection?session->Q931Connection->hostIsApproved:0),
                                session->annexEConnection,(session->annexEConnection?session->annexEConnection->hostIsApproved:0)));
                        return cmTransErr;
                    }
                }

                break;
            }

            case cmTransH245Type:
            {
                /* if no tunneling, get the real H.245 host */
                if ((session->tunnelingState == tunnNo) ||
                    (session->tunnelingState == tunnRejected) ||
                    m_sessionget(session, switchToSeparateWasAsked))
                {
                    if (!m_sessionget(session, reportedH245Connect))
                    {
                        /* unlock the session */
                        emaReleaseLocked((EMAElement) hsTransSession);
                        return cmTransErr;
                    }
                    else if ((session->H245Connection) && (session->H245Connection->dummyHost))
                    {
                        /* We have a reported existing host (i.e. no tunneling)
                        which has no TPKT object,
                        i.e. it's a dummy control session that is not connected to
                        the network and thus not allowed to send messages */
                        emaReleaseLocked((EMAElement) hsTransSession);
                        return cmTransDummyHost;
                    }

                    host = session->H245Connection;
                    /* if h.245 doesn't exist and we're NOT in tunneling, reject message */
                    if (!host)
                    {
                        /* unlock the session */
                        emaReleaseLocked((EMAElement) hsTransSession);
                        return cmTransErr;
                    }
                }
                break;
            }

            default:
                break;
        }

        /* lock the host */
        if (host)
        {
            if(!emaMarkLocked((EMAElement)host))
            {
                emaReleaseLocked((EMAElement) hsTransSession);

                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "cmTransSendMessage failed to lock host = %p", host));

                return cmTransErr;
            }
            if ((host->state == hostClosing) || (host->state == hostClosed))
            {
                emaReleaseLocked((EMAElement)host);
                emaReleaseLocked((EMAElement)hsTransSession);
                RvLogDebug(&transGlobals->hLog,
                    (&transGlobals->hLog, "cmTransSendMessage - host already closed (%p)", host));
                return cmTransErr;
            }
        }

        /* if a host was found or it's a tunneled message, handle the message */
        if ( (host) || (type == cmTransH245Type) )
        {
            RvBool waitForSetup = (!m_sessionget(session, setupSent) && m_sessionget(session, outgoing) &&
                                   (type == cmTransQ931Type) && (msgType != (int)cmQ931setup));
            /* - Add fields to Q.931 messages
               - Save H.245 tunneled messages
               - Insert H.245 addresses, if necessary to the messages
               - Insert tunneled messages
               - on first message insert to CAT
            */

            /* We want to accumulate tunneled messages that might be
               instigated by the sending of this message.
               In case it's the first send we hold tunneling, in all other
               cases we leave it in hold state */
            if (!m_sessionget(session, holdTunneling))
            {
                m_sessionset(session, holdTunneling, RV_TRUE);
                res = processOutgoingMessage(session, host, pvtNode, type);
                m_sessionset(session, holdTunneling, RV_FALSE);
            }
            else
                res = processOutgoingMessage(session, host, pvtNode, type);

            /* It might have been a tunneled message so no actual network activity is needed */
            if ((res == cmTransIgnoreMessage) && (type == cmTransH245Type))
            {
                /* check if we need to send facility for the newly sent tunneled messages */
                /* here only, if we are in hold-tunneling mode, no point in checking */
                if (!m_sessionget(session, holdTunneling))
                    initiateTunnelingFacility(transGlobals, session);

                /* release the locks and exit */
                if (host)
                    emaReleaseLocked((EMAElement)host);
                emaReleaseLocked((EMAElement)session);
                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog, "cmTransSendMessage inhibited the actual send of the message"));
                return cmTransOK;
            }
            else if (res != cmTransOK)
            {
                /* Probable encoding error or other unexpected scenarios */
                if (host)
                    emaReleaseLocked((EMAElement)host);
                emaReleaseLocked((EMAElement)session);

                return cmTransErr;
            }

            /* Start the encoding process */
            getEncodeDecodeBuffer(transGlobals->codingBufferSize, &buffer);

            res = transEncodeMessage(host,
                                     session,
                                     transGlobals,
                                     pvtNode,
                                     buffer,
                                     &encMsgSize);
            if (res != cmTransOK)
            {
                if (res == cmTransIgnoreMessage)
                {
                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmTransSendMessage: Ignored by application. Message %d was not really sent.", pvtNode));
                    res = cmTransOK;
                }
                else
                {
                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmTransSendMessage failed to obtain Encoding buffer res = %d", res));
                }

                if (host)
                    emaReleaseLocked((EMAElement)host);
                emaReleaseLocked((EMAElement)session);
                return res;
            }

            savedMessage = saveMessageToPool(transGlobals,
                                            (void *)host,
                                            buffer,
                                            encMsgSize,
                                            RV_FALSE,
                                            (msgType == (int)cmQ931setup), /* setup must be the first to be sent */
                                            session->CRV);
            if ((savedMessage) && (needToSaveSetupMessage))
            {
                saveMessageToPool(transGlobals,
                                  (void *)session->Q931Connection,
                                  buffer,
                                  encMsgSize,
                                  RV_FALSE,
                                  RV_TRUE,  /* setup must be the first to be sent */
                                  session->CRV);
            }

            if (!savedMessage)
            {
                /* NO BUFFERS, add the session to the list to be notified on buffers released */
                addSessionToPendingList(transGlobals, session);
                if (host)
                    emaReleaseLocked((EMAElement)host);
                emaReleaseLocked((EMAElement)session);
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog, "cmTransSendMessage has no buffers for host %d(%p)",
                        emaGetIndex((EMAElement)host), host));
                return cmTransWouldBlock;
            }

            if (waitForSetup)
            {
                if (host)
                    emaReleaseLocked((EMAElement)host);
                emaReleaseLocked((EMAElement)session);
                RvLogDebug(&transGlobals->hLog,
                    (&transGlobals->hLog, "cmTransSendMessage type %d will wait for setup on host %d(%p)",
                        msgType, emaGetIndex((EMAElement)host), host));
                return cmTransOK;
            }

            /* initiate the actual send through the appropriate protocol */
            RvLogDebug(&transGlobals->hLog,
                (&transGlobals->hLog, "cmTransSendMessage calls sendMessageOnHost"));

            res = sendMessageOnHost(transGlobals, host);
            if ( (res != cmTransOK) && (res != cmTransConnectionBusy) )
            {
                int hostIndex = 0;

                if (host)
                {
                    hostIndex = emaGetIndex((EMAElement)host);
                    emaReleaseLocked((EMAElement)host);
                }
                emaReleaseLocked((EMAElement)session);

                if (res == cmTransConnectionClosed)
                {
                    /* connection has been closed, no need to report an error */
                    RvLogInfo(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmTransSendMessage did not send, host %d(%p) is closed",
                            hostIndex, host));
                    return cmTransErr;
                }

                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog, "cmTransSendMessage Failed on send for host %d(%p)",
                        hostIndex, host));
                return cmTransErr;
            }

            if (msgType == (int)cmQ931setup)
            {
                m_sessionset(session, setupSent, RV_TRUE);
            }

            /* check if the above message caused a startH245 facility message, if so send it */
            if (!emaWasDeleted((EMAElement)session))
                sendStartH245Facility(transGlobals, session);
        }
        else
        {
            emaReleaseLocked((EMAElement)session);
            RvLogExcep(&transGlobals->hLog,
                (&transGlobals->hLog, "cmTransSendMessage has no host !!!"));
            return cmTransErr;
        }

        if (host)
            emaReleaseLocked((EMAElement)host);
        emaReleaseLocked((EMAElement)session);


       RvLogInfo(&transGlobals->hLog,
           (&transGlobals->hLog, "cmTransSendMessage succeeded !!!"));
        return cmTransOK;
    }
    else
        return cmTransErr;
}

/**************************************************************************************
 * cmTransSendH450Message
 *
 * Purpose: Sends a tunneled encoded message for the given session thru its host connection.
 *
 * Input:   hsTransSession  - the handle to the session.
 *          buffer          - The encoded tunneled message.
 *          size            - The size of the encoded message.
 *          flags:
 *             CM_H450_NO_FLAGS - will send on the next outgoing message, and will
 *                         initiate a facility messgae if call is connected.
 *             CM_H450_FORCE    - will force the stack to send a facility message
 *                         with the APDU.
 *             CM_H450_ALERTING - will send the APDU only with an alerting or a
 *                         connect message
 *
 * Output:  None.
 *
 * Returned Value:  cmTransWouldBlock - Last message was not sent yet, can't accept new one.
 *                  cmTransOK         - In case that the send was successful.
 *                  cmTransErr        - In case of fatal error.
 *
 **************************************************************************************/
TRANSERR cmTransSendH450Message(IN HSTRANSSESSION   hsTransSession,
                                IN RvUint8          *buffer,
                                IN int              size,
                                IN RvUint           flags)
{
    cmTransSession  *session    = (cmTransSession *)hsTransSession;
    cmTransGlobals  *transGlobals;
    RvBool          newNodeWasCreated = RV_FALSE;
    RvPvtNodeId     h450NodeId = RV_PVT_INVALID_NODEID, nodeId;

    /* lock the session */
    if (emaMarkLocked((EMAElement)hsTransSession))
    {
        /* retrieve the transport module global area */
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransSession);

        /* if no H.450 elemnt exists in for the call, create it */
        if ((flags & CM_H450_ALERTING) == 0)
        {
            if (!RV_PVT_NODEID_IS_VALID(session->h450Element))
            {
                session->h450Element =  pvtAddRoot(transGlobals->hPvt, transGlobals->synProtH450, 0, NULL);

                if (RV_PVT_NODEID_IS_VALID(session->h450Element))
                {
                    newNodeWasCreated = RV_TRUE;
                }
            }
            h450NodeId = session->h450Element;
        }
        else
        {
            if (!RV_PVT_NODEID_IS_VALID(session->h450AlertElement))
            {
                session->h450AlertElement =  pvtAddRoot(transGlobals->hPvt, transGlobals->synProtH450, 0, NULL);

                if (RV_PVT_NODEID_IS_VALID(session->h450AlertElement))
                {
                    newNodeWasCreated = RV_TRUE;
                }
            }
            h450NodeId = session->h450AlertElement;
        }

        if (!RV_PVT_NODEID_IS_VALID(h450NodeId))
        {
            emaReleaseLocked((EMAElement)session);
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "cmTransSendH450Message Failed on send for session %d(%p)",
                emaGetIndex((EMAElement)session),session));
            return cmTransErr;
        }

        /* add the new message to the H.450 element of the call */
        nodeId = pvtAdd(transGlobals->hPvt, h450NodeId, __nul(0), size, (char *)buffer, NULL);
        if (!RV_PVT_NODEID_IS_VALID(nodeId))
        {
            /* if the element was just created, it's empty so get rid of it */
            if (newNodeWasCreated)
            {
                pvtDelete(transGlobals->hPvt, h450NodeId);
                if ((flags & CM_H450_ALERTING) == 0)
                    session->h450Element = RV_PVT_INVALID_NODEID;
                else
                    session->h450AlertElement = RV_PVT_INVALID_NODEID;
            }

            emaReleaseLocked((EMAElement)session);
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "cmTransSendH450Message Failed on send for session %d(%p)",
                    emaGetIndex((EMAElement)session), session));
            return cmTransErr;
        }

        /* check if we need to send facility for the newly sent tunneled messages */
        if ((flags & CM_H450_FORCE) && !(flags & CM_H450_ALERTING))
        {
            if (flags & CM_H450_FORCE)
                m_sessionset(session, forceTunneledMessage, RV_TRUE);
            initiateTunnelingFacility(transGlobals, session);
        }

        emaReleaseLocked((EMAElement)session);
        return cmTransOK;
    }
    else
        return cmTransErr;
}

/**************************************************************************************
 * cmTransSetH450Element
 *
 * Purpose: Sets the H.450 elements to send in the next outgoin message.
 *
 * Input:   hsTransSession  - the handle to the session.
 *          elementNodeId   - PVT node ID of the new H.450 element.
 *
 * Output:  None.
 *
 * Returned Value:  cmTransOK         - In case that the send was successful.
 *                  cmTransErr        - In case of fatal error.
 *
 **************************************************************************************/
TRANSERR cmTransSetH450Element(IN HSTRANSSESSION   hsTransSession,
                               IN RvPvtNodeId      elementNodeId)
{
    cmTransSession  *session    = (cmTransSession *)hsTransSession;
    cmTransGlobals  *transGlobals;

    /* lock the session */
    if (emaMarkLocked((EMAElement) hsTransSession))
    {
        /* retrieve the transport module global area */
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransSession);

        /* if no H.450 element exists in for the call, create it */
        if (RV_PVT_NODEID_IS_VALID(session->h450Element))
        {
            pvtDelete(transGlobals->hPvt, session->h450Element);
        }
        session->h450Element = elementNodeId;

        emaReleaseLocked((EMAElement)session);
        return cmTransOK;
    }
    else
        return cmTransErr;
}

/**************************************************************************************
 * cmTransSendTunneledMessage
 *
 * Purpose: Sends a tunneled encoded message for the given session thru its host connection.
 *
 * Input:   hsTransSession  - the handle to the session.
 *          msg             - The node id of the encoded tunneled message.
 *          type            - The type of the message (AnnexM/AnnexL etc.)
 *          force           - Should we send a facility immediately with the message.
 *
 * Output:  None.
 *
 * Returned Value:  cmTransWouldBlock - Last message was not sent yet, can't accept new one.
 *                  cmTransOK         - In case that the send was successful.
 *                  cmTransErr        - In case of fatal error.
 *
 **************************************************************************************/
TRANSERR cmTransSendTunneledMessage(IN HSTRANSSESSION   hsTransSession,
                                    IN int              msg,
                                    IN TRANSTYPE        type,
                                    IN RvBool           force)
{
    cmTransSession  *session = (cmTransSession *)hsTransSession;
    cmTransGlobals  *transGlobals;

    /* lock the session */
    if (emaMarkLocked((EMAElement) hsTransSession))
    {
        /* retrieve the transport module global area */
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransSession);

        switch (type)
        {
            case cmTransAnnexLType:
            {
                /* if no annex L element exists in for the call we can accept a new one */
                if (session->annexLElement < 0)
                    session->annexLElement = msg;
                else
                {
                    emaReleaseLocked((EMAElement)session);
                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmTransSendTunneledMessage Failed on send annex L msg for session %d(%p)",
                            emaGetIndex((EMAElement)session), session));
                    return cmTransWouldBlock;
                }
            }
            break;

            case cmTransAnnexMType:
            {
                /* if no annex L element exists in for the call we can accept a new one */
                if (session->annexMElement < 0)
                    session->annexMElement = msg;
                else
                {
                    emaReleaseLocked((EMAElement)session);
                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmTransSendTunneledMessage Failed on send annex M for session %d(%p)",
                            emaGetIndex((EMAElement)session), session));
                    return cmTransWouldBlock;
                }
            }
            break;

            default:
            {
                emaReleaseLocked((EMAElement)session);
                RvLogError(&transGlobals->hLog,
                    (&transGlobals->hLog, "cmTransSendTunneledMessage illegal type = %d  for session %d(%p)",
                        type, emaGetIndex((EMAElement)session), session));
                return cmTransErr;
            }
        }

        /* check if we need to send facility for the newly sent tunneled messages */
        if (force)
        {
            m_sessionset(session, forceTunneledMessage, RV_TRUE);
            initiateTunnelingFacility(transGlobals, session);
        }

        emaReleaseLocked((EMAElement)session);
        return cmTransOK;
    }
    else
        return cmTransErr;

}

/**************************************************************************************
 * cmTransEstablishControl
 *
 * Purpose: Starts a H.245 control for a given session after fast start or no control
 *          exists. If tunneling allowed, will use tunneling, else will open a "true"
 *          H.245 connection.
 *
 * Input:   hsTransSession  - the handle to the session.
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR cmTransEstablishControl(IN HSTRANSSESSION hsTransSession)
{
    cmTransSession *session = (cmTransSession *)hsTransSession;
    cmTransGlobals *transGlobals;

    RvBool stableFaststartState = RV_FALSE;

    if (emaMarkLocked((EMAElement)session))
    {
        /* retrieve the transport module global area */
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransSession);

        /* if we are not establishing control or not supporting H.245 altogether, reject the
           call */
        if ( m_sessionget(session, notEstablishControl) || (session->h245Stage == cmTransH245Stage_noH245) )
        {
            emaReleaseLocked((EMAElement)session);
            return cmTransErr;
        }

        /* determine if we are in a stable fast start state */
        switch (session->faststartState)
        {
            case fsApproved:
            case fsRejected:
            case fsNo:
            {
                stableFaststartState = RV_TRUE;
            }
            break;

            default:
            break;
        }

        /* mark that openning control was asked */
        m_sessionset(session, openControlWasAsked, RV_TRUE);

        /* if we are in a stable state of fast start, decide what to do according
           to the tunneling state */
        if (stableFaststartState)
        {
            /* if tunneling exists, just report that a control was established */
            if (session->tunnelingState == tunnApproved)
                reportH245Establishment(transGlobals, session);
            else
            /* if tunneling does not exist, try to establish a real H.245 connection */
            if ( (session->tunnelingState == tunnNo) || (session->tunnelingState == tunnRejected) )
            {
                cmTransportAddress nullAddr;

                memset(&nullAddr, 0, sizeof(cmTransportAddress));

                /* if no host exists yet, create it */
                if (!session->H245Connection)
                    cmTransSetAddress((HSTRANSSESSION)session,
                                      &nullAddr,
                                      NULL,
                                      NULL,
                                      NULL,
                                      cmTransH245Conn,
                                      RV_TRUE);

                connectH245(transGlobals, session, cmQ931connect);

                /* check if the above message caused a startH245 facility message, if so send it */
                if (!emaWasDeleted((EMAElement)session))
                    sendStartH245Facility(transGlobals, session);
            }
        }

        emaReleaseLocked((EMAElement)session);
        return cmTransOK;
    }
    else
        return cmTransErr;
}

/**************************************************************************************
 * cmTransSwitchToSeparate
 *
 * Purpose: Starts a "true" H.245 control connection for a given session.
 *          Can be called when the call is in Fast Start, tunneling or no control.
 *
 * Input:   hsTransSession  - the handle to the session.
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR cmTransSwitchToSeparate(IN HSTRANSSESSION hsTransSession)
{
    cmTransSession *session = (cmTransSession *)hsTransSession;
    cmTransGlobals *transGlobals;

    RvBool stableFaststartState = RV_FALSE;
    RvBool stableTunnelingState = RV_FALSE;

    if (emaMarkLocked((EMAElement)session))
    {
        /* retrieve the transport module global area */
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransSession);

        /* raise the flag, even if we don't really have control */
        m_sessionset(session, switchToSeparateWasAsked, RV_TRUE);

        /* if we are not establishing control or not supporting H.245 altogether, reject the
           call */
        if ( m_sessionget(session, notEstablishControl) || (session->h245Stage == cmTransH245Stage_noH245) )
        {
            emaReleaseLocked((EMAElement)session);
            return cmTransErr;
        }

        /* determine if we are in a stable fast start state */
        switch (session->faststartState)
        {
            case fsApproved:
            case fsRejected:
            case fsNo:
            {
                stableFaststartState = RV_TRUE;
            }
            break;

            default:
                break;
        }

        /* determine if we are in a stable tunneling state */
        switch (session->tunnelingState)
        {
            case tunnApproved:
            case tunnRejected:
            case tunnNo:
            {
                stableTunnelingState = RV_TRUE;
            }
            break;

            default:
                break;
        }

        if ( (stableFaststartState) && (stableTunnelingState) )
        {
            cmTransportAddress nullAddr;
            memset(&nullAddr, 0, sizeof(cmTransportAddress));

            /* terminate EFC */
            if (transGlobals->sessionEventHandlers.cmEvTransExtFastConnestState != NULL)
            {
                transGlobals->sessionEventHandlers.cmEvTransExtFastConnestState(
                    (HSTRANSSESSION)session, (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session),
                    (EFCSTATE) efcNo);
            }

            /* if no host exists yet, create it */
            if (!session->H245Connection)
                cmTransSetAddress((HSTRANSSESSION)session,
                                  &nullAddr,
                                  NULL,
                                  NULL,
                                  NULL,
                                  cmTransH245Conn,
                                  RV_TRUE);

            connectH245(transGlobals, session, cmQ931connect);

            /* check if the above message caused a startH245 facility message, if so send it */
            sendStartH245Facility(transGlobals, session);
        }

        emaReleaseLocked((EMAElement)session);
        return cmTransOK;
    }
    else
        return cmTransErr;
}

/**************************************************************************************
 * cmTransCreateControlSession
 *
 * Purpose: Starts a "true" H.245 control connection, in case that H.245 is allowed.
 *
 * Input:   hsTransSession  -       the handle to the session with which the new H.245
 *                                  control is to be associated.
 *          addr            -       the address to listen or connect to.
 *          startListen     -       RV_TRUE:  start listening on the address.
 *                                  RV_FALSE: try to connect to the addr.
 *          nullControlSession -    RV_TRUE:  Create a dummy control session that is not connected
 *                                         to anywhere. (This imposes the startListen to RV_FALSE).
 *                                  RV_FALSE: Create a normal control session according to
 *                                         the given params.
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR cmTransCreateControlSession(IN  HSTRANSSESSION     hsTransSession,
                                     IN  cmTransportAddress *addr,
                                     IN  RvBool             startListen,
                                     IN  RvBool             nullControlSession)
{
    HSTRANSHOST    hsTransHost = NULL;
    cmTransHost    *host;
    cmTransSession *session = (cmTransSession *)hsTransSession;
    cmTransGlobals *transGlobals;
    TRANSERR       err;
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar         buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    if (emaMarkLocked((EMAElement)session))
    {
        /* retrieve the transport module global area */
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransSession);

        RvLogInfo(&transGlobals->hLog,
            (&transGlobals->hLog,
                "cmTransCreateControlSession(hsTransSession=%p, addr=[%s:%u], startListen=%d, nullControlSession=%d",
                hsTransSession, getIP(addr, buff), addr->port, startListen, nullControlSession));

        /* if we are not supporting H.245 altogether, exit */
        if ( session->h245Stage == cmTransH245Stage_noH245)
        {
            emaReleaseLocked((EMAElement)session);
            return cmTransErr;
        }

        /* if we already have a H.245 on this session, reject it */
        if ((session->H245Connection) && !session->H245Connection->dummyHost)
        {
            emaReleaseLocked((EMAElement)session);
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "cmTransCreateControlSession was called with session having H.245 host"));
            return cmTransErr;
        }

        if (session->H245Connection == NULL)
        {
            /* create a new host element */
            err = cmTransCreateHost((HAPPTRANS)transGlobals, (HSTRANSSESSION)session, NULL, &hsTransHost);
            if (err != cmTransOK)
            {
                emaReleaseLocked((EMAElement)session);
                return err;
            }
            else
            {
                host = (cmTransHost *)hsTransHost;
                host->firstSession  = session;
                host->type          = cmTransH245Conn;

                /* Make sure this session haven't notified about H245 being connected... */
                m_sessionset(session, reportedH245Connect, RV_FALSE);
            }
        }
        else
        {
            host = session->H245Connection;
            host->firstSession  = session;
            host->type          = cmTransH245Conn;
            host->dummyHost     = RV_FALSE;
        }

        /* in case of NULL connection set the address to dummy one */
        if (nullControlSession)
        {
            memset(addr, 0, sizeof(cmTransportAddress));
            startListen = RV_FALSE;
        }

        /* if a listening control session was requested */
        if (startListen)
        {
            TRANSERR reply = cmTransOK;

            host->state = hostListenning;

            /* get the address for listening, in case of NULL connection
               just send NULL and nothing will happen */
            getGoodAddress(transGlobals,
                           host->h245Listen,
                           session->Q931Connection,
                           cmTransH245Conn,
                           addr);

            /* start a listening process on the host */
            if (transGlobals->hostEventHandlers.cmEvTransHostListen)
            {
                HATRANSHOST haTransHost =
                             (HATRANSHOST)emaGetApplicationHandle(((EMAElement)host));
                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "cmEvTransHostListen(hsHost = %d(%p), haHost=%p, type=cmTransH245Conn, address = (ip:%s,port:%u))",
                        emaGetIndex((EMAElement)host), host, haTransHost, getIP(addr, buff), addr->port));

                reply = transGlobals->hostEventHandlers.cmEvTransHostListen(
                                        (HSTRANSHOST)host,
                                        haTransHost,
                                        cmTransH245Conn,
                                        addr);
                host->reported = RV_TRUE;
            }

            /* if user approved the listen and it's a real request,
               start listening */
            if (reply == cmTransOK)
                host->h245Listen = tpktOpen(transGlobals->tpktCntrl,
                                            addr,
                                            RvH323ConnectionH245,
                                            tpktServer,
                                            (EMAElement)host,
                                            transH245AcceptHandler,
                                            host);
            else
                host->h245Listen = NULL;

            /* set the host state, and save the address */
            getGoodAddress(transGlobals,
                           host->h245Listen,
                           session->Q931Connection,
                           cmTransH245Conn,
                           &host->localAddress);

            if (transGlobals->hostEventHandlers.cmEvTransHostListening)
            {
                HATRANSHOST haTransHost =
                             (HATRANSHOST)emaGetApplicationHandle(((EMAElement)host));

                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "cmEvTransHostListening(hsHost = %d(%p), haHost=%p, type=cmTransH245Conn, address = (ip:%s,port:%u) error = %d)",
                        emaGetIndex((EMAElement)host), host, haTransHost, getIP(&host->localAddress, buff), host->localAddress.port,
                        (host->h245Listen == NULL)));

                transGlobals->hostEventHandlers.cmEvTransHostListening(
                                        (HSTRANSHOST)host,
                                        haTransHost,
                                        cmTransH245Conn,
                                        &host->localAddress,
                                        (host->h245Listen == NULL));
            }

            /* the User wants the connection */
            if (reply != cmTransIgnoreMessage)
            {
                /* if not null connection and yet the listen failed
                   we have a real problem */
                if (!host->h245Listen)
                {
                    host->state = hostIdle;
                    closeHostInternal(hsTransHost, RV_TRUE);
                    emaReleaseLocked((EMAElement)session);

                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmTransCreateControlSession Failed start listen on tpkt"));
                    return cmTransErr;
                }
                else
                {
                    RvBool nat = RV_FALSE;

                    /*set localAddressInMsg properly*/
                    session->H245Connection->localAddressInMsg = session->H245Connection->localAddress;

                    if ((cmTransGetSessionParam((HSTRANSSESSION)session, cmTransParam_callSupportNat, (RvBool*)(&nat)) == cmTransOK) &&
                        (nat) && (transGlobals->sessionEventHandlers.cmEvTransNatAddressTranslation != NULL))
                    {
                        HATRANSSESSION haTransSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);

                        transGlobals->sessionEventHandlers.cmEvTransNatAddressTranslation(
                            (HSTRANSSESSION) session, haTransSession, cmProtocolH245, &(session->H245Connection->localAddressInMsg));
                    }
                }
            }
            else
            {
                host->state = hostIdle;
                closeHostInternal(hsTransHost, RV_TRUE);
                emaReleaseLocked((EMAElement)session);

                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog, "cmTransCreateControlSession start listen on tpkt refused by user"));
                return cmTransErr;
            }

            *addr = host->localAddress;
        }
        else
        {
            TRANSERR reply = cmTransOK;

            /* set the remote address */
            if (!nullControlSession)
                setRemoteAddress(host, addr);

            /* start a connect to the given address */
            if (transGlobals->hostEventHandlers.cmEvTransHostConnecting)
            {
                HATRANSHOST haTransHost =
                             (HATRANSHOST)emaGetApplicationHandle(((EMAElement)host));

                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog,
                        "cmEvTransHostConnecting(hsHost = %d(%p), haHost=%p, type=cmTransH245Conn, address = (ip:%s,port:%u))",
                        emaGetIndex((EMAElement)host), host, haTransHost, getIP(addr, buff), addr->port));

                reply = transGlobals->hostEventHandlers.cmEvTransHostConnecting(
                                        (HSTRANSHOST)host,
                                        haTransHost,
                                        cmTransH245Conn,
                                        addr);
                host->reported = RV_TRUE;
            }

            if ( (reply == cmTransOK) && (!nullControlSession) )
            {
                cmTransportAddress localAddr;
                memset(&localAddr, 0, sizeof(localAddr));
                getGoodAddress(transGlobals, NULL, session->Q931Connection, cmTransH245Conn, &localAddr);
                if (!cmTaHasIp(&localAddr))
                {
                    memcpy(&localAddr, &transGlobals->localAddress, sizeof(localAddr));
                    localAddr.port = 0;
                }

                host->hTpkt = tpktOpen(transGlobals->tpktCntrl,
                                       &localAddr,
                                       RvH323ConnectionH245,
                                       tpktClient,
                                       (EMAElement)host,
                                       transH245Handler,
                                       host);
                if (host->hTpkt)
                {
                    if (tpktConnect(host->hTpkt, addr) != RV_OK)
                    {
                        tpktClose(host->hTpkt);
                        host->hTpkt = NULL;
                        reply = cmTransErr;
                    }
                }
            }
            else
                host->hTpkt = NULL;

            /* the user wants the control session to connect */
            if (reply != cmTransIgnoreMessage)
            {
                /* a real problem occured */
                if ( (!host->hTpkt) && (!nullControlSession) )
                {
                    closeHostInternal(hsTransHost, RV_TRUE);
                    emaReleaseLocked((EMAElement)session);

                    RvLogError(&transGlobals->hLog,
                        (&transGlobals->hLog, "cmTransCreateControlSession Failed connect on tpkt"));
                    return cmTransErr;
                }
            }
            else
            {
                closeHostInternal(hsTransHost, RV_TRUE);
                emaReleaseLocked((EMAElement)session);

                RvLogInfo(&transGlobals->hLog,
                    (&transGlobals->hLog, "cmTransCreateControlSession connect on tpkt refused by user"));
                return cmTransErr;
            }

            /* set the host state and the addresses */
            if (nullControlSession)
            {
                host->localAddress = *addr;
            }
            else
                getGoodAddress(transGlobals,
                               host->hTpkt,
                               session->Q931Connection,
                               cmTransH245Conn,
                               &host->localAddress);

            host->remoteAddress     = *addr;

            host->state = hostConnecting;

            if (nullControlSession)
            {
                /* simulate a connected host */
                host->dummyHost = RV_TRUE;

                /* Make sure we're simulating a connection */
                m_sessionset(session, zeroTimerOnH245Connection, RV_TRUE);

                /* Set a zero timer only if we don't have one set already */
                if (session->zeroTimer == NULL)
                    session->zeroTimer = RvH323TimerStartWithType(transGlobals->hTimers, zeroTimerEvent,
                    session, RvInt64Const(1, 0, 0), RV_TIMER_TYPE_ONESHOT);
            }
        }

        /* connect the H.245 host to the given session */
        session->H245Connection = host;

        emaReleaseLocked((EMAElement)session);

        return cmTransOK;
    }
    return cmTransErr;
}

/**************************************************************************************
 * cmTransCloseControlSession
 *
 * Purpose: closes an H.245 connection for a given session.
 *
 * Input:   hsTransSession  - The handle of the new or old sesson associated with the
 *                            new H.245 control.
 *
 * Output: None.
 *
 * returned value: cmTransErr - in case of error, cmTransOK - otherwise
 *
 **************************************************************************************/
TRANSERR cmTransCloseControlSession(IN  HSTRANSSESSION  hsTransSession)
{
    cmTransSession *session=(cmTransSession *) hsTransSession;
    TRANSERR       err = cmTransOK;

    if (emaMarkLocked((EMAElement)session))
    {
        /* keep a local pointer to the host - we won't have one when we return*/
        cmTransHost * H245Connection = session->H245Connection;
        /* close tunneling (and parallel) */
        session->tunnelingState         = tunnNo;
        session->parallelTunnelingState = parllNo;

        /* kill the H.245 host, if exists */
        if (emaMarkLocked((EMAElement)H245Connection))
        {
            err = closeHostInternal((HSTRANSHOST)H245Connection, RV_TRUE);
            /* aren't we lucky we kept a pointer to it? */
            emaReleaseLocked((EMAElement)H245Connection);
        }
        else
            err = cmTransErr;

        emaReleaseLocked((EMAElement)session);
    }
    else
        err = cmTransErr;

    return err;
}


/**************************************************************************************
 * cmTransTryControlAfterACF
 *
 * Purpose: sees if H245 control session should be opened after ACF.
 *
 * Input:   hsTransSession  - The handle of the new or old sesson associated with the
 *                            new H.245 control.
 *
 * Output: None.
 *
 * returned value: cmTransErr - in case of error, cmTransOK - otherwise
 *
 **************************************************************************************/
TRANSERR cmTransTryControlAfterACF(IN  HSTRANSSESSION  hsTransSession)
{
    cmTransSession *session = (cmTransSession *) hsTransSession;
    cmTransGlobals *transGlobals;

    if (emaMarkLocked((EMAElement)session))
    {
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

        if (((session->h245Stage == cmTransH245Stage_setup) || (session->h245Stage == cmTransH245Stage_early)) &&
            (needToOpenH245(session, RV_FALSE)) &&
            (session->H245Connection != NULL) && cmTaHasIp(&session->H245Connection->remoteAddress))
        {
            /* all good to go. trigger H.245 connection */
            connectH245(transGlobals, session, cmQ931setup);
        }
        else
        {
            /* or maybe this is tunneled */
            initiateTunnelingFacility(transGlobals, session);
        }
        emaReleaseLocked((EMAElement)session);
    }
    return cmTransOK;
}


/**************************************************************************************
 * cmTransHasControlSession
 *
 * Purpose: reports whether an H.245 connection exists and connected.
 *
 * Input:   hsTransSession  - The handle of the sesson associated with the H.245 control.
 *
 * Output: None.
 *
 * returned value: RV_TRUE - The h245 exists and connected; RV_FALSE - Otherwise
 *
 **************************************************************************************/
RvBool cmTransHasControlSession(IN  HSTRANSSESSION    hsTransSession)
{
    cmTransSession *session=(cmTransSession *) hsTransSession;
    RvBool answer = RV_FALSE;

    if (emaMarkLocked((EMAElement)session))
    {
        answer = (RvBool)m_sessionget(session, reportedH245Connect);
        emaReleaseLocked((EMAElement)session);
    }

    return answer;
}

/**************************************************************************************
 * cmTransGetGoodAddressForH245
 *
 * Purpose: calculates the local address of the h.245
 *
 * Input:   hsTransSession  - The handle of the session associated with the H.245 control.
 *
 * Output:  addr            - local address of the H.245 connection
 *
 * returned value: RV_TRUE - The h245 exists and connected; RV_FALSE - Otherwise
 *
 **************************************************************************************/
void cmTransGetGoodAddressForH245(IN    HSTRANSSESSION     hsTransSession,
                                  INOUT cmTransportAddress *addr)
{
    cmTransGlobals *transGlobal;
    cmTransSession *session = (cmTransSession *) hsTransSession;

    if (emaMarkLocked((EMAElement)session))
    {
        transGlobal = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransSession);
        getGoodAddress(transGlobal,
                       (session->H245Connection)?session->H245Connection->hTpkt:NULL,
                       session->Q931Connection,
                       cmTransH245Conn,
                       addr);
        emaReleaseLocked((EMAElement)session);
    }
}


/**************************************************************************************
 * cmTransGetGoodAddress
 *
 * Purpose: calculates the local address of the h.245
 *
 * Input:   happTrans       - The handle of the transport module.
 *
 * Output:  addr            - local address of the H.245 connection
 *
 * returned value: RV_TRUE - The h245 exists and connected; RV_FALSE - Otherwise
 *
 **************************************************************************************/
void cmTransGetGoodAddress(IN    HAPPTRANS          happTrans,
                           INOUT cmTransportAddress *addr)
{
    cmTransGlobals *transGlobal = (cmTransGlobals *)happTrans;
    getGoodAddress(transGlobal, NULL, NULL, cmTransH245Conn, addr);
}


/**************************************************************************************
 * cmTransGetSessionByMessage
 *
 * Purpose: Find a session by an incoming message.
 *
 * Input:   hsTransHost     - The handle of the host this message arrived on.
 *          pvtNode         - The pvt of the given decoded message.
 *
 * Output:  hsTransSession  - The handle of the session that was found.
 *          haTransSession  - The application handle of the session that was found.
 *
 * Returned Value:  cmTransOK         - In case that the send was successful (also if
 *                                      session not found because it's a general message).
 *                  cmTransErr        - In case of fatal error.
 *
 **************************************************************************************/
TRANSERR cmTransGetSessionByMessage(
    IN  HSTRANSHOST     hsTransHost,
    IN  RvPvtNodeId     pvtNode,
    OUT HSTRANSSESSION  *hsTransSession,
    OUT HATRANSSESSION  *haTransSession)
{
    TRANSERR result = cmTransErr;

    if (emaLock((EMAElement)hsTransHost))
    {
        cmTransGlobals *transGlobals;
        cmTransSession *session = NULL;

        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransHost);

        if (findSessionByMessage(transGlobals, (cmTransHost *)hsTransHost, pvtNode, RV_FALSE, &session, NULL))
        {
            if (hsTransSession != NULL)
                *hsTransSession = (HSTRANSSESSION)session;
            if (haTransSession != NULL)
            {
                HATRANSSESSION haSession = NULL;
                if (session != NULL)
                    haSession = (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session);
                *haTransSession = haSession;
            }
            result = cmTransOK;
        }

        emaUnlock((EMAElement)hsTransHost);
    }

    return result;
}


/**************************************************************************************
 * cmTransGetHApp
 *
 * Purpose: gets the hApp element according to the given host
 *
 * Input:   hsTransHost  - The handle of the host.
 *
 * returned value:  hAppATrans   - The user associated handle to the module instance.
 *
 **************************************************************************************/
HAPPATRANS cmTransGetHApp(IN HSTRANSHOST hsTransHost)
{
    cmTransGlobals  *transGlobals;
    HAPPATRANS      answer = NULL;

    if (emaMarkLocked((EMAElement)hsTransHost))
    {
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransHost);
        answer = transGlobals->hAppATrans;
        emaReleaseLocked((EMAElement)hsTransHost);
    }

    return answer;
}


/**************************************************************************************
 * cmTransSetHostApplHandle
 *
 * Purpose: set the application handle for the given host
 *
 * Input:   hsTransHost  - The handle of the host.
 *          haTransHost  - The application handle to be set.
 *
 **************************************************************************************/
TRANSERR cmTransSetHostApplHandle(IN  HSTRANSHOST hsTransHost,
                                  IN  HATRANSHOST haTransHost)
{
    cmTransGlobals  *transGlobals;
    cmTransHost *host = (cmTransHost *)hsTransHost;

    if (emaLock((EMAElement)host))
    {
        emaSetApplicationHandle((EMAElement)host, (void *)haTransHost);

        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransHost);
        RvLogDebug(&transGlobals->hLog, (&transGlobals->hLog,
           "cmTransSetHostApplHandle set host %p with handle %p", hsTransHost, haTransHost));

        emaUnlock((EMAElement)host);
    }

    return cmTransOK;
}

/**************************************************************************************
 * cmTransGetHostApplHandle
 *
 * Purpose: get the application handle for the given host
 *
 * Input:   hsTransHost  - The handle of the host.
 *
 * Output:  haTransHost  - The application handle.
 *
 **************************************************************************************/
TRANSERR cmTransGetHostApplHandle(IN  HSTRANSHOST  hsTransHost,
                                  OUT HATRANSHOST *haTransHost)
{
    cmTransGlobals  *transGlobals;
    cmTransHost *host = (cmTransHost *)hsTransHost;

    if (emaLock((EMAElement)host))
    {
        *haTransHost = (HATRANSHOST)emaGetApplicationHandle((EMAElement)host);

        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransHost);
        RvLogDebug(&transGlobals->hLog, (&transGlobals->hLog,
           "cmTransGetHostApplHandle got host %p with handle %p", hsTransHost, *haTransHost));

        emaUnlock((EMAElement)host);
    }

    return cmTransOK;
}


/******************************************************************************
 * cmTransHostSendMessage
 * ----------------------------------------------------------------------------
 * General: Send a message on the host.
 *
 * Return Value: cmTransOK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsTransHost  - The handle of the host.
 *         pvtNode      - The pvtNodeId of the message to be sent.
 * Output: none
 *****************************************************************************/
TRANSERR cmTransHostSendMessage(
    IN HSTRANSHOST  hsTransHost,
    IN RvPvtNodeId  pvtNode)
{
     cmTransHost    *host = (cmTransHost *)hsTransHost;
     cmTransGlobals *transGlobals;
     TRANSERR       answer = cmTransOK;

     if (emaLock((EMAElement)host))
     {
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransHost);
        if (!encodeAndSend(transGlobals, host, pvtNode))
        {
            answer = cmTransErr;
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "cmTransHostSendMessage Failed for host %d(%p)",
                    emaGetIndex((EMAElement)host), host));
        }
        emaUnlock((EMAElement)host);
     }

    return answer;
}


/******************************************************************************
 * cmTransHostSendRawMessage
 * ----------------------------------------------------------------------------
 * General: Send a raw encoded message on the host.
 *
 * Return Value: cmTransOK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsTransHost  - The handle of the host.
 *         buffer       - Buffer to send as the message.
 *         bufferSize   - Size of buffer to send.
 * Output: none
 *****************************************************************************/
TRANSERR cmTransHostSendRawMessage(
    IN HSTRANSHOST  hsTransHost,
    IN RvUint8      *buffer,
    IN RvSize_t     bufferSize)
{
     cmTransHost    *host = (cmTransHost *)hsTransHost;
     cmTransGlobals *transGlobals;
     TRANSERR       answer = cmTransOK;

     if (emaLock((EMAElement)host))
     {
        transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransHost);
        if (!sendRawMessage(transGlobals, host, buffer, bufferSize - (TPKT_HEADER_SIZE + MSG_HEADER_SIZE)))
        {
            answer = cmTransErr;
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "cmTransHostSendRawMessage Failed for host %d(%p)",
                    emaGetIndex((EMAElement)host), host));
        }
        emaUnlock((EMAElement)host);
     }

    return answer;
}


/******************************************************************************
 * cmTransHostTpktClose
 * ----------------------------------------------------------------------------
 * Purpose: Close!!! a tpkt object.
 * Return Value: cmTransOK on success
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsTransHost  - The handle of the host.
 * Output: cmTransOK
 *****************************************************************************/
TRANSERR cmTransHostTpktClose(IN HSTRANSHOST  hsTransHost)
{
    cmTransHost    *host = (cmTransHost *)hsTransHost;

    /* We are here only in AltEp, when there were no answer from called party.
       Setting tpkt state as closed permits to vehemently close a tpkt socket
       As a result we can immediately open other tpkt connection and bind the new socket
       to the same port
    */
    tpktSetClosed(host->hTpkt);

    tpktClose(host->hTpkt);
    return cmTransOK;
}


/******************************************************************************
 * cmTransHostReceiveRawMessage
 * ----------------------------------------------------------------------------
 * General: Receive a raw Q.931 encoded message on the host.
 *
 * Return Value: cmTransOK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsTransHost  - The handle of the host.
 *         buffer       - Buffer to receive as the message.
 *         bufferSize   - Size of buffer to receive.
 * Output: none
 *****************************************************************************/
TRANSERR cmTransHostReceiveRawMessage(
    IN HSTRANSHOST  hsTransHost,
    IN RvUint8      *buffer,
    IN RvSize_t     bufferSize)
{
    cmTransHost    *host = (cmTransHost *)hsTransHost;
    cmTransGlobals *transGlobals;
    TRANSERR       answer = cmTransErr;

    if (emaLock((EMAElement)host))
    {
        if (host->type == cmTransQ931Conn)
        {
            transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)hsTransHost);
            receiveExtQ931MessageOnHost(transGlobals, host, buffer, bufferSize);
            answer = cmTransOK;
        }
        emaUnlock((EMAElement)host);
    }

    return answer;
}


/*---------------------------------------------------------------------------------------
 * dummyControlSession :
 *
 * defines if the control session is dummy
 *
 * Input:  hsTranSession   - An handle to the created session element
 * Output: none
 * Return: 1 - if control session is DUMMY.
 *         0 - otherwise
 *---------------------------------------------------------------------------------------*/
RvBool dummyControlSession(HSTRANSSESSION hsTransSession)
{
    cmTransSession  *session    = (cmTransSession *)hsTransSession;
    RvBool result = RV_FALSE; /* init the result as NOT dummy */

    if ((session->tunnelingState == tunnNo) ||
        (session->tunnelingState == tunnRejected))
    {
        if ((session->H245Connection) && (session->H245Connection->dummyHost))
        {
            /* We have a reported existing host (i.e. no tunneling)
            which has no TPKT object,
            i.e. it's a dummy control session that is not connected to
            the network and thus not allowed to send messages */
            result = RV_TRUE;
        }
    }
    return result;
}

/******************************************************************************
 * cmTransSetConnection
 * ----------------------------------------------------------------------------
 * General: Replaces a session's (and call) host with the given hConn.
 *
 * Return Value: cmTransOK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hAppTrans       - The handle to the transport module.
 *         hsTransSession  - The handle of the session.
 *         hConn           - The connection to set.
 *         bIsQ931Conn     - RV_TRUE if the connection is a Q931 host.
 * Output: none
 *****************************************************************************/
TRANSERR cmTransSetConnection(IN HAPPTRANS      hAppTrans,
                              IN HSTRANSSESSION hsTransSession,
                              IN HSTRANSHOST    hConn,
                              IN RvBool         bIsQ931Conn)
{
    cmTransGlobals  *transGlobals = (cmTransGlobals  *)hAppTrans;
    cmTransSession  *session = (cmTransSession *)hsTransSession;
    cmTransHost     *oldHost, *newHost = (cmTransHost *)hConn;
    TRANSERR status = cmTransErr;

    /*lock the session and the new host - in this order*/
    if (emaLock((EMAElement)session))
    {
        if  (emaLock((EMAElement)newHost))
        {
            if (bIsQ931Conn == RV_TRUE)
            {
                oldHost = session->Q931Connection;

                if (session->annexEConnection == NULL)
                {
                    /*remove the old host if exist*/
                    if (oldHost != NULL)
                    {
                        oldHost->firstSession = NULL;
                        closeHostInternal((HSTRANSHOST)oldHost, RV_TRUE);
                    }

                    /*replace lock of call/session with the lock of the host*/
                    transParentChange(transGlobals, newHost, session);
                    /*set new host*/
                    session->Q931Connection = newHost;
                    session->nextSession = newHost->firstSession;
                    newHost->firstSession = session;
                    status = cmTransOK;
                }
            }
            else
            {
                void * elemList[3] = {NULL, NULL, NULL};
                oldHost = session->H245Connection;

                /* remove the old host if exist */
                if (oldHost != NULL)
                {
                    oldHost->firstSession = NULL;
                    closeHostInternal((HSTRANSHOST)oldHost, RV_TRUE);
                }

                /* replace locks of new host and TPKT element with the lock of the call/session */
                /* since we may be doing this in a callback, we should do this all at once */
                elemList[0] = (void*)newHost;
                elemList[1] = (void*)newHost->hTpkt;
                transParentReplace((HAPPTRANS)transGlobals, (void**)elemList, (void*)session);

                /*set new host*/
                session->H245Connection = newHost;
                newHost->firstSession = session;
                status = cmTransOK;
            }
            /*unlock the new host*/
            emaUnlock((EMAElement)newHost);
        }
        /*unlock session*/
        emaUnlock((EMAElement)session);
    }
    return status;
}

#ifdef __cplusplus
}
#endif /* __cplusplus*/
