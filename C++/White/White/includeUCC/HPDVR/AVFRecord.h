#ifndef __AVFRECORD_H__
#define __AVFRECORD_H__

// include stl
#include <map>
#include <list>
#include <queue>   
#include <string>
#include <vector>
#pragma warning (disable:4786)
using namespace std;

#define	SEEK_EVENT_BEGIN				1
#define	SEEK_EVENT_END					2

//֡����
#define	AVC_FRAME_TYPE_TIME				0	//ʱ��֡
#define	AVC_FRAME_TYPE_START			1	//��ʼ֡
#define	AVC_FRAME_TYPE_STOP				2	//����֡
#define	AVC_FRAME_TYPE_VIDEO			3	//��Ƶ��
#define	AVC_FRAME_TYPE_AUDIO			4	//��Ƶ��

//¼��������
#define AVC_RECORD_NORMAL				0	//����¼��
#define AVC_RECORD_ALERT				1	//����¼��

//¼������
#define AVC_RECORD_SCHEDULE				0	//��ʱ¼��		
#define AVC_RECORD_MANUAL				1	//�ֶ�¼��
#define AVC_RECORD_MOVEDETECT			2	//�ƶ����¼��
#define AVC_RECORD_ALARM				3	//IO����¼��
#define AVC_RECORD_MOVEORALARM			4	//����|IO����¼��
#define AVC_RECORD_MOVEANDALARM			5	//����&IO����¼��

#define AVC_REC_DSC_LEN					256 //�����ı�����
#define AVC_REC_PWD_LEN					128	//MD5���볤��
#define AVC_REC_NME_LEN					128	//�ļ������� 
#define AVC_REC_CHK_LEN					32	//У���볤��

//¼��ʱ��
typedef struct tag_AVC_FILE_TIME
{	
	DWORD	dwYear;		// ��
	BYTE	byMonth;	// ��
	BYTE	byDay;		// ��
	BYTE	byHour;		// ʱ
	BYTE	byMinute;	// ��
	BYTE	bySecond;	// ��
}AVC_FILE_TIME,*PAVC_FILE_TIME ;


//////////////////////////////////////////////////////////////////////////
//AVC�ļ��ṹ
//�ļ�ͷ�ṹ�汾��
#define AVC_RECORD_FILE_NORMAL			0	//AVC_FILE_HEADER_OLD
#define AVC_RECORD_FILE_SURVEILLANCE	1	//AVC_FILE_HEADER_SURV
#define AVC_RECORD_FILE_EXTRA			2	//AVC_FILE_HEADEREx
#define AVC_RECORD_FILE_EXTRA3			3	//AVC_FILE_HEADEREx3
#define AVC_RECORD_FILE_VERSION_4		4	//AVC_FILE_HEADEREx3_�����
//��һ���汾
typedef struct tag_AVC_FILE_HEADER_OLD
{
	unsigned long	version;			//�汾��
	unsigned long	flag;				//��ʶ�� AVC_RECORD_FILE_NORMAL
	char			password[16];		//����
}AVC_FILE_HEADER_OLD, *PAVC_FILE_HEADER_OLD;

//�ڶ����汾
typedef struct tag_AVC_FILE_HEADER_SURV
{
	//ԭ�汾��
	unsigned long	version;			//�汾��
	unsigned long	flag;				//��ʶ�� AVC_RECORD_FILE_SURVEILLANCE
	char			password[16];		//����
	DWORD			dwsize;				//���¸�������ֽ����ܺ�
	BYTE			byRecorderType;		//¼������,ָ���ƻ�¼�񡢱���¼���
	AVC_FILE_TIME	starttime;			//¼��ʼʱ��
	AVC_FILE_TIME	endtime;			//¼�����ʱ��
	BYTE			byRecorderDev[256];	//¼���豸����(ϵͳ��Ψһ���ã��������еı��������к�)
}AVC_FILE_HEADER_SURV, *PAVC_FILE_HEADER_SURV;

//�������汾
typedef struct tag_AVC_FILE_HEADEREx
{
	DWORD			dwsize;					//���ṹ��ĳ���,���汾�޶�������
	int				flag;					// AVC_RECORD_FILE_EXTRA
	char			password[128];			//����,MD5
	BYTE			byRecorderType;			//AVC_RECORD_ NORMAL\ AVC_RECORD_ ALERT
	AVC_FILE_TIME	starttime;				//¼��ʼʱ��
	AVC_FILE_TIME	endtime;				//¼�����ʱ��
	BYTE			byRecorderDev[128];		//¼���豸����,ϵͳ��Ψһ
	BYTE			bytChannelNo;			//ͨ����
	BYTE			bytDesc[256];			//¼���ļ���������Ϣ
	BYTE			bytPriorFile[128];		//ǰһ���ļ�����,NULL=��
	BYTE			bytNextFile [128];		//��һ���ļ�����,NULL=��
	BYTE			bytRecordState;			//¼��״̬,0=����¼��,1=¼�����
	BYTE			bytUploadState;			//�ϴ�״̬,0=δ�ϴ�,1=���ϴ�,2=�ϴ����,3=У�����
	DWORD			bytUploadBytes;			//�Ѿ��ϴ��Ľ�����
}AVC_FILE_HEADEREx, *PAVC_FILE_HEADEREx;

//���ĸ��汾
typedef struct tag_AVC_FILE_HEADEREx3
{
	DWORD 			dwsize;						    //���ṹ��ĳ���,���汾�޶�������
	DWORD 			flag;						    //¼���汾�� AVC_RECORD_FILE_EXTRA3
	BYTE	 		password[AVC_REC_PWD_LEN];	    //����,MD5
	BYTE	 		bytRecordType;					//AVC_RECORD_ NORMAL\ AVC_RECORD_ ALERT
	AVC_FILE_TIME	starttime;						//¼��ʼʱ��
	AVC_FILE_TIME	endtime;						//¼�����ʱ��
	BYTE	 		bytRecordDev[AVC_REC_NME_LEN]; 	//¼���豸����,ϵͳ��Ψһ
	BYTE	 		bytChannelNo;					//ͨ����
	BYTE	 		bytDesc[AVC_REC_DSC_LEN];	  	//¼���ļ���������Ϣ
	BYTE	 		bytPriorFile[AVC_REC_NME_LEN]; 	//ǰһ���ļ�����,NULL=��
	BYTE	 		bytNextFile [AVC_REC_NME_LEN]; 	//��һ���ļ�����,NULL=��
	BYTE	 		bytRecordState;				  	//0=����¼��,1=¼�����
	BYTE	 		bytUploadState;				  	//0=δ�ϴ�,1=���ϴ�,2=�ϴ���,3=У����
	DWORD 			bytUploadBytes;			       	//�Ѿ��ϴ��Ľ�����
	BYTE  			checksum[AVC_REC_CHK_LEN];      //�ļ��۸�У����,MD5
	DWORD 			timestamp;					    //��ʼʱ����
	BYTE			bytVersion[AVC_REC_NME_LEN];	//��Ʒ�汾�� ��:(AVCON4.x.xx AVCON5.x.xx AVCON6.x.xx NDVR2.x.xx)
}AVC_FILE_HEADEREx3, *PAVC_FILE_HEADEREx3;

// �ļ�ͷ�ṹ
typedef struct tag_AVC_FILE_HEADER //(���� ���İ�)
{
	DWORD 			dwsize;						    //���ṹ��ĳ���,���汾�޶�������
	DWORD 			flag;						    //¼���汾�� AVC_RECORD_FILE_EXTRA3
	BYTE	 		password[AVC_REC_PWD_LEN];	    //����,MD5
	BYTE	 		bytRecordType;					//AVC_RECORD_ NORMAL\ AVC_RECORD_ ALERT
	AVC_FILE_TIME	starttime;						//¼��ʼʱ��
	AVC_FILE_TIME	endtime;						//¼�����ʱ��
	BYTE	 		bytRecordDev[AVC_REC_NME_LEN]; 	//¼���豸����,ϵͳ��Ψһ
	BYTE	 		bytChannelNo;					//ͨ����
	BYTE	 		bytDesc[AVC_REC_DSC_LEN];	  	//¼���ļ���������Ϣ
	BYTE	 		bytPriorFile[AVC_REC_NME_LEN]; 	//ǰһ���ļ�����,NULL=��
	BYTE	 		bytNextFile [AVC_REC_NME_LEN]; 	//��һ���ļ�����,NULL=��
	BYTE	 		bytRecordState;				  	//0=����¼��,1=¼�����
	BYTE	 		bytUploadState;				  	//0=δ�ϴ�,1=���ϴ�,2=�ϴ���,3=У����
	DWORD 			bytUploadBytes;			       	//�Ѿ��ϴ��Ľ�����
	BYTE  			checksum[AVC_REC_CHK_LEN];      //�ļ��۸�У����,MD5
	DWORD 			timestamp;					    //��ʼʱ����
	BYTE			bytVersion[AVC_REC_NME_LEN];	//��Ʒ�汾�� ��:(AVCON4.x.xx AVCON5.x.xx AVCON6.x.xx NDVR2.x.xx)
}AVC_FILE_HEADER, *PAVC_FILE_HEADER;
//////////////////////////////////////////////////////////////////////////

// ����֡����
typedef struct tag_AVC_DATA_FRAME
{	
	char				frame_type	;		//֡����
	char				key_frame	;		//�ؼ�֡
	unsigned short		sequence	;		//�Ự����
	unsigned long		session_id	;		//�Ự��ʶ
	int					channel		;		//ͨ��
	unsigned long		timestamp	;		//ʱ���
	long				data_length	;		//���ݳ���
}AVC_DATA_FRAME,*PAVC_DATA_FRAME;

// ʱ��֡
typedef struct tag_AVC_TIME_FRAME
{
	unsigned long		timestamp	;		//ʱ���
	unsigned long		position	;		//ָ������֡λ��
}AVC_TIME_FRAME,*PAVC_TIME_FRAME;

// ��������ͷ����
typedef struct tag_AVC_INDEX_HEADER
{
	char				index[4]	;		//������־:'INDX'
	char				frame_type	;		//֡����
	unsigned short		sequence	;		//�Ự����
	unsigned long		session_id	;		//�Ự��ʶ
	int					channel		;		//ͨ��
	unsigned long		start_time	;		//�����Ŀ�ʼʱ��
	unsigned long		stop_time	;		//�����Ľ���ʱ��
	long				data_length	;		//�����������ݳ���
}AVC_INDEX_HEADER, *PAVC_INDEX_HEADER;

// ��������
typedef struct tag_AVC_INDEX_DATA
{
	unsigned long		pos			;		//λ��
	unsigned long		timestamp	;		//ʱ���
	char				key_frame	;		//�Ƿ�ؼ�֡
}AVC_INDEX_DATA, *PAVC_INDEX_DATA;

typedef struct tag_STREAM_INFO
{
	char				frame_type;
	unsigned long		session_id;
	int					channel;
	unsigned short		sequence;	
}STREAM_INFO, *PSTREAM_INFO;
typedef list <STREAM_INFO> LIST_STREAM_INFO;

typedef struct tag_DATA_FRAME
{
	char*		pData;
	int			nLen;
	unsigned long ulTimestamp;
	unsigned long ulSessionId;
}DATA_FRAME, *PDATA_FRAME;
typedef list  <DATA_FRAME>	LIST_DATA_FRAME;
typedef queue <DATA_FRAME>	QUEUE_DATA_FRAME;
typedef vector <DATA_FRAME> VECTOR_DATA_FRAME;

#endif//__AVFRECORD_H__