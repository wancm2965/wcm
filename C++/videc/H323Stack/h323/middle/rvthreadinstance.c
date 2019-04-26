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

#include "rvstdio.h"
#include "rvmemory.h"
#include "rvthread.h"
#include "rvthreadinstance.h"


#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/

/* Information about a specific object */
typedef struct
{
    const RvChar*               objectName; /* Name of the object */
    ThreadInstanceDeleteFunc    deleteFunc; /* Deletion function to use */
    void*                       context; /* Context for the deletion function */

    void*                       objectPtr; /* Created object's pointer */
    RvUint32                    count; /* Number of times this object was constructed */
    RvSize_t                    amountUsed; /* Number of sub-objects we needed */
    RvSize_t                    amountAllocated; /* Number of sub-objects we have allocated */
} ThreadInstanceObjectData;


/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/
#define TMP_LOG NULL


/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/

static void ThreadInstanceExitFunc(
    IN RvThread*    threadInfo,
    IN void*        context,
    IN RvUint32     index);



/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * ThreadInstanceInitObject
 * ----------------------------------------------------------------------------
 * General: Initialize a specific object instance, making sure it is only
 *          constructed once for each thread. Multiple calls to this function
 *          with the same arguments from the same thread will be discarded.
 * Return Value: RV_OK  - if successful.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input  : objectName      - The name of the constructed object.
 *          func            - Function for creation of the object. Only called once for
 *                            each thread (or if the amount of sub-objects needed is not
 *                            satisfied.
 *          context         - Context used for the creation function.
 *          amountNeeded    - Amount of sub-objects the initialized object should
 *                            support. Can be set to 0 if not relevant.
 * Output : objectPtr       - Pointer to the cosntructed object (as returned by the
 *                            creation function func).
 *          count           - Number of times this object was called for construction
 *                            in the current thread.
 *          index           - Index to use for faster access.
 *****************************************************************************/
RvStatus ThreadInstanceInitObject(
    IN  const RvChar*               objectName,
    IN  ThreadInstanceCreateFunc    func,
    IN  void*                       context,
    IN  RvSize_t                    amountNeeded,
    OUT void**                      objectPtr,
    OUT RvUint32*                   count,
    OUT RvUint32*                   index)
{
    ThreadInstanceObjectData* objData;
    RvStatus status;
    RvBool newVarCreated = RV_FALSE;
    RvUint32 tlsIndex;
    RvSize_t missing = 0;
    RvThread* curThread;

    /* First, make sure we have a thread object for this thread. */
    curThread = RvThreadCurrent();
    if (curThread == NULL)
    {
        static int appThreadNum = 0;
        RvChar threadName[32];

        /* Seems like we'll need to create one on our own. */
        status = RvMemoryAlloc(NULL, sizeof(RvThread), TMP_LOG, (void**)&curThread);
        if (status != RV_OK)
            return status;

        status = RvThreadConstructFromUserThread(TMP_LOG, curThread);
        if (status == RV_OK)
        {
            RvThreadSetAutoDelete(curThread, RV_TRUE);
            RvSprintf(threadName, "%d H323TUsr", appThreadNum);
            status = RvThreadSetName(curThread, threadName);
            appThreadNum++;
        }

        if (status != RV_OK)
        {
            RvThreadDestruct(curThread);
            RvMemoryFree(curThread, TMP_LOG);
            return status;
        }
    }

    /* First, let's find the thread index of this object name */
    status = RvThreadFindVar(objectName, TMP_LOG, &tlsIndex);
    if (RvErrorGetCode(status) == RV_THREAD_WARNING_NOTFOUND)
    {
        /* Not found - create a new one */
        status = RvThreadCreateVar(ThreadInstanceExitFunc, objectName, TMP_LOG, &tlsIndex);
        newVarCreated = RV_TRUE;
    }

    if (status != RV_OK)
        return status;

    /* Now that we have an index, let's see if we have a variable already set */
    status = RvThreadGetVar(tlsIndex, TMP_LOG, (void**)&objData);
    if (status != RV_OK)
        return status;

    if (objData == NULL)
    {
        /* Allocate a new object - first time for this thread */
        status = RvMemoryAlloc(NULL, sizeof(ThreadInstanceObjectData), TMP_LOG, (void**)&objData);
        if (status != RV_OK)
            return status;

        memset(objData, 0, sizeof(ThreadInstanceObjectData));
        objData->objectName = objectName;
        objData->context = context;

        /* Call the creation function */
        status = func(objectName, context, amountNeeded,
            &objData->objectPtr, &objData->amountAllocated, &objData->deleteFunc);

        if (status == RV_OK)
            status = RvThreadSetVar(tlsIndex, objData, TMP_LOG);

        if (status != RV_OK)
        {
            /* oops - couldn't create this object - let's ditch it */
            RvMemoryFree(objData, TMP_LOG);
            if (newVarCreated)
                RvThreadDeleteVar(tlsIndex, TMP_LOG);
            return status;
        }
    }
    else
    {
        /* Let's see if there are any vacant objects that we can use */
        if (amountNeeded > 0)
        {
            /* Seems like we're looking for some more space */
            if (objData->amountAllocated >= objData->amountUsed + amountNeeded)
            {
                /* Got more than needed */
                objData->amountUsed += amountNeeded;
            }
            else
            {
                RvSize_t allocated;

                /* Not enough... */
                missing = amountNeeded - (objData->amountAllocated - objData->amountUsed);

                /* Call the creation function to add some more sub-objects */
                status = func(objectName, NULL, missing,
                    &objData->objectPtr, &allocated, NULL);
                if (status != RV_OK)
                    return status;

                /* Now make sure we update the amount of sub-objects */
                objData->amountAllocated += allocated;
                objData->amountUsed += amountNeeded;
            }
        }
    }

    /* We've got another reference for this object */
    objData->count++;

    if (count != NULL)
        *count = objData->count;
    if (objectPtr != NULL)
        *objectPtr = objData->objectPtr;
    if (index != NULL)
        *index = tlsIndex;

    return RV_OK;
}


/******************************************************************************
 * ThreadInstanceGetObject
 * ----------------------------------------------------------------------------
 * General: Get the object that was initialized for the current thread.
 * Return Value: RV_OK  - if successful.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input  : index       - Index of the object as given by ThreadInstanceInitObject().
 * Output : objectPtr   - Object pointer initialized.
 *****************************************************************************/
RvStatus ThreadInstanceGetObject(
    IN  RvUint32    index,
    OUT void**      objectPtr)
{
    ThreadInstanceObjectData* objData;
    RvStatus status;

    /* Now let's get the object of the current thread */
    status = RvThreadGetVar(index, TMP_LOG, (void**)&objData);
    if (status != RV_OK)
        return status;

    if (objData == NULL)
        return RV_ERROR_BADPARAM; /* Seems like we called it from the wrong thread */

    *objectPtr = objData->objectPtr;

    return RV_OK;
}


/******************************************************************************
 * ThreadInstanceFindIndex
 * ----------------------------------------------------------------------------
 * General: Find the index of an object by its name.
 * Return Value: RV_OK  - if successful.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input  : objectName  - Name of the object we're looking for.
 * Output : index       - Index of the object
 *****************************************************************************/
RvStatus ThreadInstanceFindIndex(
    IN  const RvChar*   objectName,
    OUT RvUint32*       index)
{
    RvStatus status;
    RvUint32 tlsIndex;

    /* Let's find the thread index of this object name */
    status = RvThreadFindVar(objectName, TMP_LOG, &tlsIndex);
    if (RvErrorGetCode(status) == RV_THREAD_WARNING_NOTFOUND)
        return RV_ERROR_BADPARAM;

    if (status == RV_OK)
        *index = tlsIndex; /* found it */

    return status;
}


/******************************************************************************
 * ThreadInstanceEndObject
 * ----------------------------------------------------------------------------
 * General: Deinitialize a specific object instance, making sure it is only
 *          destructed once for each thread (when no one is referencing it).
 * Return Value: RV_OK  - if successful.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input  : index   - Index of the object as given by ThreadInstanceInitObject().
 *          amount  - Amount of sub-objects we won't need from now on. Can be set to
 *                    0 if this should be discarded.
 * Output : count   - Number of times this object is still referenced.
 *****************************************************************************/
RvStatus ThreadInstanceEndObject(
    IN  RvUint32        index,
    IN  RvSize_t        amount,
    OUT RvUint32*       count)
{
    ThreadInstanceObjectData* objData;
    RvStatus status;

    /* Now let's get the object of the current thread */
    status = RvThreadGetVar(index, TMP_LOG, (void**)&objData);
    if (status != RV_OK)
        return status;

    if (objData == NULL)
        return RV_ERROR_BADPARAM; /* Seems like we called it from the wrong thread */

    objData->count--;
    if (count != NULL)
        *count = objData->count;

    if (objData->count == 0)
    {
        /* Seems like we have to delete this object */
        status = objData->deleteFunc(objData->objectName, objData->context, objData->objectPtr);
        RvMemoryFree(objData, TMP_LOG);
        RvThreadSetVar(index, NULL, TMP_LOG);
        return status;
    }
    else
    {
        /* Let's get rid of the amount of objects we don't need anymore */
        objData->amountUsed -= amount;
    }

    return RV_OK;
}





/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * ThreadInstanceExitFunc
 * ----------------------------------------------------------------------------
 * General: This exit function is called automatically when the thread is
 *          being destructed.
 * Return Value: RV_OK  - if successful.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input  : threadInfo  - Current thread information (the one being destructed)
 *          context     - Context used for this call
 *          index       - Variable index used
 * Output : none
 *****************************************************************************/
static void ThreadInstanceExitFunc(
    IN RvThread*    threadInfo,
    IN void*        context,
    IN RvUint32     index)
{
    ThreadInstanceObjectData* objData = (ThreadInstanceObjectData *)context;

    RV_UNUSED_ARG(threadInfo);
    RV_UNUSED_ARG(index);

    if (objData == NULL)
        return;

    if (objData->count > 0)
    {
        /* Kill this object */
        objData->deleteFunc(objData->objectName, objData->context, objData->objectPtr);
        RvMemoryFree(objData, TMP_LOG);
    }
}





#ifdef __cplusplus
}
#endif
