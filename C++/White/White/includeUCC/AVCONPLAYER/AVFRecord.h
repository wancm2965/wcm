#ifndef _AVFRECORD_H
#define _AVFRECORD_H

#define		SEEK_EVENT_BEGIN			1
#define		SEEK_EVENT_END				2

#define AVC_FRAME_TYPE_VIDEO	0
#define AVC_FRAME_TYPE_AUDIO	1
#define AVC_FRAME_TYPE_TIME		2
#define	AVC_FRAME_TYPE_START	3			//��ʼ֡
#define	AVC_FRAME_TYPE_STOP		4			//����֡
#define	AVC_FRAME_TYPE_USER_LIST		5	//�û��б���
#define AVC_FRAME_TYPE_AVMSVIDEO 125
#define AVC_FRAME_TYPE_AVMSAUDIO 126
#define AVC_FRAME_TYPE_VIDEO2    127			//��ѹ��֡

#define AVFIM_FRAME_TYPE_AVDLGLIST	6	//AV������ϸ��Ϣ�б�
#define AVFIM_FRAME_TYPE_ADDAVDLG	7	//���ӵ�AV������ϸ��Ϣ
#define AVFIM_FRAME_TYPE_DELAVDLG	8	//���ٵ�AV������Ϣ


//�м���ɰ� ���š�Զ��¼�Ƶ����ء��޷����ŵ�¼�񣨼���֧����Ҫ��
//#define AVC_FILE_HEADER_OLD		AVC_FILE_HEADER 

#define AVC_RECORD_FILE_NORMAL			0			// AVC_FILE_HEADER_OLD
#define AVC_RECORD_FILE_FRIST			1			// AVC_FILE_HEADER
#define AVC_RECORD_FILE_SECOND			2			// ���ɰ汾��
#define AVC_RECORD_FILE_THIRD			3

#define AVC_REC_DSC_LEN					256 //�����ı�����
#define AVC_REC_PWD_LEN					128	//MD5���볤��
#define AVC_REC_NME_LEN					128	//�ļ������� 
#define AVC_REC_CHK_LEN					32	//У���볤��

#define AVC_RECORD_NAMELEN					128
#define AVC_RECORD_DESCRIPTION				256			//¼���ļ���������Ϣ����
#define AVC_RECORD_PASSWORDLEN				128			//���볤��
#define AVC_RECORD_NORMAL					0			//����¼��
#define AVC_RECORD_ALERT					1			//����¼��


//¼������ 0��1��2��3��4 ��Ӧ�� _T("�ƻ�¼��"),_T("�ֶ�¼��"),_T("�ƶ����¼��"),_T("��������¼��"),_T("��Ӧ����¼��")
#define RT_SCHEDULE			0
#define RT_MANUAL			1
#define RT_MOVEDETECT		2
#define RT_ALARM			3
#define RT_MOVEORALARM		4
#define RT_MOVEANDALARM		5
#define RT_COMMAND			6

// ¼��ʱ�䣬����¼���ѯʱ��������Ĺؼ���
typedef struct tag_AVC_FILE_TIME
{	
	DWORD dwYear;
	BYTE byMonth,byDay,byHour,byMinute,bySecond;
}AVC_FILE_TIME,*PAVC_FILE_TIME ;

//*********************************  �ļ�ͷ����  *********************************
// version AVC_RECORD_FILE_NORMAL
// ����¼��avcon4 & avcon6
typedef struct tag_AVC_FILE_HEADER_OLD
{
	unsigned long		version		;		//�汾��
	unsigned long		flag		;		//��ʶ��
	char				password[16];		//����
}AVC_FILE_HEADER_OLD, *PAVC_FILE_HEADER_OLD;

// version AVC_RECORD_FILE_FRIST
// AVC�ļ��ṹ(��չ)  flag=AVC_RECORD_FILE_SURVEILLANCEʱ�������½ṹ
typedef struct tag_AVC_FILE_HEADER_SURV
{
	//AVC_FILE_HEADER_OLD
	unsigned long		version		;		//�汾��
	unsigned long		flag		;		//��ʶ��
	char				password[16];		//����

	// �����ֶ�
	DWORD			dwsize;					//���¸�������ֽ����ܺ�
	BYTE			byRecorderType;			//¼������,ָ���ƻ�¼�񡢱���¼���
	AVC_FILE_TIME	starttime;				//¼��ʼʱ��
	AVC_FILE_TIME	endtime;				//¼�����ʱ��
	BYTE			byRecorderDev[256];		//¼���豸����(ϵͳ��Ψһ���ã��������еı��������к�)
}AVC_FILE_HEADER_SURV, *PAVC_FILE_HEADER_SURV;

// version AVC_RECORD_FILE_SECOND
// AVC�ļ���չ�ṹ��AVCONϵͳ¼���ļ��ṹ���塷
typedef struct tag_AVC_FILE_HEADER
{
	DWORD   dwsize;								//���ṹ��ĳ���,���汾�޶�������
	int		flag;								// =AVC_RECORD_FILE_SECOND
	char	password[96];
	char	checksum[32];
	//char	password[AVC_RECORD_PASSWORDLEN];	//����,MD5
	BYTE	byRecorderType;						//AVC_RECORD_ NORMAL\ AVC_RECORD_ ALERT
	AVC_FILE_TIME starttime;					//¼��ʼʱ��
	AVC_FILE_TIME endtime;						//¼�����ʱ��
	BYTE	byRecorderDev[AVC_RECORD_NAMELEN];	//¼���豸����,ϵͳ��Ψһ
	BYTE	bytChannelNo;						//ͨ����
	BYTE	bytDesc[AVC_RECORD_DESCRIPTION];	//¼���ļ���������Ϣ
	BYTE	bytPriorFile[AVC_RECORD_NAMELEN];	//ǰһ���ļ�����,NULL=��
	BYTE	bytNextFile [AVC_RECORD_NAMELEN];	//��һ���ļ�����,NULL=��
	BYTE	bytRecordState;						//¼��״̬,0=����¼��,1=¼�����
	BYTE	bytUploadState;						//�ϴ�״̬,0=δ�ϴ�,1=���ϴ�,2=�ϴ����,3=У�����
	DWORD	bytUploadBytes;						//�Ѿ��ϴ��Ľ�����
}AVC_FILE_HEADER, *PAVC_FILE_HEADER;

// version AVC_RECORD_FILE_THIRD
// ����timestamp�ֶΣ�checksum�������
typedef struct tag_AVC_FILE_HEADER_THIRD
{
	DWORD 			dwsize;						    //���ṹ��ĳ���,���汾�޶�������
	DWORD 			flag;						    //=AVC_RECORD_FILE_THIRD
	BYTE	 		password[AVC_REC_PWD_LEN];	    //����
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
	BYTE  			checksum[AVC_REC_CHK_LEN];      //�ļ�MD5У����
	DWORD 			timestamp;					    //��ʼʱ����

	// 20071025
	BYTE			bytVersion[AVC_REC_NME_LEN];	//��Ʒ�汾��
}AVC_FILE_HEADER_THIRD, *PAVC_FILE_HEADER_THIRD;
//*********************************  End  *********************************

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

// AVC�ļ����ӿ�
//int Info(void *buffer, unsigned int dwFlag) dwFlagֵ
enum {
	AVC_FILE_INFO_RAW=0,		//�ļ��ṹ
	AVC_FILE_INFO_SUBRAW,		//����ĳ�ļ���ͷ�ṹ,buffer�����ļ���char*
	AVC_FILE_INFO_FILENAME,		//ȡ�ļ���,����ļ���֮����'\0'�ָ�,ʹ��ǰ��ȡ������(buffer)�����С
	AVC_FILE_INFO_VERSION,
	AVC_FILE_INFO_PASSWORD, 
	AVC_FILE_INFO_CHECKSUM,
	AVC_FILE_INFO_RECORDTYPE, 
	AVC_FILE_INFO_START, 
	AVC_FILE_INFO_STARTEX,
	AVC_FILE_INFO_STATRSTAMP,
	AVC_FILE_INFO_STATRSTAMPEX,
	AVC_FILE_INFO_STOP, 
	AVC_FILE_INFO_STOPEX,
	AVC_FILE_INFO_TIMESTAMP, 
	AVC_FILE_INFO_TIMESTAMPEX, 
	AVC_FILE_INFO_DURATIONEX,
	AVC_FILE_INFO_CHANNELNO,
	AVC_FILE_INFO_RECORDNAME,
	AVC_FILE_INFO_DESC,
	AVC_FILE_INFO_MAX
};
enum {
	AVC_FILE_ERROR_SUCESS=0,//�ɹ�
	AVC_FILE_ERROR_EXIST,	//�Ѿ�����
	AVC_FILE_ERROR_ACCESS,	//�ļ���ȡʧ��
	AVC_FILE_ERROR_RANGE,	//Seek������Χ
	AVC_FILE_ERROR_CHANNEL,	//ͨ����ƥ��
	AVC_FILE_ERROR_DATALACK,	//����δ���������
	AVC_FILE_RRROR_CONNECTFAIL,	//����ʧ��
	AVC_FILE_ERROR_UNKNOWN=0x80000000//δ֪����
};

class IAVCReadFactory
{
public:
	virtual	bool Connect(const char* strAVCFile) = 0;
	virtual void ReleaseConnections() = 0;

	virtual bool MoveNext() = 0;
	virtual int ReadFrame(PAVC_DATA_FRAME padf) = 0;
	virtual int ReadData(void *pData, int len) = 0;

	//|__________________|+++++++++++++++++|__________________  /* +ָ��ǰ�Ŀ���λ�� |�ؼ�֡λ��*/
	//SeekPrevKeyFrame   SeekKeyFrame     SeekNextKeyFrame	    /*���½ӿڽ�Seek����λ��*/
	virtual int SeekKeyFrame()=0;		//���عؼ�֡�ڵ�֡��
	virtual bool SeekNextKeyFrame()=0;
	virtual bool SeekPrevKeyFrame()=0;

	// �����������֡��λ��(ʱ��)
	virtual unsigned long Seek(unsigned long ulSeekTime)=0;

	virtual bool Eof() = 0;		//�����ļ�β
	virtual bool Bof() = 0;		//�����ļ���ͷ

	// ȡ�ļ���Ϣ��bufferΪNULLʱֻ����dwFlag�ֶ������ڴ��С������0ʱ��ȡ�ֶ�ʧ�ܣ������ļ��汾���ԣ�
	virtual int Info(void *buffer, unsigned int dwFlag)=0;
	virtual int LastErrCode()=0;

	virtual unsigned long GetDuration() = 0;
	virtual unsigned long GetPosition() = 0;
	virtual void DelayVideo(long ulMS) = 0;
};
#ifdef __cplusplus
extern "C" {
#endif
bool CreateAVCReadFactory(IAVCReadFactory** ppARF, bool bAutoMerge=true);
#ifdef __cplusplus
}
#endif
#endif
