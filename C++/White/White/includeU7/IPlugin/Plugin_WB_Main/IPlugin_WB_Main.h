#pragma once
#include "../../ISystem/IApp.h"
namespace AVCONPlugin
{
	class IPlugin_WB_Main : public IApp
	{
	public:
		virtual ~IPlugin_WB_Main(){};
		virtual void Exit(void) = 0;
		virtual HWND GetMainWnd() = 0;
		virtual HWND GetExeParentWnd()= 0;
	};
}