#ifndef _INTERFACE_PLUGIN_U7_COMUI_H_
#define _INTERFACE_PLUGIN_U7_COMUI_H_

#include <ISystem/IPlugin.h>
#include "IPlugin/IU7T.Comm/DisplayDevices.h"
#include <CoUI/Core/UIManager.h>

struct TUISetting
{
	float	fUIScale;	// 界面缩放比例
	int		nFontSize;	// 字体大小
	float	fBtnScale;	// 按钮缩放比例
	int		nAlpha;		// 透明度设置

	TUISetting()
	{
		fUIScale	= 1.0;
		nFontSize	= 19;
		fBtnScale	= 1.0;
		nAlpha		= 255;
	}
};

// 通用界面设置接口
namespace AVCONPlugin
{
	///观察者模式[有时又被称为发布-订阅<Publish/Subscribe>模式,源-收听者<Source/Listener>模式]
	///如果您的窗口要响应如下属性值的动态改变，则继承此接口(IPlugin_U7_ComUI_Observer)
	class IPlugin_U7_ComUI_Observer
	{
	public:
		virtual ~IPlugin_U7_ComUI_Observer(void){};
	public:
		///暂时各收听者的字体是不动态改变的。此接口暂没有实质性的用途
		virtual void SetFont(const TFontInfo* pFontInfo) = 0;
		///改变窗口的属性值(透明度)
		virtual void SetTransparent(int nOpacity)=0;
		///改变窗口的属性值(窗体大小比例)
		virtual void SetUISize(float fScale) = 0;
		///改变窗口的属性值(按钮大小比例)
		virtual void SetButtonSize(float fScale) = 0;
		///改变窗口的属性值(字体大小)
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
		///添加"响应属性值动态改变"的"收听者"
		virtual void AddObserver(IPlugin_U7_ComUI_Observer* pObserver)=0;
		///移除"响应属性值动态改变"的"收听者"
		virtual void DelObserver(IPlugin_U7_ComUI_Observer* pObserver)=0;

		///获取全局默认字体
		virtual const TFontInfo* GetDefaultFont(void)const=0;

		///获取全局窗口(四)属性
		///参数为true[默认]，则获取的是带有加权值的。否则，获取的是不带加权值得数值
		virtual TUISetting GetUISetting(bool bHasWeight=true)const=0;

		// 判断系统是否支持透明
		virtual bool BTransparentSupport() = 0;

		// 获取当前的使用的皮肤路径
		virtual CString GetSkinFolder() = 0;
	};
	class IPlugin_U7_ComUI_Control : virtual public IPlugin
	{
	public:
		///设置字体，由主应用窗口调用此接口进行设置，后续各窗口通过获取默认字体以保持一致性
		virtual void SetDefaultFont(const TFontInfo* pFontInfo)=0;
	};
}//end namespace
#endif