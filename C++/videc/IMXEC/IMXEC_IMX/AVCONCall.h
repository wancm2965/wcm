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
	*��������������״̬�Ļص�
	*���룺	cs					-��������״̬
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);

	/******************************************************************************
	* OnNETEC_NodeReceivedFromRouter
	*�������յ�����·�����ݰ��Ļص�
	*���룺	cszDomain				-����·�����ڵ���
			pData					-����
			nLen					-���ݳ���
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_NodeReceivedFromRouter(const char*cszDomain,const char* pData,int nLen);
	
	/******************************************************************************
	* OnNETEC_NodeGetUserInfo
	*��������ȡ�û���Ϣ�Ļص�
	*���룺	cszUserID				-�û���ʶ
	*����� szNodeID				-�û�������ڵ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1����ʾ���û�������
	*****************************************************************************/
	virtual int OnNETEC_NodeGetUserInfo(const char*cszUserID,char*szNodeID);

	/******************************************************************************
	* OnIMXEC_CallRingIn
	*�����������¼������ն˺��б�����ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRingIn(const char*cszCallID,const char*cszUserID,const char*cszUserName);

	/******************************************************************************
	* OnIMXEC_CallRingOut
	*�����������¼�������������гɹ���ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRingOut(const char*cszCallID,const char*cszUserID,const char*cszUserName);

	/******************************************************************************
	* OnIMXEC_CallEstablished
	*���������н����¼�����˫������ͨ���󡾱���������Զ�˵ĺ��������Զ�˽����˱����ĺ�������ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallEstablished(const char*cszCallID,const char*cszUserID,const char*cszUserName);

	/******************************************************************************
	* OnIMXEC_CallDestroyed
	*���������������¼�����ͨ�������󡾱����Ҷϻ�Զ�˹Ҷϡ�ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
			nReason			-����ԭ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallDestroyed(const char*cszCallID,const char*cszUserID,const char*cszUserName,IMXEC_CALL_END_REASON nReason);

	/******************************************************************************
	* OnIMXEC_CallRecvdPTZ
	*���������յ�PTZ�¼������Է�������̨��������ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed){};

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStart
	*������������ʼ�¼�����������ʼʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStart(const char*cszCallID){};

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStop
	*����������ֹͣ�¼���������ֹͣʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ����
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
