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

#include "transparent.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus*/



/************************************************************************
 * transParentInit
 * purpose: Initialize the parent lock system
 * input  : transGlobals   - handle to the transport globals
 *          numOfLocks     - Ammout of locks to create
 * output : none
 * return : RV_OK on success, negative o.w.
 ************************************************************************/
RvStatus transParentInit(
    IN cmTransGlobals*  transGlobals,
    IN int              numOfLocks)
{
    int i=0, j=0;
    RvStatus res = RV_ERROR_UNKNOWN;
    parentLock * prev = NULL;

    if (RvMemoryAlloc(NULL, numOfLocks*sizeof(parentLock), transGlobals->logMgr,
        (void**)&transGlobals->parentLockArray) != RV_OK)
    {
        /* could not allocate array */
        return RV_ERROR_OUTOFRESOURCES;
    }

    memset(transGlobals->parentLockArray, 0, numOfLocks*sizeof(parentLock));

    /* Create the mutex */
    RvLockConstruct(transGlobals->logMgr, &transGlobals->parentLockFreeListLock);

    /* now create mutexes and the free list */
    transGlobals->parentLockFreeList = &transGlobals->parentLockArray[0];
    prev = &transGlobals->parentLockArray[0];

    for (i = 0; i < numOfLocks; i++)
    {
        res = RvMutexConstruct(transGlobals->logMgr, &transGlobals->parentLockArray[i].mutex);
        if (res != RV_OK)
            break;
        if (i != 0)
        {
            prev->next = &transGlobals->parentLockArray[i];
            prev = &transGlobals->parentLockArray[i];
        }
    }
    if (res != RV_OK)
    {
        /* something went wrong. free everything */
        RvLockDestruct(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        for (j = 0; j < i; j++)
        {
            RvMutexDestruct(&transGlobals->parentLockArray[j].mutex, transGlobals->logMgr);
        }
        RvMemoryFree(transGlobals->parentLockArray, transGlobals->logMgr);
        transGlobals->parentLockArray = NULL;
        transGlobals->parentLockFreeList = NULL;
        return res;
    }

    prev->next = NULL;
    transGlobals->parentLockFreeListTail = prev;
    transGlobals->numParentLocks = numOfLocks;

#ifdef DEBUG_PARENT_LOCKS
    transGlobals->usedParentLocks = 0;
    RvLogDebug(&transGlobals->hLog,
        (&transGlobals->hLog, "ParentLocks: created %d", numOfLocks));
#endif

    return RV_OK;
}


/************************************************************************
 * transParentEnd
 * purpose: End the parent lock system
 * input  : transGlobals   - handle to the transport globals
 * output : none
 * return : none
 ************************************************************************/
void transParentEnd(
    IN cmTransGlobals*  transGlobals)
{
    int i;

    if (transGlobals->numParentLocks <= 0)
        return;

#ifdef DEBUG_PARENT_LOCKS
    if (transGlobals->usedParentLocks != 0)
    {
        RvLogExcep(&transGlobals->hLog,
            (&transGlobals->hLog, "ParentLocks: %d locks still used on destruction", transGlobals->usedParentLocks));
    }
    else
    {
        RvLogDebug(&transGlobals->hLog,
            (&transGlobals->hLog, "ParentLocks: destructed"));
    }
#endif

    RvLockDestruct(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
    for (i = 0; i < transGlobals->numParentLocks; i++)
    {
        RvMutexDestruct(&transGlobals->parentLockArray[i].mutex, transGlobals->logMgr);
    }
    transGlobals->numParentLocks = 0;
    RvMemoryFree(transGlobals->parentLockArray, transGlobals->logMgr);
    transGlobals->parentLockArray = NULL;
    transGlobals->parentLockFreeList = NULL;
}


void returnParentToList(cmTransGlobals* transGlobals, parentLock *parent)
{
    RvInt32 numLocks;

#ifdef DEBUG_PARENT_LOCKS
    transGlobals->usedParentLocks--;
    RvLogDebug(&transGlobals->hLog,
        (&transGlobals->hLog, "ParentLocks: Returned parent %p (used %d of %d)",
            parent, transGlobals->usedParentLocks, transGlobals->numParentLocks));
#endif

    /* make sure this was called by the LAST object deleted */
    RvMutexGetLockCounter(&parent->mutex, transGlobals->logMgr, &numLocks);
    if (numLocks > 1)
    {
        RvLogExcep(&transGlobals->hLog,
            (&transGlobals->hLog, "ParentLocks: lock %p returned while locked %d times", parent, numLocks));
    }

    /* check if we have a list */
    if (transGlobals->parentLockFreeList == NULL)
    {
        transGlobals->parentLockFreeList = parent;
        transGlobals->parentLockFreeListTail = parent;
    }
    else
    {
        transGlobals->parentLockFreeListTail->next = parent;
        transGlobals->parentLockFreeListTail = parent;
    }
    /* last element in either case, set next to NULL */
    parent->next = NULL;
}

/************************************************************************
 * transParentNew
 * purpose: Set a new lock pointer to the given EMA element
 * input  : hTransport     - handle to the transport globals
 *          emaElem        - The EMA element getting a new lock
 * output : none
 * return : RV_OK on success, negative o.w.
 ************************************************************************/
RvStatus transParentNew(
    IN HAPPTRANS    hTransport,
    IN void*        emaElem)
{
    cmTransGlobals* transGlobals = (cmTransGlobals*)hTransport;
    parentLock *parent = NULL;

    RvLockGet(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);

    /* check if there was previously a parent lock on the element */
    if (emaGetLockPointer(emaElem, (void**)&parent) != RV_OK)
    {
        /* bad element or cannot be linked. return error */
        RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        return RV_ERROR_UNKNOWN;
    }

    /* check if we already had a parent */
    if (parent != NULL)
    {
        /* this element no longer points to this element. see if we should delete it */
        parent->refCounter--;
        if (parent->refCounter == 0)
        {
            /* Wait! this lock is going to be freed anyway. just keep on using it */
            parent->refCounter++;
            RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
            return RV_OK;
        }
    }

    /* get a new lock */
    if (transGlobals->parentLockFreeList == NULL)
    {
        /* no more locks! */
#ifdef DEBUG_PARENT_LOCKS
            RvLogError(&transGlobals->hLog,
                (&transGlobals->hLog, "ParentLocks: No more locks (used %d)", transGlobals->usedParentLocks));
#endif
        RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        return RV_ERROR_UNKNOWN;
    }
    /* get a parent lock */
    parent = transGlobals->parentLockFreeList;
    /* try to set it */
    if (emaSetLockPointer(emaElem, (void*)parent) != RV_OK)
    {
        /* free the lock and return an error */
        RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        return RV_ERROR_UNKNOWN;
    }
    /* set was successful, remove it from the list */
    transGlobals->parentLockFreeList = parent->next;

#ifdef DEBUG_PARENT_LOCKS
    transGlobals->usedParentLocks++;
    RvLogDebug(&transGlobals->hLog,
        (&transGlobals->hLog, "ParentLocks: Used parent %p (used %d of %d)",
            parent, transGlobals->usedParentLocks, transGlobals->numParentLocks));
#endif

    parent->refCounter++;
    RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
    return RV_OK;
}


/************************************************************************
 * transParentChildSafetyLock
 * purpose: Lock the parent lock another time to be extra safe, also
 *          outputs a pointer to the parent.
 * input  : hTransport     - handle to the transport globals
 *          emaElem        - The EMA whose parent we want safe
 * output : none
 * return : pointer to the parent element.
 ************************************************************************/
void* transParentChildSafetyLock(
    IN HAPPTRANS    hTransport,
    IN void*        emaElem)
{
    cmTransGlobals* transGlobals = (cmTransGlobals*)hTransport;
    parentLock *parent = NULL;

    RvLockGet(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);

    /* check if there was previously a parent lock on the element */
    if ((emaGetLockPointer(emaElem, (void**)&parent) != RV_OK) ||
        (parent == NULL))
    {
        /* bad element or cannot be linked. return error */
        RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        return NULL;
    }

    /* make the lock safe */
    RvMutexLock(&parent->mutex, transGlobals->logMgr);
    parent->refCounter++;

#ifdef DEBUG_PARENT_LOCKS
    RvLogDebug(&transGlobals->hLog,
        (&transGlobals->hLog, "ParentLocks-ChildSafetyLock: parent %p element %p ref %d",
            parent, emaElem, parent->refCounter));
#endif

    RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
    return (void*)parent;
}


/************************************************************************
 * transParentChildSafetyUnlock
 * purpose: Unlock a parent lock another, maybe free it.
 * input  : hTransport     - handle to the transport globals
 *          pParentLock    - parent lock to unlock
 * output : none
 * return : none
 ************************************************************************/
void transParentChildSafetyUnlock(
    IN HAPPTRANS    hTransport,
    IN void*        pParentLock)
{
    cmTransGlobals* transGlobals = (cmTransGlobals*)hTransport;
    parentLock *parent = pParentLock;

    if (parent == NULL) return;

    RvLockGet(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);

#ifdef DEBUG_PARENT_LOCKS
    {
        RvInt32 mCount = 0;
        RvMutexGetLockCounter(&parent->mutex, transGlobals->logMgr, &mCount);
        if (mCount == 0)
        {
            RvLogExcep(&transGlobals->hLog,
                (&transGlobals->hLog, "ParentLocks: unlocking when not locked"));
            RvMutexLock(&parent->mutex, transGlobals->logMgr);
        }
    }
#endif

    parent->refCounter--;

#ifdef DEBUG_PARENT_LOCKS
    RvLogDebug(&transGlobals->hLog,
        (&transGlobals->hLog, "ParentLocks-ChildSafetyLock: parent %p element %p ref %d",
            parent, parent->refCounter));
#endif

    if (parent->refCounter == 0)
    {
        /* free this element */
        returnParentToList(transGlobals, parent);
    }
    RvMutexUnlock(&parent->mutex, transGlobals->logMgr);

    RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
}


/************************************************************************
 * transParentShare
 * purpose: Set the lock pointer of the first EMA element to that of the
 *          second
 * input  : hTransport     - handle to the transport globals
 *          emaElem        - The EMA element getting a lock
 *          sharingEmaElem - The EMA element sharing its lock
 * output : none
 * return : RV_OK on success, negative o.w.
 ************************************************************************/
RvStatus transParentShare(
    IN HAPPTRANS    hTransport,
    IN void*        emaElem,
    IN void*        sharingEmaElem)
{
    cmTransGlobals* transGlobals = (cmTransGlobals*)hTransport;
    parentLock *newParent = NULL, *oldParent = NULL;

    RvLockGet(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);

    /* make sure the sharing element has a lock */
    if ((emaGetLockPointer(sharingEmaElem, (void**)&newParent) != RV_OK) ||
        (newParent == NULL))
    {
        /* don't touch this with a six foot pole */
        RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        return RV_ERROR_UNKNOWN;
    }

    /* check if there was previously a parent lock on the element */
    if (emaGetLockPointer(emaElem, (void**)&oldParent) != RV_OK)
    {
        /* bad element or cannot be linked. return error */
        RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        return RV_ERROR_UNKNOWN;
    }

    /* check if we really have to do something */
    if (oldParent == newParent)
    {
        /* already correctly set */
        RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        return RV_OK;
    }

    if (emaSetLockPointer(emaElem, (void*)newParent) != RV_OK)
    {
        /* return an error */
        RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        return RV_ERROR_UNKNOWN;
    }

    /* check if we already had a parent */
    if (oldParent != NULL)
    {
        /* this element no longer points to this element. see if we should delete it */
        RvInt32 i, count = emaGetLockCount(emaElem);
        RvInt32 mCount = 0;

        RvMutexGetLockCounter(&oldParent->mutex, transGlobals->logMgr, &mCount);
        if (mCount < count)
        {
            RvLogExcep(&transGlobals->hLog,
                (&transGlobals->hLog, "ParentLocks: unlocking too many times (%d < %d)", mCount, count));
            count = mCount;
        }

        oldParent->refCounter--;
        /* move the locks from the old parent to the new */
        for (i = 0; i < count; i++)
        {
            RvMutexLock(&newParent->mutex, transGlobals->logMgr);
            RvMutexUnlock(&oldParent->mutex, transGlobals->logMgr);
        }
        if (oldParent->refCounter == 0)
        {
            /* free this element */
            returnParentToList(transGlobals, oldParent);
        }
    }

    newParent->refCounter++;
    RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
    return RV_OK;
}


/************************************************************************
 * transParentReplace
 * purpose: Set the lock pointer of several EMA elements to that of the
 *          another
 * input  : hTransport     - handle to the transport globals
 *          emaElems       - The EMA elements array getting a lock (NULL
 *                           terminated), all the elements using the lock
 *          sharingEmaElem - The EMA element sharing its lock
 * output : none
 * return : RV_OK on success, negative o.w.
 ************************************************************************/
RvStatus transParentReplace(
    IN HAPPTRANS    hTransport,
    IN void**       emaElems,
    IN void*        sharingEmaElem)
{
    cmTransGlobals* transGlobals = (cmTransGlobals*)hTransport;
    parentLock *newParent = NULL, *oldParent = NULL;
    void * emaElem;
    void * pParent;

    if (*emaElems == NULL)
        return RV_ERROR_NULLPTR;

    pParent = transParentChildSafetyLock(hTransport, *emaElems);
    RvLockGet(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);

    /* make sure the sharing element has a lock */
    if ((emaGetLockPointer(sharingEmaElem, (void**)&newParent) != RV_OK) ||
        (newParent == NULL))
    {
        /* don't touch this with a six foot pole */
        RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        transParentChildSafetyUnlock(hTransport, pParent);
        return RV_ERROR_UNKNOWN;
    }

    /* check if there was previously a parent lock on the element */
    if (emaGetLockPointer(*emaElems, (void**)&oldParent) != RV_OK)
    {
        /* bad element or cannot be linked. return error */
        RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        transParentChildSafetyUnlock(hTransport, pParent);
        return RV_ERROR_UNKNOWN;
    }

    /* check if we really have to do something */
    if (oldParent == newParent)
    {
        /* already correctly set */
        RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        transParentChildSafetyUnlock(hTransport, pParent);
        return RV_OK;
    }

#ifdef DEBUG_PARENT_LOCKS
    if (oldParent != NULL)
    {
        RvUint32 elemCount = 0;
        void ** p = emaElems;
        while (*p++) elemCount++;
        if (oldParent->refCounter != (elemCount+1))
        {
            RvLogExcep(&transGlobals->hLog,
                (&transGlobals->hLog, "ParentLocks: replacing parent ref-count (%d) does not equal given array (%d+1)",
                oldParent->refCounter, elemCount));
        }
    }
#endif

    emaElem = *emaElems;
    while (emaElem != NULL)
    {
        if (emaSetLockPointer(emaElem, (void*)newParent) != RV_OK)
        {
            /* return an error */
            RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
            transParentChildSafetyUnlock(hTransport, pParent);
            return RV_ERROR_UNKNOWN;
        }
        newParent->refCounter++;
        emaElems++;
        emaElem = *emaElems;
    }

    /* check if we already had a parent */
    if (oldParent != NULL)
    {
        /* this element no longer points to this element. see if we should delete it */
        RvInt32 i, count = 0;
        RvMutexGetLockCounter(&newParent->mutex, transGlobals->logMgr, &count);
        count--; /* child safety lock */
        /* move the locks from the old parent to the new */
        for (i = 0; i < count; i++)
        {
            RvMutexLock(&newParent->mutex, transGlobals->logMgr);
            RvMutexUnlock(&oldParent->mutex, transGlobals->logMgr);
        }
        /* set the ref-counter to 1, so that the child-safety-unlock will free this parent */
        oldParent->refCounter = 1;
    }

    RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
    transParentChildSafetyUnlock(hTransport, pParent);
    return RV_OK;
}


/************************************************************************
 * transParentRemove
 * purpose: Remove the lock pointer from the given EMA element
 * input  : hTransport - handle to the transport globals
 *          emaElem    - The EMA element
 * output : none
 * return : RV_OK on success, negative o.w.
 ************************************************************************/
RvStatus transParentRemove(
    IN HAPPTRANS    hTransport,
    IN void*        emaElem)
{
    cmTransGlobals* transGlobals = (cmTransGlobals*)hTransport;
    parentLock *parent = NULL;

    RvLockGet(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);

    /* check if there was previously a parent lock on the element */
    if ((emaGetLockPointer(emaElem, (void**)&parent) != RV_OK) ||
        (parent == NULL))
    {
        /* bad element or cannot be linked. return error */
        RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
        return RV_ERROR_UNKNOWN;
    }
    parent->refCounter--;
    if (parent->refCounter == 0)
    {
        /* free this element */
        returnParentToList(transGlobals, parent);
    }
    RvLockRelease(&transGlobals->parentLockFreeListLock, transGlobals->logMgr);
    return RV_OK;
}


/************************************************************************
 * emaEvSessionElementIdle
 * purpose: Event function called when a session element is idle
 * input  : elem      - the idle element
 *          bDeleting - if true, we are now deleting the element,
 *                      otherwise, it is just idle
 * output : none
 * return : none
 ************************************************************************/
void RVCALLCONV emaEvSessionElementIdle(EMAElement * elem, RvBool bDeleting)
{
    if (bDeleting)
    {
        cmTransSession * session = (cmTransSession *)elem;
        cmTransGlobals * transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);
        transParentRemove((HAPPTRANS)transGlobals, (void*)session);
    }
}


/************************************************************************
 * emaEvHostElementIdle
 * purpose: Event function called when a session element is idle
 * input  : elem      - the idle element
 *          bDeleting - if true, we are now deleting the element,
 *                      otherwise, it is just idle
 * output : none
 * return : none
 ************************************************************************/
void RVCALLCONV emaEvHostElementIdle(EMAElement * elem, RvBool bDeleting)
{
    if (bDeleting)
    {
        cmTransHost * host = (cmTransHost *)elem;
        cmTransGlobals * transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)host);
        transParentRemove((HAPPTRANS)transGlobals, (void*)host);
    }
}


/************************************************************************
 * emaEvTpktElementIdle
 * purpose: Event function called when a session element is idle
 * input  : elem      - the idle element
 *          bDeleting - if true, we are now deleting the element,
 *                      otherwise, it is just idle
 * output : none
 * return : none
 ************************************************************************/
void RVCALLCONV emaEvTpktElementIdle(EMAElement * elem, RvBool bDeleting)
{
    if (bDeleting)
    {
        transParentRemove((HAPPTRANS)tpktGetTransGlobals((HTPKT)elem), (void*)elem);
    }
}



/************************************************************************
 * transParentChange
 * purpose: Change the lock on the session, the call, and the H.245
 *          connection, if it exists, to the new lock (the host was
 *          already locked before, or we wouldn't be here)
 * input  : transGlobals - Transport globals
 *          host      - The host whose parent lock we want to use
 *          session   - The session whose parent lock we want to change
 * output : none
 * return : none
 ************************************************************************/
void RVCALLCONV transParentChange(cmTransGlobals *transGlobals, cmTransHost *host, cmTransSession *session)
{
    void * pParent = transParentChildSafetyLock((HAPPTRANS)transGlobals, (void*)session);

    transParentShare((HAPPTRANS)transGlobals, (void*)session, (void*)host);
    if (session->H245Connection != NULL)
    {
        transParentShare((HAPPTRANS)transGlobals, (void*)session->H245Connection, (void*)session);
        if (session->H245Connection->hTpkt != NULL)
            transParentShare((HAPPTRANS)transGlobals, (void*)session->H245Connection->hTpkt, (void*)session);
        if (session->H245Connection->h245Listen != NULL)
            transParentShare((HAPPTRANS)transGlobals, (void*)session->H245Connection->h245Listen, (void*)session);
    }
    transGlobals->sessionEventHandlers.cmEvTransParentChanged((HSTRANSSESSION)session,
        (HATRANSSESSION)emaGetApplicationHandle((EMAElement)session));

    transParentChildSafetyUnlock((HAPPTRANS)transGlobals, pParent);
}




#ifdef __cplusplus
}
#endif /* __cplusplus*/
