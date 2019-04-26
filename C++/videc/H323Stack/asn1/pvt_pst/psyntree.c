/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION RADVISION Ltd.. No part of this document may be reproduced
in any form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

/*
  psyntree.c

  Provides access to syntax tree array.

  Syntax tree format:
  ===================

   -- Header:
    - idSize (small) #bytes for id
    - syntax tree offset (id)
    - field array offset (id)
    - from array offset (id)
    - total size (id)

   -- Syntax tree:
    -- list of nodes:
     -- node:
      - type (small). e.g. SEQUENCE, SET, CHOICE.
      - tag (int)
      - tag class (small)
      - extension exists (bool).
      - number of childs (small).
      - from (int)
      - to (int)
      - OF (id). from node list.
      - FROM (id). from from string array.

      - list of childs:
       -- child:
        - field id (id). from field name array.
    - struct id (id). from node list.
    - is optional (bool)
    - is extended (bool)

   -- Field array:
    - list of strings, each terminated by NULL.

   -- from array:
    - list of strings, each terminated by NULL.

  */

#include "rvstdio.h"
#include "rvmemory.h"
#include "h32xLog.h"
#include "intutils.h"
#include "strutils.h"
#include "psyntreeDef.h"
#include "psyntreeStackApi.h"
#include "psyntreeDb.h"
#include "psyntree.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


#define RV_MAX_PATH 300

#define nprn(s) ((s)?(s):"(null)")


#if (RV_ASN1_CODER_USE_Q931 == RV_YES)
char *
pstGetTagClassName(pstTagClass tagClass)
{
    static char const* const asnTags[]=
    {
        "UNIVERSAL", "APPLICATION", "PRIVATE",  "EMPTY"
    };
    if (((int)tagClass <= 0) || ((int)tagClass > (int)(sizeof(asnTags)/sizeof(*asnTags))))
        return NULL;
    return (char *)asnTags[(int)tagClass-1];
}
#endif


RVAPI char* RVCALLCONV /* null terminated token name */
pstGetTokenName(
      IN pstNodeType type)
{
    static char const* const asnTokens[]=
    {
        "INTEGER",
        "NULL",
        "BOOLEAN",
        "ENUMERATED",
        "OBJECT IDENTIFIER",
        "OCTET STRING",

        "BIT STRING",
        "GeneralString",
        "UniversalString",
        "BMPString",
        "IA5String",
        "VisibleString",
        "NumericString",
        "PrintableString",

        "CHOICE",
        "SEQUENCE",
        "SET",
        "OF",
        "SEQUENCE OF",
        "SET OF"
    };
    if (((int)type <= 0) || ((int)type > (int)(sizeof(asnTokens)/sizeof(*asnTokens))))
        return NULL;
    return (char *)asnTokens[(int)type-1];
}





/*_____________________________________child_______________________________________*/


static int /* child index or negative value on failure */
stGetField(
       /* get child by field id (offset) */
       IN  HPST hSyn,
       IN  int nodeId,
       IN  RvInt32 fieldId, /* offset of child name */
       OUT stChildExt **child
       )
{
    stNodeExt* elementBuffer;
    int childNodeId, index;

    if(hSyn==NULL || fieldId<0) return RV_ERROR_UNKNOWN;

    elementBuffer = (stNodeExt *)stGetNodeDataByNodeId(hSyn, nodeId);

    index = stGetChildByNodeAndFieldName(hSyn, nodeId, (int)m_numOfChilds(elementBuffer), fieldId, &childNodeId);
    if (index < 0) return index;

    if (child != NULL)
    {
        *child = (stNodeExt *)stGetNodeDataByNodeId(hSyn, childNodeId);
        if (*child == NULL)
            return RV_ERROR_UNKNOWN;
    }

    return index;
}






/*______________________________display_________________________*/

/************************************************************************
 * pstPrintNode
 * purpose: Print the information of a syntax tree node to a buffer
 * input  : hSyn    - Syntax tree handle
 *          nodeId  - Node to print
 *          buf     - Result buffer to use
 *          len     - Length of buffer
 * output : none
 * return : Number of characters printed
 ************************************************************************/
int pstPrintNode(
    IN  HPST            hSyn,
    IN  RvPstNodeId     nodeId,
    IN  char*           buf,
    IN  int             len)
{
    /* This function has been modified to deal with buffer overruns */
    stNodeExt*  node;
    RvChar*     ptr = buf;
    RvChar*     endBuf;
    RvChar*     fromString;
    RvChar*     tmpStr;
    pstNodeType nodeType;
    RvUint32    from, to;
    RvInt       a;

    /* Get the node itself */
    node = (stNodeExt *)stGetNodeDataByNodeId(hSyn, (int)nodeId);
    if ((node == NULL) || (len < 5)) return 0;

    endBuf = buf + len;

    /* 1. GENERAL TYPE */
    nodeType = (pstNodeType)(m_type(node));
    tmpStr = pstGetTokenName(nodeType);
    ptr += RvSnprintf(ptr, (RvSize_t)len, "%s ", nprn(tmpStr));

#if (RV_ASN1_CODER_USE_Q931 == RV_YES)
    /* 2. TAG */
    a = m_tagClass(node);
    tmpStr = pstGetTagClassName((pstTagClass)a);
    ptr += RvSnprintf(ptr, (RvSize_t)(endBuf - ptr), "[%s %d] ", nprn(tmpStr), m_tag(node));
#endif

    /* 3. FROM-TO */
    from = m_from(node);
    to = m_to(node);
    if ((from > 0) || (to > 0))
    {
        /* Check for negative boundaries */
        if (from <= to)
            ptr += RvSnprintf(ptr, (RvSize_t)(endBuf - ptr), "(%d..%d) ", from, to);
        else
            ptr += RvSnprintf(ptr, (RvSize_t)(endBuf - ptr), "(%u..%u)or(%d..%d) ", from, to, from, to);
    }

    /* 4. FROM CHARS */
    a = m_fromId(node);
    if (a >= 0)
    {
        fromString = stGetNameByNameId(hSyn, a, NULL);
        if (fromString[0])
            ptr += RvSnprintf(ptr, (RvSize_t)(endBuf - ptr), "FROM '%s' ", fromString);
    }

    /* 5. EXTENSION */
    if (m_isExtension(node))
    {
        strncpy(ptr, "... ", (RvSize_t)(endBuf - ptr));
        ptr += 4;
        if (endBuf <= ptr)
            return len;
    }

    /* 6. SEQUENCE OF recursion */
    switch (nodeType)
    {
        case pstOf:
        case pstSequenceOf:
        case pstSetOf:
            {
                stNodeExt*  ofNode;
                RvInt       ofId;

                ofId = m_ofId(node);
                ofNode = (stNodeExt *)stGetNodeDataByNodeId(hSyn, ofId);
                if (ofNode != NULL)
                {
                    int printedChars = (ptr - buf);
                    return printedChars + pstPrintNode(hSyn, (RvPstNodeId)ofId, ptr, len - printedChars);
                }
            }
            break;

        default:
            break;
    }

    return (ptr - buf);
}

/*______________________________tree_________________________*/


/************************************************************************
 * pstConstruct
 * purpose: Create a PST handle for a type in an ASN.1 definition.
 *          This function uses dynamic allocation when called.
 * input  : syntax      - Syntax buffer to use (the output of the ASN.1
 *                        compiler)
 *                        The buffer is the parameter returned by
 *                        cmEmGetCommonSyntax(), cmEmGetH245Syntax() and
 *                        cmEmGetQ931Syntax()
 *          rootName    - Root name of the type to create a PST handle.
 * output : none
 * return : PST handle on success
 *          NULL on failure
 * examples:
 *      hPst = pstConstruct(cmEmGetQ931Syntax(), "AliasAddress");
 *      hPst = pstConstruct(cmEmGetH245Syntax(), "Setup-UUIE.fastStart");
 ************************************************************************/
RVAPI HPST RVCALLCONV
pstConstruct(
    IN  const RvUint8*      syntax,
    IN  const RvChar*       rootName)
{
    synStruct *syn;
    char rootType[128];
    RvChar* path=NULL;
    int nameId;

    if (!syntax || !rootName) return NULL;

    if(RvMemoryAlloc(NULL, sizeof(synStruct), H32xLogGet(), (void**)&syn) != RV_OK)
        return NULL;
    memset(syn, 0, sizeof(synStruct));

#if defined(RV_PST_DEBUG)
    strncpy(syn->syntaxName, rootName, sizeof(syn->syntaxName));
#endif
    syn->syntax = (fmt2Struct *)syntax;

    /* handle internal root definition (path handling) */
    path = (RvChar*)strchr(rootName, '.');
    if (path != NULL)
    {
        strncpy(rootType, rootName, (RvSize_t)(path-rootName+1));
        rootType[path-rootName] = 0;
        path++;
    }
    else
    {
        strncpy(rootType, rootName, sizeof(rootType));
    }

    syn->rootNodeId = (RvPstNodeId)stGetNodeIdByName((HPST)syn, rootType);
    if (syn->rootNodeId >= 0)
        syn->rootNameId = stGetNameIdByNodeId((HPST)syn, (int)syn->rootNodeId);

    if (path != NULL)
    {
        syn->rootNodeId = (RvPstNodeId)pstGetNodeIdByPath((HPST)syn, path);
        if (syn->rootNodeId >= 0)
        {
            nameId = stGetNameIdByNodeId((HPST)syn, (int)syn->rootNodeId);
            syn->rootNameId = pstGetFieldId((HPST)syn, stGetNameByNameId((HPST)syn, nameId, NULL));
        }
    }

    if ((syn->rootNodeId < 0 ) || (syn->rootNameId < 0))
    {
        RvMemoryFree(syn, H32xLogGet());
        return NULL;
    }
    return (HPST)syn;
}


/************************************************************************
 * pstDestruct
 * purpose: Destruct a PST handle created by pstConstruct().
 * input  : hSyn    - PST handle to destruct
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV
pstDestruct(IN HPST hSyn)
{
    synStruct *syn = (synStruct *)hSyn;

    if (!syn) return RV_ERROR_UNKNOWN;
    RvMemoryFree(syn, H32xLogGet());
    return RV_TRUE;
}


RVAPI pstNodeType RVCALLCONV /* type of node */
pstGetNodeType(
           IN  HPST         hSyn,
           IN  RvPstNodeId  nodeId)
{
    stNodeExt* node;

    if (hSyn != NULL)
    {
        node = (stNodeExt *)stGetNodeDataByNodeId(hSyn, (int)nodeId);
        if (node != NULL)
            return (pstNodeType)m_type(node);
    }

    return (pstNodeType)RV_ERROR_UNKNOWN;
}


RVAPI RvPstNodeId RVCALLCONV /* type of node */
pstGetNodeOfId(
           IN  HPST         hSyn,
           IN  RvPstNodeId  nodeId)
{
    stNodeExt* node;

    node = (stNodeExt *)stGetNodeDataByNodeId(hSyn, (int)nodeId);
    if (node != NULL)
        return (RvPstNodeId)m_ofId(node);
    else
        return RV_PST_ERROR_UNKNOWN;
}

#if (RV_ASN1_CODER_USE_Q931 == RV_YES)
RVAPI int RVCALLCONV /*the tag of the node or negative value on failure*/
pstGetTag(
         IN  HPST           hSyn,
         IN  RvPstNodeId    nodeId,
         OUT pstTagClass*   tagClass)
{
    stNodeExt* node;

    node = (stNodeExt *)stGetNodeDataByNodeId(hSyn, (int)nodeId);
    if (node != NULL)
    {
        if (tagClass)
            *tagClass = (pstTagClass)m_tagClass(node);
        return m_tag(node);
    }
    else
        return RV_ERROR_UNKNOWN;
}
#endif /* (RV_ASN1_CODER_USE_Q931 == RV_YES) */


RVAPI int RVCALLCONV /*is not extended or negative value on failure*/
pstGetIsExtended(
     IN  HPST           hSyn,
     IN  RvPstNodeId    nodeId)
{
    stNodeExt* node;

    node = (stNodeExt *)stGetNodeDataByNodeId(hSyn, (int)nodeId);
    if (node != NULL)
        return m_isExtension(node);
    else
        return RV_ERROR_UNKNOWN;
}

#ifdef RV_H323_COMPILE_WITH_DEAD_FUNCTIONS
RVAPI int RVCALLCONV /*is not extended or negative value on failure*/
pstGetIsOpenType(
     IN  HPST           hSyn,
     IN  RvPstNodeId    nodeId)
{
    RV_UNUSED_ARG(hSyn);
    RV_UNUSED_ARG(nodeId);
    return RV_ERROR_NOTSUPPORTED;
}
#endif /* RV_H323_COMPILE_WITH_DEAD_FUNCTIONS */

RVAPI int RVCALLCONV /* type of node */
pstGetNodeRange(
        IN  HPST        hSyn,
        IN  RvPstNodeId nodeId,
        OUT int*        from,
        OUT int*        to)
{
    stNodeExt* node;

    node = (stNodeExt *)stGetNodeDataByNodeId(hSyn, (int)nodeId);
    if (node == NULL) return RV_ERROR_UNKNOWN;

    if (from)  *from = m_from(node);
    if (to)    *to   = m_to(node);

    return (m_to(node) == 0)? RV_ERROR_UNKNOWN : 0;
}


RVAPI RvInt32 RVCALLCONV /* Field enumeration or negative value on failure */
pstGetFieldId(
          /* convert field name to internal id */
          IN  HPST hSyn,
          IN  const char *fieldName /* null terminated string */
          )
{
    if (hSyn == NULL) return RV_ERROR_UNKNOWN;

    return stGetNameIdByName(hSyn, (char *)fieldName);
}

RVAPI int RVCALLCONV /* Real length of field name (excluding null) or negative value on failure */
pstGetFieldName(
        /* convert field internal id to field name */
        IN  HPST hSyn,
        IN  RvInt32 fieldId,
        IN  int fieldNameLength, /* num. of bytes in string allocation */
        OUT char* fieldName /* null terminated. user allocated */
        )
{
    char* string;
    int   length;

    if ((fieldName != NULL) && (fieldNameLength > 0))
        fieldName[0] = 0;

    if (fieldId < 0)
    {
        return RV_ERROR_UNKNOWN;
    }

    string = stGetNameByNameId(hSyn, fieldId, &length);

    /* Check if we've got a valid string to pass on */
    if (string == NULL) return RV_ERROR_UNKNOWN;

    /* Copy the result to the user's buffer */
    if ((fieldName != NULL) && (fieldNameLength > 0))
        strncpy(fieldName, string, (RvSize_t)fieldNameLength);

    return length;
}



/******************************************************************************
 * pstGetFROMString
 * ----------------------------------------------------------------------------
 * General: Get the string that defines the alphabet of the given string
 *          ASN.1 type.
 *
 * Return Value: The actual length of the FROM constrain string if successful.
 *               Negative error value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hSyn             - The PST handle.
 *         nodeId           - The PST node ID of the string ASN.1 type.
 *         fromStringLength - Length of the fromString parameter passed to this
 *                            function (in bytes).
 * Output: fromString       - FROM constrain string. If given as NULL, this
 *                            parameter will be ignored.
 *****************************************************************************/
RVAPI  int RVCALLCONV
pstGetFROMString(
   IN  HPST         hSyn,
   IN  RvPstNodeId  nodeId,
   IN  int          fromStringLength,
   OUT char*        fromString)
{
    char *string;
    int actualLength;

    if ((fromString != NULL) && (fromStringLength > 0))
        fromString[0] = 0;

    string = pstGetFROMStringPtr(hSyn, nodeId, &actualLength);
    if ((string == NULL) || (actualLength < 0))
        return RV_ERROR_UNKNOWN;

    if ((fromString != NULL) && (fromStringLength > 0))
        strncpy(fromString, string, (RvSize_t)fromStringLength);

    return actualLength;
}


RVAPI RvPstNodeId RVCALLCONV /* Internal node id or negative value on failure */
pstGetNodeIdByPath(
           /* get internal node id from specified node path.
              Path to node should start at root, and correspond to the
              ASN module syntax structure. */
           IN  HPST hSyn,
           IN  const char *path  /* format: "a.b.c" */
           )
{
    char name[RV_MAX_PATH];
    RvChar *ptr=NULL, *nameptr=name;
    RvInt32 fieldEnum;
    stChildExt *child = NULL;
    stNodeExt *sNode;
    RvPstNodeId sNodeId;

    if (!hSyn) return RV_PST_ERROR_UNKNOWN;
    if (!path) return pstGetRoot(hSyn);
    strncpy(name, path, RV_MAX_PATH);
    sNodeId = pstGetRoot(hSyn);

    for(;;)
    {
        ptr=(RvChar*) strchr(nameptr,'.');
        if (ptr != NULL) *ptr=0;

        if (nameptr[0] != '\0')
        {
            if (isNumber(nameptr))
            {   /* sequence of index */
                sNode = (stNodeExt *)stGetNodeDataByNodeId(hSyn, (int)sNodeId);
                sNodeId = (RvPstNodeId)m_ofId(sNode);
            }
            else
            {  /* other */
                if ( (fieldEnum=pstGetFieldId(hSyn, nameptr)) <0) return RV_PST_ERROR_UNKNOWN;
                if (stGetField(hSyn, (int)sNodeId, fieldEnum, &child) <0) return RV_PST_ERROR_UNKNOWN;
                sNodeId = (RvPstNodeId)m_structId(child);
            }
        }

        nameptr=ptr+1;
        if (ptr == NULL)
            break;
    }

    return sNodeId;
}

RVAPI int RVCALLCONV /* get root name */
pstGetRootName(
               IN  HPST  hSyn,
               IN  int   bufferLength,
               OUT char *buffer)
{
    char* string;
    int length;
    synStruct *syn = (synStruct *)hSyn;

    if (syn == NULL) return RV_ERROR_UNKNOWN;
    string = stGetNameByNameId(hSyn, syn->rootNameId, &length);

    if (string == NULL)
    {
        if ((buffer != NULL) && (bufferLength > 0)) buffer[0] = 0;
        return RV_ERROR_UNKNOWN;
    }

    if ((buffer != NULL) && (bufferLength > 0))
        strncpy(buffer, string, (RvSize_t)bufferLength);

    return length;
}

RVAPI int RVCALLCONV
pstGetNumberOfChildren(
        IN HPST         hSyn,
        IN RvPstNodeId  nodeId)
{
    stNodeExt* node;

    node = (stNodeExt *)stGetNodeDataByNodeId(hSyn, (int)nodeId);
    if (node != NULL)
        return m_numOfChilds(node);
    else
        return RV_ERROR_UNKNOWN;
}


RVAPI RvPstNodeId RVCALLCONV
pstGetChild(
            IN  HPST        hSyn,
            IN  RvPstNodeId nodeId,      /* node id of parent */
            IN  int         childIndex,  /* index of child */
            OUT pstChild*   child)
{
    stChildExt* fieldInfo;
    RvUint32* err;

    if (child)
        child->index = RV_ERROR_UNKNOWN;  /* assume the worst */

    /* Get the information */
    err = stGetChildByIndex(hSyn, (int)nodeId, childIndex, &fieldInfo);
    if (err == NULL)
        return RV_PST_ERROR_UNKNOWN;

    /* Convert it to something readable */
    if (child)
    {
        child->index      = childIndex;
        child->nodeId     = (RvPstNodeId)m_structId(fieldInfo);
        child->fieldId    = m_fieldId(fieldInfo);
        child->isOptional = m_isOptional(fieldInfo);
    }

    /* We have to return back the nodeId we got on this one */
    return child->nodeId;
}


RVAPI RvBool RVCALLCONV
pstIsStringNode(
       IN  HPST         hSyn,
       IN  RvPstNodeId  nodeId)
{
    pstNodeType type = pstGetNodeType(hSyn, nodeId);
    if (((int)type) < 0)
        return RV_FALSE;

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




#ifdef __cplusplus
}
#endif



