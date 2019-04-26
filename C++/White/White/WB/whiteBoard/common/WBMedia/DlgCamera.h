#pragma once

#include <AUDEC/AUDEC_Engine.h>
#include <VIDEC/VIDEC.h>
#include <IMXEC/IMXEC_MediaChannel.h>
#include <NETEC/NETEC_Core.h>
#include <NETEC/NETEC_Node.h>
//#include "CameraNetMng.h"
#include "CameraData.h"
#include "../../MTGestureImport.h"
#include "WBWindows.h"
#include "CameraData.h"
#include "DlgCameraVideoSetting.h"
#include "DlgCameraAudioSetting.h"

#define IDC_UCOMMAND_CAMERA_SETTING_VIDEO (19082)
#define IDC_UCOMMAND_CAMERA_SETTING_AUDIO (IDC_UCOMMAND_CAMERA_SETTING_VIDEO+1)
//临时用
#define IDC_UCOMMAND_GETLOCAL_CAMERA_SETTING (IDC_UCOMMAND_CAMERA_SETTING_VIDEO+2)
#define IDC_UCOMMAND_SETREMOTE_CAMERA_SETTING (IDC_UCOMMAND_CAMERA_SETTING_VIDEO+3)
#define UWM_START_CONNECTED (WM_USER + 1905)
class CWBCamera;

 //重写虚函数回调类，Do nothing
   class IMXEC_PlayChanCallback_Empty :
   	public IMXEC_PlayChanCallback
   {
   public:
   	IMXEC_PlayChanCallback_Empty();
   	virtual ~IMXEC_PlayChanCallback_Empty();

   public:
   	//音频数据回调函数
	   virtual void OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop);
   	virtual void OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
   	//视频数据回调函数
   	//nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode);

   	void  SetCaremaPlay(DWORD  pCarema) {m_dwPlayCarema = pCarema;}
   private:
	DWORD m_dwPlayCarema;

   };

   class CCapChanPlayHandle_Empty : public IMXEC_CapChanCallback
   {
   public:
   	CCapChanPlayHandle_Empty();
   	~CCapChanPlayHandle_Empty();
   
   	//音频数据回调函数
	virtual void OnIMXEC_CapChanCallbackAudioData(unsigned char*pData,int nLen,bool&bLoop);
   	virtual void OnIMXEC_CapChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
   	//视频数据回调函数
   	//nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnIMXEC_CapChanCallbackVideoData(unsigned char*pData,int nLen,int nKeyFrame);
   	virtual void OnIMXEC_CapChanCallbackMotionDetected(int*pResultList,int nResultCount){};//pResultList[i]==0,没有动，否则在动
   
   	virtual void OnIMXEC_CapChanCallbackFilePositionChanged(unsigned long ulSeconds){};
   	virtual void OnIMXEC_CapChanCallbackFilePause(void){};
   	virtual void OnIMXEC_CapChanCallbackFilePlay(void);
   	virtual void OnIMXEC_CapChanCallbackFileEnd(void){};
   	virtual void OnIMXEC_CapChanCallbackRecvdCtrlData(void*pData,int nLen){};
   
   	virtual void OnIMXEC_CapChanCallbackAudioIDChanged(unsigned long nAudioID){};
   	virtual void OnIMXEC_CapChanCallbackVideoDevOpen(void);
   
   	virtual void OnIMXEC_CapChanCallbackVideoSignalChanged(void){};

	void  SetCaremaID(DWORD  pCarema) {m_dwCarema = pCarema;}
   private:
   	int m_nLen;
	DWORD m_dwCarema;
   };

//class CWBCameraGestureWindows :public CWBWindow, public CMTObject
//{
//	DECLARE_DYNAMIC(CWBCameraGestureWindows)
//public:
//	//如果需要向回传递触摸消息，则设置父窗口，否则设为空
//	CWBCameraGestureWindows(CMTObject * pParent = NULL);
//	~CWBCameraGestureWindows();
//public:
//	virtual BOOL CreateWBWindows(LPCTSTR lpszClassName,
//		LPCTSTR lpszWindowName, DWORD dwStyle,
//		const RECT& rect,UINT nID, CWnd *pParent = NULL);
//protected:
//	DECLARE_MESSAGE_MAP()
//	//afx_msg LRESULT OnTouch(WPARAM wParam, LPARAM lParam);
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
//	void ProcessTouch(const TOUCHINPUT* inData);
//
//private:
//	//父窗口指针（如有）
//	CMTObject * m_pParent;
//	unsigned int m_uNumContacts;
//	
//};


class CDlgCamera : public CDialogEx
	, public CCapChanPlayHandle_Empty
	, public IMXEC_PlayChanCallback_Empty
	, public NETEC_Node
{
	DECLARE_DYNAMIC(CDlgCamera)

public:
	CDlgCamera(CWnd* pParent = NULL, BOOL bActiveCreated = TRUE,BOOL bLocal = FALSE);   // 标准构造函数
	virtual ~CDlgCamera();

	// 对话框数据
	//enum { IDD = IDD_DIALOG_CAMERA_MAIN };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
protected:
	//
	//virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PostNcDestroy();
	
public:
	//停止播放
	void StopCamera();//未用

	//获取摄像头的连接信息数据
	BOOL GetRemoteData(SCameraReceiveData &data);  //从底层获取数据
	BOOL GetLocalData(CameraSet &data);				//从底层获取数据
	BOOL GetServerData(SCameraReceiveData &data);

	BOOL UpdateRemoteData(SCameraReceiveData *data);  //更新被动打开

	CMenu menu;
	void SetWBCameraOwner(CWBCamera* pOwner) { m_pCameraOwer = pOwner ;}

	//
	void StartCamera();

	BOOL GetIsCameraOpen() {return m_bIsCameraOpen;}
	CWBCamera* GetCameraOwner() {return m_pCameraOwer;}

	//屏蔽按键（ESC, ENTER, BACK-SPACED等的默认行为）
	virtual void OnCancel();
	virtual void OnOK();
	IMXEC_CapChanDev *GetMXECCapChanDev() {return m_pCapChanDev;}
	IMXEC_PlayChan *GetMXECPlayChan() {return m_pPlayChan;}

	int PlayVideoDataCarema(const void*pData,int nLen);
	int PlayAudioDataCarema(const void*pData,int nLen);

	IMXEC_PlayChanCallback_Empty *GetPlayChanCallback_Empty() {return m_pPlayChanCallbackCarema;}
	CCapChanPlayHandle_Empty *GetCapChanPlayHandle_Empty() {return m_pVideoCapHandleCarema;}

//protected:
//	virtual void OnGestureCombineHandle(const GestureCombinePar &gesPar);
//	virtual void OnCallbackGesture(const unsigned int &nMessage, const unsigned int &nAppendMessage);
//	virtual int InRegion(float x, float y);
private:
	bool m_bIsTouchDown;//鼠标按下
	//所属的CWBCamera指针
	CWBCamera* m_pCameraOwer;
	//主动创建者，还是被动创建者
	BOOL m_bActiveCreated;

	//主动创建，本地打开摄像头
	IMXEC_CapChanDev*	m_pCapChanDev;

	//被动创建,网络连接打开摄像头
	IMXEC_PlayChan*		m_pPlayChan;

	IMXEC_PlayChanCallback_Empty* m_pPlayChanCallbackCarema;
	//视频所用的参数
	CCapChanPlayHandle_Empty* m_pVideoCapHandleCarema;


	//本地打开视频的参数对象
	CameraSet  *m_pAVCamera;
	//连接摄像头的服务器部分数据信息
	SCameraServerData m_pServerData;
	//同步发送的数据信息（被动创建方使用）
	SCameraReceiveData m_CameraReceiveData;

	//摄像头是否已经打开 TURE打开 FALSE没打开
	BOOL m_bIsCameraOpen;
	////触摸
	//CWBCameraGestureWindows *m_pCameraShowTopMost;
	//菜单图片
	CBitmap m_AudioSetBmp;  //音频设置
	CBitmap m_VideoSetBmp;  //视频设置

	CDlgCameraVideoSetting *m_dlgVideoSetting;//摄像头视频设置
	CDlgCameraAudioSetting *m_dlgAudioSetting;//摄像头音频设置

	unsigned long m_channelIDCarema; 
	bool m_bIsDrop;
private:
	/****************************************************************************
	OnNETEC_NodeConnectStatusChanged
	描述：网络连接状态的回调
	*****************************************************************************/
	virtual void OnConnectStatusChanged(BOOL bConnected);
	//virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	
	
	afx_msg void OnAudioSetting();
	//afx_msg void OnGetLocalVideoSetting();
	//afx_msg void OnSetRemoteVideoSetting();
	afx_msg LRESULT OnStartCamera(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnVideoSetting();

protected:
	afx_msg LRESULT OnCaremaMoveTouch(WPARAM wParam, LPARAM lParam);
};
