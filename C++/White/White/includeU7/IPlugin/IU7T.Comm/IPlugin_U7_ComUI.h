#ifndef _INTERFACE_PLUGIN_U7_COMUI_H_
#define _INTERFACE_PLUGIN_U7_COMUI_H_

#include <ISystem/IPlugin.h>
#include "IPlugin/IU7T.Comm/DisplayDevices.h"
#include <CoUI/Core/UIManager.h>

struct TUISetting
{
	float	fUIScale;	// �������ű���
	int		nFontSize;	// �����С
	float	fBtnScale;	// ��ť���ű���
	int		nAlpha;		// ͸��������

	TUISetting()
	{
		fUIScale	= 1.0;
		nFontSize	= 19;
		fBtnScale	= 1.0;
		nAlpha		= 255;
	}
};

// ͨ�ý������ýӿ�
namespace AVCONPlugin
{
	///�۲���ģʽ[��ʱ�ֱ���Ϊ����-����<Publish/Subscribe>ģʽ,Դ-������<Source/Listener>ģʽ]
	///������Ĵ���Ҫ��Ӧ��������ֵ�Ķ�̬�ı䣬��̳д˽ӿ�(IPlugin_U7_ComUI_Observer)
	class IPlugin_U7_ComUI_Observer
	{
	public:
		virtual ~IPlugin_U7_ComUI_Observer(void){};
	public:
		///��ʱ�������ߵ������ǲ���̬�ı�ġ��˽ӿ���û��ʵ���Ե���;
		virtual void SetFont(const TFontInfo* pFontInfo) = 0;
		///�ı䴰�ڵ�����ֵ(͸����)
		virtual void SetTransparent(int nOpacity)=0;
		///�ı䴰�ڵ�����ֵ(�����С����)
		virtual void SetUISize(float fScale) = 0;
		///�ı䴰�ڵ�����ֵ(��ť��С����)
		virtual void SetButtonSize(float fScale) = 0;
		///�ı䴰�ڵ�����ֵ(�����С)
		virtual void SetFontSize(int nSize) = 0;
		virtual HWND GetHwnd() {return NULL;}
		virtual void LayoutChangedMainChannel(int nScreen){};
		virtual void LayoutChangedConsoleWnd(int nScreen)
		{
			HWND hWnd = GetHwnd();
			if (NULL != GetHwnd())
			{
				DISP_MONITOR dispDev;
				bool bSucess = GetScreenInfo(dispDev, nScreen-1);
				if (!bSucess)
					return;

				RECT rc;
				::GetWindowRect(hWnd, &rc);


				MONITORINFO oMonitor = {};
				oMonitor.cbSize = sizeof(oMonitor);	
				::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
				UICRect rcWnd = oMonitor.rcWork;

				RECT rcTemp = { 0 };
				rcTemp.left = dispDev.dmPosition.x + rc.left;
				rcTemp.top	= dispDev.dmPosition.y + rc.top;

				::SetWindowPos(hWnd, NULL, dispDev.dmPosition.x + (rc.left-rcWnd.left), dispDev.dmPosition.y + (rc.top - rcWnd.top), 0/*rc.right - rc.left*/, 0/*rc.bottom - rc.top*/,SWP_NOSIZE|SWP_NOACTIVATE);


				::GetWindowRect(hWnd, &rc);

				if (0 != rc.top)
				{
					MONITORINFO oMonitor = {};
					oMonitor.cbSize = sizeof(oMonitor);	
					UICRect dispDevRect;
					::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
					if (MONITORINFOF_PRIMARY == oMonitor.dwFlags)
					{
						SystemParametersInfo(SPI_GETWORKAREA, 0, (LPVOID)&dispDevRect, 0);
					}
					else
					{
						dispDevRect = oMonitor.rcWork;
					}

					::GetWindowRect(hWnd, &rc);
					int cx = rc.right - rc.left;
					int cy = rc.bottom - rc.top;

					if (dispDevRect.bottom - rc.bottom<140)
					{
						int nBottomDis = 140 - (dispDevRect.bottom - rc.bottom);
						rc.top = rc.top - nBottomDis;
						if (dispDevRect.top - rc.top >0)
						{
							rc.top = dispDevRect.top;
						}
					}
					::SetWindowPos(hWnd, NULL, rc.left, rc.top, cx, cy, SWP_NOSIZE|SWP_NOACTIVATE);
				}
			}	
		};
		virtual void LayoutChangedLocacPreview(int nScreen){};
	};
	class IPlugin_U7_ComUI : virtual public IPlugin
	{
	public:
		///���"��Ӧ����ֵ��̬�ı�"��"������"
		virtual void AddObserver(IPlugin_U7_ComUI_Observer* pObserver)=0;
		///�Ƴ�"��Ӧ����ֵ��̬�ı�"��"������"
		virtual void DelObserver(IPlugin_U7_ComUI_Observer* pObserver)=0;

		///��ȡȫ��Ĭ������
		virtual const TFontInfo* GetDefaultFont(void)const=0;

		///��ȡȫ�ִ���(��)����
		///����Ϊtrue[Ĭ��]�����ȡ���Ǵ��м�Ȩֵ�ġ����򣬻�ȡ���ǲ�����Ȩֵ����ֵ
		virtual TUISetting GetUISetting(bool bHasWeight=true)const=0;

		// �ж�ϵͳ�Ƿ�֧��͸��
		virtual bool BTransparentSupport() = 0;

		// ��ȡ��ǰ��ʹ�õ�Ƥ��·��
		virtual CString GetSkinFolder() = 0;
	};
	class IPlugin_U7_ComUI_Control : virtual public IPlugin
	{
	public:
		///�������壬����Ӧ�ô��ڵ��ô˽ӿڽ������ã�����������ͨ����ȡĬ�������Ա���һ����
		virtual void SetDefaultFont(const TFontInfo* pFontInfo)=0;
	};
}//end namespace
#endif