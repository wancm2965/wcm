/*
 *	HCstruct.h
 */

#define DETECTPICLEFT 35
#define DETECTPICTOP 15
#define SCREEN_EDGE 30

#define MINSPACE		(200*1024*1024)

#define	CYCLE_TIMER			1		//ѭ����ʾʱ��
#define	AUTORECORD_TIMER	2		//�Զ�¼��ʱ��
#define	CHECKSERVER_TIMER	3		//Ӳ��¼���Уʱʱ��
#define	FREESPACE_TIMER		4		//���Ӳ��ʱ��
#define	FILEINTERVAL_TIMER	5		//�����ļ�ʱ����ʱ��
#define PING_TIMER			6		// ����ʾʱ��
#define UPGRADE_TIMER		7		//����ʱ��
#define PLAY_TIMER			8		//���ػط�ʱ��ʾ����״̬ʱ��
#define RPSTATE_TIMER		9		//Զ�̻ط��ļ�����״̬ʱ��
#define DOWNSTATE_TIMER		10		//�����ļ�ʱ��
#define FORMAT_TIMER		11		//Զ�̸�ʽ��Ӳ��
#define EVENTLOG_TIMER		12		//�¼���־
#define PLAYBYTIME_TIMER	13		// ��ʱ��ط�
#define DOWNBYTIME_TIMER	14		//��ʱ������
#define SERIAL_TIMER		15		//����͸��ͨ������	

#define DEFAULTBRIGHTNESS 6
#define DEFAULTCONTRAST 6
#define DEFAULTSATURATION  6
#define DEFAULTHUE 6
#define DEFAULTVOLUME 50

#define MAXIPNUMBER 50			//������ķ�������Ŀ
#define MAXCHANNUM 16			//ÿ�����������ļ��ͨ����
#define MAXVIEWNUM	16			//���Ĵ�����
#define MAXNUMBER   512			//��Ԥ���������
#define	MAXALARMIN	16			//�ͻ������ɽ��ձ�����
//#define MAXDISKNUM 8			//���Ӳ����	

typedef struct{
	CString	m_csServerName;
	CString m_csServerIP;
	DWORD	m_dwServerIPvalue;
	CString m_csServerMultiIP;
	DWORD	m_dwServerMultiIPvalue;
	CString m_csServerAdminName;
	CString m_csServerAdminPasswd;
	LONG	m_lServerID;
	LONG	m_lStartChannel;
	int		m_iServerChannelNumber;	//ͨ����
	WORD	m_wServerPort;			//�������˿ں�
	WORD	m_wServerType;			//1 DVR, 2 ATMDVR, 3 DVS .......;
	CString	m_csSerialNumber;  //���к�
	DWORD	m_dwAlarmInNum;		//�����������
	DWORD	m_dwAlarmOutNum;	//�����������
	DWORD	m_dwDiskNum;			//Ӳ�̸���
	DWORD	m_dwDevSoftVersion;		//�豸����汾
}SERVER_INFO, *PSERVER_INFO;

typedef struct{
	CString	m_csIP;					//����������IP
	CString	m_csUsername;
	CString	m_csPassword;
	LONG	m_lServerID;
	int		m_iChannel;				//����ͨ��
	int		m_iPortIndex;			//����������
	int		m_iAlarmType;
	int		m_iAlarmLevel;
	WORD	m_wServerPort;
	LONG	m_lNetIndex;			//
}GENERALALARM_INFO,*PGENERALALARM_INFO;			//�ͻ��˱������ýṹ

typedef struct {
	CString	m_csChanName;			//ͨ������
	CString	m_csIP;					//��Ƶ������IP��ַ
	DWORD	m_dwIPvalue;
	int		m_iChannel;				//���ͨ����
	DWORD	m_dwProtocol;			//����Э��
	DWORD	m_dwAddress;			//��������ַ
	CString m_csUser;				//�û���
	CString m_csPassword;			//����
	LONG	m_lServerID;
	CString m_csMultiIP;			//
	DWORD	m_dwMultiIPvalue;
	DWORD	m_dwAlarmlevel;	
	WORD	m_wServerport;			//�������˿ں�
	WORD	m_wJMQtype;				//����������
	BOOL	m_bChoosed;				//��ͨ����ѡ�����粥��
	int		m_iServerNum;			//��Ӧ���������
	int		m_iChannelNum;			//��Ӧ���ͨ�����
	int		m_iSequence;			//���˳��
//	BOOL	m_bWiper;		//��ˢ����
//	BOOL	m_bLight;		//�ƹ⿪��
}IP_INFO,*PIP_INFO;

typedef struct{
	UINT	m_iBrightness;				//1-10
	UINT	m_iContrast;				//1-10
	UINT	m_iSaturation;				//1-10
	UINT	m_iHue;						//1-10
}VIDEO_INFO, *PVIDEO_INFO;

typedef struct{
    WORD	m_iStartHour;
	WORD	m_iStartMinute;
	WORD	m_iStopHour;
	WORD	m_iStopMinute;
	WORD	m_iStartTime;
	WORD	m_iStopTime;
}CLIENTRECORDTIME,*PCLIENTRECORDTIME;

typedef struct
{
	BOOL		m_bCyclePlay;				//�Ƿ�ѭ������
	int			m_iCycleTime;				//ѭ������ʱ��
	BOOL		m_bUseCard;					//�Ƿ���ý��뿨
	BOOL		m_bNTSC;					//FALSE��Pal��  TRUE��NTSC��
	
	BOOL		m_bAutoRecord;				//�Ƿ��Զ�¼��;
	BOOL		m_bCycleRecord;				//�Ƿ�ѭ��¼��
	int			m_iStartRecordDriver;		//�ͻ���¼����ʼ�̷�
	int			m_iEndRecordDriver;			//�ͻ���¼����ֹ�̷�
	int			m_iRecordFileInterval;		//¼���ļ����ʱ��
	CLIENTRECORDTIME	m_ClientRecordTime[7][4];
	CString		m_csClientRecordPath;		//�ͻ��˵�ǰ¼��·��
	BOOL		m_bAllDiskFull;				//�ͻ���Ӳ�����ı�־
	
	CString		m_csDownLoadPath;			//Զ���ļ����ر���·��
	CString		m_csPictureSavePath;		//ץͼ����·��
	CString		m_csLogSavePath;			//��־����·��
	
	BOOL		m_bPlaying;					//���ڲ��ŵı�־
	BOOL		m_bCycling;					//����ѭ�����ŵı�־
	BOOL		m_bPaused;					//ѭ����ͣ
	BOOL		m_bNextPage;				//�ֶ�ѭ����һҳ
	BOOL		m_bFrontPage;				//�ֶ�ѭ����һҳ
	BOOL		m_bEnlarged;				//ͼ�񴰿ڷŴ�
	BOOL		m_bFullScreen;				//ͼ��ȫ���Ŵ�
	BOOL		m_bMultiScreen;				//�ര��ȫ��
	BOOL		m_bPlaySound[MAXVIEWNUM];	//������������
	
	int			m_iAlarmHandle[MAXIPNUMBER];	//�������������
	
	VIDEO_INFO	m_VideoInfo[MAXVIEWNUM];		//��Ƶ����
	UINT		m_Volume;						//����
	
	BOOL		m_bAlarmPlaying;
	
	BOOL		m_bCheckServerTime;			//Ӳ��¼���Уʱ
	long		m_iCheckServerTime;			//Ӳ��¼���Уʱʱ����
}CLIENTPARAM, *PCLIENTPARAM;