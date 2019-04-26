/************************************************************************
 File Name	   : rvsrtpaesplugin.h
 Description   :
************************************************************************/

#if !defined(RVAESENCRYPTIONPLUGIN_H)
#define RVAESENCRYPTIONPLUGIN_H

#include "rvtypes.h"
#include "rvrtpencryptiondata.h"
#include "rvkey.h"
/*#include "rvmemory.h"*/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************
*	RvRtpEncryptionPlugIn
******************************************************************/
struct RvSrtpAesPlugIn_;



//Description
//Allows identification of encrypting AES encryption data according to the applied purposes in the SRTP module. This is necessary to avoid memory collisions in different AES encryption contexts for the same session.
//
//RvSrtpEncryptionPurpose_ENCRYPT_RTP
//The RTP encryption in mode AESCM.
//RvSrtpEncryptionPurpose_ENCRYPT_RTCP
//The RTCP encryption in mode AESCM.
//RvSrtpEncryptionPurpose_DECRYPT_RTP
//The RTP decryption in mode AESCM.
//RvSrtpEncryptionPurpose_DECRYPT_RTCP
//The RTCP decryption in mode AESCM.
//RvSrtpEncryptionPurpose_IV_BUILDING_ENCRYPT_RTP
//The AES is used for building IV for encrypting RTP.
//RvSrtpEncryptionPurpose_IV_BUILDING_ENCRYPT_RTCP
//The AES is used for building IV for encrypting RTCP.
//RvSrtpEncryptionPurpose_IV_BUILDING_DECRYPT_RTP
//The AES is used for building IV for decrypting RTP.
//RvSrtpEncryptionPurpose_IV_BUILDING_DECRYPT_RTCP
//The AES is used for building IV for decrypting RTCP.
//RvSrtpEncryptionPurpose_KeyDerivation_ENCRYPT_RTP
//The KeyDerivation in mode AESCM when encryptions RTP.
//RvSrtpEncryptionPurpose_KeyDerivation_ENCRYPT_RTCP
//The KeyDerivation in mode AESCM when encryptions RTCP.
//RvSrtpEncryptionPurpose_KeyDerivation_DECRYPT_RTP
//The KeyDerivation in mode AESCM when decrypting RTP.
//RvSrtpEncryptionPurpose_KeyDerivation_DECRYPT_RTCP
//The KeyDerivation in mode AESCM when decrypting RTCP.

typedef enum _RvSrtpAesEncryptionPurpose {  
    RvSrtpEncryptionPurpose_ENCRYPT_RTP = 0,  
    RvSrtpEncryptionPurpose_ENCRYPT_RTCP,  
    RvSrtpEncryptionPurpose_DECRYPT_RTP,  
    RvSrtpEncryptionPurpose_DECRYPT_RTCP,  
    RvSrtpEncryptionPurpose_IV_BUILDING_ENCRYPT_RTP,  
    RvSrtpEncryptionPurpose_IV_BUILDING_ENCRYPT_RTCP,  
    RvSrtpEncryptionPurpose_IV_BUILDING_DECRYPT_RTP,  
    RvSrtpEncryptionPurpose_IV_BUILDING_DECRYPT_RTCP,  
    RvSrtpEncryptionPurpose_KeyDerivation_ENCRYPT_RTP,  
    RvSrtpEncryptionPurpose_KeyDerivation_ENCRYPT_RTCP,  
    RvSrtpEncryptionPurpose_KeyDerivation_DECRYPT_RTP,  
    RvSrtpEncryptionPurpose_KeyDerivation_DECRYPT_RTCP,  
    RvSrtpEncryptionPurposes,  
}RvSrtpEncryptionPurpose;




typedef void (*RvSrtpAesPlugInEncryptDataConstructCB) (
        struct RvSrtpAesPlugIn_ *thisPtr,
        RvSrtpEncryptionPurpose purpose);

typedef void (*RvSrtpAesPlugInEncryptDataDestructCB) (
                                            struct RvSrtpAesPlugIn_ *thisPtr,
                                            RvSrtpEncryptionPurpose purpose);


typedef RvBool (*RvSrtpAesPlugInInitializeECBModeCB) (
     struct RvSrtpAesPlugIn_ *thisPtr,
     RvSrtpEncryptionPurpose purpose,
     RvUint8 direction,
     const RvUint8* key,
     RvUint16 keyBitSize,
     RvUint16 blockBitSize) ;

typedef RvBool (*RvSrtpAesPlugInInitializeCBCModeCB) (
     struct RvSrtpAesPlugIn_ *thisPtr,
     RvSrtpEncryptionPurpose purpose,
     RvUint8 direction,
     const RvUint8* key,
     RvUint16 keyBitSize,
     RvUint16 blockBitSize,
     const RvUint8* iv) ;



typedef void (*RvSrtpAesPlugInEncryptCB) (
    struct RvSrtpAesPlugIn_ *thisPtr,
     RvSrtpEncryptionPurpose purpose,
    const RvUint8 *srcBuffer,
    RvUint8 *dstBuffer,
    RvUint32 byteLength) ;


typedef void (*RvSrtpAesPlugInDecryptCB) (
    struct RvSrtpAesPlugIn_ *thisPtr,
    RvSrtpEncryptionPurpose purpose,
    const RvUint8 *srcBuffer,
    RvUint8 *dstBuffer,
    RvUint32 byteLength) ;


//typedef RvSrtpAesEncryption* RVCALLCONV RvSrtpAesEncryptionConstruct(RvSrtpAesEncryption *thisPtr) ;

//typedef  ;
//
//
//
//typedef  ;
//
//
//typedef  ;



//typedef void (*RvRtpEncryptionPlugInEncryptCB)(struct RvRtpEncryptionPlugIn_ *thisPtr,
//								const RvUint8 *srcBuffer,
//								RvUint8 *dstBuffer,
//								RvUint32 byteLength,
//                                RvRtpEncryptionData* dataPtr,
//                                const  RvKey* key);
//
//typedef void (*RvRtpEncryptionPlugInDecryptCB)(struct RvRtpEncryptionPlugIn_ *,
//								const RvUint8 *srcBuffer,
//								RvUint8 *dstBuffer,
//								RvUint32 byteLength,
//                                RvRtpEncryptionData* dataPtr,
//                                const RvKey* key);



/***************************************************************************************
Description
This interface is used to allow the RTP Session to encrypt/decrypt data. To use this interface the user must implement all of the following callbacks:
RvSrtpAesPlugInEncryptDataConstructCB
RvSrtpAesPlugInEncryptDataDestructCB
RvSrtpAesPlugInInitializeECBModeCB
RvSrtpAesPlugInInitializeCBCModeCB
RvSrtpAesPlugInEncryptCB
RvSrtpAesPlugInDecryptCB
For more information, see the example of the implementation of this plug-in which uses the OpenSSL library in the Test Application that is supplied with the package. You can find it in the appl/rtpRtcp/testapp directory.
The Test Application is currently supplied for some, but not all, operating systems.
Parameters
construct
This callback is called when the user needs to construct data (encryption configuration) related to the encryption.
destruct
This callback is called when the user needs to destruct data (encryption configuration) related to the encryption.
initECBMode
This callback is called when the user needs to initialize the AES encryption in ECB mode.
initCBCMode
This callback is called when the user needs to initialize the AES encryption in CBC mode.
encrypt
The RvSrtpAesPlugInEncryptCB callback to use with the object.
decrypt
The RvSrtpAesPlugInDecryptCB callback to use with the object.
userData
The user data associated with the object.
http://community.radvision.com/artp/documentation/srtp-ref/14526/RvSrtpAesPlugIn/
****************************************************************************************/

typedef struct RvSrtpAesPlugIn_{
	RvSrtpAesPlugInEncryptCB                 encrypt;  
	RvSrtpAesPlugInDecryptCB                 decrypt;
	RvRtpEncryptionPlugInGetBlockSizeCB      getBlockSize;
	RvRtpEncryptionPlugInGetHeaderSizeCB     getHeaderSize;
    RvSrtpAesPlugInEncryptDataConstructCB    construct;  
    RvSrtpAesPlugInEncryptDataDestructCB     destruct;  
    RvSrtpAesPlugInInitializeECBModeCB       initECBMode;  
    RvSrtpAesPlugInInitializeCBCModeCB       initCBCMode;
    void*                                    userData;  
}RvSrtpAesPlugIn;



RVAPI RvSrtpAesPlugIn* RVCALLCONV RvSrtpAesPlugInConstruct(  
RvSrtpAesPlugIn*                         plugin,  
    void*                                    userData,  
    RvSrtpAesPlugInEncryptDataConstructCB    construct,  
    RvSrtpAesPlugInEncryptDataDestructCB     destruct,  
    RvSrtpAesPlugInInitializeECBModeCB       initECBMode,  
    RvSrtpAesPlugInInitializeCBCModeCB       initCBCMode,  
    RvSrtpAesPlugInEncryptCB                 encrypt,  
    RvSrtpAesPlugInDecryptCB                 decrypt);

RVAPI RvSrtpAesPlugIn* RVCALLCONV RvSrtpAesPlugInDestruct(  
    RvSrtpAesPlugIn*    plugin);


//RVAPI void  RVCALLCONV RvRtpEncryptionPlugInEncrypt(RvSrtpAesPlugIn* thisPtr, RvUint8 * src, RvUint8 * dst,RvUint32 length,RvRtpEncryptionData * data, RvKey * key);
//
//RVAPI void  RVCALLCONV RvRtpEncryptionPlugInDecrypt( RvSrtpAesPlugIn* thisPtr, RvUint8 * src, RvUint8 * dst,RvUint32 lengthPtr,RvRtpEncryptionData * data, RvKey * key);



/* Unpublished protected methods */
//#define  RvRtpEncryptionPlugInEncrypt(thisPtr, src, dst, length, data, key)	  (thisPtr)->encrypt((thisPtr),(RvSrtpEncryptionPurpose_ENCRYPT_RTP),(src),(dst),(length))
//#define  RvRtpEncryptionPlugInDecrypt(thisPtr, src, dst, lengthPtr, data, key) (thisPtr)->decrypt((thisPtr),(RvSrtpEncryptionPurpose_ENCRYPT_RTP),(src),(dst),(lengthPtr))
//#define  RvRtpEncryptionPlugInGetHeaderSize(thisPtr)					  (thisPtr)->getHeaderSize((thisPtr))
//#define  RvRtpEncryptionPlugInGetBlockSize(thisPtr) 					  16 /*(thisPtr)->getBlockSize((thisPtr))*/

//#define  RvRtpEncryptionPlugInInitialize(thisPtr, purpose, direc, key, keybit, blockbitsize, iv ) 					  (thisPtr)->initCBCMode((thisPtr),(purpose), (direc), (key), (keybit), (blockbitsize), (iv) )

//
///***************************************************************************************
// * RvRtpEncryptionPlugInConstruct
// * description:  This method constructs a RvRtpEncryptionPlugIn. All of
// *               the callbacks must be suppled for this plugin to work.
// * parameters:
// *    thisPtr - the RvRtpEncryptionPlugIn object.
// *   userData - the user data associated with the object.
// *    encrypt - the RvRtpEncryptionPlugInEncryptCB callback to use with the object.
// *    decrypt - the RvRtpEncryptionPlugInGetBlockSizeCB callback to use with the object.
// * getBlockSize - the RvRtpEncryptionPlugInGetBlockSizeCB callback to use with the object. 
// * getHeaderSize - the RvRtpEncryptionPlugInGetHeaderSizeCB callback to use with the object. 
// * returns: A pointer to the object, if successful. NULL, otherwise.
// ***************************************************************************************/
//RVAPI
//RvRtpEncryptionPlugIn* RVCALLCONV RvRtpEncryptionPlugInConstruct(
//	RvRtpEncryptionPlugIn *thisPtr,
//	void*								 userData,
//	RvRtpEncryptionPlugInEncryptCB		 encrypt,
//	RvRtpEncryptionPlugInDecryptCB		 decrypt,
//	RvRtpEncryptionPlugInGetBlockSizeCB  getBlockSize,
//	RvRtpEncryptionPlugInGetHeaderSizeCB getHeaderSize);
///***************************************************************************************
// * RvRtpEncryptionPlugInConstructCopy
// * description:  This method constructs a copy of a RvRtpEncryptionPlugIn.
// * parameters:
// *      thisPtr - the RvRtpEncryptionPlugIn object.
// *       srcPtr - the RvRtpEncryptionPlugIn object to base the new object on.
// * allocatorPtr - The allocator to use.
// * returns: A pointer to the object, if successful. NULL, otherwise.
// ***************************************************************************************/
//RVAPI
//RvRtpEncryptionPlugIn* RVCALLCONV RvRtpEncryptionPlugInConstructCopy(
//	RvRtpEncryptionPlugIn *thisPtr,
//	const RvRtpEncryptionPlugIn *srcPtr,
//	void *allocatorPtr);
///***************************************************************************************
// * RvRtpEncryptionPlugInDestruct 
// * description:
// *     This method destroys the plugin, freeing any recources it held.
// * parameters:
// *    thisPtr - the RvRtpEncryptionPlugIn object. 
// * returns: none
// ***************************************************************************************/
//RVAPI
//void RVCALLCONV RvRtpEncryptionPlugInDestruct(RvRtpEncryptionPlugIn *thisPtr);
/***************************************************************************************
 * RvRtpEncryptionPlugInSetUserData 
 * description:
 *      This method sets the plugin's user data. This is a piece of
 *      data that the user can use for what ever they like, it is
 *      not used internally by the class.
 * parameters:
 *       thisPtr - the RvRtpEncryptionPlugIn object. 
 *      userData - the user data.
 * returns : none
 ***************************************************************************************/
//#define  RvRtpEncryptionPlugInSetUserData(thisPtr, d)  ((thisPtr)->userData = (d))
/***************************************************************************************
 * RvRtpEncryptionPlugInGetUserData  
 * description:
 *      This method returns the user supplied data from the plugin.  
 * Parameters:
 *   thisPtr - the RvRtpEncryptionPlugIn object. 
 * returns: The user supplied data.
 ***************************************************************************************/
//#define  RvRtpEncryptionPlugInGetUserData(thisPtr)	   ((thisPtr)->userData)

/* Unpublished protected methods */
//#define  RvRtpEncryptionPlugInEncrypt(thisPtr, src, dst, length, data, key)	  (thisPtr)->encrypt((thisPtr),(src),(dst),(length),(data),(key))
//#define  RvRtpEncryptionPlugInDecrypt(thisPtr, src, dst, lengthPtr, data, key) (thisPtr)->decrypt((thisPtr),(src),(dst),(lengthPtr),(data),(key))
//#define  RvRtpEncryptionPlugInGetHeaderSize(thisPtr)					  (thisPtr)->getHeaderSize((thisPtr))
//#define  RvRtpEncryptionPlugInGetBlockSize(thisPtr) 					  (thisPtr)->getBlockSize((thisPtr))

#ifdef __cplusplus
}
#endif

#endif	/* Include guard */

