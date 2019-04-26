#pragma once
//#include "HPRecordDefine.h"

#define TERM_TYPE_LIVSVR	"live"

//ֱ������ʽ;
typedef enum _STREAM_RTMPLIVE_TYPE
{		
	STREAM_RTMPLIVE_TYPE_ALL			= 1,	//ֱ������Ƶ;
	STREAM_RTMPLIVE_TYPE_VIDEO			= 2,	//ֱ����Ƶ;
	STREAM_RTMPLIVE_TYPE_AUDIO			= 3,	//ֱ����Ƶ;
}STREAM_RTMPLIVE_TYPE;

//ֱ��ͨ����Ϣ
typedef struct _tagLIVEINFO
{
	_tagLIVEINFO()
	{
		roomid			= "";
		roomname		= "";
		devid			= "";
		channelid		= "";
		channelname		= "";
		liveuserid		= "";
		devinfo			= "";
		nodeid			= "";
		mcuid       	= "";
		mcuaddr			= "";
		nataddr			= "";
		localaddr		= "";
		termtype		= "";
		mcuport			= 0;
		localport		= 0;
		videocid		= 0;
		audiocid		= 0;
		livename		= "";
		streamtype  	= 0;
		eLiveType		= STREAM_RTMPLIVE_TYPE_ALL;
		pullurl			= "";
		pushurl			= "";
		startdatetime	= "";
		enddatetime		= "";
	}
	_tagLIVEINFO& operator=(const _tagLIVEINFO& mtn)
	{
		roomid			= mtn.roomid;
		roomname		= mtn.roomname;
		devid			= mtn.devid;
		channelid		= mtn.channelid;
		channelname		= mtn.channelname;
		liveuserid		= mtn.liveuserid;
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
		livename		= mtn.livename;
		streamtype		= mtn.streamtype;
		eLiveType		= mtn.eLiveType;
		pullurl			= mtn.pullurl;
		pushurl			= mtn.pushurl;
		startdatetime	= mtn.startdatetime;
		enddatetime		= mtn.enddatetime;

		return *this;
	}
	std::string				roomid;				//������ID
	std::string				roomname;			//����������
	std::string				devid;				//��ֱ�����豸�ţ�����ԱID��
	std::string				channelid;			//��ֱ����ͨ����
	std::string				channelname;		//��ֱ��ͨ���ı���
	std::string				liveuserid;			//����ֱ������ԱID
	std::string				devinfo;			//�豸��Ϣ
	std::string				nodeid;				
	std::string				mcuid;
	std::string				mcuaddr;
	unsigned short			mcuport;
	std::string				nataddr;
	std::string				localaddr;
	unsigned short			localport;
	std::string				termtype;
	unsigned long			videocid;
	unsigned long			audiocid;
	std::string				livename;			//ֱ�����ƣ��ɲ����ã�
	unsigned short			streamtype;			//ֱ��������0�������� 1�������� 2��������
	STREAM_RTMPLIVE_TYPE	eLiveType;			//1��ֱ������Ƶ 2: ֱ����Ƶ 3��ֱ����Ƶ
	std::string				pullurl;			//ֱ��������ַ����ʼֱ���ɹ������ɣ�
	std::string				pushurl;			//ֱ��������ַ��Ŀǰ���Կͻ��˿��ţ�
	std::string				startdatetime;		//ֱ����ʼʱ��
	std::string				enddatetime;		//ֱ������ʱ��

}LIVEINFO,*LPLIVEINFO;

typedef struct tag_LIVREMOTE
{
	std::string			serverid;
	std::string			serverip;
	std::string			servernatip;
	std::string			storageaddr;
	unsigned int		policystorage;
	unsigned int		policytime;
	unsigned int		policysize;
	unsigned short		status;
	unsigned int		userport;
	unsigned int		licport;
	//unsigned long		freespace;
	//unsigned long		totalspace;

	tag_LIVREMOTE()
	{
		serverid = "";
		serverip = "";
		servernatip = "";
		storageaddr = "";
		policystorage = 0;
		policytime = 60;
		policysize = 500;
		status = 0;
		userport = 0;
		licport = 0;
		//freespace = 0;
		//totalspace = 0;
	}
}LIVREMOTE, *PLIVREMOTE;
