#ifndef __COMECCALL_H__
#define __COMECCALL_H__

#include <string>
using namespace std;
#include "COMEC.h"

typedef struct tagCOMEC_CALL_STATICS
{
	int	nSendAudioBitrate;		//���͵���Ƶ����
	int nSendAudioLostCount;	//���͵���Ƶ������
	int nSendAudioLostPercent;	//���͵���Ƶ������
	int nRecvAudioBitrate;		//���յ���Ƶ����
	int nRecvAudioLostCount;	//���յ���Ƶ������
	int nRecvAudioLostPercent;	//���յ���Ƶ������
}COMEC_CALL_STATICS,*PCOMEC_CALL_STATICS;

class COMEC_API COMECCall
{
public:
	COMECCall(void);
	virtual~COMECCall(void);
public:
	virtual void Call_Attach(void);
	virtual void Call_Detach(void);

	//������������
	virtual int Call_OutGoing(const string&strAccountID,const string&strCallTo,string&strCallID);
	//���ܺ�������
	virtual int Call_Answer(const string&strCallID);
	//�ܾ����������ҶϺ���
	virtual int Call_Hangup(const string&strCallID);

	//ȡ��������
	virtual int Call_GetStatics(const string&strCallID,COMEC_CALL_STATICS&callStatics);

	//���öԷ�������������������
	virtual int Call_SetPeerVolume(const string&strCallID,int nVolume/*0-10-20*/);
	//��ȡ�Է�������������������
	virtual int Call_GetPeerVolume(const string&strCallID,int&nVolume);
	//���öԷ����������ž�����
	virtual int Call_SetPeerMute(const string&strCallID,bool bMute);
	//��ȡ�Է����������ž�����
	virtual int Call_GetPeerMute(const string&strCallID,bool&bMute);
	//��ȡ�Է���ǰ����
	virtual int Call_GetPeerLevel(const string&strCallID,int&nLevel/*0-10*/);

	//���ñ��ؾ������
	virtual int Call_SetLocalVAD(const string&strCallID,bool bVAD);
	//��ȡ���ؾ����������
	virtual int Call_GetLocalVAD(const string&strCallID,bool&bVAD);
	//���ñ��ؾ���
	virtual int Call_SetLocalMute(const string&strCallID,bool bMute);
	//��ȡ���ؾ�������
	virtual int Call_GetLocalMute(const string&strCallID,bool&bMute);
	//��ȡ�����������ɼ�������
	virtual int Call_GetLocalLevel(const string&strCallID,int&nLevel/*0-10*/);
	//����DTMF
	virtual int Call_SendDTMF(const string&strCallID,char dtmf);

	//�绰�����¼�
	virtual void OnCall_Incoming(const string&strAccountID,const string&strCallID,const string&strCallFrom)=0;//�Է�������
	//�绰������Է���������¼�
	virtual void OnCall_Ringing(const string&strCallID)=0;//�Է����壬�ȴ��Է���Ӧ
	//�Է������˺���������¼�
	virtual void OnCall_Answered(const string&strCallID)=0;//�Է�����
	//�Է��Ҷ��˺��У������ܾ������¼�
	virtual void OnCall_Hangup(const string&strCallID)=0;//�Է��Ҷ�
	//���н������¼�
	virtual void OnCall_Established(const string&strCallID)=0;//�Ի�����
	//���н�����ʱ��
	virtual void OnCall_Terminated(const string&strCallID)=0;//�Ի�����
};


#endif