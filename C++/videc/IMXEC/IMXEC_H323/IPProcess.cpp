#include "stdafx.h"
#include <winsock2.h>
#include "IPProcess.h"
#include <IPHlpApi.h>
#include <stdio.h>
#include <time.h>
#include <Ipexport.h>
#include <WS2tcpip.h>

#ifdef HAS_H323
#include <RVINC/cm.h>
#endif

#include <platform/ipc.h>
#include "CallMgr.h"

#ifndef HAS_H323
typedef  void *  HPVT ;
typedef  void *  HPST ;
typedef  void *  HCALL ;
#endif

extern void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info);

//#pragma comment(lib,"Dnsapi")
//#pragma comment(lib,"IPHlpApi")

//sprintf(strDstIP,"%d.%d.%d.%d",((int)(nSrcIP >>24)&0xFF) ,((int)(nSrcIP >>16)&0xFF) ,((int)(nSrcIP >> 8)&0xFF) ,((int)(nSrcIP >> 0)&0xFF) );
unsigned long getIpV4Value(std::string ipV4)
{
	unsigned long nIP=0;
	if(ipV4.size() >0)
	{
		nIP = ntohl(inet_addr(ipV4.c_str()));
	}
	return nIP;
}

//查看是否为合法IP,排除掉全数字或全为首端带有0x及0X的十六进制的数字
bool ipV4Validate(std::string strIP)
{
	const char * str =strIP.c_str();
	bool bRet=false;
	bool bLicit=false;
	int len=0;
	if(str!=NULL)
	{
		len=strlen(str);
		//查看是否为合法IP
		if(inet_addr(str) != INADDR_NONE)
			bLicit=true;
	}
	//查看是否有.或: 排除掉全数字或全为首端带有0x及0X的十六进制的数字
	if(bLicit)
	{
		int i=0;
		while(i<len)
		{
			if( str[i] == '.' ||  str[i] == ':')
			{
				bRet=true;
				break;
			}
			i++;
		}
	}
	return bRet;
}

bool checkSameSegment(std::string& ip1,std::string& ip2,int nCardIndex)
{
	if(!ipV4Validate(ip1))
	{
		return false;
	}
	if(!ipV4Validate(ip2))
	{
		return false;
	}
	unsigned long mask =0;
	NetCardInfo tNetCardInfo;
	bool bGet=GetCurNetCardsInfo(tNetCardInfo,nCardIndex);
	if(bGet) return false;
	mask = getIpV4Value(tNetCardInfo.m_strSubnetMask);
	unsigned long ipValue1 = getIpV4Value(ip1);
	unsigned long ipValue2 = getIpV4Value(ip2);
	return (mask & ipValue1) == (mask & ipValue2);
}

bool checkSameSegment(std::string& ip1,std::string& ip2, unsigned long mask)
{
	if(!ipV4Validate(ip1))
	{
		return false;
	}
	if(!ipV4Validate(ip2))
	{
		return false;
	}
	unsigned long ipValue1 = getIpV4Value(ip1);
	unsigned long ipValue2 = getIpV4Value(ip2);
	return (mask & ipValue1) == (mask & ipValue2);
}

int checkIpV4Type(std::string & ipV4)
{
	unsigned long IpATypeRange[2] ={0};
	IpATypeRange[0] = getIpV4Value("1.0.0.1");
	IpATypeRange[1] = getIpV4Value("126.255.255.255");

	unsigned long IpBTypeRange[2] ={0};
	IpBTypeRange[0] = getIpV4Value("128.0.0.1");
	IpBTypeRange[1] = getIpV4Value("191.255.255.255");

	unsigned long IpCTypeRange[2] ={0};
	IpCTypeRange[0] = getIpV4Value("192.168.0.0");
	IpCTypeRange[1] = getIpV4Value("223.255.255.255");

	unsigned long IpDTypeRange[2] ={0};
	IpDTypeRange[0] = getIpV4Value("224.0.0.0");
	IpDTypeRange[1] = getIpV4Value("239.255.255.255");

	unsigned long IpETypeRange[2] ={0};
	IpETypeRange[0] = getIpV4Value("240.0.0.0");
	IpETypeRange[1] = getIpV4Value("255.255.255.254");

	unsigned long inValue = getIpV4Value(ipV4);
	if(inValue >= IpETypeRange[0] && inValue <= IpETypeRange[1])
	{
		return IPTYPEE;
	}
	else if(inValue >= IpDTypeRange[0] && inValue <= IpDTypeRange[1])
	{
		return IPTYPED;
	}
	if(inValue >= IpCTypeRange[0] && inValue <= IpCTypeRange[1])
	{
		return IPTYPEC;
	}
	else if(inValue >= IpBTypeRange[0] && inValue <= IpBTypeRange[1])
	{
		return IPTYPEB;
	}
	else if(inValue >= IpATypeRange[0] && inValue <= IpATypeRange[1])
	{
		return IPTYPEA;
	}
	return 0;
}

bool checkIpV4Private(std::string & ipV4)
{
	bool bRet=false;
	unsigned long IpATypeRange[2] ={0};
	IpATypeRange[0] = getIpV4Value("10.0.0.1");
	IpATypeRange[1] = getIpV4Value("10.255.255.255");

	unsigned long IpBTypeRange[2] ={0};
	IpBTypeRange[0] = getIpV4Value("172.16.0.1");
	IpBTypeRange[1] = getIpV4Value("172.31.255.255");

	unsigned long IpCTypeRange[2] ={0};
	IpCTypeRange[0] = getIpV4Value("192.168.0.1");
	IpCTypeRange[1] = getIpV4Value("192.168.255.255");

	unsigned long inValue = getIpV4Value(ipV4);
	if(inValue >= IpCTypeRange[0] && inValue <= IpCTypeRange[1])
	{
		bRet=true;
	}
	else if(inValue >= IpBTypeRange[0] && inValue <= IpBTypeRange[1])
	{
		bRet=true;
	}
	else if(inValue >= IpATypeRange[0] && inValue <= IpATypeRange[1])
	{
		bRet=true;
	}
	return bRet;
}

bool GetCurNetCardsInfo(NetCardInfo& tNetCardInfo, int& byCrdIdx)
{
	//利用IP helper API获取适配器信息
	PIP_ADAPTER_INFO pAdapterInfo = NULL,pAdapterInfoBkp;
	ULONG ulAdapterInfoSize = 0;
	GetAdaptersInfo(pAdapterInfo,&ulAdapterInfoSize);
	pAdapterInfo=(PIP_ADAPTER_INFO)malloc(ulAdapterInfoSize);
	//pAdapterInfo=(PIP_ADAPTER_INFO)GlobalAlloc(GPTR,ulAdapterInfoSize);
	if(pAdapterInfo ==NULL) return false;
	pAdapterInfoBkp=pAdapterInfo;
	//获取网卡详细信息, 自动分配长度
	if(GetAdaptersInfo(pAdapterInfo,&ulAdapterInfoSize)==ERROR_SUCCESS)
	{
		// 解析网卡信息(取第一个有地址的作为当前使用)
		for(int byIdx = 0; pAdapterInfo != NULL && byIdx < NUM_MAX_NETCARD; byIdx++) 
		{
			if(byIdx >= NUM_MAX_NETCARD) break;	/* MAX NETCARD */
			IP_ADDR_STRING *pIpAddrString = &(pAdapterInfo->IpAddressList);
			if (pAdapterInfo->DhcpEnabled > 0) {
				pIpAddrString = &(pAdapterInfo->DhcpServer);
			}
			if (byIdx != byCrdIdx) {
				pAdapterInfo = pAdapterInfo->Next;
				continue;
			}
			// 获取网线正常的网卡IP地址
			std::string strIpAddr = pIpAddrString->IpAddress.String;
			if (strIpAddr.empty() == FALSE && strIpAddr != "0.0.0.0") 
			{
				tNetCardInfo.m_strLocalIP = pIpAddrString->IpAddress.String;
				tNetCardInfo.m_strSubnetMask = pIpAddrString->IpMask.String;
				tNetCardInfo.m_strGateway = pAdapterInfo->GatewayList.IpAddress.String;
			}
			pAdapterInfo = pAdapterInfo->Next;
		}
	}
	if (pAdapterInfoBkp)
	{
		free(pAdapterInfoBkp);
		//GlobalFree(pAdapterInfoBkp);
	}
	// 是否取到
	if(tNetCardInfo.m_strLocalIP.empty() == TRUE || 
		tNetCardInfo.m_strLocalIP == "0.0.0.0")
	{
		return false;
	}
	return true;
}

bool GetOneSameDestIP(std::string & strLocalIP,std::string & strDestIP)
{
	bool bRet=false;
	unsigned long mask =0;
	std::string strNetMask="255.255.0.0";
	char msg[128]={0};
	NetCardInfo tNetCardInfo;
	if(!ipV4Validate(strDestIP))
	{
		OutputDebugString("GetAdaptersInfo :ipV4Validate false\n");
		return bRet;
	}
	bool bPriDest=checkIpV4Private(strDestIP);
	int nDestType=checkIpV4Type(strDestIP);
	for(int nCardIndex=0;nCardIndex < NUM_MAX_NETCARD;nCardIndex++)
	{
		bool bGet=GetCurNetCardsInfo(tNetCardInfo,nCardIndex);
		if(bGet)
		{
			mask = getIpV4Value(strNetMask);
			bool bPriSrc=checkIpV4Private(tNetCardInfo.m_strLocalIP);
			int  nSrcType=checkIpV4Type(tNetCardInfo.m_strLocalIP);
			//bool bCheck=checkSameSegment(tNetCardInfo.m_strLocalIP,strDestIP,mask);
			if(nSrcType ==nDestType)
			{
				strLocalIP =tNetCardInfo.m_strLocalIP;
				bRet=true;
			}
			else if(bPriSrc ==false && bPriDest ==false)
			{
				strLocalIP =tNetCardInfo.m_strLocalIP;
				bRet=true;
			}
			if(bRet)
			{
				sprintf(msg,"GetAdaptersInfo one ,mask:%s,gateway:%s,localip:%s\n",tNetCardInfo.m_strSubnetMask.c_str(),tNetCardInfo.m_strGateway.c_str(),tNetCardInfo.m_strLocalIP.c_str());
				OutputDebugString(msg);
				break;
			}
		}
	}
	return bRet;
}

bool GetOneSameDestIP2(std::string & strLocalIP,std::string & strDestIP)
{
	bool bRet=false;
	std::string strNetMask="255.255.0.0";
	char msg[128]={0};
	char host_name[256] ={0};
	std::string strGetIP="";
	if (gethostname(host_name, sizeof(host_name) -1 ) == SOCKET_ERROR) return bRet;
	struct hostent *phe = gethostbyname(host_name);
	if (phe == 0) return bRet;
	if (strDestIP.size() <= 0) return bRet;
	if(!ipV4Validate(strDestIP))
	{
		OutputDebugString("gethostbyname ipV4Validate false \n");
		return bRet;
	}
	unsigned long mask = getIpV4Value(strNetMask);
	bool bPriDest=checkIpV4Private(strDestIP);
	int nDestType=checkIpV4Type(strDestIP);
	char * pDest=(char *)strDestIP.c_str();
	for(int i = 0; phe->h_addr_list[i] != 0; i++)
	{
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i],sizeof(struct in_addr) );
		char * p=inet_ntoa(addr);
		if(p)
		{
			strGetIP =p;
			bool bPriSrc=checkIpV4Private(strGetIP);
			int  nSrcType=checkIpV4Type(strGetIP);
			//if( (bPriSrc ==false && bPriDest ==false) || 
			//	(bPriSrc && bPriDest && nSrcType ==nDestType) )
			if(nSrcType ==nDestType)
			{
				strLocalIP =p;
				bRet=true;
			}
			else if(bPriSrc ==false && bPriDest ==false)
			{
				strLocalIP =p;
				bRet=true;
			}
			if(bRet)
			{
				sprintf(msg,"gethostbyname one ,localip:%s\n",p);
				OutputDebugString(msg);
				break;
			}
		}
	}
	return bRet;
}

//匹配目的IP规则
bool GetResultCompareIP(int index,std::string & strGetIP,std::string & strDestIP)
{
	bool bRet =false;
	bool bPriDest=checkIpV4Private(strDestIP);
	int  nDestType=checkIpV4Type(strDestIP);
	bool bPriSrc=checkIpV4Private(strGetIP);
	int  nSrcType=checkIpV4Type(strGetIP);

	//route check
	RouteItemSave & route =CallMgr::Instance().m_AddrRoute;
	int ipLocaToken[4]={0};
	int ipDestToken[4]={0};
	int ipRtLocaToken[4]={0};
	int ipRtDestToken[4]={0};
	bool bSameLocal=false;
	bool bSameDest=false;
	GetIPTokenNum(strGetIP,ipLocaToken);
	GetIPTokenNum(strDestIP,ipDestToken);
	for(int i=0;i<route.m_nNum;i++)
	{
		bSameLocal=true;
		bSameDest=true;
		if(GetIPTokenNum(route.m_AllIP[i].strIP,ipRtLocaToken) && GetIPTokenNum(route.m_AllIP[i].strDest,ipRtDestToken) )
		{
			for(int j=0;j<route.m_AllIP[i].nType;j++)
			{
				bSameLocal =bSameLocal && ipLocaToken[j] ==ipRtLocaToken[j];
				bSameDest =bSameDest && ipDestToken[j] ==ipRtDestToken[j];
			}
			bRet = bSameLocal && bSameDest ? true:false;
		}
		if(bRet) return bRet;
	}

	switch(index)
	{
	case 0:
		//prite ip and 192,not 172 public ip
		bRet= (bPriSrc ==true && bPriDest ==false && strGetIP.substr(0,3).compare("192") ==0 && strDestIP.substr(0,3).compare("172") !=0) ? true:false;
		break;
	case 1:
		//prite ip and 10,not 172 public ip
		bRet= (bPriSrc ==true && bPriDest ==false && strGetIP.substr(0,2).compare("10") ==0 && strDestIP.substr(0,3).compare("172") !=0) ? true:false;
		break;
	case 2:
		//172 special handling
		bRet = (strDestIP.substr(0,3).compare("172") ==0 && strGetIP.substr(0,3).compare("172") ==0) ? true:false;
		break;
	case 3:
		//private ip and big category same
		bRet= (bPriSrc && bPriDest && nSrcType ==nDestType) ? true:false;
		break;
	case 4:
		//public ip
		bRet= (bPriSrc ==false && bPriDest ==false ) ? true:false;
		break;
	default:
		break;
	}
	return bRet;
}

bool GetOneSameDestIP3(IPItemSave & ipSave,std::string & strLocalIP,std::string & strDestIP)
{
	int  n=0;
	bool bRet=false;
	char msg[128]={0};
	std::string strGetIP="";
	if (strDestIP.size() <= 0) return bRet;
	if(!ipV4Validate(strDestIP))
	{
		OutputDebugString("GetLocalAllIP ipV4Validate false \n");
		LogFile(NULL,NULL,"","","GetLocalAllIP ipV4Validate false \n");
		return bRet;
	}
	//匹配目的IP规则 最大8个规则
	for(n=0;n<8;n++)
	{
		for(int i=0;i<ipSave.m_nNum;i++)
		{
			strGetIP =ipSave.m_AllIP[i].strIP;
			bRet =GetResultCompareIP(n,strGetIP,strDestIP);
			if(bRet) goto SaveResult;
		}
	}

SaveResult:
	strLocalIP =strGetIP;
	sprintf(msg,"GetLocalSameToDestIP result,rule:%d,localip:%s,destip:%s\n",n,strLocalIP.c_str(),strDestIP.c_str());
	OutputDebugString(msg);
	LogFile(NULL,NULL,"","",msg);
	IPCPrintf(1, 0, "%s", msg);
	return bRet;
}

bool GetLocalAllIP(IPItemSave & ipSave)
{
	bool bRet=false;
	char host_name[256] ={0};
	int nType=0;

	if (gethostname(host_name, sizeof(host_name) -1 ) == SOCKET_ERROR)
	{
		OutputDebugString("gethostname get local ip error.\n");
		return bRet;
	}
	struct hostent *phe = gethostbyname(host_name);
	if (phe == 0)
	{
		OutputDebugString("gethostbyname get local ip error.\n");
		return bRet;
	}

	ipSave.m_nNum =0;
	for(int i = 0; phe->h_addr_list[i] != 0; i++)
	{
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i],sizeof(struct in_addr) );
		char * p=inet_ntoa(addr);
		if(p)
		{
			if(ipSave.m_nNum <MaxIPCount)
			{
				ipSave.m_AllIP[ipSave.m_nNum].strIP =p;
				ipSave.m_AllIP[ipSave.m_nNum].nIP =addr.S_un.S_addr;
				ipSave.m_AllIP[ipSave.m_nNum].strMask = "";
				nType=checkIpV4Type(ipSave.m_AllIP[ipSave.m_nNum].strIP);
				ipSave.m_AllIP[ipSave.m_nNum].nType =(IPADDRTYPE)nType;
				ipSave.m_nNum ++;
			}
			bRet =true;
		}
	}
	//output
	if(bRet)
	{
		char msg[128]={0};
		for(int i=0;i<ipSave.m_nNum;i++)
		{
			sprintf(msg,"gethostbyname get local ip Total:%d,index:%d,type:%d,ip:%s\n",ipSave.m_nNum,i,ipSave.m_AllIP[i].nType,ipSave.m_AllIP[i].strIP.c_str());
			OutputDebugString(msg);
			LogFile(NULL,NULL,"","",msg);
			IPCPrintf(1, 0, "%s", msg);
		}
	}
	else
	{
		OutputDebugString("gethostbyname local ip not get.\n");
		LogFile(NULL,NULL,"","","gethostbyname local ip not get.");
		IPCPrintf(1, 0, "gethostbyname local ip not get.\n");
	}

	return bRet;
}

//reference RvHostWindowsGetIpV4List
bool HostWindowsGetIpV4List(IPItemSave & ipSave)
{
	bool bRet=false;
	SOCKET s;
	DWORD bytesReturned;
	int res;
	unsigned int numLocalAddr;
	int i,j;
	INTERFACE_INFO localAddr[MaxIPCount];
	int nType =0;

	//Create dummy socket in IPv4 address family
	s = socket(AF_INET, SOCK_STREAM, 0);

	if(s != INVALID_SOCKET)
	{
		res = WSAIoctl(s, SIO_GET_INTERFACE_LIST, NULL, 0, (LPVOID)&localAddr[0],sizeof(localAddr), &bytesReturned, NULL, NULL);
		if (res == SOCKET_ERROR)
		{
			OutputDebugString("WSAIoctl get local ip error.\n");
			LogFile(NULL,NULL,"","","WSAIoctl get local ip error.");
			return bRet;
		}
		ipSave.m_nNum =0;
		numLocalAddr = (bytesReturned / sizeof(INTERFACE_INFO));

		for(i = 0, j = 0; j < numLocalAddr && i < MaxIPCount; j++)
		{
			if(localAddr[j].iiFlags & IFF_LOOPBACK) continue;
			if(ipSave.m_nNum <MaxIPCount)
			{
				ipSave.m_AllIP[ipSave.m_nNum].strIP =inet_ntoa(localAddr[j].iiAddress.AddressIn.sin_addr);
				ipSave.m_AllIP[ipSave.m_nNum].nIP =localAddr[j].iiAddress.AddressIn.sin_addr.S_un.S_addr;
				ipSave.m_AllIP[ipSave.m_nNum].strMask =inet_ntoa(localAddr[j].iiNetmask.AddressIn.sin_addr);
				nType=checkIpV4Type(ipSave.m_AllIP[ipSave.m_nNum].strIP);
				ipSave.m_AllIP[ipSave.m_nNum].nType =(IPADDRTYPE)nType;
				ipSave.m_nNum ++;
			}

			bRet =true;
			i++;
		}

		closesocket(s);
	}

	//GetAdaptersInfo second get
	if(ipSave.m_nNum <=1)
	{
		NetCardInfo tNetCardInfo;
		bool bHas =false;
		for(int nCardIndex=0;nCardIndex < NUM_MAX_NETCARD;nCardIndex++)
		{
			bool bGet=GetCurNetCardsInfo(tNetCardInfo,nCardIndex);
			if(bGet && ipSave.m_nNum <MaxIPCount)
			{
				//find
				bHas =false;
				for(i=0;i<ipSave.m_nNum;i++)
				{
					if(ipSave.m_AllIP[i].nIP == inet_addr(tNetCardInfo.m_strLocalIP.c_str()))
					{
						bHas=true;
						break;
					}
				}
				//find not
				if(bHas ==false)
				{
					ipSave.m_AllIP[ipSave.m_nNum].strIP =tNetCardInfo.m_strLocalIP;
					ipSave.m_AllIP[ipSave.m_nNum].nIP =inet_addr(tNetCardInfo.m_strLocalIP.c_str());
					ipSave.m_AllIP[ipSave.m_nNum].strMask =tNetCardInfo.m_strSubnetMask;
					nType=checkIpV4Type(ipSave.m_AllIP[ipSave.m_nNum].strIP);
					ipSave.m_AllIP[ipSave.m_nNum].nType =(IPADDRTYPE)nType;
					ipSave.m_nNum ++;
					bRet =true;
				}
			}
		}
	}

	//output
	if(bRet)
	{
		char msg[128]={0};
		for(i=0;i<ipSave.m_nNum;i++)
		{
			sprintf(msg,"WSAIoctl get local ip Total:%d,index:%d,type:%d,ip:%s,mask:%s\n",ipSave.m_nNum,i,ipSave.m_AllIP[i].nType,ipSave.m_AllIP[i].strIP.c_str(),ipSave.m_AllIP[i].strMask.c_str());
			OutputDebugString(msg);
			LogFile(NULL,NULL,"","",msg);
			IPCPrintf(1, 0, "%s", msg);
		}
	}
	else
	{
		OutputDebugString("WSAIoctl local ip not get.\n");
		LogFile(NULL,NULL,"","","WSAIoctl local ip not get.");
		IPCPrintf(1, 0, "WSAIoctl local ip not get.\n");
	}

    return bRet;
}

bool GetIPTokenNum(std::string strIP,int ipToken[4])
{
	if(!ipV4Validate(strIP))
	{
		return false;
	}
	unsigned long nIP=0;
	nIP = inet_addr(strIP.c_str());
	for(int i=0;i<4;i++) ipToken[i] = (nIP >>(i*8))&0xFF;
	return true;
}
