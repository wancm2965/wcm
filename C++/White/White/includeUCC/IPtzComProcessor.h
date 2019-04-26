#pragma once
// [12/14/2009 lzy]

//����
//���ڴ�0��ʼ��
//��ַ��0��ʼ��
//Ԥ�õ��0��ʼ��

typedef struct tagPTZ_COMMAND_INFO
{
	int nType;/*��̨����*/
	int nComPort;/*COM�ں�*/
	int nSerialNumber;/*��̨�ı��*/
	int nSpeed;/*ת��*/
	int nComand;/*����*/
	int nPriority;/*���ȼ�*/
	int nBaud;/*������*/		
}PTZ_COMMAND_INFO,*PPTZ_COMMAND_INFO;

//���ƴ���֪ͨ�ӿ�
class IPtzProcessorNotify
{
public:
	IPtzProcessorNotify(void){};
	virtual ~IPtzProcessorNotify(void){};

public:
	virtual bool OnOpenCom(int nComPort)=0;
	virtual bool OnCloseCom(int nComPort)=0;

};

//��̨���ڿ��ƽӿ�
class IPtzComProcessor
{
public:
	IPtzComProcessor(){};
	virtual~IPtzComProcessor(){};
public:
	virtual void ReleaseConnections(void)=0;
	//pdata ��̨��Ϣ�ṹ��
	virtual void OnDeliver(char*pData,int nLen)=0;

};

AFX_EXT_CLASS IPtzComProcessor *CreatePtzProcessor(IPtzProcessorNotify *pNotify=NULL);