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
  pvaltreeStackApi

  This file contains functions which are available for the use of the stack modules,
  but are not part of the API provided to the users

  */

#include "rvstdio.h"
#include "rvlock.h"
#include "pvaltreeDef.h"
#include "pvaltreeStackApi.h"

#include "intutils.h"
#include "oidutils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* used for RV_ERROR_UNKNOWN reporting */
#define getSafeFieldName(_pNode) pstGetFieldNamePtr((_pNode)->hSyn, (RvInt32)VTN_SYN_FIELD(_pNode))

/************************************************************************
 *
 *                      Static Functions
 *
 ************************************************************************/
static RvBool vtIsStringNodeType(IN pstNodeType type)
{
    switch (type)
    {
        case pstObjectIdentifier:
        case pstOctetString:
        case pstBitString:
        case pstGeneralString:
        case pstUniversalString:
        case pstBMPString:
        case pstIA5String:
        case pstVisibleString:
        case pstNumericString:
        case pstPrintableString:  return RV_TRUE;
        default:                  break;
    }

    return RV_FALSE;
}


static RvBool vtCompareTwo(
     /* Compare two vt nodes */
     RTElement element1,
     RTElement element2,
     void *param /* syntax handle */
     )
{
    vtNode      *node1 = (vtNode*)element1;
    vtNode      *node2 = (vtNode*)element2;
    vtStruct    *vt     = (vtStruct *)param; /*((vtStruct**)param)[0];*/
    HPST        hSyn    = node1->hSyn; /*((HPST*)param)[1];*/
    RvPstNodeId synField;

    /* -- structure */
    synField = VTN_SYN_NODE(node1);
    if (VTN_SYN_FIELD(node1) != VTN_SYN_FIELD(node2))
        return RV_FALSE;
    if (synField != VTN_SYN_NODE(node2))
        return RV_FALSE;
    switch (pstGetNodeType(hSyn, synField))
    {
        case pstNull:
        case pstEnumeration:
        case pstChoice:
        case pstSequence:
        case pstSet:
        case pstOf:
        case pstSequenceOf:
        case pstSetOf:
            /* We don't care about these type's values - they contribute nothing at all */
            return RV_TRUE;

        default:
            /* All other types are just fine... */
            break;
    }

    /* -- value */
    if (node1->value != node2->value)
        return RV_FALSE;
    if ( (node1->string && !node2->string) || (!node1->string && node2->string))
        return RV_FALSE;
    if (node1->string &&
        rpoolCompareInternal(vt->sPool,node1->string, node2->string, node1->value))
        return RV_FALSE;

    return RV_TRUE;
}


/************************************************************************
 *
 *                  Non locking PVT functions
 *
 ************************************************************************/

/************************************************************************
 * pvtCompareContext
 * Context to use for pvtCompareFunction.
 * This contexts includes the following information:
 * hVal     - Value tree to use
 * nodeId   - Value node id to compare
 *            This node will be compare with a syntax value node
 ************************************************************************/
typedef struct
{
    HPVT        hVal;
    RvPvtNodeId nodeId;
} pvtCompareContext;


#if (RV_ASN1_CODER_USE_H450 == RV_YES)
static int vtFindObject(
    IN  vtStruct       *vt,
    IN  RvPvtNodeId    nodeId,
    IN  HPST           hSyn,
    IN  int            stNodeIdOfAT,
    OUT int*           objectId)
{
    pvtCompareContext key;

    key.hVal = (HPVT)vt;
    key.nodeId = nodeId;
    
    if (objectId)
        *objectId = pstFindObjectInAT(hSyn, stNodeIdOfAT, vtCompare, &key);
    else
        *objectId = RV_ERROR_BADPARAM;

    return (*objectId);
}
#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */


/************************************************************************
 *                      Print functions
 ************************************************************************/

/************************************************************************
 * vtPrintToLog
 * purpose: Callback function called when a node has been parsed into
 *          a string and should be sent to the log.
 * input  : type    - Type of log handle to use
 *          line    - Line information
 *                    The additional parameters are discarded
 * output : none
 * return : none
 ************************************************************************/
#if (RV_LOGMASK & RV_LOGLEVEL_DEBUG)
void vtPrintToLog(void * type, const char* line, ...)
{
    RvLogTextDebug((RvLogSource*)type, "%s", line);
}
#endif


/************************************************************************
 * vtPrintFunc
 * purpose: Print a tree from a given node.
 *          This function is the function used by all of the public
 *          print functions.
 * input  : hVal        - Value Tree handle
 *          parentId    - node ID to start printing from
 *          pFunc       - The function called for the printing itself
 *          pFuncParam  - The context to pass to the print function
 *                        on each printed line
 *          degree      - Detail degree to use when printing the message
 *                        tree
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int vtPrintFunc(
    IN  vtStruct        *vt,
    IN  RvPvtNodeId     parentId,
    IN  pvtPrintFuncP   pFunc,
    IN  void *          pFuncParam,
    IN  pvtPrintDegree  degree)
{
    /* Set printing parameters before we start printing */
    vt->pFunc       = pFunc;
    vt->pFuncParam  = pFuncParam;
    vt->degree      = degree;

    /* Print the message */
    return rtPrint(vt->vTree, (int)parentId, 0, -1, (void *)vt, pFuncParam);
}

/************************************************************************
 * vtNodePrint
 * purpose: Callback function used to print a single node using vt's
 *          print parameters
 * input  : rtH     - RTree handle used
 *          nodeId  - Node to print
 *          layer   - Level of node in tree
 *          param   - vtStruct of the node
 * output : none
 * return : vtStruct of the node (i.e. - param)
 ************************************************************************/
void *vtNodePrint(
    HRTREE rtH, 
    int nodeId, 
    int layer, 
    void *param)
{
    /* This function has been modified to deal with buffer overruns */
    const RvChar*   hexCh = "0123456789abcdef";
    const RvChar*   levels = ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ";
    vtNode*         n1 = (vtNode *)rtGetByPath(rtH, (int)nodeId);
    vtStruct*       vt = (vtStruct *)param;
    RvChar*         name = NULL;
    RvChar          string[1024];
    RvChar*         ptr;
    RvChar*         endString;
    HPST            hSyn;
    RvChar          buff[128];
    RvPstNodeId     synNodeId;
    int             tmpLen, value;
    pstNodeType     type;

    /* Make sure we have something to print */
    if ((n1 == NULL) || (param == NULL)) return param;
    ptr = string;
    hSyn = vtGetSynTree(vt, (RvPvtNodeId)nodeId);
    endString = string + sizeof(string);

    /* 0. Layer */
    /* Here, we know for sure we have enough space in the string, so we don't check for overruns */
    ptr += RvSprintf(ptr, "%2d> ", layer);
    if (layer > 32)
    {
        strcpy(ptr, levels);
        ptr += 64;
    }
    else
    {
        memcpy(ptr, levels, (RvSize_t)(layer * 2));
        ptr += (layer * 2);
    }

    /* 1. NODE ID */
    if ((int)(vt->degree & pvtPrintDegreeNodeId) != 0)
    {
        ptr += RvSnprintf(ptr, (RvSize_t)(endString - ptr), "<%d> ", nodeId);
    }


    /* 2. FIELD NAME */
    if (hSyn != NULL)
    {
        RvPstNodeId sNodeId = RV_PST_INVALID_NODEID;
        pstNodeType parentType;

        /* Get the parent's node type */
        if (vtGet(vt, vtParent(vt, (RvPvtNodeId)nodeId), NULL, &sNodeId, NULL, NULL) >= 0)
            parentType = pstGetNodeType(hSyn, sNodeId);
        else
            parentType = pstNull;
        if (parentType == pstSequenceOf || parentType == pstSetOf)
        {
            /* SEQUENCE OF - no actual field name */
            name = (RvChar*)"*";
        }
        else
        {
            if (layer == 0)
            {
                /* First node - get its type */
                synNodeId = RV_PST_INVALID_NODEID;
                vtGet(vt, (RvPvtNodeId)nodeId, NULL, &synNodeId, NULL, NULL);
                name = stGetNameByNodeId(hSyn, (int)synNodeId);
            }
            else
            {
                /* Not first node - get the field's name */
                int synField = VTN_SYN_FIELD(n1);
                name = pstGetFieldNamePtr(hSyn, synField);
            }
        }
    }
    if (name != NULL)
    {
        /* Add the name of field */
        ptr += RvSnprintf(ptr, (RvSize_t)(endString - ptr), "%s ", name);
    }
    else
    {
        /* Didn't find the name - print the value of the node */
        ptr += RvSnprintf(ptr, (RvSize_t)(endString - ptr), "$fid=%d$ ", VTN_SYN_FIELD(n1));
        name = buff;
    }


    /* 3. VALUE */

    /* Check the type of the node we're about to print */
    synNodeId = RV_PST_INVALID_NODEID;
    vtGet(vt, (RvPvtNodeId)nodeId, NULL, &synNodeId, NULL, NULL);
    type = pstGetNodeType(hSyn, synNodeId);

    if (type != pstBitString)
    {
        value = n1->value;
        tmpLen = RvMin(n1->value, (int)sizeof(buff));
    }
    else
    {
        RvUint8 bitsNotInUse;

        /* Calculate the length of the bit-string in bits */
        rpoolCopyToExternal(vt->sPool, &bitsNotInUse, n1->string, n1->value, 1);
        value = (n1->value) * 8 - bitsNotInUse;
        tmpLen = RvMin(n1->value, (int)sizeof(buff));
    }

    if ((int)(vt->degree & pvtPrintDegreeValue) != 0)
    {
        /* We always print the value */
        ptr += RvSnprintf(ptr, (RvSize_t)(endString - ptr), "= (%u) ", value);
    }
    else
    {
        /* Remove values from non leaf nodes */
        if ((vtChild(vt, (RvPvtNodeId)nodeId) < 0) || (n1->string != NULL))
        {
            /* leaf */
            ptr += RvSnprintf(ptr, (RvSize_t)(endString - ptr), "= %d ", value);
        }
    }

    /* Get the string's value if we have a string */
    if (n1->string != NULL)
    {
        /* -- show strings */
        if (type == pstObjectIdentifier)
        {
            if (endString - ptr > 3)
            {
                int resLen;

                /* We have to decode the object identifier and only then add it in */
                rpoolCopyToExternal(vt->sPool, buff, n1->string, 0, tmpLen);

                ptr[0] = '{';
                resLen = oidDecodeOID((int)tmpLen, buff, endString - ptr - 2, ptr + 1, nameForm);
                if (resLen < 0) resLen = 0;
                ptr[resLen] = '}';
                ptr += (resLen + 1);
            }
        }
        else
        {
            /* Other string types - just print them */
            int      i, leftLen;
            RvChar*  hexPtr;
            RvUint8  ip[4] = {0, 0, 0, 0};

            leftLen = endString - (ptr + 2);

            /* Put the string in place */
            rpoolCopyToExternal(vt->sPool, ptr + 1, n1->string, 0, RvMin(tmpLen, leftLen));
            if (tmpLen == 4)
            {
                /* We might need the original string later on for IP address */
                memcpy(ip, ptr + 1, sizeof(ip));
            }
            *ptr = '\''; ptr++;
            ptr[tmpLen] = '\'';

            if (leftLen - tmpLen > 4)
            {
                /* Fix unprintable characters and print the hex string */
                strcpy(ptr + tmpLen + 1, " =0x");
                hexPtr = ptr + tmpLen + 5;
                leftLen -= (tmpLen + 4);
                for (i = 0; i < tmpLen; i++)
                {
                    if (leftLen > 2)
                    {
                        hexPtr[0] = hexCh[((RvUint8)*ptr) >> 4];
                        hexPtr[1] = hexCh[((RvUint8)*ptr) & 0xf];
                        hexPtr += 2;
                    }
                    if ((RvUint8)*ptr < ' ') *ptr = '.';
                    ptr++;
                }

                ptr = hexPtr;

                /* Deal with IP address */
                if ((type == pstOctetString) && (n1->value == 4))
                {
                    /* might be an IP address */
                    ptr += RvSnprintf(ptr, (RvSize_t)(endString - ptr), " <%u.%u.%u.%u> ", ip[0], ip[1], ip[2], ip[3]);
                }
                else
                {
                    if (ptr < endString)
                    {
                        *ptr = ' ';
                        ptr++;
                    }
                }
            }
        }
    }

#if (RV_ASN1_CODER_USE_H450 == RV_YES)
    /* 4. DEPENDENCY */
    if (endString > ptr + 3)
    {
        ptr[0] = '.';
        ptr[1] = ' ';
        ptr[2] = ' ';
        ptr += 3;

        if (type != pstNull)
        {
            pstChildExt child;
            vtNode* n2 = (vtNode *)rtGetByPath(rtH, rtParent(rtH, (int)nodeId));

            /* Check if we have a child... */
            if (hSyn && (n2 != NULL))
            {
                RvPstNodeId synField2 = VTN_SYN_NODE(n2);
                int synField1 = VTN_SYN_FIELD(n1);
                if (pstGetFieldExt(hSyn, synField2, synField1, &child) >= 0)
                    if (child.speciality == pstDepending)
                    {
                        int objectId;
                        if (vtFindObject(vt, (RvPvtNodeId)nodeId, hSyn, child.enumerationValue, &objectId) >= 0)
                            ptr += RvSnprintf(ptr, (RvSize_t)(endString - ptr), "[%s] ", stGetNameByNodeId(hSyn, objectId));
                    }
            }
        }
    }
#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */

    /* 5. GENERAL TYPE */
    if (((int)(vt->degree & pvtPrintDegreeSyntax) != 0) && (hSyn != NULL))
    {
        RvPstNodeId synField = VTN_SYN_NODE(n1);
        if ((int)(vt->degree & pvtPrintDegreeSyntaxId) != 0)
            ptr += RvSnprintf(ptr, (RvSize_t)(endString - ptr), "<%d> ", synField);

        ptr += pstPrintNode(hSyn, synField, ptr, endString - ptr);
    }

    if (endString < ptr + 2)
        ptr = endString - 2;

    /* Print the line we parsed */
    ptr[0] = '\n';
    ptr[1] = '\0';
    vt->pFunc(vt->pFuncParam, string);

    return param;
}


#if (RV_LOGMASK & RV_LOGLEVEL_DEBUG)
/************************************************************************
 * vtRootPrint
 * purpose: Callback function used to print a single root for debugging
 *          purposes only
 * input  : rtH     - RTree handle used
 *          element - Node to print
 *          param   - vtStruct of the node
 * output : none
 * return : vtStruct of the node (i.e. - param)
 ************************************************************************/
void* vtRootPrint(IN HRTREE rtH, IN RTElement element, IN void* param)
{
    vtStruct*   vt = (vtStruct *)param;
    int         nodeId = rtGetNode(rtH, element);

    if (vt->printOnlyRoots)
        return vtNodePrint(rtH, nodeId, 0, param);
    else
    {
        rtPrint(vt->vTree, nodeId, 0, -1, param, vt->pFuncParam);
        return param;
    }
}
#endif


/************************************************************************
 * vtGetRoot
 * purpose: Returns the Node ID of the root of a Value Tree to which a
 *          specified node belongs
 * input  : vt      - Pointer to VT struct
 *          nodeId  - The ID of the node inside a tree
 * output : none
 * return : Root node ID of the given node ID on success
 *          Negative value on failure
 ************************************************************************/
RvPvtNodeId vtGetRoot(
    IN  vtStruct    *vt,
    IN  RvPvtNodeId  nodeId)
{
    return (RvPvtNodeId)rtGetRoot(vt->vTree, (int)nodeId);
}


/************************************************************************
 * vtGetSynTree
 * purpose: Returns a handle to the structure (Syntax Tree) definition of
 *          a specified node
 * input  : vt      - Pointer to VT struct
 *          nodeId  - The ID of the node the syntax of which is returned
 * output : none
 * return : A handle to the structure (Syntax Tree) definition on success
 *          NULL on failure
 ************************************************************************/
HPST vtGetSynTree(
    IN  vtStruct    *vt,
    IN  RvPvtNodeId nodeId)
{

    vtNode *node = (vtNode*)rtGetByPath(vt->vTree, (int)nodeId);
    if (node == NULL)
        return NULL;
    return node->hSyn;
}


/************************************************************************
 * vtTreeSize
 * purpose: Returns the number of nodes included in a Value Tree root
 * input  : hVal        - Pointer to VT struct
 *          parentId    - The ID of any node. The function returns the
 *                        number of nodes located under the specified parent.
 * output : none
 * return : The number of nodes included in a Value Tree on success
 *          Negative value on failure
 ************************************************************************/
int vtTreeSize(
    IN  vtStruct    *vt,
    IN  RvPvtNodeId parentId)
{
    return rtTreeSize(vt->vTree, (int)parentId);
}


/************************************************************************
 * vtNumChilds
 * purpose: Returns the number of the dependents (children) of a parent tree
 * input  : vt          - Pointer to VT struct
 *          parentId    - The ID of any node
 * output : none
 * return : The number of immediate nodes under the given node on success
 *          Negative value on failure
 ************************************************************************/
int vtNumChilds(
    IN  vtStruct    *vt,
    IN  RvPvtNodeId parentId)
{
    return rtNumChilds(vt->vTree, (int)parentId);
}


/************************************************************************
 * vtParent
 * purpose: Returns the ID of the parent node of a specified node
 * input  : vt          - Pointer to VT struct
 *          nodeId      - The ID of any node
 * output : none
 * return : The parent ID of the given node on success
 *          Negative value on failure
 ************************************************************************/
RvPvtNodeId vtParent(
    IN  vtStruct    *vt,
    IN  RvPvtNodeId   nodeId)
{
    return (RvPvtNodeId)rtParent(vt->vTree, (int)nodeId);
}


/************************************************************************
 * vtBrother
 * purpose: Returns the Node ID of a specified node's brother (right).
 * input  : vt          - Pointer to VT struct
 *          nodeId      - The ID of any node
 * output : none
 * return : The node ID of the given node's brother on success
 *          Negative value on failure
 * The function returns the next child (rightward). Use pvtChild() to get
 * the first dependent, and then use pvtBrother() to get to the next brother.
 ************************************************************************/
RvPvtNodeId vtBrother(
    IN  vtStruct    *vt,
    IN  RvPvtNodeId  nodeId)
{
    return (RvPvtNodeId)rtBrother(vt->vTree, (int)nodeId);
}


/************************************************************************
 * vtChild
 * purpose: Returns the Node ID of the first (leftmost) child of a parent.
 * input  : vt          - Pointer to VT struct
 *          parentId    - The ID of any node
 * output : none
 * return : The Node ID of the first (leftmost) child of a parent on success
 *          Negative value on failure
 ************************************************************************/
RvPvtNodeId vtChild(
    IN  vtStruct    *vt,
    IN  RvPvtNodeId  parentId)
{
    return (RvPvtNodeId)rtHead(vt->vTree, (int)parentId);
}


/************************************************************************
 * vtNext
 * purpose: Returns the ID of a node located after a specified node.
 * input  : vt      - Pointer to VT struct
 *          rootId  - The ID of the root node of the current Value Tree
 *          location- A Node ID inside the given root
 * output : none
 * return : The Node ID of the next node in the tree on success - this
 *          value should be given as the location on the next call.
 *          Negative value on failure
 ************************************************************************/
RvPvtNodeId vtNext(
    IN  vtStruct    *vt,
    IN  RvPvtNodeId rootId,
    IN  RvPvtNodeId location)
{
    return (RvPvtNodeId)rtNext(vt->vTree, (int)rootId, (int)location);
}


/************************************************************************
 * vtDelete
 * purpose: Deletes a sub-tree
 * input  : vt              - Pointer to VT struct
 *          subTreeRootId   - The ID of the root node to delete
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int vtDelete(
    IN  vtStruct    *vt,
    IN  RvPvtNodeId subTreeRootId)
{
    return rtDelete(vt->vTree, (int)subTreeRootId, vtDeleteFunc, (void *)&vt);
}


/************************************************************************
 * vtDeleteFunc
 * purpose: Used as callback delete function
 * input  : rtH             - Handle of rtree object that contains element
 *          element         - Pointer to node that contains rpool element
 *          ppvt            - Points to VT struct
 * output : none
 * return : ppvt on success
 *          NULL upon failure
 ************************************************************************/
void *vtDeleteFunc(HRTREE rtH, RTElement element, void *ppvt)
{
    vtNode      *node = (vtNode *)element;
    vtStruct    *vt   = *(vtStruct **)ppvt;

    RV_UNUSED_ARG(rtH);
    if (ppvt == NULL) return NULL;

    if (node && node->string)
    {
        if (rpoolFree(vt->sPool, node->string) < 0)
        {
            RvLogError(&vt->log,
                (&vt->log, "vtDeleteFunc:%d: Failed to free string from pool [%p].",
                     rtGetNode(rtH, element), node->string));
        }
    }

    return ppvt;
}


/************************************************************************
 * vtDeleteChilds
 * purpose: Deletes the children of root node.
 * input  : vt              - Pointer to VT struct
 *          subTreeRootId   - The ID of the root node to delete
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int vtDeleteChilds(
    IN  vtStruct    *vt,
    IN  RvPvtNodeId subTreeRootId)
{
    return rtDeleteChilds(vt->vTree, (int)subTreeRootId, vtDeleteFunc, (void *)&vt);
}


/************************************************************************
 * vtGet
 * purpose: Returns the value stored in a node (integer or a string) or an
 *          indication as to the type of the value
 * input  : vt          - Pointer to VT struct
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
RvPvtNodeId vtGet(
    IN  vtStruct        *vt,
    IN  RvPvtNodeId     nodeId,
    OUT RvPstFieldId*   fieldId,
    OUT RvPstNodeId*    synNodeId,
    OUT RvInt32*        value,
    OUT RvBool*         isString)
{
    vtNode *node;
    RvPstNodeId synField;

    node = (vtNode *)rtGetByPath(vt->vTree, (int)nodeId);
    if (node == NULL) {
        return RV_PVT_ERROR_UNKNOWN;
    }

    if (fieldId)    *fieldId = VTN_SYN_FIELD(node);
    if (synNodeId)  *synNodeId = VTN_SYN_NODE(node);
    if (value)      *value = node->value;

    if (isString) {
        synField = VTN_SYN_NODE(node);
        *isString = (node->string != NULL || pstIsStringNode(node->hSyn, synField)) ? 
                        RV_TRUE : RV_FALSE;
    }
    
    return nodeId;
}


/************************************************************************
 * vtGetString
 * purpose: Returns the value stored in the node, if the value is of string type.
 * input  : vt              - Pointer to VT struct
 *          nodeId          - The ID of the node to check
 *          stringLength    - The size of the buffer that will hold the returned value
 * output : string          - The return string value. This is a buffer allocated
 *                            by the user to hold the string
 * return : Actual string's length on success
 *          Negative value on failure
 ************************************************************************/
RvInt32 vtGetString(
    IN  vtStruct    *vt,
    IN  RvPvtNodeId nodeId,
    IN  RvInt32     stringLength,
    OUT char*       string)
{
    vtNode *node;
    RvInt32 result;

    node = (vtNode *)rtGetByPath(vt->vTree, (int)nodeId);
    if (node == NULL) {
        return RV_PVT_ERROR_UNKNOWN;
    }

    if (node->string == NULL) {
        return RV_ERROR_UNKNOWN;
    }

    if ((string != NULL) && (stringLength > 0)) {
        string[0] = 0;
    }

    /* Get the length of this string */
    result = node->value;

    if ((string != NULL) && (stringLength > 0))
    {
        int sizeToCopy = (int)RvMin(result, stringLength);
        rpoolCopyToExternal(vt->sPool, string, node->string,0, sizeToCopy);

        /* Make sure we NULL terminate this string */
        if (stringLength > result)
            string[result] = 0;
    }

    return result;
}


/************************************************************************
 * vtSet
 * purpose: Modifies values in a node
 * input  : vt      - Pointer to VT struct
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
RvInt32 vtSet(
    IN  vtStruct        *vt,
    IN  RvPvtNodeId     nodeId,
    IN  RvPstFieldId    fieldId,
    IN  RvInt32         value,
    IN  const char*     string)
{

    return vtSetSyn(vt, (HPST)-1, nodeId, fieldId, value, string);
}


/************************************************************************
 * vtGetChild
 * purpose: Returns the ID of a child node based on its field ID
 * input  : vt          - Pointer to VT struct
 *          parentId    - The Node ID of the immediate parent
 *          fieldId     - The field ID of the node that the function is
 *                        expected to locate
 * output : childNodeId - The ID of the node that is found, or negative value on failure
 *                        If set to NULL, it will be discarded
 * return : Child's node id on success
 *          Negative value on failure
 ************************************************************************/
RvPvtNodeId vtGetChild(
    IN  vtStruct        *vt,
    IN  RvPvtNodeId     parentId,
    IN  RvPstFieldId    fieldId,
    OUT RvPvtNodeId*    childNodeId)
{
    int         path;
    RvPvtNodeId resultNodeId;

    path = rtGetChild(vt->vTree, (int)parentId, (void *)fieldId, 1);
    if (path < 0)
        resultNodeId = RV_PVT_ERROR_UNKNOWN;
    else
        resultNodeId = (RvPvtNodeId)path;

    if (childNodeId)
        *childNodeId = resultNodeId;

    return resultNodeId;
}


/************************************************************************
 * vtGetChildByFieldId
 * purpose: Returns the child's value based on its field ID
 * input  : vt          - Pointer to VT struct
 *          parentId    - The Node ID of the immediate parent
 *          fieldId     - The field ID of the node that the function is
 *                        expected to locate
 * output : value       - Value of the root node, or length of the value if
 *                        value is a string
 *          isString    - RV_TRUE if node contains a string - see pvtGetString()
 * return : Child's node id on success
 *          Negative value on failure
 ************************************************************************/
RvPvtNodeId vtGetChildByFieldId(
    IN  vtStruct        *vt,
    IN  RvPvtNodeId     nodeId,
    IN  RvPstFieldId    fieldId,
    OUT RvInt32*        value,
    OUT RvBool*         isString)
{
    RvPvtNodeId result;
    vtNode* node;

    result = (RvPvtNodeId)rtGetChild(vt->vTree, (int)nodeId, (void *)fieldId, 1);
    if (RV_PVT_NODEID_IS_VALID(result))
    {
        node = (vtNode *)rtGetByPath(vt->vTree, (int)result);
        if (node != NULL)
        {
            if (value) {
                *value = node->value;
            }
            if (isString)
            {
                RvPstNodeId synField = VTN_SYN_NODE(node);
                *isString = (node->string != NULL ||
                    pstIsStringNode(vtGetSynTree(vt, nodeId), synField));
            }
        }
        else
            result = RV_PVT_ERROR_UNKNOWN;
    }

    return result;
}


/************************************************************************
 * vtGetByIndex
 * purpose: Returns the ID of a child node based on the child's index
 * input  : vt          - Pointer to VT struct
 *          parentId    - The Node ID of the immediate parent
 *          index       - The index of the child, as determined by pvtAdd().
 *                        This index is 1-based.
 * output : childNodeId - The ID of the node that is found, or negative value on failure
 *                        If set to NULL, it will be discarded
 * return : Child's node id on success
 *          Negative value on failure
 ************************************************************************/
RvPvtNodeId vtGetByIndex(
    IN  vtStruct        *vt,
    IN  RvPvtNodeId     parentId,
    IN  RvInt32         index,
    OUT RvPvtNodeId*    childNodeId)
{
    RvPvtNodeId path;
    
    path = (RvPvtNodeId)rtGetByIndex(vt->vTree, (int)parentId, (int)index);
    if (RV_PVT_NODEID_IS_VALID(path)) {
        if (childNodeId) *childNodeId = path;
    }
    return path;
}


/************************************************************************
 * vtGetNodeIdByPath
 * purpose: Returns the ID of a node based on a path that starts from a
 *          specified root
 * input  : vt                  - Pointer to VT struct
 *          searchRootNodeId    - The ID of the node from which the search starts from
 *          path                - The path to the searched node. format: "a.b.c"
 * output : none
 * return : Node ID we reached on success
 *          Negative value on failure
 ************************************************************************/
RvPvtNodeId vtGetNodeIdByPath(
    IN  vtStruct    *vt,
    IN  RvPvtNodeId nodeId,
    IN  const char* path)
{
    char name[256];
    char *nameptr;
    RvPstFieldId fieldEnum;
    HPST hSyn;

    hSyn = vtGetSynTree(vt, nodeId);

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

            if (isNumber(name))
            {   /* sequence of index */
                nodeId = (RvPvtNodeId)rtGetByIndex(vt->vTree, (int)nodeId, atoi(name));
            }
            else  {  /* other */

                /* -- '*': semantic for 'next child ' */
                if (name[0] == '*'  &&  name[1] == '\0')
                {  /* go to child */
                    nodeId = (RvPvtNodeId)rtGetByIndex(vt->vTree, (int)nodeId, 1);
                }
                else
                {
                    if ((fieldEnum = pstGetFieldId(hSyn, name)) == RV_ERROR_UNKNOWN)
                    {
                        nodeId = RV_PVT_ERROR_UNKNOWN;
                        break;
                    }
                    nodeId = (RvPvtNodeId)rtGetChild(vt->vTree, (int)nodeId, (void *)fieldEnum, 1);
                }
            }

            /* RV_ERROR_UNKNOWN? */
            if (nodeId < 0) break;
        }
    } while (*path);

    return nodeId;
}


/**************************************************************************************
 * vtGetNodeIdByFieldIds
 *
 * Purpose: find node id by traversing both value and syntax trees according to a path
 *          of filed Ids.
 *              - Field Ids in the path should be existing Field Ids.
 *              - Child indexes should be negative values starting from -101.
 *              - for '*' -1 is used
 *              - to add element to SEQUENCE OF -100 may be used or the last element index + 1
 *              - The list should end with LAST_TOKEN value
 *
 * Input:   vt          - Pointer to VT struct
 *          nodeId      - The start of the val sub tree.
 *          path        - An array of fieldIds, ending with LAST_TOKEN
 *
 *
 * Return value: The found node id or negative value on failure
 **************************************************************************************/
RvPvtNodeId vtGetNodeIdByFieldIds(
    IN  vtStruct        *vt,
    IN  RvPvtNodeId     nodeId,
    IN  const RvInt16   *path)
{
    RvPstFieldId    fieldEnum;
    RvPstFieldId    token;
    int             index;

    /* Go over each field id in the path */
    for (token = 0; (path[token] != LAST_TOKEN) && (token < MAX_TOKEN); token++)
    {
          /* a number less than -100 indicates a SEQUENCE OF element's index */
        if (path[token] <= -100)
        {
            /* translate it to index (-101 -> 1; -102 -> 2 etc.) */
            index = -(path[token] + 100);
            /* Get the node id of the given element */
            if ((nodeId = vtGetByIndex(vt, nodeId, index, NULL)) < 0) {
                return RV_PVT_ERROR_UNKNOWN;
            }
        }
        else
        {  /* other (not SEQUENCE OF) */

            /* -1 ('*') is a semantic symbol for 'next child ' */
            if (path[token] == -1)
            {
                /* get the first child's node id */
                if ((nodeId = vtChild(vt, nodeId)) < 0) {
                    return RV_PVT_ERROR_UNKNOWN;
                }
            }
            else
            {
                /* Search for the child with that id amongst the child elements */
                fieldEnum=path[token];
                if ((nodeId = vtGetChild(vt, nodeId, fieldEnum, NULL)) < 0) {
                    return RV_PVT_ERROR_UNKNOWN;
                }
            }
        }
    }
    
    /* sanity check */
    if (token >= MAX_TOKEN) 
        return RV_PVT_ERROR_UNKNOWN;

    return nodeId;
}


/************************************************************************
 * vtSearchPath
 * purpose: Searches for a specified path, value or both in a specified tree
 *          This function is called by pvtSearchPath. It does less sanity
 *          checks and has no locking mechanisms in it
 * input  : vtDest      - VT struct of the search tree
 *          rootNodeId  - Root ID of the search tree
 *          vtSrc       - VT struct of path to be searched in
 *          srcNodeId   - Node ID of the beginning of the path to be searched in
 *          checkLeaves - If RV_TRUE, Compare the values in the lowest level nodes (leaves)
 *                        Use RV_FALSE if the tree syntax contains choices.
 * output : none
 * return : RV_TRUE if found, RV_FALSE if not
 *          Negative value on failure
 ************************************************************************/
int vtSearchPath(
    IN  vtStruct*   vtDest,
    IN  RvPvtNodeId rootNodeId,
    IN  vtStruct*   vtSrc,
    IN  RvPvtNodeId srcNodeId,
    IN  RvBool      checkLeaves)
{
    vtNode *vtNodeDest, *vtNodeSrc;
    RvPvtNodeId destCur, srcCur;
    RvPstNodeId destSynNodeId, srcSynNodeId;
    HPST hSyn;

    /*  RvPstFieldId destFieldId, srcFieldId;
    char *destString, *srcString;
    = pvtGetSynTree(destH, rootNodeId); */

    /*vtPrint(destH, rootNodeId, ErrVt); */
    vtNodeDest = GET_NODE(vtDest, rootNodeId);
    vtNodeSrc  = GET_NODE(vtSrc,  srcNodeId);
    hSyn       = vtNodeDest->hSyn;

    /*  vtGet(srcH, srcNodeId, &srcFieldId, &srcSynNodeId, &srcValue, &srcString);
    vtGet(destH, rootNodeId, &destFieldId, &destSynNodeId, &destValue, &destString);
    */
    destSynNodeId = VTN_SYN_NODE(vtNodeDest);
    srcSynNodeId  = VTN_SYN_NODE(vtNodeSrc);

    if (!pstAreNodesCongruent(
        hSyn,             destSynNodeId,
        vtNodeSrc->hSyn,  srcSynNodeId))
    {
        return RV_ERROR_UNKNOWN;
    }

    for (destCur = rootNodeId, srcCur = srcNodeId;
         destCur >=0 && srcCur >=0; )
    {
        destSynNodeId = VTN_SYN_NODE(vtNodeDest);
        srcSynNodeId  = VTN_SYN_NODE(vtNodeSrc);

        if (destSynNodeId != srcSynNodeId)
        {
            RvLogError(&vtSrc->log,
                (&vtSrc->log, "vtSearchPath: %s Different synNodeId [%d <> %d].",
                     getSafeFieldName(vtNodeSrc), destSynNodeId, srcSynNodeId));
            return RV_FALSE;
        }

        switch (pstGetNodeType(vtNodeDest->hSyn, destSynNodeId))
        {
            case pstSet:
            case pstSequence:  /* investigate all children */
                {
                    RvPstFieldId fieldId = (RvPstFieldId)-1;
                    RvPvtNodeId srcChild = vtChild(vtSrc, srcCur);
                    RvPvtNodeId destChild;

                    while (RV_PVT_NODEID_IS_VALID(srcChild))
                    {
                        vtGet(vtSrc, srcChild, &fieldId, NULL, NULL, NULL);
                        destChild = vtGetChild(vtDest, destCur, fieldId, NULL);

                        if (!RV_PVT_NODEID_IS_VALID(destChild))
                        {
                            RvLogError(&vtSrc->log,
                                (&vtSrc->log, "vtSearchPath:Sequence: %s not in destination.",
                                     pstGetFieldNamePtr(hSyn, fieldId)));
                            return RV_FALSE;
                        }
                        if (vtSearchPath(vtDest, destChild, vtSrc, srcChild, checkLeaves) != RV_TRUE)
                            return RV_FALSE;
                        srcChild = vtBrother(vtSrc, srcChild);
                    }
                }
                return RV_TRUE;

            case pstChoice:
                break;

            case pstSetOf:
            case pstSequenceOf:
                {
                    int j;
                    int children = vtNumChilds(vtDest, destCur);
                    RvPvtNodeId child = vtChild(vtDest, destCur);
                    RvPvtNodeId srcChild = vtChild(vtSrc, srcCur);

                    vtSetSyn(vtSrc, hSyn, srcCur, -1, -1, NULL);

                    for (j = 0; j < children; j++)
                    {
                        if (vtSearchPath(vtDest, child, vtSrc, srcChild, checkLeaves) == RV_TRUE)
                        {
                            RvLogDebug(&vtSrc->log,
                                (&vtSrc->log, "vtSearchPath:SetOf: [%d] %s found.",
                                     j + 1, getSafeFieldName(vtNodeSrc)));
                            return vtSetSyn(vtSrc, hSyn, srcCur, -1, j + 1, NULL);
                        }
                        child = vtBrother(vtDest, child);
                    }
                    RvLogError(&vtSrc->log,
                        (&vtSrc->log, "vtSearchPath:SetOf: subtree of %s not found.",
                             getSafeFieldName(vtNodeSrc)));
                }
                return RV_FALSE;

            default:
            {
                /* leaf holds value */

                if (!checkLeaves)
                {
                    /* not checking leaves - no need for comparison */
                    return RV_TRUE;
                }

                RvLogDebug(&vtSrc->log,
                    (&vtSrc->log, "vtSearchPath:Leaf:%s: %d <-> %d.",
                         getSafeFieldName(vtNodeSrc), vtNodeDest->value, vtNodeSrc->value));

                /* compare values: */
                if (vtNodeDest->value != vtNodeSrc->value)
                    return RV_FALSE;

                /* see if both had/don't have strings: */
                if ((vtNodeDest->string && !vtNodeSrc->string) ||
                    (!vtNodeDest->string && vtNodeSrc->string))
                    return RV_FALSE;

                /* compare strings */
                if (vtNodeDest->string)
                {
                    if (rpoolCompareInternal(vtDest->sPool, vtNodeDest->string,
                        vtNodeSrc->string, (int)vtNodeDest->value))
                    {
                        return RV_FALSE;
                    }
                }

                /* matched */
                return RV_TRUE;
            }
        }

        {
            int synField;
            srcCur = vtChild(vtSrc, srcCur);
            vtNodeSrc = GET_NODE(vtSrc,  srcCur);
            synField = VTN_SYN_FIELD(vtNodeSrc);
            destCur = vtGetChild(vtDest, destCur, synField, NULL);
            vtNodeDest = GET_NODE(vtDest, destCur);
        }
    }

    return (srcCur < 0)? RV_TRUE : RV_FALSE;
}


/************************************************************************
 * Setting and adding nodes
 ************************************************************************/


/* syntax node id or negative value on failure */
RvPstNodeId vtResolveSynNodeId(
    IN  vtStruct        *vt,
    IN  HPST            hSyn,
    IN  RvPvtNodeId     valParentId,
    IN  RvPstFieldId    fieldEnum,
    OUT int             *index /* of child in parent */
    )
{
    RvPstNodeId parentSynNodeId = RV_PST_INVALID_NODEID;
    pstChildExt child;
    RvPstFieldId parentFieldId = RV_PST_INVALID_NODEID;

    int _index;
    pstNodeType type;
    if (index) *index = RV_ERROR_UNKNOWN;

    /* -- resolve syntax node id */
    if (vtGet(vt, valParentId, &parentFieldId, &parentSynNodeId, NULL, NULL) <0)
        return RV_PST_ERROR_UNKNOWN;

    /* -- SEQUENCE OF */
    if (((type = pstGetNodeType(hSyn, parentSynNodeId))) != (pstNodeType)RV_ERROR_UNKNOWN)
        if (type == pstSequenceOf || type == pstSetOf)
            return pstGetNodeOfId(hSyn,parentSynNodeId);

    if ( (_index = pstGetFieldExt(hSyn, parentSynNodeId, (int)fieldEnum, &child)) <0)
    {
        RvLogWarning(&vt->log,
            (&vt->log, "vtResolveSynNodeId: child '%s' does not exist under '%s'.",
                pstGetFieldNamePtr(hSyn, fieldEnum), pstGetFieldNamePtr(hSyn, parentFieldId)));
        return RV_PST_ERROR_UNKNOWN;
    }
    if (index) *index = _index;

#if (RV_ASN1_CODER_USE_H450 == RV_YES)
    if (child.speciality!=pstDependingDependent &&
        child.speciality!=pstDependent)
        return child.nodeId;

    {
        RvPvtNodeId parentId;
        pstChildExt field;
        pstConstrainingField constrainingField;
        int objectId;
        int i;
        RvPvtNodeId childNodeId;
        pstTypeFromConstraint specialType;
        pstFieldOfObjectReference fieldOfObject;

        /* getting special type */
        pstGetTypeFromConstraint(hSyn, child.nodeId, &specialType);

        /* getting constraining field */
        pstGetConstrainingField(hSyn, child.nodeId, &constrainingField);

        /* getting field itself(node) */
        /* here very intimate knowledge of the internal database is used*/
        /* it is known that fields are placed at nodeId+fieldIndex position */

        pstGetChildExt(hSyn, constrainingField.fieldId, 0, &field);

        for(i=0 , parentId = valParentId; i < constrainingField.parentDepth; i++) /* going to parent */
            parentId = vtParent(vt ,parentId);

        if((childNodeId = vtGetChild(vt, parentId, field.fieldId, &childNodeId)) < 0) /* going to depending field */
            return RV_PST_ERROR_UNKNOWN;

        if (vtFindObject(vt, childNodeId, hSyn, 
                        specialType.objectSetId, &objectId) == RV_ERROR_UNKNOWN) /* finding object */
            return RV_PST_ERROR_UNDEFINED_TYPE;

        for(i=1 ;; i++) /* finding field of object */
        {
            if (pstGetFieldOfObjectReference(hSyn, objectId, i, &fieldOfObject) < 0)
                return RV_PST_ERROR_UNKNOWN;
            if (fieldOfObject.fieldInClassNumber == specialType.fieldInClassNumber)
                break;
        }

        return fieldOfObject.settingId;
    }
#else
    return child.nodeId;
#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */
}


/************************************************************************
 * vtSetNode
 * purpose: Set a node's value
 * input  : vt          - VT struct to use
 *          hSyn        - Syntax tree to use
 *                        Can be set to -1 if it will be supplied later (by vtSetTree)
 *          nodePath    - Node Id to set
 *          nodeSynId   - Syntax node Id of the node or negative value on failure if unknown
 *          parentId    - Parent's node Id or negative value on failure if a root node
 *          fieldEnum   - Syntax tree field inside a type, or negative value on failure if a root node
 *          value       - Value of the root node, or length of the value if
 *                        value is a string
 *          string      - String value of the node, or NULL if not a string
 * output : index       - Index of the new node (1-based)
 * return : Node id of the added root on success
 *          Negative value on failure
 ************************************************************************/
RvPvtNodeId vtSetNode(
    IN  vtStruct        *vt,
    IN  HPST            hSyn,
    IN  RvPvtNodeId     nodePath,
    IN  RvPstNodeId     nodeSynId,
    IN  RvPvtNodeId     parentId,
    IN  RvPstFieldId    fieldEnum,
    IN  RvInt32         value,
    IN  const char*     string,
    OUT int*            index)
{
    vtNode      *node;
    char        bitsNotInUse;
    RvPstNodeId synField;
    RvInt32     copiedValue = value; /* Number of bytes to copy from the user - used for bit strings */
    pstNodeType nodeType;
    RvBool      isBitStr = RV_FALSE;

    node = (vtNode *)rtGetByPath(vt->vTree, (int)nodePath);
    if (node == NULL) {
        return RV_PVT_ERROR_UNKNOWN;
    }

    node->hSyn = hSyn;
    if (!RV_PST_NODEID_IS_VALID(nodeSynId)) {
        nodeSynId = vtResolveSynNodeId(vt, hSyn, parentId, fieldEnum, NULL);
    }

    VTN_SET_SYN_INFO(node, nodeSynId, fieldEnum);

    if (index != NULL)
    {
        if (RV_PVT_NODEID_IS_VALID(parentId))
            *index = rtIndex(vt->vTree, (int)parentId, (int)nodePath);
        else
            *index = 1;
    }

    /* Find out the node's type - we will need it... */
    synField = VTN_SYN_NODE(node);
    nodeType = pstGetNodeType(hSyn, synField);

    /* See if it's a bit string (If the value is negative, then the node is in 
       a temporary building state) */
    if ((nodeType == pstBitString) && (value >= 0))
    {
        /* We have a bit string to handle. In this case, the length we get is in bits,
           so we should change it to bytes and add another character in the end, telling the
           amount of bits in the last character... */
        bitsNotInUse = (char)((8 - (value % 8))%8);
        copiedValue = (value + 7) / 8; /* Number of bytes we have in the bit string itself */
        if (copiedValue > 0)
            value = copiedValue + 1; /* We add the indication of how many bits are not used to the node */
        else
            value = 0; /* No need to store a string at all */
        isBitStr = RV_TRUE;
    }

    /* If we have a string with the same length already we'll use that string's allocation
       instead of reallocating the new one */
    if ((node->string != NULL) && ((node->value != value) || (string == NULL)))
    {
        /* This is node holds a string and we're replacing it with an empty one or with one
           with a different length - in this case, we first delete the original string */
        rpoolFree(vt->sPool, node->string);
        node->string = NULL;
    }

    if ((vtIsStringNodeType(nodeType) || (string != NULL)) && (value > 0))
    {
        /* We have some kind of a string to handle... */

        if (node->string == NULL)
        {
            /* No string yet - allocate a brand new one and fill it in with the desired value */
            node->string = rpoolAllocCopyExternal(vt->sPool, 0, string, (int)copiedValue);
        }
        else
        {
            /* Looks like we have the same length in the original string this node held and
               the new one - just replace the original one if necessary. */
            if (rpoolCopyFromExternal(vt->sPool, node->string, string, 0, (int)copiedValue) == NULL)
                return RV_PVT_ERROR_UNKNOWN;
        }

        if (node->string == NULL)
            return RV_ERROR_UNKNOWN;

        if (isBitStr)
        {
            /* For bit strings, we already copied the string inself, we only have to put
               the indication of how many bits are not used in the last character of this
               bit string... */
            if (rpoolCopyFromExternal(vt->sPool, node->string, &bitsNotInUse, copiedValue, 1) == NULL)
                return RV_PVT_ERROR_UNKNOWN;
        }
    }

    /* Set the value of this node */
    node->value = copiedValue;
    return nodePath;
}

/*  new node id or negative value on failure
    Add child node under parentId.
    The new node is placed in its relative position according to syntax tree
    indexing of SEQUENCE fields of structure. */
RvPvtNodeId vtAddSyn(
    IN  vtStruct        *vt,
    IN  HPST            hSyn, /* -1 ==> copy from parent node */
    IN  RvPvtNodeId     parentId,
    IN  RvPstFieldId    fieldEnum, /* field inside SEQUENCE or CHOICE, or -1 if not applicable */
    IN  RvInt32         value, /* if string exists ==> size of string , if Bit String - length in bit*/
    IN  const char      *string, /* NULL if no string */
    OUT RvPvtNodeId     *newNodeId, /* id for the new node */
    OUT int             *index) /* index of new node */
{
    int             childIndex;
    int             childCount;
    RvPvtNodeId     path = RV_PVT_INVALID_NODEID;
    RvPvtNodeId     cur;
    RvPvtNodeId     pre;
    RvPstFieldId    curFieldId;
    RvPstNodeId     synParentNodeId;
    RvPstNodeId     synNodeId;
    pstNodeType     type;
    vtNode          *parentNode;

    if (!vt || !RV_PVT_NODEID_IS_VALID(parentId)) 
        return RV_PVT_ERROR_UNKNOWN;

    /* -- Check choice brothers and delete if exist */
    parentNode = (vtNode *)rtGetByPath(vt->vTree, (int)parentId);
    if (!parentNode) {
        return RV_PVT_ERROR_UNKNOWN;
    }

    synParentNodeId = VTN_SYN_NODE(parentNode);
    if (hSyn == (HPST)-1) {
        hSyn = parentNode->hSyn;
    }

    type = pstGetNodeType(hSyn, synParentNodeId);
    if ((int)type < 0) {
        return RV_PVT_ERROR_UNKNOWN;
    }

    if ((type == pstChoice) || (type == pstEnumeration)) {
        vtDelete(vt, vtChild(vt, parentId));
    }

    synNodeId = vtResolveSynNodeId(vt, hSyn, parentId, fieldEnum, &childIndex);
    if (!RV_PST_NODEID_IS_VALID(synNodeId))
    {
        RvLogWarning(&vt->log,
            (&vt->log, "vtAddSyn: vtResolveSynNodeId failed for %s (%d).",
                 pstGetFieldNamePtr(hSyn, fieldEnum), fieldEnum));
        return RV_PVT_ERROR_UNKNOWN;
    }
    if (childIndex < 0)
    { /* SEQUENCE OF */
        if ((path = (RvPvtNodeId)rtAddTail(vt->vTree, (int)parentId, NULL)) <0)
        {
            RvLogError(&vt->log,
                (&vt->log, "vtAddSyn: rtAddTail failed for %s (%d).",
                    pstGetFieldNamePtr(hSyn, fieldEnum), fieldEnum));
            return RV_PVT_ERROR_OUTOFRESOURCES;
        }
    }
    else
    {
        /* Find the left brother of the new node (or none) so we know
           where to insert in the child list. */

        /* pre: the previous node - the left brother */
        pre = RV_PVT_INVALID_NODEID;  /* RV_ERROR_UNKNOWN -> add as leftmost */
        childCount = pstGetNumberOfChildren(hSyn, synParentNodeId);
        if (childCount > 0)
        {
            cur = vtChild(vt, parentId);
            if (RV_PVT_NODEID_IS_VALID(cur)) {
                pstChildExt st_child;
                int         stChildIndex = 1;
                vtNode      *node;
                RvUint32*   iter;
                RvBool stop = RV_FALSE;

                iter = pstGetChildExt(hSyn, synParentNodeId, stChildIndex, &st_child);

                node = (vtNode *)rtGetByPath(vt->vTree, (int)cur);
                if (node)
                    curFieldId = VTN_SYN_FIELD(node);
                else
                    curFieldId = RV_ERROR_UNKNOWN;

                while (!stop)
                {
                    /* this is just a sanity check: */
                    if (stChildIndex > childCount)
                    {
                        /* should never reach here */
                        RvLogError(&vt->log, (&vt->log, "vtAddSyn: probable integrity failiure in vt %p.", vt));
                        pre = RV_PVT_INVALID_NODEID; /* allow leftmost insertion */
                        break;
                    }

                    if (iter == NULL)
                    {
                        pre = RV_PVT_INVALID_NODEID;
                        break;
                    }
                    if (st_child.fieldId == fieldEnum)
                        break;
                    if (st_child.fieldId != curFieldId)
                    {
                        stChildIndex++;
                        iter = pstGetBrotherExt(hSyn, stChildIndex, iter, &st_child);
                        continue;
                    }
                    pre = cur;
                    cur = (RvPvtNodeId)rtBrother(vt->vTree, (int)cur);

                    if (cur >= 0)
                    {
                        node = (vtNode *)rtGetByPath(vt->vTree, (int)cur);
                        if (node)
                            curFieldId = VTN_SYN_FIELD(node);
                        else
                            curFieldId = RV_ERROR_UNKNOWN;
                    }
                    else
                    {
                        stop = RV_TRUE; /* Make sure we get out of this while loop */
                    }

                }

                if (cur>=0 && curFieldId==fieldEnum)
                    path=cur;
            }
        }
        if (path<0)
        {
            if (pre < 0)
            {
                if ((path = (RvPvtNodeId)rtAddHead(vt->vTree, (int)parentId, NULL)) <0)
                {
                    RvLogError(&vt->log,
                        (&vt->log, "vtAddSyn: rtAddHead (left brother) failed for %s (%d).",
                             pstGetFieldNamePtr(hSyn, fieldEnum), fieldEnum));
                    return RV_PVT_ERROR_OUTOFRESOURCES;
                }
            }
            else
            {
                if ((path = (RvPvtNodeId)rtAddBrother(vt->vTree, (int)pre, NULL)) <0)
                {
                    RvLogError(&vt->log,
                        (&vt->log, "vtAddSyn: rtAddBrother failed for %s (%d).",
                             pstGetFieldNamePtr(hSyn, fieldEnum), fieldEnum));
                    return RV_PVT_ERROR_OUTOFRESOURCES;
                }
            }
        }
    }

    if (newNodeId) *newNodeId = path;
    return vtSetNode(vt, hSyn, path, synNodeId, parentId, fieldEnum, value, string, index);
}


/* Sets values for existing node */
int vtSetSyn(
    IN  vtStruct        *vt,
    IN  HPST            hSyn, /* -1 ==> don't change */
    IN  RvPvtNodeId     nodeId,
    IN  RvPstFieldId    fieldEnum, /* <0 not changing current value */
    IN  RvInt32         value,      /* if bit string - length in bits */
    IN  const char      *string)
{
    vtNode      *node;
    char        bitsNotInUse;
    RvBool      isBitStr = RV_FALSE;
    RvPstNodeId synField;
    pstNodeType nodeType;
    RvInt32     copiedValue = value; /* How many bytes to copy - used for bit strings */

    node = (vtNode *)rtGetByPath(vt->vTree, (int)nodeId);
    if (node == NULL)
        return RV_ERROR_UNKNOWN;

    if (hSyn == (HPST)-1) {
        hSyn = node->hSyn;
    }
    else {
        node->hSyn = hSyn;
    }

    if ( fieldEnum >= 0 )
    {
        /* See if the field Id was changed */
        if (fieldEnum != (RvPstFieldId)VTN_SYN_FIELD(node))
        {
            RvPvtNodeId parentId = vtParent(vt, nodeId);

            /* Make sure it's not a root node */
            if (RV_PVT_NODEID_IS_VALID(parentId))
            {
                RvPstNodeId synNodeId = vtResolveSynNodeId(vt, hSyn, parentId, fieldEnum, NULL);
                VTN_SET_SYN_NODE(node, synNodeId);
            }
        }
        VTN_SET_SYN_FIELD(node, fieldEnum);
    }

    /* Find out the node's type - we'll need it... */
    synField = VTN_SYN_NODE(node);
    nodeType = pstGetNodeType(hSyn, synField);

    /* See if it's a bit string */
    if (nodeType == pstBitString)
    {
        /* We have a bit string to handle. In this case, we the length we get is in bits,
           so we should change it to bytes and add another character in the end, telling the
           amount of bits in the last character... */
        bitsNotInUse = (char)((8 - (value % 8))%8);
        copiedValue = (value + 7) / 8; /* Number of bytes we have in the bit string itself */
        if (copiedValue > 0)
            value = copiedValue + 1; /* We add the indication of how many bits are not used to the node */
        else
            value = 0; /* No need to store a string at all */
        isBitStr = RV_TRUE;
    }

    /* If we have a string with the same length already we'll use that string's allocation
       instead of reallocating the new one */
    if ((node->string != NULL) && ((node->value != value) || (string == NULL)))
    {
        /* This is node holds a string and we're replacing it with an empty one or with one
           with a different length - in this case, we first delete the original string */
        rpoolFree(vt->sPool, node->string);
        node->string = NULL;
    }

    if ((vtIsStringNodeType(nodeType) || (string != NULL)) && (value > 0))
    {
        /* We have some kind of a string to handle... */

        if (node->string == NULL)
        {
            /* No string yet - allocate a brand new one and fill it in with the desired value */
            node->string = rpoolAllocCopyExternal(vt->sPool, 0, string, (int)copiedValue);
        }
        else
        {
            /* Looks like we have the same length in the original string this node held and
               the new one - just replace the original one if necessary. */
            if (rpoolCopyFromExternal(vt->sPool, node->string, string, 0, (int)copiedValue) == NULL)
                return RV_PVT_ERROR_UNKNOWN;
        }

        if (node->string == NULL)
            return RV_ERROR_UNKNOWN;

        if (isBitStr)
        {
            /* For bit strings, we already copied the string inself, we only have to put
               the indication of how many bits are not used in the last character of this
               bit string... */
            if (rpoolCopyFromExternal(vt->sPool, node->string, &bitsNotInUse, copiedValue, 1) == NULL)
                return RV_PVT_ERROR_UNKNOWN;
        }
    }

    /* Set the value of this node */
    node->value = copiedValue;

    return RV_TRUE;
}


#if (RV_ASN1_CODER_USE_H450 == RV_YES)
/************************************************************************
 * vtCompare
 * purpose: Comparison function used when searching for a specific object
 *          in an association table.
 * input  : hSyn        - Syntax tree used
 *          sNodeId     - Syntax value node to compare
 *          context     - Context to use for comparison
 * output : none
 * return : Negative if the key is lower than elem in dictionary comparison
 *          0 if the key and the element are equal
 *          Positive if the key is higher than elem in dictionary comparison
 ************************************************************************/
int vtCompare(
    IN HPST hSyn, 
    IN int sNodeId, 
    IN void* context)
{
    pvtCompareContext* key = (pvtCompareContext *)context;

    int cmp,vSynNodeId=-1,vValue=0,stNodeId;
    RvPvtNodeId vtNodeId;
    RvPstFieldId vFieldId=-1;
    pstValueTreeStruct sValTreeStruct;
    pstValueNodeStruct sValNodeStruct;
    char vString[2048];
    vtStruct *vt;

    vt = (vtStruct*)key->hVal;
    vtNodeId = key->nodeId;
    
    vtGet(vt, key->nodeId, NULL, &vSynNodeId, NULL, NULL);

    pstGetValueTreeStruct(hSyn, sNodeId, &sValTreeStruct);

    if (sValTreeStruct.isString != pstValueSubTree)
    {
        if (vtGetByIndex(vt, vtNodeId, 1, NULL) >= 0)
            return RV_TRUE;

        vtGet(vt, vtNodeId, NULL, NULL, &vValue, NULL);
        vtGetString(vt, vtNodeId, vValue, vString);

        if (sValTreeStruct.isString == pstValueString)
        {
            return(strcmp(vString, pstGetFieldNamePtr(hSyn,sValTreeStruct.value)));
        }
        else
        {
            if (vValue > sValTreeStruct.value) 
                return RV_TRUE;
            else {
                if (vValue < sValTreeStruct.value) 
                    return RV_ERROR_UNKNOWN;
                else 
                    return RV_FALSE;
            }
        }
    }

    for(stNodeId = stGetFirstValueNode(hSyn,sNodeId), vtNodeId = vtChild(vt, vtNodeId);
        (vtNodeId >= 0) && (stNodeId >= 0);
        vtNodeId = vtBrother(vt, vtNodeId),stNodeId=stGetNextValueNode(hSyn,stNodeId))
    {
        vtGet(vt, vtNodeId, &vFieldId, NULL, NULL, NULL);
        pstGetValueNodeStruct(hSyn, stNodeId, &sValNodeStruct);
        if(vFieldId > sValNodeStruct.fieldName)
            return RV_TRUE;
        else
            if(vFieldId<sValNodeStruct.fieldName)
                return RV_ERROR_UNKNOWN;
        else
        {
            pvtCompareContext newContext;
            newContext.hVal = key->hVal;
            newContext.nodeId = vtNodeId;
            cmp = vtCompare(hSyn, stNodeId, &newContext);
            if (cmp != 0)
                return cmp;
        }
    }

    return RV_FALSE;
}
#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */




/************************************************************************
 *
 *                          Public functions
 *
 ************************************************************************/


/************************************************************************
 * pvtCompareTree
 * purpose: Compare between two trees
 *          The trees must be structure identical, the compare function only
 *          checks that the values are identical.
 * input  : val1H       - PVT handle of the 1st tree
 *          val1RootId  - Root ID of the 1st tree
 *          val2H       - PVT handle of the 2nd tree
 *          val2RootId  - Root ID of the 2nd tree
 * output : none
 * return : Non-negative value if trees are identical
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtCompareTree(
    IN  HPVT        val1H,
    IN  RvPvtNodeId val1RootId,
    IN  HPVT        val2H,
    IN  RvPvtNodeId val2RootId)
{
    vtStruct *vt1;
    vtStruct *vt2;
    int     result;

    if (!val1H || !val2H) return RV_ERROR_UNKNOWN;

    /* Make sure we lock an an orderly fashion to diminish dead-locking possibilities */
    if (val1H > val2H)
    {
        vt1 = (vtStruct *)val2H;
        vt2 = (vtStruct *)val1H;
    }
    else
    {
        vt1 = (vtStruct *)val1H;
        vt2 = (vtStruct *)val2H;
    }

    RvLockGet(&vt1->lock, vt1->logMgr);
    if (vt1 != vt2) RvLockGet(&vt2->lock, vt2->logMgr);
    result = rtCompareTrees(vt1->vTree, (int)val1RootId, vt2->vTree, (int)val2RootId, vtCompareTwo,
        (void *)val1H);
    if (vt1 != vt2) RvLockRelease(&vt2->lock, vt2->logMgr);
    RvLockRelease(&vt1->lock, vt1->logMgr);

    return result;
}



#if (RV_ASN1_CODER_USE_H450 == RV_YES)
RVAPI int RVCALLCONV pvtFindObject(
    IN  HPVT           hVal,
    IN  RvPvtNodeId    nodeId,
    IN  HPST           hSyn,
    IN  int            stNodeIdOfAT,
    OUT int*           objectId)
{
    vtStruct* vt = (vtStruct *)hVal;
    if (vt == NULL) return RV_ERROR_UNKNOWN;


    RvLockGet(&vt->lock, vt->logMgr);
    vtFindObject(vt, nodeId, hSyn, stNodeIdOfAT, objectId);
    RvLockRelease(&vt->lock, vt->logMgr);

    return (*objectId);
}
#endif

/*
RVAPI int RVCALLCONV pvtAddChildsIfDiffer(
    IN  HPVT        destH,
    IN  RvPvtNodeId destParentId,
    IN  HPVT        srcH,
    IN  RvPvtNodeId srcParentId,
    IN  RvBool      move)
{
    RvPvtNodeId destCur, srcCur, leftBrother = RV_PVT_ERROR_UNKNOWN;
    RvBool notFound;

    if (!destH || !srcH) return RV_ERROR_UNKNOWN;

    destCur = pvtChild(destH, destParentId);
    if (destCur<0)
        return pvtAddChilds(destH, destParentId, srcH, srcParentId);

    for(; destCur >=0; destCur = pvtBrother(destH, destCur))
    leftBrother = destCur;

    for(srcCur = pvtChild(srcH, srcParentId); srcCur >= 0; srcCur = pvtBrother(srcH, srcCur))
    {
        notFound = RV_TRUE;
        for(destCur = pvtChild(destH, destParentId);
            destCur >= 0;
            destCur = pvtBrother(destH, destCur))
            if (pvtCompareTree(destH, destCur, srcH, srcCur) == RV_TRUE)
            {
                notFound = RV_FALSE;
                break;
            }

        if (notFound) // srcCur child not found in dest children 
        {
            if (move)
            {
                pvtAdoptChild(destH, srcCur, destParentId, leftBrother);
                leftBrother = pvtBrother(destH, leftBrother);
            }
            else
                pvtAddTree(destH, destParentId, srcH, srcCur);
        }
    }
    return RV_TRUE;

}
*/


#if defined(RV_DEBUG)

/************************************************************************
 * pvtAddRootShielded
 * purpose: Adds a new shielded node that constitutes the root of a new tree.
 * input  : valH    - PVT handle to use
 *          synH    - Syntax tree to use
 *                    Can be set to -1 if it will be supplied later (by pvtSetTree)
 *          value   - Value of the root node, or length of the value if
 *                    value is a string
 *          string  - String value of the node, or NULL if not a string
 * output : none
 * return : Node id of the added root on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV pvtAddRootShielded(
    IN  HPVT        valH,
    IN  HPST        synH,
    IN  RvInt32     value,
    IN  const char* string)
{
    RvPvtNodeId newNode =  pvtAddRootFromInt(valH, synH, RV_PST_INVALID_NODEID, value,  string, __FILE__, __LINE__);
    if (RV_PVT_NODEID_IS_VALID(newNode))
    {
        vtStruct *vt = (vtStruct *)valH;
        vtNode * node = (vtNode *)rtGetByPath(vt->vTree, (int)newNode);
        node->shieldedNode = RV_TRUE;
    }
    return newNode;
}


/************************************************************************
 * pvtDeleteShielded
 * purpose: Deletes a shielded sub-tree
 * input  : valH            - PVT handle to use
 *          subTreeRootId   - The ID of the root node to delete
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtDeleteShielded(
    IN  HPVT          hVal,
    IN  RvPvtNodeId   subTreeRootId)
{
    vtStruct *vt = (vtStruct *)hVal;
    vtNode * node;

    if ( (node = (vtNode *)rtGetByPath(vt->vTree, (int)subTreeRootId)) == NULL)
        return RV_PVT_ERROR_UNKNOWN;
    node->shieldedNode = RV_FALSE;
    return pvtDelete(hVal, subTreeRootId);
}

#endif /* defined(RV_DEBUG) */


#ifdef __cplusplus
}
#endif



