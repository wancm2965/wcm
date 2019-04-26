#pragma once
#include <NETEC/NETEC_Node.h>
#ifdef _DEBUG
#pragma comment(lib,"../lib/Debug/NETEC.lib")
#else
#pragma comment(lib,"../lib/release/NETEC.lib")
#endif

class CWBCameraNetMng : public NETEC_Node
{
public:
	CWBCameraNetMng();
	virtual ~CWBCameraNetMng();
protected:
	/******************************************************************************
	OnNETEC_NodeConnectStatusChanged
	描述：网络连接状态的回调
	*****************************************************************************/
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);
	//如子类需要网络监测，则可以重写如下虚函数
	virtual void OnConnectStatusChanged(BOOL bConnected){};
protected:
	//
	BOOL GetNetState();
	BOOL Connect();
	void DisConnect();

private://私有，不能被子类直接使用
	//计数器和网络连接状态标识
	static unsigned short m_nCounter;
	static BOOL m_bNetConnected;
};