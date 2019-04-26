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

    /*
    rpool.c

     memory allocation mechanism, implementation based on fixed size array

    */

#include "rvstdio.h"
#include "rvlock.h"
#include "ra.h"
#include "rpool.h"
#include "rvassert.h"


#ifdef __cplusplus
extern "C" {
#endif





/*
  An RPOOL node always has bit 0x40000000 used.
  This is the case since RPOOL and RTREE are used in the same allocation space in the PVT,
  and this bit is used to see which type of node is used in each case.
  Each RPOOL node holds in its first parameter the location of the next block.
  The last element node holds 0x60000000, ORd with the size used in the last block.
 */

/* MACRO Definitions */
#define RPOOL_LOCATION(p)       ((int)p)
#define RETURNED_VALUE(p)       (HRPOOLELEM)(p|0x40000000)
#define NODE_HANDLE(p)          (p|0x40000000)
#define RPOOL_NODE_ID(p)        ((int)p&~0x60000000)
#define IS_LAST_NODE(p)         ((p)& 0x20000000)
#define GET_LAST_NODE_SIZE(p)   ((p)&~0x20000000)
#define SET_LAST_NODE(p)        ((p) |0x20000000)
#define rpoolGetNode(raH,location)  ((rpoolElem *)raGet((raH),RPOOL_NODE_ID(location)))
#define INVALID_LOCATION(raH,location) (!((location) & 0x60000000) || (raElemIsVacant((HRA)(raH), RPOOL_NODE_ID(location))))

#define COMPARE_ERROR (int)0x80000000

#define RPOOL_NODE_SIZE(elemSize) ((int)((elemSize) + sizeof(int)))
#define RPOOL_ELEM_SIZE(nodeSize) ((int)((nodeSize) - sizeof(int)))
#define RPOOL_BLOCKS(size,elemSize)  ((int)(size)/(int)(elemSize) + ( ((int)(size)%(int)(elemSize)) ? 1 : 0 ))

#if (RV_LOCK_TYPE == RV_LOCK_NONE)
#define RPOOL_THREAD_LOCK(_ra)
#define RPOOL_THREAD_UNLOCK(_ra)
#else
#define RPOOL_THREAD_LOCK(_ra) if (((raHeader *)(_ra))->userThreadSafe) {RvLockGet(&((raHeader *)(_ra))->lock, NULL);} else {;};
#define RPOOL_THREAD_UNLOCK(_ra) if (((raHeader *)(_ra))->userThreadSafe) {RvLockRelease(&((raHeader *)(_ra))->lock, NULL);} else {;};
#endif

static int rpoolFreeNoLock(
     IN HRA         raH,
     IN HRPOOLELEM  ptr);




/************************************************************************
 * rpoolElem
 * Header of an element inside rpool
 * nextBlock    - Location of the next block for this allocation
 * data         - User's data information stored
 ************************************************************************/
typedef struct
{
    int     nextBlock;
    char    data;
} rpoolElem;







/************************************************************************
 *
 *                              Public functions
 *
 ************************************************************************/


/************************************************************************
 * rpoolConstruct
 * purpose: Create an RPOOL object.
 * input  : elemSize        - Size of elements in the RPOOL in bytes.
 *          maxNumOfBlocks  - Number of blocks in RPOOL.
 *          threadSafe      - RV_TRUE to make allocations and deallocations
 *                            thread-safe.
 *          name            - Name of RPOOL (used in log messages).
 *          logMgr          - Log manager to use.
 * output : none
 * return : Handle to RPOOL constructed on success.
 *          NULL on failure.
 ************************************************************************/
RVINTAPI HRPOOL RVCALLCONV
rpoolConstruct(
    IN int              elemSize,
    IN int              maxNumOfBlocks,
    IN RvBool           threadSafe,
    IN const RvChar*    name,
    IN RvLogMgr*        logMgr)
{
    HRA raH;

    if ((!elemSize) || (!maxNumOfBlocks))
        return NULL;

    if (!(raH = raConstruct(RPOOL_NODE_SIZE(elemSize), maxNumOfBlocks, threadSafe, name, logMgr)) )
        return NULL;

    if (threadSafe)
    {
        /* Thread safery in RPOOL is done by telling RA not to use thread-safety, but still indicating
           in RA that we do want to use thread-safety on our own - UGLY but working */
        raHeader *ra = (raHeader *)raH;
        ra->userThreadSafe = RV_TRUE;
        ra->threadSafe = RV_FALSE;
    }

    rpoolClear((HRPOOL)raH);
    return (HRPOOL)raH;
}


/************************************************************************
 * rpoolClear
 * purpose: Clear the RPOOL from any allocations
 * input  : pool    - RPOOL handle
 * output : none
 * return : none
 ************************************************************************/
void
rpoolClear(IN HRPOOL pool)
{
    if (pool != NULL)
        raClear((HRA)pool);
}


/************************************************************************
 * rpoolDestruct
 * purpose: Deallocate an RPOOL object
 * input  : pool    - RPOOL handle
 * output : none
 * return : none
 ************************************************************************/
RVINTAPI void RVCALLCONV
rpoolDestruct(IN HRPOOL pool)
{
    if (pool != NULL)
        raDestruct((HRA)pool);
}


/************************************************************************
 * rpoolAllocCopyExternalNoLock
 * purpose: Allocate a chunk of memory from RPOOL and set it to a specified
 *          value from a buffer in memory
 * input  : pool    - RPOOL handle
 *          offset  - Offset to allocate the memory at
 *          src     - Source pointer of the external buffer set
 *                    If NULL, then the memory allocated is set to zero
 *          size    - Size of allocation in bytes
 * output : none
 * return : Pointer to memory chunk on success
 *          NULL on failure
 ************************************************************************/
static HRPOOLELEM rpoolAllocCopyExternalNoLock(
       IN HRA           raH,
       IN int           offset,
       IN const void*   src,
       IN int           size)
{
    rpoolElem *elem=NULL, *newElem;
    int numOfBlocks,numOfFreeBlocks;
    int elemSize;
    int i, cur, first=0;
    int fullSize = size + offset;
    int blockToCopy = 0;
    char *curSrc = (char*)src;

    if (fullSize <= 0)
        return NULL;

    /* Calculate the number of blocks we're dealing with */
    elemSize = RPOOL_ELEM_SIZE(raElemSize(raH));
    numOfBlocks = RPOOL_BLOCKS(fullSize, elemSize);
    numOfFreeBlocks = raFreeSize(raH);

    /* Make sure we have enough room for it */
    if (numOfFreeBlocks < numOfBlocks)
    {
        return NULL;
    }
    RvAssert(offset < elemSize);
    RvAssert(numOfBlocks > 0);

    /* Start adding blocks */
    for (i = 0; i < numOfBlocks; i++)
    {
        /* Allocate another block */
        if ( (cur = raAdd(raH, (RAElement *)&newElem)) < 0 )
        {
            /* This shouldn't happen ! */
            if (elem != NULL)
            {
                /* Make sure this rpool element is valid even though it's an error */
                elem->nextBlock = SET_LAST_NODE(0);
            }
            return NULL;
        }

        /* Link the last handle to the new one */
        if (elem != NULL)
            elem->nextBlock = NODE_HANDLE(cur);
        else
            first = cur;

        elem = newElem; /* rpoolGetNode(raH,cur);    */

        /* Fill/Clear the allocated memory chunk */
        if (src != NULL)
        {
            /* We need to set from given external memory buffer */
            if ((i == numOfBlocks-1) && (fullSize % elemSize))
            {
                /* Last element - copy only the bytes that are left in it */
                memcpy((&(elem->data))+offset, curSrc, (RvSize_t)((fullSize % elemSize)-offset));
            }
            else
            {
                /* Start or middle. Copy it all */
                blockToCopy = elemSize - offset;
                memcpy((&(elem->data))+offset, curSrc, (RvSize_t)blockToCopy);
            }
            if (offset > 0)
            {
                /* Clear any header we might have */
                memset(&(elem->data), 0, (RvSize_t)offset);
                offset = 0;
            }
            curSrc += blockToCopy;
        }
        else
        {
            /* Clear the allocation */
            memset(&(elem->data), 0, (RvSize_t)elemSize);
        }
    }

    /* Set the last block as the last block */
	if (elem != NULL)
		elem->nextBlock = SET_LAST_NODE ( (fullSize%elemSize) ? fullSize%elemSize : elemSize );

    /* Return the allocation */
    return (RETURNED_VALUE(first));
}


/************************************************************************
 * rpoolAlloc
 * purpose: Allocate a chunk of memory from RPOOL
 *          The allocation is automatically set to zero in all of its bytes
 * input  : pool    - RPOOL handle
 *          size    - Size of allocation in bytes
 * output : none
 * return : Pointer to memory chunk on success
 *          NULL on failure
 ************************************************************************/
RVINTAPI HRPOOLELEM RVCALLCONV
rpoolAlloc(
      IN HRPOOL pool,
      IN int    size)
{
    HRA raH = (HRA)pool;
    HRPOOLELEM ret;

    if (pool == NULL)
        return NULL;

    RPOOL_THREAD_LOCK(raH);
    ret = rpoolAllocCopyExternalNoLock(raH, 0, NULL, size);
    RPOOL_THREAD_UNLOCK(raH);

    return ret;
}


/************************************************************************
 * rpoolAllocCopyExternal
 * purpose: Allocate a chunk of memory from RPOOL and set it to a specified
 *          value from a buffer in memory
 * input  : pool    - RPOOL handle
 *          offset  - Offset to allocate the memory at
 *          src     - Source pointer of the external buffer set
 *                    If NULL, then the memory allocated is set to zero
 *          size    - Size of allocation in bytes
 * output : none
 * return : Pointer to memory chunk on success
 *          NULL on failure
 ************************************************************************/
RVINTAPI HRPOOLELEM RVCALLCONV
rpoolAllocCopyExternal(
       IN HRPOOL        pool,
       IN int           offset,
       IN const void*   src,
       IN int           size)
{
    HRA raH = (HRA)pool;
    HRPOOLELEM ret;

    if (pool == NULL)
        return NULL;

    RPOOL_THREAD_LOCK(raH);
    ret = rpoolAllocCopyExternalNoLock(raH, offset, src, size);
    RPOOL_THREAD_UNLOCK(raH);

    return ret;
}


/************************************************************************
 * rpoolAllocCopyInternal
 * purpose: Allocate a chunk of memory from RPOOL and duplicate its value
 *          from another allocation in RPOOL
 * input  : destPool- Destination RPOOL handle, where the new buffer will
 *                    be allocated
 *          srcPool - Source RPOOL handle, where the buffer we copy from
 *                    resides
 *          src     - Source pointer of the internal buffer set
 *                    It is actually an RPOOL allocation handle returned
 *                    by rpoolAlloc() and other functions in RPOOL
 *                    If NULL, then the memory allocated is set to zero
 *          size    - Size of allocation in bytes
 * output : none
 * return : Pointer to memory chunk on success
 *          NULL on failure
 ************************************************************************/
HRPOOLELEM rpoolAllocCopyInternal(
       IN HRPOOL        destPool,
       IN HRPOOL        srcPool,
       IN const void*   src,
       IN int           size)
{
    HRA destRa = (HRA)destPool;
    HRA srcRa = (HRA)srcPool;
    rpoolElem *elem=NULL, *elemSrc=NULL, *newElem;
    int numOfBlocks,numOfFreeBlocks;
    RvSize_t elemSize;
    int i, cur, first=0;

    if ((srcPool == NULL) || (destPool == NULL) || (size <= 0))
        return NULL;

    /* Get the source element */
    if (src != NULL)
    {
        int location = RPOOL_LOCATION(src);
        if (INVALID_LOCATION(srcRa,location))
            return NULL;
        elemSrc=rpoolGetNode(srcRa,location);
    }

    RPOOL_THREAD_LOCK(destRa);

    /* Calculate the number of blocks we're dealing with */
    elemSize = (RvSize_t)RPOOL_ELEM_SIZE(raElemSize(srcRa));
    numOfBlocks = RPOOL_BLOCKS(size, elemSize);
    numOfFreeBlocks = raFreeSize(destRa);

    /* Make sure we have enough room for it */
    if ( (!size) || (numOfFreeBlocks < numOfBlocks ))
    {
        RPOOL_THREAD_UNLOCK(destRa);
        return NULL;
    }

    /* Start adding blocks */
    for(i=0; i<numOfBlocks; i++)
    {
        /* Add an RPOOL block */
        if ( (cur = raAdd(destRa, (RAElement *)&newElem)) < 0 )
        {
            if (elem != NULL)
            {
                /* Make sure this rpool element is valid */
                elem->nextBlock = SET_LAST_NODE(0);
            }
            RPOOL_THREAD_UNLOCK(destRa);
            return NULL;
        }

        /* Link previous block to the new one */
        if (elem)
            elem->nextBlock = NODE_HANDLE(cur);
        else
            first=cur;

        elem = newElem; /*rpoolGetNode(raH,cur);    */

        /* Copy/Clear the allocated block */
        if (src != NULL)
        {
            memcpy(&(elem->data), &(elemSrc->data), elemSize);
            if (IS_LAST_NODE(elemSrc->nextBlock))
                src=NULL;
            else
                elemSrc=rpoolGetNode(srcRa,elemSrc->nextBlock);
        }
        else
        {
            memset(&(elem->data), 0, elemSize);
        }
    }

    RPOOL_THREAD_UNLOCK(destRa);

	if (elem != NULL)
		elem->nextBlock = SET_LAST_NODE ( ((int)size%(int)elemSize) ? (int)size%(int)elemSize : (int)elemSize );
    return (RETURNED_VALUE(first));
}


/************************************************************************
 * rpoolRealloc
 * purpose: Reallocate chunk of memory, leaving any old bytes with the
 *          same value they had previously and setting new allocated
 *          bytes to zero.
 * input  : pool    - RPOOL handle
 *          src     - Element in RPOOL ot reallocate
 *          size    - Size of allocation in bytes
 * output : none
 * return : Non-negative value on success, other on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
rpoolRealloc(
       IN HRPOOL        pool,
       IN HRPOOLELEM    ptr,
       IN int           size)
{
    HRA raH = (HRA)pool;
    int numOfBlocks;
    int elemSize;
    int count=0;
    int contLocation, location;
    rpoolElem *elem=NULL;

    if ((pool == NULL) || (size <= 0))
        return RV_ERROR_NULLPTR;

    RPOOL_THREAD_LOCK(raH);

    location = RPOOL_LOCATION(ptr);

    elemSize = RPOOL_ELEM_SIZE(raElemSize(raH));
    numOfBlocks = RPOOL_BLOCKS(size,elemSize);

    /* Count the nodes in RPOOL that we're going to leave as is.
       We might end up holding all of them if the reallocation is bigger than the
       original size */
    while (( !IS_LAST_NODE(location) )&&( count < numOfBlocks ))
    {
        elem = rpoolGetNode(raH,location);
        location = elem->nextBlock;
        count ++;
    }

    /* New size is smaller than at previous allocation */
    if ( count == numOfBlocks )
    {
        /* Fix the next block of the new last element */
        elem->nextBlock = SET_LAST_NODE ( (size%elemSize) ? size%elemSize :  elemSize );

        /* Free the tail and we're done */
        rpoolFreeNoLock(raH, (HRPOOLELEM)location);
        RPOOL_THREAD_UNLOCK(raH);
        return RV_OK;
    }

    /* New size is bigger than at previous allocation */
    if ( IS_LAST_NODE(location) )
    {
        /* Allocate the additional space needed */
        contLocation = (int)rpoolAllocCopyExternalNoLock(raH, 0, NULL, (size - count*elemSize));
        if (contLocation == 0)
        {
            RPOOL_THREAD_UNLOCK(raH);
            return contLocation;
        }

        /* Link the new allocation with the old one */
        elem->nextBlock = contLocation;
        RPOOL_THREAD_UNLOCK(raH);
        return RV_OK;
    }

    RPOOL_THREAD_UNLOCK(raH);
    return RV_ERROR_UNKNOWN;
}


/************************************************************************
 * rpoolFreeNoLock
 * purpose: Free an element allocation in RPOOL
 * input  : pool    - RPOOL handle
 *          ptr     - Element in RPOOL
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
static int rpoolFreeNoLock(
     IN HRA         raH,
     IN HRPOOLELEM  ptr)
{
    int next, location;
    rpoolElem *elem;

    location = RPOOL_LOCATION(ptr);
    if (INVALID_LOCATION(raH,location)) return RV_ERROR_UNKNOWN;

    while (!IS_LAST_NODE(location))
    {
        location=RPOOL_NODE_ID(location);
        elem = rpoolGetNode(raH, location);
        if (elem != NULL)
        {
            next = elem->nextBlock;
            raDelete(raH, (RAElement)elem);
        }
        else
            next = -1;
        location = next;
    }
    return RV_TRUE;
}


/************************************************************************
 * rpoolFree
 * purpose: Free an element allocation in RPOOL
 * input  : pool    - RPOOL handle
 *          ptr     - Element in RPOOL
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
rpoolFree(
     IN HRPOOL      pool,
     IN HRPOOLELEM  ptr)
{
    HRA raH = (HRA)pool;
    int ret;

    if (pool == NULL)
        return RV_ERROR_NULLPTR;

    RPOOL_THREAD_LOCK(raH);
    ret = rpoolFreeNoLock(raH, ptr);
    RPOOL_THREAD_UNLOCK(raH);

    return ret;
}






/* Internal Operations */

/************************************************************************
 * rpoolCopyFromExternal
 * purpose: Copy an external memory buffer into an RPOOL element
 * input  : pool    - RPOOL handle
 *          dest    - Element in RPOOL to copy to
 *          src     - Source buffer in memory
 *          shift   - Offset in RPOOL block to copy to
 *          size    - Size of buffer to copy
 * output : none
 * return : Destination element on success
 *          NULL on failure
 ************************************************************************/
RVINTAPI HRPOOLELEM RVCALLCONV
rpoolCopyFromExternal(
    IN HRPOOL       pool,
    IN HRPOOLELEM   dest,
    IN const void*  src,
    IN int          shift,
    IN int          size)
{
    HRA raH = (HRA)pool;
    RvSize_t elemSize, tmpSize;
    int location, i, shiftBlocks;
    rpoolElem *elem;
    int srcShift=0;

    if(( !pool ) || ( !src )  || ( !size ))return NULL;

    location = RPOOL_LOCATION(dest);

    if (INVALID_LOCATION(raH,location)) return NULL;

    /* Calculate the number of blocks we want to skip */
    tmpSize = 0;
    elemSize = (RvSize_t)RPOOL_ELEM_SIZE(raElemSize(raH));
    shiftBlocks = RPOOL_BLOCKS(shift, elemSize);

    for ( i=0;!IS_LAST_NODE(location) && size>0; i++,location = elem->nextBlock)
    {
        elem=rpoolGetNode(raH,location);
        if ((i == shiftBlocks-1) && ((int)shift % (int)elemSize))
        {
            /* First block we copy to */
            tmpSize = RvMin(elemSize - (RvSize_t)shift % elemSize, (RvSize_t)size);
            memcpy( (char *)&(elem->data)+((RvSize_t)shift%elemSize), (char *)src, tmpSize);
            size-=tmpSize;
        }
        if (i>shiftBlocks-1)
        {
            /* Copy full blocks */
            tmpSize=RvMin(elemSize, (RvSize_t)size);
            memcpy( (char *)&(elem->data), (char *)src+srcShift, tmpSize);
            size-=tmpSize;
        }

        srcShift+=tmpSize;
    }

    return (dest);
}


/************************************************************************
 * rpoolCopyToExternal
 * purpose: Copy information from an RPOOL element to a memory buffer
 * input  : pool    - RPOOL handle
 *          dest    - Destination buffer in memory
 *          src     - Element in RPOOL to copy from
 *          shift   - Offset in RPOOL block to copy from
 *          size    - Size of buffer to copy
 * output : none
 * return : Destination memory buffer on success
 *          NULL on failure
 ************************************************************************/
RVINTAPI void*  RVCALLCONV
rpoolCopyToExternal(
    IN HRPOOL       pool,
    IN void*        dest,
    IN HRPOOLELEM   src,
    IN int          shift,
    IN int          size)
{
    HRA raH = (HRA)pool;
    RvSize_t elemSize, tmpSize;
    int location, i, shiftBlocks;
    rpoolElem *elem;
    int destShift=0;

    if (( !pool ) || ( !dest )  || ( !size ))
        return NULL;

    location = RPOOL_LOCATION(src);

    if (INVALID_LOCATION(raH,location))
        return NULL;

    /* Calculate the amount of blocks to skip before copying */
    tmpSize = 0;
    elemSize = (RvSize_t)RPOOL_ELEM_SIZE(raElemSize(raH));
    shiftBlocks = RPOOL_BLOCKS(shift, elemSize);

    for ( i=0;!IS_LAST_NODE(location) && size>0; i++,location = elem->nextBlock)
    {
        elem=rpoolGetNode(raH,location);
        if ((i == shiftBlocks-1) && ((int)shift % (int)elemSize))
        {
            /* First block we're copying */
            tmpSize = RvMin(elemSize - ((RvSize_t)shift % elemSize), (RvSize_t)size);
            memcpy( (char *)dest, (char *)&(elem->data)+((RvSize_t)shift%elemSize), tmpSize);
            size-=tmpSize;
        }
        if (i>shiftBlocks-1)
        {
            /* Copy the block */
            tmpSize=RvMin(elemSize, (RvSize_t)size);
            memcpy( (char *)dest + destShift, (char *)&(elem->data), tmpSize);
            size-=tmpSize;
        }
        destShift+=tmpSize;
    }

    return (dest);
}


/* Compares allocated element from pool with external block of memory */
/* Returns  the same output as memcmp */
int
rpoolCompareExternal(
             /* Compare dest to src */
             HRPOOL pool,
             HRPOOLELEM dest,
             void *src,
             int size)
{
    HRA raH = (HRA)pool;
    RvSize_t elemSize, numOfBlocks;
    int i, location, res=RV_ERROR_UNKNOWN;
    rpoolElem *elem;


    if ( (! pool ) || ( ! size) || (! src ))
        return RV_ERROR_UNKNOWN;

    location = RPOOL_LOCATION(dest);

    if (INVALID_LOCATION(raH, location))
        return COMPARE_ERROR;

    elemSize = (RvSize_t)RPOOL_ELEM_SIZE(raElemSize(raH));
    numOfBlocks = (RvSize_t)RPOOL_BLOCKS(size, elemSize);

    for(i=0;(!IS_LAST_NODE(location)&&(i<((int)numOfBlocks-1)));i++)
    {
        elem=rpoolGetNode(raH,location);
        res = memcmp((void *)&(elem->data), (char*)src+i*elemSize, elemSize);
        if (res != 0) return(res);
        location=elem->nextBlock;
    }

    if(!IS_LAST_NODE(location) && ( i == ((int)numOfBlocks-1)))
    {
        elem=rpoolGetNode(raH,location);
        res = memcmp((void *)&(elem->data),(char*)src+i*elemSize,
            ((RvSize_t)size % elemSize ? (RvSize_t)size % elemSize : elemSize));
        if (res != 0)
            return(res);
    }


    if (IS_LAST_NODE(location)&&((size-i*elemSize)>0)) return (-1);

    return (res);
}

/* Compares two allocated elements from pool */
/* Returns int that is less , equal greater then 0 */
/* if ptr1 is less, equal or greater than ptr2*/
int
rpoolCompareInternal(
             IN HRPOOL      pool,
             IN HRPOOLELEM  ptr1,
             IN HRPOOLELEM  ptr2,
             IN int         size
             )
{
    HRA raH=(HRA)pool;
    RvSize_t elemSize, numOfBlocks;
    int i, location1, location2, res=RV_ERROR_UNKNOWN;
    rpoolElem *elem1,*elem2;

    if ( (! pool ) || (! size) ) return RV_ERROR_UNKNOWN;

    location1=RPOOL_LOCATION(ptr1);
    location2=RPOOL_LOCATION(ptr2);

    if (INVALID_LOCATION(raH, location1)) return COMPARE_ERROR;
    if (INVALID_LOCATION(raH, location2)) return COMPARE_ERROR;

    elemSize = (RvSize_t)RPOOL_ELEM_SIZE(raElemSize(raH));
    numOfBlocks = (RvSize_t)RPOOL_BLOCKS(size, elemSize);

    for (i=0; (i<(int)numOfBlocks)&&!IS_LAST_NODE(location1)&&!IS_LAST_NODE(location2) ; i++)
    {
        elem1=rpoolGetNode(raH,location1);
        elem2=rpoolGetNode(raH,location2);
        res = memcmp( (void *)&(elem1->data), (void *)&(elem2->data), RvMin(elemSize,(RvSize_t)size));
        if (res != 0)
            return (res);
        location1=elem1->nextBlock;
        location2=elem2->nextBlock;
        size-=elemSize;
    }


    if ((!IS_LAST_NODE(location1) &&!IS_LAST_NODE(location2)) ||
        ( IS_LAST_NODE(location1) && IS_LAST_NODE(location2)) ||
        ( i==(int)numOfBlocks) )
        return (res);

    if( IS_LAST_NODE(location1))
        return (-1);
    else
        return (1);
}


#if 0
/*         Display Functions */
void
    rpoolPrint(
    IN HRPOOL pool
    )
{
    HRA raH = (HRA)pool;
    int location;
    int maxNumOfElem;
    int length;
    rpoolElem *elem;
    int elemSize;
    char tmp[128];

    strcpy(tmp, "");

    if( ! pool ) return ;

    maxNumOfElem=raMaxSize(raH);
    elemSize = (int)(raElemSize(raH) - sizeof(rpoolElem) + sizeof(char));
    for(location=0;location<maxNumOfElem;location++) {
        elem = rpoolGetNode(raH,location);
        length = IS_LAST_NODE(elem->nextBlock) ? (GET_LAST_NODE_SIZE(elem->nextBlock)) : elemSize ;
        if ( elem->data  ){
            memcpy(tmp, (char *)&(elem->data), elemSize);
            tmp[length]='\0';
        }
        printf("\n%d: alloc[%d] next[%d] data[%s]",
            location, raElemIsVacant(raH, location),
            elem->nextBlock, nprn(tmp));
        strcpy(tmp, "");


    }
}
#endif


RVINTAPI RvStatus RVCALLCONV
rpoolStatistics(
       /* Get pool statistics (space is in bytes) */
       IN  HRPOOL pool,
       OUT RvInt32* poolSize, /* max size of pool */
       OUT RvInt32* maxUsedSpace, /* maximum space allocated in the past */
       OUT RvInt32* allocatedSpace  /* currently allocated space */
       )
{
    HRA raH = (HRA)pool;
    int elemSize;

    if (pool == NULL)
        return RV_ERROR_NULLPTR;

    elemSize = RPOOL_ELEM_SIZE(raElemSize(raH));

    if (poolSize != NULL)
        *poolSize = raMaxSize(raH)*elemSize;
    if (maxUsedSpace != NULL)
        *maxUsedSpace = raMaxUsage(raH)*elemSize;
    if (allocatedSpace != NULL)
        *allocatedSpace = raCurSize(raH)*elemSize;

    return RV_OK;
}

RVINTAPI int RVCALLCONV
rpoolChunkSize(
           IN HRPOOL        pool,
           IN HRPOOLELEM    ptr
           )
{
    HRA raH = (HRA)pool;
    rpoolElem *elem;
    int elemSize;
    int location;
    int size = 0;
    RvBool isLastNode = RV_FALSE;

    if (pool == NULL)
        return RV_ERROR_UNKNOWN;

    location = RPOOL_LOCATION(ptr);
    if (INVALID_LOCATION(raH,location))
        return RV_ERROR_UNKNOWN;

    elemSize = RPOOL_ELEM_SIZE(raElemSize(raH));

    while (!isLastNode)
    {
        size += elemSize;
        elem = rpoolGetNode(raH, location);
        location = elem->nextBlock;
        isLastNode = IS_LAST_NODE(location);
    }
    size = (size - elemSize) + GET_LAST_NODE_SIZE(location);

    return (size);
}


/***************************************************************************************
 * rpoolGetPtr
 *
 * purpose: To get an hrpool message and offset and return a real pointer to that location
 *          and the length until the end of the contiguous part.
 *          If the offset reaches or exceeds the end of the element the length is returned as -1
 *
 * Input:  pool - the pool in question
 *         element - the element on which we are working
 *         offset - the offset in bytes into the element
 *
 * Output: pointer - A real pointer to the place calculated by the offset.
 *
 * returned value: Length - The size of the contiguous area from the pointer to the end
 *                          of the current segment of the element.
 *                 0 - The offset is out of the RPOOL element's range
 *                 Negative value - Failure.
 ****************************************************************************************/
RVINTAPI int RVCALLCONV
rpoolGetPtr(IN  HRPOOL      pool,
            IN  HRPOOLELEM  element,
            IN  int         offset,
            OUT void**      pointer)
{
    HRA raH = (HRA)pool;
    int elemSize;
    int length = 0;
    int location, i, shiftBlocks;
    rpoolElem *elem;
    RvBool found = RV_FALSE;

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    /* check that the inputs were legal */
    if ((pool == NULL) || (element == NULL))
        return RV_ERROR_NULLPTR;
#endif

    location = RPOOL_LOCATION(element);
    if (INVALID_LOCATION(raH,location))
        return -1;

    /* calculate the size of each part of the element */
    elemSize = RPOOL_ELEM_SIZE(raElemSize(raH));

    /* calculate the part in which the offset falls */
    shiftBlocks = RPOOL_BLOCKS(offset+1, elemSize);

    /* go over the parts until we reach the part in which the offset is */
    for (i = 1; (!found) && (i <= shiftBlocks); i++, location = elem->nextBlock)
    {
        if (IS_LAST_NODE(location))
            break; /* We've reached the end */

        /* get the current part entry */
        elem = rpoolGetNode(raH, location);

        /* if we reached the required part, calculate the pointer and length */
        if (i == shiftBlocks)
        {
            /* Assume the message occupies all the block and maybe more */
            if (IS_LAST_NODE(elem->nextBlock))
                length = GET_LAST_NODE_SIZE(elem->nextBlock);
            else
                length = elemSize;

            /* the contiguous area is from the offset in this block to the last byte */
            length -= (offset % elemSize);

            /* The pointer to the contiguous area is the beginning of the block plus
               the offset within the block */
            if (pointer)
                *pointer = (void *)( (char *)&(elem->data) + (offset % elemSize) );

            found = RV_TRUE;
        }
    }

    return length;
}


/***************************************************************************************
 * rpoolGetElemSize
 *
 * purpose: To get the size of a single element, as given in rpoolConstruct.
 *
 * Input:  pool - the pool in question
 *
 * Output: none.
 *
 * returned value: The size an element.
 ****************************************************************************************/
RVINTAPI int RVCALLCONV
rpoolGetElemSize(IN  HRPOOL      pool)
{
    HRA raH = (HRA)pool;
    return RPOOL_ELEM_SIZE(raElemSize(raH));
}



#ifdef __cplusplus
}
#endif



