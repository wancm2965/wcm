/************************************************************************/
/* HPDVRStruct.h                                                        */
/************************************************************************/
#pragma once

#include <map>
#include <string>
using namespace std;

//////////////////////////////////////////////////////////////////////////
// + �ṹ�� [2/27/2008]

/*
*	NDVR
*/

typedef struct tagDVR_STATE/*DVR״̬*/
{
	UINT size;						
	LONG cpu_usage;/*cpuռ����*/
	LONG mem_usage;/*�ڴ�ռ����*/
	LONG part_space;/*�����ռ�*/
	LONG up_speed;/*��������*/
	LONG down_speed;/*��������*/
	LONG thread_num;/*�߳���*/
	LONG issued_num;/*������*/
	CHAR boot_time[24];/*����ʱ��*/
	LONG reboot_num;/*��������*/
	LONG state;/*״̬*/
}HD_DVR_STATE, *LPHD_DVR_STATE;

typedef struct tagDVR_CONNECT/*DVR����*/
{
	UINT size;						
	CHAR server[64];/*����*/
	CHAR ip0[64];/*IP0*/
	CHAR ip1[64];/*IP1*/
	CHAR ip2[64];/*IP2*/
	LONG port;/*�˿�*/
}HD_DVR_CONNECT, *LPHD_DVR_CONNECT;

typedef struct tagDVR_INFO/*DVR��Ϣ*/
{
	UINT size;						
	CHAR name[64];/*����*/
	CHAR serial[48];/*���к�*/
	CHAR version[32];/*�汾��*/
	LONG dev_num;/*�豸��*/
	LONG chn_num;/*ͨ����*/
	LONG part_num;/*������*/
	LONG part_size;/*��������*/
	LONG uart_num;/*������*/
	LONG host_num;/*������*/
	LONG host_in_num;/*����˿���*/
	LONG host_out_num;/*����˿���*/
	HD_DVR_STATE state;/*״̬*/
	HD_DVR_CONNECT connect;/*����*/
}HD_DVR_INFO, *LPHD_DVR_INFO;


/*
 *	�豸
 */
// +�豸 [3/3/2008]
typedef struct tagCOD_STATE/*DEV״̬*/
{
	UINT size;						
	LONG thread_num;/*�߳���*/
	LONG issued_num;/*������*/
	CHAR boot_time[20];/*����ʱ��*/			
	LONG reboot_num;/*��������*/
	LONG state;/*״̬*/
}HD_COD_STATE, *LPHD_COD_STATE;

typedef struct tagCOD_CONNECT/*�豸����*/
{
	UINT size;						
	CHAR codid[32];						
	CHAR nvs_server[64];/*NVS ����*/
	CHAR nvs_ip[16];/*NVS IP*/
	LONG nvs_port;/*NVS �˿ں�*/
	CHAR nvs_uid[32];/*NVS �ʺ�*/
	CHAR nvs_pwd[32];/*NVS ����*/
	CHAR avc_uid[32];/*AVCON �ʺ�*/
	CHAR avc_pwd[32];/*AVCON ����*/
}HD_COD_CONNECT, *LPDH_COD_CONNECT;

typedef struct tagCOD_INFO/*�豸��Ϣ*/
{
	UINT size;						
	CHAR codid[32];						
	CHAR name[64];/*����*/
	LONG type;/*����*/
	LONG chn_num;/*ͨ����*/
	CHAR serial[48];/*���к�*/
	CHAR licence[20];/*��Ȩ��*/
	HD_COD_STATE state;/*״̬*/
	HD_COD_CONNECT connect;/*����*/
}HD_COD_INFO, *LPHD_COD_INFO;

typedef map<string, HD_COD_INFO>		HD_COD_MAP;
typedef HD_COD_MAP::iterator			HD_COD_MAP_IT;

// +ͨ�� [3/3/2008]
typedef struct tagCOD_CHN/*ͨ��*/
{
	UINT size;						
	CHAR codid[32];/*������*/
	LONG chnid;/*ͨ��*/
}HD_COD_CHN, *LPHD_COD_CHN;

typedef struct tagHD_MCU/*MCU*/
{
	CHAR mcuid[128];/*MCUID*/
	CHAR mcuaddr[64];/*MCU��ַ*/
	UINT mcuport;/*MCUPORT*/
	CHAR mcutype[32];/*MCUTYPE*/
}HD_MCU, *LPHD_MCU;

typedef struct tagCHN_CONNECT/*ͨ������*/
{
	UINT size;						
	CHAR codid[32];/*������*/
	LONG chnid;/*ͨ��*/
	LONG audid;/*��ƵID*/
	LONG vidid;/*��ƵID*/
	LONG subid;/*����ID*/
	HD_MCU mcu;/*MCU*/
	BOOL p2pfirst;/*p2p��MCU����*/
	BOOL audio_tcpfirst;/*TCP��UDP����*/
	BOOL video_tcpfirst;/*TCP��UDP����*/
	CHAR locaddr[16];/*������IP*/
	UINT tcpport;/*������UDP�����ö�̬�˿�*/
	UINT udpport;/*������UDP�����ö�̬�˿�*/
}HD_CHN_CONNECT, *LPHD_CHN_CONNECT;

typedef struct tagID_NAME
{
	UINT size;						
	LONG id;							
	CHAR desc[64];/*����*/
}HD_ID_NAME, *LPHD_ID_NAME;

//��Ƶ�豸
typedef HD_ID_NAME		HD_AUDIO_DEVICE;		
typedef LPHD_ID_NAME	LPHD_AUDIO_DEVICE;	

//��Ƶ�豸
typedef HD_ID_NAME		HD_VIDEO_DEVICE;		
typedef LPHD_ID_NAME	LPHD_VIDEO_DEVICE;	

//��Ƶ����
typedef HD_ID_NAME		HD_AUDIO_CODEC;		
typedef LPHD_ID_NAME	LPHD_AUDIO_CODEC;	

//��Ƶ����
typedef HD_ID_NAME		HD_VIDEO_CODEC;
typedef LPHD_ID_NAME	LPHD_VIDEO_CODEC;

//��Ƶ�ֱ���
typedef HD_ID_NAME		HD_VIDEO_SIZE;
typedef LPHD_ID_NAME	LPHD_VIDEO_SIZE;

//��Ƶ����
typedef HD_ID_NAME		HD_VIDEO_BITRATE;
typedef LPHD_ID_NAME	LPHD_VIDEO_BITRATE;

//��Ƶ֡��
typedef HD_ID_NAME		HD_VIDEO_FRAMERATE;
typedef LPHD_ID_NAME	LPHD_VIDEO_FRAMERATE;

typedef struct tagPTZ_CTL
{
	UINT size;
	LONG ptzcmd;
}HD_PTZ_CTL, *LPHD_PTZ_CTL;

typedef struct tagPRESET/*Ԥ�õ�*/
{
	UINT size;						
	CHAR codid[32];/*������*/
	LONG chnid;/*ͨ��*/
	LONG id;/*Ԥ�õ�*/
	CHAR name[64];/*����*/
	BOOL def_point;/*Ĭ�ϵ�*/
	LONG state;/*״̬*/
}HD_PRESET, *LPHD_PRESET;

typedef struct tagPTZ_PROTOCOL/*��̨Э��*/
{
	UINT size;						
	LONG id;/*Э��*/
	CHAR name[64];/*����*/
	LONG baud;/*����*/
	LONG parity;/*У�鷽ʽ*/
}HD_PTZ_PROTOCOL, *LPHD_PTZ_PROTOCOL;

typedef struct tagUART/*����*/
{
	UINT size;						
	CHAR name[64];/*������*/
	LONG port;/*���ں�*/
	LONG type;/*����*/
	LONG baud;/*����*/
	LONG databit;/*����λ*/
	LONG stopbit;/*ֹͣλ*/
	LONG parity;/*У�鷽ʽ*/
}HD_UART, *LPHD_UART;

typedef struct tagCHN_PTZ/*��̨*/
{
	UINT size;						
	CHAR codid[32];/*������*/
	LONG chnid;/*ͨ��*/
	LONG delay;/*ͣ��ʱ��(��Ĭ�ϵ�)*/
	LONG protocol;/*Э��*/
	LONG address;/*��ַ*/
	LONG uart;/*����*/
	LONG speed;/*�����ٶ�*/
	LONG baud;/*������*/
	LONG presetidx;/*Ԥ�õ�������*/
	LONG ctltype;/*�������� 0:��ͨ 1:ת��Ԥ�õ� 2:����Ԥ�õ� 3:���Ԥ�õ� 4:ȡԤ�õ�(����Ч)*/
}HD_CHN_PTZ, *LPHD_CHN_PTZ;

typedef struct tagCHN_BASIC/*������Ϣ*/
{
	UINT size;						
	CHAR codid[32];/*������*/
	LONG chnid;/*ͨ��(-1Ϊ�Խ�ͨ��)*/
	CHAR name[64];/*����*/
	LONG state;/*״̬���*/
	BOOL issue;/*�������*/
}HD_CHN_BASIC,*LPHD_CHN_BASIC;

typedef struct tagCHN_AUDIO/*ͨ����Ƶ*/
{
	UINT size;						
	CHAR codid[32];/*������*/
	LONG chnid;/*ͨ��*/
	LONG device;/*�豸*/
	CHAR desc[64];/*����*/
	LONG codec;/*����*/
	BOOL cap_card;/*�ɼ���Ƶ��*/
	LONG cap_mode;/*�ɼ�ģʽ��0:MIC��1:LINE_IN��2:WAVE_OUT_MIXER*/
	BOOL aec;/*��������*/
	BOOL agc;/*�Զ�����*/
	BOOL vad;/*�������*/
}HD_CHN_AUDIO,*LPHD_CHN_AUDIO;

typedef struct tagCHN_VSTREAM/*��Ƶ��*/
{
	LONG codec;/*����*/
	UINT resolution;/*�ֱ��� (PS��-1ʱ��ʾȡ�����ֱ��ʵ�1/4�ߴ�)*/
	LONG framerate;/*֡��*/
	LONG frametype;/*֡��ģʽ*/
	LONG quality;/*����*/
	LONG autoquality;/*ͼ�������Զ�����*/
}HD_CHN_VSTREAM, *LPHD_CHN_VSTREAM;

typedef struct tagCHN_VIDEO/*ͨ����Ƶ*/
{
	UINT size;						
	CHAR codid[32];/*������*/
	LONG chnid;/*ͨ��*/
	LONG device;/*�豸*/
	CHAR desc[64];/*����*/
	BOOL denoise;/*ͼ��ȥ��*/
	LONG brightness;/*������*/
	LONG contrast;/*�Աȶ�*/
	LONG saturation;/*���Ͷ�*/
	LONG hue;/*ɫ�ʶ�*/
	LONG gama;/*٤��*/
	LONG sharpen;/*��*/
	LONG substream;/*�Ƿ���������*/
	LONG tomcu;/*�Ƿ񷢲���MCU*/

	HD_CHN_VSTREAM hp_video[2];/*��ƽ*/
	NET_DVR_COMPRESSION_INFO hc_video[2];/*����*/
	DH_VIDEOENC_OPT dh_video[6]; /*�� 0,1,2¼������3,4,5������*/
	DWORD dwImageSizeMask; /*�ֱ������룬��λ��ʾ�豸�ܹ�֧�ֵķֱ���*/
	DWORD dwEncodeModeMask;/*����ģʽ���룬��λ��ʾ�豸�ܹ�֧�ֵı���ģʽ*/
}HD_CHN_VIDEO,*LPHD_CHN_VIDEO;

typedef struct tagCHN_AVINFO			//ͨ������Ƶ	
{
	UINT size;
	HD_CHN_AUDIO audio;/*��Ƶ����*/
	HD_CHN_VIDEO video;/*��Ƶ����*/
}HD_CHN_AVINFO,*LPHD_CHN_AVINFO;

typedef enum 
{
	HD_VIDEO, /*��Ƶ*/
	HD_AUDIO /*��Ƶ*/
}HDEM_AVTYPE; /*��Ƶ������Ƶ*/
typedef struct tagCHN_INFO/*ͨ����Ϣ*/
{
	UINT size;				
	CHAR codid[32];/*������*/
	LONG chnid;/*ͨ��*/
	HD_CHN_BASIC basic;/*������Ϣ*/
	HD_CHN_AUDIO audio;/*��Ƶ*/
	HD_CHN_VIDEO video;/*��Ƶ*/
	HD_CHN_PTZ ptz;/*��̨*/
	HD_CHN_CONNECT connect;/*����*/
	LONG hwnd;/*����ָ��*/
}HD_CHN_INFO, *LPHD_CHN_INFO;

typedef map<LONG, HD_CHN_INFO>		    HD_CHN_MAP;
typedef HD_CHN_MAP::iterator			HD_CHN_MAP_IT;
typedef map<string, HD_CHN_MAP>			HD_COD_CHN_MAP;
typedef HD_COD_CHN_MAP::iterator		HD_COD_CHN_MAP_IT;


// +������ [3/5/2008]
typedef struct tagVIDEO_RECT/*��Ƶ����*/
{
	UINT size;						
	LONG num;/*����*/
	RECT rects[5];/*����*/
}HD_VIDEO_RECT, *LPHD_VIDEO_RECT;

typedef struct tagVIDEO_MOTION/*�ƶ����*/
{
	UINT size;						
	CHAR codid[32];/*������*/
	LONG chnid;/*ͨ��*/
	LONG level;/*�ȼ�*/
	LONG state;/*״̬*/
	HD_VIDEO_RECT rects;/*����*/

	NET_DVR_MOTION hc_motion;/*����*/
	DH_MOTION_DETECT_CFG dh_motion;/*��*/
}HD_VIDEO_MOTION, *LPHD_VIDEO_MOTION;

typedef struct tagVIDEO_COVER/*��Ƶ����*/
{
	UINT size;						
	CHAR codid[32];/*������*/
	LONG chnid;/*ͨ��*/
	LONG state;/*״̬*/
	HD_VIDEO_RECT rects;/*����*/

	NET_DVR_SHELTER hc_cover[MAX_SHELTERNUM];/*����*/
	DHDEV_VIDEOCOVER_CFG dh_cover;/*��*/
}HD_VIDEO_COVER, *LPHD_VIDEO_COVER;

typedef struct tagVIDEO_BLOCK/*��Ƶ�ڵ�*/
{
	UINT size;						
	CHAR codid[32];/*�豸*/
	LONG chnid;/*ͨ��*/
	LONG state;/*״̬*/
	HD_VIDEO_RECT rects;/*����*/

	NET_DVR_HIDEALARM hc_block;/*����*/
	DH_BLIND_CFG dh_block;/*��*/
}HD_VIDEO_BLOCK, *LPHD_VIDEO_BLOCK;


/*
 *	����
 */
typedef struct tagALM_INPORT			//����˿�
{
	UINT size;
	CHAR host[32];						//����
	LONG port;							//�˿�
	CHAR desc[128];						//����
	LONG type;							//����
	LONG state;							//״̬
}HD_ALM_INPORT, *LPHD_ALM_INPORT;

typedef struct tagALM_OUTPORT			//����˿�
{
	UINT size;
	CHAR host[32];						//����
	LONG port;							//�˿�
	CHAR desc[128];						//����
	LONG type;							//����
	LONG state;							//״̬
}HD_ALM_OUTPORT, *LPHD_ALM_OUTPORT;

typedef struct tagALM_DEVICE			//�����豸
{
	UINT size;					
	CHAR host[32];						//����
	CHAR desc[128];						//����
	LONG type;							//����
	LONG uart;							//����
	LONG state;							//״̬
	HD_ALM_INPORT  inputs[16];			//����
	HD_ALM_OUTPORT outputs[16];			//���
}HD_ALM_DEVICE, *LPHD_ALM_DEVICE;

typedef enum
{
	EM_ALM_SRC_HOST,
	EM_ALM_SRC_VIDEO,
	EM_ALM_SRC_DISK,
	EM_ALM_SRC_SOFT
}HDEM_ALM_SRC_TYPE;

typedef struct tagALM_SOURCE			//����Դ
{
	UINT size;
	LONG id;
	CHAR desc[128];
	LONG type;
	LONG state;
	union
	{
		struct // ��������
		{
			CHAR host[32];
			LONG port;
		}host;

		struct // ��Ƶ����
		{
			CHAR codid[32];
			LONG chnid;
			LONG type;
		}video;

		struct // Ӳ���쳣
		{
			LONG type; 
		}disk;

		struct // ����쳣
		{
			LONG type;
		}soft;
	};
}HD_ALM_SOURCE, *LPHD_ALM_SOURCE;

typedef enum
{
	EM_ALM_TRG_HOST,
	EM_ALM_TRG_RECORD,
	EM_ALM_TRG_PICTURES,
	EM_ALM_TRG_PTZ,
	EM_ALM_TRG_CENTER,
	EM_ALM_TRG_CLIENT,
	EM_ALM_TRG_SMS,
	EM_ALM_TRG_EMAIL,
	EM_ALM_TRG_CALL
}HDEM_ALM_TRG_TYPE;

typedef struct tagALM_TARGET		// ����Ŀ��
{
	UINT size;
	LONG id;
	CHAR desc[128];
	LONG type;
	LONG state;
	union
	{
		struct // �������
		{
			CHAR host[32];
			LONG port;
		}host;

		struct // ����¼��
		{
			CHAR codid[32];
			LONG chnid;
		}record;

		struct // ��������
		{
			CHAR codid[32];
			LONG chnid;
			LONG photo_num;
			LONG step;
		}pictures;

		struct // ������̨
		{
			CHAR codid[32];
			LONG chnid;
			LONG prest;
		}ptz;

		struct // �ϴ�����
		{
			LONG type;
		}center;

		struct // ����֪ͨ
		{
			CHAR name[32];
			CHAR phone[16];
		}sms;

		struct // �ʼ�֪ͨ
		{
			CHAR name[32];
			CHAR mail[128];
		}email;

		struct // �绰֪ͨ
		{
			CHAR name[32];
			CHAR phone[16];
		}call;
	};
}HD_ALM_TARGET, *LPHD_ALM_TARGET;


typedef struct tagALM_RELATION			//������
{
	UINT size;
	LONG id;
	CHAR sources[256];					//Format: S1&S2&S3 �� S1|S2|S3	
	CHAR targets[256];					//Format: T1,T2,T3
}HD_ALM_RELATION, *LPHD_ALM_RELATION;

typedef struct tagALM_RELATION_REQ		//��ѯ
{
	UINT size;
	UINT mask;
	LONG id;							//Mask 0x01
	CHAR sources[256];					//Mask 0x02
	CHAR targets[256];					//Mask 0x03
}HD_ALM_RELATION_REQ, *LPHD_ALM_RELATION_REQ;


typedef struct tagALM_LOG				//������־
{
	UINT size;						
	LONG id;							//id
	LONG relation;						//����
	CHAR time[20];						//ʱ��	
	BOOL state;							//״̬
	CHAR desc[256];						//����
}HD_ALM_LOG, *LPHD_ALM_LOG;

typedef struct tagALM_LOG_REQ		//��ѯ
{
	UINT size;
	UINT mask;
	CHAR begin[20];						//mask 0x0001
	CHAR end[20];						//mask 0x0002
	LONG idx_begin;						//mask 0x0004
	LONG idx_end;						//mask 0x0008
	LONG relation;						//mask 0x0010
	LONG source;						//mask 0x0020
	LONG target;						//mask 0x0040
	BOOL state;							//mask 0x0080
}HD_ALM_LOG_REQ, *LPHD_ALM_LOG_REQ;




/*
 *	�ƻ�
 */
typedef struct tagTIME_SLICE			//ʱ���
{
	UINT size;
	UINT id;							//ʱ��
	CHAR name[64];						//����(����/��ҹ/����/��Ϣ/���Զ���)
	CHAR begin[8];						//��ʼʱ��
	CHAR end[8];						//����ʱ��
}HD_TIME_SLICE, *LPHD_TIME_SLICE;


typedef enum //������
{
	EM_DT_MONTH,	
	EM_DT_WEEK,		
	EM_DT_REST_DAY, 
	EM_DT_WORK_DAY	
}HDEM_DT_TYPE; 

typedef enum //ʱ������
{
	EM_TM_POINT,	
	EM_TM_SLICE		
}HDEM_TM_TYPE;

typedef struct tagSCH_SOURCE
{
	UINT size;
	LONG id;
	LONG type;
	CHAR desc[128];
	union
	{
		struct //��
		{
			LONG day;
			CHAR time[8];			
		}month;

		struct //��
		{
			LONG day;
			CHAR time[8];			
		}week;

		struct //��Ϣ��
		{
			LONG type;
			union
			{
				CHAR time[8];			
				HD_TIME_SLICE slice;
			};			
		}rest_day;

		struct //������
		{
			LONG type;
			union
			{
				CHAR time[8];			
				HD_TIME_SLICE slice;
			};			
		}work_day;
	};
}HD_SCH_SOURCE, *LPHD_SCH_SOURCE;


typedef enum //�ƻ�����
{
	EM_SCH_ALARM,  
	EM_SCH_RECORD, 
	EM_SCH_ADJUST, 
	EM_SCH_SYSTEM, 
	EM_SCH_BACKUP, 
	EM_SCH_REPORT  
}HDEM_SCH_TYPE; 

typedef struct tagSCH_TARGET
{
	UINT size;
	LONG id;
	LONG type;
	LONG state;
	CHAR desc[128];
	union 
	{
		struct // �����ƻ�
		{
			CHAR host[32];
		}alarm;

		struct // ¼��ƻ�
		{
			CHAR codid[32];
			LONG chnid;
		}record;

		struct // ���ڼƻ�
		{
			CHAR codid[32];
			LONG chnid;
		}adjust;

		struct // ϵͳ�ƻ�
		{
			LONG type;
		}system;

		struct // ���ݼƻ�
		{
			LONG type;
		}backup;

		struct // ����ƻ�
		{
			LONG type;
		}report;
	};
}HD_SCH_TARGET, *LPHD_SCH_TARGET;

typedef struct tagSCH_RELATION			//�ƻ���
{
	UINT size;	
	LONG id;
	CHAR sources[256];					//Format: S1|S2|S3	
	CHAR targets[256];					//Format: T1,T2,T3
}HD_SCH_RELATION, *LPHD_SCH_RELATION;

typedef struct tagSCH_RELATION_REQ		//��ѯ
{
	UINT size;
	UINT mask;
	LONG id;							//Mask 0x01
	CHAR sources[256];					//Mask 0x02
	CHAR targets[256];					//Mask 0x03
}HD_SCH_RELATION_REQ, *LPHD_SCH_RELATION_REQ;



/*
*	¼��
*/
typedef struct tagREC_PARTITION
{
	UINT size;
	LONG id;							//����
	CHAR name[64];						//����
	LONG state;							//״̬
	UINT total;							//����
	UINT space;							//�ռ�
	UINT blk_size;						//������
	UINT blk_space;						//���п�
	CHAR channels[128];					//ͨ��
	CHAR drivers[128];					//����
	CHAR desc[256];						//����
}HD_REC_PARTITION, *LPHD_REC_PARTITION;

typedef struct tagREC_BLOCK
{
	UINT size;
	LONG part_id;						//����
	LONG id;							//���
	CHAR name[64];						//����
	LONG state;							//״̬
	CHAR folder[256];				    //Ŀ¼
	CHAR file[128];						//�ļ�
	UINT total;							//������32M��64M��128M��256M��512M��
	LONG space;							//�ռ�
	CHAR create_dt[20];					//����ʱ��
	CHAR modify_dt[20];					//�޸�ʱ��
	LONG rec_num;						//¼����
	CHAR desc[256];						//����
}HD_REC_BLOCK, *LPHD_REC_BLOCK;

typedef struct tagREC_LOG				//¼����־
{
	UINT size;
	LONG part_id;						//����
	LONG blk_id;						//���
	LONG id;							//��־��
	CHAR codid[32];						//�豸��
	LONG chnid;							//ͨ����
	LONG type;							//����
	LONG type_detail;					//��ϸ����
	LONG state;							//״̬
	CHAR file[256];						//�ļ�
	CHAR url0[128];						//�㲥��ַ0
	CHAR url1[128];						//�㲥��ַ1
	CHAR url2[128];						//�㲥��ַ2
	LONG length;						//����
	LONG begin_pos;						//��ʼλ��
	LONG end_pos;						//����λ��
	CHAR begin_dt[20];					//��ʼʱ��
	CHAR end_dt[20];					//����ʱ��
	LONG prev_id;						//prev¼��
	LONG next_id;						//next¼��
	CHAR desc[256];						//����
}HD_REC_LOG, *LPHD_REC_LOG;

typedef struct tagREC_PARAM				//¼�����
{
	UINT size;						
	CHAR codid[32];						//�豸
	LONG chnid;							//ͨ��
	LONG type;							//����
	BOOL pre_record;					//Ԥ��¼��
	LONG pre_length;					//Ԥ¼ʱ��
	BOOL ext_record;					//�ӳ�¼��		
	LONG ext_length;					//�ӳ�ʱ��
	LONG cycle_mode;					//ѭ����ʽ
	LONG cover_mode;					//���Ƿ�ʽ
	LONG cover_days;					//��������
	LONG storage_mode;					//�ϴ�¼��
}HD_REC_PARAM, *LPHD_REC_PARAM;

typedef struct tagSTREAM
{
	UINT size;							//�ṹ��+���ݵĳ���
	CHAR codid[32];						//�豸id
	LONG chnid;							//ͨ��id
	CHAR stream[1];						//��
}HD_STREAM, *LPHD_STREAM;

typedef struct tagREC_SOURCE			//¼��Դ
{
	UINT size;
	LONG id;
	CHAR codid[32];
	LONG chnid;	
	LONG type;
	union
	{
		struct	// ��ͨ
		{
			LONG type; /*�ֶ�����ʱ*/
		}normal;

		struct // ����
		{
			LONG type; /*�ƶ���⡢����IO*/
		}alarm;
	};
}HD_REC_SOURCE, *LPHD_REC_SOURCE;


/*
 *	�㲥
 */
typedef struct tagRECORD_LOG
{
	UINT size;
	LONG id;
	CHAR type[32];
	CHAR device[64];
	CHAR begin[20];
	CHAR end[20];
	CHAR local_url[256];
	CHAR remote_url[256];
	LONG state;
}HD_RECORD_LOG, *LPHD_RECORD_LOG;

typedef struct tagRECORD_LOG_REQ
{
	UINT size;
	UINT mask;	
	CHAR begin[20];			//Mask 01
	CHAR end[20];			//Mask 02
	LONG id_begin;			//Mask 04
	LONG id_end;			//Mask 08
	CHAR codid[32];			//Mask 10
	LONG chnid;				//Mask 20
	LONG type;				//Mask 40
	LONG state;				//Mask 80
}HD_RECORD_LOG_REQ, *LPHD_RECORD_LOG_REQ;

/*
 *	��־
 */
typedef struct tagSYSTEM_LOG
{
	UINT size;
	LONG id;
	UINT session;
	CHAR user[32];
	CHAR cmd[32];
	CHAR type[32];
	CHAR time[20];
	LONG state;
	CHAR desc[256];
}HD_SYSTEM_LOG, *LPHD_SYSTEM_LOG;

typedef struct tagSYSTEM_LOG_REQ
{
	UINT size;
	UINT mask;
	CHAR begin[20];			//Mask 01
	CHAR end[20];			//Mask 02
	LONG id_begin;			//Mask 04
	LONG id_end;			//Mask 08
	CHAR user[32];			//Mask 10
	UINT cmd;				//Mask 20
	LONG type;				//Mask 40
	LONG state;				//Mask 80
}HD_SYSTEM_LOG_REQ, *LPHD_SYSTEM_LOG_REQ;


/*
 *	Ȩ��
 */
typedef struct tagUSER
{
	UINT size;
	CHAR user[32];
	CHAR password[32];
}HD_USER, *LPHD_USER;

typedef struct tagUSER_INFO
{
	UINT size;
	UINT user_id;				//for get&modify
	CHAR user_name[32];
	CHAR password[32];
	CHAR reg_time[20];			//ע��ʱ�䣬ģ����ϵͳ����
	CHAR act_time[20];			//���ʱ�䣬ģ����ϵͳ����
	UINT len;
	UINT permission[1];
}HD_USER_INFO, *LPHD_USER_INFO;

/*
 *	��ͼ
 */
typedef struct tagMAP_INFO		//��ͼ������Ϣ
{
	UINT size;					//�ṹ���С
	CHAR map_id[40];			//��ͼID GUID
	CHAR region_id[256];		//��ID
	CHAR file_hash[40];			//MD5������ ͬʱҲ���ļ�����
	LONG type;					//0 ��ͼ���ļ� 1 ��ͼ�ȵ�ͼƬ 2 ��ͼ�����ļ�
	LONG length;				//ͼƬ����
	CHAR content[1];			//�ļ�����
}HD_MAP_INFO, *LPHD_MAP_INFO;



/*
 *	��Ϣ/�ص����ؽṹ�����
 */
typedef struct tagMSG_DATA
{
	CHAR*data;					//�������� �����ṹ��
	LONG err_no;				//����� 0��ȷ ����:����
}HD_MSG_PARAM, *LPHD_MSG_PARAM;

/*
 *	��־/Ȩ�޷��
 */
typedef struct tagCMD_DATA
{
	UINT size;
	UINT cmd;
	LONG state;
	UINT len;
	CHAR data[1];				//�����
}HD_CMD_DATA, *LPHD_CMD_DATA;
//  [2/27/2008]
//////////////////////////////////////////////////////////////////////////