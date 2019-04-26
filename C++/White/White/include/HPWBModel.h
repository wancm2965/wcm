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
	virtual void NotifyStartTouchScreen() = 0;//通知外部申请白板的服务器授权，外部通过调用IHPWBModel中的Start(bool bTouchScreen)来回答授权是否成功
	virtual void NotifyStopTouchScreen() = 0;//通知外部撤销白板的服务器授权
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
	virtual void MemberOffline(CString strUserID)=0;    //接收外部用户下线的用户名
	virtual BOOL StartConferenceRecord()=0;				// 开始会议录像
	virtual void StopConferenceRecord()=0;				// 停止会议录像	
	virtual void Lock(bool bLock) = 0;					//是否锁定操作

	static IHPWBModel* CreateWBModel(IHPNetNotify* pNotify,IHPWBNotify* pWBNotify,CWnd* pParentWnd,bool bNeedToCheckTouchScreen,const std::string& strLanguageFile,const std::string&  strIniPath,const std::string&  strTempPath);
};


