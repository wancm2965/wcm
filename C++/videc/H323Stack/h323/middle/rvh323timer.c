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

#include "rv64ascii.h"
#include "rvtimer.h"
#include "rvtimestamp.h"
#include "rvlog.h"
#include "rvthread.h"
#include "rvselect.h"

#include "h32xLog.h"
#include "ra.h"
#include "rvthreadinstance.h"
#include "rvh323timer.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef struct
{
    RvLogMgr*       logMgr; /* Log manager to use */
    RvLogSource     log; /* Log source to use */
    RvTimerQueue*   timerQ; /* Timers queue we're working with for these timers */
    RvSelectEngine* selectEngine; /* Select engine these timers use */
    HRA             array; /* Array of actual timer objects */
    RvThread*       threadInfo; /* Thread this timers are processed in */
} RvH323Timers;







/************************************************************************
 * RvH323TimerConstruct
 * purpose: Create a timers array.
 * input  : maxTimers   - Maximum number of timers needed
 *          logMgr      - Log manager to use for timers
 * output : timersQueue - Queue of timers these timers are in
 * return : pointer to timers handle on success, NULL o.w.
 ************************************************************************/
RVAPI RvH323TimerPoolHandle RVCALLCONV RvH323TimerConstruct(
    IN    int               maxTimers,
    IN    RvLogMgr*         logMgr,
    OUT   RvTimerQueue**    timersQueue)
{
    RvStatus status;
    RvH323Timers *timers = NULL;
    RvTimerQueue *timerQ;
    RvSelectEngine *selectEngine;
    
    /* Find out an existing timer engine - we will need to work with it */
    status = RvSelectGetThreadEngine(logMgr, &selectEngine);
    if (status != RV_OK)
        return NULL;

    /* Although we have a timer engine, we want to add timers to it, so we try
       to construct one on our own */
    status = RvSelectConstruct(2048, (RvUint32)maxTimers, logMgr, &selectEngine);
    if (status != RV_OK)
        return NULL;

    status = RvSelectGetTimeoutInfo(selectEngine, NULL, &timerQ);
    if (status != RV_OK)
    {
        RvSelectDestruct(selectEngine, (RvUint32)maxTimers);
        return NULL;
    }

    /* Allocate the RA that will hold these timers */
    status = RvMemoryAlloc(NULL, sizeof(RvH323Timers), logMgr, (void**)&timers);
    if (status != RV_OK)
    {
        RvSelectDestruct(selectEngine, (RvUint32)maxTimers);
        return NULL;
    }

    timers->logMgr = logMgr;
    timers->timerQ = timerQ;
    timers->selectEngine = selectEngine;
    timers->array = raConstruct(sizeof(RvTimer), maxTimers, RV_TRUE, "Timer Array", logMgr);
    timers->threadInfo = RvThreadCurrent();

    if (timers->array == NULL)
    {
        RvMemoryFree(timers, logMgr);
        RvSelectDestruct(selectEngine, (RvUint32)maxTimers);
        return NULL;
    }

    RvLogSourceConstruct(logMgr, &timers->log, "TIMEPOOL", "Timers pool");

    if (timersQueue != NULL)
        *timersQueue = timerQ;

    return (RvH323TimerPoolHandle)timers;
}


/************************************************************************
 * RvH323TimerDestruct
 * purpose: Destruct a timers array.
 * input  : timers          - pointer to timers array
 * output : none
 * return : none
 ************************************************************************/
RVAPI void RVCALLCONV RvH323TimerDestruct(
    IN RvH323TimerPoolHandle    timersH)
{
    RvLogSource logSource; /* ugly but working */
    RvH323Timers * timers = (RvH323Timers *) timersH;
    RvSize_t numTimers = 0;
    if (timers == NULL)
        return;

    logSource = timers->log;

    if (timers->array != NULL)
    {
        RvTimer *timer;
        int curItem = raGetNext(timers->array, -1);
        numTimers = (RvSize_t)raMaxSize(timers->array);

        /* Before we destruct the timers pool we should probably cancel them all... */
        if (curItem >= 0)
        {
            RvLogExcep(&timers->log, (&timers->log,
                "RvH323TimerDestruct: %d timers are still active in tH=%p",
                raCurSize(timers->array), timersH));
            while (curItem >= 0)
            {
                timer = (RvTimer*)raGet(timers->array, curItem);
                if (timer != NULL)
                    RvTimerCancel(timer, RV_TIMER_CANCEL_WAIT_FOR_CB);
                curItem = raGetNext(timers->array, curItem);
            }
        }
        
        raDestruct(timers->array);
    }
    RvSelectDestruct(timers->selectEngine, numTimers);
    RvMemoryFree(timers, timers->logMgr);

    RvLogSourceDestruct(&logSource);
}


/************************************************************************
 * RvH323TimerStartWithType
 * purpose: Set a timer of the stack.
 * input  : timers          - pointer to timers array
 *          eventHandler    - Callback to call when timer expires
 *          context         - Context to use as parameter for callback function
 *          timeOut         - Timeout of timer in nanoseconds (0 is not ignored)
 *          timerType       - Type of timer: RV_TIMER_TYPE_ONESHOT, RV_TIMER_TYPE_PERIODIC
 * output : None
 * return : Pointer to timer on success, NULL o.w.
 ************************************************************************/
RVAPI RvTimer* RVCALLCONV RvH323TimerStartWithType(
    IN    RvH323TimerPoolHandle timersH,
    IN    RvTimerFunc           eventHandler,
    IN    void*                 context,
    IN    RvInt64               timeOut,
    IN    RvInt                 timerType)
{
    RvH323Timers* timers = (RvH323Timers *) timersH;
    RvTimer* timer = NULL;
    int loc;

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if (timers == NULL)
        return NULL;
#endif

/* Convert a signed 64 bit number to string. The buf parameter */
/* should be at least 22 bytes or an overrun can occur. */
#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
    if (RvLogIsSelected(&timers->log, RV_LOGLEVEL_ENTER))
    {
        RvChar timeoutBuf[RV_64TOASCII_BUFSIZE];
        Rv64toA(timeOut, timeoutBuf);
        RvLogTextEnter(&timers->log,
            "RvH323TimerStartWithType(tH=%p,eventHander=%p,context=%p,timeOut=%s,type=%d)",
            timersH, eventHandler, context, timeoutBuf, timerType);
    }
#endif

    /* Allocate a timer object */
    loc = raAdd(timers->array, (RAElement*)&timer);

    if (loc < 0)
        timer = NULL;
    else
    {
        RvStatus res;

        /* Start the timer */
        res = RvTimerStart(timer, timers->timerQ, timerType, timeOut, eventHandler, (void *)context);
        if (res != RV_OK)
        {
            RvLogError(&timers->log, (&timers->log,
                "Cannot start a timer (timer=%p, result=%d)", timer, res));
            raDelete(timers->array, (RAElement)timer);
            timer = NULL;
        }
    }

    RvLogLeave(&timers->log, (&timers->log,
        "RvH323TimerStartWithType(tH=%p,eventHander=%p,context=%p)=%p",
        timersH, eventHandler, context, timer));

    return timer;
}


/************************************************************************
 * RvH323TimerCancel
 * purpose: Reset a timer if it's set
 *          Used mainly for call timers.
 * input  : timer   - Timer to reset
 * output : timer   - Timer's value after it's reset
 * return : RV_OK on success, other on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323TimerCancel(
    IN    RvH323TimerPoolHandle timersH,
    INOUT RvTimer               **timer)
{
    RvH323Timers * timers = (RvH323Timers *) timersH;
    RvStatus ret = RV_OK;

    if(timers == NULL)
        return RV_ERROR_UNKNOWN;

    if (*timer != NULL)
    {
        RvLogEnter(&timers->log, (&timers->log,
            "RvH323TimerCancel(tH=%p,timer=%p)", timersH, *timer));

        ret = RvTimerCancel(*timer, RV_TIMER_CANCEL_DONT_WAIT_FOR_CB);
        if (ret!= RV_OK)
        {
            RvLogError(&timers->log, (&timers->log,
                "Error canceling timer(timer=%p)=%d", *timer, ret));
        }

        ret = raDelete(timers->array, (RAElement)*timer);

        RvLogLeave(&timers->log, (&timers->log,
            "RvH323TimerCancel(tH=%p,timer=%p)=%d", timersH, *timer, ret));

        if (ret > 0)
            ret = RV_OK;

        *timer = NULL;
    }

    return ret;
}


/************************************************************************
 * RvH323TimerClear
 * purpose: Clear a timer from the array
 * input  : timers  - pointer to timers array
 *          timer   - Timer to reset
 * output : none
 * return : RV_OK on success, other on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323TimerClear(
    IN    RvH323TimerPoolHandle timersH,
    IN    RvTimer               **timer)
{
    RvH323Timers * timers = (RvH323Timers *) timersH;
    RvStatus ret = RV_OK;

    if (timers == NULL)
        return RV_ERROR_UNKNOWN;

    if (*timer != NULL)
    {
        RvLogEnter(&timers->log, (&timers->log, "RvH323TimerClear(tH=%p,timer=%p)",
            timersH, *timer));

        ret = raDelete(timers->array, (RAElement)*timer);

        RvLogLeave(&timers->log, (&timers->log, "RvH323TimerClear(tH=%p,timer=%p)",
            timersH, *timer));

        if (ret > 0)
            ret = RV_OK;

        *timer = NULL;
    }

    return ret;
}


/************************************************************************
 * RvH323TimerStatistics
 * purpose: Get timer pool statistics
 * input  : timers      - pointer to timers array
 * output : statistics  - Statistics information about the timer pool
 * return : RV_OK on success, other on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323TimerStatistics(
    IN    RvH323TimerPoolHandle timersH,
    OUT   RvRaStatistics*       statistics)
{
    RvH323Timers * timers = (RvH323Timers *) timersH;
    RvStatus ret;

    ret = raGetStatistics(timers->array, statistics);
    if (ret > 0)
        ret = RV_OK;

    return ret;
}



#ifdef __cplusplus
}
#endif
