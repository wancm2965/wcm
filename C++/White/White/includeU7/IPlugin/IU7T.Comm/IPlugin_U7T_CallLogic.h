#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	// UCC7-T呼叫逻辑通用接口
	class IPlugin_U7T_CallLogic : virtual public IPlugin
	{
	public:
		virtual void InitCallLogic(void) = 0;		// 初始化呼叫逻辑
		virtual void UnInitCallLogic(void) = 0;	// 反初始化呼叫逻辑

		// 处理广播逻辑，在收到广播命令后，接像到模板前调用该方法
		virtual bool DealBroadcast(int& nScreenID, int& nWindowID, const std::string& strMemberID, int nCardIndex, int nChannelType) = 0;
		// 处理关闭广播逻辑，在收到关闭广播命令，处理完关闭窗口后调用该方法
		virtual bool DealCloseBroadcast(const std::string& strMemberID, int nCardIndex, int nChannelType) = 0;

		virtual int GetDigitalCard() = 0;
		virtual int GetTMMaxCount(void) = 0;
		virtual int GetMaxCount(void) = 0;	// 获取最大人数，超过时将转为会议模式
		virtual int GetAutoBroadcastCount(void) = 0;
		virtual bool IsAllowChangeMeeting(void) {return true;};
	};
}
