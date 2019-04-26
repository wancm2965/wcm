
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

	//��ʼ���ӽ��ܽӿ� algorithmId ʹ�õ��㷨
	void EncryptionInit(unsigned int algorithmId);
	//����
	void EncryptionEnd( );

	//���üӽ�����ʹ�õ����� hRTP RTP�����ʶ encrytionKey�������� decryptionKey ��������
	void SetEncryption(HRTPSESSION hRTP,const char *encrytionKey,const char *decryptionKey);

private:

	//����DES�ӽ�����ʹ�õ����� hRTP RTP�����ʶ encrytionKey�������� decryptionKey ��������
	int RtpSessionSetDesEncryption(HRTPSESSION hRTP,const char *encrytionKey,const char *decryptionKey);

	//����3DES�ӽ�����ʹ�õ����� hRTP RTP�����ʶ encrytionKey�������� decryptionKey ��������
	int RtpSessionSet3DesEncryption(HRTPSESSION hRTP,const char *encrytionKey,const char *decryptionKey);

	int RtpSessionSetAesEncryption(HRTPSESSION hRTP,const char *encrytionKey,const char *decryptionKey,int nKeyLen=16);

};

#endif  //__DesEncryption_H__