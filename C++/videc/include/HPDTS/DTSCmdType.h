
//========================================================================================
//��Ȩ����: ��Ȩ����(C) 2008���Ϻ���ƽ��Ϣ�����ɷ����޹�˾
//ϵͳ����: AVCON7
//��������: duquansheng
//��������: 2008-3-31
//����˵��: DTS�����������ͷ���弰ģ�鹲�ò�������
//

#pragma once


#define LOCAL_DEBUG
#undef	LOCAL_DEBUG


#define FILE_TRANS_MAXLENGTH 8*1024	  //ÿ�����Ĵ�С
#define FILE_TRANS_PACKETCOUNT 3      //��һ������ĸ���

#define LOCAL_SESSIONID 1		  //����SessionID
#define DEFNET_SESSIONID 2		  //Ĭ������SessionID
#define  HUBEI_MEDICAL_SDK   //����sdk�汾 Ҫ��������� ����ע��
//�ļ�����ṹ�嶨��
typedef struct tagDtsFileList
{
	uint32_t	ulFileID;
	uint32_t	ulFileLength;
	uint32_t	ulPosition;		
	uint32_t	ulFromSessionID;
	char			chSendUserID[255];
	char			chFileName[128];
	char			chFilePath[255];
	char			chDescription[255];//��Ʒ����
	char			chStartTime[30];

}DTS_FILE_LIST,*PDTS_FILE_LIST;

//��ע���װ����ṹ�嶨��
typedef struct tag_ObjDataBuf
{
	char*			pData;
	int32_t				nLen;
	uint32_t	ulObjId;
}OBJ_DATA_BUF, *POBJ_DATA_BUF;

#ifndef _WIN32
typedef uint32_t	DWORD;
typedef uint32_t	UINT;
typedef uint32_t   COLORREF;
typedef struct tagPOINT
{
	int32_t  x;
	int32_t  y;
} POINT, *PPOINT;
typedef struct tagRECT
{
	int32_t    left;
	int32_t    top;
	int32_t    right;
	int32_t    bottom;
} RECT, *PRECT;
#endif

typedef struct tagDtsTemData
{	
	UINT			nCurPage;	
	UINT			nTotalPage;	
	POINT			ptFile;			//�����ļ��Ĵ�С
	POINT			ptClient;		//��ʾ����Ĵ�С
	UINT			nFlashStatus;	//flash ����״̬
	UINT			nFlashPosition;	//flash ����λ��
	UINT			nDocZoomSize;	//�ĵ��������ű���
	UINT			nDocLeft;		//�ĵ������ƶ���λ��
	UINT			nDocTop;		//�ĵ������ƶ���λ��

}DTS_TEM_DATA,*PDTS_TEM_DATA;

//�ļ�����״̬
enum enumFILESTATUS
{
	FILESTATUS_PAUSE = 0,			//��ͣ
	FILESTATUS_DONEUP,				//�ϴ����
	FILESTATUS_DONELOAD,			//�������
	FILESTATUS_WAIT,				//�ȴ�
	FILESTATUS_TRANSMIT,			//������
};

//DTS��������
enum enumDTSCOMMAND
{	
	WB_COMMAND_DATA = 0,			//�װ�����

	/*�����ļ�������淢�������������������������������������������������������������������*/
	FILE_SEND_ADDFILE,				//��������ļ�
	FILE_SEND_DELETEFILE,			//����ɾ������
	FILE_REQUEST_DOWNLOAD,			//���������ļ�����	
	FILE_PAUSE_DOWNLOAD,			//��ͣ�����ļ�
	FILE_CONTINUE_DOWNLOAD,			//���������ļ�
	FILE_PAUSE_UPLOAD,				//��ͣ�ϴ��ļ�
	FILE_CONTINUE_UPLOAD,			//�����ϴ��ļ�	
	FILE_GET_FILEINFO,				//��ȡ�ļ���Ϣ
	FILE_GET_POSITION,				//�õ��ļ�����λ��
	FILE_REQUEST_FILELIST,			//���������ļ��б�
	/*�����ļ��������ص������������������������������������������������������������������*/
	FILE_INSERT_FILE,				 
	FILE_DELETE_FILE,				//ɾ���ļ���Ϣ
	FILE_UPDATE_STATUS,				//�����ļ�״̬	
	FILE_UPLOAD_ERROR,				//�ϴ��ļ�����
	FILE_DOWNLOAD_ERROR,			//�����ļ�����
	FILE_ONGET_FILEINFO,			//�ص��ļ���Ϣ
	FILE_ONGET_POSITION,			//�ص��ļ�����λ��

	/*�����ļ������ڲ����������������������������������������������������������������������*/
	FILE_GET_FILELIST,				//��ȡ�����ļ��б�
	FILE_ONGET_FILELIST,			//�ص������ļ��б�
	FILE_AUTO_DOWNLOAD,				//�Զ����������ļ�
	FILE_ONREQUEST_DOWNLOAD,		//�յ������ļ�����
	FILE_ONDOWNLOAD_DATA,			//�յ������ļ�����

	/*�����ĵ������ע��أ���������������������������������������������������������������*/
	FS_ADD_OBJ,						//��Ӷ���
	FS_DELETE_OBJ,					//ɾ������	
	FS_REQUEST_ALLOBJ,				//�������ж���	
	FS_CHANGE_PAGE,					//Page�ı�
	FS_GETCUR_PAGEOBJ,				//�õ���ǰPage����
	FS_DELCUR_PAGEOBJ,				//ɾ����ǰPage����
	FS_SET_FILERECT,				//�����ļ��������С

	/*����Flash������أ�����������������������������������������������������������������������*/
	FLASH_SHARE_PLAY,				//��ʼ����Flash	
	FLASH_SHARE_STOP,				//ֹͣ����Flash	
	FLASH_SHARE_PAUSE,				//��ͣ����Flash
	FLASH_SHARE_POSITION,			//����Flash����λ��
	FLASH_SHARE_SEEK,				//SeekFlash����λ��
	FLASH_SHARE_REQUEST,			//����Flash����λ��

	/*����2008.6.4 �¼� �ĵ�������أ���������������������������������������������������������������*/
	FS_SET_ZOOM,					//����	
	FS_MOVE_POS,					//�ƶ�λ��	
	FS_GET_POS,						//��ȡλ��	
	FS_ON_CURPAGEOBJ,				//�ص���ǰҳ����
	/*�°װ����Է���*/
	WB_NEW_COMMAND_DATA,				//�°װ����

	/*����20121120 �¼ӻ���������أ�������������������������������������������������������������*/
	MET_DTS_CONNECTED=1000,				//��������
	MET_DTS_ADD=1001,					//���Ӷ���
	MET_DTS_UPDATE=1002,				//�޸Ķ���
	MET_DTS_DELETE=1003,				//ɾ������
	MET_DTS_REQUEST=1004,				//�������
	MET_DTS_RESPONSE=1005,				//�ص��������
	MET_DTS_CHATMSG=1006,				//����������Ϣ
	MET_DTS_READY=1007,					//����ǰ����׼��,֪ͨ�ͻ�����������
};
//new wb
#define SHARE_OBJID      0


#define KEY_OBJ_BKG																			KEY_USER + 161 //���󱳾�����


typedef enum _eAttributeKey
{
	KEY_OBJ_ID = 0,	//����ID
	KEY_OBJ_TYPE,   //��������
	KEY_FILE_PATH,	//������Դ�ļ�·��
	KEY_FILE_SIZE,	//������Դ�ļ���С
	KEY_FILE_TYPE,	//������Դ�ļ�����
	KEY_FILE_POS,	//������Դ�ļ����ص�λ��
	KEY_FILE_SENDER,//������Դ�ϴ���
	KEY_EVENT_ID, //����������¼�ID
	KEY_OBJ_PAGE,//�������ڵ�ҳ
	KEY_OBJ_LOCK	,//������������ add by huanglongquan2011��12��3��   ����µ�key�Ļ�����������桢key_user֮ǰ���
	KEY_DRAG_CAMERA_TO_WB,	//�����Ǵ�u7��ק����ͷ���װ�

	KEY_FILE_ID,			//���ڹ켣��

	KEY_USER,			 //�û��Զ���
}eAttributeKey;

typedef enum _eWbCmdOperation
{
	CMD_ADD_ATTRIBUTE = 200,		 //�������
	CMD_DEL_ATTRIBUTE,			 //ɾ������
	CMD_CHANGE_ATTRIBUTE,		 //�ı�����
	CMD_REQUEST_ALL_ATTRIBUTE,	 //��ȡȫ������
	CMD_CLEANUP_ATTRIBUTE,		 //���ȫ������
	CMD_REQUEST_ALL_OBJ_ATTRIBUTE,//����ȫ������
	CMD_REQUEST_STATUS_INDEX,	 //��ȡ״̬����
	CMD_REQUEST_STATUS_NUM,      //��ȡ״̬����
	CMD_REDO_STATUS,			 //ǰ��һ��
	CMD_UNDO_STATUS,			 //����һ��
	CMD_SET_STATUS_NUM,			 //����״̬����	
	CMD_P2P_DATA,				 //��Ե���ʱ����
	CMD_BROADCAST_DATA,			 //�㲥��ʱ����
	CMD_P2MCU_DATA,				 //ֱ�ӷ���������
	CMD_START_STREAM,			 //��ʼ��������
	CMD_RESTART_STREAM,			 //���·�������
	CMD_ASKFOR_STREAM,			 //�������ݰ�
	CMD_SAVE_STREAM,			 //д�����ݰ�
	CMD_END_STREAM,				 //��ɷ�������
	CMD_STOP_STREAM,			 //ֹͣ������
	CMD_RELATE_STREAM,			 //�����Ժ�������
	CMD_ERROR_STREAM,		     //������������쳣
	//������ʽ
	CMD_PASSIVE_START,			 //��ʼ��������������
	CMD_PASSIVE_SAVE,		     //��������������
	CMD_PASSIVE_END,
	CMD_PASSIVE_STOP,			 //��ֹ������������
	CMD_PASSIVE_ERROR,

	CMD_DEL_ALL_ATTRIBUTE,		 //ɾ��ȫ������
	//ҳ����
	CMD_DEL_PAGE_ALL_ATTRIBUTE, //ɾ��ҳ��ȫ������
	CMD_DEL_PAGE,                       //	ɾ��page
	CMD_INSERT_PAGE,                  //����ҳ
	CMD_CHANGE_PAGE,               //����ҳ
	CMD_MOVE_PAGE,
	CMD_EXCHANGE_PAGE,

#ifdef HUBEI_MEDICAL_SDK
	//�û���ɫ��������
	CMD_ASKFOR_USERCOLOR,
	CMD_RELATIVE_USERCOLOR,
	CMD_BROADCAST_USERNAMECOLOR,
#endif

}eWbCmdOperation;

#ifdef HUBEI_MEDICAL_SDK
typedef enum _eUserColorWB
{
	Red_Color = 0,					//��ɫ RGB(255,0,0)
	Green_Color,						//��ɫ RGB(0,255,0)
	Blue_Color,						//��ɫ RGB(0,0,255)
	Yellow_Color,					//��ɫ RGB(255,255,0)
	Fuchsia_Color,					//��ɫ��ɫ RGB(255,0,255)
	Aqua_Color,						//ˮ��ɫ RGB(0,255,255)
	MidnightBlue_Color,			//��ҹ�� RGB(25,25,112)
	SeaGreen_Color,				//����ɫ RGB(46,139,87)
	Teal_Color,						//ˮѼɫ RGB(0,128,128)
	DarkSlateGray_Color,		//ī��ɫ RGB(47,79,79)
	DarkGreen_Color,				//���� RGB(0,100,0)
	SaddleBrown_Color,			//����ɫ RGB(139,69,19)
	Maroon_Color,					//��� RGB(128,0,0)
	BlueViolet_Color,				//�������� RGB(138,43,226)
	DarkOrchid_Color,			//�������� RGB(153,50,24)
	Default_Color,					//Ĭ����ɫ RGB(0,0,0)
}eUserColorWB;
#endif

