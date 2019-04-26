#include "stdafx.h"
#include "H323CallMgr.h"
#include "stkHandler.h"
#include "H323Channel.h"
//#include "CallMgr.h"
#include "XSocket.h"
#include "XSocketUDP.h"
#include "XUtil.h"
#include <NETEC/NETEC_Core.h>
#include <RVINC/ci.h>
#include <RVINC/rvrtplogger.h>
#include <RVINC/msg.h>
#include <platform/ipc.h>
#include <protocol/hpdatatrans.h>
#include <Sensapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DES_OID       (char *)"1 3 14 3 2 7"				//DES-56 CBC模式
#define DES3_OID      (char *)"1 3 14 3 2 17"				//DES-168 CBC模式
#define AES_OID       (char *)"2 16 840 1 101 3 4 1 2"		//AES-128 CBC模式
#define AES256_OID       (char *)"2 16 840 1 101 3 4 1 42"	//AES-256 CBC模式

static H323CallMgr g_H323CallMgr;

extern unsigned short GetRTPPort(unsigned short usRTPBasePort);
extern unsigned char ConfigurationArray[];
extern BYTE g_lpRevTable[256];
extern const std::string& GetLocalIPByRemoteIP(const std::string&strHost,unsigned short nPort);
extern bool IsCallIPString(const char * str);
extern bool IsDigitString(const char * strTemp);
extern VIDEO_FORMAT_TYPE GetVideoFormatType(int nVideoWidth,int nVideoHeight);
extern char * GetCallDisconnectReason(IMXEC_CALL_END_REASON nReason);
extern IMXEC_CALL_END_REASON GetCallDisconnectReason(char * szReason);
extern void GetVideoFormatType(VIDEO_FORMAT_TYPE nFormatType,int & w,int & h);

//A7 add
extern int g_nEnableNetEC;

#define GKNETECPORT  4600 

#define HAS_LOG_FILE 1 

void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info);
void LogFileChannel(H323MediaCall * pH323Call,void * p2, HCHAN hsChan,const char * cszUserID,const char * callID,const char * info,int type);
void LogFileMediaState(H323MediaCall * pH323Call,void * p2,int nState,const char * cszUserID,const char * callID,const char * info);
void ClosLogFile();
void LogFileCall(H323MediaCall * pH323Call,void * p2, HCHAN hsChan,const char * cszUserID,const char * callID,const char * info,int type);

#if defined(HAS_LOG_FILE)

FILE * pLogFile=NULL;
int  nLogFileCreate=0;
#endif

void IPCLIB_LogL2(const char* pchLog,...);

void IPCLIB_LogL2(const char* pchLog,...)
{

}

void LogFileOpen()
{
#if defined(HAS_LOG_FILE)

	if(nLogFileCreate == 0)
	{
		pLogFile=fopen("D:\\Data\\ProtocolAdap.log","wb");
		nLogFileCreate =1;
	}
#endif
}

void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info)
{
#if defined(HAS_LOG_FILE)

	if(pLogFile ==NULL)
	{
		pLogFile=fopen("D:\\Data\\ProtocolAdap.log","wb");
	}
	char szTime[128]={0};
	time_t now = time(NULL);
	_tcsftime(szTime, 128, _T("%m-%d %H:%M:%S"), localtime(&now));

	char msg[512]={0};
	sprintf(msg,"[%s][ProtocolAdap]: [Type=1],[hsCall=%p], [haCall=%p], [IP=%s], [callID=%s], [info=%s]\n",szTime,hsCall,p,cszUserID,callID,info);
	if(pLogFile)
	{
		fwrite(msg,strlen(msg),1,pLogFile);
		fflush(pLogFile);
	}
#endif

}

void LogFileChannel(H323MediaCall * pH323Call,void * p2, HCHAN hsChan,const char * cszUserID,const char * callID,const char * info,int type)
{
#if defined(HAS_LOG_FILE)

	char media[32]={"UnKnown"};
	char dir[32]={"UnKnown"};
	H323Channel* pH323Channel = (H323Channel*)p2;
	if(pH323Channel)
	{
		if(pH323Channel->m_nDirection ==cmCapReceive) strcpy(dir,"Receive");
		else if(pH323Channel->m_nDirection ==cmCapTransmit) strcpy(dir,"Transmit");
		if(pH323Channel->m_nMediaType == Enum_Channel_Media_Audio) strcpy(media,"AUDIOChannel");
		else if(pH323Channel->m_nMediaType == Enum_Channel_Media_Video) strcpy(media,"VIDEOChannel");
		else if(pH323Channel->m_nMediaType == Enum_Channel_Media_DualVideo) strcpy(media,"DUALVIDEOChannel");
		else if(pH323Channel->m_nMediaType == Enum_Channel_Media_H224) strcpy(media,"DATAChannel");
	}
	HCALL hsCall =NULL;
	std::string strRemoteIP="";
	if(pH323Call)
	{
		hsCall =pH323Call->m_hsCall;
		if(type ==1 && pH323Call->GetRemoteUserIP(strRemoteIP) ) cszUserID=strRemoteIP.c_str();
	}
	void * p =pH323Call;
	if(pLogFile ==NULL)
	{
		pLogFile=fopen("D:\\Data\\ProtocolAdap.log","wb");
	}
	char szTime[128]={0};
	time_t now = time(NULL);
	_tcsftime(szTime, 128, _T("%m-%d %H:%M:%S"), localtime(&now));

	char msg[512]={0};
	sprintf(msg,"[%s][ProtocolAdap]: [Type=2],[hsCall=%p], [haCall=%p], [IP=%s], [callID=%s], [media=%s],[dir=%s],[info=%s]\n",szTime,hsCall,p,cszUserID,callID,media,dir,info);
	if(pLogFile)
	{
		fwrite(msg,strlen(msg),1,pLogFile);
		fflush(pLogFile);
	}
#endif

}

void LogFileCall(H323MediaCall * pH323Call,void * p2, HCHAN hsChan,const char * cszUserID,const char * callID,const char * info,int type)
{
#if defined(HAS_LOG_FILE)
	LogFileChannel(pH323Call,p2, hsChan,cszUserID,callID,info,type);
#endif
}

void LogFileMediaState(H323MediaCall * pH323Call,void * p2,int nState,const char * cszUserID,const char * callID,const char * info)
{
#if defined(HAS_LOG_FILE)

	HCHAN hsChan=NULL;
	char media[32]={"UnKnown"};
	char dir[32]={"UnKnown"};
	H323Channel* pH323Channel = (H323Channel*)p2;
	if(pH323Channel)
	{
		hsChan=pH323Channel->m_hsChan;
		if(pH323Channel->m_nDirection ==cmCapReceive) strcpy(dir,"Receive");
		else if(pH323Channel->m_nDirection ==cmCapTransmit) strcpy(dir,"Transmit");
		if(pH323Channel->m_nMediaType == Enum_Channel_Media_Audio) strcpy(media,"AUDIOChannel");
		else if(pH323Channel->m_nMediaType == Enum_Channel_Media_Video) strcpy(media,"VIDEOChannel");
		else if(pH323Channel->m_nMediaType == Enum_Channel_Media_DualVideo) strcpy(media,"DUALVIDEOChannel");
		else if(pH323Channel->m_nMediaType == Enum_Channel_Media_H224) strcpy(media,"DATAChannel");
	}
	HCALL hsCall =NULL;
	std::string strRemoteIP="";
	if(pH323Call)
	{
		hsCall =pH323Call->m_hsCall;
		if(pH323Call->GetRemoteUserIP(strRemoteIP) ) cszUserID=strRemoteIP.c_str();
	}
	void * p =pH323Call;
	if(pLogFile ==NULL)
	{
		pLogFile=fopen("D:\\Data\\ProtocolAdap.log","wb");
	}
	char szTime[128]={0};
	time_t now = time(NULL);
	_tcsftime(szTime, 128, _T("%m-%d %H:%M:%S"), localtime(&now));

	char msg[512]={0};
	sprintf(msg,"[%s][ProtocolAdap]: [Type=3],[hsCall=%p], [haCall=%p], [IP=%s], [callID=%s], [media=%s],[dir=%s],[sendState=%d],[info=%s]\n",szTime,hsCall,p,cszUserID,callID,media,dir,nState,info);
	if(pLogFile)
	{
		fwrite(msg,strlen(msg),1,pLogFile);
		fflush(pLogFile);
	}
#endif
}

void ClosLogFile()
{
#if defined(HAS_LOG_FILE)

	if(pLogFile)
	{
		fclose(pLogFile);
		pLogFile=NULL;
	}
#endif

}

bool IsNumber(const char*str)
{
	char*pTemp=(char*)str;
	int nDotCount=0;
	while (*pTemp!='\0')
	{
		if (*pTemp<'0' || *pTemp>'9')
		{
			return false;
		}
		pTemp++;
	}
	return true;
}

VOID CALLBACK H323CallMgrTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	H323CallMgr::Instance().OnTimer(idEvent);
}

////test timer call
//UINT32	 m_nIDEventTimer2;
//std::string             g_strCallID;
//m_nIDEventTimer2 =0;
//g_strCallID ="";
//
//m_nIDEventTimer2=SetTimer(NULL,1,10000,H323CallMgrTimerProc2);
//if (m_nIDEventTimer2!=0)
//{
//	KillTimer(NULL,m_nIDEventTimer2);
//	m_nIDEventTimer2=0;
//}
//
//VOID CALLBACK H323CallMgrTimerProc2(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
//{
//	//test set ip ?
//	static int nTimes =0;
//	if(nTimes ==0)
//	{
//		H323CallMgr::Instance().g_strCallID=H323CallMgr::Instance().MakeCall("");
//		nTimes ++;
//	}
//	else if(nTimes ==1)
//	{
//		if(H323CallMgr::Instance().g_strCallID.size() >0)
//			H323CallMgr::Instance().HangupCall(H323CallMgr::Instance().g_strCallID.c_str());
//		H323CallMgr::Instance().g_strCallID="";
//		nTimes --;
//	}
//}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

VOID CALLBACK DtmfTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	H323CallMgr::Instance().OnTimerDtmf(idEvent);
}
#endif

void H323CallMgr::AddCall(H323MediaCall*pH323MediaCall)
{
	H235SecurityCallInit(pH323MediaCall);
	pH323MediaCall->InitMediaRtpPortAllocate();

	XAutoLock l(m_csMapTempH323MediaCall);
	m_MapTempH323MediaCall[pH323MediaCall->GetCallID()]=pH323MediaCall;
}

bool H323CallMgr::HasCall()
{
	XAutoLock l(m_csMapH323MediaCall);
	if (m_MapH323MediaCall.size() >= 1)
	{
		return true;
	}
	return false;
}

void H323CallMgr::RemoveCall(H323MediaCall*pH323MediaCall)
{
	XAutoLock l(m_csMapH323MediaCall);
	m_MapH323MediaCall.erase(pH323MediaCall->GetCallID());
}

void H323CallMgr::ThreadProcMain(void)
{
	if(m_bThreadH323Attach ==false)
	{
		cmThreadAttach(m_hApp,m_dwThreadID );
		m_bThreadH323Attach =true;
	}

while (m_bRunning)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && ( ( defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 0) ) || (! defined(NOT_WIN32_H323_MSG) ) )
m_bRunning=false;
#endif

	while (true)
	{
		XDataBuffer*pXDataBuffer=GetDataBuffer();
		if (pXDataBuffer!=NULL)
		{
			StrPacket tStrPacket(pXDataBuffer->GetData(),false);

			std::string strCMD="";
			tStrPacket.Get("CMD",strCMD);
			if (strCMD=="CALL")
			{
				std::string strState;
				std::string strCallID="";
				std::string strUserID="";
				std::string strUserName="";
				std::string strPacket="";
				std::string strTelNum="";
				std::string strUserE164ID="";

				tStrPacket.Get("STATE",strState);
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("UID",strUserID);
				tStrPacket.Get("UNAME",strUserName);
				tStrPacket.Get("UEID",strUserE164ID);
				if (strState=="IN")
				{
					tStrPacket.Get("PACKET",strPacket);
					tStrPacket.Get("TELNUM",strTelNum);
					H323MediaCall*pH323MediaCall = NULL;
					{
						XAutoLock l(m_csMapTempH323MediaCall);
						MapH323MediaCall::iterator iter=m_MapTempH323MediaCall.find(strCallID);
						if (iter!=m_MapTempH323MediaCall.end())
						{
							pH323MediaCall=(*iter).second;
							m_MapTempH323MediaCall.erase(iter);
						}
					}
					if (pH323MediaCall)
					{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
						XAutoLock l(m_csMapH323MediaCall);
#endif

						m_MapH323MediaCall[strCallID]=pH323MediaCall;
					}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
					CallMgr::Instance().OnCallRingIn(strCallID,strUserID,strUserName,strUserE164ID,"H323",strPacket.c_str(),strPacket.length(),(char *)(strTelNum.c_str()));
#endif
				}
				else if (strState=="OUT")
				{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
					CallMgr::Instance().OnCallRingOut(strCallID,strUserID,strUserName);
#endif
				}
				else if (strState=="OK")
				{
					tStrPacket.Get("PACKET",strPacket);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
					CallMgr::Instance().OnCallEstablished(strCallID,strUserID,strUserName,strPacket.c_str(),strPacket.length());
#endif
				}
				else if (strState=="BYE")
				{
					std::string strReason="";
					tStrPacket.Get("RSN",strReason);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
					CallMgr::Instance().OnCallDestroyed(strCallID,strUserID,strUserName,strReason.c_str(),strReason.length());
#endif
					H323MediaCall*pH323MediaCall=NULL;

					{
						XAutoLock l(m_csMapTempH323MediaCall);
						MapH323MediaCall::iterator iter=m_MapTempH323MediaCall.find(strCallID);
						if (iter!=m_MapTempH323MediaCall.end())
						{
							pH323MediaCall=(*iter).second;
							m_MapTempH323MediaCall.erase(iter);
						}
					}
					if (pH323MediaCall)
					{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
						XAutoLock l(m_csMapH323MediaCall);
#endif

						m_MapH323MediaCall[strCallID]=pH323MediaCall;
					}

					{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
						XAutoLock l(m_csMapH323MediaCall);
#endif

						MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
						if (iter!=m_MapH323MediaCall.end())
						{
							pH323MediaCall=iter->second;
							m_MapH323MediaCall.erase(iter);
						}
						if(pH323MediaCall)
						{
							pH323MediaCall->SendRegDRQMessage();
							pH323MediaCall->Close();
							delete pH323MediaCall;
							pH323MediaCall=NULL;
						}
					}
				}
				else if (strState=="REGBYE")
				{
					std::string strReason="";
					tStrPacket.Get("RSN",strReason);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
					CallMgr::Instance().OnCallDestroyed(strCallID,strUserID,strUserName,strReason.c_str(),strReason.length());
#endif

				}
				else if (strState=="NONS")
				{
					std::string strPacket="";
					std::string strMime ="";
					tStrPacket.Get("INFO",strPacket);
					tStrPacket.Get("MIME",strMime);
					CallMgr::Instance().OnDAP_RecvNonstandardData(strCallID.c_str(),strPacket.c_str(),strPacket.length(),strMime.c_str());
				}
				if (strState=="VFUCHGE")
				{
					unsigned int nType =0;
					tStrPacket.Get("TYPE",nType);
					H323MediaCall*pH323MediaCall=NULL;
					{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1) 
						XAutoLock l(m_csMapH323MediaCall);
#endif 
						MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
						if (iter!=m_MapH323MediaCall.end())
						{
							pH323MediaCall=iter->second;
							if(pH323MediaCall)
							{
								if(nType ==1) pH323MediaCall->RequestMainVideoKeyFrame();
								else if(nType ==2) pH323MediaCall->RequestSecondVideoKeyFrame();
							}
						}
					}
				}
			}
			else if (strCMD=="H239")
			{
				std::string strCallID="";
				std::string strState="";
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("STATE",strState);
				if (strState=="ON")
				{
					CallMgr::Instance().OnCallSecondVideoStart(strCallID);
				}
				else if (strState=="OFF")
				{
					CallMgr::Instance().OnCallSecondVideoStop(strCallID);
				}
			}
			else if (strCMD=="DAPREG")
			{
				std::string strState="";
				std::string strRegH323ID="";
				std::string strRegE164="";
				std::string strRegReason="";

				tStrPacket.Get("DAPREGST",strState);
				tStrPacket.Get("DAPREGID",strRegH323ID);
				tStrPacket.Get("DAPREGNUM",strRegE164);
				tStrPacket.Get("DAPREGRN",strRegReason);
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

				if (strState=="DAPREGOK")
				{
					CallMgr::Instance().OnDAP_H323RegistrationSuccess(strRegH323ID.c_str(),strRegE164.c_str());
				}
				else
				{
					CallMgr::Instance().OnDAP_H323RegistrationError(strRegH323ID.c_str(),strRegE164.c_str(),(char *)strRegReason.c_str());
				}
#endif
			}
			else if(strCMD=="RCHOPEN")
			{
				std::string strMedia="";
				std::string strCallID="";
				unsigned int codecType=0;
				unsigned int nBitrate=0;
				unsigned int nWidth=0;
				unsigned int nHeight=0;
				unsigned int nPayloadType=0;
				unsigned int nLevel=0;
				tStrPacket.Get("MEDIA",strMedia);
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("CODECID",codecType);
				tStrPacket.Get("PAYLOAD",nPayloadType);
				tStrPacket.Get("BIT",nBitrate);
				tStrPacket.Get("WID",nWidth);
				tStrPacket.Get("HEIG",nHeight);
				tStrPacket.Get("LEV",nLevel);
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

				if(strMedia=="FECC")
				{
					CallMgr::Instance().OnDAP_CallFECCRecvChannelOpen(strCallID.c_str());
				}
				else if(strMedia=="AUDIO")
				{ 
					CallMgr::Instance().OnDAP_CallAudioRecvChannelOpen(strCallID.c_str(),(X_AUDIO_CODEC_ID)codecType,nPayloadType);
				}
				else if(strMedia=="VIDEO")
				{
					CallMgr::Instance().OnDAP_CallMainVideoRecvChannelOpen(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType,nBitrate,nPayloadType,nWidth,nHeight,nLevel);
				}
				else if(strMedia=="DUALVIDEO")
				{
					CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelOpen(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType,nBitrate,nPayloadType,nWidth,nHeight,nLevel);
				}

#endif
			}
			else if( (strCMD=="SCHDISCO") || (strCMD=="RCHDISCO") )
			{
				std::string strMedia="";
				std::string strCallID="";
				tStrPacket.Get("MEDIA",strMedia);
				tStrPacket.Get("CID",strCallID);
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

				if(strMedia=="FECC")
				{
					if( strCMD=="SCHDISCO")
						CallMgr::Instance().OnDAP_CallFECCSendChannelDestroyed(strCallID.c_str());
					if( strCMD=="RCHDISCO")
						CallMgr::Instance().OnDAP_CallFECCRecvChannelDestroyed(strCallID.c_str());
				}
				else if(strMedia=="AUDIO")
				{
					if( strCMD=="SCHDISCO")
						CallMgr::Instance().OnDAP_CallAudioSendChannelDestroyed(strCallID.c_str());
					if( strCMD=="RCHDISCO")
						CallMgr::Instance().OnDAP_CallAudioRecvChannelDestroyed(strCallID.c_str());
				}
				else if(strMedia=="VIDEO")
				{
					if( strCMD=="SCHDISCO")
						CallMgr::Instance().OnDAP_CallMainVideoSendChannelDestroyed(strCallID.c_str());
					if( strCMD=="RCHDISCO")
						CallMgr::Instance().OnDAP_CallMainVideoRecvChannelDestroyed(strCallID.c_str());
				}
				else if(strMedia=="DUALVIDEO")
				{
					if( strCMD=="SCHDISCO")
						CallMgr::Instance().OnDAP_CallSecondVideoSendChannelDestroyed(strCallID.c_str());
					if( strCMD=="RCHDISCO")
						CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelDestroyed(strCallID.c_str());
				}

#endif

			}
			else if( (strCMD=="SCHCONN") || (strCMD=="RCHCONN") )
			{
				std::string strMedia="";
				std::string strCallID="";
				unsigned int codecType=0;
				unsigned int nBitrate=0;
				unsigned int nWidth=0;
				unsigned int nHeight=0;
				unsigned int nPayloadType=0;
				unsigned int nLevel=0;
				unsigned long ip=0;
				unsigned int  port=0;

				tStrPacket.Get("MEDIA",strMedia);
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("CODECID",codecType);
				tStrPacket.Get("PAYLOAD",nPayloadType);
				tStrPacket.Get("BIT",nBitrate);
				tStrPacket.Get("WID",nWidth);
				tStrPacket.Get("HEIG",nHeight);
				tStrPacket.Get("LEV",nLevel);
				tStrPacket.Get("IP",ip);
				tStrPacket.Get("PORT",(unsigned int)port);
				unsigned int callPointer=0;
				tStrPacket.Get("CPR",callPointer);
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

				if(strMedia=="FECC")
				{
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallFECCSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),nPayloadType,ip, port);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallFECCRecvChannelConnected(strCallID.c_str(),nPayloadType,ip, port);
				}
				else if(strMedia=="AUDIO")
				{ 
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallAudioSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(X_AUDIO_CODEC_ID)codecType,nPayloadType,ip, port);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallAudioRecvChannelConnected(strCallID.c_str(),(X_AUDIO_CODEC_ID)codecType,nPayloadType,ip, port);
				}
				else if(strMedia=="VIDEO")
				{
					//A7 add
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
					VIDEO_FORMAT_TYPE formatType=GetVideoFormatType(nWidth,nHeight);
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallMainVideoSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallMainVideoRecvChannelConnected(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
#else
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallMainVideoSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallMainVideoRecvChannelConnected(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
#endif

				}
				else if(strMedia=="DUALVIDEO")
				{
					//A7 add
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
					VIDEO_FORMAT_TYPE formatType=GetVideoFormatType(nWidth,nHeight);
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallSecondVideoSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelConnected(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
#else
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallSecondVideoSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelConnected(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
#endif

				}

#endif
			}

			delete pXDataBuffer;
			pXDataBuffer=NULL;
		}
		else
		{
			break;
		}
	}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && ( ( defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1) ) && ( defined(NOT_WIN32_H323_MSG) ) )
	XSleep(1000);
#endif

	unsigned long nTimestamp=XGetTimestamp();

	//定时发送genericIndication
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	if (m_nLastTimestampCKACT > nTimestamp)
	{
		m_nLastTimestampCKACT =nTimestamp;
	}
	else if (nTimestamp - m_nLastTimestampCKACT >= 3000) //3 seconds
	{
		DoCheckPTZAction();
		m_nLastTimestampCKACT =nTimestamp;
	}
#endif

	//定时注册
	if(m_nAutoManualReg ==1)
	{
		if (m_nLastTimestampThread > nTimestamp)
		{
			m_nLastTimestampThread =nTimestamp;
		}
		else if (nTimestamp - m_nLastTimestampThread >= m_nRegGKTimestamp && m_nRegAccountNum >0)
		{
			TimerRegisterToGK();
			m_nLastTimestampThread =nTimestamp;
		}
	}
	//发送回环测试信令,不在调SendAndCheckRoundTripDalay函数以减少频繁调函数压入堆栈资源占用

		if (m_nLastSendTripTimestamp > nTimestamp)
		{
			m_nLastSendTripTimestamp =nTimestamp;
		}
		else if (nTimestamp - m_nLastSendTripTimestamp >= 10000) //10 seconds
		{
			m_nLastSendTripTimestamp=nTimestamp;
			DoCheckRoundTrip();
		}

//发送打洞包
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	if( m_nUseMCUFWNAT >0 )
#else
	if( m_nUseTerminalFWNAT >0 )
#endif

	{
		if (m_nSendKeepliveTimestamp > nTimestamp)
		{
			m_nSendKeepliveTimestamp = nTimestamp;
		}
		else if(nTimestamp - m_nSendKeepliveTimestamp >= m_nSendKeepliveSeconds *1000)
		{
			SendChannelKeepAlive();
			m_nSendKeepliveTimestamp =nTimestamp;
		}
	}
}
	if(m_bThreadH323Attach)
	{
		m_bThreadH323Attach =false;
		cmThreadDetach(m_hApp, m_dwThreadID);
	}

}

void H323CallMgr::OnTimer(UINT32 nIDEvent)
{
//WIN32界面终端使用新接口时回调使用ThreadProcMain转调
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && ( ( defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 0) ) || (! defined(NOT_WIN32_H323_MSG) ) )
m_bRunning=true;
ThreadProcMain();
return;
#endif

	while (true)
	{
		XDataBuffer*pXDataBuffer=GetDataBuffer();
		if (pXDataBuffer!=NULL)
		{
			StrPacket tStrPacket(pXDataBuffer->GetData(),false);

			std::string strCMD="";
			tStrPacket.Get("CMD",strCMD);
			if (strCMD=="CALL")
			{
				std::string strState;
				std::string strCallID="";
				std::string strUserID="";
				std::string strUserName="";

				tStrPacket.Get("STATE",strState);
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("UID",strUserID);
				tStrPacket.Get("UNAME",strUserName);
				if (strState=="IN")
				{
					H323MediaCall*pH323MediaCall = NULL;
					{
						XAutoLock l(m_csMapTempH323MediaCall);
						MapH323MediaCall::iterator iter=m_MapTempH323MediaCall.find(strCallID);
						if (iter!=m_MapTempH323MediaCall.end())
						{
							pH323MediaCall=(*iter).second;
							m_MapTempH323MediaCall.erase(iter);
						}
					}
					if (pH323MediaCall)
					{
						m_MapH323MediaCall[strCallID]=pH323MediaCall;
					}
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else
					CallMgr::Instance().OnCallRingIn(strCallID,strUserID,strUserName,"H323");
#endif
				}
				else if (strState=="OUT")
				{
					CallMgr::Instance().OnCallRingOut(strCallID,strUserID,strUserName);
				}
				else if (strState=="OK")
				{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else
					CallMgr::Instance().OnCallEstablished(strCallID,strUserID,strUserName);
#endif

					H323MediaCall*pH323MediaCall=NULL;
					MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
					if (iter!=m_MapH323MediaCall.end())
					{
						pH323MediaCall=iter->second;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

CheckSendGenericRequest(pH323MediaCall);

#endif

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 2) ||  (HAS_H323_AVCON_MP == 1) ) 

#else

						pH323MediaCall->OpenRemoteMainPlayChan();

#endif

					}

#if !defined(HAS_H323_AVCON_MP) || (HAS_H323_AVCON_MP == 0)

					if(CallMgr::Instance().m_nUseNETECSend > 0 )
					{
						LogFile(NULL,NULL,"","","NETEC Node Start() first.");
						//OutputDebugString("NETEC Node Start() first.\n");
						if(pH323MediaCall) pH323MediaCall->NETEC_NodeStart();
						//OutputDebugString("NETEC Node Start() after.\n");
						LogFile(NULL,NULL,"","","NETEC Node Start() after.");
					}
#endif

				}
				else if (strState=="BYE")
				{
					H323MediaCall*pH323MediaCall=NULL;
					{
						XAutoLock l(m_csMapTempH323MediaCall);
						MapH323MediaCall::iterator iter=m_MapTempH323MediaCall.find(strCallID);
						if (iter!=m_MapTempH323MediaCall.end())
						{
							pH323MediaCall=(*iter).second;
							m_MapTempH323MediaCall.erase(iter);
						}
					}
					if (pH323MediaCall)
					{
						m_MapH323MediaCall[strCallID]=pH323MediaCall;
					}

					MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
					if (iter!=m_MapH323MediaCall.end())
					{
						pH323MediaCall=iter->second;
					}

//					if (pH323MediaCall)
//					{
//
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//
//#else
//						CallMgr::Instance().OnCallDestroyed(strCallID,strUserID,strUserName,(IMXEC_CALL_END_REASON)0,pH323MediaCall->GetCallNum());
//						OutputDebugString("OnTimer OnCallDestroyed\n");
//#endif
//					}

					if (pH323MediaCall)
					{
						//OutputDebugString("OnTimer delete MapH323MediaCall.erase(iter) 111\n");
						m_MapH323MediaCall.erase(iter);
						//OutputDebugString("OnTimer delete MapH323MediaCall.erase(iter) 222\n");

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else
						//add by Jan for DisConnect Reason [20141025]
						//CallMgr::Instance().OnCallDestroyed(strCallID,strUserID,strUserName,(IMXEC_CALL_END_REASON)0,pH323MediaCall->GetCallNum());
						CallMgr::Instance().OnCallDestroyed(strCallID,strUserID,strUserName,GetCallDisconnectReason((char*)pH323MediaCall->DisconnectReason().c_str()),pH323MediaCall->GetCallNum());
						//OutputDebugString("OnTimer OnCallDestroyed\n");
#endif

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

						CheckDelH239MediaCallItem(pH323MediaCall);
						CheckSetH239TokenState(pH323MediaCall);
						if(m_pVitualH323MediaCall)
						{
							std::string strCallIDTemp=m_pVitualH323MediaCall->GetCallID();
							bool bFind=false;
							int nCount=0;
							nCount=m_MapH323MediaCall.size();
							MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallIDTemp);
							if (iter!=m_MapH323MediaCall.end())
							{
								bFind=true;
							}
							//仅有m_pVitualH323MediaCall时状态复原
							if(nCount==1 && bFind)
							{
								SetH239TokenState(m_pVitualH323MediaCall,false);
								InitH239VirtualCallState();
								SetMPAgentTokenState(m_pVitualH323MediaCall,false);
								CallMgr::Instance().OnCallSecondVideoStop(strCallIDTemp);
							}
						}

#endif

#if !defined(HAS_H323_AVCON_MP) || (HAS_H323_AVCON_MP == 0)

					if(CallMgr::Instance().m_nUseNETECSend > 0 )
					{
						LogFile(NULL,NULL,"","","NETEC Node Stop() first.");
						//OutputDebugString("NETEC Node Stop() first.\n");
						if(pH323MediaCall) pH323MediaCall->NETEC_NodeStop();
						//OutputDebugString("NETEC Node Stop() after.\n");
						LogFile(NULL,NULL,"","","NETEC Node Stop() after.");
					}

#endif
						pH323MediaCall->SendRegDRQMessage();
						pH323MediaCall->Close();
						delete pH323MediaCall;
						pH323MediaCall=NULL;
						//OutputDebugString("OnTimer delete pH323MediaCall\n");
					}
				}
				else if (strState=="CAPCHGE")
				{
					unsigned int nMediaType =0;
					tStrPacket.Get("MTYPE",nMediaType);

					H323MediaCall*pH323MediaCall=NULL;
					MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
					if (iter!=m_MapH323MediaCall.end())
					{
						pH323MediaCall=iter->second;

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 2) ||  (HAS_H323_AVCON_MP == 1) ) 

#else

						if(pH323MediaCall) pH323MediaCall->OnRcvH245CapabilitiesSetChange(nMediaType);

#endif

					}
				}
				else if (strState=="NONS")
				{
					std::string strPacket="";
					std::string strMime ="";
					tStrPacket.Get("INFO",strPacket);
					tStrPacket.Get("MIME",strMime);
					CallMgr::Instance().OnDAP_RecvNonstandardData(strCallID.c_str(),strPacket.c_str(),strPacket.length(),strMime.c_str());
				}
				else if (strState=="RTPCHGE")
				{
					unsigned int nType =0;
					tStrPacket.Get("TYPE",nType);
					std::string strP="";
					unsigned long ipt=0;
					tStrPacket.Get("CHANNEL",ipt);
					H323Channel * p=(H323Channel *)ipt;
				}
				else if (strState=="VFUCHGE")
				{
					unsigned int nType =0;
					tStrPacket.Get("TYPE",nType);
					H323MediaCall*pH323MediaCall=NULL;
					MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
					if (iter!=m_MapH323MediaCall.end())
					{
						pH323MediaCall=iter->second;

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 2) ||  (HAS_H323_AVCON_MP == 1) ) 

#else 
						if(pH323MediaCall)
						{
							if(nType ==1) pH323MediaCall->RequestMainVideoKeyFrame();
							else if(nType ==2) pH323MediaCall->RequestSecondVideoKeyFrame();
						} 
#endif 
					}

				}
			}
			else if (strCMD=="H239")
			{
				std::string strCallID="";
				std::string strState="";
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("STATE",strState);
				if (strState=="ON")
				{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

					MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
					if (iter!=m_MapH323MediaCall.end())
					{
						H323MediaCall*pH323MediaCall=iter->second;
						if(pH323MediaCall==m_pVitualH323MediaCall) 
							m_pVitualH323MediaCall->OnLocalSecondVideoStarted();
					}

#endif


					CallMgr::Instance().OnCallSecondVideoStart(strCallID);

#if (  ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) )  )

#else
					MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
					if (iter!=m_MapH323MediaCall.end())
					{
						H323MediaCall*pH323MediaCall=iter->second;
						IMXEC_CALL_DIRECTION dir=IMXEC_CALL_DIR_UNKOWN;
						if(pH323MediaCall->GetSupportSecondVideo() )
						{
							pH323MediaCall->GetSecondVideoDirection(dir);
							if (dir==IMXEC_CALL_DIR_IN)
							{
								pH323MediaCall->OpenRemoteSecondPlayChan();
								pH323MediaCall->RequestSecondVideoKeyFrame();
							}
							else
							{
								pH323MediaCall->CloseRemoteSecondPlayChan();
							}
						}
					}

#endif

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

					//PrintDebugStringInfo2("OnCallSecondVideoStart",strCallID.c_str());

#endif

				}
				else if (strState=="OFF")
				{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
					MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
					if (iter!=m_MapH323MediaCall.end())
					{
						H323MediaCall*pH323MediaCall=iter->second;
						if(pH323MediaCall==m_pVitualH323MediaCall) 
							m_pVitualH323MediaCall->OnLocalSecondVideoStop();
					}
					CallMgr::Instance().OnCallSecondVideoStop(strCallID);
					//PrintDebugStringInfo2("OnCallSecondVideoStop",strCallID.c_str());
				}
				else if (strState=="BGR")
				{
					MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
					if (iter!=m_MapH323MediaCall.end())
					{
						H323MediaCall*pH323MediaCall=iter->second;
						OnBroadGenericRequest(pH323MediaCall);
					}
				}
				else if (strState=="MPBGR")
				{
					std::string strConfID="";
					tStrPacket.Get("CONFID",strConfID);
					OnBroadGenericRequest(strConfID);
				}
#else

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
#else
					SetSecondVideoWnd(strCallID.c_str(),NULL);
#endif

					CallMgr::Instance().OnCallSecondVideoStop(strCallID);

				}

#endif

			}
			else if (strCMD=="LOCAL_MAIN_VIDEO")
			{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else
				std::string strCallID="";
				std::string strState="";
				H323MediaCall * pH323MediaCall=NULL;
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("STATE",strState);
				if (strState=="ON")
				{
					MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
					if (iter!=m_MapH323MediaCall.end())
					{
						pH323MediaCall=iter->second;
					}

					//if(CallMgr::Instance().m_nUseNETECSend > 0 )
					//{
					//	OutputDebugString("NETEC Node Start() first.\n");
					//	if(pH323MediaCall) pH323MediaCall->NETEC_NodeStart();
					//	OutputDebugString("NETEC Node Start() after.\n");
					//}
					if(pH323MediaCall) pH323MediaCall->OpenLocalMainVideoCapDev();

				}
				else if (strState=="OFF")
				{

					MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
					if (iter!=m_MapH323MediaCall.end())
					{
						pH323MediaCall=iter->second;
					}

					//if(CallMgr::Instance().m_nUseNETECSend > 0 )
					//{
					//	OutputDebugString("NETEC Node Stop() first.\n");
					//	if(pH323MediaCall) pH323MediaCall->NETEC_NodeStop();
					//	OutputDebugString("NETEC Node Stop() after.\n");
					//}
					if(pH323MediaCall) pH323MediaCall->CloseLocalMainVideoCapDev();

				}
#endif

			}
			//added by jiangdingfeng 2013-11-28
			else if (strCMD == "LOCAL_SECOND_VIDEO")
			{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else
				std::string strCallID="";
				std::string strState="";
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("STATE",strState);
				if (strState=="ON")
				{
					MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
					if (iter!=m_MapH323MediaCall.end())
					{
						iter->second->OpenLocalSecondVideo();
					}
				}
				else if (strState=="OFF")
				{
				}
#endif

			}
			else if (strCMD=="REG")
			{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else
				if (m_bRegisteredToGK)
				{
					m_nRegStatusTimestamp=XGetTimestamp();

					if( !m_bRegToGKStateOld ) CallMgr::Instance().OnH323RegisterOK(m_strRegHost.c_str(),m_nRegGKPort);
				}
				else
				{
					LogFile(NULL,NULL,"","","OnH323RegisterFailed");
					CallMgr::Instance().OnH323RegisterFailed();
				}
#endif

			}

			else if( (strCMD=="SCHDISCO") || (strCMD=="RCHDISCO") )
			{
				std::string strMedia="";
				std::string strCallID="";
				tStrPacket.Get("MEDIA",strMedia);
				tStrPacket.Get("CID",strCallID);
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

				if(strMedia=="FECC")
				{
					if( strCMD=="SCHDISCO")
						CallMgr::Instance().OnDAP_CallFECCSendChannelDestroyed(strCallID.c_str());
					if( strCMD=="RCHDISCO")
						CallMgr::Instance().OnDAP_CallFECCRecvChannelDestroyed(strCallID.c_str());
				}
				else if(strMedia=="AUDIO")
				{
					if( strCMD=="SCHDISCO")
						CallMgr::Instance().OnDAP_CallAudioSendChannelDestroyed(strCallID.c_str());
					if( strCMD=="RCHDISCO")
						CallMgr::Instance().OnDAP_CallAudioRecvChannelDestroyed(strCallID.c_str());
				}
				else if(strMedia=="VIDEO")
				{
					if( strCMD=="SCHDISCO")
						CallMgr::Instance().OnDAP_CallMainVideoSendChannelDestroyed(strCallID.c_str());
					if( strCMD=="RCHDISCO")
						CallMgr::Instance().OnDAP_CallMainVideoRecvChannelDestroyed(strCallID.c_str());
				}
				else if(strMedia=="DUALVIDEO")
				{
					if( strCMD=="SCHDISCO")
						CallMgr::Instance().OnDAP_CallSecondVideoSendChannelDestroyed(strCallID.c_str());
					if( strCMD=="RCHDISCO")
						CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelDestroyed(strCallID.c_str());
				}

//#endif

			}
			else if( (strCMD=="SCHCONN") || (strCMD=="RCHCONN") )
			{
				std::string strMedia="";
				std::string strCallID="";
				unsigned int codecType=0;
				unsigned int nBitrate=0;
				unsigned int nWidth=0;
				unsigned int nHeight=0;
				unsigned int nPayloadType=0;
				unsigned int nLevel=0;
				unsigned long ip=0;
				unsigned int  port=0;

				tStrPacket.Get("MEDIA",strMedia);
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("CODECID",codecType);
				tStrPacket.Get("PAYLOAD",nPayloadType);
				tStrPacket.Get("BIT",nBitrate);
				tStrPacket.Get("WID",nWidth);
				tStrPacket.Get("HEIG",nHeight);
				tStrPacket.Get("LEV",nLevel);
				tStrPacket.Get("IP",ip);
				tStrPacket.Get("PORT",(unsigned int)port);
				unsigned int callPointer=0;
				tStrPacket.Get("CPR",callPointer);
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

				if(strMedia=="FECC")
				{
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallFECCSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),nPayloadType,ip, port);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallFECCRecvChannelConnected(strCallID.c_str(),nPayloadType,ip, port);
				}
				else if(strMedia=="AUDIO")
				{ 
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallAudioSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(X_AUDIO_CODEC_ID)codecType,nPayloadType,ip, port);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallAudioRecvChannelConnected(strCallID.c_str(),(X_AUDIO_CODEC_ID)codecType,nPayloadType,ip, port);
				}
				else if(strMedia=="VIDEO")
				{
					VIDEO_FORMAT_TYPE formatType=GetVideoFormatType(nWidth,nHeight);
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallMainVideoSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallMainVideoRecvChannelConnected(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
					//if( strCMD=="SCHCONN")
					//	CallMgr::Instance().OnDAP_CallMainVideoSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
					//	,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
					//if( strCMD=="RCHCONN")
					//	CallMgr::Instance().OnDAP_CallMainVideoRecvChannelConnected(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
					//	,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);

				}
				else if(strMedia=="DUALVIDEO")
				{

					VIDEO_FORMAT_TYPE formatType=GetVideoFormatType(nWidth,nHeight);
					if( strCMD=="SCHCONN")
						CallMgr::Instance().OnDAP_CallSecondVideoSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
					if( strCMD=="RCHCONN")
						CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelConnected(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
						,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);

					//if( strCMD=="SCHCONN")
					//	CallMgr::Instance().OnDAP_CallSecondVideoSendChannelConnected((HDAPAPPCALL)callPointer,strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
					//	,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
					//if( strCMD=="RCHCONN")
					//	CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelConnected(strCallID.c_str(),(VIDEC_CODEC_TYPE)codecType
					//	,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);

				}

//#endif
			}

			delete pXDataBuffer;
			pXDataBuffer=NULL;
		}
		else
		{
			break;
		}
	}

	//gk mcu use 20150409 
	//if (!m_bNETECNodeToGK && m_bRegisteredToGK) NETECStartToGK();
	//else if (m_bNETECNodeToGK && !m_bRegisteredToGK) NETECStopToGK();

	//定时检测媒体流接收情况 定时发送genericIndication 检测远遥操作
	unsigned long nTimestamp=XGetTimestamp();
	if (m_nLastTimestampCKACT > nTimestamp)
	{
		m_nLastTimestampCKACT =nTimestamp;
	}
	else if (nTimestamp - m_nLastTimestampCKACT >= 3000)
	{
		DoCheckPTZAction();
		m_nLastTimestampCKACT =nTimestamp;
		//check empty set call num zero
		CheckMapCallEmpty();
	}

	//定时注册
	if(m_nAutoManualReg ==1)
	{
		if (m_nLastTimestampTimer > nTimestamp)
		{
			m_nLastTimestampTimer =nTimestamp;
		}
		else if (nTimestamp - m_nLastTimestampTimer >= m_nRegGKTimestamp && m_nRegAccountNum >0)
		{
			TimerRegisterToGK();
			m_nLastTimestampTimer =nTimestamp;
		}
	}


	//发送打洞包
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	if( m_nUseMCUFWNAT >0 )
#else

	if(m_nUseGK >0)
	{
		if (m_nRegStatusTimestamp > nTimestamp)
		{
			m_nRegStatusTimestamp =nTimestamp;
		}
		else if (nTimestamp - m_nRegStatusTimestamp >= m_nTimeToLive *3)
		{
			m_nRegStatusTimestamp=nTimestamp;
			m_bRegisteredToGK =false;
			//LogFile(NULL,NULL,"","","OnH323RegisterFailed");
			CallMgr::Instance().OnH323RegisterFailed();
		}
	}

	if( m_nUseTerminalFWNAT >0 )
#endif

	{
		if (m_nSendKeepliveTimestamp > nTimestamp)
		{
			m_nSendKeepliveTimestamp =nTimestamp;
		}
		else if(nTimestamp-m_nSendKeepliveTimestamp >= m_nSendKeepliveSeconds *1000)
		{
			SendChannelKeepAlive();
			m_nSendKeepliveTimestamp=nTimestamp;
		}
	}


}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

void H323CallMgr::OnTimerDtmf(UINT32 nIDEvent)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.begin();
	while (iter!=m_MapH323MediaCall.end())
	{
		if (iter->second->GetUserID()==m_strPeerMCUAddr)
		{
			if (m_strConfID!="")
			{
				char *pbyDtmf=(char*)m_strConfID.c_str();
				H323MediaCall*pH323MediaCall=iter->second;
				if (pH323MediaCall->SendDTMF(*pbyDtmf)>=0)
				{
					m_strConfID=m_strConfID.substr(1,m_strConfID.length()-1);
				}
			}
			else
			{
				KillTimer(NULL,m_nIDDtmfTimer);
				m_nIDDtmfTimer=0;
			}
			return;
		}

		iter++;
	}
}

#endif


H323CallMgr&H323CallMgr::Instance(void)
{
	return g_H323CallMgr;
}

H323CallMgr::H323CallMgr()
: m_bRegisteredToGK(false)
, m_strH323ID("")
, m_strTelNumber("")
, m_strUserName("")
, m_strRegHost("")
, m_strRegPassword("")
, m_nUseGK(0)
, m_nH323ListenPort(1720)
, m_strLocalIP("")
, m_nRTPBasePort(12000)
, m_nSupportH239(0)
{
	m_hApp=NULL;
	m_strEmpty="";

	m_nStarted=0;

	m_nIDEventTimer=0;

	m_strConfID="";
	m_strPeerMCUAddr ="";

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

m_nIDDtmfTimer=0;

#endif

	m_nAudioMaxBitrate=64;
	m_nAudioCodecID=X_AUDIO_CODEC_G711U;
	m_nMainVideoMaxBitrate=384;
	m_nMainVideoDevID=0;
	m_nMainVideoDevFrameRate=15;
	m_nMainVideoCodecType=VIDEC_CODEC_H264;
	m_nMainVideoFormatType=VIDEO_FORMAT_CIF;
    m_nMainVideoNetworkType=VIDEC_NETWORK_PRIVATE;
    

	m_nSecondVideoMaxBitrate=384;
	m_nSecondVideoDevID=0;
	m_nSecondVideoDevFrameRate=15;
	m_nSecondVideoCodecType=VIDEC_CODEC_H264;
	m_nSecondVideoFormatType=VIDEO_FORMAT_CIF;
    m_nSecondVideoNetworkType=VIDEC_NETWORK_PRIVATE;

	MakeReverseTable( g_lpRevTable, 256 );
	H235SecuritySettingInitialize();

	m_nMaxCallNum = 128;            //notice mcu use
	m_nH245PortFrom=m_nRTPBasePort;
	m_nH245PortTo=m_nH245PortFrom + m_nMaxCallNum * 2;
	m_nGateKeeperPort=1719;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	m_bTokenUseState=false;
	m_pKeepH323MediaCall=NULL;
	m_pRequestH323MediaCall=NULL;
	m_pVitualH323MediaCall=NULL;
	m_bMPUseToken=false;
	m_bTermianlUseToken=false;
#endif

	m_strProName ="";
	m_strUserName = m_strProName;
	m_nAutoManualReg=1;
	m_nFindGKAddr=false;
	m_strProductID="AVCON";//Polycom ViaVideo Release 8.0: 8.0.1.0669
	m_strProductVersionID="Release 2.0: 2.0.0.1";
	m_nT35Code=88;
	m_nT35Extension=0;
	m_nManufacturerCode=1;
	m_bRunning=false;
	m_nEndpontType = 0;
	m_nMasterSlaveValue = 50;//notice
	m_nCapPrint=0;
	m_nLastTimestampThread=0;
	m_nLastTimestampTimer=0;
	m_nLastTimestampCKACT=0;
	m_nRegGKTimestamp=2*1000;
	m_strMCUID="";
	m_nLastSendTripTimestamp=XGetTimestamp();


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	SetDAPMasterSlaveValue(240);
	SetDAPH323EndPointType(1);
	SetMCUInfo();
#endif

	m_nRegAccountNum=0;

	m_hFwNat=NULL;
	m_nUseTerminalFWNAT=0;
	m_nUseMCUFWNAT= 1;
	m_nSendKeepliveSeconds =10;
	m_nSendKeepliveTimestamp =0;
	m_nRegStatusTimestamp=XGetTimestamp();
	m_nRegGKPort =1719;
	m_nTimeToLive =60 * 1000;
	m_bRegToGKStateOld =false;
	m_nRASCastPort =1718;
	m_nRASLocalPort=1714;
	m_strLastGKHost ="";
	m_nSendURQ=0;

	m_nDoCheckPTZCount=0;
	m_nCheckNetCardDisable=0;
	m_bRoundTripDiscMaxCount =1;
	memset(&m_MediaPortRelease,0,sizeof(m_MediaPortRelease) );
	m_strH225LogicalKA ="H225KeepAliveTest";
	memset(&m_DNSIP,0,sizeof(m_DNSIP));
	m_bThreadH323Attach =false;
	m_nCallerRate =0;
	m_nTerminalCapsType =0;
	m_nUseMediaNetRTP =0;
	m_nSendFlowCC =1;
	m_bNETECNodeToGK =false;
}

H323CallMgr::~H323CallMgr()
{

}

void H323CallMgr::H235SecuritySettingInitialize()
{
	m_bCheckIncoming=true;
	memset(m_strPassword ,0,sizeof(m_strPassword ) );
	memset(m_strSenderId ,0,sizeof( m_strSenderId) );
	memset( m_strGeneralId,0,sizeof(m_strGeneralId ) );
	m_bH235AnnexI=false;     //annex D I 消息完整性及密码认证
	m_bH235AnnexIA=false;    //annex D IA 仅密码认证
	m_bH235AnnexII=false;    //annex E II 采用公钥/私钥对的数字签名
	m_bH235AnnexIII=false;   //annex E III 端到端认证
	m_bH235AnnexIV=false;    //annex F 规程IV
	m_bUserPrimes=false;     //媒体流加密 使用素数参与生成共享密钥
	m_bH235MediaDH512=false; //媒体流加密 使用DH512生成7字节共享密钥
	m_bH235MediaDH1024=false;//媒体流加密 使用DH1024生成21字节共享密钥
	m_bH235MediaDH1536=false;//媒体流加密 使用DH1536生成21字节共享密钥
	m_nH235MediaUse=0;
	m_nH235AuthUse=0;
	m_nUseAlogrithmId=ALGORITHM_AES_CBC;
	m_nMediaEncrypSpecified=0;
	memset(m_strMediaEncrypPWD,0,sizeof(m_strMediaEncrypPWD));
}

const char* H323CallMgr::GetH235AuthCheckState(const char*cszCallID,IMXEC_CALL_H235AUTH_STATE &nState,int &nLength)
{
	int nCheck=0;
	nLength=0;
	if(m_nH235AuthUse>0) 
	{
		return m_h235Mgr.H235SecurityGetAuthCheckState(nCheck,nLength);
	}
	nState=(IMXEC_CALL_H235AUTH_STATE)nCheck;
	return NULL;
}

void H323CallMgr::OnChannelRecvMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType)
{
	std::string strMsg1="Unknow";
	if(nMediaType ==IMXEC_CALL_MEDIA_AUDIO ) strMsg1="Audio";
	else if(nMediaType ==IMXEC_CALL_MEDIA_MAINVIDEO ) strMsg1="MainVideo";
	else if(nMediaType ==IMXEC_CALL_MEDIA_H239VIDEO ) strMsg1="DualVideo";
	else if(nMediaType ==IMXEC_CALL_MEDIA_H224DATA ) strMsg1="FECC";
	strMsg1=strMsg1 +",";
	std::string strMsg2="H323Stack ChannelRecvMediaEncryption type is:" + strMsg1;

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,cszCallID);
	OutPutStackLog(0,strMsg2.c_str(),szCallIDBack);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_ChannelRecvMediaEncryption(cszCallID,nMediaType);
#else

	CallMgr::Instance().OnChannelRecvMediaEncryption(cszCallID,nMediaType);
#endif

}

void H323CallMgr::OnChannelSendMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType)
{
	std::string strMsg1="Unknow";
	if(nMediaType ==IMXEC_CALL_MEDIA_AUDIO ) strMsg1="Audio";
	else if(nMediaType ==IMXEC_CALL_MEDIA_MAINVIDEO ) strMsg1="MainVideo";
	else if(nMediaType ==IMXEC_CALL_MEDIA_H239VIDEO ) strMsg1="DualVideo";
	else if(nMediaType ==IMXEC_CALL_MEDIA_H224DATA ) strMsg1="FECC";
	strMsg1=strMsg1 +",";
	std::string strMsg2="H323Stack ChannelSendMediaEncryption type is:" + strMsg1;

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,cszCallID);
	OutPutStackLog(0,strMsg2.c_str(),szCallIDBack);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_ChannelSendMediaEncryption(cszCallID,nMediaType);
#else

	CallMgr::Instance().OnChannelSendMediaEncryption(cszCallID,nMediaType);
#endif
}

void H323CallMgr::SetH235MediaSecurityEncryptionPWD(int type,char * encrypPwd)
{
	m_nMediaEncrypSpecified=type;
	if(encrypPwd!=NULL) strcpy(m_strMediaEncrypPWD,encrypPwd);
}

void H323CallMgr::RegisterGK()
{
	if (m_nStarted==0) return ;

	RvPvtNodeId nodeId=-1;
    RvPvtNodeId tmpNodeId=-1;
	int error=-1;
	int  len;
	char tempbuf[256];
	int nIndex = 1;
	char strTmp[256];
    nodeId = cmGetRASConfigurationHandle(m_hApp);
	HPVT valH = cmGetValTree(m_hApp);
	if (nodeId >=0)
	{
		RegGKSetAutoManual();
		//设置注册信息
		if(m_strH323ID.size()>0)
		{
			sprintf(strTmp, "registrationInfo.terminalAlias.%d.h323-ID", nIndex);
			len = utlChr2Bmp(m_strH323ID.c_str(), (RvUint8*)tempbuf);
			error = pvtBuildByPath(valH, nodeId, strTmp,  len, tempbuf);
			nIndex++;
		}
		if(m_strTelNumber.size()>0)
		{
			sprintf(strTmp, "registrationInfo.terminalAlias.%d.e164", nIndex);
			len=m_strTelNumber.size();
			strcpy(tempbuf,m_strTelNumber.c_str());
			error = pvtBuildByPath(valH, nodeId, strTmp,  len, tempbuf);
			nIndex++;
		}
		cmRegister(m_hApp);
	}
}

void H323CallMgr::RegGKSetAutoManual(int type)
{
	if (m_nStarted==0) return ;

	RvPvtNodeId nodeId=-1;
	RvPvtNodeId tmpNodeId=-1;
	int error=-1;
	char strTmp[256];
	nodeId = cmGetRASConfigurationHandle(m_hApp);
	HPVT valH = cmGetValTree(m_hApp); 

	if (nodeId >=0)
	{
		if( (type==0) && ((m_nUseGK>0 && m_strRegHost.size()>0) || m_nUseGK==1) )
		{
			if (m_nUseGK==1)//自动发现
			{
				tmpNodeId = pvtGetNodeIdByPath( valH, nodeId, "manualDiscovery");
				if(tmpNodeId>=0)
				{
					error = pvtDelete(valH,tmpNodeId);
				}
				tmpNodeId = pvtGetNodeIdByPath( valH, nodeId, "manualRAS");
				if(tmpNodeId>=0)
				{
					error = pvtDelete(valH,tmpNodeId);
				}
				tmpNodeId = pvtGetNodeIdByPath( valH, nodeId, "manualRegistration");
				if(tmpNodeId>=0)
				{
					error = pvtDelete(valH,tmpNodeId);
				}
				char *szCastIP="255.255.255.255";
				int nCastPort=m_nRASCastPort;
				unsigned long nAddr = inet_addr(szCastIP);

				sprintf(strTmp, "rasMulticastAddress.ipAddress.ip");
				error = pvtBuildByPath(valH, nodeId, strTmp,  4,(const char * )&nAddr);
				sprintf(strTmp, "rasMulticastAddress.ipAddress.port");
				error = pvtBuildByPath(valH, nodeId, strTmp,  nCastPort,NULL);
			}
			else
			{
				tmpNodeId = pvtGetNodeIdByPath( valH, nodeId, "manualRAS");
				if(tmpNodeId>=0)
				{
					error = pvtDelete(valH,tmpNodeId);
				}
				tmpNodeId = pvtGetNodeIdByPath( valH, nodeId, "manualRegistration");
				if(tmpNodeId>=0)
				{
					error = pvtDelete(valH,tmpNodeId);
				}
				unsigned long nAddr=inet_addr(m_strRegHost.c_str());
				sprintf(strTmp, "manualDiscovery.defaultGatekeeper.ipAddress.ip");
				error = pvtBuildByPath(valH, nodeId, strTmp,  4,(const char * )&nAddr);
				sprintf(strTmp, "manualDiscovery.defaultGatekeeper.ipAddress.port");
				error = pvtBuildByPath(valH, nodeId, strTmp,m_nGateKeeperPort,NULL);

			}
		}
		else if(type==1)
		{
			if (m_nUseGK==1)
			{
				tmpNodeId = pvtGetNodeIdByPath( valH, nodeId, "manualDiscovery");
				if(tmpNodeId>=0) error = pvtDelete(valH,tmpNodeId);
			}
			sprintf(strTmp, "manualRAS");
			error = pvtBuildByPath(valH, nodeId, strTmp, 0,NULL );
			sprintf(strTmp, "manualRegistration");
			error = pvtBuildByPath(valH, nodeId, strTmp, 0,NULL );
		}
		else
		{
			sprintf(strTmp, "manualRAS");
			error = pvtBuildByPath(valH, nodeId, strTmp, 0,NULL );
		}

		//删除terminalType信息
		tmpNodeId = pvtGetNodeIdByPath( valH, nodeId, "registrationInfo.terminalType.mcu");
		if(tmpNodeId>=0)
		{
			error = pvtDelete(valH,tmpNodeId);
		}
		tmpNodeId = pvtGetNodeIdByPath( valH, nodeId, "registrationInfo.terminalType.terminal");
		if(tmpNodeId>=0)
		{
			error = pvtDelete(valH,tmpNodeId);
		}

		//设置terminalType信息
		if(m_nEndpontType ==0 )
		{
			sprintf(strTmp, "registrationInfo.terminalType.terminal");
			error = pvtBuildByPath(valH, nodeId, strTmp, 0,NULL );
		}
		else if(m_nEndpontType ==1 )
		{
			sprintf(strTmp, "registrationInfo.terminalType.mcu");
			error = pvtBuildByPath(valH, nodeId, strTmp, 0,NULL );
		}

		if( (m_strH323ID.size()>0) || (m_strTelNumber.size()>0) )
		{
			tmpNodeId = pvtGetNodeIdByPath( valH, nodeId, "registrationInfo.terminalAlias");
			if(tmpNodeId>=0)
			{
				error = pvtDelete(valH,tmpNodeId);
			}
		}
	}
}

void H323CallMgr::SetH235AuthData()
{
	m_h235Mgr.H235SecurityAuthSetting(m_strSenderId,m_strGeneralId,m_strPassword);
	m_h235Mgr.H235SecurityUserPrimeSetting(m_bUserPrimes);
	m_h235Mgr.H235SecurityCheckIncomingMsgSetting(m_bCheckIncoming);
	m_h235Mgr.H235SecurityAnnexISetting(m_bH235AnnexI);
	m_h235Mgr.H235SecurityAnnexIASetting(m_bH235AnnexIA);
	m_h235Mgr.H235SecurityAnnexIISetting(m_bH235AnnexII);
	m_h235Mgr.H235SecurityAnnexIIISetting(m_bH235AnnexIII);
	m_h235Mgr.H235SecurityAnnexIVSetting(m_bH235AnnexIV);
	m_h235Mgr.H235SecurityMediaDH512Setting(m_bH235MediaDH512);
	m_h235Mgr.H235SecurityMediaDH1024Setting(m_bH235MediaDH1024);
	m_h235Mgr.H235SecurityMediaDH1536Setting(m_bH235MediaDH1536);
}

void H323CallMgr::H235SecurityCallInit(H323Call *pH323MediaCall)
{
	if(pH323MediaCall->GetMgrH235AuthUse() >0)
		this->m_h235Mgr.H235SecurityCallInit(pH323MediaCall);
}

void H323CallMgr::H235SecurityCallEnd(H323Call *pH323MediaCall)
{
	if(pH323MediaCall->GetMgrH235AuthUse() >0)
		this->m_h235Mgr.H235SecurityCallEnd(pH323MediaCall);
}

void H323CallMgr::SetH235SecurityAuthOption(int option)
{
	m_nH235AuthUse=option;
}

void H323CallMgr::SetH235MediaSecurityOption(int option)
{
	m_nH235MediaUse=option;
}

void H323CallMgr::SetH235MediaEncryptionAlogrithmFirst(int alogrimId)
{
	m_nUseAlogrithmId=alogrimId;
}

void H323CallMgr::SetH235SecurityAuth(char * senderId,char * generalId,char * password)
{
	//char msg[128]={0};
	//sprintf(msg,"sendid:%s,generalid:%s,password:%s\n",senderId,generalId,password);
	//OutputDebugString(msg);
	if( (senderId ==NULL) || (generalId ==NULL) || (password ==NULL) )  return ;
	strcpy(m_strSenderId,senderId);
	strcpy(m_strGeneralId,generalId);
	strcpy(m_strPassword,password);
}

void H323CallMgr::SetH235SecurityUserPrimeEnable(bool enable)
{
	m_bUserPrimes=enable;
}

void H323CallMgr::SetH235SecurityCheckIncomingMsgEnable(bool enable)
{
	m_bCheckIncoming=enable;
}

void H323CallMgr::SetH235SecurityAnnexIEnable(bool enable)
{
	m_bH235AnnexI=enable;
}

void H323CallMgr::SetH235SecurityAnnexIAEnable(bool enable)
{
	m_bH235AnnexIA=enable;
}

void H323CallMgr::SetH235SecurityAnnexIIEnable(bool enable)
{
	m_bH235AnnexII=enable;
}

void H323CallMgr::SetH235SecurityAnnexIIIEnable(bool enable)
{
	m_bH235AnnexIII=enable;
}

void H323CallMgr::SetH235SecurityAnnexIVEnable(bool enable)
{
	m_bH235AnnexIV=enable;
}

void H323CallMgr::SetH235SecurityMediaDH512Enable(bool enable)
{
	m_bH235MediaDH512=enable;
}

void H323CallMgr::SetH235SecurityMediaDH1024Enable(bool enable)
{
	m_bH235MediaDH1024=enable;
}

void H323CallMgr::SetH235SecurityMediaDH1536Enable(bool enable)
{
	m_bH235MediaDH1536=enable;
}


int H323CallMgr::GetEncDecAlgorithmTypeFromOID(EncDecAlgorithmID &id,unsigned char * pData,int dataLen)
{
	int result=0;
	int algorithmLen=0;
	int k=0;
	EncDecAlgorithmID algId=ALGORITHM_NONE;
	bool find=false;
	id=ALGORITHM_NONE;
	int len1,len2,len3;

	for(k=1;k<ALGORITHM_COUNT;k++)
	{
		algId=(EncDecAlgorithmID) k;
		switch(algId)
		{
		case ALGORITHM_DES_CBC:
			{
				char OIDDES[32] = {0};
				len1=utlEncodeOID(sizeof(OIDDES),OIDDES,DES_OID);
				algorithmLen=len1;
				char * pValue =OIDDES;
				if(dataLen==algorithmLen)
					if(memcmp(pData,pValue,algorithmLen)==0) find=true;
			}
			break;
		case ALGORITHM_TRIPLE_DES_CBC:
			{
				char OIDDES3[32] = {0};
				len2=utlEncodeOID(sizeof(OIDDES3),OIDDES3,DES3_OID);
				algorithmLen=len2;
				char * pValue =OIDDES3;
				if(dataLen==algorithmLen)
					if(memcmp(pData,pValue,algorithmLen)==0) find=true;
			}
			break;
		case ALGORITHM_AES_CBC:
			{
				char OIDAES[32] = {0};
				len3=utlEncodeOID(sizeof(OIDAES),OIDAES,AES_OID);
				algorithmLen=len3;
				char * pValue =OIDAES;
				if(dataLen==algorithmLen)
					if(memcmp(pData,pValue,algorithmLen)==0) find=true;
			}
			break;
		case ALGORITHM_AES256_CBC:
			{
				char OIDAES[32] = {0};
				len3=utlEncodeOID(sizeof(OIDAES),OIDAES,AES256_OID);
				algorithmLen=len3;
				char * pValue =OIDAES;
				if(dataLen==algorithmLen)
					if(memcmp(pData,pValue,algorithmLen)==0) find=true;
			}
			break;
		case ALGORITHM_DES_OFB:
			{
				algorithmLen=5;
				unsigned char pValue[5]={0x28,0xCD,0x7B,0x04,0x03};
				if(dataLen==algorithmLen)
					if(memcmp(pData,pValue,algorithmLen)==0) find=true;
			}
			break;
		case ALGORITHM_DES_ECB:
			{
				algorithmLen=5;
				unsigned char pValue[5]={0x28,0xCD,0x7B,0x04,0x00};
				if(dataLen==algorithmLen)
					if(memcmp(pData,pValue,algorithmLen)==0) find=true;
			}
			break;
		case ALGORITHM_DES_CFB:
			{
				algorithmLen=5;
				unsigned char pValue[5]={0x28,0xCD,0x7B,0x04,0x01};
				if(dataLen==algorithmLen)
					if(memcmp(pData,pValue,algorithmLen)==0) find=true;
			}
			break;
		case ALGORITHM_TRIPLE_DES_ECB:
			{
				algorithmLen=9;
				unsigned char pValue[9]={0x2B,0x06,0x01,0x04,0x01,0xA6,0x41,0x01,0x07};
				if(dataLen==algorithmLen)
					if(memcmp(pData,pValue,algorithmLen)==0) find=true;
			}
			break;

		}
		if(find)
		{
			id=algId;
			result=algorithmLen;
			break;
		}

	}
	return result;
}

int H323CallMgr::GetEncDecAlgorithmOIDFromType(EncDecAlgorithmID algId,unsigned char * pData,int &dataLen)
{
	int result=0;
	int algorithmLen=0;
	int len1,len2,len3;

	switch(algId)
	{
	case ALGORITHM_DES_CBC:
		{
			char OIDDES[32] = {0};
			len1=utlEncodeOID(sizeof(OIDDES),OIDDES,DES_OID);
			algorithmLen=len1;
			char * pValue =OIDDES;
			if(dataLen<algorithmLen) return result;
			memcpy(pData,pValue,algorithmLen);
		}
		break;
	case ALGORITHM_TRIPLE_DES_CBC:
		{
			char OIDDES3[32] = {0};
			len2=utlEncodeOID(sizeof(OIDDES3),OIDDES3,DES3_OID);
			algorithmLen=len2;
			char * pValue =OIDDES3;
			if(dataLen<algorithmLen) return result;
			memcpy(pData,pValue,algorithmLen);			
		}
		break;
	case ALGORITHM_AES_CBC:
		{
			char OIDAES[32] = {0};
			len3=utlEncodeOID(sizeof(OIDAES),OIDAES,AES_OID);
			algorithmLen=len3;
			char * pValue =OIDAES;
			if(dataLen<algorithmLen) return result;
			memcpy(pData,pValue,algorithmLen);			
		}
		break;
	case ALGORITHM_AES256_CBC:
		{
			char OIDAES[32] = {0};
			len3=utlEncodeOID(sizeof(OIDAES),OIDAES,AES256_OID);
			algorithmLen=len3;
			char * pValue =OIDAES;
			if(dataLen<algorithmLen) return result;
			memcpy(pData,pValue,algorithmLen);			
		}
		break;
	case ALGORITHM_DES_OFB:
		{
			algorithmLen=5;
			unsigned char pValue[5]={0x28,0xCD,0x7B,0x04,0x03};
			if(dataLen<algorithmLen) return result;
			memcpy(pData,pValue,algorithmLen);
		}
		break;
	case ALGORITHM_DES_ECB:
		{
			algorithmLen=5;
			unsigned char pValue[5]={0x28,0xCD,0x7B,0x04,0x00};
			if(dataLen<algorithmLen) return result;
			memcpy(pData,pValue,algorithmLen);			
		}
		break;
	case ALGORITHM_DES_CFB:
		{
			algorithmLen=5;
			unsigned char pValue[5]={0x28,0xCD,0x7B,0x04,0x01};
			if(dataLen<algorithmLen) return result;
			memcpy(pData,pValue,algorithmLen);			
		}
		break;

	case ALGORITHM_TRIPLE_DES_ECB:
		{
			algorithmLen=9;
			unsigned char pValue[9]={0x2B,0x06,0x01,0x04,0x01,0xA6,0x41,0x01,0x07};
			if(dataLen<algorithmLen) return result;
			memcpy(pData,pValue,algorithmLen);			
		}
		break;
	}
	result = algorithmLen;
	dataLen=algorithmLen;
	return result;
}

int H323CallMgr::SetH323ID(const char*cszH323ID)
{
	char temp[128]={0};
	if (cszH323ID==NULL)
	{
		m_strH323ID="";
	}
	else
	{
		strncpy(temp,cszH323ID,127);
		temp[127] ='\0';
		m_strH323ID=temp;
	}

	if (m_strH323ID.empty())
	{
		m_strH323ID=m_strProName;
		//modify
		m_strH323ID="";
	}
	return 0;
}

int H323CallMgr::SetTelNumber(const char*cszTelNumber)
{
	char temp[128]={0};
	if (cszTelNumber==NULL)
	{
		m_strTelNumber="";
	}
	else
	{
		strncpy(temp,cszTelNumber,127);
		temp[127] ='\0';
		m_strTelNumber=temp;
	}
	return 0;
}

int H323CallMgr::SetRegPassword(const char*cszRegPassword)
{
	if (cszRegPassword==NULL)
	{
		m_strRegPassword="";
	}
	else
	{
		m_strRegPassword=cszRegPassword;
	}
	return 0;
}

int H323CallMgr::SetUserName(const char*cszUserName)
{
	if (cszUserName==NULL)
	{
		m_strUserName="";
	}
	else
	{
		m_strUserName=cszUserName;
	}
	if (m_strUserName.empty())
	{
		m_strUserName=m_strProName;
	}
	return 0;
}

int H323CallMgr::SetRegHost(const char*cszHost)
{
	if (cszHost==NULL)
	{
		m_strRegHost="";
	}
	else
	{
		m_strRegHost=cszHost;
	}
	return 0;
}

int H323CallMgr::Register(int nType)
{
	if (nType==0)
	{
		m_nUseGK=1;
	}
	else
	{
		m_nUseGK=2;
	}

	//LogFile(NULL,NULL,"","","H323CallMgr::Register");
	return 0;
}

int H323CallMgr::UnRegister(void)
{
	m_nUseGK=0;

	//LogFile(NULL,NULL,"","","H323CallMgr::UnRegister");

	return 0;
}

int H323CallMgr::GetRegState(IMXEC_H323::REG_STATE&nRegState)
{
	if (m_nUseGK==0)
	{
		nRegState=IMXEC_H323::REG_IDLE;
	}
	else
	{
		if (m_bRegisteredToGK)
		{
			nRegState=IMXEC_H323::REG_OK;
		}
		else
		{
			nRegState=IMXEC_H323::REG_IDLE;
		}
	}
	return 0;
}

int H323CallMgr::SetH323ListenPort(unsigned short nH323ListenPort)
{
	m_nH323ListenPort=nH323ListenPort;
	if (m_nH323ListenPort <= 0)
	{
		m_nH323ListenPort=1720;
	}
	return 0;
}

/******************************************************************************
* SetLocalIP
*描述：设置注册帐号
*输入：	cszH323ID		-注册帐号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetLocalIP(const char*cszLocalIP)
{
	if (cszLocalIP==NULL)
	{
		m_strLocalIP="";
	}
	else
	{
		m_strLocalIP=cszLocalIP;
	}
	return 0;
}

/******************************************************************************
* SetRTPBasePort
*描述：设置侦听端口
*输入：	nRTPBasePort			-RTP开始端口
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetRTPBasePort(unsigned short nRTPBasePort)
{
	m_nRTPBasePort=nRTPBasePort;
	if (m_nRTPBasePort==0)
	{
		m_nRTPBasePort=12000;
	}
	return 0;
}

/******************************************************************************
* SetSupportH239
*描述：设置是否支持H239
*输入：nSupportH239		-是否支持H239
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetSupportH239(int nSupportH239)
{
	m_nSupportH239=nSupportH239;
	return 0;
}

/******************************************************************************
* SetAudioDevParam
*描述：设置主流视频设备参数
*输入： nAudioCodecID		-音频编码
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID)
{
	m_nAudioCodecID=nAudioCodecID;
	return 0;
}

/******************************************************************************
* SetMainVideoDevParam
*描述：设置主流视频设备参数
*输入： nDevID				-设备标识
		nFrameRate			-帧率
		nBitrate			-码流
		nVideoCodecType		-视频编码
		nVideoFormatType	-视频格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
	m_nMainVideoMaxBitrate=nBitrate;
	m_nMainVideoDevID=nDevID;
	m_nMainVideoDevFrameRate=nFrameRate;
	m_nMainVideoCodecType=nVideoCodecType;
	m_nMainVideoFormatType=nVideoFormatType;
	m_nMainVideoNetworkType=nNetworkType;
	//1014 add
	//m_nMainVideoNetworkType=VIDEC_NETWORK_PRIVATE;
	return 0;
}

/******************************************************************************
* SetSecondVideoDevParam
*描述：设置辅流视频设备参数
*输入： nDevID				-设备标识
		nFrameRate			-帧率
		nBitrate			-码流
		nVideoCodecType		-视频编码
		nVideoFormatType	-视频格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
	m_nSecondVideoMaxBitrate=nBitrate;
	m_nSecondVideoDevID=nDevID;
	m_nSecondVideoDevFrameRate=nFrameRate;
	m_nSecondVideoCodecType=nVideoCodecType;
	m_nSecondVideoFormatType=nVideoFormatType;
	m_nSecondVideoNetworkType=nNetworkType;
	//1014 add
	//m_nSecondVideoNetworkType=VIDEC_NETWORK_PRIVATE;
	return 0;
}

unsigned short H323CallMgr::GetRTPBasePort(void)
{
	return m_nRTPBasePort;
}

const char* H323CallMgr::MakeCall(const char*cszUserID)
{
	return InsideMakeCall(cszUserID);
}

const char*H323CallMgr::InsideMakeCall(const char*cszUserID,int nBandWidth,char * cszPacket,ConfInfo *pConfInfo)
{
	std::string strUserID=cszUserID;
	LogFile(NULL,NULL,cszUserID,"","H323 InsideMakeCall");

	//A7 support call mcu conference
	std::string strCalledConf ="";
	int nConfFound=-1;
	std::string::size_type nConfPos=-1;
	nConfFound=strUserID.find("##");
	if(nConfFound >=0)
	{
		int nposLen =2;
		strCalledConf=strUserID.substr(nConfFound+nposLen,strUserID.length()-nConfFound-nposLen);
		strUserID=strUserID.substr(0, nConfFound);
		cszUserID=strUserID.c_str();
		m_strPeerMCUAddr=strUserID;
		m_strConfID=strCalledConf;
	}
	else
	{
		//U6 mp support call mcu conference
		nConfPos=strUserID.find('#');
		if (nConfPos!=-1)
		{
			m_strConfID=strUserID.substr(nConfPos+1,strUserID.length()-nConfPos-1);
			strUserID=strUserID.substr(0, nConfPos);
			m_strPeerMCUAddr=strUserID;
			cszUserID=strUserID.c_str();
		}
	}

	char msg[256]={0};
	bool bIP=false;
	bool bNumber=false;
	bool bDNS=false;
	char strDNSIP[32]={0};
	int  nIPLen =31;
	bool bGetDNSIP=false;
	cmAlias termAlias;
	char bmpStr[256] = {0};
	memset(&termAlias, 0, sizeof(termAlias) );
	bIP=IsCallIPString(cszUserID);
	bNumber=IsNumber(cszUserID);
	if(bIP ==false && bNumber ==false)
	{
		bDNS=IsDNS(cszUserID);
	}
	if(bDNS)
	{
		bGetDNSIP=GetDNSIP(cszUserID,strDNSIP,nIPLen);
	}
	if(bGetDNSIP)
	{
		termAlias.type = cmAliasTypeH323ID;	
		//转换成双字节的string
		termAlias.length =utlChr2Bmp(cszUserID, (RvUint8*)bmpStr );
		termAlias.string=bmpStr;
		strcpy((char *)cszUserID,strDNSIP);
		strUserID=cszUserID;
	}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if (nConfPos!=-1)
	{
		if (m_nIDDtmfTimer==0)
		{
			m_nIDDtmfTimer=SetTimer(NULL,2,1000,DtmfTimerProc);
		}
	}

#endif

	unsigned long nLocal=H323CallMgr::Instance().GetLocalIPAddress(m_strLocalIP);
	if(bIP)
	{
		unsigned long nDest=H323CallMgr::Instance().GetLocalIPAddress(strUserID);
		if(nDest==nLocal)
		{
			return m_strEmpty.c_str();
		}
	}

	HCALL hsCall = NULL;
	if(cmCallNew(m_hApp, (HAPPCALL)NULL, &hsCall)>=0)
	{
		if(bGetDNSIP && termAlias.length >0)
		{
			cmCallSetParam(hsCall,cmParamDestinationAddress,0,sizeof(cmAlias), (char*)&termAlias );
		}

		H323MediaCall *pH323MediaCall = new H323MediaCall(*this, hsCall,pConfInfo);
		if(pH323MediaCall)
		{
			pH323MediaCall->SetCallSourcePhysicalAddress(cszUserID);
			cmCallSetHandle(hsCall,(HAPPCALL)pH323MediaCall);
			pH323MediaCall->SetTerminalH460NATFwOption(m_nUseTerminalFWNAT);
			//more gk account support add ,mcu use
			if (pConfInfo != NULL && strlen(pConfInfo->m_abyLocalConfE164)>0 /*&& bIP== false*/ )
			{
				H323RegUser* pH323RegUser=FindH323RegAccount(pConfInfo->m_abyLocalConfE164);
				if(pH323RegUser)
				{
					if(pH323RegUser->m_bRegResult ==false)
					{
						LogFile(NULL,NULL,cszUserID,"","RegResult false");
						cmCallClose(hsCall);
						delete pH323MediaCall;
						pH323MediaCall =NULL;
						return m_strEmpty.c_str();
					}

					if(nBandWidth==0)
					{
						nBandWidth =SetBandWidth1(pConfInfo);
					}

					pH323MediaCall->m_strE164ID=pConfInfo->m_abyLocalConfE164;
					pH323MediaCall->m_nBandWidth=nBandWidth;
					pH323MediaCall->SeRegAccountCallMark();
					pH323RegUser->SetAccountCallMakeParam(pH323MediaCall,cszUserID,cszPacket,pConfInfo->m_nUseFWNAT);
					LogFile(NULL,NULL,cszUserID,"","InsideMakeCall SendRegARQMessage.");
					int nRasRet=pH323RegUser->SendRegARQMessage(pH323MediaCall);
					if(nRasRet <0)
					{
						LogFile(NULL,NULL,cszUserID,"","H323Stack SendARQ CallFunction Error.");
						IMXEC_CALL_END_REASON discReason=ModeDisconnectedError;
						pH323MediaCall->DisconnectReason()=GetCallDisconnectReason(discReason);
						H323CallMgr::Instance().OnDAP_CallDestroyed(pH323MediaCall->GetCallID(),pH323MediaCall->GetUserID(),pH323MediaCall->GetUserFullName(),discReason,pH323MediaCall->DisconnectReason());
						cmCallClose(hsCall);
						delete pH323MediaCall;
						pH323MediaCall=NULL;
						return m_strEmpty.c_str();
					}
					return pH323MediaCall->GetCallID().c_str();
				}
			}

			//终端用,注册后呼叫,取本机IP
			if(bIP ==false )
			{
				LogFile(NULL,NULL,cszUserID,"","IsCallIPString false");
				char * gkIP="";
				RvUint16 port=m_nGateKeeperPort;
				struct in_addr  addr;
				addr.S_un.S_addr = m_TransportAddr.ip;
				if( m_nUseGK == 1 && m_nFindGKAddr)
				{
					gkIP = inet_ntoa(addr);
					port = m_TransportAddr.port;
				}
				else
				{
					gkIP=(char *)m_strRegHost.c_str();
				}
				pH323MediaCall->m_strLocalIP =GetLocalIPByRemoteIP(gkIP,port);
				if(m_strLocalIP.size() ==0 ) m_strLocalIP =pH323MediaCall->m_strLocalIP;
			}

			H235SecurityCallInit(pH323MediaCall);

			pH323MediaCall->InitMediaRtpPortAllocate();
			LogFile(NULL,NULL,cszUserID,"","InitMediaRtpPortAllocate");

			char szDest[512]="";
			if(strncmp(cszUserID, "TA:", 3)==0 || strncmp(cszUserID, "TEL:", 4)==0)
			{
				strcpy(szDest, cszUserID);
			}
			else
			{
				if ((!strchr(cszUserID,',') && strchr(cszUserID,'.')) || strchr(cszUserID,',')>strchr(cszUserID,'.') && strchr(cszUserID,'.'))
				{
					strcpy(szDest,"TA:");
					strcat(szDest, cszUserID);

					pH323MediaCall->m_strLocalIP=GetLocalIPByRemoteIP(cszUserID,m_nH323ListenPort);

//add by Jan for Destination Address [20130903]  //add tai er ru wang test use
#if defined(TAIERLAB) && (TAIERLAB == 1)

					cmAlias alias;
					alias.type = cmAliasTypeTransportAddress;
					alias.transport.type = cmTransportTypeIP;
					alias.transport.distribution = cmDistributionUnicast;
					alias.transport.ip = inet_addr(cszUserID);
					alias.transport.port = m_nH323ListenPort;
					cmCallSetParam(hsCall,cmParamDestinationAddress,0,0,(char *)&alias);

#endif

				}
				else
				{
					if (IsNumber(cszUserID))
					{
						strcpy(szDest,"TEL:");
						strcat(szDest, cszUserID);
					}
					else
					{
						sprintf(szDest,"NAME:%s",cszUserID);
					}
				}
			}

			if(nBandWidth==0)
			{
				nBandWidth =SetBandWidth2(pConfInfo);
			}

			//gk mcu use 20150409
			bool bCallType =true;
			if(bIP && m_bNETECNodeToGK) bCallType =false;
			//notice the set is use NETEC Send
			if(CallMgr::Instance().m_nUseNETECSend > 0 && bCallType)
			{
				SetUserNonstandardInfo(pH323MediaCall,cmCallStateOffering,pConfInfo,cszPacket);
				//LogFile(NULL,NULL,cszUserID,"","SetUserNonstandardInfo");
			}

			std::string strAVCON_H323=m_strProName;
			std::string strAVCON_UserName=m_strProName;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
			strAVCON_UserName=m_strProName;
			strAVCON_H323="Product of AVCON";
#endif

			if( (m_strH323ID.size()>0) && (m_strH323ID.size()<255) )
				strAVCON_H323=m_strH323ID;
			if( (m_strUserName.size()>0) && (m_strUserName.size()<255) )
				strAVCON_UserName=m_strUserName;

			if( pConfInfo!=NULL && strlen(pConfInfo->m_abyConfName)>0 )
			{
				strAVCON_UserName = pConfInfo->m_abyConfName;
				strAVCON_H323=pConfInfo->m_abyConfName;
			}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

			//added by jiangdingfeng 2013-7-25 如果携带了对端MCU会议E164号码(会议号码)
			if (pConfInfo != NULL && strlen(pConfInfo->m_abyConfE164)>0)
			{
				strCalledConf =pConfInfo->m_abyConfE164;
				nConfFound =0;
			}

			if (m_nEndpontType ==1 && pConfInfo != NULL && strlen(pConfInfo->m_abyLocalConfE164)>0)
			{
				AddQ931CallPartyNumber(hsCall,cszUserID,cmParamCallingPartyNumber,pConfInfo->m_abyLocalConfE164);
			}

			//set call param
			int nParamIndex =0;
			if (pConfInfo != NULL && strlen(pConfInfo->m_abyConfName) >0 )
			{
				cmAlias termAlias;
				char bmpStr[288] = {0};
				std::string strSameIP="";
				bool bGet=GetOneSameDestIP3(CallMgr::Instance().m_AllLocalIPSave,strSameIP,pH323MediaCall->m_strMakeDestIP);
				if(bGet && GetH323IDAlias((char *)strSameIP.c_str(),termAlias,bmpStr) >=0)
				{
					if(cmCallSetParam(hsCall, cmParamSourceAddress,nParamIndex,sizeof(cmAlias), (char*)&termAlias ) >=0)
						nParamIndex ++;
				}
			}

			if (pConfInfo != NULL && strlen(pConfInfo->m_abyLocalConfE164)>0)
			{
				cmAlias termAlias;
				char bmpStr[288] = {0};
				if(GetE164IDAlias(pConfInfo->m_abyLocalConfE164,termAlias,bmpStr) >=0)
				{
					cmCallSetParam(hsCall, cmParamSourceAddress,nParamIndex,sizeof(cmAlias), (char*)&termAlias );
				}
			}

#endif

			if(nConfFound >=0 && strCalledConf.size() >0)
			{
				AddQ931CallPartyNumber(hsCall,cszUserID,cmParamCalledPartyNumber,(char *)strCalledConf.c_str());
				char szTemp[256] ={0};
				sprintf(szTemp,",TEL:%s",strCalledConf.c_str());
				strcat(szDest, szTemp);
			}

			LogFile(hsCall,pH323MediaCall,cszUserID,pH323MediaCall->GetCallID().c_str(),"cmCallMake");

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

			//mcu 级联测试
			if(m_nCallerRate >0) nBandWidth =m_nCallerRate;
			if(m_nEndpontType ==0) strAVCON_UserName =m_strProName;
#endif

			int nRt=cmCallMake(hsCall, nBandWidth*1000, 0, szDest, NULL, strAVCON_UserName.c_str(),strAVCON_H323.c_str(), strAVCON_H323.length());
			if(nRt < 0)
			{
				char szCallIDBack[128]={0}; strcpy(szCallIDBack,pH323MediaCall->GetCallID().c_str());
				cmCallClose(hsCall);
				OutPutStackLog(0,"H323Stack MakeCall Error.",szCallIDBack);
				LogFile(NULL,NULL,cszUserID,"","H323Stack MakeCall CallFunction Error.");
				delete pH323MediaCall;
				pH323MediaCall =NULL;
				return m_strEmpty.c_str();
			}

			if(nRt >= 0) OutPutStackLog(0,"H323Stack MakeCall OK.",(char *)pH323MediaCall->GetCallID().c_str());

			{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
				XAutoLock l(m_csMapH323MediaCall);
#endif
				m_MapH323MediaCall[pH323MediaCall->GetCallID()] = pH323MediaCall;
			}
			return pH323MediaCall->GetCallID().c_str();
		}
		else
		{
			LogFile(NULL,NULL,cszUserID,"","New H323MediaCall Failure");
			cmCallClose(hsCall);
			delete pH323MediaCall;
			pH323MediaCall =NULL;
		}
	}
	OutPutStackLog(1,"H323Stack MakeCall Failure.");
	LogFile(NULL,NULL,cszUserID,"","H323 cmCallNew Failure");

	return m_strEmpty.c_str();
}

int H323CallMgr::AcceptCall(const char*cszCallID)
{
	return InsideAcceptCall(cszCallID);
}

int H323CallMgr::InsideAcceptCall(const char*cszCallID,char * cszPacket,ConfInfo *pConfInfo)
{
	int nRet=-1;
	char szCallIDBack[128]={0};
	strcpy(szCallIDBack,cszCallID);

	LogFile(NULL,NULL,"",cszCallID,"InsideAcceptCall");

	HCALL hsCall=NULL;
	H323MediaCall*pH323MediaCall = NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=(*iter).second;
			hsCall=pH323MediaCall->m_hsCall;
		}

		if (hsCall)
		{
			//mcu 级联测试
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

			if(m_nTerminalCapsType ==1 && pConfInfo)
				pH323MediaCall->ConfCapabilitisBitrateDefault(pConfInfo);

#endif

			pH323MediaCall->SetTerminalH460NATFwOption(m_nUseTerminalFWNAT);
			//more gk account support add
			if (pConfInfo != NULL && strlen(pConfInfo->m_abyLocalConfE164)>0 )
			{
				H323RegUser* pH323RegUser=FindH323RegAccount(pConfInfo->m_abyLocalConfE164);
				if(pH323RegUser)
				{
					pH323RegUser->AccountAnswerCall(pH323MediaCall,cszCallID,cszPacket,pConfInfo);
					return 0;
				}
			}

			//notice the set is use NETEC Send
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//			if(CallMgr::Instance().m_nUseNETECSend > 0 )
//#else
//			if(CallMgr::Instance().m_nUseNETECSend > 0 /*&& NETEC_Node::IsStarted() ==0*/ ) //gk mcu use 20150409 
//#endif

			if(CallMgr::Instance().m_nUseNETECSend > 0 )
			{
				SetUserNonstandardInfo(pH323MediaCall,cmCallStateConnected,pConfInfo,cszPacket);
			}

			std::string strAVCON_H323=m_strProName;
			std::string strAVCON_UserName=m_strProName;
			if( (m_strH323ID.size()>0) && (m_strH323ID.size()<255) )
				strAVCON_H323=m_strH323ID;
			if( (m_strUserName.size()>0) && (m_strUserName.size()<255) )
				strAVCON_UserName=m_strUserName;

			if( pConfInfo!=NULL && strlen(pConfInfo->m_abyConfName)>0 )
			{
				//strAVCON_UserName +=pConfInfo->m_abyConfName;
				strAVCON_UserName = pConfInfo->m_abyConfName;
				strAVCON_H323=pConfInfo->m_abyConfName;
			}
			if (pConfInfo != NULL)
			{
				pH323MediaCall->AcceptCallSetCapabilities(pConfInfo);
			}

			std::string  strUserIP="";
			if( pH323MediaCall->GetRemoteUserIP(strUserIP) )
				LogFile(hsCall,pH323MediaCall,strUserIP.c_str(),cszCallID,"cmCallAnswerExt");
			else LogFile(hsCall,pH323MediaCall,"NoIP",cszCallID,"cmCallAnswerExt");

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

			//mcu 级联测试
			if(m_nEndpontType ==0) strAVCON_UserName =m_strProName;
#endif
			//cmCallAnswerDisplay(hsCall,strAVCON_UserName.c_str());
			nRet=cmCallAnswerExt(hsCall,strAVCON_UserName.c_str(),strAVCON_H323.c_str(),strAVCON_H323.length());
			if(nRet <0)
			{
				cmCallDropParam(hsCall,cmReasonTypeAdaptiveBusy);
				OutPutStackLog(0,"H323Stack AcceptCall Error.",szCallIDBack);
				LogFile(NULL,NULL,szCallIDBack,"","H323Stack AcceptCall CallFunction Error.");
			}
		}
	}

	if(nRet >=0)
	{
		OutPutStackLog(0,"H323Stack AcceptCall OK.",szCallIDBack);
	}
	return 0;
}

int H323CallMgr::HangupCall(const char*cszCallID)
{
	LogFile(NULL,NULL,"","","H323CallMgr::HangupCall 111");

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,cszCallID);
	HCALL hsCall=NULL;
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=(*iter).second;
			hsCall=pH323MediaCall->m_hsCall;
			pH323MediaCall->m_hsCall=NULL;
		}
		if(pH323MediaCall)
		{
			pH323MediaCall->m_bCallState =false;
			H235SecurityCallEnd(pH323MediaCall);
		}
		if (hsCall)
		{
			pH323MediaCall->m_nLocalHungUP=1;
			cmCallDrop(hsCall);
		}
	}
	OutPutStackLog(0,"H323Stack HangupCall OK.",szCallIDBack);

	LogFile(NULL,NULL,"","","H323CallMgr::HangupCall 222");

	return 0;
}

/******************************************************************************
* SendPTZ
*描述：发送PTZ【发送云台控制命令】
*输入：	cszCallID		-呼叫标识
		nPTZAction		-云台动作
		nSpeed			-动作的速度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->SendPTZ(nPTZAction,nSpeed);
	}
	return -1;
}

/******************************************************************************
* StartSecondVideo
*描述：开始辅流
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::StartSecondVideo (const char*cszCallID)
{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	std::string strCallID=cszCallID;
	if(strCallID.size()==0) strCallID="H323:DUALVIDEO";
	if(m_pVitualH323MediaCall==NULL)
	{
		m_pVitualH323MediaCall=new H323MediaCall(*this,NULL);
		if(m_pVitualH323MediaCall)
		{
			//m_pVitualH323MediaCall->SetConferenceID();
			m_pVitualH323MediaCall->SetCallID(strCallID,m_pVitualH323MediaCall);
			m_pVitualH323MediaCall->SetVitualMark(true);
			m_MapH323MediaCall[strCallID]=m_pVitualH323MediaCall;
		}
	}

	if(m_pVitualH323MediaCall)
	{
		//令牌在用不让发 notice test
		//bool bUseState=CheckH239TokenState(m_pVitualH323MediaCall);
		//if(bUseState) return -1;
		InitH239VirtualCallState();
		//CheckSendGenericRequest(m_pVitualH323MediaCall);
		m_pVitualH323MediaCall->SetSendSecondVideoParam(m_nSecondVideoDevFrameRate,m_nSecondVideoMaxBitrate,m_nSecondVideoCodecType,m_nSecondVideoFormatType/*,106*/);

		bool bGetForce=false;
		if(bGetForce)
		{
			GetH239TokenForce();
		}
		else
		{
			bool bUseToken=CheckH239TokenState(m_pVitualH323MediaCall);
			bool bHasToken=m_pVitualH323MediaCall->CheckH239TokenOwnState();
			SetH239ControlAgent(1);
			RecordH239MsgType(1);
			SetH239MediaStatus(1);
			//原H323需要加OnRecvGenericCommand
			//if(bUseToken && !bHasToken) OnRecvH239GenericCommand(m_pVitualH323MediaCall,0,0);
			OnRecvH239GenericRequest(m_pVitualH323MediaCall,0,0,0);//add
		}

		//PrintDebugStringInfo2(m_pVitualH323MediaCall->GetUserID().c_str(),m_pVitualH323MediaCall->GetCallID().c_str());
	}

#else

	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			H323MediaCall*pH323MediaCall=iter->second;
			return pH323MediaCall->StartSecondVideo();
		}
	}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	return -1;
}

/******************************************************************************
* StopSecondVideo
*描述：停止辅流
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::StopSecondVideo (const char*cszCallID)
{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	std::string strCallID=cszCallID;
	if(m_pVitualH323MediaCall)
	{
		m_pVitualH323MediaCall->OnLocalSecondVideoStop();
		SetH239ControlAgent(1);
		RecordH239MsgType(4);
		SetH239MediaStatus(4);
		OnRecvH239GenericCommand(m_pVitualH323MediaCall,0,0);
		CheckDelH239MediaCallItem(m_pVitualH323MediaCall);
		bool bFind=false;
		int nCount=0;
		nCount=m_MapH323MediaCall.size();
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			bFind=true;
		}
		if(nCount==1 && bFind) SetH239TokenState(m_pVitualH323MediaCall,false);
		InitH239VirtualCallState();
		SetMPAgentTokenState(m_pVitualH323MediaCall,false);

		//PrintDebugStringInfo2(m_pVitualH323MediaCall->GetUserID().c_str(),m_pVitualH323MediaCall->GetCallID().c_str());
		if(bFind)
		{
			m_MapH323MediaCall.erase(iter);
			delete m_pVitualH323MediaCall;
			m_pVitualH323MediaCall=NULL;
		}
	}

#else

	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			H323MediaCall*pH323MediaCall=iter->second;
			pH323MediaCall->SendGenericCommand();
			return pH323MediaCall->StopSecondVideo();
		}
	}

#endif

	return -1;
}

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )

//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#else


/******************************************************************************
* SetLocalMainVideoWnd
*描述：设置本地主流视频显示的窗口
*输入：	cszCallID		-呼叫标识
		hWnd			-本地主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetLocalMainVideoWnd(const char*cszCallID,HWND hWnd)
{
	int nRet=-1;
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		nRet=pH323MediaCall->SetLocalMainVideoWnd(hWnd);
		return nRet;
	}
	return -1;
}

/******************************************************************************
* SetLocalMainVideoWndPos
*描述：设置本地主流视频显示的窗口位置
*输入：	cszCallID		-呼叫标识
		x				-窗口的左边位置
		y				-窗口的上边位置
		cx				-窗口的宽度
		cy				-窗口的高度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetLocalMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->SetLocalMainVideoWndPos(x,y,cx,cy);
	}
	return -1;
}


/******************************************************************************
* SetLocalMainVideoParam
*描述：设置本地主流视频参数
*输入：	cszCallID		-呼叫标识
		nBrightness		-亮度值0-255
		nContrast		-对比度0-127
		nSaturation		-饱和度0-127
		nHue			-色调0-255
		nGamma			-伽马1-400
		nSharpen		-锐化0-127
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetLocalMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetLocalMainVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
	}
}

/******************************************************************************
* SetLocalMainVideoEnableOSD
*描述：设置本地主流视频是否启用OSD
*输入：	cszCallID		-呼叫标识
		nEnable			-是否启用OSD
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetLocalMainVideoEnableOSD(const char*cszCallID,int nEnable)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetLocalMainVideoEnableOSD(nEnable);
	}
}

/******************************************************************************
* SetLocalMainVideoOSDText
*描述：设置本地主流视频OSD文字
*输入：	cszCallID		-呼叫标识
		cszText			-OSD文字
		nAlign			-对齐方式
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetLocalMainVideoOSDText(const char*cszCallID,const char*cszText,int nAlign)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetLocalMainVideoOSDText(cszText,nAlign);
	}
}

/******************************************************************************
* SetLocalMainVideoOSDFont
*描述：设置本地主流视频OSD字体,字的颜色和背景色
*输入：	cszCallID		-呼叫标识
		logfont			-OSD字体
		crText			-字的颜色
		crBk			-背景色
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetLocalMainVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetLocalMainVideoOSDFont(logfont,crText,crBk);
	}
}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

/******************************************************************************
* GetLocalMainVideoInfo
*描述：获取本地主流视频信息
*输入：	cszCallID		-呼叫标识
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	int nRet=-1;
	int nFrameRate =0;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
return 0;
#else

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		nRet = pH323MediaCall->GetLocalMainVideoInfo(nWidth,nHeight,nVideoCodecType,hVideoWnd,nFrameRate);
		nRet = pH323MediaCall->GetLocalMainVideoResolution(nWidth,nHeight,chInfo);
		if(nFrameRate >0) chInfo.m_nFrame =nFrameRate;
	}
#endif

	return nRet;
}


/******************************************************************************
* GetLocalMainVideoPacketStatistics
*描述：获取本地主视频的包的统计数据
*输入：	cszCallID			-呼叫标识
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->GetLocalMainVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		pH323MediaCall->GetMainVideoPacketStatistics(0,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return 0;
}

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )

//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#else


/******************************************************************************
* SetRemoteMainVideoWnd
*描述：设置远程主流视频显示的窗口
*输入：	cszCallID		-呼叫标识
		hWnd			-远程主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetRemoteMainVideoWnd(const char*cszCallID,HWND hWnd)
{
	m_hTestWnd =hWnd;//test
	int nRet=-1;
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		nRet=pH323MediaCall->SetRemoteMainVideoWnd(hWnd);
		return nRet;
	}
	return -1;
}

/******************************************************************************
* SetRemoteMainVideoWndPos
*描述：设置远程主流视频显示的窗口位置
*输入：	cszCallID		-呼叫标识
		x				-窗口的左边位置
		y				-窗口的上边位置
		cx				-窗口的宽度
		cy				-窗口的高度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetRemoteMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->SetRemoteMainVideoWndPos(x,y,cx,cy);
	}
	return -1;
}

/******************************************************************************
* SetRemoteMainVideoParam
*描述：设置远程主流视频参数
*输入：	cszCallID		-呼叫标识
		nBrightness		-亮度值0-255
		nContrast		-对比度0-127
		nSaturation		-饱和度0-127
		nHue			-色调0-255
		nGamma			-伽马1-400
		nSharpen		-锐化0-127
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetRemoteMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetRemoteMainVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}
}


#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


/******************************************************************************
* GetRemoteMainVideoInfo
*描述：获取远程主流视频信息
*输入：	cszCallID		-呼叫标识
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	int nRet=0;
	int nFrameRate =0;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
		return nRet;
#else//

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		nRet = pH323MediaCall->GetRemoteMainVideoInfo(nWidth,nHeight,nVideoCodecType,hVideoWnd,nFrameRate);
		nRet = pH323MediaCall->GetRemoteMainVideoResolution(nWidth,nHeight,chInfo);
		if(nFrameRate >0) chInfo.m_nFrame =nFrameRate;
	}
#endif

	return nRet;
}


/******************************************************************************
* GetRemoteMainVideoPacketStatistics
*描述：获取远端主视频的包的统计数据
*输入：	cszCallID			-呼叫标识
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->GetRemoteMainVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		pH323MediaCall->GetMainVideoPacketStatistics(1,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return 0;
}


#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )

//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#else

/******************************************************************************
* SetSecondVideoWnd
*描述：设置辅流视频显示的窗口
*输入：	cszCallID		-呼叫标识
		hWnd			-远程辅流视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetSecondVideoWnd(const char*cszCallID,HWND hWnd)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->SetSecondVideoWnd(hWnd);
	}
	return -1;
}

/******************************************************************************
* SetSecondVideoWndPos
*描述：设置辅流视频显示的窗口位置
*输入：	cszCallID		-呼叫标识
		x				-窗口的左边位置
		y				-窗口的上边位置
		cx				-窗口的宽度
		cy				-窗口的高度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetSecondVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->SetSecondVideoWndPos(x,y,cx,cy);
	}
	return -1;
}

/******************************************************************************
* SetSecondVideoParam
*描述：设置辅流视频参数
*输入：	cszCallID		-呼叫标识
		nBrightness		-亮度值0-255
		nContrast		-对比度0-127
		nSaturation		-饱和度0-127
		nHue			-色调0-255
		nGamma			-伽马1-400
		nSharpen		-锐化0-127
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetSecondVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetSecondVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
	}
}

/******************************************************************************
* SetSecondVideoEnableOSD
*描述：设置辅流视频是否启用OSD
*输入：	cszCallID		-呼叫标识
		nEnable			-是否启用OSD
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetSecondVideoEnableOSD(const char*cszCallID,int nEnable)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetSecondVideoEnableOSD(nEnable);
	}
}

/******************************************************************************
* SetSecondVideoOSDText
*描述：设置辅流视频OSD文字
*输入：	cszCallID		-呼叫标识
		cszText			-OSD文字
		nAlign			-对齐方式
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetSecondVideoOSDText(const char*cszCallID,const char*cszText,int nAlign)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetSecondVideoOSDText(cszText,nAlign);
	}
}

/******************************************************************************
* SetSecondVideoOSDFont
*描述：设置辅流视频OSD字体,字的颜色和背景色
*输入：	cszCallID		-呼叫标识
		logfont			-OSD字体
		crText			-字的颜色
		crBk			-背景色
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetSecondVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetSecondVideoOSDFont(logfont,crText,crBk);
	}
}

/******************************************************************************
* HotDisplayVideo
*描述：视频显示支持显示器热插拔接口
*输入：	cszCallID		    -呼叫标识
hWndLocal			-本地主视频显示的窗口句柄
hWndRemote			-远程主视频显示的窗口句柄
hWndSecond         	-远程辅流视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::HotDisplayVideo(const char*cszCallID,HWND hWndLocal,HWND hWndRemote,HWND hWndSecond)
{
	int nRet=0;
	H323MediaCall*pH323MediaCall=NULL;
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		pH323MediaCall=iter->second;

	}
	if(pH323MediaCall)
	{
		if(hWndRemote) nRet=pH323MediaCall->SetRemoteMainVideoWnd(hWndRemote);
		if(hWndLocal) nRet=pH323MediaCall->SetLocalMainVideoWnd(hWndLocal);
		if(hWndSecond) nRet=pH323MediaCall->SetSecondVideoWnd(hWndSecond);
		if (pH323MediaCall->HasAVCONTransport())
		{
			pH323MediaCall->OpenRemoteMainPlayChan();
			pH323MediaCall->CloseRemoteSecondPlayChan();
			pH323MediaCall->OpenRemoteSecondPlayChan();
		}
		else
		{
			pH323MediaCall->OnMainVideoRecvChannelDestroyed();
			pH323MediaCall->OnMainVideoRecvChannelConnected();

			pH323MediaCall->OnSecondVideoRecvChannelDestroyed();
			pH323MediaCall->OnSecondVideoRecvChannelConnected();
		}
		return nRet;
	}
	return -1;
}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)



/******************************************************************************
* GetSecondVideoInfo
*描述：获取辅流视频信息
*输入：	cszCallID		-呼叫标识
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	int nRet=-1;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
		return 0;
#else//

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		nRet = pH323MediaCall->GetSecondVideoInfo(nWidth,nHeight,nVideoCodecType,hVideoWnd);
		nRet = pH323MediaCall->GetSecondVideoResolution(nWidth,nHeight,chInfo);
	}
#endif

	return nRet;
}


/******************************************************************************
* GetSecondVideoPacketStatistics
*描述：获取辅流视频的包的统计数据
*输入：	cszCallID			-呼叫标识
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->GetSecondVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		pH323MediaCall->GetDualVideoPacketStatistics(0,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return 0;
}


/******************************************************************************
* GetLocalAudioLevel
*描述：获取本地音频Level【实际声音大小】
*输入：	cszCallID		-呼叫标识
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetLocalAudioLevel(const char*cszCallID,int&nLevel)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
		return 0;
#else//

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{

		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->GetLocalAudioLevel(nLevel);

	}
#endif
	return 0;
}

/******************************************************************************
* SetLocalAudioVolume
*描述：获取本地音频音量
*输入：	cszCallID		-呼叫标识
		nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetLocalAudioVolume(const char*cszCallID,int nVolume)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
		return ;
#else//

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetLocalAudioVolume(nVolume);
	}
#endif
}

/******************************************************************************
* SetLocalAudioMute
*描述：获取本地音频是否静音
*输入：	cszCallID		-呼叫标识
		nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetLocalAudioMute(const char*cszCallID,int nMute)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return ;
#else//#endif

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetLocalAudioMute(nMute);
	}
#endif
}

/******************************************************************************
* GetLocalAudioCodecID
*描述：获取本地音频编码信息
*输入：	cszCallID		-呼叫标识
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return 0;
#else//

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->GetLocalAudioCodecID(nAudioCodecID);
	}
#endif
	return 0;
}

/******************************************************************************
* GetLocalAudioPacketStatistics
*描述：获取本机音频的包的统计数据
*输入：	cszCallID			-呼叫标识
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->GetLocalAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return 0;
}

/******************************************************************************
* GetRemoteAudioLevel
*描述：获取远程音频Level【实际声音大小】
*输入：	cszCallID		-呼叫标识
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetRemoteAudioLevel(const char*cszCallID,int&nLevel)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return 0;
#else//

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->GetRemoteAudioLevel(nLevel);
	}
#endif
	return 0;
}

/******************************************************************************
* SetRemoteAudioVolume
*描述：获取远程音频音量
*输入：	cszCallID		-呼叫标识
		nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetRemoteAudioVolume(const char*cszCallID,int nVolume)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return ;
#else//

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetRemoteAudioVolume(nVolume);
	}
#endif
}

/******************************************************************************
* SetRemoteAudioMute
*描述：获取远程音频是否静音
*输入：	cszCallID		-呼叫标识
		nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetRemoteAudioMute(const char*cszCallID,int nMute)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return ;
#else//

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetRemoteAudioMute(nMute);
	}
#endif
}

/******************************************************************************
* GetRemoteAudioCodecID
*描述：获取远端音频编码
*输入：	cszCallID		-呼叫标识
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return 0;
#else//

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->GetRemoteAudioCodecID(nAudioCodecID);
	}
#endif
	return 0;
}

/******************************************************************************
* GetRemoteAudioPacketStatistics
*描述：获取远端音频的包的统计数据
*输入：	cszCallID			-呼叫标识
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->GetRemoteAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return 0;
}

/******************************************************************************
* GetCallSupportSecondVideo
*描述：获取呼叫是否支持辅流【H239】
*输入：	cszCallID		-呼叫标识
*输出：	无
*返回值：支持返回非0，不支持返回0
*****************************************************************************/
int H323CallMgr::GetCallSupportSecondVideo(const char*cszCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
	XAutoLock l(m_csMapH323MediaCall);
#endif

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->GetSupportSecondVideo();
	}
	return 0;
}

unsigned long H323CallMgr::GetLocalIPAddress(const std::string&strCallLocalIP)
{
	std::string strLocalIP=strCallLocalIP;
	if (strLocalIP.size()>0 && strLocalIP!="127.0.0.1" && strLocalIP!="0.0.0.0")
	{
		return htonl(XSocket::atoh(strLocalIP.c_str()));
	}

	strLocalIP=NETEC_Node::GetLocalIP();
	if (strLocalIP.size()>0 && strLocalIP!="127.0.0.1" && strLocalIP!="0.0.0.0")
	{
		return htonl(XSocket::atoh(strLocalIP.c_str()));
	}

	strLocalIP=m_strLocalIP;
	if (strLocalIP.size()>0 && strLocalIP!="127.0.0.1" && strLocalIP!="0.0.0.0")
	{
		return htonl(XSocket::atoh(strLocalIP.c_str()));
	}

	unsigned long nLocalIP=0;
	SOCKET hSocket=INVALID_SOCKET;
	if((hSocket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP))!=INVALID_SOCKET)
	{
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("8.8.8.8");
		addr.sin_port = htons(53);
		connect(hSocket,(sockaddr *) &addr, sizeof(addr));

#ifdef WIN32	
		int nAddrLen = sizeof (sockaddr);
#else
		socklen_t nAddrLen = sizeof (sockaddr);
#endif
		if (getsockname (hSocket, (sockaddr*)&addr, &nAddrLen) != SOCKET_ERROR)
		{
			nLocalIP = addr.sin_addr.s_addr;
		}

#ifdef WIN32
		closesocket (hSocket);
#else
		close(hSocket);
#endif
		hSocket=INVALID_SOCKET;	
	}

	if (nLocalIP!=0)
	{
		return nLocalIP;
	}

	cmRASTransport tr;
	cmGetLocalCallSignalAddress(m_hApp,&tr);

	return tr.ip;
}

int H323CallMgr::GetAudioMaxBitrate(void)
{
	return m_nAudioMaxBitrate;
}

int H323CallMgr::GetMainVideoMaxBitrate(void)
{
	return m_nMainVideoMaxBitrate;
}

int H323CallMgr::GetSecondVideoMaxBitrate(void)
{
	return m_nSecondVideoMaxBitrate;
}

int H323CallMgr::Start(void)
{
	int nRet=-1;
	m_nH245PortFrom=GetH245BasePort();
	m_nH245PortTo=m_nH245PortFrom + m_nMaxCallNum * 2; //notice terminal MP use modify 20140627
	nRet=InsideStart(m_nH245PortFrom,m_nH245PortTo);
	RegisterGK();
	return nRet;
}

void OutPutH323StackLog(char *logStr, ...)
{
	char msg[2048];
	va_list varArg;
	va_start(varArg, logStr);
	vsnprintf(msg, sizeof(msg), logStr, varArg);
	va_end(varArg);
	IPCPrintf(1, 0, "[H323Stack][LOG] %s\n", msg);
	//OutputDebugString(msg);
}

////////////////////////////////////////////////////////////////////////////////
int H323CallMgr::InsideStart(int fromPort,int toPort)
{
	//OutputDebugString(OPENSSL_VERSION_TEXT);
	if (m_nStarted!=0)
		return 0;

	m_bNETECNodeToGK =false;
	m_nNETECNodeCount =0;

    if (FALSE == IsIPCInitd())
    {
        if (FALSE == IPCInit(FALSE,CallMgr::Instance().m_nIPCPort))
        {
        }
    }

	HostWindowsGetIpV4List(CallMgr::Instance().m_AllLocalIPSave);

	m_nSendURQ =0;

	m_nREGRejectReason =0;

	//设置H323注册相关端口 临时用
	H323SettingFromINI();

	LogFile(NULL,NULL,"","","H323CallMgr::InsideStart");

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else

	if(m_nUseGK <=0 ) m_nUseTerminalFWNAT =0;

#endif

	char*pConfig=NULL;
	int nConfigSize=0;
	HCFG hCfg = NULL;
	hCfg = ciConstruct((char*)ConfigurationArray);
	//msSetStackNotify(OutPutH323StackLog);

	if (hCfg)
	{
		int nGKPortTemp=0;
		ciGetValue(hCfg, "RAS.manualDiscovery.defaultGatekeeper.ipAddress.port", NULL, &nGKPortTemp);
		if(nGKPortTemp<=0) ciSetValue( hCfg, "RAS.manualDiscovery.defaultGatekeeper.ipAddress.port", RV_FALSE,1719, NULL);

		ciSetValue(hCfg, "system.allocations.maxBuffSize",RV_FALSE,2048 *2, NULL); //可调使之可以添加更多能力集
		ciSetValue( hCfg, "system.portFrom", RV_FALSE, fromPort, NULL);
		ciSetValue( hCfg, "system.portTo", RV_FALSE, toPort, NULL);

		//ciSetValue( hCfg, "system.maxRasOutTransactions", RV_FALSE, 22, NULL);//notice test 20140625

		if(m_nRASLocalPort != 1714 ) ciSetValue( hCfg, "RAS.rasPort", RV_FALSE, m_nRASLocalPort, NULL);
		if(m_nRASCastPort != 1718 ) ciSetValue( hCfg, "RAS.rasMulticastAddress.ipAddress.port", RV_FALSE, m_nRASCastPort, NULL);
		if(m_nGateKeeperPort != 1719 ) ciSetValue( hCfg, "RAS.manualDiscovery.defaultGatekeeper.ipAddress.port", RV_FALSE, m_nGateKeeperPort, NULL);

		int maxListeningAddresses=10;
		int error=ciSetValue( hCfg, "system.allocations.maxMultiHomedAddresses", RV_FALSE, maxListeningAddresses,NULL);

		error=ciSetValue( hCfg, "RAS.registrationInfo.terminalType.vendor.vendor.t35CountryCode", RV_FALSE, m_nT35Code, NULL);
		error=ciSetValue( hCfg, "RAS.registrationInfo.terminalType.vendor.vendor.t35Extension", RV_FALSE,m_nT35Extension, NULL);
		error=ciSetValue( hCfg, "RAS.registrationInfo.terminalType.vendor.vendor.manufacturerCode",RV_FALSE, m_nManufacturerCode, NULL);
		error=ciSetValue( hCfg, "Q931.callSignalingPort", RV_FALSE, m_nH323ListenPort, NULL);
		error=ciSetValue( hCfg, "RAS.registrationInfo.terminalType.vendor.productId", RV_TRUE
			,m_strProductID.length(), m_strProductID.c_str());
		error=ciSetValue( hCfg, "RAS.registrationInfo.terminalType.vendor.versionId", RV_TRUE
			,m_strProductVersionID.length(), m_strProductVersionID.c_str());

		error=ciSetValue( hCfg, "RAS.registrationInfo.timeToLive", RV_FALSE,60, NULL);

		//删除terminalType信息
		error=ciGetValue(hCfg, "RAS.registrationInfo.terminalType.mcu",NULL,NULL);
		if(error >=0) ciDeleteValue(hCfg, "RAS.registrationInfo.terminalType.mcu");
		error=ciGetValue(hCfg, "RAS.registrationInfo.terminalType.terminal",NULL,NULL);
		if(error >=0) ciDeleteValue(hCfg, "RAS.registrationInfo.terminalType.terminal");
		//设置terminalType信息
		if(m_nEndpontType ==0 )
			error = ciSetValue(hCfg, "RAS.registrationInfo.terminalType.terminal", RV_FALSE, 0, NULL);
		else if(m_nEndpontType ==1 )
			error = ciSetValue(hCfg, "RAS.registrationInfo.terminalType.mcu", RV_FALSE, 0, NULL);

		if ((m_nUseGK>0 && m_strRegHost.size()>0) || m_nUseGK==1)
			ciDeleteValue(hCfg,"RAS.manualRAS");

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

		int nRASMaxRetries =1;
		int nRASMaxTimeOut =1;
		error = ciSetValue(hCfg, "RAS.manualRAS", RV_FALSE, 1, NULL);
		error = ciSetValue(hCfg, "RAS.manualRegistration", RV_FALSE, 1, NULL);
		error = ciSetValue(hCfg, "RAS.maxRetries", RV_FALSE, nRASMaxRetries, NULL);
		//error = ciSetValue(hCfg, "RAS.responseTimeOut", RV_FALSE, nRASMaxTimeOut, NULL);

#endif

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 2) || (HAS_H323_AVCON_MP == 1) )

		error=ciSetValue( hCfg, "system.maxCalls", RV_FALSE, m_nMaxCallNum, NULL);
		error=ciSetValue( hCfg, "Q931.maxConnectingCalls", RV_FALSE, m_nMaxCallNum, NULL);

#else
		//notice test debug bug ?
		error=ciSetValue( hCfg, "system.maxCalls", RV_FALSE, 2, NULL);
		error=ciSetValue( hCfg, "Q931.maxConnectingCalls", RV_FALSE, 2, NULL);
		error=ciSetValue( hCfg, "system.maxChannels", RV_FALSE, 100, NULL);

#endif

		if(ciAllocateTargetBuffer(hCfg, (void**)&pConfig, &nConfigSize) >= 0)
		{
			if(ciSave(hCfg, pConfig) >= 0)
			{
			}
		}
	}

	if (nConfigSize<=0)
	{
		pConfig=(char*)ConfigurationArray;
	}

	if (cmInitialize(pConfig, &m_hApp) < 0 )
	{
		if (hCfg)
		{
			if (nConfigSize>0)
			{
				ciFreeTargetBuffer(hCfg, pConfig);
			}
			ciDestruct(hCfg);
		}

		m_hApp = NULL;
		OutPutStackLog(1,"H323Stack Initialize Failure!");
		LogFile(NULL,NULL,"","","H323Stack Initialize Failure!");
		return -1;
	}

	if (hCfg)
	{
		if (nConfigSize>0)
		{
			ciFreeTargetBuffer(hCfg, pConfig);
		}
		ciDestruct(hCfg);
	}

	//CMCALLEVENT
	SCMCALLEVENT cmCallEvent;
	memset(&cmCallEvent, 0, sizeof(cmCallEvent));
	cmCallEvent.cmEvCallStateChanged = cmEvCallStateChanged;
	cmCallEvent.cmEvCallUserInfo = cmEvCallUserInfo;
	cmCallEvent.cmEvCallFacility = cmEvCallFacility;
	cmCallEvent.cmEvCallSendMessage = cmEvCallSendMessage;     //for 460client
	cmCallEvent.cmEvCallRecvMessage = cmEvCallRecvMessage;     //检测挂断消息
	cmSetCallEventHandler(m_hApp, &cmCallEvent, sizeof(SCMCALLEVENT));	

	//CMEVENT
	SCMEVENT cmEvent;
	memset(&cmEvent, 0, sizeof(cmEvent));
	cmEvent.cmEvNewCall = cmEvNewCall;
	cmEvent.cmEvRegEvent= cmEvRegEvent;
	cmSetGenEventHandler(m_hApp, &cmEvent, sizeof(SCMEVENT));

	//CMCONTROLEVENT
	SCMCONTROLEVENT cmControlEvent;
	memset(&cmControlEvent, 0, sizeof(cmControlEvent));
	cmControlEvent.cmEvCallCapabilities     = cmEvCallCapabilities;
	cmControlEvent.cmEvCallCapabilitiesExt  = cmEvCallCapabilitiesExt;
	cmControlEvent.cmEvCallNewChannel		= cmEvCallNewChannel;
	cmControlEvent.cmEvCallCapabilitiesResponse	= cmEvCallCapabilitiesResponse;
	cmControlEvent.cmEvCallMasterSlaveStatus   = cmEvCallMasterSlaveStatus;
	cmControlEvent.cmEvCallControlMessage   = cmEvCallControlMessage;
	cmControlEvent.cmEvCallControlStateChanged   = cmEvCallControlStateChanged;
	cmControlEvent.cmEvCallMasterSlave		= cmEvCallMasterSlave;
	cmControlEvent.cmEvCallTerminalNumberAssign		= cmEvCallTerminalNumberAssign;
	cmControlEvent.cmEvCallUserInput		= cmEvCallUserInput;
	cmControlEvent.cmEvCallRoundTripDelay		= cmEvCallRoundTripDelay;
	cmSetControlEventHandler(m_hApp, &cmControlEvent, sizeof(SCMCONTROLEVENT));

	//CMCHANEVENT
	SCMCHANEVENT cmChannEvent;
	memset(&cmChannEvent, 0, sizeof(cmChannEvent));
	cmChannEvent.cmEvChannelParameters		= cmEvChannelParameters;
	cmChannEvent.cmEvChannelSetAddress		= cmEvChannelSetAddress;
	cmChannEvent.cmEvChannelSetRTCPAddress	= cmEvChannelSetRTCPAddress;
	cmChannEvent.cmEvChannelStateChanged	= cmEvChannelStateChanged;
	cmChannEvent.cmEvChannelHandle			= cmEvChannelHandle;
	cmChannEvent.cmEvChannelRequestCloseStatus       = cmEvChannelRequestCloseStatus;
	cmChannEvent.cmEvChannelMiscCommand		= cmEvChannelMiscCommand;
	cmChannEvent.cmEvChannelSendMessage     = cmEvChannelSendMessage;
	cmChannEvent.cmEvChannelRecvMessage		= cmEvChannelRecvMessage;
	cmChannEvent.cmEvChannelNewRate         = cmEvChannelNewRate;
	cmSetChannelEventHandler(m_hApp, &cmChannEvent, sizeof(SCMCHANEVENT));


	//CMPROTOCOLEVENT
	SCMPROTOCOLEVENT cmProtocolEvent;
	memset (&cmProtocolEvent, 0, sizeof(cmProtocolEvent));
	cmProtocolEvent.hookListen		= cmHookListen;
	cmProtocolEvent.hookListening   = cmHookListening;
	cmProtocolEvent.hookConnecting  = cmHookConnecting;
	cmProtocolEvent.hookInConn      = cmHookInConn;
	cmProtocolEvent.hookOutConn     = cmHookOutConn;
	cmProtocolEvent.hookSend        = cmHookSend;
	cmProtocolEvent.hookRecv        = cmHookRecv;
	cmProtocolEvent.hookSendTo      = cmHookSendTo;
	cmProtocolEvent.hookRecvFrom    = cmHookRecvFrom;
	cmProtocolEvent.hookClose       = cmHookClose;
	cmSetProtocolEventHandler(m_hApp,&cmProtocolEvent,sizeof(cmProtocolEvent));

	//SCMRASEVENT
	SCMRASEVENT cmRASEvent;
	memset(&cmRASEvent, 0, sizeof(cmRASEvent));
	cmRASEvent.cmEvRASRequest= cmEvRASRequest;
	cmRASEvent.cmEvRASConfirm= cmEvRASConfirm;
	cmRASEvent.cmEvRASReject= cmEvRASReject;
	cmRASEvent.cmEvRASTimeout= cmEvRASTimeout;
	cmRASEvent.cmEvRASMessageSend= cmEvRASMessageSend;
	cmRASSetEventHandler(m_hApp, &cmRASEvent, sizeof(cmRASEvent));

	//SCMAUTORASEVENT
	SCMAUTORASEVENT cmAutoRASEvent;
	memset(&cmAutoRASEvent, 0, sizeof(cmAutoRASEvent));
	cmAutoRASEvent.cmEvAutoRASRequest = cmEvAutoRASRequest;
	//cmAutoRASEvent.cmEvAutoRASConfirm = cmEvAutoRASConfirm;
	cmAutoRASSetEventHandler(m_hApp, &cmAutoRASEvent, sizeof(cmAutoRASEvent));

	//H460
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else

	if( m_nUseTerminalFWNAT > 0)
		//CallMgr::Instance().m_nUseNETECSend =0;

#endif

	{
		int status=-1;
		RvH323FwNatCallbacks cmFwEvent;
		RvH323FwNatConfiguration tFwNatConfig;
		memset(&cmFwEvent, 0, sizeof(cmFwEvent));

		cmFwEvent.fwNatGetByCallEv = NULL;
		cmFwEvent.fwNatSetByCallEv = NULL;
		cmFwEvent.fwNatGetProtConnHandleEv = NULL;
		cmFwEvent.fwNatSetProtConnHandleEv = NULL;
		cmFwEvent.fwNatRasEncapStatusEv = NULL;

		tFwNatConfig.bGatekeeper = FALSE;
		tFwNatConfig.e17Config = RvH323FwNatH46017ConfigNotUsed;
		tFwNatConfig.e18Config = RvH323FwNatH46018ConfigSupported;
		tFwNatConfig.e19Config = RvH323FwNatH46019ConfigClient;
		tFwNatConfig.e19MultiplexMediaConfig = RvH323FwNatH46019MediaMultiplexModeConfigNone;

		status=RvH323FwNatInitialize(m_hApp, &tFwNatConfig, &cmFwEvent, 256, 2048, &m_hFwNat);
		if(status < 0 ) m_hFwNat =NULL;

	}

	rtpInit(); 
	rtcpInit();

////notice mcu not use wait stable
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//
//#else

    //use medianetrtp
    //if (FALSE == IsIPCInitd())
    //{
    //    if (FALSE == IPCInit(0,CallMgr::Instance().m_nIPCPort))
    //    {
    //    }
    //}
	if (0 !=  HpSocketStartup(FALSE))
	{
	}
//#endif

	m_bRunning=true;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
	if (!StartThread())
	{
		OutPutStackLog(1,"H323Stack Start MSG Thread Failure!");
		return -1;
	}

#else
	m_nIDEventTimer=SetTimer(NULL,1,1,H323CallMgrTimerProc);
#endif

//Use to mcu start H235
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	m_nH235AuthUse =1;
	SetH235SecurityMediaDH1024Enable(true);
#endif

	if(m_nH235AuthUse>0)
	{
		m_h235Mgr.H235SecurityInit(m_hApp);	
		SetH235AuthData();
		m_h235Mgr.H235SecuritySettingSubmit();
	}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	InitH239VirtualCallState();

#endif

	m_nStarted=1;
	OutPutStackLog(0,"H323Stack Initialize Success!");

	return 0;
}

void H323CallMgr::Stop(void)
{
	if (m_nStarted==0)
	{
		return ;
	}

	m_nStarted=0;
	m_nSendURQ=0;

	m_strConfID="";
	m_strPeerMCUAddr ="";

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if (m_nIDDtmfTimer!=0)
	{
		KillTimer(NULL,m_nIDDtmfTimer);
		m_nIDDtmfTimer=0;
	}

#endif

	//gk mcu use 20150409 
	NETECStopToGK();

	if(m_nUseGK && m_hApp!=NULL)
	{
		cmUnregister(m_hApp);
	}

	HungupAll();

	//add gk more account support
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	//notice prevent add lock to lock
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csH323RegUser);
#endif

		while(true)
		{
			H323RegUser* pH323RegUser=NULL;
			MapH323RegUser::iterator iter=m_MapH323RegUser.begin();
			if(iter!=m_MapH323RegUser.end())
			{
				pH323RegUser=iter->second;
				m_MapH323RegUser.erase(iter);
			}
			else
				break;

			if(pH323RegUser)
			{
				//pH323RegUser->SendRegMessage(cmRASUnregistration);
				delete pH323RegUser;
				pH323RegUser=NULL;
			}
		}
	}
#endif

	m_nRegAccountNum=0;


	m_h235Mgr.H235SecurityEnd();

	rtcpEnd();
	rtpEnd();

	//for H460
	HH323FWNATMGR hFwNat =m_hFwNat;
	m_hFwNat =NULL;
	if(hFwNat) RvH323FwNatEnd(hFwNat);
	hFwNat=NULL;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) 
	m_bRunning=false;
	WaitForStop(); 
#else 
	if (m_nIDEventTimer!=0)
	{
		KillTimer(NULL,m_nIDEventTimer);
		m_nIDEventTimer=0;
	}
#endif

	if (m_hApp!=NULL)
	{
		cmEnd(m_hApp);
		m_hApp = NULL;
	}
	m_bRegisteredToGK = false;

	while (true)
	{
		XDataBuffer*pXDataBuffer=GetDataBuffer();
		if (pXDataBuffer!=NULL)
		{
			delete pXDataBuffer;
			pXDataBuffer=NULL;
		}
		else
			break;
	}

	ClosLogFile();

	HpSocketCleanup(FALSE);//use medianetrtp add
	IPCQuit(FALSE);

	OutPutStackLog(0,"H323Stack Stop Success!");
}

//回调函数 注册状态事件
void H323CallMgr::OnRegistrationState(bool bState)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	//if(m_bRegisteredToGK)
	//	OnDAP_H323RegistrationSuccess(m_strH323ID.c_str(),m_strTelNumber.c_str());
	//else 
	//	OnDAP_H323RegistrationError(m_strH323ID.c_str(),m_strTelNumber.c_str());
#else

	m_h235Mgr.m_bIsRegistered=bState;
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","REG");

		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

#endif
}


void H323CallMgr::OnCallRingIn(const std::string& strCallID, const std::string& strUserID,const std::string& strUserName)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","IN");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);

		std::string strText="";
		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack OnCallRingIn,",szCallIDBack);
}

void H323CallMgr::OnCallRingOut(const std::string& strCallID, const std::string& strUserID,const std::string& strUserName)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","OUT");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);

		std::string strText="";
		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack OnCallRingOut,",szCallIDBack);
}

//add get thread count log 
#include <tlhelp32.h>

int GetProcessThreadCount(int type)
{
	int ret = 0;
	CString strPath = _T("");
	GetModuleFileName(NULL,strPath.GetBufferSetLength(512 + 1),512);
	int n = strPath.ReverseFind('\\');
	strPath = strPath.Right(strPath.GetLength() - n -1);
	char * exeName = strPath.GetBuffer();

	PROCESSENTRY32 proc;
	HANDLE hSysSnapshot = NULL;
	hSysSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 );
	if (hSysSnapshot != (HANDLE)-1 )
	{
		proc.dwSize = sizeof(proc);
		if ( Process32First ( hSysSnapshot, &proc ) )  
		{
			HANDLE hSysSnapshot2 = NULL;
			do  
			{
				if (stricmp(proc.szExeFile, exeName)==0) 
				{
					HANDLE Proc_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc.th32ProcessID);
					if (Proc_handle !=NULL)
					{
						hSysSnapshot2 = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, proc.th32ProcessID );
						ret = proc.cntThreads;
						CloseHandle(Proc_handle);
						if (hSysSnapshot2 != (HANDLE)-1 ) CloseHandle(hSysSnapshot2);
						break;
					}
				} 
			} while ( Process32Next ( hSysSnapshot, &proc ) );
		}
	}
	if (hSysSnapshot != (HANDLE)-1 )  CloseHandle(hSysSnapshot);

	char strCount [32]={0};
	char * info ="";
	if(type ==0) info ="CallEstablished ProcessThreadCount ************************************";
	if(type ==1) info ="CallDestroyed   ProcessThreadCount ************************************";

	if(itoa(ret,strCount,10) ) LogFile(NULL,NULL,"",strCount,info);

	return ret;
}

void H323CallMgr::OnCallEstablished (const std::string& strCallID,const std::string& strUserID,const std::string& strUserName)
{
	//GetProcessThreadCount(0);
	//OutputDebugString("H323Stack OnCallEstablished \n");

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","OK");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);

		std::string strText="";
		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack OnCallEstablished,",szCallIDBack);
}

void H323CallMgr::OnCallDestroyed (const std::string& strCallID,const std::string& strUserID,const std::string& strUserName)
{
	//GetProcessThreadCount(1);
	//OutputDebugString("H323Stack OnCallDestroyed \n");

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","BYE");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);

		std::string strText="";

		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack OnCallDestroyed,",szCallIDBack);
}

void H323CallMgr::OnCallSecondVideoStart(const std::string&strCallID)
{
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	//Modified by WangLr 2013-6-27。消息回调不独立一个线程处理，直接从底层回调上去，上层会分线程的
	CallMgr::Instance().OnCallSecondVideoStart(strCallID);
#else

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","H239");
		tStrPacket.Set("STATE","ON");
		tStrPacket.Set("CID",strCallID);

		std::string strText="";

		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

#endif

}

void H323CallMgr::OnCallSecondVideoStop(const std::string&strCallID)
{
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	//Modified by WangLr 2013-6-27。消息回调不独立一个线程处理，直接从底层回调上去，上层会分线程的
	CallMgr::Instance().OnCallSecondVideoStop(strCallID);
#else

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","H239");
		tStrPacket.Set("STATE","OFF");
		tStrPacket.Set("CID",strCallID);

		std::string strText="";

		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
#endif
}

void H323CallMgr::OnCallLocalMainVideoStart(const std::string&strCallID)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","LOCAL_MAIN_VIDEO");
		tStrPacket.Set("STATE","ON");
		tStrPacket.Set("CID",strCallID);

		std::string strText="";

		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void H323CallMgr::OnCallLocalMainVideoStop(const std::string&strCallID)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","LOCAL_MAIN_VIDEO");
		tStrPacket.Set("STATE","OFF");
		tStrPacket.Set("CID",strCallID);

		std::string strText="";

		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2))

#else


/******************************************************************************
* StartRecordLocalMainToAVI
*描述：开始本地主流录像
*输入：	cszCallID			-呼叫标识
		cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::StartRecordLocalMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->StartRecordLocalMainToAVI(cszPathName,nStandardFormat);
	}
	return -1;
}

/******************************************************************************
* StopRecordLocalMainToAVI
*描述：停止本地主流录像
*输入：cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::StopRecordLocalMainToAVI(const char*cszCallID)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->StopRecordLocalMainToAVI();
	}

}

/******************************************************************************
* StartRecordRemoteMainToAVI
*描述：开始远端主流录像
*输入：	cszCallID			-呼叫标识
		cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::StartRecordRemoteMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->StartRecordRemoteMainToAVI(cszPathName,nStandardFormat);
	}
	return -1;
}

/******************************************************************************
* StopRecordRemoteMainToAVI
*描述：停止远端主流录像
*输入：cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::StopRecordRemoteMainToAVI(const char*cszCallID)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->StopRecordRemoteMainToAVI();
	}
}

/******************************************************************************
* StartRecordSecondToAVI
*描述：开始辅流录像
*输入：	cszCallID			-呼叫标识
		cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::StartRecordSecondToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->StartRecordSecondToAVI(cszPathName,nStandardFormat);
	}
	return -1;
}

/******************************************************************************
* StopRecordSecondToAVI
*描述：停止辅流录像
*输入：cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::StopRecordSecondToAVI(const char*cszCallID)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->StopRecordSecondToAVI();
	}
}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)



/******************************************************************************
* GetMainAudioID
*描述：获取主流音频标识
*输入： cszCallID		-呼叫标识
*输出： nMainAudioID	-主流音频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->GetMainAudioID(nMainAudioID);
	}
	return -1;
}

/******************************************************************************
* GetMainVideoID
*描述：获取主流视频标识
*输入： cszCallID		-呼叫标识
*输出： nMainVideoID	-主流视频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->GetMainVideoID(nMainVideoID);
	}
	return -1;
}

/******************************************************************************
* GetSecondVideoID
*描述：获取辅流视频标识
*输入： cszCallID		-呼叫标识
*输出： nSecondVideoID	-辅流视频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		return pH323MediaCall->GetSecondVideoID(nSecondVideoID);
	}
	return -1;
}

/******************************************************************************
* SetMainVideoScreenIndex
*描述：设置主流视频图像桌面序号
*输入： cszCallID		-呼叫标识
		nScreenIndex	-桌面序号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return 0;
#else//

	int nRet=-1;
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		if (pH323MediaCall->SetMainVideoScreenIndex(nScreenIndex)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
#endif
}

/******************************************************************************
* SetSecondVideoScreenIndex
*描述：设置辅流视频图像桌面序号
*输入： cszCallID		-呼叫标识
		nScreenIndex	-桌面序号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return 0;
#else//

	int nRet=-1;
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		if (pH323MediaCall->SetSecondVideoScreenIndex(nScreenIndex)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
#endif
}


/******************************************************************************
* SetSecondVideoScreenWnd
*描述：设置辅流视频所在屏幕窗口位置
*输入：	cszCallID		-呼叫标识
		nScreenIndex	-屏幕号
		nWndIndex		-窗口号
		nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
return ;
#else//

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetSecondVideoScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
#endif
}

/******************************************************************************
* SetRemoteVideoScreenWnd
*描述：设置远端视频所在屏幕窗口位置
*输入：	cszCallID		-呼叫标识
		nScreenIndex	-屏幕号
		nWndIndex		-窗口号
		nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return ;
#else//

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetRemoteVideoScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
#endif
}

/******************************************************************************
* RequestMainVideoKeyFrame
*描述：请求主流I帧
*输入：cszCallID		-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::RequestMainVideoKeyFrame(const char*cszCallID)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->RequestMainVideoKeyFrame();
	}
}

/******************************************************************************
* RequestSecondVideoKeyFrame
*描述：请求辅流I帧
*输入：cszCallID		-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::RequestSecondVideoKeyFrame(const char*cszCallID)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->RequestSecondVideoKeyFrame();
	}
}

/******************************************************************************
* SetLocalMainVideoSource
*描述：设置本地主视频源
*输入：cszCallID		  -呼叫标识
nMainVideoDevID    -主流设备ID
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetLocalMainVideoSource(const char*cszCallID, int nMainVideoDevID)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","LOCAL_MAIN_VIDEO");
		tStrPacket.Set("STATE","ON");
		tStrPacket.Set("CID",cszCallID);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}
/******************************************************************************
* SetLocalSecondVideoSource
*描述：设置本地辅视频源
*输入：cszCallID		  -呼叫标识
nSecondVideoDevID  -辅流设备ID
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetLocalSecondVideoSource(const char*cszCallID, int nSecondVideoDevID)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","LOCAL_SECOND_VIDEO");
		tStrPacket.Set("STATE","ON");
		tStrPacket.Set("CID",cszCallID);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )

//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#else



/******************************************************************************
* SetLocalRecordCallback
*描述：设置本地录像回调
*输入：	cszCallID		-呼叫标识
		pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetLocalRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetLocalRecordCallback(pRecordCallback);
	}
}

/******************************************************************************
* SetRemoteRecordCallback
*描述：设置远程录像回调
*输入：	cszCallID		-呼叫标识
		pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetRemoteRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetRemoteRecordCallback(pRecordCallback);
	}
}

/******************************************************************************
* SetSecondRecordCallback
*描述：设置辅流录像回调
*输入：	cszCallID		-呼叫标识
		pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void H323CallMgr::SetSecondRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SetSecondRecordCallback(pRecordCallback);
	}
}


/******************************************************************************
* SaveLocalMainVideoToBMPFile
*描述：保存本地主流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
		cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SaveLocalMainVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	int nRet=-1;
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		if (pH323MediaCall->SaveLocalMainVideoToBMPFile(cszPathName)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
}

/******************************************************************************
* SaveRemoteMainVideoToBMPFile
*描述：保存远程主流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
		cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SaveRemoteMainVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	int nRet=-1;
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		if (pH323MediaCall->SaveRemoteMainVideoToBMPFile(cszPathName)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
}

/******************************************************************************
* SaveSecondVideoToBMPFile
*描述：保存辅流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
		cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SaveSecondVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	int nRet=-1;
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		if (pH323MediaCall->SaveSecondVideoToBMPFile(cszPathName)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)



/******************************************************************************
* SendDTMF
*描述：发送DTMF
*输入：	cszCallID		-呼叫标识
		nDTMF			-DTMF
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::SendDTMF(const char*cszCallID,const char nDTMF)
{
	int nRet=-1;
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		if (pH323MediaCall->SendDTMF(nDTMF)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
}

void H323CallMgr::DoCheckPTZAction(void)
{
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.begin();
	while (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;

		if(pH323MediaCall->m_bCallState)
		{
			pH323MediaCall->DoCheckPTZAction();

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)

#else

			//terminal use
			if(pH323MediaCall && pH323MediaCall->m_bCallState)
			{
				if(m_nDoCheckPTZCount >10 || m_nDoCheckPTZCount <0) m_nDoCheckPTZCount=0;
				m_nDoCheckPTZCount++;
				bool bRet=false;
				if(m_nDoCheckPTZCount >= 3) //10 seconds check once , 10000 / 3000
				{
					TimerSendRoundTripDalay(pH323MediaCall,0);
					m_nDoCheckPTZCount =0;
				}

				bRet=TimerCheckRoundTripDalay(pH323MediaCall,0);
				if(bRet) break;
				bRet=TimerCheckNetCardDisable(pH323MediaCall,0);
				if(bRet) break;

				//////终端原接口以及WIN32界面终端使用新接口时回调使用ThreadProcMain转调 调用
				////if (CallMgr::Instance().m_nUseCheckMedia >0 && pH323MediaCall->IsExpired())
				////{
				////	HangupCall(iter->first.c_str());
				////	break;
				////}
			}

#endif

		}

		++iter;
	}
}

/******************************************************************************
* StartMixingWithWaveFile
*描述：开始对呼叫进行WAVE文件混音，对方会听到WAVE文件播放的声音
*输入：	cszCallID		-呼叫标识
		cszPathName		-WAVE文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::StartMixingWithWaveFile(const char*cszCallID,const char*cszPathName)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return 0;
#else//
	int nRet=-1;
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		if (pH323MediaCall->StartMixingWithWaveFile(cszPathName)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
#endif
}

/******************************************************************************
* StopMixingWithWaveFile
*描述：停止对呼叫进行WAVE文件混音
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::StopMixingWithWaveFile(const char*cszCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return 0;
#else//

	int nRet=-1;
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		if (pH323MediaCall->StopMixingWithWaveFile()>=0)
		{
			nRet=0;
		}
	}
	return nRet;
#endif
}

/******************************************************************************
* GetSecondVideoDirection
*描述：获取辅流的方向
*输入：	cszCallID		-呼叫标识
*输出： nDir			-辅流的方向
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir)
{
	int nRet=-1;
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
	XAutoLock l(m_csMapH323MediaCall);
#endif
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		if (pH323MediaCall->GetSecondVideoDirection(nDir)>=0)
		{
			nRet=0;
		}
	}
	return nRet;
}


bool H323CallMgr::ExcludePlayChan(unsigned long nAudioID,AUDEC_PlayChan*pPlayChan)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
return true;
#else//#endif
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.begin();
	while (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		unsigned long nTempID=0;
		pH323MediaCall->GetMainAudioID(nTempID);
		if (nTempID==nAudioID)
		{
			pH323MediaCall->ExcludePlayChan(pPlayChan);
			return true;
		}
		else
		{
			++iter;
		}
	}

#endif
	return false;
}


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


//检查令牌在用状态，传入CALL指针参数 返回true为在使用，false为不在使用
bool H323CallMgr::CheckH239TokenState(H323MediaCall*pH323MediaCall)
{
	return m_bTokenUseState;
}

//置令牌使用状态，传入CALL指针参数，bState true为在使用，false为不在使用
void H323CallMgr::SetH239TokenState(H323MediaCall*pH323MediaCall,bool bState)
{
	m_bTokenUseState=bState;
}

//置所有的会议室令牌使用状态，bState true为在使用，false为不在使用
void H323CallMgr::SetAllH239TokenState(bool bState)
{
	m_bTokenUseState=bState;
}

//添加令牌响应MAP表项，传入CALL指针参数
void H323CallMgr::AddH239MediaCallItem(H323MediaCall*pH323MediaCall)
{
	bool bFind=false;
	//虚拟call不再进入H239令牌响应表
	//if(pH323MediaCall == m_pVitualH323MediaCall) return ;
	////PrintDebugStringInfo(pH323MediaCall->GetUserID().c_str());
	XAutoLock l(m_csMapH239MediaCall);
	MapH323MediaCall::iterator iter=m_MapH239MediaCall.find(pH323MediaCall->GetCallID());
	if (iter!=m_MapH239MediaCall.end())
	{
		bFind=true;
	}
	if(!bFind) m_MapH239MediaCall[pH323MediaCall->GetCallID()]=pH323MediaCall;

	MapH323MediaCall::iterator iter2=m_MapH239MediaCall.begin();
	while (iter2!=m_MapH239MediaCall.end())
	{
		H323MediaCall*pH323Call=iter2->second;
		//if(pH323Call) PrintDebugStringInfo3("MapH239MediaCall",pH323Call->GetUserID().c_str(),pH323Call->GetCallID().c_str());
		++iter2;
	}
}

//删除令牌响应MAP表项，传入CALL指针参数
void H323CallMgr::DelH239MediaCallItem(H323MediaCall*pH323MediaCall)
{
	//虚拟call
	//if(pH323MediaCall == m_pVitualH323MediaCall) return ;
	////PrintDebugStringInfo(pH323MediaCall->GetCallID().c_str());
	XAutoLock l(m_csMapH239MediaCall);
	m_MapH239MediaCall.erase(pH323MediaCall->GetCallID());
}

//清空令牌响应MAP表，传入CALL指针参数
void H323CallMgr::RemoveH239MediaCallAll(H323MediaCall*pH323MediaCall)
{
	//PrintDebugStringInfo2(pH323MediaCall->GetUserID().c_str(),pH323MediaCall->GetUserID().c_str());
	XAutoLock l(m_csMapH239MediaCall);
	while(true)
	{
		MapH323MediaCall::iterator iter=m_MapH239MediaCall.begin();
		if (iter!=m_MapH239MediaCall.end())
		{
			m_MapH239MediaCall.erase(iter);
		}
		else
		{
			break;
		}
	}
}

void H323CallMgr::OnRecvGenericRequest(H323MediaCall*pH323MediaCall, int terLabel, int chID, int symBreaking,int type)
{
	bool bUseState=CheckH239TokenState(pH323MediaCall);
	if(!bUseState)
	{
		SetH239TokenState(pH323MediaCall,true);
		m_pKeepH323MediaCall=pH323MediaCall;
		pH323MediaCall->SetH239TokenOwnState(true);
		if(type==1) pH323MediaCall->SendGenericResponse(true);
		BroadGenericRequest(pH323MediaCall);
		OnCallSecondVideoStart(pH323MediaCall->GetCallID());
		//PrintDebugStringInfo3(pH323MediaCall->GetUserID().c_str(),pH323MediaCall->GetCallID().c_str(),"Token Not use ---------------");
	}
	else
	{
		//if mp agent token use refuse notice test
		//bool bState=CheckMPAgentTokenState(pH323MediaCall);
		//if(bState)
		//{
		//	pH323MediaCall->SendGenericResponse(false);
		//	return;
		//}
		m_pRequestH323MediaCall=pH323MediaCall;
		if(m_pKeepH323MediaCall)
		{
			char msg2[256]={0};
			sprintf(msg2,"H323CallMgr::OnRecvGenericRequest Token use . m_pKeepH323MediaCall=%p pH323MediaCall=%p \n",m_pKeepH323MediaCall,pH323MediaCall);
			//OutputDebugString(msg2);
		}
		//判断避免自身回环
		if(m_pKeepH323MediaCall && m_pKeepH323MediaCall !=pH323MediaCall) m_pKeepH323MediaCall->SendGenericRequest();
	}
}

//广播令牌请求至所有非令牌拥有者，传入CALL指针参数
void H323CallMgr::OnBroadGenericRequest(H323MediaCall*pH323MediaCall)
{
	//XAutoLock l(m_csMapH323MediaCall);
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.begin();
	while (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323Call=iter->second;
		if(pH323MediaCall== pH323Call)
		{
			++iter;
			continue;
		}
		pH323Call->SendGenericRequest();
		++iter;
	}
}

//广播令牌请求至所有非令牌拥有者，传入CALL指针参数
void H323CallMgr::BroadGenericRequest(H323MediaCall*pH323MediaCall)
{
	const std::string & strCallID=pH323MediaCall->GetCallID();
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","H239");
		tStrPacket.Set("STATE","BGR");
		tStrPacket.Set("CID",strCallID);

		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

//收到令牌响应，传入CALL指针参数
void H323CallMgr::OnRecvGenericResponse(H323MediaCall*pH323MediaCall, int response, int terLabel, int chID)
{
	bool bAccept = false;
	if(response == 126)
	{
		bAccept = true;
	}
	bool bOwn=pH323MediaCall->CheckH239TokenOwnState();
	if (bAccept)
	{
		if(bOwn)
		{
			//有请求者
			if(m_pRequestH323MediaCall)
			{
				m_pKeepH323MediaCall=m_pRequestH323MediaCall;
				m_pRequestH323MediaCall=NULL;
				m_pKeepH323MediaCall->SetH239TokenOwnState(true);
				m_pKeepH323MediaCall->SendGenericResponse(true);
				//RemoveH239MediaCallAll(m_pKeepH323MediaCall);
				DelH239MediaCallItem(m_pKeepH323MediaCall);//
				pH323MediaCall->SetH239TokenOwnState(false);
				AddH239MediaCallItem(pH323MediaCall);
				OnCallSecondVideoStop(pH323MediaCall->GetCallID());
				OnCallSecondVideoStart(m_pKeepH323MediaCall->GetCallID());
			}
		}
		else
		{
			PrintDebugStringInfo3(pH323MediaCall->GetUserID().c_str(),m_pKeepH323MediaCall==NULL ? "NULL" : m_pKeepH323MediaCall->GetUserID().c_str(),"AddH239MediaCallItem");
			AddH239MediaCallItem(pH323MediaCall);
			if(m_pKeepH323MediaCall && pH323MediaCall)
			{
				char msg2[256]={0};
				sprintf(msg2,"AddH239MediaCallItem m_pKeepH323MediaCall=%p pH323MediaCall=%p \n",m_pKeepH323MediaCall,pH323MediaCall);
				//OutputDebugString(msg2);
			}
			pH323MediaCall->SendGenericIndication();
		}
	}
	if(bOwn && (!bAccept))
	{
		//拥有并且拒绝切换 如同OnRecvGenericCommand
		m_pRequestH323MediaCall=NULL;
		OnRecvGenericCommand(pH323MediaCall,0,0);
	}
}

//收到令牌取消，传入CALL指针参数
void H323CallMgr::OnRecvGenericCommand(H323MediaCall*pH323MediaCall, int terLabel, int chID)
{
	bool bUse=CheckH239TokenState(pH323MediaCall);
	bool bOwn=pH323MediaCall->CheckH239TokenOwnState();
	//if(pH323MediaCall) PrintDebugStringInfo3(pH323MediaCall->GetUserID().c_str(),pH323MediaCall->GetCallID().c_str(),"OnRecvGenericCommand ---------------");

	if(bUse && bOwn)
	{
		if(m_pRequestH323MediaCall)
		{
			//PrintDebugStringInfo3(pH323MediaCall->GetUserID().c_str(),m_pKeepH323MediaCall==NULL ? "NULL" : m_pKeepH323MediaCall->GetUserID().c_str(),"if(bUse && bOwn)---1param-----pH323MediaCall-RemoveH239MediaCallAll--------");
			//有其他成员请求令牌广播,收到拥有者发来的GenericCommand
			BroadGenericCommand(pH323MediaCall);
			RemoveH239MediaCallAll(pH323MediaCall);
			BroadGenericRequest(m_pRequestH323MediaCall);
		}
		else
		{
			DelH239TokenKeeper(pH323MediaCall);
		}
	}
	else if(bUse && (!bOwn) )
	{
		//GenericCommand发给令牌拥有者
		//PrintDebugStringInfo3(pH323MediaCall->GetUserID().c_str(),m_pKeepH323MediaCall==NULL ? "NULL" : m_pKeepH323MediaCall->GetUserID().c_str(),"else if(bUse && (!bOwn) )--------------");
		//BroadGenericCommand(pH323MediaCall);
		if(m_pKeepH323MediaCall) m_pKeepH323MediaCall->SendGenericCommand();
	}
}

void H323CallMgr::OnRecvGenericIndication(H323MediaCall*pH323MediaCall, int terLabel, int chID)
{
	//if(pH323MediaCall) PrintDebugStringInfo3(pH323MediaCall->GetUserID().c_str(),pH323MediaCall->GetCallID().c_str(),"OnRecvGenericIndication00 ---------------");
	//call polycom rmx4000 need ,the send dualvideo is only send indication
	int nStatus=pH323MediaCall->GetH239TokenRequestStatus();
	if(nStatus <=0)
	{
		pH323MediaCall->SetH239TokenRequestStatus(1);
		//type为1 时发响应,默认发,  0时不发(如收到指示后就不需发响应)
		OnRecvGenericRequest(pH323MediaCall, terLabel, chID,0,0);
		//if(pH323MediaCall) PrintDebugStringInfo3(pH323MediaCall->GetUserID().c_str(),pH323MediaCall->GetCallID().c_str(),"OnRecvGenericIndication11 ---------------");
	}
}

//广播令牌请求至所有非令牌拥有者，传入CALL指针参数
void H323CallMgr::BroadGenericCommand(H323MediaCall*pH323MediaCall)
{
	XAutoLock l(m_csMapH239MediaCall);
	MapH323MediaCall::iterator iter=m_MapH239MediaCall.begin();
	while (iter!=m_MapH239MediaCall.end())
	{
		H323MediaCall*pH323Call=iter->second;
		if(pH323MediaCall== pH323Call)
		{
			++iter;
			continue;
		}
		pH323Call->SendGenericCommand();
		++iter;
	}
}

//收到辅流，传入CALL指针参数
void H323CallMgr::OnRecvdRemoteSecondVideoRTPPacket(H323MediaCall*pH323MediaCall,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	BroadSecondVideoRTPPacket(pH323MediaCall,pRTPPacketData,nRTPPacketLen);
}

//广播辅流至所有非令牌拥有者，传入CALL指针参数
void H323CallMgr::BroadSecondVideoRTPPacket(H323MediaCall*pH323MediaCall,unsigned char*pRTPPacketData,int nRTPPacketLen)
{
	char data[2048]={0};
	//20130322 发送通道编码类型匹配检查
	CHANNEL_INFO tSrcChannelInfo;
	VIDEC_CODEC_TYPE srcCodecType=VIDEC_CODEC_H261;
	CHANNEL_INFO tDstChannelInfo;
	VIDEC_CODEC_TYPE dstCodecType=VIDEC_CODEC_H261;
	if(pH323MediaCall==m_pVitualH323MediaCall)
	{
		m_pVitualH323MediaCall->GetChannelInfo(tSrcChannelInfo);
		srcCodecType=tSrcChannelInfo.VideoCodecType;
	}
	else if(pH323MediaCall->m_pSecondVideoSendChannel!=NULL)
	{
		pH323MediaCall->m_pSecondVideoSendChannel->GetChannelInfo(tSrcChannelInfo);
		srcCodecType=tSrcChannelInfo.VideoCodecType;
	}

	XAutoLock l(m_csMapH239MediaCall);
	MapH323MediaCall::iterator iter=m_MapH239MediaCall.begin();
	while (iter!=m_MapH239MediaCall.end())
	{
		H323MediaCall*pH323Call=iter->second;
		//20130322 发送通道编码类型匹配检查
		if(pH323Call->m_pSecondVideoSendChannel!=NULL)
		{
			pH323Call->m_pSecondVideoSendChannel->GetChannelInfo(tDstChannelInfo);
			dstCodecType=tDstChannelInfo.VideoCodecType;
		}
		if(srcCodecType==dstCodecType )
		{
			if(pH323Call==m_pVitualH323MediaCall) SendInternalH239Media(pH323Call,(char *)pRTPPacketData,nRTPPacketLen);
			else pH323Call->SendSecondVideo((char *)pRTPPacketData,nRTPPacketLen);
			////PrintDebugStringInfo3(pH323Call->GetUserID().c_str(),pH323Call->GetUserID().c_str(),"SendSecondVideo+++++++++++++++++++++");
		}

		++iter;
	}
}

void H323CallMgr::CheckSendGenericRequest(H323MediaCall*pH323MediaCall)
{
	if(pH323MediaCall)
	{
		bool bUseState=CheckH239TokenState(pH323MediaCall);
		if(bUseState)
		{
			pH323MediaCall->SendGenericRequest();
		}
	}
}

void H323CallMgr::CheckSetH239TokenState(H323MediaCall*pH323MediaCall)
{
	{
		bool bEmpty=false;
		bEmpty=m_MapH323MediaCall.empty();
		if(bEmpty) SetH239TokenState(pH323MediaCall,false);
	}
}


void H323CallMgr::CheckDelH239MediaCallItem(H323MediaCall*pH323MediaCall)
{
	if(pH323MediaCall)
	{
		bool bUseState=CheckH239TokenState(pH323MediaCall);
		if(bUseState)
		{
			DelH239MediaCallItem(pH323MediaCall);
		}
		bool bUse=CheckH239TokenState(pH323MediaCall);
		bool bOwn=pH323MediaCall->CheckH239TokenOwnState();
		if(bUse && bOwn) DelH239TokenKeeper(pH323MediaCall);
	}
}

//删除令牌拥有者相关
void H323CallMgr::DelH239TokenKeeper(H323MediaCall*pH323MediaCall)
{
	//拥有者发来的GenericCommand 清空
	//PrintDebugStringInfo3("H239 Token is cancel","pH323MediaCall->GetUserID().c_str()","-----------2param-------RemoveH239MediaCallAll-----------");
	BroadGenericCommand(pH323MediaCall);
	RemoveH239MediaCallAll(pH323MediaCall);
	SetH239TokenState(pH323MediaCall,false);
	OnCallSecondVideoStop(pH323MediaCall->GetCallID());
	pH323MediaCall->SetH239TokenOwnState(false);
	m_pKeepH323MediaCall=NULL;
}

//检查令牌在用状态，传入ConferenceID参数 返回true为在使用，false为不在使用
bool H323CallMgr::CheckH239TokenState(std::string & strConfID)
{
	return m_bTokenUseState;
}

//置令牌使用状态，传入ConferenceID参数，bState true为在使用，false为不在使用
void H323CallMgr::SetH239TokenState(std::string & strConfID,bool bState)
{
	m_bTokenUseState=bState;
}

//广播令牌请求至所有非令牌拥有者，传入ConferenceID参数
void H323CallMgr::BroadGenericCommand(std::string & strConfID)
{
	//PrintDebugStringInfo(strConfID.c_str());
	XAutoLock l(m_csMapH239MediaCall);
	MapH323MediaCall::iterator iter=m_MapH239MediaCall.begin();
	while (iter!=m_MapH239MediaCall.end())
	{
		H323MediaCall*pH323Call=iter->second;
		pH323Call->SendGenericCommand();
		++iter;
	}
}

//广播令牌请求至所有非令牌拥有者，传入ConferenceID参数
void H323CallMgr::OnBroadGenericRequest(std::string & strConfID)
{
	//XAutoLock l(m_csMapH323MediaCall);
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.begin();
	while (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323Call=iter->second;
		pH323Call->SendGenericRequest();
		++iter;
	}
}

//广播令牌请求至所有非令牌拥有者，传入ConferenceID参数
void H323CallMgr::BroadGenericRequest(std::string & strConfID)
{
	const std::string strCallID="";
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","H239");
		tStrPacket.Set("STATE","MPBGR");
		//tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("CONFID",strConfID);

		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

//清空令牌响应MAP表，传入ConferenceID参数
void H323CallMgr::RemoveH239MediaCallAll(std::string & strConfID)
{
	XAutoLock l(m_csMapH239MediaCall);
	while(true)
	{
		MapH323MediaCall::iterator iter=m_MapH239MediaCall.begin();
		if (iter!=m_MapH239MediaCall.end())
		{
			m_MapH239MediaCall.erase(iter);
		}
		else
		{
			break;
		}
	}
}

//收到其它MP发来的令牌使用消息，传入ConferenceID参数,MPID参数
void H323CallMgr::OnRecvMPGenericRequest(std::string strConfID,std::string strMPID)
{
	//PrintDebugStringInfo2(strConfID.c_str(),strMPID.c_str());
	bool bUseState=CheckH239TokenState(strConfID);
	if(!bUseState)
	{
		SetH239TokenState(strConfID,true);
		BroadGenericRequest(strConfID);//允许
	}
}

//收到其它MP发来的令牌不再使用消息，传入ConferenceID参数,MPID参数
void H323CallMgr::OnRecvMPGenericCommand(std::string strConfID,std::string strMPID)
{
	//PrintDebugStringInfo2(strConfID.c_str(),strMPID.c_str());
	bool bUse=CheckH239TokenState(strConfID);
	if(bUse)
	{
		RemoveH239MediaCallAll(strConfID);
		BroadGenericCommand(strConfID);
		SetH239TokenState(strConfID,false);
	}
}

//收到其它MP发来的再广播至至所有非令牌拥有者，传入ConferenceID参数
void H323CallMgr::OnRecvMPSecondVideoRTPPacket(std::string strConfID,unsigned char*pRTPPacketData,int nRTPPacketLen)
{
	////PrintDebugStringInfo(strConfID.c_str());
	XAutoLock l(m_csMapH239MediaCall);
	MapH323MediaCall::iterator iter=m_MapH239MediaCall.begin();
	while (iter!=m_MapH239MediaCall.end())
	{
		H323MediaCall*pH323Call=iter->second;
		pH323Call->SendSecondVideo((char *)pRTPPacketData,nRTPPacketLen);
		++iter;
	}
}



void H323CallMgr::InitH239VirtualCallState(void)
{
	m_bH239ForceGetToken=false;
	m_bH239ForceGetResp=false;
	m_nH239ControlAgent=0;
	m_nH239MsgType=0;
	m_nH239MediaSend=0;
	m_bH239HasToken=false;
	m_bH239UseToken=false;
	m_bResponseAccept=true;
}

void H323CallMgr::GetH239TokenForce(void)
{
	m_bH239ForceGetToken=false;
	m_bH239ForceGetResp=false;
	m_pVitualH323MediaCall->OnLocalSecondVideoStop();
	bool bUseToken=CheckH239TokenState(m_pVitualH323MediaCall);
	bool bHasToken=m_pVitualH323MediaCall->CheckH239TokenOwnState();
	//原H323需要加OnRecvGenericCommand
	//if(bUseToken && !bHasToken) OnRecvH239GenericCommand(m_pVitualH323MediaCall,0,0);
	OnRecvH239GenericRequest(m_pVitualH323MediaCall,0,0,0);//add
	m_bH239ForceGetToken=true;

}

int H323CallMgr::RecvExternalH239Media(H323MediaCall*pH323MediaCall,char*pRTPPacketData,int nRTPPacketLen)
{
	int nRet=0;
	BroadSecondVideoRTPPacket(pH323MediaCall,(unsigned char*)pRTPPacketData,nRTPPacketLen);
	return nRet;
}
int H323CallMgr::SendInternalH239Media(H323MediaCall*pH323MediaCall,char*pRTPPacketData,int nRTPPacketLen)
{
	int nRet=0;

	return nRet;
}
int  H323CallMgr::OnH239TokenAssign(int nDir)
{
	int nRet=0;
	if(m_bH239ForceGetToken && m_bH239ForceGetResp)
	{
		m_bH239ForceGetToken=false;
		m_bH239ForceGetResp=false;
		SetH239ControlAgent(1);
		RecordH239MsgType(1);
		SetH239MediaStatus(1);
		OnRecvH239GenericRequest(m_pVitualH323MediaCall,0,0,0);//add
		//mp agent use token 
		SetMPAgentTokenState(m_pVitualH323MediaCall,true);
	}
	return nRet;
}
int  H323CallMgr::OnH239TokenRelease(int nDir)
{
	int nRet=0;

	return nRet;
}

//收到令牌请求，传入CALL指针参数
void  H323CallMgr::OnRecvH239GenericRequest(H323MediaCall*pH323MediaCall, int terLabel, int chID, int symBreaking)
{
	if(m_nH239ControlAgent==1)
	{
		OnRecvGenericRequest(pH323MediaCall,terLabel,chID,symBreaking);
		//PrintDebugStringInfo3(pH323MediaCall->GetUserID().c_str(),pH323MediaCall->GetUserID().c_str(),"(m_nH239ControlAgent==1)");
	}

}
//收到令牌取消，传入CALL指针参数
void  H323CallMgr::OnRecvH239GenericCommand(H323MediaCall*pH323MediaCall, int terLabel, int chID)
{
	if(m_nH239ControlAgent==1)
	{
		OnRecvGenericCommand(pH323MediaCall,terLabel,chID);
		//PrintDebugStringInfo3(pH323MediaCall->GetUserID().c_str(),pH323MediaCall->GetUserID().c_str(),"(m_nH239ControlAgent==1)");

	}

}
//收到令牌响应，传入CALL指针参数
void  H323CallMgr::OnRecvH239GenericResponse(H323MediaCall*pH323MediaCall, int response, int terLabel, int chID)
{

}
void  H323CallMgr::OnRecvH239GenericIndication(H323MediaCall*pH323MediaCall, int terLabel, int chID)
{

}

void H323CallMgr::SendH239GenericRequest(H323MediaCall*pH323MediaCall)
{
	if(m_nH239ControlAgent==2)
	{
		//callback external process 
		if(m_bResponseAccept)
		{
			OnRecvGenericResponse(pH323MediaCall,126,0,0);
			OnCallSecondVideoStop(pH323MediaCall->GetCallID());
			OnH239TokenRelease(2);
			//PrintDebugStringInfo3(pH323MediaCall->GetUserID().c_str(),pH323MediaCall->GetUserID().c_str(),"(m_nH239ControlAgent==2)");
		}
	}
}

void H323CallMgr::SendH239GenericResponse(H323MediaCall*pH323MediaCall, bool bAccept)
{
	if(m_bH239ForceGetToken)
	{
		OnRecvH239GenericCommand(m_pVitualH323MediaCall,0,0);
		CheckDelH239MediaCallItem(m_pVitualH323MediaCall);
		m_bH239ForceGetResp=true;
		OnH239TokenAssign(2);
	}
	else if(m_nH239ControlAgent==2)
	{
		OnCallSecondVideoStart(pH323MediaCall->GetCallID());
		OnH239TokenAssign(2);
		//mp agent use token 
		SetMPAgentTokenState(pH323MediaCall,true);
		//PrintDebugStringInfo2(pH323MediaCall->GetUserID().c_str(),pH323MediaCall->GetUserID().c_str());
	}

}

void H323CallMgr::SendH239GenericCommand(H323MediaCall*pH323MediaCall)
{
	if(m_nH239ControlAgent==2)
	{
		OnCallSecondVideoStop(pH323MediaCall->GetCallID());
		//PrintDebugStringInfo3(pH323MediaCall->GetUserID().c_str(),pH323MediaCall->GetUserID().c_str(),"(m_nH239ControlAgent==2)");
	}
}

void H323CallMgr::SendH239GenericIndication(H323MediaCall*pH323MediaCall)
{

}

//检查MP角色令牌在用状态，传入CALL指针参数 返回true为在使用，false为不在使用
bool H323CallMgr::CheckMPAgentTokenState(H323MediaCall*pH323MediaCall)
{
	return m_bMPUseToken;
}

//检查普通终端角色令牌在用状态，传入CALL指针参数 返回true为在使用，false为不在使用
bool H323CallMgr::CheckTerminalAgentTokenState(H323MediaCall*pH323MediaCall)
{
	return m_bTermianlUseToken;
}

//置MP角色令牌使用状态，传入CALL指针参数，bState true为在使用，false为不在使用
void H323CallMgr::SetMPAgentTokenState(H323MediaCall*pH323MediaCall,bool bState)
{
	m_bMPUseToken=bState;
}

//置普通角色令牌使用状态，传入CALL指针参数，bState true为在使用，false为不在使用
void H323CallMgr::SetTerminalAgentTokenState(H323MediaCall*pH323MediaCall,bool bState)
{
	m_bTermianlUseToken=bState;
}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)




unsigned short H323CallMgr::GetH245BasePort(void)
{
	unsigned short nH245BasePort=GetRTPPort(CallMgr::Instance().GetRTPBasePort());
	//CallMgr::Instance().SetRTPBasePort(nH245BasePort+2);
	return nH245BasePort;
}

int H323CallMgr::GetMasterSlaveValue(void)
{
	return m_nMasterSlaveValue;
}

int H323CallMgr::GetDAPCapPrintEnable(void)
{
	return m_nCapPrint;
}

void H323CallMgr::OutPutStackLog(int nLogType,const char * cszLog,char * szCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	int nLen1=0;
	if(cszLog) nLen1=strlen(cszLog);
	int nLen2=0;
	if(szCallID) nLen2 =strlen(szCallID);
	char logBuf[512]={0};
	int nLenPos=0;
	if(nLen1 >0) nLenPos += _snprintf(&logBuf[nLenPos],510 -nLenPos,"%s",cszLog);
	if(nLen2 >0) nLenPos += _snprintf(&logBuf[nLenPos],510 -nLenPos,"ID:%s",szCallID);
	nLenPos += _snprintf(&logBuf[nLenPos],510 -nLenPos,"\n");

	OnDAP_OutPutStackLog(nLogType,logBuf,nLenPos);
	//OutputDebugString(logBuf);

#else
#endif

}

void H323CallMgr::OutPutCapContent(const char*cszCallID,int nCapType,const char * cszContent)
{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
OnDAP_OutPutCapContent(cszCallID,nCapType,cszContent);
#else
#endif
}

void  H323CallMgr::OnDAP_CallAudioRecvChannelOpen(const std::string& strCallID,X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","RCHOPEN");
		tStrPacket.Set("MEDIA","AUDIO");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("CODECID",(unsigned int)AudioCodecID);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void  H323CallMgr::OnDAP_CallMainVideoRecvChannelOpen(const std::string& strCallID,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,int nWidth,int nHeight,int nLevel)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","RCHOPEN");
		tStrPacket.Set("MEDIA","VIDEO");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("CODECID",(unsigned int)codecType);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		tStrPacket.Set("BIT",(unsigned int)nBitrate);
		tStrPacket.Set("WID",(unsigned int)nWidth);
		tStrPacket.Set("HEIG",(unsigned int)nHeight);
		tStrPacket.Set("LEV",(unsigned int)nLevel);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void  H323CallMgr::OnDAP_CallSecondVideoRecvChannelOpen(const std::string& strCallID,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,int nWidth,int nHeight,int nLevel)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","RCHOPEN");
		tStrPacket.Set("MEDIA","DUALVIDEO");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("CODECID",(unsigned int)codecType);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		tStrPacket.Set("BIT",(unsigned int)nBitrate);
		tStrPacket.Set("WID",(unsigned int)nWidth);
		tStrPacket.Set("HEIG",(unsigned int)nHeight);
		tStrPacket.Set("LEV",(unsigned int)nLevel);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void  H323CallMgr::OnDAP_CallFECCRecvChannelOpen(const std::string& strCallID,int nPayloadType)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","RCHOPEN");
		tStrPacket.Set("MEDIA","FECC");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void H323CallMgr::SetDataBufferChannelDestroyed(const std::string& strCallID,std::string strCMD,std::string strMedia)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD",strCMD);
		tStrPacket.Set("MEDIA",strMedia);
		tStrPacket.Set("CID",strCallID);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void H323CallMgr::SetDataBufferChannelConnectedVD(const std::string& strCallID ,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth,int nHeight,int nLevel,std::string strCMD,std::string strMedia,HDAPAPPCALL hdapCall)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD",strCMD);
		tStrPacket.Set("MEDIA",strMedia);
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("CODECID",(unsigned int)codecType);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		tStrPacket.Set("IP",ip);
		tStrPacket.Set("PORT",(unsigned int)port);
		tStrPacket.Set("BIT",(unsigned int)nBitrate);
		tStrPacket.Set("WID",(unsigned int)nWidth);
		tStrPacket.Set("HEIG",(unsigned int)nHeight);
		tStrPacket.Set("LEV",(unsigned int)nLevel);
		if(hdapCall!=NULL)
		{
			unsigned int callPointer=(unsigned int)hdapCall;
			tStrPacket.Set("CPR",callPointer);
		}
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void H323CallMgr::OnDAP_CallAudioRecvChannelConnected(const std::string& strCallID,X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType,unsigned long ip, int port)
{
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","RCHCONN");
		tStrPacket.Set("MEDIA","AUDIO");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("CODECID",(unsigned int)AudioCodecID);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		tStrPacket.Set("IP",ip);
		tStrPacket.Set("PORT",(unsigned int)port);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
#else

	CallMgr::Instance().OnDAP_CallAudioRecvChannelConnected(strCallID.c_str(),AudioCodecID,nPayloadType,ip,port);

#endif
//#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack AudioRecvChannelConnected,",szCallIDBack);
}

void H323CallMgr::OnDAP_CallAudioRecvChannelDestroyed(const std::string& strCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	SetDataBufferChannelDestroyed(strCallID,"RCHDISCO","AUDIO");
#else

	CallMgr::Instance().OnDAP_CallAudioRecvChannelDestroyed(strCallID.c_str());

#endif
#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack AudioRecvChannelDestroyed,",szCallIDBack);
}

void H323CallMgr::OnDAP_CallMainVideoRecvChannelConnected(const std::string& strCallID,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth,int nHeight,int nLevel)
{
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	SetDataBufferChannelConnectedVD(strCallID,codecType,nBitrate,nPayloadType,ip, port,nWidth,nHeight,nLevel,"RCHCONN","VIDEO");
#else

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	//A7 add
	VIDEO_FORMAT_TYPE formatType=GetVideoFormatType(nWidth,nHeight);
	CallMgr::Instance().OnDAP_CallMainVideoRecvChannelConnected(strCallID.c_str(),codecType,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
#else//#endif
	CallMgr::Instance().OnDAP_CallMainVideoRecvChannelConnected(strCallID.c_str(),codecType,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
#endif

#endif
//#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack MainVideoRecvChannelConnected,",szCallIDBack);
}

void H323CallMgr::OnDAP_CallMainVideoRecvChannelDestroyed(const std::string& strCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	SetDataBufferChannelDestroyed(strCallID,"RCHDISCO","VIDEO");
#else

	CallMgr::Instance().OnDAP_CallMainVideoRecvChannelDestroyed(strCallID.c_str());

#endif
#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack MainVideoRecvChannelDestroyed,",szCallIDBack);
}

void H323CallMgr::OnDAP_CallSecondVideoRecvChannelConnected(const std::string& strCallID ,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth,int nHeight,int nLevel)
{
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	SetDataBufferChannelConnectedVD(strCallID,codecType,nBitrate,nPayloadType,ip, port,nWidth,nHeight,nLevel,"RCHCONN","DUALVIDEO");
#else

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	//A7 add
	VIDEO_FORMAT_TYPE formatType=GetVideoFormatType(nWidth,nHeight);
	CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelConnected(strCallID.c_str(),codecType,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
#else
	CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelConnected(strCallID.c_str(),codecType,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
#endif

#endif
//#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack SecondVideoRecvChannelConnected,",szCallIDBack);
}

void H323CallMgr::OnDAP_CallSecondVideoRecvChannelDestroyed(const std::string& strCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	SetDataBufferChannelDestroyed(strCallID,"RCHDISCO","DUALVIDEO");
#else

	CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelDestroyed(strCallID.c_str());

#endif
#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack SecondVideoRecvChannelDestroyed,",szCallIDBack);
}

void H323CallMgr::OnDAP_CallFECCRecvChannelConnected(const std::string& strCallID ,int nPayloadType,unsigned long ip,int port)
{
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","RCHCONN");
		tStrPacket.Set("MEDIA","FECC");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		tStrPacket.Set("IP",ip);
		tStrPacket.Set("PORT",(unsigned int)port);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
#else

	CallMgr::Instance().OnDAP_CallFECCRecvChannelConnected(strCallID.c_str(),nPayloadType,ip,port);

#endif
//#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack FECCRecvChannelConnected,",szCallIDBack);
}

void H323CallMgr::OnDAP_CallFECCRecvChannelDestroyed(const std::string& strCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	SetDataBufferChannelDestroyed(strCallID,"RCHDISCO","FECC");
#else

	CallMgr::Instance().OnDAP_CallFECCRecvChannelDestroyed(strCallID.c_str());

#endif
#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack FECCRecvChannelDestroyed,",szCallIDBack);
}

void H323CallMgr::OnDAP_CallAudioSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID,X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType,unsigned long ip, int port)
{
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		unsigned int callPointer=(unsigned int)hdapCall;
		tStrPacket.Set("CMD","SCHCONN");
		tStrPacket.Set("MEDIA","AUDIO");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("CODECID",(unsigned int)AudioCodecID);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		tStrPacket.Set("IP",ip);
		tStrPacket.Set("PORT",(unsigned int)port);
		tStrPacket.Set("CPR",callPointer);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
#else

	CallMgr::Instance().OnDAP_CallAudioSendChannelConnected(hdapCall,strCallID.c_str(),AudioCodecID,nPayloadType,ip,port);

#endif
//#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack AudioSendChannelConnected,",szCallIDBack);
}

void H323CallMgr::OnDAP_CallAudioSendChannelDestroyed(const std::string& strCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	SetDataBufferChannelDestroyed(strCallID,"SCHDISCO","AUDIO");
#else

	CallMgr::Instance().OnDAP_CallAudioSendChannelDestroyed(strCallID.c_str());

#endif
#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack AudioSendChannelDestroyed,",szCallIDBack);

}
void H323CallMgr::OnDAP_CallMainVideoSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,unsigned long ip,int port,int nWidth,int nHeight,int nLevel)
{
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	SetDataBufferChannelConnectedVD(strCallID,codecType,nBitrate,nPayloadType,ip, port,nWidth,nHeight,nLevel,"SCHCONN","VIDEO",hdapCall);
#else

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	//A7 add
	VIDEO_FORMAT_TYPE formatType=GetVideoFormatType(nWidth,nHeight);
	CallMgr::Instance().OnDAP_CallMainVideoSendChannelConnected(hdapCall,strCallID.c_str(),codecType
		,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);

#else
	CallMgr::Instance().OnDAP_CallMainVideoSendChannelConnected(hdapCall,strCallID.c_str(),codecType
		,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
#endif

#endif
//#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());

	OutPutStackLog(0,"H323Stack MainVideoSendChannelConnected,",szCallIDBack);
}

void H323CallMgr::OnDAP_CallMainVideoSendChannelDestroyed(const std::string& strCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	SetDataBufferChannelDestroyed(strCallID,"SCHDISCO","VIDEO");
#else

	CallMgr::Instance().OnDAP_CallMainVideoSendChannelDestroyed(strCallID.c_str());

#endif
#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack MainVideoSendChannelDestroyed,",szCallIDBack);

}

void H323CallMgr::OnDAP_CallSecondVideoSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth,int nHeight,int nLevel)
{
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	SetDataBufferChannelConnectedVD(strCallID,codecType,nBitrate,nPayloadType,ip, port,nWidth,nHeight,nLevel,"SCHCONN","DUALVIDEO",hdapCall);

#else

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	//A7 add
	VIDEO_FORMAT_TYPE formatType=GetVideoFormatType(nWidth,nHeight);
	CallMgr::Instance().OnDAP_CallSecondVideoSendChannelConnected(hdapCall,strCallID.c_str(),codecType
		,nBitrate,nPayloadType,ip,port,formatType/*nWidth,nHeight*/,nLevel);
#else
	CallMgr::Instance().OnDAP_CallSecondVideoSendChannelConnected(hdapCall,strCallID.c_str(),codecType
		,nBitrate,nPayloadType,ip,port,nWidth,nHeight,nLevel);
#endif

#endif
//#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack SecondVideoSendChannelConnected,",szCallIDBack);

}

void H323CallMgr::OnDAP_CallSecondVideoSendChannelDestroyed(const std::string& strCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	SetDataBufferChannelDestroyed(strCallID,"SCHDISCO","DUALVIDEO");

#else

	CallMgr::Instance().OnDAP_CallSecondVideoSendChannelDestroyed(strCallID.c_str());

#endif
#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack SecondVideoSendChannelDestroyed,",szCallIDBack);
}

void H323CallMgr::OnDAP_CallFECCSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID,int nPayloadType,unsigned long ip, int port)
{
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		unsigned int callPointer=(unsigned int)hdapCall;
		tStrPacket.Set("CMD","SCHCONN");
		tStrPacket.Set("MEDIA","FECC");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("PAYLOAD",(unsigned int)nPayloadType);
		tStrPacket.Set("IP",ip);
		tStrPacket.Set("PORT",(unsigned int)port);
		tStrPacket.Set("CPR",callPointer);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

#else
	CallMgr::Instance().OnDAP_CallFECCSendChannelConnected(hdapCall,strCallID.c_str(),nPayloadType,ip,port);

#endif
//#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack FECCVideoSendChannelConnected,",szCallIDBack);

}

void H323CallMgr::OnDAP_CallFECCSendChannelDestroyed(const std::string& strCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)
	SetDataBufferChannelDestroyed(strCallID,"SCHDISCO","FECC");

#else

	CallMgr::Instance().OnDAP_CallFECCSendChannelDestroyed(strCallID.c_str());

#endif
#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack FECCVideoSendChannelDestroyed,",szCallIDBack);
}

void H323CallMgr::OnDAP_RecvRemoteAudioRTPPacket(const char*cszCallID,X_AUDIO_CODEC_ID codecID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_RecvRemoteAudioRTPPacket(cszCallID,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

//update U6 U7 use
#else
	CallMgr::Instance().OnDAP_RecvRemoteAudioRTPPacket(cszCallID,codecID,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

#endif
}

void H323CallMgr::OnDAP_RecvRemoteMainVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_RecvRemoteMainVideoRTPPacket(cszCallID,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
//update U6 U7 use
#else
	CallMgr::Instance().OnDAP_RecvRemoteMainVideoRTPPacket(cszCallID,codecType,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

#endif
}

void H323CallMgr::OnDAP_RecvRemoteSecondVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_RecvRemoteSecondVideoRTPPacket(cszCallID,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
//update U6 U7 use
#else
	CallMgr::Instance().OnDAP_RecvRemoteSecondVideoRTPPacket(cszCallID,codecType,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

#endif
}

void H323CallMgr::OnDAP_RecvMainVideoRequestKeyFrame(const char*cszCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_RecvMainVideoRequestKeyFrame(cszCallID);
#endif
}

void H323CallMgr::OnDAP_RecvSecondVideoRequestKeyFrame(const char*cszCallID)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	CallMgr::Instance().OnDAP_RecvSecondVideoRequestKeyFrame(cszCallID);
#endif
}

void H323CallMgr::OnDAP_RecvCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	CallMgr::Instance().OnDAP_RecvCallH245ControlMessage(cszCallID,nMsgType,cszPacket,nPacketLen);
#endif
}


void H323CallMgr::OnDAP_RecvCallChannelMessage(const char*cszCallID,H245ChanMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	CallMgr::Instance().OnDAP_RecvCallChannelMessage(cszCallID,nMsgType,cszPacket,nPacketLen);
#endif
}

void H323CallMgr::OnDAP_RecvRASMessage(H225RASMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	CallMgr::Instance().OnDAP_RecvRASMessage(nMsgType,cszPacket,nPacketLen);
#endif
}

void H323CallMgr::OnDAP_OutPutStackLog(int nLogType,const char * cszLog,int nLen)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	nLen=strlen(cszLog);
	CallMgr::Instance().OnDAP_OutPutStackLog(nLogType,cszLog,nLen);
#endif
}

void H323CallMgr::OnDAP_OutPutCapContent(const char*cszCallID,int nCapType,const char * cszContent)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	int nLen=strlen(cszContent);
	CallMgr::Instance().OnDAP_OutPutCapContent(cszCallID,nCapType,cszContent,nLen);
#endif
}

void H323CallMgr::OnDAP_CallRecvdPTZControl(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
	CallMgr::Instance().OnCallRecvdPTZ(cszCallID,nPTZAction,nSpeed);
}

int H323CallMgr::SendDAPPTZControl (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) 
		return pH323MediaCall->SendPTZ(nPTZAction,nSpeed);
	}
	return 0;
}


int H323CallMgr::SetDAPProductID(const char*cszProductID)
{
	m_strProductID=cszProductID;
	return 0;
}

int H323CallMgr::SetDAPProductVersionID(const char*cszVersionID)
{
	m_strProductVersionID=cszVersionID;
	return 0;
}

int H323CallMgr::SetDAPT35CountryCode(int nT35Code)
{
	m_nT35Code=nT35Code;
	return 0;
}

int H323CallMgr::SetDAPT35Extension(int nT35Extension)
{
	m_nT35Extension=nT35Extension;
	return 0;
}

int H323CallMgr::SetDAPManufacturerCode(int nManufacturerCode)
{
	m_nManufacturerCode=nManufacturerCode;
	return 0;
}

int H323CallMgr::SetDAPH323EndPointType(int nType)
{
	m_nEndpontType=nType;
	return 0;
}

int H323CallMgr::SetDAPH245RangePort(unsigned short nFromPort,unsigned short nToPort)
{
	m_nH245PortFrom=nFromPort;
	m_nH245PortTo=nToPort;
	return 0;
}

int H323CallMgr::SetDAPCapPrintEnable(int nPrint)
{
	m_nCapPrint=nPrint;
	return 0;
}

int H323CallMgr::SetDAPMasterSlaveValue(int nValue)
{
	m_nMasterSlaveValue=nValue;
	return 0;
}

int H323CallMgr::SetDAPLocalMainVideoParam(const char*cszCallID,int nFrameRate,int nBitrate)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->m_nMainVideoMaxBitrate=nBitrate;
		pH323MediaCall->m_nMainVideoMaxFrame=nFrameRate;
	}
	return 0;
}

int H323CallMgr::SetDAPLocalSecondVideoParam(const char*cszCallID,int nFrameRate,int nBitrate)
{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->m_nSecondVideoMaxBitrate=nBitrate;
		pH323MediaCall->m_nSecondVideoMaxFrame=nFrameRate;
	}
	return 0;
}

int H323CallMgr::SetH323CallSupportH239(const char*cszCallID,int nSupportH239)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
	XAutoLock l(m_csMapH323MediaCall);
#endif

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->m_nSupportH239=nSupportH239;
	}
	return 0;
}

int H323CallMgr::SetTypeDAPRegisterToGK(int nType)
{
	return Register(nType);
}

//int H323CallMgr::SetTypeDAPAutoManualRegGK(int nType)
//{
//	m_nAutoManualReg=nType;
//	return 0;
//}

int H323CallMgr::DAPGatekeeperRequest()
{
	int nRet = -1;
	HRAS hRas = NULL;

	const char*cszH323ID=m_strH323ID.c_str();
	const char*cszTelNumber=m_strTelNumber.c_str();

	cmTransportAddress tranAddr;
	memset( &tranAddr, 0, sizeof(cmTransportAddress) );

	char addr[128]={0};
	sprintf(addr,"255.255.255.255:%d",m_nRASCastPort);
	cmString2TransportAddress(addr,&tranAddr);

	nRet = cmRASStartTransaction(H323CallMgr::Instance().m_hApp, NULL, &hRas, cmRASGatekeeper, &tranAddr, NULL);
	if(nRet < 0 )
	{
		cmRASClose( hRas );
		return -1;
	}
	if(hRas == NULL) return -1;

	//TerminalAlias
	cmAlias termAlias;
	char bmpStr[256] = {0};
	if(strlen(cszH323ID) > 0)
	{
		memset(&termAlias, 0, sizeof(termAlias) );	
		termAlias.type = cmAliasTypeH323ID;	
		//转换成双字节的string
		termAlias.length =utlChr2Bmp(cszH323ID, (RvUint8*)bmpStr );
		termAlias.string=bmpStr;
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamTerminalAlias,0, sizeof(cmAlias), (char*)&termAlias );
		if(nRet < 0 )
		{
			cmRASClose( hRas );
			return -1;
		}
	}
	if(strlen(cszTelNumber) > 0)
	{
		memset(&termAlias, 0, sizeof(termAlias) );	
		termAlias.type = cmAliasTypeE164;	
		termAlias.length =strlen(cszTelNumber);
		termAlias.string=(char *)cszTelNumber;
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamTerminalAlias,1, sizeof(cmAlias), (char*)&termAlias );
		if(nRet < 0 )
		{
			cmRASClose( hRas );
			return -1;
		}
	}

	nRet = cmRASRequest( hRas );
	if(nRet < 0 )
	{
		cmRASClose(hRas);
		return -1;
	}

	return 0;
}

H323RegUser* H323CallMgr::FindH323RegAccount(std::string strID)
{
	//add gk more account support
	H323RegUser* pH323RegUser=NULL;
	bool bDigit=IsDigitString(strID.c_str());
	std::string strGetID="";
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csH323RegUser);
#endif

		MapH323RegUser::iterator iter=m_MapH323RegUser.begin();
		while (iter!=m_MapH323RegUser.end())
		{
			pH323RegUser=iter->second;
			if(bDigit) strGetID=pH323RegUser->GetRegPhoneNum();
			else strGetID=pH323RegUser->GetRegH323ID();
			if(pH323RegUser && ( strGetID == strID) )
			{
				break;
			}
			else
			{
				pH323RegUser=NULL;
				++iter;
			}
		}
	}
	return pH323RegUser;
}


void H323CallMgr::TimerRegisterToGK(const char*cszHost,int type)
{
	//add gk more account support
	//notice prevent add lock to lock
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csH323RegUser);
#endif

		H323RegUser* pH323RegUser=NULL;
		MapH323RegUser::iterator iter=m_MapH323RegUser.begin();
		while (iter!=m_MapH323RegUser.end())
		{
			pH323RegUser=iter->second;
			if(pH323RegUser)
			{
				if(type==1)
				{
					pH323RegUser->SendRegMessage(cmRASUnregistration);
				}
				else
				{
					pH323RegUser->SendRegMessage(cmRASRegistration,type);
					//Sleep(20);
				}
			}
			++iter;
		}
	}
}

int H323CallMgr::InsideRegisterToGK(cmRASTransaction rasTransact,const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword)
{
	int nRet = -1;
	HRAS hRas = NULL;

	cmTransportAddress tranAddr;
	memset( &tranAddr, 0, sizeof(cmTransportAddress) );

	if( m_nUseGK == 1 && m_nFindGKAddr)
	{
		memcpy( &tranAddr, &m_TransportAddr, sizeof(cmTransportAddress) );
	}
	else
	{
		char addr[128]={0};
		sprintf(addr,"%s:%d",cszHost,m_nGateKeeperPort);
		cmString2TransportAddress(addr,&tranAddr);
	}
	nRet = cmRASStartTransaction(H323CallMgr::Instance().m_hApp, NULL, &hRas, rasTransact, &tranAddr, NULL);
	if(nRet < 0 )
	{
		cmRASClose( hRas );
		return -1;
	}
	if(hRas == NULL) return -1;

	//TerminalAlias
	cmAlias termAlias;
	char bmpStr[256] = {0};
	if(strlen(cszH323ID) > 0)
	{
		memset(&termAlias, 0, sizeof(termAlias) );	
		termAlias.type = cmAliasTypeH323ID;	
		//转换成双字节的string
		termAlias.length =utlChr2Bmp(cszH323ID, (RvUint8*)bmpStr );
		termAlias.string=bmpStr;
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamTerminalAlias,0, sizeof(cmAlias), (char*)&termAlias );
		if(nRet < 0 )
		{
			cmRASClose( hRas );
			return -1;
		}
	}
	if(strlen(cszTelNumber) > 0)
	{
		memset(&termAlias, 0, sizeof(termAlias) );	
		termAlias.type = cmAliasTypeE164;	
		termAlias.length =strlen(cszTelNumber);
		termAlias.string=(char *)cszTelNumber;
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamTerminalAlias,1, sizeof(cmAlias), (char*)&termAlias );
		if(nRet < 0 )
		{
			cmRASClose( hRas );
			return -1;
		}
	}

	nRet = cmRASRequest( hRas );
	if(nRet < 0 )
	{
		cmRASClose(hRas);
		return -1;
	}

	return 0;
}

int H323CallMgr::DAPRegisterToGK(const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword,int natFwOption)
{
	int nLineID = -1;
	if (m_nStarted==0) return -1;

	if( (cszH323ID==NULL || strlen(cszH323ID)==0 ) && (cszTelNumber==NULL || strlen(cszTelNumber)==0) )
	{
		return -1;
	}
	SetH323ID(cszH323ID);
	SetTelNumber(cszTelNumber);
	SetRegHost(cszHost);
	nLineID = AddH323RegAccount(cmRASRegistration,cszH323ID,cszTelNumber,cszHost,cszRegPassword,natFwOption);
	OutPutStackLog(0,"H323Stack RegisterToGK.");
	return nLineID;
}

int H323CallMgr::UnDAPRegisterToGK(int nAccountID)
{
	int nRet = 0;
	RemoveH323RegAccount(nAccountID);
	OutPutStackLog(0,"H323Stack UnRegisterToGK.");
	return nRet;
}

void H323CallMgr::OnDAP_CallRingIn(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,const std::string& strE164ID,const std::string& strPacket,std::string & strTelNum)
{
	LogFile(NULL,NULL,"","","OnDAP_CallRingIn 111\n");

//A7 add
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	//Modified by WangLr 2013-6-27。消息回调不独立一个线程处理，直接从底层回调上去，上层会分线程的
	//Modified by gshzh
	H323MediaCall*pH323MediaCall = NULL;
	{
		XAutoLock l(m_csMapTempH323MediaCall);
		MapH323MediaCall::iterator iter=m_MapTempH323MediaCall.find(strCallID);
		if (iter!=m_MapTempH323MediaCall.end())
		{
			pH323MediaCall=(*iter).second;
			m_MapTempH323MediaCall.erase(iter);
		}
	}
	if (pH323MediaCall)
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		m_MapH323MediaCall[strCallID]=pH323MediaCall;
	}
	CallMgr::Instance().OnCallRingIn(strCallID,strUserID,strUserName,strE164ID,"H323",NULL,0);

#else

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","IN");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);
		tStrPacket.Set("UEID",strE164ID);
		tStrPacket.Set("PACKET",strPacket);
		tStrPacket.Set("TELNUM",strTelNum);

		std::string strText="";
		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack CallRingIn,",szCallIDBack);
	LogFile(NULL,NULL,"","","OnDAP_CallRingIn 222\n");

}

void H323CallMgr::OnDAP_CallRingOut(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName)
{
	LogFile(NULL,NULL,"","","OnDAP_CallRingOut 111\n");

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	//Modified by WangLr 2013-6-27。消息回调不独立一个线程处理，直接从底层回调上去，上层会分线程的
	CallMgr::Instance().OnCallRingOut(strCallID,strUserID,strUserName);

#else

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","OUT");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);

		std::string strText="";
		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack CallRingOut,",szCallIDBack);

	LogFile(NULL,NULL,"","","OnDAP_CallRingOut 222\n");

}


void H323CallMgr::OnDAP_CallEstablished(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,const std::string& strPacket)
{
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)

	LogFile(NULL,NULL,strUserID.c_str(),strCallID.c_str(),"OnDAP_CallEstablished 111");

	//Modified by WangLr 2013-6-27。消息回调不独立一个线程处理，直接从底层回调上去，上层会分线程的
	CallMgr::Instance().OnCallEstablished(strCallID,strUserID,strUserName,NULL,0);
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
			if(pH323MediaCall)
			{
				pH323MediaCall->OpenReceiverFromRemote();
			}
		}
	}

#else

////notice the test
//#if defined(HAS_THREAD_TRANSIT) && (HAS_THREAD_TRANSIT == 1)

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","OK");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);
		tStrPacket.Set("PACKET",strPacket);

		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

//#else
//
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//	//notice the test
//CallMgr::Instance().OnCallEstablished(strCallID,strUserID,strUserName,strPacket.c_str(),strPacket.length());
//#endif
//
//#endif

#endif

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack CallEstablished,",szCallIDBack);

	LogFile(NULL,NULL,"","","OnDAP_CallEstablished 222\n");

}

void H323CallMgr::OnDAP_CallDestroyed(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,IMXEC_CALL_END_REASON nReason,const std::string& strReason )
{
	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack CallDestroyed,",szCallIDBack);

	LogFile(NULL,NULL,strUserID.c_str(),strCallID.c_str(),"OnDAP_CallDestroyed 111");

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	//Modified by WangLr 2013-6-27。消息回调不独立一个线程处理，直接从底层回调上去，上层会分线程的
	//CallMgr::Instance().OnCallDestroyed(strCallID,strUserID,strUserName,(IMXEC_CALL_END_REASON)0,0);
	CallMgr::Instance().OnCallDestroyed(strCallID,strUserID,strUserName,strReason.c_str(),strReason.size());
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(strCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
			m_MapH323MediaCall.erase(iter);
		}
		if(pH323MediaCall)
		{
			pH323MediaCall->SendRegDRQMessage();
			pH323MediaCall->Close();
			delete pH323MediaCall;
			pH323MediaCall=NULL;
		}
	}

#else

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","BYE");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);
		tStrPacket.Set("RSN",strReason);

		std::string strText="";
		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

#endif

	LogFile(NULL,NULL,"","","OnDAP_CallDestroyed 222\n");
}

void H323CallMgr::OnDAP_CallH235CapSet(H323MediaCall*pH323MediaCall,const char * cszE164,ConfMediaSet & h235Set)
{
LogFile(NULL,NULL,"","","OnDAP_CallH235CapSet 111\n");

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	int nBandwidth=0;
	CallMgr::Instance().OnDAP_CallH235CapSet(pH323MediaCall->GetCallID(),cszE164,nBandwidth,h235Set);
	pH323MediaCall->m_nH235AuthUse=h235Set.m_nH235AuthUse;
	pH323MediaCall->m_nH235MediaUse=h235Set.m_nH235MediaUse;
	pH323MediaCall->m_nUseAlogrithmId=h235Set.m_nUseAlogrithmId;
	pH323MediaCall->m_nBandWidth =nBandwidth;

#endif

LogFile(NULL,NULL,"","","OnDAP_CallH235CapSet 222\n");
}

void H323CallMgr::OnDAP_RecvNonstandardData(const std::string& strCallID,const std::string& strPacket )
{
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	//CallMgr::Instance().OnDAP_RecvNonstandardData(strCallID.c_str(),strPacket.c_str(),strPacket.length(),"");
//#endif

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		std::string strMime ="";
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","NONS");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("INFO",strPacket);
		tStrPacket.Set("MIME",strMime);
		std::string strText="";
		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

}

void H323CallMgr::OnDAP_H323RegistrationSuccess(const char*cszH323ID,const char*cszTelNumber)
{
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	//Modified by WangLr 2013-6-27。消息回调不独立一个线程处理，直接从底层回调上去，上层会分线程的
	//add Modified by gshzh 2013-9-27
	//if (m_bRegisteredToGK)
	{
		//NETECStartToGK(); //gk mcu use 20150409 notice 
		CallMgr::Instance().OnDAP_H323RegistrationSuccess(cszH323ID,cszTelNumber);
	}
#else

	std::string strRegH323ID=cszH323ID;
	std::string strRegE164=cszTelNumber;
	std::string strRegReason="";
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","DAPREG");
		tStrPacket.Set("DAPREGST","DAPREGOK");
		tStrPacket.Set("DAPREGID",strRegH323ID);
		tStrPacket.Set("DAPREGNUM",strRegE164);
		tStrPacket.Set("DAPREGRN",strRegReason);

		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
#endif

	OutPutStackLog(0,"H323 RegistrationSuccess.");
}

void H323CallMgr::OnDAP_H323RegistrationError(const char*cszH323ID,const char*cszTelNumber,char* cszReason)
{
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	//Modified by WangLr 2013-6-27。消息回调不独立一个线程处理，直接从底层回调上去，上层会分线程的
	//add Modified by gshzh 2013-9-27
	{
		//NETECStopToGK(); //gk mcu use 20150409 notice 
		CallMgr::Instance().OnDAP_H323RegistrationError(cszH323ID,cszTelNumber,cszReason);
	}
#else

	std::string strRegH323ID=cszH323ID;
	std::string strRegE164=cszTelNumber;
	std::string strRegReason=cszReason;
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","DAPREG");
		tStrPacket.Set("DAPREGST","DAPREGERR");
		tStrPacket.Set("DAPREGID",strRegH323ID);
		tStrPacket.Set("DAPREGNUM",strRegE164);
		tStrPacket.Set("DAPREGRN",strRegReason);

		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
#endif

	OutPutStackLog(0,"H323 RegistrationError.");
}


const char* H323CallMgr::DAPMakeCall(const char*cszUserID,int nBandWidth,char * cszPacket,ConfInfo *pConfInfo)
{
	return InsideMakeCall(cszUserID,nBandWidth,cszPacket,pConfInfo);
}

void H323CallMgr::GetCallUserNonstandardInfo(H323MediaCall *pH323MediaCall,ConfInfo *pConfInfo,char * cszPacket)
{
//notice the not support netec send media ,if support set HAS_A7MCU_INTERFACE is 1
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && ( ( defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 0) ) || (! defined(HAS_A7MCU_INTERFACE) ) )
	return ;
#endif

	std::string strNonstandardInfo="";
//A7 add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	if(cszPacket==NULL)
	{
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
		unsigned short nLocalPort=NETEC_Core::GetUDPPort();
		if (nLocalPort && g_nEnableNetEC)
		{
			StrPacket tStrPacket;
			unsigned long nAudioID=0;
			pH323MediaCall->GetMainAudioIDToRemote(nAudioID);
			unsigned long nVideoID=0,nSecondVideoID=0;
			pH323MediaCall->GetMainVideoIDToRemote(nVideoID);
			pH323MediaCall->GetSecondVideoIDToRemote(nSecondVideoID);
			unsigned long ulLocalIP=H323CallMgr::Instance().GetLocalIPAddress(m_strLocalIP);
			std::string strSameIP="";
			bool bGet=GetOneSameDestIP3(CallMgr::Instance().m_AllLocalIPSave,strSameIP,pH323MediaCall->m_strMakeDestIP);
			//GetOneSameDestIP(strSameIP,pH323MediaCall->m_strMakeDestIP);
			if(bGet) ulLocalIP =inet_addr(strSameIP.c_str());
			tStrPacket.Set("AID",nAudioID);
			tStrPacket.Set("VID",nVideoID);
			tStrPacket.Set("SVID",nSecondVideoID);
			tStrPacket.Set("LIP",ulLocalIP);
			tStrPacket.Set("LPORT",nLocalPort);
			tStrPacket.Set("MCUID",m_strMCUID);
			if (pConfInfo)
			{
				tStrPacket.Set("AVCAST",pConfInfo->m_nEnableMulticast);
			}
			tStrPacket.GetString(strNonstandardInfo);

			//pH323MediaCall->SetUserNonstandardInfo(strNonstandardInfo.c_str());
			//pH323MediaCall->SetCallNonStandardData(cmCallStateOffering);

			IPCLIB_LogL2("[H323CallMgr::MakeCall] MCUID = %s, AID = %d, VID = %d\n", m_strMCUID.c_str(), nAudioID, nVideoID);
		}
#else
	unsigned short nLocalPort=NETEC_Core::GetUDPPort();
	IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
	CallMgr::Instance().GetCallType(nCallType);
	if (nLocalPort && nCallType==IMXEC_PERSONAL_CALL)
	{
		StrPacket tStrPacket;
		unsigned long nAudioID=0;
		unsigned long nVideoID=0;
		unsigned long ulLocalIP=H323CallMgr::Instance().GetLocalIPAddress(m_strLocalIP);
		tStrPacket.Set("AID",nAudioID);
		tStrPacket.Set("VID",nVideoID);
		tStrPacket.Set("LIP",ulLocalIP);
		tStrPacket.Set("LPORT",nLocalPort);
		std::string strMCUID="";
		tStrPacket.Set("MCUID",strMCUID);
		tStrPacket.GetString(strNonstandardInfo);
	}
#endif

	}
	else strNonstandardInfo=cszPacket;

#else

	if(cszPacket==NULL)
	{
		unsigned short nLocalPort=NETEC_Core::GetUDPPort();

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

		if (nLocalPort && 0)

#else

		IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
		CallMgr::Instance().GetCallType(nCallType);
		if (nLocalPort && nCallType==IMXEC_PERSONAL_CALL)

#endif

		{
			StrPacket tStrPacket;
			unsigned long nAudioID=0;
			unsigned long nVideoID=0;
			unsigned long nSecondVideoID=0;
			pH323MediaCall->GetLocalMainAudioID(nAudioID);
			pH323MediaCall->GetLocalMainVideoID(nVideoID);
			pH323MediaCall->GetLocalSecondVideoID(nSecondVideoID);
			unsigned long ulLocalIP=H323CallMgr::Instance().GetLocalIPAddress(m_strLocalIP);
			tStrPacket.Set("AID",nAudioID);
			tStrPacket.Set("VID",nVideoID);
			tStrPacket.Set("SVID",nSecondVideoID);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

			if (NETEC_Core::IsMCUServer())
			{
				tStrPacket.Set("MCUIP",NETEC_Node::GetMCUIP());
				tStrPacket.Set("MCUID",NETEC_Node::GetMCUID());
				tStrPacket.Set("MCUPORT",NETEC_Node::GetServerPort());
			}
			else
			{
				tStrPacket.Set("LIP",ulLocalIP);
				tStrPacket.Set("LPORT",nLocalPort);
			}
#else
			tStrPacket.Set("LIP",ulLocalIP);
			tStrPacket.Set("LPORT",nLocalPort);
			tStrPacket.Set("MCUID","TERM");

#endif
			tStrPacket.GetString(strNonstandardInfo);
		}
	}
	else strNonstandardInfo=cszPacket;

#endif

	pH323MediaCall->SetUserNonstandardInfo(strNonstandardInfo.c_str());
	//pH323MediaCall->SetCallNonStandardData(state);
}


void H323CallMgr::SetUserNonstandardInfo(H323MediaCall *pH323MediaCall,cmCallState_e state,ConfInfo *pConfInfo,char * cszPacket)
{
	std::string strNonstandardInfo="";
	GetCallUserNonstandardInfo(pH323MediaCall,pConfInfo,cszPacket);
	//pH323MediaCall->SetUserNonstandardInfo(strNonstandardInfo.c_str());
	pH323MediaCall->SetCallNonStandardData(state);
}

int H323CallMgr::DAPAcceptCall(const char*cszCallID,char * cszPacket,ConfInfo *pConfInfo)
{
	return InsideAcceptCall(cszCallID,cszPacket,pConfInfo);
}

int H323CallMgr::DAPHangupCall(const char*cszCallID)
{
	return HangupCall(cszCallID);
}

int H323CallMgr::DAPStart(void)
{
	int nRet=-1;
	nRet=InsideStart(m_nH245PortFrom,m_nH245PortTo);
	//RegGKSetAutoManual(1);
	return nRet;
}

int H323CallMgr::DAPStop(void)
{
	int nRet=-1;
	Stop();
	return 0;
}


int H323CallMgr::DAPSendCallAudio(HDAPAPPCALL hdapCall,const char*cszCallID,char* RTPPacketData, int PacketDataLen)
{
	((H323Call *)hdapCall)->SendAudio(RTPPacketData,PacketDataLen);
	return 0;
}

int H323CallMgr::DAPSendCallMainVideo(HDAPAPPCALL hdapCall,const char*cszCallID,char* RTPPacketData, int PacketDataLen)
{
	((H323Call *)hdapCall)->SendMainVideo(RTPPacketData,PacketDataLen);
	return 0;
}

int H323CallMgr::DAPSendCallDualVideo(HDAPAPPCALL hdapCall,const char*cszCallID,char* RTPPacketData, int PacketDataLen)
{
	((/*H323MediaCall*/H323Call *)hdapCall)->SendSecondVideo(RTPPacketData,PacketDataLen);
	return 0;
}

int H323CallMgr::DAPSendPTZControl (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->SendPTZ(nPTZAction,nSpeed);
	}
	return 0;
}

int H323CallMgr::DAPSendCallDTMF (const char*cszCallID,const char dtmf)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->SendDTMF(dtmf,0);
}
	return 0;
}

int H323CallMgr::DAPSendCallH245NonstandardData(const char*cszCallID,std::string strPacket)
{
	//MCU级联测试 
	//return 0;

	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->SendCallUserInfomation(strPacket);
}
	return 0;
}


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

int H323CallMgr::OpenDualVideoOutChannel(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->OpenH239VideoOutChannel();
	}
	return 0;
}

int H323CallMgr::OpenFECCOutChannel(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->OpenFECCOutChannel();
	}
	return 0;
}

int H323CallMgr::CloseDualVideoOutChannel(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->CloseDualVideoOutChannel();
}
	return 0;
}

int H323CallMgr::CloseFECCOutChannel(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->CloseFECCOutChannel();
	}
	return 0;
}

int H323CallMgr::AudioRecvChannelAnswer(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;	
		}
	//}
	if(pH323MediaCall) pH323MediaCall->AnswerMediaRecvChannel(1);
	}
	return 0;
}

int H323CallMgr::MainVideoRecvChannelAnswer(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;	
		}
	//}
	if(pH323MediaCall) pH323MediaCall->AnswerMediaRecvChannel(2);
	}
	return 0;
}

int H323CallMgr::DualVideoRecvChannelAnswer(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;	
		}
	//}
	if(pH323MediaCall) pH323MediaCall->AnswerMediaRecvChannel(3);
	}
	return 0;
}

int H323CallMgr::FECCRecvChannelAnswer(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;	
		}
	//}
	if(pH323MediaCall) pH323MediaCall->AnswerMediaRecvChannel(4);
	}
	return 0;
}

int H323CallMgr::AudioRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->RejectMediaRecvChannel(1,(cmRejectLcnReason)nReason);
	}

	return 0;
}

int H323CallMgr::MainVideoRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->RejectMediaRecvChannel(2,(cmRejectLcnReason)nReason);
	}
	return 0;
}

int H323CallMgr::DualVideoRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->RejectMediaRecvChannel(3,(cmRejectLcnReason)nReason);
	}
	return 0;
}

int H323CallMgr::FECCRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->RejectMediaRecvChannel(4,(cmRejectLcnReason)nReason);
	}
	return 0;
}

#endif


int H323CallMgr::DAPGetCallSupportSecondVideo(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) return pH323MediaCall->GetSupportSecondVideo();
	}
	return 0;
}

int H323CallMgr::SetLocalMainVideoH264Level(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->SetLocalMainVideoH264Level(nLevel);
	}
	return 0;
}

int H323CallMgr::SetLocalSecondVideoH264Level(const char*cszCallID,int nLevel)
{

	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->SetLocalSecondVideoH264Level(nLevel);
	}
	return 0;
}

int H323CallMgr::SetLocalMainVideoH264HPLevel(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->SetLocalMainVideoH264HPLevel(nLevel);
	}
	return 0;
}

int H323CallMgr::SetLocalSecondVideoH264HPLevel(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->SetLocalSecondVideoH264HPLevel(nLevel);
	}
	return 0;
}

int H323CallMgr::SetCallRTPMediaMulticast(const char*cszCallID,const char*cszMulticastIP,int ttl)
{
	H323MediaCall*pH323MediaCall=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	//}
	if(pH323MediaCall) pH323MediaCall->SetRTPMediaMulticast(cszMulticastIP,ttl);
	}
	return 0;
}


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

/******************************************************************************
* AddCapAudioAll
*描述：添加音频的所有能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioAll(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioAll();
}
	return 0;
}

/******************************************************************************
* RemoveCapAudioAll
*描述：移除音频的所有能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::RemoveCapAudioAll(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->RemoveCapAudioAll();
}
	return 0;
}



/******************************************************************************
* AddCapG711A_64K 注,各媒体类型能力集项先添加者优先
*描述：添加音频G711A能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioG711A_64K(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioG711A_64K();
}
	return 0;
}


/******************************************************************************
* AddCapG711U_64K
*描述：添加音频G711U能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioG711U_64K(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioG711U_64K();
}
	return 0;
}


/******************************************************************************
* AddCapAudioG722_64K
*描述：添加音频G722-64K能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioG722_64K(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioG722_64K();
}
	return 0;
}

/******************************************************************************
* AddCapAudioG7221_24K
*描述：添加音频G7221-24K能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioG7221_24K(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioG7221_24K();
}
	return 0;
}


/******************************************************************************
* AddCapAudioG7221_32K
*描述：添加音频G7221-32K能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioG7221_32K(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioG7221_32K();
}
	return 0;
}


/******************************************************************************
* AddCapAudioG7221C_24K
*描述：添加音频G7221C-24K能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioG7221C_24K(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioG7221C_24K();
}
	return 0;
}


/******************************************************************************
* AddCapAudioG7221C_32K
*描述：添加音频G7221C-32K能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioG7221C_32K(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioG7221C_32K();
}
	return 0;
}


/******************************************************************************
* AddCapAudioG7221C_48K
*描述：添加音频G7221C-48K能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioG7221C_48K(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioG7221C_48K();
}
	return 0;
}


/******************************************************************************
* AddCapAudioG729A
*描述：添加音频G729A能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioG729A(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioG729A();
}
	return 0;
}

/******************************************************************************
* AddCapAudioG719_32K
*描述：添加音频G719-32K能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioG719_32K(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioG719_32K();
}
	return 0;
}



/******************************************************************************
* AddCapAudioG719_48K
*描述：添加音频G719-48K能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioG719_48K(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioG719_48K();
}
	return 0;
}


/******************************************************************************
* AddCapAudioG719_64K
*描述：添加音频G719_64K能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapAudioG719_64K(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapAudioG719_64K();
	}
	return 0;
}

/******************************************************************************
* AddCapVideoAll
*描述：添加主流视频的所有能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapVideoAll(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapVideoAll();
}
	return 0;
}


/******************************************************************************
* RemoveCapVideoAll
*描述：移除主流视频的所有能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::RemoveCapVideoAll(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->RemoveCapVideoAll();
}
	return 0;
}


/******************************************************************************
* AddCapH263
*描述：添加主流视频H263能力集
*输入：	cszCallID		-呼叫标识
nHasQCIF		-使用QCIF分辨率 不使用设置为0，使用设置为1
nHasCIF			-使用CIF分辨率 不使用设置为0，使用设置为1
nHas4CIF		-使用4CIF分辨率 不使用设置为0，使用设置为1
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapVideoH263(const char*cszCallID,int nHasQCIF,int nHasCIF,int nHas4CIF)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapVideoH263(nHasQCIF,nHasCIF,nHas4CIF);
}
	return 0;
}


/******************************************************************************
* AddCapVideoH263Plus
*描述：添加主流视频H263+能力集
*输入：	cszCallID		-呼叫标识
nHasVGA			-使用VGA分辨率 不使用设置为0，使用设置为1
nHasSVGA		-使用SVGA分辨率 不使用设置为0，使用设置为1
nHasXGA			-使用XGA分辨率 不使用设置为0，使用设置为1
nHasSXGA		-使用SXGA分辨率 不使用设置为0，使用设置为1
nHas720P		-使用720P分辨率 不使用设置为0，使用设置为1
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapVideoH263Plus(const char*cszCallID,int nHasVGA,int nHasSVGA,int nHasXGA,int nHasSXGA,int nHas720P)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapVideoH263Plus(nHasVGA,nHasSVGA,nHasXGA,nHasSXGA,nHas720P);
}
	return 0;
}


/******************************************************************************
* AddCapVideoH263PlusPlus
*描述：添加主流视频H263++能力集
*输入：	cszCallID		-呼叫标识
nHasVGA			-使用VGA分辨率 不使用设置为0，使用设置为1
nHasSVGA		-使用SVGA分辨率 不使用设置为0，使用设置为1
nHasXGA			-使用XGA分辨率 不使用设置为0，使用设置为1
nHasSXGA		-使用SXGA分辨率 不使用设置为0，使用设置为1
nHas720P		-使用720P分辨率 不使用设置为0，使用设置为1
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapVideoH263PlusPlus(const char*cszCallID,int nHasVGA,int nHasSVGA,int nHasXGA,int nHasSXGA,int nHas720P)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapVideoH263PlusPlus(nHasVGA,nHasSVGA,nHasXGA,nHasSXGA,nHas720P);
}
	return 0;
}

/******************************************************************************
* AddCapVideoH264QCIF
*描述：添加主流视频的H264QCIF能力集
*输入：	cszCallID	-呼叫标识
nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapVideoH264QCIF(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapVideoH264QCIF(nLevel);
}
	return 0;
}


/******************************************************************************
* AddCapVideoH264CIF
*描述：添加主流视频的H264CIF能力集
*输入：	cszCallID	-呼叫标识
nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapVideoH264CIF(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapVideoH264CIF(nLevel);
}
	return 0;
}

/******************************************************************************
* AddCapVideoH2644CIF
*描述：添加主流视频的H264 4CIF能力集
*输入：	cszCallID	-呼叫标识
nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapVideoH2644CIF(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapVideoH2644CIF(nLevel);
}
	return 0;
}

/******************************************************************************
* AddCapVideoH264720P
*描述：添加主流视频的H264720P能力集
*输入：	cszCallID	-呼叫标识
nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapVideoH264720P(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapVideoH264720P(nLevel);
}
	return 0;
}

/******************************************************************************
* AddCapVideoH2641080P
*描述：添加主流视频的H2641080P能力集
*输入：	cszCallID	-呼叫标识
nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapVideoH2641080P(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapVideoH2641080P(nLevel);
}
	return 0;
}

/******************************************************************************
* AddCapVideoH264HP720P
*描述：添加主流视频的H264HighProfile能力集,默认最高720P
*输入：	cszCallID	-呼叫标识
nLevel		-设定此能力对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapVideoH264HP720P(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapVideoH264HP720P(nLevel);
}
	return 0;
}

/******************************************************************************
* AddCapVideoH264HP1080P
*描述：添加主流视频的H264HighProfile能力集,默认最高1080P
*输入：	cszCallID	-呼叫标识
nLevel		-设定此能力对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapVideoH264HP1080P(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapVideoH264HP1080P(nLevel);
}
	return 0;
}



/******************************************************************************
* AddCapDualVideoAll
*描述：添加辅流视频的所有能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapDualVideoAll(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapDualVideoAll();
}
	return 0;
}


/******************************************************************************
* RemoveCapDualVideoAll
*描述：移除辅流视频的所有能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::RemoveCapDualVideoAll(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->RemoveCapDualVideoAll();
}
	return 0;
}


/******************************************************************************
* AddCapDualVideoH263
*描述：添加辅流视频H263能力集
*输入：	cszCallID		-呼叫标识
nHasQCIF		-使用QCIF分辨率 不使用设置为0，使用设置为1
nHasCIF			-使用CIF分辨率 不使用设置为0，使用设置为1
nHas4CIF		-使用4CIF分辨率 不使用设置为0，使用设置为1
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapDualVideoH263(const char*cszCallID,int nHasQCIF,int nHasCIF,int nHas4CIF)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapDualVideoH263(nHasQCIF,nHasCIF,nHas4CIF);
}
	return 0;
}

/******************************************************************************
* AddCapDualVideoH263Plus
*描述：添加辅流视频H263+能力集
*输入：	cszCallID		-呼叫标识
nHasVGA			-使用VGA分辨率 不使用设置为0，使用设置为1
nHasSVGA		-使用SVGA分辨率 不使用设置为0，使用设置为1
nHasXGA			-使用XGA分辨率 不使用设置为0，使用设置为1
nHasSXGA		-使用SXGA分辨率 不使用设置为0，使用设置为1
nHas720P		-使用720P分辨率 不使用设置为0，使用设置为1
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapDualVideoH263Plus(const char*cszCallID,int nHasVGA,int nHasSVGA,int nHasXGA,int nHasSXGA,int nHas720P)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapDualVideoH263Plus(nHasVGA,nHasSVGA,nHasXGA,nHasSXGA,nHas720P);
}
	return 0;
}

/******************************************************************************
* AddCapDualVideoH263PlusPlus
*描述：添加辅流视频H263++能力集
*输入：	cszCallID		-呼叫标识
nHasVGA			-使用VGA分辨率 不使用设置为0，使用设置为1
nHasSVGA		-使用SVGA分辨率 不使用设置为0，使用设置为1
nHasXGA			-使用XGA分辨率 不使用设置为0，使用设置为1
nHasSXGA		-使用SXGA分辨率 不使用设置为0，使用设置为1
nHas720P		-使用720P分辨率 不使用设置为0，使用设置为1
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapDualVideoH263PlusPlus(const char*cszCallID,int nHasVGA,int nHasSVGA,int nHasXGA,int nHasSXGA,int nHas720P)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapDualVideoH263PlusPlus(nHasVGA,nHasSVGA,nHasXGA,nHasSXGA,nHas720P);
}
	return 0;
}

/******************************************************************************
* AddCapDualVideoH264QCIF
*描述：添加辅流视频的H264QCIF能力集
*输入：	cszCallID	-呼叫标识
nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapDualVideoH264QCIF(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapDualVideoH264QCIF(nLevel);
}
	return 0;
}


/******************************************************************************
* AddCapDualVideoH264CIF
*描述：添加辅流视频的H264CIF能力集
*输入：	cszCallID	-呼叫标识
nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapDualVideoH264CIF(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapDualVideoH264CIF(nLevel);
}
	return 0;
}


/******************************************************************************
* AddCapDualVideoH2644CIF
*描述：添加辅流视频的H264 4CIF能力集
*输入：	cszCallID	-呼叫标识
nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapDualVideoH2644CIF(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapDualVideoH2644CIF(nLevel);
}
	return 0;
}


/******************************************************************************
* AddCapDualVideoH264720P
*描述：添加辅流视频的H264720P能力集
*输入：	cszCallID	-呼叫标识
nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapDualVideoH264720P(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapDualVideoH264720P(nLevel);
}
	return 0;
}


/******************************************************************************
* AddCapDualVideoH2641080P
*描述：添加辅流视频的H2641080P能力集
*输入：	cszCallID	-呼叫标识
nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapDualVideoH2641080P(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapDualVideoH2641080P(nLevel);
}
	return 0;
}


/******************************************************************************
* AddCapDualVideoH264HP720P
*描述：添加辅流视频的H264HighProfile能力集,默认最高720P
*输入：	cszCallID	-呼叫标识
nLevel		-设定此能力对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapDualVideoH264HP720P(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapDualVideoH264HP720P(nLevel);
}
	return 0;
}

/******************************************************************************
* AddCapDualVideoH264HP1080P
*描述：添加辅流视频的H264HighProfile能力集,默认最高1080P
*输入：	cszCallID	-呼叫标识
nLevel		-设定此能力对应的H264level值,当nLevel大于0时使用此值,可选
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapDualVideoH264HP1080P(const char*cszCallID,int nLevel)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapDualVideoH264HP1080P(nLevel);
}
	return 0;
}

/******************************************************************************
* RemoveCapAll
*描述：移除所有媒体类型的所有能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::RemoveCapAll(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->RemoveCapAll();
}
	return 0;
}

/******************************************************************************
* RemoveCapFECCDataAll
*描述：移除FECC控制所有能力集
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::RemoveCapFECCDataAll(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->RemoveCapFECCDataAll();
}
	return 0;
}

/******************************************************************************
* AddCapFECCData
*描述：添加FECC控制能力集
*输入：	cszCallID	-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::AddCapFECCData(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->AddCapFECCData();
}
	return 0;
}

#endif

int H323CallMgr::DAPSendCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);
}
	return 0;
}

int H323CallMgr::SendRequestMainVideoKeyFrame(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->RequestMainVideoKeyFrame();
}
	return 0;
}

int H323CallMgr::SendRequestSecondVideoKeyFrame(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->RequestSecondVideoKeyFrame();
}
	return 0;
}

int H323CallMgr::SendCallChannelMessage(const char*cszCallID,H245ChanMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	//if(pH323MediaCall) 
	//	return pH323MediaCall->SendCallChannelMessage(cszCallID,nMsgType,cszPacket,nPacketLen);
}
	return 0;
}

	/******************************************************************************
	* SendRASMessage
	*描述：发送RAS消息 nMsgType 重定义
	*输入：	nMsgType			-消息类型
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
int H323CallMgr::SendRASMessage(H225RASMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
	return 0;
}

/******************************************************************************
* GetCallQ931TransportAddr
*描述：获取呼叫Q931通信IP地址信息
*输入：	cszCallID			-呼叫标识
szAddr				-IP地址字符串Buffer
nPort				-通信端口
nLen				-IP地址字符串长度
*输出： 无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetCallQ931TransportAddr(const char*cszCallID,char * szAddr,int &nPort,int &nLen)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->GetCallQ931TransportAddr(szAddr,nPort,nLen);
}
	return 0;
}


/******************************************************************************
* GetCallH245TransportAddr
*描述：获取呼叫H245通信IP地址信息
*输入：	cszCallID			-呼叫标识
szAddr				-IP地址字符串Buffer
nPort				-通信端口
nLen				-IP地址字符串长度
*输出： 无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323CallMgr::GetCallH245TransportAddr(const char*cszCallID,char * szAddr,int &nPort,int &nLen)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}
	
	if(pH323MediaCall) 
		return pH323MediaCall->GetCallH245TransportAddr(szAddr,nPort,nLen);
}
	return 0;
}


void H323CallMgr::SetMCUID(const char*cszMCUID)
{
	m_strMCUID = cszMCUID;
}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

void H323CallMgr::StartVideoToCall(const char*cszCallID,unsigned long nVideoID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}

	if(pH323MediaCall) 
		pH323MediaCall->StartVideoToCall(nVideoID,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort);
}
}

void H323CallMgr::StopVideoToCall(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}

	if(pH323MediaCall) 
		pH323MediaCall->StopVideoToCall();
}
}

void H323CallMgr::StartH239ToCall(const char*cszCallID,unsigned long nVideoID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}

	if(pH323MediaCall) 
		pH323MediaCall->StartH239ToCall(nVideoID,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort);
	}
}

void H323CallMgr::StopH239ToCall(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}

	if(pH323MediaCall) 
		pH323MediaCall->StopH239ToCall();
}
}

void H323CallMgr::StartAudioToCall(const char*cszCallID,unsigned long nAudioID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}

	if(pH323MediaCall)
	{
		pH323MediaCall->CheckGenerateSSRCFlag();
		pH323MediaCall->StartAudioToCall(nAudioID,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort);
	}
}
}

void H323CallMgr::StopAudioToCall(const char*cszCallID)
{
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}

	if(pH323MediaCall) 
		pH323MediaCall->StopAudioToCall();
}
}

#endif


void H323CallMgr::DoCheckRoundTrip(void)
{
	H323MediaCall*pH323MediaCall=NULL;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
	XAutoLock l(m_csMapH323MediaCall);
#endif
	MapH323MediaCall::iterator iter=m_MapH323MediaCall.begin();
	while (iter!=m_MapH323MediaCall.end())
	{
		pH323MediaCall=iter->second;
		if(pH323MediaCall && pH323MediaCall->m_bCallState)
		{
			pH323MediaCall->CheckTimestamp();
			TimerSendRoundTripDalay(pH323MediaCall,0);
			bool bRet=TimerCheckRoundTripDalay(pH323MediaCall);
			if(bRet) break;
		}

		++iter;
	}

}

int H323CallMgr::HangupCallPointer(H323MediaCall*pH323MediaCall,int type)
{
	LogFile(NULL,NULL,"","","H323CallMgr::HangupCallPointer 111");
	//OutputDebugString("H323CallMgr::HangupCallPointer 222\n");

	HCALL hsCall=NULL;
	char * cszCallID="";
	char szCallIDBack[128]={0};
	if(pH323MediaCall)
	{
		pH323MediaCall->m_bCallState =false;
		cszCallID=(char *)pH323MediaCall->GetCallID().c_str();
		strcpy(szCallIDBack,cszCallID);
		hsCall=pH323MediaCall->m_hsCall;
		pH323MediaCall->m_hsCall=NULL;

		if(type ==1 ) pH323MediaCall->m_nLocalHungUP=2;
		if (hsCall)
		{
			H235SecurityCallEnd(pH323MediaCall);
			if(type ==2)
			{
				cmCallDropParam(hsCall,cmReasonTypeUnreachableDestination); //特殊处理
			}
			else
			{
				cmCallDrop(hsCall);
			}
		}
	}
	OutPutStackLog(0,"H323Stack HangupCall OK.",szCallIDBack);

	LogFile(NULL,NULL,"","","H323CallMgr::HangupCallPointer 222");

	return 0;
}

void H323CallMgr::TimerSendRoundTripDalay(H323MediaCall * pH323MediaCall,int type)
{
	//H460 use
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	if(pH323MediaCall->m_nUseFWNAT >0 ) pH323MediaCall->SendCallUserInfomation(m_strH225LogicalKA);

#else

	if( m_nUseTerminalFWNAT > 0) pH323MediaCall->SendCallUserInfomation(m_strH225LogicalKA);

#endif

	if(CallMgr::Instance().m_nUseCheckMedia >0) //10 seconds check once , 10000 / 3000
	{
		pH323MediaCall->SendRoundTripDelay(36);
	}
}

bool H323CallMgr::TimerCheckRoundTripDalay(H323MediaCall * pH323MediaCall,int type)
{
	bool bRet=false;

	if(pH323MediaCall->m_nRoundTripSendCount <0) pH323MediaCall->m_nRoundTripSendCount =0;

	else if(pH323MediaCall->m_nRoundTripSendCount >= pH323MediaCall->m_bRoundTripMaxCount && pH323MediaCall->IsExpired())
	{
		pH323MediaCall->m_nRoundTripSendCount=0;
		H323CallMgr::Instance().HangupCallPointer(pH323MediaCall);
		bRet=true;
	}
	//if(pH323MediaCall->m_nRoundTripSendCount >= pH323MediaCall->m_bRoundTripMaxCount)
	//{
	//	pH323MediaCall->m_nRoundTripSendCount=0;
	//}

	return bRet;
}

//add gk more account support
int H323CallMgr::AddH323RegAccount(cmRASTransaction rasTransact,const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword,int natFwOption)
{
	int nLineID=0;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else

	RemoveH323RegAccount(nLineID);//此语句会限制为只注册一个账号
#endif

	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csH323RegUser);
#endif

		while (true)
		{
			MapH323RegUser::iterator iter=m_MapH323RegUser.find(nLineID);
			if (iter==m_MapH323RegUser.end())
			{
				break;
			}
			else
			{
				if(strcmp(iter->second->GetRegH323ID().c_str(),cszH323ID) == 0 || strcmp(iter->second->GetRegPhoneNum().c_str(),cszTelNumber) == 0)
				{
					char* cszReason="Repeat accounts error.";
					OnDAP_H323RegistrationError(cszH323ID,cszTelNumber,cszReason);
					return -1;
				}
				nLineID++;
			}
		}
	}
	if(nLineID+1> MaxH323RegUserCount-1)
	{
		char* cszReason="Maximum accounts error.";
		OnDAP_H323RegistrationError(cszH323ID,cszTelNumber,cszReason);
		return -1;
	}

	H323RegUser* pH323RegUser=new H323RegUser(*this);
	if(pH323RegUser)
	{
		pH323RegUser->SetRegisterAccount(cszH323ID,cszTelNumber,cszHost,cszRegPassword,nLineID,natFwOption);
		if(cszHost ==NULL || strlen(cszHost) <=0)
			pH323RegUser->SendGRQMessage();
		else
			pH323RegUser->SendRegMessage(rasTransact);
	}

	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csH323RegUser);
#endif

		m_MapH323RegUser[nLineID]=pH323RegUser;
		m_nRegAccountNum++;
	}
	return nLineID;
}

//add gk more account support
void H323CallMgr::RemoveH323RegAccount(int nLineID)
{
	H323RegUser* pH323RegUser=NULL;
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csH323RegUser);
#endif
		MapH323RegUser::iterator iter=m_MapH323RegUser.find(nLineID);
		if (iter!=m_MapH323RegUser.end())
		{
			pH323RegUser=iter->second;
			m_MapH323RegUser.erase(iter);
		}
		if(pH323RegUser)
		{
			m_nRegAccountNum --;
			pH323RegUser->SendRegMessage(cmRASUnregistration);
			//delete pH323RegUser;
			//pH323RegUser=NULL;
		}
	}
}

//A7 add
void H323CallMgr::OnMainVideoFlowControlCommand(const std::string&strCallID, unsigned int nBitRate)
{
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	CallMgr::Instance().OnMainVideoFlowControlCommand(strCallID.c_str(), nBitRate);
#endif

}

//A7 add
void H323CallMgr::OnSecondVideoFlowControlCommand(const std::string&strCallID, unsigned int nBitRate)
{
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	CallMgr::Instance().OnSecondVideoFlowControlCommand(strCallID.c_str(), nBitRate);
#endif
}

void H323CallMgr::AllowToSendSecondVideo(const char*cszCallID, bool bAllow)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
	XAutoLock l(m_csMapH323MediaCall);
#endif

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->AllowToSendSecondVideo(bAllow);
	}
}

// 发流控命令
void H323CallMgr::SendFlowControlCommand(const char*cszCallID, IMXEC_CALL_MEDIA_TYPE emMediaType, int nMaximumBitRate)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
	XAutoLock l(m_csMapH323MediaCall);
#endif

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
	if (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		pH323MediaCall->SendFlowControlCommand(emMediaType, nMaximumBitRate);
	}
}

void H323CallMgr::SetH460NATFwOption(int option)
{
	m_nUseTerminalFWNAT=option;
}

void H323CallMgr::SendChannelKeepAlive(void)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
	XAutoLock l(m_csMapH323MediaCall);
#endif

	MapH323MediaCall::iterator iter=m_MapH323MediaCall.begin();
	while (iter!=m_MapH323MediaCall.end())
	{
		H323MediaCall*pH323MediaCall=iter->second;
		if(pH323MediaCall && pH323MediaCall->m_nUseFWNAT >0 ) pH323MediaCall->SendChannelKeepAlive();
		++iter;
	}
}

void H323CallMgr::HungupAll(void)
{
	LogFile(NULL,NULL,"","","H323CallMgr::HungupAll 111");

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if(m_pVitualH323MediaCall)
	{
		m_pVitualH323MediaCall->OnLocalSecondVideoStop();
	}

#endif

	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		while (1)
		{
			H323MediaCall*pH323MediaCall=NULL;
			HCALL hsCall=NULL;
			MapH323MediaCall::iterator iter=m_MapH323MediaCall.begin();
			if (iter!=m_MapH323MediaCall.end())
			{
				pH323MediaCall=(*iter).second;
				m_MapH323MediaCall.erase(iter);
				hsCall=pH323MediaCall->m_hsCall;
			}
			else
				break;

			if(pH323MediaCall)
			{
				//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
				std::string strDisconnectReason="DisconnectedLocal";

				CallMgr::Instance().OnCallDestroyed(pH323MediaCall->GetCallID(),pH323MediaCall->GetUserID(),pH323MediaCall->GetUserFullName(),strDisconnectReason.c_str(),strDisconnectReason.length());
#else
				CallMgr::Instance().OnCallDestroyed(pH323MediaCall->GetCallID(),pH323MediaCall->GetUserID(),pH323MediaCall->GetUserFullName(),ModeDisconnectedLocal,pH323MediaCall->GetCallNum());
#endif

#if !defined(HAS_H323_AVCON_MP) || (HAS_H323_AVCON_MP == 0)

				if(CallMgr::Instance().m_nUseNETECSend > 0 )
				{
					pH323MediaCall->NETEC_NodeStop();
				}
#endif

				H235SecurityCallEnd(pH323MediaCall);
				//pH323MediaCall->SendRegDRQMessage();
				pH323MediaCall->Close();
				pH323MediaCall->m_hsCall=NULL;
				delete pH323MediaCall;
				pH323MediaCall=NULL;
			}
			if (hsCall)
			{
				cmCallDrop(hsCall);
			}
		}

	}

	while (1)
	{
		H323MediaCall*pH323MediaCall=NULL;
		HCALL hsCall=NULL;
		{
			XAutoLock ll(m_csMapTempH323MediaCall);
			MapH323MediaCall::iterator iter=m_MapTempH323MediaCall.begin();
			if (iter!=m_MapTempH323MediaCall.end())
			{
				pH323MediaCall=(*iter).second;
				m_MapTempH323MediaCall.erase(iter);
				hsCall=pH323MediaCall->m_hsCall;
			}
		}
		if(pH323MediaCall)
		{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
			std::string strDisconnectReason="DisconnectedLocal";

			CallMgr::Instance().OnCallDestroyed(pH323MediaCall->GetCallID(),pH323MediaCall->GetUserID(),pH323MediaCall->GetUserFullName(),strDisconnectReason.c_str(),strDisconnectReason.length());
#else
			CallMgr::Instance().OnCallDestroyed(pH323MediaCall->GetCallID(),pH323MediaCall->GetUserID(),pH323MediaCall->GetUserFullName(),ModeDisconnectedLocal,pH323MediaCall->GetCallNum());
#endif

			//pH323MediaCall->SendRegDRQMessage();
			pH323MediaCall->Close();
			pH323MediaCall->m_hsCall=NULL;
			delete pH323MediaCall;
			pH323MediaCall=NULL;
		}
		else break;

		if (hsCall)
		{
			cmCallDrop(hsCall);
		}
	}


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	//H239令牌广播相关清空
	if(m_pVitualH323MediaCall)
	{
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(m_pVitualH323MediaCall->GetCallID());
		if (iter!=m_MapH323MediaCall.end())
		{
			m_MapH323MediaCall.erase(iter);
		}
		SetH239TokenState(m_pVitualH323MediaCall,false);
		delete m_pVitualH323MediaCall;
		m_pVitualH323MediaCall=NULL;
		m_pKeepH323MediaCall=NULL;
		m_pRequestH323MediaCall=NULL;
	}

	m_MapH239MediaCall.clear();
	SetAllH239TokenState(false);
	m_bMPUseToken=false;
	m_bTermianlUseToken=false;

#endif

	LogFile(NULL,NULL,"","","H323CallMgr::HungupAll 222");

}

void H323CallMgr::OnDAP_CallDestroyed(H323MediaCall*pH323MediaCall)
{
	char szCallIDBack[128]={0};
	strcpy(szCallIDBack,pH323MediaCall->GetCallID().c_str());
	char szUserIDBack[128]={0};
	strcpy(szUserIDBack,pH323MediaCall->m_strUserID.c_str());
	char szUserNameBack[128]={0};
	strcpy(szUserNameBack,pH323MediaCall->m_strUserName.c_str());
	char szReasonBack[128]={0};
	strcpy(szReasonBack,pH323MediaCall->m_strDisconnectReason.c_str());

	OutPutStackLog(0,"H323Stack CallDestroyed,",szCallIDBack);

	LogFile(NULL,NULL,szUserIDBack,szCallIDBack,"OnDAP_CallDestroyed");

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)

	CallMgr::Instance().OnCallDestroyed(szCallIDBack,szUserIDBack,szUserNameBack,szReasonBack,strlen(szReasonBack));

#else

	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","REGBYE");
		tStrPacket.Set("CID",szCallIDBack);
		tStrPacket.Set("UID",szUserIDBack);
		tStrPacket.Set("UNAME",szUserNameBack);
		tStrPacket.Set("RSN",szReasonBack);

		std::string strText="";
		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

#endif

	if(pH323MediaCall)
	{
		delete pH323MediaCall;
		pH323MediaCall=NULL;
	}
}

int H323CallMgr::GetCallH323ID(const char*cszCallID ,char * szH323ID,int nMaxLen)
{
	int nRet=0;
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}

		if(pH323MediaCall)
		{
			nRet=pH323MediaCall->m_strH323ID.size();
			if(nRet < nMaxLen)
			{
				memcpy(szH323ID,pH323MediaCall->m_strH323ID.c_str(),nRet);
			}
			else nRet=0;
		}
	}
	return nRet;
}

int H323CallMgr::GetCallE164ID(const char*cszCallID ,char * szE164ID,int nMaxLen)
{
	int nRet=0;
	H323MediaCall*pH323MediaCall=NULL;
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif
		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=iter->second;
		}

		if(pH323MediaCall)
		{
			nRet=pH323MediaCall->m_strE164ID.size();
			if(nRet < nMaxLen)
			{
				memcpy(szE164ID,pH323MediaCall->m_strE164ID.c_str(),nRet);
			}
			else nRet=0;
		}
	}
	return nRet;
}

int H323CallMgr::SetH323RASPort(unsigned short nLocalPort,unsigned short nRemotePort,unsigned short nCastPort)
{
	m_nRASCastPort = nCastPort;
	m_nRASLocalPort = nLocalPort;
	m_nRegGKPort = nRemotePort;
	m_nGateKeeperPort = nRemotePort;
	if (m_nRASCastPort <=0) m_nRASCastPort =1718;
	if (m_nRASLocalPort <=0) m_nRASLocalPort=1714;
	if (m_nRegGKPort <=0) m_nRegGKPort =1719;
	if (m_nGateKeeperPort <=0) m_nGateKeeperPort=1719;
	return 0;
}

void H323CallMgr::H323SettingFromINI()
{
	int nUseNETECSend=0;
	int nH323ListenPort=0;
	int nSIPListenPort=0;
	int nRASLocalPort=0;
	int nRASDestPort=0;
	int nRASCastPort=0;
	int nUseCheckMedia=0;
	int nUseAjustMediaRate=0;

	CString strPath = _T("");
	GetModuleFileName(NULL,strPath.GetBufferSetLength(512 + 1),512);
	int n = strPath.ReverseFind('\\');
	strPath = strPath.Left(n);
	strPath += _T("\\H323Setting.ini");

	nUseNETECSend=GetPrivateProfileInt("MEDIA","NETECSendT",1,strPath); //NETECSendT ??
	nH323ListenPort=GetPrivateProfileInt("H323PORT","ListenPort",0,strPath);
	nRASLocalPort=GetPrivateProfileInt("H323PORT","RASLocalPort",0,strPath);
	nRASDestPort=GetPrivateProfileInt("H323PORT","RASDestPort",0,strPath);
	nRASCastPort=GetPrivateProfileInt("H323PORT","RASCastPort",0,strPath);
	nSIPListenPort=GetPrivateProfileInt("SIPPORT","ListenPort",0,strPath);
	nUseCheckMedia=GetPrivateProfileInt("MEDIA","CheckMedia",1,strPath);
	nUseAjustMediaRate=GetPrivateProfileInt("MEDIA","AjustMediaRate",1,strPath);

	//A7 terminal gui set
	//if(nUseNETECSend ==0) CallMgr::Instance().m_nUseNETECSend =nUseNETECSend;
	//CallMgr::Instance().m_nUseNETECSend =1; //test ??

	if(nUseCheckMedia ==0) CallMgr::Instance().m_nUseCheckMedia =nUseCheckMedia;
	if(nUseAjustMediaRate ==0) CallMgr::Instance().m_nUseAjustMediaRate =nUseAjustMediaRate;

	int nUseExternalIP=GetPrivateProfileInt("MEDIA","UseExtIP",0,strPath);
	char strExternalIP[128]={0};
	if(nUseExternalIP >0)
	{
		GetPrivateProfileString("MEDIA", "ExternalIP","",strExternalIP,127, strPath );
		if(strlen(strExternalIP) >0) CallMgr::Instance().SetExternalIP(strExternalIP);
	}

	if(nH323ListenPort >0) SetH323ListenPort(nH323ListenPort);
	if(nRASLocalPort >0 || nRASDestPort >0 || nRASCastPort >0)
	{
		SetH323RASPort(nRASLocalPort,nRASDestPort,nRASCastPort);
	}

	m_nUseMediaNetRTP =GetPrivateProfileInt("MEDIA","UseMediaNetRTP",0,strPath);    //1 use medianet rtp  ,0 not use
	m_nCallerRate =GetPrivateProfileInt("MCU","CallerRate",0,strPath);
	int nEPType =GetPrivateProfileInt("MCU","TerminalType",0,strPath);              //1 terminal 2 mcu
	if(nEPType ==1)
	{
		SetDAPH323EndPointType(0);
		SetMCUInfo();
	}
	m_nTerminalCapsType =GetPrivateProfileInt("MCU","TerminalCaps",0,strPath);     //1 use terminal caps ,2 use mcu confcaps
	m_nSendFlowCC =GetPrivateProfileInt("MCU","SendFlowCCommand",1,strPath);       //1 send flowControlCommand  ,0 not send 
 
	char dstBuf[64]={0};
	char srcBuf[64]={0};
	char strAddr[64]={0};
	int ipDestToken[4]={0};
	char strRtMask[64]={0};
	char strCmdBuf[288]={0};
	int i=0,j=0,k=0;
	int nUseRoute =GetPrivateProfileInt("ROUTE","UseRoute",0,strPath);    //设置为1会使用配置的路由表，默认为0不使用
	RouteItemSave & route =CallMgr::Instance().m_AddrRoute;
	route.m_nNum=0;
	if(nUseRoute >0)
	{
		for(i=1;i<=MaxRouteCount;i++)
		{
			sprintf(dstBuf,"DesteRoute%d",i);	
			sprintf(srcBuf,"LocalRoute%d",i);
			GetPrivateProfileString("ROUTE", dstBuf,"",strAddr,63, strPath );
			route.m_AllIP[route.m_nNum].strDest =strAddr;
			GetPrivateProfileString("ROUTE", srcBuf,"",strAddr,63, strPath );
			route.m_AllIP[route.m_nNum].strIP =strAddr;

			sprintf(srcBuf,"LocalGaWay%d",i);
			GetPrivateProfileString("ROUTE", srcBuf,"",strAddr,63, strPath );
			route.m_AllIP[route.m_nNum].strGW =strAddr;

			sprintf(srcBuf,"RtPriority%d",i);
			route.m_AllIP[route.m_nNum].nPriority=GetPrivateProfileInt("ROUTE",srcBuf,0,strPath);

			if(route.m_nNum <MaxRouteCount && 
				route.m_AllIP[route.m_nNum].strDest.size()>0 && 
				route.m_AllIP[route.m_nNum].strIP.size() >0 && 
				route.m_AllIP[route.m_nNum].strGW.size() >0 )
			{
				route.m_AllIP[route.m_nNum].nType =1;
				if(GetIPTokenNum(route.m_AllIP[route.m_nNum].strDest,ipDestToken) ) //dest type
				{
					for(j=0;j<4;j++) if(ipDestToken[j]==0) break;
				}
				route.m_AllIP[route.m_nNum].nType =j;

				if(j==1) strcpy(strRtMask,"255.0.0.0");
				else if(j==2) strcpy(strRtMask,"255.255.0.0");
				else if(j==3) strcpy(strRtMask,"255.255.255.0");
				else if(j==4) strcpy(strRtMask,"255.255.255.255");

				sprintf(strCmdBuf,"cmd /c route delete %s",route.m_AllIP[route.m_nNum].strDest.c_str());
				WinExec(strCmdBuf, SW_HIDE);

				sprintf(strCmdBuf,"cmd /c route add %s mask %s %s",route.m_AllIP[route.m_nNum].strDest.c_str(),
					strRtMask,route.m_AllIP[route.m_nNum].strGW.c_str());
				if(route.m_AllIP[route.m_nNum].nPriority >0)
				{
					sprintf(strCmdBuf,"cmd /c route add %s mask %s %s metric %d",route.m_AllIP[route.m_nNum].strDest.c_str(),
						strRtMask,route.m_AllIP[route.m_nNum].strGW.c_str(),route.m_AllIP[route.m_nNum].nPriority);
				}
				WinExec(strCmdBuf, SW_HIDE);

				route.m_nNum ++;
			}
		}


	}

}

void H323CallMgr::DAPCheckRasModule(const char*cszHost)
{
	std::string strHost="";
	if(cszHost!=NULL) strHost =cszHost;
	if(m_strLastGKHost != strHost)
	{
		int nRet=0;
		TimerRegisterToGK(NULL,1);
		nRet=cmRasModuleEnd(m_hApp);
		nRet=cmRasModuleInit(m_hApp);
		m_strLastGKHost =strHost;
	}
}

RvBool H323CallMgr::CheckRASURQSend()
{
	if(m_nStarted ==0 )
	{
		if(m_nSendURQ >= 1) return RV_TRUE;
		if(m_nSendURQ < 1)
		{
			m_nSendURQ ++;
		}
	}
	return RV_FALSE;
}

void H323CallMgr::SaveMediaReleasePort(unsigned short rtpPort)
{
	CallMgr::Instance().SaveMediaReleasePort(m_MediaPortRelease,rtpPort);
}

void H323CallMgr::OnCallH245CapChanged (const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,unsigned int nMediaType)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","CAPCHGE");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("UID",strUserID);
		tStrPacket.Set("UNAME",strUserName);
		tStrPacket.Set("MTYPE",nMediaType);

		std::string strText="";

		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

	char szCallIDBack[128]={0}; strcpy(szCallIDBack,strCallID.c_str());
	OutPutStackLog(0,"H323Stack OnCallH245CapChanged,",szCallIDBack);
}


void H323CallMgr::SetMediaPortArrayEmpty()
{
	memset(&m_MediaPortRelease,0,sizeof(m_MediaPortRelease) );
}

bool H323CallMgr::TimerCheckNetCardDisable(H323MediaCall * pH323MediaCall,int type)
{
	bool bRet=false;
	if(type ==0) //终端用
	{
		DWORD   nFlags =0;//网络连接方式
		int     bOnline=0;//网卡或网线是否连接正常
		bOnline=IsNetworkAlive(&nFlags);
		if(!bOnline)
		{
			m_nCheckNetCardDisable ++;
		}
		else m_nCheckNetCardDisable=0;

		//拔掉网线连续超过9秒挂断
		if(m_nCheckNetCardDisable >=3)
		{
			bRet=true;
			m_nCheckNetCardDisable=0;
			H323CallMgr::Instance().HangupCallPointer(pH323MediaCall,2);//cmReasonTypeUnreachableDestination
		}
	}
	return bRet;
}


bool H323CallMgr::IsDNS(const char * strDNS)
{
	bool bDNS=false;
	int i=0;
	bool bIP=false;
	bool bNumber=false;
	bIP=IsCallIPString(strDNS);
	bNumber=IsNumber(strDNS);
	if(bIP ==false && bNumber ==false)
	{
		while(strDNS[i] !='\0')
		{
			if(strDNS[i] =='.') 
			{
				bDNS=true;
				break;
			}
			i++;
		}
	}
	return bDNS;
}


//Allianz.singapore.boardroom.14.07@vc.allianz.com
bool H323CallMgr::GetDNSIPPart1(const char * strDNS,char * DNSIP,int & nLen)
{
	bool bRet=false;
	int  i=0;
	int  nDNSLen=strlen(strDNS);
	bool bAT=false;
	if(nDNSLen <256)
	{
		char temp[256]={0};
		strcpy(temp,strDNS);
		//find dns ip
		bool bGet=FindDNSIP(temp,DNSIP);
		if(bGet) return bGet;

		//dns@dns
		while(strDNS[i] !='\0')
		{
			if(strDNS[i] =='@')
			{
				bAT=true;
				break;
			}
			i++;
		}
		if(bAT)
		{
			strncpy(temp,strDNS,i);
			temp[i]='\0';
		}

		//ip@ip
		bool bIP=false;
		bIP=IsCallIPString(temp);
		if(bIP && strlen(temp) >0 && nLen>0)
		{
			nLen=strlen(temp);
			strcpy(DNSIP,temp);
			if(nLen >0)
			{
				bRet=true;
				SaveToDNSIP(temp,DNSIP);
				return bRet;
			}
		}

		i=0;
		while(temp[i] !='\0')
		{
			if(temp[i] >='0' && temp[i] <='9')
			{
				temp[i-1] ='\0';
				break;
			}
			i++;
		}

		struct hostent *phe = gethostbyname(temp);
		if (phe == 0) return bRet;
		for( i = 0; phe->h_addr_list[i] != 0; i++)
		{
			bRet=false;
			struct in_addr addr;
			memcpy(&addr, phe->h_addr_list[i],sizeof(struct in_addr) );
			char * p=inet_ntoa(addr);
			if(p)
			{
				if(strlen(p) >0 && DNSIP && nLen>0)
				{
					nLen=strlen(p);
					strcpy(DNSIP,p);
					if(nLen >0) bRet=true;
					break;
				}
			}
		}
		if(bRet) SaveToDNSIP(temp,DNSIP);
	}

	return bRet;
}

//Allianz.singapore.boardroom.14.07@vc.allianz.com
bool H323CallMgr::GetDNSIP(const char * strDNS,char * DNSIP,int & nLen)
{
	bool bRet=false;
	int  i=0;
	int  nDNSLen=strlen(strDNS);
	bool bAT=false;
	if(nDNSLen <256)
	{
		char temp[256]={0};
		strcpy(temp,strDNS);

		//find
		bool bGet=FindDNSIP(temp,DNSIP);
		if(bGet) return bGet;

		//dns@dns
		while(strDNS[i] !='\0')
		{
			if(strDNS[i] =='@')
			{
				bAT=true;
				break;
			}
			i++;
		}
		if(bAT)
		{
			memset(temp,0,sizeof(temp));
			strcpy(temp,strDNS +i +1);
		}

		//ip@ip
		bool bIP=false;
		bIP=IsCallIPString(temp);
		if(bIP && strlen(temp) >0 && nLen>0)
		{
			nLen=strlen(temp);
			strcpy(DNSIP,temp);
			if(nLen >0)
			{
				bRet=true;
				SaveToDNSIP(temp,DNSIP);
				return bRet;
			}
		}

		//get dns ip
		struct hostent *phe = gethostbyname(temp);
		if (phe == 0) return bRet;
		for( i = 0; phe->h_addr_list[i] != 0; i++)
		{
			bRet=false;
			struct in_addr addr;
			memcpy(&addr, phe->h_addr_list[i],sizeof(struct in_addr) );
			char * p=inet_ntoa(addr);
			if(p)
			{
				if(strlen(p) >0 && DNSIP && nLen>0)
				{
					nLen=strlen(p);
					strcpy(DNSIP,p);
					if(nLen >0) bRet=true;
					break;
				}
			}
		}

		if(bRet) SaveToDNSIP(temp,DNSIP);
	}

	return bRet;
}

void H323CallMgr::SaveToDNSIP(char * strDNS,char * strIP)
{
	bool bHas=false;
	for(int i=0;i<m_DNSIP.m_nNum;i++)
	{
		if(strcmp(m_DNSIP.m_Array[i].strDNS,strDNS) ==0)
		{
			strcpy(m_DNSIP.m_Array[i].strIP,strIP);
			bHas=true;
		}
	}
	if(bHas ==false && m_DNSIP.m_nNum <MAX_DNSIP_NUM)
	{
		strcpy(m_DNSIP.m_Array[m_DNSIP.m_nNum].strDNS,strDNS);
		strcpy(m_DNSIP.m_Array[m_DNSIP.m_nNum].strIP,strIP);
		m_DNSIP.m_nNum ++;
	}
}

bool H323CallMgr::FindDNSIP(char * strDNS,char * strIP)
{
	bool bHas=false;
	for(int i=0;i<m_DNSIP.m_nNum;i++)
	{
		if(strcmp(m_DNSIP.m_Array[i].strDNS,strDNS) ==0)
		{
			strcpy(strIP,m_DNSIP.m_Array[i].strIP);
			bHas=true;
			break;
		}
	}
	return bHas;
}

//回调挂断状态时再检测下是不是因为网络收不到媒体流原因挂断的
bool H323CallMgr::DisconnectCheckRoundTripDalay(H323MediaCall * pH323MediaCall)
{
	if(pH323MediaCall->m_nLocalHungUP == 1 || pH323MediaCall->m_nLocalHungUP ==3) return false;
	if(pH323MediaCall->m_nRoundTripSendCount > m_bRoundTripDiscMaxCount && pH323MediaCall->IsExpired(5000) ) //5s
	{
		return true;
	}
	return false;
}

void H323CallMgr::OnCallRTPChanged(const std::string& strCallID,H323Channel * pChannel,unsigned int nType)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		unsigned long ipt=(unsigned long)pChannel;
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","RTPCHGE");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("CHANNEL",ipt);
		tStrPacket.Set("TYPE",nType);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void H323CallMgr::OnCallKeyFrameRequest(const std::string& strCallID,unsigned int nType)  //nType 1 main 2 dualvideo
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","VFUCHGE");
		tStrPacket.Set("CID",strCallID);
		tStrPacket.Set("TYPE",nType);
		std::string strText="";
		tStrPacket.GetString(strText);
		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void H323CallMgr::AddQ931CallPartyNumber(HCALL hsCall,const char * cszUserID,cmCallParam callParam,char * cszPartyNumber)
{
	if (strlen(cszPartyNumber)>0)
	{
		cmTransportAddress ta;
		memset(&ta, 0, sizeof(cmTransportAddress));
		ta.type = cmTransportTypeIP;
		ta.ip = inet_addr(cszUserID);
		ta.port = m_nH323ListenPort;
		ta.distribution = cmDistributionUnicast;
		cmAlias al;
		memset(&al,0,sizeof(al));
		al.type = cmAliasTypeE164;
		al.length = strlen(cszPartyNumber);
		al.string = cszPartyNumber;
		al.pnType = cmPartyNumberPublicUnknown;
		al.transport = ta;
		cmCallSetParam(hsCall,callParam,0,sizeof(al),(char*)&al);
	}
}

int H323CallMgr::GetH323IDAlias(char * szH323ID,cmAlias & termAlias,char * bmpStr)
{
	int nRet=-1;
	if(strlen(szH323ID) > 0)
	{
		memset(&termAlias, 0, sizeof(termAlias) );	
		termAlias.type = cmAliasTypeH323ID;	
		termAlias.length =utlChr2Bmp(szH323ID, (RvUint8*)bmpStr );
		termAlias.string=bmpStr;
		nRet=0;
	}
	return nRet;
}

int H323CallMgr::GetE164IDAlias(char * szTelNumber,cmAlias & termAlias,char * bmpStr)
{
	int nRet=-1;
	if(strlen(szTelNumber) > 0)
	{
		memset(&termAlias, 0, sizeof(termAlias) );	
		termAlias.type = cmAliasTypeE164;	
		termAlias.length =strlen(szTelNumber);
		termAlias.string=(char *)szTelNumber;
		nRet = 0;
	}
	return nRet;
}

bool H323CallMgr::CheckMapCallEmpty(void)
{
	bool bRet1=false;
	bool bRet2=false;
	{
	XAutoLock l(m_csMapTempH323MediaCall);
	if(m_MapTempH323MediaCall.empty()) bRet1=true;
	}

	if(m_MapH323MediaCall.empty()) bRet2=true;

	return (bRet1 && bRet2 ) ? true:false;
}

int H323CallMgr::DapChanegCallCaps(const char*cszCallID,ConfInfo *pConfInfo)
{
	LogFile(NULL,NULL,"","","H323CallMgr::DapChanegCallCaps 111");
	char szCallIDBack[128]={0}; strcpy(szCallIDBack,cszCallID);

	H323MediaCall*pH323MediaCall=NULL;
	if (pConfInfo)
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
		XAutoLock l(m_csMapH323MediaCall);
#endif

		MapH323MediaCall::iterator iter=m_MapH323MediaCall.find(cszCallID);
		if (iter!=m_MapH323MediaCall.end())
		{
			pH323MediaCall=(*iter).second;
		}
		if(pH323MediaCall)
		{
			pH323MediaCall->DapChanegCallCaps(pConfInfo);
		}
	}
	LogFile(NULL,NULL,"","","H323CallMgr::DapChanegCallCaps 222");
	return 0;
}

void H323CallMgr::SetMCUInfo()
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)

	m_strProName ="AVCON MCU";
	if(m_nEndpontType ==0) m_strProName ="AVCON";
	//m_strProName="H3C ME8000";
	//m_strProductVersionID="8041 V800R005B04D051SP01";
	//m_nT35Code=38;
	//m_nT35Extension=0;
	//m_nManufacturerCode=12;
	m_strH323ID =m_strProName;
	m_strUserName =m_strProName;
	m_strProductID =m_strProName;
#endif

#endif
}

//gk mcu use 20150409
void H323CallMgr::NETECStopToGK()
{
	if (m_nNETECNodeCount >0 && NETEC_Node::IsStarted() !=0 )
	{
		m_bNETECNodeToGK =false;
		NETEC_Node::Stop();
		m_nNETECNodeCount --;
	}
}

void H323CallMgr::NETECStartToGK(void)
{
	int nTimes = 0;
	if(NETEC_Node::IsStarted() ==0)
	{
		NETEC_Node::SetServerIP(m_strRegHost.c_str());
		NETEC_Node::SetServerPort(GKNETECPORT);
		if(NETEC_Node::Start() ==0)
		{
			m_nNETECNodeCount ++;
		}
		while (NETEC_Node::GetConnectStatus() != NETEC_Session::CS_CONNECTED)
		{
			Sleep(100);
			if (nTimes++ > 30) break;
		}

		//if(NETEC_Node::GetConnectStatus() == NETEC_Session::CS_CONNECTED)
		{
			m_bNETECNodeToGK =true;
			OutputDebugString("NETECStartToGK Connected.\n"); 
		}
	}
}

int H323CallMgr::SetBandWidth1(ConfInfo * pConfInfo)
{
	int nBandWidth =0;
	if (pConfInfo != NULL)
	{
		nBandWidth=pConfInfo->m_nConfBitrate;
	}
	if (nBandWidth<128)
	{
		nBandWidth=128;
	}
	else if (nBandWidth>8192)
	{
		nBandWidth=8192;
	}
	return nBandWidth;
}

int H323CallMgr::SetBandWidth2(ConfInfo * pConfInfo)
{
	int nBandWidth =0;
	if (pConfInfo != NULL)
	{
		nBandWidth=pConfInfo->m_nConfBitrate;
	}
	else
	{
		//nBandWidth= GetAudioMaxBitrate() + GetMainVideoMaxBitrate();
		//if (m_nSupportH239!=0)
		//	nBandWidth += GetSecondVideoMaxBitrate();//Kbps
		nBandWidth= GetMainVideoMaxBitrate();
	}
	if (nBandWidth<128)
	{
		nBandWidth=128;
	}
	else if (nBandWidth>8192)
	{
		nBandWidth=8192;
	}
	return nBandWidth;
}
