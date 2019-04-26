//IMXEC_Call.h
#ifndef __IMXEC_CALL_H__
#define __IMXEC_CALL_H__

#include <IMXEC/IMXEC_Export.h>
#include <VIDEC/VIDEC.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <winsock2.h>
#include <windows.h>
#include "AdapterHeaderDefine.h"
#include <IMXEC/IMXEC_VideoSetting.h>

typedef enum
{
	IMXEC_PERSONAL_CALL=0,
	IMXEC_CONFERENCE_CALL,
	IMXEC_MP_CALL,
	IMXEC_MCU_CALL,
}IMXEC_CALL_TYPE;

//���н���ԭ��
//typedef enum
//{
//	IMXEC_CALL_END_BY_TIMEOUT=0,		//���г�ʱ
//	IMXEC_CALL_END_BY_REMOTE_HANGUP,	//Զ�˽�������
//	IMXEC_CALL_END_BY_LOCAL_HANGUP		//������������
//}IMXEC_CALL_END_REASON;

//������йҶ�ԭ��
typedef enum
{
	ModeDisconnectedBusy = 0,                // �Զ�æ
	ModeDisconnectedNormal = 1,              // �Զ������Ҷ�
	ModeDisconnectedReject = 2,              // �Զ˾ܾ�
	ModeDisconnectedUnreachable = 3,         // �Զ˲��ɴ�
	ModeDisconnectedUnknown = 4,             // δ֪ԭ��Ҷ�
	ModeDisconnectedLocal = 5,               // ���������Ҷ�
	ModeDisconnectedNotFound = 6,            // �Զ��Ҳ���
	ModeDisconnectedServiceUnavailable = 7,  // �Զ˷��񲻿���
	ModeDisconnectedRequestTimeout = 8,      // ���г�ʱ�Ҷ�
	ModeDisconnectedRemote = 9,              // �Զ˹Ҷ�
	ModeDisconnectedRemoteNotReg = 10,       // �Զ�δע��ܾ�
	ModeDisconnectedNet = 11,                // ����ԭ��Ҷ�
	ModeDisconnectedError = 12,              // ���ú�������
	ModeDisconnectedReasonCount              //
}IMXEC_CALL_END_REASON;

typedef enum
{
	IMXEC_CALL_DIR_UNKOWN=0,
	IMXEC_CALL_DIR_IN,
	IMXEC_CALL_DIR_OUT,
}IMXEC_CALL_DIRECTION;




//H235 Media use

typedef enum
{
	IMXEC_CALL_H235AUTH_UNKOWN=0,
	IMXEC_CALL_H235AUTH_SUCCESS,         /*H235��֤ͨ��״̬*/
	IMXEC_CALL_H235AUTH_FAILURE,         /*H235��֤ʧ��״̬*/
	IMXEC_CALL_H235MEDIA_FAILURE         /*H235ý����������ԿЭ��ʧ��״̬*/
}IMXEC_CALL_H235AUTH_STATE;


typedef enum
{
	IMXEC_CALL_MEDIA_UNKOWN=0,
	IMXEC_CALL_MEDIA_AUDIO,              /*��Ƶ����*/
	IMXEC_CALL_MEDIA_MAINVIDEO,          /*��Ƶ����*/
	IMXEC_CALL_MEDIA_H239VIDEO,          /*��������*/
	IMXEC_CALL_MEDIA_H224DATA            /*H224��������*/

}IMXEC_CALL_MEDIA_TYPE;





//PTZ[�����ң������]
typedef enum 
{
	PTZ_START_PAN_UP=0,		//��ʼ����
	PTZ_STOP_PAN_UP,		//ֹͣ����
	PTZ_START_PAN_DOWN,		//��ʼ����
	PTZ_STOP_PAN_DOWN,		//ֹͣ����
	PTZ_START_TILT_LEFT,	//��ʼ����
	PTZ_STOP_TILT_LEFT,		//ֹͣ����
	PTZ_START_TILT_RIGHT,	//��ʼ����
	PTZ_STOP_TILT_RIGHT,	//ֹͣ����
	PTZ_START_ZOOM_IN,		//��ʼ�Ŵ�
	PTZ_STOP_ZOOM_IN,		//ֹͣ�Ŵ�
	PTZ_START_ZOOM_OUT,		//��ʼ��С
	PTZ_STOP_ZOOM_OUT,		//ֹͣ��С
	PTZ_START_FOCUS_IN,		//��ʼ������۽�
	PTZ_STOP_FOCUS_IN,		//ֹͣ������۽�
	PTZ_START_FOCUS_OUT,	//��ʼ��Զ���۽�
	PTZ_STOP_FOCUS_OUT,		//ֹͣ��Զ���۽�
	PTZ_START_FOCUS_AUTO,	//��ʼ�Զ��۽�
	PTZ_STOP_FOCUS_AUTO,	//ֹͣ�Զ��۽�
	PTZ_START_IRIS_MINUS,	//��ʼ��Ȧ��С
	PTZ_STOP_IRIS_MINUS,	//ֹͣ��Ȧ��С
	PTZ_START_IRIS_PLUS,	//��ʼ��Ȧ���
	PTZ_STTOPIRIS_PLUS,		//ֹͣ��Ȧ���
	PTZ_AUTO_REVERSE_ON,    //�Զ���תON
	PTZ_AUTO_REVERSE_OFF,   //�Զ���תOFF
	PTZ_BRIGHT_UP,          //��������
	PTZ_BRIGHT_DOWN,        //���ȼ�С
	PTZ_MENU_ON,            //��������˵�
	PTZ_MENU_OFF,           //�ر�������˵�
	PTZ_PRESET_CALL,		//Ԥ��λ����
}PTZ_ACTION;



#define MAX_AUDIO_CAP	32
#define MAX_VIDEO_CAP	32
#define MAX_DATA_CAP	2

typedef enum {
	DATA_TYPE_H224=0,
	DATA_TYPE_COUNT
}DATA_TYPE_ID;


typedef enum tagEntityType{
    EM_ENTITY_UNKNOWN=0,
    EM_ENTITY_MCU,
    EM_ENTITY_TERMINAL
}EM_ENTITY_TYPE;

//0 ���� 1 ���� 2 ȫ���� Ĭ��99ȫ����
enum ResStandard
{
	res4CIF=0,
	res720P=1,
	res1080P=2,
	resAll=99
};

typedef struct tagAudioCapability
{
	tagAudioCapability()
	{
		m_AudioCodecType=X_AUDIO_CODEC_COUNT;
		m_nAudioBitrate=0;
		m_nDirection=0;
	}

	X_AUDIO_CODEC_ID	m_AudioCodecType;
	int					m_nAudioBitrate;
	int                 m_nDirection;    //���䷽�� cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
}AudioCapability;

typedef struct tagVideoCapability
{
	tagVideoCapability()
	{
		m_VideoCodecType=VIDEC_CODEC_COUNT;
		m_VideoFormatType=VIDEO_FORMAT_COUNT;
		m_nVideoBitrate=0;
		m_nVideoFrameRate=0;
		m_nDirection=0;
		m_nFormatValue=0;
	}

	VIDEC_CODEC_TYPE	m_VideoCodecType;
	VIDEO_FORMAT_TYPE	m_VideoFormatType;
	int					m_nVideoBitrate;
	int                 m_nVideoFrameRate;
	int                 m_nDirection;    //���䷽�� cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
	int                 m_nFormatValue;  //�ֱ��ʶ�Ӧʵ�ʴ�С
}VideoCapability;

typedef struct tagDataCapability
{
	tagDataCapability()
	{
		m_DataCodecID=DATA_TYPE_H224;
		m_nDataBitrate=0;
		m_nDirection=0;
	}
	DATA_TYPE_ID	    m_DataCodecID;
	int					m_nDataBitrate;
	int                 m_nDirection;    //���䷽�� cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
}DataCapability;


typedef struct tagDapCallCapability
{
	tagDapCallCapability()
	{
		m_nAudioCapCount=0;
		m_nMainVideoCapCount=0;
		m_nSecondVideoCapCount=0;
		m_nDataCapCount=0;
	}

	int					m_nAudioCapCount;
	AudioCapability		m_aAudioCap[MAX_AUDIO_CAP];
	int					m_nMainVideoCapCount;
	VideoCapability		m_aMainVideoCap[MAX_VIDEO_CAP];
	int					m_nSecondVideoCapCount;
	VideoCapability		m_aSecondVideoCap[MAX_VIDEO_CAP];
	int					m_nDataCapCount;
	DataCapability		m_DataCap[MAX_DATA_CAP];
}DapCallCapability;

typedef struct tagConfInfo
{
	tagConfInfo()
	{
		memset(m_abyConfName,0,sizeof(m_abyConfName));
		memset(&m_DAPCallCapability,0,sizeof(DapCallCapability));
		memset(m_abyConfE164, 0, sizeof(m_abyConfE164));
		memset(m_abyLocalConfE164, 0, sizeof(m_abyLocalConfE164));
		m_nConfBitrate=0;
		m_nConfBitrate=0;
		m_nH235MediaUse=0;
		m_nH235AuthUse=0;
		m_nUseAlogrithmId=7;
		m_nEnableMulticast=0;
		m_nUseFWNAT =0;
	}
	char				m_abyConfName[256];		//������
	unsigned int		m_nConfBitrate;			//��������(Kbps)
	int					m_nH235MediaUse;		//0��ʹ�� 1��ѡʹ�� 2ǿ��ʹ��H235ý��������
	int					m_nH235AuthUse;			//0��ʹ�� 1ʹ��
	int					m_nUseAlogrithmId;		//��ѡ�����㷨,des is set 2, aes-128 is set 7 ,aes-256 is set 9 ,3DES is set 5
	DapCallCapability	m_DAPCallCapability;	// ���е�������
	char                m_abyConfE164[256];     // �Է�MCU����E164����
	char                m_abyLocalConfE164[256];// ����MCU����E164����
	unsigned int		m_nEnableMulticast;		// �Ƿ��鲥����
	int					m_nUseFWNAT;			// �Ƿ�����H460 NAT��͸,0 not use ,1 use,default 0
}ConfInfo;


//�������ն��������л���E164��,���ݴ˻���H235ý�����,H460��Ӧ�õ��������,�ں���ص�������,ע��Ҫ��ͬһ����������������������ͬ
typedef struct tagConfMediaSet
{
	tagConfMediaSet()
	{
		m_nH235MediaUse=0;
		m_nH235AuthUse=0;
		m_nUseAlogrithmId=7;
		m_nUseFWNAT =0;
		memset(m_abyTerminalE164,0,sizeof(m_abyTerminalE164));
		memset(m_abyTerminalH323ID,0,sizeof(m_abyTerminalH323ID));
		memset(m_abyTerminalIP,0,sizeof(m_abyTerminalIP));
		memset(m_abyOther,0,sizeof(m_abyOther));
	}
	int					m_nH235MediaUse;		  //0��ʹ�� 1��ѡʹ�� 2ǿ��ʹ��H235ý��������
	int					m_nH235AuthUse;			  //0��ʹ�� 1ʹ��
	int					m_nUseAlogrithmId;		  //��ѡ�����㷨,des is set 2, aes-128 is set 7 ,aes-256 is set 9 ,3DES is set 5
	int					m_nUseFWNAT;			  //�Ƿ�����H460 NAT��͸,0 not use ,1 use,default 0
	char                m_abyTerminalE164[256];   // �ն�E164����
	char                m_abyTerminalH323ID[256]; //�ն�H323ID
	char                m_abyTerminalIP[64];      // �ն�IP
	char                m_abyOther[128];          // ����
}ConfMediaSet;

typedef struct tagCallParamSet
{
	tagCallParamSet()
	{
		m_nEntityType =EM_ENTITY_UNKNOWN;
		m_nCallDir =IMXEC_CALL_DIR_UNKOWN;
		memset(m_strCallID,0,sizeof(m_strCallID));
		memset(m_strUserID,0,sizeof(m_strUserID));
		memset(m_strUserName,0,sizeof(m_strUserName));
		memset(m_strNonStand,0,sizeof(m_strNonStand));
		memset(m_strProductID,0,sizeof(m_strProductID));
		m_nMainVideoBitrate =0;
		m_nDualVideoBitrate =0;
	}

	EM_ENTITY_TYPE		 m_nEntityType;		  //�Զ����ն˻���MCU
	IMXEC_CALL_DIRECTION m_nCallDir;	      //���з���
	char                m_strCallID[256];     //�ն�callID
	char                m_strUserID[256];     //�ն�UserID
	char                m_strUserName[256];   //�ն�UserName
	char                m_strNonStand[256];   //�Զ������еķǱ���Ϣ����
	char                m_strProductID[256];  //�Զ�ProductID
	int                 m_nMainVideoBitrate;  //�Զ���������������� �ն��� ������ʱʹ��
	int                 m_nDualVideoBitrate;  //�Զ�������ĸ������� �ն��� ������ʱʹ��
}CallParamSet;



typedef enum
{
	IMXEC_CALL_PROT_UNKOWN=0,
	IMXEC_CALL_PROT_H323,
	IMXEC_CALL_PROT_SIP,
}IMXEC_CALL_PROTOCOL;

//ͨ����״̬ ֡��
typedef struct tagCallChannelInfo
{
	tagCallChannelInfo()
	{
		m_nFrame =0;
		m_bChannelOpen =false;
		m_nDualVideoDir =IMXEC_CALL_DIR_UNKOWN;
		m_nProtocol =IMXEC_CALL_PROT_UNKOWN;
	}
	int                   m_nFrame;
	bool                  m_bChannelOpen;
	IMXEC_CALL_DIRECTION  m_nDualVideoDir;
	IMXEC_CALL_PROTOCOL   m_nProtocol;
}CallChannelInfo;


class IMXEC_RecordCallback
{
public:
	IMXEC_RecordCallback(void){};
	virtual~IMXEC_RecordCallback(void){};
public:
	//��Ƶ���ݻص�����
	virtual void OnIMXEC_RecordCallbackAudioData(unsigned char*pData,int nLen)=0;
	virtual void OnIMXEC_RecordCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)=0;
	//����Ƶ���ݻص�����
	//nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
	virtual void OnIMXEC_RecordCallbackVideoData(unsigned char*pData,int nLen,int nKeyFrame)=0;
};

//������
class IMXEC_API IMXEC_Call
{
public:
	IMXEC_Call(void);
	virtual~ IMXEC_Call(void);
protected:
	int		m_nExclusive;
public:
	int GetExclusive(void);
public:
	/******************************************************************************
	* Start
	*��������������
	*���룺nExclusive		-�Ƿ���������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int Start(int nExclusive=0);

	/******************************************************************************
	* Stop
	*������ֹͣ����
	*���룺��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual void Stop(void);

	/******************************************************************************
	* OnIMXEC_CallRingIn
	*�����������¼������ն˺��б�����ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRingIn(const char*cszCallID,const char*cszUserID,const char*cszUserName)=0;

	/******************************************************************************
	* OnIMXEC_CallRingOut
	*�����������¼�������������гɹ���ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRingOut(const char*cszCallID,const char*cszUserID,const char*cszUserName)=0;

	/******************************************************************************
	* OnIMXEC_CallEstablished
	*���������н����¼�����˫������ͨ���󡾱���������Զ�˵ĺ��������Զ�˽����˱����ĺ�������ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallEstablished(const char*cszCallID,const char*cszUserID,const char*cszUserName)=0;

	/******************************************************************************
	* OnIMXEC_CallDestroyed
	*���������������¼�����ͨ�������󡾱����Ҷϻ�Զ�˹Ҷϡ�ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
			nReason			-����ԭ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallDestroyed(const char*cszCallID,const char*cszUserID,const char*cszUserName,IMXEC_CALL_END_REASON nReason)=0;

	/******************************************************************************
	* OnIMXEC_CallRecvdPTZ
	*���������յ�PTZ�¼������Է�������̨��������ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)=0;

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStart
	*������������ʼ�¼�����������ʼʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStart(const char*cszCallID)=0;

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStop
	*����������ֹͣ�¼���������ֹͣʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStop(const char*cszCallID)=0;

	/******************************************************************************
	* OnIMXEC_SIPVirtualLineRegistrationSuccess
	*������SIPע��ɹ��¼�
	*���룺	nLineID		-��·��ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_SIPVirtualLineRegistrationSuccess(int nLineID){};

	/******************************************************************************
	* OnIMXEC_SIPVirtualLineRegistrationError
	*������SIPע��ʧ���¼�
	*���룺	nLineID			-��·��ʶ
	        nCode           -ʧ��ԭ��������
			cszErrorCode	-ʧ��ԭ���ַ���
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_SIPVirtualLineRegistrationError(int nLineID,int nCode,const char*cszErrorCode){};

	/******************************************************************************
	* OnIMXEC_H323RegistrationSuccess
	*������H323ע��ɹ��¼�
	*���룺cszGKHost ע�ᵽ��GK��ַ port ע�ᵽ��GK�˿�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_H323RegistrationSuccess(const char*cszGKHost,int port){};

	/******************************************************************************
	* OnIMXEC_H323RegistrationError
	*������H323ע��ʧ���¼�
	*���룺nCode           -ʧ��ԭ��������
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_H323RegistrationError(int nCode){};

	/******************************************************************************
	* MakeCall
	*�������������
	*���룺	cszUserID		-�Է����û�ID��IP
			cszProtocol		-����Э�飬"H323","SIP","IMXEC"
	*�������
	*����ֵ�����б�ʶ
	*****************************************************************************/
	static const char*MakeCall(const char*cszUserID,const char*cszProtocol);

	/******************************************************************************
	* AcceptCall
	*���������ܺ�������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int AcceptCall(const char*cszCallID);

	/******************************************************************************
	* HangupCall
	*�������ҶϺ���
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int HangupCall(const char*cszCallID);

	/******************************************************************************
	* SendPTZ
	*����������PTZ��������̨�������
	*���룺	cszCallID		-���б�ʶ
			nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	/******************************************************************************
	* StartSecondVideo
	*��������ʼ����
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int StartSecondVideo (const char*cszCallID);

	/******************************************************************************
	* StopSecondVideo
	*������ֹͣ����
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int StopSecondVideo (const char*cszCallID);

	/******************************************************************************
	* SetLocalMainVideoWnd
	*���������ñ���������Ƶ��ʾ�Ĵ���
	*���룺	cszCallID		-���б�ʶ
			hWnd			-��������Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetLocalMainVideoWnd(const char*cszCallID,HWND hWnd);

	/******************************************************************************
	* SetLocalMainVideoWndPos
	*���������ñ���������Ƶ��ʾ�Ĵ���λ��
	*���룺	cszCallID		-���б�ʶ
			x				-���ڵ����λ��
			y				-���ڵ��ϱ�λ��
			cx				-���ڵĿ��
			cy				-���ڵĸ߶�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetLocalMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

	/******************************************************************************
	* SetLocalMainVideoParam
	*���������ñ���������Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetLocalMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetLocalMainVideoEnableOSD
	*���������ñ���������Ƶ�Ƿ�����OSD
	*���룺	cszCallID		-���б�ʶ
			nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetLocalMainVideoEnableOSD(const char*cszCallID,int nEnable);

	/******************************************************************************
	* SetLocalMainVideoOSDText
	*���������ñ���������ƵOSD����
	*���룺	cszCallID		-���б�ʶ
			cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetLocalMainVideoOSDText(const char*cszCallID,const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetLocalMainVideoOSDFont
	*���������ñ���������ƵOSD����,�ֵ���ɫ�ͱ���ɫ
	*���룺	cszCallID		-���б�ʶ
			logfont			-OSD����
			crText			-�ֵ���ɫ
			crBk			-����ɫ
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetLocalMainVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

	/******************************************************************************
	* GetLocalMainVideoInfo
	*��������ȡ����������Ƶ��Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			chInfo          -ͨ����״̬ ֡��
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

	/******************************************************************************
	* GetLocalMainVideoPacketStatistics
	*��������ȡ��������Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetRemoteMainVideoWnd
	*����������Զ��������Ƶ��ʾ�Ĵ���
	*���룺	cszCallID		-���б�ʶ
			hWnd			-Զ������Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetRemoteMainVideoWnd(const char*cszCallID,HWND hWnd);

	/******************************************************************************
	* SetRemoteMainVideoWndPos
	*����������Զ��������Ƶ��ʾ�Ĵ���λ��
	*���룺	cszCallID		-���б�ʶ
			x				-���ڵ����λ��
			y				-���ڵ��ϱ�λ��
			cx				-���ڵĿ��
			cy				-���ڵĸ߶�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetRemoteMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

	/******************************************************************************
	* SetRemoteMainVideoParam
	*����������Զ��������Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetRemoteMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen);

	/******************************************************************************
	* GetRemoteMainVideoInfo
	*��������ȡԶ��������Ƶ��Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			chInfo          -ͨ����״̬ ֡��
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

	/******************************************************************************
	* GetRemoteMainVideoPacketStatistics
	*��������ȡԶ������Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetSecondVideoWnd
	*���������ø�����Ƶ��ʾ�Ĵ���
	*���룺	cszCallID		-���б�ʶ
			hWnd			-Զ�̸�����Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetSecondVideoWnd(const char*cszCallID,HWND hWnd);

	/******************************************************************************
	* SetSecondVideoWndPos
	*���������ø�����Ƶ��ʾ�Ĵ���λ��
	*���룺	cszCallID		-���б�ʶ
			x				-���ڵ����λ��
			y				-���ڵ��ϱ�λ��
			cx				-���ڵĿ��
			cy				-���ڵĸ߶�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetSecondVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

	/******************************************************************************
	* SetSecondVideoParam
	*���������ø�����Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetSecondVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetSecondVideoEnableOSD
	*���������ø�����Ƶ�Ƿ�����OSD
	*���룺	cszCallID		-���б�ʶ
			nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetSecondVideoEnableOSD(const char*cszCallID,int nEnable);

	/******************************************************************************
	* SetSecondVideoOSDText
	*���������ø�����ƵOSD����
	*���룺	cszCallID		-���б�ʶ
			cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetSecondVideoOSDText(const char*cszCallID,const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetSecondVideoOSDFont
	*���������ø�����ƵOSD����,�ֵ���ɫ�ͱ���ɫ
	*���룺	cszCallID		-���б�ʶ
			logfont			-OSD����
			crText			-�ֵ���ɫ
			crBk			-����ɫ
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetSecondVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

	/******************************************************************************
	* GetSecondVideoInfo
	*��������ȡ������Ƶ��Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			chInfo          -ͨ����״̬ ֡��
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);
	
	/******************************************************************************
	* GetSecondVideoPacketStatistics
	*��������ȡ������Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetLocalAudioLevel
	*��������ȡ������ƵLevel��ʵ��������С��
	*���룺	cszCallID		-���б�ʶ
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetLocalAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetLocalAudioVolume
	*��������ȡ������Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	static void SetLocalAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetLocalAudioMute
	*��������ȡ������Ƶ�Ƿ���
	*���룺	cszCallID		-���б�ʶ
			nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	static void SetLocalAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetLocalAudioCodecID
	*��������ȡ������Ƶ������Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetLocalAudioPacketStatistics
	*��������ȡ������Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetRemoteAudioLevel
	*��������ȡԶ����ƵLevel��ʵ��������С��
	*���룺	cszCallID		-���б�ʶ
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetRemoteAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetRemoteAudioVolume
	*��������ȡԶ����Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	static void SetRemoteAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetRemoteAudioMute
	*��������ȡԶ����Ƶ�Ƿ���
	*���룺	cszCallID		-���б�ʶ
			nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	static void SetRemoteAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetRemoteAudioCodecID
	*��������ȡԶ����Ƶ����
	*���룺	cszCallID		-���б�ʶ
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetRemoteAudioPacketStatistics
	*��������ȡԶ����Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetCallType
	*���������ú�������
	*���룺	nCallType		-��������
	*�����	��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetCallType(IMXEC_CALL_TYPE nCallType);

	/******************************************************************************
	* GetCallType
	*��������ȡ��������
	*���룺	��
	*�����	nCallType		-��������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetCallType(IMXEC_CALL_TYPE&nCallType);

	/******************************************************************************
	* GetCallSupportSecondVideo
	*��������ȡ�����Ƿ�֧�ָ�����H239��
	*���룺	cszCallID		-���б�ʶ
	*�����	��
	*����ֵ��֧�ַ��ط�0����֧�ַ���0
	*****************************************************************************/
	static int GetCallSupportSecondVideo(const char*cszCallID);

	/******************************************************************************
	* StartRecordLocalMainToAVI
	*��������ʼ��������¼��
	*���룺	cszCallID			-���б�ʶ
			cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int StartRecordLocalMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordLocalMainToAVI
	*������ֹͣ��������¼��
	*���룺cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	static void StopRecordLocalMainToAVI(const char*cszCallID);

	/******************************************************************************
	* StartRecordRemoteMainToAVI
	*��������ʼԶ������¼��
	*���룺	cszCallID			-���б�ʶ
			cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int StartRecordRemoteMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordRemoteMainToAVI
	*������ֹͣԶ������¼��
	*���룺cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	static void StopRecordRemoteMainToAVI(const char*cszCallID);

	/******************************************************************************
	* StartRecordSecondToAVI
	*��������ʼ����¼��
	*���룺	cszCallID			-���б�ʶ
			cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int StartRecordSecondToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordSecondToAVI
	*������ֹͣ����¼��
	*���룺cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	static void StopRecordSecondToAVI(const char*cszCallID);

	/******************************************************************************
	* GetMainAudioID
	*��������ȡ������Ƶ��ʶ
	*���룺 cszCallID		-���б�ʶ
	*����� nMainAudioID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID);

	/******************************************************************************
	* GetMainVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 cszCallID		-���б�ʶ
	*����� nMainVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID);

	/******************************************************************************
	* GetSecondVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 cszCallID		-���б�ʶ
	*����� nSecondVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID);

	/******************************************************************************
	* SetMainVideoScreenIndex
	*����������������Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenIndex
	*���������ø�����Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenWnd
	*���������ø�����Ƶ������Ļ����λ��
	*���룺	cszCallID		-���б�ʶ
			nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetRemoteVideoScreenWnd
	*����������Զ����Ƶ������Ļ����λ��
	*���룺	cszCallID		-���б�ʶ
			nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetLocalRecordCallback
	*���������ñ���¼��ص�
	*���룺	cszCallID		-���б�ʶ
			pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetLocalRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetRemoteRecordCallback
	*����������Զ��¼��ص�
	*���룺	cszCallID		-���б�ʶ
			pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetRemoteRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetSecondRecordCallback
	*���������ø���¼��ص�
	*���룺	cszCallID		-���б�ʶ
			pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetSecondRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SaveLocalMainVideoToBMPFile
	*���������汾��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszCallID		-���б�ʶ
			cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SaveLocalMainVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SaveRemoteMainVideoToBMPFile
	*����������Զ��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszCallID		-���б�ʶ
			cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SaveRemoteMainVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SaveSecondVideoToBMPFile
	*���������渨����Ƶ��BMPͼƬ�ļ�
	*���룺	cszCallID		-���б�ʶ
			cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SaveSecondVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SendDTMF
	*����������DTMF
	*���룺	cszCallID		-���б�ʶ
			nDTMF			-DTMF
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SendDTMF(const char*cszCallID,const char nDTMF);

	/******************************************************************************
	* StartMixingWithWaveFile
	*��������ʼ�Ժ��н���WAVE�ļ��������Է�������WAVE�ļ����ŵ�����
	*���룺	cszCallID		-���б�ʶ
			cszPathName		-WAVE�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int StartMixingWithWaveFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* StopMixingWithWaveFile
	*������ֹͣ�Ժ��н���WAVE�ļ�����
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int StopMixingWithWaveFile(const char*cszCallID);

	/******************************************************************************
	* SetAudioOnly
	*�����������Ƿ�ֻ֧����Ƶ
	*���룺	nAudioOnly		-�Ƿ�ֻ֧����Ƶ��0��֧����Ƶ��Ƶ����0��ֻ֧����Ƶ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetAudioOnly(int nAudioOnly);

	/******************************************************************************
	* GetCallProtocol
	*��������ȡ����Э��
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ����غ���Э�飬ʧ�ܷ��ؿմ�""
	*****************************************************************************/
	static const char*GetCallProtocol(const char*cszCallID);

	/******************************************************************************
	* GetHasVideo
	*��������ȡ�Ƿ�ֻ����Ƶ
	*���룺cszCallID		-���б�ʶ
	*�������
	*����ֵ���������Ƶ����ô���ط�0�����򷵻�0
	*****************************************************************************/
	static int GetHasVideo(const char*cszCallID);

	/******************************************************************************
	* SetEnableVideoTranscoding
	*�����������Ƿ�������Ƶת��
	*���룺nEnable		-�Ƿ�������Ƶת�롾0����������Ƶת�룬��0��������Ƶת�롿
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetEnableVideoTranscoding(int nEnable);

	/******************************************************************************
	* GetEnableVideoTranscoding
	*��������ȡ�Ƿ�������Ƶת��
	*���룺��
	*�������
	*����ֵ�����������Ƶת�룬��ô���ط�0�����򷵻�0
	*****************************************************************************/
	static int GetEnableVideoTranscoding(void);

	/******************************************************************************
	* GetSecondVideoDirection
	*��������ȡ�����ķ���
	*���룺	cszCallID		-���б�ʶ
	*����� nDir			-�����ķ���
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir);




	//H235 Media use

	/******************************************************************************
	* GetH235AuthCheckState
	*��������ȡH235��֤���״̬ ����ȡ������֤��Ϊ����ʱ����֤���
	*���룺	cszCallID		  -���б�ʶ
	*����� nState  -��֤���״̬
			nLength -������֤ʧ��ԭ���ַ�������
	*����ֵ���ɹ�������֤ʧ��ԭ��ʧ�ܷ��ؿ�
	*****************************************************************************/
	const char* GetH235AuthCheckState(const char*cszCallID,IMXEC_CALL_H235AUTH_STATE &nState,int &nLength);

	/******************************************************************************
	* OnChannelRecvMediaEncryption
	*����������ý��ͨ���Ѽ���״̬֪ͨ�����򿪼��ܵĽ���ý��ͨ��ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	nMediaType		-ý������
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnChannelRecvMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType){};

	/******************************************************************************
	* OnChannelSendMediaEncryption
	*����������ý��ͨ���Ѽ���״̬֪ͨ�����򿪼��ܵķ���ý��ͨ��ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	nMediaType		-ý������
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnChannelSendMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType){};

	/******************************************************************************
	* SetExternalIP
	*��������������ӳ��IP
	*���룺cszExternalIP		-����ӳ��IP
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetExternalIP(const char*cszExternalIP);

	/******************************************************************************
	* OnDAP_RecvCallH245ControlMessage
	*���������յ�H245������Ϣ�ص� ����flowControlIndication H239������� �����Ϣ
	*���룺	cszCallID			-���б�ʶ
			nMsgType			-������Ϣ����
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_RecvCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen){};

	//added by jiangdingfeng 2013-09-25 ����I֡����
	/******************************************************************************
	* RequestMainVideoKeyFrame
	*��������������I֡
	*���룺cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	static void RequestMainVideoKeyFrame(const char*cszCallID);
	/******************************************************************************
	* RequestSecondVideoKeyFrame
	*������������I֡
	*���룺cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	static void RequestSecondVideoKeyFrame(const char*cszCallID);
	/******************************************************************************
	* SetLocalMainVideoSource
	*���������ñ�������ƵԴ
	*���룺cszCallID		  -���б�ʶ
	       nMainVideoDevID    -�����豸ID
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetLocalMainVideoSource(const char*cszCallID, int nMainVideoDevID);
	/******************************************************************************
	* SetLocalSecondVideoSource
	*���������ñ��ظ���ƵԴ
	*���룺cszCallID		  -���б�ʶ
		   nSecondVideoDevID  -�����豸ID
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetLocalSecondVideoSource(const char*cszCallID, int nSecondVideoDevID);

	//add use to send private msg
	/******************************************************************************
	*����:	��Զ˷����Զ���Ǳ���Ϣ
	*����:	cszCallID		-���б�ʶ
			cszContent		-�Զ���Ǳ�׼��Ϣ����
			nContentLen		-�Զ���Ǳ�׼��Ϣ����
			cszMimeType		-�Զ���Ǳ���Ϣ�������� ��ΪXML ֵ��Ϊapplication/xml,��Ϊһ���ַ�����Ϊapplication/message+text 
			��Ϊ�����ַ�����Ϊapplication/message+xml
	*****************************************************************************/
	static int SendCallCustomMsg(const char*cszCallID,const char * cszContent,int nContentLen,const char * cszMimeType);

	/******************************************************************************
	* OnDAP_CallRecvNonstandardData
	*���������յ����ն˷����Ǳ���Ϣʱϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszPacket		-�Ǳ�׼��Ϣ����
			nPacketLen		-�Ǳ�׼��Ϣ����
			cszMimeType		-Ŀǰ��SIPʹ�� ��Ϣ�������� ��application/xml
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_RecvNonstandardData(const char*cszCallID,const char * cszPacket,int nPacketLen,const char * cszMimeType){};

	/******************************************************************************
	* HotDisplayVideo
	*��������Ƶ��ʾ֧����ʾ���Ȳ�νӿ�
	*���룺	cszCallID		    -���б�ʶ
			hWndLocal			-��������Ƶ��ʾ�Ĵ��ھ�� ��û��������ΪNULL
			hWndRemote			-Զ������Ƶ��ʾ�Ĵ��ھ�� ��û��������ΪNULL
			hWndSecond         	-Զ�̸�����Ƶ��ʾ�Ĵ��ھ�� ��û��������ΪNULL
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int HotDisplayVideo(const char*cszCallID,HWND hWndLocal,HWND hWndRemote,HWND hWndSecond);

	/******************************************************************************
	* GetCallH323ID
	*��������ȡH323ID
	*���룺	cszCallID		-���б�ʶ
            szH323ID        -H323ID Buf
			nMaxLen         -Buf ��󳤶�
	*�����	��
	*����ֵ����ȡ������H323ID���ȣ����򷵻�NULL
	*****************************************************************************/
	static int GetCallH323ID(const char*cszCallID ,char * szH323ID,int nMaxLen);
	/******************************************************************************
	* GetCallE164ID
	*��������ȡE164ID
	*���룺	cszCallID		-���б�ʶ
            szE164ID        -E164ID Buf
			nMaxLen         -Buf ��󳤶�
	*�����	��
	*����ֵ����ȡ������E164ID���ȣ����򷵻�NULL
	*****************************************************************************/
	static int GetCallE164ID(const char*cszCallID ,char * szE164ID,int nMaxLen);



	//Add U6 U7 use
	/******************************************************************************
	* OnDAP_RecvRemoteAudioRTPPacket
	*���������յ���Ƶý�����ص�
	*���룺	cszCallID			-���б�ʶ
	        codecID             -��Ƶ��������
			pRTPPacketData		-RTP��
			nRTPPacketLen		-RTP������
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP��ͷ����
			usSequence			-RTP������
			ulTimestamp			-RTP��ʱ���
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteAudioRTPPacket(const char*cszCallID,X_AUDIO_CODEC_ID codecID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp) {};

	/******************************************************************************
	* OnDAP_RecvRemoteMainVideoRTPPacket
	*���������յ�������Ƶý�����ص�
	*���룺	cszCallID			-���б�ʶ
	        codecType           -��Ƶ��������
			pRTPPacketData		-RTP��
			nRTPPacketLen		-RTP������
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP��ͷ����
			usSequence			-RTP������
			ulTimestamp			-RTP��ʱ���
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteMainVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp) {};
	
	/******************************************************************************
	* OnDAP_RecvRemoteSecondVideoRTPPacket
	*���������յ�������Ƶý�����ص�
	*���룺	cszCallID			-���б�ʶ
	        codecType           -��Ƶ��������
			pRTPPacketData		-RTP��
			nRTPPacketLen		-RTP������
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP��ͷ����
			usSequence			-RTP������
			ulTimestamp			-RTP��ʱ���
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteSecondVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp) {};



	/******************************************************************************
	* OnPeerEntityInfo
	*���������ضԶ�ʵ����Ϣ
	*���룺	pCallParam			-CallParamSet
	*�������
	*����ֵ����
	*****************************************************************************/
    virtual void OnPeerEntityInfo(const CallParamSet * pCallParam) {};


	/******************************************************************************
	* SetNetecTransEnable
	*�������Ƿ�ʹ��NETEC˽�д���
	*���룺	cszCallID			-���б�ʶ
	        nEnable             -�Ƿ�ʹ��,1Ϊʹ��,0Ϊ��ʹ��
	*�������
	*����ֵ����
	*****************************************************************************/
    static void SetNetecTransEnable(const char*cszCallID, int nEnable);

	//add by Jan for DTMF [20140730]
	/******************************************************************************
	* OnIMXEC_CallRecvdDTMF
	*���������յ�DTMF�¼������Է�����DTMF��������ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			szDtmf			-DTMF����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRecvdDTMF(const char*cszCallID,char szDtmf) =0;
	//end [20140730]

	/******************************************************************************
	* SetResolutionStand
	*�����������ն�֧�ֵ����ֱ�������
	*���룺	nResStand			-���ֱ�������
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetResolutionStand(ResStandard nResStand);




	/******************************************************************************
	* OnDAP_CallAudioRecvChannelConnected
	*��������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
			AudioCodecID		-��ƵCodecID
			nPayloadType		-PayloadType
			ip					-���˽���rtp ip
			port				-���˽���rtp port
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallAudioRecvChannelConnected(const char*cszCallID,X_AUDIO_CODEC_ID AudioCodecID
		,int nPayloadType,unsigned long ip, int port) {};

	/******************************************************************************
	* OnDAP_CallAudioRecvChannelConnected
	*��������Ƶ����ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallAudioRecvChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallMainVideoRecvChannelConnected
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
			codecType			-��ƵCodecID
			nBitrate			-����
			nPayloadType		-PayloadType
			width				-�ֱ��ʿ�������� ��ѡ
			nHeight				-�ֱ��ʸ߶������� ��ѡ
			ip					-���˽���rtp ip
			port				-���˽���rtp port
			nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoRecvChannelConnected(const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth=0,int nHeight=0,*/int nLevel=0) {};

	/******************************************************************************
	* OnDAP_CallMainVideoRecvChannelDestroyed
	*������������Ƶ����ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoRecvChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallSecondVideoRecvChannelConnected
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
			codecType			-��ƵCodecID
			nBitrate			-����
			nPayloadType		-PayloadType
			width				-�ֱ��ʿ�������� ��ѡ
			nHeight				-�ֱ��ʸ߶������� ��ѡ
			ip					-���˽���rtp ip
			port				-���˽���rtp port
			nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoRecvChannelConnected(const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth=0,int nHeight=0,*/int nLevel=0) {};

	/******************************************************************************
	* OnDAP_CallSecondVideoRecvChannelDestroyed
	*������������Ƶ����ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoRecvChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallFECCRecvChannelConnected
	*������FECC���ƽ���ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
			nPayloadType		-PayloadType
			ip					-���˽���rtp ip
			port				-���˽���rtp port
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallFECCRecvChannelConnected(const char*cszCallID ,int nPayloadType,unsigned long ip
		, int port) {};

	/******************************************************************************
	* OnDAP_CallFECCRecvChannelDestroyed
	*������FECC���ƽ���ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallFECCRecvChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallAudioSendChannelConnected
	*��������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			AudioCodecID		-��ƵCodecID
			nPayloadType		-PayloadType
			ip					-�Զ�rtp ip
			port				-�Զ�rtp port
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallAudioSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,X_AUDIO_CODEC_ID AudioCodecID
		,int nPayloadType,unsigned long ip, int port) {};

	/******************************************************************************
	* OnDAP_CallAudioSendChannelDestroyed
	*��������Ƶ����ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallAudioSendChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallMainVideoSendChannelConnected
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			codecType			-��ƵCodecID
			nBitrate			-����
			nPayloadType		-PayloadType
			width				-�ֱ��ʿ�������� ��ѡ
			nHeight				-�ֱ��ʸ߶������� ��ѡ
			nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
			ip					-�Զ�rtp ip
			port				-�Զ�rtp port
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip,int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth,int nHeight,*/int nLevel = 0) {};

	/******************************************************************************
	* OnDAP_CallMainVideoSendChannelDestroyed
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoSendChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallSecondVideoSendChannelConnected
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			codecType			-��ƵCodecID
			nBitrate			-����
			nPayloadType		-PayloadType
			width				-�ֱ��ʿ�������� ��ѡ
			nHeight				-�ֱ��ʸ߶������� ��ѡ
			nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
			ip					-�Զ�rtp ip
			port				-�Զ�rtp port
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth,int nHeight,*/int nLevel = 0) {};
	
	/******************************************************************************
	* OnDAP_CallSecondVideoSendChannelDestroyed
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoSendChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallFECCSendChannelConnected
	*������FECC���Ʒ���ͨ�����ӳɹ��¼�
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			ip					-�Զ�rtp ip
			port				-�Զ�rtp port
			nPayloadType		-PayloadType
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallFECCSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID,int nPayloadType
		,unsigned long ip, int port) {};

	/******************************************************************************
	* OnDAP_CallFECCSendChannelDestroyed
	*������FECC���Ʒ���ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallFECCSendChannelDestroyed(const char*cszCallID) {};


#ifdef _TIANZHU_CACHE_
	/******************************************************************************
	* SetRemoteVideoCache
	*����������Զ���Ӳ��ŵĻ���֡��,ֻ֧��IMX��������
	*���룺	cszCallID			-���б�ʶ
	        nCacheFrameCount	-�����֡����0 - 60��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetRemoteVideoCache(const char*cszCallID, int nCacheFrameCount);
#endif

};



#endif