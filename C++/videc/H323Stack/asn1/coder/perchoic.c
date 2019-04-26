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
  perChoice.c

  ____________________________________________________________________________
  ___________________________CHOICE___________________________________________
  ____________________________________________________________________________


  format:

  +----------+-------+-------+
  | ext. bit | index | field |
  +----------+-------+-------+

  */


#include "rvstdio.h"
#include "perintr.h"
#include "perchoic.h"


#ifdef __cplusplus
extern "C" {
#endif




/*
  Desc: Encode a choice node.
  According to clause 22.
  Node value contains the choice: fieldId (enumeration).

*/
int
perEncodeChoice(IN  HPER hPer,
        IN  int synParent,
        IN  int valParent, /* this is me */
        IN  RvInt32 fieldId)
{
#if (RV_ASN1_CODER_USE_H450 == RV_YES)
    int speciality;
#endif
    perStruct *per = (perStruct *)hPer;
    pstChildExt synChild;
    RvPstFieldId choiceFieldId=-1;
    RvInt32 offset=-1;
    int vtPath;
    int childNodeId=-1;
    int extChild;
    int index; /* child index under syntax parent */
    int from,to;
    int hasExtensions;

    RV_UNUSED_ARG(fieldId); /* Needed when compiling without logs */

    pstGetNodeRange(per->hSyn,synParent,&from,&to);

    /* -- get choice value */
    if ( (childNodeId = pvtChild(per->hVal, valParent)) <0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeChoice: Value node not exist. [%s]",
            pstGetFieldNamePtr(per->hSyn, fieldId)));
        return RV_ERROR_UNKNOWN;
    }
    pvtGet(per->hVal, childNodeId, &choiceFieldId, NULL, NULL, NULL);

    /* -- get syntax child */
    if ((index=pstGetFieldExt(per->hSyn, synParent, choiceFieldId, &synChild)) <0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeChoice: Syntax choice node not exist. [<%d>%s]",
            (int)choiceFieldId, pstGetFieldNamePtr(per->hSyn, choiceFieldId)));
        return RV_ERROR_UNKNOWN;
    }

    /* -- extension bit 22.5 */
    hasExtensions = pstGetIsExtended(per->hSyn,synParent);
    if (hasExtensions)
    {
        /* node has extension mark */
        /* We should put the extension mark with the right value */
        extChild = pstGetFirstExtendedChild(per->hSyn, synParent);

        perEncodeBool(index >= extChild, per->hBB);
    }
    else
    {
        /* No extensions - " simulate" first extended child by the number of nodes */
        extChild = pstGetNumberOfChildren(per->hSyn, synParent) + 1;
    }

    /* -- index */
    if ( (hasExtensions == RV_FALSE) ||  /* 22.6 */
        (index < extChild) )   /* 22.7 */
    {
        if (perEncodeInt((RvUint32)(index-1), 0, (RvUint32)((extChild<0)?(pstGetNumberOfChildren(per->hSyn,synParent)-1):(extChild-2)),
            RV_FALSE, RV_FALSE, RV_FALSE, per->hBB)< 0)
            return RV_ERROR_UNKNOWN;
    }
    else
    { /* 22.8 */
        if (extChild<0) /* report WARNING */
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perEncodeChoice: No extension fields for this node [%s].",
                pstGetFieldNamePtr(per->hSyn, fieldId)));
        }
        if (perEncodeNormallySmallInt((RvUint32)(index-extChild), per->hBB)<0)
            return RV_ERROR_UNKNOWN;
    }

    /* -- field encoding */
    if ((vtPath=pvtGetChild(per->hVal, valParent, choiceFieldId, NULL)) <0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeChoice: Value choice node not exist. [%s]",
            pstGetFieldNamePtr(per->hSyn, choiceFieldId)));
        return RV_ERROR_UNKNOWN;
    }
#if (RV_ASN1_CODER_USE_H450 == RV_YES)
    speciality = pstChildIsSpecial(per->hSyn,synParent, index);
#endif

    if (pstChildIsExtended(per->hSyn, synParent, index) == RV_FALSE)
    {
        int childSynNodeId;
        RvBool wasTypeResolvedInRunTime = RV_FALSE;
#if (RV_ASN1_CODER_USE_H450 == RV_YES)
        if ((speciality == (int)pstDependingDependent) || (speciality == (int)pstDependent))
        {
            pvtGet(per->hVal, vtPath, NULL, &childSynNodeId, NULL, NULL);
            wasTypeResolvedInRunTime = RV_TRUE;
        }
        else
#endif
        {
            childSynNodeId = synChild.nodeId;
            wasTypeResolvedInRunTime = RV_FALSE;
        }

        if (perEncNode(hPer, childSynNodeId, vtPath, synChild.fieldId, wasTypeResolvedInRunTime)<0)
            return RV_ERROR_UNKNOWN;
    }
    else
    {  /* open type encoding */
        RvPstNodeId childSynNodeId;
        RvBool wasTypeResolvedInRunTime;

		if (perEncodeOpenTypeBegin(hPer, synChild.fieldId, &offset) < 0)
			return RV_ERROR_UNKNOWN;

#if (RV_ASN1_CODER_USE_H450 == RV_YES)
        if ((speciality == (int)pstDependingDependent) || (speciality == (int)pstDependent))
        {
            pvtGet(per->hVal, vtPath, NULL, &childSynNodeId, NULL, NULL);
            wasTypeResolvedInRunTime = RV_TRUE;
        }
        else
#endif
		{
            childSynNodeId = synChild.nodeId;
			wasTypeResolvedInRunTime = RV_FALSE;
		}

		/* do the actual encoding */
        if (perEncNode(hPer, childSynNodeId, vtPath, synChild.fieldId, wasTypeResolvedInRunTime) < 0)
            return RV_ERROR_UNKNOWN;

		/* finalize the encoding */
        if (perEncodeOpenTypeEnd(hPer, offset, synChild.fieldId) < 0)
            return RV_ERROR_UNKNOWN;
    }

    return RV_TRUE;
}



/*
  Desc: Decode a choice node.
  According to clause 22.
  Node value contains the choice: fieldId (enumeration).
  */
int
perDecodeChoice(IN  HPER hPer,
        IN  int synParent, /* parent in syntax tree */
        IN  int valParent, /* field parent in value tree */
        IN  RvInt32 fieldId)   /* enum of current field */
{
    perStruct *per = (perStruct *)hPer;
    pstChildExt synChild;

    RvBool isExtended = RV_FALSE; /* true if extensions exist */
    RvBool skipChoiceValue = RV_FALSE; /* true if we want to skip the decoding of the choice's value */

    RvUint32 index=0;
    RvUint32 children=0;
    RvUint32 dec = 0; /* decoded bits */
    RvUint32 fieldLen=0;
    int vtPath=-1;
    RvUint32 from = per->decodingPosition; /* internal position */
    int extChild;
    RvBool extended = (RvBool)pstGetIsExtended(per->hSyn, synParent);
    RvUint32* rv;

    /* -- extension bit 22.5 */
    if (extended == RV_TRUE)
    { /* node has extension mark */
        if (perDecodeBool(&isExtended, hPer, from, &dec) <0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeChoice: extension bit missing [%s].",
                pstGetFieldNamePtr(per->hSyn, fieldId)));
            return RV_ERROR_UNKNOWN;
        }
        else from+=dec; /* advance decoding position */
    }

    children = (RvUint32)pstGetNumberOfChildren(per->hSyn, synParent);
    if (extended)
    {
        extChild = pstGetFirstExtendedChild(per->hSyn, synParent);
#if (RV_CHECK_MASK & RV_CHECK_OTHER)
        if (extChild < 0)
        { /* report an exception */
            RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeChoice: No extension fields for this node [%s].",
                pstGetFieldNamePtr(per->hSyn, fieldId)));
            return RV_ERROR_UNKNOWN;
        }
#endif
    }
    else
        extChild = children + 1;

    /* -- index */
    if (extended == RV_FALSE || /* 22.6 */ isExtended == RV_FALSE /* 22.7 */)
    {
        if (perDecodeInt(&index, 0, (extChild < 0) ? (RvUint32)(children-1) : (RvUint32)(extChild-2),
                RV_FALSE, RV_FALSE, RV_FALSE, hPer, from, &dec, "choice") <0)
            return RV_ERROR_UNKNOWN;
    }
    else
    { /* 22.8 */ /* Extended child */
        if (perDecodeNormallySmallInt(&index, hPer, from, &dec) <0)
            return RV_ERROR_UNKNOWN;
        index += extChild - 1;
    }

    from += dec;
    per->decodingPosition = from;

    /* -- field decoding */
    if (index + 1 > children)
    {
        if (isExtended == RV_FALSE)
        {
            /* We got a child who's out of range */
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeChoice: field index does not exist %s [%d].",
                pstGetFieldNamePtr(per->hSyn, fieldId), index+1));
            return RV_ERROR_UNKNOWN;
        }
        else
        {
            /* We have an out of range field, but this choice has "..." in it */
            RvLogWarning(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeChoice: field index extension not within range %s [%d].",
                pstGetFieldNamePtr(per->hSyn, fieldId), index+1));
            skipChoiceValue = RV_TRUE;
        }

        /* The memset is here to remove a compilation warning */
        memset(&synChild, 0, sizeof(synChild));

        /* Add the choice's root even if we won't be using it. */
        vtPath = pvtAdd(per->hVal, valParent, fieldId, -111, NULL, NULL);
    }
    else
    {
        /* -- add choice value node */
        rv = pstGetChildExt(per->hSyn, synParent, (int)(index+1), &synChild);
#if (RV_CHECK_MASK & RV_CHECK_OTHER)
        if (rv == NULL)
        {
            RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeChoice: Error getting child information for %s [%d].",
                pstGetFieldNamePtr(per->hSyn, fieldId), index+1));
            return RV_ERROR_UNKNOWN;
        }
#endif

        vtPath=valParent;
        if (fieldId!=RV_ERROR_UNKNOWN)
            vtPath = pvtAdd(per->hVal, valParent, fieldId, synChild.fieldId, NULL, NULL);
    }

    if (isExtended == RV_FALSE)
    { /* normal decoding */
        if (perDecNode(hPer, &synChild, vtPath) <0)
            return RV_ERROR_UNKNOWN;
    }
    else
    {  /* extended child decoding: open type */
        if (perDecodeLen(perLenTypeUNCONSTRAINED, &fieldLen, 0, 0, hPer, from, &dec) <0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeChoice: extended field length missing [%s].",
                pstGetFieldNamePtr(per->hSyn, fieldId)));
            return RV_ERROR_UNKNOWN;
        }
        from += dec;
        per->decodingPosition = from;

        if (skipChoiceValue == RV_FALSE)
        {
            /* valid child */
            if (perDecNode(hPer, &synChild, vtPath) < 0)
                return RV_ERROR_UNKNOWN;
        }

        per->decodingPosition = from +fieldLen*8; /* skip field */
    }

    return vtPath;
}


#ifdef __cplusplus
}
#endif



