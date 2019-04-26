// UCC7-MM人员相关功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	// UCC7-MM人员相关功能实现插件
	class IPlugin_U7_SendUserData : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_SendUserData(){};

	public:
		/*CODE_STREAM	 编码码流
		CODE_FRAME		 编码帧率
		DECOD_STREAM	 解码码流
		DECOD_FRAME		 解码帧率
		DECOD_RESOLUTION 解码分辨率*/
		virtual void SendMMUserData(std::string strMemberID, std::string strFromID, std::string strUERCMD, int nLevel) = 0;

		virtual void SendMMUserData(std::string strMemberID, std::string strFromID, std::string strUERCMD, int nTimerID, int nClose) = 0;
	};
}
