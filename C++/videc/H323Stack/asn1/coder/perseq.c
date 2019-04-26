#ifdef __cplusplus
extern "C" {
#endif



/*
***********************************************************************************

NOTICE:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

***********************************************************************************
*/

/*
  perSequence.c

  Extension encoding:
  -------------------
   - encode field.
   - complete encoding (padding).
   - open type encoding (length prefix)

   ------------------------
   | length | field | pad |
   ------------------------
   | 1/2oc. |   n octets  |
   ------------------------


  Ron S.
  14 May 1996

  ____________________________________________________________________________
  ___________________________SEQUENCE_________________________________________
  ____________________________________________________________________________


  Format:

  +----------+-------------------+---------+     +---------+
  | ext. bit | n bits (optional) | field-1 | ... | field-n |
  +----------+-------------------+---------+     +---------+

  Extensions:

  +------------+---------------------+----------+       +----------+
  | length (n) | n bits (Extensions) | Efield-1 |  ...  | Efield-n |
  +------------+---------------------+----------+       +----------+

  Efield-i: open type complete encoding.

  +-----+--------+----------+-----+
  | pad | length | encoding | pad |
  +-----+--------+----------+-----+


  */

#include "rvstdio.h"
#include "perintr.h"
#include "perseq.h"





/*
  Return: RV_TRUE if there are any extended fields in the value tree of the
          given sequence.
  */
static RvBool perAnyExtendedFields(IN  HPER hPer,
              IN  int synParent,
              IN  int valParent)
{
    perStruct *per = (perStruct *)hPer;
    pstChildExt synChild;
    RvPvtNodeId curChild;
    RvPstFieldId curFieldId = (RvPstFieldId)-1;
    int i;
    int children;
    RvUint32* iter;

    children = pstGetNumberOfChildren(per->hSyn,synParent);

    curChild = pvtChild(per->hVal, valParent);
    pvtGet(per->hVal, curChild, &curFieldId, NULL, NULL, NULL);
    iter = pstGetChildExt(per->hSyn, synParent, 1, &synChild);
    for (i = 1; i <= children; i++)
    {
        if (synChild.isExtended && (synChild.fieldId == curFieldId))
        {
            /* Found an existing extended field in the value tree */
            return RV_TRUE;
        }

        /* Get next field on value tree if the current one was exhausted */
        if (synChild.fieldId == curFieldId)
        {
            curChild = pvtBrother(per->hVal, curChild);
            pvtGet(per->hVal, curChild, &curFieldId, NULL, NULL, NULL);
        }

        iter = pstGetBrotherExt(per->hSyn, i+1, iter, &synChild);
    }

    return RV_FALSE;
}


/*
  Desc: Encode the extended fields in a sequence type.

  -- length: number of extended fields.
  -- bit field: bit for each extended field. true if exist.
  -- list of extended fields, each as open type.
  */
static int perEncodeSequenceExt(
         IN  HPER hPer,
         IN  int synParent,
         IN  int valParent,
         IN  RvInt32 fieldId)
{
    perStruct *per = (perStruct *)hPer;
    pstChildExt synChild;
    int i, numOfExtendedChilds=0;
    RvInt32 offset=-1;
    int children;
    RvUint32 * iter;
    RvPvtNodeId curChild;
    RvPvtNodeId firstChild = RV_PVT_INVALID_NODEID;
    RvPstFieldId curFieldId;

    RV_UNUSED_ARG(fieldId); /* Needed when compiling without logs */

    /* -- checks */
    if (!per)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeSequenceExt: %s: Invalid parameters.",
            pstGetFieldNamePtr(per->hSyn, fieldId)));
        return RV_ERROR_UNKNOWN;
    }

    children = pstGetNumberOfChildren(per->hSyn, synParent);

    /* -- length: 18.8: normally small length */
    iter = pstGetChildExt(per->hSyn, synParent, 1, &synChild);
    for (numOfExtendedChilds=0, i=1; i<=children; i++)
    {
        if (synChild.isExtended) numOfExtendedChilds++;
        iter = pstGetBrotherExt(per->hSyn, i+1, iter, &synChild);
    }
    if (perEncodeLen(perLenTypeNORMALLY_SMALL, (RvUint32)numOfExtendedChilds, 0, 0, per->hBB) < 0)
        return RV_ERROR_UNKNOWN;

    /* -- bit field:18.7 */
    curFieldId = (RvPstFieldId)-1;
    curChild = pvtChild(per->hVal, valParent);
    pvtGet(per->hVal, curChild, &curFieldId, NULL, NULL, NULL);
    iter = pstGetChildExt(per->hSyn, synParent, 1, &synChild);
    for (i = 1; i <= children; i++)
    {
        if (synChild.isExtended)
        {
            /* extended */
            perEncodeBool((synChild.fieldId == curFieldId) /* exist */, per->hBB);

            /* See if it's the first extended child that we're handling */
            if ((firstChild == RV_PVT_INVALID_NODEID) && RV_PVT_NODEID_IS_VALID(curChild))
                firstChild = curChild;
        }

        /* Get next field on value tree if we exhausted the current node */
        if (synChild.fieldId == curFieldId)
        {
            curChild = pvtBrother(per->hVal, curChild);
            pvtGet(per->hVal, curChild, &curFieldId, NULL, NULL, NULL);
        }

        iter = pstGetBrotherExt(per->hSyn, i+1, iter, &synChild);
    }
    if (curChild >= 0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeSequenceExt: Child not encoded %d.", curChild));
        return RV_ERROR_UNKNOWN;
    }

    /* -- field encoding: 18.9 */
    curFieldId = (RvPstFieldId)-1;
    curChild = firstChild;
    pvtGet(per->hVal, curChild, &curFieldId, NULL, NULL, NULL);
    iter = pstGetChildExt(per->hSyn, synParent, 1, &synChild);
    for (i = 1; i <= children; i++)
    {
        if (synChild.isExtended)
        { /* extended */
            /* The assumption is that nodes in the PVT tree are sorted by their position
               inside a SEQUENCE or a SET. This allows us to loop at the same time on both
               the syntax tree and the value tree */
            if (curFieldId == synChild.fieldId)
            {
                /* exist */
                RvPstNodeId childSynNodeId;
#if (RV_ASN1_CODER_USE_H450 == RV_YES)
                pstFieldSpeciality speciality = synChild.speciality;
#endif
                RvBool wasTypeResolvedInRunTime;

		        if (perEncodeOpenTypeBegin(hPer, synChild.fieldId, &offset) < 0)
			        return RV_ERROR_UNKNOWN;

#if (RV_ASN1_CODER_USE_H450 == RV_YES)
                if ((speciality == (int)pstDependingDependent) || (speciality == (int)pstDependent))
                {
                    pvtGet(per->hVal, curChild, NULL, &childSynNodeId, NULL, NULL);
                    wasTypeResolvedInRunTime = RV_TRUE;
                }
                else
#endif
		        {
                    childSynNodeId = synChild.nodeId;
			        wasTypeResolvedInRunTime = RV_FALSE;
		        }

		        /* do the actual encoding */
                if (perEncNode(hPer, childSynNodeId, curChild, synChild.fieldId, wasTypeResolvedInRunTime) < 0)
                    return RV_ERROR_UNKNOWN;

		        /* finalize the encoding */
                if (perEncodeOpenTypeEnd(hPer, offset, synChild.fieldId) < 0)
                    return RV_ERROR_UNKNOWN;

                /* Prepare for the next field, since this one got used at this point... */
                curChild = pvtBrother(per->hVal, curChild);
                pvtGet(per->hVal, curChild, &curFieldId, NULL, NULL, NULL);
            }
            else
            { /* should be optional */
                if ((synChild.isOptional == RV_FALSE) && (synChild.isDefault == RV_FALSE) && (per->isTolerant == RV_FALSE))
                { /* but is not */
                    RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                        "perEncodeSequenceExt: non optional Child not found [%d]%s->%s.",
                        valParent, pstGetFieldNamePtr(per->hSyn, fieldId),
                        pstGetFieldNamePtr(per->hSyn, synChild.fieldId)));
                    return RV_ERROR_UNKNOWN;
                }
            }
        }
        iter = pstGetBrotherExt(per->hSyn, i+1, iter, &synChild);
    }
    return RV_TRUE;
}



/*
  Desc: Encode a sequence node.
  According to clause 18.
  Node value has no meaning.
*/
int
perEncodeSequece(IN  HPER hPer,
         IN  int synParent,
         IN  int valParent,
         IN  RvInt32 fieldId)
{
    perStruct *per = (perStruct *)hPer;
    pstChildExt synChild;
    int i;
    int children;
    RvUint32 * iter;
    RvPvtNodeId curChild;
    RvPstFieldId curFieldId;
    RvBool anyExtFields = RV_FALSE;

    /* -- checks */
    if (!per)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeSequence: Invalid parameters. [%s]",
            pstGetFieldNamePtr(per->hSyn, fieldId)));
        return RV_ERROR_UNKNOWN;
    }

    /* -- extension bit 18.1 */
    if (pstGetIsExtended(per->hSyn,synParent) == RV_TRUE)
    { /* node has extension mark */
        anyExtFields = perAnyExtendedFields(hPer, synParent, valParent);
        perEncodeBool(anyExtFields, per->hBB);  /* has extended childs */
    }
    children = pstGetNumberOfChildren(per->hSyn, synParent);

    /* -- preemble 18.2-18.3: bit for each optional child */
    curFieldId = (RvPstFieldId)-1;
    curChild = pvtChild(per->hVal, valParent);
    pvtGet(per->hVal, curChild, &curFieldId, NULL, NULL, NULL);
    iter = pstGetChildExt(per->hSyn, synParent, 1, &synChild);
    for (i = 1; (i <= children) && (synChild.isExtended == RV_FALSE); i++)
    {
        if ((synChild.isOptional == RV_TRUE) || (synChild.isDefault == RV_TRUE)) /* optional child */
        {
            perEncodeBool((synChild.fieldId == curFieldId), per->hBB);/* exists */
        }

        /* get next field in value tree if we used up this one */
        if (synChild.fieldId == curFieldId)
        {
            curFieldId = (RvPstFieldId)-1;
            curChild = pvtBrother(per->hVal, curChild);
            pvtGet(per->hVal, curChild, &curFieldId, NULL, NULL, NULL);
        }

        iter = pstGetBrotherExt(per->hSyn, i+1, iter, &synChild);
    }

    /* -- preemble 18.4-18.6: encode fields */
    curFieldId = (RvPstFieldId)-1;
    curChild = pvtChild(per->hVal, valParent);
    pvtGet(per->hVal, curChild, &curFieldId, NULL, NULL, NULL);
    iter = pstGetChildExt(per->hSyn, synParent, 1, &synChild);
    for (i = 1; (i <= children) && (synChild.isExtended == RV_FALSE); i++)
    {
        if (synChild.fieldId == curFieldId)
        {
            /* exists */
            RvPstNodeId childSynNodeId;
            RvBool wasTypeResolvedInRunTime = RV_FALSE;

#if (RV_ASN1_CODER_USE_H450 == RV_YES)
            if ((synChild.speciality == pstDependingDependent) || (synChild.speciality == pstDependent))
            {
                pvtGet(per->hVal, curChild, NULL, &childSynNodeId, NULL, NULL);
                wasTypeResolvedInRunTime = RV_TRUE;
            }
            else
#endif
                childSynNodeId = synChild.nodeId;

            if (perEncNode(hPer, childSynNodeId, curChild, synChild.fieldId, wasTypeResolvedInRunTime) < 0)
                return RV_ERROR_UNKNOWN;

            /* Value node was used - get the next one from value tree */
            curFieldId = (RvPstFieldId)-1;
            curChild = pvtBrother(per->hVal, curChild);
            pvtGet(per->hVal, curChild, &curFieldId, NULL, NULL, NULL);
        }
        else
        {
            if ((synChild.isOptional == RV_FALSE) && (synChild.isDefault == RV_FALSE))
            { /* non-optional not found */
                RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                    "perEncodeSequence: Child not found %d->%s.",
                    valParent, pstGetFieldNamePtr(per->hSyn, synChild.fieldId)));
                return RV_ERROR_UNKNOWN;
            }
        }

        iter = pstGetBrotherExt(per->hSyn, i+1, iter, &synChild);
    }

    /* -- Extensions 18.7-18.9 */
    if (anyExtFields)
    {
        if (perEncodeSequenceExt(hPer, synParent, valParent, fieldId) < 0)
            return RV_ERROR_UNKNOWN;
    }
    else
    {
        if (curChild >= 0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perEncodeSequence: Child not encoded %d.", curChild));
            return RV_ERROR_UNKNOWN;
        }
    }

    return RV_TRUE;
}


/*_____________________________________decoding.........................................*/


/*
  Desc: Decode a sequence node.
  According to clause 18.
  Node value has no meaning.

*/
int
perDecodeSequece(IN  HPER hPer,
         IN  int synParent, /* parent in syntax tree */
         IN  int valParent, /* field parent in value tree */
         IN  RvInt32 fieldId)   /* enum of current field */
{
    perStruct *per = (perStruct *)hPer;
    pstChildExt synChild;

    RvBool isExtended=RV_FALSE; /* true if extensions exist */
    RvBool optionalExist=RV_FALSE;
    RvBool isExist=RV_FALSE;

    RvUint32 dec = 0; /* decoded bits */
    int i, vtPath, numOfOptionalChilds=0, extChild=-1;
    RvUint32 from = per->decodingPosition; /* internal position */
    RvInt32 saveLocation=-1;
    RvUint32 extLen=0, fieldLen=0;
    int children;
    RvUint32* iter;

    /* -- checks */
    if (!per)
    {
        RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource, "perDecodeSequece: Invalid parameters."));
        return RV_ERROR_UNKNOWN;
    }

    /* -- add sequence value node */
    vtPath=valParent;
    if (fieldId!=RV_ERROR_UNKNOWN)
    vtPath = pvtAdd(per->hVal, valParent, fieldId, -111, NULL, NULL);

    /* -- first bit 18.1 */
    if (pstGetIsExtended(per->hSyn,synParent) == RV_TRUE)
    { /* node has extension mark */
        if (perDecodeBool(&isExtended, hPer, from, &dec) <0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeSequece: extension bit missing [%s].",
                pstGetFieldNamePtr(per->hSyn, fieldId)));
            return RV_ERROR_UNKNOWN;
        }
        else from+=dec; /* advance decoding position */
    }

    /* -- preemble 18.2-18.6: bit for each optional child */
    numOfOptionalChilds = pstGetNumberOfOptionalBeforeExtension(per->hSyn, synParent);
    per->decodingPosition = from+numOfOptionalChilds; /* advance to end of bit field */
    children= pstGetNumberOfChildren(per->hSyn,synParent);

    iter = pstGetChildExt(per->hSyn, synParent, 1, &synChild);
    for (i=1; i<=children; i++)
    {
        if (synChild.isExtended == RV_FALSE)
        { /* not extended */
            if ((synChild.isOptional == RV_TRUE) || (synChild.isDefault == RV_TRUE))
            { /* optional child */
                perDecodeBool(&optionalExist, hPer, from++, &dec);
                if (optionalExist)
                {
                    if (perDecNode(hPer, &synChild, vtPath) < 0)
                        return RV_ERROR_UNKNOWN;
                }
            }
            else
            { /* not optional */
                if (perDecNode(hPer, &synChild, vtPath) < 0)
                    return RV_ERROR_UNKNOWN;
            }
        }
        iter = pstGetBrotherExt(per->hSyn, i+1, iter, &synChild);
    } /* for */

    /* -- extensions prefix: 18.7-18.8 */
    if (!isExtended) return vtPath; /* no extensions */
    from = per->decodingPosition;

    /* -- length */
    if (perDecodeLen(perLenTypeNORMALLY_SMALL, &extLen, 0, 0, hPer, from, &dec) <0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeSequece: extension length missing [%s].",
            pstGetFieldNamePtr(per->hSyn, fieldId)));
        return RV_ERROR_UNKNOWN;
    }
    else from+=dec;

    /* -- extension fields */
    per->decodingPosition = from+extLen; /* skip bit field */

    extChild = pstGetFirstExtendedChild(per->hSyn, synParent);
    if (extChild<0) /* report WARNING */
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeSequece: NO extensions for this version [%s].",
            pstGetFieldNamePtr(per->hSyn, fieldId)));
    }

    /* -- decode extension fields: open type */
    iter = pstGetChildExt(per->hSyn, synParent, extChild, &synChild);
    for (i=0; i<(int)extLen; i++)
    {
        /* First see if the field exists or not */
        if (perDecodeBool(&isExist, hPer, from+i, &dec) <0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeSequece: extended field existence boolean erroneous [%s/%d].",
                pstGetFieldNamePtr(per->hSyn, fieldId), i));
            return RV_ERROR_UNKNOWN;
        }
        if (isExist)
        {  /* extended child exist */
            if (perDecodeLen(perLenTypeUNCONSTRAINED, &fieldLen, 0, 0, hPer,per->decodingPosition, &dec) <0)
            {
                RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                    "perDecodeSequece: extended field length missing [%s/%d].",
                    pstGetFieldNamePtr(per->hSyn, fieldId), i));
                return RV_ERROR_UNKNOWN;
            }

            per->decodingPosition+=dec;
            saveLocation = per->decodingPosition;

            if (extChild >0 && extChild+i <= children)
            {  /* valid extended child */
                if (perDecNode(hPer, &synChild, vtPath) < 0)
                    return RV_ERROR_UNKNOWN;
            }
            else
            {
                RvLogWarning(&rvPerErrLogSource, (&rvPerErrLogSource,
                    "perDecodeSequece: New extension (#%d) under [%s].",
                    i, pstGetFieldNamePtr(per->hSyn, fieldId)));
            }

            per->decodingPosition = saveLocation + fieldLen*8; /* skip field */
        }

        if (iter != NULL)
        {
            /* We use the iterator only for fields we know in our version of this ASN.1 definition */
            iter = pstGetBrotherExt(per->hSyn, extChild+i+1, iter, &synChild);
        }
    }
    return vtPath;
}

#ifdef __cplusplus
}
#endif



