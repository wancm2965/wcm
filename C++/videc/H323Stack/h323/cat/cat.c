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

#include "rvmemory.h"
#include "rvinternal.h"
#include "rvaddress.h"
#include "ra.h"
#include "hash.h"
#include "cmutils.h"
#include "catdb.h"
#include "cat.h"


#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 *
 *                              Private constants
 *
 ************************************************************************/

/* Additional number of keys for call - we use this value to make sure
   the hash table is bigger than the number of elements by a reasonable
   size */
#define CAT_ADDITIONAL_KEYS (3)


/* Key index handling association for version 1 endpoints. This one is used
   with or without a destCallSignalAddress field in it. */
#define CAT_ASSOCIATION_KEY (4)




/************************************************************************
 *
 *                              Private functions
 *
 ************************************************************************/


/************************************************************************
 * catHashKey
 * purpose: CAT key hashing function
 *          It uses different hashing method for different keys
 * Input:   key     - Pointer the key.
 *          length  - The size of the key, in bytes.
 *          initVal - An initial 4-byte value. Can be the previous hash,
 *                    or an arbitrary value. Passed as 0 by the hash
 *                    module.
 * Return : 32bit hash result
 ************************************************************************/
static RvUint32 RVCALLCONV catHashKey(
    IN void     *key,
    IN RvUint32 length,
    IN RvUint32 initVal)
{
    catStruct*  catKey;
    RvUint32    hashKey = 0;
    RvUint32    bytes, keyValue;
    RvUint8*    ptr;

    RV_UNUSED_ARG(length);
    RV_UNUSED_ARG(initVal);
    catKey = ((catKeyStruct *)key)->key;
    keyValue = ((catKeyStruct *)key)->keyValue;

    /* We take into consideration only the fields that are set in the keyValue */
    if ((keyValue & catCallId) != 0)
    {
        bytes = sizeof(catKey->callID);
        ptr = (RvUint8*)catKey->callID;
        while (bytes-- > 0) hashKey = hashKey << 1 ^ *ptr++;
    }
    if ((keyValue & catAnswerCall) != 0)
    {
        hashKey = hashKey << 1 ^ (catKey->answerCall);
    }
    if ((keyValue & catCRV) != 0)
    {
        bytes = sizeof(catKey->crv);
        ptr = (RvUint8*)&catKey->crv;
        while (bytes-- > 0) hashKey = hashKey << 1 ^ *ptr++;
    }
    if ((keyValue & catRasCRV) != 0)
    {
        bytes = sizeof(catKey->rasCRV);
        ptr = (RvUint8*)&catKey->rasCRV;
        while (bytes-- > 0) hashKey = hashKey << 1 ^ *ptr++;
    }
    if ((keyValue & catDestCallSignalAddr) != 0)
    {
        hashKey = addrIntoHashKey(&catKey->destCallSignalAddr, hashKey);
    }
    if ((keyValue & catRasSrcAddr) != 0)
    {
        hashKey = addrIntoHashKey(&catKey->rasSrcAddr, hashKey);
    }
    if ((keyValue & catSrcCallSignalAddr) != 0)
    {
        hashKey = addrIntoHashKey(&catKey->srcCallSignalAddr, hashKey);
    }
    if ((keyValue & catCID) != 0)
    {
        bytes = sizeof(catKey->cid);
        ptr = (RvUint8*)catKey->cid;
        while (bytes-- > 0) hashKey = hashKey << 1 ^ *ptr++;
    }

    return hashKey;
}


/************************************************************************
 * catCompareKey
 * purpose: Comparison function for 2 key structs in CAT
 * input  : givenKey    - The key given by the user (catKeyStruct type)
 *                        The keyValue field of this struct is discarded
 *                        in the comparison process.
 *          storedKey   - Key inside hash table (catKeyStruct type)
 *          keySize     - Size of each key
 * return : RV_TRUE if elements are the same
 *          RV_FALSE otherwise
 ************************************************************************/
static RvBool RVCALLCONV catCompareKey(IN void* givenKey, IN void* storedKey, IN RvUint32 keySize)
{
    /* We check the fields one by one, looking for each flag that exist in both keys */
    catStruct*  searchFor;
    catStruct*  trueKey;
    RvUint32    keyValue;

    RV_UNUSED_ARG(keySize);

    searchFor = ((catKeyStruct *)givenKey)->key;
    trueKey = ((catKeyStruct *)storedKey)->key;
    keyValue = ((catKeyStruct *)storedKey)->keyValue;

    /* First let's make sure we're dealing with the same type of key from
       the different types of keys stored inside CAT */
    if ((searchFor->flags & keyValue) == keyValue)
    {
        /* Check each of the key parts that are set in this struct */
        if ((searchFor->flags & trueKey->flags & catCallId) != 0)
            if (memcmp(searchFor->callID, trueKey->callID, sizeof(trueKey->callID)) != 0)
                return RV_FALSE; /* CallID doesn't match */
        if ((keyValue & catAnswerCall) != 0)
            if (searchFor->answerCall != trueKey->answerCall)
                return RV_FALSE; /* answerCall doesn't match */
        if ((keyValue & catCRV) != 0)
            if (searchFor->crv != trueKey->crv)
                return RV_FALSE; /* Q931CRV doesn't match */
        if ((keyValue & catRasCRV) != 0)
            if (searchFor->rasCRV != trueKey->rasCRV)
                return RV_FALSE; /* RAS-CRV doesn't match */
        if ((keyValue & catDestCallSignalAddr) != 0)
            if (cmCompareTransportAddresses(&searchFor->destCallSignalAddr, 
                                            &trueKey->destCallSignalAddr) == RV_FALSE)
                return RV_FALSE; /* DestAddress doesn't match */
        if ((keyValue & catRasSrcAddr) != 0)
            if (cmCompareTransportAddresses(&searchFor->rasSrcAddr,
                                            &trueKey->rasSrcAddr) == RV_FALSE)
                return RV_FALSE; /* RasSrcAddr doesn't match */
        if ((keyValue & catSrcCallSignalAddr) != 0)
            if (cmCompareTransportAddresses(&searchFor->srcCallSignalAddr,
                                            &trueKey->srcCallSignalAddr) == RV_FALSE)
                return RV_FALSE; /* SrcAddress doesn't match */
        if ((keyValue & catCID) != 0)
            if (memcmp(searchFor->cid, trueKey->cid, sizeof(trueKey->cid)) != 0)
                return RV_FALSE; /* CID doesn't match */
    }
    else
        return RV_FALSE;

    /* If we're here it means that the elements match */
    return RV_TRUE;
}


/************************************************************************
 * catCheckDestCallSignalAddress
 * purpose: This function checks if we are currently handling something
 *          that might generate keys with destCallSignalAddress although
 *          this field is missing. We do this for Version 1 endpoits that
 *          have no other way of being pin-pointed (this means that we
 *          don't have their CallId).
 * input  : cat     - CAT instance to use
 *          key     - Key to check and update
 * return : RV_OK on success, other on failure
 ************************************************************************/
static RvStatus catCheckDestCallSignalAddress(
    IN      catModule   *cat,
    INOUT   catStruct   *key)
{
    if ((key->flags & catCallId) != 0)
    {
        /* We have a CallID for this one - no need to work on this one since
           it's version 2 or up */
        return RV_OK;
    }

    if ((key->flags & catDestCallSignalAddr) != 0)
    {
        /* We have the destCallSignalAddress for this call already, so there's
           nothing more we can or should do here. */
        return RV_OK;
    }

    /* Check if we have all the fields for the CID association related key. If only
       destCallSignalAddress is missing - we might as well think that it's there and
       go on with it */
    if (((key->flags | catDestCallSignalAddr) & cat->keyTypes[CAT_ASSOCIATION_KEY]) ==
        cat->keyTypes[CAT_ASSOCIATION_KEY])
    {
        /* We might as well think that we have it... */
        key->flags |= catDestCallSignalAddr;
        memset(&key->destCallSignalAddr, 0, sizeof(cmTransportAddress));
    }

    return RV_OK;
}



/************************************************************************
 *
 *                              Public functions
 *
 ************************************************************************/


/************************************************************************
 * catConstruct
 * purpose: Create a CAT instance
 * input  : numCalls        - Number of calls supported by CAT
 *          isGatekeeper    - Is this a gatekeeper or an endpoint
 *          cidAssociation  - RV_TRUE if we want to associate calls by their CID
 *                            RV_FALSE if we don't want to associate calls by their CID
 *          compare15bitCrv - Use 15bit comparison on CRV instead of 16bit
 *          logMgr          - Log manager to use
 * output : none
 * return : Handle to CAT instance on success
 *          NULL on failure
 ************************************************************************/
RVHCAT catConstruct(
    IN RvUint32     numCalls,
    IN RvBool       isGatekeeper,
    IN RvBool       compare15bitCrv,
    IN RvBool       cidAssociation,
    IN RvLogMgr*    logMgr)
{
    catModule*  cat;

    /* Allocate the CAT instance */
    if(RvMemoryAlloc(NULL, sizeof(catModule), logMgr, (void**)&cat) != RV_OK)
        return NULL;

    cat->logMgr = logMgr;
    if (isGatekeeper)
        cat->numSimultKeys = 5;
    else
        cat->numSimultKeys = 4;

    RvLogSourceConstruct(logMgr, &cat->log, "CAT", "Call Association Table");
    RvLogDebug(&cat->log,
        (&cat->log, "catConstruct: compare15bitCrv=%d,cidAssociation=%d", compare15bitCrv, cidAssociation));
    cat->compare15bitCrv = compare15bitCrv;

    /* Set the keys we can handle */
    cat->keyTypes[0] = catCallId | catAnswerCall | catDestCallSignalAddr;
    cat->keyTypes[1] = catRasCRV | catRasSrcAddr;
    cat->keyTypes[2] = catCallId | catAnswerCall;
    cat->keyTypes[3] = catCallId | catRasCRV;
    if (cidAssociation == RV_TRUE)
        cat->keyTypes[CAT_ASSOCIATION_KEY] = catCID | catAnswerCall | catDestCallSignalAddr;
    else
        cat->keyTypes[CAT_ASSOCIATION_KEY] = catSrcCallSignalAddr | catAnswerCall | catDestCallSignalAddr;

    /* Allocate the hash */
    cat->hash =
        hashConstruct((int)(numCalls * (cat->numSimultKeys + CAT_ADDITIONAL_KEYS)),
                      (int)(numCalls * cat->numSimultKeys),
                      catHashKey,
                      catCompareKey,
                      sizeof(catKeyStruct),
                      sizeof(catDataStruct*),
                      "CAT HASH",
                      logMgr);

    /* Allocate the calls */
    cat->calls =
        raConstruct(sizeof(catDataStruct), (int)numCalls, RV_FALSE, "CAT CALLS", logMgr);

    if ((cat->hash == NULL) || (cat->calls == NULL))
    {
        if (cat->hash != NULL) hashDestruct(cat->hash);
        if (cat->calls != NULL) raDestruct(cat->calls);
        return NULL;
    }

    /* Allocate a mutex */
    RvLockConstruct(logMgr, &cat->lock);

    return (RVHCAT)cat;
}


/************************************************************************
 * catDestruct
 * purpose: Delete a CAT instance
 * input  : hCat    - CAT instance handle
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int catDestruct(IN RVHCAT hCat)
{
    catModule* cat;

    if (hCat == NULL) return RV_ERROR_UNKNOWN;
    cat = (catModule *)hCat;

    hashDestruct(cat->hash);
    raDestruct(cat->calls);
    RvLockDestruct(&cat->lock, cat->logMgr);
    RvLogSourceDestruct(&cat->log);
    RvMemoryFree(cat, cat->logMgr);
    return 0;
}


/************************************************************************
 * catAdd
 * purpose: Add a new call into a CAT instance
 * input  : hCat    - CAT instance handle
 *          key     - Key structure to associate with the call
 *          hsCall  - Call handle to put
 * output : none
 * return : CAT call handle on success
 *          NULL on failure
 ************************************************************************/
RVHCATCALL catAdd(
    IN RVHCAT       hCat,
    IN catStruct*   key,
    IN HCALL        hsCall)
{
    catModule*      cat = (catModule *)hCat;
    catDataStruct*  call;
    catKeyStruct    hashKey;
    int             i, curKey;

    RvLockGet(&cat->lock, cat->logMgr);

    /* Allocate a call in the RA */
    if (raAdd(cat->calls, (RAElement*)&call) < 0)
    {
        RvLogError(&cat->log,
            (&cat->log, "catAdd: Error adding a new call"));
        RvLockRelease(&cat->lock, cat->logMgr);
        return NULL;
    }

    RvLogDebug(&cat->log,
        (&cat->log, "catAdd: flags=0x%x, hsCall=%p, cat=%p", key->flags, hsCall, call));

    if (cat->compare15bitCrv)
    {
        key->rasCRV &= 0x7fff;
    }

    catCheckDestCallSignalAddress(cat, key);

    /* Set the values we already know inside the CAT call data */
    memset(call, 0, sizeof(catDataStruct));
    memcpy(&call->key, key, sizeof(catStruct));
    call->hsCall = hsCall;
    hashKey.key = &call->key;

    /* Check to see which key combinations are set inside the key struct and update the hash
       table accordingly */
    curKey = 0;
    for (i = 0; i < CAT_KEY_TYPES; i++)
    {
        if ((key->flags & cat->keyTypes[i]) == cat->keyTypes[i])
        {
            /* We've got a new key to add - first let's make sure we're not out of bounds */
            if (curKey >= cat->numSimultKeys)
            {
                RvLogWarning(&cat->log,
                    (&cat->log,"catAdd: Too many simultaneous keys for hsCall=%p", hsCall));
                break;
            }

            /* Create a HASH key out of the value and add it in */
            hashKey.keyValue = cat->keyTypes[i];
            call->hashValues[curKey] = hashAdd(cat->hash, &hashKey, &call, RV_FALSE);
            if ( !(call->hashValues[curKey]) )
            {
                RvLogError(&cat->log,
                    (&cat->log, "catAdd: Counldn't add new call to hash table (hsCall=%p, cat=%p)", hsCall, call));
            }
            else
                curKey++;
        }
    }

    RvLockRelease(&cat->lock, cat->logMgr);
    return (RVHCATCALL)call;
}


/************************************************************************
 * catDelete
 * purpose: Delete a call from a CAT instance
 * input  : hCat        - CAT instance handle
 *          hCatCall    - CAT call handle
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int catDelete(
    IN RVHCAT       hCat,
    IN RVHCATCALL   hCatCall)
{
    catModule*      cat = (catModule *)hCat;
    catDataStruct*  call = (catDataStruct *)hCatCall;
    int i, status;

    if (hCatCall == NULL) return 0;
    RvLockGet(&cat->lock, cat->logMgr);

    RvLogDebug(&cat->log,
        (&cat->log, "catDelete: cat=%p, hsCall=%p", call, call->hsCall));

    /* First we remove all hash elements */
    i = 0;
    while ((i < cat->numSimultKeys) && (call->hashValues[i] != NULL))
    {
        hashDelete(cat->hash, call->hashValues[i]);
        i++;
    }

    /* Then we remove the call from RA */
    status = raDelete(cat->calls, (RAElement)call);

    RvLockRelease(&cat->lock, cat->logMgr);
    return status;
}


/************************************************************************
 * catFind
 * purpose: Find a CAT call handle by a given key struct that can hold
 *          several different keys
 * input  : hCat        - CAT instance handle
 *          key         - Key structure to look for
 * return : CAT call handle on success
 *          NULL on failure or when a call wasn't found
 ************************************************************************/
RVHCATCALL catFind(
    IN  RVHCAT      hCat,
    IN  catStruct*  key)
{
    catModule*      cat = (catModule *)hCat;
    catKeyStruct    catKey;
    catDataStruct*  call;
    void*           location;
    RvUint32        crv;
    int             i;

    catKey.key = key;
    call = NULL;

    /* See if we compare 16bit CRV - we'll have to xor the MSB */
    crv = key->rasCRV;
    if ((key->flags & catRasCRV) && (cat->compare15bitCrv))
        key->rasCRV &= 0x7fff;

    catCheckDestCallSignalAddress(cat, key);

    RvLockGet(&cat->lock, cat->logMgr);

    /* see if any of the possible keys represent a match */
    for (i = 0; i < CAT_KEY_TYPES; i++)
    {
        if ((key->flags & cat->keyTypes[i]) == cat->keyTypes[i])
        {
            catKey.keyValue = cat->keyTypes[i];
            location = hashFind(cat->hash, &catKey);

            if (location != NULL)
            {
                /* found a match */
                call = *((catDataStruct**)hashGetElement(cat->hash, location));
                break;
            }
        }

    }

    RvLockRelease(&cat->lock, cat->logMgr);

    /* Make sure we didn't mess up with the CRV value */
    key->rasCRV = crv;

    RvLogDebug(&cat->log,
        (&cat->log, "catFind: cat=%p, from keys %d,0x%x", call, i, key->flags));

    return (RVHCATCALL)call;
}


/************************************************************************
 * catUpdate
 * purpose: Update a CAT call information with new keys
 * input  : hCat        - CAT instance handle
 *          hCatCall    - CAT call handle to update
 *          key         - Key structure with new information
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int catUpdate(
    IN RVHCAT       hCat,
    IN RVHCATCALL   hCatCall,
    IN catStruct*   key)
{
    catModule*      cat = (catModule *)hCat;
    catDataStruct*  call = (catDataStruct *)hCatCall;
    catKeyStruct    hashKey;
    RvUint32        oldFlags;
    int             i, curKey;

    RvLogDebug(&cat->log,
        (&cat->log, "catUpdate: cat=%p, with 0x%x - adding 0x%x keys",
             call, call->key.flags, key->flags));

    if (cat->compare15bitCrv)
    {
        key->rasCRV &= 0x7fff;
    }

    catCheckDestCallSignalAddress(cat, key);

    RvLockGet(&cat->lock, cat->logMgr);

    /* See if there's any new information at all */
    if ((key->flags & call->key.flags) != key->flags)
    {
        /* We've got some updates in here... */

        /* Get all the new values that we need */
        if (((call->key.flags & catCallId) == 0) && ((key->flags & catCallId) != 0))
            memcpy(call->key.callID, key->callID, sizeof(key->callID));
        if (((call->key.flags & catAnswerCall) == 0) && ((key->flags & catAnswerCall) != 0))
            call->key.answerCall = key->answerCall;
        if (((call->key.flags & catCRV) == 0) && ((key->flags & catCRV) != 0))
            call->key.crv = key->crv;
        if (((call->key.flags & catRasCRV) == 0) && ((key->flags & catRasCRV) != 0))
            call->key.rasCRV = key->rasCRV;
        if (((call->key.flags & catDestCallSignalAddr) == 0) && ((key->flags & catDestCallSignalAddr) != 0))
            memcpy(&call->key.destCallSignalAddr, &key->destCallSignalAddr, sizeof(cmTransportAddress));
        if (((call->key.flags & catRasSrcAddr) == 0) && ((key->flags & catRasSrcAddr) != 0))
            memcpy(&call->key.rasSrcAddr, &key->rasSrcAddr, sizeof(cmTransportAddress));
        if (((call->key.flags & catSrcCallSignalAddr) == 0) && ((key->flags & catSrcCallSignalAddr) != 0))
            memcpy(&call->key.srcCallSignalAddr, &key->srcCallSignalAddr, sizeof(cmTransportAddress));
        if (((call->key.flags & catCID) == 0) && ((key->flags & catCID) != 0))
            memcpy(call->key.cid, key->cid, sizeof(key->cid));

        /* Make sure we also update the flags */
        oldFlags = call->key.flags;
        call->key.flags |= key->flags;

        /* Find out how many key combinations we currently have */
        curKey = 0;
        while ((curKey < cat->numSimultKeys) && (call->hashValues[curKey] != NULL)) curKey++;

        /* Insert the new available key combinations to the hash */
        for (i = 0; i < CAT_KEY_TYPES; i++)
        {
            if (((oldFlags & cat->keyTypes[i]) != cat->keyTypes[i]) &&
                ((key->flags & cat->keyTypes[i]) == cat->keyTypes[i]))
            {
                /* We've got a new key to add - first let's make sure we're not out of bounds */
                if (curKey >= cat->numSimultKeys)
                {
                    RvLogWarning(&cat->log,
                        (&cat->log, "catUpdate: Too many simultaneous keys for cat=%p, hsCall=%p", call, call->hsCall));
                    break;
                }

                /* Create a HASH key out of the value and add it in */
                hashKey.key = &call->key;
                hashKey.keyValue = cat->keyTypes[i];
                call->hashValues[curKey] = hashAdd(cat->hash, &hashKey, &call, RV_FALSE);
                if ( !(call->hashValues[curKey]) )
                {
                    RvLogError(&cat->log,
                        (&cat->log, "catUpdate: Counldn't add new call to hash table (hsCall=%p, cat=%p)", call->hsCall, call));
                }
                else
                    curKey++;
            }
        }
    }

    RvLockRelease(&cat->lock, cat->logMgr);

    return 0;
}


/************************************************************************
 * catGetCallHandle
 * purpose: Return the call handle of a CAT call handle
 * input  : hCat        - CAT instance handle
 *          hCatCall    - CAT call handle
 * output : none
 * return : HCALL for the CAT call handle on success
 *          NULL on failure
 ************************************************************************/
HCALL catGetCallHandle(
    IN RVHCAT       hCat,
    IN RVHCATCALL   hCatCall)
{
    catDataStruct*  call = (catDataStruct *)hCatCall;
    RV_UNUSED_ARG(hCat);
    if(call)
        return call->hsCall;
    return NULL;
}


/************************************************************************
 * catGetKey
 * purpose: Return the key struct stored inside a CAT call handle
 * input  : hCat        - CAT instance handle
 *          hCatCall    - CAT call handle
 * output : none
 * return : Key struct for the CAT call handle on success
 *          NULL on failure
 ************************************************************************/
catStruct* catGetKey(
    IN RVHCAT       hCat,
    IN RVHCATCALL   hCatCall)
{
    catDataStruct*  call = (catDataStruct *)hCatCall;
    RV_UNUSED_ARG(hCat);
    return &call->key;
}


/************************************************************************
 * catGetUnsolicitedIRR
 * purpose: Return the unsolicited IRR handle stored inside a CAT call handle
 * input  : hCat            - CAT instance handle
 *          hCatCall        - CAT call handle
 *          unsolicitedIRR  - Handle of the unsolicited IRR to set
 * output : none
 * return : Unsolicited IRR transaction handle for the CAT call handle on success
 *          NULL on failure
 ************************************************************************/
HRAS catGetUnsolicitedIRR(
    IN RVHCAT       hCat,
    IN RVHCATCALL   hCatCall)
{
    catModule*      cat = (catModule *)hCat;
    catDataStruct*  call = (catDataStruct *)hCatCall;
    HRAS    tx;

    RvLockGet(&cat->lock, cat->logMgr);
    tx = call->unsolicitedIRR;
    RvLockRelease(&cat->lock, cat->logMgr);

    return tx;
}


/************************************************************************
 * catSetUnsolicitedIRR
 * purpose: Set the unsolicited IRR handle stored inside a CAT call handle
 * input  : hCat            - CAT instance handle
 *          hCatCall        - CAT call handle
 *          unsolicitedIRR  - Handle of the unsolicited IRR to set
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int catSetUnsolicitedIRR(
    IN RVHCAT       hCat,
    IN RVHCATCALL   hCatCall,
    IN HRAS         unsolicitedIRR)
{
    catModule*      cat = (catModule *)hCat;
    catDataStruct*  call = (catDataStruct *)hCatCall;

    RvLockGet(&cat->lock, cat->logMgr);
    call->unsolicitedIRR = unsolicitedIRR;
    RvLockRelease(&cat->lock, cat->logMgr);

    return 0;
}



#ifdef __cplusplus
}
#endif


