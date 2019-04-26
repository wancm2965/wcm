#pragma once
#ifndef _IPLUGIN_WB_FRAMEDLG_H__
#define _IPLUGIN_WB_FRAMEDLG_H__
#include "ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_WB_FrameDlg_Observer
	{
	public:
		virtual ~IPlugin_WB_FrameDlg_Observer(){};
	public:
		virtual void ProcessWBToLocalData( char* pNotifyData ) = 0;//接收电子白板内部发出来的信令
	};
	
	class IPlugin_WB_FrameDlg: public IPlugin
	{
	public:
		virtual ~IPlugin_WB_FrameDlg(){};
	public:
		virtual void AddObserver(IPlugin_WB_FrameDlg_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_WB_FrameDlg_Observer* pObserver) = 0;
		virtual HWND GetHwnd() = 0;
		virtual HWND CreateWhiteBoard() = 0;
		virtual void DestroyWhiteBoard() = 0;
		virtual bool Connect() = 0;
		virtual void DisConnect() = 0;
		virtual void RequestAllObj() = 0;
		virtual BOOL SetWindowPos(HWND hWndInsertAfter,int X,int Y,int cx,int cy,UINT uFlags) = 0;
		virtual void GetUserID(std::string &strUser) = 0;
		virtual void SetLocalUserName(CString str) = 0;
		virtual void SetWBAVLock(bool block) = 0;
		virtual void OpenResource() = 0;
	};
}

#endif
