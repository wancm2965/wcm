#pragma once
#include "HPMCU/AVMTDataChannel.h"
#include "HPMCU/AVMTMCUClient.h"
#include "IPlugin/Plugin_WB_FrameDlg/IPlugin_WB_FrameDlg.h"
using namespace AVCONPlugin;
// CWBPersonalDlg 对话框

class CWBPersonalDlg
	: public CDialog
	, public AVMTDataChannel
	, public IAVMTDataSessionClientNotify
	, public IHPNetNotify
//	, public IHPWBNotify
{
	DECLARE_DYNAMIC(CWBPersonalDlg)

public:
	CWBPersonalDlg(unsigned long ulWBChannelId,CWnd* pParent = NULL);
	CWBPersonalDlg(unsigned long ulWBChannelId,std::string strLocalUserId,CWnd* pParent = NULL);
	virtual ~CWBPersonalDlg();

	// 对话框数据
	enum { IDD = IDD_WHITEBOARD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	IAVMTDataSessionClient* m_pIAVMTDataSessionClient;
	unsigned long  m_ulDataChannelId;
	bool		m_bConnected;
	int		m_nConnectCount;	

	std::string		m_mcuid;
	std::string		m_mcuaddr;
	unsigned short m_mcuport;
	std::string		m_mcutype;

	std::string		m_strLocalUserId;

public:
	bool Connect(unsigned long ulWBChannelID,const std::string& mcuid,const std::string& mcuaddr,unsigned short mcuport,const std::string& mcutype);
	void ReleaseConnection(void);
	void SetWBChannelChanged(unsigned long ulWBChannelID,const std::string& mcuid,const std::string& mcuaddr,unsigned short mcuport,const std::string& mcutype);
	bool IsConnect(void);
	
	/*处理白板内部自定义数据结构。参数：a）白板数据包 b）数据包长度*/
	bool processWBData(char * pData,long nLen);
	void SetLocalUserID(std::string strUserID);
	void SetLoaclUserName(CString str);
	void SetWBLock(bool block);
	void DestroyWBDlg();
public:
	//继承AVMTDataChannel接口
	bool IsEmpty(void){return false;};
	void OnDataToUI(const char*pData,int nLen);
	virtual void onDTSNetDataNotify(unsigned long ulSessionID,const char* pData,int nLen){};
	//继承IAVMTDataSessionClientNotify接口
	virtual void OnIAVMTDataSessionClientNotifyConnected(void){};
	// 继承IHPWBDataNotify接口
	virtual void ProcessWBDataOut(const char* pData,int iLen){};

	// 继承IHPNetNotify接口
	virtual void SendWbData(const char* pData,int iLen,bool bToNet = true);
	virtual bool OpenSaveFile(BOOL bOpenSave,char*sFilePath,LPCTSTR lpszDefExt = NULL,LPCTSTR lpszFilter = NULL,HWND hParent=NULL ,LPCTSTR strFilePath =_T(""));
	virtual int MessageBox(HWND hOwner,const CString & strTitle,const CString & strContent,UINT nStye, bool bAutoDestroy = false);
	virtual void ShowTipWindow(const CString& strTip,int nSeconds = 3,HWND pParentWnd = NULL);
	virtual void SetSelectObjStatue(MSG msg);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void addObserver(IPlugin_WB_FrameDlg_Observer* pObserver);
	void DelObserver(IPlugin_WB_FrameDlg_Observer* pObserver);
private:
	std::list<IPlugin_WB_FrameDlg_Observer*> m_lstObserver;
};
