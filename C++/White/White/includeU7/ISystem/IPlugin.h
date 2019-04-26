#ifndef PL_IPLUGIN_H_8998233424234232323
#define PL_IPLUGIN_H_8998233424234232323

// �������
#include <string>

namespace AVCONPlugin
{
	class IPlugin
	{
	public:
		virtual ~IPlugin(){};

	public:
		// ��ȡ�������
		virtual std::string Name() = 0;
		// ��ȡ������ܵ�
		virtual std::string ExtendPoint() = 0;
		// ��ʼ�����
		virtual bool InitPlugin() = 0;
		// ����˳�����
		virtual bool ClosePlugin() = 0;
	};
}

#endif