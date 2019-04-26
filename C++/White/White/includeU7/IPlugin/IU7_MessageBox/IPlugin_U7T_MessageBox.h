#pragma once
#include "ISystem/ISystem.h"///不该放在这里的。

namespace AVCONPlugin
{
	class IPlugin_U7T_MessageBox : public IPlugin
	{
	public:
		///弹出一个相对模态窗体
		// 参数bAutoDestroy: 为true则在失去焦点时销毁，否则正常销毁
		virtual int MessageBox(HWND hOwner,CString strTitle,CString strContent,UINT nStye, bool bAutoDestroy = false) = 0;
		///关闭所有弹出式的窗体
		virtual void CloseAllMessageBox() = 0;
	};
}