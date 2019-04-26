#include "EncryptionCapability.h"
#include "H323CallMgr.h"


#define H261_ENTRY   261
#define H263_ENTRY   263
#define H263P_ENTRY  2632
#define H263PP_ENTRY 2633


EncryptionCapability::EncryptionCapability(void)
{
	//当前加密算法
	m_currentCap.algoID=ALGORITHM_DES_CBC;
	m_nFirstExist=1;

	//能力表总项数 初始为0
	m_nCapTableCount=0;

	//能力描述表中中加密能力项数
	m_nCapTableEntryCount=0;

	m_H235EntryNumber=3000; //以3开头的整数作为H235 EntryNumber
	//notice 20120116 update
	m_nHighProfileLevel=H264_LEVEL_1080P;

	memset(&m_EncDecAlgoCap,0,sizeof(EncDecAlgorithmCap));
}

EncryptionCapability::~EncryptionCapability(void)
{
}

int EncryptionCapability::AddAlgorithmCap(ENCDEC_CAPABILITY_ITEM  &cap)                    
{
	int result = 0;
	int capNum=0;
	capNum=m_EncDecAlgoCap.m_nCapNum;
	memcpy(&(m_EncDecAlgoCap.m_Caps[capNum]),&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	capNum++;
	m_EncDecAlgoCap.m_nCapNum=capNum;
	result=capNum;
	return result; 
}

 //2首选 3移除
int EncryptionCapability::SetAlgorithmCap(ENCDEC_CAPABILITY_ITEM  &cap,int type)                     
{
	int result = -1;
	EncDecAlgorithmCap& capSet=m_EncDecAlgoCap;

	//-1
	type=type-1;

	//查找位置
	int capNum=capSet.m_nCapNum;
	for(int i=0;i<capNum;i++)
	{
		if(memcmp(capSet.m_Caps+i,&cap,sizeof(ENCDEC_CAPABILITY_ITEM))==0)
		{
			result=i;break;
		}
	}
	if(result==-1) return 0;       //没有查找到
	//设为首位
	if(type==1)
	{
		capNum=result;   
		AUDIO_CAPABILITY_ITEM capsTemp[MAX_ALGO_NUM];
		//COPY其前部分
		memcpy(capsTemp,capSet.m_Caps,sizeof(ENCDEC_CAPABILITY_ITEM)*capNum);
		memcpy(&(capSet.m_Caps[0]),&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
		memcpy(capSet.m_Caps+1,capsTemp,sizeof(ENCDEC_CAPABILITY_ITEM)*capNum);
		capNum=capSet.m_nCapNum;
	}
	//移除
	if(type==2)
	{
		ENCDEC_CAPABILITY_ITEM capsTemp[MAX_ALGO_NUM];
		//COPY其后部分
		memcpy(capsTemp,capSet.m_Caps+result+1,sizeof(ENCDEC_CAPABILITY_ITEM)*(capNum-result-1));
		memcpy(capSet.m_Caps+result,capsTemp,sizeof(ENCDEC_CAPABILITY_ITEM)*(capNum-result-1));
		capNum--;
		capSet.m_nCapNum=capNum;
	}
	result=capNum;
	return result; 
} 


int EncryptionCapability::RemoveAllCapabilitis()                   //移除所有能力集
{
	int result = 0;
	m_EncDecAlgoCap.m_nCapNum=0;
	m_AudioCap.m_nCapNum=0;
	m_VideoCap.m_nCapNum=0;
	m_H239VideoCap.m_nCapNum=0;
	m_DataCap.m_nCapNum=0;
	result=1;
	return result;

}
//opetionType first is 0 exist is 1
int EncryptionCapability::FindCapFromAlgorithmCaps(ENCDEC_CAPABILITY_ITEM  &cap,int opetionType)
{
	int result = 0;
	unsigned int i=0;
	EncDecAlgorithmCap& capSet=m_EncDecAlgoCap;
	//first is 0 exist is 1
	unsigned int capNum=0;
	if(opetionType==0 && capSet.m_nCapNum>0) capNum=1;
	if(opetionType==1) capNum=capSet.m_nCapNum;
	for(i=0;i<capNum;i++)
	{
		if(memcmp(capSet.m_Caps+i,&cap,sizeof(ENCDEC_CAPABILITY_ITEM))==0)
		{
			result=i+1;
			break;
		}
	}

	return result;
}


int EncryptionCapability::AddDES_cbcAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_DES_CBC;

	result=AddAlgorithmCap(cap); 
	return result; 
}

int EncryptionCapability::RemoveDES_cbcAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_DES_CBC;

	result=SetAlgorithmCap(cap,3); //1添加 2首选 3移除
	return result; 
}

int EncryptionCapability::SetFirstDES_cbcAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_DES_CBC;

	result=SetAlgorithmCap(cap,2); //1添加 2首选 3移除
	return result; 
}

int EncryptionCapability::IsExistDES_cbcAlgorithm()                      //能力是否存在
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_DES_CBC;
	
	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	result=FindCapFromAlgorithmCaps(cap,1);               
	return result; 
} 

int EncryptionCapability::IsFirstDES_cbcAlgorithm()                      //是否为同时能力项首选
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_DES_CBC;

	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	m_nFirstExist=0;
	result=FindCapFromAlgorithmCaps(cap,0); 		
	return result; 
} 




int EncryptionCapability::AddDES_ecbAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_DES_ECB;

	result=AddAlgorithmCap(cap); 
	return result; 
}

int EncryptionCapability::RemoveDES_ecbAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_DES_ECB;

	result=SetAlgorithmCap(cap,3); //1添加 2首选 3移除
	return result; 
}

int EncryptionCapability::SetFirstDES_ecbAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_DES_ECB;

	result=SetAlgorithmCap(cap,2); //1添加 2首选 3移除
	return result; 
}

int EncryptionCapability::IsExistDES_ecbAlgorithm()                      //能力是否存在
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_DES_ECB;
	
	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	result=FindCapFromAlgorithmCaps(cap,1);               
	return result; 
} 

int EncryptionCapability::IsFirstDES_ecbAlgorithm()                      //是否为同时能力项首选
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_DES_ECB;

	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	m_nFirstExist=0;
	result=FindCapFromAlgorithmCaps(cap,0); 		
	return result; 
} 





int EncryptionCapability::AddTripeDES_cbcAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_TRIPLE_DES_CBC;

	result=AddAlgorithmCap(cap); 
	return result; 
}

int EncryptionCapability::RemoveTripeDES_cbcAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_TRIPLE_DES_CBC;

	result=SetAlgorithmCap(cap,3); //1添加 2首选 3移除
	return result; 
}

int EncryptionCapability::SetFirstTripeDES_cbcAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_TRIPLE_DES_CBC;

	result=SetAlgorithmCap(cap,2); //1添加 2首选 3移除
	return result; 
}

int EncryptionCapability::IsExistTripeDES_cbcAlgorithm()                      //能力是否存在
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_TRIPLE_DES_CBC;
	
	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	result=FindCapFromAlgorithmCaps(cap,1);               
	return result; 
} 

int EncryptionCapability::IsFirstTripeDES_cbcAlgorithm()                      //是否为同时能力项首选
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_TRIPLE_DES_CBC;

	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	m_nFirstExist=0;
	result=FindCapFromAlgorithmCaps(cap,0); 		
	return result; 
} 




int EncryptionCapability::AddTripeDES_ecbAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_TRIPLE_DES_ECB;

	result=AddAlgorithmCap(cap); 
	return result; 
}

int EncryptionCapability::RemoveTripeDES_ecbAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_TRIPLE_DES_ECB;

	result=SetAlgorithmCap(cap,3); //1添加 2首选 3移除
	return result; 
}

int EncryptionCapability::SetFirstTripeDES_ecbAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_TRIPLE_DES_ECB;

	result=SetAlgorithmCap(cap,2); //1添加 2首选 3移除
	return result; 
}

int EncryptionCapability::IsExistTripeDES_ecbAlgorithm()                      //能力是否存在
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_TRIPLE_DES_ECB;
	
	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	result=FindCapFromAlgorithmCaps(cap,1);               
	return result; 
} 

int EncryptionCapability::IsFirstTripeDES_ecbAlgorithm()                      //是否为同时能力项首选
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_TRIPLE_DES_ECB;

	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	m_nFirstExist=0;
	result=FindCapFromAlgorithmCaps(cap,0); 		
	return result; 
} 



int EncryptionCapability::AddAES_cbcAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_AES_CBC;

	result=AddAlgorithmCap(cap); 
	return result; 
}

int EncryptionCapability::RemoveAES_cbcAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_AES_CBC;

	result=SetAlgorithmCap(cap,3); //1添加 2首选 3移除
	return result; 
}

int EncryptionCapability::SetFirstAES_cbcAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_AES_CBC;

	result=SetAlgorithmCap(cap,2); //1添加 2首选 3移除
	return result; 
}

int EncryptionCapability::IsExistAES_cbcAlgorithm()                      //能力是否存在
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_AES_CBC;
	
	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	result=FindCapFromAlgorithmCaps(cap,1);               
	return result; 
} 

int EncryptionCapability::IsFirstAES_cbcAlgorithm()                      //是否为同时能力项首选
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_AES_CBC;

	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	m_nFirstExist=0;
	result=FindCapFromAlgorithmCaps(cap,0); 		
	return result; 
} 




int EncryptionCapability::AddAES256_cbcAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_AES256_CBC;

	result=AddAlgorithmCap(cap); 
	return result; 
}

int EncryptionCapability::RemoveAES256_cbcAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_AES256_CBC;

	result=SetAlgorithmCap(cap,3); //1添加 2首选 3移除
	return result; 
}

int EncryptionCapability::SetFirstAES256_cbcAlgorithm()
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_AES256_CBC;

	result=SetAlgorithmCap(cap,2); //1添加 2首选 3移除
	return result; 
}

int EncryptionCapability::IsExistAES256_cbcAlgorithm()                      //能力是否存在
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_AES256_CBC;

	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	result=FindCapFromAlgorithmCaps(cap,1);               
	return result; 
} 

int EncryptionCapability::IsFirstAES256_cbcAlgorithm()                      //是否为同时能力项首选
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));

	cap.algoID=ALGORITHM_AES256_CBC;

	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	m_nFirstExist=0;
	result=FindCapFromAlgorithmCaps(cap,0); 		
	return result; 
} 



int EncryptionCapability::IsExistFromAlgorithm(EncDecAlgorithmID	algoID)                      //能力是否存在
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));
	cap.algoID=algoID;
	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	m_nFirstExist=0;
	result=FindCapFromAlgorithmCaps(cap,1); 		
	return result; 
}

int EncryptionCapability::IsFirstFromAlgorithm(EncDecAlgorithmID	algoID)                      //是否为同时能力项首选
{
	int result = 0;
	ENCDEC_CAPABILITY_ITEM  cap;
	memset(&cap,0,sizeof(ENCDEC_CAPABILITY_ITEM));
	cap.algoID=algoID;
	memcpy(&m_currentCap,&cap,sizeof(ENCDEC_CAPABILITY_ITEM));
	m_nFirstExist=0;
	result=FindCapFromAlgorithmCaps(cap,0); 		
	return result; 
}



int EncryptionCapability::IsExistSimul(int type ,int iEntryNumber)
{
	int result = 0;
	//result=FindEntryFromCapsDescriptors(type ,iEntryNumber,1);
	result=FindEntryFromCapsArray(type,iEntryNumber,1);
	return 	result;

} 

int EncryptionCapability::IsFirstSimul(int type ,int iEntryNumber)
{
	int result = 0;	
	//result=FindEntryFromCapsDescriptors(type ,iEntryNumber,0);
	result=FindEntryFromCapsArray(type,iEntryNumber,0);
	return result;
}
//opetionType first is 0 exist is 1
int EncryptionCapability::FindEntryFromCapsArray(int type ,int iEntryNumber,int opetionType)
{
	int result = 0;
	//1 audio 2 video 3 H239 4 Data
	unsigned int i=0;
	if(type==1)
	{
		X_AUDIO_CODEC_ID audioCodecID=(X_AUDIO_CODEC_ID)-1;

		switch(iEntryNumber)
		{
		case G729A_ENTRY:
			audioCodecID = X_AUDIO_CODEC_G729;
			break;
		case G711A_ENTRY_64K: 
			audioCodecID = X_AUDIO_CODEC_G711A;
			break; 
		case G711U_ENTRY_64K: 
			audioCodecID = X_AUDIO_CODEC_G711U;
			break; 
		case G7221_ENTRY_24K: 
			audioCodecID = X_AUDIO_CODEC_G7221_7_24;
			break; 
		case G7221_ENTRY_32K: 
			audioCodecID = X_AUDIO_CODEC_G7221_7_32;
			break; 
		case G7221C_ENTRY_24K: 
			audioCodecID = X_AUDIO_CODEC_G7221_14_24;
			break; 
		case G7221C_ENTRY_32K: 
			audioCodecID = X_AUDIO_CODEC_G7221_14_32;
			break;
		case G7221C_ENTRY_48K: 
			audioCodecID = X_AUDIO_CODEC_G7221_14_48;
			break;
		case G722_ENTRY_64K: 
			audioCodecID = X_AUDIO_CODEC_G722_64;
			break;
			//notice 20120116 update
		case G719_ENTRY_32K: 
			audioCodecID = X_AUDIO_CODEC_G719_32;
			break;
		case G719_ENTRY_48K: 
			audioCodecID = X_AUDIO_CODEC_G719_48;
			break;
			//notice add 20120202
		case GSM_ENTRY: 
			audioCodecID = X_AUDIO_CODEC_GSM;
			break;
		case G719_ENTRY_64K: 
			audioCodecID = X_AUDIO_CODEC_G719_64;
			break; 
			//case GSM_ENTRY: 
			//	audioCodecID = X_AUDIO_CODEC_GSM;
			//	break;
			//case G7231_ENTRY: 
			//	audioCodecID = X_AUDIO_CODEC_G7231_63;
			//	break; 
		default:break;
		}

		AudioCap& capSet=m_AudioCap;

		//first is 0 //exist is 1
		unsigned int count=0;
		if(opetionType==0 && capSet.m_nCapNum>0) count=1;
		if(opetionType==1) count=capSet.m_nCapNum;
		for(i=0;i<count;i++)
		{
			if(capSet.m_Caps[i].AudioCodecID==audioCodecID) 
			{
				result=iEntryNumber;
				break;
			}
		}		
	}

	if(type==2 || type==3)
	{
		VIDEC_CODEC_TYPE videoCodecType =(VIDEC_CODEC_TYPE)-1;
		int H264Level=0;

		if(type==3) 
		{

			if( (iEntryNumber>=(H264_ENTRY_QCIF+20000) ) && (iEntryNumber<=(/*H264_ENTRY_HIG720P*/ H264_ENTRY_HIG1080P +20000) ) )
				iEntryNumber=iEntryNumber-20000;
			if( iEntryNumber<22641)
			{
				if(iEntryNumber==2261) iEntryNumber=261;
				if(iEntryNumber==2263) iEntryNumber=263;
				if(iEntryNumber==22632) iEntryNumber=2632;
				if(iEntryNumber==22633) iEntryNumber=2633;
			}
		}

		switch(iEntryNumber)
		{
		case H264_ENTRY_QCIF:
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_LEVEL_QCIF;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_CIF:
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_LEVEL_CIF;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_4CIF:
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_LEVEL_4CIF;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_16CIF:
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_LEVEL_16CIF;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_720I: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_LEVEL_720I;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_720P: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_LEVEL_720P;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_1080I: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_LEVEL_1080I;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_1080P: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_LEVEL_1080P;  //H264此用于LEVEL
			break;
		case H264_ENTRY_VGA: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_LEVEL_VGA;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_SVGA: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_LEVEL_SVGA;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_XGA: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_LEVEL_XGA;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_SXGA: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_LEVEL_SXGA;  //H264此用于LEVEL
			break;
		case H264_ENTRY_HIG720P: 
			videoCodecType = VIDEC_CODEC_H264_SVC;
			H264Level = H264_LEVEL_720P;  //H264此用于LEVEL
			break;
		case H264_ENTRY_HIG1080P: 
			videoCodecType = VIDEC_CODEC_H264_SVC;
			H264Level = H264_LEVEL_1080P;  //H264此用于LEVEL
			break;
		case H263_ENTRY: 
			videoCodecType = VIDEC_CODEC_H263;
			break; 
		case  H263P_ENTRY: 
			videoCodecType = VIDEC_CODEC_H263P;
			break; 
		case H263PP_ENTRY: 
			videoCodecType = VIDEC_CODEC_H263PP;
			break; 
		case H261_ENTRY: 
			videoCodecType = VIDEC_CODEC_H261;
			break;
		default:break;
		}


		if(type==2)
		{
			VideoCap& capSet=m_VideoCap;

			//first is 0 //exist is 1
			unsigned int count=0;
			if(opetionType==0 && capSet.m_nCapNum>0) count=1;
			if(opetionType==1) count=capSet.m_nCapNum;
			for(i=0;i<count;i++)		
			{
				if(capSet.m_Caps[i].VideoCodecType==videoCodecType) 
				{
					//notice 20120116 update
					if( (videoCodecType == VIDEC_CODEC_H264 ) || (videoCodecType == VIDEC_CODEC_H264_SVC ) )
					{
						//if (capSet.m_Caps[i].n4CIF>=H264Level) //h264 level = is
						if (capSet.m_Caps[i].n4CIF >0 && H264Level >0) //h264 level >0 is
						{
							result=iEntryNumber;
							break;
						}
					}
					else
					{
						result=iEntryNumber;
						break;
					}
				}
			}

		}

		if(type==3) 
		{
			H239VideoCap& capSet=m_H239VideoCap;

			//first is 0 exist is 1
			unsigned int count=0;
			if(opetionType==0 && capSet.m_nCapNum>0) count=1;
			if(opetionType==1) count=capSet.m_nCapNum;
			for(i=0;i<count;i++)		
			{
				if(capSet.m_Caps[i].VideoCodecType==videoCodecType) 
				{
					//notice 20120116 update
					if( (videoCodecType == VIDEC_CODEC_H264 ) || (videoCodecType == VIDEC_CODEC_H264_SVC ) )
					{
						//if (capSet.m_Caps[i].n4CIF>=H264Level) //h264 level = is
						if (capSet.m_Caps[i].n4CIF >0 && H264Level >0) //h264 level >0 is
						{
							result=iEntryNumber;
							break;
						}
					}
					else
					{
						result=iEntryNumber;
						break;
					}
				}
			}

		}
	}


	if(type==4) 
	{
		DATA_CODEC_ID dataCodecId=DATA_CODEC_H224;
		DataCap& capSet=m_DataCap;

		//first is 0 exist is 1
		unsigned int count=0;
		if(opetionType==0 && capSet.m_nCapNum>0) count=1;
		if(opetionType==1) count=capSet.m_nCapNum;
		for(i=0;i<count;i++)
		{
			if(capSet.m_Caps[i].DataCodecID==dataCodecId) 
			{
				result=iEntryNumber;
				break;
			}
		}
	}

	return result; 
}

int EncryptionCapability::IsExistCodecTypeFromCapsArray(int type ,int codecType,int nLevel)
{
	int result = 0;
	result=FindCodecTypeFromCapsArray(type ,codecType,nLevel,1);
	return result;
}

int EncryptionCapability::IsFirstCodecTypeFromCapsArray(int type ,int codecType,int nLevel)
{
	int result = 0;
	result=FindCodecTypeFromCapsArray(type ,codecType,nLevel,0);
	return result;
}

//opetionType first is 0 exist is 1
int EncryptionCapability::FindCodecTypeFromCapsArray(int type ,int codecType,int nLevel,int opetionType)
{
	int result = 0;
	//1 audio 2 video 3 H239 4 Data
	unsigned int i=0;
	if(type==1)
	{
		X_AUDIO_CODEC_ID audioCodecID=(X_AUDIO_CODEC_ID)codecType;
		AudioCap& capSet=m_AudioCap;
		//first is 0 exist is 1
		unsigned int count=0;
		if(opetionType==0 && capSet.m_nCapNum>0) count=1;
		if(opetionType==1) count=capSet.m_nCapNum;
		for(i=0;i<count;i++)
		{
			if(capSet.m_Caps[i].AudioCodecID==audioCodecID) 
			{
				result=i+1;
				break;
			}
		}		
	}

	if(type==2 || type==3)
	{
		VIDEC_CODEC_TYPE videoCodecType =(VIDEC_CODEC_TYPE)codecType;
		int H264Level=nLevel;		
		if(type==2)
		{
			VideoCap& capSet=m_VideoCap;

			//first is 0 exist is 1
			unsigned int count=0;
			if(opetionType==0 && capSet.m_nCapNum>0) count=1;
			if(opetionType==1) count=capSet.m_nCapNum;
			for(i=0;i<count;i++)		
			{
				if(capSet.m_Caps[i].VideoCodecType==videoCodecType) 
				{
					//notice 20120116 update
					if( (videoCodecType == VIDEC_CODEC_H264 ) || (videoCodecType == VIDEC_CODEC_H264_SVC ) )
					{
						//if (capSet.m_Caps[i].n4CIF >= H264Level)//h264 level = is
						if (capSet.m_Caps[i].n4CIF >0 && H264Level >0)//h264 level >0 is
						{
							result=i+1;
							break;
						}
					}
					else	
					{
						result=i+1;
						break;
					}

				}
			}

		}

		if(type==3) 
		{
			H239VideoCap& capSet=m_H239VideoCap;

			//first is 0 exist is 1
			unsigned int count=0;
			if(opetionType==0 && capSet.m_nCapNum>0) count=1;
			if(opetionType==1) count=capSet.m_nCapNum;
			for(i=0;i<count;i++)		
			{
				if(capSet.m_Caps[i].VideoCodecType==videoCodecType) 
				{
					//notice 20120116 update
					if( (videoCodecType == VIDEC_CODEC_H264 ) || (videoCodecType == VIDEC_CODEC_H264_SVC ) )					 
					{
						//if (capSet.m_Caps[i].n4CIF >= H264Level)//h264 level = is
						if (capSet.m_Caps[i].n4CIF >0 && H264Level >0)//h264 level >0 is
						{
							result=i+1;
							break;
						}
					}
					else					
					{
						result=i+1;
						break;
					}
				}
			}

		}
	}


	if(type==4) 
	{
		DATA_CODEC_ID dataCodecId=(DATA_CODEC_ID)codecType;
		DataCap& capSet=m_DataCap;

		//first is 0 exist is 1
		unsigned int count=0;
		if(opetionType==0 && capSet.m_nCapNum>0) count=1;
		if(opetionType==1) count=capSet.m_nCapNum;
		for(i=0;i<count;i++)
		{
			if(capSet.m_Caps[i].DataCodecID==dataCodecId) 
			{
				result=i+1;
				break;
			}
		}
	}

	return result; 
}





int EncryptionCapability::buildH235MediaCaps(HPVT valH,int nodeId,int iCapIndex,int h235EntryNumber,int iEntryNumber)
{
	char tempbuf[1024];
	char temp[512];
	int error=-1;
	int k=0;
	
	sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
	error = pvtBuildByPath(valH, nodeId, tempbuf, h235EntryNumber, NULL);

	sprintf(temp, "capabilityTable.%d.capability.h235SecurityCapability", iCapIndex);
	sprintf(tempbuf, "%s.mediaCapability", temp);
	error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);

	int encAlgorithmCount=0;
	encAlgorithmCount=m_EncDecAlgoCap.m_nCapNum;
	EncDecAlgorithmID algId=ALGORITHM_NONE;

	unsigned char pValue[64]={0};
	int pValueLen=64;

	for(k=1;k<=encAlgorithmCount;k++)
	{
		sprintf(tempbuf, "%s.encryptionAuthenticationAndIntegrity.encryptionCapability.%d.algorithm", temp,k);	
		algId=m_EncDecAlgoCap.m_Caps[k-1].algoID;

		pValueLen=64;
		if(H323CallMgr::Instance().GetEncDecAlgorithmOIDFromType(algId,pValue,pValueLen))
			error = pvtBuildByPath(valH, nodeId, tempbuf, pValueLen, (char*)pValue);
	}

	if(error<0) error=0;
	return error;
}

//添加所有能力集表中媒体能力项的加密能力参数
int EncryptionCapability::BuildAllMediaEncryptionCapabilitis(int node,CallCapability &callCap)
{
	memcpy(&m_AudioCap,&callCap.m_AudioCap,sizeof(AudioCap));
	memcpy(&m_VideoCap,&callCap.m_VideoCap,sizeof(VideoCap));
	memcpy(&m_H239VideoCap,&callCap.m_H239VideoCap,sizeof(H239VideoCap));
	memcpy(&m_DataCap,&callCap.m_DataCap,sizeof(DataCap));
	return InitCapabilitis(node);
}


int EncryptionCapability::InitCapabilitis(int node)
{
	char tempbuf[1024];
	int nodeId=m_NodeId=node;
	HPVT valH = cmGetValTree( H323CallMgr::Instance().m_hApp ); 
	int error=-1, iEntryNumber = 0, iCapIndex=0;
	int nSimulNum = 1;
	int nSimultaneous = 1;
	int iAlternative = 1;
	int h235EntryNumber=0;	
	bool bCapAdd=false;
	bool bCapAddItem=false;
	int  nAddCapCount=0;
	unsigned int i;
	//取出能力集表项数
	iCapIndex=0;
	while(1)
	{
		error=-1;
		iCapIndex++;
		sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
		error=pvtGetNodeIdByPath(valH,nodeId,tempbuf);
		if(error<0) 
		{
			iCapIndex-=1;
			break;		
		}
	} 

	m_nCapTableCount=iCapIndex;

	iEntryNumber = 0 ;
	h235EntryNumber=m_H235EntryNumber;


	//注 检测是否实际添加了项
	nSimultaneous = 0;
	while(1)
	{
		error=-1;
		nSimultaneous++;
		sprintf(tempbuf,"capabilityDescriptors.1.simultaneousCapabilities.%d.1",nSimultaneous);
		error=pvtGetNodeIdByPath(valH,nodeId,tempbuf);
		if(error<0) break;
	}
	if(nSimultaneous==1) return 0;

	nSimultaneous = 1;
	//设置音频
	//iAlternative = 1;
	iAlternative = 0;
	while(1)
	{
		error=-1;
		iAlternative++;
		sprintf(tempbuf,"capabilityDescriptors.1.simultaneousCapabilities.%d.%d",nSimultaneous,iAlternative);
		error=pvtGetNodeIdByPath(valH,nodeId,tempbuf);
		if(error<0) break;
	}

	bCapAddItem=false;

	for(i=0; i<m_AudioCap.m_nCapNum; i++)
	{
		bCapAdd=false;
		switch( m_AudioCap.m_Caps[i].AudioCodecID )
		{
		case X_AUDIO_CODEC_G711A:			
			iEntryNumber = G711A_ENTRY_64K;
			bCapAdd=true;
			break;
		case X_AUDIO_CODEC_G711U:
			iEntryNumber = G711U_ENTRY_64K;
			bCapAdd=true;
			break;
		case X_AUDIO_CODEC_G722_64:
			iEntryNumber = G722_ENTRY_64K;
			bCapAdd=true;
			break;
		case X_AUDIO_CODEC_G7221_7_24:
			iEntryNumber = G7221_ENTRY_24K;
			bCapAdd=true;
			break;
		case X_AUDIO_CODEC_G7221_7_32:
			iEntryNumber = G7221_ENTRY_32K;
			bCapAdd=true;
			break;
		case X_AUDIO_CODEC_G7221_14_24:
			iEntryNumber = G7221C_ENTRY_24K;
			bCapAdd=true;
			break;
		case X_AUDIO_CODEC_G7221_14_32:
			iEntryNumber = G7221C_ENTRY_32K;
			bCapAdd=true;
			break;
		case X_AUDIO_CODEC_G7221_14_48:
			iEntryNumber = G7221C_ENTRY_48K;
			bCapAdd=true;
			break;
			//notice 20120116 update
		case X_AUDIO_CODEC_G719_32:
			iEntryNumber = G719_ENTRY_32K;
			bCapAdd=true;
			break;
		case X_AUDIO_CODEC_G719_48:
			iEntryNumber = G719_ENTRY_48K;
			bCapAdd=true;
			break;
		case X_AUDIO_CODEC_G719_64:
			iEntryNumber = G719_ENTRY_64K;
			bCapAdd=true;
			break;
			//notice add 20120202
		case X_AUDIO_CODEC_GSM: 
			iEntryNumber = GSM_ENTRY;
			bCapAdd=true;
			break;
		//case X_AUDIO_CODEC_G7231_63:
		//	iEntryNumber = G7231_ENTRY;
		//	bCapAdd=true;
		//	break;
		case X_AUDIO_CODEC_G729:
			iEntryNumber = G729A_ENTRY;
			{
				iCapIndex ++;
				h235EntryNumber++;
				buildH235MediaCaps(valH,nodeId,iCapIndex,h235EntryNumber,iEntryNumber);
				//设置同时能力集
				sprintf(tempbuf,"capabilityDescriptors.%d.simultaneousCapabilities.%d.%d",nSimulNum, nSimultaneous,iAlternative);
				error = pvtBuildByPath( valH, nodeId,tempbuf,h235EntryNumber, NULL); 
				if(error >= 0) 
				{
					iAlternative++;
				}
			}
			//add second times
			iEntryNumber = G729_ENTRY;			
			bCapAdd=true;
			break; 
		default:
			break;
		}

		if(bCapAdd)
		{
			iCapIndex ++;
			h235EntryNumber++;
			buildH235MediaCaps(valH,nodeId,iCapIndex,h235EntryNumber,iEntryNumber);
			//设置同时能力集
			sprintf(tempbuf,"capabilityDescriptors.%d.simultaneousCapabilities.%d.%d",nSimulNum, nSimultaneous,iAlternative);
			error = pvtBuildByPath( valH, nodeId,tempbuf,h235EntryNumber, NULL); 
			if(error >= 0) 
			{
				iAlternative++;
				nAddCapCount++;
				bCapAddItem=true;
			}
		}	
	}

	if(bCapAddItem) nSimultaneous++;

	//设置视频
	bCapAddItem=false;
	iAlternative = 0;
	while(1)
	{
		error=-1;
		iAlternative++;
		sprintf(tempbuf,"capabilityDescriptors.1.simultaneousCapabilities.%d.%d",nSimultaneous,iAlternative);
		error=pvtGetNodeIdByPath(valH,nodeId,tempbuf);
		if(error<0) break;
	}

	for(i=0; i<m_VideoCap.m_nCapNum; i++)
	{
		VIDEO_CAPABILITY_ITEM& capVideo = m_VideoCap.m_Caps[i];
		bCapAdd=false;
		switch( capVideo.VideoCodecType )
		{ 
		case VIDEC_CODEC_H261:				
			iEntryNumber = H261_ENTRY;
			bCapAdd=true;
			break;
		case VIDEC_CODEC_H263:				
			iEntryNumber = H263_ENTRY;
			bCapAdd=true;
			break;
		case VIDEC_CODEC_H263P:						
			iEntryNumber = H263P_ENTRY;
			bCapAdd=true;
			break;
		case VIDEC_CODEC_H263PP:			
			iEntryNumber = H263PP_ENTRY;
			bCapAdd=true;
			break;
			//notice 20120116 update
		case VIDEC_CODEC_H264_SVC:
		case VIDEC_CODEC_H264:				
			iEntryNumber = capVideo.nQCIF;
			bCapAdd=true;
			break;
		default:
			break;
		}
		iEntryNumber =GetVideoCapEntry(capVideo.VideoCodecType,capVideo.VideoFormatType,0);
		if(bCapAdd)
		{
			iCapIndex ++;
			h235EntryNumber++;
			buildH235MediaCaps(valH,nodeId,iCapIndex,h235EntryNumber,iEntryNumber);
			//设置同时能力集
			sprintf(tempbuf,"capabilityDescriptors.%d.simultaneousCapabilities.%d.%d",nSimulNum, nSimultaneous,iAlternative);
			error = pvtBuildByPath( valH, nodeId,tempbuf,h235EntryNumber, NULL); 
			if(error >= 0) 
			{
				iAlternative++;
				nAddCapCount++;
				bCapAddItem=true;
			}
		}	
	}

	if(bCapAddItem) nSimultaneous++;

	//设置双流
	bCapAddItem=false;
	iAlternative = 0;
	while(1)
	{
		error=-1;
		iAlternative++;
		sprintf(tempbuf,"capabilityDescriptors.1.simultaneousCapabilities.%d.%d",nSimultaneous,iAlternative);
		error=pvtGetNodeIdByPath(valH,nodeId,tempbuf);
		if(error<0) break;
	}

	for(i=0; i<m_H239VideoCap.m_nCapNum; i++)
	{
		VIDEO_CAPABILITY_ITEM& capVideo = m_H239VideoCap.m_Caps[i];

		bCapAdd=false;
		switch( capVideo.VideoCodecType )
		{ 
		case VIDEC_CODEC_H261:				
			iEntryNumber =2000 + H261_ENTRY;
			bCapAdd=true;
			break;
		case VIDEC_CODEC_H263:				
			iEntryNumber =2000 + H263_ENTRY;
			bCapAdd=true;
			break;
		case VIDEC_CODEC_H263P:						
			iEntryNumber = 20000 + H263P_ENTRY;
			bCapAdd=true;
			break;
		case VIDEC_CODEC_H263PP:			
			iEntryNumber = 20000 + H263PP_ENTRY;
			bCapAdd=true;
			break;
			//notice 20120116 update
		case VIDEC_CODEC_H264_SVC:
		case VIDEC_CODEC_H264:				
			iEntryNumber = capVideo.nQCIF;
			bCapAdd=true;
			break;
		default:
			break;
		}

		iEntryNumber =GetVideoCapEntry(capVideo.VideoCodecType,capVideo.VideoFormatType,1);

		if(bCapAdd)
		{
			iCapIndex ++;
			h235EntryNumber++;
			buildH235MediaCaps(valH,nodeId,iCapIndex,h235EntryNumber,iEntryNumber);
			//设置同时能力集
			sprintf(tempbuf,"capabilityDescriptors.%d.simultaneousCapabilities.%d.%d",nSimulNum, nSimultaneous,iAlternative);
			error = pvtBuildByPath( valH, nodeId,tempbuf,h235EntryNumber, NULL); 
			if(error >= 0) 
			{
				iAlternative++;
				nAddCapCount++;
				bCapAddItem=true;
			}

		}	
	}

	if(bCapAddItem) nSimultaneous++;

	//设置远遥
	bCapAddItem=false;
	iAlternative = 0;
	while(1)
	{
		error=-1;
		iAlternative++;
		sprintf(tempbuf,"capabilityDescriptors.1.simultaneousCapabilities.%d.%d",nSimultaneous,iAlternative);
		error=pvtGetNodeIdByPath(valH,nodeId,tempbuf);
		if(error<0) break;
	}

	for(i=0; i<m_DataCap.m_nCapNum; i++)
	{			
		switch(m_DataCap.m_Caps[i].DataCodecID)
		{
		case DATA_CODEC_H224:
			{
				iEntryNumber = H224_ENTRY;
				iCapIndex ++;
				h235EntryNumber++;
				buildH235MediaCaps(valH,nodeId,iCapIndex,h235EntryNumber,iEntryNumber);
				//设置同时能力集
				sprintf(tempbuf,"capabilityDescriptors.%d.simultaneousCapabilities.%d.%d",nSimulNum, nSimultaneous,iAlternative);
				error = pvtBuildByPath( valH, nodeId,tempbuf,h235EntryNumber, NULL); 
				if(error >= 0) 
				{
					iAlternative++;
					nAddCapCount++;
					bCapAddItem=true;
				}
			}
			break;
		default:
			break;
		}
	}

	m_nCapTableEntryCount=iAlternative-1;

	if(error<0) error=0;
	return error;

}


//firExi find  EncryptionDec algorithm is exist or first , 1 exist , 0 is first 
int EncryptionCapability::FindEntryFromCapsDescriptors(int type ,int iEntryNumber,int firExi)
{
	int result = 0;
	char tempbuf[200];
	int nodeId=m_NodeId;
	HPVT valH = cmGetValTree( H323CallMgr::Instance().m_hApp ); 
	int nSimulNum = 1;
	int nSimultaneous = 5;    //1 audio 2 video 3 H239 4 Data 5 h235
	int iAlternative = 1;
	int nValue=0;
	int nRet=0;
	int error=-1;
	RvPstFieldId	nField;
	RvBool			bString;
	int count=0;
	int h235EntryNumber=0;
	bool find=false;
	unsigned char encOid[128]={0};
	int algorithmLen=0;
	int nSimuBack=0;

		
	//添加时注意保存m_nCapabilityTableCount值，如果为0则返回不再添加 并保存描述表中加密能力项数capabilityTableEntryNumber

	//注 检测是否实际添加了项
	nSimultaneous = 0;
	while(1)
	{
		error=-1;
		nSimultaneous++;
		sprintf(tempbuf,"capabilityDescriptors.1.simultaneousCapabilities.%d.1",nSimultaneous);
		error=pvtGetNodeIdByPath(valH,nodeId,tempbuf);
		if(error<0) break;
	}
	if(nSimultaneous==1) return 0;

	nSimuBack=nSimultaneous;
	nSimultaneous=0;
	for(int nSimu=1;nSimu < nSimuBack;nSimu++)
	{
		nSimultaneous++;
		//find descriptors h235EntryNumber
		iAlternative = 1;
		while(1)
		{
			sprintf(tempbuf,"capabilityDescriptors.%d.simultaneousCapabilities.%d.%d",nSimulNum, nSimultaneous,iAlternative);
			nRet=pvtGetByPath(valH,nodeId,tempbuf,&nField,&nValue,&bString);
			if(nRet>=0)
			{
				h235EntryNumber = nValue;
				//find algorithm and iEntryNumber
				count=1;
				int k=1;
				while(count<=m_nCapTableCount)
				{
					sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", count);
					nRet=pvtGetByPath(valH,nodeId,tempbuf,&nField,&nValue,&bString);
					if( (nRet>=0)  && (nValue==h235EntryNumber) )
					{
						//find iEntryNumber
						sprintf(tempbuf, "capabilityTable.%d.capability.h235SecurityCapability.mediaCapability", count);
						nRet=pvtGetByPath(valH,nodeId,tempbuf,&nField,&nValue,&bString);
						if(nRet>=0)
						{
							if(iEntryNumber==nValue) find=true;
						}
						//find algorithm
						if(find)
						{
							find=false;
							k=1;
							while(1)
							{
								sprintf(tempbuf, "capabilityTable.%d.capability.h235SecurityCapability.encryptionAuthenticationAndIntegrity.encryptionCapability.%d.algorithm", count,k);	
								nRet=pvtGetByPath(valH,nodeId,tempbuf,&nField,&nValue,&bString);
								if(nRet<0) break;
								if( nRet && bString && nValue>0 )
								{
									unsigned char strData[256];
									if( pvtGetString( valH, nRet, nValue, (char*)strData ) > 0)
									{
										//找到算法
										EncDecAlgorithmID algo=ALGORITHM_NONE;
										if(H323CallMgr::Instance().GetEncDecAlgorithmTypeFromOID(algo,strData,nValue)!=0)
											if(algo==m_currentCap.algoID)
											{
												find=true;
												break;
											}
									}
								}
								//查找首选算法
								if(firExi==0) break;

								k++;
							}
						}
						if(find) break;
					}
					count++;		
				}
			}
			else break;
			iAlternative++;
			if(find) break;
		}

		if(find)
		{
			result=iEntryNumber;
			break;
		}
	}

	return result;
} 
