#ifndef _INTERFACE_PLUGIN_U7_COMMON_H_
#define _INTERFACE_PLUGIN_U7_COMMON_H_


//////////////////////////////////////////////////////////////////////////

////��Ϣ�궨��
#define WM_USER_DLGSET_CHANGE		(WM_USER + 100)
#define WM_RESET_PROMPT				(WM_USER + 101)
#define WM_PTZBTN_PRESS				(WM_USER + 102)
#define WM_USER_GOBACK				(WM_USER + 104)
#define WM_RESET_QROMPT				(WM_USER + 105)


//////////////////////////////////////////////////////////////////////////
///////////////////////////���ID���Ʊ�ʶ������/////////////////////////
//////////////////////////////////////////////////////////////////////////
///

//U7������ID
#define PLUGIN_NAME_U7_MEETING		"Plugin_U7_Meeting"

//U7�����ID
#define PLUGIN_NAME_U7T_MAIN		"Plugin_U7T_Main"

//UI���ID
#define  PLUGIN_NAME_U7_COMUI		"Plugin_U7_ComUI"

//Ƶ�����ID
#define  PLUGIN_NAME_U7_CHANNEL		"Plugin_U7T_Channel"

//1
#define PLUGIN_NAME_U7_INFORMATION	"Plugin_U7_Infomation"

//���ݿ��¼���ID
#define PLUGIN_NAME_U7_DBRECORD		"Plugin_U7_DBRecord"

//Ԥ�ò��ID
#define PLUGIN_NAME_U7_PRESET		"Plugin_U7_Preset"

//���۲��ID
#define PLUGIN_NAME_U7_CHAT			"Plugin_U7_Chat"

//��̨���ò��ID
#define PLUGIN_NAME_U7_YUNTAISET	"Plugin_U7_YuntaiSet"

//��̨���Ʋ��ID
#define PLUGIN_NAME_U7_CONTROLPTZ	"Plugin_U7_ControlPtz"



///��½������ID
#define  PLUGIN_NAME_U7_IM_LOGIN	"Plugin_U7_IM_Login"

//IM���ID
#define PLUGIN_NAME_U7_IM_MON		"Plugin_U7_IM_Mon"

//IM���ID
#define PLUGIN_NAME_U7_IM_FRIEND	"Plugin_U7_IM_Friend"

//IM���ID
#define PLUGIN_NAME_U7_IM_TALK		"Plugin_U7_IM_Talk"



//????????????
#define PLUGIN_NAME_U7_AV_CAPTUREMGR		"Plugin_U7_AV_CaptureMgr"

//ȫ�����ò��ID
#define PLUGIN_NAME_U7_AV_GLOBALSETTING		"Plugin_U7_AV_GlobalSetting"

//????????????
#define PLUGIN_NAME_U7_AV_PLAYERMGR			"Plugin_U7_AV_PlayerMgr"

//1
#define PLUGIN_NAME_U7_AV_WNDMGR			"Plugin_U7_AV_WndMgr"



//����Ƶ���ò��ID
#define PLUGIN_NAME_U7_MEETING_AVSET		"Plugin_U7_Meeting_AVSet"

//�豸�б���ID
#define PLUGIN_NAME_U7_MEETING_DEVICE		"Plugin_U7_Meeting_Device"

//�װ���ID
#define PLUGIN_NAME_U7_MEETING_WHITEBOARD	"Plugin_U7_Meeting_WhiteBoard"

//��Ա�б���ID
#define PLUGIN_NAME_U7_MEETING_MEMBERLIST	"Plugin_U7_Meeting_MemberList"

#define PLUGIN_NAME_U7T_MEETING_QUESTION	"Plugin_U7T_Meeting_Question"
//�����������ID
#define PLUGIN_NAME_U7_MEETING_BATCHOPERATE	"Plugin_U7_Meeting_BatchOperate"

//������ID
#define PLUGIN_NAME_U7_MEETING_INVITEOUT	"Plugin_U7_Meeting_InviteOut"

//������ID
#define PLUGIN_NAME_U7_MEETING_INVITEIN		"Plugin_U7_Meeting_InviteIn"

//���ù������ID
#define PLUGIN_NAME_U7_MEETING_SETBILLBOARD "Plugin_U7Meeting_SetBillboard"

//������ID
#define PLUGIN_NAME_U7_MEETING_TITLE		"Plugin_U7Meeting_Title"

//֪ͨ���ID
#define PLUGIN_NAME_U7_MEETING_NOTICE		"Plugin_U7Meeting_Notice"

//�������ID
#define PLUGIN_NAME_U7_MEETING_MUTE			"Plugin_U7Meeting_Mute"

//��Ļ�������ID
#define PLUGIN_NAME_U7_MEETING_SCREENNAVI	"Plugin_U7Meeting_ScreenNavi" 

//ģ����ID
#define PLUGIN_NAME_U7_MEETING_SELTEMPLATE	"Plugin_U7Meeting_SelTemplate"

//��Ƶģʽ���ID
#define PLUGIN_NAME_U7_MEETING_VIDEOMODE	"Plugin_U7Meeting_VideoMode"

//�������Բ��ID
#define PLUGIN_NAME_U7_MEETING_WNDPROPERTY	"Plugin_U7Meeting_WndProperty"

///U7�����������ID
#define PLUGIN_NAME_U7_MEETING_LOCK			"Plugin_U7Meeting_Lock"



//��Ա���ID
#define PLUGIN_NAME_U7_MM_MEMBER			"Plugin_U7_MM_Member"

//������ID
#define PLUGIN_NAME_U7_MM_ROOM				"Plugin_U7_MM_Room"

//���ܲ��ID
#define PLUGIN_NAME_U7_MM_FUNCTION			"Plugin_U7_MM_Function"

//���в��ID
#define PLUGIN_NAME_U7_MM_CALL				"Plugin_U7_MM_Call"

//1	
#define PLUGIN_NAME_U7_MM_SVRREC			"Plugin_U7_MM_SvrRec"



//1
#define PLUGIN_NAME_U7_CALL_H323			"Plugin_U7_Call_H323"

//
#define PLUGIN_U7_CALL_SLIP					"Plugin_U7_Call_Sip"

//1
#define PLUGIN_NAME_U7_CFG_COMM				"Plugin_U7_Cfg_Comm"

//1
#define PLUGIN_NAME_U7_TM_MFC				"Plugin_U7_TM_MFC"

//
#define PLUGIN_NAME_U7MEETING_AVINFO		"Plugin_U7Meeting_AVInfo"


#define PLUGIN_U7C_TERMINALMESG				"Plugin_U7C_TerminalMesg"




//�򿪱���Ի�����ID
#define PLUGIN_NAME_U7T_OPENSAVE			"Plugin_U7T_OpenSave"

//ý�干����ID
#define PLUGIN_NAME_U7T_MEDIASHARE			"Plugin_U7T_MediaShare"

//�ĵ�����˵����ID
#define PLUGIN_NAME_U7T_DOCSHAREMENU		"Plugin_U7T_DocShareMenu"

//�ĵ�������ID
#define PLUGIN_NAME_U7T_DOCSHARE			"Plugin_U7T_DocShare"

//��Ϣ��ʾ���ID
#define PLUGIN_NAME_U7T_MESSAGEBOX			"Plugin_U7T_MessageBox"

//������ID
#define PLUGIN_NAME_U7T_MEETING				"Plugin_U7T_Meeting"

//���в��ID
#define PLUGIN_NAME_U7T_CALL				"Plugin_U7T_Call"

//��ϲ��ID
#define PLUGIN_NAME_U7T_DIAGNOSIS			"Plugin_U7T_Diagnosis"

//���MCU���ID
#define PLUGIN_NAME_U7T_DIAGNOSIS_MCU		"Plugin_U7T_Diagnosis_MCU"

//HPDTC���ID
#define PLUGIN_NAME_U7T_HPDTC				"Plugin_U7T_HPDTC"

//U7T������ٹ㲥��������Ƶ���ID
#define PLUGIN_NAME_U7T_Meeting_BoardCastAV	"Plugin_U7T_Meeting_BoardCastAVMine"

//U7T�������ͶƱ���ID
#define PLUGIN_NAME_U7T_Meeting_VOTE		"Plugin_U7T_Meeting_VoteByShowHand"

//U7T����ͶƱ���ID
#define PLUGIN_NAME_U7T_MEETING_BALLOT		"Plugin_U7T_Meeting_Vote"

//������ƵԤ�����ID
#define PLUGIN_NAME_U7T_LOCALCHANPREVIEW	"Plugin_U7T_LocalChanPreview"

//U7T������Ƶ������ID
#define PLUGIN_NAME_U7T_MEETING_STREAMCTRL  "Plugin_U7T_Meeting_StreamCtrl"

//U7T���� "roll call" ID
#define PLUGIN_NAME_U7T_MEETING_ROLLCALL	"Plugin_U7T_Meeting_RollCall"

//��Ƶ�������ID
#define PLUGIN_NAME_U7T_VIDEOPARAMETER		"Plugin_U7T_VideoParameter"

//�������ò��ID
#define PLUGIN_NAME_U7T_SET_NET				"Plugin_U7T_SET_NET"

//��̨���Ʋ��ID
#define PLUGIN_NAME_U7T_YUNTAICTRL			"Plugin_U7T_YuntaiCtrl"

//Ԥ�õ����ò��ID
#define PLUGIN_NAME_U7T_PRESETPOINT			"Plugin_U7T_PreSetpoint"

//ң�ز��ID
#define PLUGIN_NAME_U7_REMOTECONTROL        "Plugin_U7_RemoteControl"

//������ѭ
#define  PLUGIN_NAME_U7_MEETING_LUNXUN		"Plugin_U7_Meeting_LunXun"
//�����������ID
#define PLUGIN_NAME_U7T_MEETING_SPEECHENCOURAGE	"Plugin_U7T_Meeting_SpeechEncourage"

//Ƶ���б�
#define  PLUGIN_NAME_U7T_CHANNELLIST       "Plugin_U7T_ChannelList"

//һ���ر����д�Ƶ���б����Ƶ
#define  PLUGIN_NAME_U7T_CHANNELCLOSE       "Plugin_U7T_ChannelClose"

#define PLUGIN_NAME_U7T_AVMENU				"Plugin_U7T_AVMenu"

/************************************************************************/
/* ��������ʹ�ò��                                                     */
//���͸���
#define  PLUGIN_U7T_MEETING_CCB_ASSISTFLOW       "Plugin_U7T_Meeting_CCB_AssistFlow"
/************************************************************************/

/************************************************************************/
/* ũ����ʹ�ò��                                                     */
//U7����Ԥ�ò��ID
#define PLUGIN_NAME_U7_MEETING_PRESET "Plugin_U7_Meeting_Preset"
/************************************************************************/

/************************************************************************/
/*�ļ��ַ�                                                                 */
/************************************************************************/
#define PLUGIN_U7_HPFILESEND "Plugin_U7_HPFileSend"



//����ICON�ı�ʶ
#define	IDR_MAINFRAME			128		// ������ͼ��ID
#define	IDR_MEETINGPLAYER		128		// ����¼�񲥷���ͼ��ID

#endif
