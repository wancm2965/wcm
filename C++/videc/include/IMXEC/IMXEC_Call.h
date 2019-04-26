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

//呼叫接收原因
//typedef enum
//{
//	IMXEC_CALL_END_BY_TIMEOUT=0,		//呼叫超时
//	IMXEC_CALL_END_BY_REMOTE_HANGUP,	//远端结束呼叫
//	IMXEC_CALL_END_BY_LOCAL_HANGUP		//本机结束呼叫
//}IMXEC_CALL_END_REASON;

//定义呼叫挂断原因
typedef enum
{
	ModeDisconnectedBusy = 0,                // 对端忙
	ModeDisconnectedNormal = 1,              // 对端正常挂断
	ModeDisconnectedReject = 2,              // 对端拒绝
	ModeDisconnectedUnreachable = 3,         // 对端不可达
	ModeDisconnectedUnknown = 4,             // 未知原因挂断
	ModeDisconnectedLocal = 5,               // 本端主动挂断
	ModeDisconnectedNotFound = 6,            // 对端找不到
	ModeDisconnectedServiceUnavailable = 7,  // 对端服务不可用
	ModeDisconnectedRequestTimeout = 8,      // 呼叫超时挂断
	ModeDisconnectedRemote = 9,              // 对端挂断
	ModeDisconnectedRemoteNotReg = 10,       // 对端未注册拒绝
	ModeDisconnectedNet = 11,                // 网络原因挂断
	ModeDisconnectedError = 12,              // 调用函数错误
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
	IMXEC_CALL_H235AUTH_SUCCESS,         /*H235验证通过状态*/
	IMXEC_CALL_H235AUTH_FAILURE,         /*H235验证失败状态*/
	IMXEC_CALL_H235MEDIA_FAILURE         /*H235媒体流共享密钥协商失败状态*/
}IMXEC_CALL_H235AUTH_STATE;


typedef enum
{
	IMXEC_CALL_MEDIA_UNKOWN=0,
	IMXEC_CALL_MEDIA_AUDIO,              /*音频类型*/
	IMXEC_CALL_MEDIA_MAINVIDEO,          /*视频类型*/
	IMXEC_CALL_MEDIA_H239VIDEO,          /*辅流类型*/
	IMXEC_CALL_MEDIA_H224DATA            /*H224数据类型*/

}IMXEC_CALL_MEDIA_TYPE;





//PTZ[摄像机遥控命令]
typedef enum 
{
	PTZ_START_PAN_UP=0,		//开始向上
	PTZ_STOP_PAN_UP,		//停止向上
	PTZ_START_PAN_DOWN,		//开始向下
	PTZ_STOP_PAN_DOWN,		//停止向下
	PTZ_START_TILT_LEFT,	//开始向左
	PTZ_STOP_TILT_LEFT,		//停止向左
	PTZ_START_TILT_RIGHT,	//开始向右
	PTZ_STOP_TILT_RIGHT,	//停止向右
	PTZ_START_ZOOM_IN,		//开始放大
	PTZ_STOP_ZOOM_IN,		//停止放大
	PTZ_START_ZOOM_OUT,		//开始缩小
	PTZ_STOP_ZOOM_OUT,		//停止缩小
	PTZ_START_FOCUS_IN,		//开始向近处聚焦
	PTZ_STOP_FOCUS_IN,		//停止向近处聚焦
	PTZ_START_FOCUS_OUT,	//开始向远处聚焦
	PTZ_STOP_FOCUS_OUT,		//停止向远处聚焦
	PTZ_START_FOCUS_AUTO,	//开始自动聚焦
	PTZ_STOP_FOCUS_AUTO,	//停止自动聚焦
	PTZ_START_IRIS_MINUS,	//开始光圈变小
	PTZ_STOP_IRIS_MINUS,	//停止光圈变小
	PTZ_START_IRIS_PLUS,	//开始光圈变大
	PTZ_STTOPIRIS_PLUS,		//停止光圈变大
	PTZ_AUTO_REVERSE_ON,    //自动翻转ON
	PTZ_AUTO_REVERSE_OFF,   //自动翻转OFF
	PTZ_BRIGHT_UP,          //亮度增加
	PTZ_BRIGHT_DOWN,        //亮度减小
	PTZ_MENU_ON,            //打开摄像机菜单
	PTZ_MENU_OFF,           //关闭摄像机菜单
	PTZ_PRESET_CALL,		//预置位调用
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

//0 标清 1 高清 2 全高清 默认99全能力
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
	int                 m_nDirection;    //传输方向 cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
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
	int                 m_nDirection;    //传输方向 cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
	int                 m_nFormatValue;  //分辨率对应实际大小
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
	int                 m_nDirection;    //传输方向 cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
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
	char				m_abyConfName[256];		//会议名
	unsigned int		m_nConfBitrate;			//会议码率(Kbps)
	int					m_nH235MediaUse;		//0不使用 1可选使用 2强制使用H235媒体流加密
	int					m_nH235AuthUse;			//0不使用 1使用
	int					m_nUseAlogrithmId;		//首选加密算法,des is set 2, aes-128 is set 7 ,aes-256 is set 9 ,3DES is set 5
	DapCallCapability	m_DAPCallCapability;	// 呼叫的能力集
	char                m_abyConfE164[256];     // 对方MCU会议E164号码
	char                m_abyLocalConfE164[256];// 本机MCU会议E164号码
	unsigned int		m_nEnableMulticast;		// 是否组播会议
	int					m_nUseFWNAT;			// 是否启用H460 NAT穿透,0 not use ,1 use,default 0
}ConfInfo;


//此用于终端主动呼叫会议E164号,根据此会议H235媒体加密,H460等应用的启用情况,在呼入回调中设置,注意要与同一会议主动发出呼叫设置相同
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
	int					m_nH235MediaUse;		  //0不使用 1可选使用 2强制使用H235媒体流加密
	int					m_nH235AuthUse;			  //0不使用 1使用
	int					m_nUseAlogrithmId;		  //首选加密算法,des is set 2, aes-128 is set 7 ,aes-256 is set 9 ,3DES is set 5
	int					m_nUseFWNAT;			  //是否启用H460 NAT穿透,0 not use ,1 use,default 0
	char                m_abyTerminalE164[256];   // 终端E164号码
	char                m_abyTerminalH323ID[256]; //终端H323ID
	char                m_abyTerminalIP[64];      // 终端IP
	char                m_abyOther[128];          // 备用
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

	EM_ENTITY_TYPE		 m_nEntityType;		  //对端是终端还是MCU
	IMXEC_CALL_DIRECTION m_nCallDir;	      //呼叫方向
	char                m_strCallID[256];     //终端callID
	char                m_strUserID[256];     //终端UserID
	char                m_strUserName[256];   //终端UserName
	char                m_strNonStand[256];   //对端信令中的非标消息内容
	char                m_strProductID[256];  //对端ProductID
	int                 m_nMainVideoBitrate;  //对端信令带的主流码率 终端用 作被叫时使用
	int                 m_nDualVideoBitrate;  //对端信令带的辅流码率 终端用 作被叫时使用
}CallParamSet;



typedef enum
{
	IMXEC_CALL_PROT_UNKOWN=0,
	IMXEC_CALL_PROT_H323,
	IMXEC_CALL_PROT_SIP,
}IMXEC_CALL_PROTOCOL;

//通道打开状态 帧率
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
	//音频数据回调函数
	virtual void OnIMXEC_RecordCallbackAudioData(unsigned char*pData,int nLen)=0;
	virtual void OnIMXEC_RecordCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)=0;
	//主视频数据回调函数
	//nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnIMXEC_RecordCallbackVideoData(unsigned char*pData,int nLen,int nKeyFrame)=0;
};

//呼叫类
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
	*描述：启动呼叫
	*输入：nExclusive		-是否是排他的
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int Start(int nExclusive=0);

	/******************************************************************************
	* Stop
	*描述：停止呼叫
	*输入：无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual void Stop(void);

	/******************************************************************************
	* OnIMXEC_CallRingIn
	*描述：呼入事件，有终端呼叫本机的时候系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszUserID		-用户标识或用户IP
			cszUserName		-用户名称
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallRingIn(const char*cszCallID,const char*cszUserID,const char*cszUserName)=0;

	/******************************************************************************
	* OnIMXEC_CallRingOut
	*描述：呼出事件，本机发起呼叫成功后系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszUserID		-用户标识或用户IP
			cszUserName		-用户名称
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallRingOut(const char*cszCallID,const char*cszUserID,const char*cszUserName)=0;

	/******************************************************************************
	* OnIMXEC_CallEstablished
	*描述：呼叫建立事件，当双方建立通话后【本机接受了远端的呼叫请求或远端接受了本机的呼叫请求】系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszUserID		-用户标识或用户IP
			cszUserName		-用户名称
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallEstablished(const char*cszCallID,const char*cszUserID,const char*cszUserName)=0;

	/******************************************************************************
	* OnIMXEC_CallDestroyed
	*描述：呼叫销毁事件，当通话结束后【本机挂断或远端挂断】系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszUserID		-用户标识或用户IP
			cszUserName		-用户名称
			nReason			-结束原因
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallDestroyed(const char*cszCallID,const char*cszUserID,const char*cszUserName,IMXEC_CALL_END_REASON nReason)=0;

	/******************************************************************************
	* OnIMXEC_CallRecvdPTZ
	*描述：接收到PTZ事件，当对方发送云台控制命令时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			nPTZAction		-云台动作
			nSpeed			-动作的速度
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)=0;

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStart
	*描述：辅流开始事件，当辅流开始时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStart(const char*cszCallID)=0;

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStop
	*描述：辅流停止事件，当辅流停止时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStop(const char*cszCallID)=0;

	/******************************************************************************
	* OnIMXEC_SIPVirtualLineRegistrationSuccess
	*描述：SIP注册成功事件
	*输入：	nLineID		-线路标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_SIPVirtualLineRegistrationSuccess(int nLineID){};

	/******************************************************************************
	* OnIMXEC_SIPVirtualLineRegistrationError
	*描述：SIP注册失败事件
	*输入：	nLineID			-线路标识
	        nCode           -失败原因类型码
			cszErrorCode	-失败原因字符串
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_SIPVirtualLineRegistrationError(int nLineID,int nCode,const char*cszErrorCode){};

	/******************************************************************************
	* OnIMXEC_H323RegistrationSuccess
	*描述：H323注册成功事件
	*输入：cszGKHost 注册到的GK地址 port 注册到的GK端口
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_H323RegistrationSuccess(const char*cszGKHost,int port){};

	/******************************************************************************
	* OnIMXEC_H323RegistrationError
	*描述：H323注册失败事件
	*输入：nCode           -失败原因类型码
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_H323RegistrationError(int nCode){};

	/******************************************************************************
	* MakeCall
	*描述：发起呼叫
	*输入：	cszUserID		-对方的用户ID或IP
			cszProtocol		-呼叫协议，"H323","SIP","IMXEC"
	*输出：无
	*返回值：呼叫标识
	*****************************************************************************/
	static const char*MakeCall(const char*cszUserID,const char*cszProtocol);

	/******************************************************************************
	* AcceptCall
	*描述：接受呼叫请求
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int AcceptCall(const char*cszCallID);

	/******************************************************************************
	* HangupCall
	*描述：挂断呼叫
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int HangupCall(const char*cszCallID);

	/******************************************************************************
	* SendPTZ
	*描述：发送PTZ【发送云台控制命令】
	*输入：	cszCallID		-呼叫标识
			nPTZAction		-云台动作
			nSpeed			-动作的速度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	/******************************************************************************
	* StartSecondVideo
	*描述：开始辅流
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int StartSecondVideo (const char*cszCallID);

	/******************************************************************************
	* StopSecondVideo
	*描述：停止辅流
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int StopSecondVideo (const char*cszCallID);

	/******************************************************************************
	* SetLocalMainVideoWnd
	*描述：设置本地主流视频显示的窗口
	*输入：	cszCallID		-呼叫标识
			hWnd			-本地主视频显示的窗口句柄
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetLocalMainVideoWnd(const char*cszCallID,HWND hWnd);

	/******************************************************************************
	* SetLocalMainVideoWndPos
	*描述：设置本地主流视频显示的窗口位置
	*输入：	cszCallID		-呼叫标识
			x				-窗口的左边位置
			y				-窗口的上边位置
			cx				-窗口的宽度
			cy				-窗口的高度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetLocalMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

	/******************************************************************************
	* SetLocalMainVideoParam
	*描述：设置本地主流视频参数
	*输入：	cszCallID		-呼叫标识
			nBrightness		-亮度值0-255
			nContrast		-对比度0-127
			nSaturation		-饱和度0-127
			nHue			-色调0-255
			nGamma			-伽马1-400
			nSharpen		-锐化0-127
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetLocalMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetLocalMainVideoEnableOSD
	*描述：设置本地主流视频是否启用OSD
	*输入：	cszCallID		-呼叫标识
			nEnable			-是否启用OSD
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetLocalMainVideoEnableOSD(const char*cszCallID,int nEnable);

	/******************************************************************************
	* SetLocalMainVideoOSDText
	*描述：设置本地主流视频OSD文字
	*输入：	cszCallID		-呼叫标识
			cszText			-OSD文字
			nAlign			-对齐方式
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetLocalMainVideoOSDText(const char*cszCallID,const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetLocalMainVideoOSDFont
	*描述：设置本地主流视频OSD字体,字的颜色和背景色
	*输入：	cszCallID		-呼叫标识
			logfont			-OSD字体
			crText			-字的颜色
			crBk			-背景色
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetLocalMainVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

	/******************************************************************************
	* GetLocalMainVideoInfo
	*描述：获取本地主流视频信息
	*输入：	cszCallID		-呼叫标识
	*输出：	nWidth			-图像宽度
			nHeight			-图像高度
			nVideoCodecType	-编码类型
			chInfo          -通道打开状态 帧率
			hVideoWnd		-窗口句柄
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

	/******************************************************************************
	* GetLocalMainVideoPacketStatistics
	*描述：获取本地主视频的包的统计数据
	*输入：	cszCallID			-呼叫标识
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetRemoteMainVideoWnd
	*描述：设置远程主流视频显示的窗口
	*输入：	cszCallID		-呼叫标识
			hWnd			-远程主视频显示的窗口句柄
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetRemoteMainVideoWnd(const char*cszCallID,HWND hWnd);

	/******************************************************************************
	* SetRemoteMainVideoWndPos
	*描述：设置远程主流视频显示的窗口位置
	*输入：	cszCallID		-呼叫标识
			x				-窗口的左边位置
			y				-窗口的上边位置
			cx				-窗口的宽度
			cy				-窗口的高度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetRemoteMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

	/******************************************************************************
	* SetRemoteMainVideoParam
	*描述：设置远程主流视频参数
	*输入：	cszCallID		-呼叫标识
			nBrightness		-亮度值0-255
			nContrast		-对比度0-127
			nSaturation		-饱和度0-127
			nHue			-色调0-255
			nGamma			-伽马1-400
			nSharpen		-锐化0-127
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetRemoteMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen);

	/******************************************************************************
	* GetRemoteMainVideoInfo
	*描述：获取远程主流视频信息
	*输入：	cszCallID		-呼叫标识
	*输出：	nWidth			-图像宽度
			nHeight			-图像高度
			nVideoCodecType	-编码类型
			chInfo          -通道打开状态 帧率
			hVideoWnd		-窗口句柄
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

	/******************************************************************************
	* GetRemoteMainVideoPacketStatistics
	*描述：获取远端主视频的包的统计数据
	*输入：	cszCallID			-呼叫标识
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetSecondVideoWnd
	*描述：设置辅流视频显示的窗口
	*输入：	cszCallID		-呼叫标识
			hWnd			-远程辅流视频显示的窗口句柄
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetSecondVideoWnd(const char*cszCallID,HWND hWnd);

	/******************************************************************************
	* SetSecondVideoWndPos
	*描述：设置辅流视频显示的窗口位置
	*输入：	cszCallID		-呼叫标识
			x				-窗口的左边位置
			y				-窗口的上边位置
			cx				-窗口的宽度
			cy				-窗口的高度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetSecondVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

	/******************************************************************************
	* SetSecondVideoParam
	*描述：设置辅流视频参数
	*输入：	cszCallID		-呼叫标识
			nBrightness		-亮度值0-255
			nContrast		-对比度0-127
			nSaturation		-饱和度0-127
			nHue			-色调0-255
			nGamma			-伽马1-400
			nSharpen		-锐化0-127
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetSecondVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetSecondVideoEnableOSD
	*描述：设置辅流视频是否启用OSD
	*输入：	cszCallID		-呼叫标识
			nEnable			-是否启用OSD
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetSecondVideoEnableOSD(const char*cszCallID,int nEnable);

	/******************************************************************************
	* SetSecondVideoOSDText
	*描述：设置辅流视频OSD文字
	*输入：	cszCallID		-呼叫标识
			cszText			-OSD文字
			nAlign			-对齐方式
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetSecondVideoOSDText(const char*cszCallID,const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetSecondVideoOSDFont
	*描述：设置辅流视频OSD字体,字的颜色和背景色
	*输入：	cszCallID		-呼叫标识
			logfont			-OSD字体
			crText			-字的颜色
			crBk			-背景色
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetSecondVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

	/******************************************************************************
	* GetSecondVideoInfo
	*描述：获取辅流视频信息
	*输入：	cszCallID		-呼叫标识
	*输出：	nWidth			-图像宽度
			nHeight			-图像高度
			nVideoCodecType	-编码类型
			chInfo          -通道打开状态 帧率
			hVideoWnd		-窗口句柄
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);
	
	/******************************************************************************
	* GetSecondVideoPacketStatistics
	*描述：获取辅流视频的包的统计数据
	*输入：	cszCallID			-呼叫标识
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetLocalAudioLevel
	*描述：获取本地音频Level【实际声音大小】
	*输入：	cszCallID		-呼叫标识
	*输出：	nLevel			-实际声音大小【0-10】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetLocalAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetLocalAudioVolume
	*描述：获取本地音频音量
	*输入：	cszCallID		-呼叫标识
			nVolume			-音量
	*输出：	无
	*返回值：无
	*****************************************************************************/
	static void SetLocalAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetLocalAudioMute
	*描述：获取本地音频是否静音
	*输入：	cszCallID		-呼叫标识
			nMute			-是否静音
	*输出：	无
	*返回值：无
	*****************************************************************************/
	static void SetLocalAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetLocalAudioCodecID
	*描述：获取本地音频编码信息
	*输入：	cszCallID		-呼叫标识
	*输出：	nAudioCodecID	-编码ID
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetLocalAudioPacketStatistics
	*描述：获取本机音频的包的统计数据
	*输入：	cszCallID			-呼叫标识
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetRemoteAudioLevel
	*描述：获取远程音频Level【实际声音大小】
	*输入：	cszCallID		-呼叫标识
	*输出：	nLevel			-实际声音大小【0-10】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetRemoteAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetRemoteAudioVolume
	*描述：获取远程音频音量
	*输入：	cszCallID		-呼叫标识
			nVolume			-音量
	*输出：	无
	*返回值：无
	*****************************************************************************/
	static void SetRemoteAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetRemoteAudioMute
	*描述：获取远程音频是否静音
	*输入：	cszCallID		-呼叫标识
			nMute			-是否静音
	*输出：	无
	*返回值：无
	*****************************************************************************/
	static void SetRemoteAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetRemoteAudioCodecID
	*描述：获取远端音频编码
	*输入：	cszCallID		-呼叫标识
	*输出：	nAudioCodecID	-编码ID
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetRemoteAudioPacketStatistics
	*描述：获取远端音频的包的统计数据
	*输入：	cszCallID			-呼叫标识
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetCallType
	*描述：设置呼叫类型
	*输入：	nCallType		-呼叫类型
	*输出：	无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetCallType(IMXEC_CALL_TYPE nCallType);

	/******************************************************************************
	* GetCallType
	*描述：获取呼叫类型
	*输入：	无
	*输出：	nCallType		-呼叫类型
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetCallType(IMXEC_CALL_TYPE&nCallType);

	/******************************************************************************
	* GetCallSupportSecondVideo
	*描述：获取呼叫是否支持辅流【H239】
	*输入：	cszCallID		-呼叫标识
	*输出：	无
	*返回值：支持返回非0，不支持返回0
	*****************************************************************************/
	static int GetCallSupportSecondVideo(const char*cszCallID);

	/******************************************************************************
	* StartRecordLocalMainToAVI
	*描述：开始本地主流录像
	*输入：	cszCallID			-呼叫标识
			cszPathName			-录像文件名称
			nStandardFormat		-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int StartRecordLocalMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordLocalMainToAVI
	*描述：停止本地主流录像
	*输入：cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void StopRecordLocalMainToAVI(const char*cszCallID);

	/******************************************************************************
	* StartRecordRemoteMainToAVI
	*描述：开始远端主流录像
	*输入：	cszCallID			-呼叫标识
			cszPathName			-录像文件名称
			nStandardFormat		-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int StartRecordRemoteMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordRemoteMainToAVI
	*描述：停止远端主流录像
	*输入：cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void StopRecordRemoteMainToAVI(const char*cszCallID);

	/******************************************************************************
	* StartRecordSecondToAVI
	*描述：开始辅流录像
	*输入：	cszCallID			-呼叫标识
			cszPathName			-录像文件名称
			nStandardFormat		-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int StartRecordSecondToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordSecondToAVI
	*描述：停止辅流录像
	*输入：cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void StopRecordSecondToAVI(const char*cszCallID);

	/******************************************************************************
	* GetMainAudioID
	*描述：获取主流音频标识
	*输入： cszCallID		-呼叫标识
	*输出： nMainAudioID	-主流音频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID);

	/******************************************************************************
	* GetMainVideoID
	*描述：获取主流视频标识
	*输入： cszCallID		-呼叫标识
	*输出： nMainVideoID	-主流视频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID);

	/******************************************************************************
	* GetSecondVideoID
	*描述：获取辅流视频标识
	*输入： cszCallID		-呼叫标识
	*输出： nSecondVideoID	-辅流视频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID);

	/******************************************************************************
	* SetMainVideoScreenIndex
	*描述：设置主流视频图像桌面序号
	*输入： cszCallID		-呼叫标识
			nScreenIndex	-桌面序号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenIndex
	*描述：设置辅流视频图像桌面序号
	*输入： cszCallID		-呼叫标识
			nScreenIndex	-桌面序号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenWnd
	*描述：设置辅流视频所在屏幕窗口位置
	*输入：	cszCallID		-呼叫标识
			nScreenIndex	-屏幕号
			nWndIndex		-窗口号
			nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetRemoteVideoScreenWnd
	*描述：设置远端视频所在屏幕窗口位置
	*输入：	cszCallID		-呼叫标识
			nScreenIndex	-屏幕号
			nWndIndex		-窗口号
			nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetLocalRecordCallback
	*描述：设置本地录像回调
	*输入：	cszCallID		-呼叫标识
			pRecordCallback	-录像回调
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetLocalRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetRemoteRecordCallback
	*描述：设置远程录像回调
	*输入：	cszCallID		-呼叫标识
			pRecordCallback	-录像回调
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetRemoteRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetSecondRecordCallback
	*描述：设置辅流录像回调
	*输入：	cszCallID		-呼叫标识
			pRecordCallback	-录像回调
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetSecondRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SaveLocalMainVideoToBMPFile
	*描述：保存本地主流视频到BMP图片文件
	*输入：	cszCallID		-呼叫标识
			cszPathName		-图片文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SaveLocalMainVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SaveRemoteMainVideoToBMPFile
	*描述：保存远程主流视频到BMP图片文件
	*输入：	cszCallID		-呼叫标识
			cszPathName		-图片文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SaveRemoteMainVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SaveSecondVideoToBMPFile
	*描述：保存辅流视频到BMP图片文件
	*输入：	cszCallID		-呼叫标识
			cszPathName		-图片文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SaveSecondVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SendDTMF
	*描述：发送DTMF
	*输入：	cszCallID		-呼叫标识
			nDTMF			-DTMF
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SendDTMF(const char*cszCallID,const char nDTMF);

	/******************************************************************************
	* StartMixingWithWaveFile
	*描述：开始对呼叫进行WAVE文件混音，对方会听到WAVE文件播放的声音
	*输入：	cszCallID		-呼叫标识
			cszPathName		-WAVE文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int StartMixingWithWaveFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* StopMixingWithWaveFile
	*描述：停止对呼叫进行WAVE文件混音
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int StopMixingWithWaveFile(const char*cszCallID);

	/******************************************************************************
	* SetAudioOnly
	*描述：设置是否只支持音频
	*输入：	nAudioOnly		-是否只支持音频【0：支持视频音频，非0：只支持音频】
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetAudioOnly(int nAudioOnly);

	/******************************************************************************
	* GetCallProtocol
	*描述：获取呼叫协议
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回呼叫协议，失败返回空串""
	*****************************************************************************/
	static const char*GetCallProtocol(const char*cszCallID);

	/******************************************************************************
	* GetHasVideo
	*描述：获取是否只有视频
	*输入：cszCallID		-呼叫标识
	*输出：无
	*返回值：如果有视频，那么返回非0，否则返回0
	*****************************************************************************/
	static int GetHasVideo(const char*cszCallID);

	/******************************************************************************
	* SetEnableVideoTranscoding
	*描述：设置是否启用视频转码
	*输入：nEnable		-是否启用视频转码【0：不启用视频转码，非0：启用视频转码】
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetEnableVideoTranscoding(int nEnable);

	/******************************************************************************
	* GetEnableVideoTranscoding
	*描述：获取是否启用视频转码
	*输入：无
	*输出：无
	*返回值：如果启用视频转码，那么返回非0，否则返回0
	*****************************************************************************/
	static int GetEnableVideoTranscoding(void);

	/******************************************************************************
	* GetSecondVideoDirection
	*描述：获取辅流的方向
	*输入：	cszCallID		-呼叫标识
	*输出： nDir			-辅流的方向
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir);




	//H235 Media use

	/******************************************************************************
	* GetH235AuthCheckState
	*描述：获取H235验证检查状态 用于取呼叫验证及为网守时作验证检查
	*输入：	cszCallID		  -呼叫标识
	*输出： nState  -验证检查状态
			nLength -返回验证失败原因字符串长度
	*返回值：成功返回验证失败原因，失败返回空
	*****************************************************************************/
	const char* GetH235AuthCheckState(const char*cszCallID,IMXEC_CALL_H235AUTH_STATE &nState,int &nLength);

	/******************************************************************************
	* OnChannelRecvMediaEncryption
	*描述：接收媒体通道已加密状态通知，当打开加密的接收媒体通道时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
	nMediaType		-媒体类型
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnChannelRecvMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType){};

	/******************************************************************************
	* OnChannelSendMediaEncryption
	*描述：发送媒体通道已加密状态通知，当打开加密的发送媒体通道时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
	nMediaType		-媒体类型
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnChannelSendMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType){};

	/******************************************************************************
	* SetExternalIP
	*描述：设置外网映射IP
	*输入：cszExternalIP		-外网映射IP
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetExternalIP(const char*cszExternalIP);

	/******************************************************************************
	* OnDAP_RecvCallH245ControlMessage
	*描述：接收到H245控制消息回调 包含flowControlIndication H239令牌相关 会控消息
	*输入：	cszCallID			-呼叫标识
			nMsgType			-控制消息类型
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_RecvCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen){};

	//added by jiangdingfeng 2013-09-25 增加I帧请求
	/******************************************************************************
	* RequestMainVideoKeyFrame
	*描述：请求主流I帧
	*输入：cszCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void RequestMainVideoKeyFrame(const char*cszCallID);
	/******************************************************************************
	* RequestSecondVideoKeyFrame
	*描述：请求辅流I帧
	*输入：cszCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void RequestSecondVideoKeyFrame(const char*cszCallID);
	/******************************************************************************
	* SetLocalMainVideoSource
	*描述：设置本地主视频源
	*输入：cszCallID		  -呼叫标识
	       nMainVideoDevID    -主流设备ID
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetLocalMainVideoSource(const char*cszCallID, int nMainVideoDevID);
	/******************************************************************************
	* SetLocalSecondVideoSource
	*描述：设置本地辅视频源
	*输入：cszCallID		  -呼叫标识
		   nSecondVideoDevID  -辅流设备ID
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetLocalSecondVideoSource(const char*cszCallID, int nSecondVideoDevID);

	//add use to send private msg
	/******************************************************************************
	*描述:	向对端发送自定义非标消息
	*输入:	cszCallID		-呼叫标识
			cszContent		-自定义非标准信息内容
			nContentLen		-自定义非标准信息长度
			cszMimeType		-自定义非标信息内容类型 如为XML 值设为application/xml,如为一般字符串设为application/message+text 
			如为控制字符串设为application/message+xml
	*****************************************************************************/
	static int SendCallCustomMsg(const char*cszCallID,const char * cszContent,int nContentLen,const char * cszMimeType);

	/******************************************************************************
	* OnDAP_CallRecvNonstandardData
	*描述：接收到有终端发来非标消息时系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszPacket		-非标准信息负载
			nPacketLen		-非标准信息长度
			cszMimeType		-目前仅SIP使用 信息内容类型 如application/xml
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_RecvNonstandardData(const char*cszCallID,const char * cszPacket,int nPacketLen,const char * cszMimeType){};

	/******************************************************************************
	* HotDisplayVideo
	*描述：视频显示支持显示器热插拔接口
	*输入：	cszCallID		    -呼叫标识
			hWndLocal			-本地主视频显示的窗口句柄 如没有请设置为NULL
			hWndRemote			-远程主视频显示的窗口句柄 如没有请设置为NULL
			hWndSecond         	-远程辅流视频显示的窗口句柄 如没有请设置为NULL
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int HotDisplayVideo(const char*cszCallID,HWND hWndLocal,HWND hWndRemote,HWND hWndSecond);

	/******************************************************************************
	* GetCallH323ID
	*描述：获取H323ID
	*输入：	cszCallID		-呼叫标识
            szH323ID        -H323ID Buf
			nMaxLen         -Buf 最大长度
	*输出：	无
	*返回值：获取到返回H323ID长度，否则返回NULL
	*****************************************************************************/
	static int GetCallH323ID(const char*cszCallID ,char * szH323ID,int nMaxLen);
	/******************************************************************************
	* GetCallE164ID
	*描述：获取E164ID
	*输入：	cszCallID		-呼叫标识
            szE164ID        -E164ID Buf
			nMaxLen         -Buf 最大长度
	*输出：	无
	*返回值：获取到返回E164ID长度，否则返回NULL
	*****************************************************************************/
	static int GetCallE164ID(const char*cszCallID ,char * szE164ID,int nMaxLen);



	//Add U6 U7 use
	/******************************************************************************
	* OnDAP_RecvRemoteAudioRTPPacket
	*描述：接收到音频媒体流回调
	*输入：	cszCallID			-呼叫标识
	        codecID             -音频编码类型
			pRTPPacketData		-RTP包
			nRTPPacketLen		-RTP包长度
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP包头长度
			usSequence			-RTP包次序
			ulTimestamp			-RTP包时间戳
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteAudioRTPPacket(const char*cszCallID,X_AUDIO_CODEC_ID codecID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp) {};

	/******************************************************************************
	* OnDAP_RecvRemoteMainVideoRTPPacket
	*描述：接收到主流视频媒体流回调
	*输入：	cszCallID			-呼叫标识
	        codecType           -视频编码类型
			pRTPPacketData		-RTP包
			nRTPPacketLen		-RTP包长度
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP包头长度
			usSequence			-RTP包次序
			ulTimestamp			-RTP包时间戳
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteMainVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp) {};
	
	/******************************************************************************
	* OnDAP_RecvRemoteSecondVideoRTPPacket
	*描述：接收到辅流视频媒体流回调
	*输入：	cszCallID			-呼叫标识
	        codecType           -视频编码类型
			pRTPPacketData		-RTP包
			nRTPPacketLen		-RTP包长度
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP包头长度
			usSequence			-RTP包次序
			ulTimestamp			-RTP包时间戳
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteSecondVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp) {};



	/******************************************************************************
	* OnPeerEntityInfo
	*描述：返回对端实体信息
	*输入：	pCallParam			-CallParamSet
	*输出：无
	*返回值：无
	*****************************************************************************/
    virtual void OnPeerEntityInfo(const CallParamSet * pCallParam) {};


	/******************************************************************************
	* SetNetecTransEnable
	*描述：是否使用NETEC私有传流
	*输入：	cszCallID			-呼叫标识
	        nEnable             -是否使用,1为使用,0为不使用
	*输出：无
	*返回值：无
	*****************************************************************************/
    static void SetNetecTransEnable(const char*cszCallID, int nEnable);

	//add by Jan for DTMF [20140730]
	/******************************************************************************
	* OnIMXEC_CallRecvdDTMF
	*描述：接收到DTMF事件，当对方发送DTMF控制信令时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			szDtmf			-DTMF信令
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallRecvdDTMF(const char*cszCallID,char szDtmf) =0;
	//end [20140730]

	/******************************************************************************
	* SetResolutionStand
	*描述：设置终端支持的最大分辨率能力
	*输入：	nResStand			-最大分辨率能力
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SetResolutionStand(ResStandard nResStand);




	/******************************************************************************
	* OnDAP_CallAudioRecvChannelConnected
	*描述：音频接收通道连接成功事件
	*输入：	cszCallID			-呼叫标识
			AudioCodecID		-音频CodecID
			nPayloadType		-PayloadType
			ip					-本端接收rtp ip
			port				-本端接收rtp port
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallAudioRecvChannelConnected(const char*cszCallID,X_AUDIO_CODEC_ID AudioCodecID
		,int nPayloadType,unsigned long ip, int port) {};

	/******************************************************************************
	* OnDAP_CallAudioRecvChannelConnected
	*描述：音频接收通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallAudioRecvChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallMainVideoRecvChannelConnected
	*描述：主流视频接收通道连接成功事件
	*输入：	cszCallID			-呼叫标识
			codecType			-视频CodecID
			nBitrate			-码率
			nPayloadType		-PayloadType
			width				-分辨率宽度像素数 可选
			nHeight				-分辨率高度像素数 可选
			ip					-本端接收rtp ip
			port				-本端接收rtp port
			nLevel				-分辨率实际H264level值 可选
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoRecvChannelConnected(const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth=0,int nHeight=0,*/int nLevel=0) {};

	/******************************************************************************
	* OnDAP_CallMainVideoRecvChannelDestroyed
	*描述：主流视频接收通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoRecvChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallSecondVideoRecvChannelConnected
	*描述：辅流视频接收通道连接成功事件
	*输入：	cszCallID			-呼叫标识
			codecType			-视频CodecID
			nBitrate			-码率
			nPayloadType		-PayloadType
			width				-分辨率宽度像素数 可选
			nHeight				-分辨率高度像素数 可选
			ip					-本端接收rtp ip
			port				-本端接收rtp port
			nLevel				-分辨率实际H264level值 可选
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoRecvChannelConnected(const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth=0,int nHeight=0,*/int nLevel=0) {};

	/******************************************************************************
	* OnDAP_CallSecondVideoRecvChannelDestroyed
	*描述：辅流视频接收通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoRecvChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallFECCRecvChannelConnected
	*描述：FECC控制接收通道连接成功事件
	*输入：	cszCallID			-呼叫标识
			nPayloadType		-PayloadType
			ip					-本端接收rtp ip
			port				-本端接收rtp port
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallFECCRecvChannelConnected(const char*cszCallID ,int nPayloadType,unsigned long ip
		, int port) {};

	/******************************************************************************
	* OnDAP_CallFECCRecvChannelDestroyed
	*描述：FECC控制接收通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallFECCRecvChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallAudioSendChannelConnected
	*描述：音频发送通道连接成功事件
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			AudioCodecID		-音频CodecID
			nPayloadType		-PayloadType
			ip					-对端rtp ip
			port				-对端rtp port
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallAudioSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,X_AUDIO_CODEC_ID AudioCodecID
		,int nPayloadType,unsigned long ip, int port) {};

	/******************************************************************************
	* OnDAP_CallAudioSendChannelDestroyed
	*描述：音频发送通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallAudioSendChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallMainVideoSendChannelConnected
	*描述：主流视频发送通道连接成功事件
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			codecType			-视频CodecID
			nBitrate			-码率
			nPayloadType		-PayloadType
			width				-分辨率宽度像素数 可选
			nHeight				-分辨率高度像素数 可选
			nLevel				-分辨率实际H264level值 可选
			ip					-对端rtp ip
			port				-对端rtp port
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip,int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth,int nHeight,*/int nLevel = 0) {};

	/******************************************************************************
	* OnDAP_CallMainVideoSendChannelDestroyed
	*描述：主流视频发送通道连接成功事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoSendChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallSecondVideoSendChannelConnected
	*描述：辅流视频发送通道连接成功事件
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			codecType			-视频CodecID
			nBitrate			-码率
			nPayloadType		-PayloadType
			width				-分辨率宽度像素数 可选
			nHeight				-分辨率高度像素数 可选
			nLevel				-分辨率实际H264level值 可选
			ip					-对端rtp ip
			port				-对端rtp port
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth,int nHeight,*/int nLevel = 0) {};
	
	/******************************************************************************
	* OnDAP_CallSecondVideoSendChannelDestroyed
	*描述：辅流视频发送通道连接成功事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoSendChannelDestroyed(const char*cszCallID) {};

	/******************************************************************************
	* OnDAP_CallFECCSendChannelConnected
	*描述：FECC控制发送通道连接成功事件
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			ip					-对端rtp ip
			port				-对端rtp port
			nPayloadType		-PayloadType
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallFECCSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID,int nPayloadType
		,unsigned long ip, int port) {};

	/******************************************************************************
	* OnDAP_CallFECCSendChannelDestroyed
	*描述：FECC控制发送通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallFECCSendChannelDestroyed(const char*cszCallID) {};


#ifdef _TIANZHU_CACHE_
	/******************************************************************************
	* SetRemoteVideoCache
	*描述：设置远端视播放的缓冲帧数,只支持IMX呼叫类型
	*输入：	cszCallID			-呼叫标识
	        nCacheFrameCount	-缓冲的帧数（0 - 60）
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetRemoteVideoCache(const char*cszCallID, int nCacheFrameCount);
#endif

};



#endif