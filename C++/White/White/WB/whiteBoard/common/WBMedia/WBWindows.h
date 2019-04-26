//WBWindows.h
#ifndef _AVCON_WHITEBOARD_MEDIA_WINDOWS_H_
#define _AVCON_WHITEBOARD_MEDIA_WINDOWS_H_


#include "../../MTGestureImport.h"
#include "AVCapture.h"
#include "../Define.h"
#include "CameraData.h"
#ifndef MOUSE_CURSOR_ID
#define MOUSE_CURSOR_ID 0
#endif


//视频播放状态
enum EMediaPlayState{
	Media_playing_before,
	Media_playing,
	Media_pausing,
	Media_stop
};

//是否静音
enum EVoiceState{
	VOICEOPEN = 0,
	VOICECLOSE
};

enum EMediaLoop{
	Media_Single_Play = 0,
	Media_Loop_Play
};

enum ECtrlState{
	CTRLSHOW = 1,
	CTRLHIDE
};


class CWBWindow : public CWnd
{
	DECLARE_DYNAMIC(CWBWindow)
public:
	CWBWindow();
	~CWBWindow();

public:
	virtual BOOL CreateWBWindows(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,UINT nID, CWnd *pParent = NULL);
};

class CWBMediaWindow : public CDialog
		,public CCapChanPlayHandle
		,CPlayChanEmptyHandle
{

	DECLARE_DYNAMIC(CWBMediaWindow)
public:
	friend class CWBMedia;
	CWBMediaWindow(CString strFilePath, CMTObject *pWBObject, CWnd* pParent = NULL);
	~CWBMediaWindow();

private:
	//初始化数据
	void InitialtMediaPlayingState(EMediaPlayState eMediaPlayState);

	//删除数据
	void SetInitSizeAndPos();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	void OnOpenMedia(BOOL bActiveCreated = FALSE);

	virtual void PostNcDestroy();
	//屏蔽按键（ESC, ENTER, BACK-SPACED等的默认行为）
	virtual void OnCancel();
	virtual void OnOK();

protected:

	virtual int InRegion(float x, float y);
public:
	/*获取和设置播放状态*/
	EMediaPlayState GetMediaPlayState() const { return m_eMediaPlayState; } 

	void SetMediaPlayState(EMediaPlayState mediaPlayState);

	/*翻页操作时视频的播放状态*/
	EMediaPlayState GetPrePlayState() const { return m_ePrePlayState; }

	void SetPrePlayState(EMediaPlayState prePlayState) { m_ePrePlayState = prePlayState; }

	/*获取和设置静音设置*/
	EVoiceState GetVoiceState() const { return m_eVoiceState; }

	void SetVoiceState(EVoiceState voiceState) ;

	/*获取和设置对话框的选择*/
	ECtrlState GetCtrlState() const { return m_eCtrlState; }       

	void SetCtrlState(ECtrlState ctrlDlgState);

	/*获取和设置CWBMedia指针*/
	CWBMedia* GetWBMediaOwner() const { return m_pOwnEr; }

	void SetWBMediaOwner(CWBMedia* pOwner) { m_pOwnEr = pOwner; }

	BOOL GetRemoteData( SCameraReceiveData &data );

	BOOL UpdateRemoteData( SCameraReceiveData &data );
private:
	//多媒体播放状态
	EMediaPlayState m_eMediaPlayState;

	//多媒体换页之前的状态
	EMediaPlayState m_ePrePlayState;

	//多媒体声音状态
	EVoiceState m_eVoiceState;

	//循环播放状态
	EMediaLoop m_eMediaLoopState;

	//多媒体控制工具栏状态
	ECtrlState m_eCtrlState;

	//对象指针
	CWBMedia* m_pOwnEr;

	CMTObject * m_pWBMediaObject;

	//目前已经截取的图片的张数
	static unsigned long m_pCurCatchFrame;
	//目前双击截取的图片数量
	unsigned long m_pDBCCatchNum;
	SCameraReceiveData m_CameraReceiveData;
private:
	//视频或摄像头捕获类
	CAVCapture *m_pCapture;

	//被动创建,网络连接打开摄像头
	IMXEC_PlayChan*		m_pPlayChan;
	CPlayChanEmptyHandle* m_pPlayChanCallback;

	//视频所用的参数
	CCapChanPlayHandle* m_pVideoCapHandle;

	CString strPathName;

	int					m_birate;//视频码流
	int                 m_Controlbirate;//视频码流控制

public:
	CPlayChanEmptyHandle* GetPlayChanEmptyHandleMedia() {return m_pPlayChanCallback;}

	CCapChanPlayHandle* GetCapChanPlayHandleMedia() {return m_pVideoCapHandle;}

	CAVCapture *GetCapture() const;
	IMXEC_PlayChan*GetPlayChan() const;

private:
	//初始化视频捕获
	void InitVedioCapture(const char* strVedioFile = NULL, BOOL bActiveCreated = TRUE);

	//得到视频捕获或摄像头捕获
	

	//视频设备是否打开
	BOOL IsMediaOpen();

	//音频设备是否打开
	BOOL IsAudioOpen();

	//改变视频对话框大小
	void ChangeMediaSize();

	//获取视频的文件长度，时间衡量
	unsigned long nDuration;
	unsigned long GetVedioDuration(void) const;

	//获取视频当前的位置，以时间为参照
	unsigned long nPosition;
	unsigned long GetVedioPosition(void) const;

	//定位视频播放位置，以时间为参照
	void SeekVedioPosition(unsigned long ulSeconds);

	//设置视频播放的声音大小
	int SetVedioVolume(int nVolume);

	BOOL m_bMediaLoop;

	void SetMediaLoopState(BOOL bMediaLoopState);


	//本通道所使用的设置信息
	IMXEC_CapChanDev* GetVedioCapChanDev() const ;

	CString m_strFilename;

	///原先CWBGestureWindow里的变量
	BOOL m_lButtonDownFlag;
	BOOL m_bMouseMove;

	BOOL m_bMediaCapture;//视频截屏是否成功

	bool m_bIsMediaMoveing;//视频窗口正在移动就暂停播放

public:
	void MediaPlayToPause();
	void MediaPauseToPlay();
	BOOL GetMediaCapture() {return m_bMediaCapture;}

	//视频播放
	void MediaPlay();

	//视频暂停
	void MediaPause();

	//停止播放
	void MediaStop();

	//循环播放
	void MediaLoop();

	//视频关闭
	void MediaClose();

	//保持当前图象到BMP文件
	BOOL CaptureToBMPFile();

	void MediaSetMute(EVoiceState eVoiceState);

	BOOL GetMediaLoopState() const;  

	void SetBirtrate(int birtate);//视频码流设置

	int GetBirtrate() const { return m_birate;}
	void SetControlBirtrate(int controlbirtate);//视频码流控制

	int GetControlBirtrate() const { return m_Controlbirate;}

	void IsPlanChan();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	int PlayVideoData(const void*pData,int nLen);
	int PlayAudioData(const void*pData,int nLen);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
protected:
	afx_msg LRESULT OnMediaMoveTouch(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMediaCapture(WPARAM wParam, LPARAM lParam);

public:
	void MediaPhotoEditFun();

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
private:
	CString m_StrMediaDBclkPath;//视频双击截图路径
	bool m_bFindFirst;//第一次查找文件
};
#endif
