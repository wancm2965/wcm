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
	��������������״̬�Ļص�
	*****************************************************************************/
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);
	//��������Ҫ�����⣬�������д�����麯��
	virtual void OnConnectStatusChanged(BOOL bConnected){};
protected:
	//
	BOOL GetNetState();
	BOOL Connect();
	void DisConnect();

private://˽�У����ܱ�����ֱ��ʹ��
	//����������������״̬��ʶ
	static unsigned short m_nCounter;
	static BOOL m_bNetConnected;
};