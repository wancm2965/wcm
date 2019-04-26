#pragma once
#include "HPWBModel.h"
#include "HPMCU/AVMTDataChannel.h"
#include "HPMCU/AVMTMCUClient.h"

// CIndependent_WBDlg �Ի���

class CIndependent_WBDlg
	: public CDialog
	, public AVMTDataChannel
	, public IAVMTDataSessionClientNotify
	//, public IHPWBDataNotify
	, public IHPNetNotify
	, public IHPWBNotify
	//, public IHPWBModel
{
	DECLARE_DYNAMIC(CIndependent_WBDlg)

public:
	CIndependent_WBDlg(unsigned long ulWBChannelID);
	~CIndependent_WBDlg(void);

// �Ի�������
	enum { IDD = IDD_WHITEBOARD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	BOOL PreTranslateMessage( MSG* pMsg );

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	bool Connect(unsigned long ulWBChannelID,const std::string& mcuid,const std::string& mcuaddr,unsigned short mcuport,const std::string& mcutype);
	void ReleaseConnection(void);
	void SetWBChannelChanged(unsigned long ulWBChannelID,const std::string& mcuid,const std::string& mcuaddr,unsigned short mcuport,const std::string& mcutype);
	bool IsConnect(void);

	/*����װ��ڲ��Զ������ݽṹ��������a���װ����ݰ� b�����ݰ�����*/
	bool processWBData(char * pData,long nLen);
	void SetLocalUserID(std::string strUserID);

private:
	IAVMTDataSessionClient*	m_pIAVMTDataSessionClient;

	unsigned long			m_ulWBChannelID;
	bool					m_bConnected;
	int						m_nConnectCount;
	bool                    m_bSysFullScreen;   //�Ƿ�������ͬ��ȫ��
	std::string				m_mcuid;
	std::string				m_mcuaddr;
	unsigned short			m_mcuport;
	std::string				m_mcutype;

public:
	//////////////////////////////////////////////////////////////////////////
	// �̳�AVMTDataChannel�ӿ�
	bool IsEmpty(void){ return false; };
	void OnDataToUI(const char*pData,int nLen);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// �̳�IAVMTDataSessionClientNotify�ӿ�
	virtual void OnIAVMTDataSessionClientNotifyConnected(void){};
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// �̳�IHPWBDataNotify�ӿ�
	virtual void ProcessWBDataOut(const char* pData,int iLen){};
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// �̳�IHPNetNotify�ӿ�
	virtual void SendWbData(const char* pData,int iLen,bool bToNet = true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// �̳�IHPWBNotify�ӿ�
	virtual void NotifyFullSreen(bool bFull){};
	virtual void NotifyShowMainWnd(bool bShow){};
	virtual void NotifyStartTouchScreen(){};
	virtual void NotifyStopTouchScreen(){};
	virtual void NotifyConferenceRecordData(const char* pData,int iLen, BOOL bKeyFrame){};
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// �̳�IHPWBModel�ӿ�
	virtual void SetWBWndPos(CWnd * pWndInsertAfter,int x,int y,int cx,int cy,UINT nFlag){};
	virtual void ShowWBWnd(bool bShow){};
	virtual void SetParentWnd(CWnd* pWnd){};
	virtual void RegistNetNotify(IHPNetNotify* pNotify){};
	virtual void ProcessNetDataIn(const char* pData,int iLen){};
	virtual void RequestAllObj(){};
	virtual void ShowToolBar(bool bShow){};
	virtual void Start(bool bTouchScreen){};
	virtual void SetCheckTouchScreen(bool bCheck){};
	virtual BOOL OpenPic(CString strPath){ return FALSE; };
	virtual void SetLocalUserID(CString strUserID){};
	virtual void MemberOffline(CString strUserID){};
	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
};
