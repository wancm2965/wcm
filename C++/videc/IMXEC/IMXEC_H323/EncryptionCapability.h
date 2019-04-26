
/*---------------------------------------------------------------*/
//NOTICE: ���ü���������
//DATE: 2011-11-16
/*---------------------------------------------------------------*/

#pragma once
#include "CallCapability.h"

#define MAX_ALGO_NUM 32

typedef enum {
ALGORITHM_NONE=0,
ALGORITHM_DES_ECB,			//DES-ECB
ALGORITHM_DES_CBC,			//DES-CBC
ALGORITHM_DES_CFB,			//DES-CFB
ALGORITHM_DES_OFB,			//DES-OFB
ALGORITHM_TRIPLE_DES_CBC,	//3DES-CBC  notice it oid wait find debug
ALGORITHM_TRIPLE_DES_ECB,	//3DES-ECB  notice it oid wait find debug
ALGORITHM_AES_CBC,			//aes 128
ALGORITHM_AES_ECB,
ALGORITHM_AES256_CBC,		//aes 256
ALGORITHM_COUNT
}EncDecAlgorithmID;



typedef struct tagENCDEC_CAPABILITY_ITEM
{
	EncDecAlgorithmID	algoID;
	tagENCDEC_CAPABILITY_ITEM(void)
	{
		algoID=ALGORITHM_NONE;
	}
}ENCDEC_CAPABILITY_ITEM;

struct EncDecAlgorithmCap
{
	unsigned int				m_nCapNum;	//��Ч����������
	ENCDEC_CAPABILITY_ITEM	    m_Caps[MAX_ALGO_NUM];

	EncDecAlgorithmCap()
	{
		m_nCapNum = 0;
	}
};



class EncryptionCapability : public CallCapability
{
public:
	EncryptionCapability(void);
public:
	~EncryptionCapability(void);
public:
	//����ֵΪ0ʧ�ܻ򲻴���
	virtual int IsExistSimul(int type ,int iEntryNumber); //�����Ƿ����
	virtual int IsFirstSimul(int type ,int iEntryNumber); //�����Ƿ�����ѡ	
	virtual int InitCapabilitis(int node);                //������������������ ʹ��ʱ���ȵ��ô˷���
	virtual int RemoveAllCapabilitis();                   //�Ƴ����������� �ٰ���Ҫ���
	int IsExistFromAlgorithm(EncDecAlgorithmID	algoID);  //�����Ƿ����
	int IsFirstFromAlgorithm(EncDecAlgorithmID	algoID);  //�Ƿ�Ϊͬʱ��������ѡ
	//�����������������ý��������ļ����������� ע����BuildCapabilitis����Ϊ���������������
	int BuildAllMediaEncryptionCapabilitis(int node,CallCapability &callCap);

	//1 audio 2 video 3 H239 4 Data
	int IsExistCodecTypeFromCapsArray(int type ,int codecType,int nLevel);
	int IsFirstCodecTypeFromCapsArray(int type ,int codecType,int nLevel);

	int AddDES_cbcAlgorithm();
	int RemoveDES_cbcAlgorithm();
	int SetFirstDES_cbcAlgorithm();
	int IsExistDES_cbcAlgorithm();                     //DES_cbc�����Ƿ����
	int IsFirstDES_cbcAlgorithm();                     //DES_cbc�Ƿ�Ϊͬʱ��������ѡ


	int AddDES_ecbAlgorithm();
	int RemoveDES_ecbAlgorithm();
	int SetFirstDES_ecbAlgorithm();
	int IsExistDES_ecbAlgorithm();                    //DES_ecb�����Ƿ����
	int IsFirstDES_ecbAlgorithm();                    //DES_ecb�Ƿ�Ϊͬʱ��������ѡ



	int AddTripeDES_cbcAlgorithm();
	int RemoveTripeDES_cbcAlgorithm();
	int SetFirstTripeDES_cbcAlgorithm();
	int IsExistTripeDES_cbcAlgorithm();                     //3DES_cbc�����Ƿ����
	int IsFirstTripeDES_cbcAlgorithm();                     //3DES_cbc�Ƿ�Ϊͬʱ��������ѡ


	int AddTripeDES_ecbAlgorithm();
	int RemoveTripeDES_ecbAlgorithm();
	int SetFirstTripeDES_ecbAlgorithm();
	int IsExistTripeDES_ecbAlgorithm();                    //3DES_ecb�����Ƿ����
	int IsFirstTripeDES_ecbAlgorithm();                    //3DES_ecb�Ƿ�Ϊͬʱ��������ѡ

	int AddAES_cbcAlgorithm();
	int RemoveAES_cbcAlgorithm();
	int SetFirstAES_cbcAlgorithm();
	int IsExistAES_cbcAlgorithm();                     //DES_cbc�����Ƿ����
	int IsFirstAES_cbcAlgorithm();                     //DES_cbc�Ƿ�Ϊͬʱ��������ѡ

	int AddAES256_cbcAlgorithm();
	int RemoveAES256_cbcAlgorithm();
	int SetFirstAES256_cbcAlgorithm();
	int IsExistAES256_cbcAlgorithm();                     //DES_cbc�����Ƿ����
	int IsFirstAES256_cbcAlgorithm();                     //DES_cbc�Ƿ�Ϊͬʱ��������ѡ

private:	
	//��ý�������������в���
	//opetionType first is 0 exist is 1
	int FindEntryFromCapsArray(int type ,int iEntryNumber,int opetionType);

	//opetionType first is 0 exist is 1
	int FindCodecTypeFromCapsArray(int type ,int codecType,int nLevel,int opetionType);

	//��ý���������������в��� ע��Ҫ�ȵ������㷨�Ƿ���ڻ��Ƿ���λ����
	//firExi find  EncryptionDec algorithm is exist or first , 1 exist , 0 is first 
	int FindEntryFromCapsDescriptors(int type ,int iEntryNumber,int firExi=1);

	int SetAlgorithmCap(ENCDEC_CAPABILITY_ITEM  &cap,int type);

	int AddAlgorithmCap(ENCDEC_CAPABILITY_ITEM  &cap);    

	int buildH235MediaCaps(HPVT valH,int nodeId,int iCapIndex,int h235EntryNumber,int iEntryNumber);
	
	//�Ӽӽ������������в���
	//opetionType first is 0 exist is 1
	int FindCapFromAlgorithmCaps(ENCDEC_CAPABILITY_ITEM  &cap,int opetionType);


private:
	//����������Ϣ����
	EncDecAlgorithmCap m_EncDecAlgoCap;

	//��ǰ�����㷨 ����FindEntryFromCapsDescriptors
	ENCDEC_CAPABILITY_ITEM m_currentCap;

	//����FindEntryFromCapsDescriptors
	int m_nFirstExist;

	//������������ ��ʼΪ0
	int m_nCapTableCount;

	//�������������м�����������
	int m_nCapTableEntryCount;
	
	//��3��ͷ��������ΪH235 EntryNumber
	int m_H235EntryNumber; 


};
