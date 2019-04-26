//AVCONCall.h
#ifndef __AVCONCALL_H__
#define __AVCONCALL_H__

#include <string>
#include <map>
#include <NETEC/NETEC_Core.h>
#include <NETEC/NETEC_Node.h>
#include <IMXEC/IMXEC_Call.h>
#include <IMXEC/IMXEC_IMX.h>
#include <IMXEC/IMXEC_AVCONCall.h>
#include "../IMXEC_Util/XBufferPool.h"


class AVCONCall
	: public NETEC_Node
	, public IMXEC_Call
	, public XBufferPool
{
public:
	AVCONCall(void);
	virtual~AVCONCall(void);
public:
	static AVCONCall&Instance(void);
public:
	virtual int Login(const char*cszUserID,const char*cszPassword,const char*cszServerIP,unsigned short nServerPort);
	virtual void Logout(void);
	virtual int MakeCall(const char*cszUserID);
	virtual int AcceptCall(const char*cszUserID);
	virtual void EndCall(const char*cszUserID);

	virtual int SetCallLocalWnd(const char*cszUserID,HWND hWnd);
	virtual int SetCallLocalWndPos(const char*cszUserID,int x,int y,int cx,int cy);
	virtual int SetCallRemoteWnd(const char*cszUserID,HWND hWnd);
	virtual int SetCallRemoteWndPos(const char*cszUserID,int x,int y,int cx,int cy);

	virtual void AddAVCONCall(IMXEC_AVCONCall*pAVCONCall);
	virtual void RemoveAVCONCall(IMXEC_AVCONCall*pAVCONCall);

	void OnTimer(unsigned int nTimerID);
protected:
	void SendLogin(void);
	void SendLogout(void);
	void SendSubscribe(const std::string&strUserID);

	/******************************************************************************
	* OnNETEC_NodeConnectStatusChanged
	*描述：网络连接状态的回调
	*输入：	cs					-网络连接状态
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);

	/******************************************************************************
	* OnNETEC_NodeReceivedFromRouter
	*描述：收到网络路由数据包的回调
	*输入：	cszDomain				-网络路由所在的域
			pData					-数据
			nLen					-数据长度
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_NodeReceivedFromRouter(const char*cszDomain,const char* pData,int nLen);
	
	/******************************************************************************
	* OnNETEC_NodeGetUserInfo
	*描述：获取用户信息的回调
	*输入：	cszUserID				-用户标识
	*输出： szNodeID				-用户的网络节点标识
	*返回值：成功返回0；失败返回-1，表示该用户不在线
	*****************************************************************************/
	virtual int OnNETEC_NodeGetUserInfo(const char*cszUserID,char*szNodeID);

	/******************************************************************************
	* OnIMXEC_CallRingIn
	*描述：呼入事件，有终端呼叫本机的时候系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszUserID		-用户标识或用户IP
			cszUserName		-用户名称
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallRingIn(const char*cszCallID,const char*cszUserID,const char*cszUserName);

	/******************************************************************************
	* OnIMXEC_CallRingOut
	*描述：呼出事件，本机发起呼叫成功后系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszUserID		-用户标识或用户IP
			cszUserName		-用户名称
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallRingOut(const char*cszCallID,const char*cszUserID,const char*cszUserName);

	/******************************************************************************
	* OnIMXEC_CallEstablished
	*描述：呼叫建立事件，当双方建立通话后【本机接受了远端的呼叫请求或远端接受了本机的呼叫请求】系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszUserID		-用户标识或用户IP
			cszUserName		-用户名称
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallEstablished(const char*cszCallID,const char*cszUserID,const char*cszUserName);

	/******************************************************************************
	* OnIMXEC_CallDestroyed
	*描述：呼叫销毁事件，当通话结束后【本机挂断或远端挂断】系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszUserID		-用户标识或用户IP
			cszUserName		-用户名称
			nReason			-结束原因
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallDestroyed(const char*cszCallID,const char*cszUserID,const char*cszUserName,IMXEC_CALL_END_REASON nReason);

	/******************************************************************************
	* OnIMXEC_CallRecvdPTZ
	*描述：接收到PTZ事件，当对方发送云台控制命令时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			nPTZAction		-云台动作
			nSpeed			-动作的速度
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed){};

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStart
	*描述：辅流开始事件，当辅流开始时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStart(const char*cszCallID){};

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStop
	*描述：辅流停止事件，当辅流停止时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStop(const char*cszCallID){};
protected:
	typedef struct tagPENDING_CALL
	{
		std::string		strUserID;
		std::string		strNodeID;
		std::string		strCallID;
		unsigned long	nTimestamp;
	}PENDING_CALL;

	typedef std::map<std::string,PENDING_CALL>	MapPendingCall;

	MapPendingCall		m_MapPendingCall;

	std::string			m_strUserID;
	std::string			m_strPassword;
	std::string			m_strServerIP;
	unsigned short		m_nServerPort;

	typedef enum
	{
		LS_IDLE=0,
		LS_LOGINING,
		LS_LOGINED,
		LS_LOGOUTING,
	}LOGIN_STATUS;

	LOGIN_STATUS		m_nLoginStatus;

	typedef std::map<void*,IMXEC_AVCONCall*>	MapAVCONCall;
	MapAVCONCall		m_MapAVCONCall;

	unsigned long		m_nIDEventTimer;
};

#endif
