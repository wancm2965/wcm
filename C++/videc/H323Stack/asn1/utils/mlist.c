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

#include "ra.h"
#include "mlist.h"

#if (RV_ASN1_LEAN_3G324M == RV_NO)

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 * mListElemHeader
 * This header is added to each element inside the list. It is used to
 * link the lists together.
 ************************************************************************/
typedef struct
{
    void*   next; /* Next element in list (NULL if none) */
    void*   prev; /* Previous element in list (NULL if none) */
    /* data - Beginning of user data held inside the list */
} mListElemHeader;




/************************************************************************
 * mlistConstruct
 * purpose: Create an MLIST object.
 * input  : elemSize            - Size of elements in the MLIST in bytes.
 *          maxNumOfElements    - Number of elements in MLIST.
 *          name                - Name of MLIST (used in log messages).
 *          logMgr              - Log manager to use.
 * output : none
 * return : handle to MLIST constructed on success.
 *          NULL on failure.
 ************************************************************************/
RVINTAPI HLIST RVCALLCONV
mlistConstruct(
    IN int              elemSize,
    IN int              maxNumOfElements,
    IN const RvChar*    name,
    IN RvLogMgr*        logMgr)
{
    return
        (HLIST)raConstruct(elemSize + (int)sizeof(mListElemHeader),
            maxNumOfElements, RV_FALSE, name, logMgr);
}


/************************************************************************
 * mlistDestruct
 * purpose: Free memort acquired by MLIST
 * input  : mList   - MLIST handle
 * output : none
 * return : none
 ************************************************************************/
RVINTAPI void RVCALLCONV
mlistDestruct(IN HLIST mList)
{
    raDestruct((HRA)mList);
}


/************************************************************************
 * mlistAddElement
 * purpose: Add an empty list element. This element is a list by its own.
 *          It can be linked to another list using mlistInsert()
 * input  : mList   - MLIST handle
 * output : none
 * return : Handle of new list element on success
 *          NULL on failure
 ************************************************************************/
RVINTAPI LISTElement RVCALLCONV
mlistAddElement(IN HLIST mList)
{
    mListElemHeader* elem;

    if (raAdd((HRA)mList, (RAElement *)&elem) >= 0)
    {
        elem->next = NULL;
        elem->prev = NULL;
        return (LISTElement)(((char*)elem) + sizeof(mListElemHeader));
    }

    return NULL;
}


/************************************************************************
 * mlistInsert
 * purpose: Insert an element into a list. The inserted item shouldn't
 *          belong to any list (i.e. - it was just created using
 *          mlistAddElement).
 * input  : destListElem    - Destination list element. The inserted
 *                            item is added before or after this element
 *          insertedElem    - The item inserted to the list
 *          insertAfter     - RV_TRUE if element should be inserted after the
 *                            destination element, RV_FALSE if it should be
 *                            inserted before it
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 * note   : Both elements should be in the same MLIST!
 ************************************************************************/
RVINTAPI int RVCALLCONV
mlistInsert(
    IN LISTElement  destListElem,
    IN LISTElement  insertedElem,
    IN RvBool       insertAfter)
{
    mListElemHeader* destItem;
    mListElemHeader* insertedItem;

    destItem = (mListElemHeader *)(((char *)destListElem) - sizeof(mListElemHeader));
    insertedItem = (mListElemHeader *)(((char *)insertedElem) - sizeof(mListElemHeader));

    /* All there is to do in this function is fix the pointers of both given elements */
    if (insertAfter)
    {
        mListElemHeader * nextItem = (mListElemHeader *) destItem->next;
        insertedItem->next = destItem->next;
        insertedItem->prev = destItem;
        destItem->next = insertedItem;
        if(nextItem) nextItem->prev = insertedItem;
    }
    else
    {
        mListElemHeader * prevItem = (mListElemHeader *) destItem->prev;
        insertedItem->next = destItem;
        insertedItem->prev = destItem->prev;
        destItem->prev = insertedItem;
        if(prevItem) prevItem->next = insertedItem;
    }

    return 0;
}


/************************************************************************
 * mlistDeleteElement
 * purpose: Delete a single element from a list, fixing all connection of
 *          that list.
 * input  : mList       - MLIST to use
 *          deletedElem - Element to delete
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
mlistDeleteElement(
    IN HLIST        mList,
    IN LISTElement  deletedElem)
{
    mListElemHeader* deletedItem;

    deletedItem = (mListElemHeader *)(((char *)deletedElem) - sizeof(mListElemHeader));

    /* Fix linked of adjacent elements */
    if (deletedItem->prev != NULL)
        ((mListElemHeader *)deletedItem->prev)->next = deletedItem->next;
    if (deletedItem->next != NULL)
        ((mListElemHeader *)deletedItem->next)->prev = deletedItem->prev;

    /* Delete this element from RA */
    return raDelete((HRA)mList, (RAElement)deletedItem);
}


/************************************************************************
 * mlistDeleteList
 * purpose: Delete a list of connected elements from MLIST
 * input  : mList       - MLIST to use
 *          element     - Element in the list to delete
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
mlistDeleteList(
    IN HLIST        mList,
    IN LISTElement  element)
{
    mListElemHeader* curItem;
    mListElemHeader* nextItem;

    curItem = (mListElemHeader *)(((char *)element) - sizeof(mListElemHeader));

    /* First we go to the head of the list */
    while (curItem->prev != NULL)
        curItem = curItem->prev;

    /* Delete all the items in this list */
    while (curItem != NULL)
    {
        nextItem = curItem->next;
        raDelete((HRA)mList, (RAElement)curItem);
        curItem = nextItem;
    }

    return 0;
}


/************************************************************************
 * mlistNext
 * purpose: Get next element from a list
 * input  : element - Current element in list
 * output : none
 * return : Handle of the next element on success
 *          NULL on failure (when current element is the tail of the list)
 ************************************************************************/
RVINTAPI LISTElement RVCALLCONV
mlistNext(IN LISTElement element)
{
    mListElemHeader* curItem;

    curItem = (mListElemHeader *)(((char *)element) - sizeof(mListElemHeader));

    if (curItem->next != NULL)
        return (LISTElement)(((char*)(curItem->next)) + sizeof(mListElemHeader));

    return NULL;
}


/************************************************************************
 * mlistPrev
 * purpose: Get previous element from a list
 * input  : element - Current element in list
 * output : none
 * return : Handle of the previous element on success
 *          NULL on failure (when current element is the head of the list)
 ************************************************************************/
LISTElement mlistPrev(IN LISTElement element)
{
    mListElemHeader* curItem;

    curItem = (mListElemHeader *)(((char *)element) - sizeof(mListElemHeader));

    if (curItem->prev != NULL)
        return (LISTElement)(((char*)(curItem->prev)) + sizeof(mListElemHeader));

    return NULL;
}




#ifdef __cplusplus
}
#endif


#endif /* (RV_ASN1_LEAN_3G324M == RV_NO) */

