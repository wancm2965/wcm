/************************************************************************
 File Name     : rvrtpencryptionplugin.c
 Description   :
*************************************************************************
 Copyright (c)  2001 , RADVision, Inc. All rights reserved.
*************************************************************************
 NOTICE:
 This document contains information that is proprietary to RADVision Inc.
 No part of this publication may be reproduced in any form whatsoever
 without written prior approval by RADVision Inc.

 RADVision Inc. reserves the right to revise this publication and make
 changes without obligation to notify any person of such revisions or
 changes.
*************************************************************************
 $Revision: $
 $Date:   04/19/2001 $
 $Author: Scott K. Eaton $
************************************************************************/
#include "rvlog.h"
//#include "rtptAesencryption.h"
#include "rvsrtpaesplugin.h"

#if (RV_LOGMASK != RV_LOGLEVEL_NONE)
#include "rtputil.h" /* for logger only */
#define rvLogPtr rtpGetSource(RVRTP_SECURITY_MODULE)
#else
#define rvLogPtr (NULL)
#endif
#define logMgr (NULL)



RVAPI RvSrtpAesPlugIn* RVCALLCONV RvSrtpAesPlugInConstruct(  
RvSrtpAesPlugIn*                         plugin,  
    void*                                    userData,  
    RvSrtpAesPlugInEncryptDataConstructCB    construct,  
    RvSrtpAesPlugInEncryptDataDestructCB     destruct,  
    RvSrtpAesPlugInInitializeECBModeCB       initECBMode,  
    RvSrtpAesPlugInInitializeCBCModeCB       initCBCMode,  
    RvSrtpAesPlugInEncryptCB                 encrypt,  
    RvSrtpAesPlugInDecryptCB                 decrypt)
{
	RvLogEnter(rvLogPtr, (rvLogPtr, "RvSrtpAesPlugInConstruct"));
    plugin->encrypt       = encrypt;
    plugin->decrypt       = decrypt;
    plugin->construct       = construct;
    plugin->destruct       = destruct;
    plugin->initECBMode = initECBMode;
    plugin->initCBCMode  = initCBCMode;
    plugin->userData      = userData;
	RvLogLeave(rvLogPtr, (rvLogPtr, "RvSrtpAesPlugInConstruct"));
    return plugin;
}


RVAPI RvSrtpAesPlugIn* RVCALLCONV RvSrtpAesPlugInDestruct(  
    RvSrtpAesPlugIn*    plugin)
{
	RvLogEnter(rvLogPtr, (rvLogPtr, "RvSrtpAesPlugInDestruct"));
    plugin->encrypt       = NULL;
    plugin->decrypt       = NULL;
    plugin->construct       = NULL;
    plugin->destruct       = NULL;
    plugin->initECBMode = NULL;
    plugin->initCBCMode  = NULL;
    plugin->userData      = NULL;
	RvLogLeave(rvLogPtr, (rvLogPtr, "RvSrtpAesPlugInDestruct"));
	 return plugin;
}


RVAPI void  RVCALLCONV  RvAesEncryptionInitializeCBCMode(RvSrtpAesPlugIn *thisPtr,
	RvUint8 direction,
	const RvUint8* key,
	RvUint16 keyBitSize,
	RvUint16 blockBitSize,
	const RvUint8* iv)
{
	thisPtr->initCBCMode((thisPtr),(RvSrtpEncryptionPurpose_ENCRYPT_RTP), (direction), (key), (keyBitSize), (blockBitSize), (iv) );
}





//RVAPI void  RVCALLCONV RvRtpEncryptionPlugInEncrypt(RvSrtpAesPlugIn* thisPtr, RvUint8 * src, RvUint8 * dst,RvUint32 length,RvRtpEncryptionData * data, RvKey * key)
//{
////	RvSrtpAesPlugIn * aespluginPtr=(RvSrtpAesPlugIn*)pluginPtr;
//int keyBitSize=128;
//RvUint8 out=0;
//RvUint8 * encKey=(RvUint8 *)data;
////RvRtpEncryptionPlugInInitialize(aespluginPtr, RvSrtpEncryptionPurpose_ENCRYPT_RTP, RV_AESENCRYPTION_DIRECTION_ENCRYPT, encKey
////, keyBitSize, 128, &out );
//
//	RvRtpEncryptionPlugInInitialize(thisPtr,RvSrtpEncryptionPurpose_ENCRYPT_RTP,0/*RV_AESENCRYPTION_DIRECTION_ENCRYPT*/,encKey, keyBitSize, 128,iv );
//	(thisPtr)->encrypt((thisPtr),(RvSrtpEncryptionPurpose_ENCRYPT_RTP),(src),(dst),(length));
//}

//RVAPI void  RVCALLCONV RvRtpEncryptionPlugInDecrypt(RvSrtpAesPlugIn* thisPtr, RvUint8 * src, RvUint8 * dst,RvUint32 lengthPtr,RvRtpEncryptionData * data, RvKey * key)
//{
////int keyBitSize=128;
////RvUint8 out=0;
////RvUint8 * encKey=(RvUint8 *)data;
////	RvRtpEncryptionPlugInInitialize(thisPtr,RvSrtpEncryptionPurpose_ENCRYPT_RTP,1/*RV_AESENCRYPTION_DIRECTION_DECRYPT*/,encKey, keyBitSize, 128, iv );
//
//	(thisPtr)->decrypt((thisPtr),(RvSrtpEncryptionPurpose_ENCRYPT_RTP),(src),(dst),(lengthPtr));
//}







/***************************************************************************************
 * RvRtpEncryptionPlugInConstructCopy
 * description:  This method constructs a copy of a RvRtpEncryptionPlugIn.
 * parameters:
 *      thisPtr - the RvRtpEncryptionPlugIn object.
 *       srcPtr - the RvRtpEncryptionPlugIn object to base the new object on.
 * allocatorPtr - The allocator to use.
 * returns: A pointer to the object, if successful. NULL, otherwise.
 ***************************************************************************************/

//RVAPI
//RvRtpEncryptionPlugIn* RVCALLCONV RvRtpEncryptionPlugInConstructCopy(
//	RvRtpEncryptionPlugIn *thisPtr,
//    const RvRtpEncryptionPlugIn *srcPtr,
//    void *allocatorPtr)
//{
//	RvLogEnter(rvLogPtr, (rvLogPtr, "RvRtpEncryptionPlugInConstructCopy"));
//    thisPtr->encrypt       = srcPtr->encrypt;
//    thisPtr->decrypt       = srcPtr->decrypt;
//    thisPtr->getHeaderSize = srcPtr->getHeaderSize;
//    thisPtr->getBlockSize  = srcPtr->getBlockSize;
//    thisPtr->userData      = srcPtr->userData;
//	RvLogLeave(rvLogPtr, (rvLogPtr, "RvRtpEncryptionPlugInConstructCopy"));	
//	RV_UNUSED_ARG(allocatorPtr);
//    return thisPtr;
//}
//
