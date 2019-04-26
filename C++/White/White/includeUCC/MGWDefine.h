#pragma once

// [7/28/2009 lzy]
//���������λ 

typedef enum
{
	//ϵͳ
	TYPE_SYS=0,			//ϵͳ
	TYPE_MTS,			//������
	TYPE_TOOLS,			//���ع���
	TYPE_ALARM,			//����

	//DVR
	TYPE_HP=0x100,		//��ƽDVR	
	TYPE_HIK,			//����DVR
	TYPE_DH,			//��DVR
	TYPE_KEDA,			//�ƴ�DVR
	TYPE_HHS,			//�ƺ�DVR
	TYPE_HBT,			//����DVR
	TYPE_YAA,			//����ǰ�
	TYPE_TDY,			//���ΰҵ
	TYPE_NCI,			//�����о���
	TYPE_JTN,			//���ݾ���
	TYPE_TLV,			//�Ϻ�ͨ��
	TYPE_BSC,			//����ͨ
	TYPE_PLT,			//ƽ̨
	TYPE_XINQI,			//о��
	TYPE_SKY,			//��ɫ�Ǽ�

	//RTSP
	TYPE_RTSP=0x200,	//RTSPЭ��
}ERROR_TYPE;

//enum {
//	//DVR/DVS/NVS/IPCAM
//	HPD_DVR = 0, /*��ƽ*/
//	HIK_DVR, /*����*/
//	DAH_DVR, /*��*/
//	KEDA_DVR, /*�ƴ�*/
//	HHS_DVR, /*�ƺ�*/
//	HBT_DVR, /*����߿�*/
//	YAA_DVR, /*����ǰ�*/
//	TDY_DVR, /*���ΰҵ*/
//	CJV_DVR, /*ʮ����*/
//	JTN_DVR, /*���ݾ���*/
//	PLT_DVR, /*ƽ̨*/
//	TLV_DVR, /*�Ϻ�ͨ��*/
//	BSC_DVR, /*����ͨ*/
//	MAX_DVR,
//
//	HPD_MOB = 0x99,/*��ƽ�ֻ�*/
//	HDS1000,/*�����豸*/
//	NDS410,/*��ƽDVR*/
//
//	//NDS(OEM)
//	NDS_100=(0x0100|HIK_DVR),/*����*/
//	NDS_200=(0x0200|HIK_DVR),/*����*/
//	NDS_300=(0x0300|HHS_DVR),/*�ƺ�*/
//	MAX_NDS,
//
//	//RTSP
//	RTSP_HHS = 0x1000, /*�ƺ�*/
//	RTSP_ALK, /*������*/
//	RTSP_KEDA, /*�ƴ�*/
//	RTSP_SANYO, /*����*/
//	MAX_RTSP,
//};

typedef enum 
{
	PRINT_ERROR	=0x01,/*����*/
	PRINT_WARN	=0x02,/*����*/
	PRINT_NOTICE=0x04,/*֪ͨ*/
	PRINT_INFO	=0x08,/*��Ϣ*/
}PRINT_LEVEL;//��ӡ����

#define  MOV_BIT		28
// [7/29/2009 lzy]
//������ص�������Ϣ

//--------------------------------------------------------------------------------------------
//ϵͳ�ڲ�����
typedef enum
{
	ENENT_BASE=120,
	
	/*
	��������
	*/
	EVENT_GWC2MON_GATEWAY_CHANGED,			//��������
	EVENT_GWC2MON_GATEWAY_CONNECT,
	EVENT_GWC2MON_GATEWAY_DISCONNECT,
	EVENT_GWC2MON_GATEWAY_LOGIN,
	EVENT_GWC2MON_GATEWAY_LOGOUT,

	EVENT_GWC2DVR_DEVICE_CONNECT,			
	EVENT_GWC2DVR_DEVICE_RECONNECT,
	EVENT_GWC2DVR_DEVICE_DISCONNECT,			
	EVENT_GWC2DVR_DEVICE_REMOVE,				
	EVENT_GWC2MON_DEVICE_REONLINE,			

	EVENT_DVR2MON_DEVICE_ONLINE,				
	EVENT_DVR2MON_DEVCIE_OFFLINE,			
	EVENT_DVR2MON_CHANNEL_ONLINE,			
	EVENT_DVR2MON_CHANNEL_OFFLINE,			
	EVENT_MON2MCU_CHANNEL_ONLINED,			
	EVENT_MON2MCU_CHANNEL_OFFLINED,			
	EVENT_MON2DVR_DISPATCH_MCUINFO,				
	EVENT_GWC2MCU_PROTOCOL_CHANGED,

	/*
	������������
	*/
	EVENT_DVR2MON_DEVICE_CHANGED,			
	EVENT_DVR2MON_CHANNEL_CHANGED,			
	EVENT_MON2DVR_GET_AVSETTING,			
	EVENT_MON2DVR_SET_AVSETTING	,		
	EVENT_DVR2MON_RESPOND_AVSETTING,
	EVENT_MON2DVR_CHANGE_CHINNAME,

	/*
	ʹ��MomeryToString��StringToMemory�ķ�ʽ
	*/
	EVENT_MON2DVR_GET_DVRSETTING,
	EVENT_MON2DVR_SET_DVRSETTING,
	EVENT_DVR2MON_RESPOND_DVRSETTING,

	/*
	Ԥ����������
	mcu request doplay --> dvr play
	mcu request replay --> dvr replay
	mcu request reconnect --> dvr disconnect --> dvr connect
	*/
	EVENT_MCU2DVR_REQUEST_DOPLAY,	
	EVENT_MCU2DVR_REQUEST_REPLAY,	
	EVENT_MCU2DVR_REQUEST_KEYFRAME,	

	/*
	�Խ�����
	mon called --> mcu called (start receive and send)
	mon hunged --> mcu huanged (stop receive and send)
	mcu hunged(stop receive) --> mon huanged (stop send)
	*/
	EVENT_MON2DVR_VOICE_CALLED,	
	EVENT_MON2DVR_VOICE_HUNGED,	
	EVENT_MON2MCU_VOICE_CALLED,	
	EVENT_MON2MCU_VOICE_HUNGED,	
	EVENT_MCU2MON_VOICE_HUNGED,	
	EVENT_DVR2MON_VOICE_RESPOND,
	EVENT_MON2MCU_VOICE_CHANGE,

	/*
	��̨����
	*/
	EVENT_MON2DVR_PTZ_CONTROL,	
	EVENT_MON2DVR_PTZ_SET_SETTING,	
	EVENT_MON2DVR_PTZ_GET_SETTING,	
	EVENT_DVR2MON_PTZ_RESPOND_SETTING,	

	/*
	�����ϴ�
	*/
	EVENT_DVR2MON_ALARM_SIGNAL, 

	/*
	�������Ը���
	*/
	EVENT_DVR2MON_ALARM_SGUPDATE,

	/**/
	EVENT_MON2DVR_STATE_NETCHECK,
#ifdef NVR_ENABLE
	/*
	NVR״̬�ϱ�
	*/
	EVENT_NVR2MTS_SYSTEM_STATUS,
	EVENT_NVR2MTS_CHANNEL_STATUS,
	EVENT_NVR2MTS_REQUEST_VRSINFO,
#endif
}CMD_EVENT;

#define EVENT_DVR2DVR_DEVICE_CONNECT	EVENT_GWC2DVR_DEVICE_CONNECT
#define EVENT_DVR2DVR_DEVICE_RECONNECT	EVENT_GWC2DVR_DEVICE_RECONNECT
#define EVENT_DVR2DVR_DEVICE_DISCONNECT	EVENT_GWC2DVR_DEVICE_DISCONNECT
#define EVENT_DVR2DVR_DEVICE_REMOVE		EVENT_GWC2DVR_DEVICE_REMOVE	
#define EVENT_MCU2DVR_REQUEST_RECONNECT	EVENT_GWC2DVR_DEVICE_RECONNECT

//���ش�����Ϣ
typedef enum
{
	MGW_ERR_MAX_DVR_NUMBER_LIMIT=0x1000,	/*���ص�������豸������*/
	MGW_ERR_MAX_CHANNEL_NUMBER_LIMIT,		/*���ص������ͨ��������*/
	MGW_ERR_DVR_RECONNECTING=65533,			/*�豸�Ͽ���*/
	MGW_ERR_DVR_DISCONNECTING,				/*�豸������*/
	MGW_ERR_DVR_REMOVING,					/*ɾ���豸��*/
	MGW_ERR_DVR_CONNECTING,					/*�����豸��*/
}MGW_ERROR;

//MTS���صĴ�����Ϣ
typedef enum
{
	MTS_ERR_MGW_CONNECTED=100,		//��������MTS�ɹ�
	MTS_ERR_MGW_LOGINED,			//���ص�¼�ɹ�
	MTS_ERR_DVR_ONLINED,			//DVR���߳ɹ�
	MTS_ERR_CHANNEL_ONLINED,		//ͨ�����߳ɹ�
	MTS_ERR_MGW_CONNECTTING=1000,	//������������MTS
	MTS_ERR_MGW_RECONNECTED,		//��������MTS�ɹ�
	MTS_ERR_MGW_SERVERISBUSY,		//MTS��������æ
	MTS_ERR_MGW_CONNECTFAILED,		//��������MTSʧ��
	MTS_ERR_MGW_DISCONNECTED,		//������MTS�Ͽ�����
	MTS_ERR_DVR_ACCOUNTISNULL,		//DVR��AVCON�ʺ�Ϊ��
	MTS_ERR_DVR_SERIALNOISNULL,		//DVR�����к�Ϊ��
	MTS_ERR_MGW_NOLOGIN,			//����δ��¼
	MTS_ERR_DVR_NOLOGIN,			//����δ������DVR�豸
}MTS_ERROR;

//TYPE_TOOLS  ����������(���ع��߹���������,��֤Ȩ��)				
typedef enum
{
	CMD_TOOLS_LOGIN=0,
	CMD_TOOLS_LOGOUT,
	CMD_TOOLS_HANDSHAKE,
	CMD_GET_DEV_ALLINFO,					//�õ������豸��Ϣ
	CMD_GET_DEV_INFO, 							
	CMD_GET_CHAN_INFO,						//�õ�ͨ����Ϣ
	CMD_ADD_DEV,							//�����豸
	CMD_MOD_DEV,							//�޸�
	CMD_DEL_DEV,							//ɾ��
	CMD_MOD_PSW,							//�޸��û�����
	CMD_GET_MGS_INFO,						//�õ�������Ϣ
	CMD_GET_NETWORK,				
	CMD_SET_NETWORK,						//��������
	CMD_GET_OPTLOG,							//��ȡ������־
	CMD_TOOLS_DEL_OPTLOG,
	CMD_GET_ALARMLOG,
	CMD_UPD_DEV_INFO,						//�����豸��Ϣ	
	CMD_MOD_CHN,							//�޸�ͨ����Ϣ
	CMD_UPD_OPTALM,							//���±�������
}CMD_NORMAL;

//��������
typedef enum
{
	TYPE_DISK_FULL=0,		//Ӳ����
	TYPE_SIGNAL_LOSS,		//�źŶ�ʧ
	TYPE_MOTION_DETECT,		//�ƶ����
	TYPE_IO,				//��������
	TYPE_BLOCK, 			//�ڵ�
	TYPE_MODEL_NOT_MATCH,	//��ʽ��ƥ��
	TYPE_DISK_NO_FORMAT,	//Ӳ��δ��ʽ��
	TYPE_READ_WRITE_DISK,	//��дӲ�̳���
	TYPE_ILLEGAL_ACCESS,	//�Ƿ�����
	TYPE_UNKANOWN,			//δ֪����
}ALARM_TYPE;

//--------------------------------------------------------------
//�������������� 
typedef enum
{
	HIK_ERR_NOERROR=0,		
	HIK_ERR_PASSWORD_ERROR, 	
	HIK_ERR_NOENOUGHPRI,		
	HIK_ERR_NOINIT, 		
	HIK_ERR_CHANNEL_ERROR, 	
	HIK_ERR_OVER_MAXLINK, 		
	HIK_ERR_VERSIONNOMATCH, 	
	HIK_ERR_NETWORK_FAIL_CONNECT, 	
	HIK_ERR_NETWORK_SEND_ERROR, 	
	HIK_ERR_NETWORK_RECV_ERROR, 	
	HIK_ERR_NETWORK_RECV_TIMEOUT, 	
	HIK_ERR_NETWORK_ERRORDATA, 	
	HIK_ERR_ORDER_ERROR, 		
	HIK_ERR_OPERNOPERMIT, 		
	HIK_ERR_COMMANDTIMEOUT, 	
	HIK_ERR_ERRORSERIALPORT, 	
	HIK_ERR_ERRORALARMPORT, 	
	HIK_ERR_PARAMETER_ERROR, 	
	HIK_ERR_CHAN_EXCEPTION, 	
	HIK_ERR_NODISK	, 		
	HIK_ERR_ERRORDISKNUM, 		
	HIK_ERR_DISK_FULL, 		
	HIK_ERR_DISK_ERROR, 		
	HIK_ERR_NOSUPPORT, 		
	HIK_ERR_BUSY, 			
	HIK_ERR_MODIFY_FAIL, 		
	HIK_ERR_PASSWORD_FORMAT_ERROR, 
	HIK_ERR_DISK_FORMATING, 	
	HIK_ERR_DVRNORESOURCE, 	
	HIK_ERR_DVROPRATEFAILED, 	
	HIK_ERR_OPENHOSTSOUND_FAIL, 	
	HIK_ERR_DVRVOICEOPENED, 	
	HIK_ERR_TIMEINPUTERROR, 	
	HIK_ERR_NOSPECFILE, 		
	HIK_ERR_CREATEFILE_ERROR, 	
	HIK_ERR_FILEOPENFAIL, 		
	HIK_ERR_OPERNOTFINISH, 	
	HIK_ERR_GETPLAYTIMEFAIL, 	
	HIK_ERR_PLAYFAIL, 		
	HIK_ERR_FILEFORMAT_ERROR, 	
	HIK_ERR_DIR_ERROR, 		
	HIK_ERR_ALLOC_RESOUCE_ERROR, 	
	HIK_ERR_AUDIO_MODE_ERROR, 	
	HIK_ERR_NOENOUGH_BUF, 		
	HIK_ERR_CREATESOCKET_ERROR, 	
	HIK_ERR_SETSOCKET_ERROR, 	
	HIK_ERR_MAX_NUM, 		
	HIK_ERR_USERNOTEXIST, 		
	HIK_ERR_WRITEFLASHERROR, 	
	HIK_ERR_UPGRADEFAIL, 		
	HIK_ERR_CARDHAVEINIT, 		
	HIK_ERR_PLAYERFAILED, 		
	HIK_ERR_MAX_USERNUM, 		
	HIK_ERR_GETLOCALIPANDMACFAIL, 	
	HIK_ERR_NOENCODEING, 		
	HIK_ERR_IPMISMATCH, 		
	HIK_ERR_MACMISMATCH, 		
	HIK_ERR_UPGRADELANGMISMATCH, 	
	HIK_ERR_DDRAWDEVICENOSUPPORT, 

	HIK_EXCEPTION_AUDIOEXCHANGE=0x8001,	
	HIK_EXCEPTION_ALARM, 		
	HIK_EXCEPTION_PREVIEW, 		
	HIK_EXCEPTION_SERIAL, 		
#ifdef WIN32                  
	HIK_EXCEPTION_RECONNECT, 		
#endif

}HIK_ERROR;

//�󻪴�����Ϣ
#define  ZERO_16	0x00000000
typedef enum
{
	DHD_ERR_ERROR=-1,
	DHD_ERR_NOERROR=0,
	DHD_ERR_SYSTEM_ERROR,		
	DHD_ERR_NETWORK_ERROR,		
	DHD_ERR_DEV_VER_NOMATCH,		
	DHD_ERR_INVALID_HANDLE,		
	DHD_ERR_OPEN_CHANNEL_ERROR,	
	DHD_ERR_CLOSE_CHANNEL_ERROR,	
	DHD_ERR_ILLEGAL_PARAM,		
	DHD_ERR_SDK_INIT_ERROR,		
	DHD_ERR_SDK_UNINIT_ERROR,		
	DHD_ERR_RENDER_OPEN_ERROR,		
	DHD_ERR_DEC_OPEN_ERROR,		
	DHD_ERR_DEC_CLOSE_ERROR,		
	DHD_ERR_MULTIPLAY_NOCHANNEL,	
	DHD_ERR_TALK_INIT_ERROR, 		
	DHD_ERR_TALK_NOT_INIT,		
	DHD_ERR_TALK_SENDDATA_ERROR, 	
	DHD_ERR_REAL_ALREADY_SAVING, 	
	DHD_ERR_NOT_SAVING, 		
	DHD_ERR_OPEN_FILE_ERROR, 		
	DHD_ERR_PTZ_SET_TIMER_ERROR, 	
	DHD_ERR_RETURN_DATA_ERROR, 	
	DHD_ERR_INSUFFICIENT_BUFFER, 	
	DHD_ERR_NOT_SUPPORTED, 		
	DHD_ERR_NO_RECORD_FOUND, 		
	DHD_ERR_NOT_AUTHORIZED, 		
	DHD_ERR_NOT_NOW, 			
	DHD_ERR_NO_TALK_CHANNEL, 		
	DHD_ERR_NO_AUDIO, 			
	DHD_ERR_NO_INIT, 			
	DHD_ERR_DOWNLOAD_END, 		
	DHD_ERR_EMPTY_LIST, 		
	DHD_ERR_ERROR_GETCFG_SYSATTR, 	
	DHD_ERR_ERROR_GETCFG_SERIAL, 	
	DHD_ERR_ERROR_GETCFG_GENERAL, 	
	DHD_ERR_ERROR_GETCFG_DSPCAP, 	
	DHD_ERR_ERROR_GETCFG_NETCFG, 	
	DHD_ERR_ERROR_GETCFG_CHANNAME, 	
	DHD_ERR_ERROR_GETCFG_VIDEO, 	
	DHD_ERR_ERROR_GETCFG_RECORD, 	
	DHD_ERR_ERROR_GETCFG_PRONAME, 	
	DHD_ERR_ERROR_GETCFG_FUNCNAME, 	
	DHD_ERR_ERROR_GETCFG_485DECODER, 	
	DHD_ERR_ERROR_GETCFG_232COM, 	
	DHD_ERR_ERROR_GETCFG_ALARMIN, 	
	DHD_ERR_ERROR_GETCFG_ALARMDET, 	
	DHD_ERR_ERROR_GETCFG_SYSTIME, 	
	DHD_ERR_ERROR_GETCFG_PREVIEW, 	
	DHD_ERR_ERROR_GETCFG_AUTOMT, 	
	DHD_ERR_ERROR_GETCFG_VIDEOMTRX, 	
	DHD_ERR_ERROR_GETCFG_COVER, 	

	DHD_ERR_ERROR_SETCFG_GENERAL=DHD_ERR_ERROR_GETCFG_COVER+5, 	
	DHD_ERR_ERROR_SETCFG_NETCFG, 	
	DHD_ERR_ERROR_SETCFG_CHANNAME,	
	DHD_ERR_ERROR_SETCFG_VIDEO, 	
	DHD_ERR_ERROR_SETCFG_RECORD, 	
	DHD_ERR_ERROR_SETCFG_485DECODER, 	
	DHD_ERR_ERROR_SETCFG_232COM, 	
	DHD_ERR_ERROR_SETCFG_ALARMIN, 	
	DHD_ERR_ERROR_SETCFG_ALARMDET,	
	DHD_ERR_ERROR_SETCFG_SYSTIME, 	
	DHD_ERR_ERROR_SETCFG_PREVIEW, 	
	DHD_ERR_ERROR_SETCFG_AUTOMT, 	
	DHD_ERR_ERROR_SETCFG_VIDEOMTRX, 	
	DHD_ERR_ERROR_SETCFG_COVER=DHD_ERR_ERROR_SETCFG_VIDEOMTRX+2, 	
	DHD_ERR_AUDIOENCODE_NOTINIT, 	
	DHD_ERR_DATA_TOOLONGH, 		
	DHD_ERR_UNSUPPORTED, 		
	DHD_ERR_DEVICE_BUSY, 		
	DHD_ERR_SERVER_STARTED, 		
	DHD_ERR_SERVER_STOPPED, 		
	DHD_ERR_LISTER_INCORRECT_SERIAL=DHD_ERR_SERVER_STOPPED+5, 	
	DHD_ERR_LOGIN_ERROR_PASSWORD=100, 	
	DHD_ERR_LOGIN_ERROR_USER, 		
	DHD_ERR_LOGIN_ERROR_TIMEOUT, 	
	DHD_ERR_LOGIN_ERROR_RELOGGIN, 	
	DHD_ERR_LOGIN_ERROR_LOCKED, 	
	DHD_ERR_LOGIN_ERROR_BLACKLIST, 	
	DHD_ERR_LOGIN_ERROR_BUSY, 		
	DHD_ERR_LOGIN_ERROR_CONNECT, 	
	DHD_ERR_LOGIN_ERROR_NETWORK, 	
	DHD_ERR_RENDER_SOUND_ON_ERROR=120, 	
	DHD_ERR_RENDER_SOUND_OFF_ERROR, 	
	DHD_ERR_RENDER_SET_VOLUME_ERROR, 	
	DHD_ERR_RENDER_ADJUST_ERROR, 	
	DHD_ERR_RENDER_PAUSE_ERROR, 	
	DHD_ERR_RENDER_SNAP_ERROR, 	
	DHD_ERR_RENDER_STEP_ERROR, 	
	DHD_ERR_RENDER_FRAMERATE_ERROR, 	
	DHD_ERR_GROUP_EXIST=140, 		
	DHD_ERR_GROUP_NOEXIST, 		
	DHD_ERR_GROUP_RIGHTOVER, 		
	DHD_ERR_GROUP_HAVEUSER, 		
	DHD_ERR_GROUP_RIGHTUSE, 		
	DHD_ERR_GROUP_SAMENAME, 		
	DHD_ERR_USER_EXIST, 		
	DHD_ERR_USER_NOEXIST, 		
	DHD_ERR_USER_RIGHTOVER, 		
	DHD_ERR_USER_PWD, 			
	DHD_ERR_USER_FLASEPWD, 		
	DHD_ERR_USER_NOMATCHING, 		
	DHD_ERR_CONFIG_DEVBUSY=999, 		
	DHD_ERR_CONFIG_DATAILLEGAL,
}DHD_ERROR;

//�����Ӵ������Ϣ
typedef enum
{
	AIP_ERR_HANDLE=0x00000010, 		
	AIP_ERR_PARAM, 		
	AIP_ERR_CREATETHREAD, 	
	AIP_ERR_CREATESOCKET, 	
	AIP_ERR_OUTMEMORY, 		
	AIP_ERR_MORECHANNEL,

	AIP_ERR_BEGINCONNECT=0x00001000, 	
	AIP_ERR_CONNECTSUCCESS, 	
	AIP_ERR_NETWORK, 		
	AIP_ERR_CONNECTERROR, 	
	AIP_ERR_CONNECTERROR_1000, 	
	AIP_ERR_SERVERSTOP, 	
	AIP_ERR_SERVERSTOP_1000, 	
	AIP_ERR_TIMEOUT, 		
	AIP_ERR_TIMEOUT_1000, 	
	AIP_ERR_SENDDATA, 		
	AIP_ERR_SENDDATA_1000, 	
	AIP_ERR_RECVDATA, 		
	AIP_ERR_RECVDATA_1000,

	AIP_ERR_CLOSECONNECT=0x00010000, 	
	AIP_ERR_SERVERNOSTART, 	
	AIP_ERR_SERVERERROR, 	
	AIP_ERR_CHANNELLIMIT, 	
	AIP_ERR_SERVERLIMIT, 	
	AIP_ERR_SERVERREFUSE, 	
	AIP_ERR_IPLIMIT, 		
	AIP_ERR_PORTLIMIT, 		
	AIP_ERR_TYPEERROR, 		
	AIP_ERR_USERERROR, 		
	AIP_ERR_PASSWORDERROR, 	
	AIP_ERR_DONTTURN, 		
	AIP_ERR_VERSION=0x00100000, 
}AIP_ERROR;

//�ƴ������Ϣ
typedef enum
{
	KEDA_ERR_SUCCESS			  =    (0),			   // �ɹ�
	KEDA_ERR_INVALID              =    (-1)  ,         // �ڲ���̬�����������,û�г�ʼ������,������ͨ��ָ�뷵��
	KEDA_ERR_SIZE                 =    (-2)  ,         // ��С����ȷ     
	KEDA_ERR_TYPE                 =    (-3)  ,         // ���Ͳ���ȷ
	KEDA_ERR_RANGE                =    (-4)  ,         // ��Χ����ȷ
	KEDA_ERR_HANDLE               =    (-5)  ,         // ��Ч��handle
	KEDA_ERR_OVERCAP              =    (-6)  ,         // �豸������֧��
	KEDA_ERR_TIMEOUT              =    (-7)  ,         // ��ʱ����
	KEDA_ERR_STATE                =    (-8)  ,         // ״̬����ǰ������������
	KEDA_ERR_SPACE                =    (-9)  ,         // �ռ䲻��
	KEDA_ERR_PATH                 =    (-10) ,         // �����·��
	KEDA_ERR_LACK_RESOURECE       =    (-11) ,         // ��Դ����
	KEDA_ERR_MSG_SEQUENCE         =    (-12) ,         // ��Ϣ���д���
	KEDA_ERR_DLL_LINK             =    (-13) ,         // dll���Ӵ���
	KEDA_ERR_LOCALFILE_OPEN       =    (-14) ,         // û�д򿪱����ļ�
	KEDA_ERR_LOCALFILE_PLAY       =    (-15) ,         // û�в��ű����ļ�
	KEDA_ERR_PU_ERR               =    (-16) ,         // ǰ�˷��ش���
	KEDA_ERR_NO_DLL_CAMERA        =    (-17) ,         // û��camera.dll��
	KEDA_ERR_CAMERA_TYPE          =    (-18) ,         // ����ͷ���Ͳ���ȷ
	KEDA_ERR_GET_CAMERA_OPT_CODE  =    (-19) ,         // ��ȡ����ͷ������ʧ��
	KEDA_ERR_FILE_OPT             =    (-20) ,         // �ļ�����ʧ��
	KEDA_ERR_PLAYER_HANDLE        =    (-21) ,         // handle�����palyer�������Ӧ
	KEDA_ERR_PALYER_INVALID       =    (-22) ,         // player��Ч
	KEDA_ERR_PALYER_REC           =    (-23) ,         // player����¼��
	KEDA_ERR_PALYER_PLAY          =    (-24) ,         // player���ڲ�����Ƶ
	KEDA_ERR_SEARCH               =    (-25) ,         // ��������
	KEDA_ERR_CREATE_BMP           =    (-26) ,         // ����λͼʧ��
	KEDA_ERR_CONNECT_EXIST        =    (-27) ,         // �����Ѿ�����
	KEDA_ERR_CONNECT_PU           =    (-28) ,         // �����豸ʧ��
	KEDA_ERR_REQ_RECV_AUDIO       =    (-29) ,         // ����ǰ�˽�����Ƶʧ��
	KEDA_ERR_OSP                  =    (-30) ,         // Osp�齨����
	KEDA_ERR_VSIP                 =    (-31) ,         // Э��ջ�齨����
	KEDA_ERR_LICENSE              =    (-32) ,         // LICENSE����
	KEDA_ERR_ENC                  =    (-40) ,         // ����������
	KEDA_ERR_DEC                  =    (-41) ,         // ����������
	KEDA_ERR_RPPLAY               =    (-50) ,         // ¼������
	KEDA_ERR_RPDOWNLOAD           =    (-51) ,         // ¼�����ؿ����
	KEDA_ERR_CAMERA               =    (-60) ,         // ץ�Ŀ����
	KEDA_ERR_PICISDOWN            =    (-61) ,         // ��ǰ��������ͼƬ
	KEDA_ERR_PICISUPLOAD          =    (-62) ,         // ��ǰ�����ϴ�ͼƬ
	KEDA_ERR_DEC_ICOPEN           =    (-100),         // ���ؽ������쳣
	KEDA_ERR_SRART                =    (-100),       
	KEDA_ERR_LOGIN_OTHER          =    (KEDA_ERR_SRART+11), // ��½��������
	KEDA_ERR_LOGIN_TIMEOUT        =    (KEDA_ERR_SRART+12), // ��½��ʱ
	KEDA_ERR_LOGIN_PASSWORD       =    (KEDA_ERR_SRART+13), // �û��������벻��ȷ
	KEDA_ERR_LOGIN_ADMINLOGIN     =    (KEDA_ERR_SRART+14), // ����Ա�Ѿ���¼
	KEDA_ERR_LOGIN_MAX_USER       =    (KEDA_ERR_SRART+15), // USER�û���¼�ѵ����ֵ
	KEDA_ERR_LOGIN_MAX_LOGIN      =    (KEDA_ERR_SRART+16), // �û���¼�����ѵ����ֵ
	KEDA_ERR_LOGIN_NOT_CONN       =    (KEDA_ERR_SRART+17), // û������
}KEDA_ERROR;

//�ƺӴ�����Ϣ
typedef enum 
{
	HHS_ERR_SUCCESS,				//�����ɹ�
	HHS_ERR_FAILURE,				//����ʧ��
	HHS_ERR_REFUSE_REQ,				//���󱻾ܾ�
	HHS_ERR_USER_FULL,				//��¼�û�����
	HHS_ERR_PREVIEW_FULL,			//Ԥ���û�����
	HHS_ERR_TASK_FULL,				//ϵͳ����æ�����᳢������
	HHS_ERR_CHANNEL_NOT_EXIST,		//Ҫ�򿪵�ͨ�������ڻ�����
	HHS_ERR_DEVICE_NAME,			//�򿪵��豸������
	HHS_ERR_IS_TALKING,				//���ڶԽ�
	HHS_ERR_QUEUE_FAILUE,			//���г���
	HHS_ERR_USER_PASSWORD,			//�û����������ϵͳ��ƥ��
	HHS_ERR_SHARE_SOCKET,			//socket ����
	HHS_ERR_RELAY_NOT_OPEN,			//ת������ķ���δ��
	HHS_ERR_RELAY_MULTI_PORT,		//ת���ಥ�˿ڴ���
	HHS_ERR_VIEWPUSH_CHANNEL_USING,	//��Ƶ�����ͨ���Ѿ���ռ��
	HHS_ERR_VIEWPUSH_DECODE_TYPE,	//��Ƶ����ͨ���Ľ����ʽ����0ͨ��(4cif,2cif,cif),1ͨ��(2cif,cif),2ͨ��(cif),3ͨ��(cif)
	HHS_ERR_AUTO_LINK_FAILURE,		//ת�����Զ�����ʧ��
	HHS_ERR_NOT_LOGON,				//�豸��δ��¼
	HHS_ERR_IS_SETTING,				//�豸��������
	HHS_ERR_COMMAND_FAILURE,		//����ʧ��
	HHS_ERR_RESTRICT_ID,			//IDʹ������

	HHS_ERR_INVALID_PARAMETER=100,	//���������Ч
	HHS_ERR_LOGON_FAILURE,			//��¼ʧ��
	HHS_ERR_TIME_OUT,				//������ʱ
	HHS_ERR_SOCKET_ERR,				//SOCKET����
	HHS_ERR_NOT_LINKSERVER,			//��δ���ӷ�����
	HHS_ERR_BUFFER_EXTCEED_LIMIT,	//ʹ�û��峬������	
	HHS_ERR_LOW_PRIORITY,			//����Ȩ�޲���
	HHS_ERR_BUFFER_SMALL,			//����̫С
	HHS_ERR_IS_BUSY,				//ϵͳ������æ
	HHS_ERR_UPDATE_FILE,			//�����ļ�����
	HHS_ERR_UPDATE_UNMATCH,			//�����ļ��ͻ�����ƥ��
	HHS_ERR_PORT_INUSE,				//�˿ڱ�ռ��
	HHS_ERR_RELAY_DEVICE_EXIST,		//�豸���Ѿ�����
	HHS_ERR_CONNECT_REFUSED,		//����ʱ���ܾ�
	HHS_ERR_PROT_NOT_SURPPORT,		//��֧�ָ�Э��

	HHS_ERR_FILE_OPEN_ERR,          //���ļ�ʧ��
	HHS_ERR_FILE_SEEK_ERR,          //fseekʧ�� 
	HHS_ERR_FILE_WRITE_ERR,         //fwriteʧ�� 
	HHS_ERR_FILE_READ_ERR,          //freadʧ�� 
	HHS_ERR_FILE_CLOSING,           //���ڹر��ļ� 

	HHS_ERR_ALLOC_BUF_ERR,
}HHS_ERROR;

//���������Ϣ
typedef enum 
{
	HBT_ERR_SDVR_NOERROR					=	0,		//û�д���
	HBT_ERR_SDVR_PASSWORD_ERROR 			=	-1,		//�û����������
	HBT_ERR_SDVR_NOENOUGHPRI 				=	-2,		//Ȩ�޲���
	HBT_ERR_SDVR_NOINIT 					=	-3,		//û�г�ʼ��
	HBT_ERR_SDVR_CHANNEL_ERROR 				=	-4,		//ͨ���Ŵ���
	HBT_ERR_SDVR_OVER_MAXLINK 				=	-5,		//���ӵ�DVR�Ŀͻ��˸����������
	HBT_ERR_SDVR_VERSIONNOMATCH				=	-6,		//�汾��ƥ��
	HBT_ERR_SDVR_NETWORK_FAIL_CONNECT		=	-7,		//���ӷ�����ʧ��
	HBT_ERR_SDVR_NETWORK_SEND_ERROR			=	-8,		//�����������ʧ��
	HBT_ERR_SDVR_NETWORK_RECV_ERROR			=	-9,		//�ӷ�������������ʧ��
	HBT_ERR_SDVR_NETWORK_RECV_TIMEOUT		=	-10,	//�ӷ������������ݳ�ʱ
	HBT_ERR_SDVR_NETWORK_ERRORDATA			=	-11,	//���͵���������
	HBT_ERR_SDVR_ORDER_ERROR				=	-12,	//���ô������
	HBT_ERR_SDVR_OPERNOPERMIT				=	-13,	//�޴�Ȩ��                                                      
	HBT_ERR_SDVR_COMMANDTIMEOUT				=	-14,	//DVR����ִ�г�ʱ                                                     
	HBT_ERR_SDVR_ERRORSERIALPORT			=	-15,	//���ںŴ���                                                      
	HBT_ERR_SDVR_ERRORALARMPORT				=	-16,	//�����˿ڴ���                                                      
	HBT_ERR_SDVR_PARAMETER_ERROR 			=	-17,	//��������                                                      
	HBT_ERR_SDVR_CHAN_EXCEPTION				=	-18, 	//������ͨ�����ڴ���״̬                                              
	HBT_ERR_SDVR_NODISK						=	-19, 	//û��Ӳ��
	HBT_ERR_SDVR_ERRORDISKNUM				=	-20, 	//Ӳ�̺Ŵ���                                                      
	HBT_ERR_SDVR_DISK_FULL					=	-21, 	//������Ӳ����                                                      
	HBT_ERR_SDVR_DISK_ERROR				 	=	-22, 	//������Ӳ�̳���                                                      
	HBT_ERR_SDVR_NOSUPPORT					=	-23, 	//��������֧��                                                      
	HBT_ERR_SDVR_BUSY						=	-24, 	//������æ                                                      
	HBT_ERR_SDVR_MODIFY_FAIL				=	-25, 	//�������޸Ĳ���                                                      
	HBT_ERR_SDVR_PASSWORD_FORMAT_ERROR		=	-26, 	//���������ʽ����                                                    
	HBT_ERR_SDVR_DISK_FORMATING				=	-27, 	//Ӳ�����ڸ�ʽ����ȷ                                                  
	HBT_ERR_SDVR_DVRNORESOURCE				=	-28, 	//DVR��Դ����
	HBT_ERR_SDVR_DVROPRATEFAILED			=	-29,	//DVR����ʧ�ܣ�������������                                        
	HBT_ERR_SDVR_OPENHOSTSOUND_FAIL 		=	-30,	//��PC����ʧ��                                                      
	HBT_ERR_SDVR_DVRVOICEOPENED 			=	-31,	//�����������Խ�                                                      
	HBT_ERR_SDVR_TIMEINPUTERROR				=	-32,	//ʱ�����벻��ȷ                                                      
	HBT_ERR_SDVR_NOSPECFILE					=	-33,	//�ط�ʱ������û��ռ��                                                
	HBT_ERR_SDVR_CREATEFILE_ERROR			=	-34, 	//�����ļ�����                                                      
	HBT_ERR_SDVR_FILEOPENFAIL				=	-35,	//���ļ�������ָ�����ļ�                                          
	HBT_ERR_SDVR_OPERNOTFINISH				=	-36,	//�ϴεĲ�����û                                                      
	HBT_ERR_SDVR_GETPLAYTIMEFAIL			=	-37,	//��ȡ��ǰ���ŵ�                                                      
	HBT_ERR_SDVR_PLAYFAIL					=	-38,	//���ų��������                                                
	HBT_ERR_SDVR_FILEFORMAT_ERROR			=	-39,	//�ļ���ʽ����ȷʱ�����                                              
	HBT_ERR_SDVR_DIR_ERROR					=	-40, 	//·������                                                      
	HBT_ERR_SDVR_ALLOC_RESOUCE_ERROR		=	-41,	//��Դ�������                                                      
	HBT_ERR_SDVR_AUDIO_MODE_ERROR			=	-42, 	//����ģʽ����                                                      
	HBT_ERR_SDVR_NOENOUGH_BUF				=	-43, 	//������̫С                                                      
	HBT_ERR_SDVR_CREATESOCKET_ERROR		 	=	-44, 	//����SOCKET����                                                      
	HBT_ERR_SDVR_SETSOCKET_ERROR			=	-45, 	//����SOCKET����                                                      
	HBT_ERR_SDVR_MAX_NUM					=	-46, 	//�����ﵽ���                                                      
	HBT_ERR_SDVR_USERNOTEXIST				=	-47, 	//�û�������                                                      
	HBT_ERR_SDVR_WRITEFLASHERROR			=	-48,	//дFLASH����                                                      
	HBT_ERR_SDVR_UPGRADEFAIL				=	-49,	//DVR����ʧ��                                                      
	HBT_ERR_SDVR_CARDHAVEINIT				=	-50,	//���뿨�Ѿ���ʼ                                                      
	HBT_ERR_SDVR_PLAYERFAILED				=	-51, 	//�������д���                                                      
	HBT_ERR_SDVR_MAX_USERNUM				=	-52,	//�û����ﵽ��󻯹�                                                  
	HBT_ERR_SDVR_GETLOCALIPANDMACFAIL		=	-53,	//��ÿͻ��˵�IP                                                      
	HBT_ERR_SDVR_NOENCODEING				=	-54, 	//��ͨ��û�б���                                                      
	HBT_ERR_SDVR_IPMISMATCH					=	-55, 	//IP��ַ��ƥ���ַ�������ַʧ��                                    
	HBT_ERR_SDVR_MACMISMATCH				=	-56, 	//MAC��ַ��ƥ��                                                      
	HBT_ERR_SDVR_UPGRADELANGMISMATCH		=	-57, 	//�����ļ����Բ�                                                      
	HBT_ERR_SDVR_USERISALIVE				=	-58, 	//�û��ѵ�½                                                       
	HBT_ERR_SDVR_IPERR						=	-101,	//IP��ַ��ƥ��ƥ��                                                  
	HBT_ERR_SDVR_MACERR			    		=	-102,	//MAC��ַ��ƥ��                                                      
	HBT_ERR_SDVR_PSWERR			    		=	-103,	//MAC��ַ��ƥ��                                                      
	HBT_ERR_SDVR_USERERR			    	=	-104,	//MAC��ַ��ƥ��                                                      
	HBT_ERR_SDVR_USERISFULL			    	=	-105,	//MAC��ַ��ƥ��                                                      
	HBT_ERR_SDVR_NO_PERMISSION				=	0xf0,   //�û�û��Ȩ��                                                      
	HBT_ERR_SDVR_FILE_SUCCESS				=	1000,	//����ļ���Ϣ                                                      
	HBT_ERR_SDVR_FILE_NOFIND				=	1001,	//û���ļ�                                                      
	HBT_ERR_SDVR_ISFINDING					=	1002,	//���ڲ����ļ�                                                      
	HBT_ERR_SDVR_NOMOREFILE					=	1003,	//�����ļ�ʱû��                                                      
	HBT_ERR_SDVR_FILE_EXCEPTION				=	1004,	//�����ļ�ʱ�쳣
}HBT_ERROR;

//����ǰ�������Ϣ
typedef enum
{
	YAA_ERR_SDVR_NOERROR					=	0,		//û�д���
	YAA_ERR_SDVR_PASSWORD_ERROR 			=	-1,		//�û����������
	YAA_ERR_SDVR_NOENOUGHPRI 				=	-2,		//Ȩ�޲���
	YAA_ERR_SDVR_NOINIT 					=	-3,		//û�г�ʼ��
	YAA_ERR_SDVR_CHANNEL_ERROR 				=	-4,		//ͨ���Ŵ���
	YAA_ERR_SDVR_OVER_MAXLINK 				=	-5,		//���ӵ�DVR�Ŀͻ��˸����������
	YAA_ERR_SDVR_VERSIONNOMATCH				=	-6,		//�汾��ƥ��
	YAA_ERR_SDVR_NETWORK_FAIL_CONNECT		=	-7,		//���ӷ�����ʧ��
	YAA_ERR_SDVR_NETWORK_SEND_ERROR			=	-8,		//�����������ʧ��
	YAA_ERR_SDVR_NETWORK_RECV_ERROR			=	-9,		//�ӷ�������������ʧ��
	YAA_ERR_SDVR_NETWORK_RECV_TIMEOUT		=	-10,	//�ӷ������������ݳ�ʱ
	YAA_ERR_SDVR_NETWORK_ERRORDATA			=	-11,	//���͵���������
	YAA_ERR_SDVR_ORDER_ERROR				=	-12,	//���ô������
	YAA_ERR_SDVR_OPERNOPERMIT				=	-13,	//�޴�Ȩ��                                                      
	YAA_ERR_SDVR_COMMANDTIMEOUT				=	-14,	//DVR����ִ�г�ʱ                                                     
	YAA_ERR_SDVR_ERRORSERIALPORT			=	-15,	//���ںŴ���                                                      
	YAA_ERR_SDVR_ERRORALARMPORT				=	-16,	//�����˿ڴ���                                                      
	YAA_ERR_SDVR_PARAMETER_ERROR 			=	-17,	//��������                                                      
	YAA_ERR_SDVR_CHAN_EXCEPTION				=	-18, 	//������ͨ�����ڴ���״̬                                              
	YAA_ERR_SDVR_NODISK						=	-19, 	//û��Ӳ��
	YAA_ERR_SDVR_ERRORDISKNUM				=	-20, 	//Ӳ�̺Ŵ���                                                      
	YAA_ERR_SDVR_DISK_FULL					=	-21, 	//������Ӳ����                                                      
	YAA_ERR_SDVR_DISK_ERROR				 	=	-22, 	//������Ӳ�̳���                                                      
	YAA_ERR_SDVR_NOSUPPORT					=	-23, 	//��������֧��                                                      
	YAA_ERR_SDVR_BUSY						=	-24, 	//������æ                                                      
	YAA_ERR_SDVR_MODIFY_FAIL				=	-25, 	//�������޸Ĳ���                                                      
	YAA_ERR_SDVR_PASSWORD_FORMAT_ERROR		=	-26, 	//���������ʽ����                                                    
	YAA_ERR_SDVR_DISK_FORMATING				=	-27, 	//Ӳ�����ڸ�ʽ����ȷ                                                  
	YAA_ERR_SDVR_DVRNORESOURCE				=	-28, 	//DVR��Դ����
	YAA_ERR_SDVR_DVROPRATEFAILED			=	-29,	//DVR����ʧ�ܣ�������������                                        
	YAA_ERR_SDVR_OPENHOSTSOUND_FAIL 		=	-30,	//��PC����ʧ��                                                      
	YAA_ERR_SDVR_DVRVOICEOPENED 			=	-31,	//�����������Խ�                                                      
	YAA_ERR_SDVR_TIMEINPUTERROR				=	-32,	//ʱ�����벻��ȷ                                                      
	YAA_ERR_SDVR_NOSPECFILE					=	-33,	//�ط�ʱ������û��ռ��                                                
	YAA_ERR_SDVR_CREATEFILE_ERROR			=	-34, 	//�����ļ�����                                                      
	YAA_ERR_SDVR_FILEOPENFAIL				=	-35,	//���ļ�������ָ�����ļ�                                          
	YAA_ERR_SDVR_OPERNOTFINISH				=	-36,	//�ϴεĲ�����û                                                      
	YAA_ERR_SDVR_GETPLAYTIMEFAIL			=	-37,	//��ȡ��ǰ���ŵ�                                                      
	YAA_ERR_SDVR_PLAYFAIL					=	-38,	//���ų��������                                                
	YAA_ERR_SDVR_FILEFORMAT_ERROR			=	-39,	//�ļ���ʽ����ȷʱ�����                                              
	YAA_ERR_SDVR_DIR_ERROR					=	-40, 	//·������                                                      
	YAA_ERR_SDVR_ALLOC_RESOUCE_ERROR		=	-41,	//��Դ�������                                                      
	YAA_ERR_SDVR_AUDIO_MODE_ERROR			=	-42, 	//����ģʽ����                                                      
	YAA_ERR_SDVR_NOENOUGH_BUF				=	-43, 	//������̫С                                                      
	YAA_ERR_SDVR_CREATESOCKET_ERROR		 	=	-44, 	//����SOCKET����                                                      
	YAA_ERR_SDVR_SETSOCKET_ERROR			=	-45, 	//����SOCKET����                                                      
	YAA_ERR_SDVR_MAX_NUM					=	-46, 	//�����ﵽ���                                                      
	YAA_ERR_SDVR_USERNOTEXIST				=	-47, 	//�û�������                                                      
	YAA_ERR_SDVR_WRITEFLASHERROR			=	-48,	//дFLASH����                                                      
	YAA_ERR_SDVR_UPGRADEFAIL				=	-49,	//DVR����ʧ��                                                      
	YAA_ERR_SDVR_CARDHAVEINIT				=	-50,	//���뿨�Ѿ���ʼ                                                      
	YAA_ERR_SDVR_PLAYERFAILED				=	-51, 	//�������д���                                                      
	YAA_ERR_SDVR_MAX_USERNUM				=	-52,	//�û����ﵽ��󻯹�                                                  
	YAA_ERR_SDVR_GETLOCALIPANDMACFAIL		=	-53,	//��ÿͻ��˵�IP                                                      
	YAA_ERR_SDVR_NOENCODEING				=	-54, 	//��ͨ��û�б���                                                      
	YAA_ERR_SDVR_IPMISMATCH					=	-55, 	//IP��ַ��ƥ���ַ�������ַʧ��                                    
	YAA_ERR_SDVR_MACMISMATCH				=	-56, 	//MAC��ַ��ƥ��                                                      
	YAA_ERR_SDVR_UPGRADELANGMISMATCH		=	-57, 	//�����ļ����Բ�                                                      
	YAA_ERR_SDVR_USERISALIVE				=	-58, 	//�û��ѵ�½                                                       
	YAA_ERR_SDVR_IPERR						=	-101,	//IP��ַ��ƥ��ƥ��                                                  
	YAA_ERR_SDVR_MACERR			    		=	-102,	//MAC��ַ��ƥ��                                                      
	YAA_ERR_SDVR_PSWERR			    		=	-103,	//MAC��ַ��ƥ��                                                      
	YAA_ERR_SDVR_USERERR			    	=	-104,	//MAC��ַ��ƥ��                                                      
	YAA_ERR_SDVR_USERISFULL			    	=	-105,	//MAC��ַ��ƥ��                                                      
	YAA_ERR_SDVR_NO_PERMISSION				=	0xf0,   //�û�û��Ȩ��                                                      
	YAA_ERR_SDVR_FILE_SUCCESS				=	1000,	//����ļ���Ϣ                                                      
	YAA_ERR_SDVR_FILE_NOFIND				=	1001,	//û���ļ�                                                      
	YAA_ERR_SDVR_ISFINDING					=	1002,	//���ڲ����ļ�                                                      
	YAA_ERR_SDVR_NOMOREFILE					=	1003,	//�����ļ�ʱû��                                                      
	YAA_ERR_SDVR_FILE_EXCEPTION				=	1004,	//�����ļ�ʱ�쳣
}YAA_ERROR;


//RTSP������Ϣ
typedef enum 
{
	RTSP_ERR_SUCCESS,										//�����ɹ�
	RTSP_ERR_FAILURE,										//����ʧ��

	RTSP_ERR_CONTINUE = 100,								// ����; Continue (all 100 range)
	RTSP_ERR_OK	= 200,										// �ɹ�; OK 
	RTSP_ERR_CREATE = 201,									// �Ѵ���; Created 
	RTSP_ERR_LOW_ON_STORAGE_SPACE = 250,					// �洢�ռ䲻��; Low on Storage Space 
	RTSP_ERR_MULTIPLE_CHOICES = 300,						// �ж��ѡ��; Multiple Choices 
	RTSP_ERR_MOVED_PERMANENTLY = 301,						// �������Ƴ�; Moved Permanently 
	RTSP_ERR_MOVED_TEMPORARILY = 302,						// ����ʱ�Ƴ�; Moved Temporarily 
	RTSP_ERR_SEE_OTHER = 303,								// ������; See Other  
	RTSP_ERR_NOT_MODIFIED = 304,							// û���޸�; Not Modified
	RTSP_ERR_USE_PROXY = 305,								// ʹ�ô���; Use Proxy 
	RTSP_ERR_GOING_AWAY = 350,								// �Ͽ�����; Going Away
	RTSP_ERR_LOAD_BALANCING = 351,							// ���ؾ���; Load Balancing
	RTSP_ERR_BAD_REQUEST = 400,								// ���������; Bad Request 
	RTSP_ERR_UNAUTHORIZED = 401,							// δͨ����֤; Unauthorized 
	RTSP_ERR_PAYMENT_REQUIRED = 402,						// ��Ҫ����; Payment Required 
	RTSP_ERR_FORBIDDEN = 403,								// ��ֹ; Forbidden
	RTSP_ERR_NOT_FOUND = 404,								// û���ҵ�; Not Found
	RTSP_ERR_METHOD_NOT_ALLOWED = 405,						// ������÷���; Method Not Allowed 
	RTSP_ERR_NOT_ACCEPTABLE = 406,							// ������; Not Acceptable 
	RTSP_ERR_PROXY_AUTHENTICATION_REQUIRED = 407,			// ������Ҫ��֤; Proxy Authentication Required
	RTSP_ERR_REQUEST_TIMEOUT = 408,							// ����ʱ; Request Time-out 
	RTSP_ERR_GONE = 410,									// ���ڷ�����; Gone 
	RTSP_ERR_LENGTH_REQUIRED = 411,							// ��Ҫ����; Length Required 
	RTSP_ERR_PRECONDITION_FAILED = 412,						// Ԥ����ʧ��; Precondition Failed
	RTSP_ERR_REQUEST_ENTITY_TOO_LARGE = 413,				// ����ʵ�����; Request Entity Too Large 
	RTSP_ERR_REQUEST_URI_TOO_LARGE = 414,					// ����-URI����; Request-URI Too Large 
	RTSP_ERR_UNSUPPORTED_MEDIA_TYPE = 415,					// ý�����Ͳ�֧��; Unsupported Media Type 
	RTSP_ERR_PARAMETER_NOT_UNDERSTOOD = 451,				// �����˲���; Parameter Not Understood 
	RTSP_ERR_RESERVED = 452,								// �Ҳ�������; reserved 
	RTSP_ERR_NOT_ENOUGH_BANDWIDTH = 453,					// ������; Not Enough Bandwidth 
	RTSP_ERR_SESSION_NOT_FOUND = 454,						// �Ҳ����Ự; Session Not Found 
	RTSP_ERR_METHOD_NOT_VALID_IN_THIS_STATE = 455,			// ��״̬�´˷�����Ч; Method Not Valid in This State 
	RTSP_ERR_HEADER_FIELD_NOT_VALID_FOR_RESOURCE = 456,		// ��ͷ����Ը���Դ��Ч; Header Field Not Valid for Resource
	RTSP_ERR_INVALID_RANGE = 457,							// ��Ч��Χ; Invalid Range 
	RTSP_ERR_PARAMETER_IS_READONLY = 458,					// ������ֻ����; Parameter Is Read-Only
	RTSP_ERR_AGGREGATE_OPERATION_NOT_ALLOWED = 459,			// ������Ͽ���; Aggregate operation not allowed
	RTSP_ERR_ONLY_AGGREGATE_OPERATION_ALLOWED = 460,		// ֻ����Ͽ���; Only aggregate operation allowed
	RTSP_ERR_UNSUPPORTED_TRANSPORT = 461,					// ���䷽ʽ��֧��; Unsupported transport 
	RTSP_ERR_DESTINATION_UNREACHABLE = 462,					// �޷�����Ŀ�ĵ�ַ; Destination unreachable 
	RTSP_ERR_INTERNAL_SERVER_ERROR = 500,					// �������ڲ�����; Internal Server Error 
	RTSP_ERR_NOT_IMPLEMENTED = 501,							// δʵ��; Not Implemented
	RTSP_ERR_BAD_GATEWAY = 502,								// ���ش���; Bad Gateway 
	RTSP_ERR_SERVICE_UNAVAILABLE = 503,						// �޷��õ�����; Service Unavailable
	RTSP_ERR_GATEWAY_TIMEOUT = 504,							// ���س�ʱ; Gateway Time-out
	RTSP_ERR_VERSION_NOT_SUPPORTED = 505,					// ��֧�ִ�RTSP�汾; RTSP Version not supported 
	RTSP_ERR_OPTION_NOT_SUPPORTED= 551,						// ��֧��ѡ��; Option not supported 
}RTSP_ERROR;

//���ΰҵ �����
typedef enum
{
	TIANDY_ERR_NO_DEV = 0x10000001,		//"û���ҵ��豸",				
	TIANDY_ERR_UNLOGON,					//"δ��½",					
	TIANDY_ERR_PARAM_OVER,				//"����Խ��",					
	TIANDY_ERR_REOPERATION,				//"�ظ�����",					
	TIANDY_ERR_WSASTARTUP,				//"WSAStartupִ��ʧ��",		
	TIANDY_ERR_CREATEMSG,				//"������Ϣѭ��ʧ��",			
	TIANDY_ERR_NOSUPPORTRECORD,			//"��֧��ǰ��¼��",			
	TIANDY_ERR_INITCHANNELNET,			//"ͨ�����紴��ʧ��",			
	TIANDY_ERR_CREATEDDRAW,				//"�޷����������DirectDraw",	
	TIANDY_ERR_NO_CHANNEL,				//"ͨ��û�д���",				
	TIANDY_ERR_NO_FUN,					//"�޴˹���",					
	TIANDY_ERR_PARAM_INVALID,			//"������Ч",					
	TIANDY_ERR_DEV_FULL,				//"�豸�б�����",				
	TIANDY_ERR_LOGON,					//"�豸�Ѿ���¼�����ڵ�¼",	
	TIANDY_ERR_CREATECPUTHREAD,			//"����CPU����߳�ʧ��",		
	TIANDY_ERR_CREATEPLAYER,			//"����Playerʧ��",			
	TIANDY_ERR_NOAUTHORITY,				//"Ȩ�޲���",					
	TIANDY_ERR_NOTALK,					//"δ�Խ�",					
	TIANDY_ERR_NOCALLBACK,				//"û���豸�ص�����",			
	TIANDY_ERR_CREATEFILE,				//"�����ļ�ʧ��",				
	TIANDY_ERR_NORECORD,				//"���Ǵӵ�ǰPlayer�����¼��"
	TIANDY_ERR_NOPLAYER,				//"�ж�ӦPlayer",				
	TIANDY_ERR_INITCHANNEL,				//"ͨ��û�г�ʼ��",			
	TIANDY_ERR_NOPLAYING,				//"Playerû�в���",			
	TIANDY_ERR_PARAM_LONG,				//"�ַ����������ȹ���",		
	TIANDY_ERR_INVALID_FILE,			//"�ļ�������Ҫ��",			
	TIANDY_ERR_USER_FULL,				//"�û��б�����",				
	TIANDY_ERR_USER_DEL,				//"��ǰ�û��޷�ɾ��",			
	TIANDY_ERR_CARD_LOAD,				//"���ؿ�dllʧ��",			
	TIANDY_ERR_CARD_CORE,				//"���ؿ��ں�ʧ��",			
	TIANDY_ERR_CARD_COREFILE,			//"���ؿ��ں��ļ�ʧ��",		
	TIANDY_ERR_CARD_INIT,				//"����ʼ��ʧ��",				
	TIANDY_ERR_CARD_COREREAD,			//"��ȡ���ں��ļ�ʧ��",		
	TIANDY_ERR_CHARACTER_LOAD,			//"�����ֿ�ʧ��",				
	TIANDY_ERR_NOCARD,					//"��δ��ʼ��",				
	TIANDY_ERR_SHOW_MODE,				//"��ʾģʽδ����",			
	TIANDY_ERR_FUN_LOAD,				//"����δ����",				
	TIANDY_ERR_CREATE_DOWNLOAD,			//"û�и��������ͨ������",	
	TIANDY_ERR_PROXY_DELACT,			//"ɾ����������ģʽ�ķ�����",	
	TIANDY_ERR_PROXY_HASCONNECT,		//"��������",					
	TIANDY_ERR_PROXY_NOPROXY,			//"����û������",				
	TIANDY_ERR_PROXY_IDENTITY,			//"���Ǵ�������",			
	TIANDY_ERR_CONNECT_MAX,				//"�����Ѿ��������",			
	TIANDY_ERR_NO_DISK,					//"û�йҽӴ洢�豸",			
}TIANDY_ERROR;

// ���ΰҵ��������
typedef enum
{
	TDY_SDVR_SET_DEVICECFG = 100,//�豸����
	TDY_SDVR_GET_DEVICECFG,//�豸����
	TDY_SDVR_SET_NETCFG,//�������
	TDY_SDVR_GET_NETCFG,//�������
	TDY_SDVR_SET_COMPRESSCFG,//ѹ������}
	TDY_SDVR_GET_COMPRESSCFG,//ѹ������}
	TDY_SDVR_SET_RECORDCFG,//¼�����
	TDY_SDVR_GET_RECORDCFG,//¼�����
	TDY_SDVR_SET_DECODERCFG,//����������
	TDY_SDVR_GET_DECODERCFG,//����������
	TDY_SDVR_SET_RS232CFG,//232���ڲ���
	TDY_SDVR_GET_RS232CFG,//232���ڲ���
	TDY_SDVR_SET_ALARMINCFG,//�����������
	TDY_SDVR_GET_ALARMINCFG,//�����������
	TDY_SDVR_SET_ALARMOUTCFG,//�����������
	TDY_SDVR_GET_ALARMOUTCFG,//�����������
	TDY_SDVR_SET_TIMECFG,//����DVRʱ��
	TDY_SDVR_GET_TIMECFG,//����DVRʱ��
	TDY_SDVR_SET_PREVIEWCFG,//Ԥ������
	TDY_SDVR_GET_PREVIEWCFG,//Ԥ������
	TDY_SDVR_SET_VIDEOCFG,//��Ƶ����
	TDY_SDVR_GET_VIDEOCFG,//��Ƶ����
	TDY_SDVR_SET_AUDIOCFG,//��Ƶ����
	TDY_SDVR_GET_AUDIOCFG,//��Ƶ����
	TDY_SDVR_SET_EXCEPTIONCFG,//�쳣����
	TDY_SDVR_GET_EXCEPTIONCFG,//�쳣����
	TDY_SDVR_SET_SHOWSTRING,//�����ַ�����
	TDY_SDVR_GET_SHOWSTRING,//�����ַ�����
	TDY_SDVR_SET_PICCFG_EX,//ͼ�����(SDK_V14��չ����)
	TDY_SDVR_GET_PICCFG_EX,//ͼ�����(SDK_V14��չ����)
	TDY_SDVR_SET_EVENTCOMPCFG,//�¼�����¼���
	TDY_SDVR_GET_EVENTCOMPCFG,//�¼�����¼���
}TIANDY_PARA;

typedef enum
{
	YAA_ERR_NOERROR = 0,		//"�ɹ�"
	YAA_ERR_FAILUAR,			//"ʧ��"
}YAA_ERR;

typedef enum
{
	NCI_ERR_NOERROR = 0,		//"�ɹ�",						
	NCI_ERR_LOGINERR,			//"��¼ʧ��",				    
	NCI_ERR_REALPLAYERR,		//"Ԥ��ʧ��",		
}NCI_ERROR;

typedef enum
{
	JTN_ERR_NOERROR = 0,		//"�ɹ�",						
	JTN_ERR_LOGINERR,			//"��¼ʧ��",				    
	JTN_ERR_REALPLAYERR,		//"Ԥ��ʧ��",		
}JTN_ERROR;

typedef enum
{
	TLV_ERR_NOERROR = 0,		//"�ɹ�",						
	TLV_ERR_LOGINERR,			//"��¼ʧ��",				    
	TLV_ERR_REALPLAYERR,		//"Ԥ��ʧ��",		
}TLV_ERROR;

typedef enum
{
	BSC_ERR_NOERROR = 0,		//"�ɹ�",						
	BSC_ERR_LOGINERR,			//"��¼ʧ��",				    
	BSC_ERR_REALPLAYERR,		//"Ԥ��ʧ��",		
}BSC_ERROR;

typedef enum
{
	XQI_ERR_NOERROR = 0,		//�ɹ�

}XinQi_ERROR;