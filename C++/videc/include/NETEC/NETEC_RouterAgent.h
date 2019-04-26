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
	virtual void OnNETEC_RouterAgentNodeDisconnected(const char*cszDomain,const char*cszNodeID,const char*pData,int32_t nLen){};

	//����������Ӧ���յ������ݰ��ص�
	virtual void OnNETEC_RouterAgentReceivedFromAgent(const char*cszDomain,uint32_t nAgentID,const char* pData,int32_t nLen){};
	
	//������ڵ��յ������ݰ��ص�
	virtual void OnNETEC_RouterAgentReceivedFromNode(const char*cszDomain,const char*cszNodeID,const char* pData,int32_t nLen){};

	//ָ����Agent������
	virtual void OnNETEC_RouterAgentAgentDisconnected(const char*cszDomain,uint32_t nAgentID,const char* pData,int32_t nLen){};

	//ָ����Router������
	virtual void OnNETEC_RouterAgentRouterDisconnected(const char*cszDomain){};

	//������ڵ��յ������ݰ��ص�
	virtual void OnNETEC_RouterAgentReceivedFromRouter(const char* pData,int32_t nLen){};

	//ָ����MCU������
	virtual void OnNETEC_RouterAgentMCUDisconnected(const char*cszMCUID){};

	//����������Ӧ���յ������ݰ��ص�
	virtual void OnNETEC_RouterAgentCastFromAgent(const char*cszDomain,uint32_t nAgentID,const char* pData,int32_t nLen){};
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
	virtual int32_t SendDataToAgent(const char*cszDomain,uint32_t nAgentID,const char* pData,int32_t nLen)=0;

	/******************************************************************************
	* CastDataToAgent
	*�������㲥���ݸ�ָ��������Ӧ��
	*���룺	nAgentID			-����Ӧ�ñ�ʶ
			pData				-Ҫ�͵�����
			nLen				-Ҫ�͵����ݵĳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1����ʾ����δ��ͨ
	*****************************************************************************/
	virtual int32_t CastDataToAgent(const char*cszToDomain,uint32_t nAgentID,const char* pData,int32_t nLen)=0;

	/******************************************************************************
	* SendDataToNode
	*�������������ݸ�ָ��������ڵ�
	*���룺	cszNodeID			-����ڵ��ʶ
			pData				-Ҫ�͵�����
			nLen				-Ҫ�͵����ݵĳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1����ʾ����δ��ͨ
	*****************************************************************************/
	virtual int32_t SendDataToNode(const char*cszNodeID,const char*pData,int32_t nLen)=0;

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
	virtual uint32_t GetAgentID(void)=0;

	static NETEC_RouterAgent*Create(NETEC_RouterAgentCallback&rCallback,uint32_t nAgentID);

	/******************************************************************************
	* SendDataToMCU
	*�������������ݸ�ָ����MCU
	*���룺	cszMCUID			-MCU��ʶ
			pData				-Ҫ�͵�����
			nLen				-Ҫ�͵����ݵĳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1����ʾ����δ��ͨ
	*****************************************************************************/
	virtual int32_t SendDataToMCU(const char*cszMCUID,const char*pData,int32_t nLen)=0;

};

#endif