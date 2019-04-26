//IMXEC_SIP.h
#ifndef __IMXEC_SIP_H__
#define __IMXEC_SIP_H__

#include <IMXEC/IMXEC_Export.h>
#include <IMXEC/IMXEC_VideoSetting.h>
#include <IMXEC/IMXEC_AudioSetting.h>


//SIP协议
class IMXEC_API IMXEC_SIP
{
public:
	IMXEC_SIP(void);
	virtual~IMXEC_SIP(void);
public:
	//SIP信令通道传输类型
	typedef enum
	{
		TRANS_UDP=0,				//UDP
		TRANS_TCP,					//TCP
		TRANS_TLS					//TLS
	}TRANS_TYPE;

public:
	/******************************************************************************
	* AddVirtualLine
	*描述：添加虚拟线路
	*输入：	cszUserID		-用户帐号
			cszUserName		-用户名称
			cszPassword		-用户密码
			cszHost			-SIP代理服务器
	*输出：无
	*返回值：成功返回>=0[line id]，失败返回-1
	*****************************************************************************/
	static int AddVirtualLine(const char*cszUserID,const char*cszUserName,const char*cszPassword,const char*cszSIPProxy,const char*cszAuthUserID=NULL,const char*cszDomain=NULL);

	/******************************************************************************
	* RemoveVirtualLine
	*描述：删除虚拟线路
	*输入：	nLineID			-线路标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void RemoveVirtualLine(int nLineID);

	static int SetCurrentVirtualLine(int nLineID);
	static int GetCurrentVirtualLine(void);
	static int IsVirtualLineRegistrationSuccess(int nLineID);
	static int IsCurrentVirtualLineRegistrationSuccess(void);
	static int IsVirtualLineRegistrationError(int nLineID);
	static int IsCurrentVirtualLineRegistrationError(void);
	static const char*GetVirtualLineRegistrationErrorCode(int nLineID);
	static const char*GetCurrentVirtualLineRegistrationErrorCode(void);


	/******************************************************************************
	* SetSIPListenPort
	*描述：设置侦听端口
	*输入：	nSIPListenPort			-侦听端口
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetSIPListenPort(unsigned short nSIPListenPort);

	/******************************************************************************
	* SetLocalIP
	*描述：设置注册帐号
	*输入：	cszSIPID		-注册帐号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetLocalIP(const char*cszLocalIP);

	/******************************************************************************
	* SetRTPBasePort
	*描述：设置侦听端口
	*输入：	nRTPBasePort			-RTP开始端口
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetRTPBasePort(unsigned short nRTPBasePort);

	/******************************************************************************
	* SetAudioDevParam
	*描述：设置主流视频设备参数
	*输入： nAudioCodecID		-音频编码
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID);

	/******************************************************************************
	* SetMainVideoDevParam
	*描述：设置主流视频设备参数
	*输入： nDevID				-设备标识
			nFrameRate			-帧率
			nBitrate			-码流
			nVideoCodecType		-视频编码
			nVideoFormatType	-视频格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType);

	/******************************************************************************
	* Start
	*描述：启动SIP服务
	*输入：无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int Start(void);

	/******************************************************************************
	* Stop
	*描述：停止SIP服务
	*输入：无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int Stop(void);


	/******************************************************************************
	* SetSupportH239
	*描述：设置是否支持H239
	*输入：nFlag		-是否支持H239
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetSupportH239(int nSupportH239);

	/******************************************************************************
	* SetSecondVideoDevParam
	*描述：设置辅流视频设备参数
	*输入： nDevID				-设备标识
			nFrameRate			-帧率
			nBitrate			-码流
			nVideoCodecType		-视频编码
			nVideoFormatType	-视频格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType);

	/******************************************************************************
	* SetSIPTransportType
	*描述：设置SIP信令传输类型
	*输入：type			-类型
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetSIPTransportType(IMXEC_SIP::TRANS_TYPE type);

	/******************************************************************************
	* SetICETransportCount
	*描述：设置创建的每种媒体流ICE通道数
	*输入：	nCount		-每种媒体流ICE通道数(即如只有RTP则设为1，如还需要有RTCP则设为2)
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetICETransportCount(int nCount);
	/******************************************************************************
	* SetICEEnable
	*描述：设置ICE应用状态
	*输入：	bEnable		-ICE应用状态 true使用，false不使用，默认false
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetICEEnable(bool bEnable);
	/******************************************************************************
	* SetTURNLoginAuth
	*描述：设置登录TURN服务器账户
	*输入：	cszTurnUser		-TURN服务器账户名
	cszTurnPWD		-TURN服务器账户密码
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetTURNLoginAuth(const char*cszTurnUser,const char*cszTurnPWD);
	/******************************************************************************
	* SetTURNServerIPPort
	*描述：设置TURN服务器地址,也可设为域名地址
	*输入：	cszTurnIPPort		-TURN服务器地址 形如IP:Port
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetTURNServerIPPort(const char*cszTurnIPPort);
	/******************************************************************************
	* SetSTUNServerIPPort
	*描述：设置STUN服务器地址,也可设为域名地址
	*输入：	cszStunIPPort		-STUN服务器地址 形如IP:Port
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetSTUNServerIPPort(const char*cszStunIPPort);
	/******************************************************************************
	* SetTurnTransportType
	*描述：设置TURN传输通道类型
	*输入：	nType		-TURN传输通道类型 17 udp, 6 tcp, 255 tls
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetTurnTransportType(int nType);
	/******************************************************************************
	* SetDNSServerIP
	*描述：设置DNS服务器地址 如果不是有效DNS就不要设置
	*输入：	cszDNSIP		-设置DNS服务器地址 形如IP
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetDNSServerIP(const char*cszDNSIP);
	/******************************************************************************
	* SetDNSServerIP2
	*描述：设置DNS服务器地址2 如果不是有效DNS就不要设置
	*输入：	cszDNSIP2		-设置DNS服务器地址 形如IP
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetDNSServerIP2(const char*cszDNSIP2);

};




#endif
