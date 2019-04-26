#pragma once
#include "KIMSDefine.h"
#include "KMMSDefine.h"

//=======================================================================================
// ���Ͷ���
//=======================================================================================

// ���ӷ�����״̬
enum {
	SERVER_CONNECTING		= 1,		// ��������
	SERVER_RECONNECTED,					// ��������
	SERVER_BUSY,						// ������æ
	SERVER_FAILED,						// ����ʧ��
	SERVER_DISCONNECTED,				// ���ӶϿ�
	SERVER_CONNECTED,					// ������
};

//ʹ�û���ķ�ʽ wangxin
typedef enum {
	MEETING_MODE_NORMAL = 0,	//��ͨ����
	MEETING_MODE_IM,			//IM��Ƶ����������
	MEETING_MODE_CALL,
}TMeetingMode;

// ͨ��ID����
typedef enum {
	HPCHANNEL_INDEX_MIN			= 0,		// ��Աͨ������Сֵ
	HPIPCAM_INDEX				= 50,		// IP Camͨ��ID
	HPCOMPOUNDCHANNEL_INDEX		= 99,		// �ϳ�ͨ��ID
	HPMEDIACHANNEL_INDEX		= 100,		// ý�干��ͨ��ID
	HPXSSECHANNEL_INDEX,					// ��Ļ����ͨ��ID
	HPCHATCHANNEL_INDEX,					// ����ͨ��ID
	HPWBCHANNEL_INDEX,						// �װ�ͨ��ID
	HPDOCSHARECHANNEL_INDEX,				// �ĵ�����ͨ��ID
	HPFILESENDCHANNEL_INDEX,				// �ļ��ַ�ͨ��ID
	HPROUNDCHANNEL_INDEX		= 200,		// ��ѯͨ��200-599
	HPROUNDCHANNEL_LOCAL_INDEX	= 600,		// ��ѭ����ͨ��600-699
	HPPREVIEW_LOCAL_INDEX		= 700,		// Ԥ��������Ƶ700-799
	HPH323CALL_INDEX			= 800,		// H323����ͨ��ID 800-899
	HPH323CALL_SECOND_INDEX		= 900,		// H323���и���ͨ��900-999
	HPIMCAMER_INDEX				= 1000,		// IMʹ�õ�ͨ��ID
	HPAVSET_INDEX,							// ����Ƶ����ʱ�ڲ���ͨ��ID
	HPCHANNEL_INDEX_MAX			= 1100, 	// ��Աͨ�������ֵ
}TCardIndex;

//// ����Ƶ���Ͷ���
//typedef enum {
//	HPAV_TYPE_NONE		= 0,
//	HPAV_TYPE_VIDEO		= 100,	// ��Ƶ
//	HPAV_TYPE_AUDIO,			// ��Ƶ
//	HPAV_TYPE_AV,				// ����Ƶ
//}THPAVStream;

// ��Ƶ���������Ͷ���
typedef enum {
	HPAV_AUDIO_CODEC_8		= 19/*X_AUDIO_CODEC_AMR_WB_9*/,		// 8kbps
	HPAV_AUDIO_CODEC_16		= 22/*X_AUDIO_CODEC_AMR_WB_16*/,		// 16kbps
	HPAV_AUDIO_CODEC_24		= 42/*X_AUDIO_CODEC_G7221_14_24*/,	// 24kbps
	HPAV_AUDIO_CODEC_32		= 43/*X_AUDIO_CODEC_G7221_14_32*/,	// 32kbps
	HPAV_AUDIO_CODEC_48		= 44/*X_AUDIO_CODEC_G7221_14_48*/,	// 48kbps
}THPAVAudioCodec;

// ��Ƶ���������Ͷ���
typedef enum {
	HPAV_VIDEO_CODEC_H263P		= 2/*VIDEC_CODEC_H263P*/,		// H263+
	HPAV_VIDEO_CODEC_H264		= 4/*VIDEC_CODEC_H264*/,			// H264+
	HPAV_VIDEO_CODEC_H264_SVC	= 10/*VIDEC_CODEC_H264_SVC*/,		// H264 SVC
}THPAVVideoCodec;

// �������Ͷ���
typedef enum {
	HPAV_VIDEO_NETWORK_INTERNET		= 0,	// ������
	HPAV_VIDEO_NETWORK_LAN,					// ������
	HPAV_VIDEO_NETWORK_PRIVATE,				// ר��
	HPAV_VIDEO_NETWORK_SATELLITE,			// ������
	HPAV_VIDEO_NETWORK_CDMA,				// CDMA
}THPAVVideoNetwork;

//---------------------------------------------------------------------------------------
// ��������Ϣ�ṹ
typedef struct _IMS_SERVER_ITEM
{
	std::string		strMCUID;
	std::string		strMCUType;
	unsigned long	ulConnections;
	std::string		strBandWidth;
	unsigned long	ulActivity;
	std::string		strIPAddr;
	unsigned short	usIPPort;
	std::string		strIPName;
	int				nMcuRightType;
}IMS_SERVER_ITEM,*PIMS_SERVER_ITEM;

typedef std::map<std::string,IMS_SERVER_ITEM> IMS_SERVER_ITEM_MAP;
typedef IMS_SERVER_ITEM_MAP::iterator IMS_SERVER_ITEM_MAP_IT;

//---------------------------------------------------------------------------------------
// ������Ϣ�ṹ
typedef struct _IMS_ORG_ITEM
{
	std::string strOrgID;
	std::string strOrgName;

	_IMS_ORG_ITEM()
	{
		strOrgID	= "";
		strOrgName	= "";
	};
}IMS_ORG_ITEM,*PIMS_ORG_ITEM;

typedef std::map<std::string,PIMS_ORG_ITEM> IMS_ORG_ITEM_MAP;
typedef IMS_ORG_ITEM_MAP::iterator IMS_ORG_ITEM_MAP_IT;

typedef struct _IMS_ORG_ITEM_EX :public IMS_ORG_ITEM
{
	std::string strParentKey;
	int		nLevelID;

	_IMS_ORG_ITEM_EX()
	{
		strParentKey	= "";
		nLevelID		= 1;
	};
}IMS_ORG_ITEM_EX,*PIMS_ORG_ITEM_EX;
typedef std::map<std::string,PIMS_ORG_ITEM_EX> IMS_ORG_ITEM_EX_MAP;
typedef IMS_ORG_ITEM_EX_MAP::iterator IMS_ORG_ITEM_EX_MAP_IT;


// ����������Ϣ�ṹ
typedef struct _IMS_DEPART_ITEM
{
	std::string strOrgID;
	std::string strDepartID;
	std::string strDepartName;
	int			nLevelID;
	std::string strUpgradeID;
	int			nOrderID;

	_IMS_DEPART_ITEM()
	{
		strOrgID		= "";
		strDepartID		= "";
		strDepartName	= "";
		nLevelID		= 0;
		strUpgradeID	= "";
		nOrderID		= 0;
	};
}IMS_DEPART_ITEM,*PIMS_DEPART_ITEM;

typedef std::map<std::string,PIMS_DEPART_ITEM> IMS_DEPART_ITEM_MAP;
typedef IMS_DEPART_ITEM_MAP::iterator IMS_DEPART_ITEM_MAP_IT;

// �����û���Ϣ�ṹ
typedef struct _IMS_ORGUSER_ITEM_EX
{
	std::string			strOrgID;
	std::string			strDepartID;
	IMS_CONTACT_ITEM	item;
	int					nOrderID;

	_IMS_ORGUSER_ITEM_EX()
	{
		strOrgID	= "";
		strDepartID	= "";
		nOrderID	= 0;
	};
}IMS_ORGUSER_ITEM_EX,*PIMS_ORGUSER_ITEM_EX;

typedef std::map<std::string,PIMS_ORGUSER_ITEM_EX> IMS_ORGUSER_ITEM_EX_MAP;
typedef IMS_ORGUSER_ITEM_EX_MAP::iterator IMS_ORGUSER_ITEM_EX_MAP_IT;


//---------------------------------------------------------------------------------------
// �������б�
typedef std::map<std::string,std::string> IMS_FRIEND_GROUP_MAP;
typedef IMS_FRIEND_GROUP_MAP::iterator IMS_FRIEND_GROUP_MAP_IT;


//---------------------------------------------------------------------------------------
// ���Һ����б�
typedef std::map<std::string,IMS_CONTACT_ITEM> IMS_SEARCH_FRIEND_MAP;
typedef IMS_SEARCH_FRIEND_MAP::iterator IMS_SEARCH_FRIEND_MAP_IT;


//---------------------------------------------------------------------------------------
// URL�б�
typedef struct _IMS_URL_ITEM
{
	std::string strName;
	std::string strURL;
	std::string strParams;

	_IMS_URL_ITEM()
	{
		strName		= "";
		strURL		= "";
		strParams	= "";
	};
}IMS_URL_ITEM,*PIMS_URL_ITEM;

typedef std::map<std::string,IMS_URL_ITEM> IMS_URL_ITEM_MAP;
typedef IMS_URL_ITEM_MAP::iterator IMS_URL_ITEM_MAP_IT;


//---------------------------------------------------------------------------------------
// �����ļ��б�
typedef struct _NDS_FILE_ITEM
{
	unsigned long	ulFileID;
	std::string		strFileName;
	std::string		strFileDesc;
	unsigned long	ulFileSize;
	std::string		strSendID;
	std::string		strRecvID;
	std::string     strFileMD5;//liangchuan2011/9/20
	_NDS_FILE_ITEM()
	{
		ulFileID		= 0;
		strFileName		= "";
		strFileDesc		= "";
		strSendID		= "";
		strRecvID		= "";
		strFileMD5      = ""; //liangchuan2011/9/20
	};
}NDS_FILE_ITEM,*PNDS_FILE_ITEM;

typedef std::map<unsigned long,NDS_FILE_ITEM> NDS_FILE_ITEM_MAP;
typedef NDS_FILE_ITEM_MAP::iterator NDS_FILE_ITEM_MAP_IT;


//---------------------------------------------------------------------------------------
// ��Ƶ�ֱ���
typedef std::map<int,std::string> VIDEO_SIZE_MAP;
typedef VIDEO_SIZE_MAP::iterator VIDEO_SIZE_MAP_IT;

// ��Ƶ�豸��Ϣ�ṹ
typedef struct _AUDIO_DEV
{
	std::string	strDeviceName;		// �豸����
	int			nDeviceNo;			// �豸���
}AUDIO_DEV,*PAUDIO_DEV;

typedef std::map<int,AUDIO_DEV> AUDIO_DEV_MAP;
typedef AUDIO_DEV_MAP::iterator AUDIO_DEV_MAP_IT;

// ��Ƶ�豸��Ϣ�ṹ
typedef struct _VIDEO_DEV
{
	std::string	strDeviceName;		// �豸����
	int			nDeviceNo;			// �豸���
	bool		bHDDevice;			// �Ƿ�Ϊ���忨
	bool		bHaveSoundCard;		// �Ƿ�������
}VIDEO_DEV,*PVIDEO_DEV;

typedef std::map<int,VIDEO_DEV> VIDEO_DEV_MAP;
typedef VIDEO_DEV_MAP::iterator VIDEO_DEV_MAP_IT;

/************************************************************************/
/*����Ƶ���ò�����Ϣ													*/
//��Ƶ�������ò�����Ϣ
typedef struct _VIDEOSET_INFO
{
	int	nVideoDev;			// ��Ƶ�豸
	int nVideoCodec;		// ��Ƶ������
	int	nVideoFrame;		// ͼ��֡��

	int	nBitrateCtrlType;	// ��������
	bool bVideoDenoise;		// ͼ��ȥ��
	int	nVideoSize;			// �ֱ���
	int	nVideoStream;		// ��Ƶ����


}VIDEOSET_INFO, *PVIDEOSET_INFO;

typedef std::map<int, VIDEOSET_INFO> VIDEOSET_INFO_MAP;
typedef VIDEOSET_INFO_MAP::iterator VIDEOSET_INFO_MAP_IT;

//����Ƶ���ò�����Ϣ
typedef struct _AUDIOSET_INFO
{
	int	nSpeakerDev;		// ������

	int nMicDev;			// ��˷�
	int	nAudioInType;		// ��Ƶ��������
	bool bAutoSetMicVol;	// �Զ�������˷�����

	int nAudioCodec;		// ��Ƶ������
	int nFEC;				// ǰ�����
	bool bMicBoost;			// ��˷���ǿ
	bool bVAD;				// �������
	bool bEchoCancel;		// ��������
	bool bLipSync;			// ����ͬ��

	int nPostposition;		// �����Զ�����
	int nNoisSuppression;	// ��������
	_AUDIOSET_INFO()
	{
		nSpeakerDev			= 0;
		nMicDev				= 0;		
		nAudioInType		= 0;	
		nAudioCodec			= 42/*X_AUDIO_CODEC_G7221_14_24*/; 

		bAutoSetMicVol		= 0;
		nFEC				= 0;			
		bMicBoost			= 0;		
		bVAD				= 0;			
		bEchoCancel			= 1;	
		bLipSync			= 0;		
		nPostposition		= 3;	
		nNoisSuppression	= 2;
	}

}AUDIOSET_INFO, *PAUDIOSET_INFO;

typedef std::map<int,AUDIOSET_INFO> AUDIOSET_INFO_MAP;
typedef AUDIOSET_INFO_MAP::iterator AUDIOSET_INFO_MAP_IT;
/************************************************************************/


//---------------------------------------------------------------------------------------
// �������б�����
typedef std::map<std::string,PMMS_GROUP_EX> MMS_GROUP_EX_MAP;		//key:strDomain_strRoomID_strGroupID
typedef MMS_GROUP_EX_MAP::iterator MMS_GROUP_EX_MAP_IT;


//---------------------------------------------------------------------------------------
// ��Աͨ�����ݽṹ
typedef struct _MMS_CARD
{
	std::string		memberid;		//��ԱID
	int				cardindex;		//��ID
	std::string		cardname;		//������
	unsigned long	audiochnid;		//��Ƶͨ��ID
	unsigned long	videochnid;		//��Ƶͨ��ID
	std::string		mcuid;			//MCU ID
	std::string		mcuaddr;		//MCU ��ַ
	unsigned short	mcuport;		//MCU �˿�
	std::string		devinfo;		//�豸��Ϣ

	int				screenid;		//��Ļ��
	int				windowid;		//���ں�

	_MMS_CARD()
	{
		memberid	= "";
		cardindex	= 0;
		cardname	= "";
		audiochnid	= 0;
		videochnid	= 0;
		mcuid		= "";
		mcuaddr		= "";
		mcuport		= 0;
		devinfo		= "";

		screenid	= -1;
		windowid	= -1;
	};
}MMS_CARD,*PMMS_CARD;

typedef std::map<std::string,PMMS_CARD> MMS_CARD_MAP;		//key:memberid_cardindex
typedef MMS_CARD_MAP::iterator MMS_CARD_MAP_IT;


//---------------------------------------------------------------------------------------
//��Ļ������Ϣ
typedef struct _MMS_WINDOW
{
	int				screenid;		//��Ļ��
	int				windowid;		//���ں�
	HWND			hwnd;			//���ھ��
	std::string		memberid;		//��ԱID
	int				cardindex;		//��ID

	_MMS_WINDOW()
	{
		screenid	= -1;
		windowid	= -1;
		hwnd		= NULL;
		memberid	= "";
		cardindex	= 0;
	};
}MMS_WINDOW,*PMMS_WINDOW;

typedef std::map<std::string,PMMS_WINDOW> MMS_WINDOW_MAP;	//key:screenid_windowid
typedef MMS_WINDOW_MAP::iterator MMS_WINDOW_MAP_IT;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//��ѯ
typedef struct _ROUND_CHANNEL
{
	int				nChannelType;	//1:��Ա  2:�豸
	int				nChannelIndex;
	std::string		strMemberID;
	std::string		strMemberName;
	std::string		strChannelID;
	std::string		strChannelName;
	int				nCardID;

	_ROUND_CHANNEL()
	{
		nChannelType	= 0;
		nChannelIndex	= 0;
		strMemberID		= "";
		strMemberName	= "";
		strChannelID	= "";
		strChannelName	= "";
		nCardID			= 0;
	}
}ROUND_CHANNEL, *PROUND_CHANNEL;
typedef std::map<int, ROUND_CHANNEL> MAP_ROUND_CHANNEL;
typedef MAP_ROUND_CHANNEL::iterator MAP_ROUND_CHANNEL_IT;

typedef struct _ROUND_GROUP
{
	std::string				strGroupName;
	int						nMode;		//0:����ģʽ  1:�̶�ģʽ
	int						nScreen;
	std::list<int>			lstWin;
	int						nRunWinCnt;
	int						nIntervalTime;
	bool					bIsRecvAudio;
	bool					bIsBroadcast;
	MAP_ROUND_CHANNEL		mapChannel;
	std::string				strResolution;
	int						nGroupStatus;

	_ROUND_GROUP()
	{
		strGroupName	= "";
		nMode			= 1;
		nScreen			= 0;
		nRunWinCnt		= 1;
		nIntervalTime	= 30;
		bIsRecvAudio	= false;
		bIsBroadcast	= false;
		strResolution	= "";
		nGroupStatus	= 0;
	};
}ROUND_GROUP, *PROUND_GROUP;
typedef std::map<int, ROUND_GROUP> MAP_ROUND_GROUP;
typedef MAP_ROUND_GROUP::iterator MAP_ROUND_GROUP_IT;

//////////////////////////////////////////////////////////////////////////
//��Ƶ����
typedef enum _TYPE_CALL_STATUS
{
	NO_CALL			= 0,	//δ����
	CALL_FAILD,				//����ʧ��
	CALL_SUCCESS,			//�����ɹ�
}TYPE_CALL_STATUS;

typedef struct _CALL_MEMBER
{
	int					nSortIndex;
	CString				strMemberName;
	bool				bOnline;
	TYPE_CALL_STATUS	CallStatus;

	_CALL_MEMBER()
	{
		nSortIndex		= 0;
		strMemberName	= _T("");
		bOnline			= false;
		CallStatus		= NO_CALL;
	}
}CALL_MEMBER, *PCALL_MEMBER;
typedef std::map<std::string, CALL_MEMBER> MAP_CALL_MEMBER;
typedef MAP_CALL_MEMBER::iterator MAP_CALL_MEMBER_IT;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//�����������
typedef struct _VIDEO_OUTPUT_INFO 
{
	int nScreenID;			//��Ļ��
	int nShowType;			//�����ʾ����
	int	nVideoSize;			// ��Ƶ�ֱ���	
	int	nVideoFrame;		// ͼ��֡��
	int	nVideoStream;		// ��Ƶ����
	int nTempleteBandWidth;	//ģ���ܴ���
	int nTMSize;			//ģ��ֱ���
	int nTempleteCutting;	//�Ƿ�ģ������и�
	int nWndShowType;		//��������ʾ��Ƶ
	_VIDEO_OUTPUT_INFO()
	{
		nScreenID = 0;
		nShowType = 0;
		nVideoSize = 0;
		nVideoFrame=0;
		nVideoStream=0;
		nTempleteBandWidth=0;
		nTMSize=0;
		nTempleteCutting=1;
		nWndShowType=0;
	};

}VIDEO_OUTPUT_INFO,*PVIDEO_OUTPUT_INFO;
//////////////////////////////////////////////////////////////////////////
