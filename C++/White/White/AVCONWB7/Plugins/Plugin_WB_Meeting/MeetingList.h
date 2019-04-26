#pragma once
#include "win_impl_base.hpp"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Room.h"
#include "MeetingTip.h"
enum _HIDDEN_DIRECTION   //���ط���
{
	//DIR_LEFT,          //��������
	DIR_TOP,           //��������
	DIR_RIGHT          //��������
};
#define TIMER_MOVEINWINDOW	WM_USER + 4000	//�ƽ����ڶ�ʱ��
#define TIMER_MOVEOUTWINDOW	TIMER_MOVEINWINDOW + 1		//�Ƴ����ڶ�ʱ��
#define TIMER_REFRESHMEETINGLIST          TIMER_MOVEINWINDOW + 2//ˢ�»����б�
using namespace AVCONPlugin;


namespace AVCONPlugin
{

	class CMeetingList
		: public WindowImplBase
		, public IPlugin_U7_MM_Room_Observer
	{
	public:
		CMeetingList(void);
		~CMeetingList(void);

	public:
		//////////////////////////////////////////////////////////////////////////
		// �̳�WindowImplBase�ӿ�
		virtual void Init();
		virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LPCTSTR GetWindowClassName() const;
		virtual CString GetSkinFile();
		virtual CControlUI* CreateControl(LPCTSTR pstrClass);
		virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		//////////////////////////////////////////////////////////////////////////
		
		// �Զ����ش���
		void HiddenWindow();
		// �Զ���ʾ����
		void ReShowWindow();

	public:
		//�̳�ʵ��IPlugin_U7_MM_Room_Observer
		virtual void OnRoomListEnd(void);
		virtual void OnEnterRoom(unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType);
		virtual void OnExitRoom(unsigned int nErrCode);
		virtual void OnRoomListItem(const std::string& strRoomID,const std::string& strRoomName,const std::string& strRoomPwd, unsigned short usRoomType,const std::string& strDesc,int nMemberSum,int nOnlineSum);
		virtual void OnMMData(void){};
		virtual void OnEnterRoomReady();

//////////////////////////////////////////////////////////////////////////

		void AddMeetingList();
		void RemoveAllMemList();
		//����CoUI����Ϣ���ƣ�������ƶ����б�ĳһ����Ӷ���ťʱ���ð�ť�����һ��MouseEnter��Ϣ
		//�б�������һ��MouseLeave��Ϣ�������б���ĸ���������ʧ���ú������ڽ��������
		bool ProcessListItemBtnEvent(void* p);
	protected:
		virtual void Notify(TNotifyUI& msg);

		void EnterMeetingRoom(PMMS_ROOM pMMSRoom);
		void AddMeetItem(PMMS_ROOM pRoomListItem);
		void SetControlFontSize(CLabelUI *pLabel, unsigned int nFontSize);
	public:
		void ProcessTipMessage();
	private:
		CMeetingTip *m_pMeetingTip;
		void InitLocalLang();
		CDialogBuilder		m_dlgBuilder;
		CTreeUI*			m_pMemberTree;
		bool					m_bLogout;

		bool					m_bTouchScreenAuthorize;//�����Ƿ�֧��
		//�û�Ա�б�����Զ���������
		int							m_nHiddenDir;			// �����������Զ�����ʱ�ķ���0����1���ϣ�2����
		bool						m_bTrack;				// �����Ƿ�����
		bool						m_bFloat;				// �����Ƿ�ͣ��
		
	};

}
CMeetingList* GetMeetingList(void);