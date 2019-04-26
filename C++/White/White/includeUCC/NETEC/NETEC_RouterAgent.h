//NETEC_RouterAgent.h
#ifndef __NETEC_ROUTERAGENT_H__
#define __NETEC_ROUTERAGENT_H__

#include <NETEC/NETEC_Export.h>

//����Ӧ�÷���
class NETEC_RouterAgentCallback
{
public:
	NETEC_RouterAgentCallback(void){};
	virtual~NETEC_RouterAgentCallback(void){};
public:
	//����ڵ��뱾����Ӧ�öϿ����ӵĻص�
	virtual void OnNETEC_RouterAgentNodeDisconnected(const char*cszDomain,const char*cszNodeID,const char*pData,int nLen){};

	//����������Ӧ���յ������ݰ��ص�
	virtual void OnNETEC_RouterAgentReceivedFromAgent(const char*cszDomain,unsigned int nAgentID,const char* pData,int nLen){};
	
	//������ڵ��յ������ݰ��ص�
	virtual void OnNETEC_RouterAgentReceivedFromNode(const char*cszDomain,const char*cszNodeID,const char* pData,int nLen){};

	//ָ����Agent������
	virtual void OnNETEC_RouterAgentAgentDisconnected(const char*cszDomain,unsigned int nAgentID,const char* pData,int nLen){};

	//ָ����Router������
	virtual void OnNETEC_RouterAgentRouterDisconnected(const char*cszDomain){};

	//������ڵ��յ������ݰ��ص�
	virtual void OnNETEC_RouterAgentReceivedFromRouter(const char* pData,int nLen){};

	//ָ����MCU������
	virtual void OnNETEC_RouterAgentMCUDisconnected(const char*cszMCUID){};

	//����������Ӧ���յ������ݰ��ص�
	virtual void OnNETEC_RouterAgentCastFromAgent(const char*cszDomain,unsigned int nAgentID,const char* pData,int nLen){};
};

class NETEC_API NETEC_RouterAgent
{
public:
	NETEC_RouterAgent(void){};
	virtual~NETEC_RouterAgent(void){};
public:
	/******************************************************************************
	* Close
	*�������ر�
	*���룺	��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void Close(void)=0;

	/******************************************************************************
	* SendDataToAgent
	*�������������ݸ�ָ��������Ӧ��
	*���룺	cszDomain			-����Ӧ�����ڵ���
			nAgentID			-����Ӧ�ñ�ʶ
			pData				-Ҫ�͵�����
			nLen				-Ҫ�͵����ݵĳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1����ʾ����δ��ͨ
	*****************************************************************************/
	virtual int SendDataToAgent(const char*cszDomain,unsigned int nAgentID,const char* pData,int nLen)=0;

	/******************************************************************************
	* CastDataToAgent
	*�������㲥���ݸ�ָ��������Ӧ��
	*���룺	nAgentID			-����Ӧ�ñ�ʶ
			pData				-Ҫ�͵�����
			nLen				-Ҫ�͵����ݵĳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1����ʾ����δ��ͨ
	*****************************************************************************/
	virtual int CastDataToAgent(const char*cszToDomain,unsigned int nAgentID,const char* pData,int nLen)=0;

	/******************************************************************************
	* SendDataToNode
	*�������������ݸ�ָ��������ڵ�
	*���룺	cszNodeID			-����ڵ��ʶ
			pData				-Ҫ�͵�����
			nLen				-Ҫ�͵����ݵĳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1����ʾ����δ��ͨ
	*****************************************************************************/
	virtual int SendDataToNode(const char*cszNodeID,const char*pData,int nLen)=0;

	/******************************************************************************
	* GetDomain
	*��������ȡ���ڵ���
	*���룺	��
	*����� ��
	*����ֵ�����ڵ���
	*****************************************************************************/
	virtual const char*GetDomain(void)=0;

	/******************************************************************************
	* GetAgentID
	*��������ȡ����Ӧ�ñ�ʶ
	*���룺	��
	*����� ��
	*����ֵ������Ӧ�ñ�ʶ
	*****************************************************************************/
	virtual unsigned int GetAgentID(void)=0;

	static NETEC_RouterAgent*Create(NETEC_RouterAgentCallback&rCallback,unsigned int nAgentID);

	/******************************************************************************
	* SendDataToMCU
	*�������������ݸ�ָ����MCU
	*���룺	cszMCUID			-MCU��ʶ
			pData				-Ҫ�͵�����
			nLen				-Ҫ�͵����ݵĳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1����ʾ����δ��ͨ
	*****************************************************************************/
	virtual int SendDataToMCU(const char*cszMCUID,const char*pData,int nLen)=0;

};

#endif