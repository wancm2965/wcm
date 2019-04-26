#include "DNSLookup.h"
#include <stdio.h>
#include <string.h>
#include <ws2tcpip.h>

CDNSParser::CDNSParser() : 
    m_bIsInitOK(FALSE), 
    m_sock(INVALID_SOCKET),
    m_szDNSPacket(NULL)
{
    m_bIsInitOK = Init();
}

CDNSParser::~CDNSParser()
{
    UnInit();
}

BOOL CDNSParser::DNSParser(ULONG ulDNSServerIP, char *szDomainName, std::vector<ULONG> *pveculIPList, std::vector<std::string> *pvecstrCNameList, ULONG ulTimeout, ULONG *pulTimeSpent)
{
    return DNSParserCore(ulDNSServerIP, szDomainName, pveculIPList, pvecstrCNameList, ulTimeout, pulTimeSpent);
}

BOOL CDNSParser::DNSParser(ULONG ulDNSServerIP, char *szDomainName, std::vector<std::string> *pvecstrIPList, std::vector<std::string> *pvecstrCNameList, ULONG ulTimeout, ULONG *pulTimeSpent)
{
    std::vector<ULONG> *pveculIPList = NULL;
    if (pvecstrIPList != NULL)
    {
        std::vector<ULONG> veculIPList;
        pveculIPList = &veculIPList;
    }

    BOOL bRet = DNSParserCore(ulDNSServerIP, szDomainName, pveculIPList, pvecstrCNameList, ulTimeout, pulTimeSpent);

    if (bRet)
    {
        pvecstrIPList->clear();
        char szIP[16] = {'\0'};
        for (std::vector<ULONG>::iterator iter = pveculIPList->begin(); iter != pveculIPList->end(); ++iter)
        {
            BYTE *pbyIPSegment = (BYTE*)(&(*iter));
            //sprintf_s(szIP, 16, "%d.%d.%d.%d", pbyIPSegment[0], pbyIPSegment[1], pbyIPSegment[2], pbyIPSegment[3]);
            sprintf(szIP, "%d.%d.%d.%d", pbyIPSegment[0], pbyIPSegment[1], pbyIPSegment[2], pbyIPSegment[3]);
            pvecstrIPList->push_back(szIP);
        }
    }

    return bRet;
}


BOOL CDNSParser::Init()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR)
    {
        return FALSE;
    }
    
    if ((m_sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        return FALSE;
    }

	////add
	//SetLocalAddr(12000,"192.168.15.100");
	//if (bind(m_sock,(struct sockaddr *)&m_struSockLocalAddr,sizeof(sockaddr))==SOCKET_ERROR)
	//{
	//	return FALSE;
	//}

    m_event = WSACreateEvent();
    WSAEventSelect(m_sock, m_event, FD_READ);

    m_szDNSPacket = new (std::nothrow) char[DNS_PACKET_MAX_SIZE];
    if (m_szDNSPacket == NULL)
    {
        return FALSE;
    }

    m_usCurrentProcID = (USHORT)GetCurrentProcessId();

    return TRUE;
}

void CDNSParser::SetLocalAddr(unsigned short SocketPort,void* SocketIPAS)
{
	m_struSockLocalAddr.sin_family=AF_INET;
	m_struSockLocalAddr.sin_port=htons(SocketPort);  
	m_struSockLocalAddr.sin_addr.s_addr=inet_addr((char * )(SocketIPAS));
}

BOOL CDNSParser::UnInit()
{
    WSACleanup();

    if (m_szDNSPacket != NULL)
    {
        delete [] m_szDNSPacket;
    }

    return TRUE;
}

BOOL CDNSParser::DNSParserCore(ULONG ulDNSServerIP, char *szDomainName, std::vector<ULONG> *pveculIPList, std::vector<std::string> *pvecstrCNameList, ULONG ulTimeout, ULONG *pulTimeSpent)
{
    if (m_bIsInitOK == FALSE || szDomainName == NULL)
    {
        return FALSE;
    }

    //配置SOCKET
    sockaddr_in sockAddrDNSServer; 
    sockAddrDNSServer.sin_family = AF_INET; 
    sockAddrDNSServer.sin_addr.s_addr = ulDNSServerIP;
    sockAddrDNSServer.sin_port = htons( DNS_PORT );

    //DNS查询与解析
    if (!SendDNSRequest(sockAddrDNSServer, szDomainName)
        || !RecvDNSResponse(sockAddrDNSServer, ulTimeout, pveculIPList, pvecstrCNameList, pulTimeSpent))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CDNSParser::SendDNSRequest(sockaddr_in sockAddrDNSServer, char *szDomainName)
{
    char *pWriteDNSPacket = m_szDNSPacket;
    memset(pWriteDNSPacket, 0, DNS_PACKET_MAX_SIZE);    

    //填充DNS查询报文头部
    DNSHeader *pDNSHeader = (DNSHeader*)pWriteDNSPacket;
    pDNSHeader->usTransID = m_usCurrentProcID;
    pDNSHeader->usFlags = htons(0x0100);
    pDNSHeader->usQuestionCount = htons(0x0001);
    pDNSHeader->usAnswerCount = 0x0000;
    pDNSHeader->usAuthorityCount = 0x0000;
    pDNSHeader->usAdditionalCount = 0x0000;

    //设置DNS查询报文内容
    USHORT usQType = htons(0x0001);
    USHORT usQClass = htons(0x0001);
    USHORT nDomainNameLen = strlen(szDomainName);
    char *szEncodedDomainName = (char *)malloc(nDomainNameLen + 2);
    if (szEncodedDomainName == NULL)
    {
        return FALSE;
    }
    if (!EncodeDotStr(szDomainName, szEncodedDomainName, nDomainNameLen + 2))
    {
        return FALSE;
    }

    //填充DNS查询报文内容
    USHORT nEncodedDomainNameLen = strlen(szEncodedDomainName) + 1;
    memcpy(pWriteDNSPacket += sizeof(DNSHeader), szEncodedDomainName, nEncodedDomainNameLen);
    memcpy(pWriteDNSPacket += nEncodedDomainNameLen, (char*)(&usQType), DNS_TYPE_SIZE);
    memcpy(pWriteDNSPacket += DNS_TYPE_SIZE, (char*)(&usQClass), DNS_CLASS_SIZE);
    free (szEncodedDomainName);

    //发送DNS查询报文
    USHORT nDNSPacketSize = sizeof(DNSHeader) + nEncodedDomainNameLen + DNS_TYPE_SIZE + DNS_CLASS_SIZE;
    if (sendto(m_sock, m_szDNSPacket, nDNSPacketSize, 0, (sockaddr*)&sockAddrDNSServer, sizeof(sockAddrDNSServer)) == SOCKET_ERROR)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CDNSParser::RecvDNSResponse(sockaddr_in sockAddrDNSServer, ULONG ulTimeout, std::vector<ULONG> *pveculIPList, std::vector<std::string> *pvecstrCNameList, ULONG *pulTimeSpent)
{
    ULONG ulSendTimestamp = GetTickCountCalibrate();

    if (pveculIPList != NULL)
    {
        pveculIPList->clear();
    }
    if (pvecstrCNameList != NULL)
    {
        pvecstrCNameList->clear();
    }

    char recvbuf[1024] = {'\0'};
    char szDotName[128] = {'\0'};
    USHORT nEncodedNameLen = 0;

    while (TRUE)
    {
        if (WSAWaitForMultipleEvents(1, &m_event, FALSE, 100, FALSE) != WSA_WAIT_TIMEOUT)
        {
            WSANETWORKEVENTS netEvent;
            WSAEnumNetworkEvents(m_sock, m_event, &netEvent);

            if (netEvent.lNetworkEvents & FD_READ)
            {
                ULONG ulRecvTimestamp = GetTickCountCalibrate();
                int nSockaddrDestSize = sizeof(sockAddrDNSServer);

                //接收响应报文
                if (recvfrom(m_sock, recvbuf, 1024, 0, (struct sockaddr*)&sockAddrDNSServer, &nSockaddrDestSize) != SOCKET_ERROR)
                {
                    DNSHeader *pDNSHeader = (DNSHeader*)recvbuf;
                    USHORT usQuestionCount = 0;
                    USHORT usAnswerCount = 0;

                    if (pDNSHeader->usTransID == m_usCurrentProcID
                        //&& (ntohs(pDNSHeader->usFlags) & 0xfb7f) == 0x8100 //RFC1035 4.1.1(Header section format)
                        && (usQuestionCount = ntohs(pDNSHeader->usQuestionCount)) >= 0
                        && (usAnswerCount = ntohs(pDNSHeader->usAnswerCount)) > 0)
                    {
                        char *pDNSData = recvbuf + sizeof(DNSHeader);

                        //解析Question字段
                        for (int q = 0; q != usQuestionCount; ++q)
                        {
                            if (!DecodeDotStr(pDNSData, &nEncodedNameLen, szDotName, sizeof(szDotName)))
                            {
                                return FALSE;
                            }
                            pDNSData += (nEncodedNameLen + DNS_TYPE_SIZE + DNS_CLASS_SIZE);
                        }

                        //解析Answer字段
                        for (int a = 0; a != usAnswerCount; ++a)
                        {
                            if (!DecodeDotStr(pDNSData, &nEncodedNameLen, szDotName, sizeof(szDotName), recvbuf))
                            {
                                return FALSE;
                            }
                            pDNSData += nEncodedNameLen;

                            USHORT usAnswerType = ntohs(*(USHORT*)(pDNSData));
                            USHORT usAnswerClass = ntohs(*(USHORT*)(pDNSData + DNS_TYPE_SIZE));
                            ULONG usAnswerTTL = ntohl(*(ULONG*)(pDNSData + DNS_TYPE_SIZE + DNS_CLASS_SIZE));
                            USHORT usAnswerDataLen = ntohs(*(USHORT*)(pDNSData + DNS_TYPE_SIZE + DNS_CLASS_SIZE + DNS_TTL_SIZE));
                            pDNSData += (DNS_TYPE_SIZE + DNS_CLASS_SIZE + DNS_TTL_SIZE + DNS_DATALEN_SIZE);

                            if (usAnswerType == DNS_TYPE_A && pveculIPList != NULL)
                            {
                                ULONG ulIP = *(ULONG*)(pDNSData);
                                pveculIPList->push_back(ulIP);
                            }
                            else if (usAnswerType == DNS_TYPE_CNAME && pvecstrCNameList != NULL)
                            {
                                if (!DecodeDotStr(pDNSData, &nEncodedNameLen, szDotName, sizeof(szDotName), recvbuf))
                                {
                                    return FALSE;
                                }
                                pvecstrCNameList->push_back(szDotName);
                            }

                            pDNSData += (usAnswerDataLen);
                        }

                        //计算DNS查询所用时间
                        if (pulTimeSpent != NULL)
                        {
                            *pulTimeSpent = ulRecvTimestamp - ulSendTimestamp;
                        }

                        break;
                    }
                }
            }
        }

        //超时退出
        if (GetTickCountCalibrate() - ulSendTimestamp > ulTimeout)
        {
            *pulTimeSpent = ulTimeout + 1;
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * convert "www.baidu.com" to "\x03www\x05baidu\x03com"
 * 0x0000 03 77 77 77 05 62 61 69 64 75 03 63 6f 6d 00 ff
 */
BOOL CDNSParser::EncodeDotStr(char *szDotStr, char *szEncodedStr, USHORT nEncodedStrSize)
{
    USHORT nDotStrLen = strlen(szDotStr);

    if (szDotStr == NULL || szEncodedStr == NULL || nEncodedStrSize < nDotStrLen + 2)
    {
        return FALSE;
    }

    char *szDotStrCopy = new char[nDotStrLen + 1];
    //strcpy_s(szDotStrCopy, nDotStrLen + 1, szDotStr);
    strcpy(szDotStrCopy, szDotStr);

    char *pNextToken = NULL;
    //char *pLabel = strtok_s(szDotStrCopy, ".", &pNextToken);
    char *pLabel = strtok(szDotStrCopy, ".");
    USHORT nLabelLen = 0;
    USHORT nEncodedStrLen = 0;
    while (pLabel != NULL)
    {
        if ((nLabelLen = strlen(pLabel)) != 0)
        {
            //sprintf_s(szEncodedStr + nEncodedStrLen, nEncodedStrSize - nEncodedStrLen, "%c%s", nLabelLen, pLabel);
            sprintf(szEncodedStr + nEncodedStrLen, "%c%s", nLabelLen, pLabel);
            nEncodedStrLen += (nLabelLen + 1);
        }
        //pLabel = strtok_s(NULL, ".", &pNextToken);
        pLabel = strtok(NULL, ".");
    }

    delete [] szDotStrCopy;
    
    return TRUE;
}

/*
 * convert "\x03www\x05baidu\x03com\x00" to "www.baidu.com"
 * 0x0000 03 77 77 77 05 62 61 69 64 75 03 63 6f 6d 00 ff
 * convert "\x03www\x05baidu\xc0\x13" to "www.baidu.com"
 * 0x0000 03 77 77 77 05 62 61 69 64 75 c0 13 ff ff ff ff
 * 0x0010 ff ff ff 03 63 6f 6d 00 ff ff ff ff ff ff ff ff
 */
BOOL CDNSParser::DecodeDotStr(char *szEncodedStr, USHORT *pusEncodedStrLen, char *szDotStr, USHORT nDotStrSize, char *szPacketStartPos)
{
    if (szEncodedStr == NULL || pusEncodedStrLen == NULL || szDotStr == NULL)
    {
        return FALSE;
    }

    char *pDecodePos = szEncodedStr;
    USHORT usPlainStrLen = 0;
    BYTE nLabelDataLen = 0;    
    *pusEncodedStrLen = 0;

    while ((nLabelDataLen = *pDecodePos) != 0x00)
    {
        if ((nLabelDataLen & 0xc0) == 0) //普通格式，LabelDataLen + Label
        {
            if (usPlainStrLen + nLabelDataLen + 1 > nDotStrSize)
            {
                return FALSE;
            }
            memcpy(szDotStr + usPlainStrLen, pDecodePos + 1, nLabelDataLen);
            memcpy(szDotStr + usPlainStrLen + nLabelDataLen, ".", 1);
            pDecodePos += (nLabelDataLen + 1);
            usPlainStrLen += (nLabelDataLen + 1);
            *pusEncodedStrLen += (nLabelDataLen + 1);
        }
        else //消息压缩格式，11000000 00000000，两个字节，前2位为跳转标志，后14位为跳转的偏移
        {
            if (szPacketStartPos == NULL)
            {
                return FALSE;
            }
            USHORT usJumpPos = ntohs(*(USHORT*)(pDecodePos)) & 0x3fff;
            USHORT nEncodeStrLen = 0;
            if (!DecodeDotStr(szPacketStartPos + usJumpPos, &nEncodeStrLen, szDotStr + usPlainStrLen, nDotStrSize - usPlainStrLen, szPacketStartPos))
            {
                return FALSE;
            }
            else
            {
                *pusEncodedStrLen += 2;
                return TRUE;
            }
        }
    }

    szDotStr[usPlainStrLen - 1] = '\0';
    *pusEncodedStrLen += 1;

    return TRUE;
}

ULONG CDNSParser::GetTickCountCalibrate()
{
    static ULONG s_ulFirstCallTick = 0;
    static LONGLONG s_ullFirstCallTickMS = 0;

    SYSTEMTIME systemtime;
    FILETIME filetime;
    GetLocalTime(&systemtime);    
    SystemTimeToFileTime(&systemtime, &filetime);
    LARGE_INTEGER liCurrentTime;
    liCurrentTime.HighPart = filetime.dwHighDateTime;
    liCurrentTime.LowPart = filetime.dwLowDateTime;
    LONGLONG llCurrentTimeMS = liCurrentTime.QuadPart / 10000;

    if (s_ulFirstCallTick == 0)
    {
        s_ulFirstCallTick = GetTickCount();
    }
    if (s_ullFirstCallTickMS == 0)
    {
        s_ullFirstCallTickMS = llCurrentTimeMS;
    }

    return s_ulFirstCallTick + (ULONG)(llCurrentTimeMS - s_ullFirstCallTickMS);
}

int DNSParserTest(char * szDomainName,const char * szDNSServ,int nMaxTimeOut)
{
    std::vector<ULONG> veculIPList;
    std::vector<std::string> vecstrIPList;
    std::vector<std::string> vecCNameList;
    ULONG ulTimeSpent = 0;
    CDNSParser dnslookup;
	//add
	//dnslookup.SetLocalAddr(1715,"192.168.15.100");
    BOOL bRet = dnslookup.DNSParser(inet_addr(szDNSServ), szDomainName, &vecstrIPList, &vecCNameList, nMaxTimeOut *1000, &ulTimeSpent);
	char msg[512]={0};
	sprintf(msg,"DNSParser result DNSServ(%s),DNS(%s):\n", szDNSServ,szDomainName); OutputDebugString(msg);
    if (!bRet)
    {
        sprintf(msg,"timeout!\n"); OutputDebugString(msg);
        return -1;
    }

    for (int i = 0; i != veculIPList.size(); ++i)
    {
        sprintf(msg,"IP%d(ULONG) = %u\n", i + 1, veculIPList[i]); OutputDebugString(msg);
    }
    for (int i = 0; i != vecstrIPList.size(); ++i)
    {
        sprintf(msg,"IP%d(string) = %s\n", i + 1, vecstrIPList[i].c_str()); OutputDebugString(msg);
    }
    for (int i = 0; i != vecCNameList.size(); ++i)
    {
        sprintf(msg,"CName%d = %s\n", i + 1, vecCNameList[i].c_str()); OutputDebugString(msg);
    }
    sprintf(msg,"time spent = %ums\n", ulTimeSpent); OutputDebugString(msg);
    
    return 0;
}


void LogOutputPrint(char *szFormat, ...)
{
	va_list args;
	va_start(args, szFormat);
	char buffer[2048]={0};

	_vsnprintf(buffer,sizeof(buffer),szFormat,args);
	OutputDebugString(buffer);

	va_end(args);
}

bool GetDNSNameIP(char * strDNS,char * DNSIP,int & nLen)
{
    //http://msdn.microsoft.com/en-us/library/ms738520(VS.85).aspx
	bool bRet=false;
    DWORD dwRetval;
    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;
    struct sockaddr_in  *sockaddr_ipv4;
	char * pIP=NULL;
    //iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

	//Call getaddrinfo(). If the call succeeds,the result variable will hold a linked list of addrinfo structures containing response information
	dwRetval = getaddrinfo(strDNS,NULL, &hints, &result);
	if ( dwRetval != 0 )
	{
		LogOutputPrint("getaddrinfo failed with error: %d\n", dwRetval);
		//WSACleanup();
		return bRet;
	}

    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next)
	{
        switch (ptr->ai_family)
		{
            case AF_UNSPEC:
                LogOutputPrint("getaddrinfo Unspecified\n");
                break;
            case AF_INET:
                LogOutputPrint("getaddrinfo AF_INET (IPv4)\n");
                sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
				pIP =inet_ntoa(sockaddr_ipv4->sin_addr);
                LogOutputPrint("\tIPv4 address %s\n",pIP );

				nLen=strlen(pIP);
				strcpy(DNSIP,pIP);
				if(nLen >0) bRet=true;
                break;
            case AF_INET6:
                LogOutputPrint("getaddrinfo AF_INET6 (IPv6)\n");
                break;
            case AF_NETBIOS:
                LogOutputPrint("getaddrinfo AF_NETBIOS (NetBIOS)\n");
                break;
            default:
                LogOutputPrint("getaddrinfo Other %ld\n", ptr->ai_family);
                break;
        }
    }

    freeaddrinfo(result);
    //WSACleanup();
	return bRet;
}

bool GetDNSNameIP2(const char * strDNS,char * DNSIP,int & nLen)
{
	bool bRet=false;
	int  i=0;
	int  nDNSLen=strlen(strDNS);
	bool bAT=false;
	if(nDNSLen <256)
	{
		char temp[256]={0};
		strcpy(temp,strDNS);

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

		if(bRet) LogOutputPrint("gethostbyname dns:%s,ip:%s\n",temp,DNSIP);
		else LogOutputPrint("gethostbyname not get ip,dns:%s\n",temp);
	}

	return bRet;
}
