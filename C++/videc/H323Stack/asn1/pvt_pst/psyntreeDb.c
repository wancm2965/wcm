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


/*
  psynTreeDb

  Syntax tree database access
  Provides internal functions to access the syntax database generated by the
  ASN.1 compiler.
 */

#include "rvstdio.h"
#include "intutils.h"
#include "psyntreeDb.h"

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 *
 *                              Private structures
 *
 ************************************************************************/


/************************************************************************
 * searchKey struct
 * Used as the key to search for in calls to RvH323BinarySearch().
 * syntax   - Syntax struct being searched
 * data     - Data being searched for
 ************************************************************************/
typedef struct
{
    fmt2Struct* syntax;
    void*       data;
} searchKey;






/************************************************************************
 *
 *                              Private functions
 *
 ************************************************************************/


/************************************************************************
 * fmt2NameCompare
 * purpose: Compare a search key with a table element of the syntax
 *          Used when looking for a name offset
 * input  : eKey    - Key element we're searching for
 *          elem    - Element we're currently comparing with
 * output : none
 * return : Negative if the key is lower than elem in dictionary comparison
 *          0 if the key and the element are equal
 *          Positive if the key is higher than elem in dictionary comparison
 ************************************************************************/
static int fmt2NameCompare(IN const void *eKey, IN const void *elem)
{
    searchKey*  key = (searchKey *)eKey;
    char*       str2;

    str2 = key->syntax->dictionary + ((fmt2Elem *)elem)->nameOffset + 1;

    return strcmp((char *)key->data, str2);
}


/************************************************************************
 * stGetElemByName
 * purpose: Get the element of a type by the type's name string
 * input  : hSyn        - Syntax information
 *          name        - String name
 * return : Element on success
 *          NULL on failure
 ************************************************************************/
static fmt2Elem *stGetElemByName(IN HPST hSyn, IN char *name)
{
    searchKey key;
    fmt2Elem* elem;

    if (name == NULL)
        return NULL;

    /* Create the search key */
    key.syntax  = ((synStruct *)hSyn)->syntax;
    key.data    = name;

    /* Search for the name inside the main table.
       The strings are sorted, so we're using binary search for it */
    elem = (fmt2Elem *)RvBsearch(&key, key.syntax->table, key.syntax->tableSize, sizeof(fmt2Elem), fmt2NameCompare);

    return elem;
}




/************************************************************************
 *
 *                              Public functions
 *
 ************************************************************************/


/************************************************************************
 * stGetChildByIndex
 * purpose: Get a child node of a parent node by its index
 * input  : hSyn            - Syntax information
 *          parentNodeId    - Parent's node ID
 *          index           - Index of the child (1-based)
 * output : nodeData        - Data pointer of the child node
 *                            Can be set to NULL
 * return : Pointer to use with stGetBrother on success,
 *          NULL on error
 ************************************************************************/
RvUint32* stGetChildByIndex(
    IN  HPST        hSyn,
    IN  int         parentNodeId,
    IN  int         index,
    OUT RvUint32**  nodeData)
{
    /* This function was optimized to make only a single multiplication.
       The for() loop inside this function has on average more than 6 iterations */
    int i;
    RvUint32* nodePtr;
    fmt2Struct* syntax = ((synStruct *)hSyn)->syntax;

    /* Sanity checks */
    if ((parentNodeId < 0) || (parentNodeId >= syntax->dataSize))
        return NULL;

    /* Get the pointer of the first data node and make sure the index is within valid range */
    nodePtr = &(syntax->data[parentNodeId]);

    /* Do not change the order of this function! It works better in this way on Solaris systems */
    if (index <= (int)m_numOfChilds(nodePtr+1))
    {
        /* Travel through the children to the right index */
        for (i = 0; i < index; i++)
        {
            /* Skip current node */
            nodePtr += ((*nodePtr) & 0xff);
        }

        /* See if we also have to get the data itself */
        if (nodeData != NULL)
            *nodeData = nodePtr + 1;

        /* Return the node pointer */
        return nodePtr;
    }

    return NULL;
}


#if (RV_ASN1_CODER_USE_H450 == RV_YES)
/************************************************************************
 * stGetChildByIndexUnbounded
 * purpose: Get a child node of a parent node by its index, without
 *          checking the numOfChilds value. Done instead of fixing the
 *          H.450 ASN.
 * input  : hSyn            - Syntax information
 *          parentNodeId    - Parent's node ID
 *          index           - Index of the child (1-based)
 * output : nodeData        - Data pointer of the child node
 *                            Can be set to NULL
 * return : Pointer to use with stGetBrother on success,
 *          NULL on error
 ************************************************************************/
RvUint32* stGetChildByIndexUnbounded(
    IN  HPST        hSyn,
    IN  int         parentNodeId,
    IN  int         index,
    OUT RvUint32**  nodeData)
{
    /* This function was optimized to make only a single multiplication.
       The for() loop inside this function has on average more than 6 iterations */
    int i;
    RvUint32* nodePtr;
    fmt2Struct* syntax = ((synStruct *)hSyn)->syntax;

    /* Sanity checks */
    if ((parentNodeId < 0) || (parentNodeId >= syntax->dataSize))
        return NULL;

    /* Get the pointer of the first data node and make sure the index is within valid range */
    nodePtr = &(syntax->data[parentNodeId]);

    /* Travel through the children to the right index */
    for (i = 0; i < index; i++)
    {
        /* Skip current node */
        nodePtr += ((*nodePtr) & 0xff);
    }

    /* See if we also have to get the data itself */
    if (nodeData != NULL)
        *nodeData = nodePtr + 1;

    /* Return the node pointer */
    return nodePtr;
}
#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */

/************************************************************************
 * stGetBrother
 * purpose: Get the brother of a node to the right. The responsibility
 *          not to ask for a non-existant brother is on the caller.
 * input  : hSyn            - Syntax information
 *          nodePtr         - Node pointer to which we want the brother
 * output : nodeData        - Data pointer of the brother node
 *                            Can be set to NULL
 * return : Pointer to use (responsibly) in subsequent calls to
 *          stGetBrother()
 ************************************************************************/
RvUint32* stGetBrother(
    IN  HPST        hSyn,
    IN  RvUint32*   nodePtr,
    OUT RvUint32**  nodeData)
{
    fmt2Struct* syntax = ((synStruct *)hSyn)->syntax;

    /* Travel through the children to the right */
    nodePtr += ((*nodePtr) & 0xff);

    /* make sure the pointer is in bounds */
    if (nodePtr < syntax->dataEnd)
    {
        /* See if we also have to get the data itself */
        if (nodeData != NULL)
            *nodeData = nodePtr + 1;

        /* Return the node pointer */
        return nodePtr;
    }
    return NULL;
}


#if (RV_ASN1_CODER_USE_H450 == RV_YES)
int stGetFirstValueNode(
    IN  HPST        hSyn,
    IN  int         parentNodeId)
{
    RvUint32* nodePtr;
    fmt2Struct* syntax = ((synStruct *)hSyn)->syntax;

    /* Sanity checks */
    if ((parentNodeId < 0) || (parentNodeId >= syntax->dataSize))
        return -1;

    /* Get the pointer of the first data node and make sure the index is within valid range */
    nodePtr = &(syntax->data[parentNodeId]);

    /* Get the first node after this one */
    return ( parentNodeId + ((*nodePtr) & 0xff) );
}
#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */


/************************************************************************
 * stGetNodeDataByNodeId
 * purpose: Get the data pointer of a node by its node ID
 * input  : hSyn        - Syntax information
 *          nodeId      - Node ID
 * output : none
 * return : Data pointer of node information on success
 *          NULL on failure
 ************************************************************************/
RvUint32* stGetNodeDataByNodeId(IN HPST hSyn, IN int nodeId)
{
    fmt2Struct* syntax = ((synStruct *)hSyn)->syntax;

    /* Sanity checks */
    if ((nodeId < 0) || (nodeId >= syntax->dataSize))
        return NULL;

    return (syntax->data + nodeId + 1);
}


/************************************************************************
 * stGetNameByNameId
 * purpose: Get the name string by the name ID
 * input  : hSyn        - Syntax information
 *          nameId      - Name ID of the field
 * output : length      - Length on name in bytes
 *                        Can be set to NULL
 * return : Name of field on success
 *          NULL on failure
 ************************************************************************/
char* stGetNameByNameId(
    IN  HPST        hSyn,
    IN  int         nameId,
    OUT int*        length)
{
	fmt2Struct* syntax;

	if (hSyn==0)
		return NULL;

	syntax = ((synStruct *)hSyn)->syntax;

    /* Sanity check */
    if ((nameId < 0) || (nameId >= syntax->dictionarySize))
        return NULL;

    /* Get the length */
    if (length != NULL)
        *length = syntax->dictionary[nameId] - 1;

    return (syntax->dictionary + nameId + 1);
}


/************************************************************************
 * stGetNodeIdByName
 * purpose: Get the node ID of a type by the type's name string
 * input  : hSyn        - Syntax information
 *          name        - String name
 * return : Type's node ID on success
 *          Negative value on failure
 ************************************************************************/
int stGetNodeIdByName(IN HPST hSyn, IN char *name)
{
    fmt2Elem* elem = stGetElemByName(hSyn, name);
    if (elem != NULL)
        return elem->dataOffset;
    else
        return RV_ERROR_UNKNOWN;
}


/************************************************************************
 * stGetNameIdByName
 * purpose: Get the name ID for a given name string
 * input  : hSyn        - Syntax information
 *          name        - String name
 * return : Name ID on success
 *          Negative value on failure
 ************************************************************************/
int stGetNameIdByName(IN HPST hSyn, IN char *name)
{
    fmt2Elem* elem = stGetElemByName(hSyn, name);
    if (elem != NULL)
        return elem->nameOffset;
    else
        return RV_ERROR_UNKNOWN;
}


/************************************************************************
 * stGetNameIdByNodeId
 * purpose: Get the name ID of a type by the type's node ID
 * input  : hSyn        - Syntax information
 *          nodeId      - Node ID
 * return : Type's name ID on success
 *          Negative value on failure
 ************************************************************************/
int stGetNameIdByNodeId(IN HPST hSyn,IN int nodeId)
{
    RvUint32* node = ((synStruct *)hSyn)->syntax->data + nodeId;

    return ((*node) >> 8);
}


/************************************************************************
 * stGetNameByNodeId
 * purpose: Get the name string of a type by the type's node ID
 * input  : hSyn        - Syntax information
 *          nodeId      - Node ID
 * return : Name string of node on success
 *          NULL on failure
 ************************************************************************/
char* stGetNameByNodeId(IN HPST hSyn, IN int nodeId)
{
    return stGetNameByNameId(hSyn, stGetNameIdByNodeId(hSyn, nodeId), NULL);
}


/************************************************************************
 * stGetChildByNodeAndFieldName
 * purpose: Get a child's node ID by its parent's node ID and the child's
 *          name id.
 * input  : hSyn            - Syntax information
 *          parentNodeId    - Parent's node ID
 *          numFields       - Number of fields in the parent
 *          childNameId     - Name ID of the searched child
 * output : childNodeId     - Node ID of the child
 * return : Child's index on success (1-based)
 *          Negative value on failure
 ************************************************************************/
int stGetChildByNodeAndFieldName(
    IN  HPST    hSyn,
    IN  int     parentNodeId,
    IN  int     numFields,
    IN  int     childNameId,
    OUT int*    childNodeId)
{
    fmt2Struct* syntax = ((synStruct *)hSyn)->syntax;
    int nextId, i;

    /* Sanity checks */
    if ((parentNodeId < 0) || (parentNodeId >= syntax->dataSize)) return RV_ERROR_UNKNOWN;
    nextId = parentNodeId;

    /* Go through the fields and see if any of the match */
    for (i = numFields; i > 0; i--)
    {
        /* Get the next child - it's right after the last node */
        nextId += (syntax->data[nextId] & 0xff);
        if (nextId >= syntax->dataSize) return RV_ERROR_UNKNOWN;

        if ((int)(syntax->data[nextId] >> 8) == childNameId)
        {
            /* Found the node with the same name id - we're done */
            if (childNodeId != NULL)
                *childNodeId = nextId;
            return ((numFields + 1) - i);
        }
    }

    /* Not found */
    return RV_ERROR_UNKNOWN;
}



#ifdef __cplusplus
}
#endif
