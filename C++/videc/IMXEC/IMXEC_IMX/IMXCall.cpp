#include "IMXCall.h"	
#include "IMXCallMgr.h"	

IMXCall::IMXCall(const std::string&strUserID,const std::string&strUserName,const std::string&strCallID)
:m_strUserID(strUserID)
,m_strUserName(strUserName)
,m_strCallID(strCallID)

,m_pRemoteMainPlayChan(NULL)
,m_hVideoWndRemoteMain(NULL)
{

}


IMXCall::~IMXCall(void)
{

}

void IMXCall::Close(void)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->Close();
		delete m_pRemoteMainPlayChan;
		m_pRemoteMainPlayChan=NULL;
	}
}

int IMXCall::AcceptCall(void)
{
	return 0;
}

int IMXCall::HangupCall(void)
{
	return 0;
}

int IMXCall::OpenPlay(unsigned long nAudioID,unsigned long nVideoID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU)
{
	if (m_pRemoteMainPlayChan==NULL)
	{
		m_pRemoteMainPlayChan=IMXEC_PlayChan::Create(*this);
		if (m_pRemoteMainPlayChan!=NULL)
		{
			if (0!=m_pRemoteMainPlayChan->Open(m_hVideoWndRemoteMain,nAudioID,nVideoID,cszPeerNodeID,cszPeerNATIP,nPeerNATPort,cszPeerLocalIP,nPeerLocalPort,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort,0))
			{
				m_pRemoteMainPlayChan->Close();
				delete m_pRemoteMainPlayChan;
				m_pRemoteMainPlayChan=NULL;
			}
			else
			{
				m_pRemoteMainPlayChan->StartAudio();
				m_pRemoteMainPlayChan->StartVideo();
				m_pRemoteMainPlayChan->SetFrameRateControlMode(FRAME_RATE_CONTROL_MODE_FULL);
				m_pRemoteMainPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);
			}
		}
	}

	return (m_pRemoteMainPlayChan!=NULL?0:1);
}

/******************************************************************************
* SetLocalMainVideoWnd
*描述：设置本地主流视频显示的窗口
*输入：	hWnd			-本地主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::SetLocalMainVideoWnd(HWND hWnd)
{
	return IMXCallMgr::Instance().SetLocalMainVideoWnd(hWnd);
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
int IMXCall::SetLocalMainVideoWndPos(int x,int y,int cx,int cy)
{
	return IMXCallMgr::Instance().SetLocalMainVideoWndPos(x,y,cx,cy);
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
void IMXCall::SetLocalMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	IMXCallMgr::Instance().SetLocalMainVideoParam(nBrightness, nContrast, nSaturation, nHue,nGamma,nSharpen,bSharpenLocalOnly);
}


/******************************************************************************
* SetLocalMainVideoEnableOSD
*描述：设置本地主流视频是否启用OSD
*输入：	cszCallID		-呼叫标识
		nEnable			-是否启用OSD
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::SetLocalMainVideoEnableOSD(int nEnable)
{
	IMXCallMgr::Instance().SetLocalMainVideoEnableOSD(nEnable);
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
void IMXCall::SetLocalMainVideoOSDText(const char*cszText,int nAlign)//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT
{
	IMXCallMgr::Instance().SetLocalMainVideoOSDText(cszText,nAlign);
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
void IMXCall::SetLocalMainVideoOSDFont(LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	IMXCallMgr::Instance().SetLocalMainVideoOSDFont(logfont,crText,crBk);
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
int IMXCall::GetLocalMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)
{
	return IMXCallMgr::Instance().GetLocalMainVideoInfo(nWidth,nHeight,nVideoCodecType,hVideoWnd);
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
int IMXCall::GetLocalMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return IMXCallMgr::Instance().GetLocalMainVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}


/******************************************************************************
* SetRemoteMainVideoWnd
*描述：设置远程主流视频显示的窗口
*输入：	无
		hWnd			-远程主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::SetRemoteMainVideoWnd(HWND hWnd)
{
	m_hVideoWndRemoteMain=hWnd;
	return -1;
}


/******************************************************************************
* SetRemoteMainVideoWndPos
*描述：设置远程主流视频显示的窗口位置
*输入：	x				-窗口的左边位置
		y				-窗口的上边位置
		cx				-窗口的宽度
		cy				-窗口的高度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::SetRemoteMainVideoWndPos(int x,int y,int cx,int cy)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->SetWindowPos(NULL,x,y,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);
		return 0;
	}
	return -1;
}


/******************************************************************************
* SetRemoteMainVideoParam
*描述：设置远程主流视频参数
*输入：	nBrightness		-亮度值0-255
		nContrast		-对比度0-127
		nSaturation		-饱和度0-127
		nHue			-色调0-255
		nGamma			-伽马1-400
		nSharpen		-锐化0-127
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::SetRemoteMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->SetVideoParam(nBrightness, nContrast, nSaturation, nHue,nGamma,nSharpen);
	}
}


/******************************************************************************
* GetRemoteMainVideoInfo
*描述：获取远程主流视频信息
*输入：	无
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::GetRemoteMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		hVideoWnd=m_pRemoteMainPlayChan->GetVideoWnd();
		nWidth=m_pRemoteMainPlayChan->GetVideoWidth();
		nHeight=m_pRemoteMainPlayChan->GetVideoHeight();

		return 0;
	}
	return -1;
}


/******************************************************************************
* GetRemoteMainVideoPacketStatistics
*描述：获取远端主视频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::GetRemoteMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		return m_pRemoteMainPlayChan->GetVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return -1;
}


/******************************************************************************
* SetSecondVideoWnd
*描述：设置辅流视频显示的窗口
*输入：无
		hWnd			-远程辅流视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::SetSecondVideoWnd(HWND hWnd)
{
	return -1;
}


/******************************************************************************
* SetSecondVideoWndPos
*描述：设置辅流视频显示的窗口位置
*输入：	x				-窗口的左边位置
		y				-窗口的上边位置
		cx				-窗口的宽度
		cy				-窗口的高度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::SetSecondVideoWndPos(int x,int y,int cx,int cy)
{
	return -1;
}


/******************************************************************************
* SetSecondVideoParam
*描述：设置辅流视频参数
*输入：	nBrightness		-亮度值0-255
		nContrast		-对比度0-127
		nSaturation		-饱和度0-127
		nHue			-色调0-255
		nGamma			-伽马1-400
		nSharpen		-锐化0-127
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::SetSecondVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
}


/******************************************************************************
* SetSecondVideoEnableOSD
*描述：设置辅流视频是否启用OSD
*输入：	nEnable			-是否启用OSD
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::SetSecondVideoEnableOSD(int nEnable)
{
}


/******************************************************************************
* SetSecondVideoOSDText
*描述：设置辅流视频OSD文字
*输入：	cszText			-OSD文字
		nAlign			-对齐方式
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::SetSecondVideoOSDText(const char*cszText,int nAlign)//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT
{
}


/******************************************************************************
* SetSecondVideoOSDFont
*描述：设置辅流视频OSD字体,字的颜色和背景色
*输入：	logfont			-OSD字体
		crText			-字的颜色
		crBk			-背景色
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::SetSecondVideoOSDFont(LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
}


/******************************************************************************
* GetSecondVideoInfo
*描述：获取辅流视频信息
*输入：	无
*输出：	nWidth			-图像宽度
		nHeight			-图像高度
		nVideoCodecType	-编码类型
		hVideoWnd		-窗口句柄
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::GetSecondVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)
{
	return -1;
}


/******************************************************************************
* GetSecondVideoPacketStatistics
*描述：获取辅流视频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::GetSecondVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return -1;
}


/******************************************************************************
* GetLocalAudioLevel
*描述：获取本地音频Level【实际声音大小】
*输入：	无
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::GetLocalAudioLevel(int&nLevel)
{
	return IMXCallMgr::Instance().GetLocalAudioLevel(nLevel);
}


/******************************************************************************
* SetLocalAudioVolume
*描述：获取本地音频音量
*输入：	nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void IMXCall::SetLocalAudioVolume(int nVolume)
{
	IMXCallMgr::Instance().SetLocalAudioVolume(nVolume);
}


/******************************************************************************
* SetLocalAudioMute
*描述：获取本地音频是否静音
*输入：	nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void IMXCall::SetLocalAudioMute(int nMute)
{
	IMXCallMgr::Instance().SetLocalAudioMute(nMute);
}


/******************************************************************************
* GetLocalAudioCodecID
*描述：获取本地音频编码信息
*输入：	无
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::GetLocalAudioCodecID(X_AUDIO_CODEC_ID &nAudioCodecID)
{
	return IMXCallMgr::Instance().GetLocalAudioCodecID(nAudioCodecID);
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
int IMXCall::GetLocalAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return IMXCallMgr::Instance().GetLocalAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}


/******************************************************************************
* GetRemoteAudioLevel
*描述：获取远程音频Level【实际声音大小】
*输入：	无
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::GetRemoteAudioLevel(int&nLevel)
{
	nLevel=0;
	if (m_pRemoteMainPlayChan!=NULL)
	{
		return m_pRemoteMainPlayChan->GetLevel(nLevel);
	}
	return -1;
}


/******************************************************************************
* SetRemoteAudioVolume
*描述：获取远程音频音量
*输入：	nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void IMXCall::SetRemoteAudioVolume(int nVolume)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->SetVolume(nVolume);
	}
}


/******************************************************************************
* SetRemoteAudioMute
*描述：获取远程音频是否静音
*输入：	nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void IMXCall::SetRemoteAudioMute(int nMute)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{

	}
}


/******************************************************************************
* GetRemoteAudioCodecID
*描述：获取远端音频编码
*输入：	无
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::GetRemoteAudioCodecID (X_AUDIO_CODEC_ID &nAudioCodecID)
{
	return -1;
}


/******************************************************************************
* GetRemoteAudioPacketStatistics
*描述：获取远端音频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::GetRemoteAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		return m_pRemoteMainPlayChan->GetAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
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
int IMXCall::StartRecordLocalMainToAVI(const char*cszPathName,int nStandardFormat)
{
	return IMXCallMgr::Instance().StartRecordLocalMainToAVI(cszPathName,nStandardFormat);
}


/******************************************************************************
* StopRecordLocalMainToAVI
*描述：停止本地主流录像
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::StopRecordLocalMainToAVI(void)
{
	IMXCallMgr::Instance().StopRecordLocalMainToAVI();
}


/******************************************************************************
* StartRecordRemoteMainToAVI
*描述：开始远端主流录像
*输入：	cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::StartRecordRemoteMainToAVI(const char*cszPathName,int nStandardFormat)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		return m_pRemoteMainPlayChan->StartRecordToAVI(cszPathName,nStandardFormat);
	}
	return -1;
}


/******************************************************************************
* StopRecordRemoteMainToAVI
*描述：停止远端主流录像
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::StopRecordRemoteMainToAVI(void)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->StopRecordToAVI();
	}
}


/******************************************************************************
* StartRecordSecondToAVI
*描述：开始辅流录像
*输入：	cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::StartRecordSecondToAVI(const char*cszPathName,int nStandardFormat)
{
	return -1;
}


/******************************************************************************
* StopRecordSecondToAVI
*描述：停止辅流录像
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::StopRecordSecondToAVI(void)
{
}


/******************************************************************************
* GetMainAudioID
*描述：获取主流音频标识
*输入： 无
*输出： nMainAudioID	-主流音频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::GetMainAudioID(unsigned long&nMainAudioID)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
	}
	return -1;
}


/******************************************************************************
* GetMainVideoID
*描述：获取主流视频标识
*输入： 无
*输出： nMainVideoID	-主流视频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::GetMainVideoID(unsigned long&nMainVideoID)
{
	return -1;
}


/******************************************************************************
* GetSecondVideoID
*描述：获取辅流视频标识
*输入： 无
*输出： nSecondVideoID	-辅流视频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::GetSecondVideoID(unsigned long&nSecondVideoID)
{
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
int IMXCall::SetMainVideoScreenIndex(int nScreenIndex)
{
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
int IMXCall::SetSecondVideoScreenIndex(int nScreenIndex)
{
	return -1;
}



/******************************************************************************
* SetSecondVideoScreenWnd
*描述：设置辅流视频所在屏幕窗口位置
*输入：	nScreenIndex	-屏幕号
		nWndIndex		-窗口号
		nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::SetSecondVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	return ;
}


/******************************************************************************
* SetRemoteVideoScreenWnd
*描述：设置远端视频所在屏幕窗口位置
*输入：	nScreenIndex	-屏幕号
		nWndIndex		-窗口号
		nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::SetRemoteVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->SetScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
}

/******************************************************************************
* SetLocalRecordCallback
*描述：设置本地录像回调
*输入：	pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::SetLocalRecordCallback(IMXEC_RecordCallback*pRecordCallback)
{
	return ;
}

/******************************************************************************
* SetRemoteRecordCallback
*描述：设置远程录像回调
*输入：pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::SetRemoteRecordCallback(IMXEC_RecordCallback*pRecordCallback)
{
	return ;
}


/******************************************************************************
* SetSecondRecordCallback
*描述：设置辅流录像回调
*输入：	pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void IMXCall::SetSecondRecordCallback(IMXEC_RecordCallback*pRecordCallback)
{
	return ;
}


/******************************************************************************
* SaveLocalMainVideoToBMPFile
*描述：保存本地主流视频到BMP图片文件
*输入：	cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::SaveLocalMainVideoToBMPFile(const char*cszPathName)
{
	return IMXCallMgr::Instance().SaveLocalMainVideoToBMPFile(cszPathName);
}


/******************************************************************************
* SaveRemoteMainVideoToBMPFile
*描述：保存远程主流视频到BMP图片文件
*输入：	cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::SaveRemoteMainVideoToBMPFile(const char*cszPathName)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		return m_pRemoteMainPlayChan->CaptureToBMPFile(cszPathName)?0:-1;
	}
	return -1;
}


/******************************************************************************
* SaveSecondVideoToBMPFile
*描述：保存辅流视频到BMP图片文件
*输入：	cszPathName		-图片文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXCall::SaveSecondVideoToBMPFile(const char*cszPathName)
{
	return -1;
}


#ifdef _TIANZHU_CACHE_
int IMXCall::SetRemoteVideoCache(int nCacheFrameCount)
{
	if (m_pRemoteMainPlayChan)
	{
		return m_pRemoteMainPlayChan->SetRemoteVideoCache(nCacheFrameCount);
	}
	return -1;
}
#endif
