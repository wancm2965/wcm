#ifndef __LIBXMEDIAFILECAPTURE_H__
#define __LIBXMEDIAFILECAPTURE_H__

#include <AVCONEC/libAVCONEC.h>

class IXMediaFileCaptureNotify
	: public IHPAVCaptureNotify
{
public:
	IXMediaFileCaptureNotify(void){};
	virtual~IXMediaFileCaptureNotify(void){};
public:
	virtual void OnIXMediaFileCaptureNotifyFilePositionChanged(unsigned long ulSeconds)=0;
	virtual void OnIXMediaFileCaptureNotifyFilePause(void)=0;
	virtual void OnIXMediaFileCaptureNotifyFilePlay(void)=0;
	virtual void OnIXMediaFileCaptureNotifyFileEnd(void)=0;
};

class IXMediaFileCapture
	: public IHPVideoWindow
{
public:
	IXMediaFileCapture(void){};
	virtual~IXMediaFileCapture(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual unsigned long GetDuration(void)=0;
	virtual unsigned long GetPosition(void)=0;
	virtual void Seek(unsigned long ulSeconds)=0;
	virtual void Pause(void)=0;
	virtual void Play(void)=0;
	virtual bool IsPlaying(void)=0;
	virtual void SetVolume(int nVolume)=0;
	virtual int GetVolume(void)=0;
	virtual void SetMute(bool bMute=true)=0;
	virtual bool GetMute(void)=0;
	virtual void SetBalance(int nBalance)=0;
	virtual int GetBalance(void)=0;
	virtual int GetAudioLevel(void)=0;//建议每隔200毫秒调用一次

	virtual bool HasAudio(void)=0;
	virtual bool HasVideo(void)=0;
	virtual int GetAudioBitrate(void)=0;
	virtual void CaptureVideoKeyFrame(bool bSub)=0;
	virtual int GetVideoBitrate(void)=0;

	/******************************************************************************
	* EnableOSD
	*描述：设置是否显示OSD
	*输入：	bEnable			-是否显示OSD,为true时显示，否则不显示
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void EnableOSD(bool bEnable)=0;

	/******************************************************************************
	* IsOSDEnable
	*描述：获取是否显示OSD
	*输入：无
	*输出：无
	*返回值：为true时显示，否则不显示
	*****************************************************************************/
	virtual bool IsOSDEnable(void)=0;

	/******************************************************************************
	* SetOSDText
	*描述：设置OSD的文本
	*输入：	cszText			-要显示的文本
			nAlign			-对齐方式，参考VIDEC.h中的VIDEC_OSD_ALIGN_TOP_LEFT的定义
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetOSDText(const TCHAR*cszText,int nAlign=0)=0;

	/******************************************************************************
	* SetOSDFont
	*描述：设置OSD的字体
	*输入：	logfont			-OSD的字体
			crText			-OSD的文本的颜色
			crBk			-OSD的文本的背景颜色
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetOSDFont(LOGFONT logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255))=0;

	/******************************************************************************
	* StartRecordToAVI
	*描述：开始录像
	*输入：	cszPathName		-录像文件名称
			nStandardFormat	-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int StartRecordToAVI(const char*cszPathName,int nStandardFormat=1)=0;
	
	/******************************************************************************
	* StopRecordToAVI
	*描述：停止录像
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void StopRecordToAVI(void)=0;


	/******************************************************************************
	* SetScreenWnd
	*描述：设置所在屏幕窗口位置
	*输入：	nScreenIndex	-屏幕号
			nWndIndex		-窗口号
			nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;

	//nNetworkType,参见VIDEC.h中的VIDEC_BITRATE_CONTROL_VBR等定义
	static IXMediaFileCapture*Create(IXMediaFileCaptureNotify&rNotify,HWND hWnd,const char*cszMediaFile,int nFrameRate,int nQuality,VIDEC_CODEC_TYPE codecType,bool bLoop,int nNetworkType);
};


#endif