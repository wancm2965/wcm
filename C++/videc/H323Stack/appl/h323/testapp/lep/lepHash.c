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

/***********************************************************************
epHash.c

General hash table support.
This is used to hold the different objects of the test application.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "lepHash.h"




#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/


/* Size of hash table of counters */
#define HASH_SIZE (1024)
#define HASH_MASK (0x3ff)

#define HASH_ELEMENT(_h, _n) ((HashBucketObj*)(((RvUint8*)(_h)->element) + ((_h)->elemSize * ((_n) - 1))))
#define HASH_DATA(_h) ((RvUint8*)_h + sizeof(HashBucketObj))
#define BLOCK_SIZE (128)

/* Hash bucket object */
typedef struct
{
    void            *next; /* Pointer to next free object / element in the bucket */
    RvInt32         id; /* Id of the element (used as the key) */
} HashBucketObj;


/* Counters object */
typedef struct
{
    EpObj               *ep; /* Endpoint object used */

    HashBucketObj       *element; /* Array of elements used */
    HashBucketObj       *hash[HASH_SIZE]; /* Hash table holding pointers to counter buckets */

    RvSize_t            elemSize; /* Size of each element (including the bucket header */
    RvSize_t            maxElems; /* Maximum number of elements pre-allocated */
    HashBucketObj       *firstFree; /* First free object */
    HashBucketObj       *lastFree; /* Last free object */

} HashObj;







/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/





/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/
static RvStatus findHashElement(
    IN  HashObj         *hash,
    IN  RvInt32         id,
    OUT RvInt           *bucketIndex,
    OUT HashBucketObj   **element);
static RvStatus elementsAddBlock(
    IN HashObj      *hash,
    IN RvSize_t     numElements);



/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * epHashInit
 * ----------------------------------------------------------------------------
 * General: Initialize a hash object.
 *
 * Return Value: Hash object handle on success, NULL on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep       - Endpoint object to use.
 *         elemSize - Size of elements to store in hash.
 * Output: None.
 *****************************************************************************/
HashHANDLE epHashInit(
    IN EpObj    *ep,
    IN RvSize_t elemSize)
{
    HashObj *hash;
    RvStatus status;

    hash = (HashObj *)ep->cb.epMalloc(ep, sizeof(*hash), __FILE__, __LINE__);
    if (hash == NULL)
        return NULL;

    memset(hash, 0, sizeof(*hash));
    hash->ep = ep;
    hash->elemSize = (RvSize_t)((RvUint8*)RvAlignTo(elemSize, RV_ALIGN_SIZE) + sizeof(HashBucketObj));

    status = elementsAddBlock(hash, BLOCK_SIZE);
    if (status != RV_OK)
    {
        epHashEnd((HashHANDLE)hash);
        return NULL;
    }

    return (HashHANDLE)hash;
}


/******************************************************************************
 * epHashEnd
 * ----------------------------------------------------------------------------
 * General: Destruct a hash object.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hashH    - Hash object handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epHashEnd(
    IN HashHANDLE       hashH)
{
    HashObj *hash = (HashObj *)hashH;
    HashBucketObj *bucket;

    while (hash->element != NULL)
    {
        bucket = HASH_ELEMENT(hash, BLOCK_SIZE+1)->next;

        hash->ep->cb.epFree(hash->ep, hash->element);
        hash->element = bucket;
    }

    return hash->ep->cb.epFree(hash->ep, hash);
}


/******************************************************************************
 * epHashAdd
 * ----------------------------------------------------------------------------
 * General: Add a new element to the hash.
 *          Only elements with new id's can be added.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hashH    - Hash object handle to use.
 *         id       - Id to use for hash element.
 * Output: elem     - Element allocated.
 *****************************************************************************/
RvStatus epHashAdd(
    IN  HashHANDLE      hashH,
    IN  RvInt32         id,
    OUT void            **elem)
{
    HashObj *hash = (HashObj *)hashH;
    RvInt bucketIndex;
    RvStatus status;

    status = findHashElement(hash, id, &bucketIndex, NULL);
    if (status == RV_OK)
    {
        /* Found it - shouldn't happen! */
        status = RV_ERROR_BADPARAM;
    }
    else
    {
        /* New element... */
        status = RV_OK;
        if (hash->firstFree == NULL)
        {
            /* Need more room */
            status = elementsAddBlock(hash, BLOCK_SIZE);
        }
        if (status == RV_OK)
        {
            HashBucketObj *h = hash->firstFree;
            if (hash->firstFree != hash->lastFree)
                hash->firstFree = h->next;
            else
            {
                hash->firstFree = NULL;
                hash->lastFree = NULL;
            }

            h->id = id;
            h->next = hash->hash[bucketIndex];
            hash->hash[bucketIndex] = h;

            *elem = HASH_DATA(h);
        }
    }

    return status;
}


/******************************************************************************
 * epHashRemove
 * ----------------------------------------------------------------------------
 * General: Remove an element from the hash.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hashH    - Hash object handle to use.
 *         id       - Id to use for hash element.
 * Output: None.
 *****************************************************************************/
RvStatus epHashRemove(
    IN  HashHANDLE      hashH,
    IN  RvInt32         id)
{
    HashObj *hash = (HashObj *)hashH;
    RvInt bIdx;
    HashBucketObj *prev, *cur;

    bIdx = (id & HASH_MASK);

    prev = NULL;
    cur = hash->hash[bIdx];

    /* Go through the hash */
    while (cur != NULL)
    {
        if (cur->id == id)
        {
            /* Found it - kill it */
            if (hash->firstFree == NULL)
                hash->firstFree = cur;
            if (hash->lastFree != NULL)
                hash->lastFree->next = cur;
            hash->lastFree = cur;

            if (prev != NULL)
                prev->next = cur->next;
            else
                hash->hash[bIdx] = cur->next;

            cur->next = NULL;
            cur->id = -1;
            return RV_OK;
        }
        prev = cur;
        cur = cur->next;
    }

    return RV_ERROR_NOT_FOUND;
}


/******************************************************************************
 * epHashGet
 * ----------------------------------------------------------------------------
 * General: Get an element from the hash.
 *
 * Return Value: The element found. NULL if not found.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hashH    - Hash object handle to use.
 *         id       - Id to use for hash element.
 * Output: None.
 *****************************************************************************/
void *epHashGet(
    IN  HashHANDLE      hashH,
    IN  RvInt32         id)
{
    HashObj *hash = (HashObj *)hashH;
    HashBucketObj *element;
    void *result = NULL;

    if (findHashElement(hash, id, NULL, &element) == RV_OK)
    {
        result = HASH_DATA(element);
    }

    return result;
}


/******************************************************************************
 * epHashGetAny
 * ----------------------------------------------------------------------------
 * General: Get an element that was allocated in the hash.
 *          This function is SLOW and runs in o(n).
 *
 * Return Value: The element found. NULL if not found.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hashH    - Hash object handle to use.
 *         key      - Starting point of search. Pointer to NULL to begin from
 *                    the first element.
 * Output: key      - Starting point for the next search.
 *****************************************************************************/
void *epHashGetAny(
    IN      HashHANDLE  hashH,
    INOUT   void        **key)
{
    HashObj *hash = (HashObj *)hashH;
    HashBucketObj *elem;
    void *result = NULL;

    if (*key != NULL)
        elem = (HashBucketObj *)*key;
    else
        elem = hash->element;

    while ((elem != NULL) && (result == NULL))
    {
        if (elem->id == 0)
        {
            elem = (HashBucketObj *)elem->next;
        }
        else
        {
            if (elem->id > 0)
            {
                /* Found an element */
                result = HASH_DATA(elem);
            }
            elem = (HashBucketObj *)((RvUint8*)elem + hash->elemSize);
        }
    }

    *key = elem;

    return result;
}








/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * findHashElement
 * ----------------------------------------------------------------------------
 * General: Find an element in the bucket hash.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hash         - Hash object to use.
 *         id           - Id of the searched element.
 * Output: bucketIndex  - Index of bucket this counter name belongs to.
 *         element      - Element bucket found.
 *****************************************************************************/
static RvStatus findHashElement(
    IN  HashObj         *hash,
    IN  RvInt32         id,
    OUT RvInt           *bucketIndex,
    OUT HashBucketObj   **element)
{
    RvInt bIdx;
    HashBucketObj *cur;
    void *data;

    bIdx = (id & HASH_MASK);

    if (bucketIndex != NULL)
        *bucketIndex = bIdx;
    cur = hash->hash[bIdx];

    /* Go through the hash */
    while (cur != NULL)
    {
        data = HASH_DATA(cur);
        if (cur->id == id)
        {
            /* Found it */
            if (element != NULL)
                *element = cur;
            return RV_OK;
        }
        cur = cur->next;
    }

    return RV_ERROR_NOT_FOUND;
}


/******************************************************************************
 * elementsAddBlock
 * ----------------------------------------------------------------------------
 * General: Add another block of elements to the hash table.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hash         - Hash object to use.
 *         numElements  - Number of elements to add.
 * Output: None.
 *****************************************************************************/
static RvStatus elementsAddBlock(
    IN HashObj      *hash,
    IN RvSize_t     numElements)
{
    HashBucketObj *lastBlock;
    HashBucketObj *newCnts;
    RvSize_t i;

    newCnts = (HashBucketObj *)hash->ep->cb.epMalloc(hash->ep, hash->elemSize * numElements + sizeof(HashBucketObj), __FILE__, __LINE__);
    if (newCnts == NULL)
        return RV_ERROR_OUTOFRESOURCES;
    memset(newCnts, 0, hash->elemSize * numElements);
    lastBlock = hash->element;
    hash->element = newCnts;
    HASH_ELEMENT(hash, numElements+1)->next = lastBlock;
    HASH_ELEMENT(hash, numElements+1)->id = 0;

    hash->maxElems += numElements;

    /* Fill up the free list with all the new elements */
    if (hash->firstFree == NULL)
        hash->firstFree = HASH_ELEMENT(hash, 1);
    for (i = 1; i <= numElements; i++)
    {
        HASH_ELEMENT(hash, i)->next = HASH_ELEMENT(hash, i+1);
        HASH_ELEMENT(hash, i)->id = -1;
    }
    if (hash->lastFree != NULL)
        hash->lastFree->next = HASH_ELEMENT(hash, 1);
    hash->lastFree = HASH_ELEMENT(hash, numElements);
    hash->lastFree->next = NULL;
    return RV_OK;
}




#ifdef __cplusplus
}
#endif
