#ifdef HAS_SIP
#include "stdafx.h"
#endif

#ifdef HAS_RTPRTMP
#include "../stdafx.h"
#endif

#include "SIPCallCapability.h"

#ifdef HAS_SIP
#include "SIPCallMgr.h"
#endif


//#ifdef HAS_H323
#ifndef HAS_RTPRTMP

extern void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info);

#else



void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info)
{

}

#endif


SIPCallCapability::SIPCallCapability(void)
{
	m_H264PT=106, m_H263PPT=107, m_H263PPPT=108, m_H224PT=100;
	m_VideoPort=6000, m_AudioPort=6002, m_DualVideoPort=6006,m_H224Port=6004;

	m_H264SVCPT=104;
	m_DualH264SVCPT=105;

	m_DualH264PT=106;
	m_DualH263PPT=107;
	m_DualH263PPPT=108;

	m_nBFCPConfId=0;
	m_nBFCPUserId=0;
	m_nBFCPFloorId=0;
	m_nBFCPDualVideoLabel=12;
	strcpy(m_strBFCPFloorControl,"c-only");
	strcpy(m_strBFCPSetupType,"active");

	m_nBFCPTransportType=e_BFCP_Transport_UDP;
	m_nBFCPTransportPort=5070;

	m_G7221_24k_PT=110;
	m_G7221_32k_PT=111;
	m_G7221C_24k_PT=112;
	m_G7221C_32k_PT=113;
	m_G7221C_48k_PT=114;

	m_H263PResolutionOption=e_H263_Resolution_All; //e_H263_Resolution_None
	m_H263PPResolutionOption=e_H263_Resolution_All;
	m_DualH263PResolutionOption=e_H263_Resolution_All;
	m_DualH263PPResolutionOption=e_H263_Resolution_All;

	m_nHighProfileLevel=H264_SDP_LEVEL_1080P;


	//注意：待设代理参数
	////Media Attribute (a): nortpproxy:yes	
	////buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=nortpproxy:yes\r\n");
	//设置带宽参数，是否加入RTCP参数

	//注意：待设user_agent
	//pj_ansi_sprintf(tmp, "IMXEC");
	//pj_strdup2_with_null(app_config.pool, &cfg->cfg.user_agent, tmp);

	m_bProxy=false;
	m_bRtcp=false; //notice call Polycom/Polycom RMX 2000/8.3.0 is not set true,if is true RMX2000 Not Send video

	m_bUseFECEnable=true;

	m_nAudioBandwidth=64;
	m_nVideoBandwidth=768;
	m_nDualVideoBandwidth=768;

	memset(m_strLocalIP,0,128);
	memset(m_strOwnerUsername,0,128);

	strcpy(m_strLocalIP,"192.168.5.69");
	strcpy(m_strOwnerUsername,"IMXEC");


	m_VideoPayloadType =0;
	m_AudioPayloadType=0;
	m_DualVideoPayloadType=0;
	m_H224PayloadType=0;

	m_NodeId = 0;  //能力集结点ID
			    
	//因为要使用内存段比较是否相同 所有要初始化为0
	memset(&m_AudioCap,0,sizeof(AudioCap));				    
	memset(&m_VideoCap,0,sizeof(VideoCap));				    
	memset(&m_H239VideoCap,0,sizeof(H239VideoCap));				    
	memset(&m_DataCap,0,sizeof(DataCap));				    


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
	m_AudioCap.m_Caps[capNum].AudioCodecID = X_AUDIO_CODEC_G7231_63;
	capNum++;
	m_AudioCap.m_nCapNum = capNum;

	capNum = 0;

	m_VideoCap.m_Caps[capNum].VideoCodecType = VIDEC_CODEC_H264;
	m_VideoCap.m_Caps[capNum].nMaxBitrate = H264_MBPS_QCIF;	
	m_VideoCap.m_Caps[capNum].n4CIF = H264_SDP_LEVEL_QCIF;  //H264此用于LEVEL
	m_VideoCap.m_Caps[capNum].nQCIF = H264_MacroblockPS_QCIF;  //H264此用于 最大宏块处理速度
	m_VideoCap.m_Caps[capNum].nCIF = H264_SDP_MaxFS_QCIF;     //H264此用于 最大帧长
	capNum++;

	m_VideoCap.m_Caps[capNum].VideoCodecType = VIDEC_CODEC_H264;
	m_VideoCap.m_Caps[capNum].nMaxBitrate = H264_MBPS_CIF;	  
	m_VideoCap.m_Caps[capNum].n4CIF = H264_SDP_LEVEL_CIF;    //H264此用于LEVEL
	m_VideoCap.m_Caps[capNum].nQCIF = H264_MacroblockPS_CIF;    //H264此用于ENTRY
	m_VideoCap.m_Caps[capNum].nCIF = H264_SDP_MaxFS_CIF;       //H264此用于CAP TYPE
	capNum++;

	m_VideoCap.m_Caps[capNum].VideoCodecType = VIDEC_CODEC_H263;
	m_VideoCap.m_Caps[capNum].nMaxBitrate = H263_MBPS_ALL;	//384Kbps
	m_VideoCap.m_Caps[capNum].nQCIF = 1;
	m_VideoCap.m_Caps[capNum].nCIF = 1;
	m_VideoCap.m_Caps[capNum].n4CIF = 2;
	capNum++;

	m_VideoCap.m_Caps[capNum].VideoCodecType = VIDEC_CODEC_H263P;
	m_VideoCap.m_Caps[capNum].nMaxBitrate = H263_MBPS_ALL;	//384Kbps
	m_VideoCap.m_Caps[capNum].nQCIF = 1;
	m_VideoCap.m_Caps[capNum].nCIF = 1;
	m_VideoCap.m_Caps[capNum].n4CIF = 2;
	capNum++;

	m_VideoCap.m_nCapNum = capNum;


	capNum = 0;
	m_H239VideoCap.m_Caps[capNum].VideoCodecType = VIDEC_CODEC_H264;
	m_H239VideoCap.m_Caps[capNum].nMaxBitrate = H264_MBPS_CIF;	
	m_H239VideoCap.m_Caps[capNum].n4CIF = H264_SDP_LEVEL_CIF;  //H264此用于LEVEL
	m_H239VideoCap.m_Caps[capNum].nQCIF = H264_MacroblockPS_CIF;  //H264此用于 最大宏块处理速度
	m_H239VideoCap.m_Caps[capNum].nCIF = H264_SDP_MaxFS_CIF;     //H264此用于 最大帧长
	capNum++;
	m_H239VideoCap.m_nCapNum = capNum;


	capNum = 0;
	m_DataCap.m_Caps[capNum].DataCodecID = DATA_CODEC_H224;//远遥
	capNum++;
	m_DataCap.m_nCapNum = capNum;

}

SIPCallCapability::~SIPCallCapability(void)
{
}

//opetionType first is 0 exist is 1
int SIPCallCapability::FindEntryFromCapsArray(int type ,int iEntryNumber,int opetionType)
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
		case G719_ENTRY_32K: 
			audioCodecID = X_AUDIO_CODEC_G719_32;
			break;
		case G719_ENTRY_48K: 
			audioCodecID = X_AUDIO_CODEC_G719_48;
			break;
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
			if( (iEntryNumber>=(H264_ENTRY_QCIF+20000) ) && (iEntryNumber<=(H264_ENTRY_HIG1080P+20000) ) )
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
			H264Level = H264_SDP_LEVEL_QCIF;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_CIF:
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_SDP_LEVEL_CIF;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_4CIF:
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_SDP_LEVEL_4CIF;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_16CIF:
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_SDP_LEVEL_16CIF;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_720I: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_SDP_LEVEL_720I;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_720P: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_SDP_LEVEL_720P;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_1080I: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_SDP_LEVEL_1080I;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_1080P: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_SDP_LEVEL_1080P;  //H264此用于LEVEL
			break;
		case H264_ENTRY_VGA: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_SDP_LEVEL_VGA;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_SVGA: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_SDP_LEVEL_SVGA;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_XGA: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_SDP_LEVEL_XGA;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_SXGA: 
			videoCodecType = VIDEC_CODEC_H264;
			H264Level = H264_SDP_LEVEL_SXGA;  //H264此用于LEVEL
			break; 
		case H264_ENTRY_HIG720P: 
			videoCodecType = VIDEC_CODEC_H264_SVC;
			H264Level = H264_SDP_LEVEL_720P;  //H264此用于LEVEL
			break;
		case H264_ENTRY_HIG1080P: 
			videoCodecType = VIDEC_CODEC_H264_SVC;
			H264Level = H264_SDP_LEVEL_1080P;  //H264此用于LEVEL
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
					if( (videoCodecType == VIDEC_CODEC_H264 ) || (videoCodecType == VIDEC_CODEC_H264_SVC ) )
					{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

						if (capSet.m_Caps[i].n4CIF <= H264Level) //h264 level = is
#else
						if (capSet.m_Caps[i].n4CIF >= H264Level) //h264 level = is
#endif

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
					if( (videoCodecType == VIDEC_CODEC_H264 ) || (videoCodecType == VIDEC_CODEC_H264_SVC ) )
					{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

						if (capSet.m_Caps[i].n4CIF <= H264Level) //h264 level = is
#else
						if (capSet.m_Caps[i].n4CIF >= H264Level) //h264 level = is
#endif

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

int SIPCallCapability::IsExistSimul(int type ,int iEntryNumber)
{
	int result = 0;
	result=FindEntryFromCapsArray(type,iEntryNumber,1);
	return 	result;
} 

int SIPCallCapability::IsFirstSimul(int type ,int iEntryNumber)
{
	int result = 0;	
	result=FindEntryFromCapsArray(type,iEntryNumber,0);
	return result;
}

void SIPCallCapability::SetSDPMediaPort(EMediaType type,int port)
{
	switch(type )
	{ 
	case e_Media_Video:
		m_VideoPort=port;
		break;
	case e_Media_Audio:
		m_AudioPort=port;
		break;
	case e_Media_H224:
		m_H224Port=port;
		break;
	case e_Media_DualVideo:
		m_DualVideoPort=port;
		break;
	default:break;
	}
}

void SIPCallCapability::SetH263HighResolution(EMediaCoderType coderType,int width,int height)
{
	EH263ResolutionOption option=e_H263_Resolution_None;
	switch(width)
	{
	case 1280:
		if(height==720) option=e_H263_1280_720;
		if(height==1024) option=e_H263_1280_1024;
		break;
	case 1024:
		if(height==768) option=e_H263_1024_768;
		if(height==576) option=e_H263_1024_576;
		break;
	case 800:
		if(height==600) option=e_H263_800_600;
		break;
	default:
		break;
	}

	if(coderType==e_Video_H263P) SetH263POption(option);
	if(coderType==e_Video_H263PP) SetH263PPOption(option);
	if(coderType==e_DualVideo_H263P) SetDualH263POption(option);
	if(coderType==e_DualVideo_H263PP) SetDualH263PPOption(option);
}

void SIPCallCapability::SetH263POption(EH263ResolutionOption option)
{
	m_H263PResolutionOption=option;
}
void SIPCallCapability::SetH263PPOption(EH263ResolutionOption option)
{
	m_H263PPResolutionOption=option;
}
void SIPCallCapability::SetDualH263POption(EH263ResolutionOption option)
{
	m_DualH263PResolutionOption=option;
}
void SIPCallCapability::SetDualH263PPOption(EH263ResolutionOption option)
{
	m_DualH263PPResolutionOption=option;
}

void SIPCallCapability::SetRtpProxyAttribute(bool proxy)
{
	m_bProxy=proxy;
}

void SIPCallCapability::SetRtcpAttribute(bool rtcp)
{
	m_bRtcp=rtcp;
}
void SIPCallCapability::SetSDPMediaIP(char * ip)
{
	if(ip!=NULL)
		strcpy(m_strLocalIP,ip);

}
//OwnerUsername: Administrator
void SIPCallCapability::SetSDPOwnerUsername(char * name)
{
	if(name!=NULL)
		strcpy(m_strOwnerUsername,name);
}

void SIPCallCapability::SetSDPMediaBandwidth(int audioBand,int videoBand,int dualVideoBand)
{
	m_nAudioBandwidth=audioBand;
	m_nVideoBandwidth=videoBand;
	m_nDualVideoBandwidth=dualVideoBand;
}


void SIPCallCapability::SetBFCPConfId(int confId)
{
	m_nBFCPConfId=confId;
}

void SIPCallCapability::SetBFCPFloorId(int floorId)
{
	m_nBFCPFloorId=floorId;
}

void SIPCallCapability::SetBFCPUserId(int userId)
{
	m_nBFCPUserId=userId;
}

void SIPCallCapability::SetBFCPDualVideoLabelValue(int label)
{
	m_nBFCPDualVideoLabel=label;
}

void SIPCallCapability::SetBFCPTransportType(EBFCPTransportType type)
{

	m_nBFCPTransportType=type;
}

void SIPCallCapability::SetBFCPTransportPort(int port)
{

	m_nBFCPTransportPort=port;
}

void SIPCallCapability::SetBFCPFloorControlType(EBFCPFloorControlType type)
{
// 1 s-only floor control server
// 2 c-only floor control client
// 3 c-s    both floor control client and server
	if(type==e_Floor_Control_Server)
		strcpy(m_strBFCPFloorControl,"s-only");
	if(type==e_Floor_Control_Client)
		strcpy(m_strBFCPFloorControl,"c-only");
	if(type==e_Floor_Control_All)
		strcpy(m_strBFCPFloorControl,"c-s");
}

void SIPCallCapability::SetBFCPSetupType(EBFCPSetupType type)
{

	if(type==e_BFCP_Setup_Passive)
		strcpy(m_strBFCPSetupType,"passive");
	if(type==e_BFCP_Setup_Actpass)
		strcpy(m_strBFCPSetupType,"actpass");
	if(type==e_BFCP_Setup_Active)
		strcpy(m_strBFCPSetupType,"active");

}


void SIPCallCapability::SetPayloadType(EMediaCoderType type,int pt)
	{
	switch(type )
		{ 
		case e_Video_H264:
			m_H264PT=pt;
			break;
		case e_Video_H263P:
			m_H263PPT=pt;
			break;
		case e_Video_H263PP:
			m_H263PPPT=pt;
			break;
		case e_Video_H264SVC:
			m_H264SVCPT=pt;
			break;
		case e_DualVideo_H264SVC:
			m_DualH264SVCPT=pt;
			break;
		case e_DualVideo_H264:
			m_DualH264PT=pt;
			break;
		case e_DualVideo_H263P:
			m_DualH263PPT=pt;
			break;
		case e_DualVideo_H263PP:
			m_DualH263PPPT=pt;
			break;

		case e_Audio_G7221_24k:
			m_G7221_24k_PT=pt;
			break;
		case e_Audio_G7221_32k:
			m_G7221_32k_PT=pt;
			break;
		case e_Audio_G7221C_24k:
			m_G7221C_24k_PT=pt;
			break;
		case e_Audio_G7221C_32k:
			m_G7221C_32k_PT=pt;
			break;
		case e_Audio_G7221C_48k:
			m_G7221C_48k_PT=pt;
			break;
		case e_H224:
			m_H224PT=pt;
			break;
		default:
			break;
		}
	}


int SIPCallCapability::SetSimultaneousCap( )
{
	return 1;
}


//对H263P及H263PP设置更高分辨率能力集参数
void SIPCallCapability::SetH263CustomStr(EH263ResolutionOption option,char * buffer,int &buf_len,int maxLen)
{
	switch(option)
	{
	case e_H263_Resolution_None:
		break;
	case e_H263_1280_1024:
		buf_len+=snprintf(&buffer[buf_len],maxLen-buf_len," custom=1280,1024,3");
		break;
	case e_H263_1280_720:
		buf_len+=snprintf(&buffer[buf_len],maxLen-buf_len," custom=1280,720,3");
		break;				
	case e_H263_1024_768:
		buf_len+=snprintf(&buffer[buf_len],maxLen-buf_len," custom=1024,768,1");
		break;			
	case e_H263_1024_576:
		buf_len+=snprintf(&buffer[buf_len],maxLen-buf_len," custom=1024,576,2");
		break;		
	case e_H263_800_600:
		buf_len+=snprintf(&buffer[buf_len],maxLen-buf_len," custom=800,600,1");
		break;
	case e_H263_Resolution_All:
		buf_len+=snprintf(&buffer[buf_len],maxLen-buf_len," custom=1280,1024,3 custom=1280,720,3 custom=1024,768,1 custom=1024,576,2 custom=800,600,1");
		break;
	default:
		break;
	}
}

void ConverseIP(char * strDstIP,int & nMaxDstLen,char * strSrcIP)
{
	unsigned long nSrcIP;
	nSrcIP=inet_addr(strSrcIP);
	if( (nSrcIP!= INADDR_NONE) && (nMaxDstLen>15) )
	{
		//sprintf(strDstIP,"%d.%d.%d.%d",((int)(nSrcIP >>0)&0xFF) ,((int)(nSrcIP >>8)&0xFF) ,((int)(nSrcIP >> 16)&0xFF) ,((int)(nSrcIP >> 24)&0xFF) );
		sprintf(strDstIP,"%d.%d.%d.%d",((int)(nSrcIP >>24)&0xFF) ,((int)(nSrcIP >>16)&0xFF) ,((int)(nSrcIP >> 8)&0xFF) ,((int)(nSrcIP >> 0)&0xFF) );
		nMaxDstLen=strlen(strDstIP);
	}
	else nMaxDstLen=0;
}

#ifndef HAS_SIP

int pj_gettimeofday(pj_time_val *tv)
{
#ifdef WIN32

#define SECS_TO_FT_MULT 10000000

	LARGE_INTEGER base_time;
    LARGE_INTEGER li;
    pj_status_t status;
    SYSTEMTIME st;
    FILETIME ft;
    memset(&st,0,sizeof(st));
    /* Standard Win32 GetLocalTime */
    GetLocalTime(&st);
    SystemTimeToFileTime(&st, &ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
   // li.QuadPart /= SECS_TO_FT_MULT;
   // li.QuadPart -= base_time.QuadPart;
    tv->sec = li.LowPart;
    tv->msec = st.wMilliseconds;

    return PJ_SUCCESS;

#else

    struct timeval tval;
  
    do_gettimeofday(&tval);
    tv->sec = tval.tv_sec;
    tv->msec = tval.tv_usec / 1000;

    return 0;

#endif
}


#endif

int SIPCallCapability::SDP_Capabilitis_Add(pj_pool_t *pool,pjmedia_sdp_session **p_sdp,char * copyOutBuf)
{
	//fmtp:96 CIF4=2;CIF=1;QCIF=1;SQCIF=1;F;J;T
	//fmtp:106 profile-level-id=42800d; max-mbps=20000; max-fs=1792
	//profile-level-id:这个参数用于指示H.264流的profile类型和级别. 由Base16(十六进制)表示的3个字节. 
	//第一个字节表示H.264的Profile类型, 第三个字节表示H.264的Profile级别.

	int  nUseDualVideoToken = 0; //use token interactive is 1,else is 0
	int  nSDPOrignIPConverse = 0;	 //SDP orign ip is converse ,not converse is 0 ,converse is 1

#ifdef HAS_SIP

	nSDPOrignIPConverse = SIPCallMgr::Instance().m_nSDPOrignIPConverse;
	nUseDualVideoToken = SIPCallMgr::Instance().m_nUseDualVideoToken;

#endif

	unsigned int i=0;
	char buf[4096]={0};
	int buf_len=0;
	int MAX_BUF_STR_LEN = 4096;
	char media[256]={0};		
	int media_len=0;
	int MAX_MEDIA_STR_LEN = 256 ;
	char ICEAttrStr[2000]={0}; //用于ICE
	int videoRTCPPort=m_VideoPort+1;
	char * paramToken=";";//modify 20140703 hdx7000 and huawei te30 use ;

	//Video SDP	//SDP每一行都以回车换行结束
	pj_status_t status=-1;
	pjmedia_sdp_session *sdp=NULL;

	//注意 音频SDP 必须有
	if(m_AudioCap.m_nCapNum<=0) return -1;

	pj_time_val tv;
	pj_gettimeofday(&tv);   
	unsigned int vesion=tv.sec + 2208988800UL;

	char sdpHeader[512]={0};
	int sdpHeaderLen=0;
	int MAX_SDPHR_STR_LEN = 512 ;
	char attr[512]={0};
	int attr_len=0;
	int MAX_ATTR_STR_LEN = 512 ;
	int audioRTCPPort=m_AudioPort+1;
	int h224RTCPPort=m_H224Port+1;
	int mediaCapNum=0;

	if(m_VideoCap.m_nCapNum >0 ) LogFile(NULL,NULL,"","","VideoCap CapNum >0");
	if(m_H239VideoCap.m_nCapNum >0) LogFile(NULL,NULL,"","","DualVideoCap CapNum >0");

	char strLocalConverseIP[128]={"0.0.0.0"};
	int nMaxConverseLen=128;
	ConverseIP(strLocalConverseIP,nMaxConverseLen,m_strLocalIP);
	if( nSDPOrignIPConverse <= 0 )
	{
		strcpy(strLocalConverseIP,m_strLocalIP);
	}

	sdpHeaderLen+=snprintf(&sdpHeader[sdpHeaderLen],MAX_SDPHR_STR_LEN-sdpHeaderLen,
		"v=0\r\no=%s %u %u IN IP4 %s\r\ns=%s\r\nc=IN IP4 %s\r\nt=0 0\r\n",m_strOwnerUsername,vesion,vesion,strLocalConverseIP,m_strOwnerUsername,m_strLocalIP);
	buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",sdpHeader);

	//Session Attribute (a): X-nat:0 Unknown
	//buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=X-nat:0 Unknown\r\n");

	//audio
	if(m_AudioCap.m_nCapNum>0) 
	{
		media_len=0;
		media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len,"m=audio %d RTP/AVP",m_AudioPort);

		for(i=0; i<m_AudioCap.m_nCapNum; i++)
		{
			switch( m_AudioCap.m_Caps[i].AudioCodecID )
			{
			case X_AUDIO_CODEC_G711A:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",8);
				}
				break; 
			case X_AUDIO_CODEC_G711U:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",0);
				}
				break;
			case X_AUDIO_CODEC_G722_64:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",9);
				}
				break;
			case X_AUDIO_CODEC_G7221_7_24:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",m_G7221_24k_PT);
				}
				break;
			case X_AUDIO_CODEC_G7221_7_32:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",m_G7221_32k_PT);
				}
				break;
			case X_AUDIO_CODEC_G7221_14_24:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",m_G7221C_24k_PT);
				}
				break;
			case X_AUDIO_CODEC_G7221_14_32:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",m_G7221C_32k_PT);
				}
				break;

			case X_AUDIO_CODEC_G7221_14_48:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",m_G7221C_48k_PT);
				}
				break;		
			case X_AUDIO_CODEC_G729:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",18);
				}
				break;
			case X_AUDIO_CODEC_GSM:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",3);
				}
				break;
			default:
				break;
			}
		}

		media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",101);

		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",media);
		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"b=AS:%d\r\n",m_nAudioBandwidth);

		if(m_bRtcp)
			buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=rtcp:%d IN IP4 %s\r\n",audioRTCPPort,m_strLocalIP);

		for(i=0; i<m_AudioCap.m_nCapNum; i++)
		{
			switch( m_AudioCap.m_Caps[i].AudioCodecID )
			{
			case X_AUDIO_CODEC_G711A:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:8 PCMA/8000\r\n");
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);
				}
				break; 
			case X_AUDIO_CODEC_G711U:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:0 PCMU/8000\r\n");
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);
				}
				break;
			case X_AUDIO_CODEC_G722_64:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:9 G722/16000\r\n");
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=64000\r\n",9);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);
				}
				break;
			case X_AUDIO_CODEC_G7221_7_24:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d G7221/16000\r\n",m_G7221_24k_PT);
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=24000\r\n",m_G7221_24k_PT);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);		
				}
				break;
			case X_AUDIO_CODEC_G7221_7_32:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d G7221/16000\r\n",m_G7221_32k_PT);
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=32000\r\n",m_G7221_32k_PT);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);		
				}
				break;

			case X_AUDIO_CODEC_G7221_14_24:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d G7221/32000\r\n",m_G7221C_24k_PT);
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=24000\r\n",m_G7221C_24k_PT);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);		
				}
				break;
			case X_AUDIO_CODEC_G7221_14_32:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d G7221/32000\r\n",m_G7221C_32k_PT);
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=32000\r\n",m_G7221C_32k_PT);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);		
				}
				break;
			case X_AUDIO_CODEC_G7221_14_48:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d G7221/32000\r\n",m_G7221C_48k_PT);
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=32000\r\n",m_G7221C_48k_PT);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);		
				}
				break;		
			case X_AUDIO_CODEC_G729:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:18 G729/8000\r\na=fmtp:18 annexb=no\r\n");
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);
				}
				break;
			case X_AUDIO_CODEC_GSM:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:3 GSM/8000\r\n");
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);
				}
				break;
			default:
				break;
			}

		}

		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=rtpmap:101 telephone-event/8000\r\na=fmtp:101 0-16\r\na=sendrecv\r\n");
		if(m_bUseFECEnable ) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=reliablertp:fec-enable\r\n");

	}


	//video dual video
	for(mediaCapNum=1;mediaCapNum<=2;mediaCapNum++)
	{
		int nMediaPort=0;
		int nMediaRTCPPort=0;
		int nMediaBandwidth=0;
		int nH264PT=0;
		int nH264SVCPT=0;
		int nH263PPT=0;
		int nH263PPPT=0;
		EH263ResolutionOption h263POption=e_H263_Resolution_None;
		EH263ResolutionOption h263PPOption=e_H263_Resolution_None;

		bool MediaH264RtpMap=false;
		VideoCap MediaCap;
		memset(&MediaCap,0,sizeof(VideoCap));
		MediaCap.m_nCapNum=0;

		//video
		if(mediaCapNum==1)
		{
			if(m_VideoCap.m_nCapNum<=0) continue;
			else
			{
				//此块变量为用于设置视频及辅流能力集代码相同而定义
				nMediaPort=m_VideoPort;
				nMediaRTCPPort=m_VideoPort+1;
				nMediaBandwidth=m_nVideoBandwidth;
				nH264PT=m_H264PT;
				nH264SVCPT=m_H264SVCPT;
				nH263PPT=m_H263PPT;
				nH263PPPT=m_H263PPPT;
				h263POption=m_H263PResolutionOption;
				h263PPOption=m_H263PPResolutionOption;
				memcpy(&MediaCap,&m_VideoCap,sizeof(VideoCap));
			}
		}

		//Dual video
		if(mediaCapNum==2)
		{
			if(m_H239VideoCap.m_nCapNum >0)
			{
				//此块变量为用于设置视频及辅流能力集代码相同而定义
				nMediaPort=m_DualVideoPort;
				nMediaRTCPPort=m_DualVideoPort+1;
				nMediaBandwidth=m_nDualVideoBandwidth;
				nH264PT=m_DualH264PT;
				nH264SVCPT=m_DualH264SVCPT;
				nH263PPT=m_DualH263PPT;
				nH263PPPT=m_DualH263PPPT;
				h263POption=m_DualH263PResolutionOption;
				h263PPOption=m_DualH263PPResolutionOption;

				memcpy(&MediaCap,&m_H239VideoCap,sizeof(H239VideoCap));

				if(m_VideoCap.m_nCapNum>0)
				{
					if( nUseDualVideoToken>0)
						buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=content:main\r\n");
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=label:%d\r\n",m_nBFCPDualVideoLabel-1); //-2
					if( nUseDualVideoToken>0)
						buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=answer:full\r\n");
				}
			}

			if( nUseDualVideoToken>0)
			{
				if(/*m_VideoCap.m_nCapNum >0 || */ m_H239VideoCap.m_nCapNum >0)
				{
					if(m_nBFCPTransportType==e_BFCP_Transport_UDP)
					{
						buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"m=application %d UDP/BFCP *\r\n",m_nBFCPTransportPort);
						LogFile(NULL,NULL,"","","application UDP/BFCP SDP Add");
					}
					if(m_nBFCPTransportType==e_BFCP_Transport_TCP)
					{
						buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"m=application %d TCP/BFCP *\r\n",m_nBFCPTransportPort);
						LogFile(NULL,NULL,"","","application TCP/BFCP SDP Add");
					}
					if(m_nBFCPTransportType==e_BFCP_Transport_None)
					{
						LogFile(NULL,NULL,"","","application BFCP SDP Not Add");
					}

					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=floorctrl:%s\r\n",m_strBFCPFloorControl);
					if(m_nBFCPConfId >0) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=confid:%d\r\n",m_nBFCPConfId);
					if(m_nBFCPFloorId != 0) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=floorid:%d mstrm:%d\r\n",m_nBFCPFloorId,m_nBFCPDualVideoLabel);
					if(m_nBFCPUserId >0) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=userid:%d\r\n",m_nBFCPUserId);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=setup:%s\r\n",m_strBFCPSetupType);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=connection:new\r\n");
				}
			}

			if(m_H239VideoCap.m_nCapNum<=0) continue;
		}

		//{{gsh 设置视频及辅流能力集代码相同 20120206
		media_len=0;
		media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len,"m=video %d RTP/AVP",nMediaPort);

		for(i=0; i<MediaCap.m_nCapNum; i++)
		{
			VIDEO_CAPABILITY_ITEM& capVideo = MediaCap.m_Caps[i];
			switch( capVideo.VideoCodecType )
			{ 
			case VIDEC_CODEC_H261:
				media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",31);
				break;
			case VIDEC_CODEC_H263:
				media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",34);
				break;
			case VIDEC_CODEC_H263P:
				media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",nH263PPT);
				break;
			case VIDEC_CODEC_H263PP:
				media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",nH263PPPT);
				break;
			case VIDEC_CODEC_H264:
				if(!MediaH264RtpMap)
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",nH264PT);
					MediaH264RtpMap=!MediaH264RtpMap;
				}
				break;
			case VIDEC_CODEC_H264_SVC:
				media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",nH264SVCPT);
				break;
			default:
				break;
			}
		}
		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",media);
		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"b=AS:%d\r\n",nMediaBandwidth);
		if(m_bRtcp)	buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=rtcp:%d IN IP4 %s\r\n",nMediaRTCPPort,m_strLocalIP);

		MediaH264RtpMap=false;
		for(i=0; i<MediaCap.m_nCapNum; i++)
		{
			int nHasQCIF = 0;
			int nHasCIF = 0;
			int nHas4CIF = 0;
			//int nVideoRate = 384;
			int nH264Profile=0x42;
			int nH264Payload=nH264PT;
			int nVideoRate = 0;
			int nMaxMBPS =0;
			int nMaxFS =0;
			int nLevel =0;

			VIDEO_CAPABILITY_ITEM& capVideo = MediaCap.m_Caps[i];
			nHasQCIF = capVideo.nQCIF;
			nHasCIF = capVideo.nCIF;
			nHas4CIF = capVideo.n4CIF;
			//nVideoRate = capVideo.nMaxBitrate;
			nVideoRate = 90000;

			int n4CIFValue                    = 1;
			int n4cifFormatValue              = GetVideoFormatValue(VIDEO_FORMAT_4CIF);
			VIDEC_CODEC_TYPE VideoCodecType   = capVideo.VideoCodecType;
			VIDEO_FORMAT_TYPE VideoFormatType = capVideo.VideoFormatType;
			int nFormatValue                  = capVideo.nFormatValue;
			int nVideoFrameRate               = capVideo.nVideoFrameRate;

			switch( capVideo.VideoCodecType )
			{ 
			case VIDEC_CODEC_H261:
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(nHasCIF >0) nHasQCIF=1;
				attr_len=0;
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d H261/%d\r\n",31,nVideoRate);
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d",31);
				if(nHasCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF=%d",nHasCIF);
				if(nHasQCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," QCIF=%d",nHasQCIF);
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," maxbr=%d",nMediaBandwidth);

				//attr[attr_len-1]='\0'; //;去之
				buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",attr);
				break;				
			case VIDEC_CODEC_H263:
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_4CIF) nHas4CIF =n4CIFValue;
				if(nHas4CIF >0) nHasCIF=1;
				if(nHasCIF >0) nHasQCIF=1;

				attr_len=0;
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d H263/%d\r\n",34,nVideoRate);

				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d",34);
				if(nHas4CIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," XGA=1");
				if(nHas4CIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF4=%d",nHas4CIF);
				if(nHasCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF=%d",nHasCIF);
				if(nHasQCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," QCIF=%d",nHasQCIF);

				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," maxbr=%d",nMediaBandwidth);

				//attr[attr_len-1]='\0'; //;去之
				buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",attr);
				break;

			case VIDEC_CODEC_H263P:
				if(nFormatValue >n4cifFormatValue) nHas4CIF = n4CIFValue; //VIDEO_FORMAT_4CIF 3
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_4CIF) nHas4CIF =n4CIFValue;
				if(nHas4CIF >0) nHasCIF=1;
				if(nHasCIF >0) nHasQCIF=1;

				attr_len=0;
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d H263-1998/%d\r\n",nH263PPT,nVideoRate);
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d",nH263PPT);	

				SetH263CustomStr2(VideoFormatType,attr,attr_len,MAX_ATTR_STR_LEN);
				if(nHas4CIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF4=%d",nHas4CIF);
				if(nHasCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF=%d",nHasCIF);
				if(nHasQCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," QCIF=%d",nHasQCIF);

				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," maxbr=%d",nMediaBandwidth);

				buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",attr);
				break;

			case VIDEC_CODEC_H263PP:
				if(nFormatValue >n4cifFormatValue) nHas4CIF = n4CIFValue; //VIDEO_FORMAT_4CIF 3
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_4CIF) nHas4CIF =n4CIFValue;
				if(nHas4CIF >0) nHasCIF=1;
				if(nHasCIF >0) nHasQCIF=1;

				attr_len=0;
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d H263-2000/%d\r\n",nH263PPPT,nVideoRate);
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d",nH263PPPT);

				SetH263CustomStr2(VideoFormatType,attr,attr_len,MAX_ATTR_STR_LEN);

				if(nHas4CIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF4=%d",nHas4CIF);
				if(nHasCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF=%d",nHasCIF);
				if(nHasQCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," QCIF=%d",nHasQCIF);

				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," maxbr=%d",nMediaBandwidth);

				buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",attr);
				break;
			case VIDEC_CODEC_H264_SVC:
			case VIDEC_CODEC_H264:
				if( capVideo.VideoCodecType == VIDEC_CODEC_H264_SVC ) 
				{
					nH264Profile=0x64; //notice wait find value
					nH264Payload=nH264SVCPT;
				}
				if( (capVideo.VideoCodecType == VIDEC_CODEC_H264) && (MediaH264RtpMap) ) break;
				if( capVideo.VideoCodecType == VIDEC_CODEC_H264) MediaH264RtpMap=!MediaH264RtpMap;

				GetVideoSIPH264MaxFSMBPS(nMaxMBPS,nMaxFS,nLevel,nVideoFrameRate,VideoFormatType);

				attr_len=0;
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d H264/%d\r\n",nH264Payload,nVideoRate);
				buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);

				attr_len=0;
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d",nH264Payload);
				if(nLevel >0 && nLevel<16)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," profile-level-id=%x%x0%x",nH264Profile,0x80,nLevel);
				if(nLevel >=16)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," profile-level-id=%x%x%x",nH264Profile,0x80,nLevel);

				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"%smax-br=%d",paramToken,nMediaBandwidth);

				if(nHasQCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"%smax-mbps=%d",paramToken,nMaxMBPS);
				if(nHasCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"%smax-fs=%d",paramToken,nMaxFS);

				////Media Attribute (a): fmtp:107 profile-level-id=42801F;max-br=1536;max-mbps=115000;max-fs=3840;packetization-mode=1
				//attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"%s packetization-mode=1",paramToken);

				//attr[attr_len-1]='\0'; //;去之
				buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",attr);
				break;
			default:
				break;
			}
		}

		//}}gsh 设置视频及辅流能力集代码相同 20120206

		//video
		if(mediaCapNum==1)
		{
			buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=sendrecv\r\n");
			if(m_bProxy) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=nortpproxy:yes\r\n");
			if(m_bUseFECEnable ) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=reliablertp:fec-enable\r\n");
		}
		//Dual video
		if(mediaCapNum==2)
		{
			buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=sendrecv\r\n");					
			if(m_bProxy) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=nortpproxy:yes\r\n");
			if(m_bUseFECEnable ) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=reliablertp:fec-enable\r\n");

			buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=content:slides\r\n");
			buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=label:%d\r\n",m_nBFCPDualVideoLabel);
		}
	}

	//h224
	if(m_DataCap.m_nCapNum>0) 
	{
		media_len=0;
		media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len,"m=application %d RTP/AVP",m_H224Port);

		for(i=0; i<m_DataCap.m_nCapNum; i++)
		{
			switch( m_DataCap.m_Caps[i].DataCodecID )
			{
			case DATA_CODEC_H224:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",m_H224PT);
				}
				break;
			default:
				break;
			}
		}
		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",media);
		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"b=AS:%d\r\n",480);
		if(m_bRtcp)
			buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=rtcp:%d IN IP4 %s\r\n",h224RTCPPort,m_strLocalIP);

		for(i=0; i<m_DataCap.m_nCapNum; i++)
		{
			switch( m_DataCap.m_Caps[i].DataCodecID )
			{
			case DATA_CODEC_H224:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d H224/4800\r\n",m_H224PT);
					//attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=480\r\n",m_H224PT);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);		
				}
				break;
			default:
				break;
			}
		}
		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=sendrecv\r\n");
		if(m_bUseFECEnable ) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=reliablertp:fec-enable\r\n");
	}

	if(copyOutBuf)
	{
		strcpy(copyOutBuf,buf);
		return 0;
	}

#ifdef HAS_SIP

	//status=pjmedia_sdp_parse_build(pool,buf,strlen(buf),sdp);
	status=pjmedia_sdp_parse(pool,buf,strlen(buf),&sdp);
	if (status != PJ_SUCCESS) 
	{
		return -1;
	}

	*p_sdp = pjmedia_sdp_session_clone(pool,sdp);

	if(*p_sdp==NULL) return -1;

#endif

	return 0;
}

int SIPCallCapability::InitCapabilitis(int node)
	{
	return 1;
	}

int SIPCallCapability::OperationVideoCap(eCapNum capIndex,int operation,BOOL hasQCIF,BOOL hasCIF,BOOL has4CIF)                         //添加H264能力 CIF  
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
	case CAP_H264_QCIF:
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_QCIF;	
		capability.n4CIF = H264_SDP_LEVEL_QCIF;  //H264此用于LEVEL
		capability.nQCIF = H264_MacroblockPS_QCIF;
		capability.nCIF = H264_SDP_MaxFS_QCIF;
		capability.VideoFormatType =VIDEO_FORMAT_QCIF;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_CIF:
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_CIF;	  
		capability.n4CIF = H264_SDP_LEVEL_CIF;    //H264此用于LEVEL
		capability.nQCIF = H264_MacroblockPS_CIF;
		capability.nCIF = H264_SDP_MaxFS_CIF;
		capability.VideoFormatType =VIDEO_FORMAT_CIF;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_4CIF: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_4CIF;	
		capability.n4CIF = H264_SDP_LEVEL_4CIF;
		capability.nQCIF = H264_MacroblockPS_4CIF;
		capability.nCIF = H264_SDP_MaxFS_4CIF;
		capability.VideoFormatType =VIDEO_FORMAT_4CIF;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_16CIF: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_16CIF;	
		capability.n4CIF = H264_SDP_LEVEL_16CIF;
		capability.nQCIF = H264_MacroblockPS_16CIF;
		capability.nCIF = H264_SDP_MaxFS_16CIF;
		capability.VideoFormatType =VIDEO_FORMAT_16CIF;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_720I: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_720I;	
		capability.n4CIF = H264_SDP_LEVEL_720I;
		capability.nQCIF = H264_MacroblockPS_720I;
		capability.nCIF = H264_SDP_MaxFS_720I;
		capability.VideoFormatType =VIDEO_FORMAT_720I;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_720P: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_720P;	
		capability.n4CIF = H264_SDP_LEVEL_720P;
		capability.nQCIF = H264_MacroblockPS_720P;
		capability.nCIF = H264_SDP_MaxFS_720P;
		capability.VideoFormatType =VIDEO_FORMAT_720P;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_1080I: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_1080I;	
		capability.n4CIF = H264_SDP_LEVEL_1080I;
		capability.nQCIF = H264_MacroblockPS_1080I;
		capability.nCIF = H264_SDP_MaxFS_1080I;
		capability.VideoFormatType =VIDEO_FORMAT_1080I;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_1080P: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_1080P;	
		capability.n4CIF = H264_SDP_LEVEL_1080P;
		capability.nQCIF = H264_MacroblockPS_1080P;
		capability.nCIF = H264_SDP_MaxFS_1080P;
		capability.VideoFormatType =VIDEO_FORMAT_1080P;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_VGA: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_VGA;	
		capability.n4CIF = H264_SDP_LEVEL_VGA;
		capability.nQCIF = H264_MacroblockPS_VGA;
		capability.nCIF = H264_SDP_MaxFS_VGA;
		capability.VideoFormatType =VIDEO_FORMAT_VGA;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_SVGA:
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_SVGA;	
		capability.n4CIF = H264_SDP_LEVEL_SVGA;
		capability.nQCIF = H264_MacroblockPS_SVGA;
		capability.nCIF = H264_SDP_MaxFS_SVGA;
		capability.VideoFormatType =VIDEO_FORMAT_SVGA;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_XGA: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_XGA;	
		capability.n4CIF = H264_SDP_LEVEL_XGA;
		capability.nQCIF = H264_MacroblockPS_XGA;
		capability.nCIF = H264_SDP_MaxFS_XGA;
		capability.VideoFormatType =VIDEO_FORMAT_XGA;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_SXGA: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_SXGA;	
		capability.n4CIF = H264_SDP_LEVEL_SXGA;
		capability.nQCIF = H264_MacroblockPS_SXGA;
		capability.nCIF = H264_SDP_MaxFS_SXGA;
		capability.VideoFormatType =VIDEO_FORMAT_SXGA;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_HIG720P: 
		capability.VideoCodecType = VIDEC_CODEC_H264_SVC;
		capability.nMaxBitrate = H264_MBPS_720P/2;	
		capability.n4CIF = H264_SDP_LEVEL_720P;
		capability.nQCIF = H264_MacroblockPS_720P;
		capability.nCIF = H264_SDP_MaxFS_720P;
		capability.VideoFormatType =VIDEO_FORMAT_720P;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H264_HIG1080P: 
		capability.VideoCodecType = VIDEC_CODEC_H264_SVC;
		capability.nMaxBitrate = H264_MBPS_1080P/2;	
		capability.n4CIF = H264_SDP_LEVEL_1080P;
		capability.nQCIF = H264_MacroblockPS_1080P;
		capability.nCIF = H264_SDP_MaxFS_1080P;
		capability.VideoFormatType =VIDEO_FORMAT_1080P;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H263: 
		capability.VideoCodecType = VIDEC_CODEC_H263;
		capability.nMaxBitrate = H263_MBPS_ALL;
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		capability.n4CIF = nHas4CIF;
		if(nHas4CIF >0) capability.VideoFormatType =VIDEO_FORMAT_4CIF;
		else if(nHasCIF >0) capability.VideoFormatType =VIDEO_FORMAT_CIF;
		else if(nHasQCIF >0) capability.VideoFormatType =VIDEO_FORMAT_QCIF;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H263v2: 
		capability.VideoCodecType = VIDEC_CODEC_H263P;
		capability.nMaxBitrate = H263_MBPS_ALL;
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		capability.n4CIF = nHas4CIF;
		// ?? wait
		if(nHas4CIF >0) capability.VideoFormatType =VIDEO_FORMAT_4CIF;
		else if(nHasCIF >0) capability.VideoFormatType =VIDEO_FORMAT_CIF;
		else if(nHasQCIF >0) capability.VideoFormatType =VIDEO_FORMAT_QCIF;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H263v3: 
		capability.VideoCodecType = VIDEC_CODEC_H263PP;
		capability.nMaxBitrate = H263_MBPS_ALL;
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		capability.n4CIF = nHas4CIF;
		// ?? wait
		if(nHas4CIF >0) capability.VideoFormatType =VIDEO_FORMAT_4CIF;
		else if(nHasCIF >0) capability.VideoFormatType =VIDEO_FORMAT_CIF;
		else if(nHasQCIF >0) capability.VideoFormatType =VIDEO_FORMAT_QCIF;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	case CAP_H261: 
		capability.VideoCodecType = VIDEC_CODEC_H261;
		capability.nMaxBitrate = H263_MBPS_ALL;
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		if(nHasCIF >0) capability.VideoFormatType =VIDEO_FORMAT_CIF;
		else if(nHasQCIF >0) capability.VideoFormatType =VIDEO_FORMAT_QCIF;
		capability.nFormatValue = GetVideoFormatValue(capability.VideoFormatType);
		break;
	default:break;
	}

	//add set frame param
	if(capability.VideoCodecType ==VIDEC_CODEC_H264 || capability.VideoCodecType ==VIDEC_CODEC_H264_SVC )
	{
		GetVideoFormatTypeFromCapIndex(2,capIndex,capability);//mainvideo is type 2 ,dualvideo is type 3
	}

	if(operation == 1) result=AddVideoCap(capability);  //添加
	if(operation == 2) result=SetVideoCap(1,capability); //首选
	if(operation == 3) result=SetVideoCap(2,capability); //移除

	return result; 
} 

//类同 OperationVideoCap
int SIPCallCapability::OperationH239Cap(eCapNum capIndex,int operation,BOOL hasQCIF,BOOL hasCIF,BOOL has4CIF)                         //添加H264能力 CIF  
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
		capability.n4CIF = H264_SDP_LEVEL_QCIF;  //H264此用于LEVEL
		capability.nQCIF = H264_MacroblockPS_QCIF;
		capability.nCIF = H264_SDP_MaxFS_QCIF;
		break; 
	case CAP_H264EXTEND_CIF:
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_CIF;	  
		capability.n4CIF = H264_SDP_LEVEL_CIF;    //H264此用于LEVEL
		capability.nQCIF = H264_MacroblockPS_CIF;
		capability.nCIF = H264_SDP_MaxFS_CIF; 
		break; 
	case CAP_H264EXTEND_4CIF: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_4CIF;	
		capability.n4CIF = H264_SDP_LEVEL_4CIF;
		capability.nQCIF = H264_MacroblockPS_4CIF;
		capability.nCIF = H264_SDP_MaxFS_4CIF;
		break; 
	case CAP_H264EXTEND_16CIF: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_16CIF;	
		capability.n4CIF = H264_SDP_LEVEL_16CIF;
		capability.nQCIF = H264_MacroblockPS_16CIF;
		capability.nCIF = H264_SDP_MaxFS_16CIF;
		break; 
	case CAP_H264EXTEND_720I: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_720I;	
		capability.n4CIF = H264_SDP_LEVEL_720I;
		capability.nQCIF = H264_MacroblockPS_720I;
		capability.nCIF = H264_SDP_MaxFS_720I;
		break; 
	case CAP_H264EXTEND_720P: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_720P;	
		capability.n4CIF = H264_SDP_LEVEL_720P;
		capability.nQCIF = H264_MacroblockPS_720P;
		capability.nCIF = H264_SDP_MaxFS_720P;
		break; 
	case CAP_H264EXTEND_1080I: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_1080I;	
		capability.n4CIF = H264_SDP_LEVEL_1080I;
		capability.nQCIF = H264_MacroblockPS_1080I;
		capability.nCIF = H264_SDP_MaxFS_1080I;
		break; 
	case CAP_H264EXTEND_1080P: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_1080P;	
		capability.n4CIF = H264_SDP_LEVEL_1080P;
		capability.nQCIF = H264_MacroblockPS_1080P;
		capability.nCIF = H264_SDP_MaxFS_1080P;
		break;
	case CAP_H264EXTEND_VGA: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_VGA;	
		capability.n4CIF = H264_SDP_LEVEL_VGA;
		capability.nQCIF = H264_MacroblockPS_VGA;
		capability.nCIF = H264_SDP_MaxFS_VGA;
		break; 
	case CAP_H264EXTEND_SVGA:
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_SVGA;	
		capability.n4CIF = H264_SDP_LEVEL_SVGA;
		capability.nQCIF = H264_MacroblockPS_SVGA;
		capability.nCIF = H264_SDP_MaxFS_SVGA;
		break; 
	case CAP_H264EXTEND_XGA: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_XGA;	
		capability.n4CIF = H264_SDP_LEVEL_XGA;
		capability.nQCIF = H264_MacroblockPS_XGA;
		capability.nCIF = H264_SDP_MaxFS_XGA;
		break; 
	case CAP_H264EXTEND_SXGA: 
		capability.VideoCodecType = VIDEC_CODEC_H264;
		capability.nMaxBitrate = H264_MBPS_SXGA;	
		capability.n4CIF = H264_SDP_LEVEL_SXGA;
		capability.nQCIF = H264_MacroblockPS_SXGA;
		capability.nCIF = H264_SDP_MaxFS_SXGA;
		break;
	case CAP_H264EXTEND_HIG720P: 
		capability.VideoCodecType = VIDEC_CODEC_H264_SVC;
		capability.nMaxBitrate = H264_MBPS_720P/2;	
		capability.n4CIF = H264_SDP_LEVEL_720P;
		capability.nQCIF = H264_MacroblockPS_720P;
		capability.nCIF = H264_SDP_MaxFS_720P;
		break;
	case CAP_H264EXTEND_HIG1080P: 
		capability.VideoCodecType = VIDEC_CODEC_H264_SVC;
		capability.nMaxBitrate = H264_MBPS_1080P/2;	
		capability.n4CIF = H264_SDP_LEVEL_1080P;
		capability.nQCIF = H264_MacroblockPS_1080P;
		capability.nCIF = H264_SDP_MaxFS_1080P;
		break;
	case CAP_H263EXTEND: 
		capability.VideoCodecType = VIDEC_CODEC_H263;
		capability.nMaxBitrate = H263_MBPS_ALL;
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		capability.n4CIF = nHas4CIF;
		break; 
	case CAP_H263v2EXTEND: 
		capability.VideoCodecType = VIDEC_CODEC_H263P;
		capability.nMaxBitrate = H263_MBPS_ALL;
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		capability.n4CIF = nHas4CIF;
		break; 
	case CAP_H263v3EXTEND: 
		capability.VideoCodecType = VIDEC_CODEC_H263PP;
		capability.nMaxBitrate = H263_MBPS_ALL;
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		capability.n4CIF = nHas4CIF;
		break; 
	case CAP_H261EXTEND: 
		capability.VideoCodecType = VIDEC_CODEC_H261;
		capability.nMaxBitrate = H263_MBPS_ALL;
		capability.nQCIF = nHasQCIF;
		capability.nCIF = nHasCIF;
		break;
	default:break;
	}

	//add set frame param
	if(capability.VideoCodecType ==VIDEC_CODEC_H264 || capability.VideoCodecType ==VIDEC_CODEC_H264_SVC )
	{
		GetVideoFormatTypeFromCapIndex(3,capIndex,capability);//mainvideo is type 2 ,dualvideo is type 3
	}

	if(operation == 1) result=AddH239Cap(capability);  //添加
	if(operation == 2) result=SetH239Cap(1,capability); //首选
	if(operation == 3) result=SetH239Cap(2,capability); //移除

	return result; 
} 

void SIPCallCapability::GetVideoFormatTypeFromCapIndex(int type,eCapNum capIndex,VIDEO_CAPABILITY_ITEM  & cap)
{
	float fScale=1.0;
	float fMaxMBPS=cap.nQCIF;
	float fMaxFrame=30.0;
	float fFrameRate=0;
	//terminal or mp use
	if(type ==2) fFrameRate=m_nMainVideoFrameRate;
	else if(type ==3) fFrameRate=m_nSecondVideoFrameRate;

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	int nFrame=0;
	int nBitrate =0;
	VIDEO_FORMAT_TYPE nFormatType=GetVideoFormatTypeFromCapIndex(capIndex);
	GetVideoSetOtherParam(type,cap.VideoCodecType,nFormatType,nFrame,nBitrate);
	//modify 20140707
	GetH264SVCVideoParam(type,cap.VideoCodecType,nFrame,cap.n4CIF,cap.nMaxBitrate);

	if(nFrame >0) fFrameRate=nFrame;
#endif

	fScale=fFrameRate/fMaxFrame;
	fMaxMBPS=fMaxMBPS * fScale;
	cap.nQCIF=fMaxMBPS;
}

VIDEO_FORMAT_TYPE SIPCallCapability::GetVideoFormatTypeFromCapIndex(eCapNum capIndex)
{
	VIDEO_FORMAT_TYPE nFormatType =VIDEO_FORMAT_QCIF;
	//get format
	switch(capIndex)
	{
	case CAP_H264_QCIF:
		nFormatType=VIDEO_FORMAT_QCIF;
		break;
	case CAP_H264_CIF:
		nFormatType=VIDEO_FORMAT_CIF;
		break;
	case CAP_H264_4CIF:
		nFormatType=VIDEO_FORMAT_4CIF;
		break;
	case CAP_H264_16CIF:
		nFormatType=VIDEO_FORMAT_16CIF;
		break;
	case CAP_H264_720P:
	case CAP_H264_HIG720P:
		nFormatType=VIDEO_FORMAT_720P;
		break;
	case CAP_H264_720I:
		nFormatType=VIDEO_FORMAT_720I;
		break;
	case CAP_H264_1080P:
	case CAP_H264_HIG1080P:
		nFormatType=VIDEO_FORMAT_1080P;
		break;
	case CAP_H264_1080I:
		nFormatType=VIDEO_FORMAT_1080I;
		break;
	case CAP_H264_VGA:
		nFormatType=VIDEO_FORMAT_VGA;
		break;
	case CAP_H264_SVGA:
		nFormatType=VIDEO_FORMAT_SVGA;
		break;
	case CAP_H264_XGA:
		nFormatType=VIDEO_FORMAT_XGA;
		break;
	case CAP_H264_SXGA:
		nFormatType=VIDEO_FORMAT_SXGA;
		break;
	}

	return nFormatType;
}

void SIPCallCapability::SetRtpFECEnableAttribute(bool bUseFECEnable)
{
	m_bUseFECEnable=bUseFECEnable;
}

//mcu use
void SIPCallCapability::GetH264SVCVideoParam(int type,VIDEC_CODEC_TYPE codecType,int & frame,int & nLevel,int & nBitrate)
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
				nLevel =H264_SDP_LEVEL_QCIF; 
				break;
			case VIDEO_FORMAT_CIF:
				nLevel =H264_SDP_LEVEL_CIF; 
				break;
			case VIDEO_FORMAT_4CIF:
				nLevel =H264_SDP_LEVEL_4CIF; 
				break;
			case VIDEO_FORMAT_16CIF:
				nLevel =H264_SDP_LEVEL_16CIF; 
				break;
			case VIDEO_FORMAT_720I:
				nLevel =H264_SDP_LEVEL_720I; 
				break;
			case VIDEO_FORMAT_720P:
				nLevel =H264_SDP_LEVEL_720P; 
				break;
			case VIDEO_FORMAT_1080I:
				nLevel =H264_SDP_LEVEL_1080I; 
				break;
			case VIDEO_FORMAT_1080P:
				nLevel =H264_SDP_LEVEL_1080P; 
				break;
			case VIDEO_FORMAT_VGA:
				nLevel =H264_SDP_LEVEL_VGA; 
				break;
			case VIDEO_FORMAT_SVGA:
				nLevel =H264_SDP_LEVEL_SVGA; 
				break;
			case VIDEO_FORMAT_XGA:
				nLevel =H264_SDP_LEVEL_XGA; 
				break;
			case VIDEO_FORMAT_SXGA:
				nLevel =H264_SDP_LEVEL_SXGA; 
				break;
			default :
				nLevel =H264_SDP_LEVEL_720P; 
				break;
			}
		}
	}
}

//int BuildMediaCapabilitis(int node,DapCallCapability & DapCap);
int SIPCallCapability::BuildMediaCapabilitisTest(pj_pool_t *pool,DapCallCapability & DapCap,pjmedia_sdp_session **p_sdp)
{
	//fmtp:96 CIF4=2;CIF=1;QCIF=1;SQCIF=1;F;J;T
	//fmtp:106 profile-level-id=42800d; max-mbps=20000; max-fs=1792
	//profile-level-id:这个参数用于指示H.264流的profile类型和级别. 由Base16(十六进制)表示的3个字节. 
	//第一个字节表示H.264的Profile类型, 第三个字节表示H.264的Profile级别.

	int  nUseDualVideoToken = 0; //use token interactive is 1,else is 0
	int  nSDPOrignIPConverse = 0;	 //SDP orign ip is converse ,not converse is 0 ,converse is 1

#ifdef HAS_SIP

	nSDPOrignIPConverse = SIPCallMgr::Instance().m_nSDPOrignIPConverse;
	nUseDualVideoToken = SIPCallMgr::Instance().m_nUseDualVideoToken;

#endif

	unsigned int i=0;
	char buf[4096]={0};
	int buf_len=0;
	int MAX_BUF_STR_LEN = 4096;
	char media[256]={0};		
	int media_len=0;
	int MAX_MEDIA_STR_LEN = 256 ;
	char ICEAttrStr[2000]={0}; //用于ICE
	int videoRTCPPort=m_VideoPort+1;
	char * paramToken=";";//modify 20140703 hdx7000 and huawei te30 use ;

	//Video SDP	//SDP每一行都以回车换行结束
	pj_status_t status=-1;
	pjmedia_sdp_session *sdp=NULL;


	//注意 音频SDP 必须有
	int nCapCount = DapCap.m_nAudioCapCount;
	if(nCapCount <=0) return -1;


	pj_time_val tv;
	pj_gettimeofday(&tv);
	unsigned int vesion=tv.sec + 2208988800UL;

	char sdpHeader[512]={0};
	int sdpHeaderLen=0;
	int MAX_SDPHR_STR_LEN = 512 ;
	char attr[512]={0};
	int attr_len=0;
	int MAX_ATTR_STR_LEN = 512 ;
	int audioRTCPPort=m_AudioPort+1;
	int h224RTCPPort=m_H224Port+1;
	int mediaCapNum=0;


	char strLocalConverseIP[128]={"0.0.0.0"};
	int nMaxConverseLen=128;
	ConverseIP(strLocalConverseIP,nMaxConverseLen,m_strLocalIP);
	if( nSDPOrignIPConverse <= 0 )
	{
		strcpy(strLocalConverseIP,m_strLocalIP);
	}

	sdpHeaderLen+=snprintf(&sdpHeader[sdpHeaderLen],MAX_SDPHR_STR_LEN-sdpHeaderLen,
		"v=0\r\no=%s %u %u IN IP4 %s\r\ns=%s\r\nc=IN IP4 %s\r\nt=0 0\r\n",m_strOwnerUsername,vesion,vesion,strLocalConverseIP,m_strOwnerUsername,m_strLocalIP);
	buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",sdpHeader);

	//Session Attribute (a): X-nat:0 Unknown
	//buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=X-nat:0 Unknown\r\n");

	//audio
	if(nCapCount>0) 
	{
		media_len=0;
		media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len,"m=audio %d RTP/AVP",m_AudioPort);

		for(i=0; i< nCapCount; i++)
		{
			switch( DapCap.m_aAudioCap[i].m_AudioCodecType )
			{
			case X_AUDIO_CODEC_G711A:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",8);
				}
				break; 
			case X_AUDIO_CODEC_G711U:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",0);
				}
				break;
			case X_AUDIO_CODEC_G722_64:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",9);
				}
				break;
			case X_AUDIO_CODEC_G7221_7_24:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",m_G7221_24k_PT);
				}
				break;
			case X_AUDIO_CODEC_G7221_7_32:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",m_G7221_32k_PT);
				}
				break;
			case X_AUDIO_CODEC_G7221_14_24:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",m_G7221C_24k_PT);
				}
				break;
			case X_AUDIO_CODEC_G7221_14_32:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",m_G7221C_32k_PT);
				}
				break;

			case X_AUDIO_CODEC_G7221_14_48:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",m_G7221C_48k_PT);
				}
				break;		
			case X_AUDIO_CODEC_G729:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",18);
				}
				break;
			case X_AUDIO_CODEC_GSM:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",3);
				}
				break;
			default:
				break;
			}
		}

		media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",101);

		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",media);
		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"b=AS:%d\r\n",m_nAudioBandwidth);

		if(m_bRtcp)
			buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=rtcp:%d IN IP4 %s\r\n",audioRTCPPort,m_strLocalIP);

		for(i=0; i< nCapCount; i++)
		{
			switch( DapCap.m_aAudioCap[i].m_AudioCodecType )
			{
			case X_AUDIO_CODEC_G711A:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:8 PCMA/8000\r\n");
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);
				}
				break; 
			case X_AUDIO_CODEC_G711U:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:0 PCMU/8000\r\n");
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);
				}
				break;
			case X_AUDIO_CODEC_G722_64:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:9 G722/16000\r\n");
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=64000\r\n",9);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);
				}
				break;
			case X_AUDIO_CODEC_G7221_7_24:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d G7221/16000\r\n",m_G7221_24k_PT);
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=24000\r\n",m_G7221_24k_PT);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);		
				}
				break;
			case X_AUDIO_CODEC_G7221_7_32:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d G7221/16000\r\n",m_G7221_32k_PT);
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=32000\r\n",m_G7221_32k_PT);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);		
				}
				break;

			case X_AUDIO_CODEC_G7221_14_24:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d G7221/32000\r\n",m_G7221C_24k_PT);
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=24000\r\n",m_G7221C_24k_PT);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);		
				}
				break;
			case X_AUDIO_CODEC_G7221_14_32:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d G7221/32000\r\n",m_G7221C_32k_PT);
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=32000\r\n",m_G7221C_32k_PT);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);		
				}
				break;
			case X_AUDIO_CODEC_G7221_14_48:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d G7221/32000\r\n",m_G7221C_48k_PT);
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=32000\r\n",m_G7221C_48k_PT);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);		
				}
				break;		
			case X_AUDIO_CODEC_G729:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:18 G729/8000\r\na=fmtp:18 annexb=no\r\n");
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);
				}
				break;
			case X_AUDIO_CODEC_GSM:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:3 GSM/8000\r\n");
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);
				}
				break;
			default:
				break;
			}

		}

		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=rtpmap:101 telephone-event/8000\r\na=fmtp:101 0-16\r\na=sendrecv\r\n");
		if(m_bUseFECEnable ) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=reliablertp:fec-enable\r\n");

	}


	if(DapCap.m_nMainVideoCapCount >0 ) LogFile(NULL,NULL,"","","VideoCap CapNum >0");
	if(DapCap.m_nSecondVideoCapCount >0) LogFile(NULL,NULL,"","","DualVideoCap CapNum >0");


	//video dual video
	for(mediaCapNum=1;mediaCapNum<=2;mediaCapNum++)
	{
		int nMediaPort=0;
		int nMediaRTCPPort=0;
		int nMediaBandwidth=0;
		int nH264PT=0;
		int nH264SVCPT=0;
		int nH263PPT=0;
		int nH263PPPT=0;
		EH263ResolutionOption h263POption=e_H263_Resolution_None;
		EH263ResolutionOption h263PPOption=e_H263_Resolution_None;

		bool MediaH264RtpMap=false;
		VideoCapability MediaCap[MAX_VIDEO_CAP];
		//VideoCap MediaCap;
		memset(&MediaCap,0,sizeof(MediaCap));
		int nMidiaCapCount=0;
		if(mediaCapNum==1)
		{
			nMidiaCapCount = DapCap.m_nMainVideoCapCount;
		}
		if(mediaCapNum==2)
		{
			nMidiaCapCount = DapCap.m_nSecondVideoCapCount;
		}

		//video
		if(mediaCapNum==1)
		{
			if(DapCap.m_nMainVideoCapCount <=0) continue;
			else
			{
				//此块变量为用于设置视频及辅流能力集代码相同而定义
				nMediaPort=m_VideoPort;
				nMediaRTCPPort=m_VideoPort+1;
				nMediaBandwidth=m_nVideoBandwidth;
				nH264PT=m_H264PT;
				nH264SVCPT=m_H264SVCPT;
				nH263PPT=m_H263PPT;
				nH263PPPT=m_H263PPPT;
				h263POption=m_H263PResolutionOption;
				h263PPOption=m_H263PPResolutionOption;
				memcpy(MediaCap,DapCap.m_aMainVideoCap,sizeof(MediaCap));
			}
		}

		//Dual video
		if(mediaCapNum==2)
		{
			if(DapCap.m_nSecondVideoCapCount >0)
			{
				//此块变量为用于设置视频及辅流能力集代码相同而定义
				nMediaPort=m_DualVideoPort;
				nMediaRTCPPort=m_DualVideoPort+1;
				nMediaBandwidth=m_nDualVideoBandwidth;
				nH264PT=m_DualH264PT;
				nH264SVCPT=m_DualH264SVCPT;
				nH263PPT=m_DualH263PPT;
				nH263PPPT=m_DualH263PPPT;
				h263POption=m_DualH263PResolutionOption;
				h263PPOption=m_DualH263PPResolutionOption;

				//memcpy(&MediaCap,&m_H239VideoCap,sizeof(H239VideoCap));
				memcpy(MediaCap,DapCap.m_aSecondVideoCap,sizeof(MediaCap));

				if(DapCap.m_nMainVideoCapCount >0)
				{
					if( nUseDualVideoToken>0)
						buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=content:main\r\n");
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=label:%d\r\n",m_nBFCPDualVideoLabel-1); //-2
					if( nUseDualVideoToken>0)
						buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=answer:full\r\n");
				}
			}

			if( nUseDualVideoToken>0)
			{
				if(DapCap.m_nMainVideoCapCount >0 || DapCap.m_nSecondVideoCapCount >0)
				{
					if(m_nBFCPTransportType==e_BFCP_Transport_UDP)
					{
						buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"m=application %d UDP/BFCP *\r\n",m_nBFCPTransportPort);
						LogFile(NULL,NULL,"","","application UDP/BFCP SDP Add");
					}
					if(m_nBFCPTransportType==e_BFCP_Transport_TCP)
					{
						buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"m=application %d TCP/BFCP *\r\n",m_nBFCPTransportPort);
						LogFile(NULL,NULL,"","","application TCP/BFCP SDP Add");
					}
					if(m_nBFCPTransportType==e_BFCP_Transport_None)
					{
						LogFile(NULL,NULL,"","","application BFCP SDP Not Add");
					}

					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=floorctrl:%s\r\n",m_strBFCPFloorControl);
					if(m_nBFCPConfId >0) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=confid:%d\r\n",m_nBFCPConfId);
					if(m_nBFCPFloorId != 0) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=floorid:%d mstrm:%d\r\n",m_nBFCPFloorId,m_nBFCPDualVideoLabel);
					if(m_nBFCPUserId >0) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=userid:%d\r\n",m_nBFCPUserId);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=setup:%s\r\n",m_strBFCPSetupType);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=connection:new\r\n");
				}
			}

			if(DapCap.m_nSecondVideoCapCount <=0) continue;
		}

		//{{gsh 设置视频及辅流能力集代码相同 20120206
		media_len=0;
		media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len,"m=video %d RTP/AVP",nMediaPort);

		for(i=0; i< nMidiaCapCount; i++)
		{
			VideoCapability & capVideo = MediaCap[i];
			switch( capVideo.m_VideoCodecType )
			{ 
			case VIDEC_CODEC_H261:
				media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",31);
				break;
			case VIDEC_CODEC_H263:
				media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",34);
				break;
			case VIDEC_CODEC_H263P:
				media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",nH263PPT);
				break;
			case VIDEC_CODEC_H263PP:
				media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",nH263PPPT);
				break;
			case VIDEC_CODEC_H264:
				if(!MediaH264RtpMap)
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",nH264PT);
					MediaH264RtpMap=!MediaH264RtpMap;
				}
				break;
			case VIDEC_CODEC_H264_SVC:
				media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",nH264SVCPT);
				break;
			default:
				break;
			}
		}
		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",media);
		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"b=AS:%d\r\n",nMediaBandwidth);
		if(m_bRtcp)	buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=rtcp:%d IN IP4 %s\r\n",nMediaRTCPPort,m_strLocalIP);

		MediaH264RtpMap=false;
		for(i=0; i<nMidiaCapCount; i++)
		{
			int nHasQCIF = 0;
			int nHasCIF = 0;
			int nHas4CIF = 0;
			int nVideoRate = 384;
			int nH264Profile=0x42;
			int nH264Payload=nH264PT;
			VideoCapability & capVideo = MediaCap[i];
			//VIDEO_CAPABILITY_ITEM& capVideo = MediaCap.m_Caps[i];
			//nHasQCIF = capVideo.nQCIF;
			//nHasCIF = capVideo.nCIF;
			//nHas4CIF = capVideo.n4CIF;
			//nVideoRate = capVideo.m_nVideoBitrate;
			nVideoRate = 90000;

			int nVideoFrameRate = capVideo.m_nVideoFrameRate;
			VIDEC_CODEC_TYPE	VideoCodecType = capVideo.m_VideoCodecType;
			VIDEO_FORMAT_TYPE	VideoFormatType = capVideo.m_VideoFormatType;
			int nDirection = capVideo.m_nDirection;
			int nFormatValue = capVideo.m_nFormatValue;
			//nVideoRate =DapCap.m_aMainVideoCap[i].m_nVideoBitrate;
			//iEntryNumber = GetVideoCapEntry(VideoCodecType,VideoFormatType,nDual);
			int n4CIFValue =1;

			switch( capVideo.m_VideoCodecType )
			{ 
			case VIDEC_CODEC_H261:
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_4CIF) nHas4CIF =n4CIFValue;
				if(nHas4CIF >0) nHasCIF=1;
				if(nHasCIF >0) nHasQCIF=1;
				attr_len=0;
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d H261/%d\r\n",31,nVideoRate);
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d",31);
				if(nHasCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF=%d",nHasCIF);
				if(nHasQCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," QCIF=%d",nHasQCIF);
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," maxbr=%d",nMediaBandwidth);

				//attr[attr_len-1]='\0'; //;去之
				buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",attr);
				break;				
			case VIDEC_CODEC_H263:
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_4CIF) nHas4CIF =n4CIFValue;
				if(nHas4CIF >0) nHasCIF=1;
				if(nHasCIF >0) nHasQCIF=1;

				attr_len=0;
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d H263/%d\r\n",34,nVideoRate);

				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d",34);
				if(nHas4CIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," XGA=1");
				if(nHas4CIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF4=%d",nHas4CIF);
				if(nHasCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF=%d",nHasCIF);
				if(nHasQCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," QCIF=%d",nHasQCIF);

				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," maxbr=%d",nMediaBandwidth);

				//attr[attr_len-1]='\0'; //;去之
				buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",attr);
				break;

			case VIDEC_CODEC_H263P:
				if(nFormatValue >3) nHas4CIF = n4CIFValue; //VIDEO_FORMAT_4CIF 3
				if(VideoFormatType == VIDEO_FORMAT_QCIF) nHasQCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_CIF) nHasCIF =1;
				if(VideoFormatType == VIDEO_FORMAT_4CIF) nHas4CIF =n4CIFValue;
				if(nHas4CIF >0) nHasCIF=1;
				if(nHasCIF >0) nHasQCIF=1;

				attr_len=0;
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d H263-1998/%d\r\n",nH263PPT,nVideoRate);
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d",nH263PPT);	

				SetH263CustomStr(h263POption,attr,attr_len,MAX_ATTR_STR_LEN);
				if(nHas4CIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF4=%d",nHas4CIF);
				if(nHasCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF=%d",nHasCIF);
				if(nHasQCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," QCIF=%d",nHasQCIF);

				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," maxbr=%d",nMediaBandwidth);

				buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",attr);
				break;

			case VIDEC_CODEC_H263PP:
				attr_len=0;
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d H263-2000/%d\r\n",nH263PPPT,nVideoRate);
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d",nH263PPPT);

				SetH263CustomStr(h263PPOption,attr,attr_len,MAX_ATTR_STR_LEN);

				if(nHas4CIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF4=%d",nHas4CIF);
				if(nHasCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," CIF=%d",nHasCIF);
				if(nHasQCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," QCIF=%d",nHasQCIF);

				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," maxbr=%d",nMediaBandwidth);

				buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",attr);
				break;
			case VIDEC_CODEC_H264_SVC:
			case VIDEC_CODEC_H264:
				if( capVideo.m_VideoCodecType == VIDEC_CODEC_H264_SVC ) 
				{
					nH264Profile=0x64; //notice wait find value
					nH264Payload=nH264SVCPT;
				}
				if( (capVideo.m_VideoCodecType == VIDEC_CODEC_H264) && (MediaH264RtpMap) ) break;
				if( capVideo.m_VideoCodecType == VIDEC_CODEC_H264) MediaH264RtpMap=!MediaH264RtpMap;

				attr_len=0;
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d H264/%d\r\n",nH264Payload,nVideoRate);
				buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);

				attr_len=0;
				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d",nH264Payload);
				if(nHas4CIF>0&&nHas4CIF<16)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," profile-level-id=%x%x0%x",nH264Profile,0x80,nHas4CIF);
				if(nHas4CIF>=16)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len," profile-level-id=%x%x%x",nH264Profile,0x80,nHas4CIF);

				attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"%smax-br=%d",paramToken,nMediaBandwidth);

				if(nHasQCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"%smax-mbps=%d",paramToken,nHasQCIF);
				if(nHasCIF>0)
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"%smax-fs=%d",paramToken,nHasCIF);

				////Media Attribute (a): fmtp:107 profile-level-id=42801F;max-br=1536;max-mbps=115000;max-fs=3840;packetization-mode=1
				//attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"%s packetization-mode=1",paramToken);

				//attr[attr_len-1]='\0'; //;去之
				buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",attr);
				break;
			default:
				break;
			}
		}

		//}}gsh 设置视频及辅流能力集代码相同 20120206

		//video
		if(mediaCapNum==1)
		{
			buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=sendrecv\r\n");
			if(m_bProxy) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=nortpproxy:yes\r\n");
			if(m_bUseFECEnable ) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=reliablertp:fec-enable\r\n");
		}
		//Dual video
		if(mediaCapNum==2)
		{
			buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=sendrecv\r\n");					
			if(m_bProxy) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=nortpproxy:yes\r\n");
			if(m_bUseFECEnable ) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=reliablertp:fec-enable\r\n");

			buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=content:slides\r\n");
			buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=label:%d\r\n",m_nBFCPDualVideoLabel);
		}
	}

	//h224
	if(DapCap.m_nDataCapCount >0) 
	{
		media_len=0;
		media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len,"m=application %d RTP/AVP",m_H224Port);

		for(i=0; i< DapCap.m_nDataCapCount; i++)
		{
			switch( DapCap.m_DataCap[i].m_DataCodecID )
			{
			case DATA_CODEC_H224:
				{
					media_len+=snprintf(&media[media_len],MAX_MEDIA_STR_LEN-media_len," %d",m_H224PT);
				}
				break;
			default:
				break;
			}
		}
		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s\r\n",media);
		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"b=AS:%d\r\n",480);
		if(m_bRtcp)
			buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=rtcp:%d IN IP4 %s\r\n",h224RTCPPort,m_strLocalIP);

		for(i=0; i<DapCap.m_nDataCapCount; i++)
		{
			switch(  DapCap.m_DataCap[i].m_DataCodecID )
			{
			case DATA_CODEC_H224:
				{
					attr_len=0;
					attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=rtpmap:%d H224/4800\r\n",m_H224PT);
					//attr_len+=snprintf(&attr[attr_len],MAX_ATTR_STR_LEN-attr_len,"a=fmtp:%d bitrate=480\r\n",m_H224PT);
					buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"%s",attr);		
				}
				break;
			default:
				break;
			}
		}
		buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=sendrecv\r\n");
		if(m_bUseFECEnable ) buf_len+=snprintf(&buf[buf_len],MAX_BUF_STR_LEN-buf_len,"a=reliablertp:fec-enable\r\n");
	}

#ifdef HAS_SIP

	//status=pjmedia_sdp_parse_build(pool,buf,strlen(buf),sdp);
	status=pjmedia_sdp_parse(pool,buf,strlen(buf),&sdp);
	if (status != PJ_SUCCESS) 
	{
		return -1;
	}

	*p_sdp = pjmedia_sdp_session_clone(pool,sdp);

	if(*p_sdp==NULL) return -1;

#endif

	return 0;
}

//对H263P及H263PP设置更高分辨率能力集参数
void SIPCallCapability::SetH263CustomStr2(VIDEO_FORMAT_TYPE nH263Format,char * buffer,int &buf_len,int maxLen)
{
	//CIF 352*288 , 4CIF 704*576  ,16CIF 1408*1152 ,  720I 1280*360, 720P 1280*720,  1080I 1920*540,  
	//1080P 1920*1080, VGA 640*480, SVGA 800*600, XGA 1024*768,  SXGA 1280*1024
	struct tagH263OptionItem
	{
		int nWidth;
		int nHeight;
		int nValue;
		VIDEO_FORMAT_TYPE nFormat;
	};

	int i=0;
	int j=0;
	int nCount=0;
	int error=0;
	struct tagH263OptionItem H263OptionItem[]={ {160,120,1,VIDEO_FORMAT_VGA}, {200,150,1,VIDEO_FORMAT_SVGA}, {256,192,1,VIDEO_FORMAT_XGA}, 
	{320,180,3,VIDEO_FORMAT_SXGA}, {320,256,3,VIDEO_FORMAT_720P}, {480,270,3,VIDEO_FORMAT_1080P}, {352,288,3,VIDEO_FORMAT_16CIF}};

	nCount=sizeof(H263OptionItem)/sizeof(tagH263OptionItem);

	for(j=0;j<nCount;j++)
	{
		if(H263OptionItem[j].nFormat == nH263Format)
		{
			i =j+1;
			break;
		}
	}

	for(j=i-1;j >=0;j--)
	{
		buf_len+=snprintf(&buffer[buf_len],maxLen-buf_len," custom=%d,%d,%d",H263OptionItem[i].nWidth,H263OptionItem[i].nHeight,H263OptionItem[i].nValue);
	}

	//buf_len+=snprintf(&buffer[buf_len],maxLen-buf_len," custom=1280,1024,3 custom=1280,720,3 custom=1024,768,1 custom=1024,576,2 custom=800,600,1");

}

void SIPCallCapability::GetVideoSIPH264MaxFSMBPS(int & nMaxMBPS,int & nMaxFS,int & nLevel,int nFrame,VIDEO_FORMAT_TYPE format)
{
	nLevel = GetVideoH264Level(format);
	GetVideoH264MaxFS(nMaxFS,format);
	float fFrameRate=nFrame;
	int nSIPLevel = GetH264SIPLevel(nLevel);
	nLevel   = nSIPLevel;
	nMaxMBPS = nMaxFS * fFrameRate;
	nMaxFS = nMaxFS;
	if(nMaxFS <=0) nMaxFS =1;

}

int SIPCallCapability::GetH264SIPLevel(int nH323Level)
{
	//1-15   1b-19   1.1-22   1.2-29  1.3-36   2-43  2.1-50  2.2-57   3-64   3.1-71   3.2-78  4-85   4.1-92   4.2-99    5-106  5.1-113  注:正常级别之间对应值差7
	int nSIPLevel =0;
	struct tagH264LevelItem
	{
		int nSIPLevel;
		int nH323Level;
	};

	struct tagH264LevelItem H264LevelArr[]={ {0x0A,15}, {0x0B,22}, {0x0C,29},{0x0D,36}, {0x14,43}, {0x15,50}, 
	{0x16,57},{0x1E,64}, {0x1F,71}, {0x20,78},{0x28,85},{0x29,92}, {0x2A,99}, {0x32,106}, {0x33,113}};
	int nCount=sizeof(H264LevelArr)/sizeof(tagH264LevelItem);

	for(int i=0;i<nCount;i++)
	{
		if(nH323Level >= H264LevelArr[i].nH323Level && nH323Level < H264LevelArr[i+1].nH323Level)
		nSIPLevel = H264LevelArr[i].nSIPLevel;
	}

	return nSIPLevel;
}

int SIPCallCapability::GetH264H323Level(int nSIPLevel)
{
	//1-15   1b-19   1.1-22   1.2-29  1.3-36   2-43  2.1-50  2.2-57   3-64   3.1-71   3.2-78  4-85   4.1-92   4.2-99    5-106  5.1-113  注:正常级别之间对应值差7
	int nH323Level =0;
	struct tagH264LevelItem
	{
		int nSIPLevel;
		int nH323Level;
	};

	struct tagH264LevelItem H264LevelArr[]={ {0x0A,15}, {0x0B,22}, {0x0C,29},{0x0D,36}, {0x14,43}, {0x15,50}, 
	{0x16,57},{0x1E,64}, {0x1F,71}, {0x20,78},{0x28,85},{0x29,92}, {0x2A,99}, {0x32,106}, {0x33,113}};
	int nCount=sizeof(H264LevelArr)/sizeof(tagH264LevelItem);

	for(int i=0;i<nCount;i++)
	{
		if(nSIPLevel >= H264LevelArr[i].nSIPLevel && nSIPLevel < H264LevelArr[i+1].nSIPLevel)
		nH323Level = H264LevelArr[i].nH323Level;
	}

	return nH323Level;
}

VIDEO_FORMAT_TYPE SIPCallCapability::GetSIPFormatTypeFromH264Param(int nProfile,int nLevel,int nMaxFS,int nBitrate,float fFrame,int nType) //nType 0 get min ,1 get max
{
	VIDEO_FORMAT_TYPE nMaxFSType = GetVideoFormatTypeFromMaxFS(nMaxFS);
	if(nMaxFS >0)
	{
		return nMaxFSType;
	}
	VIDEO_FORMAT_TYPE nCompareType = GetFormatTypeFromH264OtherParam(nProfile,nLevel,nBitrate,nType,nMaxFSType,fFrame);
	return nCompareType;
}

//m_VideoPayloadType, m_AudioPayloadType, m_DualVideoPayloadType,m_H224PayloadType
void SIPCallCapability::SetLiveSDPMediaPayloadType(EMediaType type,int pt)
{
	if(pt >0)
	{
		int i = e_Video_H264;
		switch(type )
		{ 
		case e_Media_Video:
			m_VideoPayloadType=pt;
			for(i =e_Video_H264;i<=e_Video_H264SVC;i++)
			{
				SetPayloadType( (EMediaCoderType)i,pt);
			}
			break;
		case e_Media_Audio:
			m_AudioPayloadType=pt;
			for(i =e_Audio_G722_48k;i<=e_Audio_G7221C_48k;i++)
			{
				SetPayloadType( (EMediaCoderType)i,pt);
			}
			break;
		case e_Media_H224:
			m_H224PayloadType=pt;
			SetPayloadType( (EMediaCoderType)e_H224,pt);
			break;
		case e_Media_DualVideo:
			m_DualVideoPayloadType=pt;
			for(i =e_DualVideo_H264SVC;i<=e_DualVideo_H263PP;i++)
			{
				SetPayloadType( (EMediaCoderType)i,pt);
			}
			break;
		default:break;
		}
	}
}

