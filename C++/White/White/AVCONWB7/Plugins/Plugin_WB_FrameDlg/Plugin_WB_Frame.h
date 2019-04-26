#pragma once
#include "IPlugin/Plugin_WB_FrameDlg/IPlugin_WB_FrameDlg.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Member.h"
#include "IPlugin/Plugin_WB_Meeting/IPlugin_WB_Meeting.h"
#include "CWBPersonalDlg.h"
#include "HPWBModel.h"
namespace AVCONPlugin
{
	class CPlugin_WB_FrameDlg
	: public IPlugin_WB_FrameDlg
	, public IPlugin_U7_MM_Member_Observer
	, public IPlugin_WB_Meeting_Card
	{
	public:
		CPlugin_WB_FrameDlg(void);
		~CPlugin_WB_FrameDlg(void);

		//�̳���IPlugin�ӿ� 
	public:
		// ��ȡ�������
		virtual std::string Name();
		// ��ȡ������ܵ�
		virtual std::string ExtendPoint();
		// ��ʼ�����
		virtual bool InitPlugin();
		// ����˳�����
		virtual bool ClosePlugin();
		
		//�̳���IPlugin_WB_Meeting_Card�ӿ�
	public:
		//���ڵ�������˵���ú������������Ҫ��������ͨ����ʵ������ͬ������
		virtual HWND GetCardWnd(const char* szMemberId,int nCardIndex);
		
		//�̳���IPlugin_U7_WhiteBoard�ӿ�
	public:
		virtual void AddObserver(IPlugin_WB_FrameDlg_Observer* pObserver);
		virtual void DelObserver(IPlugin_WB_FrameDlg_Observer* pObserver);
		
		virtual HWND GetHwnd(void);
		virtual HWND CreateWhiteBoard(void);
		virtual BOOL SetWindowPos(HWND hWndInsertAfter,int X,int Y,int cx,int cy,UINT uFlags);
		virtual void DestroyWhiteBoard(void);
		virtual bool Connect(void);
		virtual void DisConnect(void);

		virtual void RequestAllObj(void);
		virtual void EnableTouch(bool bTouchScreen);
		virtual void SetLocalUserID(char *strUserID);
		virtual void MemberOffline(char *strUserID);
		virtual void Move(int x,int y,int cx,int cy,UINT nFlag);
		virtual void ProcessWbData(const char *pData, int nLen);
		virtual void GetUserID(std::string &strUser);
		virtual void SetLocalUserName(CString str);
		virtual void SetWBAVLock(bool block);
		
	public:
		//�̳�IPlugin_U7_MM_Member_Observer�ӿ�
		virtual void OnMemberStatus(const char* strGroupID,const char* strMemberID,unsigned short usMemberType,unsigned char ucStatus);
		
		virtual void OpenResource();
	private:
		CWBPersonalDlg* m_pWhiteBoardDlg;
		bool m_bConnect;	
		std::string m_strUserID;	
	};
}