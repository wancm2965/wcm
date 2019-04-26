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

/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/

#include "rvtime.h"
#include "rvselect.h"
#include "rvtimer.h"
#include "rvstdio.h"
#include "h32xLog.h"
#include "ra.h"
#include "cm.h"
#include "rvthreadinstance.h"
#include "seli.h"


#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/
    
#define TMP_LOG H32xLogGet()

#if (RV_OS_TYPE == RV_OS_TYPE_WIN32)
/* Our Windows implementation doesn't support seliCallOn() at all. */
#define SELI_MAX_USER_FDS (0)

#else
/* (RV_OS_TYPE != RV_OS_TYPE_WIN32) */
/* Maximum number of user FDs that will be used.
   This number is the amount of seliCallOn() calls on concurrent fds the user
   is about to use. */
#define SELI_MAX_USER_FDS (10)
#endif


/* User defined SELI events on file descriptors */
typedef struct
{
    RvSelectFd      fd; /* Actual core fd to use */
    seliCallback    callback; /* Callback to use */
} SeliUserFd;



/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/

/* Maximum number of user FDs that will be used.
   This number is the amount of seliCallOn() calls on concurrent fds the user
   is about to use. */
static RvSize_t seliMaxUserFds = SELI_MAX_USER_FDS;

/* Array holding the fd's memory allocations */
static HRA seliUserFds = NULL;

/* Indication how many times seliInit() was called. */
static RvInt seliInitCount = 0;




/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/

static void seliEventsCB(
        IN RvSelectEngine*  selectEngine,
        IN RvSelectFd*      fd,
        IN RvSelectEvents   selectEvent,
        IN RvBool           error);






/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/************************************************************************
 * seliInit
 * purpose: Initialize a SELI interface.
 * input  : none
 * output : none
 * return : RV_OK on success, negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV seliInit(void)
{
    RvStatus status;
    RvSelectEngine *engine;

    cmStartUp();

    status = RvSelectConstruct(2048, 0, TMP_LOG, &engine);

    /* Make sure we allocate some additional fd's for the user */
    if ((status == RV_OK) && (seliUserFds == NULL) && (seliMaxUserFds > 0) && (seliInitCount == 0))
    {
        seliUserFds = raConstruct(sizeof(SeliUserFd), seliMaxUserFds, RV_TRUE, "SELI USER FDs", H32xLogGet());
    }

    if (status == RV_OK)
        seliInitCount++;

    return status;
}

/************************************************************************
 * seliEnd
 * purpose: End a SELI interface.
 * input  : none
 * output : none
 * return : RV_OK on success, negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV seliEnd(void)
{
    RvStatus status;
    RvSelectEngine *engine;

    status = RvSelectGetThreadEngine(TMP_LOG, &engine);
    if ((status != RV_OK) || (engine == NULL))
        return status;

    status = RvSelectDestruct(engine, 0);

    seliInitCount--;

    if ((seliUserFds != NULL) && (seliInitCount == 0))
    {
        raDestruct(seliUserFds);
        seliUserFds = NULL;
    }

    cmShutdown();

    return status;
}

/************************************************************************
 * seliSelect
 * purpose: Block on the select() interface on some operating systems.
 *          Use parallel interfaces on other operating systems.
 *          An application should write a "while (1) seliSelect();"
 *          as its main loop.
 * input  : none
 * output : none
 * return : RV_OK on success, negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV seliSelect(void)
{
    return seliSelectUntil(RV_UINT32_MAX);
}

/******************************************************************************
 * seliSelectUntil
 * ----------------------------------------------------------------------------
 * General: Block on the select() interface on some operating systems.
 *          Use parallel interfaces on other operating systems.
 *          This function also gives the application the ability to give a
 *          maximum delay to block for.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  delay    - Maximum time to block on milliseconds.
 * Output: None.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV seliSelectUntil(IN RvUint32 delay)
{
    RvSelectEngine* selectEngine;
    RvStatus status;
    RvUint64 timeout;

    status = RvSelectGetThreadEngine(TMP_LOG, &selectEngine);
    if ((status != RV_OK) || (selectEngine == NULL))
        return status;

    timeout = RvUint64Mul(RvUint64FromRvUint32(delay), RV_TIME64_NSECPERMSEC);

    if (status == RV_OK)
        status = RvSelectWaitAndBlock(selectEngine, timeout);

    return status;
}

/************************************************************************
 * seliCallOn
 * purpose: Ask the SELI interface for events on a given handle/file
 *          descriptor.
 * input  : fd              - Handle/File descriptor to wait on for events
 *          sEvents         - Events to wait for
 *          callbackFunc    - Function to call when the event occurs
 * output : none
 * return : RV_OK on success, negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV seliCallOn(
    IN int              fd,
    IN seliEvents       sEvents,
    IN seliCallback     callbackFunc)
{
    RvSelectEngine* selectEngine;
    RvStatus status;
    RvSocket s;
    RvSelectFd* coreFd;
    SeliUserFd* userFd;
    RvSelectEvents coreEvents = 0;

    /* Convert the events to the core's events */
    if (sEvents & seliEvRead) coreEvents |= RvSelectRead;
    if (sEvents & seliEvWrite) coreEvents |= RvSelectWrite;


    /* Find the select engine for this thread at first */
    status = RvSelectGetThreadEngine(TMP_LOG, &selectEngine);
    if ((status != RV_OK) || (selectEngine == NULL))
        return status;

    /* Look for this fd if we're currently waiting for events on it */
    s = (RvSocket)fd;
    coreFd = RvSelectFindFd(selectEngine, &s);

    if ((coreFd == NULL) && ((int)sEvents != 0) && (callbackFunc != NULL))
    {
        /* This is a new fd we're waiting on - add it */

        /* Allocate an FD for the user */
        if (seliUserFds == NULL)
            return RV_ERROR_NOTSUPPORTED;
        if (raAdd(seliUserFds, (RAElement*)&userFd) >= 0)
        {
            userFd->callback = callbackFunc;
            status = RvFdConstruct(&userFd->fd, &s, TMP_LOG);
            if (status == RV_OK)
            {
                status = RvSelectAdd(selectEngine, &userFd->fd, coreEvents, seliEventsCB);
                if (status != RV_OK) RvFdDestruct(&userFd->fd);
            }
            if (status != RV_OK)
                raDelete(seliUserFds, (RAElement)userFd);
        }
        else
            return RV_ERROR_OUTOFRESOURCES; /* No more fd's to spare */
    }
    else if (coreFd != NULL)
    {
        userFd = RV_GET_STRUCT(SeliUserFd, fd, coreFd);

        /* We already have it */
        if (((int)sEvents == 0) || (callbackFunc == NULL))
        {
            /* We should remove this fd */
            RvSelectRemove(selectEngine, &userFd->fd);
            RvFdDestruct(&userFd->fd);
            raDelete(seliUserFds, (RAElement)userFd);
        }
        else
        {
            /* We should update this fd */
            status = RvSelectUpdate(selectEngine, &userFd->fd, coreEvents, seliEventsCB);
            if (status == RV_OK)
                userFd->callback = callbackFunc;
        }
    }

    return status;
}



int seliSetMaxDescs(IN int maxDescs)
{
    return RvSelectSetMaxFileDescriptors((RvUint32)maxDescs);
}

int seliGetMaxDescs(void)
{
    RvUint32 maxFds;

    maxFds = RvSelectGetMaxFileDescriptors();

    if (maxFds == 0)
        return RV_ERROR_UNKNOWN;
    else
        return (int)maxFds;
}




/* The following functions are only relevant for systems supporting the select() interface */
#if (RV_SELECT_TYPE == RV_SELECT_SELECT) && (RV_OS_TYPE != RV_OS_TYPE_WINCE)

RvStatus seliSelectEventsRegistration(
    IN  int        fdSetLen,
    OUT int        *num,
    OUT fd_set     *rdSet,
    OUT fd_set     *wrSet,
    OUT fd_set     *exSet,
    OUT RvUint32   *timeOut)
{
    RvSelectEngine* selectEngine;
    RvTimerQueue* timerQueue;
    RvStatus status;

    RV_UNUSED_ARG(exSet);
    RV_UNUSED_ARG(fdSetLen);

    /* Find the select engine for this thread at first */
    status = RvSelectGetThreadEngine(TMP_LOG, &selectEngine);
    if ((status != RV_OK) || (selectEngine == NULL))
        return status;

    /* Get fd_set bits from the select engine */
    status = RvSelectGetSelectFds(selectEngine, num, rdSet, wrSet);
    if (status != RV_OK)
        return status;

    status = RvSelectGetTimeoutInfo(selectEngine, NULL, &timerQueue);
    if (status != RV_OK)
        timerQueue = NULL;

    if (timerQueue != NULL)
    {
        RvInt64 nextEvent = RvInt64Const(0, 0, 0);

        status = RvTimerQueueNextEvent(timerQueue, &nextEvent);
        if (status == RV_OK)
        {
            if (RvInt64IsGreaterThan(nextEvent, RvInt64Const(0, 0, 0)))
            {
                /* Convert it to milliseconds, rounding up */
                *timeOut = RvInt64ToRvUint32(RvInt64Div(
                    RvInt64Sub(RvInt64Add(nextEvent, RV_TIME64_NSECPERMSEC), RvInt64Const(1, 0, 1)),
                        RV_TIME64_NSECPERMSEC));
            }
            else
            {
                /* Seems like we have timers to handle - enter select() without blocking there */
                *timeOut = 0;
            }
        }
        else if (RvErrorGetCode(status) == RV_TIMER_WARNING_QUEUEEMPTY)
        {
            /* Seems like we have no timers to handle - we should give some kind of an "infinite" timeout value */
            *timeOut = (RvUint32)-1;
        }

        status = RV_OK; /* Make sure we select() */
    }

    return status;
}

RvStatus seliSelectEventsHandling(
    IN fd_set   *rdSet,
    IN fd_set   *wrSet,
    IN fd_set   *exSet,
    IN int      num,
    IN int      numEvents)
{
    RvSelectEngine* selectEngine;
    RvTimerQueue* timerQueue = NULL;
    RvStatus status;

    RV_UNUSED_ARG(exSet);

    /* Find the select engine for this thread at first */
    status = RvSelectGetThreadEngine(TMP_LOG, &selectEngine);
    if ((status != RV_OK) || (selectEngine == NULL))
        return status;

    /* Handle the events we have */
    status = RvSelectHandleSelectFds(selectEngine, rdSet, wrSet, num, numEvents);

    /* We need to check the timeout as well... */
    status = RvSelectGetTimeoutInfo(selectEngine, NULL, &timerQueue);
    if ((status == RV_OK) && (timerQueue != NULL))
        RvTimerQueueService(timerQueue, 0, NULL, NULL, NULL);

    return status;
}

#endif  /* (RV_SELECT_TYPE == RV_SELECT_SELECT) && (RV_OS_TYPE != RV_OS_TYPE_WINCE) */



/* The following functions are only relevant for systems supporting the poll() interface */
#if (RV_SELECT_TYPE == RV_SELECT_POLL)

RvStatus seliPollEventsRegistration(
    IN  int             len,
    OUT struct pollfd   *pollFdSet,
    OUT int             *num,
    OUT RvUint32        *timeOut)
{
    RvSelectEngine* selectEngine;
    RvTimerQueue* timerQueue;
    RvUint32 numFds;
    RvStatus status;

    /* Find the select engine for this thread at first */
    status = RvSelectGetThreadEngine(TMP_LOG, &selectEngine);
    if ((status != RV_OK) || (selectEngine == NULL))
        return status;

    /* Get pollfd array from the select engine */
    numFds = (RvUint32)len;
    status = RvSelectGetPollFds(selectEngine, &numFds, pollFdSet);
    if (status != RV_OK)
        return status;
    *num = (int)numFds;

    status = RvSelectGetTimeoutInfo(selectEngine, NULL, &timerQueue);
    if (status != RV_OK)
        timerQueue = NULL;

    if (timerQueue != NULL)
    {
        RvInt64 nextEvent = RvInt64Const(0, 0, 0);

        status = RvTimerQueueNextEvent(timerQueue, &nextEvent);
        if (status == RV_OK)
        {
            if (RvInt64IsGreaterThan(nextEvent, RvInt64Const(0, 0, 0)))
            {
                /* Convert it to milliseconds, rounding up */
                *timeOut = RvInt64ToRvUint32(RvInt64Div(
                    RvInt64Sub(RvInt64Add(nextEvent, RV_TIME64_NSECPERMSEC), RvInt64Const(1, 0, 1)),
                        RV_TIME64_NSECPERMSEC));
            }
            else
            {
                /* Seems like we have timers to handle - enter poll() without blocking there */
                *timeOut = 0;
            }
        }
        else if (RvErrorGetCode(status) == RV_TIMER_WARNING_QUEUEEMPTY)
        {
            /* Seems like we have no timers to handle - we should give some kind of an "infinite" timeout value */
            *timeOut = (RvUint32)-1;
        }

        status = RV_OK; /* Make sure we poll() */
    }

    return status;
}

RvStatus seliPollEventsHandling(
    IN struct pollfd    *pollFdSet,
    IN int              num,
    IN int              numEvents)
{
    RvSelectEngine* selectEngine;
    RvTimerQueue* timerQueue = NULL;
    RvStatus status;

    /* Find the select engine for this thread at first */
    status = RvSelectGetThreadEngine(TMP_LOG, &selectEngine);
    if ((status != RV_OK) || (selectEngine == NULL))
        return status;

    status = RvSelectHandlePollFds(selectEngine, pollFdSet, num, numEvents);

    /* We need to check the timeout as well... */
    status = RvSelectGetTimeoutInfo(selectEngine, NULL, &timerQueue);
    if ((status == RV_OK) && (timerQueue != NULL))
        RvTimerQueueService(timerQueue, 0, NULL, NULL, NULL);

    return status;
}

#endif  /* (RV_SELECT_TYPE == RV_SELECT_POLL) */






#if defined(RV_H323_COMPILE_WITH_DEAD_FUNCTIONS)
/* The following functions are here for backward compatability only.
   They have no purpose besides that. */

int seliSetMaxTasks(int maxTasks)
{
    RV_UNUSED_ARG(maxTasks);
    return RV_TRUE;
}

int seliGetMaxTasks(void)
{
    return RV_ERROR_UNKNOWN;
}

#endif  /* defined(RV_H323_COMPILE_WITH_DEAD_FUNCTIONS) */





/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/

static void seliEventsCB(
        IN RvSelectEngine*  selectEngine,
        IN RvSelectFd*      fd,
        IN RvSelectEvents   selectEvent,
        IN RvBool           error)
{
    SeliUserFd* userFd;
    seliCallback cb;
    RvSocket* s;
    seliEvents seliEvent = (seliEvents)0;

    RV_UNUSED_ARG(selectEngine);

    userFd = RV_GET_STRUCT(SeliUserFd, fd, fd);

    s = RvFdGetSocket(&userFd->fd);

    if (selectEvent & RvSelectRead) seliEvent = seliEvRead;
    else if (selectEvent & RvSelectWrite) seliEvent = seliEvWrite;
    else return;

    cb = userFd->callback;
    if (cb != NULL)
        cb((int)(*s), seliEvent, error);
}








#ifdef __cplusplus
}
#endif
