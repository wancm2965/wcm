#include "stdafx.h"
#include "H323Channel.h"
#include "H323Call.h"
#include "XUtil.h"
#include "H323CallMgr.h"
#include "CallMgr.h"
#include "XSocket.h"
#include "DataUdp.h"
//add medianetrtp channel 0615
#include "../IMXEC_PJSIP/MediaNet_RTP.h"

#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_RTPBUF_LEN	65535

BYTE g_lpRevTable[256];

#define UDPObjectClose(type , a) \
{ \
	type temp = a ; \
	a = NULL; \
	if (temp != NULL) \
{ \
	temp->Close(); \
	delete temp; \
	temp = NULL; \
} \
}

extern void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void eventHandler (HRTPSESSION hRTP, void* context)
{
	H323Channel*  pH323Chanel = (H323Channel*)context;

	if(pH323Chanel)
	{
		rtpParam p;
		int error;

		memset(&p,0,sizeof(rtpParam));
		if(pH323Chanel->m_bEncryptionStatus && pH323Chanel->m_bEncryptionSecret)
		{
			////the use to media secret update
			//p.payload=pH323Chanel->PayloadType();
			//p.extensionData=(RvUint32 * )pH323Chanel->m_strSharedSecretOLC;
			//if(pH323Chanel->m_nAlgorithmId == ALGORITHM_DES_CBC) { p.len=7;}
			//else if(pH323Chanel->m_nAlgorithmId == ALGORITHM_TRIPLE_DES_CBC ) { p.len=21;}
			//else if(pH323Chanel->m_nAlgorithmId == ALGORITHM_AES_CBC ) { p.len=16;}
			//else if(pH323Chanel->m_nAlgorithmId == ALGORITHM_AES256_CBC ) { p.len=32;}
			//else return;
		}

#if 1
		error = rtpReadEx(hRTP,pH323Chanel->m_pBuf,pH323Chanel->m_nBufLen,(GetCurrentTime()-pH323Chanel->m_nTime)*8,&p);
#else
		error = rtpRead(hRTP,pH323Chanel->m_pBuf,pH323Chanel->m_nBufLen,&p);
#endif
		if (error==0)
		{
			//ConvertToNetwork(pH323Chanel->m_pBuf,0,3);
			pH323Chanel->OnRecvdRTPPacket(pH323Chanel->m_pBuf, p);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void MakeReverseTable( LPBYTE lpRevTable, int nNum )
{
	BYTE bTempByte;
	int i, j, k;

	for( i = 0; i < nNum; i++ )
	{
		lpRevTable[i] = (BYTE)i;

		j = sizeof(BYTE) * 8;
		bTempByte = 0;
		for( k = 0; k < j; k++ )
		{
			if( ( ( lpRevTable[i] ) & ( ( BYTE ) 1 << k ) ) != 0 )
				bTempByte += ((BYTE)1) << ( j - 1 - k );
		}                    
		lpRevTable[i] = bTempByte;
	}
}

////////////////////////////////////////////////////////////////////////
//
H323Channel::H323Channel(H323Call& rH323Call,HCHAN hsChan)
:m_rH323Call(rH323Call)
,m_hsChan(hsChan)

,m_nChannelState(cmChannelStateIdle)
{
	m_strChannelName="";

	m_hRTP = NULL;
	m_hRTCP = NULL;
	m_pBuf = new char[MAX_RTPBUF_LEN];
	if(m_pBuf)
	{
		m_nBufLen = MAX_RTPBUF_LEN;
	}
	else
	{
		m_nBufLen = 0;
	}

	m_CurVideoID = 1;
	m_nAnnexQSequence = 0;
	m_nAnnexQTimeStamp = ::GetCurrentTime();
	m_nAnnexQSSRC = (unsigned int)this;

	m_AudioCodecID=X_AUDIO_CODEC_G711U;
	m_VideoCodecType=VIDEC_CODEC_H264;
	m_VideoFormatType=VIDEO_FORMAT_COUNT;
	m_nAlgorithmId=0;
	m_bEncryptionStatus=false;
	m_bEncryptionSecret =false;
	m_nMediaType=Enum_Channel_Media_Invalid;
	m_nH264Level=0;
	m_nWidth=0;
	m_nHeight=0;
	m_nLastRTPSeq=0;
	m_nTimestamp=0;
	m_nLastRTPTimestamp=0;
	m_nSeqCount=0;
	m_nDirection=0;
	m_nPayloadType=0;

	char szChannelID[128];
	sprintf(szChannelID, "Secretkey%uSecretkey%u", (unsigned long)this, (unsigned long)this);

	memset(m_strSharedSecretOLC,0,sizeof(m_strSharedSecretOLC));
	memset(m_strSpecifiedSecretOLC,0,sizeof(m_strSpecifiedSecretOLC));
	strcpy(m_strSpecifiedSecretOLC,szChannelID);

	sprintf(szChannelID, "Secretyek%uSecretyek%u", (unsigned long)this, (unsigned long)this);
	memset(m_strSharedSecretUpdate,0,sizeof(m_strSharedSecretUpdate));
	memset(m_strSpecifiedSecretUpdate,0,sizeof(m_strSpecifiedSecretUpdate));
	strcpy(m_strSpecifiedSecretUpdate,szChannelID);
	m_nPayloadTypeUpdate =0;
	m_nAlgorithmIdUpdate =0;
	m_bSecretUpdate=false;
	m_nSecretUpdateTimes=0;
	m_nSecretUpdateMaxTimes=100;
	m_nMsdStatus=0;

	m_ulKeepAliveIP	= 0;
	m_nKeepAlivePort = 0;
	m_pFWNAT_UDP=NULL;
	m_bGetKeepAlive=false;
	m_nRFCNumber=0;
	m_bH235OLCCap =false;
	m_bH235Cap =false;
	m_nFlowCCBitRate = 1;//flowControlCommand add
	m_nRemoteOLCMaxMBPS =0;	//用于OLC能力集可选参数值 最大宏块处理速率
	m_nRemoteOLCMaxFS =0;		//用于OLC能力集可选参数值 最大帧长
	m_nRemoteOLCMaxDPB =0;		//用于OLC能力集可选参数值 最大解码图像缓存大小
	m_nRemoteOLCMaxBR =0;		//用于OLC能力集可选参数值 最大视频码率

	//add medianetrtp channel 0615
	m_bUseMediaNetRTP =false;
	m_pMediaNet_RTP =NULL;
	m_nEndpontType =0;
	m_ulRTPRemoteIP = 0;
	m_ulRTCPRemoteIP = 0;
	m_nRTPTime =0;
	m_nIncRTPTime =0;
	m_nVideoFrameRate = VideoFrameBase ;

}

H323Channel::~H323Channel()
{
	if(m_hRTP )
	{
		m_hRTP = NULL;
		m_hRTCP = NULL;
	}
	if(m_pBuf)
	{
		delete[] m_pBuf;
		m_pBuf = NULL;
		m_nBufLen = 0;
	}
	if(EncryptionStatus()==true)
	{
		m_DesEncryption.EncryptionEnd();
		m_DecDesEncryption.EncryptionEnd();
		EncryptionStatus()=false;
		m_bH235OLCCap =false;
		m_bH235Cap =false;
	}

	UDPObjectClose(CDataUDP * ,m_pFWNAT_UDP)
	//if(m_pFWNAT_UDP!=NULL)
	//{
	//	m_pFWNAT_UDP->Close();
	//	delete m_pFWNAT_UDP;
	//	m_pFWNAT_UDP=NULL;
	//}
}

unsigned int&H323Channel::AlgorithmId(void)
{
	return m_nAlgorithmId;
}

bool& H323Channel::EncryptionStatus(void)
{
	return m_bEncryptionStatus;
}

X_AUDIO_CODEC_ID& H323Channel::AudioCodecID(void)
{
	return m_AudioCodecID;
}

VIDEC_CODEC_TYPE& H323Channel::VideoCodecType(void)
{
	return m_VideoCodecType;
}

VIDEO_FORMAT_TYPE& H323Channel::VideoFormatType(void)
{
	return m_VideoFormatType;
}

int & H323Channel::VideoFrameRate(void)
{
	return m_nVideoFrameRate;
}

std::string&H323Channel::ChannelName(void)
{
	return m_strChannelName;
}

unsigned int& H323Channel::Direction(void)
{
	return m_nDirection;
}

int& H323Channel::PayloadType(void)
{
	return m_nPayloadType;
}

int& H323Channel::MaxBitrate(void)
{
	return m_nMaxBitrate;
}

void H323Channel::SetLocalRTPAddress(unsigned long ip, unsigned short port)
{
	m_ulRTPLocalIP = ip;
	m_nLocalRTPPort = port;
}

void H323Channel::SetRemoteRTPAddress(unsigned long ip, unsigned short port)
{
	m_ulRTPRemoteIP = ip;
	m_nRTPRemotePort = port;
}

void H323Channel::SetLocalRTCPAddress(unsigned long ip, unsigned short port)
{
	m_ulRTCPLocalIP = ip;
	m_nLocalRTCPPort = port;
}

void H323Channel::SetRemoteRTCPAddress(unsigned long ip, unsigned short port)
{
	m_ulRTCPRemoteIP = ip;
	m_nRTCPRemotePort = port;

	struct in_addr addr;
	addr.S_un.S_addr =m_ulRTCPRemoteIP;
	std::string strRemoteIP=inet_ntoa(addr);
	char msg[256]={0};
	sprintf(msg,"SetRemoteRTCPAddress to set param of recv.rtp:%d,rtcp:%d,remoteIP:%s\n",m_nRTPRemotePort,m_nRTCPRemotePort,strRemoteIP.c_str());
	//OutputDebugString(msg);
}

void H323Channel::SetLocalAddress(unsigned long& ulRtpIP, unsigned short& usRtpPort, unsigned long& ulRtcpIP, unsigned short& usRtcpPort)
{
	SetLocalRTPAddress(ulRtpIP, usRtpPort);
	SetLocalRTCPAddress(ulRtcpIP, usRtcpPort);
}

int H323Channel::GetChannelInfo(CHANNEL_INFO& rChannelInfo)
{
	rChannelInfo.AudioCodecID=m_AudioCodecID;
	rChannelInfo.VideoCodecType=m_VideoCodecType;
	rChannelInfo.VideoFormatType=m_VideoFormatType;

	rChannelInfo.nDirection = m_nDirection;
	rChannelInfo.nPayloadType = m_nPayloadType;

	rChannelInfo.ulRTPLocalIP = m_ulRTPLocalIP;
	rChannelInfo.nRTPLocalPort = m_nLocalRTPPort;
	rChannelInfo.ulRTPRemoteIP = m_ulRTPRemoteIP;
	rChannelInfo.nRTPRemotePort = m_nRTPRemotePort;

	rChannelInfo.ulRTCPLocalIP = m_ulRTCPLocalIP;
	rChannelInfo.nRTCPLocalPort = m_nLocalRTCPPort;
	rChannelInfo.ulRTCPRemoteIP = m_ulRTCPRemoteIP;
	rChannelInfo.nRTCPRemotePort = m_nRTCPRemotePort;

	rChannelInfo.nMaxBitrate = m_nMaxBitrate;
	rChannelInfo.nH264Level = m_nH264Level;
	GetVideoFormatType(m_nWidth,m_nHeight);
	rChannelInfo.nWidth = m_nWidth;
	rChannelInfo.nHeight = m_nHeight;

	rChannelInfo.nFrameRate =m_nVideoFrameRate;
	//if(m_nRemoteOLCMaxMBPS >0 && m_nRemoteOLCMaxFS >0 ) 
	//	rChannelInfo.nFrameRate = (m_nRemoteOLCMaxMBPS *500 ) / ( m_nRemoteOLCMaxFS * 256 );

	return 0;
}

int H323Channel::RequestKeyFrame(void)
{
	return cmChannelVideoFastUpdatePicture(m_hsChan);
}

int H323Channel::SendChannelOn(void)//logicalChannelActive
{
	return cmChannelOn(m_hsChan);
}

int H323Channel::SendChannelOff(void)//logicalChannelInactive
{
	return cmChannelOff(m_hsChan);
}
////////////////////////////////////////////////////////////////////////////////////////////


void H323Channel::OnChannelOffering(void)
{
	unsigned short usPort = m_rH323Call.GetMediaRtpPort(m_nMediaType,this);
	unsigned long ulLocalIP=H323CallMgr::Instance().GetLocalIPAddress(m_rH323Call.m_strLocalIP);
	if(m_rH323Call.m_bForceIP) ulLocalIP =m_rH323Call.m_nForceIP;
	SetLocalRTPAddress(ulLocalIP, usPort);
	SetLocalRTCPAddress(ulLocalIP, usPort+1);

	unsigned long nExternalIPAddress=m_rH323Call.GetMgrExternalIPAddress();
	if (nExternalIPAddress)
	{
		cmChannelSetAddress(m_hsChan, nExternalIPAddress, m_nLocalRTPPort);
		cmChannelSetRTCPAddress(m_hsChan, nExternalIPAddress, m_nLocalRTCPPort);
	}
	else
	{
		if(m_rH323Call.m_bForceIP) m_ulRTCPLocalIP = m_ulRTPLocalIP =m_rH323Call.m_nForceIP;
		cmChannelSetAddress(m_hsChan, m_ulRTPLocalIP, m_nLocalRTPPort);
		cmChannelSetRTCPAddress(m_hsChan, m_ulRTCPLocalIP, m_nLocalRTCPPort);
	}

	//mcu级联测试 再次检查是否支持对方视频或辅流通道
	if( H323CallMgr::Instance().m_nTerminalCapsType ==1)
	{
		if(m_rH323Call.CheckConfCapsSupported(m_nMediaType,m_VideoCodecType,m_VideoFormatType) ==false)
		{
			cmChannelRejectReason(m_hsChan,cmRejectReasonDataTypeNotSupported);
			return;
		}
	}

	if(EncryptionStatus()==true)
	{
		int encSpecified=0;
		encSpecified=m_rH323Call.GetMgrH235MediaEncrypSpecified();
		if( (encSpecified==0) && (m_rH323Call.m_bShareSecret==false) ) 
		{
			EncryptionStatus()=false;
			cmChannelRejectReason(m_hsChan,cmRejectReasonUnspecified);
		}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
		else  cmChannelAnswer(m_hsChan);
#else
	else m_rH323Call.OnAdapCallRecvChannelOpen(this);
#endif

#else
	else  cmChannelAnswer(m_hsChan);
#endif

	}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
		else  cmChannelAnswer(m_hsChan);
#else
	else m_rH323Call.OnAdapCallRecvChannelOpen(this);
#endif

#else

	else  cmChannelAnswer(m_hsChan);

#endif


}

//通道已连接
void H323Channel::OnChannelConnected(cmChannelState_e state,cmChannelStateMode_e stateMode)
{
	m_nRTPTime =GetCalcTimeStamp();

	if(H323CallMgr::Instance().m_nUseMediaNetRTP >0) m_bUseMediaNetRTP =true;
	//notice Audio H224 not use medianetrtp
	if(m_nMediaType == Enum_Channel_Media_Audio || m_nMediaType == Enum_Channel_Media_H224) m_bUseMediaNetRTP =false;
	//if(CallMgr::Instance().m_nUseNETECSend <= 0 ) m_bUseMediaNetRTP=false;  //notice ,this not control 
	 //notice medianet rtp unsupport MPEG4,use to call kedacom
	if(m_nMediaType == Enum_Channel_Media_Video && m_VideoCodecType==VIDEC_CODEC_MPEG4) m_bUseMediaNetRTP =false;

	//20141118 add , check NETEC Transport not call
	if(m_bUseMediaNetRTP && m_rH323Call.HasAVCONTransport() ) m_bUseMediaNetRTP =false;

	if(m_bUseMediaNetRTP) return MediaNetRtpChannelConnected(state,stateMode);

	m_nFlowCCBitRate = 1;//flowControlCommand add
	m_nMsdStatus = cmCallMasterSlaveStatus(m_rH323Call.m_hsCall);
	m_nAlgorithmIdUpdate = m_nAlgorithmId;
	if(m_bEncryptionSecret && ( m_bH235OLCCap || m_bH235Cap ) ) EncryptionStatus()=true;

	if (state!=m_nChannelState)
	{
		if (m_hRTP==NULL)
		{
			m_hRTP=(HRTPSESSION ) m_rH323Call.GetCreateMediaRtpHandle(m_nLocalRTPPort);
			if(m_hRTP ==NULL)
			{
				std::string strMediaName="MediaChannel";
				if(m_nMediaType == Enum_Channel_Media_Audio) strMediaName="AUDIOChannel";
				else if(m_nMediaType == Enum_Channel_Media_Video) strMediaName="VIDEOChannel";
				else if(m_nMediaType == Enum_Channel_Media_DualVideo) strMediaName="DUALVIDEOChannel";
				else if(m_nMediaType == Enum_Channel_Media_H224) strMediaName="DATAChannel";
				m_hRTP = rtpOpenEx( m_nLocalRTPPort, 1, 0xff, (char*)strMediaName.c_str() );
				if(m_hRTP!=NULL) m_rH323Call.SaveMediaRtpHandle((void *)m_hRTP,m_nLocalRTPPort);
			}
			if( m_hRTP )
			{
				if(EncryptionStatus()==true)
				{
					AddChannelEncryption();
				}

				int nRecvBufSize=16*1024*1024;
				int nSendBufSize=65536;
				rtpSetReceiveBufferSize(m_hRTP,nRecvBufSize);
				rtpSetTransmitBufferSize(m_hRTP,nSendBufSize);

				m_nTime = GetCurrentTime();
				m_hRTCP = rtpGetRTCPSession(m_hRTP);

				if(m_nDirection ==cmCapReceive || m_nDirection ==cmCapReceiveAndTransmit)
					rtpSetEventHandler(m_hRTP,eventHandler,this);

				std::string & strMulticastIP=m_rH323Call.m_strMediaMulticastIP;
				unsigned long ulLocalMulticastIP=0;
				if (strMulticastIP.size()>0 )
				{
					ulLocalMulticastIP =htonl(XSocket::atoh(strMulticastIP.c_str()));
				}

				if(m_nDirection==cmCapTransmit)
				{
					if(! m_rH323Call.m_bMediaMulticastEnable)
					{
						rtpSetRemoteAddress(m_hRTP, m_ulRTPRemoteIP, m_nRTPRemotePort);
						rtcpSetRemoteAddress(m_hRTCP, m_ulRTCPRemoteIP, m_nRTCPRemotePort);
					}
					else
					{
						rtpSetRemoteAddress(m_hRTP, ulLocalMulticastIP, m_nRTPRemotePort);
						rtcpSetRemoteAddress(m_hRTCP, ulLocalMulticastIP, m_nRTCPRemotePort);
					}

					rtpUseSequenceNumber(m_hRTP);//采用指定的SequenceNumber
				}

				if(m_rH323Call.m_bMediaMulticastEnable)
				{
					rtpSetGroupAddress(m_hRTP, ulLocalMulticastIP);
					if(m_rH323Call.m_nMediaMulticastTTL > 0) RvRtpSetMulticastTTL(m_hRTP, m_rH323Call.m_nMediaMulticastTTL);
				}
			}
		}

		m_nChannelState=state;
		m_rH323Call.OnChannelConnected(this);
		m_rH323Call.OnChannelMediaEncryptionState(this);

		//打开发送打洞包
		if(m_hRTP && m_bGetKeepAlive) OpenKeepAliveChannel();

	}

	if (stateMode==cmChannelStateModeOn)
	{

	}

}

//信道已经关闭
void H323Channel::OnChannelDisconnected(void)
{
	HRTPSESSION hRTP=m_hRTP;
	m_hRTP = NULL;
	m_hRTCP = NULL;

	if(m_bUseMediaNetRTP) return MediaNetRtpChannelDisconnected();

	if(hRTP && m_nDirection == cmCapReceive)
		rtpSetEventHandler(hRTP,eventHandler,NULL);

	if (m_hsChan!=NULL)
	{
		//notice the not add 0718
		//cmChannelClose(m_hsChan);
		m_hsChan=NULL;
	}

	m_rH323Call.OnChannelDisconnected(this);
	if(m_nChannelState == cmChannelStateConnected)
	{
		m_rH323Call.RelatedMediaChannelCloseRTP( (void *)hRTP,this);
	}

	if(EncryptionStatus()==true)
	{
		m_DesEncryption.EncryptionEnd();
		EncryptionStatus()=false;
	}

	UDPObjectClose(CDataUDP * ,m_pFWNAT_UDP)

	m_bGetKeepAlive =false;

	//if(m_nMediaType ==Enum_Channel_Media_Video) OutputDebugString("Channel Disconnect.\n");//test 192.168.7.9##4744

}


void H323Channel::OnChannelRequestKeyFrame(void)
{
	m_rH323Call.OnChannelRequestKeyFrame(this);
}

//add VCF
void H323Channel::OnChannelRequestFreeze(void)
{
	m_rH323Call.OnChannelRequestFreeze(this);
}

void H323Channel::OnRecvdRTPPacket(char* pData, rtpParam& param)
{
	param.payload=m_nPayloadType; //notice set pt
	m_rH323Call.OnChannelRecvdRTPPacket(this,(unsigned char*)pData, param.len,param.payload,param.sByte,param.sequenceNumber,param.timestamp);

}

//Added by jiangdingfeng 20140929 增加生成SSRC接口
void H323Channel::GenerateSSRC()
{
	if (m_hRTP)
	{
		rtpRegenSSRC(m_hRTP);
	}
}

unsigned int H323Channel::GetCalcTimeStamp()
{
	if(m_nVideoFrameRate >0) m_nIncRTPTime =(90000 /m_nVideoFrameRate);
	else m_nIncRTPTime = (90000 /25);

	unsigned int nRet = 0;
	srand(timeGetTime());
	nRet = rand();
	if (nRet <=0)
	{
		nRet = 0x7fff;
	}
	return nRet;
}

void H323Channel::SendData(char* buff, int datalen)
{
	if(m_bUseMediaNetRTP) return MediaNetRtpSendData(buff,datalen);

	if(m_hRTP==NULL) return;

	rtpParam p;
	p.len = datalen;
	rtpUnpack(m_hRTP, buff, datalen, &p);

	p.payload=this->PayloadType();
	//视频辅流时间戳重新计算设置
	if(m_nMediaType ==Enum_Channel_Media_Video || m_nMediaType ==Enum_Channel_Media_DualVideo)
	{
		p.timestamp = m_nRTPTime;
		if(p.marker)
		{
			m_nRTPTime += m_nIncRTPTime;
		}
	}

	RvRtpPackExCustom(m_hRTP, p.sSrc,(RvUint8*)buff, datalen,&p);

	//RvUint32 nSrc=RvRtpGetSSRC(m_hRTP);
	//RvRtpRegenSSRC(m_hRTP);
	//nSrc=RvRtpGetSSRC(m_hRTP);
	SendDataEx(buff, datalen, &p);
}

void H323Channel::SendDataEx(char* buff, int datalen, rtpParam* p)
{
	int error;
	if(m_hRTP)
	{
		error = rtpWrite(m_hRTP, buff, datalen, p);
	}
}

//====AnnexQ
int H323Channel::AddAnnexQRtpHead(BYTE *tmp)
{
	m_nAnnexQSequence++;
	m_nAnnexQTimeStamp = XGetTimestamp() <<3;

	tmp[0] = 0x80;
	tmp[1] = m_nPayloadType&0x7f;//0x64;
	tmp[2] = (m_nAnnexQSequence>>8)&0xff;
	tmp[3] = m_nAnnexQSequence&0xff;
	tmp[4] = (m_nAnnexQTimeStamp>>24)&0xff;
	tmp[5] = (m_nAnnexQTimeStamp>>16)&0xff;
	tmp[6] = (m_nAnnexQTimeStamp>>8)&0xff;
	tmp[7] = (m_nAnnexQTimeStamp)&0xff;
	tmp[8] = (m_nAnnexQSSRC>>24)&0xff;
	tmp[9] = (m_nAnnexQSSRC>>16)&0xff;
	tmp[10]= (m_nAnnexQSSRC>>8)&0xff;
	tmp[11]= (m_nAnnexQSSRC)&0xff;	
	return 12;
}

void H323Channel::SendAnnexQInfo(RCMInfoTag rcmInfoTag)
{
	LSDStruct lsd;
	unsigned char lpFeccData[256];
	int len = 0;
	lsd.LSDLen = 0;

	EncodeBufferH281ToH224(rcmInfoTag, &lsd);
	int rtpHeaderLen = AddAnnexQRtpHead(lpFeccData);//添加RTP头
	len = rtpHeaderLen;
	memcpy(lpFeccData+len, lsd.LSDChar, lsd.LSDLen);
	len += lsd.LSDLen;

	//ConvertFromNetwork(lpFeccData, 0, 3);

	if (m_rH323Call.HasAVCONTransport())
		SendAnnexQDataNETEC((char*)lpFeccData, len); //the set use to netec send ptz 
	else
		SendData((char*)lpFeccData, len);

}

void H323Channel::RemoteCameraLeft()
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];

	rcmInfoTag.m_nDstMcuNo = 0;//对端的MCU号
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;//本端的MCU号
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_START;
	lpData[1] = H281_LEFT;
	lpData[2] = 0x0f&(CMAERA_DELAYTIME/UNIT_H281_TIME);
	memcpy(rcmInfoTag.m_bData, lpData, 3);
	rcmInfoTag.DataLen = 3;	
	rcmInfoTag.m_bData[3] = 0;

	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteCameraStopLeft()
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];
	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_STOP;
	lpData[1] = H281_LEFT;
	memcpy(rcmInfoTag.m_bData, lpData, 2);
	rcmInfoTag.DataLen = 2;	
	rcmInfoTag.m_bData[2] = 0;
	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteCameraRight()
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];

	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_START;
	lpData[1] = H281_RIGHT;
	lpData[2] = 0x0f & (CMAERA_DELAYTIME/UNIT_H281_TIME);
	memcpy(rcmInfoTag.m_bData, lpData, 3);
	rcmInfoTag.DataLen = 3;	
	rcmInfoTag.m_bData[3] = 0;

	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteCameraStopRight()
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];
	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_STOP;
	lpData[1] = H281_RIGHT;
	memcpy(rcmInfoTag.m_bData, lpData, 2);
	rcmInfoTag.DataLen = 2;	
	rcmInfoTag.m_bData[2] = 0;
	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteCameraUp()
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];

	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_START;
	lpData[1] = H281_UP;
	lpData[2] = 0x0f & (CMAERA_DELAYTIME/UNIT_H281_TIME);
	memcpy(rcmInfoTag.m_bData, lpData, 3);
	rcmInfoTag.DataLen = 3;	
	rcmInfoTag.m_bData[3] = 0;

	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteCameraStopUp()
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];
	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_STOP;
	lpData[1] = H281_UP;
	memcpy(rcmInfoTag.m_bData, lpData, 2);
	rcmInfoTag.DataLen = 2;	
	rcmInfoTag.m_bData[2] = 0;
	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteCameraDown()
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];

	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_START;
	lpData[1] = H281_DOWN;
	//lpData[2] = 0x0f & (CMAERA_DELAYTIME/UNIT_H281_TIME);
	lpData[2] = 0x0c;
	memcpy(rcmInfoTag.m_bData, lpData, 3);
	rcmInfoTag.DataLen = 3;	
	//rcmInfoTag.m_bData[3] = 0xf2;

	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteCameraStopDown()
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];
	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_STOP;
	lpData[1] = H281_DOWN;
	memcpy(rcmInfoTag.m_bData, lpData, 2);
	rcmInfoTag.DataLen = 2;	
	rcmInfoTag.m_bData[2] = 0;
	SendAnnexQInfo(rcmInfoTag);
}
void H323Channel::RemoteCameraZoomIn()
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];

	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_START;
	lpData[1] = H281_ZOOMIN;
	lpData[2] = 0x0f & (CMAERA_DELAYTIME/UNIT_H281_TIME);
	memcpy(rcmInfoTag.m_bData, lpData, 3);
	rcmInfoTag.DataLen = 3;	
	rcmInfoTag.m_bData[3] = 0;

	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteCameraStopZoomIn()
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];
	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_STOP;
	lpData[1] = H281_ZOOMIN;
	memcpy(rcmInfoTag.m_bData, lpData, 2);
	rcmInfoTag.DataLen = 2;	
	rcmInfoTag.m_bData[2] = 0;
	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteCameraZoomOut()
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];

	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_START;
	lpData[1] = H281_ZOOMOUT;
	lpData[2] = 0x0f & (CMAERA_DELAYTIME/UNIT_H281_TIME);
	memcpy(rcmInfoTag.m_bData, lpData, 3);
	rcmInfoTag.DataLen = 3;	
	rcmInfoTag.m_bData[3] = 0;

	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteCameraStopZoomOut()
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];
	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_STOP;
	lpData[1] = H281_ZOOMOUT;
	memcpy(rcmInfoTag.m_bData, lpData, 2);
	rcmInfoTag.DataLen = 2;	
	rcmInfoTag.m_bData[2] = 0;
	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteSourceSelect(int vsID)
{
	BYTE id = (BYTE)vsID;

	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];
	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_SELECT_VIDEOSRC;
	lpData[1] = (id << 4) & 0xFF;
	memcpy(rcmInfoTag.m_bData, lpData, 2);
	rcmInfoTag.DataLen = 2;	
	rcmInfoTag.m_bData[2] = 0;
	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteSourceSwitched(int vsID)
{
	BYTE id = (BYTE)vsID;

	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];
	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_SWITCH_VIDEOSRC;
	lpData[1] = (id << 4) & 0xFF;
	memcpy(rcmInfoTag.m_bData, lpData, 2);
	rcmInfoTag.DataLen = 2;	
	rcmInfoTag.m_bData[2] = 0;
	SendAnnexQInfo(rcmInfoTag);
}

void H323Channel::RemoteCameraPresetStore(int position)
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];

	if(position < 0 || position > 9)
	{
		return;
	}

	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_STORE_PRESET;
	lpData[1] = (position << 4) & 0xFF;
	memcpy(rcmInfoTag.m_bData, lpData, 2);
	rcmInfoTag.DataLen = 2;	
	rcmInfoTag.m_bData[2] = 0;

	SendAnnexQInfo(rcmInfoTag);
	return;
}

void H323Channel::RemoteCameraPresetActive(int position)
{
	RCMInfoTag rcmInfoTag;
	BYTE lpData[10];

	if(position < 0 || position > 9)
	{
		return;
	}

	rcmInfoTag.m_nDstMcuNo = 0;
	rcmInfoTag.m_nDstTerNo = 0;
	rcmInfoTag.m_nSrcMcuNo = 0;
	rcmInfoTag.m_nSrcTerNo = 0;
	rcmInfoTag.m_nPriority = HIGH_PRIORITY;
	rcmInfoTag.m_nType = TYPE_CONTROL;
	lpData[0] = H281_ACTIVE_PRESET;
	lpData[1] = (position << 4) & 0xFF;
	memcpy(rcmInfoTag.m_bData, lpData, 2);
	rcmInfoTag.DataLen = 2;	
	rcmInfoTag.m_bData[2] = 0;

	SendAnnexQInfo(rcmInfoTag);
}
//======================
USHORT H323Channel::crchware(USHORT data,USHORT genploy,USHORT accum)
{
	int i;

	data <<= 8;								// data to high byte
	for(i = 8;i > 0;i--)
	{
		if((data ^ accum) & 0x8000)			//if msb of(data XOR accum) is TRUE
			accum = (accum << 1) ^ genploy;	// shift and subtract poly
		else
			accum <<= 1;					//otherwise,transparent shift
		data <<= 1;							// move up next bit for XOR
	}
	return  accum;
}

void H323Channel::EncodeBufferH281ToH224(RCMInfoTag rcmInfoTag, LSDStruct *plsd)
{
	BYTE lpBuffer[256];
	int i, j, k, l, nSendNum;

	if(rcmInfoTag.DataLen == 0)
		return;

	for(i=k=0; i<8; i++,k++)
		lpBuffer[i] = 0x7e;

	if(rcmInfoTag.m_nPriority == HIGH_PRIORITY)
	{
		lpBuffer[i++] = 0x00;
		lpBuffer[i++] = 0x71;
	}
	else 
	{
		lpBuffer[i++] = 0x00;
		lpBuffer[i++] = 0x61;
	}

	lpBuffer[i++] = 0x03;
	if(rcmInfoTag.m_nType == TYPE_CONTROL)
	{
		lpBuffer[i++] = rcmInfoTag.m_nDstMcuNo;
		lpBuffer[i++] = rcmInfoTag.m_nDstTerNo;
		lpBuffer[i++] = rcmInfoTag.m_nSrcMcuNo;
		lpBuffer[i++] = rcmInfoTag.m_nSrcTerNo;
		lpBuffer[i++] = 0x01;
		lpBuffer[i++] = 0xc0;
	}
	else if(rcmInfoTag.m_nType == TYPE_CAPABILITY)
	{
		lpBuffer[i++] = 0;
		lpBuffer[i++] = 0;
		lpBuffer[i++] = rcmInfoTag.m_nSrcMcuNo;
		lpBuffer[i++] = rcmInfoTag.m_nSrcTerNo;
		lpBuffer[i++] = 0x00;
		lpBuffer[i++] = 0xC0;
	}
	else if(rcmInfoTag.m_nType == TYPE_CAMERA_POSITION_STATUS)
	{
		lpBuffer[i++] = 0;
		lpBuffer[i++] = 0;
		lpBuffer[i++] = rcmInfoTag.m_nSrcMcuNo;
		lpBuffer[i++] = rcmInfoTag.m_nSrcTerNo;
		lpBuffer[i++] = 0x7f;
		lpBuffer[i++] = 0x00;
		lpBuffer[i++] = 0x00;
		lpBuffer[i++] = 0x00;
		lpBuffer[i++] = 0x00;
		lpBuffer[i++] = 0x05;
		lpBuffer[i++] = 0xc0;
	}

	for(l=0; l<rcmInfoTag.DataLen; l++)
	{
		lpBuffer[i++] = rcmInfoTag.m_bData[l];
	}

	for(j=0; j<8; j++)
	{
		lpBuffer[i+j] = 0x7e;
	}

	nSendNum = Q922Encode(lpBuffer, i+j, k, i-1, g_lpRevTable, 0);

	plsd->LSDLen = nSendNum;

	memcpy(plsd->LSDChar, lpBuffer, nSendNum);
}

int H323Channel::Q922Encode(BYTE *lpSendData, int nTotalNum, int nUseStart, int nUseEnd, BYTE *lpRevTable, int nCrcCheckInd)
{
	unsigned short int nAccum = 0;
	int i = 0;
	int nAdd0Num = 0;
	int nNewByteNum = 0;       
	BYTE lpOutPutTemp[256];

	memset(lpOutPutTemp, 0x7e, 256);

	if(nUseEnd < nUseStart || nUseStart < 1)
		return 0;

	ReverseByte(lpSendData + nUseStart, nUseEnd - nUseStart + 1, lpRevTable);

	if(nCrcCheckInd == 0)
	{
		nAccum = 0xffff;	
		for( i = 0; i < nUseEnd - nUseStart + 1 ; ++i )
			nAccum = crchware( lpSendData[ nUseStart + i], 0x1021, nAccum );
		nAccum = ~nAccum;
		lpSendData[nUseEnd+1] = HIBYTE( nAccum );
		lpSendData[nUseEnd+2] = LOBYTE( nAccum );
	}

	nUseEnd = nUseEnd + 2;
	nAdd0Num = 0;

	nAdd0Num = Add0FromHDLC( lpSendData, nTotalNum, nUseStart, nUseEnd-nUseStart+1, lpOutPutTemp );
	nNewByteNum = nTotalNum + ( nAdd0Num + 7 ) / 8;
	memcpy( lpSendData, lpOutPutTemp, nNewByteNum );

	return nNewByteNum;
}

//HDLC中，如果连续5个1，则在其后插入1个0
int H323Channel::Add0FromHDLC( BYTE *lpH224HDLC, int nTotalNum, int nStartPos, int nNum, BYTE *lpH224HDLCOut )
{
	int i = 0;
	int j = 0;
	int k = 0;
	int nAdd1Num = 0;
	int nGoodByteNum = 0;
	BYTE lpTemp[4] = { 0, 0, 0, 0 };
	unsigned long* lplTemp = NULL;
	unsigned short  * lpnTemp = NULL;

	lpnTemp = (unsigned short *)lpTemp;
	lplTemp = (unsigned long*)lpTemp;
	nGoodByteNum = nStartPos;
	k = -8;

	//拷贝头部0x7e数据，这部分数据不处理
	for( i = 0; i < nStartPos; i++ )
	{
		lpH224HDLCOut[i] = lpH224HDLC[i];	      
	}

	//======================================
	//只对0x7e之间的有效数据处理
#if 0
	for( i=nStartPos; i<(nNum+nStartPos); i++ )
	{          
		lpTemp[2] = lpH224HDLC[i];
		for( j = 0; j < 8; j++ )
		{
			if( ( lpTemp[1] & 0x1f ) == 0x1f )
			{	
				(*lpnTemp) <<= 1;
				(*lpnTemp) &= 0xFFFE;
				nAdd1Num++;
				j--;
			}		
			else
			{
				*lplTemp <<= 1;
			} 
			k++;
			if( k == 8 )
			{
				k = 0;
				if( nGoodByteNum > 1000 )
					break; 
				lpH224HDLCOut[ nGoodByteNum++ ] = lpTemp[0];
			}							
		}  
	}

	//最后两个字节
	if( ( lpTemp[1] & 0x1f ) == 0x1f )
	{	
		(*lpnTemp) <<= 1;
		(*lpnTemp) &= 0xFFFE;
		nAdd1Num++;
		k++;
	}		

	if( k != 0 )                          
		lpH224HDLCOut[nGoodByteNum++] = lpTemp[0];

	lpH224HDLCOut[nGoodByteNum++] = lpTemp[1];
#else
	for( i=nStartPos; i<(nNum+nStartPos); i++ )
	{          
		lpTemp[1] = lpH224HDLC[i];
		for( j = 0; j < 8; j++ )
		{
			if( ( lpTemp[2] & 0x1f ) == 0x1f )
			{	
				(*lpnTemp) <<= 1;
				(*lpnTemp) &= 0xFFFE;
				nAdd1Num++;
				j--;
			}		
			else
			{
				*lplTemp <<= 1;
			} 
			k++;
			if( k == 8 )
			{
				k = 0;
				if( nGoodByteNum > 1000 )
					break; 
				lpH224HDLCOut[ nGoodByteNum++ ] = lpTemp[3];
			}							
		}  
	}

	//最后两个字节
	if( ( lpTemp[2] & 0x1f ) == 0x1f )
	{	
		(*lpnTemp) <<= 1;
		(*lpnTemp) &= 0xFFFE;
		nAdd1Num++;
		k++;
	}		

	if( k != 0 )                          
		lpH224HDLCOut[nGoodByteNum++] = lpTemp[3];

	lpH224HDLCOut[nGoodByteNum++] = lpTemp[2];
#endif
	//======================================

	//拷贝尾部0x7e数据，这部分数据不处理
	j = nStartPos + nNum;
	for( i = 0; i < nTotalNum - j + 1; i++ ) 
		*( lpH224HDLCOut + nGoodByteNum  + i ) = *( lpH224HDLC + j + i );       

	if( k !=0 )
	{
		for( i = 0; i < nTotalNum - j + 2; i++ )        
			MoveToPos( lpH224HDLCOut + i + nGoodByteNum - 2, lpH224HDLCOut + 1 + i + nGoodByteNum-2, 8-k );
	}

	return nAdd1Num;
}

//======================

void H323Channel::ReverseByte(BYTE *lpByte,int nNum, BYTE *lpRevTable)
{    
	int i;

	for(i=0; i<nNum; i++)
	{
		lpByte[i] = lpRevTable[lpByte[i]];
	}
}
void H323Channel::MoveToPos(LPBYTE lpForeByte, LPBYTE lpBackByte, int nPos)
{
	*lpForeByte = ( *lpForeByte << nPos ) + ( *lpBackByte >> ( sizeof(BYTE) * 8 - nPos ) );
}

void H323Channel::OnRecvAnnexQDataProc(unsigned char *pData, int nLen)
{
	RCMInfoTag rcmInfoTag;
	memset(&rcmInfoTag,0,sizeof(RCMInfoTag));

	BYTE pH224Buff[256];

	BYTE lpInputData[256];
	BYTE lpOutputData[256];
	int nInputLength = 0;
	int nOutputLength = 0;

	if(nLen > 100)
		return;

	nInputLength = nLen;
	memcpy(lpInputData, pData, nLen);
	nOutputLength = 0;

	//查找0x7E之间的有效数据
	SearchDataBetween7E( lpInputData, nInputLength, pH224Buff, g_lpRevTable, lpOutputData, &nOutputLength );
	if(nOutputLength <= 8 || nOutputLength > 128)
		return ;

	rcmInfoTag.m_nDstMcuNo = lpOutputData[3] ;
	rcmInfoTag.m_nDstTerNo = lpOutputData[4];
	rcmInfoTag.m_nSrcMcuNo = lpOutputData[5] ;
	rcmInfoTag.m_nSrcTerNo = lpOutputData[6] ;

	if(lpOutputData[1] == 0x61)
		rcmInfoTag.m_nPriority = LOW_PRIORITY ;	
	if(lpOutputData[1] == 0x71)
		rcmInfoTag.m_nPriority = HIGH_PRIORITY ;	

	if(lpOutputData[7] == 0x00/* && lpOutputData[8] == 0xC0*/)//CME,Reference Recommendation(H.224)
	{
		if(lpOutputData[9] == 0x00)//Null, does nothing
		{
		}
		else if(lpOutputData[9] == 0x01)//CME client list code
		{
		}
		else if(lpOutputData[9] == 0x02)//CME extra capabilities code
		{
			if(nOutputLength < 15)
				return ;
			memcpy(rcmInfoTag.m_bData, lpOutputData+12, nOutputLength-13);
			rcmInfoTag.DataLen = 3;
			rcmInfoTag.m_nType = TYPE_CAPABILITY;
		}
	}
	else if(lpOutputData[7] == 0x01/* && lpOutputData[8] == 0xC0*/)//Far End Camera Control,Reference Recommendation(H.281)
	{
		if( nOutputLength < 12 )
			return;
		memcpy(rcmInfoTag.m_bData, lpOutputData+9, nOutputLength-9);
		rcmInfoTag.DataLen = 3;
		rcmInfoTag.m_nType = TYPE_CONTROL;
	}
	else if(lpOutputData[7] == 0x7E)//Extended Client ID List,Reference Recommendation(H.224)
	{
	}
	else if(lpOutputData[7] == 0x7f)//Non-standard client,Reference Recommendation(H.224)
	{
		if(nOutputLength < 18)
			return;

		if(lpOutputData[12] == 0x05)
		{
			memcpy(rcmInfoTag.m_bData, lpOutputData+14, nOutputLength-14);
			rcmInfoTag.m_nType = TYPE_CAMERA_POSITION_STATUS;
		}
	}
	else
	{
		rcmInfoTag.DataLen = 0;
		return ;
	}

	DealwithCameraInfoH224ToH281(rcmInfoTag);
}

void H323Channel::DealwithCameraInfoH224ToH281(const RCMInfoTag&rcmInfoTag)
{
	int type;

	if(rcmInfoTag.DataLen == 0)
		return;

	type = rcmInfoTag.m_nType;
	if(type == TYPE_CONTROL)
	{
		DealWithCameraControl(rcmInfoTag);
	}
	else if(type == TYPE_CAPABILITY)
	{
		DealWithCameraCapability(rcmInfoTag);
	}
	else if(type == TYPE_CAMERA_POSITION_STATUS)
	{
		DealWithCameraPositionStatus(rcmInfoTag);
	}
	return;
}

void H323Channel::DealWithCameraPositionStatus(const RCMInfoTag&rcmInfoTag)
{
	return;
}

void H323Channel::DealWithCameraCapability(const RCMInfoTag&rcmInfoTag)
{
	//BYTE bH281Info[MAX_CAMERAINFO_LENGTH];
	//int nLen;
	//int i = 0;
	//memset(bH281Info, 0, MAX_CAMERAINFO_LENGTH);
	//nLen = rcmInfoTag.DataLen;	
	//if( nLen == 0 )
	//	return;
	//memcpy(bH281Info, rcmInfoTag.m_bData, nLen);

	//BYTE PositionNum = bH281Info[0] & 0x0f;//摄像头预置支持的最大个数

	//int nDevID, nIndex;
	//BYTE VideoId = 1;//远端视频源ID

	//nIndex = 1;
	//nDevID = bH281Info[nIndex] >> 4;
	//if( ( (bH281Info[0] & 0x10) != 0x10 ) && ( nIndex == 1 ) )
	//	VideoId = nDevID;

	//
}

void H323Channel::DealWithCameraControl(const RCMInfoTag&rcmInfoTag)
{
	const unsigned char*pH281data=rcmInfoTag.m_bData;
	if( rcmInfoTag.DataLen == 0 )
		return;

	switch( pH281data[0] )
	{
	case H281_START:
		OnH281Start(pH281data[1], pH281data[2] & 0x0f);
		break;

	case H281_CONTINUE:
		OnH281Continue(pH281data[1]);
		break;

	case H281_STOP:
		OnH281Stop(pH281data[1]);
		break;

	case H281_SELECT_VIDEOSRC:
		//远端设置本端的视频源
		OnSelectVideoSrc(pH281data[1]);
		break;

	case H281_SWITCH_VIDEOSRC://远端通知它当前的视频源
		OnSwitchVideoSrc(pH281data[1], rcmInfoTag.m_nSrcMcuNo, rcmInfoTag.m_nSrcTerNo);
		break;

	case H281_STORE_PRESET:
		//远端对本端进行摄像头预置(设置)
		OnCameraPresetStore(pH281data[1]);
		break;

	case H281_ACTIVE_PRESET:
		//远端对本端进行摄像头预置(激活)
		OnCameraPresetActive(pH281data[1]);
		break;

	default:
		//	printf("DealWithCameraControl: unknown type!\n");
		break;
	}
}

void H323Channel::OnH281Start(BYTE bDirection, int nPeriod)
{
	switch(bDirection)
	{
	case H281_UP:
		m_rH323Call.OnRemoteCameraUp();
		break;
	case H281_DOWN:
		m_rH323Call.OnRemoteCameraDown();
		break;
	case H281_RIGHT:
		m_rH323Call.OnRemoteCameraRight();
		break;
	case H281_LEFT:
		m_rH323Call.OnRemoteCameraLeft();
		break;
	case H281_ZOOMIN:
		m_rH323Call.OnRemoteCameraZoomIn();
		break;
	case H281_ZOOMOUT:
		m_rH323Call.OnRemoteCameraZoomOut();
		break;

	case H281_FOCUSNEAR:
		break;

	case H281_FOCUSFAR:
		break;

	default:
		break;
	};
}

void H323Channel::OnH281Continue(BYTE bDirection)
{	
	OnH281Start(bDirection, 0);
}

void H323Channel::OnH281Stop(BYTE bDirection)
{
	//判断本端是否允许远遥
	if((bDirection&H281_UP) == H281_UP)
	{
		m_rH323Call.OnRemoteCameraStopUp();
	}
	if((bDirection&H281_DOWN) == H281_DOWN)
	{
		m_rH323Call.OnRemoteCameraStopDown();
	}
	if((bDirection&H281_LEFT) == H281_LEFT)
	{
		m_rH323Call.OnRemoteCameraStopLeft();
	}
	if((bDirection&H281_RIGHT) == H281_RIGHT)
	{
		m_rH323Call.OnRemoteCameraStopRight();
	}
	if((bDirection&H281_ZOOMIN) == H281_ZOOMIN)
	{
		m_rH323Call.OnRemoteCameraStopZoomIn();
	}
	if((bDirection&H281_ZOOMOUT) == H281_ZOOMOUT)
	{
		m_rH323Call.OnRemoteCameraStopZoomOut();
	}
}

#define MAX_VIDEO_NUM                       5	//本地最多5个视频源
#define H281_VID_CHG1 0x10
#define H281_VID_CHG2 0x20
#define H281_VID_CHG3 0x30
#define H281_VID_CHG4 0x40
#define H281_VID_CHG5 0x50
#define H281_VID_CHG6 0x60
//远端设置本端视频源
//视频源ID:
//	0	Default - Current Video Source.
//	1	Main Camera.
//	2	Auxiliary Camera. 
//	3	Document Camera. 
//	4 	Auxiliary Document Camera.
//	5	Video Playback Source (such as a Video Cassette Recorder).
void H323Channel::OnSelectVideoSrc(BYTE bDirection)
{
	if(bDirection > MAX_VIDEO_NUM) return;
	switch(bDirection&0x0f0)//其实就是bDirection>>4的值，指定的视频源ID
	{
	case H281_VID_CHG1:
		m_CurVideoID = 1;
		break;

	case H281_VID_CHG2:
		m_CurVideoID = 2;
		break;

	case H281_VID_CHG3:
		m_CurVideoID = 3;
		break;

	case H281_VID_CHG4:
		m_CurVideoID = 4;
		break;

	case H281_VID_CHG5:
		m_CurVideoID = 5;
		break;

	default:
		break;
	}
//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	m_rH323Call.OnRemoteCameraControl(this,"SourceSelect",m_CurVideoID);

#else

	m_rH323Call.OnRemoteSourceSelect(m_CurVideoID);
#endif

}

//远端视频发生变化
void H323Channel::OnSwitchVideoSrc(BYTE bDirection, BYTE bMcuNo, BYTE bTerNo)
{
	BYTE VideoId = bDirection>>4;//远端视频源ID
//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	m_rH323Call.OnRemoteCameraControl(this,"SourceSwitched",VideoId);

#else
	m_rH323Call.OnRemoteSourceSwitched(VideoId);
#endif

}

void H323Channel::OnCameraPresetStore(BYTE bDirection)
{
	//往摄像头控制模块发送预置设置消息
	//预置点为： (bDirection >> 4) & 0xFF;
	int seat;
	seat = (bDirection >> 4) & 0xFF;
//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	m_rH323Call.OnRemoteCameraControl(this,"PresetStore",seat);

#else
	m_rH323Call.OnRemoteCameraPresetStore(seat);
#endif

}

void H323Channel::OnCameraPresetActive(BYTE bDirection)
{
	//往摄像头控制模块发送预置激活消息
	//预置点为： (bDirection >> 4) & 0xFF;
	int seat;
	seat = (bDirection >> 4) & 0xFF;
//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	m_rH323Call.OnRemoteCameraControl(this,"PresetActive",seat);

#else
	m_rH323Call.OnRemoteCameraPresetActive(seat);
#endif

}

int H323Channel::SearchDataBetween7E(BYTE* lpData, int nNum, BYTE* lpDataBuff, BYTE* lpRevTableTemp, BYTE* lpOutputData, int* nOutputNum)
{
	int nMoveBit = 0;
	int n7EsInd = 0;
	int nBetween7EsNum = 0;
	BYTE bForeByte = 0;	
	int nForeMoveBit = 0;
	int i = 0;
	int j = 0;
	int nDelBitInCut0 = 0;
	BYTE bKeep = 0;

	for(i = 0; i < nNum; i++)
	{
		if((nMoveBit = Find7EInData(lpData[i], &bKeep)) == 0)
		{
			if(n7EsInd != 0)
			{
				lpDataBuff[ nBetween7EsNum++ ] = lpData[i];
				if(nBetween7EsNum > 1000)
				{
					nBetween7EsNum = 0;
					n7EsInd = 0;
				}				
			}      
		}
		else
		{
			lpDataBuff[nBetween7EsNum++] = lpData[i];
			if(n7EsInd != 0)
			{
				if(nBetween7EsNum > 10)
				{                             
					for(j=0; j<nBetween7EsNum; j++)
					{
						MoveToPos(lpDataBuff+j, lpDataBuff+j+1, nForeMoveBit);
					}
					nDelBitInCut0 = Del0FromHDLC(lpDataBuff, nBetween7EsNum);
					nBetween7EsNum -= (nDelBitInCut0+nForeMoveBit+7)/8;
					ReverseByte(lpDataBuff, nBetween7EsNum, lpRevTableTemp);
					if( lpDataBuff[0] == 0x00 && lpDataBuff[2] == 0x03 
						&&( lpDataBuff[1] == 0x61 || lpDataBuff[1] == 0x71|| 
						lpDataBuff[1] == 0x70 || lpDataBuff[1] == 0x60 ) )						
					{	 
						if(nBetween7EsNum < 100)
						{
							memcpy(lpOutputData, lpDataBuff, nBetween7EsNum-1);
							*nOutputNum = nBetween7EsNum-1;	
						}
					}   
				} 
				nBetween7EsNum = 0;
				lpDataBuff[nBetween7EsNum++] = lpData[i];
			}                  
			n7EsInd = 1;
			nForeMoveBit = nMoveBit;
		}                         
		bForeByte = lpData[i];
	}
	return 1;
}                                             

int H323Channel::Find7EInData(BYTE bInPut, BYTE* bKeep)
{
	int i = 0;
	int nWhichBit = 0;
	unsigned short int uMoveBuffer = 0;
	LPBYTE lpFore = 0;
	LPBYTE lpBack = 0;

	lpBack = (LPBYTE)(&uMoveBuffer);
	lpFore = lpBack+1;

	nWhichBit = 0;
	*lpFore = *bKeep;//
	*lpBack = bInPut;
	for(i=0; i<8; i++)
	{                  
		uMoveBuffer <<= 1;
		if(*lpFore == 0x7e)
		{
			nWhichBit = i+1;
		}
	}

	*bKeep = *lpFore;//

	return nWhichBit;
}       

//HDLC中，如果连续5个1，则在其后插入1个0。因此当发现连续5个1后面跟着一个0，则需将该0去除
int H323Channel::Del0FromHDLC(LPBYTE lpH224HDLC, int nNum)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	int nDel1Num = 0;
	int nGoodByteNum = 0;
	BYTE lpTemp[4] = {0, 0, 0, 0};
	unsigned long* lplTemp = NULL;

	lplTemp = (unsigned long*)lpTemp;
	nGoodByteNum = 0;
	k = -8;      
	l = 0; 

#if 0
	for(i=0; i<=nNum; i++)
	{          
		if(i < nNum)
		{
			lpTemp[2] = lpH224HDLC[i];
		}

		for(j=0; j<8; j++)
		{
			if(((lpTemp[1]&0x1f) == 0x1f) && ((lpTemp[2]&0x80) != 0x80) && l >= 0)
			{
				lpTemp[2] <<= 1;
				nDel1Num++;
				l = -5;
			}
			else
			{
				*lplTemp <<= 1;
				k++;
				l++;
			}
			if( k == 8 )
			{
				k = 0; 
				lpH224HDLC[nGoodByteNum++] = lpTemp[0];
			}							
		}  
	}
#else
	for(i=0; i<=nNum; i++)
	{          
		if(i < nNum)
		{
			lpTemp[1] = lpH224HDLC[i];
		}

		for(j=0; j<8; j++)
		{
			if(((lpTemp[2]&0x1f) == 0x1f) && ((lpTemp[1]&0x80) != 0x80) && l >= 0)
			{
				lpTemp[1] <<= 1;
				nDel1Num++;
				l = -5;
			}
			else
			{
				*lplTemp <<= 1;
				k++;
				l++;
			}
			if( k == 8 )
			{
				k = 0; 
				lpH224HDLC[nGoodByteNum++] = lpTemp[3];
			}							
		}  
	}
#endif
	return nDel1Num;
}

int& H323Channel::H264Level(void)
{
	return m_nH264Level;
}

void H323Channel::SetH264CapOptionParam(int type)
{
	if(type ==0)
	{
		m_nRemoteOLCMaxMBPS =m_rH323Call.m_nRemoteMainVideoOLCMaxMBPS;
		m_nRemoteOLCMaxFS = m_rH323Call.m_nRemoteMainVideoOLCMaxFS;
		m_nRemoteOLCMaxDPB = m_rH323Call.m_nRemoteMainVideoOLCMaxDPB;
		m_nRemoteOLCMaxBR =m_rH323Call.m_nRemoteMainVideoOLCMaxBR;
	}
	else if(type ==1)
	{
		m_nRemoteOLCMaxMBPS = m_rH323Call.m_nRemoteDualVideoOLCMaxMBPS;
		m_nRemoteOLCMaxFS = m_rH323Call.m_nRemoteDualVideoOLCMaxFS;
		m_nRemoteOLCMaxDPB = m_rH323Call.m_nRemoteDualVideoOLCMaxDPB;
		m_nRemoteOLCMaxBR = m_rH323Call.m_nRemoteDualVideoOLCMaxBR;
	}
}

VIDEO_FORMAT_TYPE H323Channel::GetVideoFormatTypeFromMaxBitrate()
{
	VIDEO_FORMAT_TYPE VideoFormatType =VIDEO_FORMAT_QCIF;
	int div=1;
	if(m_VideoCodecType == VIDEC_CODEC_H264_SVC) div=2;

	//from frame calc multiple 
	int nTheMaxBitrate =m_nMaxBitrate;
	float fFrameRate= 30.0;
	float fFrameMultiple=1.0;
	float fMaxFrame=30.0;
	float fBitrate=nTheMaxBitrate;
	if(m_nRemoteOLCMaxMBPS >0 && m_nRemoteOLCMaxFS >0) fFrameRate = (m_nRemoteOLCMaxMBPS *500 ) / ( m_nRemoteOLCMaxFS * 256 );
	fFrameMultiple=fFrameRate / fMaxFrame;
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

void H323Channel::SetVideoFormatTypeFromMaxFSBitrate(int nType) //0 terminal use 1 mcu mp use
{
	//20141031 ??
	return;
}

VIDEO_FORMAT_TYPE H323Channel::GetCompareVideoFormatType(VIDEO_FORMAT_TYPE nType1,VIDEO_FORMAT_TYPE nType2,int nType)
{
	int nValue[2]={0};
	VIDEO_FORMAT_TYPE formatType=nType1;
	for(int i=0;i<2;i++)
	{
		if(i==1) formatType=nType2;
		switch(formatType)
		{
		case VIDEO_FORMAT_QCIF:
			nValue[i]= 0;
			break;
		case VIDEO_FORMAT_CIF:
			nValue[i]= 1;
			break;
		case VIDEO_FORMAT_4CIF:
			nValue[i]= 3;
			break;
		case VIDEO_FORMAT_16CIF:
			nValue[i]= 10;
			break;
		case VIDEO_FORMAT_720I:
			nValue[i]= 4;
			break;
		case VIDEO_FORMAT_720P:
			nValue[i]= 7;
			break;
		case VIDEO_FORMAT_1080I:
			nValue[i]= 8;
			break;
		case VIDEO_FORMAT_1080P:
			nValue[i]= 11;
			break;
		case VIDEO_FORMAT_QVGA:
			nValue[i]= 0;
			break;
		case VIDEO_FORMAT_VGA:
			nValue[i]= 2;
			break;
		case VIDEO_FORMAT_SVGA:
			nValue[i]= 5;
			break;
		case VIDEO_FORMAT_XGA:
			nValue[i]= 6;
			break;
		case VIDEO_FORMAT_SXGA:
			nValue[i]= 9;
			break;
		default:
			nValue[i]= 0 ;
			break;
		}
	}

	if(nType ==0) formatType = (nValue[0] >= nValue[1] ) ? nType1 : nType2 ;
	else if(nType ==1)
	{
		formatType = (nValue[0] <= nValue[1] ) ? nType1 : nType2 ;
		if(formatType == 0) formatType = (nValue[0] >= nValue[1] ) ? nType1 : nType2 ;
	}
	return formatType;
}

void H323Channel::GetVideoFormatType(int & w,int & h)
{
	switch(m_VideoFormatType)
	{
	case VIDEO_FORMAT_QCIF:
		w=176,h=144;
		break;
	case VIDEO_FORMAT_CIF:
		w=352,h=288;
		break;
	case VIDEO_FORMAT_4CIF:
		w=704,h=576;
		break;
	case VIDEO_FORMAT_16CIF:
		w=1408,h=1152;
		break;
	case VIDEO_FORMAT_720I:
		w=1280,h=360;
		break;
	case VIDEO_FORMAT_720P:
		w=1280,h=720;
		break;
	case VIDEO_FORMAT_1080I:
		w=1920,h=540;
		break;
	case VIDEO_FORMAT_1080P:
		w=1920,h=1080;
		break;
	case VIDEO_FORMAT_QVGA:
		w=320,h=240;
		break;
	case VIDEO_FORMAT_VGA:
		w=640,h=480;
		break;
	case VIDEO_FORMAT_SVGA:
		w=800,h=600;
		break;
	case VIDEO_FORMAT_XGA:
		w=1024,h=768;
		break;
	case VIDEO_FORMAT_SXGA:
		w=1280,h=1024;
		break;
	default:
		w=0,h=0;
		break;
	}
}

HCHAN H323Channel::GetChannelHsChan(void)
{
	return m_hsChan;
}

void H323Channel::CloseChannel(void)
{
	cmChannelDrop(m_hsChan);
	m_rH323Call.OnChannelDisconnected(this);
	m_hsChan=NULL;
}

char * H323Channel::UpdateChannelSpecifiedKey(void)
{
	char szChannelID[128];
	sprintf(szChannelID, "Secretyek%u%uSecretyek%u%u", m_nSecretUpdateTimes,(unsigned long)this, m_nSecretUpdateTimes,(unsigned long)this);
	if(m_nSecretUpdateTimes >0 || m_nSecretUpdateTimes > m_nSecretUpdateMaxTimes)
	{
		memset(m_strSpecifiedSecretOLC,0,sizeof(m_strSpecifiedSecretOLC));
		memcpy(m_strSpecifiedSecretOLC,m_strSpecifiedSecretUpdate,sizeof(m_strSpecifiedSecretUpdate));
	}
	memset(m_strSpecifiedSecretUpdate,0,sizeof(m_strSpecifiedSecretUpdate));
	strcpy(m_strSpecifiedSecretUpdate,szChannelID);
	return m_strSpecifiedSecretUpdate;
}

void H323Channel::SetChannelUpdateSharedSecret(unsigned char * secret,int len)
{
	if(m_nSecretUpdateTimes >0 || m_nSecretUpdateTimes > m_nSecretUpdateMaxTimes)
	{
		memset(m_strSharedSecretOLC,0,sizeof(m_strSharedSecretOLC));
		memcpy(m_strSharedSecretOLC,m_strSharedSecretUpdate,sizeof(m_strSharedSecretUpdate));
	}
	if(m_nSecretUpdateTimes >m_nSecretUpdateMaxTimes) m_nSecretUpdateTimes=0;

	memset(m_strSharedSecretUpdate,0,sizeof(m_strSharedSecretUpdate));
	memcpy(m_strSharedSecretUpdate,secret,len);
	m_bSecretUpdate=true;

	m_nSecretUpdateTimes++;
}

void H323Channel::ChangeChannelPayloadType()
{
	int temp=m_nPayloadType;
	m_nPayloadType=m_nPayloadTypeUpdate;
	m_nPayloadTypeUpdate=temp;
}

int H323Channel::GetChannelPayloadType()
{
	return m_nPayloadType;
}

void H323Channel::SetChannelUpdatePayloadType(int updatePT,int type)
{
	if(type >0 )
	{
		m_nPayloadTypeUpdate =m_nPayloadType;
		m_nPayloadType=updatePT;
	}
	else m_nPayloadTypeUpdate=updatePT;
}

void H323Channel::AddChannelEncryption()
{
	char *p=NULL;
	if(m_rH323Call.GetMgrH235MediaEncrypSpecified() ==1) 
		p=m_rH323Call.GetMgrH235MediaEncrypSpecifiedPWD();
	else p=m_strSharedSecretOLC;

	if(m_nSecretUpdateTimes >0 ) p=m_strSharedSecretUpdate;
	//add notice two direction not support
	if(m_nDirection==cmCapTransmit)
	{
		m_DesEncryption.EncryptionInit(this->m_nAlgorithmId);
		if(m_nSecretUpdateTimes <= 0 )
			m_DesEncryption.SetEncryption(m_hRTP,p,p);   //因共用RTP此仅创建
		m_DesEncryption.SetEncryption(m_hRTP,p,NULL);//重新赋值
	}
	else if(m_nDirection==cmCapReceive)
	{
		m_DecDesEncryption.EncryptionInit(this->m_nAlgorithmId);
		if(m_nSecretUpdateTimes <= 0 )
		m_DecDesEncryption.SetEncryption(m_hRTP,p,p);   //因共用RTP此仅创建
		m_DecDesEncryption.SetEncryption(m_hRTP,NULL,p);//重新赋值
	}

	//char msg[1024]={0};
	//sprintf(msg,"H323Channel:direc %s,meida %d name %s algoID %d shareori %s shareup %s \n",
	//	m_nDirection==1 ? "Recv":"Send",m_nMediaType,m_strChannelName.c_str(),m_nAlgorithmId,m_strSharedSecretOLC,m_strSharedSecretUpdate);
	//OutputDebugString(msg);
	char lastkey[64]={0};
	ConvertCharToHexChar((unsigned char *)lastkey,(unsigned char *)m_strSharedSecretOLC,16);
}

void H323Channel::AddChannelEncryptionAlgoChange()
{
	char *p=NULL;
	char *pRelated=NULL;
	if(m_rH323Call.GetMgrH235MediaEncrypSpecified() ==1) 
		pRelated=p=m_rH323Call.GetMgrH235MediaEncrypSpecifiedPWD();
	else p=m_strSharedSecretOLC;

	if(m_nSecretUpdateTimes >0 ) p=m_strSharedSecretUpdate;

	H323Channel* pRelatedChannel=m_rH323Call.GetRelatedChannel(this);
	if(pRelatedChannel ==NULL) return;
	pRelated=pRelatedChannel->m_strSharedSecretOLC;
	if(pRelatedChannel->m_nSecretUpdateTimes >0 )
		pRelated=m_strSharedSecretUpdate;

	//add notice two direction not support
	if(m_nDirection==cmCapTransmit)
	{
		m_DesEncryption.EncryptionEnd();
		m_DesEncryption.EncryptionInit(this->m_nAlgorithmIdUpdate);
		m_DesEncryption.SetEncryption(m_hRTP,p,p);   //因共用RTP此仅创建
		m_DesEncryption.SetEncryption(m_hRTP,p,NULL);//重新赋值
		pRelatedChannel->m_DecDesEncryption.SetEncryption(m_hRTP,NULL,pRelated);//重新赋值
	}
	else if(m_nDirection==cmCapReceive)
	{
		m_DecDesEncryption.EncryptionEnd();
		m_DecDesEncryption.EncryptionInit(this->m_nAlgorithmIdUpdate);
		m_DecDesEncryption.SetEncryption(m_hRTP,p,p);   //因共用RTP此仅创建
		m_DecDesEncryption.SetEncryption(m_hRTP,NULL,p);//重新赋值
		pRelatedChannel->m_DesEncryption.SetEncryption(m_hRTP,pRelated,NULL);//重新赋值
	}
	m_nAlgorithmId = m_nAlgorithmIdUpdate;
}

bool & H323Channel::EncryptionSecretStatus(void)
{
	return m_bEncryptionSecret;
}

void H323Channel::SetKeepAliveAddress(unsigned long ip, unsigned short port)
{
	if(!m_bGetKeepAlive)
	{
		m_ulKeepAliveIP = ip;
		m_nKeepAlivePort = port;
		m_bGetKeepAlive=true;
	}
}

void H323Channel::SendChannelKeepAlive(void)
{
	int nSendlen =0;
	int len=12;
	char keep[16]={'\n'};
	if(m_pFWNAT_UDP)
	{
		nSendlen =m_pFWNAT_UDP->SendData(keep,len);
		//char msg[128]={0};
		//sprintf(msg,"Channel SendChannelKeepAlive,FWNAT_UDP:%p,len:%d \n",m_pFWNAT_UDP,nSendlen);
		//OutputDebugString(msg);
	}
}

void H323Channel::SendAnnexQDataNETEC(char* buff, int datalen)
{
	if(m_hRTP==NULL) return;
	rtpParam p;
	p.len = datalen;
	rtpUnpack(m_hRTP, buff, datalen, &p);
	p.payload=this->PayloadType();
	RvRtpPackExCustom(m_hRTP, p.sSrc,(RvUint8*)buff, datalen,&p);
	m_rH323Call.NETECSendPTZ(buff,datalen);
}

//add medianetrtp channel 0615
void H323Channel::MediaNetRtpChannelOpen(int nDir)
{
	if(m_pMediaNet_RTP)
	{
		int nMaxFrameSize =0;
		int nNetBand =0;
		int nFrameRate=0;
		int nMediaType=0;
		int nCodecType=0;
		if(m_nMediaType == Enum_Channel_Media_Audio)
		{
			nMaxFrameSize=320;
			nNetBand =64*1024*1.5;
			nFrameRate=30;
			nMediaType=1;
			nCodecType=m_AudioCodecID;
		}
		else if(m_nMediaType == Enum_Channel_Media_Video || m_nMediaType == Enum_Channel_Media_DualVideo)
		{
			nMaxFrameSize=500<<10;
			nNetBand=6<<20;
			//if(m_nRemoteOLCMaxMBPS >0 && m_nRemoteOLCMaxFS >0 ) nFrameRate = (m_nRemoteOLCMaxMBPS *500 ) / ( m_nRemoteOLCMaxFS * 256 );
			//else nFrameRate =30;
			nFrameRate =m_nVideoFrameRate;
			nMediaType=2;
			if(m_nMediaType == Enum_Channel_Media_DualVideo) nMediaType=3;

			if(nDir ==2)
			{
				//notice the frame rate set is use gui set ,modify 20140627 
				//if option is ,the terminal called frame set use reference caller bandwidth 
				//if(! (m_nEndpontType ==0 && m_rH323Call.m_nCallDir == IMXEC_CALL_DIR_IN) )
				{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

					nFrameRate=m_rH323Call.GetVideoCapFrameParam(m_VideoCodecType,m_VideoFormatType,m_nMediaType);
#else

					nFrameRate = H323CallMgr::Instance().m_nMainVideoDevFrameRate;
					if(m_nMediaType == Enum_Channel_Media_DualVideo)
					{
						if(nDir ==2) nFrameRate = H323CallMgr::Instance().m_nSecondVideoDevFrameRate;
					}
#endif

				}

			}
			nCodecType=m_VideoCodecType;
			
		}
		else if(m_nMediaType == Enum_Channel_Media_H224)
		{
			nMaxFrameSize=320;
			nNetBand =64*1024*1.5;
			nFrameRate=30;
			nMediaType=4;
			nCodecType=DATA_CODEC_H224;
		}

		if(nFrameRate >60 ) nFrameRate=60; //notice the set is hpdatatrans.lib control
		m_pMediaNet_RTP->InitRtpParam2(nMaxFrameSize,nNetBand, nFrameRate,nMediaType,nCodecType,0,nDir);

		//notice 呼通后的过程中更新密钥暂不支持
		char * pKeyBuf =NULL;
		if(EncryptionStatus()==true)
		{
			if(m_rH323Call.GetMgrH235MediaEncrypSpecified() ==1) 
				pKeyBuf=m_rH323Call.GetMgrH235MediaEncrypSpecifiedPWD();
			else pKeyBuf=m_strSharedSecretOLC;
			if(m_nSecretUpdateTimes >0 ) pKeyBuf=m_strSharedSecretUpdate;
		}

		bool bGet=m_rH323Call.GetMediaRtpHandle(m_pMediaNet_RTP);
		//std::string strLocalIP =""; int nPort=0;
		//m_rH323Call.GetCallH245LocalSignalAddress(strLocalIP,nPort);
		if ( (m_pMediaNet_RTP->Open(m_rH323Call.m_strLocalIP.c_str(),m_nLocalRTPPort,m_nPayloadType,m_strChannelName.c_str(),nDir,pKeyBuf,m_nAlgorithmId)!=0 ) && (bGet==false) )
		{
			m_pMediaNet_RTP->Close();
			delete m_pMediaNet_RTP;
			m_pMediaNet_RTP=NULL;
		}
	}

	if( m_pMediaNet_RTP )
	{
		if(! m_rH323Call.m_bMediaMulticastEnable)
		{
			int nFlagReliable =0;
			if(m_rH323Call.HasAVCONTransport()) nFlagReliable =1;
			//if(H323CallMgr::Instance().m_nProxyUseFEC ==0 ) nFlagReliable =0; //notice fec use ?
			unsigned long remoteIP=m_ulRTPRemoteIP;
			if(remoteIP <=0) remoteIP=m_ulRTCPRemoteIP;
			struct in_addr addr;
			addr.S_un.S_addr =remoteIP;
			std::string strRemoteIP=inet_ntoa(addr);
			m_pMediaNet_RTP->AddRemote(strRemoteIP.c_str(),m_nRTPRemotePort,NULL,NULL,NULL,0,0, nFlagReliable,m_nRTCPRemotePort,nDir);
		}
		else
		{
		}

	}

}

void H323Channel::MediaNetRtpChannelConnected(cmChannelState_e state,cmChannelStateMode_e stateMode)
{
	m_nFlowCCBitRate = 1;//flowControlCommand add
	//m_nMsdStatus = cmCallMasterSlaveStatus(m_rH323Call.m_hsCall);
	m_nAlgorithmIdUpdate = m_nAlgorithmId;
	if(m_bEncryptionSecret && ( m_bH235OLCCap || m_bH235Cap ) ) EncryptionStatus()=true;

	if (state!=m_nChannelState)
	{
		if (m_pMediaNet_RTP==NULL)
		{
			m_pMediaNet_RTP=(MediaNet_RTP *) m_rH323Call.GetCreateMediaRtpHandle(m_nLocalRTPPort);
			if(m_pMediaNet_RTP ==NULL)
			{
				std::string strMediaName="MediaChannel";
				if(m_nMediaType == Enum_Channel_Media_Audio) strMediaName="AUDIOChannel";
				else if(m_nMediaType == Enum_Channel_Media_Video) strMediaName="VIDEOChannel";
				else if(m_nMediaType == Enum_Channel_Media_DualVideo) strMediaName="DUALVIDEOChannel";
				else if(m_nMediaType == Enum_Channel_Media_H224) strMediaName="DATAChannel";
				m_pMediaNet_RTP = new MediaNet_RTP(* this,NULL,0,0,0);
				if(m_pMediaNet_RTP)
				{
					m_rH323Call.SaveMediaRtpHandle(m_pMediaNet_RTP,m_nLocalRTPPort);
				}
			}
			//notice cmCapReceive 1 ,cmCapTransmit 2 ,cmCapReceiveAndTransmit 3
			if(m_pMediaNet_RTP)
			{
				MediaNetRtpChannelOpen(m_nDirection);
				m_pMediaNet_RTP->SendRecvStart(m_nDirection,this);
			}
		}

		m_nChannelState=state;
		m_rH323Call.OnChannelConnected(this);
		m_rH323Call.OnChannelMediaEncryptionState(this);

		if(m_pMediaNet_RTP && m_bGetKeepAlive) OpenKeepAliveChannel();
	}

	if (stateMode==cmChannelStateModeOn)
	{

	}
}

//打开发送打洞包
void H323Channel::OpenKeepAliveChannel(void)
{
	if(m_bGetKeepAlive)
	{
		struct in_addr addr;
		addr.S_un.S_addr =m_ulKeepAliveIP;
		std::string strRemoteIP=inet_ntoa(addr);
		int nLocalPort=m_nLocalRTPPort; 
		if ( m_ulKeepAliveIP != 0 )
		{
			CDataUDP * pFWNAT_UDP =m_pFWNAT_UDP;
			if(pFWNAT_UDP==NULL)
			{
				pFWNAT_UDP=new CDataUDP();
				if(pFWNAT_UDP)
				{
					if(true != pFWNAT_UDP->Open(m_rH323Call.m_strLocalIP.c_str(),nLocalPort,strRemoteIP.c_str(),m_nKeepAlivePort) )
					{
						pFWNAT_UDP->Close();
						delete pFWNAT_UDP;
						pFWNAT_UDP=NULL;
					}
				}
				m_pFWNAT_UDP =pFWNAT_UDP;
			}
			SendChannelKeepAlive();
		}
	}
}

void H323Channel::OnMediaNet_RTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	if (m_pMediaNet_RTP)
	{
		nPayloadType =m_nPayloadType; //notice set pt
		m_rH323Call.OnChannelRecvdRTPPacket(this,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
}

void H323Channel::MediaNetRtpChannelDisconnected(void)
{
	MediaNet_RTP * pMediaNet_RTP = m_pMediaNet_RTP;
	m_pMediaNet_RTP=NULL;
	if(pMediaNet_RTP) pMediaNet_RTP->SendRecvPause(m_nDirection);

	if (m_hsChan!=NULL)
	{
		//notice the not add 0718
		//cmChannelClose(m_hsChan);
		m_hsChan=NULL;
	}

	m_rH323Call.OnChannelDisconnected(this);
	if(m_nChannelState == cmChannelStateConnected)
	{
		//H323Channel* pChannel=NULL;
		m_rH323Call.RelatedMediaChannelCloseRTP(pMediaNet_RTP,this);
		////add notice test
		//if(pChannel) pChannel->m_pMediaNet_RTP =NULL;
	}

	if(EncryptionStatus()==true)
	{
		//m_DesEncryption.EncryptionEnd();
		EncryptionStatus()=false;
	}

	UDPObjectClose(CDataUDP * ,m_pFWNAT_UDP) 

	m_bGetKeepAlive =false;
}

void H323Channel::ChannelRTPClose(void * hRtp)
{

	if(hRtp !=NULL)
	{
		LogFile(NULL,NULL,"","","ChannelRTPClose         111");

		if(m_bUseMediaNetRTP)
		{
			MediaNet_RTP * pMediaNet_RTP =(MediaNet_RTP * )hRtp;
			//UDPObjectClose(MediaNet_RTP * ,pMediaNet_RTP) ;
			//OutputDebugString("MediaNetRtpDelete Close begin \n");
			pMediaNet_RTP->Close();
			//OutputDebugString("MediaNetRtpDelete Close end 1 \n");
			delete pMediaNet_RTP;
			pMediaNet_RTP=NULL;
			//OutputDebugString("MediaNetRtpDelete Close end 2 \n");
		}
		else
		{
			HRTPSESSION hRTPSession =(HRTPSESSION ) hRtp;
			rtpClose(hRTPSession);
		}

		LogFile(NULL,NULL,"","","ChannelRTPClose         222");

		//port save
		H323CallMgr::Instance().SaveMediaReleasePort(m_nLocalRTPPort);
	}
}

void H323Channel::MediaNetRtpSendData(char* buff, int datalen)
{
	if (m_pMediaNet_RTP)
	{
		m_pMediaNet_RTP->SendRTPPacket(NULL,0,m_nPayloadType,0,0,(unsigned char *)buff,datalen);
	}
}

void H323Channel::ChannelRTPPause(void)
{
	if(m_pMediaNet_RTP) m_pMediaNet_RTP->SendRecvPause(m_nDirection);
}

//ulTotalPackets 总共发送了多少个包 ulLostPackets 总共丢弃了多少个包AvgPacketLossRate 平均丢包率 CurPacketLossRate 当前丢包率
void H323Channel::GetRecvStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if(m_bUseMediaNetRTP && m_pMediaNet_RTP)
	{
		THpRcvStatistics tHpRcvStatistics;
		memset(&tHpRcvStatistics,0,sizeof(tHpRcvStatistics));
		m_pMediaNet_RTP->GetRecvStatistics(tHpRcvStatistics );
		unsigned int nTotals =tHpRcvStatistics.m_dwPackNum +tHpRcvStatistics.m_dwPhysicalPackLose;
		double nRate = 0.0;
		if(nTotals >0) nRate =tHpRcvStatistics.m_dwPhysicalPackLose /nTotals ;

		if(ulLostPackets <=0) ulLostPackets =tHpRcvStatistics.m_dwPhysicalPackLose;
		if(ulTotalPackets <=0) ulTotalPackets = tHpRcvStatistics.m_dwPackNum;
		if(AvgPacketLossRate <= 0.000001) AvgPacketLossRate = nRate;
		if(CurPacketLossRate <= 0.000001) CurPacketLossRate = nRate;
	}
}

//ulTotalPackets 总共发送了多少个包 ulLostPackets 总共丢弃了多少个包AvgPacketLossRate 平均丢包率 CurPacketLossRate 当前丢包率
void H323Channel::GetSendStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if(m_bUseMediaNetRTP && m_pMediaNet_RTP)
	{
		THpSndStatistics tHpSndStatistics;
		memset(&tHpSndStatistics,0,sizeof(tHpSndStatistics));
		m_pMediaNet_RTP->GetSendStatistics(tHpSndStatistics );
		if(ulTotalPackets <=0) ulTotalPackets = tHpSndStatistics.m_dwPackSendNum;
	}
}
