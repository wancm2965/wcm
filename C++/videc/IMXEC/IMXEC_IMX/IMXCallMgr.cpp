#include "IMXCallMgr.h"
#include "CallMgr.h"
#include "IMXCall.h"	

static IMXCallMgr g_IMXCallMgr;

IMXCallMgr::IMXCallMgr(void)
:m_strUserID("")
,m_strUserName("")

,m_pMainCapChanDev(NULL)

,m_nMainVideoDevID(0)
,m_nMainVideoFrameRate(15)
,m_nMainVideoBitrate(320)
,m_nMainVideoCodecType(VIDEC_CODEC_H264_SVC)
,m_nMainVideoFormatType(VIDEO_FORMAT_CIF)
,m_nAudioCodecID(X_AUDIO_CODEC_G711U)
{
	m_nIDEventTimer=0;

}

IMXCallMgr::~IMXCallMgr(void)
{

}

IMXCallMgr&IMXCallMgr::Instance(void)
{
	return g_IMXCallMgr;
}


VOID CALLBACK IMXCallMgrTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	IMXCallMgr::Instance().OnTimer(idEvent);
}

/******************************************************************************
* SetUserID
*描述：设置用户标识
*输入： cszUserID		-用户标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SetUserID(const char*cszUserID)
{
	if (cszUserID!=NULL)
	{
		m_strUserID=cszUserID;
	}
	else
	{
		m_strUserID="";
	}

	NETEC_Node::SetLocalUserID(m_strUserID.c_str());

	return 0;
}

/******************************************************************************
* SetUserName
*描述：设置用户名称
*输入： cszUserName		-用户名称
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SetUserName(const char*cszUserName)
{
	if (cszUserName!=NULL)
	{
		m_strUserName=cszUserName;
	}
	else
	{
		m_strUserName="";
	}

	NETEC_Node::SetLocalUserName(m_strUserName.c_str());

	return 0;
}

/******************************************************************************
* SetAudioDevParam
*描述：设置主流视频设备参数
*输入： nAudioCodecID		-音频编码
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID)
{
	m_nAudioCodecID=nAudioCodecID;
	return 0;
}

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
int IMXCallMgr::SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType)
{
	m_nMainVideoDevID=nDevID;
	m_nMainVideoFrameRate=nFrameRate;
	m_nMainVideoBitrate=nBitrate;
	m_nMainVideoCodecType=nVideoCodecType;
	m_nMainVideoFormatType=nVideoFormatType;
	return 0;
}

/******************************************************************************
* Start
*描述：启动SIP服务
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::Start(void)
{
	if (m_nIDEventTimer==0)
	{
		m_nIDEventTimer=SetTimer(NULL,1,1,IMXCallMgrTimerProc);
		return NETEC_Call::Start();
	}

	return 0;
}

/******************************************************************************
* Stop
*描述：停止SIP服务
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::Stop(void)
{
	while (true)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.begin();
		if (iter!=m_MapIMXCall.end())
		{
			IMXCall*pIMXCall=iter->second;
			std::string strCallID=iter->first;

			m_MapIMXCall.erase(iter);

			HangupCall(strCallID.c_str());

			pIMXCall->Close();
			delete pIMXCall;
			pIMXCall=NULL;
		}
		else
		{
			break;
		}
	}

	if (m_pMainCapChanDev!=NULL)
	{
		m_pMainCapChanDev->Close();
		delete m_pMainCapChanDev;
		m_pMainCapChanDev=NULL;
	}

	if (m_nIDEventTimer!=0)
	{
		KillTimer(NULL,m_nIDEventTimer);
		m_nIDEventTimer=0;

		NETEC_Call::Stop();
	}
	while (true)
	{
		XDataBuffer*pXDataBuffer=GetDataBuffer();
		if (pXDataBuffer!=NULL)
		{
			delete pXDataBuffer;
			pXDataBuffer=NULL;
		}
		else
			break;
	}
}

/******************************************************************************
* MakeCall
*描述：发起呼叫
*输入：	cszUserID		-对方的用户ID或IP
*输出：无
*返回值：呼叫标识
*****************************************************************************/
const char*IMXCallMgr::MakeCall(const char*cszUserID)
{
	if (m_pMainCapChanDev==NULL)
	{
		m_pMainCapChanDev=IMXEC_CapChanDev::Create(*this);
		if (m_pMainCapChanDev!=NULL)
		{
			if (0!=m_pMainCapChanDev->Open(NULL,0))
			{
				m_pMainCapChanDev->Close();
				delete m_pMainCapChanDev;
				m_pMainCapChanDev=NULL;
			}
			else
			{
				m_pMainCapChanDev->OpenAudio(-1,m_nAudioCodecID);
			}
		}
	}

	std::string strLocalSDP="";
	if (m_pMainCapChanDev!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("AID",m_pMainCapChanDev->GetAudioID());
		tStrPacket.Set("VID",m_pMainCapChanDev->GetVideoID());
		tStrPacket.Set("ID",NETEC_Node::GetMCUID());
		tStrPacket.Set("IP",NETEC_Node::GetMCUIP());
		tStrPacket.Set("PORT",NETEC_Node::GetServerPort());

		tStrPacket.GetString(strLocalSDP);
	}
	
	return NETEC_Call::MakeCall(cszUserID,"AV",strLocalSDP.c_str());
}

/******************************************************************************
* AcceptCall
*描述：接受呼叫请求
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::AcceptCall(const char*cszCallID)
{
//限制同时只能有一个call在呼叫
#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) ) )
#else

	if(CallMgr::Instance().IsCanCall() ==false)
	{
		return -1;
	}
	CallMgr::Instance().SetCanCallNot(); 

#endif

	if (m_pMainCapChanDev==NULL)
	{
		m_pMainCapChanDev=IMXEC_CapChanDev::Create(*this);
		if (m_pMainCapChanDev!=NULL)
		{
			if (0!=m_pMainCapChanDev->Open(NULL,0))
			{
				m_pMainCapChanDev->Close();
				delete m_pMainCapChanDev;
				m_pMainCapChanDev=NULL;
			}
			else
			{
				m_pMainCapChanDev->OpenAudio(-1,m_nAudioCodecID);
			}
		}
	}

	std::string strLocalSDP="";
	if (m_pMainCapChanDev!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("AID",m_pMainCapChanDev->GetAudioID());
		tStrPacket.Set("VID",m_pMainCapChanDev->GetVideoID());
		tStrPacket.Set("ID",NETEC_Node::GetMCUID());
		tStrPacket.Set("IP",NETEC_Node::GetMCUIP());
		tStrPacket.Set("PORT",NETEC_Node::GetServerPort());

		tStrPacket.GetString(strLocalSDP);
	}

	return NETEC_Call::AcceptCall(cszCallID,strLocalSDP.c_str());
}

/******************************************************************************
* HangupCall
*描述：挂断呼叫
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::HangupCall(const char*cszCallID)
{
	return NETEC_Call::HangupCall(cszCallID);
}

/******************************************************************************
* SetLocalMainVideoWnd
*描述：设置本地主流视频显示的窗口
*输入：	hWnd			-本地主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SetLocalMainVideoWnd(HWND hWnd)
{
	if (m_pMainCapChanDev!=NULL)
	{
		return m_pMainCapChanDev->Open(hWnd,0);
	}
	return -1;
}

/******************************************************************************
* SetLocalMainVideoWndPos
*描述：设置本地主流视频显示的窗口位置
*输入：	x				-窗口的左边位置
		y				-窗口的上边位置
		cx				-窗口的宽度
		cy				-窗口的高度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SetLocalMainVideoWndPos(int x,int y,int cx,int cy)
{
	if (m_pMainCapChanDev!=NULL)
	{
		return m_pMainCapChanDev->SetWindowPos(NULL,x,y,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE)?0:-1;
	}
	return -1;
}

/******************************************************************************
* SetLocalMainVideoParam
*描述：设置本地主流视频参数
*输入：	nBrightness		-亮度值0-255
		nContrast		-对比度0-127
		nSaturation		-饱和度0-127
		nHue			-色调0-255
		nGamma			-伽马1-400
		nSharpen		-锐化0-127
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetLocalMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	if (m_pMainCapChanDev!=NULL)
	{
		m_pMainCapChanDev->SetVideoParam(nBrightness, nContrast, nSaturation, nHue,nGamma,nSharpen,bSharpenLocalOnly);
	}
}

/******************************************************************************
* SetLocalMainVideoEnableOSD
*描述：设置本地主流视频是否启用OSD
*输入：	cszCallID		-呼叫标识
		nEnable			-是否启用OSD
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetLocalMainVideoEnableOSD(int nEnable)
{
	if (m_pMainCapChanDev!=NULL)
	{
		m_pMainCapChanDev->EnableOSD(nEnable!=0);
	}
}

/******************************************************************************
* SetLocalMainVideoOSDText
*描述：设置本地主流视频OSD文字
*输入：	cszCallID		-呼叫标识
		cszText			-OSD文字
		nAlign			-对齐方式
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetLocalMainVideoOSDText(const char*cszText,int nAlign)//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT
{
	if (m_pMainCapChanDev!=NULL)
	{
		m_pMainCapChanDev->SetOSDText(cszText,nAlign);
	}
}

/******************************************************************************
* SetLocalMainVideoOSDFont
*描述：设置本地主流视频OSD字体,字的颜色和背景色
*输入：	logfont			-OSD字体
		crText			-字的颜色
		crBk			-背景色
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetLocalMainVideoOSDFont(LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	if (m_pMainCapChanDev!=NULL)
	{
		m_pMainCapChanDev->SetOSDFont(logfont,crText,crBk);
	}
}

/******************************************************************************
* GetLocalMainVideoInfo
*描述：获取本地主流视频信息
*输入：	无
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetLocalMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)
{
	if (m_pMainCapChanDev!=NULL)
	{
		nWidth=m_pMainCapChanDev->GetVideoWidth();
		nHeight=m_pMainCapChanDev->GetVideoHeight();
		hVideoWnd=m_pMainCapChanDev->GetVideoWnd();
		return 0;
	}
	return -1;
}

/******************************************************************************
* GetLocalMainVideoPacketStatistics
*描述：获取本地主视频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetLocalMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if (m_pMainCapChanDev!=NULL)
	{
		return m_pMainCapChanDev->GetVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return -1;
}

/******************************************************************************
* GetLocalAudioLevel
*描述：获取本地音频Level【实际声音大小】
*输入：	无
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetLocalAudioLevel(int&nLevel)
{
	nLevel=0;
	if (m_pMainCapChanDev!=NULL)
	{
		return m_pMainCapChanDev->GetLevel(nLevel);
	}
	return -1;
}

/******************************************************************************
* SetLocalAudioVolume
*描述：获取本地音频音量
*输入：	nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetLocalAudioVolume(int nVolume)
{
	if (m_pMainCapChanDev!=NULL)
	{
		m_pMainCapChanDev->SetVolume(nVolume);
	}
}

/******************************************************************************
* SetLocalAudioMute
*描述：获取本地音频是否静音
*输入：	nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetLocalAudioMute(int nMute)
{
	if (m_pMainCapChanDev!=NULL)
	{
		m_pMainCapChanDev->SetMute(nMute!=0);
	}
}

/******************************************************************************
* GetLocalAudioCodecID
*描述：获取本地音频编码信息
*输入：	无
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetLocalAudioCodecID(X_AUDIO_CODEC_ID &nAudioCodecID)
{
	if (m_pMainCapChanDev!=NULL)
	{
		return 0;
	}
	return -1;
}

/******************************************************************************
* GetLocalAudioPacketStatistics
*描述：获取本机音频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetLocalAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if (m_pMainCapChanDev!=NULL)
	{
		return m_pMainCapChanDev->GetAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return -1;
}


/******************************************************************************
* StartRecordLocalMainToAVI
*描述：开始本地主流录像
*输入：	cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::StartRecordLocalMainToAVI(const char*cszPathName,int nStandardFormat)
{
	if (m_pMainCapChanDev!=NULL)
	{
		return m_pMainCapChanDev->StartRecordToAVI(cszPathName,nStandardFormat);
	}
	return -1;
}

/******************************************************************************
* StopRecordLocalMainToAVI
*描述：停止本地主流录像
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::StopRecordLocalMainToAVI(void)
{
	if (m_pMainCapChanDev!=NULL)
	{
		m_pMainCapChanDev->StopRecordToAVI();
	}
}

/******************************************************************************
* SaveLocalMainVideoToBMPFile
*描述：保存本地主流视频到BMP图片文件
*输入：	cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SaveLocalMainVideoToBMPFile(const char*cszPathName)
{
	if (m_pMainCapChanDev!=NULL)
	{
		return m_pMainCapChanDev->CaptureToBMPFile(cszPathName)?0:-1;
	}

	return -1;
}


/******************************************************************************
* SendPTZ
*描述：发送PTZ【发送云台控制命令】
*输入：	cszCallID		-呼叫标识
		nPTZAction		-云台动作
		nSpeed			-动作的速度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			//return iter->second->SendPTZ(nPTZAction,nSpeed);
		}
	}
	return -1;
}

/******************************************************************************
* StartSecondVideo
*描述：开始辅流
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::StartSecondVideo (const char*cszCallID)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			//return iter->second->StartSecondVideo();
		}
	}
	return -1;
}

/******************************************************************************
* StopSecondVideo
*描述：停止辅流
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::StopSecondVideo (const char*cszCallID)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			//return iter->second->StopSecondVideo();
		}
	}
	return -1;
}

/******************************************************************************
* SetLocalMainVideoWnd
*描述：设置本地主流视频显示的窗口
*输入：	cszCallID		-呼叫标识
		hWnd			-本地主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SetLocalMainVideoWnd(const char*cszCallID,HWND hWnd)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->SetLocalMainVideoWnd(hWnd);
		}
	}
	return -1;
}

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
int IMXCallMgr::SetLocalMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->SetLocalMainVideoWndPos(x,y,cx,cy);
		}
	}
	return -1;
}

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
void IMXCallMgr::SetLocalMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetLocalMainVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
		}
	}
}

/******************************************************************************
* SetLocalMainVideoEnableOSD
*描述：设置本地主流视频是否启用OSD
*输入：	cszCallID		-呼叫标识
		nEnable			-是否启用OSD
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetLocalMainVideoEnableOSD(const char*cszCallID,int nEnable)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetLocalMainVideoEnableOSD(nEnable);
		}
	}
}

/******************************************************************************
* SetLocalMainVideoOSDText
*描述：设置本地主流视频OSD文字
*输入：	cszCallID		-呼叫标识
		cszText			-OSD文字
		nAlign			-对齐方式
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetLocalMainVideoOSDText(const char*cszCallID,const char*cszText,int nAlign)//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetLocalMainVideoOSDText(cszText,nAlign);
		}
	}
}

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
void IMXCallMgr::SetLocalMainVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetLocalMainVideoOSDFont(logfont,crText,crBk);
		}
	}
}

/******************************************************************************
* GetLocalMainVideoInfo
*描述：获取本地主流视频信息
*输入：	cszCallID		-呼叫标识
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->GetLocalMainVideoInfo(nWidth,nHeight,nVideoCodecType,hVideoWnd);
		}
	}

	return -1;
}

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
int IMXCallMgr::GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->GetLocalMainVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
	}

	return -1;
}

/******************************************************************************
* SetRemoteMainVideoWnd
*描述：设置远程主流视频显示的窗口
*输入：	cszCallID		-呼叫标识
		hWnd			-远程主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SetRemoteMainVideoWnd(const char*cszCallID,HWND hWnd)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->SetRemoteMainVideoWnd(hWnd);
		}
	}
	return -1;
}

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
int IMXCallMgr::SetRemoteMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->SetRemoteMainVideoWndPos(x,y,cx,cy);
		}
	}
	return -1;
}

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
void IMXCallMgr::SetRemoteMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetRemoteMainVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
		}
	}
}

/******************************************************************************
* GetRemoteMainVideoInfo
*描述：获取远程主流视频信息
*输入：	cszCallID		-呼叫标识
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->GetRemoteMainVideoInfo(nWidth,nHeight,nVideoCodecType,hVideoWnd);
		}
	}
	return -1;
}

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
int IMXCallMgr::GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->GetRemoteMainVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
	}
	return -1;
}

/******************************************************************************
* SetSecondVideoWnd
*描述：设置辅流视频显示的窗口
*输入：	cszCallID		-呼叫标识
		hWnd			-远程辅流视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SetSecondVideoWnd(const char*cszCallID,HWND hWnd)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->SetSecondVideoWnd(hWnd);
		}
	}
	return -1;
}

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
int IMXCallMgr::SetSecondVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->SetSecondVideoWndPos(x,y,cx,cy);
		}
	}
	return -1;
}

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
void IMXCallMgr::SetSecondVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetSecondVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
		}
	}
}

/******************************************************************************
* SetSecondVideoEnableOSD
*描述：设置辅流视频是否启用OSD
*输入：	cszCallID		-呼叫标识
		nEnable			-是否启用OSD
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetSecondVideoEnableOSD(const char*cszCallID,int nEnable)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetSecondVideoEnableOSD(nEnable);
		}
	}
}

/******************************************************************************
* SetSecondVideoOSDText
*描述：设置辅流视频OSD文字
*输入：	cszCallID		-呼叫标识
		cszText			-OSD文字
		nAlign			-对齐方式
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetSecondVideoOSDText(const char*cszCallID,const char*cszText,int nAlign)//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetSecondVideoOSDText(cszText,nAlign);
		}
	}
}

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
void IMXCallMgr::SetSecondVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetSecondVideoOSDFont(logfont,crText,crBk);
		}
	}
}

/******************************************************************************
* HotDisplayVideo
*描述：视频显示支持显示器热插拔接口
*输入：	cszCallID		    -呼叫标识
hWndLocal			-本地主视频显示的窗口句柄
hWndRemote			-远程主视频显示的窗口句柄
hWndSecond         	-远程辅流视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::HotDisplayVideo(const char*cszCallID,HWND hWndLocal,HWND hWndRemote,HWND hWndSecond)
{
	//notice the not definition
	return -1;
}

/******************************************************************************
* GetSecondVideoInfo
*描述：获取辅流视频信息
*输入：	cszCallID		-呼叫标识
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->GetSecondVideoInfo(nWidth,nHeight,nVideoCodecType,hVideoWnd);
		}
	}
	return -1;
}

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
int IMXCallMgr::GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->GetSecondVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
	}
	return -1;
}

/******************************************************************************
* GetLocalAudioLevel
*描述：获取本地音频Level【实际声音大小】
*输入：	cszCallID		-呼叫标识
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetLocalAudioLevel(const char*cszCallID,int&nLevel)
{
	nLevel=0;
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->GetLocalAudioLevel(nLevel);
		}
	}
	return -1;
}

/******************************************************************************
* SetLocalAudioVolume
*描述：获取本地音频音量
*输入：	cszCallID		-呼叫标识
		nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetLocalAudioVolume(const char*cszCallID,int nVolume)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetLocalAudioVolume(nVolume);
		}
	}
}

/******************************************************************************
* SetLocalAudioMute
*描述：获取本地音频是否静音
*输入：	cszCallID		-呼叫标识
		nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetLocalAudioMute(const char*cszCallID,int nMute)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetLocalAudioMute(nMute);
		}
	}
}

/******************************************************************************
* GetLocalAudioCodecID
*描述：获取本地音频编码信息
*输入：	cszCallID		-呼叫标识
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->GetLocalAudioCodecID(nAudioCodecID);
		}
	}
	return -1;
}

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
int IMXCallMgr::GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->GetLocalAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
		}
	}
	return -1;
}

/******************************************************************************
* GetRemoteAudioLevel
*描述：获取远程音频Level【实际声音大小】
*输入：	cszCallID		-呼叫标识
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetRemoteAudioLevel(const char*cszCallID,int&nLevel)
{
	nLevel=0;
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->GetRemoteAudioLevel(nLevel);
		}
	}
	return -1;
}

/******************************************************************************
* SetRemoteAudioVolume
*描述：获取远程音频音量
*输入：	cszCallID		-呼叫标识
		nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetRemoteAudioVolume(const char*cszCallID,int nVolume)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetRemoteAudioVolume(nVolume);
		}
	}
}

/******************************************************************************
* SetRemoteAudioMute
*描述：获取远程音频是否静音
*输入：	cszCallID		-呼叫标识
		nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetRemoteAudioMute(const char*cszCallID,int nMute)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			iter->second->SetRemoteAudioMute(nMute);
		}
	}
}

/******************************************************************************
* GetRemoteAudioCodecID
*描述：获取远端音频编码
*输入：	cszCallID		-呼叫标识
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->GetRemoteAudioCodecID(nAudioCodecID);
		}
	}
	return -1;
}

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
int IMXCallMgr::GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		return iter->second->GetRemoteAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return -1;
}

/******************************************************************************
* GetCallSupportSecondVideo
*描述：获取呼叫是否支持辅流【H239】
*输入：	cszCallID		-呼叫标识
*输出：	无
*返回值：支持返回非0，不支持返回0
*****************************************************************************/
int IMXCallMgr::GetCallSupportSecondVideo(const char*cszCallID)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			//return iter->second->GetCallSupportSecondVideo();
		}
	}
	return 0;
}

/******************************************************************************
* StartRecordLocalMainToAVI
*描述：开始本地主流录像
*输入：	cszCallID			-呼叫标识
		cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::StartRecordLocalMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		return iter->second->StartRecordLocalMainToAVI(cszPathName,nStandardFormat);
	}
	return -1;
}

/******************************************************************************
* StopRecordLocalMainToAVI
*描述：停止本地主流录像
*输入：cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::StopRecordLocalMainToAVI(const char*cszCallID)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		iter->second->StopRecordLocalMainToAVI();
	}
}

/******************************************************************************
* StartRecordRemoteMainToAVI
*描述：开始远端主流录像
*输入：	cszCallID			-呼叫标识
		cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::StartRecordRemoteMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		return iter->second->StartRecordRemoteMainToAVI(cszPathName,nStandardFormat);
	}
	return -1;
}

/******************************************************************************
* StopRecordRemoteMainToAVI
*描述：停止远端主流录像
*输入：cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::StopRecordRemoteMainToAVI(const char*cszCallID)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		iter->second->StopRecordRemoteMainToAVI();
	}
}

/******************************************************************************
* StartRecordSecondToAVI
*描述：开始辅流录像
*输入：	cszCallID			-呼叫标识
		cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::StartRecordSecondToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		return iter->second->StartRecordSecondToAVI(cszPathName,nStandardFormat);
	}
	return -1;
}

/******************************************************************************
* StopRecordSecondToAVI
*描述：停止辅流录像
*输入：cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::StopRecordSecondToAVI(const char*cszCallID)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		iter->second->StopRecordSecondToAVI();
	}
}

/******************************************************************************
* GetMainAudioID
*描述：获取主流音频标识
*输入： cszCallID		-呼叫标识
*输出： nMainAudioID	-主流音频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		return iter->second->GetMainAudioID(nMainAudioID);
	}
	return -1;
}

/******************************************************************************
* GetMainVideoID
*描述：获取主流视频标识
*输入： cszCallID		-呼叫标识
*输出： nMainVideoID	-主流视频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		return iter->second->GetMainVideoID(nMainVideoID);
	}
	return -1;
}

/******************************************************************************
* GetSecondVideoID
*描述：获取辅流视频标识
*输入： cszCallID		-呼叫标识
*输出： nSecondVideoID	-辅流视频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		return iter->second->GetSecondVideoID(nSecondVideoID);
	}
	return -1;
}

/******************************************************************************
* SetMainVideoScreenIndex
*描述：设置主流视频图像桌面序号
*输入： cszCallID		-呼叫标识
		nScreenIndex	-桌面序号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		return iter->second->SetMainVideoScreenIndex(nScreenIndex);
	}
	return -1;
}

/******************************************************************************
* SetSecondVideoScreenIndex
*描述：设置辅流视频图像桌面序号
*输入： cszCallID		-呼叫标识
		nScreenIndex	-桌面序号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		return iter->second->SetSecondVideoScreenIndex(nScreenIndex);
	}

	return -1;
}


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
void IMXCallMgr::SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		iter->second->SetSecondVideoScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
}

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
void IMXCallMgr::SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		iter->second->SetRemoteVideoScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
}

/******************************************************************************
* SetLocalRecordCallback
*描述：设置本地录像回调
*输入：	cszCallID		-呼叫标识
		pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetLocalRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		iter->second->SetLocalRecordCallback(pRecordCallback);
	}
}

/******************************************************************************
* SetRemoteRecordCallback
*描述：设置远程录像回调
*输入：	cszCallID		-呼叫标识
		pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetRemoteRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		iter->second->SetRemoteRecordCallback(pRecordCallback);
	}
}

/******************************************************************************
* SetSecondRecordCallback
*描述：设置辅流录像回调
*输入：	cszCallID		-呼叫标识
		pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void IMXCallMgr::SetSecondRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		iter->second->SetSecondRecordCallback(pRecordCallback);
	}
}

/******************************************************************************
* SaveLocalMainVideoToBMPFile
*描述：保存本地主流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
		cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SaveLocalMainVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		return iter->second->SaveLocalMainVideoToBMPFile(cszPathName);
	}
	return -1;
}

/******************************************************************************
* SaveRemoteMainVideoToBMPFile
*描述：保存远程主流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
		cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SaveRemoteMainVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		return iter->second->SaveRemoteMainVideoToBMPFile(cszPathName);
	}
	return -1;
}

/******************************************************************************
* SaveSecondVideoToBMPFile
*描述：保存辅流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
		cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SaveSecondVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		return iter->second->SaveSecondVideoToBMPFile(cszPathName);
	}
	return -1;
}

/******************************************************************************
* SendDTMF
*描述：发送DTMF
*输入：	cszCallID		-呼叫标识
		nDTMF			-DTMF
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::SendDTMF(const char*cszCallID,const char nDTMF)
{
	MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
	if (iter!=m_MapIMXCall.end())
	{
		//iter->second;
		return 0;
	}
	return -1;
}

/******************************************************************************
* StartMixingWithWaveFile
*描述：开始对呼叫进行WAVE文件混音，对方会听到WAVE文件播放的声音
*输入：	cszCallID		-呼叫标识
		cszPathName		-WAVE文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::StartMixingWithWaveFile(const char*cszCallID,const char*cszPathName)
{
	return -1;
}

/******************************************************************************
* StopMixingWithWaveFile
*描述：停止对呼叫进行WAVE文件混音
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCallMgr::StopMixingWithWaveFile(const char*cszCallID)
{
	return -1;
}

#ifdef _TIANZHU_CACHE_
int IMXCallMgr::SetRemoteVideoCache(const char*cszCallID, int nCacheFrameCount)
{
	if (cszCallID!=NULL)
	{
		MapIMXCall::iterator iter=m_MapIMXCall.find(cszCallID);
		if (iter!=m_MapIMXCall.end())
		{
			return iter->second->SetRemoteVideoCache(nCacheFrameCount);
		}
	}
	return -1;
}
#endif

void IMXCallMgr::OnTimer(UINT nIDEvent)
{
	while (true)
	{
		XDataBuffer*pXDataBuffer=GetDataBuffer();
		if (pXDataBuffer!=NULL)
		{
			StrPacket tStrPacket(pXDataBuffer->GetData(),false);

			std::string strCMD="";
			tStrPacket.Get("CMD",strCMD);
			if (strCMD=="CALL")
			{
				std::string strState;
				std::string strCallID="";
				std::string strUserID="";
				std::string strUserName="";

				tStrPacket.Get("STATE",strState);
				tStrPacket.Get("CID",strCallID);
				tStrPacket.Get("UID",strUserID);
				tStrPacket.Get("UNAME",strUserName);
				if (strState=="IN")
				{
					MapIMXCall::iterator iter=m_MapIMXCall.find(strCallID);
					if (iter==m_MapIMXCall.end())
					{
						IMXCall*pIMXCall=new IMXCall(strUserID,strUserName,strCallID);
						m_MapIMXCall[strCallID]=pIMXCall;
					}

					CallMgr::Instance().OnCallRingIn(strCallID,strUserID,strUserName,"IMX");
				}
				else if (strState=="OUT")
				{
					MapIMXCall::iterator iter=m_MapIMXCall.find(strCallID);
					if (iter==m_MapIMXCall.end())
					{
						IMXCall*pIMXCall=new IMXCall(strUserID,strUserName,strCallID);
						m_MapIMXCall[strCallID]=pIMXCall;
					}

					CallMgr::Instance().OnCallRingOut(strCallID,strUserID,strUserName);
				}
				else if (strState=="OK")
				{
					std::string strSDP="";
					tStrPacket.Get("SDP",strSDP);

					CallMgr::Instance().OnCallEstablished(strCallID,strUserID,strUserName);

					if (m_pMainCapChanDev!=NULL)
					{
						int nWidth=352;
						int nHeight=288;
						switch (m_nMainVideoFormatType)
						{
						case VIDEO_FORMAT_QCIF:
							{
								nWidth=176;
								nHeight=144;
							}
							break;
						case VIDEO_FORMAT_CIF:
							{
								nWidth=352;
								nHeight=288;
							}
							break;
						case VIDEO_FORMAT_4CIF:
						case VIDEO_FORMAT_16CIF:
							{
								nWidth=704;
								nHeight=576;
							}
							break;
						case VIDEO_FORMAT_720I:
						case VIDEO_FORMAT_720P:
							{
								nWidth=1280;
								nHeight=720;
							}
							break;
						case VIDEO_FORMAT_1080I:
							{
								nWidth=1920;
								nHeight=540;
							}
							break;
						case VIDEO_FORMAT_1080P:
							{
								nWidth=1920;
								nHeight=1080;
							}
							break;
						case VIDEO_FORMAT_QVGA:
							{
								nWidth=320;
								nHeight=240;
							}
							break;
						case VIDEO_FORMAT_VGA:
							{
								nWidth=640;
								nHeight=480;
							}
							break;
						case VIDEO_FORMAT_SVGA:
							{
								nWidth=800;
								nHeight=600;
							}
							break;
						case VIDEO_FORMAT_XGA:
							{
								nWidth=1024;
								nHeight=768;
							}
							break;
						case VIDEO_FORMAT_SXGA:
							{
								nWidth=1280;
								nHeight=1024;
							}
							break;
						}

						if (CallMgr::Instance().GetAudioOnly()==0)
						{
							m_pMainCapChanDev->OpenVideoDev(m_nMainVideoDevID,nWidth,nHeight,m_nMainVideoFrameRate,m_nMainVideoBitrate,0,m_nMainVideoCodecType,0,0);
						}
					}

					StrPacket sdpStrPacket(strSDP.c_str(),false);
					unsigned long nAudioID=0;
					unsigned long nVideoID=0;
					std::string strMCUID="";
					std::string strMCUIP="";
					unsigned short nMCUPort=0;
					sdpStrPacket.Get("AID",nAudioID);
					sdpStrPacket.Get("VID",nVideoID);
					sdpStrPacket.Get("ID",strMCUID);
					sdpStrPacket.Get("IP",strMCUIP);
					sdpStrPacket.Get("PORT",nMCUPort);

					MapIMXCall::iterator iter=m_MapIMXCall.find(strCallID);
					if (iter!=m_MapIMXCall.end())
					{
						IMXCall*pIMXCall=iter->second;

						std::string strPeerNodeID=NETEC_Call::GetCallPeerNodeID(strCallID.c_str());
						std::string strPeerNATIP=NETEC_Call::GetCallPeerNATIP(strCallID.c_str());
						std::string strPeerLocalIP=NETEC_Call::GetCallPeerLocalIP(strCallID.c_str());
						unsigned short nPeerLocalPort=NETEC_Call::GetCallPeerLocalPort(strCallID.c_str());
						unsigned short nPeerNATPort=NETEC_Call::GetCallPeerNATPort(strCallID.c_str());

						pIMXCall->OpenPlay(nAudioID,nVideoID,strPeerNodeID.c_str(),strPeerNATIP.c_str(),nPeerNATPort,strPeerLocalIP.c_str(),nPeerLocalPort,strMCUID.c_str(),strMCUIP.c_str(),nMCUPort,0);
					}
				}
				else if (strState=="BYE")
				{
					CallMgr::Instance().OnCallDestroyed(strCallID,strUserID,strUserName,(IMXEC_CALL_END_REASON)0);

					if (m_pMainCapChanDev!=NULL)
					{
						m_pMainCapChanDev->CloseAudio();
						m_pMainCapChanDev->CloseVideoDev();
					}

					MapIMXCall::iterator iter=m_MapIMXCall.find(strCallID);
					if (iter!=m_MapIMXCall.end())
					{
						IMXCall*pIMXCall=iter->second;
						m_MapIMXCall.erase(iter);

						pIMXCall->Close();
						delete pIMXCall;
						pIMXCall=NULL;
					}

					if (m_MapIMXCall.size()==0)
					{
						if (m_pMainCapChanDev!=NULL)
						{
							m_pMainCapChanDev->Close();
							delete m_pMainCapChanDev;
							m_pMainCapChanDev=NULL;
						}
					}
				}
			}

			delete pXDataBuffer;
			pXDataBuffer=NULL;
		}
		else
		{
			break;
		}
	}
}

void IMXCallMgr::OnNETEC_CallRingOut(const char*cszPeerUserID,const char*cszPeerUserName,const char*cszCallID,const char*cszCallType)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","OUT");
		tStrPacket.Set("CID",cszCallID);
		tStrPacket.Set("UID",cszPeerUserID);
		tStrPacket.Set("UNAME",cszPeerUserName);

		std::string strText="";

		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}

}

void IMXCallMgr::OnNETEC_CallRingIn(const char*cszPeerUserID,const char*cszPeerUserName,const char*cszPeerSDP,const char*cszCallID,const char*cszCallType)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","IN");
		tStrPacket.Set("CID",cszCallID);
		tStrPacket.Set("UID",cszPeerUserID);
		tStrPacket.Set("UNAME",cszPeerUserName);

		std::string strText="";

		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void IMXCallMgr::OnNETEC_CallEstablished(const char*cszPeerUserID,const char*cszPeerUserName,const char*cszPeerSDP,const char*cszCallID,const char*cszCallType)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","OK");
		tStrPacket.Set("CID",cszCallID);
		tStrPacket.Set("UID",cszPeerUserID);
		tStrPacket.Set("UNAME",cszPeerUserName);
		tStrPacket.Set("SDP",cszPeerSDP);

		std::string strText="";

		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

void IMXCallMgr::OnNETEC_CallDestroyed(const char*cszPeerUserID,const char*cszPeerUserName,const char*cszCallID,const char*cszCallType)
{
	XDataBuffer*pXDataBuffer=new XDataBuffer();
	if (pXDataBuffer!=NULL)
	{
		StrPacket tStrPacket;
		tStrPacket.Set("CMD","CALL");
		tStrPacket.Set("STATE","BYE");
		tStrPacket.Set("CID",cszCallID);
		tStrPacket.Set("UID",cszPeerUserID);
		tStrPacket.Set("UNAME",cszPeerUserName);

		std::string strText="";

		tStrPacket.GetString(strText);

		pXDataBuffer->StoreData((char*)strText.c_str(),strText.size()+1);
		SetDataBuffer(pXDataBuffer);
	}
}

