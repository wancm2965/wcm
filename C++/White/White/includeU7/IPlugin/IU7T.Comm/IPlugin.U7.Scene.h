// UCC7-T场景
// 
#pragma once

namespace AVCONPlugin
{
	// 场景状态，场景模式
	enum EMetting_State{
		Scene_State_Main = 8900,	// 主场景，赋予一个较大的值，以尽量减少由于参数传递带来的勿使用。
		Scene_State_Metting,		// 会议
		Scene_State_Channel,		// 频道
		Scene_State_Calling_H323,	// H323呼叫
		Scene_State_Calling_SIP,	// SIP呼叫
		Scene_State_Metting_AVCON,	// AVCON呼叫
	};

	// 应该只有主插件才能继承此接口
	class IPlugin_U7_Scene_Manager
	{
	public:
		// 返回到主场景(如请出、各个场景的主动退出等)
		virtual bool BackToMainScene(unsigned int nSceneMark) = 0;
		// 设置当前场景(场景：主场景，会议、频道、呼叫)
		virtual void SetCurSceneMark(const unsigned int nSceneMark) = 0;
		// 获取当前场景
		virtual const unsigned int GetCurSceneMark(void) const = 0;
	};

	// 各场景主插件继承此接口（做一些可能的善后工作）
	class IPlugin_U7_Scene_Observe
	{
	public:
		// 退出场景
		virtual void ExitScene(void) = 0;
		// 进入场景
		virtual void EnterScene(void) = 0;
	};
}
