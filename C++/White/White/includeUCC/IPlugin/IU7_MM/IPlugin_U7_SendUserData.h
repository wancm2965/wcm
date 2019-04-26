// UCC7-MM��Ա��ع���ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	// UCC7-MM��Ա��ع���ʵ�ֲ��
	class IPlugin_U7_SendUserData : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_SendUserData(){};

	public:
		/*CODE_STREAM	 ��������
		CODE_FRAME		 ����֡��
		DECOD_STREAM	 ��������
		DECOD_FRAME		 ����֡��
		DECOD_RESOLUTION ����ֱ���*/
		virtual void SendMMUserData(std::string strMemberID, std::string strFromID, std::string strUERCMD, int nLevel) = 0;

		virtual void SendMMUserData(std::string strMemberID, std::string strFromID, std::string strUERCMD, int nTimerID, int nClose) = 0;
	};
}
