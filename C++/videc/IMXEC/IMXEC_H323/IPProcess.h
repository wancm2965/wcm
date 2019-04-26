
/*
* 处理IP地址 取所有IP，取IP类型
* IP地址划分:
* A类地址：1.0.0.1～126.255.255.254
* B类地址：128.0.0.1～191.255.255.254
* C类地址：192.168.0.0～223.255.255.255
* D类地址：224.0.0.1～239.255.255.254
* E类地址：240.0.0.1～255.255.255.254
* 如何判断两个IP地址是否是同一个网段中:
* 要判断两个IP地址是不是在同一个网段，就将它们的IP地址分别与子网掩码做与运算，得到的结果一网络号，如果网络号相同，就在同一子网，否则，不在同一子网。
* 如果没有进行子网划分，A类网络的子网掩码为255.0.0.0，B类网络的子网掩码为255.255.0.0，C类网络的子网掩码为255.255.255.0，缺省情况子网掩码为255.255.255.0
*/

#ifndef __IPPROCESS_H__
#define __IPPROCESS_H__

#include <string>

#define NUM_MAX_NETCARD 6	         //配置的最大网卡数
#define MaxIPCount      16   
#define MaxRouteCount      32   

typedef struct tagNetCardInfo
{
	std::string		m_strLocalIP;
	std::string		m_strSubnetMask; //子网掩码
	std::string		m_strGateway;    //网关
	std::string		m_strDnsServer;  //DNS服务器
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


//返回IP地址的整数类型值
unsigned long getIpV4Value(std::string ipV4);

//查看是否为合法IP,排除掉全数字或全为首端带有0x及0X的十六进制的数字
bool ipV4Validate(std::string strIP);

//比较两个ip地址是否在同一个网段中，如果两个都是合法地址，两个都是非法地址时，可以正常比较
//如果有其一不是合法地址则返回false
bool checkSameSegment(std::string& ip1,std::string& ip2, unsigned long mask);

//取一个与目的IP类型相同的本地IP 调GetAdaptersInfo
bool GetOneSameDestIP(std::string & strLocalIP,std::string & strDestIP);

//取一个与目的IP类型相同的本地IP 调gethostbyname
bool  GetOneSameDestIP2(std::string & strLocalIP,std::string & strDestIP);

//从取到的所有IP中取一个与目的IP类型相同的本地IP
bool GetOneSameDestIP3(IPItemSave & ipSave,std::string & strLocalIP,std::string & strDestIP);

//取所有IP 调gethostbyname 保存到ipSave
bool GetLocalAllIP(IPItemSave & ipSave);

//取所有IP 保存到ipSave 调socket 可以取到127
bool HostWindowsGetIpV4List(IPItemSave & ipSave);


//检测ipV4 的类型，包括A类，B类，C类，其它（C,D和广播）类等，返回类型IPADDRTYPE值
int checkIpV4Type(std::string & ipV4);

//检测ipV4 的是不是私有IP地址 私有返回true
bool checkIpV4Private(std::string & ipV4);

//调GetAdaptersInfo 取一个IP
bool GetCurNetCardsInfo(NetCardInfo& tNetCardInfo, int& byCrdIdx);

bool GetIPTokenNum(std::string strIP,int ipToken[4]);


#endif //__IPPROCESS_H__
