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


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "rvcbase.h"
#include "rvstdio.h"
#include "rvtimestamp.h"
#include "rvinternal.h"
#include "rvassert.h"
#include "h32xLog.h"
#include "cm.h"
#include "msg.h"
#include "cmintr.h"
#include "cmictrl.h"
#include "cmConfig.h"
#include "cmparam.h"
#include "cmConf.h"
#include "per.h"
#include "pvaltreeStackApi.h"
#include "commonasn.h"
#include "q931asn1.h"
#include "stkutils.h"
#include "cmutils.h"
#include "intutils.h"
#include "cmdebprn.h"
#include "cmCrossReference.h"
#include "cmiFastStart.h"
#include "cmrasinit.h"
#include "rvh323mibstats.h"
#include "seli.h"
#include "rvthreadinstance.h"
#include "rvhost.h"
#include "h323Version.h"
#include "cmH245.h"
#include "cmAutoRasIntr.h"
#include "cmiQ931.h"
#include "cmDns.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/
typedef enum
{
    initMalloc,
    initLog,
    initConfig,
    initWatchdog,
    initSystemparams,
    initTimers,
    initPvtPst,
    initConfigTrees,
    initCalls,
    initTransport,
    initCat,
    initDns,
    initRAS,
    initWatchdogResources,
    initH245,
    initFaststart,
    initStartCM
} initStep;



/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/

/* Identification string of this version.
   Unix systems like to search for such a string in the binary file itself */
char rvcm_ident[] = "@(#)RADVISION H.323 Protocol Stack "RV_H323_STACK_VERSION;

/* Global lock, used for cmInit() and cmEnd() calls */
static RvMutex rvCmGlobalMutex;

/* Number of times cmStartup() was called without cmShutdown() being called */
static RvInt rvCmStartupTimes = 0;

/* Number of bytes used after first call to cmStartup(). Used to make sure all memory was
   deallocated after cmShutdown(). */
static RvSize_t rvCmStartingMemorySize = 0;

/* Indication if we allocated a thread information object */
static RvBool rvCmAllocatedThreadInfo = RV_FALSE;



/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/
static void cleanOnFailure(
    IN cmElem       *app,
    IN initStep     step,
    IN const RvChar *errorMsg,
    IN RvStatus     status);
static int initPVT_PST(IN cmElem*app, IN int vtNodeCount);
static void endPVT_PST(IN cmElem* app);
static int configTreesInit(cmElem*app);
static void configTreesEnd(cmElem*app);
static RvStatus readSystemParameters(
    IN  cmElem* app,
    OUT int*    maxCalls,
    OUT int*    maxChannels,
    OUT int*    extraConnections,
    OUT int*    vtNodeCount,
    OUT int*    maxBuffSize,
    OUT int*    maxPoolSizeInKB,
    OUT int*    poolElemSize,
    OUT int*    maxListeningAddresses);
static RvBool rvCmWakeWatchdog(IN void* context);
static int RVCALLCONV cmWatchdogResourceCallback(
    IN  void*                       context,
    IN  RvUint32                    resource,
    IN  RvH323WatchdogResourceType  type,
    OUT RvUint32*                   value);
#ifdef RV_DEBUG
static void RVCALLCONV cmWatchdogResourcePrintCallback(
    IN void*                        context,
    IN RvLogSource*                 logSource);
#else
#define cmWatchdogResourcePrintCallback NULL
#endif
static RvStatus getNatAddresses(IN  cmElem* app, IN RvBool useAnnexE);




/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/********************************************************************************************
 * cmStartUp
 * purpose : This function should be called prior to any cmInit() or cmInitialize() calls.
 *           It must be called when several cmInitialize() functions are called from
 *           different threads at the same time.
 * input   : none
 * output  : none
 * return  : RV_OK on success, or a negative value on failure
 ********************************************************************************************/
RVAPI
RvStatus RVCALLCONV cmStartUp(void)
{
    RvStatus res;

    if (rvCmStartupTimes == 0)
    {
        RvMemoryInfo memInfo;

        /* Make sure we have a global mutex to synchronize between several stack instances */
        res = RvCBaseInit();
        if (res != RV_OK)
            return res;

        res = H32xLogInit();
        if (res != RV_OK)
            return res;

        res = RvMutexConstruct(NULL, &rvCmGlobalMutex);
        if (res != RV_OK)
        {
            H32xLogEnd();
            RvCBaseEnd();
            return res;
        }

        /* Count the number of allocated memory blocks */
        RvMemoryGetInfo(NULL, H32xLogGet(), &memInfo);
        rvCmStartingMemorySize = memInfo.bytes_requested_total;

        {
            /* todo: What are we going to do with it? Leave it here??? */
            RvThread* thInfo;

            /* Create a threadInfo object if we have to */
            thInfo = RvThreadCurrent();
            if (thInfo == NULL)
            {
                if (RvMemoryAlloc(NULL, sizeof(RvThread), NULL, (void**)&thInfo) == RV_OK)
                {
                    if (RvThreadConstructFromUserThread(NULL, thInfo) == RV_OK)
                        RvThreadSetName(thInfo, "H323Main");
                    rvCmAllocatedThreadInfo = RV_TRUE;
                }
            }
        }
    }

    RvMutexLock(&rvCmGlobalMutex, NULL);
    rvCmStartupTimes++;
    RvMutexUnlock(&rvCmGlobalMutex, NULL);

    return RV_OK;
}


/********************************************************************************************
 * cmShutdown
 * purpose : This function should be called when all stack instances where destructed using
 *           cmEnd(), before shutting down the application.
 *           It should be called only if cmStartUp() was called.
 * input   : none
 * output  : none
 * return  : RV_OK on success, or a negative value on failure
 * notes   : cmShutdown() must be called from the same thread that called cmStartUp().
 ********************************************************************************************/
RVAPI
RvStatus RVCALLCONV cmShutdown(void)
{
    if (rvCmStartupTimes <= 0)
        return RV_ERROR_UNINITIALIZED;

    RvMutexLock(&rvCmGlobalMutex, NULL);
    rvCmStartupTimes--;
    if (rvCmStartupTimes == 0)
    {
        RvMemoryInfo memInfo;

        /* See if we have any memory allocation leaks to deal with */
        RvMemoryGetInfo(NULL, H32xLogGet(), &memInfo);
        RvAssert(rvCmStartingMemorySize != memInfo.bytes_requested_total);

        /* Destruct the global lock - it's not needed anymore */
        RvMutexUnlock(&rvCmGlobalMutex, NULL);
        RvMutexDestruct(&rvCmGlobalMutex, NULL);

        /* Deallocate the thread object we're using */
        if (rvCmAllocatedThreadInfo == RV_TRUE)
        {
            RvThread* thInfo = RvThreadCurrent();
            if (thInfo != NULL)
            {
                RvThreadDestruct(thInfo);
                RvMemoryFree(thInfo, NULL);
            }
            rvCmAllocatedThreadInfo = RV_FALSE;
        }

        H32xLogEnd();

        /* Close the common core */
        RvCBaseEnd();
    }
    else
        RvMutexUnlock(&rvCmGlobalMutex, NULL);

    return RV_OK;
}


/************************************************************************
 * emaEvElementIdle
 * purpose: Event function called when the element is idle - element is
 *          still locked when raised.
 * input  : elem      - the idle element
 *          bDeleting - if true, we are now deleting the element,
 *                      otherwise, it is just idle
 * output : none
 * return : none
 ************************************************************************/
static void RVCALLCONV emaEvCallElementIdle(EMAElement * elem, RvBool bDeleting)
{
    callElem *call = (callElem*)elem;
    cmElem *app = (cmElem*)emaGetInstance(elem);

    if (bDeleting)
    {
        /* the element is now being deleted. kill everything */
        reallyCloseCall((HAPP)app, (HCALL)call);
    }
    else
    {
        /* raise the idle state on the element */
        if (app->cmMyCallEvent.cmEvCallStateChanged != NULL)
        {
            int nesting=emaPrepareForCallback((EMAElement)call);
            app->cmMyCallEvent.cmEvCallStateChanged((HAPPCALL)emaGetApplicationHandle((EMAElement)call),
                (HCALL)call, cmCallStateIdle, (cmCallStateMode_e)0);
            emaReturnFromCallback((EMAElement)call,nesting);
        }
        else
        {
            /* If the application has no state-change callback, we should close the call on IDLE state */
            cmCallClose((HCALL)call);
        }
    }
}


/************************************************************************
 * cmInitialize
 * purpose: Initializes the Conference Manager instance.
 *          This function must be called before any other H.323 function
 *          except cmGetVersion().
 * input  : name    - Configuration file name to use
 * output : lphApp  - Application handle created for the initialized
 *                    stack instance
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmInitialize(IN const RvChar * name, OUT LPHAPP lphApp)
{
    TRANSSESSIONEVENTS tse =
    {
        cmEvTransNewSession, cmEvTransConnectionOnSessionClosed,
        cmEvTransSessionNewConnection, cmEvTransNewMessage, NULL/*cmEvTransWrite*/,
        cmEvTransBadMessage, cmEvTransGetMessageNode, cmEvTransNewH450Message,
        cmEvTransNewAnnexMMessage, cmEvTransNewAnnexLMessage, cmEvTransWantsToStartControl,
        cmEvTransTunnelingWasRejected, cmEvTransExtFastConnestState, cmEvTransNatAddressTranslation,
        cmEvTransParentChanged, cmEvTransAltEpConnection
    };
    TRANSHOSTEVENTS the =
    {
        cmEvTransHostConnected,
        cmEvTransHostClosed,
        cmEvTransNewRawMessage,
        cmEvTransSendRawMessage,
        NULL/*cmEvTransHostNewMessage*/,
        NULL/*cmEvTransHostBadMessage*/,
        NULL/*cmEvTransHostMultiplexChangeState*/,
        cmEvTransHostListen,
        cmEvTransHostListening,
        cmEvTransHostConnecting
    };
    H245EvHandlers h245eh =
    {
        _cmiAPIEnter,
        _cmiAPIExit,
        _cmiCBEnter,
        _cmiCBExit,
        RvH245TimerStartEv,
        RvH245TimerCancelEv,
        RvH245TimerClearEv,
        notifyControlState,
        sendMessageForH245,
        sendMessageForH245Chan,
        NULL/*pfnH223LogicalChannelEv*/,
        NULL/*pfnH223MEStoDEMUXEv*/,
        NULL/*pfnH223MultiplexReconfigurationEv*/,
		NULL/*pfnMaxH223MUXPDUsize*/,
        NULL/*pfnReceivedResetRequestEv*/,
		NULL/*pfnResetH223ACPIgnoreData*/,
		NULL/*pfnClearH223ACPChannel*/,
		NULL/*pfnSetH223ACPDefaultMuxTbl*/,
        RvH245RandomNumberEv,
        cmCallInAnsweringState
    };
    cmElem* app;
    int     status;
    RvBool  useAnnexE;
    RvInt32 watchdogInterval;
    H245Config h245conf;

    RvTimerQueue* timersQueue;

    /* Define some stack defaults if not found in the configuration */
    RvUint32 maxCalls           = 20;
    int maxChannels             = 2;
    int extraConnections        = 0;
    int vtNodeCount             = 2700;
    int maxBuffSize             = 2048;
    int poolElemSize            = 512;
    int maxPoolSizeInKB         = maxCalls*maxBuffSize/1024;

    int proposed, accepted;

    *lphApp = NULL;

    /* Make sure startup was called */
    status = cmStartUp();
    if (status != RV_OK)
        return status;

    /* Make sure we don't have too many flags */
    RvAssert((int)e_lastCallFlag <= 32);

    /* Allocate an instance for the stack */
    if (RvMemoryAlloc(NULL, sizeof(cmElem), NULL, (void**)&app) != RV_OK)
        return -10;
    memset(app, 0, sizeof(cmElem));

    RvMutexLock(&rvCmGlobalMutex, app->logMgr);

    /* Create a threadInfo object if we have to */
    app->pThreadInfo = RvThreadCurrent();
    if (app->pThreadInfo == NULL)
    {
        static int cmStacksNum = 0;
        RvChar threadName[20];

        app->pThreadInfo = &app->threadInformation;
        status = RvThreadConstructFromUserThread(app->logMgr, app->pThreadInfo);
        if (status != RV_OK)
        {
            cleanOnFailure(app, initMalloc, NULL, -10);
            return -10;
        }
        RvSprintf(threadName, "%d H323Stack", cmStacksNum);
        RvThreadSetName(app->pThreadInfo, threadName);
    }

    /* Initialize a log instance */
    cmiCreateLog(app);

    /* Create a select engine to work with */
    status = seliInit();
    if (status == RV_OK)
    {
        status = RvSelectGetThreadEngine(app->logMgr, &app->selectEngine);
        if (status != RV_OK)
        {
            seliEnd();
        }
    }
    if (status != RV_OK)
    {
        cleanOnFailure(app, initLog, "Unable to start seli()", status);
        return status;
    }

    RvLockConstruct(app->logMgr, &app->appLock);
    RvMutexConstruct(app->logMgr, &app->applicationLock);

    /* Create a random number generator */
    RvRandomGeneratorConstruct(&app->randomGenerator,
        (RvRandom)RvInt64ToRvUint32(RvInt64ShiftRight(RvTimestampGet(app->logMgr), 16)));

    /* Create a GUID generator */
    app->guidGenerator = RvH323UniqueIdGeneratorConstruct(&app->randomGenerator, app->logMgr);

    /* Make sure to notify about the version of the stack... */
    cmiAPIEnter(((HAPP)app, "cmInititialize(name=%s)", nprn(name)));
    RvLogDebug(&app->logAPI, (&app->logAPI,
        "Conference Manager: Version " RV_H323_STACK_VERSION));
    RvLogSourcePrintInterfacesData(&app->logAPI, RvCCoreInterfaces());

    /* Start with the configuration */
    if ( (app->hCfg=ciConstruct(name)) == NULL)
    {
        RvChar errStr[200];
        RvSprintf(errStr, "cmInitialize: Configuration construction error (%.100s).", nprn(name));
        errStr[199] = '\0';
        cleanOnFailure(app, initConfig, errStr, -11);
        return -11;
    }

    /*allocate an instance for the watchdog*/
    {
        /* Changing the watchdog size is used by the GK toolkit for its own set of resources */
        RvInt32 size = 21;
        ciGetValue(app->hCfg, "system.watchdog.size", NULL, &size);
        status = RvWatchdogAllocate(&app->watchdog, size, app->logMgr);
        if (status < 0)
        {
            cleanOnFailure(app, initWatchdog, NULL, -10);
            return -10;
        }
    }

    /* Initialize the MIB */
    app->hStatistic = mibCreateStatistic();
    app->maxListeningAddresses = 1;

    status = readSystemParameters(app, (int*)&maxCalls, &maxChannels, &extraConnections,
        &vtNodeCount, &maxBuffSize, &maxPoolSizeInKB, &poolElemSize, &app->maxListeningAddresses);
    if ( (maxChannels < 0) || (maxCalls <= 0) || (maxBuffSize < 0) || (maxPoolSizeInKB < 0) || (status != RV_OK))
    {
        cleanOnFailure(app, initSystemparams, "cmInitialize()=Error reading parameters", RV_ERROR_UNKNOWN);
        return RV_ERROR_UNKNOWN;
    }
    app->encodeBufferSize=maxBuffSize;

    /*
        The following simultaneous timers are possible per call

        1.call.timer
        2.call.q931.timer
        3.call.q931.timerSE
        4.call.control.outcap.timer
        5.call.control.msd.timer
        6.call.control.out_RM.timer
        7.call.control.rtd.timer

        1.call.control.channel[].timer
        2.call.control.channel[].ml_timer
        3.call.control.channel[].rc_timer
    */
    app->hTimers = RvH323TimerConstruct((int)(maxCalls * (7 + maxChannels * 3)), app->logMgr, &timersQueue);
    if (app->hTimers == NULL)
    {
        cleanOnFailure(app, initTimers, "cmInitialize()=Error allocating timers", -13);
        return -13;
    }

    /* Initialize ASN related modules (coder, PVT, PST) */
    cmEmInstall(maxBuffSize);
    status = initPVT_PST(app, vtNodeCount);
    if (status < 0)
    {
        cleanOnFailure(app, initPvtPst, "cmInitialize()=Error Constructing PVT/PST", -13);
        return -13;
    }

    status = configTreesInit(app);
    if (status < 0)
    {
        cleanOnFailure(app, initConfigTrees, "cmInitialize()=Error Constructing Configuration trees", -13);
        return -13;
    }

    /* Update manual RAS here, as it affects the allocation size of call elements */
    app->manualRAS = RV_PVT_NODEID_IS_VALID(pvtGetChild(app->hVal, app->rasConf, __q931(manualRAS), NULL));

    /*set NAT mode*/
    app->cmNATMode = cmNatModeNoNAT;

    if (ciGetValue(app->hCfg, "system.tempSystemParams.natMode.natModeNAT", NULL, NULL)== ERR_CI_NOERROR)
        app->cmNATMode = cmNatModeNAT;
    else if (ciGetValue(app->hCfg, "system.tempSystemParams.natMode.natModeOnlyNAT", NULL, NULL) == ERR_CI_NOERROR)
        app->cmNATMode = cmNatModeOnlyNAT;

    /*set gatekeeperInNat*/
    if (app->cmNATMode == cmNatModeNoNAT)
        app->gatekeeperInNat = RV_TRUE;
    else if (app->cmNATMode == cmNatModeOnlyNAT)
        app->gatekeeperInNat = RV_FALSE;
    else
    {
        if (ciGetValue(app->hCfg, "system.tempSystemParams.gkInNat", NULL, NULL) == ERR_CI_NOERROR)
            app->gatekeeperInNat = RV_TRUE;
    }

    /* Initialize calls and channels objects */
    app->hCalls = cmiInitCalls((HAPP)app, (int)maxCalls);
    if (!app->hCalls)
    {
        cleanOnFailure(app, initCalls, "cmInitialize()=Error Initializing calls and channels objects", -13);
        return -13;
    }
    emaSetIdleEvent(app->hCalls, emaEvCallElementIdle);

    /* Make sure we allocated encode/decode buffers */
    encodeDecodeBufferInit();

    /* Initialize transport */
    useAnnexE =
        ((pvtGetChild(app->hVal, app->q931Conf, __q931(useAnnexE), NULL))>=0);
    if (!(app->hTransport= cmTransInit((HAPPATRANS)app, app->hVal, app->selectEngine,
        (int)maxCalls, extraConnections, maxPoolSizeInKB, poolElemSize, maxBuffSize, useAnnexE,
        app->maxListeningAddresses, app->portRange, app->logMgr)))
    {
        cleanOnFailure(app, initTransport, "cmInitialize()=cmTransInit failed", RV_ERROR_UNKNOWN);
        return RV_ERROR_UNKNOWN;
    };

    cmTransSetSessionEventHandler(app->hTransport, &tse, sizeof(TRANSSESSIONEVENTS));
    cmTransSetHostEventHandler(app->hTransport, &the, sizeof(TRANSHOSTEVENTS));

    /* Create CAT (Call Association Table) */
    if ((app->hCat = catConstruct(maxCalls,
                    (RvBool)(ciGetValue(app->hCfg, "RAS.gatekeeper" , NULL, NULL) == ERR_CI_NOERROR),
                    (RvBool)(ciGetValue(app->hCfg, "RAS.compare15bitRasCrv" , NULL, NULL) == ERR_CI_NOERROR),
                    (RvBool)(app->cidAssociate!= RV_FALSE), app->logMgr)) == NULL )
    {
        cleanOnFailure(app, initCat, "cmInitialize()=catConstruct failed", RV_ERROR_OUTOFRESOURCES);
        return RV_ERROR_OUTOFRESOURCES;
    }

     /*init dns module*/
    if ((app->dnsManager = dnsConstruct((HAPP)app)) != NULL)
    {
        status = cmDnsStart(app, RV_FALSE);
        if (status < 0)
        {
            cleanOnFailure(app, initDns, "cmInitialize()=cmDnsStart failed", status);
            return status;
        }
    }

	/* disableRAS */
	if ((RvBool)(ciGetValue(app->hCfg, "RAS.disableRAS" , NULL, NULL) != ERR_CI_NOERROR))
	{
		/* Initialize the RAS module */
		if ((status = rasInit(app)) < 0)
		{
			cleanOnFailure(app, initRAS, "cmInitialize()=rasInit failed", status);
			return status;
		}
	}
	
    if (ciGetValue(app->hCfg, "system.printMemory", NULL, NULL) == ERR_CI_NOERROR)
    {
        /* This piece of code allows us to use a performance test program with the release
           mode compilation that will display on the standard output the amount of memory
           allocated. */
        RvMemoryInfo memInfo;

        if (RvMemoryGetInfo(NULL, app->logMgr, &memInfo) == RV_OK)
        {
            RvPrintf("Total stack allocation: %d bytes in %d blocks\n",
                memInfo.bytes_requested_total, memInfo.allocs_requested);

        }
    }

    /* Set watchdog resources for the CM */
    status = emaAddWatchdogResource(app->hCalls, &app->watchdog, "CmCalls", "CM", "Call elements");
    if (status >= 0)
        status = RvWatchdogAddResource(cmiGetWatchdogHandle((HAPP)app), "CmValTree", "CM", "PVT nodes",
            (RvWatchdogResourceCallback_T)cmWatchdogResourceCallback,
            NULL, cmWatchdogResourcePrintCallback, (void*) app, &app->cmValTreeResourceVal);
    if (status >= 0)
        status = RvWatchdogAddResource(cmiGetWatchdogHandle((HAPP)app), "CmTimers", "CM", "Call Manager timers",
            (RvWatchdogResourceCallback_T)cmWatchdogResourceCallback, NULL, NULL,
            (void*) app, &app->cmTimersResourceVal);

    if (status >= 0)
    {
        /* Initialize the watchdog */
        status = RvWatchdogInit(&app->watchdog);
    }

    if (status < 0)
    {
        cleanOnFailure(app, initWatchdogResources, "cmInitialize()=RvWatchdogInit failed", -10);
        return -10;
    }

    /* Set an interval timer for the watchdog */
    if (ciGetValue(app->hCfg, "system.watchdog.interval", NULL, &watchdogInterval) == ERR_CI_NOERROR)
    {
        if (watchdogInterval > 0)
        {
            RvTimerQueue *tqueue;
            if ((RvSelectGetTimeoutInfo(app->selectEngine, NULL, &tqueue) == RV_OK) && (tqueue != NULL))
            {
                app->watchdogTimerUsed =
                    (RvTimerStart(&app->watchdogTimer, tqueue, RV_TIMER_TYPE_PERIODIC,
                        RvInt64Mul(RvInt64FromRvInt(watchdogInterval), RV_TIME64_NSECPERSEC), rvCmWakeWatchdog, app) == RV_OK);
            }
        }
    }

    memset(&h245conf, 0, sizeof(h245conf));
    h245conf.evHandlers = h245eh;
    h245conf.maxCalls = maxCalls;
    h245conf.maxChannels = maxChannels;
    h245conf.h225CtrlOffset = sizeof(callElem) + sizeof(q931Elem);
    h245conf.h223CtrlOffset = 0;
    h245conf.logMgr = app->logMgr;
    h245conf.hVal = app->hVal;
    h245conf.bIsPropertyModeNotUsed = (app->callPropertyMode == pmodeDoNotUseProperty);
    h245conf.hSynProtH245 = app->synProtH245;
    h245conf.hAddrSynH245 = app->hAddrSynH245;
    h245conf.h245Conf = app->h245Conf;
    h245conf.synOLC = app->synOLC;
    h245conf.h245TransCap = app->h245TransCap;
    h245conf.h245RedEnc = app->h245RedEnc;
    h245conf.h245DataType = app->h245DataType;
    h245conf.mibEvent = app->mibEvent;
    h245conf.mibHandle = app->mibHandle;
    h245conf.encodeBufferSize = app->encodeBufferSize;
    h245conf.watchdog = &app->watchdog;

    h245conf.bSupportEFC =
        (ciGetValue(app->hCfg, "system.supportEFC", NULL, NULL) == ERR_CI_NOERROR);


    if ((status=H245Init(&h245conf, (HAPP) app, &app->hh245)) != RV_OK)
    {
        cleanOnFailure(app, initH245, "cmInitialize()=H245Init failed", status);
        return status;
    }

    proposed=maxChannels*10;
    accepted=maxChannels;
    if (!fastStartInit(app->hh245, (int)maxCalls, proposed, accepted))
    {
        cleanOnFailure(app, initFaststart, "fastStartInit()=liInit Failed", -10);
        return -10;
    }

    app->appDB = RV_PVT_INVALID_NODEID;
    app->q931TpktAddresses = RV_PVT_INVALID_NODEID;
    app->q931AnnexEAddresses = RV_PVT_INVALID_NODEID;
    app->natq931TpktAddresses = RV_PVT_INVALID_NODEID;
    app->natq931AnnexEAddresses = RV_PVT_INVALID_NODEID;

    /* Make sure to print the first watchdog resources now */
    RvLogDebug(&app->logWatchdog, (&app->logWatchdog,
        "Resources before cmStart():"));
    RvWatchdogPrint(&app->watchdog, &app->logWatchdog);

    /* Start running the stack if we're supposed to */
    if (name && (ciGetValue(app->hCfg, "system.manualStart", NULL, NULL) != ERR_CI_NOERROR))
        if (cmStart((HAPP)app)<0)
        {
            cleanOnFailure(app, initStartCM, "cmInitialize()=cmStart Failed", RV_ERROR_UNKNOWN);
            return RV_ERROR_UNKNOWN;
        }

    *lphApp=(HAPP)app;

    /* We're done... */
    cmiAPIExit(((HAPP)app, "cmInitialize(*lphApp=%p)=0", app));
    RvMutexUnlock(&rvCmGlobalMutex, app->logMgr);
    return 0;
}

/************************************************************************
 * cmInit
 * purpose: Initializes the Conference Manager instance.
 *          This function must be called before any other H.323 function
 *          except cmGetVersion().
 * input  : name    - Configuration file name to use
 * output : none
 * return : Stack's application handle on success
 *          NULL on failure
 ************************************************************************/
RVAPI
HAPP RVCALLCONV cmInit(IN const RvChar * name)
{
    HAPP hApp;

    if (cmInitialize(name, &hApp) >= 0)
        return hApp;
    else
        return NULL;
}

/************************************************************************
 * cmStart
 * purpose: Starts the stack's activity
 *
 *          This function is only applicable when system.manualStart key
 *          is defined in the configuration. In manualStart mode cmInitialize()
 *          function does not automatically start Stack activity and accesses only
 *          the "system" configuration tree. To start the Stack use cmStart() function.
 *          The application may change configuration settings between cmInitialize()
 *          and cmStart() using cmGetXXXConfigurationHandle() functions.
 *
 * input  : hApp    - Stack handle for the application
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmStart(
        IN  HAPP        hApp)
{
    int rv=RV_ERROR_UNKNOWN;
    cmElem* app=(cmElem*)hApp;
    if (!hApp) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmStart(hApp=%p)", app));

    /* Make sure we haven't started yet */
    if (!app->start)
    {
        RvPvtNodeId sysConf;
        CMTRANSANNEXEPARAM eParams;
        cmTransportAddress tmpAddr;
        RvPvtNodeId tmpNodeId;

        /* Load the system configuration into a PVT tree */
        sysConf = pvtAddRoot(app->hVal, app->synConfCommon, 0, NULL);
        pvtLoadFromConfig(app->hCfg, app->hVal, "system", sysConf, RV_FALSE, NULL);

        /* Check for the length checking option */
        app->bCheckParamLength = RV_PVT_NODEID_IS_VALID(pvtGetChild(app->hVal, sysConf, __common(checkParamLength), NULL));


        /* --- Activate the transport module --- */

        app->diffSrcAddressInSetupAndARQ = RV_FALSE;
        if (pvtGetChild(app->hVal, app->q931Conf, __q931(diffSrcAddressInSetupAndARQ), NULL) >= 0)
            app->diffSrcAddressInSetupAndARQ = RV_TRUE;

        /* -- Find out the default local address to use */
        {
            RvUint32 ip = 0;
            RvPvtNodeId locAddrNode;

            /* local IP address (for backward compatability of v4) */
            if (RV_PVT_NODEID_IS_VALID(locAddrNode = pvtGetNodeIdByPath(app->hVal, sysConf, "localIPAddress")))
            {
                RvAddress locAddr;
                pvtGetString(app->hVal, locAddrNode, sizeof(ip), (char*)&ip);
                RvAddressConstructIpv4(&locAddr, ip, 0);
                RvH323CoreToCmAddress(&locAddr, &app->localAddress);
                RvAddressDestruct(&locAddr);
            }

            /* local IP address (IPv6 compatible) */
            if (RV_PVT_NODEID_IS_VALID(locAddrNode = pvtGetNodeIdByPath(app->hVal, sysConf, "localAddress")))
            {
                cmVtToTA(app->hVal, locAddrNode, &app->localAddress);
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
                if (app->localAddress.type == cmTransportTypeIPv6)
                {
                    RvInt32 scId;
                    if (pvtGetByPath(app->hVal, sysConf, "defaultScopeId", NULL, &scId, NULL) >= 0)
                    {
                        app->localAddress.addr.v6.scopeId = (RvInt16)scId;
                    }
                    if (!cmTaHasIp(&app->localAddress))
                    {
                        cmTransGetGoodAddress(app->hTransport, &app->localAddress);
                    }
                }
#endif /*RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY*/
            }
        }

        /* No need for the system's configuration any more */
        pvtDelete(app->hVal, sysConf);

        /* -- Deal with TPKT addresses */
        pvtDelete(app->hVal, app->q931TpktAddresses);
        pvtDelete(app->hVal, app->natq931TpktAddresses);

        app->q931TpktAddresses = pvtAddRootByPath(app->hVal, app->synConfQ931, "multipleCallSignalingAddresses", 0, NULL);
        tmpNodeId = pvtGetChild(app->hVal, app->q931Conf, __q931(multipleCallSignalingAddresses), NULL);
        if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
        {
            /* We support multi-homed. Get the addresses from the configuration and remember them. */
            pvtSetTree(app->hVal, app->q931TpktAddresses, app->hVal, tmpNodeId);
        }
        else
        {
            /* We don't have multi-homed IP addresses - check for a single address */
            RvInt32 port = 1720;
            pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(callSignalingPort), &port, NULL);
            memcpy(&tmpAddr, &app->localAddress, sizeof(tmpAddr));
            tmpAddr.port = (RvUint16)port;

            tmpNodeId = pvtAdd(app->hVal, app->q931TpktAddresses, __anyField, 0, NULL, NULL);
            cmLTAToVt(app->hVal, tmpNodeId, &tmpAddr);
        }

        /* -- Deal with Annex E parameters */
        tmpNodeId = pvtGetChild(app->hVal, app->q931Conf, __q931(useAnnexE), NULL);
        eParams.useAnnexE = RV_PVT_NODEID_IS_VALID(tmpNodeId);
        eParams.t_R1    = 500;
        eParams.n_R1    = 8;
        eParams.t_IMA1  = 6000;
        eParams.n_IMA1  = 6;
        eParams.t_DT    = 1;
        if (eParams.useAnnexE == RV_TRUE)
        {
            pvtGetChildByFieldId(app->hVal, tmpNodeId, __q931(t_R1) , (int *)&eParams.t_R1, NULL);
            pvtGetChildByFieldId(app->hVal, tmpNodeId, __q931(n_R1) , (int *)&eParams.n_R1, NULL);
            pvtGetChildByFieldId(app->hVal, tmpNodeId, __q931(t_IMA1) , (int *)&eParams.t_IMA1, NULL);
            pvtGetChildByFieldId(app->hVal, tmpNodeId, __q931(n_IMA1) , (int *)&eParams.n_IMA1, NULL);
            pvtGetChildByFieldId(app->hVal, tmpNodeId, __q931(t_DT) , (int *)&eParams.t_DT, NULL);
            eParams.t_R2    = (RvUint)(eParams.t_R1 * 21 / 10);
        }

        /* -- Deal with Annex E addresses */
        pvtDelete(app->hVal, app->q931AnnexEAddresses);
        pvtDelete(app->hVal, app->natq931AnnexEAddresses);

        if (eParams.useAnnexE == RV_TRUE)
        {
            app->q931AnnexEAddresses = pvtAddRootByPath(app->hVal, app->synConfRAS, "alternateTransportAddresses.annexE", 0, NULL);
            __pvtGetNodeIdByFieldIds(tmpNodeId, app->hVal, app->rasConf, {_q931(alternateTransportAddresses) _q931(annexE) LAST_TOKEN});
            if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
            {
                /* We support multi-homed. Get the addresses from the configuration and remember them. */
                pvtSetTree(app->hVal, app->q931AnnexEAddresses, app->hVal, tmpNodeId);
            }
            else
            {
                /* Single static address */
                memcpy(&tmpAddr, &app->localAddress, sizeof(tmpAddr));
                tmpAddr.distribution = cmDistributionUnicast;
                tmpAddr.port = 2517;

                tmpNodeId = pvtAdd(app->hVal, app->q931AnnexEAddresses, __anyField, 0, NULL, NULL);
                cmTAToVt(app->hVal, tmpNodeId, &tmpAddr);
            }
        }

        if (cmTransStart(app->hTransport, app->q931TpktAddresses, app->q931AnnexEAddresses, &app->localAddress, &eParams)==cmTransOK)
        {
            rv=0;

            /*if NAT required  - build NAT trees*/
            if (app->cmNATMode != cmNatModeNoNAT)/*if NAT trees needed*/
            {
                getNatAddresses(app, eParams.useAnnexE);
            }

            /* Create a database of default values to messages */
            app->appDB = pvtAddRoot(app->hVal, app->hSyn, 0, NULL);

            {
                RvPvtNodeId tmpNodeId1;
                RvPvtNodeId msgNodeId;
                char OID[10];
                int length;

                length=utlEncodeOID(sizeof(OID), OID, H225_PROTOCOL_IDENTIFIER);

                /*Setup*/
                tmpNodeId=pvtAdd(app->hVal, app->appDB, __q931(setup), 0, NULL, NULL);

                pvtAdd(app->hVal, tmpNodeId, __q931(protocolDiscriminator), 8, NULL, NULL);

                __pvtBuildByFieldIds(msgNodeId, app->hVal, tmpNodeId,
                    {_q931(message) _q931(setup) LAST_TOKEN}, 0, NULL);

                tmpNodeId=pvtAdd(app->hVal, msgNodeId, __q931(bearerCapability), 0, NULL, NULL);
                tmpNodeId1=pvtAdd(app->hVal, tmpNodeId, __q931(octet3), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(codingStandard), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(informationTransferCapability), 8, NULL, NULL);

                tmpNodeId1=pvtAdd(app->hVal, tmpNodeId, __q931(octet4), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(transferMode), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(informationTransferRate), 16, NULL, NULL);

                tmpNodeId1=pvtAdd(app->hVal, tmpNodeId, __q931(octet5), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(layer1Ident), 1, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(userInformationLayer1Protocol), 5, NULL, NULL);

                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(userUser), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolDiscriminator), 5, NULL, NULL);

                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(setup) LAST_TOKEN}, 0, NULL);

                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolIdentifier), length, OID, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(activeMC), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(mediaWaitForConnect), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(multipleCalls), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(maintainConnection), 0, NULL, NULL);
                __pvtBuildByFieldIds(tmpNodeId, app->hVal, tmpNodeId1,
                    {_q931(conferenceGoal) _q931(create) LAST_TOKEN}, 0, NULL);
                __pvtBuildByFieldIds(tmpNodeId, app->hVal, tmpNodeId1,
                    {_q931(callType) _q931(pointToPoint) LAST_TOKEN}, 0, NULL);
                __pvtBuildByFieldIds(tmpNodeId, app->hVal, tmpNodeId1,
                    {_q931(screeningIndicator) _q931(userProvidedVerifiedAndFailed) LAST_TOKEN}, 0, NULL);
                __pvtBuildByFieldIds(tmpNodeId, app->hVal, tmpNodeId1,
                    {_q931(presentationIndicator) _q931(presentationAllowed) LAST_TOKEN}, 0, NULL);

                /*CallProceeding*/
                tmpNodeId=pvtAdd(app->hVal, app->appDB, __q931(callProceeding), 0, NULL, NULL);

                pvtAdd(app->hVal, tmpNodeId, __q931(protocolDiscriminator), 8, NULL, NULL);

                __pvtBuildByFieldIds(msgNodeId, app->hVal, tmpNodeId,
                    {_q931(message) _q931(callProceeding) LAST_TOKEN}, 0, NULL);

                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(userUser), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolDiscriminator), 5, NULL, NULL);

                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(callProceeding) LAST_TOKEN}, 0, NULL);

                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolIdentifier), length, OID, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(multipleCalls), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(maintainConnection), 0, NULL, NULL);


                /*Alerting*/
                tmpNodeId=pvtAdd(app->hVal, app->appDB, __q931(alerting), 0, NULL, NULL);

                pvtAdd(app->hVal, tmpNodeId, __q931(protocolDiscriminator), 8, NULL, NULL);

                __pvtBuildByFieldIds(msgNodeId, app->hVal, tmpNodeId,
                    {_q931(message) _q931(alerting) LAST_TOKEN}, 0, NULL);

                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(userUser), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolDiscriminator), 5, NULL, NULL);

                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(alerting) LAST_TOKEN}, 0, NULL);

                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolIdentifier), length, OID, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(multipleCalls), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(maintainConnection), 0, NULL, NULL);
                __pvtBuildByFieldIds(tmpNodeId, app->hVal, tmpNodeId1,
                    {_q931(presentationIndicator) _q931(presentationAllowed) LAST_TOKEN}, 0, NULL);
                __pvtBuildByFieldIds(tmpNodeId, app->hVal, tmpNodeId1,
                    {_q931(screeningIndicator) _q931(userProvidedVerifiedAndFailed) LAST_TOKEN}, 0, NULL);

                /*Connect*/
                tmpNodeId=pvtAdd(app->hVal, app->appDB, __q931(connect), 0, NULL, NULL);

                pvtAdd(app->hVal, tmpNodeId, __q931(protocolDiscriminator), 8, NULL, NULL);

                __pvtBuildByFieldIds(msgNodeId, app->hVal, tmpNodeId,
                    {_q931(message) _q931(connect) LAST_TOKEN}, 0, NULL);

                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(userUser), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolDiscriminator), 5, NULL, NULL);

                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(connect) LAST_TOKEN}, 0, NULL);

                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolIdentifier), length, OID, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(multipleCalls), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(maintainConnection), 0, NULL, NULL);
                __pvtBuildByFieldIds(tmpNodeId, app->hVal, tmpNodeId1,
                    {_q931(presentationIndicator) _q931(presentationAllowed) LAST_TOKEN}, 0, NULL);
                __pvtBuildByFieldIds(tmpNodeId, app->hVal, tmpNodeId1,
                    {_q931(screeningIndicator) _q931(userProvidedVerifiedAndFailed) LAST_TOKEN}, 0, NULL);

                /*Release Complete*/
                tmpNodeId=pvtAdd(app->hVal, app->appDB, __q931(releaseComplete), 0, NULL, NULL);

                pvtAdd(app->hVal, tmpNodeId, __q931(protocolDiscriminator), 8, NULL, NULL);

                __pvtBuildByFieldIds(msgNodeId, app->hVal, tmpNodeId,
                    {_q931(message) _q931(releaseComplete) LAST_TOKEN}, 0, NULL);

                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(userUser), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolDiscriminator), 5, NULL, NULL);

                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_UserInformation) _q931(h323_uu_pdu) LAST_TOKEN}, 0, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(h245Tunneling), 0, NULL, NULL);
                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_message_body) _q931(releaseComplete) LAST_TOKEN}, 0, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolIdentifier), length, OID, NULL);
                __pvtBuildByFieldIds(tmpNodeId, app->hVal, tmpNodeId1,
                    {_q931(presentationIndicator) _q931(presentationAllowed) LAST_TOKEN}, 0, NULL);
                __pvtBuildByFieldIds(tmpNodeId, app->hVal, tmpNodeId1,
                    {_q931(screeningIndicator) _q931(userProvidedVerifiedAndFailed) LAST_TOKEN}, 0, NULL);

                /* Facility */
                tmpNodeId=pvtAdd(app->hVal, app->appDB, __q931(facility), 0, NULL, NULL);

                pvtAdd(app->hVal, tmpNodeId, __q931(protocolDiscriminator), 8, NULL, NULL);

                __pvtBuildByFieldIds(msgNodeId, app->hVal, tmpNodeId,
                    {_q931(message) _q931(facility) LAST_TOKEN}, 0, NULL);

                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(facility), 0, NULL, NULL);
                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(userUser), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolDiscriminator), 5, NULL, NULL);

                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(facility) LAST_TOKEN}, 0, NULL);

                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolIdentifier), length, OID, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(multipleCalls), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(maintainConnection), 0, NULL, NULL);
                pvtAddBranch2(app->hVal, tmpNodeId1, __q931(reason), __q931(undefinedReason));

                /* Status Enquiry */
                tmpNodeId=pvtAdd(app->hVal, app->appDB, __q931(statusEnquiry), 0, NULL, NULL);

                pvtAdd(app->hVal, tmpNodeId, __q931(protocolDiscriminator), 8, NULL, NULL);

                __pvtBuildByFieldIds(msgNodeId, app->hVal, tmpNodeId,
                    {_q931(message) _q931(statusEnquiry) LAST_TOKEN}, 0, NULL);

                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(userUser), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolDiscriminator), 5, NULL, NULL);

                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(statusInquiry) LAST_TOKEN}, 0, NULL);

                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolIdentifier), length, OID, NULL);

                /* Status */
                tmpNodeId=pvtAdd(app->hVal, app->appDB, __q931(status), 0, NULL, NULL);

                pvtAdd(app->hVal, tmpNodeId, __q931(protocolDiscriminator), 8, NULL, NULL);

                __pvtBuildByFieldIds(msgNodeId, app->hVal, tmpNodeId,
                    {_q931(message) _q931(status) LAST_TOKEN}, 0, NULL);

                tmpNodeId=pvtAdd(app->hVal, msgNodeId, __q931(cause), 0, NULL, NULL);
                tmpNodeId1=pvtAdd(app->hVal, tmpNodeId, __q931(octet3), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(codingStandard), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(spare), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(location), 0, NULL, NULL);
                tmpNodeId1=pvtAdd(app->hVal, tmpNodeId, __q931(octet4), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(causeValue), 0, NULL, NULL);

                tmpNodeId=pvtAdd(app->hVal, msgNodeId, __q931(callState), 0, NULL, NULL);
                tmpNodeId1=pvtAdd(app->hVal, tmpNodeId, __q931(codingStandard), 0, NULL, NULL);

                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(userUser), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolDiscriminator), 5, NULL, NULL);

                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(status) LAST_TOKEN}, 0, NULL);

                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolIdentifier), length, OID, NULL);

                /* Information */
                tmpNodeId=pvtAdd(app->hVal, app->appDB, __q931(information), 0, NULL, NULL);

                pvtAdd(app->hVal, tmpNodeId, __q931(protocolDiscriminator), 8, NULL, NULL);

                __pvtBuildByFieldIds(msgNodeId, app->hVal, tmpNodeId,
                    {_q931(message) _q931(information) LAST_TOKEN}, 0, NULL);

                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(userUser), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolDiscriminator), 5, NULL, NULL);

                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(information) LAST_TOKEN}, 0, NULL);

                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolIdentifier), length, OID, NULL);

                /* Setup Acknowledge */
                tmpNodeId=pvtAdd(app->hVal, app->appDB, __q931(setupAck), 0, NULL, NULL);

                pvtAdd(app->hVal, tmpNodeId, __q931(protocolDiscriminator), 8, NULL, NULL);

                __pvtBuildByFieldIds(msgNodeId, app->hVal, tmpNodeId,
                    {_q931(message) _q931(setupAck) LAST_TOKEN}, 0, NULL);

                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(userUser), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolDiscriminator), 5, NULL, NULL);

                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(setupAcknowledge) LAST_TOKEN}, 0, NULL);

                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolIdentifier), length, OID, NULL);

                /* Progress */
                tmpNodeId=pvtAdd(app->hVal, app->appDB, __q931(progress), 0, NULL, NULL);

                pvtAdd(app->hVal, tmpNodeId, __q931(protocolDiscriminator), 8, NULL, NULL);

                __pvtBuildByFieldIds(msgNodeId, app->hVal, tmpNodeId,
                    {_q931(message) _q931(progress) LAST_TOKEN}, 0, NULL);

                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(userUser), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolDiscriminator), 5, NULL, NULL);

                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(progress) LAST_TOKEN}, 0, NULL);

                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolIdentifier), length, OID, NULL);

                /* Notify */
                tmpNodeId=pvtAdd(app->hVal, app->appDB, __q931(notify), 0, NULL, NULL);

                pvtAdd(app->hVal, tmpNodeId, __q931(protocolDiscriminator), 8, NULL, NULL);

                __pvtBuildByFieldIds(msgNodeId, app->hVal, tmpNodeId,
                    {_q931(message) _q931(notify) LAST_TOKEN}, 0, NULL);

                tmpNodeId1=pvtAdd(app->hVal, msgNodeId, __q931(userUser), 0, NULL, NULL);
                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolDiscriminator), 5, NULL, NULL);

                __pvtBuildByFieldIds(tmpNodeId1, app->hVal, tmpNodeId1,
                    {_q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(notify) LAST_TOKEN}, 0, NULL);

                pvtAdd(app->hVal, tmpNodeId1, __q931(protocolIdentifier), length, OID, NULL);
                /*decide which trees to call with cmiRASUpdateCallSignalingAddress*/
                if (app->cmNATMode == cmNatModeNoNAT) /*no nat addresses required*/
                    cmiRASUpdateCallSignalingAddress(app->rasManager, app->q931TpktAddresses, app->q931AnnexEAddresses);
                else
                    cmiRASUpdateCallSignalingAddress(app->rasManager, app->natq931TpktAddresses, app->natq931AnnexEAddresses);
            }


            /* Set the intial CRV of the application to a random value */
            {
                RvRandom randomValue;
                cmLock(hApp);
                RvRandomGeneratorGetValue(&app->randomGenerator, &randomValue);
                app->crv = (int)(randomValue % 32767);
                cmUnlock(hApp);
            }

            /* Initialize RAS */
//h.e
			if (app->rasManager != 0)
//===
            rv = rasStart(app, RV_FALSE);

            if (rv==RV_OK)
            {
                app->postControlDisconnectionDelay=2000;
                pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(postControlDisconnectionDelay), &app->postControlDisconnectionDelay, NULL);

                /* Initialize H245 */
                H245Start(app->hh245);

                /* We're done */
                app->start=RV_TRUE;
            }
            else
                cmTransStop(app->hTransport);
        }
    }
    else
    {
        /* stack already started, but don't return an error */
        rv = RV_OK;
    }
    cmiAPIExit((hApp, "cmStart=%d", rv));
    return rv;
}


/************************************************************************
 * cmStop
 * purpose: Stops the stack's activity
 *
 *          After the Stack is stopped by cmStop(), the application may change
 *          configuration settings and then use cmStart() to start Stack
 *          activity again.
 *
 * input  : hApp    - Stack handle for the application
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmStop(
        IN  HAPP        hApp)
{
    cmElem* app=(cmElem*)hApp;
    if (!app)
        return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmStop(hApp=%p)", app));

    if (app->start)
    {
        /* Stop our internal modules */
        cmDnsClear(app);

		/* disableRAS */
		if (app->rasManager != NULL)
			rasStop(app, RV_FALSE);

        H245Stop(app->hh245);
        cmTransStop(app->hTransport);

        /* Indicate that we stopped */
        app->start = RV_FALSE;
    }

    /* Make sure we don't leave any un-needed PVT nodes */
    pvtDelete(app->hVal, app->appDB);
    app->appDB = RV_PVT_INVALID_NODEID;
    pvtDelete(app->hVal, app->q931TpktAddresses);
    app->q931TpktAddresses = RV_PVT_INVALID_NODEID;
    pvtDelete(app->hVal, app->q931AnnexEAddresses);
    app->q931AnnexEAddresses = RV_PVT_INVALID_NODEID;

    cmiAPIExit((hApp, "cmStop=0"));
    return 0;
}

/************************************************************************
 * cmEnd
 * purpose: Shuts down the Conference Manager instance and releases all
 *          resources that were in use by the Conference Manager instance.
 * input  : hApp    - Stack handle for the application
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 * notes  : cmEnd() must be called from the same thread that called cmInitialize().
 ************************************************************************/
RVAPI
int RVCALLCONV cmEnd(
        IN   HAPP        hApp)
{
    cmElem* app=(cmElem*)hApp;
    RvMutexLock(&rvCmGlobalMutex, NULL);

    if (app)
    {
#if defined(RV_DEBUG)
        if (app->pThreadInfo != RvThreadCurrent())
        {
            RvLogError(&app->logWatchdog, (&app->logWatchdog,
                "cmEnd() called from the wrong thread!"));
        }
#endif

        /* Stop and close the watchdog */
        if (app->watchdogTimerUsed)
            RvTimerCancel(&app->watchdogTimer, RV_TIMER_CANCEL_WAIT_FOR_CB);

        cmStop(hApp);

        /* Make sure to print the last watchdog resources now, after the CM is stopped */
        RvLogDebug(&app->logWatchdog, (&app->logWatchdog,
            "Resources at beginning of cmEnd():"));
        RvWatchdogPrint(&app->watchdog, &app->logWatchdog);

        fastStartEnd(app->hh245);
        H245End(app->hh245);
        cmEmEnd();
        rasEnd(app);
        cmDnsDestruct(hApp);
        cmTransEnd(app->hTransport);
        cmiEndCalls(app->hCalls);
        RvH323TimerDestruct(app->hTimers);
        configTreesEnd(app);
        ciDestruct(app->hCfg);
        catDestruct(app->hCat);
        endPVT_PST(app);
        if (app->portRange != NULL)
        {
            RvPortRangeDestruct(app->portRange);
            RvMemoryFree(app->portRange, app->logMgr);
        }
        mibDestroyStatistic(app->hStatistic);
        RvH323UniqueIdGeneratorDestruct(app->guidGenerator, app->logMgr);
        RvRandomGeneratorDestruct(&app->randomGenerator);
        RvWatchdogEnd(&app->watchdog);

        seliEnd();
        RvLockDestruct(&app->appLock, app->logMgr);
        RvMutexDestruct(&app->applicationLock, app->logMgr);
        RvAddressDestruct(&app->localAddress);
    }

    encodeDecodeBufferEnd();
    cmiDestroyLog(app);

    if (app)
    {
        if (app->pThreadInfo == &app->threadInformation)
            RvThreadDestruct(app->pThreadInfo);
        RvMemoryFree(app, NULL);
    }

    RvMutexUnlock(&rvCmGlobalMutex, NULL);

    cmShutdown();

    return RV_OK;
}


/************************************************************************
 * cmGetVersion
 * purpose: Returns the version of the Conference Manager in use.
 * input  : none
 * output : none
 * return : Pointer to the string representing the version of the
 *          Conference Manager. For example, "3.0.0.0" or "2.5".
 ************************************************************************/
RVAPI
char* RVCALLCONV cmGetVersion(void)
{
    return (char*)RV_H323_STACK_VERSION;
}


/************************************************************************
 * cmGetVersionName
 * purpose: Returns the version of the Conference Manager in use.
 * input  : buff    - Buffer to set the version in
 *          length  - Maximum length of the buffer in bytes
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmGetVersionName(
        IN    char*   buff,
        IN    int     length)
{
    strncpy(buff, RV_H323_STACK_VERSION, (RvSize_t)length);
    return 0;
}


RVAPI
int RVCALLCONV cmSetGenEventHandler(
                              IN    HAPP    hApp,
                              IN    CMEVENT cmEvent,
                              IN    int     size)
{
    cmElem* app=(cmElem*)hApp;
    if (hApp == NULL) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmSetGenEventHandler(hApp=%p,cmEvent,size=%d)", hApp, size));
    memset(&app->cmMyEvent, 0, sizeof(app->cmMyEvent));
    memcpy(&app->cmMyEvent, cmEvent, (RvSize_t)RvMin((int)sizeof(app->cmMyEvent), size));
    cmiAPIExit((hApp, "cmSetGenEventHandler=0"));
    return 0;
}

RVAPI
int RVCALLCONV cmGetGenEventHandler(
                              IN    HAPP    hApp,
                              OUT   CMEVENT cmEvent,
                              IN    int     size)
{
    cmElem* app=(cmElem*)hApp;
    if (hApp == NULL) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmGetGenEventHandler(hApp=%p,cmEvent,size=%d)", hApp, size));
    memset(cmEvent, 0, (RvSize_t)size);
    memcpy(cmEvent, &app->cmMyEvent, (RvSize_t)RvMin((int)sizeof(app->cmMyEvent), size));
    cmiAPIExit((hApp, "cmGetGenEventHandler=0"));

    return 0;

}


RVAPI
int RVCALLCONV cmSetCallEventHandler(
                                   IN   HAPP        hApp,
                                   IN   CMCALLEVENT cmCallEvent,
                                   IN   int     size)
{
    cmElem* app=(cmElem*)hApp;
    H245SupportH225CallEvent h245supCallEv;
    if (hApp == NULL) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmSetCallEventHandler(hApp=%p,cmCallEvent,size=%d)", hApp, size));
    memset(&app->cmMyCallEvent, 0, sizeof(app->cmMyCallEvent));
    memcpy(&app->cmMyCallEvent, cmCallEvent, (RvSize_t)RvMin((int)sizeof(app->cmMyCallEvent), size));
    memset(&h245supCallEv, 0, sizeof(h245supCallEv));

    if ((int)offsetof(SCMCALLEVENT, cmEvCallFastStart) < size )
       h245supCallEv.cmEvCallFastStart = (cmH245EvCallFastStartT)cmCallEvent->cmEvCallFastStart;

    if ((int)offsetof(SCMCALLEVENT, cmEvCallFastStartSetup) < size)
        h245supCallEv.cmEvCallFastStartSetup = (cmH245EvCallFastStartSetupT)cmCallEvent->cmEvCallFastStartSetup;

    if ((int)offsetof(SCMCALLEVENT, cmEvExtFastConnectProposalCanceled) < size)
        h245supCallEv.cmH245EvExtFastConnectProposalCanceled = (cmH245EvExtFastConnectProposalCanceledT)cmCallEvent->cmEvExtFastConnectProposalCanceled;

    H245SetH225CallEventHandler(app->hh245, &h245supCallEv, sizeof(h245supCallEv));
    cmiAPIExit((hApp, "cmSetCallEventHandler=0"));
    return 0;
}



RVAPI
int RVCALLCONV cmGetCallEventHandler(
                                   IN      HAPP        hApp,
                                   OUT     CMCALLEVENT cmCallEvent,
                                   INOUT   int         size)
{
    cmElem* app=(cmElem*)hApp;
    if (hApp == NULL) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmGetCallEventHandler(hApp=%p,cmCallEvent,size=%d)", hApp, size));
    memset(cmCallEvent, 0, (RvSize_t)size);
    memcpy(cmCallEvent, &app->cmMyCallEvent, (RvSize_t)RvMin((int)sizeof(app->cmMyCallEvent), size));
    cmiAPIExit((hApp, "cmGetCallEventHandler=0"));

    return RV_OK;

}

RVAPI
RvInt32 RVCALLCONV cmGetSysTempParamsConfigurationHandle(
                                            IN  HAPP             hApp)
{
    cmElem *app = (cmElem *)hApp;
    RvPvtNodeId nodeId;

    if (!app) return RV_ERROR_UNKNOWN;
    cmiAPIEnter(((HAPP)app, "cmGetSysTempParamsConfigurationHandle: hApp=%p.", hApp));

    nodeId = pvtGetChild(app->hVal, app->sysConf, __common(tempSystemParams), NULL);

    cmiAPIExit(((HAPP)app, "cmGetSysTempParamsConfigurationHandle=%d", nodeId));
    return nodeId;
}

RVAPI
RvInt32 RVCALLCONV cmGetQ931ConfigurationHandle(
                                            IN  HAPP             hApp)
{
    cmElem *app = (cmElem *)hApp;

    if (!app) return RV_ERROR_UNKNOWN;
    cmiAPIEnter(((HAPP)app, "cmGetQ931ConfigurationHandle: hApp=%p.", hApp));

    cmiAPIExit(((HAPP)app, "cmGetQ931ConfigurationHandle=%d", app->q931Conf));
    return app->q931Conf;
}


int cmCallPreCallBack(HAPP hApp)
{
    cmElem* app=(cmElem*)hApp;
    if (!app || !app->newCallback) return RV_ERROR_UNKNOWN;
    return app->newCallback();
}


RVAPI int RVCALLCONV
cmSetPreCallbackEvent(
                      IN    HAPP                hApp,
                      IN    cmNewCallbackEH     newCallback
                      )
{
    cmElem* app=(cmElem*)hApp;
    if (!app) return RV_ERROR_UNKNOWN;

    cmiAPIEnter(((HAPP)app, "cmSetPreCallbackEvent: hApp=%p.", hApp));
    app->newCallback=newCallback;
    cmiAPIExit(((HAPP)app, "cmSetPreCallbackEvent=1"));

    return RV_TRUE;
}

RVAPI HPVT RVCALLCONV
cmGetValTree(
             IN  HAPP            hApp
             )
{
    HPVT hPvt;
    cmElem* app=(cmElem*)hApp;

    if (!app) return NULL;
    cmiAPIEnter(((HAPP)app, "cmGetValTree: hApp=%p.", hApp));

    hPvt=app->hVal;
    cmiAPIExit(((HAPP)app, "cmGetValTree=%p", hPvt));

    return hPvt;
}

RVAPI HPST RVCALLCONV
cmGetSynTreeByRootName(
                       IN   HAPP            hApp,
                       IN   const RvChar    *name)
{
    HPST hPst=NULL;
    cmElem* app=(cmElem*)hApp;

    if (!app) return NULL;
    cmiAPIEnter((hApp, "cmGetSynTreeByRootName: hApp=%p, name=%s.", hApp, name));

    switch(name[0])
    {
    case('a'):
        if (!strcmp("addr", name))                  hPst=app->hAddrSyn;
        break;
    case('c'):
        if (!strcmp("capData", name))               hPst=app->h245DataType;
        else if (!strcmp("capTransport", name))     hPst=app->h245TransCap;
        break;
    case('C'):
        if (!strcmp("CryptoH323Token", name))       hPst=app->synGkCrypto;
        break;
    case('f'):
        if (!strcmp("fsOpenLcn", name))             hPst=app->synOLC;
        break;
    case('h'):
        if (!strcmp("h245", name))                  hPst=app->synProtH245;
        break;
    case('H'):
        if (!strcmp("H245Configuration", name))     hPst=app->synConfH245;
        else if (!strcmp("H235Key", name))          hPst=app->synH235Key;
        break;
    case('K'):
        if (!strcmp("KeySyncMaterial", name))       hPst=app->synKeySync;
        break;
    case('P'):
        if (!strcmp("PwdCertToken", name))          hPst=app->synGkPwdCert;
        break;
    case('q'):
        if (!strcmp("q931", name))                  hPst=app->synProtQ931;
        else if (!strcmp("q931App", name))          hPst=app->hSyn;
        else if (!strcmp("q931UU", name))           hPst=app->synQ931UU;
        break;
    case('Q'):
        if (!strcmp("Q931Configuration", name))     hPst=app->synConfQ931;
        break;
    case('r'):
        if (!strcmp("ras", name))                   hPst=app->synProtRAS;
        else if (!strcmp("rasApp", name))           hPst=app->hRASSyn;
        else if (!strcmp("redEncoding", name))      hPst=app->h245RedEnc;
        break;
    case('R'):
        if (!strcmp("RASConfiguration", name))      hPst=app->synConfRAS;
        break;
    case('t'):
        if (!strcmp("terminalLabel", name))         hPst=app->synTerminalLabel;
        break;
    default:
        break;
    }

    cmiAPIExit((hApp, "cmGetSynTreeByRootName: hApp=%p.", hApp));
    return hPst;
}


RVAPI int RVCALLCONV
cmSetHandle(
            IN      HAPP        hApp,
            IN      HAPPAPP     haApp)
{
    cmElem* app=(cmElem*)hApp;
    if (!app) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmSetHandle: hApp=%p. haApp=%p.", hApp, haApp));
    app->haApp = haApp;
    cmiAPIExit((hApp, "cmSetHandle=0"));
    return RV_OK;
}

RVAPI int RVCALLCONV
cmGetHandle(
            IN      HAPP                hApp,
            IN      HAPPAPP*        haApp)
{
    cmElem* app=(cmElem*)hApp;
    if (!app) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmGetHandle: hApp=%p.", hApp));
    *haApp=app->haApp;
    cmiAPIExit((hApp, "cmGetHandle=(haApp=%p).", haApp));
    return RV_OK;
}


RVAPI
int RVCALLCONV cmSetMessageEventHandler(
    IN HAPP                             hApp,
    IN cmEvCallNewRawMessageT           cmEvCallNewRawMessage,
    IN cmEvCallRecvRawMessageT          cmEvCallRecvRawMessage,
    IN cmEvCallSendRawMessageT          cmEvCallSendRawMessage,
    IN cmEvCallReleaseMessageContextT   cmEvCallReleaseMessageContext)
{
    cmElem*  app= (cmElem *)hApp;

    if (hApp == NULL) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmSetMessageEventHandler(hApp=%p)", hApp));

    app->cmEvCallNewRawMessage          = cmEvCallNewRawMessage;
    app->cmEvCallRecvRawMessage         = cmEvCallRecvRawMessage;
    app->cmEvCallSendRawMessage         = cmEvCallSendRawMessage;
    app->cmEvCallReleaseMessageContext  = cmEvCallReleaseMessageContext;

    cmiAPIExit((hApp, "cmSetMessageEventHandler(hApp=%p) ret=0", hApp));
    return 0;
}



/************************************************************************
 * cmGetConfiguration
 * purpose: Gives access to the session's configuration context.
 *          The returned HCFG should be considered read-only, and should
 *          never be destroyed.
 * input  : hApp    - Stack instance handle
 * output : none
 * return : HCFG handle of the configuration on success
 *          NULL on failure
 ************************************************************************/
RVAPI
HCFG RVCALLCONV cmGetConfiguration(IN HAPP hApp)
{
    cmElem* app=(cmElem*)hApp;
    cmiAPIEnter((hApp, "cmGetConfiguration(hApp=%p)", hApp));
    cmiAPIExit((hApp, "cmGetConfiguration(hApp=%p)=%p", hApp, app->hCfg));
    return app->hCfg;
}


/************************************************************************
 * cmGetDelimiter
 * purpose: Returns the character used as a delimiter between fields,
 *          such as in the destAddress and srcAddress fields in the
 *          cmCallMake function.
 * input  : hApp    - Stack instance handle
 * output : none
 * return : Delimiter character used
 ************************************************************************/
RVAPI char RVCALLCONV
cmGetDelimiter(HAPP hApp)
{
    cmElem* app=(cmElem*)hApp;
    cmiAPIEnter((hApp, "cmGetDelimiter(hApp=%p)", hApp));
    cmiAPIExit((hApp, "cmGetDelimiter(hApp=%p)=%c", hApp, app->delimiter));
    return app->delimiter;
}


void cmLock(IN HAPP hApp)
{
    cmElem* app=(cmElem*)hApp;
    RvLockGet(&app->appLock, app->logMgr);
}

void cmUnlock(IN HAPP hApp)
{
    cmElem* app=(cmElem*)hApp;
    RvLockRelease(&app->appLock, app->logMgr);
}


/* TODO: It should not appear in cm.h */
RVAPI int RVCALLCONV
cmSetPartnerHandle(HAPP hApp, HAPPPARTNER hAppPartner)
{
    cmElem* app=(cmElem*)hApp;
    app->hAppPartner=hAppPartner;
    return RV_TRUE;
}

RVAPI int RVCALLCONV
cmGetPartnerHandle(HAPP hApp, HAPPPARTNER* hAppPartner)
{
    cmElem* app=(cmElem*)hApp;
    if (hAppPartner)
        *hAppPartner=app->hAppPartner;
    return RV_TRUE;
}


/************************************************************************
 * cmRestartListen
 * purpose: Stops and destruct all current listening q931 (tpkt & annexE) and ras sockets
 *          and restart the listening process again on the current q931 and ras listening
 *          addresses. The currently active calls are not affected, but new incoming
 *          calls are received on the new listening addresses.
 * input  : hApp        - Stack handle for the application
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmRestartListen(
        IN   HAPP        hApp)
{
    cmElem* app=(cmElem*)hApp;
    CMTRANSANNEXEPARAM eParams;
    RvPvtNodeId tmpNodeId;
    RvStatus status = RV_ERROR_UNKNOWN;
    cmTransportAddress tmpAddr;

    if (!hApp || !app->start)
        return status;

    cmiAPIEnter(((HAPP)app, "cmRestartListen(hApp=%p)", hApp));

    /*stop ras listening*/
    rasStop(app, RV_TRUE);

    /*stop listening process*/
    cmTransStop(app->hTransport);

        /*destroy address trees they are no longer valid*/
    pvtDelete(app->hVal, app->q931TpktAddresses);
    pvtDelete(app->hVal, app->q931AnnexEAddresses);
    pvtDelete(app->hVal, app->natq931TpktAddresses);
    pvtDelete(app->hVal, app->natq931AnnexEAddresses);

    /*rebuild address nodes from configuration (we asume the user have changed the configuration)*/
    /*tpkt*/
    app->q931TpktAddresses = pvtAddRootByPath(app->hVal, app->synConfQ931, "multipleCallSignalingAddresses", 0, NULL);
    tmpNodeId = pvtGetChild(app->hVal, app->q931Conf, __q931(multipleCallSignalingAddresses), NULL);
    if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
    {
        /* We support multi-homed. Get the addresses from the configuration and remember them. */
        pvtSetTree(app->hVal, app->q931TpktAddresses, app->hVal, tmpNodeId);
    }
    else
    {
        /* We don't have multi-homed IP addresses - check for a single address */
        RvInt32 port = 1720;
        pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(callSignalingPort), &port, NULL);
        memcpy(&tmpAddr, &app->localAddress, sizeof(tmpAddr));
        tmpAddr.port = (RvUint16)port;

        tmpNodeId = pvtAdd(app->hVal, app->q931TpktAddresses, __anyField, 0, NULL, NULL);
        cmLTAToVt(app->hVal, tmpNodeId, &tmpAddr);
    }


    /* -- Deal with Annex E parameters */
    tmpNodeId = pvtGetChild(app->hVal, app->q931Conf, __q931(useAnnexE), NULL);
    eParams.useAnnexE = RV_PVT_NODEID_IS_VALID(tmpNodeId);
    eParams.t_R1    = 500;
    eParams.n_R1    = 8;
    eParams.t_IMA1  = 6000;
    eParams.n_IMA1  = 6;
    eParams.t_DT    = 1;
    if (eParams.useAnnexE == RV_TRUE)
    {
        pvtGetChildByFieldId(app->hVal, tmpNodeId, __q931(t_R1) , (int *)&eParams.t_R1, NULL);
        pvtGetChildByFieldId(app->hVal, tmpNodeId, __q931(n_R1) , (int *)&eParams.n_R1, NULL);
        pvtGetChildByFieldId(app->hVal, tmpNodeId, __q931(t_IMA1) , (int *)&eParams.t_IMA1, NULL);
        pvtGetChildByFieldId(app->hVal, tmpNodeId, __q931(n_IMA1) , (int *)&eParams.n_IMA1, NULL);
        pvtGetChildByFieldId(app->hVal, tmpNodeId, __q931(t_DT) , (int *)&eParams.t_DT, NULL);
         eParams.t_R2    = (RvUint)(eParams.t_R1 * 21 / 10);
    }

    /*annexE addresses*/
    if (eParams.useAnnexE == RV_TRUE)
    {
        app->q931AnnexEAddresses = pvtAddRootByPath(app->hVal, app->synConfRAS, "alternateTransportAddresses.annexE", 0, NULL);
        __pvtGetNodeIdByFieldIds(tmpNodeId, app->hVal, app->rasConf, {_q931(alternateTransportAddresses) _q931(annexE) LAST_TOKEN});
        if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
        {
            /* We support multi-homed. Get the addresses from the configuration and remember them. */
            pvtSetTree(app->hVal, app->q931AnnexEAddresses, app->hVal, tmpNodeId);
        }
        else
        {
            /* Single static address */
            memcpy(&tmpAddr, &app->localAddress, sizeof(tmpAddr));
            tmpAddr.distribution = cmDistributionUnicast;
            tmpAddr.port = 2517;

            tmpNodeId = pvtAdd(app->hVal, app->q931AnnexEAddresses, __anyField, 0, NULL, NULL);
            cmTAToVt(app->hVal, tmpNodeId, &tmpAddr);
        }

    }

    /*start q931 listening process*/
    if (cmTransStart(app->hTransport, app->q931TpktAddresses, app->q931AnnexEAddresses, &app->localAddress, &eParams) == cmTransOK)
    {
        /*if NAT required  - build NAT trees*/
        if (app->cmNATMode != cmNatModeNoNAT)/*if NAT trees needed*/
        {
            getNatAddresses(app, eParams.useAnnexE);
        }

        /*no need to update q931 addresses in the default ras messages it is updated in rasStart*/

        status = rasStart(app, RV_TRUE);
        if (status < 0)
            cmTransStop(app->hTransport);
        else
            status = RV_OK;
    }

    /*restart the dns module*/
    if (status == RV_OK)
        status = cmRestartDnsModule(hApp);

    cmiAPIExit((hApp, "cmRestartListen=%d", status));
    return status;
}


/************************************************************************
 * cmMeiEnter
 * purpose: Enters critical section for the specified stack instance.
 * input  : hApp        - Stack's application handle
 * output : none
 * return : Non-negative on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmMeiEnter(IN HAPP hApp)
{
    cmElem* app = (cmElem *)hApp;

    if (hApp == NULL)
        return RV_ERROR_NULLPTR;

    cmiAPIEnter((hApp, "cmMeiEnter(hApp=%p)", hApp));
    RvMutexLock(&app->applicationLock, app->logMgr);
    cmiAPIExit((hApp, "cmMeiEnter(hApp=%p)=0", hApp));
    return RV_OK;
}


/************************************************************************
 * cmMeiExit
 * purpose: Exits critical section for the specified stack instance.
 * input  : hApp        - Stack's application handle
 * output : none
 * return : Non-negative on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmMeiExit(IN HAPP hApp)
{
    cmElem* app = (cmElem *)hApp;

    if (hApp == NULL)
        return RV_ERROR_NULLPTR;

    cmiAPIEnter((hApp, "cmMeiExit(hApp=%p)", hApp));
    RvMutexUnlock(&app->applicationLock, app->logMgr);
    cmiAPIExit((hApp, "cmMeiExit(hApp=%p)=0", hApp));
    return RV_OK;
}


/************************************************************************
 * cmThreadAttach
 * purpose: Indicate that the current running thread can be used to catch
 *          events from the network.
 *          Threads that called cmInitialize() don't have to use this
 *          function.
 *          Although this function has hApp as a parameter, the result
 *          will take place for all stack instances
 * input  : hApp        - Stack's application handle
 *          threadId    - Unused
 * output : none
 * return : Non-negative on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmThreadAttach(HAPP hApp, RvUint32 threadId)
{
    cmElem* app = (cmElem *)hApp;
    RvUint32 attachedIdx = 0;
    RvBool createdThread = RV_FALSE;
    RvStatus status = RV_OK;

    RV_UNUSED_ARG(threadId);

    if (hApp == NULL)
        return RV_ERROR_NULLPTR;

    cmiAPIEnter((hApp, "cmThreadAttach(hApp=%p)", hApp));

    /* Make sure we have a thread allocated for this one... */
    if (RvThreadCurrent() == NULL)
    {
        /* Don't have a thread yet - create one */
        RvThread *newThread;
        status = RvMemoryAlloc(NULL, sizeof(RvThread), app->logMgr, (void**)&newThread);

        if (status == RV_OK)
        {
            static int attachedThreadNum = 0;
            RvChar threadName[20];
            status = RvThreadConstructFromUserThread(app->logMgr, newThread);
            if (status == RV_OK)
            {
                /* Give this thread a name we can use for logging and debugging */
                attachedThreadNum++;
                RvSprintf(threadName, "%d H323Attached", attachedThreadNum);
                RvThreadSetName(newThread, threadName);
            }
        }

        createdThread = RV_TRUE;
    }

    /* Set up the counter for deallocation */
    if (status == RV_OK)
    {
        status = RvThreadFindVar("RvH323Attached", app->logMgr, &attachedIdx);
        if (status != RV_OK)
            status = RvThreadCreateVar(NULL, "RvH323Attached", app->logMgr, &attachedIdx);
    }

    /* Increment the number of times this thread was used */
    if (status == RV_OK)
    {
        RvUint32 counter;
        status = RvThreadGetVar(attachedIdx, app->logMgr, (void**)&counter);

        if (status == RV_OK)
        {
            if (!createdThread && (counter == 0))
            {
                /* Make sure we'll never try to deallocate a thread we didn't create */
                counter = 1;
            }
            counter++;
            status = RvThreadSetVar(attachedIdx, (void*)counter, app->logMgr);
        }
    }

    if (status == RV_OK)
        status = seliInit();

    cmiAPIExit((hApp, "cmThreadAttach(hApp=%p)=%d", hApp, status));

    return status;
}


/************************************************************************
 * cmThreadDetach
 * purpose: Indicate that the current running thread cannot be used to catch
 *          events from the network.
 *          Although this function has hApp as a parameter, the result
 *          will take place for all stack instances
 * input  : hApp        - Stack's application handle
 *          threadId    - Unused
 * output : none
 * return : Non-negative on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmThreadDetach(HAPP hApp, RvUint32 threadId)
{
    cmElem* app = (cmElem *)hApp;
    RvUint32 attachedIdx = 0;
    RvStatus status = RV_OK;
    RvThread *thInfo;

    RV_UNUSED_ARG(threadId);

    if (hApp == NULL)
        return RV_ERROR_NULLPTR;

    cmiAPIEnter((hApp, "cmThreadDetach(hApp=%p)", hApp));

    /* Make sure we have a thread allocated for this one... */
    thInfo = RvThreadCurrent();
    if (thInfo == NULL)
        status = RV_ERROR_BADPARAM;

    if (status == RV_OK)
        status = seliEnd();

    /* Set up the counter for deallocation */
    if (status == RV_OK)
        status = RvThreadFindVar("RvH323Attached", app->logMgr, &attachedIdx);

    /* Decrement the number of times this thread was used */
    if (status == RV_OK)
    {
        RvUint32 counter;
        status = RvThreadGetVar(attachedIdx, app->logMgr, (void**)&counter);

        if (status == RV_OK)
        {
            counter--;
            status = RvThreadSetVar(attachedIdx, (void*)counter, app->logMgr);

            if (counter == 0)
            {
                RvThreadDestruct(thInfo);
                status = RvMemoryFree(thInfo, app->logMgr);
            }
        }

    }

    cmiAPIExit((hApp, "cmThreadDetach(hApp=%p)=%d", hApp, status));

    return status;
}

/************************************************************************
 * cmLogMessage
 * purpose: Writes a user message into the log file under the APPL filter
 * input  : hApp        - Stack's application handle
 *          line        - The message to be printed (must be null-terminated)
 * output : none
 * return : Non-negative on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmLogMessage(IN HAPP hApp, IN const char *line)
{
    cmElem* app = (cmElem *)hApp;

    RV_UNUSED_ARG(line);

    if (app == NULL)
        return RV_ERROR_NULLPTR;

    RvLogInfo(&app->logAppl,
        (&app->logAppl, "%.100s", line));
    return RV_OK;
}






/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


static void cleanOnFailure(
    IN cmElem       *app,
    IN initStep     step,
    IN const RvChar *errorMsg,
    IN RvStatus     status)
{
    RV_UNUSED_ARG(status);

    if (errorMsg != NULL)
    {
        RvLogError(&app->logERR, (&app->logERR, "%s", errorMsg));
        cmiAPIExit(((HAPP)app, "cmInitialize()=%d", status));
    }

    switch (step)
    {
    case initStartCM:          fastStartEnd(app->hh245);            /* Flow to next case */
    case initFaststart:        H245End(app->hh245);                 /* Flow to next case */
    case initH245:
    case initWatchdogResources:rasEnd(app);                         /* Flow to next case */
    case initRAS:              cmDnsDestruct((HAPP)app);            /* Flow to next case */
    case initDns:              catDestruct(app->hCat);              /* Flow to next case */
    case initCat:              cmTransEnd(app->hTransport);         /* Flow to next case */
    case initTransport:        cmiEndCalls(app->hCalls);            /* Flow to next case */
    case initCalls:            configTreesEnd(app);                 /* Flow to next case */
    case initConfigTrees:      endPVT_PST(app);                     /* Flow to next case */
    case initPvtPst:           cmEmEnd();
                               RvH323TimerDestruct(app->hTimers);   /* Flow to next case */
    case initTimers:
    case initSystemparams:     ciDestruct(app->hCfg);               /* Flow to next case */
    case initWatchdog:         RvWatchdogEnd(&app->watchdog);       /* Flow to next case */
    case initConfig:           seliEnd();                           /* Flow to next case */
    case initLog:              cmiDestroyLog(app);                  /* Flow to next case */
    case initMalloc:           RvMemoryFree(app, NULL);
    }

    RvMutexUnlock(&rvCmGlobalMutex, NULL);
    cmShutdown();
}


/************************************************************************
 * initPVT_PST
 * purpose: Initialize any information related to PVT and PST
 * input  : app         - Stack instance handle
 *          vtNodeCount - Number of nodes in PVT node pool
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
static int initPVT_PST(IN cmElem*app, IN int vtNodeCount)
{
    int res;

    app->hSyn    =pstConstruct(cmEmGetQ931Syntax(), "Q931ApplicationMessage");
    app->hRASSyn =pstConstruct(cmEmGetQ931Syntax(), "RasApplicationMessage");

    app->synProtQ931=pstConstruct(cmEmGetQ931Syntax(), "Q931Message");
    app->synProtH245=pstConstruct(cmEmGetH245Syntax(), "MultimediaSystemControlMessage");
    app->synProtRAS =pstConstruct(cmEmGetQ931Syntax(), "RasMessage");

    app->h245TransCap=pstConstruct(cmEmGetH245Syntax(), "TransportCapability");
    app->h245RedEnc=pstConstruct(cmEmGetH245Syntax(), "RedundancyEncodingMethod");
    app->h245DataType=pstConstruct(cmEmGetH245Syntax(), "DataType");

    app->hAddrSyn=pstConstruct(cmEmGetQ931Syntax(), "TransportAddress");
    app->hAddrSynH245=pstConstruct(cmEmGetH245Syntax(), "TransportAddress");
    app->synOLC=pstConstruct(cmEmGetH245Syntax(), "OpenLogicalChannel");
    app->synConfQ931=pstConstruct(cmEmGetQ931Syntax(), "Q931Configuration");
    app->synConfH245=pstConstruct(cmEmGetH245Syntax(), "H245Configuration");
    app->synConfRAS=pstConstruct(cmEmGetQ931Syntax(), "RASConfiguration");
    app->synConfCommon = pstConstruct(cmEmGetCommonSyntax(), "SystemConfiguration");

    app->synAnnexL=pstConstruct(cmEmGetQ931Syntax(), "StimulusControl");
    app->synAnnexM=pstConstruct(cmEmGetQ931Syntax(), "H323-UU-PDU.tunnelledSignallingMessage");

    app->synQ931UU=pstConstruct(cmEmGetQ931Syntax(), "H323-UU-PDU");
    app->synTerminalLabel=pstConstruct(cmEmGetH245Syntax(), "TerminalLabel");
    app->synGkCrypto=pstConstruct(cmEmGetQ931Syntax(), "CryptoH323Token");
    app->synGkPwdCert=pstConstruct(cmEmGetQ931Syntax(), "PwdCertToken");
    app->synKeySync = pstConstruct(cmEmGetQ931Syntax(), "KeySyncMaterial");
    app->synH235Key = pstConstruct(cmEmGetQ931Syntax(), "H235Key");

    res = initParamSyntax(app);

    app->hVal = pvtConstruct(0, vtNodeCount);

    /* Make sure we were able to construct everything */
    if (app->hSyn && app->hRASSyn && app->synProtQ931 && app->synProtH245 && app->synProtRAS &&
        app->h245TransCap && app->h245RedEnc && app->h245DataType && app->hAddrSyn && app->hAddrSynH245 &&
        app->synOLC && app->synConfQ931 &&
        app->synConfH245 && app->synConfRAS && app->synAnnexL && app->synAnnexM &&
        app->synConfCommon && app->hVal && (res >= 0))

            return 0;

    endPVT_PST(app);
    return RV_ERROR_UNKNOWN;
}


/************************************************************************
 * endPVT_PST
 * purpose: Deallocate any information related to PVT and PST
 * input  : app         - Stack instance handle
 * output : none
 * return : none
 ************************************************************************/
static void endPVT_PST(IN cmElem* app)
{
    pstDestruct(app->hSyn);
    pstDestruct(app->hRASSyn);

    pstDestruct(app->synProtQ931);
    pstDestruct(app->synProtH245);
    pstDestruct(app->synProtRAS);

    pstDestruct(app->h245TransCap);
    pstDestruct(app->h245RedEnc);
    pstDestruct(app->h245DataType);

    pstDestruct(app->hAddrSyn);
    pstDestruct(app->hAddrSynH245);
    pstDestruct(app->synOLC);
    pstDestruct(app->synConfQ931);
    pstDestruct(app->synConfH245);
    pstDestruct(app->synConfRAS);
    pstDestruct(app->synConfCommon);

    pstDestruct(app->synAnnexL);
    pstDestruct(app->synAnnexM);

    pstDestruct(app->synQ931UU);
    pstDestruct(app->synTerminalLabel);
    pstDestruct(app->synGkCrypto);
    pstDestruct(app->synGkPwdCert);

    pstDestruct(app->synKeySync);
    pstDestruct(app->synH235Key);

    endParamSyntax(app);

    pvtDestruct(app->hVal);
}


/************************************************************************
 * configTreesInit
 * purpose: Load the configuration related to the RAS, Q931 and H245
 *          standards into PVT structures.
 * input  : app         - Stack instance handle
 * output : none
 * return : negative value on failure
 ************************************************************************/
static int configTreesInit(cmElem*app)
{
    app->sysConf = pvtAddRootShielded(app->hVal, app->synConfCommon, 0, NULL);
    pvtLoadFromConfig(app->hCfg, app->hVal, "system", app->sysConf, RV_FALSE, &app->logConfig);

    app->rasConf = pvtAddRootShielded(app->hVal, app->synConfRAS, 0, NULL);
    pvtLoadFromConfig(app->hCfg, app->hVal, "RAS", app->rasConf, RV_TRUE, &app->logConfig);

    app->q931Conf = pvtAddRootShielded(app->hVal, app->synConfQ931, 0, NULL);
    pvtLoadFromConfig(app->hCfg, app->hVal, "Q931", app->q931Conf, RV_TRUE, &app->logConfig);

    app->h245Conf = pvtAddRootShielded(app->hVal, app->synConfH245, 0, NULL);
    pvtLoadFromConfig(app->hCfg, app->hVal, "h245", app->h245Conf, RV_TRUE, &app->logConfig);

    if ((app->rasConf<0) || (app->q931Conf<0) || (app->h245Conf<0))
      return RV_ERROR_OUTOFRESOURCES;
    else return RV_OK;

}


/************************************************************************
 * configTreesEnd
 * purpose: Delete the PVT structures of the configuration.
 * input  : app         - Stack instance handle
 * output : none
 * return : none
 ************************************************************************/
static void configTreesEnd(cmElem*app)
{
    pvtDeleteShielded(app->hVal, app->sysConf);
    app->sysConf = RV_PVT_INVALID_NODEID;

    pvtDeleteShielded(app->hVal, app->rasConf);
    app->rasConf = RV_PVT_INVALID_NODEID;

    pvtDeleteShielded(app->hVal, app->q931Conf);
    app->q931Conf = RV_PVT_INVALID_NODEID;

    pvtDeleteShielded(app->hVal, app->h245Conf);
    app->h245Conf = RV_PVT_INVALID_NODEID;
}


/************************************************************************
 * readSystemParameters
 * purpose: Get the system parameters from the configuration.
 *          These parameters are found under the "system" root of the
 *          configuration.
 * input  : app                     - Stack instance handle
 * output : maxCalls                - Maximum number of calls supported
 *          maxChannels             - Maximum number of channels supported
 *          extraConnections        - Number of extra hosts and tpkt to allocate
 *          vtNodeCount             - Number of nodes in PVT trees
 *          maxBuffSize             - Maximum size of encoded message buffers
 *          maxPoolSizeInKB         - Maximum size of the transport pool to save
 *                                    encoded but still not sent messages
 *          poolElemSize            - Maximum non-fragmented message size
 *          maxListeningAddresses   - Maximum number of listening addresses
 *                                    of each type.
 * return : RV_OK on success, negative value on failure
 ************************************************************************/
static RvStatus readSystemParameters(
    IN  cmElem* app,
    OUT int*    maxCalls,
    OUT int*    maxChannels,
    OUT int*    extraConnections,
    OUT int*    vtNodeCount,
    OUT int*    maxBuffSize,
    OUT int*    maxPoolSizeInKB,
    OUT int*    poolElemSize,
    OUT int*    maxListeningAddresses)
{
    /* Find out which property mode is used for the property database of the calls */
    app->callPropertyMode=pmodeFullProperty;
    if (ciGetValue(app->hCfg, "system.callPropertyMode.copySingleMessages", NULL, NULL) == ERR_CI_NOERROR)
        app->callPropertyMode=pmodeCopySingleMessages;
    else if (ciGetValue(app->hCfg, "system.callPropertyMode.deleteSingleMessages", NULL, NULL) == ERR_CI_NOERROR)
        app->callPropertyMode=pmodeDeleteSingleMessages;
    else if (ciGetValue(app->hCfg, "system.callPropertyMode.doNotUseProperty", NULL, NULL) == ERR_CI_NOERROR)
        app->callPropertyMode=pmodeDoNotUseProperty;

    /* Find the number of calls and channels if written */
    ciGetValue(app->hCfg, "system.maxCalls" , NULL, maxCalls);
    ciGetValue(app->hCfg, "system.maxChannels" , NULL, maxChannels);

    /* Calculate the number of nodes if we won't have it later on */
    if (*maxCalls > 0)
    {
        switch (app->callPropertyMode)
        {
            case pmodeFullProperty:
            case pmodeCopySingleMessages:
            case pmodeDeleteSingleMessages:
                          /* maxCalls *(525call +75ras+maxChannels *75)+ 900init+200ras*/
                *vtNodeCount = (*maxCalls) * (600 + (*maxChannels) * 75) + 1100;
                break;

            case pmodeDoNotUseProperty:
                /* Here we want to use less resources... */
                *vtNodeCount = (*maxCalls) * (300 + (*maxChannels) * 45) + 1100;
                break;
        }
    }

    /* Find number of extra connections */
    ciGetValue(app->hCfg, "system.allocations.extraConnections", NULL, extraConnections);

    /* Find number of listening addresses */
    ciGetValue(app->hCfg, "system.allocations.maxMultiHomedAddresses", NULL, maxListeningAddresses);

    /* Find number of nodes, buffer and pool size */
    ciGetValue(app->hCfg, "system.allocations.vtNodeCount", NULL, vtNodeCount);
    ciGetValue(app->hCfg, "system.allocations.maxBuffSize", NULL, maxBuffSize);
    ciGetValue(app->hCfg, "system.allocations.maxTcpUnit", NULL, poolElemSize);
    *maxPoolSizeInKB=*maxCalls*
                     *maxBuffSize/1024; /*One message per call in the pool*/
    ciGetValue(app->hCfg, "system.allocations.maxPoolSizeInKB", NULL, maxPoolSizeInKB);

    /* port range allocation */
    {
        RvInt portFrom = 0x10000;
        RvInt portTo = 0x10000;
        RvBool hasFrom, hasTo;
        RvStatus status = RV_OK;
        app->portRange = NULL;

        hasFrom = (ciGetValue(app->hCfg, "system.portFrom", NULL, &portFrom) == ERR_CI_NOERROR);
        hasTo = (ciGetValue(app->hCfg, "system.portTo", NULL, &portTo) == ERR_CI_NOERROR);

        if (hasFrom && hasTo)
        {
            /* We have a port range to work with */
            status = RvMemoryAlloc(NULL, sizeof(RvPortRange), app->logMgr, (void**)&app->portRange);
            if (status == RV_OK)
            {
                status = RvPortRangeConstruct((RvUint)portFrom, (RvUint)portTo, app->logMgr, app->portRange);
                if (status != RV_OK)
                    RvMemoryFree(app->portRange, app->logMgr);
            }
        }
        else if (hasFrom || hasTo)
        {
            /* We have only one of them - not legal... */
            status = RV_ERROR_BADPARAM;
        }

        if (status != RV_OK)
        {
            RvLogError(&app->logERR, (&app->logERR, "Bad port range configuration"));
            app->portRange = NULL;
            return status;
        }
    }

    app->cidAssociate = (ciGetValue(app->hCfg, "system.cidAssociate", NULL, NULL) == ERR_CI_NOERROR);
    app->delimiter = ',';
    {
        char delimiter[2];
        if (ciGetString(app->hCfg, "system.delimiter", delimiter, 2) == ERR_CI_NOERROR)
            app->delimiter = delimiter[0];
    }

    return RV_OK;
}


/************************************************************************
 * rvCmWakeWatchdog
 * purpose: Timeout of a watchdog interval. We should print statistics to
 *          the log here
 * input  : context - The stack's instance
 * output : None
 * return : None
 ************************************************************************/
static RvBool rvCmWakeWatchdog(IN void* context)
{
    cmElem* app = (cmElem *)context;

    RvLogDebug(&app->logWatchdog, (&app->logWatchdog,
        "Watchdog Timer invoked:"));
    RvWatchdogPrint(&app->watchdog, &app->logWatchdog);

    return RV_TRUE;
}


/*---------------------------------------------------------------------------------------
 * cmWatchdogResourceCallback :
 *
 * this function sets the resources of cm at the watchdog internal table
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
static RvStatus RVCALLCONV cmWatchdogResourceCallback(
    IN  void*                       context,
    IN  RvUint32                    resource,
    IN  RvH323WatchdogResourceType  type,
    OUT RvUint32*                   value)
{
    cmElem* app=(cmElem*)context;

    if (resource == app->cmTimersResourceVal)
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

    else if (resource == app->cmValTreeResourceVal)
    {
        switch (type)
        {
        case RvH323WatchdogMaxVal: *value = (RvUint32)pvtMaxSize(app->hVal); return 0;
        case RvH323WatchdogMaxUsage: *value = (RvUint32)pvtMaxUsage(app->hVal); return 0;
        case RvH323WatchdogCurrent: *value = (RvUint32)pvtCurSize(app->hVal); return 0;
        default:
            return RV_ERROR_UNKNOWN;
        }
    }

    return RV_ERROR_UNKNOWN;
}

#if defined(RV_DEBUG) && (RV_LOGMASK & RV_PVT_LOGDEBUG)
static void RVCALLCONV cmWatchdogResourcePrintCallback(
    IN void*                        context,
    IN RvLogSource*                 logSource)
{
    cmElem* app = (cmElem *)context;

    pvtPrintRootNodes(app->hVal, RV_FALSE, logSource);
}
#endif


/************************************************************************
 * getNatAddresses
 * purpose: get the NAT translated q931 addresses from application and
 * add them to q931tpktaddresses and q931annexEaddresses trees.
 * input  : app                     - Stack instance handle
 *          useAnnexE               - RV_TRUE if the eParams.useAnnexE is set
 *
 *
 * return : RV_OK on success, negative value on failure
 ************************************************************************/
static RvStatus getNatAddresses(IN  cmElem* app, IN RvBool useAnnexE)
{
    RvPvtNodeId curaddress, tmpNodeId;

    /*tpkt*/

    if (RV_PVT_NODEID_IS_VALID(app->q931TpktAddresses))
    {
        app->natq931TpktAddresses = pvtAddRootByPath(app->hVal, app->synConfQ931, "multipleCallSignalingAddresses", 0, NULL);
        if (app->cmNATMode == cmNatModeNAT)/*"real and nat addresses needed*/
        {
            /*copy q931TpktAddresses into natq931TpktAddresses*/
            pvtSetTree(app->hVal, app->natq931TpktAddresses, app->hVal, app->q931TpktAddresses);
        }

        /*for each q931TpktAddress get a translation and add it to natq931TpktAddresses*/
        curaddress = pvtChild(app->hVal, app->q931TpktAddresses);
        if (app->cmMyEvent.cmEvNatRequest)
        {
            while(RV_PVT_NODEID_IS_VALID(curaddress))
            {
                cmTransportAddress ta;
                RvChar addr[RV_TRANSPORT_ADDRESS_STRINGSIZE];

                /*convert curaddress into cmTransportAddress*/
                cmVtToLTA(app->hVal, curaddress, &ta);
                cmTransportAddress2String(&ta, sizeof(addr), addr);
                /*get translation*/
                cmiCBEnter(((HAPP)app, "cmEvNATRequest(hApp=%p,Q931,address: %s)", app, addr));
                app->cmMyEvent.cmEvNatRequest((HAPP)app,
                                               cmProtocolQ931,
                                               &ta);
                cmTransportAddress2String(&ta, sizeof(addr), addr);
                cmiCBExit(((HAPP)app, "cmEvNATRequest(NatAddress: %s)", addr));

                /*add to natq931TpktAddresses*/
                tmpNodeId = pvtAdd(app->hVal, app->natq931TpktAddresses, __anyField, 0, NULL, NULL);
                cmLTAToVt(app->hVal, tmpNodeId, &ta);

                /*get next address*/
                curaddress = pvtBrother(app->hVal, curaddress);
            }
        }
    }

    /*annexE*/
    if (useAnnexE == RV_TRUE)
    {
        if (RV_PVT_NODEID_IS_VALID(app->q931AnnexEAddresses))
        {
            app->natq931AnnexEAddresses = pvtAddRootByPath(app->hVal, app->synConfRAS, "alternateTransportAddresses.annexE", 0, NULL);
            if (app->cmNATMode == cmNatModeNAT)
            {
                /*copy q931AnnexEAddresses into natq931AnnexEAddresses*/
                pvtSetTree(app->hVal, app->natq931AnnexEAddresses, app->hVal, app->q931AnnexEAddresses);
            }
            /*for each q931AnnexEAddress get a translation and add it to natq931AnnexEAddresses*/
            curaddress = pvtChild(app->hVal, app->q931AnnexEAddresses);
            if (app->cmMyEvent.cmEvNatRequest)
            {
                while(RV_PVT_NODEID_IS_VALID(curaddress))
                {
                    cmTransportAddress ta;
                    RvChar addr[RV_TRANSPORT_ADDRESS_STRINGSIZE];

                    /*convert curaddress into cmTransportAddress*/
                    cmVtToTA(app->hVal, curaddress, &ta);

                    cmTransportAddress2String(&ta, sizeof(addr), addr);
                    /*get translation*/
                    cmiCBEnter(((HAPP)app, "cmEvNATRequest(hApp=%p,AddressType=%s,address: %s)",
                    app, "q931AddressType", addr));
                    app->cmMyEvent.cmEvNatRequest((HAPP)app, (cmProtocol)0, &ta);
                    cmTransportAddress2String(&ta, sizeof(addr), addr);
                    cmiCBExit(((HAPP)app, "cmEvNATRequest(NatAddress: %s)", addr));

                    /*add to natq931TpktAddresses*/
                    tmpNodeId = pvtAdd(app->hVal, app->natq931AnnexEAddresses, __anyField, 0, NULL, NULL);
                    cmTAToVt(app->hVal, tmpNodeId, &ta);

                    /*get next address*/
                    curaddress = pvtBrother(app->hVal, curaddress);
                }
            }
        }
    }
    return RV_OK;
}


//{{gaoshizhong 20140423 use to mcu reg more gk account

/************************************************************************
 * cmRasModuleInit
 * purpose: Shuts down RasModule. call rasInit
 * input  : hApp    - Stack handle for the application
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 * notes  : 
 ************************************************************************/
RVAPI
int RVCALLCONV cmRasModuleInit(IN   HAPP        hApp)
{
    cmElem* app = (cmElem *)hApp;

    if (hApp == NULL)
        return RV_ERROR_NULLPTR;
	return rasInit(app);
}

/************************************************************************
 * cmRasModuleEnd
 * purpose: Shuts down RasModule. call rasEnd
 * input  : hApp    - Stack handle for the application
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 * notes  : 
 ************************************************************************/
RVAPI
int RVCALLCONV cmRasModuleEnd(IN   HAPP        hApp)
{
    cmElem* app = (cmElem *)hApp;

    if (hApp == NULL)
        return RV_ERROR_NULLPTR;
	return rasEnd(app);
}

//}}gaoshizhong 20140423 use to mcu reg more gk account


#ifdef __cplusplus
}
#endif
