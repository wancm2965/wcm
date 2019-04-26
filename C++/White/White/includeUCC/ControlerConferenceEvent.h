



#ifndef _CONTROLERCONFERENCEEVENT_H 
#define _CONTROLERCONFERENCEEVENT_H 

#include <string>
using namespace std;
//#include "../CONTROLERHOOK/ontrolerConferenceEventChild.h"

class IControlerConference
{
public:
	IControlerConference() {}
	virtual ~IControlerConference(void) {};
	virtual void Release() = 0;
	virtual void EnableKeyBorad(BOOL bEnable) = 0;
};

class IControlerConferenceEvent
{
public:
	IControlerConferenceEvent(){}; 
	virtual ~IControlerConferenceEvent(void){};
	//�Ƴ�����
	virtual void OnControlQuitConference() = 0;
	//ģ�����
	virtual void OnControlTemplateAdjust() = 0;
	//��Ƶ����
	virtual void OnControlVideoPreset() = 0;
	//�㲥��Ƶ
	virtual void OnControlVideoBroadcast() = 0;
	//����װ�
	virtual void OnControlWhiteBroad() = 0;
	//��Ƶ����
	virtual void OnControlVideoShare() = 0;
	//������Ļ
	virtual void OnControlScreen() = 0;
	//��������
	virtual void OnControlVideoDiscussion() = 0;
	//¼������
	virtual void OnControlRecordAll() = 0;
	//¼������/ֹͣ
	virtual void OnControlRecordAllOrStop() = 0;
	//ѡ������
	virtual void OnControlSelectMainFrame(int iWin) = 0;
	//����̨��
	virtual void OnPTZing() = 0;
	//ѡ������
	virtual void OnControlOptionSet() = 0;
	//��F1�л�������Ƶͨ��
	virtual void OnControlChangeLocalChannel() = 0;
	//��F2�л�������ƵԴ
	virtual void OnControlChangeLocalResource() = 0;
	//��ESC���ر�����Ƶ
	virtual void OnControlHideLocalAV() = 0;
	//��ʾģ�������
	virtual void OnShowTemplateControl() = 0;

	//AVCON����/��ʾ
	virtual void OnAvconHideShow() = 0;

	//��ʾ��̨����
	virtual void OnPTZControl() = 0;
	//��ʾ��������
	virtual void OnShowFloatWindow() = 0;
	//������Ƶ����
	virtual void OnLocalVideoWindow() = 0;
	//��˷羲��
	virtual void OnMuteMicrophone() = 0;
	//����
	virtual void OnMuteSpeaker() = 0;
	//�����
	virtual void OnControlDirect(DWORD dDirect) =0;
};

//����ʵ��
AFX_EXT_CLASS IControlerConference* CreateControlConference(IControlerConferenceEvent *pControlerConferenceEvent);


#endif

