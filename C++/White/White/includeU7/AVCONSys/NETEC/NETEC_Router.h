//NETEC_Router.h
#ifndef __NETEC_ROUTER_H__
#define __NETEC_ROUTER_H__

#include "NETEC.h"
#include "NETEC_Session.h"

//网络路由
class NETEC_API NETEC_Router
{
public:
	NETEC_Router(void);
	virtual ~NETEC_Router(void);
public:	
	//启动本地服务
	virtual int32_t Start(void);
	//停止本地服务
	virtual void Stop(void);

	//设置本地域
	virtual void SetLocalDomain(const char*cszDomain="",NETEC::DOMAIN_TYPE DomainType=NETEC::BROTHER);

	virtual NETEC::DOMAIN_TYPE GetDomainType(void);
	virtual const char*GetDomain(void);

	/******************************************************************************
	* OnNETEC_RouterAgentLogin
	*描述：Agent登录回调
	*输入：	nAgentID			-Agent的内部标识
			cszAgentPassword	-Agent的密码
			cszAgentVersion		-Agent的版本
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_RouterAgentLogin(uint32_t nAgentID,const char*cszAgentPassword,const char* cszAgentVersion){};
	
	/******************************************************************************
	* OnNETEC_RouterAgentLogout
	*描述：Agent退出回调
	*输入：	nAgentID			-Agent的标识
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_RouterAgentLogout(uint32_t nAgentID){};
	
	/******************************************************************************
	* SetAgentLoginSuccessful
	*描述：通知Agent登录成功
	*输入：	nAgentID			-Agent的标识
	*输出： 无
	*返回值：返回非0为是，否则为否
	*****************************************************************************/
	virtual int32_t SetAgentLoginSuccessful(uint32_t nAgentID);
	
	/******************************************************************************
	* SetAgentLoginErrorID
	*描述：通知Agent由于Agent不存在导致登录失败
	*输入：	nAgentID			-Agent的标识
	*输出： 无
	*返回值：返回非0为是，否则为否
	*****************************************************************************/
	virtual int32_t SetAgentLoginErrorID(uint32_t nAgentID);
	
	/******************************************************************************
	* SetAgentLoginErrorPassword
	*描述：通知Agent由于密码错误导致登录失败
	*输入：	nAgentID			-Agent的标识
	*输出： 无
	*返回值：返回非0为是，否则为否
	*****************************************************************************/
	virtual int32_t SetAgentLoginErrorPassword(uint32_t nAgentID);

	virtual int32_t SendDataToAgent(uint32_t nAgentID,const char*pData);

	/******************************************************************************
	* OnNETEC_RouterMCULogin
	*描述：MCU登录回调
	*输入：	cszMCUID			-MCU的标识
			cszPassword			-MCU的密码
			cszVersion			-MCU的版本
			nMCUType			-MCU的类型
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_RouterMCULogin(const char*cszMCUID,const char*cszMCUPassword,const char*cszMCUVersion,const char*cszUserData,uint32_t nMCUType){};
	
	/******************************************************************************
	* OnNETEC_RouterMCULogout
	*描述：MCU退出回调
	*输入：	cszMCUID			-MCU的标识
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_RouterMCULogout(const char*cszMCUID){};
	
	/******************************************************************************
	* OnNETEC_RouterMCUUpdateIP
	*描述：MCUIP地址更新回调
	*输入：	cszMCUID			-MCU的标识
			cszIP				-MCU的IP
			usPort				-MCU的端口
			cszIPType			-MCU的IP类型
			nActive				-MCU的IP是否活动
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_RouterMCUUpdateIP(const char*cszMCUID,const char*cszIP,unsigned short usPort,const char*cszIPType,int32_t nActive){};
	
	/******************************************************************************
	* OnNETEC_RouterMCUUserCount
	*描述：MCU用户数改变的回调
	*输入：	cszMCUID			-MCU的标识
			nUserCount			-MCU当前用户数
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_RouterMCUUserCountChanged(const char*cszMCUID,uint32_t nUserCount){};
	
	/******************************************************************************
	* OnNETEC_RouterRecvdDataFromMCU
	*描述：MCU用户数改变的回调
	*输入：	cszMCUID			-MCU的标识
			cszData				-数据指针
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_RouterRecvdDataFromMCU(const char*cszMCUID,const char*cszData){};
	
	/******************************************************************************
	* SetMCULoginSuccessful
	*描述：通知MCU登录成功
	*输入：	cszMCUID			-MCU的标识
	*输出： 无
	*返回值：返回非0为是，否则为否
	*****************************************************************************/
	virtual int32_t SetMCULoginSuccessful(const char*cszMCUID);
	
	/******************************************************************************
	* SetMCULoginErrorID
	*描述：通知MCU由于MCU不存在导致登录失败
	*输入：	cszMCUID			-MCU的标识
	*输出： 无
	*返回值：返回非0为是，否则为否
	*****************************************************************************/
	virtual int32_t SetMCULoginErrorID(const char*cszMCUID);
	
	/******************************************************************************
	* SetMCULoginErrorPassword
	*描述：通知MCU由于密码错误导致登录失败
	*输入：	cszMCUID			-MCU的标识
	*输出： 无
	*返回值：返回非0为是，否则为否
	*****************************************************************************/
	virtual int32_t SetMCULoginErrorPassword(const char*cszMCUID);
	
	/******************************************************************************
	* SetMCULoginErrorUserCode
	*描述：通知MCU由于用户数据导致登录失败
	*输入：	cszMCUID			-MCU的标识
			cszUserCode			-用户代码
	*输出： 无
	*返回值：返回非0为是，否则为否
	*****************************************************************************/
	virtual int32_t SetMCULoginErrorUserCode(const char*cszMCUID,const char*cszUserCode);

	//从发送数据包给子域
	virtual int32_t SendToChildDomain(const char*cszDomain,const char* pData,int32_t nLen);
	//从子域收到的数据包回调
	virtual void OnNETEC_RouterReceivedFromChildDomain(const char*cszDomain,const char* pData,int32_t nLen){};
	//子域断开连接的回调
	virtual void OnNETEC_RouterChildDomainDisconnected(const char*cszDomain){};
	
	//连接父域
	virtual int32_t ConnectToParentDomain(const char*cszParentHost,unsigned short usParentPort,const char*cszInterface);
	//断开与父域的连接
	virtual void DisconnectFromParentDomain(void);
	//从发送数据包给父域
	virtual int32_t SendToParentDomain(const char* pData,int32_t nLen);
	//从Domain收到的数据包回调
	virtual void OnNETEC_RouterReceivedFromParentDomain(const char*cszParentDomain,const char* pData,int32_t nLen){};
	//收到兄弟域的连接状态
	virtual void OnNETEC_RouterParentDomainSessionConnectStatus(NETEC_Session::CONNECT_STATUS cs,int32_t nErrorCode=0){};

	//被动连接断开的回调
	virtual void OnNETEC_RouterBrotherDomainDisconnected(const char*cszBrotherDomain){};

	//连接兄弟域
	//添加兄弟域，只有根域才能添加兄弟域
	virtual int32_t AddBrotherDomain(const char*cszBrotherDomain,const char*cszBrotherHost,unsigned short usBrotherPort,const char*cszInterface);
	//删除兄弟域
	virtual int32_t RemoveBrotherDomain(const char*cszBrotherDomain);
	//发送数据给兄弟域
	virtual int32_t SendToBrotherDomain(const char*cszBrotherDomain,const char*pData,int32_t nLen);
	//从兄弟域收到的数据包回调
	virtual void OnNETEC_RouterReceivedFromBrotherDomain(const char*cszBrotherDomain,const char* pData,int32_t nLen){};
	//收到兄弟域的连接状态
	virtual void OnNETEC_RouterBrotherDomainSessionConnectStatus(const char*cszBrotherDomain,NETEC_Session::CONNECT_STATUS cs,int32_t nErrorCode=0){};

	//从网络节点收到的数据包回调
	virtual void OnNETEC_RouterReceivedFromNode(const char*cszDomain,const char*cszNodeID,const char* pData,int32_t nLen){};
	//网络节点与本网络应用断开连接的回调
	virtual void OnNETEC_RouterNodeDisconnected(const char*cszDomain,const char*cszNodeID,const char*pData,int32_t nLen){};

	/******************************************************************************
	* SendDataToNode
	*描述：发送数据给指定的网络节点
	*输入：	cszNodeID			-网络节点标识
			pData				-要送的数据
			nLen				-要送的数据的长度
	*输出： 无
	*返回值：成功返回0；失败返回-1，表示网络未连通
	*****************************************************************************/
	virtual int32_t SendDataToNode(const char*cszNodeID,const char*pData,int32_t nLen);

	//从另一个路由收到的数据包回调
	virtual void OnNETEC_RouterReceivedFromRouter(const char*cszDomain,const char* pData,int32_t nLen){};
	//另一个路由与本网络应用断开连接的回调
	virtual void OnNETEC_RouterRouterDisconnected(const char*cszDomain,const char*pData,int32_t nLen){};

	/******************************************************************************
	* SendDataToRouter
	*描述：发送数据给指定的路由
	*输入：	cszDomain			-路由的域名
			pData				-要送的数据
			nLen				-要送的数据的长度
	*输出： 无
	*返回值：成功返回0；失败返回-1，表示网络未连通
	*****************************************************************************/
	virtual int32_t SendDataToRouter(const char*cszDomain,const char*pData,int32_t nLen);
	
	/******************************************************************************
	* GetMCULocalNodeID
	*描述：获取MCU本地的NodeID
	*输入：	cszMCUID			-MCU的标识
	*输出： 无
	*返回值：成功返回MCU本地的NodeID，否则返回NULL
	*****************************************************************************/
	virtual const char* GetMCULocalNodeID(const char*cszMCUID);
};

#endif
