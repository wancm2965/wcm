// ���ϵͳӦ�ó�����������ò��������һ��ִ��
// �����ĸ������������������ļ�ȷ����

#pragma once

#include "IPlugin.h"

namespace AVCONPlugin
{
	// �����Ϣ
	// �������ʼ����Ϣ
	struct I_App_Init
	{};

	// ������ر���Ϣ
	struct I_App_Close
	{};

	// Ӧ�ó��������
	class IApp : public IPlugin
	{
	public:
		virtual ~IApp(){};

	public:
		// ִ�нӿ�
		virtual bool Init() = 0;
		virtual bool Run() = 0;        // to do   ��������в���
		virtual bool Close() = 0;
	};
}
