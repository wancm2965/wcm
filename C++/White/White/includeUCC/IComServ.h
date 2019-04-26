#pragma once


//���ڷ�����

#include <string>
using namespace std; 

//�豸����
#define		DEV_TYPE_PTZ						0			//��̨�����
#define		DEV_TYPE_ALARM						1			//����
#define		DEV_TYPE_KEYBOARD					2			//�๦�ܼ���

// ͨ�Żص��ӿ�
class ICommunicateNotify
{
public:
	ICommunicateNotify(){};
	virtual ~ICommunicateNotify(){};

public:
	//virtual void ReciveKXMLPacket(KXMLPacket&t) = 0;		//����ͨ�Ű�

};

//������̨������
class IAVVideoControlCommandProcessor
{
public:
	IAVVideoControlCommandProcessor(){};
	virtual~IAVVideoControlCommandProcessor(){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual void OnDeliver(char*pData,int nLen)=0;

};

// ����IO�����ص��ӿ�
class IAlarmIONotify
{
public:
	IAlarmIONotify(){};
	virtual ~IAlarmIONotify(){};

public:
	//----------------------------------
	//�������ܣ�֪ͨ���������Ϣ
	//����˵����nComPort:���ں� DefenceNo:������ Alarm: true ���� false ֹͣ����
	//����ֵ  ����
	//----------------------------------
	virtual void OnIONotify_InputAlarm(int nComPort,int DefenceNo,  bool Alarm) = 0;

};

//ȫ���ܼ��̻ص��ӿ�
class IKeyboardNotify
{

public:
	IKeyboardNotify(void){};
	virtual ~IKeyboardNotify(void){};

public:
	//----------------------------------
	//�������ܣ���&���� һ��ͨ���󶨵�һ����Ļ 
	//����˵����Monitor:��Ļ Camera:ͨ�������
	//����ֵ  ����
	//----------------------------------
	virtual bool KeyboardBind(int Monitor, int Camera)=0;

	//----------------------------------
	//�������ܣ�������̨���� 
	//����˵����cmd:��̨�������� Param: -1:ֹͣ  ����:����
	//����ֵ  ����
	//----------------------------------

	virtual void KeyboardPtzCtrl(int Cmd, int Param)=0;	

};

//��̨����֪ͨ��Ϣ��
class IPtzCtrlNotify
{

public:
	IPtzCtrlNotify(void){};
	virtual ~IPtzCtrlNotify(void){};

public:
	//----------------------------------
	//�������ܣ�����ָ���Ĵ��ڣ������ݿ��ж�ȡ���ڲ���
	//����˵����nComPort:���ں�
	//����ֵ  ����
	//----------------------------------
	virtual bool OnOpenCom(int nComPort)=0;

	//----------------------------------
	//�������ܣ��ر�ָ���Ĵ��ڣ����浱ǰ���ڲ����������ݿ��ж�ȡ 
	//����˵����nComPort:���ں�
	//����ֵ  ����
	//----------------------------------
	virtual int OnCloseCom(int nComPort)=0;

};


//�ⲿ�ӿ���
class IComServ
{


public:
	IComServ(void){};
	virtual ~IComServ(void){};

	//�ӿ��ڲ�����
public:
	//----------------------------------
	//�������ܣ����ô��ڲ���	���浽���ݿ�
	//����˵����nComport:�˿ں� nBaud:������ nProve:У��λ nDataBit;����λ nStopBit:ֹͣλ
	//			nType:�豸����   �˴���ֻ�ܽ�һ�������豸���磺�������� �๦�ܼ���
	//����ֵ  ����
	//----------------------------------
	virtual void	SetComParam(int nComport,int nBaud,int nProve,int nDataBit,int nStopBit,int nType)=0;	

	//----------------------------------
	//�������ܣ�������������
	//����˵����nComport:�˿ں� Ĭ��ֵ��1�Ŵ���   pAlarmNotify: �����ӿڻص�
	//����ֵ  ��true:�򿪳ɹ�	false:��ʧ��
	//----------------------------------
	virtual bool	StartAlarmIo(int nComport,IAlarmIONotify* pAlarmNotify)=0;
	virtual int		StopAlarmIo(int nComport)=0;


	//----------------------------------
	//�������ܣ�����ȫ���ܼ��̴���
	//����˵����nComport:�˿ں� Ĭ��ֵ��2�Ŵ���   
	//����ֵ  ��true:�򿪳ɹ�	false:��ʧ��
	//----------------------------------
	virtual bool	StartKeyboardIo(int nComport,IKeyboardNotify* pKeyboardNotify)=0;
	virtual int		StopKeyboardIo(int nComport)=0;



	//----------------------------------
	//�������ܣ�����ȫ���ܼ��̽ӿڻص�
	//����˵����pPtzCtrlNotify:��̨����֪ͨ�ص�
	//����ֵ  ����
	//----------------------------------
//	virtual void	SetPtzControlNotify(IPtzCtrlNotify* pPtzCtrlNotify)=0;


	//----------------------------------
	//�������ܣ���̨����
	//����˵����nComport:�˿ں�		CameraAddress:�������ַ0-255	nComand:����ֵ 
	//			nPriority:���ȼ�	nBaud:������		nType:��̨����	nSpeed:ת�� 
	//����ֵ  ����
	//----------------------------------
//	virtual void	PtzControl(int nComPort,int CameraAddress,int nSpeed, int nBaud,int nComand)=0;

	virtual void	PtzControl(int nComPort,int CameraAddress,int nComand,int nPriority,int nType,int nSpeed, int nBaud)=0;
	

	virtual void	Release()=0;		//ɾ�������Ĵ��ڷ���ӿ�ָ��

};


AFX_EXT_CLASS IComServ* CreateComServ();		//������ƽ���ڷ���ӿ�
