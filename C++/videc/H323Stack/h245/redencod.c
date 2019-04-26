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

#include "pvaltree.h"
#include "cmConf.h"
#include "cmchan.h"
#include "h245.h"
#include "redencod.h"
#include "cmH245Object.h"


#ifdef __cplusplus
extern "C" {
#endif


#if (RV_H245_LEAN_H223 == RV_NO)

int createNonStandardRedMethod( IN  HAPP                    hApp,
                                IN  cmNonStandardIdentifier *identifier,
                                IN  char *data,
                                IN  int dataLength )

 {

    HPVT        hVal;
    int         nodeId,nonStandardId;
    int         ret;
    H245Object  *app  = (H245Object*)cmiGetH245Handle(hApp);

    if (!app) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "createNonStandardRedMethod: hApp=%p",hApp));

    hVal = app->hVal;

    nodeId = pvtAddRoot(hVal, app->h245RedEnc, 0, NULL);
    if (nodeId<0)
    {
        H245APIExit((app->pAppObject,"createNonStandardRedMethod=%d", nodeId));
        return nodeId;
    }

    nonStandardId = pvtAdd(hVal, nodeId, __h245(nonStandard), 0, NULL, NULL);
    ret = cmNonStandardParameterCreate(hVal,nonStandardId,identifier,data,dataLength);


    H245APIExit((app->pAppObject, "createNonStandardRedMethod=%d", ret));
    if (ret<0)
      return ret;
    return nodeId;

}

int createRtpAudioRedMethod( IN  HAPP            hApp)
{
    HPVT       hVal;
    int        nodeId,ret;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "createRtpAudioRedMethod: hApp=%p",hApp));

    hVal = app->hVal;

    nodeId = pvtAddRoot(hVal, app->h245RedEnc, 0, NULL);
    if (nodeId<0)
    {
        H245APIExit((app->pAppObject,"createRtpAudioRedMethod=%d", nodeId));
        return nodeId;
    }
    ret=  pvtAdd(hVal, nodeId, __h245(rtpAudioRedundancyEncoding), 0, NULL, NULL);

    H245APIExit((app->pAppObject, "createRtpAudioRedMethod=%d", ret));
    if (ret<0)
      return ret;
    return nodeId;
}

int createH263VideoRedMethod( IN HH245                            hApp,
                              IN cmRTPH263VideoRedundancyEncoding * h263VRedundancyEncoding)
{
    HPVT       hVal;
    int        nodeId,videoId,ii,threadId,ret;
    int        cmFramesBetweenSyncPoints;
    int        iNumOfThreads;
    int        iFrames;
    H245Object *app;

    if (!hApp) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;

    H245APIEnter((app->pAppObject, "createH263VideoRedMethod: hApp=%p",hApp));

    hVal = app->hVal;

    nodeId = pvtAddRoot(hVal, app->h245RedEnc, 0, NULL);
    if (nodeId<0)
    {
        H245APIExit((app->pAppObject, "createH263VideoRedMethod=%d", nodeId));
        return nodeId;
    }
    videoId = pvtAdd(hVal, nodeId, __h245(rtpH263VideoRedundancyEncoding), 0, NULL, NULL);
    iNumOfThreads = h263VRedundancyEncoding->cmNumberOfThreads;
    pvtAdd(hVal, videoId, __h245(numberOfThreads),
                  iNumOfThreads, NULL, NULL);
    cmFramesBetweenSyncPoints=h263VRedundancyEncoding->cmFramesBetweenSyncPoints;
    if (!cmFramesBetweenSyncPoints)
        cmFramesBetweenSyncPoints=256;
    iFrames = h263VRedundancyEncoding->cmFramesBetweenSyncPoints;
    ret=pvtAdd(hVal, videoId, __h245(framesBetweenSyncPoints),
        iFrames, NULL, NULL);
    switch(h263VRedundancyEncoding->cmFrameToThreadMapping)
    {
        case cmRoundrobin:
            ret=pvtAdd(hVal, videoId, __h245(frameToThreadMapping), 0, NULL, NULL);
            ret=pvtAdd(hVal, ret, __h245(roundrobin), 0, NULL, NULL);
        break;
        case cmCustom:
            ret=pvtAdd(hVal, videoId, __h245(frameToThreadMapping), 0, NULL, NULL);
            ret=pvtAdd(hVal, ret, __h245(custom), 0, NULL, NULL);
        break;
    }
    if (h263VRedundancyEncoding->cmContainedThreads.cmContainedThreadsSize)
    {
        threadId = pvtAdd(hVal, videoId, __h245(containedThreads), 0, NULL, NULL);
        for (ii=0;ii< h263VRedundancyEncoding->cmContainedThreads.cmContainedThreadsSize;ii++)
        {
            int iContainedThreads = h263VRedundancyEncoding->cmContainedThreads.cmContainedThreads[ii];
            ret = pvtAdd(hVal, threadId, __nul(0), iContainedThreads, NULL, NULL);
        }
    }

    H245APIExit((app->pAppObject, "createH263VideoRedMethod=%d", ret));
    if (ret < 0)
        return ret;
    return nodeId;
}


int addH263VCustomFramMaping( IN  HH245            hApp,
                              int nodeId,
                              cmRTPH263RedundancyFrameMapping * rtpH263RedundancyFrameMapping,
                              int rtpH263RedundancyFrameMappingSize)
{
    HPVT hVal;
    H245Object *app;

    int customId,ii,frameMapId,jj,frameSeqId;
    int ret=1;

    if (!hApp) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;

    H245APIEnter((app->pAppObject, "addH263VCustomFramMaping: hApp=%p",hApp));

    hVal = ((H245Object *)hApp)->hVal;
    __pvtGetNodeIdByFieldIds(customId, hVal,nodeId,
                            {_h245(rtpH263VideoRedundancyEncoding)
                             _h245(frameToThreadMapping)
                             _h245(custom)
                             LAST_TOKEN});
    if (customId < 0)
    {
        H245APIExit((app->pAppObject, "addH263VCustomFramMaping=%d", customId));
        return customId;
    }
    for (ii=0;ii<rtpH263RedundancyFrameMappingSize;ii++)
    {
        int iThreadNumber;
        frameMapId = pvtAddBranch(hVal, customId, __nul(0));
        iThreadNumber = rtpH263RedundancyFrameMapping[ii].cmThreadNumber;
        ret = pvtAdd(hVal, frameMapId, __h245(threadNumber), iThreadNumber, NULL, NULL);
        frameSeqId = pvtAddBranch(hVal, frameMapId, __h245(frameSequence));
        ret=frameSeqId;
        for (jj = 0;jj<rtpH263RedundancyFrameMapping[ii].cmFrameSequenceSize;jj++)
        {
            int iFrameSeq = rtpH263RedundancyFrameMapping[ii].cmFrameSequence[jj];
            ret = pvtAdd(hVal, frameSeqId, __nul(0), iFrameSeq, NULL, NULL);
        }
    }

    H245APIExit((app->pAppObject, "addH263VCustomFramMaping=%d", ret));
    if(ret < 0)
        return ret;
    return nodeId;
}

RVAPI int RVCALLCONV
cmGetRedundancyEncodingMethod (IN HAPP                        hApp,
                               IN int                         redEncMethodId,
                               OUT cmRedundancyEncodingMethod * encodingMethod)

{
    HPVT       hVal;
    int        nodeId;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    H245APIEnter((app->pAppObject, "cmGetRedundancyEncodingMethod: redEncMethodId = %d ",redEncMethodId));

    hVal = app->hVal;
    nodeId = pvtGetChild(hVal,redEncMethodId,__h245(nonStandard),NULL);
    if (nodeId>=0)
      * encodingMethod = cmRedEncNonStandard;
    nodeId = pvtGetChild(hVal,redEncMethodId,__h245(rtpAudioRedundancyEncoding),NULL);
    if (nodeId>=0)
      * encodingMethod = cmRedEncRtpAudio;
    nodeId = pvtGetChild(hVal,redEncMethodId,__h245(rtpH263VideoRedundancyEncoding),NULL);
    if (nodeId>=0)
      * encodingMethod = cmRedEncH263Video;

    H245APIExit((app->pAppObject, "cmGetRedundancyEncodingMethod: encodingMethod [%d] nodeId [%d].", *encodingMethod, nodeId));
    return nodeId;
}

RVAPI int RVCALLCONV
cmGetH263RedundancyEncoding (IN  HAPP                             hApp,
                             IN  int                              h263EncMethodId,
                             OUT cmRTPH263VideoRedundancyEncoding * rtpH263RedundancyEncoding)
{
    HPVT       hVal;
    int        nodeId,ii,containedThreadsId;
    RvInt32    castValue;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    H245APIEnter((app->pAppObject, "cmGetH263RedundancyEncoding: h263EncMethodId = %d ",h263EncMethodId));

    hVal = app->hVal;
    pvtGetChildByFieldId(hVal,h263EncMethodId,__h245(numberOfThreads),
        &castValue ,NULL);
    rtpH263RedundancyEncoding->cmNumberOfThreads=(RvUint8)castValue;
    pvtGetChildByFieldId(hVal,h263EncMethodId,__h245(framesBetweenSyncPoints),
        &castValue ,NULL);
    rtpH263RedundancyEncoding->cmFramesBetweenSyncPoints=(RvUint8)castValue;

    nodeId = pvtGetChild(hVal,h263EncMethodId,__h245(containedThreads),NULL);
    if (nodeId >=0)
    {
      rtpH263RedundancyEncoding->cmContainedThreads.cmContainedThreadsSize = (RvUint8)pvtNumChilds(hVal,nodeId);
      for (ii=1;ii<= rtpH263RedundancyEncoding->cmContainedThreads.cmContainedThreadsSize;ii++)
      {
        pvtGetByIndex(hVal,nodeId,ii,&containedThreadsId);
        pvtGet(hVal,containedThreadsId,NULL,NULL,
          &castValue ,NULL);
      rtpH263RedundancyEncoding->cmContainedThreads.cmContainedThreads[ii-1]=(RvUint8)castValue;
      }

    }
    __pvtGetNodeIdByFieldIds(nodeId,hVal,h263EncMethodId,
                            {_h245(frameToThreadMapping) _h245(custom) LAST_TOKEN});
    if (nodeId >=0)
      rtpH263RedundancyEncoding->cmFrameToThreadMapping=cmCustom;
    else
    {
      nodeId = 0;
      rtpH263RedundancyEncoding->cmFrameToThreadMapping=cmRoundrobin;
    }

    H245APIExit((app->pAppObject, "cmGetH263RedundancyEncoding=%d", nodeId));
    return nodeId; /* return customId. if it is roundrobin return -1 */

}

/*
  rtp263RedundancyFrameMapping is array,allocated by user. Max size is 256 according to ASN definition
  in rtp263RedundancyFrameMappingSize as IN param the size of allocated array is passed.
  in rtp263RedundancyFrameMappingSize as OUT param  the real array size,according to the h245 message,
  is returned
*/
RVAPI int RVCALLCONV
cmGetCustomFrameToThreadMapping (
                 IN  HAPP            hApp,
                 IN  int             h263EncMethodId,
                 INOUT cmRTPH263RedundancyFrameMapping * rtpH263RedundancyFrameMapping,
                 INOUT int *         rtpH263RedundancyFrameMappingSize )
{
    HPVT       hVal;
    int        ii,customId,childs,frmMapId,frmSeqId,jj,frmSeqElemId;
    RvInt32    castValue;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmGetCustomFrameToThreadMapping: h263EncMethodId = %d ",h263EncMethodId));

    hVal = app->hVal;
    __pvtGetNodeIdByFieldIds(customId,hVal,h263EncMethodId,
                            {_h245(frameToThreadMapping) _h245(custom) LAST_TOKEN});

    if (customId>=0)
    {
      childs = pvtNumChilds(hVal,customId);
      if ( childs < *rtpH263RedundancyFrameMappingSize)
        *rtpH263RedundancyFrameMappingSize = childs;
      for (ii=1;ii<= *rtpH263RedundancyFrameMappingSize;ii++)
      {
        pvtGetByIndex(hVal,customId,ii,&frmMapId);
        pvtGetChildByFieldId(hVal,frmMapId,__h245(threadNumber),&castValue,NULL);
        rtpH263RedundancyFrameMapping[ii-1].cmThreadNumber=(RvUint8)castValue;
        frmSeqId = pvtGetChild(hVal,frmMapId,__h245(frameSequence),NULL);
        rtpH263RedundancyFrameMapping[ii-1].cmFrameSequenceSize=(RvUint8)pvtNumChilds(hVal,frmSeqId);
        for (jj=1;jj<=rtpH263RedundancyFrameMapping[ii-1].cmFrameSequenceSize;jj++)
        {
          pvtGetByIndex(hVal,frmSeqId,jj,&frmSeqElemId);
          pvtGet(hVal,frmSeqElemId,NULL,NULL,
          &castValue ,NULL);
          rtpH263RedundancyFrameMapping[ii-1].cmFrameSequence[jj-1]=(RvUint8)castValue;

        }


      }

    }
    else
      *rtpH263RedundancyFrameMappingSize=0;

    H245APIExit((app->pAppObject, "cmGetCustomFrameToThreadMapping=%d", *rtpH263RedundancyFrameMappingSize));
    return *rtpH263RedundancyFrameMappingSize;
}


RVAPI int RVCALLCONV
cmCreateNonStandardRedMethod(IN HAPP               hApp,
                             IN cmNonStandardParam *nonStandard)
{
    int        ret;
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER | RV_LOGLEVEL_LEAVE))
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);
#endif

    if (!hApp)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmCreateNonStandardRedMethod: hApp=%p", hApp));

    if (nonStandard!=NULL)
        ret = createNonStandardRedMethod(hApp,&nonStandard->info,nonStandard->data,nonStandard->length);
    else
        ret = createNonStandardRedMethod(hApp,NULL,NULL,0);

    H245APIExit((app->pAppObject, "cmCreateNonStandardRedMethod=%d", ret));
    return ret;
}

RVAPI int RVCALLCONV
cmCreateRtpAudioRedMethod( IN    HAPP            hApp)
{
    int result;
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER | RV_LOGLEVEL_LEAVE))
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);
#endif

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmCreateRtpAudioRedMethod: hApp=%p",hApp));
    result = createRtpAudioRedMethod( hApp);
    H245APIExit((app->pAppObject, "cmCreateRtpAudioRedMethod=%d", result));

    return result;
}

RVAPI int RVCALLCONV
cmCreateH263VideoRedMethod( IN HAPP                             hApp,
                            IN cmRTPH263VideoRedundancyEncoding * h263VRedundancyEncoding)
{
    int        result;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmCreateH263VideoRedMethod: hApp=%p",hApp));
    result =  createH263VideoRedMethod((HH245)app,h263VRedundancyEncoding);
    H245APIExit((app->pAppObject, "cmCreateH263VideoRedMethod=%d", result));

    return result;
}



RVAPI int RVCALLCONV
cmAddH263VCustomFrameMapping( IN  HAPP             hApp,
                              int                  nodeId,
                              cmRTPH263RedundancyFrameMapping * rtpH263RedundancyFrameMapping,
                              int rtpH263RedundancyFrameMappingSize)

{
    int        result;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;
    H245APIEnter((app->pAppObject, "cmAddH263VCustomFrameMapping: hApp=%p",hApp));
    result = addH263VCustomFramMaping((HH245)app,nodeId,rtpH263RedundancyFrameMapping,
                                     rtpH263RedundancyFrameMappingSize);
    H245APIExit((app->pAppObject, "cmAddH263VCustomFrameMapping=%d", result));

    return result;
}


#endif

#ifdef __cplusplus
}
#endif



