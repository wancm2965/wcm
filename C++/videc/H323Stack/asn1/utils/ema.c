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

/* Used when we want to debug only a specific resource type */
/*#define EMA_DEBUG_SPECIFIC
#define RV_EMA_DEBUG_DEADLOCKS
#define EMA_SPECIFIC_NAME "TRANSPORT Hosts"*/


#include <string.h>

#include "rvmutex.h"
#include "rvlock.h"
#include "rvmemory.h"
#include "rvlog.h"
#include "ra.h"
#include "ema.h"
#if defined(RV_EMA_DEBUG_DEADLOCKS)
#include "rvthread.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef EMA_DEBUG_SPECIFIC
#define RvEmaLogInfo(_params) {if (ema->logSpecific) RvLogInfo(&ema->log, _params);}
#define RvEmaLogDebug(_params) {if (ema->logSpecific) RvLogDebug(&ema->log, _params);}
#define RvEmaLogWarning(_params) {if (ema->logSpecific) RvLogWarning(&ema->log, _params);}
#define RvEmaLogError(_params) {if (ema->logSpecific) RvLogError(&ema->log, _params);}
#define RvEmaLogExcep(_params) {if (ema->logSpecific) RvLogExcep(&ema->log, _params);}

#else
#define RvEmaLogInfo(_params) RvLogInfo(&ema->log, _params)
#define RvEmaLogDebug(_params) RvLogDebug(&ema->log, _params)
#define RvEmaLogWarning(_params) RvLogWarning(&ema->log, _params)
#define RvEmaLogError(_params) RvLogError(&ema->log, _params)
#define RvEmaLogExcep(_params) RvLogExcep(&ema->log, _params)
#endif

#define SIZEOF_EMAELEM RvRoundToSize(sizeof(emaElem), RV_ALIGN_SIZE)
	
/************************************************************************
 * emaObject struct
 * Holds the information of an emaObject
 ************************************************************************/
typedef struct
{
    HRA         ra; /* Handle to RA we're using */
    RvLogMgr*   logMgr; /* Log manager used */
    RvLogSource log; /* Log to use for messages */
    RvLock      lock; /* Lock we're using */
    emaLockType lockType; /* Type of locking mechanism to use */
    RvUint32    elemSize; /* Size of each element inside EMA */
    RvUint32    type; /* Integer representing the type of objects stored in EMA */
    void*       userData; /* User related information associated with this EMA object */
    void const* instance; /* Instance associated with this EMA object */
    RvUint32    markCount; /* Number of items currently deleted and marked */
    emaEvElementIdle pIdleEv; /* Function to call when the element is not referanced and the Idle or Delete flags are on */

#ifdef EMA_DEBUG_SPECIFIC
    RvBool      logSpecific; /* Are we logging this EMA? */
#endif
} emaObject;


/************************************************************************
 * emaElem struct
 * Holds the information of an ema element
 *
 * Note: The mutex for the element is held in the end of the allocation
 *       if using emaNormalLocks. If emaLinkedLocks is used, then in the
 *       end of the allocation we'll have to pointer to the linked element
 *       in another EMA construct.
 ************************************************************************/
typedef struct
{
    emaObject*      ema; /* Pointer to the EMA object.
                            When this pointer is NULL, it indicates that the element
                            was deleted. We use this for emaLock().
                            THIS MUST BE THE FIRST FIELD IN THIS STRUCT! */
#ifdef RV_EMA_DEBUG
    RvUint32        debug1; /* Debugging bytes == EMA_DEBUG_BYTES */
#endif

    RvUint32        flags; /* Reference count and locks count.
                              It also holds a bit indicating if element was deleted */
    void*           appHandle; /* Application's handle of the EMA object */

#ifdef RV_EMA_DEBUG_DEADLOCKS
    RvThreadId      lockedThread; /* Indication who's the thread that locked this object */
#ifdef RV_EMA_DEBUG
    const RvChar*   filename; /* Filename that locked this object */
    int             lineno; /* Line number that locked this object */
#endif
#endif
#ifdef RV_EMA_DEBUG
    RvUint32        debug2; /* Debugging bytes == EMA_DEBUG_BYTES */
#endif
} emaElem;



/* Bytes used when debuging memory allocations */
#define EMA_DEBUG_BYTES (0xdeadbeef)

/* Indication in the reference count that this element was deleted */
#define EMA_ALREADY_DELETED     0x80000000
#define EMA_RAISE_IDLE          0x00008000

/* Indication of the reference count's actual value */
#define EMA_GET_REFERENCE(elem)         ((elem)->flags & 0x00007fff)
#define EMA_INC_REFERENCE(elem,count)   ((elem)->flags += (RvUint16)(count))
#define EMA_DEC_REFERENCE(elem,count)   ((elem)->flags -= (RvUint16)(count))

/* Indication of the locks count's actual value */
#define EMA_GET_LOCKS(elem)             (((elem)->flags & 0x7fff0000) >> 16)
#define EMA_INC_LOCKS(elem,count)       ((elem)->flags += (((RvUint32)(count)) << 16))
#define EMA_DEC_LOCKS(elem,count)       ((elem)->flags -= (((RvUint32)(count)) << 16))



/************************************************************************
 *
 *                              Private functions
 *
 ************************************************************************/


/************************************************************************
 * emaDeleteElement
 * purpose: Delete an element used by ema.
 * input  : ema         - EMA object used
 *          rElem       - Element to delete
 *          location    - Location of element deleted
 *          decCount    - RV_TRUE if we need to decrease count of marked/deleted elements
 *          functionName- Name of function that called this one - used for log
 * output : none
 * return : none
 ************************************************************************/
static void emaDeleteElement(
    IN emaObject*   ema,
    IN emaElem*     rElem,
    IN int          location,
    IN RvBool       decCount,
    IN const char*  functionName)
{
    RvBool locked = RV_TRUE;
    EMAElement parent = NULL;
    RV_UNUSED_ARG(location);
    RV_UNUSED_ARG(functionName);

    /* First of all, let's lock the element and remove the reference to ema on it.
       Doing this, allows us to stay blocked on an emaLock() call while trying to
       delete the element */
    switch (ema->lockType)
    {
        case emaNoLocks:        break;
        case emaNormalLocks:    RvMutexLock((RvMutex *)((char*)rElem + /*sizeof(emaElem)*/SIZEOF_EMAELEM + ema->elemSize), ema->logMgr); break;
        case emaLinkedLocks:
        {
            parent = *((EMAElement*)((char*)rElem + /*sizeof(emaElem)*/SIZEOF_EMAELEM + ema->elemSize));
            if (parent != NULL)
                locked = emaLock(parent);
            else
                locked = RV_FALSE;
            break;
        }
        case emaLockPointers:
        {
            RvMutex *pLock = *(RvMutex **)((char*)rElem + /*sizeof(emaElem)*/SIZEOF_EMAELEM + ema->elemSize);
            /* We lock through the pointer */
            RvMutexLock(pLock, ema->logMgr);

            /* make sure the lock didn't change */
            if (pLock != *(RvMutex **)((char*)rElem + /*sizeof(emaElem)*/SIZEOF_EMAELEM + ema->elemSize))
            {
                /* it changed. unlock it and lock the new pointer */
                RvMutexUnlock(pLock, ema->logMgr);
                RvMutexLock(*(RvMutex **)((char*)rElem + /*sizeof(emaElem)*/SIZEOF_EMAELEM + ema->elemSize), ema->logMgr);
            }
            break;
        }
    }
    rElem->ema = NULL;
    if (locked)
    {
        switch (ema->lockType)
        {
            case emaNoLocks:        break;
            case emaNormalLocks:    RvMutexUnlock((RvMutex *)((char*)rElem + /*sizeof(emaElem)*/SIZEOF_EMAELEM + ema->elemSize), ema->logMgr); break;
            case emaLinkedLocks:    emaUnlock(parent); break;
            case emaLockPointers:   RvMutexUnlock(*(RvMutex **)((char*)rElem + /*sizeof(emaElem)*/SIZEOF_EMAELEM + ema->elemSize), ema->logMgr); break;
        }
    }

    /* Do all the rest here */
    RvLockGet(&ema->lock, ema->logMgr);

    if (decCount)
        ema->markCount--;

    raDelete(ema->ra, (RAElement)rElem);

    RvLockRelease(&ema->lock, ema->logMgr);

    RvEmaLogDebug((&ema->log, "%s (%s): %p deleted (location=%d)", functionName, raGetName(ema->ra), rElem, location));
}







/************************************************************************
 *
 *                              Public functions
 *
 ************************************************************************/


/************************************************************************
 * emaConstruct
 * purpose: Create an EMA object
 * input  : elemSize            - Size of elements in the EMA in bytes
 *          maxNumOfElements    - Number of elements in EMA
 *          lockType            - Type of locking mechanism to use
 *          name                - Name of EMA (used in log messages)
 *          type                - Integer representing the type of objects
 *                                stored in this EMA.
 *          userData            - User related information associated with
 *                                this EMA object.
 *          instance            - Instance associated with this EMA object.
 *          logMgr              - Log manager to use.
 * output : none
 * return : Handle to RA constructed on success
 *          NULL on failure
 ************************************************************************/
RVINTAPI HEMA RVCALLCONV
emaConstruct(
    IN int          elemSize,
    IN int          maxNumOfElements,
    IN emaLockType  lockType,
    IN const char*  name,
    IN RvUint32     type,
    IN void*        userData,
    IN void const*  instance,
    IN RvLogMgr*    logMgr)
{
    emaObject* ema;
    int allocSize = 0;

    /* Allocate the object */
    if (RvMemoryAlloc(NULL, sizeof(emaObject), logMgr, (void**)&ema) != RV_OK)
        return NULL;

    /* Remember the type of elements stored */
    ema->type = type;
    ema->userData = userData;
    ema->instance = instance;
    ema->logMgr = logMgr;
    ema->pIdleEv = NULL;
#ifdef EMA_DEBUG_SPECIFIC
    ema->logSpecific = (strcmp(name, EMA_SPECIFIC_NAME) == 0);
#endif

    if (ema->logMgr != NULL)
    {
        /* Create a log handle for our use */
        RvLogSourceConstruct(ema->logMgr, &ema->log, "EMA", "Enhanced Memory Allocator");
    }
    else
    {
        ema->log = NULL;
    }

    /* Calculate the size of each element (platform-aligned) */
    ema->elemSize = RvRoundToSize(elemSize, RV_ALIGN_SIZE);

    /* See if we need a lock for each element */
    switch (lockType)
    {
        case emaNoLocks:        allocSize = (int)(ema->elemSize + /*sizeof(emaElem)*/SIZEOF_EMAELEM); break;
        case emaNormalLocks:    allocSize = (int)(ema->elemSize + /*sizeof(emaElem)*/SIZEOF_EMAELEM + sizeof(RvMutex)); break;
        case emaLinkedLocks:    allocSize = (int)(ema->elemSize + /*sizeof(emaElem)*/SIZEOF_EMAELEM + sizeof(EMAElement*)); break;
        case emaLockPointers:   allocSize = (int)(ema->elemSize + /*sizeof(emaElem)*/SIZEOF_EMAELEM + sizeof(RvMutex*)); break;
    }

    /* Create the RA */
    ema->ra = raConstruct(allocSize, maxNumOfElements, RV_FALSE, name, logMgr);

    /* Create the mutex */
    RvLockConstruct(ema->logMgr, &ema->lock);
    ema->lockType = lockType;
    ema->markCount = 0;

#if (RV_THREADNESS_TYPE != RV_THREADNESS_SINGLE)
    if (lockType == emaNormalLocks)
    {
        int i, j;
        RvStatus res = RV_OK;

        /* Initialize all the mutexes we need */
        for (i = 0; i < maxNumOfElements; i++)
        {
            res = RvMutexConstruct(ema->logMgr, (RvMutex *) (RV_RA_ELEM_DATA(ema->ra, i) + /*sizeof(emaElem)*/SIZEOF_EMAELEM + ema->elemSize));
            if (res != RV_OK)
                break;
        }
        if (res != RV_OK)
        {
            RvEmaLogError((&ema->log, "emaConstruct (%s): could not construct mutex", raGetName(ema->ra)));
            RvLockDestruct(&ema->lock, ema->logMgr);
            for (j = 0; j < i; j++)
            {
                RvMutexDestruct((RvMutex *) (RV_RA_ELEM_DATA(ema->ra, j) + /*sizeof(emaElem)*/SIZEOF_EMAELEM + ema->elemSize), ema->logMgr);
            }
            raDestruct(ema->ra);
            if (ema->log != NULL)
                RvLogSourceDestruct(&ema->log);
            RvMemoryFree(ema, ema->logMgr);
            ema = NULL;
        }
    }
#endif

    return (HEMA)ema;
}


/************************************************************************
 * emaDestruct
 * purpose: Free an EMA object, deallocating all of its used memory
 * input  : emaH   - Handle of the EMA object
 * output : none
 * return : none
 ************************************************************************/
RVINTAPI void RVCALLCONV
emaDestruct(IN HEMA emaH)
{
    emaObject* ema = (emaObject *)emaH;
    int numElems;

    if (ema == NULL) return;

    numElems = raCurSize(ema->ra);
    if (numElems > 0)
    {
        RvEmaLogWarning((&ema->log, "emaDestruct (%s): %d elements not deleted", raGetName(ema->ra), numElems));
    }

    /* Remove lock */
    RvLockDestruct(&ema->lock, ema->logMgr);

#if (RV_THREADNESS_TYPE != RV_THREADNESS_SINGLE)
    if (ema->lockType == emaNormalLocks)
    {
        int i;

        /* End all the mutexes we initialized */
        for (i = 0; i < raMaxSize(ema->ra); i++)
        {
            RvMutexDestruct((RvMutex *) (RV_RA_ELEM_DATA(ema->ra, i) + /*sizeof(emaElem)*/SIZEOF_EMAELEM + ema->elemSize), ema->logMgr);
        }
    }
#endif

    /* Free any used memory and RA */
    raDestruct(ema->ra);
    if (ema->log != NULL)
        RvLogSourceDestruct(&ema->log);
    RvMemoryFree(ema, ema->logMgr);
}


/************************************************************************
 * emaClear
 * purpose: Clear EMA from all used objects
 * input  : emaH   - Handle of the EMA object
 * output : none
 * return : none
 ************************************************************************/
RVINTAPI void RVCALLCONV
emaClear(IN HEMA emaH)
{
    emaObject* ema = (emaObject *)emaH;

    raClear(ema->ra);
}


/************************************************************************
 * emaAdd
 * purpose: Allocate an element in EMA for use, without initializing its
 *          value.
 *          This automatically locks the EMA object.
 * input  : emaH       - Handle of the EMA object
 *          appHandle   - Application's handle for the EMA object
 * output : none
 * return : Pointer to element added on success
 *          NULL on failure
 ************************************************************************/
RVINTAPI EMAElement RVCALLCONV
emaAdd(IN HEMA emaH, IN void* appHandle)
{
    emaObject* ema = (emaObject *)emaH;
    emaElem* elem;
    char* ptr;
    int location;

    /* Use RA for our purposes */
    RvLockGet(&ema->lock, ema->logMgr);
    location = raAdd(ema->ra, (RAElement*)&elem);
    RvLockRelease(&ema->lock, ema->logMgr);

    /* Make sure it was allocated */
    if (location < 0)
    {
        RvEmaLogError((&ema->log, "emaAdd (%s): Out of resources", raGetName(ema->ra)));
        return NULL;
    }

    /* Set the element's OO information */
    elem->ema = ema;
    elem->flags = 0;
    elem->appHandle = appHandle;
#if defined(RV_EMA_DEBUG)
    elem->debug1 = EMA_DEBUG_BYTES;
    elem->debug2 = EMA_DEBUG_BYTES;
#endif

    RvEmaLogDebug((&ema->log, "emaAdd (%s): Got %p (location=%d)", raGetName(ema->ra), elem, location));

	ptr = (char*)elem + /*sizeof(emaElem)*/SIZEOF_EMAELEM;

    /* Allocate a mutex if necessary */
    switch (ema->lockType)
    {
        case emaLinkedLocks:
        case emaLockPointers: memset(ptr + ema->elemSize, 0, sizeof(EMAElement*)); break;
        default:             break;
    }

    /* Calculate and return the position of the true element */
    return (EMAElement*)ptr;
}


/************************************************************************
 * emaDelete
 * purpose: Delete an element from EMA.
 *          Must be called when the element is locked
 * input  : elem    - Element to delete
 * return : Negative value on failure
 ************************************************************************/
#if defined(RV_EMA_DEBUG)
RVINTAPI int RVCALLCONV
emaDeleteDebug(IN EMAElement elem, IN const char* filename, IN int lineno)
#else
RVINTAPI int RVCALLCONV
emaDelete(IN EMAElement elem)
#endif
{
    emaObject* ema;
    emaElem* rElem;
    int location;

    if (elem == NULL) return RV_ERROR_UNKNOWN;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    /* Find the location */
    location = raGetByPtr(ema->ra, rElem);

#if defined(RV_EMA_DEBUG)
    RvEmaLogDebug((&ema->log, "emaDelete (%s): Deleting %d,%p,refCount=%d (%s:%d)",
            raGetName(ema->ra), location, rElem,
            EMA_GET_REFERENCE(rElem), filename, lineno));
#else
    RvEmaLogDebug((&ema->log, "emaDelete (%s): Deleting %d,%p (refCount=%d)",
            raGetName(ema->ra), location, rElem,
            EMA_GET_REFERENCE(rElem)));
#endif

#if defined(RV_EMA_DEBUG)
    if ((rElem->debug1 != EMA_DEBUG_BYTES) || (rElem->debug2 != EMA_DEBUG_BYTES))
    {
        RvEmaLogExcep((&ema->log, "emaDelete (%s): Someone is messing with memory %p", raGetName(ema->ra), rElem));
    }
#endif

    /* Check the reference count */
    if (rElem->flags == 0)
    {
        RvBool bParentLockExists = RV_FALSE;
        if (ema->lockType != emaLockPointers)
        {
            RvMutex *pLock = *((RvMutex **)((char*)elem + ema->elemSize));
            bParentLockExists = (pLock != NULL);
        }
        if ((ema->lockType != emaNoLocks) && bParentLockExists)
        {
            /* Must be called locked! raise an exception! */
            RvEmaLogExcep((&ema->log, "emaDelete (%s): Called while not locked! %d,%p",
                raGetName(ema->ra), location, rElem));
        }
        /* delete the element anyway */
        emaDeleteElement(ema, rElem, location, RV_FALSE, "emaDelete");
    }
    else
    {
        RvLockGet(&ema->lock, ema->logMgr);

#if defined(RV_EMA_DEBUG)
        if ((rElem->flags & EMA_ALREADY_DELETED) != 0)
        {
            RvEmaLogExcep((&ema->log, "emaDelete (%s): Deleting an element %p for the second time",
                     raGetName(ema->ra), rElem));
        }
#endif
        ema->markCount++;
        rElem->flags |= EMA_ALREADY_DELETED;

        /* remove the raise idle flag */
        rElem->flags &= ~EMA_RAISE_IDLE;

        RvLockRelease(&ema->lock, ema->logMgr);
    }


    return 0;
}


/************************************************************************
 * emaRaiseIdle
 * purpose: Delete an element from EMA
 *          Must be called when the element is locked
 * input  : elem    - Element to delete
 * return : Negative value on failure
 ************************************************************************/
#if defined(RV_EMA_DEBUG)
RVINTAPI int RVCALLCONV
emaRaiseIdleDebug(IN EMAElement elem, IN const char* filename, IN int lineno)
#else
RVINTAPI int RVCALLCONV
emaRaiseIdle(IN EMAElement elem)
#endif
{
    emaObject* ema;
    emaElem* rElem;
    int location;

    if (elem == NULL) return RV_ERROR_UNKNOWN;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    /* Find the location */
    location = raGetByPtr(ema->ra, rElem);

#if defined(RV_EMA_DEBUG)
    RvEmaLogDebug((&ema->log, "emaRaiseIdle (%s): Waiting for %d,%p,refCount=%d (%s:%d)",
            raGetName(ema->ra), location, rElem,
            EMA_GET_REFERENCE(rElem), filename, lineno));
#else
    RvEmaLogDebug((&ema->log, "emaRaiseIdle (%s): Waiting for %d,%p (refCount=%d)",
            raGetName(ema->ra), location, rElem,
            EMA_GET_REFERENCE(rElem)));
#endif

#if defined(RV_EMA_DEBUG)
    if ((rElem->debug1 != EMA_DEBUG_BYTES) || (rElem->debug2 != EMA_DEBUG_BYTES))
    {
        RvEmaLogExcep((&ema->log, "emaRaiseIdle (%s): Someone is messing with memory %p", raGetName(ema->ra), rElem));
    }
#endif

    /* Check the reference count */
    if (rElem->flags == 0)
    {
        if (ema->lockType != emaNoLocks)
        {
            /* Must be called locked! raise an exception! */
            RvEmaLogExcep((&ema->log, "emaRaiseIdle (%s): Called while not locked! %d,%p",
                raGetName(ema->ra), location, rElem));
        }
        /* raise idle anyway */
        ema->pIdleEv(elem, RV_FALSE);
    }
    else
    {
        RvLockGet(&ema->lock, ema->logMgr);

#if defined(RV_EMA_DEBUG)
        if ((rElem->flags & EMA_ALREADY_DELETED) != 0)
        {
            RvEmaLogExcep((&ema->log, "emaRaiseIdle (%s): Element was already deleted",
                     raGetName(ema->ra), rElem));
        }
#endif
        rElem->flags |= EMA_RAISE_IDLE;

        RvLockRelease(&ema->lock, ema->logMgr);
    }


    return 0;
}

/************************************************************************
 * emaLinkToElement
 * purpose: Link an EMA element to another element, from a different
 *          EMA construct. This function should be used when the EMA we're
 *          dealing with was created with emaLinkedLocks. This function
 *          allows the element to use a different element's lock.
 *          This function will only work if the element has no reference
 *          count at present.
 * input  : elem        - Element to link
 *          otherElem   - Element we're linking to. Should be constructed
 *                        with emaNormalLocks or linked to such element.
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
emaLinkToElement(IN EMAElement elem, IN EMAElement otherElem)
{
    emaObject*  ema;
    emaElem*    rElem;
    EMAElement* parent;

    if (elem == NULL) return RV_ERROR_UNKNOWN;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    /* Make sure we've got the element */
    if (ema == NULL) return RV_ERROR_UNKNOWN;

    RvEmaLogDebug((&ema->log, "emaLinkToElement (%s): Linking %p to %p", raGetName(ema->ra), rElem, otherElem));

#if defined(RV_EMA_DEBUG)
    if (ema->lockType != emaLinkedLocks)
    {
        RvEmaLogExcep((&ema->log, "emaLinkToElement (%s): This EMA cannot be linked", raGetName(ema->ra)));
        return RV_ERROR_UNKNOWN;
    }
#endif

    /* Find place of parent */
    parent = (EMAElement*)((char*)elem + ema->elemSize);
    if (EMA_GET_REFERENCE(rElem) > 0)
    {
        RvEmaLogError((&ema->log, "emaLinkToElement (%s): Cannot link %p - has a positive reference count",
                 raGetName(ema->ra), rElem));
        return RV_ERROR_UNKNOWN;
    }

    *parent = otherElem;
    return 0;
}


/************************************************************************
 * emaSetLockPointer
 * purpose: Set an EMA element to use a given lock. This function should
 *          be used when the EMA we're dealing with was created with
 *          emaLockPointers.
 * input  : elem        - Element to link
 *          lockPointer - Pointer to the lock to use.
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
emaSetLockPointer(IN EMAElement elem, IN void * lockPointer)
{
    emaObject*  ema;
    emaElem*    rElem;
    RvMutex**   pLockPtr;

    if (elem == NULL) return RV_ERROR_NULLPTR;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    /* Make sure we've got the element */
    if (ema == NULL) return RV_ERROR_UNKNOWN;

    RvEmaLogDebug((&ema->log, "emaUseLockPointer (%s): Linking %p to %p", raGetName(ema->ra), rElem, lockPointer));

#if defined(RV_EMA_DEBUG)
    if (ema->lockType != emaLockPointers)
    {
        RvEmaLogExcep((&ema->log, "emaUseLockPointer (%s): This EMA cannot use pointers", raGetName(ema->ra)));
        return RV_ERROR_UNKNOWN;
    }
#endif

    /* Find place of pointer */
    pLockPtr = (RvMutex**)((char*)elem + ema->elemSize);
    *pLockPtr = lockPointer;
    return 0;
}


/************************************************************************
 * emaUseLockPointer
 * purpose: Get the lock pointer used by an EMA element This function
 *          should be used when the EMA we're dealing with was created
 *          with emaLockPointers.
 * input  : elem        - Element to link
 * output : lockPointer - Pointer to the lock used
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
emaGetLockPointer(IN EMAElement elem, OUT void ** lockPointer)
{
    emaObject*  ema;
    emaElem*    rElem;

    if (elem == NULL) return RV_ERROR_NULLPTR;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    /* Make sure we've got the element */
    if (ema == NULL) return RV_ERROR_UNKNOWN;

#if defined(RV_EMA_DEBUG)
    if (ema->lockType != emaLockPointers)
    {
        RvEmaLogExcep((&ema->log, "emaGetLockPointer (%s): This EMA cannot use pointers", raGetName(ema->ra)));
        return RV_ERROR_UNKNOWN;
    }
#endif

    /* Get pointer */
    *lockPointer = *(RvMutex**)((char*)elem + ema->elemSize);
    return 0;
}


/************************************************************************
 * emaLock
 * purpose: Lock an element in EMA for use from the executing thread only
 *          This function will succeed only if the element exists
 * input  : elem    - Element to lock
 * output : none
 * return : RV_TRUE    - When the element exists and was locked
 *          RV_FALSE   - When the element doesn't exist (NULL are was deleted)
 *                    In this case, there's no need to call emaUnlock().
 ************************************************************************/
#if defined(RV_EMA_DEBUG)
RVINTAPI RvBool RVCALLCONV
emaLockDebug(IN EMAElement elem, IN const char* filename, IN int lineno)
#else
RVINTAPI RvBool RVCALLCONV
emaLock(IN EMAElement elem)
#endif
{
    emaObject*  ema;
    emaElem*    rElem;
    RvBool      status = RV_TRUE;
#if defined(RV_EMA_DEBUG_DEADLOCKS)
    RvThreadId  curThreadId = RvThreadCurrentId();
#endif

    if (elem == NULL) return RV_FALSE;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    /* Make sure we've got the element */
    if (ema == NULL) return RV_FALSE;

#if defined(RV_EMA_DEBUG)
    RvEmaLogDebug((&ema->log, "emaLock (%s): Locking %p (%s:%d)", raGetName(ema->ra), rElem, filename, lineno));
#endif

    switch (ema->lockType)
    {
#if defined(RV_EMA_DEBUG)
        case emaNoLocks:
            RvEmaLogExcep((&ema->log, "emaLock (%s): This EMA cannot be locked", raGetName(ema->ra)));
            break;
#endif
        case emaNormalLocks:
            /* We lock it */
            RvMutexLock((RvMutex *)((char*)elem + ema->elemSize), ema->logMgr);

            /* Now that it's locked, see if the element still exists */
            if (rElem->ema == NULL)
            {
                /* Seems like someone has deleted this element when we were trying to lock it */
#if defined(RV_EMA_DEBUG)
                RvEmaLogDebug((&ema->log, "emaLock (%s): Unlocking deleted element %p", raGetName(ema->ra), rElem));
#endif
                status = RV_FALSE;

                /* Release the lock - we shouldn't go on with it */
                RvMutexUnlock((RvMutex *)((char*)elem + ema->elemSize), ema->logMgr);
            }
            break;

        case emaLinkedLocks:
        {
            EMAElement  parent = NULL;
            /* We lock the parent */
            parent = *((EMAElement*)((char*)elem + ema->elemSize));
            if (parent != NULL)
                status = emaLock(parent);
            else
                status = RV_FALSE;

            if (status == RV_FALSE)
            {
                RvEmaLogWarning((&ema->log, "emaLock (%s): Couldn't lock parent=%p of %p for some reason",
                         raGetName(ema->ra), parent, elem));
            }
            break;
        }
        case emaLockPointers:
        {
            RvMutex *pLock = *((RvMutex **)((char*)elem + ema->elemSize));
            /* We lock through the pointer */
            RvMutexLock(pLock, ema->logMgr);

            /* make sure the lock didn't change */
            if (pLock != *(RvMutex **)((char*)elem + ema->elemSize))
            {
                /* it changed. unlock it and lock the new pointer */
                RvMutexUnlock(pLock, ema->logMgr);
                RvMutexLock(*(RvMutex **)((char*)elem + ema->elemSize), ema->logMgr);
            }

            /* Now that it's locked, see if the element still exists */
            if (rElem->ema == NULL)
            {
                /* Seems like someone has deleted this element when we were trying to lock it */
#if defined(RV_EMA_DEBUG)
                RvEmaLogDebug((&ema->log, "emaLock (%s): Unlocking deleted element %p", raGetName(ema->ra), rElem));
#endif
                status = RV_FALSE;

                /* Release the lock - we shouldn't go on with it */
                RvMutexUnlock(*(RvMutex **)((char*)elem + ema->elemSize), ema->logMgr);
            }
            break;
        }
        default:
            break;
    }

    /* Make sure we increment the reference count on this one */
    if (status == RV_TRUE)
    {
#if defined(RV_EMA_DEBUG_DEADLOCKS)
        rElem->lockedThread = curThreadId;
#if defined(RV_EMA_DEBUG)
        rElem->filename = filename;
        rElem->lineno = lineno;
#endif
#endif
        EMA_INC_REFERENCE(rElem,1);
        EMA_INC_LOCKS(rElem,1);
    }

    return status;
}


/************************************************************************
 * emaUnlock
 * purpose: Unlock an element in EMA that were previously locked by
 *          emaLock() from the same thread
 * input  : elem    - Element to unlock
 * output : none
 * return : RV_TRUE    if element still exists
 *          RV_FALSE   if element was deleted and is not valid anymore
 *          Negative value on failure
 ************************************************************************/
#if defined(RV_EMA_DEBUG)
RVINTAPI int RVCALLCONV
emaUnlockDebug(IN EMAElement elem, IN const char* filename, IN int lineno)
#else
RVINTAPI int RVCALLCONV
emaUnlock(IN EMAElement elem)
#endif
{
    emaObject* ema;
    emaElem* rElem;
    int elemExists;

    if (elem == NULL) return RV_ERROR_UNKNOWN;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

#if defined(RV_EMA_DEBUG)
    RvEmaLogDebug((&ema->log,
        "emaUnlock (%s): Unlocking %p (%s:%d)", raGetName(ema->ra), rElem, filename, lineno));

    if (EMA_GET_LOCKS(rElem) == 0)
    {
        RvEmaLogExcep((&ema->log,
            "emaUnlock (%s): Element %p (%s:%d) is not locked!", raGetName(ema->ra), rElem, filename, lineno));
        return RV_OK;
    }
#endif

    EMA_DEC_REFERENCE(rElem,1);
    EMA_DEC_LOCKS(rElem,1);
    elemExists = ((rElem->flags & EMA_ALREADY_DELETED) == 0);

    /* Check if we should raise the idle state */
    if (rElem->flags == EMA_RAISE_IDLE)
    {
        /* leave the element marked and locked */
        EMA_INC_REFERENCE(rElem,1);
        EMA_INC_LOCKS(rElem,1);
        rElem->flags &= ~EMA_RAISE_IDLE;
        ema->pIdleEv(elem, RV_FALSE);
        EMA_DEC_REFERENCE(rElem,1);
        EMA_DEC_LOCKS(rElem,1);
    }
    /* Delete element if we're done with it */
    if (rElem->flags == EMA_ALREADY_DELETED)
    {
        int location = raGetByPtr(ema->ra, rElem);

        /* clean up the element, if needed */
        if (ema->pIdleEv != NULL)
        {
            /* mark the element so we don't get to a loop the next time someone calls lock and unlock */
            EMA_INC_REFERENCE(rElem,1);
            ema->pIdleEv(elem, RV_TRUE);
            EMA_DEC_REFERENCE(rElem,1);
        }
        /* now remove it */
        emaDeleteElement(ema, rElem, location, RV_TRUE, "emaUnlock");
    }

    switch (ema->lockType)
    {
#if defined(RV_EMA_DEBUG)
        case emaNoLocks:
            RvEmaLogExcep((&ema->log, "emaUnlock (%s): This EMA cannot be unlocked", raGetName(ema->ra)));
            break;
#endif
        case emaNormalLocks:
            /* We unlock it */
            RvMutexUnlock((RvMutex *)((char*)elem + ema->elemSize), ema->logMgr);
            break;
        case emaLinkedLocks:
        {
            /* We lock the parent */
            int result = RV_ERROR_UNKNOWN;
            EMAElement  parent = *((EMAElement*)((char*)elem + ema->elemSize));
            if (parent != NULL)
                result = emaUnlock(parent);

            if (result < 0)
            {
                RvEmaLogWarning((&ema->log, "emaUnlock (%s): Couldn't unlock parent=%p of %p for some reason",
                         raGetName(ema->ra), parent, elem));
                elemExists = result;
            }
            break;
        }
        case emaLockPointers:
        {
            /* We unlock through the pointer */
            RvMutexUnlock(*(RvMutex **)((char*)elem + ema->elemSize), ema->logMgr);
            break;
        }
        default:
            break;
    }

    return elemExists;
}


/************************************************************************
 * emaMark
 * purpose: Mark an element in EMA for use, not letting anyone delete
 *          this element until it is release.
 *          This automatically locks the EMA object.
 * input  : elem    - Element to mark
 * output : none
 * return : Negative value on failure
 ************************************************************************/
#if defined(RV_EMA_DEBUG)
RVINTAPI int RVCALLCONV
emaMarkDebug(IN EMAElement elem, IN const char* filename, IN int lineno)
#else
RVINTAPI int RVCALLCONV
emaMark(IN EMAElement elem)
#endif
{
    emaObject* ema;
    emaElem* rElem;

    if (elem == NULL) return RV_ERROR_UNKNOWN;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

#if defined(RV_EMA_DEBUG)
    RvEmaLogDebug((&ema->log, "emaMark (%s): Marking %p, refCount=%d (%s:%d)",
             raGetName(ema->ra), rElem, EMA_GET_REFERENCE(rElem), filename, lineno));

    if ((rElem->debug1 != EMA_DEBUG_BYTES) || (rElem->debug2 != EMA_DEBUG_BYTES))
    {
        RvEmaLogExcep((&ema->log, "emaMark (%s): Someone is messing with memory %p", raGetName(ema->ra), rElem));
    }
#else
    RvEmaLogDebug((&ema->log,
        "emaMark (%s): Marking %p, refCount=%d", raGetName(ema->ra), rElem, EMA_GET_REFERENCE(rElem)));
#endif

    /* Increase the reference count */
    switch (ema->lockType)
    {
        case emaNoLocks:        break;
        case emaNormalLocks:    RvMutexLock((RvMutex *)((char*)elem + ema->elemSize), ema->logMgr); break;
        case emaLinkedLocks:
            emaLock(*((EMAElement*)((char*)elem + ema->elemSize)));
            emaMark(*((EMAElement*)((char*)elem + ema->elemSize)));
            break;
        case emaLockPointers:
        {
            RvMutex *pLock = *(RvMutex **)((char*)elem + ema->elemSize);
            /* We lock through the pointer */
            RvMutexLock(pLock, ema->logMgr);

            /* make sure the lock didn't change */
            if (pLock != *(RvMutex **)((char*)elem + ema->elemSize))
            {
                /* it changed. unlock it and lock the new pointer */
                RvMutexUnlock(pLock, ema->logMgr);
                RvMutexLock(*(RvMutex **)((char*)elem + ema->elemSize), ema->logMgr);
            }
            break;
        }
    }

    EMA_INC_REFERENCE(rElem,1);

    switch (ema->lockType)
    {
        case emaNoLocks:        break;
        case emaNormalLocks:    RvMutexUnlock((RvMutex *)((char*)elem + ema->elemSize), ema->logMgr); break;
        case emaLinkedLocks:    emaUnlock(*((EMAElement*)((char*)elem + ema->elemSize))); break;
        case emaLockPointers:   RvMutexUnlock(*(RvMutex **)((char*)elem + ema->elemSize), ema->logMgr); break;
    }

    return 0;
}


/************************************************************************
 * emaMarkLocked
 * purpose: Mark an element in EMA for use, not letting anyone delete
 *          this element until it is release.
 *          This does NOT lock the EMA object.
 * input  : elem    - Element to mark
 * output : none
 * return : RV_TRUE    - When the element exists and was locked
 *          RV_FALSE   - When the element doesn't exist (NULL are was deleted)
 *                       In this case, there's no need to call emaReleaseWhileLocked().
 ************************************************************************/
#if defined(RV_EMA_DEBUG)
RVINTAPI RvBool RVCALLCONV
emaMarkLockedDebug(IN EMAElement elem, IN const char* filename, IN int lineno)
#else
RVINTAPI RvBool RVCALLCONV
emaMarkLocked(IN EMAElement elem)
#endif
{
    emaObject* ema;
    emaElem* rElem;

    if (elem == NULL) return RV_FALSE;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    /* Make sure we've got the element */
    if (ema == NULL) return RV_FALSE;

#if defined(RV_EMA_DEBUG)
    RvEmaLogDebug((&ema->log, "emaMarkLocked (%s): Marking %p, refCount=%d (%s:%d)",
             raGetName(ema->ra), rElem, EMA_GET_REFERENCE(rElem), filename, lineno));

    if ((rElem->debug1 != EMA_DEBUG_BYTES) || (rElem->debug2 != EMA_DEBUG_BYTES))
    {
        RvEmaLogExcep((&ema->log, "emaMarkLocked (%s): Someone is messing with memory %p", raGetName(ema->ra), rElem));
    }

    /* make sure this is really locked */
    if (ema->lockType == emaLockPointers)
    {
#if (RV_THREADNESS_TYPE != RV_THREADNESS_SINGLE)
        int numLocks = 0;
        RvMutex *pLock = *(RvMutex **)((char*)elem + ema->elemSize);
        /* Check how many times it is locked */
        RvMutexGetLockCounter(pLock, ema->logMgr, &numLocks);
        if (numLocks == 0)
        {
            /* drek. print an exception, but continue anyway, maybe we'll get away with this */
            RvEmaLogExcep((&ema->log, "emaMarkLocked (%s): Called when not locked (%p)", raGetName(ema->ra), rElem));
        }
#endif /* (RV_THREADNESS_TYPE != RV_THREADNESS_SINGLE) */
    }
    else if (ema->lockType == emaLinkedLocks)
	{
		EMAElement  parent = NULL;
		RvStatus    status = RV_FALSE;

		/* We lock the parent */
		parent = *((EMAElement*)((char*)elem + ema->elemSize));
		if (parent != NULL)
			status = emaMarkLocked(parent);
		else
			status = RV_FALSE;

		if (status == RV_FALSE)
		{
			RvEmaLogWarning((&ema->log, "emaMarkLocked (%s): Couldn't mark parent=%p of %p for some reason",
					 raGetName(ema->ra), parent, elem));
		}
    }
#else
    RvEmaLogDebug((&ema->log,
        "emaMarkLocked (%s): Marking %p, refCount=%d", raGetName(ema->ra), rElem, EMA_GET_REFERENCE(rElem)));
#endif

    /* Increase the reference count */
    EMA_INC_REFERENCE(rElem,1);
    return RV_TRUE;
}


/************************************************************************
 * emaRelease
 * purpose: Release an element in EMA after it was marked using
 *          emaMark(), returning an indication if this element
 *          still exists.
 *          This automatically locks the EMA object.
 * input  : elem    - Element to mark
 * output : none
 * return : RV_TRUE    if element still exists
 *          RV_FALSE   if element was deleted and is not valid anymore
 *          Negative value on failure
 ************************************************************************/
#if defined(RV_EMA_DEBUG)
RVINTAPI int RVCALLCONV
emaReleaseDebug(IN EMAElement elem, IN const char* filename, IN int lineno)
#else
RVINTAPI int RVCALLCONV
emaRelease(IN EMAElement elem)
#endif
{
    emaObject* ema;
    emaElem* rElem;
    int elemExists;
    RvBool bRaiseIdle = RV_FALSE;

    if (elem == NULL) return RV_ERROR_UNKNOWN;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

#if defined(RV_EMA_DEBUG)
    RvEmaLogDebug((&ema->log,
        "emaRelease (%s): Releasing %p, refCount=%d (%s:%d)",
             raGetName(ema->ra), rElem, EMA_GET_REFERENCE(rElem), filename, lineno));

    if (EMA_GET_REFERENCE(rElem) == 0)
    {
        RvEmaLogExcep((&ema->log,
            "emaRelease (%s): Releasing %p (%s:%d) when it's not marked!",
            raGetName(ema->ra), rElem, filename, lineno));
    }

    if ((rElem->debug1 != EMA_DEBUG_BYTES) || (rElem->debug2 != EMA_DEBUG_BYTES))
    {
        RvEmaLogExcep((&ema->log,
            "emaRelease (%s): Someone is messing with memory %p", raGetName(ema->ra), rElem));
    }
#else
    RvEmaLogDebug((&ema->log,
        "emaRelease (%s): Releasing %p, refCount=%d", raGetName(ema->ra), rElem, EMA_GET_REFERENCE(rElem)));
#endif

    /* Decrease the reference count */
    switch (ema->lockType)
    {
        case emaNoLocks:        break;
        case emaNormalLocks:    RvMutexLock((RvMutex *)((char*)elem + ema->elemSize), ema->logMgr); break;
        case emaLinkedLocks:
            emaLock(*((EMAElement*)((char*)elem + ema->elemSize)));
            emaRelease(*((EMAElement*)((char*)elem + ema->elemSize)));
            break;
        case emaLockPointers:
        {
            RvMutex *pLock = *(RvMutex **)((char*)elem + ema->elemSize);
            /* We lock through the pointer */
            RvMutexLock(pLock, ema->logMgr);

            /* make sure the lock didn't change */
            if (pLock != *(RvMutex **)((char*)elem + ema->elemSize))
            {
                /* it changed. unlock it and lock the new pointer */
                RvMutexUnlock(pLock, ema->logMgr);
                RvMutexLock(*(RvMutex **)((char*)elem + ema->elemSize), ema->logMgr);
            }
            break;
        }
    }

    EMA_DEC_REFERENCE(rElem,1);
    elemExists = ((rElem->flags & EMA_ALREADY_DELETED) == 0);

    /* Delete element if we're done with it */
    if (rElem->flags == EMA_ALREADY_DELETED)
    {
        int location = raGetByPtr(ema->ra, rElem);

        /* clean up the element, if needed */
        if (ema->pIdleEv != NULL)
        {
            /* mark the element so we don't get to a loop the next time someoune calls lock and unlock */
            EMA_INC_REFERENCE(rElem,1);
            ema->pIdleEv(elem, RV_TRUE);
            EMA_DEC_REFERENCE(rElem,1);
        }
        /* now remove it */
        emaDeleteElement(ema, rElem, location, RV_TRUE, "emaRelease");
    }
    /* Check if we should raise the idle state */
    else if (rElem->flags == EMA_RAISE_IDLE)
    {
        /* leave the element marked */
        rElem->flags &= ~EMA_RAISE_IDLE;
        bRaiseIdle = RV_TRUE;
    }

    switch (ema->lockType)
    {
        case emaNoLocks:        break;
        case emaNormalLocks:    RvMutexUnlock((RvMutex *)((char*)elem + ema->elemSize), ema->logMgr); break;
        case emaLinkedLocks:
            emaUnlock(*((EMAElement*)((char*)elem + ema->elemSize)));
            break;
        case emaLockPointers:   RvMutexUnlock(*(RvMutex **)((char*)elem + ema->elemSize), ema->logMgr); break;
    }

    if (bRaiseIdle)
    {
        ema->pIdleEv(elem, RV_FALSE);
    }

    return elemExists;
}


/************************************************************************
 * emaLockedRelease
 * purpose: Release an element in EMA after it was marked using
 *          emaMark(), returning an indication if this element
 *          still exists.
 *          This does NOT lock the EMA object.
 * input  : elem    - Element to mark
 * output : none
 * return : RV_TRUE    if element still exists
 *          RV_FALSE   if element was deleted and is not valid anymore
 *          Negative value on failure
 ************************************************************************/
#if defined(RV_EMA_DEBUG)
RVINTAPI int RVCALLCONV
emaReleaseLockedDebug(IN EMAElement elem, IN const char* filename, IN int lineno)
#else
RVINTAPI int RVCALLCONV
emaReleaseLocked(IN EMAElement elem)
#endif
{
    emaObject* ema;
    emaElem* rElem;
    int elemExists;
    RvBool bRaiseIdle = RV_FALSE;

    if (elem == NULL) return RV_ERROR_UNKNOWN;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

#if defined(RV_EMA_DEBUG)
    RvEmaLogDebug((&ema->log,
        "emaReleaseLocked (%s): Releasing %p, refCount=%d (%s:%d)",
             raGetName(ema->ra), rElem, EMA_GET_REFERENCE(rElem), filename, lineno));

    if (EMA_GET_REFERENCE(rElem) == 0)
    {
        RvEmaLogExcep((&ema->log,
            "emaReleaseLocked (%s): Releasing %p (%s:%d) when it's not marked!",
            raGetName(ema->ra), rElem, filename, lineno));
    }

    if ((rElem->debug1 != EMA_DEBUG_BYTES) || (rElem->debug2 != EMA_DEBUG_BYTES))
    {
        RvEmaLogExcep((&ema->log,
            "emaReleaseLocked (%s): Someone is messing with memory %p", raGetName(ema->ra), rElem));
    }

    /* make sure this is really locked */
    if (ema->lockType == emaLockPointers)
    {
#if (RV_THREADNESS_TYPE != RV_THREADNESS_SINGLE)
        int numLocks = 0;
        RvMutex *pLock = *(RvMutex **)((char*)elem + ema->elemSize);
        /* Check how many times it is locked */
        RvMutexGetLockCounter(pLock, ema->logMgr, &numLocks);
        if (numLocks == 0)
        {
            /* drek. print an exception, but continue anyway, maybe we'll get away with this */
            RvEmaLogExcep((&ema->log, "emaReleaseLocked (%s): Called when not locked (%p)", raGetName(ema->ra), rElem));
        }
#endif /* (RV_THREADNESS_TYPE != RV_THREADNESS_SINGLE) */
    }
    else if (ema->lockType == emaLinkedLocks)
	{
		/* We lock the parent */
		int result = RV_ERROR_UNKNOWN;
		EMAElement  parent = *((EMAElement*)((char*)elem + ema->elemSize));
		if (parent != NULL)
			result = emaReleaseLocked(parent);

		if (result < 0)
		{
			RvEmaLogWarning((&ema->log, "emaReleaseLocked (%s): Couldn't release parent=%p of %p for some reason",
					 raGetName(ema->ra), parent, elem));
			elemExists = result;
		}
    }
#else
    RvEmaLogDebug((&ema->log,
        "emaReleaseLocked (%s): Releasing %p, refCount=%d", raGetName(ema->ra), rElem, EMA_GET_REFERENCE(rElem)));
#endif

    /* Decrease the reference count */
    EMA_DEC_REFERENCE(rElem,1);
    elemExists = ((rElem->flags & EMA_ALREADY_DELETED) == 0);

    /* Delete element if we're done with it */
    if (rElem->flags == EMA_ALREADY_DELETED)
    {
        int location = raGetByPtr(ema->ra, rElem);

        /* clean up the element, if needed */
        if (ema->pIdleEv != NULL)
        {
            /* mark the element so we don't get to a loop the next time someoune calls lock and unlock */
            EMA_INC_REFERENCE(rElem,1);
            ema->pIdleEv(elem, RV_TRUE);
            EMA_DEC_REFERENCE(rElem,1);
        }
        /* now remove it */
        emaDeleteElement(ema, rElem, location, RV_TRUE, "emaRelease");
    }

    /* Check if we should raise the idle state */
    else if (rElem->flags == EMA_RAISE_IDLE)
    {
        /* leave the element marked */
        rElem->flags &= ~EMA_RAISE_IDLE;
        bRaiseIdle = RV_TRUE;
    }

    if (bRaiseIdle)
    {
        ema->pIdleEv(elem, RV_FALSE);
    }

    return elemExists;
}

/************************************************************************
 * emaWasDeleted
 * purpose: Check if an element in EMA was deleted after a call to
 *          emaMark().
 * input  : elem    - Element to check
 * output : none
 * return : RV_TRUE    if element was deleted
 *          RV_FALSE   if element still exists
 ************************************************************************/
RVINTAPI RvBool RVCALLCONV
emaWasDeleted(IN EMAElement elem)
{
    emaElem* rElem;
#if (RV_LOGMASK & RV_LOGLEVEL_DEBUG) || defined(RV_EMA_DEBUG)
    emaObject *ema;
#endif

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
#if (RV_LOGMASK & RV_LOGLEVEL_DEBUG) || defined(RV_EMA_DEBUG)
    ema = rElem->ema;
#endif

    RvEmaLogDebug((&ema->log, "emaWasDeleted (%s): Checking %p (refCount=%d)",
             raGetName(ema->ra), rElem, EMA_GET_REFERENCE(rElem)));

#if defined(RV_EMA_DEBUG)
    if ((rElem->debug1 != EMA_DEBUG_BYTES) || (rElem->debug2 != EMA_DEBUG_BYTES))
    {
        RvEmaLogExcep((&ema->log, "emaWasDeleted (%s): Someone is messing with memory %p",
            raGetName(ema->ra), rElem));
    }
#endif

    /* Check if element was deleted */
    return ((rElem->flags & EMA_ALREADY_DELETED) != 0);
}


/************************************************************************
 * emaGetLockCount
 * purpose: Get the lock count for an element in EMA (must be locked).
 * input  : elem    - Element to get the locks for
 * output : none
 * return : number of locks
 ************************************************************************/
RVINTAPI int RVCALLCONV
emaGetLockCount(IN EMAElement elem)
{
    emaElem* rElem;
#if (RV_LOGMASK & RV_LOGLEVEL_DEBUG) || defined(RV_EMA_DEBUG)
    emaObject *ema;
#endif
    int locks;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
#if (RV_LOGMASK & RV_LOGLEVEL_DEBUG) || defined(RV_EMA_DEBUG)
    ema = rElem->ema;
#endif

    locks = EMA_GET_LOCKS(rElem);

    if (locks == 0)
    {
        RvEmaLogExcep((&ema->log, "emaGetLockCount (%s): Lock count for %p is zero!",
                 raGetName(ema->ra), rElem));
    }
    else
    {
        RvEmaLogDebug((&ema->log, "emaGetLockCount (%s): Getting for %p count=%d",
                 raGetName(ema->ra), rElem, locks));
    }

#if defined(RV_EMA_DEBUG)
    if ((rElem->debug1 != EMA_DEBUG_BYTES) || (rElem->debug2 != EMA_DEBUG_BYTES))
    {
        RvEmaLogExcep((&ema->log, "emaGetLockCount (%s): Someone is messing with memory %p",
            raGetName(ema->ra), rElem));
    }
#endif

    /* Check if element was deleted */
    return locks;
}


/************************************************************************
 * emaPrepareForCallback
 * purpose: Prepare an element in EMA for use in a callback to the app.
 *          This function will make sure the element is unlocked the necessary
 *          number of times, and then marked once (so the app won't delete
 *          this element).
 *          emaReturnFromCallback() should be called after the callback,
 *          with the return value of this function.
 * input  : elem    - Element to prepare
 * output : none
 * return : Number of times the element was locked on success
 *          Negative value on failure
 ************************************************************************/
#if defined(RV_EMA_DEBUG)
RVINTAPI int RVCALLCONV
emaPrepareForCallbackDebug(IN EMAElement elem, IN const char* filename, IN int lineno)
#else
RVINTAPI int RVCALLCONV
emaPrepareForCallback(IN EMAElement elem)
#endif
{
    emaObject*  ema;
    emaElem*    rElem;
    int         numLocks = 0;

    if (elem == NULL) return RV_ERROR_UNKNOWN;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    /* Make sure we've got an element */
    if (ema == NULL) return RV_ERROR_UNKNOWN;

#if defined(RV_EMA_DEBUG)
    if (ema->lockType == emaLockPointers)
    {
        RvMutex *pLock = *(RvMutex **)((char*)elem + ema->elemSize);
        /* Check how many times it is locked */
        RvMutexGetLockCounter(pLock, ema->logMgr, &numLocks);
#if (RV_THREADNESS_TYPE != RV_THREADNESS_SINGLE)
        if (numLocks == 0)
        {
            /* print an exception, but continue anyway, maybe we'll get away with this */
            RvEmaLogExcep((&ema->log, "emaPrepareForCallback (%s): Called when not locked (%p)", raGetName(ema->ra), rElem));
        }
#endif
    }
    else if ((EMA_GET_LOCKS(rElem) == 0) && (ema->lockType != emaLinkedLocks))
    {
        RvEmaLogExcep((&ema->log,
            "emaPrepareForCallback (%s): on %p (%s:%d) when not locked!",
                 raGetName(ema->ra), rElem, filename, lineno));
    }
#endif

    /* Increase the reference count */
    EMA_INC_REFERENCE(rElem,1);

    /* See where the do we have to unlock */
    switch (ema->lockType)
    {
        case emaNormalLocks:
        {
            /* We unlock the element */
            int i;
            numLocks = EMA_GET_LOCKS(rElem);

            /* First we decrease the number of locks, and then we actually exit them... */
            EMA_DEC_LOCKS(rElem,numLocks);
            for (i = 0; i < numLocks; i++)
            {
                RvMutexUnlock((RvMutex *)((char*)elem + ema->elemSize), ema->logMgr);
            }
            break;
        }
        case emaLinkedLocks:
        {
            /* We must prepare the parent */
            EMAElement  parent = *((EMAElement*)((char*)elem + ema->elemSize));
            if (parent != NULL)
                numLocks = emaPrepareForCallback(parent);
            else
                numLocks = RV_ERROR_UNKNOWN;

            if (numLocks < 0)
            {
                RvEmaLogWarning((&ema->log,
                    "emaPrepareForCallback (%s): Couldn't prepare parent=%p of %p for some reason",
                         raGetName(ema->ra), parent, elem));
            }
            break;
        }
        case emaLockPointers:
        {
            /* We unlock through the pointer */
            RvMutex *pLock = *(RvMutex **)((char*)elem + ema->elemSize);
            int i;

            RvMutexGetLockCounter(pLock, ema->logMgr, &numLocks);
            for (i = 0; i < numLocks; i++)
            {
                RvMutexUnlock(pLock, ema->logMgr);
            }
            break;
        }
        default:
            numLocks = 0;
            break;
    }

#if defined(RV_EMA_DEBUG)
    RvEmaLogDebug((&ema->log, "emaPrepareForCallback (%s): on %p - locked %d times (%s:%d)",
             raGetName(ema->ra), rElem, numLocks, filename, lineno));
#endif
    return numLocks;
}


/************************************************************************
 * emaReturnFromCallback
 * purpose: Make sure the EMA element knows it has returned from a
 *          callback. This function will ensure that the element is
 *          locked again with the specified number of times. It will also
 *          release the element, and if timersLocked==0, and the element
 *          was deleted by the app in the callback, the element will also
 *          be permanently deleted.
 * input  : elem    - Element to prepare
 * output : none
 * return : RV_TRUE    if element still exists
 *          RV_FALSE   if element was deleted and is not valid anymore
 *          Negative value on failure
 ************************************************************************/
#if defined(RV_EMA_DEBUG)
RVINTAPI int RVCALLCONV
emaReturnFromCallbackDebug(IN EMAElement elem, IN int timesLocked, IN const char* filename, IN int lineno)
#else
RVINTAPI int RVCALLCONV
emaReturnFromCallback(IN EMAElement elem, IN int timesLocked)
#endif
{
    emaObject*  ema;
    emaElem*    rElem;
    int         status = RV_ERROR_UNKNOWN;

    if (elem == NULL) return RV_ERROR_UNKNOWN;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    /* Make sure we've got an element */
    if (ema == NULL) return RV_ERROR_UNKNOWN;

#if defined(RV_EMA_DEBUG)
    RvEmaLogDebug((&ema->log, "emaReturnFromCallback (%s): on %p, %d times (%s:%d)",
             raGetName(ema->ra), rElem, timesLocked, filename, lineno));
#endif

    /* See where the do we have to unlock */
    switch (ema->lockType)
    {
        case emaNormalLocks:
        {
            /* We unlock the element */
            int i;

            /* First we should lock, and only then: increase the number of locks */
            for (i = 0; i < timesLocked; i++)
            {
                RvMutexLock((RvMutex *)((char*)elem + ema->elemSize), ema->logMgr);
            }
            EMA_INC_LOCKS(rElem,timesLocked);
            status = 0;
            break;
        }
        case emaLinkedLocks:
        {
            /* We must prepare the parent */
            EMAElement  parent = *((EMAElement*)((char*)elem + ema->elemSize));
            if (parent != NULL)
                status = emaReturnFromCallback(parent, timesLocked);
            else
                status = RV_ERROR_UNKNOWN;

            if (status < 0)
            {
                RvEmaLogWarning((&ema->log, "emaReturnFromCallback (%s): Couldn't return parent=%p of %p for some reason",
                         raGetName(ema->ra), parent, elem));
            }
            break;
        }
        case emaLockPointers:
        {
            int i;

            if (timesLocked > 0)
            {
                RvMutex *pLock = *(RvMutex **)((char*)elem + ema->elemSize);
                /* We lock through the pointer */
                RvMutexLock(pLock, ema->logMgr);

                /* make sure the lock didn't change */
                if (pLock != *(RvMutex **)((char*)elem + ema->elemSize))
                {
                    /* it changed. unlock it and lock the new pointer */
                    RvMutexUnlock(pLock, ema->logMgr);
                    RvMutexLock(*(RvMutex **)((char*)elem + ema->elemSize), ema->logMgr);
                }

                /* First we should lock, and only then: increase the number of locks */
                for (i = 1; i < timesLocked; i++)
                {
                    RvMutexLock(*(RvMutex **)((char*)elem + ema->elemSize), ema->logMgr);
                }
            }
            break;
        }
        default:
            break;
    }

    /* Decrease the reference count */
    EMA_DEC_REFERENCE(rElem,1);

    /* Delete element if we're done with it */
    if (rElem->flags == EMA_ALREADY_DELETED)
    {
        int location = raGetByPtr(ema->ra, rElem);

        /* clean up the element, if needed */
        if (ema->pIdleEv != NULL)
        {
            /* mark the element so we don't get to a loop the next time someoune calls lock and unlock */
            EMA_INC_REFERENCE(rElem,1);
            ema->pIdleEv(elem, RV_TRUE);
            EMA_DEC_REFERENCE(rElem,1);
        }
        /* now remove it */
        emaDeleteElement(ema, rElem, location, RV_TRUE, "emaRelease");
    }
    else
    {
        /* We didn't delete this element... */
        status = RV_TRUE;
    }

    return status;
}


/************************************************************************
 * emaSetApplicationHandle
 * purpose: Set the application handle of an element in EMA
 * input  : elem        - Element in EMA
 *          appHandle   - Application's handle for the element
 * output : none
 * return : Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
emaSetApplicationHandle(IN EMAElement elem, IN void* appHandle)
{
    emaElem* rElem;

    if (elem == NULL) return RV_ERROR_UNKNOWN;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    rElem->appHandle = appHandle;

#if defined(RV_EMA_DEBUG)
    {
        /* Make sure element is not vacant */
        emaObject* ema;
        int location;

        ema = rElem->ema;
        location = raGetByPtr(ema->ra, rElem);

        if (raElemIsVacant(ema->ra, location))
        {
            RvEmaLogExcep((&ema->log, "emaSetApplicationHandle (%s): Element %d,%p is vacant",
                     raGetName(ema->ra), location, rElem));
        }
    }
#endif

    return 0;
}


/************************************************************************
 * emaGetApplicationHandle
 * purpose: Get the application's handle of an element in EMA
 * input  : elem        - Element in EMA
 * output : appHandle   - Application's handle for the element
 * return : Pointer to the application handle
 *          NULL on failure
 ************************************************************************/
RVINTAPI void* RVCALLCONV
emaGetApplicationHandle(IN EMAElement elem)
{
    emaElem* rElem;

    if (elem == NULL) return NULL;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);

#if defined(RV_EMA_DEBUG)
    {
        /* Make sure element is not vacant */
        emaObject* ema;
        int location;

        ema = rElem->ema;
        location = raGetByPtr(ema->ra, rElem);

        if (raElemIsVacant(ema->ra, location))
        {
            RvEmaLogExcep((&ema->log, "emaGetApplicationHandle (%s): Element %d,%p is vacant",
                     raGetName(ema->ra), location, rElem));
        }
    }
#endif

    return rElem->appHandle;
}


/************************************************************************
 * emaGetType
 * purpose: Return the type of the element inside the EMA object.
 *          This is the type given in emaConstruct().
 * input  : elem    - Element in EMA
 * output : none
 * return : The element's type on success
 *          0 on failure
 ************************************************************************/
RVINTAPI RvUint32 RVCALLCONV
emaGetType(IN EMAElement elem)
{
    emaObject* ema;
    emaElem* rElem;

    if (elem == NULL) return 0;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    return ema->type;
}


/************************************************************************
 * emaGetUserData
 * purpose: Return the user related data of the element inside the EMA
 *          object. This is the userData given in emaConstruct().
 * input  : elem    - Element in EMA
 * output : none
 * return : The element's user data pointer on success
 *          NULL on failure
 ************************************************************************/
RVINTAPI void* RVCALLCONV
emaGetUserData(IN EMAElement elem)
{
    emaObject* ema;
    emaElem* rElem;

    if (elem == NULL) return NULL;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    return ema->userData;
}

/************************************************************************
 * emaGetUserDataByInstance
 * purpose: Return the user related data inside the EMA object, by the
 *          EMA instance returned by emaConstruct().
 *          This is the userData given in emaConstruct().
 * input  : emaH   - handle to the EMA
 * output : none
 * return : The user data pointer on success
 *          NULL on failure
 ************************************************************************/
//static void* emaGetUserDataByInstance(IN HEMA emaH)
//{
//    emaObject* ema = (emaObject *)emaH;
//
//    if (emaH == NULL) return NULL;
//
//    /* Find out our information */
//    return ema->userData;
//}

/************************************************************************
 * emaGetInstance
 * purpose: Return the instance of this EMA element.
 *          This is the instance given in emaConstruct().
 * input  : elem    - Element in EMA
 * output : none
 * return : The element's instance on success
 *          NULL on failure
 ************************************************************************/
RVINTAPI void const* RVCALLCONV
emaGetInstance(IN EMAElement elem)
{
    emaObject* ema;
    emaElem* rElem;

    if (elem != NULL)
    {
        /* Find out our element information */
        rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
        ema = rElem->ema;

        if (ema != NULL)
            return ema->instance;
    }

    return NULL;
}


/************************************************************************
 * emaSetIdleEvent
 * purpose: Set a function to be called when an object is idle
 * input  : emaH        - Handle of the EMA object
 *          pIdleEv     - Event to be called on idle element
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
emaSetIdleEvent(
    IN HEMA     emaH,
    IN emaEvElementIdle  pIdleEv)
{
    emaObject* ema = (emaObject *)emaH;
    if (ema == NULL) return RV_ERROR_NULLPTR;

    ema->pIdleEv = pIdleEv;
    return RV_OK;
}


/************************************************************************
 * emaGetObject
 * purpose: Return the EMA object this element is in
 * input  : elem    - Element in EMA
 * output : none
 * return : The element's EMA object on success
 *          NULL on failure
 ************************************************************************/
//static
//HEMA emaGetObject(IN EMAElement elem)
//{
//    emaElem* rElem;
//
//    if (elem == NULL) return NULL;
//
//    /* Find out our element information */
//    rElem = (emaElem *)((char*)elem - SIZEOF_EMAELEM);
//
//    return (HEMA)rElem->ema;
//}


/************************************************************************
 * emaDoAll
 * purpose: Call a function on all used elements in EMA
 * input  : emaH        - Handle of the EMA object
 *          func        - Function to execute on all elements
 *          param       - Context to use when executing the function
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
emaDoAll(
    IN HEMA     emaH,
    IN EMAFunc  func,
    IN void*    param)
{
    emaObject* ema = (emaObject *)emaH;
    int        i;

    /* Pass through all the elements, executing the functions on those
       which are used */
    for (i = 0; i < raMaxSize(ema->ra); i++)
    if (!raElemIsVacant(ema->ra, i))
    {
        char *elem  = (char *)raGet(ema->ra, i);

        /* We change the context by the return value of the function */
        param = func(elem + /*sizeof(emaElem)*/SIZEOF_EMAELEM, param);
    }

    return 0;
}


/************************************************************************
 * emaGetNext
 * purpose: Get the next used element in EMA.
 *          This function can be used to implement search or "doall"
 *          functions on EMA.
 * input  : emaH        - Handle of the EMA object
 *          cur         - Current EMA element whose next we're looking for
 *                        If NULL, then emaGetNext() will return the first
 *                        used element.
 * output : none
 * return : Pointer to the next used element on success
 *          NULL when no more used elements are left
 ************************************************************************/
RVINTAPI EMAElement RVCALLCONV
emaGetNext(
    IN HEMA         emaH,
    IN EMAElement   cur)
{
    emaObject* ema = (emaObject *)emaH;
    int location, curLoc;

    /* Find out our element information */
    if (cur != NULL)
    {
        emaElem* rElem = (emaElem *)((char*)cur - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
        curLoc = raGetByPtr(ema->ra, rElem);
    }
    else
        curLoc = RV_ERROR_UNKNOWN;

    /* Find the location */
    location = raGetNext(ema->ra, curLoc);

    if (location >= 0)
        return (EMAElement)((char*)raGet(ema->ra, location) + /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    else
        return NULL;
}


/************************************************************************
 * emaGetIndex
 * purpose: Returns the index of the element in the ema
 * input  : elem    - Element in EMA
 * output : none
 * return : The element's index on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
emaGetIndex(IN EMAElement elem)
{
    emaObject* ema;
    emaElem* rElem;

    if (elem == NULL) return RV_ERROR_UNKNOWN;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    /* Find the location */
    return raGetByPtr(ema->ra, rElem);
}


/************************************************************************
 * emaGetByIndex
 * purpose: Returns the ema element by its index.
 *          This function does not check if the element is vacant or not.
 * input  : emaH    - Handle of the EMA object
 *          index   - Index of element searched
 * output : none
 * return : Pointer to the element on success
 *          NULL on failure
 ************************************************************************/
RVINTAPI EMAElement RVCALLCONV
emaGetByIndex(
    IN HEMA emaH,
    IN int  index)
{
    emaObject* ema;
    RvBool isVacant;
    char * pRA = NULL;

    ema = (emaObject *)emaH;

    if ((index < 0) || (index >= raMaxSize(ema->ra)))
        return NULL;

    RvLockGet(&ema->lock, ema->logMgr);
    isVacant = raElemIsVacant(ema->ra, index);
    if (!isVacant)
        pRA = (char*)raGet(ema->ra, index);
    RvLockRelease(&ema->lock, ema->logMgr);

    if (pRA != NULL)
        return (EMAElement)(pRA + /*sizeof(emaElem)*/SIZEOF_EMAELEM);

    return NULL;
}


/************************************************************************
 * emaGetStatistics
 * purpose: Get statistics information about EMA.
 *          The number of used items also includes those deleted, but still
 *          marked.
 * input  : emaH        - Handle of the EMA object
 * output : stats       - Statistics information
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
emaGetStatistics(IN HEMA emaH, OUT emaStatistics* stats)
{
    emaObject* ema = (emaObject *)emaH;
    if (ema == NULL) return RV_ERROR_UNKNOWN;

    stats->numMarked = ema->markCount;
    return raGetStatistics(ema->ra, &stats->elems);
}


/************************************************************************
 * emaIsVacant
 * purpose: Returns whether the given object is free for allocation
 * input  : elem    - Element in EMA
 * output : none
 * return : RV_TRUE  - if the elemnt is not allocated
 *          RV_FALSE - otherwise
 ************************************************************************/
RVINTAPI int RVCALLCONV
emaIsVacant(IN EMAElement elem)
{
    emaObject* ema;
    emaElem* rElem;

    if (elem == NULL) return RV_FALSE;

    /* Find out our element information */
    rElem = (emaElem *)((char*)elem - /*sizeof(emaElem)*/SIZEOF_EMAELEM);
    ema = rElem->ema;

    /* Find the location */
    return (!ema);
}


/******************************************************************************
 * emaAddWatchdogResource
 * ----------------------------------------------------------------------------
 * General: Add an EMA instance as a resource handled by the watchdog.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  emaH         - EMA instance handle.
 *         watchdog     - The watchdog instance to add this EMA to.
 *         name         - Name of resource to use.
 *         group        - Group name for this resource.
 *         description  - Short description about this resource.
 *****************************************************************************/
RVINTAPI RvStatus RVCALLCONV
emaAddWatchdogResource(
    IN HEMA         emaH,
    IN RvWatchdog   *watchdog,
    IN const char   *name,
    IN const char   *group,
    IN const char   *description)
{
    emaObject* ema = (emaObject *)emaH;

    return raAddWatchdogResource(ema->ra, watchdog, name, group, description);
}


#ifdef __cplusplus
}
#endif

