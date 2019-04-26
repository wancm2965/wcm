//
IMXECAddressBook;


使用IAX2做点到点的视音频呼叫

使用IAX2做电话呼叫


IMXECPhone
{

};


class IMXEC_Network
{
	SetServerIP;
	SetServerPort;
	
	virtual void OnIMXEC_NetworkConnected(void)=0;
	virtual void OnIMXEC_NetworkDisconnected(void)=0;
};

class IMXECUser
{
	SetUserID;
	SetPassword;
	
	Start(void);
	Stop(void);
	
	virtual void OnIMXECUserLoginSuccessfully(void)=0;
	virtual void OnIMXECUserKickout(void)=0;
	virtual void OnIMXECUserErrorID(void)=0;
	virtual void OnIMXECUserErrorPassword(void)=0;
};

IMXECAddressBook;

class IMXECMsg
{
public:
	virtual int SendMsg(const char*cszUserID,const char*cszMsg);
	virtual void OnIMXECMsgRecvdMsg(const char*cszUserID,const char*cszMsg);
};


class IMSClient
{

};


class Call
{
public:

protected:
	IMXEC_Play*	m_pPlay;
};

class CallMgr
{
public:

protected:
	IMXEC_CapDev*	m_pCapDev;
};



class ConferenceCall
{
public:
	
};


class IAXCallOut
{
public:
	IAXCallOut(const std::string&strUserID,const std::string&strPassword,const std::string&strHost,unsigned short nHostPort,const std::string&strPeerNumber);
	virtual~IAXCallOut(void);
public:
	virtual int Open(void);
	virtual void Close(void);
	//Session;GetSession
};


//INVITE
	//Number
	//CallID
						//RING
							//CALLID
							//AUDID
							//CODEC
						//OK
							//CALLID
//BYE
	//CallID
	

typedef std::map<struct iax_session*,IAXCallOut*>	MapIAXCallOut;


class IMXEC_CallOutCallback
{
public:
	IMXEC_CallOutCallback(void){};
	virtual~IMXEC_CallOutCallback(void){};
public:
	virtual void OnIMXEC_CallOutCallbackRingOut(const char*cszCallID,const char*cszUserID,const char*cszUserName){};
	virtual void OnIMXEC_CallOutCallbackEstablished(const char*cszCallID,const char*cszUserID,const char*cszUserName){};
	virtual void OnIMXEC_CallOutCallbackDestroyed(const char*cszCallID,const char*cszUserID,const char*cszUserName,const char*cszReason){};
};

class IMXEC_CallOut
{
public:
	IMXEC_CallOut(void);
	virtual~IMXEC_CallOut(void);
public:
	virtual int Open(void)=0;
	virtual void Close(void)=0;
	
	virtual const char*GetCallID(void)=0;
	virtual const char*GetUserID(void)=0;
	virtual const char*GetUserFullName(void)=0;
	
	virtual unsigned int GetDuration(void)=0;
	
	virtual int SendDTMF(char cDigit)=0;
	
	virtual int GetLocalAudioCodecInfo(void);
	
	virtual int GetLocalAudioLevel(void)=0; 
	
	virtual int GetRemoteAudioCodecInfo(void);
	virtual int GetRemoteAudioLevel(void)=0;
	virtual int GetRemoteAudioVolume(void)=0;
	virtual void SetRemoteAudioVolume(int nVolume)=0;
	
	/******************************************************************************
	* GetSendAudioNetworkStatus
	*描述：获取发送音频的网络状态
	*输入：	无
	*输出： nNetworkStatus	-网络状态
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetSendAudioNetworkStatus(unsigned long&nNetworkStatus)=0;
	
	/******************************************************************************
	* GetRecvAudioNetworkStatus
	*描述：获取接收音频的网络状态
	*输入：	无
	*输出： nNetworkStatus	-网络状态
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetRecvAudioNetworkStatus(unsigned long&nNetworkStatus)=0;
	
	/******************************************************************************
	* GetSendAudioPacketStatistics
	*描述：获取发送音频的包的统计数据
	*输入：	无
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetSendAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;
	
	/******************************************************************************
	* GetRecvAudioPacketStatistics
	*描述：获取接收音频的包的统计数据
	*输入：	无
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetRecvAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;
	
	static IMXEC_CallOut*Create(IMXEC_CallOutCallback&rCallback,const char*cszUserID);
};


class SubClass
{
public:
	SubClass(void){};
	virtual~SubClass(void){};
public:
	virtual int Open(void)=0;
	virtual void Close(void)=0;
	virtual void OnRecvdData(const char*pData,int nLen){};
};

class CallSubclass
	: public SubClass
{
public:
	CallSubclass(void);
	virtual~CallSubclass(void);
public:
	virtual void OnRecvdData(const char*pData,int nLen);
};



IMXEC_CallOut;

IMXEC_CallIn;


CallOut;

class CallOutMgr
{

	//AudioCap;//大家共用-》
};


class IMXEC_CallOutCallback
{

};

class IMXEC_CallOut
{

};



//用MCU去接收，然后用本地Sink接过来


class IMXEC_PDFFile
{
public:
	IMXEC_PDFFile(void){};
	virtual~IMXEC_PDFFile(void){};
public:
	virtual int Open(const char*cszPathName)=0;
	virtual void Close(void)=0;
	virtual int GetPageCount(void)=0;
	virtual int GetPageSize(int&nWidth,int&nHeight)=0;
	virtual int DrawPage(HDC hDC,int nPageIndex,int x,int y,int cx,int cy)=0;
	
	static IMXEC_PDFFile*Create(void);
};



CapChanRTP;

DevCapChanRTP;
ExtCapChanRTP;



class IMXEC_ScreenWnd
{
public:
	ScreenWnd(void){};
	virtual~ScreenWnd(void){};
public:
	virtual int Open(void)=0;
	virtual void Close(void)=0;
	virtual int SetScreenIndex(int nScreenIndex)=0;
	virtual int GetScreenIndex(void)=0;
	virtual int SetVisible(int nVisible)=0;
	virtual int GetVisible(void)=0;
	virtual int SetFullScreen(int nFullScreen)=0;
	virtual int GetFullScreen(void)=0;
	virtual int GetImage(void)=0;
	virtual int GetSamples(void)=0;
	
	static int SetScreenSize(int nScreenIndex,int nWidth,int nHeight);
};


class VIDEC_MP
	: public VIDEC_ImageSink
{
public:
	VIDEC_MP(void);
	virtual~VIDEC_MP(void);
public:
	virtual int Open(void);
	virtual void Close(void);
	virtual int GetWndIndex(void)=0;
	virtual int GetScreenIndex(void)=0;

	virtual void OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource)=0;
	virtual void OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource)=0;
	virtual void OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame=false,int nWidth=0,int nHeight=0,int nHeaderLen=0)=0;
	virtual void OnReceivedSubStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame=false,int nWidth=0,int nHeight=0,int nHeaderLen=0){};
	virtual void OnReceivedAudioStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen){};

protected:
	VIDEC_StreamSource*	m_pStreamSource;
};



class AudioCodecs;

class VideoCodec
{
public:
	VideoCodec(void){};
	virtual~VideoCodec(void){};
public:
	virtual int GetWidth(void)=0;
	virtual int GetHeight(void)=0;
	virtual VIDEC_CODEC_TYPE GetCodecType(void)=0;
	virtual int GetPayloadType(void)=0;
	virtual const char*GetCodecName(void)=0;
	virtual const char*GetParameter(void)=0;
};

class H263QCIF
	: public VideoCodec
{
};

class H263CIF
{
};

class H2634CIF
{
};

class VideoCodecs
{
public:
	EnableH263CIF(void);
	DisableH263CIF(void);
	GetEnableH263CIF(void);
};


class NETEC_Call
{
public:
	NETEC_Call(void);
	virtual~NETEC_Call(void){};
public:
	virtual int Start(void);
	virtual void Stop(void);
	
	virtual const char* MakeCall(const char*cszPeerUserID,const char*cszCallType,const char*cszLocalSDP);
	virtual int HangupCall(const char*cszCallID);
	virtual int AcceptCall(const char*cszCallID,const char*cszLocalSDP);
	
	virtual void OnNETEC_CallRingOut(const char*cszPeerUserID,const char*cszPeerUserName,const char*cszCallID,const char*cszCallType)=0;
	virtual void OnNETEC_CallRingIn(const char*cszPeerUserID,const char*cszPeerUserName,const char*cszPeerSDP,const char*cszCallID,const char*cszCallType)=0;
	virtual void OnNETEC_CallEstablished(const char*cszPeerUserID,const char*cszPeerUserName,const char*cszPeerSDP,const char*cszCallID,const char*cszCallType)=0;
	virtual void OnNETEC_CallDestroyed(const char*cszPeerUserID,const char*cszPeerUserName,const char*cszCallID,const char*cszCallType)=0;
};


//是否是MP模式
//在MP模式下永远不显示图像，
//有屏号和窗号的就开始解码和显示
//如果一个屏上只有一个图像，并且这路图像是要转播的【来自H323终端的】，并且大小和码流和编码都是一样的，那么不解码，不压缩，直接发送


//720P压缩一个码流（H264）
//4CIF压缩一个码流（H264+H263）
//CIF压缩一路码流（H264+H263）


尽量共用相同的压缩码流【不要重复压缩】

编码+码流+分辨率==唯一关键字

要设置MP的编码，码流，分辨率


FEC使用SVC：
压缩30帧，取一半帧率，然后