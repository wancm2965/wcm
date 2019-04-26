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

#include "rvtimestamp.h"
#include "rvclock.h"
#include "rvlog.h"
#include "h32xLog.h"
#include "ra.h"
#include "rvh323timer.h"
#include "seli.h"
#include "mti.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct RvH323ApplicationTimers_arg RvH323ApplicationTimers;
struct RvH323ApplicationTimers_arg
{
    RvLogMgr*                   logMgr; /* MTI log manager to use */
    RvLogSource                 log; /* MTI log source */
    RvH323TimerPoolHandle       timersPool; /* Timers pool to use */
    HRA                         timers; /* Array of timers we're holding */
    HAPPTIMER                   applicationHandle; /* Application handle for this set of timers */
    RvLock                      lock; /* Lock for these RA objects */

    RvH323ApplicationTimers*    prev; /* Previous timer pool used by application */
    RvH323ApplicationTimers*    next; /* Next timer pool used by application */
};


typedef struct
{
    RvH323ApplicationTimers*timers; /* Pool we're working in */
    RvTimer*                timer; /* Timer object we're working with */
    LPMTIMEREVENTHANDLER    eventHandler; /* Timer's callback to use */
    void*                   context; /* Context to use with the callback */
    RvBool                  inCallback; /* RV_TRUE if we're currently inside a user callback */
    RvBool                  deleted; /* RV_TRUE if timer should be deleted */
} RvH323ApplicationTimer;


/* todo: lock linked list access */
/* todo: change this parameter to thread specific */
static RvH323ApplicationTimers* mtiFirstApplTimers = NULL; /* First element in linked list
                                                              of application timers */



static RvBool mtimerTimerFunc(IN void* context)
{
    RvH323ApplicationTimers* timers;
    RvH323ApplicationTimer* timer = (RvH323ApplicationTimer *)context;
    RvBool result = RV_TRUE;

    /* Make sure element is valid and mark it */
    timers = timer->timers;
    if (timers == NULL)
    {
        /* Probably no timers at all */
        return RV_FALSE;
    }
    RvLockGet(&timers->lock, timers->logMgr);
    if (timer->timers == NULL)
    {
        RvLockRelease(&timers->lock, timers->logMgr);
        return RV_FALSE;
    }

    timer->inCallback = RV_TRUE;
    RvLockRelease(&timers->lock, timers->logMgr);

    /* Callback to application */
    timer->eventHandler(timer->context);

    /* Here, we just see if we have to kill this timer or not */
    RvLockGet(&timers->lock, timers->logMgr);
    timer->inCallback = RV_FALSE;

    if (timer->deleted)
    {
        RvH323TimerClear(timers->timersPool, &timer->timer);
        raDelete(timers->timers, (RAElement)timer);
        result = RV_FALSE;
    }

    RvLockRelease(&timers->lock, timers->logMgr);

    return result;
}




RVAPI RvUint32 RVCALLCONV timerGetTimeInMilliseconds(void)
{
    RvUint32 timestampInMillisec;

    timestampInMillisec = RvInt64ToRvUint32(RvInt64Div(RvTimestampGet(NULL), RV_TIME64_NSECPERMSEC));

    /* Make sure type-casted -1 is not used */
    if (timestampInMillisec == (RvUint32)-1)
        timestampInMillisec = 0;

    return timestampInMillisec;
}


RVAPI RvUint32 RVCALLCONV timerGetTimeInSeconds(void)
{
    return (RvUint32)RvClockGet(NULL, NULL);
}



RVAPI HSTIMER RVCALLCONV
mtimerInit(IN int maxTimers, IN HAPPTIMER appHndl)
{
    RvH323ApplicationTimers* timers;
    RvStatus status;

    status = seliInit();
    if (status != RV_OK)
        return NULL;

    status = RvMemoryAlloc(NULL, sizeof(RvH323ApplicationTimers), H32xLogGet(), (void**)&timers);
    if (status!= RV_OK)
        return NULL;
    memset(timers, 0, sizeof(RvH323ApplicationTimers));
    timers->logMgr = H32xLogGet();

    status = RvLogSourceConstruct(timers->logMgr, &timers->log, "MTI", "Application timers");
    if (status != RV_OK)
    {
        RvMemoryFree(timers, timers->logMgr);
        return NULL;
    }

    RvLogEnter(&timers->log, (&timers->log,
        "mtimerInit(maxTimers=%d,app=%p)", maxTimers, appHndl));

    timers->timers = raConstruct(sizeof(RvH323ApplicationTimer), maxTimers, RV_FALSE, "ApplicationTimers", timers->logMgr);
    if (timers == NULL)
    {
        RvLogSourceDestruct(&timers->log);
        RvMemoryFree(timers, timers->logMgr);
        return NULL;
    }

    /* Create the timers pool */
    timers->timersPool = RvH323TimerConstruct(maxTimers, timers->logMgr, NULL);
    if (timers->timersPool == NULL)
    {
        raDestruct(timers->timers);
        RvLogSourceDestruct(&timers->log);
        RvMemoryFree(timers, timers->logMgr);
        return NULL;
    }

    status = RvLockConstruct(timers->logMgr, &timers->lock);
    if (status != RV_OK)
    {
        RvH323TimerDestruct(timers->timersPool);
        raDestruct(timers->timers);
        RvLogSourceDestruct(&timers->log);
        RvMemoryFree(timers, timers->logMgr);
        return NULL;
    }

    timers->applicationHandle = appHndl;

    /* Fix linked list of timer pools */
    if (mtiFirstApplTimers == NULL)
        mtiFirstApplTimers = timers;
    else
    {
        /* We will add this one as first in the list */
        mtiFirstApplTimers->prev = timers;
        timers->next = mtiFirstApplTimers;
        mtiFirstApplTimers = timers;
    }

    RvLogLeave(&timers->log, (&timers->log,
        "mtimerInit()=%p", timers));

    return (HSTIMER)timers;
}


RVAPI int RVCALLCONV
mtimerEnd(IN HSTIMER timer)
{
    RvH323ApplicationTimers* timers;

    if (timer == NULL)
        return RV_ERROR_NULLPTR;

    timers = (RvH323ApplicationTimers *)timer;

    RvLogEnter(&timers->log, (&timers->log,
        "mtimerEnd(timer=%p)", timer));

    /* Remove this pool from the list */
    if (timers == mtiFirstApplTimers)
    {
        /* Removing the first one in the list */
        mtiFirstApplTimers = timers->next;
        if (mtiFirstApplTimers != NULL)
            mtiFirstApplTimers->prev = NULL;
    }
    else
    {
        /* Not the first element in list */
        timers->prev->next = timers->next;
        if (timers->next != NULL)
            timers->next->prev = timers->prev;
    }

    /* Kill this pool */
    RvH323TimerDestruct(timers->timersPool);
    raDestruct(timers->timers);
    RvLockDestruct(&timers->lock, timers->logMgr);
    RvLogSourceDestruct(&timers->log);
    RvMemoryFree(timers, timers->logMgr);

    return seliEnd();
}


RVAPI HTI RVCALLCONV
mtimerSet(
    IN HSTIMER              timer,
    IN LPMTIMEREVENTHANDLER eventHandler,
    IN void*                context,
    IN RvUint32             timeOut  /* 1 msec units */
    )
{
    RvH323ApplicationTimers* timers;
    RvH323ApplicationTimer* userTimer;
    int res;

    if (timer == NULL)
        return HTI_NULL_TIMER;

    timers = (RvH323ApplicationTimers *)timer;

    RvLogEnter(&timers->log, (&timers->log,
        "mtimerSet(timer=%p,handler=%p,context=%p,timeout=%d)",
        timer, eventHandler, context, timeOut));

    RvLockGet(&timers->lock, timers->logMgr);

    res = raAdd(timers->timers, (RAElement *)&userTimer);
    if (res < 0)
    {
        RvLogError(&timers->log, (&timers->log,
            "mtimerSet: Out of resources. Too many timers are already allocated."));
        res = -1;
    }
    else
    {
        if (timeOut == 0)
            timeOut = 1;

        userTimer->timer = RvH323TimerStartPeriodic(timers->timersPool, mtimerTimerFunc, userTimer, timeOut);
        if (userTimer->timer == NULL)
        {
            RvLogError(&timers->log, (&timers->log,
                "mtimerSet: Error starting the timer"));
            res = -1;

            raDelete(timers->timers, (RAElement)userTimer);
        }
        else
        {
            userTimer->timers = timers;
            userTimer->eventHandler = eventHandler;
            userTimer->context = context;
            userTimer->inCallback = RV_FALSE;
            userTimer->deleted = RV_FALSE;
        }
    }

    RvLockRelease(&timers->lock, timers->logMgr);

    RvLogLeave(&timers->log, (&timers->log,
        "mtimerSet()=%p", (HTI)res));

    return (HTI)res;
}



RVAPI int RVCALLCONV
mtimerReset(
    IN HSTIMER  timer,
    IN HTI      tElem)
{
    RvH323ApplicationTimers* timers;
    RvH323ApplicationTimer* userTimer;

    if ((timer == NULL) || ((RvInt32)tElem < 0))
        return RV_ERROR_NULLPTR;

    timers = (RvH323ApplicationTimers *)timer;

    RvLogEnter(&timers->log, (&timers->log,
        "mtimerReset(timer=%p,tElem=%p", timer, tElem));

    userTimer = (RvH323ApplicationTimer *)raGet(timers->timers, (int)tElem);
    if (userTimer == NULL)
    {
        RvLogError(&timers->log, (&timers->log,
            "mtimerReset: Timer element %p doesn't exist or not allocated!", tElem));
        return RV_ERROR_BADPARAM;
    }

    RvLockGet(&timers->lock, timers->logMgr);
    if (userTimer->timers != timers)
    {
        RvLogError(&timers->log, (&timers->log,
            "mtimerReset: Timer element %p failed the sanity check", tElem));
        RvLockRelease(&timers->lock, timers->logMgr);
        return RV_ERROR_BADPARAM;
    }

    if (userTimer->inCallback)
    {
        userTimer->deleted = RV_TRUE;
    }
    else
    {
        RvH323TimerCancel(timers->timersPool, &userTimer->timer);
        raDelete(timers->timers, (RAElement)userTimer);
    }

    RvLockRelease(&timers->lock, timers->logMgr);

    RvLogLeave(&timers->log, (&timers->log, "mtimerReset()=0"));
    return RV_OK;
}





#ifdef RV_H323_COMPILE_WITH_DEAD_FUNCTIONS

RVAPI int RVCALLCONV
mtimerEndByHandle(IN HAPPTIMER appHndl)
{
    RvH323ApplicationTimers* timers;

    /* First we'll try to find this one */
    timers = mtiFirstApplTimers;
    while (timers != NULL)
    {
        if (timers->applicationHandle == appHndl)
        {
            /* Found it! */
            return mtimerEnd((HSTIMER)timers);
        }

        timers = timers->next;
    }

    return RV_ERROR_BADPARAM;
}


RVAPI HTI RVCALLCONV
mtimerSetByHandle(
    IN HAPPTIMER            appHndl,
    IN LPMTIMEREVENTHANDLER eventHandler,
    IN void*                context,
    IN RvUint32             timeOut)
{
    RvH323ApplicationTimers* timers;

    /* First we'll try to find this one */
    timers = mtiFirstApplTimers;
    while (timers != NULL)
    {
        if (timers->applicationHandle == appHndl)
        {
            /* Found it! */
            return mtimerSet((HSTIMER)timers, eventHandler, context, timeOut);
        }

        timers = timers->next;
    }

    return HTI_NULL_TIMER;
}


RVAPI int RVCALLCONV
mtimerResetByValue(
    IN HSTIMER              timer,
    IN LPMTIMEREVENTHANDLER eventHandler,
    IN void*                context)
{
    RvH323ApplicationTimers* timers = (RvH323ApplicationTimers *)timer;
    RvH323ApplicationTimer* tm;
    int curElem = -1;
    int result = RV_ERROR_BADPARAM;
    RvBool found = RV_FALSE;

    if (timers == NULL)
        return RV_ERROR_NULLPTR;

    RvLockGet(&timers->lock, timers->logMgr);

    while (!found)
    {
        curElem = raGetNext(timers->timers, curElem);
        if (curElem >= 0)
        {
            tm = (RvH323ApplicationTimer *)raGet(timers->timers, curElem);
            if (tm != NULL)
            {
                found = (tm->context == context) && (tm->eventHandler == eventHandler);
            }
        }
        else
        {
            /* Looks like we're done */
            break;
        }
    }

    RvLockRelease(&timers->lock, timers->logMgr);

    if (found)
    {
        /* We found one to reset */
        result = mtimerReset(timer, (HTI)curElem);
    }

    return result;
}


RVAPI int RVCALLCONV
mtimerResetByHandle(
    IN HAPPTIMER    appHndl,
    IN HTI          tElem)
{
    RvH323ApplicationTimers* timers;

    /* First we'll try to find this one */
    timers = mtiFirstApplTimers;
    while (timers != NULL)
    {
        if (timers->applicationHandle == appHndl)
        {
            /* Found it! */
            return mtimerReset((HSTIMER)timers, tElem);
        }

        timers = timers->next;
    }

    return RV_ERROR_BADPARAM;
}



/************************************************************************
 * mtimerSetMaxTimerSets
 * purpose: To change the maximum number of timer sets (mtimerInit calls)
 *          allowed per thread (the default is 16)
 * input  : timerSets   - the maximum number of timer sets
 * output : none
 * return : none
 ************************************************************************/
RVAPI void RVCALLCONV mtimerSetMaxTimerSets(IN int timerSets)
{
    /* Empty function. It's not actually needed anymore */
    RV_UNUSED_ARG(timerSets);
}

#endif /* RV_H323_COMPILE_WITH_DEAD_FUNCTIONS */



#ifdef __cplusplus
}
#endif
