#include "CallCapability.h"

#ifdef HAS_H323
#include "H323CallMgr.h"
#endif

////H264 Level Value  因适应RADVision ViaIp MCU 720P作出改变
int H264_LEVEL_QCIF = 22 ;     //1.1 30fps
int H264_LEVEL_CIF  = 36 ;     //1.3 30fps
int H264_LEVEL_4CIF = 57 ;     //3   25fps //RMX20007.0.2 64 -->57
int H264_LEVEL_16CIF =85  ;    //4   39fps
int H264_LEVEL_720I = 71 ;     //3.1 60fps ?
int H264_LEVEL_720P = 71 ;     //3.1 30fps
int H264_LEVEL_1080I =78 ;     //3.2 60fps ?
int H264_LEVEL_1080P= 85 ;     //4   30fps
int H264_LEVEL_VGA  = 64 ;     //3   34fps
int H264_LEVEL_SVGA = 71 ;     //3.1 57fps
int H264_LEVEL_XGA  = 71 ;     //3.1 35fps
int H264_LEVEL_SXGA = 78 ;     //3.2 42fps

CallCapability::CallCapability(void)
{
	//因为要使用内存段比较是否相同 所有要初始化为0
	memset(&m_AudioCap,0,sizeof(AudioCap));				    
	memset(&m_VideoCap,0,sizeof(VideoCap));				    
	memset(&m_H239VideoCap,0,sizeof(H239VideoCap));				    
	memset(&m_DataCap,0,sizeof(DataCap));				    
	memset(m_videoParam,0,sizeof(m_videoParam) );
	memset(m_dualParam,0,sizeof(m_dualParam) );

	m_NodeId = 0;
	m_nHighProfileLevel=H264_LEVEL_1080P;

	int capNum = 0;
	//设置本端能力集
	m_AudioCap.m_Caps[capNum].AudioCodecID = X_AUDIO_CODEC_G729;
	capNum++;
	m_AudioCap.m_Caps[capNum].AudioCodecID = X_AUDIO_CODEC_G711A;
	capNum++;
	m_AudioCap.m_Caps[capNum].AudioCodecID = X_AUDIO_CODEC_G711U;
	capNum++;
	m_AudioCap.m_Caps[capNum].AudioCodecID = X_AUDIO_CODEC_G7221_7_24;
	capNum++;
	m_AudioCap.m_Caps[capNum].AudioCodecID = X_AUDIO_CODEC_G7221_7_32;
	capNum++;
	m_AudioCap.m_Caps[capNum].AudioCodecID = X_AUDIO_CODEC_G7221_14_24;
	capNum++;
	m_AudioCap.m_Caps[capNum].AudioCodecID = X_AUDIO_CODEC_G7221_14_32;
	capNum++;
	m_AudioCap.m_Caps[capNum].AudioCodecID = X_AUDIO_CODEC_G7221_14_48;
	capNum++;
	m_AudioCap.m_Caps[capNum].AudioCodecID = X_AUDIO_CODEC_G722_64;
	capNum++;
	m_AudioCap.m_nCapNum = capNum;


	capNum = 0;

	m_VideoCap.m_Caps[capNum].VideoCodecType = VIDEC_CODEC_H264;
	m_VideoCap.m_Caps[capNum].nMaxBitrate = H264_MBPS_QCIF;	
	m_VideoCap.m_Caps[capNum].n4CIF = H264_LEVEL_QCIF;  //H264此用于LEVEL
	m_VideoCap.m_Caps[capNum].nQCIF = H264_ENTRY_QCIF;  //H264此用于ENTRY
	m_VideoCap.m_Caps[capNum].nCIF = CAP_H264_QCIF;     //H264此用于CAP TYPE
	capNum++;


	m_VideoCap.m_Caps[capNum].VideoCodecType = VIDEC_CODEC_H264;
	m_VideoCap.m_Caps[capNum].nMaxBitrate = H264_MBPS_CIF;	  
	m_VideoCap.m_Caps[capNum].n4CIF = H264_LEVEL_CIF;    //H264此用于LEVEL
	m_VideoCap.m_Caps[capNum].nQCIF = H264_ENTRY_CIF;    //H264此用于ENTRY
	m_VideoCap.m_Caps[capNum].nCIF = CAP_H264_CIF;       //H264此用于CAP TYPE
	capNum++;

	m_VideoCap.m_Caps[capNum].VideoCodecType = VIDEC_CODEC_H263;
	m_VideoCap.m_Caps[capNum].nMaxBitrate = H263_MBPS_ALL;	//384Kbps
	m_VideoCap.m_Caps[capNum].nQCIF = 1;
	m_VideoCap.m_Caps[capNum].nCIF = 1;
	m_VideoCap.m_Caps[capNum].n4CIF = 2;
	capNum++;

	m_VideoCap.m_nCapNum = capNum;



	capNum = 0;

	m_H239VideoCap.m_Caps[capNum].VideoCodecType = VIDEC_CODEC_H263;
	m_H239VideoCap.m_Caps[capNum].nMaxBitrate = H263_MBPS_ALL;	//384Kbps
	m_H239VideoCap.m_Caps[capNum].nQCIF = 1;
	m_H239VideoCap.m_Caps[capNum].nCIF = 1;
	m_H239VideoCap.m_Caps[capNum].n4CIF = 2;
	capNum++;

	m_H239VideoCap.m_Caps[capNum].VideoCodecType = VIDEC_CODEC_H264;
	m_H239VideoCap.m_Caps[capNum].nMaxBitrate = H264_MBPS_CIF;	  
	m_H239VideoCap.m_Caps[capNum].n4CIF = H264_LEVEL_CIF;    //H264此用于LEVEL
	m_H239VideoCap.m_Caps[capNum].nQCIF = H264_ENTRY_CIF+20000;    //H264此用于ENTRY
	m_H239VideoCap.m_Caps[capNum].nCIF = CAP_H264EXTEND_CIF+18;  //H264此用于CAP TYPE
	capNum++;

	m_H239VideoCap.m_nCapNum = capNum;



	capNum = 0;
	m_DataCap.m_Caps[capNum].DataCodecID = DATA_CODEC_H224;//远遥
	capNum++;
	m_DataCap.m_nCapNum = capNum;


	m_nUseCapOLC=0;
	m_nG729Type=0;

	m_nOLCMaxMBPS=0;									//用于OLC能力集可选参数值 最大宏块处理速率
	m_nOLCMaxFS=0;										//用于OLC能力集可选参数值 最大帧长
	m_nOLCMaxDPB=0;										//用于OLC能力集可选参数值 最大解码图像缓存大小
	m_nOLCMaxBR=0;										//用于OLC能力集可选参数值 最大视频码率
	m_nMainVideoFrameRate=30;
	m_nSecondVideoFrameRate=30;

}

CallCapability::~CallCapability(void)
{
}


int CallCapability::SetVideoCap(int type,VIDEO_CAPABILITY_ITEM& cap)                     //视频能力是否存在
{
	int result = -1;
	VideoCap& capSet=m_VideoCap;

	//查找位置
	int capNum=capSet.m_nCapNum;
	for(int i=0;i<capNum;i++)
	{
		if(memcmp(capSet.m_Caps+i,&cap,sizeof(VIDEO_CAPABILITY_ITEM))==0)
		{
			result=i;break;
		}
	}
	if(result==-1) return 0;       //没有查找到
	//设为首位
	if(type==1)
	{
		capNum=result;   
		VIDEO_CAPABILITY_ITEM capsTemp[MAX_VIDEOCAP_NUM];
		//COPY其前部分
		memcpy(capsTemp,capSet.m_Caps,sizeof(VIDEO_CAPABILITY_ITEM)*capNum);
		memcpy(&(capSet.m_Caps[0]),&cap,sizeof(VIDEO_CAPABILITY_ITEM));
		memcpy(capSet.m_Caps+1,capsTemp,sizeof(VIDEO_CAPABILITY_ITEM)*capNum);
		capNum=capSet.m_nCapNum;
	}
	//移除
	if(type==2)
	{
		VIDEO_CAPABILITY_ITEM capsTemp[MAX_VIDEOCAP_NUM];
		//COPY其后部分
		memcpy(capsTemp,capSet.m_Caps+result+1,sizeof(VIDEO_CAPABILITY_ITEM)*(capNum-result-1));
		memcpy(capSet.m_Caps+result,capsTemp,sizeof(VIDEO_CAPABILITY_ITEM)*(capNum-result-1));
		capNum--;
		capSet.m_nCapNum=capNum;
	}
	SetSimultaneousCap();                 //重新设置同时能力集参数
	result=capNum;
	return result; 
} 
int CallCapability::SetAudioCap(int type,AUDIO_CAPABILITY_ITEM& cap)                     //视频能力是否存在
{
	int result = -1;
	AudioCap& capSet=m_AudioCap;

	//查找位置
	int capNum=capSet.m_nCapNum;
	for(int i=0;i<capNum;i++)
	{
		if(memcmp(capSet.m_Caps+i,&cap,sizeof(AUDIO_CAPABILITY_ITEM))==0)
		{
			result=i;break;
		}
	}
	if(result==-1) return 0;       //没有查找到
	//设为首位
	if(type==1)
	{
		capNum=result;   
		AUDIO_CAPABILITY_ITEM capsTemp[MAX_AUDIOCAP_NUM];
		//COPY其前部分
		memcpy(capsTemp,capSet.m_Caps,sizeof(AUDIO_CAPABILITY_ITEM)*capNum);
		memcpy(&(capSet.m_Caps[0]),&cap,sizeof(AUDIO_CAPABILITY_ITEM));
		memcpy(capSet.m_Caps+1,capsTemp,sizeof(AUDIO_CAPABILITY_ITEM)*capNum);
		capNum=capSet.m_nCapNum;
	}
	//移除
	if(type==2)
	{
		AUDIO_CAPABILITY_ITEM capsTemp[MAX_AUDIOCAP_NUM];
		//COPY其后部分
		memcpy(capsTemp,capSet.m_Caps+result+1,sizeof(AUDIO_CAPABILITY_ITEM)*(capNum-result-1));
		memcpy(capSet.m_Caps+result,capsTemp,sizeof(AUDIO_CAPABILITY_ITEM)*(capNum-result-1));
		capNum--;
		capSet.m_nCapNum=capNum;
	}
	SetSimultaneousCap();                 //重新设置同时能力集参数
	result=capNum;
	return result; 
} 


int CallCapability::SetH239Cap(int type,VIDEO_CAPABILITY_ITEM& cap)                     //视频能力是否存在
{
	int result = -1;
	H239VideoCap& capSet=m_H239VideoCap;

	//查找位置
	int capNum=capSet.m_nCapNum;
	for(int i=0;i<capNum;i++)
	{
		if(memcmp(capSet.m_Caps+i,&cap,sizeof(VIDEO_CAPABILITY_ITEM))==0)
		{
			result=i;break;
		}
	}
	if(result==-1) return 0;       //没有查找到
	//设为首位
	if(type==1)
	{
		capNum=result;   
		VIDEO_CAPABILITY_ITEM capsTemp[MAX_VIDEOCAP_NUM];
		//COPY其前部分
		memcpy(capsTemp,capSet.m_Caps,sizeof(VIDEO_CAPABILITY_ITEM)*capNum);
		memcpy(&(capSet.m_Caps[0]),&cap,sizeof(VIDEO_CAPABILITY_ITEM));
		memcpy(capSet.m_Caps+1,capsTemp,sizeof(VIDEO_CAPABILITY_ITEM)*capNum);
		capNum=capSet.m_nCapNum;
	}
	//移除
	if(type==2)
	{
		VIDEO_CAPABILITY_ITEM capsTemp[MAX_VIDEOCAP_NUM];
		//COPY其后部分
		memcpy(capsTemp,capSet.m_Caps+result+1,sizeof(VIDEO_CAPABILITY_ITEM)*(capNum-result-1));
		memcpy(capSet.m_Caps+result,capsTemp,sizeof(VIDEO_CAPABILITY_ITEM)*(capNum-result-1));
		capNum--;
		capSet.m_nCapNum=capNum;
	}
	SetSimultaneousCap();                 //重新设置同时能力集参数
	result=capNum;
	return result; 
} 
int CallCapability::SetDataCap(int type,DATA_CAPABILITY_ITEM& cap)                     //视频能力是否存在
{
	int result = -1;
	DataCap& capSet=m_DataCap;
	//查找位置
	int capNum=capSet.m_nCapNum;
	for(int i=0;i<capNum;i++)
	{
		if(memcmp(capSet.m_Caps+i,&cap,sizeof(DATA_CAPABILITY_ITEM))==0)
		{
			result=i;break;
		}
	}
	if(result==-1) return 0;       //没有查找到
	//设为首位
	if(type==1)
	{
		capNum=result;   
		DATA_CAPABILITY_ITEM capsTemp[MAX_DATACAP_NUM];
		//COPY其前部分
		memcpy(capsTemp,capSet.m_Caps,sizeof(DATA_CAPABILITY_ITEM)*capNum);
		memcpy(&(capSet.m_Caps[0]),&cap,sizeof(DATA_CAPABILITY_ITEM));
		memcpy(capSet.m_Caps+1,capsTemp,sizeof(DATA_CAPABILITY_ITEM)*capNum);
		capNum=capSet.m_nCapNum;
	}
	//移除
	if(type==2)
	{
		DATA_CAPABILITY_ITEM capsTemp[MAX_DATACAP_NUM];
		//COPY其后部分
		memcpy(capsTemp,capSet.m_Caps+result+1,sizeof(DATA_CAPABILITY_ITEM)*(capNum-result-1));
		memcpy(capSet.m_Caps+result,capsTemp,sizeof(DATA_CAPABILITY_ITEM)*(capNum-result-1));
		capNum--;
		capSet.m_nCapNum=capNum;
	}
	SetSimultaneousCap();                 //重新设置同时能力集参数
	result=capNum;
	return result; 
} 

int CallCapability::AddVideoCap(VIDEO_CAPABILITY_ITEM& cap)                     //视频能力是否存在
{
	int result = 0;
	int capNum=m_VideoCap.m_nCapNum;
	memcpy(&(m_VideoCap.m_Caps[capNum]),&cap,sizeof(VIDEO_CAPABILITY_ITEM));
	capNum++;
	m_VideoCap.m_nCapNum=capNum;
	SetSimultaneousCap();                 //重新设置同时能力集参数
	result=capNum;
	return result; 
} 
int CallCapability::AddAudioCap(AUDIO_CAPABILITY_ITEM& cap)                     //视频能力是否存在
{
	int result = 0;
	int capNum=m_AudioCap.m_nCapNum;
	memcpy(&(m_AudioCap.m_Caps[capNum]),&cap,sizeof(AUDIO_CAPABILITY_ITEM));
	capNum++;
	m_AudioCap.m_nCapNum=capNum;
	SetSimultaneousCap();                 //重新设置同时能力集参数
	result=capNum;
	return result; 
} 
int CallCapability::AddH239Cap(VIDEO_CAPABILITY_ITEM& cap)                     //视频能力是否存在
{
	int result = 0;
	int capNum=m_H239VideoCap.m_nCapNum;
	memcpy(&(m_H239VideoCap.m_Caps[capNum]),&cap,sizeof(VIDEO_CAPABILITY_ITEM));	
	capNum++;
	m_H239VideoCap.m_nCapNum=capNum;
	SetSimultaneousCap();                 //重新设置同时能力集参数
	result=capNum;
	return result; 
} 
int CallCapability::AddDataCap(DATA_CAPABILITY_ITEM& cap)                     //视频能力是否存在
{
	int result = 0;
	int capNum=m_DataCap.m_nCapNum;
	memcpy(&(m_DataCap.m_Caps[capNum]),&cap,sizeof(DATA_CAPABILITY_ITEM));
	capNum++;
	m_DataCap.m_nCapNum=capNum;
	SetSimultaneousCap();                 //重新设置同时能力集参数
	result=capNum;
	return result; 
} 

int CallCapability::IsExistSimul(int type ,int iEntryNumber)                     //视频能力是否存在
{
	int result = 0;
#ifdef HAS_H323

	char tempbuf[200];
	int nodeId=m_NodeId;
	HPVT valH = cmGetValTree( H323CallMgr::Instance().m_hApp ); 
	int nSimulNum = 1;
	int nSimultaneous = type;    //1 audio 2 video 3 H239 4 Data
	int iAlternative = 1;
	int nValue=0;
	int nRet=0;
	RvPstFieldId	nField;
	RvBool			bString;

	//判断是否实际加入能力项 notice
	int capNum[16]={0};
	if(this->m_AudioCap.m_nCapNum==0) capNum[0]=1;
	if(this->m_VideoCap.m_nCapNum==0) capNum[1]=1;
	if(this->m_H239VideoCap.m_nCapNum==0) capNum[2]=1;
	if(this->m_DataCap.m_nCapNum==0) capNum[3]=1;
	for(int k=0;k<type-1;k++)
		if(capNum[k]) type--;

	nSimultaneous = type;
	for(;;iAlternative++)
	{
		sprintf(tempbuf,"capabilityDescriptors.%d.simultaneousCapabilities.%d.%d",nSimulNum, nSimultaneous,iAlternative);
		nRet=pvtGetByPath(valH,nodeId,tempbuf,&nField,&nValue,&bString);
		if(nRet>=0)
		{
			if(iEntryNumber == nValue)
			{
				result = iEntryNumber;
				break;
			}
		}
		else break;
	}

#endif
	return result; 
} 

int CallCapability::IsFirstSimul(int type ,int iEntryNumber)                     //视频能力是否存在
{
	int result = 0;
#ifdef HAS_H323
	char tempbuf[200];
	int nodeId=m_NodeId;
	HPVT valH = cmGetValTree( H323CallMgr::Instance().m_hApp ); 
	int nSimulNum = 1;
	int nSimultaneous = type;    //1 audio 2 video 3 H239 4 Data
	int iAlternative = 1;
	int nValue=0;
	int nRet=0;
	RvPstFieldId	nField;
	RvBool			bString;

	//判断是否实际加入能力项 notice
	int capNum[16]={0};
	if(this->m_AudioCap.m_nCapNum==0) capNum[0]=1;
	if(this->m_VideoCap.m_nCapNum==0) capNum[1]=1;
	if(this->m_H239VideoCap.m_nCapNum==0) capNum[2]=1;
	if(this->m_DataCap.m_nCapNum==0) capNum[3]=1;
	for(int k=0;k<type-1;k++)
		if(capNum[k]) type--;

	nSimultaneous = type;

	sprintf(tempbuf,"capabilityDescriptors.%d.simultaneousCapabilities.%d.1",nSimulNum, nSimultaneous,iAlternative);
	nRet=pvtGetByPath(valH,nodeId,tempbuf,&nField,&nValue,&bString);
	if(nRet>=0)
	{
		if(iEntryNumber == nValue)
			result = iEntryNumber;
	}
#endif
	return result; 
} 


int CallCapability::SetSimultaneousCap( )
{
	//return InitCapabilitis(m_NodeId);	
	return 1;
}

int CallCapability::BuildCapabilitis(int node)               //提交生成能力集表
{
	return InitCapabilitis(node);	
}

int CallCapability::InitCapabilitis(int node)
{
#ifdef HAS_H323

	char tempbuf[1024]; char subBuf[1024]={0};
	int nodeId=m_NodeId=node;
	HPVT valH = cmGetValTree( H323CallMgr::Instance().m_hApp ); 
	int error, iEntryNumber = 0, iCapIndex=0;
	int subId2;

	int nSimulNum = 1;
	int nSimultaneous = 1;
	int iAlternative = 1;

	int param=0;
	//删除能力集表
	subId2 = pvtGetNodeIdByPath( valH, nodeId, "capabilityTable");
	if(subId2>=0)
	{
		error = pvtDelete(valH,subId2);
	}

	//删除所有的同时能力
	//H235 Media use
	subId2 = pvtGetNodeIdByPath( valH, nodeId, "capabilityDescriptors.1.simultaneousCapabilities.5");
	if(subId2>=0)
	{
		error = pvtDelete(valH,subId2);
	}

	subId2 = pvtGetNodeIdByPath( valH, nodeId, "capabilityDescriptors.1.simultaneousCapabilities.4");
	if(subId2>=0)
	{
		error = pvtDelete(valH,subId2);
	}
	subId2 = pvtGetNodeIdByPath( valH, nodeId, "capabilityDescriptors.1.simultaneousCapabilities.3");
	if(subId2>=0)
	{
		error = pvtDelete(valH,subId2);
	}
	subId2 = pvtGetNodeIdByPath( valH, nodeId, "capabilityDescriptors.1.simultaneousCapabilities.2");
	if(subId2>=0)
	{
		error = pvtDelete(valH,subId2);
	}
	subId2 = pvtGetNodeIdByPath( valH, nodeId, "capabilityDescriptors.1.simultaneousCapabilities.1");
	if(subId2>=0)
	{
		error = pvtDelete(valH,subId2);
	}

	if(m_nUseCapOLC==0)
	{
		sprintf(tempbuf, 
			"multiplexCapability.h2250Capability.mediaPacketizationCapability.h261aVideoPacketization");
		error = pvtBuildByPath(valH, nodeId, tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf, 
			"multiplexCapability.h2250Capability.mediaPacketizationCapability.rtpPayloadType.1.payloadDescriptor.rfc-number");
		error = pvtBuildByPath(valH, nodeId, tempbuf, 2429, NULL);

		sprintf(tempbuf, 
			"multiplexCapability.h2250Capability.mediaPacketizationCapability.rtpPayloadType.2.payloadDescriptor.oid");
		unsigned char pValueOid[10]={0x00,0x08,0x81,0x71,0x00,0x00,0x00,0x00,0};
		error = pvtBuildByPath(valH, nodeId, tempbuf, 8, (char*)pValueOid);
	}

	unsigned int i;
	//设置音频同时能力集
	iAlternative = 1;
	for(i=0; i<m_AudioCap.m_nCapNum; i++)
	{
		switch( m_AudioCap.m_Caps[i].AudioCodecID )
		{
		case X_AUDIO_CODEC_G711A:
			{
				iEntryNumber = G711A_ENTRY_64K;
				iCapIndex ++;

				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.g711Alaw64k", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 20, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break; 
		case X_AUDIO_CODEC_G711U:
			{
				iEntryNumber = G711U_ENTRY_64K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.g711Ulaw64k", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 20, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case X_AUDIO_CODEC_G722_64:
			{
				iEntryNumber = G722_ENTRY_64K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.g722-64k", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 30, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case X_AUDIO_CODEC_G7221_7_24:
			{
				iEntryNumber = G7221_ENTRY_24K;
				iCapIndex ++;						
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[6]={0x00,0x07,0xB8,0x35,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 24000, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case X_AUDIO_CODEC_G7221_7_32:
			{
				iEntryNumber = G7221_ENTRY_32K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[6]={0x00,0x07,0xB8,0x35,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 32000, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

		case X_AUDIO_CODEC_G7221_14_24:
			{
				iEntryNumber = G7221C_ENTRY_24K;
				iCapIndex ++;						
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[7]={0x00,0x07,0xB8,0x35,0x01,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 240, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterValue.booleanArray", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 64, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case X_AUDIO_CODEC_G7221_14_32:
			{
				iEntryNumber = G7221C_ENTRY_32K;
				iCapIndex ++;						
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[7]={0x00,0x07,0xB8,0x35,0x01,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 320, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterValue.booleanArray", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 32, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

		case X_AUDIO_CODEC_G7221_14_48:
			{
				iEntryNumber = G7221C_ENTRY_48K;
				iCapIndex ++;						
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[7]={0x00,0x07,0xB8,0x35,0x01,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 480, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterValue.booleanArray", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 16, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

			//notice 20111227 update
		case X_AUDIO_CODEC_G719_32:
			{
				iEntryNumber = G719_ENTRY_32K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[6]={0x00,0x07,0x85,0x4F,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 320, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.3.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 3, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.3.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case X_AUDIO_CODEC_G719_48:
			{
				iEntryNumber = G719_ENTRY_48K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[6]={0x00,0x07,0x85,0x4F,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 480, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.3.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 3, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.3.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case X_AUDIO_CODEC_G719_64:
			{
				iEntryNumber = G719_ENTRY_64K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[6]={0x00,0x07,0x85,0x4F,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 640, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 4, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.3.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 3, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.3.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;


		case X_AUDIO_CODEC_G7231_63:
			//{
			//	iEntryNumber = G7231_ENTRY;
			//	iCapIndex ++;
			//	AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);
			//	sprintf(tempbuf, "%s.g7231.maxAl-sduAudioFrames",subBuf);
			//	error = pvtBuildByPath(valH, nodeId, tempbuf, 8, NULL);
			//	sprintf(tempbuf, "%s.g7231.silenceSuppression",subBuf);
			//	error = pvtBuildByPath(valH, nodeId, tempbuf, RV_FALSE, NULL);
			//	//设置同时能力集
			//	DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
			//}
			break;
		case X_AUDIO_CODEC_G729:
			{
				if( (m_nUseCapOLC==0) || (m_nUseCapOLC==1 && m_nG729Type==2) )
				{
					iEntryNumber = G729A_ENTRY;
					iCapIndex ++;
					AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);
					sprintf(tempbuf, "%s.g729AnnexA", subBuf);
					error = pvtBuildByPath(valH, nodeId, tempbuf, 20, NULL);
					//设置同时能力集
					DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
				}
				if( (m_nUseCapOLC==0) || (m_nUseCapOLC==1 && m_nG729Type==1) )
				{
					iEntryNumber = G729_ENTRY;
					iCapIndex ++;
					AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);
					sprintf(tempbuf, "%s.g729", subBuf);
					error = pvtBuildByPath(valH, nodeId, tempbuf, 30, NULL);
					//设置同时能力集
					DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
				}
			}
			break;
		default:
			break;
		}

	}

	//add tai er ru wang test use
#if defined(TAIERLAB) && (TAIERLAB == 1)

	if(m_nUseCapOLC==0)
	{
		{
			iEntryNumber = G7231_ENTRY;
			iCapIndex ++;
			AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);
			sprintf(tempbuf, "%s.g7231.maxAl-sduAudioFrames",subBuf);
			error = pvtBuildByPath(valH, nodeId, tempbuf, 8, NULL);
			sprintf(tempbuf, "%s.g7231.silenceSuppression",subBuf);
			error = pvtBuildByPath(valH, nodeId, tempbuf, RV_FALSE, NULL);
			//设置同时能力集
			DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
		}
		{
			iEntryNumber = G728_ENTRY;
			iCapIndex ++;
			AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);
			sprintf(tempbuf, "%s.g728",subBuf);
			error = pvtBuildByPath(valH, nodeId, tempbuf, 30, NULL);
			//设置同时能力集
			DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
		}
	}

#endif

	if(iAlternative>1)
	{
		nSimultaneous++;
	}

	//设置视频同时能力集
	iAlternative = 1;
	for(i=0; i<m_VideoCap.m_nCapNum; i++)
	{
		int nHasQCIF = 0;
		int nHasCIF = 0;
		int nHas4CIF = 0;
		int nVideoRate = 384;


		VIDEO_CAPABILITY_ITEM& capVideo = m_VideoCap.m_Caps[i];
		nHasQCIF = capVideo.nQCIF;
		nHasCIF = capVideo.nCIF;
		nHas4CIF = capVideo.n4CIF;
		nVideoRate = capVideo.nMaxBitrate;
		nVideoRate=GetMainVideoMaxBitrate(nVideoRate);
		switch( capVideo.VideoCodecType )
		{ 
		case VIDEC_CODEC_H261:
			{
				//修改H261能力参数
				iEntryNumber = 261;
				iCapIndex ++;
				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);
				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.h261VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.h261VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				sprintf(tempbuf,"%s.h261VideoCapability.temporalSpatialTradeOffCapability",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.h261VideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					if(nVideoRate >1920 )
						error = pvtBuildByPath(valH, nodeId,tempbuf,1920 *10, NULL);
					else
						error = pvtBuildByPath(valH, nodeId,tempbuf,nVideoRate*10, NULL);
				}

				sprintf(tempbuf,"%s.h261VideoCapability.stillImageTransmission",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
			}
			break;

		case VIDEC_CODEC_H263:
			{
				//修改H261能力参数
				iEntryNumber = 263;
				iCapIndex ++;

				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				if (nHas4CIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.cif4MPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHas4CIF, NULL);
				}


				sprintf(tempbuf,"%s.h263VideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}


				sprintf(tempbuf,"%s.h263VideoCapability.unrestrictedVector",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.h263VideoCapability.arithmeticCoding",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.h263VideoCapability.advancedPrediction",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.h263VideoCapability.pbFrames",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.h263VideoCapability.temporalSpatialTradeOffCapability",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.h263VideoCapability.errorCompensation",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case VIDEC_CODEC_H263P:
			{				
				iEntryNumber = 2632;
				iCapIndex ++;
				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				if (nHas4CIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.cif4MPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHas4CIF, NULL);
				}


				sprintf(tempbuf,"%s.h263VideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				//注意此
				int tempNodeId=nodeId;
				//注意此
				sprintf(tempbuf,"%s.h263VideoCapability",subBuf);
				nodeId = pvtBuildByPath(valH, nodeId,tempbuf, NULL, NULL);

				if(nodeId >=0 )
				{
					sprintf(tempbuf,"unrestrictedVector");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"arithmeticCoding");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"advancedPrediction");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"pbFrames");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"temporalSpatialTradeOffCapability");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"errorCompensation");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//h263Options

					sprintf(tempbuf,"h263Options.advancedIntraCodingMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.deblockingFilterMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.improvedPBFramesMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.unlimitedMotionVectors");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					sprintf(tempbuf,"h263Options.fullPictureFreeze");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.partialPictureFreezeAndRelease");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					sprintf(tempbuf,"h263Options.resizingPartPicFreezeAndRelease");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.fullPictureSnapshot");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.partialPictureSnapshot");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.videoSegmentTagging");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					sprintf(tempbuf,"h263Options.progressiveRefinement");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.dynamicPictureResizingByFour");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					sprintf(tempbuf,"h263Options.dynamicPictureResizingSixteenthPel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.dynamicWarpingHalfPel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.dynamicWarpingSixteenthPel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.independentSegmentDecoding");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					sprintf(tempbuf,"h263Options.slicesInOrder-NonRect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.slicesInOrder-Rect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					sprintf(tempbuf,"h263Options.slicesNoOrder-NonRect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.slicesNoOrder-Rect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.alternateInterVLCMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.modifiedQuantizationMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.reducedResolutionUpdate");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.separateVideoBackChannel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					//add customPictureFormat
					SetH263OptionParam(2,capVideo.VideoCodecType,tempbuf,valH,nodeId);

				}


				//注意此
				nodeId=tempNodeId;

				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
			}
			break;

		case VIDEC_CODEC_H263PP:
			{
				iEntryNumber = 2633;
				iCapIndex ++;

				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				if (nHas4CIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.cif4MPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHas4CIF, NULL);
				}


				sprintf(tempbuf,"%s.h263VideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}
				//注意此
				int tempNodeId=nodeId;
				//注意此
				sprintf(tempbuf,"%s.h263VideoCapability",subBuf);
				nodeId = pvtBuildByPath(valH, nodeId,tempbuf, NULL, NULL);

				if(nodeId >=0 )
				{
					sprintf(tempbuf,"unrestrictedVector");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"arithmeticCoding");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"advancedPrediction");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"pbFrames");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"temporalSpatialTradeOffCapability");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"errorCompensation");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//h263Options

					sprintf(tempbuf,"h263Options.advancedIntraCodingMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.deblockingFilterMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.improvedPBFramesMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.unlimitedMotionVectors");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					sprintf(tempbuf,"h263Options.fullPictureFreeze");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.partialPictureFreezeAndRelease");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					sprintf(tempbuf,"h263Options.resizingPartPicFreezeAndRelease");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.fullPictureSnapshot");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.partialPictureSnapshot");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.videoSegmentTagging");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					sprintf(tempbuf,"h263Options.progressiveRefinement");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.dynamicPictureResizingByFour");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					sprintf(tempbuf,"h263Options.dynamicPictureResizingSixteenthPel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.dynamicWarpingHalfPel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.dynamicWarpingSixteenthPel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.independentSegmentDecoding");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					sprintf(tempbuf,"h263Options.slicesInOrder-NonRect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.slicesInOrder-Rect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					sprintf(tempbuf,"h263Options.slicesNoOrder-NonRect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.slicesNoOrder-Rect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.alternateInterVLCMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.modifiedQuantizationMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.reducedResolutionUpdate");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.separateVideoBackChannel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.videoBadMBsCap");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					//add customPictureFormat //notice call MG6050-R5242 not add , 20140827
					//SetH263OptionParam(2,capVideo.VideoCodecType,tempbuf,valH,nodeId);

					//h263Version3Options
					{
						sprintf(tempbuf,"h263Options.h263Version3Options.dataPartitionedSlices");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.fixedPointIDCT0");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.interlacedFields");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.currentPictureHeaderRepetition");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.previousPictureHeaderRepetition");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.nextPictureHeaderRepetition");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.pictureNumber");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.spareReferencePictures");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					}


				}


				//注意此
				nodeId=tempNodeId;

				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);


			}
			break;


		case VIDEC_CODEC_H264:
			{
				iEntryNumber = capVideo.nQCIF;
				iCapIndex ++;
				int nValue=nHas4CIF;

				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.genericVideoCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[10]={0x00,0x08,0x81,0x71,0x00,0x00,0x01,0};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf,"%s.genericVideoCapability.collapsing.1.parameterIdentifier.standard",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 41, NULL);	


				sprintf(tempbuf,"%s.genericVideoCapability.collapsing.1.parameterValue.booleanArray",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 64, NULL);	

				sprintf(tempbuf,"%s.genericVideoCapability.collapsing.2.parameterIdentifier.standard",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 42, NULL);	


				sprintf(tempbuf,"%s.genericVideoCapability.collapsing.2.parameterValue.unsignedMin",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, nValue, NULL);	

				char *szPath=tempbuf;
				int nNode=nodeId;
				HPVT hPvt=valH;
				char tempStr[512]={0};
				int nCount=0;
				sprintf(tempStr,"%s.genericVideoCapability.collapsing",subBuf);
				//notice set standard value
				tagCollapsing Collapsing[16]={0};

				int nBitrateGet= SetH264CapOptionParam(nValue,Collapsing,nCount,2,capVideo.VideoCodecType,iEntryNumber);

				for(int j=0;j<nCount;j++)
				{
					sprintf(szPath,"%s.%u.parameterIdentifier.standard",tempStr,j+3);
					error = pvtBuildByPath(hPvt, nNode, szPath, Collapsing[j].standard, NULL);
					sprintf(szPath,"%s.%u.parameterValue.unsignedMin",tempStr,j+3);
					error = pvtBuildByPath(hPvt, nNode, szPath, Collapsing[j].value, NULL);
				}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

				if(m_nUseCapOLC==0) nVideoRate=nBitrateGet;
#endif

				sprintf(tempbuf,"%s.genericVideoCapability.maxBitRate",
					subBuf);
				//if(nVideoRate <=0) nVideoRate=768;
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
			}
			break;

		case VIDEC_CODEC_H264_SVC:
			{	
				iEntryNumber = capVideo.nQCIF;
				iCapIndex ++;
				int nValue=nHas4CIF;

				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.genericVideoCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[10]={0x00,0x08,0x81,0x71,0x00,0x00,0x01,0};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf,"%s.genericVideoCapability.collapsing.1.parameterIdentifier.standard",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 41, NULL);	


				sprintf(tempbuf,"%s.genericVideoCapability.collapsing.1.parameterValue.booleanArray",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 8, NULL);	

				sprintf(tempbuf,"%s.genericVideoCapability.collapsing.2.parameterIdentifier.standard",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 42, NULL);	

				sprintf(tempbuf,"%s.genericVideoCapability.collapsing.2.parameterValue.unsignedMin",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, nValue, NULL);	

				//char *szPath=tempbuf;
				//int nNode=nodeId;
				//HPVT hPvt=valH;
				//char tempStr[512]={0};
				//int nCount=0;
				//struct tagCollapsing
				//{
				//	int standard;
				//	int value;
				//};
				//sprintf(tempStr,"%s.genericVideoCapability.collapsing",subBuf);
				////set standard value
				//tagCollapsing Collapsing[]={ {6,80} };
				//nCount=sizeof(Collapsing)/sizeof(tagCollapsing);
				//notice set standard value

				char *szPath=tempbuf;
				int nNode=nodeId;
				HPVT hPvt=valH;
				char tempStr[512]={0};
				int nCount=0;
				sprintf(tempStr,"%s.genericVideoCapability.collapsing",subBuf);
				//notice set standard value
				tagCollapsing Collapsing[16]={0};
				int nBitrateGet= SetH264CapOptionParam(nValue,Collapsing,nCount,2,capVideo.VideoCodecType,iEntryNumber);

				for(int j=0;j<nCount;j++)
				{
					sprintf(szPath,"%s.%u.parameterIdentifier.standard",tempStr,j+3);
					error = pvtBuildByPath(hPvt, nNode, szPath, Collapsing[j].standard, NULL);
					sprintf(szPath,"%s.%u.parameterValue.unsignedMin",tempStr,j+3);
					error = pvtBuildByPath(hPvt, nNode, szPath, Collapsing[j].value, NULL);
				}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

				if(m_nUseCapOLC==0) nVideoRate=nBitrateGet;
#endif
				sprintf(tempbuf,"%s.genericVideoCapability.maxBitRate",
					subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;


		case VIDEC_CODEC_MPEG4: //notice only use ,use to call kedacom
			{
				iEntryNumber = capVideo.nQCIF;
				iCapIndex ++;

// 7>       nonStandard = 0
// 8>        nonStandardIdentifier = 0
// 9>         h221NonStandard = 0
//10>          t35CountryCode = 38
//10>          t35Extension = 0
//10>          manufacturerCode = 8888
// 8>        data = 32 '....kedacom_js_China............' 

				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.nonStandard.nonStandardIdentifier.h221NonStandard.t35CountryCode", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 38,NULL);

				sprintf(tempbuf, "%s.nonStandard.nonStandardIdentifier.h221NonStandard.t35Extension", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 0,NULL);

				sprintf(tempbuf, "%s.nonStandard.nonStandardIdentifier.h221NonStandard.manufacturerCode", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 8888,NULL);

				sprintf(tempbuf, "%s.nonStandard.data", subBuf);
				//....kedacom_js_China............
				if(m_nUseCapOLC==0)
				{
				unsigned char pValue[32]=
				{ 0x00 ,0x00 ,0x00 ,0x11 ,0x6b ,
				0x65 ,0x64 ,0x61 ,0x63 ,0x6f ,0x6d ,0x5f ,0x6a ,
				0x73 ,0x5f ,0x43 ,0x68 ,0x69 ,0x6e ,0x61 ,0x00 ,
				0x00 ,0x03 ,0x00 ,0x00 ,0x01 ,0x01 ,0x00 ,0x01 ,
				0x01 ,0x01 ,0x00 };
				error = pvtBuildByPath(valH, nodeId, tempbuf, 32, (char*)pValue);
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
				}
				else
				{
					unsigned char pValue[32]=
					{ 0x00 ,0x00 ,0x00 ,0x11 ,0x6b ,
					0x65 ,0x64 ,0x61 ,0x63 ,0x6f ,0x6d ,0x5f ,0x6a ,
					0x73 ,0x5f ,0x43 ,0x68 ,0x69 ,0x6e ,0x61 ,0x00 ,
					0x00 ,0x01 ,0x40 ,0x00 ,0x00 ,0x00 ,0x00 ,0x01 ,
					0x00 ,0x00 ,0x00 };
					error = pvtBuildByPath(valH, nodeId, tempbuf, 32, (char*)pValue);
					DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
				}
			}
			break;

		default:
			break;
		}

	}

	//add by Jan for fake MPEG4 [20130903] //add tai er ru wang test use
#if defined(TAIERLAB) && (TAIERLAB == 1)

	if(m_nUseCapOLC==0)
	{
		iEntryNumber = MPE4_ENTRY;
		iCapIndex ++;
		sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);
		char OID[16] = {0};
		int length;
		length=utlEncodeOID(sizeof(OID),OID,"itu-t recommendation h 245 1 0 0");
		sprintf(tempbuf, "capabilityTable.%d.capability.receiveVideoCapability.genericVideoCapability.capabilityIdentifier.standard", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, length, OID);
		sprintf(tempbuf,
			"capabilityTable.%u.capability.receiveVideoCapability.genericVideoCapability.maxBitRate",
			iCapIndex);
		error = pvtBuildByPath(valH, nodeId,tempbuf, 384*10, NULL);

		sprintf(tempbuf,
			"capabilityTable.%u.capability.receiveVideoCapability.genericVideoCapability.nonCollapsing.1.parameterIdentifier.standard",
			iCapIndex);
		error = pvtBuildByPath(valH, nodeId,tempbuf, 0, NULL);

		sprintf(tempbuf,
			"capabilityTable.%u.capability.receiveVideoCapability.genericVideoCapability.nonCollapsing.1.parameterValue.unsignedMax",
			iCapIndex);
		error = pvtBuildByPath(valH, nodeId,tempbuf, 3, NULL);//默认是3   14496-2中定义的simple/L2 是cif值为3

		DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

	}

#endif

	//end [20130903]

	if(iAlternative>1)
	{
		nSimultaneous++;
	}


	//设置双流同时能力
	iAlternative = 1;
	bool bHasH239Cap = false;
	for(i=0; i<m_H239VideoCap.m_nCapNum; i++)
	{
		switch(m_H239VideoCap.m_Caps[i].VideoCodecType)
		{
		case VIDEC_CODEC_H261:
		case VIDEC_CODEC_H263:
		case VIDEC_CODEC_H263P:
		case VIDEC_CODEC_H263PP:
			//notice 20120116 update
		case VIDEC_CODEC_H264:
		case VIDEC_CODEC_H264_SVC:
			bHasH239Cap = true;
			break;
		default:
			break;
		}
		if(bHasH239Cap)
			break;
	}

	if(m_nUseCapOLC==0)
	{
		if(bHasH239Cap)
		{
			//===重新构建H239控制能力
			iEntryNumber = 239;
			iCapIndex ++;			
			sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
			error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);

			unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x01,0};
			sprintf(tempbuf,"capabilityTable.%d.capability.genericControlCapability.capabilityIdentifier.standard",iCapIndex);
			error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

		}
	}

	//设置双流能力集参数
	for(i=0; i<m_H239VideoCap.m_nCapNum; i++)
	{
		int nHasQCIF = 0;
		int nHasCIF = 0;
		int nHas4CIF = 0;
		int nVideoRate = 384;


		VIDEO_CAPABILITY_ITEM& capVideo = m_H239VideoCap.m_Caps[i];
		nHasQCIF = capVideo.nQCIF;
		nHasCIF = capVideo.nCIF;
		nHas4CIF = capVideo.n4CIF;
		nVideoRate = capVideo.nMaxBitrate;

		nVideoRate=GetSecondVideoMaxBitrate(nVideoRate);

		//修改双流能力参数
		switch( capVideo.VideoCodecType )
		{ 
		case VIDEC_CODEC_H261:
			{
				//修改H261Extend能力参数
				iEntryNumber = 2261;
				iCapIndex ++;
				DualVideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h261VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h261VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}


				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h261VideoCapability.temporalSpatialTradeOffCapability",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h261VideoCapability.maxBitRate",
					subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}


				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h261VideoCapability.stillImageTransmission",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

		case VIDEC_CODEC_H263:
			{
				//修改H263Extend能力参数
				iEntryNumber = 2263;
				iCapIndex ++;
				DualVideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				if (nHas4CIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.cif4MPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHas4CIF, NULL);
				}


				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.maxBitRate",
					subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}


				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.unrestrictedVector",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.arithmeticCoding",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.advancedPrediction",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.pbFrames",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.temporalSpatialTradeOffCapability",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.errorCompensation",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);




				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

		case VIDEC_CODEC_H263P:
			{
				//修改H263v2Extend能力参数
				iEntryNumber = 22632;
				iCapIndex ++;
				DualVideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				if (nHas4CIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.cif4MPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHas4CIF, NULL);
				}


				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.maxBitRate",
					subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				//注意此
				int tempNodeId=nodeId;
				//注意此
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability",subBuf);
				nodeId = pvtBuildByPath(valH, nodeId,tempbuf, NULL, NULL);

				if(nodeId >=0 )
				{
					sprintf(tempbuf,"unrestrictedVector");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"arithmeticCoding");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"advancedPrediction");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"pbFrames");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"temporalSpatialTradeOffCapability");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"errorCompensation");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//h263Options

					sprintf(tempbuf,"h263Options.advancedIntraCodingMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.deblockingFilterMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.improvedPBFramesMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.unlimitedMotionVectors");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					sprintf(tempbuf,"h263Options.fullPictureFreeze");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.partialPictureFreezeAndRelease");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					sprintf(tempbuf,"h263Options.resizingPartPicFreezeAndRelease");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.fullPictureSnapshot");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.partialPictureSnapshot");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.videoSegmentTagging");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					sprintf(tempbuf,"h263Options.progressiveRefinement");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.dynamicPictureResizingByFour");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					sprintf(tempbuf,"h263Options.dynamicPictureResizingSixteenthPel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.dynamicWarpingHalfPel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.dynamicWarpingSixteenthPel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.independentSegmentDecoding");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					sprintf(tempbuf,"h263Options.slicesInOrder-NonRect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.slicesInOrder-Rect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					sprintf(tempbuf,"h263Options.slicesNoOrder-NonRect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.slicesNoOrder-Rect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.alternateInterVLCMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.modifiedQuantizationMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.reducedResolutionUpdate");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.separateVideoBackChannel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//add customPictureFormat
					SetH263OptionParam(3,capVideo.VideoCodecType,tempbuf,valH,nodeId);
				}


				//注意此
				nodeId=tempNodeId;



				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);


			}
			break;

		case VIDEC_CODEC_H263PP:
			{
				//修改H263v3Extend能力参数
				iEntryNumber = 22633;
				iCapIndex ++;

				DualVideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				if (nHas4CIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.cif4MPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHas4CIF, NULL);
				}


				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.maxBitRate",
					subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}


				//注意此
				int tempNodeId=nodeId;
				//注意此
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability",subBuf);
				nodeId = pvtBuildByPath(valH, nodeId,tempbuf, NULL, NULL);

				if(nodeId >=0 )
				{
					sprintf(tempbuf,"unrestrictedVector");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"arithmeticCoding");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"advancedPrediction");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"pbFrames");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"temporalSpatialTradeOffCapability");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"errorCompensation");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//h263Options

					sprintf(tempbuf,"h263Options.advancedIntraCodingMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.deblockingFilterMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.improvedPBFramesMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.unlimitedMotionVectors");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					sprintf(tempbuf,"h263Options.fullPictureFreeze");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.partialPictureFreezeAndRelease");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					sprintf(tempbuf,"h263Options.resizingPartPicFreezeAndRelease");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.fullPictureSnapshot");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.partialPictureSnapshot");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.videoSegmentTagging");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					sprintf(tempbuf,"h263Options.progressiveRefinement");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.dynamicPictureResizingByFour");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					sprintf(tempbuf,"h263Options.dynamicPictureResizingSixteenthPel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.dynamicWarpingHalfPel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.dynamicWarpingSixteenthPel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.independentSegmentDecoding");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					sprintf(tempbuf,"h263Options.slicesInOrder-NonRect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.slicesInOrder-Rect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					sprintf(tempbuf,"h263Options.slicesNoOrder-NonRect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.slicesNoOrder-Rect");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.alternateInterVLCMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.modifiedQuantizationMode");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.reducedResolutionUpdate");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.separateVideoBackChannel");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.videoBadMBsCap");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//add customPictureFormat
					SetH263OptionParam(3,capVideo.VideoCodecType,tempbuf,valH,nodeId);

					//h263Version3Options
					{
						sprintf(tempbuf,"h263Options.h263Version3Options.dataPartitionedSlices");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.fixedPointIDCT0");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.interlacedFields");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.currentPictureHeaderRepetition");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.previousPictureHeaderRepetition");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.nextPictureHeaderRepetition");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.pictureNumber");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.spareReferencePictures");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					}


				}


				//注意此
				nodeId=tempNodeId;


				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);


			}
			break;
		case VIDEC_CODEC_H264:
			{	
				iEntryNumber = capVideo.nQCIF;
				iCapIndex ++;
				int nValue=nHas4CIF;

				DualVideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[10]={0x00,0x08,0x81,0x71,0x00,0x00,0x01,0};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf,
					"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.1.parameterIdentifier.standard",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 41, NULL);	

				sprintf(tempbuf,
					"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.1.parameterValue.booleanArray",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 64, NULL);

				sprintf(tempbuf,
					"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.2.parameterIdentifier.standard",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 42, NULL);	

				sprintf(tempbuf,
					"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.2.parameterValue.unsignedMin",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, nValue, NULL);	

				char *szPath=tempbuf;
				int nNode=nodeId;
				HPVT hPvt=valH;
				char tempStr[512]={0};
				int nCount=0;
				sprintf(tempStr,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing",subBuf);

				//notice set standard value
				tagCollapsing Collapsing[16]={0};
				int nBitrateGet= SetH264CapOptionParam(nValue,Collapsing,nCount,3,capVideo.VideoCodecType,iEntryNumber);

				for(int j=0;j<nCount;j++)
				{
					sprintf(szPath,"%s.%u.parameterIdentifier.standard",tempStr,j+3);
					error = pvtBuildByPath(hPvt, nNode, szPath, Collapsing[j].standard, NULL);
					sprintf(szPath,"%s.%u.parameterValue.unsignedMin",tempStr,j+3);
					error = pvtBuildByPath(hPvt, nNode, szPath, Collapsing[j].value, NULL);
				}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

				if(m_nUseCapOLC==0) nVideoRate=nBitrateGet;
#endif
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				////add
				//sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.maxBitRate",subBuf);
				//error = pvtBuildByPath(valH, nodeId,tempbuf, 0, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

		case VIDEC_CODEC_H264_SVC:
			{	
				iEntryNumber = capVideo.nQCIF;
				iCapIndex ++;
				int nValue=nHas4CIF;

				DualVideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

				sprintf(tempbuf, "%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[10]={0x00,0x08,0x81,0x71,0x00,0x00,0x01,0};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf,
					"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.1.parameterIdentifier.standard",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 41, NULL);	


				sprintf(tempbuf,
					"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.1.parameterValue.booleanArray",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 8, NULL);	


				sprintf(tempbuf,
					"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.2.parameterIdentifier.standard",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 42, NULL);	


				sprintf(tempbuf,
					"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.2.parameterValue.unsignedMin",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, nValue, NULL);	

				//char *szPath=tempbuf;
				//int nNode=nodeId;
				//HPVT hPvt=valH;
				//char tempStr[512]={0};
				//int nCount=0;
				//struct tagCollapsing
				//{
				//	int standard;
				//	int value;
				//};
				//sprintf(tempStr,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing",subBuf);
				////notice set standard value
				//tagCollapsing Collapsing[]={ {6,80} };
				//nCount=sizeof(Collapsing)/sizeof(tagCollapsing);

				char *szPath=tempbuf;
				int nNode=nodeId;
				HPVT hPvt=valH;
				char tempStr[512]={0};
				int nCount=0;
				sprintf(tempStr,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing",subBuf);
				//notice set standard value
				tagCollapsing Collapsing[16]={0};
				int nBitrateGet= SetH264CapOptionParam(nValue,Collapsing,nCount,3,capVideo.VideoCodecType,iEntryNumber);

				for(int j=0;j<nCount;j++)
				{
					sprintf(szPath,"%s.%u.parameterIdentifier.standard",tempStr,j+3);
					error = pvtBuildByPath(hPvt, nNode, szPath, Collapsing[j].standard, NULL);
					sprintf(szPath,"%s.%u.parameterValue.unsignedMin",tempStr,j+3);
					error = pvtBuildByPath(hPvt, nNode, szPath, Collapsing[j].value, NULL);
				}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

				if(m_nUseCapOLC==0) nVideoRate=nBitrateGet;
#endif

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		default:
			break;
		}


	}
	if(iAlternative>1)
	{
		nSimultaneous++;
	}

	//设置远遥同时能力集
	iAlternative = 1;
	for(i=0; i<m_DataCap.m_nCapNum; i++)
	{			
		switch(m_DataCap.m_Caps[i].DataCodecID)
		{
		case DATA_CODEC_H224:
			iEntryNumber = H224_ENTRY;
			iCapIndex ++;

			DataEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex);

			sprintf(tempbuf, "%s.application.h224.hdlcFrameTunnelling", subBuf);
			error = pvtBuildByPath(valH, nodeId, tempbuf, NULL, NULL);
			sprintf(tempbuf, "%s.maxBitRate", subBuf);
			error = pvtBuildByPath(valH, nodeId, tempbuf, 480, NULL);

			//备用
			//{
			//sprintf(tempbuf, "%s.application.genericDataCapability.capabilityIdentifier.standard", subBuf);
			//unsigned char pValue[10]={0x00,0x08,0x81,0x60,0x01,0x00,0};
			//error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)pValue);
			//sprintf(tempbuf, "%s.maxBitRate", subBuf);
			//error = pvtBuildByPath(valH, nodeId, tempbuf,50, NULL);
			//}
			//设置同时能力集
			DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
			break;
		default:
			break;
		}

	}
	if(iAlternative>1)
	{
		nSimultaneous++;
	}


	//add by Jan for DTMF [20140730]
	if(m_nUseCapOLC==0)
	{
		iEntryNumber = DTMF_ENTRY;
		iAlternative = 1;
		iCapIndex ++;
		sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);

		sprintf(tempbuf, "capabilityTable.%d.capability.receiveUserInputCapability.dtmf", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, NULL, NULL);

		DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

		if(iAlternative>1)
		{
			nSimultaneous++;
		}
	}
	//end [20140730]

	//add by Jan for add cap [20130903] //add tai er ru wang test use
#if defined(TAIERLAB) && (TAIERLAB == 1)
	if(m_nUseCapOLC==0)
	{
		//UserInputCap
		iEntryNumber = USERINPUT_ENTRY;
		iAlternative = 1;
		iCapIndex ++;
		sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);

		sprintf(tempbuf, "capabilityTable.%d.capability.receiveUserInputCapability.basicString", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, NULL, NULL);

		DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

		if(iAlternative>1)
		{
			nSimultaneous++;
		}

		//ConfCap
		iEntryNumber = CONF_ENTRY;
		iAlternative = 1;
		iCapIndex ++;
		sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);

		sprintf(tempbuf, "capabilityTable.%d.capability.conferenceCapability", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, NULL, NULL);

		sprintf(tempbuf,"chairControlCapability");
		error = pvtBuildByPath(valH, error,tempbuf, RV_FALSE, NULL);

		DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

		if(iAlternative>1)
		{
			nSimultaneous++;
		}
	}

#endif

	//end [20130903]

#endif

	return 0;
}



int CallCapability::RemoveAllCapabilitis()                   //移除所有能力集
{
	int result = 0;

	m_AudioCap.m_nCapNum=0;
	m_VideoCap.m_nCapNum=0;
	m_H239VideoCap.m_nCapNum=0;
	m_DataCap.m_nCapNum=0;
	result=1;
	return result;

}

int CallCapability::OperationVideoCap(eCapNum capIndex,int operation,BOOL hasQCIF,BOOL hasCIF,BOOL has4CIF)                         //添加H264能力 CIF  
{
	int result = 0;
	VIDEO_CAPABILITY_ITEM  capability;
	memset(&capability,0,sizeof(VIDEO_CAPABILITY_ITEM));

	//用于H261,H263
	int nHasQCIF=1,nHasCIF=1,nHas4CIF=1; //H3C nHas4CIF 2 --> 1
	if(operation==1)
	{
		nHasQCIF=0,nHasCIF=0,nHas4CIF=0;
		if(hasQCIF) nHasQCIF=1;
		if(hasCIF)  nHasCIF=1;
		if(has4CIF) nHas4CIF=1; //H3C nHas4CIF 2 --> 1
	}

	switch(capIndex)
	{
	case CAP_H264_QCIF:
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_QCIF;	
		capability.n4CIF = H264_LEVEL_QCIF;  //H264此用于LEVEL
		capability.nQCIF = H264_ENTRY_QCIF;  //H264此用于ENTRY
		capability.nCIF = CAP_H264_QCIF;     //H264此用于CAP TYPE
		break;
	case CAP_H264_CIF:
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_CIF;	  
		capability.n4CIF = H264_LEVEL_CIF;    //H264此用于LEVEL
		capability.nQCIF = H264_ENTRY_CIF;    //H264此用于ENTRY
		capability.nCIF = CAP_H264_CIF;       //H264此用于CAP TYPE
		break; 
	case CAP_H264_4CIF: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_4CIF;	
		capability.n4CIF = H264_LEVEL_4CIF;
		capability.nQCIF = H264_ENTRY_4CIF;
		capability.nCIF = CAP_H264_4CIF;
		break; 
	case CAP_H264_16CIF: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_16CIF;	
		capability.n4CIF = H264_LEVEL_16CIF;
		capability.nQCIF = H264_ENTRY_16CIF;
		capability.nCIF = CAP_H264_16CIF;
		break; 
	case CAP_H264_720I: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_720I;	
		capability.n4CIF = H264_LEVEL_720I;
		capability.nQCIF = H264_ENTRY_720I;
		capability.nCIF = CAP_H264_720I;
		break; 
	case CAP_H264_720P: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_720P;	
		capability.n4CIF = H264_LEVEL_720P;
		capability.nQCIF = H264_ENTRY_720P;
		capability.nCIF = CAP_H264_720P;
		break; 
	case CAP_H264_1080I: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_1080I;	
		capability.n4CIF = H264_LEVEL_1080I;
		capability.nQCIF = H264_ENTRY_1080I;
		capability.nCIF = CAP_H264_1080I;
		break; 
	case CAP_H264_1080P: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_1080P;	
		capability.n4CIF = H264_LEVEL_1080P;
		capability.nQCIF = H264_ENTRY_1080P;
		capability.nCIF = CAP_H264_1080P;
		break;
	case CAP_H264_VGA: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_VGA;	
		capability.n4CIF = H264_LEVEL_VGA;
		capability.nQCIF = H264_ENTRY_VGA;
		capability.nCIF = CAP_H264_VGA;
		break; 
	case CAP_H264_SVGA:
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_SVGA;	
		capability.n4CIF = H264_LEVEL_SVGA;
		capability.nQCIF = H264_ENTRY_SVGA;
		capability.nCIF = CAP_H264_SVGA;
		break; 
	case CAP_H264_XGA: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_XGA;	
		capability.n4CIF = H264_LEVEL_XGA;
		capability.nQCIF = H264_ENTRY_XGA;
		capability.nCIF = CAP_H264_XGA;
		break; 
	case CAP_H264_SXGA: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_SXGA;	
		capability.n4CIF = H264_LEVEL_SXGA;
		capability.nQCIF = H264_ENTRY_SXGA;
		capability.nCIF = CAP_H264_SXGA;
		break;

	case CAP_H264_HIG720P: 
		capability.VideoCodecType = VIDEC_CODEC_H264_SVC;
		capability.nMaxBitrate = H264_MBPS_720P /2;	
		capability.n4CIF = H264_LEVEL_720P;
		capability.nQCIF = H264_ENTRY_HIG720P;
		capability.nCIF = CAP_H264_HIG720P;
		break;
	case CAP_H264_HIG1080P: 
		capability.VideoCodecType = VIDEC_CODEC_H264_SVC;
		capability.nMaxBitrate = H264_MBPS_1080P /2;	
		capability.n4CIF = H264_LEVEL_1080P;
		capability.nQCIF = H264_ENTRY_HIG1080P;
		capability.nCIF = CAP_H264_HIG1080P;
		break;
	case CAP_H263: 
		capability.VideoCodecType = VIDEC_CODEC_H263;
		capability.nMaxBitrate = H263_MBPS_ALL;
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		capability.n4CIF = nHas4CIF;
		break; 
	case CAP_H263v2: 
		capability.VideoCodecType = VIDEC_CODEC_H263P;
		capability.nMaxBitrate = H263_MBPS_ALL;
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		capability.n4CIF = nHas4CIF;
		break; 
	case CAP_H263v3: 
		capability.VideoCodecType = VIDEC_CODEC_H263PP;
		capability.nMaxBitrate = H263_MBPS_ALL;
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		capability.n4CIF = nHas4CIF;
		break; 
	case CAP_H261: 
		capability.VideoCodecType = VIDEC_CODEC_H261;
		capability.nMaxBitrate = H263_MBPS_ALL;
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		break;
	default:break;
	}

//modify 20140707
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	int nFrame=30;
	GetH264SVCVideoParam(2,capability.VideoCodecType,nFrame,capability.n4CIF,capability.nMaxBitrate);
#endif

	if(operation == 1) result=AddVideoCap(capability);  //添加
	if(operation == 2) result=SetVideoCap(1,capability); //首选
	if(operation == 3) result=SetVideoCap(2,capability); //移除

	return result; 
} 

int CallCapability::OperationAudioCap(eCapNum capIndex,int operation)                         //添加H264能力 CIF  
{
	int result = 0;

	AUDIO_CAPABILITY_ITEM  capability;
	memset(&capability,0,sizeof(AUDIO_CAPABILITY_ITEM));
	switch(capIndex)
	{
	case CAP_G729A:
		capability.AudioCodecID = X_AUDIO_CODEC_G729;
		break;
	case CAP_G711A_64K: 
		capability.AudioCodecID = X_AUDIO_CODEC_G711A;
		break; 
	case CAP_G711U_64K: 
		capability.AudioCodecID = X_AUDIO_CODEC_G711U;
		break; 
	case CAP_G7221_24K: 
		capability.AudioCodecID = X_AUDIO_CODEC_G7221_7_24;
		break; 
	case CAP_G7221_32K: 
		capability.AudioCodecID = X_AUDIO_CODEC_G7221_7_32;
		break;

	case CAP_G7221C_32K: 
		capability.AudioCodecID = X_AUDIO_CODEC_G7221_14_32;
		break; 
	case CAP_G7221C_24K: 
		capability.AudioCodecID = X_AUDIO_CODEC_G7221_14_24;
		break; 
	case CAP_G7221C_48K: 
		capability.AudioCodecID = X_AUDIO_CODEC_G7221_14_48;
		break; 

	case CAP_G722_64K: 
		capability.AudioCodecID = X_AUDIO_CODEC_G722_64;
		break; 
	case CAP_GSM: 
		capability.AudioCodecID = X_AUDIO_CODEC_GSM;
		break;
	case CAP_G719_32K: 
		capability.AudioCodecID = X_AUDIO_CODEC_G719_32;
		break; 
	case CAP_G719_48K: 
		capability.AudioCodecID = X_AUDIO_CODEC_G719_48;
		break; 
	case CAP_G719_64K: 
		capability.AudioCodecID = X_AUDIO_CODEC_G719_64;
		break; 
	//case CAP_G7231: 
	//	capability.AudioCodecID = X_AUDIO_CODEC_G7231_63;
	//	break; 
	default:break;
	}

	if(operation == 1) result=AddAudioCap(capability);  //添加
	if(operation == 2) result=SetAudioCap(1,capability); //首选
	if(operation == 3) result=SetAudioCap(2,capability); //移除

	return result; 
} 



int CallCapability::OperationH239Cap(eCapNum capIndex,int operation,BOOL hasQCIF,BOOL hasCIF,BOOL has4CIF)                    
{
	int result = 0;

	VIDEO_CAPABILITY_ITEM  capability;
	memset(&capability,0,sizeof(VIDEO_CAPABILITY_ITEM));

	//用于H261,H263
	int nHasQCIF=1,nHasCIF=1,nHas4CIF=2;
	if(operation==1)
	{
		nHasQCIF=0,nHasCIF=0,nHas4CIF=0;
		if(hasQCIF) nHasQCIF=1;
		if(hasCIF)  nHasCIF=1;
		if(has4CIF) nHas4CIF=2;
	}

	switch(capIndex)
	{
	case CAP_H264EXTEND_QCIF:
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_QCIF;	
		capability.n4CIF = H264_LEVEL_QCIF;  //H264此用于LEVEL
		capability.nQCIF = H264_ENTRY_QCIF+20000;  //H264此用于ENTRY
		capability.nCIF = CAP_H264EXTEND_QCIF+18;     //H264此用于CAP TYPE
		break; 
	case CAP_H264EXTEND_CIF:
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_CIF;	  
		capability.n4CIF = H264_LEVEL_CIF;    //H264此用于LEVEL
		capability.nQCIF = H264_ENTRY_CIF+20000;    //H264此用于ENTRY
		capability.nCIF = CAP_H264EXTEND_CIF+18;       //H264此用于CAP TYPE
		break; 
	case CAP_H264EXTEND_4CIF: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_4CIF;	
		capability.n4CIF = H264_LEVEL_4CIF;
		capability.nQCIF = H264_ENTRY_4CIF+20000;
		capability.nCIF = CAP_H264EXTEND_4CIF+18;
		break; 
	case CAP_H264EXTEND_16CIF: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_16CIF;	
		capability.n4CIF = H264_LEVEL_16CIF;
		capability.nQCIF = H264_ENTRY_16CIF+20000;
		capability.nCIF = CAP_H264EXTEND_16CIF+18;
		break; 
	case CAP_H264EXTEND_720I: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_720I;	
		capability.n4CIF = H264_LEVEL_720I;
		capability.nQCIF = H264_ENTRY_720I+20000;
		capability.nCIF = CAP_H264EXTEND_720I+18;
		break; 
	case CAP_H264EXTEND_720P: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_720P;	
		capability.n4CIF = H264_LEVEL_720P;
		capability.nQCIF = H264_ENTRY_720P+20000;
		capability.nCIF = CAP_H264EXTEND_720P+18;
		break; 
	case CAP_H264EXTEND_1080I: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_1080I;	
		capability.n4CIF = H264_LEVEL_1080I;
		capability.nQCIF = H264_ENTRY_1080I+20000;
		capability.nCIF = CAP_H264EXTEND_1080I+18;
		break; 
	case CAP_H264EXTEND_1080P: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_1080P;	
		capability.n4CIF = H264_LEVEL_1080P;
		capability.nQCIF = H264_ENTRY_1080P+20000;
		capability.nCIF = CAP_H264EXTEND_1080P+18;
		break;
	case CAP_H264EXTEND_VGA: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_VGA;	
		capability.n4CIF = H264_LEVEL_VGA;
		capability.nQCIF = H264_ENTRY_VGA+20000;
		capability.nCIF = CAP_H264EXTEND_VGA+18;
		break; 
	case CAP_H264EXTEND_SVGA:
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_SVGA;	
		capability.n4CIF = H264_LEVEL_SVGA;
		capability.nQCIF = H264_ENTRY_SVGA+20000;
		capability.nCIF = CAP_H264EXTEND_SVGA+18;
		break; 
	case CAP_H264EXTEND_XGA: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_XGA;	
		capability.n4CIF = H264_LEVEL_XGA;
		capability.nQCIF = H264_ENTRY_XGA+20000;
		capability.nCIF = CAP_H264EXTEND_XGA+18;
		break; 
	case CAP_H264EXTEND_SXGA: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_SXGA;	
		capability.n4CIF = H264_LEVEL_SXGA;
		capability.nQCIF = H264_ENTRY_SXGA+20000;
		capability.nCIF = CAP_H264EXTEND_SXGA+18;
		break;
	case CAP_H264EXTEND_HIG720P: 
		capability.VideoCodecType = VIDEC_CODEC_H264_SVC;
		capability.nMaxBitrate = H264_MBPS_720P /2;	
		capability.n4CIF = H264_LEVEL_720P;
		capability.nQCIF = H264_ENTRY_HIG720P+20000;
		capability.nCIF = CAP_H264EXTEND_HIG720P;
		break;
	case CAP_H264EXTEND_HIG1080P: 
		capability.VideoCodecType = VIDEC_CODEC_H264_SVC;
		capability.nMaxBitrate = H264_MBPS_1080P /2;	
		capability.n4CIF = H264_LEVEL_1080P;
		capability.nQCIF = H264_ENTRY_HIG1080P+20000;
		capability.nCIF = CAP_H264EXTEND_HIG1080P;
		break;
	case CAP_H263EXTEND:
		capability.VideoCodecType = VIDEC_CODEC_H263;
		capability.nMaxBitrate = H263_MBPS_ALL;	//384Kbps
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		capability.n4CIF = nHas4CIF;
		break;
	case CAP_H263v2EXTEND:
		capability.VideoCodecType = VIDEC_CODEC_H263P;
		capability.nMaxBitrate = H263_MBPS_ALL;	//384Kbps
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		capability.n4CIF = nHas4CIF;
		break;
	case CAP_H263v3EXTEND:
		capability.VideoCodecType = VIDEC_CODEC_H263PP;
		capability.nMaxBitrate = H263_MBPS_ALL;	//384Kbps
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		capability.n4CIF = nHas4CIF;
		break;
	case CAP_H261EXTEND:
		capability.VideoCodecType = VIDEC_CODEC_H261;
		capability.nMaxBitrate = H263_MBPS_ALL;	//384Kbps
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		break;
	default:break;
	}
	
	//modify 20140707
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	int nFrame=30;
	GetH264SVCVideoParam(3,capability.VideoCodecType,nFrame,capability.n4CIF,capability.nMaxBitrate);
#endif

	if(operation == 1) result=AddH239Cap(capability);  //添加
	if(operation == 2) result=SetH239Cap(1,capability); //首选
	if(operation == 3) result=SetH239Cap(2,capability); //移除

	return result; 
}
int CallCapability::OperationDataCap(eCapNum capIndex,int operation)                         
{
	int result = 0;

	DATA_CAPABILITY_ITEM  capability;
	memset(&capability,0,sizeof(DATA_CAPABILITY_ITEM));

	switch(capIndex)
	{
	case CAP_H224:
		capability.DataCodecID = DATA_CODEC_H224;//远遥
		break; 
	default:break;
	}
	if(operation == 1) result=AddDataCap(capability);  //添加
	if(operation == 2) result=SetDataCap(1,capability); //首选
	if(operation == 3) result=SetDataCap(2,capability); //移除

	return result; 
}

int CallCapability::AddH264CIF()                         //添加H264能力 CIF  
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_CIF,1); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH264CIF()                      //移除H264能力 CIF
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_CIF,3); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH264FirstCIF()                    //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_CIF,2); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH264CIF()                     //H264 CIF能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_CIF);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH264CIF()                     //H264 CIF是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_CIF); 	
	return result; 
} 

int CallCapability::AddH264QCIF()                         //添加H264能力 CIF  
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_QCIF,1); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH264QCIF()                      //移除H264能力 CIF
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_QCIF,3); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH264FirstQCIF()                    //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_QCIF,2); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH264QCIF()                     //H264 CIF能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_QCIF);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH264QCIF()                     //H264 CIF是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_QCIF); 	
	return result; 
} 


int CallCapability::AddH2644CIF()
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_4CIF,1); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH2644CIF()
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_4CIF,3); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH264First4CIF()
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_4CIF,2); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH2644CIF()                     //H264 4CIF能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_4CIF);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH2644CIF()                     //H264 4CIF是否为同时能力项首选
{
	int result = 0;   
	result=IsFirstSimul(2,H264_ENTRY_4CIF); 		
	return result; 
} 
int CallCapability::AddH26416CIF()
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_16CIF,1); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH26416CIF()
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_16CIF,3); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH264First16CIF()
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_16CIF,2);				//1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH26416CIF()                     //H264 16CIF能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_16CIF);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH26416CIF()                     //H264 16CIF是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_16CIF); 		
	return result; 
}
int CallCapability::AddH264720P()                         //添加H264能力 720P
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_720P,1);					//1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH264720P()                      //移除H264能力 720P
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_720P,3);				//1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH264First720P()                    //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_720P,2);				//1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH264720P()                     //H264 720P能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_720P);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH264720P()                     //H264 720P是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_720P); 		
	return result; 
} 
int CallCapability::AddH264720I()                         //添加H264能力 720I
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_720I,1);				//1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH264720I()                      //移除H264能力 720I
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_720I,3);				//1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH264First720I()                    //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_720I,2);					//1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH264720I()                     //H264 720I能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_720I);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH264720I()                     //H264 720I是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_720I); 		
	return result; 
} 
int CallCapability::AddH2641080P()                         //添加H264能力 1080P 
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_1080P,1); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH2641080P()                      //移除H264能力 1080P
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_1080P,3); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH264First1080P()                    //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_1080P,2); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH2641080P()                     //H264 1080P能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_1080P);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH2641080P()                     //H264 1080P是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_1080P); 		
	return result; 
} 
int CallCapability::AddH2641080I()                         //添加H264能力 1080I 
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_1080I,1); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH2641080I()                      //移除H264能力 1080I
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_1080I,3); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH264First1080I()                    //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_1080I,2); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH2641080I()                     //H264 1080I能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_1080I);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH2641080I()                     //H264 1080I是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_1080I); 		
	return result; 
} 
int CallCapability::AddH264VGA()                           //添加H264能力 VGA 
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_VGA,1); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH264VGA()                        //移除H264能力 VGA
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_VGA,3); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH264FirstVGA()                      //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_VGA,2); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH264VGA()                       //H264 VGA能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_VGA);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH264VGA()                       //H264 VGA是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_VGA); 		
	return result; 
} 
int CallCapability::AddH264SVGA()                           //添加H264能力 SVGA 
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_SVGA,1); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH264SVGA()                        //移除H264能力 SVGA
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_SVGA,3); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH264FirstSVGA()                      //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_SVGA,2); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH264SVGA()                       //H264 SVGA能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_SVGA);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH264SVGA()                       //H264 SVGA是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_SVGA); 		
	return result; 
} 
int CallCapability::AddH264XGA()                           //添加H264能力 XGA 
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_XGA,1);					//1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH264XGA()                        //移除H264能力 XGA
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_XGA,3);					//1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH264FirstXGA()                      //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_XGA,2);					//1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH264XGA()                       //H264 XGA能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_XGA);					 //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH264XGA()                       //H264 XGA是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_XGA); 		
	return result; 
} 

int CallCapability::AddH264SXGA()                           //添加H264能力 SXGA 
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_SXGA,1);					//1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH264SXGA()                        //移除H264能力 SXGA
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_SXGA,3);					//1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH264FirstSXGA()                      //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_SXGA,2);					//1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH264SXGA()                       //H264 SXGA能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_SXGA);						//视频能力是否存在
	return result; 
} 



int CallCapability::AddH264720PHighProfile()                           //添加H264能力 HighProfile 
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_HIG720P,1);					//1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH264720PHighProfile()								//移除H264能力 HighProfile
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_HIG720P,3);					//1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetFirstH264720PHighProfile()							//设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_HIG720P,2);					//1添加 2首选 3移除
	return result; 
}


int CallCapability::IsExistMainVideoH264720PHighProfile()                       //H264 HighProfile能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_HIG720P);						//视频能力是否存在
	return result; 
} 

int CallCapability::IsFirstMainVideoH264720PHighProfile()                       //H264 HighProfile是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_HIG720P); 		
	return result; 
} 






int CallCapability::AddH2641080PHighProfile()                           //添加H264能力 HighProfile 
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_HIG1080P,1);							//1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH2641080PHighProfile()								//移除H264能力 HighProfile
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_HIG1080P,3);					//1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetFirstH2641080PHighProfile()							//设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H264_HIG1080P,2);					//1添加 2首选 3移除
	return result; 
}

int CallCapability::IsExistMainVideoH2641080PHighProfile()                //H264 HighProfile能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,H264_ENTRY_HIG1080P);						//视频能力是否存在
	return result; 
} 

int CallCapability::IsFirstMainVideoH2641080PHighProfile()					//H264 HighProfile是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_HIG1080P); 		
	return result; 
} 


int CallCapability::IsExistMainVideoH264HighProfile()                //H264 HighProfile能力是否存在
{
	int result = 0;
	if(IsExistSimul(2,H264_ENTRY_HIG720P))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_HIG1080P))
	{ result=1; }

	return result;
} 

int CallCapability::IsFirstMainVideoH264HighProfile()					//H264 HighProfile是否为同时能力项首选
{
	int result = 0;
	result = IsFirstMainVideoH264720PHighProfile();
	result |= IsFirstMainVideoH2641080PHighProfile();
	return result; 
} 

int CallCapability::AddH239VideoH264720PHighProfile()                           //添加H264能力 HighProfile 
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_HIG720P,1);						//1添加 2首选 3移除
	return result; 
}

int CallCapability::RemoveH239VideoH264720PHighProfile()                        //移除H264能力 HighProfile
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_HIG720P,3);						//1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetFirstH239VideoH264720PHighProfile()                      //设置同时能力项为首选
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_HIG720P,2);						//1添加 2首选 3移除
	return result; 
} 

int CallCapability::IsExistH239VideoH264720PHighProfile()                       //H264 HighProfile能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_HIG720P+20000);						//视频能力是否存在
	return result; 
} 

int CallCapability::IsFirstH239VideoH264720PHighProfile()                       //H264 HighProfile是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_HIG720P+20000); 		
	return result; 
} 


int CallCapability::AddH239VideoH2641080PHighProfile()                           //添加H264能力 HighProfile 
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_HIG1080P,1);						//1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH239VideoH2641080PHighProfile()                        //移除H264能力 HighProfile
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_HIG1080P,3);						//1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetFirstH239VideoH2641080PHighProfile()                      //设置同时能力项为首选
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_HIG1080P,2);						//1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH239VideoH2641080PHighProfile()                       //H264 HighProfile能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_HIG1080P+20000);						//视频能力是否存在
	return result; 
} 

int CallCapability::IsFirstH239VideoH2641080PHighProfile()                       //H264 HighProfile是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_HIG1080P+20000); 		
	return result; 
} 


int CallCapability::IsExistH239VideoH264HighProfile()                       //H264 HighProfile能力是否存在
{
	int result = 0;
	if(IsExistSimul(3,H264_ENTRY_HIG720P+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_HIG1080P+20000))
	{ result=1; }

	return result; 
} 

int CallCapability::IsFirstH239VideoH264HighProfile()                       //H264 HighProfile是否为同时能力项首选
{
	int result = 0;
	result = IsFirstH239VideoH264720PHighProfile();
	result |= IsFirstH239VideoH2641080PHighProfile();
	return result;
} 


int CallCapability::IsExistMainVideoH264BaseLineProfile()                       //H264 BaseLineProfile能力是否存在
{
	int result = 0;
	if(IsExistSimul(2,H264_ENTRY_SXGA))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_XGA))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_720P))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_720I))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_SVGA))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_VGA))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_1080I))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_1080P))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_CIF))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_4CIF))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_QCIF))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_16CIF))
	{ result=1; }
	return result; 
} 

int CallCapability::IsFirstMainVideoH264BaseLineProfile()                       //H264 BaseLineProfile是否为同时能力项首选
{
	int nResult=IsFirstH264QCIF();
	nResult|=IsFirstH264CIF();
	nResult|=IsFirstH2644CIF();
	nResult|=IsFirstH26416CIF();
	nResult|=IsFirstH264720P();
	nResult|=IsFirstH264720I();
	nResult|=IsFirstH2641080P();
	nResult|=IsFirstH2641080I();
	nResult|=IsFirstH264VGA();
	nResult|=IsFirstH264SVGA();
	nResult|=IsFirstH264XGA();
	nResult|=IsFirstH264SXGA();
	return nResult;
} 


//notice 20111227 update

int CallCapability::IsExistH239VideoH264BaseLineProfile()                       //H264 BaseLineProfile能力是否存在
{
	int result = 0;
	if(IsExistSimul(3,H264_ENTRY_SXGA+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_XGA+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_720P+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_720I+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_SVGA+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_VGA+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_1080I+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_1080P+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_CIF+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_4CIF+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_QCIF+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_16CIF+20000))
	{ result=1; }

	return result; 
} 

int CallCapability::IsFirstH239VideoH264BaseLineProfile()                       //H264 BaseLineProfile是否为同时能力项首选
{
	int nResult=IsFirstH239VideoH264QCIFExtend();
	nResult|=IsFirstH239VideoH264CIFExtend();
	nResult|=IsFirstH239VideoH2644CIFExtend();
	nResult|=IsFirstH239VideoH26416CIFExtend();
	nResult|=IsFirstH239VideoH264720PExtend();
	nResult|=IsFirstH239VideoH264720IExtend();
	nResult|=IsFirstH239VideoH2641080PExtend();
	nResult|=IsFirstH239VideoH2641080IExtend();
	nResult|=IsFirstH239VideoH264VGAExtend();
	nResult|=IsFirstH239VideoH264SVGAExtend();
	nResult|=IsFirstH239VideoH264XGAExtend();
	nResult|=IsFirstH239VideoH264SXGAExtend();
	return nResult;
} 





int CallCapability::IsExistH264()                          //任一H264能力是否存在
{
	int result = 0;
	if(IsExistSimul(2,H264_ENTRY_SXGA))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_XGA))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_720P))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_720I))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_SVGA))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_VGA))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_1080I))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_1080P))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_CIF))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_4CIF))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_QCIF))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_16CIF))
	{ result=1; }
	//notice 20111227 update
	else if(IsExistSimul(2,H264_ENTRY_HIG720P))
	{ result=1; }
	else if(IsExistSimul(2,H264_ENTRY_HIG1080P))
	{ result=1; }

	return result; 
} 

int CallCapability::IsFirstH264(void)
{
	int nResult=IsFirstH264QCIF();
	nResult|=IsFirstH264CIF();
	nResult|=IsFirstH2644CIF();
	nResult|=IsFirstH26416CIF();
	nResult|=IsFirstH264720P();
	nResult|=IsFirstH264720I();
	nResult|=IsFirstH2641080P();
	nResult|=IsFirstH2641080I();
	nResult|=IsFirstH264VGA();
	nResult|=IsFirstH264SVGA();
	nResult|=IsFirstH264XGA();
	nResult|=IsFirstH264SXGA();
	//notice 20111227 update
	nResult|=IsFirstMainVideoH264HighProfile();
	return nResult;
}


int CallCapability::IsExistH239VideoH264Extend()                          //任一H264能力是否存在
{
	int result = 0;
	if(IsExistSimul(3,H264_ENTRY_SXGA+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_XGA+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_720P+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_720I+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_SVGA+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_VGA+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_1080I+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_1080P+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_CIF+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_4CIF+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_QCIF+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_16CIF+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_HIG720P+20000))
	{ result=1; }
	else if(IsExistSimul(3,H264_ENTRY_HIG1080P+20000))
	{ result=1; }
	return result; 
} 


int CallCapability::IsFirstH239VideoH264Extend(void)
{
	int nResult=IsFirstH239VideoH264QCIFExtend();
	nResult|=IsFirstH239VideoH264CIFExtend();
	nResult|=IsFirstH239VideoH2644CIFExtend();
	nResult|=IsFirstH239VideoH26416CIFExtend();
	nResult|=IsFirstH239VideoH264720PExtend();
	nResult|=IsFirstH239VideoH264720IExtend();
	nResult|=IsFirstH239VideoH2641080PExtend();
	nResult|=IsFirstH239VideoH2641080IExtend();
	nResult|=IsFirstH239VideoH264VGAExtend();
	nResult|=IsFirstH239VideoH264SVGAExtend();
	nResult|=IsFirstH239VideoH264XGAExtend();
	nResult|=IsFirstH239VideoH264SXGAExtend();
	//notice 20111227 update
	nResult|=IsFirstH239VideoH264HighProfile();
	return nResult;
}


int CallCapability::IsFirstH264SXGA()                       //H264 SXGA是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,H264_ENTRY_SXGA); 		
	return result; 
} 



int CallCapability::AddH261(BOOL hasQCIF,BOOL hasCIF)                           //添加H264能力 SXGA 
{
	int result = 0;
	result=OperationVideoCap(CAP_H261,1,hasQCIF,hasCIF); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH261()                        //移除H264能力 SXGA
{
	int result = 0;
	result=OperationVideoCap(CAP_H261,3); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH261First()                      //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H261,2); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH261()                       //H264 SXGA能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,261);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH261()                       //H264 SXGA是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,261); 		
	return result; 
} 

int CallCapability::AddH263(BOOL hasQCIF,BOOL hasCIF,BOOL has4CIF)                           //添加H264能力 SXGA 
{
	int result = 0;
	result=OperationVideoCap(CAP_H263,1,hasQCIF,hasCIF,has4CIF); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH263()                        //移除H264能力 SXGA
{
	int result = 0;
	result=OperationVideoCap(CAP_H263,3); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH263First()                      //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H263,2); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH263()                       //H264 SXGA能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,263);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH263()                       //H264 SXGA是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,263); 		
	return result; 
} 








int CallCapability::AddH263v2(BOOL hasQCIF,BOOL hasCIF,BOOL has4CIF)                           //添加H264能力 SXGA 
{
	int result = 0;
	result=OperationVideoCap(CAP_H263v2,1,hasQCIF,hasCIF,has4CIF); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH263v2()                        //移除H264能力 SXGA
{
	int result = 0;
	result=OperationVideoCap(CAP_H263v2,3); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH263v2First()                      //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H263v2,2); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH263v2()                       //H264 SXGA能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,2632);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH263v2()                       //H264 SXGA是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,2632); 		
	return result; 
} 



int CallCapability::AddH263v3(BOOL hasQCIF,BOOL hasCIF,BOOL has4CIF)                           //添加H264能力 SXGA 
{
	int result = 0;
	result=OperationVideoCap(CAP_H263v3,1,hasQCIF,hasCIF,has4CIF); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::RemoveH263v3()                        //移除H264能力 SXGA
{
	int result = 0;
	result=OperationVideoCap(CAP_H263v3,3); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::SetH263v3First()                      //设置同时能力项为首选
{
	int result = 0;
	result=OperationVideoCap(CAP_H263v3,2); //1添加 2首选 3移除
	return result; 
} 
int CallCapability::IsExistH263v3()                       //H264 SXGA能力是否存在
{
	int result = 0;
	result=IsExistSimul(2,2633);               //视频能力是否存在
	return result; 
} 
int CallCapability::IsFirstH263v3()                       //H264 SXGA是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(2,2633); 		
	return result; 
} 









//音频:
int CallCapability::AddG7221_32K()                        //添加G7221_32K能力
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221_32K,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG7221_32K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221_32K,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstG7221_32K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221_32K,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistG7221_32K()                      //G7221_32K能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,G7221_ENTRY_32K);               
	return result; 
} 
int CallCapability::IsFirstG7221_32K()                      //G7221_32K是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,G7221_ENTRY_32K); 		
	return result; 
} 
int CallCapability::AddG7221_24K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221_24K,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG7221_24K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221_24K,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstG7221_24K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221_24K,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistG7221_24K()                      //G7221_24K能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,G7221_ENTRY_24K);               
	return result; 
} 
int CallCapability::IsFirstG7221_24K()                      //G7221_24K是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,G7221_ENTRY_24K); 		
	return result; 
} 


int CallCapability::AddG7221C_24K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221C_24K,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG7221C_24K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221C_24K,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstG7221C_24K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221C_24K,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistG7221C_24K()                      //G7221_24K能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,G7221C_ENTRY_24K);               
	return result; 
} 
int CallCapability::IsFirstG7221C_24K()                      //G7221_24K是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,G7221C_ENTRY_24K); 		
	return result; 
} 





int CallCapability::AddG7221C_32K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221C_32K,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG7221C_32K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221C_32K,3); //1添加 2首选 3移除
	return result; 
}

int CallCapability::SetFirstG7221C_32K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221C_32K,2); //1添加 2首选 3移除
	return result; 
}

int CallCapability::IsExistG7221C_32K()                      //G7221_24K能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,G7221C_ENTRY_32K);               
	return result; 
}

int CallCapability::IsFirstG7221C_32K()                      //G7221_24K是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,G7221C_ENTRY_32K); 		
	return result; 
} 









int CallCapability::AddG7221C_48K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221C_48K,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG7221C_48K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221C_48K,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstG7221C_48K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7221C_48K,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistG7221C_48K()                      //G7221_24K能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,G7221C_ENTRY_48K);               
	return result; 
} 
int CallCapability::IsFirstG7221C_48K()                      //G7221_24K是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,G7221C_ENTRY_48K); 		
	return result; 
} 




//notice 20111227 update
int CallCapability::AddG719_64K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G719_64K,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG719_64K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G719_64K,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstG719_64K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G719_64K,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistG719_64K()                      //G7221_24K能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,G719_ENTRY_64K);               
	return result; 
} 
int CallCapability::IsFirstG719_64K()                      //G7221_24K是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,G719_ENTRY_64K); 		
	return result; 
} 


int CallCapability::AddG719_32K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G719_32K,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG719_32K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G719_32K,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstG719_32K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G719_32K,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistG719_32K()                      //G7221_24K能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,G719_ENTRY_32K);               
	return result; 
} 
int CallCapability::IsFirstG719_32K()                      //G7221_24K是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,G719_ENTRY_32K); 		
	return result; 
} 


int CallCapability::AddG719_48K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G719_48K,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG719_48K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G719_48K,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstG719_48K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G719_48K,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistG719_48K()                      //G7221_24K能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,G719_ENTRY_48K);               
	return result; 
} 
int CallCapability::IsFirstG719_48K()                      //G7221_24K是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,G719_ENTRY_48K); 		
	return result; 
} 













int CallCapability::AddG722_64K()                        //添加G7221_32K能力
{
	int result = 0;
	result=OperationAudioCap(CAP_G722_64K,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG722_64K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G722_64K,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstG722_64K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G722_64K,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistG722_64K()                      //G7221_32K能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,G722_ENTRY_64K);               
	return result; 
} 
int CallCapability::IsFirstG722_64K()                      //G7221_32K是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,G722_ENTRY_64K); 		
	return result; 
} 

int CallCapability::AddG7231()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7231,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG7231()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7231,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstG7231()
{
	int result = 0;
	result=OperationAudioCap(CAP_G7231,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistG7231()                          //G7231能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,G7231_ENTRY);               
	return result; 
} 
int CallCapability::IsFirstG7231()                          //G7231是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,G7231_ENTRY); 		
	return result; 
} 
int CallCapability::AddG729A()
{
	int result = 0;
	result=OperationAudioCap(CAP_G729A,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG729A()
{
	int result = 0;
	result=OperationAudioCap(CAP_G729A,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstG729A()
{
	int result = 0;
	result=OperationAudioCap(CAP_G729A,2); //1添加 2首选 3移除
	return result; 
}



int CallCapability::IsExistG729A(int type)                          //G729A能力是否存在
{
	int result = 0;
	if(type==2) 
		result=IsExistSimul(1,G729A_ENTRY);
	else 
		result=IsExistSimul(1,G729_ENTRY); 

	return result; 
} 
int CallCapability::IsFirstG729A(int type)                          //G729A是否为同时能力项首选
{
	int result = 0;
	if(type==2) 
		result=IsFirstSimul(1,G729A_ENTRY); 
	else 	
		result=IsFirstSimul(1,G729_ENTRY); 

	return result; 
}

//notice add 20120202
int CallCapability::AddGSM()
{
	int result = 0;
	result=OperationAudioCap((eCapNum)CAP_GSM,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveGSM()
{
	int result = 0;
	result=OperationAudioCap((eCapNum)CAP_GSM,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstGSM()
{
	int result = 0;
	result=OperationAudioCap((eCapNum)CAP_GSM,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistGSM()                      //G7221_24K能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,GSM_ENTRY);               
	return result; 
} 
int CallCapability::IsFirstGSM()                      //G7221_24K是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,GSM_ENTRY); 		
	return result; 
} 








int CallCapability::AddG711u_64K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G711U_64K,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG711u_64K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G711U_64K,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstG711u_64K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G711U_64K,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistG711u_64K()                      //G711u_64K能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,G711U_ENTRY_64K);               
	return result; 
} 
int CallCapability::IsFirstG711u_64K()                      //G711u_64K是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,G711U_ENTRY_64K); 		
	return result; 
} 
int CallCapability::AddG711a_64K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G711A_64K,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveG711a_64K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G711A_64K,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstG711a_64K()
{
	int result = 0;
	result=OperationAudioCap(CAP_G711A_64K,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistG711a_64K()                       //G711a_64K能力是否存在
{
	int result = 0;
	result=IsExistSimul(1,G711A_ENTRY_64K);               
	return result; 
} 
int CallCapability::IsFirstG711a_64K()                       //G711a_64K是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(1,G711A_ENTRY_64K); 		
	return result; 
} 



//H239
int CallCapability::AddH239VideoH264QCIFExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_QCIF,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::AddH239VideoH264CIFExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_CIF,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::AddH239VideoH2644CIFExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_4CIF,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::AddH239VideoH26416CIFExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_16CIF,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::AddH239VideoH264720PExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_720P,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::AddH239VideoH264720IExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_720I,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::AddH239VideoH2641080PExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_1080P,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::AddH239VideoH2641080IExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_1080I,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::AddH239VideoH264VGAExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_VGA,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::AddH239VideoH264SVGAExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_SVGA,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::AddH239VideoH264XGAExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_XGA,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::AddH239VideoH264SXGAExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_SXGA,1); //1添加 2首选 3移除
	return result; 
}


int CallCapability::RemoveH239VideoH264QCIFExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_QCIF,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH264CIFExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_CIF,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH2644CIFExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_4CIF,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH26416CIFExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_16CIF,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH264720PExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_720P,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH264720IExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_720I,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH2641080PExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_1080P,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH2641080IExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_1080I,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH264VGAExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_VGA,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH264SVGAExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_SVGA,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH264XGAExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_XGA,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH264SXGAExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_SXGA,3); //1添加 2首选 3移除
	return result; 
}


int CallCapability::SetFirstH239VideoH264QCIFExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_QCIF,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH264QCIFExtend()                       //H264Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_QCIF+20000);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH264QCIFExtend()                       //H264Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_QCIF+20000); 		
	return result; 
} 





int CallCapability::SetFirstH239VideoH264CIFExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_CIF,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH264CIFExtend()                       //H264Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_CIF+20000);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH264CIFExtend()                       //H264Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_CIF+20000); 		
	return result; 
} 





int CallCapability::SetFirstH239VideoH2644CIFExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_4CIF,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH2644CIFExtend()                       //H264Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_4CIF+20000);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH2644CIFExtend()                       //H264Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_4CIF+20000); 		
	return result; 
} 




int CallCapability::SetFirstH239VideoH26416CIFExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_16CIF,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH26416CIFExtend()                       //H264Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_16CIF+20000);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH26416CIFExtend()                       //H264Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_16CIF+20000); 		
	return result; 
} 





int CallCapability::SetFirstH239VideoH264720PExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_720P,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH264720PExtend()                       //H264Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_720P+20000);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH264720PExtend()                       //H264Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_720P+20000); 		
	return result; 
} 




int CallCapability::SetFirstH239VideoH264720IExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_720I,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH264720IExtend()                       //H264Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_720I+20000);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH264720IExtend()                       //H264Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_720I+20000); 		
	return result; 
} 




int CallCapability::SetFirstH239VideoH2641080PExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_1080P,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH2641080PExtend()                       //H264Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_1080P+20000);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH2641080PExtend()                       //H264Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_1080P+20000); 		
	return result; 
} 




int CallCapability::SetFirstH239VideoH2641080IExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_1080I,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH2641080IExtend()                       //H264Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_1080I+20000);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH2641080IExtend()                       //H264Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_1080I+20000); 		
	return result; 
} 




int CallCapability::SetFirstH239VideoH264VGAExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_VGA,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH264VGAExtend()                       //H264Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_VGA+20000);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH264VGAExtend()                       //H264Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_VGA+20000); 		
	return result; 
} 




int CallCapability::SetFirstH239VideoH264SVGAExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_SVGA,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH264SVGAExtend()                       //H264Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_SVGA+20000);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH264SVGAExtend()                       //H264Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_SVGA+20000); 		
	return result; 
} 




int CallCapability::SetFirstH239VideoH264XGAExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_XGA,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH264XGAExtend()                       //H264Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_XGA+20000);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH264XGAExtend()                       //H264Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_XGA+20000); 		
	return result; 
} 




int CallCapability::SetFirstH239VideoH264SXGAExtend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H264EXTEND_SXGA,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH264SXGAExtend()                       //H264Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,H264_ENTRY_SXGA+20000);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH264SXGAExtend()                       //H264Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,H264_ENTRY_SXGA+20000); 		
	return result; 
} 



int CallCapability::AddH239VideoH263Extend(BOOL hasQCIF,BOOL hasCIF,BOOL has4CIF)
{
	int result = 0;
	result=OperationH239Cap(CAP_H263EXTEND,1, hasQCIF, hasCIF, has4CIF); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH263Extend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H263EXTEND,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstH239VideoH263Extend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H263EXTEND,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH263Extend()                       //H263Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,2263);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH263Extend()                       //H263Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,2263); 		
	return result; 
} 


int CallCapability::AddH239VideoH263v2Extend(BOOL hasQCIF,BOOL hasCIF,BOOL has4CIF)
{
	int result = 0;
	result=OperationH239Cap(CAP_H263v2EXTEND,1,hasQCIF,hasCIF,has4CIF); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH263v2Extend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H263v2EXTEND,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstH239VideoH263v2Extend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H263v2EXTEND,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH263v2Extend()                       //H263Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,22632);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH263v2Extend()                       //H263Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,22632); 		
	return result; 
} 


int CallCapability::AddH239VideoH263v3Extend(BOOL hasQCIF,BOOL hasCIF,BOOL has4CIF)
{
	int result = 0;
	result=OperationH239Cap(CAP_H263v3EXTEND,1,hasQCIF,hasCIF,has4CIF); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH263v3Extend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H263v3EXTEND,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstH239VideoH263v3Extend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H263v3EXTEND,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH263v3Extend()                       //H263Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,22633);               
	return result; 
} 
int CallCapability::IsFirstH239VideoH263v3Extend()                       //H263Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,22633); 		
	return result; 
} 


int CallCapability::AddH239VideoH261Extend(BOOL hasQCIF,BOOL hasCIF)
{
	int result = 0;
	result=OperationH239Cap(CAP_H261EXTEND,1, hasQCIF, hasCIF); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveH239VideoH261Extend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H261EXTEND,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstH239VideoH261Extend()
{
	int result = 0;
	result=OperationH239Cap(CAP_H261EXTEND,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistH239VideoH261Extend()                       //H263Extend能力是否存在
{
	int result = 0;
	result=IsExistSimul(3,2261);               
	return result; 
}

int CallCapability::IsFirstH239VideoH261Extend()                       //H263Extend是否为同时能力项首选
{
	int result = 0;
	result=IsFirstSimul(3,2261); 		
	return result; 
} 

int CallCapability::AddFECCData()
{
	int result = 0;
	result=OperationDataCap(CAP_H224,1); //1添加 2首选 3移除
	return result; 
}
int CallCapability::RemoveFECCData()
{
	int result = 0;
	result=OperationDataCap(CAP_H224,3); //1添加 2首选 3移除
	return result; 
}
int CallCapability::SetFirstFECCData()
{
	int result = 0;
	result=OperationDataCap(CAP_H224,2); //1添加 2首选 3移除
	return result; 
}
int CallCapability::IsExistFECCData()                       //H224能力是否存在
{
	int result = 0;
	int type=4;
	result=IsExistSimul(type,H224_ENTRY);               
	return result; 
} 
int CallCapability::IsFirstFECCData()                       //H224是否为同时能力项首选
{
	int result = 0;
	int type=4;
	result=IsFirstSimul(type,H224_ENTRY); 		
	return result; 
} 

int CallCapability::RemoveAudioAllCapabilitis()                   //移除所有能力集
{
	int result = 0;
	m_AudioCap.m_nCapNum=0;
	result=1;
	return result;
}

int CallCapability::RemoveVideoAllCapabilitis()                   //移除所有能力集
{
	int result = 0;
	m_VideoCap.m_nCapNum=0;
	result=1;
	return result;
}

int CallCapability::RemoveDualVideoAllCapabilitis()                   //移除所有能力集
{
	int result = 0;
	m_H239VideoCap.m_nCapNum=0;
	result=1;
	return result;
}

int CallCapability::RemoveFECCDataAllCapabilitis()                   //移除所有能力集
{
	int result = 0;
	m_DataCap.m_nCapNum=0;
	result=1;
	return result;
}

int CallCapability::GetMainVideoMaxBitrate(int nVideoRate)
{
	if(m_nUseCapOLC==1)
		nVideoRate=m_nMainVideoMaxBitrate;
	if(m_nUseCapOLC==0)
	{
		//if(nVideoRate>H323CallMgr::Instance().GetMainVideoMaxBitrate())
		//	nVideoRate=H323CallMgr::Instance().GetMainVideoMaxBitrate();

		//大于最大码流时,取小值
		//if(nVideoRate> m_nMainVideoMaxBitrate)
			nVideoRate = m_nMainVideoMaxBitrate;
	}
	return nVideoRate;
}

int CallCapability::GetSecondVideoMaxBitrate(int nVideoRate)
{
	if(m_nUseCapOLC==1)
		nVideoRate=m_nSecondVideoMaxBitrate;
	if(m_nUseCapOLC==0)
	{
		//if(nVideoRate>H323CallMgr::Instance().GetSecondVideoMaxBitrate())
		//	nVideoRate=H323CallMgr::Instance().GetSecondVideoMaxBitrate();

		//大于最大码流时,取小值
		//if(nVideoRate> m_nSecondVideoMaxBitrate)
			nVideoRate = m_nSecondVideoMaxBitrate;
	}
	return nVideoRate;
}

void CallCapability::SetMainVideoMaxBitrate(int nMaxBitrate)
{
	m_nMainVideoMaxBitrate=nMaxBitrate;
}

void CallCapability::SetSecondVideoMaxBitrate(int nMaxBitrate)
{
	m_nSecondVideoMaxBitrate=nMaxBitrate;
}

void CallCapability::SetCapabilitisUseType(int nType)
{
	m_nUseCapOLC=nType;
}

int CallCapability::AddH264MainVideoCap(VIDEC_CODEC_TYPE codecType,int nLevel)                         //用于OLC 添加H264能力 level  
{
	int result = 0;
	VIDEO_CAPABILITY_ITEM  capability;
	memset(&capability,0,sizeof(VIDEO_CAPABILITY_ITEM));
	capability.VideoCodecType = codecType;
	capability.nMaxBitrate = m_nMainVideoMaxBitrate;	
	capability.n4CIF = nLevel;  //H264此用于LEVEL
	capability.nQCIF = H264_ENTRY_QCIF;  //H264此用于ENTRY
	capability.nCIF = CAP_H264_QCIF;     //H264此用于CAP TYPE
	result=AddVideoCap(capability);  //添加
	return result; 
}

int CallCapability::AddH264SecondVideoCap(VIDEC_CODEC_TYPE codecType,int nLevel)                         //用于OLC 添加H264能力 level  
{
	int result = 0;
	VIDEO_CAPABILITY_ITEM  capability;
	memset(&capability,0,sizeof(VIDEO_CAPABILITY_ITEM));
	capability.VideoCodecType = codecType;
	capability.nMaxBitrate = m_nSecondVideoMaxBitrate;	
	capability.n4CIF = nLevel;  //H264此用于LEVEL
	capability.nQCIF = H264_ENTRY_QCIF+20000;  //H264此用于ENTRY
	capability.nCIF = CAP_H264EXTEND_QCIF+18;     //H264此用于CAP TYPE
	result=AddH239Cap(capability);  //添加
	return result; 
}

void CallCapability::DescriptorBuild(HPVT valH,int nodeId,char * tempbuf,int iEntryNumber,int &nSimulNum,int &nSimultaneous,int &iAlternative)
{
#ifdef HAS_H323
	if(m_nUseCapOLC==0)
	{
		//设置同时能力集
		sprintf(tempbuf,"capabilityDescriptors.%d.simultaneousCapabilities.%d.%d",nSimulNum, nSimultaneous,iAlternative);

		int error = pvtBuildByPath( valH, nodeId,tempbuf,iEntryNumber, NULL); 
		if(error >= 0)
		{
			iAlternative++;
		}
	}
#endif

}

void CallCapability::VideoEntryBuild(HPVT valH,int nodeId,char * subBuf,char * tempbuf,int iEntryNumber,int iCapIndex,int nDir)
{
#ifdef HAS_H323
	char direction[64]={0};
	if(nDir ==1) strcpy(direction,"receiveVideoCapability");
	if(nDir ==2) strcpy(direction,"transmitVideoCapability");
	if(nDir ==3) strcpy(direction,"receiveAndTransmitVideoCapability");

	int error=0;
	if(m_nUseCapOLC==0)
	{
		sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);
	}
	if(m_nUseCapOLC==0)
		sprintf(subBuf,"capabilityTable.%d.capability.%s", iCapIndex,direction);
	else if(m_nUseCapOLC==1)
		sprintf(subBuf,"videoData");

#endif

}

void CallCapability::DualVideoEntryBuild(HPVT valH,int nodeId,char * subBuf,char * tempbuf,int iEntryNumber,int iCapIndex,int nDir)
{
	VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDir);
}

void CallCapability::AudioEntryBuild(HPVT valH,int nodeId,char * subBuf,char * tempbuf,int iEntryNumber,int iCapIndex,int nDir)
{
#ifdef HAS_H323

	char direction[64]={0};
	if(nDir ==1) strcpy(direction,"receiveAudioCapability");
	if(nDir ==2) strcpy(direction,"transmitAudioCapability");
	if(nDir ==3) strcpy(direction,"receiveAndTransmitAudioCapability");

	int error=0;
	if(m_nUseCapOLC==0)
	{
	sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
	error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);
	}
	if(m_nUseCapOLC==0)
		sprintf(subBuf,"capabilityTable.%d.capability.%s", iCapIndex,direction);
	else if(m_nUseCapOLC==1)
		sprintf(subBuf,"audioData");
#endif
}

void CallCapability::DataEntryBuild(HPVT valH,int nodeId,char * subBuf,char * tempbuf,int iEntryNumber,int iCapIndex,int nDir)
{
#ifdef HAS_H323
	char direction[64]={0};
	if(nDir ==1) strcpy(direction,"receiveDataApplicationCapability");
	if(nDir ==2) strcpy(direction,"transmitDataApplicationCapability");
	if(nDir ==3) strcpy(direction,"receiveAndTransmitDataApplicationCapability");

	int error=0;
	if(m_nUseCapOLC==0)
	{
		sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);
	}
	if(m_nUseCapOLC==0)
		sprintf(subBuf,"capabilityTable.%d.capability.%s", iCapIndex,direction);
		//sprintf(subBuf,"capabilityTable.%d.capability.receiveAndTransmitDataApplicationCapability", iCapIndex);
	else if(m_nUseCapOLC==1)
		sprintf(subBuf,"data");
#endif
}

void CallCapability::SetG729Type(int nType)
{
	m_nG729Type=nType;
}

void CallCapability::SetH264OptionParamMaxMBPS(int nValue)
{
	m_nOLCMaxMBPS=nValue;
}

void CallCapability::SetH264OptionParamMaxFS(int nValue)
{
	m_nOLCMaxFS=nValue;
}

void CallCapability::SetH264OptionParamMaxDPB(int nValue)
{
	m_nOLCMaxDPB=nValue;
}

void CallCapability::SetH264OptionParamMaxBR(int nValue)
{
	m_nOLCMaxBR=nValue;
}

VIDEO_FORMAT_TYPE CallCapability::GetVideoFormatTypeFromEntry(int type,int format)
{
	VIDEO_FORMAT_TYPE nFormatType =VIDEO_FORMAT_QCIF;
	//get format
	//双流对应减20000
	if(type ==3) format= format - 20000;
	switch(format)
	{
	case H264_ENTRY_QCIF:
		nFormatType=VIDEO_FORMAT_QCIF;
		break;
	case H264_ENTRY_CIF:
		nFormatType=VIDEO_FORMAT_CIF;
		break;
	case H264_ENTRY_4CIF:
		nFormatType=VIDEO_FORMAT_4CIF;
		break;
	case H264_ENTRY_16CIF:
		nFormatType=VIDEO_FORMAT_16CIF;
		break;
	case H264_ENTRY_720P:
	case H264_ENTRY_HIG720P:
		nFormatType=VIDEO_FORMAT_720P;
		break;
	case H264_ENTRY_720I:
		nFormatType=VIDEO_FORMAT_720I;
		break;
	case H264_ENTRY_1080P:
	case H264_ENTRY_HIG1080P:
		nFormatType=VIDEO_FORMAT_1080P;
		break;
	case H264_ENTRY_1080I:
		nFormatType=VIDEO_FORMAT_1080I;
		break;
	case H264_ENTRY_VGA:
		nFormatType=VIDEO_FORMAT_VGA;
		break;
	case H264_ENTRY_SVGA:
		nFormatType=VIDEO_FORMAT_SVGA;
		break;
	case H264_ENTRY_XGA:
		nFormatType=VIDEO_FORMAT_XGA;
		break;
	case H264_ENTRY_SXGA:
		nFormatType=VIDEO_FORMAT_SXGA;
		break;
	}

	return nFormatType;
}

void CallCapability::GetVideoSetOtherParam(int type,VIDEC_CODEC_TYPE codecType,VIDEO_FORMAT_TYPE nFormatType,int & frame,int & bitrate)
{
	for(int i=0;i< MAX_VIDEOCAP_NUM;i++)
	{
		if(type==2)
		{
			if(m_videoParam[i].nCodecType == codecType && m_videoParam[i].nFormat == nFormatType)
			{
				frame   =m_videoParam[i].frame;
				bitrate =m_videoParam[i].bitrate;
			}
		}
		else if(type==3)
		{
			if(m_dualParam[i].nCodecType == codecType && m_dualParam[i].nFormat == nFormatType)
			{
				frame   =m_dualParam[i].frame;
				bitrate =m_dualParam[i].bitrate;
			}
		}
	}
}

int CallCapability::SetH264CapOptionParam(int nValue,tagCollapsing Collapsing[],int & nCount,int type,VIDEC_CODEC_TYPE codecType,int format) //1 audio 2 video 3 H239 4 Data
{
	int nBitrateSet=0;

	float fScale=1.0;
	int nMaxMBPS=0;
	float fMaxFrame=30.0;
	float fFrameRate=0;
	//terminal or mp use
	if(type==2) fFrameRate=m_nMainVideoFrameRate;
	if(type==3) fFrameRate=m_nSecondVideoFrameRate;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	if(m_nUseCapOLC==0)
	{
		int nFrame=0;
		VIDEO_FORMAT_TYPE nFormatType=GetVideoFormatTypeFromEntry(type,format);
		GetVideoSetOtherParam(type,codecType,nFormatType,nFrame,nBitrateSet);
		//modify 20140707
		int nLevel=0;
		GetH264SVCVideoParam(type,codecType,nFrame,nLevel,nBitrateSet);
		fFrameRate=nFrame;
	}
#endif

	fScale=fFrameRate/fMaxFrame;
	nCount=0;
	if(m_nUseCapOLC==0)
	{
		//if(nValue <= H264_LEVEL_720P)
		//{
		//	nMaxMBPS=216 * fScale;
		//	tagCollapsing CollapsingTemp[]={ {3,nMaxMBPS},{4,15},{6,560} };
		//	nCount=sizeof(CollapsingTemp)/sizeof(tagCollapsing);
		//	memcpy(Collapsing,CollapsingTemp,sizeof(CollapsingTemp));
		//}
		//else if(nValue < H264_LEVEL_1080P)
		//{
		//	nMaxMBPS=492 * fScale;
		//	tagCollapsing CollapsingTemp[]={ {3,nMaxMBPS},{4,32},{6,1000} };
		//	nCount=sizeof(CollapsingTemp)/sizeof(tagCollapsing);
		//	memcpy(Collapsing,CollapsingTemp,sizeof(CollapsingTemp));
		//}
		//notice add set the modify 20140625
		int nMaxFS =15;
		GetVideoMaxFSMBPSFromEntry(nMaxMBPS,nMaxFS,type,format);
		if(nMaxFS > 0 && nMaxMBPS >0)
		{
			nMaxMBPS=nMaxMBPS * fScale;
			tagCollapsing CollapsingTemp[]={ {3,nMaxMBPS},{4,nMaxFS}};
			nCount=sizeof(CollapsingTemp)/sizeof(tagCollapsing);
			memcpy(Collapsing,CollapsingTemp,sizeof(CollapsingTemp));
		}
	}
	else
	{
		if(m_nOLCMaxMBPS>0)
		{
			Collapsing[nCount].standard=3;
			Collapsing[nCount].value=m_nOLCMaxMBPS * fScale;
			nCount++;
		}
		if(m_nOLCMaxFS>0)
		{
			Collapsing[nCount].standard=4;
			Collapsing[nCount].value=m_nOLCMaxFS;
			nCount++;
		}
		if(m_nOLCMaxDPB>0)
		{
			Collapsing[nCount].standard=5;
			Collapsing[nCount].value=m_nOLCMaxDPB;
			nCount++;
		}
		if(m_nOLCMaxBR>0)
		{
			Collapsing[nCount].standard=6;
			Collapsing[nCount].value=m_nOLCMaxBR;
			nCount++;
		}
	}

	return nBitrateSet;
}

void CallCapability::SetMainVideoFrameRate(int nFrame)
{
	m_nMainVideoFrameRate=nFrame;
}

void CallCapability::SetSecondVideoFrameRate(int nFrame)
{
	m_nSecondVideoFrameRate=nFrame;
}

void CallCapability::CopyMainVideoRateArray(MediaParam videoParam[])
{
	memcpy(m_videoParam,videoParam,sizeof(m_videoParam) );
}

void CallCapability::CopySecondVideoRateArray(MediaParam dualParam[])
{
	memcpy(m_dualParam,dualParam,sizeof(m_dualParam) );
}

//分治法递归求取数组中的最大和最小值
void GetMaxMinBitrateValue(int i,int j,int &fmax,int &fmin,MediaParam A[])
{
	if(i==j)
	{
		fmax=A[i].bitrate;
		fmin=A[i].bitrate;
	}
	else if(i==j-1)
	{
		if(A[i].bitrate < A[j].bitrate)
		{
			fmax=A[j].bitrate;
			fmin=A[i].bitrate;
		}
		else
		{
			fmax=A[i].bitrate;
			fmin=A[j].bitrate;
		}
	}
	else
	{
		int mid;
		int lmax,lmin,rmax,rmin;
		mid=(i+j)/2;
		GetMaxMinBitrateValue(i,mid,lmax,lmin,A);
		GetMaxMinBitrateValue(mid+1,j,rmax,rmin,A);
		if(lmax>rmax)
			fmax=lmax;
		else
			fmax=rmax;
		if(lmin>rmin)
			fmin=rmin;
		else 
			fmin=lmin;
	}
}

void GetMaxFormatValue(int &fmax,MediaParam videoParam[])
{
	//需要与VIDEO_FORMAT_TYPE保持一致
	typedef enum {
		VIDEO_FORMAT_QCIF_ORDER=0,	//25344
		VIDEO_FORMAT_QVGA_ORDER,	//76800
		VIDEO_FORMAT_CIF_ORDER,		//101376
		VIDEO_FORMAT_VGA_ORDER,		//307200
		VIDEO_FORMAT_4CIF_ORDER,	//405504
		VIDEO_FORMAT_720I_ORDER,	//460800
		VIDEO_FORMAT_SVGA_ORDER,	//480000
		VIDEO_FORMAT_XGA_ORDER,		//786432
		VIDEO_FORMAT_720P_ORDER,	//921600
		VIDEO_FORMAT_1080I_ORDER,	//1036800
		VIDEO_FORMAT_SXGA_ORDER,	//1310720
		VIDEO_FORMAT_16CIF_ORDER,	//1622016
		VIDEO_FORMAT_1080P_ORDER,	//2073600
		VIDEO_FORMAT_COUNT_ORDER
	}VIDEO_FORMAT_TYPE_ORDER;
	int formatOrider[VIDEO_FORMAT_COUNT_ORDER][2] = {{VIDEO_FORMAT_QCIF,VIDEO_FORMAT_QCIF_ORDER},
	{VIDEO_FORMAT_QVGA,VIDEO_FORMAT_QVGA_ORDER},{VIDEO_FORMAT_CIF,VIDEO_FORMAT_CIF_ORDER},
	{VIDEO_FORMAT_VGA,VIDEO_FORMAT_VGA_ORDER},{VIDEO_FORMAT_4CIF,VIDEO_FORMAT_4CIF_ORDER},
	{VIDEO_FORMAT_720I,VIDEO_FORMAT_720I_ORDER},{VIDEO_FORMAT_SVGA,VIDEO_FORMAT_SVGA_ORDER},
	{VIDEO_FORMAT_XGA,VIDEO_FORMAT_XGA_ORDER},{VIDEO_FORMAT_720P,VIDEO_FORMAT_720P_ORDER},
	{VIDEO_FORMAT_1080I,VIDEO_FORMAT_1080I_ORDER},{VIDEO_FORMAT_SXGA,VIDEO_FORMAT_SXGA_ORDER},
	{VIDEO_FORMAT_16CIF,VIDEO_FORMAT_16CIF_ORDER},{VIDEO_FORMAT_1080P,VIDEO_FORMAT_1080P_ORDER} };

	int index=0;
	int i=0;
	bool nFind=false;
	for(i=VIDEO_FORMAT_COUNT_ORDER-1;i >=0;i--)
	{
		for(index=0;index<MAX_VIDEOCAP_NUM;index++)
		{
			if(videoParam[index].nFormat ==formatOrider[i][0])
			{
				fmax=formatOrider[i][0];
				nFind=true;
				break;
			}
		}
		if(nFind) break;
	}
	if(!nFind) fmax=videoParam[0].nFormat;
}

int CallCapability::GetH263CapOptionParam(int type,VIDEC_CODEC_TYPE codecType,VIDEO_FORMAT_TYPE nFormatType) //1 audio 2 video 3 H239 4 Data
{
	int nRet =-1;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	//if(m_nUseCapOLC==0)
	{
		for(int i=0;i< MAX_VIDEOCAP_NUM;i++)
		{
			if(type==2)
			{
				if(m_videoParam[i].nCodecType == codecType && m_videoParam[i].nFormat == nFormatType)
				{
					nRet=i;
					break;
				}
			}
			else if(type==3)
			{
				if(m_dualParam[i].nCodecType == codecType && m_dualParam[i].nFormat == nFormatType)
				{
					nRet=i;
					break;
				}
			}
		}
	}

#else

	nRet =0;

#endif

	return nRet;
}

void CallCapability::SetMainVideoH263OLCOptionParam(VIDEC_CODEC_TYPE codecType,int nFormat)
{
	m_videoParam[0].nCodecType = codecType;
	m_videoParam[0].nFormat = (VIDEO_FORMAT_TYPE ) nFormat;
}

void CallCapability::SetDualVideoH263OLCOptionParam(VIDEC_CODEC_TYPE codecType,int nFormat)
{
	m_dualParam[0].nCodecType = codecType;
	m_dualParam[0].nFormat = (VIDEO_FORMAT_TYPE ) nFormat;
}

void CallCapability::SetH263OptionParam(int type,VIDEC_CODEC_TYPE codecType,char * szPath,HPVT hPvt,int nNode)
{
#ifdef HAS_H323
	struct tagH263OptionItem
	{
		int nWidth;
		int nHeight;
		VIDEO_FORMAT_TYPE nFormat;
	};
	//CIF 352*288 , 4CIF 704*576  ,16CIF 1408*1152 ,  720I 1280*360, 720P 1280*720,  1080I 1920*540,  
	//1080P 1920*1080, VGA 640*480, SVGA 800*600, XGA 1024*768,  SXGA 1280*1024

	int i=0;
	int error=0;
	struct tagH263OptionItem H263OptionItem[16]={0};
	H263OptionItem[i].nWidth =160;
	H263OptionItem[i].nHeight =120;
	H263OptionItem[i].nFormat =VIDEO_FORMAT_VGA; i++;

	H263OptionItem[i].nWidth =200;
	H263OptionItem[i].nHeight =150;
	H263OptionItem[i].nFormat =VIDEO_FORMAT_SVGA; i++;

	H263OptionItem[i].nWidth =256;
	H263OptionItem[i].nHeight =192;
	H263OptionItem[i].nFormat =VIDEO_FORMAT_XGA; i++;

	H263OptionItem[i].nWidth =320;
	H263OptionItem[i].nHeight =256;
	H263OptionItem[i].nFormat =VIDEO_FORMAT_SXGA; i++;

	H263OptionItem[i].nWidth =320;
	H263OptionItem[i].nHeight =180;
	H263OptionItem[i].nFormat =VIDEO_FORMAT_720P; i++;

	int cnt =0;
	for(int j=0;j<i;j++)
	{
		if( GetH263CapOptionParam(type,codecType,H263OptionItem[j].nFormat) >=0 )
		{
			//sprintf(szPath,"h263Options.customPictureFormat.2.maxCustomPictureWidth");
			//error = pvtBuildByPath(hPvt, nNode, szPath, 200, NULL);

			//sprintf(szPath,"h263Options.customPictureFormat.2.maxCustomPictureHeight");
			//error = pvtBuildByPath(hPvt, nNode, szPath, 150, NULL);

			//sprintf(szPath,"h263Options.customPictureFormat.2.minCustomPictureWidth");
			//error = pvtBuildByPath(hPvt, nNode, szPath, 200, NULL);

			//sprintf(szPath,"h263Options.customPictureFormat.2.minCustomPictureHeight");
			//error = pvtBuildByPath(hPvt, nNode, szPath, 150, NULL);

			//sprintf(szPath,"h263Options.customPictureFormat.2.mPI.standardMPI");
			//error = pvtBuildByPath(hPvt, nNode, szPath, 3, NULL);

			////sprintf(szPath,"h263Options.customPictureFormat.2.pixelAspectInformation.pixelAspectCode.1");
			////error = pvtBuildByPath(hPvt, nNode, szPath,1, NULL);
			//sprintf(szPath,"h263Options.customPictureFormat.2.pixelAspectInformation.anyPixelAspectRatio");
			//error = pvtBuildByPath(hPvt, nNode, szPath, RV_TRUE, NULL);

			////pixelAspectInformation.anyPixelAspectRatio (0)


			cnt ++;
			sprintf(szPath,"h263Options.customPictureFormat.%u.maxCustomPictureWidth",cnt);
			error = pvtBuildByPath(hPvt, nNode, szPath, H263OptionItem[j].nWidth, NULL);

			sprintf(szPath,"h263Options.customPictureFormat.%u.maxCustomPictureHeight",cnt);
			error = pvtBuildByPath(hPvt, nNode, szPath, H263OptionItem[j].nHeight, NULL);

			sprintf(szPath,"h263Options.customPictureFormat.%u.minCustomPictureWidth",cnt);
			error = pvtBuildByPath(hPvt, nNode, szPath, H263OptionItem[j].nWidth, NULL);

			sprintf(szPath,"h263Options.customPictureFormat.%u.minCustomPictureHeight",cnt);
			error = pvtBuildByPath(hPvt, nNode, szPath, H263OptionItem[j].nHeight, NULL);

			sprintf(szPath,"h263Options.customPictureFormat.%u.mPI.standardMPI",cnt);
			error = pvtBuildByPath(hPvt, nNode, szPath, 2/*4*/, NULL);

			//或为设为其下两行
			//sprintf(szPath,"h263Options.customPictureFormat.%u.pixelAspectInformation.pixelAspectCode.1",cnt);
			//error = pvtBuildByPath(hPvt, nNode, szPath,1, NULL);
			sprintf(szPath,"h263Options.customPictureFormat.%u.pixelAspectInformation.anyPixelAspectRatio",cnt);
			error = pvtBuildByPath(hPvt, nNode, szPath, RV_TRUE, NULL);
		}
	}
#endif
}

//CIF 352*288 , 4CIF 704*576  , 16CIF 1408*1152 ,  720I 1280*360, 720P 1280*720,  1080I 1920*540,  
//1080P 1920*1080,  VGA 640*480, SVGA 800*600, XGA 1024*768,  SXGA 1280*1024
void CallCapability::GetVideoMaxFSMBPSFromEntry(int & nMaxMBPS,int & nMaxFS,int type,int format)
{
	nMaxFS = 176 *144 / 256;
	//get format
	//双流对应减20000
	if(type ==3) format= format - 20000;
	switch(format)
	{
	case H264_ENTRY_QCIF:
		nMaxFS = 176 *144 / 256;
		break;
	case H264_ENTRY_CIF:
		nMaxFS = 352 *288 / 256;
		break;
	case H264_ENTRY_4CIF:
		nMaxFS = 704 *576 / 256;
		break;
	case H264_ENTRY_16CIF:
		nMaxFS = 1408 *1152 / 256;
		break;
	case H264_ENTRY_720P:
	case H264_ENTRY_HIG720P:
		nMaxFS = 1280 * 720 / 256;
		break;
	case H264_ENTRY_720I:
		nMaxFS = 1280 * 360 / 256;
		break;
	case H264_ENTRY_1080P:
	case H264_ENTRY_HIG1080P:
		nMaxFS = 1920 * 1080 / 256;
		break;
	case H264_ENTRY_1080I:
		nMaxFS = 1920 * 540 / 256;
		break;
	case H264_ENTRY_VGA:
		nMaxFS = 640 * 480 / 256;
		break;
	case H264_ENTRY_SVGA:
		nMaxFS = 800 * 600 / 256;
		break;
	case H264_ENTRY_XGA:
		nMaxFS = 1024 * 768 / 256;
		break;
	case H264_ENTRY_SXGA:
		nMaxFS = 1280 * 1024 / 256;
		break;
	}

	nMaxMBPS = nMaxFS * 30 / 500;
	nMaxFS = nMaxFS /256;
	if(nMaxFS >0) nMaxFS = nMaxFS +1;//notice

}

//mcu use
void CallCapability::GetH264SVCVideoParam(int type,VIDEC_CODEC_TYPE codecType,int & frame,int & nLevel,int & nBitrate)
{
	if(codecType == VIDEC_CODEC_H264_SVC)
	{
		VIDEO_FORMAT_TYPE nFormat =VIDEO_FORMAT_COUNT;

		for(int i=0;i< MAX_VIDEOCAP_NUM;i++)
		{
			if(type==2)
			{
				if(m_videoParam[i].nCodecType == codecType)
				{
					frame   = m_videoParam[i].frame;
					nFormat  = m_videoParam[i].nFormat;
					nBitrate = m_videoParam[i].bitrate;
				}
			}
			else if(type==3)
			{
				if(m_dualParam[i].nCodecType == codecType)
				{
					frame   =m_dualParam[i].frame;
					nFormat =m_dualParam[i].nFormat;
					nBitrate = m_dualParam[i].bitrate;
				}
			}
		}
		if(nFormat != VIDEO_FORMAT_COUNT)
		{
			switch(nFormat)
			{
			case VIDEO_FORMAT_QCIF:
				nLevel =H264_LEVEL_QCIF; 
				break;
			case VIDEO_FORMAT_CIF:
				nLevel =H264_LEVEL_CIF; 
				break;
			case VIDEO_FORMAT_4CIF:
				nLevel =H264_LEVEL_4CIF; 
				break;
			case VIDEO_FORMAT_16CIF:
				nLevel =H264_LEVEL_16CIF; 
				break;
			case VIDEO_FORMAT_720I:
				nLevel =H264_LEVEL_720I; 
				break;
			case VIDEO_FORMAT_720P:
				nLevel =H264_LEVEL_720P; 
				break;
			case VIDEO_FORMAT_1080I:
				nLevel =H264_LEVEL_1080I; 
				break;
			case VIDEO_FORMAT_1080P:
				nLevel =H264_LEVEL_1080P; 
				break;
			case VIDEO_FORMAT_VGA:
				nLevel =H264_LEVEL_VGA; 
				break;
			case VIDEO_FORMAT_SVGA:
				nLevel =H264_LEVEL_SVGA; 
				break;
			case VIDEO_FORMAT_XGA:
				nLevel =H264_LEVEL_XGA; 
				break;
			case VIDEO_FORMAT_SXGA:
				nLevel =H264_LEVEL_SXGA; 
				break;
			default :
				nLevel =H264_LEVEL_720P; 
				break;
			}
		}
	}
}

void CallCapability::AddMPEG4(VIDEO_FORMAT_TYPE format,int nBitrate,int nFrame)                         //添加MPEG4能力 ,use to call kedacom
{
	VIDEO_CAPABILITY_ITEM  capability;
	memset(&capability,0,sizeof(VIDEO_CAPABILITY_ITEM));
	if(format ==VIDEO_FORMAT_COUNT) format =VIDEO_FORMAT_4CIF;

	capability.VideoCodecType = VIDEC_CODEC_MPEG4;
	capability.nMaxBitrate = nBitrate;	
	capability.n4CIF = format;
	capability.nQCIF = MPEG4_ENTRY;
	//capability.nCIF = ;
	AddVideoCap(capability);  //添加
}

void CallCapability::SetMPEG4First(VIDEO_FORMAT_TYPE format,int nBitrate,int nFrame) 
{
	VIDEO_CAPABILITY_ITEM  capability;
	memset(&capability,0,sizeof(VIDEO_CAPABILITY_ITEM));
	if(format ==VIDEO_FORMAT_COUNT) format =VIDEO_FORMAT_4CIF;

	capability.VideoCodecType = VIDEC_CODEC_MPEG4;
	capability.nMaxBitrate = nBitrate;	
	capability.n4CIF = format;
	capability.nQCIF = MPEG4_ENTRY;
	//capability.nCIF = ;
	SetVideoCap(1,capability); //首选
}

int CallCapability::IsExistMPEG4(VIDEO_FORMAT_TYPE format,int nBitrate,int nFrame) 
{
	int result = 0;
	VIDEO_CAPABILITY_ITEM  capability;
	memset(&capability,0,sizeof(VIDEO_CAPABILITY_ITEM));
	if(format ==VIDEO_FORMAT_COUNT) format =VIDEO_FORMAT_4CIF;
	capability.VideoCodecType = VIDEC_CODEC_MPEG4;
	capability.nMaxBitrate = nBitrate;	
	capability.n4CIF = format;
	capability.nQCIF = MPEG4_ENTRY;
	//capability.nCIF = ;
	VideoCap& capSet=m_VideoCap;
	int capNum=capSet.m_nCapNum;
	for(int i=0;i<capNum;i++)
	{
		if(capSet.m_Caps[i].nQCIF == capability.nQCIF)
		{
			result=capability.nQCIF;break;
		}
	}
	return result;
}

int CallCapability::IsFirstMPEG4(VIDEO_FORMAT_TYPE format,int nBitrate,int nFrame) 
{
	int result = 0;
	VIDEO_CAPABILITY_ITEM  capability;
	memset(&capability,0,sizeof(VIDEO_CAPABILITY_ITEM));
	if(format ==VIDEO_FORMAT_COUNT) format =VIDEO_FORMAT_4CIF;
	capability.VideoCodecType = VIDEC_CODEC_MPEG4;
	capability.nMaxBitrate = nBitrate;	
	capability.n4CIF = format;
	capability.nQCIF = MPEG4_ENTRY;
	//capability.nCIF = ;
	VideoCap& capSet=m_VideoCap;
	if(capSet.m_Caps[0].nQCIF == capability.nQCIF)
	{
		result=capability.nQCIF;
	}
	return result; 
}


//20141030 Add
int CallCapability::BuildConfCapabilitis(int node,ConfInfo & confCap)
{
#ifdef HAS_H323

	char tempbuf[1024]; char subBuf[1024]={0};
	int nodeId=m_NodeId=node;
	HPVT valH = cmGetValTree( H323CallMgr::Instance().m_hApp ); 
	int error, iEntryNumber = 0, iCapIndex=0;
	int subId2;

	//int nSimulNum = 1;
	//int nSimultaneous = 1;
	//int iAlternative = 1;

	int param=0;
	//删除能力集表
	subId2 = pvtGetNodeIdByPath( valH, nodeId, "capabilityTable");
	if(subId2>=0)
	{
		error = pvtDelete(valH,subId2);
	}

	//删除所有的同时能力
	//H235 Media use
	subId2 = pvtGetNodeIdByPath( valH, nodeId, "capabilityDescriptors.1.simultaneousCapabilities.5");
	if(subId2>=0)
	{
		error = pvtDelete(valH,subId2);
	}

	subId2 = pvtGetNodeIdByPath( valH, nodeId, "capabilityDescriptors.1.simultaneousCapabilities.4");
	if(subId2>=0)
	{
		error = pvtDelete(valH,subId2);
	}
	subId2 = pvtGetNodeIdByPath( valH, nodeId, "capabilityDescriptors.1.simultaneousCapabilities.3");
	if(subId2>=0)
	{
		error = pvtDelete(valH,subId2);
	}
	subId2 = pvtGetNodeIdByPath( valH, nodeId, "capabilityDescriptors.1.simultaneousCapabilities.2");
	if(subId2>=0)
	{
		error = pvtDelete(valH,subId2);
	}
	subId2 = pvtGetNodeIdByPath( valH, nodeId, "capabilityDescriptors.1.simultaneousCapabilities.1");
	if(subId2>=0)
	{
		error = pvtDelete(valH,subId2);
	}

	if(m_nUseCapOLC==0)
	{
		sprintf(tempbuf, 
			"multiplexCapability.h2250Capability.mediaPacketizationCapability.h261aVideoPacketization");
		error = pvtBuildByPath(valH, nodeId, tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf, 
			"multiplexCapability.h2250Capability.mediaPacketizationCapability.rtpPayloadType.1.payloadDescriptor.rfc-number");
		error = pvtBuildByPath(valH, nodeId, tempbuf, 2429, NULL);

		sprintf(tempbuf, 
			"multiplexCapability.h2250Capability.mediaPacketizationCapability.rtpPayloadType.2.payloadDescriptor.oid");
		unsigned char pValueOid[10]={0x00,0x08,0x81,0x71,0x00,0x00,0x00,0x00,0};
		error = pvtBuildByPath(valH, nodeId, tempbuf, 8, (char*)pValueOid);
	}

	BuildMediaCapabilitis(node,confCap.m_DAPCallCapability);

#endif

	return 0;
}


int CallCapability::GetAudioCapEntry(X_AUDIO_CODEC_ID CodecType,int nType)
{
	int iEntryNumber =0;
	switch( CodecType)
	{
	case X_AUDIO_CODEC_G711A:			
		iEntryNumber = G711A_ENTRY_64K;
		break;
	case X_AUDIO_CODEC_G711U:
		iEntryNumber = G711U_ENTRY_64K;
		break;
	case X_AUDIO_CODEC_G722_64:
		iEntryNumber = G722_ENTRY_64K;
		break;
	case X_AUDIO_CODEC_G7221_7_24:
		iEntryNumber = G7221_ENTRY_24K;
		break;
	case X_AUDIO_CODEC_G7221_7_32:
		iEntryNumber = G7221_ENTRY_32K;
		break;
	case X_AUDIO_CODEC_G7221_14_24:
		iEntryNumber = G7221C_ENTRY_24K;
		break;
	case X_AUDIO_CODEC_G7221_14_32:
		iEntryNumber = G7221C_ENTRY_32K;
		break;
	case X_AUDIO_CODEC_G7221_14_48:
		iEntryNumber = G7221C_ENTRY_48K;
		break;
	case X_AUDIO_CODEC_G719_32:
		iEntryNumber = G719_ENTRY_32K;
		break;
	case X_AUDIO_CODEC_G719_48:
		iEntryNumber = G719_ENTRY_48K;
		break;
	case X_AUDIO_CODEC_G719_64:
		iEntryNumber = G719_ENTRY_64K;
		break;
	case X_AUDIO_CODEC_GSM: 
		iEntryNumber = GSM_ENTRY;
		break;
	case X_AUDIO_CODEC_G7231_63:
			iEntryNumber = G7231_ENTRY;
			break;
	case X_AUDIO_CODEC_G729:
		{
			if(nType==2) iEntryNumber = G729A_ENTRY;
			if(nType==1) iEntryNumber = G729_ENTRY;
		}
		break; 
	default:
		break;
	}
	return iEntryNumber;

}

int CallCapability::GetVideoCapEntry(VIDEC_CODEC_TYPE CodecType,VIDEO_FORMAT_TYPE format,int nDual)  //nDual 0 is main video ,1 is dual video 
{
	int nEntryBase =0;
	int nEntryNumber =0;
	switch(CodecType)
	{
	case VIDEC_CODEC_H264:
		nEntryBase =H264_ENTRY_BASE;
		break;
	case VIDEC_CODEC_H264_SVC:
		nEntryBase =H264HP_ENTRY_BASE;
		break;
	case VIDEC_CODEC_H263:
		nEntryBase =H263_ENTRY_BASE;
		break;
	case VIDEC_CODEC_H263P:
		nEntryBase =H263P_ENTRY_BASE;
		break;
	case VIDEC_CODEC_H263PP:
		nEntryBase =H263PP_ENTRY_BASE;
		break;
	case VIDEC_CODEC_H261:
		nEntryBase =H261_ENTRY_BASE;
		break;
	}

	if(nDual ==1)
	{
		nEntryBase = nEntryBase + 20000;
	}

	switch(format)
	{
	case VIDEO_FORMAT_QCIF:
		nEntryNumber = nEntryBase + ENTRY_QCIF;
		break;
	case VIDEO_FORMAT_CIF:
		nEntryNumber = nEntryBase + ENTRY_CIF;
		break; 
	case VIDEO_FORMAT_4CIF: 
		nEntryNumber = nEntryBase + ENTRY_4CIF;
		break; 
	case VIDEO_FORMAT_16CIF: 
		nEntryNumber = nEntryBase + ENTRY_16CIF;
		break; 
	case VIDEO_FORMAT_720I: 
		nEntryNumber = nEntryBase + ENTRY_720I;
		break; 
	case VIDEO_FORMAT_720P: 
		nEntryNumber = nEntryBase + ENTRY_720P;
		break; 
	case VIDEO_FORMAT_1080I: 
		nEntryNumber = nEntryBase + ENTRY_1080I;
		break; 
	case VIDEO_FORMAT_1080P: 
		nEntryNumber = nEntryBase + ENTRY_1080P;
		break;
	case VIDEO_FORMAT_VGA: 
		nEntryNumber = nEntryBase + ENTRY_VGA;
		break; 
	case VIDEO_FORMAT_SVGA:
		nEntryNumber = nEntryBase + ENTRY_SVGA;
		break; 
	case VIDEO_FORMAT_XGA: 
		nEntryNumber = nEntryBase + ENTRY_XGA;
		break; 
	case VIDEO_FORMAT_SXGA: 
		nEntryNumber = nEntryBase + ENTRY_SXGA;
		break;
	default:
		break;
	}

	//notice only use ,use to call kedacom
	if(CodecType ==VIDEC_CODEC_MPEG4 )
	{
		nEntryNumber = MPEG4_ENTRY;
		if(nDual ==1)
		{
			nEntryNumber = nEntryNumber + 20000;
		}
	}

	return nEntryNumber;
}

void CallCapability::GetVideoH264MaxFSMBPS(int & nMaxMBPS,int & nMaxFS,int & nLevel,int nFrame,VIDEO_FORMAT_TYPE format)
{
	GetVideoH264MaxFS(nMaxFS,format);
	nLevel = GetVideoH264Level(format);
	float fFrameRate=nFrame;
	nMaxMBPS = nMaxFS * fFrameRate / 500;
	nMaxFS = nMaxFS /256;
	nMaxFS = nMaxFS +1;//notice
	if(nMaxFS <=0) nMaxFS =1;
}

int CallCapability::GetVideoH264Level(VIDEO_FORMAT_TYPE nFormat)
{
	int nLevel =H264_LEVEL_QCIF;

	switch(nFormat)
	{
	case VIDEO_FORMAT_QCIF:
		nLevel =H264_LEVEL_QCIF; 
		break;
	case VIDEO_FORMAT_CIF:
		nLevel =H264_LEVEL_CIF; 
		break;
	case VIDEO_FORMAT_4CIF:
		nLevel =H264_LEVEL_4CIF; 
		break;
	case VIDEO_FORMAT_16CIF:
		nLevel =H264_LEVEL_16CIF; 
		break;
	case VIDEO_FORMAT_720I:
		nLevel =H264_LEVEL_720I; 
		break;
	case VIDEO_FORMAT_720P:
		nLevel =H264_LEVEL_720P; 
		break;
	case VIDEO_FORMAT_1080I:
		nLevel =H264_LEVEL_1080I; 
		break;
	case VIDEO_FORMAT_1080P:
		nLevel =H264_LEVEL_1080P; 
		break;
	case VIDEO_FORMAT_VGA:
		nLevel =H264_LEVEL_VGA; 
		break;
	case VIDEO_FORMAT_SVGA:
		nLevel =H264_LEVEL_SVGA; 
		break;
	case VIDEO_FORMAT_XGA:
		nLevel =H264_LEVEL_XGA; 
		break;
	case VIDEO_FORMAT_SXGA:
		nLevel =H264_LEVEL_SXGA; 
		break;
	default :
		nLevel =H264_LEVEL_QCIF; 
		break;
	}
	return nLevel;

}

void CallCapability::SetH263OptionParam(char * tempbuf,HPVT valH,int nodeId,VIDEO_FORMAT_TYPE nH263Format)
{
#ifdef HAS_H323
	//CIF 352*288 , 4CIF 704*576  ,16CIF 1408*1152 ,  720I 1280*360, 720P 1280*720,  1080I 1920*540,  
	//1080P 1920*1080, VGA 640*480, SVGA 800*600, XGA 1024*768,  SXGA 1280*1024
	struct tagH263OptionItem
	{
		int nWidth;
		int nHeight;
		VIDEO_FORMAT_TYPE nFormat;
		int nStandMPI; // 30/(1.001 * frame rate)
	};

	int i=0;
	int j=0;
	int nCount=0;
	int error=0;
	//struct tagH263OptionItem H263OptionItem[]={ {160,120,VIDEO_FORMAT_VGA}, {200,150,VIDEO_FORMAT_SVGA}, {256,192,VIDEO_FORMAT_XGA}, 
	//{320,180,VIDEO_FORMAT_SXGA}, {320,256,VIDEO_FORMAT_720P}, {480,270,VIDEO_FORMAT_1080P}, {352,288,VIDEO_FORMAT_16CIF}};
	struct tagH263OptionItem H263OptionItem[]={ {160,120,VIDEO_FORMAT_VGA,2}, {200,150,VIDEO_FORMAT_SVGA,3}, {256,192,VIDEO_FORMAT_XGA,4}, 
	{320,180,VIDEO_FORMAT_SXGA,4}, {320,256,VIDEO_FORMAT_720P,4}, {480,270,VIDEO_FORMAT_1080P,6}, {352,288,VIDEO_FORMAT_16CIF,6}};

	nCount=sizeof(H263OptionItem)/sizeof(tagH263OptionItem);

	for(j=0;j<nCount;j++)
	{
		if(H263OptionItem[j].nFormat == nH263Format)
		{
			i =j+1;
			break;
		}
	}

	if(nodeId >=0 )
	{
		//h263Options
		sprintf(tempbuf,"h263Options.advancedIntraCodingMode");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.deblockingFilterMode");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.improvedPBFramesMode");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.unlimitedMotionVectors");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
		sprintf(tempbuf,"h263Options.fullPictureFreeze");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.partialPictureFreezeAndRelease");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


		sprintf(tempbuf,"h263Options.resizingPartPicFreezeAndRelease");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.fullPictureSnapshot");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.partialPictureSnapshot");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.videoSegmentTagging");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
		sprintf(tempbuf,"h263Options.progressiveRefinement");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.dynamicPictureResizingByFour");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


		sprintf(tempbuf,"h263Options.dynamicPictureResizingSixteenthPel");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.dynamicWarpingHalfPel");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.dynamicWarpingSixteenthPel");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.independentSegmentDecoding");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
		sprintf(tempbuf,"h263Options.slicesInOrder-NonRect");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.slicesInOrder-Rect");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


		sprintf(tempbuf,"h263Options.slicesNoOrder-NonRect");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.slicesNoOrder-Rect");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.alternateInterVLCMode");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.modifiedQuantizationMode");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.reducedResolutionUpdate");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

		sprintf(tempbuf,"h263Options.separateVideoBackChannel");
		error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
		//add customPictureFormat
		int cnt =0;
		for(j=0;j<i;j++)
		{
			//sprintf(szPath,"h263Options.customPictureFormat.2.maxCustomPictureWidth");
			//error = pvtBuildByPath(valH, nodeId, tempbuf, 200, NULL);

			//sprintf(tempbuf,"h263Options.customPictureFormat.2.maxCustomPictureHeight");
			//error = pvtBuildByPath(valH, nodeId, tempbuf, 150, NULL);

			//sprintf(tempbuf,"h263Options.customPictureFormat.2.minCustomPictureWidth");
			//error = pvtBuildByPath(valH, nodeId, tempbuf, 200, NULL);

			//sprintf(tempbuf,"h263Options.customPictureFormat.2.minCustomPictureHeight");
			//error = pvtBuildByPath(valH, nodeId, tempbuf, 150, NULL);

			//sprintf(tempbuf,"h263Options.customPictureFormat.2.mPI.standardMPI");
			//error = pvtBuildByPath(valH, nodeId, tempbuf, 3, NULL);

			////sprintf(tempbuf,"h263Options.customPictureFormat.2.pixelAspectInformation.pixelAspectCode.1");
			////error = pvtBuildByPath(valH, nodeId, tempbuf,1, NULL);
			//sprintf(tempbuf,"h263Options.customPictureFormat.2.pixelAspectInformation.anyPixelAspectRatio");
			//error = pvtBuildByPath(valH, nodeId, tempbuf, RV_TRUE, NULL);

			////pixelAspectInformation.anyPixelAspectRatio (0)


			cnt ++;
			sprintf(tempbuf,"h263Options.customPictureFormat.%u.maxCustomPictureWidth",cnt);
			error = pvtBuildByPath(valH, nodeId, tempbuf, H263OptionItem[j].nWidth, NULL);

			sprintf(tempbuf,"h263Options.customPictureFormat.%u.maxCustomPictureHeight",cnt);
			error = pvtBuildByPath(valH, nodeId, tempbuf, H263OptionItem[j].nHeight, NULL);

			sprintf(tempbuf,"h263Options.customPictureFormat.%u.minCustomPictureWidth",cnt);
			error = pvtBuildByPath(valH, nodeId, tempbuf, H263OptionItem[j].nWidth, NULL);

			sprintf(tempbuf,"h263Options.customPictureFormat.%u.minCustomPictureHeight",cnt);
			error = pvtBuildByPath(valH, nodeId, tempbuf, H263OptionItem[j].nHeight, NULL);

			sprintf(tempbuf,"h263Options.customPictureFormat.%u.mPI.standardMPI",cnt);
			error = pvtBuildByPath(valH, nodeId, tempbuf, H263OptionItem[j].nStandMPI/*4*/, NULL);

			//或为设为其下两行
			//sprintf(tempbuf,"h263Options.customPictureFormat.%u.pixelAspectInformation.pixelAspectCode.1",cnt);
			//error = pvtBuildByPath(valH, nodeId, tempbuf,1, NULL);
			sprintf(tempbuf,"h263Options.customPictureFormat.%u.pixelAspectInformation.anyPixelAspectRatio",cnt);
			error = pvtBuildByPath(valH, nodeId, tempbuf, RV_TRUE, NULL);
		}

	}


#endif

}

void CallCapability::CopyToOriginalCaps(DapCallCapability & DapCap)
{
	//copy to original save , encrytion capability need
	int i=0;
	m_AudioCap.m_nCapNum =DapCap.m_nAudioCapCount;
	for(i=0; i<DapCap.m_nAudioCapCount; i++)
	{
		m_AudioCap.m_Caps[i].AudioCodecID =DapCap.m_aAudioCap[i].m_AudioCodecType;
	}
	m_VideoCap.m_nCapNum =DapCap.m_nMainVideoCapCount;
	for(i=0; i<DapCap.m_nMainVideoCapCount; i++)
	{
		m_VideoCap.m_Caps[i].VideoCodecType =DapCap.m_aMainVideoCap[i].m_VideoCodecType;
		m_VideoCap.m_Caps[i].VideoFormatType =DapCap.m_aMainVideoCap[i].m_VideoFormatType;
	}
	m_H239VideoCap.m_nCapNum =DapCap.m_nSecondVideoCapCount;
	for(i=0; i<DapCap.m_nSecondVideoCapCount; i++)
	{
		m_H239VideoCap.m_Caps[i].VideoCodecType =DapCap.m_aSecondVideoCap[i].m_VideoCodecType;
		m_H239VideoCap.m_Caps[i].VideoFormatType =DapCap.m_aSecondVideoCap[i].m_VideoFormatType;
	}
	m_DataCap.m_nCapNum =DapCap.m_nDataCapCount;
	for(i=0; i<DapCap.m_nDataCapCount; i++)
	{
		m_DataCap.m_Caps[i].DataCodecID =(DATA_CODEC_ID) DapCap.m_DataCap[i].m_DataCodecID;
	}
}

int CallCapability::BuildMediaCapabilitis(int node,DapCallCapability & DapCap)
{
#ifdef HAS_H323

	//copy to original save , encrytion capability need
	CopyToOriginalCaps(DapCap);

	char tempbuf[1024];
	char subBuf[1024]={0};
	int nodeId=m_NodeId=node;
	HPVT valH = cmGetValTree( H323CallMgr::Instance().m_hApp ); 
	int error, iEntryNumber = 0, iCapIndex=0;
	//int subId2;
	int nSimulNum = 1;
	int nSimultaneous = 1;
	int iAlternative = 1;

	int param=0;
	unsigned int i;
	//设置音频同时能力集
	iAlternative = 1;

	for(i=0; i<DapCap.m_nAudioCapCount; i++)
	{
		int nDirection =DapCap.m_aAudioCap[i].m_nDirection;
		X_AUDIO_CODEC_ID CodecID =DapCap.m_aAudioCap[i].m_AudioCodecType;
		iEntryNumber = GetAudioCapEntry(CodecID,m_nG729Type);

		switch( CodecID)
		{
		case X_AUDIO_CODEC_G711A:
			{
				//iEntryNumber = G711A_ENTRY_64K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.g711Alaw64k", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 20, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break; 
		case X_AUDIO_CODEC_G711U:
			{
				//iEntryNumber = G711U_ENTRY_64K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.g711Ulaw64k", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 20, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case X_AUDIO_CODEC_G722_64:
			{
				//iEntryNumber = G722_ENTRY_64K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.g722-64k", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 30, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case X_AUDIO_CODEC_G7221_7_24:
			{
				//iEntryNumber = G7221_ENTRY_24K;
				iCapIndex ++;						
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[6]={0x00,0x07,0xB8,0x35,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 24000, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case X_AUDIO_CODEC_G7221_7_32:
			{
				//iEntryNumber = G7221_ENTRY_32K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[6]={0x00,0x07,0xB8,0x35,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 32000, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

		case X_AUDIO_CODEC_G7221_14_24:
			{
				//iEntryNumber = G7221C_ENTRY_24K;
				iCapIndex ++;						
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[7]={0x00,0x07,0xB8,0x35,0x01,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 240, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterValue.booleanArray", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 64, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case X_AUDIO_CODEC_G7221_14_32:
			{
				//iEntryNumber = G7221C_ENTRY_32K;
				iCapIndex ++;						
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[7]={0x00,0x07,0xB8,0x35,0x01,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 320, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterValue.booleanArray", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 32, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

		case X_AUDIO_CODEC_G7221_14_48:
			{
				//iEntryNumber = G7221C_ENTRY_48K;
				iCapIndex ++;						
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[7]={0x00,0x07,0xB8,0x35,0x01,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 480, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterValue.booleanArray", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 16, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

			//notice 20111227 update
		case X_AUDIO_CODEC_G719_32:
			{
				//iEntryNumber = G719_ENTRY_32K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[6]={0x00,0x07,0x85,0x4F,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 320, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.3.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 3, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.3.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case X_AUDIO_CODEC_G719_48:
			{
				//iEntryNumber = G719_ENTRY_48K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[6]={0x00,0x07,0x85,0x4F,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 480, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.3.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 3, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.3.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case X_AUDIO_CODEC_G719_64:
			{
				//iEntryNumber = G719_ENTRY_64K;
				iCapIndex ++;
				AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.genericAudioCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[6]={0x00,0x07,0x85,0x4F,0x01,0x00};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)pValue);

				sprintf(tempbuf, "%s.genericAudioCapability.maxBitRate", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 640, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.1.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 2, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.2.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 4, NULL);

				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.3.parameterIdentifier.standard", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 3, NULL);
				sprintf(tempbuf, "%s.genericAudioCapability.collapsing.3.parameterValue.unsignedMin", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;


		case X_AUDIO_CODEC_G7231_63:
			//{
			//	//iEntryNumber = G7231_ENTRY;
			//	iCapIndex ++;
			//	AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);
			//	sprintf(tempbuf, "%s.g7231.maxAl-sduAudioFrames",subBuf);
			//	error = pvtBuildByPath(valH, nodeId, tempbuf, 8, NULL);
			//	sprintf(tempbuf, "%s.g7231.silenceSuppression",subBuf);
			//	error = pvtBuildByPath(valH, nodeId, tempbuf, RV_FALSE, NULL);
			//	//设置同时能力集
			//	DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
			//}
			break;
		case X_AUDIO_CODEC_G729:
			{
				if( (m_nUseCapOLC==0) || (m_nUseCapOLC==1 && m_nG729Type==2) )
				{
					if(m_nUseCapOLC==0) iEntryNumber = GetAudioCapEntry(CodecID,2);
					else iEntryNumber = GetAudioCapEntry(CodecID,m_nG729Type);
					//iEntryNumber = G729A_ENTRY;
					iCapIndex ++;
					AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);
					sprintf(tempbuf, "%s.g729AnnexA", subBuf);
					error = pvtBuildByPath(valH, nodeId, tempbuf, 20, NULL);
					//设置同时能力集
					DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
				}
				if( (m_nUseCapOLC==0) || (m_nUseCapOLC==1 && m_nG729Type==1) )
				{
					if(m_nUseCapOLC==0) iEntryNumber = GetAudioCapEntry(CodecID,1);
					else iEntryNumber = GetAudioCapEntry(CodecID,m_nG729Type);
					//iEntryNumber = G729_ENTRY;
					iCapIndex ++;
					AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);
					sprintf(tempbuf, "%s.g729", subBuf);
					error = pvtBuildByPath(valH, nodeId, tempbuf, 30, NULL);
					//设置同时能力集
					DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
				}
			}
			break;
		default:
			break;
		}

	}

	//add tai er ru wang test use
#if defined(TAIERLAB) && (TAIERLAB == 1)

	if(m_nUseCapOLC==0)
	{
		{
			iEntryNumber = G7231_ENTRY;
			iCapIndex ++;
			AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);
			sprintf(tempbuf, "%s.g7231.maxAl-sduAudioFrames",subBuf);
			error = pvtBuildByPath(valH, nodeId, tempbuf, 8, NULL);
			sprintf(tempbuf, "%s.g7231.silenceSuppression",subBuf);
			error = pvtBuildByPath(valH, nodeId, tempbuf, RV_FALSE, NULL);
			//设置同时能力集
			DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
		}
		{
			iEntryNumber = G728_ENTRY;
			iCapIndex ++;
			AudioEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);
			sprintf(tempbuf, "%s.g728",subBuf);
			error = pvtBuildByPath(valH, nodeId, tempbuf, 30, NULL);
			//设置同时能力集
			DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
		}
	}

#endif

	if(iAlternative>1)
	{
		nSimultaneous++;
	}



	//设置视频同时能力集
	iAlternative = 1;
	for(i=0; i <DapCap.m_nMainVideoCapCount; i++)
	{
		int n4CIFValue =2; //H3C nHas4CIF 2 --> 1
		int nHasQCIF = 0;
		int nHasCIF = 0;
		int nHas4CIF = 0;
		int nVideoRate = 0;
		int nMaxMBPS =0;
		int nMaxFS =0;
		int nLevel =0;
		int nDual =0;
		int nVideoFrameRate =DapCap.m_aMainVideoCap[i].m_nVideoFrameRate;
		VIDEC_CODEC_TYPE	VideoCodecType =DapCap.m_aMainVideoCap[i].m_VideoCodecType;
		VIDEO_FORMAT_TYPE	VideoFormatType =DapCap.m_aMainVideoCap[i].m_VideoFormatType;

		int nDirection =DapCap.m_aMainVideoCap[i].m_nDirection;
		int nFormatValue = DapCap.m_aMainVideoCap[i].m_nFormatValue;
		nVideoRate =DapCap.m_aMainVideoCap[i].m_nVideoBitrate;
		iEntryNumber = GetVideoCapEntry(VideoCodecType,VideoFormatType,nDual);
		int n4cifFormatValue = GetVideoFormatValue(VIDEO_FORMAT_4CIF);

		switch(VideoCodecType )
		{
		case VIDEC_CODEC_H261:
			{
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(nHasCIF >0) nHasQCIF=1;
				//修改H261能力参数
				iCapIndex ++;
				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);
				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.h261VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.h261VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				sprintf(tempbuf,"%s.h261VideoCapability.temporalSpatialTradeOffCapability",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.h261VideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					if(nVideoRate >1920 )
						error = pvtBuildByPath(valH, nodeId,tempbuf,1920 *10, NULL);
					else
						error = pvtBuildByPath(valH, nodeId,tempbuf,nVideoRate*10, NULL);
				}

				sprintf(tempbuf,"%s.h261VideoCapability.stillImageTransmission",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
			}
			break;

		case VIDEC_CODEC_H263:
			{
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_4CIF) nHas4CIF =n4CIFValue;
				if(nHas4CIF >0) nHasCIF=1;
				if(nHasCIF >0) nHasQCIF=1;
				//修改H263能力参数
				iCapIndex ++;

				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				if (nHas4CIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.cif4MPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHas4CIF, NULL);
				}


				sprintf(tempbuf,"%s.h263VideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}


				sprintf(tempbuf,"%s.h263VideoCapability.unrestrictedVector",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.h263VideoCapability.arithmeticCoding",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.h263VideoCapability.advancedPrediction",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.h263VideoCapability.pbFrames",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.h263VideoCapability.temporalSpatialTradeOffCapability",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.h263VideoCapability.errorCompensation",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		case VIDEC_CODEC_H263P:
			{
				if(nFormatValue >n4cifFormatValue) nHas4CIF = n4CIFValue; //VIDEO_FORMAT_4CIF 3
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_4CIF) nHas4CIF =n4CIFValue;
				if(nHas4CIF >0) nHasCIF=1;
				if(nHasCIF >0) nHasQCIF=1;

				iCapIndex ++;
				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				if (nHas4CIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.cif4MPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHas4CIF, NULL);
				}


				sprintf(tempbuf,"%s.h263VideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				//注意此
				int tempNodeId=nodeId;
				//注意此
				sprintf(tempbuf,"%s.h263VideoCapability",subBuf);
				nodeId = pvtBuildByPath(valH, nodeId,tempbuf, NULL, NULL);

				if(nodeId >=0 )
				{
					sprintf(tempbuf,"unrestrictedVector");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"arithmeticCoding");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"advancedPrediction");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"pbFrames");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"temporalSpatialTradeOffCapability");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"errorCompensation");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//h263Options
					SetH263OptionParam(tempbuf,valH,nodeId,VideoFormatType);
				}

				//	//h263Options

				//	sprintf(tempbuf,"h263Options.advancedIntraCodingMode");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.deblockingFilterMode");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.improvedPBFramesMode");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.unlimitedMotionVectors");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
				//	sprintf(tempbuf,"h263Options.fullPictureFreeze");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.partialPictureFreezeAndRelease");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


				//	sprintf(tempbuf,"h263Options.resizingPartPicFreezeAndRelease");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.fullPictureSnapshot");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.partialPictureSnapshot");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.videoSegmentTagging");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
				//	sprintf(tempbuf,"h263Options.progressiveRefinement");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.dynamicPictureResizingByFour");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


				//	sprintf(tempbuf,"h263Options.dynamicPictureResizingSixteenthPel");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.dynamicWarpingHalfPel");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.dynamicWarpingSixteenthPel");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.independentSegmentDecoding");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
				//	sprintf(tempbuf,"h263Options.slicesInOrder-NonRect");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.slicesInOrder-Rect");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


				//	sprintf(tempbuf,"h263Options.slicesNoOrder-NonRect");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.slicesNoOrder-Rect");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.alternateInterVLCMode");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.modifiedQuantizationMode");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.reducedResolutionUpdate");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				//	sprintf(tempbuf,"h263Options.separateVideoBackChannel");
				//	error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
				//	//add customPictureFormat
				//	SetH263OptionParam(2,capVideo.VideoCodecType,tempbuf,valH,nodeId);

				//}


				//注意此
				nodeId=tempNodeId;

				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
			}
			break;

		case VIDEC_CODEC_H263PP:
			{
				if(nFormatValue >n4cifFormatValue) nHas4CIF = n4CIFValue; //VIDEO_FORMAT_4CIF 3
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_4CIF) nHas4CIF =n4CIFValue;
				if(nHas4CIF >0) nHasCIF=1;
				if(nHasCIF >0) nHasQCIF=1;

				iCapIndex ++;

				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				if (nHas4CIF)
				{
					sprintf(tempbuf,"%s.h263VideoCapability.cif4MPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHas4CIF, NULL);
				}


				sprintf(tempbuf,"%s.h263VideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}
				//注意此
				int tempNodeId=nodeId;
				//注意此
				sprintf(tempbuf,"%s.h263VideoCapability",subBuf);
				nodeId = pvtBuildByPath(valH, nodeId,tempbuf, NULL, NULL);
				if(nodeId >=0 )
				{
					sprintf(tempbuf,"unrestrictedVector");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"arithmeticCoding");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"advancedPrediction");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"pbFrames");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"temporalSpatialTradeOffCapability");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"errorCompensation");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//h263Options
					SetH263OptionParam(tempbuf,valH,nodeId,VideoFormatType);


					//sprintf(tempbuf,"h263Options.advancedIntraCodingMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.deblockingFilterMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.improvedPBFramesMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.unlimitedMotionVectors");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					//sprintf(tempbuf,"h263Options.fullPictureFreeze");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.partialPictureFreezeAndRelease");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					//sprintf(tempbuf,"h263Options.resizingPartPicFreezeAndRelease");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.fullPictureSnapshot");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.partialPictureSnapshot");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.videoSegmentTagging");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					//sprintf(tempbuf,"h263Options.progressiveRefinement");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.dynamicPictureResizingByFour");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					//sprintf(tempbuf,"h263Options.dynamicPictureResizingSixteenthPel");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.dynamicWarpingHalfPel");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.dynamicWarpingSixteenthPel");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.independentSegmentDecoding");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					//sprintf(tempbuf,"h263Options.slicesInOrder-NonRect");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.slicesInOrder-Rect");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					//sprintf(tempbuf,"h263Options.slicesNoOrder-NonRect");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.slicesNoOrder-Rect");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.alternateInterVLCMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.modifiedQuantizationMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.reducedResolutionUpdate");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.separateVideoBackChannel");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.videoBadMBsCap");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					////add customPictureFormat //notice call MG6050-R5242 not add , 20140827
					////SetH263OptionParam(2,capVideo.VideoCodecType,tempbuf,valH,nodeId);

					//h263Version3Options
					{
						sprintf(tempbuf,"h263Options.h263Version3Options.dataPartitionedSlices");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.fixedPointIDCT0");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.interlacedFields");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.currentPictureHeaderRepetition");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.previousPictureHeaderRepetition");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.nextPictureHeaderRepetition");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.pictureNumber");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.spareReferencePictures");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					}


				}


				//注意此
				nodeId=tempNodeId;

				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);


			}
			break;


		case VIDEC_CODEC_H264:
			{
				iCapIndex ++;

				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.genericVideoCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[10]={0x00,0x08,0x81,0x71,0x00,0x00,0x01,0};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf,"%s.genericVideoCapability.collapsing.1.parameterIdentifier.standard",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 41, NULL);	


				sprintf(tempbuf,"%s.genericVideoCapability.collapsing.1.parameterValue.booleanArray",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 64, NULL);	

				//sprintf(tempbuf,"%s.genericVideoCapability.collapsing.2.parameterIdentifier.standard",
				//	subBuf);
				//error = pvtBuildByPath(valH, nodeId,tempbuf, 42, NULL);	


				//sprintf(tempbuf,"%s.genericVideoCapability.collapsing.2.parameterValue.unsignedMin",
				//	subBuf);
				//error = pvtBuildByPath(valH, nodeId,tempbuf, nLevel, NULL);	

				char tempStr[512]={0};
				int nCount=0;
				sprintf(tempStr,"%s.genericVideoCapability.collapsing",subBuf);
				//notice set standard value
				GetVideoH264MaxFSMBPS(nMaxMBPS,nMaxFS,nLevel,nVideoFrameRate,VideoFormatType);
				tagCollapsing Collapsing[]={ {42,nLevel},{3,nMaxMBPS},{4,nMaxFS}};
				nCount=sizeof(Collapsing)/sizeof(tagCollapsing);

				for(int j=0;j<nCount;j++)
				{
					sprintf(tempbuf,"%s.%u.parameterIdentifier.standard",tempStr,j+2);
					error = pvtBuildByPath(valH, nodeId, tempbuf, Collapsing[j].standard, NULL);
					sprintf(tempbuf,"%s.%u.parameterValue.unsignedMin",tempStr,j+2);
					error = pvtBuildByPath(valH, nodeId, tempbuf, Collapsing[j].value, NULL);
				}

				//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
				//
				//				if(m_nUseCapOLC==0) nVideoRate=nBitrateGet;
				//#endif

				sprintf(tempbuf,"%s.genericVideoCapability.maxBitRate",subBuf);
				//if(nVideoRate <=0) nVideoRate=768;
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
			}
			break;

		case VIDEC_CODEC_H264_SVC:
			{
				iCapIndex ++;

				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.genericVideoCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[10]={0x00,0x08,0x81,0x71,0x00,0x00,0x01,0};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf,"%s.genericVideoCapability.collapsing.1.parameterIdentifier.standard",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 41, NULL);	


				sprintf(tempbuf,"%s.genericVideoCapability.collapsing.1.parameterValue.booleanArray",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 8, NULL);	

				//sprintf(tempbuf,"%s.genericVideoCapability.collapsing.2.parameterIdentifier.standard",
				//	subBuf);
				//error = pvtBuildByPath(valH, nodeId,tempbuf, 42, NULL);	

				//sprintf(tempbuf,"%s.genericVideoCapability.collapsing.2.parameterValue.unsignedMin",
				//	subBuf);
				//error = pvtBuildByPath(valH, nodeId,tempbuf, nLevel, NULL);	

				//tagCollapsing Collapsing[]={ {6,80} };

				char tempStr[512]={0};
				int nCount=0;
				sprintf(tempStr,"%s.genericVideoCapability.collapsing",subBuf);
				//notice set standard value
				GetVideoH264MaxFSMBPS(nMaxMBPS,nMaxFS,nLevel,nVideoFrameRate,VideoFormatType);
				tagCollapsing Collapsing[]={ {42,nLevel},{3,nMaxMBPS},{4,nMaxFS}};
				nCount=sizeof(Collapsing)/sizeof(tagCollapsing);

				for(int j=0;j<nCount;j++)
				{
					sprintf(tempbuf,"%s.%u.parameterIdentifier.standard",tempStr,j+2);
					error = pvtBuildByPath(valH, nodeId, tempbuf, Collapsing[j].standard, NULL);
					sprintf(tempbuf,"%s.%u.parameterValue.unsignedMin",tempStr,j+2);
					error = pvtBuildByPath(valH, nodeId, tempbuf, Collapsing[j].value, NULL);
				}

				//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
				//
				//				if(m_nUseCapOLC==0) nVideoRate=nBitrateGet;
				//#endif

				sprintf(tempbuf,"%s.genericVideoCapability.maxBitRate",
					subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;


		case VIDEC_CODEC_MPEG4: //notice only use ,use to call kedacom
			{
				//iEntryNumber = MPEG4_ENTRY;
				iCapIndex ++;
				// 7>       nonStandard = 0
				// 8>        nonStandardIdentifier = 0
				// 9>         h221NonStandard = 0
				//10>          t35CountryCode = 38
				//10>          t35Extension = 0
				//10>          manufacturerCode = 8888
				// 8>        data = 32 '....kedacom_js_China............' 
				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.nonStandard.nonStandardIdentifier.h221NonStandard.t35CountryCode", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 38,NULL);

				sprintf(tempbuf, "%s.nonStandard.nonStandardIdentifier.h221NonStandard.t35Extension", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 0,NULL);

				sprintf(tempbuf, "%s.nonStandard.nonStandardIdentifier.h221NonStandard.manufacturerCode", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 8888,NULL);

				sprintf(tempbuf, "%s.nonStandard.data", subBuf);
				//....kedacom_js_China............
				if(m_nUseCapOLC==0)
				{
					unsigned char pValue[32]=
					{ 0x00 ,0x00 ,0x00 ,0x11 ,0x6b ,
					0x65 ,0x64 ,0x61 ,0x63 ,0x6f ,0x6d ,0x5f ,0x6a ,
					0x73 ,0x5f ,0x43 ,0x68 ,0x69 ,0x6e ,0x61 ,0x00 ,
					0x00 ,0x03 ,0x00 ,0x00 ,0x01 ,0x01 ,0x00 ,0x01 ,
					0x01 ,0x01 ,0x00 };
					error = pvtBuildByPath(valH, nodeId, tempbuf, 32, (char*)pValue);
					DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
				}
				else
				{
					unsigned char pValue[32]=
					{ 0x00 ,0x00 ,0x00 ,0x11 ,0x6b ,
					0x65 ,0x64 ,0x61 ,0x63 ,0x6f ,0x6d ,0x5f ,0x6a ,
					0x73 ,0x5f ,0x43 ,0x68 ,0x69 ,0x6e ,0x61 ,0x00 ,
					0x00 ,0x01 ,0x40 ,0x00 ,0x00 ,0x00 ,0x00 ,0x01 ,
					0x00 ,0x00 ,0x00 };
					error = pvtBuildByPath(valH, nodeId, tempbuf, 32, (char*)pValue);
					DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
				}
			}
			break;

		default:
			break;
		}

	}

	//add by Jan for fake MPEG4 [20130903] //add tai er ru wang test use
#if defined(TAIERLAB) && (TAIERLAB == 1)

	if(m_nUseCapOLC==0)
	{
		iEntryNumber = MPE4_ENTRY;
		iCapIndex ++;
		sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);
		char OID[16] = {0};
		int length;
		length=utlEncodeOID(sizeof(OID),OID,"itu-t recommendation h 245 1 0 0");
		sprintf(tempbuf, "capabilityTable.%d.capability.receiveVideoCapability.genericVideoCapability.capabilityIdentifier.standard", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, length, OID);
		sprintf(tempbuf,
			"capabilityTable.%u.capability.receiveVideoCapability.genericVideoCapability.maxBitRate",
			iCapIndex);
		error = pvtBuildByPath(valH, nodeId,tempbuf, 384*10, NULL);

		sprintf(tempbuf,
			"capabilityTable.%u.capability.receiveVideoCapability.genericVideoCapability.nonCollapsing.1.parameterIdentifier.standard",
			iCapIndex);
		error = pvtBuildByPath(valH, nodeId,tempbuf, 0, NULL);

		sprintf(tempbuf,
			"capabilityTable.%u.capability.receiveVideoCapability.genericVideoCapability.nonCollapsing.1.parameterValue.unsignedMax",
			iCapIndex);
		error = pvtBuildByPath(valH, nodeId,tempbuf, 3, NULL);//默认是3   14496-2中定义的simple/L2 是cif值为3

		DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

	}

#endif

	//end [20130903]

	if(iAlternative>1)
	{
		nSimultaneous++;
	}


	//设置双流同时能力
	iAlternative = 1;
	bool bHasH239Cap = false;

	for(i=0; i < DapCap.m_nSecondVideoCapCount; i++)
	{
		switch(DapCap.m_aSecondVideoCap[i].m_VideoCodecType)
		{
		case VIDEC_CODEC_H261:
		case VIDEC_CODEC_H263:
		case VIDEC_CODEC_H263P:
		case VIDEC_CODEC_H263PP:
		case VIDEC_CODEC_H264:
		case VIDEC_CODEC_H264_SVC:
			bHasH239Cap = true;
			break;
		default:
			break;
		}
		if(bHasH239Cap)
			break;
	}

	if(bHasH239Cap && m_nUseCapOLC==0)
	{
		//===重新构建H239控制能力
		iEntryNumber = 239;
		iCapIndex ++;			
		sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);

		unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x01,0};
		sprintf(tempbuf,"capabilityTable.%d.capability.genericControlCapability.capabilityIdentifier.standard",iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

	}

	//设置双流能力集参数
	for(i=0; i< DapCap.m_nSecondVideoCapCount; i++)
	{
		int n4CIFValue =2; //H3C nHas4CIF 2 --> 1
		int nHasQCIF = 0;
		int nHasCIF = 0;
		int nHas4CIF = 0;
		int nVideoRate = 0;
		int nMaxMBPS =0;
		int nMaxFS =0;
		int nLevel =0;
		int nDual =1;
		int nVideoFrameRate =DapCap.m_aSecondVideoCap[i].m_nVideoFrameRate;
		VIDEC_CODEC_TYPE	VideoCodecType =DapCap.m_aSecondVideoCap[i].m_VideoCodecType;
		VIDEO_FORMAT_TYPE	VideoFormatType =DapCap.m_aSecondVideoCap[i].m_VideoFormatType;

		int nDirection =DapCap.m_aSecondVideoCap[i].m_nDirection;
		int nFormatValue = DapCap.m_aSecondVideoCap[i].m_nFormatValue;
		nVideoRate =DapCap.m_aSecondVideoCap[i].m_nVideoBitrate;
		iEntryNumber = GetVideoCapEntry(VideoCodecType,VideoFormatType,nDual);

		//修改双流能力参数
		switch( VideoCodecType)
		{
		case VIDEC_CODEC_H261:
			{
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(nHasCIF >0) nHasQCIF=1;
				//修改H261Extend能力参数
				iCapIndex ++;
				DualVideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h261VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h261VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}


				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h261VideoCapability.temporalSpatialTradeOffCapability",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h261VideoCapability.maxBitRate",
					subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}


				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h261VideoCapability.stillImageTransmission",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

		case VIDEC_CODEC_H263:
			{
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_4CIF) nHas4CIF =n4CIFValue;
				if(nHas4CIF >0) nHasCIF=1;
				if(nHasCIF >0) nHasQCIF=1;
				//修改H263Extend能力参数
				iCapIndex ++;
				DualVideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				if (nHas4CIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.cif4MPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHas4CIF, NULL);
				}


				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.maxBitRate",
					subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}


				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.unrestrictedVector",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.arithmeticCoding",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.advancedPrediction",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.pbFrames",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.temporalSpatialTradeOffCapability",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.errorCompensation",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);




				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

		case VIDEC_CODEC_H263P:
			{
				if(nFormatValue >3) nHas4CIF = n4CIFValue; //VIDEO_FORMAT_4CIF 3
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_4CIF) nHas4CIF =n4CIFValue;
				if(nHas4CIF >0) nHasCIF=1;
				if(nHasCIF >0) nHasQCIF=1;

				//修改H263v2Extend能力参数
				iCapIndex ++;
				DualVideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				if (nHas4CIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.cif4MPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHas4CIF, NULL);
				}


				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.maxBitRate",
					subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				//注意此
				int tempNodeId=nodeId;
				//注意此
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability",subBuf);
				nodeId = pvtBuildByPath(valH, nodeId,tempbuf, NULL, NULL);

				if(nodeId >=0 )
				{
					sprintf(tempbuf,"unrestrictedVector");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"arithmeticCoding");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"advancedPrediction");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"pbFrames");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"temporalSpatialTradeOffCapability");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"errorCompensation");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//h263Options
					SetH263OptionParam(tempbuf,valH,nodeId,VideoFormatType);

					//sprintf(tempbuf,"h263Options.advancedIntraCodingMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.deblockingFilterMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.improvedPBFramesMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.unlimitedMotionVectors");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					//sprintf(tempbuf,"h263Options.fullPictureFreeze");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.partialPictureFreezeAndRelease");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					//sprintf(tempbuf,"h263Options.resizingPartPicFreezeAndRelease");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.fullPictureSnapshot");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.partialPictureSnapshot");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.videoSegmentTagging");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					//sprintf(tempbuf,"h263Options.progressiveRefinement");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.dynamicPictureResizingByFour");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					//sprintf(tempbuf,"h263Options.dynamicPictureResizingSixteenthPel");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.dynamicWarpingHalfPel");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.dynamicWarpingSixteenthPel");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.independentSegmentDecoding");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					//sprintf(tempbuf,"h263Options.slicesInOrder-NonRect");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.slicesInOrder-Rect");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					//sprintf(tempbuf,"h263Options.slicesNoOrder-NonRect");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.slicesNoOrder-Rect");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.alternateInterVLCMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.modifiedQuantizationMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.reducedResolutionUpdate");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.separateVideoBackChannel");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					////add customPictureFormat
					//SetH263OptionParam(3,capVideo.VideoCodecType,tempbuf,valH,nodeId);
				}


				//注意此
				nodeId=tempNodeId;



				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);


			}
			break;

		case VIDEC_CODEC_H263PP:
			{
				if(nFormatValue >3) nHas4CIF = n4CIFValue; //VIDEO_FORMAT_4CIF 3
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_4CIF) nHas4CIF =n4CIFValue;
				if(nHas4CIF >0) nHasCIF=1;
				if(nHasCIF >0) nHasQCIF=1;

				//修改H263v3Extend能力参数
				iCapIndex ++;

				DualVideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				if (nHasQCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.qcifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasQCIF, NULL);
				}
				if (nHasCIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.cifMPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHasCIF, NULL);
				}

				if (nHas4CIF)
				{
					sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.cif4MPI",subBuf);
					error = pvtBuildByPath(valH, nodeId,tempbuf, nHas4CIF, NULL);
				}


				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability.maxBitRate",
					subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}


				//注意此
				int tempNodeId=nodeId;
				//注意此
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.h263VideoCapability",subBuf);
				nodeId = pvtBuildByPath(valH, nodeId,tempbuf, NULL, NULL);

				if(nodeId >=0 )
				{
					sprintf(tempbuf,"unrestrictedVector");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"arithmeticCoding");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"advancedPrediction");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"pbFrames");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"temporalSpatialTradeOffCapability");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"errorCompensation");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//h263Options
					SetH263OptionParam(tempbuf,valH,nodeId,VideoFormatType);

					////h263Options

					//sprintf(tempbuf,"h263Options.advancedIntraCodingMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.deblockingFilterMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.improvedPBFramesMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.unlimitedMotionVectors");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					//sprintf(tempbuf,"h263Options.fullPictureFreeze");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.partialPictureFreezeAndRelease");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					//sprintf(tempbuf,"h263Options.resizingPartPicFreezeAndRelease");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.fullPictureSnapshot");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.partialPictureSnapshot");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.videoSegmentTagging");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					//sprintf(tempbuf,"h263Options.progressiveRefinement");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.dynamicPictureResizingByFour");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					//sprintf(tempbuf,"h263Options.dynamicPictureResizingSixteenthPel");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.dynamicWarpingHalfPel");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.dynamicWarpingSixteenthPel");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.independentSegmentDecoding");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);
					//sprintf(tempbuf,"h263Options.slicesInOrder-NonRect");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.slicesInOrder-Rect");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);


					//sprintf(tempbuf,"h263Options.slicesNoOrder-NonRect");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.slicesNoOrder-Rect");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.alternateInterVLCMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.modifiedQuantizationMode");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.reducedResolutionUpdate");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					//sprintf(tempbuf,"h263Options.separateVideoBackChannel");
					//error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					sprintf(tempbuf,"h263Options.videoBadMBsCap");
					error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					////add customPictureFormat
					//SetH263OptionParam(3,capVideo.VideoCodecType,tempbuf,valH,nodeId);

					//h263Version3Options
					{
						sprintf(tempbuf,"h263Options.h263Version3Options.dataPartitionedSlices");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.fixedPointIDCT0");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.interlacedFields");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.currentPictureHeaderRepetition");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.previousPictureHeaderRepetition");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.nextPictureHeaderRepetition");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.pictureNumber");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

						sprintf(tempbuf,"h263Options.h263Version3Options.spareReferencePictures");
						error = pvtBuildByPath(valH, nodeId,tempbuf, RV_FALSE, NULL);

					}

				}

				//注意此
				nodeId=tempNodeId;

				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);


			}
			break;
		case VIDEC_CODEC_H264:
			{
				iCapIndex ++;
				//int nValue=nHas4CIF;

				DualVideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[10]={0x00,0x08,0x81,0x71,0x00,0x00,0x01,0};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf,
					"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.1.parameterIdentifier.standard",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 41, NULL);	

				sprintf(tempbuf,
					"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.1.parameterValue.booleanArray",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 64, NULL);

				//sprintf(tempbuf,
				//	"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.2.parameterIdentifier.standard",
				//	subBuf);
				//error = pvtBuildByPath(valH, nodeId,tempbuf, 42, NULL);	

				//sprintf(tempbuf,
				//	"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.2.parameterValue.unsignedMin",
				//	subBuf);
				//error = pvtBuildByPath(valH, nodeId,tempbuf, nLevel, NULL);	

				//char *szPath=tempbuf;
				//int nNode=nodeId;
				//HPVT hPvt=valH;
				//char tempStr[512]={0};
				//int nCount=0;
				//sprintf(tempStr,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing",subBuf);

				////notice set standard value
				//tagCollapsing Collapsing[16]={0};
				//int nBitrateGet= SetH264CapOptionParam(nValue,Collapsing,nCount,3,capVideo.VideoCodecType,iEntryNumber);

				//for(int j=0;j<nCount;j++)
				//{
				//	sprintf(szPath,"%s.%u.parameterIdentifier.standard",tempStr,j+3);
				//	error = pvtBuildByPath(hPvt, nNode, szPath, Collapsing[j].standard, NULL);
				//	sprintf(szPath,"%s.%u.parameterValue.unsignedMin",tempStr,j+3);
				//	error = pvtBuildByPath(hPvt, nNode, szPath, Collapsing[j].value, NULL);
				//}

				char tempStr[512]={0};
				int nCount=0;
				sprintf(tempStr,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing",subBuf);
				//notice set standard value
				GetVideoH264MaxFSMBPS(nMaxMBPS,nMaxFS,nLevel,nVideoFrameRate,VideoFormatType);
				tagCollapsing Collapsing[]={ {42,nLevel},{3,nMaxMBPS},{4,nMaxFS}};
				nCount=sizeof(Collapsing)/sizeof(tagCollapsing);

				for(int j=0;j<nCount;j++)
				{
					sprintf(tempbuf,"%s.%u.parameterIdentifier.standard",tempStr,j+2);
					error = pvtBuildByPath(valH, nodeId, tempbuf, Collapsing[j].standard, NULL);
					sprintf(tempbuf,"%s.%u.parameterValue.unsignedMin",tempStr,j+2);
					error = pvtBuildByPath(valH, nodeId, tempbuf, Collapsing[j].value, NULL);
				}

//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//
//				if(m_nUseCapOLC==0) nVideoRate=nBitrateGet;
//#endif
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				////add
				//sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.maxBitRate",subBuf);
				//error = pvtBuildByPath(valH, nodeId,tempbuf, 0, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

		case VIDEC_CODEC_H264_SVC:
			{
				iCapIndex ++;
				//int nValue=nHas4CIF;

				DualVideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.capabilityIdentifier.standard", subBuf);
				unsigned char pValue[10]={0x00,0x08,0x81,0x71,0x00,0x00,0x01,0};
				error = pvtBuildByPath(valH, nodeId, tempbuf, 7, (char*)pValue);

				sprintf(tempbuf,
					"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.1.parameterIdentifier.standard",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 41, NULL);	


				sprintf(tempbuf,
					"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.1.parameterValue.booleanArray",
					subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 8, NULL);	


				//sprintf(tempbuf,
				//	"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.2.parameterIdentifier.standard",
				//	subBuf);
				//error = pvtBuildByPath(valH, nodeId,tempbuf, 42, NULL);	


				//sprintf(tempbuf,
				//	"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing.2.parameterValue.unsignedMin",
				//	subBuf);
				//error = pvtBuildByPath(valH, nodeId,tempbuf, nLevel, NULL);	

				//char *szPath=tempbuf;
				//int nNode=nodeId;
				//HPVT hPvt=valH;
				//char tempStr[512]={0};
				//int nCount=0;
				//struct tagCollapsing
				//{
				//	int standard;
				//	int value;
				//};
				//sprintf(tempStr,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing",subBuf);
				////notice set standard value
				//tagCollapsing Collapsing[]={ {6,80} };
				//nCount=sizeof(Collapsing)/sizeof(tagCollapsing);

				//char *szPath=tempbuf;
				//int nNode=nodeId;
				//HPVT hPvt=valH;
				//char tempStr[512]={0};
				//int nCount=0;
				//sprintf(tempStr,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing",subBuf);
				////notice set standard value
				//tagCollapsing Collapsing[16]={0};
				//int nBitrateGet= SetH264CapOptionParam(nValue,Collapsing,nCount,3,capVideo.VideoCodecType,iEntryNumber);

				//for(int j=0;j<nCount;j++)
				//{
				//	sprintf(szPath,"%s.%u.parameterIdentifier.standard",tempStr,j+3);
				//	error = pvtBuildByPath(hPvt, nNode, szPath, Collapsing[j].standard, NULL);
				//	sprintf(szPath,"%s.%u.parameterValue.unsignedMin",tempStr,j+3);
				//	error = pvtBuildByPath(hPvt, nNode, szPath, Collapsing[j].value, NULL);
				//}

				char tempStr[512]={0};
				int nCount=0;
				sprintf(tempStr,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.collapsing",subBuf);
				//notice set standard value
				GetVideoH264MaxFSMBPS(nMaxMBPS,nMaxFS,nLevel,nVideoFrameRate,VideoFormatType);
				tagCollapsing Collapsing[]={ {42,nLevel},{3,nMaxMBPS},{4,nMaxFS}};
				nCount=sizeof(Collapsing)/sizeof(tagCollapsing);

				for(int j=0;j<nCount;j++)
				{
					sprintf(tempbuf,"%s.%u.parameterIdentifier.standard",tempStr,j+2);
					error = pvtBuildByPath(valH, nodeId, tempbuf, Collapsing[j].standard, NULL);
					sprintf(tempbuf,"%s.%u.parameterValue.unsignedMin",tempStr,j+2);
					error = pvtBuildByPath(valH, nodeId, tempbuf, Collapsing[j].value, NULL);
				}

//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//
//				if(m_nUseCapOLC==0) nVideoRate=nBitrateGet;
//#endif

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);


				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;

		case VIDEC_CODEC_MPEG4: //notice only use ,use to call kedacom
			{
				//iEntryNumber = MPEG4_ENTRY +20000;
				iCapIndex ++;

				VideoEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

				sprintf(tempbuf, "%s.extendedVideoCapability.videoCapability.1.nonStandard.nonStandardIdentifier.h221NonStandard.t35CountryCode", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 38,NULL);

				sprintf(tempbuf, "%s.extendedVideoCapability.videoCapability.1.nonStandard.nonStandardIdentifier.h221NonStandard.t35Extension", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 0,NULL);

				sprintf(tempbuf, "%s.extendedVideoCapability.videoCapability.1.nonStandard.nonStandardIdentifier.h221NonStandard.manufacturerCode", subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 8888,NULL);

				sprintf(tempbuf, "%s.extendedVideoCapability.videoCapability.1.nonStandard.data", subBuf);
				//....kedacom_js_China............
				if(m_nUseCapOLC==0)
				{
					unsigned char pValue[32]=
					{ 0x00 ,0x00 ,0x00 ,0x11 ,0x6b ,
					0x65 ,0x64 ,0x61 ,0x63 ,0x6f ,0x6d ,0x5f ,0x6a ,
					0x73 ,0x5f ,0x43 ,0x68 ,0x69 ,0x6e ,0x61 ,0x00 ,
					0x00 ,0x03 ,0x00 ,0x00 ,0x01 ,0x01 ,0x00 ,0x01 ,
					0x01 ,0x01 ,0x00 };
					error = pvtBuildByPath(valH, nodeId, tempbuf, 32, (char*)pValue);
				}
				else
				{
					unsigned char pValue[32]=
					{ 0x00 ,0x00 ,0x00 ,0x11 ,0x6b ,
					0x65 ,0x64 ,0x61 ,0x63 ,0x6f ,0x6d ,0x5f ,0x6a ,
					0x73 ,0x5f ,0x43 ,0x68 ,0x69 ,0x6e ,0x61 ,0x00 ,
					0x00 ,0x01 ,0x40 ,0x00 ,0x00 ,0x00 ,0x00 ,0x01 ,
					0x00 ,0x00 ,0x00 };
					error = pvtBuildByPath(valH, nodeId, tempbuf, 32, (char*)pValue);
				}

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapability.1.genericVideoCapability.maxBitRate",subBuf);
				if(nVideoRate>0)
				{
					error = pvtBuildByPath(valH, nodeId,tempbuf, nVideoRate*10, NULL);
				}

				unsigned char data[16]={0x00,0x08,0x81,0x6F,0x01,0x02,0};
				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.capabilityIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)data);

				////add
				//sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.maxBitRate",subBuf);
				//error = pvtBuildByPath(valH, nodeId,tempbuf, 0, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterIdentifier.standard",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);

				sprintf(tempbuf,"%s.extendedVideoCapability.videoCapabilityExtension.1.collapsing.1.parameterValue.booleanArray",subBuf);
				error = pvtBuildByPath(valH, nodeId,tempbuf, 1, NULL);
				//设置同时能力集
				DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

			}
			break;
		default:
			break;
		}


	}
	if(iAlternative>1)
	{
		nSimultaneous++;
	}

	//设置远遥同时能力集
	iAlternative = 1;
	for(i=0; i<DapCap.m_nDataCapCount; i++)
	{
		int nDirection =DapCap.m_DataCap[i].m_nDirection;
		switch(DapCap.m_DataCap[i].m_DataCodecID)
		{
		case DATA_CODEC_H224:
			iEntryNumber = H224_ENTRY;
			iCapIndex ++;

			DataEntryBuild(valH,nodeId,subBuf,tempbuf,iEntryNumber,iCapIndex,nDirection);

			sprintf(tempbuf, "%s.application.h224.hdlcFrameTunnelling", subBuf);
			error = pvtBuildByPath(valH, nodeId, tempbuf, NULL, NULL);
			sprintf(tempbuf, "%s.maxBitRate", subBuf);
			error = pvtBuildByPath(valH, nodeId, tempbuf, 480, NULL);

			//备用
			//{
			//sprintf(tempbuf, "%s.application.genericDataCapability.capabilityIdentifier.standard", subBuf);
			//unsigned char pValue[10]={0x00,0x08,0x81,0x60,0x01,0x00,0};
			//error = pvtBuildByPath(valH, nodeId, tempbuf, 6, (char*)pValue);
			//sprintf(tempbuf, "%s.maxBitRate", subBuf);
			//error = pvtBuildByPath(valH, nodeId, tempbuf,50, NULL);
			//}
			//设置同时能力集
			DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);
			break;
		default:
			break;
		}

	}
	if(iAlternative>1)
	{
		nSimultaneous++;
	}


	//add by Jan for DTMF [20140730]
	if(m_nUseCapOLC==0)
	{
		iEntryNumber = DTMF_ENTRY;
		iAlternative = 1;
		iCapIndex ++;
		sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);

		sprintf(tempbuf, "capabilityTable.%d.capability.receiveUserInputCapability.dtmf", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, NULL, NULL);

		DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

		if(iAlternative>1)
		{
			nSimultaneous++;
		}
	}
	//end [20140730]

	//add by Jan for add cap [20130903] //add tai er ru wang test use
#if defined(TAIERLAB) && (TAIERLAB == 1)
	if(m_nUseCapOLC==0)
	{
		//UserInputCap
		iEntryNumber = USERINPUT_ENTRY;
		iAlternative = 1;
		iCapIndex ++;
		sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);

		sprintf(tempbuf, "capabilityTable.%d.capability.receiveUserInputCapability.basicString", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, NULL, NULL);

		DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

		if(iAlternative>1)
		{
			nSimultaneous++;
		}

		//ConfCap
		iEntryNumber = CONF_ENTRY;
		iAlternative = 1;
		iCapIndex ++;
		sprintf(tempbuf, "capabilityTable.%d.capabilityTableEntryNumber", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, iEntryNumber, NULL);

		sprintf(tempbuf, "capabilityTable.%d.capability.conferenceCapability", iCapIndex);
		error = pvtBuildByPath(valH, nodeId, tempbuf, NULL, NULL);

		sprintf(tempbuf,"chairControlCapability");
		error = pvtBuildByPath(valH, error,tempbuf, RV_FALSE, NULL);

		DescriptorBuild(valH,nodeId,tempbuf,iEntryNumber,nSimulNum,nSimultaneous,iAlternative);

		if(iAlternative>1)
		{
			nSimultaneous++;
		}
	}

#endif

	//end [20130903]

#endif

	return 0;
}


int CallCapability::GetVideoFormatValue(VIDEO_FORMAT_TYPE formatType)
{
	int nResult= 0;
	switch(formatType)
	{
	case VIDEO_FORMAT_QCIF:
		nResult= 0;
		break;
	case VIDEO_FORMAT_QVGA:
		nResult= 0;
		break;
	case VIDEO_FORMAT_CIF:
		nResult= 1;
		break;
	case VIDEO_FORMAT_VGA:
		nResult= 2;
		break;
	case VIDEO_FORMAT_4CIF:
		nResult= 3;
		break;
	case VIDEO_FORMAT_720I:
		nResult= 4;
		break;
	case VIDEO_FORMAT_SVGA:
		nResult= 5;
		break;
	case VIDEO_FORMAT_XGA:
		nResult= 6;
		break;
	case VIDEO_FORMAT_720P:
		nResult= 7;
		break;
	case VIDEO_FORMAT_1080I:
		nResult= 8;
		break;
	case VIDEO_FORMAT_SXGA:
		nResult= 9;
		break;
	case VIDEO_FORMAT_16CIF:
		nResult= 10;
		break;
	case VIDEO_FORMAT_1080P:
		nResult= 11;
		break;
	default:
		nResult= 0 ;
		break;
	}

	return nResult;
}

float CallCapability::GetFrameRateFromH264Param(int nMaxFS,int nMBPS)
{
	float fFrameRate= VideoFrameBase;
	//nMaxFS = nMaxFS -1;//notice
	if(nMBPS >0 && nMaxFS >0) fFrameRate = (nMBPS *500 ) / ( nMaxFS * 256 );
	return fFrameRate;
}

VIDEO_FORMAT_TYPE CallCapability::GetH264FormatTypeFromBitrate(float nFrame,int nProfile,int nBitrate)
{
	VIDEO_FORMAT_TYPE VideoFormatType =VIDEO_FORMAT_QCIF;
	int div=1;
	if(nProfile == 8) div=2;

	//from frame calc multiple 
	int nTheMaxBitrate = nBitrate;
	float fBitrate=nBitrate;
	float fFrameMultiple=1.0;
	float fFrameRate = VideoFrameBase;
	float fMaxFrame = VideoFrameBase;

	fFrameMultiple=nFrame / fMaxFrame;
	if(fFrameMultiple >0) fBitrate = fBitrate / fFrameMultiple;
	nTheMaxBitrate = fBitrate;

	struct tagH264Option
	{
		VIDEO_FORMAT_TYPE FormatType;
		int nMaxBitrate;
	};
	tagH264Option H264OptionArray[]=
	{
		{VIDEO_FORMAT_CIF,384},{VIDEO_FORMAT_4CIF,768},
		{VIDEO_FORMAT_720P,1024},{VIDEO_FORMAT_720P,1536},{VIDEO_FORMAT_720P,1920},
		{VIDEO_FORMAT_1080P,2048},{VIDEO_FORMAT_1080P,4096},{VIDEO_FORMAT_1080P,8192},{VIDEO_FORMAT_1080P,20480}
	};
	int nCount=sizeof(H264OptionArray) / sizeof(tagH264Option);
	if(nTheMaxBitrate >0)
	{
		for(int i=0;i<nCount;i++)
		{
			if(nTheMaxBitrate <= H264OptionArray[i].nMaxBitrate /div )
			{
				VideoFormatType = H264OptionArray[i].FormatType;
				break;
			}
		}
	}
	return VideoFormatType;
}

VIDEO_FORMAT_TYPE CallCapability::GetFormatTypeFromH264Param(int nProfile,int nLevel,int nMaxFS,int nMBPS,int nBitrate,int nType) //nType 0 get min ,1 get max
{
	VIDEO_FORMAT_TYPE nMaxFSType = GetVideoFormatTypeFromMaxFS(nMaxFS *256 );
	if(nMaxFS >0)
	{
		return nMaxFSType;
	}

	float nFrame=GetFrameRateFromH264Param(nMaxFS,nMBPS);
	VIDEO_FORMAT_TYPE nCompareType = GetFormatTypeFromH264OtherParam(nProfile,nLevel,nBitrate,nType,nMaxFSType,nFrame);
	return nCompareType;
}

VIDEO_FORMAT_TYPE CallCapability::GetFormatTypeFromLevel(int nLevel)
{
	VIDEO_FORMAT_TYPE videoFormatType=VIDEO_FORMAT_COUNT;
	switch(nLevel)
	{
	case 15:
	case 19:
	case 22:
		videoFormatType=VIDEO_FORMAT_QCIF;
		break;
	case 29:
	case 36:
		videoFormatType=VIDEO_FORMAT_CIF;
		break;
	case 43:
		//videoFormatType=VIDEO_FORMAT_CIF;
		//break;
	case 50:
	case 57:
	case 64:
		videoFormatType=VIDEO_FORMAT_4CIF;
		break;
	case 71:
		videoFormatType=VIDEO_FORMAT_720P;
		break;
	case 78:
		videoFormatType=VIDEO_FORMAT_SXGA;
		break;
	case 85:
		videoFormatType=VIDEO_FORMAT_1080P;
		break;
	default:
		if(nLevel > 85) videoFormatType=VIDEO_FORMAT_1080P;
		else videoFormatType=VIDEO_FORMAT_CIF;
		break;
	}

	////华三标清用productId: H3C MG6060
	//int nFound=-1;
	//nFound=m_strProductID.find("H3C MG6060");
	//if( nFound >=0 && nLevel==43)
	//{
	//	videoFormatType=VIDEO_FORMAT_4CIF;
	//}
	return videoFormatType;
}

VIDEO_FORMAT_TYPE CallCapability::GetVideoFormatTypeFromMaxFS(int nMaxFS,int nAllMaxFS)
{
	VIDEO_FORMAT_TYPE VideoFormatType =VIDEO_FORMAT_QCIF;

	if(nMaxFS ==0 && nAllMaxFS >0) nMaxFS=nAllMaxFS;

	if (nMaxFS >= 8100)
	{
		VideoFormatType = VIDEO_FORMAT_1080P;
	}
	else if (nMaxFS >= 5120)
	{
		VideoFormatType = VIDEO_FORMAT_SXGA;
	}
	else if (nMaxFS >= 3600)
	{
		VideoFormatType = VIDEO_FORMAT_720P;
	}
	else if (nMaxFS >= 3072)
	{
		VideoFormatType = VIDEO_FORMAT_XGA;
	}
	else if (nMaxFS >= 1875)
	{
		VideoFormatType = VIDEO_FORMAT_SVGA;
	}
	else if (nMaxFS >= 1584)
	{
		VideoFormatType = VIDEO_FORMAT_4CIF;
	}
	else if (nMaxFS >= 1200)
	{
		VideoFormatType = VIDEO_FORMAT_VGA;
	}
	else if (nMaxFS >= 396)
	{
		VideoFormatType = VIDEO_FORMAT_CIF;
	}
	else
	{
		VideoFormatType = VIDEO_FORMAT_QCIF;
	}

	return VideoFormatType;
}

VIDEO_FORMAT_TYPE CallCapability::GetFormatTypeFromH264OtherParam(int nProfile,int nLevel,int nBitrate,int nType,VIDEO_FORMAT_TYPE nMaxFSType,float nFrame) //nType 0 get min ,1 get max
{
	VIDEO_FORMAT_TYPE nBitrateType = GetH264FormatTypeFromBitrate(nFrame,nProfile,nBitrate);
	VIDEO_FORMAT_TYPE nLevelType = GetFormatTypeFromLevel(nLevel);
	int nBitrateValue =GetVideoFormatValue(nBitrateType);
	int nLevelValue =GetVideoFormatValue(nLevelType);
	int nMaxFSValue =GetVideoFormatValue(nMaxFSType);
	VIDEO_FORMAT_TYPE nCompareType=VIDEO_FORMAT_QCIF;
	int nCompare=0;
	if(nType ==0)
	{
		nCompare =(nBitrateValue >= nLevelValue ? nLevelValue:nBitrateValue);
	}
	else if(nType ==1)
	{
		nCompare =(nBitrateValue >= nLevelValue ? nBitrateValue:nLevelValue);
	}
	if(nCompare == nLevelValue) nCompareType =nLevelType;
	if(nCompare == nBitrateValue) nCompareType =nBitrateType;
	return nCompareType;
}

void CallCapability::GetVideoH264MaxFS(int & nMaxFS,VIDEO_FORMAT_TYPE format)
{
	nMaxFS = 176 *144 / 256;
	switch(format)
	{
	case VIDEO_FORMAT_QCIF:
		nMaxFS = 176 *144 / 256;
		break;
	case VIDEO_FORMAT_CIF:
		nMaxFS = 352 *288 / 256;
		break;
	case VIDEO_FORMAT_4CIF:
		nMaxFS = 704 *576 / 256;
		break;
	case VIDEO_FORMAT_16CIF:
		nMaxFS = 1408 *1152 / 256;
		break;
	case VIDEO_FORMAT_720P:
		nMaxFS = 1280 * 720 / 256;
		break;
	case VIDEO_FORMAT_720I:
		nMaxFS = 1280 * 360 / 256;
		break;
	case VIDEO_FORMAT_1080P:
		nMaxFS = 1920 * 1080 / 256;
		break;
	case VIDEO_FORMAT_1080I:
		nMaxFS = 1920 * 540 / 256;
		break;
	case VIDEO_FORMAT_VGA:
		nMaxFS = 640 * 480 / 256;
		break;
	case VIDEO_FORMAT_SVGA:
		nMaxFS = 800 * 600 / 256;
		break;
	case VIDEO_FORMAT_XGA:
		nMaxFS = 1024 * 768 / 256;
		break;
	case VIDEO_FORMAT_SXGA:
		nMaxFS = 1280 * 1024 / 256;
		break;
	}
}

char * GetAudioCodecName(X_AUDIO_CODEC_ID type,char * pNameBuf)
{
	switch(type)
	{
	case X_AUDIO_CODEC_G711U : strcpy(pNameBuf,"G711U"); break;
	case X_AUDIO_CODEC_G711A : strcpy(pNameBuf,"G711A"); break;//8khz
	case X_AUDIO_CODEC_G7231_53 : strcpy(pNameBuf,"G7231_53"); break;//8khz
	case X_AUDIO_CODEC_G7231_63 : strcpy(pNameBuf,"G7231_63"); break;//8khz
	case X_AUDIO_CODEC_G729 : strcpy(pNameBuf,"G729"); break;//8khz
	case X_AUDIO_CODEC_GSM : strcpy(pNameBuf,"GSM"); break;//8khz

	case X_AUDIO_CODEC_SIREN_G7221_7_24 : strcpy(pNameBuf,"S_G7221_7_24"); break;//16khz
	case X_AUDIO_CODEC_SIREN_G7221_7_32 : strcpy(pNameBuf,"S_G7221_7_32"); break;//16khz
	case X_AUDIO_CODEC_SIREN_G7221_14_24 : strcpy(pNameBuf,"S_G7221_14_24"); break;//32khz
	case X_AUDIO_CODEC_SIREN_G7221_14_32 : strcpy(pNameBuf,"S_G7221_14_32"); break;//32khz
	case X_AUDIO_CODEC_SIREN_G7221_14_48 : strcpy(pNameBuf,"S_G7221_14_48"); break;//32khz

	case X_AUDIO_CODEC_G722_64 : strcpy(pNameBuf,"G722_64"); break;//16khz
	case X_AUDIO_CODEC_G7221_7_24 : strcpy(pNameBuf,"G7221_7_24"); break;//16khz
	case X_AUDIO_CODEC_G7221_7_32 : strcpy(pNameBuf,"G7221_7_32"); break;//16khz
	case X_AUDIO_CODEC_G7221_14_24 : strcpy(pNameBuf,"G7221_14_24"); break;//32khz
	case X_AUDIO_CODEC_G7221_14_32 : strcpy(pNameBuf,"G7221_14_32"); break;//32khz
	case X_AUDIO_CODEC_G7221_14_48 : strcpy(pNameBuf,"G7221_14_48"); break;//32khz
	case X_AUDIO_CODEC_G719_32 : strcpy(pNameBuf,"G719_32"); break;
	case X_AUDIO_CODEC_G719_48 : strcpy(pNameBuf,"G719_48"); break;
	case X_AUDIO_CODEC_G719_64 : strcpy(pNameBuf,"G719_64"); break;
	default: strcpy(pNameBuf,"UnKnown"); break;
	}
	return pNameBuf;
}

char * GetVideoCodecName(VIDEC_CODEC_TYPE type,char * pNameBuf)
{
	switch(type)
	{
	case VIDEC_CODEC_H261 : strcpy(pNameBuf,"H261"); break;
	case VIDEC_CODEC_H263 : strcpy(pNameBuf,"H263"); break;
	case VIDEC_CODEC_H263P : strcpy(pNameBuf,"H263P"); break;
	case VIDEC_CODEC_H263PP : strcpy(pNameBuf,"H263PP"); break;	
	case VIDEC_CODEC_H264 : strcpy(pNameBuf,"H264"); break;
	case VIDEC_CODEC_LSCC : strcpy(pNameBuf,"LSCC"); break;
	case VIDEC_CODEC_MPEG4 : strcpy(pNameBuf,"MPEG4"); break;
	case VIDEC_CODEC_H264_SVC : strcpy(pNameBuf,"H264_SVC"); break;
	default: strcpy(pNameBuf,"UnKnown"); break;
	}
	return pNameBuf;
}

char * GetVideoFormatName(VIDEO_FORMAT_TYPE type,char * pNameBuf)
{
	switch(type)
	{
	case VIDEO_FORMAT_QCIF : strcpy(pNameBuf,"FORMAT_QCIF"); break;
	case VIDEO_FORMAT_CIF : strcpy(pNameBuf,"FORMAT_CIF"); break;
	case VIDEO_FORMAT_4CIF : strcpy(pNameBuf,"FORMAT_4CIF"); break;
	case VIDEO_FORMAT_16CIF : strcpy(pNameBuf,"FORMAT_16CIF"); break;	
	case VIDEO_FORMAT_720I : strcpy(pNameBuf,"FORMAT_720I"); break;
	case VIDEO_FORMAT_720P : strcpy(pNameBuf,"FORMAT_720P"); break;
	case VIDEO_FORMAT_1080I : strcpy(pNameBuf,"FORMAT_1080I"); break;
	case VIDEO_FORMAT_1080P : strcpy(pNameBuf,"FORMAT_1080P"); break;
	case VIDEO_FORMAT_QVGA : strcpy(pNameBuf,"FORMAT_QVGA"); break;
	case VIDEO_FORMAT_VGA : strcpy(pNameBuf,"FORMAT_VGA"); break;
	case VIDEO_FORMAT_SVGA : strcpy(pNameBuf,"FORMAT_SVGA"); break;
	case VIDEO_FORMAT_XGA : strcpy(pNameBuf,"FORMAT_XGA"); break;
	case VIDEO_FORMAT_SXGA : strcpy(pNameBuf,"FORMAT_SXGA"); break;
	default: strcpy(pNameBuf,"FORMAT_UnKnown"); break;
	}
	return pNameBuf;
}
