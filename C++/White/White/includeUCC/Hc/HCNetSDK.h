#ifndef  _HC_NETSDK_H
#define  _HC_NETSDK_H
#define NET_DVR_API  extern "C"__declspec(dllimport)

#define NET_DVR_NOERROR 				0  //û�д���
#define NET_DVR_PASSWORD_ERROR 			1  //�û����������
#define NET_DVR_NOENOUGHPRI 			2  //Ȩ�޲���
#define NET_DVR_NOINIT 					3  //û�г�ʼ��
#define NET_DVR_CHANNEL_ERROR 			4  //ͨ���Ŵ���
#define NET_DVR_OVER_MAXLINK 			5  //���ӵ�DVR�Ŀͻ��˸����������
#define NET_DVR_VERSIONNOMATCH			6  //�汾��ƥ��
#define NET_DVR_NETWORK_FAIL_CONNECT	7 //���ӷ�����ʧ��
#define NET_DVR_NETWORK_SEND_ERROR		8 //�����������ʧ��
#define NET_DVR_NETWORK_RECV_ERROR		9 //�ӷ�������������ʧ��
#define NET_DVR_NETWORK_RECV_TIMEOUT	10 //�ӷ������������ݳ�ʱ
#define NET_DVR_NETWORK_ERRORDATA		11 //���͵���������
#define NET_DVR_ORDER_ERROR				12 //���ô������
#define NET_DVR_OPERNOPERMIT			13 //�޴�Ȩ��
#define NET_DVR_COMMANDTIMEOUT			14 //DVR����ִ�г�ʱ
#define NET_DVR_ERRORSERIALPORT			15 //���ںŴ���
#define NET_DVR_ERRORALARMPORT			16 //�����˿ڴ���
#define NET_DVR_PARAMETER_ERROR 		17  //��������
#define NET_DVR_CHAN_EXCEPTION			18	//������ͨ�����ڴ���״̬
#define NET_DVR_NODISK					19	//û��Ӳ��
#define NET_DVR_ERRORDISKNUM			20	//Ӳ�̺Ŵ���
#define NET_DVR_DISK_FULL				21	//������Ӳ����
#define NET_DVR_DISK_ERROR				22	//������Ӳ�̳���
#define NET_DVR_NOSUPPORT				23	//��������֧��
#define NET_DVR_BUSY					24	//������æ
#define NET_DVR_MODIFY_FAIL				25	//�������޸Ĳ��ɹ�
#define NET_DVR_PASSWORD_FORMAT_ERROR	26	//���������ʽ����ȷ
#define NET_DVR_DISK_FORMATING			27	//Ӳ�����ڸ�ʽ����������������
#define NET_DVR_DVRNORESOURCE			28	//DVR��Դ����
#define	NET_DVR_DVROPRATEFAILED			29  //DVR����ʧ��
#define NET_DVR_OPENHOSTSOUND_FAIL 		30  //��PC����ʧ��
#define NET_DVR_DVRVOICEOPENED 			31  //�����������Խ���ռ��
#define	NET_DVR_TIMEINPUTERROR			32  //ʱ�����벻��ȷ
#define	NET_DVR_NOSPECFILE				33  //�ط�ʱ������û��ָ�����ļ�
#define NET_DVR_CREATEFILE_ERROR		34	//�����ļ�����
#define	NET_DVR_FILEOPENFAIL			35  //���ļ�����
#define	NET_DVR_OPERNOTFINISH			36  //�ϴεĲ�����û�����
#define	NET_DVR_GETPLAYTIMEFAIL			37  //��ȡ��ǰ���ŵ�ʱ�����
#define	NET_DVR_PLAYFAIL				38  //���ų���
#define NET_DVR_FILEFORMAT_ERROR		39  //�ļ���ʽ����ȷ
#define NET_DVR_DIR_ERROR				40	//·������
#define NET_DVR_ALLOC_RESOUCE_ERROR		41  //��Դ�������
#define NET_DVR_AUDIO_MODE_ERROR		42	//����ģʽ����
#define NET_DVR_NOENOUGH_BUF			43	//������̫С
#define NET_DVR_CREATESOCKET_ERROR		44	//����SOCKET����
#define NET_DVR_SETSOCKET_ERROR			45	//����SOCKET����
#define NET_DVR_MAX_NUM					46	//�����ﵽ���
#define NET_DVR_USERNOTEXIST			47	//�û�������
#define NET_DVR_WRITEFLASHERROR			48  //дFLASH����
#define NET_DVR_UPGRADEFAIL				49  //DVR����ʧ��
#define NET_DVR_CARDHAVEINIT			50  //���뿨�Ѿ���ʼ����
#define NET_DVR_PLAYERFAILED			51	//�������д���
#define NET_DVR_MAX_USERNUM				52  //�û����ﵽ���
#define NET_DVR_GETLOCALIPANDMACFAIL	53  //��ÿͻ��˵�IP��ַ�������ַʧ��
#define NET_DVR_NOENCODEING				54	//��ͨ��û�б���
#define NET_DVR_IPMISMATCH				55	//IP��ַ��ƥ��
#define NET_DVR_MACMISMATCH				56	//MAC��ַ��ƥ��
#define NET_DVR_UPGRADELANGMISMATCH		57	//�����ļ����Բ�ƥ��
#define NET_DVR_DDRAWDEVICENOSUPPORT	58	//�����Կ���֧��

//�����ļ�����־��������ֵ
#define NET_DVR_FILE_SUCCESS			1000	//����ļ���Ϣ
#define NET_DVR_FILE_NOFIND				1001	//û���ļ�
#define NET_DVR_ISFINDING				1002	//���ڲ����ļ�
#define	NET_DVR_NOMOREFILE				1003	//�����ļ�ʱû�и�����ļ�
#define	NET_DVR_FILE_EXCEPTION			1004	//�����ļ�ʱ�쳣

//NET_DVR_IsSupport()����ֵ
//1��9λ�ֱ��ʾ������Ϣ��λ����TRUE)��ʾ֧�֣�
#define NET_DVR_SUPPORT_DDRAW			0x01	//֧��DIRECTDRAW�������֧�֣��򲥷������ܹ�����
#define NET_DVR_SUPPORT_BLT				0x02	//�Կ�֧��BLT�����������֧�֣��򲥷������ܹ�����
#define NET_DVR_SUPPORT_BLTFOURCC		0x04	//�Կ�BLT֧����ɫת���������֧�֣��������������������RGBת����
#define NET_DVR_SUPPORT_BLTSHRINKX		0x08	//�Կ�BLT֧��X����С�������֧�֣�ϵͳ�����������ת����
#define NET_DVR_SUPPORT_BLTSHRINKY		0x10	//�Կ�BLT֧��Y����С�������֧�֣�ϵͳ�����������ת����
#define NET_DVR_SUPPORT_BLTSTRETCHX		0x20	//�Կ�BLT֧��X��Ŵ������֧�֣�ϵͳ�����������ת����
#define NET_DVR_SUPPORT_BLTSTRETCHY		0x40	//�Կ�BLT֧��Y��Ŵ������֧�֣�ϵͳ�����������ת����
#define NET_DVR_SUPPORT_SSE				0x80	//CPU֧��SSEָ�Intel Pentium3����֧��SSEָ�
#define NET_DVR_SUPPORT_MMX				0x100	//CPU֧��MMXָ���Intel Pentium3����֧��SSEָ�

#define SET_PRESET		8	// ����Ԥ�õ�
#define CLE_PRESET		9	// ���Ԥ�õ�
#define GOTO_PRESET		39	// ת��Ԥ�õ�

#define LIGHT_PWRON		2	/* ��ͨ�ƹ��Դ */
#define WIPER_PWRON		3	/* ��ͨ��ˢ���� */
#define FAN_PWRON		4	/* ��ͨ���ȿ��� */
#define HEATER_PWRON	5	/* ��ͨ���������� */
#define AUX_PWRON		6	/* ��ͨ�����豸���� */

#define HC_ZOOM_IN			11	/* �������ٶ�SS���(���ʱ��) */
#define HC_ZOOM_OUT		12	/* �������ٶ�SS��С(���ʱ�С) */
#define FOCUS_NEAR		13	/* �������ٶ�SSǰ�� */
#define FOCUS_FAR		14	/* �������ٶ�SS��� */
#define IRIS_OPEN		15	/* ��Ȧ���ٶ�SS���� */
#define IRIS_CLOSE		16	/* ��Ȧ���ٶ�SS��С */
#define TILT_UP			21	/* ��̨��SS���ٶ����� */
#define TILT_DOWN		22	/* ��̨��SS���ٶ��¸� */
#define PAN_LEFT		23	/* ��̨��SS���ٶ���ת */
#define PAN_RIGHT		24	/* ��̨��SS���ٶ���ת */
#define UP_LEFT			25	/* ��̨��SS���ٶ���������ת */
#define UP_RIGHT		26	/* ��̨��SS���ٶ���������ת */
#define DOWN_LEFT		27	/* ��̨��SS���ٶ��¸�����ת */
#define DOWN_RIGHT		28	/* ��̨��SS���ٶ��¸�����ת */
#define PAN_AUTO		29	/* ��̨��SS���ٶ������Զ�ɨ�� */
#define FILL_PRE_SEQ	30	/* ��Ԥ�õ����Ѳ������ */
#define SET_SEQ_DWELL	31	/* ����Ѳ����ͣ��ʱ�� */
#define SET_SEQ_SPEED	32	/* ����Ѳ���ٶ� */
#define CLE_PRE_SEQ		33	/* ��Ԥ�õ��Ѳ��������ɾ�� */
#define STA_MEM_CRUISE	34	/* ��ʼ��¼�켣 */
#define STO_MEM_CRUISE	35	/* ֹͣ��¼�켣 */
#define RUN_CRUISE		36	/* ��ʼ�켣 */
#define RUN_SEQ			37	/* ��ʼѲ�� */
#define STOP_SEQ		38	/* ֹͣѲ�� */

//��ʾģʽ
enum{NORMALMODE = 0,OVERLAYMODE};
//����ģʽ
enum{PTOPTCPMODE, PTOPUDPMODE, MULTIMODE, RTPMODE, AUDIODETACH, NOUSEMODE};

#define NET_DVR_SYSHEAD				1	//ϵͳͷ����
#define NET_DVR_STREAMDATA			2	//������

//���ſ�������궨�� NET_DVR_PlayBackControl,NET_DVR_PlayControlLocDisplay,NET_DVR_DecPlayBackCtrl�ĺ궨��
#define NET_DVR_PLAYSTART			1//��ʼ����
#define NET_DVR_PLAYSTOP			2//ֹͣ����
#define NET_DVR_PLAYPAUSE			3//��ͣ����
#define NET_DVR_PLAYRESTART			4//�ָ�����
#define NET_DVR_PLAYFAST			5//���
#define NET_DVR_PLAYSLOW			6//����
#define NET_DVR_PLAYNORMAL			7//�����ٶ�
#define NET_DVR_PLAYFRAME			8//��֡��
#define NET_DVR_PLAYSTARTAUDIO		9//������
#define NET_DVR_PLAYSTOPAUDIO		10//�ر�����
#define NET_DVR_PLAYAUDIOVOLUME		11//��������
#define NET_DVR_PLAYSETPOS			12//�ı��ļ��طŵĽ���
#define NET_DVR_PLAYGETPOS			13//��ȡ�ļ��طŵĽ���
#define NET_DVR_PLAYGETTIME			14//��ȡ��ǰ�Ѿ����ŵ�ʱ��
#define NET_DVR_PLAYGETFRAME		15//��ȡ��ǰ�Ѿ����ŵ�֡��
#define NET_DVR_GETTOTALFRAMES  	16//��ȡ��ǰ�����ļ��ܵ�֡��
#define NET_DVR_GETTOTALTIME    	17//��ȡ��ǰ�����ļ��ܵ�ʱ��
#define NET_DVR_THROWBFRAME			20//��B֡

//NET_DVR_GetDVRConfig,NET_DVR_GetDVRConfig�������
#define NET_DVR_GET_DEVICECFG		100		//��ȡ�豸����
#define NET_DVR_SET_DEVICECFG		101		//�����豸����
#define NET_DVR_GET_NETCFG			102		//��ȡ�������
#define NET_DVR_SET_NETCFG			103		//�����������
#define NET_DVR_GET_PICCFG			104		//��ȡͼ�����
#define NET_DVR_SET_PICCFG			105		//����ͼ�����
#define NET_DVR_GET_COMPRESSCFG		106		//��ȡѹ������
#define NET_DVR_SET_COMPRESSCFG		107		//����ѹ������
#define NET_DVR_GET_COMPRESSCFG_EX	204		//��ȡѹ������(��չ)
#define NET_DVR_SET_COMPRESSCFG_EX	205		//����ѹ������(��չ)
#define NET_DVR_GET_RECORDCFG		108		//��ȡ¼��ʱ�����
#define NET_DVR_SET_RECORDCFG		109		//����¼��ʱ�����
#define NET_DVR_GET_DECODERCFG		110		//��ȡ����������
#define NET_DVR_SET_DECODERCFG		111		//���ý���������
#define NET_DVR_GET_RS232CFG 		112		//��ȡ232���ڲ���
#define NET_DVR_SET_RS232CFG		113		//����232���ڲ���
#define NET_DVR_GET_ALARMINCFG 		114		//��ȡ�����������
#define NET_DVR_SET_ALARMINCFG		115		//���ñ����������
#define NET_DVR_GET_ALARMOUTCFG 	116		//��ȡ�����������
#define NET_DVR_SET_ALARMOUTCFG		117		//���ñ����������
#define NET_DVR_GET_TIMECFG 		118		//��ȡDVRʱ��
#define NET_DVR_SET_TIMECFG			119		//����DVRʱ��
#define NET_DVR_GET_PREVIEWCFG 		120		//��ȡԤ������
#define NET_DVR_SET_PREVIEWCFG		121		//����Ԥ������
#define NET_DVR_GET_VIDEOOUTCFG 	122		//��ȡ��Ƶ�������
#define NET_DVR_SET_VIDEOOUTCFG		123		//������Ƶ�������
#define NET_DVR_GET_USERCFG 		124		//��ȡ�û�����
#define NET_DVR_SET_USERCFG			125		//�����û�����
#define NET_DVR_GET_EXCEPTIONCFG 	126		//��ȡ�쳣����
#define NET_DVR_SET_EXCEPTIONCFG	127		//�����쳣����
#define NET_DVR_GET_SHOWSTRING		130		//��ȡ�����ַ�����
#define NET_DVR_SET_SHOWSTRING		131		//���õ����ַ�����
#define NET_DVR_GET_EVENTCOMPCFG	132		//��ȡ�¼�����¼�����
#define NET_DVR_SET_EVENTCOMPCFG	133		//�����¼�����¼�����
#define NET_DVR_GET_AUXOUTCFG		140		//��ȡ�����������
#define NET_DVR_SET_AUXOUTCFG		141		//���ø����������
//2006-04-13 -sϵ��˫���
#define NET_DVR_GET_PREVIEWCFG_AUX 	142		//��ȡԤ������
#define NET_DVR_SET_PREVIEWCFG_AUX	143		//����Ԥ������
#define NET_DVR_GET_PICCFG_EX		200		//��ȡͼ���������չ��
#define NET_DVR_SET_PICCFG_EX		201		//����ͼ���������չ��
#define NET_DVR_GET_USERCFG_EX 		202		//��ȡ�û�����
#define NET_DVR_SET_USERCFG_EX		203		//�����û�����
#define NET_DVR_GET_NETAPPCFG		222		//��ȡ����Ӧ�ò��� NTP/DDNS
#define NET_DVR_SET_NETAPPCFG		223		//��������Ӧ�ò��� NTP/DDNS
#define NET_DVR_GET_NFSCFG			230		/* NFS disk config */
#define NET_DVR_SET_NFSCFG			231
#define NET_DVR_GET_NETCFG_OTHER	244		//��ȡ�������(��·������)
#define NET_DVR_SET_NETCFG_OTHER	245		//�����������(��·������)
#define NET_DVR_GET_EMAILPARACFG	250		//Get EMAIL parameters
#define NET_DVR_SET_EMAILPARACFG	251		//Setup EMAIL parameters

//�ص���������
#define COMM_ALARM					0x1100	//������Ϣ
#define	COMM_TRADEINFO				0x1500 //ATMDVR�����ϴ�������Ϣ

//��Ϣ��ʽ
//�쳣����
#define EXCEPTION_AUDIOEXCHANGE		0x8001	//�����Խ��쳣
#define EXCEPTION_ALARM				0x8002	//�����쳣
#define EXCEPTION_PREVIEW			0x8003	//����Ԥ���쳣
#define EXCEPTION_SERIAL			0x8004	//͸��ͨ���쳣
#define EXCEPTION_RECONNECT			0x8005	//Ԥ��ʱ����

#define NAME_LEN 			32
#define SERIALNO_LEN 		48
#define MACADDR_LEN 		6
#define MAX_ETHERNET 		2
#define PATHNAME_LEN 		128
#define PASSWD_LEN 			16
#define MAX_CHANNUM 		16
#define MAX_ALARMOUT 		4
#define MAX_TIMESEGMENT		4
#define MAX_PRESET			128
#define MAX_DAYS			7
#define PHONENUMBER_LEN		32
#define MAX_DISKNUM 		16
#define MAX_WINDOW 			16
#define MAX_VGA 			1
#define MAX_USERNUM 		16
#define MAX_EXCEPTIONNUM	16
#define MAX_LINK			6
#define MAX_ALARMIN			16
#define MAX_VIDEOOUT		2
#define MAX_NAMELEN			16		//DVR���ص�½��
#define MAX_RIGHT			32		//Ȩ��
#define CARDNUM_LEN 		20
#define MAX_SHELTERNUM		4		//�ڵ�������
#define MAX_DECPOOLNUM		4
#define MAX_DECNUM			4
#define MAX_TRANSPARENTNUM	2
#define MAX_STRINGNUM		4
#define MAX_AUXOUT			4
#define MAX_NFS_DISK		8
#define MAX_CYCLE_CHAN		16

/* ����ӿڶ��� */
#define NET_IF_10M_HALF		1					/* 10M ethernet */
#define NET_IF_10M_FULL		2
#define NET_IF_100M_HALF	3					/* 100M ethernet */
#define NET_IF_100M_FULL	4
#define NET_IF_AUTO			5

//�豸�ͺ�(DVR����)
#define DVR				1
#define ATMDVR			2
#define DVS				3
#define DEC				4  /* 6001D */
#define ENC_DEC			5  /* 6001F */
#define DVR_HC			6
#define DVR_HT			7
#define DVR_HF			8
#define DVR_HS			9
#define DVR_HTS			10
#define DVR_HB			11
#define DVR_HCS			12
#define DVS_A			13
#define DVR_HC_S		14
#define DVR_HT_S		15
#define DVR_HF_S		16
#define DVR_HS_S		17
#define ATMDVR_S		18
#define LOWCOST_DVR		19
#define DEC_MAT			20	//��·������
#define NETRET_IPCAM 30  /*IP �����*/
#define NETRET_IPDOME  40  /*IP ����*/
#define NETRET_IPMOD  50  /*IP ģ��*/

typedef struct  {
	BYTE sSerialNumber[SERIALNO_LEN];  //���к�
	BYTE byAlarmInPortNum;		//DVR�����������
	BYTE byAlarmOutPortNum;		//DVR�����������
	BYTE byDiskNum;				//DVR Ӳ�̸���
	BYTE byDVRType;				//DVR����, 1:DVR 2:ATM DVR 3:DVS ......
	BYTE byChanNum;				//DVR ͨ������
	BYTE byStartChan;			//��ʼͨ����,����DVS-1,DVR - 1
}NET_DVR_DEVICEINFO, *LPNET_DVR_DEVICEINFO;

//Ԥ������
typedef struct{
	long bToScreen;
	long bToVideoOut;
	long nLeft;
	long nTop;
	long nWidth;
	long nHeight;
	long nReserved;
}NET_DVR_DISPLAY_PARA,*LPNET_DVR_DISPLAY_PARA;

typedef struct{
	LONG lChannel;//ͨ����
	LONG lLinkMode;//���λ(31)Ϊ0��ʾ��������Ϊ1��ʾ��������0��30λ��ʾ�������ӷ�ʽ: 0��TCP��ʽ,1��UDP��ʽ,2���ಥ��ʽ,3 - RTP��ʽ��4-����Ƶ�ֿ�(TCP)
	HWND hPlayWnd;//���Ŵ��ڵľ��,ΪNULL��ʾ������ͼ��
	char* sMultiCastIP;//�ಥ���ַ
}NET_DVR_CLIENTINFO,*LPNET_DVR_CLIENTINFO;

typedef struct{
	LONG lChannel;//ͨ����
	LONG lLinkMode; //���λ(31)Ϊ0��ʾ��������Ϊ1��ʾ��������0��30λ��ʾ�������ӷ�ʽ: 0��TCP��ʽ,1��UDP��ʽ,2���ಥ��ʽ,3 - RTP��ʽ��4-�绰�ߣ�5��128k�����6��256k�����7��384k�����8��512k�����
	char* sMultiCastIP;
	NET_DVR_DISPLAY_PARA struDisplayPara;
}NET_DVR_CARDINFO,*LPNET_DVR_CARDINFO;

typedef struct{
	DWORD dwYear;		//��
	DWORD dwMonth;		//��
	DWORD dwDay;		//��
	DWORD dwHour;		//ʱ
	DWORD dwMinute;		//��
	DWORD dwSecond;		//��
}NET_DVR_TIME,*LPNET_DVR_TIME;

typedef struct{
	//��ʼʱ��
    BYTE byStartHour;
	BYTE byStartMin;
	//����ʱ��
	BYTE byStopHour;
	BYTE byStopMin;
}NET_DVR_SCHEDTIME,*LPNET_DVR_SCHEDTIME;

/*����ʽ*/
#define NOACTION			0x0				/*����Ӧ*/
#define WARNONMONITOR		0x1				/*�������Ͼ���*/
#define WARNONAUDIOOUT		0x2				/*��������*/
#define UPTOCENTER			0x4				/*�ϴ�����*/
#define TRIGGERALARMOUT		0x8				/*�����������*/

typedef struct
{
	DWORD	dwHandleType;			/*����ʽ,����ʽ��"��"���*/
									/*0x00: ����Ӧ*/
									/*0x01: �������Ͼ���*/
									/*0x02: ��������*/
									/*0x04: �ϴ�����*/
									/*0x08: �����������*/
									/*0x20: ����ץͼ*/
	BYTE byRelAlarmOut[MAX_ALARMOUT];  //�������������ͨ��,�������������,Ϊ1��ʾ���������
}NET_DVR_HANDLEEXCEPTION;

//DVR�豸����
typedef struct{
	DWORD dwSize;
	BYTE sDVRName[NAME_LEN];     //DVR����
	DWORD dwDVRID;				//DVR ID,����ң����
	DWORD dwRecycleRecord;		//�Ƿ�ѭ��¼��,0:����; 1:��
//���²��ܸ���
	BYTE sSerialNumber[SERIALNO_LEN];  //���к�
	DWORD dwSoftwareVersion;			//����汾��,��16λ�����汾,��16λ�Ǵΰ汾
	DWORD dwSoftwareBuildDate;			//�����������,0xYYYYMMDD
	DWORD dwDSPSoftwareVersion;		//DSP����汾,��16λ�����汾,��16λ�Ǵΰ汾
	DWORD dwDSPSoftwareBuildDate;		// DSP�����������,0xYYYYMMDD
	DWORD dwPanelVersion;				// ǰ���汾,��16λ�����汾,��16λ�Ǵΰ汾
	DWORD dwHardwareVersion;	// Ӳ���汾,��16λ�����汾,��16λ�Ǵΰ汾
	BYTE byAlarmInPortNum;		//DVR�����������
	BYTE byAlarmOutPortNum;		//DVR�����������
	BYTE byRS232Num;			//DVR 232���ڸ���
	BYTE byRS485Num;			//DVR 485���ڸ���
	BYTE byNetworkPortNum;		//����ڸ���
	BYTE byDiskCtrlNum;			//DVR Ӳ�̿���������
	BYTE byDiskNum;				//DVR Ӳ�̸���
	BYTE byDVRType;				//DVR����, 1:DVR 2:ATM DVR 3:DVS ......
	BYTE byChanNum;				//DVR ͨ������
	BYTE byStartChan;			//��ʼͨ����,����DVS-1,DVR - 1
	BYTE byDecordChans;			//DVR ����·��
	BYTE byVGANum;				//VGA�ڵĸ���
	BYTE byUSBNum;				//USB�ڵĸ���
	char reservedData[3];		//����
}NET_DVR_DEVICECFG,*LPNET_DVR_DEVICECFG;

typedef struct {
	char sDVRIP[16];          //DVR IP��ַ
	char sDVRIPMask[16];      // DVR IP��ַ����
	DWORD dwNetInterface;   //����ӿ� 1-10MBase-T 2-10MBase-Tȫ˫�� 3-100MBase-TX 4-100Mȫ˫�� 5-10M/100M����Ӧ
	WORD wDVRPort;		//�˿ں�
	BYTE byMACAddr[MACADDR_LEN];		//�������������ַ
}NET_DVR_ETHERNET;

//�������ýṹ
typedef struct{
	DWORD dwSize;
	NET_DVR_ETHERNET struEtherNet[MAX_ETHERNET];		/* ��̫���� */
	char sManageHostIP[16];		//Զ�̹���������ַ
	WORD wManageHostPort; //Զ�̹��������˿ں�
	char sIPServerIP[16];            //DNS��������ַ
	char sMultiCastIP[16];     //�ಥ���ַ
	char sGatewayIP[16];       	//���ص�ַ
	char sNFSIP[16];			//NAS����IP��ַ
	BYTE sNFSDirectory[PATHNAME_LEN];//NASĿ¼
	DWORD dwPPPOE;				//0-������,1-����
	BYTE sPPPoEUser[NAME_LEN];	//PPPoE�û���
	char sPPPoEPassword[PASSWD_LEN];// PPPoE����
	char sPPPoEIP[16];			//PPPoE IP��ַ(ֻ��)
	WORD wHttpPort;				//HTTP�˿ں�
}NET_DVR_NETCFG,*LPNET_DVR_NETCFG;

//ͨ��ͼ��ṹ
//�ƶ����
typedef struct{
	BYTE byMotionScope[18][22];	/*�������,����22*18��С���,Ϊ1��ʾ�ú�����ƶ��������,0-��ʾ����*/
	BYTE byMotionSensitive;		/*�ƶ����������, 0 - 5,Խ��Խ����,0xff�ر�*/
	BYTE byEnableHandleMotion;	/* �Ƿ����ƶ���� */
	NET_DVR_HANDLEEXCEPTION strMotionHandleType;	/* ����ʽ */
	NET_DVR_SCHEDTIME struAlarmTime[MAX_DAYS][MAX_TIMESEGMENT];//����ʱ��
	BYTE byRelRecordChan[MAX_CHANNUM]; //����������¼��ͨ��,Ϊ1��ʾ������ͨ��
}NET_DVR_MOTION,*LPNET_DVR_MOTION;

//�ڵ���������Ϊ704*576
typedef struct{
	DWORD dwEnableHideAlarm;			/* �Ƿ������ڵ����� ,0-��,1-�������� 2-�������� 3-�������� */
	WORD wHideAlarmAreaTopLeftX;			/* �ڵ������x���� */
	WORD wHideAlarmAreaTopLeftY;			/* �ڵ������y���� */
	WORD wHideAlarmAreaWidth;			/* �ڵ�����Ŀ� */
	WORD wHideAlarmAreaHeight;			/*�ڵ�����ĸ�*/
	NET_DVR_HANDLEEXCEPTION strHideAlarmHandleType;	/* ����ʽ */
	NET_DVR_SCHEDTIME struAlarmTime[MAX_DAYS][MAX_TIMESEGMENT];//����ʱ��
}NET_DVR_HIDEALARM,*LPNET_DVR_HIDEALARM;

//�źŶ�ʧ����
typedef struct{
	BYTE byEnableHandleVILost;	/* �Ƿ����źŶ�ʧ���� */
	NET_DVR_HANDLEEXCEPTION strVILostHandleType;	/* ����ʽ */
	NET_DVR_SCHEDTIME struAlarmTime[MAX_DAYS][MAX_TIMESEGMENT];//����ʱ��
}NET_DVR_VILOST,*LPNET_DVR_VILOST;

typedef struct
{
	DWORD dwSize;
	BYTE sChanName[NAME_LEN];
	DWORD dwVideoFormat;	/* ֻ������Ƶ��ʽ 1-NTSC 2-PAL*/
	BYTE byBrightness;  	/*����,0-255*/
	BYTE byContrast;    	/*�Աȶ�,0-255*/
	BYTE bySaturation;  	/*���Ͷ�,0-255 */
	BYTE byHue;    			/*ɫ��,0-255*/
	//��ʾͨ����
	DWORD dwShowChanName; /* Ԥ����ͼ�����Ƿ���ʾͨ������,0-����ʾ,1-��ʾ ����Ϊ704*576 */
	WORD wShowNameTopLeftX;				/* ͨ��������ʾλ�õ�x���� */
	WORD wShowNameTopLeftY;				/* ͨ��������ʾλ�õ�y���� */
	//�źŶ�ʧ����
	NET_DVR_VILOST struVILost;
	//�ƶ����
	NET_DVR_MOTION struMotion;
	//�ڵ�����
	NET_DVR_HIDEALARM struHideAlarm;
	//�ڵ�
	DWORD dwEnableHide;		/* �Ƿ������ڵ� ,0-��,1-�� ����Ϊ704*576*/
	WORD wHideAreaTopLeftX;				/* �ڵ������x���� */
	WORD wHideAreaTopLeftY;				/* �ڵ������y���� */
	WORD wHideAreaWidth;				/* �ڵ�����Ŀ� */
	WORD wHideAreaHeight;				/*�ڵ�����ĸ�*/
	//OSD
	DWORD dwShowOsd;	/* Ԥ����ͼ�����Ƿ���ʾOSD,0-����ʾ,1-��ʾ*/
	WORD wOSDTopLeftX;				/* OSD��x���� */
	WORD wOSDTopLeftY;				/* OSD��y���� */
	BYTE byOSDType;					/* OSD����(��Ҫ�������ո�ʽ) */
							/* 0: XXXX-XX-XX ������ */
							/* 1: XX-XX-XXXX ������ */
							/* 2: XXXX��XX��XX�� */
							/* 3: XX��XX��XXXX�� */
	BYTE byDispWeek;				/* �Ƿ���ʾ���� */
	BYTE byOSDAttrib;				/* OSD����:͸������˸ */
									/* 1: ͸��,��˸ */
									/* 2: ͸��,����˸ */
									/* 3: ��˸,��͸�� */
									/* 4: ��͸��,����˸ */
}NET_DVR_PICCFG,*LPNET_DVR_PICCFG;

typedef struct{
	WORD wHideAreaTopLeftX;				/* �ڵ������x���� */
	WORD wHideAreaTopLeftY;				/* �ڵ������y���� */
	WORD wHideAreaWidth;				/* �ڵ�����Ŀ� */
	WORD wHideAreaHeight;				/*�ڵ�����ĸ�*/
}NET_DVR_SHELTER,*LPNET_DVR_SHELTER;

//SDK_V14
typedef struct
{
	DWORD dwSize;
	BYTE sChanName[NAME_LEN];
	DWORD dwVideoFormat;	/* ֻ�� ��Ƶ��ʽ 1-NTSC 2-PAL*/
	BYTE byBrightness;  	/*����,0-255*/
	BYTE byContrast;    	/*�Աȶ�,0-255*/
	BYTE bySaturation;  	/*���Ͷ�,0-255 */
	BYTE byHue;    			/*ɫ��,0-255*/
	//��ʾͨ����
	DWORD dwShowChanName; // Ԥ����ͼ�����Ƿ���ʾͨ������,0-����ʾ,1-��ʾ �����С704*576
	WORD wShowNameTopLeftX;				/* ͨ��������ʾλ�õ�x���� */
	WORD wShowNameTopLeftY;				/* ͨ��������ʾλ�õ�y���� */
	//�źŶ�ʧ����
	NET_DVR_VILOST struVILost;
	//�ƶ����
	NET_DVR_MOTION struMotion;
	//�ڵ�����
	NET_DVR_HIDEALARM struHideAlarm;
	//�ڵ�  �����С704*576
	DWORD dwEnableHide;		/* �Ƿ������ڵ� ,0-��,1-��*/
	NET_DVR_SHELTER struShelter[MAX_SHELTERNUM];
	//OSD
	DWORD dwShowOsd;// Ԥ����ͼ�����Ƿ���ʾOSD,0-����ʾ,1-��ʾ �����С704*576
	WORD wOSDTopLeftX;				/* OSD��x���� */
	WORD wOSDTopLeftY;				/* OSD��y���� */
	BYTE byOSDType;					/* OSD����(��Ҫ�������ո�ʽ) */
	/* 0: XXXX-XX-XX ������ */
	/* 1: XX-XX-XXXX ������ */
	/* 2: XXXX��XX��XX�� */
	/* 3: XX��XX��XXXX�� */
	BYTE byDispWeek;				/* �Ƿ���ʾ���� */
	BYTE byOSDAttrib;				/* OSD����:͸������˸ */
	/* 1: ͸��,��˸ */
	/* 2: ͸��,����˸ */
	/* 3: ��˸,��͸�� */
	/* 4: ��͸��,����˸ */
}NET_DVR_PICCFG_EX,*LPNET_DVR_PICCFG_EX;

//ѹ������
typedef struct{
	BYTE byStreamType;		//��������0-��Ƶ��,1-������
	BYTE byResolution;  	//�ֱ���0-DCIF 1-CIF, 2-QCIF, 3-4CIF, 4-2CIF
	BYTE byBitrateType;		//��������0:�����ʣ�1:������
	BYTE  byPicQuality;		//ͼ������ 0-��� 1-�κ� 2-�Ϻ� 3-һ�� 4-�ϲ� 5-��
	DWORD dwVideoBitrate; 	//��Ƶ���� 0-���� 1-���� 2-32K 3-48k 4-64K 5-80K 6-96K 7-128K 8-160k 9-192K 10-224K 11-256K 12-320K
				// 13-384K 14-448K 15-512K 16-640K 17-768K 18-896K 19-1024K 20-1280K 21-1536K 22-1792K 23-2048K
				//���λ(31λ)�ó�1��ʾ���Զ�������, 0-30λ��ʾ����ֵ(MIN-16K MAX-8192K)��
	DWORD dwVideoFrameRate;	//֡�� 0-ȫ��; 1-1/16; 2-1/8; 3-1/4; 4-1/2; 5-1; 6-2; 7-4; 8-6; 9-8; 10-10; 11-12; 12-16; 13-20;
}NET_DVR_COMPRESSION_INFO,*LPNET_DVR_COMPRESSION_INFO;

typedef struct{
	BYTE byStreamType;		//��������0-��Ƶ��,1-������
	BYTE byResolution;  	//�ֱ���0-DCIF 1-CIF, 2-QCIF, 3-4CIF, 4-2CIF
	BYTE byBitrateType;		//��������0:�����ʣ�1:������
	BYTE  byPicQuality;		//ͼ������ 0-��� 1-�κ� 2-�Ϻ� 3-һ�� 4-�ϲ� 5-��
	DWORD dwVideoBitrate; 	//��Ƶ���� 0-���� 1-16K(����) 2-32K 3-48k 4-64K 5-80K 6-96K 7-128K 8-160k 9-192K 10-224K 11-256K 12-320K
	// 13-384K 14-448K 15-512K 16-640K 17-768K 18-896K 19-1024K 20-1280K 21-1536K 22-1792K 23-2048K
	//���λ(31λ)�ó�1��ʾ���Զ�������, 0-30λ��ʾ����ֵ(MIN-32K MAX-8192K)��
	DWORD dwVideoFrameRate;	//֡�� 0-ȫ��; 1-1/16; 2-1/8; 3-1/4; 4-1/2; 5-1; 6-2; 7-4; 8-6; 9-8; 10-10; 11-12; 12-16; 13-20;
	WORD  wIntervalFrameI;  //I֡���
	//2006-08-11 ���ӵ�P֡�����ýӿڣ����Ը���ʵʱ����ʱ����
	BYTE  byIntervalBPFrame;//0-BBP֡; 1-BP֡; 2-��P֡ BP֡�����ݲ�֧��
	BYTE  byRes;
}NET_DVR_COMPRESSION_INFO_EX,*LPNET_DVR_COMPRESSION_INFO_EX;

typedef struct{
	DWORD dwSize;
	NET_DVR_COMPRESSION_INFO struRecordPara; //¼��
	NET_DVR_COMPRESSION_INFO struNetPara;	//����
}NET_DVR_COMPRESSIONCFG,*LPNET_DVR_COMPRESSIONCFG;

typedef struct{
	DWORD dwSize;
	NET_DVR_COMPRESSION_INFO_EX struRecordPara; //¼��
	NET_DVR_COMPRESSION_INFO_EX struNetPara;	//����
}NET_DVR_COMPRESSIONCFG_EX,*LPNET_DVR_COMPRESSIONCFG_EX;

//¼��
typedef struct{
	NET_DVR_SCHEDTIME struRecordTime;
	BYTE byRecordType;	//0:��ʱ¼��1:�ƶ���⣬2:����¼��3:����|������4:����&����, 5:�����, 6: �ֶ�¼��
	char reservedData[3];
}NET_DVR_RECORDSCHED,*LPNET_DVR_RECORDSCHED;

typedef struct {
	WORD wAllDayRecord;				/* �Ƿ�ȫ��¼�� */
	BYTE byRecordType;				/* ¼������ 0:��ʱ¼��1:�ƶ���⣬2:����¼��3:����|������4:����&���� 5:�����, 6: �ֶ�¼��*/
	char reservedData;
}NET_DVR_RECORDDAY;

typedef struct {
	DWORD dwSize;
	DWORD dwRecord;  /*�Ƿ�¼�� 0-�� 1-��*/
	NET_DVR_RECORDDAY struRecAllDay[MAX_DAYS];
	NET_DVR_RECORDSCHED struRecordSched[MAX_DAYS][MAX_TIMESEGMENT];
	DWORD dwRecordTime;	/* ¼��ʱ�䳤�� */
	DWORD dwPreRecordTime;	/* Ԥ¼ʱ�� 0-��Ԥ¼ 1-5�� 2-10�� 3-15�� 4-20�� 5-25�� 6-30�� 7-0xffffffff(������Ԥ¼) */
}NET_DVR_RECORD,*LPNET_DVR_RECORD;

/* PTZ type */
#define YOULI						0
#define LILIN_1016					1
#define LILIN_820					2
#define PELCO_P						3
#define DM_QUICKBALL				4
#define HD600						5
#define JC4116						6
#define PELCO_DWX					7
#define PELCO_D						8
#define VCOM_VC_2000				9
#define NETSTREAMER					10
#define SAE							11
#define SAMSUNG						12
#define KALATEL_KTD_312				13
#define CELOTEX						14
#define TLPELCO_P					15
#define TL_HHX2000					16
#define BBV							17
#define RM110						18
#define KC3360S						19
#define ACES						20
#define ALSON						21
#define INV3609HD					22
#define HOWELL						23
#define TC_PELCO_P					24
#define TC_PELCO_D					25
#define AUTO_M						26
#define AUTO_H						27
#define ANTEN						28
#define CHANGLIN					29
#define DELTADOME					30
#define	XYM_12						31
#define ADR8060						32
#define EVI							33
#define Demo_Speed					34
#define DM_PELCO_D					35
#define ST_832						36
#define LC_D2104					37
#define HUNTER						38
#define A01							39
#define TECHWIN						40
#define WEIHAN						41
#define LG							42
#define D_MAX						43
#define PANASONIC					44
#define KTD_348						45
#define INFINOVA					46
#define PIH_7625					47
#define IDOME_IVIEW_LCU				48
#define Dennar_dDome				49
#define Philips						50
#define SAMPLE						51
#define PLD							52
#define PARCO						53
#define HY							54
#define NAIJIE						55
#define CAT_KING					56
#define YH_06						57
#define SP9096X						58
#define M_PANEL						59
#define M_MV2050					60
#define SAE_QUICKBALL				61
#define RED_APPLE					62
#define NKO8G						63
#define DH_CC440					64
#define TX_CONTROL_232				65
#define	VCL_SPEED_DOME				66
#define	ST_2C160					67
#define TDWY						68
#define TWHC						69
#define USNT						70
#define KLT_NVD2200PS				71
#define VIDO_B01                    72
#define LG_MULTIX                   73
#define ENKEL                       74
#define YT_PELCOD                   75
#define  HIKVISION				    76
#define  PE60						77
#define  LiAo						78
#define  NK16                       79
#define  DaLi                       80
#define  HN_4304                    81
#define  VIDEOTEC                   82
#define  HNDCB                      83
#define  Lion_2007                  84
#define  LG_LVC_C372                85
#define  Gold_Video                 86
#define  NVD1600PS                  87

//������
typedef struct{
	DWORD dwSize;
	DWORD dwBaudRate;//������(bps)��0��50��1��75��2��110��3��150��4��300��5��600��6��1200��7��2400��8��4800��9��9600��10��19200�� 11��38400��12��57600��13��76800��14��115.2k;
	BYTE byDataBit;// �����м�λ 0��5λ��1��6λ��2��7λ��3��8λ;
	BYTE byStopBit;// ֹͣλ 0��1λ��1��2λ;
	BYTE byParity;// У�� 0����У�飬1����У�飬2��żУ��;
	BYTE byFlowcontrol;// 0���ޣ�1��������,2-Ӳ����
	WORD wDecoderType;//����������, 0��YouLi��1��LiLin-1016��2��LiLin-820��3��Pelco-p��4��DM DynaColor��5��HD600��6��JC-4116��7��Pelco-d WX��8��Pelco-d PICO
	WORD wDecoderAddress;	/*��������ַ:0 - 255*/
	BYTE bySetPreset[MAX_PRESET];		/* Ԥ�õ��Ƿ�����,0-û������,1-����*/
	BYTE bySetCruise[MAX_PRESET];		/* Ѳ���Ƿ�����: 0-û������,1-���� */
	BYTE bySetTrack[MAX_PRESET];		/* �켣�Ƿ�����,0-û������,1-����*/
}NET_DVR_DECODERCFG,*LPNET_DVR_DECODERCFG;

//RS232����
typedef struct{
	char sRemoteIP[16];            //Զ��IP��ַ
	char sLocalIP[16];             //����IP��ַ
	char sLocalIPMask[16];         //����IP��ַ����
	BYTE sUsername[NAME_LEN];		/* �û��� */
	BYTE sPassword[PASSWD_LEN];		/* ���� */
	BYTE byPPPMode;            //PPPģʽ, 0��������1������
	BYTE byRedial;            //�Ƿ�ز� ��0-��,1-��
	BYTE byRedialMode;        //�ز�ģʽ,0-�ɲ�����ָ��,1-Ԥ�ûز�����
	BYTE byDataEncrypt;	     //���ݼ���,0-��,1-��
	DWORD dwMTU;              //MTU
	char sTelephoneNumber[PHONENUMBER_LEN];   //�绰����
}NET_DVR_PPPCFG,*LPNET_DVR_PPPCFG;

typedef struct{
	DWORD dwSize;
	DWORD dwBaudRate;//������(bps)��0��50��1��75��2��110��3��150��4��300��5��600��6��1200��7��2400��8��4800��9��9600��10��19200�� 11��38400��12��57600��13��76800��14��115.2k;
	BYTE byDataBit;// �����м�λ 0��5λ��1��6λ��2��7λ��3��8λ;
	BYTE byStopBit;// ֹͣλ 0��1λ��1��2λ;
	BYTE byParity;// У�� 0����У�飬1����У�飬2��żУ��;
	BYTE byFlowcontrol;// 0���ޣ�1��������,2-Ӳ����
	DWORD dwWorkMode;// ����ģʽ��0��խ������(232��������PPP����)��1������̨(232�������ڲ�������)��2��͸��ͨ��
	NET_DVR_PPPCFG struPPPConfig;
}NET_DVR_RS232CFG,*LPNET_DVR_RS232CFG;

//��������
typedef struct{
	DWORD dwSize;
	BYTE sAlarmInName[NAME_LEN];	/* ���� */
	BYTE byAlarmType;	//����������,0������,1������
	BYTE byAlarmInHandle;	/* �Ƿ��� */
	NET_DVR_HANDLEEXCEPTION struAlarmHandleType;	/* ����ʽ */
	NET_DVR_SCHEDTIME struAlarmTime[MAX_DAYS][MAX_TIMESEGMENT];//����ʱ��
	BYTE byRelRecordChan[MAX_CHANNUM]; //����������¼��ͨ��,Ϊ1��ʾ������ͨ��
	BYTE byEnablePreset[MAX_CHANNUM];		/* �Ƿ����Ԥ�õ� */
	BYTE byPresetNo[MAX_CHANNUM];			/* ���õ���̨Ԥ�õ����,һ������������Ե��ö��ͨ������̨Ԥ�õ�, 0xff��ʾ������Ԥ�õ㡣*/
	BYTE byEnableCruise[MAX_CHANNUM];		/* �Ƿ����Ѳ�� */
	BYTE byCruiseNo[MAX_CHANNUM];			/* Ѳ�� */
	BYTE byEnablePtzTrack[MAX_CHANNUM];		/* �Ƿ���ù켣 */
	BYTE byPTZTrack[MAX_CHANNUM];			/* ���õ���̨�Ĺ켣��� */
}NET_DVR_ALARMINCFG,*LPNET_DVR_ALARMINCFG;

//�ϴ�������Ϣ
typedef struct{
	DWORD dwAlarmType;/*0-�ź�������,1-Ӳ����,2-�źŶ�ʧ,3���ƶ����,4��Ӳ��δ��ʽ��,5-��дӲ�̳���,6-�ڵ�����,7-��ʽ��ƥ��, 8-�Ƿ�����*/
	DWORD dwAlarmInputNumber;/*��������˿�*/
	DWORD dwAlarmOutputNumber[MAX_ALARMOUT];/*��������˿ڶ�Ӧ������˿ڣ���һλΪ1��ʾ��Ӧ��һ�����*/
	DWORD dwAlarmRelateChannel[MAX_CHANNUM];/*��������˿ڶ�Ӧ��¼��ͨ������һλΪ1��ʾ��Ӧ��һ·¼��,dwAlarmRelateChannel[0]��Ӧ��1��ͨ��*/
	DWORD dwChannel[MAX_CHANNUM];/*dwAlarmTypeΪ2��3ʱ����ʾ�ĸ�ͨ����dwChannel[0]λ��Ӧ��0��ͨ��*/
	DWORD dwDiskNumber[MAX_DISKNUM];/*dwAlarmTypeΪ4ʱ,��ʾ�ĸ�Ӳ��*/
}NET_DVR_ALARMINFO,*LPNET_DVR_ALARMINFO;

//DVR�������
typedef struct{
	DWORD dwSize;
	BYTE sAlarmOutName[NAME_LEN];	/* ���� */
	DWORD dwAlarmOutDelay;	/* �������ʱ��(-1Ϊ���ޣ��ֶ��ر�) */
	NET_DVR_SCHEDTIME struAlarmOutTime[MAX_DAYS][MAX_TIMESEGMENT];/* �����������ʱ��� */
}NET_DVR_ALARMOUTCFG,*LPNET_DVR_ALARMOUTCFG;

//DVR����Ԥ������
typedef struct{
	DWORD dwSize;
	BYTE byPreviewNumber;//Ԥ����Ŀ,0-1����,1-4����,2-9����,3-16����,0xff:�����
	BYTE byEnableAudio;//�Ƿ�����Ԥ��,0-��Ԥ��,1-Ԥ��
	WORD wSwitchTime;//�л�ʱ��,0-���л�,1-5s,2-10s,3-20s,4-60s,5-120s,6-300s
	BYTE bySwitchSeq[MAX_WINDOW];//�л�˳��,���lSwitchSeq[i]Ϊ 0xff��ʾ����
}NET_DVR_PREVIEWCFG,*LPNET_DVR_PREVIEWCFG;

//DVR��Ƶ���
typedef struct {
	WORD wResolution;							/* �ֱ��� */
	WORD wFreq;								/* ˢ��Ƶ�� */
	DWORD dwBrightness;							/* ���� */
}NET_DVR_VGAPARA;

typedef struct{
	WORD wDisplayLogo;						/* ��ʾ��Ƶͨ���� */
	WORD wDisplayOsd;						/* ��ʾʱ�� */
}NET_DVR_MATRIXPARA;

typedef struct{
	BYTE byVideoFormat;						/* �����ʽ,0-PAL,1-N	*/
	BYTE byMenuAlphaValue;					/* �˵��뱳��ͼ��Աȶ� */
	WORD wScreenSaveTime;					/* ��Ļ����ʱ�� */
	WORD wVOffset;							/* ��Ƶ���ƫ�� */
	WORD wBrightness;						/* ��Ƶ������� */
	BYTE byStartMode;						/* ��������Ƶ���ģʽ(0:�˵�,1:Ԥ��)*/
	char reservedData;
}NET_DVR_VOOUT;

typedef struct {
	DWORD dwSize;
	NET_DVR_VOOUT struVOOut[MAX_VIDEOOUT];
	NET_DVR_VGAPARA struVGAPara[MAX_VGA];	/* VGA���� */
	NET_DVR_MATRIXPARA struMatrixPara;		/* MATRIX���� */
}NET_DVR_VIDEOOUT,*LPNET_DVR_VIDEOOUT;

//DVR�û�����
typedef struct{
	BYTE sUserName[NAME_LEN];		/* �û��� */
	BYTE sPassword[PASSWD_LEN];		/* ���� */
	DWORD dwLocalRight[MAX_RIGHT];	/* Ȩ�� */
									/*����0: ���ؿ�����̨*/
									/*����1: �����ֶ�¼��*/
									/*����2: ���ػط�*/
									/*����3: �������ò���*/
									/*����4: ���ز鿴״̬����־*/
									/*����5: ���ظ߼�����(��������ʽ�����������ػ�)*/
	DWORD dwRemoteRight[MAX_RIGHT];	/* Ȩ�� */
									/*����0: Զ�̿�����̨*/
									/*����1: Զ���ֶ�¼��*/
									/*����2: Զ�̻ط�*/
									/*����3: Զ�����ò���*/
									/*����4: Զ�̲鿴״̬����־*/
									/*����5: Զ�̸߼�����(��������ʽ�����������ػ�)*/
									/*����6: Զ�̷��������Խ�*/
									/*����7: Զ��Ԥ��*/
									/*����8: Զ�����󱨾��ϴ����������*/
									/*����9: Զ�̿��ƣ��������*/
									/*����10: Զ�̿��ƴ���*/
	char sUserIP[16];					/* �û�IP��ַ(Ϊ0ʱ��ʾ�����κε�ַ) */
	BYTE byMACAddr[MACADDR_LEN];		/* �����ַ */
}NET_DVR_USER_INFO,*LPNET_DVR_USER_INFO;

typedef struct{
	DWORD dwSize;
	NET_DVR_USER_INFO struUser[MAX_USERNUM];
}NET_DVR_USER,*LPNET_DVR_USER;

//DVR�û�������չ
typedef struct{
	BYTE sUserName[NAME_LEN];		/* �û��� */
	BYTE sPassword[PASSWD_LEN];		/* ���� */
	DWORD dwLocalRight[MAX_RIGHT];	/* Ȩ�� */
	/*����0: ���ؿ�����̨*/
	/*����1: �����ֶ�¼��*/
	/*����2: ���ػط�*/
	/*����3: �������ò���*/
	/*����4: ���ز鿴״̬����־*/
	/*����5: ���ظ߼�����(��������ʽ�����������ػ�)*/
	DWORD dwLocalPlaybackRight;		/* ���ؿ��Իطŵ�ͨ�� bit0 -- channel 1*/
	DWORD dwRemoteRight[MAX_RIGHT];	/* Ȩ�� */
	/*����0: Զ�̿�����̨*/
	/*����1: Զ���ֶ�¼��*/
	/*����2: Զ�̻ط� */
	/*����3: Զ�����ò���*/
	/*����4: Զ�̲鿴״̬����־*/
	/*����5: Զ�̸߼�����(��������ʽ�����������ػ�)*/
	/*����6: Զ�̷��������Խ�*/
	/*����7: Զ��Ԥ��*/
	/*����8: Զ�����󱨾��ϴ����������*/
	/*����9: Զ�̿��ƣ��������*/
	/*����10: Զ�̿��ƴ���*/
	DWORD dwNetPreviewRight;		/* Զ�̿���Ԥ����ͨ�� bit0 -- channel 1*/
	DWORD dwNetPlaybackRight;		/* Զ�̿��Իطŵ�ͨ�� bit0 -- channel 1*/
	char sUserIP[16];				/* �û�IP��ַ(Ϊ0ʱ��ʾ�����κε�ַ) */
	BYTE byMACAddr[MACADDR_LEN];	/* �����ַ */
}NET_DVR_USER_INFO_EX,*LPNET_DVR_USER_INFO_EX;

typedef struct{
	DWORD dwSize;
	NET_DVR_USER_INFO_EX struUser[MAX_USERNUM];
}NET_DVR_USER_EX,*LPNET_DVR_USER_EX;

//DVR�쳣����
typedef struct{
	DWORD dwSize;
	NET_DVR_HANDLEEXCEPTION struExceptionHandleType[MAX_EXCEPTIONNUM];
	/*����0-����,1- Ӳ�̳���,2-���߶�,3-��������IP ��ַ��ͻ,4-�Ƿ�����, 5-����/�����Ƶ��ʽ��ƥ��*/
}NET_DVR_EXCEPTION,*LPNET_DVR_EXCEPTION;

typedef struct
{
	char sFileName[100];//�ļ���
	NET_DVR_TIME struStartTime;//�ļ��Ŀ�ʼʱ��
	NET_DVR_TIME struStopTime;//�ļ��Ľ���ʱ��
	DWORD dwFileSize;//�ļ��Ĵ�С
}NET_DVR_FIND_DATA,*LPNET_DVR_FIND_DATA;

typedef struct tagFINDDATA_CARD
{
	char sFileName[100];//�ļ���
	NET_DVR_TIME struStartTime;//�ļ��Ŀ�ʼʱ��
	NET_DVR_TIME struStopTime;//�ļ��Ľ���ʱ��
	DWORD dwFileSize;//�ļ��Ĵ�С
	char sCardNum[32];
}NET_DVR_FINDDATA_CARD,*LPNET_DVR_FINDDATA_CARD;

typedef struct {
	BYTE byRecordStatic; //ͨ���Ƿ���¼��,0-��¼��,1-¼��
	BYTE bySignalStatic; //���ӵ��ź�״̬,0-����,1-�źŶ�ʧ
	BYTE byHardwareStatic;//ͨ��Ӳ��״̬,0-����,1-�쳣,����DSP����
	char reservedData;
	DWORD dwBitRate;//ʵ������
	DWORD dwLinkNum;//�ͻ������ӵĸ���
	DWORD dwClientIP[MAX_LINK];//�ͻ��˵�IP��ַ
}NET_DVR_CHANNELSTATE,*LPNET_DVR_CHANNELSTATE;

typedef struct {
	DWORD dwVolume;//Ӳ�̵�����
	DWORD dwFreeSpace;//Ӳ�̵�ʣ��ռ�
	DWORD dwHardDiskStatic; //Ӳ�̵�״̬,����,�,��������
}NET_DVR_DISKSTATE,*LPNET_DVR_DISKSTATE;

typedef struct{
	DWORD dwDeviceStatic; 	//�豸��״̬,0-����,1-CPUռ����̫��,����85%,2-Ӳ������,���紮������
	NET_DVR_DISKSTATE  struHardDiskStatic[MAX_DISKNUM];
	NET_DVR_CHANNELSTATE struChanStatic[MAX_CHANNUM];//ͨ����״̬
	BYTE  byAlarmInStatic[MAX_ALARMIN]; //�����˿ڵ�״̬,0-û�б���,1-�б���
	BYTE  byAlarmOutStatic[MAX_ALARMOUT]; //��������˿ڵ�״̬,0-û�����,1-�б������
	DWORD  dwLocalDisplay;//������ʾ״̬,0-����,1-������
}NET_DVR_WORKSTATE,*LPNET_DVR_WORKSTATE;

//DVR��־
/* ���� */
//������
#define MAJOR_ALARM				0x1
//������
#define MINOR_ALARM_IN			0x1		/* �������� */
#define MINOR_ALARM_OUT			0x2		/* ������� */
#define MINOR_MOTDET_START		0x3		/* �ƶ���ⱨ����ʼ */
#define MINOR_MOTDET_STOP		0x4		/* �ƶ���ⱨ������ */
#define MINOR_HIDE_ALARM_START	0x5		/* �ڵ�������ʼ */
#define MINOR_HIDE_ALARM_STOP	0x6		/* �ڵ��������� */

/* �쳣 */
//������
#define MAJOR_EXCEPTION			0x2
//������
#define MINOR_VI_LOST			0x21	/* �źŶ�ʧ */
#define MINOR_ILLEGAL_ACCESS	0x22	/* �Ƿ����� */
#define MINOR_HD_FULL			0x23	/* Ӳ���� */
#define MINOR_HD_ERROR			0x24	/* Ӳ�̴��� */
#define MINOR_DCD_LOST			0x25	/* MODEM ���� */
#define MINOR_IP_CONFLICT		0x26	/* IP��ַ��ͻ */
#define MINOR_NET_BROKEN		0x27	/* ���߶� */

/* ���� */
//������
#define MAJOR_OPERATION			0x3
//������
#define MINOR_START_DVR			0x41	/* ���� */
#define MINOR_STOP_DVR			0x42	/* �ػ� */
#define MINOR_STOP_ABNORMAL		0x43	/* �Ƿ��ػ� */

#define MINOR_LOCAL_LOGIN		0x50	/* ���ص�½ */
#define MINOR_LOCAL_LOGOUT		0x51	/* ����ע����½ */
#define MINOR_LOCAL_CFG_PARM	0x52	/* �������ò��� */
#define MINOR_LOCAL_PLAYBYFILE	0x53	/* ���ذ��ļ��ط� */
#define MINOR_LOCAL_PLAYBYTIME	0x54	/* ���ذ�ʱ��ط� */
#define MINOR_LOCAL_START_REC	0x55	/* ���ؿ�ʼ¼�� */
#define MINOR_LOCAL_STOP_REC	0x56	/* ����ֹͣ¼�� */
#define MINOR_LOCAL_PTZCTRL		0x57	/* ������̨���� */
#define MINOR_LOCAL_PREVIEW		0x58	/* ����Ԥ�� */
#define MINOR_LOCAL_MODIFY_TIME	0x59	/* �����޸�ʱ�� */
#define MINOR_LOCAL_UPGRADE		0x5a	/* �������� */
#define MINOR_LOCAL_COPYFILE	0x5b	/* ���ر����ļ� */

#define MINOR_REMOTE_LOGIN		0x70	/* Զ�̵�¼ */
#define MINOR_REMOTE_LOGOUT		0x71	/* Զ��ע����½ */
#define MINOR_REMOTE_START_REC	0x72	/* Զ�̿�ʼ¼�� */
#define MINOR_REMOTE_STOP_REC	0x73	/* Զ��ֹͣ¼�� */
#define MINOR_START_TRANS_CHAN	0x74	/* ��ʼ͸������ */
#define MINOR_STOP_TRANS_CHAN	0x75	/* ֹͣ͸������ */
#define MINOR_REMOTE_GET_PARM	0x76	/* Զ�̻�ò��� */
#define MINOR_REMOTE_CFG_PARM	0x77	/* Զ�����ò��� */
#define MINOR_REMOTE_GET_STATUS 0x78	/* Զ�̻��״̬ */
#define MINOR_REMOTE_ARM		0x79	/* Զ�̲��� */
#define MINOR_REMOTE_DISARM		0x7a	/* Զ�̳��� */
#define MINOR_REMOTE_REBOOT		0x7b	/* Զ������ */
#define MINOR_START_VT			0x7c	/* ��ʼ�����Խ� */
#define MINOR_STOP_VT			0x7d	/* ֹͣ�����Խ� */
#define MINOR_REMOTE_UPGRADE	0x7e	/* Զ������ */
#define MINOR_REMOTE_PLAYBYFILE	0x7f	/* Զ�̰��ļ��ط� */
#define MINOR_REMOTE_PLAYBYTIME	0x80	/* Զ�̰�ʱ��ط� */
#define MINOR_REMOTE_PTZCTRL	0x81	/* Զ����̨���� */
//����־��������ΪMAJOR_OPERATION=03��������ΪMINOR_LOCAL_CFG_PARM=0x52����MINOR_REMOTE_GET_PARM=0x76����MINOR_REMOTE_CFG_PARM=0x77ʱ��dwParaType:����������Ч���京�����£�
#define PARA_VIDEOOUT 0x1
#define PARA_IMAGE  0x2
#define PARA_ENCODE  0x4
#define PARA_NETWORK 0x8
#define PARA_ALARM  0x10
#define PARA_EXCEPTION 0x20
#define PARA_DECODER 0x40    /*������*/
#define PARA_RS232  0x80
#define PARA_PREVIEW 0x100
#define PARA_SECURITY 0x200
#define PARA_DATETIME 0x400
#define PARA_FRAMETYPE 0x800    /*֡��ʽ*/

typedef struct {
	NET_DVR_TIME strLogTime;
	DWORD	dwMajorType;	//������ 1-����; 2-�쳣; 3-����; 0xff-ȫ��
	DWORD	dwMinorType;//������ 0xff-ȫ��;
	BYTE	sPanelUser[MAX_NAMELEN]; //���������û���
	BYTE	sNetUser[MAX_NAMELEN];//����������û���
	char	sRemoteHostAddr[16];//Զ��������ַ
	DWORD	dwParaType;//��������
	DWORD	dwChannel;//ͨ����
	DWORD	dwDiskNumber;//Ӳ�̺�
	DWORD	dwAlarmInPort;//��������˿�
	DWORD	dwAlarmOutPort;//��������˿�
}NET_DVR_LOG,*LPNET_DVR_LOG;

typedef struct {
	char sRedirectorIP[16];	// ת��������IP��ַ
	WORD wRedirectorPort;	//�˿ں�
}NET_DVR_REDIRECTORINFO, *LPNET_DVR_REDIRECTORINFO;

typedef struct{
	BYTE Output[MAX_ALARMOUT];
}NET_DVR_ALARMOUTSTATUS, *LPNET_DVR_ALARMOUTSTATUS;

typedef struct{
	USHORT m_Year;
	USHORT m_Month;
	USHORT m_Day;
	USHORT m_Hour;
	USHORT m_Minute;
	USHORT m_Second;
	BYTE DeviceName[24];	//�豸����
	DWORD dwChannelNumer;	//ͨ����
	BYTE CardNumber[32];	//����
	char cTradeType[12];	//��������
	DWORD dwCash;			//���׽��
}NET_DVR_TRADEINFO, *LPNET_DVR_TRADEINFO;

//����ΪATMר��
/*֡��ʽ*/
typedef struct{
	BYTE code[12];		/* ���� */
}NET_DVR_FRAMETYPECODE;

typedef struct{
	DWORD dwSize;
	char sATMIP[16];						/* ATM IP��ַ */
	DWORD dwATMType;						/* ATM���� */
	DWORD dwInputMode;						/* ���뷽ʽ	*/
	DWORD dwFrameSignBeginPos;              /* ���ı�־λ����ʼλ��*/
	DWORD dwFrameSignLength;				/* ���ı�־λ�ĳ��� */
	BYTE  byFrameSignContent[12];			/* ���ı�־λ������ */
	DWORD dwCardLengthInfoBeginPos;			/* ���ų�����Ϣ����ʼλ�� */
	DWORD dwCardLengthInfoLength;			/* ���ų�����Ϣ�ĳ��� */
	DWORD dwCardNumberInfoBeginPos;			/* ������Ϣ����ʼλ�� */
	DWORD dwCardNumberInfoLength;			/* ������Ϣ�ĳ��� */
	DWORD dwBusinessTypeBeginPos;           /* �������͵���ʼλ�� */
	DWORD dwBusinessTypeLength;				/* �������͵ĳ��� */
	NET_DVR_FRAMETYPECODE frameTypeCode[10];/* ���� */
}NET_DVR_FRAMEFORMAT, *LPNET_DVR_FRAMEFORMAT;

//DS-6001D/F
/*�����豸�����붨��*/
#define NET_DEC_STARTDEC		1
#define NET_DEC_STOPDEC			2
#define NET_DEC_STOPCYCLE		3
#define NET_DEC_CONTINUECYCLE	4

/*���ӵ�ͨ������*/
typedef struct {
	char sDVRIP[16];				/* DVR IP��ַ */
	WORD wDVRPort;			 		/* �˿ں� */
	BYTE sUserName[NAME_LEN];		/* �û��� */
	BYTE sPassword[PASSWD_LEN];		/* ���� */
	BYTE byChannel;					/* ͨ���� */
	BYTE byLinkMode;				/* ����ģʽ */
	BYTE byLinkType;				/* �������� 0�������� 1�������� */
}NET_DVR_DECCHANINFO, *LPNET_DVR_DECCHANINFO;

/*ÿ������ͨ��������*/
typedef struct {
	BYTE	byPoolChans;			/*ÿ·����ͨ���ϵ�ѭ��ͨ������, ���4ͨ��*/
	NET_DVR_DECCHANINFO struchanConInfo[MAX_DECPOOLNUM];
	BYTE	byEnablePoll;			/*�Ƿ���Ѳ 0-�� 1-��*/
	BYTE	byPoolTime;				/*��Ѳʱ�� 0-���� 1-10�� 2-15�� 3-20�� 4-30�� 5-45�� 6-1���� 7-2���� 8-5���� */
}NET_DVR_DECINFO, *LPNET_DVR_DECINFO;

/*�����豸��������*/
typedef struct {
	DWORD	dwSize;
	DWORD	dwDecChanNum; 		/*����ͨ��������*/
	NET_DVR_DECINFO struDecInfo[MAX_DECNUM];
}NET_DVR_DECCFG, *LPNET_DVR_DECCFG;

//2005-08-01
/* �����豸͸��ͨ������ */
typedef struct {
	DWORD dwEnableTransPort;	/* �Ƿ�����͸��ͨ�� 0�������� 1������*/
	char sDecoderIP[16];		/* DVR IP��ַ */
	WORD wDecoderPort;			/* �˿ں� */
	WORD wDVRTransPort;			/* ����ǰ��DVR�Ǵ�485/232�����1��ʾ485����,2��ʾ232���� */
	char cReserve[4];
}NET_DVR_PORTINFO, *LPNET_DVR_PORTINFO;

typedef struct {
	DWORD dwSize;
	NET_DVR_PORTINFO struTransPortInfo[MAX_TRANSPARENTNUM]; /* ����0��ʾ485 ����1��ʾ232 */
}NET_DVR_PORTCFG, *LPNET_DVR_PORTCFG;

/* ���������ļ��ط� */
typedef struct {
	DWORD dwSize;
	char sDecoderIP[16];		/* DVR IP��ַ */
	WORD wDecoderPort;			/* �˿ں� */
	WORD wLoadMode;				/* �ط�����ģʽ 1�������� 2����ʱ�� */
	union
	{
		BYTE byFile[100];		/* �طŵ��ļ��� */
		struct
		{
			DWORD dwChannel;
			BYTE sUserName[NAME_LEN];	/*������Ƶ�û���*/
			BYTE sPassword[PASSWD_LEN];	/* ���� */
			NET_DVR_TIME struStartTime;	/* ��ʱ��طŵĿ�ʼʱ�� */
			NET_DVR_TIME struStopTime;	/* ��ʱ��طŵĽ���ʱ�� */
		}bytime;
	}mode_size;
}NET_DVR_PLAYREMOTEFILE,*LPNET_DVR_PLAYREMOTEFILE;

/*��ǰ�豸��������״̬*/
typedef struct {
	DWORD dwWorkType;		/*������ʽ��1����Ѳ��2����̬���ӽ��롢3�����ļ��ط� 4����ʱ��ط�*/
	char sDVRIP[16];		/*���ӵ��豸ip*/
	WORD wDVRPort;			/*���Ӷ˿ں�*/
	BYTE byChannel;					/* ͨ���� */
	BYTE byLinkMode;				/* ����ģʽ */
	DWORD	dwLinkType;		/*�������� 0�������� 1��������*/
	union
	{
		struct
		{
			BYTE sUserName[NAME_LEN];	/*������Ƶ�û���*/
			BYTE sPassword[PASSWD_LEN];	/* ���� */
			char cReserve[52];
		}userInfo;
		struct
		{
			BYTE   fileName[100];
		}fileInfo;
		struct
		{
			DWORD	dwChannel;
			BYTE	sUserName[NAME_LEN];	/*������Ƶ�û���*/
			BYTE	sPassword[PASSWD_LEN];	/* ���� */
			NET_DVR_TIME struStartTime;		/* ��ʱ��طŵĿ�ʼʱ�� */
			NET_DVR_TIME struStopTime;		/* ��ʱ��طŵĽ���ʱ�� */
		}timeInfo;
	}objectInfo;
}NET_DVR_DECCHANSTATUS, *LPNET_DVR_DECCHANSTATUS;

typedef struct {
	DWORD   dwSize;
	NET_DVR_DECCHANSTATUS struDecState[MAX_DECNUM];
}NET_DVR_DECSTATUS, *LPNET_DVR_DECSTATUS;

//�����ַ�
typedef struct
{
	WORD wShowString;				// Ԥ����ͼ�����Ƿ���ʾ�ַ�,0-����ʾ,1-��ʾ �����С704*576,�����ַ��Ĵ�СΪ32*32
	WORD wStringSize;				/* �����ַ��ĳ��ȣ����ܴ���44���ַ� */
	WORD wShowStringTopLeftX;		/* �ַ���ʾλ�õ�x���� */
	WORD wShowStringTopLeftY;		/* �ַ�������ʾλ�õ�y���� */
	char sString[44];				/* Ҫ��ʾ���ַ����� */
}NET_DVR_SHOWSTRINGINFO, *LPNET_DVR_SHOWSTRINGINFO;

//�����ַ�
typedef struct
{
	DWORD dwSize;
	NET_DVR_SHOWSTRINGINFO struStringInfo[MAX_STRINGNUM];				/* Ҫ��ʾ���ַ����� */
}NET_DVR_SHOWSTRING, *LPNET_DVR_SHOWSTRING;

/* ftp �ϴ�����*/
typedef struct
{
	DWORD	dwSize;
	DWORD	dwEnableFTP;			/*�Ƿ�����ftp�ϴ�����*/
	char	sFTPIP[16];				/*ftp ������*/
	DWORD	dwFTPPort;				/*ftp�˿�*/
	BYTE	sUserName[NAME_LEN];	/*�û���*/
	BYTE	sPassword[PASSWD_LEN];	/*����*/
	//BYTE	reservedData[32];
	DWORD	dwDirLevel;	/*0 = ��ʹ��Ŀ¼�ṹ��ֱ�ӱ����ڸ�Ŀ¼,1 = ʹ��1��Ŀ¼,2=ʹ��2��Ŀ¼*/
	WORD 	wTopDirMode;	/* 0x1 = ʹ���豸��,0x2 = ʹ���豸��,0x3 = ʹ���豸ip��ַ*/
	WORD 	wSubDirMode;	/* 0x4 = ʹ��ͨ����,0x5 = ʹ��ͨ����*/
	BYTE 	reservedData[24];
}NET_DVR_FTPCFG, *LPNET_DVR_FTPCFG;

/*����������ͼƬ�����Ԫ�� */
#define PICNAME_ITEM_DEV_NAME 	1
#define PICNAME_ITEM_DEV_NO 	2
#define PICNAME_ITEM_DEV_IP 	3
#define PICNAME_ITEM_CHAN_NAME 	4
#define PICNAME_ITEM_CHAN_NO 	5
#define PICNAME_ITEM_TIME 		6
#define PICNAME_ITEM_CARDNO		7
#define PICNAME_MAXITEM 		15

typedef struct
{
	BYTE 	byItemOrder[PICNAME_MAXITEM];	/*	�����鶨���ļ������Ĺ��� */
	BYTE 	byDelimiter;		/*�ָ�����һ��Ϊ'_'*/
}NET_DVR_PICTURE_NAME, *LPNET_DVR_PICTURE_NAME;

//ͼƬ����
typedef struct
{
	WORD	wPicSize;				/* 0=CIF, 1=QCIF, 2=D1 */
	WORD	wPicQuality;			/* ͼƬ����ϵ�� 0-��� 1-�Ϻ� 2-һ�� */
}NET_DVR_JPEGPARA, *LPNET_DVR_JPEGPARA;

/* ����ץͼ����*/
typedef struct
{
	BYTE	byStrFlag;	/*�������ݿ�ʼ��*/
	BYTE 	byEndFlag;	/*������*/
	WORD	wCardIdx;	/*���������ʼλ*/
	DWORD 	dwCardLen;	/*���ų���*/
	DWORD   dwTriggerPicChans;	/*��������ͨ���ţ���λ���ӵ�1λ��ʼ�ƣ���0x2��ʾ��һͨ��*/
}NET_DVR_SERIAL_CATCHPIC_PARA, *LPNET_DVR_SERIAL_CATCHPIC_PARA;

typedef struct
{
	DWORD	dwSize;
	NET_DVR_JPEGPARA struJpegPara[MAX_CHANNUM];	/*ÿ��ͨ����ͼ�����*/
	WORD	wBurstMode;							/*ץͼ��ʽ,��λ����.0x1=�������봥����0x2=�ƶ���ⴥ�� 0x4=232������0x8=485������0x10=���紥��*/
	WORD	wUploadInterval;					/*ͼƬ�ϴ����(��)[0,65535]*/
	NET_DVR_PICTURE_NAME 	struPicNameRule;	/* ͼƬ�������� */
	BYTE	bySaveToHD;		/*�Ƿ񱣴浽Ӳ��*/
	BYTE	res1;
	WORD	wCatchInterval;		/*ץͼ���(����)[0,65535]*/
	BYTE	res2[12];
	NET_DVR_SERIAL_CATCHPIC_PARA struRs232Cfg;
	NET_DVR_SERIAL_CATCHPIC_PARA struRs485Cfg;
	DWORD dwTriggerPicTimes[MAX_CHANNUM];	/* ÿ��ͨ��һ�δ������մ��� */
	DWORD dwAlarmInPicChanTriggered[MAX_ALARMIN]; /*��������ץ��ͨ��,��λ���ã��ӵ�0λ��ʼ*/
}NET_DVR_JPEGCFG, *LPNET_DVR_JPEGCFG;

typedef struct
{
	DWORD dwReturn;		/*�Ƿ�ֱ�ӷ���ͼƬ����*/
	DWORD dwUploadFtp;	/*�Ƿ��ϴ�Ftp������*/
	DWORD dwSaveHd;		/*�Ƿ񱣴浽�豸Ӳ��*/
	char sImageName[PATHNAME_LEN];	/*�Զ��������ͼƬ�ļ���*/
	char sOsdStr[16];	/*��Ҫ������ͼƬ�ϵ�OSD�ַ���*/
	WORD wOsdLen;		/*OSD�ַ�������*/
}NET_DVR_GETJPEG, *LPNET_DVR_GETJPEG;

//HS�豸�����������
typedef struct
{
	DWORD dwSize;
	DWORD dwAlarmOutChan;                       	/* ѡ�񱨾�������������ͨ��: 0:�����/1:��1/2:��2/3:��3/4:��4 */
	DWORD dwAlarmChanSwitchTime;                	/* ����ͨ���л�ʱ�䣺1:1�� - 10:10�� */
	DWORD dwAuxSwitchTime[MAX_AUXOUT];		/* ��������л�ʱ��: 0-���л�,1-5s,2-10s,3-20s,4-30s,5-60s,6-120s,7-300s */
	BYTE  byAuxOrder[MAX_AUXOUT][MAX_WINDOW];	/* �������Ԥ��˳��, 0xff��ʾ��Ӧ�Ĵ��ڲ�Ԥ�� */
}NET_DVR_AUXOUTCFG, *LPNET_DVR_AUXOUTCFG;

typedef struct tagPOINT_FRAME
{
	int xTop; // ������ʼ���x����
	int yTop; // ����������y����
	int xBottom; // ����������x����
	int yBottom; //����������y����
	int bCounter; //����
}NET_DVR_POINT_FRAME, *LPNET_DVR_POINT_FRAME;

typedef struct tagCRUISE_RET{
	struct{
		BYTE	PresetNum;	//Ԥ�õ�
		BYTE	Dwell;		//ͣ��ʱ��
		BYTE	Speed;		//�ٶ�
		BYTE	Reserve;	//����
	}struCruisePoint[32];			//���֧��32��Ѳ����
}NET_DVR_CRUISE_RET, *LPNET_DVR_CRUISE_RET;

//2007-05-23 ��·������ added by zxy
typedef struct tagNETCFG_OTHER{
	DWORD	dwSize;
	char	sFirstDNSIP[16];
	char	sSecondDNSIP[16];
	char	sRes[32];
}NET_DVR_NETCFG_OTHER,*LPNET_DVR_NETCFG_OTHER;

typedef struct tagDVRCHANINFO{
	char 	sDVRIP[16];				/* DVR IP��ַ */
	WORD 	wDVRPort;			 	/* �˿ں� */
	BYTE 	byChannel;				/* ͨ���� */
	BYTE	byTransProtocol;			/* ����Э������ */
	BYTE	byTransMode;				/* ��������ģʽ 0�������� 1��������*/
	BYTE	byRes[3];
	BYTE	sUserName[NAME_LEN];			/* ���������½�ʺ� */
	BYTE	sPassword[PASSWD_LEN];			/* ����������� */
}NET_DVR_MATRIX_DECINFO,*LPNET_DVR_MATRIX_DECINFO;

//����/ֹͣ��̬����
typedef struct tagDYNAMIC_TEST{
	DWORD	dwSize;				
	NET_DVR_MATRIX_DECINFO struDecChanInfo;		/* ��̬����ͨ����Ϣ */
}NET_DVR_MATRIX_DYNAMIC_DEC,*LPNET_DVR_MATRIX_DYNAMIC_DEC;

typedef struct  tagMATRIX_DEC_CHAN_STATUS
{
    DWORD 	dwSize;
    DWORD   dwIsLinked;         /* Network link status tag, 0: hasn't linked yet, 1: linked */
    DWORD   dwStreamCpRate;     /* Stream copy rate, X kbits/second */
    char    cRes[64];		/* ���� */
}NET_DVR_MATRIX_DEC_CHAN_STATUS,*LPNET_DVR_MATRIX_DEC_CHAN_STATUS;

typedef struct tagDEC_MATRIX_CHAN_INFO
{
	DWORD	dwSize;
	NET_DVR_MATRIX_DECINFO struDecChanInfo;		/* ����ͨ����Ϣ */
	DWORD	dwDecState;	/* 0-��̬���� 1��ѭ������ 2�����ļ��ط� 3����ʱ��ط� */
	NET_DVR_TIME StartTime;		/* ��ʱ��طſ�ʼʱ�� */
	NET_DVR_TIME StopTime;		/* ��ʱ��ط�ֹͣʱ�� */
	char    sFileName[128];		/* ���ļ��ط��ļ��� */
}NET_DVR_MATRIX_DEC_CHAN_INFO,*LPNET_DVR_MATRIX_DEC_CHAN_INFO;

//���ӵ�ͨ������ 2007-11-05
typedef struct tagMATRIXDECCHANINFO{
	DWORD dwEnable;					/* �Ƿ����� 0���� 1������*/
	NET_DVR_MATRIX_DECINFO struDecChanInfo;		/* ��ѭ����ͨ����Ϣ */
}NET_DVR_MATRIX_DECCHANINFO, *LPNET_DVR_MATRIX_DECCHANINFO;

//2007-11-05 ����ÿ������ͨ��������
typedef struct tagLOOPDECINFO{
	DWORD	dwSize;
	DWORD	dwPoolTime;			/*��Ѳʱ�� */
	NET_DVR_MATRIX_DECCHANINFO struchanConInfo[MAX_CYCLE_CHAN];
}NET_DVR_MATRIX_LOOP_DECINFO, *LPNET_DVR_MATRIX_LOOP_DECINFO;

typedef struct tagEMAILCFG
{	/* 12 bytes */
	DWORD	dwSize;
	char	sUserName[32];
	char 	sPassWord[32];
	char 	sFromName[32];			/* Sender *///�ַ����еĵ�һ���ַ������һ���ַ�������"@",�����ַ�����Ҫ��"@"�ַ�
	char 	sFromAddr[48];			/* Sender address */
	char 	sToName1[32];			/* Receiver1 */
	char 	sToName2[32];			/* Receiver2 */
	char 	sToAddr1[48];			/* Receiver address1 */
	char 	sToAddr2[48];			/* Receiver address2 */
	char	sEmailServer[32];		/* Email server address */
	BYTE	byServerType;			/* Email server type: 0-SMTP, 1-POP, 2-IMTP��*/
	BYTE	byUseAuthen;			/* Email server authentication method: 1-enable, 0-disable */
	BYTE	byAttachment;			/* enable attachment */
	BYTE	byMailinterval;			/* mail interval 0-2s, 1-3s, 2-4s. 3-5s*/
} NET_DVR_EMAILCFG, *LPNET_DVR_EMAILCFG;

typedef struct tagNTPPARA{
	BYTE sNTPServer[64];   /* Domain Name or IP addr of NTP server */
	WORD wInterval;		 /* adjust time interval(hours) */
	BYTE byEnableNTP;    /* enable NPT client 0-no��1-yes*/
	signed char cTimeDifferenceH; /* ����ʱ�׼ʱ���ʱ�� -12 ... +13 */
	signed char cTimeDifferenceM;
	BYTE res[11];
}NET_DVR_NTPPARA, *LPNET_DVR_NTPPARA;

typedef struct tagDDNSPARA{
	BYTE sUsername[NAME_LEN];  /* DDNS�˺��û���/���� */
	BYTE sPassword[PASSWD_LEN];
	BYTE sDomainName[64];       /* ���� */
	BYTE byEnableDDNS;			/*�Ƿ�Ӧ�� 0-��1-��*/
	BYTE res[15];
}NET_DVR_DDNSPARA, *LPNET_DVR_DDNSPARA;

typedef struct tagNETAPPCFG{
	DWORD  dwSize;
	char  sDNSIp[16];                /* DNS��������ַ */
	NET_DVR_NTPPARA  struNtpClientParam;      /* NTP���� */
	NET_DVR_DDNSPARA struDDNSClientParam;     /* DDNS���� */
	BYTE res[464];			/* ���� */
}NET_DVR_NETAPPCFG, *LPNET_DVR_NETAPPCFG;

typedef struct tag_NFSCFG{
	DWORD  dwSize;
	struct{
		char sNfsHostIPAddr[16];
		BYTE sNfsDirectory[PATHNAME_LEN];        // PATHNAME_LEN = 128
	}struNfsDiskParam[MAX_NFS_DISK];
}NET_DVR_NFSCFG, *LPNET_DVR_NFSCFG;

NET_DVR_API BOOL __stdcall NET_DVR_Init();
NET_DVR_API BOOL __stdcall NET_DVR_Cleanup();
NET_DVR_API BOOL __stdcall NET_DVR_SetDVRMessage(UINT nMessage,HWND hWnd);
NET_DVR_API BOOL __stdcall NET_DVR_SetDVRMessCallBack(BOOL (CALLBACK *fMessCallBack)(LONG lCommand,char *sDVRIP,char *pBuf,DWORD dwBufLen));
NET_DVR_API BOOL __stdcall NET_DVR_SetDVRMessCallBack_EX(BOOL (CALLBACK *fMessCallBack_EX)(LONG lCommand,LONG lUserID,char *pBuf,DWORD dwBufLen));
NET_DVR_API BOOL __stdcall NET_DVR_SetDVRMessCallBack_NEW(BOOL (CALLBACK *fMessCallBack_NEW)(LONG lCommand,char *sDVRIP,char *pBuf,DWORD dwBufLen, WORD dwLinkDVRPort));
NET_DVR_API BOOL __stdcall NET_DVR_SetDVRMessageCallBack(BOOL (CALLBACK *fMessageCallBack)(LONG lCommand,char *sDVRIP,char *pBuf,DWORD dwBufLen, DWORD dwUser), DWORD dwUser);
NET_DVR_API BOOL __stdcall NET_DVR_SetConnectTime(DWORD dwWaitTime,DWORD dwTryTimes);
NET_DVR_API DWORD __stdcall NET_DVR_GetSDKVersion();
NET_DVR_API int __stdcall NET_DVR_IsSupport();
NET_DVR_API BOOL __stdcall NET_DVR_StartListen(char *sLocalIP,WORD wLocalPort);
NET_DVR_API BOOL __stdcall NET_DVR_StopListen();
NET_DVR_API LONG __stdcall NET_DVR_Login(char *sDVRIP,WORD wDVRPort,char *sUserName,char *sPassword,LPNET_DVR_DEVICEINFO lpDeviceInfo);
NET_DVR_API BOOL __stdcall NET_DVR_Logout(LONG lUserID);
NET_DVR_API DWORD __stdcall NET_DVR_GetLastError();
NET_DVR_API BOOL __stdcall NET_DVR_SetShowMode(DWORD dwShowType,COLORREF colorKey);
NET_DVR_API BOOL __stdcall NET_DVR_GetDVRIPByResolveSvr(char *sServerIP, WORD wServerPort, BYTE *sDVRName,WORD wDVRNameLen,BYTE *sDVRSerialNumber,WORD wDVRSerialLen,char* sGetIP);
//ͼ��Ԥ��
NET_DVR_API LONG __stdcall NET_DVR_RealPlay(LONG lUserID,LPNET_DVR_CLIENTINFO lpClientInfo);
NET_DVR_API BOOL __stdcall NET_DVR_StopRealPlay(LONG lRealHandle);
NET_DVR_API int __stdcall NET_DVR_GetRealPlayerIndex(LONG lRealHandle);
//��Ƶ����������ֵ 1-10
NET_DVR_API BOOL __stdcall NET_DVR_ClientSetVideoEffect(LONG lRealHandle,DWORD dwBrightValue,DWORD dwContrastValue, DWORD dwSaturationValue,DWORD dwHueValue);
NET_DVR_API BOOL __stdcall NET_DVR_ClientGetVideoEffect(LONG lRealHandle,DWORD *pBrightValue,DWORD *pContrastValue, DWORD *pSaturationValue,DWORD *pHueValue);
NET_DVR_API BOOL __stdcall NET_DVR_RigisterDrawFun(LONG lRealHandle,void (CALLBACK* fDrawFun)(LONG lRealHandle,HDC hDc,DWORD dwUser),DWORD dwUser);
NET_DVR_API BOOL __stdcall NET_DVR_SetPlayerBufNumber(LONG lRealHandle,DWORD dwBufNum);
NET_DVR_API BOOL __stdcall NET_DVR_ThrowBFrame(LONG lRealHandle,DWORD dwNum);
NET_DVR_API BOOL __stdcall NET_DVR_SetAudioMode(DWORD dwMode);
NET_DVR_API BOOL __stdcall NET_DVR_OpenSound(LONG lRealHandle);
NET_DVR_API BOOL __stdcall NET_DVR_CloseSound();
NET_DVR_API BOOL __stdcall NET_DVR_OpenSoundShare(LONG lRealHandle);
NET_DVR_API BOOL __stdcall NET_DVR_CloseSoundShare(LONG lRealHandle);
NET_DVR_API BOOL __stdcall NET_DVR_Volume(LONG lRealHandle,WORD wVolume);
NET_DVR_API BOOL __stdcall NET_DVR_SaveRealData(LONG lRealHandle,char *sFileName);
NET_DVR_API BOOL __stdcall NET_DVR_StopSaveRealData(LONG lRealHandle);
NET_DVR_API BOOL __stdcall NET_DVR_SetRealDataCallBack(LONG lRealHandle,void(CALLBACK *fRealDataCallBack) (LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,DWORD dwUser),DWORD dwUser);
NET_DVR_API BOOL __stdcall NET_DVR_MakeKeyFrame(LONG lUserID, LONG lChannel);
NET_DVR_API BOOL __stdcall NET_DVR_MakeKeyFrameSub(LONG lUserID, LONG lChannel);
NET_DVR_API BOOL __stdcall NET_DVR_RefreshPlay(LONG lPlayHandle);
NET_DVR_API BOOL __stdcall NET_DVR_CapturePicture(LONG lRealHandle,char *sPicFileName);

//��̨����
NET_DVR_API BOOL __stdcall NET_DVR_PTZControl(LONG lRealHandle,DWORD dwPTZCommand,DWORD dwStop);
NET_DVR_API BOOL __stdcall NET_DVR_PTZControl_Other(LONG lUserID,LONG lChannel,DWORD dwPTZCommand,DWORD dwStop);
NET_DVR_API BOOL __stdcall NET_DVR_TransPTZ(LONG lRealHandle,char *pPTZCodeBuf,DWORD dwBufSize);
NET_DVR_API BOOL __stdcall NET_DVR_TransPTZ_Other(LONG lUserID,LONG lChannel,char *pPTZCodeBuf,DWORD dwBufSize);
NET_DVR_API BOOL __stdcall NET_DVR_PTZPreset(LONG lRealHandle,DWORD dwPTZPresetCmd,DWORD dwPresetIndex);
NET_DVR_API BOOL __stdcall NET_DVR_PTZPreset_Other(LONG lUserID,LONG lChannel,DWORD dwPTZPresetCmd,DWORD dwPresetIndex);
NET_DVR_API BOOL __stdcall NET_DVR_TransPTZ_EX(LONG lRealHandle,char *pPTZCodeBuf,DWORD dwBufSize);
NET_DVR_API BOOL __stdcall NET_DVR_PTZControl_EX(LONG lRealHandle,DWORD dwPTZCommand,DWORD dwStop);
NET_DVR_API BOOL __stdcall NET_DVR_PTZPreset_EX(LONG lRealHandle,DWORD dwPTZPresetCmd,DWORD dwPresetIndex);
NET_DVR_API BOOL __stdcall NET_DVR_PTZCruise(LONG lRealHandle,DWORD dwPTZCruiseCmd,BYTE byCruiseRoute, BYTE byCruisePoint, WORD wInput);
NET_DVR_API BOOL __stdcall NET_DVR_PTZCruise_Other(LONG lUserID,LONG lChannel,DWORD dwPTZCruiseCmd,BYTE byCruiseRoute, BYTE byCruisePoint, WORD wInput);
NET_DVR_API BOOL __stdcall NET_DVR_PTZCruise_EX(LONG lRealHandle,DWORD dwPTZCruiseCmd,BYTE byCruiseRoute, BYTE byCruisePoint, WORD wInput);
NET_DVR_API BOOL __stdcall NET_DVR_PTZTrack(LONG lRealHandle,DWORD dwPTZTrackCmd);
NET_DVR_API BOOL __stdcall NET_DVR_PTZTrack_Other(LONG lUserID,LONG lChannel,DWORD dwPTZTrackCmd);
NET_DVR_API BOOL __stdcall NET_DVR_PTZTrack_EX(LONG lRealHandle,DWORD dwPTZTrackCmd);
//���ٶȵ���̨����
NET_DVR_API BOOL __stdcall NET_DVR_PTZControlWithSpeed(LONG lRealHandle, DWORD dwPTZCommand, DWORD dwStop, DWORD dwSpeed);
NET_DVR_API BOOL __stdcall NET_DVR_PTZControlWithSpeed_Other(LONG lUserID, LONG lChannel, DWORD dwPTZCommand, DWORD dwStop, DWORD dwSpeed);
//2007-05-11 IP����
NET_DVR_API BOOL __stdcall NET_DVR_GetPTZCruise(LONG lUserID,LONG lChannel,LONG lCruiseRoute, LPNET_DVR_CRUISE_RET lpCruiseRet);
NET_DVR_API BOOL __stdcall NET_DVR_PTZSelZoomIn(LONG lRealHandle, LPNET_DVR_POINT_FRAME pStruPointFrame);

//�ļ��ط�
NET_DVR_API LONG __stdcall NET_DVR_FindFile(LONG lUserID,LONG lChannel,DWORD dwFileType, LPNET_DVR_TIME lpStartTime, LPNET_DVR_TIME lpStopTime);
NET_DVR_API LONG __stdcall NET_DVR_FindNextFile(LONG lFindHandle,LPNET_DVR_FIND_DATA lpFindData);
NET_DVR_API BOOL __stdcall NET_DVR_FindClose(LONG lFindHandle);
NET_DVR_API LONG __stdcall NET_DVR_FindFileByCard(LONG lUserID,LONG lChannel,DWORD dwFileType, BOOL bNeedCardNum, BYTE *sCardNumber, LPNET_DVR_TIME lpStartTime, LPNET_DVR_TIME lpStopTime);
NET_DVR_API LONG __stdcall NET_DVR_PlayBackByName(LONG lUserID,char *sPlayBackFileName, HWND hWnd);
NET_DVR_API LONG __stdcall NET_DVR_PlayBackByTime(LONG lUserID,LONG lChannel, LPNET_DVR_TIME lpStartTime, LPNET_DVR_TIME lpStopTime, HWND hWnd);
NET_DVR_API BOOL __stdcall NET_DVR_PlayBackControl(LONG lPlayHandle,DWORD dwControlCode,DWORD dwInValue,DWORD *lpOutValue);
NET_DVR_API BOOL __stdcall NET_DVR_StopPlayBack(LONG lPlayHandle);
NET_DVR_API BOOL __stdcall NET_DVR_SetPlayDataCallBack(LONG lPlayHandle,void(CALLBACK *fPlayDataCallBack) (LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,DWORD dwUser),DWORD dwUser);
NET_DVR_API BOOL __stdcall NET_DVR_PlayBackSaveData(LONG lPlayHandle,char *sFileName);
NET_DVR_API BOOL __stdcall NET_DVR_StopPlayBackSave(LONG lPlayHandle);
NET_DVR_API BOOL __stdcall NET_DVR_GetPlayBackOsdTime(LONG lPlayHandle, LPNET_DVR_TIME lpOsdTime);
NET_DVR_API BOOL __stdcall NET_DVR_PlayBackCaptureFile(LONG lPlayHandle,char *sFileName);
NET_DVR_API LONG __stdcall NET_DVR_GetFileByName(LONG lUserID,char *sDVRFileName,char *sSavedFileName);
NET_DVR_API LONG __stdcall NET_DVR_GetFileByTime(LONG lUserID,LONG lChannel, LPNET_DVR_TIME lpStartTime, LPNET_DVR_TIME lpStopTime, char *sSavedFileName);
NET_DVR_API BOOL __stdcall NET_DVR_StopGetFile(LONG lFileHandle);
NET_DVR_API int __stdcall NET_DVR_GetDownloadPos(LONG lFileHandle);
NET_DVR_API int __stdcall NET_DVR_GetPlayBackPlayerIndex(LONG lPlayHandle);

//�ָ�Ĭ��ֵ
NET_DVR_API BOOL __stdcall NET_DVR_RestoreConfig(LONG lUserID);
//�������
NET_DVR_API BOOL __stdcall NET_DVR_SaveConfig(LONG lUserID);
//����
NET_DVR_API BOOL __stdcall NET_DVR_RebootDVR(LONG lUserID);
//�ر�DVR
NET_DVR_API BOOL __stdcall NET_DVR_ShutDownDVR(LONG lUserID);
//����
NET_DVR_API LONG __stdcall NET_DVR_Upgrade(LONG lUserID,char *sFileName);
NET_DVR_API int __stdcall NET_DVR_GetUpgradeState(LONG lUpgradeHandle);
NET_DVR_API BOOL __stdcall NET_DVR_CloseUpgradeHandle(LONG lUpgradeHandle);
//Զ�̸�ʽ��Ӳ��
NET_DVR_API LONG __stdcall NET_DVR_FormatDisk(LONG lUserID,LONG lDiskNumber);
NET_DVR_API BOOL __stdcall NET_DVR_GetFormatProgress(LONG lFormatHandle,LONG *pCurrentFormatDisk,LONG *pCurrentDiskPos,LONG *pFormatStatic);
NET_DVR_API BOOL __stdcall NET_DVR_CloseFormatHandle(LONG lFormatHandle);
//����
NET_DVR_API LONG __stdcall NET_DVR_SetupAlarmChan(LONG lUserID);
NET_DVR_API BOOL __stdcall NET_DVR_CloseAlarmChan(LONG lAlarmHandle);
NET_DVR_API BOOL __stdcall NET_DVR_GetAlarmOut(LONG lUserID,LPNET_DVR_ALARMOUTSTATUS lpAlarmOutState);
NET_DVR_API BOOL __stdcall NET_DVR_SetAlarmOut(LONG lUserID,LONG lAlarmOutPort,LONG lAlarmOutStatic);
//�����Խ�
NET_DVR_API LONG __stdcall NET_DVR_StartVoiceCom(LONG lUserID, void(CALLBACK *fVoiceDataCallBack)(LONG lVoiceComHandle,char *pRecvDataBuffer,DWORD dwBufSize,BYTE byAudioFlag,DWORD dwUser), DWORD dwUser);
NET_DVR_API BOOL __stdcall NET_DVR_SetVoiceComClientVolume(LONG lVoiceComHandle,WORD wVolume);
NET_DVR_API BOOL __stdcall NET_DVR_StopVoiceCom(LONG lVoiceComHandle);
//�����㲥
NET_DVR_API BOOL __stdcall NET_DVR_ClientAudioStart();
NET_DVR_API BOOL __stdcall NET_DVR_ClientAudioStop();
NET_DVR_API BOOL __stdcall NET_DVR_AddDVR(LONG lUserID);
NET_DVR_API BOOL __stdcall NET_DVR_DelDVR(LONG lUserID);
//����ת��
NET_DVR_API LONG __stdcall NET_DVR_StartVoiceCom_MR(LONG lUserID, void(CALLBACK *fVoiceDataCallBack)(LONG lVoiceComHandle,char *pRecvDataBuffer,DWORD dwBufSize,BYTE byAudioFlag,DWORD dwUser), DWORD dwUser);
NET_DVR_API BOOL __stdcall NET_DVR_VoiceComSendData(LONG lVoiceComHandle,char *pSendBuf,DWORD dwBufSize);
////////////////////////////////////////////////////////////
//͸��ͨ������
NET_DVR_API LONG __stdcall NET_DVR_SerialStart(LONG lUserID,LONG lSerialPort,void(CALLBACK *fSerialDataCallBack)(LONG lSerialHandle,char *pRecvDataBuffer,DWORD dwBufSize,DWORD dwUser),DWORD dwUser);
NET_DVR_API BOOL __stdcall NET_DVR_SerialSend(LONG lSerialHandle, LONG lChannel, char *pSendBuf,DWORD dwBufSize);
NET_DVR_API BOOL __stdcall NET_DVR_SerialStop(LONG lSerialHandle);
NET_DVR_API BOOL __stdcall NET_DVR_SendTo232Port(LONG lUserID,char *pSendBuf,DWORD dwBufSize);
//Զ�̿��Ʊ�����ʾ
NET_DVR_API BOOL __stdcall NET_DVR_ClickKey(LONG lUserID,LONG lKeyIndex);
//Զ�̿����ֶ�¼��
NET_DVR_API BOOL __stdcall NET_DVR_StartDVRRecord(LONG lUserID,LONG lChannel,LONG lRecordType);
NET_DVR_API BOOL __stdcall NET_DVR_StopDVRRecord(LONG lUserID,LONG lChannel);
//���뿨
NET_DVR_API BOOL __stdcall NET_DVR_InitDevice_Card(long *pDeviceTotalChan);
NET_DVR_API BOOL __stdcall NET_DVR_ReleaseDevice_Card();
NET_DVR_API BOOL __stdcall NET_DVR_InitDDraw_Card(HWND hParent,COLORREF colorKey);
NET_DVR_API BOOL __stdcall NET_DVR_ReleaseDDraw_Card();
NET_DVR_API LONG __stdcall NET_DVR_RealPlay_Card(LONG lUserID,LPNET_DVR_CARDINFO lpCardInfo,long lChannelNum);
NET_DVR_API BOOL __stdcall NET_DVR_ResetPara_Card(LONG lRealHandle,LPNET_DVR_DISPLAY_PARA lpDisplayPara);
NET_DVR_API BOOL __stdcall NET_DVR_RefreshSurface_Card();
NET_DVR_API BOOL __stdcall NET_DVR_ClearSurface_Card();
NET_DVR_API BOOL __stdcall NET_DVR_RestoreSurface_Card();
NET_DVR_API BOOL __stdcall NET_DVR_OpenSound_Card(LONG lRealHandle);
NET_DVR_API BOOL __stdcall NET_DVR_CloseSound_Card(LONG lRealHandle);
NET_DVR_API BOOL __stdcall NET_DVR_SetVolume_Card(LONG lRealHandle,WORD wVolume);
NET_DVR_API BOOL __stdcall NET_DVR_AudioPreview_Card(LONG lRealHandle,BOOL bEnable);
NET_DVR_API BOOL __stdcall NET_DVR_CapturePicture_Card(LONG lRealHandle, char *sPicFileName);
NET_DVR_API BOOL __stdcall NET_DVR_SetDspErrMsg_Card(UINT nMessage,HWND hWnd);
NET_DVR_API BOOL __stdcall NET_DVR_ResetDSP_Card(long lChannelNum);
NET_DVR_API BOOL __stdcall NET_DVR_GetSerialNum_Card(long lChannelNum,DWORD *pDeviceSerialNo);
NET_DVR_API HANDLE __stdcall NET_DVR_GetChanHandle_Card(LONG lRealHandle);

//������״̬
NET_DVR_API BOOL __stdcall NET_DVR_GetDVRWorkState(LONG lUserID,LPNET_DVR_WORKSTATE lpWorkState);
//��־
NET_DVR_API LONG __stdcall NET_DVR_FindDVRLog(LONG lUserID,LONG lSelectMode, DWORD dwMajorType,DWORD dwMinorType, LPNET_DVR_TIME lpStartTime, LPNET_DVR_TIME lpStopTime);
NET_DVR_API LONG __stdcall NET_DVR_FindNextLog(LONG lLogHandle,LPNET_DVR_LOG lpLogData);
NET_DVR_API BOOL __stdcall NET_DVR_FindLogClose(LONG lLogHandle);
//��������
NET_DVR_API BOOL __stdcall NET_DVR_GetDVRConfig(LONG lUserID, DWORD dwCommand,LONG lChannel, LPVOID lpOutBuffer,DWORD dwOutBufferSize,LPDWORD lpBytesReturned);
NET_DVR_API BOOL __stdcall NET_DVR_SetDVRConfig(LONG lUserID, DWORD dwCommand,LONG lChannel, LPVOID lpInBuffer,DWORD dwInBufferSize);

NET_DVR_API BOOL __stdcall NET_DVR_ClientGetframeformat(LONG lUserID, LPNET_DVR_FRAMEFORMAT lpFrameFormat);
NET_DVR_API BOOL __stdcall NET_DVR_ClientSetframeformat(LONG lUserID, LPNET_DVR_FRAMEFORMAT lpFrameFormat);

//����
NET_DVR_API void* __stdcall NET_DVR_InitG722Decoder(int nBitrate);
NET_DVR_API void __stdcall NET_DVR_ReleaseG722Decoder(void *pDecHandle);
NET_DVR_API BOOL __stdcall NET_DVR_DecodeG722Frame(void *pDecHandle, unsigned char* pInBuffer, unsigned char* pOutBuffer);
//����
NET_DVR_API void* __stdcall NET_DVR_InitG722Encoder();
NET_DVR_API BOOL __stdcall NET_DVR_EncodeG722Frame(void *pEncodeHandle,unsigned char* pInBuffer, unsigned char* pOutBuffer);
NET_DVR_API void __stdcall NET_DVR_ReleaseG722Encoder(void *pEncodeHandle);
//��������
NET_DVR_API BOOL __stdcall NET_DVR_GetConfigFile(LONG lUserID, char *sFileName);
NET_DVR_API BOOL __stdcall NET_DVR_SetConfigFile(LONG lUserID, char *sFileName);
NET_DVR_API BOOL __stdcall NET_DVR_GetConfigFile_EX(LONG lUserID, char *sOutBuffer, DWORD dwOutSize);
NET_DVR_API BOOL __stdcall NET_DVR_SetConfigFile_EX(LONG lUserID, char *sInBuffer, DWORD dwInSize);
//6001D/F�����豸�ӿ�
NET_DVR_API BOOL __stdcall NET_DVR_SetDecInfo(LONG lUserID, LONG lChannel, LPNET_DVR_DECCFG lpDecoderinfo);
NET_DVR_API BOOL __stdcall NET_DVR_GetDecInfo(LONG lUserID, LONG lChannel, LPNET_DVR_DECCFG lpDecoderinfo);
NET_DVR_API BOOL __stdcall NET_DVR_SetDecTransPort(LONG lUserID, LPNET_DVR_PORTCFG lpTransPort);
NET_DVR_API BOOL __stdcall NET_DVR_GetDecTransPort(LONG lUserID, LPNET_DVR_PORTCFG lpTransPort);
NET_DVR_API BOOL __stdcall NET_DVR_DecPlayBackCtrl(LONG lUserID, LONG lChannel, DWORD dwControlCode, DWORD dwInValue,DWORD *lpOutValue, LPNET_DVR_PLAYREMOTEFILE lpRemoteFileInfo);
NET_DVR_API BOOL __stdcall NET_DVR_StartDecSpecialCon(LONG lUserID, LONG lChannel, LPNET_DVR_DECCHANINFO lpDecChanInfo);
NET_DVR_API BOOL __stdcall NET_DVR_StopDecSpecialCon(LONG lUserID, LONG lChannel, LPNET_DVR_DECCHANINFO lpDecChanInfo);
NET_DVR_API BOOL __stdcall NET_DVR_DecCtrlDec(LONG lUserID, LONG lChannel, DWORD dwControlCode);
NET_DVR_API BOOL __stdcall NET_DVR_DecCtrlScreen(LONG lUserID, LONG lChannel, DWORD dwControl);
NET_DVR_API BOOL __stdcall NET_DVR_GetDecCurLinkStatus(LONG lUserID, LONG lChannel, LPNET_DVR_DECSTATUS lpDecStatus);

//JPEGץͼ
NET_DVR_API BOOL __stdcall NET_DVR_CaptureJPEGPicture(LONG lUserID, LONG lChannel, LPNET_DVR_JPEGPARA lpJpegPara, char *sPicFileName);
NET_DVR_API BOOL __stdcall NET_DVR_CaptureJPEGPicture_NEW(LONG lUserID, LONG lChannel, LPNET_DVR_JPEGPARA lpJpegPara, char *sJpegPicBuffer, DWORD dwPicSize,  LPDWORD lpSizeReturned);

//����ATM PORT����
NET_DVR_API BOOL __stdcall NET_DVR_SetATMPortCFG(LONG lUserID, WORD wATMPort);
//��ȡATM PORT����
NET_DVR_API BOOL __stdcall NET_DVR_GetATMPortCFG(LONG lUserID, WORD *lpOutATMPort);

//�����Ƿ��������Žӿ�
//������������
NET_DVR_API BOOL __stdcall NET_DVR_SetScaleCFG(LONG lUserID, DWORD dwScale);
//��ȡ��������
NET_DVR_API BOOL __stdcall NET_DVR_GetScaleCFG(LONG lUserID, DWORD *lpOutScale);
//֧���Կ�
//#if (WINVER > 0x0400)
//Note: These funtion must be builded under win2000 or above with Microsoft Platform sdk.
//	    You can download the sdk from "http://www.microsoft.com/msdownload/platformsdk/sdkupdate/";
NET_DVR_API BOOL __stdcall NET_DVR_InitDDrawDevice();
NET_DVR_API BOOL __stdcall NET_DVR_ReleaseDDrawDevice();
NET_DVR_API LONG __stdcall NET_DVR_GetDDrawDeviceTotalNums();
NET_DVR_API BOOL __stdcall NET_DVR_SetDDrawDevice(LONG lPlayPort, DWORD nDeviceNum);
//#endif
//��·������
NET_DVR_API BOOL __stdcall NET_DVR_MatrixStartDynamic(LONG lUserID, DWORD dwDecChanNum, LPNET_DVR_MATRIX_DYNAMIC_DEC lpDynamicInfo);
NET_DVR_API BOOL __stdcall NET_DVR_MatrixStopDynamic(LONG lUserID, DWORD dwDecChanNum);
NET_DVR_API BOOL __stdcall NET_DVR_MatrixGetDecChanStatus(LONG lUserID, DWORD dwDecChanNum, LPNET_DVR_MATRIX_DEC_CHAN_STATUS lpInter);
NET_DVR_API BOOL __stdcall NET_DVR_MatrixGetDecChanInfo(LONG lUserID, DWORD dwDecChanNum, LPNET_DVR_MATRIX_DEC_CHAN_INFO lpInter);
NET_DVR_API BOOL __stdcall NET_DVR_MatrixSetLoopDecChanInfo(LONG lUserID, DWORD dwDecChanNum, LPNET_DVR_MATRIX_LOOP_DECINFO lpInter);
NET_DVR_API BOOL __stdcall NET_DVR_MatrixGetLoopDecChanInfo(LONG lUserID, DWORD dwDecChanNum, LPNET_DVR_MATRIX_LOOP_DECINFO lpInter);
NET_DVR_API BOOL __stdcall NET_DVR_MatrixSetLoopDecChanEnable(LONG lUserID, DWORD dwDecChanNum, DWORD dwEnable);
NET_DVR_API BOOL __stdcall NET_DVR_MatrixGetLoopDecChanEnable(LONG lUserID, DWORD dwDecChanNum, LPDWORD lpdwEnable);
NET_DVR_API BOOL __stdcall NET_DVR_MatrixGetLoopDecEnable(LONG lUserID, LPDWORD lpdwEnable);
NET_DVR_API BOOL __stdcall NET_DVR_MatrixSetDecChanEnable(LONG lUserID, DWORD dwDecChanNum, DWORD dwEnable);
NET_DVR_API BOOL __stdcall NET_DVR_MatrixGetDecChanEnable(LONG lUserID, DWORD dwDecChanNum, LPDWORD lpdwEnable);
#endif
