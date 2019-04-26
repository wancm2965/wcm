#pragma once
using namespace std;
#include "KBASE.h"
#include "KSYSClient.h"
#include "StructDefine.h"
#include "KMONDefine.h"
#pragma comment(lib,"KBASE.lib")
#pragma comment(lib,"KSYSClient.lib")
#include "HPDEV_Define.h"

#include <map>
#include <string>
using namespace std;
#define TYPE_PTZDLG		1
#define TYPE_PRESET		2

/************************************************************************/
/* ��̨����                                                             */
/************************************************************************/
#define CMD_UP			101   //����
#define CMD_STOP_UP		201   
#define CMD_DOWN		102   //����
#define CMD_STOP_DOWN   202
#define CMD_LEFT		103	  //����
#define CMD_STOP_LEFT	203	  
#define CMD_RIGHT		104   //����
#define CMD_STOP_RIGHT  204
#define CMD_DN			105   //����
#define CMD_STOP_DN     205
#define CMD_DF			106   //���Զ
#define CMD_STOP_DF     206
#define CMD_FS			107   //�佹С
#define CMD_STOP_FS		207
#define CMD_FB			108   //�佹��
#define CMD_STOP_FB     208
#define CMD_HS			109   //��ȦС
#define CMD_STOP_HS		209
#define CMD_HB			110   //��Ȧ��
#define CMD_STOP_HB     210
#define CMD_AUTO		111	  //�Զ�	
#define CMD_STOP_AUTO	211
#define CMD_AE_ON		112		//�Զ��ع�ON
#define CMD_AE_OFF		113		//�Զ��ع�OFF
#define CMD_EV_SET		114		//�����ع�����ֵ
#define CMD_EV_SET_END	126		//�����ع�����ֵ����

#define CMD_LIGHT		240		//�ƹ�
#define CMD_WIPER		241		//��ˢ
#define CMD_FAN			242		//����

//Ԥ�õ�(��)
#define CMD_PRESET		212		//����Ԥ�õ�
#define CMD_CLR_PRESET	213		//���Ԥ�õ�
#define CMD_GOTO_PRESET	214		//����Ԥ�õ�
#define CMD_AUTO_SCAN   218		//�Զ�Ѳ�� == CMD_PATROL_RUN

//Ԥ�õ�(ԭ��)
#define CMD_PREST_SET	301
#define CMD_PREST_CALL  401

//Ѳ������:Pattern
#define CMD_PATROL_START 216
#define CMD_PATROL_STOP	217
#define CMD_PATROL_RUN	218 //ֹͣѲ���ɵ���������һ����������

//��̨���� ���û��Ľӿ�
class HPPTZ_Control
{
public:
	//----------------------------------
	//�������ܣ����öԻ���λ��
	//����ֵ  ����
	//----------------------------------
	virtual void SetWindowPos (int x,int y,int cx,int cy)=0;

	//----------------------------------
	//�������ܣ���ʾ�Ի���
	//����˵����nCmdShow: true:��ʾ false:����
	//����ֵ  ����
	//----------------------------------
	virtual void ShowWindow(bool nCmdShow) = 0;

	//----------------------------------
	//�������ܣ�ʹ�Ի����Ͽؼ���Ч������Ч
	//����˵����bEnbale: TRUE:��Ч FALSE:��Ч Type:Ŀǰ����Ҫ //��̨TYPE_PTZDLG ����Ԥ�õ� 
	//����ֵ  ����
	//----------------------------------
	virtual void SetControlEnable(BOOL bEnable,int Type=0) = 0;

	//----------------------------------
	//�������ܣ����ٶ�Ӧ�ĶԻ���
	//----------------------------------
	virtual void Destory()=0;
	

	//----------------------------------
	//�������ܣ�����ͨ����Ԥ�õ�
	//����˵����	Preid:Ԥ�õ�� 
	//				Name:Ԥ�õ����� 
	//����ֵ  ����
	//----------------------------------
	virtual void UpdatePreset(map<int,CString> info_map) = 0;


	//----------------------------------
	//�������ܣ���̨����
	//����˵����cmd: �������� chnid:ͨ����
	//����ֵ  ����
	//----------------------------------
	virtual void Control(int cmd, int chnid) = 0;

	//�����ٶ�
	virtual void SetSpeed(int speed) = 0;

	//// [3/3/2009 lzy]
	////��������·��
	//virtual void  SetLanguageIniPath(CString strIniFullPath)=0;

	// [5/19/2009 lzy]
	// nCmdShow SW_SHOW SW_HIDE
	virtual void ShowToolbar(int nCmdShow)=0;		//��ʾ��������
	virtual void SetToolbarPos(int x,int y,int nReserved=0)=0;		//nReserved=0 ��ʾtitle 1:p���ڿ��� 2:�ö�
	virtual BOOL GetToolbarShow()=0;				//��ȡ�Ƿ���ʾ
	virtual void GetToolbarRect(CRect& rc)=0;	
	virtual void UpdateToolbarTitle(CString strTitle)=0;			//����ͨ������
	virtual void UpdateToolbarPrePointName(std::map<int,CString> & mapName)=0;		//����Ԥ�õ�����
	virtual void NotifySelectChann(string ChannID)=0;		//֪ͨѡ��ָ��ͨ��
	virtual void SetMixVideo(int WindowNum=0)=0; //���û����Ƶ
	virtual void NotifyMoveEnd()=0;	//֪ͨ������̨p�����ƶ�����

};

//֪ͨ�ӿ���
class HPPTZ_ControlNotify
{
public:

	//----------------------------------
	//�������ܣ�֪ͨ�û���̨������
	//����˵����cmd:��̨����
	//����ֵ  ����
	//----------------------------------
	virtual void HPPTZ_ControlNotifyCall(int cmd,int speed,int subtype=0) = 0;

	//Ѳ��
	virtual void HPPTZ_ControlNotifyCruise(bool bStart,int CruiseNo)=0;

	//�켣
	virtual void HPPTZ_ControlNotifyTrack(bool bStart,int TrackNo)=0;

	virtual void HPPTZ_SetSpeed(int speed)=0;	//֪ͨ�����ٶ�
	virtual void HPPTZ_NotifyState(BOOL bShow)=0;	//֪ͨ������̨״̬�ı� ��ʾ�����˳�
	virtual void HPPTZ_NotifyToolbarChange()=0;		//֪ͨ������̨��չ��ȡ�۵�

};


//----------------------------------
//�������ܣ�������̨���ƶԻ���
//����˵����pParentWnd:������ָ�� pNotify:�ص��ӿ�ָ��
//����ֵ  ��������̨�ӿ�ָ��
//----------------------------------

AFX_EXT_CLASS  HPPTZ_Control* CreateHPPTZ(CWnd* pParentWnd ,HPPTZ_ControlNotify* pNotify);



#define CMD_SET_PTZ_PARAM					"cmd_set_ptz_param"/*����̨����*/
#define CMD_GET_PTZ_PARAM					"cmd_get_ptz_param"/*ȡ��̨����*/
#define CMD_ALARM_PARAM_SET					"cmd_set_alarm_param"/*�豨������*/
#define CMD_ALARM_PARAM_GET					"cmd_get_alarm_param"/*ȡ��������*/
#define CMD_ENCODER_RECORD_SET				"cmd_set_record_param"/*��¼�����*/
#define CMD_ENCODER_RECORD_GET				"cmd_get_record_param"/*ȡ¼�����*/
#define CMD_VIDEOPLAN_SET					"cmd_set_record_schedule"/*����¼��ƻ���*/
#define CMD_VIDEOPLAN_GET					"cmd_get_record_schedule"/*��ȡ¼��ƻ���*/
#define CMD_CHANNEL_GET_MOVE_DETECT			"cmd_get_motion_detect"/*ȡ�ƶ�������*/
#define CMD_CHANNEL_SET_MOVE_DETECT 		 "cmd_set_motion_detect"/*���ƶ�������*/
#define CMD_HPDEV_REBOOT					"cmd_reboot"	//������ƽ�豸
#define CMD_VIDEOPLAN_START					 "cmd_start_defence"
#define CMD_VIDEOPLAN_STOP					"cmd_stop_defence"
#define CMD_GET_DEVCFGINFO					"cmd_get_devcfginfo"		//�õ��豸����
#define CMD_SET_DEVCFGINFO					"cmd_set_devcfginfo"		//�����豸����
#define CMD_REMOTE_CHANNEL_GET_AV_WIZARD	"cmd_get_channel_param"		//�õ�ͨ������
#define CMD_REMOTE_CHANNEL_SET_AV_WIZARD	"cmd_set_channel_param"
#define CMD_LOCAL_IPSETTING					"cmd_locip_set"
#define CMD_AVCON_SERVER_INFO				"cmd_svr_address"
#define CMD_DOWNLOAD_SERVERLIST				"cmd_down_svrlst"

#define CMD_FROMID							"FROMID"
#define CMD_TOID							"TOID"

//HHS�������
#define REMOTE_GET_DVRCONFIG				"REMOTE_GET_DVRCONFIG"
#define REMOTE_SET_DVRCONFIG				"REMOTE_SET_DVRCONFIG"	
#define USERCMD								"USERCMD"
#define REMOTE_SHOW						1	//Զ�̶Ի���������ʾ 
#define REMOTE_HIDE						0	//Զ�̶Ի���û����ʾ

//��Զ��������Ϣ���������
class IRemoteCfgOUT
{
public:
	//sCmdData:��ص����� �磺�õ���̨����������̨��Ϣ
	virtual	void SendData(KCmdUserDataPacket& sCmdData)=0;
};


//�������ط�����Ϣ���뵽Զ������
class IRemoteCfgIN
{
public:
	//�ӻ�ƽ�������������������Ϣ �磺¼�� ����
	virtual	void ReceiveData(KCmdUserDataPacket& sCmdData)=0;
	virtual	void DeleteDlg()=0;		//�쳣�˳�ʱ���Ե���

};

// [4/10/2009 lzy]
//�޸�Ϊnew����
//Զ�����öԻ���ӿ�
class IRemoteCfg
{
public:
	virtual void ShowWindow(int nCmdShow)=0;
	virtual void Destory()=0;
	virtual	void UpdateInfo(MapMonDomainGroupItem& GroupItemInfo,MapMonDomainChannelItem& ChannelItemInfo,
		CString RootName,MMS_MCU* pmcu)=0;	//����Զ������Ҫ��ʼ������Ϣ
	virtual	void ReceiveData(KCmdUserDataPacket& sCmdData)=0; //֪ͨ��ƽ��������
	virtual void NotifyUpdateChanState(const CHANNELINFO &chn_info)=0;//֪ͨ����ͨ��״̬
	virtual int GetRemoteState()=0;		//�õ�Զ������״̬ ����ֵ:REMOTE_SHOW REMOTE_HIDE
	virtual void NotifyUpdateDev(string& name,string& groupid )=0;	//�����豸
	virtual void NotifyUpdateChann(string& name,string& channid )=0;	//����ͨ�� 

};
//----------------------------------
//�������ܣ���������Զ�����öԻ���
//����˵����pParentWnd:������ָ�� RootName:���������
//����ֵ  ��Զ�����ýӿ�ָ��
//----------------------------------

AFX_EXT_CLASS  void ShowRemoteConfigDlg(CWnd* pParentWnd,MapMonGroupItem& GroupItemInfo,
										MapMonChannelItem& ChannelItemInfo,CString RootName,
										IRemoteCfgOUT* pOUT,IRemoteCfgIN** pIN,MMS_MCU* pmcu);


AFX_EXT_CLASS  IRemoteCfg* CreateRemoteConfigDlg(CWnd* pParentWnd ,IRemoteCfgOUT* pNotify);

//������������öԽ���ӿ�
class IMatrixCtrl
{
public:
	virtual void ShowWindow(bool nCmdShow) = 0;
	virtual void Destory()=0;
	virtual void SetCurrenMatrixCtrl(const string& strMatrixDevID)=0;
	//���¾������������
	virtual	void UpdateMatrixDevInfo(MapMonDomainGroupItem& GroupItemInfo,MapMonDomainChannelItem& ChannelItemInfo)=0;
	virtual void OnChannelInfo(std::string strDevID,std::string strGroupName,const CHANNELINFO &chn_info)=0;
	virtual void OnMatrixCtrlRcvP2PData(const std::string&strNodeID,KCmdPacket& packet)=0;
	virtual void OnUpdateChannelName(const std::string& strChannelID,const std::string& strName, unsigned long ulGroupID)=0;
};

class HP_MatrixCtrlNotify
{
public:
	virtual void ChangeMatrix(const string& strMatrixCtrl,const std::string& strChanID,int nInput,const std::string strInputName,const std::string strMatrixID){};
	virtual bool GetMatrixInfo(const string& strMatrixCtrl,const string& strChanID,const string& strUserData){ return false ;};
};

AFX_EXT_CLASS  IMatrixCtrl* CreateMatrixCtrl(CWnd* pParentWnd ,HP_MatrixCtrlNotify* pNotify);