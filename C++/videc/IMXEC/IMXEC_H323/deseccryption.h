
/*---------------------------------------------------------------*/
//NOTICE: Call DES 3DES implementation
//DATE: 2011-11-22
/*---------------------------------------------------------------*/


#ifndef __DesEncryption_H__
#define __DesEncryption_H__

#include <RVINC/rtptRtpDesEncryption.h>
#include <RVINC/rtptRtp3DesEncryption.h>
#include <RVINC/rtptSrtpaesencryption.h>

#include <RVINC/rtp.h>
#include <RVINC/rtcp.h>

////////////////////////////////////////////////////////////////////////////////////////
class DesEncryption
{
public:
	DesEncryption();
	~DesEncryption();

public:
	RvRtpDesEncryption		m_DesEncryptor;			//DES encryption implementation
	RvRtp3DesEncryption		m_TripleDesEncryptor;	//3DES encryption implementation
	RvSrtpAesEncryption		m_AesEncryptor;			//AES encryption implementation
	unsigned int			m_nAlgorithmId;			//encryption algorithm
	bool					m_bInit;				//construct result

	//初始化加解密接口 algorithmId 使用的算法
	void EncryptionInit(unsigned int algorithmId);
	//结束
	void EncryptionEnd( );

	//设置加解密所使用的密码 hRTP RTP句柄标识 encrytionKey加密密码 decryptionKey 解密密码
	void SetEncryption(HRTPSESSION hRTP,const char *encrytionKey,const char *decryptionKey);

private:

	//设置DES加解密所使用的密码 hRTP RTP句柄标识 encrytionKey加密密码 decryptionKey 解密密码
	int RtpSessionSetDesEncryption(HRTPSESSION hRTP,const char *encrytionKey,const char *decryptionKey);

	//设置3DES加解密所使用的密码 hRTP RTP句柄标识 encrytionKey加密密码 decryptionKey 解密密码
	int RtpSessionSet3DesEncryption(HRTPSESSION hRTP,const char *encrytionKey,const char *decryptionKey);

	int RtpSessionSetAesEncryption(HRTPSESSION hRTP,const char *encrytionKey,const char *decryptionKey,int nKeyLen=16);

};

#endif  //__DesEncryption_H__