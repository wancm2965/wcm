/************************************************************************/
/* HPDVRDefine.h                                                        */
/************************************************************************/
#pragma once

/*�������豸*/
/*��������*/
#define HP3TH_PARAM_CHANNEL			0x0001	/*ͨ������*/
#define HP3TH_PARAM_CHNTITLE		0x0002	/*�������*/
#define HP3TH_PARAM_V_MOTION		0x0003	/*������*/
#define HP3TH_PARAM_V_COVER			0x0004	/*���ڲ���*/
#define HP3TH_PARAM_V_BLOCK			0x0005	/*�ڵ�����*/
#define HP3TH_PARAM_V_LOST			0x0006	/*��ʧ����*/
#define HP3TH_PARAM_PTZCTRL			0x0007	/*��̨����*/

/*����*/
#define HP3CMD_GETPARAM				0x00000000	/*��ȡ����*/
#define HP3CMD_SETPARAM				0x00000001  /*���ò���*/

/*�豸*/
#define HP3CMD_INIT					0x00001000	/*�豸��ʼ��*/		
#define HP3CMD_EXIT					0x00001001	/*�豸����ʼ��*/
#define HP3CMD_STARTLISTEN			0x00001002	/*��ʼ����*/	
#define HP3CMD_STOPLISTEN			0x00001003	/*ֹͣ����*/
#define HP3CMD_SETCALLBACK			0x00001004	/*���ûص�*/
#define HP3CMD_LOGIN				0x00001005	/*��¼�豸*/
#define HP3CMD_LOGOUT				0x00001006	/*ע���豸*/
#define HP3CMD_REBOOT				0x00001007  /*�����豸*/
#define HP3CMD_REMOTECLICKKEY		0x00001008	/*Զ�̰���*/
#define HP3CMD_GETSDKVERSION		0x00001009	/*�豸�汾*/
#define HP3CMD_GETLASTERROR			0x00001010	/*������*/

/*ͨ��*/
#define HP3CMD_REALPLAY				0x00002000	/*ʵʱԤ��*/
#define HP3CMD_STOPPLAY				0x00002001	/*ֹͣԤ��*/	
#define HP3CMD_AUDIODATA			0x00002002	/*��Ƶ����*/	
#define HP3CMD_VIDEODATA			0x00002003	/*��Ƶ����*/
#define HP3CMD_STARTVOICE			0x00002004	/*��ʼ�Խ�*/
#define HP3CMD_STOPVOICE			0x00002005	/*ֹͣ�Խ�*/
#define HP3CMD_VOICEDATA			0x00002006	/*�Խ�����*/
#define HP3CMD_SWITCHSTREAM			0x00002007	/*�л���*/
#define HP3CMD_CAPTUREKEYFRAME		0x00002008	/*�ؼ�֡*/
#define HP3CMD_CAPTUREPICTURE		0x00002009	/*ץ��*/
#define HP3CMD_OPENALARMCHANNEL		0x00002010	/*�򿪱���*/	
#define HP3CMD_CLOSEALARMCHANNEL    0x00002011	/*ֹͣ����*/

/*��Ƶ*/
#define HP3CMD_STARTVIDEOMOTION		0x00003000	/*�������*/
#define HP3CMD_STOPVIDEOMOTION		0x00003001	/*ֹͣ���*/
#define HP3CMD_STARTVIDEOCOVER		0x00003002	/*��������*/
#define HP3CMD_STOPVIDEOCOVER		0x00003003	/*ֹͣ����*/
#define HP3CMD_STARTVIDEOBLOCK		0x00003004	/*�����ڵ�*/
#define HP3CMD_STOPVIDEOBLOCK		0x00003005	/*ֹͣ�ڵ�*/
#define HP3CMD_STARTVIDEOLOST		0x00003006	/*��ⶪʧ*/
#define HP3CMD_STOPVIDEOLOST		0x00003007	/*���Զ�ʧ*/

/*��̨*/
#define HP3CMD_PTZCONTROL			0x00004000	/*��̨����*/

/*״̬*/
#define HP3CMD_STATUSCHANGED		0x00005000	/*״̬�ı�*/





