//NETEC_Router.h
#ifndef __NETEC_ROUTER_H__
#define __NETEC_ROUTER_H__

#include "NETEC.h"
#include "NETEC_Session.h"

//����·��
class NETEC_API NETEC_Router
{
public:
	NETEC_Router(void);
	virtual ~NETEC_Router(void);
public:	
	//�������ط���
	virtual int32_t Start(void);
	//ֹͣ���ط���
	virtual void Stop(void);

	//���ñ�����
	virtual void SetLocalDomain(const char*cszDomain="",NETEC::DOMAIN_TYPE DomainType=NETEC::BROTHER);

	virtual NETEC::DOMAIN_TYPE GetDomainType(void);
	virtual const char*GetDomain(void);

	/******************************************************************************
	* OnNETEC_RouterAgentLogin
	*������Agent��¼�ص�
	*���룺	nAgentID			-Agent���ڲ���ʶ
			cszAgentPassword	-Agent������
			cszAgentVersion		-Agent�İ汾
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_RouterAgentLogin(uint32_t nAgentID,const char*cszAgentPassword,const char* cszAgentVersion){};
	
	/******************************************************************************
	* OnNETEC_RouterAgentLogout
	*������Agent�˳��ص�
	*���룺	nAgentID			-Agent�ı�ʶ
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_RouterAgentLogout(uint32_t nAgentID){};
	
	/******************************************************************************
	* SetAgentLoginSuccessful
	*������֪ͨAgent��¼�ɹ�
	*���룺	nAgentID			-Agent�ı�ʶ
	*����� ��
	*����ֵ�����ط�0Ϊ�ǣ�����Ϊ��
	*****************************************************************************/
	virtual int32_t SetAgentLoginSuccessful(uint32_t nAgentID);
	
	/******************************************************************************
	* SetAgentLoginErrorID
	*������֪ͨAgent����Agent�����ڵ��µ�¼ʧ��
	*���룺	nAgentID			-Agent�ı�ʶ
	*����� ��
	*����ֵ�����ط�0Ϊ�ǣ�����Ϊ��
	*****************************************************************************/
	virtual int32_t SetAgentLoginErrorID(uint32_t nAgentID);
	
	/******************************************************************************
	* SetAgentLoginErrorPassword
	*������֪ͨAgent������������µ�¼ʧ��
	*���룺	nAgentID			-Agent�ı�ʶ
	*����� ��
	*����ֵ�����ط�0Ϊ�ǣ�����Ϊ��
	*****************************************************************************/
	virtual int32_t SetAgentLoginErrorPassword(uint32_t nAgentID);

	virtual int32_t SendDataToAgent(uint32_t nAgentID,const char*pData);

	/******************************************************************************
	* OnNETEC_RouterMCULogin
	*������MCU��¼�ص�
	*���룺	cszMCUID			-MCU�ı�ʶ
			cszPassword			-MCU������
			cszVersion			-MCU�İ汾
			nMCUType			-MCU������
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_RouterMCULogin(const char*cszMCUID,const char*cszMCUPassword,const char*cszMCUVersion,const char*cszUserData,uint32_t nMCUType){};
	
	/******************************************************************************
	* OnNETEC_RouterMCULogout
	*������MCU�˳��ص�
	*���룺	cszMCUID			-MCU�ı�ʶ
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_RouterMCULogout(const char*cszMCUID){};
	
	/******************************************************************************
	* OnNETEC_RouterMCUUpdateIP
	*������MCUIP��ַ���»ص�
	*���룺	cszMCUID			-MCU�ı�ʶ
			cszIP				-MCU��IP
			usPort				-MCU�Ķ˿�
			cszIPType			-MCU��IP����
			nActive				-MCU��IP�Ƿ�
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_RouterMCUUpdateIP(const char*cszMCUID,const char*cszIP,unsigned short usPort,const char*cszIPType,int32_t nActive){};
	
	/******************************************************************************
	* OnNETEC_RouterMCUUserCount
	*������MCU�û����ı�Ļص�
	*���룺	cszMCUID			-MCU�ı�ʶ
			nUserCount			-MCU��ǰ�û���
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_RouterMCUUserCountChanged(const char*cszMCUID,uint32_t nUserCount){};
	
	/******************************************************************************
	* OnNETEC_RouterRecvdDataFromMCU
	*������MCU�û����ı�Ļص�
	*���룺	cszMCUID			-MCU�ı�ʶ
			cszData				-����ָ��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_RouterRecvdDataFromMCU(const char*cszMCUID,const char*cszData){};
	
	/******************************************************************************
	* SetMCULoginSuccessful
	*������֪ͨMCU��¼�ɹ�
	*���룺	cszMCUID			-MCU�ı�ʶ
	*����� ��
	*����ֵ�����ط�0Ϊ�ǣ�����Ϊ��
	*****************************************************************************/
	virtual int32_t SetMCULoginSuccessful(const char*cszMCUID);
	
	/******************************************************************************
	* SetMCULoginErrorID
	*������֪ͨMCU����MCU�����ڵ��µ�¼ʧ��
	*���룺	cszMCUID			-MCU�ı�ʶ
	*����� ��
	*����ֵ�����ط�0Ϊ�ǣ�����Ϊ��
	*****************************************************************************/
	virtual int32_t SetMCULoginErrorID(const char*cszMCUID);
	
	/******************************************************************************
	* SetMCULoginErrorPassword
	*������֪ͨMCU������������µ�¼ʧ��
	*���룺	cszMCUID			-MCU�ı�ʶ
	*����� ��
	*����ֵ�����ط�0Ϊ�ǣ�����Ϊ��
	*****************************************************************************/
	virtual int32_t SetMCULoginErrorPassword(const char*cszMCUID);
	
	/******************************************************************************
	* SetMCULoginErrorUserCode
	*������֪ͨMCU�����û����ݵ��µ�¼ʧ��
	*���룺	cszMCUID			-MCU�ı�ʶ
			cszUserCode			-�û�����
	*����� ��
	*����ֵ�����ط�0Ϊ�ǣ�����Ϊ��
	*****************************************************************************/
	virtual int32_t SetMCULoginErrorUserCode(const char*cszMCUID,const char*cszUserCode);

	//�ӷ������ݰ�������
	virtual int32_t SendToChildDomain(const char*cszDomain,const char* pData,int32_t nLen);
	//�������յ������ݰ��ص�
	virtual void OnNETEC_RouterReceivedFromChildDomain(const char*cszDomain,const char* pData,int32_t nLen){};
	//����Ͽ����ӵĻص�
	virtual void OnNETEC_RouterChildDomainDisconnected(const char*cszDomain){};
	
	//���Ӹ���
	virtual int32_t ConnectToParentDomain(const char*cszParentHost,unsigned short usParentPort,const char*cszInterface);
	//�Ͽ��븸�������
	virtual void DisconnectFromParentDomain(void);
	//�ӷ������ݰ�������
	virtual int32_t SendToParentDomain(const char* pData,int32_t nLen);
	//��Domain�յ������ݰ��ص�
	virtual void OnNETEC_RouterReceivedFromParentDomain(const char*cszParentDomain,const char* pData,int32_t nLen){};
	//�յ��ֵ��������״̬
	virtual void OnNETEC_RouterParentDomainSessionConnectStatus(NETEC_Session::CONNECT_STATUS cs,int32_t nErrorCode=0){};

	//�������ӶϿ��Ļص�
	virtual void OnNETEC_RouterBrotherDomainDisconnected(const char*cszBrotherDomain){};

	//�����ֵ���
	//����ֵ���ֻ�и����������ֵ���
	virtual int32_t AddBrotherDomain(const char*cszBrotherDomain,const char*cszBrotherHost,unsigned short usBrotherPort,const char*cszInterface);
	//ɾ���ֵ���
	virtual int32_t RemoveBrotherDomain(const char*cszBrotherDomain);
	//�������ݸ��ֵ���
	virtual int32_t SendToBrotherDomain(const char*cszBrotherDomain,const char*pData,int32_t nLen);
	//���ֵ����յ������ݰ��ص�
	virtual void OnNETEC_RouterReceivedFromBrotherDomain(const char*cszBrotherDomain,const char* pData,int32_t nLen){};
	//�յ��ֵ��������״̬
	virtual void OnNETEC_RouterBrotherDomainSessionConnectStatus(const char*cszBrotherDomain,NETEC_Session::CONNECT_STATUS cs,int32_t nErrorCode=0){};

	//������ڵ��յ������ݰ��ص�
	virtual void OnNETEC_RouterReceivedFromNode(const char*cszDomain,const char*cszNodeID,const char* pData,int32_t nLen){};
	//����ڵ��뱾����Ӧ�öϿ����ӵĻص�
	virtual void OnNETEC_RouterNodeDisconnected(const char*cszDomain,const char*cszNodeID,const char*pData,int32_t nLen){};

	/******************************************************************************
	* SendDataToNode
	*�������������ݸ�ָ��������ڵ�
	*���룺	cszNodeID			-����ڵ��ʶ
			pData				-Ҫ�͵�����
			nLen				-Ҫ�͵����ݵĳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1����ʾ����δ��ͨ
	*****************************************************************************/
	virtual int32_t SendDataToNode(const char*cszNodeID,const char*pData,int32_t nLen);

	//����һ��·���յ������ݰ��ص�
	virtual void OnNETEC_RouterReceivedFromRouter(const char*cszDomain,const char* pData,int32_t nLen){};
	//��һ��·���뱾����Ӧ�öϿ����ӵĻص�
	virtual void OnNETEC_RouterRouterDisconnected(const char*cszDomain,const char*pData,int32_t nLen){};

	/******************************************************************************
	* SendDataToRouter
	*�������������ݸ�ָ����·��
	*���룺	cszDomain			-·�ɵ�����
			pData				-Ҫ�͵�����
			nLen				-Ҫ�͵����ݵĳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1����ʾ����δ��ͨ
	*****************************************************************************/
	virtual int32_t SendDataToRouter(const char*cszDomain,const char*pData,int32_t nLen);
	
	/******************************************************************************
	* GetMCULocalNodeID
	*��������ȡMCU���ص�NodeID
	*���룺	cszMCUID			-MCU�ı�ʶ
	*����� ��
	*����ֵ���ɹ�����MCU���ص�NodeID�����򷵻�NULL
	*****************************************************************************/
	virtual const char* GetMCULocalNodeID(const char*cszMCUID);
};

#endif
