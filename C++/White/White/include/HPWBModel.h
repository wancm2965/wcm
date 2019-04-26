#pragma once

class IHPNetNotify
{
public:
	virtual void SendWbData(const char* pData,int iLen,bool bToNet = true) = 0;
	virtual bool OpenSaveFile(BOOL bOpenSave,char* sFilePath,LPCTSTR lpszDefExt = NULL,LPCTSTR lpszFilter = NULL,HWND hParent=NULL ,LPCTSTR strFilePath =_T("")) = 0;
	virtual int MessageBox(HWND hOwner,const CString & strTitle,const CString & strContent,UINT nStye, bool bAutoDestroy = false) = 0;
	virtual void ShowTipWindow(const CString& strTip,int nSeconds = 3,HWND pParentWnd = NULL) = 0;
	virtual void SetSelectObjStatue(MSG msg) =0;
};

class IHPWBNotify
{
public:
	virtual void NotifyFullSreen(bool bFull) = 0;
	virtual void NotifyShowMainWnd(bool bShow) = 0;
	virtual void NotifyStartTouchScreen() = 0;//֪ͨ�ⲿ����װ�ķ�������Ȩ���ⲿͨ������IHPWBModel�е�Start(bool bTouchScreen)���ش���Ȩ�Ƿ�ɹ�
	virtual void NotifyStopTouchScreen() = 0;//֪ͨ�ⲿ�����װ�ķ�������Ȩ
	virtual void NotifyConferenceRecordData(const char* pData,int iLen, bool bKeyFrame) = 0;
};

class AFX_EXT_CLASS IHPWBModel
{
public:
	IHPWBModel(void){};
	virtual ~IHPWBModel(void){};
	virtual void Release(void) = 0;

	virtual void SetWBWndPos(CWnd * pWndInsertAfter,int x,int y,int cx,int cy,UINT nFlag) = 0;
	virtual void ShowWBWnd(bool bShow) = 0;
	virtual void SetParentWnd(CWnd* pWnd) = 0;
	virtual void RegistNetNotify(IHPNetNotify* pNotify) = 0;
	virtual void ProcessNetDataIn(const char* pData,int iLen) = 0;
	virtual void RequestAllObj() = 0;
	virtual void ShowToolBar(bool bShow) = 0;
	virtual void Start(bool bTouchScreen) = 0;
	virtual void SetCheckTouchScreen(bool bCheck) = 0;
	virtual BOOL OpenPic(CString strPath) = 0;
	virtual void SetLocalUserID(CString strUserID)=0;
	virtual void MemberOffline(CString strUserID)=0;    //�����ⲿ�û����ߵ��û���
	virtual BOOL StartConferenceRecord()=0;				// ��ʼ����¼��
	virtual void StopConferenceRecord()=0;				// ֹͣ����¼��	
	virtual void Lock(bool bLock) = 0;					//�Ƿ���������

	static IHPWBModel* CreateWBModel(IHPNetNotify* pNotify,IHPWBNotify* pWBNotify,CWnd* pParentWnd,bool bNeedToCheckTouchScreen,const std::string& strLanguageFile,const std::string&  strIniPath,const std::string&  strTempPath);
};


