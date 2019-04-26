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


#include "rvstdio.h"
#include "intutils.h"
#include "psyntree.h"
#include "cmConf.h"
#include "h245.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


int /* RV_TRUE or negative value on failure */
confGetDataTypeName(
            /* Generate dataName using field name as in H.245 standard. */
            IN  HPVT hVal,
            IN  int dataTypeId, /* Data type node id */
            IN  int dataNameSize,
            OUT char *dataName, /* copied into user allocate space [128 chars] */
            OUT confDataType* type, /* of data */
            OUT RvInt32* typeId /* node id of media type */
            )
{
    HPST synConf = pvtGetSynTree(hVal, dataTypeId);
    int dataId = pvtChild(hVal, dataTypeId);
    int choice=-1;
    RvPstFieldId fieldId=-1;
    char* dataNamePtr = NULL;

    if( (synConf == NULL) || (dataId < 0) )
        return RV_ERROR_UNKNOWN;

    pvtGet(hVal, dataId, &fieldId, NULL, NULL, NULL);
    switch(fieldId)
    {
    case __h245(nonStandard):
        choice = (int)confNonStandard;
        break;
    case __h245(nullData):
        choice = (int)confNullData;
        break;
    case __h245(videoData):
        choice = (int)confVideoData;
        dataId = pvtChild(hVal, dataId);
        break;
    case __h245(audioData):
        choice = (int)confAudioData;
        dataId = pvtChild(hVal, dataId);
        break;
    case __h245(data):
        choice = (int)confData;
        dataId = pvtChild(hVal, pvtChild(hVal, dataId));
        break;

#if (RV_H245_LEAN_H223 == RV_NO)
    case __h245(encryptionData):
        choice = (int)confEncryptionData;
        dataNamePtr = (char*)"encryptionData: Not supported";
        break;
#endif

    case __h245(h235Control):
        choice = (int)confH235Control;
        dataNamePtr = (char*)"h235Control: Not supported";
        break;
        
#if (RV_H245_LEAN_H223 == RV_NO)
    case __h245(h235Media):
        choice = (int)confH235Media;
        dataNamePtr = (char*)"h235Media: Not supported";
        break;
#endif        
    case __h245(multiplexedStream):
        choice = (int)confMultiplexedStream;
        dataNamePtr = (char*)"multiplexedStream: Not supported";
        break;

#if (RV_H245_LEAN_H223 == RV_NO)
    case __h245(redundancyEncoding):
        choice = (int)confRedundancyEncoding;
        dataNamePtr = (char*)"redundancyEncoding: Not supported";
        break;
        
    case __h245(multiplePayloadStream):
        choice = (int)confMultiplePayloadStream;
        dataNamePtr = (char*)"multiplePayloadStream: Not supported";
        break;
        
    case __h245(fec):
        choice = (int)confFec;
        dataNamePtr = (char*)"fec: Not supported";
        break;
#endif
    default:
        break;
    }

    if (dataName != NULL)
    {
        if (dataNamePtr == NULL)
        {
            /* Found a supported data type */
            pvtGet(hVal, dataId, &fieldId, NULL, NULL, NULL);
            pstGetFieldName(synConf, fieldId, dataNameSize, dataName);
        }
        else
        {
            /* An unsupported one... */
            strncpy(dataName, dataNamePtr, (RvSize_t)dataNameSize);
        }
    }

    if (type) *type = (confDataType)choice;
    if (typeId) *typeId = dataId;

    return RV_TRUE;
}



int
confGetDataType(
        /* Search channel name in channels conf. and create appropriate dataType tree */
        IN  HPVT hVal,
        IN  int confRootId,
        IN  const RvChar* channelName, /* in channels conf */
        OUT int dataTypeId, /* node id: user supplied */
        OUT RvBool* isDynamicPayload, /* true if dynamic */
        RvBool nonH235 /*If true means remove h235Media.mediaType level */
        )
{
    int pathId, dataId,ret;
    HPST synConf = pvtGetSynTree(hVal, confRootId);

#if (RV_H245_LEAN_H223 == RV_YES)
    RV_UNUSED_ARG(nonH235);
#endif

    pathId = pvtAddRoot(hVal, synConf, 0, NULL);
    if (pathId<0)
        return pathId;
    __pvtBuildByFieldIds(dataId, hVal, pathId,
                    {_h245(channels) _nul(1) _h245(name) LAST_TOKEN},
                    (RvInt32)strlen(channelName), channelName);

    if (pvtSearchPath(hVal, confRootId, hVal, pathId, RV_TRUE) == RV_TRUE)
    {
        /* found entry */
        int channelTableId = pvtParent(hVal, pvtParent(hVal, dataId));
        RvInt32 index;
        int tableEntryId, dtId;
        int confChanTableId;

        pvtGet(hVal, channelTableId, NULL, NULL, &index, NULL);
        confChanTableId = pvtGetChild(hVal, confRootId, __h245(channels), NULL);
        tableEntryId = pvtGetByIndex(hVal, confChanTableId, index, NULL);
        dtId = pvtGetChild(hVal, tableEntryId, __h245(dataType), NULL);
        if (isDynamicPayload)
            *isDynamicPayload =
                (pvtGetChild(hVal, tableEntryId, __h245(isDynamicPayloadType), NULL) >=0)?RV_TRUE:RV_FALSE;

        {
#if (RV_H245_LEAN_H223 == RV_NO)
            int nonH235DtId;

            if (nonH235 && (nonH235DtId=pvtGetChild2(hVal,dtId,__h245(h235Media) , __h245(mediaType)))>=0)
                ret =pvtSetTree(hVal, dataTypeId, hVal, nonH235DtId);
            else
#endif
                ret = pvtSetTree(hVal, dataTypeId, hVal, dtId);
        }

        pvtDelete(hVal, pathId);
        if (ret<0)
            return ret;

        return RV_TRUE;
    }


    pvtDelete(hVal, pathId);
    return RV_ERROR_UNKNOWN; /* not found */
}


int /* real number of channels or negative value on failure */
confGetChannelNames(
            /* build array of channels names as in configuration */
            IN  HPVT hVal,
            IN  int confRootId,
            IN  int nameArSize, /* number of elements in nameArray */
            IN  int nameLength, /* length of each name in array */
            OUT char** nameArray /* user allocated array of strings */
            )
{
    RvPvtNodeId chanSetId, chanEntryId;
    RvInt32 i;
    
    if (!hVal || confRootId<0 || nameArSize<1 || !nameArray)
        return RV_ERROR_UNKNOWN;
    
    chanSetId = pvtGetChild(hVal, confRootId, __h245(channels), NULL);
    if (!RV_PVT_NODEID_IS_VALID(chanSetId))
        return RV_ERROR_UNKNOWN;

    /* loop all channels */
    i = 0;
    chanEntryId = pvtChild(hVal, chanSetId);
    while (RV_PVT_NODEID_IS_VALID(chanEntryId) && (i < nameArSize))
    {
        pvtGetString(hVal, pvtGetChild(hVal, chanEntryId, __h245(name), NULL), (RvInt32)nameLength, nameArray[i]);
        chanEntryId = pvtBrother(hVal, chanEntryId);
        i++;
    }

    /* Beware of overruns... */
    if (i < nameArSize)
        nameArray[i] = NULL;

    return pvtNumChilds(hVal, chanSetId);
}





int /* RV_TRUE if found. negative value if not found */
confGetModeEntry(
         /* Search mode name in configuration. */
         IN  HPVT hVal,
         IN  int confRootId,
         IN  char *modeName, /* in conf. */
         OUT RvInt32 *entryId /* mode entry id */
         )
{
  int pathId, dataId;
  HPST synConf = pvtGetSynTree(hVal, confRootId);
  RvInt32 index=RV_ERROR_UNKNOWN;
  int ret = RV_ERROR_UNKNOWN;

  if (!hVal || !synConf || !entryId || !modeName) return RV_ERROR_UNKNOWN;

  pathId = pvtAddRoot(hVal, synConf, 0, NULL);
  if (pathId<0)
      return pathId;
  __pvtBuildByFieldIds(dataId, hVal, pathId,
                       {_h245(modes) _nul(1) _h245(name) LAST_TOKEN},
                       (RvInt32)strlen(modeName), modeName);

  if (pvtSearchPath(hVal, confRootId, hVal, pathId, RV_TRUE) == RV_TRUE)  { /* found entry */
    pvtGet(hVal, pvtParent(hVal, pvtParent(hVal, dataId)), NULL, NULL, &index, NULL);
    ret = RV_TRUE;
  }

  if (entryId) {
    int modesId = pvtGetChild(hVal, confRootId, __h245(modes), NULL);
    *entryId=pvtGetByIndex(hVal, pvtGetByIndex(hVal, modesId, index, NULL), 2, NULL);
  }

  pvtDelete(hVal, pathId);
  return ret;
}



#ifdef __cplusplus
}
#endif



