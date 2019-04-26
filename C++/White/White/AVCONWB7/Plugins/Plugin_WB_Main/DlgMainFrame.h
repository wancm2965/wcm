#pragma once
#include "win_impl_base.hpp"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Function.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Member.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Room.h"
#include "IPlugin/Plugin_WB_FrameDlg/IPlugin_WB_FrameDlg.h"
#include "MenuBarDlg.h"

class CDlgMainFrame 
	: public WindowImplBase
	, public IPlugin_U7_MM_Function_Observer
	, public IPlugin_U7_MM_Member_Observer
	, public IPlugin_U7_MM_Room_Observer
	, public IPlugin_WB_FrameDlg_Observer
{
public:
	CDlgMainFrame(void);
	~CDlgMainFrame(void);
	//�̳�WindowImplBase�ӿ�
public:
	virtual void Init();
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LPCTSTR GetWindowClassName() const;
	virtual CString GetSkinFile();

	//�̳�IPlugin_U7_MM_Function_Observer�ӿ�
	virtual void OnBroadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,const char* strCardName,int nChannelType,const MMS_MEMBER_TYPE& memberType);
	virtual void OnCloseBroadcast(const char* strMemberID,int nCardIndex,int nChannelType);
	virtual void OnTouchScreenAuthorize(bool bAuthorize);

	//�̳�IPlugin_U7_MM_Room_Observer�ӿ�
	virtual void OnEnterRoom(unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType);
		/* �յ��˳��������¼�
		* nErrCode���ص�¼���ֵ
		* 0: ��ʾ�����˳�������
		* 1: ��ʾ�������˳�������
		* 2: ��ʾ���߳�������
		* 3: �����Ա�ʺ��ڸ÷������ϲ����ڻ�÷�����û�м��ػ���ģ�� */
	virtual void OnEnterRoomReady();
	virtual void OnExitRoom(unsigned int nErrCode);

public:
	//�̳�IPlugin_WB_FrameDlg_Observer�ӿ�
	virtual void ProcessWBToLocalData( char* pNotifyData );//���յ��Ӱװ��ڲ�������������
public:
	bool Connect();
	bool ReleaseConnect();

	void OnExitInternet();//�����˻ص�����
	
	void ShowMenuBarDlg();
protected:
	virtual void Notify(TNotifyUI& msg);
	void OnCoUITimer( TNotifyUI& msg );
	//�����ж�
	//virtual bool BInRegion( const POINT &pt ) const;
private:

//	CSkinNotifyTransitionToNet* m_pSkinNotifyTransitionToNet;
	bool					m_bTouchScreenAuthorize;
	unsigned short		m_usMemberType; //�û��ڻ����е���� 
	bool					m_bTakeDataToNet; //�Ƿ�Ҫ�������ݵ�����
	bool					m_bAcceptBroadCast;//�Ƿ���ܵ��㲥��Ϣ

	bool                   m_bIsExitRoomCurTime;//�Ƿ��˳�����wang

	std::string             m_strUserMemberID;//��¼�û�ID
	
	
	CMenuBarDlg*            m_pMenuBarDlg;
}; 

CDlgMainFrame* GetDlgMainFrame_(void);
