//������Ա�б���
// 
#pragma once
#include "ISystem/IPlugin.h"
namespace AVCONPlugin
{
	// UCC7-T������
	class IPlugin_Meeting_Member : public IPlugin
	{
	public:
		virtual ~IPlugin_Meeting_Member(){};
	public:
		virtual void ShowUI(bool bShow) = 0;
		virtual void ResleseResource() = 0; //�ͷ���Դ
		virtual void DestroyUI() = 0;
		virtual void ProcessTipMessage() =0;
	};
}
