#pragma once

#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <string>
#include <vector>

#define MAX_DOMAINNAME_LEN  255
#define DNS_PORT            53
#define DNS_TYPE_SIZE       2
#define DNS_CLASS_SIZE      2
#define DNS_TTL_SIZE        4
#define DNS_DATALEN_SIZE    2
#define DNS_TYPE_A          0x0001 //1 a host address
#define DNS_TYPE_CNAME      0x0005 //5 the canonical name for an alias
#define DNS_PACKET_MAX_SIZE (sizeof(DNSHeader) + MAX_DOMAINNAME_LEN + DNS_TYPE_SIZE + DNS_CLASS_SIZE)

struct DNSHeader
{
    USHORT usTransID; //��ʶ��
    USHORT usFlags; //���ֱ�־λ
    USHORT usQuestionCount; //Question�ֶθ��� 
    USHORT usAnswerCount; //Answer�ֶθ���
    USHORT usAuthorityCount; //Authority�ֶθ���
    USHORT usAdditionalCount; //Additional�ֶθ���
};

class CDNSParser
{
public:
    CDNSParser();
    ~CDNSParser();

    BOOL DNSParser(ULONG ulDNSServerIP, char *szDomainName, std::vector<ULONG> *pveculIPList = NULL, std::vector<std::string> *pvecstrCNameList = NULL, ULONG ulTimeout = 1000, ULONG *pulTimeSpent = NULL);
    BOOL DNSParser(ULONG ulDNSServerIP, char *szDomainName, std::vector<std::string> *pvecstrIPList = NULL, std::vector<std::string> *pvecstrCNameList = NULL, ULONG ulTimeout = 1000, ULONG *pulTimeSpent = NULL);
	void SetLocalAddr(unsigned short SocketPort,void* SocketIPAS);

private:
    BOOL Init();
	sockaddr_in    m_struSockLocalAddr;       //local socket address
    BOOL UnInit();
    BOOL DNSParserCore(ULONG ulDNSServerIP, char *szDomainName, std::vector<ULONG> *pveculIPList, std::vector<std::string> *pvecstrCNameList, ULONG ulTimeout, ULONG *pulTimeSpent);
    BOOL SendDNSRequest(sockaddr_in sockAddrDNSServer, char *szDomainName);
    BOOL RecvDNSResponse(sockaddr_in sockAddrDNSServer, ULONG ulTimeout, std::vector<ULONG> *pveculIPList, std::vector<std::string> *pvecstrCNameList, ULONG *pulTimeSpent);
    BOOL EncodeDotStr(char *szDotStr, char *szEncodedStr, USHORT nEncodedStrSize);
    BOOL DecodeDotStr(char *szEncodedStr, USHORT *pusEncodedStrLen, char *szDotStr, USHORT nDotStrSize, char *szPacketStartPos = NULL);
    ULONG GetTickCountCalibrate();

private:
    BOOL m_bIsInitOK;
    SOCKET m_sock;
    WSAEVENT m_event;
    USHORT m_usCurrentProcID;
    char *m_szDNSPacket;
};

//not support more netcard
int DNSParserTest(char * szDomainName,const char * szDNSServ,int nMaxTimeOut);
void LogOutputPrint(char *szFormat, ...);
//support more netcard
bool GetDNSNameIP(char * strDNS,char * DNSIP,int & nLen);
//not support more netcard
bool GetDNSNameIP2(const char * strDNS,char * DNSIP,int & nLen);
