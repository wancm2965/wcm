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
  psyntreeStackApi

  Internal Syntax-Tree functions used as API for other stack modules.

  */

#include "rvstdio.h"
#include "intutils.h"
#include "psyntreeDef.h"
#include "psyntreeDb.h"
#include "psyntreeStackApi.h"

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 *
 *                          Private constants
 *
 ************************************************************************/


/************************************************************************
 * ones array
 * This array holds the amount of '1' bits in binary representation for
 * the index value from 0 to 31. We use this to check how many optional
 * fields a type is holding. By checking the number of bits we actually
 * know the position of the optional field inside the type, since it
 * may vary.
 * The comments indicate which kind of field we're looking for in each
 * of the values.
 ************************************************************************/
unsigned char ones[32] =
{
    /* Bits: to, from, fromID, toID, tag */
    0,  /* 00000 */ /* Should never happen */

    /* tag existence */
    1,  /* 00001 */

    /* ofId for arrays */
    1,  /* 00010 */
    2,  /* 00011 */

    /* fromId for strings */
    1,  /* 00100 */
    2,  /* 00101 */
    2,  /* 00110 */
    3,  /* 00111 */

    /* from constraint */
    1,  /* 01000 */
    2,  /* 01001 */
    2,  /* 01010 */
    3,  /* 01011 */
    2,  /* 01100 */
    3,  /* 01101 */
    3,  /* 01110 */
    4,  /* 01111 */

    /* to constraint */
    1,  /* 10000 */
    2,  /* 10001 */
    2,  /* 10010 */
    3,  /* 10011 */
    2,  /* 10100 */
    3,  /* 10101 */
    3,  /* 10110 */
    4,  /* 10111 */
    2,  /* 11000 */
    3,  /* 11001 */
    3,  /* 11010 */
    4,  /* 11011 */
    3,  /* 11100 */
    4,  /* 11101 */
    4,  /* 11110 */
    5   /* 11111 */
};


/************************************************************************
 * stATSearchStruct
 * Association table search struct.
 * Used on calls to RvH323BinarySearch().
 * hSyn         - Syntax tree used
 * compareFunc  - Comparison function to call
 * context      - Context used
 ************************************************************************/
typedef struct
{
    HPST                hSyn;
    pstCompareFunction  compareFunc;
    void*               context;
} stATSearchStruct;






/************************************************************************
 *
 *                          Private functions
 *
 ************************************************************************/


#if (RV_ASN1_CODER_USE_H450 == RV_YES)
/************************************************************************
 * pstSearch
 * purpose: Prepare parameters when trying to search an object inside
 *          an association table (=object set).
 * input  : eKey    - Key element we're searching for
 *          elem    - Element we're currently comparing with
 * output : none
 * return : Negative if the key is lower than elem in dictionary comparison
 *          0 if the key and the element are equal
 *          Positive if the key is higher than elem in dictionary comparison
 ************************************************************************/
static int pstSearch(IN const void *eKey, IN const void *elem)
{
    stATSearchStruct*           key = (stATSearchStruct *)eKey;
    stAssociationTableValue*    atNode = (stAssociationTableValue *)elem;

    return key->compareFunc(key->hSyn, (int)m_ATValueValue(atNode), key->context);
}
#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */








/************************************************************************
 *
 *                          Public functions
 *
 ************************************************************************/


/************************************************************************
 * pstGetChildExt
 * purpose: Return extended information about one of the child nodes of
 *          a parent node
 * input  : hSyn        - Syntax tree used
 *          nodeId      - Parent's node ID
 *          childIndex  - Index of the child (1-based)
 * output : child       - Extended child node information
 * return : Pointer to use with pstGetBrotherExt() on success,
 *          NULL on error
 ************************************************************************/
RvUint32* pstGetChildExt(
    IN  HPST            hSyn,
    IN  RvPstNodeId     nodeId,
    IN  int             childIndex,
    OUT pstChildExt*    child)
{
    stChildExt* fieldInfo;
    RvUint32* err;

    if (child)
        child->index = RV_ERROR_UNKNOWN;  /* assume the worst */

    /* Get the information */
    err = stGetChildByIndex(hSyn, (int)nodeId, childIndex, &fieldInfo);
    if (err != NULL)
    {
        /* Convert it to something readable */
        child->index            = childIndex;
        child->nodeId           = (RvPstNodeId)m_structId(fieldInfo);
        child->fieldId          = m_fieldId(fieldInfo);
        child->isOptional       = m_isOptional(fieldInfo);
        child->isDefault        = m_isDefault(fieldInfo);
        child->isExtended       = m_isExtended(fieldInfo);
#if (RV_ASN1_CODER_USE_H450 == RV_YES)
        child->speciality       = (pstFieldSpeciality)m_isSpecial(fieldInfo);
#endif
        child->enumerationValue = m_enumerationValue(fieldInfo);
    }
    return err;
}


/************************************************************************
 * pstGetBrotherExt
 * purpose: Return extended information about the right brother of a
 *          node. responsibility not to call for a non-existant brother
 *          is on the caller (use num of children)
 * input  : hSyn        - Syntax tree used
 *          childIndex  - index of the child (not really needed)
 *          nodePtr     - Node pointer to which we want the brother
 * output : brother     - Extended child node information
 * return : Pointer to use (responsibly) in subsequent calls to
 *          stGetBrother()
 ************************************************************************/
RvUint32* pstGetBrotherExt(
    IN  HPST            hSyn,
    IN  int             childIndex,
    IN  RvUint32*       nodePtr,
    OUT pstChildExt*    brother)
{
    stChildExt* fieldInfo;
    RvUint32* err;

    if (brother)
        brother->index = RV_ERROR_UNKNOWN;  /* assume the worst */

    /* Get the information */
    err = stGetBrother(hSyn, nodePtr, &fieldInfo);
    if (err != NULL)
    {
        /* Convert it to something readable */
        brother->index            = childIndex;
        brother->nodeId           = (RvPstNodeId)m_structId(fieldInfo);
        brother->fieldId          = m_fieldId(fieldInfo);
        brother->isOptional       = m_isOptional(fieldInfo);
        brother->isDefault        = m_isDefault(fieldInfo);
        brother->isExtended       = m_isExtended(fieldInfo);
#if (RV_ASN1_CODER_USE_H450 == RV_YES)
        brother->speciality       = (pstFieldSpeciality)m_isSpecial(fieldInfo);
#endif
        brother->enumerationValue = m_enumerationValue(fieldInfo);
    }
    return err;
}


int
pstGetField(
            IN  HPST        hSyn,
            IN  RvPstNodeId nodeId,      /* node id of parent */
            IN  int         fieldId,  /* index of child */
            OUT pstChild*   child)
{
    stNodeExt* elementBuffer;
    int fieldNodeId;
    int childIndex;

    if (nodeId < 0)
        return RV_ERROR_UNKNOWN;

    elementBuffer = (stNodeExt *)stGetNodeDataByNodeId(hSyn, (int)nodeId);

    childIndex = stGetChildByNodeAndFieldName(hSyn, (int)nodeId, (int)m_numOfChilds(elementBuffer), fieldId, &fieldNodeId);
    if ((childIndex > 0) && (child != NULL))
    {
        stChildExt *fieldInfo;

        fieldInfo = stGetNodeDataByNodeId(hSyn, fieldNodeId);
        child->index      = childIndex;
        child->nodeId     = (RvPstNodeId)m_structId(fieldInfo);
        child->fieldId    = m_fieldId(fieldInfo);
        child->isOptional = m_isOptional(fieldInfo);
    }

    return childIndex;
}

int
pstGetFieldExt(
            IN  HPST            hSyn,
            IN  RvPstNodeId     nodeId,      /* node id of parent */
            IN  int             fieldId,  /* index of child */
            OUT pstChildExt*    child)
{
    stNodeExt* elementBuffer;
    int fieldNodeId;
    int childIndex;

    if (nodeId < 0)
        return RV_ERROR_UNKNOWN;

    elementBuffer = (stNodeExt *)stGetNodeDataByNodeId(hSyn, (int)nodeId);

    childIndex = stGetChildByNodeAndFieldName(hSyn, (int)nodeId, (int)m_numOfChilds(elementBuffer), fieldId, &fieldNodeId);
    if (childIndex > 0)
    {
        stChildExt *fieldInfo;

        fieldInfo = stGetNodeDataByNodeId(hSyn, fieldNodeId);
        child->index            = childIndex;
        child->nodeId           = (RvPstNodeId)m_structId(fieldInfo);
        child->fieldId          = m_fieldId(fieldInfo);
        child->isOptional       = m_isOptional(fieldInfo);
        child->isDefault        = m_isDefault(fieldInfo);
        child->isExtended       = m_isExtended(fieldInfo);
#if (RV_ASN1_CODER_USE_H450 == RV_YES)
        child->speciality       = (pstFieldSpeciality)m_isSpecial(fieldInfo);
#endif
        child->enumerationValue = m_enumerationValue(fieldInfo);
    }

    return childIndex;
}


RVINTAPI int RVCALLCONV
pstChildIsExtended(
      IN  HPST        hSyn,
      IN  int         nodeId,      /* node id of parent */
      IN  int         childIndex   /* index of child */
      )
{
    stChildExt *fieldInfo;
    RvUint32 * err;

    err = stGetChildByIndex(hSyn, nodeId, childIndex, &fieldInfo);
    if (err == NULL) return RV_ERROR_UNKNOWN;

    return m_isExtended(fieldInfo);
}


int
pstGetFirstExtendedChild(
         IN  HPST    hSyn,
         IN  int     nodeId)
{
    stNodeExt* node;

    node = stGetNodeDataByNodeId(hSyn, nodeId);
    if (node == NULL) return RV_ERROR_UNKNOWN;

    return m_childsBeforeExt(node) + 1;
}


int
pstGetNumberOfOptionalBeforeExtension(
         IN  HPST    hSyn,
         IN  int     nodeId)
{
    stNodeExt* node;

    node = stGetNodeDataByNodeId(hSyn, nodeId);
    if (node == NULL) return RV_ERROR_UNKNOWN;

    return m_numOptBeforeExt(node);
}


/* convert field internal id to field name */
/* null terminated field name */
RVINTAPI char * RVCALLCONV
pstGetFieldNamePtr(
        IN  HPST hSyn,
        IN  RvInt32 fieldId
        )
{
    char* string = stGetNameByNameId(hSyn, fieldId, NULL);
    return (string != NULL) ? string : (char *)"";
}


int /* type of node */
pstGetNodeRangeExt(
      IN  HPST    hSyn,
      IN  int     nodeId,
      OUT int *   from,
      OUT int *   to,
      OUT RvBool*   _fromAbsent,
      OUT RvBool*   _toAbsent)
{
    stNodeExt* node;

    node = (stNodeExt *)stGetNodeDataByNodeId(hSyn, nodeId);
    if (node == NULL) return RV_ERROR_UNKNOWN;

    if (from)  *from = (int)m_from(node);
    if (to)    *to   = (int)m_to(node);

    if (_fromAbsent) *_fromAbsent = (int)(m_flags(node) & synFlagFromAbsent);
    if (_toAbsent) *_toAbsent = (int)(m_flags(node) & synFlagToAbsent);

    return 0;
}


#if (RV_ASN1_CODER_USE_H450 == RV_YES)
int
pstChildIsSpecial(
      IN  HPST        hSyn,
      IN  RvPstNodeId nodeId,      /* node id of parent */
      IN  int         childIndex   /* index of child */
      )
{
    stChildExt *fieldInfo;
    RvUint32 * err;

    err = stGetChildByIndex(hSyn, (int)nodeId, childIndex, &fieldInfo);
    if (err == NULL) return RV_ERROR_UNKNOWN;

    return m_isSpecial(fieldInfo);
}



int pstGetTypeFromConstraint(
            IN  HPST                    hSyn,
            IN  RvPstNodeId             specialTypeNodeId,
            OUT pstTypeFromConstraint*  specialType)
{
    synStruct *syn = (synStruct *)hSyn;
    RvUint8 * node;

    if (!syn  ||  !RV_PST_NODEID_IS_VALID(specialTypeNodeId) || !specialType) return RV_ERROR_UNKNOWN;

    node = (RvUint8 *)stGetNodeDataByNodeId(hSyn, (int)specialTypeNodeId);
    *specialType=*(pstTypeFromConstraint*)node;
    return  RV_TRUE;
}


int pstGetConstrainingField(
            IN  HPST                    hSyn,
            IN  RvPstNodeId             specialTypeNodeId,
            OUT pstConstrainingField*   constrainingField)
{
    synStruct *syn = (synStruct *)hSyn;
    RvUint8 * node;

    if (!syn  ||  specialTypeNodeId < 0 || !constrainingField) return RV_ERROR_UNKNOWN;

    node = (RvUint8 *)stGetNodeDataByNodeId(hSyn, (int)specialTypeNodeId + 3);
    *constrainingField=*(pstConstrainingField*)node;
    return  RV_TRUE;
}


int pstGetFieldOfObjectReference(
            IN  HPST                        hSyn,
            IN  int                         objectNodeId,
            IN  int                         index,
            OUT pstFieldOfObjectReference*  fieldOfObject)
{
    RvUint32* node;
    RvUint32* err;

    if (!hSyn  ||  objectNodeId < 0 || !fieldOfObject) return RV_ERROR_UNKNOWN;

    err = stGetChildByIndexUnbounded(hSyn, objectNodeId, index, &node);
    if (err == NULL) return RV_ERROR_UNKNOWN;

    *fieldOfObject=*(pstFieldOfObjectReference*)node;
    return  RV_TRUE;
}


int pstGetValueTreeStruct(
            IN  HPST                        hSyn,
            IN  int                         vtStructNodeId,
            OUT pstValueTreeStruct*         valueTreeStruct)
{
    stValueTree* node;

    if (!hSyn  ||  vtStructNodeId < 0 || !valueTreeStruct) return RV_ERROR_UNKNOWN;

    node = (stValueTree *)stGetNodeDataByNodeId(hSyn, vtStructNodeId);

    valueTreeStruct->typeReference  = m_valueTreeTypeReference(node);
    valueTreeStruct->isString       = (pstValueType)m_valueTreeIsString(node);
    valueTreeStruct->value          = m_valueTreeValue(node);

    return 0;
}


int pstGetValueNodeStruct(
            IN  HPST                hSyn,
            IN  int                 vtNodeNodeId,
            OUT pstValueNodeStruct* valueTreeNode)
{
    stValueNode* node;

    if (!hSyn  ||  vtNodeNodeId < 0 || !valueTreeNode) return RV_ERROR_UNKNOWN;

    node = (stValueNode *)stGetNodeDataByNodeId(hSyn, vtNodeNodeId);

    valueTreeNode->fieldName    = m_valueNodeFieldName(node);
    valueTreeNode->isString     = (pstValueType)m_valueNodeIsString(node);
    valueTreeNode->value        = m_valueNodeValue(node);

    return 0;
}


/************************************************************************
 * pstFindObjectInAT
 * purpose: Find the type of field from the association table that matches
 *          a given tree node. This is used when trying to encode/decode
 *          messages with object sets.
 * input  : hSyn        - Syntax tree used
 *          atNodeId    - Association table node ID in the syntax tree
 *          compareFunc - Comparison function to use for nodes
 *          context     - Context to use for comparison function
 * output : none
 * return : Node ID of the matching syntax value on success
 *          Negative value on failure
 ************************************************************************/
int pstFindObjectInAT(
    IN HPST                 hSyn,
    IN int                  atNodeId,
    IN pstCompareFunction   compareFunc,
    IN void*                context)
{
    stAssociationTable*         atNode;
    int                         numObjects;
    stAssociationTableValue*    base;
    stAssociationTableValue*    node;
    stATSearchStruct            key;

    /* Find out how many items are then in this association table */
    atNode = (stAssociationTable *)stGetNodeDataByNodeId(hSyn, atNodeId);
    numObjects = m_ATNumObjects(atNode);

    /* Find the position of the first object in the association table */
    base = (stAssociationTableValue *)m_ATTable(atNode);

    /* Set the key to search for */
    key.hSyn = hSyn;
    key.compareFunc = compareFunc;
    key.context = context;

    /* Use binary search to look for the specific object id that matches */
    node = (stAssociationTableValue *)
        RvBsearch(&key, base, (RvSize_t)numObjects, m_ATObjectSize(atNode), pstSearch);

    /* Make sure we've got something */
    if (node == NULL) return RV_ERROR_UNKNOWN;

    /* Found! get the syntax tree for this object */
    return m_ATValueObject(node);
}
#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */


/* RV_TRUE if nodes have the same structure */
RVINTAPI RvBool RVCALLCONV
pstAreNodesCongruent(
            IN HPST         hSyn1,
            IN RvPstNodeId  synNodeId1,
            IN HPST         hSyn2,
            IN RvPstNodeId  synNodeId2)
{
    synStruct *syn1 = (synStruct *)hSyn1;
    synStruct *syn2 = (synStruct *)hSyn2;
    stChildExt *fieldInfo1 = NULL;
    stChildExt *fieldInfo2 = NULL;
    stNodeExt *node1=NULL;
    stNodeExt *node2=NULL;
    RvUint32* iter1;
    RvUint32* iter2;
    RvInt i;

    if (syn1==NULL || syn2==NULL ||
       (syn1->syntax != syn2->syntax)) return RV_FALSE;

    if (!synNodeId1) synNodeId1=pstGetRoot(hSyn1);
    if (!synNodeId2) synNodeId2=pstGetRoot(hSyn2);

    node1 = (stNodeExt *)stGetNodeDataByNodeId(hSyn1, (int)synNodeId1);
    node2 = (stNodeExt *)stGetNodeDataByNodeId(hSyn2, (int)synNodeId2);
    if (!node1 || !node2) return RV_FALSE;

    if (m_isExtension(node1) != m_isExtension(node2)) return RV_FALSE;
    if (m_numOfChilds(node1) != m_numOfChilds(node2)) return RV_FALSE;
    if (m_from(node1) != m_from(node2)) return RV_FALSE;
    if (m_to(node1) != m_to(node2)) return RV_FALSE;
    if (m_ofId(node1) != m_ofId(node2)) return RV_FALSE;
    if (m_fromId(node1) != m_fromId(node2)) return RV_FALSE;

    /* check childs */
    iter1 = stGetChildByIndex(hSyn1, synNodeId1, 1, &fieldInfo1);
    iter2 = stGetChildByIndex(hSyn1, synNodeId2, 1, &fieldInfo2);
    for (i=1; i<=(RvInt)m_numOfChilds(node1); i++)
    {
        /*if (child1->fieldId != child2->fieldId) return RV_FALSE;*/
        if (m_structId(fieldInfo1) != m_structId(fieldInfo2)) return RV_FALSE;
        if (m_isExtension(fieldInfo1) != m_isExtension(fieldInfo2)) return RV_FALSE;
        if (m_isOptional(fieldInfo1) != m_isOptional(fieldInfo2)) return RV_FALSE;
        iter1 = stGetBrother(hSyn1, iter1, &fieldInfo1);
        iter2 = stGetBrother(hSyn2, iter2, &fieldInfo2);
    }

    return RV_TRUE;
}


char* /* actual length of the fromString or negative value on failure */
pstGetFROMStringPtr(
         /* Get the character constraints of the syntax node */
         IN  HPST hSyn,
         IN  RvPstNodeId nodeId,
         OUT int*actualLength)
{
    stNodeExt* node=NULL;
    char* string=NULL;

    *actualLength = 0;
    node = (stNodeExt *)stGetNodeDataByNodeId(hSyn, (int)nodeId);
    if (node == NULL) return NULL;

    if(m_fromId(node)<0) return NULL;

    string = stGetNameByNameId(hSyn, m_fromId(node), actualLength);

    return string;
}


RVINTAPI RvPstNodeId RVCALLCONV
pstGetRoot(IN HPST hSyn)
{
    synStruct *syn = (synStruct *)hSyn;

    if (!syn)
        return RV_PST_ERROR_UNKNOWN;

    return syn->rootNodeId;
}

#ifdef __cplusplus
}
#endif



