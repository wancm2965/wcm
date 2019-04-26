/************************************************************************/
/*¼����                                                            */
/************************************************************************/

#ifndef __RECORD_DEFINE_H__
#define __RECORD_DEFINE_H__

#ifdef WIN32
#define RECORD_EXPORT
#ifdef RECORD_EXPORT
#define RECORD_API _declspec(dllexport)
#elif RECORD_DLL
#define RECORD_API _declspec(dllimport)
#else
#define RECORD_API
#endif

#else
#define RECORD_API
#define TRACE  printf
#endif

#include <map>
#include <string>
using namespace std;

typedef enum _RECERR_CODE
{
	REC_START_SUCCESS,			//¼��ʼ�ɹ�
	REC_START_RECMAXCNT,		//�ﵽ��Ȩ����
	REC_STREAM_FAILED,			//����ʧ��
	REC_START_EXIST,			//�Ѿ���¼
	REC_START_DISKFULL,			//���̿ռ䲻��
	REC_START_NODISE,			//û�д���
	REC_CREATEFILE_FAILED,		//�����ļ�ʧ��
	REC_START_OTHER,			//��������
}RECERR_CODE;

//�洢ģʽ
typedef enum _RECSAVE_TYPE
{
	REC_SAVE_LOCAL,				//���ش洢
	REC_SAVE_SAN,				//SAN�洢��������
	REC_SAVE_DAS,				//DASֱ���洢
}RECSAVE_TYPE;

//¼��ģʽ
typedef enum _RECMODE_TYPE
{
	REC_MODE_NORMAL,			//����¼��,���̲���ֹͣ
	REC_MODE_CYCLE,				//����¼��,���̲�����ɾ��			
}RECMODE_TYPE;

//¼��������ʽ
typedef enum _RECSTART_TYPE
{
	RECSTART_TYPE_NORMAL,	//�����ֶ�¼��
	RECSTART_TYPE_ALARM,	//����¼��
	RECSTART_TYPE_PLAN,		//�ƻ�¼��
	RECSTART_TYPE_CONF,		//����¼��
}RECSTART_TYPE;

typedef enum _RECDEL_TYPE
{
	RECDEL_TYPE_SIGNED,	//���ݿ��־����Ҫɾ����
	RECDEL_TYPE_EARLY,	//��������Ҫɾ������һ���
}RECDEL_TYPE;

typedef enum _RECSTOP_TYPE
{
	RECSTOP_TYPE_NORMAL,	//�����ֶ�ֹͣ
	RECSTOP_TYPE_CUT,		//�ָ�ֹͣ
	RECSTOP_TYPE_DISKFULL,	//������ֹͣ
	RECSTOP_TYPE_OTHER,		//������ʽֹͣ
}RECSTOP_TYPE;

typedef enum _RECOVER_RESULT
{
	RECOVER_RESULT_NOFILE,		//�ļ�������
	RECOVER_RESULT_NOTACTIVE,	//�ļ��Ƿ�
	RECOVER_RESULT_COMPLITE,	//���
	RECOVER_RESULT_NORMALAVC,	//���������޸�
}RECOVER_RESULT;
//ͨ����Ϣ
typedef struct _tagRECINFO
{
	_tagRECINFO()
	{
		roomid		= "";
		roomname	= "";
		devid		= "";
		channelid	= "";
		channelname	= "";
		recuserid	= "";
		devinfo		= "";
		nodeid		= "";
		mcuid       = "";
		mcuaddr		= "";
		nataddr		= "";
		localaddr	= "";
		termtype	= "";
		mcuport		= 0;
		localport	= 0;
		videocid	= 0;
		audiocid	= 0;
		recfilename	= "";
		recfilepath = "";
		recStatus	= 0;
		starttime	= "";
		endtime		= "";
		streamtype  = 0;
		stopType    = RECSTOP_TYPE_NORMAL;
		ulFileSize	= 0;
	}
	_tagRECINFO& operator=(const _tagRECINFO& mtn)
	{
		roomid			= mtn.roomid;
		roomname		= mtn.roomname;
		devid			= mtn.devid;
		channelid		= mtn.channelid;
		channelname		= mtn.channelname;
		recuserid		= mtn.recuserid;
		devinfo			= mtn.devinfo;
		nodeid			= mtn.nodeid;
		mcuid			= mtn.mcuid;
		mcuaddr			= mtn.mcuaddr;
		mcuport			= mtn.mcuport;
		nataddr			= mtn.nataddr;
		localaddr		= mtn.localaddr;
		localport		= mtn.localport;
		termtype		= mtn.termtype;
		videocid		= mtn.videocid;
		audiocid		= mtn.audiocid;
		recfilename		= mtn.recfilename;
		recfilepath		= mtn.recfilepath;
		recStatus		= mtn.recStatus;
		starttime		= mtn.starttime;
		endtime			= mtn.endtime;
		streamtype		= mtn.streamtype;
		stopType		= mtn.stopType;
		ulFileSize		= mtn.ulFileSize;
		return *this;
	}
	std::string		roomid;				//������ID
	std::string		roomname;			//����������
	std::string		devid;				//��¼����豸�ţ�����ԱID��
	std::string		channelid;			//��¼���ͨ����
	std::string		channelname;		//��¼��ͨ���ı���
	std::string		recuserid;			//����¼�����ԱID
	std::string		devinfo;			//�豸��Ϣ
	std::string		nodeid;				
	std::string		mcuid;
	std::string		mcuaddr;
	unsigned short	mcuport;
	std::string		nataddr;
	std::string		localaddr;
	unsigned short localport;
	std::string		termtype;
	unsigned long	videocid;
	unsigned long	audiocid;
	std::string		recfilename;	//��ʼ¼�������
	std::string		recfilepath;	//��ʼ¼�������
	unsigned short	recStatus;		//¼��״̬
	std::string		starttime;		//��ʼ¼��ʱ��
	std::string     endtime;		//����ʱ��
	int				streamtype;
	RECSTOP_TYPE	stopType;		//ֹͣ����
	unsigned long   ulFileSize;

}REC_INFO,*LPREC_INFO;


#endif


