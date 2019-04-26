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
//��ʱ��
#define IDC_UCOMMAND_GETLOCAL_CAMERA_SETTING (IDC_UCOMMAND_CAMERA_SETTING_VIDEO+2)
#define IDC_UCOMMAND_SETREMOTE_CAMERA_SETTING (IDC_UCOMMAND_CAMERA_SETTING_VIDEO+3)
#define UWM_START_CONNECTED (WM_USER + 1905)
class CWBCamera;

 //��д�麯���ص��࣬Do nothing
   class IMXEC_PlayChanCallback_Empty :
   	public IMXEC_PlayChanCallback
   {
   public:
   	IMXEC_PlayChanCallback_Empty();
   	virtual ~IMXEC_PlayChanCallback_Empty();

   public:
   	//��Ƶ���ݻص�����
	   virtual void OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop);
   	virtual void OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
   	//��Ƶ���ݻص�����
   	//nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
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
   
   	//��Ƶ���ݻص�����
	virtual void OnIMXEC_CapChanCallbackAudioData(unsigned char*pData,int nLen,bool&bLoop);
   	virtual void OnIMXEC_CapChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
   	//��Ƶ���ݻص�����
   	//nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
	virtual void OnIMXEC_CapChanCallbackVideoData(unsigned char*pData,int nLen,int nKeyFrame);
   	virtual void OnIMXEC_CapChanCallbackMotionDetected(int*pResultList,int nResultCount){};//pResultList[i]==0,û�ж��������ڶ�
   
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
//	//�����Ҫ��ش��ݴ�����Ϣ�������ø����ڣ�������Ϊ��
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
//	//������ָ�루���У�
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
	CDlgCamera(CWnd* pParent = NULL, BOOL bActiveCreated = TRUE,BOOL bLocal = FALSE);   // ��׼���캯��
	virtual ~CDlgCamera();

	// �Ի�������
	//enum { IDD = IDD_DIALOG_CAMERA_MAIN };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
protected:
	//
	//virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PostNcDestroy();
	
public:
	//ֹͣ����
	void StopCamera();//δ��

	//��ȡ����ͷ��������Ϣ����
	BOOL GetRemoteData(SCameraReceiveData &data);  //�ӵײ��ȡ����
	BOOL GetLocalData(CameraSet &data);				//�ӵײ��ȡ����
	BOOL GetServerData(SCameraReceiveData &data);

	BOOL UpdateRemoteData(SCameraReceiveData *data);  //���±�����

	CMenu menu;
	void SetWBCameraOwner(CWBCamera* pOwner) { m_pCameraOwer = pOwner ;}

	//
	void StartCamera();

	BOOL GetIsCameraOpen() {return m_bIsCameraOpen;}
	CWBCamera* GetCameraOwner() {return m_pCameraOwer;}

	//���ΰ�����ESC, ENTER, BACK-SPACED�ȵ�Ĭ����Ϊ��
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
	bool m_bIsTouchDown;//��갴��
	//������CWBCameraָ��
	CWBCamera* m_pCameraOwer;
	//���������ߣ����Ǳ���������
	BOOL m_bActiveCreated;

	//�������������ش�����ͷ
	IMXEC_CapChanDev*	m_pCapChanDev;

	//��������,�������Ӵ�����ͷ
	IMXEC_PlayChan*		m_pPlayChan;

	IMXEC_PlayChanCallback_Empty* m_pPlayChanCallbackCarema;
	//��Ƶ���õĲ���
	CCapChanPlayHandle_Empty* m_pVideoCapHandleCarema;


	//���ش���Ƶ�Ĳ�������
	CameraSet  *m_pAVCamera;
	//��������ͷ�ķ���������������Ϣ
	SCameraServerData m_pServerData;
	//ͬ�����͵�������Ϣ������������ʹ�ã�
	SCameraReceiveData m_CameraReceiveData;

	//����ͷ�Ƿ��Ѿ��� TURE�� FALSEû��
	BOOL m_bIsCameraOpen;
	////����
	//CWBCameraGestureWindows *m_pCameraShowTopMost;
	//�˵�ͼƬ
	CBitmap m_AudioSetBmp;  //��Ƶ����
	CBitmap m_VideoSetBmp;  //��Ƶ����

	CDlgCameraVideoSetting *m_dlgVideoSetting;//����ͷ��Ƶ����
	CDlgCameraAudioSetting *m_dlgAudioSetting;//����ͷ��Ƶ����

	unsigned long m_channelIDCarema; 
	bool m_bIsDrop;
private:
	/****************************************************************************
	OnNETEC_NodeConnectStatusChanged
	��������������״̬�Ļص�
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
