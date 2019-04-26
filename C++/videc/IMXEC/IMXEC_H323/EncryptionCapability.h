
/*---------------------------------------------------------------*/
//NOTICE: 设置加密能力项
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
	unsigned int				m_nCapNum;	//有效能力集个数
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
	//返回值为0失败或不存在
	virtual int IsExistSimul(int type ,int iEntryNumber); //能力是否存在
	virtual int IsFirstSimul(int type ,int iEntryNumber); //能力是否是首选	
	virtual int InitCapabilitis(int node);                //设置所有能力集参数 使用时首先调用此方法
	virtual int RemoveAllCapabilitis();                   //移除所有能力集 再按需要添加
	int IsExistFromAlgorithm(EncDecAlgorithmID	algoID);  //能力是否存在
	int IsFirstFromAlgorithm(EncDecAlgorithmID	algoID);  //是否为同时能力项首选
	//添加所有能力集表中媒体能力项的加密能力参数 注区别：BuildCapabilitis函数为按需添加能力集项
	int BuildAllMediaEncryptionCapabilitis(int node,CallCapability &callCap);

	//1 audio 2 video 3 H239 4 Data
	int IsExistCodecTypeFromCapsArray(int type ,int codecType,int nLevel);
	int IsFirstCodecTypeFromCapsArray(int type ,int codecType,int nLevel);

	int AddDES_cbcAlgorithm();
	int RemoveDES_cbcAlgorithm();
	int SetFirstDES_cbcAlgorithm();
	int IsExistDES_cbcAlgorithm();                     //DES_cbc能力是否存在
	int IsFirstDES_cbcAlgorithm();                     //DES_cbc是否为同时能力项首选


	int AddDES_ecbAlgorithm();
	int RemoveDES_ecbAlgorithm();
	int SetFirstDES_ecbAlgorithm();
	int IsExistDES_ecbAlgorithm();                    //DES_ecb能力是否存在
	int IsFirstDES_ecbAlgorithm();                    //DES_ecb是否为同时能力项首选



	int AddTripeDES_cbcAlgorithm();
	int RemoveTripeDES_cbcAlgorithm();
	int SetFirstTripeDES_cbcAlgorithm();
	int IsExistTripeDES_cbcAlgorithm();                     //3DES_cbc能力是否存在
	int IsFirstTripeDES_cbcAlgorithm();                     //3DES_cbc是否为同时能力项首选


	int AddTripeDES_ecbAlgorithm();
	int RemoveTripeDES_ecbAlgorithm();
	int SetFirstTripeDES_ecbAlgorithm();
	int IsExistTripeDES_ecbAlgorithm();                    //3DES_ecb能力是否存在
	int IsFirstTripeDES_ecbAlgorithm();                    //3DES_ecb是否为同时能力项首选

	int AddAES_cbcAlgorithm();
	int RemoveAES_cbcAlgorithm();
	int SetFirstAES_cbcAlgorithm();
	int IsExistAES_cbcAlgorithm();                     //DES_cbc能力是否存在
	int IsFirstAES_cbcAlgorithm();                     //DES_cbc是否为同时能力项首选

	int AddAES256_cbcAlgorithm();
	int RemoveAES256_cbcAlgorithm();
	int SetFirstAES256_cbcAlgorithm();
	int IsExistAES256_cbcAlgorithm();                     //DES_cbc能力是否存在
	int IsFirstAES256_cbcAlgorithm();                     //DES_cbc是否为同时能力项首选

private:	
	//从媒体能力集数组中查找
	//opetionType first is 0 exist is 1
	int FindEntryFromCapsArray(int type ,int iEntryNumber,int opetionType);

	//opetionType first is 0 exist is 1
	int FindCodecTypeFromCapsArray(int type ,int codecType,int nLevel,int opetionType);

	//从媒体能力集描述表中查找 注意要先调加密算法是否存在或是否首位函数
	//firExi find  EncryptionDec algorithm is exist or first , 1 exist , 0 is first 
	int FindEntryFromCapsDescriptors(int type ,int iEntryNumber,int firExi=1);

	int SetAlgorithmCap(ENCDEC_CAPABILITY_ITEM  &cap,int type);

	int AddAlgorithmCap(ENCDEC_CAPABILITY_ITEM  &cap);    

	int buildH235MediaCaps(HPVT valH,int nodeId,int iCapIndex,int h235EntryNumber,int iEntryNumber);
	
	//从加解密能力数组中查找
	//opetionType first is 0 exist is 1
	int FindCapFromAlgorithmCaps(ENCDEC_CAPABILITY_ITEM  &cap,int opetionType);


private:
	//加密能力信息数组
	EncDecAlgorithmCap m_EncDecAlgoCap;

	//当前能力算法 用于FindEntryFromCapsDescriptors
	ENCDEC_CAPABILITY_ITEM m_currentCap;

	//用于FindEntryFromCapsDescriptors
	int m_nFirstExist;

	//能力表总项数 初始为0
	int m_nCapTableCount;

	//能力描述表中中加密能力项数
	int m_nCapTableEntryCount;
	
	//以3开头的整数作为H235 EntryNumber
	int m_H235EntryNumber; 


};
