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


//��Ƶ����״̬
enum EMediaPlayState{
	Media_playing_before,
	Media_playing,
	Media_pausing,
	Media_stop
};

//�Ƿ���
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
	//��ʼ������
	void InitialtMediaPlayingState(EMediaPlayState eMediaPlayState);

	//ɾ������
	void SetInitSizeAndPos();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	void OnOpenMedia(BOOL bActiveCreated = FALSE);

	virtual void PostNcDestroy();
	//���ΰ�����ESC, ENTER, BACK-SPACED�ȵ�Ĭ����Ϊ��
	virtual void OnCancel();
	virtual void OnOK();

protected:

	virtual int InRegion(float x, float y);
public:
	/*��ȡ�����ò���״̬*/
	EMediaPlayState GetMediaPlayState() const { return m_eMediaPlayState; } 

	void SetMediaPlayState(EMediaPlayState mediaPlayState);

	/*��ҳ����ʱ��Ƶ�Ĳ���״̬*/
	EMediaPlayState GetPrePlayState() const { return m_ePrePlayState; }

	void SetPrePlayState(EMediaPlayState prePlayState) { m_ePrePlayState = prePlayState; }

	/*��ȡ�����þ�������*/
	EVoiceState GetVoiceState() const { return m_eVoiceState; }

	void SetVoiceState(EVoiceState voiceState) ;

	/*��ȡ�����öԻ����ѡ��*/
	ECtrlState GetCtrlState() const { return m_eCtrlState; }       

	void SetCtrlState(ECtrlState ctrlDlgState);

	/*��ȡ������CWBMediaָ��*/
	CWBMedia* GetWBMediaOwner() const { return m_pOwnEr; }

	void SetWBMediaOwner(CWBMedia* pOwner) { m_pOwnEr = pOwner; }

	BOOL GetRemoteData( SCameraReceiveData &data );

	BOOL UpdateRemoteData( SCameraReceiveData &data );
private:
	//��ý�岥��״̬
	EMediaPlayState m_eMediaPlayState;

	//��ý�廻ҳ֮ǰ��״̬
	EMediaPlayState m_ePrePlayState;

	//��ý������״̬
	EVoiceState m_eVoiceState;

	//ѭ������״̬
	EMediaLoop m_eMediaLoopState;

	//��ý����ƹ�����״̬
	ECtrlState m_eCtrlState;

	//����ָ��
	CWBMedia* m_pOwnEr;

	CMTObject * m_pWBMediaObject;

	//Ŀǰ�Ѿ���ȡ��ͼƬ������
	static unsigned long m_pCurCatchFrame;
	//Ŀǰ˫����ȡ��ͼƬ����
	unsigned long m_pDBCCatchNum;
	SCameraReceiveData m_CameraReceiveData;
private:
	//��Ƶ������ͷ������
	CAVCapture *m_pCapture;

	//��������,�������Ӵ�����ͷ
	IMXEC_PlayChan*		m_pPlayChan;
	CPlayChanEmptyHandle* m_pPlayChanCallback;

	//��Ƶ���õĲ���
	CCapChanPlayHandle* m_pVideoCapHandle;

	CString strPathName;

	int					m_birate;//��Ƶ����
	int                 m_Controlbirate;//��Ƶ��������

public:
	CPlayChanEmptyHandle* GetPlayChanEmptyHandleMedia() {return m_pPlayChanCallback;}

	CCapChanPlayHandle* GetCapChanPlayHandleMedia() {return m_pVideoCapHandle;}

	CAVCapture *GetCapture() const;
	IMXEC_PlayChan*GetPlayChan() const;

private:
	//��ʼ����Ƶ����
	void InitVedioCapture(const char* strVedioFile = NULL, BOOL bActiveCreated = TRUE);

	//�õ���Ƶ���������ͷ����
	

	//��Ƶ�豸�Ƿ��
	BOOL IsMediaOpen();

	//��Ƶ�豸�Ƿ��
	BOOL IsAudioOpen();

	//�ı���Ƶ�Ի����С
	void ChangeMediaSize();

	//��ȡ��Ƶ���ļ����ȣ�ʱ�����
	unsigned long nDuration;
	unsigned long GetVedioDuration(void) const;

	//��ȡ��Ƶ��ǰ��λ�ã���ʱ��Ϊ����
	unsigned long nPosition;
	unsigned long GetVedioPosition(void) const;

	//��λ��Ƶ����λ�ã���ʱ��Ϊ����
	void SeekVedioPosition(unsigned long ulSeconds);

	//������Ƶ���ŵ�������С
	int SetVedioVolume(int nVolume);

	BOOL m_bMediaLoop;

	void SetMediaLoopState(BOOL bMediaLoopState);


	//��ͨ����ʹ�õ�������Ϣ
	IMXEC_CapChanDev* GetVedioCapChanDev() const ;

	CString m_strFilename;

	///ԭ��CWBGestureWindow��ı���
	BOOL m_lButtonDownFlag;
	BOOL m_bMouseMove;

	BOOL m_bMediaCapture;//��Ƶ�����Ƿ�ɹ�

	bool m_bIsMediaMoveing;//��Ƶ���������ƶ�����ͣ����

public:
	void MediaPlayToPause();
	void MediaPauseToPlay();
	BOOL GetMediaCapture() {return m_bMediaCapture;}

	//��Ƶ����
	void MediaPlay();

	//��Ƶ��ͣ
	void MediaPause();

	//ֹͣ����
	void MediaStop();

	//ѭ������
	void MediaLoop();

	//��Ƶ�ر�
	void MediaClose();

	//���ֵ�ǰͼ��BMP�ļ�
	BOOL CaptureToBMPFile();

	void MediaSetMute(EVoiceState eVoiceState);

	BOOL GetMediaLoopState() const;  

	void SetBirtrate(int birtate);//��Ƶ��������

	int GetBirtrate() const { return m_birate;}
	void SetControlBirtrate(int controlbirtate);//��Ƶ��������

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
	CString m_StrMediaDBclkPath;//��Ƶ˫����ͼ·��
	bool m_bFindFirst;//��һ�β����ļ�
};
#endif
