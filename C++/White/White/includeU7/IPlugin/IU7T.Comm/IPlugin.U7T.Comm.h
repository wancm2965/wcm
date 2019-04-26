// UCC7-T��������ͨ�ýӿ�
// 
#pragma once
#include "../../ISystem/IPlugin.h"
 
namespace AVCONPlugin
{
	typedef	struct _U7TBAR_BTN_INFO		// ��ť���õ������б���ͼ
	{
		std::string	strBtnName;			// ��ť�����֣��������Ĳ������һ�£�
		std::string strBtnText;			// ���������
		std::string	strBtnTooltip;		// ���������ʾ
		std::string strNoSelNormal;		// ��ťδѡ��״̬�µ���ͨ״̬
		std::string strNoSelHot;		// ��ťδѡ��״̬�µ���ͣ״̬
		std::string strNoSelPush;		// ��ťδѡ��״̬�µİ���״̬
		std::string strNoSelDisable;	// ��ťδѡ��״̬�µĽ���״̬
		std::string strSelNormal;		// ��ťѡ��״̬�µ���ͨ״̬
		std::string strSelHot;			// ��ťѡ��״̬�µ���ͣ״̬
		std::string strSelPush;			// ��ťѡ��״̬�µİ���״̬
		std::string strSelDisable;		// ��ťѡ��״̬�µĽ���״̬
	}U7TBAR_BTN_INFO;

	// UCC7-T��������ͨ�ýӿ�
	class IPlugin_U7T_Comm_Notify
	{
	public:
		virtual ~IPlugin_U7T_Comm_Notify(){};

	public:
		// ��ť״̬�任�ص�
		// ������strBtnName ��ť���֣���������һ�£�
		//		 bSel ��ť�Ƿ�ѡ��
		virtual void OnBtnStatus(std::string strBtnName, bool bSel) = 0;
	};

	// UCC7-T��������ͨ�ýӿ�
	class IPlugin_U7T_Comm : virtual public IPlugin
	{
	public:
		virtual void MainFrameHWnd(HWND hWnd) = 0;
		virtual const char* GetButtonImage(void) = 0;
		virtual const char* GetButtonName(void) = 0;
		virtual void OnU7TBtnClick(void) = 0;

		virtual void SetU7TCommNotify(IPlugin_U7T_Comm_Notify* pNotify) = 0;
		// ���ò����Ӧ��ť�����ֺͱ���ͼ
		// ������structBtnBGImg ��ť���ּ���ť���� �ṹ��
		virtual	void GetU7TBtnInfo(U7TBAR_BTN_INFO& sBtnInfo) = 0;

		virtual void NotifyShowWnd() = 0;
		virtual void NotifyHideWnd() = 0;
	};
}
