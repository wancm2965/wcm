
/*
* ����IP��ַ ȡ����IP��ȡIP����
* IP��ַ����:
* A���ַ��1.0.0.1��126.255.255.254
* B���ַ��128.0.0.1��191.255.255.254
* C���ַ��192.168.0.0��223.255.255.255
* D���ַ��224.0.0.1��239.255.255.254
* E���ַ��240.0.0.1��255.255.255.254
* ����ж�����IP��ַ�Ƿ���ͬһ��������:
* Ҫ�ж�����IP��ַ�ǲ�����ͬһ�����Σ��ͽ����ǵ�IP��ַ�ֱ������������������㣬�õ��Ľ��һ����ţ�����������ͬ������ͬһ���������򣬲���ͬһ������
* ���û�н����������֣�A���������������Ϊ255.0.0.0��B���������������Ϊ255.255.0.0��C���������������Ϊ255.255.255.0��ȱʡ�����������Ϊ255.255.255.0
*/

#ifndef __IPPROCESS_H__
#define __IPPROCESS_H__

#include <string>

#define NUM_MAX_NETCARD 6	         //���õ����������
#define MaxIPCount      16   
#define MaxRouteCount      32   

typedef struct tagNetCardInfo
{
	std::string		m_strLocalIP;
	std::string		m_strSubnetMask; //��������
	std::string		m_strGateway;    //����
	std::string		m_strDnsServer;  //DNS������
	std::string		m_strAdapterName;
}NetCardInfo;

typedef enum{
	IPTYPEA=1,
	IPTYPEB,
	IPTYPEC,
	IPTYPED,
	IPTYPEE
}IPADDRTYPE;

struct ipItem
{
	IPADDRTYPE nType;
	unsigned long nIP;
	std::string strIP;
	std::string strMask;
};

typedef struct tagIPItemSave
{
	int	m_nNum;
	ipItem	m_AllIP[MaxIPCount];
	tagIPItemSave()
	{
		m_nNum = 0;
	}
}IPItemSave;

struct routeItem
{
	int nType;         //network segment type
	int nPriority;     //route priority
	std::string strIP; //local ip segment
	std::string strDest; //dest segment 
	std::string strGW;   //local gateway
};

typedef struct tagRouteItemSave
{
	int	m_nNum;
	routeItem	m_AllIP[MaxRouteCount];
	tagRouteItemSave()
	{
		m_nNum = 0;
	}
}RouteItemSave;


//����IP��ַ����������ֵ
unsigned long getIpV4Value(std::string ipV4);

//�鿴�Ƿ�Ϊ�Ϸ�IP,�ų���ȫ���ֻ�ȫΪ�׶˴���0x��0X��ʮ�����Ƶ�����
bool ipV4Validate(std::string strIP);

//�Ƚ�����ip��ַ�Ƿ���ͬһ�������У�����������ǺϷ���ַ���������ǷǷ���ַʱ�����������Ƚ�
//�������һ���ǺϷ���ַ�򷵻�false
bool checkSameSegment(std::string& ip1,std::string& ip2, unsigned long mask);

//ȡһ����Ŀ��IP������ͬ�ı���IP ��GetAdaptersInfo
bool GetOneSameDestIP(std::string & strLocalIP,std::string & strDestIP);

//ȡһ����Ŀ��IP������ͬ�ı���IP ��gethostbyname
bool  GetOneSameDestIP2(std::string & strLocalIP,std::string & strDestIP);

//��ȡ��������IP��ȡһ����Ŀ��IP������ͬ�ı���IP
bool GetOneSameDestIP3(IPItemSave & ipSave,std::string & strLocalIP,std::string & strDestIP);

//ȡ����IP ��gethostbyname ���浽ipSave
bool GetLocalAllIP(IPItemSave & ipSave);

//ȡ����IP ���浽ipSave ��socket ����ȡ��127
bool HostWindowsGetIpV4List(IPItemSave & ipSave);


//���ipV4 �����ͣ�����A�࣬B�࣬C�࣬������C,D�͹㲥����ȣ���������IPADDRTYPEֵ
int checkIpV4Type(std::string & ipV4);

//���ipV4 ���ǲ���˽��IP��ַ ˽�з���true
bool checkIpV4Private(std::string & ipV4);

//��GetAdaptersInfo ȡһ��IP
bool GetCurNetCardsInfo(NetCardInfo& tNetCardInfo, int& byCrdIdx);

bool GetIPTokenNum(std::string strIP,int ipToken[4]);


#endif //__IPPROCESS_H__
