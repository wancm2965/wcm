// UCC7-T����
// 
#pragma once

namespace AVCONPlugin
{
	// ����״̬������ģʽ
	enum EMetting_State{
		Scene_State_Main = 8900,	// ������������һ���ϴ��ֵ���Ծ����������ڲ������ݴ�������ʹ�á�
		Scene_State_Metting,		// ����
		Scene_State_Channel,		// Ƶ��
		Scene_State_Calling_H323,	// H323����
		Scene_State_Calling_SIP,	// SIP����
		Scene_State_Metting_AVCON,	// AVCON����
	};

	// Ӧ��ֻ����������ܼ̳д˽ӿ�
	class IPlugin_U7_Scene_Manager
	{
	public:
		// ���ص�������(����������������������˳���)
		virtual bool BackToMainScene(unsigned int nSceneMark) = 0;
		// ���õ�ǰ����(�����������������顢Ƶ��������)
		virtual void SetCurSceneMark(const unsigned int nSceneMark) = 0;
		// ��ȡ��ǰ����
		virtual const unsigned int GetCurSceneMark(void) const = 0;
	};

	// ������������̳д˽ӿڣ���һЩ���ܵ��ƺ�����
	class IPlugin_U7_Scene_Observe
	{
	public:
		// �˳�����
		virtual void ExitScene(void) = 0;
		// ���볡��
		virtual void EnterScene(void) = 0;
	};
}
