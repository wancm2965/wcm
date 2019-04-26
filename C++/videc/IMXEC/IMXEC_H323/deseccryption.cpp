
#include "deseccryption.h"
#include "EncryptionCapability.h"

DesEncryption::DesEncryption()
{
	m_bInit=false;
	m_nAlgorithmId=0;
}

DesEncryption::~DesEncryption()
{

}


void DesEncryption::EncryptionInit(unsigned int algorithmId)
{	

	if(m_bInit==false)
	{
		RvEncryptionMode mode;
		m_nAlgorithmId=algorithmId;
		EncDecAlgorithmID algId=(EncDecAlgorithmID)algorithmId;
		switch(algId)
		{
		case ALGORITHM_AES_CBC:
		case ALGORITHM_AES256_CBC:
		case ALGORITHM_DES_CBC:
		case ALGORITHM_TRIPLE_DES_CBC:
			mode=RV_ENCRYPTIONMODE_CBC;
			break;
		case ALGORITHM_AES_ECB:
		case ALGORITHM_DES_ECB:
		case ALGORITHM_TRIPLE_DES_ECB:
			mode=RV_ENCRYPTIONMODE_ECB;
			break;

		default:
			return;
		}

	rvRtpDesEncryptionConstruct(mode,&m_DesEncryptor);
	rvRtp3DesEncryptionConstruct(mode,&m_TripleDesEncryptor);
	RvSrtpAesEncryptionConstruct(&m_AesEncryptor);
	m_bInit=true;
	}
}

void DesEncryption::EncryptionEnd( )
{
	if(m_bInit)
	{
	rvRtpDesEncryptionDestruct(&m_DesEncryptor);
	rvRtp3DesEncryptionDestruct(&m_TripleDesEncryptor);
	RvSrtpAesEncryptionDestruct(&m_AesEncryptor);
	}

	m_bInit=false;
	m_nAlgorithmId=0;
}


void DesEncryption::SetEncryption(HRTPSESSION hRTP, const char *encrytionKey,const char *decryptionKey)
{
	EncDecAlgorithmID algId=(EncDecAlgorithmID)m_nAlgorithmId;
	switch(algId)
	{
	case ALGORITHM_DES_CBC:
	case ALGORITHM_DES_ECB:
		RtpSessionSetDesEncryption(hRTP,encrytionKey,decryptionKey);
		break;

	case ALGORITHM_TRIPLE_DES_CBC:
	case ALGORITHM_TRIPLE_DES_ECB:
		RtpSessionSet3DesEncryption(hRTP,encrytionKey,decryptionKey);
		break;
	case ALGORITHM_AES_CBC:
		RtpSessionSetAesEncryption(hRTP,encrytionKey,decryptionKey);
		break;
	case ALGORITHM_AES256_CBC:
		RtpSessionSetAesEncryption(hRTP,encrytionKey,decryptionKey,32);
		break;
	case ALGORITHM_AES_ECB:
		break;
	}

}

int DesEncryption::RtpSessionSetDesEncryption(HRTPSESSION hRTP,const char *encrytionKey,const char *decryptionKey)
{
    RvKey ekey, dkey;
	RvRtpEncryptionPlugIn* pluginPtr = rvRtpDesEncryptionGetPlugIn(&m_DesEncryptor);
	int status = RV_OK;
	RvKeyConstruct(&ekey);
	RvKeyConstruct(&dkey);

	RvChar eKeyData[8];
	RvChar dKeyData[8];
	memset(eKeyData,0,8);
	memset(dKeyData,0,8);
	if (NULL!=encrytionKey)
		strncpy(eKeyData, encrytionKey, 7);
	if (NULL!=decryptionKey)
		strncpy(dKeyData, decryptionKey, 7);

	RvKeySetValue(&ekey, eKeyData, 56);
	RvKeySetValue(&dkey, dKeyData, 56);
	 //RvRtpEncryptionMode RV_RTPENCRYPTIONMODE_H235_PADDING仅此可用
	//status = RvRtpSetDoubleKeyEncryption(hRTP,RV_RTPENCRYPTIONMODE_H235_PADDING,&ekey, &dkey, pluginPtr);
	RvKey *pEKey= &ekey;
	RvKey *pDKey= &dkey;
	if (NULL == encrytionKey) pEKey=NULL;
	if (NULL == decryptionKey) pDKey=NULL;
	status = RvRtpSetDoubleKeyEncryption(hRTP, RV_RTPENCRYPTIONMODE_H235_PADDING, pEKey, pDKey, pluginPtr);

	RvKeyDestruct(&ekey);
	RvKeyDestruct(&dkey);
	return status;
}

int DesEncryption::RtpSessionSet3DesEncryption(HRTPSESSION hRTP,const char *encrytionKey,const char *decryptionKey)
{
    RvKey ekey, dkey;
	RvRtpEncryptionPlugIn* pluginPtr = rvRtp3DesEncryptionGetPlugIn(&m_TripleDesEncryptor);
	int status = RV_OK;
	RvChar eKeyData[21];
	RvChar dKeyData[21];
	memset(eKeyData,0,21);
	memset(dKeyData,0,21);
    if (NULL!=encrytionKey)
	    strncpy(eKeyData, encrytionKey, 21);
    if (NULL!=decryptionKey)
		strncpy(dKeyData, decryptionKey, 21);

	RvKeyConstruct(&ekey);
	RvKeyConstruct(&dkey);
	RvKeySetValue(&ekey, eKeyData, 168);
	RvKeySetValue(&dkey, dKeyData, 168);

	RvKey *pEKey= &ekey;
	RvKey *pDKey= &dkey;
	if (NULL == encrytionKey) pEKey=NULL;
	if (NULL == decryptionKey) pDKey=NULL;
	status = RvRtpSetDoubleKeyEncryption(hRTP, RV_RTPENCRYPTIONMODE_H235_PADDING, pEKey, pDKey, pluginPtr);

	//RvRtpEncryptionMode RV_RTPENCRYPTIONMODE_H235_PADDING仅此可用
	//status = RvRtpSetDoubleKeyEncryption(hRTP, RV_RTPENCRYPTIONMODE_H235_PADDING, &ekey, &dkey, pluginPtr);
	RvKeyDestruct(&ekey);
	RvKeyDestruct(&dkey);
	return status;
}

int DesEncryption::RtpSessionSetAesEncryption(HRTPSESSION hRTP,const char *encrytionKey,const char *decryptionKey,int nKeyLen)
{
	RvKey ekey, dkey;
	RvRtpEncryptionPlugIn* pluginPtr = (RvRtpEncryptionPlugIn* )RvSrtpAesEncryptionGetPlugIn(&m_AesEncryptor);
	int status = RV_OK;
	RvChar eKeyData[64];
	RvChar dKeyData[64];

	memset(eKeyData,0,sizeof(eKeyData));
	memset(dKeyData,0,sizeof(dKeyData));
	if (NULL!=encrytionKey)
		strncpy(eKeyData, encrytionKey,nKeyLen);
	if (NULL!=decryptionKey)
		strncpy(dKeyData, decryptionKey,nKeyLen);

	RvKeyConstruct(&ekey);
	RvKeyConstruct(&dkey);
	RvKeySetValue(&ekey, eKeyData, nKeyLen * 8);
	RvKeySetValue(&dkey, dKeyData, nKeyLen * 8);
	//RvRtpEncryptionMode RV_RTPENCRYPTIONMODE_H235_PADDING仅此可用
	RvKey *pEKey= &ekey;
	RvKey *pDKey= &dkey;
	if (NULL == encrytionKey) pEKey=NULL;
	if (NULL == decryptionKey) pDKey=NULL;
	status = RvRtpSetDoubleKeyEncryption(hRTP, RV_RTPENCRYPTIONMODE_H235_PADDING, pEKey, pDKey, pluginPtr);

	RvKeyDestruct(&ekey);
	RvKeyDestruct(&dkey);
	return status;
}
