#pragma once
//��ͷ�ļ�����AVView������ ���ڹ���

#include <string>
#include <vector>
#include <list>
#include <map>
#include "hptm\HPTM_Templet.h"
#include "..\include\HPDEV_Define.h"
using namespace std;

// [6/25/2009 lzy]
//---����������--------------------------------------------------------------------------
//���ֶ�������
#define MONITOR_AUDIO_INDEX							201		//��ضԽ�ͨ��ID
#define BEGIN_COMMAND_ID	0
#define END_COMMAND_ID		65535


#define ENABLE_CHANNEL_ONLINE		1 			//��ͷ����(���ɿ�)
#define ENABLE_CHANNEL_RCV			2 			//��ͷ����ͼ��
#define ENABLE_CHANNEL_REC			4			//¼��
#define ENABLE_CHANNEL_RCV_A		8			//��ͷ������Ƶͼ��
#define ENABLE_CHANNEL_RCV_V		16			//��ͷ������Ƶͼ��
#define ENABLE_CHANNEL_POLL			32			//��ѭ

#define WM_DECODER_CLOSE_DISPLAY		WM_USER+2233		//֪ͨӲ���뿨�ر�ͨ��

//�ַ�����������
#define CMD_GET_AVWIZARD					"AVWIZARD_GETTING"
#define CMD_SET_AVWIZARD					"AVWIZARD_SETTING"
#define CMD_FILL_WND_SHOW					"FILL_WND_SHOW"
#define CMD_FILL_VIDEO_SHOW					"FILL_VIDEO_SHOW"
#define CMD_FILL_WND_SHOW_FOR_ALL_TVS		"FILL_WND_SHOW_FOR_ALL_TVS"
#define CMD_FILL_VIDEO_SHOW_FOR_ALL_TVS		"FILL_VIDEO_SHOW_FOR_ALL_TVS"
#define CMD_CHANGE_MAIN_SUB_FOR_ALL_TVS     "CHANGE_MAIN_SUB_FOR_ALL_TVS"
#define CMD_CHANGE_MAIN_SUB_FOR_TVS			"CHANGE_MAIN_SUB_FOR_TVS"
#define CMD_DEFAULT_SUB_WIN_BIG_FOR_ALL_TVS	"DEFAULT_SUB_WIN_BIG_FOR_ALL_TVS"
#define CMD_DEFAULT_SUB_WIN_SMALL_FOR_ALL_TVS	"DEFAULT_SUB_WIN_SMALL_FOR_ALL_TVS"
#define CMD_GET_AVWIZARD_CONFERENCE			"AVWIZARD_RUEST"
#define CMD_AVWIZARD_CONFERENCE_RESULT		"AVWIZARD_ACCEPT"
#define CMD_AVWIZARD_CONFERENCE_SETTING		"AVWIZARD_SETTING"

//����ͷ
#define RECEIVE_VIDEO			"RECEIVE_VIDEO"			//������Ƶ
#define CHANGE_VIDEO			"CHANGE_VIDEO"			//ͬһ��TVS�Ĵ�����ק/����
#define CLOSE_VIDEO				"CLOSE_VIDEO"			//�ر�ָ����Ƶ
#define CHANGE_TM				"CHANGE_TM"				//����ģ��
#define VIDEO_FULLSCR			"VIDEO_FULLSCR"			//��Ƶȫ��
#define RECEIVE_VIDEO_RESULT	"RECEIVE_VIDEO_RESULT"	//TVS������Ƶ�������
#define CHANGE_VIDEO_RESULT		"CHANGE_VIDEO_RESULT"	//ͬһ��TVS�Ĵ�����ק/�����������
#define CLOSE_VIDEO_RESULT		"CLOSE_VIDEO_RESULT"	//�ر�ָ����Ƶ�������
#define CHANGE_TM_RESULT		"CHANGE_TM_RESULT"		//����ģ��������
#define VIDEO_FULLSCR_RESULT	"VIDEO_FULLSCR_RESULT"	//��Ƶȫ���������
#define GET_TVS_TMINFO			"GET_TVS_TMINFO"		//�õ���ǰģ����Ϣ
#define SEND_TVS_TMINFO			"SEND_TVS_TMINFO"		//�ص���TVC��ǰ����ģ��
#define GET_FULLSCR_STATUS		"GET_FULLSCR_STATUS"	//��ҪTVSȫ��״̬
#define FULLSCR_STATUS_RESULT	"FULLSCR_STATUS_RESULT"	//����TVSȫ��״̬
#define SEND_TVS_MONALERT		"SEND_TVS_MONALERT"		//���ͱ���
#define TVS_MONALERT_RESULT		"TVS_MONALERT_RESULT"	//TVS���յ�TVC�ı�������
#define TVS_DO_ALARM			"TVS_DO_ALARM"			//TVS��ʾ��������
#define TVS_CMD_END_RESULT		"TVS_CMD_END_RESULT"	//TVS���ش�����
#define TVS_CMD_UDP_END			"TVC_CMD_UDP_END"		//119�Ӿ�һ����������
#define TVS_CMD_PLUGIN			"TVS_CMD_PLUGIN"		//���ƶ˷�������ˣ����ز��������

#define TVWALL_CMD				"USERCMD"			//����ͷ
#define TVC_ID					"TVC_ID"			//TVC�ĵ�¼ID
#define TVS_ID					"TVS_ID"			//TVS�ĵ�¼ID
#define FROM_SCR				"FromScr"			//Դ��Ļ��
#define FROM_INDEX				"FromIndex"			//Դ���ں�
#define TO_SCR					"TOScr"				//Ŀ����Ļ��
#define TO_INDEX				"TOIndex"			//Ŀ�괰�ں�
#define TVS_SCR					"TVSScr"			//TVS����Ļ��
#define TVS_INDEX				"TVSIndex"			//TVS�Ĵ��ں�
#define MEMBER_ID				"Video_memberid"	//��Ƶ��memberid
#define CARD_ID					"Video_cardid"		//��Ƶ�Ŀ�id
#define TM_TYPE					"TMType"			//ģ������4��3����16��9
#define TM_SUBTYPE				"TMSubType"			//ģ��������
#define RESULT					"result"			//���
#define FROMID					"FROMID"			//����ID
#define TOID					"TOID"				//����ID
#define TM_TYPEARR				"TMType%d"			//ģ������
#define TM_SUBTYPEARR			"TMSubType%d"		//ģ��������
#define TM_FULL_SCREEN			"TMFullScreen"		//ģ��ȫ��/ȡ��ȫ��
#define SCR_COUNT				"SCR_COUNT"			//��Ļ��
#define FULL_TVS_INDEX			"FULL_TVS_INDEX"	//ȫ���Ĵ��ں�
#define REV_TYPE				"REV_TYPE"			//�������ͣ���Ƶ����ͼ�ȣ�
#define LASTPLAYINFO_NUM		"LastPlayInfo_Num"		//�ϴν��յ���Ƶ����
#define LASTPLAYINFO_SCR		"LastPlayInfo_Scr%d"	//�ϴν��յ���Ƶ����
#define LASTPLAYINFO_INDEX		"LastPlayInfo_Index%d"	//�ϴν��յ���Ƶ���ں�
#define LASTPLAYINFO_MEMID		"LastPlayInfo_Memid%d"	//�ϴν��յ���Ƶmemberid
#define LASTPLAYINFO_CARDID		"LastPlayInfo_Cardid%d"	//�ϴν��յ���Ƶ��id
#define LASTPLAYINFO_BROUND		"LastPlayInfo_BRound%d" //�Ƿ�����ѯ��Ƶ
#define DECODE_TYPE		    	"DecoderType"			//��Ӳ���������� 0:soft 1:hk,2DH
#define HARD_DECODER_MAX_INPUT	"HardDecoderMaxInput"		//Ӳ���뿨�����������
#define TVC_CMD_END				"TVC_CMD_END"			//����ǽ�Ӿ�һ�����ֽ�����ʾ
#define TVC_CMD_UDP_END			"TVC_CMD_UDP_END"		//119�Ӿ�һ����������
#define PLUGIN_TYPE				"PLUGIN_TYPE"			//���ز��������
#define PLUGIN_CURVER			"PLUGIN_CURVER"			//���ز���ĵ�ǰ�汾
#define	PLUGIN_CMD				"PLUGIN_CMD"			//���ز��������
//���ջ�����Ա��Ƶʹ��
#define ChanName				"ChanName"
#define AudChanID				"AudChanID"	//��Ա����ƵID
#define	VidChanID				"VidChanID"	//��Ա����ƵID
#define PeerNodeID				"PeerNodeID"
#define PeerNATIP				"PeerNATIP"
#define PeerLocalIP				"PeerLocalIP"
#define PeerLocalPort			"PeerLocalPort"
#define PeerMCUID				"PeerMCUID"
#define PeerMCUIP				"PeerMCUIP"
#define PeerMCUPort				"PeerMCUPort"

#define ATTRIB_RESULT			"result"
#define ATTRIB_RESULT_SUCCESS	"success"
#define ATTRIB_RESULT_FAILUER	"failure"
#define ATTRIB_RESULT_BUSY		"busy"
#define ATTRIB_RESULT_FINISH	"finish"
#define ATTRIB_RESULT_REFUSE	"refuse"
#define ATTRIB_RESULT_REFUSE	"refuse"
#define ATTRIB_RESULT_SELF		"self"
#define ATTRIB_RESULT_SAMEROOM	"sameroom"

#define ATTRIB_USER				"user"
#define ATTRIB_USERNAME			"usrname"

// [6/26/2009 lzy] �˴��������TVS�˷��͸�TVC����صĽ��
#define TVC_RESULT_OPEN_VIDEO_SUCCESS			0		//TVS�˴���Ƶʧ��
#define TVC_RESULT_OPEN_VIDEO_FAILED			1		//TVS�˴���Ƶʧ��
#define TVC_RESULT_CHANGE_TM_FAILED				2		//����ģ�����
#define TVC_RESULT_NO_INPORT_USED				3			//û������˿ڿ���
#define TVC_RESULT_UNSUPPORT					4			//���뿨��֧�ִ���
#define TVC_RESULT_CHANN_CONNECTING				5			//ͨ����������
#define TVC_RESULT_CHANN_PLAYBACK_FAILED		6		//¼��طŴ�ʧ��

//---ö������--------------------------------------------------------------------------
enum {GROUP_ORG,GROUP_DEV};

//�豸����
typedef enum
{
	TVW_DEV_UNKNOWN	= 0,
	TVW_DEV_TVS_H	= 1,	//��������ǽ������
	TVW_DEV_TVS_D,			//Ӳ�������ǽ������
	TVW_DEV_MATRIX,			//����
	TVW_DEV_ANALOG,			//ģ���ź�����豸
}TVW_DEVICE_TYPE;

//���뿨����
typedef enum
{
	DECODE_CARD_H	= 0,	//���
	DECODE_CARD_HK,			//Ӳ�������� ����
	DECODE_CARD_DH,			//Ӳ�������� ��
	DECODE_CARD_AN,			//ģ���ź�
}DECODE_CARD_TYPE;

//����ǽ��������
enum HPTVC_TM_LAYOUT
{
	TVC_TM_LAYOUT_NO = 0,
	TVC_TM_LAYOUT_3X1 = 3,
	TVC_TM_LAYOUT_3X2 = 6,
};

typedef enum _TVW_RECV_TYPE
{
	TVW_RECV_TYPE_VIDEO = 0,
	TVW_RECV_TYPE_MAP,
	TVW_RECV_TYPE_PLAYBACK,
}TVW_RECV_TYPE;


enum _MENU_TYPE
{
	M_RcvAVChannel=0,	//����ͨ������Ƶ
	M_RcvAChannel,		//������Ƶ
	M_RcvVChannel,		//������Ƶ
	M_FindRecFile,		//����¼���ļ�
	M_RecChannel,		//¼��
	M_CloseChannel,		//�ر����ڽ����е�������Ƶ
	M_DevTalk,			//��ضԽ�
	M_DevTalkTest,		//��ضԽ�����
	M_DevMatrix,		//�������
	M_ShowTMControl,	//��ʾģ�������
	M_ShowTitle,		//��ʾ������
	M_SetAudio,			//��Ƶ����
	M_ChangeMainSub,	//�л���Ƶ��/����
	M_PlugInManage,		//�������
	M_ExitMonClient,	//�˳���ؿͻ���
	M_ShowFillWin,		//��䴰����ʾ
	M_ShowPorportion,	//�Ӿ�����ȫ����ʾ
	M_DefSubWinBig,		//�󴰿�Ĭ�Ͻ�������
	M_DefSubWinSmall,	//С����Ĭ�Ͻ�������
	M_DevRound,			//�豸��ѭ
	M_TempGroupRound,	//��ʱ������ѭ�������ڣ�
	M_TempGroupRoundM,	//��ʱ������ѭ���ര�ڣ�
	M_TempTVCGroupRound, //����ǽ����ѭ
	M_OpenMap,			//�򿪵�ͼ  
	M_EditMap,			//�༭��ͼ  
	M_SaveMap,			//�����ͼ
	M_GoToChannel,
	M_DelMap,			//ɾ����ͼ
	M_CloseMap,  		//�رյ�ͼ
	M_LOSS,				//����
	M_OpenAVDiagnosis	//������Ƶ���
};

//��Ϣ����wParam
typedef enum
{
	Mon_TempShow = 0,			//Ԥ��
	Mon_Replay = 1,				//�ط�
	Mon_Map,					//��ͼ
	Mon_SelectScr1,				//ѡ1����
	Mon_SelectScr2,				//ѡ2����
	Mon_SelectScr3,				//ѡ3����
	Mon_SelectScr4,				//ѡ4����
	Mon_LocalSetting,			//��������
	Mon_MatrixSet,				//�������������
	Mon_DVRSetting,				//Զ��DVR����
	Mon_Lock,					//����
	Mon_Exit,					//�˳�
	Mon_Select,					//ѡ��ģ��
	Mon_FullScr,				//ȫ��
	Mon_Video,					//¼��
	Mon_BeginTalk,				//�����Խ�
	Mon_Info,					//��ϸ��Ϣ
	Mon_Photo,					//����
	Mon_PriScr,					//����
	Mon_ConShoot,				//����
	Mon_ChkLoss,				//����
	Mon_TVSSetting,				//����ǽ��Ļ����
	Mon_DiagnosisLog,			//�����־

}TOOLBAR_TYPE;

enum ITEM_STATUS
{
	S_RES_ROOT=0,			//�����
	S_GROUP,		        //����
	S_DEV_ONLINE,			//�豸����
	S_DEV_OFFICE,			//�豸������
	S_CHANNEL_ONLINE,		//��ͷ����(���ɿ�)
	S_CHANNEL_ONLINE_OPT,	//��ͷ����(�ɿ�)
	S_CHANNEL_OFFLINE,		//��ͷ������
	S_CHANNEL_RCV,			//��ͷ����ͼ��(���ɿ�)
	S_CHANNEL_RCV_OPT,		//��ͷ����ͼ��(�ɿ�)
	S_CHANNEL_REC,			//¼��(���ɿ�)
	S_CHANNEL_REC_OPT,		//¼��(�ɿ�)
	S_CHANNEL_POLL, 		//��ѭ
	S_CHANNEL_POLL_OPT,		//��ѭ(�ɿ�)
	S_CHANNEL_POLLREC,		//��ѭ¼��

	S_SELFDEFINE_DEV_ROOT,	//�Զ����豸�����
	S_ANG_DEV,				//�Զ����豸
	S_ANG_PORT,				//�Զ����豸����˿�����״̬
	S_ANG_PORTLINK,			//�Զ����豸����˿ڹ���״̬

	S_CHANNEL_RCV_A,		//��ͷ������Ƶͼ��
	S_CHANNEL_RCV_A_OPT,	//��ͷ������Ƶͼ��(�ɿ�)
	S_CHANNEL_RCV_V,		//��ͷ������Ƶͼ��
	S_CHANNEL_RCV_V_OPT,	//��ͷ������Ƶͼ��(�ɿ�)
};

//[3/16/2009 licong]
//��ر�������
enum _ALARM_TYPE{
	ALARM_DISK_FULL = 0,	//Ӳ����                        �������
	ALARM_VIDEO_LOST,		//�źŶ�ʧ                      ͨ����
	ALARM_VIDEO_MOTION,		//�ƶ����						ͨ����
	ALARM_DEFENT_IO,		//�ź�������                    ��������˿ں�
	ALARM_VIDEO_BLOCK,		//�ڵ�����						ͨ����
	ALARM_VIDEO_NOMATCH,	//��ʽ��ƥ��					ͨ����
	ALARM_DISK_UNFORMAT,	//Ӳ��δ��ʽ��					�������
	ALARM_DISK_READ_ERROR,	//��дӲ�̳���					�������
	ALARM_ACCESS_VIOLATION, //�Ƿ�����						����
	ALARM_UNKNOW_ERROR,		//�Ƿ�����						����
};



//---------�ṹ������------------------------------------------------------------------

//TVS����״̬
typedef struct _TVS_LASTPLAY_INFO
{
	int screen;				//����
	int index;				//���ں�
	string memberid;		//��Ƶ��memberid
	unsigned long cardid;	//��id
	bool bRound;			//�Ƿ�Ϊ��ѯ��������
}TVS_LASTPLAY_INFO;


//������������TVS�б�
typedef struct _TVW_SERVER_INFO
{
	std::string         ServerID;  		//TVS-ID
	std::string			ServerName;		//TVS-����
	std::string			Status;			//TVS-״̬
}TVW_SERVER_INFO,*PTVW_SERVER_INFO;
typedef map<std::string, TVW_SERVER_INFO> MAP_TV_SERVER_INFO;
typedef MAP_TV_SERVER_INFO::iterator MAP_TV_SERVER_INFO_IT;


//�豸��Ϣ
typedef struct _TVW_DEVICE_INFO
{
	TVW_DEVICE_TYPE		Type;			//�豸����
	std::string			ID;				//�豸ID��TVS-ID�����ID��
	std::string			Name;			//�豸���ƣ�TVS-���ƻ�������ƣ�
	int					InPort;			//����˿����������������˿ڸ�����
	int					OutPort;		//����˿���������������˿ڸ�����TVS��Ļ������
	bool				bStatus;		//������״̬
}TVW_DEVICE_INFO,*PTVW_DEVICE_INFO;
typedef map<std::string, TVW_DEVICE_INFO> MAP_TVW_DEVICE_INFO;		//��map��key��Type��ID����� ΪType_ID
typedef MAP_TVW_DEVICE_INFO::iterator MAP_TVW_DEVICE_INFO_IT;



//�豸��TV֮�������Ϣ
typedef struct _TV_INFO
{
	int 				ScreenID;   	//��ĻID
	std::string			ScreenName;  	//��Ļ����
	std::string			DevInfoKey;		//�������豸��Ϣ
	int					Port;			//�������豸��ĳ���˿�
	TVW_DEVICE_TYPE		Type;
	DECODE_CARD_TYPE	CardType;		//��ӦTVS�Ľ�������
}TV_INFO,*PTV_INFO;
typedef map<int, TV_INFO> MAP_TV_INFO;
typedef MAP_TV_INFO::iterator MAP_TV_INFO_IT;

typedef map<int,int> MAP_TM2TVPOS;
typedef MAP_TM2TVPOS::iterator MAP_TM2TVPOS_IT;
//�豸�˿���Ϣ
typedef struct _DEV_PORT_INFO
{
	TVW_DEVICE_TYPE     deviceType;		//�豸���ͣ������TVS��
	DECODE_CARD_TYPE	CardType;		//���TVS�Ľ�������
	int 				PortID;			//�˿�ID����Ӧ����ļ��Ŷ˿ڻ���TVS�ļ�������
	string           	PortName;		//�˿�����
	string 				strDevID;		//�豸ID��TVS-ID�����ID��
	string				strDevName;		//�豸���ƣ�TVS-���ƻ�������ƣ�
	BOOL             	bIsOnLine;		//�Ƿ�����
	int					tm_index;		//ģ����
	HPTM_TM_TYPE		tm_type;		//ģ������
	HPTM_TM_SUBTYPE		tm_subtype;		//ģ��������
	int					iPos;			//�������ӻ���Ϣ��-1 δ����������ֵΪ����λ�ã�
	BOOL				bLinkMatrix;	//�Ƿ���������

	int					iFullWndIndex;	//��Ƶȫ��������
	BOOL				bLink;			//�Ƿ��ѱ�����
	CWnd*				pWndShow;		//��������ʾ��Ϣ
}DEV_PORT_INFO,*PDEV_PORT_INFO;
typedef std::map<string, DEV_PORT_INFO> MAP_DEV_PORT_INFO;		//��map��key��deviceType_strDevID_PortID���
typedef MAP_DEV_PORT_INFO::iterator MAP_DEV_PORT_INFO_IT;



typedef struct _MATRIX_INPUT_INFO
{
	string 				MatrixID;		//����ID
	string				strMatrixName;	//��������
	UINT				InputID;
	UINT				InputCnt;
	UINT				OutputID;
	UINT				OutputCnt;
	string 				strDevID;		//�豸ID��TVS-ID�����ID��	
	string				strDevName;		//�豸���ƣ�TVS-���ƻ�������ƣ�
	int 				DevPortID;			//�˿�ID����Ӧ����ļ��Ŷ˿ڻ���TVS�ļ�������
	TVW_DEVICE_TYPE     deviceType;		//�豸���ͣ������TVS��
	DECODE_CARD_TYPE	CardType;		//���TVS�Ľ�������
}MATRIX_INPUT_INFO,*PMATRIX_INPUT_INFO;

typedef std::map<int, MATRIX_INPUT_INFO> MAP_MATRIX_INPUT_INFO;//��KEY�ɾ���ID*1000 + inputID ���
typedef MAP_MATRIX_INPUT_INFO::iterator  MAP_MATRIX_INPUT_INFO_IT;


typedef struct tagGroupTempAdminItem
{
	std::string channelid;
	std::string channelname;
	std::string groupid;
	std::string groupname;
	std::string tempgroupid;
}GROUPTEMP,*PGROUPTEMP;

typedef struct tagTmpChannel
{
	std::string channelid;
	int status;
	std::string tempgroupid;
}TMPCHANNEL,*PTMPCHANNEL;

typedef std::map<HTREEITEM,TMPCHANNEL*>			Map_TmpGrupChnl;


//-------------------------------------------------------------------------------------