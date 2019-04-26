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
  pvaltree.c

  Holds message values to be used by coding systems and message compilers.

  Parameters:
   - parentId: Id of parent node. Unique node identifier.
   - fieldEnum: enumeration of field as in the message.
   - value: integer value of node.
   - index: of child under parent. >=1

  Version 1: Ron S. 18 July 1996. Add syntax tree association.
  Now any value tree construction should be associated with a syntax tree. Each value node
  holds reference to the correlated syntax tree node. So you may get syntax node directly
  from the vtGet() function.


   */

#ifdef __cplusplus
extern "C" {
#endif

#include "rvtypes.h"
#include <stdlib.h>

#include "rvstdio.h"
#include "rvmemory.h"
#include "rvlock.h"
#include "rvlog.h"
#include "rtree.h"
#include "rpool.h"
#include "h32xLog.h"
#include "intutils.h"
#include "strutils.h"
#include "psyntreeDb.h"
#include "psyntreeStackApi.h"
#include "pvaltreeDef.h"
#include "pvaltreeStackApi.h"
#include "pvaltree.h"


#define nprn(s) ((s)?(s):"(null)")


typedef struct
{
    vtStruct*   dest;
    vtStruct*   src;
} vtStructs;



/* We check here that the values of the following macros is the same.
   Don't remove this check and don't change the value of RV_PVT_LOGDEBUG
   to something other than a constant value. The reason for that is the
   fact that we don't want to include rvlog.h in the header file pvaltree.h */
#if (RV_PVT_LOGDEBUG != RV_LOGLEVEL_DEBUG)
#error RV_PVT_LOGDEBUG must have the same value as RV_LOGLEVEL_DEBUG!
#endif




/************************************************************************
 *
 *                          Private functions
 *
 ************************************************************************/

static RvBool vtNodeCompare(RTElement e1, void *param)
{
    return ( (RvPstFieldId)VTN_SYN_FIELD((vtNode *)e1) == (RvPstFieldId)param );
}


static void *vtAddTreeFunc(HRTREE rtH, RTElement element, void *vts)
{
    vtNode *node = (vtNode *)element;
    vtStructs* v = (vtStructs *)vts;
    HRPOOLELEM string;
    RV_UNUSED_ARG(rtH);

    if (!vts || !node) return NULL;
    string = node->string;
    if (string)
    {
        if (!(node->string = rpoolAllocCopyInternal(v->dest->sPool, v->src->sPool, string, (int)node->value)))
        {
            RvLogError(&v->dest->log,
                (&v->dest->log, "vtAddTreeFunc: String allocation failed for '%d' (%d).",
                     (int)string, (int)node->value));
            return NULL;
        }
    }

    return vts;
}



/************************************************************************
 *
 *                          Public functions
 *
 ************************************************************************/


/************************************************************************
 * pvtConstruct
 * purpose: Construct a pool of PVT nodes.
 * input  : stringBufferSize    - Not used
 *          numOfNodes          - The maximum number of nodes to be allowed
 *                                in the tree
 * output : none
 * return : Handle to PVT constructed on success
 *          NULL on failure
 ************************************************************************/
RVAPI HPVT RVCALLCONV pvtConstruct(
    IN  int stringBufferSize,
    IN  int numOfNodes)
{
    vtStruct *vt;
    RV_UNUSED_ARG(stringBufferSize);

    if (RvMemoryAlloc(NULL, sizeof(vtStruct), H32xLogGet(), (void**)&vt) != RV_OK)
        return NULL;
    memset(vt, 0, sizeof(vtStruct));
    
    vt->logMgr = H32xLogGet();

    RvLogSourceConstruct(vt->logMgr, &vt->log, "VT", "Value Tree");
    RvLogSourceConstruct(vt->logMgr, &vt->unregLog, "UNREG", "Any non-registered user log");

    vt->vTree = rtConstruct(sizeof(vtNode), numOfNodes, "VT tree", vt->logMgr);
    vt->sPool = (HRPOOL)vt->vTree;/*rpoolConstruct(stringBufferSize);*/

    RvLockConstruct(vt->logMgr, &vt->lock);

    if (!vt->vTree || !vt->sPool)
    {
        pvtDestruct((HPVT)vt);
        return NULL;
    }

    rtSetCompareFunc(vt->vTree, vtNodeCompare);
    rtSetPrintFunc(vt->vTree, vtNodePrint);

    return (HPVT)vt;
}


/************************************************************************
 * pvtDestruct
 * purpose: Destruct a pool of PVT nodes.
 * input  : hVal    - The PVT handle
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtDestruct(
    IN  HPVT hVal)
{
    vtStruct *vt = (vtStruct *)hVal;

    if (vt == NULL)
        return RV_ERROR_NULLPTR;

    if (rtCurSize(vt->vTree) > 0)
    {
        RvLogWarning(&vt->log, (&vt->log,
            "pvtDestruct: %d nodes left in database", rtCurSize(vt->vTree)));
    }

    rtDestruct(vt->vTree);
    RvLockDestruct(&vt->lock, vt->logMgr);
    RvLogSourceDestruct(&vt->log);
    RvLogSourceDestruct(&vt->unregLog);

    RvMemoryFree(vt, vt->logMgr);

    return RV_OK;
}


/************************************************************************
 * pvtGetRoot
 * purpose: Returns the Node ID of the root of a Value Tree to which a
 *          specified node belongs
 * input  : hVal    - The PVT handle
 *          nodeId  - The ID of the node inside a tree
 * output : none
 * return : Root node ID of the given node ID on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtGetRoot(
    IN  HPVT         hVal,
    IN  RvPvtNodeId  nodeId)
{
    RvPvtNodeId result;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL)
        return RV_ERROR_NULLPTR;

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtGetRoot(vt, nodeId);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtGetSynTree
 * purpose: Returns a handle to the structure (Syntax Tree) definition of
 *          a specified node
 * input  : hVal    - The PVT handle
 *          nodeId  - The ID of the node the syntax of which is returned
 * output : none
 * return : A handle to the structure (Syntax Tree) definition on success
 *          NULL on failure
 ************************************************************************/
RVAPI HPST RVCALLCONV pvtGetSynTree(
    IN  HPVT          hVal,
    IN  RvPvtNodeId   nodeId)
{
    HPST        result = NULL;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL)
        return NULL;


    RvLockGet(&vt->lock, vt->logMgr);
    result = vtGetSynTree(vt, nodeId);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}



/*** Statistics ***/

/************************************************************************
 * pvtCurSize
 * purpose: Gets the number of nodes currently used in the Value Tree forest
 * input  : hVal    - The PVT handle
 * output : none
 * return : The number of nodes in the Value Tree forest on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtCurSize(
    IN HPVT hVal)
{
    vtStruct *vt = (vtStruct *)hVal;
    if (!vt) return RV_ERROR_NULLPTR;
    return rtCurSize(vt->vTree);
}


/************************************************************************
 * pvtMaxUsage
 * purpose: Gets the highest number of nodes used in the Value Tree forest
 *          since the cmInitialize() function was called.
 * input  : hVal    - The PVT handle
 * output : none
 * return : The maximum number of nodes used in the Value Tree forest on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtMaxUsage(
    IN HPVT hVal)
{
    vtStruct *vt = (vtStruct *)hVal;
    if (!vt) return RV_ERROR_NULLPTR;
    return rtMaxUsage(vt->vTree);
}


/************************************************************************
 * pvtMaxSize
 * purpose: Gets the highest number of nodes that cab be used in the Value Tree forest
 * input  : hVal    - The PVT handle
 * output : none
 * return : The maximum number of nodes in the Value Tree forest on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtMaxSize(
    IN HPVT hVal)
{
    if (!hVal) return RV_ERROR_NULLPTR;
    return rtMaxSize(((vtStruct *)hVal)->vTree);
}


/************************************************************************
 * pvtPoolStatistics
 * purpose: Get pool statistics (space is in bytes)
 * input  : hVal            - The PVT handle
 * output : poolSize        - Maximum size of pool
 *          availableSpace  - Current available space
 *          maxFreeChunk    - Always returned as 0
 *          numOfChunks     - Always returned as 0
 *          If any output parameter is set to NULL, it will be discarded
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtPoolStatistics(
    IN  HPVT     hVal,
    OUT RvInt32* poolSize,
    OUT RvInt32* availableSpace,
    OUT RvInt32* maxFreeChunk,
    OUT RvInt32* numOfChunks)
{
    vtStruct    *vt = (vtStruct *)hVal;
    RvInt32     usedSize, allocSize;
    RvStatus    status;

    if (vt == NULL)
        return RV_ERROR_NULLPTR;

    if (maxFreeChunk) *maxFreeChunk=0;
    if (numOfChunks)  *numOfChunks=0;
    status = rpoolStatistics(vt->sPool, &allocSize, NULL, &usedSize);
    if (status == RV_OK)
    {
        if (poolSize != NULL)
            *poolSize = allocSize;
        if (availableSpace != NULL)
            *availableSpace = allocSize - usedSize;
    }

    return status;
}



/************************************************************************
 * pvtTreeSize
 * purpose: Returns the number of nodes included in a Value Tree root
 * input  : hVal        - The PVT handle
 *          parentId    - The ID of any node. The function returns the
 *                        number of nodes located under the specified parent.
 * output : none
 * return : The number of nodes included in a Value Tree on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtTreeSize(
    IN  HPVT        hVal,
    IN  RvPvtNodeId parentId)
{
    int         result;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL)
        return RV_ERROR_NULLPTR;

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtTreeSize(vt, parentId);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtNumChilds
 * purpose: Returns the number of the dependents (children) of a parent tree
 * input  : hVal        - The PVT handle
 *          parentId    - The ID of any node
 * output : none
 * return : The number of immediate nodes under the given node on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtNumChilds(
    IN  HPVT        hVal,
    IN  RvPvtNodeId parentId)
{
    int         result;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL)
        return RV_ERROR_NULLPTR;

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtNumChilds(vt, parentId);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtParent
 * purpose: Returns the ID of the parent node of a specified node
 * input  : hVal        - The PVT handle
 *          nodeId      - The ID of any node
 * output : none
 * return : The parent ID of the given node on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtParent(
      IN  HPVT          hVal,
      IN  RvPvtNodeId   nodeId)
{
    RvPvtNodeId result;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL)
        return RV_ERROR_NULLPTR;

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtParent(vt, nodeId);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtBrother
 * purpose: Returns the Node ID of a specified node's brother (right).
 * input  : hVal        - The PVT handle
 *          nodeId      - The ID of any node
 * output : none
 * return : The node ID of the given node's brother on success
 *          Negative value on failure
 * The function returns the next child (rightward). Use pvtChild() to get
 * the first dependent, and then use pvtBrother() to get to the next brother.
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtBrother(
       IN  HPVT         hVal,
       IN  RvPvtNodeId  nodeId)
{
    RvPvtNodeId result;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL)
        return RV_ERROR_NULLPTR;

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtBrother(vt, nodeId);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtLBrother
 * purpose: Gets the ID of the node before (left) a particular node
 * input  : hVal        - The PVT handle
 *          nodeId      - The ID of any node
 * output : none
 * return : The node ID of the previous node on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtLBrother(
    IN  HPVT         hVal,
    IN  RvPvtNodeId  nodeId)
{
    vtStruct    *vt = (vtStruct *)hVal;
    RvPvtNodeId parentId;
    RvPvtNodeId lbrotherId = RV_PVT_ERROR_UNKNOWN;
    RvPvtNodeId currId;

    RvLockGet(&vt->lock, vt->logMgr);
    parentId = vtParent(vt, nodeId);
    if (!RV_PVT_NODEID_IS_VALID(parentId))
    {
        RvLockRelease(&vt->lock, vt->logMgr);
        return RV_PVT_ERROR_UNKNOWN;
    }

    currId = vtChild(vt, parentId);
    while (currId != nodeId)
    {
        if (!RV_PVT_NODEID_IS_VALID(parentId))
        {
            RvLockRelease(&vt->lock, vt->logMgr);
            return RV_PVT_ERROR_UNKNOWN;
        }
        lbrotherId = currId;
        currId = vtBrother(vt, currId);
    }

    RvLockRelease(&vt->lock, vt->logMgr);
    return lbrotherId;
}


/************************************************************************
 * pvtChild
 * purpose: Returns the Node ID of the first (leftmost) child of a parent.
 * input  : hVal        - The PVT handle
 *          parentId    - The ID of any node
 * output : none
 * return : The Node ID of the first (leftmost) child of a parent on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtChild(
    IN  HPVT         hVal,
    IN  RvPvtNodeId  parentId)
{
    RvPvtNodeId result;
    vtStruct    *vt = (vtStruct *)hVal;
    if (hVal == NULL)
        return RV_ERROR_NULLPTR;

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtChild(vt, parentId);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtNext
 * purpose: Returns the ID of a node located after a specified node.
 * input  : hVal    - The PVT handle
 *          rootId  - The ID of the root node of the current Value Tree
 *          location- A Node ID inside the given root
 * output : none
 * return : The Node ID of the next node in the tree on success - this
 *          value should be given as the location on the next call.
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtNext(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     rootId,
    IN  RvPvtNodeId     location)
{
    RvPvtNodeId result;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL)
        return RV_ERROR_NULLPTR;

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtNext(vt, rootId, location);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtAddRootFromInt
 * purpose: Add a new root to PVT. This function should not be used
 *          directly by user applications. It is called internally through
 *          macros or other stack functions.
 * input  : hVal    - PVT handle to use
 *          hSyn    - Syntax tree to use
 *                    Can be set to -1 if it will be supplied later (by pvtSetTree)
 *          nodePath- Syntax node Id to use for the root.
 *                    Can be set to -1 if it will be supplied later or if
 *                    it should be the root of the syntax tree created.
 *          value   - Value of the root node, or length of the value if
 *                    value is a string
 *          string  - String value of the node, or NULL if not a string
 *          fileName- Filename that called this function
 *                    NULL if unknown
 *          lineno  - Line number in the filename that called this function
 *                    0 if unknown
 * output : none
 * return : Node id of the added root on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV
pvtAddRootFromInt(
    IN  HPVT            hVal,
    IN  HPST            hSyn,
    IN  RvPstNodeId     nodePath,
    IN  RvInt32         value,
    IN  const RvChar*   string,
    IN  const RvChar*   fileName,
    IN  int             lineno)
{
    RvPvtNodeId path;
    RvPvtNodeId result;
    vtStruct    *vt = (vtStruct *)hVal;
    
    RV_UNUSED_ARG(lineno);
    RV_UNUSED_ARG(fileName);

    if (vt == NULL) return RV_PVT_ERROR_UNKNOWN;
    RvLockGet(&vt->lock, vt->logMgr);

    /* Try and add the new root */
    path = (RvPvtNodeId)rtAddRoot(vt->vTree, NULL);
    if (!RV_PVT_NODEID_IS_VALID(path))
    {
        RvLogError(&vt->log,
            (&vt->log, "pvtAddRootFromInt: (@%s:%d) Cannot add root [size=%d]",
                 nprn(fileName), lineno, rtCurSize(vt->vTree)));
        RvLockRelease(&vt->lock, vt->logMgr);
        return RV_PVT_ERROR_OUTOFRESOURCES;
    }

    RvLogDebug(&vt->log,
        (&vt->log, "pvtAddRootFromInt: (@%s:%d) hVal=%p. Added root=%d",
             nprn(fileName), lineno, hVal, path));

    /* Make sure we know the syntax node Id of the added node */
    if (!RV_PST_NODEID_IS_VALID(nodePath))
        nodePath = pstGetRoot(hSyn);

    /* Set the value of the root node */
    result = vtSetNode(vt, hSyn, path, nodePath, 
                RV_PVT_ERROR_UNKNOWN, RV_ERROR_UNKNOWN, value, string, NULL);

    RvLockRelease(&vt->lock, vt->logMgr);
    return result;
}


/* Remove definitions of macros for these functions. The definitions are there to allow
   printing information about these functions */
#if defined(pvtAddRoot) || defined(pvtAddRootByPath)
#undef pvtAddRoot
#undef pvtAddRootByPath
#endif


/************************************************************************
 * pvtAddRoot
 * purpose: Adds a new node that constitutes the root of a new tree.
 * input  : hVal    - PVT handle to use
 *          synH    - Syntax tree to use
 *                    Can be set to -1 if it will be supplied later (by pvtSetTree)
 *          value   - Value of the root node, or length of the value if
 *                    value is a string
 *          string  - String value of the node, or NULL if not a string
 * output : none
 * return : Node id of the added root on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtAddRoot(
    IN  HPVT            hVal,
    IN  HPST            synH,
    IN  RvInt32         value,
    IN  const RvChar*   string)
{
    return pvtAddRootFromInt(
        hVal, synH, RV_PST_INVALID_NODEID, value,  string, __FILE__, __LINE__);
}



/************************************************************************
 * pvtAddRootByPath
 * purpose: Adds a new node that constitutes the root of a new tree.
 * input  : hVal        - PVT handle to use
 *          synH        - Syntax tree to use
 *                        Can be set to -1 if it will be supplied later (by pvtSetTree)
 *          syntaxPath  - A path separated by periods (for example, "a.b.c") that
 *                        identifies the node of the Value Tree's syntax node.
 *                        The path starts from the syntax handle root node
 *          value       - Value of the root node, or length of the value if
 *                        value is a string
 *          string      - String value of the node, or NULL if not a string
 * output : none
 * return : Node id of the added root on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtAddRootByPath(
    IN  HPVT            hVal,
    IN  HPST            synH,
    IN  const RvChar*   syntaxPath,
    IN  RvInt32         value,
    IN  const RvChar*   string)
{
    return pvtAddRootFromInt(
        hVal, synH, pstGetNodeIdByPath(synH, syntaxPath), value, string, __FILE__, __LINE__);
}




/************************************************************************
 * pvtAdd
 * purpose: Add child node under parentId.
 *          The new node is placed in its relative position according to syntax tree
 *          indexing of SEQUENCE fields of structure.
 * input  : hVal        - PVT handle to use
 *          parentId    - The Node ID of the immediate parent of the new node.
 *          fieldId     - The field ID returned by pstGetFieldId().
 *                        If set to -1, it will be copied from the parent node.
 *          value       - Value of the root node, or length of the value if
 *                        value is a string
 *          string      - String value of the node, or NULL if not a string
 * output : index       - The index of the added child (1-based)
 * return : Node id of the added node on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtAdd(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     parentId,
    IN  RvPstFieldId    fieldId,
    IN  RvInt32         value,
    IN  const RvChar*   string,
    OUT int*            index)
{
    RvPvtNodeId result;
    vtStruct* vt = (vtStruct *)hVal;

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtAddSyn(vt, (HPST)-1, parentId, fieldId, value, string, NULL, index);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtAddTree
 * purpose: Copies a sub-tree and places it under a specified parent in
 *          another sub-tree.
 * input  : destH       - The handle returned by cmGetValTree() for the
 *                        parent (destination).
 *          destNodeId  - The Node ID of the parent (destination).
 *                        The new sub-tree is placed under this node.
 *          srcH        - The handle returned by cmGetValTree() for the
 *                        source (copied sub-tree).
 *          srcNodeId   - The ID of the root node of the source sub-tree.
 * output : none
 * return : Node id of the added node on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtAddTree(
    IN  HPVT        destH,
    IN  RvPvtNodeId destNodeId,
    IN  HPVT        srcH,
    IN  RvPvtNodeId srcNodeId)
{
    vtStructs   v;
    vtStruct *vt1, *vt2;
    RvPvtNodeId result;

    if (!destH || !srcH) 
        return RV_ERROR_NULLPTR;

    v.dest = (vtStruct *)destH;
    v.src = (vtStruct *)srcH;

    /* Make sure we lock an an orderly fashion to diminish dead-locking possibilities */
    if (destH > srcH)
    {
        vt1 = v.src;
        vt2 = v.dest;
    }
    else
    {
        vt1 = v.dest;
        vt2 = v.src;
    }

    RvLockGet(&vt1->lock, vt1->logMgr);
    if (vt1 != vt2) RvLockGet(&vt2->lock, vt2->logMgr);

    result = (RvPvtNodeId)rtAdd(v.dest->vTree, (int)destNodeId, v.src->vTree, (int)srcNodeId, vtAddTreeFunc, (void *)&v);
    
    if (vt1 != vt2) RvLockRelease(&vt2->lock, vt2->logMgr);
    RvLockRelease(&vt1->lock, vt1->logMgr);

    return result;
}


/************************************************************************
 * pvtAddChilds
 * purpose: Copies a sub-tree, excluding its root node, and places it under
 *          a specified parent.
 * input  : destH       - The handle returned by cmGetValTree() for the
 *                        parent (destination).
 *          destNodeId  - The Node ID of the parent (destination).
 *                        The new sub-tree is placed under this node.
 *          srcH        - The handle returned by cmGetValTree() for the
 *                        source (copied sub-tree).
 *          srcNodeId   - The ID of the root node of the source sub-tree.
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtAddChilds(
    IN  HPVT        destH,
    IN  RvPvtNodeId destNodeId,
    IN  HPVT        srcH,
    IN  RvPvtNodeId srcNodeId)
{
    vtStructs   v;
    vtStruct    *vt1;
    vtStruct    *vt2;
    int         result;

    if (!destH || !srcH) 
        return RV_ERROR_NULLPTR;

    v.dest = (vtStruct *)destH;
    v.src = (vtStruct *)srcH;

    /* Make sure we lock an an orderly fashion to diminish dead-locking possibilities */
    if (destH > srcH)
    {
        vt1 = v.src;
        vt2 = v.dest;
    }
    else
    {
        vt1 = v.dest;
        vt2 = v.src;
    }

    RvLockGet(&vt1->lock, vt1->logMgr);
    if (vt1 != vt2) RvLockGet(&vt2->lock, vt2->logMgr);
    result = rtAddChilds(v.dest->vTree, (int)destNodeId, v.src->vTree, (int)srcNodeId, vtAddTreeFunc, (void *)&v);
    if (vt1 != vt2) RvLockRelease(&vt2->lock, vt2->logMgr);
    RvLockRelease(&vt1->lock, vt1->logMgr);

    return result;
}


/************************************************************************
 * pvtDeleteDbg
 * purpose: Deletes a sub-tree, with debug information
 * input  : hVal            - PVT handle to use
 *          subTreeRootId   - The ID of the root node to delete
 *          fileName        - File from which pvtDelete was called
 *          lineno          - Line number in the file
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtDeleteDbg(
    IN  HPVT          hVal,
    IN  RvPvtNodeId   subTreeRootId,
    IN  const char*   fileName,
    IN  int           lineno)
{
    vtStruct    *vt = (vtStruct *)hVal;
    int         result;


    if (!RV_PVT_NODEID_IS_VALID(subTreeRootId))
        return RV_PVT_INVALID_NODEID;

    RvLockGet(&vt->lock, vt->logMgr);

#if defined(RV_DEBUG)
    /* We'd like to make sure this node isn't a shielded node - we do that to reduce the
       amount of bugs our customers are going to have while developing. */
    {
        vtNode * node;
        node = (vtNode *)rtGetByPath(vt->vTree, (int)subTreeRootId);
        if (node == NULL)
        {
            RvLogError(&vt->log,
                (&vt->log, "pvtDelete: hVal=%p. Attempted to delete an unknown node=%d (%s:%d)", hVal, subTreeRootId, fileName, lineno));
            RvLockRelease(&vt->lock, vt->logMgr);
            return RV_PVT_ERROR_UNKNOWN;
        }

        if (node->shieldedNode)
        {
            RvLogError(&vt->log,
                (&vt->log, "pvtDelete: hVal=%p. Attempted to delete shielded root=%d (%s:%d)", hVal, subTreeRootId, fileName, lineno));
            RvLockRelease(&vt->lock, vt->logMgr);
            return RV_PVT_ERROR_UNKNOWN;
        }
    }
#endif

#if (RV_LOGMASK & RV_LOGLEVEL_DEBUG)
    /* If it's a root node, we notify that it's being deleted */
    if (rtParent(vt->vTree, (int)subTreeRootId) < 0)
    {
        RvLogDebug(&vt->log,
            (&vt->log, "pvtDelete: hVal=%p. Deleted root=%d (%s:%d)", hVal, subTreeRootId, fileName, lineno));
    }
#else
    /* Needed when compiling without logs */
    RV_UNUSED_ARG(lineno);
    RV_UNUSED_ARG(fileName);
#endif

    result = rtDelete(vt->vTree, (int)subTreeRootId, vtDeleteFunc, (void *)&hVal);
    RvLockRelease(&vt->lock, vt->logMgr);

#if defined(RV_DEBUG)
    /* If it's a root node, we notify that it's being deleted */
    if (result < 0)
    {
        RvLogExcep(&vt->log,
            (&vt->log, "pvtDelete: hVal=%p. Cannot delete %d - probably already deleted (%s:%d)", hVal, subTreeRootId, fileName, lineno));
    }
#endif

    return result;
}



/* Remove definitions of macro for these functions. The definitions are there to allow
   printing information about these functions */
#if defined(pvtDelete)
#undef pvtDelete
#endif

/************************************************************************
 * pvtDelete
 * purpose: Deletes a sub-tree
 * input  : hVal            - PVT handle to use
 *          subTreeRootId   - The ID of the root node to delete
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtDelete(
    IN  HPVT          hVal,
    IN  RvPvtNodeId   subTreeRootId)
{
    vtStruct    *vt = (vtStruct *)hVal;
    
    if (!RV_PVT_NODEID_IS_VALID(subTreeRootId))
        return RV_PVT_INVALID_NODEID;
    
    return vtDelete(vt, subTreeRootId);
}


/************************************************************************
 * pvtDeleteChilds
 * purpose: Deletes the children of root node.
 * input  : hVal            - PVT handle to use
 *          subTreeRootId   - The ID of the root node to delete
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtDeleteChilds(
    IN  HPVT        hVal,
    IN  RvPvtNodeId subTreeRootId)
{
    int         result;
    vtStruct    *vt = (vtStruct *)hVal;


    if (!RV_PVT_NODEID_IS_VALID(subTreeRootId))
        return RV_PVT_INVALID_NODEID;

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtDeleteChilds(vt, subTreeRootId);
    RvLockRelease(&vt->lock, vt->logMgr);

#if defined(RV_DEBUG)
    /* If it's a root node, we notify that it's being deleted */
    if (result < 0)
    {
        RvLogExcep(&vt->log,
            (&vt->log, "pvtDeleteChilds: hVal=%p. Cannot delete %d - probably already deleted", hVal, subTreeRootId));
    }
#endif

    return result;
}


/************************************************************************
 * pvtDeleteAll
 * purpose: Deletes all the nodes in a Value Tree.
 * input  : hVal            - PVT handle to use
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtDeleteAll(
    IN  HPVT hVal)
{
    vtStruct *vt = (vtStruct *)hVal;

    if (hVal == NULL)
        return RV_ERROR_NULLPTR;

    RvLockGet(&vt->lock, vt->logMgr);
    rtClear(vt->vTree);
    rpoolClear(vt->sPool);

    RvLogDebug(&vt->log,
        (&vt->log, "pvtDeleteAll: Called for hVal=%p", hVal));
    RvLockRelease(&vt->lock, vt->logMgr);

    return RV_TRUE;
}


/************************************************************************
 * pvtSetTree
 * purpose: Copies a sub-tree from one Value Tree to another,
 *          overwriting the destination's tree
 * input  : destH       - The handle returned by cmGetValTree() for the
 *                        parent (destination).
 *          destNodeId  - The Node ID of the destination sub-tree.
 *                        The copied sub-tree overwrites this one
 *          srcH        - The handle returned by cmGetValTree() for the
 *                        source (copied sub-tree).
 *          srcNodeId   - The ID of the root node of the source sub-tree.
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtSetTree(
    IN  HPVT        destH,
    IN  RvPvtNodeId destNodeId,
    IN  HPVT        srcH,
    IN  RvPvtNodeId srcNodeId)
{
    vtStructs   v;
    RvUint32    destSynFieldAndNode;
    HPST        destSynH;
    vtNode*     node;
    vtStruct    *vt1, *vt2;
    int         result;

    if (!destH || !srcH) 
        return RV_ERROR_NULLPTR;
    if (!RV_PVT_NODEID_IS_VALID(destNodeId) || !RV_PVT_NODEID_IS_VALID(srcNodeId))
        return RV_PVT_INVALID_NODEID;

    if ((destH == srcH) && (destNodeId == srcNodeId)) 
        return RV_TRUE;

    v.dest = (vtStruct *)destH;
    v.src = (vtStruct *)srcH;

    /* Make sure we lock an an orderly fashion to diminish dead-locking possibilities */
    if (destH > srcH)
    {
        vt1 = v.src;
        vt2 = v.dest;
    }
    else
    {
        vt1 = v.dest;
        vt2 = v.src;
    }

    RvLockGet(&vt1->lock, vt1->logMgr);
    if (vt1 != vt2) RvLockGet(&vt2->lock, vt2->logMgr);

    if (!(node = (vtNode *)rtGetByPath(v.dest->vTree, (int)destNodeId)))
        result = RV_ERROR_UNKNOWN;
    else
    {
        destSynFieldAndNode = node->synFieldAndNode;
        destSynH = node->hSyn;

        result = rtSet(v.dest->vTree, (int)destNodeId, v.src->vTree, (int)srcNodeId,
            vtAddTreeFunc, vtDeleteFunc, (void*)&v);
        if (result >= 0)
        {
            if (destSynH)
                node->synFieldAndNode = destSynFieldAndNode;
#ifdef RV_DEBUG
            node->shieldedNode = RV_FALSE; /* Copied nodes should never be shielded... */
#endif
            result = RV_TRUE;
        }
    }

    if (vt1 != vt2) RvLockRelease(&vt2->lock, vt2->logMgr);
    RvLockRelease(&vt1->lock, vt1->logMgr);

    return result;
}


/************************************************************************
 * pvtMoveTree
 * purpose: Moves a sub-tree to another location within the same Value Tree
 * input  : destH       - The handle returned by cmGetValTree() for the
 *                        parent (destination).
 *          destRootId  - The ID of the root node to which the sub-tree is
 *                        moved (destination root) - this node is overwritted.
 *          srcRootId   - The ID of the root node to which the sub-tree
 *                        belongs (source node).
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtMoveTree(
    IN  HPVT        destH,
    IN  RvPvtNodeId destRootId,
    IN  RvPvtNodeId srcRootId)
{
    vtStruct *vtDest = (vtStruct *)destH;
    RvUint32 destSynFieldAndNode;
    HPST     destSynH;
    vtNode   *node;

    if (destH == NULL)
        return RV_ERROR_NULLPTR;
    if (!RV_PVT_NODEID_IS_VALID(srcRootId))
        return RV_PVT_INVALID_NODEID;

    RvLockGet(&vtDest->lock, vtDest->logMgr);

#if defined(RV_DEBUG)
    /* We'd like to make sure this node isn't a shielded node - we do that to reduce the
       amount of bugs our customers are going to have while developing. */
    node = (vtNode *)rtGetByPath(vtDest->vTree, (int)srcRootId);
    if ((node != NULL) && node->shieldedNode)
    {
        RvLogError(&vtDest->log,
            (&vtDest->log, "pvtMoveTree: hVal=%p. Attempted to delete shieleded root=%d", destH, srcRootId));
        RvLockRelease(&vtDest->lock, vtDest->logMgr);
        return RV_PVT_ERROR_UNKNOWN;
    }
#endif

    node = (vtNode *)rtGetByPath(vtDest->vTree, (int)destRootId);
    if (node == NULL)
    {
        RvLockRelease(&vtDest->lock, vtDest->logMgr);
        return RV_ERROR_UNKNOWN;
    }

    destSynFieldAndNode = node->synFieldAndNode;
    destSynH = node->hSyn;

#if (RV_LOGMASK & RV_LOGLEVEL_DEBUG)
    /* If it's a root node, we notify that it's being moved */
    if (rtParent(vtDest->vTree, (int)srcRootId) < 0)
    {
        RvLogDebug(&vtDest->log,
            (&vtDest->log, "pvtMoveTree: hVal=%p. root=%d is being moved as a sub-tree",
                 destH, srcRootId));
    }
#endif

    if (rtMove(vtDest->vTree, (int)destRootId, (int)srcRootId, RV_FALSE,
        vtDeleteFunc, (void *)&destH) < 0)
    {
        RvLogExcep(&vtDest->log,
            (&vtDest->log, "pvtMoveTree(hVal=%p,src=%d,dst=%d): error moving the tree",
                 destH, srcRootId, destRootId));
        RvLockRelease(&vtDest->lock, vtDest->logMgr);
        return RV_ERROR_UNKNOWN;
    }

    /* Make sure the syntax in the destination node is the one used */
    if (destSynH)
        node->synFieldAndNode = destSynFieldAndNode;

    RvLockRelease(&vtDest->lock, vtDest->logMgr);
    return RV_TRUE;
}


/************************************************************************
 * pvtShiftTree
 * purpose: Moves a sub-tree to another location within the same Value Tree,
 *          without changing the value of the source handle.
 * input  : destH       - The handle returned by cmGetValTree() for the
 *                        parent (destination).
 *          destRootId  - The ID of the root node to which the sub-tree
 *                        is moved (destination root).
 *          srcRootId   - The ID of the root node to which the sub-tree
 *                        belongs (source node).
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtShiftTree(
    IN  HPVT        destH,
    IN  RvPvtNodeId destRootId,
    IN  RvPvtNodeId srcRootId)
{
    vtStruct *vtDest = (vtStruct *)destH;
    RvUint32 destSynFieldAndNode;
    HPST     destSynH;

#ifdef RV_DEBUG
    RvBool destIsShielded;
#endif
    vtNode *node;

    if (destH == NULL) 
        return RV_ERROR_UNKNOWN;
    RvLockGet(&vtDest->lock, vtDest->logMgr);

    if (!(node = (vtNode *)rtGetByPath(vtDest->vTree, (int)destRootId)))
    {
        RvLockRelease(&vtDest->lock, vtDest->logMgr);
        return RV_ERROR_UNKNOWN;
    }
    destSynFieldAndNode = node->synFieldAndNode;
    destSynH = node->hSyn;
#ifdef RV_DEBUG
    destIsShielded = node->shieldedNode;
#endif

    if (rtMove(vtDest->vTree, (int)destRootId, (int)srcRootId, RV_TRUE,
        vtDeleteFunc, (void *)&destH) < 0)
    {
        RvLogExcep(&vtDest->log,
            (&vtDest->log, "pvtShiftTree(hVal=%p,src=%d,dst=%d): error moving the tree",
                 destH, srcRootId, destRootId));
        RvLockRelease(&vtDest->lock, vtDest->logMgr);
        return RV_ERROR_UNKNOWN;
    }

    /* Make sure the syntax in the destination node is the one used */
    if (destSynH)
        node->synFieldAndNode = destSynFieldAndNode;
#ifdef RV_DEBUG
    node->shieldedNode = destIsShielded;
#endif

    RvLockRelease(&vtDest->lock, vtDest->logMgr);
    return RV_TRUE;
}



/************************************************************************
 * pvtAdoptChild
 * purpose: Moves the child of a specific tree to below a specific node in
 *          a different tree. This process is referred to as adopting a child
 * input  : hVal            - PVT handle to use
 *          adoptedChildId  - The child you want to move
 *          newParentId     - The node that you want to move the child under.
 *                            This node becomes the root. If this is set to -1,
 *                            then the node becomes a root
 *          newBrotherId    - The node below the new root that you want the new
 *                            child to follow. If -1, then the node is the first
 *                            born.
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtAdoptChild(
    IN  HPVT        hVal,
    IN  RvPvtNodeId adoptedChildId,
    IN  RvPvtNodeId newParentId,
    IN  RvPvtNodeId newBrotherId)
{
    int result;
    vtStruct* vt = (vtStruct *)hVal;
    
    if (hVal == NULL) 
        return RV_ERROR_UNKNOWN;

    RvLockGet(&vt->lock, vt->logMgr);
    result = rtAdoptChild(vt->vTree, (int)adoptedChildId, (int)newParentId, (int)newBrotherId);
    if ((result >= 0) && (newParentId < 0))
    {
        RvLogDebug(&vt->log,
            (&vt->log, "pvtAdoptChild: hVal=%p. Changed to a root=%d", hVal, adoptedChildId));
    }
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}



/************************************************************************
 * pvtGet
 * purpose: Returns the value stored in a node (integer or a string) or an
 *          indication as to the type of the value
 * input  : hVal        - PVT handle to use
 *          nodeId      - The ID of the node to check
 * output : fieldId     - The returned field ID of the node. You can then
 *                        use the pstGetFieldName() function to obtain the field name
 *          synNodeId   - The ID of the node in the Syntax Tree
 *          value       - Value of the root node, or length of the value if
 *                        value is a string
 *          isString    - RV_TRUE if node contains a string - see pvtGetString()
 * return : The node Id value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtGet(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     nodeId,
    OUT RvPstFieldId*   fieldId,
    OUT RvPstNodeId*    synNodeId,
    OUT RvInt32*        value,
    OUT RvBool*         isString)
{
    vtStruct *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_UNKNOWN;
    RvLockGet(&vt->lock, vt->logMgr);
    nodeId = vtGet(vt, nodeId, fieldId, synNodeId, value, isString);
    RvLockRelease(&vt->lock, vt->logMgr);

    return nodeId;
}


/************************************************************************
 * pvtGetString
 * purpose: Returns the value stored in the node, if the value is of string type.
 * input  : hVal            - PVT handle to use
 *          nodeId          - The ID of the node to check
 *          stringLength    - The size of the buffer that will hold the returned value
 * output : string          - The return string value. This is a buffer allocated
 *                            by the user to hold the string
 * return : Actual string's length on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvInt32 RVCALLCONV pvtGetString(
    IN  HPVT        hVal,
    IN  RvPvtNodeId nodeId,
    IN  RvInt32     stringLength,
    OUT char*       string)
{
    RvInt32     result;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_UNKNOWN;

    /* validation not added for improving performance
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_PVT_INVALID_NODEID;
    */

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtGetString(vt, nodeId, stringLength, string);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtGetBitString
 * purpose: Returns the value stored in the node, if the value is of a bit string type.
 * input  : hVal            - PVT handle to use
 *          nodeId          - The ID of the node to check
 *          stringLength    - The size of the buffer that will hold the returned value
 *                            The length is gien in bytes
 * output : string          - The return string value. This is a buffer allocated
 *                            by the user to hold the string
 * return : Actual string's length in bits on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvInt32 RVCALLCONV pvtGetBitString(
    IN  HPVT        hVal,
    IN  RvPvtNodeId nodeId,
    IN  RvInt32     stringLength,
    OUT char*       string)
{
    char        bitsNotInUse = 0;
    int         lengthInBits;
    vtNode      *node;
    vtStruct    *vt = (vtStruct*)hVal;

    if (hVal == NULL) 
        return RV_ERROR_UNKNOWN;

    /* validation not added for improving performance
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_PVT_INVALID_NODEID;
    */

    RvLockGet(&vt->lock, vt->logMgr);

    if (! (node = (vtNode *)rtGetByPath(vt->vTree, (int)nodeId)) )
    {
        RvLockRelease(&vt->lock, vt->logMgr);
        return RV_ERROR_UNKNOWN;
    }

    if (string && stringLength>0) string[0]=0;

    if ((node->string == NULL) && (string != NULL))
    {
        /* no string */
        RvLockRelease(&vt->lock, vt->logMgr);
        return RV_ERROR_UNKNOWN;
    }

    if (string && stringLength>0)
        rpoolCopyToExternal(vt->sPool,&bitsNotInUse ,node->string,
               node->value, 1);

    lengthInBits = node->value*8 - bitsNotInUse;

    if (string && stringLength>0)
    {
        int sizeToCopy = (int)RvMin(node->value, stringLength);
        rpoolCopyToExternal(vt->sPool, string, node->string,0, sizeToCopy);
    }

    RvLockRelease(&vt->lock, vt->logMgr);
    return lengthInBits;
}


/************************************************************************
 * pvtSet
 * purpose: Modifies values in a node
 * input  : hVal    - PVT handle to use
 *          nodeId  - The ID of the node to modify
 *          fieldId - The new field ID of the node.
 *                    A negative value means no change in fieldId.
 *          value   - Value of the root node, or length of the value if
 *                    value is a string
 *          string  - String value of the node, or NULL if not a string
 *                    The string is allocated and stored in the PVT.
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvInt32 RVCALLCONV pvtSet(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     nodeId,
    IN  RvPstFieldId    fieldId,
    IN  RvInt32         value,
    IN  const char*     string)
{
    int         result;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_UNKNOWN;
    
    /* validation not added for improving performance
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_PVT_INVALID_NODEID;
    */

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtSet(vt, nodeId, fieldId, value, string);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtGetChild
 * purpose: Returns the ID of a child node based on its field ID
 * input  : hVal        - PVT handle to use
 *          parentId    - The Node ID of the immediate parent
 *          fieldId     - The field ID of the node that the function is
 *                        expected to locate
 * output : childNodeId - The ID of the node that is found, or negative value on failure
 *                        If set to NULL, it will be discarded
 * return : Child's node id on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtGetChild(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     parentId,
    IN  RvPstFieldId    fieldId,
    OUT RvPvtNodeId*    childNodeId)
{
    RvPvtNodeId resultNodeId;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_UNKNOWN;

    /* validation not added for improving performance
    if (!RV_PVT_NODEID_IS_VALID(parentId))
        return RV_PVT_INVALID_NODEID;
    */

    RvLockGet(&vt->lock, vt->logMgr);
    resultNodeId = vtGetChild(vt, parentId, fieldId, childNodeId);
    RvLockRelease(&vt->lock, vt->logMgr);

    return resultNodeId;
}


/************************************************************************
 * pvtGetChildByFieldId
 * purpose: Returns the child's value based on its field ID
 * input  : hVal        - PVT handle to use
 *          parentId    - The Node ID of the immediate parent
 *          fieldId     - The field ID of the node that the function is
 *                        expected to locate
 * output : value       - Value of the root node, or length of the value if
 *                        value is a string
 *          isString    - RV_TRUE if node contains a string - see pvtGetString()
 * return : Child's node id on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtGetChildByFieldId(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     nodeId,
    IN  RvPstFieldId    fieldId,
    OUT RvInt32*        value,
    OUT RvBool*         isString)
{
    RvPvtNodeId result;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_UNKNOWN;

    /* validation not added for improving performance
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_PVT_INVALID_NODEID;
    */

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtGetChildByFieldId(vt, nodeId, fieldId, value, isString);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtGetByIndex
 * purpose: Returns the ID of a child node based on the child's index
 * input  : hVal        - PVT handle to use
 *          parentId    - The Node ID of the immediate parent
 *          index       - The index of the child, as determined by pvtAdd().
 *                        This index is 1-based.
 * output : childNodeId - The ID of the node that is found, or negative value on failure
 *                        If set to NULL, it will be discarded
 * return : Child's node id on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtGetByIndex(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     parentId,
    IN  RvInt32         index,
    OUT RvPvtNodeId*    childNodeId)
{
    RvPvtNodeId result;
    vtStruct    *vt = (vtStruct *)hVal;
    
    if (hVal == NULL) 
        return RV_ERROR_UNKNOWN;
    if (!RV_PVT_NODEID_IS_VALID(parentId))
        return RV_PVT_INVALID_NODEID;

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtGetByIndex(vt, parentId, index, childNodeId);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtGetSyntaxIndex
 * purpose: Gets the index of the node in the parent syntax structure.
 * input  : hVal    - PVT handle to use
 *          nodeId  - The PVT Node ID
 * output : none
 * return : The index of the specified node in the parent syntax
 *          structure on success (this is 1-based)
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtGetSyntaxIndex(
    IN  HPVT        hVal,
    IN  RvPvtNodeId nodeId)
{
    int             result;
    RvPstNodeId     parentSynNodeId = RV_PST_INVALID_NODEID;
    RvPstFieldId    fieldEnum = -1;
    vtStruct        *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_NULLPTR;
     
    /* validation not added for improving performance
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_PVT_INVALID_NODEID;
    */

    RvLockGet(&vt->lock, vt->logMgr);

    vtGet(vt, vtParent(vt, nodeId), NULL, &parentSynNodeId, NULL, NULL);
    vtGet(vt, nodeId, &fieldEnum, NULL, NULL, NULL);
    result = pstGetField(vtGetSynTree(vt, nodeId), parentSynNodeId, (int)fieldEnum, NULL);
    
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtSearchPath
 * purpose: Searches for a specified path, value or both in a specified tree
 * input  : destH       - PVT handle of the search tree
 *          rootNodeId  - Root ID of the search tree
 *          srcH        - PVT handle of path to be searched in
 *          srcNodeId   - Node ID of the beginning of the path to be searched in
 *          checkLeaves - If RV_TRUE, Compare the values in the lowest level nodes (leaves)
 *                        Use RV_FALSE if the tree syntax contains choices.
 * output : none
 * return : RV_TRUE if found, RV_FALSE if not
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtSearchPath(
    IN  HPVT        destH,
    IN  RvPvtNodeId rootNodeId,
    IN  HPVT        srcH,
    IN  RvPvtNodeId srcNodeId,
    IN  RvBool      checkLeaves)
{
    vtStruct    *vtDest = (vtStruct *)destH;
    vtStruct    *vtSrc  = (vtStruct *)srcH;
    vtStruct    *vt1; 
    vtStruct    *vt2;
    int         result;


    if (destH == NULL || srcH == NULL) 
        return RV_ERROR_NULLPTR;

    /* validation not added for improving performance
    if (!RV_PVT_NODEID_IS_VALID(rootNodeId))
        return RV_PVT_INVALID_NODEID;
    if (!RV_PVT_NODEID_IS_VALID(srcNodeId))
        return RV_PVT_INVALID_NODEID;
    */

    /* Make sure we lock an an orderly fashion to diminish dead-locking possibilities */
    if (destH > srcH)
    {
        vt1 = vtSrc;
        vt2 = vtDest;
    }
    else
    {
        vt1 = vtDest;
        vt2 = vtSrc;
    }

    RvLockGet(&vt1->lock, vt1->logMgr);
    if (vt1 != vt2) RvLockGet(&vt2->lock, vt2->logMgr);
    result = vtSearchPath(vtDest, rootNodeId, vtSrc, srcNodeId, checkLeaves);    
    if (vt1 != vt2) RvLockRelease(&vt2->lock, vt2->logMgr);
    RvLockRelease(&vt1->lock, vt1->logMgr);

    return result;
}



/*---------------------- by path operations ------------------------ */


/************************************************************************
 * pvtGetNodeIdByPath
 * purpose: Returns the ID of a node based on a path that starts from a
 *          specified root
 * input  : hVal                - Value Tree handle
 *          nodeId              - The ID of the node from which the search starts from
 *          path                - The path to the searched node. format: "a.b.c"
 * output : none
 * return : Node ID we reached on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtGetNodeIdByPath(
    IN  HPVT        hVal,
    IN  RvPvtNodeId nodeId,
    IN  const char* path)
{
    vtStruct *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_NULLPTR;
    if (path == NULL) 
        return RV_ERROR_NULLPTR;
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_PVT_INVALID_NODEID;

    RvLockGet(&vt->lock, vt->logMgr);
    nodeId = vtGetNodeIdByPath(vt, nodeId, path);
    RvLockRelease(&vt->lock, vt->logMgr);

    return nodeId;
}


/************************************************************************
 * pvtSetByPath
 * purpose: Modifies the value stored in a node. The function first locates
 *          the node based on a path. The function will fail if "path" doesn't exist
 * input  : hVal    - Value Tree handle
 *          nodeId  - The ID of the node from which the search starts from
 *          path    - The path to the searched node. format: "a.b.c"
 *          value   - Value of the root node, or length of the value if
 *                    value is a string
 *          string  - String value of the node, or NULL if not a string
 *                    The string is allocated and stored in the PVT.
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtSetByPath(
    IN  HPVT        hVal,
    IN  RvPvtNodeId nodeId,
    IN  const char  *path,
    IN  RvInt32     value,
    IN  const char* string)
{
    int         result;
    RvPvtNodeId vNodeId;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_NULLPTR;

    /* validation not added for improving performance
    if (path == NULL) 
        return RV_ERROR_NULLPTR;
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_PVT_INVALID_NODEID;
    */

    RvLockGet(&vt->lock, vt->logMgr);

    if ((vNodeId = vtGetNodeIdByPath(vt, nodeId, path)) >= 0)
        result = vtSet(vt, vNodeId, -1, value, string);
    else
        result = RV_ERROR_UNKNOWN;

    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtBuildByPath
 * purpose: Modifies the value stored in a node. The function first builds
 *          the path to the node, if the path does not exist.
 * input  : hVal    - Value Tree handle
 *          rootId  - The ID of the node from which the search starts from
 *          path    - The path to the searched node. format: "a.b.c"
 *          value   - Value of the root node, or length of the value if
 *                    value is a string
 *          data    - String value of the node, or NULL if not a string
 *                    The string is allocated and stored in the PVT.
 * output : none
 * return : Last built node ID on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtBuildByPath(
    IN  HPVT        hVal,
    IN  RvPvtNodeId rootNodeId,
    IN  const char  *path,
    IN  RvInt32     value,
    IN  const char  *data)
{
    char        name[300];
    char        *nameptr;
    RvPvtNodeId vtNodeId = rootNodeId;
    RvPvtNodeId vtChildId = 0;
    RvPstFieldId fieldId = -800;
    RvPvtNodeId result = (RvPvtNodeId)0; /* Put a valid value - we'll replace it near the end */
    HPST        hSyn;
    int         index;
    vtStruct    *vt = (vtStruct *)hVal;


    if (hVal == NULL) 
        return RV_ERROR_NULLPTR;
    if (path == NULL) 
        return RV_ERROR_NULLPTR;
    if (!RV_PVT_NODEID_IS_VALID(rootNodeId))
        return RV_PVT_INVALID_NODEID;

    RvLockGet(&vt->lock, vt->logMgr);

    hSyn = vtGetSynTree(vt, rootNodeId);

    /* -- generate path */
    do
    {
        nameptr = name;
        while (*path)
        {
            if (*path == '.')
            {
                path++;
                break;
            }
            *nameptr++ = *path++;
        }

        if (nameptr != name)
        {
            *nameptr = '\0';

            /* -- sequence of */
            if (isNumber(name))
            {
                index = atoi(name);
                if (index < 0 || index > vtNumChilds(vt, vtNodeId)+1)
                {
                    result = RV_PVT_ERROR_UNKNOWN;
                    break;
                }

                if (vtGetByIndex(vt, vtNodeId, index, &vtChildId) < 0)
                {
                    /* does not exist */
                    fieldId=-800;
                    if (vtAddSyn(vt, hSyn, vtNodeId, fieldId, -555, NULL, &vtChildId, NULL) <0) /* create node */
                    {
                        result = RV_PVT_ERROR_UNKNOWN;
                        break;
                    }
                }
            }
            else {
                /* -- other */
                if (name[0] == '*'  &&  name[1] == '\0')
                {
                    /* go to child */
                    if ((vtChildId = vtChild(vt, vtNodeId)) <0)
                    {
                        result = RV_PVT_ERROR_UNKNOWN;
                        break;
                    }
                }
                else
                {
                    if ((fieldId = pstGetFieldId(hSyn, name)) < 0)
                    {
                        result = RV_PVT_ERROR_UNKNOWN;
                        break;
                    }
                    if ((vtChildId = vtGetChild(vt, vtNodeId, fieldId, NULL)) <0)
                    {
                        /* does not exist */
                        if (vtAddSyn(vt, hSyn, vtNodeId, fieldId, -555, NULL, &vtChildId, NULL) <0) /* create node */
                        {
                            result = RV_PVT_ERROR_UNKNOWN;
                            break;
                        }
                    }
                }
            }

            vtNodeId = vtChildId;
        }
    } while (*path);

    if (result >= 0)
    {
        if (vtSetSyn(vt, hSyn, vtNodeId, fieldId, value, data) >= 0)
           result = vtNodeId;
        else
           result = RV_PVT_ERROR_UNKNOWN;
    }

    RvLockRelease(&vt->lock, vt->logMgr);
    return result;
}


#if (RV_ASN1_CODER_USE_Q931 == RV_YES)
/************************************************************************
 * pvtGetChildTagByPath
 * purpose: Gets the tag value of the node specified in the path format
 * input  : hVal    - Value Tree handle
 *          nodeId  - The ID of the node
 *          path    - The path of the child node from the node specified in
 *                    the nodeId parameter
 *          relation- The number of levels down the tree from the node specified
 *                    in the nodeId parameter
 * output : none
 * return : The tag value of the specified node on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtGetChildTagByPath(
    IN  HPVT        hVal,
    IN  RvPvtNodeId nodeId,
    IN  const char  *path,
    IN  int         relation)
{
    int             i;
    int             result;
    HPST            hSyn;
    RvPstFieldId    fieldId = -1;
    RvPstNodeId     sNodeId = RV_PST_INVALID_NODEID;
    RvPvtNodeId     vParentNodeId;
    pstChild        child;
    vtStruct        *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_NULLPTR;

    /* validation not added for improving performance
    if (path == NULL) 
        return RV_ERROR_NULLPTR;
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_PVT_INVALID_NODEID;
    */
    
    RvLockGet(&vt->lock, vt->logMgr);

    if ((nodeId = vtGetNodeIdByPath(vt, nodeId, path)) <0)
    {
        RvLockRelease(&vt->lock, vt->logMgr);
        return RV_ERROR_UNKNOWN;
    }

    vParentNodeId = nodeId; /* <- to avoid warning */

    for (i = 0; i < relation; i++)
    {
        vParentNodeId = nodeId;
        if ( (nodeId = vtChild(vt, vParentNodeId)) < 0)
        {
            RvLockRelease(&vt->lock, vt->logMgr);
             return RV_ERROR_UNKNOWN;
        }
    }

    vtGet(vt, vParentNodeId, NULL, &sNodeId, NULL, NULL);
    vtGet(vt, nodeId, &fieldId, NULL, NULL, NULL);

    hSyn = vtGetSynTree(vt, nodeId);

    if (pstGetField(hSyn, sNodeId, fieldId, &child) <0)
    {
        RvLockRelease(&vt->lock, vt->logMgr);
        return RV_ERROR_UNKNOWN;
    }

    result = pstGetTag(hSyn, child.nodeId, NULL);

    RvLockRelease(&vt->lock, vt->logMgr);
    return result;
}
#endif /* (RV_ASN1_CODER_USE_Q931 == RV_YES) */


/************************************************************************
 * pvtGetByPath
 * purpose: This function returns the value in a node of the Value Tree.
 *          As input to the function, you need to provide the starting point
 *          Node ID and the path to the node.
 * input  : hVal    - Value Tree handle
 *          nodeId  - The ID of the node
 *          path    - The path of the child node from the node specified in
 *                    the nodeId parameter
 * output : fieldId     - The returned field ID of the node. You can then
 *                        use the pstGetFieldName() function to obtain the field name
 *          value       - Value of the root node, or length of the value if
 *                        value is a string
 *          isString    - RV_TRUE if node contains a string - see pvtGetString()
 * return : Node ID we've reached on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtGetByPath(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     nodeId,
    IN  const char      *path,
    OUT RvPstFieldId    *fieldId,
    OUT RvInt32         *value,
    OUT RvBool          *isString)
{
    RvPvtNodeId result;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_NULLPTR;

    /* validation not added for improving performance
    if (path == NULL) 
        return RV_ERROR_NULLPTR;
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_PVT_INVALID_NODEID;
    */

    RvLockGet(&vt->lock, vt->logMgr);

    if ((nodeId = vtGetNodeIdByPath(vt, nodeId, path)) >= 0)
        result = vtGet(vt, nodeId, fieldId, NULL, value, isString);
    else
        result = RV_PVT_ERROR_UNKNOWN;

    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/**************************************************************************************
 * pvtGetNodeIdByFieldIds
 *
 * Purpose: find node id by traversing both value and syntax trees according to a path
 *          of filed Ids.
 *              - Field Ids in the path should be existing Field Ids.
 *              - Child indexes should be negative values starting from -101.
 *              - for '*' -1 is used
 *              - to add element to SEQUENCE OF -100 may be used or the last element index + 1
 *              - The list should end with LAST_TOKEN value
 *
 * Input:   hVal       - vtStruct of val tree
 *          nodeId     - The start of the val sub tree.
 *          path       - An array of fieldIds, ending with LAST_TOKEN
 *
 *
 * Return value: The found node id or negative value on failure
 **************************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtGetNodeIdByFieldIds(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     nodeId,
    IN  const RvInt16   *path)
{
    vtStruct *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_NULLPTR;
    if (path == NULL) 
        return RV_ERROR_NULLPTR;
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_PVT_INVALID_NODEID;

    RvLockGet(&vt->lock, vt->logMgr);
    nodeId = vtGetNodeIdByFieldIds(vt, nodeId, path);
    RvLockRelease(&vt->lock, vt->logMgr);

    return nodeId;
}


/**************************************************************************************
 * pvtGetByFieldIds
 *
 * Purpose: Get the data of a node pointed by a given path of field ids
 *
 * Input:   hVal        - vtStruct of val tree
 *          nodeId      - The root of the value sub tree
 *          path        - An array of fieldIds, ending with LAST_TOKEN
 *
 * Output:  fieldId     - field id of the element
 *          value       - The value of the element (or string length if any)
 *          isSstring   - whether value string or not
 *
 * Return value: negative value on failure or node id
 **************************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtGetByFieldIds(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     nodeId,
    IN  RvInt16         *path,
    OUT RvPstFieldId    *fieldId,
    OUT RvInt32         *value,
    OUT RvBool          *isString)
{
    RvPvtNodeId result;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_NULLPTR;
    
    /* validation not added for improving performance
    if (path == NULL) 
        return RV_ERROR_NULLPTR;
    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_PVT_INVALID_NODEID;
    */

    RvLockGet(&vt->lock, vt->logMgr);

    /* Get the node id by the given path */
    if ((nodeId = vtGetNodeIdByFieldIds(vt, nodeId, path)) >= 0)
    {
        /* Get the data of the node by its node id */
        result = vtGet(vt, nodeId, fieldId, NULL, value, isString);
    }
    else
        result = RV_PVT_ERROR_UNKNOWN;

    RvLockRelease(&vt->lock, vt->logMgr);

    return result;
}


/************************************************************************
 * pvtBuildByFieldIds
 * purpose: Modifies the value stored in a node. The function first builds
 *          the path to the node, if the path does not exist.
 *          This function works faster than pvtBuildByPath
 * input  : hVal        - Value Tree handle
 *          rootNodeId  - The ID of the node from which the search starts from
 *          path        - The path to the searched node. format: {a,b,c,LAST_TOKEN}
 *          value       - Value of the root node, or length of the value if
 *                        value is a string
 *          string      - String value of the node, or NULL if not a string
 *                        The string is allocated and stored in the PVT.
 * output : none
 * return : Last built node ID on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtBuildByFieldIds(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     rootNodeId,
    IN  RvInt16         *path,
    IN  RvInt32         value,
    IN  const RvChar    *data)
{
    int             index;
    HPST            hSyn;
    RvPstFieldId    token;
    RvPstFieldId    fieldId = -800;
    RvPvtNodeId     vtChildId = 0;
    RvPvtNodeId     vtNodeId = rootNodeId;
    vtStruct        *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_NULLPTR;
    if (path == NULL) 
        return RV_ERROR_NULLPTR;
    if (!RV_PVT_NODEID_IS_VALID(rootNodeId))
        return RV_PVT_INVALID_NODEID;

    RvLockGet(&vt->lock, vt->logMgr);
    hSyn = vtGetSynTree(vt, rootNodeId);

    /* Go over each field id in the path */
    for (token = 0; path[token] != LAST_TOKEN; token++)
    {
        /* Sanity check */
        if (token >= MAX_TOKEN)
        {
            vtNodeId = RV_PVT_ERROR_UNKNOWN;
            break;
        }

        /* if the number is negative less than 100, that is an element position in a SEQUENCE OF field */
        if (path[token] > -100)
        {
            /* Not a SEQUENCE OF field */

            /* if first child is requested (the __anyField is equivalent to * sign) get its node id */
            if (path[token] == __anyField)
            {
                fieldId = -800;
                if ((vtChildId = vtChild(vt,vtNodeId)) < 0)
                {
                    vtNodeId = RV_PVT_ERROR_UNKNOWN;
                    break;
                }
            }
            else
            {
                /* a specific field Id was given */
                fieldId = path[token];

                /* Get the given field's node id within the val tree */
                if ((vtChildId = vtGetChild(vt, vtNodeId, fieldId, NULL)) < 0)
                {
                    /* field doesn't exists, create it */
                    if (vtAddSyn(vt, hSyn, vtNodeId, fieldId, -555, NULL, &vtChildId, NULL) <0)
                    {
                        vtNodeId = RV_PVT_ERROR_UNKNOWN;
                        break;
                    }
                }
            }
        }
        else
        {
            /* SEQUENCE OF field */

            /* translate it to index (-101 -> 1; -102 -> 2 etc.) */
            index = -(path[token] + 100);
            if ((index < 0) || (index > vtNumChilds(vt, vtNodeId) + 1))
            {
                vtNodeId = RV_PVT_ERROR_UNKNOWN;
                break;
            }

            /* Get the given element's node id within the val tree */
            if ((vtChildId = vtGetByIndex(vt, vtNodeId, index,NULL)) < 0)
            {
                /* element doesn't exists, create it */
                fieldId = -800;
                if (vtAddSyn(vt, hSyn, vtNodeId, fieldId, -555, NULL, &vtChildId, NULL) < 0)
                {
                    vtNodeId = RV_PVT_ERROR_UNKNOWN;
                    break;
                }
            }
        }

        /* we have the node id (new or old) */
        vtNodeId = vtChildId;
    }

    if (vtNodeId >= 0)
    {
        /* set the given values to the node */
        if (vtSetSyn(vt, hSyn, vtNodeId, fieldId, value, data) < 0)
            vtNodeId = RV_PVT_ERROR_UNKNOWN;
    }

    RvLockRelease(&vt->lock, vt->logMgr);

    return vtNodeId;
}





/************************************************************************
 * pvtPrint
 * purpose: Print a tree from a given node, using a specified printing
 *          function
 * input  : hVal        - Value Tree handle
 *          parentId    - node ID to start printing from
 *          pFunc       - The function called for the printing itself
 *          pFuncParam  - The context to pass to the print function
 *                        on each printed line
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtPrint(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     parentId,
    IN  pvtPrintFuncP   pFunc,
    IN  void            *pFuncParam)
{
    int         result;
    vtStruct    *vt = (vtStruct*)hVal;

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtPrintFunc(vt, parentId, pFunc, pFuncParam, pvtPrintDegreeAll);
    RvLockRelease(&vt->lock, vt->logMgr);
    
    return result;
}


/************************************************************************
 * pvtPrintInternal
 * purpose: Print a tree from a given node.
 *          This function should be used internally instead of using
 *          pvtPrintStd().
 * input  : hVal        - Value Tree handle
 *          parentId    - node ID to start printing from
 *          logHandle   - Log source to print to
 *          level       - Debug level to use for printing
 *                        0,1 - None
 *                        2 - Print the tree
 *                        3 - Print all values in the tree (even the irrelevant onse)
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtPrintInternal(
    IN  HPVT            hVal,
    IN  RvPvtNodeId     parentId,
    IN  RvLogSource     *logHandle,
    IN  int             level)
{
#if (RV_LOGMASK & RV_LOGLEVEL_DEBUG)
    pvtPrintDegree  degree;
    int             result;
    vtStruct        *vt = (vtStruct*)hVal;

    /* See if we can print a message to this log or not */
    if ((level < 2) || !RvLogIsSelected(logHandle, RV_LOGLEVEL_DEBUG))
        return 0;

    if (level >= 3)
        degree = pvtPrintDegreeAll;
    else
        degree = (pvtPrintDegree)(pvtPrintDegreeSyntax | pvtPrintDegreeNodeId);

    RvLockGet(&vt->lock, vt->logMgr);
    result = vtPrintFunc(vt, parentId, vtPrintToLog, logHandle, degree);
    RvLockRelease(&vt->lock, vt->logMgr);

    return result;

#else
    RV_UNUSED_ARG(hVal);
    RV_UNUSED_ARG(parentId);
    RV_UNUSED_ARG(logHandle);
    RV_UNUSED_ARG(level);
    return RV_OK;
#endif
}


/************************************************************************
 * pvtPrintStd
 * purpose: Print a tree from a given node
 *
 * NOTE   : This function call a different API function (pvtPrintInternal())
 *
 * input  : hVal        - Value Tree handle
 *          parentId    - node ID to start printing from
 *          logHandle   - Log source to print to
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtPrintStd(
    IN  HPVT        hVal,
    IN  RvPvtNodeId parentId,
    IN  int         logHandle)
{
    RvLogSource *logSource;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return RV_ERROR_NULLPTR;


    if (logHandle == -1) {
        /* No specific log - use UNREG... */
        logSource = &vt->unregLog;
    }
    else {
        /* todo: FIX logSource from the handle!!! */
        logSource = NULL;
    }

    return pvtPrintInternal(hVal, parentId, logSource, 3/* --> PvtPrintAll*/);
}



#if (RV_LOGMASK & RV_LOGLEVEL_DEBUG)

/************************************************************************
 * pvtPrintRootNodes
 * purpose: Prints all the root nodes currently allocated inside the
 *          PVT. It is only here for debugging purposes and should not
 *          be used in any other type of implementation.
 * input  : hVal            - Value Tree handle
 *          printOnlyRoot   - If RV_TRUE, only the root nodes are printed
 *                            If RV_FALSE, the whole tree nodes are printed
 *          msa             - Log source to print to
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI void RVCALLCONV pvtPrintRootNodes(
    IN HPVT     hVal, 
    IN RvBool   printOnlyRoot, 
    IN void     *msa)
{
    int         i;
    vtStruct    *vt = (vtStruct *)hVal;

    if (hVal == NULL) 
        return;

    if (!RvLogIsSelected(msa, RV_LOGLEVEL_DEBUG)) 
        return;

    RvLockGet(&vt->lock, vt->logMgr);
    vt->pFunc = vtPrintToLog;
    vt->pFuncParam = msa;
    vt->printOnlyRoots = printOnlyRoot;
    vt->degree = pvtPrintDegreeAll;

    RvLogTextDebug((RvLogSource *)msa, "PVT Nodes (%p):", hVal);
    rtDoAll(vt->vTree, vtRootPrint, vt);

    if (!printOnlyRoot)
    {
        RvLogTextDebug((RvLogSource *)msa, "PVT Elements (%p):", hVal);
        for (i = 0; i < raMaxSize((HRA)vt->vTree); i++)
            if (!raElemIsVacant((HRA)vt->vTree, i))
            {
                RvLogTextDebug((RvLogSource *)msa, "Element %d is used", i);
            }
    }

    RvLockRelease(&vt->lock, vt->logMgr);
}

#endif  /* (RV_LOGMASK & RV_LOGLEVEL_DEBUG) */



#ifdef __cplusplus
}
#endif



