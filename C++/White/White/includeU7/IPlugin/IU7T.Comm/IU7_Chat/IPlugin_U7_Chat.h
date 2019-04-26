// UCC7-TChat���
// 
#pragma once

#include "IPlugin/IU7Meeting_Comm/IPlugin_U7Meeting_Comm.h"

namespace AVCONPlugin
{
	// UCC7-T������
	class IPlugin_U7_Chat : public IPlugin_U7Meeting_Comm
	{
	public:
		virtual ~IPlugin_U7_Chat(){};

	public:
		virtual void ShowMainUI(bool bShow) = 0;
		virtual HWND CreateChat() = 0;
		virtual void DestroyChat() = 0;
	};
}