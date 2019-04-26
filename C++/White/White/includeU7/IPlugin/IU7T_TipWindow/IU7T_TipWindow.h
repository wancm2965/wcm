#pragma once

#include "../../ISystem/IPlugin.h"

#define PLUGIN_NAME_U7T_TIPWINDOW			"Plugin_U7T_TipWindow"

namespace AVCONPlugin
{
	// ÆÕÍ¨²å¼þPlugin_U7T_ShowUserInfo
	class IPlugin_U7T_TipWindow : public IPlugin
	{
	public:
		virtual ~IPlugin_U7T_TipWindow(){};

	public:
		virtual void ShowTipWindow(CString strTip, int nSeconds = 3, HWND hwndParent = NULL) = 0;
		virtual void DestroyAllTip(void) = 0;
	};
}
