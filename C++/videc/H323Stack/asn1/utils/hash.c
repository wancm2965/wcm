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
  ohash.c

  Opened hash table implementation


  Notes:
  Hash function is provided by the user.
  Hash function parameters may be duplicated in the hash table.
  Each table element holds list id. New collided parameter is
  positioned in the list corresponding to parameter key.
  Duplicated parameters following each other in the corresponding list.
  The hash size is the first prime number above the supplied hashSize.

  */

#include <string.h>

#include "rvmemory.h"
#include "ra.h"
#include "intutils.h"
#include "hash.h"


#if (RV_ASN1_LEAN_3G324M == RV_NO)

#ifdef __cplusplus
extern "C" {
#endif



/************************************************************************
 * hashStruct structure
 * Structure holding a HASH object
 ************************************************************************/
typedef struct
{
    hashFunc        hash; /* Hash function used */
    hashCompareFunc compare; /* Comparison function used */
    RvUint32        mask; /* Mask needed when dealing with keys (instead of modulu) */
    RvUint32        userKeySize; /* Size of the keys inside the hash table */
    RvUint32        userElemSize; /* Size of the elements inside the hash table */
    RvUint32        alignedKeySize; /* Size of the keys inside the hash table, aligned to 32bits */
    RvUint32        numElements; /* Number of elements that can be stored inside the hash */
    RvUint32        curSize; /* Current number of occupied elements */
    RAElement*      keys; /* List of keys (the hash table). This array holds pointers to the elements */
    HRA             elements; /* Table of stored elements (RA).
                                 See hashListElement to see the value itself */
    RvLogMgr*       logMgr; /* Log manager used */
} hashStruct;


/************************************************************************
 * hashListElement struct
 * This structure will be included in any user element and serve for
 * pointing to the next and prev elements in the list of elements that
 * were mapped to the same entry (collision/key)
 *
 * Note: Inside the elements RA, the information stored in each element
 *       is the list element, the key and the data in their written order.
 ************************************************************************/
typedef struct hashListElement_tag hashListElement;
struct hashListElement_tag
{
    hashListElement*    next; /* Pointer to the next user element with the same key value */
    hashListElement*    prev; /* Pointer to the previous user element with the same key value */
    int                 entryNum; /* The entry location of the key value in the hash table */
};



/* Mix up 3 bytes each time */
#define RV_HASH_MIX(a, b, c) \
{                                   \
    a -= b; a -= c; a ^= (c>>13);   \
    b -= c; b -= a; b ^= (a<<8);    \
    c -= a; c -= b; c ^= (b>>13);   \
    a -= b; a -= c; a ^= (c>>12);   \
    b -= c; b -= a; b ^= (a<<16);   \
    c -= a; c -= b; c ^= (b>>5);    \
    a -= b; a -= c; a ^= (c>>3);    \
    b -= c; b -= a; b ^= (a<<10);   \
    c -= a; c -= b; c ^= (b>>15);   \
}





/*************************************************************************
 * hashDefaultFunc
 * ------------------------------------------------------------------------
 * General: A generic fast hashing function for hash-tables.
 *   Hash a variable-length key into a 32-bit value
 *   Originally by Bob Jenkins, December 1996, Public Domain.
 *   Use for hash table lookup, or anything where one collision in 2^32 is
 *   acceptable.  Do NOT use for cryptographic purposes.
 *
 *   Returns a 32-bit value.  Every bit of the key affects every bit of
 *   the return value.  Every 1-bit and 2-bit delta achieves avalanche.
 *   About 36+6len instructions.
 *   The best hash table sizes are powers of 2.  There is no need to do
 *   mod a prime (mod is so slow!).
 *
 * Return Value: RvUint32 hash value.
 * ------------------------------------------------------------------------
 * Arguments: 
 * Input:   key     - Pointer the key.
 *          length  - The size of the key, in bytes.
 *          initVal - An initial 4-byte value. Can be the previous hash,
 *                    or an arbitrary value.
 * Output:  None.
 *************************************************************************/
RVINTAPI RvUint32 RVCALLCONV
hashDefaultFunc(
    IN void         *key,
    IN RvUint32     length,
    IN RvUint32     initVal)
{
    RvUint32 a, b, c, len;
    RvUint8 *k = (RvUint8 *)key;
    
    /* Set up the internal state */
    len = length;
    a = b = 0x9e3779b9;     /* the golden ratio; an arbitrary value      */
    c = initVal;            /* the previous hash value                   */
    
    /* Handle most of the key */
    while (len >= 12)
    {
        a += (k[0] +((RvUint32)k[1]<<8) +((RvUint32)k[2]<<16) +((RvUint32)k[3]<<24));
        b += (k[4] +((RvUint32)k[5]<<8) +((RvUint32)k[6]<<16) +((RvUint32)k[7]<<24));
        c += (k[8] +((RvUint32)k[9]<<8) +((RvUint32)k[10]<<16)+((RvUint32)k[11]<<24));
        RV_HASH_MIX(a,b,c);
        k += 12; 
        len -= 12;
    }
    
    /* Handle the last 11 bytes */
    c += length;
    switch (len)              /* all the case statements fall through */
    {
        case 11: c+=((RvUint32)k[10]<<24);  /* flow to next case */
        case 10: c+=((RvUint32)k[9]<<16);   /* flow to next case */
        case 9 : c+=((RvUint32)k[8]<<8);    /* flow to next case */
            /* the first byte of c is reserved for the length */
        case 8 : b+=((RvUint32)k[7]<<24);   /* flow to next case */
        case 7 : b+=((RvUint32)k[6]<<16);   /* flow to next case */
        case 6 : b+=((RvUint32)k[5]<<8);    /* flow to next case */
        case 5 : b+=k[4];                   /* flow to next case */
        case 4 : a+=((RvUint32)k[3]<<24);   /* flow to next case */
        case 3 : a+=((RvUint32)k[2]<<16);   /* flow to next case */
        case 2 : a+=((RvUint32)k[1]<<8);    /* flow to next case */
        case 1 : a+=k[0];                   /* flow to next case */
            /* case 0: nothing left to add */
        default:
            break;
    }

    RV_HASH_MIX(a,b,c);
    return(c);
}


/************************************************************************
 * Default comparison function. Checks byte-by-byte.
 * input  : key1, key2  - Keys to compare
 *          keySize     - Size of each key
 * return : RV_TRUE if elements are the same
 *          RV_FALSE otherwise
 ************************************************************************/
RVINTAPI RvBool RVCALLCONV
hashDefaultCompare(IN void *key1, IN void* key2, IN RvUint32 keySize)
{
    return (memcmp(key1, key2, keySize) == 0);
}



/************************************************************************
 * hashConstruct
 * purpose: Create a HASH object, holding a hash table of keys and the
 *          actual data in an array
 * input  : numOfKeys       - Size of hash table.
 *                            This is the amount of keys available for use
 *                            It should be greater than the number of
 *                            elements in the table
 *          numOfElems      - Number of elements that will be stored
 *                            in the hash table
 *          hashFunc        - Hash function used on the data
 *          compareFunc     - Comparison function used on the keys
 *          keySize         - Size of the keys
 *          elemSize        - Size of the elements
 *          name            - Name of HASH (used in log messages)
 *          logMgr          - Log manager to use
 * output : none
 * return : Handle to RA constructed on success
 *          NULL on failure
 ************************************************************************/
RVINTAPI HHASH RVCALLCONV
hashConstruct(
    IN  int             numOfKeys,
    IN  int             numOfElems,
    IN  hashFunc        hashFunct,
    IN  hashCompareFunc compareFunc,
    IN  int             keySize,
    IN  int             elemSize,
    IN  const RvChar*   name,
    IN  RvLogMgr*       logMgr)
{
    hashStruct* hash;
    RvUint32    needed, shift;
    RvUint32    actualKeySize;

    /* Calculate the actual size of the hash table = number of keys */
    needed = (RvUint32)RvMax(numOfKeys-1, 1);
    shift = 0;
    while (needed > 0)
    {
        shift += 1;
        needed >>= 1;
    }
    actualKeySize = (1 << shift);

    /* Allocate the HASH object and the hash table itself
     * We make sure to leave it empty to the hash table will be filled with NULL
     * pointers.
     */
    RvMemoryAlloc(NULL, sizeof(hashStruct)+actualKeySize*sizeof(RAElement), logMgr, (void**)&hash);
    memset(hash, 0, sizeof(hashStruct)+actualKeySize*sizeof(RAElement));

    /* Set the HASH object struct */
    if (hashFunct != NULL)
       hash->hash = hashFunct;
    else
        hash->hash = hashDefaultFunc;

    if (compareFunc != NULL)
       hash->compare = compareFunc;
    else
       hash->compare = hashDefaultCompare;

    hash->mask = actualKeySize-1;
    hash->userKeySize = (RvUint32)keySize;
    hash->userElemSize = (RvUint32)elemSize;
    hash->alignedKeySize = RvRoundToSize(keySize, RV_ALIGN_SIZE);
    hash->numElements = (RvUint32)numOfElems;
    hash->curSize = 0;
    hash->keys = (RAElement*)((RvUint8*)hash + sizeof(hashStruct));
    hash->logMgr = logMgr;

    /* Construct the elements RA */
    hash->elements =
        raConstruct((int)(elemSize + hash->alignedKeySize + sizeof(hashListElement)), numOfElems, RV_FALSE, name, logMgr);
    if (hash->elements == NULL)
    {
        RvMemoryFree(hash, logMgr);
        return NULL;
    }

    return (HHASH)hash;
}


/************************************************************************
 * hashDestruct
 * purpose: Delete a HASH object, freeing all of the taken memory
 * input  : hHash   - HASH object handle
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
hashDestruct(
    IN  HHASH hHash)
{
    hashStruct* hash = (hashStruct *)hHash;

    if (!hash) return RV_ERROR_UNKNOWN;
    raDestruct(hash->elements);
    RvMemoryFree(hash, hash->logMgr);

    return 0;
}


/************************************************************************
 * hashAdd
 * purpose: Add a new element to the hash table.
 *          This function will not add the element if an element with the
 *          same key already exists if asked
 * input  : hHash               - HASH object handle
 *          key                 - Pointer to the key
 *          userElem            - Pointer to the element to store
 *          searchBeforeInsert  - Check for the same key inside the HASH or not
 * output : none
 * return : Pointer to the element's location in the hash table on success
 *          NULL on failure
 ************************************************************************/
RVINTAPI void* RVCALLCONV
hashAdd(
     IN  HHASH  hHash,
     IN  void*  key,
     IN  void*  userElem,
     IN  RvBool searchBeforeInsert)
{
    hashStruct* hash = (hashStruct *)hHash;
    hashListElement* newElem;
    RvUint32 keyValue;

    /* See if such an element exists */
    if (searchBeforeInsert)
    {
        newElem = (hashListElement*)hashFind(hHash, key);
        if (newElem != NULL) return newElem;
    }

    /* Try to allocate a new element in the hash */
    if (raAdd(hash->elements, (RAElement*)&newElem) < 0)
    {
        /* Not found... */
        return NULL;
    }

    /* Calculate the key's hash value */
    keyValue = hash->hash(key, (RvUint32)(hash->userKeySize), 0) & hash->mask;

    /* Update new element's information (this is the first in the list) */
    newElem->prev = NULL;
    newElem->entryNum = keyValue;
    newElem->next = (hashListElement *)hash->keys[keyValue];

    /* Fix the next one in list if there is one */
    if (newElem->next != NULL) newElem->next->prev = newElem;

    /* Fill in with the element */
    memcpy((char*)newElem + sizeof(hashListElement), key, hash->userKeySize);
    if (userElem != NULL)
        memcpy((char*)newElem + sizeof(hashListElement) + hash->alignedKeySize, userElem, hash->userElemSize);
    hash->keys[keyValue] = (RAElement)newElem;

    return newElem;
}


/************************************************************************
 * hashFind
 * purpose: Find the location of an element by its key
 * input  : hHash       - HASH object handle
 *          key         - Pointer to the key
 * output : none
 * return : Pointer to the element's location in the hash table on success
 *          NULL on failure or if the element wasn't found
 ************************************************************************/
RVINTAPI void* RVCALLCONV
hashFind(
    IN  HHASH hHash,
    IN  void* key)
{
    hashStruct* hash = (hashStruct *)hHash;
    RvUint32 keyValue;
    hashListElement* hashElem;

    if (hHash == NULL) return NULL;

    /* Calculate the key's hash value */
    keyValue = hash->hash(key, (RvUint32)(hash->userKeySize), 0) & hash->mask;

    /* See if there are any elements in this key value at all */
    if (hash->keys[keyValue] == NULL)
        return NULL;

    /* Start looking for the element */
    hashElem = (hashListElement *)hash->keys[keyValue];

    /* Search for this element */
    while (hashElem != NULL)
    {
        if (hash->compare(key, (char*)hashElem + sizeof(hashListElement), hash->userKeySize))
        {
            /* Found! */
            return hashElem;
        }
        hashElem = hashElem->next;
    }

    /* No such element was found */
    return NULL;
}


/************************************************************************
 * hashFindNext
 * purpose: Find the location of the next element with the same key
 * input  : hHash       - HASH object handle
 *          key         - Pointer to the key
 *          location    - Location given in the last call to hashFindNext()
 *                        or hashFind().
 * output : none
 * return : Pointer to the element's location in the hash table on success
 *          NULL on failure or if the element wasn't found
 ************************************************************************/
RVINTAPI void* RVCALLCONV
hashFindNext(
    IN  HHASH hHash,
    IN  void* key,
    IN  void* location)
{
    hashStruct* hash = (hashStruct *)hHash;
    hashListElement* hashElem;

    if (location == NULL) return NULL;

    /* Start looking for the element */
    hashElem = (hashListElement *)location;

    /* First we skip the one we already know is ok */
    hashElem = hashElem->next;

    /* Search for this element */
    while (hashElem != NULL)
    {
        if (hash->compare(key, (char*)hashElem + sizeof(hashListElement), hash->userKeySize))
        {
            /* Found! */
            return hashElem;
        }
        hashElem = hashElem->next;
    }

    /* No such element was found */
    return NULL;
}


/************************************************************************
 * hashGetElement
 * purpose: Get the element's data by its location (given by hashFind()).
 * input  : hHash       - HASH object handle
 *          location    - Pointer to the element in hash
 *                        (given by hashAdd)
 * output : none
 * return : Pointer to the element's date in the hash table on success
 *          NULL on failure or if the element wasn't found
 ************************************************************************/
RVINTAPI void* RVCALLCONV
hashGetElement(
    IN  HHASH hHash,
    IN  void* location)
{
    hashStruct* hash = (hashStruct *)hHash;

    if (location == NULL) return NULL;

    return ((char*)location + sizeof(hashListElement) + hash->alignedKeySize);
}


/************************************************************************
 * hashSetElement
 * purpose: Set the element's data by its location (given by hashFind()).
 * input  : hHash       - HASH object handle
 *          location    - Pointer to the element in hash
 *                        (given by hashAdd)
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
hashSetElement(
    IN  HHASH   hHash,
    IN  void*   location,
    IN  void*   userElem)
{
    hashStruct* hash = (hashStruct *)hHash;

    if (hash == NULL) return RV_ERROR_UNKNOWN;

    memcpy((char*)location + sizeof(hashListElement) + hash->alignedKeySize, userElem, hash->userElemSize);
    return 0;
}


/************************************************************************
 * hashGetKey
 * purpose: Get the element's key by its location (given by hashFind()).
 * input  : hHash       - HASH object handle
 *          location    - Pointer to the element in hash
 *                        (given by hashAdd)
 * output : none
 * return : Pointer to the element's key in the hash table on success
 *          NULL on failure or if the element wasn't found
 ************************************************************************/

RVINTAPI void* RVCALLCONV
hashGetKey(
    IN  HHASH hHash,
    IN  void* location)
{
    RV_UNUSED_ARG(hHash);

    if (location == NULL) return NULL;
    return ((char*)location + sizeof(hashListElement));
}


/************************************************************************
 * hashDelete
 * purpose: Delete an element from the HASH
 * input  : hHash       - HASH object handle
 *          location    - Pointer to the element in hash
 *                        (given by hashAdd)
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
hashDelete(
    IN  HHASH   hHash,
    IN  void*   location)
{
    hashStruct* hash = (hashStruct *)hHash;
    hashListElement* userElem = (hashListElement *)location;

    if (location == NULL)
        return RV_ERROR_NULLPTR;

    /* Remove linkages to this element, making sure to update the key if
       we have to */
    if (userElem->prev == NULL)
    {
        /* First element - update keys table */
        hash->keys[userElem->entryNum] = (RAElement)userElem->next;
    }
    else
        userElem->prev->next = userElem->next;

    if (userElem->next != NULL)
        userElem->next->prev = userElem->prev;

    /* Remove this element from RA */
    return raDelete(hash->elements, (RAElement)userElem);
}


/************************************************************************
 * hashDoAll
 * purpose: Call a function on all used elements stored in HASH
 * input  : hHash       - HASH object handle
 *          func        - Function to execute on all elements. If this
 *                        function returns NULL, then this function
 *                        stops its execution.
 *          param       - Context to use when executing the function
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVINTAPI int RVCALLCONV
hashDoAll(
    IN HHASH        hHash,
    IN HASHEFunc    func,
    IN void*        param)
{
    hashStruct* hash = (hashStruct *)hHash;
    int cur;
    if (hash == NULL) return RV_ERROR_UNKNOWN;

    cur = -1;
    while ((param != NULL) && (cur = raGetNext(hash->elements, cur)) >= 0)
    {
        param = func(hHash, (void*)raGet(hash->elements, cur), param);
    }

    return RV_OK;
}


#ifdef __cplusplus
}
#endif


#endif /* (RV_ASN1_LEAN_3G324M == RV_NO) */

