// UCC7-MM服务器录播功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7_MM_SvrRec_Observer
	{
	public:
		virtual ~IPlugin_U7_MM_SvrRec_Observer(){};

	public:
		/************************************************************************
		* ERRCODE 说明：
		*  宏                           值                       说明
		* ERR_REC_SVRREC_BUSY			    70001					录播服务器忙，或没有可用的录播服务器
		* ERR_REC_SVR_EXCEPTION				70002					录播服务器异常
		* REC_START_SUCCESS					0						开始录像
		* REC_START_RECMAXCNT				1						达到授权数
		* REC_STREAM_FAILED					2						接流失败
		* REC_START_EXIST					3						已经在录
		* REC_START_DISKFULL				4						磁盘空间不足
		* REC_START_NODISE					5						没有磁盘
		* REC_START_OTHER					6						其他错误
		* 如果有更新请参考HPRecordDefine.h -> RECERR_CODE定义
		************************************************************************/
		virtual void OnRecServerStatus(const char* cszMemberID, int nCardIndex, int nErrorCode){};
		virtual void OnRecServerConfStatus(const char* cszRoomID, int nErrorCode){};
		virtual void OnRecServerConfAVStatus(const char* cszMemberID, int nCardIndex, int nErrorCode){};
		virtual void OnRecServerConfFull(){};
	};

	// UCC7-MM服务器录播功能实现插件
	class IPlugin_U7_MM_SvrRec : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_MM_SvrRec(){};

	public:
		virtual void AddObserver(IPlugin_U7_MM_SvrRec_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_MM_SvrRec_Observer* pObserver) = 0;

		virtual bool StartChnRec(const char* cszMemberID, int nCardIndex,bool bIsLiveStream = false/*是否为直播*/) = 0;
		virtual bool StopChnRec(const char* cszMemberID, int nCardIndex) = 0;
		virtual bool StopAllRec(void) = 0;
		virtual bool StartConfRec(const std::string& strRecFilename = "") = 0;
		virtual bool StopConfRec(void) = 0;
		virtual bool StartConfAVRec(const char* cszMemberID, int nCardIndex) = 0;	
		virtual bool StopConfAVRec(const char* cszMemberID, int nCardIndex) = 0;
		virtual void SaveConfStream(UINT uFrameType, UINT uKeyFrame, unsigned char* outPacket, int nLen, const std::string& strMemberID = "") = 0;
		virtual void StartConfDosRec(const std::string& strMemberID) = 0;
		virtual void StopConfDosRec(const std::string& strMemberID) = 0;
		virtual void SendPicToRecServer(const std::string& strRoomID, const std::string& strMemberID, const std::string& strChnID, const std::string& strFile, bool bConfRecord) = 0;
		virtual void ChangeStreamType(const std::string& strMemberID, int nCardIndex, unsigned short uStreamType = 0) = 0;
		virtual bool IsConfRecording(void) = 0;
	};
}
